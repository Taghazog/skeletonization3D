/**********************************************************************/
/*  Copyright (c) 2014, Jerome Bouzillard
/*  All rights reserved.
/*
/*  Redistribution and use in source and binary forms, with or without
/*  modification, are permitted as soon as it retains the above copyright
/*  notice.
*********************************************************************/
/**********************************************************************/
/*
/* This file provides implementation of skeletonization of a
/* Tubular Object, a Graph (Nodes, Edges) of the skeleton, and
/*  information related mostly to cancellous bone.
/*  @implements Tubular_object, Node, Edge.
/*
/**********************************************************************/

#include "trabecula/analyze_loader.hpp"
#include "trabecula/tubular_object.hpp"

#include <iostream>
#include <cstring>
#include <fstream>
#include <cmath>
#include <limits>

#include <bitset>

namespace Trabecula
{

/***********************************************  UTILITIES  declaration  ***************************************************/

/* This constant provides all the 26-adjacent neighbours of each of the 26 neighbours of a point */
static const unsigned short S26[171] = {
    1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 18, 19, 20, 21    //    U
  , 2, 3, 5, 6, 7, 8, 10, 11, 14, 15, 16, 18, 19, 22, 23      //     N
  , 4, 5, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 22, 24         // W
  , 4, 5, 6, 8, 9, 11, 13, 14, 16, 17, 19, 21, 23, 25         // E
  , 3, 5, 7, 8, 9, 12, 13, 15, 16, 17, 20, 21, 24, 25         //     S
  , 3, 4, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25      //   D

  , 3, 7, 8, 10, 11, 18, 19                                   //   U N
  , 4, 6, 9, 10, 12, 18, 20                                   // W U
  , 4, 6, 9, 11, 13, 19, 21                                   // E U
  , 3, 7, 8, 12, 13, 20, 21                                   //   U S
  , 5, 6, 7, 14, 15, 18, 22                                   // W   N
  , 5, 6, 8, 14, 16, 19, 23                                   // E   N
  , 5, 7, 9, 15, 17, 20, 24                                   // W   S
  , 5, 8, 9, 16, 17, 21, 25                                   // E   S
  , 3, 10, 11, 15, 16, 22, 23                                 //   D N
  , 4, 10, 12, 14, 17, 22, 24                                 // W D
  , 4, 11, 13, 14, 17, 23, 25                                 // E D
  , 3, 12, 13, 15, 16, 24, 25                                 //   D S

                                                              // W U N
                                                              // E U N
                                                              // W U S
                                                              // E U S
                                                              // W D N
                                                              // E D N
                                                              // W D S
                                                              // E D S
                                        };

/* This constant provides the 6-adjacent neighbours of the 18 neighbours of a point */
static const unsigned short S6_18[48] = {
   6, 7, 8, 9        //    U
 , 6, 10, 11, 14     //     N
 , 7, 10, 12, 15     // W
 , 8, 11, 13, 16     // E
 , 9, 12, 13, 17     //     S
 , 14, 15, 16, 17    //   D
 , 1, 0              //   U N
 , 2, 0              // W U
 , 3, 0              // E U
 , 4, 0              //   U S
 , 2, 1              // W   N
 , 3, 1              // E   N
 , 4, 2              // W   S
 , 3, 4              // E   S
 , 1, 5              //   D N
 , 2, 5              // W D
 , 3, 5              // E D
 , 4, 5              //   D S

                    // W U N
                    // E U N
                    // W U S
                    // E U S
                    // W D N
                    // E D N
                    // W D S
                    // E D S
                                        };


static const unsigned short INDICESS26[27] = {0, 16, 31, 45, 59, 73, 87, 94, 101, 108, 115, 122, 129, 136, 143, 150, 157, 164, 171, 171, 171, 171, 171, 171, 171, 171, 171};
static const unsigned short INDICESS6_18[19] = {0, 4, 8, 12, 16, 20, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48};
static const float BRANCH_THRESHOLD = 5.0;
static const float EDGE_THRESHOLD = 2.1;

// functions mostly related to the skeletonization process, but not only.
static int skeletonize_data(const unsigned char* data, unsigned char* skeleton, const Sizes& sizes);
static int subiter(unsigned char* data, std::list<int>& black_points_set, int direction, const Sizes& sizes);
static bool is_border_point(const unsigned char* data, int direction, int p);
static void collect_26_neighbours( int p, const Sizes& sizes, int np[26] );
static bool is_simple( const int np[26]);
static int connected26(const int np[26], int i, bool *visited);
static bool is_cond_2_satisfied(const int np[26]);
static void connected6_18(const int np[26], int i, bool *visited, std::bitset<6>& adjacent);
static bool is_cond_4_satisfied(const int np[26]);

//functions to border data with zeroes, and get the indice from the original.
static void bordering(const unsigned char* from, unsigned char* with_borders, const Sizes& sizes);
static int untransformed(int indice, const Sizes& sizes);

//functions to build the graph.
static int find_edge(const unsigned char *thinned, const Sizes& sizes, int np[26]);
static void identify_voxels(int ind, const unsigned char *data, const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids);
static void remove_small_branches(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids);
static void refine_nodes(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids);
static bool is_node_refinable(int ind, const Edge* edge, const Sizes& sizes, std::pair<Node*, Edge*>*voxel_ids);
static bool is_branch(const Edge* edge, Node*& node_back, Node*& node_front, const Sizes& sizes, const std::pair<Node*, Edge*>* voxel_ids);
static void fusion_nodes(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids);

/***********************************************  TubularObject  definition  ************************************************/

/* Constructors/Destructors */
Tubular_object::Tubular_object(): mData(0), mSkeleton(0), mDsr(0)
{

}

Tubular_object::~Tubular_object()
{
    delete mDsr;
    delete [] mData;
    delete [] mSkeleton;

    for (std::list<Edge*>::const_iterator it = mEdges.begin(); it != mEdges.end(); ++it)
    {
        delete *it;
    }

    for (std::list<Node*>::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
    {
        delete *it;
    }
}

/* Setters */



/* Getters */
const unsigned char* Tubular_object::data() const
{
    return mData;
}

const unsigned char* Tubular_object::skeleton_data() const
{
    return mSkeleton;
}

const ANALYZE_DSR* Tubular_object::dsr() const
{
    return mDsr;
}

const Sizes& Tubular_object::sizes() const
{
    return mSizes;
}

 /* Member Functions */
int Tubular_object::load_from_file(const std::string& filename)
{
    mDsr = new ANALYZE_DSR;

    unsigned path = filename.find_last_of("/");
    mFilename = filename.substr(path+1);

    std::string headerFilename;
    headerFilename = filename + ".hdr";

    std::string imageFilename;
    imageFilename = filename + ".img";

    if(anaReadHeader(headerFilename.c_str(), mDsr))
    {
        std::cerr << "Image header read failed!" << std::endl;
        return 1;
    }

    mSizes.size_x = mDsr->dime.dim[1];
    mSizes.size_y = mDsr->dime.dim[2];
    mSizes.size_z = mDsr->dime.dim[3];
    mSizes.size_x_enlarged = mSizes.size_x + 2;
    mSizes.size_y_enlarged = mSizes.size_y + 2;
    mSizes.size_z_enlarged = mSizes.size_z + 2;
    mSizes.size = mSizes.size_x * mSizes.size_y * mSizes.size_z;
    mSizes.size_enlarged = mSizes.size_x_enlarged * mSizes.size_y_enlarged * mSizes.size_z_enlarged;
    mSizes.xOy_size = mSizes.size_x * mSizes.size_y;
    mSizes.xOy_enlarged_size = mSizes.size_x_enlarged * mSizes.size_y_enlarged;

    mSkeleton = new unsigned char[ mSizes.size_enlarged ];
    memset(mSkeleton, 0, mSizes.size_enlarged * sizeof(unsigned char));

    mData = new unsigned char[ mSizes.size_enlarged ];
    unsigned char* data_tmp = new unsigned char[mSizes.size];

    if(anaReadImagedata(imageFilename.c_str(), mDsr, 1, (char*)data_tmp))
    {
        std::cerr << "Image data read failed!" << std::endl;
        return 2;
    }
    bordering(data_tmp, mData, mSizes);
    delete [] data_tmp;
}

/********************************************************************
* this function return the volume pourcentage bone/total
*  this function assumes the image dimensions to be the same
*  and the image being a sphere and not a cube.
*********************************************************************/
float Tubular_object::bv_tv() const
{
    static const float pi = 3.14159265;
    float total = 1.0/6.0 * pi * mSizes.size; // 4/3 * Pi * R^3
    float nb_object_voxels = 0.0;

    for (int i = 0; i < mSizes.size_enlarged; ++i)
    {
        if(mData[i])
        {
            nb_object_voxels += 1.0;
        }
    }
    return nb_object_voxels/total * 100.0;
}

/*******************************************************************************
* this function computes the average trabecular length, min, max, and
* deviation.
********************************************************************************/
void Tubular_object::average_trabecular_length(float values[4])
{
    values[0] = 0.0;
    float min = (float) std::numeric_limits<int>::max();
    float max = 0.0;
    float variance;
    float tmp;

    for (std::list<Edge*>::const_iterator it = mEdges.begin(); it != mEdges.end(); ++it)
    {
       values[0] += (*it)->length();
       if((*it)->length() > max)
       {
            // max
            max = (*it)->length();
       }

       if((*it)->length() < min && (*it)->length() > 2.0)
       {
            // min
            min = (*it)->length();
       }
    }

    values[1] = min * mDsr->dime.pixdim[1];
    values[2] = max * mDsr->dime.pixdim[1];

    // standard Mean
    values[0] = values[0]/mEdges.size() * mDsr->dime.pixdim[1];

    for (std::list<Edge*>::const_iterator it = mEdges.begin(); it != mEdges.end(); ++it)
    {
        tmp = std::abs(values[0] - (*it)->length());
        tmp = tmp * tmp;
        variance += tmp;
    }

    // standard Deviation
    values[3] = std::sqrt(variance/mEdges.size()) * mDsr->dime.pixdim[1];
}

/*******************************************************************************
* this function provides the number of trabeculae in the object
********************************************************************************/
int Tubular_object::number_of_trabeculae()
{
    return mEdges.size();
}

/*******************************************************************************
* this function provides the connectivity of the nodes (trabeculae junctions)
********************************************************************************/
void Tubular_object::nodes_connectivity(std::vector<int>& con)
{
    int connectivity[26] = {};

    for (std::list<Node*>::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
    {
        ++connectivity[(*it)->connectivity()];
    }

    for (int i = 0; i < 26; ++i)
    {
        if(connectivity[i] != 0)
        {
            con.push_back(i);
            con.push_back(connectivity[i]);
        }
    }
}

/*******************************************************************************
* this function computes the average trabecular spacing
********************************************************************************/
void Tubular_object::tb_sp()
{
    //TODO
}

/*******************************************************************************
* this function provides information about the shape of trabeculae.
* it classifies rod-like and plate-like trabeculae.
********************************************************************************/
void Tubular_object::tb_shape()
{
    //TODO
}

/******************************************************************************************
* Skeletonize : this function compute a skeleton of tubular object and store
* its result in the skeleton data structure.
******************************************************************************************/
int Tubular_object::skeletonize()
{
    return skeletonize_data(mData, mSkeleton, mSizes);
}

/******************************************************************************************
* Graph : this function Extract Edges and Nodes (Junctions and Trabeculae) from the skeleton
* in 3 passes. (edges and nodes are provided into simple lists with adjacencies)
******************************************************************************************/
int Tubular_object::build_graph()
{
    if(!mSkeleton)
    {
        std::cerr << "error, no skeleton!" << std::endl;
        return 1;
    }

    /*  Create a copy of data with binary values and zero borders */
    unsigned char *data_tmp = new unsigned char[mSizes.size_enlarged];
    for (int i = 0; i < mSizes.size_enlarged; ++i)
    {
        data_tmp[i] = mSkeleton[i];
    }

    /*  Create a marker pair array to mark every voxel with edge or node status */
    std::pair<Node*, Edge*>* voxel_ids = new std::pair<Node*, Edge*>[mSizes.size_enlarged];
    memset(voxel_ids, 0, mSizes.size_enlarged * sizeof(std::pair<Node*, Edge*>));

    /*  Find the indice of a starting edge */
    int np[26];
    int ind = find_edge(data_tmp, mSizes, np);

    if(ind == mSizes.size_enlarged)
    {
        std::cerr << "couldnt build graph, skeleton is empty or no nodes in it!" << std::endl;
        return 2;
    }

    /** FIRST PASS: Remove the noisy branches on the skeleton. **/

    /* Compute a depth-first search to create the nodes and edges */
    identify_voxels(ind, data_tmp, mSizes, voxel_ids);

    /* Refine the nodes to their minimum of voxels             */
    refine_nodes(mSizes, voxel_ids);

    /* Delete any of the branches which are smaller than a threshold
        (noise from skeletonization, or segmentation)                       */
    remove_small_branches(mSizes, voxel_ids);

    for (int i = 0; i < mSizes.size_enlarged; ++i)
    {
        if(voxel_ids[i].second || voxel_ids[i].first )
        {
            mSkeleton[i] = 1;
        }
        else
        {
            mSkeleton[i] = 0;
        }
    }
int nb = 0;

    // reskeletonize after deleting noisy branches to prepare the second pass.
    skeletonize_data(mSkeleton, mSkeleton, mSizes);

    // Free the memory allocated by nodes and edges before Second pass
    Node* node_tmp;
    Edge* edge_tmp;
    for (int i = 0; i < mSizes.size_enlarged; ++i)
    {
        if(voxel_ids[i].first)
        {
            node_tmp = voxel_ids[i].first;
            for (std::list<int>::const_iterator it = node_tmp->positions().begin(); it != node_tmp->positions().end(); ++it)
            {

                voxel_ids[*it].first = 0;
            }
            delete node_tmp;
        }
        else if(voxel_ids[i].second)
        {
            edge_tmp = voxel_ids[i].second;
            for (std::deque<int>::const_iterator it = edge_tmp->data().begin(); it != edge_tmp->data().end(); ++it)
            {
                voxel_ids[*it].second = 0;
            }
            delete edge_tmp;
        }

        data_tmp[i] = mSkeleton[i];
    }

    /** SECOND PASS: Fusion the nodes that are connected each other by a too small edge **/
    ind = find_edge(data_tmp, mSizes, np);

    /* Compute a depth-first search to create the nodes and edges */
    identify_voxels(ind, data_tmp, mSizes, voxel_ids);

    /* Refine the nodes to their minimum of voxels             */
    refine_nodes(mSizes, voxel_ids);

    /* fusion the nodes that are too close (separated by an edge smaller than EDGE_THRESHOLD) */
    fusion_nodes(mSizes, voxel_ids);

    /** FINAL PASS, list of Edges and Nodes and their adjacencies. **/
    bool* visited_tmp = new bool[mSizes.size_enlarged];
    memset(visited_tmp, 0, mSizes.size_enlarged * sizeof(bool));

    // for each edges, stores the connected nodes, stores the edge to the connected nodes
    // and fill the list of edges and nodes not yet visited to the tubular object.
    for (int i = 0; i < mSizes.size_enlarged; ++i)
    {
        if(voxel_ids[i].second)
        {
            edge_tmp = voxel_ids[i].second;
            if(!visited_tmp[edge_tmp->data().back()])
            {
                visited_tmp[edge_tmp->data().back()] = true;
                mEdges.push_back(edge_tmp);

                collect_26_neighbours(edge_tmp->data().front(), mSizes, np);
                for (int j = 0; j < 26; ++j)
                {
                    if (voxel_ids[np[j]].first)
                    {
                        node_tmp = voxel_ids[np[j]].first;
                        node_tmp->add_edge(edge_tmp);

                        if (!visited_tmp[*(node_tmp->positions().begin())])
                        {
                            mNodes.push_back(node_tmp);
                            visited_tmp[*(node_tmp->positions().begin())] = true;
                        }

                        break;
                    }
                }

                collect_26_neighbours(edge_tmp->data().back(), mSizes, np);
                for (int j = 0; j < 26; ++j)
                {
                    if (voxel_ids[np[j]].first && node_tmp != voxel_ids[np[j]].first)
                    {
                        node_tmp = voxel_ids[np[j]].first;
                        node_tmp->add_edge(edge_tmp);

                        if (!visited_tmp[*(node_tmp->positions().begin())])
                        {
                            mNodes.push_back(node_tmp);
                            visited_tmp[*(node_tmp->positions().begin())] = true;
                        }

                        break;
                    }
                }
            }
        }
    }

    delete [] visited_tmp;
    delete [] voxel_ids;
    delete [] data_tmp;

    return 0;
}

/******************************************************************************************
* Dump Infos : this function Computes all the measures needed for the cancellous bone
* and write the results into a file in your build directory.
******************************************************************************************/
int Tubular_object::dump_infos()
{
    std::string filename = mFilename + "_infos.txt";

    std::ofstream myfile;
    myfile.open (filename.c_str());
    myfile << " *********************************************************************************\n";
    myfile << " ******** This file provides structural information about Cancellous Bone ********\n";
    myfile << " *********************************************************************************\n\n";

    myfile << "Name of input image data file: " << mFilename << std::endl;
    myfile << "Image Dimensions: " << mSizes.size_x << " " << mSizes.size_y << " " << mSizes.size_y << std::endl;
    myfile << "Voxel width: " << mDsr->dime.pixdim[1] << "mm (should be isotropic in x, y and z directions)\n";

    myfile << "Number of Trabeculae: " << number_of_trabeculae() << std::endl;

    myfile << "BV/TV: " << ((int) floor(bv_tv() * 100 + 0.5))/100.0 << " \%" << std::endl;

    float values[4];
    average_trabecular_length(values);
    myfile << "Average Trabecular Length: " << ((int) floor(values[0] * 100 + 0.5))/100.0 << "mm" << std::endl;
    myfile << "Minimum Trabecular Length: " << ((int) floor(values[1] * 100 + 0.5))/100.0 << "mm" << std::endl;
    myfile << "Maximum Trabecular Length: " << ((int) floor(values[2] * 100 + 0.5))/100.0 << "mm" << std::endl;
    myfile << "Standard Deviation of Trabecular Length: " << ((int) floor(values[3] * 100 + 0.5))/100.0 << "mm" << std::endl;

    std::vector<int> connectivities;
    nodes_connectivity(connectivities);

    myfile << "Junction Histogram: (Junction Connectivity - Number of Junctions)" << std::endl;

    for (int i = 0; i < connectivities.size(); i += 2)
    {
        myfile << connectivities[i] << " - " << connectivities[i+1] << std::endl;
    }

    //tb_th();
    //tb_sp();
    //tb_shape();

    myfile.close();
}

/******************************************************************************************
* Save Skeleton : this function saves the skeleton into a new Analyze file.
******************************************************************************************/
int Tubular_object::save_skeleton()
{
    std::string filename;
    filename = mFilename + "_skeleton.img";
    if(anaWriteImagedata(filename.c_str(), mDsr, (char*)mSkeleton))
    {
        return 1;
    }

    filename = mFilename + "_skeleton.hdr";
    if(anaWriteHeader(filename.c_str(), mDsr))
    {
        return 1;
    }
}

/***********************************************  Node  definition  *********************************************************/

/* Constructors/Destructors */
Node::Node() : mConnectivity(0)
{
}

Node::~Node()
{
}

/* Setters */
void Node::set_connectivity(int nb_edges)
{
    mConnectivity = nb_edges;
}

/* Getters */
int Node::connectivity() const
{
    return mConnectivity;
}

const std::list<Edge*>& Node::edges() const
{
    return mEdges;
}

const std::list<int>& Node::positions() const
{
    return mPositions;
}

/* Member Functions */
void Node::add_edge(Edge* edge)
{
    edge->set_nodes(this);
    mEdges.push_back(edge);
}

void Node::add_voxel(int indice)
{
    mPositions.push_back(indice);
}

void Node::remove_voxel(int indice)
{
    mPositions.remove(indice);
}

/***********************************************  Edge  definition  *********************************************************/

/* Constructors/Destructors */

Edge::Edge() : mLength(0.0), mFirst(0), mSecond(0)
{
}

Edge::~Edge()
{
}

/* Setters */
void Edge::set_nodes(Node* node)
{
    if(!mFirst)
    {
        mFirst = node;
    }
    else
    {
        mSecond = node;
    }
}

/* Getters */
float Edge::length() const
{
    return mLength;
}

const Node* Edge::first() const
{
    return mFirst;
}

const Node* Edge::second() const
{
    return mSecond;
}

const std::deque<int>& Edge::data() const
{
    return mIndices;
}

/* Member Functions */
/**************************************************************************
*   This function add a voxel to an existing edge, and update its length
*   depending on adjacency. The function assumes the image 3D to be isotropic.
**************************************************************************/
void Edge::add_voxel(int ind, int adjacency, bool back)
{
    if(adjacency < 6)
    {
        mLength += 1.0; // 1 for 6-adjacency
    }
    else if(adjacency < 18)
    {
        mLength += 1.41421356; // sqrt(2) for 18-adjacency
    }
    else
    {
        mLength += 1.73205080; // sqrt(3) for 26-adjacency
    }

    if(back)
    {
        mIndices.push_back(ind);
    }
    else
    {
        mIndices.push_front(ind);
    }
}

/***********************************************  UTILITIES  definition  ****************************************************/
/******************************************************************************************
* Skeletonize_data : this function computes a skeleton of 3D image data and store
* its result in the skeleton data structure.
* Implementation of : A sequential 3D thinning algorithm and its medical applications (2001)
******************************************************************************************/
static int skeletonize_data(const unsigned char* data, unsigned char* skeleton, const Sizes& sizes)
{
    if(!data)
    {
        std::cerr << "error, no data to skeletonize!" << std::endl;
        return 1;
    }

    /*  copy the Black points set indices into an array */
    /*  and create a copy of data with binary values and zero borders */
    unsigned char *data_tmp = new unsigned char[sizes.size_enlarged];
    for (int i = 0; i < sizes.size_enlarged; ++i)
    {
        data_tmp[i] = data[i];
    }

    memset(skeleton, 0, sizes.size_enlarged * sizeof(unsigned char));
    std::list<int> black_points_set;

    for(int i = 0; i < sizes.size_enlarged; ++i)
    {
        if(data_tmp[i])
        {
            black_points_set.push_back(i);
        }
    }

    int modified;

    /* Compute the 6 subiterations until no points are deleted */
    do
    {
        modified = 0;
        modified += subiter(data_tmp, black_points_set, -sizes.size_x_enlarged, sizes);       // Up
        modified += subiter(data_tmp, black_points_set, sizes.size_x_enlarged, sizes);        // Down
        modified += subiter(data_tmp, black_points_set, sizes.xOy_enlarged_size, sizes);      // North
        modified += subiter(data_tmp, black_points_set, -sizes.xOy_enlarged_size, sizes);     // South
        modified += subiter(data_tmp, black_points_set, 1, sizes);                             // East
        modified += subiter(data_tmp, black_points_set, -1, sizes);                            // West

    } while(modified > 0);

    /* copy the result into the skeleton data */
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        skeleton[*p] = 1;
    }

    delete [] data_tmp;

    return 0;
}
/*******************************************************************************
*   subiter : Return the number of deleted points in the subiteration from
*             a particular direction (6 subiterations to do).
*   @params : The image data, the black points set,
*             the direction, image dimensions sizes
*******************************************************************************/
static int subiter(unsigned char* data, std::list<int>& black_points_set, int direction, const Sizes& sizes)
{
    int modified = 0;
    int np[26];
    int nb = 0;

    /* list of simple and non end points, pointers from black_points_set */
    std::list<std::list<int>::iterator> list;

    // fill the list in a first check loop.
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        if( is_border_point( data, direction, *p) )
        {
            collect_26_neighbours(*p, sizes, np);
            nb = 0;
            for (int i = 0; i < 26; ++i)
            {
                if(data[np[i]] != 0)
                {
                    ++nb;
                    np[i] = 1;
                }
                else
                {
                    np[i] = 0;
                }
            }

            if( nb > 1 )
            {
                if( is_simple(np) )
                {
                    list.push_back(p);
                }
            }
        }
    }

    int value = -1;

    // remove each point of the list if they remain simple and non endpoint.
    while( value != modified )
    {
        value = modified;
        for(std::list<std::list<int>::iterator>::iterator p = list.begin(); p != list.end();)
        {
            collect_26_neighbours(**p, sizes, np);
            nb = 0;
            for (int i = 0; i < 26; ++i)
            {
                if(data[np[i]] != 0)
                {
                    ++nb;
                    np[i] = 1;
                }
                else
                {
                    np[i] = 0;
                }
            }

            if( nb > 1 )
            {
                if( is_simple(np) )
                {
                    data[**p] = 0;
                    black_points_set.erase(*p);
                    p = list.erase(p);
                    ++modified;
                }
                else ++p;
            }
            else ++p;
        }
    }

    return modified;
}

/*******************************************************************************
*   is_borderPoint : Return true if the point p is a border Point from one
*                    direction.
*   @params : The image data, the direction, the point p
*******************************************************************************/
static bool is_border_point(const unsigned char* data, int direction, int p)
{
    return !data[p + direction];
}


/*******************************************************************************
*   collect_26_neighbours : save the neighbour indices in the data np.
*   @params : the point p, image dimensions sizes, 26 neighbours
*******************************************************************************/
static void collect_26_neighbours( int p, const Sizes& sizes, int np[26] )
{
    /*  west : p - 1
        east : p + 1
        north : p + width*height
        south : p - width*height
        up : p - width
        down : p + width
    */
    int vertical = sizes.size_x_enlarged;
    int depth = sizes.xOy_enlarged_size;

    int nb = 0;

    /* 6-adjacent */
    np[0] = p - vertical;                //    U
    np[1] = p + depth;                   //     N
    np[2] = p - 1;                       // W
    np[3] = p + 1;                       // E
    np[4] = p - depth;                   //     S
    np[5] = p + vertical;                //   D

    /* 18-adjacent */
    np[6] = p - vertical + depth;        //   U N
    np[7] = p - 1 - vertical;            // W U
    np[8] = p + 1 - vertical;            // E U
    np[9] = p - vertical - depth;        //   U S
    np[10] = p - 1  + depth;             // W   N
    np[11] = p + 1 + depth;              // E   N
    np[12] = p - 1 - depth;              // W   S
    np[13] = p + 1 - depth;              // E   S
    np[14] = p + vertical + depth;       //   D N
    np[15] = p - 1 + vertical;           // W D
    np[16] = p + 1 + vertical;           // E D
    np[17] = p + vertical - depth;       //   D S

    /* 26-adjacent */
    np[18] = p - 1 - vertical + depth;   // W U N
    np[19] = p + 1 - vertical + depth;   // E U N
    np[20] = p - 1 - vertical - depth;   // W U S
    np[21] = p + 1 - vertical - depth;   // E U S
    np[22] = p - 1 + vertical + depth;   // W D N
    np[23] = p + 1 + vertical + depth;   // E D N
    np[24] = p - 1 + vertical - depth;   // W D S
    np[25] = p + 1 + vertical - depth;   // E D S
}

/*******************************************************************************
*   is_simple : return true if the point is simple, i.e does not alter the
*               topology of the picture :
*   1. the set N26(p)∩(B\{p}) is not empty (i.e., p is not an isolated point);
*   2. the set N26(p)∩(B\{p}) is 26–connected (in itself );
*   3. the set (ZZ^3\B)∩ N6(p) is not empty (i.e., p is a border point); and
*   4. the set (ZZ^3\B)∩ N6(p) is 6–connected in the set (ZZ^3\B)∩ N18(p)
*   @params : 26 neighbour values of p
*******************************************************************************/
static bool is_simple(const int np[26])
{
    if( is_cond_2_satisfied(np) )
    {
        if( is_cond_4_satisfied(np ) )
        {
            return true;
        }
    }
    return false;
}

/********************************************************************************
* This condition check if the black points in the neighbourhood
* of point p are 26-adjacent, to prevent from deleting p which would
* change topology of B set. (the key of the erosion thinning process).
* p must be a non end-point to enter this function
*********************************************************************************/
static bool is_cond_2_satisfied(const int np[26])
{
    bool visited[26] = {false};
    int i = 0;
    while(!np[i])
    {
        visited[i] = true;
        ++i;
    }

    int res = connected26(np, i, visited);

    int nb = 0;
    for(int i = 0; i < 26; ++i)
    {
        if(np[i] != 0)
        {
            ++nb;
        }
    }

    if(res != nb)
    {
        return false;
    }

    return true;
}

/********************************************************************************
* This is a recursive fonction adding neighbors 26-adjacent in itself
*********************************************************************************/
static int connected26(const int np[26], int i, bool *visited)
{
    unsigned short nb = 1;
    visited[i] = true;
    unsigned short ind;
    for(int j = INDICESS26[i]; j < INDICESS26[i+1]; ++j)
    {
        ind = S26[j];
        if(!visited[ind] && np[ind])
        {
            nb += connected26(np, ind, visited);
        }
    }

    return nb;
}

/********************************************************************************
* This condition check if the white points in the 6 neighbourhood
* of point p are 6-adjacent in 18-adjacency to keep the topology
* of B set. (the key of the erosion thinning process)
*********************************************************************************/
static bool is_cond_4_satisfied(const int np[26])
{
    bool visited[18] = {false};
    std::bitset<6> adjacent(0b000000);
    int i = 0;
    while(np[i])
    {
        visited[i] = true;
        ++i;
    }

    adjacent[i] = 1;
    connected6_18(np, i, visited, adjacent);

    if(adjacent.count() != 6 - (np[0] + np[1] + np[2] + np[3] + np[4] + np[5]))
    {
        return false;
    }

    return true;
}

/********************************************************************************
* This is a recursive fonction adding 6-adjacent neighbors 6-adjacent in 18 set
********************************************************************************/
static void connected6_18(const int np[26], int i, bool *visited, std::bitset<6>& adjacent)
{
    visited[i] = true;
    unsigned short ind;
    for(int j = INDICESS6_18[i]; j < INDICESS6_18[i+1]; ++j)
    {
        ind = S6_18[j];
        if(!visited[ind] && !np[ind])
        {
            if(ind >= 0 && ind < 6)
            {
                adjacent[ind] = 1;
            }
            connected6_18(np, ind, visited, adjacent);
        }
    }
}

/**************************************************************************
*   This function fill the data with zero borders
**************************************************************************/
static void bordering(const unsigned char* from, unsigned char* with_borders, const Sizes& sizes)
{
    memset(with_borders, 0, sizes.size_enlarged * sizeof(unsigned char));

    // xOy read order
    int depth, height, depth_enlarged, height_enlarged, ind;

    for(int i = 0; i < sizes.size_z; ++i)
    {
        depth = i * sizes.xOy_size;
        depth_enlarged = (i+1) * sizes.xOy_enlarged_size;
        for(int j = 0; j < sizes.size_y; ++j)
        {
            height = j * sizes.size_x;
            height_enlarged = (j+1) * sizes.size_x_enlarged;
            for(int k = 0; k < sizes.size_x; ++k)
            {
                if(from[depth + height + k] != 0 )
                {
                    ind = depth_enlarged + height_enlarged + k+1;
                    with_borders[ind] = 1;
                }
            }
        }
    }
}

/**************************************************************************
*   This function transform an indice from an array with zero borders
*   to the original array without the zero borders
**************************************************************************/
int untransformed(int indice, const Sizes& sizes)
{
    int depth, height, ind;

    depth = indice / sizes.xOy_enlarged_size;
    depth = (depth - 1) * sizes.xOy_size;
    height = (indice % sizes.xOy_enlarged_size) / sizes.size_x_enlarged;
    height = (height - 1) * sizes.size_x;
    ind = ( indice % sizes.xOy_enlarged_size ) % sizes.size_x_enlarged;
    ind += depth + height - 1;

    return ind;
}


/**************************************************************************
*   This function finds a starting edge in the skeleton that is not yet
visited to build the graph.
**************************************************************************/
int find_edge(const unsigned char *data, const Sizes& sizes, int np[26])
{
    int nb;
    int i = 0;
    while (i < sizes.size_enlarged )
    {
        if(data[i] != 0)
        {
            collect_26_neighbours(i, sizes, np);
            nb = 0;
            for (int j = 0; j < 26; ++j)
            {
                if(data[np[j]] != 0)
                {
                    ++nb;
                }
            }

            if(nb == 1)
            {
                return i;
            }
        }
        ++i;
    }
    return i;
}

/**************************************************************************
*   This function compute a recursive depth-first search
*   algorithm to identify and create edges and nodes on the skeleton.
**************************************************************************/
static void identify_voxels(int ind, const unsigned char *data, const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids)
{
    // build the edge until the destination node is encountered.
    Edge* edge = new Edge();
    int np[26];
    int indice;
    bool on_edge = true;
    int connectivity;
    int adjacency = 6;

    do
    {
        collect_26_neighbours(ind, sizes, np);
        connectivity = 0;
        for (int i = 0; i < 26; ++i)
        {
            if(data[np[i]] != 0)
            {
                ++connectivity;
            }
        }

        if(connectivity > 2)
        {
            on_edge = false;
        }
        else
        {
            voxel_ids[ind].second = edge;
            edge->add_voxel(ind, adjacency, true);

            int i = 0;
            while (i < 26)
            {
                if(data[np[i]] && !voxel_ids[np[i]].second && !voxel_ids[np[i]].first)
                {
                    ind = np[i];
                    adjacency = i;
                    break;
                }
                ++i;
            }
            if(i == 26)
            {
                ind = 0;
                on_edge = false;
            }
        }
    } while(on_edge);

    // build the node if it is not yet visited and stores its connected edges.
    if(ind)
    {
        Node* node = new Node();

        int np[26];
        int connectivity;
        std::list<int> edges;
        std::deque<int> queue;

        queue.push_back(ind);
        voxel_ids[ind].first = node;
        node->add_voxel(ind);

        while(!queue.empty())
        {
            ind = queue.front();
            queue.pop_front();

            collect_26_neighbours(ind, sizes, np);
            connectivity = 0;
            for (int i = 0; i < 26; ++i)
            {
                if(data[np[i]] != 0)
                {
                    ++connectivity;
                }
            }

            if(connectivity <= 2)
            {
                voxel_ids[ind].first = 0;
                node->remove_voxel(ind);
                edges.push_back(ind);
            }
            else
            {
                for (int i = 0; i < 26; ++i)
                {
                    if(data[np[i]] && !voxel_ids[np[i]].first)
                    {
                        queue.push_back(np[i]);
                        voxel_ids[np[i]].first = node;
                        node->add_voxel(np[i]);
                    }
                }
            }
        }

        node->set_connectivity(edges.size());

        // build edges that starts from the node and are not already built in depth-first search.
        for (std::list<int>::iterator it = edges.begin(); it != edges.end(); ++it)
        {
            if (!voxel_ids[*it].second)
            {
                identify_voxels(*it, data, sizes, voxel_ids);
            }
        }
    }
}

/**************************************************************************
* This function check if the node (indice ind) is deletable to extend the edge
* This deletion is possible only under 2 conditions.
* 1: check that node voxel deletion doesn't disconnect its node voxel neighbours
* 2: check if the edges connected to the node voxel are still connected to the node.
**************************************************************************/
static bool is_node_refinable(int ind, const Edge* edge, const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids)
{
    int np[26];
    std::list<Edge*> edges;
    std::list<int> node_voxels;
int nttt = 0;
    // check the neighbours of the node voxels
    collect_26_neighbours(ind, sizes, np);
    for (int i = 0; i < 26; ++i)
    {
        // temporarily stores the edges connected to that node voxel.
        if (voxel_ids[np[i]].second && voxel_ids[np[i]].second != edge)
        {
            edges.push_back(voxel_ids[np[i]].second);
        }

        // temporarily stores the node voxels 26-adjacent to that node voxel.
        if (voxel_ids[np[i]].first)
        {
            node_voxels.push_back(np[i]);
            np[i] = 1;
        }
        else
        {
            np[i] = 0;
        }
    }

    // if the node is not alone and there are edges connected to this node voxel, check our 2 conditions.
    if(!node_voxels.empty() && !edges.empty())
    {
        // condition 1.
        if (is_cond_2_satisfied(np))
        {
            // condition 2.
            for (std::list<int>::iterator it = node_voxels.begin(); it != node_voxels.end() && !edges.empty(); ++it)
            {
                collect_26_neighbours(*it, sizes, np);
                for (int i = 0; i < 26 && !edges.empty(); ++i)
                {
                    for (std::list<Edge*>::iterator edge_it = edges.begin(); edge_it != edges.end();)
                    {
                        if (voxel_ids[np[i]].second == *edge_it)
                        {
                            edge_it = edges.erase(edge_it);
                        }
                        else ++edge_it;
                    }
                }
            }
        }
    }

    if (edges.empty())
    {
        return true;
    }

    return false;
}

/**************************************************************************
*   This function try to refine the nodes to their minimum
*   of voxels.
**************************************************************************/
static void refine_nodes(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids)
{
    bool* visited_tmp = new bool[sizes.size_enlarged];
    memset(visited_tmp, 0, sizes.size_enlarged * sizeof(bool));
    int np[26];
    int ind;
    int front, back;

    for (int i = 0; i < sizes.size_enlarged; ++i)
    {
        // for each edges non visited yet
        if(voxel_ids[i].second)
        {
            // find its start and end voxel and for both, refine the connected node voxel
            front = voxel_ids[i].second->data().front();
            back = voxel_ids[i].second->data().back();

            if(!visited_tmp[front])
            {
                collect_26_neighbours(front, sizes, np);
                for (int j = 0; j < 26; ++j)
                {
                    if(voxel_ids[np[j]].first)
                    {
                        ind = np[j];
                        // refine the front node if it is possible.
                        if(is_node_refinable(ind, voxel_ids[i].second, sizes, voxel_ids))
                        {
                            voxel_ids[ind].second = voxel_ids[i].second;
                            voxel_ids[ind].second->add_voxel(ind, j, false);
                            voxel_ids[ind].first->remove_voxel(ind);
                            voxel_ids[ind].first = 0;
                            break;
                        }
                    }
                }

                collect_26_neighbours(back, sizes, np);
                for (int j = 0; j < 26; ++j)
                {
                    if(voxel_ids[np[j]].first)
                    {
                        ind = np[j];
                        // refine the back node if it is possible.
                        if(is_node_refinable(ind, voxel_ids[i].second, sizes, voxel_ids))
                        {
                            voxel_ids[ind].second = voxel_ids[i].second;
                            voxel_ids[ind].second->add_voxel(ind, j, true);
                            voxel_ids[ind].first->remove_voxel(ind);
                            voxel_ids[ind].first = 0;
                            break;
                        }
                    }
                }

                front = voxel_ids[i].second->data().front();
                visited_tmp[front] = true;
            }
        }
    }

    delete [] visited_tmp;
}

/**************************************************************************
*   This function removes branches that are smaller than a
*   given threshold.
**************************************************************************/
static void remove_small_branches(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids)
{
    bool* visited_tmp = new bool[sizes.size_enlarged];
    memset(visited_tmp, 0, sizes.size_enlarged * sizeof(bool));
    int np[26];
    Edge* edge;
    Node* node_front;
    Node* node_back;
    int ind, back;

    for (int i = 0; i < sizes.size_enlarged; ++i)
    {
        node_front = 0;
        node_back = 0;

        // for each edges non visited yet
        if(voxel_ids[i].second)
        {
            edge = voxel_ids[i].second;
            back = edge->data().back();

            if (!visited_tmp[back])
            {
                // if the edge is a branch
                if (is_branch(edge, node_back, node_front, sizes, voxel_ids))
                {
                    // if the branch is too small, remove it
                    if (edge->length() < BRANCH_THRESHOLD)
                    {
                        for (int j = 0; j < edge->data().size(); ++j)
                        {
                            ind = edge->data()[j];
                            voxel_ids[ind].second = 0;
                        }

                        delete edge;
                    }
                }

                visited_tmp[back] = true;
            }
        }
    }

    delete [] visited_tmp;
}

/**************************************************************************
*   This function check if the edge is a branch, and update its back
*   and front nodes.
**************************************************************************/
static bool is_branch(const Edge* edge, Node*& node_back, Node*& node_front, const Sizes& sizes, const std::pair<Node*, Edge*>* voxel_ids)
{
    int np[26];
    int edge_junctions = 0;

    /* if there is a node on the back of edge, get it. */
    collect_26_neighbours(edge->data().back(), sizes, np);
    for (int j = 0; j < 26; ++j)
    {
        if (voxel_ids[np[j]].first)
        {
            edge_junctions += 1;
            node_back = voxel_ids[np[j]].first;
            break;
        }
    }

    /* if there is a node at the front of edge, get it. */
    collect_26_neighbours(edge->data().front(), sizes, np);
    for (int j = 0; j < 26; ++j)
    {
        if (voxel_ids[np[j]].first && voxel_ids[np[j]].first != node_back)
        {
            edge_junctions += 1;
            node_front = voxel_ids[np[j]].first;
            break;
        }
    }

    /* only one node makes it a branch, 2 nodes makes it internal edge. */
    if (edge_junctions == 1)
    {
        return true;
    }

    return false;
}

/**************************************************************************
*   This function fusion the nodes connected by an edge smaller than a
*   threshold. (the edge and the back node both become part of the unique
*   front node)
**************************************************************************/
static void fusion_nodes(const Sizes& sizes, std::pair<Node*, Edge*>* voxel_ids)
{
    bool* visited_tmp = new bool[sizes.size_enlarged];
    memset(visited_tmp, 0, sizes.size_enlarged * sizeof(bool));
    std::deque<int> queue;
    int np[26];
    Edge* edge;
    Node* node_front;
    Node* node_back;
    int ind, back;

    for (int i = 0; i < sizes.size_enlarged; ++i)
    {
        node_front = 0;
        node_back = 0;

        // for each edges non visited yet
        if(voxel_ids[i].second)
        {
            edge = voxel_ids[i].second;
            back = edge->data().back();

            if (!visited_tmp[back])
            {
                // if the edge is not a branch
                if (!is_branch(edge, node_back, node_front, sizes, voxel_ids))
                {
                    // if the edge is too small, replace it by the front node
                    if (edge->length() < EDGE_THRESHOLD)
                    {
                        for (int j = 0; j < edge->data().size(); ++j)
                        {
                            ind = edge->data()[j];
                            voxel_ids[ind].second = 0;
                            voxel_ids[ind].first = node_front;
                            node_front->add_voxel(ind);
                        }

                        delete edge;

                        // Fusion the nodes that were connected to the edge: all back node voxels becomes front node.
                        for (std::list<int>::const_iterator it = node_back->positions().begin(); it != node_back->positions().end(); ++it)
                        {
                            node_front->add_voxel(*it);
                            voxel_ids[*it].first = node_front;
                        }

                        // update the fusionned node connectivity.
                        node_front->set_connectivity(node_back->connectivity() + node_front->connectivity() - 2);

                        delete node_back;
                    }
                }

                visited_tmp[back] = true;
            }
        }
    }

    delete [] visited_tmp;
}

} // end of namespace Trabecula