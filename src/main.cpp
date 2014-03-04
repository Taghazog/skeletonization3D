
#include <cstdio>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "analyze_loader.hpp"
#include "skeletonize.hpp"

int main(int argc, char *argv[])
{
    ANALYZE_DSR *h = new ANALYZE_DSR;

    if(argc < 2)
    {
        std::cout << "usage: filename" << std::endl;
        return 0;
    }
    std::string headerFilename = argv[1];
    headerFilename = headerFilename + ".hdr";
    std::string imageFilename = argv[1];
    std::string skeletonized_image = imageFilename + "_skeletonized.img";
    imageFilename = imageFilename + ".img";
    anaReadHeader(headerFilename.c_str(), h);

    FILE *headerOF = fopen("print.txt", "w");
    anaPrintHeader(h, headerOF);

    char *data = new char[  h->dime.dim[1] * 
                            h->dime.dim[2] *
                            h->dime.dim[3]];
    
    anaReadImagedata(imageFilename.c_str(), h, 1, data);    

    char *thinned = new char[   h->dime.dim[1] * 
                                h->dime.dim[2] *
                                h->dime.dim[3]]; 

    memset(thinned, 0, h->dime.dim[1] * h->dime.dim[2] * h->dime.dim[3]);

    skeletonize(h, data, thinned);  
    
    anaWriteImagedata(skeletonized_image.c_str(), h, thinned);

    delete [] data;
    delete [] thinned;
    
    return EXIT_SUCCESS;

}

