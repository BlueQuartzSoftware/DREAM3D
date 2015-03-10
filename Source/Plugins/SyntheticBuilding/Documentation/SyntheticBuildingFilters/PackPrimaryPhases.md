Pack Primary Phases {#packprimaryphases}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
If there are primary phases in the Statistics file, then this filter will place primary **Features** with the sizes, shapes, physical orientations and locations corresponding to the goal statistics.  The user can specify if they want *periodic boundary conditions* and whether they would like to write out the goal attributes of the generated **Features**. 


## Parameters ##

| Name | Type |
|------|------|
| Periodic Boundary | Boolean (On or Off) |
| Write Goal Attributes | Boolean (On or Off) |
| Goal Attributes CSV File | Output File |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |
| Ensemble | ShapeTypes |  |  | Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | Statistics |  |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | Active |  |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

