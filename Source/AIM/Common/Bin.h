/*
 * Bin.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef BIN_H_
#define BIN_H_

/*
 *
 */
class Bin
{
  public:
    Bin();
    virtual ~Bin();

    double euler1;
    double euler2;
    double euler3;
    double dprobability;
    double sprobability;
    double Nprobability;
    double Nvalue;
    double bctotprobability;
    double diameter;
    double rad;
    double bctotal;
    double difference;
    double height;
    double binrank;

  private:
    Bin(const Bin&);    // Copy Constructor Not Implemented
      void operator=(const Bin&);  // Operator '=' Not Implemented
};

#endif /* BIN_H_ */
