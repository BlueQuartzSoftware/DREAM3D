/*
 * Reconstruction.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>

/*
 *
 */
class Reconstruction
{
  public:
    MXA_SHARED_POINTERS(Reconstruction);
    MXA_STATIC_NEW_MACRO(Reconstruction);

    virtual ~Reconstruction();


    MXA_INSTANCE_STRING_PROPERTY(InputDirectory, m_InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(AngFilePrefix, m_AngFilePrefix)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexEnd)
    MXA_INSTANCE_PROPERTY_m(double, ZResolution)
    MXA_INSTANCE_PROPERTY_m(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY_m(int32, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY_m(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY_m(double, MisorientationTolerance)
    MXA_INSTANCE_PROPERTY_m(double, CrystalStructure)
    MXA_INSTANCE_PROPERTY_m(bool, AlreadyFormed)


    void parseAngFile();


    int32 compute();

  protected:
    Reconstruction();

  private:
    Reconstruction(const Reconstruction&);    // Copy Constructor Not Implemented
    void operator=(const Reconstruction&);  // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTION_H_ */
