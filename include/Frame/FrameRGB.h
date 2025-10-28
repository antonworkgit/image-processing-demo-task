#pragma once
#include <vector>
#include "ColorChannel.h"

struct FrameRGB {
	int width;
	int height;
	std::vector<uint8_t> data;

	uint8_t& getPixel(int index, RGBChannel channel);
	uint8_t& at(int x, int y, RGBChannel channel);

	FrameRGB();
	FrameRGB(int width, int height);
	FrameRGB(int width, int height, uint8_t* raw_data);
};