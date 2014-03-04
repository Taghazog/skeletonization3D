
#include <cstdio>
#include <iostream>
#include <fstream>
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

    /* Read the data from the Header, and the Image Data, to process Thinning */
    std::string headerFilename = argv[1];
    headerFilename = headerFilename + ".hdr";
    std::string imageFilename = argv[1];
    std::string skeletonized_image = imageFilename + "_skeletonized.img";
    std::string skeletonized_header = headerFilename + "_skeletonized.hdr";
    imageFilename = imageFilename + ".img";
    anaReadHeader(headerFilename.c_str(), h);

    char *data = new char[  h->dime.dim[1] * 
                            h->dime.dim[2] *
                            h->dime.dim[3]];
    
    anaReadImagedata(imageFilename.c_str(), h, 1, data);    

    //FILE *headerOF = fopen("print.txt", "w");
    //anaPrintHeader(h, headerOF);

    /* Process the Thinning Algorithm on the 3D tubular binary image data */
    char *thinned = new char[   h->dime.dim[1] * 
                                h->dime.dim[2] *
                                h->dime.dim[3]]; 
    memset(thinned, 0, h->dime.dim[1] * h->dime.dim[2] * h->dime.dim[3]);
    skeletonize(h, data, thinned);  

    /* Copying the header file for the new skeletonized Image File */
    anaWriteImagedata(skeletonized_image.c_str(), h, thinned);   

    std::ifstream source(headerFilename.c_str());    
    std::ofstream dest(skeletonized_header.c_str());   

    if(source && dest)
    {
        dest << source.rdbuf();
    }
    
    /* free any used memory */
    delete [] data;
    delete [] thinned;
    
    return EXIT_SUCCESS;

}

