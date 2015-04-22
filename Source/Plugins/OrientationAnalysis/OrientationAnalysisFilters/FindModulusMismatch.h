/* ============================================================================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FindModulusMismatch_H_
#define _FindModulusMismatch_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"

/**
 * @class FindModulusMismatch FindModulusMismatch.h DREAM3DLib/GenericFilters/FindModulusMismatch.h
 * @brief
 * @author Michael A Groeber (AFRL)
 * @author Joseph C Tucker (UES)
 * @date Jul 28, 2014
 * @version 5.1
 */
class FindModulusMismatch : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindModulusMismatch)
    DREAM3D_STATIC_NEW_MACRO(FindModulusMismatch)
    DREAM3D_TYPE_MACRO_SUPER(FindModulusMismatch, AbstractFilter)

    virtual ~FindModulusMismatch();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ModuliArrayPath)
    Q_PROPERTY(DataArrayPath ModuliArrayPath READ getModuliArrayPath WRITE setModuliArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshDeltaModulusArrayName)
    Q_PROPERTY(QString SurfaceMeshDeltaModulusArrayName READ getSurfaceMeshDeltaModulusArrayName WRITE setSurfaceMeshDeltaModulusArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

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
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FindModulusMismatch();

  private:
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Moduli)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, SurfaceMeshDeltaModulus)

    void dataCheckVoxel();
    void dataCheckSurfaceMesh();

    FindModulusMismatch(const FindModulusMismatch&); // Copy Constructor Not Implemented
    void operator=(const FindModulusMismatch&); // Operator '=' Not Implemented
};

#endif /* FindModulusMismatch_H_ */


