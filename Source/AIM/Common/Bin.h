
#ifndef BIN_H_
#define BIN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


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
