#ifndef FINDFIELDCLUSTERING_H_
#define FINDFIELDCLUSTERING_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @class FindFieldClustering FindFieldClustering.h DREAM3DLib/GenericFilters/FindFieldClustering.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindFieldClustering : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindFieldClustering)
    DREAM3D_STATIC_NEW_MACRO(FindFieldClustering)
    DREAM3D_TYPE_MACRO_SUPER(FindFieldClustering, AbstractFilter)

    virtual ~FindFieldClustering();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    //------ Required Cell Data

    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ClusteringListArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::MorphologicalFilters; }
    virtual const QString getHumanLabel() { return "Find Field Clustering"; }

    /**
    * @brief Reimplemented from @see AbstractFilter class
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

    virtual void execute();
    virtual void preflight();

  protected:
    FindFieldClustering();

    void find_clustering();

  private:
    int32_t* m_FieldPhases;
    float* m_Centroids;
    float* m_EquivalentDiameters;
    NeighborList<float>* m_ClusteringList;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    FindFieldClustering(const FindFieldClustering&); // Copy Constructor Not Implemented
    void operator=(const FindFieldClustering&); // Operator '=' Not Implemented
};


#endif /* FINDFIELDCLUSTERING_H_ */
