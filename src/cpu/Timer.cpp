#include "Timer.hpp"

#include "src/Gameboy.hpp"
#include "src/cpu/InterruptController.hpp"

#include <iostream>

constexpr Word Timer::Frequencies[];

void Timer::reset() {
  _rTAC = 0;
  _rTMA = 0x00;

  _divider_counter = Counter(0x03);
  _timer_counter = Counter(0x00);
}

uint8_t Timer::read(Word addr) const {
  switch (addr) {
  case DIV:
    return _divider_counter.get_value();
  case TIMA:
    return _timer_counter.get_value();
  case TMA:
    return _rTMA;
  case TAC:
    return _rTAC;
  default:
    return 0;
  }
}

void Timer::write(Word addr, uint8_t val) {
  switch (addr) {
  case DIV:
    _divider_counter.set_value(0x0);
    break;
  case TIMA:
    _timer_counter.set_value(val);
    break;
  case TMA:
    _rTMA = val;
    break;
  case TAC:
    if (test_bit(2, val)) {
      _timer_counter.start();
    } else {
      _timer_counter.stop();
    }
    _timer_counter.set_frequency(val & 0x3);
    break;
  }
}

void Timer::update(Word cycles) {
  _divider_counter.step(cycles);

  if (_timer_counter.step(cycles)) {
    _timer_counter.set_value(_rTMA);
    _components.interrupt_controller->request_interrupt(
        InterruptController::TOI);
  }
}

Timer::Counter::Counter(Byte freq)
    : _is_running(true), _value(0), _freq(freq),
      _cycles(Timer::Frequencies[freq]) {}

bool Timer::Counter::step(Word cycles) {
  if (!_is_running)
    return false;

  _cycles -= cycles;
  while (_cycles <= 0) {
    _cycles += Timer::Frequencies[_freq];
    _value++;
    if (_value == 0x00) {
      return true;
    }
  }
  return false;
}
