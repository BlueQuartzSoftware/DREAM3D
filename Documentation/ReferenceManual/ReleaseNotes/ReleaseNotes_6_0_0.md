Version 6.0. Release Notes {#releasenotes_6_0_0}
===========

# Major Changes #



# Filters that have changed names in Version 6 #

| Version 4 Name | Version 6 Name |
|----------------|----------------|
| AlignSectionsPhaseCorrelation |   |
| AvizoRectilinearCoordinateWriter | Removed from distribution<sup>[0]</sup>  |
| AvizoUniformCoordinateWriter | Removed from distribution<sup>[0]</sup> |
| BridgeParentIdsStatisticsToGrainIds |   |
| CAxisSegmentGrains | Segment Features (C-Axis Misalignment) |
| ChangeGrainIds |   |
| ClearData |   |
| ConvertEulerAngles | Convert Angles to Degrees or Radians  |
| ConvertOrientation | Convert Orientation Representation  |
| CopyFieldArrayToCellArray | Create Element Array From Feature Array |
| CopyFieldArrayToFaceArray | Create Element Array From Feature Array |
| CreateFieldArrayFromCellArray | Create Feature Array From Cell Array |
| CropSurfaceMesh |   |
| DecimateSurfaceMesh |   |
| EBSDSegmentGrains | Segment Features (Misorientation) |
| FFTElasticReader |   |
| FieldDataCSVWriter | Write Feature Data as CSV File |
| FieldInfoReader | Read Feature Info File |
| FindBoundingBoxGrains |   |
| FindCellQuats | Convert Orientation Representation |
| FindFaceAverage |   |
| FindFieldAverage |   |
| FindFieldNeighborCAxisMisalignments | Find Local Average C-Axis Misalignments |
| FindGrainCentroids |   |
| FindGrainPhases |   |
| FindGrainReferenceCAxisMisorientations |   |
| FindGrainReferenceMisorientations |   |
| FindKernelAvgMisorientationsIgnoreGrains |   |
| FindNumFields |   |
| FindSurfaceCells |   |
| FindSurfaceGrains |   |
| FindSurfaceMeshNodeType |   |
| FindSurfaceVoxelFractions |   |
| GenerateFaceMisorientationColors | Generate Misorientation Colors (Face)  |
| GenerateSurfaceMeshConnectivity | Generate Geometry Connectivity |
| GrainFaceCurvatureFilter | Feature Face Connectivity |
| GroupMicroTextureRegions |   |
| H5VoxelFileReader | Removed from distribution <sup>[3]</sup> |
| Hex2SqrConverter | Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)  |
| LinkFieldMapToCellArray |   |
| M3CSliceBySlice | Removed from distribution <sup>[2]</sup> |
| MergeArrays |   |
| MergeChannels | Gray to RGB (Merge Channels) |
| MoveGrain |   |
| MovingFiniteElementSmoothing | Removed from distribution <sup>[2]</sup> |
| MultiThresholdCells | Threshold |
| MultiThresholdFields | Threshold |
| NodesTrianglesToStl | Removed from distribution<sup>[1]</sup> |
| NodesTrianglesToVtk | Removed from distribution<sup>[1]</sup> |
| OpenCloseBadData |   |
| OpenCloseCoordinationNumber |   |
| PerPhaseMinSize |   |
| ReadOrientationData | Read EDAX EBSD Data or Read Oxford Instr. EBSD Data  |
| RemoveJitter |   |
| RenameCellArray | Rename Attribute Array |
| RenameFaceArray | Rename Attribute Array |
| RenameFieldArray | Rename Attribute Array |
| RenameSurfaceMeshFieldArray | Rename Attribute Array |
| RotateSliceEulerRefFrame | Rotate Euler Reference Frame |
| SampleVolume |   |
| SaveImages | Write Images |
| ScalarSegmentGrains | Segment Features (Scalar) |
| SegmentBoundaries |   |
| SharedGrainFaceFilter | Generate Triangle Face Ids |
| SingleThresholdCells | Threshold Objects |
| SingleThresholdFields | Threshold Objects |
| SolidMeshToVtk | Removed from distribution <sup>[3]</sup> |
| SurfaceMeshToNodesTrianglesEdges | Removed from distribution <sup>[3]</sup> |
| SurfaceMeshToNonconformalVtk | Removed from distribution <sup>[3]</sup> |
| SurfaceMeshToStl | Write STL Files from Triangle Geometry |
| SurfaceMeshToVtk | Write Vtk PolyData from Triangle Geometry |
| Threshold | Threshold Objects |
| TranslateSurfaceMesh | Change Origin of Volume |
| UnifyTriangleWinding | Removed from distribution <sup>[4]</sup> |
| UpdateCellQuats | Removed from distribution <sup>[3]</sup> |
| VerifyTriangleWinding | Removed from distribution <sup>[3]</sup> |
| VtkGrainIdReader | Removed from distribution <sup>[3]</sup> |
| VtkRectilinearGridWriter |Vtk Rectilinear Grid Writer|
| YSChoiAbaqusReader | Removed from distribution <sup>[3]</sup> |


[0] Test files are not available to verify proper output is being created.
[1] Introduction of dedicated Geometry classes allow us to remove this filter.
[2] Original developer is not updating this filter any longer.
[3] This filter is considered legacy and no longer supported.
[4] This filter is removed for technical reasons



