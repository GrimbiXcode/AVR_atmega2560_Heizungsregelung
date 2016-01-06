# AVR Atmega2560 Heizungssteuerung

Die Software der Heizungsteuerung soll folgende Punkte erfüllen:
* Die aktuelle Temperatur (Istwert) von einemgeeigneten Temperatursensor am Analog-Eingang ADC0 zur Verfügung gestellt werde (Kennlinie: 0V = 0°C, +0.1V/°C)* Sowie die Vorgabe-Temperatur (Sollwert) als digitaler 8-Bit-Wert am I/O-Port A des ATmega16 (Skalierung: Wert 0 = 0°C in 0.1°C Schritte).* Als Heizungsausgang soll das Portbit PC0 dienen (0 = Heizung aus, 1 = Heinzung ein)
