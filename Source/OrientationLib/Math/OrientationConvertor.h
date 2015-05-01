#ifndef _OrientationConvertor_H_
#define _OrientationConvertor_H_


#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"


class OrientationLib_EXPORT OrientationConvertor
{

  public:

    enum OrientationType
    {
      Euler,
      OrientationMatrix,
      Quaternion,
      AxisAngle,
      Rodrigues,
      Homochoric,
      Cubochoric,
      UnknownOrientationType
    };

    OrientationConvertor();
    virtual ~OrientationConvertor();

    /**
     * @brief getOrientationRepresentation
     * @return
     */
    virtual OrientationType getOrientationRepresentation()
    {
      return UnknownOrientationType;
    }

    /**
     * @brief convert
     * @param src
     * @return
     */
    void convertRepresentationTo( OrientationType repType );

    virtual void toEulers() = 0;
    virtual void toOrientationMatrix() = 0;
    virtual void toQuaternion() = 0;
    virtual void toAxisAngle() = 0;
    virtual void toRodrigues() = 0;
    virtual void toHomochoric() = 0;
    virtual void toCubochoric() = 0;

    DREAM3D_INSTANCE_PROPERTY(FloatArrayType::Pointer, InputData)
    DREAM3D_INSTANCE_PROPERTY(FloatArrayType::Pointer, OutputData)

  protected:



  private:
    OrientationConvertor(const OrientationConvertor&); // Copy Constructor Not Implemented
    void operator=(const OrientationConvertor&); // Operator '=' Not Implemented

};

class OrientationLib_EXPORT EulerConvertor : public OrientationConvertor
{
  public:
    EulerConvertor();
    explicit EulerConvertor(FloatArrayType::Pointer data);
    virtual ~EulerConvertor();

    virtual OrientationType getOrientationRepresentation()
    {
      return Euler;
    }


    virtual void toEulers();
    virtual void toOrientationMatrix();
    virtual void toQuaternion();
    virtual void toAxisAngle();
    virtual void toRodrigues();
    virtual void toHomochoric();
    virtual void toCubochoric();

  private:

    EulerConvertor(const EulerConvertor&); // Copy Constructor Not Implemented
    void operator=(const EulerConvertor&); // Operator '=' Not Implemented
};


class OrientationLib_EXPORT QuaternionConvertor : public OrientationConvertor
{
  public:
    QuaternionConvertor();
    explicit QuaternionConvertor(FloatArrayType::Pointer data);
    virtual ~QuaternionConvertor();

    virtual OrientationType getOrientationRepresentation()
    {
      return Quaternion;
    }


    virtual void toEulers();
    virtual void toOrientationMatrix();
    virtual void toQuaternion();
    virtual void toAxisAngle();
    virtual void toRodrigues();
    virtual void toHomochoric();
    virtual void toCubochoric();

  private:

    QuaternionConvertor(const QuaternionConvertor&); // Copy Constructor Not Implemented
    void operator=(const QuaternionConvertor&); // Operator '=' Not Implemented
};

#endif /* _OrientationConvertor_H_ */
