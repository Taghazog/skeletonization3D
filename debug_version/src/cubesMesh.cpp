// Copyright (C) 2008-2011 Gael Guennebaud <gael.guennebaud@inria.fr>


#include "cubesMesh.hpp"
#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <limits>

#include <QCoreApplication>
#include <Eigen/Geometry>

using namespace Eigen;

CubesMesh::CubesMesh()
    : mIsInitialized(false)
{
}

CubesMesh::CubesMesh(const unsigned char* data, const Trabecula::Sizes& sizes)
    : mIsInitialized(false)
{
        load(data, sizes);
}

void CubesMesh::load(const unsigned char* data, const Trabecula::Sizes& sizes)
{
int tg = 0;
    // range les vertices dans l'ordre des faces
    int ind[4];
    int height, depth;
    for(int i = 0; i < sizes.size_z_enlarged; ++i)
    {
        depth = i * sizes.size_y_enlarged * sizes.size_x_enlarged;
        ind[1] = i;
        for (int j = 0; j < sizes.size_y_enlarged; ++j)
        {
            height = j * sizes.size_x_enlarged;
            ind[2] = j;
            for (int k = 0; k < sizes.size_x_enlarged; ++k)
            {
                ind[3] = k;
                ind[0] = depth + height + k;
                if(data[ind[0]] != 0 )
                {
                    addCube(data, sizes, ind);
                }
            }
        }
    }

    computeNormals();
}

void CubesMesh::addFace(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4)
{

    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);

    mVertices.push_back(v3);
    mVertices.push_back(v4);
    mVertices.push_back(v1);
}

void CubesMesh::addCube(const unsigned char* data, const Trabecula::Sizes& sizes, int* ind)
{
    Vector3 v[8];
    int vertical = sizes.size_x_enlarged;
    int depth = sizes.size_y_enlarged * sizes.size_x_enlarged;

    v[0].x() = ind[3];        // WUN
    v[0].y() = ind[2] + 1;
    v[0].z() = ind[1] - 1;

    v[1].x() = ind[3] + 1;    // EUN
    v[1].y() = ind[2] + 1;
    v[1].z() = ind[1] - 1;

    v[2].x() = ind[3];        // WUS
    v[2].y() = ind[2] + 1;
    v[2].z() = ind[1];

    v[3].x() = ind[3] + 1;    // EUS
    v[3].y() = ind[2] + 1;
    v[3].z() = ind[1];

    v[4].x() = ind[3];        // WDN
    v[4].y() = ind[2];
    v[4].z() = ind[1] - 1;

    v[5].x() = ind[3] + 1;    // EDN
    v[5].y() = ind[2];
    v[5].z() = ind[1] - 1;

    v[6].x() = ind[3];        // WDS
    v[6].y() = ind[2];
    v[6].z() = ind[1];

    v[7].x() = ind[3] + 1;    // EDS
    v[7].y() = ind[2];
    v[7].z() = ind[1];


    //Up
    if(ind[2] == sizes.size_y_enlarged-1)
    {
        addFace(v[0], v[2], v[3], v[1]);
    }
    else if(is_borderPoint(data, vertical, ind[0]))
    {
        addFace(v[0], v[2], v[3], v[1]);
    }

    //Down
    if(ind[2] == 0)
    {
        addFace(v[6], v[4], v[5], v[7]);
    }
    else if(is_borderPoint(data, -vertical, ind[0]))
    {
        addFace(v[6], v[4], v[5], v[7]);
    }

    //North
    if(ind[1] == 0)
    {
        addFace(v[4], v[0], v[1], v[5]);
    }
    else if(is_borderPoint(data, -depth, ind[0]))
    {
        addFace(v[4], v[0], v[1], v[5]);
    }

    //South
    if(ind[1] == sizes.size_z_enlarged-1)
    {
        addFace(v[2], v[6], v[7], v[3]);
    }
    else if(is_borderPoint(data, depth, ind[0]))
    {
        addFace(v[2], v[6], v[7], v[3]);
    }

    //East
    if(ind[3] == sizes.size_x_enlarged-1)
    {
        addFace(v[3], v[7], v[5], v[1]);
    }
    else if(is_borderPoint(data, 1, ind[0]))
    {
        addFace(v[3], v[7], v[5], v[1]);
    }

    //West
    if(ind[3] == 0)
    {
        addFace(v[0], v[4], v[6], v[2]);
    }
    else if(is_borderPoint(data, -1, ind[0]))
    {
        addFace(v[0], v[4], v[6], v[2]);
    }
}

