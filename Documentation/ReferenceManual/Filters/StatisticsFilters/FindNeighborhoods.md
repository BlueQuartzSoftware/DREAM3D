Find Field Neighborhoods {#findneighborhoods}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter searches the __Cell__ and __Field__ arrays based on a user choice of distribution types to identify the __Neighborhoods__ of the __Cells__. The types available are: LogNormal, Beta, and Power.

## Parameters ##

| Name | Type |
|------|------|
| Distribution Type | Choices |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated |
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Field**. | Filter will calculate equivalent diameters for the **Fields** if not already calculated |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Neighborhoods | Number (int) of **Fields** that have their centroid within one equivalent sphere diameter of each **Field** | **Fields** do not need to actually touch each other, only be within a distance equal to the reference **Field**'s equivalent diameter |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



