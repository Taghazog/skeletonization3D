
#include "tubular_object.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>



int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout << "usage: filename without extension" << std::endl;
        return 0;
    }
    const std::string filename = argv[1];
    
    TubularObject* cancellous_Bones = new TubularObject(filename);

    cancellous_Bones->skeletonize();
    cancellous_Bones->build_Graph();
    
    //TODO : write in files information, and skeleton data
    //FILE *headerOF = fopen("print.txt", "w");
    //anaPrintHeader(h, headerOF);    

    delete cancellous_Bones;
    
    return EXIT_SUCCESS;

}

