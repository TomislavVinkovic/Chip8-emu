#include <iostream>
#include "Chip8.h"
#include <SFML/Graphics.hpp>
#include "Machine.h"

int main() {
    Machine machine("Chip8 test", 16.f, 500);
    machine.loadRom("/home/tomislav/Desktop/emudev/Chip8/roms/chip8-test-suite.ch8");
    machine.runLoop();
    return 0;
}
