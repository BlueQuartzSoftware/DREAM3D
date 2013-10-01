
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _MXA_DLL_EXPORT_H_
#define _MXA_DLL_EXPORT_H_


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

/* Cmake will define MXA_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define MXA_EXPORTS when
building the MXADatModel DLL on windows.
*/

#if defined (MXA_BUILT_AS_DYNAMIC_LIB)

  #if defined (MXA_EXPORTS)  /* Compiling the MXA DLL/Dylib */
    #if defined (_MSC_VER)  /* MSVC Compiler Case */
      #define  MXA_EXPORT __declspec(dllexport)
    #elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
      #define MXA_EXPORT __attribute__ ((visibility("default")))
    #endif
  #else  /* Importing the DLL into another project */
    #if defined (_MSC_VER)  /* MSVC Compiler Case */
      #define  MXA_EXPORT __declspec(dllimport)
    #elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
      #define MXA_EXPORT __attribute__ ((visibility("default")))
    #endif
  #endif
#endif

/* If MXA_EXPORT was never defined, define it here */
#ifndef MXA_EXPORT
  #define MXA_EXPORT
#endif

#if 0
#if defined (_WIN32) || defined __CYGWIN__

  #if defined (MXA_BUILT_AS_DYNAMIC_LIB)
    #if defined(MXA_EXPORTS)
      #define  MXA_EXPORT __declspec(dllexport)
    #else
      #define  MXA_EXPORT __declspec(dllimport)
    #endif /* MXA_EXPORTS */
  #else
    #define MXA_EXPORT
  #endif
#elif __GNUC__ >= 4
 #define FLOW_DLL __attribute__ ((visibility("default")))
 #define DLL_LOCAL  __attribute__ ((visibility("hidden")
#else /* defined (_WIN32) && defined (MXA_BUILD_SHARED_LIBS)  */
 #define MXA_EXPORT
#endif
#endif

#endif /* _MXA_DLL_EXPORT_H_ */
