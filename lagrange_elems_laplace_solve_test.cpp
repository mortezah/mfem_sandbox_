#include <mfem.hpp>
#include <fstream>
#include <iostream>
#include <queue>

#include "LagrangeElements.hpp"

using namespace std;
using namespace mfem;



// source function corresponding to heat source/sink at (0.25,0.25) and (0.75, 0.75)
double source_term(const Vector& x)
{
  double res = 0.;
  if ( (x(0) - 0.25)*(x(0) - 0.25) + (x(1) - 0.25)*(x(1) - 0.25) < 0.01)
    res = 1.;
  if ( (x(0) - 0.75)*(x(0) - 0.75) + (x(1) - 0.75)*(x(1) - 0.75) < 0.01)
    res = -1.;
  return res;
}

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
  args.AddOption(&mrefine, "-mr", "--mrefine",
      "Refinement level used to refine mesh after loading it");
  args.AddOption(&vrefine, "-vr", "--vrefine",
      "Refinement level used for visualization");
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

  // TODO: load the mesh
  Mesh* mfem_mesh = ;

  // refine the mesh if needed
  for (int i = 0; i < mrefine; i++)
    mfem_mesh->UniformRefinement();

  int dim  = mfem_mesh->Dimension();
  int sdim = mfem_mesh->SpaceDimension();


  // TODO: create the Lagrange finite element collection and space for a scalar Temperature field
  FiniteElementCollection *fec = ;
  FiniteElementSpace *fes = ;


  // TODO: set Dirichlet boundary condition on all edges and get the essential dofs
  // Hint: see ex1.cpp in mfem/examples/
  Array<int> ess_tdof_list;


  // TODO: set and assemble the linear form (the right-hand-side)
  // Hints:
  //   use FunctionCoefficient to define right-side of equation - del del u = source_term.
  //   use DomainFIntegrator
  //   see ex1.cpp in mfem/examples/
  LinearForm b(fes);

  // define the solution vector and initialize to 0.
  GridFunction x(fes);
  x = 0.;

  // TODO: set and assemble the bilinear form (the left-hand-side)
  // Hints:
  //   use DiffusionIntegrator for the operator -del del ()
  //   use ConstantCoefficient to define the coefficient of - del del () (in this case 1)
  //   see ex1.cpp in mfem/examples/
  BilinearForm a(fes);


  // form the linear system
  OperatorPtr A;
  Vector B, X;
  a.FormLinearSystem(ess_tdof_list, x, b, A, X, B);

  // Solve step
  GSSmoother M((SparseMatrix&)(*A));
  PCG(*A, M, B, X, 1, 200, 1e-12, 0.0);

  // Recover the solution
  a.RecoverFEMSolution(X, b, x);

  // Write to VTK for visualization
  stringstream ss;
  ss << "mesh_field_order_" << order << ".vtk";
  ofstream ofs;
  ofs.open(ss.str().c_str(), ofstream::out);
  mfem_mesh->PrintVTK(ofs, vrefine);
  x.SaveVTK(ofs, "field", vrefine);
  ofs.close();

  /* delete grid_f; */
  delete mfem_mesh;
  delete fes;
  delete fec;
  MPI_Finalize();

  return 0;
}
