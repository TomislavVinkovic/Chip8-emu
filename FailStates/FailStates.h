#pragma once

#include <cstdint>

struct FailStates {
    FailStates() = delete;
    ~FailStates() = delete;

    const static std::uint8_t FILE_NOT_FOUND = 1;
    const static std::uint8_t ROM_NOT_LOADED = 2;
};
