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
#   FMRIEngineSetImageFormat the
#   FMRIEngineBuildUIForAnalyze the
#   FMRIEngineEnter
#   FMRIEngineExit
#   FMRIEngineUpdateVolume the
#   FMRIEngineLoadVolumes 
#   FMRIEngineLoadAnalyzeVolumes 
#   FMRIEngineComputeActivationVolume 
#   FMRIEngineClear 
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
    set Module($m,row1List) "Help Load Compute"
    set Module($m,row1Name) "{Help} {Load} {Compute}"
    set Module($m,row1,tab) Load

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
    set Module($m,depend) "CISGFile BXH"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.5 $} {$Date: 2004/05/20 17:35:03 $}]

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
    set Module(verbose) 0
    
    # Creates bindings
    FMRIEngineCreateBindings 

    # Source all appropriate tcl files here. 
    source "$FMRIEngine(modulePath)/tcl/FMRIEnginePlot.tcl"
    source "$FMRIEngine(modulePath)/tcl/FMRIEngineParadigmParser.tcl"
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
    # Load tab 
    #-------------------------------------------
    set fLoad $Module(FMRIEngine,fLoad)
    set f $fLoad

    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2  
    frame $f.fMiddle -bg $Gui(activeWorkspace) -height 280  
    frame $f.fBot -bg $Gui(activeWorkspace)   

    grid $f.fTop -row 0 -column 0 -sticky ew 
    grid $f.fMiddle -row 1 -column 0 -sticky ew 
    grid $f.fBot -row 3 -column 0 -sticky ew -pady 0 

    #------------------------------
    # Load->Bottom frame
    #------------------------------
    set f $fLoad.fBot
    frame $f.fLogos  -bg $Gui(activeWorkspace)
    pack $f.fLogos \
        -side bottom -fill x -pady $Gui(pad)

    # Load->Bottom->Logos frame
    set f $fLoad.fBot.fLogos
    set uselogo [image create photo -file \
        $FMRIEngine(modulePath)/tcl/images/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
        -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 0 -pady $Gui(pad) -expand 0

    #------------------------------
    # Load->Middle frame
    #------------------------------
    set f $fLoad.fMiddle

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
    # Load->Top frame
    #------------------------------
    set f $fLoad.fTop

    frame $f.fType   -bg $Gui(backdrop)
    pack $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    # Load->Top->Type frame
    set f $fLoad.fTop.fType

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
  
    #-------------------------------------------
    # Compute tab 
    #-------------------------------------------
    set fCompute $Module(FMRIEngine,fCompute)
    set f $fCompute

    frame $f.fFile   -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)

    pack $f.fFile $f.fApply \
        -side top -fill x -pady $Gui(pad)

    # File frame
    DevAddFileBrowse $f.fFile FMRIEngine "paradigmFileName" "Paradigm design file:" \
        "" "txt" "\$Volume(DefaultDir)" \
        "Open" "Browse for a text file" "" "Absolute" 

    # Apply frame
    set f $f.fApply
    DevAddButton $f.bApply "Compute" "FMRIEngineComputeActivationVolume" 8
    grid $f.bApply -padx $Gui(pad)
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
    
    FMRIEngineClear

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
    global FMRIEngine Gui 

    set f $parent
    frame $f.fVolume -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fSlider -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)
 
    pack $f.fVolume $f.fSlider $f.fApply \
        -side top -fill x -pady $Gui(pad)

    set f $parent.fVolume
    DevAddFileBrowse $f Volume "VolAnalyze,FileName" "Analyze Header File:" \
        "VolAnalyzeSetFileName;FMRIEngineSetVolumeName" "hdr" "\$Volume(DefaultDir)" \
        "Open" "Browse for an Analyze header file (.hdr) that has a matching .img" \
        "" "Absolute"

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
        $Gui(WSA) {-showvalue 0}

    set FMRIEngine(slider) $f.slider
    pack $f.label $f.slider -side left -expand false -fill x

    set f $parent.fApply
    DevAddButton $f.bApply "Apply" "FMRIEngineLoadVolumes" 8 
    DevAddButton $f.bClear "Cancel" "VolumesPropsCancel" 8 
    grid $f.bApply $f.bClear -padx $Gui(pad)
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

    MainSlicesSetVolumeAll Back $FMRIEngine($volumeNo,id)
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
    global FMRIEngine Volume Mrml

    $FMRIEngine(slider) set 0 
    $FMRIEngine(slider) configure -showvalue 0

    if {[info exists FMRIEngine(lastIndex)]} {
        set i 1
        while {$i <= $FMRIEngine(lastIndex)} {
            unset FMRIEngine($i,id)
            incr i
        }

        unset FMRIEngine(lastIndex)
    }

    set fileName $Volume(VolAnalyze,FileName)
    if {$fileName == ""} {
        DevErrorWindow "File name field is empty."
        return
    }

    set lastSlash [string last "/" $fileName]
    set path [string range $fileName 0 $lastSlash]
    set pattern [file join $path "*.hdr"]

    set fileList [glob -nocomplain $pattern]
    if {$fileList == ""} {
        set path $Mrml(dir)
        DevErrorWindow "Your Analyze file is not valid: $fileName"
        return
    }

    set analyzeFiles [lsort -dictionary $fileList]
    set t 1

    foreach f $analyzeFiles { 

        puts "reading $f"
        MainVolumesSetActive "NEW"
        set Volume(VolAnalyze,FileName) $f
        set FMRIEngine($t,f) $f

        set volName [VolBXHCreateVolumeNameFromFileName $f] 
        set Volume(name) $volName
        set FMRIEngine(name) $volName

        set id [VolAnalyzeApply]
        set FMRIEngine($t,id) $id


        MainVolumesSetParam Window 800
        MainVolumesSetParam Level  500
        incr t
    }
    set FMRIEngine(lastIndex) [llength $analyzeFiles] 

    # Add volumes into vtkActivationVolumeGenerator
    if {[info exists FMRIEngine(actvolgen)]} {
        $FMRIEngine(actvolgen) Delete
    }
    vtkActivationVolumeGenerator ad
    set ii 1
    while {$ii < $t} {

        set volId $FMRIEngine($ii,id)
        Volume($volId,vol) Update
        ad AddInput [Volume($volId,vol) GetOutput]
        incr ii
    }
    set FMRIEngine(actvolgen) ad

    set no [expr $ii-1]
    set ext [[Volume($no,vol) GetOutput] GetWholeExtent]
    set FMRIEngine(volextent) $ext 

    set m [expr $t-1]
    $FMRIEngine(slider) configure -from 1 -to $m
    $FMRIEngine(slider) configure -state active
    $FMRIEngine(slider) configure -showvalue 1 

    # show the first volume by default
    MainSlicesSetVolumeAll Back $FMRIEngine(1,id)
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
    set slider [$FMRIEngine(slider) get]
    
    if {$slider == 0} {
        DevErrorWindow "Please load volumes first."
        return
    }

    # Checks if paradigm file has been properly parsed 
    if {! [FMRIEngineParseParadigm]} {
        return
    }

    set stimSize [$FMRIEngine(stimulus) GetNumberOfTuples]
    set volSize [$FMRIEngine(actvolgen) GetNumberOfInputs]
    if {$stimSize != $volSize} {
        DevErrorWindow "Stimulus size ($stimSize) is not same as no of volumes ($volSize)."
        return
    }

    if {[info exists FMRIEngine(detector)]} {
        $FMRIEngine(detector) Delete
    }
    vtkActivationDetector detector
    detector SetDetectionMethod 1
    detector SetNumberOfPredictors [lindex $FMRIEngine(paradigm) 1] 
    detector SetStimulus $FMRIEngine(stimulus) 
    set FMRIEngine(detector) detector

    $FMRIEngine(actvolgen) SetDetector detector  
    $FMRIEngine(actvolgen) Update
    set id [$FMRIEngine(actvolgen) GetOutput]
    $id Update

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    $n SetName "Activation"
    $n SetDescription "Activation"

    eval Volume($i,node) SetSpacing [$id GetSpacing]
    Volume($i,node) SetScanOrder IS
    Volume($i,node) SetNumScalars [$id GetNumberOfScalarComponents]
    set ext [$id GetWholeExtent]
    Volume($i,node) SetImageRange [expr 1 + [lindex $ext 4]] [expr 1 + [lindex $ext 5]]
    Volume($i,node) SetScalarType [$id GetScalarType]
    Volume($i,node) SetDimensions [lindex [$id GetDimensions] 0] [lindex [$id GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [::Volume($i,node) GetScanOrder]

    MainUpdateMRML
    Volume($i,vol) SetImageData $id
    MainSlicesSetVolumeAll Fore $i
    MainVolumesSetActive $i
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineClear 
# Gets ready for next run 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineClear {} {
    global FMRIEngine Volume
 
    set FMRIEngine(bxh-fileName) ""
    set Volume(VolAnalyze,FileName) ""

    set slider [$FMRIEngine(slider) get] 
    if {$slider == 0} {
        return
    }

    $FMRIEngine(slider) configure -from 0 -to 0 
    $FMRIEngine(slider) configure -state disabled 
    $FMRIEngine(slider) configure -showvalue 0 

    if {[info exist FMRIEngine(lastindex)]} {
        set ii 1
        while {$ii <= $FMRIEngine(lastindex) } {
            Volume($ii,vol,rw) Delete
            incr ii
        }
    }
    
    if {[info exists FMRIEngine(lastindex)]} {
        unset FMRIEngine(lastindex)
    }
    if {[info exists FMRIEngine(actvolgen)]} {
        $FMRIEngine(actvolgen) Delete
        unset FMRIEngine(actvolgen)
    }
    if {[info exists FMRIEngine(stimulus)]} {
        $FMRIEngine(stimulus) Delete
        unset FMRIEngine(stimulus)
    }
    if {[info exists FMRIEngine(detector)]} {
        $FMRIEngine(detector) Delete
        unset FMRIEngine(detector)
    }

    MainMrmlDeleteAll
    MainUpdateMRML
    MainSetup
    RenderAll
    MainMrmlBuildTreesVersion2.0 [MainMrmlAddColors ""]  
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
