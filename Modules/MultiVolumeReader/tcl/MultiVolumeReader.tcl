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
# FILE:        MultiVolumeReader.tcl
# PROCEDURES:  
#   MultiVolumeReaderInit
#   MultiVolumeReaderBuildGUI  the
#   MultiVolumeReaderUpdateVolume the
#   MultiVolumeReaderSetWindowLevelThresholds 
#   MultiVolumeReaderSetFileFilter 
#   MultiVolumeReaderLoad 
#   MultiVolumeReaderLoadAnalyze 
#   MultiVolumeReaderLoadBXH 
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderInit {} { 
    global MultiVolumeReader Module Volume Model
    
    set m MultiVolumeReader

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is to load a sequence of volumes into slicer."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Liu, Haiying, SPL, Brigham and Women's Hospital, hliu@bwh.harvard.edu"
    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "I/O"

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
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
#    set Module($m,row1List) "Help Stuff"
#    set Module($m,row1Name) "{Help} {Tons o' Stuff}"
#    set Module($m,row1,tab) Stuff

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) MultiVolumeReaderBuildVTK
    #
    #   All the options are:

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
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
#    set Module($m,procGUI) MultiVolumeReaderBuildGUI
#    set Module($m,procVTK) MultiVolumeReaderBuildVTK
#    set Module($m,procEnter) MultiVolumeReaderEnter
#    set Module($m,procExit) MultiVolumeReaderExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
#    set Module($m,depend) "Analyze BXH"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.7 $} {$Date: 2004/11/24 19:05:42 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
#    set MultiVolumeReader(count) 0
#    set MultiVolumeReader(Volume1) $Volume(idNone)
#    set MultiVolumeReader(Model1)  $Model(idNone)
#    set MultiVolumeReader(FileName)  ""
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
# .PROC MultiVolumeReaderBuildGUI 
# Creates UI for for user input 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderBuildGUI {parent} {
    global Gui MultiVolumeReader Module Volume Model
   
    set f $parent
    frame $f.fVols -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fNav -bg $Gui(activeWorkspace) 
    pack $f.fVols $f.fNav -side top -pady $Gui(pad)

    # The Volume frame
    set f $parent.fVols
    DevAddLabel $f.lNote "Configure the reader:"
    frame $f.fConfig -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.lNote -side top -pady 6 
    pack $f.fConfig -side top

    set f $parent.fVols.fConfig
    set MultiVolumeReader(fileTypes) {hdr .bxh}
    DevAddFileBrowse $f MultiVolumeReader "fileName" "File Name:" \
        "MultiVolumeReaderSetFileFilter" "\$MultiVolumeReader(fileTypes)" \
        "\$Volume(DefaultDir)" "Open" "Browse for a volume file" "" "Absolute"

    frame $f.fFilter -bg $Gui(activeWorkspace)
    pack $f.fFilter -pady $Gui(pad)
    set f $f.fFilter

    set filter \
        "Load a single file: Only read the above input file.\n\
        Load multiple files: Read files in the same directory\n\
        matching the pattern in the Filter field.              "

    eval {radiobutton $f.r1 -width 27 -text {Load a single file} \
        -variable MultiVolumeReader(filterChoice) -value single \
        -relief flat -offrelief flat -overrelief raised \
        -selectcolor blue} $Gui(WEA)
    pack $f.r1 -side top -pady 2 
    TooltipAdd $f.r1 $filter 
    frame $f.fMulti -bg $Gui(activeWorkspace) -relief groove -bd 1 
    pack $f.fMulti -pady 3
    set f $f.fMulti
    eval {radiobutton $f.r2 -width 27 -text {Load multiple files} \
        -variable MultiVolumeReader(filterChoice) -value multiple \
        -relief flat -offrelief flat -overrelief raised \
        -selectcolor blue} $Gui(WEA)
    TooltipAdd $f.r2 $filter 

    DevAddLabel $f.lFilter " Filter:"
    eval {entry $f.eFilter -width 24 \
        -textvariable MultiVolumeReader(filter)} $Gui(WEA)
    bind $f.eFilter <Return> "MultiVolumeReaderLoad" 
    TooltipAdd $f.eFilter $filter 

    #The "sticky" option aligns items to the left (west) side
    grid $f.r2 -row 0 -column 0 -columnspan 2 -padx 5 -pady 3 -sticky w
    grid $f.lFilter -row 1 -column 0 -padx 1 -pady 3 -sticky w
    grid $f.eFilter -row 1 -column 1 -padx 1 -pady 3 -sticky w

    set MultiVolumeReader(filterChoice) single
    set MultiVolumeReader(singleRadiobutton) $f.r1
    set MultiVolumeReader(multipleRadiobutton) $f.r2
    set MultiVolumeReader(filterEntry) $f.eFilter
    
    set f $parent.fVols
    DevAddButton $f.bApply "Apply" "MultiVolumeReaderLoad" 12 
    pack $f.bApply -side top -pady 5 

    frame $f.fVName -bg $Gui(activeWorkspace)
    pack $f.fVName  -pady 5 
    set f $f.fVName
    DevAddLabel $f.lVName "Load status (latest loaded volume):"
    set MultiVolumeReader(emptyLoadStatus) ""
    eval {entry $f.eVName -width 30 \
        -state normal \
        -textvariable MultiVolumeReader(emptyLoadStatus)} $Gui(WEA)
    TooltipAdd $f.eVName \
        "Don't need to input anything here. This\n\
        entry is used to display loading status."
    pack $f.lVName $f.eVName -side top -padx $Gui(pad) -pady 2 
    if { [ info exists MultiVolumeReaser(loadStatusEntry)] } {
        set MultiVolumeReader(loadStatusEntry) $f.eVName
    }
    
    # The Navigate frame
    set f $parent.fNav

    DevAddButton $f.bSet "Set Window/Level/Thresholds" \
        "MultiVolumeReaderSetWindowLevelThresholds" 30
    # put a tooltip over the button 
    TooltipAdd $f.bSet \
        "Set window, level and low/high threshold\n\
        for the first volume. Hit this button to set\n\
        the same values for the entire sequence."
 
    DevAddLabel $f.lVolNo "Vol Index:"
    eval { scale $f.sSlider \
        -orient horizontal \
        -from 0 -to 0 \
        -resolution 1 \
        -bigincrement 10 \
        -length 130 \
        -state active \
        -command {MultiVolumeReaderUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}
    set MultiVolumeReader(slider) $f.sSlider
    TooltipAdd $f.sSlider \
        "Slide this scale to navigate multi-volume sequence."
 
    #The "sticky" option aligns items to the left (west) side
    grid $f.bSet -row 0 -column 0 -columnspan 2 -padx 5 -pady 3 -sticky w
    grid $f.lVolNo -row 1 -column 0 -padx 1 -pady 1 -sticky w
    grid $f.sSlider -row 1 -column 1 -padx 1 -pady 1 -sticky w
}


