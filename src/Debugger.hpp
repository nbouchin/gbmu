#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "src/Fwd.hpp"
#include "cpu/Core.hpp"
#include "src/MemoryBus.hpp"
#include "src/Cartridge.hpp"
#include <vector>
#include <map>
#include <chrono>

class Debugger {
	public:
		Debugger(ComponentsContainer &components);
		struct _instr_info {
			const char * instr = nullptr;
			uint8_t size = 0x00;
		};


		struct _debug_info {
			_debug_info(uint16_t pc, const _instr_info &map_info, Core::Iterator it, uint8_t size);

			uint16_t pc = 0x00;
			const char * instr = nullptr;
			Byte value[3] = {0, 0, 0};
			uint8_t size = 0x00;
		};

	private:

		ComponentsContainer &_components;
		bool	_enabled = true;
		bool	_lock = true;
		bool	_run_one_frame = false;
		bool	_run_one_sec = false;
		bool	_run_one_step = false;
		int		_frame_size = 10;
		std::chrono::time_point<std::chrono::high_resolution_clock> _past;

		std::vector<_debug_info> _instr_pool;
		std::vector<uint16_t> _breakpoint_pool;
		std::vector<uint16_t> _register_pool;
		std::vector<std::pair<int, uint16_t>> _register_diffs;

