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
# FILE:        fMRIEngine.tcl
# PROCEDURES:  
#   fMRIEngineInit
#   fMRIEngineBuildGUI
#   fMRIEngineUpdateHelpTab
#   fMRIEngineViewGNULicense
#   fMRIEngineCloseGPLWindow
#   fMRIEngineEnter
#   fMRIEngineExit
#   fMRIEnginePushBindings 
#   fMRIEnginePopBindings 
#   fMRIEngineCreateBindings  
#==========================================================================auto=
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
    set Module($m,row1List) "Help Sequence Setup ROI Compute View"
    set Module($m,row1Name) "{Help} {Sequence} {Set Up} {ROI} {Compute} {View}"
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
        {$Revision: 1.18 $} {$Date: 2005/11/01 20:45:22 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set fMRIEngine(dir)  ""
    set fMRIEngine(currentTab) "Sequence"
    set fMRIEngine(modulePath) "$env(SLICER_HOME)/Modules/vtkFMRIEngine"

    set fMRIEngine(baselineEVsAdded) 0

    # For now, spew heavily.
    # this bypasses the command line setting of --verbose or -v
    # set Module(verbose) 0
    
    # Creates bindings
    fMRIEngineCreateBindings 

    # error if no private segment
    if { [catch "package require BLT"] } {
        DevErrorWindow "Must have the BLT package for building fMRIEngine UI \
        and plotting time course."
        return
    }

    # Source all appropriate tcl files here. 
    source "$fMRIEngine(modulePath)/tcl/notebook.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEnginePlot.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineModel.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineInspect.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineCompute.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineSequence.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineHelpText.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineContrasts.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineModelView.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineSignalModeling.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineParadigmDesign.tcl"
    source "$fMRIEngine(modulePath)/tcl/fMRIEngineRegionAnalysis.tcl"
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
    <BR><BR>
    <B>Sequence</B> allows you to load or select a sequence of fMRI \
    volumes to process.
    <BR>
    <B>Set up</B> allows you to specify the model and contrasts.
    <BR>
    <B>Compute</B> lets you to choose contrast(s) to compute \
    activation volume(s).
    <BR>
    <B>View</B> gives you the ability to view the activation \
    at different thresholds and dynamically plot any voxel \
    time course.
    <BR>
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
    bind $b <1> "fMRIEngineUpdateSetupTab" 

    #-------------------------------------------
    # ROI tab 
    #-------------------------------------------
    set fROI $Module(fMRIEngine,fROI)
    fMRIEngineBuildUIForROITab $fROI
    set b $Module(fMRIEngine,bROI)
    bind $b <1> "fMRIEngineUpdateBGVolumeList; \
        fMRIEngineUpdateCondsForROIPlot"
 
    #-------------------------------------------
    # Compute tab 
    #-------------------------------------------
    set fCompute $Module(fMRIEngine,fCompute)
    fMRIEngineBuildUIForComputeTab $fCompute
    set b $Module(fMRIEngine,bCompute)
    bind $b <1> "fMRIEngineUpdateContrastList" 

    #-------------------------------------------
    # View tab 
    #-------------------------------------------
    set fView $Module(fMRIEngine,fView)
    fMRIEngineBuildUIForViewTab $fView
    set b $Module(fMRIEngine,bView)
    bind $b <1> "fMRIEngineUpdateViewTab;fMRIEngineUpdateEVsForPlotting"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateHelpTab
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateHelpTab {} {
    global fMRIEngine

    set fMRIEngine(currentTab) "Help"
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
        set gplText [file join $env(SLICER_HOME) Modules vtkFMRIEngine data gpl.txt]
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
# .PROC fMRIEngineEnter
# Called when this module is entered by the user. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineEnter {} {
    global fMRIEngine Volume

    set Volume(name) ""

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
        {set xc %x; set yc %y; fMRIEngineProcessMouseEvent $xc $yc}

    EvAddWidgetToBindingSet FMRISlice0Events $Gui(fSl0Win) {fMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice1Events $Gui(fSl1Win) {fMRIEngineSlicesEvents}
    EvAddWidgetToBindingSet FMRISlice2Events $Gui(fSl2Win) {fMRIEngineSlicesEvents}    
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineProcessMouseEvent
# Processes mouse click on the three slice windows
# .ARGS
# x the X coordinate; y the Y coordinate
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineProcessMouseEvent {x y} {
    global fMRIEngine 

    if {$fMRIEngine(currentTab) == "ROI"} {
        fMRIEngineClickROI $x $y
    } elseif {$fMRIEngine(currentTab) == "Inspect"} {
        if {$fMRIEngine(tcPlottingOption) == ""} {
            DevErrorWindow "Timecourse or Peristimulus histogram?"
            return  
        }

        set fMRIEngine(voxelLocation,x) $x
        set fMRIEngine(voxelLocation,y) $y
        set fMRIEngine(timecoursePlot) "Voxel"
        fMRIEnginePlotTimecourse
    } else {
    }
}
