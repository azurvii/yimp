## Introduction ##

This program is aimed to analyze oriented peptide screening blots.


## User Interface ##

There are 5 tabs, designed according to the workflow, from left to right:
  * **Image** is where you can see the blot image.
  * **Inspector** is a space where you can inspect the image and sampling area.
  * **Plot** is used for plotting average intensities.
  * **Matrix** is where the PSSMs are generated.
  * **Console** is textual information output.


## Analysis ##

### Simply put ###

  * Load an image (**Image** tab)
  * Designate the sampling area (**Image** tab)
  * Inspect and modify the sampling area (**Image** and **Inspector** tabs)
  * Review the density plots and decide what selection the motif has at each position (**Plot** tab)
  * Review and export the PSSM (**Matrix** tab)
  * Test the PSSM on [Scansite](http://scansite.mit.edu/) (Use any web browser at your will)


### **Image** tab ###

  * **Image scale** allows you to zoom in/out on the image.
  * **Grid** group box: defines a grid to specify the sampling area. You can use ellipse/circle or rectangle/square grids. The grid is defined by the top-left and bottom-right geometries. To get the coordinates, simply click on the center of the geometry, and the information will show in the status bar (all status bar information will be recorded in **Console** tab). The "Type" combo box tells what grid you want; you can get circle/square grid using the check box below. To define how large the grid is, you can input the numbers in the following fields; or you can click the "Row & col" button, input row and column labels in the pop-up dialog, and the dimension of the grid will be calculated according to your input (**NOTE:** use white spaces (space, tab, etc.) to separate the labels, which means you cannot use spaces in your label names).  Some following functions are relied on the labels. The software will memorize the row and column labels, so you need to enter it once for repeating images. You can also rotate the grid around the top-left point, by modifying the "Rotation" parameter.
  * **Patch** is used to refine the sampling grid. Basically, patches do the opposite job of "grid". While grid defines where should be included in consideration, patches define unwanted areas. Currently only a polygon shaped tool is implemented. Select "Polygon", single clicks on the image designate the polygon path; a double click ends a polygon. (The polygon will not show until you end it) If you made a mistake, the last added polygon can be canceled, through menu "Image" > "Clear last patch". Select "None" to quit patch definition mode.


### **Inspector** tab ###

  * The image will be rasterized and represented in a 3D space. The XY plane is used as a mapping to the coordinates of the image pixels. The Z-value of each point in space represents the gray value of corresponding pixel (if it's a color image, the gray value will be calculated from the RGB values).
  * Red dots (if you see any) are the pixels hitting the boundary values of the gray image (brightest and darkest colors this image format can present). These dots might suggest the over-exposure of this image, and the lose in information.
  * Most dots should be colored in an gradient pattern, from blue(dark in image) to yellow(bright in image).
  * To navigate in the space (better try out yourself):
    * Drag mouse holding left-button: to change where the camera is targeting.
    * Drag mouse holding right-button: to move the camera.
    * Mouse wheel: to move camera up and down.
  * The panel on the right allows you to change mouse sensitivity and the mapping scale from the image to the space.
  * After you click "Apply mask", only the sampling regions are shown here. "Toggle mask" turns on/off the mask.
  * "Ortho view" disables the perspective effects. "Look down Z" allows you to look down z-axis. "Reset view" would be helpful if you are lost in the space.


### **Plot** tab ###

  * This is where the average intensities are plotted.
  * The panel on the right provides some parameters for adjusting the plots.
  * The plots will show after the averages are calculated.
  * The residue excluding function is used to compensate the short of current Scansite, which doesn't recognize phosphorylated residues. This function relies on the column labels. So before using it, ensure that the column labels are properly set in **Image** tab.


### **Matrix** tab ###

  * Here lays the thing we are really after, the PSSM.
  * Specify the central residue(s) in the right text box (this also need properly labeled columns). Then click "Update matrix", the matrix displayed would be updated according to the central residues.


### **Console** tab ###

  * Textual information is provided here.
  * Key events are headed with `**`, and errors will be headed with `!!`.