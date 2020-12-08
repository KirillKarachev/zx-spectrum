#ifndef Z80CPU_H_
#define Z80CPU_H_

#include "Memory.h"
#include "libz80\z80.h"

 byte In_mem	(void* param, ushort address);
 void Out_mem (void* param, ushort address, byte data);
 byte In_io	(void* param, ushort address);
 void Out_io (void* param, ushort address, byte data);


class Z80CPU
{
protected:
	AddressSpace & _bus;
	Z80Context _context {};
	friend byte In_mem	(void* param, ushort address);
	friend void Out_mem (void* param, ushort address, byte data);
	friend byte In_io	(void* param, ushort address);
	friend void Out_io (void* param, ushort address, byte data);

public:
	Z80CPU(AddressSpace & bus): _bus(bus) {
		_context.memRead = In_mem;
		_context.memWrite = Out_mem;
		_context.ioRead = In_io;
		_context.ioWrite = Out_io;
		_context.ioParam = this;
		_context.memParam = this;
	}
	void tick() {
		Z80Execute (&_context);
	}
	void ticks(unsigned ticks) {
		Z80ExecuteTStates(&_context, ticks);
	}
	void reset() {
		Z80RESET(&_context);
	}
	void intr(byte value) {
		Z80INT(&_context,value);
	}
	void nmi() {
		Z80NMI(&_context);
	}

	void save_state_sna(const char * filename);
	void load_state_sna(const char * filename);
	void load_state_z80(const char * filename);

	void load_state_sna_libspectrum(const char * filename);
	void load_state_z80_libspectrum(const char * filename);

};

#endif /* Z80CPU_H_ */
