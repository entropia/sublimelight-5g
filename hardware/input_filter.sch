EESchema Schematic File Version 5
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
Wire Wire Line
	5000 3200 5500 3200
Wire Wire Line
	5500 3200 5500 3300
Connection ~ 5500 3200
Wire Wire Line
	5500 3600 5500 3500
$Comp
L power:GND #PWR01
U 1 1 5E59BF1E
P 3500 4000
F 0 "#PWR01" H 3500 3750 50  0001 C CNN
F 1 "GND" H 3505 3827 50  0000 C CNN
F 2 "" H 3500 4000 50  0001 C CNN
F 3 "" H 3500 4000 50  0001 C CNN
	1    3500 4000
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5E59B8CB
P 3500 4000
F 0 "#FLG01" H 3500 4075 50  0001 C CNN
F 1 "PWR_FLAG" H 3500 4173 50  0000 C CNN
F 2 "" H 3500 4000 50  0001 C CNN
F 3 "~" H 3500 4000 50  0001 C CNN
	1    3500 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3500 4500 3600
Connection ~ 4500 3600
Wire Wire Line
	5000 3600 5000 3500
$Comp
L Device:CP_Small C1
U 1 1 5E5994CF
P 5000 3400
F 0 "C1" H 5088 3445 50  0000 L CNN
F 1 "EEU-FP1V511B" H 4650 3100 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 5000 3400 50  0001 C CNN
F 3 "~" H 5000 3400 50  0001 C CNN
	1    5000 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3600 5000 3600
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 5E597810
P 4300 3500
F 0 "J1" H 4219 3177 50  0000 C CNN
F 1 "Conn_01x02" H 4219 3268 50  0000 C CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-3-2-5.08_1x02_P5.08mm_Horizontal" H 4300 3500 50  0001 C CNN
F 3 "~" H 4300 3500 50  0001 C CNN
	1    4300 3500
	-1   0    0    1   
$EndComp
Wire Wire Line
	6000 3600 6000 3500
Wire Wire Line
	6000 3200 6000 3400
Wire Wire Line
	5500 3200 6000 3200
Wire Wire Line
	5500 3600 6000 3600
$Comp
L power:GND #PWR02
U 1 1 5E59B525
P 4500 3600
F 0 "#PWR02" H 4500 3350 50  0001 C CNN
F 1 "GND" H 4505 3427 50  0000 C CNN
F 2 "" H 4500 3600 50  0001 C CNN
F 3 "" H 4500 3600 50  0001 C CNN
	1    4500 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:L L1
U 1 1 5E599CDE
P 4750 3200
F 0 "L1" V 4939 3200 50  0000 C CNN
F 1 "5252-RC" V 4848 3200 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_Power_L38.0mm_W9.0mm_P40.64mm" H 4750 3200 50  0001 C CNN
F 3 "~" H 4750 3200 50  0001 C CNN
	1    4750 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4500 3400 4500 3200
Wire Wire Line
	4500 3200 4600 3200
Text Label 4500 3200 0    50   ~ 0
V_IN
Connection ~ 5500 3600
Wire Wire Line
	5000 3600 5500 3600
Connection ~ 5000 3600
$Comp
L Device:CP_Small C2
U 1 1 5E5985DB
P 5500 3400
F 0 "C2" H 5588 3445 50  0000 L CNN
F 1 "EEU-FP1V511B" H 5350 3100 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 5500 3400 50  0001 C CNN
F 3 "~" H 5500 3400 50  0001 C CNN
	1    5500 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5E597B79
P 6200 3400
F 0 "J2" H 6280 3392 50  0000 L CNN
F 1 "Conn_01x02" H 6280 3301 50  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-3-2-5.08_1x02_P5.08mm_Horizontal" H 6200 3400 50  0001 C CNN
F 3 "~" H 6200 3400 50  0001 C CNN
	1    6200 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3200 5000 3200
Wire Wire Line
	5000 3200 5000 3300
Connection ~ 5000 3200
Text Label 6000 3200 2    50   ~ 0
V_OUT
$EndSCHEMATC
