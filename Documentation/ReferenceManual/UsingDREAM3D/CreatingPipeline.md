Creating a Pipeline {#creatingpipeline}
=========
In DREAM3D the user processes their data by creating what is known as a _Pipeline_ which is constructed  out of a series of _Filters_. By chaining together a series of filters, the data is processed in quick succession and can ultimately be saved to a number of different file formats. The user should be aware that all processing on the data is done **in place**, i.e., there is only a single copy of the data resident in memory at any one time.

## Building a Pipeline
In order the build a pipeline the user can either double click on a particular filter or drag the filter from the _Filter List_ into the _Pipeline Area_.

![Figure 1: Empty pipeline area.](Images/CreatingPipeline-1.png)

@image latex Images/CreatingPipeline-1.png "Figure 1: Empty pipeline area." width=6in 

The user can place multiple filters into the pipeline to generate a workflow of data input, analysis and data output filters. The user can reorder the filters by simply using drag-and-drop of a selected filter in the pipeline and moving it into a new position in the pipeline. As filters are placed into the pipeline a basic error checking algorithm called the **Preflight** is executed on the entire pipeline. During the preflight stage the pipeline will have each filter check its input parameters and the availability of required data arrays. Figure 2 shows a pipeline ready to execute with no errors.

----------

![Figure 2: Correctly populated pipeline.](Images/CreatingPipeline-2.png)

@image latex Images/CreatingPipeline-2.png "Figure 2: Correctly populated pipeline." width=6in 

----------

![Figure 3: Selected Filter in the pipeline & How to remove the filter.](Images/CreatingPipeline-4.png)

@image latex Images/CreatingPipeline-4.png "Figure 3: Selected Filter in the pipeline & How to remove the filter." width=6in 

----------

To remove a filter from the pipeline the user can use the mouse to click the small **X** icon in the upper left corner of the filter.

If the preflight did not complete successfully, the user will see the offending filters show a red outline and red title area. This indicates that one or more of the input parameters have an error or the filter requires data that will not be available at that point in the pipeline. By looking at the error table, the user can read the error message from the pipeline and act accordingly. An example pipeline with errors is shown in figure 4.

----------

![Figure 4: Errors in the constructed pipeline.](Images/CreatingPipeline-3.png)

@image latex Images/CreatingPipeline-3.png "Figure 4: Errors in the constructed pipeline." width=6in 

-------------

Once the errors are resolved, the user can now execute the pipeline and generate the outputs. Because the DREAM3D native file format stores the complete state of the data arrays, the user can use this idea to add _checkpoints_ to the pipeline in the case of a long running pipeline.
@htmlonly
|   | Navigation |    |
|----|---------|------|
| [Back](userinterface.html) | [Top](usermanual.html) | [Next Section](importexportsavepipeline.html) |
@endhtmlonly