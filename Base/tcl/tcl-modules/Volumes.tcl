#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        Volumes.tcl
# PROCEDURES:  
#   VolumesInit
#   VolumesBuildGUI
#   VolumesCheckForManualChanges n
#   VolumesManualSetPropertyType n
#   VolumesAutomaticSetPropertyType n
#   VolumesSetPropertyType
#   VolumesPropsApply
#   VolumesPropsCancel
#   VolumesSetFirst
#   VolumesSetScanOrder
#   VolumesSetScalarType
#   VolumesSetLast
#   VolumesEnter
#   VolumesExit
#   VolumesStorePresets
#   VolumesRecallPresets
#   VolumesSetReformatOrientation
#   VolumesProjectVectorOnPlane
#   VolumesReformatSlicePlane
#   VolumesRotateSlicePlane
#   VolumesReformatSave orientation slice
#   VolumesReformatSave
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC VolumesInit
# Default proc called on program start.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesInit {} {
    global Volumes Volume Module Gui Path prog

    # Define Tabs
    set m Volumes
    set Module($m,row1List) "Help Display Props Reformat" 
    set Module($m,row1Name) "{Help} {Display} {Props} {Reformat}"
    set Module($m,row1,tab) Display
    
    set Module($m,row2List) "Other Deface" 
    set Module($m,row2Name) "{Other} {Deface}"
    set Module($m,row2,tab) Other

    # Module Summary Info
    set Module($m,overview) "Load/display 3d volumes (grayscale or label) in the slicer."

    # Define Procedures
    set Module($m,procGUI)  VolumesBuildGUI

    # For now, never display histograms to avoid bug in histWin Render
    # call in MainVolumesSetActive. (This happened when starting slicer,
    # switching to Volumes panel, switching back to Data, and then 
    # adding 2 transforms.)
    # Windows98 Version II can't render histograms
    set Volume(histogram) On

    # Define Dependencies
    set Module($m,depend) Fiducials

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.87 $} {$Date: 2003/08/14 19:44:35 $}]

    # Props
    set Volume(propertyType) VolBasic
    # text for menus displayed on Volumes->Props->Header GUI
    set Volume(scalarTypeMenu) "Char UnsignedChar Short UnsignedShort\ 
    {Int} UnsignedInt Long UnsignedLong Float Double"
    set Volume(scanOrderMenu) "{Sagittal:LR} {Sagittal:RL} {Axial:SI}\
            {Axial:IS} {Coronal:AP} {Coronal:PA}"
    # corresponding values to use in Volume(scanOrder)
    set Volume(scanOrderList) "LR RL SI IS AP PA" 
    
    MainVolumesSetGUIDefaults

    set Volume(DefaultDir) ""

    #reformatting variable
    #---------------------------------------------
    vtkImageWriter Volumes(writer)
    vtkImageReformat Volumes(reformatter)
    Volumes(writer) SetFileDimensionality 2
    Volumes(writer) SetStartMethod      MainStartProgress
    Volumes(writer) SetProgressMethod  "MainShowProgress Volumes(writer)"
    Volumes(writer) SetEndMethod        MainEndProgress

    set Volumes(prefixSave) ""


    # Submodules for reading various volume types
    #---------------------------------------------

    # Find all tcl files in subdirectory and source them
    set dir [file join tcl-modules Volumes]

    # save the already loaded volume readers (from modules)
    set cmds [info command Vol*Init]

    set Volume(readerModules,idList) [DevSourceTclFilesInDirectory $dir]

    # get the init functions from the modules, skipping base tcl file init procedures
    foreach c $cmds {
        if {$Module(verbose) == 1} {puts "vol-init = $c"}
        if {$c != "VolumesInit" && $c != "VolumeMathInit" && $c != "VolRendInit"} {
            scan $c "%\[^I\]sInit" name
            lappend Volume(readerModules,idList) $name
        }
    }

    # Call each submodule's init function if it exists
    foreach m $Volume(readerModules,idList) {
        if {[info command ${m}Init] != ""} {
            ${m}Init
        }
    }

    if {0} {
        # register sub modules color functions if they exist
        foreach m $Volume(readerModules,idList) {
            if {[info exists Volume(readerModules,$m,procColor)] == 1} {
                lappend colorProcs $Volume(readerModules,$m,procColor)
            }
        }
        puts "Volumes.tcl registering colour procedures: $colorProcs"
        set Module(Volumes,procColor) $colorProcs
    }

    # Legacy things specific to submodules 
    #---------------------------------------------
    # Added by Attila Tanacs 10/18/2000
    set Module($m,procEnter) VolumesEnter
    set Module($m,procExit) VolumesExit
    lappend Module(procStorePresets) VolumesStorePresets
    lappend Module(procRecallPresets) VolumesRecallPresets
    set Volumes(eventManager) {}
    # End

}

