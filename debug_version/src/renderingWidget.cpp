
#include <iostream>

#include "renderingWidget.hpp"

RenderingWidget::RenderingWidget()
    :
#ifdef __APPLE__
    QGLWidget(new Core3_2_context(QGLFormat::defaultFormat())),
#endif
        mScene(NULL),

        mRefreshRate(30),
        mTimer(new QTimer(this))
{
}

RenderingWidget::~RenderingWidget()
{
    delete mScene;
}

void RenderingWidget::setScene(Scene* scene)
{
    mScene = scene;
}

void RenderingWidget::paintGL()
{
    GL_TEST_ERR;

    if(mScene)
        mScene->draw(mRefreshRate/1000.);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GL_TEST_ERR;
}

void RenderingWidget::initializeGL()
{
    std::cout << "Using OpenGL version: \"" << glGetString(GL_VERSION) << "\"" << std::endl;
    std::cout << "OpenGL context: " << context()->format().majorVersion()
                        << "." << context()->format().minorVersion()
                        << " " << ((context()->format().profile() == QGLFormat::CoreProfile)? "Core":
                                             (context()->format().profile() == QGLFormat::CompatibilityProfile)? "Compatibility":
                                             "No profile")
                        << "\n";    
    std::cout << std::endl;

    glClearColor(0.,0.,0.,0.);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 

    bool ok = mScene->initialize();
    assert(ok);

    // Start a timer that redraw the scene.
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    mTimer->start(mRefreshRate);
}

void RenderingWidget::resizeGL(int width, int height)
{
    if(mScene)
        mScene->resize(width, height);
}

void RenderingWidget::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
        case Qt::Key_Up:
            break;
        case Qt::Key_Down:
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        case Qt::Key_Escape:
        {
            exit(EXIT_SUCCESS);
        }    

        default:
            break;
    }

    if(mScene)
        mScene->keyPressEvent(e);

    updateGL();
}

void RenderingWidget::mousePressEvent(QMouseEvent * e) {
    if(mScene) {
        mScene->mousePressEvent(e);
        if(e->isAccepted())
            updateGL();
    }
}

void RenderingWidget::mouseReleaseEvent(QMouseEvent * e) {
    if(mScene) {
        mScene->mouseReleaseEvent(e);
        if(e->isAccepted())
            updateGL();
    }
}

void RenderingWidget::mouseMoveEvent(QMouseEvent * e) {
    if(mScene) {
        mScene->mouseMoveEvent(e);
        if(e->isAccepted())
            updateGL();
    }
}

void RenderingWidget::wheelEvent(QWheelEvent * e) {
    if(mScene) {
        mScene->wheelEvent(e);
        if(e->isAccepted())
            updateGL();
    }
}
