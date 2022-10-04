/*
 * Your License or Copyright can go here
 */

#include "RigidPointCloudTransform.h"

#include <QtCore/QTextStream>

#include <Eigen/Dense>
#include <chrono>
#include <complex>
#include <random>

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

typedef Eigen::Matrix<std::complex<float>, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixNx3cf;
/**
 * @brief The AlignPointCloudsFromPointsList class adjusts a geometry's points based on alignment lists
 *
 */
class AlignPointCloudsFromPointsList
{
  typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> AffineTransfromMatrix;
  typedef Eigen::Matrix<std::complex<float>, 4, 4, Eigen::RowMajor> AffineTransfromMatrixC;
  typedef Eigen::Matrix<float, 1, 3, Eigen::RowMajor> Row;

public:
  AlignPointCloudsFromPointsList(RigidPointCloudTransform* filter, SharedVertexList::Pointer pointsToAlign, const MatrixNx3f& movingPointsList, const MatrixNx3f& staticPointsList)
  : m_PointsToAlign(pointsToAlign)
  , m_MovingPointsList(movingPointsList)
  , m_StaticPointsList(staticPointsList)
  , m_Filter(filter)
  {
  }
  virtual ~AlignPointCloudsFromPointsList() = default;

  void generate()
  {
    // load vector of proper size
    std::vector<size_t> iterate(m_StaticPointsList.rows() - 2);
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
      movingPlanarCentroids.block<1, 3>(i, 0) = findPlanarCentroids(createTrianglePlanebyIndex(m_MovingPointsList, rowIndices));
      staticPlanarCentroids.block<1, 3>(i, 0) = findPlanarCentroids(createTrianglePlanebyIndex(m_StaticPointsList, rowIndices));
    }

    auto affineTransform = findTranslationMatrix(movingPlanarCentroids, staticPlanarCentroids);
    ExportAffineTransformMatrixToDataStructure(affineTransform);
    TransformSetOfPoints(m_PointsToAlign, affineTransform);
  }

