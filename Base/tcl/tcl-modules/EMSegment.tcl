#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
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
# FILE:        EMSegment.tcl
# PROCEDURES:  
#   EMSegmentInit
#   EMSegmentBuildGUI
#   EMSegmentProbColorLabelWindow
#   EMSegmentEnter
#   EMSegmentExit
#   EMSegmentShowFile
#   EMSegmentBindingCallback
#   EMSegmentUpdateMRML
#   EMSegmentProbVolumeSelectNode
#   EMSegmentMRMLDeleteCreateNodesNodesListEntries 
#   EMSegmentSaveSetting 
#   EMSegmentStartEM
#   EMSegmentImportCIM
#   EMSegmentClickLabel
#   EMSegmentDisplayClassDefinition
#   EMSegmentUseSamples
#   EMSegmentChangeClass
#   EMSegmentUpdateClassButton 
#   EMSegmentCalculateClassMeanCovariance
#   EMSegmentCalcProb
#   EMSegmentWriteTextBox  
#   EMSegmentEraseSample  
#   EMSegmentReadTextBox  
#   EMSegmentScrolledText  
#   EMSegmentScrolledHorizontal  
#   EMSegmentCreateDeleteClasses  
#   EMSegmentSetMaxInputChannelDef
#   EMSegmentReadGreyValue
#   EMSegmentDefineSample
#   EMSegmentDisplaySampleLogValues 
#   EMSegmentDisplaySampleLine 
#   EMSegmentMoveRedSampleLine
#   EMSegmentDisplaySampleCross
#   EMSegmentEnterSample 
#   EMSegmentCreateRedLine 
#   EMSegmentCreateRedCross 
#   EMSegmentLeaveSample 
#   EMSegmentChangeVolumeGraph
#   EMSegmentCreateGraphButton
#   EMSegmentMultipleDrawDeleteCurveRegion
#   EMSegmentDeleteGraphButton
#   EMSegmentDrawDeleteCurveRegion
#   EMSegmentCalculateClassCurveRegion 
#   EMSegmentGlobalGuassianInside 
#   EMSegmentFindLineMaximum 
#   EMSegmentFind2LineMaximum 
#   EMSegmentCheckClassCurve
#   EMSegmentGraphGridCalculate 
#   EMSegmentDefineCalculationBorder 
#   EMSegmentCalculateClassGaussianDistribution
#   EMSegmentCalculateClassGaussian2DDistribution
#   EMSegmentSetRegionsToZero 
#   EMSegmentSetGaussRegionToZero 
#   EMSegmentDrawClassCurveRegion
#   EMSegmentDrawSingleCurve
#   EMSegmentDrawClassRegion
#   EMSegmentDrawRegion
#   EMSegmentCalulateGraphXleUXscnXscpXpos 
#   EMSegmentCalulateGraphYscnYscp
#   EMSegmentCreateXLabels 
#   EMSegmentCreateYLabels 
#   EMSegmentRedrawGraph 
#   EMSegmentRescaleGraphMulti 
#   EMSegmentRescaleGraph
#   EMSegmentRescaleXAxis
#   EMSegmentCreateGraphPixels 
#   EMSegmentRescaleYAxis
#   EMSegmentExecuteCIM 
#   EMSegmentBrowseAndLoadCIMFile 
#   EMSegmentCreateCIMRowsColumns
#   EMSegmentSetCIMMatrix
#   EMSegmentChangeCIMMatrix
#   EMSegmentTrainCIMField
#   EMSegmentReadCIMFile 
#   EMSegmentReadMeanfromFile
#   EMSegmentReadCovariancefromFile
#   EMSegmentReadToNextLine
#   EMSegmentDisplayHorizontal 
#   EMSegmentEndSlice 
#   EMSegmentLoadVolume 
#   EMSegmentCalculateMoment
#   EMSegmentAssignInput
#   EMSegmentSelectfromVolumeList
#   EMSegmentChangeVolumeSegmented 
#   EMSegmentTransfereVolume
#   EMSegmentUpdateVolumeList
#   EMSegmentDeleteFromSelList
#   EMSegmentCreateMeanCovarianceRowsColumns 
#   EMSegmentCreateLogGrayValueDisplay
#   EMSegmentCreateGraph
#   EMSegmentCreateHistogramButton
#   EMSegmentCalcGaussian2DExtrem
#   EMSegmentDebug
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
# 
#  int EMSegment(count) counts the button presses for the demo 
#  list EMSegment(eventManager)  list of event bindings used by this module
#  widget EMSegment(textBox)  the text box widget
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC EMSegmentInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentInit {} {
    global EMSegment Module Volume Model Mrml Color Slice Gui env
    # For later version where we can use local prios
    # Normal     = 0
    # LocalPrior = 1
    # MultiDim   = 2
    if {[info exists env(SLICER_HOME)] == 0 || $env(SLICER_HOME) == ""} {
      set EMSegment(SegmentMode) 0
    } else {
    if {[file exist [file join $env(SLICER_HOME) Modules/EMLocalSegment]]} {
        set EMSegment(SegmentMode) 2
        package require vtkSlicerEMLocalSegment
    } else {
        set EMSegment(SegmentMode) 0
    }
    } 
    # EMSegment(SegmentMode) == 0 <=> Set all Probabilty maps to none, EMSegment(SegmentMode) == 1
    # Soucre EMSegmentAlgorithm.tcl File in sudirectory
    set found [FindNames tcl-modules/EMSegment]
    if {[lsearch $found EMSegmentAlgorithm] > -1} {
    set AlgoFile [GetFullPath EMSegmentAlgorithm tcl tcl-modules/EMSegment]
    source $AlgoFile
    }

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
    set m EMSegment
    set Module($m,row1List) "Help EM Class CIM Setting"
    set Module($m,row1Name) "{Help} {EM} {Class} {CIM} {Setting}"
    set Module($m,row1,tab) EM
    set Module($m,overview) "Automatic segmentation based on the EM Algorithm"
    set Module($m,author)   "Kilian Pohl, MIT AI Lab"

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
    #   set Module($m,procVTK) EMSegmentBuildVTK
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
    set Module($m,procGUI)   EMSegmentBuildGUI
    set Module($m,procEnter) EMSegmentEnter
    set Module($m,procExit)  EMSegmentExit
    set Module($m,procMRML)  EMSegmentUpdateMRML

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
        {$Revision: 1.13 $} {$Date: 2002/07/03 18:10:37 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #

    set EMSegment(ProbVolumeSelect) $Volume(idNone)
    set EMSegment(FileCIM) ""

    # MRML Variables
    set EMSegment(SegmenterNode) ""
    set EMSegment(EndSegmenterNode) ""
    set EMSegment(SegmenterGraphNodeList) ""
    set EMSegment(SegmenterInputNodeList) ""
    set EMSegment(SegmenterClassNodeList) ""
    set EMSegment(SegmenterCIMNodeList) ""

    set EMSegment(Debug) 0

    # This is important for multiple Input images
    set EMSegment(AllVolList,ActiveID) -1
    set EMSegment(SelVolList,ActiveID) -1
    set EMSegment(AllVolList,VolumeList) {}
    set EMSegment(SelVolList,VolumeList) {}

    vtkImageEMSegmenter vtkEMInit
    # Default Paramter from vtkEMSegm
    # set EMSegment(EMiteration)  1  
    # set EMSegment(MFAiteration) 1 
    # set EMSegment(Alpha)       0.7
    # set EMSegment(SmWidth)    11 
    # set EMSegment(SmSigma)    5
    # set EMSegment(StartSlice) 1    
    # set EMSegment(EndSlice)   1

    # Currently there is a default setting in vtkImageEMEMSegment. If you want to ovewrwrite it
    # just comment out the part where the program loads the variavle from vtk, e.g.
    # replace 'set EMSegment(EMiteration)  [vtkEMInit GetNumIter]
    # with    'set EMSegment(EMiteration) 10

    set EMSegment(NumClasses)   4
    set EMSegment(NumClassesNew) -1 
    # The next variable is needed so variables are independent from input
    set EMSegment(MaxInputChannelDef) 0
    # Current Number of Input Channels
    set EMSegment(NumInputChannel) 0

    set EMSegment(EMiteration)  [vtkEMInit GetNumIter]
    set EMSegment(MFAiteration) [vtkEMInit GetNumRegIter]
    set EMSegment(Alpha)        [vtkEMInit GetAlpha]
    set EMSegment(SmWidth)      [vtkEMInit GetSmoothingWidth]
    set EMSegment(SmSigma)      [vtkEMInit GetSmoothingSigma]
    set EMSegment(StartSlice)   [vtkEMInit GetStartSlice]
    set EMSegment(EndSlice)    -1 
    set EMSegment(ImgTestNo)        [vtkEMInit GetImgTestNo]
    set EMSegment(ImgTestDivision)  [vtkEMInit GetImgTestDivision]
    set EMSegment(ImgTestPixel)     [vtkEMInit GetImgTestPixel]

    set EMSegment(PrintIntermediateResults)     [vtkEMInit GetPrintIntermediateResults] 
    set EMSegment(PrintIntermediateSlice)       [vtkEMInit GetPrintIntermediateSlice] 
    set EMSegment(PrintIntermediateFrequency)   [vtkEMInit GetPrintIntermediateFrequency] 
    vtkEMInit Delete

    # Special EMSegment(SegmentMode) == 1 Variables
    # How did I come up with the number 82 ? It is a long story ....
    set EMSegment(NumberOfTrainingSamples) 82

    # Class 1 is default class
    set EMSegment(Class) 1 

    # Should Samples be used for calculating Mean and Variance or not (not = 0 / yes = 1)
    set EMSegment(UseSamples) 1

    # Needed for intensity correction 
    # IntensityAvgClass <0 => intensity correction is enabled
    set EMSegment(IntensityAvgClass) -1

    # Sequence of how CIM values are represented
    # Where                      pixel to the North  
    #                                    |
    #              pixel to the West - pixel - pixel to the East
    #                                    |
    #                            pixel to the South
    # Pixel Up   = Pixel at same position from next slice
    # Pixel Down = Pixel at same position from previous slice
    set EMSegment(CIMList) {West North Up East South Down}

    # This is not a good way of doing it but Color($i,node) is not set up when runnning through this
    # Originally :
    # set EMSegment(ColorLabelList) {}
    #for set i 1 $i < 10 incr i 
    #    lappend EMSegment(ColorLabelList) [MakeColorNormalized [Color($i,node) GetDiffuseColor]]
    #    lappend EMSegment(ColorLabelList) [lindex [Color(1,node) GetLabels] 0]
    # It is always <color> <corresponding label> 
    set EMSegment(ColorLabelList) [list #ffccb2 2 #ffffff 3 #66b2ff 4 #e68080 5 #80e680 6 #80e6e6 7 #e6e680 8 #e6b2e6 9 #e6e680 10]

    # EMSegment(MenuColor,$Color) :
    # -2  = Has not been displayed in list or assigned in class
    # -1  = Has been assigned to class
    # >-1 = Index of Color in list 
    # foreach Color $EMSegment(ColorList) {
    #    set EMSegment(MenuColor,$Color) -2
    # }

    # There are three NumGraph modes defined
    # 1 = display one 1-D Graph
    # 2 = display two 1-D Graph
    # 3 = display two 1-D Graph and one 2-D Graph (in the middle) => 3rd graph is 2D Graph 
    if {$EMSegment(SegmentMode) > 1} {
      set EMSegment(NumGraph) 3
    } else {
      set EMSegment(NumGraph) 1
    }
    EMSegmentCreateDeleteClasses 
    # Define for Class 0 for Histogram
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    set EMSegment(Cattrib,0,Graph,$i) 0
    # Represents Volume ID choosen for paramters
    set EMSegment(Cattrib,0,Label,$i) ""
    set EMSegment(Cattrib,0,ColorCode) #ffff00
    # This is the VolumeID of the volume last clalculated historgram
    set EMSegment(Graph,0,VolumeID,$i) -1  
    set EMSegment(Graph,0,XMin,$i)     -1
    set EMSegment(Graph,0,XMax,$i)     -1
    set EMSegment(Graph,0,YMin,$i)     -1
    set EMSegment(Graph,0,YMax,$i)     -1    
    }
    # Define Graph for "Class" panel
    # Define Border between canvas and coord system on the rigth and left /up and down
    set EMSegment(Graph,XboL) 25
    set EMSegment(Graph,XboR) 5
    set EMSegment(Graph,YboU) 5
    set EMSegment(Graph,YboD) 12
    # Length / 2 of scalling lines 
    set EMSegment(Graph,Xscl) 2
    set EMSegment(Graph,Yscl) 2
    # Difference bettween Text and Scalling line on Axis 
    set EMSegment(Graph,Xsct) 5
    set EMSegment(Graph,Ysct) 12
    # Should Probablilites be displayed in graph or not
    set EMSegment(Graph,DisplayProb) 0
    set EMSegment(Graph,DisplayProbNew) $EMSegment(Graph,DisplayProb)

    if {$EMSegment(NumGraph) < 3} {
    # I had there 135
    set SingleGraphLength [expr int(140.0/$EMSegment(NumGraph))] 
    } else {
    set SingleGraphLength [expr int(140.0/(($EMSegment(NumGraph)-1)*2.0))] 
    }
    # Define Number Scalling in value difference
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    # Define length of Canvas system
    set EMSegment(Graph,Cxle,$i) 200
    set EMSegment(Graph,Cyle,$i) $SingleGraphLength 
    if {$i>1} {
       incr EMSegment(Graph,Cyle,$i) $EMSegment(Graph,Cyle,$i) 
    } 
    # Define coord system len in pixels   
    set EMSegment(Graph,Xlen,$i) [expr $EMSegment(Graph,Cxle,$i) - $EMSegment(Graph,XboL) - $EMSegment(Graph,XboR)]
    set EMSegment(Graph,Ylen,$i) [expr $EMSegment(Graph,Cyle,$i) - $EMSegment(Graph,YboU) - $EMSegment(Graph,YboD)]

    set EMSegment(Graph,Xsca,$i) 50
    set EMSegment(Graph,XscaNew,$i) $EMSegment(Graph,Xsca,$i)
    # Min - Max values
    set EMSegment(Graph,Xmin,$i) 10
    set EMSegment(Graph,XminNew,$i) $EMSegment(Graph,Xmin,$i) 
    set EMSegment(Graph,Xmax,$i) 255
    set EMSegment(Graph,XmaxNew,$i) $EMSegment(Graph,Xmax,$i)
    if {$i < 2} {
        set EMSegment(Graph,Ysca,$i) 0.5
        set EMSegment(Graph,Ymin,$i) 0.0
        set EMSegment(Graph,Ymax,$i) 1.0
    } else {
        set EMSegment(Graph,Ysca,$i) $EMSegment(Graph,Xsca,$i)
        set EMSegment(Graph,Ymin,$i) $EMSegment(Graph,Xmin,$i)
        set EMSegment(Graph,Ymax,$i) $EMSegment(Graph,Xmax,$i)
        # The Minimum Display value that should be displayed in the graph
        # Leave it away right now set EMSegment(Graph,MinDisplayValue,$i) 0.000000001  
    }
    # calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn,$index), EMSegment(Graph,Xscp,$index) and EMSegment(Graph,Xpos)
    EMSegmentCalulateGraphXleUXscnXscpXpos $i
    # calulate EMSegment(Graph,Yscn) and EMSegment(Graph,Yscp) 
    EMSegmentCalulateGraphYscnYscp $i
    }

    # Event bindings! (see EMSegmentEnter, EMSegmentExit, tcl-shared/Events.tcl)
    set EMSegment(eventManager) {}
    # Not necessarily the most beautiful method but it works for right now
    foreach s $Slice(idList) {
        set widget $Gui(fSl${s}Win)
        append EMSegment(eventManager)  " \
                {$widget  <Control-Button-1> {EMSegmentBindingCallback Sample  %x %y}}\
                {$widget  <Motion>           {MainInteractorMotion %W %x %y;EMSegmentBindingCallback Display %x %y}} \
                {$widget  <Leave>            {MainInteractorExit %W;EMSegmentBindingCallback Leave   %x %y}} \
                {$widget  <Enter>            {MainInteractorEnter %W %x %y;EMSegmentBindingCallback Enter %x %y}} "
    }
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
# .PROC EMSegmentBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBuildGUI {} {
    global Gui EMSegment Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(EMSegment,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # EM
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
    set help "The EMSegment module is a realization of the EM-MRF EMSegment algortihm defined in
    Tina Kapur's PhD Thesis. Her thesis can be found on the web :<BR> 
    http://www.ai.mit.edu/people/tkapur/publications.html<P>
Description of the tabs:
    <UL>
    <LI><B>EM:</B> Beginner users should just use this panel. It guides one through the steps of segmentation.
    <LI><B>Class:</B> This tab allowes one to define the class distribition. Their are two ways how to define a class distribution:
    <BR> - \"Use Sample\" is raised: the data can be entered manually on the right side of the panel.
    <BR> - \"Use Sample\" is lowered: the class distribution is defined by taking samples from the lower image panel. Just press CTRL-Left Mouse Button to take a sample.
    <BR> The differnet distributions and the histogram of the image can be viewd in the graph.
    <LI><B>CIM:</B> The Class interaction matrix is defined in this tab. The matrix's row represents the neighbour and the colume defines the current voxel, i.e P(voxel) = CIM * P(neighbour) where P defines the probability of the different tissue classes
    <LI><B>Setting:</B>One can tune the algorithm and define paramters about the graph can be done her."
    regsub -all "\n" $help {} help
    MainHelpApplyTags EMSegment $help
    MainHelpBuildGUI EMSegment
    
    #-------------------------------------------
    # EM frame
    #-------------------------------------------
    set fEM $Module(EMSegment,fEM)
    set f $fEM
    
    for {set i 1} {$i < 4} {incr i} {
        frame $f.fStep$i -bg $Gui(activeWorkspace)  -relief groove -bd 3
        pack $f.fStep$i -side top -padx 0 -pady 2 -fill x
    }
    frame $f.fStep4 -bg $Gui(activeWorkspace)
    pack $f.fStep4 -side top -padx 0 -pady 2
    
    #-------------------------------------------
    # EM->Step1 frame: Volume Selection
    #-------------------------------------------
    set f $fEM.fStep1
    if {$EMSegment(SegmentMode) < 2} {
       DevAddLabel $f.l "Step 1: Select Volume to segment"
    } else {
       DevAddLabel $f.l "Step 1: Select Volumes to semgent"
    }
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)
    
    frame $f.fSelection -bg $Gui(activeWorkspace) 
    pack $f.fSelection -side top -padx $Gui(pad) 

    if {$EMSegment(SegmentMode) < 2} {
    set EMSegment(EM-mbVolumeSelect) $f.fSelection.mbVolumeSelect
    set EMSegment(EM-mVolumeSelect) $f.fSelection.mbVolumeSelect.m 
    if {$EMSegment(NumInputChannel) > 0} {
        set MenuText [Volume([lindex $EMSegment(SelVolList,VolumeList) 0],node) GetName]
    } else {
        set MenuText None
    }

    eval {menubutton $EMSegment(EM-mbVolumeSelect) -text $MenuText  -menu $EMSegment(EM-mVolumeSelect) -width 13} $Gui(WMBA) 
    pack $EMSegment(EM-mbVolumeSelect) -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $EMSegment(EM-mbVolumeSelect) "Select volume to be segmented"

    # Define Menu selection 
    eval {menu $EMSegment(EM-mVolumeSelect)} $Gui(WMA)
    # Add Selection entry
    set index 0
    foreach v $EMSegment(SelVolList,VolumeList)  {
        set VolName [Volume($v,node) GetName]
        $EMSegment(EM-mVolumeSelect) add command -label $VolName -command "EMSegmentChangeVolumeSegmented $index"
        incr index
    }
    # Append menus and buttons to lists that get refreshed during UpdateMRML
    # lappend Volume(mbActiveList) $f.fSelection.mbVolumeSelect
    # lappend Volume(mActiveList) $f.fSelection.mbVolumeSelect.m
    } else {
    EMSegmentAssignInput $f.fSelection
    }

   #-------------------------------------------
    # EM->Step2 frame: Set EM Parameters
    #-------------------------------------------
    set f $fEM.fStep2
    DevAddLabel $f.l "Step 2: Define Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)
    frame $f.fCol1 -bg $Gui(activeWorkspace)
    frame $f.fCol2 -bg $Gui(activeWorkspace)
    frame $f.fCol3 -bg $Gui(activeWorkspace)
    frame $f.fCol4 -bg $Gui(activeWorkspace)
    pack $f.fCol1 $f.fCol2 $f.fCol3 $f.fCol4 -side left -padx 0 -pady 2 -fill x 

    DevAddLabel $f.fCol1.lNumClasses "Classes:"  
    eval {entry $f.fCol2.eNumClasses -width 4 -textvariable EMSegment(NumClassesNew) } $Gui(WEA)
    TooltipAdd $f.fCol2.eNumClasses "Define Number of Classes"
    bind $f.fCol2.eNumClasses <Return> EMSegmentCreateDeleteClasses
    bind $f.fCol2.eNumClasses <Tab>    EMSegmentCreateDeleteClasses
    bind $f.fCol2.eNumClasses <Leave>  EMSegmentCreateDeleteClasses

    DevAddLabel $f.fCol3.lEMI "Iterations:"
    eval {entry $f.fCol4.eEMI -width 4 -textvariable EMSegment(EMiteration) } $Gui(WEA)
    TooltipAdd $f.fCol4.eEMI "Number of EM Iterations"

    DevAddLabel $f.fCol1.lStartSlice "Start Slice:"
    eval {entry $f.fCol2.eStartSlice -width 4 -textvariable EMSegment(StartSlice) } $Gui(WEA)
    TooltipAdd $f.fCol2.eStartSlice "Slice Number to start EM-EMSegment"

    DevAddLabel $f.fCol3.lEndSlice "End Slice:"
    eval {entry $f.fCol4.eEndSlice -width 4 -textvariable EMSegment(EndSlice) } $Gui(WEA)
    TooltipAdd $f.fCol4.eEndSlice "Slice number to end EM-EMSegment"

    pack  $f.fCol1.lNumClasses $f.fCol3.lEMI      -side top -padx $Gui(pad) -pady 2 -anchor w  
    pack  $f.fCol1.lStartSlice $f.fCol3.lEndSlice -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack  $f.fCol2.eNumClasses $f.fCol4.eEMI      -side top -padx $Gui(pad) -pady 1 -anchor w  
    pack  $f.fCol2.eStartSlice $f.fCol4.eEndSlice -side top -padx $Gui(pad) -pady 1 -anchor w 

    #-------------------------------------------
    # EM->Step3 frame : Select and Define Classes
    #-------------------------------------------
    set f $fEM.fStep3
    DevAddLabel $f.lHead "Step 3: Define Tissue Classes"
    pack $f.lHead -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    # Make a box around class 3 
    frame $f.fClass  -bg $Gui(activeWorkspace)
    frame $f.fDefine -bg $Gui(activeWorkspace) -relief sunken -bd 2 
    frame $f.fProb   -bg $Gui(activeWorkspace) -relief sunken -bd 2 

    pack $f.fClass -side top -padx $Gui(pad) 
    pack $f.fDefine $f.fProb -side top -padx 2 -pady 2 -fill x


    # Classes Box 
    # General Information on how to create menubtton
    # Gui = defines general setting for buttons, menues, ..., eg. font size ...
    # Through the eval command a list is created with {..} $Gui(..) 
    # and afterwards executed   
    set menu $f.fClass.mbClasses.m 
    set Sclass $EMSegment(Class) 

    DevAddLabel $f.fClass.lText "Class:"
    pack $f.fClass.lText -side left -padx $Gui(pad) -pady 0     
    #Define Menu button
    eval {menubutton $f.fClass.mbClasses -text $EMSegment(Cattrib,$Sclass,Label) -menu $menu -width 5} $Gui(WMBA)
    $f.fClass.mbClasses configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    set EMSegment(EM-mbClasses) $f.fClass.mbClasses
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    # Add Selection entry
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i 1} {
        # change Menu Button when selected
    $menu add command -label "$EMSegment(Cattrib,$i,Label)" -command "EMSegmentChangeClass $i" \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 
    pack $f.fClass.mbClasses -side left -padx 0 -pady 0     

    frame $f.fDefine.fSample -bg $Gui(activeWorkspace)
    pack $f.fDefine.fSample -side top -padx 0 -pady 0 -fill x

    eval {checkbutton $f.fDefine.fSample.cUseSample -text "Use Sample" -variable EMSegment(UseSamples) -indicatoron 1 -command "EMSegmentUseSamples 1"} $Gui(WCA)
    set EMSegment(Cl-cUseSample) $f.fDefine.fSample.cUseSample
    TooltipAdd $EMSegment(Cl-cUseSample) "Press button to to use samples for the calucation of Mean and Variance"
    pack $f.fDefine.fSample.cUseSample -side left -padx $Gui(pad) -pady 0    

    DevAddLabel $f.fDefine.fSample.ltxt "Samples taken:"
    if {[llength $EMSegment(SelVolList,VolumeList)]  } {
        DevAddLabel $f.fDefine.fSample.lvar [llength $EMSegment(Cattrib,$Sclass,[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)]  
    } else {
        DevAddLabel $f.fDefine.fSample.lvar 0 
    }
    set EMSegment(EM-lSampvar) $f.fDefine.fSample.lvar
    pack $f.fDefine.fSample.lvar -side right -padx 2 -pady 0    
    pack $f.fDefine.fSample.ltxt -side right -padx 0 -pady 0    

    frame $f.fDefine.fMean -bg $Gui(activeWorkspace)
    pack $f.fDefine.fMean -side top -padx 0 -pady 0 -fill x
    DevAddLabel $f.fDefine.fMean.ltxt "Class Mean:"
    frame $f.fDefine.fMean.fvar -bg $Gui(activeWorkspace)
    set EMSegment(EM-fMeanVar) $f.fDefine.fMean.fvar
    pack  $f.fDefine.fMean.ltxt $f.fDefine.fMean.fvar -side left -padx $Gui(pad) -pady 0    
    # The text in fram fvar will be added at a later point in time 

    DevAddLabel $f.fDefine.lexpltxt "Use Ctrl-left mouse but. to sample"
    set EMSegment(EM-lexpltxt) $f.fDefine.lexpltxt
    pack $f.fDefine.lexpltxt -side top -padx $Gui(pad) -pady 1

    EMSegmentProbWindow $f.fProb EM $Sclass

    #-------------------------------------------
    # EM->Step4 Frame: Run Algorithm
    #-------------------------------------------
    set f $fEM.fStep4
        
    if {$EMSegment(Debug) > 0} {
       DevAddButton $f.bDebug Debug EMSegmentDebug 
       pack $f.bDebug -side left -padx $Gui(pad) -pady 0
    }
    # Define Button to start algorithm
    DevAddButton $f.bStart Segment EMSegmentStartEM 
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bStart "Press this button to start EM-MRF Segmentation."
    pack $f.bStart -side left -padx $Gui(pad) -pady 0

    #--------------------------------------------
    # Class Frame
    #-------------------------------------------
 
    set fCl $Module(EMSegment,fClass)
    set f $fCl
    
    for {set i 1} {$i < 3} {incr i} {
        frame $f.fSec$i -bg $Gui(activeWorkspace)
        pack $f.fSec$i -side top -padx 0 -pady 1 -fill x
    }

    frame $f.fSec3 -bg $Gui(activeWorkspace)
    frame $f.fSec4 -bg $Gui(activeWorkspace)

    # Do not Display Graph if you use log values
    pack $f.fSec3 -side top -padx 0 -pady 1 -fill x
    pack $f.fSec4 -side top -padx 0 -pady 1 -fill x

    #--------------------------------------------
    # Class->Section 1 Frame: Sample Display Class values
    #-------------------------------------------

    set f $fCl.fSec1

    #Define Menu button
    set menu $f.mbClasses.m 
    eval {menubutton $f.mbClasses -text "$EMSegment(Cattrib,$Sclass,Label)" -menu $menu -width 10} $Gui(WMBA)
    
    $f.mbClasses configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)

    pack $f.mbClasses -side top -padx $Gui(pad) -pady 2     
    set EMSegment(Cl-mbClasses) $f.mbClasses
 
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    
    # Add Selection entry
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i 1} {
        # change Menu Button when selected
    $menu add command -label "$EMSegment(Cattrib,$i,Label)" -command  "EMSegmentChangeClass $i" \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 
    
    #--------------------------------------------
    # Class->Section 2
    #-------------------------------------------
    set f $fCl.fSec2
    $f config -relief groove -bd 3    

    #--------------------------------------------
    # Class->Section 2->body Frame
    #-------------------------------------------
    frame $f.fbody -bg $Gui(activeWorkspace) -relief sunken -bd 2 
    pack $f.fbody -side top -pady 2 -fill x 
    frame $f.frest -bg $Gui(activeWorkspace) -relief sunken -bd 2 
    pack $f.frest -side top -pady 2 -fill x 

    #--------------------------------------------
    # Class->Section 2->body->right Frame
    #-------------------------------------------
    set f $f.fbody
    for {set i 1} {$i < 3} {incr i} {
    frame $f.f$i -bg $Gui(activeWorkspace)
    pack $f.f$i  -side top -padx 2 -pady 2  -fill x
    }

    #-------------------------------------------
    # Class->Section 2->Use Sample frame 
    #------------------------------------------- 
    eval {checkbutton $f.f1.cSample -text "Use Sample" -variable EMSegment(UseSamples) -indicatoron 1 -command "EMSegmentUseSamples 1"} $Gui(WCA)
    # The old usesample button we could have had a lot easier by using checkebutton and -indicatoron 0       
    # eval {button $f.f1.bSample -text "Use Samples" -width 13 -command "EMSegmentUseSamples 1"} $Gui(WBA)
    set EMSegment(Cl-cSample) $f.f1.cSample
    pack  $f.f1.cSample -side left -padx $Gui(pad) -pady 1

    set menu $f.f1.mbEraseSample.m 
    #Define Menu button
    eval {menubutton $f.f1.mbEraseSample -text "Erase Samples" -menu $menu -width 15} $Gui(WMBA)
    pack $f.f1.mbEraseSample -side right -padx 0 -pady 0   
    TooltipAdd $f.f1.mbEraseSample "Erase a sample from the sample list "

    set EMSegment(Cl-mbEraseSample) $f.f1.mbEraseSample
    set EMSegment(Cl-mEraseSample) $f.f1.mbEraseSample.m

    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    # Add Selection entry
    $menu add command -label " x     y     z"
    if {[llength $EMSegment(SelVolList,VolumeList)] > 0 } {
    set v [lindex $EMSegment(SelVolList,VolumeList) 0] 
    set index 1
    foreach line $EMSegment(Cattrib,$Sclass,$v,Sample) {
            $menu add command -label "[format "%3d %3d %3d" [lindex $line 0] [lindex $line 1] [lindex $line 2]]" -command "EMSegmentEraseSample $index"
            incr index
    }
    } 

    #DevAddLabel $fl.lCoord "    X      Y     Brig"
    #pack  $fl.lCoord -side top -padx $Gui(pad) -anchor w
    # EMSegment(textBox) links to path $f.tText.text 
    # where the text is actually displayed

    #set EMSegment(Cl-textBox) [EMSegmentScrolledText $fl.tText]
    #pack $fl.tText -side top  -padx $Gui(pad) -expand true
    # Write values to Box
    #EMSegmentWriteTextBox

    frame $f.f2.fleft -bg $Gui(activeWorkspace)
    pack  $f.f2.fleft -side left -fill y
    frame $f.f2.fright -bg $Gui(activeWorkspace)
    pack  $f.f2.fright -side left 

    DevAddLabel $f.f2.fleft.lMeantxt "Mean:"
    DevAddLabel $f.f2.fleft.lCovtxt  "Covariance:"
    pack $f.f2.fleft.lMeantxt $f.f2.fleft.lCovtxt -side top -pady 5 -padx $Gui(pad) -anchor nw   

    frame $f.f2.fright.fMeanVar -bg $Gui(activeWorkspace)
    set EMSegment(Cl-fLogMeanVar) $f.f2.fright.fMeanVar

    frame $f.f2.fright.fCovVar -bg $Gui(activeWorkspace)
    set EMSegment(Cl-fLogCovVar) $f.f2.fright.fCovVar 
    pack $f.f2.fright.fMeanVar $f.f2.fright.fCovVar -pady 2 -side top -anchor nw
 
    EMSegmentCreateMeanCovarianceRowsColumns 0  $EMSegment(NumInputChannel)

    #--------------------------------------------
    # Class->Section 2->body->rest Frame
    #-------------------------------------------
    set f $fCl.fSec2.frest

    frame $f.fProb -bg $Gui(activeWorkspace)
    pack $f.fProb -side left -padx 2 -pady 2
    frame $f.fColor -bg $Gui(activeWorkspace)
    pack $f.fColor -side right -padx 2 -pady 2

    EMSegmentProbWindow $f.fProb Cl $Sclass

    #Define Color
    DevAddLabel $f.lColorLabel "Color/Label:"
    eval {button $f.bColorLabel -text "$EMSegment(Cattrib,$Sclass,Label)" -command "ShowLabels EMSegmentClickLabel" -width 7} $Gui(WBA)
    $f.bColorLabel configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    TooltipAdd $f.bColorLabel "Choose label value for class."
    set  EMSegment(Cl-bColorLabel) $f.bColorLabel
    pack $f.lColorLabel -side left
    pack $f.bColorLabel -side left -padx 1

    #-------------------------------------------
    # Class->Section 3: Display Graph   
    #-------------------------------------------
    set f $fCl.fSec3
    $f config -relief groove -bd 3  
        # Button Frame 
    frame $f.fGraphButtons -bg $Gui(activeWorkspace)
    pack $f.fGraphButtons -side top -padx $Gui(pad) -pady 1 -fill x
    set EMSegment(Cl-fGraphButtons) $f.fGraphButtons
    # Create Button for Histogram
    # First Button is selecting the Volume for the Histogram 
    #Define Menu button
    EMSegmentCreateHistogramButton $f.fGraphButtons 0

    if {$EMSegment(NumGraph) < 2} {
    set GraphList {0}
    } elseif {$EMSegment(NumGraph) < 3} {
    set GraphList {0 1}
    } else {
    set GraphList {0 2 1}
    } 

    foreach i $GraphList {
    frame $f.fgraph$i -bg $Gui(activeWorkspace) 
    pack $f.fgraph$i -side top -padx 0 -pady 0 -fill x
    EMSegmentCreateGraph $f.fgraph$i $i
    }

    frame $f.fGraphButtonsBelow -bg $Gui(activeWorkspace)
    pack $f.fGraphButtonsBelow -side top -padx $Gui(pad) -pady 1 -fill x
    set EMSegment(Cl-fGraphButtonsBelow) $f.fGraphButtonsBelow

    if {$EMSegment(NumGraph) > 1} {EMSegmentCreateHistogramButton $f.fGraphButtonsBelow 1}

    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
    EMSegmentCreateGraphButton $i $EMSegment(Cattrib,$i,Label) $EMSegment(Cattrib,$i,ColorCode)
    }

    #-------------------------------------------
    # Class->Section 4->bUpdate : Update Button  
    #-------------------------------------------
    set f $fCl.fSec4
    DevAddButton $f.bUpdate Update EMSegmentUpdateClassButton
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bUpdate "Press this button to update class values and graph."
    pack $f.bUpdate -side bottom -padx $Gui(pad)  -pady 1

    #-------------------------------------------
    # CIM frame:
    #-------------------------------------------
    set fMa $Module(EMSegment,fCIM)
    set f $fMa
    set CIMMenu {Train Edit}
    set CIMMenuLabel {"     Train    "   "    Edit    "}
    

    #-------------------------------------------
    # CIM->Sec1 Frame: Menu Selection      
    #-------------------------------------------
    TabbedFrame EMSegment $f "" $CIMMenu $CIMMenuLabel \
          {"Define a new Class Interaction Matrix by training at already images"\
           "Edit Matrix defining Class Interaction"}\
           0 Edit

    foreach i $CIMMenu {
    $f.fTop.fTabbedFrameButtons.f.r$i configure -command "EMSegmentExecuteCIM $i"
    }
    set EMSegment(Ma-tabCIM) $f

    set f $f.fTabbedFrame

    set EMSegment(Ma-fCIM) $f

    #-------------------------------------------
    # CIM->Sec2 Frame: Import CIM Random Field 
    #-------------------------------------------
    # set f $EMSegment(Ma-fCIM).fImport

    # DevAddLabel $f.lLoadText "Selecting File defining Class Interaction Matrix"
    # pack $f.lLoadText -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    # set EMSegment(Ma-lLoadText) $f.lLoadText

    #-------------------------------------------
    # CIM->Sec2 Frame: Train CIM Random Field 
    #-------------------------------------------
    set f $EMSegment(Ma-fCIM).fTrain

    frame $f.fAction -bg $Gui(activeWorkspace) -relief groove -bd 3  
    pack $f.fAction -side top -padx $Gui(pad)  -pady $Gui(pad) -fill x

    DevAddLabel $f.fAction.lTitle "Training Class Interaction Matrix"
    pack $f.fAction.lTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w

    DevAddLabel $f.fAction.lStep1 "Step 1: Select Training Volume"
    pack $f.fAction.lStep1 -side top -padx $Gui(pad) -pady 1 -anchor w
    frame $f.fAction.fSelection -bg $Gui(activeWorkspace) 
    pack $f.fAction.fSelection -side top -padx $Gui(pad) -pady $Gui(pad)

    DevAddSelectButton  EMSegment $f.fAction.fSelection VolumeSelect "" Pack "Volume to use for training"
    # Append menus and buttons to lists that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.fAction.fSelection.mbVolumeSelect
    lappend Volume(mActiveList) $f.fAction.fSelection.mbVolumeSelect.m

    DevAddLabel $f.fAction.lStep2 "Step 2: Define gray value distribution \nClick on the class panel and define \ndistribution for every class" 
    $f.fAction.lStep2 config -justify left 

    DevAddLabel $f.fAction.lStep3 "Step 3: Start training for MRF-Paramters"
    pack $f.fAction.lStep2 $f.fAction.lStep3 -side top -padx $Gui(pad) -pady 1 -anchor w
    DevAddButton $f.fAction.bStep3 "Run Algorithm" EMSegmentTrainCIMField 

    DevAddLabel $f.fAction.lCaution "Caution: Training with the Training Volume \n will not only change the Random Field \n Definition but also class probablity and \n class distribution!" 
    $f.fAction.lCaution configure -fg red
    
    pack $f.fAction.bStep3 $f.fAction.lCaution -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # CIM->Sec3 Frame: Edit CIM Random Field 
    #-------------------------------------------
    set f $EMSegment(Ma-fCIM).fEdit

    # frame $f.fSec4  -bg $Gui(backdrop) -relief sunken -bd 2
    # pack $f.fSec4 -side top -padx 0 -pady $Gui(pad) -fill x

    frame $f.fNeighbour -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fDefinition -bg $Gui(activeWorkspace)
    pack $f.fNeighbour $f.fDefinition -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    $f.fDefinition config -relief groove -bd 3 
 
    #-------------------------------------------
    # CIM->Sec3->Neighbour Frame: 
    # Select Neighbour to be edit 
    #-------------------------------------------
    eval {label $f.fNeighbour.lText -text "Neighbour:"} $Gui(BLA)
    frame $f.fNeighbour.fSelection -bg $Gui(backdrop) 
        # the first row and second row
    frame $f.fNeighbour.fSelection.f1 -bg $Gui(inactiveWorkspace)
    frame $f.fNeighbour.fSelection.f2 -bg $Gui(inactiveWorkspace)
    pack $f.fNeighbour.fSelection.f1 $f.fNeighbour.fSelection.f2 -side top -fill x -anchor w

    #
    # NOTE: As you want more functions, don't forget
    #       to add more rows above.
    #

    set row 1
    foreach p $EMSegment(CIMList) {
            eval {radiobutton $f.fNeighbour.fSelection.f$row.r$p \
            -text "$p" -command "EMSegmentChangeCIMMatrix $p" \
            -variable EMSegment(CIMType) -value $p -width 6 \
            -indicatoron 0} $Gui(WCA)
        pack $f.fNeighbour.fSelection.f$row.r$p -side left -pady 0
            if { $p == "Up" } {incr row};
    }

    pack $f.fNeighbour.lText $f.fNeighbour.fSelection -side left -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w

    set EMSegment(CIMType) [lindex $EMSegment(CIMList) 0]

    #-------------------------------------------
    # CIM->Sec3->Definition Frame: Define CIM Field
    #-------------------------------------------
    set f $f.fDefinition
    set EMSegment(CIM-fDefinition)  $f 
 
    DevAddLabel $f.lHead "Class Interaction Matrix"
    pack  $f.lHead -side top -padx $Gui(pad) -pady $Gui(pad)

    frame $f.fMatrix -bg $Gui(activeWorkspace)
    set f $f.fMatrix
    pack $f -expand true -side top -fill both
    canvas $f.cMatrix -xscrollcommand "$f.fScroll set" -width 200  -height 100 -scrollregion "0 0 300 1"  -bg $Gui(activeWorkspace) -highlightthickness 0
    eval { scrollbar $f.fScroll -orient horizontal -command "$f.cMatrix xview" } $Gui(WSBA)

    grid $f.cMatrix -sticky news
    grid $f.fScroll -sticky news
    lower $f.fScroll

    frame $f.cMatrix.fMatrix -bd 0 -bg $Gui(activeWorkspace)
    # put the frame inside the canvas (so it can scroll)
    $f.cMatrix create window 0 0 -anchor nw -window $f.cMatrix.fMatrix
    set EMSegment(CIM-fMatrix)  $f.cMatrix.fMatrix 
    EMSegmentCreateCIMRowsColumns 1 $EMSegment(NumClasses)

    #-------------------------------------------
    # CIM->Sec4 Frame: Export CIM Random Field 
    #-------------------------------------------
    # set f $EMSegment(Ma-fCIM).fExport

    # DevAddLabel $f.lSaveText "Export Class Interaction Matrix to a file"
    # pack $f.lSaveText -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    # set EMSegment(Ma-lSaveText) $f.lSaveText

    #-------------------------------------------
    # Setting frame: Define Algorithem Paramter
    #-------------------------------------------
    set fSe $Module(EMSegment,fSetting)
    set f $fSe

    for {set i 1} {$i < 2} {incr i} {
        frame $f.fSect$i -bg $Gui(activeWorkspace)
        pack $f.fSect$i -side top -padx 0 -pady $Gui(pad) -fill x
    }

    for {set i 1} {$i < 3} {incr i} {
    frame $f.fSect1.fCol$i -bg $Gui(activeWorkspace)
    pack $f.fSect1.fCol$i -side left -padx 0 -pady $Gui(pad) 
    }
    
    DevAddLabel $f.fSect1.fCol1.lNumClasses "Classes:"  
    eval {entry $f.fSect1.fCol2.eNumClasses -width 4 -textvariable EMSegment(NumClassesNew) } $Gui(WEA)

    TooltipAdd $f.fSect1.fCol2.eNumClasses "Define Number of Classes"
    bind $f.fSect1.fCol2.eNumClasses <Return> EMSegmentCreateDeleteClasses
    bind $f.fSect1.fCol2.eNumClasses <Tab>    EMSegmentCreateDeleteClasses
    bind $f.fSect1.fCol2.eNumClasses <Leave>  EMSegmentCreateDeleteClasses

    DevAddLabel $f.fSect1.fCol1.lEMI "EM-Iterations:"
    eval {entry $f.fSect1.fCol2.eEMI -width 4 -textvariable EMSegment(EMiteration) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eEMI "Number of EM Iterations"

    DevAddLabel $f.fSect1.fCol1.lMRFI "MFA-Iterations:"
    eval {entry $f.fSect1.fCol2.eMRFI -width 4 -textvariable EMSegment(MFAiteration) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eMRFI "Number of Mean Field Approximation Iterations to regularize weights"

    DevAddLabel $f.fSect1.fCol1.lAlpha "Alpha:"
    eval {entry $f.fSect1.fCol2.eAlpha -width 4 -textvariable EMSegment(Alpha) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eAlpha "Alpha defines the influence of the Markov Field! \n Alpha = 0 => Normal EM EMSegment without Markov Field! Alpha has to be 0 <= alpha <= 1"

    DevAddLabel $f.fSect1.fCol1.lSmWidth "Smoothing Width:"
    eval {entry $f.fSect1.fCol2.eSmWidth -width 4 -textvariable EMSegment(SmWidth) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eSmWidth "Smoothing Width determines how much the EM Algorithm \n should smooth the Image after each iteration. "

    DevAddLabel $f.fSect1.fCol1.lSmSigma "Smoothing Sigma:"
    eval {entry $f.fSect1.fCol2.eSmSigma -width 4 -textvariable EMSegment(SmSigma) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eSmSigma "Smoothing Sigma defines the Sigma parameter for the Gausian smoothing curve."

    DevAddLabel $f.fSect1.fCol1.lCreateFile "Create File:"
    frame  $f.fSect1.fCol2.fCreateFile -bg $Gui(activeWorkspace)
    foreach value "1 0" text "On Off" width "4 4" {
    eval {radiobutton $f.fSect1.fCol2.fCreateFile.r$value -width $width -indicatoron 0\
        -text "$text" -value "$value" -variable EMSegment(PrintIntermediateResults) } $Gui(WCA)
    }
    TooltipAdd $f.fSect1.fCol2.fCreateFile "Create Matlab Files (EMSegmResult<iter>.m) for displaying intermediate results"

    DevAddLabel $f.fSect1.fCol1.lWriteSlice "Print Slice:"
    eval {entry $f.fSect1.fCol2.eWriteSlice -width 4 -textvariable EMSegment(PrintIntermediateSlice) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eWriteSlice "Print out the result of which slice ?"

    DevAddLabel $f.fSect1.fCol1.lWriteFrequency "Printing Frequency:"
    eval {entry $f.fSect1.fCol2.eWriteFrequency -width 4 -textvariable EMSegment(PrintIntermediateFrequency) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eWriteFrequency "Print out the result after how many steps ?"

    DevAddLabel $f.fSect1.fCol1.lStartSlice "Start Slice:"
    eval {entry $f.fSect1.fCol2.eStartSlice -width 4 -textvariable EMSegment(StartSlice) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eStartSlice "Slice Number to start EM-EMSegment"

    DevAddLabel $f.fSect1.fCol1.lEndSlice "End Slice:"
    frame $f.fSect1.fCol2.fEndSlice -bg $Gui(activeWorkspace)
    eval {entry $f.fSect1.fCol2.fEndSlice.e -width 4 -textvariable EMSegment(EndSlice) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.fEndSlice.e "Slice number to end EM-EMSegment"
    DevAddButton $f.fSect1.fCol2.fEndSlice.b Max EMSegmentEndSlice
    $f.fSect1.fCol2.fEndSlice.b configure -width 4 -command {EMSegmentEndSlice 1}
    TooltipAdd $f.fSect1.fCol2.fEndSlice.b "Setting End Slice to the last Slice of the current Volume"  

    DevAddLabel $f.fSect1.fCol1.lUseProb "Use Probability:"
    frame  $f.fSect1.fCol2.fUseProb -bg $Gui(activeWorkspace)
    foreach value "1 0" text "On Off" width "4 4" {
       eval {radiobutton $f.fSect1.fCol2.fUseProb.r$value -width $width -indicatoron 0\
               -text "$text" -value "$value" -variable EMSegment(Graph,DisplayProbNew) -command { \
               if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} { set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew); EMSegmentRedrawGraph } }   
            } $Gui(WCA)
    }
    TooltipAdd $f.fSect1.fCol2.fUseProb "Use class propability to caluclate curves in graph"
    # eval {checkbutton $f.fSect1.fCol2.cUseProb -variable EMSegment(Graph,DisplayProbNew) -bg $Gui(activeWorkspace) -activebackground $Gui(activeWorkspace) -highlightthickness 0}

    DevAddLabel $f.fSect1.fCol1.lTrSample "Training Samples:"
    eval {entry $f.fSect1.fCol2.eTrSample -width 4 -textvariable EMSegment(NumberOfTrainingSamples) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eTrSample "Number of Training Samples used for Probability Maps"

    set menu $f.fSect1.fCol2.mbIntensityClass.m
    DevAddLabel  $f.fSect1.fCol1.lIntensityClass "Intensity Class:"
    #Define Menu button
    set IntResult [EMSegmentFindClassAndTestfromIntClass $EMSegment(IntensityAvgClass)]
    set IntText [lindex $IntResult 1]
    set IntClass [lindex $IntResult 0]

    eval {menubutton $f.fSect1.fCol2.mbIntensityClass -text $IntText -menu $menu -width 5} $Gui(WMBA)
    if {$IntText != "None"} {
    $f.fSect1.fCol2.mbIntensityClass configure -bg $EMSegment(Cattrib,$IntClass,ColorCode) -activebackground $EMSegment(Cattrib,$IntClass,ColorCode)
    }
    set EMSegment(DE-mbIntClass)  $f.fSect1.fCol2.mbIntensityClass

    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    # Add Selection entry
    $menu add command -label "None" -command "EMSegmentChangeIntensityClass -1 1" 
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i 1} {
        # change Menu Button when selected
    $menu add command -label "$EMSegment(Cattrib,$i,Label)" -command "EMSegmentChangeIntensityClass $i " \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 

    DevAddButton $f.bSaveSetting "Save Setting" "EMSegmentSaveSetting 1"
    pack $f.bSaveSetting -side top -padx $Gui(pad) -pady 0

    # DevAddLabel $f.fSect1.fCol1.lTNo "Image Test No:"
    # eval {entry $f.fSect1.fCol2.eTNo -width 4 -textvariable EMSegment(ImgTestNo) } $Gui(WEA)
    # TooltipAdd $f.fSect1.fCol2.eTNo "Use Test picture for evaluation algorithms (-1 => No Test Picture)"

    # DevAddLabel $f.fSect1.fCol1.lTDiv "Image Test Division"
    # eval {entry $f.fSect1.fCol2.eTDiv -width 4 -textvariable EMSegment(ImgTestDivision) } $Gui(WEA)
    # TooltipAdd $f.fSect1.fCol2.eTDiv "Number of divison classes in the test image" 

    # DevAddLabel $f.fSect1.fCol1.lTPix "Image Test Pixel:"
    # eval {entry $f.fSect1.fCol2.eTPix -width 4 -textvariable EMSegment(ImgTestPixel) } $Gui(WEA)
    # TooltipAdd $f.fSect1.fCol2.eTPix "Pixel width/length of divison (-1 = max length/width for division)"

    DevAddLabel $f.fSect1.fCol1.lEmpty1 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty1 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty2 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty2 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty3 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty3 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty4 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty4 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty5 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty5 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty6 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty6 ""

    DevAddLabel $f.fSect1.fCol1.lEmpty7 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty7 ""
   
    # Pack 1 Block
    pack $f.fSect1.fCol1.lNumClasses -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eNumClasses -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty1 $f.fSect1.fCol2.lEmpty1 -side top -padx $Gui(pad) -pady 1 -anchor w  
    
    # Pack 2. Block
    pack $f.fSect1.fCol1.lEMI $f.fSect1.fCol1.lMRFI $f.fSect1.fCol1.lAlpha -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eEMI $f.fSect1.fCol2.eMRFI $f.fSect1.fCol2.eAlpha -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty2 $f.fSect1.fCol2.lEmpty2 -side top -padx $Gui(pad) -pady 1 -anchor w  

    #Pack 3. Block
    pack $f.fSect1.fCol1.lStartSlice $f.fSect1.fCol1.lEndSlice -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eStartSlice $f.fSect1.fCol2.fEndSlice -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty3 $f.fSect1.fCol2.lEmpty3 -side top -padx $Gui(pad) -pady 1 -anchor w 
    pack $f.fSect1.fCol2.fEndSlice.e $f.fSect1.fCol2.fEndSlice.b -side left

    #Pack 4.Block
    pack $f.fSect1.fCol1.lSmWidth $f.fSect1.fCol1.lSmSigma -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eSmWidth $f.fSect1.fCol2.eSmSigma -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty4 $f.fSect1.fCol2.lEmpty4 -side top -padx $Gui(pad) -pady 1 -anchor w 

    #Pack 5.Block
    pack $f.fSect1.fCol1.lCreateFile  $f.fSect1.fCol1.lWriteSlice $f.fSect1.fCol1.lWriteFrequency -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.fCreateFile  $f.fSect1.fCol2.eWriteSlice $f.fSect1.fCol2.eWriteFrequency -side top -anchor w
    pack $f.fSect1.fCol2.fCreateFile.r1  $f.fSect1.fCol2.fCreateFile.r0 -side left -fill x
    pack $f.fSect1.fCol1.lEmpty5 $f.fSect1.fCol2.lEmpty5 -side top -padx $Gui(pad) -pady 1 -anchor w 

    if {$EMSegment(NumGraph) > 1} {
    set Border 2
    } else {
    set Border 1 
    }
    for {set i 0} { $i < $Border} {incr i} { 
    DevAddLabel $f.fSect1.fCol1.lGraph$i "Graph [expr $i+1]"
    DevAddLabel $f.fSect1.fCol2.lGraph$i " "
    DevAddLabel $f.fSect1.fCol1.lXMin$i "X-Minium:"
    eval {entry $f.fSect1.fCol2.eXMin$i -width 4 -textvariable EMSegment(Graph,XminNew,$i) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXMin$i "Minimum X value represented in Graph [expr $i+1]" 
    bind $f.fSect1.fCol2.eXMin$i <Return> "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXMin$i <Tab>    "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXMin$i <Leave>  "EMSegmentRescaleGraphMulti $i" 

    DevAddLabel $f.fSect1.fCol1.lXMax$i "X-Maximum:"
    eval {entry $f.fSect1.fCol2.eXMax$i -width 4 -textvariable EMSegment(Graph,XmaxNew,$i) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXMax$i "Maximum X value represented in Graph [expr $i+1]" 
    bind $f.fSect1.fCol2.eXMax$i <Return> "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXMax$i <Tab>    "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXMax$i <Leave>  "EMSegmentRescaleGraphMulti $i" 

    DevAddLabel $f.fSect1.fCol1.lXSca$i "X-Scalling:"
    eval {entry $f.fSect1.fCol2.eXSca$i -width 4 -textvariable EMSegment(Graph,XscaNew,$i) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXSca$i "Value difference between two tabs on the X-axis" 
    bind $f.fSect1.fCol2.eXSca$i <Return> "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXSca$i <Tab>    "EMSegmentRescaleGraphMulti $i" 
    bind $f.fSect1.fCol2.eXSca$i <Leave>  "EMSegmentRescaleGraphMulti $i" 
    pack $f.fSect1.fCol1.lGraph$i $f.fSect1.fCol2.lGraph$i $f.fSect1.fCol1.lXMin$i $f.fSect1.fCol1.lXMax$i $f.fSect1.fCol1.lXSca$i -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eXMin$i $f.fSect1.fCol2.eXMax$i $f.fSect1.fCol2.eXSca$i -side top -anchor w
    }


    #Pack 6.Block
    pack $f.fSect1.fCol1.lUseProb  -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.fUseProb -side top -anchor w
    pack $f.fSect1.fCol2.fUseProb.r1  $f.fSect1.fCol2.fUseProb.r0 -side left -fill x
    if {$EMSegment(SegmentMode) > 0 } {    
      #Pack 7.Block
      pack $f.fSect1.fCol1.lEmpty6 $f.fSect1.fCol2.lEmpty6 -side top -padx $Gui(pad) -pady 1 -anchor w 
      pack $f.fSect1.fCol1.lTrSample -side top -padx $Gui(pad) -pady 2 -anchor w 
      pack $f.fSect1.fCol2.eTrSample -side top -anchor w 

      pack $f.fSect1.fCol1.lIntensityClass -side top -padx $Gui(pad) -pady 2 -anchor w 
      pack $f.fSect1.fCol2.mbIntensityClass -side top -anchor w
    }
    # Special Function Calls
    # Depending on the Situation we want to en- or disable certain fields
    EMSegmentUseSamples 0
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentProbColorLabelWindow
# Defines the panel with Probability, Color and Label Class Definition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentProbWindow {f Panel Sclass} {
    global EMSegment Gui
    set lProbText "Prob.:"
    if {$Panel == "EM"} {
    set Xoff 2
    set Yoff 1
    } else {
    set Xoff 0
    set Yoff 0
    }
    DevAddLabel $f.lProb $lProbText
    eval {entry $f.eProb -width 4 -textvariable EMSegment(Cattrib,$Sclass,Prob) } $Gui(WEA)
    set EMSegment(${Panel}-eProb) $f.eProb
    pack $f.lProb -side left -padx $Xoff -pady $Yoff    
    pack $f.eProb -side left -padx 1 -pady $Yoff 
    if  {$EMSegment(SegmentMode)} {
    
    set menubutton  $f.mbProbVolumeSelect
    set menu        $f.mbProbVolumeSelect.m

    eval {menubutton $menubutton -text "None" -relief raised -bd 2 -width 9 -menu $menu} $Gui(WMBA)
    eval {menu $menu} $Gui(WMA)
    if {$Panel == "EM"} {
        DevAddLabel $f.lProbMap "Prob. Map:"
        pack $menubutton $f.lProbMap -side right -padx 1 -pady $Yoff 
    } else {
        pack $menubutton -side left -padx 1 -pady 0 
    }
    TooltipAdd $menubutton "Select Probability Map representing the tissue class!"
    set EMSegment(mb${Panel}-ProbVolumeSelect) $menubutton
    set EMSegment(m${Panel}-ProbVolumeSelect) $menu
    }

}

#-------------------------------------------------------------------------------
# .PROC EMSegmentEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEnter {} {
    global EMSegment
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $EMSegment(eventManager)
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentExit {} {

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
# .PROC EMSegmentShowFile
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc EMSegmentShowFile {} {
    global EMSegment
    
    $EMSegment(lFile) config -text "You entered: $EMSegment(FileName)"
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentBindingCallback
# Callback routine for bindings to take samples for classes
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBindingCallback { event x y} {
    # W = window where mouse was over !
    global EMSegment

    # Find out intensity at pixel and update class
    # EMSegmentReadGreyValue $X $Y
    switch -exact $event {
    "Display" { set SampleList [EMSegmentReadGreyValue $x $y]
                EMSegmentDisplaySampleLogValues $SampleList
                set ValList [EMSegmentDisplaySampleLine $SampleList]
                if {$EMSegment(NumGraph) > 2} {EMSegmentDisplaySampleCross $ValList}
                  }
    "Sample"  { if {$EMSegment(UseSamples) == 0} {
                  DevErrorWindow "Before taking samples, the 'Use Sample' button in the Class tab has to be checked !"
                  return
                }
            EMSegmentDefineSample [EMSegmentReadGreyValue $x $y]
                # Update Just Mean and Sigma Values
                EMSegmentCalculateClassMeanCovariance
                EMSegmentDisplayClassDefinition
                  }
    "Enter"   { EMSegmentEnterSample $x $y}
        "Leave"   { EMSegmentLeaveSample}
    }
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentUpdateMRML
#
# Update EMSegment Variables from MRML Tree
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateMRML {} {
    global Mrml EMSegment Volume

    # --------------------------------------------------
    # 1.) Update the Volume Lists - Volumes could be 
    #     added or deleted
    # -------------------------------------------------
    if {$EMSegment(UseSamples) == 1} {set EMSegment(UseSamples) 0;EMSegmentUseSamples 1}
    if {$EMSegment(SegmentMode) > 0} {
      DevUpdateNodeSelectButton Volume EMSegment Cl-ProbVolumeSelect ProbVolumeSelect EMSegmentProbVolumeSelectNode
      DevUpdateNodeSelectButton Volume EMSegment EM-ProbVolumeSelect ProbVolumeSelect EMSegmentProbVolumeSelectNode
    }
    EMSegmentUpdateVolumeList   
    set IntensityAvgClass $EMSegment(IntensityAvgClass) 
    # puts "---------------------- EMSegmentUpdateMRML ---------------"
    # Do we need it ?
    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
    while { $item != "" } {
       set ClassName [$item GetClassName]
       if { $ClassName == "vtkMrmlSegmenterNode" } {
        # --------------------------------------------------
        # 2.) Check if we already work on this Segmenter
        #     => if yes , do not do anything
        # -------------------------------------------------
        set pid [$item GetID]
        if {$EMSegment(SegmenterNode) != "" && $pid == [$EMSegment(SegmenterNode) GetID] && [$EMSegment(SegmenterNode) GetAlreadyRead]} {
            break   
        }  
        set VolumeList ""
        foreach VolID $Volume(idList) {
          lappend VolumeList "[Volume($VolID,node) GetName] {[Volume($VolID,node) GetImageRange]} [Volume($VolID,node) GetFilePrefix]"
        }
        
        set NumClasses 0
        set NumberOfGraphs 0
        set EMSegment(SegmenterNode) $item 
        $EMSegment(SegmenterNode) SetAlreadyRead 1

        # Reset all Input and Graph Values
        set EMSegment(SegmenterGraphNodeList) ""
        set EMSegment(SegmenterInputNodeList) ""
        set EMSegment(SegmenterClassNodeList) ""
        set EMSegment(SegmenterCIMNodeList) ""
        EMSegmentDeleteFromSelList $EMSegment(SelVolList,VolumeList)
        # --------------------------------------------------
        # 3.) Update variables 
        # -------------------------------------------------
        # If the path is not the same, define all Segmenter variables 
        set EMSegment(NumClassesNew)              [Segmenter($pid,node) GetNumClasses]                 
        EMSegmentSetMaxInputChannelDef            [Segmenter($pid,node) GetMaxInputChannelDef]
        EMSegmentCreateDeleteClasses
        set EMSegment(EMiteration)                [Segmenter($pid,node) GetEMiteration]
        set EMSegment(MFAiteration)               [Segmenter($pid,node) GetMFAiteration]                
        set EMSegment(Alpha)                      [Segmenter($pid,node) GetAlpha]                       
        set EMSegment(SmWidth)                    [Segmenter($pid,node) GetSmWidth]                     
        set EMSegment(SmSigma)                    [Segmenter($pid,node) GetSmSigma] 
        set EMSegment(PrintIntermediateResults)   [Segmenter($pid,node) GetPrintIntermediateResults]    
        set EMSegment(PrintIntermediateSlice)     [Segmenter($pid,node) GetPrintIntermediateSlice]      
        set EMSegment(PrintIntermediateFrequency) [Segmenter($pid,node) GetPrintIntermediateFrequency]  
        set EMSegment(StartSlice)                 [Segmenter($pid,node) GetStartSlice]
        set EMSegment(EndSlice)                   [Segmenter($pid,node) GetEndSlice]                 
        set EMSegment(Graph,DisplayProbNew)       [Segmenter($pid,node) GetDisplayProb]                 
        if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} { set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew); EMSegmentRedrawGraph }    
        set EMSegment(NumberOfTrainingSamples)    [Segmenter($pid,node) GetNumberOfTrainingSamples]
    set IntensityAvgClass                     [Segmenter($pid,node) GetIntensityAvgClass]
    
    } elseif {$ClassName == "vtkMrmlSegmenterGraphNode" } {
        # --------------------------------------------------
        # 4.) Only change Graph variables until graph 
        #     definitions extends the Number of Graphs 
        #     defined in the Module 
        # -------------------------------------------------
        if {($NumberOfGraphs < $EMSegment(NumGraph)) && ($NumberOfGraphs < 2)} {
          lappend EMSegment(SegmenterGraphNodeList)  $item 
          set pid [$item GetID]
          set EMSegment(Graph,XminNew,$NumberOfGraphs) [SegmenterGraph($pid,node) GetXmin]
          set EMSegment(Graph,XmaxNew,$NumberOfGraphs) [SegmenterGraph($pid,node) GetXmax]
          set EMSegment(Graph,XscaNew,$NumberOfGraphs) [SegmenterGraph($pid,node) GetXsca]
          EMSegmentRescaleGraphMulti $NumberOfGraphs
          incr NumberOfGraphs
        }
    } elseif {$ClassName == "vtkMrmlSegmenterInputNode" } {
        # --------------------------------------------------
        # 5.) Update selected Input List 
        # -------------------------------------------------
        # find out the Volume correspnding to the following description
        lappend EMSegment(SegmenterInputNodeList)  $item 
        set pid [$item GetID]
        set FilePrefix [SegmenterInput($pid,node) GetFilePrefix]
        set FileName   [SegmenterInput($pid,node) GetFileName]
        set ImageRange [SegmenterInput($pid,node) GetImageRange]

        foreach VolID $Volume(idList) VolAttr $VolumeList {
           if {([lindex $VolAttr 0] == $FileName) && ([lindex $VolAttr 1] == $ImageRange) && ([lindex $VolAttr 2] == $FilePrefix) } {
              set EMSegment(AllVolList,ActiveID) [lsearch -exact $EMSegment(AllVolList,VolumeList) $VolID]
              EMSegmentTransfereVolume All
          set EMSegment(IntensityAvgValue,$VolID) [SegmenterInput($pid,node) GetIntensityAvgValuePreDef]
              break;
           }
        }
    } elseif {$ClassName == "vtkMrmlSegmenterClassNode" } {
        # --------------------------------------------------
        # 6.) Update selected Class List 
        # -------------------------------------------------
        lappend EMSegment(SegmenterClassNodeList)  $item 
        incr NumClasses
        set pid [$item GetID]
        EMSegmentClickLabel $NumClasses [SegmenterClass($pid,node) GetLabel] 
        set EMSegment(Cattrib,$NumClasses,Prob)   [SegmenterClass($pid,node) GetProb]
        set LocalPriorPrefix [SegmenterClass($pid,node) GetLocalPriorPrefix]
        set LocalPriorName   [SegmenterClass($pid,node) GetLocalPriorName]
        set LocalPriorRange  [SegmenterClass($pid,node) GetLocalPriorRange]
        set EMSegment(Cattrib,$NumClasses,ProbabilityData) ""
        foreach VolID $Volume(idList) VolAttr $VolumeList {
            if {([lindex $VolAttr 0] == $LocalPriorName) && ([lindex $VolAttr 1] == $LocalPriorRange) && ([lindex $VolAttr 2] == $LocalPriorPrefix) } {
               set EMSegment(Cattrib,$NumClasses,ProbabilityData) $VolID
               break;
            }
        }
        if {$EMSegment(Cattrib,$NumClasses,ProbabilityData) == ""} {
           set EMSegment(Cattrib,$NumClasses,ProbabilityData) $Volume(idNone) 
        }
        set index 0
        set LogCovariance  [SegmenterClass($pid,node) GetLogCovariance]
        set LogMean [SegmenterClass($pid,node) GetLogMean]
        for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
           set EMSegment(Cattrib,$NumClasses,LogMean,$y) [lindex $LogMean $y]
           for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
              set EMSegment(Cattrib,$NumClasses,LogCovariance,$y,$x)  [lindex $LogCovariance $index]
              incr index
           }
           # This is for the extra character at the end of the line (';')
           incr index
        }  
    } elseif {$ClassName == "vtkMrmlSegmenterCIMNode" } {
        # --------------------------------------------------
        # 6.) Update selected CIM List 
        # -------------------------------------------------
        lappend EMSegment(SegmenterCIMNodeList) $item 
        set pid [$item GetID]
        set dir [SegmenterCIM($pid,node) GetName]
        if {[lsearch $EMSegment(CIMList) $dir] > -1} { 
        set i 0
        set CIMMatrix [SegmenterCIM($pid,node) GetCIMMatrix]
        for {set y 1} { $y<= $EMSegment(NumClasses)} {incr y} {
            for {set x 1} { $x<= $EMSegment(NumClasses)} {incr x} {
              set EMSegment(CIMMatrix,$x,$y,$dir) [lindex $CIMMatrix $i]
              incr i
            }
            incr i
          }
        }
    } elseif {$ClassName == "vtkMrmlEndSegmenterNode" } {
        # if there is no EndSegmenterNode yet and we are reaading one, and set
        # the EMSegment(EndSegmenterNode) variable
        if { $EMSegment(EndSegmenterNode) == ""} {
        set EMSegment(EndSegmenterNode) $item 
        }
        # Update the new new data which we got from our class definitions
        if {$EMSegment(SegmentMode) > 0} {
        set EMSegment(ProbVolumeSelect)  $EMSegment(Cattrib,$EMSegment(Class),ProbabilityData)
        }
        EMSegmentChangeClass 1
    }
    
    set item [Mrml(dataTree) GetNextItem]
  }
  if {$EMSegment(IntensityAvgClass) != $IntensityAvgClass} {
     EMSegmentChangeIntensityClass [lindex [EMSegmentFindClassAndTestfromIntClass $IntensityAvgClass] 0] 0
  }
  EMSegmentDisplayClassDefinition
  # puts "---------------------- End EMSegmentUpdateMRML ---------------"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentProbVolumeSelectNode
#
# Called by when the mb{type}-ProbVolumeSelect is called 
# .END
#-------------------------------------------------------------------------------
proc EMSegmentProbVolumeSelectNode { type id ArrayName ModelLabel ModelName} {
    global Volume EMSegment

    if {$id == ""} {
    set Text "None"
    } elseif {$id == -5} {
        set Text "Create New"
        set EMSegment(ProbVolumeSelect) $id
    } else {
    set Text [${type}($id,node) GetName]
    set EMSegment(ProbVolumeSelect) $id
    }
    $EMSegment(mbCl-ProbVolumeSelect) config -text $Text
    $EMSegment(mbEM-ProbVolumeSelect) config -text $Text

}

#-------------------------------------------------------------------------------
# .PROC EMSegmentMRMLDeleteCreateNodesNodesListEntries 
#
# Deletes or adds Nodes to the list 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentMRMLDeleteCreateNodesListEntries {Type New} {
    global EMSegment
    set Old [llength $EMSegment(Segmenter${Type}NodeList)]
    if {$New == $Old} {return}

    if {$New < $Old} {
       # Delete Nodes that are to many 
    foreach id [lrange $EMSegment(Segmenter${Type}NodeList) $New end] {
       MainMrmlDeleteNode Segmenter${Type} [$id GetID]
       }
       set EMSegment(Segmenter${Type}NodeList) [lrange $EMSegment(Segmenter${Type}NodeList) 0 [expr $New-1]]
   } else {
       # We Have to create new Nodes
       for {set i $Old} { $i <$New} { incr i} {
       set id [MainMrmlInsertBeforeNode $EMSegment(EndSegmenterNode) Segmenter${Type}]
       lappend EMSegment(Segmenter${Type}NodeList) $id
       }
   }
   
   for {set i 0} { $i <$New} { incr i} {
       set id [lindex $EMSegment(Segmenter${Type}NodeList) $i]
       switch [string tolower $Type] {
           "graph" {Segmenter${Type}([$id GetID],node) SetName [expr $i+1]}
           "input" {Segmenter${Type}([$id GetID],node) SetName "Channel$i"} 
           "cim"   {Segmenter${Type}([$id GetID],node) SetName [lindex $EMSegment(CIMList) $i]}
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSaveSetting 
# Updates the MRML Tree and saves the setting to a file  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSaveSetting {FileFlag} {
    global EMSegment Volume
    # -------------------------------------------------------------------
    # 1.) Segmenter 
    # -------------------------------------------------------------------
    
   #Do we already have an instance or not
    if { $EMSegment(SegmenterNode) == ""} { 
    set EMSegment(SegmenterNode) [MainMrmlAddNode Segmenter]
    set EMSegment(EndSegmenterNode) [MainMrmlAddNode "EndSegmenter"]
    } 
    # Update Segmenter values in MRML Graph
    set pid [$EMSegment(SegmenterNode) GetID]
    Segmenter($pid,node) SetAlreadyRead                 1 
    Segmenter($pid,node) SetNumClasses                  $EMSegment(NumClasses)
    Segmenter($pid,node) SetMaxInputChannelDef          $EMSegment(MaxInputChannelDef)
    Segmenter($pid,node) SetEMiteration                 $EMSegment(EMiteration)
    Segmenter($pid,node) SetMFAiteration                $EMSegment(MFAiteration)  
    Segmenter($pid,node) SetAlpha                       $EMSegment(Alpha)  
    Segmenter($pid,node) SetSmWidth                     $EMSegment(SmWidth)  
    Segmenter($pid,node) SetSmSigma                     $EMSegment(SmSigma)  
    Segmenter($pid,node) SetPrintIntermediateResults    $EMSegment(PrintIntermediateResults)  
    Segmenter($pid,node) SetPrintIntermediateSlice      $EMSegment(PrintIntermediateSlice)  
    Segmenter($pid,node) SetPrintIntermediateFrequency  $EMSegment(PrintIntermediateFrequency)  
    Segmenter($pid,node) SetStartSlice                  $EMSegment(StartSlice)  
    Segmenter($pid,node) SetEndSlice                    $EMSegment(EndSlice)  
    Segmenter($pid,node) SetDisplayProb                 $EMSegment(Graph,DisplayProb)  
    Segmenter($pid,node) SetNumberOfTrainingSamples     $EMSegment(NumberOfTrainingSamples)
    Segmenter($pid,node) SetIntensityAvgClass           $EMSegment(IntensityAvgClass)

    # -------------------------------------------------------------------
    # 2.) SegmenterGraph 
    # -------------------------------------------------------------------
    EMSegmentMRMLDeleteCreateNodesListEntries Graph [expr $EMSegment(NumGraph) > 1 ? 2 : 1]
    # Update Values
    set i 0
    foreach id $EMSegment(SegmenterGraphNodeList) { 
       set pid [$id GetID]
       SegmenterGraph($pid,node) SetXmin  $EMSegment(Graph,Xmin,$i)
       SegmenterGraph($pid,node) SetXmax  $EMSegment(Graph,Xmax,$i)
       SegmenterGraph($pid,node) SetXsca  $EMSegment(Graph,Xsca,$i)
       incr i
    } 
    # -------------------------------------------------------------------
    # 3.) SegmenterInput 
    # -------------------------------------------------------------------
    EMSegmentMRMLDeleteCreateNodesListEntries Input $EMSegment(NumInputChannel) 
    foreach v $EMSegment(SelVolList,VolumeList) id $EMSegment(SegmenterInputNodeList) {
       # Set Values
       set pid [$id GetID]
       SegmenterInput($pid,node) SetFilePrefix [Volume($v,node) GetFilePrefix]
       SegmenterInput($pid,node) SetFileName   [Volume($v,node) GetName]
       eval SegmenterInput($pid,node) SetImageRange [Volume($v,node) GetImageRange] 
       SegmenterInput($pid,node) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v)
   }
    # -------------------------------------------------------------------
    # 4.) SegmenterClass 
    # -------------------------------------------------------------------
    EMSegmentMRMLDeleteCreateNodesListEntries Class $EMSegment(NumClasses)
    set i 0
    set EMSegment(Cattrib,$EMSegment(Class),ProbabilityData) $EMSegment(ProbVolumeSelect)

    foreach id $EMSegment(SegmenterClassNodeList) {
    incr i 
    # Set Values
    set pid [$id GetID]
    SegmenterClass($pid,node) SetName  "$EMSegment(Cattrib,$i,Label)"
    SegmenterClass($pid,node) SetLabel $EMSegment(Cattrib,$i,Label)
    SegmenterClass($pid,node) SetProb  $EMSegment(Cattrib,$i,Prob)
    set v $EMSegment(Cattrib,$i,ProbabilityData)
    if {$v != $Volume(idNone) } {
        SegmenterClass($pid,node) SetLocalPriorPrefix     [Volume($v,node) GetFilePrefix]
        SegmenterClass($pid,node) SetLocalPriorName       [Volume($v,node) GetName]
        eval SegmenterClass($pid,node) SetLocalPriorRange [Volume($v,node) GetImageRange]
    } else {
        SegmenterClass($pid,node) SetLocalPriorPrefix ""
        SegmenterClass($pid,node) SetLocalPriorName   ""
    }
    set LogMean ""
    set LogCovariance ""
    for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
        lappend LogMean $EMSegment(Cattrib,$i,LogMean,$y)
        for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
        lappend LogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x)
        }
        lappend LogCovariance "|"
    }
    SegmenterClass($pid,node) SetLogMean "$LogMean"
    SegmenterClass($pid,node) SetLogCovariance "$LogCovariance"
    }
    
    # -------------------------------------------------------------------
    # 5.) SegmenterCIM 
    # -------------------------------------------------------------------
    EMSegmentMRMLDeleteCreateNodesListEntries CIM [llength $EMSegment(CIMList)] 
    foreach id $EMSegment(SegmenterCIMNodeList) dir $EMSegment(CIMList) {
      # Set Values
      set pid [$id GetID]
      set CIMMatrix ""
      for {set y 1} { $y<= $EMSegment(NumClasses)} {incr y} {
        for {set x 1} { $x<= $EMSegment(NumClasses)} {incr x} {
          lappend CIMMatrix $EMSegment(CIMMatrix,$x,$y,$dir)
        }
        lappend CIMMatrix "|"
      }
      SegmenterCIM($pid,node) SetCIMMatrix $CIMMatrix 
    }
    if {$FileFlag} {MainMenu File SaveAs}
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentStartEM
# Starts the EM Algorithm 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentStartEM { } {
   global EMSegment Volume Mrml
   # ----------------------------------------------
   # 1. Update Values
   # ----------------------------------------------
   EMSegmentCalculateClassMeanCovariance
   EMSegmentCalcProb
   # ----------------------------------------------
   # 2. Check Values and Update MRML Tree
   # ----------------------------------------------
   if {$EMSegment(NumInputChannel)  == 0} {
       DevErrorWindow "Please load a volume before starting the segmentation algorithm!"
       return
   }
   
   if {$EMSegment(StartSlice) < 1 } {
       DevErrorWindow "Start Slice must be greater than 0 !" 
       return
   }
   if  { [EMSegmentEndSlice 0] <  $EMSegment(EndSlice)} {
       DevErrorWindow "End Slices is greate than the existing number of slices!" 
       return
   }
   # Update MRML Tree
   # EMSegmentSaveSetting 0

   # ----------------------------------------------
   # 3. Call Algorithm
   # ----------------------------------------------

   set NumInputImagesSet [EMSegmentAlgorithmStart] 
   # ----------------------------------------------
   # 4. Write Back Results 
   # ----------------------------------------------
   set result [DevCreateNewCopiedVolume [lindex $EMSegment(SelVolList,VolumeList) 0] "" "EMSegResult" ]
   set node [Volume($result,vol) GetMrmlNode]
   $node SetLabelMap 1
   Mrml(dataTree) RemoveItem $node 
   set nodeBefore [Volume([lindex $EMSegment(SelVolList,VolumeList) 0],vol) GetMrmlNode]
   Mrml(dataTree) InsertAfterItem $nodeBefore $node
   # Display Result in label mode 
   Volume($result,vol) UseLabelIndirectLUTOn
   Volume($result,vol) Update
   Volume($result,node) SetLUTName -1
   Volume($result,node) SetInterpolate 0
   # Volume($result,node) SetScalarTypeToUnsignedShort
   #  Write Solution to new Volume  -> Here the thread is called
   Volume($result,vol) SetImageData [EMSegment(vtkEMSegment) GetOutput]
   EMSegment(vtkEMSegment) Update
  
   # ----------------------------------------------
   # 5. Recover Values 
   # ----------------------------------------------
   if {$EMSegment(SegmentMode) > 0} {
       set index 1
       foreach v $EMSegment(SelVolList,VolumeList) {
       if {$EMSegment(IntensityAvgValue,$v) < 0} {
           set EMSegment(IntensityAvgValue,$v) [EMSegment(vtkEMSegment) GetIntensityAvgValueCurrent $index]
       }
       incr index
       }
   }

   # Update MRML Tree
   EMSegmentSaveSetting 0

   # Update MRML
   MainUpdateMRML
   
   # This is necessary so that the data is updated correctly.
   # If the programmers forgets to call it, it looks like nothing
   # happened
   MainVolumesUpdate $result

   # ----------------------------------------------
   # 6. Clean up mess 
   # ----------------------------------------------
   # This is done so the vtk instance won't be called again when saving the model
   if {$EMSegment(SegmentMode) > 0} {
       while {$NumInputImagesSet > 0} {
       incr NumInputImagesSet -1
       EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet "" 
       }
   } else {
       EMSegment(vtkEMSegment) SetInput ""
   } 
   Volume($result,vol) SetImageData [EMSegment(vtkEMSegment) GetOutput]
   EMSegment(vtkEMSegment) SetOutput ""
   # Delete instance
   EMSegment(vtkEMSegment) Delete
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentImportCIM
# Imports the CIM Matrix and other paramters 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentImportCIM {flag} {
   global EMSegment
   if {[DevFileExists $EMSegment(FileCIM)]} {
      EMSegmentReadCIMFile $flag
   } 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentClickLabel
# Sets everything correctly after user choosed label number and color 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentClickLabel {{Sclass ""} {label ""} {colorcode ""}} {
    global EMSegment Label Mrml Color 
    if {$Sclass == ""} {set  Sclass $EMSegment(Class)}
    # if label ="" it was called back from Label.tcl

    set ChangeIntensityMenu 0
    if {$EMSegment(Cattrib,$Sclass,Label) == $EMSegment(IntensityAvgClass)} {set ChangeIntensityMenu 1}

    if {$label == ""} {
        set EMSegment(Cattrib,$Sclass,Label) $Label(label)    
        set EMSegment(Cattrib,$Sclass,ColorCode) [MakeColorNormalized $Label(diffuse)] 
    } else {
       set EMSegment(Cattrib,$Sclass,Label) $label    
       if {$colorcode == ""} {
         foreach c $Color(idList) {
            set i [lsearch -exact [Color($c,node) GetLabels] $label]
            if {$i > -1} { break}
         }
         if {$i > -1} {
           set EMSegment(Cattrib,$Sclass,ColorCode) [MakeColorNormalized [Color($c,node) GetDiffuseColor]]
         } else {
            # Default color is black [MakeColorNormalized "0 0 0"]
            set EMSegment(Cattrib,$Sclass,ColorCode) "#000000"
         }
       } else {
          set EMSegment(Cattrib,$Sclass,ColorCode) $colorcode
       }
    }
    # Color Graph new 
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
       if {$EMSegment(Cattrib,$Sclass,Graph,$i) } {
          $EMSegment(Cl-caGraph$i) itemconfigure  Graph$Sclass -fill $EMSegment(Cattrib,$Sclass,ColorCode)
       }
    }

    # Change Color of button
    if {$Sclass == $EMSegment(Class)} {
      $EMSegment(EM-mbClasses) configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)

      $EMSegment(Cl-mbClasses) configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)

      $EMSegment(Cl-bColorLabel) configure -text $EMSegment(Cattrib,$Sclass,Label) -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) 
    }
    if {$ChangeIntensityMenu} {
    set EMSegment(IntensityAvgClass) $EMSegment(Cattrib,$Sclass,Label)
    $EMSegment(DE-mbIntClass) configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
          -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label) 
    }


    $EMSegment(EM-mbClasses).m entryconfigure [expr $Sclass-1] -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)

    $EMSegment(Cl-mbClasses).m entryconfigure [expr $Sclass-1] -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)

    $EMSegment(DE-mbIntClass).m entryconfigure $Sclass -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)

    if {$Sclass < 5} { 
    $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)
    } else {
    $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)
    }
    set f $EMSegment(CIM-fMatrix)
    $f.fLineL.l$Sclass configure -text "$EMSegment(Cattrib,$Sclass,Label)"
    $f.fLine$Sclass.lLabel configure -text "$EMSegment(Cattrib,$Sclass,Label)"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplayClassDefinition
