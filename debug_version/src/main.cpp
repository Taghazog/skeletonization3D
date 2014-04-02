

#include "trabecula/tubular_object.hpp"
#include "renderingWidget.hpp"
#include "scene.hpp"

#include <cstdio>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout << "usage: filename without extension" << std::endl;
        return 0;
    }
    const std::string filename = argv[1];

    Trabecula::Tubular_object* cancellous_bones = new Trabecula::Tubular_object();

    cancellous_bones->load_from_file(filename);
    cancellous_bones->skeletonize();
    cancellous_bones->build_graph();
    //cancellous_bones->save_skeleton();
    //cancellous_bones->dump_infos();

    QApplication app(argc, argv);

    QGLFormat gl_profile(QGL::DoubleBuffer | QGL::Rgba | QGL::AlphaChannel);
    gl_profile.setVersion(3, 3);
    gl_profile.setProfile(QGLFormat::CoreProfile);
    QGLFormat::setDefaultFormat(gl_profile);

    Scene* scene = new Scene();

    // Meshes
    Eigen::Vector4f m[2];
    m[0] = Eigen::Vector4f(0.1, 0.1, 0.0, 1.0); // Ka
    m[1] = Eigen::Vector4f(0.5, 0.4, 0.1, 1.0); // Kd
    scene->addObject(cancellous_bones->data(), cancellous_bones->sizes(), m);

    m[0] = Eigen::Vector4f(0.1, 0.1, 0.0, 1.0); // Ka
    m[1] = Eigen::Vector4f(0.5, 0.1, 0.1, 1.0); // Kd
    scene->addObject(cancellous_bones->skeleton_data(), cancellous_bones->sizes(), m);

    m[0] = Eigen::Vector4f(0.1, 0.1, 0.0, 1.0); // Ka
    m[1] = Eigen::Vector4f(0.4, 0.1, 0.9, 1.0); // Kd
    unsigned char* test = new unsigned char[ cancellous_bones->sizes().size_enlarged ];
    memset(test, 0, cancellous_bones->sizes().size_enlarged * sizeof(unsigned char));

    for (std::list<Trabecula::Node*>::const_iterator node_it = cancellous_bones->nodes().begin(); node_it != cancellous_bones->nodes().end(); ++node_it)
    {
        for (std::list<int>::const_iterator it = (*node_it)->positions().begin(); it != (*node_it)->positions().end(); ++it)
        {
            test[*it] = 1;
        }

    }
    scene->addObject(test, cancellous_bones->sizes(), m);




    delete cancellous_bones;
    delete [] test;

    RenderingWidget simple_gl;
    simple_gl.setScene(scene);

    simple_gl.resize(1280,800);
    simple_gl.show();
    return app.exec();

}

