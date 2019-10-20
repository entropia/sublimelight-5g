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
$Comp
L RF_Module:ESP32-WROOM-32D U2
U 1 1 5DAB7DE9
P 3000 3200
F 0 "U2" H 2500 1850 50  0000 C CNN
F 1 "ESP32-WROOM-32D" H 3450 1850 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 3000 1700 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 2700 3250 50  0001 C CNN
	1    3000 3200
	1    0    0    -1  
$EndComp
$Comp
L meanwell_led_driver:LDD-H U3
U 1 1 5DABA11A
P 9200 4200
F 0 "U3" H 8950 3750 50  0000 C CNN
F 1 "LDD-H" H 9400 3750 50  0000 C CNN
F 2 "meanwell_led_driver:LDD_H" H 9200 4200 50  0001 C CNN
F 3 "" H 9200 4200 50  0001 C CNN
	1    9200 4200
	1    0    0    -1  
$EndComp
$Comp
L meanwell_led_driver:LDD-H U4
U 1 1 5DABA9E5
P 9200 5700
F 0 "U4" H 8950 5250 50  0000 C CNN
F 1 "LDD-H" H 9400 5250 50  0000 C CNN
F 2 "meanwell_led_driver:LDD_H" H 9200 5700 50  0001 C CNN
F 3 "" H 9200 5700 50  0001 C CNN
	1    9200 5700
	1    0    0    -1  
$EndComp
NoConn ~ 2400 3700
NoConn ~ 2400 3600
NoConn ~ 2400 3500
NoConn ~ 2400 3400
NoConn ~ 2400 3300
NoConn ~ 2400 3200
$Comp
L power:GND #PWR017
U 1 1 5DABB558
P 3000 4600
F 0 "#PWR017" H 3000 4350 50  0001 C CNN
F 1 "GND" H 3005 4427 50  0000 C CNN
F 2 "" H 3000 4600 50  0001 C CNN
F 3 "" H 3000 4600 50  0001 C CNN
	1    3000 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR027
U 1 1 5DABBD60
P 9200 6200
F 0 "#PWR027" H 9200 5950 50  0001 C CNN
F 1 "GND" H 9205 6027 50  0000 C CNN
F 2 "" H 9200 6200 50  0001 C CNN
F 3 "" H 9200 6200 50  0001 C CNN
	1    9200 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 5DABC4BF
P 9200 4700
F 0 "#PWR018" H 9200 4450 50  0001 C CNN
F 1 "GND" H 9205 4527 50  0000 C CNN
F 2 "" H 9200 4700 50  0001 C CNN
F 3 "" H 9200 4700 50  0001 C CNN
	1    9200 4700
	1    0    0    -1  
$EndComp
$Comp
L power:+28V #PWR013
U 1 1 5DABD9FC
P 9200 3700
F 0 "#PWR013" H 9200 3550 50  0001 C CNN
F 1 "+28V" H 9215 3873 50  0000 C CNN
F 2 "" H 9450 3750 50  0001 C CNN
F 3 "" H 9450 3750 50  0001 C CNN
	1    9200 3700
	1    0    0    -1  
$EndComp
$Comp
L power:+28V #PWR019
U 1 1 5DABE142
P 9200 5200
F 0 "#PWR019" H 9200 5050 50  0001 C CNN
F 1 "+28V" H 9215 5373 50  0000 C CNN
F 2 "" H 9450 5250 50  0001 C CNN
F 3 "" H 9450 5250 50  0001 C CNN
	1    9200 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5DAC0A83
P 1600 2200
F 0 "C1" H 1715 2246 50  0000 L CNN
F 1 "100n" H 1715 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1638 2050 50  0001 C CNN
F 3 "~" H 1600 2200 50  0001 C CNN
	1    1600 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5DAC1D32
P 1600 1800
F 0 "R1" H 1670 1846 50  0000 L CNN
F 1 "10k" H 1670 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1530 1800 50  0001 C CNN
F 3 "~" H 1600 1800 50  0001 C CNN
	1    1600 1800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5DAC2671
P 1600 2350
F 0 "#PWR08" H 1600 2100 50  0001 C CNN
F 1 "GND" H 1605 2177 50  0000 C CNN
F 2 "" H 1600 2350 50  0001 C CNN
F 3 "" H 1600 2350 50  0001 C CNN
	1    1600 2350
	1    0    0    -1  
