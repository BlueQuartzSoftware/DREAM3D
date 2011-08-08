/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// C++ Includes
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <limits>

// AIMRepresentation Includes
//#include <AIM/StatsGenerator/StatsGen.h>
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"
#include "AIM/GrainGenerator/GrainGenerator.h"

#include "CMU/StatsGenerator.h"



int main(int argc, char **argv)
{
  std::cout << "Starting...." << std::endl;
  int err = 0;

  // Not sure where to get these arguments from? Command line maybe?
  double mu = 1.0;
  double sigma = 0.1;
  double sigmaCutOff = 5.0;
  double binStepSize = 0.5;





  std::vector<double> xCo;
  std::vector<double> yCo;
  std::vector<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  std::vector<double> x;
  std::vector<double> y;

  StatsGenerator::Pointer statsGenerator = StatsGenerator::New();
  statsGenerator->setMu(mu);
  statsGenerator->setSigma(sigma);
  statsGenerator->setSigmaCutOff(sigmaCutOff);
  statsGenerator->setBinStepSize(binStepSize);

  err = statsGenerator->computeBinsAndCutOffs(binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return EXIT_FAILURE; }


  // We need to compute the Max and Min Diameter Bin Values
  double mindiameter = xCo[0];
  double maxdiameter = xCo[1];

  double avglogdiam = mu;
  double sdlogdiam = sigma;
  double stepSize = binStepSize;

  size_t nBins = 0;

  /*
   Joe,
      Generate your data for each of the stats groups. The data needs to be stored
    in a std::vector<double> container in order to use our codes. Depending on
    the type of distribution that you want to use there are several methods
    that can be used to write the data to the hdf5 file.
    You might consider writing a class for each type of statistic and have it inherit
    from a base class so that you can make sure you always use the same methods
    to generate and write the data.

   * In order to write to the HDF5 files there are predefined paths that you WILL
   * use when writing the data. Those paths are defined in AIM/Common/Constants.h
   * The are in the 'AIM::HDF5' namespace. So for example if you wanted to write
   * out a beta distribution the you would need to generate your Alpha and Beta
   * data, store it in a std::vector<double> and then pass those vectors to the
   * H5ReconStatsWriter::writeBetaDistribution() method.
   *
   * The std::string constants are:
   * AIM::HDF5::Grain_SizeVOmega3_Distributions
   * AIM::HDF5::Grain_SizeVBoverA_Distributions
   * AIM::HDF5::Grain_SizeVBoverA_Distributions
   * AIM::HDF5::Grain_SizeVBoverA_Distributions
   * AIM::HDF5::Grain_SizeVCoverA_Distributions
   * AIM::HDF5::Grain_SizeVCoverB_Distributions
   * AIM::HDF5::Grain_SizeVNeighbors_Distributions
   */
  std::string hdfFilePath("/tmp/JoesHDF5File.h5");
  H5ReconStatsWriter::Pointer writer = H5ReconStatsWriter::New(hdfFilePath);
  err = writer->writeSizeDistribution(maxdiameter, mindiameter, stepSize, avglogdiam, sdlogdiam, nBins);
  if (err < 0) {
    //TODO: Add something here in case the file could not be written
  }


  {
    std::vector<double> alpha;
    std::vector<double> beta;
    // Now fill the vectors with the correct data

    // Now write the data
    err = writer->writeBetaDistribution(AIM::HDF5::Grain_SizeVOmega3_Distributions, alpha, beta);

  }
  /* To write a Log Normal distribution you need a pair of vectors */
  {
    std::vector<double> average;
    std::vector<double> stdDev;

    err = writer->writeLogNormalDistribution(AIM::HDF5::Grain_SizeVBoverA_Distributions, average, stdDev);
  }

  {
    std::vector<double> alpha;
    std::vector<double> k;
    std::vector<double> beta;
    // Now fill the vectors with the correct data

    // Now write the data
    err = writer->writePowerDistribution(AIM::HDF5::Grain_SizeVBoverA_Distributions, alpha, k, beta);
  }



  /*
   * Next you need to write the ODF data to the file. Again you can generate the
   * data how ever you like and store the data in a std::vector<double>. We have
   * some methods ready to go in the AIM/Common/Texture.h class if you want to
   * use those. We have an enumeration for the 2 crystal structures that we currently
   * use: Ebsd::Hexagonal and Ebsd::Cubic. You WILL
   * use these enumerations as it lets us add more without creating bugs in your codes.
   */

  std::vector<double> odf;
  double totalWeight = 0.0;
  // Generator your ODF data

  // Calculate the 'totalWeight' variable.

  double* odfPtr = &(odf.front());
  err = writer->writeODFData(Ebsd::Cubic, odfPtr, totalWeight);


  // Force the clean up of the writer by assigning a NULL pointer which will
  // have the effect of executing the destructor of the H5ReconStatsWriter Class
  writer = H5ReconStatsWriter::NullPointer();


  /* Now that the HDF5 File is written we can now setup to use the GrainGenerator
   * class to generate your microstructure.
   */
  int gg_NumGrains = 500;
  std::string gg_OutputDir("/tmp/JoesSimulationOutput");
  AIM::SyntheticBuilder::ShapeClass shapeclass = AIM::SyntheticBuilder::Superellipsoid;
  AIM::SyntheticBuilder::Precipitates Precipitates = AIM::SyntheticBuilder::NoPrecipitates;
  double xRes = 0.25;
  double yRes = 0.25;
  double zRes = 0.25;
  double fillingErrorWeight = 1.0;
  double neighborhoodErrorWeight = 1.0;
  double sizeDistErrorWeight = 1.0;
  double fractionPrecipitates = 0.0;

  GrainGenerator::Pointer m_GrainGenerator = GrainGenerator::New();
  m_GrainGenerator->setH5StatsFile(hdfFilePath );
  m_GrainGenerator->setOutputDirectory(gg_OutputDir);
  m_GrainGenerator->setNumGrains(gg_NumGrains);
  m_GrainGenerator->setShapeClass(shapeclass);
  m_GrainGenerator->setPrecipitates(Precipitates);
  m_GrainGenerator->setXResolution(xRes);
  m_GrainGenerator->setYResolution(yRes);
  m_GrainGenerator->setZResolution(zRes);
  m_GrainGenerator->setFillingErrorWeight(fillingErrorWeight);
  m_GrainGenerator->setNeighborhoodErrorWeight(neighborhoodErrorWeight);
  m_GrainGenerator->setSizeDistErrorWeight(sizeDistErrorWeight);
  m_GrainGenerator->setFractionPrecipitates(fractionPrecipitates);
  m_GrainGenerator->setAlreadyFormed(false);

  Ebsd::CrystalStructure crystruct = Ebsd::Cubic;

  m_GrainGenerator->setCrystalStructure(crystruct);

  m_GrainGenerator->compute();
  if (m_GrainGenerator->getErrorCondition() < 0 )
  {
    //TODO: Handle the error condition
  }

  /* The Synthetic Builder should now be complete. Any messages during the process
   * will be printed out to the console. We can probably add a toggle to turn that
   * on or off depending on what you want.
   */




  std::cout << "Finished ....." << std::endl;
  return EXIT_SUCCESS;
}
