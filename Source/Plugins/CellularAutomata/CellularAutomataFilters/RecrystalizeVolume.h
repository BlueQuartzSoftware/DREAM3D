/*
 * Your License or Copyright Information can go here
 */

#ifndef _RecrystalizeVolume_H_
#define _RecrystalizeVolume_H_

#include <QtCore/QDateTime>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class RecrystalizeVolume RecrystalizeVolume.h CellularAutomata/CellularAutomataFilters/RecrystalizeVolume.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class RecrystalizeVolume : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(RecrystalizeVolume)
    DREAM3D_STATIC_NEW_MACRO(RecrystalizeVolume)
    DREAM3D_TYPE_MACRO_SUPER(RecrystalizeVolume, AbstractFilter)

    virtual ~RecrystalizeVolume();

    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, RecrystallizationTimeArrayName)
    Q_PROPERTY(QString RecrystallizationTimeArrayName READ getRecrystallizationTimeArrayName WRITE setRecrystallizationTimeArrayName)

    DREAM3D_FILTER_PARAMETER(QString, RecrystallizationHistoryArrayName)
    Q_PROPERTY(QString RecrystallizationHistoryArrayName READ getRecrystallizationHistoryArrayName WRITE setRecrystallizationHistoryArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AvramiArrayName)
    Q_PROPERTY(QString AvramiArrayName READ getAvramiArrayName WRITE setAvramiArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    DREAM3D_FILTER_PARAMETER(IntVec3_t, Dimensions)
    Q_PROPERTY(IntVec3_t Dimensions READ getDimensions WRITE setDimensions)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    DREAM3D_FILTER_PARAMETER(float, NucleationRate)
    Q_PROPERTY(float NucleationRate READ getNucleationRate WRITE setNucleationRate)

    DREAM3D_FILTER_PARAMETER(unsigned int, Neighborhood)
    Q_PROPERTY(unsigned int Neighborhood READ getNeighborhood WRITE setNeighborhood)

    /* Place your input parameters here using the DREAM3D macros to declare the Filter Parameters
     * or other instance variables
     */
    //DREAM3D_FILTER_PARAMETER(QString, ImagePrefix)
    /* If you declare a filter parameter above then you MUST create a Q_PROPERTY for that FilterParameter */
    //Q_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)

    /* Here is another example of declaring an integer FilterParameter */
    // DREAM3D_FILTER_PARAMETER(int, ImageSize)
    // Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)



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
    RecrystalizeVolume();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:
    /* Your private class instance variables go here. You can use several preprocessor macros to help
     * make sure you have all the variables defined correctly. Those are "DEFINE_REQUIRED_DATAARRAY_VARIABLE()"
     * and  DEFINE_CREATED_DATAARRAY_VARIABLE() which are defined in DREAM3DGetSetMacros.h
     */
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(uint32_t, RecrystallizationTime)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, RecrystallizationHistory)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Avrami)

    RecrystalizeVolume(const RecrystalizeVolume&); // Copy Constructor Not Implemented
    void operator=(const RecrystalizeVolume&); // Operator '=' Not Implemented
};

#endif /* _RecrystalizeVolume_H_ */
