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
# FILE:        FMRIEngine.tcl
# PROCEDURES:  
#   FMRIEngineInit
#   FMRIEngineBuildGUI
#   FMRIEngineViewGNULicense
#   FMRIEngineCloseGPLWindow
#   FMRIEngineBuildUIForDetectors the
#   FMRIEngineSetDetector the
#   FMRIEngineBuildUIForParadigm the
#   FMRIEngineBuildUIForSelect the
#   FMRIEngineSelectSequence
#   FMRIEngineUpdateSequences
#   FMRIEngineBuildUIForLoad the
#   FMRIEngineScaleActivation the
#   FMRIEngineSetImageFormat the
#   FMRIEngineEnter
#   FMRIEngineExit
#   FMRIEngineUpdateVolume the
#   FMRIEngineComputeActivationVolume 
#   FMRIEngineSetWindowLevelThresholds
#   FMRIEnginePushBindings 
#   FMRIEnginePopBindings 
#   FMRIEngineCreateBindings  
#   FMRIEngineSetVolumeName   
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
#  Description
# This module computes activation volume from a sequence of fMRI images. 
# To find it when you run the Slicer, click on More->FMRIEngine.
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# .PROC FMRIEngineInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineInit {} {
    global FMRIEngine Module Volume Model env

    set m FMRIEngine
    
    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "Computes fMRI activation volume."

    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Wendy Plesniak, SPL, wjp@bwh.harvard.edu; Haiying Liu, SPL, hliu@bwh.harvard.edu"
    set Module($m,category) "Application"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
    set Module($m,row1List) "Help Sequence Compute Display"
    set Module($m,row1Name) "{Help} {Sequence} {Compute} {Display}"
    set Module($m,row1,tab) Sequence 

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources all *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) FMRIEngineBuildVTK
    #
    #   All the options are:
    #
    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #   procRecallPresets  = Called when the user clicks one of the Presets buttons
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) FMRIEngineBuildGUI
    set Module($m,procEnter) FMRIEngineEnter
    set Module($m,procExit) FMRIEngineExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
#    set Module($m,depend) "MultiVolumeReader"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.36 $} {$Date: 2004/11/23 20:54:11 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set FMRIEngine(dir)  ""
    set FMRIEngine(trackMotion) 1
    set FMRIEngine(modulePath) "$env(SLICER_HOME)/Modules/vtkFMRIEngine"


    # For now, spew heavily.
    # this bypasses the command line setting of --verbose or -v
    # set Module(verbose) 0
    
    # Creates bindings
    FMRIEngineCreateBindings 

    # Source all appropriate tcl files here. 
    source "$FMRIEngine(modulePath)/tcl/FMRIEnginePlot.tcl"
    source "$FMRIEngine(modulePath)/tcl/FMRIEngineParadigmParser.tcl"
    source "$FMRIEngine(modulePath)/tcl/notebook.tcl"
}


# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildGUI
# Creates the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildGUI {} {
    global Gui FMRIEngine Module Volume Model

    # A frame has already been constructed automatically for each tab.
    # A frame named "FMRI" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(FMRIEngine,fFMRI)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # FMRI
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
    
    #-------------------------------------------
    # Help tab 
    #-------------------------------------------
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    set help "

    The fMRIEngine module is intended to process/display fMRI data.
    <P>
    <B>Sequence</B> allows you to load/select a sequence of fMRI \
    volumes to process.
    <P>
    <B>Compute</B> lets you to input a paradigm design, select \
    an activation detector and compute activation.
    <P>
    <B>Display</B> gives you the ability to view the activation \
    volume at different thresholds and dynamically plot any voxel \
    time course.
    <P>

    Check the file README.txt in the root directory of this module \
    for details about how to build and use the module.
    <BR><BR>
    ----------------------
    <BR><BR>
    (c) Copyright 2004 Massachusetts Institute of Technology (MIT) \
    All Rights Reserved.
    <P>
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    <P>

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    <P>

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags FMRIEngine $help
    MainHelpBuildGUI FMRIEngine

    set helpWidget $FMRIEngine(helpWidget) 
    $helpWidget configure -height 22

    set fHelp $Module(FMRIEngine,fHelp)
    set f $fHelp
    frame $f.fGPL -bg $Gui(activeWorkspace) 
    pack $f.fGPL -side top 

    DevAddButton $f.fGPL.bView "View GNU License" \
        "FMRIEngineViewGNULicense" 22
    pack $f.fGPL.bView -side left -pady 5 

    #-------------------------------------------
    # Sequence tab 
    #-------------------------------------------
    set fSequence $Module(FMRIEngine,fSequence)
    set f $fSequence
    frame $f.fOption -bg $Gui(activeWorkspace) 
    grid $f.fOption -row 0 -column 0 -sticky ew 
    
    #------------------------------
    # Sequence->Option frame
    #------------------------------
    set f $fSequence.fOption

    Notebook:create $f.fNotebook \
                    -pages {Select Load} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1
    set w [Notebook:frame $f.fNotebook Select]
    FMRIEngineBuildUIForSelect $w
    set w [Notebook:frame $f.fNotebook Load]
    FMRIEngineBuildUIForLoad $w

    #-------------------------------------------
    # Compute tab 
    #-------------------------------------------
    set fCompute $Module(FMRIEngine,fCompute)
    set f $fCompute
    frame $f.fOption -bg $Gui(activeWorkspace) 
    grid $f.fOption -row 0 -column 0 -sticky ew 
    
    #------------------------------
    # Compute->Option frame
    #------------------------------
    set f $fCompute.fOption

    Notebook:create $f.fNotebook \
                    -pages {Paradigm Detectors} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1

    set w [Notebook:frame $f.fNotebook Paradigm]
    FMRIEngineBuildUIForParadigm $w
    set w [Notebook:frame $f.fNotebook Detectors]
    FMRIEngineBuildUIForDetectors $w

    #-------------------------------------------
    # Display tab 
    #-------------------------------------------
    set fDisplay $Module(FMRIEngine,fDisplay)
    set f $fDisplay

    foreach m "ActThresholding TcPlotting" {
        frame $f.f${m} -bg $Gui(activeWorkspace) -relief groove -bd 3
        pack $f.f${m} -side top -fill x -pady $Gui(pad) 
    }

    # Act thresholding frame 
    #-----------------------
    set f $fDisplay.fActThresholding 
    foreach m "Title Params" {
        frame $f.f${m} -bg $Gui(activeWorkspace)
        pack $f.f${m} -side top -fill x -pady $Gui(pad)
    }

    set f $fDisplay.fActThresholding.fTitle 
    DevAddLabel $f.lTitle "Activation thresholding:"
    pack $f.lTitle -side top -fill x -padx $Gui(pad) 

    set f $fDisplay.fActThresholding.fParams 
    # Entry fields (the loop makes a frame for each variable)
    foreach param "pValue tStat actScale" \
        name "{p Value} {t Statistic} {Act Scale}" {

        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        if {$param == "actScale"} {
            eval {scale $f.e$param \
                -orient horizontal \
                -from 1 -to 20 \
                -resolution 1 \
                -bigincrement 10 \
                -length 155 \
                -command {FMRIEngineScaleActivation}} \
                $Gui(WSA) {-showvalue 1}
        } else {
            set FMRIEngine($param) "None"
            eval {entry $f.e$param -width 10 -state readonly \
                -textvariable FMRIEngine($param)} $Gui(WEA)
        }

        grid $f.l$param $f.e$param -padx $Gui(pad) -pady 2 -sticky e
        grid $f.e$param -sticky w
    }

    # Time course plotting frame 
    #---------------------------
    set f $fDisplay.fTcPlotting 
    DevAddLabel $f.lTitle "Time series plotting:"
    pack $f.lTitle -side top -fill x -pady $Gui(pad) 

    foreach param "None Long Short ROI" \
        name "{None} {Voxel-Natural} {Voxel-Combined} {ROI}" {

        eval {radiobutton $f.r$param -width 20 -text $name \
            -variable FMRIEngine(tcPlottingOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor blue} $Gui(WEA)
        pack $f.r$param -side top -pady $Gui(pad) 
    } 

    $f.rROI configure -state disabled
    set FMRIEngine(tcPlottingOption) None
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineViewGNULicense
# Displays GNU license information 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineViewGNULicense {} {
    global FMRIEngine Gui env

    if {[info exists FMRIEngine(GPLToplevel)] == 0 } {
        set w .tcren
        toplevel $w
        wm title $w "Gnu General Public License" 
        wm minsize $w 570 500 
        wm maxsize $w 570 500 
        wm geometry $w "+285+100" 
        wm protocol $w WM_DELETE_WINDOW "FMRIEngineCloseGPLWindow" 
        set FMRIEngine(GPLToplevel) $w

        frame $w.fWidget
        frame $w.fButton
        pack $w.fWidget $w.fButton -side top -padx 2 -fill both 

        set f $w.fWidget
        text $f.t -height 33 -wrap word \
            -yscrollcommand "$f.sy set" -font {Times 10}
        scrollbar $f.sy -orient vert -command "$f.t yview"
        set bg [$f.t cget -bg]
        pack $f.sy -side right -fill y
        pack $f.t -side left -fill both -expand true

        # Reads the data file
        set gplText [file join $env(SLICER_HOME) Modules vtkFMRIEngine gpl.txt]
        set fp [open $gplText r]
        set data [read $fp]
        regsub -all "\f" $data {} data 
        $f.t insert 1.0 $data 

        set f $w.fButton
        button $f.bDone -text "Done" -command "FMRIEngineCloseGPLWindow" -width 8 
        pack $f.bDone -side top -pady $Gui(pad) 
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCloseGPLWindow
# Closes GNU license information window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCloseGPLWindow {} {
    global FMRIEngine 

    destroy $FMRIEngine(GPLToplevel)
    unset -nocomplain FMRIEngine(GPLToplevel)
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForDetectors
# Creates UI for Detectors page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForDetectors {parent} {
    global FMRIEngine Gui

    frame $parent.fType   -bg $Gui(backdrop)
    frame $parent.fApply  -bg $Gui(activeWorkspace)
    frame $parent.fName   -bg $Gui(activeWorkspace)
    frame $parent.fStatus -bg $Gui(activeWorkspace)

    pack $parent.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)
    pack $parent.fName $parent.fApply $parent.fStatus -side top -fill x -pady $Gui(pad)

    # Type frame
    set f $parent.fType

    # Build pulldown menu image format 
    eval {label $f.l -text "Choose a detector:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # GLM is default format 
    set detectorList [list t-test GLM MI-1 MI-2]
    set df [lindex $detectorList 1] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)

    # Add menu items
    foreach m $detectorList  {
        $f.mbType.m add command -label $m \
            -command "FMRIEngineSetDetector $m"
    }

    # save menubutton for config
    set FMRIEngine(gui,mbActDetector) $f.mbType

    # Name frame
    set f $parent.fName
    DevAddLabel $f.label "Volume Name:"
    eval {entry $f.eName -width 20 \
                -textvariable FMRIEngine(actVolName)} $Gui(WEA)
    pack $f.label $f.eName -side left -expand false -fill x -padx 6
    bind $f.eName <Enter> "FMRIEngineComputeActivationVolume"
    TooltipAdd $f.eName "Input a name for your activation volume."
 
    # Apply frame
    set f $parent.fApply
    DevAddButton $f.bApply "Compute" "FMRIEngineComputeActivationVolume" 10 
    grid $f.bApply -padx $Gui(pad)

    # Status frame    
    set f $parent.fStatus
    set FMRIEngine(computeStatus) ""
    eval {label $f.eName -textvariable FMRIEngine(computeStatus) -width 50} $Gui(WLA)
    pack $f.eName -side left -padx 0 -pady 50 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetDetector
# Switches activation detector 
# .ARGS
# actDetector the activation detector 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetDetector {detector} {
    global FMRIEngine
    
    set FMRIEngine(actDetector) $detector

    # configure menubutton
    $FMRIEngine(gui,mbActDetector) config -text $detector 

#    set f  $FMRIEngine(f${imgFormat})
#    raise $f
#    focus $f
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForParadigm
# Creates UI for Paradigm page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForParadigm {parent} {
    global FMRIEngine Gui

    Notebook:create $parent.fNotebook \
                    -pages {{Load from file} {Input from user}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $parent.fNotebook -fill both -expand 1

    set fLoad [Notebook:frame $parent.fNotebook {Load from file}]
    set fInput [Notebook:frame $parent.fNotebook {Input from user}]

    #-------------------------------------------
    # Load tab 
    #-------------------------------------------
    frame $fLoad.fFile  -bg $Gui(activeWorkspace)
    pack $fLoad.fFile -side top -fill x -pady $Gui(pad)
    DevAddFileBrowse $fLoad.fFile FMRIEngine "paradigmFileName" \
        "Paradigm design file:" "" "txt" "\$Volume(DefaultDir)" \
        "Open" "Browse for a text file" "" "Absolute" 

    DevAddButton $fLoad.bLoad "Load" "FMRIEngineParseParadigm" 10 
    pack $fLoad.bLoad -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Input tab 
    #-------------------------------------------
    frame $fInput.fGrid  -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $fInput.fGrid -side top -fill x -pady $Gui(pad)
    set f $fInput.fGrid
    
    # Entry fields (the loop makes a frame for each variable)
    set FMRIEngine(paramNames) [list {Total Volumes} {Number Of Conditions} \
        {Volumes Per Baseline} {Volumes Per Condition} {Volumes At Start} \
        {Starts With}]
    set FMRIEngine(paramVariables) [list tVols nStims bVols cVols sVols start]
    set i 0      
    set len [llength $FMRIEngine(paramNames)]
    while {$i < $len} { 

        set name [lindex $FMRIEngine(paramNames) $i]
        set param [lindex $FMRIEngine(paramVariables) $i]
 
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 8 -textvariable FMRIEngine($param)} $Gui(WEA)

        grid $f.l$param $f.e$param -padx $Gui(pad) -pady $Gui(pad) -sticky e
        grid $f.e$param -sticky w
        incr i
    }

    DevAddButton $f.bStore "Done" "FMRIEngineStoreParadigm" 10 
    TooltipAdd $f.bStore "Hit this button if you finish your input."
 
    DevAddButton $f.bSave "Save" "FMRIEngineSaveParadigm" 8 
    TooltipAdd $f.bSave "Save all values into a paradigm design file."
 
    grid $f.bStore $f.bSave -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.bSave -sticky w
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForSelect
# Creates UI for Select page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForSelect {parent} {
    global FMRIEngine Gui

    frame $parent.fTop    -bg $Gui(activeWorkspace) 
    frame $parent.fMiddle -bg $Gui(activeWorkspace) 
    frame $parent.fBottom -bg $Gui(activeWorkspace) 
    frame $parent.fSpace -bg $Gui(activeWorkspace) -height 83  
    frame $parent.fLogo -bg $Gui(activeWorkspace)  
    pack $parent.fTop $parent.fMiddle -side top -padx $Gui(pad) 
    pack $parent.fBottom -side top -pady 15 
    pack $parent.fSpace $parent.fLogo -side top 

    set f $parent.fTop
    DevAddLabel $f.l "Available sequences:"
    listbox $f.lb -height 3 -bg $Gui(activeWorkspace) 
    pack $f.l $f.lb -side top -pady $Gui(pad)   

    set FMRIEngine(seqsListBox) $f.lb

    set f $parent.fMiddle
    DevAddButton $f.bSelect "Select" "FMRIEngineSelectSequence" 10 
    TooltipAdd $f.bSelect "Select a sequence."
 
    DevAddButton $f.bUpdate "Update" "FMRIEngineUpdateSequences" 10 
    TooltipAdd $f.bUpdate "Update the sequence list."
 
    pack $f.bUpdate $f.bSelect -side left -expand 1 -pady $Gui(pad) -padx $Gui(pad) -fill both

    # The Navigate frame
    set f $parent.fBottom

    DevAddButton $f.bSet "Set Window/Level/Thresholds" \
        "FMRIEngineSetWindowLevelThresholds" 30
    TooltipAdd $f.bSet \
        "Set window, level and low/high threshold\n\
        for the first volume within the selected \n\
        sequence. Hit this button to set the same \n\
        values for the entire sequence.             "
 
    DevAddLabel $f.lVolNo "Vol Index:"
    eval { scale $f.sSlider \
        -orient horizontal \
        -from 0 -to 0 \
        -resolution 1 \
        -bigincrement 10 \
        -length 130 \
        -state active \
        -command {FMRIEngineUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}

    set FMRIEngine(slider) $f.sSlider
    TooltipAdd $f.sSlider \
        "Slide this scale to navigate the selected multi-volume sequence."
 
    #The "sticky" option aligns items to the left (west) side
    grid $f.bSet -row 0 -column 0 -columnspan 2 -padx 5 -pady 3 -sticky w
    grid $f.lVolNo -row 1 -column 0 -padx 1 -pady 1 -sticky w
    grid $f.sSlider -row 1 -column 1 -padx 1 -pady 1 -sticky w

    set f $parent.fLogo
    set uselogo [image create photo -file \
        $FMRIEngine(modulePath)/tcl/images/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
        -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 0 -pady \
        $Gui(pad) -expand 0
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSelectSequence
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSelectSequence {} {
    global FMRIEngine Ibrowser MultiVolumeReader

    set ci [$FMRIEngine(seqsListBox) cursel]
    set size [$FMRIEngine(seqsListBox) size]

    if {[string length $ci] == 0} {
        if {$size > 1} {
            DevErrorWindow "Please select a sequence."
            return
        } else {
            set ci 0 
        }
    }

    set cc [$FMRIEngine(seqsListBox) get $ci]
    set l [string trim $cc]

    if {$l == "none"} {
        DevErrorWindow "No sequence available."
        return
    } elseif {$l == "Loaded-in-fMRIEngine"} {
        set FMRIEngine(firstMRMLid) $MultiVolumeReader(firstMRMLid) 
        set FMRIEngine(lastMRMLid) $MultiVolumeReader(lastMRMLid)
        set FMRIEngine(volumeExtent) $MultiVolumeReader(volumeExtent) 
        set FMRIEngine(noOfVolumes) $MultiVolumeReader(noOfVolumes) 
    } else {
        set index [string last "-" $l]
        set start [expr $index + 1]
        set id [string range $l $start end]
        set id [string trim $id]
        set FMRIEngine(firstMRMLid) $Ibrowser($id,firstMRMLid)
        set FMRIEngine(lastMRMLid) $Ibrowser($id,lastMRMLid)

        set ext [[Volume($FMRIEngine(firstMRMLid),vol) GetOutput] GetWholeExtent]
        set FMRIEngine(volumeExtent) $ext 
        set FMRIEngine(noOfVolumes) \
            [expr $FMRIEngine(lastMRMLid) - $FMRIEngine(firstMRMLid) + 1]
    }

    # Sets range for the volume slider
    $FMRIEngine(slider) configure -from 1 -to $FMRIEngine(noOfVolumes)
    # Sets the first volume in the sequence as the active volume
    MainVolumesSetActive $FMRIEngine(firstMRMLid)
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateSequences
# Updates sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateSequences {} {
    global FMRIEngine Ibrowser MultiVolumeReader 

    # clears the listbox
    set size [$FMRIEngine(seqsListBox) size]
    $FMRIEngine(seqsListBox) delete 0 [expr $size - 1]
 
    # checks sequences from Ibrowser
    set b1 [info exists Ibrowser(idList)] 
    set n1 [expr {$b1 == 0 ? 0 : [llength $Ibrowser(idList)]}]

    # checks sequence loaded from fMRIEngine
    set b2 [info exists MultiVolumeReader(noOfVolumes)] 
    set n2 [expr {$b2 == 0 ? 0 : $MultiVolumeReader(noOfVolumes)}]

    set n [expr $n1 + $n2]
    if {$n > 1} {
        set i 1 
        while {$i < $n1} {
            set id [lindex $Ibrowser(idList) $i]
            # $FMRIEngine(seqsListBox) insert end "$Ibrowser($id,name) \[id: $id\]" 
            $FMRIEngine(seqsListBox) insert end "$Ibrowser($id,name)-$id" 
            incr i
        }

        if {$n2 > 1} {
            $FMRIEngine(seqsListBox) insert end "Loaded-in-fMRIEngine" 
        }
    } else {
        $FMRIEngine(seqsListBox) insert end none 
    }
}

  
#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForLoad
# Creates UI for Load page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForLoad {parent} {
    global FMRIEngine Gui

    frame $parent.fTop -bg $Gui(activeWorkspace)
    pack $parent.fTop -side top 
 
    set f $parent.fTop

    # error if no private segment
    if {[catch "package require MultiVolumeReader"]} {
        DevAddLabel $f.lError \
            "Loading function is disabled\n\
            due to the unavailability\n\
            of module MultiVolumeReader." 
        pack $f.lError -side top -pady 30
        return
    }

    MultiVolumeReaderBuildGUI $f
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineScaleActivation
# Scales the activation volume 
# .ARGS
# no the scale index 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineScaleActivation {no} {
    global Volume FMRIEngine

    if {[info exists FMRIEngine(allPValues)] == 0} {
        
        set i 10 
        while {$i >= 1} {
            set v [expr 1 / pow(10,$i)] 
            lappend FMRIEngine(allPValues) $v 
            if {$i == 2} {
                lappend FMRIEngine(allPValues) 0.05
            }
            set i [expr $i - 1] 
        }
        set i 1
        while {$i <= 9} {
            set v [expr 0.1 + 0.1 * $i]
            lappend FMRIEngine(allPValues) $v
            incr i
        }
    }

    if {[info exists FMRIEngine(noOfVolumes)] == 1} {

        vtkCDF cdf
        set dof [expr $FMRIEngine(noOfVolumes) - 1]
        # The index of list starts with 0
        set p [lindex $FMRIEngine(allPValues) [expr $no - 1]]
        set t [cdf p2t $p $dof]
       
        cdf Delete
        set FMRIEngine(pValue) $p
        set FMRIEngine(tStat) $t

        set index [MIRIADSegmentGetVolumeByName $FMRIEngine(actVolName)] 
        if {$index > 0} {

            # map the t value into the range between 1 and 100
            set value [expr 100 * ($t - $FMRIEngine(actLow)) / \
                ($FMRIEngine(actHigh) - $FMRIEngine(actLow))]
     
            Volume($index,node) AutoThresholdOff
            Volume($index,node) ApplyThresholdOn
            Volume($index,node) SetLowerThreshold [expr round($value)] 
            MainVolumesSetParam ApplyThreshold 
            MainVolumesRender
            # puts "low = $FMRIEngine(actLow)"
            # puts "high = $FMRIEngine(actHigh)"
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetImageFormat
# Switches image format 
# .ARGS
# imgFormat the image format
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetImageFormat {imgFormat} {
    global Volume FMRIEngine
    
    set FMRIEngine(imageFormat) $imgFormat

    # configure menubutton
    $FMRIEngine(gui,mbImgFormat) config -text $imgFormat 

    set f  $FMRIEngine(f${imgFormat})
    raise $f
    focus $f
#    focus $Volume(f$Volume(propertyType))
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineEnter
# Called when this module is entered by the user. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineEnter {} {
    global FMRIEngine

    #--- push all event bindings onto the stack.
    FMRIEnginePushBindings

    #--- For now, hide it; not fully integrated yet.
    #if {[winfo exists $toplevelName]} {
    #    lower $toplevelName
    #    wm iconify $toplevelName
    #}
}
 

#-------------------------------------------------------------------------------
# .PROC FMRIEngineExit
# Called when this module is exited by the user
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineExit {} {

    # pop event bindings
    FMRIEnginePopBindings
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateVolume
# Updates image volume as user moves the slider 
# .ARGS
# volumeNo the volume number
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateVolume {volumeNo} {
    global FMRIEngine

    if {$volumeNo == 0} {
#        DevErrorWindow "Volume number must be greater than 0."
        return
    }

    set v [expr $volumeNo-1]
    set id [expr $FMRIEngine(firstMRMLid)+$v]

    MainSlicesSetVolumeAll Back $id 
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineComputeActivationVolume 
# Computes activation volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineComputeActivationVolume {} {
    global FMRIEngine Module Volume

    # Checks if volumes have been loaded
    if {[info exists FMRIEngine(firstMRMLid)] == 0} {
        DevErrorWindow "Please load volumes first."
        return
    }

    # Checks if a name is entered for the act volume 
    string trim $FMRIEngine(actVolName)
    if {$FMRIEngine(actVolName) == ""} {
        DevErrorWindow "Please input a name for the activation volume."
        return
    }

    # Add volumes into vtkActivationVolumeGenerator
    if {[info commands FMRIEngine(actvolgen)] != ""} {
        FMRIEngine(actvolgen) Delete
        unset -nocomplain FMRIEngine(actvolgen)
    }
    vtkActivationVolumeGenerator FMRIEngine(actvolgen)
    set id $FMRIEngine(firstMRMLid) 
    while {$id <= $FMRIEngine(lastMRMLid)} {
        Volume($id,vol) Update
        FMRIEngine(actvolgen) AddInput [Volume($id,vol) GetOutput]
        incr id
    }

    # Checks if paradigm file has been properly parsed 
    if {! [info exists FMRIEngine(paradigm)]} {
        DevErrorWindow "Paradigm design doesn't exist."
        return
    }

    set stimSize [FMRIEngine(stimulus) GetNumberOfTuples]
    set volSize [FMRIEngine(actvolgen) GetNumberOfInputs]
    if {$stimSize != $volSize} {
        DevErrorWindow "Stimulus size ($stimSize) is not same as no of volumes ($volSize)."
        return
    }

    set FMRIEngine(computeStatus) \
        "The activation volume is being\n\
        computed. This may take a while.\n\
        Upon completion, it will be displayed\n\
        as the foreground image.
        " 

    puts "Computing Act Volume $FMRIEngine(actVolName)..." 

    if {[info commands FMRIEngine(detector)] != ""} {
        FMRIEngine(detector) Delete
        unset -nocomplain FMRIEngine(detector)
    }
    vtkActivationDetector FMRIEngine(detector)
    FMRIEngine(detector) SetDetectionMethod 1
    FMRIEngine(detector) SetNumberOfPredictors [lindex $FMRIEngine(paradigm) 1] 
    FMRIEngine(detector) SetStimulus FMRIEngine(stimulus) 

    if {[info exists FMRIEngine(lowerThreshold)]} {
        FMRIEngine(actvolgen) SetLowerThreshold $FMRIEngine(lowerThreshold)
    }
    FMRIEngine(actvolgen) SetDetector FMRIEngine(detector)  
    FMRIEngine(actvolgen) Update
    set FMRIEngine(actLow) [FMRIEngine(actvolgen) GetLowRange] 
    set FMRIEngine(actHigh) [FMRIEngine(actvolgen) GetHighRange] 

    set act [FMRIEngine(actvolgen) GetOutput]
    $act Update

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    $n SetName $FMRIEngine(actVolName) 
    $n SetDescription $FMRIEngine(actVolName) 

    eval Volume($i,node) SetSpacing [$act GetSpacing]
    Volume($i,node) SetScanOrder [Volume($FMRIEngine(firstMRMLid),node) GetScanOrder]
    Volume($i,node) SetNumScalars [$act GetNumberOfScalarComponents]
    set ext [$act GetWholeExtent]
    Volume($i,node) SetImageRange [expr 1 + [lindex $ext 4]] [expr 1 + [lindex $ext 5]]
    Volume($i,node) SetScalarType [$act GetScalarType]
    Volume($i,node) SetDimensions [lindex [$act GetDimensions] 0] [lindex [$act GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [Volume($i,node) GetScanOrder]

    Volume($i,vol) SetImageData $act
    MainSlicesSetVolumeAll Fore $i
    MainVolumesSetActive $i
    MainUpdateMRML
    RenderAll

    set FMRIEngine(computeStatus) ""
    puts "...done"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetWindowLevelThresholds
# For a time series, set window, level, and low/high thresholds for all volumes
# with the first volume's values
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetWindowLevelThresholds {} {
   global FMRIEngine Volume 

    if {[info exists FMRIEngine(noOfVolumes)] == 0} {
        return
    }

    set low [Volume($FMRIEngine(firstMRMLid),node) GetLowerThreshold]
    set win [Volume($FMRIEngine(firstMRMLid),node) GetWindow]
    set level [Volume($FMRIEngine(firstMRMLid),node) GetLevel]
    set FMRIEngine(lowerThreshold) $low

    set i $FMRIEngine(firstMRMLid)
    while {$i <= $FMRIEngine(lastMRMLid)} {
        # If AutoWindowLevel is ON, 
        # we can't set new values for window and level.
        Volume($i,node) AutoWindowLevelOff
        Volume($i,node) SetWindow $win 
        Volume($i,node) SetLevel $level 
 
        Volume($i,node) AutoThresholdOff
        Volume($i,node) ApplyThresholdOn
        Volume($i,node) SetLowerThreshold $low 
        incr i
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEnginePushBindings 
# Pushes onto the event stack a new event manager that
# deals with events when the FMRIEngine module is active
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEnginePushBindings {} {
   global Ev Csys

    EvActivateBindingSet FMRISlice0Events
    EvActivateBindingSet FMRISlice1Events
    EvActivateBindingSet FMRISlice2Events
}


#-------------------------------------------------------------------------------
# .PROC FMRIEnginePopBindings 
# Removes bindings when FMRIEnginer module is inactive
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEnginePopBindings {} {
    global Ev Csys

    EvDeactivateBindingSet FMRISlice0Events
    EvDeactivateBindingSet FMRISlice1Events
    EvDeactivateBindingSet FMRISlice2Events
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCreateBindings  
# Creates FMRIEngine event bindings for the three slice windows 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCreateBindings {} {
    global Gui Ev

    EvDeclareEventHandler FMRIEngineSlicesEvents <ButtonPress-1> \
        {set FMRIEngine(trackMotion) [expr {! $FMRIEngine(trackMotion)}]}
 
    EvDeclareEventHandler FMRIEngineSlicesEvents <Motion> \
        { FMRIEnginePopUpPlot %x %y }
           
    EvAddWidgetToBindingSet FMRISlice0Events $Gui(fSl0Win) {FMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice1Events $Gui(fSl1Win) {FMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice2Events $Gui(fSl2Win) {FMRIEngineSlicesEvents}    
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetVolumeName   
# Saves current volume name 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetVolumeName {} {
    global Volume FMRIEngine 

    set FMRIEngine(name) $Volume(name)
}
