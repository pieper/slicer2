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
# FILE:        FMRIEnginePlot.tcl
# PROCEDURES:  
#   FMRIEnginePopUpPlot y)
#   FMRIEngineCloseTimeCourseWindow
#   FMRIEngineGetDataVolumeDimensions
#   FMRIEngineGetVoxelFromSelection y)
#   FMRIEngineCheckSelectionAgainstVolumeLimits the
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC FMRIEnginePopUpPlot
# This routine pops up a plot of a selected voxel's response over
# time, overlayed onto a reference signal. This reference may be the
# experimental protocol, or the protocol convolved with a hemodynamic
# response function.
# .ARGS
# (x, y) the selected point
# .END
#-------------------------------------------------------------------------------
proc FMRIEnginePopUpPlot {x y} {
    global FMRIEngine

    # Checks time course plotting option
    switch $FMRIEngine(tcPlottingOption) {
        "None" {
            return
        }
        "Long" {
            puts "Long"
        }
        "Short" {
            puts "Short"
        }
        "ROI" {
            puts "ROI"
        }
    }

    if {! [info exists FMRIEngine(firstMRMLid)] ||
        ! [info exists FMRIEngine(lastMRMLid)]} {
        # DevErrorWindow "Please load volume sequence first."
        return
    }

    # Get the indices of selected voxel. Then, check
    # these indices against the dimensions of the volume.
    # If they're good values, assemble the selected voxel's
    # time-course, the reference signal, and plot both.
    scan [FMRIEngineGetVoxelFromSelection $x $y] "%d %d %d" i j k
    if {$i == -1} {
        return
    }

    scan [FMRIEngineGetDataVolumeDimensions] "%d %d %d %d %d %d" \
        xmin ymin zmin xmax ymax zmax

    # Check to make sure that the selected voxel
    # is within the data volume. If not, return.
    set argstr "$i $j $k $xmin $ymin $zmin $xmax $ymax $zmax"
    if { [ FMRIEngineCheckSelectionAgainstVolumeLimits $argstr] == 0 } {
        # DevErrorWindow "Selected voxel not in volume."
        return 
    }

    # Plot the time course
    if {[info exists FMRIEngine(timeCourseToplevel)] == 0 } {
        set w .tcren
        toplevel $w
        wm title $w "Voxel Time Course"
        wm minsize $w 650 250
        wm geometry $w +315+300 
        set FMRIEngine(timeCourseToplevel) $w

        vtkTimeCoursePlotActor tcPlot
        tcPlot SetVoxelIndex $i $j $k
        tcPlot SetPlot [FMRIEngine(actvolgen) GetTimeCourse $i $j $k] \
        $FMRIEngine(stimulus) 

        vtkRenderer render
        render AddActor2D tcPlot 
        render SetBackground 1.0 1.0 1.0
        render SetViewport 0 0 1 1
           vtkRenderWindow renWin
        renWin AddRenderer render

        set vtkw [vtkTkRenderWidget .tcren.rw \
            -width 650 \
            -height 250 \
            -rw renWin]
        ::vtk::bind_tk_render_widget $vtkw  
        pack $vtkw -side top -fill both -expand yes     
        wm protocol $w WM_DELETE_WINDOW "FMRIEngineCloseTimeCourseWindow" 
        set FMRIEngine(tcPlot) tcPlot
        set FMRIEngine(render) render
        set FMRIEngine(renWin) renWin
    }

    $FMRIEngine(tcPlot) SetVoxelIndex $i $j $k
    $FMRIEngine(tcPlot) SetPlot [FMRIEngine(actvolgen) GetTimeCourse $i $j $k] \
        $FMRIEngine(stimulus) 

    #Update the graph for the new data
    $FMRIEngine(renWin) Render 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCloseTimeCourseWindow
# Cleans up if the time course window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCloseTimeCourseWindow {} {
    global FMRIEngine

    # timer to remove the time course plot
    if {[info exists FMRIEngine(timer)]} {
        after cancel $FMRIEngine(timer)
    }

    $FMRIEngine(tcPlot) Delete
    $FMRIEngine(render) Delete
    [$FMRIEngine(renWin) GetInteractor] Delete

    # I found some useful information from "vtkusers mailing list" about
    # some problems on destroying a vtkTkRenderWidget. Here is the respone
    # from Hideaki Hiraki:
    # 1. "[$renWin GetInteractor] Delete" is required if the 
    # vtkGenericRenderWindowInteractor is created implicitly in 
    # ::vtk::bind_tk_render_widget.
    # 2. "$renWin MakeCurrent" is sometimes required (maybe when 
    # the vtkRenderWindow is created implicitly in the widget).
    # 3. "rename $widget {}" is required as "destroy $widget" is 
    # not enough to remove the command.
    $FMRIEngine(renWin) Delete

    destroy $FMRIEngine(timeCourseToplevel)
    unset FMRIEngine(timeCourseToplevel)
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineGetDataVolumeDimensions
# Gets volume dimensions 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineGetDataVolumeDimensions {} {
    global Volume FMRIEngine

    set ext $FMRIEngine(volextent) 

    set xmin [lindex $ext 0]
    set xmax [lindex $ext 1]
    set ymin [lindex $ext 2]
    set ymax [lindex $ext 3]
    set zmin [lindex $ext 4]
    set zmax [lindex $ext 5]

    return "$xmin $ymin $zmin $xmax $ymax $zmax"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineGetVoxelFromSelection
# Gets voxel index from the selection 
# .ARGS
# (x, y) the selected point 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineGetVoxelFromSelection {x y} {
    global FMRIEngine Interactor Gui
    
    # Which slice was picked?
    set s $Interactor(s)
    if {$s == ""} {
        DevErrorWindow "No slice was picked."
        return "-1 -1 -1"
    }

    set fvName [[[Slicer GetForeVolume $s] GetMrmlNode] GetName]
    if {! [info exists FMRIEngine(actVolName)] ||
        $fvName != $FMRIEngine(actVolName)} {
        return "-1 -1 -1"
    }

    set xs $x
    set ys $y

    # Which xy coordinates were picked?
    scan [MainInteractorXY $s $xs $ys] "%d %d %d %d" xs ys x y
    puts "Click: $s $x $y"

    # Which voxel index (ijk) were picked?
    $Interactor(activeSlicer) SetReformatPoint $s $x $y
    scan [$Interactor(activeSlicer) GetIjkPoint]  "%g %g %g" i j k
    puts "Voxel coords: $i $j $k"

    # Let's snap to the nearest voxel
    set i [expr round ($i) ]
    set j [expr round ($j) ]    
    set k [expr round ($k) ]
    puts "Rounded voxel coords: $i $j $k"
    
    return "$i $j $k"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCheckSelectionAgainstVolumeLimits
# Checks voxel selection against volume limits 
# .ARGS
# argstr the data string
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCheckSelectionAgainstVolumeLimits {argstr} {

    scan $argstr "%d %d %d %d %d %d %d %d %d" i j k xmin ymin zmin xmax ymax zmax
    puts "argstr = $argstr"
    if {$i < $xmin || $j < $ymin || $k < $zmin} {
        return 0 
    }
    if {$i > $xmax || $j > $ymax || $k > $zmax} {
        return 0 
    }

    return 1 
}
