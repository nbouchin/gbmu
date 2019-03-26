## Auto generated make tool, don't edit manually.
src/sound/$(OBJECT_DIR)/%.o: src/sound/%.cpp
	$(COMP) $@ $<
src/sound/$(OBJECT_DIR)/APU.o: src/sound/APU.cpp src/sound/APU.hpp \
  src/Fwd.hpp src/sound/Fwd.hpp src/IReadWrite.hpp \
  src/sound/AudioInterface.hpp src/sound/VolumeTable.hpp \
  src/sound/NoiseChannel.hpp src/sound/SoundChannel.hpp \
  src/sound/ModulationUnits.hpp src/sound/SquareChannel.hpp \
  src/sound/WaveChannel.hpp utils/Operations_utils.hpp
src/sound/APU.hpp:
src/Fwd.hpp:
src/sound/Fwd.hpp:
src/IReadWrite.hpp:
src/sound/AudioInterface.hpp:
src/sound/VolumeTable.hpp:
src/sound/NoiseChannel.hpp:
src/sound/SoundChannel.hpp:
src/sound/ModulationUnits.hpp:
src/sound/SquareChannel.hpp:
src/sound/WaveChannel.hpp:
utils/Operations_utils.hpp:
src/sound/$(OBJECT_DIR)/ModulationUnits.o: src/sound/ModulationUnits.cpp \
  src/sound/ModulationUnits.hpp src/Fwd.hpp src/sound/Fwd.hpp \
  src/sound/SoundChannel.hpp src/IReadWrite.hpp
src/sound/$(OBJECT_DIR)/NoiseChannel.o: src/sound/NoiseChannel.cpp \
  src/sound/NoiseChannel.hpp src/Fwd.hpp src/sound/Fwd.hpp \
  src/sound/SoundChannel.hpp src/IReadWrite.hpp \
  src/sound/ModulationUnits.hpp
src/sound/$(OBJECT_DIR)/SoundChannel.o: src/sound/SoundChannel.cpp \
  src/sound/SoundChannel.hpp src/Fwd.hpp src/sound/Fwd.hpp \
  src/IReadWrite.hpp src/sound/ModulationUnits.hpp \
  utils/Operations_utils.hpp
src/sound/$(OBJECT_DIR)/SquareChannel.o: src/sound/SquareChannel.cpp \
  src/sound/SquareChannel.hpp src/Fwd.hpp src/sound/Fwd.hpp \
  src/sound/ModulationUnits.hpp src/sound/SoundChannel.hpp \
  src/IReadWrite.hpp utils/Operations_utils.hpp
src/sound/$(OBJECT_DIR)/WaveChannel.o: src/sound/WaveChannel.cpp \
  src/sound/WaveChannel.hpp src/Fwd.hpp src/sound/Fwd.hpp \
  src/sound/ModulationUnits.hpp src/sound/SoundChannel.hpp \
  src/IReadWrite.hpp src/sound/VolumeTable.hpp
