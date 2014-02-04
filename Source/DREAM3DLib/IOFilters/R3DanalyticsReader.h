/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef R3DanalyticsReader_H_
#define R3DanalyticsReader_H_



#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <vector>
#include <string>

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/MXA.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

/**
 * @class R3DanalyticsReader R3DanalyticsReader.h R3DanalyticsReader/R3DanalyticsReader.h
 * @brief This class is used to import multiple R3Danalytics .csv files into an HDF5 file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Dr. Michael Groeber, US Air Force Research Laboratories
 * @author Dr. Joseph C. Tucker, UES
 * @date February 4, 2014
 * @version 1.2
 *
 */
class DREAM3DLib_EXPORT R3DanalyticsReader : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(R3DanalyticsReader)
    DREAM3D_STATIC_NEW_MACRO(R3DanalyticsReader)
    DREAM3D_TYPE_MACRO_SUPER(R3DanalyticsReader, AbstractFilter)

    virtual ~R3DanalyticsReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFile)
    DREAM3D_INSTANCE_PROPERTY(int64_t, ZStartIndex)
    DREAM3D_INSTANCE_PROPERTY(int64_t, ZEndIndex)
    DREAM3D_INSTANCE_PROPERTY(float, ZResolution)
    DREAM3D_INSTANCE_PROPERTY(std::vector<std::string>, EbsdFileList)
    DREAM3D_INSTANCE_PROPERTY(float, SampleTransformationAngle)
    DREAM3D_INSTANCE_PROPERTY(std::vector<float>, SampleTransformationAxis)
    DREAM3D_INSTANCE_PROPERTY(float, EulerTransformationAngle)
    DREAM3D_INSTANCE_PROPERTY(std::vector<float>, EulerTransformationAxis)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, RefFrameZDir)

    virtual void preflight();

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
	virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const std::string getHumanLabel() { return "Read R3Danalytics File(s)"; }

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

  protected:
    R3DanalyticsReader();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:

    R3DanalyticsReader(const R3DanalyticsReader&); // Copy Constructor Not Implemented
    void operator=(const R3DanalyticsReader&); // Operator '=' Not Implemented
};


#endif /* R3DanalyticsReader_H_ */
