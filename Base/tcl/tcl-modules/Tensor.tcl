#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        Tensor.tcl
# PROCEDURES:  
#   TensorInit
#   TensorUpdateMRML
#   TensorEnter
#   TensorExit
#   TensorBuildGUI
#   TensorRaiseFrame
#   TensorBuildScrolledGUI
#   TensorCheckScrollLimits
#   TensorSizeScrolledGUI
#   TensorSetPropertyType
#   TensorPropsApply
#   TensorPropsCancel
#   TensorAdvancedApply
#   TensorSetFileName
#   TensorUpdateScalarBar
#   TensorShowScalarBar
#   TensorHideScalarBar
#   TensorSelect x y z
#   TensorUpdateThreshold
#   TensorUpdateMaskLabel
#   TensorUpdateGlyphEigenvector
#   TensorUpdateGlyphColor
#   TensorUpdateGlyphScalarRange
#   TensorUpdateActor
#   TensorUpdate
#   TensorUpdateMathParams
#   TensorCreateEmptyVolume
#   TensorDoMath
#   TensorGrabVol
#   TensorApplyVisualizationParameters
#   TensorMakeVTKObject
#   TensorAddObjectProperty
#   TensorBuildVTK
#   annotatePick
#   ConvertVolumeToTensors
#   TensorRecalculateTensors
#   TensorWriteStructuredPoints
#   TensorCalculateActorMatrix transform t
#   TensorSpecificVisualizationSettings
#   TensorResetDefaultVisualizationSettings
#   TensorApplyVisualizationSettings mode
#   TensorSetOperation math
#   TensorAddStreamline
#   TensorDoDiffusionNot
#   TensorDoDiffusion
#   TensorPreprocess
#   TensorRoundFloatingPoint
#   TensorRemoveAllActors
#   TensorRemoveAllStreamlines
#   TensorSeedStreamlinesFromSegmentation
#   TensorExecuteForProgrammableFilter
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC TensorInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorInit {} {
    global Tensor Module Volume
    
    # Define Tabs
    #------------------------------------
    set m Tensor
    set Module($m,row1List) "Help Display ROI Scalars Props"
    set Module($m,row1Name) "{Help} {Disp} {ROI} {Scalars} {Props}"
    set Module($m,row1,tab) Display
    # Use these lines to add a second row of tabs
    set Module($m,row2List) "Diffuse Advanced Devel"
    set Module($m,row2Name) "{Diffuse} {VTK} {Devel}"
    set Module($m,row2,tab) Advanced
    
    # Define Procedures
    #------------------------------------
    set Module($m,procGUI) TensorBuildGUI
    set Module($m,procMRML) TensorUpdateMRML
    set Module($m,procVTK) TensorBuildVTK
    set Module($m,procEnter) TensorEnter
    set Module($m,procExit) TensorExit
   
    # Define Dependencies
    #------------------------------------
    set Module($m,depend) ""
    
    # Set Version Info
    #------------------------------------
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.7 $} {$Date: 2002/02/11 23:47:38 $}]
    
    # Props: GUI tab we are currently on
    #------------------------------------
    set Tensor(propertyType) Basic
    
    # Initial path to search when loading files
    #------------------------------------
    set Tensor(DefaultDir) ""
    # Lauren for testing only
    set Tensor(DefaultDir) "/export/home/odonnell/data/"

    # Lauren: should vis be in tcl or C++?
    #------------------------------------
    # Visualization-related variables
    #------------------------------------

    # type of settings in 3d view, anno, etc
    set Tensor(mode,visualizationSettingsType) default
    set Tensor(mode,visualizationSettingsTypeList) {tensors default}
    set Tensor(mode,visualizationSettingsTypeList,tooltips) [list \
	    "Settings to display tensors well: black background, etc." \
	    "Reset to Slicer default settings."  \
	    ]

    # type of reformatting
    set Tensor(mode,reformatType) 0
    set Tensor(mode,reformatTypeList) {None 0 1 2}
    set Tensor(mode,reformatTypeList,tooltips) [list \
	    "No reformatting: display all tensors." \
	    "Reformat tensors along with slice 0."  \
	    "Reformat tensors along with slice 1."  \
	    "Reformat tensors along with slice 2."  \
	    ]

    # type of visualization to use
    set Tensor(mode,visualizationType) None
    set Tensor(mode,visualizationTypeList) {None Glyphs Tracks}
    set Tensor(mode,visualizationTypeList,tooltip) "To display tensors, select the type of visualization you would like.\nSettings for this type of visualization will appear below."

    # type of glyph to display (default to lines since fastest)
    set Tensor(mode,glyphType) Lines
    set Tensor(mode,glyphTypeList) {Axes Lines Ellipses}
    set Tensor(mode,glyphTypeList,tooltips) {{Display tensors as 3 axes aligned with eigenvectors and scaled by eigenvalues.} {Display tensors as lines aligned with one eigenvector and scaled by its eigenvalue.} {Display tensors as ellipses aligned with eigenvectors and scaled by eigenvalues.}}

    # type of eigenvector to draw glyph lines for
    set Tensor(mode,glyphEigenvector) Max
    set Tensor(mode,glyphEigenvectorList) {Max Middle Min}
    set Tensor(mode,glyphEigenvectorList,tooltips) {{When displaying tensors as Lines, use the eigenvector corresponding to the largest eigenvalue.} {When displaying tensors as Lines, use the eigenvector corresponding to the middle eigenvalue.} {When displaying tensors as Lines, use the eigenvector corresponding to the smallest eigenvalue.}}

    # type of glyph coloring
    set Tensor(mode,glyphColor) Linear
    set Tensor(mode,glyphColorList) {Linear Planar Spherical Max Middle Min MaxMinusMiddle RA FA Direction}
    set Tensor(mode,glyphColorList,tooltip) {Color tensors according to Linear, Planar, or Spherical measures, with the Max, Middle, or Min eigenvalue, with relative or fractional anisotropy (RA or FA), or by direction of major eigenvector.}

    # How to handle display of colors: like W/L but scalar range
    set Tensor(mode,glyphScalarRange) Auto
    set Tensor(mode,glyphScalarRangeList) {Auto Manual}
    set Tensor(mode,glyphScalarRangeList,tooltips) [list \
	    "Scalar range will be set to max and min scalar in the data." \
	    "User-adjustable scalar range to highlight areas of interest (like window/level does)."]
    # slider min/max values
    set Tensor(mode,glyphScalarRange,min) 0
    set Tensor(mode,glyphScalarRange,max) 10
    # slider current settings
    set Tensor(mode,glyphScalarRange,low) 0
    set Tensor(mode,glyphScalarRange,hi) 1

    # whether to reformat tensors along with slices
    set Tensor(mode,reformat) 0

    # display properties of the actors
    set Tensor(actor,ambient) 1
    set Tensor(actor,diffuse) .2
    set Tensor(actor,specular) .4

    # Whether to remove actors on module exit
    set Tensor(vtk,glyphs,actor,remove) 1
    set Tensor(vtk,streamln,actor,remove) 1

    # scalar bar
    set Tensor(mode,scalarBar) Off
    set Tensor(mode,scalarBarList) {On Off}
    set Tensor(mode,scalarBarList,tooltips) [list \
	    "Display a scalar bar to show correspondence between numbers and colors." \
	    "Do not display the scalar bar."]


    #------------------------------------
    # Variables vtk object creation
    #------------------------------------
    
    # ID of the last streamline created
    set Tensor(vtk,streamline,currentID) -1
    set Tensor(vtk,streamline,idList) ""

    #------------------------------------
    # Variables for preprocessing
    #------------------------------------

    # upper and lower values allowable when thresholding
    set Tensor(thresh,threshold,rangeLow) 0
    set Tensor(thresh,threshold,rangeHigh) 500

    # type of thresholding to use to reduce number of tensors
    set Tensor(mode,threshold) None
    set Tensor(mode,thresholdList) {None Trace Anisotropy}
    set Tensor(mode,thresholdList,tooltips) {{No thresholding.  Display all tensors.} {Only display tensors where the trace is between the threshold values.}  {Only display tensors where the anisotropy is between the threshold values.}}

    # type of masking to use to reduce volume of tensors
    set Tensor(mode,mask) None
    set Tensor(mode,maskList) {None MaskWithLabelmap}
    set Tensor(mode,maskList,tooltips) {{No masking.  Display all tensors.} {Only display tensors where the mask labelmap shows the chosen label value.}}
    set Tensor(mode,maskLabel,tooltip) "The ROI colored with this label will be used to mask tensors.  Tensors will be shown only inside the ROI."

    # labelmap to use as mask
    set Tensor(MaskLabelmap) $Volume(idNone)

    #------------------------------------
    # Variables for producing scalar volumes
    #------------------------------------

    # math op to produce scalars from tensors
    set Tensor(scalars,operation) Trace
    set Tensor(scalars,operationList) [list Trace Determinant \
	    RelativeAnisotropy FractionalAnisotropy LinearAnisotropy \
	    PlanarAnisotropy SphericalAnisotropy MaxEigenvalue \
	    MiddleEigenvalue MinEigenvalue ColorByOrientation D11 D22 D33]
    set Tensor(scalars,operationList,tooltip) "Produce a scalar volume from tensor data.\nTrace, Determinant, Anisotropy, and Eigenvalues produce grayscale volumes,\nwhile Orientation produces a 3-component (Color) volume that is best viewed in the 3D window."

    # whether to compute vol from ROI or whole tensor volume
    set Tensor(scalars,ROI) None
    set Tensor(scalars,ROIList) {None Threshold Mask}
    set Tensor(scalars,ROIList,tooltips) {"No ROI: derive the scalar volume from the entire tensor volume." "Use the thresholded area defined in the ROI tab to mask the tensor volume before scalar volume creation." "Use the mask labelmap volume defined in the ROI tab to mask the tensor volume before scalar volume creation."}


    #------------------------------------
    # Event bindings
    #------------------------------------
    lappend Tensor(eventManager) {$Gui(fSl0Win) <KeyPress-l> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    {  eval TensorSelect $Select(xyz); Render3D } } }
    lappend Tensor(eventManager) {$Gui(fSl1Win) <KeyPress-l> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    {  eval TensorSelect $Select(xyz);Render3D } } }
    
    lappend Tensor(eventManager) {$Gui(fSl2Win) <KeyPress-l> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval TensorSelect $Select(xyz);Render3D } } }

    lappend Tensor(eventManager) {$Gui(fViewWin)  <KeyPress-l> \
	    { if { [SelectPick Tensor(vtk,picker) %W %x %y] != 0 } \
	    { eval TensorSelect $Select(xyz);Render3D } } }

    #------------------------------------
    # Developers panel variables
    #------------------------------------
    set Tensor(devel,fileName) "tensors.vtk"
    set tmp "\
	    {1 0 0 0}  \
	    {0 1 0 0}  \
	    {0 0 1 0}  \
	    {0 0 0 1}  "
    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
	foreach col $cols {
	    set Tensor(recalculate,userMatrix,$row,$col) \
		    [lindex [lindex $tmp $row] $col]
	} 
    }

    #------------------------------------
    # Diffusion panel variables
    #------------------------------------


    #------------------------------------
    # Number display variables
    #------------------------------------
    # round floats to 8 digits
    set Tensor(floatingPointFormat) "%0.5f"

    

    # Lauren fix this
    set Tensor(Description) ""
    set Tensor(Name) ""
}

################################################################
#  Procedures called automatically by the slicer
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateMRML {} {
    puts "Lauren in TensorUpdateMRML"
}

#-------------------------------------------------------------------------------
# .PROC TensorEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorEnter {} {
    global Tensor Slice
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $Tensor(eventManager)

    # configure all scrolled GUIs so frames inside fit
    TensorSizeScrolledGUI $Tensor(scrolledGui,advanced)

    # color label selection widgets
    LabelsColorWidgets

    # Add our actors but make them invisible until needed
    MainAddActor Tensor(vtk,glyphs,actor)
    MainAddActor Tensor(vtk,streamln,actor)
    Tensor(vtk,glyphs,actor) VisibilityOff
    Tensor(vtk,streamln,actor) VisibilityOff

    # Default to reformatting along with the currently active slice
    set Tensor(mode,reformatType) $Slice(activeID)

}

#-------------------------------------------------------------------------------
# .PROC TensorExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorExit {} {
    global Tensor
    
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager


    TensorRemoveAllActors

    # make 3D slices opaque now
    #MainSlicesReset3DOpacity
}

