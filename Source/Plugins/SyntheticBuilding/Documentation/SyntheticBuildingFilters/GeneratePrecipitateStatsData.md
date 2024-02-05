Generate Precipitate StatsData 
=============

## Group (Subgroup) ##

StatsGenerator (StatsGenerator)

## Description ##

This filter allows the user to generate statistics for a **Precipitate** phase that is suitable for use as an input into
the @see InsertPrecipitates filter when generating a synthetic microstructure. The goal of this particular filter is in
the use/generation of automated pipeline files. In this scenario the power user is wanting to generate a varied array
of microstructures using some sort of scripting environment or language. By careful replacements of the values in a pipeline
file the user can automate the generation of a synthetic microstructure. If the use wishes to have a more interactive
environment in which to explore the various distribution curves then the @see **StatsGenerator** filter should be used instead.

### Important Note ###

Getting the phase fraction correct between multiple instances of this filter (and of the Generate Precipitates Stats Data filter)
is the responsibility of the user. All phase fractions from **ALL** instances of this filter and the 'Generate Precipitates Stats
Data Filter' should = 1.0.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Parameter Name | Parameter Type | Description of parameter... |

## Required Geometry ###

Required Geometry Type -or- Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Element/Feature/Ensemble/etc. Attribute Array** | AttributeArray Name | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Element/Feature/Ensemble/etc. Attribute Array** | AttributeArray Name | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
