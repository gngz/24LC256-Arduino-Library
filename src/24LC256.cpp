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

/*
	Function to detect if 24LC256 is present in bus with the address passed in constructor.
	Returns true if eeprom is present in i2c bus.
*/

bool E24LC256::detect() {
	byte code;
	Wire.beginTransmission(address);
	code = Wire.endTransmission();
	return (code == 0);
}


/*
	Acknowledge Pooling Algoritihm recomended by Manufacturer	
*/

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

/*
	Function that writes a byte in EEPROM.
	address parameter: eeprom cell address between 0x0000 and 0x7CFF
	data parameter: byte to write to a eeprom cell.
	Returns the state of operation (WRITE_SUCESS or WRITE_ERROR)
*/

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

/*
	Function that reads one byte from eeprom.
	address parameter: eeprom cell address between 0x0000 and 0x7CFF
	Returns the byte readed from eeprom.
*/

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

/*
	Write multiple bytes into eeprom
	address parameter:	address of the first eeprom cell to write
	size parameter:	size of buffer
	buffer parameter: address of buffer in primary memory.
	Returns the state of operation (WRITE_ERROR or WRITE_SUCESS) 

	To do: Check memory size.
*/

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

/*
	Read multiple eeprom cell address's and saves into a buffer
	address parameter:	address of the first eeprom cell to read
	size parameter:	size of buffer
	buffer parameter: address of buffer in primary memory.
	Returns the number of bytes readed

*/

int E24LC256::readPage(int address,int size, byte *buffer)
{
	int bytes_readed;
	Wire.beginTransmission(this->address);
	Wire.write((byte) address>>8);
	Wire.write((byte) address&0xFF);
	Wire.endTransmission();


	for(int i = 0;i<size;i++)
	{
      Wire.requestFrom(this->address,size);
      if(Wire.available())
      {
        buffer[i] = Wire.read();
        bytes_readed++;
      }
	}


	return bytes_readed;
}
