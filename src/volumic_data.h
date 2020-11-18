#ifndef VOLUMIC_DATA_H
#define VOLUMIC_DATA_H

#include <vector>

class VolumicData {
public:
  // The data from the volume stored:
  // - column by column
  // - line by line
  // - slice by slice
  std::vector<unsigned char> data;

  int width;
  int height;
  int depth;

  double pixel_width;
  double pixel_height;
  double slice_spacing;

  // TODO: add pixel_width, pixel_height and pixel_depth

  // The data provided
  VolumicData();
  VolumicData(int width, int height, int depth);
  VolumicData(const VolumicData &other);
  ~VolumicData();

  unsigned char getValue(int col, int row, int layer);

  void setLayer(unsigned char *layer_data, int layer);
};

#endif // VOLUMIC_DATA_H
