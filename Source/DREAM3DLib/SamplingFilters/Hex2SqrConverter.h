/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef Hex2SqrConverter_H_
#define Hex2SqrConverter_H_



#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <vector>
#include <QtCore/QString>




#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/AbstractFilterParametersWriter.h"

/**
 * @class Hex2SqrConverter Hex2SqrConverter.h Hex2SqrConverter/Hex2SqrConverter.h
 * @brief This class is used to convert Hex grid TSL .ang files into Square grid
 * .ang files
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Dr. Michael Groeber, US Air Force Research Laboratories
 * @date July 23, 2012
 * @version 1.2
 *
 */
class DREAM3DLib_EXPORT Hex2SqrConverter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(Hex2SqrConverter)
    DREAM3D_STATIC_NEW_MACRO(Hex2SqrConverter)
    DREAM3D_TYPE_MACRO_SUPER(Hex2SqrConverter, AbstractFilter)

    virtual ~Hex2SqrConverter();

    DREAM3D_INSTANCE_PROPERTY(int64_t, ZStartIndex)
    DREAM3D_INSTANCE_PROPERTY(int64_t, ZEndIndex)
    DREAM3D_INSTANCE_PROPERTY(float, XResolution)
    DREAM3D_INSTANCE_PROPERTY(float, YResolution)
    DREAM3D_INSTANCE_PROPERTY(int, NumCols)
    DREAM3D_INSTANCE_PROPERTY(int, NumRows)
    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, EbsdFileList)

    DREAM3D_INSTANCE_PROPERTY(bool, HeaderIsComplete)

    virtual void preflight();

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SamplingFilters; }
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::ResolutionFilters; }
    virtual const QString getHumanLabel() { return "Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)"; }

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

  protected:
    Hex2SqrConverter();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    /** @brief Modifies a single line of the header section of the TSL .ang file if necessary
    * @param line The line to possibly modify
    */
    QString modifyAngHeaderLine(QString buf);

    Hex2SqrConverter(const Hex2SqrConverter&); // Copy Constructor Not Implemented
    void operator=(const Hex2SqrConverter&); // Operator '=' Not Implemented
};

#endif /* Hex2SqrConverter_H_ */
