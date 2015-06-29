Version 6.0. Release Notes {#releasenotes_6_0_0}
===========

# Major Changes #

+ The core DREAM.3D data structure has been rewritten. **This is a major change that breaks compatibility with previous DREAM.3D version files. Files generated with a version of DREAM.3D before v6.x are INCOMPATIBLE with v6.x and any subsequent versions!**
+ Several **Filters** have been renamed or removed. Consult the table below for a detailed list. **The removal or renaming of Filters may break compatibility with Pipelines generated in previous versions of DREAM.3D. Users are encouraged to update their Pipelines to utilize the current Filter suite of v6.x!**

# Filters that have changed names in Version 6 #

| Version 4 Name | Version 6 Name |
|----------------|----------------|
| AlignSectionsPhaseCorrelation | Removed from distribution<sup>2</sup>  |
| AvizoRectilinearCoordinateWriter | Removed from distribution<sup>0</sup>  |
| AvizoUniformCoordinateWriter | Removed from distribution<sup>0</sup> |
| BridgeParentIdsStatisticsToGrainIds | Removed from distribution<sup>2</sup>  |
| CAxisSegmentGrains | Segment Features (C-Axis Misalignment) |
| ChangeGrainIds | Removed from distribution<sup>3</sup>  |
| ClearData | Clear Data (Mask)  |
| ConvertEulerAngles | Convert Angles to Degrees or Radians  |
| ConvertOrientation | Convert Orientation Representation  |
| CopyFieldArrayToCellArray | Create Element Array From Feature Array |
| CopyFieldArrayToFaceArray | Create Element Array From Feature Array |
| CreateFieldArrayFromCellArray | Create Feature Array From Cell Array |
| CropSurfaceMesh | Removed from distribution<sup>4</sup>  |
| DecimateSurfaceMesh | Removed from distribution<sup>2</sup> |
| EBSDSegmentGrains | Segment Features (Misorientation) |
| FFTElasticReader | Removed from distribution<sup>2</sup>  |
| FieldDataCSVWriter | Write Feature Data as CSV File |
| FieldInfoReader | Read Feature Info File |
| FindBoundingBoxGrains | Extract Flagged Features (Rogues Gallery)  |
| FindCellQuats | Convert Orientation Representation |
| FindFaceAverage | Removed from distribution<sup>4</sup>  |
| FindFieldAverage | Removed from distribution<sup>4</sup>  |
| FindFieldNeighborCAxisMisalignments | Find Local Average C-Axis Misalignments |
| FindGrainCentroids | Find Feature Centroids  |
| FindGrainPhases | Find Feature Phases  |
| FindGrainReferenceCAxisMisorientations | Find Feature Reference C-Axis Misalignments  |
| FindGrainReferenceMisorientations |  Find Feature Reference Misorientations |
| FindKernelAvgMisorientationsIgnoreGrains |  Find Kernel Average Misorientations |
| FindNumFields | Find Number of Features  |
| FindSurfaceCells | Find Boundary Cells (Image)  |
| FindSurfaceGrains | Find Surface Features  |
| FindSurfaceMeshNodeType |  Removed from distribution<sup>1</sup> |
| FindSurfaceVoxelFractions | Find Feature Boundary Element Fractions  |
| GenerateFaceMisorientationColors | Generate Misorientation Colors (Face)  |
| GenerateSurfaceMeshConnectivity | Generate Geometry Connectivity |
| GrainFaceCurvatureFilter | Feature Face Connectivity |
| GroupMicroTextureRegions |  Removed from distribution<sup>2</sup> |
| H5VoxelFileReader | Removed from distribution <sup>3</sup> |
| Hex2SqrConverter | Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)  |
| LinkFieldMapToCellArray | Link Feature Attribute Matrix to Element Attribute Array  |
| M3CSliceBySlice | Removed from distribution <sup>2</sup> |
| MergeArrays | Removed from distribution <sup>4</sup>  |
| MergeChannels | Gray to RGB (Merge Channels) |
| MoveGrain |  Removed from distribution <sup>4</sup> |
| MovingFiniteElementSmoothing | Removed from distribution <sup>2</sup> |
| MultiThresholdCells | Threshold Objects |
| MultiThresholdFields | Threshold Objects |
| NodesTrianglesToStl | Removed from distribution<sup>1</sup> |
| NodesTrianglesToVtk | Removed from distribution<sup>1</sup> |
| OpenCloseBadData | Erode/Dilate Bad Data  |
| OpenCloseCoordinationNumber |  Removed from distribution<sup>4</sup> |
| PerPhaseMinSize |  Minimum Size |
| ReadOrientationData | Read EDAX EBSD Data or Read Oxford Instr. EBSD Data  |
| RemoveJitter |  Removed from distribution<sup>4</sup>  |
| RenameCellArray | Rename Attribute Array |
| RenameFaceArray | Rename Attribute Array |
| RenameFieldArray | Rename Attribute Array |
| RenameSurfaceMeshFieldArray | Rename Attribute Array |
| RotateSliceEulerRefFrame | Rotate Euler Reference Frame |
| SampleVolume | Removed from distribution<sup>4</sup>  |
| SaveImages | Write Images |
| ScalarSegmentGrains | Segment Features (Scalar) |
| SegmentBoundaries |  Removed from distribution<sup>4</sup> |
| SharedGrainFaceFilter | Generate Triangle Face Ids |
| SingleThresholdCells | Threshold Objects |
| SingleThresholdFields | Threshold Objects |
| SolidMeshToVtk | Removed from distribution <sup>3</sup> |
| SurfaceMeshToNodesTrianglesEdges | Removed from distribution <sup>3</sup> |
| SurfaceMeshToNonconformalVtk | Removed from distribution <sup>3</sup> |
| SurfaceMeshToStl | Write STL Files from Triangle Geometry |
| SurfaceMeshToVtk | Write Vtk PolyData from Triangle Geometry |
| Threshold | Threshold Objects |
| TranslateSurfaceMesh | Change Origin of Volume |
| UnifyTriangleWinding | Removed from distribution <sup>4</sup> |
| UpdateCellQuats | Removed from distribution <sup>3</sup> |
| VerifyTriangleWinding | Removed from distribution <sup>3</sup> |
| VtkGrainIdReader | Removed from distribution <sup>3</sup> |
| VtkRectilinearGridWriter |Removed from distribution <sup>4</sup>|
| YSChoiAbaqusReader | Removed from distribution <sup>3</sup> |



[0] Test files are not available to verify proper output is being created  
[1] Introduction of dedicated **Geometry** classes allow us to remove this **Filter**
[2] Original developer is not updating this **Filter** any longer
[3] This **Filter** is considered legacy and no longer supported 
[4] This **Filter** is removed for technical reasons 

+ Several improvements to the user interface have been made. Consult the updated user manual for details. Most importantly, DREAM.3D now utilizes a document model for its interface, which will allow users to have multiple instances of DREAM.3D open at once, all operating independently.
+ The core code for orientation transformations has been redone based on the work of M. De Graef et al.
+ Many major and minor bug issues addressed.

# Known Issues #

+ The DDDAnalysisToolbox, ImageProcessing, HEDMAnalysis, and TransformationPhase curated **Plugins** are temporarily moved to the _Unsupported_ group until they are updated to the current DREAM.3D standards for code and documentation. These **Plugins** will be moved out of _Unsupported_ as they are updated.
+ The "rolled" preset dialog does not currently verify if the user has entered values such that A > B > C, which may result in incorrect statistics. The user should be aware of this and make sure to enter values such that A > B > C. A check of this condition will be added in a future release.
+ The Match Crystallography **Filter** may intermittenly crash.



