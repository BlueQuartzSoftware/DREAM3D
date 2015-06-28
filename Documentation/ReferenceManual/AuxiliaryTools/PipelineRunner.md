PipelineRunner {#pipelinerunner}
========

**PipelineRunner** is a _terminal_ or _command line_ application that is designed to simply run a DREAM.3D **Pipeline** without the use of a GUI. Because of this design, **PipelineRunner** can be launched through a terminal or command prompt or through other ways such as directly from a shell script (Linux), batch file (Windows), Python script, IDL script or MATLAB script.

**PipelineRunner** only takes a single argument: the path to the **Pipeline** file (i.e., a .json file). An example of its usage is:


### Unix Terminal ###

	[user@machine] $ ./PipelineRunner -p /Some/Path/to/Your/Pipeline.json
	

### Windows Command Prompt ###

	[ user ] > PipelineRunner -p C:/Some/Path/To/Your/Pipeline.json
	

Any output from the **Filters** will be printed to the console. This includes progress information, which can make the output very long for some pipelines. Using advanced shell or batch file techniques the user can "pipe" or redirect the output to a log file of their choosing.

## Use Cases ##

There are several use cases for **PipelineRunner**. The first is running DREAM.3D **Pipelines** from another environment such as Python or MATLAB. Other uses include having another program systematically generate a **Pipeline** file and the have **PipelineRunner** execute that **Pipeline**. This workflow can be useful for performing a parametric study on specific **Filters** or studying how inputs might affect the output of a **Filter**.