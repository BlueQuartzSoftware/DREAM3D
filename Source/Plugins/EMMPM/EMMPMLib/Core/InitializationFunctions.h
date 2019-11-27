/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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


#pragma once

//-- SIMPLib Includes
#include <memory>

#include "SIMPLib/SIMPLib.h"


//-- EM/MPM Lib Includes
#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Data.h"

/**
 * @brief
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT InitializationFunction
{
  public:
    using Self = InitializationFunction;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for InitializationFunction;
     */
    virtual QString getNameOfClass() const;
    /**
     * @brief Returns the name of the class for InitializationFunction;
     */
    static QString ClassName();

    virtual ~InitializationFunction();

    virtual void initialize(EMMPM_Data::Pointer data);

  protected:
    InitializationFunction();

  public:
    InitializationFunction(const InitializationFunction&) = delete;            // Copy Constructor Not Implemented
    InitializationFunction(InitializationFunction&&) = delete;                 // Move Constructor Not Implemented
    InitializationFunction& operator=(const InitializationFunction&) = delete; // Copy Assignment Not Implemented
    InitializationFunction& operator=(InitializationFunction&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @brief The standard EM/MPM initialization functions
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT BasicInitialization : public InitializationFunction
{

  public:
    using Self = BasicInitialization;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for BasicInitialization;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for BasicInitialization;
     */
    static QString ClassName();

    ~BasicInitialization() override;

    void initialize(EMMPM_Data::Pointer data) override;

  protected:
    BasicInitialization();

  public:
    BasicInitialization(const BasicInitialization&) = delete;            // Copy Constructor Not Implemented
    BasicInitialization(BasicInitialization&&) = delete;                 // Move Constructor Not Implemented
    BasicInitialization& operator=(const BasicInitialization&) = delete; // Copy Assignment Not Implemented
    BasicInitialization& operator=(BasicInitialization&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @brief Initialization using user defined areas of the input image
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT UserDefinedAreasInitialization : public InitializationFunction
{

  public:
    using Self = UserDefinedAreasInitialization;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for UserDefinedAreasInitialization;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for UserDefinedAreasInitialization;
     */
    static QString ClassName();

    ~UserDefinedAreasInitialization() override;

    void initialize(EMMPM_Data::Pointer data) override;

  protected:
    UserDefinedAreasInitialization();

  public:
    UserDefinedAreasInitialization(const UserDefinedAreasInitialization&) = delete;            // Copy Constructor Not Implemented
    UserDefinedAreasInitialization(UserDefinedAreasInitialization&&) = delete;                 // Move Constructor Not Implemented
    UserDefinedAreasInitialization& operator=(const UserDefinedAreasInitialization&) = delete; // Copy Assignment Not Implemented
    UserDefinedAreasInitialization& operator=(UserDefinedAreasInitialization&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @brief Initializes the XT (Label Map) array
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT XtArrayInitialization : public InitializationFunction
{

  public:
    using Self = XtArrayInitialization;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for XtArrayInitialization;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for XtArrayInitialization;
     */
    static QString ClassName();

    ~XtArrayInitialization() override;

    void initialize(EMMPM_Data::Pointer data) override;

  protected:
    XtArrayInitialization();

  public:
    XtArrayInitialization(const XtArrayInitialization&) = delete;            // Copy Constructor Not Implemented
    XtArrayInitialization(XtArrayInitialization&&) = delete;                 // Move Constructor Not Implemented
    XtArrayInitialization& operator=(const XtArrayInitialization&) = delete; // Copy Assignment Not Implemented
    XtArrayInitialization& operator=(XtArrayInitialization&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @brief Initializes the Gradient Variables
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT GradientVariablesInitialization : public InitializationFunction
{

  public:
    using Self = GradientVariablesInitialization;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for GradientVariablesInitialization;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for GradientVariablesInitialization;
     */
    static QString ClassName();

    ~GradientVariablesInitialization() override;

    void initialize(EMMPM_Data::Pointer data) override;

  protected:
    GradientVariablesInitialization();

  public:
    GradientVariablesInitialization(const GradientVariablesInitialization&) = delete;            // Copy Constructor Not Implemented
    GradientVariablesInitialization(GradientVariablesInitialization&&) = delete;                 // Move Constructor Not Implemented
    GradientVariablesInitialization& operator=(const GradientVariablesInitialization&) = delete; // Copy Assignment Not Implemented
    GradientVariablesInitialization& operator=(GradientVariablesInitialization&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @brief Initialization using the curvature penalty functions
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPMLib_EXPORT CurvatureInitialization : public InitializationFunction
{

  public:
    using Self = CurvatureInitialization;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for CurvatureInitialization;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for CurvatureInitialization;
     */
    static QString ClassName();

    ~CurvatureInitialization() override;

    void initialize(EMMPM_Data::Pointer data) override;

    /**
     * @brief Initialize the Curvature Penalty variables
     * @param data
     */
    void initCurvatureVariables(EMMPM_Data::Pointer data);

  protected:
    CurvatureInitialization();

  private:
  public:
    CurvatureInitialization(const CurvatureInitialization&) = delete;            // Copy Constructor Not Implemented
    CurvatureInitialization(CurvatureInitialization&&) = delete;                 // Move Constructor Not Implemented
    CurvatureInitialization& operator=(const CurvatureInitialization&) = delete; // Copy Assignment Not Implemented
    CurvatureInitialization& operator=(CurvatureInitialization&&) = delete;      // Move Assignment Not Implemented
};