#-------------------------------------------------------------------------------
# .PROC VolumesBuildGUI
# Builds volumes module's GUI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesBuildGUI {} {
    global Gui Slice Volume Lut Module Volumes Fiducials Path

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    # Props
    #   Top
    #     Active
    #     Type
    #   Bot
    #     Basic
    #     Header
    # Other
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
Description by tab:
<P>
<UL>
<LI><B>Display:</B> Set the volume you wish to change as the 
<B>Active Volume</B>.  Then adjust these:
<BR><B>Window/Level:</B> sets which pixel values will have the most
visible range of color values.
<BR><B>Threshold:</B> pixels that do not satisfy the threshold appear
transparent on the reformatted slices.  Use this to clip away occluding
slice planes.
<BR><B>Palette:</B> select the color scheme for the data. Overlay colored
functional data on graylevel anatomical data.
<BR><B>Interpolate:</B> indicate whether to interpolate between voxels
on the reformatted slices.
<BR><LI><B>Props</B> set the header information
<BR><LI><B>Other</B> The <B>Slider Range</B> for the Window/Level/Threshold
sliders is normally automatically set according to the min and max voxel 
values in the volume.  However, some applications, such as monitoring 
thermal surgery, requires setting these manually to retain the same color 
scheme as the volume's data changes over time due to realtime data
acquisition.
<BR><LI><B> Reformat: </B>
<BR> You can reformat any 3 slice in any arbitrary orientation or define a new axial,sagittal or coronal orientation.
<BR> To do that, create and select 3 fiducials that will define the new
orientation plane of the slice (To see how to create/select Fiducials, press the 'How do I create Fiducials?' button when you are in the reformat panel)
<BR>Once 3 Fiducials are selected, press the 'reformat plane' button and the active slice will now have the new orientation.
<BR> If you would like to also define a rotation around the plane normal, create and select 2 fiducials that will define the alignment of the plane and press the 'rotate plane' button.
<BR> 
<BR> If you would like to save a reformatted volume: just select a volume, select the scan order and a location and click save. What this does is it saves new volume files that were created by 'slicing' the original volume with the plane defined in the scan order menu.

 "
    regsub -all "\n" $help { } help
    MainHelpApplyTags Volumes $help
    MainHelpBuildGUI  Volumes

    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Volumes,fDisplay)
    set f $fDisplay

    # Frames
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fWinLvl    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fThresh    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fHistogram -bg $Gui(activeWorkspace)
    frame $f.fInterpolate -bg $Gui(activeWorkspace)
    pack $f.fActive $f.fWinLvl $f.fThresh $f.fHistogram $f.fInterpolate \
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Display->Active frame
    #-------------------------------------------
    set f $fDisplay.fActive

    eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m


    #-------------------------------------------
    # Display->WinLvl frame
    #-------------------------------------------
    set f $fDisplay.fWinLvl

    frame $f.fAuto    -bg $Gui(activeWorkspace)
    frame $f.fSliders -bg $Gui(activeWorkspace)
    pack $f.fAuto $f.fSliders -side top -fill x -expand 1

    #-------------------------------------------
    # Display->WinLvl->Auto frame
    #-------------------------------------------
    set f $fDisplay.fWinLvl.fAuto

    DevAddLabel $f.lAuto "Window/Level:"
    frame $f.fAuto -bg $Gui(activeWorkspace)
    pack $f.lAuto $f.fAuto -side left -padx $Gui(pad)  -pady $Gui(pad) -fill x

    foreach value "1 0" text "Auto Manual" width "5 7" {
        eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(autoWindowLevel) \
            -command "MainVolumesSetParam AutoWindowLevel; MainVolumesRender"} $Gui(WCA)
        pack $f.fAuto.rAuto$value -side left -fill x
    }

    #-------------------------------------------
    # Display->WinLvl->Sliders frame
    #-------------------------------------------
    set f $fDisplay.fWinLvl.fSliders

    foreach slider "Window Level" text "Win Lev" {
        DevAddLabel $f.l${slider} "$text:"
        eval {entry $f.e${slider} -width 6 \
            -textvariable Volume([Uncap ${slider}])} $Gui(WEA)
        bind $f.e${slider} <Return>   \
            "MainVolumesSetParam ${slider}; MainVolumesRender"
        bind $f.e${slider} <FocusOut> \
            "MainVolumesSetParam ${slider}; MainVolumesRender"
        eval {scale $f.s${slider} -from 1 -to 700 -length 140\
            -variable Volume([Uncap ${slider}])  -resolution 1 \
            -command "MainVolumesSetParam ${slider}; MainVolumesRenderActive"} \
             $Gui(WSA) {-sliderlength 14}
        bind $f.s${slider} <Leave> "MainVolumesRender"
        grid $f.l${slider} $f.e${slider} $f.s${slider} -padx 2 -pady $Gui(pad) \
            -sticky news
    }
    # Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
    lappend Volume(sWindowList) $f.sWindow
    lappend Volume(sLevelList)  $f.sLevel

    
    #-------------------------------------------
    # Display->Thresh frame
    #-------------------------------------------
    set f $fDisplay.fThresh

    frame $f.fAuto    -bg $Gui(activeWorkspace)
    frame $f.fSliders -bg $Gui(activeWorkspace)
    pack $f.fAuto $f.fSliders -side top  -fill x -expand 1

    #-------------------------------------------
    # Display->Thresh->Auto frame
    #-------------------------------------------
    set f $fDisplay.fThresh.fAuto

    DevAddLabel $f.lAuto "Threshold: "
    frame $f.fAuto -bg $Gui(activeWorkspace)
    pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -fill x

    foreach value "1 0" text "Auto Manual" width "5 7" {
        eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(autoThreshold) \
            -command "MainVolumesSetParam AutoThreshold; MainVolumesRender"} $Gui(WCA)
        pack $f.fAuto.rAuto$value -side left -fill x
    }
    eval {checkbutton $f.cApply \
        -text "Apply" -variable Volume(applyThreshold) \
        -command "MainVolumesSetParam ApplyThreshold; MainVolumesRender" -width 6 \
        -indicatoron 0} $Gui(WCA)
    pack $f.cApply -side left -padx $Gui(pad)

    #-------------------------------------------
    # Display->Thresh->Sliders frame
    #-------------------------------------------
    set f $fDisplay.fThresh.fSliders

    foreach slider "Lower Upper" text "Lo Hi" {
        DevAddLabel $f.l${slider} "$text:"
        eval {entry $f.e${slider} -width 6 \
            -textvariable Volume([Uncap ${slider}]Threshold)} $Gui(WEA)
            bind $f.e${slider} <Return>   \
                "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
            bind $f.e${slider} <FocusOut> \
                "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
        eval {scale $f.s${slider} -from 1 -to 700 -length 140 \
            -variable Volume([Uncap ${slider}]Threshold)  -resolution 1 \
            -command "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"} \
            $Gui(WSA) {-sliderlength 14}
        grid $f.l${slider} $f.e${slider} $f.s${slider} -padx 2 -pady $Gui(pad) \
            -sticky news
    }
    # Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
    lappend Volume(sLevelList) $f.sLower
    lappend Volume(sLevelList) $f.sUpper


    #-------------------------------------------
    # Display->Histogram frame
    #-------------------------------------------
    set f $fDisplay.fHistogram

    frame $f.fHistBorder -bg $Gui(activeWorkspace) -relief sunken -bd 2
    frame $f.fLut -bg $Gui(activeWorkspace)
    pack $f.fLut $f.fHistBorder -side left -padx $Gui(pad) -pady $Gui(pad)
    
    #-------------------------------------------
    # Display->Histogram->Lut frame
    #-------------------------------------------
    set f $fDisplay.fHistogram.fLut

    DevAddLabel $f.lLUT "Palette:"
    eval {menubutton $f.mbLUT \
        -text "$Lut([lindex $Lut(idList) 0],name)" \
            -relief raised -bd 2 -width 9 \
        -menu $f.mbLUT.menu} $Gui(WMBA)
        eval {menu $f.mbLUT.menu} $Gui(WMA)
        # Add menu items
        foreach l $Lut(idList) {
            $f.mbLUT.menu add command -label $Lut($l,name) \
                -command "MainVolumesSetParam LutID $l; MainVolumesRender"
        }
        set Volume(mbLUT) $f.mbLUT

    pack $f.lLUT $f.mbLUT -pady $Gui(pad) -side top

    #-------------------------------------------
    # Display->Histogram->HistBorder frame
    #-------------------------------------------
    set f $fDisplay.fHistogram.fHistBorder

    if {$Volume(histogram) == "On"} {
        MakeVTKImageWindow hist
        histMapper SetInput [Volume(0,vol) GetHistogramPlot]

        vtkTkRenderWidget $f.fHist -rw histWin \
            -width $Volume(histWidth) -height $Volume(histHeight)  
        bind $f.fHist <Expose> {ExposeTkImageViewer %W %x %y %w %h}
        pack $f.fHist
    }

    #-------------------------------------------
    # Display->Interpolate frame
    #-------------------------------------------
    set f $fDisplay.fInterpolate

    DevAddLabel $f.lInterpolate "Interpolation:"
    pack $f.lInterpolate -pady $Gui(pad) -padx $Gui(pad) -side left -fill x

    foreach value "1 0" text "On Off" width "4 4" {
        eval {radiobutton $f.rInterp$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(interpolate) \
            -command "MainVolumesSetParam Interpolate; MainVolumesRender"} $Gui(WCA)
        pack $f.rInterp$value -side left -fill x
    }


    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(Volumes,fProps)
    set f $fProps

    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 310
    pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Props->Bot frame
    #-------------------------------------------
    set f $fProps.fBot

    # Make a frame for each reader submodule
    foreach m $Volume(readerModules,idList) {
        frame $f.f${m} -bg $Gui(activeWorkspace)
        place $f.f${m} -in $f -relheight 1.0 -relwidth 1.0

        set Volume(f$m) $f.f${m}
    }
    # raise the default one 
    raise $Volume(fVolBasic)

    #-------------------------------------------
    # Props->Top frame
    #-------------------------------------------
    set f $fProps.fTop

    frame $f.fActive -bg $Gui(backdrop)
    frame $f.fType   -bg $Gui(backdrop)
    pack $f.fActive $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Props->Top->Active frame
    #-------------------------------------------
    set f $fProps.fTop.fActive

    eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Props->Top->Type frame
    #-------------------------------------------
    set f $fProps.fTop.fType

    # Build pulldown menu for volume properties
    eval {label $f.l -text "Properties:"} $Gui(BLA)
    frame $f.f -bg $Gui(backdrop)
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    eval {menubutton $f.mbType -text \
            $Volume(readerModules,$Volume(propertyType),name) \
            -relief raised -bd 2 -width 20 \
            -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach m $Volume(readerModules,idList)  {
        $f.mbType.m add command -label $Volume(readerModules,$m,name) \
                -command "VolumesSetPropertyType $m"
    }
    # save menubutton for config
    set Volume(gui,mbPropertyType) $f.mbType
    # put a tooltip over the menu
    TooltipAdd $f.mbType \
            "Choose the type of file information to display."

    #-------------------------------------------
    # Various Reader types frames
    #-------------------------------------------
    
    foreach m $Volume(readerModules,idList) {
        # If it has a procedure for building the GUI
        if {[info command $Volume(readerModules,$m,procGUI)] != ""} {
            # then call it
            if {$Module(verbose) == 1} {
                puts "VolumesBuildGUI calling: $Volume(readerModules,$m,procGUI)"
            }
            $Volume(readerModules,$m,procGUI) $Volume(f$m)
        }
    }
        
    #-------------------------------------------
    # Reformat frame
    #-------------------------------------------
    set fReformat $Module(Volumes,fReformat)
    set f $fReformat

    # binding stuff to create a fiducials list
    
    
    
    foreach frame "Active ReOrient Save" {
        set f $fReformat.f$frame
        frame $f -bg $Gui(activeWorkspace) -relief groove -bd 3
        pack $f -side top -pady 0
    }
    
    set f $fReformat.fActive

    eval {label $f.lActive -text "Active Slice:"} $Gui(WLA)
    pack $f.lActive -side left -pady $Gui(pad) -padx $Gui(pad) -fill x
    
    foreach s $Slice(idList) text "Red Yellow Green" width "4 7 6" {
        eval {radiobutton $f.r$s -width $width -indicatoron 0\
            -text "$text" -value "$s" -variable Slice(activeID) \
            -command "MainSlicesSetActive"} $Gui(WCA) {-selectcolor $Gui(slice$s)}
        pack $f.r$s -side left -fill x -anchor e
    }

    set f $fReformat.fReOrient
    foreach frame "top middle1 middle2 bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace) 
        pack $f.f$frame -side top -pady 1
    }
    set f $fReformat.fReOrient.ftop


    eval {label $f.lintro -text "You can reformat the active slice by using fiducial points: " -wraplength 180} $Gui(WLA)
    eval {button $f.bintro -text "How ?" } $Gui(WBA)
    TooltipAdd $f.bintro "To reformat the volume, you need to specify which orientation you are going to re-define: ReformatAxial, ReformatSagittal, ReformatCoronal or newOrient. 

