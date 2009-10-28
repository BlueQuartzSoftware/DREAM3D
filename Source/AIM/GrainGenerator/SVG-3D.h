///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SVG3D_H_
#define SVG3D_H_

#include <string>
#ifdef SVG_3D_LIBRARY

int SVG_3D_Main(const std::string &inDir, const std::string &outDir,
                  int ng, int sc, double rx, double ry, double rz,
                  double oallowed, int oassignment, int crxst);
#endif

#endif /* SVG3D_H_ */
