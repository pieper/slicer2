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
#   fMRIEngineInit
#   fMRIEngineBuildGUI
#   fMRIEngineViewGNULicense
#   fMRIEngineCloseGPLWindow
#   fMRIEngineBuildUIForDetectors the
#   fMRIEngineSetDetector the
#   fMRIEngineBuildUIForParadigm the
#   fMRIEngineBuildUIForSelect the
#   fMRIEngineSelectSequence
#   fMRIEngineUpdateSequences
#   fMRIEngineBuildUIForLoad the
#   fMRIEngineScaleActivation the
#   fMRIEngineSetImageFormat the
#   fMRIEngineEnter
#   fMRIEngineExit
#   fMRIEngineUpdateVolume the
#   fMRIEngineComputeActivationVolume 
#   fMRIEngineSetWindowLevelThresholds
#   fMRIEnginePushBindings 
#   fMRIEnginePopBindings 
#   fMRIEngineCreateBindings  
#   fMRIEngineSetVolumeName   
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
# To find it when you run the Slicer, click on More->fMRIEngine.
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# .PROC fMRIEngineInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineInit {} {
    global fMRIEngine Module Volume Model env

    set m fMRIEngine
    
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
    #   set Module($m,procVTK) fMRIEngineBuildVTK
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
    set Module($m,procGUI) fMRIEngineBuildGUI
    set Module($m,procEnter) fMRIEngineEnter
    set Module($m,procExit) fMRIEngineExit

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
        {$Revision: 1.4 $} {$Date: 2005/03/28 21:06:11 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set fMRIEngine(dir)  ""
    set fMRIEngine(modulePath) "$env(SLICER_HOME)/Modules/vtkFMRIEngine"

    set fMRIEngine(baselineEVsAdded) 0
 

    # For now, spew heavily.
    # this bypasses the command line setting of --verbose or -v
    # set Module(verbose) 0
    
    # Creates bindings
    fMRIEngineCreateBindings 

    # Source all appropriate tcl files here. 
    source "$fMRIEngine(modulePath)/tcl/notebook.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEnginePlot.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineModel.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineHelpText.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineContrasts.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineModelView.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineSignalModeling.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineUserInputForModelView.tcl"
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
# .PROC fMRIEngineBuildGUI
# Creates the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildGUI {} {
    global Gui fMRIEngine Module Volume Model

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
    # ie: $Module(fMRIEngine,fFMRI)
    
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
    set b $Module(fMRIEngine,bHelp)
    bind $b <1> "fMRIEngineUpdateHelpTab" 

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
    MainHelpApplyTags fMRIEngine $help
    MainHelpBuildGUI fMRIEngine

    set helpWidget $fMRIEngine(helpWidget) 
    $helpWidget configure -height 22

    set fHelp $Module(fMRIEngine,fHelp)
    set f $fHelp
    frame $f.fGPL -bg $Gui(activeWorkspace) 
    pack $f.fGPL -side top 

    DevAddButton $f.fGPL.bView "View GNU License" \
        "fMRIEngineViewGNULicense" 22
    pack $f.fGPL.bView -side left -pady 5 

    #-------------------------------------------
    # Sequence tab 
    #-------------------------------------------
    set b $Module(fMRIEngine,bSequence)
    bind $b <1> "fMRIEngineUpdateSequenceTab" 

    set fSequence $Module(fMRIEngine,fSequence)
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
        fMRIEngineUpdateSequences
    pack $f.fNotebook -fill both -expand 1
 
    set w [Notebook-frame $f.fNotebook Load]
    fMRIEngineBuildUIForLoad $w
    set w [Notebook-frame $f.fNotebook Select]
    fMRIEngineBuildUIForSelect $w
 
    #-------------------------------------------
    # Setup tab 
    #-------------------------------------------
    set fSetup $Module(fMRIEngine,fSetup)
    fMRIEngineBuildUIForSetupTab $fSetup
    set b $Module(fMRIEngine,bSetup)
    bind $b <1> "fMRIEngineAskModelClearing" 

    #-------------------------------------------
    # Compute tab 
    #-------------------------------------------
    set fCompute $Module(fMRIEngine,fCompute)
    fMRIEngineBuildUIForComputeTab $fCompute
    set b $Module(fMRIEngine,bCompute)
    bind $b <1> "fMRIEngineUpdateContrastList" 

    #-------------------------------------------
    # Inspect tab 
    #-------------------------------------------
    set fInspect $Module(fMRIEngine,fInspect)
    fMRIEngineBuildUIForInspectTab $fInspect
    set b $Module(fMRIEngine,bInspect)
    bind $b <1> "fMRIEngineUpdateInspectTab" 
}


