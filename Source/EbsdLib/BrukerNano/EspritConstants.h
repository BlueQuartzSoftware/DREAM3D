/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QString>

namespace Ebsd
{
namespace Esprit
{
const QString EulerAngles("EulerAngles");
const QString CrystalStructures("CrystalStructures");
const QString MaterialName("MaterialName");
const QString LatticeConstants("LatticeConstants");
const QString BravaisLattice("BravaisLattice");
//   const QString PatternData("PatternData");
} // namespace Esprit
namespace H5Esprit
{
const QString BrukerNano("Bruker Nano");
const QString Manufacturer("Manufacturer");
const QString Version("Version");
const QString H5FileExt("h5");
const QString Isometric("isometric");
const QString EBSD("EBSD");
const QString Header("Header");
const QString Phases("Phases");
const QString Data("Data");

// Header Section for EBSD Data
const QString CameraTilt("CameraTilt");
const QString GridType("Grid Type");
const QString KV("KV");
const QString MADMax("MADMax");
const QString Magnification("Magnification");
const QString MapStepFactor("MapStepFactor");
const QString MaxRadonBandCount("MaxRadonBandCount");
const QString MinIndexedBands("MinIndexedBands");
const QString NCOLS("NCOLS");
const QString NPoints("NPoints");
const QString NROWS("NROWS");
const QString OriginalFile("OriginalFile");
const QString PatternHeight("PatternHeight");
const QString PatternWidth("PatternWidth");
const QString PixelByteCount("PixelByteCount");
const QString SEMImage("SEM Image"); // <=== Actual SEM Image is stored here.
const QString SEPixelSizeX("SEPixelSizeX");
const QString SEPixelSizeY("SEPixelSizeY");
const QString SampleTilt("SampleTilt");
const QString TopClip("TopClip");
const QString UnClippedPatternHeight("UnClippedPatternHeight");
const QString WD("WD");
const QString XSTEP("XSTEP");
const QString YSTEP("YSTEP");
const QString ZOffset("ZOffset");

// Phases section of EBSD/Header
const QString Formula("Formula");
const QString IT("IT");
const QString LatticeConstants("LatticeConstants");
const QString Name("Name");
const QString Setting("Setting");
const QString SpaceGroup("SpaceGroup");

// Data Section for EBSD Data
const QString DD("DD");
const QString MAD("MAD");
const QString MADPhase("MADPhase");
const QString NIndexedBands("NIndexedBands");
const QString PCX("PCX");
const QString PCY("PCY");
const QString PHI("PHI");
const QString Phase("Phase");
const QString RadonBandCount("RadonBandCount");
const QString RadonQuality("RadonQuality");
const QString RawPatterns("RawPatterns");
const QString XBEAM("X BEAM");
const QString YBEAM("Y BEAM");
const QString XSAMPLE("X SAMPLE");
const QString YSAMPLE("Y SAMPLE");
const QString phi1("phi1");
const QString phi2("phi2");

using DD_t = float;
using MAD_t = float;
using MADPhase_t = int32_t;
using NIndexedBands_t = int32_t;
using PCX_t = float;
using PCY_t = float;
using PHI_t = float;
using Phase_t = int32_t;
using RadonBandCount_t = int32_t;
using RadonQuality_t = float;
using RawPatterns_t = uint8_t;
using XBEAM_t = int32_t;
using YBEAM_t = int32_t;
using XSAMPLE_t = float;
using YSAMPLE_t = float;
using phi1_t = float;
using phi2_t = float;

const QString SEM("SEM");
// Header Section for SEM Data
const QString SEM_IX("SEM IX");
const QString SEM_IY("SEM IY");
const QString SEM_Image("SEM Image");
const QString SEM_ImageHeight("SEM ImageHeight");
const QString SEM_ImageWidth("SEM ImageWidth");
const QString SEM_KV("SEM KV");
const QString SEM_Magnification("SEM Magnification");
const QString SEM_WD("SEM WD");
const QString SEM_XResolution("SEM XResolution");
const QString SEM_YResolution("SEM YResolution");
const QString SEM_ZOffset("SEM ZOffset");

} // namespace H5Esprit
} // namespace Ebsd
