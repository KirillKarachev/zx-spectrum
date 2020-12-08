#ifndef MEMORY_H_
#define MEMORY_H_

#include <vector>
#include <cstdint>
#include <random>

#include "IO.h"

class Memory{
protected:
	std::vector<uint8_t> _contents;
public:
	virtual ~Memory() = default;

	virtual void write(unsigned addr, uint8_t value) = 0;
	virtual uint8_t read(unsigned addr) { return _contents[addr % _contents.size()]; }
};

class RAM: public Memory{
protected:
	unsigned _width;
	unsigned _size;
	unsigned _mask;
public:
	RAM(unsigned width): _width(width){
		_size = 1 << width;
		_mask = _size - 1;
		_contents.resize(_size);

		std::minstd_rand rnd;
		for (auto && b : _contents) {
			b = rnd() & 0xff;
		}
	}

	virtual void write(unsigned addr, uint8_t value) override {
		_contents[addr & _mask] = value;
	}

	unsigned size() const { return _size; }
	unsigned width() const { return _width; }
	unsigned mask() const { return _mask; }
};

class ROM: public Memory{
protected:
	uint16_t _addr_mask;
public:
	ROM(const char * filename);
	virtual void write(unsigned addr, uint8_t value) override { }
	virtual uint8_t read(unsigned addr) override {
		return Memory::read(addr & _addr_mask);
	}

};

class AddressSpace{
protected:
	RAM & _ram;
	ROM & _rom;
	IO & _io;
public:
	AddressSpace(RAM & ram, ROM & rom, IO & io): _ram(ram), _rom(rom), _io(io) {}

	void write(unsigned address, uint8_t value, bool io = false);
	uint8_t read(unsigned address, bool io = false);

	void write16(unsigned address, uint16_t value);
	uint16_t read16(unsigned address);

};


#endif /* MEMORY_H_ */