		std::map<int, _instr_info> _instr_map = {
			{0x0, {"NOP", 1}}, {0x1, {"LD BC d16", 3}}, {0x2, {"LD (BC) A", 1}}, 
			{0x3, {"INC BC", 1}}, {0x4, {"INC B", 1}}, {0x5, {"DEC B", 1}}, 
			{0x6, {"LD B d8", 2}}, {0x7, {"RLCA", 1}}, {0x8, {"LD (a16) SP", 3}}, 
			{0x9, {"ADD HL BC", 1}}, {0xA, {"LD A (BC)", 1}}, {0xB, {"DEC BC", 1}}, 
			{0xC, {"INC C", 1}}, {0xD, {"DEC C", 1}}, {0xE, {"LD C d8", 2}}, 
			{0xF, {"RRCA", 1}}, {0x10, {"STOP 0", 2}}, {0x11, {"LD DE d16", 3}}, 
			{0x12, {"LD (DE) A", 1}}, {0x13, {"INC DE", 1}}, {0x14, {"INC D", 1}}, 
			{0x15, {"DEC D", 1}}, {0x16, {"LD D d8", 2}}, {0x17, {"RLA", 1}}, 
			{0x18, {"JR r8", 2}}, {0x19, {"ADD HL DE", 1}}, {0x1A, {"LD A (DE)", 1}}, 
			{0x1B, {"DEC DE", 1}}, {0x1C, {"INC E", 1}}, {0x1D, {"DEC E", 1}}, 
			{0x1E, {"LD E d8", 2}}, {0x1F, {"RRA", 1}}, {0x20, {"JR NZ r8", 2}}, 
			{0x21, {"LD HL d16", 3}}, {0x22, {"LD (HL+) A", 1}}, {0x23, {"INC HL", 1}}, 
			{0x24, {"INC H", 1}}, {0x25, {"DEC H", 1}}, {0x26, {"LD H d8", 2}}, 
			{0x27, {"DAA", 1}}, {0x28, {"JR Z r8", 2}}, {0x29, {"ADD HL HL", 1}}, 
			{0x2A, {"LD A (HL+)", 1}}, {0x2B, {"DEC HL", 1}}, {0x2C, {"INC L", 1}}, 
			{0x2D, {"DEC L", 1}}, {0x2E, {"LD L d8", 2}}, {0x2F, {"CPL", 1}}, 
			{0x30, {"JR NC r8", 2}}, {0x31, {"LD SP d16", 3}}, {0x32, {"LD (HL-) A", 1}}, 
			{0x33, {"INC SP", 1}}, {0x34, {"INC (HL)", 1}}, {0x35, {"DEC (HL)", 1}}, 
			{0x36, {"LD (HL) d8", 2}}, {0x37, {"SCF", 1}}, {0x38, {"JR C r8", 2}}, 
			{0x39, {"ADD HL SP", 1}}, {0x3A, {"LD A (HL-)", 1}}, {0x3B, {"DEC SP", 1}}, 
			{0x3C, {"INC A", 1}}, {0x3D, {"DEC A", 1}}, {0x3E, {"LD A d8", 2}}, 
			{0x3F, {"CCF", 1}}, {0x40, {"LD B B", 1}}, {0x41, {"LD B C", 1}}, 
			{0x42, {"LD B D", 1}}, {0x43, {"LD B E", 1}}, {0x44, {"LD B H", 1}}, 
			{0x45, {"LD B L", 1}}, {0x46, {"LD B (HL)", 1}}, {0x47, {"LD B A", 1}}, 
			{0x48, {"LD C B", 1}}, {0x49, {"LD C C", 1}}, {0x4A, {"LD C D", 1}}, 
			{0x4B, {"LD C E", 1}}, {0x4C, {"LD C H", 1}}, {0x4D, {"LD C L", 1}}, 
			{0x4E, {"LD C (HL)", 1}}, {0x4F, {"LD C A", 1}}, {0x50, {"LD D B", 1}}, 
			{0x51, {"LD D C", 1}}, {0x52, {"LD D D", 1}}, {0x53, {"LD D E", 1}}, 
			{0x54, {"LD D H", 1}}, {0x55, {"LD D L", 1}}, {0x56, {"LD D (HL)", 1}}, 
			{0x57, {"LD D A", 1}}, {0x58, {"LD E B", 1}}, {0x59, {"LD E C", 1}}, 
			{0x5A, {"LD E D", 1}}, {0x5B, {"LD E E", 1}}, {0x5C, {"LD E H", 1}}, 
			{0x5D, {"LD E L", 1}}, {0x5E, {"LD E (HL)", 1}}, {0x5F, {"LD E A", 1}}, 
			{0x60, {"LD H B", 1}}, {0x61, {"LD H C", 1}}, {0x62, {"LD H D", 1}}, 
			{0x63, {"LD H E", 1}}, {0x64, {"LD H H", 1}}, {0x65, {"LD H L", 1}}, 
			{0x66, {"LD H (HL)", 1}}, {0x67, {"LD H A", 1}}, {0x68, {"LD L B", 1}}, 
			{0x69, {"LD L C", 1}}, {0x6A, {"LD L D", 1}}, {0x6B, {"LD L E", 1}}, 
			{0x6C, {"LD L H", 1}}, {0x6D, {"LD L L", 1}}, {0x6E, {"LD L (HL)", 1}}, 
			{0x6F, {"LD L A", 1}}, {0x70, {"LD (HL) B", 1}}, {0x71, {"LD (HL) C", 1}}, 
			{0x72, {"LD (HL) D", 1}}, {0x73, {"LD (HL) E", 1}}, {0x74, {"LD (HL) H", 1}}, 
			{0x75, {"LD (HL) L", 1}}, {0x76, {"HALT", 1}}, {0x77, {"LD (HL) A", 1}}, 
			{0x78, {"LD A B", 1}}, {0x79, {"LD A C", 1}}, {0x7A, {"LD A D", 1}}, 
			{0x7B, {"LD A E", 1}}, {0x7C, {"LD A H", 1}}, {0x7D, {"LD A L", 1}}, 
			{0x7E, {"LD A (HL)", 1}}, {0x7F, {"LD A A", 1}}, {0x80, {"ADD A B", 1}}, 
			{0x81, {"ADD A C", 1}}, {0x82, {"ADD A D", 1}}, {0x83, {"ADD A E", 1}}, 
			{0x84, {"ADD A H", 1}}, {0x85, {"ADD A L", 1}}, {0x86, {"ADD A (HL)", 1}}, 
			{0x87, {"ADD A A", 1}}, {0x88, {"ADC A B", 1}}, {0x89, {"ADC A C", 1}}, 
			{0x8A, {"ADC A D", 1}}, {0x8B, {"ADC A E", 1}}, {0x8C, {"ADC A H", 1}}, 
			{0x8D, {"ADC A L", 1}}, {0x8E, {"ADC A (HL)", 1}}, {0x8F, {"ADC A A", 1}}, 
			{0x90, {"SUB B", 1}}, {0x91, {"SUB C", 1}}, {0x92, {"SUB D", 1}}, 
			{0x93, {"SUB E", 1}}, {0x94, {"SUB H", 1}}, {0x95, {"SUB L", 1}}, 
			{0x96, {"SUB (HL)", 1}}, {0x97, {"SUB A", 1}}, {0x98, {"SBC A B", 1}}, 
			{0x99, {"SBC A C", 1}}, {0x9A, {"SBC A D", 1}}, {0x9B, {"SBC A E", 1}}, 
			{0x9C, {"SBC A H", 1}}, {0x9D, {"SBC A L", 1}}, {0x9E, {"SBC A (HL)", 1}}, 
			{0x9F, {"SBC A A", 1}}, {0xA0, {"AND B", 1}}, {0xA1, {"AND C", 1}}, 
			{0xA2, {"AND D", 1}}, {0xA3, {"AND E", 1}}, {0xA4, {"AND H", 1}}, 
			{0xA5, {"AND L", 1}}, {0xA6, {"AND (HL)", 1}}, {0xA7, {"AND A", 1}}, 
			{0xA8, {"XOR B", 1}}, {0xA9, {"XOR C", 1}}, {0xAA, {"XOR D", 1}}, 
			{0xAB, {"XOR E", 1}}, {0xAC, {"XOR H", 1}}, {0xAD, {"XOR L", 1}}, 
			{0xAE, {"XOR (HL)", 1}}, {0xAF, {"XOR A", 1}}, {0xB0, {"OR B", 1}}, 
			{0xB1, {"OR C", 1}}, {0xB2, {"OR D", 1}}, {0xB3, {"OR E", 1}}, 
			{0xB4, {"OR H", 1}}, {0xB5, {"OR L", 1}}, {0xB6, {"OR (HL)", 1}}, 
			{0xB7, {"OR A", 1}}, {0xB8, {"CP B", 1}}, {0xB9, {"CP C", 1}}, 
			{0xBA, {"CP D", 1}}, {0xBB, {"CP E", 1}}, {0xBC, {"CP H", 1}}, 
			{0xBD, {"CP L", 1}}, {0xBE, {"CP (HL)", 1}}, {0xBF, {"CP A", 1}}, 
			{0xC0, {"RET NZ", 1}}, {0xC1, {"POP BC", 1}}, {0xC2, {"JP NZ a16", 3}}, 
			{0xC3, {"JP a16", 3}}, {0xC4, {"CALL NZ a16", 3}}, {0xC5, {"PUSH BC", 1}}, 
			{0xC6, {"ADD A d8", 2}}, {0xC7, {"RST 00H", 1}}, {0xC8, {"RET Z", 1}}, 
			{0xC9, {"RET", 1}}, {0xCA, {"JP Z a16", 3}}, {0xCB, {"PREFIX CB", 1}}, 
			{0xCC, {"CALL Z a16", 3}}, {0xCD, {"CALL a16", 3}}, {0xCE, {"ADC A d8", 2}}, 
			{0xCF, {"RST 08H", 1}}, {0xD0, {"RET NC", 1}}, {0xD1, {"POP DE", 1}}, 
			{0xD2, {"JP NC a16", 3}}, {0xD4, {"CALL NC a16", 3}}, {0xD5, {"PUSH DE", 1}}, 
			{0xD6, {"SUB d8", 2}}, {0xD7, {"RST 10H", 1}}, {0xD8, {"RET C", 1}}, 
			{0xD9, {"RETI", 1}}, {0xDA, {"JP C a16", 3}}, {0xDC, {"CALL C a16", 3}}, 
			{0xDE, {"SBC A d8", 2}}, {0xDF, {"RST 18H", 1}}, {0xE0, {"LDH (a8) A", 2}}, 
			{0xE1, {"POP HL", 1}}, {0xE2, {"LD (C) A", 2}}, {0xE5, {"PUSH HL", 1}}, 
			{0xE6, {"AND d8", 2}}, {0xE7, {"RST 20H", 1}}, {0xE8, {"ADD SP r8", 2}}, 
			{0xE9, {"JP (HL)", 1}}, {0xEA, {"LD (a16) A", 3}}, {0xEE, {"XOR d8", 2}}, 
			{0xEF, {"RST 28H", 1}}, {0xF0, {"LDH A (a8)", 2}}, {0xF1, {"POP AF", 1}}, 
			{0xF2, {"LD A (C)", 2}}, {0xF3, {"DI", 1}}, {0xF5, {"PUSH AF", 1}}, 
			{0xF6, {"OR d8", 2}}, {0xF7, {"RST 30H", 1}}, {0xF8, {"LD HL SP+r8", 2}}, 
			{0xF9, {"LD SP HL", 1}}, {0xFA, {"LD A (a16)", 3}}, {0xFB, {"EI", 1}}, 
			{0xFE, {"CP d8", 2}}, {0xFF, {"RST 38H", 1}}, {0xCB0, {"RLC B", 2}}, 
			{0xCB1, {"RLC C", 2}}, {0xCB2, {"RLC D", 2}}, {0xCB3, {"RLC E", 2}}, 
			{0xCB4, {"RLC H", 2}}, {0xCB5, {"RLC L", 2}}, {0xCB6, {"RLC (HL)", 2}}, 
			{0xCB7, {"RLC A", 2}}, {0xCB8, {"RRC B", 2}}, {0xCB9, {"RRC C", 2}}, 
			{0xCBA, {"RRC D", 2}}, {0xCBB, {"RRC E", 2}}, {0xCBC, {"RRC H", 2}}, 
			{0xCBD, {"RRC L", 2}}, {0xCBE, {"RRC (HL)", 2}}, {0xCBF, {"RRC A", 2}}, 
			{0xCB10, {"RL B", 2}}, {0xCB11, {"RL C", 2}}, {0xCB12, {"RL D", 2}}, 
			{0xCB13, {"RL E", 2}}, {0xCB14, {"RL H", 2}}, {0xCB15, {"RL L", 2}}, 
			{0xCB16, {"RL (HL)", 2}}, {0xCB17, {"RL A", 2}}, {0xCB18, {"RR B", 2}}, 
			{0xCB19, {"RR C", 2}}, {0xCB1A, {"RR D", 2}}, {0xCB1B, {"RR E", 2}}, 
			{0xCB1C, {"RR H", 2}}, {0xCB1D, {"RR L", 2}}, {0xCB1E, {"RR (HL)", 2}}, 
			{0xCB1F, {"RR A", 2}}, {0xCB20, {"SLA B", 2}}, {0xCB21, {"SLA C", 2}}, 
			{0xCB22, {"SLA D", 2}}, {0xCB23, {"SLA E", 2}}, {0xCB24, {"SLA H", 2}}, 
			{0xCB25, {"SLA L", 2}}, {0xCB26, {"SLA (HL)", 2}}, {0xCB27, {"SLA A", 2}}, 
			{0xCB28, {"SRA B", 2}}, {0xCB29, {"SRA C", 2}}, {0xCB2A, {"SRA D", 2}}, 
			{0xCB2B, {"SRA E", 2}}, {0xCB2C, {"SRA H", 2}}, {0xCB2D, {"SRA L", 2}}, 
			{0xCB2E, {"SRA (HL)", 2}}, {0xCB2F, {"SRA A", 2}}, {0xCB30, {"SWAP B", 2}}, 
			{0xCB31, {"SWAP C", 2}}, {0xCB32, {"SWAP D", 2}}, {0xCB33, {"SWAP E", 2}}, 
			{0xCB34, {"SWAP H", 2}}, {0xCB35, {"SWAP L", 2}}, {0xCB36, {"SWAP (HL)", 2}}, 
			{0xCB37, {"SWAP A", 2}}, {0xCB38, {"SRL B", 2}}, {0xCB39, {"SRL C", 2}}, 
			{0xCB3A, {"SRL D", 2}}, {0xCB3B, {"SRL E", 2}}, {0xCB3C, {"SRL H", 2}}, 
			{0xCB3D, {"SRL L", 2}}, {0xCB3E, {"SRL (HL)", 2}}, {0xCB3F, {"SRL A", 2}}, 
			{0xCB40, {"BIT 0, B", 2}}, {0xCB41, {"BIT 0, C", 2}}, {0xCB42, {"BIT 0, D", 2}}, 
			{0xCB43, {"BIT 0, E", 2}}, {0xCB44, {"BIT 0, H", 2}}, {0xCB45, {"BIT 0, L", 2}}, 
			{0xCB46, {"BIT 0, (HL)", 2}}, {0xCB47, {"BIT 0, A", 2}}, {0xCB48, {"BIT 1, B", 2}}, 
			{0xCB49, {"BIT 1, C", 2}}, {0xCB4A, {"BIT 1, D", 2}}, {0xCB4B, {"BIT 1, E", 2}}, 
			{0xCB4C, {"BIT 1, H", 2}}, {0xCB4D, {"BIT 1, L", 2}}, {0xCB4E, {"BIT 1, (HL)", 2}}, 
			{0xCB4F, {"BIT 1, A", 2}}, {0xCB50, {"BIT 2, B", 2}}, {0xCB51, {"BIT 2, C", 2}}, 
			{0xCB52, {"BIT 2, D", 2}}, {0xCB53, {"BIT 2, E", 2}}, {0xCB54, {"BIT 2, H", 2}}, 
			{0xCB55, {"BIT 2, L", 2}}, {0xCB56, {"BIT 2, (HL)", 2}}, {0xCB57, {"BIT 2, A", 2}}, 
			{0xCB58, {"BIT 3, B", 2}}, {0xCB59, {"BIT 3, C", 2}}, {0xCB5A, {"BIT 3, D", 2}}, 
			{0xCB5B, {"BIT 3, E", 2}}, {0xCB5C, {"BIT 3, H", 2}}, {0xCB5D, {"BIT 3, L", 2}}, 
			{0xCB5E, {"BIT 3, (HL)", 2}}, {0xCB5F, {"BIT 3, A", 2}}, {0xCB60, {"BIT 4, B", 2}}, 
			{0xCB61, {"BIT 4, C", 2}}, {0xCB62, {"BIT 4, D", 2}}, {0xCB63, {"BIT 4, E", 2}}, 
			{0xCB64, {"BIT 4, H", 2}}, {0xCB65, {"BIT 4, L", 2}}, {0xCB66, {"BIT 4, (HL)", 2}}, 
			{0xCB67, {"BIT 4, A", 2}}, {0xCB68, {"BIT 5, B", 2}}, {0xCB69, {"BIT 5, C", 2}}, 
			{0xCB6A, {"BIT 5, D", 2}}, {0xCB6B, {"BIT 5, E", 2}}, {0xCB6C, {"BIT 5, H", 2}}, 
			{0xCB6D, {"BIT 5, L", 2}}, {0xCB6E, {"BIT 5, (HL)", 2}}, {0xCB6F, {"BIT 5, A", 2}}, 
			{0xCB70, {"BIT 6, B", 2}}, {0xCB71, {"BIT 6, C", 2}}, {0xCB72, {"BIT 6, D", 2}}, 
			{0xCB73, {"BIT 6, E", 2}}, {0xCB74, {"BIT 6, H", 2}}, {0xCB75, {"BIT 6, L", 2}}, 
			{0xCB76, {"BIT 6, (HL)", 2}}, {0xCB77, {"BIT 6, A", 2}}, {0xCB78, {"BIT 7, B", 2}}, 
			{0xCB79, {"BIT 7, C", 2}}, {0xCB7A, {"BIT 7, D", 2}}, {0xCB7B, {"BIT 7, E", 2}}, 
			{0xCB7C, {"BIT 7, H", 2}}, {0xCB7D, {"BIT 7, L", 2}}, {0xCB7E, {"BIT 7, (HL)", 2}}, 
			{0xCB7F, {"BIT 7, A", 2}}, {0xCB80, {"RES 0, B", 2}}, {0xCB81, {"RES 0, C", 2}}, 
			{0xCB82, {"RES 0, D", 2}}, {0xCB83, {"RES 0, E", 2}}, {0xCB84, {"RES 0, H", 2}}, 
			{0xCB85, {"RES 0, L", 2}}, {0xCB86, {"RES 0, (HL)", 2}}, {0xCB87, {"RES 0, A", 2}}, 
			{0xCB88, {"RES 1, B", 2}}, {0xCB89, {"RES 1, C", 2}}, {0xCB8A, {"RES 1, D", 2}}, 
			{0xCB8B, {"RES 1, E", 2}}, {0xCB8C, {"RES 1, H", 2}}, {0xCB8D, {"RES 1, L", 2}}, 
			{0xCB8E, {"RES 1, (HL)", 2}}, {0xCB8F, {"RES 1, A", 2}}, {0xCB90, {"RES 2, B", 2}}, 
			{0xCB91, {"RES 2, C", 2}}, {0xCB92, {"RES 2, D", 2}}, {0xCB93, {"RES 2, E", 2}}, 
			{0xCB94, {"RES 2, H", 2}}, {0xCB95, {"RES 2, L", 2}}, {0xCB96, {"RES 2, (HL)", 2}}, 
			{0xCB97, {"RES 2, A", 2}}, {0xCB98, {"RES 3, B", 2}}, {0xCB99, {"RES 3, C", 2}}, 
			{0xCB9A, {"RES 3, D", 2}}, {0xCB9B, {"RES 3, E", 2}}, {0xCB9C, {"RES 3, H", 2}}, 
			{0xCB9D, {"RES 3, L", 2}}, {0xCB9E, {"RES 3, (HL)", 2}}, {0xCB9F, {"RES 3, A", 2}}, 
			{0xCBA0, {"RES 4, B", 2}}, {0xCBA1, {"RES 4, C", 2}}, {0xCBA2, {"RES 4, D", 2}}, 
			{0xCBA3, {"RES 4, E", 2}}, {0xCBA4, {"RES 4, H", 2}}, {0xCBA5, {"RES 4, L", 2}}, 
			{0xCBA6, {"RES 4, (HL)", 2}}, {0xCBA7, {"RES 4, A", 2}}, {0xCBA8, {"RES 5, B", 2}}, 
			{0xCBA9, {"RES 5, C", 2}}, {0xCBAA, {"RES 5, D", 2}}, {0xCBAB, {"RES 5, E", 2}}, 
			{0xCBAC, {"RES 5, H", 2}}, {0xCBAD, {"RES 5, L", 2}}, {0xCBAE, {"RES 5, (HL)", 2}}, 
			{0xCBAF, {"RES 5, A", 2}}, {0xCBB0, {"RES 6, B", 2}}, {0xCBB1, {"RES 6, C", 2}}, 
			{0xCBB2, {"RES 6, D", 2}}, {0xCBB3, {"RES 6, E", 2}}, {0xCBB4, {"RES 6, H", 2}}, 
			{0xCBB5, {"RES 6, L", 2}}, {0xCBB6, {"RES 6, (HL)", 2}}, {0xCBB7, {"RES 6, A", 2}}, 
			{0xCBB8, {"RES 7, B", 2}}, {0xCBB9, {"RES 7, C", 2}}, {0xCBBA, {"RES 7, D", 2}}, 
			{0xCBBB, {"RES 7, E", 2}}, {0xCBBC, {"RES 7, H", 2}}, {0xCBBD, {"RES 7, L", 2}}, 
			{0xCBBE, {"RES 7, (HL)", 2}}, {0xCBBF, {"RES 7, A", 2}}, {0xCBC0, {"SET 0, B", 2}}, 
			{0xCBC1, {"SET 0, C", 2}}, {0xCBC2, {"SET 0, D", 2}}, {0xCBC3, {"SET 0, E", 2}}, 
			{0xCBC4, {"SET 0, H", 2}}, {0xCBC5, {"SET 0, L", 2}}, {0xCBC6, {"SET 0, (HL)", 2}}, 
			{0xCBC7, {"SET 0, A", 2}}, {0xCBC8, {"SET 1, B", 2}}, {0xCBC9, {"SET 1, C", 2}}, 
			{0xCBCA, {"SET 1, D", 2}}, {0xCBCB, {"SET 1, E", 2}}, {0xCBCC, {"SET 1, H", 2}}, 
			{0xCBCD, {"SET 1, L", 2}}, {0xCBCE, {"SET 1, (HL)", 2}}, {0xCBCF, {"SET 1, A", 2}}, 
			{0xCBD0, {"SET 2, B", 2}}, {0xCBD1, {"SET 2, C", 2}}, {0xCBD2, {"SET 2, D", 2}}, 
			{0xCBD3, {"SET 2, E", 2}}, {0xCBD4, {"SET 2, H", 2}}, {0xCBD5, {"SET 2, L", 2}}, 
			{0xCBD6, {"SET 2, (HL)", 2}}, {0xCBD7, {"SET 2, A", 2}}, {0xCBD8, {"SET 3, B", 2}}, 
			{0xCBD9, {"SET 3, C", 2}}, {0xCBDA, {"SET 3, D", 2}}, {0xCBDB, {"SET 3, E", 2}}, 
			{0xCBDC, {"SET 3, H", 2}}, {0xCBDD, {"SET 3, L", 2}}, {0xCBDE, {"SET 3, (HL)", 2}}, 
			{0xCBDF, {"SET 3, A", 2}}, {0xCBE0, {"SET 4, B", 2}}, {0xCBE1, {"SET 4, C", 2}}, 
			{0xCBE2, {"SET 4, D", 2}}, {0xCBE3, {"SET 4, E", 2}}, {0xCBE4, {"SET 4, H", 2}}, 
			{0xCBE5, {"SET 4, L", 2}}, {0xCBE6, {"SET 4, (HL)", 2}}, {0xCBE7, {"SET 4, A", 2}}, 
			{0xCBE8, {"SET 5, B", 2}}, {0xCBE9, {"SET 5, C", 2}}, {0xCBEA, {"SET 5, D", 2}}, 
			{0xCBEB, {"SET 5, E", 2}}, {0xCBEC, {"SET 5, H", 2}}, {0xCBED, {"SET 5, L", 2}}, 
			{0xCBEE, {"SET 5, (HL)", 2}}, {0xCBEF, {"SET 5, A", 2}}, {0xCBF0, {"SET 6, B", 2}}, 
			{0xCBF1, {"SET 6, C", 2}}, {0xCBF2, {"SET 6, D", 2}}, {0xCBF3, {"SET 6, E", 2}}, 
			{0xCBF4, {"SET 6, H", 2}}, {0xCBF5, {"SET 6, L", 2}}, {0xCBF6, {"SET 6, (HL)", 2}}, 
			{0xCBF7, {"SET 6, A", 2}}, {0xCBF8, {"SET 7, B", 2}}, {0xCBF9, {"SET 7, C", 2}}, 
			{0xCBFA, {"SET 7, D", 2}}, {0xCBFB, {"SET 7, E", 2}}, {0xCBFC, {"SET 7, H", 2}}, 
			{0xCBFD, {"SET 7, L", 2}}, {0xCBFE, {"SET 7, (HL)", 2}}, {0xCBFF, {"SET 7, A", 2}}, 
		};

