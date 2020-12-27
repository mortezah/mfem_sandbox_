#include <mfem.hpp>
#include <fstream>
#include <iostream>

using namespace std;
using namespace mfem;

Mesh* read_mfem_mesh(const char* mesh_file);
GridFunction* read_mfem_solution(const char* solution_file, Mesh* mesh);
void write_mesh_vtk(Mesh* mesh, const char* file_name);

int main(int argc, char *argv[])
{
   // 1. Parse command-line options.
   const char *mesh_file = "";
   const char *sol_file = "";
   const char *outvtk_file = "";

   int res = 1;

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to use.");
   args.AddOption(&res, "-res", "--resolution",
                  "Resolution for vtk writers.");
   args.AddOption(&outvtk_file, "-ov", "--outvtk",
                  "Name of output vtk file (.vkt).");
   args.Parse();
   args.Parse();
   if (!args.Good())
   {
      args.PrintUsage(cout);
      return 1;
   }
   args.PrintOptions(cout);

   MPI_Init(&argc,&argv);

   //    Read the mesh from the given mesh file. We can handle triangular,
   //    quadrilateral, tetrahedral, hexahedral, surface and volume meshes with
   //    the same code.
   Mesh *mesh = read_mfem_mesh(mesh_file);


   write_mesh_vtk(mesh, outvtk_file);

   delete mesh;
   MPI_Finalize();
   return 0;
}


Mesh* read_mfem_mesh(const char* mesh_file)
{
   // read the mesh and solution files
   named_ifgzstream meshin(mesh_file);
   if (!meshin)
   {
     cerr << "Can not open mesh file " << mesh_file << ". Exit.\n";
     exit(1);
   }

   Mesh* mesh = new Mesh(meshin, 1, 0, false);
   return mesh;
}

/* GridFunction* read_mfem_solution(const char* solution_file, Mesh* mesh, Vector& sol) */
GridFunction* read_mfem_solution(const char* solution_file, Mesh* mesh)
{
   ifgzstream* solin = new ifgzstream(solution_file);
   if (!solin)
   {
     cerr << "Can not open solution file " << solution_file << ". Exit.\n";
     exit(1);
   }
   GridFunction* gf = new GridFunction(mesh, *solin);
   return gf;
}

void write_mesh_vtk(Mesh* mesh, const char* file_name)
{
   ofstream ofs;
   ofs.open(file_name, ofstream::out);
   mesh->PrintVTK(ofs, 1);
   ofs.close();
}
