/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _H5MatrixStatsData_DELEGATE_H_
#define _H5MatrixStatsData_DELEGATE_H_

#include <string>

#include <hdf5.h>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/StatsData/MatrixStatsData.h"
#include "DREAM3DLib/HDF5/H5StatsDataDelegate.h"

/**
 * @class H5MatrixStatsDataDelegate H5MatrixStatsDataDelegate.h DREAM3DLib/HDF5/H5MatrixStatsDataDelegate.h
 * @brief This class handles reading to and writing from a Statistics file that is
 * HDF5 based.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 22, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT H5MatrixStatsDataDelegate : public H5StatsDataDelegate
{
  public:

    DREAM3D_SHARED_POINTERS(H5MatrixStatsDataDelegate)
    DREAM3D_STATIC_NEW_MACRO(H5MatrixStatsDataDelegate)
    DREAM3D_TYPE_MACRO_SUPER(H5MatrixStatsDataDelegate, H5StatsDataDelegate)
    virtual ~H5MatrixStatsDataDelegate();

    int writeMatrixStatsData(MatrixStatsData* data, hid_t groupId);
    int readMatrixStatsData(MatrixStatsData* data, hid_t groupId);

  protected:
    H5MatrixStatsDataDelegate();

	int writePhaseFraction(MatrixStatsData* data, hid_t groupId);
    int readPhaseFraction(MatrixStatsData* data, hid_t groupId);

  private:
    H5MatrixStatsDataDelegate(const H5MatrixStatsDataDelegate&); // Copy Constructor Not Implemented
    void operator=(const H5MatrixStatsDataDelegate&); // Operator '=' Not Implemented
};

#endif /* _H5MatrixStatsData_DELEGATE_H_ */
