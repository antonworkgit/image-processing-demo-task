#pragma once

constexpr auto RGB_NUM_CHANNELS = 3;
constexpr auto YUV_NUM_CHANNELS = 3;

enum class RGBChannel {
    R = 0,
    G = 1,
    B = 2
};

enum class YUVChannel {
    Y = 0,
    U = 1,
    V = 2
};