################################################################
#  Procedures for building the GUI
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorBuildGUI
# Builds the GUI panel.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildGUI {} {
    global Module Gui Tensor Label Volume

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    #    Active
    #       VisMethods
    #          VisParams
    #             None
    #             Glyphs
    #             Tracks
    #       VisUpdate
    # ROI
    # Scalars
    # Props
    # Advanced
    # Diffuse
    # Devel
    #-------------------------------------------

    #puts "Lauren in TensorBuildGUI, fix the frame hierarchy comment"

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    This module sucks
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Basic:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags Tensor $help
    MainHelpBuildGUI Tensor
    
    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Tensor,fDisplay)
    set f $fDisplay

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fSettings  -bg $Gui(activeWorkspace)
    pack $f.fSettings -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fReformat  -bg $Gui(activeWorkspace)
    pack $f.fReformat -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fVisMethods  -bg $Gui(activeWorkspace)
    pack $f.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true
    #$f.fVisMethods config -relief groove -bd 3

    foreach frame "VisUpdate" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    
    #-------------------------------------------
    # Display->Active frame
    #-------------------------------------------
    set f $fDisplay.fActive

    # menu to select active tensor
    DevAddSelectButton  Tensor $f Active "Active Tensor:" Grid \
	    "Active Tensor" 13 BLA
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Display->Settings frame
    #-------------------------------------------
    set f $fDisplay.fSettings

    DevAddLabel $f.l "3D View Settings:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $Tensor(mode,visualizationSettingsTypeList) tip $Tensor(mode,visualizationSettingsTypeList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,visualizationSettingsType) \
		-command {TensorApplyVisualizationSettings} \
		-indicatoron 0} $Gui(WCA) 
	pack $f.rMode$vis -side left -padx 0 -pady 0
	TooltipAdd  $f.rMode$vis $tip
    }   

    #-------------------------------------------
    # Display->Reformat frame
    #-------------------------------------------
    set f $fDisplay.fReformat

    DevAddLabel $f.l "Reformat With Slice:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    set colors [list $Gui(activeWorkspace) $Gui(slice0) $Gui(slice1) $Gui(slice2)]
    set widths [list 4 2 2 2]

    foreach vis $Tensor(mode,reformatTypeList) tip $Tensor(mode,reformatTypeList,tooltips) color $colors width $widths {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,reformatType) \
		-command {TensorUpdate} \
		-indicatoron 0} $Gui(WCA) \
		{-bg $color -width $width}
	pack $f.rMode$vis -side left -padx 0 -pady 0
	TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods

    frame $f.fVisMode -bg $Gui(activeWorkspace) 
    pack $f.fVisMode -side top -padx 0 -pady $Gui(pad) -fill x

    # note the height is necessary to place frames inside later
    frame $f.fVisParams -bg $Gui(activeWorkspace) -height 338
    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    $f.fVisParams config -relief groove -bd 3

    #-------------------------------------------
    # Display->VisMethods->VisMode frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods.fVisMode

    eval {label $f.lVis -text "Visualization Method: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $Tensor(mode,visualizationType) \
	    -relief raised -bd 2 -width 20 \
	    -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $Tensor(mode,visualizationTypeList) {
	$f.mbVis.m add command -label $vis \
		-command "TensorRaiseFrame $vis; set Tensor(mode,visualizationType) $vis; TensorUpdate"
    }
    # save menubutton for config
    set Tensor(gui,mbVisMode) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $Tensor(mode,visualizationTypeList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods.fVisParams
    set fParams $f

    # make a parameters frame for each visualization type
    # types are: None Glyphs Tracks
    foreach frame $Tensor(mode,visualizationTypeList) {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	# for raising one frame at a time
	place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
	#pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	set Tensor(frame,$frame) $f.f$frame
    }
    raise $Tensor(frame,$Tensor(mode,visualizationType))

    ##########################################################
    #  NONE  (frame raised when None is selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->None frame
    #-------------------------------------------
    set f $fParams.fNone

    DevAddLabel $f.l "Visualization is off."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    ##########################################################
    #  GLYPHS   (frame raised when Glyphs are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs frame
    #-------------------------------------------
    set f $fParams.fGlyphs

    foreach frame "Label GlyphType Lines Colors ScalarBar GlyphScalarRange Slider" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Label frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fLabel

    DevAddLabel $f.l "Glyph Visualization Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->GlyphType frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fGlyphType

    DevAddLabel $f.l "Glyph Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $Tensor(mode,glyphTypeList) tip $Tensor(mode,glyphTypeList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,glyphType) \
		-command {TensorUpdate} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 1
	TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Lines frame
    #-------------------------------------------

    set f $fParams.fGlyphs.fLines

    DevAddLabel $f.l "Line Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $Tensor(mode,glyphEigenvectorList) tip $Tensor(mode,glyphEigenvectorList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,glyphEigenvector) \
		-command TensorUpdateGlyphEigenvector \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 1
	TooltipAdd $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Colors frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fColors

    eval {label $f.lVis -text "Color by: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $Tensor(mode,glyphColor) \
	    -relief raised -bd 2 -width 12 \
	    -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $Tensor(mode,glyphColorList) {
	$f.mbVis.m add command -label $vis \
		-command "set Tensor(mode,glyphColor) $vis; TensorUpdateGlyphColor"
    }
    # save menubutton for config
    set Tensor(gui,mbGlyphColor) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $Tensor(mode,glyphColorList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->ScalarBar frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fScalarBar

    DevAddLabel $f.l "Scalar Bar:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $Tensor(mode,scalarBarList) tip $Tensor(mode,scalarBarList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,scalarBar) \
		-command {TensorUpdateScalarBar} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 1
	TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->GlyphScalarRange frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fGlyphScalarRange

    DevAddLabel $f.l "Scalar Range:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $Tensor(mode,glyphScalarRangeList) tip $Tensor(mode,glyphScalarRangeList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,glyphScalarRange) \
		-command {TensorUpdateGlyphScalarRange; Render3D} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 1
	TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Slider frame
    #-------------------------------------------
    foreach slider "Low Hi" text "Lo Hi" {

	set f $fParams.fGlyphs.fSlider

	frame $f.f$slider -bg $Gui(activeWorkspace)
	pack $f.f$slider -side top -padx $Gui(pad) -pady 1
	set f $f.f$slider

	eval {label $f.l$slider -text "$text:"} $Gui(WLA)
	eval {entry $f.e$slider -width 10 \
		-textvariable Tensor(mode,glyphScalarRange,[Uncap $slider])} \
		$Gui(WEA)
	eval {scale $f.s$slider -from $Tensor(mode,glyphScalarRange,min) \
		-to $Tensor(mode,glyphScalarRange,max) \
		-length 90 \
		-variable Tensor(mode,glyphScalarRange,[Uncap $slider]) \
		-resolution 0.1 \
		-command {TensorUpdateGlyphScalarRange; Render3D}} \
		$Gui(WSA) {-sliderlength 15}
	pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
	set Tensor(gui,slider,$slider) $f.s$slider
	bind $f.e${slider} <Return>   \
		"TensorUpdateGlyphScalarRange ${slider}; Render3D"

    }

    ##########################################################
    #  TRACKS   (frame raised when Tracks are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracks frame
    #-------------------------------------------
    set f $fParams.fTracks

    foreach frame "Label" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracks->Label frame
    #-------------------------------------------
    set f $fParams.fTracks.fLabel

    DevAddLabel $f.l "Tracks Visualization Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Display->VisUpdate frame
    #-------------------------------------------
    #set f $fDisplay.fVisUpdate
    #DevAddButton $f.bTest "Junk" {puts "this button is junk"} 4    
    #pack $f.bTest -side top -padx 0 -pady 0


    #-------------------------------------------
    # ROI frame
    #-------------------------------------------
    set fROI $Module(Tensor,fROI)
    set f $fROI

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "Threshold Mask" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
	$f.f$frame config -relief groove -bd 3
    }

    #-------------------------------------------
    # ROI->Active frame
    #-------------------------------------------
    set f $fROI.fActive

    # menu to select active tensor
    DevAddSelectButton  Tensor $f Active "Active Tensor:" Grid \
	    "Active Tensor" 13 BLA
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # ROI->Threshold frame
    #-------------------------------------------
    set f $fROI.fThreshold

    foreach frame "Label Mode Slider" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # ROI->Threshold->Label frame
    #-------------------------------------------
    set f $fROI.fThreshold.fLabel

    DevAddLabel $f.l "Tensor Threshold Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Threshold->Mode frame
    #-------------------------------------------
    set f $fROI.fThreshold.fMode

    DevAddLabel $f.l "Value:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $Tensor(mode,thresholdList) tip $Tensor(mode,thresholdList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,threshold) \
		-command {TensorUpdate} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 0
	TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # ROI->Threshold->Slider frame
    #-------------------------------------------
    foreach slider "Lower Upper" text "Lo Hi" {

	set f $fROI.fThreshold.fSlider

	frame $f.f$slider -bg $Gui(activeWorkspace)
	pack $f.f$slider -side top -padx $Gui(pad) -pady $Gui(pad)
	set f $f.f$slider

	eval {label $f.l$slider -text "$text:"} $Gui(WLA)
	eval {entry $f.e$slider -width 6 \
		-textvariable Tensor(thresh,threshold,[Uncap $slider])} \
		$Gui(WEA)
	bind $f.e$slider <Return>   "TensorUpdateThreshold"
	#bind $f.e$slider <FocusOut> "EdThresholdUpdate; RenderActive;"
	eval {scale $f.s$slider -from $Tensor(thresh,threshold,rangeLow) \
		-to $Tensor(thresh,threshold,rangeHigh) \
		-length 130 \
		-variable Tensor(thresh,threshold,[Uncap $slider]) \
		-resolution 0.1 \
		-command {TensorUpdateThreshold}} \
		$Gui(WSA) {-sliderlength 15}
	#grid $f.l$slider $f.e$slider -padx 2 -pady 2 -sticky w
	#grid $f.l$slider -sticky e
	#grid $f.s$slider -columnspan 2 -pady 2 
	pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
	set Tensor(gui,slider,$slider) $f.s$slider
    }


    #-------------------------------------------
    # ROI->Mask frame
    #-------------------------------------------
    set f $fROI.fMask

    foreach frame "Label Mode Volume ChooseLabel" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # ROI->Mask->Label frame
    #-------------------------------------------
    set f $fROI.fMask.fLabel

    DevAddLabel $f.l "Tensor Mask Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Mask->Mode frame
    #-------------------------------------------
    set f $fROI.fMask.fMode

    DevAddLabel $f.l "Mask:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $Tensor(mode,maskList) tip $Tensor(mode,maskList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(mode,mask) \
		-command {TensorUpdate} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 0
	TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # ROI->Mask->Volume frame
    #-------------------------------------------
    set f $fROI.fMask.fVolume

    # menu to select a volume: will set Volume(activeID)
    set name MaskLabelmap
    DevAddSelectButton  Tensor $f $name "Mask Labelmap:" Grid \
	    "Select a labelmap volume to use as a mask.\nTensors will be displayed only where the label matches the label you select below." \
	    13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # ROI->Mask->ChooseLabel frame
    #-------------------------------------------
    set f $fROI.fMask.fChooseLabel

    # mask label
    eval {button $f.bOutput -text "Label:" \
	    -command "ShowLabels TensorUpdateMaskLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
	    -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "TensorUpdateMaskLabel"
    bind $f.eOutput <FocusOut> "TensorUpdateMaskLabel"
    eval {entry $f.eName -width 14 \
	    -textvariable Label(name)} $Gui(WEA) \
	    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    TooltipAdd  $f.bOutput $Tensor(mode,maskLabel,tooltip)
    TooltipAdd  $f.eOutput $Tensor(mode,maskLabel,tooltip)
    TooltipAdd  $f.eName $Tensor(mode,maskLabel,tooltip)

    #-------------------------------------------
    # Scalars frame
    #-------------------------------------------
    set fScalars $Module(Tensor,fScalars)
    set f $fScalars
    
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "ChooseOutput UseROI Apply" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Scalars->Active frame
    #-------------------------------------------
    set f $fScalars.fActive

    # menu to select active tensor
    DevAddSelectButton  Tensor $f Active "Active Tensor:" Grid \
	    "Active Tensor" 13 BLA
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Scalars->ChooseOutput frame
    #-------------------------------------------
    set f $fScalars.fChooseOutput

    eval {label $f.lMath -text "Create Volume: "} $Gui(WLA)
    eval {menubutton $f.mbMath -text $Tensor(scalars,operation) \
	    -relief raised -bd 2 -width 12 \
	    -menu $f.mbMath.m} $Gui(WMBA)
    eval {menu $f.mbMath.m} $Gui(WMA)
    pack $f.lMath $f.mbMath -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach math $Tensor(scalars,operationList) {
	$f.mbMath.m add command -label $math \
		-command "TensorSetOperation $math"
    }
    # save menubutton for config
    set Tensor(gui,mbMath) $f.mbMath
    # Add a tooltip
    TooltipAdd $f.mbMath $Tensor(scalars,operationList,tooltip)

    #-------------------------------------------
    # Scalars->UseROI frame
    #-------------------------------------------
    set f $fScalars.fUseROI

    DevAddLabel $f.l "ROI:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $Tensor(scalars,ROIList) tip $Tensor(scalars,ROIList,tooltips) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(scalars,ROI) \
		-command TensorUpdateMathParams \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 0
	TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # Scalars->Apply frame
    #-------------------------------------------
    set f $fScalars.fApply

    DevAddButton $f.bApply "Apply" "TensorDoMath" 4    
    pack $f.bApply -side top -padx 0 -pady 0

    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(Tensor,fProps)
    set f $fProps
    
    foreach frame "Top Middle Bottom Convert" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Props->Top frame
    #-------------------------------------------
    set f $fProps.fTop
    # file browse box: widget, array, var, label, proc, "", startdir
    DevAddFileBrowse $f Tensor FileName "File" \
	    TensorSetFileName "" {\$Tensor(defaultDir)}
    # Lauren does default dir work?

    #-------------------------------------------
    # Props->Bottom frame
    #-------------------------------------------
    set f $fProps.fBottom
    # Lauren test
    DevAddButton $f.bTest "Add" MainTensorAddTensor 8

    DevAddButton $f.bApply "Apply" "TensorPropsApply; Render3D" 8
    DevAddButton $f.bCancel "Cancel" "TensorPropsCancel" 8

    # Lauren test
    grid $f.bTest $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)
    #grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Props->Convert frame
    #-------------------------------------------
    set f $fProps.fConvert

    foreach frame "Select Apply" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    #-------------------------------------------
    # Props->Convert->Select frame
    #-------------------------------------------
    set f $fProps.fConvert.fSelect
    # Lauren test
    # menu to select a volume: will set Volume(activeID)
    DevAddSelectButton  Volume $f Active "Input Volume:" Grid \
	    "Input Volume to create tensors from." 13 BLA

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Props->Convert->Apply frame
    #-------------------------------------------
    set f $fProps.fConvert.fApply
    DevAddButton $f.bTest "Convert Volume" ConvertVolumeToTensors 20
    pack $f.bTest -side top -padx 0 -pady $Gui(pad) -fill x

    #-------------------------------------------
    # Advanced frame
    #-------------------------------------------
    set fAdvanced $Module(Tensor,fAdvanced)
    set f $fAdvanced
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Advanced->Top frame
    #-------------------------------------------
    set f $fAdvanced.fTop
    DevAddLabel $f.l "VTK objects in the pipeline"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Advanced->Middle frame
    #-------------------------------------------
    set f $fAdvanced.fMiddle
    set fScrolled [TensorBuildScrolledGUI $f]
    set Tensor(scrolledGui,advanced) $fScrolled

    # loop through all the vtk objects and build GUIs
    #------------------------------------
    foreach o $Tensor(vtkObjectList) {

	set f $fScrolled

	# if the object has properties
	#-------------------------------------------
	if {$Tensor(vtkPropertyList,$o) != ""} {
	    
	    # make a new frame for this vtk object
	    #-------------------------------------------
	    frame $f.f$o -bg $Gui(activeWorkspace)
	    $f.f$o configure  -relief groove -bd 3 
	    pack $f.f$o -side top -padx $Gui(pad) -pady 2 -fill x
	    
	    # object name becomes the label for the frame
	    #-------------------------------------------
	    # Lauren we need an object description
	    # and also basic or advanced attrib
	    DevAddLabel $f.f$o.l$o [Cap $o]
	    pack $f.f$o.l$o -side top \
		    -padx 0 -pady 0
	}

	# loop through all the parameters for this object
	# and build appropriate user entry stuff for each
	#------------------------------------
	foreach p $Tensor(vtkPropertyList,$o) {

	    set f $fScrolled.f$o

	    # name of entire tcl variable
	    set variableName Tensor(vtk,$o,$p)
	    # its value is:
	    set value $Tensor(vtk,$o,$p)
	    # description of the parameter
	    set desc $Tensor(vtk,$o,$p,description)
	    # datatype of the parameter
	    set type $Tensor(vtk,$o,$p,type)

	    # make a new frame for this parameter
	    frame $f.f$p -bg $Gui(activeWorkspace)
	    pack $f.f$p -side top -padx 0 -pady 1 -fill x
	    set f $f.f$p

	    # see if value is a list (not used now)
	    #------------------------------------		
	    set length [llength $value]
	    set isList [expr $length > "1"]

	    # Build GUI entry boxes, etc
	    #------------------------------------		
	    switch $type {
		"int" {
		    eval {entry $f.e$p \
			    -width 5 \
			    -textvariable $variableName\
			} $Gui(WEA)
		    DevAddLabel $f.l$p $desc:
		    pack $f.l$p $f.e$p -side left \
			    -padx $Gui(pad) -pady 2
		}
		"float" {
		    eval {entry $f.e$p \
			    -width 5 \
			    -textvariable $variableName\
			} $Gui(WEA)
		    DevAddLabel $f.l$p $desc:
		    pack $f.l$p $f.e$p -side left \
			    -padx $Gui(pad) -pady 2
		}
		"bool" {
		    puts "bool: $variableName, $desc"
		    eval {checkbutton $f.r$p  \
			    -text $desc -variable $variableName \
			} $Gui(WCA)
		    
		    pack  $f.r$p -side left \
			    -padx $Gui(pad) -pady 2
		}
	    }
	    
	}
    }
    # end foreach vtk object in tensors object list

    # figure out how tall a row is in the above gui, to scroll
    # Lauren you are here
    #set Tensor(advancedScrolledGui,height) [winfo reqheight $fScrolled.f]

    # Here's a button with text "Apply" that calls "AdvancedApply"
    DevAddButton $fAdvanced.fMiddle.bApply Apply TensorAdvancedApply
    pack $fAdvanced.fMiddle.bApply -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Diffuse frame
    #-------------------------------------------
    set fDiffuse $Module(Tensor,fDiffuse)
    set f $fDiffuse
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Diffuse->Top frame
    #-------------------------------------------
    set f $fDiffuse.fTop
    # menu to select a volume: will set Volume(activeID)
    DevAddSelectButton  Volume $f Active "Input Volume:" Grid \
	    "Input Volume: heat distribution." 13 BLA

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Diffuse->Middle frame
    #-------------------------------------------
    set f $fDiffuse.fMiddle

    DevAddButton $f.bRun "Run Diffusion" {TensorDoDiffusion}
    pack $f.bRun -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Devel frame
    #-------------------------------------------
    set fDevel $Module(Tensor,fDevel)
    set f $fDevel
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Devel->Top frame
    #-------------------------------------------
    set f $fDevel.fTop

    DevAddButton $f.bSave "Save Structured Points" {TensorWriteStructuredPoints $Tensor(devel,fileName)}
    pack $f.bSave -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Devel->Middle frame
    #-------------------------------------------
    set f $fDevel.fMiddle
    frame $f.fEntry
    frame $f.fButton
    pack $f.fEntry $f.fButton -side top -padx $Gui(pad) -pady $Gui(pad)

    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
	set f $fDevel.fMiddle.fEntry
	frame $f.f$row
	pack $f.f$row -side top -padx $Gui(pad) -pady $Gui(pad)
	set f $f.f$row
	foreach col $cols {
	    eval {entry $f.e$col -width 5 \
			    -textvariable \
			    Tensor(recalculate,userMatrix,$row,$col) \
			} $Gui(WEA)
	    pack $f.e$col -side left -padx $Gui(pad) -pady 2	    
	}
    }    
    set f $fDevel.fMiddle.fButton
    DevAddButton $f.bApply "Recalculate Tensors" {TensorRecalculateTensors}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad) 
}


