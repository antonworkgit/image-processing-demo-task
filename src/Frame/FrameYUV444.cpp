#include "Frame/FrameYUV444.h"
#include <stdexcept>

uint8_t& FrameYUV444::getPixel(const int index, const YUVChannel channel) {
    if (index < 0 || index >= width * height) throw std::out_of_range("Pixel index out of range");
    return data[index * YUV_NUM_CHANNELS + static_cast<int>(channel)];
}

uint8_t& FrameYUV444::at(const int x, const int y, const YUVChannel channel) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        throw std::out_of_range("Coordinates out of range");
    return data[(y * width + x) * YUV_NUM_CHANNELS + static_cast<int>(channel)];
}

FrameYUV444::FrameYUV444() : width(0), height(0){}

FrameYUV444::FrameYUV444(int width, int height) : width(width), height(height)
{
    size_t size = width * height * YUV_NUM_CHANNELS;
    data.resize(size);
}