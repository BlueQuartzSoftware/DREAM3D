/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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


#ifndef _spparkstextreader_h_
#define _spparkstextreader_h_

#include <QtCore/QFile>

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/DataParser.hpp"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

/**
 * @brief The SPParksTextReader class. See [Filter documentation](@ref spparkstextreader) for details.
 */
class SPParksTextReader : public FileReader
{
    Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(SPParksTextReader)
    SIMPL_STATIC_NEW_MACRO(SPParksTextReader)
    SIMPL_TYPE_MACRO_SUPER(SPParksTextReader, FileReader)

    virtual ~SPParksTextReader();

    SIMPL_FILTER_PARAMETER(QString, VolumeDataContainerName)
    Q_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

    SIMPL_FILTER_PARAMETER(bool, OneBasedArrays)
    Q_PROPERTY(bool OneBasedArrays READ getOneBasedArrays WRITE setOneBasedArrays)

    SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    SPParksTextReader();

    /**
     * @brief readHeader Reimplemented from @see FileReader class
     */
    virtual int32_t readHeader();

    /**
     * @brief readFile Reimplemented from @see FileReader class
     */
    virtual int32_t readFile();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief updateCellInstancePointers Updates raw cell pointers
     */
    void updateCellInstancePointers();

    /**
     * @brief getPointerType Returns the enumeration type of the incoming data
     * @param featureName Name of incoming data
     * @return Enumeration type
     */
    Ebsd::NumType getPointerType(const QString& featureName);

    /**
     * @brief getTypeSize Returns the size in bytes of the incoming data
     * @param featureName Name of incoming data
     * @return Integer number of bytes
     */
    int32_t getTypeSize(const QString& featureName);

    /**
     * @brief parseDataLine Reads a line of data from the .dump file
     * @param line Line of data to read
     * @param dims Dimensional offset
     * @param xCol X coordinate
     * @param yCol Y coordinate
     * @param zCol Z coordinate
     */
    void parseDataLine(QByteArray& line, QVector<size_t> dims, int64_t xCol, int64_t yCol, int64_t zCol);

  private:
    QFile m_InStream;
    QMap<QString, DataParser::Pointer> m_NamePointerMap;

    SPParksTextReader(const SPParksTextReader&); // Copy Constructor Not Implemented
    void operator=(const SPParksTextReader&); // Operator '=' Not Implemented
};

#endif /* _SPParksTextReader_H_ */