$EndComp
Text Label 2400 2000 2    50   ~ 0
EN
$Comp
L power:+3.3V #PWR02
U 1 1 5DAC2E5E
P 1600 1650
F 0 "#PWR02" H 1600 1500 50  0001 C CNN
F 1 "+3.3V" H 1615 1823 50  0000 C CNN
F 2 "" H 1600 1650 50  0001 C CNN
F 3 "" H 1600 1650 50  0001 C CNN
	1    1600 1650
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR05
U 1 1 5DAC35A2
P 3000 1800
F 0 "#PWR05" H 3000 1650 50  0001 C CNN
F 1 "+3.3V" H 3015 1973 50  0000 C CNN
F 2 "" H 3000 1800 50  0001 C CNN
F 3 "" H 3000 1800 50  0001 C CNN
	1    3000 1800
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_2_Open JP1
U 1 1 5DAC3EBD
P 4200 2000
F 0 "JP1" H 4200 2235 50  0000 C CNN
F 1 "Jumper_2_Open" H 4200 2144 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4200 2000 50  0001 C CNN
F 3 "~" H 4200 2000 50  0001 C CNN
	1    4200 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 2000 4000 2000
$Comp
L power:GND #PWR06
U 1 1 5DAC4CBD
P 4500 2000
F 0 "#PWR06" H 4500 1750 50  0001 C CNN
F 1 "GND" H 4505 1827 50  0000 C CNN
F 2 "" H 4500 2000 50  0001 C CNN
F 3 "" H 4500 2000 50  0001 C CNN
	1    4500 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 2000 4500 2000
Text Label 3650 2000 0    50   ~ 0
IO0
$Comp
L Connector:Barrel_Jack_Switch J3
U 1 1 5DAC5A88
P 10300 4200
F 0 "J3" H 10450 4400 50  0000 R CNN
F 1 "Barrel_Jack_Switch" H 10500 4000 50  0000 R CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 10350 4160 50  0001 C CNN
F 3 "~" H 10350 4160 50  0001 C CNN
	1    10300 4200
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J5
U 1 1 5DAC6FE8
P 10300 5700
F 0 "J5" H 10450 5900 50  0000 R CNN
F 1 "Barrel_Jack_Switch" H 10500 5500 50  0000 R CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 10350 5660 50  0001 C CNN
F 3 "~" H 10350 5660 50  0001 C CNN
	1    10300 5700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9500 5600 10000 5600
Wire Wire Line
	9500 5800 9950 5800
Wire Wire Line
	10000 5700 9950 5700
Wire Wire Line
	9950 5700 9950 5800
Connection ~ 9950 5800
Wire Wire Line
	9950 5800 10000 5800
Wire Wire Line
	9500 4100 10000 4100
Wire Wire Line
	10000 4300 9950 4300
Wire Wire Line
	10000 4200 9950 4200
Wire Wire Line
	9950 4200 9950 4300
Connection ~ 9950 4300
Wire Wire Line
	9950 4300 9500 4300
Wire Wire Line
	8300 4200 8900 4200
Wire Wire Line
	8300 5700 8900 5700
Text Label 8300 4200 0    50   ~ 0
PWM_W_COLD
Text Label 8300 5700 0    50   ~ 0
PWM_W_WARM
Text Label 9500 4100 0    50   ~ 0
W_COLD+
Text Label 9500 4300 0    50   ~ 0
W_COLD-
Text Label 9500 5600 0    50   ~ 0
W_WARM+
Text Label 9500 5800 0    50   ~ 0
W_WARM-
$Comp
L Connector_Generic:Conn_01x05 J4
U 1 1 5DAD2266
P 5700 5250
F 0 "J4" H 5780 5292 50  0000 L CNN
F 1 "Conn_01x05" H 5780 5201 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 5700 5250 50  0001 C CNN
F 3 "~" H 5700 5250 50  0001 C CNN
	1    5700 5250
	1    0    0    -1  
$EndComp
NoConn ~ 5500 5050
NoConn ~ 5500 5450
Text Notes 5250 5050 0    50   ~ 0
+3V3
Text Notes 5250 5450 0    50   ~ 0
+5V
Wire Wire Line
	5100 5150 5500 5150
