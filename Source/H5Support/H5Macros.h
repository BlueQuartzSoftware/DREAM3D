/* ============================================================================
 * Copyright (c) 2013 Michael A. Jackson (BlueQuartz Software)
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _H5Functions_H_
#define _H5Functions_H_


//-- HDF Headers
#include <hdf5.h>

#include "H5Support/H5Support.h"
#include "H5Support/H5SupportDLLExport.h"

//TODO: Add tests for the find* methods

#define CloseH5A(aid, err, retError)\
  err = H5Aclose( attr_id );\
  if (err<0) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): " << "Error Closing Attribute." << std::endl;retErr = err;}

#define CloseH5D(did, err, retError)\
  err = H5Dclose(did);\
  if (err < 0) { std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error Closing Dataset." << std::endl; retError = err;}

#define CloseH5S(sid, err, retError)\
  err = H5Sclose(sid); \
  if ( err < 0) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error closing Dataspace." << std::endl;retErr = err;}

#define CloseH5T(tid, err, retError)\
  err = H5Tclose(tid);\
  if (err < 0 ) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error closing DataType" << std::endl; retErr = err;}

#define HDF_ERROR_HANDLER_OFF\
  herr_t (*_oldHDF_error_func)(hid_t, void *);\
  void *_oldHDF_error_client_data;\
  H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);\
  H5Eset_auto(H5E_DEFAULT, NULL, NULL);

#define HDF_ERROR_HANDLER_ON  H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);


#define UNUSED(x) ((void)(x));


#endif /* _H5Functions_H_ */

