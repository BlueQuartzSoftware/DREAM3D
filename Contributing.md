## Contributing to DREAM.3D ##
The DREAM.3D project embraces community contributions, and desires to keep the process of contributing changes as easy as possible.  There are a few guidelines contributors should follow; adhering to these guidelines will make the contribution process faster and easier.

## How to Contribute ##
The DREAM.3D project utilizes the [fork and pull](https://help.github.com/articles/using-pull-requests/) model for development.  There are a few initial steps to follow to prepare for this model of contribution:

+ Sign up for a [GitHub account](https://github.com/signup/free)
+ Create an issue on the DREAM.3D issue tracker for your expected contribution, if one does not already exist
	+ In the issue, please describe the scope of the intended contribution and apply the proper label
	+ If the issue is a bug, please include steps necessary to reproduce the bug
+ Fork the DREAM.3D repository into your own GitHub workspace
+ Create a branch that will encapsulate the work that will address the issue
	+ The branch should be named with reference to the number of the issue 
	+ For example, for an issue about "Foo" with number "123", the branch should be named "123_foo"
+ Make any commits to this branch to address the issue in logical, atomic units
	+ Please follow established DREAM.3D style when writing your code for contribution
	+ Primary DREAM.3D styles include tabs as two spaces, camel case for function naming, and the "m_" naming scheme for instance variables
+ Format your commit messages to reference the open issue
+ Add any necessary unit tests for your additions
+ Ensure that your changes compile on your development platform for the most recent HEAD of the DREAM.3D develop branch
+ Ensure that all DREAM.3D unit tests complete without failure

## Submitting Contributions ##

+ Sign the [Contributor License Agreement](http://dream3d.bluequartz.net/?page_id=448)
+ Submit a pull requests from your remote branch to the DREAM.3D project
	+ If your pull request contains several commits, please squash them into a single commit
+ Update the open issue with a link to the submitted pull request
+ Pull requests are reviewed bi-weekly by at least two core DREAM.3D developers.  The DREAM.3D development team will respond to the pull request with any questions or comments as required by the content of the pull request.  If no action is required, the pull request will be merged immediately.
+ After giving any feedback, the DREAM.3D developers will allow two weeks for response from the contributor.  If no further action is taken by the contributor, the DREAM.3D development team may close the pull request as inactive.


## Submission of Curated Plugins ##

If you have a **Plugin** you would like to see distributed with the DREAM.3D binary, but do not want it merged into the main DREAM.3D repository, you can request to have that **Plugin** become _curated_. The DREAM.3D development team will then store that **Plugin's** source at our parent [GitHub page](https://github.com/dream3d/). This allows developers to maintain control over their source code while still having their work distributed with the main DREAM.3D application to the public. The DREAM.3D development team will not provide any public support or contributions to a curated **Plugin** unless tasked to do so by the **Plugin** maintainers.  Curated **Plugins** must adhere certain standards with regards to documentation, content, and unit testing.  For more information on curated **Plugins**, please contact the DREAM.3D development team.