#ifndef SQUARECHANNEL_HPP
#define SQUARECHANNEL_HPP

#include "src/sound/ModulationUnits.hpp"
#include "src/sound/SoundChannel.hpp"

#include <array>

namespace sound {

class SquareChannel : public SoundChannel {
 private:
  static const std::array<Byte, 4> s_waveforms;
  Byte _waveform_selected = 2;
  Byte _waveform_step = 0;
  Word _frequency = 0;
  Byte _volume = 0;
  Word _timer = 0;

  SweepUnit _sweep;
  LengthUnit _length;
  EnvelopeUnit _envelope;

  bool _sweep_enabled;

 public:
  SquareChannel(bool sweep_enabled = true)
      : _sweep(_timer),
        _length(63),
        _envelope(_volume),
        _sweep_enabled(sweep_enabled) {
    if (sweep_enabled) bind_module(&_sweep);
    bind_module(&_length);
    bind_module(&_envelope);
  }

  void do_update() override;
  void do_trigger() override;

  void write(Word addr, Byte v) override;
  Byte read(Word addr) const override;
};

}  // namespace sound

#endif  // SQUARECHANNEL_HPP
