/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _H5PrimaryStatsData_DELEGATE_H_
#define _H5PrimaryStatsData_DELEGATE_H_

#include <QtCore/QString>

#include <hdf5.h>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/HDF5/H5StatsDataDelegate.h"

/**
 * @class H5PrimaryStatsDataDelegate H5PrimaryStatsDataDelegate.h DREAM3DLib/HDF5/H5PrimaryStatsDataDelegate.h
 * @brief This class handles reading to and writing from a Statistics file that is
 * HDF5 based.
 *
 * @date Jan 22, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT H5PrimaryStatsDataDelegate : public H5StatsDataDelegate
{
  public:

    SIMPL_SHARED_POINTERS(H5PrimaryStatsDataDelegate)
    SIMPL_STATIC_NEW_MACRO(H5PrimaryStatsDataDelegate)
    SIMPL_TYPE_MACRO_SUPER(H5PrimaryStatsDataDelegate, H5StatsDataDelegate)
    virtual ~H5PrimaryStatsDataDelegate();

    int writePrimaryStatsData(PrimaryStatsData* data, hid_t groupId);
    int readPrimaryStatsData(PrimaryStatsData* data, hid_t groupId);

  protected:
    H5PrimaryStatsDataDelegate();

    int writeBoundaryArea(PrimaryStatsData* data, hid_t groupId);
    int readBoundaryArea(PrimaryStatsData* data, hid_t groupId);

    int writePhaseFraction(PrimaryStatsData* data, hid_t groupId);
    int readPhaseFraction(PrimaryStatsData* data, hid_t groupId);

    int writeFeatureDiameterInfo(PrimaryStatsData* data, hid_t groupId);
    int readFeatureDiameterInfo(PrimaryStatsData* data, hid_t groupId);

    int writeBinNumbers(PrimaryStatsData* data, hid_t groupId);
    int readBinNumbers(PrimaryStatsData* data, hid_t groupId);

    int readMDFWeights(hid_t pid, PrimaryStatsData* data);
    int readODFWeights(hid_t pid, PrimaryStatsData* data);
    int readAxisODFWeights(hid_t pid, PrimaryStatsData* data);

    int writeWeightsData(hid_t pid, const QString& hdf5GroupName,
                         VectorOfFloatArray colData);

    int writeDistributionData(hid_t pid,
                              uint32_t disType,
                              const QString& hdf5GroupName,
                              VectorOfFloatArray colData);

    uint32_t readDistributionType(hid_t pid, const QString& hdf5GroupName);

    int readDistributionData(hid_t pid,
                             const QString& hdf5GroupName,
                             VectorOfFloatArray colData);


    int writeVectorOfArrays(hid_t pid, VectorOfFloatArray colData);
    int readVectorOfArrays(hid_t pid, VectorOfFloatArray colData);

    VectorOfFloatArray createDistributionVector(unsigned int distType);

    VectorOfFloatArray createBetaDistributionArrays();

    VectorOfFloatArray createPowerDistributionArrays();

    VectorOfFloatArray createLogNormalDistributionArrays();

  private:
    H5PrimaryStatsDataDelegate(const H5PrimaryStatsDataDelegate&); // Copy Constructor Not Implemented
    void operator=(const H5PrimaryStatsDataDelegate&); // Operator '=' Not Implemented
};

#endif /* _H5PrimaryStatsData_DELEGATE_H_ */

