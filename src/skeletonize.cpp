#include "skeletonize.hpp"


/*******************************************************************************
*   Skeletonize : 3D Thinning sequential Algorithm, which the result is stored
*                 in the char *thinned data structure.
*   @params : Analyze7.5 Header descriptor, Analyze7.5 Image data, Thinned data
*******************************************************************************/
void skeletonize(const ANALYZE_DSR *h, const char *data, char *thinned)
{

    /*  copy the Black points set indices into an array */
    /*  and create a copy of data with zeros in borders */
    const std::size_t size = (h->dime.dim[1] + 1) * (h->dime.dim[2] + 1) * (h->dime.dim[3] + 1);
    boost::dynamic_bitset<> data2(size);
    std::vector<int> black_points_set;        
    
    for(int i = 0; i < h->dime.dim[1] * h->dime.dim[2] * h->dime.dim[3]; ++i)
    {
        if(data[i] != 0)
        {
            data2[i] = 1;
            black_points_set.push_back(i);            
        }        
    }

    int modified = 0;
    
    /* Compute the 6 subiterations until no points are deleted */
    do
    {
        modified += subiter(data2, black_points_set, -h->dime.dim[1], h);                    // Up
        modified += subiter(data2, black_points_set, h->dime.dim[1], h);                     // Down
        modified += subiter(data2, black_points_set, h->dime.dim[1] * h->dime.dim[2], h);    // North
        modified += subiter(data2, black_points_set, -h->dime.dim[1] * h->dime.dim[2], h);   // South
        modified += subiter(data2, black_points_set, 1, h);                                  // East
        modified += subiter(data2, black_points_set, -1, h);                                 // West

    } while(modified > 0);

    /* copy the result into the thinned output */
    for(std::vector<int>::iterator iter = black_points_set.begin(); iter != black_points_set.end(); ++iter)
    {
        thinned[*iter] = 1;
    }
}

/*******************************************************************************
*   subiter : Return the number of deleted points in the subiteration from
*             a particular direction (6 subiterations to do).
*   @params : The image data bitset, the black points set, 
*             the direction, Analyze7.5 Header descriptor
*******************************************************************************/
int subiter(boost::dynamic_bitset<>& data, std::vector<int>& black_points_set, int direction, const ANALYZE_DSR *h)
{
    int modified = 0;
    std::bitset<26> np( 0 );

    /* list of simple and non end points */
    std::vector<int> list;
    
    for(std::vector<int>::iterator p = black_points_set.begin(); p != black_points_set.end(); ++p)
    {
        if( is_borderPoint( data, direction, *p) )
        {
            np = collect_26_neighbours( data, *p, h);
            if( !is_endPoint( np ) ) 
            {
                if( is_simple( np ) )
                {
                    list.push_back(*p);
                }                    
            }
        }        
    }
                
    while( list.empty() )
    {
        for(std::vector<int>::iterator p = list.begin(); p != list.end(); ++p)
        {
            np = collect_26_neighbours( data, *p, h);
            if( !is_endPoint( np ) )
            {
                if( is_simple( np ) )
                {
                    black_points_set.erase(p);
                    list.erase(p);
                    ++modified;
                }
            }        
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
    


    np.set(0, data[p - 1 - vertical + depth]);    // 0 :  W U N
    np.set(1, data[p]);                           // 1 :    U N
    np.set(2, data[p + 1 - vertical + depth]);    // 2 :  E U N
    np.set(3, data[p - 1 - vertical]);            // 3 :  W U
    np.set(4, data[p - vertical]);                // 4 :    U
    np.set(5, data[p + 1 - vertical]);            // 5 :  E U
    np.set(6, data[p - 1 - vertical - depth]);    // 6 :  W U S
    np.set(7, data[p - vertical - depth]);        // 7 :    U S
    np.set(8, data[p + 1 - vertical - depth]);    // 8 :  E U S
    np.set(9, data[p - 1  + depth]);              // 9 :  W   N
    np.set(10, data[p + depth]);                  // 10 :     N
    np.set(11, data[p + 1 + depth]);              // 11 : E   N
    np.set(12, data[p - 1]);                      // 12 : W
    np.set(13, data[p + 1]);                      // 13 : E
    np.set(14, data[p - 1 - depth]);              // 14 : W   S
    np.set(15, data[p - depth]);                  // 15 :     S
    np.set(16, data[p + 1 - depth]);              // 16 : E   S
    np.set(17, data[p - 1 + vertical + depth]);   // 17 : W D N
    np.set(18, data[p + vertical + depth]);       // 18 :   D N
    np.set(19, data[p + 1 + vertical + depth]);   // 19 : E D N
    np.set(20, data[p - 1 + vertical]);           // 20 : W D
    np.set(21, data[p + vertical]);               // 21 :   D
    np.set(22, data[p + 1 + vertical]);           // 22 : E D
    np.set(23, data[p - 1 + vertical - depth]);   // 23 : W D S
    np.set(24, data[p + vertical + depth]);       // 24 :   D S
    np.set(25, data[p + 1 + vertical - depth]);   // 25 : E D S
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


/* This condition check if the black points in the neighbourhood       */
/* of point p are 26 connected, to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_2_satisfied(const std::bitset<26>& np)
{/*
    int label = 0;
    int L[26];
    for( int i = 0; i < 26; ++i)
    {
        L[i] = 0;
    }
    
    for( int i = 0; i < 26; ++i)
    {
        if( np[i] == 1 )
        {
            ++label;
        }
        
        for( int j = indicesS26[i]; j < indicesS26[i+1]; ++j)
        {
            if( L[s26[j]] > 0 )
            {
                for( int k = 0; k < i; ++k)
                {
                    if( L[k] == L[j] )
                    {   
                        L[k] = label;
                    } 
                }            
            }            
        }        
    }    
    
    for( int i = 0; i < 26; ++i)
    {
        if ( np[i] == 1 && L[i] != label )
        {
            return false;
        }        
    }
    */
    return true;
}

/* This condition check if the white points in the neighbourhood       */
/* of point p are 6 connected , to prevent from deleting p which would */
/* change topology of B set. (the key of the erosion thinning process) */
bool is_cond_4_satisfied(const std::bitset<26>& np)
{/*
    int label = 0;
    int L[6];
    for( int i = 0; i < 6; ++i)
    {
        L[i] = 0;
    }
    
    for( int i = 0; i < 6; ++i)
    {
        if( np[i] == 0 )
        {
            ++label;
        }
        
        for( int j = indicesS6[i]; j < indicesS6[i+1]; ++j)
        {
            if( L[s6[j]] > 0 )
            {
                for( int k = 0; k < i; ++k)
                {
                    if( L[k] == L[j] )
                    {   
                        L[k] = label;
                    } 
                }            
            }            
        }        
    }    
    
    for( int i = 0; i < 26; ++i)
    {
        if ( np[i] == 0 && L[i] != label )
        {
            return false;
        }        
    }*/
    
    return true;
}
