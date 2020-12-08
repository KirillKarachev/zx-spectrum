 /*
 * Z80CPU.cc
 *
 *  Created on: 25 июн. 2020 г.
 *      Author: Даниил
 */

#include "Z80CPU.h"
#include <sstream>
#include <iomanip>

static bool par_even(uint8_t v)
{
	uint8_t res = v;
	res ^= (res >> 4);
	res ^= (res >> 2);
	res ^= (res >> 1);
	return (res&1) == 0;
}

void Z80CPU::tick(){

[[maybe_unused]] 	uint8_t f1, f2, f3;
					uint16_t t1;
					uint8_t ub1;
					uint8_t sb1;

	if(_state == 0){
		_instruction = _bus.read(_regs.pc);
		_state = 2;
	} else if (_state == 1){
		_state =2;
	} else if (_state == 2) {

		f1 = (_instruction >> 6) & 0x03;
		f2 = (_instruction >> 3) & 0x07;
		f3 = (_instruction >> 0) & 0x07;

		switch(_instruction){

		case 0x00: //NOP
			_wait = 2;
			_regs.pc ++;

			break;

		case 0x08: //EX AF, AF'
			_wait = 2;
			_regs.pc ++;

			t1 = _regs.af;
			_regs.af = _shadow_regs.af;
			_shadow_regs.af = t1;
			break;

		case 0x18: // JR d
			_wait = 10;
			sb1 = static_cast<int8_t>(_bus.read(_regs.pc +1));
			_regs.pc += sb1;

			break;

//		case 0x10:
//		case 0x0d:
//		case 0x5c:
//		case 0x7d:
//		case 0xcd:
//		case 0x41:
//		case 0x45:
//		case 0x47:
//		case 0x46:
//		case 0x50:
//		case 0x53:
//		case 0x4f:
//		case 0x20:
//		case 0x4c:
//		case 0x4d:
//		case 0x43:
//		case 0x49:
//		case 0xc8:
//		case 0xd4:
//		case 0x56:
//		case 0x54:
//		case 0x52:
//		case 0xc4:
//		case 0x4e:
//		case 0x42:
//		case 0x44:
//		case 0x55:
//		case 0xcf:
//		case 0xc5:
//		case 0x58:

		case 0xc2: // 11 000 010	JC NZ, (nn)
		case 0xca: // 11 001 010	JC Z, (nn)
		case 0xd2: // 11 010 010	JC NC, (nn)
		case 0xda: // 11 011 010	JC C, (nn)
		case 0xe2: // 11 100 010	JC PO, (nn)
		case 0xea: // 11 101 010	JC PE, (nn)
		case 0xf2: // 11 110 010	JC P, (nn)
		case 0xfa: // 11 111 010	JC M, (nn)

		case 0x32: // LD [nn], A
			 _wait = 11;
			 t1 = _bus.read16(_regs.pc + 1);
			 _regs.pc +=3;

			 _bus.write(t1, _regs.a);
			break;

		case 0xa8 ... 0xaf: // XOR A, SSS
			_wait =2;
			_regs.pc ++;

			ub1 = get_reg(f3);
			_regs.a ^=ub1;
			_regs.f = _regs.a & 0xa8;					// Флаги S, 5 и 3, остальное 0
			_regs.f |= (_regs.a == 0) ? 0x40 : 0x00;	// Флаг Z
			_regs.f |= par_even(_regs.a)? 0x04 : 0x00;	// Флаг P/V
			break;

		case 0xf3: //DI
			_wait =2;
			_regs.pc ++;

			_iff1 = false;
			_iff2 = false;
			break;

		default:{
			std::stringstream ss;
			ss << "Unknown instruction " << std::hex <<
					std::setfill('0') <<
					std::setw(2) << unsigned (_instruction) <<
					" @ " <<
					std::setw(4) << unsigned (_regs.pc);
			throw std::runtime_error(ss.str());
			}
		}
		_state = 3;
	} else {
		--_wait;
		if(_wait == 0)
			_state = 0;
	}
}

uint8_t Z80CPU::get_reg(uint8_t reg) const{
	switch(reg){
	case 0: return _regs.b; break;
	case 1: return _regs.c; break;
	case 2: return _regs.d; break;
	case 3: return _regs.e; break;
	case 4: return _regs.h; break;
	case 5: return _regs.l; break;
	case 6: return _bus.read(_regs.hl); break;
	case 7: return _regs.a; break;
	default:
		throw std::runtime_error("unknown register!");
	}
}

void Z80CPU::reset(){
	_regs.pc = 0;
}


