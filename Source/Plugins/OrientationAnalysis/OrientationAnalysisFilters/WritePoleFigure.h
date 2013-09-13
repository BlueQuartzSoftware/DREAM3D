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
#ifndef _WritePoleFigure_H_
#define _WritePoleFigure_H_

#include <QtCore/QString>

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"



/**
 * @class WritePoleFigure WritePoleFigure.h /IOFilters/WritePoleFigure.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT WritePoleFigure : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(WritePoleFigure)
    DREAM3D_STATIC_NEW_MACRO(WritePoleFigure)
    DREAM3D_TYPE_MACRO_SUPER(WritePoleFigure, AbstractFilter)

    virtual ~WritePoleFigure();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GoodVoxelsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)


    DREAM3D_INSTANCE_STRING_PROPERTY(ImagePrefix)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputPath)
    DREAM3D_INSTANCE_PROPERTY(int, ImageFormat)
    DREAM3D_INSTANCE_PROPERTY(int, ImageSize)
    DREAM3D_INSTANCE_PROPERTY(int, LambertSize)
    DREAM3D_INSTANCE_PROPERTY(int, NumColors)


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
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }

    /**
     * @brief getSubGroupName This returns the subgroup within the main group for this filter.
     * @return
     */
    virtual const QString getSubGroupName() { return "Output"; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Write Pole Figure Image"; }

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


  protected:
    WritePoleFigure();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    /**
     * @brief generateCubicPoleFigures
     * @param eulers
     */
    void generateCubicHighPoleFigures(FloatArrayType *eulers, int phaseIndex);
    /**
     * @brief generateHexPoleFigures
     * @param eulers
     */
    void generateHexHighPoleFigures(FloatArrayType *eulers, int phaseIndex);
    /**
     * @brief generateOrthoPoleFigures
     * @param eulers
     */
    void generateOrthorhombicPoleFigures(FloatArrayType *eulers, int phaseIndex);

    QString generateVtkPath( QString label);
    void writeVtkFile(const QString filename,  DoubleArrayType* poleFigurePtr, int dimension);

    QString generateImagePath( QString label);
    void writeImage(const QString outputPath, QImage image, int dimension, QString label);


    QImage createQImageFromRgbaArray(UInt8ArrayType* poleFigurePtr, int imageDimension, QString imageLabel, bool includeOverlay);
    QImage paintPoleFigureOverlay(int imageWidth, int imageHeight, QString label, QImage image);

  private:
    float* m_CellEulerAngles;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;

    WritePoleFigure(const WritePoleFigure&); // Copy Constructor Not Implemented
    void operator=(const WritePoleFigure&); // Operator '=' Not Implemented
};

#endif /* _WritePoleFigure_H_ */
