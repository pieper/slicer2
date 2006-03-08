#=auto==========================================================================
#   Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: MRAblation.tcl,v $
#   Date:      $Date: 2006/03/08 15:18:51 $
#   Version:   $Revision: 1.1.2.1 $
# 
#===============================================================================
# FILE:        MRAblation.tcl
# PROCEDURES:  
#   MRAblationInit
#   MRAblationBuildGUI
#   MRAblationUpdateDisplayTab
#   MRAblationDisplayThermalVolume
#   MRAblationCompute
#   MRAblationToggleComputing
#   MRAblationGetWorkingDir
#   MRAblationLoadVolume 
#   MRAblationCreateVolume
#   MRAblationBrowse 
#   MRAblationBuildVTK
#   MRAblationEnter
#   MRAblationExit
#   MRAblationUpdateComputeTab
#   MRAblationUpdateRealAndImaginaryImages
#   MRAblationUpdateColdAndHotImages
#   MRAblationSelectImage
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC MRAblationInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationInit {} {
    global MRAblation Module Volume Model env

    set m MRAblation

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "MR ablation."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Nobuhiko Hata, SPL/BWH, <hata@bwh.harvard.edu> &
                           Haiying Liu, SPL/BWH <hliu@bwh.harvard.edu>"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
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
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Help Load Compute Display"
    set Module($m,row1Name) "{Help} {Load} {Compute} {Display}"
    set Module($m,row1,tab) Load 

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
    #   set Module($m,procVTK) MRAblationBuildVTK
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
    set Module($m,procGUI) MRAblationBuildGUI
    set Module($m,procVTK) MRAblationBuildVTK
    set Module($m,procEnter) MRAblationEnter
    set Module($m,procExit) MRAblationExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1.2.1 $} {$Date: 2006/03/08 15:18:51 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set MRAblation(count) 0
    set MRAblation(Volume1) $Volume(idNone)
    set MRAblation(Model1)  $Model(idNone)
    set MRAblation(FileName)  ""
    set MRAblation(eventManager) ""
    set MRAblation(phaseCount) 1 
    set MRAblation(modulePath) "$env(SLICER_HOME)/Modules/vtkMRAblation"
    set MRAblation(volumeNames) ""
    set MRAblation(sequenceList) ""
    set MRAblation(updateComputeTab) 1 
 

    # Source all appropriate tcl files here. 
    source "$MRAblation(modulePath)/tcl/MRAblationHelpText.tcl"
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
# .PROC MRAblationBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationBuildGUI {} {
    global Gui MRAblation Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(MRAblation,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Stuff
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
    The MRAblation module is an example for developers.  It shows how to add a module 
    to the Slicer.  The source code is in slicer2/Modules/vtkMRAblation/tcl/MRAblation.tcl.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Stuff:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags MRAblation $help
    MainHelpBuildGUI MRAblation

    #-------------------------------------------
    # Load frame
    #-------------------------------------------
    set fLoad $Module(MRAblation,fLoad)
    set f $fLoad
    
    foreach frame "Top Middle Bottom" {
        if {$frame != "Bottom"} {
            frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 2 
        } else {
            frame $f.f$frame -bg $Gui(activeWorkspace)
        }
        pack $f.f$frame -side top -padx 10 -pady 5 -fill x
    }
    
    #----------------------------
    # Load->Top frame
    #----------------------------
    set f $fLoad.fTop
    foreach frame "ImageDir WorkingDir" {
        frame $f.f$frame -bg $Gui(activeWorkspace) 
        pack $f.f$frame -side top -pady 3 
    }

    # Image dir
    set f $fLoad.fTop.fImageDir
    foreach frame "Up Down" {
        frame $f.f$frame -bg $Gui(activeWorkspace) 
        pack $f.f$frame -side top -pady 5
    }

    set f $fLoad.fTop.fImageDir.fUp
    DevAddLabel  $f.lPlace "Where are the images?" 
    DevAddButton $f.bBrowse "Browse..." "MRAblationBrowse imageDir" 9 
    pack $f.lPlace $f.bBrowse -side left -padx 5

    set f $fLoad.fTop.fImageDir.fDown
    eval {entry $f.eDir -textvariable MRAblation(imageDir) -width 70} $Gui(WEA)
    pack $f.eDir -side left -padx 5
   
    # Working dir
    set f $fLoad.fTop.fWorkingDir
    foreach frame "Up Down" {
        frame $f.f$frame -bg $Gui(activeWorkspace) 
        pack $f.f$frame -side top -pady 5 
    }

    set f $fLoad.fTop.fWorkingDir.fUp
    DevAddLabel  $f.lPlace "Working directory?" 
    DevAddButton $f.bBrowse "Browse..." "MRAblationBrowse workingDir" 9 
    DevAddButton $f.bSame "Same" "MRAblationGetWorkingDir" 8 
    pack $f.lPlace $f.bBrowse $f.bSame -side left -padx 1 

    set f $fLoad.fTop.fWorkingDir.fDown
    eval {entry $f.eDir -textvariable MRAblation(workingDir) -width 70} $Gui(WEA)
    pack $f.eDir -side left -padx 5
 
    #---------------------------
    # Load->Middle frame
    #---------------------------
    set f $fLoad.fMiddle
    foreach frame "Title Params" {
        frame $f.f$frame -bg $Gui(activeWorkspace) 
        pack $f.f$frame -side top -pady 5
    }

    # tile
    set f $fLoad.fMiddle.fTitle
    DevAddLabel $f.l "Specify the experiment:" 
    pack $f.l -side top -pady 5 -fill x

    # parameters
    set f $fLoad.fMiddle.fParams
    DevAddLabel $f.lSeq "Types of images:"
    eval {entry $f.eSeq -textvariable MRAblation(sequence) -width 40} $Gui(WEA)
    set MRAblation(sequence) "first;second;third;fourth"

    DevAddLabel $f.lPhase "# of timepoints:"
    eval {entry $f.ePhase -textvariable MRAblation(phase) -width 40} $Gui(WEA)
    set MRAblation(phase) "10"

    DevAddLabel $f.lPlane "# of slices:"
    eval {entry $f.ePlane -textvariable MRAblation(plane) -width 40} $Gui(WEA)
    set MRAblation(plane) "10"

    blt::table $f \
       0,0 $f.lSeq   -padx 5 -pady 1 -anchor e \
       0,1 $f.eSeq   -padx 5 -pady 1 -anchor w \
       1,0 $f.lPhase -padx 5 -pady 1 -anchor e \
       1,1 $f.ePhase -padx 5 -pady 1 -anchor w \
       2,0 $f.lPlane -padx 5 -pady 1 -anchor e \
       2,1 $f.ePlane -padx 5 -pady 1 -anchor w
   
    #----------------------------
    # Load->Bottom frame
    #----------------------------
    set f $fLoad.fBottom
    DevAddButton $f.bApply "Apply" "MRAblationLoadVolume" 15 
    set MRAblation(gui,loadApplyButton) $f.bApply
    pack $f.bApply -side top -padx 8 

    #-------------------------------------------
    # Compute frame
    #-------------------------------------------
    set b $Module(MRAblation,bCompute)
    bind $b <1> "MRAblationUpdateComputeTab" 

    set fCompute $Module(MRAblation,fCompute)
    set f $fCompute
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 10 -pady 5 -fill x
    }
    
    #----------------------------
    # Compute->Top frame
    #----------------------------
    set f $fCompute.fTop
    DevAddButton $f.bHelp "?" "MRAblationHelpComputing" 2

    eval {checkbutton $f.cbComputing \
        -variable MRAblation(checkbuttonComputing) \
        -width 28 \
        -text "Automatic"} $Gui(WEA) 
    $f.cbComputing select 
    bind $f.cbComputing <1> "MRAblationToggleComputing"

    grid $f.bHelp $f.cbComputing -padx 1 -pady 5 

    #---------------------------
    # Compute->Middle frame
    #---------------------------
    set f $fCompute.fMiddle

    # Cold image
    #---------------------------
    DevAddLabel $f.lCold "Cold images:"

    set coldImgList [list {phase-1}]
    set MRAblation(coldImageCurrent) phase-1
    set df [lindex $coldImgList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 10 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    # Save menubutton for config
    set MRAblation(gui,coldImgMenuButton) $f.mbType
    set MRAblation(gui,coldImgMenu) $f.mbType.m
    $MRAblation(gui,coldImgMenuButton) config -state disabled 
 
    MRAblationSelectImage cold phase-1 

    # Hot image
    #---------------------------
    DevAddLabel $f.lHot "Hot images:"

    set hotImgList [list {phase-2}]
    set MRAblation(hotImageCurrent) phase-2
    set df [lindex $hotImgList 0] 
    eval {menubutton $f.mbType2 -text $df \
          -relief raised -bd 2 -width 10 \
          -indicatoron 1 \
          -menu $f.mbType2.m} $Gui(WMBA)
    eval {menu $f.mbType2.m} $Gui(WMA)

    # Save menubutton for config
    set MRAblation(gui,hotImgMenuButton) $f.mbType2
    set MRAblation(gui,hotImgMenu) $f.mbType2.m
    $MRAblation(gui,hotImgMenuButton) config -state disabled 

    MRAblationSelectImage hot phase-2 

    # Real image
    #---------------------------
    DevAddLabel $f.lReal "Real images:"

    set realImgList [list {none}]
    set df [lindex $realImgList 0] 
    eval {menubutton $f.mbType3 -text $df \
          -relief raised -bd 2 -width 10 \
          -indicatoron 1 \
          -menu $f.mbType3.m} $Gui(WMBA)
    eval {menu $f.mbType3.m} $Gui(WMA)
    
    # Save menubutton for config
    set MRAblation(gui,realImgMenuButton) $f.mbType3
    set MRAblation(gui,realImgMenu) $f.mbType3.m

    MRAblationSelectImage real none

    # Imaginary image
    #---------------------------
    DevAddLabel $f.lImag "Imaginary images:"

    set imagImgList [list {none}]
    set df [lindex $imagImgList 0] 
    eval {menubutton $f.mbType4 -text $df \
          -relief raised -bd 2 -width 10 \
          -indicatoron 1 \
          -menu $f.mbType4.m} $Gui(WMBA)
    eval {menu $f.mbType4.m} $Gui(WMA)

    # Save menubutton for config
    set MRAblation(gui,imaginaryImgMenuButton) $f.mbType4
    set MRAblation(gui,imaginaryImgMenu) $f.mbType4.m
 
    MRAblationSelectImage imaginary none

    # Volume prefix:
    #---------------------------
    DevAddLabel $f.lPrefix "Volume prefix:"
    eval {entry $f.ePrefix -textvariable MRAblation(volumePrefix) -width 19} $Gui(WEA)
    set MRAblation(volumePrefix) "T-"

    blt::table $f \
       0,0 $f.lCold   -padx 5 -pady 2 -anchor e \
       0,1 $f.mbType   -padx 5 -pady 2 -anchor w -fill x \
       1,0 $f.lHot -padx 5 -pady 2 -anchor e \
       1,1 $f.mbType2 -padx 5 -pady 2 -anchor w -fill x \
       2,0 $f.lReal -padx 5 -pady 2 -anchor e \
       2,1 $f.mbType3 -padx 5 -pady 2 -anchor w -fill x \
       3,0 $f.lImag -padx 5 -pady 2 -anchor e \
       3,1 $f.mbType4 -padx 5 -pady 2 -anchor w -fill x \
       4,0 $f.lPrefix -padx 5 -pady 2 -anchor e \
       4,1 $f.ePrefix -padx 5 -pady 2 -anchor w -fill x
   
    #----------------------------
    # Compute->Bottom frame
    #----------------------------
    set f $fCompute.fBottom
    DevAddButton $f.bApply "Apply" "MRAblationCompute" 15 
    set MRAblation(gui,computeApplyButton) $f.bApply 
    pack $f.bApply -side top -padx 8

    #-------------------------------------------
    # Display frame
    #-------------------------------------------

    set b $Module(MRAblation,bDisplay)
    bind $b <1> "MRAblationUpdateDisplayTab" 

    set fDisplay $Module(MRAblation,fDisplay)
    set f $fDisplay

    DevAddLabel $f.l "Choose a volume to view:"

    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set MRAblation(displayVerticalScroll) $f.vs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::MRAblation(displayVerticalScroll) set}
    set MRAblation(displayListBox) $f.lb
    $MRAblation(displayVerticalScroll) configure -command {$MRAblation(displayListBox) yview}

    DevAddButton $f.bGo "Select" "MRAblationDisplayThermalVolume" 15 
 
    blt::table $f \
        0,0 $f.l -cspan 2 -fill x -padx 1 -pady 5 \
        1,0 $MRAblation(displayListBox) -fill x -padx 1 -pady 1 \
        1,1 $MRAblation(displayVerticalScroll) -fill y -padx 1 -pady 1 \
        2,0 $f.bGo -cspan 2 -padx 1 -pady 5     
 
}


