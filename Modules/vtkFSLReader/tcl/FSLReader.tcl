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
# FILE:        FSLReader.tcl
# PROCEDURES:  
#   FSLReaderInit
#   FSLReaderBuildGUI
#   FSLReaderWarn
#   FSLReaderShowVolumes
#   FSLReaderLoadVolume a
#   FSLReaderSetDirectory
#   FSLReaderLaunchBrowser
#   FSLReaderBuildVTK
#   FSLReaderEnter
#   FSLReaderExit
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
    global FSLReader Module Volume Model

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

    set Module($m,row1List) "Help Report Volumes"
    set Module($m,row1Name) "{Help} {Report} {Volumes}"
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
        {$Revision: 1.1 $} {$Date: 2004/07/30 15:47:23 $}]

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
    The FSLReader module is intended to display activation \
    results out of FSL from Univeristy of Oxford, UK.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Report:</B> View FSL report.
    <LI><B>Volumes:</B> Display Analyze volumes - usually \
    an activation overlay onto a structural or standard scan.
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
    DevAddButton $f.f.b "Browse..." FSLReaderSetDirectory 
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
    DevAddButton $f.bWeb "Read" FSLReaderLaunchBrowser
    
    pack $f.label2 $f.entry $f.bWeb -side left -padx $Gui(pad) -pady $Gui(pad)  

    #-------------------------------------------
    # Volumes frame
    #-------------------------------------------
    set fVolumes $Module(FSLReader,fVolumes)
    set f $fVolumes

    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    set f $fVolumes.fTop
    DevAddLabel $f.label "You should check this:"
    DevAddButton $f.bWarn "Warning" "FSLReaderWarn" 8 
    pack $f.label $f.bWarn -side left -padx $Gui(pad) -pady $Gui(pad)  

    set f $fVolumes.fMiddle
    DevAddFileBrowse $f FSLReader "bgFileName" "Background Volume:" \
        "" "hdr" "\$FSLReader(FSLDir)" \
        "Open" "Browse for a background volume" "" "Absolute"

    set f $fVolumes.fBottom
    DevAddFileBrowse $f FSLReader "fgFileName" \
        "Activation Volume:" "" "hdr" "\$FSLReader(FSLDir)" \
        "Open" "Browse for an activation volume" "" "Absolute"

    DevAddButton $fVolumes.bApply "Apply" "FSLReaderShowVolumes" 8 
    pack $fVolumes.bApply -side top -padx $Gui(pad) -pady $Gui(pad)  
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderWarn
# Create a warning dialog 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderWarn {} {

    DevWarningWindow \
    "Before overlaying an activation onto \
    a structrual or standard scan, you need \
    co-register them first using FSL tool named \
    \"flirt.\" For more information, check \
    http://www.fmrib.ox.ac.uk/fsl/flirt/index.html
    "
}

 
#-------------------------------------------------------------------------------
# .PROC FSLReaderShowVolumes
# Show volumes 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderShowVolumes {} {
    global FSLReader 

    if {! [info exists FSLReader(bgFileName)]} {
        DevErrorWindow "Please input a background volume."
        return
    }
    if {! [file exists $FSLReader(bgFileName)]} {
        DevErrorWindow "Background volume file doesn't exist: $FSLReader(bgFileName)."
        return
    }

    if {! [info exists FSLReader(fgFileName)]} {
        DevErrorWindow "Please input an activation volume."
        return
    }
    if {! [file exists $FSLReader(fgFileName)]} {
        DevErrorWindow "Activation volume file doesn't exist: $FSLReader(fgFileName)."
        return
    }

    FSLReaderLoadVolume 1 
    FSLReaderLoadVolume 0 
}


#-------------------------------------------------------------------------------
# .PROC FSLReaderLoadVolume
# Load a volume 
# .ARGS
# bg a boolean value; 1 - the volume displayed as background, 0 - foreground 
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
# .PROC FSLReaderSetDirectory
# Select a directory (folder) 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FSLReaderSetDirectory {} {
    global FSLReader Volume

    set newdir [tk_chooseDirectory -initialdir $Volume(DefaultDir)]
    set FSLReader(FSLDir) $newdir

    set FSLReader(htmlFile) $FSLReader(FSLDir)/report.html
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
    popEventManager
}
