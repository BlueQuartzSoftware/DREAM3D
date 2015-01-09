/*
 * Your License or Copyright Information can go here
 */

#ifndef _EMMPMFilter_H_
#define _EMMPMFilter_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class EMMPMFilter EMMPMFilter.h EMMPM/EMMPMFilters/EMMPMFilter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class EMMPMFilter : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(EMMPMFilter)
    DREAM3D_STATIC_NEW_MACRO(EMMPMFilter)
    DREAM3D_TYPE_MACRO_SUPER(EMMPMFilter, AbstractFilter)

    virtual ~EMMPMFilter();

    /* Place your input parameters here using the DREAM3D macros to declare the Filter Parameters
     * or other instance variables
     */
    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputDataArrayPath)
    Q_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)

    DREAM3D_FILTER_PARAMETER(int, NumClasses)
    Q_PROPERTY(int NumClasses READ getNumClasses WRITE setNumClasses)

    DREAM3D_FILTER_PARAMETER(float, ExchangeEnergy)
    Q_PROPERTY(float ExchangeEnergy READ getExchangeEnergy WRITE setExchangeEnergy)

    DREAM3D_FILTER_PARAMETER(int, HistogramLoops)
    Q_PROPERTY(int HistogramLoops READ getHistogramLoops WRITE setHistogramLoops)

    DREAM3D_FILTER_PARAMETER(int, SegmentationLoops)
    Q_PROPERTY(int SegmentationLoops READ getSegmentationLoops WRITE setSegmentationLoops)

    DREAM3D_FILTER_PARAMETER(bool, UseSimulatedAnnealing)
    Q_PROPERTY(bool UseSimulatedAnnealing READ getUseSimulatedAnnealing WRITE setUseSimulatedAnnealing)

    DREAM3D_FILTER_PARAMETER(bool, UseGradientPenalty)
    Q_PROPERTY(bool UseGradientPenalty READ getUseGradientPenalty WRITE setUseGradientPenalty)

    DREAM3D_FILTER_PARAMETER(float, GradientPenalty)
    Q_PROPERTY(float GradientPenalty READ getGradientPenalty WRITE setGradientPenalty)

    DREAM3D_FILTER_PARAMETER(bool, UseCurvaturePenalty)
    Q_PROPERTY(bool UseCurvaturePenalty READ getUseCurvaturePenalty WRITE setUseCurvaturePenalty)

    DREAM3D_FILTER_PARAMETER(float, CurvaturePenalty)
    Q_PROPERTY(float CurvaturePenalty READ getCurvaturePenalty WRITE setCurvaturePenalty)

    DREAM3D_FILTER_PARAMETER(float, RMax)
    Q_PROPERTY(float RMax READ getRMax WRITE setRMax)

    DREAM3D_FILTER_PARAMETER(int, EMLoopDelay)
    Q_PROPERTY(int EMLoopDelay READ getEMLoopDelay WRITE setEMLoopDelay)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
    Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

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
    EMMPMFilter();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint8_t, InputImage)
    DEFINE_CREATED_DATAARRAY_VARIABLE(uint8_t, OutputImage)


    EMMPMFilter(const EMMPMFilter&); // Copy Constructor Not Implemented
    void operator=(const EMMPMFilter&); // Operator '=' Not Implemented
};

#endif /* _EMMPMFilter_H_ */
