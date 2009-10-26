/*
 * Orient.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

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
class Orients
{
public:

};