proc fMRIEngineUpdateSequenceTab {} {
    global fMRIEngine

    set fMRIEngine(currentTab) "Sequence"
}


proc fMRIEngineUpdateHelpTab {} {
    global fMRIEngine

    set fMRIEngine(currentTab) "Help"
}

     
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
# .PROC fMRIEngineComputeContrasts
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineComputeContrasts {} {
    global fMRIEngine Gui MultiVolumeReader Volume

    set curs [$fMRIEngine(computeListBox) curselection] 
    if {$curs != ""} {
        if {! [info exists fMRIEngine(actBetaAndStd)]} {
            DevErrorWindow "Estimating the model first."
            return
        }

        unset -nocomplain fMRIEngine(actVolumeNames)

        # total volumes from all runs
        set totalVols 0
        for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} { 
            set seqName $fMRIEngine($r,sequenceName)
            set totalVols [expr $MultiVolumeReader($seqName,noOfVolumes) + $totalVols]
        }

        set size [llength $curs]
        for {set ii 0} {$ii < $size} {incr ii} {
            set jj [lindex $curs $ii]
            set name [$fMRIEngine(computeListBox) get $jj] 

            if {$name != ""} {
                # always uses a new instance of vtkActivationVolumeGenerator 
                if {[info commands fMRIEngine(actVolumeGenerator)] != ""} {
                    fMRIEngine(actVolumeGenerator) Delete
                        unset -nocomplain fMRIEngine(actVolumeGenerator)
                }
                vtkActivationVolumeGenerator fMRIEngine(actVolumeGenerator)

                # adds progress bar
                set obs1 [fMRIEngine(actVolumeGenerator) AddObserver StartEvent MainStartProgress]
                set obs2 [fMRIEngine(actVolumeGenerator) AddObserver ProgressEvent \
                    "MainShowProgress fMRIEngine(actVolumeGenerator)"]
                set obs3 [fMRIEngine(actVolumeGenerator) AddObserver EndEvent MainEndProgress]
                set Gui(progressText) "Computing contrast $name..."
                set fMRIEngine(actVolName) $name 

                set vec $fMRIEngine($name,contrastVector) 
                set contrList [split $vec " "]
                set len [llength $contrList]
                if {[info commands fMRIEngine(contrast)] != ""} {
                    fMRIEngine(contrast) Delete
                        unset -nocomplain fMRIEngine(contrast)
                }
                vtkIntArray fMRIEngine(contrast)

                fMRIEngine(contrast) SetNumberOfTuples $len 
                fMRIEngine(contrast) SetNumberOfComponents 1
                set count 0
                foreach entry $contrList {
                    fMRIEngine(contrast) SetComponent $count 0 $entry 
                    incr count
                }

                fMRIEngine(actVolumeGenerator) SetNumberOfVolumes $totalVols 
                fMRIEngine(actVolumeGenerator) SetContrastVector fMRIEngine(contrast) 
                fMRIEngine(actVolumeGenerator) SetInput $fMRIEngine(actBetaAndStd) 
                set act [fMRIEngine(actVolumeGenerator) GetOutput]
                $act Update
                set fMRIEngine(act) $act

                # add a mrml node
                set n [MainMrmlAddNode Volume]
                set i [$n GetID]
                MainVolumesCreate $i

                # set the name and description of the volume
                $n SetName "activation-$name" 
                lappend fMRIEngine(actVolumeNames) "activation-$name"
                $n SetDescription "activation-$name"

                eval Volume($i,node) SetSpacing [$act GetSpacing]
                Volume($i,node) SetScanOrder [Volume($fMRIEngine(firstMRMLid),node) GetScanOrder]
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

                set fMRIEngine($i,actLow) [fMRIEngine(actVolumeGenerator) GetLowRange] 
                set fMRIEngine($i,actHigh) [fMRIEngine(actVolumeGenerator) GetHighRange] 
                # puts "low = $fMRIEngine($i,actLow)"
                # puts "high = $fMRIEngine($i,actHigh)"
                
            }
        } 

        set fMRIEngine(actVolName) "" 
        set Gui(progressText) "Updating..."
        puts $Gui(progressText)
        MainStartProgress
        MainShowProgress fMRIEngine(actVolumeGenerator) 
        MainUpdateMRML
        # RenderAll
        MainEndProgress

        fMRIEngine(actVolumeGenerator) RemoveObserver $obs1 
        fMRIEngine(actVolumeGenerator) RemoveObserver $obs2 
        fMRIEngine(actVolumeGenerator) RemoveObserver $obs3 

        puts "...done"

    } else {
        DevWarningWindow "There is no contrast to compute."
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateContrastList
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateContrastList {} {
    global fMRIEngine

    set fMRIEngine(currentTab) "Compute"

    $fMRIEngine(computeListBox) delete 0 end

    set size [$fMRIEngine(contrastsListBox) size]
    for {set i 0} {$i < $size} {incr i} {
        set name [$fMRIEngine(contrastsListBox) get $i] 
        if {$name != ""} {
            $fMRIEngine(computeListBox) insert end $name
        }
    } 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineAskModelClearing
# Asks the usre if s/he needs to clear the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAskModelClearing {} {
    global fMRIEngine

    set fMRIEngine(currentTab) "Set Up"

    if {$fMRIEngine(baselineEVsAdded) == 0} {
        fMRIEngineAddBaselineEVs
    } elseif {$fMRIEngine(baselineEVsAdded) != 0 &&
      $fMRIEngine(baselineEVsAdded) != $fMRIEngine(noOfRuns)} {
        DevWarningWindow "The number of runs has changed. You probably need to save/clear the current model."
        Tab fMRIEngine row1 Model
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForInspectTab
# Creates UI for the inspect tab 
# .ARGS
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
    set fMRIEngine(pValue) "None"
    set fMRIEngine(tStat) "None"
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
           -command {fMRIEngineScaleActivation}} \
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
            -variable fMRIEngine(tcPlottingOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor white} $Gui(WEA)
        pack $f.r$param -side top -pady 2 
    } 

    $f.rROI configure -state disabled
    set fMRIEngine(tcPlottingOption) "" 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForComputeTab
# Creates UI for the compute tab 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForComputeTab {parent} {
    global fMRIEngine Gui

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
    set fMRIEngine(computeVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -selectmode multiple \
        -yscrollcommand {$::fMRIEngine(computeVerScroll) set}
    set fMRIEngine(computeListBox) $f.lb
    $fMRIEngine(computeVerScroll) configure -command {$fMRIEngine(computeListBox) yview}

    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $fMRIEngine(computeListBox) -fill x -padx 1 -pady 1 \
        1,1 $fMRIEngine(computeVerScroll) -fill y -padx 1 -pady 1

    set f $parent.fTop.fButtons
    DevAddButton $f.bHelp "?" "fMRIEngineHelpComputeActivationVolume" 2 
    DevAddButton $f.bCompute "Compute" "fMRIEngineComputeContrasts" 10 
    grid $f.bHelp $f.bCompute -padx 1 -pady 3 

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBot
    DevAddLabel $f.l "Computing volume for this contrast:"
    eval {entry $f.eStatus -width 20 \
                -textvariable fMRIEngine(actVolName)} $Gui(WEA)
    pack $f.l $f.eStatus -side top -expand false -fill x -padx 5 -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineViewGNULicense
# Displays GNU license information 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineViewGNULicense {} {
    global fMRIEngine Gui env

    if {[info exists fMRIEngine(GPLToplevel)] == 0 } {
        set w .tcren
        toplevel $w
        wm title $w "Gnu General Public License" 
        wm minsize $w 570 500 
        wm maxsize $w 570 500 
        wm geometry $w "+285+100" 
        wm protocol $w WM_DELETE_WINDOW "fMRIEngineCloseGPLWindow" 
        set fMRIEngine(GPLToplevel) $w

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
        set gplText [file join $env(SLICER_HOME) Modules vtkfMRIEngine data gpl.txt]
        set fp [open $gplText r]
        set data [read $fp]
        regsub -all "\f" $data {} data 
        $f.t insert 1.0 $data 

        set f $w.fButton
        button $f.bDone -text "Done" -command "fMRIEngineCloseGPLWindow" -width 8 
        pack $f.bDone -side top -pady $Gui(pad) 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCloseGPLWindow
# Closes GNU license information window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCloseGPLWindow {} {
    global fMRIEngine 

    destroy $fMRIEngine(GPLToplevel)
    unset -nocomplain fMRIEngine(GPLToplevel)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForDetectors
# Creates UI for Detectors page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForDetectors {parent} {
    global fMRIEngine Gui

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
            -command "fMRIEngineSetDetector $m"
    }

    # save menubutton for config
    set fMRIEngine(gui,mbActDetector) $f.mbType

    # Name frame
    set f $parent.fName
    DevAddLabel $f.label "Volume Name:"
    eval {entry $f.eName -width 20 \
                -textvariable fMRIEngine(actVolName)} $Gui(WEA)
    pack $f.label $f.eName -side left -expand false -fill x -padx 6
    bind $f.eName <Return> "fMRIEngineComputeActivationVolume"
    TooltipAdd $f.eName "Input a name for your activation volume."
 
    # Apply frame
    set f $parent.fApply
    DevAddButton $f.bApply "Compute" "fMRIEngineComputeActivationVolume" 10 
    grid $f.bApply -padx $Gui(pad)

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetDetector
# Switches activation detector 
# .ARGS
# actDetector the activation detector 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetDetector {detector} {
    global fMRIEngine
    
    set fMRIEngine(actDetector) $detector

    # configure menubutton
    $fMRIEngine(gui,mbActDetector) config -text $detector 

#    set f  $fMRIEngine(f${imgFormat})
#    raise $f
#    focus $f
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForParadigm
# Creates UI for Paradigm page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForParadigm {parent} {
    global fMRIEngine Gui

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
    DevAddFileBrowse $fLoad.fFile fMRIEngine "paradigmFileName" \
        "Paradigm design file:" "" "txt" "\$Volume(DefaultDir)" \
        "Open" "Browse for a text file" "" "Absolute" 

    DevAddButton $fLoad.bLoad "Load" "fMRIEngineParseParadigm" 10 
    pack $fLoad.bLoad -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Input tab 
    #-------------------------------------------
    frame $fInput.fGrid  -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $fInput.fGrid -side top -fill x -pady $Gui(pad)
    set f $fInput.fGrid
    
    # Entry fields (the loop makes a frame for each variable)
    set fMRIEngine(paramNames) [list {Total Volumes} {Number Of Conditions} \
        {Volumes Per Baseline} {Volumes Per Condition} {Volumes At Start} \
        {Starts With}]
    set fMRIEngine(paramVariables) [list tVols nStims bVols cVols sVols start]
    set i 0      
    set len [llength $fMRIEngine(paramNames)]
    while {$i < $len} { 

        set name [lindex $fMRIEngine(paramNames) $i]
        set param [lindex $fMRIEngine(paramVariables) $i]
 
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 8 -textvariable fMRIEngine($param)} $Gui(WEA)

        grid $f.l$param $f.e$param -padx $Gui(pad) -pady $Gui(pad) -sticky e
        grid $f.e$param -sticky w
        incr i
    }

    DevAddButton $f.bStore "Done" "fMRIEngineStoreParadigm" 10 
    TooltipAdd $f.bStore "Hit this button if you finish your input."
 
    DevAddButton $f.bSave "Save" "fMRIEngineSaveParadigm" 8 
    TooltipAdd $f.bSave "Save all values into a paradigm design file."
 
    grid $f.bStore $f.bSave -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.bSave -sticky w
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForSelect
# Creates UI for Select page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForSelect {parent} {
    global fMRIEngine Gui

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
        -textvariable fMRIEngine(noOfRuns)} $Gui(WEA)
    set fMRIEngine(noOfRuns) 1

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
    set fMRIEngine(gui,sequenceMenuButton) $f.mbType
    set fMRIEngine(gui,sequenceMenu) $f.mbType.m

    # Build pulldown menu for all runs 
    DevAddLabel $f.lRun "Used for run#:"
    set runList [list {1}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbType2 -text $df \
        -relief raised -bd 2 -width 17 \
        -menu $f.mbType2.m} $Gui(WMBA)
    bind $f.mbType2 <1> "fMRIEngineUpdateRuns" 
    eval {menu $f.mbType2.m} $Gui(WMA)

    set fMRIEngine(currentSelectedRun) 1

    # Save menubutton for config
    set fMRIEngine(gui,runListMenuButton) $f.mbType2
    set fMRIEngine(gui,runListMenu) $f.mbType2.m
    fMRIEngineUpdateRuns

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
    DevAddButton $f.bOK "OK" "fMRIEngineAddSeq-RunMatch" 6 
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
    set fMRIEngine(seqVerScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(seqVerScroll) set}
    set fMRIEngine(seqListBox) $f.lb
    $fMRIEngine(seqVerScroll) configure -command {$fMRIEngine(seqListBox) yview}

    blt::table $f \
        0,0 $f.lSeq -cspan 2 -pady 5 -fill x \
        1,0 $fMRIEngine(seqListBox) -padx 1 -pady 1 \
        1,1 $fMRIEngine(seqVerScroll) -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fTop.fListbox.fAction
    DevAddButton $f.bDelete "Delete" "fMRIEngineDeleteSeq-RunMatch" 6 
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
        "fMRIEngineSetWindowLevelThresholds" 30 
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
        -command {fMRIEngineUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}
    grid $f.lVolno $f.sSlider 

    set fMRIEngine(slider) $f.sSlider
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateRuns
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateRuns {} {
    global fMRIEngine 

    set runs [string trim $fMRIEngine(noOfRuns)]
    if {$runs < 1} {
        DevErrorWindow "No of runs must be at least 1."
    } else { 
        $fMRIEngine(gui,runListMenu) delete 0 end
        set count 1
        while {$count <= $runs} {
            $fMRIEngine(gui,runListMenu) add command -label $count \
                -command "fMRIEngineSelectRun $count"
            incr count
        }
    }
}


