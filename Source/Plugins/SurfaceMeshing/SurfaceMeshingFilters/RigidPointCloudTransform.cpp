/*
 * Your License or Copyright can go here
 */

#include "RigidPointCloudTransform.h"

#include <QtCore/QTextStream>

#include <Eigen/Dense>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/**
 * @brief The AlignPointCloudsFromPointsList class adjusts a geometry's points based on alignment lists
 *
 */
class AlignPointCloudsFromPointsList
{
  typedef Eigen::Matrix<float, Eigen::Dynamic, 3> MatrixNx3f;
  typedef Eigen::Matrix<float, 4, 4> AffineTransfromMatrix;

public:
  AlignPointCloudsFromPointsList(SharedVertexList::Pointer pointsToAlign, const MatrixNx3f& movingPointsList, const MatrixNx3f& staticPointsList)
  : m_PointsToAlign(pointsToAlign)
  , m_MovingPointsList(movingPointsList)
  , m_StaticPointsList(staticPointsList)
  {
  }
  virtual ~AlignPointCloudsFromPointsList() = default;

  void generate(size_t start, size_t end)
  {
    // load vector of proper size
    std::vector<size_t> iterate;
    iterate.resize(m_StaticPointsList.rows() - 2);
    std::iota(iterate.begin(), iterate.end(), 0);

    // declare centroid matrices
    MatrixNx3f movingPlanarCentroids;
    MatrixNx3f staticPlanarCentroids;
    movingPlanarCentroids.resize(iterate.size(), Eigen::NoChange);
    staticPlanarCentroids.resize(iterate.size(), Eigen::NoChange);

    // load centroid matrices (could possibly be parallelized)
    for(const auto& i : iterate)
    {
      std::vector rowIndices{i, (i + 1), (i + 2)};
      movingPlanarCentroids.row(i) = findPlanarCentroids(createTrianglePlanebyIndex(m_MovingPointsList, rowIndices));
      staticPlanarCentroids.row(i) = findPlanarCentroids(createTrianglePlanebyIndex(m_StaticPointsList, rowIndices));
    }
  }

private:
  SharedVertexList::Pointer m_PointsToAlign;
  const MatrixNx3f& m_MovingPointsList;
  const MatrixNx3f& m_StaticPointsList;

  Eigen::Matrix<float, 1, 3> findPlanarCentroids(const MatrixNx3f& planarPoints) // could be parallelized
  {
    Eigen::Matrix<float, 1, 3> centroidsXYZ;
    for(size_t col = 0; col < planarPoints.cols(); col++)
    {
      float coordSum = 0.0f;
      for(size_t row = 0; row < planarPoints.rows(); row++)
      {
        coordSum += centroidsXYZ(row, col);
      }
      centroidsXYZ(0, col) = (coordSum / planarPoints.rows());
    }
    return centroidsXYZ;
  }

  MatrixNx3f createTrianglePlanebyIndex(const MatrixNx3f& pointsList, const std::vector<size_t>& rowIndices) // could be parallelized
  {
    MatrixNx3f planeCoords;
    planeCoords.resize(rowIndices.size(), Eigen::NoChange);
    for(size_t i = 0; i < rowIndices.size(); i++)
    {
      planeCoords.row(i) = pointsList.row(rowIndices[i]);
    }
    return planeCoords;
  }

  AffineTransfromMatrix findTranslationMatrix(MatrixNx3f movingCentroids, MatrixNx3f staticCentroids)
  {
    std::array<float, 6> optimizationArray = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // [dx, dy, dz, phi1, PHI, phi2, ?]

    // declare parameters
    // V_S = np.zeros(shape = len(SS_data), dtype = np.float64)
    // V_M = np.zeros(shape = len(CT_data), dtype = np.float64)
    size_t iMax = 1000;
    float learningRate = 0.1;
    bool notRegisteredFlag = true;
    size_t iterCount = 0;
    size_t iterMax = 10;
    float gradientConst = 0.000001f;

    auto movingBest = movingCentroids;
    // define actual algorithm
    do
    {
        optimizationArray = leastSquaresFit(optimizationArray, staticCentroids, movingCentroids, gradientConst, iMax, learningRate);
        movingBest = optimize(movingCentroids, optimizationArray);


      if(iterCount >= iterMax)
      {
        notRegisteredFlag = false;
      }
    } while(notRegisteredFlag);

    // create final affine transformation Matrix
    auto TransformationMatrix = optimizeArrayToTMatrix(optimizationArray);

    return TransformationMatrix;
  }

  AffineTransfromMatrix optimizeArrayToTMatrix(const std::array<float, 6>& optimizationArray)
  {
    // translation
    Eigen::Vector3f translation;
    translation(0, 0) = optimizationArray[0]; // dx
    translation(0, 1) = optimizationArray[1]; // dy
    translation(0, 2) = optimizationArray[2]; // dz

    // rotation matrix
    auto rotationMatrix = eulerAnglesToRotationMatrix(optimizationArray[3], optimizationArray[4], optimizationArray[5]); // phi1, PHI, phi2

    // affine transformation matrix
    return craftTransformationMatrix(rotationMatrix, translation);
  }

