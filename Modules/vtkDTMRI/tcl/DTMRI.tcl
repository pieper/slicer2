#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology
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
# FILE:        DTMRI.tcl
# PROCEDURES:
#   DTMRIInit
#   DTMRIUpdateMRML
#   DTMRIEnter
#   DTMRIExit
#   DTMRIBuildGUI
#   DTMRIRaiseMoreOptionsFrame mode
#   DTMRIBuildScrolledGUI f
#   DTMRICheckScrollLimits
#   DTMRISizeScrolledGUI f
#   DTMRISetPropertyType
#   DTMRIPropsApply
#   DTMRIPropsCancel
#   DTMRIAdvancedApply
#   DTMRISetFileName
#   DTMRICreateBindings
#   DTMRIRoundFloatingPoint
#   DTMRIRemoveAllActors
#   DTMRIAddAllActors
#   DTMRIUpdateReformatType
#   DTMRIUpdateScalarBar
#   DTMRIShowScalarBar
#   DTMRIHideScalarBar
#   DTMRIUpdateThreshold
#   DTMRIUpdateMaskLabel
#   DTMRIUpdateActor
#   DTMRISpecificVisualizationSettings
#   DTMRIResetDefaultVisualizationSettings
#   DTMRIApplyVisualizationSettings mode
#   DTMRIUpdateGlyphEigenvector
#   DTMRIUpdateGlyphColor
#   DTMRIUpdateGlyphScalarRange
#   DTMRISelectRemoveHyperStreamline
#   DTMRISelectStartHyperStreamline x y z
#   DTMRIUpdateStreamlines
#   DTMRIAddStreamline
#   DTMRIConfigureStreamline
#   DTMRIUpdateTractColor
#   DTMRIRemoveAllStreamlines
#   DTMRIAddAllStreamlines
#   DTMRIDeleteAllStreamlines
#   DTMRISeedStreamlinesFromSegmentation
#   DTMRIExecuteForProgrammableFilter
#   DTMRIUpdate
#   DTMRISetOperation math
#   DTMRIUpdateMathParams
#   DTMRICreateEmptyVolume
#   DTMRIDoMath
#   DTMRIApplyVisualizationParameters
#   DTMRIDeleteVTKObject
#   DTMRIMakeVTKObject
#   DTMRIAddObjectProperty
#   DTMRIBuildVTK
#   ConvertVolumeToTensors
#   DTMRIWriteStructuredPoints
#   DTMRISaveStreamlinesAsIJKPoints subdir name verbose
#   DTMRISaveStreamlinesAsModel
#   DTMRIGetPointsFromSegmentationInIJKofDTMRIVolume verbose
#   DTMRIGetScaledIjkCoordinatesFromWorldCoordinates x y z
#   DTMRICalculateActorMatrix transform t
#   DTMRICalculateIJKtoRASRotationMatrix
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC DTMRIInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIInit {} {
    global DTMRI Module Volume
    
    set m DTMRI
     
    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "Diffusion DTMRI MRI visualization and more..."
    set Module($m,author) "Lauren O'Donnell"
    # version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.4 $} {$Date: 2004/03/11 19:55:15 $}]

    # Define Tabs
    #------------------------------------
    set Module($m,row1List) "Help Display ROI Scalars Props"
    set Module($m,row1Name) "{Help} {Disp} {ROI} {Scalars} {Props}"
    set Module($m,row1,tab) Display
    # Use these lines to add a second row of tabs
    set Module($m,row2List) "Diffuse Advanced Devel"
    set Module($m,row2Name) "{Diffuse} {Advanced} {Devel}"
    set Module($m,row2,tab) Advanced
    
    # Define Procedures
    #------------------------------------
    set Module($m,procGUI) DTMRIBuildGUI
    set Module($m,procMRML) DTMRIUpdateMRML
    set Module($m,procVTK) DTMRIBuildVTK
    set Module($m,procEnter) DTMRIEnter
    set Module($m,procExit) DTMRIExit
    
    # Define Dependencies
    #------------------------------------
    # This module depends on Csys (in future)
    #set Module($m,depend) Csys
    set Module($m,depend) ""
    
    # Create any specific bindings for this module
    #------------------------------------
    DTMRICreateBindings


    # Props: GUI tab we are currently on
    #------------------------------------
    set DTMRI(propertyType) Basic
    
    # Initial path to search when loading files
    #------------------------------------
    set DTMRI(DefaultDir) ""


    #------------------------------------
    # conversion from volume to DTMRIs variables
    #------------------------------------
    vtkImageDiffusionTensor _default
    set DTMRI(convert,numberOfGradients) [_default GetNumberOfGradients]
    set DTMRI(convert,gradients) ""
    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
        _default SelectDiffusionGradient $i
        lappend DTMRI(convert,gradients) [_default GetSelectedDiffusionGradient]
    }
    # puts $DTMRI(convert,gradients)
    set DTMRI(convert,firstGradientImage) 1
    set DTMRI(convert,lastGradientImage) 6
    set DTMRI(convert,firstNoGradientImage) 7
    set DTMRI(convert,lastNoGradientImage) 8
    _default Delete

    #------------------------------------
    # Visualization-related variables
    #------------------------------------

    # type of settings in 3d view, anno, etc
    set DTMRI(mode,visualizationSettingsType) default
    set DTMRI(mode,visualizationSettingsTypeList) {DTMRIs default}
    set DTMRI(mode,visualizationSettingsTypeList,tooltips) [list \
            "3D View settings to display DTMRIs: transparent slices." \
            "Reset to Slicer default settings."  \
            ]

    # type of reformatting
    set DTMRI(mode,reformatType) 0
    #set DTMRI(mode,reformatTypeList) {None 0 1 2}
    #set DTMRI(mode,reformatTypeList) {0 1 2}
    set DTMRI(mode,reformatTypeList) {0 1 2 None}
    set DTMRI(mode,reformatTypeList,text) {"" "" "" Vol}
    set DTMRI(mode,reformatTypeList,tooltips) [list \
           "Display DTMRIs as glyphs (ex. lines) in the location of the leftmost slice."  \
            "Display DTMRIs as glyphs (ex. lines) in the location of the middle slice."  \
            "Display DTMRIs as glyphs (ex. lines) in the location of the rightmost slice."  \
        "Display all DTMRIs in the volume.  Please use an ROI." \
            ]
    #set DTMRI(mode,reformatTypeList,tooltips) [list \
    #        "No reformatting: display all DTMRIs." \
    #       "Reformat DTMRIs along with slice 0."  \
    #      "Reformat DTMRIs along with slice 1."  \
    #     "Reformat DTMRIs along with slice 2."  \
    #]

    # whether we are currently displaying glyphs
    set DTMRI(mode,visualizationType,glyphsOn) 0n
    set DTMRI(mode,visualizationType,glyphsOnList) {On Off}
    set DTMRI(mode,visualizationType,glyphsOnList,tooltip) [list \
    "Display each DTMRI as a glyph\n(for example a line or ellipsoid)" \
    "Do not display glyphs" ]

    # whether we are currently displaying tracts
    set DTMRI(mode,visualizationType,tractsOn) 0n
    set DTMRI(mode,visualizationType,tractsOnList) {On Off Delete}
    set DTMRI(mode,visualizationType,tractsOnList,tooltip) [list \
        "Display all 'tracts'" \
        "Hide all 'tracts'" \
        "Clear all 'tracts'" ]
    # guard against multiple actor add/remove from GUI
    set DTMRI(vtk,streamline,actorsAdded) 1

    # type of visualization settings GUI to display to user
    set DTMRI(mode,visualizationTypeGui) Tracts
    set DTMRI(mode,visualizationTypeGuiList) {Help Glyphs Tracts AutoTracts SaveTracts}
    set DTMRI(mode,visualizationTypeGuiList,tooltip) "Select from this menu\n and settings for each type\n of visualization will appear below."

    # type of glyph to display (default to lines since fastest)
    set DTMRI(mode,glyphType) Lines
    set DTMRI(mode,glyphTypeList) {Axes Lines Ellipsoids Boxes}
    set DTMRI(mode,glyphTypeList,tooltips) {{Display DTMRIs as 3 axes aligned with eigenvectors and scaled by eigenvalues.} {Display DTMRIs as lines aligned with one eigenvector and scaled by its eigenvalue.} {Display DTMRIs as ellipses aligned with eigenvectors and scaled by eigenvalues.} {Display DTMRIs as scaled oriented cubes.}}

    # type of eigenvector to draw glyph lines for
    set DTMRI(mode,glyphEigenvector) Max
    set DTMRI(mode,glyphEigenvectorList) {Max Middle Min}
    set DTMRI(mode,glyphEigenvectorList,tooltips) {{When displaying DTMRIs as Lines, use the eigenvector corresponding to the largest eigenvalue.} {When displaying DTMRIs as Lines, use the eigenvector corresponding to the middle eigenvalue.} {When displaying DTMRIs as Lines, use the eigenvector corresponding to the smallest eigenvalue.}}

    # type of glyph coloring
    set DTMRI(mode,glyphColor) Direction; # default to direction
    set DTMRI(mode,glyphColorList) {Linear Planar Spherical Max Middle Min MaxMinusMiddle RA FA Direction}
    set DTMRI(mode,glyphColorList,tooltip) "Color DTMRIs according to\nLinear, Planar, or Spherical measures,\nwith the Max, Middle, or Min eigenvalue,\nwith relative or fractional anisotropy (RA or FA),\nor by direction of major eigenvector."

    # type of tract coloring
    set DTMRI(mode,tractColor) SolidColor;
    set DTMRI(mode,tractColorList) {SolidColor MultiColor}
    set DTMRI(mode,tractColorList,tooltip) {Color streamlines with a solid color OR MultiColor by scalars from the data (diffusion magnitude).}


    # How to handle display of colors: like W/L but scalar range
    set DTMRI(mode,glyphScalarRange) Auto
    set DTMRI(mode,glyphScalarRangeList) {Auto Manual}
    set DTMRI(mode,glyphScalarRangeList,tooltips) [list \
            "Scalar range will be set to max and min scalar in the data." \
            "User-adjustable scalar range to highlight areas of interest (like window/level does)."]
    # slider min/max values
    set DTMRI(mode,glyphScalarRange,min) 0
    set DTMRI(mode,glyphScalarRange,max) 10
    # slider current settings
    set DTMRI(mode,glyphScalarRange,low) 0
    set DTMRI(mode,glyphScalarRange,hi) 1

    # whether to reformat DTMRIs along with slices
    set DTMRI(mode,reformat) 0

    # display properties of the actors
    set DTMRI(actor,ambient) 1
    set DTMRI(actor,diffuse) .2
    set DTMRI(actor,specular) .4

    # Whether to remove actors on module exit
    set DTMRI(vtk,actors,removeOnExit) 1

    # scalar bar
    set DTMRI(mode,scalarBar) Off
    set DTMRI(mode,scalarBarList) {On Off}
    set DTMRI(mode,scalarBarList,tooltips) [list \
            "Display a scalar bar to show correspondence between numbers and colors." \
            "Do not display the scalar bar."]


    #------------------------------------
    # Variables for streamline display
    #------------------------------------
    set DTMRI(stream,variableList) [list \
        MaximumPropagationDistance IntegrationStepLength \
        StepLength Radius  NumberOfSides IntegrationDirection]
    set DTMRI(stream,variableList,text) [list \
        "Max Length" "Step Size" \
        "Smoothness (along)" "Radius"  "Smoothness (around)" "Direction"]
    set DTMRI(stream,variableList,tooltips) [list \
        "MaximumPropagationDistance: Tractography will stop after this distance" \
        "IntegrationStepLength: step size when following path" \
        "StepLength: Length of each displayed tube segment" \
        "Radius: Initial radius (thickness) of displayed tube" \
        "NumberOfSides: Number of sides of displayed tube" \
        "IntegrationDirection: Number of directions to follow from initial point (1 or 2)"]
    
    set DTMRI(stream,MaximumPropagationDistance)  30
    # nominal integration step size (expressed as a fraction of the
    # size of each cell)  0.2 is default
    set DTMRI(stream,IntegrationStepLength)  0.1
    # Set / get the length of a tube segment composing the
    # hyperstreamline. The length is specified as a fraction of the
    # diagonal length of the input bounding box.  0.01 is vtk default
    set DTMRI(stream,StepLength)  0.005
    # radius of (polydata) tube that is displayed
    set DTMRI(stream,Radius)  0.2 
    # sides of tube
    #set DTMRI(stream,NumberOfSides)  30
    set DTMRI(stream,NumberOfSides)  10
    # 2 means SetIntegrationDirectionToIntegrateBothDirections
    set DTMRI(stream,IntegrationDirection)  2

    #------------------------------------
    # Variables for auto streamline display
    #------------------------------------
    set DTMRI(mode,autoTractsLabel,tooltip) "A tract will be seeded in each voxel of the ROI which is colored with this label."

    #------------------------------------
    # Variables for vtk object creation
    #------------------------------------
    
    # ID of the last streamline created
    set DTMRI(vtk,streamline,currentID) -1
    set DTMRI(vtk,streamline,idList) ""

    #------------------------------------
    # Variables for preprocessing
    #------------------------------------

    # upper and lower values allowable when thresholding
    set DTMRI(thresh,threshold,rangeLow) 0
    set DTMRI(thresh,threshold,rangeHigh) 500

    # type of thresholding to use to reduce number of DTMRIs
    set DTMRI(mode,threshold) None
    set DTMRI(mode,thresholdList) {None Trace LinearMeasure SphericalMeasure PlanarMeasure}
    set DTMRI(mode,thresholdList,tooltips) {{No thresholding.  Display all DTMRIs.} {Only display DTMRIs where the trace is between the threshold values.}  {Only display DTMRIs where the anisotropy is between the threshold values.}}

    # type of masking to use to reduce volume of DTMRIs
    set DTMRI(mode,mask) None
    set DTMRI(mode,maskList) {None MaskWithLabelmap}
    set DTMRI(mode,maskList,tooltips) {{No masking.  Display all DTMRIs.} {Only display DTMRIs where the mask labelmap shows the chosen label value.}}
    set DTMRI(mode,maskLabel,tooltip) "The ROI colored with this label will be used to mask DTMRIs.  DTMRIs will be shown only inside the ROI."

    # labelmap to use as mask
    set DTMRI(MaskLabelmap) $Volume(idNone)

    #------------------------------------
    # Variables for producing scalar volumes
    #------------------------------------

    # math op to produce scalars from DTMRIs
    set DTMRI(scalars,operation) Trace
    set DTMRI(scalars,operationList) [list Trace Determinant \
            RelativeAnisotropy FractionalAnisotropy LinearMeasure \
            PlanarMeasure SphericalMeasure MaxEigenvalue \
            MiddleEigenvalue MinEigenvalue ColorByOrientation D11 D22 D33]

    set DTMRI(scalars,operationList,tooltip) "Produce a scalar volume from DTMRI data.\nTrace, Determinant, Anisotropy, and Eigenvalues produce grayscale volumes,\nwhile Orientation produces a 3-component (Color) volume that is best viewed in the 3D window."

    # how much to scale the output floats by
    set DTMRI(scalars,scaleFactor) 10000
    set DTMRI(scalars,scaleFactor,tooltip) \
    "Multiplicative factor applied to output images for better viewing."
    
    # whether to compute vol from ROI or whole DTMRI volume
    set DTMRI(scalars,ROI) None
    set DTMRI(scalars,ROIList) {None Threshold Mask}
    set DTMRI(scalars,ROIList,tooltips) {"No ROI: derive the scalar volume from the entire DTMRI volume." "Use the thresholded area defined in the ROI tab to mask the DTMRI volume before scalar volume creation." "Use the mask labelmap volume defined in the ROI tab to mask the DTMRI volume before scalar volume creation."}


    #------------------------------------
    # Developers panel variables
    #------------------------------------
    set DTMRI(devel,subdir) ""
    set DTMRI(devel,fileNamePoints) ""
    set DTMRI(devel,fileName) "DTMRIs.vtk"
    set tmp "\
            {1 0 0 0}  \
            {0 1 0 0}  \
            {0 0 1 0}  \
            {0 0 0 1}  "
    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
        foreach col $cols {
            set DTMRI(recalculate,userMatrix,$row,$col) \
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
    set DTMRI(floatingPointFormat) "%0.5f"

    

    # Lauren fix this
    set DTMRI(Description) ""
    set DTMRI(Name) ""
}

