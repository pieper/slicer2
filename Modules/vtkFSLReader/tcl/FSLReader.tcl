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
# FILE:        FSLReader.tcl
# PROCEDURES:  
#   FSLReaderInit
#   FSLReaderBuildGUI
#   FSLReaderSetPlottingOption option
#   FSLReaderSetBackgroundOption option
#   FSLReaderLoadAnalyze4D 
#   FSLReaderLoadTimeSeries
#   FSLReaderLoadBackgroundVolume
#   FSLReaderLoadModels
#   FSLReaderLoadForegroundVolume
#   FSLReaderLoadVolume bg
#   FSLReaderSetFSLDir option pathName
#   FSLReaderLaunchBrowser
#   FSLReaderBuildVTK
#   FSLReaderEnter
#   FSLReaderExit
#   FSLReaderPushBindings 
#   FSLReaderPopBindings 
#   FSLReaderCreateBindings  
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC FSLReaderInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderInit {} {
    global FSLReader Module Volume Model env

    set m FSLReader

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is to display the output of FSL from \
        University of Oxford, UK."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Haiying, Liu, BWH SPL, hliu@bwh.harvard.edu"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "IO"

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

    set Module($m,row1List) "Help Report TimeSeries ActOverlay"
    set Module($m,row1Name) "{Help} {Report} {Time Series} {Act Overlay} "
    set Module($m,row1,tab) Report 

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
    #   set Module($m,procVTK) FSLReaderBuildVTK
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
    set Module($m,procGUI) FSLReaderBuildGUI
    set Module($m,procVTK) FSLReaderBuildVTK
    set Module($m,procEnter) FSLReaderEnter
    set Module($m,procExit) FSLReaderExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) "Analyze"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.7 $} {$Date: 2005/04/19 18:35:20 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set FSLReader(count) 0
    set FSLReader(Volume1) $Volume(idNone)
    set FSLReader(Model1)  $Model(idNone)
    set FSLReader(FileName)  ""

    # Creates bindings
    FSLReaderCreateBindings 

    set FSLReader(modulePath) "$env(SLICER_HOME)/Modules/vtkFSLReader"

    # Source all appropriate tcl files here. 
    source "$FSLReader(modulePath)/tcl/FSLReaderPlot.tcl"
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
# .PROC FSLReaderBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderBuildGUI {} {
    global Gui FSLReader Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Display" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(FSLReader,fDisplay)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The FSLReader module is intended to view activation \
    results out of FSL from Univeristy of Oxford, UK.
    <P>
    The <B>Report</B> tab allows you to choose an FSL output \
    directory and to read the html report by specifying any \
    web browser available on your system.
    <BR><BR>
    You can load the time series (filtered_func_data.hdr) on \
    <B>Time Series</B> tab. You have options to turn on or off \
    the time series plotting.
    <P>
    On <B>Act Overlay</B> tab, you can overlay an activation \
    onto any background volume. For better display, you need \
    to make sure they are co-registered. If you already enabled \
    time series plotting on the previous tab, you may see voxel \
    time course plotting in a separate window as you move the \
    mouse over any of the three slice windows. Again, you need \
    verify that the activation, background volume and time series \
    are all in the same space. 
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags FSLReader $help
    MainHelpBuildGUI FSLReader

    #-------------------------------------------
    # Report frame
    #-------------------------------------------
    set fReport $Module(FSLReader,fReport)
    set f $fReport
    
    foreach frame "Upper Lower" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        $f.f$frame configure -relief groove -bd 3 \
            -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    # Report->Upper frame
    set f $fReport.fUpper
    
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.f -side top -padx $Gui(pad) -pady $Gui(pad)
   
    DevAddLabel  $f.f.l "FSL Output Directory" 
    DevAddButton $f.f.b "Browse..." "FSLReaderSetFSLDir 1" 
    pack $f.f.l $f.f.b -side left -padx $Gui(pad)

    eval {entry $f.efile -textvariable FSLReader(FSLDir) -width 50} $Gui(WEA)
    pack $f.efile -side top -pady $Gui(pad) -padx $Gui(pad) \
        -expand 1 -fill x    

    # Report->Lower frame
    set f $fReport.fLower
    
    DevAddLabel $f.label1 "Read Report:"
    pack $f.label1 -side top -padx $Gui(pad) -pady 4 

    DevAddFileBrowse $f FSLReader "htmlFile" "Html File:" \
        "" "html" "\$FSLReader(FSLDir)" \
        "Open" "Browse for an Analyze file" "" "Absolute"

    frame $f.fView -bg $Gui(activeWorkspace)
    pack $f.fView -side top  
    set f $f.fView
    DevAddLabel $f.label2 "Browser:"
    set FSLReader(browser) "mozilla"
    eval {entry $f.entry -width 10 \
        -textvariable FSLReader(browser)} $Gui(WEA)
    bind $f.entry <Return> FSLReaderLaunchBrowser

    # make a button that pops up the report 
    DevAddButton $f.bWeb "Read..." FSLReaderLaunchBrowser
    
    pack $f.label2 $f.entry $f.bWeb -side left -padx $Gui(pad) -pady $Gui(pad)  

    #-------------------------------------------
    # Time course frame
    #-------------------------------------------
    set fTimeSeries $Module(FSLReader,fTimeSeries)
    set f $fTimeSeries
    foreach frame "Upper Lower" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    # Load time series
    # ----------------
    set f $fTimeSeries.fUpper
    DevAddFileBrowse $f FSLReader "tcFileName" "Time Series:" \
        "FSLReaderSetFSLDir 2 \$FSLReader(tcFileName)" "hdr" \
        "\$FSLReader(FSLDir)" "Open" "Browse for a time series" \
        "" "Absolute"

    frame $f.fVolName -bg $Gui(activeWorkspace)
    pack $f.fVolName -side top -fill x
    set f $f.fVolName 
    DevAddLabel $f.flVolName "Vol Name:" 
    DevAddEntry Volume name $f.eTcVolName 25 
    $f.eTcVolName config -state readonly
    set FSLReader(eTcVolName) $f.eTcVolName
    pack $f.flVolName $f.eTcVolName -side left -padx $Gui(pad) 

    DevAddButton $fTimeSeries.fUpper.bApply "Load" "FSLReaderLoadTimeSeries" 13 
    pack $fTimeSeries.fUpper.bApply -side top -pady $Gui(pad) 

    # Options 
    # -------
    set f $fTimeSeries.fLower
    $f config -bg $Gui(backdrop)
 
    set ops [list No Yes]

    DevAddLabel $f.flOptions "Enable time series plotting: " 
    pack $f.flOptions -side left -padx $Gui(pad) -fill x -anchor w

    set df [lindex $ops 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 13 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 5 -padx $Gui(pad)

    # Add menu items
    foreach m $ops  {
        $f.mbType.m add command -label $m \
            -command "FSLReaderSetPlottingOption $m"
    }

    # save menubutton for config
    set FSLReader(gui,mbPlottingOption) $f.mbType
    FSLReaderSetPlottingOption $df

    #-------------------------------------------
    # Activation overlay frame
    #-------------------------------------------
    set fActOverlay $Module(FSLReader,fActOverlay)
    set f $fActOverlay

    foreach frame "Fg Bg" {
        frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 3 
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    # Fg frame
    #---------
    set f $fActOverlay.fFg

    DevAddLabel $f.flTitle "Foreground image:" 
    pack $f.flTitle -side top -pady 6  

    DevAddFileBrowse $f FSLReader "fgFileName" "Activation Volume:" \
        "FSLReaderSetFSLDir 2 \$FSLReader(fgFileName)" "hdr" \
        "\$FSLReader(FSLDir)" "Open" "Browse for an activation volume" \
        "" "Absolute"

    frame $f.fVolName -bg $Gui(activeWorkspace)
    pack $f.fVolName -side top -fill x
    set f $f.fVolName 
    DevAddLabel $f.flVolName "Vol Name:" 
    set FSLReader(fgVolName) None
    DevAddEntry FSLReader fgVolName $f.eFgVolName 25 
    $f.eFgVolName config -state readonly
    pack $f.flVolName $f.eFgVolName -side left -padx $Gui(pad) 

    DevAddButton $fActOverlay.fFg.bApply "Load" "FSLReaderLoadForegroundVolume" 13 
    pack $fActOverlay.fFg.bApply -side top -pady $Gui(pad) 

    # Bg frame
    #---------
    set f $fActOverlay.fBg

    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    # Top frame
    DevAddLabel $f.fTop.flTitle "Background image:" 
    pack $f.fTop.flTitle -side top -pady 2  

    # Middle frame 
    set f $fActOverlay.fBg.fMiddle
    $f config -bg $Gui(backdrop)
    
    DevAddLabel $f.flOptions "What to load: " 
    pack $f.flOptions -side left -padx $Gui(pad) -fill x -anchor w

    set options [list Time-Series-Volume Other-Volume]
    set df [lindex $options 1] 
    eval {menubutton $f.mbType -text $df \
        -relief raised -bd 2 -width 22 \
        -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)

    # Add menu items
    foreach m $options  {
        $f.mbType.m add command -label $m \
            -command "FSLReaderSetBackgroundOption $m"
    }

    # save menubutton for config
    set FSLReader(gui,mbBgOption) $f.mbType

    # Bottom frame
    set f $fActOverlay.fBg.fBottom
    $f config -height 110 
    set FSLReader(bgVolFrame) $f

    # Makes a frame for each reader submodule
    foreach m $options {
        frame $f.f${m} -bg $Gui(activeWorkspace) 
        place $f.f${m} -in $f -relheight 1.0 -relwidth 1.0
        switch $m {
            "Other-Volume" {

                set f $f.f${m}

                DevAddFileBrowse $f FSLReader "bgFileName" \
                    "Background Volume:" "" "hdr" "\$FSLReader(FSLDir)" \
                    "Open" "Browse for a background volume" "" "Absolute"

                frame $f.fVolName -bg $Gui(activeWorkspace)
                pack $f.fVolName -side top -fill x
                DevAddLabel $f.fVolName.flVolName "Vol Name:" 
                set FSLReader(bgVolName) None
                DevAddEntry FSLReader bgVolName $f.fVolName.eBgVolName 25 
                $f.fVolName.eBgVolName config -state readonly
                pack $f.fVolName.flVolName $f.fVolName.eBgVolName \
                    -side left -padx $Gui(pad) 

                DevAddButton $f.bApply "Load" "FSLReaderLoadBackgroundVolume" 13 
                pack $f.bApply -side top -pady $Gui(pad) 
            }
            "Time-Series-Volume" {
            }
        }
        set FSLReader(f$m) $f
    }

    FSLReaderSetBackgroundOption $df
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderSetPlottingOption
# Switches time series plotting options 
# .ARGS
# string option the option to be set 
# .END
#-------------------------------------------------------------------------------
proc FSLReaderSetPlottingOption {option} {
    global Volume FSLReader 

    # If a time series has been loaded
    if {$option == "Yes" &&
        ! ([info exists FSLReader(firstMRMLid)] && 
           [info exists FSLReader(lastMRMLid)])} {
 
        DevErrorWindow "Please load a time series first."
        return
    }

    # configure menubutton
    $FSLReader(gui,mbPlottingOption) config -text $option
    set FSLReader(tcPlottingOption) $option
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderSetBackgroundOption
# Switches background volume 
# .ARGS
# string option the volume tag to switch 
# .END
#-------------------------------------------------------------------------------
proc FSLReaderSetBackgroundOption {option} {
    global Volume FSLReader 

    set id 0
    if {$option == "Other-Volume"} {
        $FSLReader(bgVolFrame) config -height 110 

        if {[info exists FSLReader(bgVolName)]  &&
            $FSLReader(bgVolName) != "None"} {
            set id [MIRIADSegmentGetVolumeByName $FSLReader(bgVolName)]
        }
    } else {
        if {[info exists FSLReader(firstMRMLid)] &&
            [info exists FSLReader(lastMRMLid)]} {
            set id $FSLReader(firstMRMLid)
        } else {
            DevErrorWindow "Please load a time series first."
            return
        }

        $FSLReader(bgVolFrame) config -height 1 
    }

    if {$id > 0} {
        MainSlicesSetVolumeAll Back $id 
        RenderAll
    }
    
    # configure menubutton
    $FSLReader(gui,mbBgOption) config -text $option 
    set FSLReader(bgOption) $option

    raise $FSLReader(f$option)
    focus $FSLReader(f$option)
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadAnalyze4D 
# Reads an Analyze 4D file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadAnalyze4D {} {
    global AnalyzeCache FSLReader Volume Mrml

    unset -nocomplain AnalyzeCache(MRMLid)
    unset -nocomplain FSLReader(firstMRMLid)
    unset -nocomplain FSLReader(lastMRMLid)

    set AnalyzeCache(fileName) $FSLReader(tcFileName) 
    AnalyzeApply

    set FSLReader(firstMRMLid) [lindex $AnalyzeCache(MRMLid) 0] 
    set FSLReader(lastMRMLid) [lindex $AnalyzeCache(MRMLid) end] 
    set FSLReader(noOfAnalyzeVolumes) [llength $AnalyzeCache(MRMLid)] 

    # show the first volume by default
    MainSlicesSetVolumeAll Back [lindex $AnalyzeCache(MRMLid) 0] 
    RenderAll
}

   
#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadTimeSeries
# Loads a sequence of volumes 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadTimeSeries {} {
    global FSLReader Volume 

    if {! [info exists FSLReader(tcFileName)]} {
        DevErrorWindow "Please input a time series."
        return
    }
    if {! [file exists $FSLReader(tcFileName)]} {
        DevErrorWindow "Time series file doesn't exist: $FSLReader(tcFileName)."
        return
    }

    # Analyze 4D file
    FSLReaderLoadAnalyze4D

    set volName Volume(name) 
    $FSLReader(eTcVolName) config -textvariable volName

    # Loads models from tsplot directory 
    FSLReaderLoadModels
}
 

#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadBackgroundVolume
# Loads a background volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadBackgroundVolume {} {
    global FSLReader Volume 

    if {! [info exists FSLReader(bgFileName)]} {
        DevErrorWindow "Please input a background volume."
        return
    }
    if {! [file exists $FSLReader(bgFileName)]} {
        DevErrorWindow "Background volume file doesn't exist: $FSLReader(bgFileName)."
        return
    }

    FSLReaderLoadVolume 1 
    set FSLReader(bgVolName) $Volume(name)
}
 

#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadModels
# Loads models from tsplot directory 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadModels {} {
    global FSLReader 

    set pattern "tsplot_*.txt"
    set pattern [file join $FSLReader(FSLDir) tsplot $pattern] 
    set plotFiles [glob -nocomplain $pattern]

    if {$plotFiles == ""} {
        DevErrorWindow "Time course text files don't exist."
        return
    }

    foreach f $plotFiles {

        set name [string range [file tail $f] 7 end-4]

        # puts "file = $name"

        # Reads file
        set fp [open $f r]
        set data [string trim [read $fp]]
        set lines [split $data "\n"]
        close $fp

        vtkFloatArray FSLReader($name,model)     
        FSLReader($name,model) SetNumberOfTuples $FSLReader(noOfAnalyzeVolumes)
        FSLReader($name,model) SetNumberOfComponents 1

        set count 0
        set i [string first "f" $name]
        set n [expr {$i == -1 ? 2 : 1}] 
        foreach l $lines {
            set tokens [split $l " "]
            FSLReader($name,model) SetComponent $count 0 [lindex $tokens $n]
            incr count
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadForegroundVolume
# Loads a foreground volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadForegroundVolume {} {
    global FSLReader Volume 

    if {! [info exists FSLReader(fgFileName)]} {
        DevErrorWindow "Please input an activation volume."
        return
    }
    if {! [file exists $FSLReader(fgFileName)]} {
        DevErrorWindow "Activation volume file doesn't exist: $FSLReader(fgFileName)."
        return
    }

    FSLReaderLoadVolume 0 

    set FSLReader(fgVolName) $Volume(name)
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadVolume
# Load a volume 
# .ARGS
# boolean bg 1 - the volume displayed as background, 0 - foreground 
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLoadVolume {bg} {
    global Volume FSLReader AnalyzeCache 

    set fileName [expr {$bg == 1 ? $FSLReader(bgFileName) : \
        $FSLReader(fgFileName)}]
    set AnalyzeCache(fileName) $fileName 

    if {[file extension $fileName] == ".hdr"} {
        set fileName [AnalyzeSwitchExtension $fileName]
    }
    set volName [AnalyzeCreateVolumeNameFromFileName $fileName] 
    set volName $volName-1

    # Checks if the volume is already loaded.
    set found 0
    foreach v $Volume(idList) {
        set name [Volume($v,node) GetName]
            if {$volName == $name} {
                set found 1 
            }
    }
    if {! $found} {
        AnalyzeApply
    }

    set id [MIRIADSegmentGetVolumeByName $volName]
    set opt [expr {$bg == 1 ? "Back" : "Fore"}]

    MainSlicesSetVolumeAll $opt $id 
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderSetFSLDir
# Sets FSL output directory 
# .ARGS
# string option where to call
# path pathName the input where the FSL output directory to be derived
# .END
#-------------------------------------------------------------------------------
proc FSLReaderSetFSLDir {option {pathName ""}} {
    global FSLReader Volume
   
    if {$option == "1"} {
        set FSLReader(FSLDir) \
            [tk_chooseDirectory -initialdir $Volume(DefaultDir)]
    } else {
        if {! [file exists $pathName]} {
            set dir ""
            return
        } else {
            set dir [file dirname $pathName] 
        }
        set FSLReader(FSLDir) $dir
    }

    if {$FSLReader(FSLDir) == ""} {

        set FSLReader(htmlFile) "" 
        set FSLReader(tcFileName) "" 
        set FSLReader(fgFileName) "" 
        set FSLReader(bgFileName) "" 
    } else {
        set FSLReader(htmlFile) [file join $FSLReader(FSLDir) report.html]
        set FSLReader(tcFileName) [file join $FSLReader(FSLDir) \
            filtered_func_data.hdr]
    }
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderLaunchBrowser
# Launch a specified browser to view html doc 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderLaunchBrowser {} {
    global FSLReader

    if {! [info exists FSLReader(htmlFile)]} {
        DevErrorWindow "Input your html file."
        return
    }
    if {[string trim $FSLReader(htmlFile)] == ""} {
        DevErrorWindow "Input your html file."
        return
    }
    if {[file exists $FSLReader(htmlFile)] == 0} {
        DevErrorWindow "Report doesn't exist: $FSLReader(htmlFile)."
        return
    }

    set protocol "file://"
    set browserUrl $protocol$FSLReader(htmlFile)
    set browser [string trim $FSLReader(browser)] 
    if {$browser == ""} {
        DevErrorWindow "Input your valid web browser name (e.g. mozilla) on your platform."
        return
    }

    catch {exec $browser $browserUrl &}
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderBuildVTK {} {

}


#-------------------------------------------------------------------------------
# .PROC FSLReaderEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderEnter {} {
    global FSLReader
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    # pushEventManager $FSLReader(eventManager)

    # clear the text box and put instructions there
    # $FSLReader(textBox) delete 1.0 end
    # $FSLReader(textBox) insert end "Shift-Click anywhere!\n"

    FSLReaderPushBindings
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    # popEventManager
    FSLReaderPopBindings
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderPushBindings 
# Pushes onto the event stack a new event manager that
# deals with events when the FSLReader module is active
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderPushBindings {} {
   global Ev Csys

    EvActivateBindingSet FSLSlice0Events
    EvActivateBindingSet FSLSlice1Events
    EvActivateBindingSet FSLSlice2Events
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderPopBindings 
# Removes bindings when FSLReader module is inactive
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderPopBindings {} {
    global Ev Csys

    EvDeactivateBindingSet FSLSlice0Events
    EvDeactivateBindingSet FSLSlice1Events
    EvDeactivateBindingSet FSLSlice2Events
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderCreateBindings  
# Creates FSLReader event bindings for the three slice windows 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderCreateBindings {} {
    global Gui Ev

    EvDeclareEventHandler FSLReaderSlicesEvents <Motion> \
        {FSLReaderPopUpPlot %x %y}
            
    EvAddWidgetToBindingSet FSLSlice0Events $Gui(fSl0Win) {FSLReaderSlicesEvents}
    EvAddWidgetToBindingSet FSLSlice1Events $Gui(fSl1Win) {FSLReaderSlicesEvents}
    EvAddWidgetToBindingSet FSLSlice2Events $Gui(fSl2Win) {FSLReaderSlicesEvents}    
}