#-------------------------------------------------------------------------------
# .PROC TensorRaiseFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorRaiseFrame {mode} {
    global Tensor

    raise $Tensor(frame,$mode)
    focus $Tensor(frame,$mode)
}

#-------------------------------------------------------------------------------
# .PROC TensorBuildScrolledGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildScrolledGUI {f} {
    global Gui Tensor

    # window paths
    #-------------------------------------------
    set canvas $f.cGrid
    set s $f.sGrid
    
    # create canvas and scrollbar
    #-------------------------------------------
    canvas $canvas -yscrollcommand "$s set" -bg $Gui(activeWorkspace)
    eval "scrollbar $s -command \"TensorCheckScrollLimits $canvas yview\"\
	    $Gui(WSBA)"
    pack $s -side right -fill y
    pack $canvas -side top -fill both -pady $Gui(pad) -expand true
    
    set f $canvas.f
    #frame $f -bd 0 -bg $Gui(activeWorkspace)
    frame $f -bd 0
    $f configure  -relief groove -bd 3 
    
    # put the frame inside the canvas (so it can scroll)
    #-------------------------------------------
    $canvas create window 0 0 -anchor nw -window $f
    
    # y spacing important for calculation of frame height for scrolling
    set pady 2
    
    # save name of canvas to configure later
    set Tensor(scrolledGUI,$f,canvas) $canvas

    # return path to the frame for filling
    return $f
}


#-------------------------------------------------------------------------------
# .PROC TensorCheckScrollLimits
# This procedure allows scrolling only if the entire frame is not visible
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorCheckScrollLimits {args} {

    set canvas [lindex $args 0]
    set view   [lindex $args 1]
    set fracs [$canvas $view]

    if {double([lindex $fracs 0]) == 0.0 && \
	    double([lindex $fracs 1]) == 1.0} {
	return
    }
    eval $args
}

#-------------------------------------------------------------------------------
# .PROC TensorSizeScrolledGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSizeScrolledGUI {f} {
    global Tensor

    # see how tall our frame is these days
    set height [winfo reqheight  $f]    
    set canvas $Tensor(scrolledGUI,$f,canvas)

    # tell the canvas to scroll so we can see the whole frame
    $canvas config -scrollregion "0 0 1 $height"
}

#-------------------------------------------------------------------------------
# .PROC TensorSetPropertyType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSetPropertyType {} {
    global Module Tensor

    #Lauren need properties subframes for this to work
    #raise $Volume(f$Volume(propertyType))
    #focus $Volume(f$Volume(propertyType))

    # Lauren temporarily use this
    raise $Module(Tensor,f$Tensor(propertyType))
}

################################################################
#  Procedures called by the GUI: Apply, Cancel, etc.
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorPropsApply
#  Apply the settings from the GUI into the currently
#  active MRML node (this is NEW or an existing node).  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorPropsApply {} {
    global Tensor Module
    puts "Lauren in TensorPropsApply, note this only works for NEW currently"
    
    set d $Tensor(activeID)
    if {$d == ""} {return}

    # Check all user input
    #-------------------------------------------
    set errcode [MainTensorValidateUserInput]
    if {$errcode != "1"} {
	return $errcode
    }

    # If we are in the process of adding a new data object
    #--------------------------------------------------------
    if {$d == "NEW"} {

	# Create the node
	# Lauren HUGE HACK!
	# When we convert to tensors we are using a volume node
	# here for structured points files we use a tensor node
	# this should be only one kind of node
	set n [MainMrmlAddNode Tensor]
	#set n [MainMrmlAddNode Volume Tensor]
	set i [$n GetID]

	# Set everything up in the node.
	MainTensorSetAllVariablesToNode $i

	# Update MRML (this will read in the new data)
	MainUpdateMRML
        # If failed, then it's no longer in the idList
	# Lauren this is a bug that the user is not informed?
	# And we stay frozen...
        if {[lsearch $Tensor(idList) $i] == -1} {
	    puts "Lauren node doesn't exist, should unfreeze and fix volumes.tcltoo"
            return
        }

	# Lauren what is this for?
        set Tensor(freeze) 0

	# Activate the new data object
        MainDataSetActive Tensor $i

    } else {
	# do we do a re-read of the object if user changed certain things?
	# Lauren implement this?
	#MainTensorSetAllVariablesToNode
    }

    # Apply normal settings changed by user (no file reread needed)
    #--------------------------------------------------------
    # Lauren these are only things that can be changed,
    # need to sort all vars into change/no change categories
    # OR just need to do a reread after if certain things changed.
    #MainTensorSetAllVariablesToNode "update"

    # If we were adding a NEW object, unfreeze tabs now
    #--------------------------------------------------------
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }

    # Lauren do we need this?
    # Update pipeline
    #MainVolumesUpdate $m

    # Update MRML to apply changed settings
    #--------------------------------------------------------
    MainUpdateMRML
    puts "end of tensor props apply"
}

