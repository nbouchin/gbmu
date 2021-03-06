#pragma once
#ifndef FWD_HPP
#define FWD_HPP

#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

class MemoryBus;
class IReadWrite;
class Core;
class Cartridge;
class InterruptController;
class InputController;
class Timer;
class LCDRegisters;
class UnitWorkingRAM;
class Gameboy;
class Bios;
struct ComponentsContainer;

#include "src/sound/Fwd.hpp"

#endif // FWD_HPP
