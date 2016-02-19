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


#ifndef _vtkstructuredpointsreader_h_
#define _vtkstructuredpointsreader_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

/**
 * @brief The VtkStructuredPointsReader class. See [Filter documentation](@ref vtkstructuredpointsreader) for details.
 */
class VtkStructuredPointsReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(VtkStructuredPointsReader)
    SIMPL_STATIC_NEW_MACRO(VtkStructuredPointsReader)
    SIMPL_TYPE_MACRO_SUPER(VtkStructuredPointsReader, AbstractFilter)

    virtual ~VtkStructuredPointsReader();

    SIMPL_FILTER_PARAMETER(bool, ReadCellData)
    Q_PROPERTY(bool ReadCellData READ getReadCellData WRITE setReadCellData)

    SIMPL_FILTER_PARAMETER(QString, VolumeDataContainerName)
    Q_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(bool, ReadPointData)
    Q_PROPERTY(bool ReadPointData READ getReadPointData WRITE setReadPointData)

    SIMPL_FILTER_PARAMETER(QString, VertexDataContainerName)
    Q_PROPERTY(QString VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    SIMPL_INSTANCE_STRING_PROPERTY(Comment)
    SIMPL_INSTANCE_STRING_PROPERTY(DatasetType)
    SIMPL_INSTANCE_PROPERTY(bool, FileIsBinary)

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
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

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

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    VtkStructuredPointsReader();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief readHeader Reads the .vtk file header ; CURRENTLY NOT IMPLEMENTED
     * @return Integer error value
     */
    int32_t readHeader();

    /**
     * @brief readFile Handles the main reading of the .vtk file
     * @return Integer error value
     */
    virtual int32_t readFile();

    /**
     * @brief readData Reads a section of data from the .vtk file
     * @param instream Incoming file stream
     */
    virtual void readData(std::istream& instream);

    /**
     * @brief parseCoordinateLine Parses a line representing coordinates
     * @param input Incoming line to parse
     * @param value Coordinate value
     * @return Integer error value
     */
    int32_t parseCoordinateLine(const char* input, size_t& value);

    /**
      * @brief parseByteSize Parses the byte size from a data set declaration line
      * @param text Line to parse
      * @return Integer error value
      */
    size_t parseByteSize(QString text);

    /**
     * @brief readLine Reads a line from the .vtk file
     * @param in Incoming file stream
     * @param result Char pointer to store line
     * @param length Length of line
     * @return Integer error value
     */
    int32_t readLine(std::istream& in, char* result, size_t length);

    /**
     * @brief readString Reas a string from the .vtk file
     * @param in Incoming file stream
     * @param result Char pointer to store string
     * @param length Length of string
     * @return Integer error value
     */
    int32_t readString(std::istream& in, char* result, size_t length);

    /**
     * @brief lowerCase Converts a string to lower case
     * @param str Incoming string to convert
     * @param len Length of string
     * @return Integer error value
     */
    char* lowerCase(char* str, const size_t len);

    /**
     * @brief readDataTypeSection Determines the type of data to be read from the .vtk file
     * @param in Incoming file stream
     * @param numPts Number of points to read
     * @param nextKeyWord Keyword for data type
     * @return Integer error value
     */
    int32_t readDataTypeSection(std::istream& in, int numPts, const std::string& nextKeyWord);

    /**
     * @brief readScalarData Reads scalar data attribute types
     * @param in Incoming file stream
     * @return Integer error value
     */
    int32_t readScalarData(std::istream& in, int numPts);

    /**
     * @brief readVectorData Reads vector data attribute types
     * @param in Incoming file stream
     * @param numPts Number of points
     * @return Integer error value
     */
    int32_t readVectorData(std::istream& in, int numPts);

    /**
     * @brief DecodeString Decodes a binary string from the .vtk file
     * @param resname Resulting decoded string
     * @param name Binary string to decode
     * @return Integer error value
     */
    int32_t DecodeString(char* resname, const char* name);

  private:
    AttributeMatrix::Pointer m_CurrentAttrMat;

    VtkStructuredPointsReader(const VtkStructuredPointsReader&); // Copy Constructor Not Implemented
    void operator=(const VtkStructuredPointsReader&); // Operator '=' Not Implemented
};

#endif /* VtkStructuredPointsReader_H_ */
