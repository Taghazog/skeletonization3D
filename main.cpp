
#include "trabecula/tubular_object.hpp"

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

    Trabecula::Tubular_object* cancellous_bones = new Trabecula::Tubular_object();

    cancellous_bones->load_from_file(filename);
    cancellous_bones->skeletonize();
    cancellous_bones->build_graph();
    cancellous_bones->save_skeleton();
    cancellous_bones->dump_infos();

    delete cancellous_bones;

    return EXIT_SUCCESS;

}

