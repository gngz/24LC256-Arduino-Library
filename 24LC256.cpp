/*
	Microchip 24LC256 I2C 256K EEPROM Arduino Library - Code File
	Made by Gonçalo Passos (more info @ http://diogopassos.pt) 2018
	BSD 2-Clause License
*/

#include "24LC256.h"


E24LC256::E24LC256(byte address)
{
	this->address = address;
	Wire.begin();
}


bool E24LC256::detect() {
	byte code;
	Wire.beginTransmission(address);
	code = Wire.endTransmission();
	return (code == 0);
}

void E24LC256::ack_pooling()
{
	byte code = -1;
	do
	{
		Wire.beginTransmission(this->address);
		Wire.write((byte) 0);
		code = Wire.endTransmission();
	}
	while(code != 0);
}

int8_t E24LC256::writeByte(int address, byte data)
{
	ack_pooling();
	Wire.beginTransmission(this->address);
	Wire.write((byte) address>>8);
	Wire.write((byte) address & 0xFF);
	Wire.write((byte) data);

	if(Wire.endTransmission() == 0)
	{
		return WRITE_SUCESS;
	}
	else
	{
		return WRITE_ERROR;
	}
}

byte E24LC256::readByte(int address)
{
	ack_pooling();
	Wire.beginTransmission(this->address);
	Wire.write((byte) address>>8);
	Wire.write((byte) address&0xFF);
	Wire.endTransmission();
	Wire.requestFrom(this->address,(int) 1);
	Wire.available();
	return Wire.read();
}

int8_t E24LC256::writePage(int address,int size, byte *buffer)
{
	ack_pooling();
	
	bool error = false;

	if(size > 64)
	{
		
		int new_size = size;
		int current_buffer_loc;
		int base_addr = address;
		while(new_size > 64)
		{
			Wire.beginTransmission(this->address);
			Wire.write((byte) base_addr>>8);
			Wire.write((byte) base_addr&0xFF);

			for(int i = current_buffer_loc; i<current_buffer_loc + 64;i++)
			{
				Wire.write((byte) buffer[i]);
			}

			error = error || (Wire.endTransmission() == 0);

			if(error)
			{
				return WRITE_ERROR;
			}

			ack_pooling();

			current_buffer_loc += 63;
			base_addr += 63;
			new_size -= 64;
		}

		Wire.beginTransmission(this->address);
		Wire.write((byte) base_addr>>8);
		Wire.write((byte) base_addr&0xFF);

		for(int i = current_buffer_loc ; i<new_size;i++)
		{
			Wire.write(buffer[i]);
		}

		Wire.endTransmission();

	}
	else
	{
		Wire.beginTransmission(this->address);
		Wire.write((byte) address>>8);
		Wire.write((byte) address&0xFF);
		for(int i = 0; i<size;i++)
		{
			Wire.write((byte) buffer[i]);
		}

		Wire.endTransmission();



	}


	if(error)
	{
		return WRITE_ERROR;
	}
	else
	{
		return WRITE_SUCESS;
	}




}

int E24LC256::readPage(int address,int size, byte *buffer)
{
	int bytes_readed;
	Wire.beginTransmission(this->address);
	Wire.write((byte) address>>8);
	Wire.write((byte) address&0xFF);
	Wire.endTransmission();
	Wire.requestFrom(this->address,size);

	for(int i = 0; Wire.available()>0;i++)
	{

			buffer[i] = Wire.read();
			bytes_readed++;
	}


	return bytes_readed;
}
