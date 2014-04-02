#include <cmath>

#include "camera.hpp"

Camera::Camera()
  : mCameraPosition(Eigen::Vector3f::Zero()),
    mViewMatrix(Eigen::Matrix4f::Identity()),
    mProjectionMatrix(Eigen::Matrix4f::Identity())
{
}

Camera::~Camera()
{
}

void Camera::lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up)
{ 
  mCameraPosition = position;

  Eigen::Matrix3f L;
  L.row(2) = (position - target).normalized();
  L.row(0) = up.cross(L.row(2)).normalized();
  L.row(1) = L.row(2).cross(L.row(0)).normalized();

  mViewMatrix.block<3, 3>(0, 0) = L;
  mViewMatrix.block<3, 1>(0, 3) = L * (-position);
}

void Camera::setPerspective(float left, float right, float top, float bottom,
                            float near, float far, float focus)
{
  mProjectionMatrix <<
    2./(right-left),               0, (right+left)/(focus*(right-left)),                               0,
                  0, 2./(top-bottom), (top+bottom)/(focus*(top-bottom)),                               0,
                  0,               0,    -(far+near)/(focus*(far-near)), -2.*far*near/(focus*(far-near)),
                  0,               0,                          -1/focus,                               0;
}

void Camera::setPerspective(float fovX, float aspect, float near, float far, float focus)
{
  float r = std::tan(fovX/2.) * focus;
  float t = r / aspect;
  setPerspective(-r, r, t, -t, near, far, focus);
}

void Camera::setOrthogonal(float left, float right, float bottom, float top, float near, float far)
{
  float rMl = 1.0f / (right - left);
  float tMb = 1.0f / (top - bottom);
  float fMn = 1.0f / (far - near);

  mProjectionMatrix <<
    2.0f*rMl, 0       , 0       , -(right+left)*rMl,
    0       , 2.0f*tMb, 0       , -(top+bottom)*tMb,
    0       , 0       ,-2.0f*fMn, -(far+near)*fMn  ,
    0       , 0       , 0       , 1.0f             ;
}

const Eigen::Vector3f& Camera::getPosition() const
{
  return mCameraPosition;
}

const Eigen::Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

const Eigen::Matrix4f& Camera::projectionMatrix() const
{
  return mProjectionMatrix;
}
