/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the names of any of the DREAM3D Consortium contributors
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _HoughCircles_H_
#define _HoughCircles_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "Plugins/ImageProcessing/ImageProcessingConstants.h"


/**
 * @class HoughCircles HoughCircles.h ImageProcessing/ImageProcessingFilters/HoughCircles.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class HoughCircles : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(HoughCircles)
    DREAM3D_STATIC_NEW_MACRO(HoughCircles)
    DREAM3D_TYPE_MACRO_SUPER(HoughCircles, AbstractFilter)

    virtual ~HoughCircles();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedCellArrayPath)
    Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, NewCellArrayName)
    Q_PROPERTY(QString NewCellArrayName READ getNewCellArrayName WRITE setNewCellArrayName)

    DREAM3D_FILTER_PARAMETER(bool, SaveAsNewArray)
    Q_PROPERTY(bool SaveAsNewArray READ getSaveAsNewArray WRITE setSaveAsNewArray)
    DREAM3D_FILTER_PARAMETER(float, MinRadius)
    Q_PROPERTY(float MinRadius READ getMinRadius WRITE setMinRadius)
    DREAM3D_FILTER_PARAMETER(float, MaxRadius)
    Q_PROPERTY(float MaxRadius READ getMaxRadius WRITE setMaxRadius)
    DREAM3D_FILTER_PARAMETER(int, NumberCircles)
    Q_PROPERTY(int NumberCircles READ getNumberCircles WRITE setNumberCircles)

    /**
     * @brief getCompiledLibraryName Returns the name of the Library that this filter is a part of
     * @return
     */
    virtual const QString getCompiledLibraryName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const QString getSubGroupName();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    * @param index The index that the data should be written to
    * @return Returns the next index for writing
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    * @param index The index to read the information from
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

    /**
     * @brief newFilterInstance Returns a new instance of the filter optionally copying the filter parameters from the
     * current filter to the new instance.
     * @param copyFilterParameters
     * @return
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  signals:
    /**
     * @brief updateFilterParameters This is emitted when the filter requests all the latest Filter Parameters need to be
     * pushed from a user facing control such as the FilterParameter Widget
     * @param filter The filter to push the values into
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged This signal can be emitted when any of the filter parameters are changed internally.
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before the dataCheck() is called. This can change if needed.
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after the dataCheck() is called. Typically. This can change if needed.
     */
    void preflightExecuted();

  protected:
    HoughCircles();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(ImageProcessing::DefaultPixelType, SelectedCellArray)
    DEFINE_CREATED_DATAARRAY_VARIABLE(ImageProcessing::DefaultPixelType, NewCellArray)

    HoughCircles(const HoughCircles&); // Copy Constructor Not Implemented
    void operator=(const HoughCircles&); // Operator '=' Not Implemented
};

#endif /* _HoughCircles_H_ */
