#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        fMRIEnginePlot.tcl
# PROCEDURES:  
#   fMRIEnginePopUpPlot y)
#   fMRIEngineDrawPlotShort y,
#   fMRIEngineShowData a
#   fMRIEngineCreateCurvesFromTimeCourse j,
#   fMRIEngineDrawPlotLong y,
#   fMRIEngineCloseDataWindow
#   fMRIEngineCloseTimeCourseWindow
#   fMRIEngineGetDataVolumeDimensions
#   fMRIEngineGetVoxelFromSelection y)
#   fMRIEngineCheckSelectionAgainstVolumeLimits the
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC fMRIEnginePopUpPlot
# This routine pops up a plot of a selected voxel's response over
# time, overlayed onto a reference signal. This reference may be the
# experimental protocol, or the protocol convolved with a hemodynamic
# response function.
# .ARGS
# (x, y) the selected point
# .END
#-------------------------------------------------------------------------------
proc fMRIEnginePopUpPlot {x y} {
    global fMRIEngine MultiVolumeReader

    if {$fMRIEngine(currentTab) != "Inspect"} {
        return
    }

    # Checks time course plotting option
    switch $fMRIEngine(tcPlottingOption) {
        "" {
            return
        }
        "Long" {
            set plotTitle "Timecourse Plot"
            set plotGeometry "+335+200"
            set plotHeight 250 

            if {$MultiVolumeReader(noOfVolumes) > 150} { 
                set plotWidth 700
                set graphWidth 700
            } else {
                set plotWidth 500
                set graphWidth 500
            }
        }
        "Short" {
            set plotTitle "Peristimulus Histogram"
            set plotHeight 250 
            set plotGeometry "+335+200"

            if {$MultiVolumeReader(noOfVolumes) > 150} { 
                set plotWidth 700
                set graphWidth 700
            } else {
                set plotWidth 500
                set graphWidth 500
            }
        }
        "ROI" {
            puts "ROI plotting is being constructed."
            return
        }
    }

#    if {! [info exists fMRIEngine(firstMRMLid)] ||
#        ! [info exists fMRIEngine(lastMRMLid)]} {
        # DevErrorWindow "Please load volume sequence first."
#        return
#    }

    # Get the indices of selected voxel. Then, check
    # these indices against the dimensions of the volume.
    # If they're good values, assemble the selected voxel's
    # time-course, the reference signal, and plot both.
    scan [fMRIEngineGetVoxelFromSelection $x $y] "%d %d %d" i j k
    if {$i == -1} {
        return
    }

    scan [fMRIEngineGetDataVolumeDimensions] "%d %d %d %d %d %d" \
        xmin ymin zmin xmax ymax zmax

    # Check to make sure that the selected voxel
    # is within the data volume. If not, return.
    set argstr "$i $j $k $xmin $ymin $zmin $xmax $ymax $zmax"
    if {[ fMRIEngineCheckSelectionAgainstVolumeLimits $argstr] == 0} {
        # DevErrorWindow "Selected voxel not in volume."
        return 
    }

    # Plot the time course
    if {[info exists fMRIEngine(timeCourseToplevel)] == 0 } {
        set w .tcren
        toplevel $w
        wm title $w $plotTitle 
        wm minsize $w $plotWidth $plotHeight
        wm geometry $w $plotGeometry 

        blt::graph $w.graph -plotbackground white -width $graphWidth 
        pack $w.graph 
        $w.graph legend configure -position bottom -relief raised \
            -font fixed -fg black 
        $w.graph axis configure y -title "Intensity"
        # $w.graph grid on
        # $w.graph grid configure -color black

        wm protocol $w WM_DELETE_WINDOW "fMRIEngineCloseTimeCourseWindow" 

        set fMRIEngine(timeCourseToplevel) $w
        set fMRIEngine(timeCourseGraph) $w.graph
    }

    if {$fMRIEngine(tcPlottingOption) == "Short"} {
        $fMRIEngine(timeCourseGraph) axis configure x \
            -title "Combined Condition/Baseline Volume Number" 
    } else {
        $fMRIEngine(timeCourseGraph) axis configure x -title "Volume Number" 
    }

    fMRIEngineDrawPlot$fMRIEngine(tcPlottingOption) $i $j $k 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDrawPlotShort
# Draws time course plot in short format 
# .ARGS
# (x, y, z) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDrawPlotShort {x y z} {
    global fMRIEngine

    # Creates curves from time course
    fMRIEngineCreateCurvesFromTimeCourse $x $y $z
  
    set volsPerBaseline [lindex $fMRIEngine(paradigm) 2]
    set volsPerCondition [lindex $fMRIEngine(paradigm) 3]
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

    $fMRIEngine(timeCourseGraph) axis configure x -min 1 -max $noVols 
    $fMRIEngine(timeCourseGraph) axis configure y \
        -min $fMRIEngine(timeCourseYMin) -max $fMRIEngine(timeCourseYMax)

    xVecCon set $xAxisCondition
    yVecCon set $fMRIEngine(conditionTCAve)
    xVecBase set $xAxisBaseline
    yVecBase set $fMRIEngine(baselineTCAve)

    set i 0 
    while {$i < $volsPerBaseline} {
        lappend coordsBase $xVecBase($i) 
        lappend coordsBase [lindex $fMRIEngine(baselineTCMax) $i] 
        lappend coordsBase $xVecBase($i) 
        lappend coordsBase [lindex $fMRIEngine(baselineTCMin) $i] 
 
        incr i
    }

    if {[info exists fMRIEngine(conditionCurve)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(conditionCurve)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(conditionCurve)
    }
    if {[info exists fMRIEngine(baselineCurve)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(baselineCurve)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(baselineCurve)
    }
    if {[info exists fMRIEngine(voxelIndices)] &&
        [$fMRIEngine(timeCourseGraph) marker exists $fMRIEngine(voxelIndices)]} {
        $fMRIEngine(timeCourseGraph) marker delete $fMRIEngine(voxelIndices)
    }


    set fMRIEngine(conditionCurve) cCurve 
    set fMRIEngine(baselineCurve) bCurve 
    set fMRIEngine(voxelIndices) voxelIndices

    $fMRIEngine(timeCourseGraph) element create $fMRIEngine(conditionCurve) \
        -label "Condition Average" -xdata xVecCon -ydata yVecCon
    $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(conditionCurve) \
        -symbol none -color red -linewidth 2 

    $fMRIEngine(timeCourseGraph) element bind $fMRIEngine(conditionCurve) <ButtonPress-1> { 
        # puts "Touched $fMRIEngine(conditionCurve)"
        fMRIEngineShowData condition 
    }

    set i 0 
    while {$i < $volsPerCondition} {
        set x1 $xVecCon($i) 
        set y1 [lindex $fMRIEngine(conditionTCMax) $i] 
        set x2 $xVecCon($i) 
        set y2 [lindex $fMRIEngine(conditionTCMin) $i] 

        $fMRIEngine(timeCourseGraph) marker create line \
            -coords {$x1 $y1 $x2 $y2} -name lineMarkerCon$i -linewidth 1 \
            -outline red -under yes 

        incr i
    }

    $fMRIEngine(timeCourseGraph) element create $fMRIEngine(baselineCurve) \
        -label "Baseline Average" -xdata xVecBase -ydata yVecBase
    $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(baselineCurve) \
        -symbol none -color blue -linewidth 2 

    $fMRIEngine(timeCourseGraph) element bind $fMRIEngine(baselineCurve) <ButtonPress-1> {
        # puts "Touched $fMRIEngine(baselineCurve)"
        fMRIEngineShowData baseline 
    }

    set i 0 
    while {$i < $volsPerBaseline} {
 
        set x1 $xVecBase($i) 
        set y1 [lindex $fMRIEngine(baselineTCMax) $i] 
        set x2 $xVecBase($i) 
        set y2 [lindex $fMRIEngine(baselineTCMin) $i] 

        $fMRIEngine(timeCourseGraph) marker create line \
            -coords {$x1 $y1 $x2 $y2} -name lineMarkerBase$i -linewidth 1 -outline blue \
            -xoffset 1 -under yes 

        incr i
    }
    
    # Voxel indices
    $fMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$noVols $fMRIEngine(timeCourseYMax)} \
        -yoffset 5 -xoffset -70 -name $fMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowData
# Pops a separate window to show data for the selected curve 
# .ARGS
# curve a string to indicate which curve has been selected. 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineShowData {curve} {
    global fMRIEngine Gui

    # if the user selects a different curve, close the current window
    # and open a new window for the new curve
    if {[info exists fMRIEngine(curve)] && 
        $fMRIEngine(curve) != $curve} {
        fMRIEngineCloseDataWindow
    }
    set fMRIEngine(curve) $curve
    if {$curve == "condition"} {
        set title "Condition Data"
    } else {
        set title "Baseline Data"
    }

    if {[info exists fMRIEngine(dataToplevel)] == 0 } {
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

        set volsPerBaseline [lindex $fMRIEngine(paradigm) 2]
        set volsPerCondition [lindex $fMRIEngine(paradigm) 3]
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
        button $w.bDismiss -text "Dismiss" -font fixed -command "fMRIEngineCloseDataWindow"
        blt::table $w $w.bDismiss $i,1 -cspan 3 

        wm protocol $w WM_DELETE_WINDOW "fMRIEngineCloseDataWindow" 
        set fMRIEngine(dataToplevel) $w
        set fMRIEngine(dataTable) $w.table

        # adding data into the table
        set i 1
        while {$i <= $noVols} {

            set index [expr $i - 1]

            set j 1
            foreach m "TCMin TCMax TCAve TCStd" {
                set v ""
                append v $curve $m 
                set v [lindex $fMRIEngine($v) $index]

                label $w.l$m$i -text $v -font fixed  
                blt::table $w $w.l$m$i $i,$j

                incr j
            }

            incr i
        }

    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCreateCurvesFromTimeCourse
# Creates curves for short format time course plotting 
# .ARGS
# (i, j, k) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCreateCurvesFromTimeCourse {i j k} {
    global fMRIEngine

    # Retrieves paradigm (model) parameter 
    set TotalVolumes [lindex $fMRIEngine(paradigm) 0]
    set NumberOfConditions [lindex $fMRIEngine(paradigm) 1] 
    set VolumesPerBaseline [lindex $fMRIEngine(paradigm) 2] 
    set VolumesPerCondition [lindex $fMRIEngine(paradigm) 3] 
    set VolumesAtStart [lindex $fMRIEngine(paradigm) 4] 
    set StartsWith [lindex $fMRIEngine(paradigm) 5] 

    # Cleans variables
    unset -nocomplain fMRIEngine(conditionTCMax)
    unset -nocomplain fMRIEngine(conditionTCMin)
    unset -nocomplain fMRIEngine(conditionTCAve)
    unset -nocomplain fMRIEngine(conditionTCStd)
    unset -nocomplain fMRIEngine(baselineTCMax)
    unset -nocomplain fMRIEngine(baselineTCMin)
    unset -nocomplain fMRIEngine(baselineTCAve)
    unset -nocomplain fMRIEngine(baselineTCStd)
 
    # Creates curves
    set cStart [expr {$StartsWith == 1 ? ($VolumesAtStart + $VolumesPerBaseline) : $VolumesAtStart}]
    set bStart [expr {$StartsWith == 0 ? ($VolumesAtStart + $VolumesPerCondition) : $VolumesAtStart}]

    set timeCourse [fMRIEngine(actEstimator) GetTimeCourse $i $j $k]
    set myRange [$timeCourse GetRange]
    set fMRIEngine(timeCourseYMin) [lindex $myRange 0]
    set max [lindex $myRange 1]
    set fMRIEngine(timeCourseYMax) [expr {$max == 0 ? 1 : $max}] 

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

        lappend fMRIEngine(conditionTCMin) $min
        lappend fMRIEngine(conditionTCMax) $max
        set ave [expr round($total / $count)]
        lappend fMRIEngine(conditionTCAve) $ave

        # calculates std
        set v1 [expr $total * $total / $VolumesPerCondition]
        set var [expr {($qtotal - $v1) / ($VolumesPerCondition - 1)}]
        set var [expr abs($var)]
        set std [expr sqrt($var)]
        set rv [expr round($std)]
        lappend fMRIEngine(conditionTCStd) $rv

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

        lappend fMRIEngine(baselineTCMin) $min
        lappend fMRIEngine(baselineTCMax) $max
        set ave [expr round($total / $count)]
        lappend fMRIEngine(baselineTCAve) $ave 

        # calculates std
        set v1 [expr $total * $total / $VolumesPerBaseline]
        set var [expr {($qtotal - $v1) / ($VolumesPerBaseline - 1)}]
        set var [expr abs($var)]
        set std [expr sqrt($var)]
        set rv [expr round($std)]
        lappend fMRIEngine(baselineTCStd) $rv

        incr i
   }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDrawPlotLong
# Draws time course plot in long format 
# .ARGS
# (x, y, z) the index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDrawPlotLong {x y z} {
    global MultiVolumeReader fMRIEngine fMRIModelView 


    # Cleans variables
    unset -nocomplain fMRIEngine(conValues)
    unset -nocomplain fMRIEngine(parValues1)
    unset -nocomplain fMRIEngine(parValues2)
 
    set timeCourse [fMRIEngine(actEstimator) GetTimeCourse $x $y $z]
    set myRange [$timeCourse GetRange]
    set timeCourseYMin [lindex $myRange 0]
    set max [lindex $myRange 1]
    set timeCourseYMax [expr {$max == 0 ? 1 : $max}] 

    set TotalVolumes $MultiVolumeReader(noOfVolumes) 

    set name $fMRIEngine(currentActVolName)
    set cName [string range $name 11 end]
    set cVec $fMRIEngine($cName,contrastVector)
    # trim white spaces at beginning and end
    set cVec [string trim $cVec]
    # replace multiple spaces in the middle of the string by one space  
    regsub -all {( )+} $cVec " " cVec
    set cList [split $cVec " "]
    set a [lsearch -exact $cList 1]
    set b [lsearch -exact $cList -1]

    set done [fMRIModelViewGenerateModel]
    if {! $done} {
        return 
    }

    if {$a != -1} {
        set index [expr $a+1]
        set paradigm1 $fMRIModelView(Data,Run1,EV$index,EVData)
    }
    if {$b != -1} {
        set index [expr $b+1]
        set paradigm2 $fMRIModelView(Data,Run1,EV$index,EVData)
    }
    
    set diff [expr {$timeCourseYMax - $timeCourseYMin}]
    set parMin [expr {$timeCourseYMin + ($diff * 0.25)}]
    set parMax [expr {$timeCourseYMin + ($diff * 0.75)}]
    set i 0
    while {$i < $TotalVolumes} {
        lappend xAxis [expr $i + 1]
        lappend fMRIEngine(conValues) [$timeCourse GetComponent $i 0]

        if {$a != -1} {
            set v [lindex $paradigm1 $i]
            lappend fMRIEngine(parValues1) [expr $v * ($parMin + $parMax) / 2] 
        }
        if {$b != -1} {
            set v [lindex $paradigm2 $i]
            lappend fMRIEngine(parValues2) [expr $v * ($parMin + $parMax) / 2] 
        }

        incr i
    }


    $fMRIEngine(timeCourseGraph) axis configure x -min 1 -max $TotalVolumes 
    $fMRIEngine(timeCourseGraph) axis configure y \
        -min $timeCourseYMin -max $timeCourseYMax

    blt::vector xVecCon yVecCon xVecPar1 yVecPar1 xVecPar2 yVecPar2
    xVecCon set $xAxis
    yVecCon set $fMRIEngine(conValues)

    if {$a != -1} {
        xVecPar1 set $xAxis
        yVecPar1 set $fMRIEngine(parValues1)
    }
    if {$b != -1} {
        xVecPar2 set $xAxis
        yVecPar2 set $fMRIEngine(parValues2)
    }

   
    if {[info exists fMRIEngine(conditionCurve)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(conditionCurve)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(conditionCurve)
    }
    if {[info exists fMRIEngine(baselineCurve1)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(baselineCurve1)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(baselineCurve1)
    }
    if {[info exists fMRIEngine(baselineCurve2)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(baselineCurve2)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(baselineCurve2)
    }
    if {[info exists fMRIEngine(voxelIndices)] &&
        [$fMRIEngine(timeCourseGraph) marker exists $fMRIEngine(voxelIndices)]} {
        $fMRIEngine(timeCourseGraph) marker delete $fMRIEngine(voxelIndices)
    }


    set fMRIEngine(conditionCurve) cCurve 
    set fMRIEngine(baselineCurve1) bCurve1 
    set fMRIEngine(baselineCurve2) bCurve2 
    set fMRIEngine(voxelIndices) voxelIndices

    $fMRIEngine(timeCourseGraph) element create $fMRIEngine(conditionCurve) \
        -label "Response" -xdata xVecCon -ydata yVecCon
    $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(conditionCurve) \
        -symbol none -color red -linewidth 1 

    if {$a != -1} {
        $fMRIEngine(timeCourseGraph) element create $fMRIEngine(baselineCurve1) \
            -label "Condition1" -xdata xVecPar1 -ydata yVecPar1
        $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(baselineCurve1) \
            -symbol none -color blue -linewidth 1 
    }   
    if {$b != -1} {
        $fMRIEngine(timeCourseGraph) element create $fMRIEngine(baselineCurve2) \
            -label "Condition2" -xdata xVecPar2 -ydata yVecPar2
        $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(baselineCurve2) \
            -symbol none -color black -linewidth 1 
    }   


    # Voxel indices
    $fMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$TotalVolumes $timeCourseYMax} \
        -yoffset 5 -xoffset -70 -name $fMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCloseDataWindow
# Cleans up if the data window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCloseDataWindow {} {
    global fMRIEngine

    unset -nocomplain fMRIEngine(curve)

    destroy $fMRIEngine(dataTable)
    unset -nocomplain fMRIEngine(dataTable)

    destroy $fMRIEngine(dataToplevel)
    unset fMRIEngine(dataToplevel)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCloseTimeCourseWindow
# Cleans up if the time course window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCloseTimeCourseWindow {} {
    global fMRIEngine

    destroy $fMRIEngine(timeCourseGraph)
    unset -nocomplain fMRIEngine(timeCourseGraph)
    unset -nocomplain fMRIEngine(conditionCurve)]
    unset -nocomplain fMRIEngine(baselineCurve)]

    destroy $fMRIEngine(timeCourseToplevel)
    unset fMRIEngine(timeCourseToplevel)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineGetDataVolumeDimensions
# Gets volume dimensions 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineGetDataVolumeDimensions {} {
    global Volume fMRIEngine

    set ext $fMRIEngine(volumeExtent) 

    set xmin [lindex $ext 0]
    set xmax [lindex $ext 1]
    set ymin [lindex $ext 2]
    set ymax [lindex $ext 3]
    set zmin [lindex $ext 4]
    set zmax [lindex $ext 5]

    return "$xmin $ymin $zmin $xmax $ymax $zmax"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineGetVoxelFromSelection
# Gets voxel index from the selection 
# .ARGS
# (x, y) the selected point 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineGetVoxelFromSelection {x y} {
    global fMRIEngine Interactor Gui
    
    # Which slice was picked?
    set s $Interactor(s)
    if {$s == ""} {
        DevErrorWindow "No slice was picked."
        return "-1 -1 -1"
    }

#    set fvName [[[Slicer GetForeVolume $s] GetMrmlNode] GetName]
#    if {! [info exists fMRIEngine(actVolName)] ||
#        $fvName != $fMRIEngine(actVolName)} {
#        return "-1 -1 -1"
#    }

    set xs $x
    set ys $y

    # Which xy coordinates were picked?
    scan [MainInteractorXY $s $xs $ys] "%d %d %d %d" xs ys x y
    # puts "Click: $s $x $y"

    # We had the following problem for time course plotting:
    # If the background volume is structural (even it was well co-registrated 
    # with the functional volumes) and the foreground volume is the activation
    # the time course plotting cannot work properly. This was caused by method
    # GetIJKPoint which returned voxel index of the background image (but we
    # needed that from the activation itself).
    #
    # To fix this problem, (permitted by Steve P.) I added another 
    # SetReformatPoint in vtkMrmlSlicer with different signature:
    # void vtkMrmlSlicer::SetReformatPoint(vtkMrmlDataVolume *vol, 
    #                                 vtkImageReformat *ref,  
    #                                 int s, int x, int y)
    # This change keeps the current functionality of vtkMrmlSlicer and adds
    # a possibility for user to get the voxel index of the activation volume 
    # (i.e. the foreground image) as s/he moves the mouse over one of the three
    # slice windows.
    set fVol [$Interactor(activeSlicer) GetForeVolume $s]
    set fRef [$Interactor(activeSlicer) GetForeReformat $s]
    $Interactor(activeSlicer) SetReformatPoint $fVol $fRef $s $x $y
    # Which voxel index (ijk) were picked?
    scan [$Interactor(activeSlicer) GetIjkPoint]  "%g %g %g" i j k

    # Let's snap to the nearest voxel
    set i [expr round ($i)]
    set j [expr round ($j)]    
    set k [expr round ($k)]
    # puts "Rounded voxel coords: $i $j $k"
    puts "Voxel indices: $i $j $k"
    
    return "$i $j $k"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCheckSelectionAgainstVolumeLimits
# Checks voxel selection against volume limits 
# .ARGS
# argstr the data string
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCheckSelectionAgainstVolumeLimits {argstr} {

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
