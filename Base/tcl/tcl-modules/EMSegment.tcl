#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for internal 
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
#   EMSegmentSaveSettingClass 
#   EMSegmentChangeSuperClassName
#   EMSegmentStartEM
#   EMSegmentClickLabel
#   EMSegmentDisplayClassDefinition
#   EMSegmentTransfereClassType
#   EMSegmentChangeSuperClass
#   EMSegmentUseSamples
#   EMSegmentFindClassAndTestfromIntClass
#   EMSegmentChangeClass
#   EMSegmentChangeIntensityClass 
#   EMSegmentUpdateClasses 
#   EMSegmentPlotCurveRegion
#   EMSegmentCalculateClassMeanCovariance
#   EMSegmentCalcProb
#   EMSegmentEraseSample  
#   EMSegmentCreateDeleteClasses  
#   EMSegmentSetMaxInputChannelDef
#   EMSegmentReadGreyValue
#   EMSegmentDefineSample
#   EMSegmentEnterDisplaySample 
#   EMSegmentLeaveSample 
#   EMSegmentChangeVolumeGraph
#   EMSegmentCreateGraphButton
#   EMSegmentMultipleDrawDeleteCurveRegion
#   EMSegmentDeleteGraphButton
#   EMSegmentDrawDeleteCurveRegion
#   EMSegmentExecuteCIM 
#   EMSegmentCreateCIMRowsColumns
#   EMSegmentSetCIMMatrix
#   EMSegmentChangeCIMMatrix
#   EMSegmentSuperClassChildren 
#   EMSegmentTrainCIMField
#   EMSegmentEndSlice 
#   EMSegmentAssignInput
#   EMSegmentSelectfromVolumeList
#   EMSegmentChangeVolumeSegmented 
#   EMSegmentTransfereVolume
#   EMSegmentUpdateVolumeList
#   EMSegmentDeleteFromSelList
#   EMSegmentCreateMeanCovarianceRowsColumns 
#   EMSegmentCreateGraphDisplayButton
#   EMSegmentShowGraphWindow
#   EMSegmentCreateGraphWindow
#   EMSegmentCreateHistogramButton
#   EMSegmentGraphXAxisUpdate path Xmin Xmax Xsca 
#   EMSegmentGraphYAxisUpdate path Ymin Ymax Ysca 
#   EMSegmentCreateDisplayRedLine 
#   EMSegmentCreateDisplayRedCross 
#   EMSegmentChangeDiceVolume
#   EMSegmentCalcDice
#   EMSegmentReadTextBox  
#   EMSegmentScrolledHorizontal  
#   EMSegmentScrolledText  
#   EMSegmentWriteTextBox  
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
        {$Revision: 1.21 $} {$Date: 2003/03/13 22:31:01 $}]

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

   # Sequence of how CIM values are represented
    # Where                      pixel to the North  
    #                                    |
    #              pixel to the West - pixel - pixel to the East
    #                                    |
    #                            pixel to the South
    # Pixel Up   = Pixel at same position from next slice
    # Pixel Down = Pixel at same position from previous slice
    set EMSegment(CIMList) {West North Up East South Down}

    # Currently there is a default setting in vtkImageEMEMSegment. If you want to ovewrwrite it
    # just comment out the part where the program loads the variavle from vtk, e.g.
    # replace 'set EMSegment(EMiteration)  [vtkEMInit GetNumIter]
    # with    'set EMSegment(EMiteration) 10

    # For Test phase this is the first super class 

    set EMSegment(GlobalSuperClassList) -1
    set EMSegment(GlobalClassList) ""
    set EMSegment(Cattrib,0,Name) "Head"
    set EMSegment(Cattrib,0,IsSuperClass) 1
    set EMSegment(NumClassesNew) 4 
    set EMSegment(Cattrib,0,Prob) 0.0 
    set EMSegment(Cattrib,0,ProbabilityData) $Volume(idNone) 
    set EMSegment(Cattrib,0,ClassList) ""
    set EMSegment(Cattrib,0,ColorCode) $Gui(activeWorkspace)
    set EMSegment(Cattrib,0,Label) $EMSegment(Cattrib,0,Name)
    set EMSegment(Cattrib,0,Node) ""

    foreach dir  $EMSegment(CIMList) { 
    set EMSegment(Cattrib,0,CIMMatrix,$dir,Node) ""
    }

    # Current Super Class
    set EMSegment(SuperClass) 0 

    set EMSegment(NewSuperClassName) $EMSegment(Cattrib,0,Name)
 
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

    set EMSegment(PrintIntermediateResults)     [vtkEMInit GetPrintIntermediateResults] 
    set EMSegment(PrintIntermediateSlice)       [vtkEMInit GetPrintIntermediateSlice] 
    set EMSegment(PrintIntermediateFrequency)   [vtkEMInit GetPrintIntermediateFrequency] 

    set EMSegment(RunRemoteFlag) 0
    set EMSegment(RunRemoteServer) ""

    set EMSegment(DICELabelList) ""
    set EMSegment(DICESelectedVolume) $Volume(idNone)
    set EMSegment(DICELabeledVolume) $Volume(idNone)
    set EMSegment(DICEVolumeList) {}

    set EMSegment(SegmentIndex) 0
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

    # Bias Field will not be printed
    set EMSegment(BiasPrint) 0
    set EMSegment(PrintIntermediateDir) "."

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
    # set EMSegment(NumGraph) 1
    # Define Histogram 
    set EMSegment(Cattrib,0,ColorGraphCode) #ffb2b2 

    # How many Graph buttons should be in one row 
    set EMSegment(Graph,ButtonNum) 10
    # Should Probablilites be displayed in graph or not
    set EMSegment(Graph,DisplayProb) 0
    set EMSegment(Graph,DisplayProbNew) $EMSegment(Graph,DisplayProb)

    if {$EMSegment(NumGraph) < 3} {
      set SingleGraphLength [expr int(350.0/$EMSegment(NumGraph))] 
    } else {
      set SingleGraphLength [expr int(350.0/(($EMSegment(NumGraph)-1)*2.0))] 
    }

    # Define Number Scalling in value difference
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
      # Kilian : Have to update it differently bc no control over it anymore 

      set EMSegment(Graph,$i,Xsca) 50
      # Min - Max values
      set EMSegment(Graph,$i,Xmin) 10
      set EMSegment(Graph,$i,Xmax) 260

      if {$i < 2} {
        set EMSegment(Graph,$i,Ysca) 0.5
        set EMSegment(Graph,$i,Ymin) 0.0
        set EMSegment(Graph,$i,Ymax) 1.0
    set EMSegment(Graph,$i,Yfor) "%0.2f"
        set EMSegment(Graph,$i,Dimension) 1
      } else {
        set EMSegment(Graph,$i,Ysca) $EMSegment(Graph,$i,Xsca)
        set EMSegment(Graph,$i,Ymin) $EMSegment(Graph,$i,Xmin)
        set EMSegment(Graph,$i,Ymax) $EMSegment(Graph,$i,Xmax)
      set EMSegment(Graph,$i,Yfor) "%3.0f"
        set EMSegment(Graph,$i,Dimension) 2
        # The Minimum Display value that should be displayed in the graph
        # Leave it away right now set EMSegment(Graph,MinDisplayValue,$i) 0.000000001  
      }

      # Define coord system len in pixels   
      set EMSegment(Graph,$i,Xlen) 370 
      set EMSegment(Graph,$i,Ylen) [expr $EMSegment(Graph,$i,Dimension)*$SingleGraphLength - 17] 
      # What Volume is set for the graph 
      for {set j 0} {$j < $EMSegment(Graph,$i,Dimension)} { incr j} {
      set EMSegment(Graph,$i,VolumeID,$j) -1
      set EMSegment(Graph,$i,LineID,$j) -1
      }
    }

    # Create Classes 
    EMSegmentCreateDeleteClasses 1 1
    
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
    # This has to be done here otherwise reboot does not work correctly
    set EMSegment(DisplaySampleFlag) 0

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
    bind $f.fCol2.eNumClasses <Return> "EMSegmentCreateDeleteClasses 1 1"
    bind $f.fCol2.eNumClasses <Tab>    "EMSegmentCreateDeleteClasses 1 1"
    bind $f.fCol2.eNumClasses <Leave>  "EMSegmentCreateDeleteClasses 1 1"

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
    eval {menubutton $f.fClass.mbClasses -text $EMSegment(Cattrib,$Sclass,Label) -menu $menu -width 8} $Gui(WMBA)
    $f.fClass.mbClasses configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    set EMSegment(EM-mbClasses) $f.fClass.mbClasses
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    # Add Selection entry 
    set MenuClassList [lsort -integer "$EMSegment(GlobalSuperClassList) $EMSegment(GlobalClassList)"]
    foreach i "$MenuClassList" {
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
    foreach i $MenuClassList {
        # change Menu Button when selected
        $menu add command -label "$EMSegment(Cattrib,$i,Label)" -command  "EMSegmentChangeClass $i" \
           -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 
    frame $f.fSuper -bg $Gui(activeWorkspace)
    pack $f.fSuper -side top -padx 2 -pady 2 -fill x

    if {$Sclass } { set text "Sub Class of $EMSegment(Cattrib,$EMSegment(SuperClass),Name)" 
    } else { set text ""}
    DevAddLabel $f.fSuper.lAssigned $text
    pack $f.fSuper.lAssigned -side right -padx $Gui(pad) -pady 0  
    set EMSegment(Cl-lAssigned) $f.fSuper.lAssigned 

    eval {checkbutton $f.fSuper.cSuperClass -text "Super Class" -variable EMSegment(Cattrib,$Sclass,IsSuperClass) -indicatoron 1 -command "EMSegmentTransfereClassType 1 1"} $Gui(WCA)
    TooltipAdd $f.fSuper.cSuperClass "If box is checked than the follwing class is a super class otherwise not. Be careful, when using the checkbox all definitons for this class might get lost !"
    pack $f.fSuper.cSuperClass -side left -padx $Gui(pad) -pady 0  
    set EMSegment(Cl-cSuperClass) $f.fSuper.cSuperClass
    if {($Sclass == 0) || ($EMSegment(SegmentMode) == 0)} { $EMSegment(Cl-cSuperClass) configure -state disabled}

    #--------------------------------------------
    # Class->Section 2
    #-------------------------------------------
    set f $fCl.fSec2
    # $f config -relief groove -bd 3    

    #-------------------------------------------
    # Class->Sec2 Frame: Class/Super Class Selection      
    #-------------------------------------------
    frame $f.fTabbedFrame -bg $Gui(activeWorkspace) -height 310
    pack $f.fTabbedFrame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    set ftab $f.fTabbedFrame  
    foreach tab {0 1} {
      frame $ftab.f$tab -bg $Gui(activeWorkspace)
      place $ftab.f$tab -in $ftab -relheight 1.0 -relwidth 1.0
    }
    raise $ftab.f$EMSegment(Cattrib,$Sclass,IsSuperClass)
    set EMSegment(Cl-fClass) $ftab

    #--------------------------------------------
    # Class->Section 2->body Frame
    #-------------------------------------------
    set f $EMSegment(Cl-fClass).f0
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
    set f $EMSegment(Cl-fClass).f0.frest

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

    EMSegmentCreateGraphDisplayButton $EMSegment(Cl-fClass).f0
    #-------------------------------------------
    # Class->Section 3: Display SuperClass properties   
    #-------------------------------------------
    set f $EMSegment(Cl-fClass).f1

    frame $f.fbody -bg $Gui(activeWorkspace) -relief sunken -bd 2 
    pack $f.fbody -side top -pady 2 -fill x 

    set f $f.fbody
    frame $f.fLeft -bg $Gui(activeWorkspace)
    frame $f.fRight -bg $Gui(activeWorkspace)
    pack $f.fLeft  $f.fRight -side left -padx 0 -pady 0

    DevAddLabel $f.fLeft.lName "Name:"  
    eval {entry $f.fRight.eName -width 8 -textvariable EMSegment(NewSuperClassName) } $Gui(WEA)
    set EMSegment(ClSuper-eName) $f.fRight.eName 

    TooltipAdd $f.fRight.eName "Define the Name of the super class"
    bind $f.fRight.eName <Return> "EMSegmentChangeSuperClassName 1"
    bind $f.fRight.eName <Tab>    "EMSegmentChangeSuperClassName 1"
    bind $f.fRight.eName <Leave>  "EMSegmentChangeSuperClassName 1"


    DevAddLabel $f.fLeft.lProb "Class Porbability:"
    eval {entry $f.fRight.eProb -width 4 -textvariable EMSegment(Cattrib,$Sclass,Prob) } $Gui(WEA)
    set EMSegment(ClSuper-eProb) $f.fRight.eProb

    DevAddLabel $f.fLeft.lNumClasses "Number of Classes:"  
    eval {entry $f.fRight.eNumClasses -width 4 -textvariable EMSegment(NumClassesNew) } $Gui(WEA)

    TooltipAdd $f.fRight.eNumClasses "Define Number of Classes"
    bind $f.fRight.eNumClasses <Return> "EMSegmentCreateDeleteClasses 1 1"
    bind $f.fRight.eNumClasses <Tab>    "EMSegmentCreateDeleteClasses 1 1"
    bind $f.fRight.eNumClasses <Leave>  "EMSegmentCreateDeleteClasses 1 1"

    pack $f.fLeft.lName $f.fLeft.lProb $f.fLeft.lNumClasses -side top -pady 5 -padx $Gui(pad) -anchor w 
    pack $f.fRight.eName $f.fRight.eProb $f.fRight.eNumClasses -side top -pady 3 -side top -anchor w
    
    EMSegmentCreateGraphDisplayButton $EMSegment(Cl-fClass).f1

    EMSegmentCreateGraphWindow  
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

    DevAddLabel $f.fAction.lStep1 "Step 1: Define Classes \n  Click on the class panel and define \n  every class."
    $f.fAction.lStep1 config -justify left
 
    DevAddLabel $f.fAction.lStep2 "Step 2: Segment Volume \n  Click on the EM-Panel and press the \n  'Segment' button." 
    $f.fAction.lStep2 config -justify left 

    DevAddLabel $f.fAction.lStep3 "Step 3: Select the Segmented Image"

    pack $f.fAction.lStep1 $f.fAction.lStep2 $f.fAction.lStep3 -side top -padx $Gui(pad) -pady 1 -anchor w

    frame $f.fAction.fSelection -bg $Gui(activeWorkspace) 
    pack $f.fAction.fSelection -side top -padx $Gui(pad) -pady $Gui(pad)

    DevAddSelectButton  EMSegment $f.fAction.fSelection VolumeSelect "" Pack "Volume to use for training" 25
    # Append menus and buttons to lists that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.fAction.fSelection.mbVolumeSelect
    lappend Volume(mActiveList) $f.fAction.fSelection.mbVolumeSelect.m


    DevAddLabel $f.fAction.lStep4 "Step 4: Start training for MRF-Paramters"
    pack $f.fAction.lStep4 -side top -padx $Gui(pad) -pady 1 -anchor w
    DevAddButton $f.fAction.bStep4 "Run Algorithm" EMSegmentTrainCIMField 

    DevAddLabel $f.fAction.lCaution "Caution: Training with the Training Volume \n will not only change the Random Field \n Definition but also class probablity !" 
    $f.fAction.lCaution configure -fg red
    
    pack $f.fAction.bStep4 $f.fAction.lCaution -side top -padx $Gui(pad) -pady $Gui(pad)

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
    DevAddLabel $f.lHead2 "Super Class: $EMSegment(Cattrib,$EMSegment(SuperClass),Name)"
    pack  $f.lHead $f.lHead2 -side top -padx $Gui(pad) -pady 0

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

    set f $EMSegment(CIM-fMatrix)
    frame $f.fLineL -bg $Gui(activeWorkspace)
    pack $f.fLineL -side top -padx 0 -pady 0 -fill x
    
    eval {label $f.fLineL.lLabel  -text "" -width 3} $Gui(WLA)
    pack $f.fLineL.lLabel -side left -padx $Gui(pad) -pady 1 
 
    EMSegmentCreateCIMRowsColumns 1 [expr [llength $EMSegment(Cattrib,0,ClassList)]+1]

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
        pack $f.fSect$i -side top -padx 0 -pady 0 -fill x
    }

    for {set i 1} {$i < 3} {incr i} {
    frame $f.fSect1.fCol$i -bg $Gui(activeWorkspace)
    pack $f.fSect1.fCol$i -side left -padx 0 -pady $Gui(pad) 
    }
    
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
               if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} { set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew); EMSegmentUpdateClasses 0 } }   
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
    foreach i $EMSegment(GlobalClassList) {
        # change Menu Button when selected
        $menu add command -label "$EMSegment(Cattrib,$i,Label)" -command "EMSegmentChangeIntensityClass $i 1" \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 

    DevAddLabel $f.fSect1.fCol1.lRunRemote "Run Remotely:"
    frame  $f.fSect1.fCol2.fRunRemote -bg $Gui(activeWorkspace)
    eval {entry $f.fSect1.fCol2.fRunRemote.eServer -width 6 -textvariable EMSegment(RunRemoteServer) } $Gui(WEA)
    set EMSegment(AL-eServer) $f.fSect1.fCol2.fRunRemote.eServer
    foreach value "1 0" text "On Off" width "4 4" {
       eval {radiobutton $f.fSect1.fCol2.fRunRemote.r$value -width $width -indicatoron 0\
               -text "$text" -value "$value" -variable EMSegment(RunRemoteFlag) -command { 
                   if {$EMSegment(RunRemoteFlag)} {$EMSegment(AL-eServer) configure -state normal
                   } else {$EMSegment(AL-eServer) configure -state disabled} 
           }
       } $Gui(WCA)
    }

    TooltipAdd $f.fSect1.fCol2.fRunRemote "You can run the segmentation remotly on another machin - enter machine name in the blank field"

    DevAddLabel $f.fSect1.fCol1.lDICEMeasure "Dice Measure:"
    frame  $f.fSect1.fCol2.fDICEMeasure -bg $Gui(activeWorkspace)
    DevAddSelectButton  EMSegment $f.fSect1.fCol2.fDICEMeasure  DICE "" Pack "Select Volume to calculate Dice Measure of segmentation result" 8

    $EMSegment(mbDICE) configure -text  [Volume($EMSegment(DICESelectedVolume),node) GetName]
    foreach vol  $EMSegment(DICEVolumeList) {
        $EMSegment(mDICE) add command -label [Volume($vol,node) GetName] -command "EMSegmentChangeDiceVolume $vol"
    }
    # Append menus and buttons to lists that get refreshed during UpdateMRML
    # lappend Volume(mbActiveList)  $f.fSect1.fCol2.fDICEMeasure.mbDICE
    # lappend Volume(mActiveList) $EMSegment(mDICE) 

    eval {entry $f.fSect1.fCol2.fDICEMeasure.eDICEMeasure -width 4 -textvariable EMSegment(DICELabelList) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.fDICEMeasure.eDICEMeasure "Enter labels to compare with segmented image for Dice Measure, e.g. 4 6"

    DevAddButton $f.fSect1.fCol2.fDICEMeasure.bDICEMeasure "Run" "EMSegmentCalcDice"

    DevAddButton $f.bSaveSetting "Save Setting" "EMSegmentSaveSetting 1"
    pack $f.bSaveSetting -side top -padx $Gui(pad) -pady 0


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

    DevAddLabel $f.fSect1.fCol1.lEmpty8 ""
    DevAddLabel $f.fSect1.fCol2.lEmpty8 ""

   
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

    if {$EMSegment(SegmentMode)} {
      pack $f.fSect1.fCol1.lEmpty8 $f.fSect1.fCol2.lEmpty8 -side top -padx $Gui(pad) -pady 1 -anchor w 

      pack $f.fSect1.fCol1.lRunRemote  -side top -padx $Gui(pad) -pady 2 -anchor w 
      pack $f.fSect1.fCol2.fRunRemote -side top -anchor w
      pack $f.fSect1.fCol2.fRunRemote.r1  $f.fSect1.fCol2.fRunRemote.r0 $f.fSect1.fCol2.fRunRemote.eServer -side left -fill x
 
      pack $f.fSect1.fCol1.lDICEMeasure  -side top -padx $Gui(pad) -pady 2 -anchor w 
      pack $f.fSect1.fCol2.fDICEMeasure -side top -anchor w
      pack $f.fSect1.fCol2.fDICEMeasure.eDICEMeasure $f.fSect1.fCol2.fDICEMeasure.bDICEMeasure -side left -fill x
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
    global EMSegment
    switch -exact $event {
    "Sample"  { if {$EMSegment(UseSamples) == 0} {
                  DevErrorWindow "Before taking samples, the 'Use Sample' button in the Class tab has to be checked !"
                  return
                }
                EMSegmentDefineSample [EMSegmentReadGreyValue $x $y 1]
                # Update Just Mean and Sigma Values
                EMSegmentCalculateClassMeanCovariance
                EMSegmentDisplayClassDefinition
              }
    "Enter"   { EMSegmentEnterDisplaySample $x $y}
    "Display" { EMSegmentEnterDisplaySample $x $y}
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
    global Mrml EMSegment Volume Gui
    # Current Desing of Node structure : (order is important !) 
    # Segmenter
    # -> SegmenterInput
    # -> SegmenterGraph
    # -> SegmenterClass 
    # -> SegmenterCIM 
    # -> SegmenterSuperClass
    #    -> SegmenterClass 
    #    -> SegmenterCIM 

    # Goal at some point
    # SegmenterSuperClass (Segmenter = SegmenterSuperClass '0')
    # -> SegmenterInput
    # -> SegmenterGraph
    # -> SegmenterClass
    # -> SegmenterCIM 

    # Caused by a subroutine call from UpdateMRML => do not want to call this function again
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
    # Store Parent SuperClasses and its left over children that have not yet been read in
    set SclassMemory ""

    # puts "---------------------- Start EMSegmentUpdateMRML ---------------"
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
        
        set NumberOfGraphs 0
        set EMSegment(SegmenterNode) $item
        # Current SupperClass
        set EMSegment(SuperClass) 0 
        # set EMSegment(Cattrib,0,ClassList) ""
        $EMSegment(SegmenterNode) SetAlreadyRead 1

        # Reset all Input and Graph Values
        set EMSegment(SegmenterGraphNodeList) ""
        set EMSegment(SegmenterInputNodeList) ""
        EMSegmentDeleteFromSelList $EMSegment(SelVolList,VolumeList)
        # --------------------------------------------------
        # 3.) Update variables 
        # -------------------------------------------------
        # If the path is not the same, define all Segmenter variables
        # Delete old values Kilian: I believe it would slow it down so do not do it => You have to delete all super classes individual => otherwise to much trash
        # Start new Values
        set EMSegment(NumClassesNew)              [Segmenter($pid,node) GetNumClasses]                 
        EMSegmentSetMaxInputChannelDef            [Segmenter($pid,node) GetMaxInputChannelDef]
        EMSegmentCreateDeleteClasses 1 0
        set CurrentClassList $EMSegment(Cattrib,0,ClassList)       
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
        if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} { set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew); EMSegmentUpdateClasses 0 }    
        set EMSegment(NumberOfTrainingSamples)    [Segmenter($pid,node) GetNumberOfTrainingSamples]
        set IntensityAvgClass                     [Segmenter($pid,node) GetIntensityAvgClass]    
        EMSegmentChangeClass 0
    } elseif {$ClassName == "vtkMrmlSegmenterGraphNode" } {
        # --------------------------------------------------
        # 4.) Only change Graph variables until graph 
        #     definitions extends the Number of Graphs 
        #     defined in the Module 
        # -------------------------------------------------
        if {($NumberOfGraphs < $EMSegment(NumGraph)) && ($NumberOfGraphs < 2)} {
          lappend EMSegment(SegmenterGraphNodeList)  $item 
          set pid [$item GetID]
      GraphRescaleAxis EMSegment $EMSegment(Graph,$NumberOfGraphs,path) [SegmenterGraph($pid,node) GetXmin] [SegmenterGraph($pid,node) GetXmax] [SegmenterGraph($pid,node) GetXsca] 0
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
    } elseif {$ClassName == "vtkMrmlSegmenterSuperClassNode" } {
        # --------------------------------------------------
        # 6.) Update variables for SuperClass 
        # -------------------------------------------------
        set pid [$item GetID]
        # If you get an error mesaage in the follwoing lines then CurrentClassList to short
        set NumClass [lindex $CurrentClassList 0]
        if {$NumClass == ""} { DevErrorWindow "Error in XML File : Super class $EMSegment(SuperClass)  has not a sub-classes defined" }
        
        # Save status when returning to parent of this class 
        set CurrentClassList [lrange $CurrentClassList 1 end]
        lappend SclassMemory [list "$EMSegment(SuperClass)" "$CurrentClassList"]

        # Transfer from Class to SuperClass
        set EMSegment(Class) $NumClass
        if {$EMSegment(Cattrib,$NumClass,IsSuperClass) == 0} {
           set EMSegment(Cattrib,$NumClass,IsSuperClass) 1
           # Set current class to current SuperClass
           EMSegmentTransfereClassType 0 0
        } else {
        set EMSegment(SuperClass) $NumClass
    }
        set EMSegment(NewSuperClassName) [SegmenterSuperClass($pid,node) GetName]
        EMSegmentChangeSuperClassName 0

        set EMSegment(Cattrib,$NumClass,Prob) [SegmenterSuperClass($pid,node) GetProb]
        # Create Sub Classes
        set EMSegment(NumClassesNew)          [SegmenterSuperClass($pid,node) GetNumClasses]       
        EMSegmentCreateDeleteClasses 0 0

        set EMSegment(Cattrib,$NumClass,Node) $item
        set CurrentClassList $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)
    } elseif {$ClassName == "vtkMrmlSegmenterClassNode" } {
        # --------------------------------------------------
        # 7.) Update selected Class List 
        # -------------------------------------------------
        # If you get an error mesaage in the follwoing lines then CurrentClassList to short
        set NumClass [lindex $CurrentClassList 0]
        if {$NumClass == ""} { DevErrorWindow "Error in XML File : Super class $EMSegment(SuperClass)  has not a sub-classes defined" }
        set CurrentClassList [lrange $CurrentClassList 1 end]

        set EMSegment(Class) $NumClass
        set pid [$item GetID]
        if {$EMSegment(Cattrib,$NumClass,IsSuperClass) == 1} {
           set EMSegment(Cattrib,$NumClass,IsSuperClass) 0
           # Set current class to current Class
           EMSegmentTransfereClassType 0 0
        }
        set EMSegment(Cattrib,$NumClass,Node)  $item

        EMSegmentClickLabel $NumClass [expr !$EMSegment(SuperClass)] [SegmenterClass($pid,node) GetLabel] 
        set EMSegment(Cattrib,$NumClass,Prob)   [SegmenterClass($pid,node) GetProb]

        set LocalPriorPrefix [SegmenterClass($pid,node) GetLocalPriorPrefix]
        set LocalPriorName   [SegmenterClass($pid,node) GetLocalPriorName]
        set LocalPriorRange  [SegmenterClass($pid,node) GetLocalPriorRange]
        set EMSegment(Cattrib,$NumClass,ProbabilityData) ""
        foreach VolID $Volume(idList) VolAttr $VolumeList {
            if {([lindex $VolAttr 0] == $LocalPriorName) && ([lindex $VolAttr 1] == $LocalPriorRange) && ([lindex $VolAttr 2] == $LocalPriorPrefix) } {
               set EMSegment(Cattrib,$NumClass,ProbabilityData) $VolID
               break;
            }
        }
        if {$EMSegment(Cattrib,$NumClass,ProbabilityData) == ""} {
           set EMSegment(Cattrib,$NumClass,ProbabilityData) $Volume(idNone) 
        }
        set index 0
        set LogCovariance  [SegmenterClass($pid,node) GetLogCovariance]
        set LogMean [SegmenterClass($pid,node) GetLogMean]
        for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
           set EMSegment(Cattrib,$NumClass,LogMean,$y) [lindex $LogMean $y]
           for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
              set EMSegment(Cattrib,$NumClass,LogCovariance,$y,$x)  [lindex $LogCovariance $index]
              incr index
           }
           # This is for the extra character at the end of the line (';')
           incr index
        }
    } elseif {$ClassName == "vtkMrmlSegmenterCIMNode" } {
        # --------------------------------------------------
        # 8.) Update selected CIM List 
        # -------------------------------------------------
        set pid [$item GetID]        
        set dir [SegmenterCIM($pid,node) GetName]
        if {[lsearch $EMSegment(CIMList) $dir] > -1} { 
          set i 0
          set CIMMatrix [SegmenterCIM($pid,node) GetCIMMatrix]
          set EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$dir,Node) $item
          foreach y $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
             foreach x $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
               set EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$x,$y,$dir) [lindex $CIMMatrix $i]
               incr i
             }
             incr i
          }
        }
    } elseif {$ClassName == "vtkMrmlEndSegmenterSuperClassNode" } {
        # --------------------------------------------------
        # 9.) End of super class 
        # -------------------------------------------------
        set EMSegment(Cattrib,$EMSegment(SuperClass),EndNode) $item
        # Pop the last parent from the Stack
        set temp [lindex $SclassMemory end]
        set SclassMemory [lreplace $SclassMemory end end]
        set CurrentClassList [lindex $temp 1] 
        EMSegmentChangeSuperClass [lindex $temp 0] 0
    } elseif {$ClassName == "vtkMrmlEndSegmenterNode" } {
        # --------------------------------------------------
        # 10.) End of Segmenter
        # -------------------------------------------------
        # if there is no EndSegmenterNode yet and we are reading one, and set
        # the EMSegment(EndSegmenterNode) variable
        if { $EMSegment(EndSegmenterNode) == ""} {
           set EMSegment(EndSegmenterNode) $item 
        }
        # EMSegmentChangeSuperClass 0   
        # set EMSegment(NumClassesNew) [llength $EMSegment(Cattrib,0,ClassList)]
    set EMSegment(Class) 0
        EMSegmentChangeClass 0
    }
    
    set item [Mrml(dataTree) GetNextItem]
  }
  if {$EMSegment(IntensityAvgClass) != $IntensityAvgClass} {
     EMSegmentChangeIntensityClass [lindex [EMSegmentFindClassAndTestfromIntClass $IntensityAvgClass] 0] 0
  }

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
# Deletes or adds Nodes to the list - just used for Input and graph => get rid of it 
# at some point
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentMRMLDeleteCreateNodesListEntries {Type New} {
    global EMSegment

    if {$Type == "EndSegmenterSuperClass"} {
    set NodeName $Type
    } else {
    set NodeName Segmenter${Type}
    }

    set Old [llength $EMSegment(${NodeName}NodeList)]
    if {$New == $Old} {return}

    if {$New < $Old} {
       # Delete Nodes that are to many 
       foreach id [lrange $EMSegment(${NodeName}NodeList) $New end] {
         MainMrmlDeleteNode $NodeName [$id GetID]
       }
       set EMSegment(${NodeName}NodeList) [lrange $EMSegment(${NodeName}NodeList) 0 [expr $New-1]]
    } else {
        # We Have to create new Nodes
        for {set i $Old} { $i <$New} { incr i} {
           set id [MainMrmlInsertBeforeNode $EMSegment(EndSegmenterNode) Segmenter${Type}]
           lappend EMSegment(${NodeName}NodeList) $id
        }
   }
   
   for {set i 0} { $i <$New} { incr i} {
       set id [lindex $EMSegment(Segmenter${Type}NodeList) $i]
       switch [string tolower $Type] {
           "graph" {Segmenter${Type}([$id GetID],node) SetName [expr $i+1]}
           "input" {Segmenter${Type}([$id GetID],node) SetName "Channel$i"} 
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSaveSetting 
# Updates the MRML Tree and saves the setting to a file  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSaveSetting {FileFlag {FileName -1}} {
    global EMSegment Volume Mrml
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
    set SuperClass 0
    Segmenter($pid,node) SetAlreadyRead                 1
    Segmenter($pid,node) SetNumClasses                  [llength $EMSegment(Cattrib,$SuperClass,ClassList)]
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
    # Kilian Change this here
    foreach id $EMSegment(SegmenterGraphNodeList) { 
       set pid [$id GetID]
       SegmenterGraph($pid,node) SetXmin  $EMSegment(Graph,$i,Xmin)
       SegmenterGraph($pid,node) SetXmax  $EMSegment(Graph,$i,Xmax)
       SegmenterGraph($pid,node) SetXsca  $EMSegment(Graph,$i,Xsca)
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
   # 4.) SegmenterSuperClass - Class - CIM
   # -------------------------------------------------------------------
   set EMSegment(Cattrib,$EMSegment(Class),ProbabilityData) $EMSegment(ProbVolumeSelect)
   EMSegmentSaveSettingSuperClass 0 $id
     
   # We can directly save as a file
   if {$FileFlag} {
       if {$FileName == -1} {MainMenu File SaveAs
   } else {MainMrmlWrite $FileName}
   }       
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSaveSettingClass 
# Just saves the class setting ! Necessary because of Super Class 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSaveSettingSuperClass {SuperClass LastNode} {
   global EMSegment Volume
   foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      if {$EMSegment(Cattrib,$i,IsSuperClass)} { 
      # Another Super Class
          # Check if UpdateNodeFlag is set => delete current node if it exists !
          if {$EMSegment(Cattrib,$i,Node) == ""} {set EMSegment(Cattrib,$i,Node) [MainMrmlInsertAfterNode $LastNode SegmenterSuperClass] }
          set pid [$EMSegment(Cattrib,$i,Node) GetID]
      SegmenterSuperClass($pid,node) SetName        $EMSegment(Cattrib,$i,Name)
          SegmenterSuperClass($pid,node) SetProb        $EMSegment(Cattrib,$i,Prob)  
          SegmenterSuperClass($pid,node) SetNumClasses  [llength $EMSegment(Cattrib,$i,ClassList)]

      set LastNode [EMSegmentSaveSettingSuperClass $i $EMSegment(Cattrib,$i,Node)]
      } else {
      # A normal class
      if {$EMSegment(Cattrib,$i,Node) == ""} {set EMSegment(Cattrib,$i,Node) [MainMrmlInsertAfterNode $LastNode SegmenterClass] }
          set pid [$EMSegment(Cattrib,$i,Node) GetID]
      set LastNode $EMSegment(Cattrib,$i,Node)

          # Set Values
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
          SegmenterClass($pid,node) SetLogCovariance "[lrange $LogCovariance 0 [expr [llength $LogCovariance]-2]]"
       }
   }
   # -------------------------------------------------------------------
   # Save CIM Setting for the SuperClass 
   # -------------------------------------------------------------------
   if {$SuperClass > 0}  {
       if {$EMSegment(Cattrib,$SuperClass,EndNode) == "" } {
       set EMSegment(Cattrib,$SuperClass,EndNode) [MainMrmlInsertAfterNode $LastNode EndSegmenterSuperClass] 
       }
       set LastNode $EMSegment(Cattrib,$SuperClass,EndNode)
    }
    foreach dir $EMSegment(CIMList) {
      # Set Values
      if {$EMSegment(Cattrib,$SuperClass,CIMMatrix,$dir,Node) == ""} {
      if {$SuperClass > 0} { set EMSegment(Cattrib,$SuperClass,CIMMatrix,$dir,Node) [MainMrmlInsertBeforeNode $LastNode SegmenterCIM] 
      } else {set EMSegment(Cattrib,$SuperClass,CIMMatrix,$dir,Node) [MainMrmlInsertAfterNode $LastNode SegmenterCIM]}
         set pid [$EMSegment(Cattrib,$SuperClass,CIMMatrix,$dir,Node) GetID]
         SegmenterCIM($pid,node) SetName $dir
      } else {
        set pid [$EMSegment(Cattrib,$SuperClass,CIMMatrix,$dir,Node) GetID]
      }
      set CIMMatrix ""
      foreach y $EMSegment(Cattrib,$SuperClass,ClassList) {
         foreach x $EMSegment(Cattrib,$SuperClass,ClassList) {
            lappend CIMMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$x,$y,$dir)
        }
        lappend CIMMatrix "|"
      }
      if {$CIMMatrix != ""} {  
          SegmenterCIM($pid,node) SetCIMMatrix "[lrange $CIMMatrix 0 [expr [llength $CIMMatrix]-2]]"  
      } 
    }

    return $LastNode
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeSuperClassName
# Change the name of the current Super Class
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeSuperClassName {Active} {
    global EMSegment
    set SuperClass $EMSegment(SuperClass)
    if {$EMSegment(Cattrib,$SuperClass,Name) == $EMSegment(NewSuperClassName)} {return}

    set EMSegment(Cattrib,$SuperClass,Name) $EMSegment(NewSuperClassName)
    set EMSegment(Cattrib,$SuperClass,Label) $EMSegment(Cattrib,$SuperClass,Name)

    $EMSegment(EM-mbClasses).m entryconfigure $SuperClass -label $EMSegment(Cattrib,$SuperClass,Label)
    $EMSegment(Cl-mbClasses).m entryconfigure $SuperClass -label $EMSegment(Cattrib,$SuperClass,Label)
    if {$Active} {
      $EMSegment(CIM-fDefinition).lHead2 config -text "Super Class: $EMSegment(Cattrib,$SuperClass,Name)"
      $EMSegment(EM-mbClasses) configure -text $EMSegment(Cattrib,$SuperClass,Label)
      $EMSegment(Cl-mbClasses) configure -text $EMSegment(Cattrib,$SuperClass,Label)
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentStartEM
# Starts the EM Algorithm 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentStartEM { } {
   global EMSegment Volume Mrml env tcl_platform
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
   # ----------------------------------------------
   # 3. Call Algorithm
   # ----------------------------------------------
   if {$EMSegment(RunRemoteFlag)} {
      # 1.) Save the file
      EMSegmentSaveSetting 1 [file join $env(SLICER_HOME) TempBlubber.xml]
      # 2.) Run the command
      puts "============================================================================"
      puts "Runing segmentation on  $EMSegment(RunRemoteServer) "
      # exec ssh $EMSegment(RunRemoteServer) "cd [file join $env(SLICER_HOME) Modules/EMLocalSegment/scripts/]; ./segmentSubject [file join $env(SLICER_HOME) TempBlubber.xml] 0 >> [file join $env(SLICER_HOME) EMSegment.log]"
      exec ssh $EMSegment(RunRemoteServer) "cd [file join $env(SLICER_HOME) Modules/EMLocalSegment/scripts/]; ./segmentSubject [file join $env(SLICER_HOME) TempBlubber.xml] 0"
      puts "Returning to Slicer"
      puts "============================================================================"      
      bell
      bell
      bell
   } else {
     set NumInputImagesSet [EMSegmentAlgorithmStart] 
   }  
   # ----------------------------------------------
   # 4. Write Back Results 
   # ----------------------------------------------
   set VolIndex [lindex $EMSegment(SelVolList,VolumeList) 0]
   incr EMSegment(SegmentIndex)
   set result [DevCreateNewCopiedVolume $VolIndex "" "EMSegResult$EMSegment(SegmentIndex)" ]
   set node [Volume($result,vol) GetMrmlNode]
   $node SetLabelMap 1
   Mrml(dataTree) RemoveItem $node 
   set nodeBefore [Volume($VolIndex,vol) GetMrmlNode]
   Mrml(dataTree) InsertAfterItem $nodeBefore $node
   # Display Result in label mode 
   Volume($result,vol) UseLabelIndirectLUTOn
   Volume($result,vol) Update
   Volume($result,node) SetLUTName -1
   Volume($result,node) SetInterpolate 0
   #  Write Solution to new Volume  -> Here the thread is called
   if {$EMSegment(RunRemoteFlag)} {
     Volume($result,node) SetFilePrefix [file join $Mrml(dir) EMResult]
     Volume($result,node) SetFullPrefix [file join $Mrml(dir) EMResult]
     if {$tcl_platform(byteOrder) == "littleEndian"} { Volume($result,node) SetLittleEndian 1
     } else { Volume($result,node) SetLittleEndian 0}    
     MainVolumesRead $result
   } else {
     Volume($result,vol) SetImageData [EMSegment(vtkEMSegment) GetOutput]
     EMSegment(vtkEMSegment) Update
     # ----------------------------------------------
     # 5. Recover Values 
     # ----------------------------------------------
     if {$EMSegment(SegmentMode) > 0} {
       set index 0
       foreach v $EMSegment(SelVolList,VolumeList) {
         if {$EMSegment(IntensityAvgValue,$v) < 0} {
           set EMSegment(IntensityAvgValue,$v) [EMSegment(vtkEMSegment) GetIntensityAvgValueCurrent $index]
         }
       incr index
       }
     }

     # Update MRML Tree
     EMSegmentSaveSetting 0
   }

   # Update MRML
   MainUpdateMRML
   
   # This is necessary so that the data is updated correctly.
   # If the programmers forgets to call it, it looks like nothing
   # happened
   MainVolumesUpdate $result

   # Display segmentation in window . can also be set to Back 
   MainSlicesSetVolumeAll Fore $result
   MainVolumesRender

   # ----------------------------------------------
   # 6. Clean up mess 
   # ----------------------------------------------
   if {$EMSegment(RunRemoteFlag)} {
       exec rm [file join $env(SLICER_HOME) TempBlubber.xml]
   } else {
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
   # ----------------------------------------------
   # 7. Run Dice measure if necessary 
   # ----------------------------------------------
   set EMSegment(DICELabeledVolume) $result
   EMSegmentCalcDice 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentClickLabel
# Sets everything correctly after user choosed label number and color 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentClickLabel {{Sclass ""} {ActiveGui ""} {label ""} {colorcode ""} } {
    global EMSegment Label Mrml Color 
    # if label ="" and everything else is "" it was called back from Label.tcl

    if {$Sclass == ""} {set  Sclass $EMSegment(Class)}
    # If the gui should be shanged too set ActiveGui = 1
    if {$ActiveGui == ""} {set ActiveGui 1}
 
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
    if {$EMSegment(Cattrib,$Sclass,IsSuperClass) == 0} {
    set RGB [GraphHexToRGB [string range $EMSegment(Cattrib,$Sclass,ColorCode) 1 6]]
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
        if {$EMSegment(Graph,$i,ID,$Sclass) > -1 } {
        EMSegment(Graph,$EMSegment(Graph,$i,path),vtkImageGraph) SetColor [EMSegment(Graph,$i,Data,$Sclass) GetOutput] [lindex $RGB 0] [lindex $RGB 1] [lindex $RGB 2]
        GraphRender EMSegment $EMSegment(Graph,$i,path)
        }
    }
    }

    $EMSegment(EM-mbClasses).m entryconfigure $Sclass -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)

    $EMSegment(Cl-mbClasses).m entryconfigure $Sclass -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)
    
    if {$EMSegment(Cattrib,$Sclass,IsSuperClass) == 0 } {
      set index [lsearch -exact $EMSegment(GlobalClassList) $Sclass]
      $EMSegment(DE-mbIntClass).m entryconfigure [expr $index+1] -background $EMSegment(Cattrib,$Sclass,ColorCode) \
        -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -label $EMSegment(Cattrib,$Sclass,Label)

      if {$index < $EMSegment(Graph,ButtonNum)} { 
        $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
          -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)
      } else {
        $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
          -activebackground $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)
      }
    }

    if {$ActiveGui == 0} {return}

    if {$EMSegment(Cattrib,$Sclass,IsSuperClass) == 0} {
       set f $EMSegment(CIM-fMatrix)
       $f.fLineL.l$Sclass configure -text "$EMSegment(Cattrib,$Sclass,Label)"
       $f.fLine$Sclass.lLabel configure -text "$EMSegment(Cattrib,$Sclass,Label)"
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
# .PROC EMSegmentTransfereClassType
# Transfere class type form Class to SuperClass and the otherway around 
# DeleteNode should be set to 1 if not called by UpdateMRML
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentTransfereClassType {ActiveGui DeleteNode} {
   global EMSegment Gui
   set Sclass $EMSegment(Class)
   if {$EMSegment(Cattrib,$Sclass,IsSuperClass)} {
     # Transfer from Class to SuperClass
     # 1.) Remove Graph Button
     EMSegmentDeleteGraphButton $Sclass

     # 2.) Remove from Class List and add to Super Class List
     set ClassIndex [lsearch -exact $EMSegment(GlobalClassList)  $Sclass]
     set EMSegment(GlobalClassList) [lreplace $EMSegment(GlobalClassList) $ClassIndex $ClassIndex] 

     # Put Class in the right order ! Important for class index in CreateDeleteClass
     set index 0 
     set ListLength [llength $EMSegment(GlobalSuperClassList)] 
     set i [lindex $EMSegment(GlobalSuperClassList) $index]
     while {($index <$ListLength) && ($i < $Sclass)} {
        incr index
        set i [lindex $EMSegment(GlobalSuperClassList) $index]
     }
     set EMSegment(GlobalSuperClassList) [linsert $EMSegment(GlobalSuperClassList) $index $Sclass] 

     # 3.) Define Class Paramters 
     set EMSegment(Cattrib,$Sclass,Name)      $EMSegment(Cattrib,$Sclass,Label)
     set Label $EMSegment(Cattrib,$Sclass,Label)
     set Color $Gui(activeWorkspace)

     # 4.) Change Class Panels 
     $EMSegment(DE-mbIntClass).m delete [expr $ClassIndex + 1] [expr $ClassIndex + 1]
     # Check if it is currently selected => if so change to none
     if {$EMSegment(IntensityAvgClass) == $Sclass} {EMSegmentChangeIntensityClass -1 1}

     # 5.) Delete Node!
     if {$EMSegment(Cattrib,$Sclass,Node) != "" && $DeleteNode} {
     MainMrmlDeleteNode SegmenterClass [$EMSegment(Cattrib,$Sclass,Node) GetID]
     set EMSegment(Cattrib,$Sclass,Node) ""
     }

     # 6.) Define new current SuperClass
     EMSegmentChangeSuperClass $Sclass $ActiveGui
   } else {
     # Transfer from SuperClass to Class
     # 1.) Remove all sub classes
     set EMSegment(NumClassesNew) 0
     EMSegmentCreateDeleteClasses 1 $DeleteNode

     # 2.) Remove from SuperClass List and add to Class List
     set index [lsearch -exact $EMSegment(GlobalSuperClassList)  $Sclass]
     set EMSegment(GlobalSuperClassList) [lreplace $EMSegment(GlobalSuperClassList) $index $index] 
     # Put Class in the right order ! Important for class index in CreateDeleteClass
     set ClassIndex 0
     set ClassListLength [llength $EMSegment(GlobalClassList)] 
     set i [lindex $EMSegment(GlobalClassList) $ClassIndex]
     while {($ClassIndex <$ClassListLength) && ($i < $Sclass)} {
        incr ClassIndex
        set i [lindex $EMSegment(GlobalClassList) $ClassIndex]
     }

     set  EMSegment(GlobalClassList) [linsert $EMSegment(GlobalClassList) $ClassIndex $Sclass] 
     incr ClassListLength

     # 3.) Create Graph Button
     set ColorLabelLength [expr [llength $EMSegment(ColorLabelList)] / 2]
     set Color            [lindex $EMSegment(ColorLabelList) [expr 2*(($Sclass-1)%$ColorLabelLength)]]
     set Label            [lindex $EMSegment(ColorLabelList) [expr 2*(($Sclass-1)%$ColorLabelLength)+1]]
     
     EMSegmentCreateGraphButton $Sclass $Label $Color 

     # 4.) Change Class Panel 
     # Add to Intensity correction 
     $EMSegment(DE-mbIntClass).m add command 
     # Move the buttons in the right order
     set List [lrange $EMSegment(GlobalClassList) $ClassIndex end]
     set index $ClassIndex
     foreach i $List {
     $EMSegment(DE-mbIntClass).m entryconfigure $index -background $EMSegment(Cattrib,$i,ColorCode) \
         -activebackground $EMSegment(Cattrib,$i,ColorCode) -label $EMSegment(Cattrib,$i,Label) -command "EMSegmentChangeIntensityClass $i 1" 
     incr index
     } 
     # Enable Buttons !
     # 5.) Define new current SuperClass
     set index 0
     set ListLength [llength $EMSegment(GlobalSuperClassList)] 
     set i [lindex $EMSegment(GlobalSuperClassList) $index]
     while {($index < $ListLength) && ([lsearch -exact $EMSegment(Cattrib,$i,ClassList) $Sclass] == -1)} {
        incr index
        set i [lindex $EMSegment(GlobalSuperClassList) $index]
     }

     # Delete Node
     if {($EMSegment(Cattrib,$Sclass,Node) != "") && $DeleteNode} {
     MainMrmlDeleteNode SegmenterSuperClass [$EMSegment(Cattrib,$Sclass,Node) GetID]
     foreach dir $EMSegment(CIMList) {
         if {$EMSegment(Cattrib,$Sclass,CIMMatrix,$dir,Node) != ""}  {
         MainMrmlDeleteNode SegmenterCIM [$EMSegment(Cattrib,$Sclass,CIMMatrix,$dir,Node) GetID]
         set EMSegment(Cattrib,$Sclass,CIMMatrix,$dir,Node) ""
         }
     } 
     if {$EMSegment(Cattrib,$Sclass,EndNode) != ""} { 
         MainMrmlDeleteNode EndSegmenterSuperClass [$EMSegment(Cattrib,$Sclass,EndNode) GetID] 
         set EMSegment(Cattrib,$Sclass,EndNode) ""
     }
     set EMSegment(Cattrib,$Sclass,Node) ""
     }
     EMSegmentChangeSuperClass $i $ActiveGui 
  } 
  # Define Class Label and Color
  EMSegmentClickLabel $Sclass 1 $Label $Color
  raise $EMSegment(Cl-fClass).f$EMSegment(Cattrib,$Sclass,IsSuperClass)
  focus $EMSegment(Cl-fClass).f$EMSegment(Cattrib,$Sclass,IsSuperClass)
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeSuperClass
# Changes Active Super Class 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeSuperClass {NewSuperClass ActiveGui} {
   global EMSegment
   # Remove all the CIMS and Create New Once
   if {$NewSuperClass == $EMSegment(SuperClass)} {return} 
    
   if {$ActiveGui} {
     set f $EMSegment(CIM-fMatrix) 
     foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
       destroy $f.fLineL.l$i
       destroy $f.fLine$i
     }
   }  
   # Setup new paramters
   set EMSegment(SuperClass) $NewSuperClass
   set EMSegment(NumClassesNew) [llength $EMSegment(Cattrib,$NewSuperClass,ClassList)]
   set EMSegment(NewSuperClassName) $EMSegment(Cattrib,$NewSuperClass,Name)

   if {$ActiveGui} {
     $EMSegment(CIM-fDefinition).lHead2 config -text "Super Class: $EMSegment(Cattrib,$EMSegment(SuperClass),Name)"
     EMSegmentCreateCIMRowsColumns "$EMSegment(Cattrib,$NewSuperClass,ClassList)"
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
    foreach IntClass $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
      if {$IntLabel == $EMSegment(Cattrib,$IntClass,Label)} {
    return "$IntClass $IntLabel"
      }
    } 
    return "-1 None"
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeClass
# Changes from one to another class and displays new class in window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeClass {Sclass} {
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
    set EMSegment(Class) $Sclass

    # Change Super Class Setting
    if {$Sclass && $EMSegment(SegmentMode)} { $EMSegment(Cl-cSuperClass) configure -state normal 
    } else {$EMSegment(Cl-cSuperClass) configure -state disabled }
    
    # Change Variable the Entry field is assigned width
    for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} { 
      $EMSegment(EM-fMeanVar).e$y config -textvariable  EMSegment(Cattrib,$Sclass,LogMean,$y) 
      $EMSegment(Cl-fLogMeanVar).e$y config -textvariable  EMSegment(Cattrib,$Sclass,LogMean,$y) 
      for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} { 
        $EMSegment(Cl-fLogCovVar).fLine$y.e$x config -textvariable  EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x)
      }
    }
    # Changes due to Super Class
    set index 0
    set ListLength [llength $EMSegment(GlobalSuperClassList)] 
    set i [lindex $EMSegment(GlobalSuperClassList) $index]
    while {($index < $ListLength) && ([lsearch -exact $EMSegment(Cattrib,$i,ClassList) $Sclass] == -1)} {
       incr index
       set i [lindex $EMSegment(GlobalSuperClassList) $index]
    }
    if {$EMSegment(Cattrib,$Sclass,IsSuperClass)} {
       EMSegmentChangeSuperClass $Sclass 1
    } else {
       EMSegmentChangeSuperClass $i 1
    }
    # For Head Class
    if {$i == ""} {
       $EMSegment(Cl-lAssigned) config -text "" 
    } else {
       $EMSegment(Cl-lAssigned) config -text "Sub Class of $EMSegment(Cattrib,$i,Name)" 
    }
    $EMSegment(Cl-cSuperClass) config -variable EMSegment(Cattrib,$Sclass,IsSuperClass)

    $EMSegment(EM-eProb)  config  -textvariable EMSegment(Cattrib,$Sclass,Prob)
    $EMSegment(Cl-eProb)  config  -textvariable EMSegment(Cattrib,$Sclass,Prob)
    $EMSegment(ClSuper-eProb) config -textvariable EMSegment(Cattrib,$Sclass,Prob)

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

    # Raise the right panel 
    raise $EMSegment(Cl-fClass).f$EMSegment(Cattrib,$Sclass,IsSuperClass)
    focus $EMSegment(Cl-fClass).f$EMSegment(Cattrib,$Sclass,IsSuperClass)
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
# .PROC  EMSegmentUpdateClasses 
# Updates the class vlaues and plots the new curves 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateClasses {flag} {
    global EMSegment
    EMSegmentCalculateClassMeanCovariance
    EMSegmentCalcProb
    # Kilian - Just one 1D right now 
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {($EMSegment(Graph,$i,VolumeID,0)  == -1) && ($flag)} {
        DevErrorWindow "Please select a volume at the yellow graph button before triing to plot a histogram!"
        return -1
        } else {
        EMSegmentPlotCurveRegion $i
    }
    }
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentPlotCurveRegion
# Plot the curves of a graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentPlotCurveRegion {numGraph} {
    global EMSegment
    # Update History Plot
    if {$EMSegment(Graph,$numGraph,Dimension) == 1} { 
    set NumIndex [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,$numGraph,VolumeID,0)]
    if {$NumIndex > -1} {
        EMSegment(Graph,$numGraph,Data,0)Accu SetInput [Volume($EMSegment(Graph,$numGraph,VolumeID,0),vol) GetOutput]
        EMSegment(Graph,$numGraph,Data,0)Accu Update
        EMSegment(Graph,$numGraph,Data,0)Res Update
    }

    # Update Classes
    foreach j $EMSegment(GlobalClassList) {
        if {$NumIndex > -1} {
        EMSegment(Graph,$numGraph,Data,$j) SetMean        $EMSegment(Cattrib,$j,LogMean,$NumIndex)  0
            EMSegment(Graph,$numGraph,Data,$j) SetCovariance  $EMSegment(Cattrib,$j,LogCovariance,$NumIndex,$NumIndex) 0 0
        }
        EMSegment(Graph,$numGraph,Data,$j) SetProbability [expr ($EMSegment(Graph,DisplayProb) > 0 ?  $EMSegment(Cattrib,$j,Prob) : 1.0)]
        EMSegment(Graph,$numGraph,Data,$j) Update
    }
    } else {
    set NumIndex(0) [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,$numGraph,VolumeID,0)]
    set NumIndex(1) [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,$numGraph,VolumeID,1)]
    # Update Classes
    foreach j $EMSegment(GlobalClassList) {
        if {($NumIndex(0) > -1) && ($NumIndex(1) > -1)} {
        for {set y 0 } { $y < 2} {incr y} {
            EMSegment(Graph,$numGraph,Data,$j) SetMean        $EMSegment(Cattrib,$j,LogMean,$NumIndex($y))  $y
            for {set x 0 } {$x < 2} {incr x} {
            EMSegment(Graph,$numGraph,Data,$j) SetCovariance  $EMSegment(Cattrib,$j,LogCovariance,$NumIndex($y),$NumIndex($x)) $y $x 
            }
        }
        } 
        EMSegment(Graph,$numGraph,Data,$j) SetProbability [expr ($EMSegment(Graph,DisplayProb) > 0 ?  $EMSegment(Cattrib,$j,Prob) : 1.0)]
        EMSegment(Graph,$numGraph,Data,$j) Update
    }
    }
    GraphRender EMSegment $EMSegment(Graph,$numGraph,path)
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
                set EMSegment(Cattrib,$Sclass,LogCovariance,$x,$y) $EMSegment(Cattrib,$Sclass,LogCovariance,$y,$x) 
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
    # Super Class currently has not probability
    foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
       set NormProb [expr $NormProb + $EMSegment(Cattrib,$i,Prob)]
    }  
    # Round it up or down otherwise it recalculates everything every time this function is called  
    if {($NormProb == 0.0) || ($NormProb == 1.0)} {return} 

    set OldNorm $NormProb
    foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) { 
      set EMSegment(Cattrib,$i,Prob) [expr round($EMSegment(Cattrib,$i,Prob) / $NormProb * 100) / 100.0]
      set OldNorm [expr $OldNorm - $EMSegment(Cattrib,$i,Prob)]
    }  
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
# .PROC  EMSegmentCreateDeleteClasses  
# Creates or deletes classes for segmentation 
# If Gui should be changed too set ChangeGui > 0
# If it is not called from UpdateMRML DeleteNode == 1
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateDeleteClasses {ChangeGui DeleteNode} {
    global EMSegment Volume Gui
    # Look for super class ! 
    set NumClasses [llength $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)]
    if {$EMSegment(NumClassesNew) == $NumClasses} { return  }
    # ---------------------------------------------------------------------------------
    # Now $EMSegment(NumClassesNew) < $NumClasses 
    # => Delete Menue Buttons
    # ---------------------------------------------------------------------------------
    if {$EMSegment(NumClassesNew) < $NumClasses} {
       # Update Button if necessary
       set DeleteList [lrange $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) $EMSegment(NumClassesNew) end]
       set f $EMSegment(CIM-fMatrix)

       foreach i $DeleteList { 
          # It is a super class => destroy also all sub classes
          if {$EMSegment(Cattrib,$i,IsSuperClass)} {
            set SuperClass $EMSegment(SuperClass)
            set NumClassesNew $EMSegment(NumClassesNew)
            set EMSegment(SuperClass) $i
            set EMSegment(NumClassesNew) 0
            EMSegmentCreateDeleteClasses 0 $DeleteNode
            set EMSegment(SuperClass) $SuperClass 
            set EMSegment(NumClassesNew) $NumClassesNew
         
            # Delete from menu list 
            set index 0
            foreach j $EMSegment(GlobalClassList) {
              if {$j > $i} {break}
              incr index
            } 
            set SCindex [lsearch -exact $EMSegment(GlobalSuperClassList) $i]   
            incr index $SCindex
          
            $EMSegment(Cl-mbClasses).m delete $index $index
            $EMSegment(EM-mbClasses).m delete $index $index
          
            # Remove from Global list
            set EMSegment(GlobalSuperClassList) [lreplace $EMSegment(GlobalSuperClassList) $SCindex $SCindex]  
         

            # Delete Node from Graph and unset 
            if {$EMSegment(Cattrib,$i,Node) != "" && $DeleteNode} { 
              MainMrmlDeleteNode SegmenterSuperClass [$EMSegment(Cattrib,$i,Node) GetID]
              foreach dir $EMSegment(CIMList) {
                if {$EMSegment(Cattrib,$i,CIMMatrix,$dir,Node) != ""}  {MainMrmlDeleteNode SegmenterCIM [$EMSegment(Cattrib,$i,CIMMatrix,$dir,Node) GetID]}
              } 
              if {$EMSegment(Cattrib,$i,EndNode) != ""} { MainMrmlDeleteNode EndSegmenterSuperClass [$EMSegment(Cattrib,$i,EndNode) GetID] }
       }
        } else {
            # ----------------------------------------------------
            # Delete normal class
            # ----------------------------------------------------
            # Delete the Graph Button before deleting it out of the Global List
            EMSegmentDeleteGraphButton $i
            # Delete from menu list 
            set index 0
            foreach j $EMSegment(GlobalSuperClassList) {
              if {$j > $i} {break}
              incr index
            } 
            set Cindex [lsearch -exact $EMSegment(GlobalClassList) $i]  
            incr index $Cindex
            
            $EMSegment(Cl-mbClasses).m delete $index $index
            $EMSegment(EM-mbClasses).m delete $index $index
            $EMSegment(DE-mbIntClass).m delete $Cindex $Cindex 
       
            # Remove from Global list
            set EMSegment(GlobalClassList) [lreplace $EMSegment(GlobalClassList) $Cindex $Cindex]  

            # Delete Node from Graph 
            if {($EMSegment(Cattrib,$i,Node) != "") && $DeleteNode} { MainMrmlDeleteNode SegmenterClass [$EMSegment(Cattrib,$i,Node) GetID] }
    }
    
        # Unset variables - CIM Variable is unset a little bit later
        unset EMSegment(Cattrib,$i,Node)     
        unset EMSegment(Cattrib,$i,EndNode)
        unset EMSegment(Cattrib,$i,IsSuperClass)
     
        if  {$EMSegment(Cattrib,$i,Label) ==  $EMSegment(IntensityAvgClass)} {
          EMSegmentChangeIntensityClass -1 1
        }
        # Delete Additonal CIM Fields
        # Free the variables 
        if {$ChangeGui} { 
          destroy $f.fLineL.l$i
          destroy $f.fLine$i
        }
     
        foreach k $EMSegment(CIMList) {
          unset EMSegment(Cattrib,$i,CIMMatrix,$k,Node)
          foreach j $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
          unset EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$i,$j,$k) 
          if {[lsearch $DeleteList $j] < 0} {
             if {$ChangeGui} {destroy $f.fLine$j.eCol$i}
             unset EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$j,$i,$k)
          } 
        }
    }
    # Class Definition
    unset EMSegment(Cattrib,$i,ColorCode) EMSegment(Cattrib,$i,Label)  
    unset EMSegment(Cattrib,$i,Prob) EMSegment(Cattrib,$i,ProbabilityData)
    
    for {set y 0} {$y <  $EMSegment(MaxInputChannelDef)} {incr y} {
        unset  EMSegment(Cattrib,$i,LogMean,$y)
        for {set x 0} {$x <  $EMSegment(MaxInputChannelDef)} {incr x} { 
        unset EMSegment(Cattrib,$i,LogCovariance,$y,$x) 
        }
    }
    # Sample List
    foreach v $EMSegment(SelVolList,VolumeList) {
        unset EMSegment(Cattrib,$i,$v,Sample) 
    }
      }
      # Destory elements from list
      set EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) [lrange $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) 0 [expr $EMSegment(NumClassesNew) -1]] 
      # Reconfigure width and height of canvas
      if {$ChangeGui} {EMSegmentSetCIMMatrix} 
    
      # Update Values of new Class
      # Recaluclate Probabilites
      EMSegmentCalcProb
      # Have to change this to first class of child or other!
      if {$ChangeGui && ($EMSegment(SuperClass) !=  $EMSegment(Class))} {EMSegmentChangeClass [lindex $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) 0]}
      return
    } 

    # ---------------------------------------------------------------------------------
    # Now $EMSegment(NumClassesNew) > $NumClasses 
    # => Delete Menue Buttons
    # ---------------------------------------------------------------------------------
    set ColorLabelLength [expr [llength $EMSegment(ColorLabelList)] / 2]
    if {$EMSegment(GlobalSuperClassList) == -1} {
       set Cstart 1
       set Cprob [expr 1 / double($EMSegment(NumClassesNew))]
       set EMSegment(GlobalSuperClassList) "0"
       set ClassInit 1
    } else {
       set GCL [lindex $EMSegment(GlobalClassList) end]
       set GSCL [lindex $EMSegment(GlobalSuperClassList) end]
       if {$GCL == ""} {
          if {$GSCL ==""} {set Cstart 0
          } else { set Cstart $GSCL}
       } else {
          if {$GSCL ==""} {set Cstart $GCL
          } else { set Cstart [expr ($GSCL > $GCL ? $GSCL : $GCL)]}
       }
       incr Cstart 
       set Cprob 0.0
       set ClassInit 0
    }
    set Cfinish [expr $EMSegment(NumClassesNew) - $NumClasses + $Cstart]

    for {set i $Cstart} {$i < $Cfinish } {incr i 1} {
      lappend EMSegment(GlobalClassList) $i
      lappend EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) $i 

      set EMSegment(Cattrib,$i,Prob) $Cprob 
      set EMSegment(Cattrib,$i,ClassList) ""

      set EMSegment(Cattrib,$i,Node)  ""   
      set EMSegment(Cattrib,$i,EndNode) ""
      set EMSegment(Cattrib,$i,IsSuperClass) 0

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
    }
    # Define CIM Field as Matrix M(Class1,Class2,Relation of Pixels)
    # where the "Relation of the Pixels" can be set as Pixel with "left", 
    # "right", "up" or "down" Neighbour  
    # EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,<y>,<x>,<Type>)
    # Add new Rows and lines to the CIM Matrix
    foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
      # Just update only new Row elements or whole line 
      if { $i < $Cstart} { set jlist  [lrange $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) $NumClasses end] 
      } else { set jlist $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)}
      foreach k $EMSegment(CIMList) {
     if { $i >= $Cstart} {set EMSegment(Cattrib,$i,CIMMatrix,$k,Node) ""}
         foreach j $jlist {
            if {$i == $j} {
        set EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$i,$j,$k) 1
        set EMSegment(Cattrib,$j,CIMMatrix,$k,Node) ""

            } else  {set EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$i,$j,$k) 0}
         }
      }
    }
    # ---------------------------------------------------------------------------------    
    # Initializing GUI: Class never has changed before or GUI is not changed
    # ---------------------------------------------------------------------------------
    if {$ClassInit} {return}

    # ---------------------------------------------------------------------------------    
    # Add new classes on GUI
    # ---------------------------------------------------------------------------------
    # When all classes should be displayed this has to change

    for {set i $Cstart} {$i < $Cfinish} {incr i 1} {
    # change Menu Button when selected
    $EMSegment(Cl-mbClasses).m add command -label "$EMSegment(Cattrib,$i,Label)" -command  "EMSegmentChangeClass $i" \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode) 
    $EMSegment(EM-mbClasses).m add command -label "$EMSegment(Cattrib,$i,Label)" -command  "EMSegmentChangeClass $i" \
             -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode) 
    $EMSegment(DE-mbIntClass).m add command -label "$EMSegment(Cattrib,$i,Label)" -command "EMSegmentChangeIntensityClass $i 1" \
        -background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)

        EMSegmentCreateGraphButton $i $EMSegment(Cattrib,$i,Label) $EMSegment(Cattrib,$i,ColorCode)
    }
    if {$ChangeGui} {EMSegmentCreateCIMRowsColumns $Cstart $Cfinish}
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
      # Kilian : Has to be changed later if we have different input channels for different classes !
       set List "$EMSegment(GlobalSuperClassList) $EMSegment(GlobalClassList)"
       for {set ind $EMSegment(MaxInputChannelDef)} {$ind < $NewMaxInputChannelDef} {incr ind} { 
          foreach c $List {
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
# if flag is set it also gives coordinates 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadGreyValue {x y flag} {
    global EMSegment Interactor
    if {$EMSegment(NumInputChannel) == 0 } {return}     
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
      if {$flag} {
      lappend result "$x $y $zIjk $pixel" 
      } else {
      lappend result $pixel 
      }
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
# .PROC EMSegmentEnterDisplaySample 
# Sets up everything when the mouse enters the graph with the image to be sampled
# or if DisplayFlag is set it just adjus values/lines/cross to the current position 
# of the mouse 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEnterDisplaySample {x y} {
    global EMSegment Gui
    if {$EMSegment(DisplaySampleFlag) || ([wm state $Gui(wEMSegment)] == "withdrawn") || ($EMSegment(Graph,0,VolumeID,0) < 0)} {return}
    set EMSegment(DisplaySampleFlag) 1
    set SampleList [EMSegmentReadGreyValue $x $y 0]
    set Value(0) [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,0,VolumeID,0)]] 
    EMSegmentCreateDisplayRedLine 0 $Value(0)
    if {$EMSegment(NumGraph) > 1} {
    if {$EMSegment(Graph,1,VolumeID,0) <  0 } { 
       set EMSegment(DisplaySampleFlag) 0
       return 
    }
    set Value(1) [lindex $SampleList [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,1,VolumeID,0)]] 
    EMSegmentCreateDisplayRedLine 1 $Value(1)
    if {$EMSegment(NumGraph) > 2} { EMSegmentCreateDisplayRedCross 2 $Value(0) $Value(1) }
    }
    set EMSegment(DisplaySampleFlag) 0
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentLeaveSample 
# Destroys everything after the mouse leaves the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentLeaveSample { } {
    global EMSegment Gui
    if {[wm state $Gui(wEMSegment)] == "withdrawn"} {return} 
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
    if {$EMSegment(Graph,$i,LineID,0) > -1} {
        GraphDeleteLine EMSegment $EMSegment(Graph,$i,path) $EMSegment(Graph,$i,LineID,0)
        GraphDeleteValueDisplay EMSegment $EMSegment(Graph,$i,path)
        set EMSegment(Graph,$i,LineID,0) -1
        if {$i > 1} {
        GraphDeleteLine EMSegment $EMSegment(Graph,$i,path) $EMSegment(Graph,$i,LineID,1)
        set EMSegment(Graph,$i,LineID,1) -1
        }
        GraphRender EMSegment $EMSegment(Graph,$i,path)
    }
    }
} 

