/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"


#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"

class Bin;




class AIMCOMMON_EXPORT H5ReconStatsWriter
{
  public:
    MXA_SHARED_POINTERS(H5ReconStatsWriter);
    MXA_STATIC_NEW_MACRO(H5ReconStatsWriter);

    static Pointer New(const std::string &filename);

    virtual ~H5ReconStatsWriter();

    MXA_INSTANCE_STRING_PROPERTY(FileName);

    //FIXME: We are NOT writing the Precipitate data?
    int writeAxisOrientationData(Bin* axisodf,
                                 AIM::Reconstruction::CrystalStructure crystruct,
                                 double totalaxes);
    int writeMicroTextureData(double* microbin, size_t nElements, double actualgrains);
    int writeMisorientationBinsData(double* misobins, size_t nElements);
    int writeODFData(AIM::Reconstruction::CrystalStructure crystruct,
                     Bin* eulerodf, double totalvol);
    int writeStatsData(int maxdiameter, int mindiameter,
                       double avglogdiam, double sdlogdiam, double actualgrains,
                       const std::vector<std::vector<int> > &neighborhood,
                       const std::vector<std::vector<double> > &svbovera,
                       const std::vector<std::vector<double> > &svcovera,
                       const std::vector<std::vector<double> > &svcoverb,
                       const std::vector<std::vector<double> > &svschmid,
                       const std::vector<std::vector<double> > &svomega3);

  protected:
    H5ReconStatsWriter();
  private:
    hid_t m_FileId;

    H5ReconStatsWriter(const H5ReconStatsWriter&); // Copy Constructor Not Implemented
    void operator=(const H5ReconStatsWriter&); // Operator '=' Not Implemented
};

#endif /* _H5RECONSTRUCTIONIO_H_ */
