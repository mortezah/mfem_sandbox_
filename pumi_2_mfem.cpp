#include "mfem.hpp"
#include <fstream>
#include <iostream>

#ifdef MFEM_USE_SIMMETRIX
#include <SimUtil.h>
#include <gmi_sim.h>
#endif
#include <apfMDS.h>
#include <gmi_null.h>
#include <PCU.h>
#include <apfConvert.h>
#include <gmi_mesh.h>
#include <crv.h>

using namespace std;
using namespace mfem;


int main(int argc, char *argv[])
{
    //initilize mpi
    int num_proc, myId;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

   // 1. Parse command-line options.
   const char *mesh_file = "";
#ifdef MFEM_USE_SIMMETRIX
   const char *model_file = "";
#else
   const char *model_file = "";
#endif

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to use.");
   args.AddOption(&model_file, "-p", "--parasolid",
                  "Parasolid model to use.");   

   args.Parse();
   if (!args.Good())
   {
      args.PrintUsage(cout);
      return 1;
   }
   args.PrintOptions(cout);

   //Read the SCOREC Mesh
   PCU_Comm_Init();
#ifdef MFEM_USE_SIMMETRIX
   //SimUtil_start();
   Sim_readLicenseFile(0);
   gmi_sim_start();
   gmi_register_sim();
#endif
   gmi_register_mesh();

   apf::Mesh2* pumi_mesh;
   pumi_mesh = apf::loadMdsMesh(model_file, mesh_file);


   Mesh *mesh = new PumiMesh(pumi_mesh, 1, 1);

   // 3. Add attributes based on reverse classification
   //Boundary faces
   int dim = mesh->Dimension();
   apf::MeshIterator* itr = pumi_mesh->begin(dim-1);
   apf::MeshEntity* ent ;
   int ent_cnt = 0;
   while ((ent = pumi_mesh->iterate(itr)))
   {
      apf::ModelEntity *me = pumi_mesh->toModel(ent);
      if (pumi_mesh->getModelType(me) == (dim-1))
      {
         //Get tag from model by  reverse classification
         int tag = pumi_mesh->getModelTag(me);
         (mesh->GetBdrElement(ent_cnt))->SetAttribute(tag);
         ent_cnt++;
      }
   }
   pumi_mesh->end(itr);

   //Volume faces
   itr = pumi_mesh->begin(dim);
   ent_cnt = 0;
   while ((ent = pumi_mesh->iterate(itr)))
   {
       apf::ModelEntity *me = pumi_mesh->toModel(ent);
       int tag = pumi_mesh->getModelTag(me);
       mesh->SetAttribute(ent_cnt, tag);
       ent_cnt++;
   }
   pumi_mesh->end(itr);

   //Apply the attributes
   mesh->SetAttributes();

   //Write mesh in MFEM fromat
   ofstream fout("MFEMformat.mesh");
   //ofstream fout("MFEMformat.vtku");
   fout.precision(8);
   mesh->Print(fout);
   //mesh->PrintVTK(fout);

   delete mesh;

   pumi_mesh->destroyNative();
   apf::destroyMesh(pumi_mesh);
   PCU_Comm_Free();

#ifdef MFEM_USE_SIMMETRIX
   gmi_sim_stop();
   Sim_unregisterAllKeys();
   //SimUtil_stop();
#endif

   MPI_Finalize();
   return 0;
}
