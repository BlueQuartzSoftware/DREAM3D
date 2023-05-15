#include "OrientationAnalysisUtilities.h"

#include "EbsdLib/LaueOps/LaueOps.h"

namespace OrientationUtilities {


    Matrix3fR OrientationMatrixToGMatrixTranspose(const Orientation<float> &oMatrix) {
        if (oMatrix.size() != 9) {
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

    std::string CrystalStructureEnumToString(uint32_t crystalStructureType) {
        if (crystalStructureType > 10) {
            return "UnknownCrystalStructure";
        }
        const std::vector<std::string> allLaueNames = LaueOps::GetLaueNames();
        return allLaueNames[crystalStructureType];
    }


} // namespace OrientationUtilities
