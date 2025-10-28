#pragma once

void startConversion();
void signalHandler(int sig);
void loadImagesWorker();
void convertRawRGBToYUVWorker();
void convertYUVToGrayscaleWorker();
void convertGrayscaleToRGBWorker();
void saveProcessedWorker();