# Displays in window current definition of class 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplayClassDefinition {} {
    global EMSegment Volume

    set Sclass $EMSegment(Class)
    #--------------------- 
    # Update panel EM
    #--------------------- 
    $EMSegment(EM-mbClasses) config -text $EMSegment(Cattrib,$Sclass,Label)    
    if {[llength $EMSegment(SelVolList,VolumeList)]  } {
    $EMSegment(EM-lSampvar) config -text [llength $EMSegment(Cattrib,$Sclass,[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)]  
    } else {
    $EMSegment(EM-lSampvar) config -text 0 
    }
    #---------------------
    #Update panel Class
    #---------------------
    $EMSegment(Cl-mbClasses) config -text "$EMSegment(Cattrib,$Sclass,Label)"
   
    if {$EMSegment(SegmentMode) > 0} {
    if {[lsearch -exact $Volume(idList) $EMSegment(Cattrib,$Sclass,ProbabilityData)] < 0} {
        set EMSegment(Cattrib,$Sclass,ProbabilityData) $Volume(idNone) 
    }
    set name [Volume($EMSegment(Cattrib,$Sclass,ProbabilityData),node) GetName]
    $EMSegment(mbEM-ProbVolumeSelect) configure -text $name
    $EMSegment(mbCl-ProbVolumeSelect) configure -text $name 
    set EMSegment(ProbVolumeSelect) $EMSegment(Cattrib,$Sclass,ProbabilityData)
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentUseSamples
# Use Sample for calulating Mean or Variance
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUseSamples {change} {
    global EMSegment Gui
    if {$EMSegment(UseSamples) == 1} {
        TooltipAdd $EMSegment(Cl-cSample) "Press button to manually enter Mean and Covariance."
        for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} { 
           $EMSegment(Cl-fLogMeanVar).e$y configure -state disabled
           $EMSegment(EM-fMeanVar).e$y configure -fg $Gui(textDark)
           for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
             $EMSegment(Cl-fLogCovVar).fLine$y.e$x configure -state disabled
           }
        }  
        $EMSegment(Cl-mbEraseSample) configure -state normal
        $EMSegment(EM-lSampvar)      configure -fg $Gui(textDark)
        $EMSegment(EM-lexpltxt)      configure -fg $Gui(textDark)
        if {$change == 1} {EMSegmentCalculateClassMeanCovariance}
    } else {
       TooltipAdd $EMSegment(Cl-cSample) "Press button to use samples for the calucation of Mean and Covariance"
       for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {  
          $EMSegment(Cl-fLogMeanVar).e$y configure -state normal
          $EMSegment(EM-fMeanVar).e$y configure -fg $Gui(textDisabled)
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {  
             $EMSegment(Cl-fLogCovVar).fLine$y.e$x configure -state normal
          }
       }
       $EMSegment(Cl-mbEraseSample) configure -state disabled
       $EMSegment(EM-lSampvar)      configure -fg $Gui(textDisabled)
       $EMSegment(EM-lexpltxt)      configure -fg $Gui(textDisabled)
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentFindClassAndTestfromIntClass
# Find Class and Text to put on menu button 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentFindClassAndTestfromIntClass {IntLabel} {
    global EMSegment
    set IntClass 1
    set IntText "None"
    while { ($IntClass <= $EMSegment(NumClasses)) && ($IntText == "None")} {
    if {$IntLabel == $EMSegment(Cattrib,$IntClass,Label)} {
        set IntText $IntLabel 
    } else {
        incr IntClass
    }
    }
    if {$IntText == "None"} {set IntClass -1}
    return "$IntClass $IntText"
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeClass
# Changes from one to another class and displays new class in window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeClass {i} {
    global EMSegment

    EMSegmentCalculateClassMeanCovariance
    if {$EMSegment(UseSamples) == 1} {
        set EMSegment(UseSamples) 0
        EMSegmentUseSamples 1   
    } 
    set EMSegment(Cattrib,$EMSegment(Class),ProbabilityData) $EMSegment(ProbVolumeSelect)

    # Delete old Sample entries of the new class
    if { $EMSegment(NumInputChannel) } {
      if { [llength $EMSegment(Cattrib,$EMSegment(Class),[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)] > 0 }  {
        $EMSegment(Cl-mEraseSample)  delete 1 end
      }
    }

    # Change active class to new one
    set EMSegment(Class) $i
    set Sclass $EMSegment(Class)

    # Change Variable the Entry field is assigned width
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} { 
      $EMSegment(EM-fMeanVar).e$y config -textvariable  EMSegment(Cattrib,$Sclass,LogMean,$y) 
      $EMSegment(Cl-fLogMeanVar).e$y config -textvariable  EMSegment(Cattrib,$Sclass,LogMean,$y) 
      for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
        $EMSegment(Cl-fLogCovVar).fLine$y.e$x config -textvariable  EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x)
      }
    }

    $EMSegment(EM-eProb)  config  -textvariable EMSegment(Cattrib,$Sclass,Prob)
    $EMSegment(Cl-eProb)  config  -textvariable EMSegment(Cattrib,$Sclass,Prob)
    $EMSegment(EM-mbClasses) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-mbClasses) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-bColorLabel) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)\
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)

    # Define new Sample list 
    if { $EMSegment(NumInputChannel) } {
       set v [lindex $EMSegment(SelVolList,VolumeList) 0] 
       set index 1
       foreach line $EMSegment(Cattrib,$Sclass,$v,Sample) {
           $EMSegment(Cl-mEraseSample) add command -label "[format "%3d %3d %3d" [lindex $line 0] [lindex $line 1] [lindex $line 2]]" -command "EMSegmentEraseSample $index"
           incr index
       }
    }
    # Display new class
    EMSegmentDisplayClassDefinition
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeIntensityClass 
# Changes from one to another intensity class 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeIntensityClass {Sclass reset} {
    global EMSegment Gui
    if {$Sclass > 0} {
    set EMSegment(IntensityAvgClass) $EMSegment(Cattrib,$Sclass,Label)
    $EMSegment(DE-mbIntClass) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)
    } else {
    set EMSegment(IntensityAvgClass) -1 
    eval {$EMSegment(DE-mbIntClass) config -text "None"}  $Gui(WMBA)
    }
    # Reset Values 
    if { ($EMSegment(NumInputChannel)) && $reset } {
       foreach v $EMSegment(SelVolList,VolumeList) {
       set EMSegment(IntensityAvgValue,$v) -1.0 
       }
    }
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentUpdateClassButton 
# Calculates  the mean and Sigma
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateClassButton { } {
    global EMSegment
    set Sclass $EMSegment(Class)
    set Change [EMSegmentCalculateClassMeanCovariance]
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {$EMSegment(Cattrib,$Sclass,Graph,$i)} {
        set Change [EMSegmentCalculateClassCurveRegion $Sclass $i]
        if {$Change == -1} {return}
    }
    }
    set ChangeProb [EMSegmentCalcProb]
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {[expr ($Change == 1) && ($ChangeProb == 0) && $EMSegment(Cattrib,$Sclass,Graph,$i)]} {
        EMSegmentDrawClassCurveRegion $Sclass $i
    }
    }    
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassMeanCovariance
# Calculates  the mean and Sigma 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassMeanCovariance { } {
    global EMSegment 
    set Sclass $EMSegment(Class)
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
    set oldMean($y)  $EMSegment(Cattrib,$Sclass,LogMean,$y)
    for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
        set oldCovariance($y,$x) $EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x)
    }
    }
    if { $EMSegment(UseSamples) == 1 } {
    #------------------------------------- 
    # Read Sample Values 
    #------------------------------------- 
    # EMSegmentReadTextBox
    
    #------------------------------------- 
    # Calculate Mean 
    #-------------------------------------
    if {$EMSegment(NumInputChannel) } {
        set Clength [llength $EMSegment(Cattrib,$Sclass,[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)]
        for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {   
        set LogMeanValue 0.0
        set VolID [lindex $EMSegment(SelVolList,VolumeList) $x]
        for {set i 0} {$i < $Clength} {incr i} {
            set LogMeanValue [expr $LogMeanValue + log([lindex [lindex $EMSegment(Cattrib,$Sclass,$VolID,Sample) $i] 3] + 1)]
        }
        if {$Clength > 0} {
            set EMSegment(Cattrib,$Sclass,LogMean,$x) [expr round($LogMeanValue / double($Clength)*10000.0)/10000.0] 
                } else { 
            set EMSegment(Cattrib,$Sclass,LogMean,$x) -1 
        }
        }
        #------------------------------------- 
        # Calculate Variance 
        #-------------------------------------
        for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} { 
        # Covariance Matrix : cov[x][y] = cov[y][x]
        set YVolID [lindex $EMSegment(SelVolList,VolumeList) $y]
        for {set x $y} {$x < $EMSegment(NumInputChannel)} {incr x} { 
            set LogCovariance 0.0 
            set XVolID [lindex $EMSegment(SelVolList,VolumeList) $x]
            for {set i 0} {$i < $Clength} {incr i} { 
            # Covariance = (Sum(Sample(x,i) - mean(x))*(Sample(y,i) - mean(y)))/(n-1)
            set LogCovariance [expr $LogCovariance + ( (log([lindex [lindex $EMSegment(Cattrib,$Sclass,$XVolID,Sample) $i] 3]+1) - $EMSegment(Cattrib,$Sclass,LogMean,$x)) * (log([lindex [lindex $EMSegment(Cattrib,$Sclass,$YVolID,Sample) $i] 3]+1) - $EMSegment(Cattrib,$Sclass,LogMean,$y)))]    
            }        
            if { $EMSegment(SegmentMode) < 2 } {
            if {$Clength < 1} {
                set EMSegment(Cattrib,$Sclass,LogCovariance,$x,$x) -1
            } elseif {$Clength < 2} {   
                set EMSegment(Cattrib,$Sclass,LogCovariance,$x,$x) 0.0
            } else {
                # We calculate Sigma => We have to square it / for the multichannel version we calculate the covariance Matrix
                # set EMSegment(Cattrib,$Sclass,Sigma) [expr sqrt($Variance / double($Clength))]
                set EMSegment(Cattrib,$Sclass,LogCovariance,$x,$x) [expr round(sqrt($LogCovariance / double($Clength - 1.0))*10000.0)/10000.0]
            }
            } else {
            if {$Clength < 2} {
                set EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) 0.0
            } else {
                set EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) [expr round($LogCovariance / double($Clength - 1.0)*10000.0)/10000.0]
            }
            if {$x != $y} {                
                set EMSegment(Cattrib,$Sclass,LogCovariance,$x,$y) $EMSegment(Cattrib,$Sclass,LogCovariance,$x,$y) 
            }
            }
        }
        }
    }
    } else {
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
        set EMSegment(Cattrib,$Sclass,LogMean,$y)  [expr round($EMSegment(Cattrib,$Sclass,LogMean,$y)*10000.0)/10000.0] 
        for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
        set EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) [expr round($EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x)*10000.0)/10000.0]
        }
    }
    } 
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
    if {$EMSegment(Cattrib,$Sclass,LogMean,$y) != $oldMean($y) } { return 1}
    for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
        if {$EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) != $oldCovariance($y,$x)} { return 1}
    }
    }
    # No change
    return 0
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalcProb
# Norm the Probablilities  => all are between 0 an 1 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalcProb {} {
    global EMSegment
    set NormProb 0.0
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {    
       set NormProb [expr $NormProb + $EMSegment(Cattrib,$i,Prob)]
    }  
    # Round it up or down otherwise it recalculates everything every time this function is called  
    if {$NormProb != 1.0} {
    set OldNorm $NormProb
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
        set EMSegment(Cattrib,$i,Prob) [expr round($EMSegment(Cattrib,$i,Prob) / $NormProb * 100) / 100.0]
        set OldNorm [expr $OldNorm - $EMSegment(Cattrib,$i,Prob)]
    }  
    #Redraw Graph
    if [expr ($OldNorm != 0.0) && $EMSegment(Graph,DisplayProb)] {
        EMSegmentRedrawGraph
        return 1
    }
    }
    return 0 
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentWriteTextBox  
# Writes values to the Sample Text Box Field  
# EMSegment(Cattrib,$Sclass,Sample)
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentWriteTextBox {} {
   global EMSegment
   set Sclass $EMSegment(Class)
   set i 1
 
   # Display Sample Text Box 
   if {$EMSegment(UseSamples) == 0} {$EMSegment(Cl-textBox) configure -state normal}

   $EMSegment(Cl-textBox) delete 1.0 end
   if {$EMSegment(NumInputChannel)} {
       set x [lindex $EMSegment(SelVolList,VolumeList) 0]       
       foreach Sline $EMSegment(Cattrib,$Sclass,$x,Sample) {
       if {$EMSegment(SegmentMode) < 2 } { 
           $EMSegment(Cl-textBox) insert end "$Sline \n" 
       } else {
           $EMSegment(Cl-textBox) insert end "[lrange $Sline 0 2]  \n" 
       }
       }
   }
   if {$EMSegment(UseSamples) == 0} {$EMSegment(Cl-textBox) configure -state disabled}
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentEraseSample  
# Erases Sample i form the board 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEraseSample {i} {
    global EMSegment
    set Sclass $EMSegment(Class)
    $EMSegment(Cl-mEraseSample) delete $i
    set i [expr $i-1] 
    foreach v $EMSegment(SelVolList,VolumeList) {
    set EMSegment(Cattrib,$Sclass,$v,Sample) [lreplace $EMSegment(Cattrib,$Sclass,$v,Sample) $i $i]
    }
    # Now every entry in the list has to be reconfigured
    incr i
    set length [llength $EMSegment(Cattrib,$Sclass,[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)] 
    for {set index $i} {$index <= $length} {incr index} {
    $EMSegment(Cl-mEraseSample) entryconfigure $index -command "EMSegmentEraseSample $index"
    }
    EMSegmentCalculateClassMeanCovariance 
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentReadTextBox  
# Reades Values out of the Sample Text Box Field and saves it in 
# EMSegment(Cattrib,$Sclass,Sample)
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadTextBox {} {
   global EMSegment
   set Sclass $EMSegment(Class)
   set i 1
   set ErrorFlag 0
   set ReadSample {}
   set ReadLine [$EMSegment(Cl-textBox) get $i.0 $i.end]
   if {$EMSegment(NumInputChannel) == 0} { return }
   while { $ReadLine != {} && $ErrorFlag == 0}  { 
       set ReadX [lindex $ReadLine 0]
       set ReadY [lindex $ReadLine 1]
       if { $EMSegment(SegmentMode) < 2 } { 
       set ReadBrightness [lindex $ReadLine 2]
       if {$ReadBrightness > -1} {
           lappend ReadSample "$ReadX $ReadY $ReadBrightness"
       } else {
           set ErrorFlag 1 
           DevErrorWindow "Sample $i not correctly defined !"
           return
       }
       } else {
      lappend ReadSample "$ReadX $ReadY"
       } 
       incr i
       set ReadLine [$EMSegment(Cl-textBox) get $i.0 $i.end]
   }
   if {$ErrorFlag == 0} {
       set vol [lindex $EMSegment(SelVolList,VolumeList) 0]
       if { $EMSegment(SegmentMode) < 2 } { 
       set EMSegment(Cattrib,$Sclass,$vol,Sample) $ReadSample
       } else {
       set index 0
       foreach elem $EMSegment(Cattrib,$Sclass,$vol,Sample)  {
           if {[lsearch -exact $ReadSample $elem] == -1} {
           foreach v $EMSegment(SelVolList,VolumeList) {
               set EMSegment(Cattrib,$Sclass,$v,Sample) [lreplace $EMSegment(Cattrib,$Sclass,$v,Sample) $index $index]
           }
           } else {
           incr index
           }
       }
       }
   }
}    

#-------------------------------------------------------------------------------
# .PROC  EMSegmentScrolledText  
# Creates a Text box with only a Y Scroll Bar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentScrolledText {f} {
    global Gui
    frame $f -bg $Gui(activeWorkspace)
    eval {text $f.text \
        -yscrollcommand [list $f.yscroll set] -bg} $Gui(normalButton)
    eval {scrollbar $f.yscroll -orient vertical \
        -command [list $f.text yview]} $Gui(WSBA)
    grid $f.text $f.yscroll -sticky news
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    $f.text configure -height 6 -width 12
    return $f.text
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentScrolledHorizontal  
# Creates a Text box with only a X Scroll Bar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentScrolledHorizontal {f} {
    global Gui
    frame $f -bg $Gui(activeWorkspace)
    eval {text $f.text \
        -xscrollcommand [list $f.xscroll set] -bg} $Gui(normalButton)
    eval {scrollbar $f.xscroll -orient horizontal \
        -command [list $f.text yview]} $Gui(WSBA)
    grid $f.text $f.xscroll -sticky news
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    $f.text configure -height 6 -width 12
    return $f.text
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentCreateDeleteClasses  
# Creates or deletes classes for segmentation 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateDeleteClasses {} {
    global EMSegment Volume Gui
    set ColorLabelLength [expr [llength $EMSegment(ColorLabelList)] / 2]
    set CIMflag 0
    if {$EMSegment(NumClassesNew) == $EMSegment(NumClasses)} {return}
    # Class never has changed befor
    if {$EMSegment(NumClassesNew) == -1} {
    set Cstart 1
    set Cfinish [expr $EMSegment(NumClasses) + 1]
    set EMSegment(NumClassesNew) $EMSegment(NumClasses) 
    set Cprob [expr 1 / double($EMSegment(NumClasses))]
    } elseif {$EMSegment(NumClassesNew) < $EMSegment(NumClasses)} {
    # New amount of classes are smaller than old
       set Cstart 1
       set Cfinish 0
       # Delete Menu Buttons
       $EMSegment(Cl-mbClasses).m delete $EMSegment(NumClassesNew)  [expr $EMSegment(NumClasses) -1]
       $EMSegment(EM-mbClasses).m delete $EMSegment(NumClassesNew)  [expr $EMSegment(NumClasses) -1]

       $EMSegment(DE-mbIntClass).m delete $EMSegment(NumClassesNew)  [expr $EMSegment(NumClasses) -1]
       # Update Button if necessary
       set i [expr $EMSegment(NumClassesNew)+1]

       while {$i <= $EMSegment(NumClasses) } {
       if  {$EMSegment(Cattrib,$i,Label) ==  $EMSegment(IntensityAvgClass)} {
           EMSegmentChangeIntensityClass -1 1
           set i $EMSegment(NumClasses)
       }
           incr i
       }
       set LowBound [expr $EMSegment(NumClassesNew)+1]
       # Insert Color Selection bc color is unused now and delete Graph Button
       for {set i $LowBound} {$i <= $EMSegment(NumClasses) } {incr i} { 
       # EMSegmentInsertColorMenuEntry $EMSegment(Cattrib,$i,Color)
       EMSegmentDeleteGraphButton $i
       }

       # Delete Additonal CIM Fields
       set f $EMSegment(CIM-fMatrix)

       # Remove Labels in first line
       for {set j $LowBound} {$j <= $EMSegment(NumClasses) } {incr j} {
          destroy $f.fLineL.l$j
       }
   
       for {set i 0} {$i <= $EMSegment(NumClasses) } {incr i} {
       if {$i < $LowBound} {
               # Just remove additonal Rows 
           for {set j $LowBound} {$j <= $EMSegment(NumClasses)} {incr j} {
                  destroy $f.fLine$i.eCol$j
                  foreach k $EMSegment(CIMList) {
                 set EMSegment(CIMMatrix,$i,$j,$k) ""
              }
          }
      } else {
           destroy $f.fLine$i
               # Just remove whole Line 
               for {set j 1} {$j <= $EMSegment(NumClasses)} {incr j} {
                  foreach k $EMSegment(CIMList) {
                 set EMSegment(CIMMatrix,$i,$j,$k) ""
              }
          }
      }
      }

      set EMSegment(NumClasses) $EMSegment(NumClassesNew)
      # Reconfigure width and height of canvas
      EMSegmentSetCIMMatrix $EMSegment(NumClasses)
      
      # Update Values of new Class
      # Recaluclate Probabilites
      EMSegmentCalcProb
      EMSegmentChangeClass {1}
  } else {
      set Cstart  [expr $EMSegment(NumClasses) + 1]
      set Cfinish [expr $EMSegment(NumClassesNew) +1]
      set EMSegment(NumClasses) $EMSegment(NumClassesNew)
      set Cprob 0.0
      # Add Menu buttons
      for {set i $Cstart} {$i < $Cfinish} {incr i 1} {
          # change Menu Button when selected
          set label [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)+1]]
          set color [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)]]
          $EMSegment(Cl-mbClasses).m add command -label "$label" -command  "EMSegmentChangeClass $i" \
             -background $color -activebackground $color 
          $EMSegment(EM-mbClasses).m add command -label "$label" -command  "EMSegmentChangeClass $i" \
             -background $color -activebackground $color 
      $EMSegment(DE-mbIntClass).m add command -label "$label" -command "EMSegmentChangeIntensityClass $i 1" \
             -background $color -activebackground $color

          # Add fields to Marcov Matrix
          # EMSegment(CIMMatrix,<x>,<y>,<z>)
          set f $EMSegment(CIM-fMatrix) 
          foreach k $EMSegment(CIMList) {
          for {set j $Cstart} {$j < $Cfinish} {incr j} {
          if {$i == $j} {set EMSegment(CIMMatrix,$i,$j,$k) 1
              } else  {set EMSegment(CIMMatrix,$i,$j,$k) 0}
          }
      }

      # Create Graph button
      for {set j 0} {$j < $EMSegment(NumGraph)} {incr j} {
          set EMSegment(Cattrib,$i,Graph,$j) 0
      } 
      EMSegmentCreateGraphButton $i $label $color
      }
      set CIMflag 1
  }
  # Set all parameter for each class
  # it is always defined as EMSegment(Catribute,<Class>,<Attribute>)
  for {set i $Cstart} {$i < $Cfinish } {incr i 1} {
      set EMSegment(Cattrib,$i,Prob) $Cprob 
      for {set y 0} {$y <  $EMSegment(MaxInputChannelDef)} {incr y} {
      set  EMSegment(Cattrib,$i,LogMean,$y) -1
      for {set x 0} {$x <  $EMSegment(MaxInputChannelDef)} {incr x} { 
          set EMSegment(Cattrib,$i,LogCovariance,$y,$x) 0.0
      }
      }
      # Sample S is defined as list S(i:) = {position1 value1} {position2 value2} ... 
      foreach v $EMSegment(SelVolList,VolumeList) {
      set EMSegment(Cattrib,$i,$v,Sample) {}  
      }
      set EMSegment(Cattrib,$i,ColorCode) [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)]]
      # Graph of class is plottet (== 1) or not (== 0)  
      set EMSegment(Cattrib,$i,Label)     [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)+1]]

      # Special EMSegment(SegmentMode) == 1 Variables     
      set EMSegment(Cattrib,$i,ProbabilityData) $Volume(idNone)

      for {set j 0} { $j < $EMSegment(NumGraph)} {incr j} {
      set EMSegment(Cattrib,$i,Graph,$j) 0
      # Define Class Specific Graph Parameters
      if {$j > 1} {
          set EMSegment(Graph,$i,InvLogCov,1,1,$j) -1.0
          set EMSegment(Graph,$i,InvLogCov,1,2,$j) -1.0
          set EMSegment(Graph,$i,InvLogCov,2,2,$j) -1.0
          set EMSegment(Graph,$i,LogMean,1,$j)  -1.0
          set EMSegment(Graph,$i,LogMean,2,$j)  -1.0
          set EMSegment(Graph,$i,XcaL,$j) -1
          set EMSegment(Graph,$i,YcaL,$j) -1
      } else {
          set EMSegment(Graph,$i,LogCov,$j) -1 
          set EMSegment(Graph,$i,LogMean,$j)  -1
      }
      set EMSegment(Graph,$i,XMin,$j)  -1
      set EMSegment(Graph,$i,XMax,$j)  -1
      set EMSegment(Graph,$i,YMin,$j)  -1
      set EMSegment(Graph,$i,YMax,$j)  -1
      }
  }
  # Define CIM Field as Matrix M(Class1,Class2,Relation of Pixels)
  # where the "Relation of the Pixels" can be set as Pixel with "left", 
  # "right", "up" or "down" Neighbour  
  # EMSegment(CIMMatrix,<y>,<x>,<Type>)

  # Add new Rows and lines to the CIM Matrix 
  if {$CIMflag == 1} {EMSegmentCreateCIMRowsColumns $Cstart $EMSegment(NumClassesNew) } 
  for {set i 1} {$i < $Cfinish } {incr i 1} {
      # Just update only new Row elements or whole line 
      if { $i < $Cstart} { set jstart $Cstart 
      } else { set jstart 1 }
      foreach k $EMSegment(CIMList) {
      for {set j $jstart} {$j <$Cfinish} {incr j} {
          if {$i == $j} {set EMSegment(CIMMatrix,$i,$j,$k) 1
          } else  {set EMSegment(CIMMatrix,$i,$j,$k) 0}
      }
      }
  }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSetMaxInputChannelDef
