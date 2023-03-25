#pragma once

#include "Chip8.h"
#include <string>
#include <iostream>
#include "FailStates.h"
#include <SFML/Graphics.hpp>

class Machine {
    private:
        //the standard chip8 clock frequency is about 500 hz
        float frequency;
        int scale;
        Chip8 chip8;
        sf::RenderWindow window;
        sf::RectangleShape pixel;
        sf::Color backgroundColor = {255, 231, 122};
        sf::Color foregroundColor = {44, 95, 45};
    public:
        Machine(
                const std::string& title,
                int scale, float frequency
        );

        void draw();
        void processInput();
        void processSound();
        void runLoop();
        void loadRom(const std::string& filePath);

};
