/*
	Microchip 24LC256 I2C 256K EEPROM Arduino Library - Header File
	Made by Gonçalo Passos (more info @ http://diogopassos.pt) 2018
	BSD 2-Clause License
*/

#ifndef _24LC256_H
#define _24LC256_H

#include "Arduino.h"
#include "Wire.h"

#define WRITE_SUCESS 1
#define WRITE_ERROR -1
#define E24LC256_SIZE 0X7D00


class E24LC256 
{
public:
	bool detect();
	E24LC256(byte address);
	int8_t writeByte(int address, byte data);
	int8_t writePage(int address,int size, byte *buffer);
	byte readByte(int address);
	int readPage(int address,int size, byte *buffer);
private:
	byte address;
	void ack_pooling();


};

#endif