################################################################
#  Procedures called automatically by the slicer
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMRML {} {
    #puts "Lauren in DTMRIUpdateMRML"
}

#-------------------------------------------------------------------------------
# .PROC DTMRIEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIEnter {} {
    global DTMRI Slice View
    
    # set global flag to avoid possible render loop
    set View(resetCameraClippingRange) 0

    # add event handling for 3D
    #CsysPushBindings DTMRI DTMRI3DStreamlineEvents
    #puts "Lauren Fix bindings"
    # add actor for csys
    #puts "Not adding csys actor"
    #MainAddActor DTMRI(coordinateAxesTool,actor)
    
    # add event handling for slices
    EvActivateBindingSet DTMRISlice0Events
    EvActivateBindingSet DTMRISlice1Events
    EvActivateBindingSet DTMRISlice2Events

    # add event handling for 3D
    EvActivateBindingSet DTMRI3DEvents

    # configure all scrolled GUIs so frames inside fit
    DTMRISizeScrolledGUI $DTMRI(scrolledGui,advanced)

    # color label selection widgets
    LabelsColorWidgets


    # Default to reformatting along with the currently active slice
    set DTMRI(mode,reformatType) $Slice(activeID)

    # Add our actors if removed on module exit.
    if {$DTMRI(vtk,actors,removeOnExit)} {
    DTMRIAddAllActors
    }
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIExit {} {
    global DTMRI View
    
    # unset global flag to avoid possible render loop
    set View(resetCameraClippingRange) 1

    # get rid of event handling for csys
    #CsysPopBindings DTMRI
    # remove actor for csys
    #MainRemoveActor DTMRI(coordinateAxesTool,actor)


    # remove event handling for slices
    EvDeactivateBindingSet DTMRISlice0Events
    EvDeactivateBindingSet DTMRISlice1Events
    EvDeactivateBindingSet DTMRISlice2Events

    # remove event handling for 3D
    EvDeactivateBindingSet DTMRI3DEvents

    # Remove our actors if the user wants that on module exit.
    if {$DTMRI(vtk,actors,removeOnExit)} {
    DTMRIRemoveAllActors
    }
    # make 3D slices opaque now
    #MainSlicesReset3DOpacity
}

