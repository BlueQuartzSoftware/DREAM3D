/* ============================================================================
 * Copyright (c) 2013 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2013 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _H5FilterParametersConstants_H_
#define _H5FilterParametersConstants_H_

#include <string>


namespace H5FilterParameter
{
  const std::string ArrayNameConstant = "_ArrayName";
  const std::string CompOperatorConstant = "_CompOperator";
  const std::string CompValueConstant = "_CompValue";
  const std::string AngleConstant = "_Angle";
  const std::string HConstant = "_H";
  const std::string KConstant = "_K";
  const std::string LConstant = "_L";
  const std::string CellComparisonInputsConstant = "CellComparisonInputs";
  const std::string FieldComparisonInputsConstant = "FieldComparisonInputs";
  const std::string EnsembleComparisonInputsConstant = "EnsembleComparisonInputs";
  const std::string PointComparisonInputsConstant = "PointComparisonInputs";
  const std::string FaceComparisonInputsConstant = "FaceComparisonInputs";
  const std::string EdgeComparisonInputsConstant = "EdgeComparisonInputs";
  const std::string AxisAngleInputsConstant = "AxisAngleInputs";
  const std::string StlFilePrefixConstant = "StlFilePrefix";
  const std::string MaxIterationsConstant = "MaxIterations";
  const std::string MisorientationToleranceConstant = "MisorientationTolerance";
  const std::string InputFileConstant = "InputFile";
  const std::string InputPathConstant = "InputPath";
  const std::string OutputFileConstant = "OutputFile";
  const std::string OutputPathConstant = "OutputPath";
  const std::string WriteAlignmentShiftsConstant = "WriteAlignmentShifts";
  const std::string ConversionTypeConstant = "ConversionType";
  const std::string SelectedCellArrayNameConstant = "SelectedCellArrayName";
  const std::string SelectedFieldArrayNameConstant = "SelectedFieldArrayName";
  const std::string SelectedEnsembleArrayNameConstant = "SelectedEnsembleArrayName";
  const std::string SurfaceMeshPointArrayNameConstant = "SurfaceMeshPointArrayName";
  const std::string SurfaceMeshFaceArrayNameConstant = "SurfaceMeshFaceArrayName";
  const std::string SurfaceMeshEdgeArrayNameConstant = "SurfaceMeshEdgeArrayName";
  const std::string SolidMeshPointArrayNameConstant = "SolidMeshPointArrayName";
  const std::string SolidMeshFaceArrayNameConstant = "SolidMeshFaceArrayName";
  const std::string SolidMeshEdgeArrayNameConstant = "SolidMeshEdgeArrayName";
  const std::string GenericTestDimensionsConstant = "Dimensions";
  const std::string GenericTestOriginConstant = "Origin";
}

#endif /* _H5FilterParametersConstants_H_ */
