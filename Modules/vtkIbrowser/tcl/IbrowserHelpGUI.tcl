#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        IbrowserHelpGUI.tcl
# PROCEDURES:  
#   IbrowserBuildHelpFrame
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserBuildHelpFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildHelpFrame { } {
    #-------------------------------------------
    #--- Help frame
    #-------------------------------------------
    set help " The <B> Ibrowser </B> is still in early form; bug fixes, new viewing and recording functions, 
processing options, more flexible load and populate options, and the ability to load models, label maps and other 
data will be added in the future. The Ibrowser build has been tested on Windows and Solaris, not yet under Linux or OS X. 
Some known bugs exist in the canvas scrolling, animation controls, and slice visibility options.<P> 
 The <B> Ibrowser </B> provides a way to view time-series, multi-channel, 
and other sequences or collections of data in the Slicer. Intervals which contain the data, 
as well as individual data 'drops' are represented on an independent control panel. 
Selected data is displayed, manipulated and animated in the Slicer's Main Viewer. 
<P> <B>New</B> lets you create and populate intervals. The Ibrowser currently reads 
only Analyze format images, and will create a sequence out of all files within a directory. 
More loading options will be available soon, including the ability to select volumes already loaded 
into slicer to populate an interval. <P> <B>Display</B> options allow 
you to view a sequence or collection, and to adjust window, level and threshold 
for the sequence. <P> <B>Process</B> options allow you to perform general processing 
operations on the sequence.  A suite of options is being developed.<P> <B>Save</B> options 
will let you save a sequence or collection; currently there is no way to save. 
<P> <B>Delete</B> lets you delete sequences or collections. <P> 
 The <B>Controller</B> lets you scroll through the volumes in an interval; change the 
interval's name; change the order of intervals; put one interval in the foreground (FG) and 
one in the background (BG); toggle the visibility in the FG and BG together; copy the contents of 
one interval to another; delete an interval and its contents; and specify whether the last 
volume within an interval should be maintained or turned off in the viewer when the index 
is scrolled beyond its index. Clicking on an interval outlines it in red and marks it as the active 
interval to which processing is applied. The active interval may be different from the FG or BG 
intervals. <P> The Controller also has a set of <B> animation controls </B>, which 
enable (from left to right) single frame stepping; zoom in and out (not yet working); goto first volume; play 
backward; stop; record (not yet working); pause; play forward; goto last volume; continuous animation loop; 
and continous animation ping-pong (both of which are stopped with the stop button). 
The controller also has a scrolled message box in which system messages are displayed.<P> "
    
    regsub -all "\n" $help {} help
    MainHelpApplyTags Ibrowser $help
    MainHelpBuildGUI Ibrowser

}

