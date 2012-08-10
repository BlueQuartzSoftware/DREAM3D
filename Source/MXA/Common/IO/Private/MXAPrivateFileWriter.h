
//
//  Copyright (c) 2008, Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef _FILEWRITER_H_
#define _FILEWRITER_H_

#include "MXA/MXA.h"

#include <fstream>
#define WRITER64_OUTPUT_STREAM std::ofstream

// Hard set our stream buffer to 4096 bytes
#define BUFF_SIZE 4096

// C++ Includes
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>

/**
 * @class MXAFileWriter64 MXAFileWriter64.h Common/IO/MXAFileWriter64.h
 * @brief This class is a private implementation class around platform specific
 * native streams to make sure that we can write files larger than 2GB
 * @author Mike Jackson @ BlueQuartz Software.us
 * @date August 2007
 * @version 1.2
 */
class MXA_EXPORT MXAFILEWRITER_CLASS_NAME
{


public:
  explicit MXAFILEWRITER_CLASS_NAME(const std::string &filename);

  virtual ~MXAFILEWRITER_CLASS_NAME();

  /**
   * @brief Initializes our stream object and opens the file
   * @return True on success. False on any failure
   */
  bool initWriter();

  /**
 * @brief Writes a number of bytes from the underlying stream
 * @param data The char pointer to read the data into
 * @param numBytes The number of bytes to read
 */
  bool write(char* data, int64_t numBytes)
  {
    _outStream.write(data, numBytes);
    if (_outStream.bad() == true) {return false;}
    return true;
  }

  /**
   * @brief Writes a string to the file
   * @param str The string to write
   * @return True on success
   */
  bool writeString(const std::string &str)
  {
    _outStream.write(str.c_str(), str.size());
    if (_outStream.bad() == true) {return false;}
    return true;
  }

  /**
   * @brief Writes a single value to a file with no byte swapping performed
   * @param value The value to write to the file
   */
  template<typename T>
  bool writeValue(T* value)
  {
    _outStream.write(reinterpret_cast<char*>(value), sizeof(T) );
    if (_outStream.bad() == true) {return false;}
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
    int64_t numBytes = numElements * sizeof(T);
    _outStream.write(reinterpret_cast<char*>(front), numBytes);
    if (_outStream.bad() == true) {return false;}
    return true;
  }

  /**
 * @brief Sets the filepointer of the underlying stream
 * @param position The position to set the file pointer to
 */
  void setFilePointer64( int64_t position )
  {
    _outStream.seekp(position);
  }

/**
 * @brief Returns the current position of the file pointer
 * @return the Current position of the file pointer
 */
  int64_t getFilePointer64()
  {
    return _outStream.tellp();
  }


private:
  std::string _filename;
  WRITER64_OUTPUT_STREAM _outStream;
  std::vector<char> _buffer;

};

#endif // _FILEWRITER_H_
