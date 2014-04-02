#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "openGL.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "cubesMesh.hpp"
#include "image3D.hpp"
#include "trabecula/analyze_loader.hpp"

class Scene
{
public:
    Scene();
    ~Scene();

    bool initialize();
    void destroy();
    void addObject(const unsigned char* data, const Trabecula::Sizes& sizes, const Eigen::Vector4f m[2]);

    void resetCamera();
    void updateCameraViewMatrix();
    void updateCameraProjectionMatrix();

    void resize(int width, int height);
    void draw(float etime);

    void keyPressEvent(QKeyEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void wheelEvent(QWheelEvent * e);

    const Camera& camera() const;
    Camera& camera();
    void setCamera(const Camera& camera);


protected:
    // Shaders
    Shader mSimpleProgram;

    // Meshes
    std::vector<CubesMesh*> mMeshes;

    std::vector<Image3D*> mImages3D;
    Camera mCamera;
    int mSwitch;
    float mCamPhy;
    float mCamTheta;
    float mCamDist;
    Eigen::Vector3f mCamLookAt;
    float mCamFov;


    float mTime;

    int mWidth;
    int mHeight;

    QPoint mLastMousePos;
};

#endif // SCENE_HPP
