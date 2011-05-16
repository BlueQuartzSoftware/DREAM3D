/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#ifndef _H5RECONSTRUCTIONIO_H_
#define _H5RECONSTRUCTIONIO_H_


#include "hdf5.h"

#include <vector>
#include <string>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"


#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"




/**
 * @class H5ReconStatsWriter H5ReconStatsWriter.h AIM/Common/HDF5/H5ReconStatsWriter.h
 * @brief This class writes Reconstruction Module Statistical data to an HDF5 file
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for USAF Research Laboratory
 * @date Apr 15, 2011
 * @version 1.0
 */
class AIMCOMMON_EXPORT H5ReconStatsWriter
{
  public:
    MXA_SHARED_POINTERS(H5ReconStatsWriter);
    MXA_STATIC_NEW_MACRO(H5ReconStatsWriter);

    static Pointer New(const std::string &filename);

    virtual ~H5ReconStatsWriter();

    MXA_INSTANCE_STRING_PROPERTY(FileName);

    //FIXME: We are NOT writing the Precipitate data?
    int writeAxisOrientationData(int phase, float* axisodf, float totalaxes);

    int writeMicroTextureData(int phase, float* microbin, size_t nElements, float actualgrains);

    int writeMisorientationBinsData(int phase, unsigned long long int* nElements, float* misobins);

    int writeMDFWeights(int phase, uint64_t* nElements, float* angles, float* axes, float* weights);

    int writeODFData(int phase,
                     unsigned long long* dims,
                     float* eulerodf);

    int writeODFWeights(int phase, unsigned long long int *dims,
                                  float* e1, float* e2, float* e3, float* weights, float* spreads);

    std::vector<float> generateBins(int phase, float maxDiameter, float minDiameter, float diameterStep);

    int writeBetaDistribution(int phase, const std::string &hdf5GroupName,
                               std::vector<float> &alpha,
                               std::vector<float> &beta);

    int writeLogNormalDistribution(int phase, const std::string &hdf5GroupName,
                               std::vector<float> &average,
                               std::vector<float> &stdDev );


    int writePowerDistribution(int phase, const std::string &hdf5GroupName,
                               std::vector<float> &alpha,
                               std::vector<float> &k,
                               std::vector<float> &beta);


    int writeDistributionData(int phase, const std::string &disType,
                              const std::string &hdf5GroupName,
                              std::vector<std::string> &columnHeaders,
                              std::vector<std::vector<float> > &colData);

    int writeSizeDistribution(int phase, float maxdiameter, float mindiameter,
                              float binStepSize,
                              float avglogdiam, float sdlogdiam,
                              size_t &numberOfBins);

    int writeVolumeStats(int phase, AIM::Reconstruction::CrystalStructure xtal,
                         AIM::Reconstruction::PhaseType phaseType,
                         float phasefraction, float maxdiameter, float mindiameter, float diamStepSize,
                         float avglogdiam, float sdlogdiam, std::vector<std::vector<float> > &svbovera,
                         std::vector<std::vector<float> > &svcovera, std::vector<std::vector<float> > &svcoverb,
                         std::vector<std::vector<float> > &neighborhoodfit, std::vector<std::vector<float> > &svomega3);

    int writeVolumeStats2D(int phase,
                           AIM::Reconstruction::CrystalStructure xtal,
                           AIM::Reconstruction::PhaseType phaseType,
                           float phasefraction, float maxdiameter, float mindiameter, float diamStepSize,
                         float avglogdiam, float sdlogdiam, std::vector<std::vector<float> > &svbovera,
                         std::vector<std::vector<float> > &neighborhoodfit);
    int writePhaseInformation(int phase,
                              AIM::Reconstruction::PhaseType  pt,
                              AIM::Reconstruction::CrystalStructure xtal,
                              float phasefraction);
  protected:
    H5ReconStatsWriter();
  private:
    hid_t m_FileId;

    H5ReconStatsWriter(const H5ReconStatsWriter&); // Copy Constructor Not Implemented
    void operator=(const H5ReconStatsWriter&); // Operator '=' Not Implemented
};

#endif /* _H5RECONSTRUCTIONIO_H_ */