################################################################
#  Procedures for building the GUI
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildGUI
# Builds the GUI panel.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildGUI {} {
    global Module Gui DTMRI Label Volume Tensor

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
    #             Tracts
    #             AutoTracts
    #             SaveTracts
    #       VisUpdate
    # ROI
    # Scalars
    # Props
    # Advanced
    # Diffuse
    # Devel
    #-------------------------------------------

    #puts "Lauren in DTMRIBuildGUI, fix the frame hierarchy comment"

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    This module allows visualization of DTMRI-valued data, 
especially Diffusion DTMRI MRI.
    <P>

    For <B>tractography</B>, point at the voxel of interest with the mouse and click\n the letter '<B>s</B>' (for start, or streamline). To <B>delete</B> a tract, point at it and click '<B>d</B>' (for delete).

    <P>
    Description by tab:
    <BR>
    <B>Disp (Visualization and Display Settings Tab)</B>
    <BR>
    <UL>
    <LI><B>3D View Settings:</B> click 'DTMRIs' view for transparent slices (this makes it easier to see 3D glyphs and tracts). 
    <LI><B>Display Glyphs:</B> turn glyphs on and off. Glyphs are little models for each DTMRI.  They display the eigensystem (principal directions of diffusion).
    <LI><B>Glyphs on Slice:</B> glyphs are displayed in the 3D view over this reformatted slice.  The slice-selection buttons are colored to match the colors of the three slice windows at the bottom of the Viewer window.
    <LI><B>Display Tracts:</B> turn display of tracts on and off, or delete all tracts.  Tracts are seeded when you point the mouse and hit the 's' key.  There are many more settings for tracts under the Visualization Menu below.
    <LI><B>Visualization Menu:</B> Settings for Tracts and Glyphs.
    </UL>


    <P>
    <B>Props Tab</B>
    <BR>
    <UL>
    <LI>This tab is for file reading/DTMRI conversion.
    </UL>
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags DTMRI $help
    MainHelpBuildGUI DTMRI
    
    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(DTMRI,fDisplay)
    set f $fDisplay

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fSettings  -bg $Gui(activeWorkspace)
    pack $f.fSettings -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fGlyphsMode  -bg $Gui(activeWorkspace)
    pack $f.fGlyphsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fReformat  -bg $Gui(activeWorkspace)
    pack $f.fReformat -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fTractsMode  -bg $Gui(activeWorkspace)
    pack $f.fTractsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fVisMethods  -bg $Gui(activeWorkspace)
    pack $f.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true
    #$f.fVisMethods config -relief groove -bd 3
    $f.fVisMethods config -relief sunken -bd 2

    foreach frame "VisUpdate" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    
    #-------------------------------------------
    # Display->Active frame
    #-------------------------------------------
    set f $fDisplay.fActive

    # menu to select active DTMRI
    DevAddSelectButton  DTMRI $f Active "Active DTMRI:" Grid \
            "Active DTMRI" 13 BLA
    
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

    foreach vis $DTMRI(mode,visualizationSettingsTypeList) tip $DTMRI(mode,visualizationSettingsTypeList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,visualizationSettingsType) \
                -command {DTMRIApplyVisualizationSettings} \
                -indicatoron 0} $Gui(WCA) 
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }   

    #-------------------------------------------
    # Display->Reformat frame
    #-------------------------------------------
    set f $fDisplay.fReformat

    DevAddLabel $f.l "Glyphs on Slice:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    set colors [list  $Gui(slice0) $Gui(slice1) $Gui(slice2) $Gui(activeWorkspace)]
    set widths [list  2 2 2 4]

    foreach vis $DTMRI(mode,reformatTypeList) \
    tip $DTMRI(mode,reformatTypeList,tooltips) \
    text $DTMRI(mode,reformatTypeList,text) \
    color $colors \
    width $widths {
        eval {radiobutton $f.rMode$vis \
              -text "$text" -value "$vis" \
              -variable DTMRI(mode,reformatType) \
              -command {DTMRIUpdateReformatType} \
              -indicatoron 0 } $Gui(WCA) \
        {-bg $color -selectcolor $color -width $width}
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }
    
    #-------------------------------------------
    # Display->GlyphsMode frame
    #-------------------------------------------
    set f $fDisplay.fGlyphsMode

    eval {label $f.lVis -text "Display Glyphs: "} $Gui(WLA)
    pack $f.lVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationType,glyphsOnList) \
    tip $DTMRI(mode,visualizationType,glyphsOnList,tooltip) {
        eval {radiobutton $f.r$vis \
              -text $vis \
              -command "DTMRIUpdate" \
              -value $vis \
              -variable DTMRI(mode,visualizationType,glyphsOn) \
          -indicatoron 0} $Gui(WCA)

        pack $f.r$vis -side left -fill x
        TooltipAdd $f.r$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods

    frame $f.fVisMode -bg $Gui(activeWorkspace) 
    pack $f.fVisMode -side top -padx 0 -pady 0 -fill x

    # note the height is necessary to place frames inside later
    frame $f.fVisParams -bg $Gui(activeWorkspace) -height 338
    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    $f.fVisMode config -relief sunken -bd 2
    #$f.fVisParams config -relief groove -bd 3
    #$f.fVisParams config -bd 3

    #-------------------------------------------
    # Display->TractsMode frame
    #-------------------------------------------
    set f $fDisplay.fTractsMode

    eval {label $f.lVis -text "Display 'Tracts': "} $Gui(WLA)
    pack $f.lVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationType,tractsOnList) \
    tip $DTMRI(mode,visualizationType,tractsOnList,tooltip) {
        eval {radiobutton $f.r$vis \
              -text $vis \
              -command "DTMRIUpdateStreamlines" \
              -value $vis \
              -variable DTMRI(mode,visualizationType,tractsOn) \
          -indicatoron 0} $Gui(WCA)

        pack $f.r$vis -side left -fill x
        TooltipAdd $f.r$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisMode frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods.fVisMode

    eval {label $f.lVis -text "Visualization Menu: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,visualizationTypeGui) \
            -relief raised -bd 2 -width 10 \
            -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationTypeGuiList) {
        $f.mbVis.m add command -label $vis \
                -command "DTMRIRaiseMoreOptionsFrame $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbVisMode) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,visualizationTypeGuiList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams frame
    #-------------------------------------------
    set f $fDisplay.fVisMethods.fVisParams
    set fParams $f

    # make a parameters frame for each visualization type
    # types are: Help Glyphs Tracts
    foreach frame $DTMRI(mode,visualizationTypeGuiList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
        set DTMRI(frame,$frame) $f.f$frame
    }
    raise $DTMRI(frame,$DTMRI(mode,visualizationTypeGui))

    ##########################################################
    #  HELP  (frame raised when Help is selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->Help frame
    #-------------------------------------------
    set f $fParams.fHelp

    DevAddLabel $f.l "Select from the Visualization\n menu above to adjust\n visualization parameters."
    DevAddLabel $f.l2 "For tractography, point at \nthe voxel of interest with\n the mouse and click\n the letter 's'. To delete\n a tract, point and click 'd'."
    pack $f.l $f.l2 -side top -padx $Gui(pad) -pady $Gui(pad)

    ##########################################################
    #  GLYPHS   (frame raised when Glyphs are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs frame
    #-------------------------------------------
    set f $fParams.fGlyphs

    foreach frame "GlyphType Lines Colors ScalarBar GlyphScalarRange Slider" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->GlyphType frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fGlyphType

    DevAddLabel $f.l "Glyph Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    eval {menubutton $f.mbVis -text $DTMRI(mode,glyphType) \
            -relief raised -bd 2 -width 12 \
            -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack  $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,glyphTypeList) {
        $f.mbVis.m add command -label $vis \
                -command "$f.mbVis config -text $vis; set DTMRI(mode,glyphType) $vis; DTMRIUpdate"
    }
    # save menubutton for config
    set DTMRI(gui,mbGlyphType) $f.mbVis
    # Add a tooltip
    #TooltipAdd $f.mbVis $DTMRI(mode,glyphColorList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Lines frame
    #-------------------------------------------

    set f $fParams.fGlyphs.fLines

    DevAddLabel $f.l "Line Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $DTMRI(mode,glyphEigenvectorList) tip $DTMRI(mode,glyphEigenvectorList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,glyphEigenvector) \
                -command DTMRIUpdateGlyphEigenvector \
                -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 1
        TooltipAdd $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->Colors frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fColors

    eval {label $f.lVis -text "Color by: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,glyphColor) \
            -relief raised -bd 2 -width 12 \
            -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,glyphColorList) {
        $f.mbVis.m add command -label $vis \
                -command "set DTMRI(mode,glyphColor) $vis; DTMRIUpdateGlyphColor"
    }
    # save menubutton for config
    set DTMRI(gui,mbGlyphColor) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,glyphColorList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams->Glyphs->ScalarBar frame
    #-------------------------------------------
    set f $fParams.fGlyphs.fScalarBar

    DevAddLabel $f.l "Scalar Bar:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $DTMRI(mode,scalarBarList) tip $DTMRI(mode,scalarBarList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,scalarBar) \
                -command {DTMRIUpdateScalarBar} \
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

    foreach vis $DTMRI(mode,glyphScalarRangeList) tip $DTMRI(mode,glyphScalarRangeList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,glyphScalarRange) \
                -command {DTMRIUpdateGlyphScalarRange; Render3D} \
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
                -textvariable DTMRI(mode,glyphScalarRange,[Uncap $slider])} \
                $Gui(WEA)
        eval {scale $f.s$slider -from $DTMRI(mode,glyphScalarRange,min) \
                -to $DTMRI(mode,glyphScalarRange,max) \
                -length 90 \
                -variable DTMRI(mode,glyphScalarRange,[Uncap $slider]) \
                -resolution 0.1 \
                -command {DTMRIUpdateGlyphScalarRange; Render3D}} \
                $Gui(WSA) {-sliderlength 15}
        pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
        set DTMRI(gui,slider,$slider) $f.s$slider
        bind $f.e${slider} <Return>   \
                "DTMRIUpdateGlyphScalarRange ${slider}; Render3D"

    }

    ##########################################################
    #  TRACTS   (frame raised when Tracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts frame
    #-------------------------------------------
    set f $fParams.fTracts

    foreach frame "Colors Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts->Colors frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors
    foreach frame "ChooseColor ColorBy " {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts->Colors->ChooseColor frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fChooseColor

    DevAddButton $f.b "Color:" "ShowColors"
    eval {entry $f.e -width 20 \
            -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    pack $f.b $f.e -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

    lappend Label(colorWidgetList) $f.e

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts->Colors->ColorBy frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fColorBy

    eval {label $f.lVis -text "Color by: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,tractColor) \
            -relief raised -bd 2 -width 12 \
            -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,tractColorList) {
        $f.mbVis.m add command -label $vis \
                -command "set DTMRI(mode,tractColor) $vis; DTMRIUpdateTractColor"
    }
    # save menubutton for config
    set DTMRI(gui,mbTractColor) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,tractColorList,tooltip)

    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts->Entries frame
    #-------------------------------------------
    foreach entry $DTMRI(stream,variableList) \
        text $DTMRI(stream,variableList,text) \
        tip $DTMRI(stream,variableList,tooltips) {

    set f $fParams.fTracts.fEntries

        frame $f.f$entry -bg $Gui(activeWorkspace)
        pack $f.f$entry -side top -padx $Gui(pad) -pady 1 -fill x
        set f $f.f$entry

        eval {label $f.l$entry -text "$text:"} $Gui(WLA)
        eval {entry $f.e$entry -width 10 \
                -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
    TooltipAdd $f.l$entry $tip
    TooltipAdd $f.e$entry $tip
        pack $f.l$entry -side left  -padx $Gui(pad)
        pack $f.e$entry -side right  -padx $Gui(pad)
    }

    ##########################################################
    #  AUTOTRACTS   (frame raised when AutoTracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->AutoTracts frame
    #-------------------------------------------
    set f $fParams.fAutoTracts

    foreach frame "Label2 Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->Autotracts->Label2 frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fLabel2

    DevAddLabel $f.l "Automatically start tractography\nfrom each voxel in an ROI."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Display->VisMethods->VisParams->Tracts->Entries frame
    set f $fParams.fAutoTracts.fEntries
    foreach frame "Volume ChooseLabel Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------

    #-------------------------------------------
    # Display->VisMethods->VisParams->AutoTracts->Entries->Volume frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fVolume

    # menu to select a volume: will set Volume(activeID)
    set name MaskLabelmap
    DevAddSelectButton  Volume $f $name "ROI Labelmap:" Grid \
    "This labelmap will be used to seed tracts."\
            13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # Display->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel

    # mask label
    eval {button $f.bOutput -text "Label:" \
            -command "ShowLabels DTMRIUpdateMaskLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
            -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "DTMRIUpdateMaskLabel"
    bind $f.eOutput <FocusOut> "DTMRIUpdateMaskLabel"
    eval {entry $f.eName -width 14 \
            -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    TooltipAdd  $f.bOutput $DTMRI(mode,autoTractsLabel,tooltip)
    TooltipAdd  $f.eOutput $DTMRI(mode,autoTractsLabel,tooltip)
    TooltipAdd  $f.eName $DTMRI(mode,autoTractsLabel,tooltip)


    #-------------------------------------------
    # Display->VisMethods->VisParams->AutoTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fApply
    DevAddButton $f.bApply "Seed 'Tracts' in ROI" \
        {puts "Seeding streamlines"; DTMRISeedStreamlinesFromSegmentation}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Seed a 'tract' from each point in the ROI.\nThis can be slow; be patient."



    ##########################################################
    #  SAVETRACTS   (frame raised when SaveTracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display->VisMethods->VisParams->SaveTracts frame
    #-------------------------------------------
    set f $fParams.fSaveTracts

    foreach frame "Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->SaveTracts->Entries frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries
    foreach frame "Info Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display->VisMethods->VisParams->SaveTracts->Entries->Info frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fInfo
    DevAddLabel $f.l "Save currently visible\ntracts as a model."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display->VisMethods->VisParams->SaveTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fApply
    DevAddButton $f.bApply "Save tracts in model tracts.vtk" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsModel "" tracts}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save visible tracts to vtk file.  Must be re-added to mrml tree."

    #-------------------------------------------
    # Display->VisUpdate frame
    #-------------------------------------------
    #set f $fDisplay.fVisUpdate
    #DevAddButton $f.bTest "Junk" {puts "this button is junk"} 4    
    #pack $f.bTest -side top -padx 0 -pady 0


    #-------------------------------------------
    # ROI frame
    #-------------------------------------------
    set fROI $Module(DTMRI,fROI)
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

    # menu to select active DTMRI
    DevAddSelectButton  DTMRI $f Active "Active DTMRI:" Grid \
            "Active DTMRI" 13 BLA
    
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

    DevAddLabel $f.l "DTMRI Threshold Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Threshold->Mode frame
    #-------------------------------------------
    set f $fROI.fThreshold.fMode

    DevAddLabel $f.l "Value:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(mode,thresholdList) tip $DTMRI(mode,thresholdList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,threshold) \
                -command {DTMRIUpdate} \
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
                -textvariable DTMRI(thresh,threshold,[Uncap $slider])} \
                $Gui(WEA)
        bind $f.e$slider <Return>   "DTMRIUpdateThreshold"
        #bind $f.e$slider <FocusOut> "EdThresholdUpdate; RenderActive;"
        eval {scale $f.s$slider -from $DTMRI(thresh,threshold,rangeLow) \
                -to $DTMRI(thresh,threshold,rangeHigh) \
                -length 130 \
                -variable DTMRI(thresh,threshold,[Uncap $slider]) \
                -resolution 0.1 \
                -command {DTMRIUpdateThreshold}} \
                $Gui(WSA) {-sliderlength 15}
        #grid $f.l$slider $f.e$slider -padx 2 -pady 2 -sticky w
        #grid $f.l$slider -sticky e
        #grid $f.s$slider -columnspan 2 -pady 2 
        pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
        set DTMRI(gui,slider,$slider) $f.s$slider
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

    DevAddLabel $f.l "DTMRI Mask Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Mask->Mode frame
    #-------------------------------------------
    set f $fROI.fMask.fMode

    DevAddLabel $f.l "Mask:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(mode,maskList) tip $DTMRI(mode,maskList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(mode,mask) \
                -command {DTMRIUpdate} \
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
    DevAddSelectButton  DTMRI $f $name "Mask Labelmap:" Grid \
            "Select a labelmap volume to use as a mask.\nDTMRIs will be displayed only where the label matches the label you select below." \
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
            -command "ShowLabels DTMRIUpdateMaskLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
            -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "DTMRIUpdateMaskLabel"
    bind $f.eOutput <FocusOut> "DTMRIUpdateMaskLabel"
    eval {entry $f.eName -width 14 \
            -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    TooltipAdd  $f.bOutput $DTMRI(mode,maskLabel,tooltip)
    TooltipAdd  $f.eOutput $DTMRI(mode,maskLabel,tooltip)
    TooltipAdd  $f.eName $DTMRI(mode,maskLabel,tooltip)

    #-------------------------------------------
    # Scalars frame
    #-------------------------------------------
    set fScalars $Module(DTMRI,fScalars)
    set f $fScalars
    
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "Top" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
        $f.f$frame config -relief groove -bd 3
    }

    #-------------------------------------------
    # Scalars->Active frame
    #-------------------------------------------
    set f $fScalars.fActive

    # menu to select active DTMRI
    DevAddSelectButton  DTMRI $f Active "Active DTMRI:" Grid \
            "Active DTMRI" 13 BLA
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m
    
    #-------------------------------------------
    # Scalars->Top frame
    #-------------------------------------------
    set f $fScalars.fTop
    
    foreach frame "ChooseOutput UseROI ScaleFactor Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Scalars->Top->ChooseOutput frame
    #-------------------------------------------
    set f $fScalars.fTop.fChooseOutput

    eval {label $f.lMath -text "Create Volume: "} $Gui(WLA)
    eval {menubutton $f.mbMath -text $DTMRI(scalars,operation) \
            -relief raised -bd 2 -width 12 \
            -menu $f.mbMath.m} $Gui(WMBA)

    eval {menu $f.mbMath.m} $Gui(WMA)
    pack $f.lMath $f.mbMath -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach math $DTMRI(scalars,operationList) {
        $f.mbMath.m add command -label $math \
                -command "DTMRISetOperation $math"
    }
    # save menubutton for config
    set DTMRI(gui,mbMath) $f.mbMath
    # Add a tooltip
    TooltipAdd $f.mbMath $DTMRI(scalars,operationList,tooltip)

    #-------------------------------------------
    # Scalars->Top->UseROI frame
    #-------------------------------------------
    set f $fScalars.fTop.fUseROI

    DevAddLabel $f.l "ROI:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(scalars,ROIList) tip $DTMRI(scalars,ROIList,tooltips) {
        eval {radiobutton $f.rMode$vis \
                -text "$vis" -value "$vis" \
                -variable DTMRI(scalars,ROI) \
                -command DTMRIUpdateMathParams \
                -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # Scalars->Top->ScaleFactor frame
    #-------------------------------------------
    set f $fScalars.fTop.fScaleFactor
    DevAddLabel $f.l "Scale Factor:"
    eval {entry $f.e -width 14 \
            -textvariable DTMRI(scalars,scaleFactor)} $Gui(WEA)
    TooltipAdd $f.e $DTMRI(scalars,scaleFactor,tooltip) 
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # Scalars->Top->Apply frame
    #-------------------------------------------
    set f $fScalars.fTop.fApply

    DevAddButton $f.bApply "Apply" "DTMRIDoMath"    
    pack $f.bApply -side top -padx 0 -pady 0

    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(DTMRI,fProps)
    set f $fProps
    
    foreach frame "Top Middle Bottom Convert" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    $f.fConvert configure  -relief groove -bd 3 

    #-------------------------------------------
    # Props->Top frame
    #-------------------------------------------
    set f $fProps.fTop
    # file browse box: widget, array, var, label, proc, "", startdir
    DevAddFileBrowse $f DTMRI FileName "File" \
            DTMRISetFileName "" {\$DTMRI(defaultDir)}
    # Lauren does default dir work?

    #-------------------------------------------
    # Props->Bottom frame
    #-------------------------------------------
    set f $fProps.fBottom
    # Lauren test
    DevAddButton $f.bTest "Add" MainTensorAddTensor 8

    DevAddButton $f.bApply "Apply" "DTMRIPropsApply; Render3D" 8
    DevAddButton $f.bCancel "Cancel" "DTMRIPropsCancel" 8

    # Lauren test
    grid $f.bTest $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)
    #grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Props->Convert frame
    #-------------------------------------------
    set f $fProps.fConvert

    foreach frame "Select GradientNum GradientImages NoGradientImages Gradients Apply" {
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
            "Input Volume to create DTMRIs from." 13 BLA

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Props->Convert->GradientNum frame
    #-------------------------------------------
    set f $fProps.fConvert.fGradientNum
    
    DevAddLabel $f.l "Number of Gradient Directions:"
    eval {entry $f.eEntry -textvariable DTMRI(convert,numberOfGradients) \
          -width 5} $Gui(WEA)
    pack $f.l $f.eEntry -side left -padx $Gui(pad) -pady 0 -fill x

    #-------------------------------------------
    # Props->Convert->GradientImages frame
    #-------------------------------------------
    set f $fProps.fConvert.fGradientImages
    DevAddLabel $f.l "Gradient:"
    eval {entry $f.eEntry1 \
          -textvariable DTMRI(convert,firstGradientImage) \
          -width 5} $Gui(WEA)
    eval {entry $f.eEntry2 \
          -textvariable DTMRI(convert,lastGradientImage) \
          -width 5} $Gui(WEA)
    pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
    TooltipAdd $f.eEntry1 \
    "First gradient (diffusion-weighted)\nimage number at first slice location"
    TooltipAdd $f.eEntry2 \
    "Last gradient (diffusion-weighted)\niimage number at first slice location"

    #-------------------------------------------
    # Props->Convert->NoGradientImages frame
    #-------------------------------------------
    set f $fProps.fConvert.fNoGradientImages
    DevAddLabel $f.l "Baseline:"
    eval {entry $f.eEntry1 \
          -textvariable DTMRI(convert,firstNoGradientImage) \
          -width 5} $Gui(WEA)
    eval {entry $f.eEntry2 \
          -textvariable DTMRI(convert,lastNoGradientImage) \
          -width 5} $Gui(WEA)
    pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
    TooltipAdd $f.eEntry1 \
    "First NO gradient (not diffusion-weighted)\nimage number at first slice location"
    TooltipAdd $f.eEntry2 \
    "Last NO gradient (not diffusion-weighted)\n image number at first slice location"

    #-------------------------------------------
    # Props->Convert->Gradients frame
    #-------------------------------------------
    set f $fProps.fConvert.fGradients
    eval {entry $f.eEntry -textvariable DTMRI(convert,gradients) \
          -width 20} $Gui(WEA)
    pack $f.eEntry -side top -padx 0 -pady $Gui(pad) -fill x
    TooltipAdd $f.eEntry "List of diffusion gradient directions"

    #-------------------------------------------
    # Props->Convert->Apply frame
    #-------------------------------------------
    set f $fProps.fConvert.fApply
    DevAddButton $f.bTest "Convert Volume" ConvertVolumeToTensors 20
    pack $f.bTest -side top -padx 0 -pady $Gui(pad) -fill x

    #-------------------------------------------
    # Advanced frame
    #-------------------------------------------
    set fAdvanced $Module(DTMRI,fAdvanced)
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
    set fScrolled [DTMRIBuildScrolledGUI $f]
    set DTMRI(scrolledGui,advanced) $fScrolled

    # loop through all the vtk objects and build GUIs
    #------------------------------------
    foreach o $DTMRI(vtkObjectList) {

        set f $fScrolled

        # if the object has properties
        #-------------------------------------------
        if {$DTMRI(vtkPropertyList,$o) != ""} {
            
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
        foreach p $DTMRI(vtkPropertyList,$o) {

            set f $fScrolled.f$o

            # name of entire tcl variable
            set variableName DTMRI(vtk,$o,$p)
            # its value is:
            set value $DTMRI(vtk,$o,$p)
            # description of the parameter
            set desc $DTMRI(vtk,$o,$p,description)
            # datatype of the parameter
            set type $DTMRI(vtk,$o,$p,type)

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
                    # puts "bool: $variableName, $desc"
                    eval {checkbutton $f.r$p  \
                            -text $desc -variable $variableName \
                        } $Gui(WCA)
                    
                    pack  $f.r$p -side left \
                            -padx $Gui(pad) -pady 2
                }
            }
            
        }
    }
    # end foreach vtk object in DTMRIs object list

    # Here's a button with text "Apply" that calls "AdvancedApply"
    DevAddButton $fAdvanced.fMiddle.bApply Apply DTMRIAdvancedApply
    pack $fAdvanced.fMiddle.bApply -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Diffuse frame
    #-------------------------------------------
    set fDiffuse $Module(DTMRI,fDiffuse)
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

    DevAddButton $f.bRun "Run Diffusion" {DTMRIDoDiffusion}
    pack $f.bRun -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Devel frame
    #-------------------------------------------
    set fDevel $Module(DTMRI,fDevel)
    set f $fDevel
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    $f.fTop configure  -relief groove -bd 3 
    $f.fMiddle configure  -relief groove -bd 3 

    #-------------------------------------------
    # Devel->Top frame
    #-------------------------------------------
    set f $fDevel.fTop

    DevAddButton $f.bSave "Save Structured Points" {DTMRIWriteStructuredPoints $DTMRI(devel,fileName)}
    pack $f.bSave -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd $f.bSave "Save DTMRIs.vtk"

    #-------------------------------------------
    # Devel->Middle frame
    #-------------------------------------------
    set f $fDevel.fMiddle
    frame $f.fEntry  -bg $Gui(activeWorkspace)
    frame $f.fEntry2  -bg $Gui(activeWorkspace)
    frame $f.fButton  -bg $Gui(activeWorkspace)
    pack $f.fEntry $f.fEntry2 $f.fButton -side top -padx $Gui(pad) -pady $Gui(pad)

#     set rows {0 1 2 3}
#     set cols {0 1 2 3}    
#     foreach row $rows {
#         set f $fDevel.fMiddle.fEntry
#         frame $f.f$row
#         pack $f.f$row -side top -padx $Gui(pad) -pady $Gui(pad)
#         set f $f.f$row
#         foreach col $cols {
#             eval {entry $f.e$col -width 5 \
#                     -textvariable \
#                     DTMRI(recalculate,userMatrix,$row,$col) \
#                 } $Gui(WEA)
#             pack $f.e$col -side left -padx $Gui(pad) -pady 2        
#         }
#     }    
#     set f $fDevel.fMiddle.fButton
#     DevAddButton $f.bApply "Recalculate DTMRIs" {DTMRIRecalculateDTMRIs}
#     pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad) 

    set f $fDevel.fMiddle.fEntry
    
    DevAddLabel $f.l "dir:"
    eval {entry $f.e -width 25 -textvariable \
              DTMRI(devel,subdir) \
          } $Gui(WEA)    
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 2        
    TooltipAdd $f.e "Directory where text files with points will be saved"

    set f $fDevel.fMiddle.fEntry2
    DevAddLabel $f.l "name:"
    eval {entry $f.e -width 25 -textvariable \
              DTMRI(devel,fileNamePoints) \
          } $Gui(WEA)    
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 2        
    TooltipAdd $f.e "Filename prefix of text files with points"

    set f $fDevel.fMiddle.fButton
    DevAddButton $f.bApply "Save streamlines in scaled IJK" \
        {DTMRISaveStreamlinesAsIJKPoints $DTMRI(devel,subdir) $DTMRI(devel,fileNamePoints)}

    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad) 
}


#-------------------------------------------------------------------------------
# .PROC DTMRIRaiseMoreOptionsFrame
# 
# .ARGS
# string mode identifies the frame to raise
# .END
#-------------------------------------------------------------------------------
proc DTMRIRaiseMoreOptionsFrame {mode} {
    global DTMRI

    raise $DTMRI(frame,$mode)
    focus $DTMRI(frame,$mode)

    set DTMRI(mode,visualizationTypeGui) $mode
    # config menubutton
    $DTMRI(gui,mbVisMode)    config -text $mode
}

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildScrolledGUI
# 
# .ARGS
# string f path to frame
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildScrolledGUI {f} {
    global Gui DTMRI

    # window paths
    #-------------------------------------------
    set canvas $f.cGrid
    set s $f.sGrid
    
    # create canvas and scrollbar
    #-------------------------------------------
    canvas $canvas -yscrollcommand "$s set" -bg $Gui(activeWorkspace)
    eval "scrollbar $s -command \"DTMRICheckScrollLimits $canvas yview\"\
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
    set DTMRI(scrolledGUI,$f,canvas) $canvas

    # return path to the frame for filling
    return $f
}


#-------------------------------------------------------------------------------
# .PROC DTMRICheckScrollLimits
# This procedure allows scrolling only if the entire frame is not visible
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICheckScrollLimits {args} {

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
# .PROC DTMRISizeScrolledGUI
# 
# .ARGS
# string f path to frame
# .END
#-------------------------------------------------------------------------------
proc DTMRISizeScrolledGUI {f} {
    global DTMRI

    # see how tall our frame is these days
    set height [winfo reqheight  $f]    
    set canvas $DTMRI(scrolledGUI,$f,canvas)

    # tell the canvas to scroll so we can see the whole frame
    $canvas config -scrollregion "0 0 1 $height"
}

#-------------------------------------------------------------------------------
# .PROC DTMRISetPropertyType
# raise a panel in the GUI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISetPropertyType {} {
    global Module DTMRI

    #Lauren need properties subframes for this to work
    #raise $Volume(f$Volume(propertyType))
    #focus $Volume(f$Volume(propertyType))

    # Lauren temporarily use this
    raise $Module(DTMRI,f$DTMRI(propertyType))
}

