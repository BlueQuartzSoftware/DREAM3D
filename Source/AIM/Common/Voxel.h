/*
 * Voxel.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef VOXEL_H_
#define VOXEL_H_

#include <vector>
/*
 *
 */
class Voxel
{
public:
    Voxel();
    virtual ~Voxel();

    int grainname;
    double confidence;
    int alreadychecked;
    double xc;
    double yc;
    double zc;
    double euler1;
    double euler2;
    double euler3;
    int hasneighbor;
    int neighbor;
    double misorientation;
    int surfacevoxel;
    int neighbor1;
    int neighbor2;
    int neighbor3;
    int neighbor4;
    int neighbor5;
    int neighbor6;
    int available;
    int available90;
    int inside;
    std::vector<int> voxneighlist;

  private:

    Voxel(const Voxel&);    // Copy Constructor Not Implemented
      void operator=(const Voxel&);  // Operator '=' Not Implemented
};

#endif /* VOXEL_H_ */
