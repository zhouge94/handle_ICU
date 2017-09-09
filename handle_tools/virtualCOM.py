#! /usr/bin/env python
#coding=utf-8

import pty
import os
import select
import time
import struct 
import binascii
import math as m
fs=250.0;
T=1.0/fs;
t=0.0;

def mkpty():
	master, slave = pty.openpty()
	slaveName = os.ttyname(slave)
	print '\nslave device names: ',slaveName
	return master ,slaveName
def sendECG(ecg1,ecg2,ecg3):
	value=(ecg1,ecg2,ecg3)
	s=struct.Struct('hhh')
	sendbuffer=s.pack(*value)
	return sendbuffer
if __name__ == "__main__":
	master,slaveName= mkpty()
	while True:
		head=(0xaa,0xaa,0x08,0xf7,0x01,0x00)
		s=struct.Struct('BBBBBB')
		buff=s.pack(*head)
		os.write(master,buff)
		ecg1=m.sin(2*3.14*1*t)*100
		ecg2=m.sin(2*3.14*2*t)*100
		ecg3=m.sin(2*3.14*3*t)*100
		data=sendECG(ecg1,ecg2,ecg3)
		#data=sendECG(1.1,2.2,3.3)
		if True:
			print slaveName + " ;%d data." % len(data) 
			print 'Format:',binascii.hexlify(data)
		os.write(master, data)
		time.sleep(T)
		t=t+T
		print 'ecg1:[%2.2f] ' % ecg1 +'ecg2:[%2.2f] ' %ecg2 +'ecg2:[%2.2f]' %ecg3