proc fMRIEngineSelectRun {run} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,runListMenuButton) config -text $run
    set fMRIEngine(currentSelectedRun) $run
}


proc fMRIEngineDeleteSeq-RunMatch {} {
    global fMRIEngine 

    set curs [$fMRIEngine(seqListBox) curselection]
    if {$curs != ""} {
        set first [lindex $curs 0] 
        set last [lindex $curs end]
        $fMRIEngine(seqListBox) delete $first $last
    }
}


proc fMRIEngineAddSeq-RunMatch {} {
    global fMRIEngine 

    # Add a sequence-run match
    if {! [info exists fMRIEngine(currentSelectedSequence)] ||
        $fMRIEngine(currentSelectedSequence) == "none"} {
        DevErrorWindow "Select a valid sequence."
        return
    }

    if {! [info exists fMRIEngine(currentSelectedRun)] ||
        $fMRIEngine(currentSelectedRun) == "none"} {
        DevErrorWindow "Select a valid run."
        return
    }

    set str \
        "r$fMRIEngine(currentSelectedRun):$fMRIEngine(currentSelectedSequence)"
    set i 0
    set found 0
    set size [$fMRIEngine(seqListBox) size]
    while {$i < $size} {  
        set v [$fMRIEngine(seqListBox) get $i] 
        if {$v != ""} {
            set i1 1 
            set i2 [string first ":" $v]
            set r [string range $v $i1 [expr $i2-1]] 
            set r [string trim $r]

            if {$r == $fMRIEngine(currentSelectedRun)} {
                set found 1
                break
            }
        }

        incr i
    }

    if {$found} {
        DevErrorWindow "The r$r has been specified."
    } else {
        $fMRIEngine(seqListBox) insert end $str 
        set fMRIEngine($fMRIEngine(currentSelectedRun),sequenceName) \
            $fMRIEngine(currentSelectedSequence)
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectSequence
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectSequence {seq} {
    global fMRIEngine Ibrowser MultiVolumeReader

    # configure menubutton
    $fMRIEngine(gui,sequenceMenuButton) config -text $seq
    set fMRIEngine(currentSelectedSequence) $seq
    if {$seq == "none"} {
        return
    }

    set l [string trim $seq]

    if {[info exists MultiVolumeReader(sequenceNames)]} {
        set found [lsearch -exact $MultiVolumeReader(sequenceNames) $seq]
        if {$found >= 0} {
            set fMRIEngine(firstMRMLid) $MultiVolumeReader($seq,firstMRMLid) 
            set fMRIEngine(lastMRMLid) $MultiVolumeReader($seq,lastMRMLid)
            set fMRIEngine(volumeExtent) $MultiVolumeReader($seq,volumeExtent) 
            set fMRIEngine(noOfVolumes) $MultiVolumeReader($seq,noOfVolumes) 
        }
    }

    # Sets range for the volume slider
    $fMRIEngine(slider) configure -from 1 -to $fMRIEngine(noOfVolumes)
    # Sets the first volume in the sequence as the active volume
    MainVolumesSetActive $fMRIEngine(firstMRMLid)
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateSequences
# Updates sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateSequences {} {
    global fMRIEngine Ibrowser MultiVolumeReader 

    # clears the menu 
    $fMRIEngine(gui,sequenceMenu) delete 0 end 

    # checks sequence loaded from fMRIEngine
    set b [info exists MultiVolumeReader(sequenceNames)] 
    set n [expr {$b == 0 ? 0 : [llength $MultiVolumeReader(sequenceNames)]}]

    $fMRIEngine(gui,sequenceMenu) add command -label "none" \
        -command "fMRIEngineSelectSequence none"

    if {$n > 0} {
        set i 0 
        while {$i < $n} {
            set name [lindex $MultiVolumeReader(sequenceNames) $i]
            $fMRIEngine(gui,sequenceMenu) add command -label $name \
                -command "fMRIEngineSelectSequence $name"
            incr i
        }
    }
}

  
#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForLoad
# Creates UI for Load page 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForLoad {parent} {
    global fMRIEngine Gui

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
        $fMRIEngine(modulePath)/tcl/images/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
        -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 0 -pady 0 -expand 0
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineScaleActivation
# Scales the activation volume 
# .ARGS
# no the scale index 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineScaleActivation {no} {
    global Volume fMRIEngine MultiVolumeReader

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
            # map the t value into the range between 1 and 100
            set value [expr 100 * ($t - $fMRIEngine($id,actLow)) / \
                ($fMRIEngine($id,actHigh) - $fMRIEngine($id,actLow))]
     
            Volume($id,node) AutoThresholdOff
            Volume($id,node) ApplyThresholdOn
            Volume($id,node) SetLowerThreshold [expr round($value)] 
            MainVolumesSetParam ApplyThreshold 
            MainVolumesRender
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetImageFormat
# Switches image format 
# .ARGS
# imgFormat the image format
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetImageFormat {imgFormat} {
    global Volume fMRIEngine
    
    set fMRIEngine(imageFormat) $imgFormat

    # configure menubutton
    $fMRIEngine(gui,mbImgFormat) config -text $imgFormat 

    set f  $fMRIEngine(f${imgFormat})
    raise $f
    focus $f
#    focus $Volume(f$Volume(propertyType))
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineEnter
# Called when this module is entered by the user. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineEnter {} {
    global fMRIEngine

    fMRIEngineUpdateSequences

    #--- push all event bindings onto the stack.
    fMRIEnginePushBindings

    #--- For now, hide it; not fully integrated yet.
    #if {[winfo exists $toplevelName]} {
    #    lower $toplevelName
    #    wm iconify $toplevelName
    #}
}
 

#-------------------------------------------------------------------------------
# .PROC fMRIEngineExit
# Called when this module is exited by the user
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineExit {} {

    # pop event bindings
    fMRIEnginePopBindings
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateVolume
# Updates image volume as user moves the slider 
# .ARGS
# volumeNo the volume number
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateVolume {volumeNo} {
    global fMRIEngine

    if {$volumeNo == 0} {
#        DevErrorWindow "Volume number must be greater than 0."
        return
    }

    set v [expr $volumeNo-1]
    set id [expr $fMRIEngine(firstMRMLid)+$v]

    MainSlicesSetVolumeAll Back $id 
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineComputeActivationVolume 
# Computes activation volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineComputeActivationVolume {} {
    global fMRIEngine Module Volume Gui

    # Checks if volumes have been loaded
    if {[info exists fMRIEngine(firstMRMLid)] == 0} {
        DevErrorWindow "Please load volumes first."
        return
    }

    # Checks if a name is entered for the act volume 
    string trim $fMRIEngine(actVolName)
    if {$fMRIEngine(actVolName) == ""} {
        DevErrorWindow "Please input a name for the activation volume."
        return
    }

    # Add volumes into vtkActivationVolumeGenerator
    if {[info commands fMRIEngine(actvolgen)] != ""} {
        fMRIEngine(actvolgen) Delete
        unset -nocomplain fMRIEngine(actvolgen)
    }
    vtkActivationVolumeGenerator fMRIEngine(actvolgen)
    set obs1 [fMRIEngine(actvolgen) AddObserver StartEvent MainStartProgress]
    set obs2 [fMRIEngine(actvolgen) AddObserver ProgressEvent \
              "MainShowProgress fMRIEngine(actvolgen)"]
    set obs3 [fMRIEngine(actvolgen) AddObserver EndEvent MainEndProgress]
    set Gui(progressText) "Computing Volume $fMRIEngine(actVolName)..."

    set id $fMRIEngine(firstMRMLid) 
    while {$id <= $fMRIEngine(lastMRMLid)} {
        Volume($id,vol) Update
        fMRIEngine(actvolgen) AddInput [Volume($id,vol) GetOutput]
        incr id
    }

    # Checks if paradigm file has been properly parsed 
    if {! [info exists fMRIEngine(paradigm)]} {
        DevErrorWindow "Paradigm design doesn't exist."
        return
    }

    set stimSize [fMRIEngine(stimulus) GetNumberOfTuples]
    set volSize [fMRIEngine(actvolgen) GetNumberOfInputs]
    if {$stimSize != $volSize} {
        DevErrorWindow "Stimulus size ($stimSize) is not same as no of volumes ($volSize)."
        return
    }

    puts $Gui(progressText)

    if {[info commands fMRIEngine(detector)] != ""} {
        fMRIEngine(detector) Delete
        unset -nocomplain fMRIEngine(detector)
    }
    vtkActivationDetector fMRIEngine(detector)
    fMRIEngine(detector) SetDetectionMethod 1
    fMRIEngine(detector) SetNumberOfPredictors [lindex $fMRIEngine(paradigm) 1] 
    fMRIEngine(detector) SetStimulus fMRIEngine(stimulus) 

    if {[info exists fMRIEngine(lowerThreshold)]} {
        fMRIEngine(actvolgen) SetLowerThreshold $fMRIEngine(lowerThreshold)
    }
    fMRIEngine(actvolgen) SetDetector fMRIEngine(detector)  
    fMRIEngine(actvolgen) Update
    set fMRIEngine(actLow) [fMRIEngine(actvolgen) GetLowRange] 
    set fMRIEngine(actHigh) [fMRIEngine(actvolgen) GetHighRange] 

    set act [fMRIEngine(actvolgen) GetOutput]
    $act Update

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    $n SetName $fMRIEngine(actVolName) 
    $n SetDescription $fMRIEngine(actVolName) 

    eval Volume($i,node) SetSpacing [$act GetSpacing]
    Volume($i,node) SetScanOrder [Volume($fMRIEngine(firstMRMLid),node) GetScanOrder]
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
    MainShowProgress fMRIEngine(actvolgen) 
    MainUpdateMRML
    RenderAll
    MainEndProgress
 
    fMRIEngine(actvolgen) RemoveObserver $obs1 
    fMRIEngine(actvolgen) RemoveObserver $obs2 
    fMRIEngine(actvolgen) RemoveObserver $obs3 

    puts "...done"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetWindowLevelThresholds
# For a time series, set window, level, and low/high thresholds for all volumes
# with the first volume's values
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetWindowLevelThresholds {} {
   global fMRIEngine Volume 

    if {[info exists fMRIEngine(noOfVolumes)] == 0} {
        return
    }

    set low [Volume($fMRIEngine(firstMRMLid),node) GetLowerThreshold]
    set win [Volume($fMRIEngine(firstMRMLid),node) GetWindow]
    set level [Volume($fMRIEngine(firstMRMLid),node) GetLevel]
    set fMRIEngine(lowerThreshold) $low

    set i $fMRIEngine(firstMRMLid)
    while {$i <= $fMRIEngine(lastMRMLid)} {
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
# .PROC fMRIEnginePushBindings 
# Pushes onto the event stack a new event manager that
# deals with events when the fMRIEngine module is active
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEnginePushBindings {} {
   global Ev Csys

    EvActivateBindingSet FMRISlice0Events
    EvActivateBindingSet FMRISlice1Events
    EvActivateBindingSet FMRISlice2Events
}


#-------------------------------------------------------------------------------
# .PROC fMRIEnginePopBindings 
# Removes bindings when fMRIEnginer module is inactive
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEnginePopBindings {} {
    global Ev Csys

    EvDeactivateBindingSet FMRISlice0Events
    EvDeactivateBindingSet FMRISlice1Events
    EvDeactivateBindingSet FMRISlice2Events
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCreateBindings  
# Creates fMRIEngine event bindings for the three slice windows 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCreateBindings {} {
    global Gui Ev

    EvDeclareEventHandler fMRIEngineSlicesEvents <1> \
        { fMRIEnginePopUpPlot %x %y }
           
    EvAddWidgetToBindingSet FMRISlice0Events $Gui(fSl0Win) {fMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice1Events $Gui(fSl1Win) {fMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice2Events $Gui(fSl2Win) {fMRIEngineSlicesEvents}    
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetVolumeName   
# Saves current volume name 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetVolumeName {} {
    global Volume fMRIEngine 

    set fMRIEngine(name) $Volume(name)
}
