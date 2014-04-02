#include "image3D.hpp"
#include "openGL.hpp"

    /** Default constructor */
    Image3D::Image3D():   mPosition(0.0, 0.0, 0.0),                    
                    mShape(0),
                    mShader(0),
                    mTransform(Eigen::Matrix4f::Identity())

    {        
        mMaterial[0] = Eigen::Vector4f(0.1, 0.1, 0.0, 1.0); // Ka
        mMaterial[1] = Eigen::Vector4f(1.0, 1.0, 1.0, 1.0); // Kd        
    }

    /** A constructor of a Planetoid with its material */
    Image3D::Image3D( const Eigen::Vector3f& pos
                    , const Eigen::Vector4f *m) :   mPosition(pos),                                    
                                                    mShape(0),
                                                    mShader(0),
                                                    mTransform(Eigen::Matrix4f::Identity())
    {        
        mMaterial[0] = m[0];
        mMaterial[1] = m[1]; 
    }

    /** Destructor */
    Image3D::~Image3D()
    {

    }

    // Get planetoid Position
    const Eigen::Vector3f& Image3D::position() const 
    {
        return mPosition;
    }

    // Get planetoid Ambient Color
    const Eigen::Vector4f& Image3D::ka() const
    {
        return mMaterial[0];
    }

    // Get planetoid Diffuse Color
    const Eigen::Vector4f& Image3D::kd() const
    {
        return mMaterial[1];
    }

    void Image3D::attachMesh(const CubesMesh* mesh)
    {
      mShape = mesh;
    }

    void Image3D::attachShader(const Shader* shader)
    {
      mShader = shader;
    }


    const Shader* Image3D::shader() const
    {
      return mShader;
    }

    /** Send the Image3D to OpenGL for drawing
      * \param prg_id id of the GLSL program used to draw the geometry */
    void Image3D::draw(const Camera &camera)
    {
        mShader->activate();

        int viewMatrixLoc = mShader->getUniformLocation("view_matrix");
        Eigen::Matrix4f objectViewMatrix = camera.viewMatrix() * mTransform;
        if(viewMatrixLoc >= 0)
        {
            glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, objectViewMatrix.data());        
        }

        int view_distloc = mShader->getUniformLocation("cam_dist");
        Eigen::Vector3f vec(154.0/2.0, 154.0/2.0, 154.0/2.0);
        float cam_dist = (camera.getPosition() - vec).norm();
        if(view_distloc >= 0)
        {
            glUniform1f(view_distloc, cam_dist);        
        }
        
        int normalMatrixLoc = mShader->getUniformLocation("normal_matrix");
        if(normalMatrixLoc >= 0) 
        {
            Eigen::Matrix3f normalMatrix = objectViewMatrix.block<3, 3>(0, 0).partialPivLu().inverse().transpose();
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix.data());
        }        

        int projectionMatrixLoc = mShader->getUniformLocation("projection_matrix");
        if(projectionMatrixLoc >= 0)
        {
            glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, camera.projectionMatrix().data());
        } 
        
        glUniform4fv(mShader->getUniformLocation("Ka"), 1, mMaterial[0].data());  
        glUniform4fv(mShader->getUniformLocation("Kd"), 1, mMaterial[1].data());   
        glUniform4fv(mShader->getUniformLocation("Ld"), 1, Eigen::Vector4f(1.0, 1.0, 1.0, 1.0).data());  
        
        mShape->drawGeometry(mShader->id());
    }
