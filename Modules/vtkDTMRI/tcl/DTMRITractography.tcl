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
# FILE:        DTMRITractography.tcl
# PROCEDURES:  
#   DTMRITractographyBuildGUI
#   DTMRISelectRemoveHyperStreamline x y z
#   DTMRISelectChooseHyperStreamline x y z
#   DTMRISelectStartHyperStreamline x y z render
#   DTMRIUpdateStreamlineSettings
#   DTMRIUpdateStreamlines
#   DTMRIUpdateTractingMethod TractingMethod
#   DTMRIUpdateBSplineOrder SplineOrder
#   DTMRIUpdateTractColorToSolid
#   DTMRIUpdateTractColorToMulti
#   DTMRIUpdateTractColor mode
#   DTMRIRemoveAllStreamlines
#   DTMRIAddAllStreamlines
#   DTMRIDeleteAllStreamlines
#   DTMRISeedStreamlinesFromSegmentation verbose
#   DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI
#   DTMRISeedStreamlinesEvenlyInMask verbose
#   DTMRISeedAndSaveStreamlinesFromSegmentation verbose
#   DTMRIFindStreamlinesThroughROI verbose
#   DTMRISaveStreamlinesAsIJKPoints verbose
#   DTMRISaveStreamlinesAsPolyLines subdir name verbose
#   DTMRISaveStreamlinesAsModel verbose
#==========================================================================auto=
proc DTMRITractographyInit {} {

    global DTMRI

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
    set DTMRI(mode,visualizationTypeGuiList) {Help Tracts AutoTracts SaveTracts}
    set DTMRI(mode,visualizationTypeGuiList,tooltip) "Select from this menu\n and settings for each type\n of visualization will appear below."


    # type of tract coloring
    set DTMRI(mode,tractColor) SolidColor;
    set DTMRI(mode,tractColorList) {SolidColor MultiColor}
    set DTMRI(mode,tractColorList,tooltip) "Color tracts with a solid color \nOR MultiColor by scalars from the menu below."


    #------------------------------------
    # Variables for streamline display
    #------------------------------------
    # BSpline Orders
    set DTMRI(stream,BSplineOrder) "3"
    set DTMRI(stream,BSplineOrderList) {"0" "1" "2" "3" "4" "5"}
    set DTMRI(stream,BSplineOrderList,tooltip) {"Order of the BSpline interpolation."}

    # Method Orders
    set DTMRI(stream,MethodOrder) "rk4"
    set DTMRI(stream,MethodOrderList) {"rk2" "rk4" "rk45"}
    set DTMRI(stream,MethodOrderList,tooltip) {"Order of the tractography"}

    # type of tracting method
    set DTMRI(stream,tractingMethod) NoSpline
    set DTMRI(stream,tractingMethodList) {NoSpline BSpline }
    set DTMRI(stream,tractingMethodList,tooltip) {"Method for interpolating signal"}

    set DTMRI(stream,methodvariableList) [list UpperBoundBias LowerBoundBias CorrectionBias ]

    set DTMRI(stream,methodvariableList,text) [list "High Fractional Anisotropy" "Low Fractional Anisotropy" "Correction Bias Magnitude" ]

    set DTMRI(stream,methodvariableList,tooltips) [list \
                               "Inferior bound for fractional anisotropy before adding a regularization bias"\
                               "Lowest fractional anisotropy allowable for tractography"\
                               "Magnitude of the correction bias added for tractography" ]
    set DTMRI(stream,variableList) [list \
                    MaximumPropagationDistance IntegrationStepLength \
                    StepLength Radius  NumberOfSides MaxCurvature MinFractionalAnisotropy]
    set DTMRI(stream,precisevariableList) [list \
                           MaximumPropagationDistance MinimumPropagationDistance TerminalEigenvalue \
                           IntegrationStepLength \
                           StepLength Radius  NumberOfSides  \
                           MaxStep MinStep MaxError MaxAngle LengthOfMaxAngle]
    set DTMRI(stream,variableList,text) [list \
                         "Max Length" "Step Size" \
                         "Smoothness (along)" "Radius"  "Smoothness (around)" "Curvature Threshold" "FA Threshold"]
    set DTMRI(stream,precisevariableList,text) [list \
                            "Max Length" "Min Length" "Terminal Eigenvalue"\
                            "Step Size" \
                            "Smoothness (along)" "Radius"  "Smoothness (around)" \
                            "Max Step" "Min Step" "Max Error" "Max Angle" "Length for Max Angle"]
    set DTMRI(stream,variableList,tooltips) [list \
                         "MaximumPropagationDistance: Tractography will stop after this distance" \
                         "IntegrationStepLength: step size when following path" \
                         "StepLength: Length of each displayed tube segment" \
                         "Radius: Initial radius (thickness) of displayed tube" \
                         "NumberOfSides: Number of sides of displayed tube" \
                         "Curvature Threshold: Max curvature allowed in tracking"\
                         "FA Threshold: If FA falls below this value, tracking stops"]
    
    set DTMRI(stream,precisevariableList,tooltips) [list \
                            "MaximumPropagationDistance: Tractography will stop after this distance" \
                            "MinimumPropagationDistance: Streamline will be rejected if total length is under this value" \
                            "TerminalEigenvalue: Set minimum propagation speed"\
                            "IntegrationStepLength: step size when following path" \
                            "StepLength: Length of each displayed tube segment" \
                            "Radius: Initial radius (thickness) of displayed tube" \
                            "NumberOfSides: Number of sides of displayed tube" \
                            "MaxStep: Maximum step size when following path" \
                            "MinStep: Minimum step size when following path" \
                            "MaxError: Maximum Error of each step" \
                            "MaxAngle: Maximum Angle allowed per fiber" \
                            "MaxError: Length of fiber when considering maximum angle" ]

    # Upper Bound to add regularization Bias
    set DTMRI(stream,UpperBoundBias)  0.3
    # Lower Bound to add regularization Bias
    set DTMRI(stream,LowerBoundBias)  0.2
    # Magnitude of the correction bias
    set DTMRI(stream,CorrectionBias)  0.5

    set DTMRI(stream,MaximumPropagationDistance)  600.0
    set DTMRI(stream,MinimumPropagationDistance)  30.0
    # Terminal Eigenvalue
    set DTMRI(stream,TerminalEigenvalue)  0.0
    # nominal integration step size (expressed as a fraction of the
    # size of each cell)  0.2 is default
    set DTMRI(stream,IntegrationStepLength)  0.1
    # Set/Get the Maximum Step of integration
    set DTMRI(stream,MaxStep) 1.0
    # Set/Get the Minimum Step of integration
    set DTMRI(stream,MinStep) 0.001
    # Set/Get the Maximum Error per step of integration
    set DTMRI(stream,MaxError) 0.000001

    # Set/Get the Maximum Angle of a fiber
    set DTMRI(stream,MaxAngle) 30

    # Set/Get the length of the fiber when considering the maximum angle
    set DTMRI(stream,LengthOfMaxAngle) 1
    # Set / get the length of a tube segment composing the
    # hyperstreamline. The length is specified as a fraction of the
    # diagonal length of the input bounding box.  0.01 is vtk default
    set DTMRI(stream,StepLength)  0.005
    # radius of (polydata) tube that is displayed
    #set DTMRI(stream,Radius)  0.2 
    set DTMRI(stream,Radius)  0.4
    # sides of tube
    #set DTMRI(stream,NumberOfSides)  4
    set DTMRI(stream,NumberOfSides)  6
    #set DTMRI(stream,MaxCurvature) 1.3
    set DTMRI(stream,MaxCurvature) 1.15
    set DTMRI(stream,MinFractionalAnisotropy) 0.07
    
     set DTMRI(activeStreamlineID) ""
    
    #------------------------------------
    # Variablel to Find tracts that pass through ROI values
    #------------------------------------
    set DTMRI(stream,ListLabels) {0}

    #------------------------------------
    # Variables for auto streamline display
    #------------------------------------
    set DTMRI(mode,autoTractsLabel,tooltip) "A tract will be seeded in each voxel of the ROI which is colored with this label."

}




