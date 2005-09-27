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
#   fMRIEngineBuildUIForROIBlob
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
    global fMRIEngine Gui

    frame $parent.fTasks   -bg $Gui(activeWorkspace) 
    pack $parent.fTasks -side top -fill x -pady 3 -padx 5 

    fMRIEngineBuildUIForROITasks $parent.fTasks
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
    eval {label $f.l -text "Label map:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # Paradigm design is default task 
    set taskList [list {Load} {Choose} {Create}]
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
    set cList [list {Shape} {Anatomy} {Blob}]
    foreach mi $taskList {
        if {$mi == "Create"} {
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

    set fList [list {Load} {Choose} {Shape} {Anatomy} {Blob}]
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
    DevAddButton $f.bCompute "Select" "fMRIEngineSelectBG {ShapeBGListBox}" 10 
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
    DevAddButton $f.bCompute "Select" "fMRIEngineSelectBG {AnatomyBGListBox}" 10 
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
# .PROC fMRIEngineBuildUIForROIBlob
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForROIBlob {parent} {
    global fMRIEngine Gui Label

    frame $parent.fColor  -bg $Gui(activeWorkspace)
    frame $parent.fTop -bg $Gui(activeWorkspace)
    pack $parent.fColor $parent.fTop -side top -fill x -pady 5 -padx 5 

    set f $parent.fColor
    DevAddLabel $f.lColor "Color: "    
    #--- popup panel for color selection.
    scan $Label(diffuse) "%f %f %f" r g b
    set r [expr round($r * 255)]
    set g [expr round($g * 255)]
    set b [expr round($b * 255)]
    set fMRIEngine(labelMapColor) [format \#%02X%02X%02X $r $g $b]
    eval { button $f.bColorSelection -width 10 -textvariable Label(name) \
               -command "ShowColors" -bg $Gui(activeWorkspace) -fg black}
    TooltipAdd $f.bColorSelection "Choose a color for the label map."
    grid $f.lColor $f.bColorSelection -sticky w -row 1 -columnspan 2 -pady 2 -padx 1
    lappend Label(colorWidgetList) $f.bColorSelection

    set f $parent.fTop
    DevAddButton $f.bCreate "Create label map from activation blob" "fMRIEngineCreateLabelMap" 35 
    pack $f.bCreate -side top -pady 2 -padx 5
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
    global fMRIEngine Editor Ed Volume Slice 

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

    #---------------------------------
    # Editor->Volumes->Setup
    #---------------------------------
    EditorSetOriginal $id 
    EditorSetWorking "NEW" 
    set Editor(nameWorking) "Working"

    #---------------------------------
    # Editor->Details->to (press) ->Th
    #---------------------------------
    set e "EdThreshold"
    # Remember prev
    set prevID $Editor(activeID)
    # Set new
    set Editor(activeID) $e 
    set Editor(btn) $e 

    # Reset Display
    if {$e != "EdNone"} {
        EditorResetDisplay
        RenderAll
    }

    if {$e != $prevID} {
        EditorExitEffect $prevID
        # execute enter procedure
        EditorUpdateEffect
    }

    #---------------------------------
    # Editor->Details->Th
    #---------------------------------
    set Ed(EdThreshold,lower) 1
    set Ed(EdThreshold,upper) $Ed(EdThreshold,rangeHigh)
    set Ed(EdThreshold,interact) "Slices"
    EdThresholdApply

    MainSlicesSetVolumeAll Fore Volume(idNone) 

    set id $fMRIEngine(activeActivationID)
    Volume($id,node) InterpolateOff
    MainSlicesSetVolumeAll Back $id 

    RenderAll

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
    if {$task != "Choose" && $task != "Load"} {
        set l "Create"
    }

    # configure menubutton
    $fMRIEngine(gui,currentROITask) config -text $l

    set count -1 
    switch $task {
        "Load" {
            set count 1
        }
        "Choose" {
            set count 2
            fMRIEngineUpdateLabelMapList
        }
        "Shape" {
            set count 3
        }
        "Anatomy" {
            set count 4 
        }
        "Blob" {
            set count 5 
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

