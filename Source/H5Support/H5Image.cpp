/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* This code has been modified by Mike Jackson at BlueQuartz Software to allow for better    *
*  integration into C++ environments.                                        *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "H5Image.h"

//-- C includes
#include <string.h>
#include <stdlib.h>

//-- STL Includes
#include <string>
#include <list>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


#if 1
/*-------------------------------------------------------------------------
 * Function: find_palette
 *
 * Purpose: operator function used by H5LT_find_palette
 *
 * Return:
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: May 28, 2001
 *
 * Comments:
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static herr_t find_palette( hid_t loc_id, const char *name, const H5A_info_t*, void  *op_data )
{

 /* Define a default zero value for return. This will cause the iterator to continue if
  * the palette attribute is not found yet.
  */

 int32_t ret = 0;

 /* Shut compiler */
 loc_id=loc_id;
 op_data=op_data;

 /* Define a positive value for return value if the attribute was found. This will
  * cause the iterator to immediately return that positive value,
  * indicating short-circuit success
  */

 if( strcmp( name, "PALETTE" ) == 0 )
  ret = 1;


 return ret;
}
#endif

/*-------------------------------------------------------------------------
 * Function: H5IMmake_image_8bit
 *
 * Purpose: Creates and writes an image as an 8 bit image
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: June 13, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t H5Image::H5IMmake_image_8bit( hid_t loc_id,
                            std::string datasetName,
                            hsize_t width,
                            hsize_t height,
                            const std::string &displayOrigin,
                            const unsigned char *buffer )
{
 int32_t    rank = 2;
 hsize_t  dims[2];
 //herr_t err = -1;

  /* Initialize the image dimensions */
 dims[0] = height;
 dims[1] = width;
 //dims[2] = 1;

 /* Make the dataset */
 if (H5Lite::writeDataset(loc_id, datasetName, rank, dims, buffer) < 0 )
  return -1;

 /* Attach the CLASS attribute */
 if ( H5Lite::writeStringAttribute( loc_id, datasetName,
                                    const_cast<std::string&>(MXA::H5Image::ImageClass),
                                    const_cast<std::string&>(MXA::H5Image::Image) ) < 0 )
  return -1;

 /* Attach the VERSION attribute */
 if ( H5Lite::writeScalarAttribute( loc_id, datasetName,
                                    const_cast<std::string&>(MXA::H5Image::ImageWhiteIsZero), 0 ) < 0 )
  return -1;

 /* Attach the IMAGE_SUBCLASS attribute */
 if ( H5Lite::writeStringAttribute( loc_id, datasetName,
                                    const_cast<std::string&>(MXA::H5Image::ImageSubclass),
                                    const_cast<std::string&>(MXA::H5Image::ImageGrayScale) ) < 0 )
  return -1;

 // set the display origin
 //    "UL": (0,0) is at the upper left.
 //    "LL": (0,0) is at the lower left.
 //    "UR": (0,0) is at the upper right.
 //    "LR": (0,0) is at the lower right.
 if ( H5Lite::writeStringAttribute(loc_id,
                                   datasetName,
                                   const_cast<std::string&>(MXA::H5Image::DisplayOrigin),
                                   displayOrigin) < 0 )
   return -1;

 return 0;
}



