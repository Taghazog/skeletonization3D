#ifndef IMAGE3D_HPP
#define IMAGE3D_HPP

#include <vector>
#include <Eigen/Geometry>
#include "cubesMesh.hpp"
#include "shader.hpp"
#include "camera.hpp"

/** \class Image3D
  * A class to represent a planetoid
  */
class Image3D
{

public:
    /** Default constructor */
    Image3D();

    /** A constructor loading a triangular Image3D from the file \a filename */
    Image3D( const Eigen::Vector3f& pos, const Eigen::Vector4f *m);

    /** Destructor */
    ~Image3D();


    // Get planetoid Position
    const Eigen::Vector3f& position() const;

    // Get planetoid Ambient Color
    const Eigen::Vector4f& ka() const;

    // Get planetoid Diffuse Color
    const Eigen::Vector4f& kd() const;

    void attachMesh(const CubesMesh* mesh);    

    void attachShader(const Shader* shader);

    const Shader* shader() const;   
 
    /** Send the Image3D to OpenGL for drawing
      * \param prg_id id of the GLSL program used to draw the geometry */
    void draw(const Camera &camera);
protected:

    Eigen::Vector3f mPosition;
    const CubesMesh* mShape;
    const Shader* mShader;
    Eigen::Matrix4f mTransform; 

    Eigen::Vector4f mMaterial[2];
    

};

#endif //IMAGE3D_HPP
