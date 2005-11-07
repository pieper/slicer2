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
#   DTMRITractographyInit
#   DTMRITractographyBuildGUI
#   DTMRISelectRemoveHyperStreamline x y z
#   DTMRISelectChooseHyperStreamline x y z
#   DTMRISelectStartHyperStreamline x y z render
#   DTMRIUpdateStreamlineSettings
#   DTMRIUpdateStreamlines
#   DTMRIUpdateTractingMethod TractingMethod
#   DTMRIUpdateBSplineOrder SplineOrder
#   DTMRIUpdateTractColorToSolid
#   DTMRIUpdateTractColorToSolidFromShowLabels
#   DTMRIUpdateROILabelWidgetFromShowLabels
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




#-------------------------------------------------------------------------------
# .PROC DTMRITractographyInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRITractographyInit {} {

    global DTMRI Volume Label

    # Version info for files within DTMRI module
    #------------------------------------
    set m "Tractography"
    lappend DTMRI(versions) [ParseCVSInfo $m \
                                 {$Revision: 1.36 $} {$Date: 2005/11/07 01:00:30 $}]

    #------------------------------------
    # Tab 1: Settings (Per-streamline settings)
    #------------------------------------

    # type of tract coloring
    set DTMRI(mode,tractColor) SolidColor;
    set DTMRI(mode,tractColorList) {SolidColor MultiColor}
    set DTMRI(mode,tractColorList,tooltip) "Color tracts with a solid color \nOR MultiColor by scalars from the menu below."
    
    # Label/color value for coloring tracts
    set DTMRI(TractLabel) $DTMRI(defaultLabel)
    # Name of this label/color
    set DTMRI(TractLabelName) ""
    # Color ID corresponding to the label
    set DTMRI(TractLabelColorID) ""


    # types of tractography: subclasses of vtkHyperStreamline
    #------------------------------------
    set DTMRI(stream,tractingMethod) NoSpline
    # put the default last so its GUI is built on top.
    set DTMRI(stream,tractingMethodList) {BSpline NoSpline}
    set DTMRI(stream,tractingMethodList,tooltip) {"Method for interpolating signal"}


    # vtkHyperStreamline tractography variables
    # Initialize here (some can be changed in the GUI).
    #------------------------------------
    # Max length (in number of steps?)
    set DTMRI(stream,MaximumPropagationDistance)  600.0
    set DTMRI(stream,MinimumPropagationDistance)  30.0
    # Terminal Eigenvalue
    set DTMRI(stream,TerminalEigenvalue)  0.0
    # nominal integration step size (expressed as a fraction of the
    # size of each cell)  0.2 is default
    set DTMRI(stream,IntegrationStepLength)  0.1
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


    # "NoSpline" tractography variables (lists are for GUI creation)
    #------------------------------------
    set DTMRI(stream,variableList) \
        [list \
             MaximumPropagationDistance IntegrationStepLength \
             StepLength Radius  NumberOfSides MaxCurvature MinFractionalAnisotropy]

    set DTMRI(stream,variableList,text) \
        [list \
             "Max Length" "Step Size" \
             "Smoothness (along)" "Radius"  "Smoothness (around)" "Curvature Threshold" "FA Threshold"]
    set DTMRI(stream,variableList,tooltips) \
        [list \
             "MaximumPropagationDistance: Tractography will stop after this distance" \
             "IntegrationStepLength: step size when following path" \
             "StepLength: Length of each displayed tube segment" \
             "Radius: Initial radius (thickness) of displayed tube" \
             "NumberOfSides: Number of sides of displayed tube" \
             "Curvature Threshold: Max curvature allowed in tracking"\
             "FA Threshold: If FA falls below this value, tracking stops"]
    #set DTMRI(stream,MaxCurvature) 1.3
    set DTMRI(stream,MaxCurvature) 1.15
    set DTMRI(stream,MinFractionalAnisotropy) 0.07
    

    # B-spline tractography variables (lists are for GUI creation)
    #------------------------------------
    set DTMRI(stream,methodvariableList) \
        [list UpperBoundBias LowerBoundBias CorrectionBias ]

    set DTMRI(stream,methodvariableList,text) \
        [list "High Fractional Anisotropy" "Low Fractional Anisotropy" "Correction Bias Magnitude" ]

    set DTMRI(stream,methodvariableList,tooltips) \
        [list \
             "Inferior bound for fractional anisotropy before adding a regularization bias"\
             "Lowest fractional anisotropy allowable for tractography"\
             "Magnitude of the correction bias added for tractography" ]

    set DTMRI(stream,precisevariableList) \
        [list \
             MaximumPropagationDistance MinimumPropagationDistance TerminalEigenvalue \
             IntegrationStepLength \
             StepLength Radius  NumberOfSides  \
             MaxStep MinStep MaxError MaxAngle LengthOfMaxAngle]

    set DTMRI(stream,precisevariableList,text) \
        [list \
             "Max Length" "Min Length" "Terminal Eigenvalue"\
             "Step Size" \
             "Smoothness (along)" "Radius"  "Smoothness (around)" \
             "Max Step" "Min Step" "Max Error" "Max Angle" "Length for Max Angle"]
    set DTMRI(stream,precisevariableList,tooltips) \
        [list \
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

    # BSpline Orders
    set DTMRI(stream,BSplineOrder) "3"
    set DTMRI(stream,BSplineOrderList) {"0" "1" "2" "3" "4" "5"}
    set DTMRI(stream,BSplineOrderList,tooltip) {"Order of the BSpline interpolation."}

    # Method Orders
    set DTMRI(stream,MethodOrder) "rk4"
    set DTMRI(stream,MethodOrderList) {"rk2" "rk4" "rk45"}
    set DTMRI(stream,MethodOrderList,tooltip) {"Order of the tractography"}

    # Upper Bound to add regularization Bias
    set DTMRI(stream,UpperBoundBias)  0.3
    # Lower Bound to add regularization Bias
    set DTMRI(stream,LowerBoundBias)  0.2
    # Magnitude of the correction bias
    set DTMRI(stream,CorrectionBias)  0.5

    # Set/Get the Minimum Step of integration
    set DTMRI(stream,MinStep) 0.001
    # Set/Get the Maximum Step of integration
    set DTMRI(stream,MaxStep) 1.0
    # Set/Get the Maximum Error per step of integration
    set DTMRI(stream,MaxError) 0.000001

    # Set/Get the Maximum Angle of a fiber
    set DTMRI(stream,MaxAngle) 30

    # Set/Get the length of the fiber when considering the maximum angle
    set DTMRI(stream,LengthOfMaxAngle) 1


    #------------------------------------
    # Tab 2: Seeding (automatic from ROI)
    #------------------------------------

    # Labelmap volume that gives input seed locations
    set DTMRI(ROILabelmap) $Volume(idNone)
    set DTMRI(ROI2Labelmap) $Volume(idNone)
    # Label value indicating seed location
    set DTMRI(ROILabel) $DTMRI(defaultLabel)
    # Label value indicating seed location
    set DTMRI(ROI2Label) $DTMRI(defaultLabel)
    # Color value corresponding to the label
    set DTMRI(ROILabelColorID) ""
    # Color value corresponding to the label
    set DTMRI(ROI2LabelColorID) ""

    #------------------------------------
    # Tab 3: Display of (all) streamlines
    #------------------------------------

    # whether we are currently displaying tracts
    set DTMRI(mode,visualizationType,tractsOn) 0n
    set DTMRI(mode,visualizationType,tractsOnList) {On Off Delete}
    set DTMRI(mode,visualizationType,tractsOnList,tooltip) \
        [list \
             "Display all 'tracts'" \
             "Hide all 'tracts'" \
             "Clear all 'tracts'" ]
    # guard against multiple actor add/remove from GUI
    set DTMRI(vtk,streamline,actorsAdded) 1

    # Non-labelmap volume that can be used for coloring tracts
    set DTMRI(ColorByVolume) $Volume(idNone)

    # whether we are currently clipping tracts with slice planes
    set DTMRI(mode,visualizationType,tractsClip) 0ff
    set DTMRI(mode,visualizationType,tractsClipList) {On Off}
    set DTMRI(mode,visualizationType,tractsClipList,tooltip) \
        [list \
             "Clip tracts with slice planes. Clipping settings are in Models->Clip." \
             "Do not clip tracts." ]
    
    set DTMRI(activeStreamlineID) ""
    
    #------------------------------------
    # Variable to Find tracts that pass through ROI values
    #------------------------------------
    set DTMRI(stream,ListANDLabels) ""
    set DTMRI(stream,ListNOTLabels) ""
    set DTMRI(stream,threshhold) 1
    set DTMRI(stream,threshold,max) 100
    set DTMRI(stream,threshold,min) 1
}


