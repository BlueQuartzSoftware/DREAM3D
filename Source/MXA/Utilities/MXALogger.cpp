
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//


#include "MXALogger.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXALogger::MXALogger()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXALogger::~MXALogger()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXALogger::Pointer MXALogger::instance()
{
  static MXALogger::Pointer singleton;
  if (singleton.get() == NULL)
    {
  //  std::cout << "MXALogger constructing singleton instance" << std::endl;
      singleton.reset (new MXALogger() );
    }
    return singleton;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXALogger_Implementation::MXALogger_Implementation() :
  m_IsFileBased(false),
  m_FileName("NO FILE SET")
{
  //std::cout << "MXALogger_Implementation constructing" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXALogger_Implementation::~MXALogger_Implementation()
{
  this->m_OutStream.flush();
  this->m_OutStream.close();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXALogger_Implementation::open(const std::string &fn, std::ios::openmode mode)
{
  if (fn == this->m_FileName && m_OutStream.is_open() == true) {
    return true;
  }
  if (fn != this->m_FileName && m_OutStream.is_open() == true) {
     this->m_OutStream.flush();
     this->m_OutStream.close();
     this->m_IsFileBased = false;
     this->m_FileName = ""; // Close the current file stream
   }


  this->m_FileName = fn;
  m_OutStream.open(m_FileName.c_str(),  std::ios::out | mode);
  if (m_OutStream.is_open() )
  {
    this->m_IsFileBased = true;
  }
  else
  {
    this->m_FileName = "";
    this->m_IsFileBased = false;
  }
  return m_IsFileBased;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXALogger_Implementation::close()
{
  m_OutStream.flush();
  m_OutStream.close();
  m_IsFileBased = false;
  setFileName("");
  return !m_OutStream.is_open();
}