#-------------------------------------------------------------------------------
# .PROC TensorPropsCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorPropsCancel {} {
    global Tensor Module
    puts "Lauren in TensorPropsCancel: need to get out of freezer"

    set Tensor(freeze) 0
    # If we gave up on adding a NEW object, unfreeze tabs now
    #--------------------------------------------------------
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC TensorAdvancedApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorAdvancedApply {} {
    global Tensor    
    puts "Applying"
    TensorApplyVisualizationParameters    
    puts "done applying"
    
    # display our changes
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorSetFileName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSetFileName {} {
    global Tensor
    
    # Do nothing if the user cancelled
    if {$Tensor(FileName) == ""} {return}
    
    # Update the Default Directory
    set Tensor(DefaultDir) [file dirname $Tensor(FileName)]
    
    # Name the tensors based on the entered file.
    set Tensor(Name) [ file root [file tail $Tensor(FileName)]]
}


################################################################
#  Procedures to handle visualization interaction
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorUpdateScalarBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateScalarBar {} {
    global Tensor

    set mode $Tensor(mode,scalarBar)

    switch $mode {
	"On" {
	    TensorShowScalarBar
	}
	"Off" {
	    TensorHideScalarBar
	}
    }

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorShowScalarBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorShowScalarBar {} {
    TensorUpdateGlyphScalarRange
    Tensor(vtk,scalarBar,actor) VisibilityOn
    #viewRen AddProp Tensor(vtk,scalarBar,actor)
}

#-------------------------------------------------------------------------------
# .PROC TensorHideScalarBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorHideScalarBar {} {
    Tensor(vtk,scalarBar,actor) VisibilityOff
    #viewRen RemoveActor Tensor(vtk,scalarBar,actor)
}

#-------------------------------------------------------------------------------
# .PROC TensorSelect
# Given x,y,z in world coordinates, starts a streamline from that point
# in the active tensor dataset.
# .ARGS
# int x 
# int y
# int z 
# .END
#-------------------------------------------------------------------------------
proc TensorSelect {x y z} {
    global Tensor
    global Select

    puts "Select Picker  (x,y,z):  $x $y $z"

    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
	puts "TensorSelect: No tensors have been read into the slicer"
	return
    }

    # Use our world to ijk matrix information to correct x,y,z.
    # The streamline class doesn't know about the
    # tensor actor's UserMatrix.  We need to transform xyz by
    # the inverse of this matrix (almost) so the streamline will start 
    # from the right place in the tensors.
    #------------------------------------
    vtkTransform transform
    TensorCalculateActorMatrix transform $t    
    transform Inverse
    set point [transform TransformPoint $x $y $z]
    set x [lindex $point 0]
    set y [lindex $point 1]
    set z [lindex $point 2]
    transform Delete

    # make new hyperstreamline 
    #------------------------------------
    TensorAddStreamline
    set streamln streamln,$Tensor(vtk,streamline,currentID)

    # start pipeline (never use reformatted data here)
    #------------------------------------
    Tensor(vtk,$streamln) SetInput [Tensor($t,data) GetOutput]
    
    # start hyperstreamline here
    #------------------------------------
    Tensor(vtk,$streamln) SetStartPosition $x $y $z
    
    # Make actor visible now that it has inputs
    #------------------------------------
    MainAddActor Tensor(vtk,$streamln,actor) 
    Tensor(vtk,$streamln,actor) VisibilityOn

    # Put the output streamline's actor in the right place.
    # Just use the same matrix we use to position the tensors.
    #------------------------------------
    vtkTransform transform
    TensorCalculateActorMatrix transform $t
    Tensor(vtk,$streamln,actor) SetUserMatrix [transform GetMatrix]
    transform Delete

    # Force pipeline execution and render scene
    #------------------------------------
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC TensorUpdateThreshold
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateThreshold {{not_used ""}} {
    global Tensor
    
    Tensor(vtk,thresh,threshold)  ThresholdBetween \
	    $Tensor(thresh,threshold,lower) \
	    $Tensor(thresh,threshold,upper)

    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorUpdateMaskLabel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateMaskLabel {} {
    global Label

    LabelsFindLabel

    # this label becomes 1 in the mask
    set thresh Tensor(vtk,mask,threshold)
    $thresh ThresholdBetween $Label(label) $Label(label)

    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorUpdateGlyphEigenvector
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateGlyphEigenvector {} {
    global Tensor

    set mode $Tensor(mode,glyphEigenvector)

    # Scaling along x-axis corresponds to major 
    # eigenvector, etc.  So move the line to 
    # point along the proper axis for scaling
    switch $mode {
	"Max" {
	    Tensor(vtk,glyphs,line) SetPoint2 1 0 0    
	}
	"Middle" {
	    Tensor(vtk,glyphs,line) SetPoint2 0 1 0    
	}
	"Min" {
	    Tensor(vtk,glyphs,line) SetPoint2 0 0 1    
	}
    }
    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorUpdateGlyphColor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateGlyphColor {} {
    global Tensor

    set mode $Tensor(mode,glyphColor)
    
    # display new mode while we are working...
    $Tensor(gui,mbGlyphColor)	config -text $mode

    switch $mode {
	"Linear" {
	    Tensor(vtk,glyphs) ColorGlyphsWithLinearAnisotropy
	}
	"Planar" {
	    Tensor(vtk,glyphs) ColorGlyphsWithPlanarAnisotropy
	}
	"Spherical" {
	    Tensor(vtk,glyphs) ColorGlyphsWithSphericalAnisotropy
	}
	"Max" {
	    Tensor(vtk,glyphs) ColorGlyphsWithMaxEigenvalue
	}
	"Middle" {
	    Tensor(vtk,glyphs) ColorGlyphsWithMiddleEigenvalue
	}
	"Min" {
	    Tensor(vtk,glyphs) ColorGlyphsWithMinEigenvalue
	}
	"MaxMinusMiddle" {
	    Tensor(vtk,glyphs) ColorGlyphsWithMaxMinusMidEigenvalue
	}
	"RA" {
	    Tensor(vtk,glyphs) ColorGlyphsWithRelativeAnisotropy
	}
	"FA" {
	    Tensor(vtk,glyphs) ColorGlyphsWithFractionalAnisotropy
	}
	"Direction" {
	    Tensor(vtk,glyphs) ColorGlyphsWithDirection
	}

    }

    # Tell actor where to get scalar range
    set Tensor(mode,glyphScalarRange) Auto
    TensorUpdateGlyphScalarRange

    # Update pipelines
    Render3D

}

#-------------------------------------------------------------------------------
# .PROC TensorUpdateGlyphScalarRange
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateGlyphScalarRange {{not_used ""}} {
    global Tensor

    # make sure the pipeline is up-to-date so we get the right
    # scalar range.  Otherwise the first render will not have
    # the right glyph colors.
    Tensor(vtk,glyphs) Update

    set mode $Tensor(mode,glyphScalarRange)

    switch $mode {
	"Auto" {

	    scan [[Tensor(vtk,glyphs) GetOutput] GetScalarRange] \
		    "%f %f" \
		    Tensor(mode,glyphScalarRange,low) \
		    Tensor(mode,glyphScalarRange,hi) 

	    Tensor(vtk,glyphs,mapper) SetScalarRange \
	    $Tensor(mode,glyphScalarRange,low) \
		    $Tensor(mode,glyphScalarRange,hi) 

	}
	"Manual" {
	    Tensor(vtk,glyphs,mapper) SetScalarRange \
		    $Tensor(mode,glyphScalarRange,low) \
		    $Tensor(mode,glyphScalarRange,hi) 
	}
    }


    # Round the scalar range numbers to requested precision
    # This way -4e-12 will not look like a negative eigenvalue in
    # the GUI
    set Tensor(mode,glyphScalarRange,low) \
	    [TensorRoundFloatingPoint $Tensor(mode,glyphScalarRange,low)]
    set Tensor(mode,glyphScalarRange,hi) \
	    [TensorRoundFloatingPoint $Tensor(mode,glyphScalarRange,hi)]

    # If we are doing streamlines, tell them how to build LUT
    # Lauren why does this have little effect?  
    # need log lookup table?
    eval {Tensor(vtk,streamln,mapper)  SetScalarRange}  \
	    [[Tensor(vtk,glyphs) GetOutput] GetScalarRange] 

    # This causes multiple renders since for some reason
    # the scalar bar does not update on the first one
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorUpdateActor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateActor {actor} {
    global Tensor
    
    [$actor GetProperty] SetAmbient $Tensor(actor,ambient)
    [$actor GetProperty] SetDiffuse $Tensor(actor,diffuse)
    [$actor GetProperty] SetSpecular $Tensor(actor,specular)

}

#-------------------------------------------------------------------------------
# .PROC TensorUpdate
# The whole enchilada (if this were a vtk filter, this would be
# the Execute function...)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdate {} {
    global Tensor Slice Volume Label Gui

    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
	puts "TensorUpdate: Can't visualize Nothing"
	return
    }


    # reset progress text for any filter that uses the blue bar
    set Gui(progressText) "Working..."

    #------------------------------------
    # preprocessing pipeline
    #------------------------------------


    # threshold tensors if required
    #------------------------------------
    set mode $Tensor(mode,threshold)
    if {$mode != "None"} {
	
	puts "thresholding by $Tensor(mode,threshold)"
	set math Tensor(vtk,thresh,math)
	
	$math SetInput 0 [Tensor($t,data) GetOutput]
	$math SetOperationTo$Tensor(mode,threshold)
	
	set thresh Tensor(vtk,thresh,threshold)
	$thresh ThresholdBetween $Tensor(thresh,threshold,lower) \
		$Tensor(thresh,threshold,upper)

	# this line seems to be needed
	$thresh Update
	
	#Tensor(vtk,glyphs) SetScalarMask [$thresh GetOutput]
	# tell our filter to use this information
	#Tensor(vtk,glyphs) MaskGlyphsWithScalarsOn

	set mask Tensor(vtk,thresh,mask)
	$mask SetImageInput [Tensor($t,data) GetOutput]

	set dataSource [$mask GetOutput]
    } else {
	set dataSource [Tensor($t,data) GetOutput]
	#Tensor(vtk,glyphs) MaskGlyphsWithScalarsOff
    }

    # mask tensors if required
    #------------------------------------
    set mode $Tensor(mode,mask)
    if {$mode != "None"} {
	
	puts "masking by $Tensor(mode,mask)"
	# note it would be more efficient to 
	# combine the two masks (from thresh and here) 
	# but probably not both used at once 

	set thresh Tensor(vtk,mask,threshold)	
	$thresh ThresholdBetween $Label(label) $Label(label)
	set v $Volume(activeID)
	$thresh SetInput [Volume($v,vol) GetOutput]

	# this line seems to be needed
	$thresh Update
	
	set mask Tensor(vtk,mask,mask)
	# use output from above thresholding pipeline as input
	$mask SetImageInput $dataSource

	# set the dataSource to point to our output 
	# for the following pipelines
	set preprocessedSource [$mask GetOutput]
    } else {
	set preprocessedSource $dataSource
    }

    #------------------------------------
    # visualization pipeline
    #------------------------------------
    set mode $Tensor(mode,visualizationType)
    puts "Setting mode $mode for tensor $t"
    
    switch $mode {
	"Glyphs" {
	    puts "glyphs! $Tensor(mode,glyphType)"

	    # Find input to pipeline
	    #------------------------------------
	    set slice $Tensor(mode,reformatType)
	    if {$slice != "None"} {

		# We are reformatting a slice of glyphs
		Tensor(vtk,reformat) SetInput $preprocessedSource
		puts "Lauren, reformat resolution? image size?"

		# Lauren note that the FOV should be set based
		# on the characteristics of the tensor volume
		# when it is read in!
		# set FOV same as for volumes in the slicer?
		#set dim     [lindex [Volume($v,node) GetDimensions] 0]
		#set spacing [lindex [Volume($v,node) GetSpacing] 0]
		#set fov     [expr $dim*$spacing]
		#set View(fov) $fov
		#MainViewSetFov
		Tensor(vtk,reformat) SetFieldOfView [Slicer GetFieldOfView]

		set node Tensor($Tensor(activeID),node)
		Tensor(vtk,reformat) SetInterpolate [$node GetInterpolate]
		Tensor(vtk,reformat) SetWldToIjkMatrix [$node GetWldToIjk]
		
		# Lauren these should match the tensor resolution?
		# Use the extents to figure this out
		Tensor(vtk,reformat) SetResolution 128
		#Tensor(vtk,reformat) SetFieldOfView 128
		# Lauren this should hook in better
		# Follow active slice for now
		#set m [Slicer GetReformatMatrix $Slice(activeID)]
		set m [Slicer GetReformatMatrix $slice]
		Tensor(vtk,reformat) SetReformatMatrix $m
		set visSource [Tensor(vtk,reformat) GetOutput]
		
		# Want actor to be positioned with the slice
		Tensor(vtk,glyphs,actor) SetUserMatrix $m
		
	    } else {
		# We are displaying the whole volume of glyphs!
		set visSource $preprocessedSource
		
		# Want actor to be positioned in center with slices
		vtkTransform t1
		TensorCalculateActorMatrix t1 $Tensor(activeID)

		#t1 Identity
		#t1 PreMultiply
		#t1 SetMatrix [$node GetWldToIjk]
		#t1 Inverse
		#scan [$node GetSpacing] "%g %g %g" res_x res_y res_z
		##t1 PostMultiply
		#t1 PreMultiply
		#t1 Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] [expr 1.0 / $res_z]
		Tensor(vtk,glyphs,actor) SetUserMatrix [t1 GetMatrix]
		t1 Delete
	    }


	    # start pipeline
	    #------------------------------------
	    Tensor(vtk,glyphs) SetInput $visSource

	    # Use axes or ellipses
	    #------------------------------------
	    switch $Tensor(mode,glyphType) {
		"Axes" {
		    Tensor(vtk,glyphs) SetSource \
			    [Tensor(vtk,glyphs,axes) GetOutput]

		    # this is too slow, but might make nice pictures
		    #[Tensor(vtk,glyphs,tubeAxes) GetOutput]

		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs) GetOutput]
		}
		"Lines" {
		    Tensor(vtk,glyphs) SetSource \
			    [Tensor(vtk,glyphs,line) GetOutput]
		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs) GetOutput]
		}
		"Ellipses" {
		    Tensor(vtk,glyphs) SetSource \
			    [Tensor(vtk,glyphs,sphere) GetOutput]
		    # Lauren test
		    # way too expensive?
		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs,normals) GetOutput]
		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs) GetOutput]
		}
	    }

	    # Make actor visible
	    #------------------------------------
	    Tensor(vtk,glyphs,actor) VisibilityOn

	}
	"Tracks" {

	    puts "tracks!"
	    # Lauren this button is really only to access tractography
	    # GUI which should be its own thing anyway?

	}
	"None" {
	    puts "Turning off tensor visualization"

	    # make invisible so output
	    # not requested from pipeline anymore
	    #------------------------------------
	    Tensor(vtk,glyphs,actor) VisibilityOff
	    foreach id $Tensor(vtk,streamline,idList) {
		set streamline streamln,$id
		Tensor(vtk,$streamline,actor) VisibilityOff
	    }
	}
    }

    # config menubutton
    $Tensor(gui,mbVisMode)	config -text $mode


    # make sure the scalars are updated
    TensorUpdateGlyphScalarRange

    # update 3D window (causes pipeline update)
    Render3D
}

