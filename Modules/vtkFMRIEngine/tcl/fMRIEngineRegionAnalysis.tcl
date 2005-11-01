#=auto==========================================================================
# (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
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
# FILE:        fMRIEngineROI.tcl
# PROCEDURES:  
#   fMRIEngineBuildUIForROITab parent
#   fMRIEngineBuildUIForROITasks parent
#   fMRIEngineBuildUIForROIChoose
#   fMRIEngineSelectLabelMap
#   fMRIEngineLoadLabelMap
#   fMRIEngineBuildUIForROIShape
#   fMRIEngineBuildUIForROIAnatomy
#   fMRIEngineSelectBG
#   fMRIEngineBuildUIForROIActivation
#   fMRIEngineCreateLabelMap
#   fMRIEngineCastActivation
#   fMRIEngineSetROITask task
#   fMRIEngineUpdateBGVolumeList
#   fMRIEngineUpdateLabelMapList
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROITab
# Creates UI for ROI tab 
# .ARGS
# windowpath parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROITab {parent} {
    global fMRIEngine Gui Module

    set f $Module(fMRIEngine,fROI)
    Notebook-create $f.fNotebook \
                    -pages {{Region Map} Stats} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1
 
    set w [Notebook-frame $f.fNotebook {Region Map}]
    fMRIEngineBuildUIForROIRegionMap $w
    set w [Notebook-frame $f.fNotebook Stats]
    fMRIEngineBuildUIForROIStats $w
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROITasks
# Creates UI for tasks in ROI 
# .ARGS
# windowpath parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROITasks {parent} {
    global fMRIEngine Gui

    frame $parent.fTop  -bg $Gui(backdrop)
    frame $parent.fBot  -bg $Gui(activeWorkspace) -height 300 
    pack $parent.fTop $parent.fBot \
        -side top -fill x -pady 2 -padx 5 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetup" 2 
    pack $f.bHelp -side left -padx 1 -pady 1 
 
    # Build pulldown task menu 
    # eval {label $f.l -text "Label map:"} $Gui(BLA)
    # pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # Paradigm design is default task 
    set taskList [list {Load} {New}]
    set df [lindex $taskList 0] 
    eval {menubutton $f.mbTask -text $df \
          -relief raised -bd 2 -width 33 \
          -indicatoron 1 \
          -menu $f.mbTask.m} $Gui(WMBA)
    eval {menu $f.mbTask.m} $Gui(WMA)
    pack  $f.mbTask -side left -pady 1 -padx $Gui(pad)

    # Save menubutton for config
    set fMRIEngine(gui,currentROITask) $f.mbTask

    # Add menu items
    set count 1
    set cList [list {Shape} {Anatomy} {Activation}]
    foreach mi $taskList {
        if {$mi == "New"} {
            $f.mbTask.m add cascade -label $mi -menu $f.mbTask.m.sub
            set m2 [eval {menu $f.mbTask.m.sub -tearoff 0} $Gui(WMA)]
            foreach c $cList {
                $m2 add command -label $c \
                    -command "fMRIEngineSetROITask {$c}"
            }
        } else {
            $f.mbTask.m add command -label $mi \
                -command "fMRIEngineSetROITask {$mi}"
        }
    }

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBot

    set fList [list {Load} {Shape} {Anatomy} {Activation}]
    set count 1
    foreach m $fList {
        # Makes a frame for each submodule
        frame $f.f$count -bg $Gui(activeWorkspace) 
        place $f.f$count -relwidth 1.0 -relx 0.0 -relheight 1.0 -rely 0.0 
        fMRIEngineBuildUIForROI$m $f.f$count

        set fMRIEngine(fROI$count) $f.f$count
        incr count
    }

    # raise the default one 
    raise $fMRIEngine(fROI1)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIChoose
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIChoose {parent} {
    global fMRIEngine Gui

    frame $parent.fTop -bg $Gui(activeWorkspace)
    pack $parent.fTop -side top -fill x -pady 5 -padx 5 

    set f $parent.fTop
    frame $f.fBox    -bg $Gui(activeWorkspace)
    frame $f.fButtons -bg $Gui(activeWorkspace)
    pack $f.fBox $f.fButtons -side top -fill x -pady 2 -padx 1 

    set f $parent.fTop.fBox
    DevAddLabel $f.l "Choose a loaded label map:"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set fMRIEngine(LMVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -selectmode simple \
        -yscrollcommand {$::fMRIEngine(LMVerScroll) set}
    set fMRIEngine(LMListBox) $f.lb
    $fMRIEngine(LMVerScroll) configure -command {$fMRIEngine(LMListBox) yview}

    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $fMRIEngine(LMListBox) -fill x -padx 1 -pady 1 \
        1,1 $fMRIEngine(LMVerScroll) -fill y -padx 1 -pady 1

    set f $parent.fTop.fButtons
    DevAddButton $f.bCompute "Select" "fMRIEngineSelectLabelMap" 10 
    grid $f.bCompute -padx 1 -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectLabelMap
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectLabelMap {} {
    global fMRIEngine 

    set curs [$fMRIEngine(LMListBox) curselection] 
    if {$curs != ""} {
        set name [$fMRIEngine(LMListBox) get $curs] 
        set id [MIRIADSegmentGetVolumeByName $name] 
        MainSlicesSetVolumeAll Label $id
        RenderAll
    }
}

 
proc fMRIEngineBuildUIForROILoad {parent} {
    global fMRIEngine Gui
   
    frame $parent.fLabel   -bg $Gui(activeWorkspace)
    frame $parent.fFile    -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $parent.fApply   -bg $Gui(activeWorkspace)
    pack $parent.fLabel $parent.fFile $parent.fApply -side top -pady 1 

    set f $parent.fLabel
    DevAddLabel $f.lLabel "Load a label map:"
    pack $f.lLabel -side top -pady 5 

    set f $parent.fFile
    DevAddFileBrowse $f fMRIEngine "lmFileName" "File Name:" \
        "" "xml .mrml" "\$Volume(DefaultDir)" "Open" "Browse for an xml file" "" "Absolute"

    set f $parent.fApply
    DevAddButton $f.bApply "Apply" "fMRIEngineLoadLabelMap" 12 
    pack $f.bApply -side top -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineLoadLabelMap
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineLoadLabelMap {} {
    global fMRIEngine File Mrml Volume

    set fileName $fMRIEngine(lmFileName)

    # Do nothing if the user cancelled
    if {$fileName == ""} {return}

    # Make it a relative prefix
    set File(filePrefix) [MainFileGetRelativePrefix $fileName]

    # If it's MRML instead of XML, then add the .mrml back
    if {[regexp {.*\.mrml$} $fileName] == 1} {
        set File(filePrefix) $File(filePrefix).mrml
    }

    # Prefix cannot be blank
    if {$File(filePrefix) == ""} {
        DevWarningWindow "No file to open specified."
        return
    }
    
    # Relative to root.
    # If it's MRML instead of XML, then don't add the .xml
    if {[regexp {.*\.mrml$} $File(filePrefix)] == 0} {
        set filename [file join $Mrml(dir) $File(filePrefix).xml]
    } else {
        set filename [file join $Mrml(dir) $File(filePrefix)]
    }

    # Bring nodes from a mrml file into the current tree
    MainMrmlImport $filename

    set size [llength $Volume(idList)]
    # In Volume(idList), the last one is the id for None volume
    # and the second last one is the id for the volume just added.
    set i [expr $size - 2]
    set id [lindex $Volume(idList) $i]
    # This is a labelmap
    MainSlicesSetVolumeAll Label $id

    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIShape
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIShape {parent} {
    global fMRIEngine Gui

    frame $parent.fTitle  -bg $Gui(activeWorkspace)
    frame $parent.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $parent.fTitle $parent.fTop -side top -fill x -pady 5 -padx 5 

    set f $parent.fTitle
    DevAddLabel $f.l "Create a label map from shape:"
    pack $f.l -side top -fill x -pady 2 -padx 5 

    set f $parent.fTop
    frame $f.fBox    -bg $Gui(activeWorkspace)
    frame $f.fButtons -bg $Gui(activeWorkspace)
    pack $f.fBox $f.fButtons -side top -fill x -pady 2 -padx 1 

    set f $parent.fTop.fBox
    DevAddLabel $f.l "Choose a background volume:"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set fMRIEngine(ShapeBGVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -selectmode simple \
        -yscrollcommand {$::fMRIEngine(ShapeBGVerScroll) set}
    set fMRIEngine(ShapeBGListBox) $f.lb
    $fMRIEngine(ShapeBGVerScroll) configure -command {$fMRIEngine(ShapeBGListBox) yview}

    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $fMRIEngine(ShapeBGListBox) -fill x -padx 1 -pady 1 \
        1,1 $fMRIEngine(ShapeBGVerScroll) -fill y -padx 1 -pady 1

    set f $parent.fTop.fButtons
    DevAddButton $f.bCompute "Go to Editor Module" "fMRIEngineSelectBG {ShapeBGListBox}" 20 
    grid $f.bCompute -padx 1 -pady 3
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIAnatomy
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIAnatomy {parent} {
    global fMRIEngine Gui

    frame $parent.fTitle  -bg $Gui(activeWorkspace)
    frame $parent.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $parent.fTitle $parent.fTop -side top -fill x -pady 5 -padx 5 

    set f $parent.fTitle
    DevAddLabel $f.l "Create a label map from anatomy:"
    pack $f.l -side top -fill x -pady 2 -padx 5 

    set f $parent.fTop
    frame $f.fBox    -bg $Gui(activeWorkspace)
    frame $f.fButtons -bg $Gui(activeWorkspace)
    pack $f.fBox $f.fButtons -side top -fill x -pady 2 -padx 1 

    set f $parent.fTop.fBox
    DevAddLabel $f.l "Choose a background volume:"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set fMRIEngine(AnatomyBGVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -selectmode simple \
        -yscrollcommand {$::fMRIEngine(AnatomyBGVerScroll) set}
    set fMRIEngine(AnatomyBGListBox) $f.lb
    $fMRIEngine(AnatomyBGVerScroll) configure -command {$fMRIEngine(AnatomyBGListBox) yview}

    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $fMRIEngine(AnatomyBGListBox) -fill x -padx 1 -pady 1 \
        1,1 $fMRIEngine(AnatomyBGVerScroll) -fill y -padx 1 -pady 1

    set f $parent.fTop.fButtons
    DevAddButton $f.bCompute "Go to Editor Module" "fMRIEngineSelectBG {AnatomyBGListBox}" 20 
    grid $f.bCompute -padx 1 -pady 3
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectBG
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectBG {lb} {
    global fMRIEngine 

    set curs [$fMRIEngine($lb) curselection] 
    if {$curs != ""} {
        set name [$fMRIEngine($lb) get $curs] 
        set id [MIRIADSegmentGetVolumeByName $name] 
        MainSlicesSetVolumeAll Back $id
        RenderAll

        # Switch to Editor module
        Tab Editor row1 Volumes 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIActivation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIActivation {parent} {
    global fMRIEngine Gui Module
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForLabelmap
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIRegionMap {parent} {
    global fMRIEngine Gui Label 

    frame $parent.fTasks   -bg $Gui(activeWorkspace) 
    pack $parent.fTasks -side top -fill x -pady 3 -padx 5 

    fMRIEngineBuildUIForROITasks $parent.fTasks
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIActivation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIActivation {parent} {
    global fMRIEngine Gui Label 

    frame $parent.fTop -bg $Gui(activeWorkspace)
    pack $parent.fTop -side top -fill x -pady 5 -padx 5 

    #---------------------------------
    # Make label map 
    #---------------------------------
    set f $parent.fTop
    DevAddButton $f.bApply "Create label map from activation" "fMRIEngineCreateLabelMap" 33 
    pack $f.bApply -side top -pady 1 -padx 5
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForROIStats
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIStats {parent} {
    global fMRIEngine Gui Label 

    frame $parent.fNote -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $parent.fShow -bg $Gui(activeWorkspace)
    frame $parent.fPlot -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $parent.fReset -bg $Gui(activeWorkspace)
    pack $parent.fNote -side top -fill x -pady 5 -padx 5 
    pack $parent.fShow -side top -fill x -pady 5 -padx 20 
    pack $parent.fPlot -side top -fill x -pady 5 -padx 5 
    pack $parent.fReset -side top -fill x -pady 5 -padx 20 

    set f $parent.fNote
    set fMRIEngine(roiStatsNote) "Select label(s) by\n clicking region(s)."
    eval {label $f.lNote -width 30 -bg $Gui(activeWorkspace) \
        -textvariable fMRIEngine(roiStatsNote)}
    pack $f.lNote -side top -fill x -pady 1 -padx 3 

    set f $parent.fShow
    DevAddButton $f.bShow "Show stats" "fMRIEngineDoROIStats t" 20 
    pack $f.bShow -side top -fill x -pady 1 -padx 3 

    set f $parent.fPlot
    frame $f.fConds        -bg $Gui(activeWorkspace)
    frame $f.fTimecourse   -bg $Gui(activeWorkspace)
    frame $f.fPeristimulus -bg $Gui(activeWorkspace)
    frame $f.fPlot         -bg $Gui(activeWorkspace)
    pack $f.fConds $f.fTimecourse $f.fPeristimulus $f.fPlot -side top -padx 2 -pady 1 

    set f $parent.fPlot.fConds
    DevAddLabel $f.lLabel "Condition:"

    set condList [list {none}]
    set df [lindex $condList 0] 
    eval {menubutton $f.mbType -text $df \
         -relief raised -bd 2 -width 15 \
         -indicatoron 1 \
         -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    foreach m $condList  {
        $f.mbType.m add command -label $m \
            -command ""
    }
    grid $f.lLabel $f.mbType -padx 1 -pady 3 
    # Save menubutton for config
    set fMRIEngine(gui,condsMenuButtonROI) $f.mbType
    set fMRIEngine(gui,condsMenuROI) $f.mbType.m

    set f $parent.fPlot.fTimecourse
    set param Long 
    set name {Timecourse}
    eval {radiobutton $f.r$param -width 25 -text $name \
        -variable fMRIEngine(tcPlottingOption) -value $param \
        -relief raised -offrelief raised -overrelief raised \
        -selectcolor white} $Gui(WEA)
    pack $f.r$param -side top -pady 2 

    set f $parent.fPlot.fPeristimulus
    set param Short 
    set name {Peristimulus histogram}
    eval {radiobutton $f.r$param -width 25 -text $name \
        -variable fMRIEngine(tcPlottingOption) -value $param \
        -relief raised -offrelief raised -overrelief raised \
        -selectcolor white} $Gui(WEA)
    pack $f.r$param -side top -pady 2 

    set f $parent.fPlot.fPlot
    DevAddButton $f.bPlot "Plot time series" "fMRIEnginePlotRegionTimecourse" 20 
    pack $f.bPlot -side top -pady 5 -padx 1 
    set fMRIEngine(tcPlottingOption) ""

    set f $parent.fReset
    DevAddButton $f.bReset "Reset labelmap" "fMRIEngineResetLabelMap" 20 
    pack $f.bReset -side top -fill x -pady 1 -padx 2 

}

#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateCondsForROIPlot
# Updates condition list for ROI timecourse plotting 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateCondsForROIPlot {} {
    global fMRIEngine

    set run $fMRIEngine(curRunForModelFitting)
    if {$run == "none"} {
        return
    }

    if {$run == "combined"} {
        set run 1
    }

    if {! [info exists fMRIEngine($run,namesOfConditionEVs)]} {
        return
    }

    if {[llength $fMRIEngine($run,namesOfConditionEVs)] > 0} {
        #--- wjp changed 09/21/05: filter out temporal derivative EV names
        $fMRIEngine(gui,condsMenuROI) delete 0 end
        set count 1 
        foreach name $fMRIEngine($run,namesOfConditionEVs) { 
            $fMRIEngine(gui,condsMenuROI) add command -label $name \
                -command "fMRIEngineSelectCondForROIPlot $name $count"

            fMRIEngineSelectCondForROIPlot $name $count 
            incr count
        }
    }
} 


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectCondForROIPlot
# 
# .ARGS
# string cond 
# int count
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectCondForROIPlot {cond count} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,condsMenuButtonROI) config -text $cond
    set fMRIEngine(curEVIndexForPlotting) $count 
    set fMRIEngine(curEVForPlotting) $cond 

    if {[info exists fMRIEngine(timeCourseToplevel)] &&
        $fMRIEngine(tcPlottingOption) == "Long"} {
            set x $fMRIEngine(x,voxelIndex)
            set y $fMRIEngine(y,voxelIndex)
            set z $fMRIEngine(z,voxelIndex)

            # re-plot due to condition switch
            fMRIEngineDrawPlotLong $x $y $z
    }
}


proc fMRIEngineResetLabelMap {} {
    global fMRIEngine Slicer Volume Interactor

    if {[info exists fMRIEngine(roiStatsLabelMapOriginal)]} {
        set s $Interactor(s)
        set foreNode [[Slicer GetForeVolume $s] GetMrmlNode]
        set v [$foreNode GetID]
        Volume($v,vol) SetImageData $fMRIEngine(roiStatsLabelMapOriginal)

        MainVolumesUpdate $v
        MainUpdateMRML
        RenderAll
    }
}

 
#-------------------------------------------------------------------------------
# .PROC fMRIEngineDoROIStats
# 
# .ARGS
# string type type = 1 for intensity; type = 2 for t 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDoROIStats {type} {

    set r 1 
    switch $type {
        "intensity" {
            set r [fMRIEngineComputeROIIntensityStats]
        }
        "t" {
            set r [fMRIEngineComputeROITStats]
        }
    }

    if {$r == 0} {
        fMRIEnginePlotROIStats $type
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineComputeROIIntensityStats
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineComputeROIIntensityStats {} {

    return 1 

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineComputeROITStats
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineComputeROITStats {} {
    global fMRIEngine Volume Slice

    set n $Volume(idNone)
    set tId $n 
    set lId $n 
    foreach s $Slice(idList) {
        if {$tId == $n} {
            set tId $Slice($s,backVolID)
        }
        if {$lId == $n} {
            set lId $Slice($s,labelVolID)
        }
    }

    if {$n == $tId} {
        DevErrorWindow "Put your activation volume into the background."
        return 1 
    }
    if {$n == $lId} {
        DevErrorWindow "Your label map is not visible."
        return 1 
    }

    # always uses a new instance of vtkActivationRegionStats 
    if {[info commands fMRIEngine(actROIStats)] != ""} {
        fMRIEngine(actROIStats) Delete
        unset -nocomplain fMRIEngine(actROIStats)
    }
    vtkActivationRegionStats fMRIEngine(actROIStats)

    fMRIEngine(actROIStats) AddInput [Volume($lId,vol) GetOutput]
    fMRIEngine(actROIStats) AddInput [Volume($tId,vol) GetOutput]
    fMRIEngine(actROIStats) SetLabel 16 
    fMRIEngine(actROIStats) Update 

    set count [fMRIEngine(actROIStats) GetCount] 
    if {$count < 1} {
        DevErrorWindow "No label has been selected."
        return 1
    }

    set fMRIEngine(t,count) $count 
    set fMRIEngine(t,max)   [fMRIEngine(actROIStats) GetMax] 
    set fMRIEngine(t,min)   [fMRIEngine(actROIStats) GetMin] 
    set fMRIEngine(t,mean)  [fMRIEngine(actROIStats) GetMean] 

    return 0

}
 

#-------------------------------------------------------------------------------
# .PROC fMRIEngineCloseROIStatsWindow
# Cleans up if the data window is closed 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCloseROIStatsWindow {} {
    global fMRIEngine

    if {[info exists fMRIEngine(roiStatsTable)]} {
        destroy $fMRIEngine(roiStatsTable)
        unset -nocomplain fMRIEngine(roiStatsTable)
    }

    if {[info exists fMRIEngine(roiStatsToplevel)]} {
        destroy $fMRIEngine(roiStatsToplevel)
        unset -nocomplain fMRIEngine(roiStatsToplevel)
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEnginePlotROIStats
# 
# .ARGS
# string type type can be "intensity," or "t." 
# .END
#-------------------------------------------------------------------------------
proc fMRIEnginePlotROIStats {type} {
    global fMRIEngine

    fMRIEngineCloseROIStatsWindow

    set w .roiStatsWin
    toplevel $w
    wm title $w "ROI Stats for t" 
    wm minsize $w 250 160 
    # wm geometry $w "+898+200" 
    # wm geometry $w "+850+200" 
    wm geometry $w "+320+440" 

    # data table headers
    label $w.count -text "Count" -font fixed
    label $w.max -text "Max" -font fixed
    label $w.min -text "Min" -font fixed
    label $w.mean -text "Mean" -font fixed
    blt::table $w \
        $w.count 0,0 $w.max 0,1 $w.min 0,2 $w.mean 0,3

    label $w.countVal -text $fMRIEngine($type,count) -font fixed
    label $w.maxVal -text $fMRIEngine($type,max) -font fixed
    label $w.minVal -text $fMRIEngine($type,min) -font fixed
    label $w.meanVal -text $fMRIEngine($type,mean) -font fixed

    # todo: expression didn't have numeric value
    set count 1
    blt::table $w \
        $w.countVal $count,0 $w.maxVal $count,1 \
        $w.minVal $count,2 $w.meanVal $count,3

    button $w.bClose -text "Close" -font fixed -command "fMRIEngineCloseROIStatsWindow"
    incr count
    blt::table $w $w.bClose $count,2 

    wm protocol $w WM_DELETE_WINDOW "fMRIEngineCloseROIStatsWindow" 
    set fMRIEngine(roiStatsToplevel) $w
    set fMRIEngine(roiStatsTable) $w.table
}


#-------------------------------------------------------------------------------
# .PROC fMRIEnginePlotRegionTimecourse
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEnginePlotRegionTimecourse {} {
    global fMRIEngine Slice Volume

    if {$fMRIEngine(tcPlottingOption) == ""} {
        DevErrorWindow "Timecourse or Peristimulus histogram?"
        return  
    }

    set nId $Volume(idNone)
    set lId $nId 
    foreach s $Slice(idList) {
        if {$lId == $nId} {
            set lId $Slice($s,labelVolID)
        }
    }

    if {$nId == $lId} {
        DevErrorWindow "Label map is not visible."
        return  
    }

    # always uses a new instance of vtkActivationRegionStats 
    if {[info commands fMRIEngine(actROIStats)] != ""} {
        fMRIEngine(actROIStats) Delete
        unset -nocomplain fMRIEngine(actROIStats)
    }
    vtkActivationRegionStats fMRIEngine(actROIStats)

    # Get all voxel indices in the labelmap volume, 
    # which are inside the defined ROI.
    fMRIEngine(actROIStats) AddInput [Volume($lId,vol) GetOutput]
    fMRIEngine(actROIStats) SetLabel 16 
    fMRIEngine(actROIStats) Update 

    set voxels [fMRIEngine(actROIStats) GetRegionVoxels]
    if {$voxels == ""} {
        DevErrorWindow "No region has been selected."
        return
    }

    fMRIEngine(actEstimator) SetRegionVoxels $voxels
    set fMRIEngine(timecourse) [fMRIEngine(actEstimator) GetRegionTimeCourse] 
    set fMRIEngine(timecoursePlot) "Region"
    fMRIEnginePlotTimecourse 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCreateLabelMap
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCreateLabelMap {} {

    set r [fMRIEngineCastActivation]
    # Got error!
    if {$r == 1} {
        return
    }

    fMRIEngineCreateLabelMapReal 
}

 
#-------------------------------------------------------------------------------
# .PROC fMRIEngineCreateLabelMapReal
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCreateLabelMapReal {} {
    global fMRIEngine Editor Ed Volume Slice Label Gui

    set n $Volume(idNone)
    foreach s $Slice(idList) {
        set id $Slice($s,foreVolID)
        if {$n != $id} {
            continue
        }
    }

    if {$n == $id} {
        DevErrorWindow "Put your activation volume into the foreground."
        return 1 
    }

    set Gui(progressText) "Creating label map..."
    puts $Gui(progressText)

    #---------------------------------
    # Create label map 
    #---------------------------------

    EditorEnter

    # Editor->Volumes->Setup
    #-----------------------
    EditorSetOriginal $id 
    EditorSetWorking "NEW" 

    set actName [Volume($fMRIEngine(activeActivationID),node) GetName]
    set t $fMRIEngine(tStat)
    # Say t = 3.3. Dot is not allowed in the volume name in slicer
    # the regsub replaces . by dot.
    regsub -all {\.} $t dot t 
    set Editor(nameWorking) "$actName-t$t-labelMap" 

    # Editor->Details->to 
    # (press) ->Th
    #-----------------------
    set e "EdThreshold"
    # Remember prev
    set prevID $Editor(activeID)
    # Set new
    set Editor(activeID) $e 
    set Editor(btn) $e 

    # Reset Display
    EditorResetDisplay
    RenderAll
    EditorExitEffect $prevID
    # execute enter procedure
    EditorUpdateEffect

    # Editor->Details->Th
    #-----------------------
    set Label(label) 2 
    set Ed($e,lower) 1 
    set Ed($e,upper) $Ed(EdThreshold,rangeHigh) 
    set Ed($e,interact) "3D"
    EdThresholdApply

    #---------------------------------
    # Identify islands in label map:
    # each label has a unique value (id)
    #---------------------------------

    # Editor->Details->to 
    # (press) ->II
    #-----------------------
    set e "EdIdentifyIslands"
    # Remember prev
    set prevID $Editor(activeID)
    set Editor(activeID) $e 
    set Editor(btn) $e 

    EditorResetDisplay
    RenderAll
    EditorExitEffect $prevID
    EditorUpdateEffect

    # Editor->Details->II
    #-----------------------
    set Ed($e,inputLabel) 0
    set Ed($e,scope) "3D"
    EdIdentifyIslandsApply

    #---------------------------------
    # Change all labels to high values 
    # so that they all display white in
    # slicer
    #---------------------------------
 
    # always uses a new instance of vtkLabelMapModifier
    if {[info commands fMRIEngine(labelMapModifier)] != ""} {
        fMRIEngine(labelMapModifier) Delete
        unset -nocomplain fMRIEngine(labelMapModifier)
    }
    vtkLabelMapModifier fMRIEngine(labelMapModifier)

    set id [MIRIADSegmentGetVolumeByName $Editor(nameWorking)] 
    set lmVol [Volume($id,vol) GetOutput] 
    fMRIEngine(labelMapModifier) SetInput $lmVol 
    fMRIEngine(labelMapModifier) Update 
    $lmVol DeepCopy [fMRIEngine(labelMapModifier) GetOutput]

    MainUpdateMRML
    RenderAll
    EditorExit

    set id $fMRIEngine(activeActivationID)
    Volume($id,node) InterpolateOff
    MainSlicesSetVolumeAll Back $id 
    MainUpdateMRML
    RenderAll
    puts "...done"

    return 0
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCastActivation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCastActivation {} {
    global fMRIEngine Volume Slice

    set n $Volume(idNone)
    foreach s $Slice(idList) {
        set id $Slice($s,foreVolID)
        if {$n != $id} {
            continue 
        }
    }

    if {$n == $id} {
        DevErrorWindow "Put your activation volume into the foreground."
        return 1 
    }

    set fMRIEngine(activeActivationID) $id

    # always uses a new instance of vtkActivationVolumeCaster 
    if {[info commands fMRIEngine(actVolumeCaster)] != ""} {
        fMRIEngine(actVolumeCaster) Delete
        unset -nocomplain fMRIEngine(actVolumeCaster)
    }
    vtkActivationVolumeCaster fMRIEngine(actVolumeCaster)

    set low [[Volume($id,vol) GetIndirectLUT] GetLowerThreshold]
    set high [[Volume($id,vol) GetIndirectLUT] GetUpperThreshold]
    fMRIEngine(actVolumeCaster) SetLowerThreshold $low
    fMRIEngine(actVolumeCaster) SetUpperThreshold $high

    fMRIEngine(actVolumeCaster) SetInput [Volume($id,vol) GetOutput] 

    set act [fMRIEngine(actVolumeCaster) GetOutput]
    $act Update

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    if {! [info exists fMRIEngine(actCastVolNameExt)]} {
        set fMRIEngine(actCastVolNameExt) 1 
    } else { 
        incr fMRIEngine(actCastVolNameExt)
    }
    set name "actCastVol-$fMRIEngine(actCastVolNameExt)"
    $n SetName "$name" 
    $n SetDescription "$name"

    eval Volume($i,node) SetSpacing [$act GetSpacing]
    Volume($i,node) SetScanOrder [Volume($fMRIEngine(firstMRMLid),node) GetScanOrder]
    Volume($i,node) SetNumScalars [$act GetNumberOfScalarComponents]
    set ext [$act GetWholeExtent]
    Volume($i,node) SetImageRange [expr 1 + [lindex $ext 4]] [expr 1 + [lindex $ext 5]]
    Volume($i,node) SetScalarType [$act GetScalarType]
    Volume($i,node) SetDimensions [lindex [$act GetDimensions] 0] [lindex [$act GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [Volume($i,node) GetScanOrder]

    Volume($i,vol) SetImageData $act

    Volume($i,vol) SetRangeLow [fMRIEngine(actVolumeCaster) GetLowRange] 
    Volume($i,vol) SetRangeHigh [fMRIEngine(actVolumeCaster) GetHighRange] 

    # set the lower threshold to the actLow 
    Volume($i,node) AutoThresholdOff
    Volume($i,node) ApplyThresholdOn
    Volume($i,node) SetLowerThreshold [fMRIEngine(actVolumeCaster) GetLowRange]
    Volume($i,node) InterpolateOff

    MainUpdateMRML
    MainSlicesSetVolumeAll Fore $i
    MainVolumesSetActive $i
    RenderAll

    return 0
} 


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetROITask
# Switches roi task 
# .ARGS
# string task the roi task 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetROITask {task} {
    global fMRIEngine
    
    set fMRIEngine(currentROITask) $task

    set l $task
    if {$task != "Load"} {
        set l "New"
    }

    # configure menubutton
    $fMRIEngine(gui,currentROITask) config -text $l

    set count -1 
    switch $task {
        "Load" {
            set count 1
        }
        "Shape" {
            set count 2 
        }
        "Anatomy" {
            set count 3 
        }
        "Activation" {
            set count 4 
        }
    }

    set f  $fMRIEngine(fROI$count)
    raise $f
    focus $f
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateBGVolumeList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateBGVolumeList {} {
    global fMRIEngine Volume
 
    set fMRIEngine(currentTab) "ROI"

    $fMRIEngine(AnatomyBGListBox) delete 0 end
    $fMRIEngine(ShapeBGListBox) delete 0 end
 
    foreach v $Volume(idList) {
        if {$v > 0} {
            set b [Volume($v,node) GetLabelMap]
            if {! $b} {
                set volName [Volume($v,node) GetName] 
                if {$volName != ""} {
                    $fMRIEngine(AnatomyBGListBox) insert end $volName
                    $fMRIEngine(ShapeBGListBox) insert end $volName
                }
            }
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateLabelMapList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateLabelMapList {} {
    global fMRIEngine Volume
 
    set fMRIEngine(currentTab) "ROI"

    $fMRIEngine(LMListBox) delete 0 end
    foreach v $Volume(idList) {
        if {$v > 0} {
            set b [Volume($v,node) GetLabelMap]
            if {$b} {
                set volName [Volume($v,node) GetName] 
                if {$volName != ""} {
                    $fMRIEngine(LMListBox) insert end $volName
                }
            }
        }
    }
}


proc fMRIEngineClickROI {x y} {
    global fMRIEngine Interactor Ed Editor Volume

    set s $Interactor(s)

    set backNode [[Slicer GetBackVolume $s] GetMrmlNode]
    set backName [$backNode GetName]
    if {$backName == "None"} {
        DevErrorWindow "Put your activation volume in the background (Bg)."
        return
    }

    set foreNode [[Slicer GetForeVolume $s] GetMrmlNode]
    set foreName [$foreNode GetName]
    set labelMap [$foreNode GetLabelMap]
    if {$foreName == "None" || $labelMap != "1"} {
        DevErrorWindow "Put your labelmap volume in the foreground (Fg)."
        return
    }

    set labelNode [[Slicer GetLabelVolume $s] GetMrmlNode]
    set labelName [$labelNode GetName]
    if {$labelName == "None"} {
        DevErrorWindow "Make your labelmap visible (in Lb)."
        return
    }

    # Save a copy of the labelmap volume
    if {! [info exists fMRIEngine(roiStatsLabelMapOriginal)]} {
        vtkImageData data

        set v [$foreNode GetID]
        data DeepCopy [Volume($v,vol) GetOutput]
        set fMRIEngine(roiStatsLabelMapOriginal) data
    }
     
    # The value of forePix is the labelmap of the region selected.
    set xs $x
    set ys $y
    scan [MainInteractorXY $s $xs $ys] "%d %d %d %d" xs ys x y
    set forePix [$Interactor(activeSlicer) GetForePixel $s $x $y]

    # The colors of all labels are white although they have 
    # different values. If a label is clicked, we are trying
    # to change its color (i.e. change its value). 

    # The background has a value > 0
    if {$forePix > 0} {
        EditorEnter

        set e EdChangeLabel
        set v [$foreNode GetID]
        EditorSetOriginal [$backNode GetID] 
 
        set prevID $Editor(activeID)
        set Editor(activeID) $e 
        set Editor(btn) $e 
        EditorResetDisplay
        RenderAll
        EditorExitEffect $prevID
        EditorUpdateEffect

        set Ed($e,inputLabel) $forePix
        # 16 = color of domino
        set Label(label) 16 
        EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

        set fg       $Ed($e,inputLabel)
        set fgNew    $Label(label)
        Ed(editor)   ChangeLabel $fg $fgNew
        Ed(editor)   SetInput ""
        Ed(editor)   UseInputOff

        EdUpdateAfterApplyEffect $v

        EditorExit
    }
}


proc fMRIEngineShowROIStats {} {
    global fMRIEngine 

    puts "do nothing in ROI."
}


