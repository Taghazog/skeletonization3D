
#include <cstdio>
#include <iostream>
#include <cstring>

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
    imageFilename = imageFilename + ".img";
    anaReadHeader(headerFilename.c_str(), h);

    //FILE *headerOF = fopen("print.txt", "w");
    //anaPrintHeader(h, headerOF);

    char *data = new char[  h->dime.dim[1] * 
                            h->dime.dim[2] *
                            h->dime.dim[3]];
    
    anaReadImagedata(imageFilename.c_str(), h, 1, data);
    

    char *thinned = new char[   h->dime.dim[1] * 
                                h->dime.dim[2] *
                                h->dime.dim[3]](); 

    skeletonize(h, data, thinned);

    delete(data);
    


    FILE *imageOF = fopen("printImageThinned.txt", "w");
    for (int k = 0; k < 154; ++k)
    {
        for(int i = 0; i < 154; ++i)
        {
            for(int j = 0; j < 154; ++j)
                fprintf(imageOF, "%d", thinned[k*154*154 + i*154 + j]);
            fprintf(imageOF, "\n");
            
        }
        fprintf(imageOF, "\n\n\n\n");
    }    

    return 1;
}
