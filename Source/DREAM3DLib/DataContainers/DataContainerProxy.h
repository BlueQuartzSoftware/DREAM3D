/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _DataContainerProxy_H_
#define _DataContainerProxy_H_


#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/AttributeMatrixProxy.h"


/**
 * @brief The DataContainerProxy class
 */
class DataContainerProxy
{
  public:
    /**
     * @brief DataContainerProxy
     */
    DataContainerProxy() :
      flag(Qt::Unchecked),
      name("")
    {}

    /**
     * @brief DataContainerProxy
     * @param dc_name
     * @param read_dc
     * @param dc_type
     */
    DataContainerProxy(QString dc_name, uint8_t read_dc = Qt::Checked) :
      flag(read_dc),
      name(dc_name)
      {}

    /**
    * @brief Copy Constructor
    */
    DataContainerProxy(const DataContainerProxy& amp)
    {
      flag = amp.flag;
      name = amp.name;
      dcType = amp.dcType;
      attributeMatricies = amp.attributeMatricies;
    }

    /**
    * @brief operator = method
    */
    void operator=(const DataContainerProxy& amp)
    {
      flag = amp.flag;
      name = amp.name;
      dcType = amp.dcType;
      attributeMatricies = amp.attributeMatricies;
    }

	/**
	* @brief operator == method
	*/
	bool operator==(const DataContainerProxy& amp)
	{
		if (flag == amp.flag && name == amp.name && dcType == amp.dcType)
		{
			return true;
		}
		return false;
	}

    //----- Our variables, publicly available
    uint8_t flag;
    QString name;
    unsigned int dcType;
    QMap<QString, AttributeMatrixProxy> attributeMatricies;

};

Q_DECLARE_METATYPE(DataContainerProxy)



#endif /* _DataContainerProxy_H_ */

