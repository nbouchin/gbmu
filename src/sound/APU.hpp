#ifndef APU_HPP
#define APU_HPP

#include "src/Fwd.hpp"
#include "src/IReadWrite.hpp"

#include "src/sound/AudioInterface.hpp"
#include "src/sound/VolumeTable.hpp"

#include <memory>
#include <chrono>

#define CPU_FREQ (4194304)
#define UPDATE_FREQ (512)

namespace sound {

class SoundChannel;

class APU : public IReadWrite {
 private:
  struct MemoryRangedChannel {
    Word begin, end;
    std::unique_ptr<SoundChannel> channel;
    ~MemoryRangedChannel();
  };

  int _update_countdown = 0;
  int _downsampling_countdown = 0;
  unsigned int _modulation_units_steps = 0;
  bool _APU_on = false;
  Byte _vin_and_volumes = 0;
  Byte _channel_to_terminal_output = 0;

  std::size_t _output_index = 0;
  AudioInterface::MonoSamples _right_output;
  AudioInterface::MonoSamples _left_output;

  AudioInterface* const _audio_interface;
  ComponentsContainer& _comps;

  VolumeTable _wave_ram = {};

  std::array<MemoryRangedChannel, 4> _channels;

  void update_clock();
  void clear();
  float fetch_and_mix_samples(Byte enabled_channels, float vol) const;
  float right_volume() const { return _vin_and_volumes & 0x07;}
  float left_volume() const { return (_vin_and_volumes & 0x70) >> 4;}

  mutable bool _dump = false;
  std::chrono::time_point<std::chrono::system_clock> _last_dump;

 public:
  APU(AudioInterface*, ComponentsContainer&);

  APU() = delete;

  const auto& channels() const { return _channels; }
  void update_clock(Word);
  Byte read(Word addr) const override;
  void write(Word addr, Byte) override;
  void toggle_dump() const { 
    _dump = !_dump; 
  }
  void dump() const;
};
}  // namespace sound

#endif  // APU_HPP