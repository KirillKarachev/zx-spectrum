#include <fstream>
#include "Memory.h"

ROM::ROM(const char * filename)
{
	std::fstream romfile;
	romfile.open(filename, std::ios::in | std::ios::ate);
	_contents.resize(romfile.tellg());
	romfile.seekg(0);
	romfile.read(reinterpret_cast<char*>(&_contents[0]), _contents.size());
	_addr_mask = _contents.size() - 1;
}

void AddressSpace::write(unsigned address, uint8_t value, bool io)
{
	if (io == true)
		_io.write(address, value);
	else
		_ram.write(address, value);
}

uint8_t AddressSpace::read(unsigned address, bool io)
{
	if (io == true)
		return _io.read(address);
	else {
		if (address < 16384)
			return _rom.read(address);
		else
			return _ram.read(address);
	}
}

void AddressSpace::write16(unsigned address, uint16_t value)
{
	write(address,     (value >> 0) & 0x00ff);
	write(address + 1, (value >> 8) & 0x00ff);
}

uint16_t AddressSpace::read16(unsigned address)
{
	uint16_t result = 0;
	result |= read(address);
	result |= uint16_t(read(address)) << 8;
	return result;
}
