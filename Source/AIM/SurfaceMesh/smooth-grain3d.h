///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SMOOTHGRAIN3D_H_
#define SMOOTHGRAIN3D_H_

#include <string>


  /**
   *
   * @param nodesFile
   * @param trianglesFile
   * @param inpFile
   * @param iterations
   * @param printInterval
   * @param lockquads
   * @return
   */
  int SmoothGrain3D(const std::string &nodesFile, const std::string &trianglesFile,
                    const std::string &outputDir, size_t updatesMax, int interval,
                    int lockquads);



#endif /* SMOOTHGRAIN3D_H_ */