Wire Wire Line
	5100 5250 5500 5250
$Comp
L power:GND #PWR020
U 1 1 5DAD4BEF
P 5300 5350
F 0 "#PWR020" H 5300 5100 50  0001 C CNN
F 1 "GND" V 5305 5222 50  0000 R CNN
F 2 "" H 5300 5350 50  0001 C CNN
F 3 "" H 5300 5350 50  0001 C CNN
	1    5300 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 5350 5500 5350
Text Label 5100 5150 0    50   ~ 0
USB_TXD
Text Label 5100 5250 0    50   ~ 0
USB_RXD
$Comp
L Interface_UART:SP3485CN U1
U 1 1 5DAD68F8
P 8000 1300
F 0 "U1" H 7700 950 50  0000 C CNN
F 1 "ST1480ACDR" H 8300 950 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9050 950 50  0001 C CIN
F 3 "" H 8000 1300 50  0001 C CNN
	1    8000 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5DAED7F4
P 8000 1700
F 0 "#PWR03" H 8000 1450 50  0001 C CNN
F 1 "GND" H 8005 1527 50  0000 C CNN
F 2 "" H 8000 1700 50  0001 C CNN
F 3 "" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:RJ45_Shielded J1
U 1 1 5DAEF5B5
P 10250 1300
F 0 "J1" H 9920 1247 50  0000 R CNN
F 1 "RJ45" H 9920 1338 50  0000 R CNN
F 2 "" V 10250 1325 50  0001 C CNN
F 3 "~" V 10250 1325 50  0001 C CNN
	1    10250 1300
	-1   0    0    1   
$EndComp
$Comp
L Connector:RJ45_Shielded J2
U 1 1 5DAF1CB2
P 10250 2600
F 0 "J2" H 9920 2547 50  0000 R CNN
F 1 "RJ45" H 9920 2638 50  0000 R CNN
F 2 "" V 10250 2625 50  0001 C CNN
F 3 "~" V 10250 2625 50  0001 C CNN
	1    10250 2600
	-1   0    0    1   
$EndComp
$Comp
L Device:R R3
U 1 1 5DB00DCA
P 8500 2800
F 0 "R3" H 8570 2846 50  0000 L CNN
F 1 "120R" H 8570 2755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8430 2800 50  0001 C CNN
F 3 "~" H 8500 2800 50  0001 C CNN
	1    8500 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 1200 8800 1200
Wire Wire Line
	8800 1400 8400 1400
Text Label 8800 1200 2    50   ~ 0
RS485_A
Text Label 8800 1400 2    50   ~ 0
RS485_B
Wire Wire Line
	9500 1000 9850 1000
Wire Wire Line
	9500 1100 9850 1100
Wire Wire Line
	9500 2300 9850 2300
Wire Wire Line
	9500 2400 9850 2400
$Comp
L power:GND #PWR04
U 1 1 5DB0E861
P 9750 1700
F 0 "#PWR04" H 9750 1450 50  0001 C CNN
F 1 "GND" H 9755 1527 50  0000 C CNN
F 2 "" H 9750 1700 50  0001 C CNN
F 3 "" H 9750 1700 50  0001 C CNN
	1    9750 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 1700 9750 1700
Wire Wire Line
	9850 1600 9750 1600
Wire Wire Line
	9750 1600 9750 1700
Connection ~ 9750 1700
$Comp
L power:GND #PWR012
U 1 1 5DB0FB88
P 9750 3000
F 0 "#PWR012" H 9750 2750 50  0001 C CNN
F 1 "GND" H 9755 2827 50  0000 C CNN
F 2 "" H 9750 3000 50  0001 C CNN
F 3 "" H 9750 3000 50  0001 C CNN
	1    9750 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 2900 9750 2900
Wire Wire Line
	9750 2900 9750 3000
Wire Wire Line
	9850 3000 9750 3000
Connection ~ 9750 3000
Text Label 9500 1000 0    50   ~ 0
RS485_A
Text Label 9500 2300 0    50   ~ 0
RS485_A
Text Label 9500 1100 0    50   ~ 0
RS485_B
Text Label 9500 2400 0    50   ~ 0
RS485_B
Wire Wire Line
	8150 2150 8500 2150
Wire Wire Line
	8150 3000 8500 3000