################################################################
#  Procedures called by the GUI: Apply, Cancel, etc.
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIPropsApply
#  Apply the settings from the GUI into the currently
#  active MRML node (this is NEW or an existing node).  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIPropsApply {} {
    global DTMRI Module Tensor
    puts "Lauren in DTMRIPropsApply, note this only works for NEW currently"
    
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
    set newvol [MainMrmlAddNode Volume Tensor]
        set i [$newvol GetID]

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
        set DTMRI(freeze) 0

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

    # Update MRML to apply changed settings
    #--------------------------------------------------------
    MainUpdateMRML
    puts "end of DTMRI props apply"
}

#-------------------------------------------------------------------------------
# .PROC DTMRIPropsCancel
# Cancel 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIPropsCancel {} {
    global DTMRI Module
    puts "Lauren in DTMRIPropsCancel: need to get out of freezer"

    set DTMRI(freeze) 0
    # If we gave up on adding a NEW object, unfreeze tabs now
    #--------------------------------------------------------
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAdvancedApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAdvancedApply {} {
    global DTMRI    
    puts "Applying"
    DTMRIApplyVisualizationParameters    
    puts "done applying"
    
    # display our changes
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRISetFileName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISetFileName {} {
    global DTMRI
    
    # Do nothing if the user cancelled
    if {$DTMRI(FileName) == ""} {return}
    
    # Update the Default Directory
    set DTMRI(DefaultDir) [file dirname $DTMRI(FileName)]
    
    # Name the DTMRIs based on the entered file.
    set DTMRI(Name) [ file root [file tail $DTMRI(FileName)]]
}


################################################################
#  bindings for user interaction
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRICreateBindings
#  Makes bindings for the module.  These are in effect when module is entered
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICreateBindings {} {
    global Gui Ev; # CustomCsys Csys
        
    #EvDeclareEventHandler DTMRICsysEvents <KeyPress-c> {CustomCsysDoSomethingCool}

    # this seeds a stream when the l key is hit (use s instead, it's nicer)
    EvDeclareEventHandler DTMRISlicesStreamlineEvents <KeyPress-l> \
    { if { [SelectPick2D %W %x %y] != 0 } \
          {  eval DTMRISelectStartHyperStreamline $Select(xyz); Render3D } }
    # this seeds a stream when the s key is hit
    EvDeclareEventHandler DTMRISlicesStreamlineEvents <KeyPress-s> \
    { if { [SelectPick2D %W %x %y] != 0 } \
          {  eval DTMRISelectStartHyperStreamline $Select(xyz); Render3D } }
    
    EvAddWidgetToBindingSet DTMRISlice0Events $Gui(fSl0Win) {DTMRISlicesStreamlineEvents}
    EvAddWidgetToBindingSet DTMRISlice1Events $Gui(fSl1Win) {DTMRISlicesStreamlineEvents}
    EvAddWidgetToBindingSet DTMRISlice2Events $Gui(fSl2Win) {DTMRISlicesStreamlineEvents}

    # this seeds a stream when the l key is hit (use s instead, it's nicer)
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-l> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectStartHyperStreamline $Select(xyz);Render3D } }
    # this seeds a stream when the s key is hit
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-s> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectStartHyperStreamline $Select(xyz);Render3D } }

    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-d> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectRemoveHyperStreamline $Select(xyz);Render3D } }

    # This contains all the regular events from tkInteractor.tcl, 
    # which will happen after ours.  For some reason we don't need 
    # this for the slice windows, apparently their original bindings
    # are not done using Ev.tcl and they stay even when we add ours.
    EvAddWidgetToBindingSet DTMRI3DEvents $Gui(fViewWin) {{DTMRI3DStreamlineEvents} {tkMouseClickEvents} {tkMotionEvents} {tkRegularEvents}}
}




################################################################
#  little procedures to handle display control, interaction with user
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRIRoundFloatingPoint
# Format floats for GUI display (we don't want -5e-11)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIRoundFloatingPoint {val} {
    global DTMRI

    return [format $DTMRI(floatingPointFormat) $val]
}


