Write Feature Data as CSV File {#featuredatacsvwriter}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes the data associated with each **Feature** to a file name specified by the user in *CSV* format. Every array in the **Feature** map is written as a column of data in the *CSV* file.  The user can choose to also write the neighbor data. Neighbor data are data arrays that are associated with the neighbors of a **Feature**, such as: list of neighbors, list of misorientations, list of shared surface areas, etc. These blocks of info are written after the scalar data arrays.  Since the number of neighbors is variable for each **Feature**, the data is written as follows (for each **Feature**): Id, number of neighbors, value1, value2,...valueN.


### Example Output ###
The *CSV* file:     
	 
	2352														
    Feature_ID	Active	AvgEulerAngles_0	AvgEulerAngles_1	AvgEulerAngles_2	AvgQuats_0	AvgQuats_1	AvgQuats_2	AvgQuats_3	EquivalentDiameters	NumCells	NumNeighbors	ParentIds	Phases	Volumes
    1	1	0.915113	0.790951	-0.89174	0.238438	0.302593	0.010785	0.92275	2.19832	356	8	947	1	5.5625
    2	1	1.20286	0.829786	-0.72003	0.230703	0.330544	0.218794	0.88862	1.88375	224	8	1673	1	3.5
    3	1	-0.65165	0.787782	0.261665	0.344458	-0.16923	-0.17892	0.905923	0.781593	16	7	1301	1	0.25
    4	1	-2.6596	0.629984	2.50802	-0.26285	-0.16398	-0.07199	0.94807	4.61277	3289	22	1818	1	51.3906
    5	1	0.140752	0.727944	-0.48842	0.338518	0.110151	-0.16163	0.920407	3.05771	958	12	535	1	14.9688
    6	1	-1.303	0.552201	1.37898	0.062097	-0.26544	0.03654	0.961432	1.03357	37	3	1026	1	0.578125
    7	1	0.978054	0.717843	-0.86783	0.21198	0.280091	0.051574	0.934855	0.984745	32	6	576	1	0.5

The *CSV* file with Write Neighbor Data checked:

    2352														
    Feature_ID	Active	AvgEulerAngles_0	AvgEulerAngles_1	AvgEulerAngles_2	AvgQuats_0	AvgQuats_1	AvgQuats_2	AvgQuats_3	EquivalentDiameters	NumCells	NumNeighbors	ParentIds	Phases	Volumes
    1	1	0.915113	0.790951	-0.89174	0.238438	0.302593	0.010785	0.92275	2.19832	356	8	947	1	5.5625
    2	1	1.20286	0.829786	-0.72003	0.230703	0.330544	0.218794	0.88862	1.88375	224	8	1673	1	3.5
    3	1	-0.65165	0.787782	0.261665	0.344458	-0.16923	-0.17892	0.905923	0.781593	16	7	1301	1	0.25
    4	1	-2.6596	0.629984	2.50802	-0.26285	-0.16398	-0.07199	0.94807	4.61277	3289	22	1818	1	51.3906
    5	1	0.140752	0.727944	-0.48842	0.338518	0.110151	-0.16163	0.920407	3.05771	958	12	535	1	14.9688
    6	1	-1.303	0.552201	1.37898	0.062097	-0.26544	0.03654	0.961432	1.03357	37	3	1026	1	0.578125
    7	1	0.978054	0.717843	-0.86783	0.21198	0.280091	0.051574	0.934855	0.984745	32	6	576	1	0.5
    8	1	-3.10085	0.678288	-2.69658	-0.32591	0.066789	0.226799	0.915361	2.51958	536	12	627	1	8.375


## Parameters ##

| Name | Type |Description |
|------|------|------|
| Output File | File Path | The output .csv file path |
| Write Neighbor Data | bool | Whether to write the **Feature** neighbor data |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | None | Feature | N/A | **Feature Attribute Matrix** that holds the data write |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


