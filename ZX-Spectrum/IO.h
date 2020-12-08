#ifndef IO_H_
#define IO_H_

#include <cstdint>
#include "AudioDriver.h"

class IO
{
protected:
	uint8_t _port_fe { 0x00 };
	uint8_t _key_matrix[8] { 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	AudioDriver * _adrv;

public:
	IO(AudioDriver * drv): _adrv(drv) {}

	void write(unsigned address, uint8_t value);
	uint8_t read(unsigned address) const;

	uint8_t border() const { return _port_fe & 0x07; }

	void keydown(unsigned row, unsigned col);
	void keyup(unsigned row, unsigned col);
};





#endif /* IO_H_ */
