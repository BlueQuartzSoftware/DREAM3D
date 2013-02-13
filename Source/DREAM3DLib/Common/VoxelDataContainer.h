/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _VoxelDataContainer_H_
#define _VoxelDataContainer_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- EBSD Lib Includes
#include "EbsdLib/EbsdConstants.h"

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Observable.h"


/**
 * @class VoxelDataContainer VoxelDataContainer DREAM3DLib/Common/VoxelDataContainer.h
 * @brief This data container holds data the represents a structured rectangular
 * grid of data typically referred to as a Voxel Volume
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for AFRL
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT VoxelDataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(VoxelDataContainer)
    DREAM3D_STATIC_NEW_MACRO(VoxelDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(VoxelDataContainer, Observable)

    virtual ~VoxelDataContainer();

//    template<typename T>
//    std::string TypeForPrimitive(T value)
//    {

//      if (typeid(value) == typeid(float)) return "H5T_NATIVE_FLOAT";
//      if (typeid(value) == typeid(double)) return "H5T_NATIVE_DOUBLE";

//      if (typeid(value) == typeid(int8_t)) return "H5T_NATIVE_INT8";
//      if (typeid(value) == typeid(uint8_t)) return "H5T_NATIVE_UINT8";
//# if CMP_TYPE_CHAR_IS_SIGNED
//      if (typeid(value) == typeid(char)) return "H5T_NATIVE_INT8";
//#else
//      if (typeid(value) == typeid(char)) return "H5T_NATIVE_UINT8";
//#endif
//      if (typeid(value) == typeid(signed char)) return "H5T_NATIVE_INT8";
//      if (typeid(value) == typeid(unsigned char)) return "H5T_NATIVE_UINT8";


//      if (typeid(value) == typeid(int16_t)) return "H5T_NATIVE_INT16";
//      if (typeid(value) == typeid(short)) return "H5T_NATIVE_INT16";
//      if (typeid(value) == typeid(signed short)) return "H5T_NATIVE_INT16";
//      if (typeid(value) == typeid(uint16_t)) return "H5T_NATIVE_UINT16";
//      if (typeid(value) == typeid(unsigned short)) return "H5T_NATIVE_UINT16";


//      if (typeid(value) == typeid(int32_t)) return "H5T_NATIVE_INT32";
//      if (typeid(value) == typeid(uint32_t)) return "H5T_NATIVE_UINT32";
//#if (CMP_SIZEOF_INT == 4)
//      if (typeid(value) == typeid(int)) return "H5T_NATIVE_INT32";
//      if (typeid(value) == typeid(signed int)) return "H5T_NATIVE_INT32";
//      if (typeid(value) == typeid(unsigned int)) return "H5T_NATIVE_UINT32";
//#endif


//#if (CMP_SIZEOF_LONG == 4)
//      if (typeid(value) == typeid(long int)) return "H5T_NATIVE_INT32";
//      if (typeid(value) == typeid(signed long int)) return "H5T_NATIVE_INT32";
//      if (typeid(value) == typeid(unsigned long int)) return "H5T_NATIVE_UINT32";
//#elif (CMP_SIZEOF_LONG == 8)
//      if (typeid(value) == typeid(long int)) return "H5T_NATIVE_INT64";
//      if (typeid(value) == typeid(signed long int)) return "H5T_NATIVE_INT64";
//      if (typeid(value) == typeid(unsigned long int)) return "H5T_NATIVE_UINT64";
//#endif


//#if (CMP_SIZEOF_LONG_LONG == 8)
//      if (typeid(value) == typeid(long long int)) return "H5T_NATIVE_INT64";
//      if (typeid(value) == typeid(signed long long int)) return "H5T_NATIVE_INT64";
//      if (typeid(value) == typeid(unsigned long long int)) return "H5T_NATIVE_UINT64";
//#endif
//      if (typeid(value) == typeid(int64_t)) return "H5T_NATIVE_INT64";
//      if (typeid(value) == typeid(uint64_t)) return "H5T_NATIVE_UINT64";

//      if (typeid(value) == typeid(bool)) return "H5T_NATIVE_UINT8";

//      return "H5T_UNKNOWN";
//    }

    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Cell)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Field)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Ensemble)

    METHOD_DEF_TEMPLATE_GETARRAYDATA(getCellData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA(getFieldData)
    METHOD_DEF_TEMPLATE_GETARRAYDATA(getEnsembleData)


   /**
   * @brief Adds/overwrites the data for a named array
   * @param name The name that the array will be known by
   * @param data The IDataArray::Pointer that will hold the data
   */
    virtual void addCellData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual IDataArray::Pointer getCellData(const std::string &name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual IDataArray::Pointer removeCellData(const std::string &name);

    /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearCellData();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual std::list<std::string> getCellArrayNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumCellArrays();

    /**
    * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
    * in during a set of filtering operations then the a value of '32' would be returned.
    * @return
    */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumCellTuples)

    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    */
    virtual void addFieldData(const std::string &name, IDataArray::Pointer data);

    /**
    * @brief Returns the array for a given named array or the equivelant to a
    * null pointer if the name does not exist.
    * @param name The name of the data array
    */
    virtual IDataArray::Pointer getFieldData(const std::string &name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method
    * @param name The name of the array
    * @return
    */
    virtual IDataArray::Pointer removeFieldData(const std::string &name);

    /**
     * @brief Removes all the Field Arrays
     */
    virtual void clearFieldData();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Field (Formerly Grain) group
    * @return
    */
    virtual std::list<std::string> getFieldArrayNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Field group
    * @return
    */
    virtual int getNumFieldArrays();

    /**
    * @brief Returns the number of Tuples that the field data has. For example if there are 32 grains
    * in during a set of filtering operations then the a value of '32' would be returned.
    * @return
    */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumFieldTuples)

    /**
    * @brief Resizes all of the Field Arrays to have 'size' tuples
    * @param size The number of tuples that each DataArray should contain.
    */
    virtual void resizeFieldDataArrays(size_t size);

    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    */
    virtual void addEnsembleData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual IDataArray::Pointer getEnsembleData(const std::string &name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual IDataArray::Pointer removeEnsembleData(const std::string &name);

    /**
     * @brief Removes all the ensemble data
     */
    virtual void clearEnsembleData();

    virtual std::list<std::string> getEnsembleArrayNameList();

     /**
     * @brief Returns if a data set with the name exists in the data container for the Cell Data
     */

    virtual int getNumEnsembleArrays();

    DREAM3D_INSTANCE_PROPERTY(size_t, NumEnsembleTuples)


    /* ****************** END Map Based Methods *******************************/

    DREAM3D_INSTANCE_VEC3_PROPERTY(size_t, Dimensions)

    size_t getXPoints() { return m_Dimensions[0];}
    size_t getYPoints() { return m_Dimensions[1];}
    size_t getZPoints() { return m_Dimensions[2];}

    int64_t getTotalPoints() { return (int64_t)m_Dimensions[0] * (int64_t)m_Dimensions[1] * (int64_t)m_Dimensions[2]; }

// -----------------------------------------------------------------------------
//  Resolution Methods
// -----------------------------------------------------------------------------
    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Resolution)

    float getXRes() { return m_Resolution[0];}
    float getYRes() { return m_Resolution[1];}
    float getZRes() { return m_Resolution[2];}

    // -----------------------------------------------------------------------------
    //  Origin Methods
    // -----------------------------------------------------------------------------
    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Origin)



    DOES_DATASET_EXIST_DECL(CellData)
    DOES_DATASET_EXIST_DECL(FieldData)
    DOES_DATASET_EXIST_DECL(EnsembleData)


  protected:
    VoxelDataContainer();

  private:

    std::map<std::string, IDataArray::Pointer> m_CellData;
    std::map<std::string, IDataArray::Pointer> m_FieldData;
    std::map<std::string, IDataArray::Pointer> m_EnsembleData;

    VoxelDataContainer(const VoxelDataContainer&);
    void operator =(const VoxelDataContainer&);
};

#endif