bool CubesMesh::is_borderPoint(const unsigned char* data, int direction, int p)
{
    return !data[p + direction];
}

CubesMesh::~CubesMesh()
{
    deleteBuffers();
}

void CubesMesh::computeNormals()
{
    // pass 1: set the normal to 0
    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!= mVertices.end() ; ++v_iter)
    {
        v_iter->normal.setZero();
    }

    // pass 2: compute face normals and accumulate
    Vector3f v0, v1, v2, n;

    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!= mVertices.end() ; v_iter += 6)
    {
        v0 = (*v_iter).position;
        v1 = (*(v_iter+1)).position;
        v2 = (*(v_iter+2)).position;

        n = (v1-v0).cross(v2-v0);//.normalized();

        (*v_iter).normal = n;
        (*(v_iter+1)).normal = n;
        (*(v_iter+2)).normal = n;
        (*(v_iter+3)).normal = n;
        (*(v_iter+4)).normal = n;
        (*(v_iter+5)).normal = n;
    }

    // pass 3: normalize
    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    {
        v_iter->normal.normalize();
    }
}

void CubesMesh::makeUnitary()
{
    // computes the lowest and highest coordinates of the axis aligned bounding box,
    // which are equal to the lowest and highest coordinates of the vertex positions.
    Eigen::Vector3f lowest, highest;
    lowest.fill(std::numeric_limits<float>::max());   // "fill" sets all the coefficients of the vector to the same value
    highest.fill(-std::numeric_limits<float>::max());

    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    {
        // - v_iter is an iterator over the elements of mVertices,
        //   an iterator behaves likes a pointer, it has to be dereferenced (*v_iter, or v_iter->) to access the referenced element.
        // - Here the .aray().min(_) and .array().max(_) operators work per component.
        //
        lowest  = lowest.array().min(v_iter->position.array());
        highest = highest.array().max(v_iter->position.array());
    }

    // TODO: appliquer une transformation à tous les sommets de mVertices de telle sorte
    // que la boite englobante de l'objet soit centrée en (0,0,0)  et que sa plus grande dimension soit de 1
    Eigen::Vector3f center = (lowest+highest)/2.0;
    float m = (highest-lowest).maxCoeff();
    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    {
        v_iter->position = (v_iter->position - center) / m;
    }
}

void CubesMesh::initBuffers()
{
    GL_TEST_ERR;

    assert(!mIsInitialized);
    mIsInitialized = true;

    glGenVertexArrays(1,&mVertexArrayId);
    glBindVertexArray(mVertexArrayId);

    glGenBuffers(1,&mVertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices[0].position.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void CubesMesh::deleteBuffers()
{
    GL_TEST_ERR;

    assert(mIsInitialized);
    mIsInitialized = false;

    glDeleteBuffers(1,&mVertexBufferId);
    glDeleteVertexArrays(1, &mVertexArrayId);

}

void CubesMesh::drawGeometry(int prg_id) const
{
    // bind the vertex array
    glBindVertexArray(mVertexArrayId);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);

    int vertex_loc = glGetAttribLocation(prg_id, "vertexPosition");
    int normal_loc = glGetAttribLocation(prg_id, "vtx_normal");

    // specify the vertex data
    if(vertex_loc>=0)
    {
        glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(vertex_loc);
    }
    else
        glDisableVertexAttribArray(vertex_loc);

    if(normal_loc>=0)
    {
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)(sizeof(Vector3f)));
        glEnableVertexAttribArray(normal_loc);
    }
    else
        glDisableVertexAttribArray(normal_loc);

    // send the geometry
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

    if(vertex_loc>=0)
    {
        glDisableVertexAttribArray(vertex_loc);
    }


    if(normal_loc>=0)
    {
        glDisableVertexAttribArray(normal_loc);
    }

    // release the vertex array
    glBindVertexArray(0);
}


