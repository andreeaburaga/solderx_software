import tkinter
from guifunctions import *

BUTTON_SIZE = 0

def makelayout(tkTop):
    tkTop.geometry('1200x800')
    tkTop.title("Ground Station SOLDERX")
    label1 = tkinter.Label(tkTop, text='Commands', font=("Courier", 14,'bold'))
    label1.grid(row=0, column=1, padx=10, pady=10)

    label2 = tkinter.Label(tkTop, text='Turn LEDs',font=("Courier", 12,'bold'))
    label2.grid(row=1, column=0, padx=10, pady=10)

    label7 = tkinter.Label(tkTop, text='Turn Camera',font=("Courier", 12,'bold'))
    label7.grid(row=2, column=0, padx=10, pady=10)

    # label8 = tkinter.Label(tkTop, text='Turn Optical Encoder',font=("Courier", 12,'bold'))
    # label8.grid(row=3, column=0, padx=10, pady=10)

    label3 = tkinter.Label(tkTop, text='Rotate Sample Disk',font=("Courier", 12,'bold'))
    label3.grid(row=4, column=0, padx=10, pady=10)

    label4 = tkinter.Label(tkTop, text='Move Soldering Unit',font=("Courier", 12,'bold'))
    label4.grid(row=5, column=0, padx=10, pady=10)

    label5 = tkinter.Label(tkTop, text='Rotate Feeding Mechanism',font=("Courier", 12,'bold'))
    label5.grid(row=6, column=0, padx=10, pady=10)

    label6 = tkinter.Label(tkTop, text='Heating Control',font=("Courier", 12,'bold'))
    label6.grid(row=7, column=0, padx=10, pady=10)

    label9 = tkinter.Label(tkTop, text='t=0',font=("Courier", 12,'bold'))
    label9.grid(row=8, column=0, padx=10, pady=10)

    #LEDs on
    button1state = tkinter.Button(tkTop,
        text="ON",
        command=set_button1_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='green'
    )
    button1state.grid(row=1, column=1, padx=10, pady=15, sticky='nsew')

    #LEDs off
    button2state = tkinter.Button(tkTop,
        text="OFF",
        command=set_button2_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button2state.grid(row=1, column=2, padx=10, pady=15, sticky='nsew')

    #Camera on
    button3state = tkinter.Button(tkTop,
        text="ON",
        command=set_button3_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='green'
    )
    button3state.grid(row=2, column=1, padx=10, pady=15, sticky='nsew')

    #Camera off
    button4state = tkinter.Button(tkTop,
        text="OFF",
        command=set_button4_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button4state.grid(row=2, column=2, padx=10, pady=15, sticky='nsew')

    # #Optical encoder on
    # button5state = tkinter.Button(tkTop,
    #     text="ON",
    #     command=set_button5_state,
    #     height=BUTTON_SIZE,
    #     fg="black",
    #     bd=3,
    #     activebackground='green'
    # )
    # button5state.grid(row=3, column=1, padx=10, pady=15, sticky='nsew')

    # #Optical encoder off
    # button6state = tkinter.Button(tkTop,
    #     text="OFF",
    #     command=set_button6_state,
    #     height=BUTTON_SIZE,
    #     fg="black",
    #     bd=3,
    #     activebackground='red'
    # )
    # button6state.grid(row=3, column=2, padx=10, pady=15, sticky='nsew')

    #Sample disk
    button7state = tkinter.Button(tkTop,
        text="Clockwise",
        command=set_button7_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='green'
    )
    button7state.grid(row=4, column=1, padx=10, pady=15, sticky='nsew')

    #Sample disk
    button8state = tkinter.Button(tkTop,
        text="Counter-Clockwise",
        command=set_button8_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button8state.grid(row=4, column=2, padx=10, pady=15, sticky='nsew')

    #Linear motor
    button9state = tkinter.Button(tkTop,
        text="Forward",
        command=set_button9_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='green'
    )
    button9state.grid(row=5, column=1, padx=10, pady=15, sticky='nsew')

    #Linear motor
    button10state = tkinter.Button(tkTop,
        text="Backward",
        command=set_button10_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button10state.grid(row=5, column=2, padx=10, pady=15, sticky='nsew')


    '''Buttons for feeding mechanism'''
    button11state = tkinter.Button(tkTop,
        text="Clockwise",
        command=set_button11_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button11state.grid(row=6, column=1, padx=10, pady=15, sticky='nsew')

    button12state = tkinter.Button(tkTop,
        text="Counter-Clockwise",
        command=set_button12_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button12state.grid(row=6, column=2, padx=10, pady=15, sticky='nsew')

    ''''''
    '''Heating control'''
    button13state = tkinter.Button(tkTop,
        text="ON",
        command=set_button13_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button13state.grid(row=7, column=1, padx=10, pady=15, sticky='nsew')

    button14state = tkinter.Button(tkTop,
        text="Keep constant",
        command=set_button14_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button14state.grid(row=7, column=3, padx=10, pady=15, sticky='nsew')

    button15state = tkinter.Button(tkTop,
        text="OFF",
        command=set_button15_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button15state.grid(row=7, column=2, padx=10, pady=15, sticky='nsew')

    '''Arming'''

    button16state = tkinter.Button(tkTop,
        text="Arm without heating",
        command=set_button16_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='blue'
    )
    button16state.grid(row=9, column=2, padx=10, pady=15, sticky='nsew')

    button17state = tkinter.Button(tkTop,
        text="Arm with heating",
        command=set_button17_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='red'
    )
    button17state.grid(row=10, column=2, padx=10, pady=15, sticky='nsew')

    button18state = tkinter.Button(tkTop,
        text="Disarm",
        command=set_button18_state,
        height=BUTTON_SIZE,
        fg="black",
        bd=3,
        activebackground='green'
    )
    button18state.grid(row=11, column=2, padx=10, pady=15, sticky='nsew')

    '''TELEMETRY'''

    label5 = tkinter.Label(tkTop, text='Log', font=("Courier", 14,'bold'))
    label5.grid(row=0, column=4, padx=10, pady=10, sticky='ne')

    text_box = tkinter.Label(tkTop, font=("Courier", 14,'bold'), justify='left', background='white') 
    text_box.grid(row=1, column=5, rowspan=12, pady=10, padx=10, sticky='ne')

    since_last = tkinter.Label(tkTop, text='Time since last packet: ',font=("Courier", 12,'bold'))
    since_last.grid(row=9, column=0, padx=10, pady=10)

    no_errors = tkinter.Label(tkTop, text='Erros: ',font=("Courier", 12,'bold'))
    no_errors.grid(row=10, column=0, padx=10, pady=10)

    #Exit application
    tkButtonQuit = tkinter.Button(
        tkTop,
        text="Quit",
        command=quit,
        height=BUTTON_SIZE,
        fg="black",
        bg='yellow',
        bd=3
    )
    tkButtonQuit.grid(row=12, column=0, padx=10, pady=10, sticky='nsew')

    return label9, text_box, since_last, no_errors

'''
#read_serial_data()
'''
