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
# FILE:        fMRIEngine.tcl
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
    set Module($m,row1List) "Help Sequence Setup Compute Inspect"
    set Module($m,row1Name) "{Help} {Sequence} {Set Up} {Compute} {Inspect}"
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
        {$Revision: 1.2 $} {$Date: 2005/03/28 16:20:39 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set FMRIEngine(dir)  ""
    set FMRIEngine(modulePath) "$env(SLICER_HOME)/Modules/vtkFMRIEngine"

    set FMRIEngine(baselineEVsAdded) 0
 

    # For now, spew heavily.
    # this bypasses the command line setting of --verbose or -v
    # set Module(verbose) 0
    
    # Creates bindings
    FMRIEngineCreateBindings 

    # Source all appropriate tcl files here. 
    source "$FMRIEngine(modulePath)/tcl/notebook.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEnginePlot.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineModel.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineHelpText.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineContrasts.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineModelView.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineSignalModeling.tcl"
    source "$FMRIEngine(modulePath)/tcl/fMRIEngineUserInputForModelView.tcl"
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

    # error if no private segment
    if { [catch "package require BLT"] } {
        DevErrorWindow "Must have the BLT package for building fMRIEngine UI \
        and plotting time course."
        return
    }

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
    set b $Module(FMRIEngine,bHelp)
    bind $b <1> "FMRIEngineUpdateHelpTab" 

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
    <B>Inspect</B> gives you the ability to view the activation \
    volume at different thresholds and dynamically plot any voxel \
    time course.
    <P>

    Check the file README.txt in the docs directory of this module \
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
    set b $Module(FMRIEngine,bSequence)
    bind $b <1> "FMRIEngineUpdateSequenceTab" 

    set fSequence $Module(FMRIEngine,fSequence)
    set f $fSequence
    frame $f.fOption -bg $Gui(activeWorkspace) 
    grid $f.fOption -row 0 -column 0 -sticky ew 
    
    #------------------------------
    # Sequence->Option frame
    #------------------------------
    set f $fSequence.fOption

    Notebook-create $f.fNotebook \
                    -pages {Load Select} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    Notebook-setCallback $f.fNotebook \
        FMRIEngineUpdateSequences
    pack $f.fNotebook -fill both -expand 1
 
    set w [Notebook-frame $f.fNotebook Load]
    FMRIEngineBuildUIForLoad $w
    set w [Notebook-frame $f.fNotebook Select]
    FMRIEngineBuildUIForSelect $w
 
    #-------------------------------------------
    # Setup tab 
    #-------------------------------------------
    set fSetup $Module(FMRIEngine,fSetup)
    FMRIEngineBuildUIForSetupTab $fSetup
    set b $Module(FMRIEngine,bSetup)
    bind $b <1> "FMRIEngineAskModelClearing" 

    #-------------------------------------------
    # Compute tab 
    #-------------------------------------------
    set fCompute $Module(FMRIEngine,fCompute)
    FMRIEngineBuildUIForComputeTab $fCompute
    set b $Module(FMRIEngine,bCompute)
    bind $b <1> "FMRIEngineUpdateContrastList" 

    #-------------------------------------------
    # Inspect tab 
    #-------------------------------------------
    set fInspect $Module(FMRIEngine,fInspect)
    FMRIEngineBuildUIForInspectTab $fInspect
    set b $Module(FMRIEngine,bInspect)
    bind $b <1> "FMRIEngineUpdateInspectTab" 
}


proc FMRIEngineUpdateSequenceTab {} {
    global FMRIEngine

    set FMRIEngine(currentTab) "Sequence"
}


