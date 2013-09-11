
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef _MXAPRIVATE_FILEREADER_H_
#define _MXAPRIVATE_FILEREADER_H_


// Hard set our stream buffer to 4096 bytes
#define BUFF_SIZE 4096

// C++ Includes
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "MXA/MXA.h"

#define READER64_INPUT_STREAM std::ifstream

/**
* @class MXAFileReader64
* @brief 64 Bit capable file reader.
* @author mjackson
* @date Feb 26, 2009
* @version 1.0
*/
class MXA_EXPORT MXAFILEREADER_CLASS_NAME
{
  public:
    explicit MXAFILEREADER_CLASS_NAME(const QString &filename);
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
     * @return false if errors
     */
      bool rawRead(char* data, std::streamsize numBytes)
      {
       if ( EOF == _instream.peek() )
       {
          //throw std::runtime_error ( "Error: Unexpected end of file reached" );
         return false;
       }
       _instream.read ( data, numBytes );
       if (_instream.gcount() != numBytes)
       {
         //throw std::runtime_error ( "Error: Number of bytes read does NOT match size of input." );
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
       if ( EOF == _instream.peek() )
       {
          //throw std::runtime_error ( "Error: Unexpected end of file reached" );
         return false;
       }
       _instream.read ( reinterpret_cast<char*>( &t ), sizeof ( P ) );
       return true;
      }


      /**
       * @brief This method will read an array of values and swap each value
       * @param t Pointer to the front of the array where the values will be stored.
       * The array MUST already be preallocated.
       * @param size The number of elements in the array
       */
      template < typename EndianPolicy, typename P>
      bool readArrayWithSwap(P* t, int64_t size)
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
        int64_t numBytes = numElements * sizeof(T);
        return rawRead(reinterpret_cast<char*>(t), numBytes);
      }

    /**
     * @brief Sets the filepointer of the underlying stream
     * @param position The position to set the file pointer to
     */
      void setFilePointer64( int64_t position )
      {
        _instream.seekg(position);
      }

    /**
     * @brief Returns the current position of the file pointer
     * @return the Current position of the file pointer
     */
      int64_t getFilePointer64()
      {
        return _instream.tellg();
      }


    private:
      QString _filename;
      READER64_INPUT_STREAM _instream;
      QVector<char> _buffer;

      MXAFILEREADER_CLASS_NAME(const MXAFILEREADER_CLASS_NAME&);    //Not Implemented
      void operator=(const MXAFILEREADER_CLASS_NAME&); //Not Implemented

};





#endif /* _MXAPRIVATE_FILEREADER_H_ */
