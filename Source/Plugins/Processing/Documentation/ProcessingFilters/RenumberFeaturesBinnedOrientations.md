Renumber Features (Binned Orientations) {#renumberfeaturesbinnedorientations}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter assigns an *Id* to each **Feature** according to the orientation bin it belongs to when discretizing orientation space into bins.  The bins are established in homochoric Rodriguez space for equal volume consideration, but the *Bin Width* value (in degrees) is used to eastablish the total number of bins in orientation space.  


## Parameters ##
| Name | Type | Description |
|------|------|------|
| Bin Width | Int | Sets size (in degrees) of orientation bins that **Features** are assigned to |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z) |  | Find Average Orientations (Statistics). |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | ParentIds | Ids (ints) that specify to which bin each **Feature** is placed. |  |

## Authors ##

**Copyright:** 2015 Joseph C. Tucker (UES, Inc.), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

