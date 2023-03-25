#include "Machine.h"

Machine::Machine(
        const std::string &title, int scale, float frequency = 60
) : window{
    sf::RenderWindow(
    sf::VideoMode(
            chip8.DISPLAY_WIDTH * scale,
            chip8.DISPLAY_HEIGHT * scale),title,
            sf::Style::Close
    )
}, chip8{} {
    this->frequency = frequency;
    this->scale = scale;

    pixel = sf::RectangleShape(sf::Vector2f(scale, scale));
    pixel.setFillColor(foregroundColor);
}

void Machine::draw() {

    //the default clear color is black
    window.clear(backgroundColor);

    for(std::size_t i = 0; i < chip8.DISPLAY_HEIGHT; i++) {
        for(std::size_t j = 0; j < chip8.DISPLAY_WIDTH; j++) {
            //if the pixel at this position is set
            if(chip8.display[i * chip8.DISPLAY_WIDTH + j]) {
                pixel.setPosition(j * scale, i * scale);
                window.draw(pixel);
            }
        }
    }

    window.display();

}

void Machine::processInput() {
    sf::Event event;
    while(window.pollEvent(event)) {

        if(event.type == sf::Event::Closed) {
            window.close();
            break;
        }

        if(event.type == sf::Event::KeyPressed) {
            //TODO: add dynamic ROM loading
            if(event.key.code == sf::Keyboard::Escape) {
                window.close();
                break;
            }

            chip8.keyPad[0x0] = event.key.code == sf::Keyboard::X;
            chip8.keyPad[0x1] = event.key.code == sf::Keyboard::Num1;
            chip8.keyPad[0x2] = event.key.code == sf::Keyboard::Num2;
            chip8.keyPad[0x3] = event.key.code == sf::Keyboard::Num3;
            chip8.keyPad[0x4] = event.key.code == sf::Keyboard::Q;
            chip8.keyPad[0x5] = event.key.code == sf::Keyboard::W;
            chip8.keyPad[0x6] = event.key.code == sf::Keyboard::E;
            chip8.keyPad[0x7] = event.key.code == sf::Keyboard::A;
            chip8.keyPad[0x8] = event.key.code == sf::Keyboard::S;
            chip8.keyPad[0x9] = event.key.code == sf::Keyboard::D;
            chip8.keyPad[0xA] = event.key.code == sf::Keyboard::Z;
            chip8.keyPad[0xB] = event.key.code == sf::Keyboard::C;
            chip8.keyPad[0xC] = event.key.code == sf::Keyboard::Num4;
            chip8.keyPad[0xD] = event.key.code == sf::Keyboard::R;
            chip8.keyPad[0xE] = event.key.code == sf::Keyboard::F;
            chip8.keyPad[0xF] = event.key.code == sf::Keyboard::V;
        }
        if(event.type == sf::Event::KeyReleased) {
            chip8.keyPad[0x1] = chip8.keyPad[0x1] && !(event.key.code == sf::Keyboard::Num1);
            chip8.keyPad[0x2] = chip8.keyPad[0x2] && !(event.key.code == sf::Keyboard::Num2);
            chip8.keyPad[0x3] = chip8.keyPad[0x3] && !(event.key.code == sf::Keyboard::Num3);
            chip8.keyPad[0xc] = chip8.keyPad[0xc] && !(event.key.code == sf::Keyboard::Num4);
            chip8.keyPad[0x4] = chip8.keyPad[0x4] && !(event.key.code == sf::Keyboard::Q);
            chip8.keyPad[0x5] = chip8.keyPad[0x5] && !(event.key.code == sf::Keyboard::W);
            chip8.keyPad[0x6] = chip8.keyPad[0x6] && !(event.key.code == sf::Keyboard::E);
            chip8.keyPad[0x7] = chip8.keyPad[0x7] && !(event.key.code == sf::Keyboard::A);
            chip8.keyPad[0x8] = chip8.keyPad[0x8] && !(event.key.code == sf::Keyboard::S);
            chip8.keyPad[0x9] = chip8.keyPad[0x9] && !(event.key.code == sf::Keyboard::D);
            chip8.keyPad[0xe] = chip8.keyPad[0xe] && !(event.key.code == sf::Keyboard::F);
            chip8.keyPad[0xd] = chip8.keyPad[0xd] && !(event.key.code == sf::Keyboard::R);
            chip8.keyPad[0xa] = chip8.keyPad[0xa] && !(event.key.code == sf::Keyboard::Z);
            chip8.keyPad[0x0] = chip8.keyPad[0x0] && !(event.key.code == sf::Keyboard::X);
            chip8.keyPad[0xb] = chip8.keyPad[0xb] && !(event.key.code == sf::Keyboard::C);
            chip8.keyPad[0xf] = chip8.keyPad[0xf] && !(event.key.code == sf::Keyboard::V);

        }
    }
}

void Machine::processSound() {
    //TODO: add sound
}

void Machine::runLoop() {
    if(!chip8.romLoaded) {
        std::cout << "You have to load a ROM first" << std::endl;
        exit(FailStates::ROM_NOT_LOADED);
    }

    //the main clock
    sf::Clock clock;

    //CPU frequency
    sf::Time cpuClockSpeed = sf::seconds(1.f / frequency);
    sf::Time cpuClockAccumulator = sf::Time::Zero;

    //timer frequency
    sf::Time timerClockSpeed = sf::seconds(1.f / 60.f);
    sf::Time timerClockAccumulator = sf::Time::Zero;


    while(window.isOpen()) {
        //managing the inputs
        processInput();

        //managing the timers
        if(cpuClockAccumulator >= cpuClockSpeed) {
            chip8.cycle();
            cpuClockAccumulator = sf::Time::Zero;
        }

        if(timerClockAccumulator >= timerClockSpeed) {
            if(chip8.delay_timer > 0) {
                chip8.delay_timer--;
            }

            if(chip8.sound_timer > 0) {
                //TODO: process the sound
                chip8.sound_timer--;
            }

            timerClockAccumulator = sf::Time::Zero;
        }

        //drawing to the screen
        draw();

        //advancing the clock
        cpuClockAccumulator += clock.getElapsedTime();
        timerClockAccumulator += clock.restart();
    }
}

void Machine::loadRom(const std::string& filePath) {
    chip8.loadRom(filePath);
}