# Set Maximum Input Channel => only changes something if new max input channel
# is larger than old max input channel  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetMaxInputChannelDef {NewMaxInputChannelDef} {
    global EMSegment
    if {$NewMaxInputChannelDef > $EMSegment(MaxInputChannelDef)} {
       for {set ind $EMSegment(MaxInputChannelDef)} {$ind < $NewMaxInputChannelDef} {incr ind} { 
          for {set c 1 } { $c <= $EMSegment(NumClasses)} {incr c} {
            set EMSegment(Cattrib,$c,LogMean,$ind) -1
            for {set j 0} {$j < $NewMaxInputChannelDef} {incr j} {
              set EMSegment(Cattrib,$c,LogCovariance,$j,$ind) 0.0
              set EMSegment(Cattrib,$c,LogCovariance,$ind,$j) 0.0
            }
          }
       }
       set EMSegment(MaxInputChannelDef) $NewMaxInputChannelDef
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentReadGreyValue
# When mouse is clicked, find out location and pixel gray value.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadGreyValue {x y} {
    global EMSegment Interactor
    if {$EMSegment(NumInputChannel) == 0 } {
    return
    }     
    # Look in which window we are (0 1 or 2)
    set s $Interactor(s)

    # Transform Coordinates:
    # Officially :
    # (xs, ys) is the point relative to the lower, left corner 
    # of the slice window (0-255 or 0-511).
    #
    # (x, y) is the point with Zoom and Double taken into consideration
    # (zoom=2 means range is 64-128 instead of 1-256)
    # My Version
    # x,y rigth now reperesent coordinates system placed in the upper left corner lookin down
    # x stays way the it is and ybefore = (255 or 512) - yafter
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
    # => now x y represent lower left corener looking up

    Slicer SetReformatPoint $s $x $y
    scan [Slicer GetIjkPoint] "%g %g %g" xIjk yIjk zIjk
    set xIjk [expr int($xIjk)]
    set yIjk [expr int($yIjk)]
    set zIjk [expr int($zIjk)]

    # Extended for Multi Channel Segmentation
    set result {}
    foreach v $EMSegment(SelVolList,VolumeList) {
    set ImageData [Volume($v,vol) GetOutput]
    scan [$ImageData GetExtent]  "%d %d %d %d %d %d" Xmin Xmax Ymin Ymax Zmin Zmax
    if {[expr (($Xmin > $xIjk) || ($Xmax < $xIjk) || ($Ymin > $yIjk) || ($Ymax < $yIjk) || ($Zmin > $zIjk) || ($Zmax < $zIjk))]} {
        set pixel 0
    } else {
        set pixel [$ImageData GetScalarComponentAsFloat $xIjk $yIjk $zIjk 0]
    }
    lappend result "$x $y $zIjk $pixel" 
    }
    return $result
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDefineSample
# Transferes the taken sample from the image into EMSegment structure
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDefineSample {SampleList} {
    global EMSegment
    # Update class 
    set Sclass $EMSegment(Class)    

    if {$EMSegment(NumInputChannel) == 0 } {
    DevErrorWindow "Before taking samples, define volume(s) to  be segmented !"
    return
    } 
    
    foreach v $EMSegment(SelVolList,VolumeList) sample $SampleList {
    lappend EMSegment(Cattrib,$Sclass,$v,Sample) $sample
    }
    set Line [lindex $SampleList 0]
    $EMSegment(Cl-mEraseSample) add command -label "[format "%3d %3d %3d" [lindex $Line 0] [lindex $Line 1] [lindex $Line 2]]" \
        -command "EMSegmentEraseSample [llength $EMSegment(Cattrib,$Sclass,[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)]" 

}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplaySampleLogValues 
# Displays the currrent log gray value of the image in the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplaySampleLogValues {SampleList} {
    global EMSegment
    if {$EMSegment(NumGraph) < 3} { 
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
        set volume $EMSegment(Cattrib,0,Label,$i)
        if {$volume != ""} {
        set sample [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $volume]]
        $EMSegment(Cl-caGraph$i) itemconfigure GrayT0 -text  [format "%01.4f" [expr log([lindex $sample 3]+1)]]
        }
    }
    } else {
    for {set i 0} { $i < 2} {incr i} {
        set volume $EMSegment(Cattrib,0,Label,$i)
        if {$volume != ""} {
        set sample [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $volume]]
        $EMSegment(Cl-caGraph$i) itemconfigure GrayT0  -text  [format "%01.4f" [expr log([lindex $sample 3]+1)]]
        $EMSegment(Cl-caGraph2) itemconfigure GrayT$i -text  [format "%01.4f" [expr log([lindex $sample 3]+1)]]
        }
    }
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplaySampleLine 
# Displays the currrent log gray value as a red line in the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplaySampleLine {SampleList} {
    global EMSegment
    set resultval [EMSegmentMoveRedSampleLine $SampleList 0]
    if {$EMSegment(NumGraph) > 1} {
    lappend resultval [EMSegmentMoveRedSampleLine $SampleList 1]        
    }
    return $resultval
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentMoveRedSampleLine
# Displays the currrent log gray value as a red line in the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentMoveRedSampleLine {SampleList i} {
    global EMSegment
    set index $EMSegment(Cattrib,0,Label,$i) 
    if {$index != "" } {
    set val [expr [lindex [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $index]] 3] - $EMSegment(Graph,Xmin,$i)]
    if {[expr ($val < ($EMSegment(Graph,Xmax,$i) - $EMSegment(Graph,Xmin,$i) + 1)) && ($val > -1)] } { 
        set xpos $EMSegment(Graph,Xpos,$val,$i)
        $EMSegment(Cl-caGraph$i) coords GrayLine $xpos  [expr $EMSegment(Graph,Ylen,$i) + $EMSegment(Graph,YboU)] $xpos $EMSegment(Graph,YboU)
        return $val
    } 
    } 
    return "-"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplaySampleCross
