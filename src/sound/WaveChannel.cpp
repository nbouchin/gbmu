#include "src/sound/WaveChannel.hpp"
#include "src/sound/VolumeTable.hpp"

#include <cassert>

static constexpr Word compute_timer(Word freq, Byte speed) noexcept {
  return ((2048 - freq) * 2) * speed;
}

namespace sound {

const Byte WaveChannel::s_volume_shifts[VolumeShiftCount] = {4, 0, 1, 2};

void WaveChannel::do_update() {
  if (_timer == 0) {
    _timer = compute_timer(_frequency, p_speed);
    if (++_table_index >= _table.size()) _table_index = 0;
  }
  assert(_volume_code < VolumeShiftCount);
  Byte shift = s_volume_shifts[_volume_code];
  p_output_volume = _table.get_nibble(_table_index) >> shift;
  --_timer;
}

void WaveChannel::do_trigger() {
  _timer = compute_timer(_frequency, p_speed);
  _table_index = 0;
}

void WaveChannel::do_clear() {
  _frequency = 0;
  _timer = 0;
  _volume_code = 0;
  _table_index = 0;
}

void WaveChannel::write(Word addr, Byte v) {
  switch ((addr & 0xf) % 5) {
    case 0x0:
      p_enabled = v & 0x80;
      break;
    case 0x1:
      _length.set_length(v);
      break;
    case 0x2:
      _volume_code = (v & 0x60) >> 5;
      break;
    case 0x3:
      _frequency &= ~0xff;
      _frequency |= v;
      break;
    case 0x4:
      _length.enable((v & 0x40) >> 6);
      _frequency &= 0xff;
      _frequency |= (v & 0x7) << 8;
      if (v & 0x80) trigger();
      break;
  }
}

Byte WaveChannel::read(Word addr) const {
  switch ((addr & 0xf) % 5) {
    case 0x0:
      return (p_enabled << 7) | 0x7f;
    case 0x1:
      return 0xff;
    case 0x2:
      return (_volume_code << 5) | 0x9f;
    case 0x3:
      return 0xff;
    case 0x4:
      return (_length.is_enabled() << 6) | 0xbf;
  }

  assert(false);  // TODO throw some exception
}

}  // namespace sound
