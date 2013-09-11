
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef WINDOWSFILEREADER_H_
#define WINDOWSFILEREADER_H_

#ifndef _MSC_VER
#error THIS FILE IS FOR WINDOWS ONLY. If this is being included on any other platform\
  please file a bug report.
#endif

#include "MXA/MXA.h"
#include <MXA/Common/Win32Defines.h> // This will define WINDOWS_LARGE_FILE_SUPPORT to true, which is what we want


// Conditional Includes/Defines for Large File Support on Windows
#if defined (WINDOWS_LARGE_FILE_SUPPORT)
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>
#define READER64_INPUT_STREAM HANDLE
#else
#include <fstream>
#define READER64_INPUT_STREAM std::ifstream
#endif

#include <vector>
/**
 * @class MXAFileReader64 MXAFileReader64.h MXA/Common/IO/MXAFileReader64.h
 * @brief This class is a wrapper around platform specific native streams to make
 * sure that we can read files larger than 2GB
 * @author Mike Jackson @ BlueQuartz Software.us
 * @date August 2007
 * @version $Revision: 1.2 $
 */
class MXA_EXPORT MXAFILEREADER_CLASS_NAME
{


public:
  MXAFILEREADER_CLASS_NAME(const QString &filename);

  virtual ~MXAFILEREADER_CLASS_NAME();

  /**
   * @brief Initializes our stream object and opens the file
   * @return True on success. False on any failure
   */
  bool initReader();

/**
 * @brief Reads and swaps (if necessary) a single primitive value.
 * @param t The value to read the value into
*/
  template <typename EndianPolicy, typename P>
  bool read(P &t)
  {
    bool ok = this->readValue<P>(t);
    EndianPolicy::convert(t);
    return ok;
  }

/**
 * @brief Reads a number of bytes from the underlying stream
 * @param data The char pointer to read the data into
 * @param numBytes The number of bytes to read
 */
  bool rawRead(char* data, std::streamsize numBytes)
  {
  DWORD nBytesToRead = static_cast<DWORD>(numBytes);
  DWORD nBytesRead = 0;
  int error = ReadFile(_instream,  // Which is really an HANDLE hFile on Windows
                   data,
                   nBytesToRead,
                   &nBytesRead,
                   NULL) ;
  if (nBytesRead != nBytesToRead)
  {
   // throw std::runtime_error ( "Error: Number of bytes read does NOT match size of input." );
    return false;
  }
  return true;
  }

/**
 * @brief Reads a single primitive value from the underlying stream without any
 * byte swapping or conversions
 * @param t The value to read the primitive into
 */
  template <  typename P >
  bool readValue ( P &t )
  {
  DWORD nBytesToRead = sizeof(P);
  DWORD nBytesRead = 0;
  int error = ReadFile(_instream,  // Which is really an HANDLE hFile on Windows
                   &t,
                   nBytesToRead,
                   &nBytesRead,
                   NULL) ;
  if (nBytesRead != nBytesToRead)
  {
   // throw std::runtime_error ( "Error: Number of bytes read does NOT match size of data type." );
    return false;
  }
  return true;
  }


  /**
   * @brief This method will read an array of values and swap each value
   * @param t Pointer to the front of the array where the values will be stored.
   * The array MUST already be preallocated.
   * @param size The number of elements in the array
   */
  template < typename EndianPolicy, typename P>
  bool readArrayWithSwap(P* t, int32_t size)
  {
    bool ok = false;
    for (int32_t i = 0; i < size; ++i)
    {
      ok = read<EndianPolicy>(t[i]);
      if (!ok)
      {return ok;}
    }
    return true;
  }

  /**
   * @brief This method will read an array of values and swap each value
   * @param t Pointer to the front of the array where the values will be stored.
   * The array MUST already be preallocated.
   * @param numElements The number of elements in the array
   */
  template <typename T>
  bool readArray(T* t, int64_t numElements)
  {
    DWORD nBytesToRead = static_cast<DWORD>(numElements * sizeof(T));
    DWORD nBytesRead = 0;
    int error = ReadFile(_instream,  // Which is really an HANDLE hFile on Windows
                   reinterpret_cast<char*>(t),
                   nBytesToRead,
                   &nBytesRead,
                   NULL) ;
    if (nBytesRead != nBytesToRead)
    {
     // throw std::runtime_error ( "Error: Number of bytes read does NOT match size of input." );
      return false;
    }
    return true;
  }


/**
 * @brief Sets the filepointer of the underlying stream
 * @param position The position to set the file pointer to
 */
  void setFilePointer64( int64_t position )
  {
    LARGE_INTEGER posOut;
    LARGE_INTEGER offset;
    offset.QuadPart = position;
    SetFilePointerEx(_instream, offset, &posOut , FILE_BEGIN); //Seek from the beginning of file
  }

/**
 * @brief Returns the current position of the file pointer
 * @return the Current position of the file pointer
 */
  int64_t getFilePointer64()
  {
    LARGE_INTEGER posOut;
    LARGE_INTEGER offset;
    offset.QuadPart = 0;
    SetFilePointerEx(_instream, offset, &posOut , FILE_CURRENT); //From where we are now
    return posOut.QuadPart;
  }


private:

  QString _filename;
  READER64_INPUT_STREAM _instream;
  QVector<char> _buffer;

};


#endif /* WINDOWSFILEREADER_H_ */
