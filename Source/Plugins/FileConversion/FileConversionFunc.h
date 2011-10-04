
#ifndef _FileConversionFunc_H
#define _FileConversionFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @class FileConversionFunc FileConversionFunc.h AIM/Common/FileConversionFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class FileConversionFunc
{
public:
    DREAM3D_SHARED_POINTERS(FileConversionFunc)
    DREAM3D_TYPE_MACRO(FileConversionFunc)
    DREAM3D_STATIC_NEW_MACRO(FileConversionFunc);

    virtual ~FileConversionFunc();

    typedef boost::shared_array<float>    SharedFloatArray;
    typedef boost::shared_array<int>      SharedIntArray;

protected:
    FileConversionFunc();

private:
    FileConversionFunc(const FileConversionFunc& );
    void operator =(const FileConversionFunc& );
};


#endif
