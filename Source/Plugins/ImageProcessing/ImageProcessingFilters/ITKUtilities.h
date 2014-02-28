/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ITKUtilities_H_
#define _ITKUtilities_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "Plugins/ImageProcessing/ImageProcessingConstants.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"

/*
 * @class ITKUtilities ITKUtilities.h DREAM3DLib/Plugins/ImageProcessing/ImageProcessingFilters/ITKUtilities.h
 * @brief This class performs conversions between dream3d and ITK formats and other utilities
 * @author Will Lenthe (UCSB)
 * @date Feb 26, 2014
 * @version 1.0
 */

class DREAM3DLib_EXPORT ITKUtilities
{
  public:
    DREAM3D_SHARED_POINTERS(ITKUtilities)
    DREAM3D_TYPE_MACRO(ITKUtilities)

    virtual ~ITKUtilities();

    //conversion from dream3d arrays to itk images
    static ImageProcessing::ImportInt8FilterType::Pointer Dream3DInt8toITK(VoxelDataContainer* m, int8_t* data);
    static ImageProcessing::ImportUInt8FilterType::Pointer Dream3DUInt8toITK(VoxelDataContainer* m, uint8_t* data);
    static ImageProcessing::ImportInt16FilterType::Pointer Dream3DInt16toITK(VoxelDataContainer* m, int16_t* data);
    static ImageProcessing::ImportUInt16FilterType::Pointer Dream3DUInt16toITK(VoxelDataContainer* m, uint16_t* data);
    static ImageProcessing::ImportInt32FilterType::Pointer Dream3DInt32toITK(VoxelDataContainer* m, int32_t* data);
    static ImageProcessing::ImportUInt32FilterType::Pointer Dream3DUInt32toITK(VoxelDataContainer* m, uint32_t* data);
    static ImageProcessing::ImportInt64FilterType::Pointer Dream3DInt64toITK(VoxelDataContainer* m, int64_t* data);
    static ImageProcessing::ImportUInt64FilterType::Pointer Dream3DUInt64toITK(VoxelDataContainer* m, uint64_t* data);
    static ImageProcessing::ImportFloatFilterType::Pointer Dream3DFloattoITK(VoxelDataContainer* m, float* data);
    static ImageProcessing::ImportDoubleFilterType::Pointer Dream3DDoubletoITK(VoxelDataContainer* m, double* data);

    //static ImageProcessing::ImportRGBUint8FilterType::Pointer Dream3DRGBUInt8toITK(VoxelDataContainer* m, uint8_t* data);
    //static ImageProcessing::ImportRGBAFloatFilterType::Pointer Dream3DRGBAFloattoITK(VoxelDataContainer* m, float* data);

    ITKUtilities();

  private:
    ITKUtilities(const ITKUtilities&); // Copy Constructor Not Implemented
    void operator=(const ITKUtilities&); // Operator '=' Not Implemented
};

#endif /* _ITKUtilities_H_ */
