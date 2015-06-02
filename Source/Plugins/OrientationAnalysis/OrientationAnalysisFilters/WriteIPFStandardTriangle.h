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


#ifndef _WriteIPFStandardTriangle_H_
#define _WriteIPFStandardTriangle_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The WriteIPFStandardTriangle class. See [Filter documentation](@ref writeipfstandardtriangle) for details.
 */
class WriteIPFStandardTriangle : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(WriteIPFStandardTriangle)
    DREAM3D_STATIC_NEW_MACRO(WriteIPFStandardTriangle)
    DREAM3D_TYPE_MACRO_SUPER(WriteIPFStandardTriangle, AbstractFilter)

    virtual ~WriteIPFStandardTriangle();

    DREAM3D_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    DREAM3D_FILTER_PARAMETER(int, ImageFormat)
    Q_PROPERTY(int ImageFormat READ getImageFormat WRITE setImageFormat)

    DREAM3D_FILTER_PARAMETER(int, ImageSize)
    Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)

    enum ImageFormatType
    {
      TifImageType = 0,
      BmpImageType = 1,
      PngImageType = 2,
      JpgImageType = 3
    };

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName This returns the subgroup within the main group for this filter.
     * @return
     */
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
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

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();


  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    WriteIPFStandardTriangle();

    void dataCheck();

    /**
     * @brief writeImage Writes the generated IPF triangle to a file
     * @param image Image to write
     */
    void writeImage(QImage& image);

    /**
     * @brief generateCubicHighTriangle Generates the standard IPF triangle for a cubic symmetry
     * @return QImage of cubic IPF triangle
     */
    QImage generateCubicHighTriangle();

    /**
     * @brief overlayCubicHighText Overlays the legend of the IPF triangle
     * @param image QImage of cubic IPF triangle
     * @return
     */
    QImage overlayCubicHighText(QImage image);

  private:
    WriteIPFStandardTriangle(const WriteIPFStandardTriangle&); // Copy Constructor Not Implemented
    void operator=(const WriteIPFStandardTriangle&); // Operator '=' Not Implemented
};

#endif /* _WriteIPFStandardTriangle_H_ */
