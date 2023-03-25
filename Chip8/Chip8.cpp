#include "Chip8.h"

Chip8::Chip8() : randomEngine(time(NULL)) {

    //setting the uniform int distribution
    randByte = std::uniform_int_distribution<std::uint8_t>(0,255U);
    pc = start_address;
    keyPad.fill(false);
    for(ll i = 0; i < fontset_size; i++) {
        memory[fontset_start_address + i] = fontSet[i];
    }

    //filling the instruction table
    instructionTable[0x0] = &Chip8::Table0;
    instructionTable[0x1] = &Chip8::OP_1NNN;
    instructionTable[0x2] = &Chip8::OP_2NNN;
    instructionTable[0x3] = &Chip8::OP_3XNN;
    instructionTable[0x4] = &Chip8::OP_4XNN;
    instructionTable[0x5] = &Chip8::OP_5XY0;
    instructionTable[0x6] = &Chip8::OP_6XNN;
    instructionTable[0x7] = &Chip8::OP_7XNN;
    instructionTable[0x8] = &Chip8::Table8;
    instructionTable[0x9] = &Chip8::OP_9XY0;
    instructionTable[0xA] = &Chip8::OP_ANNN;
    instructionTable[0xB] = &Chip8::OP_BNNN;
    instructionTable[0xC] = &Chip8::OP_CXNN;
    instructionTable[0xD] = &Chip8::OP_DXYN;
    instructionTable[0xE] = &Chip8::TableE;
    instructionTable[0xF] = &Chip8::TableF;

    for(std::uint8_t i = 0; i < 0x65 + 1; i++) {
        if(i < 0xE + 1) {
            table0[i] = &Chip8::OP_NULL;
            table8[i] = &Chip8::OP_NULL;
            tableE[i] = &Chip8::OP_NULL;
        }
        tableF[i] = &Chip8::OP_NULL;
    }

    //table0 - instructions starting with a zero
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xe] = &Chip8::OP_00EE;

    //table8 - instructions starting with a eight
    table8[0x0] = &Chip8::OP_8XY0;
    table8[0x1] = &Chip8::OP_8XY1;
    table8[0x2] = &Chip8::OP_8XY2;
    table8[0x3] = &Chip8::OP_8XY3;
    table8[0x4] = &Chip8::OP_8XY4;
    table8[0x5] = &Chip8::OP_8XY5;
    table8[0x6] = &Chip8::OP_8XY6;
    table8[0x7] = &Chip8::OP_8XY7;
    table8[0xE] = &Chip8::OP_8XYE;

    //tableE - instructions starting with E
    tableE[0x1] = &Chip8::OP_EXA1;
    tableE[0xE] = &Chip8::OP_EX9E;

    //table F - instructions starting with F
    tableF[0x07] = &Chip8::OP_FX07;
    tableF[0x0A] = &Chip8::OP_FX0A;
    tableF[0x15] = &Chip8::OP_FX15;
    tableF[0x18] = &Chip8::OP_FX18;
    tableF[0x1E] = &Chip8::OP_FX1E;
    tableF[0x29] = &Chip8::OP_FX29;
    tableF[0x33] = &Chip8::OP_FX33;
    tableF[0x55] = &Chip8::OP_FX55;
    tableF[0x65] = &Chip8::OP_FX65;

}

void Chip8::Table0() {
    std::uint8_t index = opcode & 0x000Fu;
    auto f = table0[index];
    (this->*f)();
}

void Chip8::Table8() {
    std::uint8_t index = opcode & 0x000Fu;
    auto f = table8[index];
    (this->*f)();
}

void Chip8::TableE() {
    std::uint8_t index = opcode & 0x000Fu;
    auto f = tableE[index];
    (this->*f)();
}

void Chip8::TableF() {
    std::uint8_t index = opcode & 0x00FFu;
    auto f = tableF[index];
    (this->*f)();
}

