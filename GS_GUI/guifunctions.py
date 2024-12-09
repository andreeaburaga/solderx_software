'''Functions definitions'''
import tkinter
import serial

ser = serial.Serial('/dev/ttyUSB0', 38400)

def quit():
    global tkTop, ser
    if ser.is_open:
        ser.write(bytes('Q', 'UTF-8'))
        # ser.close()
    # tkTop.destroy()


#Text box for telemetry data
def read_serial_data(text_box=None):
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting).decode('UTF-8')
        text_box.insert(tkinter.END, data)
        text_box.yview(tkinter.END)
    tkTop.after(50, read_serial_data)


#Turn ON camera LEDs
def set_button1_state():
    ser.write(bytes('LL', 'UTF-8'))

#Turn OFF camera LEDs
def set_button2_state():
    ser.write(bytes('ll', 'UTF-8'))

#Turn ON camera 
def set_button3_state():
    ser.write(bytes('CC', 'UTF-8'))

#Turn OFF camera
def set_button4_state():
    ser.write(bytes('cc', 'UTF-8'))

#Turn ON 12VDC/DC converter
def set_button5_state():
    ser.write(bytes('PP', 'UTF-8'))

#Turn OFF 12VDC/DC converter 
def set_button6_state():
    ser.write(bytes('pp', 'UTF-8'))

#Turn ON 10VDC/DC converter
def set_button19_state():
    ser.write(bytes('RR', 'UTF-8'))

#Turn OFF 10VDC/DC converter 
def set_button20_state():
    ser.write(bytes('rr', 'UTF-8'))

# MOTORS
#Rotate Sample disk clockwise
def set_button7_state():
    ser.write(bytes('MM', 'UTF-8'))

#Rotate Sample disk counter-clockwise
def set_button8_state():
    ser.write(bytes('mm', 'UTF-8'))

#Move soldering unit
def set_button9_state():
    ser.write(bytes('ss', 'UTF-8'))

#Move soldering unit
def set_button10_state():
    ser.write(bytes('SS', 'UTF-8'))

#Rotate feeding mechanism stepper clockwise
def set_button11_state():
    ser.write(bytes('DD', 'UTF-8'))

#Rotate feeding mechanism stepper counter-clockwise
def set_button12_state():
    ser.write(bytes('dd', 'UTF-8'))

# HEATING
#Turn heating on
def set_button13_state():
    ser.write(bytes('HH', 'UTF-8'))

# #Maintain temperature
# def set_button14_state():
#     ser.write(bytes('hh', 'UTF-8'))

#Turn heating off
def set_button15_state():
    ser.write(bytes('hh', 'UTF-8'))

# ARMING
#Cold arm experiment
def set_button16_state():
    ser.write(bytes('NN', 'UTF-8'))

#Hot arm experiment
def set_button17_state():
    ser.write(bytes('FF', 'UTF-8'))

#Disarm experiment
def set_button18_state():
    ser.write(bytes('xx', 'UTF-8'))
