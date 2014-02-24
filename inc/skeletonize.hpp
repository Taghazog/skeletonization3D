#ifndef _SKELETONIZE_HPP
#define _SKELETONIZE_HPP


#include <vector>
#include <bitset>
#include <boost/dynamic_bitset.hpp>

#include "analyze_loader.hpp"
#include <list>

const unsigned short s26[132] = {  1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 18, 19, 20, 21 // 4  
                                 , 2, 3, 5, 6, 7, 8, 10, 11, 14, 15, 16, 18, 19, 22, 23   // 10  
                                 , 4, 5, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 22, 24      // 12 
                                 , 4, 5, 6, 8, 9, 11, 13, 14, 16, 17, 19, 21, 23, 25      // 13   
                                 , 5, 7, 8, 9, 12, 13, 15, 16, 17, 20, 21, 24, 25         // 15  
                                 , 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25         // 21 

                                 , 7, 8, 10, 11, 18, 19             // 1
                                 , 9, 10, 12, 18, 20                // 3  
                                 , 9, 11, 13, 19, 21                // 5 
                                 , 12, 13, 20, 21                   // 7 
                                 , 14, 15, 18, 22                   // 9  
                                 , 14, 16, 19, 23                   // 11 
                                 , 15, 17, 20, 24                   // 14 
                                 , 16, 17, 21, 25                   // 16
                                 , 15, 16, 22, 23                   // 18
                                 , 17, 22, 24                       // 20
                                 , 17, 23, 25                       // 22
                                 , 24, 25                           // 24   
                                                            // 0
                                                            // 2 
                                                            // 6 
                                                            // 8
                                                            // 17
                                                            // 19
                                                            // 23
                                                            // 25                              
                                };

const unsigned short s6[12] =  {   1, 3
                                 , 1, 9
                                 , 3, 9
                                 , 5, 11
                                 , 7, 14
                                 , 20, 18                                 
                                };

const unsigned short indicesS26[27] = {0, 16, 31, 45, 59, 72, 84, 90, 95, 100, 104, 108, 112, 116, 120, 124, 127, 130, 132, 132, 132, 132, 132, 132, 132, 132, 132};
const unsigned short indicesS6[7] = {0, 2, 0, 6, 8, 10, 12};

/*******************************************************************************
*   Skeletonize : 3D Thinning sequential Algorithm, which the result is stored
*                 in the char *thinned data structure.
*   @params : Analyze7.5 Header descriptor, Analyze7.5 Image data, Thinned data
*******************************************************************************/
void skeletonize(const ANALYZE_DSR *h, const char *data, char *thinned);

/*******************************************************************************
*   subiter : Return the number of deleted points in the subiteration from
*             a particular direction (6 subiterations to do).
*   @params : The image data bitset, the black points set, 
*             the direction, Analyze7.5 Header descriptor
*******************************************************************************/
int subiter(boost::dynamic_bitset<>& data, std::list<int>& black_points_set, int direction, const ANALYZE_DSR *h);

/*******************************************************************************
*   is_borderPoint : Return true if the point p is a border Point from one
*                    direction.
*   @params : The image data bitset, the direction, the point p
*******************************************************************************/
bool is_borderPoint(const boost::dynamic_bitset<>& data, int direction, int p);


/*******************************************************************************
*   collect_26_neighbours : return the 26 neighbours of a point in a bitset
*   @params : The image data bitset, the point being studied, Analyze7.5 Header descriptor
*******************************************************************************/
const std::bitset<26> collect_26_neighbours( const boost::dynamic_bitset<> &data, int p, const ANALYZE_DSR *h );


/*******************************************************************************
*   is_endPoint : Return true if there is maximum of 1 point in the neighbours
*   @params : the 26 neighbours of the point p studied
*******************************************************************************/
bool is_endPoint( const std::bitset<26> np );


/*******************************************************************************
*   is_simple : return true if the point is simple, i.e does not alter the 
*               topology of the picture : 
*   1. the set N26(p)∩(B\{p}) is not empty (i.e., p is not an isolated point);
*   2. the set N26(p)∩(B\{p}) is 26–connected (in itself );
*   3. the set (ZZ^3\B)∩ N6(p) is not empty (i.e., p is a border point); and
*   0. the set (ZZ^3\B)∩ N6(p) is 6–connected in the set (ZZ^3\B)∩ N18(p)
*   @params : bits array of the 26 neighbours of the studied point
*******************************************************************************/
bool is_simple( const std::bitset<26>& np );

/* THis is a recursive fonction adding neighbors connected  */
int connected(const std::bitset<26>& np, int i, bool *visited);

/* This condition check if the black points in the neighbourhood       */
/* of point p are 26 connected, to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_2_satisfied(const std::bitset<26>& np);

/* This condition check if the white points in the neighbourhood       */
/* of point p are 6 connected , to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_4_satisfied(const std::bitset<26>& np);

#endif //_SKELETONIZE_HPP