For the first three orientations (axial,sagittal,coronal), once you defined the new orientation, the other 2 orientation planes are automatically computed to be orthogonal to the plane you re-defined. 
To see the 3 slices in their Reformat orientation, select 'ReformatAxiSagCor' on the dropdown menu 'Or:' on one of the slice panel.  

The last orientation (NewOrient) does not have any effect on any other orientation and each slice can have an arbitray NewOrient orientation. 
To see the active slice in its NewOrient orientation, select 'NewOrient' on the dropdown menu of orientations for that slice.
 
    To define a new plane orientation, you need to:
    1. select the orientation that you want to redefine with the drop down menu
    2. create and select 3 fiducials and then press the 'reformat plane' button
    => you have now defined a new orientation for your volume
    3. to define a new RL line for the axial or coronal, or a new PA line for the sagittal or newOrient orientation, 
you need to create and select 2 fiducials and then press the 'define new axis' button"
    
    pack $f.lintro $f.bintro -side left -padx 0 -pady $Gui(pad)
    
    set f $fReformat.fReOrient.fmiddle1 
    eval {label $f.lOr -text "Redefine plane:"} $Gui(WLA)
    
    
    set Volumes(reformat,orientation) "ReformatSagittal"
    set Volumes(reformat,ReformatSagittalAxis) "PA"
    set Volumes(reformat,ReformatAxialAxis) "RL"
    set Volumes(reformat,ReformatCoronalAxis) "RL"
    set Volumes(reformat,NewOrientAxis) "PA"


    
    eval {menubutton $f.mbActive -text "CHOOSE" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    set Volumes(reformat,orMenu) $f.mbActive.m
    set Volumes(reformat,orMenuB) $f.mbActive
    pack $f.lOr  $f.mbActive -side left -padx $Gui(pad) -pady 0 
    # Append widgets to list that gets refreshed during UpdateMRML
    $Volumes(reformat,orMenu) add command -label "ReformatAxial" \
        -command "VolumesSetReformatOrientation ReformatAxial"
    $Volumes(reformat,orMenu) add command -label "ReformatSagittal" \
        -command "VolumesSetReformatOrientation ReformatSagittal"
    $Volumes(reformat,orMenu) add command -label "ReformatCoronal" \
        -command "VolumesSetReformatOrientation ReformatCoronal"
    $Volumes(reformat,orMenu) add command -label "NewOrient" \
        -command "VolumesSetReformatOrientation NewOrient"
    
   
 
    set f $fReformat.fReOrient.fmiddle2
    FiducialsAddActiveListFrame $f 7 25 reformat
    
    set f $fReformat.fReOrient.fbottom
    eval {button $f.bref -text "$Volumes(reformat,orientation) Plane" -command "VolumesReformatSlicePlane $Volumes(reformat,orientation)"} $Gui(WBA)
    eval {button $f.brot -text "Define new $Volumes(reformat,$Volumes(reformat,orientation)Axis) axis" -command "VolumesRotateSlicePlane $Volumes(reformat,orientation)"} $Gui(WBA)
    pack $f.bref $f.brot -side left -padx $Gui(pad)
    set Volumes(reformat,planeButton)  $f.bref
    set Volumes(reformat,axisButton)  $f.brot

    set f $fReformat.fSave.fChoose
    frame $f
    pack $f

    # Volume menu
    DevAddSelectButton  Volume $f VolumeSelect "Choose Volume:" Pack \
        "Volume to save." 14

    # bind menubutton to update stuff when volume changes.
    bindtags $Volume(mVolumeSelect) [list Menu \
        $Volume(mVolumeSelect) all]
    
    # Append menu and button to lists that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbVolumeSelect
    lappend Volume(mActiveList) $f.mbVolumeSelect.m
    
    

    set f $fReformat.fSave.fScanOrder
    frame $f
    pack $f
    
    eval {label $f.lOrient -text "Choose a scan order:"} $Gui(WLA)
    
    # This slice
    eval {menubutton $f.mbOrient -text CHOOSE -menu $f.mbOrient.m \
        -width 13} $Gui(WMBA)
    pack $f.lOrient $f.mbOrient -side left -pady 0 -padx 2 -fill x

    # Choose scan order to save it in
    eval {menu $f.mbOrient.m} $Gui(WMA)
    foreach item "[Slicer GetOrientList]" {
    $f.mbOrient.m add command -label $item -command \
        "set Volumes(reformat,saveOrder) $item; $f.mbOrient config -text $item"
    }
    set Volumes(reformat,saveMenu) $f.mbOrient.m 

    #-------------------------------------------
    # Volumes->TabbedFrame->File->Vol->Prefix
    #-------------------------------------------

    set f $fReformat.fSave.fPrefix
    frame $f
    pack $f
    
    eval {label $f.l -text "Filename Prefix:"} $Gui(WLA)
    eval {entry $f.e -textvariable Volume(prefixSave)} $Gui(WEA)
    TooltipAdd $f.e "To save the Volume, enter the prefix here or just click Save."
    pack $f.l -padx 3 -side left
    pack $f.e -padx 3 -side left -expand 1 -fill x
    
    #-------------------------------------------
    # Volumes->TabbedFrame->File->Vol->Btns
    #-------------------------------------------
    set f $fReformat.fSave.fSave
    frame $f
    pack $f
    
    eval {button $f.bWrite -text "Save" -width 5 \
        -command "VolumesReformatSave"} $Gui(WBA)
    TooltipAdd $f.bWrite "Save the Volume."
    pack $f.bWrite
    
    #-------------------------------------------
    # Other frame
    #-------------------------------------------
    set fOther $Module(Volumes,fOther)
    set f $fOther

    # Frames
    frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fRange  -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.fRange  -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Other->Active frame
    #-------------------------------------------
    set f $fOther.fActive

    eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m


    #-------------------------------------------
    # Other->Range frame
    #-------------------------------------------
    set f $fOther.fRange

    frame $f.fAuto    -bg $Gui(activeWorkspace)
    frame $f.fSliders -bg $Gui(activeWorkspace)
    pack $f.fAuto -pady $Gui(pad) -side top -fill x -expand 1
    pack $f.fSliders -side top -fill x -expand 1

    #-------------------------------------------
    # Other->Range->Auto frame
    #-------------------------------------------
    set f $fOther.fRange.fAuto

    eval {label $f.lAuto -text "Slider Range:"} $Gui(WLA)
    frame $f.fAuto -bg $Gui(activeWorkspace)
    pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach value "1 0" text "Auto Manual" width "5 7" {
        eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable Volume(rangeAuto) \
            -command "MainVolumesSetParam RangeAuto; MainVolumesRender"} $Gui(WCA)
        pack $f.fAuto.rAuto$value -side left -fill x
    }

    #-------------------------------------------
    # Other->Range->Sliders frame
    #-------------------------------------------
    set f $fOther.fRange.fSliders

    foreach slider "Low High" {
        eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
        eval {entry $f.e${slider} -width 7 \
            -textvariable Volume(range${slider})} $Gui(WEA)
        bind $f.e${slider} <Return>   \
            "MainVolumesSetParam Range${slider}; MainVolumesRender"
        bind $f.e${slider} <FocusOut> \
            "MainVolumesSetParam Range${slider}; MainVolumesRender"
        grid $f.l${slider} $f.e${slider}  -padx 2 -pady $Gui(pad) -sticky w
    }
