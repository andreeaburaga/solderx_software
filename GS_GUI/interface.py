import time
import tkinter
from guilayout import *
import threading
import random 
import os
from enum import Enum
from typing import Tuple

def get_bit(byte, bit):
    return (byte & (1 << bit)) >> bit

def decode_4_bits(byte):
    parity = 0
    for i in range(1, 8):
        if (get_bit(byte, i) == 1):
            parity ^= i
    
    if (parity != 0):
        print("Detected error, trying to correct")
    byte ^= 1 << parity

    result = (get_bit(byte, 7) << 3) | (get_bit(byte, 6) << 2) | (get_bit(byte, 5) << 1) | get_bit(byte, 3)
    return result

class DecodeResult(Enum):
    NO_SYNC = 1
    WRONG_CHECKSUM = 2
    GOOD = 3

def is_valid_msg(buffer: bytearray) -> DecodeResult:
    assert(len(buffer) == 15)

    if (buffer[0] != buffer[1] or buffer[0] != 0b10101010):
        return DecodeResult.NO_SYNC

    checksum = 0
    for i in range(2, 14):
        checksum ^= buffer[i]

    if (checksum != buffer[14]):
        return DecodeResult.WRONG_CHECKSUM

    return DecodeResult.GOOD


def try_decode(buffer: bytearray) -> Tuple[DecodeResult, bytearray]:
    assert(len(buffer) == 30)

    msg = bytearray()

    for i in range(30):
        half = decode_4_bits(buffer[i])
        if i % 2 == 0:
            half <<= 4
            msg.append(half)
        else:
            msg[-1] |= half

    is_valid = is_valid_msg(msg)
    return (is_valid, msg)


def readFromSerial():
    global ser, textbox, since_last, elapsed
    buffer = bytearray()
    saveFile = open("log_data.bin", "wb")
    saveFileText = open("log_data_text.txt", "wt")

    solderingTargetTemperature = 0
    solderingCurrentTemperature = 0
    sampleDiscPosition = 0
    feedingMechanismPosition = 0
    linearMotorPosition = 0
    LO_State = 0
    SOE_State = 0
    stateMachineStatus = 0

    wrong = 0
    last = time.time_ns()
    while True:
        # print("Before")
        if (ser.in_waiting >= 1):
            currentByte = ser.read(1)
            # print("After")
            # print(currentByte)

            buffer.append(currentByte[0])

            if (len(buffer) != 30):
                continue

            (error, msg) = try_decode(buffer)
            if (error == DecodeResult.GOOD):
                last = time.time_ns()
                print("Good Message")

                saveFile.write(msg)
                saveFileText.write("New Message:")

                solderingTargetTemperature = int.from_bytes(msg[2:4], byteorder='little')
                saveFileText.write(f"solderingTargetTemperature:{solderingTargetTemperature},")

                solderingCurrentTemperature = int.from_bytes(msg[4:6], byteorder='little')
                saveFileText.write(f"solderingCurrentTemperature:{solderingCurrentTemperature},")

                sampleDiscPosition = int.from_bytes(msg[6:8], byteorder='little')
                saveFileText.write(f"sampleDiscPosition:{sampleDiscPosition},")
                
                feedingMechanismPosition = int.from_bytes(msg[8:10], byteorder='little')
                saveFileText.write(f"feedingMechanismPosition:{feedingMechanismPosition},")

                linearMotorPosition = int.from_bytes(msg[10:12],byteorder='little')
                saveFileText.write(f"linearMotorPosition:{linearMotorPosition},")
                
                signalsReceived = int.from_bytes(msg[12:13], byteorder='little')
                LO_State = signalsReceived & 1
                saveFileText.write(f"LO_State:{LO_State},")
                SOE_State = (signalsReceived >> 1) & 1
                saveFileText.write(f"SOE_State:{SOE_State},")

                stateMachineStatus = int.from_bytes(msg[13:14], byteorder='little')
                saveFileText.write(f"stateMachineStatus:{stateMachineStatus},")

                buffer = bytearray()
            else:
                buffer.pop(0)
                if (error == DecodeResult.WRONG_CHECKSUM):
                    wrong += 1
                    print("Found a corrupted message")
                else:
                    # this is expected to appear a bunch of times in short bursts (like ~30 at a time)
                    # this is because once 1 byte within a packet is corrupted which would cause either
                    # the sync check or the checksum to fail, we can expect the following attempts to decode
                    # a packet to also fail. TODO: write a better explanation
                    print("Not Sync")

        elapsed = time.time_ns() - last


        ## TEST - nu sterge, comenteaza        
        # solderingTargetTemperature = 250
        # solderingCurrentTemperature = 200 + int(random.random()*5)
        # sampleDiscPosition = 10
        # feedingMechanismPosition = 3
        # linearMotorPosition = 4
        # signalsReceived = 5
        # stateMachineStatus = 6
        # checksum = 0
        # zeroEndedString = 0
        # LO_State = 1
        # SOE_State = 0
        # END TEST

        # textul asta trebuie sa fie fara indenturi, altfel apar si in textbox
        textbox["text"] = ''' 
Time: {} s
Soldering Target Temperature: {} °C
Soldering Current Temperature: {} °C
Sample Disc Position: {}
Feeding Mechanism Position: {}
Linear Motor Position: {}
LO_Pin State: {}
SOE_Pin State: {}
State Machine Status: {}
'''.format(int((time.time() - starttime) * 1000)/1000, solderingTargetTemperature, solderingCurrentTemperature, sampleDiscPosition, 
                feedingMechanismPosition, linearMotorPosition, 'HIGH' if LO_State else 'LOW',
                'HIGH' if SOE_State else 'LOW', stateMachineStatus)

        elapsed_ms = round(elapsed/(10**6))
        elapsed_text = 'Time since last packet: {:5}ms'.format(elapsed_ms)
        if (elapsed_ms > 600):
            elapsed_text = "WARN: " + elapsed_text
        else:
            elapsed_text = "      " + elapsed_text

        since_last.config(text=elapsed_text)
        no_errors["text"] = 'Errors: {}'.format(wrong)

# label9 = 0

def inctimer():
    global starttime, label9
    try:
        label9.config(text='t={0:.3f}s'.format(time.time() - starttime))
    except:
        pass
    
    while True:
        time.sleep(.01)
        try:
            label9.config(text='t={0:.3f}s'.format(time.time() - starttime))
        except:
            pass
    # p = threading.Timer(.01, inctimer)
    # p.start()
    # p.join()


if __name__ == '__main__':
    print("Reset Arduino2")
    time.sleep(3)
    # ser.write(bytes('R', 'UTF-8'))
    starttime = time.time()

    tkTop = tkinter.Tk()

    label9, textbox, since_last, no_errors = makelayout(tkTop)

    t1 = threading.Thread(target=inctimer, daemon=True)
    t2 = threading.Thread(target=readFromSerial, daemon=True)

    t1.start()
    t2.start()
    tkTop.mainloop()

    t2.join()
    t1.join()

'''
    t1 = threading.Thread(target=tkTop.mainloop)
    t2 = threading.Thread(target=readFromSerial)

    t1.start()
    t2.start()

    t1.join()
    t2.join()
'''