void Chip8::cycle() {
    opcode = (memory[pc] << 8u) | memory[pc+1];
    //increment the program counter before execution
    pc += 2;

    //process the opcode
    executeInstruction();

    //decrementing the delay timer and sound timer
    if(delay_timer > 0) --delay_timer;
    if(sound_timer > 0) --sound_timer;
}

void Chip8::executeInstruction() {
    std::uint8_t index = (opcode & 0xF000u) >> 12u;
    auto f = instructionTable[index];
    (this->*f)();
}

void Chip8::loadRom(const std::string& filePath) {
    std::ifstream  file(filePath, std::ios::binary | std::ios::ate);
    if(file.is_open()) {

        //tell me the position
        //of the cursor <=> filesize
        std::streampos size = file.tellg();
        program_size = size;
        char* buffer = new char[size];


        //go back to the beginning of the file
        file.seekg(0, std::ios::beg);

        file.read(buffer, size);

        file.close();

        for(ll i = 0; i < size; i++) {
            memory[start_address + i] = buffer[i];
        }

        delete[] buffer;
        romLoaded = true;
    }
    else {
        std::cout << "ERROR: ROM file not found" << std::endl;
        exit(FailStates::FILE_NOT_FOUND);
    }
}

void Chip8::OP_NULL() {
    return;
}

//Useless instruction
//void Chip8::OP_0NNN() {
//
//}

void Chip8::OP_00E0() {
    //simply fill the screen with zeroes
    display.fill(0);
}

void Chip8::OP_00EE() {
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1NNN() {
    uint16_t addr = opcode & 0x0FFFu;
    pc = addr;
}

void Chip8::OP_2NNN() {
    uint16_t addr = opcode & 0x0FFFu;
    stack[sp++] = pc;
    pc = addr;
}

void Chip8::OP_3XNN() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t val = opcode & 0x00FFu;
    if(val == registers[reg]) pc += 2;
}

void Chip8::OP_4XNN() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t val = opcode & 0x00FFu;
    if(registers[reg] != val) pc += 2;
}

void Chip8::OP_5XY0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    if(registers[reg1] == registers[reg2]) pc += 2;
}

void Chip8::OP_6XNN() {
    uint8_t val = (opcode & 0x00FFu);
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[reg] = val;
}

void Chip8::OP_7XNN() {
    uint8_t val = (opcode & 0x00FFu);
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[reg] += val;
}

void Chip8::OP_8XY0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    registers[reg1] = registers[reg2];
}

void Chip8::OP_8XY1() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    registers[reg1] = registers[reg1] | registers[reg2];
}

void Chip8::OP_8XY2() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    registers[reg1] = registers[reg1] & registers[reg2];
}

void Chip8::OP_8XY3() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    registers[reg1] = registers[reg1] ^ registers[reg2];
}

void Chip8::OP_8XY4() {
    uint8_t regX = (opcode & 0x0F00u) >> 8u;
    uint8_t regY = (opcode & 0x00F0U) >> 4u;

    uint16_t result = registers[regX] + registers[regY];
    bool cy = (result > 0xFFu);

    registers[0xF] = cy;
    registers[regX] = result & 0xFFu;
}

void Chip8::OP_8XY5() {
    uint8_t regX = (opcode & 0x0F00u) >> 8u;
    uint8_t regY = (opcode & 0x00F0U) >> 4u;

    bool borrow = (registers[regX] > registers[regY]);
    registers[0xF] = borrow; //realisticaly this is !borrow

    registers[regX] -= registers[regY];
}

void Chip8::OP_8XY6() {
    uint8_t regX = (opcode & 0x0F00u) >> 8u;
    uint8_t regY = (opcode & 0x00F0U) >> 4u;

    registers[regX] = (registers[regY] >> 1u);
    registers[0xF] = registers[regY] & 0x01u;
}

