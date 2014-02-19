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
#ifndef _ComparisonInputs_H_
#define _ComparisonInputs_H_

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"

typedef struct { QString dataContainerName;
                 QString attributeMatrixName;
                 QString attributeArrayName;
                 int compOperator;
                 double compValue; } ComparisonInput_t;

/**
 * @class ComparisonInputs ComparisonInputs.h DREAM3DLib/Common/ComparisonInputs.h
 * @brief  This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Feb 18, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT ComparisonInputs : public QObject
{
    Q_OBJECT

  public:
    ComparisonInputs();
    ComparisonInputs(const ComparisonInputs& rhs);
    //explicit ComparisonInputs(ComparisonInputs& rhs);

    virtual ~ComparisonInputs();

    int size();

    void addInput(const QString dataContainerName,
                 const QString attributeMatrixName,
                 const QString arrayName,
                 int compOperator,
                 double compValue);
    void addInput(const ComparisonInput_t& input);

    ComparisonInput_t& getInput(int index);
    QVector<ComparisonInput_t>& getInputs();

    void operator=(const ComparisonInputs&);

    ComparisonInput_t& operator[](int index);

  private:

    QVector<ComparisonInput_t> m_Inputs;


};


Q_DECLARE_METATYPE(ComparisonInput_t)
Q_DECLARE_METATYPE(ComparisonInputs)


#endif /* _ComparisonInputs_H_ */