# DDD1
    #-------------------------------------------
    # Deface frame
    #-------------------------------------------
    set fDeface $Module(Volumes,fDeface)
    set f $fDeface
    # Frames
    frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2 -height 20
    frame $f.fRange  -bg $Gui(activeWorkspace) -relief flat -bd 3

    pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.fRange  -side top -pady $Gui(pad) -padx $Gui(pad) -fill x



    #-------------------------------------------
    # Deface->Active frame
    #-------------------------------------------
    set f $fDeface.fActive

    eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Deface->Range frame
    #-------------------------------------------
    set f $fDeface.fRange

    eval {button $f.select -text "Select Images" -width 20 -command "DefaceSelectMain"} $Gui(WBA)
    
    pack $f.select -pady $Gui(pad) -side top -fill y -expand 1

    #bind $f.fAuto <Enter>  "puts {DYW: fAuto }"
    #bind $f.fSliders <Enter> "puts {DYW: fSliders}"

# DDD2 
}
    

#-------------------------------------------------------------------------------
# .PROC VolumesCheckForManualChanges
# 
# This Procedure is called to see if any important properties
# were changed that might require re-reading the volume.
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesCheckForManualChanges {n} {
    global Lut Volume Label Module Mrml

    if { !$Volume(isDICOM)} {
        if {[$n GetFilePrefix] != [file root $Volume(firstFile)] } { 
            return 1 
        }
        set firstNum [MainFileFindImageNumber First [file join $Mrml(dir) $Volume(firstFile)]]
        if {[lindex [$n GetImageRange] 0 ]  != $firstNum }  { return 1 }
        if {[lindex [$n GetImageRange] 1 ]  != $Volume(lastNum) } { return 1 }
        if {[$n GetLabelMap] != $Volume(labelMap)} { return 1 }
        if {[$n GetFilePattern] != $Volume(filePattern) } { return 1 }
        if {[lindex [$n GetDimensions] 1 ]  != $Volume(height) } { return 1 }
        if {[lindex [$n GetDimensions] 0 ]  != $Volume(width) } { return 1 }
        if {[$n GetScanOrder] != $Volume(scanOrder)} { return 1 }
        if {[$n GetScalarTypeAsString] != $Volume(scalarType)} { return 1 }
        if {[$n GetNumScalars] != $Volume(numScalars)} { return 1 }
        if {[$n GetLittleEndian] != $Volume(littleEndian)} { return 1 }
    }
    return 0
}

#-------------------------------------------------------------------------------
# .PROC VolumesManualSetPropertyType
# 
# Sets all necessary info into a vtkMrmlVolumeNode.
#
# This procedure is called when manually setting the properties
# to read in a volume
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesManualSetPropertyType {n} {
    global Lut Volume Label Module Mrml

    # These get set down below, but we need them before MainUpdateMRML
    # parse out the filename
    set parsing [MainFileParseImageFile $Volume(firstFile) 0]

#    $n SetFilePrefix [file root $Volume(firstFile)]
    puts "Volumes.tcl: VolumesManualSetPropertyType: setting file prefix to [lindex $parsing 1]"
    $n SetFilePrefix [lindex $parsing 1]

    # this check should be obsolete now
    if {[$n GetFilePrefix] == $Volume(firstFile)} {
        # file root didn't work in this case, trim the right hand numerals
        set tmpPrefix [string trimright $Volume(firstFile) 0123456789]
        # now take the assumed single separater character off of the end as well (works with more than one instance, ie if have --)
        if {$Module(verbose)} {
            puts "Volumes.tcl: VolumesManualSetPropertyType: setting file prefix to  [string trimright $tmpPrefix [string index $tmpPrefix end]]"
        }
        $n SetFilePrefix [string trimright $tmpPrefix [string index $tmpPrefix end]]
    }
#    $n SetFilePattern $Volume(filePattern)
    $n SetFilePattern [lindex $parsing 0]

    if {$::Module(verbose)} {
        puts "VolumesManualSetPropertyType: setting full prefix from mrml dir ($Mrml(dir)) and file prefix ([$n GetFilePrefix]) to [file join $Mrml(dir) [$n GetFilePrefix]]"
    }
    $n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
    if { !$Volume(isDICOM) } {
        set firstNum [MainFileFindImageNumber First [file join $Mrml(dir) $Volume(firstFile)]]
    } else {
        set firstNum 1
    }
    # can get this from the parsed out file name
#    set firstNum [lindex $parsing 2]
    set filePostfix [lindex $parsing 3]

    $n SetImageRange $firstNum $Volume(lastNum)
    $n SetDimensions $Volume(width) $Volume(height)
    eval $n SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    $n SetScalarTypeTo$Volume(scalarType)
    $n SetNumScalars $Volume(numScalars)
    $n SetLittleEndian $Volume(littleEndian)
    $n SetTilt $Volume(gantryDetectorTilt)
    $n ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # added by odonnell for DTI data: will move to submodule
    if {$Volume(tensors,DTIdata) == 1} {
        #$n UseFrequencyPhaseSwapOn
        $n SetFrequencyPhaseSwap $Volume(tensors,pfSwap)
        # recompute 
        $n ComputeRasToIjkFromScanOrder $Volume(scanOrder)
    }
}