#-------------------------------------------------------------------------------
# .PROC DTMRIRemoveAllActors
# Rm all actors from scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIRemoveAllActors {} {
    global DTMRI
    
    # rm glyphs
    MainRemoveActor DTMRI(vtk,glyphs,actor)

    # rm streamlines
    DTMRIRemoveAllStreamlines

    Render3D

}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddAllActors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddAllActors {} {
    global DTMRI
    
    # rm glyphs
    MainAddActor DTMRI(vtk,glyphs,actor)

    # rm streamlines
    DTMRIAddAllStreamlines

    Render3D

}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateReformatType
#  Reformat the requested slice (from GUI input) or all.  Then call
#  pipeline update proc (DTMRIUpdate) to make this happen.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateReformatType {} {
    global DTMRI

    set mode $DTMRI(mode,reformatType)

    set result ok

    # make sure we don't display all DTMRIs by accident
    switch $mode {
        "None" {
        set message "This will display ALL DTMRIs.  If the volume is not masked using a labelmap or threshold ROI, this may take a long time or not work on your machine.  Proceed?"
        set result [tk_messageBox -type okcancel -message $message]
    }
    }

    # display what was requested
    if {$result == "ok"} {
    DTMRIUpdate
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateScalarBar
# Display scalar bar for glyph coloring
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateScalarBar {} {
    global DTMRI

    set mode $DTMRI(mode,scalarBar)

    switch $mode {
        "On" {
            DTMRIShowScalarBar
        }
        "Off" {
            DTMRIHideScalarBar
        }
    }

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIShowScalarBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIShowScalarBar {} {
    DTMRIUpdateGlyphScalarRange
    DTMRI(vtk,scalarBar,actor) VisibilityOn
    #viewRen AddProp DTMRI(vtk,scalarBar,actor)
}

#-------------------------------------------------------------------------------
# .PROC DTMRIHideScalarBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIHideScalarBar {} {
    DTMRI(vtk,scalarBar,actor) VisibilityOff
    #viewRen RemoveActor DTMRI(vtk,scalarBar,actor)
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateThreshold
# If we are thresholding the glyphs to display a subvolume and
# the user requests a new threshold range this is called.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateThreshold {{not_used ""}} {
    global DTMRI
    
    DTMRI(vtk,thresh,threshold)  ThresholdBetween \
            $DTMRI(thresh,threshold,lower) \
            $DTMRI(thresh,threshold,upper)

    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMaskLabel
# If we are masking the glyphs to display a subvolume and
# the user requests a new mask label value this is called. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMaskLabel {} {
    global Label

    LabelsFindLabel

    # this label becomes 1 in the mask
    set thresh DTMRI(vtk,mask,threshold)
    $thresh ThresholdBetween $Label(label) $Label(label)

    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateActor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateActor {actor} {
    global DTMRI
    
    [$actor GetProperty] SetAmbient $DTMRI(actor,ambient)
    [$actor GetProperty] SetDiffuse $DTMRI(actor,diffuse)
    [$actor GetProperty] SetSpecular $DTMRI(actor,specular)
    [$actor GetProperty] SetOpacity $DTMRI(actor,opacity)

}

#-------------------------------------------------------------------------------
# .PROC DTMRISpecificVisualizationSettings
# Set up visualization to see DTMRIs well.
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
proc DTMRISpecificVisualizationSettings {} {
    global Anno

    # let us see DTMRIs through the slices
    MainSlicesSet3DOpacityAll 0.5
    #MainSlicesSet3DOpacityAll 0.1
    #MainViewSetBackgroundColor Black
    # show all digits of float data (i.e. trace)
    MainAnnoSetPixelDisplayFormat "full"
    # turn off those irritating letters
    set Anno(letters) 0
    MainAnnoSetVisibility
}

#-------------------------------------------------------------------------------
# .PROC DTMRIResetDefaultVisualizationSettings
# Undo the DTMRISpecific settings and use defaults 
# of the slicer.  
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIResetDefaultVisualizationSettings {} {
    global Anno

    # let us see DTMRIs through the slices
    MainSlicesReset3DOpacityAll
    #MainViewSetBackgroundColor Blue
    # show all digits of float data (i.e. trace)
    MainAnnoSetPixelDisplayFormat "default"
    # turn back on those irritating letters
    #set Anno(letters) 1
    MainAnnoSetVisibility
}

#-------------------------------------------------------------------------------
# .PROC DTMRIApplyVisualizationSettings
#  Set the settings the user requested, default or for DTMRIs
# .ARGS
# str mode optional, default or DTMRIs
# .END
#-------------------------------------------------------------------------------
proc DTMRIApplyVisualizationSettings {{mode ""}} {
    global DTMRI
    
    if {$mode == ""} {
        set mode $DTMRI(mode,visualizationSettingsType)
    }
    
    switch $mode {
        "default" {
            DTMRIResetDefaultVisualizationSettings
        }
        "DTMRIs" {
            DTMRISpecificVisualizationSettings
        }
    }

    Render3D
}

################################################################
#  visualization procedures that deal with glyphs
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateGlyphEigenvector
# choose max middle or min for display
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateGlyphEigenvector {} {
    global DTMRI

    set mode $DTMRI(mode,glyphEigenvector)

    # Scaling along x-axis corresponds to major 
    # eigenvector, etc.  So move the line to 
    # point along the proper axis for scaling
    switch $mode {
        "Max" {
            DTMRI(vtk,glyphs,line) SetPoint1 -1 0 0
            DTMRI(vtk,glyphs,line) SetPoint2 1 0 0    
        }
        "Middle" {
            DTMRI(vtk,glyphs,line) SetPoint1 0 -1 0
            DTMRI(vtk,glyphs,line) SetPoint2 0 1 0    
        }
        "Min" {
            DTMRI(vtk,glyphs,line) SetPoint1 0 0 -1
            DTMRI(vtk,glyphs,line) SetPoint2 0 0 1    
        }
    }
    # Update pipelines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateGlyphColor
# switch between various color options the user can select for glyphs
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateGlyphColor {} {
    global DTMRI
    
    set mode $DTMRI(mode,glyphColor)
    
    # display new mode while we are working...
    $DTMRI(gui,mbGlyphColor)    config -text $mode
    
    switch $mode {
        "Linear" {
            DTMRI(vtk,glyphs) ColorGlyphsWithLinearMeasure
        }
        "Planar" {
            DTMRI(vtk,glyphs) ColorGlyphsWithPlanarMeasure
        }
        "Spherical" {
            DTMRI(vtk,glyphs) ColorGlyphsWithSphericalMeasure
        }
        "Max" {
            DTMRI(vtk,glyphs) ColorGlyphsWithMaxEigenvalue
        }
        "Middle" {
            DTMRI(vtk,glyphs) ColorGlyphsWithMiddleEigenvalue
        }
        "Min" {
            DTMRI(vtk,glyphs) ColorGlyphsWithMinEigenvalue
        }
        "MaxMinusMiddle" {
            DTMRI(vtk,glyphs) ColorGlyphsWithMaxMinusMidEigenvalue
        }
        "RA" {
            DTMRI(vtk,glyphs) ColorGlyphsWithRelativeAnisotropy
        }
        "FA" {
            DTMRI(vtk,glyphs) ColorGlyphsWithFractionalAnisotropy
        }
        "Direction" {
            DTMRI(vtk,glyphs) ColorGlyphsWithDirection
        }
        
    }
    # Tell actor where to get scalar range
    set DTMRI(mode,glyphScalarRange) Auto
    DTMRIUpdateGlyphScalarRange

    # Update pipelines
    Render3D

}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateGlyphScalarRange
# Called to reset the scalar range displayed to correspond to the 
# numbers output by the current coloring method
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateGlyphScalarRange {{not_used ""}} {
    global DTMRI Tensor

    # make sure we have a DTMRI displayed now
    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
        return
    }

    # make sure the pipeline is up-to-date so we get the right
    # scalar range.  Otherwise the first render will not have
    # the right glyph colors.
    DTMRI(vtk,glyphs) Update

    set mode $DTMRI(mode,glyphScalarRange)

    # find scalar range if not set by user
    switch $mode {
        "Auto" {
            scan [[DTMRI(vtk,glyphs) GetOutput] GetScalarRange] \
                    "%f %f" s1 s2
        }
    "Manual" {
        set s1 $DTMRI(mode,glyphScalarRange,low) 
        set s2 $DTMRI(mode,glyphScalarRange,hi) 
    }
    }

    # make sure that the scalars hi and low are not equal since
    # this causes an error from the mapper
    if {$s2 == $s1} {
    set s1 0
    set s2 1
    }    
    # set this scalar range for glyph display
    DTMRI(vtk,glyphs,mapper) SetScalarRange $s1 $s2

    # Round the scalar range numbers to requested precision
    # This way -4e-12 will not look like a negative eigenvalue in
    # the GUI
    set DTMRI(mode,glyphScalarRange,low) \
            [DTMRIRoundFloatingPoint $s1]
    set DTMRI(mode,glyphScalarRange,hi) \
            [DTMRIRoundFloatingPoint $s2]

    # If we are doing streamlines, tell them how to build LUT
    # Get scalar range from the data since this is how we
    # are coloring streamlines now
    set t $Tensor(activeID)
    scan [[Tensor($t,data) GetOutput] GetScalarRange]  \
        "%f %f" s1 s2
    # make sure that the scalars hi and low are not equal since
    # this causes an error from the mapper
    if {$s2 == $s1} {
    set s1 0
    set s2 1
    }
    foreach id $DTMRI(vtk,streamline,idList) {
        set streamline streamln,$id
        eval {DTMRI(vtk,$streamline,mapper)   SetScalarRange}  \
        $s1 $s2
    }

    # This causes multiple renders since for some reason
    # the scalar bar does not update on the first one
    Render3D
}


################################################################
#  visualization procedures that deal with tracts
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRISelectRemoveHyperStreamline
#  Remove the selected hyperstreamline
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISelectRemoveHyperStreamline {x y z} {
    global DTMRI
    global Select

    puts "Select Picker  (x,y,z):  $x $y $z"

    # see which actor was picked
    set actor [DTMRI(vtk,picker) GetActor]

    # if it's one of our streamlines, remove it from the scene
    foreach id $DTMRI(vtk,streamline,idList) {
        set streamline streamln,$id
    if {[string compare $actor "DTMRI(vtk,$streamline,actor)"] == 0} {
        MainRemoveActor $actor
            $actor SetVisibility 0
    }
    }    
}

#-------------------------------------------------------------------------------
# .PROC DTMRISelectStartHyperStreamline
# Given x,y,z in world coordinates, starts a streamline from that point
# in the active DTMRI dataset.
# .ARGS
# int x 
# int y
# int z 
# .END
#-------------------------------------------------------------------------------
proc DTMRISelectStartHyperStreamline {x y z {render "true"} } {
    global DTMRI Tensor
    global Select

    #puts "Select Picker  (x,y,z):  $x $y $z"


    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
        puts "DTMRISelect: No DTMRIs have been read into the slicer"
        return
    }

    # Get coordinates in scaled-ijk land
    #------------------------------------
    set point [DTMRIGetScaledIjkCoordinatesFromWorldCoordinates $x $y $z]

    # check if inside data set (return value -1 if not)
    #------------------------------------
    foreach p $point {
        if {$p < 0} {
            puts "Point $x $y $z outside of DTMRI dataset"
            return
        }
    }

    #puts "Point $point inside of DTMRI dataset"

    # make new hyperstreamline 
    #------------------------------------
    DTMRIAddStreamline
    set streamln streamln,$DTMRI(vtk,streamline,currentID)

    # start pipeline (never use reformatted data here)
    #------------------------------------
    DTMRI(vtk,$streamln) SetInput [Tensor($t,data) GetOutput]
    
    # start hyperstreamline here
    #------------------------------------
    eval {DTMRI(vtk,$streamln) SetStartPosition} $point
    
    # Make actor visible now that it has inputs
    #------------------------------------
    MainAddActor DTMRI(vtk,$streamln,actor) 
    DTMRI(vtk,$streamln,actor) VisibilityOn

    # Put the output streamline's actor in the right place.
    # Just use the same matrix we use to position the DTMRIs.
    #------------------------------------
    vtkTransform transform
    DTMRICalculateActorMatrix transform $t
    DTMRI(vtk,$streamln,actor) SetUserMatrix [transform GetMatrix]
    transform Delete

    # Force pipeline execution and render scene
    #------------------------------------
    if { $render == "true" } {
        Render3D
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateStreamlines
# show/hide/delete all
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateStreamlines {} {
    global DTMRI
    
    set mode $DTMRI(mode,visualizationType,tractsOn)

    switch $mode {
        "On" {
            # add actors
            DTMRIAddAllStreamlines
        }
        "Off" {
            # hide actors
            DTMRIRemoveAllStreamlines
        }
        "Delete" {
            # kill all objects
            DTMRIDeleteAllStreamlines
            # set mode to Off (will be set to On when add new stream)
            set DTMRI(mode,visualizationType,tractsOn) Off
        }
    }
}



#-------------------------------------------------------------------------------
# .PROC DTMRIAddStreamline
# Add a streamline to the scene. Called by SelectStartHyperStreamline, etc.
# makes all the vtk objects for a new streamline
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddStreamline {} {
    global DTMRI Label

    # Make sure we have streamline actors visible now before 
    # adding a new one
    set DTMRI(mode,visualizationType,tractsOn) On
    DTMRIUpdateStreamlines

    incr DTMRI(vtk,streamline,currentID)
    set id $DTMRI(vtk,streamline,currentID)
    lappend DTMRI(vtk,streamline,idList) $id

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    set streamline streamln,$id
    #DTMRIMakeVTKObject vtkHyperStreamline $streamline
    # use our subclass which makes (i,j,k) points available
    DTMRIMakeVTKObject vtkHyperStreamlinePoints $streamline

    foreach var $DTMRI(stream,variableList) {
    DTMRI(vtk,$streamline) Set$var $DTMRI(stream,$var)
    }

    #DTMRI(vtk,$streamline) DebugOn

    #DTMRI(vtk,$streamline) SetStartMethod      MainStartProgress
    #DTMRI(vtk,$streamline) SetProgressMethod  "MainShowProgress DTMRI(vtk,$streamline)"
    #DTMRI(vtk,$streamline) SetEndMethod        MainEndProgress


    # Display of DTMRI streamline: LUT and Mapper
    #------------------------------------
    set object $streamline,lut
    # Lauren we may want to use this once have no neg eigenvalues
    #DTMRIMakeVTKObject vtkLogLookupTable $object
    DTMRIMakeVTKObject vtkLookupTable $object
    DTMRI(vtk,$object) SetHueRange .6667 0.0
    #    DTMRIAddObjectProperty $object HueRange \
            #        {.6667 0.0} float {Hue Range}

    set object $streamline,mapper
    DTMRIMakeVTKObject vtkPolyDataMapper $object
    DTMRI(vtk,$streamline,mapper) SetInput [DTMRI(vtk,$streamline) GetOutput]
    DTMRI(vtk,$streamline,mapper) SetLookupTable DTMRI(vtk,$streamline,lut)
    DTMRIAddObjectProperty $object ImmediateModeRendering \
            1 bool {Immediate Mode Rendering}    

    # Display of DTMRI streamline: Actor
    #------------------------------------
    set object $streamline,actor
    DTMRIMakeVTKObject vtkActor $object
    DTMRI(vtk,$streamline,actor) SetMapper DTMRI(vtk,$streamline,mapper)
    [DTMRI(vtk,$streamline,actor) GetProperty] SetAmbient 1
    [DTMRI(vtk,$streamline,actor) GetProperty] SetDiffuse 0

    # Update the scalar range of everything
    # this makes the tube's mapper display the streamline in colors
    DTMRIUpdateGlyphScalarRange

    # if we are coloring a solid color do so
    # save the color from when this was created
    set DTMRI(vtk,$streamline,color) $Label(name)
    switch $DTMRI(mode,tractColor) {
        "SolidColor" {
            DTMRIConfigureStreamline $streamline ScalarVisibility 0
            DTMRIConfigureStreamline $streamline Color \
                $DTMRI(vtk,$streamline,color)
        }
        "MultiColor" {
            # do nothing, default
        }
    }    
}

#-------------------------------------------------------------------------------
# .PROC DTMRIConfigureStreamline
# called by DTMRIUpdateTractColor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIConfigureStreamline {streamline operation parameter} {
    global DTMRI Color

    switch $operation {
        "Color" {
            set prop [DTMRI(vtk,$streamline,actor) GetProperty] 
            # find color from the name we have saved; use 1st as default
            set c [lindex $Color(idList) 1]
            foreach id $Color(idList) {
                if {[Color($id,node) GetName] == $parameter} {
                    set c $id
                }
            }
            
            $prop SetAmbient       [Color($c,node) GetAmbient]
            $prop SetDiffuse       [Color($c,node) GetDiffuse]
            $prop SetSpecular      [Color($c,node) GetSpecular]
            $prop SetSpecularPower [Color($c,node) GetPower]
            eval $prop SetColor    [Color($c,node) GetDiffuseColor] 
        }
        "ScalarVisibility" {
            DTMRI(vtk,$streamline,mapper) SetScalarVisibility $parameter  
        }
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTractColor
# configure the coloring to be solid or scalar per triangle 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractColor {} {
    global DTMRI

    set mode $DTMRI(mode,tractColor)
    
    # display new mode while we are working...
    $DTMRI(gui,mbTractColor)    config -text $mode
    
    switch $mode {
        "SolidColor" {
            foreach id $DTMRI(vtk,streamline,idList) {
                set streamline streamln,$id
                DTMRIConfigureStreamline $streamline ScalarVisibility 0
                DTMRIConfigureStreamline $streamline Color \
                    $DTMRI(vtk,$streamline,color)
            }
        }
        "MultiColor" {
            foreach id $DTMRI(vtk,streamline,idList) {
                set streamline streamln,$id
                DTMRIConfigureStreamline $streamline ScalarVisibility 1
            }
        }
    }

    Render3D
}



#-------------------------------------------------------------------------------
# .PROC DTMRIRemoveAllStreamlines
# Remove all streamline actors from scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIRemoveAllStreamlines {} {
    global DTMRI

    # guard against duplicate add/remove actors
    if {$DTMRI(vtk,streamline,actorsAdded) == "1"} {
        #puts "Removing"
        foreach id $DTMRI(vtk,streamline,idList) {
            set streamline streamln,$id
            MainRemoveActor DTMRI(vtk,$streamline,actor) 
        }
        set DTMRI(vtk,streamline,actorsAdded) 0
        Render3D
    }
    
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddAllStreamlines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddAllStreamlines {} {
    global DTMRI

    # guard against duplicate add/remove actors
    if {$DTMRI(vtk,streamline,actorsAdded) == "0"} {
        #puts "Adding"
        foreach id $DTMRI(vtk,streamline,idList) {
            set streamline streamln,$id
            MainAddActor DTMRI(vtk,$streamline,actor) 
        }
        set DTMRI(vtk,streamline,actorsAdded) 1
        Render3D
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIDeleteAllStreamlines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIDeleteAllStreamlines {} {
    global DTMRI

    foreach id $DTMRI(vtk,streamline,idList) {
        set streamline streamln,$id

        # guard against duplicate add/remove actors
    if {$DTMRI(vtk,streamline,actorsAdded) == "1"} {
        #puts "Removing"
        MainRemoveActor DTMRI(vtk,$streamline,actor) 
    }

    # Remove node's ID from idList
    #set i [lsearch $DTMRI(vtk,streamline,idList) $id]
    #set newList [lreplace $newList $i $i]

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    set streamline streamln,$id
    DTMRIDeleteVTKObject $streamline

    # Display of DTMRI streamline: LUT and Mapper
    #------------------------------------
    set object $streamline,lut
    DTMRIDeleteVTKObject $object
    
    set object $streamline,mapper
    DTMRIDeleteVTKObject $object

    # Display of DTMRI streamline: Actor
    #------------------------------------
    set object $streamline,actor
    DTMRIDeleteVTKObject $object
    }
    
    # reset the list with all removed
    set DTMRI(vtk,streamline,idList) ""

    # guard against duplicate add/remove actors    
    set DTMRI(vtk,streamline,actorsAdded) 0

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRISeedStreamlinesFromSegmentation
# In conjunction with DTMRIExecuteForProgrammableFilter, seeds
# streamlines at all points in a segmentation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedStreamlinesFromSegmentation {{verbose 1}} {
    global DTMRI Volume Tensor
    set v $Volume(activeID)

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name [Volume($v,node) GetName]
        set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # filter to grab list of points in the segmentation
    catch "DTMRI(vtk,programmableFilt) Delete"
    vtkProgrammableAttributeDataFilter DTMRI(vtk,programmableFilt)

    set t $Tensor(activeID)
    set v $Volume(activeID)

    #DTMRI(vtk,programmableFilt) SetInput [Tensor($t,data) GetOutput] 
    DTMRI(vtk,programmableFilt) SetInput [Volume($v,vol) GetOutput] 
    DTMRI(vtk,programmableFilt) SetExecuteMethod \
            DTMRIExecuteForProgrammableFilter

    set DTMRI(streamlineList) ""

    DTMRI(vtk,programmableFilt) Update
    DTMRI(vtk,programmableFilt) Delete

    # Now do a streamline at each point
    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform trans
    set v $Volume(activeID)
    set node Volume($v,node)
    trans SetMatrix [$node GetWldToIjk]
    # now it's ijk to world
    trans Inverse
    foreach point $DTMRI(streamlineList) {
        #puts "..$point"

        # find out where the point actually is in world space
        set world [eval {trans TransformPoint} $point]
        puts $world
        flush stdout
        eval {DTMRISelectStartHyperStreamline} $world "false"
        
        # if we are keeping track of all of the points that 
        # were calculated in the streamline
        #set streamln streamln,$DTMRI(vtk,streamline,currentID)
        #puts "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
        #puts [[DTMRI(vtk,$streamln) GetHyperStreamline0] Print]
        #puts "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"    
    }

    trans Delete
}



#-------------------------------------------------------------------------------
# .PROC DTMRIExecuteForProgrammableFilter
# Just makes a tcl list of nonzero points in the input volume.
# Used as execute proc of a programmable attribute data filter.
# We call this to figure out where to seed the streamlines.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIExecuteForProgrammableFilter {} {
    global Volume DTMRI Label

    # proc for ProgrammableAttributeDataFilter.  Note the use of "double()"
    # in the calculations.  This protects us from Tcl using ints and 
    # overflowing.

    puts "Programmable filter executing"
    set input [DTMRI(vtk,programmableFilt) GetInput]
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
        puts "Slice $z"
        for {set y $ymin} {$y < $ymax} {incr y} {
            
            for {set x $xmin} {$x < $xmax} {incr x} {
                
                set s [$scalars GetTuple1 $i]
                
                # if this matches the selected label
                if {$s == $Label(label)} {
                    
                    # for each labelled point in the segmentation
                    # we want to start a streamline
                    #puts $s

                    # save on the list of points for later use
                    lappend DTMRI(streamlineList) "$x $y $z"
                }
                
                # point index
                incr i
                
            } 
        } 
    } 
    
    # should we pass the input through (points)?
    #[$output GetPointData] PassData [$input GetPointData]

    puts "done $x $y $z $i"
    # ignore output for now, we don't care
    #[[DTMRI(vtk,programmableFilt) GetOutput] GetPointData] SetScalars scalars
}



################################################################
#  MAIN visualization procedure: pipeline control is here
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdate
# The whole enchilada (if this were a vtk filter, this would be
# the Execute function...)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdate {} {
    global DTMRI Slice Volume Label Gui Tensor

    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
        puts "DTMRIUpdate: Can't visualize Nothing"
        return
    }

    # reset progress text for any filter that uses the blue bar
    set Gui(progressText) "Working..."

    #------------------------------------
    # preprocessing pipeline
    #------------------------------------


    # threshold DTMRIs if required
    #------------------------------------
    set mode $DTMRI(mode,threshold)
    if {$mode != "None"} {
        
        puts "thresholding by $DTMRI(mode,threshold)"
        set math DTMRI(vtk,thresh,math)

        # calculate trace or whatever we are thresholding by
        $math SetInput 0 [Tensor($t,data) GetOutput]
        $math SetOperationTo$DTMRI(mode,threshold)

        # threshold to make a mask of the area of interest
        set thresh1 DTMRI(vtk,thresh,threshold)
        $thresh1 ThresholdBetween $DTMRI(thresh,threshold,lower) \
                $DTMRI(thresh,threshold,upper)

        # this line seems to be needed
        $thresh1 Update
        
        #DTMRI(vtk,glyphs) SetScalarMask [$thresh GetOutput]
        # tell our filter to use this information
        #DTMRI(vtk,glyphs) MaskGlyphsWithScalarsOn

        #  set DTMRIs to 0 outside of mask generated above
        set mask1 DTMRI(vtk,thresh,mask)
        $mask1 SetImageInput [Tensor($t,data) GetOutput]

        set dataSource [$mask1 GetOutput]
    } else {
        set dataSource [Tensor($t,data) GetOutput]
        #DTMRI(vtk,glyphs) MaskGlyphsWithScalarsOff
    }

    # mask DTMRIs if required
    #------------------------------------
    set mode $DTMRI(mode,mask)
    if {$mode != "None"} {
        
        puts "masking by $DTMRI(mode,mask)"
        # note it would be more efficient to 
        # combine the two masks (from thresh and here) 
        # but probably not both used at once 

        set thresh DTMRI(vtk,mask,threshold)    
        $thresh ThresholdBetween $Label(label) $Label(label)
        set v $Volume(activeID)
        $thresh SetInput [Volume($v,vol) GetOutput]

        # this line seems to be needed
        $thresh Update
        
        set mask DTMRI(vtk,mask,mask)
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
    #set mode $DTMRI(mode,visualizationType)
    set mode $DTMRI(mode,visualizationType,glyphsOn)
    puts "Setting glyph mode $mode for DTMRI $t"
    
    switch $mode {
        "On" {
            puts "glyphs! $DTMRI(mode,glyphType)"
            
            # Find input to pipeline
            #------------------------------------
            set slice $DTMRI(mode,reformatType)

        # find ijk->ras rotation to apply to each DTMRI
        vtkTransform t2 
        DTMRICalculateIJKtoRASRotationMatrix t2 $t
        #puts "Lauren testing rm -y"
        #t2 Scale 1 -1 1
        puts [[t2 GetMatrix] Print]
        DTMRI(vtk,glyphs) SetTensorRotationMatrix [t2 GetMatrix]
        t2 Delete

            if {$slice != "None"} {

                # We are reformatting a slice of glyphs
                DTMRI(vtk,reformat) SetInput $preprocessedSource

                # set fov same as volume we are overlaying
                DTMRI(vtk,reformat) SetFieldOfView [Slicer GetFieldOfView]

                # tell reformatter to obey the node
                set node Tensor($Tensor(activeID),node)
                DTMRI(vtk,reformat) SetInterpolate [$node GetInterpolate]
                DTMRI(vtk,reformat) SetWldToIjkMatrix [$node GetWldToIjk]
                
                #  reformat resolution should match the DTMRI resolution.
                # Use the extents to figure this out.
                set ext [[Tensor($Tensor(activeID),data) GetOutput] GetExtent]
                set resx [expr [lindex $ext 1] - [lindex $ext 0] + 1]
                set resy [expr [lindex $ext 3] - [lindex $ext 2] + 1]
                if {$resx > $resy} {
                    set res $resx
                } else {
                    set res $resy
                }

                DTMRI(vtk,reformat) SetResolution $res

                set m [Slicer GetReformatMatrix $slice]
                DTMRI(vtk,reformat) SetReformatMatrix $m
                set visSource [DTMRI(vtk,reformat) GetOutput]
                
                # Position glyphs with the slice.
                # The glyph filter will transform output points by this 
                # matrix.  We can't just move the actor in space
                # since this will rotate the DTMRIs, so this is wrong:
                # DTMRI(vtk,glyphs,actor) SetUserMatrix $m
                DTMRI(vtk,glyphs) SetVolumePositionMatrix $m

            } else {
                # We are displaying the whole volume of glyphs!
                set visSource $preprocessedSource
                
                # Want actor to be positioned in center with slices
                vtkTransform t1
                DTMRICalculateActorMatrix t1 $Tensor(activeID)
                
                # Position glyphs in the volume.
                # The glyph filter will transform output points by this 
                # matrix.  We can't just move the actor in space
                # since this will rotate the DTMRIs, so this is wrong:
                #DTMRI(vtk,glyphs,actor) SetUserMatrix [t1 GetMatrix]
                DTMRI(vtk,glyphs) SetVolumePositionMatrix [t1 GetMatrix]
                t1 Delete
            }


            # start pipeline
            #------------------------------------
            DTMRI(vtk,glyphs) SetInput $visSource

            # for lines don't use normals filter before mapper
        DTMRI(vtk,glyphs,mapper) SetInput \
            [DTMRI(vtk,glyphs) GetOutput]

            # Use axes or ellipsoids
            #------------------------------------
            switch $DTMRI(mode,glyphType) {
                "Axes" {
                    DTMRI(vtk,glyphs) SetSource \
                            [DTMRI(vtk,glyphs,axes) GetOutput]
                    # this is too slow, but might make nice pictures
                    #[DTMRI(vtk,glyphs,tubeAxes) GetOutput]

                }
                "Lines" {
                    DTMRI(vtk,glyphs) SetSource \
                            [DTMRI(vtk,glyphs,line) GetOutput]
                }
                "Ellipsoids" {
                    DTMRI(vtk,glyphs) SetSource \
                            [DTMRI(vtk,glyphs,sphere) GetOutput]
                    # this normal filter improves display but is slow.
                    DTMRI(vtk,glyphs,mapper) SetInput \
                            [DTMRI(vtk,glyphs,normals) GetOutput]
                }
                "Boxes" {
                    DTMRI(vtk,glyphs) SetSource \
                            [DTMRI(vtk,glyphs,box) GetOutput]
                    # this normal filter improves display but is slow.
                    DTMRI(vtk,glyphs,mapper) SetInput \
                            [DTMRI(vtk,glyphs,normals) GetOutput]
                }
            }

            # Make actor visible
            #------------------------------------
            DTMRI(vtk,glyphs,actor) VisibilityOn

        }
        "Off" {
            puts "Turning off DTMRI visualization"

            # make invisible so output
            # not requested from pipeline anymore
            #------------------------------------
            DTMRI(vtk,glyphs,actor) VisibilityOff
        }
    }

    # make sure the scalars are updated (if we have anything displayed)
    if {$mode != "None"} {
        DTMRIUpdateGlyphScalarRange
    }
    # update 3D window (causes pipeline update)
    Render3D
}

################################################################
#  Procedures used to derive scalar volumes from DTMRI data
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRISetOperation
# Set the mathematical operation we should do to produce
# a scalar volume from the DTMRIs
# .ARGS
# str math the name of the operation from list $DTMRI(scalars,operationList)
# .END
#-------------------------------------------------------------------------------
proc DTMRISetOperation {math} {
    global DTMRI

    set DTMRI(scalars,operation) $math
    
    # config menubutton
    $DTMRI(gui,mbMath) config -text $math
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMathParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMathParams {} {
    global DTMRI


    # Just check that if they requested a 
    # preprocessing step, that we are already
    # doing that step
    
    set mode $DTMRI(scalars,ROI)

    set err "The $mode ROI is not currently being computed.  Please turn this feature on in the ROI tab before creating the volume."

    switch $mode {
        "None" {
        }
        "Threshold" {
            if {$DTMRI(mode,threshold)    == "None"} {
                set DTMRI(scalars,ROI) None
                tk_messageBox -message $err
            }
        }
        "Mask" {
            if {$DTMRI(mode,mask)    == "None"} {
                set DTMRI(scalars,ROI) None
                tk_messageBox -message $err
            }
        }
    }    

}

#-------------------------------------------------------------------------------
# .PROC DTMRICreateEmptyVolume
# Just like DevCreateNewCopiedVolume, but uses a Tensor node
# to copy parameters from instead of a volume node
# Used for scalar output from DTMRI math calculations.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICreateEmptyVolume {OrigId {Description ""} { VolName ""}} {
    global Volume Tensor

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
# .PROC DTMRIDoMath
# Called to compute a scalar vol from DTMRIs
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIDoMath {{operation ""}} {
    global DTMRI Gui Tensor


    # if this was called from user input GUI menu
    if {$operation == ""} {
        set operation $DTMRI(scalars,operation) 
    }

    # validate user input
    if {[ValidateFloat $DTMRI(scalars,scaleFactor)] != "1"} {
    tk_messageBox -message \
        "Please enter a number for the scale factor."
    # reset default
    set DTMRI(scalars,scaleFactor) 1000
    return
    }

    # should use DevCreateNewCopiedVolume if have a vol node
    # to copy...
    set t $Tensor(activeID) 
    if {$t == "" || $t == $Tensor(idNone)} {
    tk_messageBox -message \
        "Please select an input DTMRI volume (Active DTMRI)"
        return
    }
    set name [Tensor($t,node) GetName]
    set name ${operation}_$name
    set description "$operation volume derived from DTMRI volume $name"
    set v [DTMRICreateEmptyVolume $t $description $name]

    # find input
    set mode $DTMRI(scalars,ROI)
    
    switch $mode {
        "None" {
            set input [Tensor($t,data) GetOutput]
        }
        "Threshold" {
            set input [DTMRI(vtk,thresh,mask) GetOutput]
        }
        "Mask" {
            set input [DTMRI(vtk,mask,mask) GetOutput]
        }
    }

    # create vtk object to do the operation
    vtkTensorMathematics math
    math SetScaleFactor $DTMRI(scalars,scaleFactor)
    math SetInput 0 $input
    math SetOperationTo$operation
    math SetStartMethod      MainStartProgress
    math SetProgressMethod  "MainShowProgress math"
    math SetEndMethod        MainEndProgress
    set Gui(progressText) "Creating Volume $operation"

    # put the filter output into a slicer volume
    math Update
    #puts [[math GetOutput] Print]
    Volume($v,vol) SetImageData [math GetOutput]
    MainVolumesUpdate $v
    # tell the node what type of data so MRML file will be okay
    Volume($v,node) SetScalarType [[math GetOutput] GetScalarType]


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


################################################################
#  Procedures to set up pipelines and create/modify vtk objects.
#  TODO: try to create objects only if needed!
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRIApplyVisualizationParameters
#  For interaction with pipeline under advanced tab.
#  Apply all GUI parameters into our vtk objects.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIApplyVisualizationParameters {} {
    global DTMRI

    # this code actually makes a bunch of calls like the following:
    # DTMRI(vtk,axes) SetScaleFactor $DTMRI(vtk,axes,scaleFactor)
    # DTMRI(vtk,tubeAxes) SetRadius $DTMRI(vtk,tubeAxes,radius)
    # DTMRI(vtk,glyphs) SetClampScaling 1
    # we can't do calls like MyObject MyVariableOn now

    # our naming convention is:
    # DTMRI(vtk,object)  is the name of the object
    # paramName is the name of the parameter
    # $DTMRI(vtk,object,paramName) is the value 

    # Lauren we need to validate too!!!!!!!!
    #  too bad vtk can't return a string type desrciptor...

    # loop through all vtk objects
    #------------------------------------
    foreach o $DTMRI(vtkObjectList) {

        # loop through all parameters of the object
        #------------------------------------
        foreach p $DTMRI(vtkPropertyList,$o) {

            # value of the parameter is $DTMRI(vtk,$o,$p)
            #------------------------------------
            set value $DTMRI(vtk,$o,$p)
            
            # Actually set the value appropriately in the vtk object
            #------------------------------------    

            # first capitalize the parameter name
            set param [Cap $p]
            
            # MyObject SetMyParameter $value    
            # handle the case in which value is a list with an eval 
            # this puts it into the correct format for feeding to vtk
            eval {DTMRI(vtk,$o) Set$param} $value
        }
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIDeleteVTKObject
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIDeleteVTKObject {object} {
    global DTMRI

    # delete the object
    #------------------------------------
    DTMRI(vtk,$object) Delete

    # rm from list for updating of its variables by user
    #------------------------------------
    set i [lsearch $DTMRI(vtkObjectList) $object]
    set DTMRI(vtkObjectList) [lreplace $DTMRI(vtkObjectList) $i $i]

    # kill list of its variables
    #------------------------------------
    unset DTMRI(vtkPropertyList,$object) 
}

#-------------------------------------------------------------------------------
# .PROC DTMRIMakeVTKObject
#  Wrapper for vtk object creation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIMakeVTKObject {class object} {
    global DTMRI

    # make the object
    #------------------------------------
    $class DTMRI(vtk,$object)

    # save on list for updating of its variables by user
    #------------------------------------
    lappend DTMRI(vtkObjectList) $object

    # initialize list of its variables
    #------------------------------------
    set DTMRI(vtkPropertyList,$object) ""
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddObjectProperty
#  Initialize vtk object access: saves object's property on list
#  for automatic GUI creation so user can change the property.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddObjectProperty {object parameter value type desc} {
    global DTMRI

    # create tcl variables of the form:
    #set DTMRI(vtk,tubeAxes,numberOfSides) 6
    #set DTMRI(vtk,tubeAxes,numberOfSides,type) int
    #set DTMRI(vtk,tubeAxes,numberOfSides,description) 
    #lappend DTMRI(vtkPropertyList,tubeAxes) numberOfSides 


    # make tcl variable and save its attributes (type, desc)
    #------------------------------------
    set param [Uncap $parameter]
    set DTMRI(vtk,$object,$param) $value
    set DTMRI(vtk,$object,$param,type) $type
    set DTMRI(vtk,$object,$param,description) $desc

    # save on list for updating variable by user
    #------------------------------------
    lappend DTMRI(vtkPropertyList,$object) $param
    
}

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildVTK
# Called automatically by the slicer program.
# builds pipelines.
# See also DTMRIUpdate for pipeline use.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildVTK {} {
    global DTMRI

    # Lauren: maybe this module should kill its objects on exit.
    # We must be hogging a ton of memory all over the slicer.
    # So possibly build (some) VTK upon entering module.

    #---------------------------------------------------------------
    # coordinate system actor (also called "Csys" and "gyro" 
    # in other modules).  Used for user interaction with hyperstreamlines
    #---------------------------------------------------------------
    #CsysCreate DTMRI coordinateAxesTool -1 -1 -1

    #---------------------------------------------------------------
    # Pipeline for preprocessing of glyphs
    #---------------------------------------------------------------

    # objects for thresholding before glyph display
    #------------------------------------

    # compute scalar data for thresholding
    set object thresh,math
    DTMRIMakeVTKObject vtkTensorMathematics $object
    DTMRIAddObjectProperty $object ExtractEigenvalues 1 bool {Extract Eigenvalues}

    # threshold the scalar data to produce binary mask 
    set object thresh,threshold
    #DTMRIMakeVTKObject vtkImageThresholdBeyond $object
    DTMRIMakeVTKObject vtkImageThreshold $object
    DTMRI(vtk,$object) SetInValue       1
    DTMRI(vtk,$object) SetOutValue      0
    DTMRI(vtk,$object) SetReplaceIn     1
    DTMRI(vtk,$object) SetReplaceOut    1
    DTMRI(vtk,$object) SetInput \
            [DTMRI(vtk,thresh,math) GetOutput]

    # convert the mask to unsigned char
    # Lauren it would be better to have the threshold filter do this
    set object thresh,cast
    DTMRIMakeVTKObject vtkImageCast $object
    DTMRI(vtk,$object) SetOutputScalarTypeToUnsignedChar    
    DTMRI(vtk,$object) SetInput \
            [DTMRI(vtk,thresh,threshold) GetOutput]

    # mask the DTMRIs 
    set object thresh,mask
    DTMRIMakeVTKObject vtkTensorMask $object
    #DTMRI(vtk,$object) SetMaskInput \
            #    [DTMRI(vtk,thresh,threshold) GetOutput]
    DTMRI(vtk,$object) SetMaskInput \
            [DTMRI(vtk,thresh,cast) GetOutput]

    # objects for masking before glyph display
    #------------------------------------

    # produce binary mask from the input mask labelmap
    set object mask,threshold
    #DTMRIMakeVTKObject vtkImageThresholdBeyond $object
    DTMRIMakeVTKObject vtkImageThreshold $object
    DTMRI(vtk,$object) SetInValue       1
    DTMRI(vtk,$object) SetOutValue      0
    DTMRI(vtk,$object) SetReplaceIn     1
    DTMRI(vtk,$object) SetReplaceOut    1

    # convert the mask to short
    # (use this most probable input type to try to avoid data copy)
    set object mask,cast
    DTMRIMakeVTKObject vtkImageCast $object
    DTMRI(vtk,$object) SetOutputScalarTypeToShort    
    DTMRI(vtk,$object) SetInput \
            [DTMRI(vtk,mask,threshold) GetOutput]

    # mask the DTMRIs 
    set object mask,mask
    DTMRIMakeVTKObject vtkTensorMask $object
    DTMRI(vtk,$object) SetMaskInput \
            [DTMRI(vtk,mask,cast) GetOutput]

    #---------------------------------------------------------------
    # Pipeline for display of glyphs
    #---------------------------------------------------------------

    # User interaction objects
    #------------------------------------
    # Lauren: doing this like Endoscopic (this vs PointPicker?)
    set object picker
    DTMRIMakeVTKObject vtkCellPicker $object
    DTMRIAddObjectProperty $object Tolerance 0.001 float {Pick Tolerance}

    # Lauren test by displaying picked point
    #DTMRI(vtk,picker) SetEndPickMethod "annotatePick DTMRI(vtk,picker)"
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
    DTMRIMakeVTKObject vtkAxes $object
    DTMRIAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeAxes
    #DTMRIMakeVTKObject vtkTubeFilter $object
    #DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs,axes) GetOutput]
    #DTMRIAddObjectProperty $object Radius 0.1 float {Radius}
    #DTMRIAddObjectProperty $object NumberOfSides 6 int \
            #    {Number Of Sides}

    # One line
    set object glyphs,line
    DTMRIMakeVTKObject vtkLineSource $object
    DTMRIAddObjectProperty $object Resolution 10 int {Resolution}
    #DTMRI(vtk,$object) SetPoint1 0 0 0
    # use a stick that points both ways, not a vector from the origin!
    DTMRI(vtk,$object) SetPoint1 -1 0 0
    DTMRI(vtk,$object) SetPoint2 1 0 0
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeLine
    #DTMRIMakeVTKObject vtkTubeFilter $object
    #DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs,line) GetOutput]
    #DTMRIAddObjectProperty $object Radius 0.1 float {Radius}
    #DTMRIAddObjectProperty $object NumberOfSides 6 int \
            #    {Number Of Sides}

    # Ellipsoids
    set object glyphs,sphere
    DTMRIMakeVTKObject vtkSphereSource  $object
    #DTMRIAddObjectProperty $object ThetaResolution 1 int ThetaResolution
    #DTMRIAddObjectProperty $object PhiResolution 1 int PhiResolution
    DTMRIAddObjectProperty $object ThetaResolution 12 int ThetaResolution
    DTMRIAddObjectProperty $object PhiResolution 12 int PhiResolution

    # Boxes
    set object glyphs,box
    DTMRIMakeVTKObject vtkCubeSource  $object

    # objects for placement of glyphs in dataset
    #------------------------------------
    set object glyphs
    #DTMRIMakeVTKObject vtkDTMRIGlyph $object
    DTMRIMakeVTKObject vtkInteractiveTensorGlyph $object
    #DTMRI(vtk,glyphs) SetSource [DTMRI(vtk,glyphs,axes) GetOutput]
    #DTMRI(vtk,glyphs) SetSource [DTMRI(vtk,glyphs,sphere) GetOutput]
    #DTMRIAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    DTMRIAddObjectProperty $object ScaleFactor 5000 float {Scale Factor}
    DTMRIAddObjectProperty $object ClampScaling 0 bool {Clamp Scaling}
    DTMRIAddObjectProperty $object ExtractEigenvalues 1 bool {Extract Eigenvalues}
    DTMRI(vtk,$object) SetStartMethod      MainStartProgress

    DTMRI(vtk,$object) SetProgressMethod  "MainShowProgress DTMRI(vtk,$object)"
    DTMRI(vtk,$object) SetEndMethod        MainEndProgress

    # poly data normals filter cleans up polydata for nice display
    # use this for ellipses/boxes only
    #------------------------------------
    # very slow
    set object glyphs,normals
    DTMRIMakeVTKObject vtkPolyDataNormals $object
    DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs) GetOutput]

    # Display of DTMRI glyphs: LUT and Mapper
    #------------------------------------
    set object glyphs,lut
    #DTMRIMakeVTKObject vtkLogLookupTable $object
    DTMRIMakeVTKObject vtkLookupTable $object
    DTMRIAddObjectProperty $object HueRange \
            {.6667 0.0} float {Hue Range}

    # mapper
    set object glyphs,mapper
    DTMRIMakeVTKObject vtkPolyDataMapper $object
    #Raul
    DTMRI(vtk,glyphs,mapper) SetInput [DTMRI(vtk,glyphs) GetOutput]
    #DTMRI(vtk,glyphs,mapper) SetInput [DTMRI(vtk,glyphs,normals) GetOutput]
    DTMRI(vtk,glyphs,mapper) SetLookupTable DTMRI(vtk,glyphs,lut)
    DTMRIAddObjectProperty $object ImmediateModeRendering \
            1 bool {Immediate Mode Rendering}    

    # Display of DTMRI glyphs: Actor
    #------------------------------------
    set object glyphs,actor
    #DTMRIMakeVTKObject vtkActor $object
    DTMRIMakeVTKObject vtkLODActor $object
    DTMRI(vtk,glyphs,actor) SetMapper DTMRI(vtk,glyphs,mapper)
    # intermediate level of detail produces visible points with 10
    [DTMRI(vtk,glyphs,actor) GetProperty] SetPointSize 10
    
    [DTMRI(vtk,glyphs,actor) GetProperty] SetAmbient 1
    [DTMRI(vtk,glyphs,actor) GetProperty] SetDiffuse .2
    [DTMRI(vtk,glyphs,actor) GetProperty] SetSpecular .4

    # Scalar bar actor
    #------------------------------------
    set object scalarBar,actor
    DTMRIMakeVTKObject vtkScalarBarActor $object
    DTMRI(vtk,scalarBar,actor) SetLookupTable DTMRI(vtk,glyphs,lut)
    viewRen AddProp DTMRI(vtk,scalarBar,actor)
    DTMRI(vtk,scalarBar,actor) VisibilityOff

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    # we don't really use this one, should rm this code
    set object streamln
    #DTMRIMakeVTKObject vtkHyperStreamline $object
    DTMRIMakeVTKObject vtkHyperStreamlinePoints $object
    
    DTMRI(vtk,$object) SetStartMethod      MainStartProgress
    DTMRI(vtk,$object) SetProgressMethod  "MainShowProgress DTMRI(vtk,$object)"
    DTMRI(vtk,$object) SetEndMethod        MainEndProgress

    #DTMRIAddObjectProperty $object StartPosition {9 9 -9} float {Start Pos}
    #DTMRIAddObjectProperty $object IntegrateMinorEigenvector \
            #1 bool IntegrateMinorEv
    DTMRIAddObjectProperty $object MaximumPropagationDistance 60.0 \
            float {Max Propagation Distance}
    DTMRIAddObjectProperty $object IntegrationStepLength 0.01 \
            float {Integration Step Length}
    DTMRIAddObjectProperty $object StepLength 0.0001 \
            float {Step Length}        
    DTMRIAddObjectProperty $object Radius 0.25 \
            float {Radius}
    DTMRIAddObjectProperty $object  NumberOfSides 30 \
            int {Number of Sides}
    DTMRIAddObjectProperty $object  IntegrationDirection 2 \
            int {Integration Direction}
    
    # Display of DTMRI streamline: LUT and Mapper
    #------------------------------------
    set object streamln,lut

    DTMRIMakeVTKObject vtkLookupTable $object
    DTMRI(vtk,$object) SetHueRange .6667 0.0

    set object streamln,mapper
    DTMRIMakeVTKObject vtkPolyDataMapper $object
    DTMRI(vtk,streamln,mapper) SetInput [DTMRI(vtk,streamln) GetOutput]
    DTMRI(vtk,streamln,mapper) SetLookupTable DTMRI(vtk,streamln,lut)
    DTMRIAddObjectProperty $object ImmediateModeRendering \
            1 bool {Immediate Mode Rendering}    

    #---------------------------------------------------------------
    # Pipeline for display of DTMRIs over 2D slice
    #---------------------------------------------------------------
    
    # Lauren how should reformatting be hooked into regular
    # slicer slice reformatting?  Ideally want to follow
    # the 3 slices.
    DTMRIMakeVTKObject vtkImageReformat reformat


    #---------------------------------------------------------------
    # Pipeline for diffusion simulations
    #---------------------------------------------------------------
    set object diffusion    
    DTMRIMakeVTKObject vtkImageTensorDiffusion $object
    DTMRIAddObjectProperty $object NumberOfIterations 1 \
            int {Number of Iterations}
    DTMRIAddObjectProperty $object S 1 \
            float {Diffusion Speed}


    # Apply all settings from tcl variables that were
    # created above using calls to DTMRIAddObjectProperty
    #------------------------------------
    DTMRIApplyVisualizationParameters

}

################################################################
# procedures for converting volumes into DTMRIs.
# TODO: this should happen automatically and be in MRML
################################################################

#-------------------------------------------------------------------------------
# .PROC ConvertVolumeToTensors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ConvertVolumeToTensors {} {
    global DTMRI Volume Tensor

    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create DTMRIs from None volume"
        return
    }

    # DTMRI creation filter
    vtkImageDiffusionTensor DTMRI

    # setup - these are now globals linked with GUI
    #set slicePeriod 8
    #set offsetsGradient "0 1 2 3 4 5"
    #set offsetsNoGradient "6 7"
    #set numberOfGradientImages 6
    #set numberOfNoGradientImages 2
    set count 0
    for {set i $DTMRI(convert,firstGradientImage)} \
    {$i  <= $DTMRI(convert,lastGradientImage) } \
    {incr i} {
        # 0-based offsets, so subtract 1
        lappend offsetsGradient [expr $i -1]
        incr count
    } 
    set numberOfGradientImages $count
    set count 0
    for {set i $DTMRI(convert,firstNoGradientImage)} \
    {$i  <= $DTMRI(convert,lastNoGradientImage) } \
    {incr i} {
        # 0-based offsets, so subtract 1
        lappend offsetsNoGradient [expr $i -1]
        incr count
    }
    set numberOfNoGradientImages $count

    set slicePeriod \
    [expr $numberOfGradientImages+$numberOfNoGradientImages]
    
    set numberOfGradientImages $DTMRI(convert,numberOfGradients) 
    DTMRI SetNumberOfGradients $numberOfGradientImages

    #puts $offsetsGradient 
    #puts $offsetsNoGradient

    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
