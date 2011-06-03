/*
 * VTKFileWritersTest.cpp
 *
 *  Created on: Jun 3, 2011
 *      Author: mjackson
 */

#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3D/Reconstruction/ReconstructionFunc.h"
#include "DREAM3D/MicrostructureStatistics/MicrostructureStatisticsFunc.h"

int main(int argc, char **argv)
{
  VTKFileWriters::Pointer vtkWriter = VTKFileWriters::New();

  MicrostructureStatisticsFunc::Pointer m = MicrostructureStatisticsFunc::New();
  vtkWriter->writeDisorientationFile(m.get(), "out");
  vtkWriter->writeSchmidFactorVizFile(m.get(), "out");

  ReconstructionFunc::Pointer r = ReconstructionFunc::New();
  vtkWriter->setWriteBinaryFiles(true);
  vtkWriter->writeVisualizationFile(r.get(), "out");
  vtkWriter->writeIPFVizFile(r.get(), "out");
  vtkWriter->writeImageQualityVizFile(r.get(), "out");
  vtkWriter->writeDownSampledVizFile(r.get(), "out");

  return 0;
}