#-------------------------------------------------------------------------------
# .PROC DTMRITractographyBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRITractographyBuildGUI {} {

    global DTMRI Tensor Module Gui Label Volume

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Tract
    #    Active
    #    Notebook
    #       Settings
    #          Colors
    #          TractingMethod
    #          TractingVar
    #             BSpline
    #                BSplineOrder
    #                MethodOrder
    #                MethodVariables ...
    #                PreciseVariables ...
    #             NoSpline
    #                Variables...
    #       Seeding
    #
    #       Display
    #          OnOffDelete
    #          ColorBy
    #          ColorByVol
    #-------------------------------------------

    #-------------------------------------------
    # Tract frame
    #-------------------------------------------
    set fTract $Module(DTMRI,fTract)
    set f $fTract

    # active tensor frame
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    # notebook frame (to contain settings, seeding, display frames)
    Notebook:create $f.fNotebook \
        -pages {{Settings} {Seeding} {Display}} \
        -pad 2 \
        -bg $Gui(activeWorkspace) \
        -height 500 \
        -width 400
    pack $f.fNotebook -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true

    #-------------------------------------------
    # Tract->Active frame
    #-------------------------------------------
    set f $fTract.fActive

    # menu to select active DTMRI
    DevAddSelectButton  DTMRI $f Active "Active DTMRI:" Pack \
        "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m


    #-------------------------------------------
    # Tract->Notebook frame
    #-------------------------------------------
    set f $fTract.fNotebook

    set fSettings [Notebook:frame $f {Settings}]
    set fSeeding [Notebook:frame $f {Seeding}]
    set fDisplay [Notebook:frame $f {Display}]
    foreach frame "$fSettings $fSeeding $fDisplay" {
        $frame configure -relief groove -bd 3
    }


    ##########################################################
    #
    #  Settings Frame
    #
    ##########################################################

    #-------------------------------------------
    # Tract->Notebook->Settings frame
    #-------------------------------------------
    set f $fSettings

    foreach frame "Colors TractingMethod TractingVar" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }
    # note the height is necessary to place frames inside later
    $f.fTractingVar configure -height 500
    #pack $f.fTractingVar -side top -padx 0 -pady $Gui(pad) -fill both -expand true


    #-------------------------------------------
    # Tract->Notebook->Settings->Colors frame
    #-------------------------------------------
    set f $fSettings.fColors

    # label/color for new tracts we create
    eval {button $f.bOutput -text "Color:" -width 8 \
              -command "ShowLabels DTMRIUpdateTractColorToSolidFromShowLabels"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
              -textvariable DTMRI(TractLabel)} $Gui(WEA)
    bind $f.eOutput <Return>   "DTMRIUpdateTractColorToSolid"
    eval {entry $f.eName -width 14 \
              -textvariable DTMRI(TractLabelName)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    #grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    #grid $f.eOutput $f.eName -sticky w
    pack $f.bOutput -padx $Gui(pad) -pady $Gui(pad) -side left
    pack $f.eName $f.eOutput -padx $Gui(pad) -pady $Gui(pad) -side right

    # save for changing color later
    set DTMRI(TractLabelWidget) $f.eName


    #-------------------------------------------
    # Tract->Notebook->Settings->TractingMethod frame
    #-------------------------------------------
    set f $fSettings.fTractingMethod

    eval {label $f.lVis -text "Interpolation Method: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(stream,tractingMethod) \
              -relief raised -bd 2 -width 11 \
              -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis -side left -pady 1 -padx $Gui(pad)
    pack $f.mbVis -side right -pady 1 -padx $Gui(pad)
    foreach vis $DTMRI(stream,tractingMethodList) {
        $f.mbVis.m add command -label $vis \
            -command "DTMRIUpdateTractingMethod $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbTractingMethod) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,tractingMethodList,tooltip)

    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar frame
    #-------------------------------------------
    set f $fSettings.fTractingVar

    foreach frame $DTMRI(stream,tractingMethodList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady 1 -fill x
        set DTMRI(stream,tractingFrame,$frame) $f.f$frame
    }
    
    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->BSpline frame
    #-------------------------------------------
    set f $DTMRI(stream,tractingFrame,BSpline)
    
    frame $f.fBSplineOrder -bg $Gui(activeWorkspace) 
    pack $f.fBSplineOrder -side top -padx 0 -pady 0 -fill x

    frame $f.fMethodOrder -bg $Gui(activeWorkspace) 
    pack $f.fMethodOrder -side top -padx 0 -pady 0 -fill x

    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->BSpline->BSplineOrder frame
    #-------------------------------------------
    set f $DTMRI(stream,tractingFrame,BSpline).fBSplineOrder

    eval {label $f.lVis -text "Spline Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,BSplineOrder) \
              -relief raised -bd 2 -width 11 \
              -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis  -side left -pady 1 -padx $Gui(pad)
    pack $f.mbVis -side right -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(stream,BSplineOrderList) {
        $f.mbVis.m add command -label $vis \
            -command "DTMRIUpdateBSplineOrder $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbBSplineOrder) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->BSpline->MethodOrder frame
    #-------------------------------------------
    set f $DTMRI(stream,tractingFrame,BSpline).fMethodOrder

    eval {label $f.lVis -text "Method Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,MethodOrder) \
              -relief raised -bd 2 -width 11 \
              -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis  -side left -pady 1 -padx $Gui(pad)
    pack $f.mbVis -side right -pady 1 -padx $Gui(pad)
    # save menubutton for config
    set DTMRI(gui,mbMethodOrder) $f.mbVis
    # Add menu items
    foreach vis $DTMRI(stream,MethodOrderList) {
        $f.mbVis.m add command -label $vis \
            -command "set DTMRI(vtk,ivps) DTMRI(vtk,$vis); $DTMRI(gui,mbMethodOrder) config -text $vis"
    }
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->BSpline->MethodVariables frames
    #-------------------------------------------
    foreach entry $DTMRI(stream,methodvariableList) \
        text $DTMRI(stream,methodvariableList,text) \
        tip $DTMRI(stream,methodvariableList,tooltips) {
            
            set f $DTMRI(stream,tractingFrame,BSpline)
            
            frame $f.f$entry -bg $Gui(activeWorkspace)
            #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
            pack $f.f$entry -side top -padx 0 -pady 1 -fill x
            set f $f.f$entry

            eval {label $f.l$entry -text "$text:"} $Gui(WLA)
            eval {entry $f.e$entry -width 8 \
                      -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
            TooltipAdd $f.l$entry $tip
            TooltipAdd $f.e$entry $tip
            pack $f.l$entry -side left  -padx $Gui(pad)
            pack $f.e$entry -side right  -padx $Gui(pad)
        }

    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->BSpline->PreciseVariables frames
    #-------------------------------------------
    foreach entry $DTMRI(stream,precisevariableList) \
        text $DTMRI(stream,precisevariableList,text) \
        tip $DTMRI(stream,precisevariableList,tooltips) {
            
            set f $DTMRI(stream,tractingFrame,BSpline)
            
            frame $f.f$entry -bg $Gui(activeWorkspace)
            #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
            pack $f.f$entry -side top -padx 0 -pady 1 -fill x
            set f $f.f$entry

            eval {label $f.l$entry -text "$text:"} $Gui(WLA)
            eval {entry $f.e$entry -width 8 \
                      -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
            TooltipAdd $f.l$entry $tip
            TooltipAdd $f.e$entry $tip
            pack $f.l$entry -side left  -padx $Gui(pad)
            pack $f.e$entry -side right  -padx $Gui(pad)
        }


    #-------------------------------------------
    # Tract->Notebook->Settings->TractingVar->NoSpline->Variables frames
    #-------------------------------------------

    foreach entry $DTMRI(stream,variableList) \
        text $DTMRI(stream,variableList,text) \
        tip $DTMRI(stream,variableList,tooltips) {

            set f $DTMRI(stream,tractingFrame,NoSpline)

            frame $f.f$entry -bg $Gui(activeWorkspace)
            #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
            pack $f.f$entry -side top -padx 0 -pady 1 -fill x
            set f $f.f$entry

            eval {label $f.l$entry -text "$text:"} $Gui(WLA)
            eval {entry $f.e$entry -width 8 \
                      -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
            TooltipAdd $f.l$entry $tip
            TooltipAdd $f.e$entry $tip
            pack $f.l$entry -side left  -padx $Gui(pad)
            pack $f.e$entry -side right  -padx $Gui(pad)
        }

    ##########################################################
    #
    #  Seeding Frame
    #
    ##########################################################

    #-------------------------------------------
    # Tract->Notebook->Seeding frame
    #-------------------------------------------
    set f $fSeeding

    foreach frame "Title ROIMethod Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }
    $f.fROIMethod config -relief groove -bd 2 
    $f.fEntries config -relief groove -bd 2

    #-------------------------------------------
    # Tract->Notebook->Seeding->Title frame
    #-------------------------------------------
    set f $fSeeding.fTitle

    DevAddLabel $f.l "Seed tracts in a region of interest (ROI)."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod
    foreach frame "ROI ChooseLabel ROI2 ChooseLabel2 Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady 2 -fill both
    }

    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod->ROI frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod.fROI

    # menu to select a volume: will set DTMRI(ROILabelmap)
    # works with DevUpdateNodeSelectButton in UpdateMRML
    set name ROILabelmap
    DevAddSelectButton  DTMRI $f $name "Seed ROI:" Pack \
        "The region of interest in which to seed tracts."\
        13
    
    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod->ChooseLabel frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod.fChooseLabel
    
    # label in input ROI for seeding
    eval {button $f.bOutput -text "Seed Label:" \
              -command "ShowLabels DTMRIUpdateROILabelWidgetFromShowLabels"} $Gui(WBA)
    eval {entry $f.eOutput -width 4 \
              -textvariable DTMRI(ROILabel)} $Gui(WEA)

    bind $f.eOutput <Return>   "DTMRIUpdateLabelWidget ROILabel"
    eval {entry $f.eName -width 14 \
              -textvariable DTMRI(ROILabelName)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    # save for changing color later
    set DTMRI(ROILabelWidget) $f.eName

    set tip \
        "Choose the color (label) of the region of interest."
    TooltipAdd  $f.bOutput $tip
    TooltipAdd  $f.eOutput $tip
    TooltipAdd  $f.eName $tip

    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod->ROI2 frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod.fROI2

    # menu to select a volume: will set DTMRI(ROI2Labelmap)
    # works with DevUpdateNodeSelectButton in UpdateMRML
    set name ROI2Labelmap
    DevAddSelectButton  \
        DTMRI $f $name "Selection ROI (Optional):" Pack \
        "Tracts will selected if they pass through this ROI.\nChoose None to skip this step." \
        13

    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod->ChooseLabel2 frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod.fChooseLabel2
    
    # label in input ROI for seeding
    eval {button $f.bOutput -text "Select Label:" \
              -command "ShowLabels DTMRIUpdateROI2LabelWidgetFromShowLabels"} $Gui(WBA)
    eval {entry $f.eOutput -width 4 \
              -textvariable DTMRI(ROI2Label)} $Gui(WEA)

    bind $f.eOutput <Return>   "DTMRIUpdateLabelWidget ROI2Label"
    eval {entry $f.eName -width 14 \
              -textvariable DTMRI(ROI2LabelName)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    # save for changing color later
    set DTMRI(ROI2LabelWidget) $f.eName

    set tip \
        "Choose the color (label) of the region of interest."
    TooltipAdd  $f.bOutput $tip
    TooltipAdd  $f.eOutput $tip
    TooltipAdd  $f.eName $tip

    #-------------------------------------------
    # Tract->Notebook->Seeding->ROIMethod->Apply frame
    #-------------------------------------------
    set f $fSeeding.fROIMethod.fApply
    DevAddButton $f.bApply "Seed Tracts" \
        {puts "Seeding streamlines"; DTMRISeedStreamlinesInROI}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Seed tracts in the region of interest.\nThis can be slow for large ROIs."

    #-------------------------------------------
    # Tract->Notebook->Seeding->Entries frame
    #-------------------------------------------
    set f $fSeeding.fEntries
    foreach frame "FindTracts" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Tract->Notebook->Seeding->Entries->FindTracts frame
    #-------------------------------------------
    set f $fSeeding.fEntries.fFindTracts
    
    foreach frame "Title ListANDLabels ListNOTLabels Sensitivity Apply1 Apply2" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
    
    #-------------------------------------------
    # Tract->Notebook->Seeding->Entries->FindTracts->Title frame
    #-------------------------------------------
    set f $fSeeding.fEntries.fFindTracts.fTitle
    
    DevAddLabel $f.l "Choose Tracts that pass through\na set of labels"
    pack $f.l -side top
    
    #-------------------------------------------
    # Tract->Notebook->Seeding->Entries->FindTracts->ListLabels frame
    #-------------------------------------------
    set f $fSeeding.fEntries.fFindTracts.fListANDLabels
    
    DevAddLabel $f.l "List of labels:"
    pack $f.l
    
     DevAddLabel $f.lAND "AND:"
    eval {entry $f.eAND -width 25 \
              -textvariable DTMRI(stream,ListANDLabels)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace)}
    
    pack $f.lAND $f.eAND -side left
    
    set f $fSeeding.fEntries.fFindTracts.fListNOTLabels
    
    DevAddLabel $f.lNOT "NOT:"
    eval {entry $f.eNOT -width 25 \
              -textvariable DTMRI(stream,ListNOTLabels)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace)}
    
    pack $f.lNOT $f.eNOT -side left
    
    set f $fSeeding.fEntries.fFindTracts.fSensitivity
    
    DevAddLabel $f.l "Sensitivity (H<->L):"
    eval {entry $f.e -width 3 \
                      -textvariable DTMRI(stream,threshold)} \
                $Gui(WEA)
    eval {scale $f.s -from $DTMRI(stream,threshold,min) \
                     -to $DTMRI(stream,threshold,max)    \
          -variable  DTMRI(stream,threshold) \
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)
      
    pack $f.l $f.e $f.s -side left
    
    #-------------------------------------------
    # Tract->Notebook->Seeding->Entries->FindTracts->Apply frame
    #-------------------------------------------
    set f $fSeeding.fEntries.fFindTracts.fApply1
    
    DevAddButton $f.bApply1 "Find 'Tracts' through ROI" \
        {DTMRIFindStreamlinesThroughROI}
    pack $f.bApply1 -side top -padx $Gui(pad) -pady $Gui(pad)

    set f $fSeeding.fEntries.fFindTracts.fApply2
    DevAddButton $f.bApply2 "Apply" \
        {DTMRIDeleteStreamlinesNotPassTest}
    DevAddButton $f.bApply3 "Reset" \
        {DTMRIResetStreamlinesThroughROI}
        
    pack $f.bApply2 $f.bApply3 -side left -padx $Gui(pad) -pady $Gui(pad)
    
    
    ##########################################################
    #
    #  Display Frame
    #
    ##########################################################

    #-------------------------------------------
    # Tract->Notebook->Display frame
    #-------------------------------------------
    set f $fDisplay

    foreach frame "OnOffDelete ColorBy ColorByVol Clip" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }


    #-------------------------------------------
    # Tract->Notebook->Display->OnOffDelete frame
    #-------------------------------------------
    set f $fDisplay.fOnOffDelete
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
    # Tract->Notebook->Display->ColorBy frame
    #-------------------------------------------
    set f $fDisplay.fColorBy
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
    # Tract->Notebook->Display->ColorByVol frame
    #-------------------------------------------
    set f $fDisplay.fColorByVol

    # menu to select a volume: will set DTMRI(ColorByVolume)
    # works with DevUpdateNodeSelectButton in UpdateMRML
    set name ColorByVolume
    DevAddSelectButton  DTMRI $f $name "Color by Volume:" Pack \
        "First select Color by MultiColor, \nthen select the volume to use \nto color the tracts. \nFor example to color by FA, \ncreate the FA volume using the \n<More...> tab in this module, \nthen the <Scalars> tab.  \nThen select that volume from this list." \
        13

    #-------------------------------------------
    # Tract->Notebook->Display->Clip frame
    #-------------------------------------------
    set f $fDisplay.fClip

    eval {label $f.lVis -text "Clip 'Tracts': "} $Gui(WLA)
    pack $f.lVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationType,tractsClipList) \
        tip $DTMRI(mode,visualizationType,tractsClipList,tooltip) {
            eval {radiobutton $f.r$vis \
                      -text $vis \
                      -command "DTMRITractographySetClipping" \
                      -value $vis \
                      -variable DTMRI(mode,visualizationType,tractsClip) \
                      -indicatoron 0} $Gui(WCA)

            pack $f.r$vis -side left -fill x
            TooltipAdd $f.r$vis $tip
        }    

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
        [[DTMRI(vtk,streamlineControl) GetDisplayTracts] \
             GetStreamlineIndexFromActor $actor]
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
    [DTMRI(vtk,streamlineControl) GetSeedTracts] SeedStreamlineFromPoint $x $y $z
    [DTMRI(vtk,streamlineControl) GetDisplayTracts] AddStreamlinesToScene

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

    set seedTracts [DTMRI(vtk,streamlineControl) GetSeedTracts]

    # set up type of streamline to create
    switch $DTMRI(stream,tractingMethod) {
        "BSpline" {

            # What type of streamline object to create
            $seedTracts UseVtkPreciseHyperStreamlinePoints

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
            $seedTracts UseVtkHyperStreamlinePoints

            # apply correct settings to example streamline object
            set streamline "streamlineControl,vtkHyperStreamlinePoints"
            foreach var $DTMRI(stream,variableList) {
                DTMRI(vtk,$streamline) Set$var $DTMRI(stream,$var)
            }

            
        }
    }

    # No matter what kind we are making, set the display parameters
    set radius [DTMRI(vtk,$streamline) GetRadius]
    set sides [DTMRI(vtk,$streamline) GetNumberOfSides]
    
    set display [DTMRI(vtk,streamlineControl) GetDisplayTracts]
    $display SetTubeRadius $radius
    $display SetTubeNumberOfSides $sides
    
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
                
                if {$t != "" } {
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

    # update the color and label numbers
    DTMRIUpdateLabelWidget TractLabel

    # update our pipeline
    DTMRIUpdateTractColor SolidColor
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTractColorToSolidFromShowLabels
# Callback after ShowLabels window receives a label selection
# from the user.  Gets this value from Label(label) and stores 
# it as DTMRI(TractLabel).  Then calls DTMRIUpdateTractColorToSolid.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateTractColorToSolidFromShowLabels {} {

    # update the color and label numbers
    DTMRIUpdateLabelWidgetFromShowLabels TractLabel

    # update our pipeline
    DTMRIUpdateTractColor SolidColor
}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateROILabelWidgetFromShowLabels
# Callback after ShowLabels window receives a label selection
# from the user.  Calls DTMRIUpdateLabelWidgetFromShowLabels
# with an argument (which is not possible to do directly as the
# callback proc).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateROILabelWidgetFromShowLabels {} {

    DTMRIUpdateLabelWidgetFromShowLabels ROILabel

}

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateROILabelWidgetFromShowLabels
# Callback after ShowLabels window receives a label selection
# from the user.  Calls DTMRIUpdateLabelWidgetFromShowLabels
# with an argument (which is not possible to do directly as the
# callback proc).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateROI2LabelWidgetFromShowLabels {} {

    DTMRIUpdateLabelWidgetFromShowLabels ROI2Label

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

    set displayTracts [DTMRI(vtk,streamlineControl) GetDisplayTracts]

    # whether scalars should be displayed
    switch $mode {
        "SolidColor" {

            # Get the color calculated in UpdateLabelWidget procs
            set c $DTMRI(TractLabelColorID)

            # display new mode while we are working...
            $DTMRI(gui,mbTractColor)    config -text $mode

            # set up properties of the new actors we will create
            set prop [$displayTracts GetStreamlineProperty] 
            if { $c != "" } {
                #$prop SetAmbient       [Color($c,node) GetAmbient]
                #$prop SetDiffuse       [Color($c,node) GetDiffuse]
                #$prop SetSpecular      [Color($c,node) GetSpecular]
                #$prop SetSpecularPower [Color($c,node) GetPower]
                eval "$prop SetColor" [Color($c,node) GetDiffuseColor] 
            }

            # display solid colors instead of scalars
            $displayTracts ScalarVisibilityOff
        }
        "MultiColor" {
            # put the volume we wish to color by as the Scalars field 
            # in the tensor volume.
            set t $Tensor(activeID)            
            set v $DTMRI(ColorByVolume)

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

                $displayTracts ScalarVisibilityOn
                eval {[$displayTracts GetStreamlineLookupTable] \
                          SetRange} [[Volume($v,vol) GetOutput] GetScalarRange]
                
                # set up properties of the new actors we will create
                set prop [$displayTracts GetStreamlineProperty] 
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

    [DTMRI(vtk,streamlineControl) GetDisplayTracts] RemoveStreamlinesFromScene
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

    [DTMRI(vtk,streamlineControl) GetDisplayTracts] AddStreamlinesToScene
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


proc DTMRISeedStreamlinesInROI {} {
    global DTMRI Volume

    # call the seeding procedure depending on whether
    # a selection ROI was chosen
    if {$DTMRI(ROI2Labelmap) == $Volume(idNone) || $DTMRI(ROI2Labelmap) == ""} {

        # no selection ROI
        DTMRISeedStreamlinesFromSegmentation

    } else {

        # we have a selection ROI
        DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI
    }

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
    set v $DTMRI(ROILabelmap)

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

    # cast to short (as these are labelmaps the values are really integers
    # so this prevents errors with float labelmaps which come from editing
    # scalar volumes derived from the tensors).
    vtkImageCast castVSeedROI
    castVSeedROI SetOutputScalarTypeToShort
    castVSeedROI SetInput [Volume($v,vol) GetOutput] 
    castVSeedROI Update

    # set up the input segmented volume
    set seedTracts [DTMRI(vtk,streamlineControl) GetSeedTracts]
    $seedTracts SetInputROI [castVSeedROI GetOutput] 
    $seedTracts SetInputROIValue $DTMRI(ROILabel)

    # color the streamlines like this ROI
    set DTMRI(TractLabel) $DTMRI(ROILabel)
    DTMRIUpdateTractColorToSolid

    # make sure the settings are current
    DTMRIUpdateStreamlineSettings
    
    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $seedTracts SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    $seedTracts SeedStreamlinesInROI
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    puts "Creating and displaying new tracts..."

    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    [DTMRI(vtk,streamlineControl) GetDisplayTracts] AddStreamlinesToScene

    castVSeedROI Delete
}



#-------------------------------------------------------------------------------
# .PROC DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI
# Seed streamlines in all points in ROI, keep those that pass through
# ROI2.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISeedStreamlinesFromSegmentationAndIntersectWithROI {{verbose 1}} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    # seed at the labeled voxels in this volume
    set vSeedROI $DTMRI(ROILabelmap)
    # save and display the tracts that hit labeled voxels in this volume
    set vSaveROI $DTMRI(ROI2Labelmap)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($vSeedROI,node) GetLabelMap] != 1} {
        set name [Volume($vSeedROI,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }
    # make sure they are using a segmentation (labelmap)
    if {[Volume($vSaveROI,node) GetLabelMap] != 1} {
        set name [Volume($vSaveROI,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # ask for user confirmation first
    if {$verbose == "1"} {
        set name1 [Volume($vSeedROI,node) GetName]
        set name2 [Volume($vSaveROI,node) GetName]
        set msg "Ready to seed tracts in all labeled voxels of volume $name1, and keep the ones that pass through labeled voxels in volume $name2.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }
    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    # cast to short (as these are labelmaps the values are really integers
    # so this prevents errors with float labelmaps which come from editing
    # scalar volumes derived from the tensors).
    vtkImageCast castVSeedROI
    castVSeedROI SetOutputScalarTypeToShort
    castVSeedROI SetInput [Volume($vSeedROI,vol) GetOutput] 
    castVSeedROI Update

    vtkImageCast castVSaveROI
    castVSaveROI SetOutputScalarTypeToShort
    castVSaveROI SetInput [Volume($vSaveROI,vol) GetOutput] 
    castVSaveROI Update

    # set up the input segmented volumes
    set seedTracts [DTMRI(vtk,streamlineControl) GetSeedTracts]
    $seedTracts SetInputROI [castVSeedROI GetOutput]
    $seedTracts SetInputROIValue $DTMRI(ROILabel)

    $seedTracts SetInputROI2 [castVSaveROI GetOutput]
    $seedTracts SetInputROI2Value $DTMRI(ROI2Label)
    
    
    # Get positioning information from the MRML node for the seed ROI
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($vSeedROI,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $seedTracts SetROIToWorld transform
    transform Delete

    # Get positioning information from the MRML node for the second ROI
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($vSaveROI,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $seedTracts SetROI2ToWorld transform
    transform Delete

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"

    
    $seedTracts SeedStreamlinesFromROIIntersectWithROI2
    puts "New number of tracts will be: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"

    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    puts "Creating and displaying new tracts..."
    [DTMRI(vtk,streamlineControl) GetDisplayTracts] AddStreamlinesToScene

    castVSeedROI Delete
    castVSaveROI Delete
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
    set v $DTMRI(ROILabelmap)

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
    set seedTracts [DTMRI(vtk,streamlineControl) GetSeedTracts]
    $seedTracts SetInputROI [Volume($v,vol) GetOutput] 
    $seedTracts SetInputROIValue $DTMRI(ROILabel)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $seedTracts SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Starting to seed streamlines. Files will be $filename*.*"
    $seedTracts SeedAndSaveStreamlinesFromROI \
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
    set v $DTMRI(ROILabelmap)

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
   set numLabels [llength $DTMRI(stream,ListANDLabels)]
    
    DTMRI(vtk,ListANDLabels) SetNumberOfValues $numLabels
    set idx 0
    foreach value $DTMRI(stream,ListANDLabels) {
        eval "DTMRI(vtk,ListANDLabels) SetValue" $idx $value
        incr idx
    }
    
    set numLabels [llength $DTMRI(stream,ListNOTLabels)]
    
    DTMRI(vtk,ListNOTLabels) SetNumberOfValues $numLabels
    set idx 0
    foreach value $DTMRI(stream,ListNOTLabels) {
        eval "DTMRI(vtk,ListNOTLabels) SetValue" $idx $value
        incr idx
    }    
 
    # set up the input segmented volume
    set ROISelectTracts DTMRI(vtk,ROISelectTracts)
    
    $ROISelectTracts SetInputROI [Volume($v,vol) GetOutput] 
    $ROISelectTracts SetInputROIValue $DTMRI(ROILabel)
    $ROISelectTracts SetInputANDROIValues DTMRI(vtk,ListANDLabels)
    $ROISelectTracts SetInputNOTROIValues DTMRI(vtk,ListNOTLabels)
    $ROISelectTracts SetConvolutionKernel DTMRI(vtk,convKernel)    
    $ROISelectTracts SetPassThreshold $DTMRI(stream,threshold)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $ROISelectTracts SetROIToWorld transform
    transform Delete

    # create all streamlines
    puts "Initial number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    $ROISelectTracts FindStreamlinesThatPassThroughROI
 
    puts "Creating and displaying new tracts..."
    $ROISelectTracts HighlightStreamlinesPassTest
    # actually display streamlines 
    # (this is the slow part since it causes pipeline execution)
    Render3D
}

proc DTMRIDeleteStreamlinesNotPassTest { {verbose 1} } {

  global DTMRI
  
  DTMRI(vtk,ROISelectTracts) DeleteStreamlinesNotPassTest
  puts "Final number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
  Render3D
}

proc DTMRIResetStreamlinesThroughROI { {verbose 1} } {

  global DTMRI
  
  DTMRI(vtk,ROISelectTracts) ResetStreamlinesPassTest
  Render3D
}  

proc DTMRITractographyColorROIFromStreamlines {} {
    global DTMRI Label Volume

    set v $DTMRI(ROILabelmap)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # set up the input segmented volume
    set ColorROIFromTracts [DTMRI(vtk,streamlineControl) GetColorROIFromTracts]

    $ColorROIFromTracts SetInputROIForColoring [Volume($v,vol) GetOutput] 
    #$ColorROIFromTracts SetInputROIValue $DTMRI(ROILabel)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    $ColorROIFromTracts SetROIToWorld transform
    transform Delete

    $ColorROIFromTracts ColorROIFromStreamlines

    set output [$ColorROIFromTracts GetOutputROIForColoring]

    # export output to the slicer environment:
    # slicer MRML volume creation and display
    set v2 [DevCreateNewCopiedVolume $v "Color back from clusters" "TractColors_$v"]
    Volume($v2,vol) SetImageData $output
    MainVolumesUpdate $v2
    # tell the node what type of data so MRML file will be okay
    Volume($v2,node) SetScalarType [$output GetScalarType]
    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v2
    RenderAll
}


proc DTMRITractographySetClipping {{val ""}} {
    global Tensor DTMRI

    if {$val ==""} {
        switch $DTMRI(mode,visualizationType,tractsClip) {
            "On" {
                set val 1
            }
            "Off" {
                set val 0
            }
        }
    }

    if {$val ==  "1"} {
        
        if {[[Slice(clipPlanes) GetFunction] GetNumberOfItems] < 1} {
            set DTMRI(mode,visualizationType,tractsClip) Off
            set message "First select slice planes to clip by in the Models->Clip tab."
            tk_messageBox  -message $message
            return
        }


        # Use the current clip planes from the models GUI
        # copy its properties into a new object since
        # we have to transform it.
        vtkImplicitBoolean clipPlanes
        clipPlanes SetOperationType [Slice(clipPlanes) GetOperationType]
        set functions [Slice(clipPlanes) GetFunction]
        $functions InitTraversal
        set func [$functions GetNextItemAsObject]

        while {$func != ""} {
            puts $func
            eval {clipPlanes AddFunction} $func
            set func [$functions GetNextItemAsObject]            
        }

        # They have to be transformed into scaled IJK of the tensors
        catch "transform Delete"
        vtkTransform transform
        # WorldToTensorScaledIJK
        set t $Tensor(activeID)
        DTMRICalculateActorMatrix transform $t    
        #transform Inverse
        clipPlanes SetTransform transform
        transform Delete  
        
        [DTMRI(vtk,streamlineControl) GetDisplayTracts] \
            SetClipFunction clipPlanes
        clipPlanes Delete
    }

    # Turn clipping on/off
    [DTMRI(vtk,streamlineControl) GetDisplayTracts] \
        SetClipping $val

    # display it
    Render3D
    
}
