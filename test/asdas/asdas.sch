EESchema Schematic File Version 2
LIBS:IC_raspberry
LIBS:power
LIBS:device
LIBS:valves
LIBS:contrib
LIBS:atmel
LIBS:opto
LIBS:siliconi
LIBS:cypress
LIBS:display
LIBS:philips
LIBS:digital-audio
LIBS:interface
LIBS:audio
LIBS:intel
LIBS:texas
LIBS:motorola
LIBS:conn
LIBS:microchip
LIBS:dsp
LIBS:microcontrollers
LIBS:special
LIBS:xilinx
LIBS:memory
LIBS:adc-dac
LIBS:cmos4000
LIBS:74xx
LIBS:regul
LIBS:linear
LIBS:analog_switches
LIBS:transistors
LIBS:asdas-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "9 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Entry Bus Bus
	5800 3700 5900 3800
Entry Bus Bus
	7500 3850 7600 3950
Entry Bus Bus
	6950 3050 7050 3150
NoConn ~ 6850 3600
NoConn ~ 7700 3600
Text GLabel 6750 3900 0    60   Input ~ 0
asd
$Comp
L 7400 U?
U 1 1 5486C34F
P 3550 2650
F 0 "U?" H 3550 2700 60  0000 C CNN
F 1 "7400" H 3550 2550 60  0000 C CNN
F 2 "~" H 3550 2650 60  0000 C CNN
F 3 "~" H 3550 2650 60  0000 C CNN
	1    3550 2650
	1    0    0    -1  
$EndComp
$Comp
L MOC3042M IC?
U 1 1 5486C36B
P 2750 1650
F 0 "IC?" H 2536 1829 40  0000 C CNN
F 1 "MOC3042M" H 2850 1465 40  0000 C CNN
F 2 "DIP6" H 2586 1475 29  0000 C CNN
F 3 "" H 2750 1650 60  0000 C CNN
	1    2750 1650
	1    0    0    -1  
$EndComp
$Comp
L MOC3042M IC?
U 1 1 5486C385
P 3900 3800
F 0 "IC?" H 3686 3979 40  0000 C CNN
F 1 "MOC3042M" H 4000 3615 40  0000 C CNN
F 2 "DIP6" H 3736 3625 29  0000 C CNN
F 3 "" H 3900 3800 60  0000 C CNN
	1    3900 3800
	1    0    0    -1  
$EndComp
$Comp
L RASPBERRY_IO RPi?
U 1 1 5486C534
P 2400 4200
F 0 "RPi?" H 2400 4900 60  0000 C CNN
F 1 "RASPBERRY_IO" V 2400 4200 50  0000 C CNN
F 2 "" H 2400 4200 60  0000 C CNN
F 3 "" H 2400 4200 60  0000 C CNN
	1    2400 4200
	1    0    0    -1  
$EndComp
$EndSCHEMATC
