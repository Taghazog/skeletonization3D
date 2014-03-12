////////////////////////////////////////////////////////////////
//
// This is TubularObject class
//
////////////////////////////////////////////////////////////////

#ifndef TUBULAR_OBJECT_HPP
#define TUBULAR_OBJECT_HPP

#include <cstdlib>
#include <string>
#include <vector>

#include "analyze_loader.hpp"

class Node;

struct Sizes
{
	std::size_t size_x;
	std::size_t size_y;
	std::size_t size_z;
	std::size_t size_x_enlarged;
	std::size_t size_y_enlarged;
	std::size_t size_z_enlarged;
	std::size_t size;
	std::size_t size_enlarged;	
	std::size_t xOy_size;	
	std::size_t xOy_enlarged_size;
	
};

class TubularObject
{

public:
	/* Constructors/Destructors */
    TubularObject();  
    TubularObject(const std::string& filename);   
    ~TubularObject();

public:
	/* Setters */
    

public:
	/* Getters */
    const unsigned char* data() const;
    const unsigned char* skeleton_data() const;
    const ANALYZE_DSR* dsr() const;

public:
	/* Member Functions */
    void skeletonize();
    void build_Graph();

private:
	/* Member Variables */ 
	ANALYZE_DSR *mDsr; 
	Sizes mSizes;

	unsigned char* mData;
	unsigned char* mSkeleton;	
	Node* mRoot;	

	
    
};

#endif // TUBULAR_OBJECT_HPP


////////////////////////////////////////////////////////////////
//
// This is Node class
//
////////////////////////////////////////////////////////////////

#ifndef NODE_HPP
#define NODE_HPP

class Edge;

class Node
{

public:
	/* Constructors/Destructors */
    Node();     
    Node(int ind);
    ~Node();

public:
	/* Setters */
	
    

public:
	/* Getters */
	int position() const;
	int connectivity() const;
    const std::vector<Edge*>& edges() const;

public:
	/* Member Functions */
    void add_Edge(Edge* edge);

private:
	/* Member Variables */
	std::vector<Edge*> mEdges;
	int mIndice;
	int mConnectivity;    
    
};

#endif // NODE_HPP

////////////////////////////////////////////////////////////////
//
// This is Edge class
//
////////////////////////////////////////////////////////////////

#ifndef EDGE_HPP
#define EDGE_HPP

class Edge
{

public:
	/* Constructors/Destructors */
    Edge(); 
    ~Edge();

public:
	/* Setters */
	void set_dest_Node(Node* to);
	void set_origin_Node(Node* from);
    

public:
	/* Getters */
	int length() const;
	const Node* from() const;
	const Node* to() const;
	const std::vector<int>& data() const;
    

public:
	/* Member Functions */
    void add_voxel(int ind);
    

private:
	/* Member Variables */    
	int mLength;
	std::vector<int> mIndices;
	Node* mFrom;
	Node* mTo;
    
};


#endif // EDGE_HPP
