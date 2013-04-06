Creating a Pipeline {#creatingpipeline}
=========


### Building a Pipeline
In order the build a pipeline the user can either double click on a particular filter or drag the filter from the _Filter List_ into the _Pipeline Area_.


![](Images/UI_3.png)
@image latex Images/UI_3.png "User Interface" width=5in

After placing a filter into the Pipeline area the user interface will look like the next figure. The user should note the red outline of the filter widget which indicates that one or more of the input parameters has an error. By looking at the error table the user can read the error message from the pipeline and act accordingly. In the figure shown the error message is indicating that the input file for the filter is missing or has not been filled into the Text Edit (the widget with the Red outline).


![](Images/UI_4.png)
@image latex Images/UI_4.png "User Interface" width=5in


The user can place multiple filters into the pipeline to generate a workflow of data and analysis filters culminating in the output of one or several data files. The user can reorder the filters by simply using drag-and-drop of a selected filter in the pipeline and moving it into a new position in the pipeline. As filters are placed into the pipeline a basic error checking algorithm called the **Preflight** is executed on the entire pipeline. During the preflight stage the pipeline will have each filter check its input parameters and the availability needed data arrays. If the user places a filter that requires data that is **not** available an error will be added to the error table. The user will need to insert a filter into the pipeline before the error'ed filter that generates the missing data array.
