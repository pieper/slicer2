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
#   FMRIEngineDrawPlotShort y,
#   FMRIEngineShowData a
#   FMRIEngineCreateCurvesFromTimeCourse j,
#   FMRIEngineDrawPlotLong y,
#   FMRIEngineCloseDataWindow
#   FMRIEngineCloseTimeCourseWindow
#   FMRIEngineGetDataVolumeDimensions
#   FMRIEngineGetVoxelFromSelection y)
#   FMRIEngineCheckSelectionAgainstVolumeLimits the
#==========================================================================auto=
#===============================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
#===============================================================================
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

    # error if no private segment
    if { [catch "package require BLT"] } {
        DevErrorWindow "Must have the BLT package for time course plotting."
        return
    }

    if {! $FMRIEngine(trackMotion)} {
        return
    }

    # Checks time course plotting option
    switch $FMRIEngine(tcPlottingOption) {
        "None" {
            return
        }
        "Long" {
            set plotTitle "Voxel Time Course - Natural"
            set plotWidth 500
            set plotHeight 250 
            set plotGeometry "+385+200"
 
            # set plotWidth 650
            # set plotHeight 250
            # set plotGeometry "+315+300"
        }
        "Short" {
            set plotTitle "Voxel Time Course - Combined"
            set plotWidth 500
            set plotHeight 250 
            set plotGeometry "+385+200"
        }
        "ROI" {
            puts "ROI plotting is being constructed."
            return
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
    if {[ FMRIEngineCheckSelectionAgainstVolumeLimits $argstr] == 0} {
        # DevErrorWindow "Selected voxel not in volume."
        return 
    }

    # Plot the time course
    if {[info exists FMRIEngine(timeCourseToplevel)] == 0 } {
        set w .tcren
        toplevel $w
        wm title $w $plotTitle 
        wm minsize $w $plotWidth $plotHeight
        wm geometry $w $plotGeometry 

        blt::graph $w.graph -plotbackground white 
        pack $w.graph 
        $w.graph legend configure -position bottom -relief raised \
            -font fixed -fg black 
        $w.graph axis configure y -title "Intensity"
        # $w.graph grid on
        # $w.graph grid configure -color black

        wm protocol $w WM_DELETE_WINDOW "FMRIEngineCloseTimeCourseWindow" 

        set FMRIEngine(timeCourseToplevel) $w
        set FMRIEngine(timeCourseGraph) $w.graph
    }

    if {$FMRIEngine(tcPlottingOption) == "Short"} {
        $FMRIEngine(timeCourseGraph) axis configure x \
            -title "Combined Condition/Baseline Volume Number" 
    } else {
        $FMRIEngine(timeCourseGraph) axis configure x -title "Volume Number" 
    }

    FMRIEngineDrawPlot$FMRIEngine(tcPlottingOption) $i $j $k 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineDrawPlotShort
# Draws time course plot in short format 
# .ARGS
# (x, y, z) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineDrawPlotShort {x y z} {
    global FMRIEngine

    # Creates curves from time course
    FMRIEngineCreateCurvesFromTimeCourse $x $y $z
  
    set volsPerBaseline [lindex $FMRIEngine(paradigm) 2]
    set volsPerCondition [lindex $FMRIEngine(paradigm) 3]
    set noVols [expr $volsPerBaseline + $volsPerCondition - 1] 

    blt::vector xVecCon yVecCon xVecBase yVecBase
    set i 0 
    set min [expr $volsPerCondition - 1]
    while {$i < $noVols} {
        if {$i < $volsPerCondition} {
            lappend xAxisCondition [expr $i + 1]
        } 
        if {$i >= $min} { 
            lappend xAxisBaseline [expr $i + 1]
        }
        incr i
    }

    $FMRIEngine(timeCourseGraph) axis configure x -min 1 -max $noVols 
    $FMRIEngine(timeCourseGraph) axis configure y \
        -min $FMRIEngine(timeCourseYMin) -max $FMRIEngine(timeCourseYMax)

    xVecCon set $xAxisCondition
    yVecCon set $FMRIEngine(conditionTCAve)
    xVecBase set $xAxisBaseline
    yVecBase set $FMRIEngine(baselineTCAve)

    set i 0 
    while {$i < $volsPerBaseline} {
        lappend coordsBase $xVecBase($i) 
        lappend coordsBase [lindex $FMRIEngine(baselineTCMax) $i] 
        lappend coordsBase $xVecBase($i) 
        lappend coordsBase [lindex $FMRIEngine(baselineTCMin) $i] 
 
        incr i
    }

    if {[info exists FMRIEngine(conditionCurve)] &&
        [$FMRIEngine(timeCourseGraph) element exists $FMRIEngine(conditionCurve)]} {
        $FMRIEngine(timeCourseGraph) element delete $FMRIEngine(conditionCurve)
    }
    if {[info exists FMRIEngine(baselineCurve)] &&
        [$FMRIEngine(timeCourseGraph) element exists $FMRIEngine(baselineCurve)]} {
        $FMRIEngine(timeCourseGraph) element delete $FMRIEngine(baselineCurve)
    }
    if {[info exists FMRIEngine(voxelIndices)] &&
        [$FMRIEngine(timeCourseGraph) marker exists $FMRIEngine(voxelIndices)]} {
        $FMRIEngine(timeCourseGraph) marker delete $FMRIEngine(voxelIndices)
    }


    set FMRIEngine(conditionCurve) cCurve 
    set FMRIEngine(baselineCurve) bCurve 
    set FMRIEngine(voxelIndices) voxelIndices

    $FMRIEngine(timeCourseGraph) element create $FMRIEngine(conditionCurve) \
        -label "Condition Average" -xdata xVecCon -ydata yVecCon
    $FMRIEngine(timeCourseGraph) element configure $FMRIEngine(conditionCurve) \
        -symbol none -color red -linewidth 2 

    $FMRIEngine(timeCourseGraph) element bind $FMRIEngine(conditionCurve) <ButtonPress-1> { 
        # puts "Touched $FMRIEngine(conditionCurve)"
        FMRIEngineShowData condition 
    }

    set i 0 
    while {$i < $volsPerCondition} {
        set x1 $xVecCon($i) 
        set y1 [lindex $FMRIEngine(conditionTCMax) $i] 
        set x2 $xVecCon($i) 
        set y2 [lindex $FMRIEngine(conditionTCMin) $i] 

        $FMRIEngine(timeCourseGraph) marker create line \
            -coords {$x1 $y1 $x2 $y2} -name lineMarkerCon$i -linewidth 1 \
            -outline red -under yes 

        incr i
    }

    $FMRIEngine(timeCourseGraph) element create $FMRIEngine(baselineCurve) \
        -label "Baseline Average" -xdata xVecBase -ydata yVecBase
    $FMRIEngine(timeCourseGraph) element configure $FMRIEngine(baselineCurve) \
        -symbol none -color blue -linewidth 2 

    $FMRIEngine(timeCourseGraph) element bind $FMRIEngine(baselineCurve) <ButtonPress-1> {
        # puts "Touched $FMRIEngine(baselineCurve)"
        FMRIEngineShowData baseline 
    }

    set i 0 
    while {$i < $volsPerBaseline} {
 
        set x1 $xVecBase($i) 
        set y1 [lindex $FMRIEngine(baselineTCMax) $i] 
        set x2 $xVecBase($i) 
        set y2 [lindex $FMRIEngine(baselineTCMin) $i] 

        $FMRIEngine(timeCourseGraph) marker create line \
            -coords {$x1 $y1 $x2 $y2} -name lineMarkerBase$i -linewidth 1 -outline blue \
            -xoffset 1 -under yes 

        incr i
    }
    
    # Voxel indices
    $FMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$noVols $FMRIEngine(timeCourseYMax)} \
        -yoffset 5 -xoffset -70 -name $FMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineShowData
# Pops a separate window to show data for the selected curve 
# .ARGS
# curve a string to indicate which curve has been selected. 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineShowData {curve} {
    global FMRIEngine Gui

    # if the user selects a different curve, close the current window
    # and open a new window for the new curve
    if {[info exists FMRIEngine(curve)] && 
        $FMRIEngine(curve) != $curve} {
        FMRIEngineCloseDataWindow
    }
    set FMRIEngine(curve) $curve
    if {$curve == "condition"} {
        set title "Condition Data"
    } else {
        set title "Baseline Data"
    }

    if {[info exists FMRIEngine(dataToplevel)] == 0 } {
        set w .dataren
        toplevel $w
        wm title $w $title 
        wm minsize $w 250 360 
        wm geometry $w "+898+200" 

        label $w.vol -text "vol" -font fixed
        label $w.min -text "min" -font fixed
        label $w.max -text "max" -font fixed
        label $w.ave -text "ave" -font fixed
        label $w.std -text "std" -font fixed
   
        blt::table $w 
        blt::table $w $w.vol 0,0 $w.min 0,1 $w.max 0,2 $w.ave 0,3 $w.std 0,4

        set volsPerBaseline [lindex $FMRIEngine(paradigm) 2]
        set volsPerCondition [lindex $FMRIEngine(paradigm) 3]
        set noVols \
        [expr {$curve == "baseline" ? $volsPerBaseline : $volsPerCondition}] 

        set i 1
        while {$i <= $noVols} {
            if {$curve == "condition"} {
                label $w.$i -text $i -font fixed
            } else {
                label $w.$i -font fixed -text [expr $i + $volsPerCondition - 1] 
 
            }

            blt::table $w $w.$i $i,0 
            incr i
        }
      
        set i [expr $noVols + 1]
        button $w.bDismiss -text "Dismiss" -font fixed -command "FMRIEngineCloseDataWindow"
        blt::table $w $w.bDismiss $i,1 -cspan 3 

        wm protocol $w WM_DELETE_WINDOW "FMRIEngineCloseDataWindow" 
        set FMRIEngine(dataToplevel) $w
        set FMRIEngine(dataTable) $w.table

        # adding data into the table
        set i 1
        while {$i <= $noVols} {

            set index [expr $i - 1]

            set j 1
            foreach m "TCMin TCMax TCAve TCStd" {
                set v ""
                append v $curve $m 
                set v [lindex $FMRIEngine($v) $index]

                label $w.l$m$i -text $v -font fixed  
                blt::table $w $w.l$m$i $i,$j

                incr j
            }

            incr i
        }

    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCreateCurvesFromTimeCourse
# Creates curves for short format time course plotting 
# .ARGS
# (i, j, k) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCreateCurvesFromTimeCourse {i j k} {
    global FMRIEngine

    # Retrieves paradigm (model) parameter 
    set TotalVolumes [lindex $FMRIEngine(paradigm) 0]
    set NumberOfConditions [lindex $FMRIEngine(paradigm) 1] 
    set VolumesPerBaseline [lindex $FMRIEngine(paradigm) 2] 
    set VolumesPerCondition [lindex $FMRIEngine(paradigm) 3] 
    set VolumesAtStart [lindex $FMRIEngine(paradigm) 4] 
    set StartsWith [lindex $FMRIEngine(paradigm) 5] 

    # Cleans variables
    unset -nocomplain FMRIEngine(conditionTCMax)
    unset -nocomplain FMRIEngine(conditionTCMin)
    unset -nocomplain FMRIEngine(conditionTCAve)
    unset -nocomplain FMRIEngine(conditionTCStd)
    unset -nocomplain FMRIEngine(baselineTCMax)
    unset -nocomplain FMRIEngine(baselineTCMin)
    unset -nocomplain FMRIEngine(baselineTCAve)
    unset -nocomplain FMRIEngine(baselineTCStd)
 
    # Creates curves
    set cStart [expr {$StartsWith == 1 ? ($VolumesAtStart + $VolumesPerBaseline) : $VolumesAtStart}]
    set bStart [expr {$StartsWith == 0 ? ($VolumesAtStart + $VolumesPerCondition) : $VolumesAtStart}]

    set timeCourse [FMRIEngine(actvolgen) GetTimeCourse $i $j $k]
    set myRange [$timeCourse GetRange]
    set FMRIEngine(timeCourseYMin) [lindex $myRange 0]
    set max [lindex $myRange 1]
    set FMRIEngine(timeCourseYMax) [expr {$max == 0 ? 1 : $max}] 

    # For condition
    set i 0
    while {$i < $VolumesPerCondition} {
        set total 0.0
        set qtotal 0.0
        set max 0.0
        set min 1000000.0
        set j [expr $cStart + $i]
        set count 0

        while {$j < $TotalVolumes} {
            set v [$timeCourse GetComponent $j 0]
            set total [expr $total + $v]
            set qtotal [expr $qtotal + $v * $v]
            if {$v > $max} {
                set max $v 
            }
            if {$v < $min} {
                set min $v
            }

            set j [expr $j + $VolumesPerBaseline + $VolumesPerCondition]
            incr count 
        }

        lappend FMRIEngine(conditionTCMin) $min
        lappend FMRIEngine(conditionTCMax) $max
        set ave [expr round($total / $count)]
        lappend FMRIEngine(conditionTCAve) $ave

        # calculates std
        set v1 [expr $total * $total / $VolumesPerCondition]
        set var [expr {($qtotal - $v1) / ($VolumesPerCondition - 1)}]
        set std [expr sqrt($var)]
        set rv [expr round($std)]
        lappend FMRIEngine(conditionTCStd) $rv

        incr i
    }
   
    # For baseline 
    set i 0
    while {$i < $VolumesPerBaseline} {
        set total 0.0
        set qtotal 0.0
        set max 0.0
        set min 1000000.0
        set j [expr $bStart + $i]
        set count 0

        while {$j < $TotalVolumes} {
            set v [$timeCourse GetComponent $j 0]
            set total [expr $total + $v]
            set qtotal [expr $qtotal + $v * $v]
            if {$v > $max} {
                set max $v 
            }
            if {$v < $min} {
                set min $v 
            }

            set j [expr $j + $VolumesPerBaseline + $VolumesPerCondition]
            incr count
        }

        lappend FMRIEngine(baselineTCMin) $min
        lappend FMRIEngine(baselineTCMax) $max
        set ave [expr round($total / $count)]
        lappend FMRIEngine(baselineTCAve) $ave 

        # calculates std
        set v1 [expr $total * $total / $VolumesPerBaseline]
        set var [expr {($qtotal - $v1) / ($VolumesPerBaseline - 1)}]
        set std [expr sqrt($var)]
        set rv [expr round($std)]
        lappend FMRIEngine(baselineTCStd) $rv

        incr i
   }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineDrawPlotLong
# Draws time course plot in long format 
# .ARGS
# (x, y, z) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineDrawPlotLong {x y z} {
    global FMRIEngine

    # Cleans variables
    unset -nocomplain FMRIEngine(conValues)
    unset -nocomplain FMRIEngine(parValues)
 
    set timeCourse [FMRIEngine(actvolgen) GetTimeCourse $x $y $z]
    set myRange [$timeCourse GetRange]
    set timeCourseYMin [lindex $myRange 0]
    set max [lindex $myRange 1]
    set timeCourseYMax [expr {$max == 0 ? 1 : $max}] 

    set TotalVolumes [lindex $FMRIEngine(paradigm) 0]
    set paradigm FMRIEngine(stimulus)

    set diff [expr {$timeCourseYMax - $timeCourseYMin}]
    set parMin [expr {$timeCourseYMin + ($diff * 0.25)}]
    set parMax [expr {$timeCourseYMin + ($diff * 0.75)}]
    set i 0
    while {$i < $TotalVolumes} {
        lappend xAxis [expr $i + 1]
        lappend FMRIEngine(conValues) [$timeCourse GetComponent $i 0]
        set v [$paradigm GetComponent $i 0]
        if {$v == 0} {
            lappend FMRIEngine(parValues) $parMin 
        } else {
            lappend FMRIEngine(parValues) $parMax 
        }

        incr i
    }

    $FMRIEngine(timeCourseGraph) axis configure x -min 1 -max $TotalVolumes 
    $FMRIEngine(timeCourseGraph) axis configure y \
        -min $timeCourseYMin -max $timeCourseYMax

    blt::vector xVecCon yVecCon xVecPar yVecPar
    xVecCon set $xAxis
    yVecCon set $FMRIEngine(conValues)
    xVecPar set $xAxis
    yVecPar set $FMRIEngine(parValues)

    if {[info exists FMRIEngine(conditionCurve)] &&
        [$FMRIEngine(timeCourseGraph) element exists $FMRIEngine(conditionCurve)]} {
        $FMRIEngine(timeCourseGraph) element delete $FMRIEngine(conditionCurve)
    }
    if {[info exists FMRIEngine(baselineCurve)] &&
        [$FMRIEngine(timeCourseGraph) element exists $FMRIEngine(baselineCurve)]} {
        $FMRIEngine(timeCourseGraph) element delete $FMRIEngine(baselineCurve)
    }
    if {[info exists FMRIEngine(voxelIndices)] &&
        [$FMRIEngine(timeCourseGraph) marker exists $FMRIEngine(voxelIndices)]} {
        $FMRIEngine(timeCourseGraph) marker delete $FMRIEngine(voxelIndices)
    }


    set FMRIEngine(conditionCurve) cCurve 
    set FMRIEngine(baselineCurve) bCurve 
    set FMRIEngine(voxelIndices) voxelIndices

    $FMRIEngine(timeCourseGraph) element create $FMRIEngine(conditionCurve) \
        -label "Condition" -xdata xVecCon -ydata yVecCon
    $FMRIEngine(timeCourseGraph) element configure $FMRIEngine(conditionCurve) \
        -symbol none -color red -linewidth 2 

    $FMRIEngine(timeCourseGraph) element create $FMRIEngine(baselineCurve) \
        -label "Paradigm" -xdata xVecPar -ydata yVecPar
    $FMRIEngine(timeCourseGraph) element configure $FMRIEngine(baselineCurve) \
        -symbol none -color blue -linewidth 2 
   
    # Voxel indices
    $FMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$TotalVolumes $timeCourseYMax} \
        -yoffset 5 -xoffset -70 -name $FMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCloseDataWindow
# Cleans up if the data window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCloseDataWindow {} {
    global FMRIEngine

    unset -nocomplain FMRIEngine(curve)

    destroy $FMRIEngine(dataTable)
    unset -nocomplain FMRIEngine(dataTable)

    destroy $FMRIEngine(dataToplevel)
    unset FMRIEngine(dataToplevel)
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCloseTimeCourseWindow
# Cleans up if the time course window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCloseTimeCourseWindow {} {
    global FMRIEngine

    destroy $FMRIEngine(timeCourseGraph)
    unset -nocomplain FMRIEngine(timeCourseGraph)
    unset -nocomplain FMRIEngine(conditionCurve)]
    unset -nocomplain FMRIEngine(baselineCurve)]

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

    set ext $FMRIEngine(volumeExtent) 

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

#    set fvName [[[Slicer GetForeVolume $s] GetMrmlNode] GetName]
#    if {! [info exists FMRIEngine(actVolName)] ||
#        $fvName != $FMRIEngine(actVolName)} {
#        return "-1 -1 -1"
#    }

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
    set i [expr round ($i)]
    set j [expr round ($j)]    
    set k [expr round ($k)]
    # puts "Rounded voxel coords: $i $j $k"
    puts "Voxel indices: $i $j $k"
    
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
    # puts "argstr = $argstr\n"
    if {$i < $xmin || $j < $ymin || $k < $zmin} {
        return 0 
    }
    if {$i > $xmax || $j > $ymax || $k > $zmax} {
        return 0 
    }

    return 1 
}
