#include <iostream>

#include "skeletonize.hpp"



/*******************************************************************************
*   Skeletonize : 3D Thinning sequential Algorithm, which the result is stored
*                 in the char *thinned data structure.
*   @params : Analyze7.5 Header descriptor, Analyze7.5 Image data, Thinned data
*******************************************************************************/
void skeletonize(const ANALYZE_DSR *h, const char *data, char *thinned)
{

    /*  copy the Black points set indices into an array */
    /*  and create a copy of data with binary values */
    const std::size_t size = h->dime.dim[1] * h->dime.dim[2] * h->dime.dim[3];
    boost::dynamic_bitset<> data2(size);
    std::list<int> black_points_set;
    
    for(int i = 0; i < size; ++i)
    {
        if(data[i] > 0 )
        {
            data2[i] = 1;
            black_points_set.push_back(i);         
        }        
    }

    int modified; 
    
    /* Compute the 6 subiterations until no points are deleted */
    do
    {
        modified = 0;
        modified += subiter(data2, black_points_set, -h->dime.dim[1], h);                    // Up
        modified += subiter(data2, black_points_set, h->dime.dim[1], h);                     // Down
        modified += subiter(data2, black_points_set, h->dime.dim[1] * h->dime.dim[2], h);    // North
        modified += subiter(data2, black_points_set, -h->dime.dim[1] * h->dime.dim[2], h);   // South
        modified += subiter(data2, black_points_set, 1, h);                                  // East
        modified += subiter(data2, black_points_set, -1, h);                                 // West

    } while(modified > 0);

    /* copy the result into the thinned output */    
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        thinned[*p] = 1;
    }

   }

