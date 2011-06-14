

#include <iostream>


#include "DREAM3D/GrainGenerator/GrainGeneratorFunc.h"
#include "DREAM3D/StructureReaders/DXStructureReader.h"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"



int main(int argc, char **argv)
{

  GrainGeneratorFunc::Pointer r = GrainGeneratorFunc::New();

  DXStructureReader::Pointer reader = DXStructureReader::New();
  reader->setInputFileName(argv[1]);

  std::cout << "Reading DX File...." << std::endl;

  int err = reader->readStructure(r.get());
  if (err < 0)
  {
    std::cout << "Error Reading Structure." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Writing VTK File..." << std::endl;


  VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<GrainGeneratorFunc>(r.get()));
  std::vector<VtkScalarWriter*> scalarsToWrite;
  scalarsToWrite.push_back(w0);


  VTKRectilinearGridFileWriter writer;
  writer.write<GrainGeneratorFunc>("/tmp/vtkfilewriterstest.vtk", r.get(), scalarsToWrite);

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}

