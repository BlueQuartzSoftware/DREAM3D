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
#ifndef _emmpmlibdllexport_h_
#define _emmpmlibdllexport_h_


/* Cmake will define EMMPMLib_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define EMMPMLib_EXPORTS when
building the EMMPMDatModel DLL on windows.
*/

#if defined (EMMPMLib_BUILT_AS_DYNAMIC_LIB)

#if defined (EMMPMLib_EXPORTS)  /* Compiling the EMMPM DLL/Dylib */
#if defined (_MSC_VER)  /* MSVC Compiler Case */
#define  EMMPMLib_EXPORT __declspec(dllexport)
#elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
#define EMMPMLib_EXPORT __attribute__ ((visibility("default")))
#endif
#else  /* Importing the DLL into another project */
#if defined (_MSC_VER)  /* MSVC Compiler Case */
#define  EMMPMLib_EXPORT __declspec(dllimport)
#elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
#define EMMPMLib_EXPORT __attribute__ ((visibility("default")))
#endif
#endif
#endif

/* If EMMPMLib_EXPORT was never defined, define it here */
#ifndef EMMPMLib_EXPORT
#define EMMPMLib_EXPORT
#endif


#endif /* _EMMPM_DLL_EXPORT_H_ */
