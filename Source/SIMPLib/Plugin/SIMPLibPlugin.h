/* ============================================================================
* Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* All rights reserved.
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
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _SIMPLibPlugin_H_
#define _SIMPLibPlugin_H_

#include <QtCore/QObject>
#include "SIMPLib/SIMPLib.h"
#include "ISIMPLibPlugin.h"


/**
* @class SIMPLibPlugin SIMPLibPlugin.h
* @brief
* @author
* @date
* @version 1.0
*/
class SIMPLib_EXPORT SIMPLibPlugin : public QObject
{
    Q_OBJECT

  public:
    SIMPLibPlugin();
    virtual ~SIMPLibPlugin();
    /**
    * @brief Returns the name of the plugin
    */
    QString getPluginName();

    /**
    * @brief Sets the plugin name of the plugin
    */
    void setPluginName(QString name);

    /**
    * @brief Returns the version
    */
    QString getVersion();

    /**
    * @brief Sets the version of the plugin
    */
    void setVersion(QString version);

    /**
    * @brief Returns the name of the vendor
    */
    QString getVendor();

    /**
    * @brief Sets the vendor of the plugin
    */
    void setVendor(QString vendor);

    /**
    * @brief Returns the location of the plugin
    */
    QString getLocation();

    /**
    * @brief Sets the location of the plugin on the file system.
    * This is required so that we can cache the file path information
    * as the plugin is loaded.
    */
    void setLocation(QString filePath);

    /**
    * @brief Returns the load status of the plugin
    */
    QString getStatus();

    /**
    * @brief Sets the load status of the plugin
    */
    void setStatus(QString status);

  private:
    QString       m_PluginName;
    QString             m_Version;
    QString             m_Vendor;
    QString             m_Location;
    QString             m_Status;

    SIMPLibPlugin(const SIMPLibPlugin&); // Copy Constructor Not Implemented
    void operator=(const SIMPLibPlugin&); // Operator '=' Not Implemented
};

#endif /* _SIMPLibPlugin_H_ */
