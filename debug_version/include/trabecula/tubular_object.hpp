/**********************************************************************/
/*  Copyright (c) 2014, Jerome Bouzillard
/*  All rights reserved.
/*
/*  Redistribution and use in source and binary forms, with or without
/*  modification, are permitted as soon as it retains the above copyright
/*  notice.
*********************************************************************/

#ifndef TUBULAR_OBJECT_HPP
#define TUBULAR_OBJECT_HPP

#include "trabecula/analyze_loader.hpp"

#include <cstdlib>
#include <string>
#include <list>
#include <vector>
#include <deque>

namespace Trabecula
{

class Node;
class Edge;


/* Struct storing 3D dimensions and the enlarged dimensions */
/*	when images are zero-bordered                           */
struct Sizes
{
	unsigned int size_x;
	unsigned int size_y;
	unsigned int size_z;
	unsigned int size_x_enlarged;
	unsigned int size_y_enlarged;
	unsigned int size_z_enlarged;
	unsigned int size;
	unsigned int size_enlarged;
	unsigned int xOy_size;
	unsigned int xOy_enlarged_size;
};

/********************************************************/
/* Main class, Tubular_object stores all the structures */
/* necessary to the analyze of trabeculae :             */
/* image data from Analayze 7.5, skeleton, graph.       */
/* many medical measures can be computed to the object  */
/* via its member functions.                            */
/********************************************************/
class Tubular_object
{

public:
	/* Constructors/Destructors */
    Tubular_object();
    ~Tubular_object();

public:
	/* Setters */


public:
	/* Getters */
    const unsigned char* data() const;
    const unsigned char* skeleton_data() const;
    const std::list<Node*>& nodes() const;
    const std::list<Edge*>& edges() const;

    const ANALYZE_DSR* dsr() const;
    const Sizes& sizes() const;

public:
	/* Member Functions */
	int load_from_file(const std::string& filename);
	float bv_tv() const;
	void average_trabecular_length(float values[4]);
	int number_of_trabeculae();
	void nodes_connectivity(std::vector<int>& con);
    void tb_th();
    void tb_sp();
    void tb_shape();
    int skeletonize();
    int build_graph();
    int dump_infos();
    int save_skeleton();

private:
	/* Member Variables */
	ANALYZE_DSR *mDsr;
	Sizes mSizes;

	std::string mFilename;

	unsigned char* mData;
	unsigned char* mSkeleton;

	std::list<Node*> mNodes;
	std::list<Edge*> mEdges;

};

////////////////////////////////////////////////////////////////
//
// This is Node class, having connected Edges, voxels on the node
// and the connectivity (number of edges connected)
//
////////////////////////////////////////////////////////////////

class Node
{

public:
	/* Constructors/Destructors */
    Node();
    ~Node();

public:
	/* Setters */
	void set_connectivity(int nb_edges);


public:
	/* Getters */
	int connectivity() const;
    const std::list<Edge*>& edges() const;
    const std::list<int>& positions() const;


public:
	/* Member Functions */
    void add_edge(Edge* edge);
    void add_voxel(int indice);
    void remove_voxel(int indice);

private:
	/* Member Variables */
	std::list<Edge*> mEdges;
	std::list<int> mPositions;
	int mConnectivity;

};

////////////////////////////////////////////////////////////////
//
// This is Edge class, having destination and origin nodes,
// length, and a set of voxels.
//
////////////////////////////////////////////////////////////////

class Edge
{

public:
	/* Constructors/Destructors */
    Edge();
    ~Edge();

public:
	/* Setters */
	void set_nodes(Node* node);

public:
	/* Getters */
	float length() const;
	const Node* first() const;
	const Node* second() const;
	const std::deque<int>& data() const;

public:
	/* Member Functions */
    void add_voxel(int ind, int adjacency, bool front);

private:
	/* Member Variables */
	float mLength;
	std::deque<int> mIndices;
	Node* mFirst;
	Node* mSecond;
};

} // end of namespace Trabecula

#endif // TUBULAR_OBJECT_HPP
