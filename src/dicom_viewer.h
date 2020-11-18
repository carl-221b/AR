#ifndef DICOM_VIEWER_H
#define DICOM_VIEWER_H

#include <QGridLayout>
#include <QMainWindow>
#include <QCheckBox>
#include <QComboBox>

#include <map>
#include <memory>

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include "double_slider.h"
#include "glwidget.h"
#include "image_label.h"
#include "int_slider.h"

class DicomViewer : public QMainWindow {
  Q_OBJECT

public:
  DicomViewer(QWidget *parent = 0);
  ~DicomViewer();
  QSize sizeHint() const { return QSize(600, 400); }

public slots:
  void openDicomCollection();
  void showStats();
  void save();

  void onSliceChange(int new_slice);
  void onWindowCenterChange(double new_window_center);
  void onWindowWidthChange(double new_window_width);

  void onCheckHide2dChange(bool check);
  void onCheckHide3dChange(bool check);
  void onCheckHideEmptyPointsChange(bool check);
  void onCheckHighlightChange(bool check);
  void onCheckHideAboveChange(bool check);
  void onCheckHideBelowChange(bool check);
  void onCheckBitsChange(bool check);

private:
  QWidget *widget;
  QGridLayout *layout;

  IntSlider *slice_slider;
  IntSlider *k_slider;
  DoubleSlider *alpha_slider;
  DoubleSlider *window_center_slider;
  DoubleSlider *window_width_slider;

  /// The area in which the image is shown
  ImageLabel *img_label;

  /// The container for display of volumic data
  GLWidget *gl_widget;

  /// Images options
  QCheckBox *check_hide_2d;
  QCheckBox *check_hide_3d;
  QCheckBox *check_hide_empty_points;
  QCheckBox *check_highlight;
  QCheckBox *check_hide_above;
  QCheckBox *check_hide_below;
  QCheckBox *use_16_bits;
  QComboBox *proj_view;

  /// The files loaded by the DicomViewer, indexed by acquisition number
  std::map<int, std::unique_ptr<DcmFileFormat>> active_files;

  /// The current displayed layer
  int current_layer;

  /// The lowest instance number among active files
  int min_instance;
  /// The highest instance number among active files
  int max_instance;

  /// The active Dicom image
  DicomImage *image;

  /// The 8-bits image to be shown on screen
  uchar *img_data;

  /// The width of a pixel in [mm]
  /// - negative value if no image is loaded
  double pixel_width;
  /// The height of a pixel in [mm]
  /// - negative value if no image is loaded
  double pixel_height;
  /// The space between two consecutive slices [mm]
  /// - 0 if less than 2 images are loaded
  double slice_spacing;

  /// The name of the patient the collection concerns
  std::string patient_name;
  /// Minimal value used among the whole collection
  double collection_min;
  /// Maximal value used among the whole collection
  double collection_max;

  /// Retrieve access to the dataset of active slice
  /// if dataset is not available return nullptr
  DcmDataset *getDataset();

  /// Update min_instance and max_instance based on 'active_files'
  void updateInstanceLimits();

  /// Adjust the range of the slice slider based on 'active_files'
  void updateSliceSlider();

  /// Adjust the size of the window based on file content
  void updateWindowSliders();

  /// Load the DicomImage from the active slice
  /// If there are no active slice available, set image to nullptr
  void loadDicomImage();

  /// Retrive the image from the given dataset
  /// On failure, return nullptr and shows a messagebox
  DicomImage *loadDicomImage(DcmDataset *dataset);

  /// Import the default parameters from the DicomImage
  void applyDefaultWindow();

  /// Update the image based on current status of the object
  void updateImage();

  /// Update the volumic_data element based on active_files
  void updateVolumicData();

  /// Update the raw_data element based on active_files
  void updateRawData();

  /// Retrieve patient name from active file
  /// return 'FAIL' if no active file is found
  std::string getPatientName(DcmDataset *dataset);

  /// Retrieve image from active file, converting to appropriate transfer syntax
  /// return nullptr on failure
  DicomImage *getDicomImage();

  /// Convert current Dicom Image to a QImage according to actual parameters
  QImage getQImage();

  // Returns a two elements vector with [row_spacing, col_spacing] in mm
  std::vector<double> getPixelSpacing(DcmDataset *dataset);

  // Returns the position of the first voxel transmitted in a three elements
  // vector with [x,y,z] in mm
  std::vector<double> getImagePosition(DcmDataset *dataset);

  /// Extract min (and max) used (and allowed) values
  void getMinMax(double *min_used_value, double *max_used_value,
                 double *min_allowed_value = nullptr,
                 double *max_allowed_value = nullptr);

  /// Fill min and max with the extremum values found in the active layer
  void getFrameMinMax(DicomImage *img, double *min, double *max);

  /// Fill min and max with the extremum values found it all the loaded files
  void getCollectionMinMax(double *min, double *max);

  void getWindow(double *min_value, double *max_value);

  double getSlope();
  double getIntercept();

  double getWindowCenter();
  double getWindowWidth();
  double getWindowMin();
  double getWindowMax();

  int getSeriesNumber(DcmDataset *dataset);
  int getInstanceNumber(DcmDataset *dataset);
  int getAcquisitionNumber(DcmDataset *dataset);

  void setCheckBoxes(bool check);
};

template <typename T>
T getField(DcmItem *item, const DcmTagKey &tag_key, unsigned long pos = 0);
template <typename T>
T getField(DcmItem *item, unsigned int g, unsigned int e,
           unsigned long pos = 0) {
  return getField<T>(item, DcmTagKey(g, e), pos);
}

template <>
double getField<double>(DcmItem *item, const DcmTagKey &tag_key,
                        unsigned long pos);
template <>
short int getField<short int>(DcmItem *item, const DcmTagKey &tag_key,
                              unsigned long pos);
template <>
int getField<int>(DcmItem *item, const DcmTagKey &tag_key, unsigned long pos);
template <>
std::string getField<std::string>(DcmItem *item, const DcmTagKey &tag_key,
                                  unsigned long pos);

template <typename T>
std::vector<T> getFieldVector(DcmItem *item, const DcmTagKey &tag_key,
                              int fixed_size) {
  std::vector<T> result(fixed_size);
  for (int i = 0; i < fixed_size; i++) {
    result[i] = getField<T>(item, tag_key, i);
  }
  return result;
}

#endif // DICOM_VIEWER_H