// Copyright (C) 2008-2011 Gael Guennebaud <gael.guennebaud@inria.fr>

#ifndef _CubesMesh_h_
#define _CubesMesh_h_

#include <vector>
#include <string>
#include <Eigen/Core>

#include "trabecula/tubular_object.hpp"

/** \class CubesMesh
  * A class to represent a 3D triangular CubesMesh
  */
class CubesMesh
{
public:

    /** Default constructor */
    CubesMesh();

    /** A constructor loading a triangular CubesMesh */
    CubesMesh(const unsigned char* data, const Trabecula::Sizes& sizes);

    /** Destructor */
    virtual ~CubesMesh();

    /** creates a CubesMesh from the voxels data */
    void load(const unsigned char* data, const Trabecula::Sizes& sizes);

    void addCube(const unsigned char* data, const Trabecula::Sizes& sizes, int* ind);

    bool is_borderPoint(const unsigned char* data, int direction, int p);

    void addFace(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const Eigen::Vector3f& v3, const Eigen::Vector3f& v4);

    void computeNormals();

    void initBuffers();

    void deleteBuffers();

    /** Send the CubesMesh to OpenGL for drawing
      * \param prg_id id of the GLSL program used to draw the geometry */
    virtual void drawGeometry(int prg_id) const;

    void makeUnitary();

protected:

    typedef Eigen::Vector3f Vector3;

    /** Represents a vertex of the CubesMesh */
    struct Vertex
    {
      Vertex()
        : position(Eigen::Vector3f::Zero()), normal(Eigen::Vector3f::Zero())
      {}
      Vertex(const Eigen::Vector3f& pos)
        : position(pos), normal(Eigen::Vector3f::Zero())
      {}
      Eigen::Vector3f position;
      Eigen::Vector3f normal;
    };

    typedef std::vector<Vertex> VertexArray;

    /** The list of vertices */
    VertexArray mVertices;


    unsigned int mVertexBufferId; ///< the id of the BufferObject storing the vertex attributes
    unsigned int mVertexArrayId;  ///< the id of the VertexArray object
    bool mIsInitialized;
};

#endif
