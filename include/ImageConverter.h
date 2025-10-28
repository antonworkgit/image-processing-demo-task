#pragma once
#include "Frame/FrameRGB.h"
#include "Frame/FrameYUV444.h"
#include "Frame/FrameGrayscale.h"

class ImageConverter {
public:
    static FrameYUV444 RGBToYUV(FrameRGB& frameRGB);
    static FrameRGB YUVToRGB(FrameYUV444& frameYUV);
    static FrameGrayscale YUVToGrayscale(FrameYUV444& frameYUV);
    static FrameRGB GrayscaleToRGB(FrameGrayscale& frameGrayscale);
};