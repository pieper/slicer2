#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: CompareModule.tcl,v $
#   Date:      $Date: 2005/12/27 18:34:28 $
#   Version:   $Revision: 1.1.2.3 $
# 
#===============================================================================
# FILE:        CompareModule.tcl
# PROCEDURES:  
#   CompareModuleInit
#   CompareModuleBuildGUI
#   CompareModuleBuildVTK
#   CompareModuleEnter
#   CompareModuleExit
#   CompareModuleSetLinking
#   CompareModuleResetOffsets
#   CompareModuleEnableLinkControls
#==========================================================================auto=

# TODO : update the range depending on the volumes range (CompareModuleSetLinking).
# The volumes are to be comparable (i-e : of same dimension, spacing...)
# -> retrieve range from first volume

#-------------------------------------------------------------------------------
# .PROC CompareModuleInit
# Set CompareModule array to the proper initial values.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleInit {} {
    global CompareModule Module Volume Model MultiSlicer CompareView

    set m CompareModule

    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1.2.3 $} {$Date: 2005/12/27 18:34:28 $}]

    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "This module provides volumes comparison tools."
    set Module($m,author) "Anquez, Jeremie, Ecole Nationale Superieure des \
    Telecommunications Laboratoire TSI, anquez@tsi.enst.fr"

    set Module($m,category) "Visualisation"

    # Define Tabs
    #------------------------------------
    set Module($m,row1List) "Help Display Mosaik Flip"
    set Module($m,row1Name) "{Help} {Display} {Mosaik} {Flip}"
    set Module($m,row1,tab) Display

    # Define Procedures
    #------------------------------------
    set Module($m,procGUI) CompareModuleBuildGUI
    set Module($m,procVTK) CompareModuleBuildVTK
    set Module($m,procEnter) CompareModuleEnter

    # calls init procedures for the other module files
    CompareAnnoInit
    CompareSlicesInit
    CompareMosaikInit
    CompareInteractorInit
    CompareViewerInit
    CompareFlipInit
}

