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

#include "WriteIPFStandardTriangle.h"

#include <QtCore/QDir>


#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"

#include "OrientationLib/LaueOps/LaueOps.h"


#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/HexagonalIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/CubicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/HexagonalLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/CubicLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TriclinicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/MonoclinicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/OrthorhombicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TetragonalLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TetragonalIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TrigonalLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TrigonalIPFLegendPainter.h"

// Include the MOC generated file for this class
#include "moc_WriteIPFStandardTriangle.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::WriteIPFStandardTriangle()
    : AbstractFilter()
    , m_OutputFile("")
    , m_ImageSize(512)
{
    setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::~WriteIPFStandardTriangle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setupFilterParameters()
{
    FilterParameterVector parameters;

    QVector<QString> choices = QVector<QString>::fromStdVector(LaueOps::GetLaueNames());
    choices.pop_back(); // Remove the last name because we don't need it.

    parameters.push_back(SIMPL_NEW_CHOICE_FP("Select Laue Class", LaueClass, FilterParameter::Parameter, WriteIPFStandardTriangle, choices, false));

    parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, WriteIPFStandardTriangle, "*.tif, *.bmp, *.png", "Image"));
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Image Size (Square Pixels)", ImageSize, FilterParameter::Parameter, WriteIPFStandardTriangle));
    setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
    reader->openFilterGroup(this, index);
    setOutputFile(reader->readString("OutputFile", getOutputFile()));
    setImageSize(reader->readValue("ImageSize", getImageSize()));
    reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::dataCheck()
{
    setErrorCondition(0);

    QString ss;
    if(getOutputFile().isEmpty() == true)
    {
        ss = QObject::tr("The output file must be set");
        setErrorCondition(-1);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
    }

    QFileInfo fi(getOutputFile());
    QDir parentPath = fi.path();
    QString ext = fi.completeSuffix();

    if(parentPath.exists() == false)
    {
        ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
        notifyWarningMessage(getHumanLabel(), ss, -1);
    }

    if(ext.isEmpty())
    {
        ss = QObject::tr("The output file does not have an extension");
        setErrorCondition(-1003);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
    }
    else if(ext != "tif" && ext != "bmp" && ext != "png")
    {
        ss = QObject::tr("The output file has an unsupported extension.  Please select a TIF, BMP, or PNG file");
        setErrorCondition(-1004);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
    }

    if(m_ImageSize <= 0)
    {
        setErrorCondition(-1005);
        notifyErrorMessage(getHumanLabel(), "The size of the image must be positive", getErrorCondition());
        return;
    }

    if(m_LaueClass < 0 || m_LaueClass > 10)
    {
        setErrorCondition(-1006);
        notifyErrorMessage(getHumanLabel(), "The Laue Class value must be in the range [0-10]. See documentation for the complete list of values.", getErrorCondition());
        return;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::preflight()
{
    setInPreflight(true);
    emit preflightAboutToExecute();
    emit updateFilterParameters(this);
    dataCheck();
    emit preflightExecuted();
    setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::execute()
{
    setErrorCondition(0);
    dataCheck();
    if(getErrorCondition() < 0)
    {
        return;
    }

    QImage image;

    if(m_LaueClass == 0)
    {
        HexagonalIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 1)
    {
        CubicIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 2)
    {
        HexagonalLowIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 3)
    {
        CubicLowIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 4)
    {
        TriclinicIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 5)
    {
        MonoclinicIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 6)
    {
        OrthorhombicIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 7)
    {
        TetragonalLowIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 8)
    {
        TetragonalIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 9)
    {
        TrigonalLowIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }
    else if(m_LaueClass == 10)
    {
        TrigonalIPFLegendPainter legendPainter;
        image = legendPainter.createLegend(getImageSize(), getImageSize());
    }

    if(!image.isNull()) {
        writeImage(image);
    }

    /* Let the GUI know we are done with this filter */
    notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::writeImage(QImage& image)
{

    QString ss = QObject::tr("Writing Image %1").arg(getOutputFile());
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    QFileInfo fi((m_OutputFile));
    QDir parent(fi.absolutePath());
    if(parent.exists() == false)
    {
        parent.mkpath(fi.absolutePath());
    }

    bool saved = image.save((m_OutputFile));
    if(!saved)
    {
        QString ss = QObject::tr("The Triangle image file '%1' was not saved").arg(getOutputFile());
        setErrorCondition(-90011);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteIPFStandardTriangle::newFilterInstance(bool copyFilterParameters)
{
    WriteIPFStandardTriangle::Pointer filter = WriteIPFStandardTriangle::New();
    if(true == copyFilterParameters)
    {
        copyFilterParameterInstanceVariables(filter.get());
    }
    return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getCompiledLibraryName()
{
    return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getBrandingString()
{
    return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getFilterVersion()
{
    QString version;
    QTextStream vStream(&version);
    vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
    return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getGroupName()
{
    return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getSubGroupName()
{
    return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteIPFStandardTriangle::getHumanLabel()
{
    return "Export IPF Triangle Legend";
}