################################################################
#  Procedures used to derive scalar volumes from tensor data
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorUpdateMathParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateMathParams {} {
    global Tensor


    # Just check that if they requested a 
    # preprocessing step, that we are already
    # doing that step
    
    set mode $Tensor(scalars,ROI)

    set err "The $mode ROI is not currently being computed.  Please turn this feature on in the ROI tab before creating the volume."

    switch $mode {
	"None" {
	}
	"Threshold" {
	    if {$Tensor(mode,threshold)	== "None"} {
		set Tensor(scalars,ROI) None
		tk_messageBox -message $err
	    }
	}
	"Mask" {
	    if {$Tensor(mode,mask)	== "None"} {
		set Tensor(scalars,ROI) None
		tk_messageBox -message $err
	    }
	}
    }    

}

#-------------------------------------------------------------------------------
# .PROC TensorCreateEmptyVolume
# Just like DevCreateNewCopiedVolume, but uses a tensor node
# to copy parameters from instead of a volume node
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorCreateEmptyVolume {OrigId {Description ""} { VolName ""}} {
    global Volume

    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume]
    $newvol Copy Tensor($OrigId,node)
    
    # Set the Description and Name
    if {$Description != ""} {
        $newvol SetDescription $Description 
    }
    if {$VolName != ""} {
        $newvol SetName $VolName
    }

    # Create the volume (vtkMrmlDataVolume class)
    set n [$newvol GetID]
    MainVolumesCreate $n

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML

    return $n
}

#-------------------------------------------------------------------------------
# .PROC TensorDoMath
# Called to compute a scalar vol from tensors
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorDoMath {{operation ""}} {
    global Tensor Gui

    puts "creating volume"


    # if this was called from user input GUI menu
    if {$operation == ""} {
	set operation $Tensor(scalars,operation) 
    }

    # should use DevCreateNewCopiedVolume if have a vol node
    # to copy...
    set t $Tensor(activeID) 
    if {$t == "" || $t == $Tensor(idNone)} {
	puts "Can't do math if no tensor selected"
	return
    }
    set name [Tensor($t,node) GetName]
    set name ${operation}_$name
    set description "$operation volume derived from tensor volume $name"
    set v [TensorCreateEmptyVolume $t $description $name]

    puts "created volume $v"

    puts "pipeline"
    
    # find input
    set mode $Tensor(scalars,ROI)
    
    switch $mode {
	"None" {
	    set input [Tensor($t,data) GetOutput]
	}
	"Threshold" {
	    set input [Tensor(vtk,thresh,mask) GetOutput]
	}
	"Mask" {
	    set input [Tensor(vtk,mask,mask) GetOutput]
	}
    }

    vtkTensorMathematics math
    math SetInput 0 $input
    math SetOperationTo$operation
    math SetStartMethod      MainStartProgress
    math SetProgressMethod  "MainShowProgress math"
    math SetEndMethod        MainEndProgress
    set Gui(progressText) "Creating Volume $operation"

    # put the filter output into a slicer volume
    math Update
    # Lauren debug
    puts [[math GetOutput] Print]
    Volume($v,vol) SetImageData [math GetOutput]
    MainVolumesUpdate $v

    math SetInput 0 ""    
    math SetInput 1 ""    
    # this is to disconnect the pipeline
    # this object hangs around, so try this trick from Editor.tcl:
    math SetOutput ""
    math Delete
    
    # reset blue bar text
    set Gui(progressText) ""

    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC TensorGrabVol
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorGrabVol {filter} {
    global Tensor

    puts "creating volume"

    # should use DevCreateNewCopiedVolume if have a vol node
    # to copy...
    set t $Tensor(activeID) 
    if {$t == "" || $t == $Tensor(idNone)} {
	puts "Can't do math if no tensor selected"
	return
    }
    set name [Tensor($t,node) GetName]
    set name trace_$name
    set description "Trace volume derived from tensor volume $name"
    set v [TensorCreateEmptyVolume $t $description $name]

    puts "created volume $v"

    puts "pipeline"

    # put the filter output into a slicer volume
    Volume($v,vol) SetImageData [$filter GetOutput]
    MainVolumesUpdate $v

}


################################################################
#  Procedures to set up pipelines and create objects.
#  Lauren: try to create objects only if needed!
#  Should this be in a vtk class?
################################################################


#-------------------------------------------------------------------------------
# .PROC TensorApplyVisualizationParameters
#  For interaction with pipeline under advanced tab.
#  Apply all GUI parameters into our vtk objects.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorApplyVisualizationParameters {} {
    global Tensor

    # this code actually makes a bunch of calls like the following:
    # Tensor(vtk,axes) SetScaleFactor $Tensor(vtk,axes,scaleFactor)
    # Tensor(vtk,tubeAxes) SetRadius $Tensor(vtk,tubeAxes,radius)
    # Tensor(vtk,glyphs) SetClampScaling 1
    # we can't do calls like MyObject MyVariableOn now

    # our naming convention is:
    # Tensor(vtk,object)  is the name of the object
    # paramName is the name of the parameter
    # $Tensor(vtk,object,paramName) is the value 

    # Lauren we need to validate too!!!!!!!!
    #  too bad vtk can't return a string type desrciptor...

    # loop through all vtk objects
    #------------------------------------
    foreach o $Tensor(vtkObjectList) {

	# loop through all parameters of the object
	#------------------------------------
	foreach p $Tensor(vtkPropertyList,$o) {

	    # value of the parameter is $Tensor(vtk,$o,$p)
	    #------------------------------------
	    set value $Tensor(vtk,$o,$p)
	    
	    # Actually set the value appropriately in the vtk object
	    #------------------------------------	

	    # first capitalize the parameter name
	    set param [Cap $p]
	    
	    # MyObject SetMyParameter $value	
	    # handle the case in which value is a list with an eval 
	    # this puts it into the correct format for feeding to vtk
	    eval {Tensor(vtk,$o) Set$param} $value
	}
    }

}

#-------------------------------------------------------------------------------
# .PROC TensorMakeVTKObject
#  Wrapper for vtk object creation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorMakeVTKObject {class object} {
    global Tensor

    # make the object
    #------------------------------------
    $class Tensor(vtk,$object)

    # save on list for updating of its variables by user
    #------------------------------------
    lappend Tensor(vtkObjectList) $object

    # initialize list of its variables
    #------------------------------------
    set Tensor(vtkPropertyList,$object) ""
}

#-------------------------------------------------------------------------------
# .PROC TensorAddObjectProperty
#  Initialize vtk object access: saves object's property on list
#  for automatic GUI creation so user can change the property.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorAddObjectProperty {object parameter value type desc} {
    global Tensor

    # create tcl variables of the form:
    #set Tensor(vtk,tubeAxes,numberOfSides) 6
    #set Tensor(vtk,tubeAxes,numberOfSides,type) int
    #set Tensor(vtk,tubeAxes,numberOfSides,description) 
    #lappend Tensor(vtkPropertyList,tubeAxes) numberOfSides 


    # make tcl variable and save its attributes (type, desc)
    #------------------------------------
    set param [Uncap $parameter]
    set Tensor(vtk,$object,$param) $value
    set Tensor(vtk,$object,$param,type) $type
    set Tensor(vtk,$object,$param,description) $desc

    # save on list for updating variable by user
    #------------------------------------
    lappend Tensor(vtkPropertyList,$object) $param
    
}

