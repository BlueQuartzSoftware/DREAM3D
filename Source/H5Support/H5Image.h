///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MXA_H5IMAGE_H
#define _MXA_H5IMAGE_H

//TODO: Reimplement Indexed image read/write
//TODO: Check other methods to conform to latest H5IM Spec
//TODO: Add ImageSpec Version attribute

//MXA Includes
#include "MXA/MXADLLExport.h"
#include "MXA/MXA.h"

//-- STL headers
#include <string>

//-- HDF5 Header
#include <hdf5.h>

namespace MXA
{
 namespace H5Image
 {
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
    const std::string ImageClass ("CLASS");
    const std::string Image("IMAGE");
    const std::string ImageVersion("IMAGE_VERSION");
    const std::string ImageSubclass("IMAGE_SUBCLASS");
    const std::string ImageIndexed("IMAGE_INDEXED");
    const std::string InterlacePixel("INTERLACE_PIXEL");
    const std::string InterlacePlane("INTERLACE_PLANE");
    const std::string ImageTrueColor("IMAGE_TRUECOLOR");
    const std::string InterlaceMode ("INTERLACE_MODE");
    const std::string Palette("PALETTE");
    const std::string PalVersion("PAL_VERSION");
    const std::string PalColorMap("PAL_COLORMAP");
    const std::string PalType("PAL_TYPE");
    const std::string Standard8("STANDARD8");
    const std::string RGB_COLOR("RGB");
    const std::string PalVersionValue("1.2");
    const std::string ImageGrayScale("IMAGE_GRAYSCALE");
    const std::string ImageWhiteIsZero("IMAGE_WHITE_IS_ZERO");
    const std::string ImageColorModel("IMAGE_COLORMODEL");
    const std::string DisplayOrigin("DISPLAY_ORIGIN");
    const std::string UpperLeft("UL");
    const std::string LowerLeft("LL");
    const std::string UpperRight("UR");
    const std::string LowerRight("LR");
 }
}
/**
* @brief Reimplementation of the H5IM API from the HDF5 High Level API set.
* @date April 2007
* @version $Revision: 1.2 $
*/
class H5Image
{

public:

/**
* @brief Find the attribute const_cast<std::string&>(H5ImageConst::Palette) in the image dataset
* @param loc_id HDF5 File or Group ID
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IM_find_palette( hid_t loc_id);

/**
* @brief Creates and writes an image as an 8 bit image
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @param width The Width of the image in pixels
* @param height The height of the image in pixels
* @param displayOrigin One of UL,UR,LL,LR
* @param buffer Where to store the image data or where to read the image data into
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMmake_image_8bit( hid_t loc_id,
                                              std::string datasetName,
                                              hsize_t width,
                                              hsize_t height,
                                              const std::string &displayOrigin,
                                              const unsigned char *buffer );

/**
* @brief Creates and writes a 24Bit RGB image
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @param width The Width of the image in pixels
* @param height The height of the image in pixels
* @param interlace Interlace mode of the image
* @param buffer Where to store the image data or where to read the image data into
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMmake_image_24bit( hid_t loc_id,
                             std::string datasetName,
                             hsize_t width,
                             hsize_t height,
                             const std::string &interlace,
                             const unsigned char *buffer );

/**
* @brief Gets information about an image dataset (dimensions, interlace mode
 *          and number of associated palettes)
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @param width The Width of the image in pixels
* @param height The height of the image in pixels
* @param planes The Image Planes
* @param interlace Interlace mode of the image
* @param npals Number of palettes
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMget_image_info( hid_t loc_id,
                     std::string datasetName,
                     hsize_t *width,
                     hsize_t *height,
                     hsize_t *planes,
                     std::string &interlace,
                     hssize_t *npals );

/**
* @brief Reads image data from disk
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @param buffer Where to store the image data or where to read the image data into
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMread_image( hid_t loc_id,
                       std::string datasetName,
                       unsigned char *buffer );


/**
* @brief Creates and writes a palette
* @param loc_id HDF5 File or Group ID
* @param pal_name The name of the Palette
* @param pal_dims The dimensions of the Palette
* @param pal_data The Palette Data to write to the file
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMmake_palette( hid_t loc_id,
                         std::string pal_name,
                         const hsize_t *pal_dims,
                         const unsigned char *pal_data );

/**
* @brief This function attaches a palette to an existing image dataset
* @param loc_id HDF5 File or Group ID
* @param imageName The name of the image
* @param pal_name The name of the Palette
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMlink_palette( hid_t loc_id,
                        std::string imageName,
                        std::string pal_name );

/**
* @brief This function dettaches a palette from an existing image dataset
* @param loc_id HDF5 File or Group ID
* @param imageName The name of the image
* @param pal_name THe name of the Palette
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMunlink_palette( hid_t loc_id,
                           std::string imageName,
                           std::string pal_name );

/**
* @brief Gets the number of palettes associated to an image
* @param loc_id HDF5 File or Group ID
* @param imageName The name of the image
* @param npals Number of Palettes
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMget_npalettes( hid_t loc_id,
                          std::string imageName,
                          hssize_t *npals );

/**
* @brief Get palette information
* @param loc_id HDF5 File or Group ID
* @param imageName The name of the image
* @param pal_number The index of the palette
* @param pal_dims The dimensions of the palette
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMget_palette_info( hid_t loc_id,
                        std::string imageName,
                        int32_t pal_number,
                        hsize_t *pal_dims );


/**
* @brief Read palette
* @param loc_id HDF5 File or Group ID
* @param imageName The name of the image
* @param pal_number The index of the palette
* @param pal_data A buffer to store the Palette data into
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMget_palette( hid_t loc_id,
                        std::string imageName,
                        int32_t pal_number,
                        unsigned char *pal_data );

/**
* @brief Is a data set an Image
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMis_image( hid_t loc_id,
                     std::string datasetName );

/**
* @brief Is a dataset a Palette
* @param loc_id HDF5 File or Group ID
* @param datasetName The name of the data set
* @return HDF5 Standard Error Condition
*/
static MXA_EXPORT herr_t H5IMis_palette( hid_t loc_id,
                     std::string datasetName );

/**
 * @brief Retrieves the image dimensions from the mxa file
 * @param fileId The hdf5 fileid of the mxa file
 * @param datasetPath The complete path to the image data set.
 * @param imageDims The dimensions (width, height) of the image
 * @return Error code. Less than Zero (0) is an error.
 */
static MXA_EXPORT herr_t H5IMget_image_dimensions(hid_t fileId,
                             const std::string datasetPath,
                             int imageDims[2]);
};





#endif
