//#include <QImage>

#include "scene.hpp"
#include "image3D.hpp"

Scene::Scene()
    :   mSimpleProgram(),
        mSwitch(0),
        mCamera(),
        mCamPhy(0.5),
        mCamTheta(0.2),
        mCamDist(100.0),
        mCamLookAt(Eigen::Vector3f(154/2.0, 154/2.0, 154/2.0)),
        mCamFov(M_PI_2),

        mTime(0.),

        mWidth(-1),
        mHeight(-1),

        mLastMousePos()
{

}

Scene::~Scene()
{

}

bool Scene::initialize()
{
    resetCamera();

    GL_TEST_ERR;

    // Shaders
    bool ok = mSimpleProgram.loadFromFiles(
         TRABECULA_DIR"/shaders/simple.vert",
         TRABECULA_DIR"/shaders/simple.frag");

    GL_TEST_ERR;

    for(int i = 0; i < mMeshes.size(); ++i)
    {
        mMeshes[i]->initBuffers();
        mImages3D[i]->attachShader(&mSimpleProgram);
        mImages3D[i]->attachMesh(mMeshes[i]);
    }

    return ok;
}

void Scene::destroy()
{
    delete &mMeshes;
    delete &mImages3D;
}

void Scene::addObject(const unsigned char* data, const Trabecula::Sizes& sizes, const Eigen::Vector4f m[2])
{
    mMeshes.push_back(new CubesMesh(data, sizes));
    mImages3D.push_back(new Image3D(Eigen::Vector3f::Zero(), m));
}

void Scene::resetCamera()
{
    mCamPhy = 0.5;
    mCamTheta = 0.2;
    mCamDist = 100.0;
    mCamLookAt = Eigen::Vector3f(154/2.0, 154/2.0, 154/2.0);
    mCamFov = M_PI_2;
}

void Scene::updateCameraViewMatrix()
{
    Eigen::Vector3f cameraPos = mCamLookAt + mCamDist * Eigen::Vector3f(
                std::sin(mCamPhy)*std::cos(mCamTheta),
                std::sin(mCamTheta),
                std::cos(mCamPhy)*std::cos(mCamTheta));

    mCamera.lookAt(cameraPos, mCamLookAt, Eigen::Vector3f::UnitY());
}

void Scene::updateCameraProjectionMatrix()
{
    mCamera.setPerspective(mCamFov, float(mWidth)/float(mHeight), .01, 250.);
}

void Scene::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    glViewport(0, 0, width, height);
    updateCameraProjectionMatrix();
}

void Scene::draw(float etime)
{
    mTime += etime;

    GL_TEST_ERR;

    ////// Final rendering //////
    updateCameraViewMatrix();


    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mImages3D[mSwitch]->draw(mCamera);

    GL_TEST_ERR;
}

void Scene::keyPressEvent(QKeyEvent* e)
{
    switch(e->key())
    {
        case Qt::Key_A:
        {
            mSwitch = (mSwitch + 1) % mImages3D.size();
            break;
        }

        default:
            break;
    }
}

void Scene::mousePressEvent(QMouseEvent* e)
{
    mLastMousePos = e->pos();
    e->accept();
}

void Scene::mouseReleaseEvent(QMouseEvent* e)
{
    mLastMousePos = e->pos();
    e->accept();
}

void Scene::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        mCamPhy += -float(e->x() - mLastMousePos.x()) / 256.;
        mCamTheta += float(e->y() - mLastMousePos.y()) / 256.;
        mCamTheta = std::min(M_PI_2-.001, std::max(-M_PI_2+0.001, double(mCamTheta)));
    }
    else if(e->buttons() & Qt::MiddleButton)
    {
        float offset = mCamDist * std::tan(mCamFov/mWidth);
        Eigen::Vector3f z = mCamera.getPosition()-mCamLookAt;
        Eigen::Vector3f x = offset * (Eigen::Vector3f::UnitY().cross(z)).normalized();
        Eigen::Vector3f y = offset * (z.cross(x)).normalized();
        mCamLookAt += -x*(e->x() - mLastMousePos.x()) +
                                     y*(e->y() - mLastMousePos.y());
    }
    mLastMousePos = e->pos();
    e->accept();
}

void Scene::wheelEvent(QWheelEvent * e)
{
    mCamDist *= (e->delta()>0)? 1./1.1: 1.1;
    e->accept();
}

const Camera& Scene::camera() const
{
    return mCamera;
}

Camera& Scene::camera()
{
    return mCamera;
}

void Scene::setCamera(const Camera& camera)
{
    mCamera = camera;
}