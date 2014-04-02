
#ifndef RENDERINGWIDGET_HPP
#define RENDERINGWIDGET_HPP

#include "openGL.hpp"

#include <Eigen/Geometry>

#include <QGLWidget>
#include <QApplication>
#include <QTimer>
#include <QKeyEvent>

#include "scene.hpp"

class RenderingWidget : public QGLWidget
{
  Q_OBJECT

  Scene* mScene;

  // Other
  int mRefreshRate;
  QTimer* mTimer;

protected:

  /** This method is automatically called by Qt once the GL context has been created.
    * It is called only once per execution */
  virtual void initializeGL();

  /** This method is automatically called by Qt everytime the opengl windows is resized.
    * \param width the new width of the windows (in pixels)
    * \param height the new height of the windows (in pixels)
    *
    * This function must never be called directly. To redraw the windows, emit the updateGL() signal:
      \code
      emit updateGL();
      \endcode
    */
  virtual void resizeGL(int width, int height);

  /** This method is automatically called by Qt everytime the opengl windows has to be refreshed. */
  virtual void paintGL();

  /** This method is automatically called by Qt everytime a key is pressed */
  void keyPressEvent(QKeyEvent * e);

  void mousePressEvent(QMouseEvent * e);
  void mouseReleaseEvent(QMouseEvent * e);
  void mouseMoveEvent(QMouseEvent * e);
  void wheelEvent(QWheelEvent * e);

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  // default constructor
  RenderingWidget();
  ~RenderingWidget();

  void setScene(Scene* scene);

};

#endif // RENDERINGWIDGET_HPP

