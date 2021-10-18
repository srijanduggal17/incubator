
import serial.tools.list_ports as port_list
import serial
import csv
import re

i = 1
s = serial.Serial("COM4", 9600)

f = open("temp_sensors.csv", 'w')
while 1:
	res = s.readline().decode()
	f.write(res)
	print(res, end="")