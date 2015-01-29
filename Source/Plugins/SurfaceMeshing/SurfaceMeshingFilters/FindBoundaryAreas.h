/* ============================================================================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FindBoundaryAreas_H_
#define _FindBoundaryAreas_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"


#include "SurfaceMeshing/SurfaceMeshingConstants.h"
/**
 * @class FindBoundaryAreas FindBoundaryAreas.h /FilterCategoryFilters/FindBoundaryAreas.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class FindBoundaryAreas : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindBoundaryAreas)
    DREAM3D_STATIC_NEW_MACRO(FindBoundaryAreas)
    DREAM3D_TYPE_MACRO_SUPER(FindBoundaryAreas, AbstractFilter)

    virtual ~FindBoundaryAreas();

    //required attribute matrix
    DREAM3D_FILTER_PARAMETER(DataArrayPath, FaceFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath FaceFeatureAttributeMatrixPath READ getFaceFeatureAttributeMatrixPath WRITE setFaceFeatureAttributeMatrixPath)

    //required arrays
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshTriangleAreasArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshTriangleAreasArrayPath READ getSurfaceMeshTriangleAreasArrayPath WRITE setSurfaceMeshTriangleAreasArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFeatureFaceIdsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)

    //created array name
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshBoundaryAreasArrayName)
    Q_PROPERTY(QString SurfaceMeshBoundaryAreasArrayName READ getSurfaceMeshBoundaryAreasArrayName WRITE setSurfaceMeshBoundaryAreasArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

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

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FindBoundaryAreas();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(double, SurfaceMeshTriangleAreas)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshBoundaryAreas)

    FindBoundaryAreas(const FindBoundaryAreas&); // Copy Constructor Not Implemented
    void operator=(const FindBoundaryAreas&); // Operator '=' Not Implemented
};

#endif /* FindBoundaryAreas_H_ */


