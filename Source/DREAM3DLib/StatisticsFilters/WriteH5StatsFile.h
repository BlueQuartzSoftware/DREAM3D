/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef WRITEH5STATSFILE_H_
#define WRITEH5STATSFILE_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/HDF5/H5StatsWriter.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Common/DataContainer.h"

/**
 * @class WriteH5StatsFile WriteH5StatsFile.h DREAM3DLib/GenericFilters/WriteH5StatsFile.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT WriteH5StatsFile : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(WriteH5StatsFile);
    DREAM3D_STATIC_NEW_MACRO(WriteH5StatsFile);
    DREAM3D_TYPE_MACRO_SUPER(WriteH5StatsFile, AbstractFilter);

    virtual ~WriteH5StatsFile();

    DREAM3D_INSTANCE_STRING_PROPERTY(H5StatsFile)
    DREAM3D_INSTANCE_PROPERTY(float, BinStepSize);
    DREAM3D_INSTANCE_PROPERTY(bool, CreateNewStatsFile)

    virtual void preflight();

	 /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    void write_h5statsfile(H5StatsWriter::Pointer h5io, float binstepsize);
    void write_h5statsfile2D(H5StatsWriter::Pointer h5io, float binstepsize);

  protected:
    WriteH5StatsFile();

  private:
    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

	int32_t* m_NumNeighbors;
	int32_t* m_Phases;
	int32_t* m_Neighborhoods;
	bool* m_Active;
	bool* m_UnbiasedFields;
	float* m_EquivalentDiameters;
	float* m_AspectRatios;
	float* m_Schmids;
	float* m_Omega3s;

    WriteH5StatsFile(const WriteH5StatsFile&); // Copy Constructor Not Implemented
    void operator=(const WriteH5StatsFile&); // Operator '=' Not Implemented
};

#endif /* WRITEH5STATSFILE_H_ */
