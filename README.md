# README #

This repository can be used to write code that links to both SCOREC/core and MFEM libraries and is meant to be used for the finite element course assignments/projects.

### What is this repository for? ###

* environment setup script
* cmake config script
* example codes using both SCOREC/core and MFEM libraries
* blank .cpp and .hpp files corresponding to the assignment

### Build Instructions ###

Note: the following build instructions are only tested on SCOREC workstations, and they can be modified for other systems (e.g., your personal computer).

Assuming you are at the root source directory, follow these steps

0. `source modules.sh`
1. `mkdir build`
2. `cd build`
3. `export PUMIPATH=path_to_pumi_install`, where __path_to_pumi_install__ is the path to the installation of SCOREC/core
3. `export MFEMPATH=path_to_mfem_install`, where __path_to_mfem_install__ is the path to the installation of MFEM
*  `source ../config.sh`
*  `make`

If the above process is successful you should see the executables `pumi_2_mfem` and `mfem_2_vtk`. You can test them as follows (assuming you are still in the `build` folder)

1. Running
`./pumi_2_mfem --mesh ../data/1x1_square_mesh.smb --parasolid ../data/1x1_square_nat.x_t`
will convert a SCOREC (.smb) mesh to an MFEM mesh named `MFEMformat.mesh`

2. Running (after completed 1)
`./mfem_2_vtk --mesh ./MFEMformat.mesh --outvtk outmesh.vtk`
will write a vtk mesh using the MFEM mesh generated from last step. The vtk can be looked at using paraview (`paraview outmesh.vtk`)

### Building your Own Executables ###

You can write your own code and use the same build system to build the corresponding executable. For example if you have your source in the file `main.cpp`, you will need to do the following:

1. put the source file `main.cpp` in the root directory of this repository
2. add the following line to the file `CMakeLists.txt`
`setup_exe(my_exec main.cpp)` after the line that says (__executables that do not use simmetrix go here__) if your code does not depend on simmetrix or after the line that says (__executables that do     use simmetrix go here__) if your code does depend on simmetrix.
3. repeat the build process mentioned earlier
4. if successful you should see the executable `my_exec` under your `build` directory
