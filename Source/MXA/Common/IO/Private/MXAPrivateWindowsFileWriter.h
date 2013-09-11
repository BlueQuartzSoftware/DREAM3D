
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef WINDOWSFILEWRITER_H_
#define WINDOWSFILEWRITER_H_

#include "MXA/MXA.h"

#if defined (_MSC_VER)
#include <MXA/Common/Win32Defines.h>
#endif
//


// Conditional Includes/Defines for Large File Support on Windows
#if defined (WINDOWS_LARGE_FILE_SUPPORT)
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>
#define WRITER64_OUTPUT_STREAM HANDLE
#else  // Non Windows Platform
#include <fstream>
#define WRITER64_OUTPUT_STREAM std::ofstream
#endif

// Hard set our stream buffer to 4096 bytes
#define BUFF_SIZE 4096

// C++ Includes
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>

/**
 * @class MXAFileWriter64 MXAFileWriter64.h MXA/Common/IO/MXAFileWriter64.h
 * @brief This class is a wrapper around platform specific native streams to make
 * sure that we can write files larger than 2GB
 * @author Mike Jackson @ BlueQuartz Software.us
 * @date August 2007
 * @version $Revision: 1.2 $
 */
class MXA_EXPORT  MXAFILEWRITER_CLASS_NAME
{


public:
  explicit MXAFILEWRITER_CLASS_NAME(const QString &filename);

  virtual ~MXAFILEWRITER_CLASS_NAME();

  /**
   * @brief Initializes our stream object and opens the file
   * @return True on success. False on any failure
   */
  bool initWriter();

  /**
 * @brief Reads a number of bytes from the underlying stream
 * @param data The char pointer to read the data into
 * @param numBytes The number of bytes to read
 */
  bool write(char* data, int64_t numBytes)
  {
  DWORD nBytesToWrite = static_cast<DWORD>(numBytes);
  DWORD nBytesWritten = 0;
  int error = WriteFile(_outStream,  // Which is really an HANDLE hFile on Windows
                   data,
                   nBytesToWrite,
                   &nBytesWritten,
                   NULL) ;
    if (nBytesToWrite != nBytesWritten)
    {
      //throw std::runtime_error ( "Error: Number of bytes written did not match number of bytes asked." );
      return false;
    }
    return true;
  }

  /**
   * @brief Writes a string to the file
   * @param str The string to write
   * @return True on success
   */
  bool writeString(const QString &str)
  {
    DWORD nBytesToWrite = static_cast<DWORD>(str.size());
    DWORD nBytesWritten = 0;
    int error = WriteFile(_outStream,  // Which is really an HANDLE hFile on Windows
                     str.c_str(),
                     nBytesToWrite,
                     &nBytesWritten,
                     NULL) ;
      if (nBytesToWrite != nBytesWritten)
      {
        //throw std::runtime_error ( "Error: Number of bytes written did not match number of bytes asked." );
        return false;
      }
      return true;
  }



  /**
   * @brief Writes a single value to a file with no byte swapping performed
   * @param value The value to write to the file
   */
  template<typename T>
  bool writeValue(T* value)
  {
    DWORD nBytesToWrite = static_cast<DWORD>(sizeof(T));
    DWORD nBytesWritten = 0;
    int error = WriteFile(_outStream,  // Which is really an HANDLE hFile on Windows
                          (char*)(value),
                          nBytesToWrite,
                          &nBytesWritten,
                          NULL) ;
    if (nBytesToWrite != nBytesWritten)
    {
     // throw std::runtime_error ( "Error: Number of bytes written did not match number of bytes asked." );
      return false;
    }
    return true;
  }

  /**
   * @brief Writes a "C" array into the file
   * @param front pointer to the location in the array to start writing
   * @param numElements number of elements of the array to write
   */
  template<typename T>
  bool writeArray(T* front, int64_t numElements)
  {
    DWORD nBytesToWrite = static_cast<DWORD>(sizeof(T) * numElements);
    DWORD nBytesWritten = 0;
    int error = WriteFile(_outStream,  // Which is really an HANDLE hFile on Windows
                          (char*)(front),
                          nBytesToWrite,
                          &nBytesWritten,
                          NULL) ;
    if (nBytesToWrite != nBytesWritten)
    {
     // throw std::runtime_error ( "Error: Number of bytes written did not match number of bytes asked." );
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
    SetFilePointerEx(_outStream, offset, &posOut , FILE_BEGIN); //Seek from the beginning of file

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
    SetFilePointerEx(_outStream, offset, &posOut , FILE_CURRENT); //From where we are now
    return posOut.QuadPart;
  }


private:
  QString _filename;
  WRITER64_OUTPUT_STREAM _outStream;
  QVector<char> _buffer;

};

#endif /* WINDOWSFILEWRITER_H_ */