	public:
		//information fetch
		//TODO: get_mem_dump();
		void set_instruction_pool_size(int size) {_frame_size = size;}
		const std::vector<std::pair<int, uint16_t>> get_register_diffs();
		const std::vector<_debug_info> & get_instruction_pool() const {return _instr_pool;}
		const std::vector<uint16_t> & get_breakpoints() const {return _breakpoint_pool;}

		//event trigger
		void add_breakpoint(uint16_t addr);
		void remove_breakpoint(uint16_t addr);
		void toggle() { _enabled = _enabled ? false : true; }
		void run_one_sec();
		void run_one_frame();
		void run_one_step();

		void fetch(const Core::Iterator &it, uint16_t pc);
		bool is_enabled() const { return _enabled; }
	private:
		bool is_step_passed();
		bool is_sec_passed();
		bool is_frame_passed();
		std::vector<uint16_t> construct_register_pool();
		std::vector<uint8_t> construct_rom_dump(uint16_t addr);
		void set_instruction_pool(const Core::Iterator &it, uint16_t pc);
		void update_data(const Core::Iterator &it, uint16_t pc);
		void unlock_game(uint16_t pc);
		bool on_breakpoint(uint16_t pc);
		using it = std::vector<uint16_t>::const_iterator;
		const std::vector<std::pair<int, uint16_t>> rdiff(it prev_begin, it current_begin);
};

#endif
