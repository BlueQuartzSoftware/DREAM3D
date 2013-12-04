#ifndef FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_
#define FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @class FindFeatureNeighborCAxisMisalignments FindFeatureNeighborCAxisMisalignments.h DREAM3DLib/GenericFilters/FindFeatureNeighborCAxisMisalignments.h
 * @brief
 * @author Michael A Groeber (AFRL)
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindFeatureNeighborCAxisMisalignments : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindFeatureNeighborCAxisMisalignments)
    DREAM3D_STATIC_NEW_MACRO(FindFeatureNeighborCAxisMisalignments)
    DREAM3D_TYPE_MACRO_SUPER(FindFeatureNeighborCAxisMisalignments, AbstractFilter)

    virtual ~FindFeatureNeighborCAxisMisalignments();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)

    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborListArrayName)
    //------ Created Feature Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CAxisMisalignmentListArrayName)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::CrystallographicFilters; }
    virtual const QString getHumanLabel() { return "Find Feature Neighbor C-Axis Misalignments"; }

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
    virtual void preflight();

  protected:
    FindFeatureNeighborCAxisMisalignments();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    NeighborList<int>* m_NeighborList;
    NeighborList<float>* m_CAxisMisalignmentList;
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgCAxisMisalignments)

    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)

    void dataCheck();

    FindFeatureNeighborCAxisMisalignments(const FindFeatureNeighborCAxisMisalignments&); // Copy Constructor Not Implemented
    void operator=(const FindFeatureNeighborCAxisMisalignments&); // Operator '=' Not Implemented
};

#endif /* FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_ */
