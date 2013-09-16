#ifndef FINDBASALLOADINGFACTOR_H_
#define FINDBASALLOADINGFACTOR_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Common/VolumeDataContainer.h"

/**
 * @class FindBasalLoadingFactor FindBasalLoadingFactor.h DREAM3DLib/GenericFilters/FindBasalLoadingFactor.h
 * @brief
 * @author
 * @date Sep 16, 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindBasalLoadingFactor : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindBasalLoadingFactor)
    DREAM3D_STATIC_NEW_MACRO(FindBasalLoadingFactor)
    DREAM3D_TYPE_MACRO_SUPER(FindBasalLoadingFactor, AbstractFilter)

    virtual ~FindBasalLoadingFactor();

    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)

    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(BasalLoadingFactorArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
     virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::CrystallographicFilters; }
    virtual const std::string getHumanLabel() { return "Find Schmid Factors"; }

    DREAM3D_INSTANCE_PROPERTY(FloatVec3Widget_t, LoadingDir)


    virtual void setupFilterParameters();
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
    FindBasalLoadingFactor();

  private:
    std::vector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    float* m_BasalLoadingFactor;
    int32_t* m_FieldPhases;
    float* m_AvgQuats;

    unsigned int* m_CrystalStructures;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    FindBasalLoadingFactor(const FindBasalLoadingFactor&); // Copy Constructor Not Implemented
    void operator=(const FindBasalLoadingFactor&); // Operator '=' Not Implemented
};


#endif /* FINDBASALLOADINGFACTOR_H_ */