#    eval {DTMRI SetDiffusionGradient $i} [lindex DTMRI(convert,gradients) $i]
    eval {DTMRI SetDiffusionGradient $i} [lindex $DTMRI(convert,gradients) $i]
    }

    # volume we use for input
    set input [Volume($v,vol) GetOutput]

    # transform gradient directions to make DTMRIs in ijk
    vtkTransform trans    
    puts "If not phase-freq flipped, swapping x and y in gradient directions"
    set swap [Volume($v,node) GetFrequencyPhaseSwap]
    if {$swap == 0} {
    set elements "\
                    {0 1 0 0}  \
                    {1 0 0 0}  \
                    {0 0 1 0}  \
                    {0 0 0 1}  "

    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
        foreach col $cols {
        [trans GetMatrix] SetElement $row $col \
            [lindex [lindex $elements $row] $col]
        }
    }
    }   else { 
    puts "Creating DTMRIs with -y for vtk compliance"
    trans Scale 1 -1 1
    }
    DTMRI SetTransform trans
    trans Delete

    # produce input vols for DTMRI creation
    set inputNum 0
    set DTMRI(recalculate,gradientVolumes) ""
    foreach slice $offsetsGradient {
        vtkImageExtractSlices extract$slice
        extract$slice SetInput $input
        extract$slice SetSliceOffset $slice
        extract$slice SetSlicePeriod $slicePeriod

        #puts "----------- slice $slice update --------"    
        extract$slice Update
        #puts [[extract$slice GetOutput] Print]

        # pass along in pipeline
        DTMRI SetDiffusionImage \
                $inputNum [extract$slice GetOutput]
        incr inputNum
        
        # put the filter output into a slicer volume
        # Lauren this should be optional
        # make a MRMLVolume for this output
        set name [Volume($v,node) GetName]
        set description "$slice gradient volume derived from volume $name"
        set name gradient${slice}_$name
        set id [DevCreateNewCopiedVolume $v $description $name]
        # save id in case we recalculate the DTMRIs
        lappend DTMRI(recalculate,gradientVolumes) $id
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
        puts "-------computing ras to ijk from scan order----"
        Volume($id,node) ComputeRasToIjkFromScanOrder $order


        # update slicer internals
        MainVolumesUpdate $id
        
    }
    # save ids in case we recalculate the DTMRIs
    set DTMRI(recalculate,noGradientVolumes) ""
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
        # save id in case we recalculate the DTMRIs
        lappend DTMRI(recalculate,noGradientVolumes) $id
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

    # average the two slices of no gradient 
    # NOTE THIS WILL NOT WORK WITH MORE THAN 2
     vtkImageMathematics math
     math SetOperationToAdd
     math SetInput 0 [extract6 GetOutput]
     math SetInput 1 [extract7 GetOutput]

     vtkImageMathematics math2
     math2 SetOperationToMultiplyByK
     math2 SetConstantK 0.5
     math2 SetInput 0 [math GetOutput]
    
    # set the no diffusion input
    DTMRI SetNoDiffusionImage [extract6 GetOutput]
    DTMRI SetNoDiffusionImage [math2 GetOutput]
    
    
    puts "----------- DTMRI update --------"
    DTMRI Update
    puts "----------- after DTMRI update --------"


    # put output into a Tensor volume
    # Lauren if volumes and tensos are the same
    # this should be done like the above
    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume Tensor]
    $newvol Copy Volume($v,node)
    $newvol SetDescription "DTMRI volume"
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
    MainDataCreate Tensor $n Volume
    
    # Set the slicer object's image data to what we created
    DTMRI Update
    #Tensor($n,data) SetData [DTMRI GetOutput]
    Tensor($n,data) SetImageData [DTMRI GetOutput]

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

    math SetOutput ""
    math2 SetOutput ""
    DTMRI SetOutput ""

    DTMRI Delete
    math Delete
    math2 Delete
}