Text Label 8150 2150 0    50   ~ 0
RS485_A
Text Label 8150 3000 0    50   ~ 0
RS485_B
$Comp
L Jumper:Jumper_2_Open JP2
U 1 1 5DB13698
P 8500 2350
F 0 "JP2" V 8454 2448 50  0000 L CNN
F 1 "Jumper_2_Open" V 8545 2448 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8500 2350 50  0001 C CNN
F 3 "~" H 8500 2350 50  0001 C CNN
	1    8500 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	8500 3000 8500 2950
Wire Wire Line
	8500 2550 8500 2650
Wire Wire Line
	4000 2300 3600 2300
Wire Wire Line
	4000 2100 3600 2100
Text Label 4000 2300 2    50   ~ 0
USB_TXD
Text Label 4000 2100 2    50   ~ 0
USB_RXD
Wire Wire Line
	1600 1950 1600 2000
Wire Wire Line
	1600 2000 2400 2000
Connection ~ 1600 2000
Wire Wire Line
	1600 2000 1600 2050
Text Notes 9950 3300 0    50   ~ 0
 IEC 60603-7\nANSI E1.11-2008
Wire Wire Line
	9500 1200 9850 1200
Wire Wire Line
	9500 1300 9850 1300
Wire Wire Line
	9500 1400 9850 1400
Wire Wire Line
	9500 1500 9850 1500
Wire Wire Line
	9500 2500 9850 2500
Wire Wire Line
	9500 2600 9850 2600
Wire Wire Line
	9500 2700 9850 2700
Wire Wire Line
	9500 2800 9850 2800
Text Label 9500 1200 0    50   ~ 0
RJ45_3
Text Label 9500 1300 0    50   ~ 0
RJ45_4
Text Label 9500 1400 0    50   ~ 0
RJ45_5
Text Label 9500 1500 0    50   ~ 0
RJ45_6
Text Label 9500 2500 0    50   ~ 0
RJ45_3
Text Label 9500 2600 0    50   ~ 0
RJ45_4
Text Label 9500 2700 0    50   ~ 0
RJ45_5
Text Label 9500 2800 0    50   ~ 0
RJ45_6
$Comp
L Device:C C2
U 1 1 5DAB92A8
P 7650 2250
F 0 "C2" H 7765 2296 50  0000 L CNN
F 1 "100n" H 7765 2205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7688 2100 50  0001 C CNN
F 3 "~" H 7650 2250 50  0001 C CNN
	1    7650 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR01
U 1 1 5DAECCE5
P 8000 900
F 0 "#PWR01" H 8000 750 50  0001 C CNN
F 1 "+3.3V" H 8015 1073 50  0000 C CNN
F 2 "" H 8000 900 50  0001 C CNN
F 3 "" H 8000 900 50  0001 C CNN
	1    8000 900 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR07
U 1 1 5DABCD9F
P 7650 2100
F 0 "#PWR07" H 7650 1950 50  0001 C CNN
F 1 "+3.3V" H 7665 2273 50  0000 C CNN
F 2 "" H 7650 2100 50  0001 C CNN
F 3 "" H 7650 2100 50  0001 C CNN
	1    7650 2100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5DABD6C3
P 7650 2400
F 0 "#PWR09" H 7650 2150 50  0001 C CNN
F 1 "GND" H 7655 2227 50  0000 C CNN
F 2 "" H 7650 2400 50  0001 C CNN
F 3 "" H 7650 2400 50  0001 C CNN
	1    7650 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1100 7600 1100
Wire Wire Line
	7350 1500 7600 1500
Text Label 7350 1100 0    50   ~ 0
RO
Wire Wire Line
	7350 1300 7600 1300
Wire Wire Line
	7600 1300 7600 1200
Wire Wire Line
	7600 1300 7600 1400
Connection ~ 7600 1300
Text Label 7350 1300 0    50   ~ 0
DE
Text Label 7350 1500 0    50   ~ 0
DI
Wire Wire Line
	4000 3100 3600 3100
Wire Wire Line
	4000 3000 3600 3000
