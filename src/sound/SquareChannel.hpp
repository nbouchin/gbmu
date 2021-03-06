#ifndef SQUARECHANNEL_HPP
#define SQUARECHANNEL_HPP

#include "src/Fwd.hpp"

#include "src/sound/ModulationUnits.hpp"
#include "src/sound/SoundChannel.hpp"

#include <array>

namespace sound {

class SquareChannel : public SoundChannel {
 private:
  static const std::array<Byte, 4> s_waveforms;
  Byte _waveform_selected;
  Byte _waveform_step;
  Word _frequency;
  Byte _volume;
  Word _timer;

  SweepUnit _sweep;
  LengthUnit _length;
  EnvelopeUnit _envelope;

  void do_update() override;
  void do_trigger() override;
  void do_clear() override;

#define __serial_square(OP) \
  ar OP _waveform_selected; \
  ar OP _waveform_step;     \
  ar OP _frequency;         \
  ar OP _volume;            \
  ar OP _timer;             \
  ar OP _sweep;             \
  ar OP _length;            \
  ar OP _envelope;

  void do_serialize(boost::archive::text_iarchive& ar) override {
    __serial_square(>>);
  }
  void do_serialize(boost::archive::text_oarchive& ar) override {
    __serial_square(<<);
  }

 public:
  SquareChannel(bool sweep_enabled = true)
      : _sweep(_frequency), _length(63), _envelope(_volume) {
    if (sweep_enabled) bind_module(&_sweep);
    bind_module(&_length);
    bind_module(&_envelope);
    clear();
  }

  void write(Word addr, Byte v) override;
  Byte read(Word addr) const override;
};

}  // namespace sound

#endif  // SQUARECHANNEL_HPP
