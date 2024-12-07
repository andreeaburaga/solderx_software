# solderx_software
test

- cmd command: avrdude -c usbasp -p atmega328 -Pusb -B10
- burn bootloader pe 324 pe UART0 (324 are bootloader, 328 nu)

Trebuie modificat in programmers.txt comanda de upload la usbasp: 
usbasp.name=USBasp
usbasp.communication=usb
usbasp.protocol=usbasp
usbasp.program.protocol=usbasp
usbasp.program.tool=avrdude
usbasp.program.tool.default=avrdude
usbasp.program.extra_params=-Pusb -B10

-instalat mightycore si am selectat 324PB
-Driverul de instalat pt usb asp nu e cel cu win e cel cu lsuk...

Gasit aici: /home/maria/.arduino15
Cu:  find -name *prog*

Ca sa gasesti interfata seriala, de inlocuit in codul de python de GS:
sudo dmesg | rg tty

