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
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/IGeometry2D.h"
#include "SIMPLib/Geometry/IGeometry3D.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

namespace
{
constexpr float k_Alpha = 0.2;
constexpr float k_GradientConst = 0.000001f;
constexpr size_t k_LoopMax = 1000;
constexpr float k_LearningRate = 0.1;
constexpr size_t k_IterationsMax = 10;
constexpr std::array<float, 10> k_Steps = {-0.11, -0.011, -0.0011, 0.001, 0.01, 0.1, 0.5, 1.0, 2.0, 10.0};
constexpr float k_Pi = 3.14159265359f;
} // namespace

typedef Eigen::Matrix<float, Eigen::Dynamic, 3> MatrixNx3f;
/**
 * @brief The AlignPointCloudsFromPointsList class adjusts a geometry's points based on alignment lists
 *
 */
class AlignPointCloudsFromPointsList
{
  typedef Eigen::Matrix<float, 4, 4> AffineTransfromMatrix;
  typedef Eigen::Matrix<float, 1, 3> Row;

public:
  AlignPointCloudsFromPointsList(SharedVertexList::Pointer pointsToAlign, const MatrixNx3f& movingPointsList, const MatrixNx3f& staticPointsList)
  : m_PointsToAlign(pointsToAlign)
  , m_MovingPointsList(movingPointsList)
  , m_StaticPointsList(staticPointsList)
  {
  }
  virtual ~AlignPointCloudsFromPointsList() = default;

  void generate()
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

    auto affineTransform = findTranslationMatrix(movingPlanarCentroids, staticPlanarCentroids);
    TransformSetOfPoints(m_PointsToAlign, affineTransform);
  }

