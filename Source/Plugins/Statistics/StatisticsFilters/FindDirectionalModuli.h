/* ============================================================================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FindDirectionalModuli_H_
#define _FindDirectionalModuli_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FindDirectionalModuli FindDirectionalModuli.h DREAM3DLib/GenericFilters/FindDirectionalModuli.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindDirectionalModuli : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindDirectionalModuli)
    DREAM3D_STATIC_NEW_MACRO(FindDirectionalModuli)
    DREAM3D_TYPE_MACRO_SUPER(FindDirectionalModuli, AbstractFilter)

    virtual ~FindDirectionalModuli();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalCompliancesArrayPath)
    Q_PROPERTY(DataArrayPath CrystalCompliancesArrayPath READ getCrystalCompliancesArrayPath WRITE setCrystalCompliancesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, DirectionalModuliArrayName)
    Q_PROPERTY(QString DirectionalModuliArrayName READ getDirectionalModuliArrayName WRITE setDirectionalModuliArrayName)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, LoadingDirection)
    Q_PROPERTY(FloatVec3_t LoadingDirection READ getLoadingDirection WRITE setLoadingDirection)



    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters
     * @param writer
     * @param index
     * @return
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
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FindDirectionalModuli();

  private:
    std::vector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_CREATED_DATAARRAY_VARIABLE(float, DirectionalModuli)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AvgQuats)

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CrystalCompliances)

    void dataCheck();

    FindDirectionalModuli(const FindDirectionalModuli&); // Copy Constructor Not Implemented
    void operator=(const FindDirectionalModuli&); // Operator '=' Not Implemented
};

#endif /* FindDirectionalModuli_H_ */





