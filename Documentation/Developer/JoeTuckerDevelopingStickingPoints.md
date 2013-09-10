# Joe Tucker Developing Sticking Points #
=========
+ When trying to make a new filter show up in the DREAM.3D GUI (after copying the code from another filter) I had to change getHumanLabel to a unique name that I wanted to show up in the DREAM.3D GUI
+ I was not sure if I had to rerun cmake every time a edit a file (you don't have to) unless building from QTCreater e.g.
+ I was unsure if when manipulating Euler angles (writing a dummy filter to add 5 to every Euler) I had to operate on cell or field data (answer is field data)
+ I was unsure how Euler cell data was stored in an array.  It is stored in a flat array (e0,e1,e2)(e0,e1,e2)....