#-------------------------------------------------------------------------------
# .PROC CompareModuleBuildGUI
# Builds module GUI to be displayed when the module is selected by the user
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleBuildGUI {} {
    global Module Gui Volume View CompareModule CompareAnno CompareViewer

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    #   General
    #     NbSlices
    #     OpacityManager
    #   Linking
    #     Mode
    #     Offset
    #     OffsetIncrement
    #     Orientation
    #   Cursor
    #   Anno
    #     Coords
    #     Precision
    # Mosaik
    #   Volume
    #     ForeVolume
    #     BackVolume
    #   Subdivision
    #     WidthDivision
    #     HeightDivision
    #   Opacity
    #   OffsetOrientation
    #     Offset
    #     OffsetIncrement
    #     Orientation
    #     Reset
    # Flip
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------

set help "
Description by tab:
<BR><BR><B>GENERAL REMARK :</B> for this module to work properly, only display \"Comparable\"
 volumes, i-e volumes of same origin, dimension and spacing.
<BR><LI><B>Display:</B> Set the <B>Number of slices</B> you wish to display, using
 the radio buttons. Use the controls in the top right corner of each slice to
 set the background, foreground and labelmap displayed.
<BR><BR>You can fade from background to foreground using the <B>opacity</B> slider.
 By default, the slices display is independant (<B>linking</B> 'Off' button selected).
 Each slice has its own orientation, zoom/pan and offset (like in 3D Slicer general
 framework).
<BR><BR>Clicking on <B>Linking</B> 'On' button activates linked display. Specific link
 controls are enabled. The orientation, zoom/pan, offset and cursor are then the same
 on every slice.
<BR> The <B>R</B> button resets zoom and pan on every slice (pressing 'r' key over
 a slice realizes the same operation)
<BR><BR>Select the <B>cursor</B> display (visibility and intersection)
<BR><BR>Select the <B>annotation</B> display (RAS, IJK or XY coordinate mode and pixel
 display precision).

<BR><LI><B>Mosaik</B> Display a mosaik mixing 2 volumes (useful to check registration
 results)
<BR>Set the <B>reference</B> and the <B>second</B> volumes to be displayed.
<BR><BR>Set the number of subdivision, following <B>width</B> and <B>height</B>
<BR><BR>Set the <B>opacity</B> between the reference and the second volume
<BR><BR>Set <B>offset</B>, <B>offset increment</B> and <B>orientation</B> of the mosaik.

<BR><LI><B>Flip</B> Flip the volume following the 3 main axes. This operation doesn't
 generate any transform node, but modifies the Volume node."

    regsub -all "\n" $help {} help
    MainHelpApplyTags CompareModule $help
    MainHelpBuildGUI CompareModule

    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(CompareModule,fDisplay)
    bind $fDisplay <Expose> {
      if {$CompareViewer(multiOrMosaik) == "mosaik"} {
        set CompareViewer(multiOrMosaik) "multiSlice"
        CompareViewerSetMode $CompareViewer(mode)
      }
    }

    set f $fDisplay

    frame $f.fGeneral   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fLinking    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fCursor    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fAnno    -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fGeneral $f.fLinking $f.fCursor $f.fAnno \
        -side top -pady 2 -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Display->General frame
    #-------------------------------------------

    set f $fDisplay.fGeneral

    frame $f.fNbSlices    -bg $Gui(activeWorkspace)
    frame $f.fOpacityManager -bg $Gui(activeWorkspace)
    pack $f.fNbSlices $f.fOpacityManager -side top -fill x -expand 1

    #-------------------------------------------
    # Display->General->NbSlices frame
    #-------------------------------------------

    set f $fDisplay.fGeneral.fNbSlices

    DevAddLabel $f.lNbSlices "Number of slices :"
    frame $f.fMode -bg $Gui(activeWorkspace)
    pack $f.lNbSlices $f.fMode -side left -padx $Gui(pad)  -pady $Gui(pad) -fill x

    foreach value "2 3 4 6 9" text "2 3 4 6 9" width "3 3 3 3 3" {
        eval {radiobutton $f.fMode.rNbSlices$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable CompareViewer(mode) \
            -command "CompareViewerSetMode; CompareRenderSlices"} $Gui(WCA)
        pack $f.fMode.rNbSlices$value -side left -fill x
    }

    #-------------------------------------------
    # Display->General->OpacityManager frame
    #-------------------------------------------

    set f $fDisplay.fGeneral.fOpacityManager

    frame $f.fToggle -bg $Gui(activeWorkspace)
    frame $f.fSlider -bg $Gui(activeWorkspace)
    pack $f.fToggle $f.fSlider -side left -padx 4 -pady $Gui(pad) -fill x

    set f $fDisplay.fGeneral.fOpacityManager.fToggle
    DevAddLabel $f.lOpacity "Opacity :"

    eval {button $f.bToggle -text Toggle -width 6 \
        -command "CompareSlicesSetOpacityToggle; CompareRenderSlices"} $Gui(WBA)

    pack $f.lOpacity $f.bToggle -side left -padx 1

    set f $fDisplay.fGeneral.fOpacityManager.fSlider

    DevAddLabel $f.lBg "Bg"

    eval {scale $f.sOpacity -from 0.0 -to 1.0 -variable CompareSlice(opacity) \
        -command "CompareSlicesSetOpacityAll; CompareRenderSlices" \
        -length 80 -resolution 0.1} $Gui(WSA) {-sliderlength 30  \
        -orient horizontal}

    TooltipAdd $f.sOpacity "Slice overlay slider: Fade from\n\
        the Background to the Foreground slice."

    DevAddLabel $f.lFg "Fg"

    pack $f.lBg $f.sOpacity $f.lFg -side left -padx 1

    #-------------------------------------------
    # Display->Linking frame
    #-------------------------------------------

    set f $fDisplay.fLinking

    frame $f.fMode    -bg $Gui(activeWorkspace)
    frame $f.fOffset -bg $Gui(activeWorkspace)
    frame $f.fOffsetIncrement -bg $Gui(activeWorkspace)
    frame $f.fOrientation -bg $Gui(activeWorkspace)
    pack $f.fMode $f.fOffset $f.fOffsetIncrement $f.fOrientation -side top -fill x -expand 1

    #-------------------------------------------
    # Display->Linking->Mode frame
    #-------------------------------------------

    set f $fDisplay.fLinking.fMode

    DevAddLabel $f.lLinking "Link slices :"
    frame $f.fLinkingSetting -bg $Gui(activeWorkspace)
    frame $f.fReset -bg $Gui(activeWorkspace)
    pack $f.lLinking $f.fLinkingSetting $f.fReset -side left -padx $Gui(pad) \
    -pady $Gui(pad) -fill x

    foreach value "on off" text "on off" width "8 8" {
        eval {radiobutton $f.fLinkingSetting.rLinking$value -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable CompareViewer(linked) \
            -command "CompareModuleSetLinking"} $Gui(WCA)
        pack $f.fLinkingSetting.rLinking$value -side left
    }

    eval {button $f.fReset.bReset \
        -text "R" -width 2 \
        -command "CompareSlicesResetZoomAll ; CompareRenderSlices"} $Gui(WBA)
    pack $f.fReset.bReset -side left
    # tooltip for reset button
    TooltipAdd $f.fReset.bReset "Click to reset zoom and pan."

    #-------------------------------------------
    # Display->Linking->Offset frame
    #-------------------------------------------

    set f $fDisplay.fLinking.fOffset

    DevAddLabel $f.lOffset "Offset :"
    frame $f.fSlider -bg $Gui(activeWorkspace)

    pack $f.lOffset $f.fSlider -side left -padx $Gui(pad) \
    -pady $Gui(pad) -fill x

    set f $f.fSlider
    set fov2 [expr $View(fov) / 2]

    eval {entry $f.eOffset -width 4 -textvariable CompareSlice(offset)} $Gui(WEA)
    bind $f.eOffset <Return>   "CompareSlicesSetOffsetAll; CompareRenderSlices"
        bind $f.eOffset <FocusOut> "CompareSlicesSetOffsetAll; CompareRenderSlices"

    # tooltip for entry box
    set tip "Current slice: in mm or slice increments,\n \
        depending on the slice orientation you have chosen.\n \
        The default (AxiSagCor orientation) is in mm. \n \
        When editing (Slices orientation), slice numbers are shown.\n\
        To change the distance between slices from the default\n\
        1 mm, right-click on the V button."

    TooltipAdd $f.eOffset $tip

    eval {scale $f.sOffset -from -$fov2 -to $fov2 \
        -variable CompareSlice(offset) -length 125 -resolution 1.0 -command \
        "CompareSlicesSetOffsetAll; CompareRenderSlices"} $Gui(WSA)

    pack $f.sOffset $f.eOffset -side left -anchor w -padx 2 -pady 0

    #-------------------------------------------
    # Display->Linking->OffsetIncrement frame
    #-------------------------------------------

    set f $fDisplay.fLinking.fOffsetIncrement

    eval {label $f.lIncrement -text "Slice Increment: "} $Gui(WLA)

    eval {entry $f.eIncrement -width 7 \
        -textvariable CompareSlice(offsetIncrement)} $Gui(WEA)
    bind $f.eIncrement <Return>   \
        "CompareSlicesSetOffsetIncrementAll"
    bind $f.eIncrement <FocusOut>   \
        "CompareSlicesSetOffsetIncrementAll"

    pack $f.lIncrement $f.eIncrement -side left -padx $Gui(pad) \
    -pady $Gui(pad)

    TooltipAdd $f.eIncrement "Enter increment between reformatted\n\
        slices in mm, and hit Enter.\nThe slider will move by this amount."

    #-------------------------------------------
    # Display->Linking->Orientation frame
    #-------------------------------------------

    set f $fDisplay.fLinking.fOrientation

    DevAddLabel $f.lOrient "Orientation :"
    frame $f.fChooseOrient -bg $Gui(activeWorkspace)

    pack $f.lOrient $f.fChooseOrient -side left -padx $Gui(pad) \
    -pady $Gui(pad) -fill x

    set f $fDisplay.fLinking.fOrientation.fChooseOrient

    eval {menubutton $f.mbOrient -text INIT -menu $f.mbOrient.m \
        -width 13} $Gui(WMBA)

    pack $f.mbOrient -side left -pady 0 -padx 2 -fill x

    # tooltip for orientation menu for slice
    TooltipAdd $f.mbOrient "Set Orientation of all slices."

    eval {menu $f.mbOrient.m} $Gui(WMA)
    set CompareModule(menu) $f.mbOrient.m

    foreach item "[MultiSlicer GetOrientList]" {
        $f.mbOrient.m add command -label $item -command \
            "CompareSlicesSetOrientAll $item; CompareViewerHideSliceControls; CompareRenderSlices"
    }

    # enable/disable controls following CompareViewer(linked) initial value
    CompareModuleEnableLinkControls

    #-------------------------------------------
    # Display->Cursor frame
    #-------------------------------------------

    set f $fDisplay.fCursor

    eval {label $f.lTitle -text "Cursor Display"} $Gui(WTA)
    pack $f.lTitle -side top -padx $Gui(pad) -pady $Gui(pad)

    eval {checkbutton $f.cShowCursor -text "Show Cursor" \
              -variable CompareAnno(cross) -indicatoron 0 \
              -command "CompareAnnoSetVisibility; CompareRenderSlices"} $Gui(WCA)
    pack $f.cShowCursor -side top -fill x -padx $Gui(pad) -pady $Gui(pad)

    eval {checkbutton $f.cShowCross -text "Show Hash Marks" \
              -variable CompareAnno(hashes) -indicatoron 0 \
              -command "CompareAnnoSetVisibility; CompareRenderSlices"} $Gui(WCA)
    pack $f.cShowCross -side top -fill x -padx $Gui(pad) -pady $Gui(pad)

    eval {checkbutton $f.cCrossIntersect -text "Intersect Crosshairs" \
              -variable CompareAnno(crossIntersect) -indicatoron 0 \
              -command "CompareAnnoSetVisibility; CompareRenderSlices"} $Gui(WCA)
    pack $f.cCrossIntersect -side top -fill x -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display->Anno frame
    #-------------------------------------------

    set f $fDisplay.fAnno

    eval {label $f.lTitle -text "Annotations Display"} $Gui(WTA)
    frame $f.fCoords -bg $Gui(activeWorkspace)
    frame $f.fPrecision -bg $Gui(activeWorkspace)

    pack $f.lTitle $f.fCoords $f.fPrecision -side top -pady $Gui(pad)

    #-------------------------------------------
    # Display->Anno->Coords frame
    #-------------------------------------------
    set f $fDisplay.fAnno.fCoords

    set tip1 "Display of coordinates on 2D slices:\n"
    eval {label $f.l -text "Slice Cursor:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    foreach mode "RAS IJK XY" \
        tip {"RAS coordinates" "array indices into volume" \
        "2D slice coordinates"} {
        eval {radiobutton $f.f.r$mode -width 4 \
            -text "$mode" -variable CompareAnno(cursorMode) -value $mode \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.r$mode -side left -padx 0 -pady 0
        TooltipAdd $f.f.r$mode "$tip1 $tip"
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Display->Anno->Precision frame
    #-------------------------------------------
    set f $fDisplay.fAnno.fPrecision

    set tip1 "Display of pixel values on 2D slices:\n"
    eval {label $f.l -text "Pixel Display:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)

    foreach button "int flt full" \
        mode "%.f %6.2f %f" \
        text "1 1.00 full" \
        tip {"integer display" "floating point" \
        "full: all decimal places shown"} \
        {
        eval {radiobutton $f.f.r$button -width 4 \
            -text "$text" -variable CompareAnno(pixelDispFormat) \
            -value $mode \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.r$button -side left -padx 0 -pady 0
        TooltipAdd $f.f.r$button "$tip1 $tip"

    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Mosaik frame
    #-------------------------------------------
    set fMosaik $Module(CompareModule,fMosaik)

    bind $fMosaik <Expose> {

      if {$CompareViewer(multiOrMosaik) == "multiSlice"} {
        set CompareViewer(multiOrMosaik) "mosaik"
        CompareViewerSetMode
      }
    }

    set f $fMosaik

    frame $f.fVolume   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fSubdivision   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fOpacity    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fOffsetOrientation    -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fVolume $f.fSubdivision $f.fOpacity $f.fOffsetOrientation \
        -side top -pady 2 -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Mosaik->Volume frame
    #-------------------------------------------

    set f $fMosaik.fVolume

    frame $f.fBackVolume    -bg $Gui(activeWorkspace)
    frame $f.fForeVolume -bg $Gui(activeWorkspace)
    pack $f.fBackVolume $f.fForeVolume -side top -fill x -expand 1


    #-------------------------------------------
    # Mosaik->Volume->BackVolume frame
    #-------------------------------------------

    set f $fMosaik.fVolume.fBackVolume

    set layer "Back"
    DevAddLabel $f.l${layer}Volume "Reference volume :"

    eval {menubutton $f.mb${layer}Volume -text None -width 13 \
        -menu $f.mb${layer}Volume.m} $Gui(WMBA)

    eval {menu $f.mb${layer}Volume.m} $Gui(WMA)

    TooltipAdd $f.mb${layer}Volume "Volume Selection: choose a volume\
        to appear\nin the $layer layer in this slice window."

    pack $f.l${layer}Volume $f.mb${layer}Volume \
        -pady 0 -padx 2 -side top

    #-------------------------------------------
    # Mosaik->Volume->ForeVolume frame
    #-------------------------------------------

    set f $fMosaik.fVolume.fForeVolume

    set layer "Fore"
    DevAddLabel $f.l${layer}Volume "Second volume :"

    eval {menubutton $f.mb${layer}Volume -text None -width 13 \
        -menu $f.mb${layer}Volume.m} $Gui(WMBA)

    eval {menu $f.mb${layer}Volume.m} $Gui(WMA)

    TooltipAdd $f.mb${layer}Volume "Volume Selection: choose a volume\
        to appear\nin the $layer layer in this slice window."

    pack $f.l${layer}Volume $f.mb${layer}Volume \
        -pady 0 -padx 2 -side top

    #-------------------------------------------
    # Mosaik->Subdivision frame
    #-------------------------------------------

    set f $fMosaik.fSubdivision

    frame $f.fWidthDivision    -bg $Gui(activeWorkspace)
    frame $f.fHeightDivision -bg $Gui(activeWorkspace)
    pack $f.fWidthDivision $f.fHeightDivision -side top -fill x -expand 1

    #-------------------------------------------
    # Mosaik->Subdivision->WidthDivision frame
    #-------------------------------------------

    set f $f.fWidthDivision

    DevAddLabel $f.lWidthDivision "Nb of width subdivisions :"
    frame $f.fCheckButtons -bg $Gui(activeWorkspace)
    pack $f.lWidthDivision $f.fCheckButtons -side top -padx $Gui(pad)  -pady $Gui(pad)

    foreach value "128 64 32 16 8" text "2 4 8 16 32" width "3 3 3 3 3" {
        eval {radiobutton $f.fCheckButtons.rNbDivisions$text -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable CompareMosaik(widthDivision) \
            -command "CompareMosaikSetDivision; CompareRenderMosaik"} $Gui(WCA)
        pack $f.fCheckButtons.rNbDivisions$text -side left
    }

    #-------------------------------------------
    # Mosaik->Subdivision->HeightDivision frame
    #-------------------------------------------

    set f $fMosaik.fSubdivision.fHeightDivision

    DevAddLabel $f.lHeightDivision "Nb of height subdivisions :"
    frame $f.fCheckButtons -bg $Gui(activeWorkspace)
    pack $f.lHeightDivision $f.fCheckButtons -side top -padx $Gui(pad)  -pady $Gui(pad)

    foreach value "128 64 32 16 8" text "2 4 8 16 32" width "3 3 3 3 3" {
        eval {radiobutton $f.fCheckButtons.rNbDivisions$text -width $width -indicatoron 0\
            -text "$text" -value "$value" -variable CompareMosaik(heightDivision) \
            -command "CompareMosaikSetDivision; CompareRenderMosaik"} $Gui(WCA)
        pack $f.fCheckButtons.rNbDivisions$text -side left
    }

    #-------------------------------------------
    # Mosaik->Opacity frame
    #-------------------------------------------

    set f $fMosaik.fOpacity

    frame $f.fToggle -bg $Gui(activeWorkspace)
    frame $f.fSlider -bg $Gui(activeWorkspace)
    pack $f.fToggle $f.fSlider -side left -padx 2  -pady $Gui(pad) -fill x

    set f $fMosaik.fOpacity.fToggle
    DevAddLabel $f.lOpacity "Opacity :"

    eval {button $f.bToggle -text Toggle -width 6 \
        -command "CompareMosaikSetOpacityToggle; CompareRenderMosaik"} $Gui(WBA)

    pack $f.lOpacity $f.bToggle -side left -padx 1

    set f $fMosaik.fOpacity.fSlider

    DevAddLabel $f.lRef "Ref"

    eval {scale $f.sOpacity -from 0.0 -to 1.0 -variable CompareMosaik(opacity) \
        -command "CompareMosaikSetOpacity; CompareRenderMosaik" \
        -length 80 -resolution 0.1} $Gui(WSA) {-sliderlength 30  \
        -orient horizontal}

    TooltipAdd $f.sOpacity "Mosaik overlay slider: Fade from\n\
        the Foreground to the Background slice."

    DevAddLabel $f.l2 "2nd"

    pack $f.lRef $f.sOpacity $f.l2 -side left -padx 1

    #-------------------------------------------
    # Mosaik->OffsetOrientation frame
    #-------------------------------------------

    set f $fMosaik.fOffsetOrientation

    frame $f.fOffset -bg $Gui(activeWorkspace)
    frame $f.fOffsetIncrement -bg $Gui(activeWorkspace)
    frame $f.fOrientation -bg $Gui(activeWorkspace)
    frame $f.fReset -bg $Gui(activeWorkspace)
    pack $f.fOffset $f.fOffsetIncrement $f.fOrientation $f.fReset -side top -fill x -expand 1

    #-------------------------------------------
    # Mosaik->OffsetOrientation->Offset frame
    #-------------------------------------------

    set f $fMosaik.fOffsetOrientation.fOffset

    DevAddLabel $f.lOffset "Offset :"
    frame $f.fSlider -bg $Gui(activeWorkspace)

    pack $f.lOffset $f.fSlider -side left -padx $Gui(pad) \
    -pady $Gui(pad) -fill x

    set f $f.fSlider
    set fov2 [expr $View(fov) / 2]

    eval {entry $f.eOffset -width 4 -textvariable CompareMosaik(offset)} $Gui(WEA)
    bind $f.eOffset <Return>   "CompareMosaikSetOffset; CompareRenderMosaik"
        bind $f.eOffset <FocusOut> "CompareMosaikSetOffset; CompareRenderMosaik"

    # tooltip for entry box
    set tip "Current slice: in mm or slice increments,\n \
        depending on the slice orientation you have chosen.\n \
        The default (AxiSagCor orientation) is in mm. \n \
        When editing (Slices orientation), slice numbers are shown.\n\
        To change the distance between slices from the default\n\
        1 mm, right-click on the V button."

    TooltipAdd $f.eOffset $tip

    eval {scale $f.sOffset -from -$fov2 -to $fov2 \
        -variable CompareMosaik(offset) -length 125 -resolution 1.0 -command \
        "CompareMosaikSetOffset; CompareRenderMosaik"} $Gui(WSA)

    pack $f.sOffset $f.eOffset -side left -anchor w -padx 2 -pady 0

    #-------------------------------------------
    # Mosaik->OffsetOrientation->OffsetIncrement frame
    #-------------------------------------------

    set f $fMosaik.fOffsetOrientation.fOffsetIncrement

    eval {label $f.lIncrement -text "Slice Increment: "} $Gui(WLA)

    eval {entry $f.eIncrement -width 7 \
        -textvariable CompareMosaik(offsetIncrement)} $Gui(WEA)
    bind $f.eIncrement <Return>   \
        "CompareMosaikSetOffsetIncrement"
    bind $f.eIncrement <FocusOut>   \
        "CompareMosaikSetOffsetIncrement"

    pack $f.lIncrement $f.eIncrement -side left -padx $Gui(pad) \
    -pady $Gui(pad)

    TooltipAdd $f.eIncrement "Enter increment between reformatted\n\
        slices in mm, and hit Enter.\nThe slider will move by this amount."

    #-------------------------------------------
    # Mosaik->OffsetOrientation->Orientation frame
    #-------------------------------------------

    set f $fMosaik.fOffsetOrientation.fOrientation

    DevAddLabel $f.lOrient "Orientation :"
    frame $f.fChooseOrient -bg $Gui(activeWorkspace)

    pack $f.lOrient $f.fChooseOrient -side left -padx $Gui(pad) \
    -pady $Gui(pad) -fill x

    set f $fMosaik.fOffsetOrientation.fOrientation.fChooseOrient

    eval {menubutton $f.mbOrient -text INIT -menu $f.mbOrient.m \
        -width 13} $Gui(WMBA)

    pack $f.mbOrient -side left -pady 0 -padx 2 -fill x

    # tooltip for orientation menu for slice
    TooltipAdd $f.mbOrient "Set Orientation of all slices."

    eval {menu $f.mbOrient.m} $Gui(WMA)

    foreach item "[MultiSlicer GetOrientList]" {
        $f.mbOrient.m add command -label $item -command \
            "CompareMosaikSetOrient $item; CompareRenderMosaik"
    }

    #-------------------------------------------
    # Mosaik->OffsetOrientation->Reset frame
    #-------------------------------------------

    set f $fMosaik.fOffsetOrientation.fReset

    DevAddLabel $f.lReset "Reset zoom & pan :"
    eval {button $f.bReset -text R -width 3 \
        -command "CompareMosaikResetZoom; CompareRenderMosaik"} $Gui(WBA)
    pack $f.lReset $f.bReset -side left -padx $Gui(pad)  -pady $Gui(pad) -fill x


    #-------------------------------------------
    # Flip frame
    #---------------------------------

    set fFlip $Module(CompareModule,fFlip)
    set f $fFlip

    frame $f.fVolume   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fFlippers    -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fVolume $f.fFlippers\
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Flip->Volume frame
    #---------------------------------

    set f $f.fVolume

    eval {label $f.lVolume -text "Volume to flip"} $Gui(WTA)
    eval {menubutton $f.mbVolume -text None -width 13 \
        -menu $f.mbVolume.m} $Gui(WMBA)

    eval {menu $f.mbVolume.m} $Gui(WMA)

    TooltipAdd $f.mbVolume "Volume Selection: choose a volume\
        to flip"

    pack $f.lVolume $f.mbVolume \
        -pady $Gui(pad) -padx $Gui(pad) -side top

    #-------------------------------------------
    # Flip->Flippers frame
    #---------------------------------

    set f $fFlip.fFlippers

    DevAddButton $f.bFlipRL "flip R/L" "CompareFlipApply RL" 20
    DevAddButton $f.bFlipAP "flip A/P" "CompareFlipApply AP" 20
    DevAddButton $f.bFlipSI "flip S/I" "CompareFlipApply SI" 20

    pack $f.bFlipRL $f.bFlipAP $f.bFlipSI -side top -fill x -padx $Gui(pad) -pady $Gui(pad)

    # load compare viewer GUI
    CompareViewerBuildGUI
    # sets original orientation (should be called somewhere else)
    CompareSlicesSetOrientAll "Sagittal"
    CompareMosaikSetOrient "Sagittal"
}

#-------------------------------------------------------------------------------
# .PROC CompareModuleBuildVTK
# Set label indirect LUT to display label maps and have the MultiSlicer use this
# NoneVolume instead of its own creation
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleBuildVTK {} {
  global MultiSlicer Lut Volume CompareInteractor

  vtkMrmlMultiSlicer MultiSlicer

  MultiSlicer SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)

  set v $Volume(idNone)
  MultiSlicer SetNoneVolume Volume($v,vol)

  set CompareInteractor(activeSlicer) MultiSlicer
}

#-------------------------------------------------------------------------------
# .PROC CompareModuleEnter
# Called when this module is entered by the user.
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc CompareModuleEnter {} {
    global CompareModule CompareViewer CompareGui

    CompareViewerSetMode $CompareViewer(mode)
    wm deiconify $CompareGui(tCompareViewer)
    CompareRenderSlices

    #Update ENST logo
    set modulepath $::PACKAGE_DIR_VTKCOMPAREMODULE/../../../images
    if {[file exist [ExpandPath [file join \
                     $modulepath "slicer_ENST_CNRS_GET_logo.ppm"]]]} {
        image create photo iWelcome \
        -file [ExpandPath [file join $modulepath "slicer_ENST_CNRS_GET_logo.ppm"]]
    }

}

#-------------------------------------------------------------------------------
# .PROC CompareModuleExit
# Called when this module is exited by the user.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleExit {} {

    #Restore standard slicer logo
    image create photo iWelcome \
        -file [ExpandPath [file join gui "welcome.ppm"]]
}


#-------------------------------------------------------------------------------
# .PROC CompareModuleSetLinking
# Called when "on" or "off" linking button is pressed. Update display, especially
# when setting linked display.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleSetLinking {} {
  global Module View CompareSlice CompareViewer

  if {$CompareViewer(linked) == "on"} {

    # enables linking controls and disables orientation/offset slice controls
    CompareModuleEnableLinkControls
    CompareSlicesEnableControls

    # set all orientations to the value displayed in the orientation menu button
    set mOrient ${Module(CompareModule,fDisplay)}.fLinking.fOrientation.fChooseOrient.mbOrient
    set cget  "-text"
    set value [eval $mOrient cget $cget]
    CompareSlicesSetOrientAll $value

    # Set offset increment to 1 on every slice
    set CompareSlice(offsetIncrement) 1
    foreach s $CompareSlice(idList) {
      set CompareSlice($s,offsetIncrement) 1
    }

  } else {
    # disable controls if linked mode is off
    CompareModuleEnableLinkControls
    CompareSlicesEnableControls
  }
  CompareRenderSlices
}

#-------------------------------------------------------------------------------
# .PROC CompareModuleResetOffsets
# Set offsets to 0 for every slice, in any direction.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleResetOffsets {} {
  global MultiSlicer CompareSlice

  foreach s $CompareSlice(idList) {
     foreach orient "[MultiSlicer GetOrientList]" {
        MultiSlicer SetOrientString $s $orient
        MultiSlicer SetOffset $s 0
     }
  }
}

#-------------------------------------------------------------------------------
# .PROC CompareModuleEnableLinkControls
# Depending on linking is "on" or "off", enables or disables linking controls
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareModuleEnableLinkControls {} {
  global CompareViewer Module

  if {$CompareViewer(linked) == "on"} {
    $Module(CompareModule,fDisplay).fLinking.fOffset.fSlider.eOffset \
    configure -state normal
    $Module(CompareModule,fDisplay).fLinking.fOffset.fSlider.sOffset \
    configure -state active
    $Module(CompareModule,fDisplay).fLinking.fOffsetIncrement.eIncrement \
    configure -state normal
    $Module(CompareModule,fDisplay).fLinking.fOrientation.fChooseOrient.mbOrient \
    configure -state normal
  } else {
    $Module(CompareModule,fDisplay).fLinking.fOffset.fSlider.eOffset \
    configure -state disabled
    $Module(CompareModule,fDisplay).fLinking.fOffset.fSlider.sOffset \
    configure -state disabled
    $Module(CompareModule,fDisplay).fLinking.fOffsetIncrement.eIncrement \
    configure -state disabled
    $Module(CompareModule,fDisplay).fLinking.fOrientation.fChooseOrient.mbOrient \
    configure -state disabled
  }
}
