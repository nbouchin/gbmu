#include "Gameboy.hpp"
#include "src/Cartridge.hpp"
#include "src/MemoryBus.hpp"
#include "src/cpu/Core.hpp"
#include "src/cpu/InterruptController.hpp"
#include "src/cpu/Timer.hpp"
#include "src/LCDRegisters.hpp"

#include <fstream>
#include <iostream>
#include <ostream>

ComponentsContainer::ComponentsContainer(const std::string &rom_path) {
  cartridge = std::make_unique<Cartridge>(rom_path);
  interrupt_controller = std::make_unique<InterruptController>(*this);
  core = std::make_unique<Core>(*this);
  timer = std::make_unique<Timer>(*this);
  lcd_registers = std::make_unique<LCDRegisters>();
  mem_bus = std::make_unique<MemoryBus>(*this);
}

// Dtor Implementation MUST be here where the types are complete
ComponentsContainer::~ComponentsContainer() {}

Gameboy::Gameboy(const std::string &rom_path) : _components(rom_path) {
  _begin = _components.cartridge->get_begin();
  _end = _components.cartridge->get_end();
}

void Gameboy::step() {
  if (_debugger.is_enabled()) {
    _debugger.get_instruction_set(_begin + _components.core->pc());
    _debugger.wait_user_interaction();
  }
  _components.core->execute(_begin);
  _components.interrupt_controller->ParseInterrupt();
  _components.timer->Update(_components.core->clock());
}

int Gameboy::run() {

  system("sh -c clear");// for debug TODO: remove
  while (_begin + _components.core->pc() != _end) {
    step();
  }
  return (0);
}

void Gameboy::init() {
  _components.mem_bus->write(0xFF05, 0x00); // Timer Counter Register
  _components.mem_bus->write(0xFF06, 0x00); // Timer Modulo Register
  _components.mem_bus->write(0xFF07, 0x00); // Timer Controller Registerr
  _components.mem_bus->write(0xFF10, 0x80); // Sound1 Register
  _components.mem_bus->write(0xFF11, 0xBF); // Sound1 Register
  _components.mem_bus->write(0xFF12, 0xF3); // Sound1 Register
  _components.mem_bus->write(0xFF14, 0xBF); // Sound1 Register
  _components.mem_bus->write(0xFF16, 0x3F); // Sound2 Register
  _components.mem_bus->write(0xFF17, 0x00); // Sound2 Register
  _components.mem_bus->write(0xFF19, 0xBF); // Sound2 Register
  _components.mem_bus->write(0xFF1A, 0x7F); // Sound3 Register
  _components.mem_bus->write(0xFF1B, 0xFF); // Sound3 Register
  _components.mem_bus->write(0xFF1C, 0x9F); // Sound3 Register
  _components.mem_bus->write(0xFF1E, 0xBF); // Sound3 Register
  _components.mem_bus->write(0xFF20, 0xFF); // Sound4 Register
  _components.mem_bus->write(0xFF21, 0x00); // Sound4 Register
  _components.mem_bus->write(0xFF22, 0x00); // Sound4 Register
  _components.mem_bus->write(0xFF23, 0xBF); // Sound4 Register
  _components.mem_bus->write(0xFF24, 0x77); // Sound4 Register
  _components.mem_bus->write(0xFF25, 0xF3); // Sound4 Register
  _components.mem_bus->write(0xFF26, 0xF1); // Sound4 Register
  _components.mem_bus->write(0xFF40, 0x91); // LCD Register
  _components.mem_bus->write(0xFF42, 0x00); // LCD Register
  _components.mem_bus->write(0xFF43, 0x00); // LCD Register
  _components.mem_bus->write(0xFF45, 0x00); // LCD Register
  _components.mem_bus->write(0xFF47, 0xFC); // LCD Register
  _components.mem_bus->write(0xFF48, 0xFF); // LCD Register
  _components.mem_bus->write(0xFF49, 0xFF); // LCD Register
  _components.mem_bus->write(0xFF4A, 0x00); // LCD Register
  _components.mem_bus->write(0xFF4B, 0x00); // LCD Register
  _components.mem_bus->write(0xFFFF, 0x00);
}

void Gameboy::save(std::string save_name) {
  std::filebuf fb;

  fb.open(save_name, std::ios::binary | std::ios::out | std::ios::trunc);
  if (fb.is_open()) {
    std::ostream os(&fb);
    AMemoryBankController::RAMContainer ram_content;

    _components.cartridge->save_ram_to_buffer(ram_content);
    for (unsigned int i = 0; i < AMemoryBankController::RAMSize; i++) {
      os << ram_content[i];
    }
    fb.close();
  }
}

void Gameboy::load_save(std::string save_name) {
  GameSave gs;

  std::ifstream in(save_name, std::ios::binary | std::ios::in | std::ios::ate);

  if (in.is_open()) {
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char *>(&gs), size);
    _components.cartridge->set_ram_content(gs.ram);
  }
}
