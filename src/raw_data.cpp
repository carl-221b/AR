#include "raw_data.h"

#include <iostream>
#include <stdexcept>
#include <cmath>

RawData::RawData()
    : width(-1), height(-1), depth(-1), pixel_width(-1), pixel_height(-1),
      slice_spacing(0) {}

RawData::RawData(int W, int H, int D)
    : data(W * H * D), width(W), height(H), depth(D) {}

RawData::RawData(const RawData &other)
    : data(other.data), width(other.width), height(other.height),
      depth(other.depth), pixel_width(other.pixel_width),
      pixel_height(other.pixel_height), slice_spacing(other.slice_spacing) {}

RawData::~RawData() {}

void RawData::setWindow(double window_center, double window_width,
                        double collec_min, double collec_max){
  /* Maybe change the args to get only the window parameters */
  double min = window_center - window_width;
  double max = window_center + window_width;
  int offset = (collec_max - collec_min) / 2 + (pow(2,15) - collec_max); ; 
  w_min = min + offset;
  w_max = max + offset;
}

uint16_t RawData::getValue(int col, int row, int layer) {
  return data[col + row * width + layer * width * height];
}

void RawData::setLayer(uint16_t *layer_data, int layer) {
  if (layer >= depth)
    throw std::out_of_range(
        "Layer " + std::to_string(layer) +
        " is outside of volume (depth=" + std::to_string(depth) + ")");
  int offset = width * height * layer;
  for (int i = 0; i < width * height; i++) {
    data[i + offset] = layer_data[i];
  }
}
