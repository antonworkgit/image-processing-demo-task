#pragma once
#include <vector>
#include "ColorChannel.h"

struct FrameGrayscale {
    int width;
    int height;
    std::vector<uint8_t> data;

    uint8_t& getPixel(int index);
    uint8_t& at(int x, int y);

    FrameGrayscale();
    FrameGrayscale(int width, int height);
};