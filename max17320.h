#ifndef MAX17320_h
#define MAX17320_h

#include <OneWire.h>

#define REG_MSB_320				0x00
#define REG_CAPACITY_320				0x06
#define REG_VOLTAGE_320				0xda
#define REG_CURRENT_320				0x1c
#define REG_TEMPERATURE_320			0x1b
#define	REG_C1V_320					0xd8
#define	REG_C2V_320					0xd7
#define	REG_C3V_320					0xd6
#define	REG_C4V_320					0xd5
#define	REG_FCAP_320					0x05
#define REG_TYPE_320					0x21

class MAX17320{
	public:
		uint16_t _readCapacity();
		uint16_t _readVoltage();
		uint16_t _readCell1();
		uint16_t _readCell2();
		uint16_t _readCell3();
		uint16_t _readCell4();
		uint16_t _readFCap();
		uint8_t _readType();
		int _readCurrent();
		int _readTemperature();
		String GetData(void);
		uint8_t _scan();
		void _writeData(uint8_t, uint8_t, uint8_t*);
		void _fReset();
		uint8_t _readSerial();
		uint8_t _cellNum();
	private:
		uint16_t _readReg(uint8_t, uint8_t);
};

#endif
		
	