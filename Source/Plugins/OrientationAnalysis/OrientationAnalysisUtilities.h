#pragma once

#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/Core/Orientation.hpp"

#include <Eigen/Dense>

namespace OrientationUtilities {
    using Matrix3fR = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
    using Matrix3dR = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>;

    template<typename T>
    Eigen::Matrix<T, 3, 3, Eigen::RowMajor> OrientationMatrixToGMatrix(const Orientation<T>& oMatrix) {
        if (oMatrix.size() != 9) {
            throw std::out_of_range("Orientation matrix has invalid size.");
        }
        Eigen::Matrix<T, 3, 3, Eigen::RowMajor> g1;
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

    Matrix3fR OrientationMatrixToGMatrixTranspose(const Orientation<float> &oMatrix);

    std::string CrystalStructureEnumToString(uint32_t crystalStructureType);

    template<typename T>
    Eigen::Matrix<T, 3, 3, Eigen::RowMajor> EbsdLibMatrixToEigenMatrix(const EbsdLib::Matrix3X3<T>& ebsdMatrix) {
        Eigen::Matrix<T, 3, 3, Eigen::RowMajor> eigenMatrix;
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
