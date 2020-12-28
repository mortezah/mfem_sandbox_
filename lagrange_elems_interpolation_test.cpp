#include <mfem.hpp>
#include <fstream>
#include <iostream>
#include <queue>

#include "LagrangeElements.hpp"

using namespace std;
using namespace mfem;


// for testing
void VField_exact(const Vector &x, Vector &E);

int main(int argc, char *argv[])
{
  int num_procs, myid;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  const char *mfem_mesh_file = "mfem_mesh";
  int vrefine = 1;
  int mrefine = 0;
  int order  = 1;

  OptionsParser args(argc, argv);
  args.AddOption(&mfem_mesh_file, "-m", "--mesh",
      "MFEM Mesh file to use.");
  args.AddOption(&vrefine, "-vr", "--vrefine",
      "Refinement level used for visualization");
  args.AddOption(&mrefine, "-mr", "--mrefine",
      "Refinement level used to refine the mesh after loading it");
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

  // TODO: Read the mesh
  Mesh* mfem_mesh = ;

  for (int i = 0; i < mrefine; i++)
    mfem_mesh->UniformRefinement();


  int dim  = mfem_mesh->Dimension();
  int sdim = mfem_mesh->SpaceDimension();

  // TODO: Construct the Lagrange finite element collection and space
  FiniteElementCollection *fec = ;
  FiniteElementSpace *fes = ;


  // TODO: Create a GridFunction and project the exact field on to it


  // Create a linear (order = 1) Lagrange finite element collection and space
  // for the coordinate field, since the mesh is linear
  FiniteElementCollection *fec_linear = new LG_FECollection(1, dim);
  FiniteElementSpace *fes_linear = new FiniteElementSpace(mfem_mesh, fec_linear, sdim);
  GridFunction coords(fes_linear);
  mfem_mesh->GetNodes(coords);


  // Make an integration point for the center of triangular element
  IntegrationPoint ip;
  ip.Set2(1./3.,1./3.);

  // TODO: Loop over all elements in the mesh and
  // (a) get the physical coordinate of the center of the mesh element (x)
  // (b) get the interpolated field value using the GridFunction (f_interpolated)
  // (c) using (x) get the exact field value (f_exact)
  // (e) compute the error for each element [ elem_error := |f_interpolated - f_exact| ]
  // (f) print the total_error for the mesh [ total_error := sqrt(sum of elem_error^2) / #elems ]
  double total_error = 0.;
  for (int i = 0; i < mfem_mesh->GetNE(); i++) {
    // to be filled
  }
  printf("total interpolation error is %e \n", sqrt(total_error) / mfem_mesh->GetNE());

  /* clean ups */
  delete mfem_mesh;
  delete fes;
  delete fec;
  delete fes_linear;
  delete fec_linear;
  MPI_Finalize();

  return 0;
}

void VField_exact(const Vector &x, Vector &E)
{
  E(0) = 100. * x(0) * x(0);
  E(1) =  50. * x(0) * x(1);
}