# Displays the currrent log gray value as a cross in the 2D-graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplaySampleCross {ValList} {
    global EMSegment
    set Xval [lindex $ValList 0]
    set Yval [lindex $ValList 1]
    if {$Xval == "-"} {return}
    set xpos $EMSegment(Graph,Xpos,$Xval,2)
    if {$Yval == "-"} {return}
    set ypos $EMSegment(Graph,Ypos,$Yval,2)
    $EMSegment(Cl-caGraph2) coords GrayYCross $xpos [expr $ypos-2]  $xpos [expr $ypos+3]  
    $EMSegment(Cl-caGraph2) coords GrayXCross [expr $xpos-2] $ypos [expr $xpos+3] $ypos 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentEnterSample 
# Sets up everything when the moouse enters the graph with the image to be sampled
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEnterSample {x y} {
    global EMSegment
    set SampleList [EMSegmentReadGreyValue $x $y]
    EMSegmentDisplaySampleLogValues $SampleList
    set ValList [EMSegmentCreateRedLine $SampleList 0]
    if {$EMSegment(NumGraph) > 1} {
    lappend ValList [EMSegmentCreateRedLine $SampleList 1]
    if {$EMSegment(NumGraph) > 2} {
        EMSegmentCreateRedCross $ValList
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateRedLine 
# Creates a red line on the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateRedLine {SampleList num} {
    global EMSegment
    set index $EMSegment(Cattrib,0,Label,$num) 
    if {$index != "" } {
    set val [expr [lindex [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $index]] 3] - $EMSegment(Graph,Xmin,$num)]
    if {$val < 0} {
        set val 0
    } else {
        set MaxVal [expr $EMSegment(Graph,Xmax,$num) - $EMSegment(Graph,Xmin,$num)]
        if {$val > $MaxVal} {
        set val $MaxVal
        }
    }
    set xpos $EMSegment(Graph,Xpos,$val,$num) 
    $EMSegment(Cl-caGraph$num) create line $xpos [expr $EMSegment(Graph,Ylen,$num) + $EMSegment(Graph,YboU)] $xpos $EMSegment(Graph,YboU) -width 1  -tag GrayLine -fill red
    return $val
    }
    return "-"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateRedCross 
# Creates a red cross on the 2D graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateRedCross {ValList} {
    global EMSegment
    set Xval [lindex $ValList 0]
    set Yval [lindex $ValList 0]
    if {$Xval == "-"} {return}
    set xpos $EMSegment(Graph,Xpos,$Xval,2)
    if {$Yval == "-"} {return}
    set ypos $EMSegment(Graph,Ypos,$Yval,2)
    $EMSegment(Cl-caGraph2) create line $xpos [expr $ypos-2]  $xpos [expr $ypos+3] -width 1 -tag GrayYCross -fill red
    $EMSegment(Cl-caGraph2) create line [expr $xpos-2] $ypos [expr $xpos+3] $ypos  -width 1 -tag GrayXCross -fill red
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentLeaveSample 
# Destroys everything after the mouse leaves the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentLeaveSample { } {
    global EMSegment
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    $EMSegment(Cl-caGraph$i) itemconfigure GrayT0 -text ""
    if {$i > 1} {
        $EMSegment(Cl-caGraph$i) itemconfigure GrayT1 -text ""
        $EMSegment(Cl-caGraph$i) delete GrayXCross
        $EMSegment(Cl-caGraph$i) delete GrayYCross
    } else {
        $EMSegment(Cl-caGraph$i) delete GrayLine
    }
    }    
} 

