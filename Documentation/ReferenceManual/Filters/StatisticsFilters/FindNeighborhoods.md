Find Field Neighborhoods {#findneighborhoods}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines the number of **Fields** whose *centroids* lie within a distance of one *Equivalent Sphere Diameter* from a each **Field**.  The algorithm for determining the number of **Fields** is given below:

1. Find the *centroid* of a **Field**.
2. Define a sphere centered at the the **Field**'s *centroid*  and with radius equal to the **Field**'s *Equivalent Sphere Diameter*.
3. Check every other **Field**'s centroid to see if it lies within the sphere and keep count of those that satisfy.
4. Repeat 1-3 for all **Fields**.



## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated |
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Field**. | Filter will calculate equivalent diameters for the **Fields** if not already calculated |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Neighborhoods | Number (int) of **Fields** that have their centroid within one equivalent sphere diameter of each **Field** | **Fields** do not need to actually touch each other, only be within a distance equal to the reference **Field**'s equivalent diameter |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



