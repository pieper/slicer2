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
# FILE:        FSLReaderPlot.tcl
# PROCEDURES:  
#   FSLReaderPopUpPlot y)
#   FSLReaderRetrieveTimeCourse
#   FSLReaderCloseTimeCourseWindow
#   FSLReaderGetDataVolumeDimensions
#   FSLReaderGetVoxelFromSelection y)
#   FSLReaderCheckSelectionAgainstVolumeLimits the
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC FSLReaderPopUpPlot
# This routine pops up a plot of a selected voxel's response over
# time, overlayed onto a reference signal. This reference may be the
# experimental protocol, or the protocol convolved with a hemodynamic
# response function.
# .ARGS
# (x, y) the selected point
# .END
#-------------------------------------------------------------------------------
proc FSLReaderPopUpPlot {x y} {
    global FSLReader

    if {! [info exists FSLReader(firstMRMLid)] ||
        ! [info exists FSLReader(lastMRMLid)]} {
        # DevErrorWindow "Please load filtered_func_data.hdr first."
        return
    }

    set ext [[Volume($FSLReader(firstMRMLid),vol) GetOutput] GetWholeExtent]
    set FSLReader(volextent) $ext 

    # Get the indices of selected voxel. Then, check
    # these indices against the dimensions of the volume.
    # If they're good values, assemble the selected voxel's
    # time-course, the reference signal, and plot both.
    scan [FSLReaderGetVoxelFromSelection $x $y] "%d %d %d" i j k
    if {$i == -1} {
        return
    }

    scan [FSLReaderGetDataVolumeDimensions] "%d %d %d %d %d %d" \
        xmin ymin zmin xmax ymax zmax

    # Check to make sure that the selected voxel
    # is within the data volume. If not, return.
    set argstr "$i $j $k $xmin $ymin $zmin $xmax $ymax $zmax"
    if { [ FSLReaderCheckSelectionAgainstVolumeLimits $argstr] == 0 } {
        # DevErrorWindow "Selected voxel not in volume."
        return 
    }

    FSLReaderRetrieveTimeCourse $i $j $k  

    # Plot the time course
    if {! [info exists FSLReader(timeCourseToplevel)]} {
        set w .tcren
        toplevel $w
        wm title $w "Voxel Time Course"
        wm minsize $w 650 250
        wm geometry $w +315+300 
        set FSLReader(timeCourseToplevel) $w

        vtkTimeCoursePlotActor2 tcPlot
        tcPlot SetVoxelIndex $i $j $k
        tcPlot SetPlot $FSLReader(timeCourse) \
            FSLReader($FSLReader(currentModelName),model) 

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
        wm protocol $w WM_DELETE_WINDOW "FSLReaderCloseTimeCourseWindow" 
        set FSLReader(tcPlot) tcPlot
        set FSLReader(render) render
        set FSLReader(renWin) renWin
    }

    $FSLReader(tcPlot) SetVoxelIndex $i $j $k
    $FSLReader(tcPlot) SetPlot $FSLReader(timeCourse) \
        FSLReader($FSLReader(currentModelName),model) 

    #Update the graph for the new data
    $FSLReader(renWin) Render 

    # timer to remove the time course plot
    if {[info exists FSLReader(timer)]} {
        after cancel $FSLReader(timer)
    }
    set FSLReader(timer) [after 6000 {FSLReaderCloseTimeCourseWindow}]
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderRetrieveTimeCourse
# Returns time course data for a specified voxel
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderRetrieveTimeCourse {i j k} {
    global FSLReader Volume

    if {[info exists FSLReader(timeCourse)]} {
        $FSLReader(timeCourse) Delete
        unset -nocomplain FSLReader(timeCourse)
    } 

    vtkFloatArray timeCourse 
    timeCourse SetNumberOfTuples $FSLReader(noOfAnalyzeVolumes) 
    timeCourse SetNumberOfComponents 1

    set start $FSLReader(firstMRMLid) 
    set end $FSLReader(lastMRMLid)

    set ii 0
    while {$start <= $end} {

        set vol [Volume($start,vol) GetOutput]
        $vol Update
        set val [$vol GetScalarComponentAsFloat $i $j $k 0]
        timeCourse SetComponent $ii 0 $val 

        incr ii
        incr start
    }

    set FSLReader(timeCourse) timeCourse
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderCloseTimeCourseWindow
# Cleans up if the time course window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderCloseTimeCourseWindow {} {
    global FSLReader

    $FSLReader(tcPlot) Delete
    $FSLReader(render) Delete
    [$FSLReader(renWin) GetInteractor] Delete

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
    $FSLReader(renWin) Delete

    destroy $FSLReader(timeCourseToplevel)
    unset FSLReader(timeCourseToplevel)
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderGetDataVolumeDimensions
# Gets volume dimensions 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderGetDataVolumeDimensions {} {
    global Volume FSLReader

    set ext $FSLReader(volextent) 

    set xmin [lindex $ext 0]
    set xmax [lindex $ext 1]
    set ymin [lindex $ext 2]
    set ymax [lindex $ext 3]
    set zmin [lindex $ext 4]
    set zmax [lindex $ext 5]

    return "$xmin $ymin $zmin $xmax $ymax $zmax"
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderGetVoxelFromSelection
# Gets voxel index from the selection 
# .ARGS
# (x, y) the selected point 
# .END
#-------------------------------------------------------------------------------
proc FSLReaderGetVoxelFromSelection {x y} {
    global FSLReader Interactor Gui
    
    # Which slice was picked?
    set s $Interactor(s)
    if {$s == ""} {
        DevErrorWindow "No slice was picked."
        return "-1 -1 -1"
    }

    set bvName [[[Slicer GetBackVolume $s] GetMrmlNode] GetName]

    set index [string first "filtered_func_data" $bvName 0]
    if {$index != 0} {
        # DevErrorWindow "The background volume is not filtered_func_data.hdr."
        return "-1 -1 -1"
    }

    set fvName [[[Slicer GetForeVolume $s] GetMrmlNode] GetName]
    set start [string first "_" $fvName 0]
    set end [string first "-" $fvName 0]
    set name [string range $fvName [expr $start + 1] [expr $end - 1]]

    if {[info command FSLReader($name,model)] == ""} {
        return "-1 -1 -1"
    }

    set FSLReader(currentModelName) $name

    set xs $x
    set ys $y

    # Which xy coordinates were picked?
    scan [MainInteractorXY $s $xs $ys] "%d %d %d %d" xs ys x y
    # puts "Click: $s $x $y"

    # Which voxel index (ijk) were picked?
    $Interactor(activeSlicer) SetReformatPoint $s $x $y
    scan [$Interactor(activeSlicer) GetIjkPoint]  "%g %g %g" i j k
    # puts "Voxel coords: $i $j $k"

    # Let's snap to the nearest voxel
    set i [expr round ($i) ]
    set j [expr round ($j) ]    
    set k [expr round ($k) ]
    # puts "Rounded voxel coords: $i $j $k"
    puts "Voxel coords: $i $j $k"
    
    return "$i $j $k"
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderCheckSelectionAgainstVolumeLimits
# Checks voxel selection against volume limits 
# .ARGS
# argstr the data string
# .END
#-------------------------------------------------------------------------------
proc FSLReaderCheckSelectionAgainstVolumeLimits {argstr} {

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
