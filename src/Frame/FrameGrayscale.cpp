#include "Frame/FrameGrayscale.h"
#include <stdexcept>

uint8_t& FrameGrayscale::getPixel(const int index) {
    if (index < 0 || index >= width * height) throw std::out_of_range("Pixel index out of range");
    return data[index];
}

uint8_t& FrameGrayscale::at(const int x, const int y) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        throw std::out_of_range("Coordinates out of range");
    return data[y * width + x];
}

FrameGrayscale::FrameGrayscale() : width(0), height(0){}

FrameGrayscale::FrameGrayscale(int width, int height) : width(width), height(height)
{
    size_t size = width * height;
    data.resize(size);
}