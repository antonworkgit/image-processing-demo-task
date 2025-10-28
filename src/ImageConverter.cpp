#include "ImageConverter.h"
#include "Frame/FrameGrayscale.h"

// Demo only
// Unsafe operations
// No SIMD/AVX

FrameYUV444 ImageConverter::RGBToYUV(FrameRGB& frameRGB) {
    FrameYUV444 frameYUV(frameRGB.width, frameRGB.height);

    for (int y = 0; y < frameRGB.height; ++y) {
        for (int x = 0; x < frameRGB.width; ++x) {
            uint8_t& R = frameRGB.at(x, y, RGBChannel::R);
            uint8_t& G = frameRGB.at(x, y, RGBChannel::G);
            uint8_t& B = frameRGB.at(x, y, RGBChannel::B);

            uint8_t Y = static_cast<uint8_t>(0.299 * R + 0.587 * G + 0.114 * B);
            uint8_t U = static_cast<uint8_t>(-0.147 * R - 0.289 * G + 0.436 * B);
            uint8_t V = static_cast<uint8_t>(0.615 * R - 0.515 * G - 0.100 * B);

            frameYUV.at(x, y, YUVChannel::Y) = Y;
            frameYUV.at(x, y, YUVChannel::U) = U;
            frameYUV.at(x, y, YUVChannel::V) = V;
        }
    }

    return frameYUV;
}

FrameRGB ImageConverter::YUVToRGB(FrameYUV444& frameYUV) {
    FrameRGB frameRGB(frameYUV.width, frameYUV.height);

    for (int y = 0; y < frameYUV.height; ++y) {
        for (int x = 0; x < frameYUV.width; ++x) {
            uint8_t& Y = frameYUV.at(x, y, YUVChannel::Y);
            uint8_t& U = frameYUV.at(x, y, YUVChannel::U);
            uint8_t& V = frameYUV.at(x, y, YUVChannel::V);

            uint8_t R = static_cast<uint8_t>(Y + 1.140 * V);
            uint8_t G = static_cast<uint8_t>(Y - 0.395 * U - 0.581 * V);
            uint8_t B = static_cast<uint8_t>(Y + 2.032 * U);

            frameRGB.at(x, y, RGBChannel::R) = R;
            frameRGB.at(x, y, RGBChannel::G) = G;
            frameRGB.at(x, y, RGBChannel::B) = B;
        }
    }

    return frameRGB;
}

FrameGrayscale ImageConverter::YUVToGrayscale(FrameYUV444& frameYUV) {
    FrameGrayscale frameGrayscale(frameYUV.width, frameYUV.height);

    for (int y = 0; y < frameYUV.height; ++y) {
        for (int x = 0; x < frameYUV.width; ++x) {
            uint8_t& Y = frameYUV.at(x, y, YUVChannel::Y);

            frameGrayscale.at(x, y) = Y;
        }
    }

    return frameGrayscale;
}

FrameRGB ImageConverter::GrayscaleToRGB(FrameGrayscale& frameGrayscale) {
    FrameRGB frameRGB(frameGrayscale.width, frameGrayscale.height);

    for (int y = 0; y < frameGrayscale.height; ++y) {
        for (int x = 0; x < frameGrayscale.width; ++x) {
            uint8_t& gray = frameGrayscale.at(x, y);
            
            frameRGB.at(x, y, RGBChannel::R) = gray;
            frameRGB.at(x, y, RGBChannel::G) = gray;
            frameRGB.at(x, y, RGBChannel::B) = gray;
        }
    }

    return frameRGB;
}