void Chip8::OP_8XY7() {
    uint8_t regX = (opcode & 0x0F00u) >> 8u;
    uint8_t regY = (opcode & 0x00F0U) >> 4u;

    bool borrow = (registers[regY] > registers[regX]);
    registers[0xF] = borrow; //realisticaly this is !borrow

    registers[regX] = registers[regY] - registers[regX];
}

void Chip8::OP_8XYE() {
    uint8_t regX = (opcode & 0x0F00u) >> 8u;
    uint8_t regY = (opcode & 0x00F0U) >> 4u;

    registers[regX] = (registers[regY] << 1u);
    registers[0xF] = (registers[regY] & 0x80u) >> 7u;
}

void Chip8::OP_9XY0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0U) >> 4u;

    if(registers[reg1] != registers[reg2]) pc += 2;
}

void Chip8::OP_ANNN() {
    uint16_t addr = opcode & 0x0FFFu;
    vi = addr;
}

void Chip8::OP_BNNN() {
    uint16_t addr = opcode & 0x0FFFu;
    pc = addr + registers[0x0];
}

void Chip8::OP_CXNN() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t mask = opcode & 0x00FFu;
    uint8_t rand = randByte(randomEngine);

    registers[reg] = rand & mask;

}

void Chip8::OP_DXYN() {
    uint8_t regX = (opcode & 0x0F00) >> 8u;
    uint8_t regY = (opcode & 0x00F0) >> 4u;
    uint8_t bytes = opcode & 0x000F;

    //wrap if going beyond screen boundaries
    uint8_t xPos = registers[regX] & (DISPLAY_WIDTH - 1);
    uint8_t yPos = registers[regY] & (DISPLAY_HEIGHT - 1);
    if(xPos > DISPLAY_WIDTH - 1 || xPos < 0 || yPos < 0 || yPos > DISPLAY_HEIGHT - 1) {
        return;
    }
    //set to 0 if no overlap, 1 otherwise
    registers[0xF] = 0;

    for(uint8_t row = 0; row < bytes; row++) {
        uint8_t sprite_byte = memory[vi + row];

        //the sprite is guaranteed to have 8 columns
        for(uint8_t col = 0; col < 8; col++) {
            //extract the specific bit
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            bool* screen_pixel = &display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];

            if(sprite_pixel) {
                //collision
                if(*screen_pixel == 1) {
                    registers[0xF] = 1;
                }

                //XOR the pixel with a pixel currently on the screen
                *screen_pixel ^= 1;
            }
        }
    }
}

void Chip8::OP_EX9E() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[reg];

    if(keyPad[key]) {
        pc += 2;
    }
}

void Chip8::OP_EXA1() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[reg];

    if(!keyPad[key]) {
        pc += 2;
    }
}

void Chip8::OP_FX07() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[reg] = delay_timer;
}

void Chip8::OP_FX0A() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    bool pressed = false;
    for(int i = 0; i < keyPad.size(); i++) {
        if(keyPad[i]) {
            pressed = true;
            registers[reg] = i;
            break;
        }
    }
    //this will make the instruction run in an endless loop
    if(!pressed) pc -= 2;
}

void Chip8::OP_FX15() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    delay_timer = registers[reg];
}

void Chip8::OP_FX18() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    sound_timer = registers[reg];
}

void Chip8::OP_FX1E() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    vi += registers[reg];
}

void Chip8::OP_FX29() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    vi = fontset_start_address + (5 * registers[reg]);
}

void Chip8::OP_FX33() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;

    //binary coded decimal equivalent
    uint8_t val = registers[reg];
    uint8_t k;
    for(int i = 2; i >= 0; i--) {
        k = val % 10;
        memory[vi + i] = k;
        val /= 10;
    }
}

void Chip8::OP_FX55() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0u; i <= reg; i++) {
        memory[vi + i] = registers[i];
    }
}

void Chip8::OP_FX65() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0u; i <= reg; i++) {
        registers[i] = memory[vi + i];
    }
}