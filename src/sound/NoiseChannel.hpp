#ifndef NOISECHANNEL_HPP
#define NOISECHANNEL_HPP

#include "src/Fwd.hpp"

#include "src/sound/ModulationUnits.hpp"
#include "src/sound/SoundChannel.hpp"

namespace sound {

class NoiseChannel : public SoundChannel {
 private:
  Word _timer;
  Word _lfsr;  // Linear feedback feed register
  Byte _shift;
  bool _width_mode;
  Byte _divisor_code;
  Byte _volume;

  LengthUnit _length;
  EnvelopeUnit _envelope;

  void do_update() override;
  void do_trigger() override;
  void do_clear() override;

#define __serial_noise(OP) \
  ar OP _timer;            \
  ar OP _lfsr;             \
  ar OP _width_mode;       \
  ar OP _divisor_code;     \
  ar OP _volume;           \
  ar OP _length;           \
  ar OP _envelope;

  void do_serialize(boost::archive::text_iarchive& ar) override {
    __serial_noise(>>);
  }
  void do_serialize(boost::archive::text_oarchive& ar) override {
    __serial_noise(<<);
  }

  static constexpr Word get_divider(Byte code) noexcept {
    Word ret = 8u;
    if (code != 0) ret = 16 * code;
    return ret;
  }

 public:
  NoiseChannel() : _length(63), _envelope(_volume) {
    bind_module(&_length);
    bind_module(&_envelope);
    clear();
  }

  void write(Word, Byte) override;
  Byte read(Word) const override;
};

}  // namespace sound

#endif  // NOISECHANNEL_HPP