/*******************************************************************************
*   subiter : Return the number of deleted points in the subiteration from
*             a particular direction (6 subiterations to do).
*   @params : The image data bitset, the black points set, 
*             the direction, Analyze7.5 Header descriptor
*******************************************************************************/
int subiter(boost::dynamic_bitset<>& data, std::list<int>& black_points_set, int direction, const ANALYZE_DSR *h)
{
    int modified = 0;
    std::bitset<26> np( 0 );

    /* list of simple and non end points, pointers from black_points_set */
    std::list<std::list<int>::iterator> list;
    
    for(std::list<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        if( is_borderPoint( data, direction, *p) )
        {
            np = collect_26_neighbours( data, *p, h);
            if( !is_endPoint( np ) ) 
            {
                if( is_simple( np ) )
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
            np = collect_26_neighbours( data, **p, h);
            if( !is_endPoint( np ) ) 
            {
                if( is_simple( np ) )
                {
                    data.set(**p, 0);
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
*   @params : The image data bitset, the direction, the point p
*******************************************************************************/
bool is_borderPoint(const boost::dynamic_bitset<>& data, int direction, int p)
{
    return !data[p + direction];
}


/*******************************************************************************
*   collect_26_neighbours : return the 26 neighbours of a point in a bitset
*   @params : The image data bitset, the point being studied, Analyze7.5 Header descriptor
*******************************************************************************/
const std::bitset<26> collect_26_neighbours( const boost::dynamic_bitset<> &data, int p, const ANALYZE_DSR *h )
{
    std::bitset<26> np(0);

    /*  west : p - 1
        east : p + 1
        north : p + width*height
        south : p - width*height
        up : p - width
        down : p + width
    */
    int vertical = h->dime.dim[1];
    int depth = h->dime.dim[2] * h->dime.dim[1];
    
    /*  6-connected */
    np.set(0, data[p - vertical]);                //    U
    np.set(1, data[p + depth]);                   //     N
    np.set(2, data[p - 1]);                       // W
    np.set(3, data[p + 1]);                       // E
    np.set(4, data[p - depth]);                   //     S
    np.set(5, data[p + vertical]);                //   D

    /* 18-connected */    
    np.set(6, data[p - vertical + depth]);        //   U N
    np.set(7, data[p - 1 - vertical]);            // W U
    np.set(8, data[p + 1 - vertical]);            // E U
    np.set(9, data[p - vertical - depth]);        //   U S
    np.set(10, data[p - 1  + depth]);             // W   N
    np.set(11, data[p + 1 + depth]);              // E   N
    np.set(12, data[p - 1 - depth]);              // W   S
    np.set(13, data[p + 1 - depth]);              // E   S
    np.set(14, data[p + vertical + depth]);       //   D N
    np.set(15, data[p - 1 + vertical]);           // W D
    np.set(16, data[p + 1 + vertical]);           // E D
    np.set(17, data[p + vertical + depth]);       //   D S

    /* 26-connected */
    np.set(18, data[p - 1 - vertical + depth]);   // W U N
    np.set(19, data[p + 1 - vertical + depth]);   // E U N
    np.set(20, data[p - 1 - vertical - depth]);   // W U S
    np.set(21, data[p + 1 - vertical - depth]);   // E U S    
    np.set(22, data[p - 1 + vertical + depth]);   // W D N
    np.set(23, data[p + 1 + vertical + depth]);   // E D N
    np.set(24, data[p - 1 + vertical - depth]);   // W D S    
    np.set(25, data[p + 1 + vertical - depth]);   // E D S

    return np;
}


/*******************************************************************************
*   is_endPoint : Return true if there is maximum of 1 point in the neighbours
*   @params : the 26 neighbours of the point p studied
*******************************************************************************/
bool is_endPoint( const std::bitset<26> np )
{ 
    return np.count() > 1 ? false : true;
}


/*******************************************************************************
*   is_simple : return true if the point is simple, i.e does not alter the 
*               topology of the picture : 
*   1. the set N26(p)∩(B\{p}) is not empty (i.e., p is not an isolated point);
*   2. the set N26(p)∩(B\{p}) is 26–connected (in itself );
*   3. the set (ZZ^3\B)∩ N6(p) is not empty (i.e., p is a border point); and
*   4. the set (ZZ^3\B)∩ N6(p) is 6–connected in the set (ZZ^3\B)∩ N18(p)
*   @params : bits array of the 26 neighbours of the studied point
*******************************************************************************/
bool is_simple( const std::bitset<26>& np )
{
    if( is_cond_2_satisfied( np ) )
    {
        if( is_cond_4_satisfied( np ) )
        {
            return true;
        }
    }
    return false;
}


/* This condition check if the black points in the neighbourhood        */
/* of point p are 26 connected, to prevent from deleting p which would  */
/* change topology of B set. (the key of the erosion thinning process). */
/* there is at least one black neighbour to call this function          */
bool is_cond_2_satisfied(const std::bitset<26>& np)
{   
    bool visited[26] = {false};
    int i = 0;
    while(!np[i])
    {
        visited[i] = true;
        ++i;
    }

    int res = connected26(np, i, visited); 

    if(res != np.count())
    {
        return false;
    }      

    return true;
}

/* THis is a recursive fonction adding neighbors 26 connected in itself */
int connected26(const std::bitset<26>& np, int i, bool *visited)
{
    unsigned short nb = 1;
    visited[i] = true;
    unsigned short ind;
    for(int j = indicesS26[i]; j < indicesS26[i+1]; ++j)
    {   
        ind = s26[j];    
        if(!visited[ind] && np[ind])
        {            
            nb += connected26(np, ind, visited);
        }        
    }

    return nb;
}


/* This condition check if the white points in the neighbourhood        */
/* of point p are 6 connected , to prevent from deleting p which would  */
/* change topology of B set. (the key of the erosion thinning process)  */
bool is_cond_4_satisfied(const std::bitset<26>& np)
{
    bool visited[18] = {false};
    int i = 0;
    while(np[i])
    {
        visited[i] = true;
        ++i;
    }

    int res = connected6_18(np, i, visited); 
    
    if(res != 18 - (np<<8).count())
    {
        return false;
    }     

    return true;
}

/* THis is a recursive fonction adding neighbors 6 connected in 18 set */
int connected6_18(const std::bitset<26>& np, int i, bool *visited)
{
    unsigned short nb = 1;
    visited[i] = true;
    unsigned short ind;
    for(int j = indicesS6_18[i]; j < indicesS6_18[i+1]; ++j)
    {   
        ind = s6_18[j];    
        if(!visited[ind] && !np[ind])
        {            
            nb += connected6_18(np, ind, visited);
        }        
    }

    return nb;
}