#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeVolumeGraph
# Changes the Volume of the graph for which everything is plotted
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeVolumeGraph {VolumeID index} {
    global EMSegment
    set VolName [Volume($VolumeID,node) GetName]
    set EMSegment(Cattrib,0,Label,$index) $VolumeID
    if {$EMSegment(SegmentMode)} {
    $EMSegment(Cl-mbGraphHistogram$index) configure -text $VolName
    set maxiter  [expr $EMSegment(NumClasses)+1]
    } else {
    set maxiter 1
    $EMSegment(Cl-bGraphHistogram$index) configure -text $VolName
    }
    for {set i 0} {$i < $maxiter} {incr i} {
    if {$EMSegment(Cattrib,$i,Graph,$index) } {
        if {[EMSegmentCalculateClassCurveRegion $i $index] < 0} {return}
        EMSegmentDrawClassCurveRegion $i $index
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraphButton
# Creates for Class <Sclass> a Button so the class distribution can be diplayed 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphButton {Sclass Label Color} {
    global EMSegment Gui
    if {$Sclass == 0 } {
    set TooltipText "Press button to display histogram of current active volume" 
    } else {
    set TooltipText "Press button to display Gaussian of class $Sclass" 
    }

    if {$Sclass < 5} {
    set f $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass
    } else {
    set f $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass
    }
    eval {button $f -text $Label -width 3 -command "EMSegmentMultipleDrawDeleteCurveRegion $Sclass"} $Gui(WBA)
    $f configure -bg $Color -activebackground $Color
    pack $f -side left -padx $Gui(pad)
    TooltipAdd $f $TooltipText
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {$EMSegment(Cattrib,$Sclass,Graph,$i) == 1} {
        set EMSegment(Cattrib,$Sclass,Graph,$i) 0
        if {[EMSegmentDrawDeleteCurveRegion $Sclass $i] < 1} {return}
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentMultipleDrawDeleteCurveRegion
# Draws or delete curves/regions in all exisitng graphs 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentMultipleDrawDeleteCurveRegion {Sclass} {
    global EMSegment
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {[EMSegmentDrawDeleteCurveRegion $Sclass $i] < 1} {return}
    }
}
 
#-------------------------------------------------------------------------------
# .PROC EMSegmentDeleteGraphButton
# Delete Grpah Button and curve for Class <Sclass> 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDeleteGraphButton {Sclass} {
    global EMSegment
    # Delete Button
    if {$Sclass < 5} { 
    destroy $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass 
    } else {
    destroy $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass 
    }
    # Delete Line 
     for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
          if {$EMSegment(Cattrib,$Sclass,Graph,$i) == 1} {
             $EMSegment(Cl-caGraph$i) delete Graph$Sclass
     }
     }
 }

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawDeleteCurveRegion
# Depending if the graph for the class <Sclass> exist it deletes it or 
# otherwise cretes a new one
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawDeleteCurveRegion {Sclass NumGraph} {
    global EMSegment Volume
    if {$EMSegment(Cattrib,$Sclass,Graph,$NumGraph) == 1} {
    set EMSegment(Cattrib,$Sclass,Graph,$NumGraph) 0
    # Delete Line and raise button
    if {$Sclass > 0} {
        $EMSegment(Cl-caGraph$NumGraph) delete Graph$Sclass
        if {$NumGraph > 1} {
        set NumActiveRegion 0
        set i 1
        while {($i <= $EMSegment(NumClasses)) && ($NumActiveRegion <1) } {
            if {$EMSegment(Cattrib,$i,Graph,$NumGraph) == 1 } {
            set NumActiveRegion 1 
            }
            incr i
        }
        if {$NumActiveRegion > 0} {
            EMSegmentDrawRegion $Sclass $NumGraph 2  
        } else {
            # Reset Values
            set Xmin $EMSegment(Graph,Xmin,0)
            set Xmax [expr $EMSegment(Graph,Xmax,0)+1]
            set Ymin $EMSegment(Graph,Xmin,1)
            set Ymax [expr $EMSegment(Graph,Xmax,1)+1]

            for {set x $Xmin} {$x < $Xmax} {incr x} {
            for {set y $Ymin} {$y < $Ymax} {incr y} {
                set EMSegment(Graph,MAP,$NumGraph,$x,$y) 0
            }
            }
        }
        }
        if {$Sclass < 5} {
        $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief raised
        } else {
        $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -relief raised
        }
    } else {
        $EMSegment(Cl-caGraph$NumGraph) delete Graph$Sclass
        if  {$EMSegment(SegmentMode) > 1} {
        # $EMSegment(Cl-bGraphHistogram$NumGraph) configure -text $VolName
        $EMSegment(Cl-mbGraphHistogram$NumGraph) configure -relief raised
        } else {
        $EMSegment(Cl-bGraphHistogram$NumGraph) configure -relief raised
        }
    }
    EMSegmentRescaleGraph $EMSegment(Graph,Xmin,$NumGraph) $EMSegment(Graph,Xmax,$NumGraph) $EMSegment(Graph,Xsca,$NumGraph) $NumGraph 0
    } else {
    # Draw Graph and lower button
    set result 0
    if {$Sclass > 0} {
        if {$EMSegment(NumInputChannel) == 0} {
            DevErrorWindow "Before triing to plot a graph please do the following :\n 1.) Define volume(s) to be segmented \n 2.) Define which volume's parameters to plot by selecting one at the yellow graph button"
            return -1
        } 
        if {$NumGraph < 2} {
        set index $EMSegment(Cattrib,0,Label,$NumGraph)
        if { $index == "" } {
            DevErrorWindow "Please define which volume's parameters to plot for graph [expr $NumGraph+1] by selecting one at the yellow graph button"
            return -1
        }
        set index [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Cattrib,0,Label,$NumGraph)]
        if {($EMSegment(Cattrib,$Sclass,LogMean,$index) < 0) || ($EMSegment(Cattrib,$Sclass,LogCovariance,$index,$index) < 0)} { 
            DevErrorWindow "Before graph can be plotted Mean and Covariance have to be defined for Volume [Volume($index,node) GetName]!"
            return -1
        }
        } 
        # If the current selected graph is the active class we have to recalculate mean and covariance
        if {$Sclass  == $EMSegment(Class)} { 
        set result [EMSegmentCalculateClassMeanCovariance]
        }
    } 
    if {[EMSegmentCalculateClassCurveRegion $Sclass $NumGraph]  < 0} {return -1}
    set EMSegment(Cattrib,$Sclass,Graph,$NumGraph) 1
    set result [EMSegmentCalcProb]
    if {$result == 0} {EMSegmentDrawClassCurveRegion $Sclass $NumGraph}
    if {$Sclass > 0} {
        if {$Sclass < 5} { 
        $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief sunken
        } else {
        $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -relief sunken
        }
    } else {
        if  {$EMSegment(SegmentMode) > 1} {
        $EMSegment(Cl-mbGraphHistogram$NumGraph) configure -relief raised
        } else {
        $EMSegment(Cl-bGraphHistogram$NumGraph) configure -relief sunken 
        }
    }
    }
    # puts "Leave EMSegmentDrawDeleteCurveRegion $Sclass $NumGraph"
    return 1
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassCurveRegion 
# Calculates the gaussian distribution of class Sclass for the 1D Graph and 2D 
# YFlag is only important for 2D Case
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassCurveRegion {Sclass graphnum} {
    global EMSegment Volume
    # puts "EMSegmentCalculateClassCurveRegion $Sclass $graphnum"

    if {$graphnum < 2} {
      # --------------------------------------------------
      # 1D Case: Calculate Curves 
      # --------------------------------------------------

      # 1.) Check Values 
      if {$EMSegment(Cattrib,0,Label,$graphnum)  == "" } {
        DevErrorWindow "Please select a volume at the yellow graph button before triing to plot a histogram!"
        return -1
      }
      if {$Sclass} {   
        # Stores the Flag asssignemnt for the 2D Graph
        set NumIndex [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Cattrib,0,Label,$graphnum)]
        set AxisFlag [expr ($EMSegment(Graph,$Sclass,LogCov,$graphnum) == $EMSegment(Cattrib,$Sclass,LogCovariance,$NumIndex,$NumIndex)) && \
                       ($EMSegment(Graph,$Sclass,LogMean,$graphnum)  == $EMSegment(Cattrib,$Sclass,LogMean,$NumIndex))]
      } else {
        set AxisFlag  0
      }
    
      if {[expr ($EMSegment(Graph,$Sclass,XMin,$graphnum) == $EMSegment(Graph,Xmin,$graphnum)) && \
        ($EMSegment(Graph,$Sclass,XMax,$graphnum) == $EMSegment(Graph,Xmax,$graphnum))]} {
          if {$Sclass > 0} {
            if {$AxisFlag} {return 0}
          } else {
            # Histogram has been already calculated for this Volume
            if { $EMSegment(Graph,$Sclass,VolumeID,$graphnum) == $EMSegment(Cattrib,0,Label,$graphnum)} {
              return 0
          }
        }
      } 

      # 2.) Calculate Curve
      if  {$Sclass == 0} {
        
        set EMSegment(Graph,$Sclass,XMin,$graphnum)  $EMSegment(Graph,Xmin,$graphnum)
        set EMSegment(Graph,$Sclass,XMax,$graphnum)  $EMSegment(Graph,Xmax,$graphnum)
        set EMSegment(Graph,$Sclass,YMax,$graphnum) 0  
    
        # --------------------------------------------------
        # Curve to be calculated is the Histogram
        # --------------------------------------------------
        set EMSegment(Graph,$Sclass,VolumeID,$graphnum)  $EMSegment(Cattrib,0,Label,$graphnum)
        
        vtkImageAccumulate histogram
        # --------------------------------------------------
        # Define Settings for Histogram
        # --------------------------------------------------
        histogram SetInput [Volume($EMSegment(Cattrib,0,Label,$graphnum),vol) GetOutput]
        
        set data [histogram GetInput]
        $data Update
        
        set inputRange [[[$data GetPointData] GetScalars] GetRange]
        set origin [lindex $inputRange 0]
        set numBins [lindex $inputRange 1]
        
        # set spacing [expr 1.0 * ([lindex $inputRange 1] - $origin) / $numBins]
        # histogram SetComponentSpacing $spacing 1.0 1.0
        histogram SetComponentExtent 0 $numBins 0 0 0 0
        histogram SetComponentOrigin $origin 0.0 0.0
        histogram SetComponentSpacing 1.0 1.0 1.0
        
        # --------------------------------------------------
        # Actually get Histogram
        # --------------------------------------------------
        set data [histogram GetOutput]
        $data Update
        
        set histSum 0 
        # Xrange of Histogram
        set XhistRangeMin [lindex $inputRange 0]
        set XhistRangeMax $numBins 
        # Yrange of Histogram
        set histRange [[[$data GetPointData] GetScalars] GetRange]
        
        # --------------------------------------------------
        # Read values from Histogram 
        # --------------------------------------------------
        if {$EMSegment(Graph,Xmin,$graphnum) > $XhistRangeMin} {
          set XLower $EMSegment(Graph,Xmin,$graphnum) 
        } else {
           set XLower $XhistRangeMin 
           for {set i $EMSegment(Graph,Xmin,$graphnum)} { $i < $XhistRangeMin} {incr i} {
              set EMSegment(Graph,$Sclass,$i,$graphnum) 0
           }
        }
        
        if {$EMSegment(Graph,Xmax,$graphnum) > $XhistRangeMax} {
          set XUpper $XhistRangeMax
          for {set i [expr $XhistRangeMax + 1]} { $i <= $EMSegment(Graph,Xmax,$graphnum)} {incr i} {
            set EMSegment(Graph,$Sclass,$i,$graphnum) 0
          }
        } else {
          set XUpper $EMSegment(Graph,Xmax,$graphnum)
        }
        
        incr XUpper
        for {set idx $XLower} {$idx < $XUpper} { incr idx} {
          set EMSegment(Graph,$Sclass,$idx,$graphnum) [$data GetScalarComponentAsFloat $idx 0 0 0]
        if {$EMSegment(Graph,$Sclass,$idx,$graphnum) > $EMSegment(Graph,$Sclass,YMax,$graphnum) } {
            set EMSegment(Graph,$Sclass,YMax,$graphnum) [expr double($EMSegment(Graph,$Sclass,$idx,$graphnum))]
        }
        }
        if {$EMSegment(Graph,Ymax,$graphnum) == 0 } {
        set EMSegment(Graph,Ymax,$graphnum) 1.0
        }
        for {set idx $XLower} {$idx < $XUpper} { incr idx} {
        set EMSegment(Graph,$Sclass,$idx,$graphnum) [expr  $EMSegment(Graph,$Sclass,$idx,$graphnum) * $EMSegment(Graph,Ymax,$graphnum) / double($EMSegment(Graph,$Sclass,YMax,$graphnum))]
        }
        set EMSegment(Graph,$Sclass,YMax,$graphnum) $EMSegment(Graph,Ymax,$graphnum)
        histogram Delete
        return 1
      }
      # --------------------------------------------------
      # Curve to be calculated is a Gaussian distribution 
      # --------------------------------------------------
      # Gaus Value at the origin of the Coordinate system
      set EMSegment(Graph,$Sclass,LogCov,$graphnum)   $EMSegment(Cattrib,$Sclass,LogCovariance,$NumIndex,$NumIndex) 
      set EMSegment(Graph,$Sclass,LogMean,$graphnum)  $EMSegment(Cattrib,$Sclass,LogMean,$NumIndex)
    
      set border [EMSegmentDefineCalculationBorder $EMSegment(Graph,Xmin,$graphnum) $EMSegment(Graph,Xmax,$graphnum) $EMSegment(Graph,$Sclass,XMin,$graphnum) \
        $EMSegment(Graph,$Sclass,XMax,$graphnum) $AxisFlag]

      if {[lindex $border 2] > -1} {
        if {[lindex $border 0] > -1} {EMSegmentCalculateClassGaussianDistribution $Sclass $graphnum [lindex $border 0] [lindex $border 1] }
        EMSegmentCalculateClassGaussianDistribution $Sclass $graphnum [lindex $border 2] [lindex $border 3] 
      }
    
      set EMSegment(Graph,$Sclass,XMin,$graphnum)  $EMSegment(Graph,Xmin,$graphnum)
      set EMSegment(Graph,$Sclass,XMax,$graphnum)  $EMSegment(Graph,Xmax,$graphnum)
      # Define Maximum value of current curve
      if [expr $EMSegment(Graph,$Sclass,LogMean,$graphnum) >= log($EMSegment(Graph,Xmin,$graphnum)+1)]  {
        if [expr $EMSegment(Graph,$Sclass,LogMean,$graphnum) <= log($EMSegment(Graph,Xmax,$graphnum)+1)]  {
        # 1. Case: Globale Maximum is inside the interval
        set index [expr round(exp($EMSegment(Graph,$Sclass,LogMean,$graphnum)) - 1.0)]
        set EMSegment(Graph,$Sclass,YMax,$graphnum) $EMSegment(Graph,$Sclass,$index,$graphnum)
        } else {
        # 2. Case: Globale Maximum is to the right of the interval
        set EMSegment(Graph,$Sclass,YMax,$graphnum) $EMSegment(Graph,$Sclass,$EMSegment(Graph,Xmax,$graphnum),$graphnum)
        }
      }  else {
        # 3. Case: Globale Maximum is to the left of the interval
        set EMSegment(Graph,$Sclass,YMax,$graphnum) $EMSegment(Graph,$Sclass,$EMSegment(Graph,Xmin,$graphnum),$graphnum)
      }
      # Update Graph Values 
      # Caluclation Error Sigma to small
      if {$EMSegment(Graph,$Sclass,YMax,$graphnum) == 0} {set EMSegment(Graph,$Sclass,YMax,$graphnum) 0.1} 
      return 1
    } 
    # --------------------------------------------------
    # 2D Case: Calculate Region 
    # --------------------------------------------------

    # --------------------------------------------------
    #     1.)  Check Values 
    # --------------------------------------------------
    if  {$Sclass == 0} {return 1}
    # a.) Find out XIndex and YIndex = the Volumes currently selected by the yellow buttons 
    if {($EMSegment(Cattrib,0,Label,0)  == "") || ($EMSegment(Cattrib,0,Label,1) == "") } {
      DevErrorWindow "Please select a volume at the yellow graph button before triing to plot a histogram!"
      return -1
    }
    if {$EMSegment(Cattrib,0,Label,0) == $EMSegment(Cattrib,0,Label,1)} {
    # Do not calculate anything
    return 1
    }
    set VolXIndex [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Cattrib,0,Label,0)]
    set VolYIndex [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Cattrib,0,Label,1)]
    # b.) Calculate Determinant of the Log Covariance Matrix and the Inverse Covariance 
    set pi 3.141592653
    set DetLogCov  [expr $EMSegment(Cattrib,$Sclass,LogCovariance,$VolXIndex,$VolXIndex)*$EMSegment(Cattrib,$Sclass,LogCovariance,$VolYIndex,$VolYIndex) -\
        $EMSegment(Cattrib,$Sclass,LogCovariance,$VolXIndex,$VolYIndex)*$EMSegment(Cattrib,$Sclass,LogCovariance,$VolYIndex,$VolXIndex) ]
    if {$DetLogCov < 0.0000001} {
    DevErrorWindow "Covariance Matrix of class $EMSegment(Cattrib,$Sclass,Label) is not positiv definit!"
    return -2
    }
    set InvLogCov(1,1) [expr $EMSegment(Cattrib,$Sclass,LogCovariance,$VolYIndex,$VolYIndex) / $DetLogCov]
    set InvLogCov(1,2) [expr $EMSegment(Cattrib,$Sclass,LogCovariance,$VolXIndex,$VolYIndex) / $DetLogCov]
    set InvLogCov(2,2) [expr $EMSegment(Cattrib,$Sclass,LogCovariance,$VolXIndex,$VolXIndex) / $DetLogCov]

    # c.) Find out if anything chnaged in the class definition => If so, the whole graph has to be recalculated
    set AxisFlag [expr ($EMSegment(Graph,$Sclass,LogMean,1,$graphnum) == $EMSegment(Cattrib,$Sclass,LogMean,$VolXIndex)) && ($EMSegment(Graph,$Sclass,LogMean,2,$graphnum) == $EMSegment(Cattrib,$Sclass,LogMean,$VolYIndex)) &&  ($EMSegment(Graph,$Sclass,InvLogCov,1,1,$graphnum) == $InvLogCov(1,1)) && ($EMSegment(Graph,$Sclass,InvLogCov,1,2,$graphnum) == $InvLogCov(1,2)) && ($EMSegment(Graph,$Sclass,InvLogCov,2,2,$graphnum) == $InvLogCov(2,2)) && ($EMSegment(Graph,$Sclass,XcaL,$graphnum) == $EMSegment(Graph,XcaL,$graphnum)) && ($EMSegment(Graph,$Sclass,YcaL,$graphnum) == $EMSegment(Graph,YcaL,$graphnum))] 

    # d.) Check on range 
    if {$AxisFlag} {
    if {[expr ($EMSegment(Graph,$Sclass,XMin,$graphnum) == $EMSegment(Graph,Xmin,$graphnum)) && ($EMSegment(Graph,$Sclass,XMax,$graphnum) == $EMSegment(Graph,Xmax,$graphnum))]} {return 0}
    }

    # --------------------------------------------------
    #     2.)  Define Values 
    # --------------------------------------------------
    set EMSegment(Graph,$Sclass,LogMean,1,$graphnum) $EMSegment(Cattrib,$Sclass,LogMean,$VolXIndex) 
    set EMSegment(Graph,$Sclass,LogMean,2,$graphnum) $EMSegment(Cattrib,$Sclass,LogMean,$VolYIndex)
    set EMSegment(Graph,$Sclass,InvLogCov,1,1,$graphnum) $InvLogCov(1,1) 
    set EMSegment(Graph,$Sclass,InvLogCov,1,2,$graphnum) $InvLogCov(1,2) 
    set EMSegment(Graph,$Sclass,InvLogCov,2,2,$graphnum) $InvLogCov(2,2) 
    set Norm [expr 0.5/($pi*sqrt($DetLogCov))]

    # Find out if we have to recalculate the curve
    # Calculate Zero - Borders
    # Leave Zero away figures do not look good
    # set SqrtDetLogCov [expr sqrt($DetLogCov)]
    # set XZero [EMSegmentCalcGaussian2DExtrem $graphnum $Sclass $SqrtDetLogCov $pi 0]
    set XZeroMin 1.0 
    set XZeroMax 1.0 
    # set YZero [EMSegmentCalcGaussian2DExtrem $graphnum $Sclass $SqrtDetLogCov $pi 1]
    set YZeroMin 1.0 
    set YZeroMax 1.0 


    set Xborder [EMSegmentDefineCalculationBorder $EMSegment(Graph,Xmin,$graphnum) $EMSegment(Graph,Xmax,$graphnum) $EMSegment(Graph,$Sclass,XMin,$graphnum) \
        $EMSegment(Graph,$Sclass,XMax,$graphnum) $AxisFlag]
    

    # --------------------------------------------------
    #     3.) Calculate Region
    # --------------------------------------------------
    #  |B|                A-D => Regions have to be calculated 
    # A|X|C               X   => Already predefined 
    #  |D|
    set XCalcUpperMiddle [lindex $Xborder 2] 
    if {$XCalcUpperMiddle > -1} {
    # If it is true we have to calculate C for sure
    set XCalcMin [lindex $Xborder 0]
    set XCalcUpperMiddle [lindex $Xborder 2] 
    if  {$XCalcMin > -1} {
        # Calculate A 
        set XCalcLowerMiddle [lindex $Xborder 1] 
        EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $XCalcMin $EMSegment(Graph,Ymin,$graphnum) $XCalcLowerMiddle $EMSegment(Graph,Ymax,$graphnum)
        set Xmin $XCalcLowerMiddle
        set Xmax $XCalcUpperMiddle
    } else {
        # Nothing has to be calculated in terms of the Y direction
        set Xmin -1
        set Xmax -1
    }
    EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $XCalcUpperMiddle $EMSegment(Graph,Ymin,$graphnum) [lindex $Xborder 3]  $EMSegment(Graph,Ymax,$graphnum)
    } else {
    set Xmin $EMSegment(Graph,Xmin,$graphnum)
    set Xmax $EMSegment(Graph,Ymin,$graphnum)
    }
 
    if {$Xmin > -1} {
    set Yborder [EMSegmentDefineCalculationBorder $EMSegment(Graph,Ymin,$graphnum) $EMSegment(Graph,Ymax,$graphnum) $EMSegment(Graph,$Sclass,YMin,$graphnum) \
        $EMSegment(Graph,$Sclass,YMax,$graphnum) $AxisFlag]

    set YCalcUpperMiddle [lindex $Yborder 2]
    if {$YCalcUpperMiddle > -1} {
        # If it is true we have to calculate B for sure
        set YCalcMin [lindex $Yborder 0]
        if  {$YCalcMin > -1} {
        # Calculate B 
        EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $Xmin $YCalcMin $Xmax [lindex $Yborder 1] 
        }  
        EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $Xmin $YCalcUpperMiddle $Xmax [lindex $Yborder 3] 
    }
    } 

    # If you want ot have it without all the border stuff this is the way to calculated - should be slower
    # EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $EMSegment(Graph,Xmin,0) $EMSegment(Graph,Xmin,1) [expr $EMSegment(Graph,Xmax,0) + 1] [expr $EMSegment(Graph,Xmax,1) + 1]  

    set EMSegment(Graph,$Sclass,XMin,$graphnum)  $EMSegment(Graph,Xmin,$graphnum)
    set EMSegment(Graph,$Sclass,XMax,$graphnum)  $EMSegment(Graph,Xmax,$graphnum)
    set EMSegment(Graph,$Sclass,XcaL,$graphnum)  $EMSegment(Graph,XcaL,$graphnum)
    set EMSegment(Graph,$Sclass,YMin,$graphnum)  $EMSegment(Graph,Ymin,$graphnum)
    set EMSegment(Graph,$Sclass,YMax,$graphnum)  $EMSegment(Graph,Ymax,$graphnum)
    set EMSegment(Graph,$Sclass,YcaL,$graphnum)  $EMSegment(Graph,YcaL,$graphnum)
   
    # --------------------------------------------------
    #     4.) Calculate Maximum
    # --------------------------------------------------
    # Define Maximum value of current curve
    # Find out if Mean is inside the X/Y Axis retirctions or outside
    # set XExtIndex [EMSegmentGlobalGuassianInside $Sclass 0]
    # set YExtIndex [EMSegmentGlobalGuassianInside $Sclass 0]

    # Calculate Maqximum
    #if {$XExtIndex != ""} {
    # Set Closed Grid
    #    if {$YExtIndex != ""} {
        # Case: Globale Maximum is outside both intervals 
    #        set EMSegment(Graph,$Sclass,ZMax,$graphnum) [EMSegmentFind2LineMaximum $Sclass $graphnum $EMSegment(Graph,$XExtIndex,0) $EMSegment(Graph,$XExtIndex,1)] 
    #    } else {
            # Case: Globale Maximum is only outside the x interval
    #        set EMSegment(Graph,$Sclass,YZMax,$graphnum) [EMSegmentFindLineMaximum $graphnum $Sclass $EMSegment(Graph,$XExtIndex,0) 1] 
    #    }
    #} elseif {$YExtIndex != ""} {
    # Case: Globale Maximum is only outside the y interval     
    #    set EMSegment(Graph,$Sclass,YZMax,$graphnum) [EMSegmentFindLineMaximum $graphnum $Sclass $EMSegment(Graph,$YExtIndex,1) 0] 
    # } else {
    # Case: Globale Maximum is inside the interval
    #    set Xindex [expr round(exp($EMSegment(Graph,$Sclass,LogMean,0)) - 1.0)]
    #    set XindexGrid [EMSegmentGraphGridCalculate $graphnum $Xindex 0]
    #    set Yindex [expr round(exp($EMSegment(Graph,$Sclass,LogMean,1)) - 1.0)]
    #    set YindexGrid [EMSegmentGraphGridCalculate $graphnum $Yindex 1]
    #    set EMSegment(Graph,$Sclass,ZMax,$graphnum) $EMSegment(Graph,$Sclass,$graphnum,$XindexGrid,$YindexGrid)
    # }

    # Update Graph Values 
    # Caluclation Error Sigma to small
    # if {$EMSegment(Graph,$Sclass,YMax,$graphnum) == 0} {set EMSegment(Graph,$Sclass,YMax,$graphnum) 0.1} 
    return 1
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentGlobalGuassianInside 
# Find out if Mean value is inside or outside the axis => We can find out about 
# global Maximum
# Return Value  
# 0 => Mean is inside the interval of the *-Axis
# 1 => Mean is to the left/below the interval of the *-Axis
# 2 => Mean is to the right/above the interval of the *-Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentGlobalGuassianInside {Sclass Axis} {
    global EMSegment
    if [expr $EMSegment(Graph,$Sclass,LogMean,$Axis) < log($EMSegment(Graph,Xmin,$Axis)+1)]  {return "Xmin"} 
    if [expr $EMSegment(Graph,$Sclass,LogMean,$Axis) > log($EMSegment(Graph,Xmax,$Axis)+1)]  {return "Xmax"}
    return ""
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentFindLineMaximum 
# Find the Maximum Along the Line (x/y = SetValue):
# Axis :
# 0 = along X-Axis => Y Value is fixed 
# 1 = along Y-Axis => X Value is fixed
# Returns the value where the gaussian distribution takes the maximum value 
# alonge the axis 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentFindLineMaximum {GraphNum Sclass SetVal Axis} {
    global EMSegment
    # a = fixed value , b = set value
    set SetVal [EMSegmentGraphGridCalculate $GraphNum $SetVal $Axis] 
    set a [expr 1 + $Axis] 
    set b [expr 2 - $Axis] 
    # val is the log value 
    set val [expr $EMSegment(Graph,$Sclass,LogMean,$a,$GraphNum) - $EMSegment(Graph,$Sclass,InvLogCov,1,2,$GraphNum) / $EMSegment(Graph,$Sclass,InvLogCov,$a,$a,$GraphNum) * (log($SetVal+1) \
        - $EMSegment(Graph,$Sclass,LogMean,$b,$GraphNum))]
    # Now we recover the int grey value
    set val [expr round(exp($val) - 1.0)]
    if {$val < $EMSegment(Graph,Xmin,$Axis)} {set val $EMSegment(Graph,Xmin,$Axis)
    } elseif { $val > $EMSegment(Graph,Xmax,$Axis)} {set val $EMSegment(Graph,Xmax,$Axis) }
    # Now calculate the column it is closest to 
    set val [EMSegmentGraphGridCalculate $GraphNum $val $Axis]
    if {$Axis} {
    return $EMSegment(Graph,$Sclass,$GraphNum,$SetVal,$val) 
    } 
    return $EMSegment(Graph,$Sclass,$GraphNum,$val,$SetVal) 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentFind2LineMaximum 