# This procedure will load ADC-images from LSDIrecon (by Stephan Maier)
#
# The main reason for not doing the reconstruction in the slicer, except
# for the dual basis stuff to transform ADC:s into DTMRIs, is that 
# a correct reconstruction from the raw DTI images is quite complex and 
# involves a lot of MRI knowledge. It is therefore logical to put all the
# intelligence for reconstruction in one program and not have a quasi-optimal
# reconstruction in the slicer. 
#
# Author: Anders Brun

proc ConvertLSDIrecon {} {
    global DTMRI Volume Tensor

    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create DTMRIs from None volume"
        return
    }

    # DTMRI creation filter
    vtkImageDiffusionTensor DTMRI

    # setup - these are now globals linked with GUI (Nope, not anymore... AB)
    set slicePeriod 7
    set offsetsGradient "0 1 2 3 4 5"
    set offsetsNoGradient "6"
    set numberOfGradientImages 6
    set numberOfNoGradientImages 1

    
    set numberOfGradientImages $DTMRI(convert,numberOfGradients) 
    DTMRI SetNumberOfGradients $numberOfGradientImages


    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
    eval {DTMRI SetDiffusionGradient $i} [lindex $DTMRI(convert,gradients) $i]
    }

    # volume we use for input
    set input [Volume($v,vol) GetOutput]

    # transform gradient directions to make DTMRIs in ijk
    # One axis should have a minus, due to the fact that the gradients are oriented relative
    # to the read direction. Or something...
    
    vtkTransform trans    
    set elements "\
                  {0 1 0 0}  \
                  {-1 0 0 0}  \
                  {0 0 1 0}  \
                  {0 0 0 1}  "

    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
    foreach col $cols {
        [trans GetMatrix] SetElement $row $col \
        [lindex [lindex $elements $row] $col]
    }
    }

    DTMRI SetTransform trans
    trans Delete

    # produce input vols for DTMRI creation
    set inputNum 0
    set DTMRI(recalculate,gradientVolumes) ""
    foreach slice $offsetsGradient {
        vtkImageExtractSlices extract$slice
        extract$slice SetInput $input
        extract$slice SetSliceOffset $slice
        extract$slice SetSlicePeriod $slicePeriod

        #puts "----------- slice $slice update --------"    
        extract$slice Update
        #puts [[extract$slice GetOutput] Print]

        # pass along in pipeline
        DTMRI SetDiffusionImage \
                $inputNum [extract$slice GetOutput]
        incr inputNum
        
        # put the filter output into a slicer volume
        # Lauren this should be optional
        # make a MRMLVolume for this output
        set name [Volume($v,node) GetName]
        set description "$slice gradient volume derived from volume $name"
        set name gradient${slice}_$name
        set id [DevCreateNewCopiedVolume $v $description $name]
        # save id in case we recalculate the DTMRIs
        lappend DTMRI(recalculate,gradientVolumes) $id
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
        puts "-------computing ras to ijk from scan order----"
        Volume($id,node) ComputeRasToIjkFromScanOrder $order


        # update slicer internals
        MainVolumesUpdate $id
        
    }
    # save ids in case we recalculate the DTMRIs
    set DTMRI(recalculate,noGradientVolumes) ""
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
        # save id in case we recalculate the DTMRIs
        lappend DTMRI(recalculate,noGradientVolumes) $id
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

    # set the no diffusion input
    DTMRI SetNoDiffusionImage [extract6 GetOutput]
    
    
    puts "----------- DTMRI update --------"
    DTMRI Update
    puts "----------- after DTMRI update --------"


    # put output into a DTMRI volume
    # Lauren if volumes and DTMRIs are the same
    # this should be done like the above
    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume Tensor]
    $newvol Copy Volume($v,node)
    $newvol SetDescription "DTMRI volume"
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
    MainDataCreate DTMRI $n Volume
    
    # Set the slicer object's image data to what we created
    DTMRI Update
    #Tensor($n,data) SetData [DTMRI GetOutput]
    Tensor($n,data) SetImageData [DTMRI GetOutput]

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

    DTMRI SetOutput ""
    DTMRI Delete

}