  Eigen::Matrix<float, 3, 3> eulerAnglesToRotationMatrix(const float phi1, const float PHI, const float phi2)
  {
    Eigen::Matrix<float, 3, 3> rotationMatrix;

    auto cos1 = std::cosf(phi1);
    auto COS = std::cosf(PHI);
    auto cos2 = std::cosf(phi2);
    auto sin1 = std::sinf(phi1);
    auto SIN = std::sinf(PHI);
    auto sin2 = std::sinf(phi2);

    // compute rotation matrix (ZXZ convention)
    rotationMatrix(0, 0) = (cos1 * cos2 - sin1 * sin2 * COS);
    rotationMatrix(0, 1) = (sin1 * cos2 + cos1 * sin2 * COS);
    rotationMatrix(0, 2) = (sin2 * SIN);
    rotationMatrix(1, 0) = (-cos1 * sin2 - sin1 * cos2 * COS);
    rotationMatrix(1, 1) = (-sin1 * sin2 + cos1 * cos2 * COS);
    rotationMatrix(1, 2) = (cos2 * SIN);
    rotationMatrix(2, 0) = (sin1 * SIN);
    rotationMatrix(2, 1) = (-cos1 * SIN);
    rotationMatrix(2, 2) = (COS);

    return rotationMatrix;
  }

  AffineTransfromMatrix craftTransformationMatrix(const Eigen::Matrix<float, 3, 3>& rotationMatrix, const Eigen::Vector3f& translationVector)
  {
    AffineTransfromMatrix transfromMatrix;
    transfromMatrix.setIdentity(); // bottom row = [0,0,0,1]
    transfromMatrix.block<3, 3>(0, 0) = rotationMatrix;
    transfromMatrix.block<3, 1>(0, 3) = translationVector;
    return transfromMatrix;
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RigidPointCloudTransform::RigidPointCloudTransform()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RigidPointCloudTransform::~RigidPointCloudTransform() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataContainerSelectionFilterParameter::RequirementType dcsReq;
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Geometry To Align", MovingGeometry, FilterParameter::Category::Parameter, RigidPointCloudTransform, dcsReq));
  // Table 1 - Dynamic rows and fixed columns
  {
    QStringList cHeaders;
    cHeaders << "X"
             << "Y"
             << "Z";
    std::vector<std::vector<double>> defaultTable(1, std::vector<double>(3, 0.0));
    m_MovingKeyPoints.setColHeaders(cHeaders);
    m_MovingKeyPoints.setTableData(defaultTable);
    m_MovingKeyPoints.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Key Points from Moving", MovingKeyPoints, FilterParameter::Category::Parameter, RigidPointCloudTransform, false));
  }
  // Table 2 - Dynamic rows and fixed columns
  {
    QStringList cHeaders;
    cHeaders << "X"
             << "Y"
             << "Z";
    std::vector<std::vector<double>> defaultTable(1, std::vector<double>(3, 0.0));
    m_FixedKeyPoints.setColHeaders(cHeaders);
    m_FixedKeyPoints.setTableData(defaultTable);
    m_FixedKeyPoints.setDynamicRows(true);
    parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Key Points from Fixed", FixedKeyPoints, FilterParameter::Category::Parameter, RigidPointCloudTransform, false));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  //IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry>(this, getCellAttributeMatrixPath().getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  if(getWarningCode() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-44360, ss);
  }

  if(getErrorCode() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-44361, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RigidPointCloudTransform::newFilterInstance(bool copyFilterParameters) const
{
  RigidPointCloudTransform::Pointer filter = RigidPointCloudTransform::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getSubGroupName() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getHumanLabel() const
{
  return "RigidPointCloudTransform";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RigidPointCloudTransform::getUuid() const
{
  return QUuid("{b254dd43-6234-5c44-8231-4545cf3845bc}");
}

// -----------------------------------------------------------------------------
RigidPointCloudTransform::Pointer RigidPointCloudTransform::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RigidPointCloudTransform> RigidPointCloudTransform::New()
{
  struct make_shared_enabler : public RigidPointCloudTransform
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::getNameOfClass() const
{
  return QString("RigidPointCloudTransform");
}

// -----------------------------------------------------------------------------
QString RigidPointCloudTransform::ClassName()
{
  return QString("RigidPointCloudTransform");
}

// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setMovingGeometry(const DataArrayPath& value)
{
  m_MovingGeometry = value;
}
// -----------------------------------------------------------------------------
DataArrayPath RigidPointCloudTransform::getMovingGeometry() const
{
  return m_MovingGeometry;
}
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setMovingKeyPoints(const DynamicTableData& value)
{
  m_MovingKeyPoints = value;
}
// -----------------------------------------------------------------------------
DynamicTableData RigidPointCloudTransform::getMovingKeyPoints() const
{
  return m_MovingKeyPoints;
}
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setFixedKeyPoints(const DynamicTableData& value)
{
  m_FixedKeyPoints = value;
}
// -----------------------------------------------------------------------------
DynamicTableData RigidPointCloudTransform::getFixedKeyPoints() const
{
  return m_FixedKeyPoints;
}