# Find the Maximum Along a fixed X and Y Axis and return result
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentFind2LineMaximum {Sclass GraphNum XFix YFix} {
    global EMSegment
    # Now we have to check to cases -> along the X and Y axis
    # 1.) Check along the X Axis
    set XVal [EMSegmentFindLineMaximum $GraphNum $Sclass $YFix 0] 
    # 2.) Check along the Y Axis 
    set YVal [EMSegmentFindLineMaximum $GraphNum $Sclass $XFix 1] 
    if {$XVal > $YVal} {
    return $XVal 
    } 
    return $YVal 
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCheckClassCurve
# Check values of graph defintion before calculating class curve
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCheckClassCurve {graphnum Sclass IndexGraphNum AxisFlag} {
    global EMSegment
    
    if {[expr ($EMSegment(Graph,$Sclass,XMin,$graphnum) == $EMSegment(Graph,Xmin,$graphnum)) && \
        ($EMSegment(Graph,$Sclass,XMax,$graphnum) == $EMSegment(Graph,Xmax,$graphnum))]} {
    if {$Sclass > 0} {
        if {$AxisFlag} {return 0}
    } else {
        # Histogram has been already calculated for this Volume
        if { $EMSegment(Graph,$Sclass,VolumeID,$graphnum) == $index} {
        return 0
        }
    }
    } 
    return $index
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentGraphGridCalculate 
# Calculates the closest row or column to a point  
# Axis  = 0 => X Axis , 1 => Y Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentGraphGridCalculate {GraphNum Value Axis } {
    global EMSegment
    if {$Axis} {
    set UnitLength $EMSegment(Graph,YcaL,$GraphNum)
    } else {
    set UnitLength $EMSegment(Graph,XcaL,$GraphNum)
    }
    # Do Not Change it to $EMSegment(Graph,Xmin,$GraphNum) otherwise we have problems when calculating the region !!! Value is not set yet !!
    set grid [expr $UnitLength * round(double($Value - $EMSegment(Graph,Xmin,$Axis)) / double($UnitLength)) + $EMSegment(Graph,Xmin,$Axis)]
    if {$grid > $EMSegment(Graph,Xmax,$Axis)} {
        return [expr $UnitLength * int(double($EMSegment(Graph,Xmax,$Axis) - $EMSegment(Graph,Xmin,$Axis)) / double($UnitLength)) + $EMSegment(Graph,Xmin,$Axis)]
    } elseif {$grid < $EMSegment(Graph,Xmin,$Axis)} {return $EMSegment(Graph,Xmin,$Axis)}
    return $grid
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDefineCalculationBorder 
# Defines the borders where Curve has to be recalcalculated. There are three 
# scenarios :
# 1.Case: BorLowerLeft == BorUpperLeft == BorLowerRight == BorUpperRight == -1 
#         => Nothing to calculate
# 2.Case: BorLowerLeft == BorUpperLeft == -1 
#         => Only Calculate in [BorLowerRight,BorUpperRight] 
# 3.Case: BorLowerLeft != -1 != BorUpperLeft  
#         => Calculate in [BorLowerLeft,BorUpperLeft], [BorLowerRight,BorUpperRight] 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc  EMSegmentDefineCalculationBorder {MinNew MaxNew MinOld MaxOld flag} {
    # Depending on how the x-axis has been changed we do not recalculate everything
    # Default - everything has to be recalculated
    set BorLowerLeft -1
    set BorUpperLeft -1
    set BorLowerRight $MinNew 
    set BorUpperRight $MaxNew 
    # puts "EMSegmentDefineCalculationBorder $MinNew $MaxNew $MinOld $MaxOld $flag"
    if $flag {
    # Just the Axis are changed
    # Find out if they have joint areas that do not have to be recalculated 
    if {$MinOld <= $MinNew} {
        if {$MaxOld > $MinNew} {
        if {$MaxOld >= $MaxNew} {
            # 1. Case: We do not have to calculate anything because new definition is an inclusion of the old
            set BorLowerRight -1 
            set BorUpperRight -1
        } else {
            # 2. Case: The curve got longer - we have to calculate an upper area
            set BorLowerRight $MaxOld 
        }
        } 
    } elseif {$MinOld < $MaxNew} {
        if {$MaxOld <= $MaxNew} {
        # 3. Case: Have to calculate upper and lower area
        set BorLowerLeft  $MinNew 
        set BorUpperLeft  $MinOld
        set BorLowerRight $MaxOld 
        } else {
        # 2.Case: We only have to calculate upper area
        set BorLowerRight $MinNew 
        set BorUpperRight $MinOld 
        }     
    } 
    }
    # puts "EMSegmentDefineCalculationBorder leave $BorLowerLeft $BorUpperLeft $BorLowerRight $BorUpperRight" 
    return "$BorLowerLeft $BorUpperLeft $BorLowerRight $BorUpperRight" 
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassGaussianDistribution
# Calucates the gaussian distribution 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassGaussianDistribution {Sclass graphnum Min Max } {
    global EMSegment
    set pi 3.141592653
    if {$EMSegment(Graph,$Sclass,LogCov,$graphnum) == 0} {
    for {set i $Min} {$i < $Max} { incr i} {
        set EMSegment(Graph,$Sclass,$i,$graphnum) 0.0 
    }
    set mean [expr int(exp($EMSegment(Graph,$Sclass,LogMean,$graphnum))) - 1]
    if {($Min <= $mean) && ($Max >= $mean)} {
        set EMSegment(Graph,$Sclass,$mean,$graphnum) 1.0
    }
    return
    }
    set norm [expr sqrt(0.5/($pi*$EMSegment(Graph,$Sclass,LogCov,$graphnum)))]
    incr Max
    for {set i $Min} {$i < $Max} { incr i} {
    set term [expr log($i+1.0)-$EMSegment(Graph,$Sclass,LogMean,$graphnum)]
    set EMSegment(Graph,$Sclass,$i,$graphnum) [expr $norm*exp(-0.5*$term*$term/$EMSegment(Graph,$Sclass,LogCov,$graphnum))]
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassGaussian2DDistribution
# Calucates the gaussian distribution 
# XZeroMin YZeroMin XZeroMax YZeroMax are the inner borders for which rectangular 
# Zero has to be calculated => x < XZeroMin or x > XZeroMax => f(x,y) = 0 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassGaussian2DDistribution {graphnum Sclass Norm XMin YMin XMax YMax} {
    global EMSegment 
    # puts "EMSegmentCalculateClassGaussian2DDistribution $graphnum $Sclass $Norm $XMin $YMin $XMax $YMax"
    # Already fill region with Zero
    # set Border [EMSegmentSetRegionToZero $graphnum $Sclass $XMin $YMin $XMax $YMax $XZeroMin $YZeroMin $XZeroMax $YZeroMax]
    # set XMin [lindex $Border 0]
    # if {$XMin < 0 } { return}

    # Define New Borders to calculate Gauss Values
    set XMin [EMSegmentGraphGridCalculate $graphnum $XMin 0]
    set YMin [EMSegmentGraphGridCalculate $graphnum $YMin 1]
    incr XMax 
    incr YMax 

    set Xterm [expr log($XMin+1.0) - $EMSegment(Graph,$Sclass,LogMean,1,$graphnum)]
    for {set y $YMin} {$y < $YMax} {incr y $EMSegment(Graph,YcaL,$graphnum)} {
    set Yterm($y) [expr log($y+1.0) - $EMSegment(Graph,$Sclass,LogMean,2,$graphnum)]
    set YLogCov($y) [expr $Yterm($y)*$Yterm($y)*$EMSegment(Graph,$Sclass,InvLogCov,2,2,$graphnum)]
    set term [expr $Xterm*($EMSegment(Graph,$Sclass,InvLogCov,1,1,$graphnum)*$Xterm + 2.0*$EMSegment(Graph,$Sclass,InvLogCov,1,2,$graphnum)*$Yterm($y)) + $YLogCov($y)]
    # Be carful different between 1D and 2D Case 
    set EMSegment(Graph,$Sclass,$graphnum,$XMin,$y) [expr $Norm*exp(-0.5*$term)]
    }
    incr XMin $EMSegment(Graph,XcaL,$graphnum)
    for {set x $XMin} {$x < $XMax} {incr x $EMSegment(Graph,XcaL,$graphnum)} {
    set Xterm [expr log($x+1.0) - $EMSegment(Graph,$Sclass,LogMean,1,$graphnum)]
    for {set y $YMin} {$y < $YMax} {incr y $EMSegment(Graph,YcaL,$graphnum)} {
        set term [expr $Xterm*($EMSegment(Graph,$Sclass,InvLogCov,1,1,$graphnum)*$Xterm + 2.0*$EMSegment(Graph,$Sclass,InvLogCov,1,2,$graphnum)*$Yterm($y)) + $YLogCov($y)]
        # Be carful different between 1D and 2D Case 
        set EMSegment(Graph,$Sclass,$graphnum,$x,$y) [expr $Norm*exp(-0.5*$term)]
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSetRegionsToZero 
# Sets the defined Regions to Zero and returns new calculation intervals
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetRegionToZero {GraphNum Sclass XCalcMin YCalcMin XCalcMax YCalcMax XZeroMin YZeroMin XZeroMax YZeroMax} {
    global EMSegment
    if {$XCalcMin < $XZeroMin} {
    if {$XCalcMax < $XZeroMin} {
        # Nothing has to be calculated
        EMSegmentSetGaussRegionToZero $GraphNum $Sclass $XCalcMin $YCalcMin $XCalcMax $YCalcMax
        return "-1 -1 -1 -1"
    } 
    set Xmin $XZeroMin
    EMSegmentSetGaussRegionToZero $GraphNum $Sclass $XCalcMin $YCalcMin [expr $XZeroMin - 1] $YCalcMax 
    } else {
    set Xmin $XCalcMin
    }
    if {$XCalcMax > $XZeroMax} {
    if {$XCalcMin > $XZeroMax} {
        # Nothing has to be calculated
        EMSegmentSetGaussRegionToZero $GraphNum $Sclass $XMin $YCalcMin $XCalcMax $YCalcMax 
        return "-1 -1 -1 -1"
    } 
    set Xmax $XZeroMax
    EMSegmentSetGaussRegionToZero $GraphNum $Sclass [expr $XZeroMax +1] $YCalcMin $XCalcMax $YCalcMax
    } else {
    set Xmax $XCalcMax
    }

    if {$YCalcMin < $YZeroMin} {
    if {$YCalcMax < $YZeroMin} {
        # Nothing has to be calculated
        EMSegmentSetGaussRegionToZero $GraphNum $Sclass $Xmin $YCalcMin $Xmax $YCalcMax
        return "-1 -1 -1 -1"
    } 
    set Ymin $YZeroMin
    EMSegmentSetGaussRegionToZero $GraphNum $Sclass $Xmin $YCalcMin $Xmax [expr $YZeroMin -1]
    } else {
    set Ymin $YCalcMin
    }

    if {$YCalcMax > $YZeroMax} {
    if {$YCalcMin > $YZeroMax} {
        # Nothing has to be calculated
        EMSegmentSetGaussRegionToZero $GraphNum $Sclass $Xmin $YCalcMin $Xmax $YCalcMax
        return "-1 -1 -1 -1"
    } 
    set Ymax $YZeroMax
    EMSegmentSetGaussRegionToZero $GraphNum $Sclass $Xmin [expr $YZeroMax +1] $Xmax $YCalcMax
    } else {
    set Ymax $YCalcMax
    }
    return "$Xmin $Ymin $Xmax $Ymax"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSetGaussRegionToZero 
# Sets the defined gauss Region to Zero 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetGaussRegionToZero {graphnum Sclass XMin YMin XMax YMax} {
    global EMSegment
    set XMin [EMSegmentGraphGridCalculate $graphnum $XMin 0]
    incr XMax 
    set YMin [EMSegmentGraphGridCalculate $graphnum $YMin 1]
    incr YMax 
    for {set x $XMin} {$x < $XMax} {incr x $EMSegment(Graph,XcaL,$graphnum)} {
    for {set y $YMin} {$y < $YMax} {incr y $EMSegment(Graph,YcaL,$graphnum)} {
        # Be carful different between 1D and 2D Case 
        set EMSegment(Graph,$Sclass,$graphnum,$x,$y) 0.0
    }
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawClassCurveRegion
# Calucates the Class Curve/Region, rescales (if needed) and draws it in the graph. 
# Remember Sclass == 0 is the Histogram of current 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawClassCurveRegion {Sclass index} {
    global EMSegment
    set EMSegment(Cattrib,$Sclass,Graph,$index) 1
    if {$index < 2} {
      if {[EMSegmentRescaleGraph $EMSegment(Graph,Xmin,$index) $EMSegment(Graph,Xmax,$index) $EMSegment(Graph,Xsca,$index) $index 0] == 0 } {
         # Curve has not been painted yet 
         EMSegmentDrawSingleCurve $Sclass $index
      }
    } else {
      EMSegmentDrawRegion $Sclass $index 1
    }
}  

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawSingleCurve
# Draws a single curve in the 1D-graph. Remember Sclass == 0 is the Histogram of current 
# active Volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawSingleCurve {Sclass index} {
   global EMSegment
   # Delete old line 
   if {$EMSegment(Cattrib,$Sclass,Graph,$index) == 1} {
       $EMSegment(Cl-caGraph$index) delete Graph$Sclass
   } else {
      set EMSegment(Cattrib,$Sclass,Graph,$index) 1
   }
   if {[expr $EMSegment(Graph,DisplayProb) && ($Sclass > 0)] } {
       set prob $EMSegment(Cattrib,$Sclass,Prob)
   } else {
       set prob 1
   }
   # Draw the Line
   set Xmin $EMSegment(Graph,Xmin,$index)
   set Xlength [expr $EMSegment(Graph,Xmax,$index) - $EMSegment(Graph,Xmin,$index) + 1]
   # Graph does not display those values anymore because of side scaling 
   if {$EMSegment(Graph,Ymax,$index) >  0.001} {
       if {$Sclass > 0} {
       set YposLast [expr int((1.0 - $prob * $EMSegment(Graph,$Sclass,$Xmin,$index)/double($EMSegment(Graph,Ymax,$index))) * \
           $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU))]
       } else {
       set YposLast [expr int((1.0 - $EMSegment(Graph,0,$Xmin,$index) / double ($EMSegment(Graph,0,YMax,$index))) * \
           $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU))]
       }
       for {set i 1 } {$i < $Xlength} {incr i } {
       if {$Sclass > 0} {
           set Ypos [expr int((1.0 - $prob*$EMSegment(Graph,$Sclass,[expr $i+$Xmin],$index)/double($EMSegment(Graph,Ymax,$index))) * \
                 $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU))]
           } else {
           set Ypos [expr int((1.0 - $EMSegment(Graph,0,[expr $i+$Xmin],$index) / double ($EMSegment(Graph,0,YMax,$index))) * \
                 $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU))]
       }
       $EMSegment(Cl-caGraph$index) create line  $EMSegment(Graph,Xpos,[expr $i-1],$index) $YposLast $EMSegment(Graph,Xpos,$i,$index) $Ypos \
               -width 1 -fill $EMSegment(Cattrib,$Sclass,ColorCode) -tag Graph$Sclass
           set YposLast $Ypos
       }
   } else {
       set Ypos [expr int($EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU))]
       for {set i 1 } {$i < $Xlength} {incr i } {
       for {set j 0} { $j < $EMSegment(NumGraph)} {incr j} {
           $EMSegment(Cl-caGraph$j) create line  $EMSegment(Graph,Xpos,[expr $i-1],$index) $Ypos $EMSegment(Graph,Xpos,$i,$index) $Ypos \
               -width 1 -fill $EMSegment(Cattrib,$Sclass,ColorCode) -tag Graph$Sclass
       }
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawClassRegion
# Draws a single region in the 2D-Graph. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawClassRegion {GraphNum Class XStart XEnd Y} {
    global EMSegment
    set YCoords $EMSegment(Graph,Ypos,[expr $Y - $EMSegment(Graph,Xmin,1)],$GraphNum) 
    $EMSegment(Cl-caGraph$GraphNum) create line $EMSegment(Graph,Xpos,[expr $XStart-$EMSegment(Graph,Xmin,0)],$GraphNum) $YCoords \
        $EMSegment(Graph,Xpos,[expr $XEnd-$EMSegment(Graph,Xmin,0)],$GraphNum) $YCoords  -width $EMSegment(Graph,YcaW,$GraphNum) \
        -tag Graph$Class -fill $EMSegment(Cattrib,$Class,ColorCode)
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawRegion
# Draws a single region in the 2D-Graph. 
# Status :
# 1 => just one single Region gets added, 
# 2 => just recalculate area with Sclass (when it gets deleted from the map)
# 3 => Redraw the whole part
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawRegion {Sclass GraphNum Status} {
   global EMSegment
   #  puts "EMSegmentDrawRegion $Sclass $GraphNum $Status"
   # Remeber: We cannot display the histogram in 2D currently 
   if {$Sclass < 1} {return}
   if {$EMSegment(Cattrib,0,Label,1)  == $EMSegment(Cattrib,0,Label,0) } {return}
 
   if {$Status < 2} {set EMSegment(Cattrib,$Sclass,Graph,$GraphNum) 1}
   # Find out which Classes are active 
   set ActiveList ""
   set ProbList ""
   set WholeProbList {0}
   set ProbMin 1.0 
   for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
       if {$EMSegment(Cattrib,$i,Graph,$GraphNum) == 1 } {
       lappend ActiveList $i
       if {$EMSegment(Graph,DisplayProb) } {  
           lappend ProbList $EMSegment(Cattrib,$i,Prob) 
           if {$ProbMin > $EMSegment(Cattrib,$i,Prob) } {
                   set ProbMin $EMSegment(Cattrib,$i,Prob)
           }
       } else {
           lappend ProbList 1.0 
       }
       }
       if {$EMSegment(Graph,DisplayProb) } {  
       lappend WholeProbList $EMSegment(Cattrib,$i,Prob) 
       } else {
       lappend WholeProbList 1.0 
       }
   }
   # Delete old Setting
   # Right Now redraw the whole graph 
   if {[llength $ActiveList] == 0} {return}
   if {$Status != 2} {
       foreach elem $ActiveList {
       $EMSegment(Cl-caGraph$GraphNum) delete Graph$elem
       }
   }
   set InitElem [lindex $ActiveList 0]
   set ActiveList [lrange $ActiveList 1 end]
   set InitProb [lindex $ProbList 0]
   set ProbList [lrange $ProbList 1 end]
   # Do we use Probability for our calculation or not 
   if {$EMSegment(Graph,DisplayProb)  } {
       set ClassProb $EMSegment(Cattrib,$Sclass,Prob)
   } else {
       set ClassProb 1.0
   }
   # Recalculate MAP and draw the point
   set Xmin $EMSegment(Graph,Xmin,$GraphNum)
   set Xmax [expr $EMSegment(Graph,Xmax,$GraphNum)+1]

   set Ymin $EMSegment(Graph,Ymin,$GraphNum)
   set Ymax [expr $EMSegment(Graph,Ymax,$GraphNum)+1]

   # ShortCut if Active Lenght is 0 no comparison needed
   set oldMAPClass $Sclass
   # set CutOff  [expr $EMSegment(Graph,MinDisplayValue,$GraphNum)*$ProbMin]
   for {set y $Ymin} {$y < $Ymax} {incr y $EMSegment(Graph,YcaL,$GraphNum)} {
       set lastMaxClass -1
       for {set x $Xmin} {$x < $Xmax} {incr x $EMSegment(Graph,XcaL,$GraphNum)} {
       # Find out if current class is bigger than older once - also lock through and through out old tissue class values 
       if {$Status < 3} { set oldMAPClass $EMSegment(Graph,MAP,$GraphNum,$x,$y)}    
       # Remember: For Status = 3 $oldMAPClass == $Sclass always !
       if {$oldMAPClass == $Sclass} {
           # we have to recalculate everything for this point and display
           set EMSegment(Graph,MAP,$GraphNum,$x,$y) $InitElem
           set MaxValue [expr $EMSegment(Graph,$InitElem,$GraphNum,$x,$y)*$InitProb]
           foreach elem $ActiveList prob $ProbList {
           if [expr $MaxValue < ($EMSegment(Graph,$elem,$GraphNum,$x,$y)*$prob)] {
               set EMSegment(Graph,MAP,$GraphNum,$x,$y) $elem
               set MaxValue [expr $EMSegment(Graph,$elem,$GraphNum,$x,$y)*$prob]
           }
           }
       } else {
           if { $Status < 2} { 
           # Just compare it with the maximum
           if {$oldMAPClass > -1} {
               set prob [lindex $WholeProbList $oldMAPClass]
               set MaxValue [expr $EMSegment(Graph,$oldMAPClass,$GraphNum,$x,$y)*$prob]
               set NewValue [expr $EMSegment(Graph,$Sclass,$GraphNum,$x,$y)*$ClassProb]
               if {$MaxValue < $NewValue} {
               set EMSegment(Graph,MAP,$GraphNum,$x,$y) $Sclass
               # set MaxValue $NewValue
               }
           } else { 
               #  set MaxValue [expr $EMSegment(Graph,$Sclass,$GraphNum,$x,$y)*$ClassProb]
               set EMSegment(Graph,MAP,$GraphNum,$x,$y) $Sclass
           }
           } else {
           # Must be Status 2 because Status 3 never gets her
           if {$lastMaxClass > -1} {
               # Have to draw line because this pixel doe not belong anymore to the class that has to be deleted 
               EMSegmentDrawClassRegion $GraphNum $lastMaxClass $XStartCoordMax $x $y  
               set lastMaxClass -1
           } 
           # set MaxValue 1.0
           }
       }
       #if {$MaxValue < $CutOff} {
       #    set EMSegment(Graph,MAP,$GraphNum,$x,$y) -1
       # }
       if {$lastMaxClass != $EMSegment(Graph,MAP,$GraphNum,$x,$y) } {
           # Finish with the last line 
           # Have to change to real position calculation 
           if {$lastMaxClass > -1} {
           EMSegmentDrawClassRegion $GraphNum $lastMaxClass $XStartCoordMax $x $y
           }
           # Have to start a new line 
           set XStartCoordMax $x
           if {$Status !=2} {
           set lastMaxClass $EMSegment(Graph,MAP,$GraphNum,$x,$y) 
           } elseif {$oldMAPClass == $Sclass} { 
           set lastMaxClass $EMSegment(Graph,MAP,$GraphNum,$x,$y) 
           }   
       }
       }
       if {$lastMaxClass > -1} {EMSegmentDrawClassRegion $GraphNum $lastMaxClass $XStartCoordMax [expr $Xmax-1] $y}
   }
   $EMSegment(Cl-caGraph$GraphNum) raise GrayT0
   $EMSegment(Cl-caGraph$GraphNum) raise GrayT1
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalulateGraphXleUXscnXscpXpos 
# It will calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn,$index), 
# EMSegment(Graph,Xscp) and EMSegment(Graph,Xpos)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalulateGraphXleUXscnXscpXpos {index} {
    global EMSegment
    # Define Length one unit in pixels
    set EMSegment(Graph,XleU,$index) [expr $EMSegment(Graph,Xlen,$index) / double($EMSegment(Graph,Xmax,$index) - $EMSegment(Graph,Xmin,$index))]
    if {$index > 1} {
    if {$EMSegment(Graph,XleU,$index) < 1.0} {
        # Define the drawing surface, i.e. how many lines are left out to be calculated for 2 Dim Graph  
        set EMSegment(Graph,XcaL,$index) [expr int(1.0 / $EMSegment(Graph,XleU,$index))]
    } else {
        # Define the drawing surface, i.e. how many lines are left out to be calculated for 2 Dim Graph  
        set EMSegment(Graph,XcaL,$index) 1
    }
    }
    # Number of scalling lines 
    set EMSegment(Graph,Xscn,$index) [expr int(($EMSegment(Graph,Xmax,$index) - $EMSegment(Graph,Xmin,$index))/double($EMSegment(Graph,Xsca,$index)))]
    # Distance between scalling lines 
    set EMSegment(Graph,Xscp,$index) [expr $EMSegment(Graph,Xlen,$index)*$EMSegment(Graph,Xsca,$index) / double($EMSegment(Graph,Xmax,$index) - $EMSegment(Graph,Xmin,$index))] 

    # Xscaling per point does not change, so I just have to caluclate it once ! Makes it a little bit speedier
    # Start with point 1 not origin ! 
    set Xlength [expr $EMSegment(Graph,Xmax,$index) - $EMSegment(Graph,Xmin,$index) + 1]
    for {set i 0 } {$i < $Xlength} {incr i } {
         set EMSegment(Graph,Xpos,$i,$index) [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,XleU,$index))]
    }
    # if {$index > 1} {puts "EMSegmentCalulateGraphXleUXscnXscpXpos $EMSegment(Graph,XleU,$index) $EMSegment(Graph,XcaL,$index) $EMSegment(Graph,Xscn,$index) $EMSegment(Graph,Xscp,$index)"}
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalulateGraphYscnYscp
# It will calulate EMSegment(Graph,Yscn) and EMSegment(Graph,Xscn,Yscp) 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalulateGraphYscnYscp {index} {
    global EMSegment
    # Number of scalling lines
    set EMSegment(Graph,Yscn,$index) [expr int(double($EMSegment(Graph,Ymax,$index) - $EMSegment(Graph,Ymin,$index)) / double($EMSegment(Graph,Ysca,$index)))]
    # Distance between scalling lines 
    set EMSegment(Graph,Yscp,$index) [expr $EMSegment(Graph,Ylen,$index)*$EMSegment(Graph,Ysca,$index) / double($EMSegment(Graph,Ymax,$index) - $EMSegment(Graph,Ymin,$index))] 

    # This is for 2D Histogram
    if {$index > 1} {
    # Define Length one unit in pixels
    set EMSegment(Graph,YleU,$index) [expr $EMSegment(Graph,Ylen,$index) / double($EMSegment(Graph,Ymax,$index) - $EMSegment(Graph,Ymin,$index))]
    if {$EMSegment(Graph,YleU,$index) < 1.0} {
        # Define the drawing surface, i.e. how many lines are left out to be calculated for 2 Dim Graph  
        set EMSegment(Graph,YcaL,$index) [expr int(1.0 / $EMSegment(Graph,YleU,$index))]
        # Width of the line painted 
        set EMSegment(Graph,YcaW,$index) 1
    } else {
        # Define the drawing surface, i.e. how many lines are left out to be calculated for 2 Dim Graph  
        set EMSegment(Graph,YcaL,$index) 1
        # Width of the line painted 
        set EMSegment(Graph,YcaW,$index) [expr int($EMSegment(Graph,YleU,$index)+0.999999999999999)]
    }
    # Xscaling per point does not change, so I just have to caluclate it once ! Makes it a little bit speedier
    # Start with point 1 not origin ! 
    set Ylength [expr $EMSegment(Graph,Ymax,$index) - $EMSegment(Graph,Ymin,$index) + 1]
    set ZeroY [expr $EMSegment(Graph,Cyle,$index) - $EMSegment(Graph,YboD)]
    
    # To push it pu we calcualte -2
    for {set i 0 } {$i < $Ylength} {incr i } {
        set EMSegment(Graph,Ypos,$i,$index) [expr $ZeroY - int($i*$EMSegment(Graph,YleU,$index)) - 1]
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateXLabels 
# Creates the Lables for the Graph's X-Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateXLabels {index} {
    global EMSegment Gui
    # Lables of X Coordinate Axis
    for {set i 0} { $i <= $EMSegment(Graph,Xscn,$index)} {incr i} {
        $EMSegment(Cl-caGraph$index) create line [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp,$index))]\
            [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU) + $EMSegment(Graph,Xscl)]\
            [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp,$index))]\
            [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU) - $EMSegment(Graph,Xscl)]\
            -width 1 -tag GreyL$i
        $EMSegment(Cl-caGraph$index) create text [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp,$index))]\
            [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU)+$EMSegment(Graph,Xscl) + $EMSegment(Graph,Xsct)]\
            -text [expr ($i * $EMSegment(Graph,Xsca,$index)) + $EMSegment(Graph,Xmin,$index)]\
            -tag GreyT$i -font {Helvetica 6} -fill $Gui(textDark)
    } 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateYLabels 