#-------------------------------------------------------------------------------
# .PROC DTMRITractographyBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRITractographyBuildGUI {} {

    global DTMRI Tensor Module Gui

    #-------------------------------------------
    # Tract frame
    #-------------------------------------------
    set fTract $Module(DTMRI,fTract)
    set f $fTract

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fTractsMode  -bg $Gui(activeWorkspace)
    pack $f.fTractsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fVisMethods  -bg $Gui(activeWorkspace) -bd 2
    pack $f.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true

    #-------------------------------------------
    # Tract->Active frame
    #-------------------------------------------
    set f $fTract.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m


    #-------------------------------------------
    # Display-> Notebook -> Tract frame->TractsMode frame
    #-------------------------------------------
    set f $fTract.fTractsMode

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
    # Display-> Notebook -> Tract frame ->VisMethods frame
    #-------------------------------------------
    set f $fTract.fVisMethods

    frame $f.fVisMode -bg $Gui(activeWorkspace) 
    pack $f.fVisMode -side top -padx 0 -pady 0 -fill x

    # note the height is necessary to place frames inside later
    frame $f.fVisParams -bg $Gui(activeWorkspace) -height 500
    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    $f.fVisMode config -relief sunken -bd 2
    #$f.fVisParams config -relief groove -bd 3
    #$f.fVisParams config -bd 3

    #-----------------------------------------2--
    # Display-> Notebook ->Tract frame ->VisMethods ->VisMode frame
    #-------------------------------------------
    set f $fTract.fVisMethods.fVisMode

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
    # Display-> Notebook ->Tract frame ->VisMethods ->VisParams frame
    #-------------------------------------------
    set f $fTract.fVisMethods.fVisParams
    set fParams $f

    # make a parameters frame for each visualization type
    # types are: Help  Tracts AutoTracts SaveTracts
    foreach frame $DTMRI(mode,visualizationTypeGuiList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
        set DTMRI(frame,$frame) $f.f$frame
    }
    raise $DTMRI(frame,$DTMRI(mode,visualizationTypeGui))

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Help frame
    #-------------------------------------------
    set f $fParams.fHelp

    DevAddLabel $f.l "Select from the Visualization\n menu above to adjust\n visualization parameters."
    DevAddLabel $f.l2 "For tractography, point at \nthe voxel of interest with\n the mouse and click\n the letter 's'. To delete\n a tract, point and click 'd'."
    pack $f.l $f.l2 -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts frame
    #-------------------------------------------
    set f $fParams.fTracts

    foreach frame "Colors Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors
    foreach frame "ChooseColor ColorBy ColorByVol" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ChooseColor frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fChooseColor

    DevAddButton $f.b "Color:" {ShowColors DTMRIUpdateTractColorToSolid}
    eval {entry $f.e -width 20 \
          -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    pack $f.b $f.e -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

    lappend Label(colorWidgetList) $f.e

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ColorBy frame
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
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ColorByVol frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fColorByVol

    # menu to select a volume: will set Volume(activeID)
    set name ColorByVolume
    DevAddSelectButton  Volume $f $name "Color by Volume:" Pack \
    "First select Color by MultiColor, \nthen select the volume to use \nto color the tracts. \nFor example to color by FA, \ncreate the FA volume using the \n<More...> tab in this module, \nthen the <Scalars> tab.  \nThen select that volume from this list." \
    13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Entries frame
    #-------------------------------------------
    set f $fParams.fTracts.fEntries

    frame $f.fTractingMethod -bg $Gui(activeWorkspace) 
    pack $f.fTractingMethod -side top -padx 0 -pady 0 -fill x

    # note the height is necessary to place frames inside later
    frame $f.fTractingVar -bg $Gui(activeWorkspace) -height 500
    pack $f.fTractingVar -side top -padx 0 -pady $Gui(pad) -fill both -expand true

    #    frame $f -bg $Gui(activeWorkspace)
    #    pack $f -side top -padx $Gui(pad) -pady 1 -fill x

    set f $fParams.fTracts.fEntries.fTractingMethod

    eval {label $f.lVis -text "Tracting Method: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(stream,tractingMethod) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    foreach vis $DTMRI(stream,tractingMethodList) {
        $f.mbVis.m add command -label $vis \
        -command "DTMRIUpdateTractingMethod $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbTractingMethod) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,tractingMethodList,tooltip)

    set f $fParams.fTracts.fEntries.fTractingVar

    #    frame $f -bg $Gui(activeWorkspace)
    #place $f.fVar -in $f -relheight 1.0 -relwidth 1.0
    #    pack $f -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    #    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true

    foreach frame $DTMRI(stream,tractingMethodList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady 1 -fill x
        set DTMRI(stream,tractingFrame,$frame) $f.f$frame
    }
    raise $DTMRI(stream,tractingFrame,$DTMRI(stream,tractingMethod))
    
    set f $DTMRI(stream,tractingFrame,BSpline)
    
    frame $f.fBSplineOrder -bg $Gui(activeWorkspace) 
    pack $f.fBSplineOrder -side top -padx 0 -pady 0 -fill x

    set f $f.fBSplineOrder

    eval {label $f.lVis -text "Spline Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,BSplineOrder) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(stream,BSplineOrderList) {
        $f.mbVis.m add command -label $vis \
        -command "DTMRIUpdateBSplineOrder $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbBSplineOrder) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    set f $DTMRI(stream,tractingFrame,BSpline)
    
    frame $f.fMethodOrder -bg $Gui(activeWorkspace) 
    pack $f.fMethodOrder -side top -padx 0 -pady 0 -fill x

    set f $f.fMethodOrder

    eval {label $f.lVis -text "Method Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,MethodOrder) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # save menubutton for config
    set DTMRI(gui,mbMethodOrder) $f.mbVis
    # Add menu items
    foreach vis $DTMRI(stream,MethodOrderList) {
        $f.mbVis.m add command -label $vis \
        -command "set DTMRI(vtk,ivps) DTMRI(vtk,$vis); $DTMRI(gui,mbMethodOrder) config -text $vis"
    }
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    foreach entry $DTMRI(stream,methodvariableList) \
        text $DTMRI(stream,methodvariableList,text) \
        tip $DTMRI(stream,methodvariableList,tooltips) {
        
        set f $DTMRI(stream,tractingFrame,BSpline)
        
        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
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

    foreach entry $DTMRI(stream,variableList) \
    text $DTMRI(stream,variableList,text) \
    tip $DTMRI(stream,variableList,tooltips) {

        set f $DTMRI(stream,tractingFrame,NoSpline)

        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
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

    foreach entry $DTMRI(stream,precisevariableList) \
        text $DTMRI(stream,precisevariableList,text) \
        tip $DTMRI(stream,precisevariableList,tooltips) {
        
        set f $DTMRI(stream,tractingFrame,BSpline)
        
        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
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
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts frame
    #-------------------------------------------
    set f $fParams.fAutoTracts

    foreach frame "Label2 Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Autotracts->Label2 frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fLabel2

    DevAddLabel $f.l "Automatically start tractography\nfrom each voxel in an ROI."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Entries frame
    set f $fParams.fAutoTracts.fEntries
    foreach frame "Volume" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
    foreach frame "ChooseLabel FindTracts" {
        frame $f.f$frame -bg $Gui(activeWorkspace) -bd 2
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->Volume frame
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
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel
    
    foreach frame "Title Label Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
    
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Title frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fTitle
    
    DevAddLabel $f.l "Create Tracts from label value"
    pack $f.l -side top
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Label frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fLabel
    
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
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Apply frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fApply
    DevAddButton $f.bApply "Seed 'Tracts' in ROI" \
        {puts "Seeding streamlines"; DTMRISeedStreamlinesFromSegmentation}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Seed a 'tract' from each point in the ROI.\nThis can be slow; be patient."


    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts
    
    foreach frame "Title ListLabels Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
   
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->Title frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fTitle
    
    DevAddLabel $f.l "Choose Tracts that pass through\na set of labels"
    pack $f.l -side top
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->ListLabels frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fListLabels
    
    DevAddLabel $f.l "List of labels:"
    
    eval {entry $f.eName -width 25 \
          -textvariable DTMRI(stream,ListLabels)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace)}
    
    pack $f.l $f.eName -side left
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->Apply frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fApply
    
    DevAddButton $f.bApply "Find 'Tracts' through ROI" \
        {DTMRIFindStreamlinesThroughROI}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)


    ##########################################################
    #  SAVETRACTS   (frame raised when SaveTracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts frame
    #-------------------------------------------
    set f $fParams.fSaveTracts

    foreach frame "Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display--> Notebook ->Tract frame>VisMethods->VisParams->SaveTracts->Entries frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries
    foreach frame "Info1 Apply1 Info2 Apply2" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Info frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fInfo1
    DevAddLabel $f.l "Save the tracts you have created."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fApply1
    DevAddButton $f.bApply "Save tracts in model file(s)" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsModel}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save tracts to vtk file(s).\nEach color of tract will become a separate model.\n Choose the initial part of the filename, and models\nwill be saved as filename_0.vtk, filename_1.vtk, etc.\nThen you can load the models into slicer\n(they must be re-added to the mrml tree)."

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Info frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fInfo2
    DevAddLabel $f.l "Save currently visible tracts as a polyline.\n Useful for further processing on tracts.\nONLY DEVELOPERS"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fApply2
    DevAddButton $f.bApply "Save tracts in vtk file" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsPolyLines "" tracts}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save visible tracts to vtk file as a set of polylines."

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisUpdate frame
    #-------------------------------------------
    #set f $fDisplay.fVisUpdate
    #DevAddButton $f.bTest "Junk" {puts "this button is junk"} 4    
    #pack $f.bTest -side top -padx 0 -pady 0

}


################################################################
#  visualization procedures that deal with tracts
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRISelectRemoveHyperStreamline
#  Remove the selected hyperstreamline
# .ARGS
# int x
# int y
# int z
# .END
#-------------------------------------------------------------------------------
proc DTMRISelectRemoveHyperStreamline {x y z} {
    global DTMRI
    global Select

    puts "Select Picker  (x,y,z):  $x $y $z"

    # see which actor was picked
    set actor [DTMRI(vtk,picker) GetActor]

    DTMRI(vtk,streamlineControl) DeleteStreamline $actor
}


#-------------------------------------------------------------------------------
# .PROC DTMRISelectChooseHyperStreamline
# 
# .ARGS
# int x
# int y
# int z
# .END
#-------------------------------------------------------------------------------
proc DTMRISelectChooseHyperStreamline {x y z} {
    global DTMRI
    global Select

    puts "Select Picker  (x,y,z):  $x $y $z"

    # see which actor was picked
    set actor [DTMRI(vtk,picker) GetActor]

    set DTMRI(activeStreamlineID) \
        [DTMRI(vtk,streamlineControl) GetStreamlineIndexFromActor $actor]
}

#-------------------------------------------------------------------------------
# .PROC DTMRISelectStartHyperStreamline
# Given x,y,z in world coordinates, starts a streamline from that point
# in the active DTMRI dataset.
# .ARGS
# int x 
# int y
# int z 
# bool render Defaults to true
# .END
#-------------------------------------------------------------------------------
proc DTMRISelectStartHyperStreamline {x y z {render "true"} } {
    global DTMRI Tensor Color Label Volume
    global Select


    set t $Tensor(activeID)
    if {$t == "" || $t == $Tensor(idNone)} {
        puts "DTMRISelect: No DTMRIs have been read into the slicer"
        return
    }
    
    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On


    # Set up all parameters from the user
    # NOTE: TODO: make an Apply button and only call this 
    # when the user changes settings. Here it is too slow.
    DTMRIUpdateStreamlineSettings

    # actually create and display the streamline
    DTMRI(vtk,streamlineControl) SeedStreamlineFromPoint $x $y $z
    DTMRI(vtk,streamlineControl) AddStreamlinesToScene

    # Force pipeline execution and render scene
    #------------------------------------
    if { $render == "true" } {
        Render3D
    }
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateStreamlineSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateStreamlineSettings {} {
    global DTMRI

    # set up type of streamline to create
    switch $DTMRI(stream,tractingMethod) {
        "BSpline" {

            # What type of streamline object to create
            DTMRI(vtk,streamlineControl) UseVtkPreciseHyperStreamlinePoints

            # apply correct settings to example streamline object
            set streamline "streamlineControl,vtkPreciseHyperStreamlinePoints"

            DTMRI(vtk,$streamline) SetMethod $DTMRI(vtk,ivps)
            if {$DTMRI(stream,LowerBoundBias) > $DTMRI(stream,UpperBoundBias)} {
                set DTMRI(stream,UpperBoundBias) $DTMRI(stream,LowerBoundBias)
            }
            DTMRI(vtk,$streamline) SetTerminalFractionalAnisotropy \
                $DTMRI(stream,LowerBoundBias)
            foreach var $DTMRI(stream,methodvariableList) {
                DTMRI(vtk,itf) Set$var $DTMRI(stream,$var)
            }
            foreach var $DTMRI(stream,precisevariableList) {
                if { $var == "MaxAngle" } {
                    DTMRI(vtk,$streamline) Set$var \
                        [ expr cos( $DTMRI(stream,$var) * 3.14159265 / 180 ) ]
                } else {
                    DTMRI(vtk,$streamline) Set$var $DTMRI(stream,$var)
                }
    
            }

        }

        "NoSpline" {
            # What type of streamline object to create
            DTMRI(vtk,streamlineControl) UseVtkHyperStreamlinePoints

            # apply correct settings to example streamline object
            set streamline "streamlineControl,vtkHyperStreamlinePoints"
            foreach var $DTMRI(stream,variableList) {
                DTMRI(vtk,$streamline) Set$var $DTMRI(stream,$var)
            }

            
        }
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
# .PROC DTMRIUpdateTractingMethod
# .ARGS
# string TractingMethod
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractingMethod { TractingMethod } {
    global DTMRI Tensor
 
    if {$TractingMethod != $DTMRI(stream,tractingMethod) } {
        set DTMRI(stream,tractingMethod) $TractingMethod
        switch $DTMRI(stream,tractingMethod) {
            "NoSpline" {
                raise $DTMRI(stream,tractingFrame,NoSpline)
                focus $DTMRI(stream,tractingFrame,NoSpline)
                $DTMRI(gui,mbTractingMethod)    config -text $TractingMethod
                
            }
            "BSpline" {
                raise $DTMRI(stream,tractingFrame,BSpline)
                focus $DTMRI(stream,tractingFrame,BSpline)
                $DTMRI(gui,mbTractingMethod)    config -text $TractingMethod

                # Apparently all of these Updates really are needed
                # set up the BSpline tractography pipeline
                set t $Tensor(activeID)
                set DTMRI(vtk,BSpline,data) 1
                set DTMRI(vtk,BSpline,init) 1;
                DTMRI(vtk,itf) SetDataBounds [Tensor($t,data) GetOutput]
                for {set i 0} {$i < 6} {incr i} {
                    DTMRI(vtk,extractor($i)) SetInput [Tensor($t,data) GetOutput]
                }
                for {set i 0} {$i < 6} {incr i} {
                    DTMRI(vtk,extractor($i)) Update
                    DTMRI(vtk,bspline($i)) SetInput [DTMRI(vtk,extractor($i)) GetOutput]
                }          
                DTMRIUpdateBSplineOrder $DTMRI(stream,BSplineOrder)
                for {set i 0} {$i < 6} {incr i} {
                    DTMRI(vtk,bspline($i)) Update
                    DTMRI(vtk,impComp($i)) SetInput [DTMRI(vtk,bspline($i)) GetOutput]
                }

            }
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateBSplineOrder
# .ARGS
# string SplineOrder
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateBSplineOrder { SplineOrder } {
    global DTMRI
    if { $SplineOrder != $DTMRI(stream,BSplineOrder) } {
        set DTMRI(stream,BSplineOrder) $SplineOrder
        $DTMRI(gui,mbBSplineOrder)    config -text $SplineOrder

        for {set i 0} {$i < 6} {incr i 1} {
            DTMRI(vtk,impComp($i)) SetSplineOrder $SplineOrder
            DTMRI(vtk,bspline($i)) SetSplineOrder $SplineOrder
            if { $DTMRI(vtk,BSpline,init) == 1 } {
                DTMRI(vtk,bspline($i)) Update
                DTMRI(vtk,impComp($i)) SetInput [DTMRI(vtk,bspline($i)) GetOutput]
            }
        }

    }
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTractColorToSolid
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractColorToSolid {} {
    DTMRIUpdateTractColor SolidColor
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTractColorToMulti
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractColorToMulti {} {
    DTMRIUpdateTractColor MultiColor
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTractColor
# configure the coloring to be solid or scalar per triangle 
# .ARGS
# string mode Optional, defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractColor {{mode ""}} {
    global DTMRI Volume Tensor Color Label

    if {$mode == ""} {
        set mode $DTMRI(mode,tractColor)
    }
    
    # whether scalars should be displayed
    switch $mode {
        "SolidColor" {

            # display new mode while we are working...
            $DTMRI(gui,mbTractColor)    config -text $mode

            # find color from the name we have saved; use 1st as default
            set c [lindex $Color(idList) 1]
            foreach id $Color(idList) {
                if {[Color($id,node) GetName] == $Label(name)} {
                    set c $id
                }
            }
            # set up properties of the new actors we will create
            set prop [DTMRI(vtk,streamlineControl) GetStreamlineProperty] 
            #$prop SetAmbient       [Color($c,node) GetAmbient]
            #$prop SetDiffuse       [Color($c,node) GetDiffuse]
            #$prop SetSpecular      [Color($c,node) GetSpecular]
            #$prop SetSpecularPower [Color($c,node) GetPower]
            eval "$prop SetColor" [Color($c,node) GetDiffuseColor] 

            # display solid colors instead of scalars
            DTMRI(vtk,streamlineControl) ScalarVisibilityOff
        }
        "MultiColor" {
            # put the volume we wish to color by as the Scalars field 
            # in the tensor volume.
            set t $Tensor(activeID)            
            set v $Volume(activeID)

            # make sure they have the same extent
            set ext1 [[Tensor($t,data) GetOutput] GetWholeExtent]
            set ext2 [[Volume($v,vol) GetOutput] GetWholeExtent]
            if {[string equal $ext1 $ext2]} {

                $DTMRI(gui,mbTractColor)    config -text $mode

                # put the scalars there
                DTMRI(vtk,streamline,merge) SetScalars [Volume($v,vol) GetOutput]

                # setting scalars like this caused a crash in
                # the vtkMrmlDataVolume's vtkImageAccumulateDiscrete. why??
                #[[Tensor($t,data) GetOutput] GetPointData] SetScalars \
                #    [[[Volume($v,vol) GetOutput] GetPointData] GetScalars]

                DTMRI(vtk,streamlineControl) ScalarVisibilityOn
                eval {[DTMRI(vtk,streamlineControl) GetStreamlineLookupTable] \
                          SetRange} [[Volume($v,vol) GetOutput] GetScalarRange]
                
                # set up properties of the new actors we will create
                set prop [DTMRI(vtk,streamlineControl) GetStreamlineProperty] 
                # By default make them brighter than slicer default colors
                # slicer's colors have ambient 0, diffuse 1, and specular 0
                #$prop SetAmbient       0.5
                #$prop SetDiffuse       0.1
                #$prop SetSpecular      0.2

            } else {
                set message "Please select a volume with the same dimensions as the DTMRI dataset (for example one you have created from the Scalars tab)."
                set result [tk_messageBox  -message $message]

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

    DTMRI(vtk,streamlineControl) RemoveStreamlinesFromScene
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddAllStreamlines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddAllStreamlines {} {
    global DTMRI

    DTMRI(vtk,streamlineControl) AddStreamlinesToScene
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRIDeleteAllStreamlines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIDeleteAllStreamlines {} {
    global DTMRI

    DTMRI(vtk,streamlineControl) DeleteAllStreamlines
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC DTMRISeedStreamlinesFromSegmentation
# Seeds streamlines at all points in a segmentation.
# .ARGS
# int verbose Defaults to 1
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedStreamlinesFromSegmentation {{verbose 1}} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name [Volume($v,node) GetName]
        set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    DTMRI(vtk,streamlineControl) SeedStreamlinesFromROI
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    puts "Creating and displaying new tracts..."

    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    DTMRI(vtk,streamlineControl) AddStreamlinesToScene
}



#-------------------------------------------------------------------------------
# .PROC DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI {{verbose 1}} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name [Volume($v,node) GetName]
        set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)

    DTMRI(vtk,streamlineControl) SetInputROIForIntersection \
        [Volume($v,vol) GetOutput] 

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    DTMRI(vtk,streamlineControl) SeedStreamlinesFromROIIntersectWithROI2
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    puts "Creating and displaying new tracts..."

    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    DTMRI(vtk,streamlineControl) AddStreamlinesToScene
}



#-------------------------------------------------------------------------------
# .PROC DTMRISeedStreamlinesEvenlyInMask
# Seeds streamlines at all points in a segmentation.
# .ARGS
# int verbose defaults to 1
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedStreamlinesEvenlyInMask {{verbose 1}} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name [Volume($v,node) GetName]
        set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    DTMRI(vtk,streamlineControl) SeedStreamlinesEvenlyInROI 
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    puts "Creating and displaying new tracts..."

    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    DTMRI(vtk,streamlineControl) AddStreamlinesToScene
}


#-------------------------------------------------------------------------------
# .PROC DTMRISeedAndSaveStreamlinesFromSegmentation
# Seeds streamlines at all points in a segmentation.
# This does not display anything, just one by one seeds
# the streamline and saves it to disk. So nothing is 
# visualized, this is for exporting files only.
# (Actually displaying all of the streamlines would be impossible
# with a whole brain ROI.)
# .ARGS
# int verbose Defaults to 1
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedAndSaveStreamlinesFromSegmentation {{verbose 1}} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # set base filename for all stored files
    set filename [tk_getSaveFile  -title "Save Tracts: Choose Initial Filename"]
    if { $filename == "" } {
        return
    }


    # make a subdirectory for them, named the same as the files                            
    set name [file root [file tail $filename]]
    set dir [file dirname $filename]
    set newdir [file join $dir tract_files_$name]
    file mkdir $newdir
    set filename [file join $newdir $name]
    # make a subdirectory for the vtk models                                               
    set newdir2 [file join $newdir vtk_model_files]
    file mkdir $newdir2
    set filename2 [file join $newdir2 $name]

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name [Volume($v,node) GetName]
        set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # make sure the settings are current for the models we save to disk              
    #DTMRIUpdateTractColor                                                          
    DTMRIUpdateStreamlineSettings

    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Starting to seed streamlines. Files will be $filename*.*"
    DTMRI(vtk,streamlineControl) SeedAndSaveStreamlinesFromROI \
        $filename  $filename2

    # let user know something happened
    if {$verbose == "1"} {
        set msg "Finished writing tracts. The filename is: $filename*.*"
        tk_messageBox -message $msg
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIFindStreamlinesThroughROI
# Seeds streamlines at all points in a segmentation.
# .ARGS
# int verbose defaults to 1
# .END
#-------------------------------------------------------------------------------
proc DTMRIFindStreamlinesThroughROI { {verbose 1} } {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    #Define list of ROI Values
    set numLabels [llength $DTMRI(stream,ListLabels)]
   
    DTMRI(vtk,ListLabels) SetNumberOfValues $numLabels
    set idx 0
    foreach value $DTMRI(stream,ListLabels) {
      eval "DTMRI(vtk,ListLabels) SetValue" $idx $value
      incr idx
    }  
    
    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)
    DTMRI(vtk,streamlineControl) SetInputMultipleROIValues DTMRI(vtk,ListLabels)
    DTMRI(vtk,streamlineControl) SetConvolutionKernel DTMRI(vtk,convKernel)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    DTMRI(vtk,streamlineControl) FindStreamlinesThatPassThroughROI
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    
    puts "Creating and displaying new tracts..."
    DTMRI(vtk,streamlineControl) HighlightStreamlinesPassTest
    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    DTMRI(vtk,streamlineControl) AddStreamlinesToScene
}




#-------------------------------------------------------------------------------
# .PROC DTMRISaveStreamlinesAsIJKPoints
# Save all points from the streamline paths as text files
# .ARGS
# int verbose default is 1 
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveStreamlinesAsIJKPoints {{verbose "1"}} {
    
    # check we have streamlines
    if {[DTMRI(vtk,streamlineControl) GetNumberOfStreamlines] < 1} {
        set msg "There are no tracts to save. Please create tracts first."
        tk_messageBox -message $msg
        return
    }

    # set base filename for all stored files
    set filename [tk_getSaveFile  -title "Save Tracts: Choose Initial Filename"]
    if { $filename == "" } {
        return
    }

    # save the tracts
    DTMRI(vtk,streamlineControl) SaveStreamlinesAsTextFiles \
        $filename 

    # let user know something happened
    if {$verbose == "1"} {
        set msg "Finished writing tracts. The filenames are: $filename*.txt"
        tk_messageBox -message $msg
    }

} 

#-------------------------------------------------------------------------------
# .PROC DTMRISaveStreamlinesAsPolyLines
# Save all points from the streamline paths as polyline in a vtkfile
# .ARGS
# path subdir subdirectory to save the models in
# string name the filename prefix of each model
# int verbose default is 1 
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveStreamlinesAsPolyLines {subdir name {verbose "1"}} {
    global DTMRI

    set thelist $DTMRI(vtk,streamline,idList)
    set thePoints ""
    set filename "NONE"
    
    set filename [tk_getSaveFile -defaultextension ".vtk" -title "Save Streamlines as polyline"]
    if { $filename == "" } {
        return
    }
    
    
    #Write streamlines to a vtkPolyData file with lines
    
    #1. Count total number of points
    set numpts 0
    foreach id $thelist {
        set streamln streamln,$id
        
        foreach dir {0 1} {
            set numpts [expr $numpts + [[DTMRI(vtk,$streamln) GetHyperStreamline$dir] GetNumberOfPoints]]
        }
    }
    
    #2. Fill point data and cell data
    vtkPolyData p
    vtkCellArray c
    vtkPoints pt
    
    pt SetNumberOfPoints $numpts
    
    #global id for cell
    set idcell 0
    #global id for points
    set idp 0
    foreach id $thelist {
        set streamln streamln,$id
        
        foreach dir {0 1} {
            set cellnumpts [[DTMRI(vtk,$streamln) GetHyperStreamline$dir] GetNumberOfPoints]
            c InsertNextCell $cellnumpts
            #Get transformation matrix: IJK -> RAS
            set matrix [DTMRI(vtk,$streamln,actor) GetUserMatrix]
            
            for {set i 0} {$i < $cellnumpts} {incr i} {
                set ijkpoint [[DTMRI(vtk,$streamln) GetHyperStreamline$dir] GetPoint $i]
                set raspoint [$matrix MultiplyPoint [lindex $ijkpoint 0] [lindex $ijkpoint 1] [lindex $ijkpoint 2] 1]
                #If we want to save points in ijk
                #eval "pt SetPoint $idp" [[DTMRI(vtk,$streamln) GetHyperStreamline$dir] GetPoint $i]
                
                #If we want to save points in ras
                pt SetPoint $idp [lindex $raspoint 0] [lindex $raspoint 1] [lindex $raspoint 2]
                
                c InsertCellPoint $idp
                incr idp
            
            }   
            incr idcell
        }
    }
    
    #3. Build polydata
    p SetLines c
    p SetPoints pt
    p Update
    
    
    #4. Write PolyData
    vtkPolyDataWriter w
    w SetFileName [file join $subdir $filename]  
    w SetInput p
    w SetFileTypeToASCII
    w Write 

    #5. Delete Objects
    w Delete
    p Delete
    c Delete
    pt Delete

    # let user know something happened
    if {$verbose == "1"} {
        set msg "Wrote streamlines as vtk files, last file was $filename"
        tk_messageBox -message $msg
    }


}

#-------------------------------------------------------------------------------
# .PROC DTMRISaveStreamlinesAsModel
# Save all streamlines as a vtk model(s).
# Each color is written as a separate model.
# .ARGS
# int verbose default is 1
# .END
#-------------------------------------------------------------------------------
proc DTMRISaveStreamlinesAsModel {{verbose "1"}} {
    
    # check we have streamlines
    if {[DTMRI(vtk,streamlineControl) GetNumberOfStreamlines] < 1} {
        set msg "There are no tracts to save. Please create tracts first."
        tk_messageBox -message $msg
        return
    }

    # set base filename for all stored files
    set filename [tk_getSaveFile  -title "Save Tracts: Choose Initial Filename"]
    if { $filename == "" } {
        return
    }

    # set name for models in slicer interface
    set modelname [file root [file tail $filename]]

    # save the models as well as a MRML file with their colors
    DTMRI(vtk,streamlineControl) SaveStreamlinesAsPolyData \
        $filename $modelname Mrml(colorTree)

    # let user know something happened
    if {$verbose == "1"} {
        set msg "Finished writing tracts and scene file. The filename is: $filename.xml"
        tk_messageBox -message $msg
    }

}



