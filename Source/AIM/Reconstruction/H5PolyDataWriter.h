/*
 * H5PolyDataWriter.h
 *
 *  Created on: Nov 16, 2010
 *      Author: mjackson
 */

#ifndef H5POLYDATAWRITER_H_
#define H5POLYDATAWRITER_H_


#include <MXA/Common/MXASetGetMacros.h>
/*
 *
 */
class H5PolyDataWriter
{
  public:
    MXA_SHARED_POINTERS(H5PolyDataWriter);
    MXA_STATIC_NEW_MACRO(H5PolyDataWriter);

    virtual ~H5PolyDataWriter();

    int writePolyData(const std::string &hdfFile, const std::string &hdfInternalPath);


  protected:
    H5PolyDataWriter();

  private:
    H5PolyDataWriter(const H5PolyDataWriter&); // Copy Constructor Not Implemented
    void operator=(const H5PolyDataWriter&); // Operator '=' Not Implemented
};

#endif /* H5POLYDATAWRITER_H_ */
