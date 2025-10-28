#include "Frame/FrameRGB.h"
#include <stdexcept>

uint8_t& FrameRGB::getPixel(const int index, const RGBChannel channel) {
    if (index < 0 || index >= width * height) throw std::out_of_range("Pixel index out of range");
    return data[index * RGB_NUM_CHANNELS + static_cast<int>(channel)];
}

uint8_t& FrameRGB::at(const int x, const int y, const RGBChannel channel) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        throw std::out_of_range("Coordinates out of range");
    return data[(y * width + x) * RGB_NUM_CHANNELS + static_cast<int>(channel)];
}

FrameRGB::FrameRGB() : width(0), height(0){}

FrameRGB::FrameRGB(int width, int height) : width(width), height(height)
{
    size_t size = width * height * RGB_NUM_CHANNELS;
    data.resize(size);
}

// raw to vec copy overhead
FrameRGB::FrameRGB(int width, int height, uint8_t* raw_data) : width(width), height(height)
{
    size_t size = width * height * RGB_NUM_CHANNELS;
    data.resize(size);
    std::copy(raw_data, raw_data + size, data.begin());
}