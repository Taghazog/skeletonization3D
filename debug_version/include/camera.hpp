#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Geometry>

class Camera
{
  public:
    Camera();
    virtual ~Camera();

    void lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up);
    void setPerspective(float left, float right, float top, float bottom,
                        float near, float far, float focus=.4);
    void setPerspective(float fovX, float aspect, float near, float far,
                        float focus=.4);
    void setOrthogonal(float left, float right, float bottom, float top,
                       float near, float far);
    
    const Eigen::Vector3f& getPosition(void) const;
    const Eigen::Matrix4f& viewMatrix(void) const;
    const Eigen::Matrix4f& projectionMatrix(void) const;
    
  protected:
    Eigen::Vector3f mCameraPosition;
    Eigen::Matrix4f mViewMatrix;
    Eigen::Matrix4f mProjectionMatrix;
};

#endif // EIGEN_CAMERA_H