#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderUpdateVolume
# Updates image volume as user moves the slider 
# .ARGS
# volumeNo the volume number
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderUpdateVolume {volumeNo} {
    global MultiVolumeReader 

    if {$volumeNo == 0} {
        return
    }

    set v [expr $volumeNo-1]
    set id [expr $MultiVolumeReader(firstMRMLid)+$v]

    MainSlicesSetVolumeAll Back $id 
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderSetWindowLevelThresholds 
# Sets window, level and thresholds for the entire sequence using the values
# of the first volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderSetWindowLevelThresholds {} {
    global MultiVolumeReader Volume

    if {! [info exists MultiVolumeReader(noOfVolumes)]} {
        return
    }

    set low [Volume($MultiVolumeReader(firstMRMLid),node) GetLowerThreshold]
    set win [Volume($MultiVolumeReader(firstMRMLid),node) GetWindow]
    set level [Volume($MultiVolumeReader(firstMRMLid),node) GetLevel]
    set MultiVolumeReader(lowerThreshold) $low

    set i $MultiVolumeReader(firstMRMLid)
    while {$i <= $MultiVolumeReader(lastMRMLid)} {
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
# .PROC MultiVolumeReaderSetFileFilter 
# Sets the file filter depending on file type 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderSetFileFilter {} {
    global MultiVolumeReader

    set fileName $MultiVolumeReader(fileName)
    set fileName [string trim $fileName]
    if {$fileName == ""} {
        return
    }

    if {! [file exists $fileName]} {
        return
    }

    set MultiVolumeReader(fileName) $fileName
    set MultiVolumeReader(fileExtension) [file extension $fileName]
    switch $MultiVolumeReader(fileExtension) {
        ".hdr" {
            $MultiVolumeReader(multipleRadiobutton) configure -state active 
            $MultiVolumeReader(filterEntry) configure -state normal 
        }
        ".bxh" {
            set MultiVolumeReader(filterChoice) single
            $MultiVolumeReader(multipleRadiobutton) configure -state disabled 
            $MultiVolumeReader(filterEntry) configure -state disabled 
        }
        default {
        }
    }
}   


#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderLoad 
# Loads volumes 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderLoad {} {
    global MultiVolumeReader Volume

    set fileName $MultiVolumeReader(fileName)
    set fileName [string trim $fileName]
    if {$fileName == ""} {
        DevErrorWindow "File name is empty."
        return 1
    }

    if {! [file exists $fileName]} {
        DevErrorWindow "File doesn't exist: $fileName."
        set MultiVolumeReader(fileName) ""
        return 1
    }

    unset -nocomplain MultiVolumeReader(noOfVolumes)
    unset -nocomplain MultiVolumeReader(firstMRMLid)
    unset -nocomplain MultiVolumeReader(lastMRMLid)
    unset -nocomplain MultiVolumeReader(volumeExtent)

    if { [ info exists MultiVolumeReaser(loadStatusEntry)] } {
        $MultiVolumeReader(loadStatusEntry) configure -textvariable \
            Volume(name)
    }

    switch $MultiVolumeReader(fileExtension) {
        ".hdr" {
            set val [MultiVolumeReaderLoadAnalyze]
        }
        ".bxh" {
            set val [MultiVolumeReaderLoadBXH]
        }
        default {
            DevErrorWindow "Can't read this file for a volume sequence: $fileName."
            set val 1
        }
    }

    set MultiVolumeReader(fileName) ""

    if {$val == 1} {
        return 1
    }
 
    # Sets range for the volume slider
    if { [info exists ::MultiVolumeReader(slider)] } {
        $MultiVolumeReader(slider) configure -from 1 -to $MultiVolumeReader(noOfVolumes)
    }
    # Sets the first volume in the sequence as the active volume
    MainVolumesSetActive $MultiVolumeReader(firstMRMLid)

    if { [ info exists MultiVolumeReaser(loadStatusEntry)] } {
        $MultiVolumeReader(loadStatusEntry) configure -textvariable \
            MultiVolumeReader(emptyLoadStatus)
    }

    return 0
}   