#-------------------------------------------------------------------------------
# .PROC TensorBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildVTK {} {
    global Tensor

    # Lauren: maybe this module should kill its objects on exit.
    # We must be hogging a ton of memory all over the slicer.
    # So possibly build (some) VTK upon entering module.

    #---------------------------------------------------------------
    # Pipeline for preprocessing of glyphs
    #---------------------------------------------------------------

    # objects for thresholding before glyph display
    #------------------------------------

    # compute scalar data for thresholding
    set object thresh,math
    TensorMakeVTKObject vtkTensorMathematics $object

    # threshold the scalar data to produce binary mask 
    set object thresh,threshold
    TensorMakeVTKObject vtkImageThresholdBeyond $object
    Tensor(vtk,$object) SetInValue       1
    Tensor(vtk,$object) SetOutValue      0
    Tensor(vtk,$object) SetReplaceIn     1
    Tensor(vtk,$object) SetReplaceOut    1
    Tensor(vtk,$object) SetInput \
	    [Tensor(vtk,thresh,math) GetOutput]

    # convert the mask to unsigned char
    # Lauren it would be better to have the threshold filter do this
    set object thresh,cast
    TensorMakeVTKObject vtkImageCast $object
    Tensor(vtk,$object) SetOutputScalarTypeToUnsignedChar    
    Tensor(vtk,$object) SetInput \
	    [Tensor(vtk,thresh,threshold) GetOutput]

    # mask the tensors 
    set object thresh,mask
    TensorMakeVTKObject vtkTensorMask $object
    #Tensor(vtk,$object) SetMaskInput \
	#    [Tensor(vtk,thresh,threshold) GetOutput]
    Tensor(vtk,$object) SetMaskInput \
	    [Tensor(vtk,thresh,cast) GetOutput]

    # objects for masking before glyph display
    #------------------------------------

    # produce binary mask from the input mask labelmap
    set object mask,threshold
    TensorMakeVTKObject vtkImageThresholdBeyond $object
    Tensor(vtk,$object) SetInValue       1
    Tensor(vtk,$object) SetOutValue      0
    Tensor(vtk,$object) SetReplaceIn     1
    Tensor(vtk,$object) SetReplaceOut    1

    # convert the mask to short
    # (use this most probable input type to try to avoid data copy)
    set object mask,cast
    TensorMakeVTKObject vtkImageCast $object
    Tensor(vtk,$object) SetOutputScalarTypeToShort    
    Tensor(vtk,$object) SetInput \
	    [Tensor(vtk,mask,threshold) GetOutput]

    # mask the tensors 
    set object mask,mask
    TensorMakeVTKObject vtkTensorMask $object
    Tensor(vtk,$object) SetMaskInput \
	    [Tensor(vtk,mask,cast) GetOutput]

    #---------------------------------------------------------------
    # Pipeline for display of glyphs
    #---------------------------------------------------------------

    # User interaction objects
    #------------------------------------
    # Lauren: doing this like Endoscopic (this vs PointPicker?)
    set object picker
    TensorMakeVTKObject vtkCellPicker $object
    TensorAddObjectProperty $object Tolerance 0.001 float {Pick Tolerance}

    # Lauren test
    #Tensor(vtk,picker) DebugOn

    # Lauren test by displaying picked point
    #Tensor(vtk,picker) SetEndPickMethod "annotatePick Tensor(vtk,picker)"
    vtkTextMapper textMapper
    textMapper SetFontFamilyToArial
    textMapper SetFontSize 20
    textMapper BoldOn
    textMapper ShadowOn
    vtkActor2D textActor
    textActor VisibilityOff
    textActor SetMapper textMapper
    [textActor GetProperty] SetColor 1 0 0
    viewRen AddActor2D textActor

    # objects for creation of polydata glyphs
    #------------------------------------

    # Axes
    set object glyphs,axes
    TensorMakeVTKObject vtkAxes $object
    TensorAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeAxes
    #TensorMakeVTKObject vtkTubeFilter $object
    #Tensor(vtk,$object) SetInput [Tensor(vtk,glyphs,axes) GetOutput]
    #TensorAddObjectProperty $object Radius 0.1 float {Radius}
    #TensorAddObjectProperty $object NumberOfSides 6 int \
	#    {Number Of Sides}

    # Lauren test
    # One line
    set object glyphs,line
    TensorMakeVTKObject vtkLineSource $object
    TensorAddObjectProperty $object Resolution 10 int {Resolution}
    Tensor(vtk,$object) SetPoint1 0 0 0
    Tensor(vtk,$object) SetPoint2 1 0 0
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeLine
    #TensorMakeVTKObject vtkTubeFilter $object
    #Tensor(vtk,$object) SetInput [Tensor(vtk,glyphs,line) GetOutput]
    #TensorAddObjectProperty $object Radius 0.1 float {Radius}
    #TensorAddObjectProperty $object NumberOfSides 6 int \
	#    {Number Of Sides}

    # Ellipsoids
    set object glyphs,sphere
    TensorMakeVTKObject vtkSphereSource  $object
    TensorAddObjectProperty $object ThetaResolution 1 int ThetaResolution
    TensorAddObjectProperty $object PhiResolution 1 int PhiResolution

    # objects for placement of glyphs in dataset
    #------------------------------------
    set object glyphs
    #TensorMakeVTKObject vtkTensorGlyph $object
    TensorMakeVTKObject vtkInteractiveTensorGlyph $object
    #Tensor(vtk,glyphs) SetSource [Tensor(vtk,glyphs,axes) GetOutput]
    #Tensor(vtk,glyphs) SetSource [Tensor(vtk,glyphs,sphere) GetOutput]
    #TensorAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    TensorAddObjectProperty $object ScaleFactor 20000 float {Scale Factor}
    TensorAddObjectProperty $object ClampScaling 0 bool {Clamp Scaling}
    TensorAddObjectProperty $object ExtractEigenvalues 1 bool {Extract Eigenvalues}
    Tensor(vtk,$object) SetStartMethod      MainStartProgress
    Tensor(vtk,$object) SetProgressMethod  "MainShowProgress Tensor(vtk,$object)"
    Tensor(vtk,$object) SetEndMethod        MainEndProgress

    # poly data normals filter cleans up polydata for nice display
    # use this for ellipses only
    #------------------------------------
    # very slow
    # Lauren test
    set object glyphs,normals
    TensorMakeVTKObject vtkPolyDataNormals $object
    Tensor(vtk,$object) SetInput [Tensor(vtk,glyphs) GetOutput]

    # Display of tensor glyphs: LUT and Mapper
    #------------------------------------
    set object glyphs,lut
    #TensorMakeVTKObject vtkLogLookupTable $object
    TensorMakeVTKObject vtkLookupTable $object
    TensorAddObjectProperty $object HueRange \
	    {.6667 0.0} float {Hue Range}

    # mapper
    set object glyphs,mapper
    TensorMakeVTKObject vtkPolyDataMapper $object
    #Tensor(vtk,glyphs,mapper) SetInput [Tensor(vtk,glyphs) GetOutput]
    #Tensor(vtk,glyphs,mapper) SetInput [Tensor(vtk,glyphs,normals) GetOutput]
    Tensor(vtk,glyphs,mapper) SetLookupTable Tensor(vtk,glyphs,lut)
    TensorAddObjectProperty $object ImmediateModeRendering \
	    1 bool {Immediate Mode Rendering}    

    # Lauren fix this!!!!! (copied from a tcl example)
    #eval Tensor(vtk,glyphs,mapper)  SetScalarRange 0.0415224 0.784456 

    # Display of tensor glyphs: Actor
    #------------------------------------
    set object glyphs,actor
    TensorMakeVTKObject vtkActor $object
    Tensor(vtk,glyphs,actor) SetMapper Tensor(vtk,glyphs,mapper)
    [Tensor(vtk,glyphs,actor) GetProperty] SetAmbient 1
    [Tensor(vtk,glyphs,actor) GetProperty] SetDiffuse .2
    [Tensor(vtk,glyphs,actor) GetProperty] SetSpecular .4

    # Scalar bar actor
    #------------------------------------
    set object scalarBar,actor
    TensorMakeVTKObject vtkScalarBarActor $object
    Tensor(vtk,scalarBar,actor) SetLookupTable Tensor(vtk,glyphs,lut)
    viewRen AddProp Tensor(vtk,scalarBar,actor)
    Tensor(vtk,scalarBar,actor) VisibilityOff

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    set object streamln
    TensorMakeVTKObject vtkHyperStreamline $object
    
    Tensor(vtk,$object) SetStartMethod      MainStartProgress
    Tensor(vtk,$object) SetProgressMethod  "MainShowProgress Tensor(vtk,$object)"
    Tensor(vtk,$object) SetEndMethod        MainEndProgress

    #TensorAddObjectProperty $object StartPosition {9 9 -9} float {Start Pos}
    #TensorAddObjectProperty $object IntegrateMinorEigenvector \
	    #1 bool IntegrateMinorEv
    TensorAddObjectProperty $object MaximumPropagationDistance 60.0 \
	    float {Max Propagation Distance}
    TensorAddObjectProperty $object IntegrationStepLength 0.01 \
	    float {Integration Step Length}
    TensorAddObjectProperty $object StepLength 0.0001 \
	    float {Step Length}	    
    TensorAddObjectProperty $object Radius 0.25 \
	    float {Radius}
    TensorAddObjectProperty $object  NumberOfSides 30 \
	    int {Number of Sides}
    TensorAddObjectProperty $object  IntegrationDirection 2 \
	    int {Integration Direction}
    
    # Display of tensor streamline: LUT and Mapper
    #------------------------------------
    set object streamln,lut
    # Lauren we may want to use this once have no neg eigenvalues
    #TensorMakeVTKObject vtkLogLookupTable $object
    TensorMakeVTKObject vtkLookupTable $object
    Tensor(vtk,$object) SetHueRange .6667 0.0
#    TensorAddObjectProperty $object HueRange \
#	    {.6667 0.0} float {Hue Range}

    set object streamln,mapper
    TensorMakeVTKObject vtkPolyDataMapper $object
    Tensor(vtk,streamln,mapper) SetInput [Tensor(vtk,streamln) GetOutput]
    Tensor(vtk,streamln,mapper) SetLookupTable Tensor(vtk,streamln,lut)
    TensorAddObjectProperty $object ImmediateModeRendering \
	    1 bool {Immediate Mode Rendering}    

    #eval Tensor(vtk,streamln,mapper)  SetScalarRange 0.0415224 0.784456 

    # Display of tensor streamline: Actor
    #------------------------------------
    set object streamln,actor
    TensorMakeVTKObject vtkActor $object
    Tensor(vtk,streamln,actor) SetMapper Tensor(vtk,streamln,mapper)
    [Tensor(vtk,streamln,actor) GetProperty] SetAmbient 1
    [Tensor(vtk,streamln,actor) GetProperty] SetDiffuse 0

    #---------------------------------------------------------------
    # Pipeline for display of tensors over 2D slice
    #---------------------------------------------------------------
    
    # Lauren how should reformatting be hooked into regular
    # slicer slice reformatting?  Ideally want to follow
    # the 3 slices.
    TensorMakeVTKObject vtkImageReformat reformat


    #---------------------------------------------------------------
    # Pipeline for diffusion simulations
    #---------------------------------------------------------------
    set object diffusion    
    TensorMakeVTKObject vtkImageTensorDiffusion $object
    TensorAddObjectProperty $object NumberOfIterations 1 \
	    int {Number of Iterations}
    TensorAddObjectProperty $object S 1 \
	    float {Diffusion Speed}


    # Apply all settings from tcl variables that were
    # created above using calls to TensorAddObjectProperty
    #------------------------------------
    TensorApplyVisualizationParameters

}

#-------------------------------------------------------------------------------
# .PROC annotatePick
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc annotatePick {picker} {
    if { [$picker GetCellId] < 0 } {
	textActor VisibilityOff

    } else {
	set selPt [$picker GetSelectionPoint]
	set x [lindex $selPt 0] 
	set y [lindex $selPt 1]
	set pickPos [$picker GetPickPosition]
	set xp [lindex $pickPos 0] 
	set yp [lindex $pickPos 1]
	set zp [lindex $pickPos 2]

	textMapper SetInput "($xp, $yp, $zp)"
	textActor SetPosition $x $y
	textActor VisibilityOn
    }

    #renWin Render
    Render3D
}

##########################################
# procedures for converting volumes into tensors.
# this should happen automatically and be in MRML
# but to get things working try here first

