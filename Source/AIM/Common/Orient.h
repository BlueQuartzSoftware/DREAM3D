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

#ifndef ORIENT_H_
#define ORIENT_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


/*
 *
 */
class Orient
{
  public:
    Orient();
    virtual ~Orient();

    double rad1x;
    double rad1y;
    double rad1z;
    double rad2x;
    double rad2y;
    double rad2z;
    double rad3x;
    double rad3y;
    double rad3z;

  private:
    Orient(const Orient&);    // Copy Constructor Not Implemented
      void operator=(const Orient&);  // Operator '=' Not Implemented
};

#endif /* ORIENT_H_ */
