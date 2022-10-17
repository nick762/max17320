#include "max17320.h"
#include <OneWire.h>
#include "Arduino.h"

OneWire _max320(A0);


uint8_t MAX17320::_scan(){
	uint8_t addr[8];
	_max320.reset_search();
	if(!_max320.search(addr)){
		_max320.reset_search();
		return 0;
	}else{
		return 1;
	}
}			

uint8_t MAX17320::_cellNum(){
    uint8_t num = lowByte(_readReg(0xb5, 0x01));
	num = num<<6;
	if(num == 0){
		return 2;
	}else if(num == 64){
		return 3;
	}else if(num == 128){
		return 4;
	}else{
		return 0;
	}
}	
		
uint16_t MAX17320::_readReg(uint8_t lsb, uint8_t msb)
{
		boolean present;
		uint16_t vol;
		present = _max320.reset();
		_max320.skip();
		if(present){
			_max320.write(0x69);
			_max320.write(lsb);
			_max320.write(msb);
			vol = _max320.read();
			vol |= _max320.read()<<8;
			return vol;
		}else{
			return 0;
		}
}

void MAX17320::_writeData(uint8_t lsb, uint8_t msb, uint8_t* data){
	boolean present;
	present = _max320.reset();
	if(present){
		_max320.write(0x6c);
		_max320.write(lsb);
		_max320.write(msb,1);
		_max320.write(data[0]);
		_max320.write(data[1]);
		delay(25);
	}
}

void MAX17320::_fReset()
{ 
	byte cmd1[] = {0x00,0x00};
	byte cmd2[] = {0x0f,0x00};
	byte cmd3[] = {0x80,0x00};
	byte cmd4[] = {0xf9,0x00};
	
	_writeData(0x61,0x00, cmd1);
	_writeData(0x61,0x00, cmd1);
	_writeData(0x60,0x00, cmd2);
	delay(10);
	_writeData(0x61,0x00, cmd1);
	_writeData(0x61,0x00, cmd1);
	_writeData(0xAB,0x00, cmd3);
	delay(50);
	_writeData(0x61,0x00, cmd4);
	delay(10);
}

uint8_t MAX17320::_readType()
{	
	uint8_t type = lowByte(_readReg(REG_TYPE_320, REG_MSB_320));
	type = type<<4;
	//Serial.println(type,HEX);
	return type;
}

uint16_t MAX17320::_readCapacity(){
		uint16_t capacity = _readReg(REG_CAPACITY_320, REG_MSB_320);
		return capacity / 256; 
}
	
uint16_t MAX17320::_readFCap(){
	uint16_t capacity = (int16_t)_readReg(REG_FCAP_320, REG_MSB_320);
	uint16_t rSense = _readReg(0xcf, 0x01);
	if(rSense == 500){		
		return capacity * 1.0F;
	}else if(rSense == 1000){
		return capacity * 0.5F;
	}else if(rSense == 2000){
		return capacity * 0.25F;
	}else{
		//return -1;	
		return capacity * ((156250 / rSense ) / 10) / 100;		
	}
}
	
uint16_t MAX17320::_readVoltage(){
		uint16_t voltage = _readReg(REG_VOLTAGE_320, REG_MSB_320);
		//return voltage * 1.25F;
		return voltage * 0.3125F;
}

int MAX17320::_readCurrent(){
	uint16_t rSense = _readReg(0xcf, 0x01);
	int current = (int16_t)_readReg(REG_CURRENT_320, REG_MSB_320);
	if(rSense == 500){		
		return current * 31.25F / 100;
	}else if(rSense == 1000){
		return current * 15.625F / 100;
	}else if(rSense == 2000){
		return current * 7.8125F / 100;
	}else{
		//return -1;
		//return current * 28.935F / 100;
		return current * ((156250 / rSense ) / 10) / 100;
	}
}

int MAX17320::_readTemperature(){
	int temperature = (int16_t)_readReg(REG_TEMPERATURE_320, REG_MSB_320);
	return temperature / 256;
}

uint16_t MAX17320::_readCell1(){
	uint16_t cell = _readReg(REG_C1V_320 ,REG_MSB_320);
	return cell * 0.078125F;
}

uint16_t MAX17320::_readCell2(){
	uint16_t cell = _readReg(REG_C2V_320 ,REG_MSB_320);
	return cell * 0.078125F;
}

uint16_t MAX17320::_readCell3(){
	uint16_t cell = _readReg(REG_C3V_320 ,REG_MSB_320);
	return cell * 0.078125F;
}

uint16_t MAX17320::_readCell4(){
	uint16_t cell = _readReg(REG_C4V_320 ,REG_MSB_320);
	return cell * 0.078125F;
}

uint8_t MAX17320::_readSerial(){
	/*char *str;
	uint16_t sn;
	for(int i=0; i<6; i++){
		sn = _readReg(0xd8+i, 0x01);
		sn |= _readReg(0xd8+i, 0x01)<<8;
		*str++ = sn;
	}	
	return str;*/
	uint8_t addr[8];
	_max320.reset_search();
	if(!_max320.search(addr)){
		_max320.reset_search();
		return 0;
	}else{
		//for(int i=0; i<8; i++);
		return addr[7];
	}
}

String MAX17320::GetData()
{	
	String comm = ",";
	String data = "";
	uint16_t voltage,c1v,c2v,c3v,c4v,fCap,capacity;
	uint8_t ser;
	int temperature, current, stat, date;
	
	voltage = _readVoltage();
	current = _readCurrent();
	capacity = _readCapacity();
	fCap = _readFCap();
	temperature = _readTemperature();
	ser = _readSerial();
	stat = 0;
	date = 0;
	uint8_t num  = _cellNum();
	switch(num){
		case 2:
			c1v = _readCell1();
			c2v = _readCell2();
			c3v = 0;
			c4v = 0;
			break;
		case 3:
			c1v = _readCell1();
			c2v = _readCell2();
			c3v = _readCell3();
			c4v = 0;
			break;
		case 4:
			c1v = _readCell1();
			c2v = _readCell2();
			c3v = _readCell3();
			c4v = _readCell4();	
			break;
		default:
			c1v = 0;
			c2v = 0;
			c3v = 0;
			c4v = 0;
			break;
	}
	{
		data = voltage+comm+current+comm+temperature+comm+capacity+comm+c1v+comm+c2v+comm+c3v+comm+c4v+comm+ser+comm+stat+comm+date+comm+fCap;
	}
	return data;
}
