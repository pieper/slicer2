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
#   fMRIEnginePopUpPlot x y
#   fMRIEngineDrawPlotShort x y z
#   fMRIEngineShowData loc
#   fMRIEngineSortEVsForStat i j k
#   fMRIEngineCreateCurvesFromTimeCourse i j k
#   fMRIEngineDrawPlotLong x y z
#   fMRIEngineCloseDataWindow
#   fMRIEngineCloseTimeCourseWindow
#   fMRIEngineGetDataVolumeDimensions
#   fMRIEngineGetVoxelFromSelection x y
#   fMRIEngineCheckSelectionAgainstVolumeLimits argstr
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC fMRIEnginePopUpPlot
# This routine pops up a plot of a selected voxel's response over
# time, overlayed onto a reference signal. This reference may be the
# experimental protocol, or the protocol convolved with a hemodynamic
# response function.
# .ARGS
# int x the selected point's x coord
# int y the selected point's y coord
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

    if {[info exists fMRIEngine(timeCourseToplevel)] &&
        $fMRIEngine(curPlotting) != $fMRIEngine(tcPlottingOption)} { 
        fMRIEngineCloseTimeCourseWindow
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
            -title "Combined All-EV Volume Number" 
    } else {
        $fMRIEngine(timeCourseGraph) axis configure x -title "Volume Number" 
    }

    fMRIEngineDrawPlot$fMRIEngine(tcPlottingOption) $i $j $k 
    set fMRIEngine(x,voxelIndex) $i
    set fMRIEngine(y,voxelIndex) $j
    set fMRIEngine(z,voxelIndex) $k
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDrawPlotShort
# Draws time course plot in short format 
# .ARGS
# int x the x index of voxel whose time course is to be plotted
# int y the y index of voxel whose time course is to be plotted
# int z the z index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDrawPlotShort {x y z} {
    global fMRIEngine

    # Creates curves from time course
    fMRIEngineCreateCurvesFromTimeCourse $x $y $z

    set timeCourse [fMRIEngine(actEstimator) GetTimeCourse $x $y $z]
    set myRange [$timeCourse GetRange]
    set timeCourseYMin [lindex $myRange 0]
    set max [lindex $myRange 1]
    set timeCourseYMax [expr {$max == 0 ? 1 : $max}] 

    set count 1
    set noVols 0
    foreach ev $fMRIEngine(allEVs) {
        set len [llength $fMRIEngine($ev,max)]
        # We have end points overlapping in the graph
        if {$noVols > 0} {
            set noVols [expr $noVols-1]
        }
        set noVols [expr $noVols+$len]

        blt::vector xVec$ev yVec$ev
        set tmpList ""

        # We have end points overlapping in the graph
        for {set i 1} {$i <= $len} {incr i} {
            lappend tmpList $count
            if {$i != $len} {
                incr count
            }
        }
        xVec$ev set $tmpList
        yVec$ev set $fMRIEngine($ev,ave)
    }

    $fMRIEngine(timeCourseGraph) axis configure x -min 1 -max $noVols 
    $fMRIEngine(timeCourseGraph) axis configure y \
        -min $timeCourseYMin -max $timeCourseYMax

    # colors for lines
    set allColors [list red purple pink blue orange yellow]
    set len [llength $fMRIEngine(allEVs)]
    set colors [lrange $allColors 0 [expr $len-2]]
    lappend colors black

    foreach ev $fMRIEngine(allEVs) \
            color $colors {
        # cleaning
        if {[info exists fMRIEngine(curve$ev)] &&
            [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(curve$ev)]} {
                $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(curve$ev)
        }

        # create curves
        set fMRIEngine(curve$ev) $ev 
        $fMRIEngine(timeCourseGraph) element create $fMRIEngine(curve$ev) \
            -label "$ev" -xdata xVec$ev -ydata yVec$ev
        $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(curve$ev) \
            -symbol none -color $color -linewidth 2 

        $fMRIEngine(timeCourseGraph) element bind $fMRIEngine(curve$ev) <ButtonPress-1> { 
            fMRIEngineShowData
        }

        # create vertical bars
        set len [llength $fMRIEngine($ev,max)]
        blt::vector xVec
        xVec set xVec$ev
 
        set i 0 
        while {$i < $len} {
            set x1 $xVec($i) 
            set y1 [lindex $fMRIEngine($ev,max) $i] 
            set x2 $x1 
            set y2 [lindex $fMRIEngine($ev,min) $i] 

            set lmName "lm$ev$i"
            $fMRIEngine(timeCourseGraph) marker create line \
                -coords {$x1 $y1 $x2 $y2} -name $lmName -linewidth 1 \
                -outline $color -under yes 

            incr i
        }
        unset xVec
    }

    # Voxel indices
    if {[info exists fMRIEngine(voxelIndices)] &&
        [$fMRIEngine(timeCourseGraph) marker exists $fMRIEngine(voxelIndices)]} {
        $fMRIEngine(timeCourseGraph) marker delete $fMRIEngine(voxelIndices)
    }
 
    set fMRIEngine(voxelIndices) voxelIndices
    $fMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$noVols $timeCourseYMax} \
        -yoffset 5 -xoffset -70 -name $fMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 

    set fMRIEngine(curPlotting) "Short"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowData
