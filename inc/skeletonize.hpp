#ifndef _SKELETONIZE_HPP
#define _SKELETONIZE_HPP


#include <vector>
#include <bitset>
#include <boost/dynamic_bitset.hpp>

#include "analyze_loader.hpp"
#include <list>

const unsigned short s26[167] = {  1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 18, 19, 20, 21   //    U                                  
                                 , 2, 3, 5, 6, 7, 8, 10, 11, 14, 15, 16, 18, 19, 22, 23     //     N                                  
                                 , 4, 5, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 22, 24        // W                                  
                                 , 4, 5, 6, 8, 9, 11, 13, 14, 16, 17, 19, 21, 23, 25        // E                                  
                                 , 3, 5, 7, 8, 9, 12, 13, 15, 16, 17, 20, 21, 24, 25        //     S                                  
                                 , 3, 4, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25     //   D

                                 , 3, 7, 8, 10, 11, 18, 19                                  //   U N             
                                 , 4, 6, 9, 10, 12, 18, 20                                  // W U             
                                 , 4, 6, 9, 11, 13, 19, 21                                  // E U             
                                 , 3, 7, 8, 12, 13, 20, 21                                  //   U S             
                                 , 6, 7, 14, 15, 18, 22                                     // W   N             
                                 , 6, 8, 14, 16, 19, 23                                     // E   N             
                                 , 7, 9, 15, 17, 20, 24                                     // W   S             
                                 , 8, 9, 16, 17, 21, 25                                     // E   S             
                                 , 3, 10, 11, 15, 16, 22, 23                                //   D N             
                                 , 4, 10, 12, 14, 17, 22, 24                                // W D             
                                 , 4, 11, 13, 14, 17, 23, 25                                // E D             
                                 , 3, 12, 13, 15, 16, 24, 25                                //   D S 

                                                                                            // W U N                                                      
                                                                                            // E U N                                                      
                                                                                            // W U S                                                      
                                                                                            // E U S                                                      
                                                                                            // W D N                                                      
                                                                                            // E D N                                                      
                                                                                            // W D S                                                      
                                                                                            // E D S                                                                     
                                };

const unsigned short s6_18[48] = {    6, 7, 8, 9            //    U                      
                                    , 6, 10, 11, 14         //     N                     
                                    , 7, 10, 12, 15         // W                         
                                    , 8, 11, 13, 16         // E                         
                                    , 9, 12, 13, 17         //     S                     
                                    , 14, 15, 16, 17        //   D  

                                    , 1, 0                  //   U N   
                                    , 2, 0                  // W U     
                                    , 3, 0                  // E U     
                                    , 4, 0                  //   U S   
                                    , 2, 1                  // W   N   
                                    , 3, 1                  // E   N   
                                    , 4, 2                  // W   S   
                                    , 5, 1                  //   D N   
                                    , 5, 2                  // W D     
                                    , 5, 3                  // E D     
                                    , 5, 4                  //   D S    

                                                            // W U N                                                      
                                                            // E U N                                                      
                                                            // W U S                                                      
                                                            // E U S                                                      
                                                            // W D N                                                      
                                                            // E D N                                                      
                                                            // W D S                                                      
                                                            // E D S        
                                 };


const unsigned short indicesS26[27] = {0, 16, 31, 45, 59, 73, 87, 94, 101, 108, 115, 121, 127, 133, 139, 146, 153, 160, 167, 167, 167, 167, 167, 167, 167, 167, 167};
const unsigned short indicesS6_18[19] = {0, 4, 8, 12, 16, 20, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48};

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
int connected26(const std::bitset<26>& np, int i, bool *visited);

/* This condition check if the black points in the neighbourhood       */
/* of point p are 26 connected, to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_2_satisfied(const std::bitset<26>& np);

/* THis is a recursive fonction adding neighbors connected  */
void connected6_18(const std::bitset<26>& np, int i, bool *visited, std::bitset<6>& adjacent);

/* This condition check if the white points in the neighbourhood       */
/* of point p are 6 connected , to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_4_satisfied(const std::bitset<26>& np);

#endif //_SKELETONIZE_HPP