#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderLoadAnalyze 
# Loads Analyze volumes. It returns 0 if successful; 1 otherwise. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderLoadAnalyze {} {
    global MultiVolumeReader AnalyzeCache Volume Mrml

    unset -nocomplain AnalyzeCache(MRMLid)

    set fileName $MultiVolumeReader(fileName)
    set analyzeFiles [list $fileName]
 
    # file filter
    if {$MultiVolumeReader(filterChoice) == "multiple"} {
        set path [file dirname $fileName]
        set name [file tail $fileName]

        set filter $MultiVolumeReader(filter)
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
                return 1
            }

            set analyzeFiles [lsort -dictionary $fileList]
        }
    }

    foreach f $analyzeFiles { 
        # VolAnalyzeApply without argument is for Cindy's data (IS scan order) 
        # VolAnalyzeApply "PA" is for Chandlee's data (PA scan order) 
        # set id [VolAnalyzeApply "PA"]
        # lappend mrmlIds [VolAnalyzeApply]
        set AnalyzeCache(fileName) $f 
        set val [AnalyzeApply]
        if {$val == 1} {
            return $val
        }
    }

    set MultiVolumeReader(firstMRMLid) [lindex $AnalyzeCache(MRMLid) 0] 
    set MultiVolumeReader(lastMRMLid) [lindex $AnalyzeCache(MRMLid) end] 
    set MultiVolumeReader(noOfVolumes) [llength $AnalyzeCache(MRMLid)] 
    set MultiVolumeReader(volumeExtent) $AnalyzeCache(volumeExtent) 

    # show the first volume by default
    MainSlicesSetVolumeAll Back $MultiVolumeReader(firstMRMLid)
    RenderAll

    return 0
}


#-------------------------------------------------------------------------------
# .PROC MultiVolumeReaderLoadBXH 
# Loads BXH volumes. It returns 0 if successful; 1 otherwise. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MultiVolumeReaderLoadBXH {} {
    global MultiVolumeReader VolBXH 

    set VolBXH(bxh-fileName) $MultiVolumeReader(fileName)
    set val [VolBXHLoadVolumes]
    if {$val == 1} {
        return $val
    }

    set MultiVolumeReader(firstMRMLid) [lindex $VolBXH(MRMLid) 0] 
    set MultiVolumeReader(lastMRMLid) [lindex $VolBXH(MRMLid) end] 
    set MultiVolumeReader(noOfVolumes) [llength $VolBXH(MRMLid)] 
    set MultiVolumeReader(volumeExtent) $VolBXH(volumeExtent) 

    return 0
}