# Pops a separate window to show data for all curves 
# .ARGS
# int loc defaults to 0
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineShowData {{loc 0}} {
    global fMRIEngine

    if {[info exists fMRIEngine(dataToplevel)] &&
        $loc == $fMRIEngine(currIndexForDataShow)} {
        # data window exists without any changes
        return
    }

    if {$loc == 0} {
        set fMRIEngine(currIndexForDataShow) 0
    } elseif {$loc == -2} {
        set fMRIEngine(currIndexForDataShow) \
            [expr ($fMRIEngine(currIndexForDataShow)+1) % [llength $fMRIEngine(allEVs)]]
    } else {
        set i [expr $fMRIEngine(currIndexForDataShow)-1]
        if {$i < 0} {
            set fMRIEngine(currIndexForDataShow) \
                [expr [llength $fMRIEngine(allEVs)]-1]
        } else {
            set fMRIEngine(currIndexForDataShow) $i
        }
    }

    set ev [lindex $fMRIEngine(allEVs) $fMRIEngine(currIndexForDataShow)]
    if {[info exists fMRIEngine(dataToplevel)]} { 
        fMRIEngineCloseDataWindow
    }

    set w .dataren
    toplevel $w
    wm title $w "Intensities for $ev" 
    # wm minsize $w 250 360 
    # wm geometry $w "+898+200" 
    wm geometry $w "+850+200" 

    # data table headers
    label $w.vol -text "VolIndex" -font fixed
    label $w.min -text "Min" -font fixed
    label $w.max -text "Max" -font fixed
    label $w.ave -text "Average" -font fixed
    blt::table $w \
        $w.vol 0,0 $w.min 0,1 $w.max 0,2 $w.ave 0,3

    # data for the first ev
    set count 1
    foreach min $fMRIEngine($ev,min) \
            max $fMRIEngine($ev,max) \
            ave $fMRIEngine($ev,ave) {

        label $w.vol$count -text $count -font fixed
        label $w.min$count -text $min -font fixed
        label $w.max$count -text $max -font fixed
        label $w.ave$count -text $ave -font fixed
            
        # todo: expression didn't have numeric value
        blt::table $w \
            $w.vol$count $count,0 $w.min$count $count,1 \
            $w.max$count $count,2 $w.ave$count $count,3

        incr count
    }

    button $w.bPrev -text "Prev EV" -font fixed -command "fMRIEngineShowData -1"
    blt::table $w $w.bPrev $count,1 
    button $w.bNext -text "Next EV" -font fixed -command "fMRIEngineShowData -2"
    blt::table $w $w.bNext $count,2 
    button $w.bDismiss -text "Dismiss" -font fixed -command "fMRIEngineCloseDataWindow"
    blt::table $w $w.bDismiss $count,3 

    wm protocol $w WM_DELETE_WINDOW "fMRIEngineCloseDataWindow" 
    set fMRIEngine(dataToplevel) $w
    set fMRIEngine(dataTable) $w.table
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSortEVsForStat
# Sorts EVs into different bins 
# .ARGS
# (x, y, z) index of the voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSortEVsForStat {x y z} {
    global fMRIEngine MultiVolumeReader

    # cleaning
    for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        unset -nocomplain fMRIEngine($r,timeCourse)
        unset -nocomplain fMRIEngine($r,fakeTimeCourse)
    }
    if {[info exists fMRIEngine(allEVs)]} {
        foreach name $fMRIEngine(allEVs) {
            unset -nocomplain fMRIEngine(count$name)
            if {[info exists fMRIEngine($name,noOfSections)]} {
                for {set c 1} {$c <= $fMRIEngine($name,noOfSections)} {incr c} {
                    unset -nocomplain fMRIEngine($name,$c,sections)
                }
            }
        }
    }

    # signal (response) time course
    set oriTimeCourse [fMRIEngine(actEstimator) GetTimeCourse $x $y $z]
    set totalVolumes [$oriTimeCourse GetNumberOfTuples]
    set run $fMRIEngine(curRunForModelFitting)

    if {$run != "combined"} {
        set first $run
        set last $run

        set i 0
        while {$i < $totalVolumes} {
            lappend fMRIEngine($run,timeCourse) [$oriTimeCourse GetComponent $i 0]
            lappend fMRIEngine($run,fakeTimeCourse) 0 
            incr i
        }

    } else {
        set first 1 
        set last $fMRIEngine(noOfSpecifiedRuns)

        set start 0
        for {set r 1} {$r <= $last} {incr r} {
            set seqName $fMRIEngine($r,sequenceName)
            set vols $MultiVolumeReader($seqName,noOfVolumes) 
            set end [expr $start+$vols-1]

            # split the time course according to runs
            set i $start
            while {$i <= $end} {
                lappend fMRIEngine($r,timeCourse) [$oriTimeCourse GetComponent $i 0]
                lappend fMRIEngine($r,fakeTimeCourse) 0
                incr i
            }

            set start [expr $start+$vols]
        }
    }

    # sort out EVs
    for {set r $first} {$r <= $last} {incr r} {
        set tc $fMRIEngine($r,timeCourse)

        foreach name $fMRIEngine($r,namesOfEVs) {
            set onsetsStr $fMRIEngine($r,$name,onsets)
            set onsetsStr [string trim $onsetsStr]
            regsub -all {( )+} $onsetsStr " " onsetsStr 
            set onsets [split $onsetsStr " "]     

            set durationsStr $fMRIEngine($r,$name,durations)
            set durationsStr [string trim $durationsStr]
            regsub -all {( )+} $durationsStr " " durationsStr 
            set durations [split $durationsStr " "]     

            if {! [info exists fMRIEngine(count$name)]} {
                set fMRIEngine(count$name) 1
            }
            foreach slot $onsets \
                    dur  $durations {
                set end [expr $slot+$dur-1]
                for {set j $slot} {$j <= $end} {incr j} {
                    lappend fMRIEngine($name,$fMRIEngine(count$name),sections) [lindex $tc $j]
                    set fMRIEngine($r,fakeTimeCourse) \
                        [lset fMRIEngine($r,fakeTimeCourse) $j "1"] 
                }
                incr fMRIEngine(count$name)
            }
            set fMRIEngine($name,noOfSections) [expr $fMRIEngine(count$name)-1]
        }
    }

    # sort out rest blocks
    set fakeTc ""
    for {set r $first} {$r <= $last} {incr r} {
        set fakeTc [concat $fakeTc $fMRIEngine($r,fakeTimeCourse)]
    }

    set i 0
    set found 0 
    set count 0
    while {$i < $totalVolumes} {
        set val [lindex $fakeTc $i]
        if {$val != 0} {
            set found 0
        } else {
            if {$found == 0} {
                set found 1
                incr count
            }
            lappend fMRIEngine(rest,$count,sections) [$oriTimeCourse GetComponent $i 0]
 
        }

        incr i
    }
    set fMRIEngine(rest,noOfSections) $count
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCreateCurvesFromTimeCourse
# Creates curves for short format time course plotting 
# .ARGS
# int i the i index of voxel whose time course is to be plotted
# int j the j index of voxel whose time course is to be plotted
# int k the k index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCreateCurvesFromTimeCourse {i j k} {
    global fMRIEngine

    # Cleaning
    if {[info exists fMRIEngine(allEVs)]} {
        foreach name $fMRIEngine(allEVs) {
            unset -nocomplain fMRIEngine($name,max)
            unset -nocomplain fMRIEngine($name,min)
            unset -nocomplain fMRIEngine($name,ave)
            # unset -nocomplain fMRIEngine($name,std)
        }
    }

    fMRIEngineSortEVsForStat $i $j $k 

    set run $fMRIEngine(curRunForModelFitting)
    if {$run == "combined"} {
        set run 1
    }
 
    unset -nocomplain fMRIEngine(allEVs)
    set fMRIEngine(allEVs) $fMRIEngine($run,namesOfEVs)
    set fMRIEngine(allEVs) [lappend fMRIEngine(allEVs) rest]

    # For each ev, there are multiple sections which may not be 
    # identical in length. Pick up the bigest length for the
    # following calculation:
    foreach ev $fMRIEngine(allEVs) {
        set no $fMRIEngine($ev,noOfSections)
        set maxLen 1
        for {set k 1} {$k <= $no} {incr k} {
            set len [llength $fMRIEngine($ev,$k,sections)]
            if {$len > $maxLen} {
                set maxLen $len
            }
        }
        set fMRIEngine($ev,sectionLength) $maxLen
    }

    foreach ev $fMRIEngine(allEVs) {
        set no $fMRIEngine($ev,noOfSections)
        set len $fMRIEngine($ev,sectionLength)
        
        for {set i 0} {$i < $len} {incr i} {
             set total 0.0
             set max 0.0
             set min 1000000.0
             set count 0

             for {set j 1} {$j <= $no} {incr j} {
                 set sec $fMRIEngine($ev,$j,sections)
                 if {$i < [llength $sec]} {
                     set value [lindex $sec $i]

                     if {$value > $max} {
                         set max $value 
                     }

                     if {$value < $min} {
                         set min $value
                     }

                     set total [expr $total+$value]
                     incr count
                 }
             }

             lappend fMRIEngine($ev,min) $min
             lappend fMRIEngine($ev,max) $max
             set ave [expr round($total / $count)]
             lappend fMRIEngine($ev,ave) $ave
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDrawPlotLong
# Draws time course plot in long format 
# .ARGS
# int x the x index of voxel whose time course is to be plotted
# int y the y index of voxel whose time course is to be plotted
# int z the z index of voxel whose time course is to be plotted
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDrawPlotLong {x y z} {
    global fMRIEngine fMRIModelView 

    # clean variables
    unset -nocomplain fMRIEngine(signalArray,plotting)
    unset -nocomplain fMRIEngine(evArray,plotting)

    # signal (response) time course
    set timeCourse [fMRIEngine(actEstimator) GetTimeCourse $x $y $z]
    set myRange [$timeCourse GetRange]
    set timeCourseYMin [lindex $myRange 0]
    set max [lindex $myRange 1]
    set timeCourseYMax [expr {$max == 0 ? 1 : $max}] 

    set totalVolumes [$timeCourse GetNumberOfTuples]

    set done [fMRIModelViewGenerateModel]
    if {! $done} {
        puts "Failed in generating model for model view."
        return 
    }

    # ev array
    set run $fMRIEngine(curRunForModelFitting)
    set index $fMRIEngine(curEVIndexForPlotting)
    if {$run != "combined"} {
        set ev $fMRIModelView(Data,Run$run,EV$index,EVData)
    } else {
        set ev ""
        for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
            set ev [concat $ev $fMRIModelView(Data,Run$r,EV$index,EVData)] 
        }
    }

    # get min and max of this ev
    set evMin 1 
    set evMax -1
    foreach name $ev { 
        if {$name > $evMax} {
            set evMax $name
        }
        if {$name < $evMin} {
            set evMin $name
        }
    }

    set evMinToBe [expr {$timeCourseYMin + ($timeCourseYMax-$timeCourseYMin) / 4}]
    set evMaxToBe [expr {$timeCourseYMax - ($timeCourseYMax-$timeCourseYMin) / 4}]

    set i 0
    while {$i < $totalVolumes} {
        lappend xAxis [expr $i + 1]
        lappend fMRIEngine(signalArray,plotting) [$timeCourse GetComponent $i 0]

        set v [lindex $ev $i]
        set newV [expr {(($evMaxToBe-$evMinToBe) * ($v-$evMin) / ($evMax-$evMin)) + $evMinToBe}]
        lappend fMRIEngine(evArray,plotting) $newV 

        incr i
    }

    $fMRIEngine(timeCourseGraph) axis configure x -min 1 -max $totalVolumes 
    $fMRIEngine(timeCourseGraph) axis configure y \
        -min $timeCourseYMin -max $timeCourseYMax

    blt::vector xVecSig yVecSig xVecEV yVecEV
    xVecSig set $xAxis
    yVecSig set $fMRIEngine(signalArray,plotting)

    xVecEV set $xAxis
    yVecEV set $fMRIEngine(evArray,plotting)
   
    if {[info exists fMRIEngine(signalCurve)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(signalCurve)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(signalCurve)
    }
    if {[info exists fMRIEngine(evCurve)] &&
        [$fMRIEngine(timeCourseGraph) element exists $fMRIEngine(evCurve)]} {
        $fMRIEngine(timeCourseGraph) element delete $fMRIEngine(evCurve)
    }
    if {[info exists fMRIEngine(voxelIndices)] &&
        [$fMRIEngine(timeCourseGraph) marker exists $fMRIEngine(voxelIndices)]} {
        $fMRIEngine(timeCourseGraph) marker delete $fMRIEngine(voxelIndices)
    }

    set fMRIEngine(signalCurve) signalCurve 
    set fMRIEngine(evCurve) evCurve 
    set fMRIEngine(voxelIndices) voxelIndices

    $fMRIEngine(timeCourseGraph) element create $fMRIEngine(signalCurve) \
        -label "response" -xdata xVecSig -ydata yVecSig
    $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(signalCurve) \
        -symbol none -color red -linewidth 1 
    $fMRIEngine(timeCourseGraph) element create $fMRIEngine(evCurve) \
        -label $fMRIEngine(curEVForPlotting) -xdata xVecEV -ydata yVecEV
    $fMRIEngine(timeCourseGraph) element configure $fMRIEngine(evCurve) \
        -symbol none -color blue -linewidth 1 

    # Voxel indices
    $fMRIEngine(timeCourseGraph) marker create text -text "Voxel: ($x,$y,$z)" \
        -coords {$totalVolumes $timeCourseYMax} \
        -yoffset 5 -xoffset -70 -name $fMRIEngine(voxelIndices) -under yes -bg white \
        -font fixed 

    set fMRIEngine(curPlotting) "Long"
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
    unset -nocomplain fMRIEngine(signalCurve)]
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
# int x the selected point's x
# int y the selected point's y
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
# string argstr the data string
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
