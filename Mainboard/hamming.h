#ifndef _HAMMING_H_
#define _HAMMING_H_

/*
 * AUTHOR: Alexandru-Dragos BUCUR
 *
 * Hamming code was chosen because it is easy to implement and is good enough
 * for the expected error rates.
 *
 * The *maximum* error rate you can set on the RXSM module is 2^-4, so 1 bit
 * error every 2 bytes. So a hamming code of at least (15,11) should be chosen
 * to cover these errors. A (15,11) Hamming Code encodes 11 bits of useful data
 * in 15 bits which will be transmitted, being able to fix 1 bit error within
 * the transmitted 15 bits.
 *
 * I chose (8,4) because it would be more reliable, and because we can
 * comfortably afford the 2x overhead. (8,4) Hamming codes can fix 1 bit error
 * every byte, for every 4 bits of useful data 8 bits are transmitted.
 *
 * NOTE: Why (8,4) instead of (7,4)?
 *
 * A (8,4) has more robust error *detection* compared to (7,4). However, both
 * (8,4) and (7,4) can reliably correct only 1 bit error, so using an additional
 * bit does not really improve anything for our case, because we will always
 * attempt to correct the message.
 *
 * The reason we use (8,4) is to handle *byte drops*. If we were to use (7,4)
 * the following could happen (note that the actual encoding of the parity bits
 * is not correct, and only for demonstration purposes):
 *
 * (Transmitted bits on top, amount of encoded useful data on bottom)
 *
 * 00000001 11111100 00000111 1111
 * ------||------||------||------|
 * 4 bits  4 bits  4 bits  4 bits
 * |-------------||--------------|
 *   1 data byte    1 data byte
 *
 * In the above example, if the RXSM module were to drop the 2nd byte
 * (11111100), we would lose at least 2 bytes of data (potentially more
 * depending on how the decoder is implemented), since the 2nd transmitted byte
 * contains decoding information for both the 1st and 2nd data byte. is a lot
 * less tedious. Theoretically, this could also corrupt 2 whole packets, since
 * the affected 2 bytes could be the final and first byte of 2 different
 * packets.
 *
 * *Most importantly*, this would have an cascading effect, where all of the
 * following encoded bits would be shifted, so the decoder would need to be
 * implemented as a sliding window over the received *bits*, in order to
 * eventually correct itself and not corrupt future correct packets.
 *
 * In comparison, a (8,4) code would do the following:
 *
 * 00000000 11111111 00000000 11111111
 * |------| |------| |------| |------|
 *  4 bits   4 bits   4 bits   4 bits
 * |---------------| |---------------|
 *    1 data byte       1 data byte
 *
 * Now, if we were to lose any transmitted byte, we would lose information about
 * a single data byte, which would corrupt only that specific packet and would
 * not cause a shift in the bits of the transmitted code. In this scenario, we
 * can implement the decoder as a sliding window over the received *bytes* while
 * still being able to skip over corrupt packets without influencing future
 * packets.
 *
 * This makes everything a lot less tedious and less error prone, so I decided
 * that the additional 1 bit is worth it.
 */

typedef struct {
    uint8_t *buf;
    uint32_t used;
    uint32_t allocated;
} buffer_hamming;

#define GET_BIT(byte, bit) (((byte) & (1 << (bit))) >> (bit))

uint8_t get_1_count(uint8_t byte) {
    uint8_t count = 0;

    while (byte) {
        count += byte & 1;
        byte >>= 1;
    }

    return count;
}

/*
 * TODO: Pretty ugly interface, it should have taken just the data and
 * simply return `encoded_bytes`, but i do not want to refactor the tests rn.
 * Also the implementation is pretty messy (but still correct).
 *
 * So these is the fundamental building block for the Hamming(8,4) code.
 * It takes 4 bits of data and transforms it into an encoded byte.
 *
 * The bit positions of `data` are as follow (bit position on top, bit label on
 * bottom):
 *
 * 011 010 001 000
 *  a   b   c   d
 *
 * So, if `data` is 0b1001, then:
 * a = 1
 * b = 0
 * c = 0
 * d = 1
 *
 * The bits of `encoded_byte` will be arranged as follow:
 *
 * 111 110 101 100 011 010 001 000
 *  a   b   c   p3  d   p2  p1  p0
 *
 *  p3, p2, p1, and p0 are parity bits
 *
 *  p0 - is the additional detection bit specified by (8,4), which we do not
 * really use
 *
 * p1, p2, p3 are pretty standard:
 * https://en.wikipedia.org/wiki/Hamming_code#General_algorithm
 *
 * Other useful links (3Blue1Brown):
 * https://youtu.be/X8jsijhllIA?si=CwlFf3mln5EN35RM (Theory: 20 mins)
 * https://youtu.be/b3NxrZOu_CE?si=bHiTZJVZoncFXPZq (Implementation: 16 mins)
 */
