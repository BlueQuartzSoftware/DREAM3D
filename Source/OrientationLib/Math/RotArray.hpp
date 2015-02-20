


#ifndef _RotArray_H_
#define _RotArray_H_


#include <string.h>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "OrientationLib/OrientationLib.h"


template<typename T>
class OrientationLib_EXPORT RotArray
{

  public:
    /**
     * @brief RotArray
     * @param size
     * @param init
     */
    RotArray(size_t size, T init = (T)(0) ) :
      m_Ptr(NULL),
      m_Size(size),
      m_Owns(true)
    {
      allocate();
      for(size_t i = 0; i < m_Size; i++)
      {
        m_Ptr[i] = init;
      }
    }

    /**
     * @brief RotArray
     * @param ptr
     * @param size
     */
    RotArray(T* ptr, size_t size) :
      m_Ptr(ptr),
      m_Size(size),
      m_Owns(false)
    {

    }

    /**
     * @brief RotArray
     * @param rhs
     */
    RotArray(const RotArray<T>& rhs)
    {
      m_Size = rhs.m_Size;
      m_Owns = true;
      allocate();
      ::memcpy(m_Ptr, rhs.m_Ptr, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }

    virtual ~RotArray() {
      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
      }
      m_Ptr = NULL;
    }

    /**
     * @brief operator =
     */
    void operator=(const RotArray& rhs)
    {
      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
      }
      m_Ptr = NULL;
      m_Size = rhs.size();
      allocate();
      ::memcpy(m_Ptr, rhs.m_Ptr, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }

    /**
     * @brief length
     * @return
     */
    size_t size() const { return m_Size; }

    /**
     * @brief operator []
     * @param i
     * @return
     */
    T& operator[](size_t i) const { return m_Ptr[i]; }

    /**
     * @brief data
     * @return
     */
    T* data() const { return m_Ptr; }

    /**
     * @brief sum
     * @return
     */
    T sum() const {
      T sum = static_cast<T>(0);
      for(size_t i = 0; i < m_Size; i++)
      {
        sum = sum + m_Ptr[i];
      }
      return sum;
    }

    /**
     * @brief maxval
     * @return
     */
    T maxval() const {
      T max = m_Ptr[0];
      for(size_t i = 1; i < m_Size; i++)
      {
        if(m_Ptr[i] > max) { max = m_Ptr[i]; }
      }
      return max;
    }

    /**
     * @brief minval
     * @return
     */
    T minval() const {
      T min = m_Ptr[0];
      for(size_t i = 1; i < m_Size; i++)
      {
        if(m_Ptr[i] < min) { min = m_Ptr[i]; }
      }
      return min;
    }

    /**
     * @brief product
     * @return
     */
    T product() const {
      T pro = m_Ptr[0];
      for(size_t i = 1; i < m_Size; i++)
      {
        pro *= m_Ptr[i];
      }
      return pro;
    }


    /**
     * @brief operator *
     * @param rhs
     * @return
     */
    RotArray<T> operator*(const RotArray<T>& rhs) const
    {
      RotArray<T> out(m_Size); // Create a new blank array
      for(size_t i = 0; i < m_Size; i++)
      {
        out[i] = m_Ptr[i] * rhs[i];
      }
      return out;
    }

    RotArray<T> absValue() const {
      RotArray<T> out(m_Size); // Create a new blank array
      for(size_t i = 0; i < m_Size; i++)
      {
        out[i] = abs((int)(m_Ptr[i]));
      }
      return out;
    }

    void toRadians()
    {
      for(size_t i = 0; i < m_Size; i++)
      {
        m_Ptr[i] =m_Ptr[i] * DREAM3D::Constants::k_PiOver180;
      }
    }

    void toDegrees()
    {
      for(size_t i = 0; i < m_Size; i++)
      {
        m_Ptr[i] =m_Ptr[i] * DREAM3D::Constants::k_180OverPi;
      }
    }

  protected:
    void allocate() {
      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
        m_Ptr = NULL;
      }
      if(m_Ptr == NULL)
      {
        m_Ptr = reinterpret_cast<T*>(malloc(sizeof(T) * m_Size));
        ::memset(m_Ptr, 0, sizeof(T) * m_Size);
        m_Owns = true;
      }
    }

  private:
    T* m_Ptr;
    size_t m_Size;
    bool m_Owns;

};

typedef RotArray<float> RotArrayF;
typedef RotArray<double> RotArrayD;


#endif /* _RotArray_H_ */
