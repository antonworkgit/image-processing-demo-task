#include <csignal>
#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include "main.h"
#include "SPSC_DataBuffer.h"
#include "Frame/FrameRGB.h"
#include "Frame/FrameYUV444.h"
#include "Frame/FrameGrayscale.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "ImageConverter.h"

#define DEBUG true

// C++ 17 (filesystem)
namespace fs = std::filesystem;

constexpr int RAW_BUFFER_SIZE = 5;
constexpr int YUV_BUFFER_SIZE = 5;
constexpr int GRAYSCALE_BUFFER_SIZE = 5;
constexpr int PROCESSED_BUFFER_SIZE = 5;

constexpr int OUTPUT_JPG_QUALITY = 80;
constexpr const char* JPG_EXTENSION = ".jpg";

SPSC_DataBuffer<FrameRGB> rawBuffer(RAW_BUFFER_SIZE);
SPSC_DataBuffer<FrameYUV444> yuvBuffer(YUV_BUFFER_SIZE);
SPSC_DataBuffer<FrameGrayscale> grayscaleBuffer(GRAYSCALE_BUFFER_SIZE);
SPSC_DataBuffer<FrameRGB> processedBuffer(PROCESSED_BUFFER_SIZE);

fs::path inputDir;
fs::path outputDir;

int main(int argc, char** argv)
{
	std::signal(SIGINT, signalHandler);

#if DEBUG
	const char* debug_argv[] = {
		"program",
		"sample/input",
		"sample/output"
	};
	argc = 3;
	argv = const_cast<char**>(debug_argv);
#endif

	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <input_dir> <output_dir>\n";
		return EXIT_FAILURE;
	}

	inputDir = argv[1];
	outputDir = argv[2];

	if (!fs::exists(inputDir)) {
		std::cerr << "Error: input directory does not exist " << inputDir << "\n";
			return EXIT_FAILURE;
	}

	if (!fs::exists(outputDir)) {
		std::cerr << "Error: output directory does not exist. Creating " << outputDir << "\n";
		if (!fs::create_directories(outputDir)) {
			std::cerr << "Failed to create output directory!\n";
			return EXIT_FAILURE;
		}
		std::cout << "Output directory created: " << outputDir << "\n";
	}

	std::cout << "Starting processing...\n" << "Press Ctrl + C to stop the program\n";

	startConversion();

	std::cout << "Completed\n";

	return EXIT_SUCCESS;
}

void signalHandler(int sig) {
	if (sig == SIGINT) {
		std::cout << "\nCancelling the program...\n" << "The current writing frame will be written safely.\n";
		rawBuffer.cancel_operations();
		yuvBuffer.cancel_operations();
		grayscaleBuffer.cancel_operations();
		processedBuffer.cancel_operations();
	}
}

void startConversion()
{
	std::thread loadImagesWorkerThread(loadImagesWorker);
	std::thread convertRawRGBToYUVThread(convertRawRGBToYUVWorker);
	std::thread convertYUVToGrayscaleThread(convertYUVToGrayscaleWorker);
	std::thread convertGrayscaleToRGBThread(convertGrayscaleToRGBWorker);
	std::thread saveProcessedThread(saveProcessedWorker);

	loadImagesWorkerThread.join();
	convertRawRGBToYUVThread.join();
	convertYUVToGrayscaleThread.join();
	convertGrayscaleToRGBThread.join();
	saveProcessedThread.join();
}

#pragma region Workers

void loadImagesWorker() {
	try {
		for (const auto& entry : fs::directory_iterator(inputDir)) {
			if (!rawBuffer.is_cancelled() && fs::is_regular_file(entry)) {
				std::string filepath = entry.path().string();

				int x, y, n;
				int ok = stbi_info(filepath.c_str(), &x, &y, &n);

				if (!ok) {
					std::cerr << "Failed to read image info for: " << filepath << '\n';
					continue;
				}

				if (n != RGB_NUM_CHANNELS) {
					std::cerr << "Image " << filepath << " does not have 3 channels. Channels: " << n << '\n';
					continue;
				}

				uint8_t* data = stbi_load(filepath.c_str(), &x, &y, &n, RGB_NUM_CHANNELS);

				if (!data) {
					std::cerr << "Failed to load image data for: " << filepath << '\n';
					continue;
				}

				rawBuffer.push(FrameRGB(x, y, data));
				stbi_image_free(data);
			}
		}

		rawBuffer.finish_writing();
	}
	catch (const std::exception& e) {
		std::cerr << "Error iterating directory: " << e.what() << '\n';
	}
}

void convertRawRGBToYUVWorker() {
	FrameRGB frame;
	while (!rawBuffer.is_cancelled() && rawBuffer.try_pop(frame)) {
		yuvBuffer.push(ImageConverter::RGBToYUV(frame));
	}
	yuvBuffer.finish_writing();
}

void convertYUVToGrayscaleWorker() {
	FrameYUV444 frame;
	while (!yuvBuffer.is_cancelled() && yuvBuffer.try_pop(frame)) {
		grayscaleBuffer.push(ImageConverter::YUVToGrayscale(frame));
	}
	grayscaleBuffer.finish_writing();
}

void convertGrayscaleToRGBWorker() {
	FrameGrayscale frame;
	while (!grayscaleBuffer.is_cancelled() && grayscaleBuffer.try_pop(frame)) {
		processedBuffer.push(ImageConverter::GrayscaleToRGB(frame));
	}
	processedBuffer.finish_writing();
}

void saveProcessedWorker() {
	try {
		FrameRGB frame;
		int frameCounter = 0;
		while (!processedBuffer.is_cancelled() && processedBuffer.try_pop(frame)) {
			int w = frame.width;
			int h = frame.height;
			uint8_t* data = frame.data.data();

			// workaround. forgot to store names
			std::ostringstream oss;
			oss << "frame_" << ++frameCounter << JPG_EXTENSION;
			std::string uniquename = oss.str();

			fs::path filepath = outputDir / uniquename;
			if (!stbi_write_jpg(filepath.string().c_str(), frame.width, frame.height, RGB_NUM_CHANNELS, frame.data.data(), OUTPUT_JPG_QUALITY)) {
				std::cerr << "Error saving file: " << filepath << '\n';
				continue;
			}
			std::cout << "Successfully saved processed file: " << filepath << '\n';
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error saving: " << e.what() << '\n';
	}
}

#pragma endregion