proc FMRIEngineUpdateHelpTab {} {
    global FMRIEngine

    set FMRIEngine(currentTab) "Help"
}

     
proc FMRIEngineUpdateInspectTab {} {
    global FMRIEngine Gui Volume

    set FMRIEngine(tcPlottingOption) "" 
    set FMRIEngine(currentTab) "Inspect"

    $FMRIEngine(inspectListBox) delete 0 end
    if {[info exists FMRIEngine(actVolumeNames)]} {
        set size [llength $FMRIEngine(actVolumeNames)]

        for {set i 0} {$i < $size} {incr i} {
            set name [lindex $FMRIEngine(actVolumeNames) $i] 
            if {$name != ""} {
                $FMRIEngine(inspectListBox) insert end $name
            }
        } 
    }
}


proc FMRIEngineInspectActVolume {} {
    global FMRIEngine Gui Volume

    unset -nocomplain FMRIEngine(currentActVolID)

    set curs [$FMRIEngine(inspectListBox) curselection] 
    if {$curs != ""} {
        set name [$FMRIEngine(inspectListBox) get $curs] 

        set id [MIRIADSegmentGetVolumeByName $name] 
        set FMRIEngine(currentActVolID) $id
        set FMRIEngine(currentActVolName) $name

        MainSlicesSetVolumeAll Fore $id
        MainVolumesSetActive $id
        MainVolumesRender
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineComputeContrasts
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineComputeContrasts {} {
    global FMRIEngine Gui Volume

    set curs [$FMRIEngine(computeListBox) curselection] 
    if {$curs != ""} {
        if {! [info exists FMRIEngine(actBetaAndStd)]} {
            DevErrorWindow "Estimating the model first."
            return
        }

        unset -nocomplain FMRIEngine(actVolumeNames)
 
        set size [llength $curs]
        for {set ii 0} {$ii < $size} {incr ii} {
            set jj [lindex $curs $ii]
            set name [$FMRIEngine(computeListBox) get $jj] 

            if {$name != ""} {
                # always uses a new instance of vtkActivationVolumeGenerator 
                if {[info commands FMRIEngine(actVolumeGenerator)] != ""} {
                    FMRIEngine(actVolumeGenerator) Delete
                        unset -nocomplain FMRIEngine(actVolumeGenerator)
                }
                vtkActivationVolumeGenerator FMRIEngine(actVolumeGenerator)

                # adds progress bar
                set obs1 [FMRIEngine(actVolumeGenerator) AddObserver StartEvent MainStartProgress]
                set obs2 [FMRIEngine(actVolumeGenerator) AddObserver ProgressEvent \
                    "MainShowProgress FMRIEngine(actVolumeGenerator)"]
                set obs3 [FMRIEngine(actVolumeGenerator) AddObserver EndEvent MainEndProgress]
                set Gui(progressText) "Computing contrast $name..."
                set FMRIEngine(actVolName) $name 

                set vec $FMRIEngine($name,contrastVector) 
                set contrList [split $vec " "]
                set len [llength $contrList]
                if {[info commands FMRIEngine(contrast)] != ""} {
                    FMRIEngine(contrast) Delete
                        unset -nocomplain FMRIEngine(contrast)
                }
                vtkIntArray FMRIEngine(contrast)

                FMRIEngine(contrast) SetNumberOfTuples $len 
                FMRIEngine(contrast) SetNumberOfComponents 1
                set count 0
                foreach entry $contrList {
                    FMRIEngine(contrast) SetComponent $count 0 $entry 
                    incr count
                }

                FMRIEngine(actVolumeGenerator) SetContrastVector FMRIEngine(contrast) 
                FMRIEngine(actVolumeGenerator) SetInput $FMRIEngine(actBetaAndStd) 
                set act [FMRIEngine(actVolumeGenerator) GetOutput]
                $act Update
                set FMRIEngine(act) $act

                # add a mrml node
                set n [MainMrmlAddNode Volume]
                set i [$n GetID]
                MainVolumesCreate $i

                # set the name and description of the volume
                $n SetName "activation-$name" 
                lappend FMRIEngine(actVolumeNames) "activation-$name"
                $n SetDescription "activation-$name"

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

                # set the lower threshold to 1
                Volume($i,node) AutoThresholdOff
                Volume($i,node) ApplyThresholdOn
                Volume($i,node) SetLowerThreshold 1

                # set the act volume to the color of iron
                MainVolumesSetParam LutID 1 

                set FMRIEngine($i,actLow) [FMRIEngine(actVolumeGenerator) GetLowRange] 
                set FMRIEngine($i,actHigh) [FMRIEngine(actVolumeGenerator) GetHighRange] 
                # puts "low = $FMRIEngine($i,actLow)"
                # puts "high = $FMRIEngine($i,actHigh)"
                
            }
        } 

        set FMRIEngine(actVolName) "" 
        set Gui(progressText) "Updating..."
        puts $Gui(progressText)
        MainStartProgress
        MainShowProgress FMRIEngine(actVolumeGenerator) 
        MainUpdateMRML
        # RenderAll
        MainEndProgress

        FMRIEngine(actVolumeGenerator) RemoveObserver $obs1 
        FMRIEngine(actVolumeGenerator) RemoveObserver $obs2 
        FMRIEngine(actVolumeGenerator) RemoveObserver $obs3 

        puts "...done"

    } else {
        DevWarningWindow "There is no contrast to compute."
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateContrastList
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateContrastList {} {
    global FMRIEngine

    set FMRIEngine(currentTab) "Compute"

    $FMRIEngine(computeListBox) delete 0 end

    set size [$FMRIEngine(contrastsListBox) size]
    for {set i 0} {$i < $size} {incr i} {
        set name [$FMRIEngine(contrastsListBox) get $i] 
        if {$name != ""} {
            $FMRIEngine(computeListBox) insert end $name
        }
    } 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineAskModelClearing
# Asks the usre if s/he needs to clear the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineAskModelClearing {} {
    global FMRIEngine

    set FMRIEngine(currentTab) "Set Up"

    if {$FMRIEngine(baselineEVsAdded) == 0} {
        FMRIEngineAddBaselineEVs
    } elseif {$FMRIEngine(baselineEVsAdded) != 0 &&
      $FMRIEngine(baselineEVsAdded) != $FMRIEngine(noOfRuns)} {
        DevWarningWindow "The number of runs has changed. You probably need to save/clear the current model."
        Tab FMRIEngine row1 Model
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForInspectTab
# Creates UI for the inspect tab 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForInspectTab {parent} {
    global FMRIEngine Gui

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
    set FMRIEngine(inspectVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::FMRIEngine(inspectVerScroll) set}
    set FMRIEngine(inspectListBox) $f.lb
    $FMRIEngine(inspectVerScroll) configure -command {$FMRIEngine(inspectListBox) yview}

    DevAddButton $f.bGo "Select" "FMRIEngineInspectActVolume" 15 
 
    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $FMRIEngine(inspectListBox) -fill x -padx 1 -pady 1 \
        1,1 $FMRIEngine(inspectVerScroll) -fill y -padx 1 -pady 1 \
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
    set FMRIEngine(pValue) "None"
    set FMRIEngine(tStat) "None"
    eval {entry $f.ePV -width 10 -state readonly \
        -textvariable FMRIEngine(pValue)} $Gui(WEA)
    eval {entry $f.eTS -width 10 -state readonly \
        -textvariable FMRIEngine(tStat)} $Gui(WEA)
   grid $f.lPV $f.ePV -padx 1 -pady 2 -sticky e
   grid $f.lTS $f.eTS -padx 1 -pady 2 -sticky e

   set f $parent.fThreshold.fParams.fScale 
#   DevAddLabel $f.lactScale "Levels:"
   eval {scale $f.sactScale \
       -orient horizontal \
           -from 1 -to 20 \
           -resolution 1 \
           -bigincrement 10 \
           -length 155 \
           -command {FMRIEngineScaleActivation}} \
           $Gui(WSA) {-showvalue 0}
#   grid $f.lactScale $f.sactScale -padx 1 -pady 1 
   grid $f.sactScale -padx 1 -pady 1 
 
   # grid $f.l$param $f.e$param -padx $Gui(pad) -pady 2 -sticky e
   #  grid $f.e$param -sticky w

    #-------------------------------------------
    # Plot frame 
    #-------------------------------------------
    set f $parent.fPlot
    frame $f.fTitle  -bg $Gui(activeWorkspace)
    frame $f.fChoice -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fChoice -side top -fill x -padx 2 -pady 1 

    set f $parent.fPlot.fTitle
    DevAddButton $f.bHelp "?" "fMRIEngineHelpInspectPlotting " 2
    DevAddLabel $f.lLabel "Time series plotting:"
    grid $f.bHelp $f.lLabel -padx 1 -pady 5 

    set f $parent.fPlot.fChoice
    foreach param "Long Short ROI" \
        name "{Timecourse} {Peristimulus histogram} {ROI}" {

        eval {radiobutton $f.r$param -width 20 -text $name \
            -variable FMRIEngine(tcPlottingOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor white} $Gui(WEA)
        pack $f.r$param -side top -pady 2 
    } 

    $f.rROI configure -state disabled
    set FMRIEngine(tcPlottingOption) "" 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForComputeTab
# Creates UI for the compute tab 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForComputeTab {parent} {
    global FMRIEngine Gui

    frame $parent.fTop -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $parent.fBot -bg $Gui(activeWorkspace)
    pack $parent.fTop $parent.fBot \
        -side top -fill x -pady 5 -padx 5 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    frame $f.fBox    -bg $Gui(activeWorkspace)
    frame $f.fButtons -bg $Gui(activeWorkspace)
    pack $f.fBox $f.fButtons -side top -fill x -pady 2 -padx 1 

    set f $parent.fTop.fBox
    DevAddLabel $f.l "Select contrast(s):"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set FMRIEngine(computeVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -selectmode multiple \
        -yscrollcommand {$::FMRIEngine(computeVerScroll) set}
    set FMRIEngine(computeListBox) $f.lb
    $FMRIEngine(computeVerScroll) configure -command {$FMRIEngine(computeListBox) yview}

    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $FMRIEngine(computeListBox) -fill x -padx 1 -pady 1 \
        1,1 $FMRIEngine(computeVerScroll) -fill y -padx 1 -pady 1

    set f $parent.fTop.fButtons
    DevAddButton $f.bHelp "?" "fMRIEngineHelpComputeActivationVolume" 2 
    DevAddButton $f.bCompute "Compute" "FMRIEngineComputeContrasts" 10 
    grid $f.bHelp $f.bCompute -padx 1 -pady 3 

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBot
    DevAddLabel $f.l "Computing volume for this contrast:"
    eval {entry $f.eStatus -width 20 \
                -textvariable FMRIEngine(actVolName)} $Gui(WEA)
    pack $f.l $f.eStatus -side top -expand false -fill x -padx 5 -pady 3 
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
        set gplText [file join $env(SLICER_HOME) Modules vtkFMRIEngine data gpl.txt]
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

    pack $parent.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)
    pack $parent.fName $parent.fApply -side top -fill x -pady $Gui(pad)

    # Type frame
    set f $parent.fType

    # Build pulldown menu image format 
    eval {label $f.l -text "Choose a detector:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # GLM is default format 
    set detectorList [list GLM]
    set df [lindex $detectorList 0] 
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
    bind $f.eName <Return> "FMRIEngineComputeActivationVolume"
    TooltipAdd $f.eName "Input a name for your activation volume."
 
    # Apply frame
    set f $parent.fApply
    DevAddButton $f.bApply "Compute" "FMRIEngineComputeActivationVolume" 10 
    grid $f.bApply -padx $Gui(pad)

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

    Notebook-create $parent.fNotebook \
                    -pages {{Load from file} {Input from user}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $parent.fNotebook -fill both -expand 1

    set fLoad [Notebook-frame $parent.fNotebook {Load from file}]
    set fInput [Notebook-frame $parent.fNotebook {Input from user}]

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

    frame $parent.fTop    -bg $Gui(activeWorkspace) -relief groove -bd 3 
    frame $parent.fBottom -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $parent.fTop $parent.fBottom -side top -pady 5 -padx 0  

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    frame $f.fSeqs    -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $f.fOK      -bg $Gui(activeWorkspace)
    frame $f.fListbox -bg $Gui(activeWorkspace)
    pack $f.fSeqs $f.fOK $f.fListbox -side top -pady 3 -padx 1  

    #------------------------------
    # Loaded sequences 
    #------------------------------
    set f $parent.fTop.fSeqs

    DevAddLabel $f.lNo "How many runs:"
    eval {entry $f.eRun -width 17 \
        -textvariable FMRIEngine(noOfRuns)} $Gui(WEA)
    set FMRIEngine(noOfRuns) 1

    # Build pulldown menu for all loaded sequences 
    DevAddLabel $f.lSeq "Choose a sequence:"
    set sequenceList [list {none}]
    set df [lindex $sequenceList 0] 
    eval {menubutton $f.mbType -text $df \
        -relief raised -bd 2 -width 17 \
        -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)

    # Add menu items
    foreach m $sequenceList {
        $f.mbType.m add command -label $m \
            -command ""
    }

    # Save menubutton for config
    set FMRIEngine(gui,sequenceMenuButton) $f.mbType
    set FMRIEngine(gui,sequenceMenu) $f.mbType.m

    # Build pulldown menu for all runs 
    DevAddLabel $f.lRun "Used for run#:"
    set runList [list {1}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbType2 -text $df \
        -relief raised -bd 2 -width 17 \
        -menu $f.mbType2.m} $Gui(WMBA)
    bind $f.mbType2 <1> "FMRIEngineUpdateRuns" 
    eval {menu $f.mbType2.m} $Gui(WMA)

    set FMRIEngine(currentSelectedRun) 1

    # Save menubutton for config
    set FMRIEngine(gui,runListMenuButton) $f.mbType2
    set FMRIEngine(gui,runListMenu) $f.mbType2.m
    FMRIEngineUpdateRuns

    blt::table $f \
        0,0 $f.lNo -padx 3 -pady 3 \
        0,1 $f.eRun -padx 2 -pady 3 \
        1,0 $f.lSeq -fill x -padx 3 -pady 3 \
        1,1 $f.mbType -padx 2 -pady 3 \
        2,0 $f.lRun -fill x -padx 3 -pady 3 \
        2,1 $f.mbType2 -padx 2 -pady 3 

    #------------------------------
    # OK  
    #------------------------------
    set f $parent.fTop.fOK
    DevAddButton $f.bOK "OK" "FMRIEngineAddSeq-RunMatch" 6 
    grid $f.bOK -padx 2 

    #-----------------------
    # List box  
    #-----------------------
    set f $parent.fTop.fListbox
    frame $f.fBox -bg $Gui(activeWorkspace)
    frame $f.fAction  -bg $Gui(activeWorkspace)
    pack $f.fBox $f.fAction -side top -fill x -pady 1 -padx 2 

    set f $parent.fTop.fListbox.fBox
    DevAddLabel $f.lSeq "Specified runs:"
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set FMRIEngine(seqVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::FMRIEngine(seqVerScroll) set}
    set FMRIEngine(seqListBox) $f.lb
    $FMRIEngine(seqVerScroll) configure -command {$FMRIEngine(seqListBox) yview}

    blt::table $f \
        0,0 $f.lSeq -cspan 2 -pady 5 -fill x \
        1,0 $FMRIEngine(seqListBox) -padx 1 -pady 1 \
        1,1 $FMRIEngine(seqVerScroll) -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fTop.fListbox.fAction
    DevAddButton $f.bDelete "Delete" "FMRIEngineDeleteSeq-RunMatch" 6 
    grid $f.bDelete -padx 2 -pady 2 

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBottom
    frame $f.fLabel   -bg $Gui(activeWorkspace)
    frame $f.fButtons -bg $Gui(activeWorkspace)
    frame $f.fSlider  -bg $Gui(activeWorkspace)
    pack $f.fLabel $f.fButtons $f.fSlider -side top -fill x -pady 1 -padx 2 

    set f $parent.fBottom.fLabel
    DevAddLabel $f.lLabel "Navigate the sequence:"
    pack $f.lLabel -side top -fill x -pady 1 -padx 2 

    set f $parent.fBottom.fButtons
    DevAddButton $f.bHelp "?" "fMRIEngineHelpLoadVolumeAdjust" 2
    DevAddButton $f.bSet "Set Window/Level/Thresholds" \
        "FMRIEngineSetWindowLevelThresholds" 30 
    grid $f.bHelp $f.bSet -padx 1 

    set f $parent.fBottom.fSlider
    DevAddLabel $f.lVolno "Volume index:"
    eval { scale $f.sSlider \
        -orient horizontal \
        -from 0 -to 0 \
        -resolution 1 \
        -bigincrement 10 \
        -length 120 \
        -state active \
        -command {FMRIEngineUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}
    grid $f.lVolno $f.sSlider 

    set FMRIEngine(slider) $f.sSlider
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateRuns
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateRuns {} {
    global FMRIEngine 

    set runs [string trim $FMRIEngine(noOfRuns)]
    if {$runs < 1} {
        DevErrorWindow "No of runs must be at least 1."
    } else { 
        $FMRIEngine(gui,runListMenu) delete 0 end
        set count 1
        while {$count <= $runs} {
            $FMRIEngine(gui,runListMenu) add command -label $count \
                -command "FMRIEngineSelectRun $count"
            incr count
        }
    }
}


proc FMRIEngineSelectRun {run} {
    global FMRIEngine 

    # configure menubutton
    $FMRIEngine(gui,runListMenuButton) config -text $run
    set FMRIEngine(currentSelectedRun) $run
}


proc FMRIEngineDeleteSeq-RunMatch {} {
    global FMRIEngine 

    set curs [$FMRIEngine(seqListBox) curselection]
    if {$curs != ""} {
        set first [lindex $curs 0] 
        set last [lindex $curs end]
        $FMRIEngine(seqListBox) delete $first $last
    }
}


proc FMRIEngineAddSeq-RunMatch {} {
    global FMRIEngine 

    # Add a sequence-run match
    if {! [info exists FMRIEngine(currentSelectedSequence)] ||
        $FMRIEngine(currentSelectedSequence) == "none"} {
        DevErrorWindow "Select a valid sequence."
        return
    }

    if {! [info exists FMRIEngine(currentSelectedRun)] ||
        $FMRIEngine(currentSelectedRun) == "none"} {
        DevErrorWindow "Select a valid run."
        return
    }

    set str \
        "r$FMRIEngine(currentSelectedRun):$FMRIEngine(currentSelectedSequence)"
    set i 0
    set found 0
    set size [$FMRIEngine(seqListBox) size]
    while {$i < $size} {  
        set v [$FMRIEngine(seqListBox) get $i] 
        if {$v != ""} {
            set i1 1 
            set i2 [string first ":" $v]
            set r [string range $v $i1 [expr $i2-1]] 
            set r [string trim $r]

            if {$r == $FMRIEngine(currentSelectedRun)} {
                set found 1
                break
            }
        }

        incr i
    }

    if {$found} {
        DevErrorWindow "The r$r has been specified."
    } else {
        $FMRIEngine(seqListBox) insert end $str 
        set FMRIEngine($FMRIEngine(currentSelectedRun),sequenceName) \
            $FMRIEngine(currentSelectedSequence)
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSelectSequence
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSelectSequence {seq} {
    global FMRIEngine Ibrowser MultiVolumeReader

    # configure menubutton
    $FMRIEngine(gui,sequenceMenuButton) config -text $seq
    set FMRIEngine(currentSelectedSequence) $seq
    if {$seq == "none"} {
        return
    }

    set l [string trim $seq]

    if {[info exists MultiVolumeReader(sequenceNames)]} {
        set found [lsearch -exact $MultiVolumeReader(sequenceNames) $seq]
        if {$found >= 0} {
            set FMRIEngine(firstMRMLid) $MultiVolumeReader($seq,firstMRMLid) 
            set FMRIEngine(lastMRMLid) $MultiVolumeReader($seq,lastMRMLid)
            set FMRIEngine(volumeExtent) $MultiVolumeReader($seq,volumeExtent) 
            set FMRIEngine(noOfVolumes) $MultiVolumeReader($seq,noOfVolumes) 
        }
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

    # clears the menu 
    $FMRIEngine(gui,sequenceMenu) delete 0 end 

    # checks sequence loaded from fMRIEngine
    set b [info exists MultiVolumeReader(sequenceNames)] 
    set n [expr {$b == 0 ? 0 : [llength $MultiVolumeReader(sequenceNames)]}]

    $FMRIEngine(gui,sequenceMenu) add command -label "none" \
        -command "FMRIEngineSelectSequence none"

    if {$n > 0} {
        set i 0 
        while {$i < $n} {
            set name [lindex $MultiVolumeReader(sequenceNames) $i]
            $FMRIEngine(gui,sequenceMenu) add command -label $name \
                -command "FMRIEngineSelectSequence $name"
            incr i
        }
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
    frame $parent.fBot -bg $Gui(activeWorkspace)
    pack $parent.fTop $parent.fBot -side top 
 
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

    MultiVolumeReaderBuildGUI $f 1

    set f $parent.fBot
    set uselogo [image create photo -file \
        $FMRIEngine(modulePath)/tcl/images/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
        -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 0 -pady 0 -expand 0
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineScaleActivation
# Scales the activation volume 
# .ARGS
# no the scale index 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineScaleActivation {no} {
    global Volume FMRIEngine MultiVolumeReader

    if {! [info exists FMRIEngine(allPValues)]} {
        
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

    if {[info exists FMRIEngine(currentActVolID)]} {
        vtkCDF cdf
        set dof [expr $MultiVolumeReader(noOfVolumes) - 1]
        # The index of list starts with 0
        set p [lindex $FMRIEngine(allPValues) [expr $no - 1]]
        set t [cdf p2t $p $dof]
       
        cdf Delete
        set FMRIEngine(pValue) $p
        set FMRIEngine(tStat) $t


        set id $FMRIEngine(currentActVolID) 
        if {$id > 0} {
            # map the t value into the range between 1 and 100
            set value [expr 100 * ($t - $FMRIEngine($id,actLow)) / \
                ($FMRIEngine($id,actHigh) - $FMRIEngine($id,actLow))]
     
            Volume($id,node) AutoThresholdOff
            Volume($id,node) ApplyThresholdOn
            Volume($id,node) SetLowerThreshold [expr round($value)] 
            MainVolumesSetParam ApplyThreshold 
            MainVolumesRender
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

    FMRIEngineUpdateSequences

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
    global FMRIEngine Module Volume Gui

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
    set obs1 [FMRIEngine(actvolgen) AddObserver StartEvent MainStartProgress]
    set obs2 [FMRIEngine(actvolgen) AddObserver ProgressEvent \
              "MainShowProgress FMRIEngine(actvolgen)"]
    set obs3 [FMRIEngine(actvolgen) AddObserver EndEvent MainEndProgress]
    set Gui(progressText) "Computing Volume $FMRIEngine(actVolName)..."

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

    puts $Gui(progressText)

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

    set Gui(progressText) "Updating..."
    MainStartProgress
    MainShowProgress FMRIEngine(actvolgen) 
    MainUpdateMRML
    RenderAll
    MainEndProgress
 
    FMRIEngine(actvolgen) RemoveObserver $obs1 
    FMRIEngine(actvolgen) RemoveObserver $obs2 
    FMRIEngine(actvolgen) RemoveObserver $obs3 

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

    EvDeclareEventHandler FMRIEngineSlicesEvents <1> \
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