Text Label 4000 3000 2    50   ~ 0
RO
Text Label 4000 3100 2    50   ~ 0
DI
NoConn ~ 3600 2600
$Comp
L power:GND #PWR016
U 1 1 5DAC0BD5
P 2000 4300
F 0 "#PWR016" H 2000 4050 50  0001 C CNN
F 1 "GND" H 2005 4127 50  0000 C CNN
F 2 "" H 2000 4300 50  0001 C CNN
F 3 "" H 2000 4300 50  0001 C CNN
	1    2000 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5DAC0BE3
P 2000 4150
F 0 "C3" H 2115 4196 50  0000 L CNN
F 1 "100n" H 2115 4105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2038 4000 50  0001 C CNN
F 3 "~" H 2000 4150 50  0001 C CNN
	1    2000 4150
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR015
U 1 1 5DAC3D24
P 2000 4000
F 0 "#PWR015" H 2000 3850 50  0001 C CNN
F 1 "+3.3V" H 2015 4173 50  0000 C CNN
F 2 "" H 2000 4000 50  0001 C CNN
F 3 "" H 2000 4000 50  0001 C CNN
	1    2000 4000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 5DAC8E0B
P 1300 6000
F 0 "J6" H 1218 6217 50  0000 C CNN
F 1 "Conn_01x02" H 1218 6126 50  0000 C CNN
F 2 "" H 1300 6000 50  0001 C CNN
F 3 "~" H 1300 6000 50  0001 C CNN
	1    1300 6000
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR024
U 1 1 5DAC9E2D
P 1500 6100
F 0 "#PWR024" H 1500 5850 50  0001 C CNN
F 1 "GND" H 1505 5927 50  0000 C CNN
F 2 "" H 1500 6100 50  0001 C CNN
F 3 "" H 1500 6100 50  0001 C CNN
	1    1500 6100
	1    0    0    -1  
$EndComp
NoConn ~ 3600 2200
$Comp
L Regulator_Switching:R-78C3.3-1.0 U5
U 1 1 5DACA989
P 3000 6000
F 0 "U5" H 2800 5750 50  0000 C CNN
F 1 "VXO7803-1000" H 3000 6151 50  0000 C CNN
F 2 "Converter_DCDC:Converter_DCDC_RECOM_R-78E-0.5_THT" H 3050 5750 50  0001 L CIN
F 3 "https://www.recom-power.com/pdf/Innoline/R-78Cxx-1.0.pdf" H 3000 6000 50  0001 C CNN
	1    3000 6000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5DAD0455
P 3000 6300
F 0 "#PWR028" H 3000 6050 50  0001 C CNN
F 1 "GND" H 3005 6127 50  0000 C CNN
F 2 "" H 3000 6300 50  0001 C CNN
F 3 "" H 3000 6300 50  0001 C CNN
	1    3000 6300
	1    0    0    -1  
$EndComp
$Comp
L power:+28V #PWR021
U 1 1 5DAD711F
P 1500 6000
F 0 "#PWR021" H 1500 5850 50  0001 C CNN
F 1 "+28V" H 1515 6173 50  0000 C CNN
F 2 "" H 1750 6050 50  0001 C CNN
F 3 "" H 1750 6050 50  0001 C CNN
	1    1500 6000
	0    1    1    0   
$EndComp
$Comp
L Device:C C4
U 1 1 5DADC344
P 2400 6200
F 0 "C4" H 2515 6246 50  0000 L CNN
F 1 "10u" H 2515 6155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2438 6050 50  0001 C CNN
F 3 "~" H 2400 6200 50  0001 C CNN
	1    2400 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR029
U 1 1 5DADC351
P 2400 6350
F 0 "#PWR029" H 2400 6100 50  0001 C CNN
F 1 "GND" H 2405 6177 50  0000 C CNN
F 2 "" H 2400 6350 50  0001 C CNN
F 3 "" H 2400 6350 50  0001 C CNN
	1    2400 6350
	1    0    0    -1  
$EndComp
$Comp
L power:+28V #PWR025
U 1 1 5DADDD99
P 2400 5950
F 0 "#PWR025" H 2400 5800 50  0001 C CNN
F 1 "+28V" H 2415 6123 50  0000 C CNN
F 2 "" H 2650 6000 50  0001 C CNN
F 3 "" H 2650 6000 50  0001 C CNN
	1    2400 5950
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR026
U 1 1 5DADFBD1
P 3600 5950
F 0 "#PWR026" H 3600 5800 50  0001 C CNN
F 1 "+3.3V" H 3615 6123 50  0000 C CNN
F 2 "" H 3600 5950 50  0001 C CNN
F 3 "" H 3600 5950 50  0001 C CNN
	1    3600 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 5DADFBDF
