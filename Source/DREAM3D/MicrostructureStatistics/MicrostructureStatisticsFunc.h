/*
 * MicrostructureStatisticsFunc.h
 *
 *  Created on: May 28, 2011
 *      Author: mjackson
 */

#ifndef MICROSTRUCTURESTATISTICSFUNC_H_
#define MICROSTRUCTURESTATISTICSFUNC_H_


#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"

/*
 *
 */
class DREAM3DLib_EXPORT MicrostructureStatisticsFunc
{
  public:
    MXA_SHARED_POINTERS(MicrostructureStatisticsFunc)
    MXA_STATIC_NEW_MACRO(MicrostructureStatisticsFunc)

    virtual ~MicrostructureStatisticsFunc();

  protected:
    MicrostructureStatisticsFunc();

  private:
    MicrostructureStatisticsFunc(const MicrostructureStatisticsFunc&);    // Copy Constructor Not Implemented
    void operator=(const MicrostructureStatisticsFunc&);  // Operator '=' Not Implemented
};

#endif /* MICROSTRUCTURESTATISTICSFUNC_H_ */