private:
  SharedVertexList::Pointer m_PointsToAlign;
  const MatrixNx3f& m_MovingPointsList;
  const MatrixNx3f& m_StaticPointsList;
  RigidPointCloudTransform* m_Filter;

  Eigen::Matrix<float, 1, 3> findPlanarCentroids(const MatrixNx3f& planarPoints) // could be parallelized
  {
    Eigen::Matrix<float, 1, 3> centroidsXYZ;
    for(int col = 0; col < planarPoints.cols(); col++)
    {
      float coordSum = 0.0f;
      for(int row = 0; row < planarPoints.rows(); row++)
      {
        coordSum += planarPoints(row, col);
      }
      centroidsXYZ(0, col) = (coordSum / planarPoints.rows());
    }
    return centroidsXYZ;
  }

  MatrixNx3f createTrianglePlanebyIndex(const MatrixNx3f& pointsList, const std::vector<size_t>& rowIndices) // could be parallelized
  {
    MatrixNx3f planeCoords;
    planeCoords.resize(rowIndices.size(), Eigen::NoChange);
    for(size_t row = 0; row < rowIndices.size(); row++)
    {
      planeCoords.block<1, 3>(row, 0) = pointsList.block<1, 3>(rowIndices[row], 0);
    }
    return planeCoords;
  }

  AffineTransfromMatrix findTranslationMatrix(const MatrixNx3f& movingCentroids, const MatrixNx3f& staticCentroids)
  {
    std::array<std::complex<float>, 6> optimizationArray = {std::complex<float>(11.9389f, 0), std::complex<float>(-1.5032f, 0), std::complex<float>(0.4524f, 0),
                                                            std::complex<float>(3.9960f, 0),  std::complex<float>(0.0744f, 0),  std::complex<float>(0.8539f, 0)}; // [dx, dy, dz, phi1, PHI, phi2]
    std::vector<std::pair<size_t, size_t>> correlationMatrix(staticCentroids.rows(), std::make_pair(0, 0));
    for(size_t i = 0; i < correlationMatrix.size(); i++)
    {
      correlationMatrix[i] = std::make_pair(i, i);
    }

    // pull complex form
    auto complexMovingCentroids = getComplex(movingCentroids);
    auto complexStaticCentroids = getComplex(staticCentroids);

    // declare parameters
    auto movingBest = movingCentroids;

    // define actual algorithm
    for(int iteration = 0; iteration < k_IterationsMax; iteration++)
    {
      m_Filter->notifyStatusMessage(QObject::tr("Generation: %1 of %2").arg(iteration + 1).arg(k_IterationsMax));
      optimizationArray = getComplex(LeastSquaresFit(optimizationArray, complexStaticCentroids, complexMovingCentroids, correlationMatrix));
      movingBest = OptimizeR(movingCentroids, optimizationArray);
      correlationMatrix = FindClosestPoints(staticCentroids, movingBest, correlationMatrix);
    }

    // create final affine transformation Matrix
    auto TransformationMatrix = getReal(OptimizeArrayToTMatrix(optimizationArray));

    return TransformationMatrix;
  }

  template <typename T>
  Eigen::Matrix<T, 4, 4, Eigen::RowMajor> OptimizeArrayToTMatrix(const std::array<T, 6>& optimizationArray)
  {
    // translation
    Eigen::Matrix<T, 1, 3, Eigen::RowMajor> translation;
    translation(0, 0) = optimizationArray[0]; // dx
    translation(0, 1) = optimizationArray[1]; // dy
    translation(0, 2) = optimizationArray[2]; // dz

    // rotation matrix
    Eigen::Matrix<T, 3, 3, Eigen::RowMajor> rotationMatrix = EulerAnglesToRotationMatrix(optimizationArray[3], optimizationArray[4], optimizationArray[5]); // phi1, PHI, phi2

    // affine transformation matrix
    return CraftTransformationMatrix(rotationMatrix, translation);
  }

  template <typename T>
  Eigen::Matrix<T, 3, 3, Eigen::RowMajor> EulerAnglesToRotationMatrix(T phi1, T PHI, T phi2)
  {
    Eigen::Matrix<T, 3, 3, Eigen::RowMajor> rotationMatrix;

    auto cos1 = std::cos(phi1);
    auto COS = std::cos(PHI);
    auto cos2 = std::cos(phi2);
    auto sin1 = std::sin(phi1);
    auto SIN = std::sin(PHI);
    auto sin2 = std::sin(phi2);

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

  template <typename T>
  Eigen::Matrix<T, 4, 4, Eigen::RowMajor> CraftTransformationMatrix(Eigen::Matrix<T, 3, 3, Eigen::RowMajor> rotationMatrix, Eigen::Matrix<T, 1, 3, Eigen::RowMajor> translationVector)
  {
    Eigen::Matrix<T, 4, 4, Eigen::RowMajor> transfromMatrix;
    transfromMatrix.setIdentity(); // bottom row = [0,0,0,1]
    transfromMatrix.block<3, 3>(0, 0) = rotationMatrix;
    transfromMatrix.block<3, 1>(0, 3) = translationVector.transpose();
    return transfromMatrix;
  }

  std::complex<float> LossFunctionWrapper(const std::array<std::complex<float>, 6>& optimizationArray, const MatrixNx3cf& complexStaticCentroids, const MatrixNx3cf& complexMovingCentroids,
                                          const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    return LossFunction(complexStaticCentroids, Optimize(complexMovingCentroids, optimizationArray), correlationMatrix);
  }

  std::complex<float> LossFunction(const MatrixNx3cf& complexStaticCentroid, const MatrixNx3cf& complexAttemptedAlignment, const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    auto result = std::complex<float>(0.0f, 0.0f);
    std::vector<std::complex<float>> rowSums;
    for(int row = 0; row < correlationMatrix.size(); row++)
    {
      auto sum = std::complex<float>(0.0f, 0.0f);
      std::vector<std::complex<float>> sums;
      for(int col = 0; col < complexStaticCentroid.cols(); col++)
      {
        sums.push_back(std::pow(complexStaticCentroid(correlationMatrix[row].first, col) - complexAttemptedAlignment(correlationMatrix[row].second, col), 2.0));
      }
      for(int col = 0; col < complexStaticCentroid.cols(); col++)
      {
        sum += std::pow(complexStaticCentroid(correlationMatrix[row].first, col) - complexAttemptedAlignment(correlationMatrix[row].second, col), 2.0);
      }
      rowSums.push_back(sum);
    }
    for(auto& num : rowSums)
    {
      result += num;
    }
    return result;
  }

  MatrixNx3f OptimizeR(const MatrixNx3f& movingCentroids, const std::array<std::complex<float>, 6>& optimizationArray)
  {
    return TranformPointsR(movingCentroids, OptimizeArrayToTMatrix(optimizationArray));
  }
  MatrixNx3cf Optimize(const MatrixNx3cf& complexMovingCentroids, const std::array<std::complex<float>, 6>& optimizationArray)
  {
    return TranformPoints(complexMovingCentroids, OptimizeArrayToTMatrix(optimizationArray));
  }

  MatrixNx3f TranformPointsR(const MatrixNx3f& movingCentroids, AffineTransfromMatrixC& TransformationMatrix)
  {
    return getReal(TranformPoints(getComplex(movingCentroids), TransformationMatrix));
  }
  MatrixNx3cf TranformPoints(const MatrixNx3cf& movingCentroids, AffineTransfromMatrixC& TransformationMatrix)
  {
    MatrixNx3cf movedCentroids(movingCentroids);
    Eigen::Matrix<std::complex<float>, 4, 1> coordinateColumn;
    coordinateColumn(3, 0) = std::complex<float>(1.0f, 0.0f); // insert 1 at bottom of vect
    for(int row = 0; row < movingCentroids.rows(); row++)
    {
      coordinateColumn.block<3, 1>(0, 0) = movingCentroids.block<1, 3>(row, 0).transpose();
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
            if(squaredDistance < std::numeric_limits<double>::infinity())
            {
              foundCandidateCorrelation = true;
              candidateCorrelation = std::make_pair(staticIndex, movingIndex);
            }
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
      /*for (int i = 0; i < movingBest.rows(); i++)
      {
          newCorrelation.push_back(std::make_pair(0, 0));
      }*/

      newCorrelation = correlation;
    }
    return newCorrelation;
  }

  double CalculateDistance(const Row& movingRow, const Row& staticRow)
  { // sqrt(sum(pow(movingRow - staticRow, 2)))
    auto sum = 0.0;
    for(int i = 0; i < staticRow.cols(); i++)
    {
      sum += pow(movingRow(i) - staticRow(i), 2);
    }
    return sqrt(sum);
  }

  std::array<float, 6> LeastSquaresFit(const std::array<std::complex<float>, 6>& oldOptimizationArray, const MatrixNx3cf& staticCentroids, const MatrixNx3cf& movingCentroids,
                                       const std::vector<std::pair<size_t, size_t>>& correlation)
  {
    auto bestLoss = LossFunctionWrapper(oldOptimizationArray, staticCentroids, movingCentroids, correlation).real();
    auto bestOptimizationArray = getReal(oldOptimizationArray);

    auto workingLoss = std::numeric_limits<float>::max();
    auto workingOptimizationArray = oldOptimizationArray;
    auto thisOptimizationArray = oldOptimizationArray;
    auto start = std::chrono::steady_clock::now();
    for(size_t iteration = 0; iteration < k_LoopMax; iteration++)
    {
      auto now = std::chrono::steady_clock::now();
      if(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > 1000)
      {
        auto percentage = static_cast<int>(100 * (static_cast<float>(iteration) / static_cast<float>(k_LoopMax)));
        m_Filter->notifyStatusMessage(QObject::tr("Iterations complete: %1%").arg(percentage));
        start = std::chrono::steady_clock::now();
      }
      auto loss = LossFunctionWrapper(workingOptimizationArray, staticCentroids, movingCentroids, correlation);
      auto gradient = CalculateGradient(workingOptimizationArray, staticCentroids, movingCentroids, correlation);
      for(auto& value : gradient)
      {
        value = k_LearningRate * loss / value;
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
          stepOptimizationArray[i] += std::complex<float>(step, 0.0) * (testOptimizationArray[i] - workingOptimizationArray[i]);
        }

        auto stepLoss = LossFunctionWrapper(stepOptimizationArray, staticCentroids, movingCentroids, correlation).real();

        if(stepLoss < workingLoss)
        {
          thisOptimizationArray = stepOptimizationArray;
          workingLoss = stepLoss;
        }
      }

      workingOptimizationArray = thisOptimizationArray;
      if(workingLoss < bestLoss)
      {
        bestOptimizationArray = getReal(thisOptimizationArray);
        bestLoss = workingLoss;
      }
      else // gen new guesses
      {
        for(auto& value : workingOptimizationArray) // gen new guesses
        {
          static std::default_random_engine i;
          static std::uniform_real_distribution<> distribution(0, 1);
          value *= std::complex<float>(((distribution(i) - 0.5) * 0.1 + 1.0), 0.0);
        }
      }

      auto workingArray = getReal(workingOptimizationArray);
    }

    // update euler angles properly
    for(size_t i = 3; i < 6; i++)
    {
      bestOptimizationArray[i] = std::fmod(bestOptimizationArray[i], (2.0f * k_Pi));
    }

    return bestOptimizationArray;
  }

  std::array<std::complex<float>, 6> CalculateGradient(const std::array<std::complex<float>, 6>& optimizationArray, const MatrixNx3cf& staticCentroid, const MatrixNx3cf& attemptedAlignment,
                                                       const std::vector<std::pair<size_t, size_t>>& correlationMatrix)
  {
    std::array<std::complex<float>, 6> gradient;
    gradient.fill(0.0f);
    for(size_t i = 0; i < optimizationArray.size(); i++)
    {
      auto workingOptimizationArray = optimizationArray;
      workingOptimizationArray[i] += std::complex<float>(0.0f, 1.0f) * k_GradientConst;
      gradient[i] = LossFunctionWrapper(workingOptimizationArray, staticCentroid, attemptedAlignment, correlationMatrix).imag() / k_GradientConst;
    }
    return gradient;
  }

  void TransformSetOfPoints(SharedVertexList::Pointer pointsToAlign, const AffineTransfromMatrix& TransformationMatrix)
  {
    Eigen::Matrix<float, 4, 1> coordinateColumn;
    coordinateColumn(3, 0) = 1.0f; // insert 1 at bottom of vect
    auto start = std::chrono::steady_clock::now();
    for(size_t i = 0; i < pointsToAlign->getSize(); i += 3)
    {
      auto now = std::chrono::steady_clock::now();
      if(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > 1000)
      {
        auto percentage = static_cast<int>(100 * (static_cast<float>(i) / static_cast<float>(pointsToAlign->getSize())));
        m_Filter->notifyStatusMessage(QObject::tr("Points updated: %1%").arg(percentage));
        start = std::chrono::steady_clock::now();
      }

      coordinateColumn(0, 0) = pointsToAlign->getValue(i);
      coordinateColumn(1, 0) = pointsToAlign->getValue(i + 1);
      coordinateColumn(2, 0) = pointsToAlign->getValue(i + 2);

      auto resultingCoord = TransformationMatrix * coordinateColumn;

      pointsToAlign->setValue(i, resultingCoord(0, 0));
      pointsToAlign->setValue(i + 1, resultingCoord(1, 0));
      pointsToAlign->setValue(i + 2, resultingCoord(2, 0));
    }
  }

  std::array<float, 6> getReal(const std::array<std::complex<float>, 6>& complex)
  {
    std::array<float, 6> reals;
    for(int i = 0; i < reals.size(); i++)
    {
      reals[i] = complex[i].real();
    }
    return reals;
  }
  MatrixNx3f getReal(const MatrixNx3cf& complex)
  {
    MatrixNx3f reals;
    reals.resize(complex.rows(), Eigen::NoChange);
    for(int row = 0; row < reals.rows(); row++)
    {
      for(int col = 0; col < reals.cols(); col++)
      {
        reals(row, col) = complex(row, col).real();
      }
    }
    return reals;
  }
  AffineTransfromMatrix getReal(const AffineTransfromMatrixC& complex)
  {
    AffineTransfromMatrix reals;
    reals.resize(complex.rows(), Eigen::NoChange);
    for(int row = 0; row < reals.rows(); row++)
    {
      for(int col = 0; col < reals.cols(); col++)
      {
        reals(row, col) = complex(row, col).real();
      }
    }
    return reals;
  }
  std::array<std::complex<float>, 6> getComplex(const std::array<float, 6>& reals)
  {
    std::array<std::complex<float>, 6> complexes;
    for(int i = 0; i < complexes.size(); i++)
    {
      complexes[i] = std::complex<float>(reals[i], 0.0);
    }
    return complexes;
  }
  MatrixNx3cf getComplex(const MatrixNx3f& reals)
  {
    MatrixNx3cf complexes;
    complexes.resize(reals.rows(), Eigen::NoChange);
    for(int row = 0; row < complexes.rows(); row++)
    {
      for(int col = 0; col < complexes.cols(); col++)
      {
        complexes(row, col) = std::complex<float>(reals(row, col), 0);
      }
    }
    return reals;
  }
  AffineTransfromMatrixC getComplex(const AffineTransfromMatrix& reals)
  {
    AffineTransfromMatrixC complexes;
    complexes.resize(reals.rows(), Eigen::NoChange);
    for(int row = 0; row < complexes.rows(); row++)
    {
      for(int col = 0; col < complexes.cols(); col++)
      {
        complexes(row, col) = std::complex<float>(reals(row, col), 0);
      }
    }
    return reals;
  }

  void ExportAffineTransformMatrixToDataStructure(const AffineTransfromMatrix& TransformationMatrix)
  {
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

  Eigen::Matrix<float, 1, 3, Eigen::RowMajor> coordinateRow;
  for(size_t rowIndx = 0; rowIndx < fixedRows.size(); rowIndx++)
  {
    auto row = fixedRows[rowIndx];
    for(size_t i = 0; i < row.size(); i++)
    {
      coordinateRow(0, i) = static_cast<float>(row[i]);
    }
    fixedMatrix.block<1, 3>(rowIndx, 0) = coordinateRow.block<1, 3>(0, 0).transpose();
  }

  for(size_t rowIndx = 0; rowIndx < movingRows.size(); rowIndx++)
  {
    auto row = movingRows[rowIndx];
    for(size_t i = 0; i < row.size(); i++)
    {
      coordinateRow(0, i) = static_cast<float>(row[i]);
    }
    movingMatrix.block<1, 3>(rowIndx, 0) = coordinateRow.block<1, 3>(0, 0).transpose();
  }

  setMovingKeyPointsMatrix(movingMatrix);
  setFixedKeyPointsMatrix(fixedMatrix);
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

  AlignPointCloudsFromPointsList(this, completeMovingPoints, getMovingKeyPointsMatrix(), getFixedKeyPointsMatrix()).generate();

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
