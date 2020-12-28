#include <mfem.hpp>
#include <fstream>
#include <iostream>
#include <queue>

#include "LagrangeElements.hpp"

using namespace std;
using namespace mfem;


// for testing
void VField_exact(const Vector &x, Vector &E);

Mesh* read_mfem_mesh(const char* mesh_file);

int main(int argc, char *argv[])
{
  int num_procs, myid;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  const char *mfem_mesh_file = "";
  int vrefine = 1;
  int mrefine = 0;
  int order  = 1;

  OptionsParser args(argc, argv);
  args.AddOption(&mfem_mesh_file, "-m", "--mesh",
      "MFEM Mesh file to use.");
  args.AddOption(&vrefine, "-vr", "--vrefine",
      "Refinement level used for visualization");
  args.AddOption(&mrefine, "-mr", "--mrefine",
      "Refinement level used to refine mesh after loading it");
  args.AddOption(&order, "-o", "--order",
      "Order for Lagrange Elements 1 or 2");
  args.Parse();
  if (!args.Good())
  {
    if ( myid == 0)
    {
      args.PrintUsage(cout);
    }
    MPI_Finalize();
    return 1;
  }
  if (myid == 0)
  {
    args.PrintOptions(cout);
  }

  Mesh* mfem_mesh = read_mfem_mesh(mfem_mesh_file);

  for (int i = 0; i < mrefine; i++)
    mfem_mesh->UniformRefinement();

  int dim  = mfem_mesh->Dimension();
  int sdim = mfem_mesh->SpaceDimension();

  FiniteElementCollection *fec = new LG_FECollection(order, dim);
  FiniteElementSpace *fes = new FiniteElementSpace(mfem_mesh, fec, sdim);


  GridFunction gf(fes);
  VectorFunctionCoefficient E(sdim, VField_exact);
  gf.ProjectCoefficient(E);

  stringstream ss;
  ss << "mesh_field_order_" << order << ".vtk";
  ofstream ofs;
  ofs.open(ss.str().c_str(), ofstream::out);
  mfem_mesh->PrintVTK(ofs, vrefine);
  gf.SaveVTK(ofs, "field", vrefine);
  ofs.close();

  /* delete grid_f; */
  delete mfem_mesh;
  delete fes;
  delete fec;
  MPI_Finalize();

  return 0;
}

void VField_exact(const Vector &x, Vector &E)
{
  E(0) = 100. * x(0) * x(0);
  E(1) =  50. * x(0) * x(1);
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
