///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _DREAM3DLib_DLL_EXPORT_H_
#define _DREAM3DLib_DLL_EXPORT_H_


#if defined (_MSC_VER)
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4710)
  #pragma warning(disable: 4820)
  #pragma warning(disable: 4668)
  #pragma warning(disable: 4265)
  #pragma warning(disable: 4189)
  #pragma warning(disable: 4640)
  #pragma warning(disable: 4996)
  #pragma warning(disable: 4548)
#endif

/* Cmake will define DREAM3DLib_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define DREAM3DLib_EXPORTS when
building the MXADatModel DLL on windows.
*/

#if defined (DREAM3DLib_BUILT_AS_DYNAMIC_LIB)

  #if defined (DREAM3DLib_EXPORTS)  /* Compiling the MXA DLL/Dylib */
    #if defined (_MSC_VER)  /* MSVC Compiler Case */
      #define  DREAM3DLib_EXPORT __declspec(dllexport)
    #elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
      #define DREAM3DLib_EXPORT __attribute__ ((visibility("default")))
    #endif
  #else  /* Importing the DLL into another project */
    #if defined (_MSC_VER)  /* MSVC Compiler Case */
      #define  DREAM3DLib_EXPORT __declspec(dllimport)
    #elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
      #define DREAM3DLib_EXPORT __attribute__ ((visibility("default")))
    #endif
  #endif
#endif

/* If DREAM3DLib_EXPORT was never defined, define it here */
#ifndef DREAM3DLib_EXPORT
  #define DREAM3DLib_EXPORT
#endif

#if 0
#if defined (_WIN32) || defined __CYGWIN__

  #if defined (MXA_BUILT_AS_DYNAMIC_LIB)
    #if defined(DREAM3DLib_EXPORTS)
      #define  DREAM3DLib_EXPORT __declspec(dllexport)
    #else
      #define  DREAM3DLib_EXPORT __declspec(dllimport)
    #endif /* DREAM3DLib_EXPORTS */
  #else
    #define DREAM3DLib_EXPORT
  #endif
#elif __GNUC__ >= 4
 #define FLOW_DLL __attribute__ ((visibility("default")))
 #define DLL_LOCAL  __attribute__ ((visibility("hidden")
#else /* defined (_WIN32) && defined (MXA_BUILD_SHARED_LIBS)  */
 #define DREAM3DLib_EXPORT
#endif
#endif

#endif /* _DREAM3DLib_DLL_EXPORT_H_ */