#-------------------------------------------------------------------------------
# .PROC ConvertVolumeToTensors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ConvertVolumeToTensors {} {
    global Tensor Volume

    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
	puts "Can't create tensors from None volume"
	return
    }

    # setup - Lauren these should be globals linked with GUI
    set slicePeriod 8
    set offsetsGradient "0 1 2 3 4 5"
    set offsetsNoGradient "6 7"
    set numberOfGradientImages 6
    # Lauren the gradient dirs should also be settable

    # volume we use for input
    set input [Volume($v,vol) GetOutput]

    # tensor creation filter
    vtkImageDiffusionTensor tensor
    tensor SetNumberOfGradients $numberOfGradientImages
    
    # --------------------------------------------------------
    # We want the tensors to rotate with the volume (when it
    # is properly rotated for axial, sag, cor orientations).  
    # So rotate the gradient basis.
    # (this matrix JUST does the rotation needed for ijk->ras)
    #vtkTransform transform
    #vtkMatrix4x4 matRotate
    #Volume($v,node) MakePermuteMatrix matRotate [Volume($v,node) GetScanOrder] 
    #transform SetMatrix matRotate

    ##[transform GetMatrix] Invert
    #tensor SetTransform transform
    #transform Delete
    #matRotate Delete
    # --------------------------------------------------------

    # Lauren perhaps rm the rotation matrix stuff above and in the class

    # Rotate the gradient vectors if needed.
    set order [Volume($v,node) GetScanOrder]
    set id 0
    switch $order {
	"IS" { 
	    set id 0
	}
	"SI" {
	    set id 0
	}
	"AP" {
	    set id 1
	}
	"PA" {
	    set id 1
	}
	"LR" {
	    set id 2
	}
	"RL" {
	    set id 2
	}
    }

    # only sag is okay -- maybe we need -y!  since the
    # vtk y-axis is negative perhaps the polydata is messed up.

    # I don't get it
    # it seems that if the gradient vectors are in RAS-space
    # then we are set.
    # so it seems that this could happen once to the gradients
    # and be the same for all orientations since supine.  why not?
    # (note that applying an external transform to the volume
    # will not work on the tensors too right now)
    switch $id {
	"0" {
	    # axial: swap x/y in plane of tensors is goal?
	    # tried swap xy, rot 90 abt z, swap xz...
	    set elements "\
		    {0 0 1 0}  \
		    {0 1 0 0}  \
		    {1 0 0 0}  \
		    {0 0 0 1}  \
		    "
	}
	"1" {
	    # coronal: rotate tensors 90 deg in plane
	    # so swap y and z
	    set elements "\
		    {1 0 0 0}  \
		    {0 0 1 0}  \
		    {0 -1 0 0}  \
		    {0 0 0 1}  \
		    "
	    
	}
	"2" {
	    # sagittal: do nothing
	    # the gradient vectors are correct for sagittal
	    # identity matrix
	    set elements "\
		    {1 0 0 0}  \
		    {0 1 0 0}  \
		    {0 0 1 0}  \
		    {0 0 0 1}  \
		    "
	}
    }


    # try -y!!!!!
    set elements "\
	    {1  0 0 0}  \
	    {0 -1 0 0}  \
	    {0  0 1 0}  \
	    {0  0 0 1}  \
	    "

    # Lauren test
    # do nothing always!
    set elements "\
	    {1 0 0 0}  \
	    {0 1 0 0}  \
	    {0 0 1 0}  \
	    {0 0 0 1}  "

    vtkTransform trans    
    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
	foreach col $cols {
	    [trans GetMatrix] SetElement $row $col \
		    [lindex [lindex $elements $row] $col]
	}
    }
    
    # Tensor creation filter will transform gradient dirs by this matrix
    tensor SetTransform trans

    # produce input vols for tensor creation
    set inputNum 0
    set Tensor(recalculate,gradientVolumes) ""
    foreach slice $offsetsGradient {
	vtkImageExtractSlices extract$slice
	extract$slice SetInput $input
	extract$slice SetSliceOffset $slice
	extract$slice SetSlicePeriod $slicePeriod

	#puts "----------- slice $slice update --------"    
	extract$slice Update
	#puts [[extract$slice GetOutput] Print]

	# pass along in pipeline
	tensor SetDiffusionImage \
		$inputNum [extract$slice GetOutput]
	incr inputNum
	
	# put the filter output into a slicer volume
	# Lauren this should be optional
	# make a MRMLVolume for this output
	set name [Volume($v,node) GetName]
	set description "$slice gradient volume derived from volume $name"
	set name gradient${slice}_$name
	set id [DevCreateNewCopiedVolume $v $description $name]
	# save id in case we recalculate the tensors
	lappend Tensor(recalculate,gradientVolumes) $id
	puts "created volume $id"
	Volume($id,vol) SetImageData [extract$slice GetOutput]
	# fix the image range in the node (less slices than the original)
	set extent [[Volume($id,vol) GetOutput] GetExtent]
	set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
	eval {Volume($id,node) SetImageRange} $range
	# recompute the matrices using this offset to center vol in the cube
	# for some reason this uses the wrong node spacing!
	# Lauren test 
	eval {Volume($id,node) SetSpacing} [Volume($id,node) GetSpacing]

	set order [Volume($id,node) GetScanOrder]
	Volume($id,node) ComputeRasToIjkFromScanOrder $order


	# update slicer internals
	MainVolumesUpdate $id
	
    }
    # save ids in case we recalculate the tensors
    set Tensor(recalculate,noGradientVolumes) ""
    foreach slice $offsetsNoGradient {
	vtkImageExtractSlices extract$slice
	extract$slice SetInput $input
	extract$slice SetSliceOffset $slice
	extract$slice SetSlicePeriod $slicePeriod
	
	#puts "----------- slice $slice update --------"    
	extract$slice Update


	# put the filter output into a slicer volume
	# Lauren this should be optional
	# make a MRMLVolume for this output
	set name [Volume($v,node) GetName]
	set name noGradient${slice}_$name
	set description "$slice no gradient volume derived from volume $name"
	set id [DevCreateNewCopiedVolume $v $description $name]
	# save id in case we recalculate the tensors
	lappend Tensor(recalculate,noGradientVolumes) $id
	puts "created volume $id"
	Volume($id,vol) SetImageData [extract$slice GetOutput]
	# fix the image range in the node (less slices than the original)
	set extent [[Volume($id,vol) GetOutput] GetExtent]
	set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
	eval {Volume($id,node) SetImageRange} $range
	# recompute the matrices using this offset to center vol in the cube
	set order [Volume($id,node) GetScanOrder]
	Volume($id,node) ComputeRasToIjkFromScanOrder $order

	# update slicer internals
	MainVolumesUpdate $id
    }
    # also set the no diffusion input
    # Lauren the real pipeline should average these 2
    tensor SetNoDiffusionImage [extract6 GetOutput]
    
    
    puts "----------- tensor update --------"
    tensor Update
    puts "----------- after tensor update --------"


    # put output into a tensor volume
    # Lauren if volumes and tensors are the same
    # this should be done like the above
    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume Tensor]
    $newvol Copy Volume($v,node)
    $newvol SetDescription "tensor volume"
    $newvol SetName [Volume($v,node) GetName]
    set n [$newvol GetID]

    puts "SPACING [$newvol GetSpacing] DIMS [$newvol GetDimensions] MAT [$newvol GetRasToIjkMatrix]"
    # fix the image range in the node (less slices than the original)
    set extent [[Volume($id,vol) GetOutput] GetExtent]
    set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
    eval {$newvol SetImageRange} $range
    # recompute the matrices using this offset to center vol in the cube
    set order [$newvol GetScanOrder]
    
    $newvol ComputeRasToIjkFromScanOrder $order
  
    puts "SPACING [$newvol GetSpacing] DIMS [$newvol GetDimensions] MAT [$newvol GetRasToIjkMatrix]"
    MainDataCreate Tensor $n
    
    # we need to convert image data to structured 
    # points here...  
    # Lauren there may be a better way.    
    tensor Update
    Tensor($n,data) SetData [tensor GetOutput]

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Tensor(idList) $n] == -1} {
	puts "Lauren node doesn't exist, should unfreeze and fix volumes.tcltoo"
    } else {
	# Activate the new data object
	MainDataSetActive Tensor $n
    }


    # kill objects
    foreach slice $offsetsGradient {
	extract$slice SetOutput ""
	extract$slice Delete
    }
    foreach slice $offsetsNoGradient {
	extract$slice SetOutput ""
	extract$slice Delete
    }
    tensor Delete
    trans Delete
}

#-------------------------------------------------------------------------------
# .PROC TensorRecalculateTensors
# Using user-specified matrix to transform gradient vectors
# (and consequently the tensors), recalculate tensors
# from gradient volumes.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorRecalculateTensors {} {
    global Tensor

    set t $Tensor(activeID)

    # setup - Lauren these should be globals linked with GUI
    set slicePeriod 8
    set offsetsGradient "0 1 2 3 4 5"
    set offsetsNoGradient "6 7"
    set numberOfGradientImages 6
    # Lauren the gradient dirs should also be settable

    # tensor creation filter
    vtkImageDiffusionTensor tensor
    # Lauren Regularization factor should go in GUI
    #tensor SetRegularization $Tensor(regularization)

    tensor SetNumberOfGradients $numberOfGradientImages
    tensor DebugOn
    [tensor GetDualBasis] DebugOn
    # gradient image inputs
    set inputNum 0
    foreach v $Tensor(recalculate,gradientVolumes) {
	tensor SetDiffusionImage $inputNum [Volume($v,vol) GetOutput]
	incr inputNum
    }
    
    # also set the no diffusion input
    # Lauren the real pipeline should average these 2
    set v [lindex $Tensor(recalculate,noGradientVolumes) 0]
    tensor SetNoDiffusionImage [Volume($v,vol) GetOutput]


    # Use matrix entered by user
    #set elements $Tensor(recalculate,userMatrix)
    #set elements "\
	    #	    {1 0 0 0}  \
	    #	    {0 1 0 0}  \
	    #	    {0 0 1 0}  \
	    #	    {0 0 0 1}  "

    vtkTransform trans    
    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
	foreach col $cols {
	    [trans GetMatrix] SetElement $row $col \
		    $Tensor(recalculate,userMatrix,$row,$col)
	    #[lindex [lindex $elements $row] $col]
	}
    }    
    foreach row $rows {
	foreach col $cols {
	    puts -nonewline "$Tensor(recalculate,userMatrix,$row,$col) "
	    #[lindex [lindex $elements $row] $col]
	}
	puts ""
    }    

    # Tensor creation filter will transform gradient dirs by this matrix
    # This will really transform the output tensors in the same way
    tensor SetTransform trans

    # Recreate the tensors
    tensor Update
    Tensor($t,data) SetData [tensor GetOutput]
    puts [Tensor($t,data) Print]

    # Display the new stuff
    TensorUpdate

    # clean up
    tensor Delete
    trans Delete
}

#-------------------------------------------------------------------------------
# .PROC TensorWriteStructuredPoints
# Dump tensors to structured points file.  this ignores
# world to ijk now.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorWriteStructuredPoints {filename} {
    global Tensor

    set t $Tensor(activeID)

    vtkStructuredPointsWriter writer
    writer SetInput [Tensor($t,data) GetOutput]
    writer SetFileName $filename
    puts "Writing $filename..."
    writer Write
    writer Delete
    puts "Wrote tensor data, id $t, as $filename"
}

#-------------------------------------------------------------------------------
# .PROC TensorCalculateActorMatrix
# Place the entire tensor volume in world coordinates
# using this transform.  Uses world to IJk matrix but
# removes the spacing since the data/actor know about this.
# .ARGS
# vtkTransform transform the transform to modify
# int t the id of the tensor volume to calculate the matrix for
# .END
#-------------------------------------------------------------------------------
proc TensorCalculateActorMatrix {transform t} {

    # Grab the node whose data we want to position 
    set node Tensor($t,node)

    # the user matrix is either the reformat matrix
    # to place the slice, OR it needs to place the entire 
    # tensor volume.

    # In this procedure we calculate the second kind of matrix,
    # to place the whole volume.
    $transform Identity
    $transform PreMultiply

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    $transform SetMatrix [$node GetWldToIjk]

    # now it's ijk to world
    $transform Inverse

    # the data knows its spacing already so remove it
    # (otherwise the actor would be stretched, ouch)
    scan [$node GetSpacing] "%g %g %g" res_x res_y res_z
    $transform Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
	    [expr 1.0 / $res_z]
}


#-------------------------------------------------------------------------------
# .PROC TensorSpecificVisualizationSettings
# Set up vizualization to see tensors well.
# Transparent 3D slices and black background...
# 
# FUTURE IDEAS:
# It would be nice if this sort of setting could be pushed/
# popped like the bindings stack that Peter wrote (Events.tcl).
# This would allow modules to control the visualization
# but not interfere with other modules.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSpecificVisualizationSettings {} {
    global Anno

    # let us see tensors through the slices
    MainSlicesSet3DOpacityAll 0.1
    MainViewSetBackgroundColor Black
    # show all digits of float data (i.e. trace)
    MainAnnoSetPixelDisplayFormat "full"
    # turn off those irritating letters
    set Anno(letters) 0
    MainAnnoSetVisibility
}

#-------------------------------------------------------------------------------
# .PROC TensorResetDefaultVisualizationSettings
# Undo the TensorSpecific settings and use defaults 
# of the slicer.  
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorResetDefaultVisualizationSettings {} {
    global Anno

    # let us see tensors through the slices
    MainSlicesReset3DOpacityAll
    MainViewSetBackgroundColor Blue
    # show all digits of float data (i.e. trace)
    MainAnnoSetPixelDisplayFormat "default"
    # turn back on those irritating letters
    set Anno(letters) 1
    MainAnnoSetVisibility
}

