Initialize Synthetic Volume {#initializesyntheticvolume}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
This filter creates an empty volume for inserting features into to create synthetic microstructures.  The user is asked to set the number of **Cells** in each dimension as well as the edge length of each **Cell** (units are typically in microns, but are actually the user's choice).  The user must also specify the path to the file that holds the goal statistics that will be matched durign the synthetic microstructure generation process.  The filter, after reading the stats file, will populate a *ShapeTypes* section, where the user must provide a type of geometrical object to represent the various phases with (i.e. ellipsoids, super-ellipsoids, cubo-octahedra, cylinders).

The Filter produces an estimate of the number of **Fields** in the volume associated with the
values the user entered.


## Parameters ##

| Name | Type |
|------|------|
| Statistics File | Input File |
| X Cells | Integer |
| Y Cells | Integer |
| Z Cells | Integer |
| X Res | Double |
| Y Res | Double |
| Z Res | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | ShapeTypes | Enumeration (int) specifying the shape class of the **Fields** of each **Ensemble** (Ellipsoid=0, SuperEllipsoid=1, CubeOctahedron=2, Cylinder=3) | Values are obtained from user input on GUI |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase each Ensemble is (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) | Values are obtained from input .dream3d file |
| Ensemble | CrystalStructures | Enumeration (int) specifying the symmetry of the **Fields** of each **Ensemble** (Hexagonal=0, Cubic=1, OrthoRhombic=2) | Values are obtained from input .dream3d file |
| Ensemble | Statistics |  | Values are obtained from input .dream3d file |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** |  |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



