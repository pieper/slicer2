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
# FILE:        fMRIEngineInspect.tcl
# PROCEDURES:  
#   fMRIEngineScaleActivation no
#   fMRIEngineUpdateInspectTab
#   fMRIEngineInspectActVolume
#   fMRIEngineBuildUIForInspectTab parent
#   fMRIEngineUpdateEVsForPlotting
#   fMRIEngineSelectEVForPlotting ev count
#==========================================================================auto=
     
#-------------------------------------------------------------------------------
# .PROC fMRIEngineScaleActivation
# Scales the activation volume 
# .ARGS
# int no the scale index 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineScaleActivation {no} {
    global Volume fMRIEngine MultiVolumeReader Slicer

    if {! [info exists fMRIEngine(allPValues)]} {
        set i 20 
        while {$i >= 1} {
            set v [expr 1 / pow(10,$i)] 
            lappend fMRIEngine(allPValues) $v 
            if {$i == 2} {
                lappend fMRIEngine(allPValues) 0.05
            }
            set i [expr $i - 1] 
        }
        set i 1
        while {$i <= 9} {
            set v [expr 0.1 + 0.1 * $i]
            lappend fMRIEngine(allPValues) $v
            incr i
        }
    }

    if {[info exists fMRIEngine(currentActVolID)]} {
        vtkCDF cdf
        set dof [expr $MultiVolumeReader(noOfVolumes) - 1]
        # The index of list starts with 0
        set p [lindex $fMRIEngine(allPValues) [expr $no - 1]]
        set t [cdf p2t $p $dof]
       
        cdf Delete
        set fMRIEngine(pValue) $p
        set fMRIEngine(tStat) $t

        set id $fMRIEngine(currentActVolID) 
        if {$id > 0} {
            Volume($id,node) AutoThresholdOff
            Volume($id,node) ApplyThresholdOn

            Volume($id,node) SetLowerThreshold $t 
            MainVolumesSetParam ApplyThreshold 
            MainVolumesRender
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateInspectTab
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateInspectTab {} {
    global fMRIEngine Gui Volume

    set fMRIEngine(tcPlottingOption) "" 
    set fMRIEngine(currentTab) "Inspect"

    $fMRIEngine(inspectListBox) delete 0 end
    if {[info exists fMRIEngine(actVolumeNames)]} {
        set size [llength $fMRIEngine(actVolumeNames)]

        for {set i 0} {$i < $size} {incr i} {
            set name [lindex $fMRIEngine(actVolumeNames) $i] 
            if {$name != ""} {
                $fMRIEngine(inspectListBox) insert end $name
            }
        } 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineInspectActVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineInspectActVolume {} {
    global fMRIEngine Gui Volume

    unset -nocomplain fMRIEngine(currentActVolID)

    set curs [$fMRIEngine(inspectListBox) curselection] 
    if {$curs != ""} {
        set name [$fMRIEngine(inspectListBox) get $curs] 

        set id [MIRIADSegmentGetVolumeByName $name] 
        set fMRIEngine(currentActVolID) $id
        set fMRIEngine(currentActVolName) $name

        MainSlicesSetVolumeAll Fore $id
        MainVolumesSetActive $id
        MainVolumesRender
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForInspectTab
# Creates UI for the inspect tab 
# .ARGS
# windowpath parent
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForInspectTab {parent} {
    global fMRIEngine Gui

    frame $parent.fChoose    -bg $Gui(activeWorkspace)
    frame $parent.fThreshold -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $parent.fPlot      -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $parent.fChoose $parent.fThreshold $parent.fPlot \
        -side top -fill x -padx 5 -pady 3 

    #-------------------------------------------
    # Choose a volume tab 
    #-------------------------------------------
    set f $parent.fChoose
    DevAddLabel $f.l "Choose a volume:"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set fMRIEngine(inspectVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(inspectVerScroll) set}
    set fMRIEngine(inspectListBox) $f.lb
    $fMRIEngine(inspectVerScroll) configure -command {$fMRIEngine(inspectListBox) yview}

    DevAddButton $f.bGo "Select" "fMRIEngineInspectActVolume" 15 
 
    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $fMRIEngine(inspectListBox) -fill x -padx 1 -pady 1 \
        1,1 $fMRIEngine(inspectVerScroll) -fill y -padx 1 -pady 1 \
        2,0 $f.bGo -cspan 2 -padx 1 -pady 3

    #-------------------------------------------
    # Threshold tab 
    #-------------------------------------------
    set f $parent.fThreshold
    foreach m "Title Params" {
        frame $f.f${m} -bg $Gui(activeWorkspace)
        pack $f.f${m} -side top -fill x -pady $Gui(pad)
    }

    set f $parent.fThreshold.fTitle 
    DevAddButton $f.bHelp "?" "fMRIEngineHelpInspectActivationThreshold" 2
    DevAddLabel $f.lLabel "Activation thresholding:"
    grid $f.bHelp $f.lLabel -padx 1 -pady 3 

    set f $parent.fThreshold.fParams 
    frame $f.fStat  -bg $Gui(activeWorkspace) 
    frame $f.fScale -bg $Gui(activeWorkspace)
    pack $f.fStat $f.fScale -side top -fill x -padx 2 -pady 1 

    set f $parent.fThreshold.fParams.fStat 
    DevAddLabel $f.lPV "p Value:"
    DevAddLabel $f.lTS "t Stat:"
    set fMRIEngine(pValue) "none"
    set fMRIEngine(tStat) "none"
    eval {entry $f.ePV -width 10 -state readonly \
        -textvariable fMRIEngine(pValue)} $Gui(WEA)
    eval {entry $f.eTS -width 10 -state readonly \
        -textvariable fMRIEngine(tStat)} $Gui(WEA)
    grid $f.lPV $f.ePV -padx 1 -pady 2 -sticky e
    grid $f.lTS $f.eTS -padx 1 -pady 2 -sticky e

    set f $parent.fThreshold.fParams.fScale 
#   DevAddLabel $f.lactScale "Levels:"
    eval {scale $f.sactScale \
        -orient horizontal \
        -from 1 -to 30 \
        -resolution 1 \
        -bigincrement 10 \
        -length 155 \
        -command {fMRIEngineScaleActivation}} $Gui(WSA) {-showvalue 0}
    grid $f.sactScale -padx 1 -pady 1 
 
    #-------------------------------------------
    # Plot frame 
    #-------------------------------------------
    set f $parent.fPlot
    frame $f.fTitle  -bg $Gui(activeWorkspace)
    frame $f.fLong   -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $f.fChoice -bg $Gui(activeWorkspace)
    pack $f.fTitle -side top -fill x -padx 2 -pady 1 
    pack $f.fLong -side top -padx 2 -pady 1 
    pack $f.fChoice -side top -fill x -padx 2 -pady 1 

    set f $parent.fPlot.fTitle
    DevAddButton $f.bHelp "?" "fMRIEngineHelpInspectPlotting " 2
    DevAddLabel $f.lLabel "Time series plotting:"

    grid $f.bHelp $f.lLabel -padx 1 -pady 5 

    set f $parent.fPlot.fLong
    eval {radiobutton $f.rLong -width 10 -text "Timecourse" \
        -variable fMRIEngine(tcPlottingOption) -value Long \
        -relief raised -offrelief raised -overrelief raised \
        -selectcolor white} $Gui(WEA)

    set evList [list {none}]
    set df [lindex $evList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 11 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    foreach m $evList  {
        $f.mbType.m add command -label $m \
            -command ""
    }
    grid $f.rLong $f.mbType -padx 1 -pady 2 

    # Save menubutton for config
    set fMRIEngine(gui,evsMenuButtonForPlotting) $f.mbType
    set fMRIEngine(gui,evsMenuForPlotting) $f.mbType.m

    set f $parent.fPlot.fChoice
    foreach param "Short ROI" \
        name "{Peristimulus histogram} {ROI}" {
        eval {radiobutton $f.r$param -width 27 -text $name \
            -variable fMRIEngine(tcPlottingOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor white} $Gui(WEA)
        pack $f.r$param -side top -pady 2 
    } 

    $f.rROI configure -state disabled
    set fMRIEngine(tcPlottingOption) "" 
}

     
#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateEVsForPlotting
# Updates ev list for timecourse plotting 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateEVsForPlotting {} {
    global fMRIEngine

    set run $fMRIEngine(curRunForModelFitting)
    if {$run == "none"} {
        return
    }

    if {$run == "combined"} {
        set run 1
    }

    if {! [info exists fMRIEngine($run,namesOfEVs)]} {
        return
    }

    if {[llength $fMRIEngine($run,namesOfEVs)] > 0} {
        $fMRIEngine(gui,evsMenuForPlotting) delete 0 end
        set count 1 
        foreach name $fMRIEngine($run,namesOfEVs) { 
            $fMRIEngine(gui,evsMenuForPlotting) add command -label $name \
                -command "fMRIEngineSelectEVForPlotting $name $count"

            fMRIEngineSelectEVForPlotting $name $count 
            incr count
        }
    }
} 


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectEVForPlotting
# 
# .ARGS
# string ev
# int count
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectEVForPlotting {ev count} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,evsMenuButtonForPlotting) config -text $ev
    set fMRIEngine(curEVIndexForPlotting) $count 
    set fMRIEngine(curEVForPlotting) $ev 

    if {[info exists fMRIEngine(timeCourseToplevel)] &&
        $fMRIEngine(tcPlottingOption) == "Long"} {
            set x $fMRIEngine(x,voxelIndex)
            set y $fMRIEngine(y,voxelIndex)
            set z $fMRIEngine(z,voxelIndex)

            # re-plot due to ev switch
            fMRIEngineDrawPlotLong $x $y $z
    }
}

