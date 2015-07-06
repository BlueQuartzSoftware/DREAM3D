/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "UpdateCheckData.h"


#include "moc_UpdateCheckData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheckData::UpdateCheckData(QObject* parent) :
  QObject(parent),
  m_HasUpdate(false),
  m_HasError(false),
  m_MessageDescription(""),
  m_AppString(""),
  m_ServerString("")
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheckData::~UpdateCheckData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheckData::UpdateCheckData(const UpdateCheckData& rhs)
{
  m_AppString = rhs.m_AppString;
  m_HasUpdate = rhs.m_HasUpdate;
  m_HasError = rhs.m_HasError;
  m_MessageDescription = rhs.m_MessageDescription;
  m_ServerString = rhs.m_ServerString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::operator=(const UpdateCheckData& rhs)
{
  m_AppString = rhs.m_AppString;
  m_HasUpdate = rhs.m_HasUpdate;
  m_HasError = rhs.m_HasError;
  m_MessageDescription = rhs.m_MessageDescription;
  m_ServerString = rhs.m_ServerString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool UpdateCheckData::operator==(const UpdateCheckData& rhs)
{
  return (m_AppString == rhs.m_AppString &&
          m_HasUpdate == rhs.m_HasUpdate &&
          m_HasError == rhs.m_HasError &&
          m_MessageDescription == rhs.m_MessageDescription &&
          m_ServerString == rhs.m_ServerString);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool UpdateCheckData::hasUpdate()
{
  return m_HasUpdate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool UpdateCheckData::hasError()
{
  return m_HasError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckData::getMessageDescription()
{
  return m_MessageDescription;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckData::getAppString()
{
  return m_AppString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UpdateCheckData::getServerString()
{
  return m_ServerString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::setHasUpdate(bool val)
{
  m_HasUpdate = val;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::setHasError(bool val)
{
  m_HasError = val;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::setMessageDescription(QString msg)
{
  m_MessageDescription = msg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::setAppString(QString str)
{
  m_AppString = str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheckData::setServerString(QString str)
{
  m_ServerString = str;
}
