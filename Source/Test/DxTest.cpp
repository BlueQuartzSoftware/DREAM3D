

#include <iostream>


#include "DREAM3D/GrainGenerator/GrainGeneratorFunc.h"
#include "DREAM3D/StructureReaders/DXStructureReader.h"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"



int main(int argc, char **argv)
{

  GrainGeneratorFunc::Pointer m = GrainGeneratorFunc::New();

  DXStructureReader::Pointer reader = DXStructureReader::New();
  reader->setInputFileName(argv[1]);

  std::cout << "Reading DX File...." << std::endl;

  int err = reader->readStructure(m.get());
  if (err < 0)
  {
    std::cout << "Error Reading Structure." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Writing VTK File..." << std::endl;
  VtkMiscFileWriter::Pointer writer = VtkMiscFileWriter::New();
//  err = writer->writeGrainGenRectilinearGrid(m.get(), argv[2], false, false, false);
//  if (err < 0)
//  {
//    std::cout << "Error Writing VTK Structure." << std::endl;
//    return EXIT_FAILURE;
//  }

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}

