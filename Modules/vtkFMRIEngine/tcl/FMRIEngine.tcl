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
#   FMRIEngineBuildUIForDetectors the
#   FMRIEngineSetDetector the
#   FMRIEngineBuildUIForParadigm the
#   FMRIEngineBuildUIForSelect the
#   FMRIEngineSelectSequence
#   FMRIEngineUpdateSequences
#   FMRIEngineBuildUIForLoad the
#   FMRIEngineScaleActivation the
#   FMRIEngineSetImageFormat the
#   FMRIEngineBuildUIForAnalyze the
#   FMRIEngineEnter
#   FMRIEngineExit
#   FMRIEngineUpdateVolume the
#   FMRIEngineLoadVolumes 
#   FMRIEngineLoadAnalyzeVolumes 
#   FMRIEngineComputeActivationVolume 
#   FMRIEngineSetWindowLevelThresholds
#   FMRIEnginePushBindings 
#   FMRIEnginePopBindings 
#   FMRIEngineCreateBindings  
#   FMRIEngineSetVolumeName   
#==========================================================================auto=

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
    set Module($m,depend) "Analyze BXH"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.21 $} {$Date: 2004/07/22 19:08:26 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set FMRIEngine(dir)  ""
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
    This module allows you to load a sequence of fMRI data volumes, \
    to input paradigm design file, to compute activation volume, and \
    to view time course voxel by voxel. You can save the activation \
    volume and easily load it at a later time.
    <P>
    Tab content descriptions:
    <BR>
    <LI><B>Load:</B><BR>
    By browsing you can input any file name in a sequence of fMRI data \
    volumes. The Name field displays a name for each volume. Press Load button \
    to start loading. After all volumes are loaded, you can move the slider \
    to check volumes one at a time. Use Clear button to clean the module \
    for next activation computation; it removes all volumes inside the module \
    and updates the display accordingly.
    <BR>
    <LI><B>Compute:</B><BR>
    Input your paradigm design file. Hit Compute button to compute activation \
    volume. After the computation is done, adjust window, level and thresholds \
    to make the activation volume a better view. Re-enter the FMRIEngine module, \
    and then you can view a voxel time course plot by hittig any voxel on \
    one of the three slices at the bottom part of the view window.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags FMRIEngine $help
    MainHelpBuildGUI FMRIEngine
    
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
                    -pages {{Load Seq} {Select Seq}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 356 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1
    set w [Notebook:frame $f.fNotebook {Load Seq}]
    FMRIEngineBuildUIForLoad $w
    set w [Notebook:frame $f.fNotebook {Select Seq}]
    FMRIEngineBuildUIForSelect $w

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
    
    frame $f.fActScale  -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.fActScale -side top -fill x -pady $Gui(pad)
    
    set f $f.fActScale 
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

        grid $f.l$param $f.e$param -padx $Gui(pad) -pady $Gui(pad) -sticky e
        grid $f.e$param -sticky w
    }
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

    frame $parent.fType  -bg $Gui(backdrop)
    frame $parent.fApply -bg $Gui(activeWorkspace)
    frame $parent.fName  -bg $Gui(activeWorkspace)
    pack $parent.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)
    pack $parent.fName $parent.fApply -side top -fill x -pady $Gui(pad)

    # Type frame
    set f $parent.fType

    # Build pulldown menu image format 
    eval {label $f.l -text "Detector Type:"} $Gui(BLA)
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

    # Apply frame
    set f $parent.fApply
    DevAddButton $f.bApply "Compute" "FMRIEngineComputeActivationVolume" 8
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

    frame $parent.fFile  -bg $Gui(activeWorkspace)
    pack $parent.fFile -side top -fill x -pady $Gui(pad)

    # File frame
    set f $parent.fFile
    DevAddFileBrowse $f FMRIEngine "paradigmFileName" "Paradigm design file:" \
        "" "txt" "\$Volume(DefaultDir)" \
        "Open" "Browse for a text file" "" "Absolute" 
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
    pack $parent.fTop $parent.fMiddle $parent.fBottom \
        -side top -padx 10 

    set f $parent.fTop
    DevAddLabel $f.l "Sequence list from Ibrowser:"
    pack $f.l -side top -pady 10   

    set f $parent.fMiddle
    listbox $f.lb -yscrollcommand "$parent.fMiddle.sb set" -bg $Gui(activeWorkspace) 
    scrollbar $f.sb -orient vertical -bg $Gui(activeWorkspace) -command "$parent.fMiddle.lb yview"
    pack $f.lb -side left -expand 1 -fill both
    pack $f.sb -side left -fill y

    set FMRIEngine(seqsListBox) $f.lb
    FMRIEngineUpdateSequences

    set f $parent.fBottom
    DevAddButton $f.bSelect "Select" "FMRIEngineSelectSequence" 10 
    DevAddButton $f.bUpdate "Update" "FMRIEngineUpdateSequences" 10 
    pack $f.bUpdate $f.bSelect -side left -expand 1 -pady 10 -padx 5 -fill both
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSelectSequence
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSelectSequence {} {
    global FMRIEngine 

    set ci [$FMRIEngine(seqsListBox) cursel]
    if {[string length $ci] > 0} { 
        set cc [$FMRIEngine(seqsListBox) get $ci]
        set id $Ibrowser($cc,id)
        set FMRIEngine(firstMRMLid) Ibrowser($id,firstMRMLid)
        set FMRIEngine(lastMRMLid) Ibrowser($id,lastMRMLid)
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateSequences
# Updates sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateSequences {} {
    global FMRIEngine Ibrowser 

    # clears the listbox
    set size [$FMRIEngine(seqsListBox) size]
    $FMRIEngine(seqsListBox) delete 0 [expr $size-1]

    set b [info exists Ibrowser(idList)] 
    set n [expr {$b == 0 ? 0 : [llength $Ibrowser(idList)]}]
    if {$n > 0} {
        set i 0
        while {$i < $n} {
            set id [lindex $Ibrowder(idList) $i]
            $FMRIEngine(seqsListBox) insert end $Ibrowser($id,name) 
            incr i
        }
    } else {
        $FMRIEngine(seqsListBox) insert end None 
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

    frame $parent.fTop -bg $Gui(backdrop) -relief sunken -bd 2  
    frame $parent.fMiddle -bg $Gui(activeWorkspace) -height 270  
    frame $parent.fBot -bg $Gui(activeWorkspace)   

    grid $parent.fTop    -row 1 -column 0 -sticky ew 
    grid $parent.fMiddle -row 2 -column 0 -sticky ew 
    grid $parent.fBot    -row 4 -column 0 -sticky ew -pady 0 
 
    #------------------------------
    # Bottom frame
    #------------------------------
    set f $parent.fBot
    frame $f.fLogos  -bg $Gui(activeWorkspace)
    pack $f.fLogos \
        -side bottom -fill x -pady $Gui(pad)

    # Bottom->Logos frame
    set f $parent.fBot.fLogos
    set uselogo [image create photo -file \
        $FMRIEngine(modulePath)/tcl/images/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
        -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 0 -pady $Gui(pad) -expand 0

    #------------------------------
    # Middle frame
    #------------------------------
    set f $parent.fMiddle

    # All image formats supported
    set imgFormatList {Analyze BXH}

    # Makes a frame for each reader submodule
    foreach m $imgFormatList {
        frame $f.f${m} -bg $Gui(activeWorkspace)
        place $f.f${m} -in $f -relheight 1.0 -relwidth 1.0

        switch $m {
            "Analyze" {
                FMRIEngineBuildUIForAnalyze $f.f${m}
            }
            "BXH" {
                VolBXHBuildGUI $f.f${m}
            }
        }
        set FMRIEngine(f$m) $f.f${m}
    }
    # raise the default one 
    raise $FMRIEngine(fAnalyze)

    #------------------------------
    # Top frame
    #------------------------------
    set f $parent.fTop

    frame $f.fType   -bg $Gui(backdrop)
    pack $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    # Top->Type frame
    set f $parent.fTop.fType

    # Build pulldown menu image format 
    eval {label $f.l -text "Image Format:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # Analyze is default format 
    set df [lindex $imgFormatList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)

    # Add menu items
    foreach m $imgFormatList  {
        $f.mbType.m add command -label $m \
            -command "FMRIEngineSetImageFormat $m"
    }

    # save menubutton for config
    set FMRIEngine(gui,mbImgFormat) $f.mbType
    # put a tooltip over the menu
    # TooltipAdd $f.mbType \
    #        "Choose the type of file information to display."

    # Analyze is default format
    FMRIEngineSetImageFormat $df
    set FMRIEngine(imageFormat) $df
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

    if {[info exists FMRIEngine(noOfAnalyzeVolumes)] == 1} {

        vtkCDF cdf
        set dof [expr $FMRIEngine(noOfAnalyzeVolumes) - 1]
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
# .PROC FMRIEngineBuildUIForAnalyze
# Creates UI for Analyze input 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForAnalyze {parent} {
    global FMRIEngine Gui Volume 

    set f $parent
    frame $f.fVolume     -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fSlider     -bg $Gui(activeWorkspace)
    frame $f.fApply      -bg $Gui(activeWorkspace)
    frame $f.fStatus     -bg $Gui(activeWorkspace)
 
    pack $f.fVolume $f.fSlider $f.fApply $f.fStatus \
        -side top -fill x -pady $Gui(pad)

    set f $parent.fVolume
    DevAddFileBrowse $f Volume "VolAnalyze,FileName" "Analyze Header File:" \
        "VolAnalyzeSetFileName" "hdr" "\$Volume(DefaultDir)" \
        "Open" "Browse for an Analyze header file (.hdr) that has a matching .img" \
        "" "Absolute"
    DevAddLabel $f.label "Filter:"
    eval {entry $f.entry -width 30 \
              -textvariable FMRIEngine(filter)} $Gui(WEA)
    pack $f.label $f.entry -side left -padx $Gui(pad) -pady 5
 
    set f $parent.fSlider
    DevAddLabel $f.label "Volume No:"
    eval { scale $f.slider \
        -orient horizontal \
        -from 0 -to 0 \
        -resolution 1 \
        -bigincrement 10 \
        -length 160 \
        -state disabled \
        -command {FMRIEngineUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}

    set FMRIEngine(slider) $f.slider
    pack $f.label $f.slider -side left -expand false -fill x

    set f $parent.fApply
    DevAddButton $f.bApply "Load" "FMRIEngineLoadVolumes" 8 
    DevAddButton $f.bThrlds "Set Win/Lev/Thresholds" \
        "FMRIEngineSetWindowLevelThresholds" 20 
    grid $f.bApply $f.bThrlds -padx $Gui(pad)

    set f $parent.fStatus
    set FMRIEngine(name) " "
    eval {label $f.eName -textvariable FMRIEngine(name) -width 50} $Gui(WLA)
    pack $f.eName -side left -padx 0 -pady 25 
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
# .PROC FMRIEngineLoadVolumes 
# Reads a series of Analyze files
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineLoadVolumes {} {
    global FMRIEngine 

    switch $FMRIEngine(imageFormat) {
        "Analyze" {
            FMRIEngineLoadAnalyzeVolumes 
        }
        "BXH" {
            VolBXHLoadVolumes
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineLoadAnalyzeVolumes 
# Reads a series of Analyze files
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineLoadAnalyzeVolumes {} {
    global AnalyzeCache FMRIEngine Volume Mrml

    $FMRIEngine(slider) set 0 
    $FMRIEngine(slider) configure -showvalue 1 

    unset -nocomplain FMRIEngine(noOfAnalyzeVolumes)
    unset -nocomplain FMRIEngine(firstMRMLid)
    unset -nocomplain FMRIEngine(lastMRMLid)

    set fileName $Volume(VolAnalyze,FileName)
    if {$fileName == ""} {
        DevErrorWindow "File name field is empty."
        return
    }

    # file filter
    set path [file dirname $fileName]
    set filter $FMRIEngine(filter)
    string trim $filter
    set len [string length $filter]
    if {$len == 0} {
        lappend analyzeFiles $fileName
    } else {
        set hdr ".hdr"
        set ext [file extension $filter]

        if {$ext == ".*"} {
            set filter [string replace $filter [expr $len-2] end $hdr] 
        } elseif {$ext == $hdr} {
        } else {
            set filter $filter$hdr
        }
        set pattern [file join $path $filter]
        set fileList [glob -nocomplain $pattern]
        if {$fileList == ""} {
            DevErrorWindow "No Analyze file is selected through your filter: $filter"
            return
        }

        set analyzeFiles [lsort -dictionary $fileList]
    }

    unset -nocomplain AnalyzeCache(MRMLid)
    foreach f $analyzeFiles { 
       
        # VolAnalyzeApply without argument is for Cindy's data (IS scan order) 
        # VolAnalyzeApply "PA" is for Chandlee's data (PA scan order) 
        # set id [VolAnalyzeApply "PA"]
        # lappend mrmlIds [VolAnalyzeApply]

       set AnalyzeCache(fileName) $f 
       AnalyzeApply
    }

    set FMRIEngine(firstMRMLid) [lindex $AnalyzeCache(MRMLid) 0] 
    set FMRIEngine(lastMRMLid) [lindex $AnalyzeCache(MRMLid) end] 
    set FMRIEngine(noOfAnalyzeVolumes) [llength $AnalyzeCache(MRMLid)] 
    set ext [[Volume([lindex $AnalyzeCache(MRMLid) 0],vol) GetOutput] GetWholeExtent]
    set FMRIEngine(volextent) $ext 

    $FMRIEngine(slider) configure -from 1 -to [llength $AnalyzeCache(MRMLid)] 
    $FMRIEngine(slider) configure -state active
    $FMRIEngine(slider) configure -showvalue 1 

    # show the first volume by default
    MainSlicesSetVolumeAll Back [lindex $AnalyzeCache(MRMLid) 0] 
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
    if {! [FMRIEngineParseParadigm]} {
        return
    }

    set stimSize [$FMRIEngine(stimulus) GetNumberOfTuples]
    set volSize [FMRIEngine(actvolgen) GetNumberOfInputs]
    if {$stimSize != $volSize} {
        DevErrorWindow "Stimulus size ($stimSize) is not same as no of volumes ($volSize)."
        return
    }

    if {[info commands FMRIEngine(detector)] != ""} {
        FMRIEngine(detector) Delete
        unset -nocomplain FMRIEngine(detector)
    }
    vtkActivationDetector FMRIEngine(detector)
    FMRIEngine(detector) SetDetectionMethod 1
    FMRIEngine(detector) SetNumberOfPredictors [lindex $FMRIEngine(paradigm) 1] 
    FMRIEngine(detector) SetStimulus $FMRIEngine(stimulus) 

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
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetWindowLevelThresholds
# For a time series, set window, level, and low/high thresholds for all volumes
# with the first volume's values
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetWindowLevelThresholds {} {
   global FMRIEngine Volume 

    if {[info exists FMRIEngine(noOfAnalyzeVolumes)] == 0} {
        DevErrorWindow "Please load volumes first."
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
