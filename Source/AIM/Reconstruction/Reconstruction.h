///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>

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
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory, m_OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(AngFilePrefix, m_AngFilePrefix)
    MXA_INSTANCE_PROPERTY_m(int, AngSeriesMaxSlice)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexEnd)
    MXA_INSTANCE_PROPERTY_m(double, ZResolution)
    MXA_INSTANCE_PROPERTY_m(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY_m(int32, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY_m(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY_m(double, MisorientationTolerance)
    MXA_INSTANCE_PROPERTY_m(AIM::Reconstruction::CrystalStructure, CrystalStructure)
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
