#pragma once
#include <vector>
#include "ColorChannel.h"

struct FrameYUV444 {
    int width;
    int height;
    std::vector<uint8_t> data;

    uint8_t& getPixel(int index, YUVChannel channel);
    uint8_t& at(int x, int y, YUVChannel channel);

    FrameYUV444();
    FrameYUV444(int width, int height);
};