void encode_4_bits(buffer_hamming *buf_out, uint8_t data) {
//    assert(data <= 0b1111);

    uint8_t bits[4];
    for (int i = 3; i >= 0; i--) {
        bits[3 - i] = GET_BIT(data, i);
    }

    const uint8_t positions[4] = {0b111, 0b110, 0b101, 0b011};

    uint8_t parity = 0;
    for (int j = 0; j < 4; j++) {
        if (bits[j]) {
            parity ^= positions[j];
        }
    }

    uint8_t parity_bits[4] = {0};
    for (int j = 1; j < 4; j++) {
        parity_bits[j] = GET_BIT(parity, j - 1);
    }

    // encoding: a b c p3 d p2 p1 p0
    // p0 is global parity bit
    char encoded_byte = (bits[0] << 7) | (bits[1] << 6) | (bits[2] << 5) |
                        (parity_bits[3] << 4) | (bits[3] << 3) |
                        (parity_bits[2] << 2) | (parity_bits[1] << 1);
    encoded_byte |= get_1_count(encoded_byte) % 2;

    buf_out->buf[buf_out->used] = encoded_byte;
    buf_out->used++;
}

// split each data byte in 2 encoded bytes, most significant half first
void encode_hamming(const buffer_hamming *buf_in, buffer_hamming *buf_out) {
    // buf_out already needs to have enough allocated memory
    //assert(buf_in->used <= buf_out->allocated / 2);

    buf_out->used = 0;
    for (uint32_t i = 0; i < buf_in->used; i++) {
        uint8_t currentByte = buf_in->buf[i];

        uint8_t first = (currentByte & 0xF0) >> 4;
        uint8_t second = (currentByte & 0x0F);

        encode_4_bits(buf_out, first);
        encode_4_bits(buf_out, second);
    }
}

/*
 * So the encoded byte is:
 *
 * 111 110 101 100 011 010 001 000
 *  a   b   c   p3  d   p2  p1  p0
 *
 * There are 2 steps:
 *      1. Correct the byte - xoring the positions of all 1 bits will give you
 * the position of the error bit, which will need to be flipped in order to
 * correct the byte. If xoring gives you 0, everything is fine.
 *      2. Get the data - simply select bits a, b, c, d and return them
 */
uint8_t decode_4_bits(uint8_t data) {
    uint8_t parity = 0;

    // we do not take parity bit 0 (p0) into consideration
    // while error checking, that's why
    for (int i = 7; i >= 1; i--) {
        if (GET_BIT(data, i)) {
            parity ^= i;
        }
    }

    // there was an error, xoring the positions of all bits which are 1
    // gives us the position of the error bit
    if (parity != 0) {
        data ^= 1 << parity;
    }

    uint8_t result = (GET_BIT(data, 7) << 3) | (GET_BIT(data, 6) << 2) |
                     (GET_BIT(data, 5) << 1) | GET_BIT(data, 3);
    return result;
}

// Every 2 encoded bytes will contain 1 byte of data.
void decode_hamming(const buffer_hamming *buf_in, buffer_hamming *buf_out) {
    // buf_out already needs to have enough allocated memory
    //assert((buf_in->used + 1) / 2 <= buf_out->allocated);

    buf_out->used = 0;
    for (uint32_t i = 0; i < buf_in->used; i++) {
        uint8_t byte = buf_in->buf[i];
        uint8_t half = decode_4_bits(byte);

        if (i % 2 == 0) {
            buf_out->buf[buf_out->used] = half << 4;
        } else {
            buf_out->buf[buf_out->used] |= half;
            buf_out->used++;
        }
    }
}

#endif
