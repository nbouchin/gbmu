#pragma once
#ifndef CORE_HPP
#define CORE_HPP

#include "utils/Operations_utils.hpp"
#include "../MemoryBus.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

using Byte = uint8_t;
using Word = uint16_t;

union Register {
  Word word = 0;
  struct {
    Byte low, high;
  };
};

//static std::array<Byte, 0xFFFF> tmp_memory;
//
//template <typename T>
//T read(Word addr) {
//  T ret = 0;
//  auto i = sizeof(T);
//  while (i > 0) {
//    i--;
//    ret |= tmp_memory[addr + i] >> (i * 8);
//  }
//  return ret;
//}
//template <typename T>
//void write(Word addr, T v) {
//  auto i = sizeof(T);
//  while (i > 0) {
//    i--;
//    tmp_memory[addr + i] = v >> (i * 8);
//  }
//}

class Accessor;
class MemoryBus;
class InterruptController;

class Core {
 public:
  enum class Flags { C = 0x10, H = 0x20, N = 0x40, Z = 0x80 };

  Core(MemoryBus *b, InterruptController *ic) : mem_bus(b), ic(ic){}

  friend class Accessor;

 private:
  Register _pc = {.word = 0x0100};
  Register _sp = {.word = 0xfffe};
  Register _af = {.word = 0x01b0};
  Register _bc = {.word = 0x0013};
  Register _de = {.word = 0x00d8};
  Register _hl = {.word = 0x014d};
  Word _clock = 0x00;
  bool _in_jump_state = false;

  MemoryBus *mem_bus;
  InterruptController *ic;

  void exec_instruction(std::function<void(void)> instr, Byte clock_cycles) {
    instr();
    _clock += clock_cycles;
  }

  void exec_instruction(std::function<void(Byte&)> instr, Word addr,
                        Byte clock_cycles) {
    Byte b = mem_bus->read<Byte>(addr);
    instr(b);
    mem_bus->write<Byte>(addr, b);
    _clock += clock_cycles;
  }

  void exec_instruction(std::function<void(Word&)> instr, Word addr,
                        Byte clock_cycles) {
    Word b = mem_bus->read<Word>(addr);
    instr(b);
    mem_bus->write<Word>(addr, b);
    _clock += clock_cycles;
  }

 public:
  void set_flag(Flags f, bool v) {
    int mask = static_cast<int>(f);
    if (v)
      _af.low |= mask;
    else if (!v && _af.low & mask)
      _af.low ^= mask;
  }

 public:
  template <typename T>
  void instr_ld(T& a, T b) {
    a = b;
  }
  void instr_ldd(Byte&, Byte);
  void instr_ldi(Byte&, Byte);
  void instr_ldhl(Byte a);
  void instr_push(Word v);
  void instr_pop(Word& dest);

  template <typename A, typename B>
  void instr_add(A& a, B b);
  void instr_adc(Byte&, Byte);
  void instr_sub(Byte&, Byte);
  void instr_sbc(Byte&, Byte);
  void instr_and(Byte&, Byte);
  void instr_or(Byte&, Byte);
  void instr_xor(Byte&, Byte);
  void instr_cp(Byte&, Byte);

  void instr_inc(Byte&);
  void instr_inc(Word&);
  void instr_dec(Byte&);
  void instr_dec(Word&);

  void instr_daa();
  void instr_cpl();
  void instr_ccf();
  void instr_scf();
  void instr_nop() {}
  void instr_halt() {}
  void instr_stop() {}
  void instr_di();
  void instr_ei();

  enum class JumpCondition { None, NonZero, Zero, NonCarry, Carry };
  bool is_condition_fulfilled(JumpCondition);
  void instr_jp(JumpCondition, Word);
  void instr_jr(JumpCondition, Byte);
  void instr_call(JumpCondition, Word);
  void instr_ret(JumpCondition);
  void instr_reti();
  void instr_rst(Byte);

  void flag_handle(std::function<void(Byte&)> action, Byte& reg);
  void instr_rlc(Byte&);
  void instr_rl(Byte&);
  void instr_rrc(Byte&);
  void instr_rr(Byte&);
  void instr_sla(Byte&);
  void instr_sra(Byte&);
  void instr_srl(Byte&);

  void instr_rlca() { instr_rlc(_af.high); }
  void instr_rla() { instr_rl(_af.high); }
  void instr_rrca() { instr_rrc(_af.high); }
  void instr_rra() { instr_rr(_af.high); }

  void instr_swap(Byte&);
  void instr_bit(Byte, Byte);
  void instr_set(Byte&, Byte);
  void instr_res(Byte&, Byte);

  bool get_flag(Flags f) const { return _af.low & static_cast<int>(f); }
  auto clock() const { return _clock; }
  Word pc() const { return _pc.word; }
  Word sp() const { return _sp.word; }
  Register af() const { return _af; };
  Register bc() const { return _bc; };
  Register de() const { return _de; };
  Register hl() const { return _hl; };
  bool in_jump_state() const { return _in_jump_state; }

  using Iterator = std::vector<Byte>::iterator;
  void execute(Iterator it);
};

template <>
inline void Core::instr_add<Byte, Byte>(Byte& a, Byte b) {
  _af.low = 0u;
  Byte overflowing_nibbles = check_add_overflows(a, b);
  set_flag(Flags::H, test_bit(3, overflowing_nibbles));
  set_flag(Flags::C, test_bit(7, overflowing_nibbles));
  a += b;
  set_flag(Flags::Z, a == 0u);
}

template <>
inline void Core::instr_add<Word, Word>(Word& a, Word b) {
  _af.low = 0u;
  Word overflowing_nibbles = check_add_overflows(a, b);
  set_flag(Flags::H, test_bit(11, overflowing_nibbles));
  set_flag(Flags::C, test_bit(15, overflowing_nibbles));
  a += b;
  set_flag(Flags::Z, a == 0u);
}

template <>
inline void Core::instr_add<Word, Byte>(Word& a, Byte b) {
  instr_add<Word, Word>(a, static_cast<Word>(b));
}
#endif
