#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QString>

#include <memory>

#include "volumic_data.h"
#include "raw_data.h"

class GLWidget : public QOpenGLWidget {
public:
  Q_OBJECT
public:
  enum ViewType { ORTHO, FRUSTUM };

  GLWidget(QWidget *parent = 0);
  ~GLWidget();
  QSize sizeHint() const { return QSize(200, 200); }

  float getAlpha() const;

  void setCurrentSlice(int slice) { current_slice = slice; }
  void setHideEmptyPoints(bool check) { hide_empty_points = check; }
  void setHighlight(bool check) { highlight = check; }
  void setHideAbove(bool check) { hide_above = check; }
  void setHideBelow(bool check) { hide_below = check; }
  void setBitEncode(bool check) { change_bit_encode = check; }

  bool getHighlight() { return highlight; }
  bool getHideAbove() { return hide_above; }
  bool getHideBelow() { return hide_below; }
  bool getBitEncode() { return change_bit_encode; }

  int getK(){return k;}

  void updateVolumicData(std::unique_ptr<VolumicData> new_data);
  void updateRawData(std::unique_ptr<RawData> new_data);

public slots:
  void setAlpha(double new_alpha);
  void setK(int new_k);
  void setProj(int index);

protected:
  struct DrawablePoint {
    QVector3D pos;
    int depth;
    double c;
    int vol_idx;
  };

  void initializeGL() override;
  void paintGL() override;

  void updateDisplayPoints();

  void wheelEvent(QWheelEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

  /**
   * If 'shift' modifier is pressed, multiplies value by 10
   */
  double modifiedDelta(double delta);

  QPoint lastPos;
  float alpha;
  int k; 
  /**
   * Zoom value using a log scale
   * - positive is zoom in
   * - negative is zoom out
   */
  float log2_zoom;
  QMatrix4x4 transform;

  ViewType view_type;

  /// When enabled, all points with a drawing color = 0 are hidden
  bool hide_empty_points;
  bool highlight, hide_above, hide_below, change_bit_encode;

  /// The data of all the slices stored in a single object
  std::unique_ptr<VolumicData> volumic_data;
  std::unique_ptr<RawData> raw_data;

  /// The points to be drawn
  std::vector<DrawablePoint> display_points;
private:
  int current_slice;
  
  void getColor(DrawablePoint point, double alpha);
};

#endif // GLWIDGET_H