#ifndef RAW_DATA_H
#define RAW_DATA_H

#include <cstdint>
#include <vector>

class RawData {
public:
  std::vector<uint16_t> data;

  int width;
  int height;
  int depth;
  double pixel_width;
  double pixel_height;
  int slice_spacing;
  int w_min;
  int w_max;
  RawData();
  RawData(int width, int height, int depth);
  RawData(const RawData &other);
  ~RawData();

  uint16_t getValue(int col, int row, int layer);

  void setLayer(uint16_t *layer_data, int layer);
  void setWindow(double window_center,double window_width, double collec_min, double collec_max);
};

#endif // RAW_DATA_H
