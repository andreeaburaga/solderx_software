#ifndef _HAMMING_H_
#define _HAMMING_H_

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
    // assert((buf_in->used + 1) / 2 <= buf_out->allocated);

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
