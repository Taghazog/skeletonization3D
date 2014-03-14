////////////////////////////////////////////////////////////////
//
// This file provides implementation of skeletonization of a 
// Tubular Object, a Graph (Nodes, Edges) of the skeleton, and 
//  information related to the Tubular object.
//  @implements TubularObject, Node, Edge.
//
////////////////////////////////////////////////////////////////

#include "analyze_loader.hpp"
#include "tubular_object.hpp"

#include <iostream>
#include <cstring>

#include <bitset>
#include <list>

/***********************************************  UTILITIES  declaration  ***************************************************/

/* This constant provides all the 26-adjacent neighbours of each of the 26 neighbours of a point */
static const unsigned short s26[171] = {  1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 18, 19, 20, 21    //    U                                  
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
static const unsigned short s6_18[48] = { 6, 7, 8, 9        //    U                      
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
                                        , 5, 1              //   D N   
                                        , 5, 2              // W D     
                                        , 5, 3              // E D     
                                        , 5, 4              //   D

                                                            // W U N                                                      
                                                            // E U N                                                      
                                                            // W U S                                                      
                                                            // E U S                                                      
                                                            // W D N                                                      
                                                            // E D N                                                      
                                                            // W D S                                                      
                                                            // E D S        
                                        };


static const unsigned short indicesS26[27] = {0, 16, 31, 45, 59, 73, 87, 94, 101, 108, 115, 122, 129, 136, 143, 150, 157, 164, 171, 171, 171, 171, 171, 171, 171, 171, 171};
static const unsigned short indicesS6_18[19] = {0, 4, 8, 12, 16, 20, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48};

static int subiter(unsigned char* data, std::list<int>& black_points_set, int direction, const Sizes& sizes);
static bool is_borderPoint(const unsigned char* data, int direction, int p);
static int collect_26_neighbours( const unsigned char* data, int p, const Sizes& sizes, int np[26] );
static bool is_simple(const unsigned char* data, const int np[26], int nb );
static int connected26(const unsigned char* data, const int np[26], int i, bool *visited);
static bool is_cond_2_satisfied(const unsigned char* data, const int np[26], int nb);
static void connected6_18(const unsigned char* data, const int np[26], int i, bool *visited, std::bitset<6>& adjacent);
static bool is_cond_4_satisfied(const unsigned char* data, const int np[26]);

static void bordering(const unsigned char* from, unsigned char* with_borders, const Sizes& sizes);
static int untransformed(int indice, const Sizes& sizes);

static int findNode(const unsigned char *thinned, const Sizes& sizes, int np[26]);
static void depth_search(int ind, Edge* edge, const unsigned char *data, const Sizes& sizes, bool * visited);

static void process_nodes_from_graph(const Node* node, std::vector<const Node*>& indices);
static void process_indices_from_graph(const Node* node, std::vector<int>& indices);

/***********************************************  TubularObject  definition  ************************************************/

/* Constructors/Destructors */
TubularObject::TubularObject(): mData(NULL), mSkeleton(NULL), mDsr(NULL)
{

}

TubularObject::TubularObject(const std::string& filename)
{
    mDsr = new ANALYZE_DSR; 

    std::string headerFilename;
    headerFilename = filename + ".hdr";

    std::string imageFilename;
    imageFilename = filename + ".img";

    if(anaReadHeader(headerFilename.c_str(), mDsr))
    {
        std::cerr << "Image header read failed!" << std::endl;
        exit(EXIT_FAILURE);
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
    

    mSkeleton = new unsigned char[ mSizes.size ]; 

    memset(mSkeleton, 0, mSizes.size * sizeof(unsigned char)); 

    mData = new unsigned char[ mSizes.size ];
    
    if(anaReadImagedata(imageFilename.c_str(), mDsr, 1, (char*)mData))
    {
        std::cerr << "Image data read failed!" << std::endl;
        exit(EXIT_FAILURE);
    }    
}  

TubularObject::~TubularObject()
{
    delete mDsr;
    delete [] mData;
    delete [] mSkeleton;    

    //delete_graph();
}

/* Setters */
    


/* Getters */
const unsigned char* TubularObject::data() const
{
    return mData;
}   

/* return an array with the indices of the nodes from the graph */
const unsigned char* TubularObject::nodes_data() const
{      
    std::vector<const Node*> nodes;    
    nodes.push_back( mRoot);   

    process_nodes_from_graph(mRoot, nodes);   

    unsigned char* data = new unsigned char[mSizes.size];
    memset(data, 0, mSizes.size * sizeof(unsigned char));

    for (std::vector<const Node*>::const_iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it)
    {
        if((*node_it)->connectivity() > 2)
        data[(*node_it)->position()] = 1;
    }  
    return data;
}  

const unsigned char* TubularObject::skeleton_data() const
{
    return mSkeleton;
}  

const ANALYZE_DSR* TubularObject::dsr() const
{
    return mDsr;
}  

/********************************************************************
* this function return the volume pourcentage bone/total
*  this function assumes the image dimensions to be the same
*  and the image being a sphere and not a cube. 
*********************************************************************/
float TubularObject::bv_tv() const
{
    const float pi = 3.14159265;
    float total = 1.0/6.0 * pi * mSizes.size; // 4/3 * Pi * R^3
    float nb_object_voxels = 0.0;

    for (int i = 0; i < mSizes.size; ++i)
    {
        if(mData[i])
        {
            nb_object_voxels += 1.0;
        }
    }
    return nb_object_voxels/total * 100.0;
}

/********************************************************************
* this function computes the average trabeculae thickness
*********************************************************************/
void TubularObject::tb_Th()
{
    //TODO
}

/********************************************************************
* this function computes the average trabeculae spacing
*********************************************************************/
void TubularObject::tb_Sp()
{
    //TODO
}

/********************************************************************
* this function provides information about the shape of trabeculae.
* it classifies rod-like and plate-like trabeculae.
*********************************************************************/
void TubularObject::tb_Shape()
{
    //TODO
}

/* Member Functions */
/*******************************************************************************
*   Skeletonize : this function compute a skeleton of tubular object and store
*   its result in the skeleton data structure. 
*   from : A sequential 3D thinning algorithm and its medical applications (2001)  
*******************************************************************************/
void TubularObject::skeletonize()
{
    /*  copy the Black points set indices into an array */
    /*  and create a copy of data with binary values and zero borders */    
    unsigned char *data_tmp = new unsigned char[mSizes.size_enlarged];
    bordering(mData, data_tmp, mSizes);   

    std::list<int> black_points_set;   
    
    for(int i = 0; i < mSizes.size_enlarged; ++i)
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
        modified += subiter(data_tmp, black_points_set, -mSizes.size_x_enlarged, mSizes);       // Up
        modified += subiter(data_tmp, black_points_set, mSizes.size_x_enlarged, mSizes);        // Down
        modified += subiter(data_tmp, black_points_set, mSizes.xOy_enlarged_size, mSizes);      // North
        modified += subiter(data_tmp, black_points_set, -mSizes.xOy_enlarged_size, mSizes);     // South
        modified += subiter(data_tmp, black_points_set, 1, mSizes);                             // East
        modified += subiter(data_tmp, black_points_set, -1, mSizes);                            // West

    } while(modified > 0);

    /* copy the result into the skeleton data */    
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {        
        mSkeleton[untransformed((*p), mSizes)] = 255;
    }    
}   

void TubularObject::build_Graph()
{ 
    /*  Create a copy of data with binary values and zero borders */    
    unsigned char *data_tmp = new unsigned char[mSizes.size_enlarged];
    bordering(mSkeleton, data_tmp, mSizes); 

    int np[26]; 
    bool * visited = new bool[mSizes.size_enlarged];
    memset(visited, 0, mSizes.size_enlarged * sizeof(bool));   

    int ind = findNode(data_tmp, mSizes, np);

    if(ind == mSizes.size_enlarged)
    {
        std::cerr << "couldnt build graph, skeleton is empty or no nodes in it!" << std::endl;
        exit(EXIT_FAILURE);
    }

    mRoot = new Node(untransformed(ind, mSizes));    
    visited[ind] = true;

    for (int i = 0; i < 26; ++i)
    {
        if (data_tmp[np[i]] != 0 && !visited[np[i]])
        {
            Edge* edge = new Edge();
            mRoot->add_Edge(edge);
            visited[np[i]] = true;
            depth_search(np[i], edge, data_tmp, mSizes, visited);
        }
    }  

    delete [] visited;
}


/***********************************************  Node  definition  *********************************************************/

/* Constructors/Destructors */
Node::Node() : mIndice(0), mConnectivity(0)
{

}     

Node::Node(int ind) : mIndice(ind), mConnectivity(0)
{

}     

Node::~Node()
{
    
}     

/* Setters */
     

/* Getters */
int Node::position() const
{
    return mIndice;
}   

int Node::connectivity() const
{
    return mConnectivity;
} 

const std::vector<Edge*>& Node::edges() const
{
    return mEdges;
}     

/* Member Functions */
void Node::add_Edge(Edge* edge)
{
    ++mConnectivity;
    edge->set_origin_Node(this);
    mEdges.push_back(edge);
}

/***********************************************  Edge  definition  *********************************************************/

/* Constructors/Destructors */

Edge::Edge() : mLength(0), mFrom(NULL), mTo(NULL)
{
    
} 

Edge::~Edge()
{
    delete mFrom;
    delete mTo;    
}     

/* Setters */
void Edge::set_dest_Node(Node* to)
{
    add_voxel(to->position());
    mTo = to;
}

void Edge::set_origin_Node(Node* from)
{
    add_voxel(from->position());
    mFrom = from;
}

/* Getters */
int Edge::length() const
{
    return mLength;
}   

const Node* Edge::from() const
{
    return mFrom;
}   

const Node* Edge::to() const
{
    return mTo;
} 

const std::vector<int>& Edge::data() const
{
    return mIndices;
}     

/* Member Functions */
void Edge::add_voxel(int ind)
{
    ++mLength;
    mIndices.push_back(ind);
}


/***********************************************  UTILITIES  definition  ****************************************************/

/*******************************************************************************
*   subiter : Return the number of deleted points in the subiteration from
*             a particular direction (6 subiterations to do).
*   @params : The image data, the black points set, 
*             the direction, Analyze7.5 Header descriptor
*******************************************************************************/
static int subiter(unsigned char* data, std::list<int>& black_points_set, int direction, const Sizes& sizes)
{
    int modified = 0;
    int np[26];
    int nb = 0;

    /* list of simple and non end points, pointers from black_points_set */
    std::list<std::list<int>::iterator> list;
    
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        if( is_borderPoint( data, direction, *p) )
        {
            nb = collect_26_neighbours( data, *p, sizes, np);
            if( nb > 1 ) 
            {
                if( is_simple( data, np, nb ) )
                {
                    list.push_back(p);
                }                    
            }
        }        
    }
              
    int value = -1;  

    while( value != modified )
    {
        value = modified;
        for(std::list<std::list<int>::iterator>::iterator p = list.begin(); p != list.end();)
        {
            nb = collect_26_neighbours( data, **p, sizes, np);
            if( nb > 1 ) 
            {
                if( is_simple( data, np, nb ) )
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
static bool is_borderPoint(const unsigned char* data, int direction, int p)
{
    return !data[p + direction];
}


/*******************************************************************************
*   collect_26_neighbours : return the number of neighbours of a point and save the
*   neighbour indices in the data "neighbours".
*   @params : The image data, the point p, Analyze7.5 Header descriptor, 26 neghbours
*******************************************************************************/ 
static int collect_26_neighbours( const unsigned char* data, int p, const Sizes& sizes, int np[26] )
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

    for (int i = 0; i < 26; ++i)
    {
        if(data[np[i]] != 0)
        {
            ++nb;
        }
    }

    return nb;
}

/*******************************************************************************
*   is_simple : return true if the point is simple, i.e does not alter the 
*               topology of the picture : 
*   1. the set N26(p)∩(B\{p}) is not empty (i.e., p is not an isolated point);
*   2. the set N26(p)∩(B\{p}) is 26–connected (in itself );
*   3. the set (ZZ^3\B)∩ N6(p) is not empty (i.e., p is a border point); and
*   4. the set (ZZ^3\B)∩ N6(p) is 6–connected in the set (ZZ^3\B)∩ N18(p)
*   @params : data, 26 neighbours of p, number of neighbours
*******************************************************************************/
static bool is_simple( const unsigned char* data, const int np[26], int nb )
{
    if( is_cond_2_satisfied( data, np, nb ) )
    {
        if( is_cond_4_satisfied( data, np ) )
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
static bool is_cond_2_satisfied(const unsigned char* data, const int np[26], int nb)
{   
    bool visited[26] = {false};
    int i = 0;
    while(!data[np[i]])
    {
        visited[i] = true;
        ++i;
    }

    int res = connected26(data, np, i, visited); 

    if(res != nb)
    {
        return false;
    }      

    return true;
}

/********************************************************************************  
* This is a recursive fonction adding neighbors 26-adjacent in itself 
*********************************************************************************/
static int connected26(const unsigned char* data, const int np[26], int i, bool *visited)
{
    unsigned short nb = 1;
    visited[i] = true;
    unsigned short ind;
    for(int j = indicesS26[i]; j < indicesS26[i+1]; ++j)
    {   
        ind = s26[j];    
        if(!visited[ind] && data[np[ind]])
        {            
            nb += connected26(data, np, ind, visited);
        }        
    }

    return nb;
}

/********************************************************************************  
* This condition check if the white points in the 6 neighbourhood      
* of point p are 6-adjacent in 18-adjacency to keep the topology      
* of B set. (the key of the erosion thinning process)  
*********************************************************************************/
static bool is_cond_4_satisfied(const unsigned char* data, const int np[26])
{
    bool visited[18] = {false};
    std::bitset<6> adjacent(0b000000);
    int i = 0;
    while(data[np[i]])
    {
        visited[i] = true;        
        ++i;
    }

    adjacent[i] = 1;
    connected6_18(data, np, i, visited, adjacent); 
    
    if(adjacent.count() != 6 - (data[np[0]] + data[np[1]] + data[np[2]] + data[np[3]] + data[np[4]] + data[np[5]]))
    {
        return false;
    }     

    return true;
}

/********************************************************************************  
* This is a recursive fonction adding 6-adjacent neighbors 6-adjacent in 18 set 
********************************************************************************/
static void connected6_18(const unsigned char* data, const int np[26], int i, bool *visited, std::bitset<6>& adjacent)
{
    visited[i] = true;
    unsigned short ind;
    for(int j = indicesS6_18[i]; j < indicesS6_18[i+1]; ++j)
    {   
        ind = s6_18[j];    
        if(!visited[ind] && !data[np[ind]])
        { 
            if(ind >= 0 && ind < 6)
            {
                adjacent[ind] = 1;   
            }                 
            connected6_18(data, np, ind, visited, adjacent);
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
*   This function finds a node in the skeleton that is not yet visited
*   to build the graph
**************************************************************************/
int findNode(const unsigned char *data, const Sizes& sizes, int np[26])
{ 
    int nb;
    int i = 0; 
    while (i < sizes.size_enlarged )
    {        
        if(data[i] != 0)
        {
            nb = collect_26_neighbours(data, i, sizes, np); 
            if(nb > 2)
            {
                return i;
            }           
            
        }
        ++i;
    }
    return i;
}

/**************************************************************************  
*   This function compute a depth-first search algorithm to build the graph
*   of the skeleton.
**************************************************************************/
void depth_search(int ind, Edge* edge, const unsigned char *data, const Sizes& sizes, bool * visited)
{
    int np[26];    

    int nb = 0;
    int indice = untransformed(ind, sizes);
    collect_26_neighbours(data, ind, sizes, np);  

    std::vector<int> unvisited;

    for (int i = 0; i < 26; ++i)
    {
        if(data[np[i]] != 0 && !visited[np[i]])
        {
            ++nb;
            unvisited.push_back(np[i]);
            visited[np[i]] = true;    
        }           
    }

    if(nb != 1)
    {
        Node* newNode = new Node(indice);
        edge->set_dest_Node(newNode);

        if (nb > 1)
        {
            for (std::vector<int>::iterator it = unvisited.begin(); it != unvisited.end(); ++it)
            {        
                Edge* ed = new Edge();
                newNode->add_Edge(ed);
                depth_search(*it, ed, data, sizes, visited);                
            }
        }        
    }
    else
    {
        edge->add_voxel(indice);         
        depth_search(unvisited[0], edge, data, sizes, visited);  
    }
    
   
}

static void process_nodes_from_graph(const Node* node, std::vector<const Node*>& nodes)
{
    
    std::vector<Edge*> edges = node->edges();
    for(std::vector<Edge*>::const_iterator edge_it = edges.begin(); edge_it != edges.end(); ++edge_it)
    {   
        nodes.push_back((*edge_it)->from());
        process_nodes_from_graph((*edge_it)->to(), nodes);
    }
}

static void process_indices_from_graph(const Node* node, std::vector<int>& indices)
{
    
    std::vector<Edge*> edges = node->edges();
    for(std::vector<Edge*>::const_iterator edge_it = edges.begin(); edge_it != edges.end(); ++edge_it)
    {   
        //debug += (*edge_it)->length();
        for(std::vector<int>::const_iterator ind_it = (*edge_it)->data().begin()+1; ind_it != (*edge_it)->data().end(); ++ind_it)
        {          
            indices.push_back(*ind_it);
        }
        //debug -= (*edge_it)->to()->connectivity();
        process_indices_from_graph((*edge_it)->to(), indices);
    }
}

 