# Creates the Lables for the Graph's Y-Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateYLabels {index} {
    global EMSegment Gui
    # Scalers of Y Coordinate Axis
    for {set i 0} { $i <= $EMSegment(Graph,Yscn,$index)} {incr i} {
    $EMSegment(Cl-caGraph$index) create line [expr $EMSegment(Graph,XboL) - $EMSegment(Graph,Yscl)] \
        [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU) - int($i * $EMSegment(Graph,Yscp,$index) ) ] \
        [expr $EMSegment(Graph,XboL) + $EMSegment(Graph,Yscl)] \
        [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU) - int($i * $EMSegment(Graph,Yscp,$index) ) ] \
        -width 1 -tag ProbL$i

    if {$index < 2} {set TEXT [expr int($i*$EMSegment(Graph,Ysca,$index)*1000.0)/1000.0 + $EMSegment(Graph,Ymin,$index)] 
        } else { set TEXT [expr $i*$EMSegment(Graph,Ysca,$index) + $EMSegment(Graph,Ymin,$index)] }

    $EMSegment(Cl-caGraph$index) create text [expr $EMSegment(Graph,XboL) - $EMSegment(Graph,Yscl) - $EMSegment(Graph,Ysct)] \
        [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU) - int($i*$EMSegment(Graph,Yscp,$index))] \
        -text $TEXT -tag ProbT$i -font {Helvetica 6} -fill $Gui(textDark)  
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRedrawGraph 
# Recalcluates and redraws the whole graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRedrawGraph { } {
    global EMSegment
    for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
        for {set j 0} { $j < $EMSegment(NumGraph)} {incr j} {
            if { $EMSegment(Cattrib,$i,Graph,$j) == 1 } {
               if {[EMSegmentCalculateClassCurveRegion $i $j] == -1} {return}
               # This just makes sure the gaph is gone be rescaled for sure !!
               if {$j < 2} {set EMSegment(Graph,Ymax,$j) [expr $EMSegment(Graph,$i,YMax,$j) -1] }
            }
        }
    }
    for {set j 0} { $j < $EMSegment(NumGraph)} {incr j} {
    EMSegmentRescaleGraph $EMSegment(Graph,Xmin,$j) $EMSegment(Graph,Xmax,$j) $EMSegment(Graph,Xsca,$j) $j 0 
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRescaleGraphMulti 
# Calls EMSegmentRescaleGraph for multi graph version
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRescaleGraphMulti {index} {
    global EMSegment
    EMSegmentRescaleGraph $EMSegment(Graph,XminNew,$index) $EMSegment(Graph,XmaxNew,$index) $EMSegment(Graph,XscaNew,$index) $index 0
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRescaleGraph
# It will rescale the Graph if necessary
# Setting axis to :
# 0 = X Axis
# 1 = Y Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRescaleGraph {Min Max Sca index axis} {
    global EMSegment
    if {$axis < 1} {
        # if XRange changes we have to control if Y- Axis also has to be changed
        set GraphRescale [EMSegmentRescaleXAxis $Min $Max $Sca $index]
        # Check if we have to rescale graph in y-direction
        if {$index < 2} {
            set Ymax -1
            for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
                if {$EMSegment(Cattrib,$i,Graph,$index) == 1} {
                    if {$EMSegment(Graph,DisplayProb)} {
                       set ClassMax [expr $EMSegment(Cattrib,$i,Prob)*$EMSegment(Graph,$i,YMax,$index)]
                    } else { 
                       set ClassMax $EMSegment(Graph,$i,YMax,$index)
                    }
                    if {$ClassMax > $Ymax } {
                       set Ymax $ClassMax
                    }
                }
            }
            if {$Ymax > -1} { 
               if {$Ymax != $EMSegment(Graph,Ymax,$index)} {
                    set GraphRescale 1
                    set Ysca [expr ($Ymax - $EMSegment(Graph,Ymin,$index)) /double($EMSegment(Graph,Yscn,$index))]
                    EMSegmentRescaleGraph $EMSegment(Graph,Ymin,$index) $Ymax $Ysca $index 1
               }
            } else {
               if {[expr ($EMSegment(Cattrib,0,Graph,$index) == 1) && ($EMSegment(Graph,Ymax,$index) != 1.0)]} {
                    set GraphRescale 1
                    set Ymax 1.0
                    set Ysca [expr ($Ymax - $EMSegment(Graph,Ymin,$index)) /double($EMSegment(Graph,Yscn,$index))]
                    EMSegmentRescaleGraph $EMSegment(Graph,Ymin,$index) $Ymax $Ysca $index 1
               }
            }
       }
    } else {
       set GraphRescale [EMSegmentRescaleYAxis $Min $Max $Sca $index]
    }
    if {$GraphRescale == 1 } {
        # Redraw Curves
        if {$index < 2} {
            for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
                if { $EMSegment(Cattrib,$i,Graph,$index) == 1 } {
                   EMSegmentDrawSingleCurve $i $index
                }
            }
        } else {
            EMSegmentDrawRegion $EMSegment(Class) $index 3
        }
        return 1
    }
    return 0
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRescaleXAxis
# It will rescale the Graph for the changes in the X-Axis 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRescaleXAxis {Xmin Xmax Xsca index} {
    global EMSegment
    set GraphRescale 0
    # puts "EMSegmentRescaleXAxis $Xmin $Xmax $Xsca $index" 
    if {[expr ($Xmin !=  $EMSegment(Graph,Xmin,$index)) || \
              ($Xmax !=  $EMSegment(Graph,Xmax,$index)) || \
              ($Xsca !=  $EMSegment(Graph,Xsca,$index))]} {
      
        if [expr ($Xmin !=  $EMSegment(Graph,Xmin,$index)) || ($Xmax !=  $EMSegment(Graph,Xmax,$index))] {
            set GraphRescale 1
            if {$index > 1} {
                # Initiate / delete Extra pixels in graph
                set XoldMin  $EMSegment(Graph,Xmin,$index) 
                set XoldMax  $EMSegment(Graph,Xmax,$index)
                set yMin $EMSegment(Graph,Ymin,$index) 
                set yMax [expr $EMSegment(Graph,Ymax,$index)+1] 
                if {$Xmin < $XoldMin} {
                    EMSegmentCreateGraphPixels $index $Xmin $yMin $XoldMin $yMax
                }
                if {$Xmax > $XoldMax} {
                    EMSegmentCreateGraphPixels $index $XoldMax $yMin $Xmax $yMax
                }
             }
        }
        # Rescale X-Axis if necessary
        set EMSegment(Graph,Xmin,$index) $Xmin
        set EMSegment(Graph,Xmax,$index) $Xmax
        set EMSegment(Graph,Xsca,$index) $Xsca

        # Delete Labeling of X-Axis
        for {set i 0} { $i <= $EMSegment(Graph,Xscn,$index)} {incr i} {
            $EMSegment(Cl-caGraph$index) delete GreyL$i
            $EMSegment(Cl-caGraph$index) delete GreyT$i
        }

        # calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn,$index) and EMSegment(Graph,Xscp,$index) 
        EMSegmentCalulateGraphXleUXscnXscpXpos $index
        # Relabel X-Axis
        EMSegmentCreateXLabels $index

        # Update 2D Graph
        if {[expr ($EMSegment(NumGraph) > 2) && ($index < 2)] } {
             EMSegmentRescaleGraph $Xmin $Xmax $Xsca 2 $index
        }
        # Recalculate Curves
        if {$GraphRescale} {
            # If GraphRescale == 0 than only Xsca changed which changes the tabs but not the curve
            for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
                 if { $EMSegment(Cattrib,$i,Graph,$index) == 1 } {
                      if {[EMSegmentCalculateClassCurveRegion $i $index] == -1} {return}
                 }
            }
         }
    }
    # puts "EMSegmentRescaleXAxis GraphRescale $GraphRescale"
    return $GraphRescale
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraphPixels 
# It creates graph pixels as given 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphPixels {NumGraph xMin yMin xMax yMax} {
    global EMSegment
    incr xMax
    incr yMax
    for {set x $xMin} {$x < $xMax} {incr x} {
         for {set y $yMin} {$y < $yMax} {incr y} {
            set EMSegment(Graph,MAP,$NumGraph,$x,$y) 0
            set EMSegment(Graph,0,$NumGraph,$x,$y) 0.0
         }
    } 
}  

