#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <ctime>

#include "FailStates.h"

typedef long long ll;
typedef void (*Instruction)(void);

struct Chip8 {

    Chip8();

    const static unsigned int start_address = 0x200;
    const static unsigned int fontset_start_address = 0x50;
    const static unsigned int fontset_size = 80;

    const static uint8_t DISPLAY_WIDTH = 64;
    const static uint8_t DISPLAY_HEIGHT = 32;

    bool romLoaded{false};

    //temporary
    std::uint16_t program_size{};

    //random engine for the random function
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<std::uint8_t> randByte;

    typedef void (Chip8::*Chip8Func)();
    //instruction table
    std::array<Chip8Func, 0xF + 1> instructionTable;
    std::array<Chip8Func, 0xE + 1> table0;
    std::array<Chip8Func, 0xE + 1> table8;
    std::array<Chip8Func, 0xE + 1> tableE;
    std::array<Chip8Func, 0x65 + 1> tableF;

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    //trenutni opcode
    std::uint16_t opcode{};

    //16 8-bit, multi-purpose registers
    std::array<std::uint8_t, 16> registers;

    //special 16-bit index register
    std::uint16_t vi{};

    //16-bit program counter (PC)
    std::uint16_t pc{};

    //16-level stack of 16 bit values
    std::array<std::uint16_t, 16> stack{};

    //8-bit stack pointer
    std::uint8_t sp{};

    //8-bit delay timer
    std::uint8_t delay_timer{};

    //8-bit sound timer
    std::uint8_t sound_timer{};

    //keymap for 16 available keys

    //recommended key mappings

//    Keypad       Keyboard
//    +-+-+-+-+    +-+-+-+-+
//    |1|2|3|C|    |1|2|3|4|
//    +-+-+-+-+    +-+-+-+-+
//    |4|5|6|D|    |Q|W|E|R|
//    +-+-+-+-+ => +-+-+-+-+
//    |7|8|9|E|    |A|S|D|F|
//    +-+-+-+-+    +-+-+-+-+
//    |A|0|B|F|    |Z|X|C|V|
//    +-+-+-+-+    +-+-+-+-+
    std::array<bool, 16> keyPad{};

    //4 kilobytes of memory
    std::array<std::uint8_t, 4096> memory{};

    //B&W, 64*32(2048) bytes large display memory
    std::array<bool, 64*32> display{};

    //the standard font set
    const std::array<uint8_t, fontset_size> fontSet = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    /**
     *
     * @param filePath: Absolute path to the ROM file
     */
    void loadRom(const std::string& filePath);

    /**
     * One instruction cycle
     */
    void cycle();

    /**
     * Execute the current instruction
     */
    void executeInstruction();
    //INSTRUCTION SET functions

    ///NULL: Do nothing
    void OP_NULL();

//    /// 0NNN: Execute machine language subroutine at address NNN
// Useless istruction
//    void OP_0NNN();

    /// 00E0: Clear the screen
    void OP_00E0();

    /// 00EE: Return from a subroutine
    void OP_00EE();

    /// 1NNN: Jump to address NNN
    void OP_1NNN();

    /// 2NNN: Execute subroutine starting at address NNN
    void OP_2NNN();

    /// 3XNN: Skip the following instruction if the value of register VX equals NN
    void OP_3XNN();

    /// 4XNN: Skip the following instruction if the value of register VX is not equal to NN
    void OP_4XNN();

    /// 5XY0: Skip the following instruction if the value of register VX is equal to the value of register VY
    void OP_5XY0();

    /// 6XNN: Store number NN in register VX
    void OP_6XNN();

    /// 7XNN: Add the value NN to register VX
    void OP_7XNN();

    /// 8XY0: Store the value of register VY in register VX
    void OP_8XY0();

    /// 8XY1: Set VX to VX OR VY
    void OP_8XY1();

    /// 8XY2: Set VX to VX AND VY
    void OP_8XY2();

    /// 8XY3: Set VX to VX XOR VY
    void OP_8XY3();

    /**
     * 8XY4: Add the value of register VY to register VX
     * Set VF to 01 if a carry occurs
     * Set VF to 00 if a carry does not occur
     */
    ///
    void OP_8XY4();

    /// 8XY5: Subtract the value of register VY from register VX
    void OP_8XY5();

    /// 8XY6: Store the value of register VY shifted right one bit in register VX
    void OP_8XY6();

    /// 8XY7: Set register VX to the value of VY minus VX
    void OP_8XY7();

    /// 8XYE: Store the value of register VY shifted left one bit in register VX
    void OP_8XYE();

    /// 9XY0: Skip the following instruction if the value of register VX is not equal to the value of register VY
    void OP_9XY0();

    /// ANNN: Store memory address NNN in register I
    void OP_ANNN();

    /// BNNN: Jump to address NNN + V0
    void OP_BNNN();

    /// CXNN: Set VX to a random number with a mask of NN
    void OP_CXNN();

    /// DXYN: Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
    void OP_DXYN();

    /// EX9E: Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
    void OP_EX9E();

    /// EXA1: Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
    void OP_EXA1();

    /// FX07: Store the current value of the delay timer in register VX
    void OP_FX07();

    /// FX0A: Wait for a keypress and store the result in register VX
    void OP_FX0A();

    /// FX15: Set the delay timer to the value of register VX
    void OP_FX15();

    /// FX18: Set the sound timer to the value of register VX
    void OP_FX18();

    /// FX1E: Add the value stored in register VX to register I
    void OP_FX1E();

    /// Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
    void OP_FX29();

    /// Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
    void OP_FX33();

    /**
     * Store the values of registers V0 to VX inclusive in memory starting at address
     * I. I is set to I + X + 1 after operation
     */
    void OP_FX55();

    /**
     * Fill registers V0 to VX inclusive with the values stored in memory starting at address I
     * I is set to I + X + 1 after operation
     */
    void OP_FX65();

};
