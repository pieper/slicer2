#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for internal 
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
# FILE:        MainRender.tcl
# PROCEDURES:  
#   Render3D
#   RenderSlice
#   RenderActive
#   RenderSlices
#   RenderAll
#   RenderBoth
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC Render3D
# 
# If $View(movie) > 0, saves the frame for a movie
#
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Render3D {{scale ""}} {
    global Video viewWin Twin twinWin Save View Slice

    # don't render when starting/stopping the program
    if { $View(render_on) == 1 } {
        return
    }
    # Apply the fog parameters to all the renderers of viewWin
    FogApply $viewWin

    
    set rens [$viewWin GetRenderers]
    set rencount [$rens GetNumberOfItems] 
    for {set r 0} {$r < $rencount} {incr r} {
        set ren [$rens GetItemAsObject $r]
    # don't reset clipping planes for the endoscopic
    # screen, otherwise it does not look good when
    # the endoscope is inside a model
    if {$ren != "endoscopicScreen"} {
         # wrap this in global flag to avoid possible render loop
             if {$View(resetCameraClippingRange) == 1} {
                 $ren ResetCameraClippingRange    
             }

    }  
    }

    $viewWin Render
    
    if {[IsModule Twin] == 1 && $Twin(mode) == "On"} {
        Twin(src) Modified
        Twin(src) Update
        Twin(mapper) Modified
        $twinWin Render
    }

    if {[IsModule Video] == 1 && $Video(record) == "On"} {
        VideoSave
    }

    if { [SaveModeIsMovie] } {
        Save3DImage
    }
}

#-------------------------------------------------------------------------------
# .PROC RenderSlice
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderSlice {s {scale ""}} {
    global Slice View Interactor

    sl${s}Win Render

    if {$s == $Interactor(s)} {
        if {$View(createMagWin) == "Yes" && $View(closeupVisibility) == "On"
                && [info command magWin] != "" } {
            magWin Render
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC RenderActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderActive {{scale ""}} {
    global Slice 

    RenderSlice $Slice(activeID)
}

#-------------------------------------------------------------------------------
# .PROC RenderSlices
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderSlices {{scale ""}} {
    global Slice 

    foreach s $Slice(idList) {
        RenderSlice $s
    }
}

#-------------------------------------------------------------------------------
# .PROC RenderAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderAll { {scale ""}} {
    global Slice

    foreach s $Slice(idList) {
        RenderSlice $s
    }
    # render3d last in case we want the newly rendered slices in the movie
    Render3D
    
}
 
#-------------------------------------------------------------------------------
# .PROC RenderBoth
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderBoth {s {scale ""}} {

    RenderSlice $s
    # render3d last in case we want the newly rendered slices in the movie
    Render3D
}
