#include "OrientationAnalysisUtilities.h"

#include "EbsdLib/LaueOps/LaueOps.h"

namespace OrientationUtilities
{
Matrix3fR OrientationMatrixToGMatrix(const Orientation<float>& oMatrix)
{
  if(oMatrix.size() != 9)
  {
    throw std::out_of_range("Orientation matrix has invalid size.");
  }
  Matrix3fR g1;
  g1(0, 0) = oMatrix[0];
  g1(0, 1) = oMatrix[1];
  g1(0, 2) = oMatrix[2];
  g1(1, 0) = oMatrix[3];
  g1(1, 1) = oMatrix[4];
  g1(1, 2) = oMatrix[5];
  g1(2, 0) = oMatrix[6];
  g1(2, 1) = oMatrix[7];
  g1(2, 2) = oMatrix[8];
  return g1;
}

Matrix3fR OrientationMatrixToGMatrixTranspose(const Orientation<float>& oMatrix)
{
  if(oMatrix.size() != 9)
  {
    throw std::out_of_range("Orientation matrix has invalid size.");
  }
  Matrix3fR g1t;
  g1t(0, 0) = oMatrix[0];
  g1t(0, 1) = oMatrix[3];
  g1t(0, 2) = oMatrix[6];
  g1t(1, 0) = oMatrix[1];
  g1t(1, 1) = oMatrix[4];
  g1t(1, 2) = oMatrix[7];
  g1t(2, 0) = oMatrix[2];
  g1t(2, 1) = oMatrix[5];
  g1t(2, 2) = oMatrix[8];
  return g1t;
}

std::string CrystalStructureEnumToString(uint32_t crystalStructureType)
{
  if(crystalStructureType < 0 || crystalStructureType > 10)
  {
    return "UnknownCrystalStructure";
  }
  const std::vector<std::string> allLaueNames = LaueOps::GetLaueNames();
  return allLaueNames[crystalStructureType];
}

Matrix3fR EbsdLibMatrixToEigenMatrix(const EbsdLib::Matrix3X3F& ebsdMatrix)
{
  Matrix3fR eigenMatrix;
  eigenMatrix(0, 0) = ebsdMatrix[0];
  eigenMatrix(0, 1) = ebsdMatrix[1];
  eigenMatrix(0, 2) = ebsdMatrix[2];
  eigenMatrix(1, 0) = ebsdMatrix[3];
  eigenMatrix(1, 1) = ebsdMatrix[4];
  eigenMatrix(1, 2) = ebsdMatrix[5];
  eigenMatrix(2, 0) = ebsdMatrix[6];
  eigenMatrix(2, 1) = ebsdMatrix[7];
  eigenMatrix(2, 2) = ebsdMatrix[8];
  return eigenMatrix;
}
} // namespace OrientationUtilities
