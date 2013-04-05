Find Axis ODF {#}
======
<h1 class="pHeading1">Find Axis ODF Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindAxisODF.html#wp2">Description</a> | <a href="../StatisticsFilters/FindAxisODF.html#wp3">Options</a> | <a href="../StatisticsFilters/FindAxisODF.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindAxisODF.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindAxisODF.html#wp1">Authors</a> 

Statistics Filters


The ODF is defined as the volume fraction of **Fields** with a certain orientation vector.
The orientation vector is normally identified using three _Euler_ angles which are used to describe the transition from the _sample_’s reference frame into the _crystallographic_ reference frame of each individual **Field** of the polycrystal. One thus ends up with a large set of different _Euler_ angles, the distribution of which is described by the ODF.
This Filter searches and identifies the ODF axis of the volume fraction of **Fields**.


## Parameters ##

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | AxisEulerAngles |  |
| Field | FieldPhases |  |
| Field | SurfaceFields |  |

## Created Arrays ##
None

## Authors ##


**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



