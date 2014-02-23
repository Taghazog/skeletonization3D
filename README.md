The goal of this little project is to skeletonize tubular objects from a binary 3D image, and find information from the skeleton. (The data is acquired from tomography and then binarized)

We use the 3D sequential Thinning algorithm from there : http://www.inf.u-szeged.hu/ipcg/publications/papers/Palagyi_etal_IPMI2001.pdf

To use it, simply open a command line and go into your build folder, then do 

"cmake .."
"make"
"./ped myImagePath"