#-------------------------------------------------------------------------------
# .PROC VolumesAutomaticSetPropertyType
# 
# Sets all necessary info into a vtkMrmlVolumeNode.
#
# This procedure is called when reading the header of a volume
# to get the header information. Returns 1 on success
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesAutomaticSetPropertyType {n} {
    global Lut Volume Label Module Mrml

    set errmsg [GetHeaderInfo [file join $Mrml(dir) $Volume(firstFile)] \
            $Volume(lastNum) $n 1]
    if {$errmsg == "-1"} {
        set msg "No header information found. Please enter header info manually."
        puts $msg
        tk_messageBox -message $msg
        # set readHeaders to manual
        set Volume(readHeaders) 0
        # switch to vols->props->header frame
        VolumesSetPropertyType VolHeader
        # Remove node
        MainMrmlUndoAddNode Volume $n
        return 0
    } elseif {$errmsg != ""} {
        # File not found, most likely
        puts $errmsg
        tk_messageBox -message $errmsg
        # Remove node
        MainMrmlUndoAddNode Volume $n
        return 0
    }
   return 1
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetPropertyType
# Switch the visible volumes->props GUI.  Either
# Basic, Header, or DICOM
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetPropertyType {type} {
    global Volume
    
    set Volume(propertyType) $type

    # configure menubutton
    set name $Volume(readerModules,$Volume(propertyType),name)
    $Volume(gui,mbPropertyType) config -text $name

    raise $Volume(f$Volume(propertyType))
    focus $Volume(f$Volume(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC VolumesPropsApply
# Called from Volumes->Props GUI's apply button.
# Updates volume properties and calls update MRML.
# If volume is NEW, causes volume to be read in.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesPropsApply {} {
    global Lut Volume Label Module Mrml View


    set m $Volume(activeID)
    if {$m == ""} {
        DevErrorWindow "VolumesPropsApply: no active volume"
        return
    }
    if {$::Module(verbose)} {
        puts "VolumesPropsApply: active id = $m"
    }

    set Volume(isDICOM) [expr [llength $Volume(dICOMFileList)] > 0]
        
    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    # first file
    if {[file exists $Volume(firstFile)] == 0} {
        tk_messageBox -message "The first file must exist, if you haven't saved a newly created volume, please press cancel and then go to the Editor Module, Volumes tab, Save button"
        return
    }

    # lastNum
    if { $Volume(isDICOM) == 0 } {
        if {[ValidateInt $Volume(lastNum)] == 0} {
            tk_messageBox -message "The last number must be an integer."
            return
        }
    }
    # resolution
    if {[ValidateInt $Volume(width)] == 0} {
        tk_messageBox -message "The width must be an integer."
        return
    }
    if {[ValidateInt $Volume(height)] == 0} {
        tk_messageBox -message "The height must be an integer."
        return
    }
    # pixel size
    if {[ValidateFloat $Volume(pixelWidth)] == 0} {
        tk_messageBox -message "The pixel size must be a number."
        return
    }
    if {[ValidateFloat $Volume(pixelHeight)] == 0} {
        tk_messageBox -message "The pixel size must be a number."
        return
    }

    # slice thickness
    if {[ValidateFloat $Volume(sliceThickness)] == 0} {
        tk_messageBox -message "The slice thickness must be a number."
        return
    }
    # slice spacing
    if {[ValidateFloat $Volume(sliceSpacing)] == 0} {
        tk_messageBox -message "The slice spacing must be a number."
        return
    }
    # tilt
    if {[ValidateFloat $Volume(gantryDetectorTilt)] == 0} {
        tk_messageBox -message "The tilt must be a number."
        return
    }
    # num scalars
    if {[ValidateInt $Volume(numScalars)] == 0} {
        tk_messageBox -message "The number of scalars must be an integer."
        return
    }

    if { $Volume(isDICOM) } {
        set Volume(readHeaders) 0
    }

    # Manual headers
    if {$Volume(readHeaders) == "0"} {
        # if on basic frame, switch to header frame.
        if {$Volume(propertyType) != "VolHeader"} {
            VolumesSetPropertyType VolHeader
            return
        }
    }

    # if the volume is NEW we may read it in...
    if {$m == "NEW"} {

        # add a MRML node for this volume (so that in UpdateMRML we can read it in according to the path, etc. in the node)
        set n [MainMrmlAddNode Volume]
        set newID [$n GetID]
           
        # Added by Attila Tanacs 10/11/2000 1/4/02

        $n DeleteDICOMFileNames
        $n DeleteDICOMMultiFrameOffsets
        for  {set j 0} {$j < [llength $Volume(dICOMFileList)]} {incr j} {
            $n AddDICOMFileName [$Volume(dICOMFileListbox) get $j]
        }
            
        if { $Volume(isDICOM) } {
            #$Volume(dICOMFileListbox) insert 0 [$n GetNumberOfDICOMFiles];
            set firstNum 1
            if {$Volume(DICOMMultiFrameFile) == "0"} {
                set Volume(lastNum) [llength $Volume(dICOMFileList)]
            } else {
                set Volume(lastNum) $Volume(DICOMMultiFrameFile)
                for {set j 0} {$j < $Volume(lastNum)} {incr j} {
                    $n AddDICOMMultiFrameOffset [lindex $Volume(DICOMSliceOffsets) $j]
                }
            }
        }        

        # End of Part added by Attila Tanacs

        # Fill in header information for reading the volume
        # Manual headers
        if {$Volume(readHeaders) == "0"} {
            # These setting are set down below, 
            # but we need them before MainUpdateMRML
            
            VolumesManualSetPropertyType $n
        } else {
            # Read headers
            if {[VolumesAutomaticSetPropertyType $n] == 0} {
                return
            }
        }
        # end Read Headers

        $n SetName $Volume(name)
        $n SetDescription $Volume(desc)
        $n SetLabelMap $Volume(labelMap)

        MainUpdateMRML
        # If failed, then it's no longer in the idList
        if {[lsearch $Volume(idList) $newID] == -1} {
            return
        }

        # allow use of other module GUIs
        set Volume(freeze) 0

        # set active volume on all menus
        MainVolumesSetActive $newID

        # save the ID for later in this proc
        set m $newID

        # if we are successful set the FOV for correct display of this volume
        # (check all dimensions and pix max - special cases for z for dicom, 
        # but since GE files haven't been parsed yet, no way to know their 
        # z extent yet.  TODO: fix GE z extent parsing)
        set fov 0
        for {set i 0} {$i < 2} {incr i} {
            set dim     [lindex [Volume($newID,node) GetDimensions] $i]
            set spacing [lindex [Volume($newID,node) GetSpacing] $i]
            set newfov     [expr $dim * $spacing]
            if { $newfov > $fov } {
                set fov $newfov
            }
        }
        set dim [llength $Volume(dICOMFileList)]
        if { $dim == 0 } {
            # do nothing for non-dicom because size isn't known yet
        } else {
            set spacing [lindex [Volume($newID,node) GetSpacing] 2]
            set newfov [expr $dim * $spacing]
            if { $newfov > $fov } {
                set fov $newfov
            }
        }
        set View(fov) $fov
        MainViewSetFov

        # display the new volume in the background of all slices
        MainSlicesSetVolumeAll Back $newID
    } else {
        # End   if the Volume is NEW
        ## Maybe we would like to do a reread of the file?
        if {$m != $Volume(idNone) } {
            if {[VolumesCheckForManualChanges Volume($m,node)] == 1} {
                set ReRead [ DevYesNo "Reread the Image?" ]
                puts "ReRead"
                if {$ReRead == "yes"} {
                    set Volume(readHeaders) 0
                    if {$Volume(readHeaders) == "0"} {
                        # These setting are set down below, 
                        # but we need them before MainUpdateMRML

                        VolumesManualSetPropertyType  Volume($m,node)
                    } else {
                        # Read headers
                if {[VolumesAutomaticSetPropertyType Volume($m,node)] == 0} {
                            return
                        }
                    }
                    if {[MainVolumesRead $m]<0 } {
                        DevErrorWindow "Error reading volume!"
                        return 0
                    }
                }
                # end if they chose to reread
            }
            # end if they should be asked to reread
        }
        # end if the volume id is not none.
    }
    # End thinking about rereading.


    # Update all fields that the user changed (not stuff that would 
    # need a file reread)
    
    Volume($m,node) SetName $Volume(name)
    Volume($m,node) SetDescription $Volume(desc)
    Volume($m,node) SetLabelMap $Volume(labelMap)
    eval Volume($m,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($m,node) SetTilt $Volume(gantryDetectorTilt)
    
    # This line can't be allowed to overwrite a RasToIjk matrix made
    # from headers when the volume is first created.
    #
    if {$Volume(readHeaders) == "0"} {
        Volume($m,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)
    }

    # If tabs are frozen, then 
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
        
    # Update pipeline
    MainVolumesUpdate $m

    # Update MRML: this reads in new volumes, among other things
    MainUpdateMRML
}
#-------------------------------------------------------------------------------
# .PROC VolumesPropsCancel
# Cancel: do not read in a new volume if in progress.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesPropsCancel {} {
    global Volume Module

    # Reset props
    set m $Volume(activeID)
    if {$m == "NEW"} {
        set m [lindex $Volume(idList) 0]
    }
    set Volume(freeze) 0
    MainVolumesSetActive $m

    # Unfreeze
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetFirst
# 
# Called after the User Selects the first file of the volume.
# Finds the filename, directory, and last image number.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetFirst {} {
    global Volume Mrml

    if {$::Module(verbose)} {
        puts "VolumesSetFirst: firstFile = $Volume(firstFile), Mrml(dir) = $Mrml(dir),\n calling  MainFileFindImageNumber Last with [file join $Mrml(dir) $Volume(firstFile)]"
    }
    # check to see if user cancelled and set filename to empty string
    if {$Volume(firstFile) == {} || $Volume(firstFile) == ""} {
        puts "VolumesSetFirst: firstFile not set"
        return
    }
    # check to see if user entered a non existant first file
    if {[file exists $Volume(firstFile)] == 0} {
        puts "VolumesSetFirst: first file does not exist: $Volume(firstFile)"
        DevErrorWindow "VolumesSetFirst: first file does not exist: $Volume(firstFile).\nSave the volume via Editor->Volumes->Save."
        return
    }
    set Volume(name)  [file root [file tail $Volume(firstFile)]]
    set Volume(DefaultDir) [file dirname [file join $Mrml(dir) $Volume(firstFile)]]
    # lastNum is an image number
    set Volume(lastNum)  [MainFileFindImageNumber Last \
        [file join $Mrml(dir) $Volume(firstFile)]]
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetScanOrder
# Set scan order for active volume, configure menubutton.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetScanOrder {order} {
    global Volume
    

    set Volume(scanOrder) $order

    # set the button text to the matching order from the scanOrderMenu
    $Volume(mbscanOrder) config -text [lindex $Volume(scanOrderMenu)\
            [lsearch $Volume(scanOrderList) $order]]

}

#-------------------------------------------------------------------------------
# .PROC VolumesSetScalarType
# Set scalar type and config menubutton to match.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetScalarType {type} {
    global Volume

    set Volume(scalarType) $type

    # update the button text
    $Volume(mbscalarType) config -text $type

}

#-------------------------------------------------------------------------------
# .PROC VolumesSetLast
# Sets last number and filename.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetLast {} {
    global Mrml Volume

    set Volume(lastNum) [MainFileFindImageNumber Last\
        [file join $Mrml(dir) $Volume(firstFile)]]
    set Volume(name) [file root [file tail $Volume(firstFile)]]
}

#-------------------------------------------------------------------------------
# .PROC VolumesEnter
# Called when module is entered.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesEnter {} {
    global Volumes Fiducials
    # push the Fiducials event manager onto the events stack so that user 
    # can add Fiducials with keys/mouse
    pushEventManager $Fiducials(eventManager)
    pushEventManager $Volumes(eventManager)

    DataExit
    bind Listbox <Control-Button-1> {tkListboxBeginToggle %W [%W index @%x,%y]}
    #tk_messageBox -type ok -message "VolumesEnter" -title "Title" -icon  info
    $Volumes(reformat,orMenu) invoke "ReformatSagittal"
    $Volumes(reformat,saveMenu) invoke "ReformatCoronal"
}