P 3600 6200
F 0 "C5" H 3715 6246 50  0000 L CNN
F 1 "22u" H 3715 6155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3638 6050 50  0001 C CNN
F 3 "~" H 3600 6200 50  0001 C CNN
	1    3600 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 5DADFBEC
P 3600 6350
F 0 "#PWR030" H 3600 6100 50  0001 C CNN
F 1 "GND" H 3605 6177 50  0000 C CNN
F 2 "" H 3600 6350 50  0001 C CNN
F 3 "" H 3600 6350 50  0001 C CNN
	1    3600 6350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5DAE8440
P 6500 3900
F 0 "#PWR014" H 6500 3650 50  0001 C CNN
F 1 "GND" V 6505 3772 50  0000 R CNN
F 2 "" H 6500 3900 50  0001 C CNN
F 3 "" H 6500 3900 50  0001 C CNN
	1    6500 3900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5800 3900 6100 3900
Text Label 5800 3900 0    50   ~ 0
TASTER
$Comp
L Device:LED D1
U 1 1 5DAE9293
P 6350 2650
F 0 "D1" H 6343 2395 50  0000 C CNN
F 1 "LED" H 6343 2486 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 6350 2650 50  0001 C CNN
F 3 "~" H 6350 2650 50  0001 C CNN
	1    6350 2650
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 5DAEB863
P 6350 3000
F 0 "D2" H 6343 2745 50  0000 C CNN
F 1 "LED" H 6343 2836 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 6350 3000 50  0001 C CNN
F 3 "~" H 6350 3000 50  0001 C CNN
	1    6350 3000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 5DAEEB89
P 6050 2650
F 0 "R2" H 6120 2696 50  0000 L CNN
F 1 "1k" H 6120 2605 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5980 2650 50  0001 C CNN
F 3 "~" H 6050 2650 50  0001 C CNN
	1    6050 2650
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5DAEF6CE
P 6050 3000
F 0 "R4" H 6120 3046 50  0000 L CNN
F 1 "1k" H 6120 2955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5980 3000 50  0001 C CNN
F 3 "~" H 6050 3000 50  0001 C CNN
	1    6050 3000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5DAF0DF0
P 6500 3000
F 0 "#PWR011" H 6500 2750 50  0001 C CNN
F 1 "GND" V 6505 2872 50  0000 R CNN
F 2 "" H 6500 3000 50  0001 C CNN
F 3 "" H 6500 3000 50  0001 C CNN
	1    6500 3000
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5DAF1420
P 6500 2650
F 0 "#PWR010" H 6500 2400 50  0001 C CNN
F 1 "GND" V 6505 2522 50  0000 R CNN
F 2 "" H 6500 2650 50  0001 C CNN
F 3 "" H 6500 2650 50  0001 C CNN
	1    6500 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5700 2650 5900 2650
Wire Wire Line
	5700 3000 5900 3000
Text Label 5700 2650 0    50   ~ 0
LED0
Text Label 5700 3000 0    50   ~ 0
LED1
$Comp
L Switch:SW_Push SW1
U 1 1 5DACE5E2
P 6300 3900
F 0 "SW1" H 6300 4185 50  0000 C CNN
F 1 "SW_Push" H 6300 4094 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 6300 4100 50  0001 C CNN
F 3 "~" H 6300 4100 50  0001 C CNN
	1    6300 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 6000 2400 6000
Wire Wire Line
	2400 6000 2400 5950
Wire Wire Line
	2400 6000 2400 6050
Connection ~ 2400 6000
Wire Wire Line
	3600 5950 3600 6000
Wire Wire Line
	3300 6000 3600 6000
Connection ~ 3600 6000
Wire Wire Line
	3600 6000 3600 6050
Text Notes 1300 5650 0    50   ~ 0
add reverse polarity prevention?\nadd inductor?\nadd fuse?
Text Notes 9950 2000 0    50   ~ 0
change rj45 footprint?
Text Notes 4100 3700 0    50   ~ 0
add pin header for gpios
$EndSCHEMATC
