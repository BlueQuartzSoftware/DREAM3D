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
 * @class FindFeatureClustering FindFeatureClustering.h DREAM3DLib/GenericFilters/FindFeatureClustering.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindFeatureClustering : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindFeatureClustering)
    DREAM3D_STATIC_NEW_MACRO(FindFeatureClustering)
    DREAM3D_TYPE_MACRO_SUPER(FindFeatureClustering, AbstractFilter)

    virtual ~FindFeatureClustering();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    //------ Required Cell Data

    //------ Required Feature Data
                //------ Created Feature Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ClusteringListArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::MorphologicalFilters; }
    virtual const QString getHumanLabel() { return "Find Feature Clustering"; }

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
    FindFeatureClustering();

    void find_clustering();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, Centroids)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, EquivalentDiameters)
    NeighborList<float>* m_ClusteringList;

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);

    FindFeatureClustering(const FindFeatureClustering&); // Copy Constructor Not Implemented
    void operator=(const FindFeatureClustering&); // Operator '=' Not Implemented
};


#endif /* FINDFIELDCLUSTERING_H_ */