/*-------------------------------------------------------------------------
 * Function: H5IMmake_image_24bit
 *
 * Purpose:
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: June 13, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Interlace Mode Dimensions in the Dataspace
 * INTERLACE_PIXEL [height][width][pixel components]
 * INTERLACE_PLANE [pixel components][height][width]
 *
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMmake_image_24bit(hid_t loc_id, std::string datasetName,
                                     hsize_t width,
                                     hsize_t height,
                                     const std::string &interlace,
                                     const unsigned char *buffer)
{
  int32_t rank = 3;
  hsize_t dims[3];

  /* Initialize the image dimensions */

  if (interlace == MXA::H5Image::InterlacePixel)
  {
    /* Number of color planes is defined as the third dimension */
    dims[0] = height;
    dims[1] = width;
    dims[2] = 3;
  }
  else if (interlace == MXA::H5Image::InterlacePlane)
  {
    /* Number of color planes is defined as the first dimension */
    dims[0] = 3;
    dims[1] = height;
    dims[2] = width;
  }
  else
  {
    return -1;
  }

  /* Make the dataset */
  if (H5Lite::writeDataset(loc_id, datasetName, rank, dims, buffer) < 0)
    return -1;

  /* Attach the CLASS attribute */
  if (H5Lite::writeStringAttribute(loc_id, datasetName, const_cast<std::string&>(MXA::H5Image::ImageClass), const_cast<std::string&>(MXA::H5Image::Image) ) < 0)
    return -1;

  /* Attach the VERSION attribute */
  if (H5Lite::writeStringAttribute(loc_id, datasetName, const_cast<std::string&>(MXA::H5Image::PalVersion), const_cast<std::string&>(MXA::H5Image::PalVersionValue) )
      < 0)
    return -1;

  /* Attach the IMAGE_SUBCLASS attribute */
  if (H5Lite::writeStringAttribute(loc_id, datasetName, const_cast<std::string&>(MXA::H5Image::ImageSubclass), const_cast<std::string&>(MXA::H5Image::ImageTrueColor) )
      < 0)
    return -1;

  /* Attach the INTERLACE_MODE attribute. This attributes is only for true color images */
  if (H5Lite::writeStringAttribute(loc_id, datasetName, const_cast<std::string&>(MXA::H5Image::InterlaceMode), const_cast<std::string&>(interlace) ) < 0)
    return -1;

  return 0;

}



/*-------------------------------------------------------------------------
 * Function: H5IM_find_palette
 *
 * Purpose: Private function. Find the attribute const_cast<std::string&>(MXA::H5Image::Palette) in the image dataset
 *
 * Return: Success: 1, Failure: 0
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: May 11, 2001
 *
 * Comments:
 *  The function uses H5Aiterate with the operator function find_palette
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IM_find_palette( hid_t loc_id )
{

 hsize_t attr_num;     /* Starting attribute to look up */
 herr_t      ret;

 attr_num = 0;
 ret = H5Aiterate( loc_id, H5_INDEX_NAME, H5_ITER_INC, &attr_num, find_palette, 0 );

 return ret;
}


/*-------------------------------------------------------------------------
 * Function: H5IMget_image_info
 *
 * Purpose: Gets information about an image dataset (dimensions, interlace mode
 *          and number of associated palettes).
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: July 25, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t H5Image::H5IMget_image_info(hid_t loc_id, std::string datasetName, hsize_t *width, hsize_t *height, hsize_t *planes, std::string &interlace, hssize_t *npals)
{
  hid_t did, sid;
  hsize_t dims[3];
  hid_t attr_id;
  hid_t attr_type;
  int32_t hasInterlaceMode;
  hid_t attr_space_id;
  hid_t attr_class;
  int32_t has_pal;
  herr_t retErr = 0;
  herr_t err = 0;

  /*assume initially we have no palettes attached*/
  *npals = 0;

  /* Open the dataset. */
  did = H5Dopen(loc_id, datasetName.c_str(), H5P_DEFAULT  );
  if (did < 0)
  {
    return did;
  }
  /* Try to find the attribute "INTERLACE_MODE" on the >>image<< dataset */
  hasInterlaceMode = H5Lite::findAttribute(did, MXA::H5Image::InterlaceMode);
  interlace.clear();
  char interlaceBuffer[32];
  ::memset(interlaceBuffer, 0, sizeof(interlaceBuffer)); // Zero out the buffer
  /* It exists, get it */
  if (hasInterlaceMode == 1)
  {
    err = H5Lite::readStringAttribute(loc_id, datasetName, MXA::H5Image::InterlaceMode, interlace);
  }
  if (err < 0)
  { retErr = err;}

  /* Get the dataspace handle */
  sid = H5Dget_space(did);
  if (sid >= 0)
  {
    /* Get dimensions */
    err = H5Sget_simple_extent_dims(sid, dims, NULL);
    if ( err >= 0)
    {
      /* Initialize the image dimensions */
      if (hasInterlaceMode == 1)
      /* This is a 24 bit image */
      {
        if (MXA::H5Image::InterlacePixel.compare(interlace) == 0)
        {
          /* Number of color planes is defined as the third dimension */
          *height = dims[0];
          *width = dims[1];
          *planes = dims[2];
        }
        else if (MXA::H5Image::InterlacePlane.compare(interlace) == 0)
        {
          /* Number of color planes is defined as the first dimension */
          *planes = dims[0];
          *height = dims[1];
          *width = dims[2];
        }
        else
        {
          retErr = err;
        }
      }
      else
      /* This is a 8 bit image */
      {
        *height = dims[0];
        *width = dims[1];
        *planes = dims[2];
      }
    }
    else
    {
      retErr = err;
    }
    /* Close */
    err = H5Sclose(sid);
    if (err < 0)
    {
      retErr = err;
    }
  }
  /* Get number of palettes */
  /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
  has_pal = H5IM_find_palette( did );

  if ( has_pal == 1 )
  {
    attr_id = H5Aopen_by_name( loc_id, datasetName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT );
    if ( attr_id >= 0 )
    {
      attr_type = H5Aget_type( attr_id );
      if ( attr_type >= 0 )
      {
        attr_class = H5Tget_class( attr_type );
        if ( attr_class == H5T_REFERENCE )
        {
          /* Get the reference(s) */
          attr_space_id = H5Aget_space( attr_id );
          if ( attr_space_id < 0 )
          {
            *npals = H5Sget_simple_extent_npoints( attr_space_id );
            err = H5Sclose( attr_space_id );
            if ( err < 0 )
            { retErr = err;}
          }

        } /* H5T_REFERENCE */

        err = H5Tclose( attr_type );
        if ( err < 0 )
        { retErr = err;}
      }
      /* Close the attribute. */
      err = H5Aclose( attr_id );
      if ( err < 0 )
      { retErr = err;}
    }

  }

  /* End access to the dataset and release resources used by it. */
  err = H5Dclose( did );
  if (err < 0)
  { retErr = err;}
  return retErr;
}


