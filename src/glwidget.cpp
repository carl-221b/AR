#include <QMessageBox>
#include <QString>
#include <QTransform>
#include <QtGui>

#include "glwidget.h"

#include <iostream>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent), alpha(0.05), log2_zoom(0),
      view_type(ViewType::ORTHO),hide_empty_points(false),highlight(false),hide_above(false),hide_below(false),change_bit_encode(false){
  QSizePolicy size_policy;
  size_policy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
  size_policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
  setSizePolicy(size_policy);
}

GLWidget::~GLWidget() {}

float GLWidget::getAlpha() const { return alpha; }

void GLWidget::setAlpha(double new_alpha) {
  alpha = new_alpha;
  update();
}

void GLWidget::setK(int new_k){
  k = new_k;
  update();
}

void GLWidget::setProj(int index) {
  if(index == 0)
    view_type = ViewType::ORTHO;
  else
    view_type = ViewType::FRUSTUM;
  update();
}

void GLWidget::updateVolumicData(std::unique_ptr<VolumicData> new_data) {
  volumic_data = std::move(new_data);
  updateDisplayPoints();
}

void GLWidget::updateRawData(std::unique_ptr<RawData> new_data) {
  raw_data = std::move(new_data);
  updateDisplayPoints();
}

void GLWidget::updateDisplayPoints() {
  display_points.clear();
  if (!volumic_data)
    return;
  int W = volumic_data->width;
  int H = volumic_data->height;
  int D = volumic_data->depth;
  int col = 0;
  int row = 0;
  int depth = 0;
  double x_factor = volumic_data->pixel_width;
  double y_factor = volumic_data->pixel_height;
  double z_factor = volumic_data->slice_spacing;
  double max_size =
      std::max(std::max(x_factor * W, y_factor * H), z_factor * D);
  double global_factor = 2.0 / max_size;
  x_factor *= global_factor;
  y_factor *= global_factor;
  z_factor *= global_factor;
  int idx_start = 0;
  int idx_end = W * H * D;
  display_points.reserve(idx_end - idx_start);
  // Importing points
  for (int idx = idx_start; idx < idx_end; idx++) {
    double c;
    DrawablePoint p;
    /* 8-bit colors */
    if(!change_bit_encode){
      c = volumic_data->data[idx] / 255.0;
      p.c = c;
    /* 16-bit colors */
    } else {
      c = raw_data->data[idx];
      if(c < raw_data->w_min){
        p.c = 0.0;
        p.vol_idx = -1;
      } else if(c >= raw_data->w_max) {
        p.c = 1.0;
        if(c == raw_data->w_max)
          p.vol_idx = k;
        else
          p.vol_idx = -1;
      } else {
        p.c = (c - raw_data->w_min) / (raw_data->w_max - raw_data->w_min);
        p.vol_idx = floor(k * p.c);
      }
    }
    p.pos = QVector3D((col - W / 2.) * x_factor, (row - H / 2.) * y_factor,
                      (depth - D / 2.) * z_factor);
    p.depth = depth;
    display_points.push_back(p);
    col++;
    if (col == W) {
      row++;
      col = 0;
    }
    if (row == H) {
      depth++;
      row = 0;
    }
  }
  //std::cout << "Nb points: " << display_points.size() << std::endl;
}

void GLWidget::initializeGL() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_NEVER);
}

void GLWidget::getColor(DrawablePoint point, double alpha) {
  if(change_bit_encode){
    switch(point.vol_idx){
      case 0:
        glColor4f(1.0, 0.0, 0.0, alpha);
        break;
      case 1:
        glColor4f(0.0, 1.0, 0.0, alpha);
        break;
      case 2:
        glColor4f(0.0, 0.0, 1.0, alpha);
        break;
      case 3:
        glColor4f(1.0, 1.0, 0.0, alpha);
        break;
      case 4:
        glColor4f(1.0, 0.0, 1.0, alpha);
        break;
      case 5:
        glColor4f(0.0, 1.0, 1.0, alpha);
        break;
      case 6:
        glColor4f(1.0, 1.0, 1.0, alpha);
        break;
    }
  } else {
    glColor4f(point.c, point.c, point.c, alpha);
  }
}

void GLWidget::paintGL() {
  QSize viewport_size = size();
  int width = viewport_size.width();
  int height = viewport_size.height();
  double aspect_ratio = width / (float)height;
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  switch (view_type) {
    case ViewType::ORTHO: {
      double view_half_size = std::pow(2, -log2_zoom);
      glScalef(1.0, aspect_ratio, 1.0);
      QVector3D center(0, 0, 0);
      glOrtho(center.x() - view_half_size, center.x() + view_half_size,
              center.y() - view_half_size, center.y() + view_half_size,
              center.z() - view_half_size, center.z() + view_half_size);
      glMultMatrixf(transform.constData());
      break;
    }
    case ViewType::FRUSTUM: {
      float near_dist = 0.5;
      float far_dist = 5.0;
      QMatrix4x4 projection;
      projection.perspective(90, aspect_ratio, near_dist, far_dist);
      QMatrix4x4 cam_offset;
      cam_offset.translate(0, 0, -2 * (1 - log2_zoom));
      QMatrix4x4 pov = projection * cam_offset * transform;
      glMultMatrixf(pov.constData());
    }
  }
  
  glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glBegin(GL_POINTS);
  for (const DrawablePoint &p : display_points) {
    if (p.vol_idx != -1 || !change_bit_encode){
      if ((hide_empty_points && p.c == 0) ||
        (hide_above && p.depth > current_slice) ||
          (hide_below && p.depth < current_slice))
      {
        getColor(p, 0.0);
      }
      else{
        if (highlight && p.depth == current_slice)
        {
          getColor(p, 1.0);
        }
        else{
          getColor(p, alpha);
        }
      }
      glVertex3d(p.pos.x(), p.pos.y(), p.pos.z());
    }
  }
  glEnd();
}

void GLWidget::mousePressEvent(QMouseEvent *event) { lastPos = event->pos(); }

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  double dx = modifiedDelta(event->x() - lastPos.x());
  double dy = modifiedDelta(event->y() - lastPos.y());

  if (event->buttons() & Qt::LeftButton) {
    QQuaternion local_rotation =
        QQuaternion::fromEulerAngles(0.5 * dy, 0.5 * dx, 0);
    QMatrix4x4 local_matrix;
    local_matrix.rotate(local_rotation);
    transform = local_matrix * transform;
  } else if (event->buttons() & Qt::RightButton) {
    QMatrix4x4 local_matrix;
    local_matrix.translate(QVector3D(dx, -dy, 0) * 0.001);
    transform = local_matrix * transform;
  }

  lastPos = event->pos();
  update();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
  double delta = modifiedDelta(event->delta() / 1000.0);
  log2_zoom += delta;
  update();
}

double GLWidget::modifiedDelta(double delta) {
  if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier) {
    return 10 * delta;
  }
  return delta;
}


