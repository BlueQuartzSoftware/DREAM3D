Triangle Writer (GBCd Format) {#gbcdtriangledumper}
=====

## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter does the following creates an ASCII file that contains triangle information that
can then be used in an external GBCD calculation using the GBCD codes from Carnegie Mellon University.
The crystal symmetry and phase index for each phase is also written as additional meta data.

The general format of the file is the following:

  # Triangles Produced from DREAM3D version 4.2.4793
  # Column 1-3:    right hand average orientation (phi1, PHI, phi2 in DEGREES)
  # Column 4-6:    left hand average orientation (phi1, PHI, phi2 in DEGREES)
  # Column 7-9:    triangle normal
  # Column 8:      surface area
  # Column 9,10:   Left hand Phase Id, Right hand Phase Id
  # Phase indexing starts at 1. Phase Zero is for Internal DREAM3D use only
  # Phase: 1
  # Crystal Symmetry: Hexagonal-High 6/mmm
  # Material Name: Magnesium
  # Number Triangles: 12321354
  2.2580 1.4090 4.1832 0.0000 0.0000 0.0000 0.0000 1.0000 -0.0000 0.0312 1 0
  2.2580 1.4090 4.1832 0.0000 0.0000 0.0000 0.0000 1.0000 0.0000 0.0312 1 0
  2.2580 1.4090 4.1832 0.0000 0.0000 0.0000 0.0000 1.0000 -0.0000 0.0312 1 0



## Parameters ##
| Name | Type | Comment |
|------|------|------|
| Output File | Output File Path | |

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | MaterialNames | | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. These values are optional | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Face   | SurfaceMeshFaceAreas | |  | Generate Triangle Areas (SurfaceMeshing) |
| Face   | SurfaceMeshFaceNormals | N x 3 Col of floats |  | Generate Triangle Normals Filter (SurfaceMeshing) |
| Face   | SurfaceMeshPhaseLabels | N x 2 Col of signed integer |  |  |



## Created Arrays ##

None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