/*-------------------------------------------------------------------------
 * Function: H5IMread_image
 *
 * Purpose: Reads image data from disk.
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: June 13, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMread_image( hid_t loc_id,
                       std::string datasetName,
                       unsigned char *buffer )
{
 hid_t   did;

 /* Open the dataset. */
 if ( (did = H5Dopen( loc_id, datasetName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Read */
 if ( H5Dread( did, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer ) < 0 )
  goto out;

 /* End access to the dataset and release resources used by it. */
 if ( H5Dclose( did ) )
  return -1;

 return 0;

out:
 H5Dclose( did );
 return -1;

}


/*-------------------------------------------------------------------------
 * Function: H5IMmake_palette
 *
 * Purpose: Creates and writes a palette.
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: May 01, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMmake_palette( hid_t loc_id,
                         std::string pal_name,
                         const hsize_t *pal_dims,
                         const unsigned char *pal_data )

{

 int32_t has_pal = 0;

 /* Check if the dataset already exists */
 has_pal = H5Lite::findDataset( loc_id, pal_name );


 /* It exists. Return */
 if ( has_pal == 1 )
  return 0;

 /* Make the palette dataset. */
 hid_t rank = 2;
 if ( H5Lite::writeDataset(loc_id, pal_name, rank, const_cast<hsize_t*>(pal_dims),  pal_data ) <  0 )
  return -1;

 /* Attach the attribute const_cast<std::string&>(MXA::H5Image::ImageClass) to the >>palette<< dataset*/
 if ( H5Lite::writeStringAttribute( loc_id, pal_name, const_cast<std::string&>(MXA::H5Image::ImageClass), const_cast<std::string&>(MXA::H5Image::Palette) ) < 0 )
  return -1;

 /* Attach the attribute const_cast<std::string&>(MXA::H5Image::PalVersion) to the >>palette<< dataset*/
 if ( H5Lite::writeStringAttribute( loc_id, pal_name, const_cast<std::string&>(MXA::H5Image::PalVersion), const_cast<std::string&>(MXA::H5Image::PalVersionValue) ) < 0 )
  return -1;

 return 0;

}


/*-------------------------------------------------------------------------
 * Function: H5IMlink_palette
 *
 * Purpose: This function attaches a palette to an existing image dataset
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: May 01, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 *  An image (dataset) within an HDF5 file may optionally specify an array of
 *  palettes to be viewed with. The dataset will have an attribute
 *  which contains an array of object reference pointers which refer to palettes in the file.
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMlink_palette( hid_t loc_id,
                         std::string imageName,
                         std::string pal_name )

{

 hid_t       image_id;
 hid_t       attr_type;
 hid_t       attr_id;
 hid_t       attr_space_id;
 hid_t       attr_class;
 hobj_ref_t  ref;         /* write a new reference */
 hobj_ref_t  *refbuf;     /* buffer to read references */
 hssize_t    n_refs;
 hsize_t     dim_ref;
 int32_t         ok_pal;

 /* The image dataset may or not have the attribute const_cast<std::string&>(MXA::H5Image::Palette)
  * First we try to open to see if it is already there; if not, it is created.
  * If it exists, the array of references is extended to hold the reference
  * to the new palette
  */

 /* First we get the image id */
 if ( (image_id = H5Dopen( loc_id, imageName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
 ok_pal = H5Lite::findAttribute( image_id, MXA::H5Image::Palette.c_str() );

 /* It does not exist. We create the attribute and one reference */
 if ( ok_pal == 0 )
 {

  if ( (attr_space_id = H5Screate( H5S_SCALAR )) < 0 )
   goto out;

  /* Create the attribute type for the reference */
  if ( (attr_type = H5Tcopy( H5T_STD_REF_OBJ )) < 0 )
   goto out;

  /* Create the attribute const_cast<std::string&>(MXA::H5Image::Palette) to be attached to the image*/
  if ( (attr_id = H5Acreate( image_id, MXA::H5Image::Palette.c_str(), attr_type, attr_space_id, H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  /* Create a reference. The reference is created on the local id.  */
  if ( H5Rcreate( &ref, loc_id, pal_name.c_str(), H5R_OBJECT, -1 ) < 0 )
   goto out;

  /* Write the attribute with the reference */
  if ( H5Awrite( attr_id, attr_type, &ref ) < 0 )
   goto out;

  if ( H5Sclose( attr_space_id ) < 0 )
   goto out;

 }

 /* The attribute already exists, open it */

 else if ( ok_pal ==  1 )

 {

  if ( (attr_id = H5Aopen_by_name( loc_id, imageName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( (attr_class = H5Tget_class( attr_type )) < 0 )
   goto out;

  /* Check if it is really a reference */

  if ( attr_class == H5T_REFERENCE )
  {

   /* Get and save the old reference(s) */

   if ( (attr_space_id = H5Aget_space( attr_id )) < 0 )
    goto out;

   n_refs = H5Sget_simple_extent_npoints( attr_space_id );

   dim_ref = n_refs + 1;

   refbuf = static_cast<hobj_ref_t*>(malloc( sizeof(hobj_ref_t) * (int)dim_ref ) );

   if ( H5Aread( attr_id, attr_type, refbuf ) < 0 )
    goto out;

   if ( H5Sclose( attr_space_id ) < 0 )
    goto out;

   /* The attribute must be deleted, in order to the new one can reflect the changes*/
   if ( H5Adelete( image_id, MXA::H5Image::Palette.c_str() ) < 0 )
    goto out;

   /* Create a new reference for this palette. */
   if ( H5Rcreate( &ref, loc_id, pal_name.c_str(), H5R_OBJECT, -1 ) < 0 )
    goto out;

   refbuf[n_refs] = ref;

   /* Create the data space for the new references */
   if ( (attr_space_id = H5Screate_simple( 1, &dim_ref, NULL )) < 0 )
    goto out;

   /* Create the attribute again with the changes of space */
   if ( (attr_id = H5Acreate( image_id, MXA::H5Image::Palette.c_str(), attr_type, attr_space_id, H5P_DEFAULT, H5P_DEFAULT )) < 0 )
    goto out;

    /* Write the attribute with the new references */
   if ( H5Awrite( attr_id, attr_type, refbuf ) < 0 )
    goto out;

   if ( H5Sclose( attr_space_id ) < 0 )
    goto out;

   free( refbuf );

  } /* H5T_REFERENCE */

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  /* Close the attribute. */
  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }

  /* Close the image dataset. */
 if ( H5Dclose( image_id ) < 0 )
  return -1;

 return 0;

out:
 H5Dclose( image_id );
 return -1;
}



/*-------------------------------------------------------------------------
 * Function: H5IMunlink_palette
 *
 * Purpose: This function dettaches a palette from an existing image dataset
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: September 10, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMunlink_palette( hid_t loc_id,
                           std::string imageName,
                           std::string pal_name )
{
 hid_t       image_id;
 hid_t       attr_type;
 hid_t       attr_id;
 hid_t       attr_class;
 int32_t         ok_pal, has_pal;

 /* Try to find the palette dataset */
 has_pal = H5Lite::findDataset( loc_id, pal_name );

 /* It does not exist. Return */
 if ( has_pal == 0 )
  return -1;

 /* The image dataset may or not have the attribute const_cast<std::string&>(MXA::H5Image::Palette)
  * First we try to open to see if it is already there; if not, it is created.
  * If it exists, the array of references is extended to hold the reference
  * to the new palette
  */

 /* First we get the image id */
 if ( (image_id = H5Dopen( loc_id, imageName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
 ok_pal = H5Lite::findAttribute( image_id, MXA::H5Image::Palette.c_str() );

 /* It does not exist. Nothing to do */
 if ( ok_pal == 0 )
  return -1;

 /* The attribute exists, open it */
 else if ( ok_pal ==  1 )
 {
  if ( (attr_id = H5Aopen_by_name( loc_id, imageName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( (attr_class = H5Tget_class( attr_type )) < 0 )
   goto out;

  /* Check if it is really a reference */
  if ( attr_class == H5T_REFERENCE )
  {

   /* Deelete the attribute */
   if ( H5Adelete( image_id, MXA::H5Image::Palette.c_str() ) < 0 )
    goto out;

  }  /* H5T_REFERENCE */

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  /* Close the attribute. */
  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 } /* ok_pal */

  /* Close the image dataset. */
 if ( H5Dclose( image_id ) < 0 )
  return -1;

 return 0;

out:
 H5Dclose( image_id );
 return -1;
}


/*-------------------------------------------------------------------------
 * Function: H5IMget_npalettes
 *
 * Purpose: Gets the number of palettes associated to an image
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: July 22, 2001
 *
 * Comments:
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMget_npalettes( hid_t loc_id,
                          std::string imageName,
                          hssize_t *npals )
{
 hid_t      image_id;
 hid_t      attr_type;
 hid_t      attr_id;
 hid_t      attr_space_id;
 hid_t      attr_class;
 int32_t        has_pal;

 /*assume initially we have no palettes attached*/
 *npals = 0;

 /* Open the dataset. */
 if ( (image_id = H5Dopen( loc_id, imageName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
 has_pal = H5IM_find_palette( image_id );

 if ( has_pal ==  1 )
 {

  if ( (attr_id = H5Aopen_by_name( loc_id, imageName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( (attr_class = H5Tget_class( attr_type )) < 0 )
   goto out;

  /* Check if it is really a reference */

  if ( attr_class == H5T_REFERENCE )
  {
   if ( (attr_space_id = H5Aget_space( attr_id )) < 0 )
    goto out;

   *npals = H5Sget_simple_extent_npoints( attr_space_id );

   if ( H5Sclose( attr_space_id ) < 0 )
    goto out;

  } /* H5T_REFERENCE */

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  /* Close the attribute. */
  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }

     /* Close the image dataset. */
 if ( H5Dclose( image_id ) < 0 )
  return -1;

 return 0;

out:
 H5Dclose( image_id );
 return -1;

}


/*-------------------------------------------------------------------------
 * Function: H5IMget_palette_info
 *
 * Purpose: Get palette information
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: July 22, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMget_palette_info( hid_t loc_id,
                        std::string imageName,
                        int32_t pal_number,
                        hsize_t *pal_dims )
{
 hid_t      image_id;
 int32_t        has_pal;
 hid_t      attr_type;
 hid_t      attr_id;
 hid_t      attr_space_id;
 hid_t      attr_class;
 hssize_t   n_refs;
 hsize_t    dim_ref;
 hobj_ref_t *refbuf;     /* buffer to read references */
 hid_t      pal_id;
 hid_t      pal_space_id;
 hsize_t    pal_maxdims[2];

 /* Open the dataset. */
 if ( (image_id = H5Dopen( loc_id, imageName.c_str(), H5P_DEFAULT)) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
 has_pal = H5Image::H5IM_find_palette( image_id );

 if ( has_pal ==  1 )
 {

  if ( (attr_id = H5Aopen_by_name( loc_id, imageName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( (attr_class = H5Tget_class( attr_type )) < 0 )
   goto out;

  /* Check if it is really a reference */

  if ( attr_class == H5T_REFERENCE )
  {

   /* Get the reference(s) */

   if ( (attr_space_id = H5Aget_space( attr_id )) < 0 )
    goto out;

   n_refs = H5Sget_simple_extent_npoints( attr_space_id );

   dim_ref = n_refs;

   refbuf = static_cast<hobj_ref_t*>(malloc( sizeof(hobj_ref_t) * (int)dim_ref ) );

   if ( H5Aread( attr_id, attr_type, refbuf ) < 0 )
    goto out;

 /* Get the actual palette */

  if ( (pal_id = H5Rdereference( image_id, H5R_OBJECT, &refbuf[pal_number] )) < 0 )
    goto out;

  if ( (pal_space_id = H5Dget_space( pal_id )) < 0 )
   goto out;

  if ( H5Sget_simple_extent_ndims( pal_space_id ) < 0 )
   goto out;

  if ( H5Sget_simple_extent_dims( pal_space_id, pal_dims, pal_maxdims ) < 0 )
   goto out;

  if ( H5Sclose( pal_space_id ) < 0 )
   goto out;

  if ( H5Sclose( attr_space_id ) < 0 )
   goto out;

   free( refbuf );

  } /* H5T_REFERENCE */

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  /* Close the attribute. */
  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }

  /* Close the image dataset. */
 if ( H5Dclose( image_id ) < 0 )
  return -1;

 return 0;

out:
 H5Dclose( image_id );
 return -1;

}


/*-------------------------------------------------------------------------
 * Function: H5IMget_palette
 *
 * Purpose: Read palette
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: August 30, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMget_palette( hid_t loc_id,
                        std::string imageName,
                        int32_t pal_number,
                        unsigned char *pal_data )
{
 hid_t      image_id;
 int32_t        has_pal;
 hid_t      attr_type;
 hid_t      attr_id;
 hid_t      attr_space_id;
 hid_t      attr_class;
 hssize_t   n_refs;
 hsize_t    dim_ref;
 hobj_ref_t *refbuf;     /* buffer to read references */
 hid_t      pal_id;

 /* Open the dataset. */
 if ( (image_id = H5Dopen( loc_id, imageName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::Palette) on the >>image<< dataset */
 has_pal = H5IM_find_palette( image_id );

 if ( has_pal ==  1 )
 {

  if ( (attr_id = H5Aopen_by_name( loc_id, imageName.c_str(), MXA::H5Image::Palette.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( (attr_class = H5Tget_class( attr_type )) < 0 )
   goto out;

  /* Check if it is really a reference */

  if ( attr_class == H5T_REFERENCE )
  {

   /* Get the reference(s) */

   if ( (attr_space_id = H5Aget_space( attr_id )) < 0 )
    goto out;

   n_refs = H5Sget_simple_extent_npoints( attr_space_id );

   dim_ref = n_refs;

   refbuf = static_cast<hobj_ref_t*>(malloc( sizeof(hobj_ref_t) * (int)dim_ref ) );

   if ( H5Aread( attr_id, attr_type, refbuf ) < 0 )
    goto out;

   /* Get the palette id */
   if ( (pal_id = H5Rdereference( image_id, H5R_OBJECT, &refbuf[pal_number] )) < 0 )
    goto out;

   /* Read the palette dataset */
   if ( H5Dread( pal_id, H5Dget_type(pal_id), H5S_ALL, H5S_ALL, H5P_DEFAULT, pal_data ) < 0 )
    goto out;

   if ( H5Sclose( attr_space_id ) < 0 )
    goto out;

   free( refbuf );

  } /* H5T_REFERENCE */

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  /* Close the attribute. */
  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }


     /* Close the image dataset. */
 if ( H5Dclose( image_id ) < 0 )
  return -1;

 return 0;

out:
 H5Dclose( image_id );
 return -1;

}

/*-------------------------------------------------------------------------
 * Function: H5IMis_image
 *
 * Purpose:
 *
 * Return: true, false, fail
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: August 30, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMis_image( hid_t loc_id,
                     std::string datasetName )
{
 hid_t      did;
 int32_t        has_class;
 hid_t      attr_type;
 hid_t      attr_id;
 char       attr_data[20];
 herr_t    ret;

 /* Assume initially fail condition */
 ret = -1;

 /* Open the dataset. */
 if ( (did = H5Dopen( loc_id, datasetName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

 /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::ImageClass) on the dataset */
 has_class = H5Lite::findAttribute( did, MXA::H5Image::ImageClass );

 if ( has_class ==  0 )
 {
  H5Dclose( did );
  return 0;
 }

 else if ( has_class ==  1 )
 {

  if ( (attr_id = H5Aopen_by_name( loc_id, datasetName.c_str(), MXA::H5Image::ImageClass.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( H5Tget_class( attr_type ) < 0 )
   goto out;

  if ( H5Aread( attr_id, attr_type, attr_data ) < 0 )
    goto out;

  if( strcmp( attr_data, MXA::H5Image::Image.c_str() ) == 0 )
   ret = 1;
  else
   ret = 0;

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }

 /* Close the dataset. */
 if ( H5Dclose( did ) < 0 )
  return -1;

 return ret;

out:
 H5Dclose( did );
 return -1;

}



/*-------------------------------------------------------------------------
 * Function: H5IMis_palette
 *
 * Purpose:
 *
 * Return: true, false, fail
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: August 30, 2001
 *
 * Comments:
 *  based on HDF5 Image and Palette Specification
 *  http://hdf.ncsa.uiuc.edu/HDF5/H5Image/ImageSpec.html
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */

herr_t H5Image::H5IMis_palette( hid_t loc_id,
                       std::string datasetName )
{
 hid_t      did;
 int32_t        has_class;
 hid_t      attr_type;
 hid_t      attr_id;
 char       attr_data[20];
 herr_t      ret;

 /* Assume initially fail condition */
 ret = -1;

 /* Open the dataset. */
 if ( (did = H5Dopen( loc_id, datasetName.c_str(), H5P_DEFAULT )) < 0 )
  return -1;

    /* Try to find the attribute const_cast<std::string&>(MXA::H5Image::ImageClass) on the dataset */
 has_class = H5Lite::findAttribute( did, MXA::H5Image::ImageClass);

 if ( has_class ==  0 )
 {
  H5Dclose( did );
  return 0;
 }

 else if ( has_class ==  1 )
 {

  if ( (attr_id = H5Aopen_by_name( loc_id, datasetName.c_str(), MXA::H5Image::ImageClass.c_str(), H5P_DEFAULT, H5P_DEFAULT )) < 0 )
   goto out;

  if ( (attr_type = H5Aget_type( attr_id )) < 0 )
   goto out;

  if ( H5Tget_class( attr_type ) < 0 )
   goto out;

  if ( H5Aread( attr_id, attr_type, attr_data ) < 0 )
    goto out;

  if( strcmp( attr_data, MXA::H5Image::Palette.c_str() ) == 0 )
   ret = 1;
  else
   ret = 0;

  if ( H5Tclose( attr_type ) < 0 )
   goto out;

  if ( H5Aclose( attr_id ) < 0 )
   goto out;

 }

     /* Close the dataset. */
 if ( H5Dclose( did ) < 0 )
  return -1;

 return ret;

out:
 H5Dclose( did );
 return -1;

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5Image::H5IMget_image_dimensions(hid_t loc_id,
                                      const std::string datasetName,
                                      int iDims[2])
{
  hid_t did, sid;
  hsize_t dims[3];
  herr_t err = 0;

  /* Open the dataset. */
  did = H5Dopen(loc_id, datasetName.c_str(), H5P_DEFAULT);
  if (did < 0)
  {
    return did;
  }

  /* Get the dataspace handle */
  sid = H5Dget_space(did);
  if (sid >= 0)
  {
    /* Get dimensions */
    err = H5Sget_simple_extent_dims(sid, dims, NULL);
    if (err >= 0)
    {
      iDims[1] = dims[0];
      iDims[0] = dims[1];
    }
    err = H5Sclose(sid);
  }
  err = H5Dclose(did);
  return err;
}