#-------------------------------------------------------------------------------
# .PROC VolumesExit
# Called when module is exited.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesExit {} {
    global Volumes

    popEventManager
    #tk_messageBox -type ok -message "VolumesExit" -title "Title" -icon  info
}

# >> Presets

#-------------------------------------------------------------------------------
# .PROC VolumesStorePresets
# Store preset values from this module into global array
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesStorePresets {p} {
    global Preset Volumes

    set Preset(Volumes,$p,DICOMStartDir) $Volumes(DICOMStartDir)
    set Preset(Volumes,$p,FileNameSortParam) $Volumes(FileNameSortParam)
    set Preset(Volumes,$p,DICOMPreviewWidth) $Volumes(DICOMPreviewWidth)
    set Preset(Volumes,$p,DICOMPreviewHeight) $Volumes(DICOMPreviewHeight)
    set Preset(Volumes,$p,DICOMPreviewHighestValue) $Volumes(DICOMPreviewHighestValue)
    set Preset(Volumes,$p,DICOMDataDictFile) $Volumes(DICOMDataDictFile)
}

#-------------------------------------------------------------------------------
# .PROC VolumesRecallPresets
# Set preset values from this module from global array
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesRecallPresets {p} {
    global Preset Volumes

    set Volumes(DICOMStartDir) $Preset(Volumes,$p,DICOMStartDir)
    set Volumes(FileNameSortParam) $Preset(Volumes,$p,FileNameSortParam)
    set Volumes(DICOMPreviewWidth) $Preset(Volumes,$p,DICOMPreviewWidth)
    set Volumes(DICOMPreviewHeight) $Preset(Volumes,$p,DICOMPreviewHeight)
    set Volumes(DICOMPreviewHighestValue) $Preset(Volumes,$p,DICOMPreviewHighestValue)
    set Volumes(DICOMDataDictFile) $Preset(Volumes,$p,DICOMDataDictFile)
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetReformatOrientation
# This procedures changes the text of the buttons on the reformat panel based on the current reformat orientation chosen by the user.
#
# .ARGS str or the orientation string: either ReformatAxial, ReformatSagittal, ReformatCoronal, or NewOrient
# .END
#-------------------------------------------------------------------------------
proc VolumesSetReformatOrientation {or} {
    global Volumes 

    set Volumes(reformat,orientation) $or
    $Volumes(reformat,orMenuB) config -text $or
    $Volumes(reformat,planeButton) configure -text "$Volumes(reformat,orientation) Plane"
    $Volumes(reformat,axisButton) configure -text "Define new $Volumes(reformat,$Volumes(reformat,orientation)Axis) axis"

}




#-------------------------------------------------------------------------------
# .PROC VolumesProjectVectorOnPlane
# Given a Vector V defined by V1{xyz} and V2{xyz} and a plane defined by its
# coefficients {A,B,C,D}, return a vector P that is the projection of V onto the plane
# .ARGS 
# .END
#-------------------------------------------------------------------------------
proc VolumesProjectVectorOnPlane {A B C D V1x V1y V1z V2x V2y V2z} {

    global Volumes P1 P2

    # tang is in the direction of p2-p1
    
    set evaluateP1 [expr $P1(x)*$A + $P1(y)*$B + $P1(z)*$C + $D]
    set evaluateP2 [expr $P2(x)*$A + $P2(y)*$B + $P2(z)*$C + $D]
    set Norm [expr sqrt($A*$A + $B*$B + $C*$C)]
    
    # in case p2 and p1 are not on the plane
    set distp1 [expr abs($evaluateP1/$Norm)]
    set distp2 [expr abs($evaluateP2/$Norm)]
    
    # now define the unit normal to this plane
    set n(x) $A
    set n(y) $B
    set n(z) $C
    Normalize n

    # see if the point is under or over the plane to know which direction
    # to project it in
    set multiplier 1
    if {$evaluateP1 < 0} {
        set multiplier -1 
    } 
    set p1projx [expr $P1(x)  - ($multiplier * $distp1 * $n(x))] 
    set p1projy [expr $P1(y)  - ($multiplier * $distp1 * $n(y))]
    set p1projz [expr $P1(z) - ($multiplier * $distp1 * $n(z))]
    
    set multiplier 1
    if {$evaluateP2 < 0} {
        set multiplier -1 
    } 
    set p2projx [expr $P2(x)  - ($multiplier * $distp2 * $n(x))] 
    set p2projy [expr $P2(y)  - ($multiplier * $distp2 * $n(y))]
    set p2projz [expr $P2(z)  - ($multiplier * $distp2 * $n(z))]
    
    
    set Projection(x) [expr $p2projx - $p1projx]
    set Projection(y) [expr $p2projy - $p1projy]
    set Projection(z) [expr $p2projz - $p1projz]
    Normalize Projection
    return "$Projection(x) $Projection(y) $Projection(z)"    
}

#-------------------------------------------------------------------------------
# .PROC VolumesReformatSlicePlane
#  This procedure changes the reformat matrix of either the ReformatAxial, 
# ReformatSagittal,ReformatCoronal orientation or NewOrient. The new 
# orientation is the plane defined by the 3 selected Fiducials.
# If the reformat orientation is either ReformatAxial, ReformatSagittal or 
# ReformatCoronal, then and the other 2 orthogonal orientations are also 
# calculated. This means that if the user decides to redefine the 
# ReformatAxial orientation, then the ReformatSagittal and ReformatCoronal 
# are automatically computed so that the 3 orientations are orthogonal.
#
# If the reformat orientation is NewOrient, then it doesn't affect any other
# slice orientations.
#
#  If there are more or less than 3 selected Fiducials, this procedure tells 
#  the user and is a no-op
# .ARGS str orientation has to be either reformatAxial, reformatSagittal, reformatCoronal or NewOrient
# .END
#-------------------------------------------------------------------------------
proc VolumesReformatSlicePlane {orientation} {
    global Volumes Fiducials Slice Slices P1 P2


    # first check that we are reading the right orientation
    if {$orientation != "ReformatAxial" && $orientation != "ReformatSagittal" && $orientation != "ReformatCoronal" && $orientation != "NewOrient"} {
        tk_messageBox -message "The orientation $orientation is not a valid one"
        return;
    }
    
    # next check to see that only 3 fiducials are selected
    set list [FiducialsGetAllSelectedPointIdList]
    if { [llength $list] < 3 } {
        # give warning and exit
        tk_messageBox -message "You have to create and select 3 fiducials"
        return
    } elseif { [llength $list] > 3 } {
        # give warning and exit
        tk_messageBox -message "Please select only 3 fiducials"
        return
    } else {
        # get the 3 selected fiducial points coordinates
        set count 1
        foreach pid $list {
            set xyz [Point($pid,node) GetXYZ]
            set p${count}x [lindex $xyz 0]
            set p${count}y [lindex $xyz 1]
            set p${count}z [lindex $xyz 2]
            incr count
        }

        # 3D plane equation
        set N(x) [expr $p1y * ($p2z - $p3z) + $p2y * ($p3z-$p1z) + $p3y * ($p1z-$p2z)]
        set N(y) [expr $p1z * ($p2x - $p3x) + $p2z * ($p3x-$p1x) + $p3z * ($p1x-$p2x)]
        set N(z) [expr $p1x * ($p2y-$p3y) + $p2x * ($p3y - $p1y) + $p3x * ($p1y - $p2y)]
        set coef [expr -($p1x * (($p2y* $p3z) - ($p3y* $p2z)) + $p2x * (($p3y * $p1z) - ($p1y * $p3z)) + $p3x * (($p1y*$p2z) - ($p2y *$p1z)))]

        
        # save the reformat plane equation coefficients
        set s $Slice(activeID)
        set Slice($s,reformatPlaneCoeff,A) $N(x)
        set Slice($s,reformatPlaneCoeff,B) $N(y)
        set Slice($s,reformatPlaneCoeff,C) $N(z)
        set Slice($s,reformatPlaneCoeff,D) $coef
            
        Normalize N

        ######################################################################
        ##################### CASE AXIAL, SAGITTAL, CORONAL ##################
        ######################################################################
        
        if {$orientation != "NewOrient" } {
            # Step 1, make sure the normal is oriented the right way by taking its dot product with the original normal


            if {$orientation == "ReformatSagittal" } {
                set originalN(x) -1
                set originalN(y) 0
                set originalN(z) 0
                set P1(x) 0
                set P1(y) 1
                set P1(z) 0
                set P2(x) 0
                set P2(y) 0
                set P2(z) 0
            } elseif { $orientation == "ReformatAxial" } {
                set originalN(x) 0
                set originalN(y) 0
                set originalN(z) -1
                set P1(x) 1
                set P1(y) 0
                set P1(z) 0
                set P2(x) 0
                set P2(y) 0
                set P2(z) 0
            } elseif { $orientation == "ReformatCoronal" } {
                set originalN(x) 0
                set originalN(y) 1
                set originalN(z) 0
                set P1(x) 1
                set P1(y) 0
                set P1(z) 0
                set P2(x) 0
                set P2(y) 0
                set P2(z) 0
            }
            
            if {[expr $N(x)*$originalN(x) +  $N(y)*$originalN(y) +  $N(z)*$originalN(z)] <0 } {
            
                set N(x) [expr -$N(x)]
                set N(y) [expr -$N(y)]
                set N(z) [expr -$N(z)]
                set Slice($s,reformatPlaneCoeff,A) [expr -$Slice($s,reformatPlaneCoeff,A)]
                set Slice($s,reformatPlaneCoeff,B) [expr -$Slice($s,reformatPlaneCoeff,B)]
                set Slice($s,reformatPlaneCoeff,C) [expr -$Slice($s,reformatPlaneCoeff,C)]
                set Slice($s,reformatPlaneCoeff,D) [expr -$Slice($s,reformatPlaneCoeff,D)]
            }
            
            # get the distance from 0,0,0 to the plane
            set dist [expr -$Slice($s,reformatPlaneCoeff,D)/ sqrt($Slice($s,reformatPlaneCoeff,A)*$Slice($s,reformatPlaneCoeff,A)+ $Slice($s,reformatPlaneCoeff,B)*$Slice($s,reformatPlaneCoeff,B) + $Slice($s,reformatPlaneCoeff,C)*$Slice($s,reformatPlaneCoeff,C))]
        

            # Step 2, project the original tangent onto the plane
            set proj [VolumesProjectVectorOnPlane $Slice($s,reformatPlaneCoeff,A) $Slice($s,reformatPlaneCoeff,B) $Slice($s,reformatPlaneCoeff,C) $Slice($s,reformatPlaneCoeff,D) $P1(x) $P1(y) P1(z) $P2(x) $P2(y) $P2(z)]
            set T(x) [lindex $proj 0]
            set T(y) [lindex $proj 1]
            set T(z) [lindex $proj 2]
            
            
            # set the reformat matrix of the active slice, make the origin 0 by default.
            Slicer SetReformatNTP $orientation $N(x) $N(y) $N(z) $T(x) $T(y) $T(z) 0 0 0
            MainSlicesSetOrientAll "ReformatAxiSagCor"
        } else {
            
            ###################################################################
            ############################CASE NEW ORIENT  ######################
            ###################################################################
            # we are less smart about things, just take the 0 -1 0 vector and
            # project it onto the new plane to get a tangent 
            # 
        

            set P1(x) 0
            set P1(y) 1
            set P1(z) 0
            set P2(x) 0
            set P2(y) 0
            set P2(z) 0
            set T(x) [lindex $proj 0]
            set T(y) [lindex $proj 1]
            set T(z) [lindex $proj 2]
            Slicer SetNewOrientNTP $Slice(activeID) $N(x) $N(y) $N(z) $T(x) $T(y) $T(z) 0 0 0
            MainSlices SetOrient $Slice(activeID) "NewOrient"
        }
        # make all 3 slices show the new Reformat orientation

        MainSlicesSetOffset $Slice(activeID) $dist
        RenderAll
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumesRotateSlicePlane
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesRotateSlicePlane {orientation} {
    global Volumes Slices Slice P1 P2
    
    # the tangent is in the direction of the selected 2 fiducials
    
    # first check to see that only 2 fiducials are selected
    set list [FiducialsGetAllSelectedPointIdList]
    if { [llength $list] < 2 } {
        # give warning and exit
        tk_messageBox -message "You have to create and select 2 fiducials (they do not necessarily have to be on the reformated plane)"        
        return
    } elseif { [llength $list] > 2 } {
        # give warning and exit
        tk_messageBox -message "Please select only 2 fiducials"
        return
    } else {
        # get the 2 points coordinates
        set count 1
        foreach pid $list {
            set xyz [Point($pid,node) GetXYZ]
            set temp${count}(x) [lindex $xyz 0]
            set temp${count}(y) [lindex $xyz 1]
            set temp${count}(z) [lindex $xyz 2]
            incr count
        }

        # if we want to define a new "RL" axis for the reformatted axial,
        # the first point needs to be the one closest to R, so with the 
        # highest x coordinate
        
        if {$orientation == "ReformatAxial" || $orientation == "ReformatCoronal"} {
            if {$temp1(x) < $temp2(x)} {
                set P1(x) $temp2(x)
                set P1(y) $temp2(y)
                set P1(z) $temp2(z)
                set P2(x) $temp1(x)
                set P2(y) $temp1(y)
                set P2(z) $temp1(z)
            } else {
                set P1(x) $temp1(x)
                set P1(y) $temp1(y)
                set P1(z) $temp1(z)
                set P2(x) $temp2(x)
                set P2(y) $temp2(y)
                set P2(z) $temp2(z)
            }
        } 

        # if we want to define a new "PA" axis for the reformatted axial,
        # the first point needs to be the one closest to R, so with the 
        # highest y coordinate

        if {$orientation == "ReformatSagittal" || $orientation == "NewOrient" } {
            if {$temp1(y) < $temp2(y)} {
                set P1(x) $temp2(x)
                set P1(y) $temp2(y)
                set P1(z) $temp2(z)
                set P2(x) $temp1(x)
                set P2(y) $temp1(y)
                set P2(z) $temp1(z)
            } else {
                set P1(x) $temp1(x)
                set P1(y) $temp1(y)
                set P1(z) $temp1(z)
                set P2(x) $temp2(x)
                set P2(y) $temp2(y)
                set P2(z) $temp2(z)
            }
        } 
        set s $Slice(activeID)
        set A $Slice($s,reformatPlaneCoeff,A) 
        set B $Slice($s,reformatPlaneCoeff,B) 
        set C $Slice($s,reformatPlaneCoeff,C) 
        set D $Slice($s,reformatPlaneCoeff,D) 

        set proj [VolumesProjectVectorOnPlane $A $B $C $D $P1(x) $P1(y) P1(z) $P2(x) $P2(y) $P2(z) ]
        set T(x) [lindex $proj 0]
        set T(y) [lindex $proj 1]
        set T(z) [lindex $proj 2]
        
        set N(x) $A
        set N(y) $B
        set N(z) $C
        Normalize N
        
        # set the reformat matrix of the active slice
        Slicer SetReformatNTP $orientation $N(x) $N(y) $N(z) $T(x) $T(y) $T(z) 0 0 0

        MainSlicesSetOrientAll "ReformatAxiSagCor"
        RenderBoth $Slice(activeID)    
    }

}



#-------------------------------------------------------------------------------
# .PROC VolumesReformatSave
#  Save the Active Volume slice by slice with the reformat matrix of the 
#  chosen slice orientation in $Volumes(reformat,scanOrder)
# .ARGS
#       str orientation orientation of that slice to use when saving the 
#       volume slice by slice
# .END
#-------------------------------------------------------------------------------
# turn into smart image writer


#-------------------------------------------------------------------------------
# .PROC VolumesReformatSave
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesReformatSave {} {
    
    global Slices Slice Volume Gui Volumes
    
    # get the chosen volume
    set v $Volume(activeID)
    
    # set initial directory to dir where vol last opened if unset
    if {$Volumes(prefixSave) == ""} {
        set Volumes(prefixSave) \
            [file join $Volume(DefaultDir) [Volume($v,node) GetName]]
    }
    
    if {$Gui(pc) == 1} {
        # this is a hack to get a full path for the save directory - sp
        global Mrml savedir
        set savedir $Mrml(dir) 
        set Mrml(dir) ""
    }
    # Show user a File dialog box
    set Volumes(prefixSave) [MainFileSaveVolume $v $Volumes(prefixSave)]
    if {$Gui(pc) == 1} {
        global Mrml savedir
        set Mrml(dir) $savedir
    }
    if {$Volumes(prefixSave) == ""} {
        DevErrorWindow "Set a file prefix to save the volume"
        return
    }
    
    
    
    # the idea is to slide the volume from the low to the high offset and
    # save a slice each time
    set s $Slice(activeID)
    # make the slice the right orientation to get the right reformat
    # matrix
    # check to make sure that the saveOrder was set
    if {[info exists Volumes(reformat,saveOrder)] == 0} {
        DevErrorWindow "Please choose a scan order"
        return
    }
    MainSlicesSetOrient $s $Volumes(reformat,saveOrder)

    scan [Volume($Volume(activeID),node) GetImageRange] "%d %d" lo hi
    
    set num [expr ($hi - $lo) + 1]
    set slo [Slicer GetOffsetRangeLow  $s]
    set shi [Slicer GetOffsetRangeHigh $s]
    
    set extra [expr $shi - int($num/2)]
    
    set lo [expr $slo + $extra]
    set hi [expr $shi - $extra]
    
    Volumes(reformatter) SetInput [Volume($Volume(activeID),vol) GetOutput]
    Volumes(reformatter) SetWldToIjkMatrix [[Volume($Volume(activeID),vol) GetMrmlNode] GetWldToIjk]
    Volumes(reformatter) SetInterpolate 1
    Volumes(reformatter) SetResolution [lindex [Volume($Volume(activeID),node) GetDimensions] 0]
    # Volumes(reformatter) SetFieldOfView [expr [lindex [Volume($Volume(activeID),node) GetDimensions] 0] * [lindex [Volume($Volume(activeID),node) GetSpacing] 0]]
    set maxfov 0
    for {set i 0} {$i < 2} {incr i} {
        set dim [lindex [Volume($Volume(activeID),node) GetDimensions] $i] 
        set space [lindex [Volume($Volume(activeID),node) GetSpacing] $i]
        set fov [expr $dim * $space]
        if {$fov > $maxfov} {
            set maxfov $fov
        }
    }
    set space [lindex [Volume($Volume(activeID),node) GetSpacing] 2]
    set fov [expr $num * $space]
    if {$fov > $maxfov} {
        set maxfov $fov
    }


    Volumes(reformatter) SetFieldOfView $maxfov
    
    set ref [Slicer GetReformatMatrix $s]
    
    # need the slices to be written out as 1-n, instead of 0-(n-1)
    # set ii 0
    set ii 1
    if {$::Module(verbose)} {
        puts "VolumesReformatSave: starting file names from $ii"
    }
    set lo [expr -1 * round ($maxfov / 2.)]
    set hi [expr -1 * $lo]
    for {set i $lo} {$i<= $hi} {set i [expr $i + 1]} {
    
        MainSlicesSetOffset $s $i
        Volumes(reformatter) SetReformatMatrix $ref
        Volumes(reformatter) Modified
        Volumes(reformatter) Update
        #RenderBoth $s
        Volumes(writer) SetInput [Volumes(reformatter) GetOutput]
        set ext [expr $i + $hi]
        set iii [format %03d $ii]
        Volumes(writer) SetFileName "$Volumes(prefixSave).$iii"
        set Gui(progressText) "Writing slice $ext"
        Volumes(writer) Write
        incr ii
    }
    set Gui(progressText) "Done!"
    Volumes(writer) UpdateProgress 0
    
}