#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeVolumeGraph
# Changes the Volume of the graph for which everything is plotted
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeVolumeGraph {VolumeID numGraph} {
    global EMSegment
    set VolName [Volume($VolumeID,node) GetName]
    set EMSegment(Graph,$numGraph,VolumeID,0) $VolumeID
    if {$EMSegment(NumGraph) == 3} {
    set EMSegment(Graph,2,VolumeID,$numGraph) $VolumeID
    }
    if {$EMSegment(SegmentMode)} {
      EMSegmentPlotCurveRegion $numGraph
      if {$EMSegment(NumGraph) == 3} {
    EMSegmentPlotCurveRegion 2
      }
      $EMSegment(Cl-mbGraphHistogram$numGraph) configure -text $VolName
    } else {
       $EMSegment(Cl-bGraphHistogram$numGraph) configure -text $VolName
    }
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraphButton
# Creates for Class <Sclass> a Button so the class distribution can be diplayed 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphButton {Sclass Label Color {Above 0} {UpdateGraph 1}} {
    global EMSegment Gui
    set index [lsearch $EMSegment(GlobalClassList) $Sclass]
    set TooltipText "Press button to display Gaussian of $Sclass class  (Label: $Label)" 

    if {($index < $EMSegment(Graph,ButtonNum) ) || $Above} {
       set f $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass
    } else {
       set f $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass
    }
    eval {button $f -text $Label -width 3 -command "EMSegmentMultipleDrawDeleteCurveRegion $Sclass"} $Gui(WBA)
    $f configure -bg $Color -activebackground $Color
    pack $f -side left -padx $Gui(pad)
    TooltipAdd $f $TooltipText
    if {$UpdateGraph} {
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
        # Generate setup for each curve 
        set EMSegment(Graph,$i,ID,$Sclass) -1
        set mean ""
        set cov ""
        set SetFlag 1
        for {set y 0 } { $y < $EMSegment(Graph,$i,Dimension) } {incr y} {
        set NumIndex($y) [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,$i,VolumeID,$y)]
        if {$NumIndex($y) < 0} { set SetFlag 0} 
        }
      
        for {set y 0 } { $y < $EMSegment(Graph,$i,Dimension) } {incr y} {
        if {$SetFlag} {
            lappend mean $EMSegment(Cattrib,$Sclass,LogMean,$NumIndex($y))
        } else {
            lappend mean 0.0
        }
        for {set x 0 } { $x < $EMSegment(Graph,$i,Dimension) } {incr x} {
           if {$SetFlag} {
            lappend cov $EMSegment(Cattrib,$Sclass,LogCovariance,$NumIndex($y),$NumIndex($x)) 
            } else {
            if {$y == $x} {lappend cov 1.0
                } else { lappend cov 0.0}
            }
        }
        }
        set Prob [expr ($EMSegment(Graph,DisplayProb) > 0 ?  $EMSegment(Cattrib,$Sclass,Prob) : 1.0)]
        GraphCreateGaussianCurveRegion EMSegment(Graph,$i,Data,$Sclass) $mean $cov $Prob 2 $EMSegment(Graph,$i,Dimension) $EMSegment(Graph,$i,Xmin) $EMSegment(Graph,$i,Xmax) $EMSegment(Graph,$i,Xlen) $EMSegment(Graph,$i,Ymin) $EMSegment(Graph,$i,Ymax) $EMSegment(Graph,$i,Ylen) 
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
# Delete Graph Button and curve for Class <Sclass> 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDeleteGraphButton {Sclass} {
    global EMSegment
    # Delete Button
    set index [lsearch $EMSegment(GlobalClassList) $Sclass]
    # It was a super class before so nothing is to be distroyed
    if {$index == -1} {return}
    if {$index < $EMSegment(Graph,ButtonNum)} { 
       destroy $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass 
       set NextClass [lindex $EMSegment(GlobalClassList) $EMSegment(Graph,ButtonNum)]
       # Move up a line 
       if {$NextClass != ""} {
         destroy $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$NextClass
         EMSegmentCreateGraphButton $NextClass $EMSegment(Cattrib,$NextClass,Label)  $EMSegment(Cattrib,$NextClass,ColorCode) 1 0
       }
    } else {
       destroy $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass 
    }
    # Delete curve instance !
    for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
       
       if {$EMSegment(Graph,$i,ID,$Sclass) > -1} {
          GraphRemoveCurve EMSegment $EMSegment(Graph,$i,path) $EMSegment(Graph,$i,ID,$Sclass)
       }
       unset EMSegment(Graph,$i,ID,$Sclass)
       EMSegment(Graph,$i,Data,$Sclass) Delete
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
    set ClassIndex [lsearch -exact $EMSegment(GlobalClassList) $Sclass]

    if {$EMSegment(Graph,$NumGraph,ID,$Sclass) > -1} {
       GraphRemoveCurve EMSegment $EMSegment(Graph,$NumGraph,path) $EMSegment(Graph,$NumGraph,ID,$Sclass)
       set EMSegment(Graph,$NumGraph,ID,$Sclass) -1
       # Delete Line and raise button
       if {$Sclass > 0} {
       # Lower Button  
       if {$ClassIndex < $EMSegment(Graph,ButtonNum)} { $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief raised
       } else { $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -relief raised }
       } else {
       if  {$EMSegment(SegmentMode) > 1} {$EMSegment(Cl-mbGraphHistogram$NumGraph) configure -relief raised
       } else {$EMSegment(Cl-bGraphHistogram$NumGraph) configure -relief raised }
       }
    } else {
    # Draw Graph and lower button
      set result 0
      if {$Sclass > 0} {
        # Check Setting
        if {$EMSegment(NumInputChannel) == 0} {
            DevErrorWindow "Before triing to plot a graph please do the following :\n 1.) Define volume(s) to be segmented \n 2.) Define which volume's parameters to plot by selecting one at the yellow graph button"
            return -1
        } 
        if {$NumGraph < 2} {
           if { $EMSegment(Graph,$NumGraph,VolumeID,0) == -1 } {
              DevErrorWindow "Please define which volume's parameters to plot for graph [expr $NumGraph+1] by selecting one at the yellow graph button"
              return -1
           }
           set index [lsearch -exact $EMSegment(SelVolList,VolumeList) $EMSegment(Graph,$NumGraph,VolumeID,0)]
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
      # Draw Curve
      EMSegmentCalcProb
      set flag [expr ($Sclass > 0 ? 0 : 1)]
      if {$Sclass} {
      set EMSegment(Graph,$NumGraph,ID,$Sclass) [GraphAddCurveRegion EMSegment $EMSegment(Graph,$NumGraph,path) EMSegment(Graph,$NumGraph,Data,$Sclass) [GraphHexToRGB [string range $EMSegment(Cattrib,$Sclass,ColorCode) 1 6]] $flag $flag]
      } else {
      set EMSegment(Graph,$NumGraph,ID,$Sclass) [GraphAddCurveRegion EMSegment $EMSegment(Graph,$NumGraph,path) EMSegment(Graph,$NumGraph,Data,$Sclass)Res [GraphHexToRGB [string range $EMSegment(Cattrib,$Sclass,ColorGraphCode) 1 6]] $flag $flag]
      }
 
      if {$Sclass > 0} {
        if {$ClassIndex < $EMSegment(Graph,ButtonNum)} { $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief sunken
        } else { $EMSegment(Cl-fGraphButtonsBelow).bGraphButton$Sclass configure -relief sunken }
      } else {
        if  {$EMSegment(SegmentMode) > 1} {$EMSegment(Cl-mbGraphHistogram$NumGraph) configure -relief raised
        } else {$EMSegment(Cl-bGraphHistogram$NumGraph) configure -relief sunken }
      }
  }
  return 1
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
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateCIMRowsColumns
# Creates the CIM Matrix (with start =1 and end = [llength $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)]
# in the CIM Panel or adds Rows and Columns to it  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateCIMRowsColumns {start {end ""}} {
   global EMSegment Gui
   set f $EMSegment(CIM-fMatrix) 
   set CIMType $EMSegment(CIMType)

   if {$end != ""} { 
     # Define a start and end point instead of a list
     set List ""
     for {set i $start} {$i < $end} {incr i} {
        lappend List $i
     }
   } else {
     set List $start
     set start [lindex $List 0]
     set end [expr [llength $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)] + 1]
   }

   # Add row text
   foreach i $List {
       eval {label $f.fLineL.l$i -text "$EMSegment(Cattrib,$i,Label)" -width 5} $Gui(WLA)
       pack $f.fLineL.l$i -side left -padx 3  -pady 1 
   }
   # Change
   foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) { 
       if {$i < $start} {
          # Only add new Columns
          # EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,<x>,<y>,<z>)
          foreach j $List {
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$j,$i,$CIMType) -width 5} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
          } 
        } else {
          # Add new lines to the Matrix
          frame $f.fLine$i -bg $Gui(activeWorkspace)
          pack $f.fLine$i -side top -padx 0 -pady 0 -fill x

          eval {label $f.fLine$i.lLabel  -text "$EMSegment(Cattrib,$i,Label)" -width 3} $Gui(WLA)
          pack $f.fLine$i.lLabel -side left -padx $Gui(pad) -pady 1 
          # Add new columns to the matrix
          foreach j $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) { 
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$j,$i,$CIMType) -width 5} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
          } 
        }
    }
    EMSegmentSetCIMMatrix
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentSetCIMMatrix
# Defines the size of the canvas $EMSegment(CIM-cMatrix)
# in the CIM Panel or adds Rows and Columns to it  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetCIMMatrix {} {
   global EMSegment
   set dim [llength $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList)]
   if {$dim == 0} {
      set f $EMSegment(CIM-fDefinition)
      $f.fMatrix.cMatrix config -height 0 -scrollregion "0 0 0 1"  
   } else {
     set f $EMSegment(CIM-fMatrix)
     set i [lindex $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) 0]
     set reqheight [winfo reqheight $f.fLine$i.eCol$i]
     set reqwidth  [winfo reqwidth $f.fLine$i.eCol$i]
     set reqlabel  [winfo reqwidth $f.fLine$i.lLabel]
     set f $EMSegment(CIM-fDefinition)
     $f.fMatrix.cMatrix config -height [expr ($reqheight+2)*($dim+1)] -scrollregion "0 0 [expr ($reqwidth+1)*$dim + $reqlabel+20] 1"   
   } 
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

   foreach i $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
       foreach j $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
           $f.fLine$i.eCol$j configure -textvariable EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$j,$i,$CIMType) 
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSuperClassChildren 
# Finds out the all children, grandchildren and ... of a super class
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSuperClassChildren {SuperClass} {
    global EMSegment
    if {$EMSegment(Cattrib,$SuperClass,IsSuperClass) == 0} {
    return     $EMSegment(Cattrib,$SuperClass,Label)
    }
    set result ""
    foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
           # it is defined as SetType<TYPE> <ID>  
       set result "$result [EMSegmentSuperClassChildren $i]" 
    } else {
        lappend result $EMSegment(Cattrib,$i,Label)  
    }
     } 
     return $result
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
    foreach i $EMSegment(GlobalSuperClassList) {
    vtkImageEMMarkov EMCIM    
    # EM Specific Information
    set NumClasses [llength $EMSegment(Cattrib,$i,ClassList)]
        EMCIM SetNumClasses     $NumClasses  
    EMCIM SetStartSlice     $EMSegment(StartSlice)
    EMCIM SetEndSlice       $EMSegment(EndSlice)

        # Kilian : Get rid of those 
    EMCIM SetImgTestNo       -1 
    EMCIM SetImgTestDivision  0 
    EMCIM SetImgTestPixel     0 
        set index 0
        foreach j $EMSegment(Cattrib,$i,ClassList) {
        set LabelList [EMSegmentSuperClassChildren $j]
        EMCIM SetLabelNumber $index [llength $LabelList]
        foreach l $LabelList {
        EMCIM SetLabel $index $l
        }
            incr index
    }

    # Transfer image information
    EMCIM SetInput [Volume($Volume(activeID),vol) GetOutput]
    set data [EMCIM GetOutput]
    # This Command calls the Thread Execute function
    $data Update
    set xindex 0 
    foreach x $EMSegment(Cattrib,$i,ClassList) {
        set EMSegment(Cattrib,$x,Prob) [EMCIM GetProbability $xindex]
        set yindex 0
        # EMCIM traines the matrix (y=t, x=t-1) just the other way EMSegment (y=t-1, x=t) needs it - Sorry !
        foreach y $EMSegment(Cattrib,$i,ClassList) {
        for {set z 0} {$z < 6} {incr z} {
            # Error made in x,y coordinates in EMSegment - I checked everything - it workes in XML and CIM Display in correct order - so do not worry - it is just a little bit strange - but strange things happen
            set temp [$data GetScalarComponentAsFloat $yindex $xindex  $z 0]
            set EMSegment(Cattrib,$i,CIMMatrix,$x,$y,[lindex $EMSegment(CIMList) $z]) [expr round($temp*100000)/100000.0]        
        }
        incr yindex
        }
        incr xindex
    }
    # Delete instance
    EMCIM Delete
    }
    # Jump to edit field 
    EMSegmentExecuteCIM Edit
    # Move Button to Edit
    set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
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
      # Kilian : Has to be changed later if we have different input channels for different classes !
      set List "$EMSegment(GlobalSuperClassList) $EMSegment(GlobalClassList)"
      foreach i $List {
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
    # This is for tab settings-> Dice 
    # 1.) Check if we have any old once in the list
    set MenuIndex 0
    foreach vol $EMSegment(DICEVolumeList) {
    set VolIndex [lsearch -exact $Volume(idList) $vol]
    if {$VolIndex < 0} { 
        $EMSegment(mDICE) delete $MenuIndex $MenuIndex
        set EMSegment(DICEVolumeList) [lreplace $EMSegment(DICEVolumeList) $MenuIndex $MenuIndex]
            if {$vol == $EMSegment(DICESelectedVolume)} { EMSegmentChangeDiceVolume $Volume(idNone)}
    } else { incr MenuIndex}
    }

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
        # This is for Tab EM-> Step 1 
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
      # This is for tab settings-> Dice 
      # Look for new entries 
      if {[lsearch -exact $EMSegment(DICEVolumeList) $v] < 0} {
      $EMSegment(mDICE) add command -label [Volume($v,node) GetName] -command "EMSegmentChangeDiceVolume $v"
      lappend EMSegment(DICEVolumeList) $v
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
    # Kilian : Has to be changed later if we have different input channels for different classes !
    set ClassList "0 $EMSegment(GlobalClassList)"
    if  {$EMSegment(SegmentMode) > 1} {
        $EMSegment(Cl-mGraphHistogram0) delete $index
        if {$EMSegment(NumGraph) > 1} {$EMSegment(Cl-mGraphHistogram1) delete $index}
           for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
           if {$EMSegment(Graph,$i,VolumeID,0) == $v } {
              set EMSegment(Graph,$i,VolumeID,0) -1
          if {$EMSegment(NumGraph) == 3} {
          set EMSegment(Graph,2,VolumeID,$i) -1
          } 
              $EMSegment(Cl-mbGraphHistogram$i) config -text ""
              if {$EMSegment(Graph,$i,ID,0) > -1} {
                EMSegmentDrawDeleteCurveRegion 0 $i
              }
          foreach j $ClassList { 
                if {$EMSegment(Graph,$i,ID,$j) > -1} {
                  EMSegmentMultipleDrawDeleteCurveRegion $j
                }
              }
           }
        }
    } else {
        for {set i 0} { $i < $EMSegment(NumGraph)} {incr i} {
            set EMSegment(Graph,$i,VolumeID,0) -1
        if {$EMSegment(NumGraph) == 2} {
        set EMSegment(Graph,2,VolumeID,$i) -1
        }
            $EMSegment(Cl-bGraphHistogram$i) config -text ""
            if {$EMSegment(Cattrib,$i,ID,0) > -1} {
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
# .PROC EMSegmentCreateGraphDisplayButton
# Creates the graph display button on the Class Tab
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphDisplayButton {f} {
    global Gui
    DevAddButton $f.bGraphWindow "Class Distribution" "" 
    bind $f.bGraphWindow <ButtonPress-1>  " EMSegmentShowGraphWindow %X %Y"
    TooltipAdd $f.bGraphWindow "Press this button to open up a window displaying graphically the class distribution."
    pack $f.bGraphWindow -side top -padx $Gui(pad)  -pady $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentShowGraphWindow
# Creates the graph window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentShowGraphWindow {{x 0} {y 0}} {
    global EMSegment Gui

    # Recreate popup if user killed it
    if {[winfo exists $Gui(wEMSegment)] == 0} {
        EMSegmentCreateGraphWindow
    }

    # Update Classes so they are up to date 
    EMSegmentUpdateClasses 0 
    # LabelsUnHideLabels
    ShowPopup $Gui(wEMSegment) $x $y
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraphWindow
# Creates the graph window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphWindow { } {
    global EMSegment Gui
 
    set w .wEMSegmenterGraph
    set Gui(wEMSegment) $w

    if {[winfo exists $Gui(wEMSegment)]} {
       for {set i 0} {$i < $EMSegment(NumGraph)} {incr i} {
       GraphDelete EMSegment $EMSegment(Graph,$i,path) 
       }
       destroy  $Gui(wEMSegment) 
    }
    #-------------------------------------------
    # Popup Window
    #-------------------------------------------
    toplevel $w -class Dialog -bg $Gui(activeWorkspace)
    wm title $w "Display Class Distribution"
    wm iconname $w Dialog
    wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
    # wm resizable $w  0 0
    if {$Gui(pc) == "0"} {
        wm transient $w .
    }
    wm withdraw $w
    # regexp {([^x]*)x([^\+]*)} [wm geometry $w] match w h
    # Frames
    set f $w
    
    # wm positionfrom $w user

    eval {label $f.lTitle -text "Display Class Distribution" } $Gui(WTA)
    pack $f.lTitle -side top -padx 4 -pady 4

    frame $f.fGraphButtons -bg $Gui(activeWorkspace)
    pack $f.fGraphButtons -side top -padx $Gui(pad) -pady 2 -fill x
    set EMSegment(Cl-fGraphButtons) $f.fGraphButtons
    # Create Button for Histogram
    # First Button is selecting the Volume for the Histogram 
    #Define Menu button
    EMSegmentCreateHistogramButton $f.fGraphButtons 0

    set ValueList " "
    if {$EMSegment(NumGraph) < 2} {
      set GraphList {0}
    } elseif {$EMSegment(NumGraph) < 3} {
      set GraphList {0 1}
    } else {
      set GraphList {0 2 1}
      lappend ValueList " "
    } 

    foreach i $GraphList {
       frame $f.fgraph$i -bg $Gui(activeWorkspace) 
       pack $f.fgraph$i -side top -padx 0 -pady 0 -fill x
       GraphCreate EMSegment $f.fgraph$i $EMSegment(Graph,$i,Xlen) $EMSegment(Graph,$i,Ylen) "" "0 0" 0 $EMSegment(Graph,$i,Dimension) 1 "" $EMSegment(Graph,$i,Xmin) $EMSegment(Graph,$i,Xmax) $EMSegment(Graph,$i,Xsca) "%d" $EMSegment(Graph,$i,Ymin) $EMSegment(Graph,$i,Ymax) $EMSegment(Graph,$i,Ysca) $EMSegment(Graph,$i,Yfor) 1
       GraphChangeBackgroundColor EMSegment $f.fgraph$i 0.0 0.8 0.0
       GraphCreateDisplayValues EMSegment $f.fgraph$i "$ValueList" 0 0
       set EMSegment(Graph,$i,path) $f.fgraph$i
    }

    frame $f.fGraphButtonsBelow -bg $Gui(activeWorkspace)
    pack $f.fGraphButtonsBelow -side top -padx $Gui(pad) -pady 1 -fill x
    set EMSegment(Cl-fGraphButtonsBelow) $f.fGraphButtonsBelow

    if {$EMSegment(NumGraph) > 1} {EMSegmentCreateHistogramButton $f.fGraphButtonsBelow 1}

    foreach i $EMSegment(GlobalClassList) {
       EMSegmentCreateGraphButton $i $EMSegment(Cattrib,$i,Label) $EMSegment(Cattrib,$i,ColorCode)
    }

    frame $f.fLastLine -bg $Gui(activeWorkspace)
    pack $f.fLastLine -side top -padx 2 -pady 2 -fill x

    frame $f.fLastLine.fGraph -bg $Gui(activeWorkspace)
    frame $f.fLastLine.fButtons -bg $Gui(activeWorkspace)

    pack $f.fLastLine.fButtons -side top -padx 0 -pady 2 
    pack $f.fLastLine.fGraph -side top -padx 0 -pady 2 

    eval {button $f.fLastLine.fButtons.bCancel -text "Cancel" -width 8 -command "wm withdraw $w"} $Gui(WBA)
    DevAddLabel $f.fLastLine.fButtons.lEmpty2 "      "
    eval {button $f.fLastLine.fButtons.bUpdate -text "Update" -width 8 -command "EMSegmentUpdateClasses 1"} $Gui(WBA)
    TooltipAdd $f.fLastLine.fButtons.bUpdate "Press this button to update class values and graph."

    pack  $f.fLastLine.fButtons.bCancel $f.fLastLine.fButtons.lEmpty2 $f.fLastLine.fButtons.bUpdate -side left  -side left -padx 0  -pady 0
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateHistogramButton
# Defines the Historgramm button
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateHistogramButton {f index} {
    global Gui EMSegment Volume

    if {$EMSegment(Graph,$index,VolumeID,0) == -1 } {
       set MenuText ""
       set volume [Volume($Volume(idNone),vol) GetOutput]
    } else {
       set MenuText [Volume($EMSegment(Graph,$index,VolumeID,0),node) GetName]
       set volume [Volume($EMSegment(Graph,$index,VolumeID,0),vol) GetOutput]
    }
    # Kilian - have to change this does not work this way anymore 
    GraphCreateHistogramCurve EMSegment(Graph,$index,Data,0) $volume $EMSegment(Graph,$index,Xmin) $EMSegment(Graph,$index,Xmax) $EMSegment(Graph,$index,Xlen)
    set EMSegment(Graph,$index,ID,0) -1

    if  {$EMSegment(SegmentMode) > 1} {
      set EMSegment(Cl-mbGraphHistogram$index) $f.mbHistogram
      set EMSegment(Cl-mGraphHistogram$index) $f.mbHistogram.m
    
      eval {menubutton $EMSegment(Cl-mbGraphHistogram$index) -text $MenuText  -menu $EMSegment(Cl-mGraphHistogram$index) -width 8} $Gui(WBA) 
      $EMSegment(Cl-mbGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorGraphCode)
      $EMSegment(Cl-mbGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorGraphCode)

      pack $EMSegment(Cl-mbGraphHistogram$index) -side left -padx $Gui(pad) 
      TooltipAdd  $EMSegment(Cl-mbGraphHistogram$index) "Press left mouse button to selct volume - press right mouse button to display volume's histogram "
      bind $EMSegment(Cl-mbGraphHistogram$index) <Button-3> "EMSegmentDrawDeleteCurveRegion 0 $index" 

      # Define Menu selection 
      eval {menu $EMSegment(Cl-mGraphHistogram$index)} $Gui(WMA)
      $EMSegment(Cl-mGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorGraphCode)
      $EMSegment(Cl-mGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorGraphCode)
      # Add Selection entry
      foreach v $EMSegment(SelVolList,VolumeList)  {
         set VolName [Volume($v,node) GetName]
         $EMSegment(Cl-mGraphHistogram$index) add command -label $VolName -command "EMSegmentChangeVolumeGraph $v $index"
      }
    } else {
      set EMSegment(Cl-bGraphHistogram$index) $f.bHistogram
      eval {button $EMSegment(Cl-bGraphHistogram$index) -text $MenuText -width 8 -command "EMSegmentDrawDeleteCurveRegion 0 $index" } $Gui(WBA)
      $EMSegment(Cl-bGraphHistogram$index) configure -bg $EMSegment(Cattrib,0,ColorGraphCode)
      $EMSegment(Cl-bGraphHistogram$index) configure -activebackground $EMSegment(Cattrib,0,ColorGraphCode)
      TooltipAdd $EMSegment(Cl-bGraphHistogram$index) "Press button to display histogram of current active volume" 
      pack $EMSegment(Cl-bGraphHistogram$index) -side left -padx $Gui(pad)
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentGraphXAxisUpdate path Xmin Xmax Xsca 
# Called from Graph when X axis is update 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentGraphXAxisUpdate {path Xmin Xmax Xsca} {
    global EMSegment
    set NumGraph 0  
    while { ($NumGraph < $EMSegment(NumGraph)) && ($EMSegment(Graph,$NumGraph,path) != $path)   } {incr NumGraph}
    if {$NumGraph == $EMSegment(NumGraph)} { 
    puts "EMSegmentGraphXAxisUpdate:Error: Could not find graph with path $path" 
    return
    }

    # Could be done nicer but works right now
    set EMSegment(Graph,$NumGraph,Xmin) $Xmin
    set EMSegment(Graph,$NumGraph,Xmax) $Xmax
    set EMSegment(Graph,$NumGraph,Xsca) $Xsca
    if {$NumGraph < 2} {
      # Update Histogram
      set dist [expr $Xmax - $Xmin]
      EMSegment(Graph,$NumGraph,Data,0)Accu SetComponentOrigin $Xmin 0.0 0.0 
      EMSegment(Graph,$NumGraph,Data,0)Accu SetComponentExtent 0 [expr int($dist - 1)] 0 0 0 0
      EMSegment(Graph,$NumGraph,Data,0)Accu UpdateWholeExtent
      EMSegment(Graph,$NumGraph,Data,0)Accu Update
    # If only the scalling changed we do not have to go through all the fuss
    if  {[ expr int($dist * [EMSegment(Graph,$NumGraph,Data,0)Res GetAxisMagnificationFactor 0])]  != $EMSegment(Graph,$path,Xlen)} {
        set XInvUnit $EMSegment(Graph,$path,XInvUnit) 
        EMSegment(Graph,$NumGraph,Data,0)Res  SetAxisMagnificationFactor 0 $XInvUnit
        EMSegment(Graph,$NumGraph,Data,0)Res  Update

        set extent [[EMSegment(Graph,$NumGraph,Data,0)Res GetOutput] GetExtent]
        while {[expr [lindex $extent 1] - [lindex $extent 0] + 1] <  $EMSegment(Graph,$path,Xlen) } {
        set XInvUnit [expr $XInvUnit * 1.001]
        EMSegment(Graph,$NumGraph,Data,0)Res SetAxisMagnificationFactor 0 $XInvUnit
        EMSegment(Graph,$NumGraph,Data,0)Res Update 
        set extent [[EMSegment(Graph,$NumGraph,Data,0)Res GetOutput] GetExtent]
        }
    } else {
        EMSegment(Graph,$NumGraph,Data,0)Res  Update
    }
    }
    # Update Classes
    foreach i $EMSegment(GlobalClassList) {
    EMSegment(Graph,$NumGraph,Data,$i) SetXmin $Xmin
    EMSegment(Graph,$NumGraph,Data,$i) SetXmax $Xmax
    EMSegment(Graph,$NumGraph,Data,$i) Update
    }
    
    if {$EMSegment(NumGraph) > 2} {
    if {$NumGraph == 2} {
        if {($EMSegment(Graph,0,Xmin) != $EMSegment(Graph,2,Xmin)) ||  ($EMSegment(Graph,0,Xmax) != $EMSegment(Graph,2,Xmax)) ||  ($EMSegment(Graph,0,Xsca) != $EMSegment(Graph,2,Xsca))} {
        GraphRescaleAxis EMSegment $EMSegment(Graph,0,path) $Xmin $Xmax $Xsca 0
        }
    } else { 
 if {($EMSegment(Graph,$NumGraph,Xmin) != $EMSegment(Graph,2,Xmin)) ||  ($EMSegment(Graph,$NumGraph,Xmax) != $EMSegment(Graph,2,Xmax)) ||  ($EMSegment(Graph,$NumGraph,Xsca) != $EMSegment(Graph,2,Xsca))} {
        GraphRescaleAxis EMSegment $EMSegment(Graph,2,path) $Xmin $Xmax $Xsca $NumGraph
          }
        }
    }
}



#-------------------------------------------------------------------------------
# .PROC EMSegmentGraphYAxisUpdate path Ymin Ymax Ysca 
# Called from Graph when Y axis is update 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentGraphYAxisUpdate {path Ymin Ymax Ysca} {
    global EMSegment
    set NumGraph 0  
    while { ($NumGraph < $EMSegment(NumGraph)) && ($EMSegment(Graph,$NumGraph,path) != $path)   } {incr NumGraph}
    if {$NumGraph == $EMSegment(NumGraph)} { 
    puts "EMSegmentGraphYAxisUpdate:Error: Could not find graph with path $path" 
    return
    }
    if {$EMSegment(Graph,$NumGraph,Dimension) != 2 } {return }

    # Could be done nicer but works right now
    set EMSegment(Graph,$NumGraph,Ymin) $Ymin
    set EMSegment(Graph,$NumGraph,Ymax) $Ymax
    set EMSegment(Graph,$NumGraph,Ysca) $Ysca
    
    # Update Classes
    foreach i $EMSegment(GlobalClassList) {
    EMSegment(Graph,$NumGraph,Data,$i) SetYmin $Ymin
    EMSegment(Graph,$NumGraph,Data,$i) SetYmax $Ymax
    EMSegment(Graph,$NumGraph,Data,$i) Update
    }
    if {($EMSegment(Graph,1,Xmin) != $EMSegment(Graph,2,Ymin)) ||  ($EMSegment(Graph,1,Xmax) != $EMSegment(Graph,1,Ymax)) ||  ($EMSegment(Graph,1,Xsca) != $EMSegment(Graph,2,Ysca))} {
        GraphRescaleAxis EMSegment $EMSegment(Graph,1,path) $Ymin $Ymax $Ysca 0
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateDisplayRedLine 
# Creates or displays a red line on the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateDisplayRedLine {NumGraph Value} {
    global EMSegment
    set path $EMSegment(Graph,$NumGraph,path)
    if {$EMSegment(Graph,$NumGraph,LineID,0) < 0} {
    set EMSegment(Graph,$NumGraph,LineID,0) [GraphCreateLine EMSegment $path "1 0 0" 0]
    GraphCreateDisplayValues EMSegment $path $Value 0 1
    } else {
    GraphCreateDisplayValues EMSegment $path $Value 1 1
    }
    set Xcoord [GraphTransformValueInCoordinate  EMSegment $path $Value 0]
    GraphSetLineCoordinates EMSegment $path $EMSegment(Graph,$NumGraph,LineID,0) "$Xcoord 0 $Xcoord $EMSegment(Graph,$NumGraph,Ylen)"
    GraphRender EMSegment $path
    # Value should be displayed
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateDisplayRedCross 
# Creates or displays a cross line on the graph 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateDisplayRedCross {NumGraph Xvalue Yvalue} {
    global EMSegment
    set path $EMSegment(Graph,$NumGraph,path)
    if {$EMSegment(Graph,$NumGraph,LineID,0) < 0} {
    set EMSegment(Graph,$NumGraph,LineID,0) [GraphCreateLine EMSegment $path "1 0 0" 0]
    set EMSegment(Graph,$NumGraph,LineID,1) [GraphCreateLine EMSegment $path "1 0 0" 0]
    GraphCreateDisplayValues EMSegment $path "$Xvalue $Yvalue" 0 1
    } else {
    GraphCreateDisplayValues EMSegment $path "$Xvalue $Yvalue" 1 1
    }

    set Xcoord [GraphTransformValueInCoordinate EMSegment $path $Xvalue 0]
    set Ycoord [GraphTransformValueInCoordinate EMSegment $path $Yvalue 1]
    GraphSetLineCoordinates EMSegment $path $EMSegment(Graph,$NumGraph,LineID,0) "$Xcoord [expr $Ycoord-2] $Xcoord [expr $Ycoord+3]"
    GraphSetLineCoordinates EMSegment $path $EMSegment(Graph,$NumGraph,LineID,1) "[expr $Xcoord -2] $Ycoord [expr $Xcoord+3] $Ycoord"
    GraphRender EMSegment $path
    # Value should be displayed
    # GraphCreateDisplayValues EMSegment $EMSegment(Graph,0,path) $ValueList 1 1
}



#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeDiceVolume
# Just changes the selected volume for the DICE measure 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeDiceVolume {vol} {
  global EMSegment
  set EMSegment(DICESelectedVolume) $vol 
  $EMSegment(mbDICE) configure -text  [Volume($vol,node) GetName]
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalcDice
# Calculates dice volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalcDice { } {  
   global EMSegment Volume
   vtkImageEMGeneral EMDice
   if {($EMSegment(DICESelectedVolume) !=  $Volume(idNone)) && ($EMSegment(DICELabeledVolume) != $Volume(idNone))} {
       foreach label $EMSegment(DICELabelList) {
       puts "======================= Result for Label $label ===================="
       EMDice CalcSimularityMeasure [Volume($EMSegment(DICELabeledVolume),vol) GetOutput] [Volume($EMSegment(DICESelectedVolume),vol) GetOutput] $label 1
       }
   }
   EMDice Delete
}





# ------------------------------------------------------------------------------
# TextBox Sample Procedures - not used anymore

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