private:
  SharedVertexList::Pointer m_PointsToAlign;
  const MatrixNx3f& m_MovingPointsList;
  const MatrixNx3f& m_StaticPointsList;

  Eigen::Matrix<float, 1, 3> findPlanarCentroids(const MatrixNx3f& planarPoints) // could be parallelized
  {
    Eigen::Matrix<float, 1, 3> centroidsXYZ;
    for(int col = 0; col < planarPoints.cols(); col++)
    {
      float coordSum = 0.0f;
      for(int row = 0; row < planarPoints.rows(); row++)
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
    std::array<float, 6> optimizationArray = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // [dx, dy, dz, phi1, PHI, phi2]
    std::vector<std::pair<size_t, size_t>> correlationMatrix(staticCentroids.rows(), std::make_pair(0, 0));
    for(size_t i = 0; i < correlationMatrix.size(); i++)
    {
      correlationMatrix[i] = std::make_pair(i, i);
    }

    // declare parameters
    bool notRegisteredFlag = true;
    size_t iteration = 0;
    auto movingBest = movingCentroids;
    std::vector<float> lostListFull((k_IterationsMax + 1));

    // define actual algorithm
    lostListFull[iteration] = LossFunctionWrapper(optimizationArray, staticCentroids, movingCentroids, correlationMatrix);
    do
    {
      optimizationArray = LeastSquaresFit(optimizationArray, staticCentroids, movingCentroids, correlationMatrix);
      movingBest = Optimize(movingCentroids, optimizationArray);
      correlationMatrix = FindClosestPoints(staticCentroids, movingBest, correlationMatrix);
      iteration++;
      lostListFull[iteration] = LossFunctionWrapper(optimizationArray, staticCentroids, movingCentroids, correlationMatrix);
      if(iteration >= k_IterationsMax)
      {
        notRegisteredFlag = false;
      }
    } while(notRegisteredFlag);

    // create final affine transformation Matrix
    auto TransformationMatrix = OptimizeArrayToTMatrix(optimizationArray);

    return TransformationMatrix;
  }

  AffineTransfromMatrix OptimizeArrayToTMatrix(const std::array<float, 6>& optimizationArray)
  {
    // translation
    Eigen::Vector3f translation;
    translation(0, 0) = optimizationArray[0]; // dx
    translation(0, 1) = optimizationArray[1]; // dy
    translation(0, 2) = optimizationArray[2]; // dz

    // rotation matrix
    auto rotationMatrix = EulerAnglesToRotationMatrix(optimizationArray[3], optimizationArray[4], optimizationArray[5]); // phi1, PHI, phi2

    // affine transformation matrix
    return CraftTransformationMatrix(rotationMatrix, translation);
  }

  Eigen::Matrix<float, 3, 3> EulerAnglesToRotationMatrix(const float phi1, const float PHI, const float phi2)
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

  AffineTransfromMatrix CraftTransformationMatrix(const Eigen::Matrix<float, 3, 3>& rotationMatrix, const Eigen::Vector3f& translationVector)
  {
    AffineTransfromMatrix transfromMatrix;
    transfromMatrix.setIdentity(); // bottom row = [0,0,0,1]
    transfromMatrix.block<3, 3>(0, 0) = rotationMatrix;
    transfromMatrix.block<3, 1>(0, 3) = translationVector;
    return transfromMatrix;
  }

  float LossFunctionWrapper(const std::array<float, 6>& optimizationArray, const MatrixNx3f& staticCentroids, const MatrixNx3f& movingCentroids,
                            const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    return LossFunction(staticCentroids, Optimize(movingCentroids, optimizationArray), correlationMatrix);
  }

  float LossFunction(const MatrixNx3f& staticCentroid, const MatrixNx3f& attemptedAlignment, const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    float result = 0.0f;
    std::vector<float> rowSums(correlationMatrix.size());
    for(int row = 0; row < correlationMatrix.size(); row++)
    {
      float sum;
      for(int col = 0; col < staticCentroid.cols(); col++)
      {
        sum += std::powf(staticCentroid(correlationMatrix[row].first, col) - attemptedAlignment(correlationMatrix[row].second, col), 2.0);
      }
      rowSums.push_back(sum);
    }
    for(auto& num : rowSums)
    {
      result += num;
    }
    return result;
  }

  MatrixNx3f Optimize(const MatrixNx3f& movingCentroids, const std::array<float, 6>& optimizationArray)
  {
    return TranformPoints(movingCentroids, OptimizeArrayToTMatrix(optimizationArray));
  }

  MatrixNx3f TranformPoints(const MatrixNx3f& movingCentroids, const AffineTransfromMatrix& TransformationMatrix)
  {
    MatrixNx3f movedCentroids(movingCentroids);
    Eigen::Vector4f coordinateColumn;
    coordinateColumn(3, 0) = 1.0; // insert 1 at bottom of vect
    for(int row = 0; row < movingCentroids.rows(); row++)
    {
      coordinateColumn.block<3, 1>(0, 0) = movingCentroids.row(row).transpose();
      auto resultingCoord = TransformationMatrix * coordinateColumn;
      movedCentroids.block<1, 3>(row, 0) = resultingCoord.block<3, 1>(0, 0).transpose();
    }
    return movedCentroids;
  }

  std::vector<std::pair<size_t, size_t>> FindClosestPoints(const MatrixNx3f& staticCentroids, const MatrixNx3f& movingBest, const std::vector<std::pair<size_t, size_t>>& correlation)
  {
    std::vector<std::pair<size_t, size_t>> newCorrelation;
    auto indexAvailable = std::vector<bool>(staticCentroids.rows(), true);
    for(int movingIndex = 0; movingIndex < movingBest.rows(); movingIndex++)
    {
      double lowestError = std::numeric_limits<double>::max();
      bool foundCandidateCorrelation = false;
      std::pair<size_t, size_t> candidateCorrelation(0, 0);
      for(int staticIndex = 0; staticIndex < staticCentroids.rows(); staticIndex++)
      {
        if(indexAvailable[staticIndex])
        {
          auto squaredDistance = CalculateDistance(movingBest.row(movingIndex), staticCentroids.row(movingIndex));
          auto calculatedError = k_Alpha * squaredDistance;
          if(calculatedError < lowestError)
          {
            lowestError = calculatedError;
            foundCandidateCorrelation = true;
            candidateCorrelation = std::make_pair(staticIndex, movingIndex);
          }
        }
      }
      if(foundCandidateCorrelation)
      {
        indexAvailable[candidateCorrelation.first] = false;
        newCorrelation.push_back(candidateCorrelation);
      }
    }
    if(correlation.size() == 0)
    {
      newCorrelation = correlation;
    }
    return newCorrelation;
  }

  double CalculateDistance(const Row& movingRow, const Row& staticRow)
  { // sqrt(sum(pow(movingRow - staticRow, 2)))
    double sum = 0.0;
    for(int i = 0; i < staticRow.cols(); i++)
    {
      sum += pow(movingRow(i) - staticRow(i), 2);
    }
    return sqrt(sum);
  }

  std::array<float, 6> LeastSquaresFit(std::array<float, 6> oldOptimizationArray, const MatrixNx3f& staticCentroids, const MatrixNx3f& movingCentroids,
                                       const std::vector<std::pair<size_t, size_t>>& correlation)
  {
    auto bestLoss = LossFunctionWrapper(oldOptimizationArray, staticCentroids, movingCentroids, correlation);
    auto bestOptimizationArray = oldOptimizationArray;

    auto workingLoss = std::numeric_limits<float>::max();
    auto workingOptimizationArray = oldOptimizationArray;
    auto thisOptimizationArray = oldOptimizationArray;
    for(size_t iteration = 0; iteration < k_LoopMax; iteration++)
    {
      auto loss = LossFunctionWrapper(workingOptimizationArray, staticCentroids, movingCentroids, correlation) * k_LearningRate;
      auto gradient = CalculateGradient(workingOptimizationArray, staticCentroids, movingCentroids, correlation);
      for(auto& value : gradient)
      {
        value = loss / value;
      }
      auto testOptimizationArray = workingOptimizationArray;
      for(size_t i = 0; i < testOptimizationArray.size(); i++)
      {
        testOptimizationArray[i] = workingOptimizationArray[i] - gradient[i];
      }
      for(const auto& step : k_Steps)
      {
        auto stepOptimizationArray = workingOptimizationArray;
        for(size_t i = 0; i < stepOptimizationArray.size(); i++)
        {
          stepOptimizationArray[i] = step * (testOptimizationArray[i] - workingOptimizationArray[i]);
        }

        auto stepLoss = LossFunctionWrapper(stepOptimizationArray, staticCentroids, movingCentroids, correlation);

        if(stepLoss < workingLoss)
        {
          thisOptimizationArray = stepOptimizationArray;
          workingLoss = stepLoss;
        }
      }
      workingOptimizationArray = thisOptimizationArray;
      if(workingLoss < bestLoss)
      {
        bestOptimizationArray = thisOptimizationArray;
        bestLoss = workingLoss;
      }
      else // gen new guesses
      {
        for(auto& value : workingOptimizationArray) // gen new guesses
        {
          value *= (((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5) * 0.1 + 1.0);
        }
      }
    }

    // update euler angles properly
    for(size_t i = 3; i < 6; i++)
    {
      bestOptimizationArray[i] = std::fmod(bestOptimizationArray[i], static_cast<float>(2.0 * k_Pi));
    }

    return bestOptimizationArray;
  }

  std::array<float, 6> CalculateGradient(const std::array<float, 6>& optimizationArray, const MatrixNx3f& staticCentroid, const MatrixNx3f& attemptedAlignment,
                                         const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    std::array<float, 6> gradient;
    gradient.fill(0.0f);
    for(size_t i = 0; i < optimizationArray.size(); i++)
    {
      auto workingOptimizationArray = optimizationArray;
      workingOptimizationArray[i] += k_GradientConst;
      gradient[i] = LossFunctionWrapper(workingOptimizationArray, staticCentroid, attemptedAlignment, correlationMatrix);
    }
    return gradient;
  }

  void TransformSetOfPoints(SharedVertexList::Pointer pointsToAlign, const AffineTransfromMatrix& TransformationMatrix)
  {
    Eigen::Vector4f coordinateColumn;
    coordinateColumn(3, 0) = 1.0; // insert 1 at bottom of vect
    for(size_t i = 0; i < pointsToAlign->getSize(); i += 3)
    {
      coordinateColumn(0, 0) = pointsToAlign->getValue(i);
      coordinateColumn(1, 0) = pointsToAlign->getValue(i + 1);
      coordinateColumn(2, 0) = pointsToAlign->getValue(i + 2);

      auto resultingCoord = TransformationMatrix * coordinateColumn;

      pointsToAlign->setValue(i, resultingCoord(0, 0));
      pointsToAlign->setValue(i, resultingCoord(1, 0));
      pointsToAlign->setValue(i, resultingCoord(2, 0));
    }
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

  std::vector<std::vector<double>> fixedRows = m_FixedKeyPoints.getTableData();
  std::vector<std::vector<double>> movingRows = m_MovingKeyPoints.getTableData();

  if(movingRows.size() != fixedRows.size())
  {
    QString ss = QObject::tr("Moving Key Points Table and Fixed Key Points Table do not have the same amount of rows");
    setErrorCondition(-44360, ss);
  }

  if(movingRows.size() < 3)
  {
    QString ss =
        QObject::tr("To accurately align objects at least 3 points must be provided. Only %1 points were provided, 5 or more recommended (not all coplanar for best results)").arg(movingRows.size());
    setErrorCondition(-44361, ss);
  }

  MatrixNx3f fixedMatrix, movingMatrix;
  fixedMatrix.resize(fixedRows.size(), Eigen::NoChange);
  movingMatrix.resize(movingRows.size(), Eigen::NoChange);

  Eigen::Vector3f coordinateRow;
  for(size_t rowIndx = 0; rowIndx < fixedRows.size(); rowIndx++)
  {
    auto row = fixedRows[rowIndx];
    for(size_t i = 0; i < row.size(); i++)
    {
      coordinateRow(i, 0) = static_cast<float>(row[i]);
    }
    fixedMatrix.block<1, 3>(rowIndx, 0) = coordinateRow.block<3, 1>(0, 0).transpose();
  }

  for(size_t rowIndx = 0; rowIndx < movingRows.size(); rowIndx++)
  {
    auto row = movingRows[rowIndx];
    for(size_t i = 0; i < row.size(); i++)
    {
      coordinateRow(i, 0) = static_cast<float>(row[i]);
    }
    movingMatrix.block<1, 3>(rowIndx, 0) = coordinateRow.block<3, 1>(0, 0).transpose();
  }
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

  SharedVertexList::Pointer completeMovingPoints;

  auto igeom = getDataContainerArray()->getDataContainer(getMovingGeometry())->getGeometryAs<IGeometry>();

  if(IGeometry2D::Pointer igeom2D = std::dynamic_pointer_cast<IGeometry2D>(igeom))
  {
    completeMovingPoints = igeom2D->getVertices();
  }
  else if(IGeometry3D::Pointer igeom3D = std::dynamic_pointer_cast<IGeometry3D>(igeom))
  {
    completeMovingPoints = igeom3D->getVertices();
  }
  else if(VertexGeom::Pointer vertex = std::dynamic_pointer_cast<VertexGeom>(igeom))
  {
    completeMovingPoints = vertex->getVertices();
  }
  else if(EdgeGeom::Pointer edge = std::dynamic_pointer_cast<EdgeGeom>(igeom))
  {
    completeMovingPoints = edge->getVertices();
  }
  else
  {
    return;
  }

  auto align = AlignPointCloudsFromPointsList(completeMovingPoints, m_MovingKeyPointsMatrix, m_FixedKeyPointsMatrix);

  if(getWarningCode() < 0)
  {
    QString ss = QObject::tr("Warning Flagged in RigidPointCloudTransform Filter");
    setWarningCondition(-44362, ss);
  }

  if(getErrorCode() < 0)
  {
    QString ss = QObject::tr("An error arose from RigidPointCloudTransform Filter");
    setErrorCondition(-44363, ss);
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
  return "Align Geometries from points";
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
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setMovingKeyPointsMatrix(const MatrixNx3f& value)
{
  m_MovingKeyPointsMatrix = value;
}
// -----------------------------------------------------------------------------
MatrixNx3f RigidPointCloudTransform::getMovingKeyPointsMatrix() const
{
  return m_MovingKeyPointsMatrix;
}
// -----------------------------------------------------------------------------
void RigidPointCloudTransform::setFixedKeyPointsMatrix(const MatrixNx3f& value)
{
  m_FixedKeyPointsMatrix = value;
}
// -----------------------------------------------------------------------------
MatrixNx3f RigidPointCloudTransform::getFixedKeyPointsMatrix() const
{
  return m_FixedKeyPointsMatrix;
}
