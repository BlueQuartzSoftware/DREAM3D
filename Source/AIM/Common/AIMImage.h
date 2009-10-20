/* <distribution-statement>
 * *****************************************************************************
 * Distribution authorized to DOD components and DOD contractors only;         *
 * Administrative or Operational Use;                                          *
 * February 2009.                                                              *
 * Other requests for this document shall be referred to                       *
 * U.S. Air Force Research Laboratory                                          *
 * 2230 Tenth St., Ste. 1                                                      *
 * Wright-Patterson AFB, OH 45433-7817                                         *
 *                                                                             *
 * Destroy by contents or reconstruction of the document.                      *
 *                                                                             *
 * This code was written under the U.S. Air Force contrct F33615-03-C-5220     *
 * *****************************************************************************
 * </distribution-statement> */

#ifndef R3DMOSAIC_H_
#define R3DMOSAIC_H_

#include <MXA/Common/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <iostream>


class AIMImage
{
  public:

  MXA_SHARED_POINTERS(AIMImage);
  MXA_STATIC_NEW_MACRO(AIMImage);
  MXA_TYPE_MACRO(AIMImage);
  virtual ~AIMImage();

  /**
  * @brief Creates a new AIMImage by copying all the settings from a source AIMImage
  * object and allocate the buffer at the same time. If the 'allocateBuffer' argument is
  * false then the image buffer will NOT be allocated. Note that the actual data from
  * the source mosaic is NOT copied into the new object.
  * @param image The source AIMImage to copy settings from.
  * @param allocateBuffer If false, the memory to hold the image is NOT allocated. Default=TRUE
  * @return AIMImage::Pointer object
  */
  static AIMImage::Pointer NewFromSourceMosaic(AIMImage::Pointer image, bool allocateBuffer = true);

  MXA_INSTANCE_2DVECTOR_PROPERTY(int, ImagePixelSize, _pixelSize);
  const int32* getImagePixelSize()
  {
    return _pixelSize;
  }

  void setImagePixelSize(AIMImage::Pointer image);
  int32 getImagePixelWidth();
  int32 getImagePixelHeight();


  void setImageBuffer(unsigned char* value, bool manageMemory = false);
  uint8* allocateImageBuffer(int32 width, int32 height, bool manageMemory = false);
  uint8* allocateSameSizeImage(AIMImage::Pointer image);
  unsigned char* getImageBuffer();
  
  uint8* getPointer(size_t index = 0);

  void deallocateImageBuffer();
  MXA_INSTANCE_PROPERTY(bool, ManageMemory, _managememory);

  int32 initializeImageWithSourceData(int32 width, int32 height, uint8* source);

  int32 fillImageBuffer(uint8 val);

  void printSelf(std::ostream& out);

  size_t getTotalPixels();

  protected:
    AIMImage();

  private:
    uint8*      _imageBuffer;
    AIMImage(const AIMImage&);    // Copy Constructor Not Implemented
    void operator=(const AIMImage&);  // Operator '=' Not Implemented
};

#endif /* R3DMOSAIC_H_ */