#-------------------------------------------------------------------------------
# .PROC TensorApplyVisualizationSettings
#  Set the settings the user requested, default or for tensors
# .ARGS
# str mode optional, default or tensors
# .END
#-------------------------------------------------------------------------------
proc TensorApplyVisualizationSettings {{mode ""}} {
    global Tensor
    
    if {$mode == ""} {
	set mode $Tensor(mode,visualizationSettingsType)
    }
    
    switch $mode {
	"default" {
	    TensorResetDefaultVisualizationSettings
	}
	"tensors" {
	    TensorSpecificVisualizationSettings
	}
    }

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorSetOperation
# Set the mathematical operation we should do to produce
# a scalar volume from the tensors
# .ARGS
# str math the name of the operation from list $Tensor(scalars,operationList)
# .END
#-------------------------------------------------------------------------------
proc TensorSetOperation {math} {
    global Tensor

    set Tensor(scalars,operation) $math
    
    # config menubutton
    $Tensor(gui,mbMath) config -text $math
}


#-------------------------------------------------------------------------------
# .PROC TensorAddStreamline
# Add a streamline to the scene
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorAddStreamline {} {
    global Tensor

    incr Tensor(vtk,streamline,currentID)
    set id $Tensor(vtk,streamline,currentID)
    lappend Tensor(vtk,streamline,idList) $id

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    set streamline streamln,$id
    TensorMakeVTKObject vtkHyperStreamline $streamline
    
    Tensor(vtk,$streamline) SetStartMethod      MainStartProgress
    Tensor(vtk,$streamline) SetProgressMethod  "MainShowProgress Tensor(vtk,$streamline)"
    Tensor(vtk,$streamline) SetEndMethod        MainEndProgress

    #TensorAddObjectProperty $streamline StartPosition {9 9 -9} float {Start Pos}
    #TensorAddObjectProperty $streamline IntegrateMinorEigenvector \
	    #1 bool IntegrateMinorEv
    TensorAddObjectProperty $streamline MaximumPropagationDistance 60.0 \
	    float {Max Propagation Distance}
    TensorAddObjectProperty $streamline IntegrationStepLength 0.01 \
	    float {Integration Step Length}
    TensorAddObjectProperty $streamline StepLength 0.0001 \
	    float {Step Length}	    
    TensorAddObjectProperty $streamline Radius 0.25 \
	    float {Radius}
    TensorAddObjectProperty $streamline  NumberOfSides 30 \
	    int {Number of Sides}
    TensorAddObjectProperty $streamline  IntegrationDirection 2 \
	    int {Integration Direction}
    
    # Display of tensor streamline: LUT and Mapper
    #------------------------------------
    set object $streamline,lut
    # Lauren we may want to use this once have no neg eigenvalues
    #TensorMakeVTKObject vtkLogLookupTable $object
    TensorMakeVTKObject vtkLookupTable $object
    Tensor(vtk,$object) SetHueRange .6667 0.0
#    TensorAddObjectProperty $object HueRange \
#	    {.6667 0.0} float {Hue Range}

    set object $streamline,mapper
    TensorMakeVTKObject vtkPolyDataMapper $object
    Tensor(vtk,$streamline,mapper) SetInput [Tensor(vtk,$streamline) GetOutput]
    Tensor(vtk,$streamline,mapper) SetLookupTable Tensor(vtk,$streamline,lut)
    TensorAddObjectProperty $object ImmediateModeRendering \
	    1 bool {Immediate Mode Rendering}    

    #eval Tensor(vtk,$streamline,mapper)  SetScalarRange 0.0415224 0.784456 

    # Display of tensor streamline: Actor
    #------------------------------------
    set object $streamline,actor
    TensorMakeVTKObject vtkActor $object
    Tensor(vtk,$streamline,actor) SetMapper Tensor(vtk,$streamline,mapper)
    [Tensor(vtk,$streamline,actor) GetProperty] SetAmbient 1
    [Tensor(vtk,$streamline,actor) GetProperty] SetDiffuse 0
}


#-------------------------------------------------------------------------------
# .PROC TensorDoDiffusionNot
# junk
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorDoDiffusionNot {} {
    global Tensor Volume

    set t $Tensor(activeID)
    # This was selected from the menu 
    set id $Volume(activeID)

    # Make sure we have float inputs
    vtkImageCast cast
    cast SetInput [Volume($id,vol) GetOutput]
    cast SetOutputScalarTypeToFloat
    
    # This is a heat distribution, for now a segmentation
    Tensor(vtk,diffusion) SetInput [cast GetOutput]
    Tensor(vtk,diffusion) SetInputTensor [Tensor($t,data) GetOutput]
    # Lauren this is dumb, this class does not need this input
    Tensor(vtk,diffusion) SetInputTexture [cast GetOutput]
    #Tensor(vtk,diffusion) SetS 0.5

    puts "updating diffusion"
    Tensor(vtk,diffusion) Update
    puts "done updating diffusion"

    # Put the output into a slicer volume
    set name [Tensor($t,node) GetName]
    set name Diffusion_$name
    set description "Diffusion volume derived from tensor volume $name"
    set v [TensorCreateEmptyVolume $t $description $name]
    puts "created volume $v"
    Volume($v,node) SetScalarTypeToFloat
    Volume($v,vol) SetImageData [Tensor(vtk,diffusion) GetOutput]
    MainVolumesUpdate $v
    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v
    RenderAll

    # this is to disconnect the pipeline
    #Tensor(vtk,diffusion) SetOutput ""
    Tensor(vtk,diffusion) SetInput ""
    Tensor(vtk,diffusion) SetInputTexture ""

    cast Delete
}

#-------------------------------------------------------------------------------
# .PROC TensorDoDiffusion
# Test diffusion simulation proc
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorDoDiffusion {} {
    global Tensor Volume


    # Lauren
    # hack so we can use settings from GUI
    # but pipeline will disconnect -- why won't it anyway?
    vtkImageTensorDiffusion diff
    foreach prop {NumberOfIterations S} {
	diff Set$prop [Tensor(vtk,diffusion) Get$prop]
    }
    set t $Tensor(activeID)
    # This was selected from the menu 
    set id $Volume(activeID)

    # Make sure we have float inputs
    vtkImageCast cast
    cast SetInput [Volume($id,vol) GetOutput]
    cast SetOutputScalarTypeToFloat
    
    # This is an initial heat distribution
    diff SetInput [cast GetOutput]
    diff SetInputTensor [Tensor($t,data) GetOutput]
    # Make a source where they segmented too
    diff SetInputSource [cast GetOutput]
    #diff SetS 0.5

    puts "updating diffusion"
    diff Update
    puts "done updating diffusion"


    # Lauren 
    # hack to see things since floats and slicer don't work
    vtkImageMathematics math
    math SetOperationToMultiplyByK
    math SetConstantK 1000
    math SetInput 0 [diff GetOutput]
    math Update

    # Put the output into a slicer volume
    set name [Tensor($t,node) GetName]
    set name Diffusion_$name
    set description "Diffusion volume derived from tensor volume $name"
    set v [TensorCreateEmptyVolume $t $description $name]
    puts "created volume $v"
    Volume($v,node) SetScalarTypeToFloat
    #Volume($v,vol) SetImageData [diff GetOutput]
    Volume($v,vol) SetImageData [math GetOutput]
    MainVolumesUpdate $v
    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v
    RenderAll


    # hack ? to disconnect
    #diff SetInput ""    
    #diff SetInputTensor ""    
    # this is to disconnect the pipeline
    # this object hangs around, so try this trick from Editor.tcl:
    #diff SetOutput ""
    diff Delete

    math Delete
    cast Delete
}


#-------------------------------------------------------------------------------
# .PROC TensorPreprocess
# Test regularization proc
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorPreprocess {} {
    global Tensor Volume

    # Lauren need a  menu 
    set t $Tensor(activeID)

    vtkTensorRegularization regularize

    regularize SetInput [Tensor($t,data) GetOutput]

    # Lauren for now replace old data with new
    regularize Update
    Tensor($t,data) SetData [regularize GetOutput]
 
    # Display the new stuff
    TensorUpdate

   #regularize SetOperationTo
    regularize Delete
}

#-------------------------------------------------------------------------------
# .PROC TensorRoundFloatingPoint
# Format floats for GUI display (we don't want -5e-11)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorRoundFloatingPoint {val} {
    global Tensor

    return [format $Tensor(floatingPointFormat) $val]
}


#-------------------------------------------------------------------------------
# .PROC TensorRemoveAllActors
# Rm all actors from scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorRemoveAllActors {} {
    global Tensor
    
    # Remove our actors if the user wants that on module exit.
    # We always remove for now, later if removal is optional 
    # need to keep track of if actor was added already.
    if {$Tensor(vtk,glyphs,actor,remove)} {
	MainRemoveActor Tensor(vtk,glyphs,actor)
    }

    TensorRemoveAllStreamlines
    #if {$Tensor(vtk,streamln,actor,remove)} {
	#MainRemoveActor Tensor(vtk,streamln,actor)
    #}
    # make them actually disappear
    Render3D

}

#-------------------------------------------------------------------------------
# .PROC TensorRemoveAllStreamlines
# Remove all streamline actors from scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorRemoveAllStreamlines {} {
    global Tensor

    foreach id $Tensor(vtk,streamline,idList) {
	set streamline streamln,$id
	MainRemoveActor Tensor(vtk,$streamline,actor) 
    }
}


#-------------------------------------------------------------------------------
# .PROC TensorSeedStreamlinesFromSegmentation
# In conjunction with TensorExecuteForProgrammableFilter, seeds
# streamlines at all points in a segmentation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSeedStreamlinesFromSegmentation {} {
    global Tensor Volume

    # filter to grab list of points in the segmentation
    vtkProgrammableAttributeDataFilter Tensor(vtk,programmableFilt)

    set t $Tensor(activeID)
    set v $Volume(activeID)

    #Tensor(vtk,programmableFilt) SetInput [Tensor($t,data) GetOutput] 
    Tensor(vtk,programmableFilt) SetInput [Volume($v,vol) GetOutput] 
    Tensor(vtk,programmableFilt) SetExecuteMethod \
	    TensorExecuteForProgrammableFilter

    set Tensor(streamlineList) ""

    Tensor(vtk,programmableFilt) Update
    Tensor(vtk,programmableFilt) Delete

    # Now do a streamline at each point
    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform trans
    set v $Volume(activeID)
    set node Volume($v,node)
    trans SetMatrix [$node GetWldToIjk]
    # now it's ijk to world
    trans Inverse
    foreach point $Tensor(streamlineList) {
	puts "..$point"
	# find out where the point actually is in world space
	set world [eval {trans TransformPoint} $point]
	puts $world
	eval {TensorSelect} $world
	# save point to make a streamline there
	#eval {TensorSelect} $point
    }

    trans Delete
}

#-------------------------------------------------------------------------------
# .PROC TensorExecuteForProgrammableFilter
# Just makes a tcl list of nonzero points in the input volume.
# Used as execute proc of a programmable attribute data filter.
# We call this to figure out where to seed the streamlines.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorExecuteForProgrammableFilter {} {
    global Volume Tensor

    # proc for ProgrammableAttributeDataFilter.  Note the use of "double()"
    # in the calculations.  This protects us from Tcl using ints and 
    # overflowing.

    puts "Programmable filter executing"
    set input [Tensor(vtk,programmableFilt) GetInput]
    set numPts [$input GetNumberOfPoints]
    
    set scalars [[$input GetPointData] GetScalars]
    
    set extent [$input GetExtent]
    puts $extent
    set xmin [lindex $extent 0]
    set xmax [expr [lindex $extent 1] + 1]

    set ymin [lindex $extent 2]
    set ymax [expr [lindex $extent 3] + 1]

    set zmin [lindex $extent 4]
    set zmax [expr [lindex $extent 5] + 1]

    # this programmable filter is amazing, we are really
    # looping through the pixels in tcl code here:
    set i 0
    
    for {set z $zmin} {$z < $zmax} {incr z} {
	
	for {set y $ymin} {$y < $ymax} {incr y} {
	    
	    for {set x $xmin} {$x < $xmax} {incr x} {
		
		set s [$scalars GetScalar $i]
		
		if {$s > 0} {
		    
		    # for each nonzero point in the segmentation
		    # we want to start a streamline
		    puts $s

		    # save on the list of points for later use
		    lappend Tensor(streamlineList) "$x $y $z"
		}
		
		# point index
		incr i
		
	    } 
	} 
    } 
    
    # should we pass the input through (points)?
    #[$output GetCellData] PassData [$input GetCellData]

    puts "done $x $y $z $i"
    # ignore output for now, we don't care
    #[[Tensor(vtk,programmableFilt) GetOutput] GetPointData] SetScalars scalars
		
}