#-------------------------------------------------------------------------------
# .PROC MRAblationUpdateDisplayTab
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationUpdateDisplayTab {} {
    global MRAblation 

    $MRAblation(displayListBox) delete 0 end
    if {[info exists MRAblation(thermalVolumeNames)]} {
        set size [llength $MRAblation(thermalVolumeNames)]

        for {set i 0} {$i < $size} {incr i} {
            set name [lindex $MRAblation(thermalVolumeNames) $i] 
            if {$name != ""} {
                $MRAblation(displayListBox) insert end $name
            }
        } 
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationDisplayThermalVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationDisplayThermalVolume {} {
    global MRAblation Gui Volume

    set curs [$MRAblation(displayListBox) curselection] 
    if {$curs != ""} {
        set name [$MRAblation(displayListBox) get $curs] 

        set id [MIRIADSegmentGetVolumeByName $name] 
        set MRAblation(currentActVolID) $id
        set MRAblation(currentActVolName) $name

        MainSlicesSetVolumeAll Fore $id
        MainVolumesSetActive $id
        MainVolumesRender
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationCompute
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationCompute {} {
    global MRAblation Volume Gui

    # Error checking
    if {$MRAblation(coldImageCurrent) == $MRAblation(hotImageCurrent)} {
        DevErrorWindow "Cold image and hot image cannot be the same."
        return
    }
    if {$MRAblation(realImageCurrent) == $MRAblation(imaginaryImageCurrent)} {
        DevErrorWindow "Real image and imaginary image cannot be the same."
        return
    }


    # Cold phase number
    set i [string last "-" $MRAblation(coldImageCurrent)]
    set cold [string range $MRAblation(coldImageCurrent) [expr $i + 1] end]

    # Hot phase number(s)
    if {$MRAblation(hotImageCurrent) == "all"} {
        set hot all 
    } else {
        set i [string last "-" $MRAblation(hotImageCurrent)]
        set hot [string range $MRAblation(hotImageCurrent) [expr $i + 1] end]
    }


    # Check if images are loaded
    set pList ""
    if {$hot == "all"} {
        for {set p 1} {$p <= $MRAblation(phase)} {incr p} {
            lappend pList $p
        } 
    } else {
        lappend pList $cold
        lappend pList $hot
    }

    set iList ""
    lappend iList $MRAblation(realImageCurrent)
    lappend iList $MRAblation(imaginaryImageCurrent)
    foreach p $pList {
        foreach e $iList {
            set n laser-$e-$p
            set id [MIRIADSegmentGetVolumeByName $n] 
            if {$id <= 0} {
                DevErrorWindow "Volume doesn't exist: $n"
                return
            }
        }
    }

    $MRAblation(gui,computeApplyButton) config -state disabled 
 
    # Compute thermal volume(s)    
    set pList ""
    if {$hot == "all"} {
        for {set p 1} {$p <= $MRAblation(phase)} {incr p} {
            if {$p != $cold} {
                lappend pList $p
            }
        } 
    } else {
        lappend pList $hot
    }

    # real image of cold image
    set coldRealVolName laser-$MRAblation(realImageCurrent)-$cold
    set coldRealVolID [MIRIADSegmentGetVolumeByName $coldRealVolName] 
    set coldRealImageData [Volume($coldRealVolID,vol) GetOutput] 
 
    # imaginary image of cold image
    set coldImaginaryVolName laser-$MRAblation(imaginaryImageCurrent)-$cold
    set coldImaginaryVolID [MIRIADSegmentGetVolumeByName $coldImaginaryVolName] 
    set coldImaginaryImageData [Volume($coldImaginaryVolID,vol) GetOutput] 

    set MRAblation(thermalVolumeNames) ""
 
    foreach p $pList {
        # real image of hot image
        set hotRealVolName laser-$MRAblation(realImageCurrent)-$p
        set hotRealVolID [MIRIADSegmentGetVolumeByName $hotRealVolName] 
        set hotRealImageData [Volume($hotRealVolID,vol) GetOutput] 

        # imaginary image of hot image
        set hotImaginaryVolName laser-$MRAblation(imaginaryImageCurrent)-$p
        set hotImaginaryVolID [MIRIADSegmentGetVolumeByName $hotImaginaryVolName] 
        set hotImaginaryImageData [Volume($hotImaginaryVolID,vol) GetOutput] 
        

        # always uses a new instance of vtkImageThermalMap
        if {[info commands MRAblation(thermalMap)] != ""} {
            MRAblation(thermalMap) Delete
            unset -nocomplain MRAblation(thermalMap)
        }
        vtkImageThermalMap MRAblation(thermalMap) 

        MRAblation(thermalMap) AddInput $coldRealImageData
        MRAblation(thermalMap) AddInput $coldImaginaryImageData 
        MRAblation(thermalMap) AddInput $hotRealImageData 
        MRAblation(thermalMap) AddInput $hotImaginaryImageData 

        # adds progress bar

        set name $MRAblation(volumePrefix)c$cold-h$p
        set Gui(progressText) "Computing $name..."
        puts $Gui(progressText)

        set obs1 [MRAblation(thermalMap) AddObserver StartEvent MainStartProgress]
        set obs2 [MRAblation(thermalMap) AddObserver ProgressEvent \
            "MainShowProgress MRAblation(thermalMap)"]
        set obs3 [MRAblation(thermalMap) AddObserver EndEvent MainEndProgress]
 
        MRAblation(thermalMap) Update 
 
        # add a mrml node
        set n [MainMrmlAddNode Volume]
        set i [$n GetID]
        MainVolumesCreate $i

        # set the name and description of the volume
        $n SetName "$name" 
        lappend MRAblation(thermalVolumeNames) "$name"
        $n SetDescription "$name"

        Volume($i,vol) CopyNode Volume($coldRealVolID,vol)
        Volume($i,node) SetScalarType [[MRAblation(thermalMap) GetOutput] GetScalarType]
        Volume($i,vol) SetImageData [MRAblation(thermalMap) GetOutput]
        set scalarRange [[MRAblation(thermalMap) GetOutput] GetScalarRange]
        set low [lindex $scalarRange 0] 
        set high [lindex $scalarRange 1] 
        Volume($i,vol) SetRangeLow $low 
        Volume($i,vol) SetRangeHigh $high 

        # To keep variables 'RangeLow' and 'RangeHigh' as float 
        # in vtkMrmlDataVolume for float volume, use this function:
        Volume($i,vol) SetRangeAuto 0

        # set the lower threshold to the actLow 
        Volume($i,node) AutoThresholdOff
        Volume($i,node) ApplyThresholdOn
        Volume($i,node) SetLowerThreshold $low 
        Volume($i,node) SetUpperThreshold $high 


        MainSlicesSetVolumeAll Fore $i
        MainVolumesSetActive $i

        # set the act volume to the color of FMRI 
        MainVolumesSetParam LutID 2 

        MainUpdateMRML
        RenderAll

        set Gui(progressText) ""
        puts "...done"
        MainEndProgress


    }

    $MRAblation(gui,computeApplyButton) config -state normal 
 
}


#-------------------------------------------------------------------------------
# .PROC MRAblationToggleComputing
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationToggleComputing {} {
    global MRAblation 

    set toggle [expr {$MRAblation(checkbuttonComputing) == 1 ? 0 : 1}]
    if {$toggle} {
        $MRAblation(gui,coldImgMenuButton) config -state disabled 
        $MRAblation(gui,hotImgMenuButton) config -state disabled 

        # set cold and hot image
        set l [$MRAblation(gui,coldImgMenu) entrycget 0 -label]
        MRAblationSelectImage cold $l 

        set l [$MRAblation(gui,hotImgMenu) entrycget end -label]
        MRAblationSelectImage hot $l 
    } else {
        $MRAblation(gui,coldImgMenuButton) config -state normal 
        $MRAblation(gui,hotImgMenuButton) config -state normal 
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationGetWorkingDir
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationGetWorkingDir {} {
    global MRAblation Volume 

    set MRAblation(workingDir) $MRAblation(imageDir)
}

 
#-------------------------------------------------------------------------------
# .PROC MRAblationLoadVolume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationLoadVolume {} {
    global MRAblation

    #-------------------------------------------
    # Error checking of user input 
    #-------------------------------------------

    if {! [file exists $MRAblation(imageDir)]} {
        DevErrorWindow "The image directory doesn't exist."
        return
    }

    if {! [file exists $MRAblation(workingDir)]} {
        DevErrorWindow "The working directory doesn't exist."
        return
    }

    if {! [file writable $MRAblation(workingDir)]} {
        DevErrorWindow "The working directory is not writable."
        return
    }

    if {$MRAblation(sequenceList) == ""} {
        set MRAblation(sequenceList) [split $MRAblation(sequence) ";"]
    }
    set sequenceLength [llength $MRAblation(sequenceList)]
    if {$sequenceLength < 1} {
        DevErrorWindow "Types of images have not been specified."
        return
    }

    if {[ValidateInt $MRAblation(phase)] == 0 || $MRAblation(phase) < 2} {
        DevErrorWindow "# of timepoints must be a (> 1) integer number."
        return
    }

    if {[ValidateInt $MRAblation(plane)] == 0 || $MRAblation(plane) < 1} {
        DevErrorWindow "# of slices must be a positive integer number."
        return
    }

    set MRAblation(volumeNames) ""
    $MRAblation(gui,loadApplyButton) config -state disabled 
    set MRAblation(phaseCount) 1
    MRAblationWatch 

}


#-------------------------------------------------------------------------------
# .PROC MRAblationCreateVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationCreateVolume {c} {
    global MRAblation Volume 

    set lenOfSequence [llength $MRAblation(sequenceList)]
    set startIndex [expr ($c - 1) * $MRAblation(plane) * $lenOfSequence]
    for {set s 0} {$s < $lenOfSequence} {incr s} {

        #-------------------------------------------
        # Make a directory called mrablation-tmp
        # under the $MRAblation(workingDir).
        #-------------------------------------------
        set tmp [file join $MRAblation(workingDir) mrablation-tmp]
        if {[file exists $tmp]} {
            file delete -force $tmp
        }
        file mkdir $tmp

        #-------------------------------------------
        # For each sequence, we copy its image files
        # into mrablation-tmp and re-name them as
        # I.001, I.002, I.003, ..., which is required
        # by basic volume reader in slicer.
        #-------------------------------------------
        set index [expr $startIndex + $s]
        for {set p 0} {$p < $MRAblation(plane)} {incr p} {
            set fileName [lindex $MRAblation(imageFiles) $index]
            file copy -force $fileName $tmp

            set tail [file tail $fileName]
            set source [file join $tmp $tail]
            set ext [format "%03d" [expr $p + 1]]
            set target [file join $tmp I.$ext]

            file rename -force $source $target 

            set index [expr $index + $lenOfSequence]
        }

        #-------------------------------------------
        # Call basic reader in slicer 
        #-------------------------------------------
        set Volume(activeID) NEW
        set Volume(firstFile) [file join $tmp I.001] 
        VolumesSetFirst

        set seqName [lindex $MRAblation(sequenceList) $s]
        set Volume(name) laser-$seqName-$c

        # auto
        set Volume(readHeaders) 1 
        # Load greyscale
        set Volume(labelMap) 0

        VolumesPropsApply
        RenderAll

        lappend MRAblation(volumeNames) $Volume(name) 

        # Clean up
        if {[file exists $tmp]} {
            file delete -force $tmp
        }
    }
}
   
 
proc MRAblationWatch {} {
    global MRAblation Gui 
  
    set MRAblation(imageFiles) ""
    set MRAblation(imageFiles) [glob -nocomplain -directory $MRAblation(imageDir) I.*]
    set MRAblation(imageFiles) [lsort -ascii $MRAblation(imageFiles)]

    set numOfExistingFiles [llength $MRAblation(imageFiles)]
    set numOfRequiredFiles \
        [expr $MRAblation(phaseCount) * $MRAblation(plane) * [llength $MRAblation(sequenceList)]] 

    if {$numOfExistingFiles >= $numOfRequiredFiles} {
        MRAblationCreateVolume $MRAblation(phaseCount) 

        if {$MRAblation(phaseCount) == $MRAblation(phase)} {
            $MRAblation(gui,loadApplyButton) config -state normal 
            set MRAblation(updateComputeTab) 1 
            return
        }

        incr MRAblation(phaseCount)
    } else {
 
        set Gui(progressText) "Waiting images for phase #$MRAblation(phaseCount)..."
        puts $Gui(progressText)

    } 

    after 1000 "MRAblationWatch"
}


#-------------------------------------------------------------------------------
# .PROC MRAblationBrowse 
# Sets image directory 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationBrowse {dir} {
    global MRAblation Volume 
   
    if {[file exists $MRAblation($dir)]} {
        set initDir $MRAblation($dir)
    } else {
        set initDir $Volume(DefaultDir)
    }

    set MRAblation($dir) \
        [tk_chooseDirectory -initialdir $initDir] 

    $MRAblation(gui,loadApplyButton) config -state normal 
    after cancel "MRAblationWatch"
}

#-------------------------------------------------------------------------------
# .PROC MRAblationBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationBuildVTK {} {

}


#-------------------------------------------------------------------------------
# .PROC MRAblationEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationEnter {} {
    global MRAblation
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $MRAblation(eventManager)
}


#-------------------------------------------------------------------------------
# .PROC MRAblationExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}


#-------------------------------------------------------------------------------
# .PROC MRAblationUpdateComputeTab
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationUpdateComputeTab {} {
    global MRAblation

    if {$MRAblation(updateComputeTab)} {
        MRAblationUpdateColdAndHotImages
        MRAblationUpdateRealAndImaginaryImages

        set MRAblation(updateComputeTab) 0
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationUpdateRealAndImaginaryImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationUpdateRealAndImaginaryImages {} {
    global MRAblation

    $MRAblation(gui,realImgMenu) delete 0 end 
    $MRAblation(gui,imaginaryImgMenu) delete 0 end 
    
    if {$MRAblation(sequenceList) == ""} {
        set MRAblation(sequenceList) [split $MRAblation(sequence) ";"]
    }
    set len [llength $MRAblation(sequenceList)]
    if {$len <= 0} {
        $MRAblation(gui,realImgMenu) add command -label none \
            -command "MRAblationSelectImage real none"

        $MRAblation(gui,imaginaryImgMenu) add command -label none \
            -command "MRAblationSelectImage imaginary none"
    } else {
        foreach s $MRAblation(sequenceList) {
            $MRAblation(gui,realImgMenu) add command -label $s \
                -command "MRAblationSelectImage real $s"

            $MRAblation(gui,imaginaryImgMenu) add command -label $s \
                -command "MRAblationSelectImage imaginary $s"
        }

        MRAblationSelectImage real [lindex $MRAblation(sequenceList) 0] 
        MRAblationSelectImage imaginary [lindex $MRAblation(sequenceList) end] 
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationUpdateColdAndHotImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationUpdateColdAndHotImages {} {
    global MRAblation

    $MRAblation(gui,coldImgMenu) delete 0 end 
    $MRAblation(gui,hotImgMenu) delete 0 end 
    
    set len [llength $MRAblation(volumeNames)]
    if {$len <= 0} {
        $MRAblation(gui,coldImgMenu) add command -label phase-1 \
            -command "MRAblationSelectImage cold phase-1"

        $MRAblation(gui,hotImgMenu) add command -label phase-2 \
            -command "MRAblationSelectImage hot phase-2"
    } else {
        for {set p 1} {$p <= $MRAblation(phase)} {incr p} {
            set l phase-$p
            $MRAblation(gui,coldImgMenu) add command -label $l \
                -command "MRAblationSelectImage cold $l"

            $MRAblation(gui,hotImgMenu) add command -label $l \
                -command "MRAblationSelectImage hot $l"
        }

        # phase 1 is default for the cold image
        MRAblationSelectImage cold phase-1 

        if {$MRAblation(phase) > 2} {
            set l all
            # add "all" for hot image
            $MRAblation(gui,hotImgMenu) add command -label $l \
                -command "MRAblationSelectImage hot $l"
        }
        MRAblationSelectImage hot $l 
    }
}


#-------------------------------------------------------------------------------
# .PROC MRAblationSelectImage
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationSelectImage {type name} {
    global MRAblation

    if {$type == "hot"} {
        $MRAblation(gui,hotImgMenuButton) config -text $name
        set MRAblation(hotImageCurrent) $name 
    } elseif {$type == "cold"} {
        $MRAblation(gui,coldImgMenuButton) config -text $name
        set MRAblation(coldImageCurrent) $name 
    } elseif {$type == "real"} {
        $MRAblation(gui,realImgMenuButton) config -text $name
        set MRAblation(realImageCurrent) $name 
    } else {
        $MRAblation(gui,imaginaryImgMenuButton) config -text $name
        set MRAblation(imaginaryImageCurrent) $name 
    }
}