#-------------------------------------------------------------------------------
# .PROC EMSegmentRescaleYAxis
# It will rescale the Graph for the changes in the Y-Axis 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRescaleYAxis {Ymin Ymax Ysca index} {
    global EMSegment
    set GraphRescale 0
    # puts "Enter EMSegmentRescaleYAxis $Ymin $Ymax $Ysca $index"
    if {[expr ($Ymin !=  $EMSegment(Graph,Ymin,$index)) || \
              ($Ymax !=  $EMSegment(Graph,Ymax,$index)) || \
              ($Ysca !=  $EMSegment(Graph,Ysca,$index))]} {
         if [expr ($Ymin !=  $EMSegment(Graph,Ymin,$index)) || ($Ymax !=  $EMSegment(Graph,Ymax,$index))] {
             set GraphRescale 1
             if {$index > 1} {
                 # Initiate / delete Extra pixels in graph
                 set YoldMin  $EMSegment(Graph,Ymin,$index) 
                 set YoldMax  $EMSegment(Graph,Ymax,$index)
                 set xMin $EMSegment(Graph,Xmin,$index) 
                 set xMax [expr $EMSegment(Graph,Xmax,$index)+1] 
                 if {$Ymin < $YoldMin} {
                     EMSegmentCreateGraphPixels $index $xMin $Ymin $xMax $YoldMin
                 }
                 if {$Ymax > $YoldMax} {
                     EMSegmentCreateGraphPixels $index $xMin $YoldMax $xMax $Ymax
                 }
             }
        }
        # Rescale Y-Axis if necessary
        set EMSegment(Graph,Ymin,$index) $Ymin
        set EMSegment(Graph,Ymax,$index) $Ymax
        set EMSegment(Graph,Ysca,$index) $Ysca

        # Delete Labeling of Y-Axis
        for {set i 0} { $i <= $EMSegment(Graph,Yscn,$index)} {incr i} {
            $EMSegment(Cl-caGraph$index) delete ProbL$i
            $EMSegment(Cl-caGraph$index) delete ProbT$i
        }
        if {[expr ($EMSegment(Graph,Ymax,$index) - $EMSegment(Graph,Ymin,$index)) > 0.001]} {    
            if {$index < 2} {
                # Due to calculation error this value can be changed even though we do not want to
                set oldYscn $EMSegment(Graph,Yscn,$index)
                EMSegmentCalulateGraphYscnYscp $index  
                set EMSegment(Graph,Yscn,$index) $oldYscn
            } else {
                EMSegmentCalulateGraphYscnYscp $index  
            }
        } else {
            set EMSegment(Graph,Yscp,$index) $EMSegment(Graph,Ylen,$index)
        }
        # Relabel Y-Axis
        EMSegmentCreateYLabels $index
        # Recalculate Curves for 2D Graph
        if [expr ($index > 1) && $GraphRescale] {
           for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
                if { $EMSegment(Cattrib,$i,Graph,$index) == 1 } {
                    if {[EMSegmentCalculateClassCurveRegion $i $index] == -1} {return}
                }
           }
        }
    }
    return $GraphRescale
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentExecuteCIM 
# Executes command that is selected in CIM Menu Selection
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentExecuteCIM {command} {
    global EMSegment
    
    # Make sure Tab is set to CIM
    Tab EMSegment row1 CIM
    # Display Tab command
    set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) $command
    # Display what the window 
    raise $EMSegment(Ma-fCIM).f$command
    focus $EMSegment(Ma-fCIM).f$command
    switch $command {
       Import { $EMSegment(Ma-lLoadText) configure -text "Selecting File defining Class Interaction Matrix"
                 EMSegmentBrowseAndLoadCIMFile
                 # Raise window edit
                 EMSegmentExecuteCIM Edit
              }
        
       Export {             
                $EMSegment(Ma-lSaveText) configure -text "Export Class Interaction Matrix to a file" 
                set EMSegment(FileCIM) [ DevGetFile $EMSegment(FileCIM) 1 mrf "." "Export Class Interaction Matrix to a file" Save "" ];  
                EMSegmentSaveCIMFile
            $EMSegment(Ma-lSaveText) configure -text "Finished saving. \nUse other buttons for further actions!" 
            }
   }
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentBrowseAndLoadCIMFile 
# The Load Window for files pops up and the File is imported
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBrowseAndLoadCIMFile { } {
    global EMSegment
    set EMSegment(FileCIM) [DevGetFile $EMSegment(FileCIM) 1 mrf "." "Import File with Class Interaction Matrix"  "" " " ]; 
    EMSegmentImportCIM 1
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateCIMRowsColumns
# Creates the CIM Matrix (with start =1 and end = EMSegment(NumClasses) 
# in the CIM Panel or adds Rows and Columns to it  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateCIMRowsColumns {start end} {
   global EMSegment Gui
   set f $EMSegment(CIM-fMatrix) 
   set CIMType $EMSegment(CIMType)

   incr end
   # The CIM Matrix is intentiated for the first time
   if { $start == 1} {
      frame $f.fLineL -bg $Gui(activeWorkspace)
      pack $f.fLineL -side top -padx 0 -pady 0 -fill x
    
      eval {label $f.fLineL.lLabel  -text "" -width 3} $Gui(WLA)
      pack $f.fLineL.lLabel -side left -padx $Gui(pad) -pady 1 
   }

   # Add row text
   for {set i $start} {$i < $end} {incr i} {
       eval {label $f.fLineL.l$i -text "$EMSegment(Cattrib,$i,Label)" -width 5} $Gui(WLA)
       pack $f.fLineL.l$i -side left -padx 3  -pady 1 
   }

   for {set i 1} {$i < $end} {incr i} {
       if {$i < $start} {
          # Only add new Columns
          # EMSegment(CIMMatrix,<x>,<y>,<z>)
          for {set j $start} {$j < $end} {incr j} {         
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(CIMMatrix,$j,$i,$CIMType) -width 5} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
          } 
        } else {
          # Add new lines to the Matrix
          frame $f.fLine$i -bg $Gui(activeWorkspace)
          pack $f.fLine$i -side top -padx 0 -pady 0 -fill x

          eval {label $f.fLine$i.lLabel  -text "$EMSegment(Cattrib,$i,Label)" -width 3} $Gui(WLA)
          pack $f.fLine$i.lLabel -side left -padx $Gui(pad) -pady 1 
          # Add new columns to the matrix
          for {set j 1} {$j < $end} {incr j} {
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(CIMMatrix,$j,$i,$CIMType) -width 5} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
      } 
        }
    }
    EMSegmentSetCIMMatrix [expr $end -1]
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSetCIMMatrix
# Defines the size of the canvas $EMSegment(CIM-cMatrix)
# in the CIM Panel or adds Rows and Columns to it  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetCIMMatrix {dim} {
   global EMSegment
   set f $EMSegment(CIM-fMatrix)
   set reqheight [winfo reqheight $f.fLine1.eCol1]
   set reqwidth  [winfo reqwidth $f.fLine1.eCol1]
   set reqlabel  [winfo reqwidth $f.fLine1.lLabel]
   set f $EMSegment(CIM-fDefinition)
   $f.fMatrix.cMatrix config -height [expr ($reqheight+2)*($dim+1)] -scrollregion "0 0 [expr ($reqwidth+1)*$dim + $reqlabel+20] 1"    
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeCIMMatrix
# Changes the Marcov Matrix depending on the MarcovType 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeCIMMatrix {CIMType} {
   global EMSegment Gui
   set f $EMSegment(CIM-fMatrix) 
   set $EMSegment(CIMType) $CIMType

   for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
       for {set j 1} {$j <= $EMSegment(NumClasses)} {incr j} {
      $f.fLine$i.eCol$j configure -textvariable EMSegment(CIMMatrix,$j,$i,$CIMType) 
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentTrainCIMField
# Traines the CIM Field with a given Image
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentTrainCIMField {} {
    global EMSegment Volume

    # Transferring Information
    vtkImageEMMarkov EMCIM    
    # EM Specific Information
    EMCIM SetNumClasses     $EMSegment(NumClasses)  
    EMCIM SetStartSlice     $EMSegment(StartSlice)
    EMCIM SetEndSlice       $EMSegment(EndSlice)

    EMCIM SetImgTestNo       $EMSegment(ImgTestNo)
    EMCIM SetImgTestDivision $EMSegment(ImgTestDivision)
    EMCIM SetImgTestPixel    $EMSegment(ImgTestPixel)

    for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
        if {$EMSegment(ImgTestNo) < 1} { 
            EMCIM SetMu           $EMSegment(Cattrib,$i,Label) $i
            EMCIM SetSigma        1.0 $i
        } else {     
            EMCIM SetMu           [expr (($i-1)*255)/($EMSegment(NumClasses)-1)] $i
            EMCIM SetSigma        10 $i
        }
    }  

    # Transfer image information
    EMCIM SetInput [Volume($Volume(activeID),vol) GetOutput]

    # set data [EMCIM GetInput]
    # EMCIM Update
 
    # $data_blub SetWholeExtent 0 $EMSegment(NumClasses) 0 $EMSegment(NumClasses) 0 5
    # EMCIM SetComponentExtent 0 $EMSegment(NumClasses) 0 $EMSegment(NumClasses) 0 5

    set data [EMCIM GetOutput]
    # This Command calls the Thread Execute function
    $data Update

    for {set x 1} {$x <= $EMSegment(NumClasses) } {incr x} {
        set EMSegment(Cattrib,$x,Prob) [EMCIM GetProbability $x]
        # EMCIM traines the matrix (y=t, x=t-1) just the other way EMSegment (y=t-1, x=t) needs it - Sorry !
        for {set y 1} {$y <=  $EMSegment(NumClasses) } {incr y} {
            for {set z 0} {$z < 6} {incr z} {
              # Error made in x,y coordinates in EMSegment - I checked everything - it owrkes in XML and CIM Display in correct order - so do not worry - it is just a little bit strange - but strange things happen
              set EMSegment(CIMMatrix,$x,$y,[lindex $EMSegment(CIMList) $z]) [expr round([$data GetScalarComponentAsFloat [expr $y-1] [expr $x-1]  $z 0]*100000)/100000.0]        
            }
        }
    }
   
    # Delete instance
    EMCIM Delete
 
    # Jump to edit field 
    EMSegmentExecuteCIM Edit
    # Move Button to Edit 
    set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentReadCIMFile 
# Reads the File specifing the CIM Matrix and writes it to 
# EMSegment(MarkoMatrix,: ,: ,:) (To use some Matlab Connotation
# Lines Starting with # Will be ignored
# The form of the file: 
# The order of the Matricees has to be Neighbour = Down, Up, Left and Right
# Matricees are devided by one empty line
# eg Input for 2 Class Matrix would be
# # Matrix South
# 1 0
# 0 1
#
# # Matrix North
# 0 1
# 1 0 
# etc .....
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadCIMFile {PermissionFlag} {
   global EMSegment Volume
   # Open file
   set FileID [open $EMSegment(FileCIM) r]
   # Read Lines
   set MatrixNumber 1
   set ErrorFlag 0 
   set NumberOfInputImages 1
   # Read Class Paramters
   set PrintOut 0
   if {$EMSegment(UseSamples) == 1} {set EMSegment(UseSamples) 0;EMSegmentUseSamples 1}

   if {$PermissionFlag == 1} {set OverwriteSetting [DevYesNo "Do you want to overwrite current class setting ?"]
   } else {set OverwriteSetting "yes"}
   set LineNumber -2
   # Read Number of Classes and Input Images
   # Had to do this this way because if (number of classes != EMSegment(NumClasses)) and user does not want to overwrite class setting
   # We had to stop right here
   while { [expr ![eof $FileID] && !$ErrorFlag && ($LineNumber < $NumberOfInputImages)] } {
       set LineInput [EMSegmentReadToNextLine $FileID $PrintOut]
       if {[string index $LineInput 0] != "\#"} {
       incr LineNumber
       switch -exact -- $LineNumber {
           -1  { if {$EMSegment(NumClasses) != [lindex $LineInput 0]}  {
                if {$OverwriteSetting == "no"} {
                DevErrorWindow "Error(6) - while loading file:\nFile does not define paramters for $EMSegment(NumClasses) classes!"
                return 6
            }
            set EMSegment(NumClassesNew) [lindex $LineInput 0]
            EMSegmentCreateDeleteClasses
             }
           }
            0  { set NumberOfInputImages [lindex $LineInput 0]
             if {$OverwriteSetting == "yes"} {
             if {[expr (($EMSegment(SegmentMode) < 2) && ($NumberOfInputImages > 1))] == 1} {
                 DevErrorWindow "Error(8) - while loading file:\n File does define paramters for more than 1 input image!"
                 return 8
             }
             EMSegmentDeleteFromSelList $EMSegment(SelVolList,VolumeList)
              } else {
             if {$EMSegment(NumInputImages) != $NumberOfInputImages} {
                 DevErrorWindow "Error(7) - while loading file:\n File does not define paramters for $LenVol input images!"
                 return 7
             }
              }
            }
         default { if {$OverwriteSetting == "yes"} { 
                       EMSegmentReadVolumeAttributesAndLoad $LineInput
                       # Attach File to Selected input file list
                       set EMSegment(AllVolList,ActiveID) [lsearch -exact $EMSegment(AllVolList,VolumeList) $Volume(activeID)]
                       EMSegmentTransfereVolume All
                    }
                  }
       }
       }
   }
   set LineNumber 0
   # Read Class Paramteres
   while { [expr ![eof $FileID] && !$ErrorFlag && ($LineNumber < $EMSegment(NumClasses))] } {
       set LineInput [EMSegmentReadToNextLine $FileID $PrintOut]
       if {[string index $LineInput 0] != "\#"} {
           incr LineNumber
           if {$OverwriteSetting == "yes"} { 
               # Read Class Probability and Distribution
               set linelength [llength $LineInput]
               if { $linelength < 3} {
                   DevErrorWindow "Error(5) - while loading file:\nFile does not define enough Class paramters !" 
                   set ErrorFlag 5
               }
               EMSegmentClickLabel $LineNumber [lindex $LineInput 0] [lindex $LineInput 1]
               set EMSegment(Cattrib,$LineNumber,Prob)  [lindex $LineInput 2]
               if {[expr (($linelength > 9) && ($EMSegment(SegmentMode)))]} {
                   EMSegmentReadVolumeAttributesAndLoad [lrange $LineInput 3 end] 
                   set EMSegment(Cattrib,$LineNumber,ProbabilityData) $Volume(activeID) 
               } else {
                   set  EMSegment(Cattrib,$LineNumber,ProbabilityData) $Volume(idNone) 
               }
               # Read Mean 
               set LineInput [EMSegmentReadToNextLine $FileID $PrintOut]
               EMSegmentReadMeanfromFile $LineInput $LineNumber
               # Read Covariance
               EMSegmentReadCovariancefromFile $FileID $LineNumber $PrintOut
            } else {
               EMSegmentReadToNextLine $FileID $PrintOut
               for {set i 0} { $i < $EMSegment(NumInputChannel)} { incr i} {
                   EMSegmentReadToNextLine $FileID $PrintOut
               }
           }
       }
   }

   # Set the right probability data map for current displayed class
   if {$OverwriteSetting == "yes"} {
      if {$EMSegment(SegmentMode) > 0} {
      set EMSegment(ProbVolumeSelect)  $EMSegment(Cattrib,$EMSegment(Class),ProbabilityData)
      }
      EMSegmentChangeClass 1
   }


   gets $FileID LineInput 
   set LineNumber 0
   # Read CIM Paramters
   while { [expr ![eof $FileID] && !$ErrorFlag] } {
       set LineInput [EMSegmentReadToNextLine $FileID $PrintOut]  
       if {[string index $LineInput 0] != "\#"} {
          if { [llength $LineInput] < $EMSegment(NumClasses)} {
              if {$LineNumber != $EMSegment(NumClasses)} {
                 DevErrorWindow "Error(1) - while loading file:\nFile does not define an MRF-Field for $EMSegment(NumClasses) classes!" 
                 set ErrorFlag 1
              } else {
                 # Seperation between Matricees -> Read A new matrix
                 if {$MatrixNumber <  6 } {
                   set LineNumber 0
                   incr MatrixNumber
                 } else {
                   # There are additional lines in there, which I do not care about
                   while {![eof $FileID]} {gets $FileID LineInput}
                 }
          }
           } else {
              incr  LineNumber
              if {$LineNumber > $EMSegment(NumClasses) } {
                DevErrorWindow "Error(2) - while loading file:\nFile does not define an MRF-Field for $EMSegment(NumClasses) classes!" 
                set ErrorFlag 1
              }
              for {set i 0} {$i < $EMSegment(NumClasses)} { incr i} {
                # Test if inputs are numbers 
                if [catch {expr [lindex $LineInput $i] +1.0}] {
                   DevErrorWindow "Error(3) - while loading file:\nFile has non-numeric MRF-paramters in MRF Matrix!" 
                   set ErrorFlag 1
                   set i $EMSegment(NumClasses)
                 } else {
                   set CIMMatrix([expr $i+1],$LineNumber,$MatrixNumber) [lindex $LineInput $i]
                 }
              }
           }
       }
   }

   # Close File
   close $FileID 

   # Check If we read enough Data
   if {!$ErrorFlag} {
       if {[expr ($LineNumber != $EMSegment(NumClasses)) || ($MatrixNumber != 6)]} {
       DevErrorWindow "Error(4) - while loading file:\nFile does not define enough MRF-Parameters for $EMSegment(NumClasses) classes!" 
       set ErrorFlag 1
       } else {
          # Write Data to EMSegment
          for {set i 1} {$i < 7} { incr i} {
             for {set j 1} {$j <= $EMSegment(NumClasses)} { incr j} {
                 for {set k 1} {$k <= $EMSegment(NumClasses)} { incr k} {
                    set EMSegment(CIMMatrix,$j,$k,[lindex $EMSegment(CIMList) [expr $i -1]]) $CIMMatrix($j,$k,$i)
                 }
             }
          }
       }
   }
   return [expr 1-$ErrorFlag]
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentReadMeanfromFile
# Reads the mean definition from the file 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadMeanfromFile {LineInput Class} {
    global EMSegment
    for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
    set EMSegment(Cattrib,$Class,LogMean,$x)  [lindex $LineInput $x]
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentReadCovariancefromFile
# Reads the covariance definition from the file 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadCovariancefromFile {FileID Class Debug} {
    global EMSegment
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} { 
       set LineInput [EMSegmentReadToNextLine $FileID $Debug] 
       for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
          set EMSegment(Cattrib,$Class,LogCovariance,$y,$x)  [lindex $LineInput $x]
       }
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentReadToNextLine
# Reads the next uncommented line in the file 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadToNextLine {FileID Debug} {
    global EMSegment
    if {[eof $FileID]  == 0} {
    gets $FileID LineInput
    } else {return "# Error: End Of File"}
    if {$Debug} {puts $LineInput}
    while { ![eof $FileID] && ([string index $LineInput 0] == "\#") } {
    gets $FileID LineInput
    if {$Debug} {puts $LineInput}
    }
    return $LineInput
}
proc EMSegmentReadVolumeAttributesAndLoad {LineInput} {
    set VolumeFile     [lindex $LineInput 0] 
    set VolumeName     [lindex $LineInput 1] 
    set First          [lindex $LineInput 2]
    set Last           [lindex $LineInput 3]
    set LittleEndian   [lindex $LineInput 4]
    set PixelWidth     [lindex $LineInput 5]
    set PixelHeight    [lindex $LineInput 6]
    set SliceThickness [lindex $LineInput 7]
    EMSegmentLoadVolume . [format "%s.%03d" $VolumeFile $First] $Last $VolumeName $LittleEndian $PixelWidth $PixelHeight $SliceThickness
    DevWarningWindow "Loaded $VolumeName"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplayHorizontal 
# Displays Text horizontal -> Does not look very good so I do not use it  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplayHorizontal { Frame Text Length } {
   global Gui
   set TextLength [string length $Text]
   for {set i 0} {$i < $Length} {incr i} {
       if {$i <= $TextLength} { eval {label $Frame.l$i -text [string index $Text $i]} $Gui(WLA)
       } else { eval {label $Frame.l$i -text " "} $Gui(WLA)}
       pack $Frame.l$i -side top -padx 0 -pady 0
   }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentEndSlice 
# Calculates the last slice of the current selected Volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEndSlice {flag {VolID -1}} {
    global EMSegment Volume
    if {$VolID == -1} {
    if {$EMSegment(NumInputChannel)} {
        set VolID [lindex $EMSegment(SelVolList,VolumeList) 0]
    } else {
        DevErrorWindow "Before calculating the last slice a volume for segmentation must be selected"
        return -1
    }
    }
    set SlideRange [Volume($VolID,node) GetImageRange] 
    set result [expr ([lindex $SlideRange 1] - [lindex $SlideRange 0] + 1)]
    if {$flag} {
       set EMSegment(EndSlice) $result
    } else {
       return $result
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentLoadVolume 
# Loads a volume into slicer 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentLoadVolume {DefaultDir StartFile End Name LittleEndian PixelWidth PixelHeight SliceThickness} {
    global Data Volume Module EMSegment
    # Data.tcl DataAddVolume
    # Setup entries
    set Volume(propertyType) VolHeader
    MainVolumesSetActive NEW
    set Volume(freeze) 1

    # set Module(freezer) "EMSegment row1 EM"

    # User Entries    
    set Volume(DefaultDir)   $DefaultDir 
    set Volume(firstFile)    $StartFile 
    set Volume(name)         $Name 
    set Volume(littleEndian) $LittleEndian

    #Default entries
    set Volume(readHeaders) 0
    set Volume(labelMap) 0            
    # set Volume(lastNum) [expr ($First + $EMSegment(EndSlice) - 1)] 
    set Volume(lastNum) $End
    set Volume(isDICOM) 0
    set Volume(height) 256
    set Volume(width) 256
    # set Volume(pixelSize) 0.9375
    set Volume(pixelWidth) $PixelWidth 
    set Volume(pixelHeight) $PixelHeight
    set Volume(sliceThickness) $SliceThickness 
    set Volume(sliceSpacing) 0.0
    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    set Volume(tensors,pfSwap) 0
    
    VolumesPropsApply 
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateMoment
# Calculates the moment of the acitve image
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateMoment {} {
    global EMSegment Volume

    # Transferring Information
    vtkImageMoments Moments    
    # Transfer image information
    Moments SetInput [Volume($Volume(activeID),vol) GetOutput]

    #set data [Moments GetInput]
    #$data Update

    set data [Moments GetOutput]
    $data Update

    for {set x 0} {$x < 3 } {incr x} {
        for {set y 0} {$y < 3 } {incr y} {
           puts -nonewline "[$data GetScalarComponentAsFloat $x $y 0 0] "        
        }
    }
   
    # Delete instance
    Delete Moments
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentAssignInput
# Create selection Mask for multiple input images in EM Tab
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentAssignInput {froot} {
    global EMSegment Gui Volume
    #-------------------------------------------
    # EM->Step 1->Input Images frame
    #-------------------------------------------
    frame $froot.fAllVol  -bg $Gui(activeWorkspace)
    pack $froot.fAllVol -side left -padx 2 -pady 1 -fill x
    frame $froot.fButtons -bg $Gui(activeWorkspace)
    pack $froot.fButtons -side left -padx 2
    frame $froot.fSelVol  -bg $Gui(activeWorkspace) 
    pack $froot.fSelVol -side left -padx 2 -pady 1 -fill x
    
    set f $froot.fAllVol 
    eval {label $f.lTitle -text "Volume List"} $Gui(WTA)

    set EMSegment(fAllVolList) [ScrolledListbox $f.list 1 1 -height 3 -width 11]
    bind $EMSegment(fAllVolList) <ButtonRelease-1> {EMSegmentSelectfromVolumeList All}
    
    # lappend Volume(mbActiveList) $f.fSelection.mbVolumeSelect
    # lappend Volume(mActiveList) $f.fSelection.mbVolumeSelect.m

    pack $f.lTitle -side top -pady 2
    pack $f.list   -side top -pady $Gui(pad)

    set f $froot.fButtons
    eval {button $f.bAdd    -text "=>" -width 3 -command "EMSegmentTransfereVolume All"} $Gui(WBA)
    eval {button $f.bRemove -text "<=" -width 3 -command "EMSegmentTransfereVolume Sel"} $Gui(WBA)
    pack $f.bAdd $f.bRemove -side top -pady $Gui(pad)
    
    set f  $froot.fSelVol 
    eval {label $f.lTitle -text "Input Volumes"} $Gui(WTA)

    set EMSegment(fSelVolList) [ScrolledListbox $f.list 1 1 -height 3 -width 11]
    bind $EMSegment(fSelVolList) <ButtonRelease-1>  {EMSegmentSelectfromVolumeList Sel} 
    
    pack $f.lTitle -side top -pady 2
    pack $f.list -side top -pady $Gui(pad) 

    # Add all images to the list
    foreach v $EMSegment(AllVolList,VolumeList) {
        $EMSegment(fAllVolList) insert end  [Volume($v,node) GetName] 
    }

    foreach v $EMSegment(SelVolList,VolumeList) {
        $EMSegment(fSelVolList) insert end  [Volume($v,node) GetName] 
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentSelectfromVolumeList
# Activate selection from either list 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSelectfromVolumeList {type} {
    global Volume EMSegment
    set i [$EMSegment(f${type}VolList) curselection]
    $EMSegment(f${type}VolList) selection set $i $i
    set EMSegment(${type}VolList,ActiveID) $i
    if {$type == "All"} {
        set EMSegment(SelVolList,ActiveID) -1
    } else {
        set EMSegment(AllVolList,ActiveID) -1
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeVolumeSegmented 
# This is for the one channel case !
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeVolumeSegmented {index} {
    global EMSegment
    set EMSegment(SelVolList,ActiveID) 0
    EMSegmentTransfereVolume Sel
    set EMSegment(AllVolList,ActiveID) $index
    EMSegmentTransfereVolume All
}  
    

#-------------------------------------------------------------------------------
# .PROC EMSegmentTransfereVolume
# Transfer one entry form the one window to the other one 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentTransfereVolume {from} {
    global EMSegment
    if {$EMSegment(${from}VolList,ActiveID) == -1} {return}
    set VolumeID [lindex $EMSegment(${from}VolList,VolumeList) $EMSegment(${from}VolList,ActiveID)]
    if {$from == "All"} { 
      set to Sel
      # Delete entires in Menu Sample list
      if {$EMSegment(NumInputChannel) } {
         if { [llength $EMSegment(Cattrib,$EMSegment(Class),[lindex $EMSegment(SelVolList,VolumeList) 0],Sample)] > 0 }  {
           $EMSegment(Cl-mEraseSample)  delete 1 end
         }
      }
      incr EMSegment(NumInputChannel)
      EMSegmentSetMaxInputChannelDef $EMSegment(NumInputChannel)
      # Reset Sample list 
      for {set i 1 } { $i <= $EMSegment(NumClasses)} {incr i} {
        foreach v $EMSegment(SelVolList,VolumeList) {
          set EMSegment(Cattrib,$i,$v,Sample) {}
        }
        set EMSegment(Cattrib,$i,$VolumeID,Sample) {}
      }
      EMSegmentCreateMeanCovarianceRowsColumns [expr $EMSegment(NumInputChannel)-1] $EMSegment(NumInputChannel) 
      EMSegmentCalculateClassMeanCovariance
      # Add it to the Volume Graph Button
      if {$EMSegment(SegmentMode) > 1} {
        $EMSegment(Cl-mGraphHistogram0) add command -label [Volume($VolumeID,node) GetName] -command "EMSegmentChangeVolumeGraph $VolumeID 0"
        if {$EMSegment(NumGraph) > 1} { $EMSegment(Cl-mGraphHistogram1) add command -label [Volume($VolumeID,node) GetName] -command "EMSegmentChangeVolumeGraph $VolumeID 1"}
        $EMSegment(fAllVolList) delete $EMSegment(AllVolList,ActiveID)
      } else {
        $EMSegment(EM-mbVolumeSelect) configure -text "[Volume($VolumeID,node) GetName]"
        $EMSegment(EM-mVolumeSelect) delete $EMSegment(AllVolList,ActiveID)
        set IndexLen [expr [llength $EMSegment(AllVolList,VolumeList)]-1]
        for {set i $EMSegment(AllVolList,ActiveID)} {$i < $IndexLen} {incr i} {
          $EMSegment(EM-mVolumeSelect) entryconfigure $i -command "EMSegmentChangeVolumeSegmented $i"
        }
        for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
          EMSegmentChangeVolumeGraph $VolumeID $i
        }
      }
      # Define End Slice
      set EndSlice [EMSegmentEndSlice 0  $VolumeID] 
      if {[expr (($EMSegment(EndSlice) == -1) || ($EndSlice < $EMSegment(EndSlice)))] } { set EMSegment(EndSlice) $EndSlice}
      set EMSegment(AllVolList,VolumeList) [lreplace $EMSegment(AllVolList,VolumeList) $EMSegment(AllVolList,ActiveID) $EMSegment(AllVolList,ActiveID)]

    } else { 
      set to All 
      EMSegmentDeleteFromSelList $VolumeID
    }
    if {$EMSegment(SegmentMode) > 1} { 
      $EMSegment(f${to}VolList) insert end [Volume($VolumeID,node) GetName]
    } 
    lappend EMSegment(${to}VolList,VolumeList) $VolumeID
    set EMSegment(${from}VolList,ActiveID) -1

}

#-------------------------------------------------------------------------------
# .PROC EMSegmentUpdateVolumeList
# This is automatically called from UpdateGui or EMSegmentReadCIMFile  when a 
# new Volume is added to the SelList and non exisitng volumes are erased from both lists
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateVolumeList { } {
    global Volume EMSegment
    
    # Menu of NodeTypes
    # ------------------------------------
    # set m EMSegment(mb$Label).m
    # $m delete 0 end
    # All volumes except none
    set EMSegment(AllVolList,ActiveID) -1
    set EMSegment(SelVolList,ActiveID) -1
    set EMSegment(AllVolList,VolumeList) { }
    set NewList { } 
    # Delete All Values from the List in panel EM
    if {$EMSegment(SegmentMode) < 2} {
      $EMSegment(EM-mVolumeSelect) delete 0 end
    } else {
      $EMSegment(fAllVolList) delete 0 end
    }

    set i 0
    foreach v $Volume(idList) {
      if {$v != $Volume(idNone)} {
        set index [lsearch -exact $EMSegment(SelVolList,VolumeList) $v]
        if {$index > -1} {
          lappend NewList $v 
        } else {
          if {$EMSegment(SegmentMode) < 2} {
            $EMSegment(EM-mVolumeSelect) add  command -label [Volume($v,node) GetName] -command "EMSegmentChangeVolumeSegmented $i"
            incr i
          } else {
            $EMSegment(fAllVolList) insert end  [Volume($v,node) GetName]
        set EMSegment(IntensityAvgValue,$v) -1.0 
          }
          lappend EMSegment(AllVolList,VolumeList) $v
        }
      }
    }
    # Erase non exisitng Volumes and place the other once in the same order as before. Important for class definition   
    foreach v $EMSegment(SelVolList,VolumeList)  {
      if { [lsearch -exact $NewList $v] == -1 } {
        EMSegmentDeleteFromSelList $v
      } 
    }    
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentDeleteFromSelList
# Delete entries from the Selection List 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDeleteFromSelList {args} {
    global EMSegment Volume
    set List [join $args]
    # puts "EMSegmentDeleteFromSelList $args List $List"
    # First Destroy Fields : Remeber only the position in the Graph matches the input - the input is independent from the graph
    # Therefore: if you delete the first element in the Graph (SelVolList) than the second input channel gets assigned the 
    # values previously defined for the just deleted input channel because the second input channel is now the first !!!!
    set DestroyStart  [expr $EMSegment(NumInputChannel) - [llength $List]] 
    for {set dest $DestroyStart} {$dest < $EMSegment(NumInputChannel)} {incr dest} {
    destroy $EMSegment(EM-fMeanVar).e$dest
    destroy $EMSegment(Cl-fLogMeanVar).e$dest
    destroy $EMSegment(Cl-fLogCovVar).fLine$dest
    for {set y 0} {$y < $DestroyStart} {incr y} {
        destroy $EMSegment(Cl-fLogCovVar).fLine$y.e$dest
    }
    }
    foreach v $List {
    # Delete Mean and Covariance in Class panel
    set index [lsearch -exact $EMSegment(SelVolList,VolumeList) $v]
    # Now we have tom move everything one step down otherwise the entry fields do not correspond anymore to the position in the volume list
    if {$EMSegment(SegmentMode) < 2} {
        if { [lsearch -exact $Volume(idList) $v] > -1} {  
        $EMSegment(EM-mVolumeSelect) add command -label [Volume($v,node) GetName] -command "EMSegmentChangeVolumeSegmented [llength $EMSegment(AllVolList,VolumeList)]"  
        } 
        # There is only one displayed 
        $EMSegment(EM-mbVolumeSelect) configure -text "None"
    } else {
        $EMSegment(fSelVolList) delete $index
    }
    # We have to destroy the menu selection because no volume is available anymore
    if {$EMSegment(NumInputChannel) == 1} {
       if { [llength $EMSegment(Cattrib,$EMSegment(Class),$v,Sample)] > 0 }  {
        $EMSegment(Cl-mEraseSample) delete 1 end
        } 
        set EMSegment(EndSlice) -1
    }
    # Delete from Graph Volume List
    if  {$EMSegment(SegmentMode) > 1} {
        $EMSegment(Cl-mGraphHistogram0) delete $index
        if {$EMSegment(NumGraph) > 1} {$EMSegment(Cl-mGraphHistogram1) delete $index}
        for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
        if {$EMSegment(Cattrib,0,Label,$i) == $v } {
            set EMSegment(Cattrib,0,Label,$i) ""
            $EMSegment(Cl-mbGraphHistogram$i) config -text ""
            if {$EMSegment(Cattrib,0,Graph,$i) == 1} {
                EMSegmentDrawDeleteCurveRegion 0 $i
            }
            for {set j 1} {$j <= $EMSegment(NumClasses)} {incr j} {   
            if {$EMSegment(Cattrib,$j,Graph,$i) == 1} {
                EMSegmentMultipleDrawDeleteCurveRegion $j
            }
            }
        }
        }
    } else {
        for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
        set EMSegment(Cattrib,0,Label,$i) ""
        $EMSegment(Cl-bGraphHistogram$i) config -text ""
        if {$EMSegment(Cattrib,0,Graph,$i) == 1} {
            EMSegmentDrawDeleteCurveRegion 0 $i
        }
        }
    }
    # Delete entry from list
    set EMSegment(SelVolList,VolumeList) [lreplace $EMSegment(SelVolList,VolumeList) $index $index]    
    incr EMSegment(NumInputChannel) -1 
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateMeanCovarianceRowsColumns 
# Creates the Mean and Covariance Matrix for each noew element in the list 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateMeanCovarianceRowsColumns {OldNumInputCh NewNumInputCh} {
    global EMSegment Gui
    # Start with Mean
    set fem    $EMSegment(EM-fMeanVar)
    set fcllog $EMSegment(Cl-fLogMeanVar)
    set Sclass $EMSegment(Class)
    if {$EMSegment(UseSamples) == 1} {
      set VorderGrund $Gui(textDark)
    } else {
      set VorderGrund $Gui(textDisabled)
    }

    for {set x $OldNumInputCh} {$x < $NewNumInputCh} {incr x} {
    eval {entry  $fem.e$x -textvariable EMSegment(Cattrib,$Sclass,LogMean,$x) -width 6 -state disabled -font {helvetica 8} -bg $Gui(activeWorkspace) \
        -fg $VorderGrund -bd 0 -relief flat -highlightthickness 0 } 
    pack $fem.e$x -side left  -padx 1 -pady 1
    eval {entry  $fcllog.e$x -textvariable EMSegment(Cattrib,$Sclass,LogMean,$x) -width 5} $Gui(WEA)
    pack $fcllog.e$x -side left  -padx 1 -pady 1
    }

    set flog $EMSegment(Cl-fLogCovVar)
    for {set y 0} {$y < $OldNumInputCh} {incr y} { 
    for {set x $OldNumInputCh} {$x < $NewNumInputCh} {incr x} { 
        eval {entry  $flog.fLine$y.e$x -textvariable EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) -width 5} $Gui(WEA) 
        pack $flog.fLine$y.e$x -side left  -padx 1 -pady 1
    }
    }
    for {set y $OldNumInputCh} {$y < $NewNumInputCh} {incr y} { 
    frame $flog.fLine$y -bg $Gui(activeWorkspace)
    pack $flog.fLine$y -side top -padx 0 -pady 0 -fill x
     for {set x 0} {$x < $OldNumInputCh} {incr x} { 
        eval {entry  $flog.fLine$y.e$x -textvariable EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) -width 5} $Gui(WEA) 
        pack $flog.fLine$y.e$x -side left  -padx 1 -pady 1
    }
    for {set x $OldNumInputCh} {$x < $NewNumInputCh} {incr x} { 
        eval {entry  $flog.fLine$y.e$x -textvariable EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) -width 5} $Gui(WEA) 
        pack $flog.fLine$y.e$x -side left  -padx 1 -pady 1
    }
    }
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentCreateLogGrayValueDisplay
# Creates the entry field in the upper right corner of each graph displaying the 
# current log gray value of the image 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateLogGrayValueDisplay {pos List index} {
    global EMSegment Gui
    set ca $EMSegment(Cl-caGraph$index)
    set xpos [expr $EMSegment(Graph,Cxle,$index) - $EMSegment(Graph,XboR) - 18]
    $ca create text $xpos [expr $EMSegment(Graph,YboU) + $pos*7 + 2] -text "" -tag GrayT0 -font {Helvetica 6} -fill $Gui(textDark)      
    if {$index > 1} {
    incr pos
    $ca create text $xpos [expr $EMSegment(Graph,YboU) + $pos*7 + 2] -text "" -tag GrayT1 -font {Helvetica 6} -fill $Gui(textDark)
    }      
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraph
# Creates a graph with x and y axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraph {f index} {
    global EMSegment Gui

    canvas $f.caGraph -width $EMSegment(Graph,Cxle,$index) -height $EMSegment(Graph,Cyle,$index)      
    set ca $f.caGraph
    set EMSegment(Cl-caGraph$index) $ca
    pack $ca -side top  -padx $Gui(pad)  -pady $Gui(pad)

    # Coordinate Axis
    $ca create line $EMSegment(Graph,XboL) [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU)] \
                                       [expr $EMSegment(Graph,Xlen,$index) + $EMSegment(Graph,XboL)] \
                       [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU)] \
                       -width 1 -tag Grayvalue 
    $ca create line $EMSegment(Graph,XboL) [expr $EMSegment(Graph,Ylen,$index) + $EMSegment(Graph,YboU)] \
        $EMSegment(Graph,XboL) $EMSegment(Graph,YboU) -width 1 -tag Probaility 

    # Scalers of X and Y Coordinate Axis
    EMSegmentCreateXLabels $index
    EMSegmentCreateYLabels $index
    # Define Log Gray Value Box 
    EMSegmentCreateLogGrayValueDisplay 0 $EMSegment(SelVolList,VolumeList) $index
    #Initialize value for 2D Graph
    if {$index > 1} {
    EMSegmentCreateGraphPixels $index $EMSegment(Graph,Xmin,$index) $EMSegment(Graph,Ymin,$index) $EMSegment(Graph,Xmax,$index) $EMSegment(Graph,Ymax,$index)
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateHistogramButton
# Defines the Historgramm button
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateHistogramButton {f index} {
    global Gui EMSegment

    if {$EMSegment(Cattrib,0,Label,$index) == "" } {
    set MenuText ""
    } else {
    set MenuText [Volume($EMSegment(Cattrib,0,Label,$index),node) GetName]
    }

    if  {$EMSegment(SegmentMode) > 1} {
    set EMSegment(Cl-mbGraphHistogram$index) $f.mbHistogram
    set EMSegment(Cl-mGraphHistogram$index) $f.mbHistogram.m
    
    eval {menubutton $EMSegment(Cl-mbGraphHistogram$index) -text $MenuText  -menu $EMSegment(Cl-mGraphHistogram$index) -width 8} $Gui(WBA) 
    $EMSegment(Cl-mbGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorCode)
    $EMSegment(Cl-mbGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorCode)

    pack $EMSegment(Cl-mbGraphHistogram$index) -side left -padx $Gui(pad) 
    TooltipAdd  $EMSegment(Cl-mbGraphHistogram$index) "Press left mouse button to selct volume - press right mouse button to display volume's histogram "
    bind $EMSegment(Cl-mbGraphHistogram$index) <Button-3> "EMSegmentDrawDeleteCurveRegion 0 $index" 

    # Define Menu selection 
    eval {menu $EMSegment(Cl-mGraphHistogram$index)} $Gui(WMA)
    $EMSegment(Cl-mGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorCode)
    $EMSegment(Cl-mGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorCode)
    # Add Selection entry
    foreach v $EMSegment(SelVolList,VolumeList)  {
        set VolName [Volume($v,node) GetName]
        $EMSegment(Cl-mGraphHistogram$index) add command -label $VolName -command "EMSegmentChangeVolumeGraph $v $index"
    }
    } else {
    set EMSegment(Cl-bGraphHistogram$index) $f.bHistogram
    eval {button $EMSegment(Cl-bGraphHistogram$index) -text $MenuText -width 8 -command "EMSegmentDrawDeleteCurveRegion 0 $index" } $Gui(WBA)
    $EMSegment(Cl-bGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorCode)
    $EMSegment(Cl-bGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorCode)
    TooltipAdd $EMSegment(Cl-bGraphHistogram$index) "Press button to display histogram of current active volume" 
    pack $EMSegment(Cl-bGraphHistogram$index) -side left -padx $Gui(pad)
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalcGaussian2DExtrem
# Calcualtes the maximum and minimum X Value for a given value 
# Axis :
# 0 = X - Axis
# 1 = Y - Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalcGaussian2DExtrem {GraphNum Sclass SqrtDetLogCov PI Axis} {
    global EMSegment
    set LogValue [expr -2.0*log(2.0*$PI*$SqrtDetLogCov*$EMSegment(Graph,MinDisplayValue,$GraphNum))] 
    set c12  $EMSegment(Graph,$Sclass,InvLogCov,1,2,$GraphNum) 
    set a [expr 1+$Axis]
    set b [expr 2-$Axis]
    set c11 $EMSegment(Graph,$Sclass,InvLogCov,$a,$a,$GraphNum) 
    set c22 $EMSegment(Graph,$Sclass,InvLogCov,$b,$b,$GraphNum)  
    set xmu $EMSegment(Graph,$Sclass,LogMean,$a,$GraphNum)
    set ymu $EMSegment(Graph,$Sclass,LogMean,$b,$GraphNum)
    # Calculte y+mu paramter 
    set c12sqr [expr $c12*$c12]
    set Norm [expr $c11*$c22  - $c12sqr]
    # This is the value of y including the mean
    set ySqrt [expr sqrt($c12sqr*$LogValue /($Norm*$c22))]
    for {set i 1} { $i <3} {incr i} {
    set blub1 [expr sqrt($LogValue*$c11 - $ySqrt*$ySqrt*$Norm)]
    set blub2  [expr pow(-1,$i)*$ySqrt*$c12] 
    set xMax($i)  [expr ($blub2 + $blub1)/$c11]
    set xMin($i)  [expr ($blub2 - $blub1)/$c11]
    }
    if {$xMax(2) > $xMax(1)} {set xMax(1) $xMax(2)}
    if {$xMin(2) > $xMin(1)} {set xMin(1) $xMin(2)}
    # Transform from Log Values to Gray Values 
    set Xmin [expr exp($xMin(1) + $xmu) - 1]
    set Xmax [expr exp($xMax(1) + $xmu) - 1]
    return "[EMSegmentGraphGridCalculate $GraphNum $Xmin $Axis] [EMSegmentGraphGridCalculate $GraphNum $Xmax $Axis]"
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDebug
# For debugging purposes 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDebug { } {
    global EMSegment Anno

    # Set EM Paramters
    set EMSegment(EMiteration)  1
    set EMSegment(MFAiteration) 1 
    set EMSegment(StartSlice)   1
    set EMSegment(Alpha)     0.9
    #set EMSegment(SmWidth)  23
    set EMSegment(SmWidth)  21
    #set EMSegment(SmSigma)  5
    set EMSegment(SmSigma)  9
    set EMSegment(Graph,XminNew,1) 100 
    set EMSegment(Graph,XmaxNew,1)  600
    set EMSegment(Graph,XscaNew,1) 50
    if {$EMSegment(NumGraph) > 1} {EMSegmentRescaleGraph 100 600 50 1 0}
    set EMSegment(PrintIntermediateResults) 0
    set EMSegment(Graph,DisplayProbNew) 1
    if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} {set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew); EMSegmentRedrawGraph}   

    # No Cross is shown in display
    set Anno(cross) 0
    MainAnnoSetVisibility; 
    RenderSlices

    # set First 60
    # set EMSegment(FileCIM) brainWar13.mrf
    # set EMSegment(EndSlice)   25
    #set First 70
    #set EMSegment(FileCIM) case3War9.mrf
    #set EMSegment(EndSlice) 10
    # set EMSegment(FileCIM) brainWar5c2Log.mrf
    if {$EMSegment(SegmentMode) < 2} {
    set EMSegment(FileCIM) mrf/simon/brainCase3_7c1.mrf  
    } else {
    # set EMSegment(FileCIM) mrf/simon/brainCase3_5_c2-notlocal.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase3_7_stg_c2_nonr-V2.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase19_6_stg_c2.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase3_7_am_c2.mrf
    set EMSegment(FileCIM) mrf/simon/brainCase3_7_stg_c2v3.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase3_9_am_para_c2.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase3_7_hip_c2.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase3_9c2.mrf
    set EMSegment(FileCIM) mrf/simon/brainCase3_13c2.mrf
    # set EMSegment(FileCIM) mrf/simon/brainCase50_6_stg_c2-V2final.mrf
        # set EMSegment(FileCIM) mrf/simon/brainCase3_9c2.mrf
    }
    # Read in Class Values
    EMSegmentImportCIM 0   
    # If you get an error message here graph could not read the volume 
    EMSegmentChangeVolumeGraph 1 0  
    if {$EMSegment(NumGraph) > 1} {EMSegmentChangeVolumeGraph 2 1}
    EMSegmentStartEM
}