#-------------------------------------------------------------------------------
# .PROC MyConvertVolumeToTensors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MyConvertVolumeToTensors {} {
    global DTMRI Volume Tensor

    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create DTMRIs from None volume"
        return
    }

    # DTMRI creation filter
    vtkLSDIreconDTMRI DTMRI
    DTMRI SetInputScaleFactor 10000000

    set count 0
    for {set i $DTMRI(convert,firstGradientImage)} \
    {$i  <= $DTMRI(convert,lastGradientImage) } \
    {incr i} {
        # 0-based offsets, so subtract 1
        lappend offsetsGradient [expr $i -1]
        incr count
    } 
    set numberOfGradientImages $count

    set numberOfNoGradientImages 0

    set slicePeriod [expr $numberOfGradientImages]
    
    set numberOfGradientImages $DTMRI(convert,numberOfGradients) 
    DTMRI SetNumberOfGradients $numberOfGradientImages

    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
    eval {DTMRI SetDiffusionGradient $i} [lindex $DTMRI(convert,gradients) $i]
    }

    # volume we use for input
    set input [Volume($v,vol) GetOutput]

    # transform gradient directions to make DTMRIs in ijk
    vtkTransform trans    
    puts "bla bla"
    set swap [Volume($v,node) GetFrequencyPhaseSwap]
    if {$swap == 0} {
    set elements "\
                   {0 1 0 0}  \
                    {1 0 0 0}  \
                    {0 0 -1 0}  \
                    {0 0 0 1}  "

    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
        foreach col $cols {
        [trans GetMatrix] SetElement $row $col \
            [lindex [lindex $elements $row] $col]
        }
    }
    }   else { 
    puts "Creating DTMRIs with -y for vtk compliance"
    trans Scale 1 -1 1
    }
    DTMRI SetTransform trans
    trans Delete

    # produce input vols for DTMRI creation
    set inputNum 0
    set DTMRI(recalculate,gradientVolumes) ""
    foreach slice $offsetsGradient {
        vtkImageExtractSlices extract$slice
        extract$slice SetInput $input
        extract$slice SetSliceOffset $slice
        extract$slice SetSlicePeriod $slicePeriod

        #puts "----------- slice $slice update --------"    
        extract$slice Update
        #puts [[extract$slice GetOutput] Print]

        # pass along in pipeline
        DTMRI SetDiffusionImage $inputNum [extract$slice GetOutput]
        incr inputNum
        
        # put the filter output into a slicer volume
        # Lauren this should be optional
        # make a MRMLVolume for this output
        set name [Volume($v,node) GetName]
        set description "$slice gradient volume derived from volume $name"
        set name gradient${slice}_$name
        set id [DevCreateNewCopiedVolume $v $description $name]
        # save id in case we recalculate the DTMRIs
        lappend DTMRI(recalculate,gradientVolumes) $id
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
        puts "-------computing ras to ijk from scan order----"
        Volume($id,node) ComputeRasToIjkFromScanOrder $order


        # update slicer internals
        MainVolumesUpdate $id
        
    }

    
    puts "----------- DTMRI update --------"
    DTMRI Update
    puts "----------- after DTMRI update --------"


    # put output into a DTMRI volume
    # Lauren if volumes and DTMRIs are the same
    # this should be done like the above
    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume Tensor]
    $newvol Copy Volume($v,node)
    $newvol SetDescription "DTMRI volume"
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
    MainDataCreate Tensor $n Volume
    
    # Set the slicer object's image data to what we created
    DTMRI Update
    Tensor($n,data) SetImageData [DTMRI GetOutput]

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Tensor(idList) $n] == -1} {
        puts "Lauren node doesn't exist, should unfreeze and fix volumes.tcl too"
    } else {
        # Activate the new data object
        MainDataSetActive Tensor $n
    }


    # kill objects
    foreach slice $offsetsGradient {
        extract$slice SetOutput ""
        extract$slice Delete
    }
}






################################################################
# procedures for saving DTMRIs, streamlines, etc.
# TODO: some of this should happen automatically and be in MRML
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIWriteStructuredPoints
# Dump DTMRIs to structured points file.  this ignores
# world to RAS, DTMRIs are just written in scaled ijk coordinate system.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIWriteStructuredPoints {filename} {
    global DTMRI Tensor

    set t $Tensor(activeID)

    vtkStructuredPointsWriter writer
    writer SetInput [Tensor($t,data) GetOutput]
    writer SetFileName $filename
    writer SetFileTypeToBinary
    puts "Writing $filename..."
    writer Write
    writer Delete
    puts "Wrote DTMRI data, id $t, as $filename"
}

#-------------------------------------------------------------------------------
# .PROC DTMRISaveStreamlinesAsIJKPoints
# Save all points from the streamline paths as text files
# .ARGS
# path subdir subdirectory to save the models in
# string name the filename prefix of each model
# int verbose default is 1 
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveStreamlinesAsIJKPoints {subdir name {verbose "1"}} {
    global DTMRI

    set thelist $DTMRI(vtk,streamline,idList)
    set thePoints ""
    set filename "NONE"
    foreach id $thelist {
        set streamln streamln,$id

        # write the points (0,1 both dirs from start pt)
        foreach dir {0 1} {
            set filename "${name}_points${id}_${dir}.txt"
            set filename [file join $subdir $filename]
            # output file
            set out [open $filename w]
            set input [DTMRI(vtk,$streamln) GetHyperStreamline$dir]
            set num [$input GetNumberOfPoints]
            for {set i 0} {$i < $num} {incr i} {
                puts $out [$input GetPoint $i]
            }
            close $out

            #vtkPolyData p
            #p SetPoints [DTMRI(vtk,$streamln) GetHyperStreamline$dir] 
            #vtkDataSetWriter w
            #w SetInput p
            #w SetFileName $filename
            #w Write
            #w Delete
            #p Delete
            # write a file explaining what this is
            #set filename "${name}_readme.txt"
        }
    }

    # let user know something happened
    if {$verbose == "1"} {
    set msg "Wrote streamlines as files, last file was $filename"
    tk_messageBox -message $msg
    }


}

#-------------------------------------------------------------------------------
# .PROC DTMRISaveStreamlinesAsModel
# Save all streamlines as a vtk model(s).
# Each color is written as a separate model.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveStreamlinesAsModel {subdir name {verbose "1"}} {
    global DTMRI

    # append all streamlines together into one model
    catch "appender0 Delete"
    vtkAppendPolyData appender0
    set appenderList 0

    #set thelist {0 1 2}
    set thelist $DTMRI(vtk,streamline,idList)

    # find first color
    set id [lindex $thelist 0]
    set streamline streamln,$id
    set color $DTMRI(vtk,$streamline,color) 
    set count 1
    set colorList $color

    foreach id $thelist {
        set streamln streamln,$id

    # only save it if it's visible
    if {[DTMRI(vtk,$streamln,actor) GetVisibility] == "1"} {

            # find name of color
            set currentColor $DTMRI(vtk,$streamln,color) 
            # if does not match the prev color, 
            # then start appending to new appender
            # (make one model file for each color)
            if {$currentColor != $color } {
                catch "appender$count Delete"
                vtkAppendPolyData appender$count
                lappend appenderList $count
                lappend colorList $currentColor
                incr count
                set color $currentColor
            }
    
            # the model is expected to be in RAS so we must transform
            # it by its actor matrix here
            # ---------------------------------------------
            catch "trans Delete"
            vtkTransform trans
            trans SetMatrix [DTMRI(vtk,$streamln,actor) GetUserMatrix]
            catch "transformer Delete"
            vtkTransformPolyDataFilter transformer
            transformer SetInput [DTMRI(vtk,$streamln) GetOutput]
            transformer SetTransform trans
            transformer Update
            
            # clean up normals
            set p normals
            catch "$p Delete"
            vtkPolyDataNormals $p
            $p SetInput [transformer GetOutput]
            $p SetFeatureAngle 60
            
            # make triangle strips
            set p stripper
            catch "$p Delete"
            vtkStripper $p
            $p SetInput [normals GetOutput]
            
            # add this model to the output
            set appender [lindex $appenderList end]
            appender$appender AddInput [stripper GetOutput] 
            
            # delete vtk objects
            transformer Delete
            trans Delete
            normals Delete
            stripper Delete
        }
    }

    # write the models
    catch "tree Delete"
    vtkMrmlTree tree
    foreach appender $appenderList color $colorList {
#        set filename "$name$appender.vtk"
        #set filename "/spl/tmp/talos.vtk"
        set filename [tk_getSaveFile -defaultextension ".vtk" -title "Save Tracts"]
        if { $filename == "" } {
            return
        }
        catch "writer Delete"
        vtkPolyDataWriter writer
        #writer DebugOn
        #writer SetInput [DTMRI(vtk,$streamln) GetOutput] 
        writer SetInput [appender$appender GetOutput]
        writer SetFileType 2
        writer SetFileName [file join $subdir $filename]
        puts "DTMRI module writing model $filename..."
        writer Update
        writer SetInput ""
        puts "DTMRI module finished writing model $filename."

        # make a MRML node for it
        catch "node Delete"
        vtkMrmlModelNode node
        node SetName "$name$appender"
        node SetFileName  $filename
        node SetFullFileName  $filename
        node SetDescription "tract created by DTMRI.tcl"
        node SetColor $color
        if {$DTMRI(mode,tractColor) == "MultiColor"} {
            node ScalarVisibilityOn
            # set scalar range in node so when mrml read in, 
            # model colors will look good
            # don't always do this since old MRML did not support floats
            eval {node SetScalarRange} \
                [[appender$appender GetOutput] GetScalarRange]

        }
    
        tree AddItem node

        # delete all vtk objects
        node Delete
        writer Delete
        appender$appender Delete
    }

    # Write the MRML file
#    tree Write [file join $subdir "$name.xml"]
    #tree Write [file join "/spl/tmp/" "talos.xml"]
    
    tree Write [file rootname $filename].xml

    tree Delete
    
    # let user know something happened
    if {$verbose == "1"} {
        set msg "Wrote streamlines as file $filename along with a MRML file"
        tk_messageBox -message $msg
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIGetPointsFromSegmentationInIJKofDTMRIVolume
# just for development -- write out list of point in segmentation
# in the ijk coord of the tensor volume, for use in matlab. 
# .ARGS
# int verbose default is 1
# .END
#-------------------------------------------------------------------------------
proc DTMRIGetPointsFromSegmentationInIJKofDTMRIVolume {{verbose 1}} {
    global DTMRI Volume Tensor
    set v $Volume(activeID)

    # ask for user confirmation first
    if {$verbose == "1"} {
    set name [Volume($v,node) GetName]
    set msg "About to list all labelled voxels of volume $name.Go ahead?"
    if {[tk_messageBox -type yesno -message $msg] == "no"} {
        return
    }
    }

    # filter to grab list of points in the segmentation
    vtkProgrammableAttributeDataFilter DTMRI(vtk,programmableFilt)

    set t $Tensor(activeID)
    set v $Volume(activeID)

    DTMRI(vtk,programmableFilt) SetInput [Volume($v,vol) GetOutput] 
    DTMRI(vtk,programmableFilt) SetExecuteMethod \
            DTMRIExecuteForProgrammableFilter
    # output points are stored here
    set DTMRI(streamlineList) ""
    DTMRI(vtk,programmableFilt) Update
    DTMRI(vtk,programmableFilt) Delete

    # Now convert each point to ijk of DTMRI volume
    # transform point into world coordinates
    catch "trans Delete"
    vtkTransform trans
    trans SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    trans Inverse    
    # now transform it into ijk of other volume
    catch "trans2 Delete"
    vtkTransform trans2
    trans2 SetMatrix [Tensor($t,node) GetWldToIjk]

    # to check point is in bounds of the other dataset
    set dims [[Tensor($t,data) GetOutput] GetDimensions]

    # output file
    set out [open points.txt w]

    foreach point $DTMRI(streamlineList) {
        # find out where the point actually is in other ijk space
        set world [eval {trans TransformPoint} $point]
        set ijk [eval {trans2 TransformPoint} $world]

        # if inside other dataset write to file
        set ok 1
        foreach d $dims p $ijk {
            if {$p < 0} {
                set ok 0
            } elseif {$p > $d} {
                set ok 0
            }
        }
        if {$ok} {
            puts $out $ijk        
        }
    }
    close $out

    trans Delete
    trans2 Delete
    set DTMRI(streamlineList) ""
}

################################################################
#  Procedures that deal with coordinate systems
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIGetScaledIjkCoordinatesFromWorldCoordinates
#
# Use our world to ijk matrix information to correct x,y,z.
# The streamline class doesn't know about the
# DTMRI actor's "UserMatrix" (actually implemented with two matrices
# in the glyph class).  We need to transform xyz by
# the inverse of this matrix (almost) so the streamline will start 
# from the right place in the DTMRIs.  The "almost" is because the 
# DTMRIs know about their spacing (vtkImageData) and so we must
# remove the spacing from this matrix.
#
#
# .ARGS
# int x x-coordinate of input world coordinates point 
# int y y-coord
# int z z-coord
# .END
#-------------------------------------------------------------------------------
proc DTMRIGetScaledIjkCoordinatesFromWorldCoordinates {x y z} {
    global DTMRI Tensor

    set t $Tensor(activeID)

    vtkTransform transform
    DTMRICalculateActorMatrix transform $t    
    transform Inverse
    set point [transform TransformPoint $x $y $z]
    transform Delete

    # check point is in bounds of the dataset
    set dims [[Tensor($t,data) GetOutput] GetDimensions]
    set space [[Tensor($t,data) GetOutput] GetSpacing]
    # return "-1 -1 -1" if out of bounds error 
    foreach d $dims s $space p $point {
        if {$p < 0} {
            set point "-1 -1 -1"
        } elseif {$p > [expr $d*$s]} {
            set point "-1 -1 -1"
        }
    }

    return $point
}

#-------------------------------------------------------------------------------
# .PROC DTMRICalculateActorMatrix
# Place the entire Tensor volume in world coordinates
# using this transform.  Uses world to IJk matrix but
# removes the spacing since the data/actor know about this.
# .ARGS
# vtkTransform transform the transform to modify
# int t the id of the DTMRI volume to calculate the matrix for
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateActorMatrix {transform t} {
    global Tensor
    # Grab the node whose data we want to position 
    set node Tensor($t,node)

    # the user matrix is either the reformat matrix
    # to place the slice, OR it needs to place the entire 
    # DTMRI volume.

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
# .PROC DTMRICalculateIJKtoRASRotationMatrix
# 
#  The IJK to RAS matrix has two actions on the DTMRIs.
#  <p>
#  1.  Each DTMRI glyph must be placed at the (x,y,z) location
#  determined by the matrix.  This is analogous to setting the
#  reformat matrix as the actor's user matrix when placing 
#  scalar data.  However, actor matrices do not work here because
#  of number 2, next.
#  <p>
#  2.  Each DTMRI itself must be rotated from ijk to ras.  This
#  uses the ijk to ras matrix, but without any scaling or translation.
#  The DTMRIs are created in the ijk coordinate system so that 
#  diffusion-simulation filters and hyperstreamlines, which do not 
#  know about RAS or actor placement, can correctly handle the data.
#
#
#  <p> This procedure removes translation and scaling 
#  from a volume's ijk to ras matrix, and it returns
#  a rotation matrix that can act on each DTMRI.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateIJKtoRASRotationMatrix {transform t} {
    global Volume Tensor

    # --------------------------------------------------------
    # Rotate DTMRIs to RAS  (actually to World space)
    # --------------------------------------------------------
    # We want the DTMRIs to be displayed in the RAS coordinate system

    # The upper left 3x3 part of this matrix is the rotation.
    # (It also has voxel scaling which we will remove.)
    # -------------------------------------
    #$transform SetMatrix [Tensor($t,node)  GetRasToIjk]
    $transform SetMatrix [Tensor($t,node)  GetWldToIjk]
    # Now it's ijk to ras
    $transform Inverse

    # Remove the voxel scaling from the matrix.
    # -------------------------------------
    scan [Tensor($t,node) GetSpacing] "%g %g %g" res_x res_y res_z

    # We want -y since vtk flips the y axis
    #puts "Not flipping y"
    #set res_y [expr -$res_y]
    $transform Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
            [expr 1.0 / $res_z]

    # Remove the translation part from the last column.
    # (This was in there to center the volume in the cube.)
    # -------------------------------------
    [$transform GetMatrix] SetElement 0 3 0
    [$transform GetMatrix] SetElement 1 3 0
    [$transform GetMatrix] SetElement 2 3 0
    # Set element (4,4) to 1: homogeneous point
    [$transform GetMatrix] SetElement 3 3 1

    # Now this matrix JUST does the rotation needed for ijk->ras.
    # -------------------------------------
    puts "-----------------------------------"
    puts [$transform Print]
    puts "-----------------------------------"

}

