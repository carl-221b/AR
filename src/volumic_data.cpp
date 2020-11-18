#include "volumic_data.h"

#include <stdexcept>
#include<iostream>

VolumicData::VolumicData()
    : width(-1), height(-1), depth(-1), pixel_width(-1), pixel_height(-1),
      slice_spacing(0) {}
VolumicData::VolumicData(int W, int H, int D)
    : data(W * H * D), width(W), height(H), depth(D) {}

VolumicData::VolumicData(const VolumicData &other)
    : data(other.data), width(other.width), height(other.height),
      depth(other.depth), pixel_width(other.pixel_width),
      pixel_height(other.pixel_height), slice_spacing(other.slice_spacing) {}

VolumicData::~VolumicData() {}

unsigned char VolumicData::getValue(int col, int row, int layer) {
  return data[col + row * width + layer * width * height];
}

void VolumicData::setLayer(unsigned char *layer_data, int layer) {
  if (layer >= depth)
    throw std::out_of_range(
        "Layer " + std::to_string(layer) +
        " is outside of volume (depth=" + std::to_string(depth) + ")");
  int offset = width * height * layer;
  for (int i = 0; i < width * height; i++) {
    data[i + offset] = layer_data[i];
  }
}
