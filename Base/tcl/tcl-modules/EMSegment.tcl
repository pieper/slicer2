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
# FILE:        EMSegment.tcl
# PROCEDURES:  
#   EMSegmentInit
#   EMSegmentBuildGUI
#   EMSegmentEnter
#   EMSegmentExit
#   EMSegmentUpdateGUI
#   EMSegmentShowFile
#   EMSegmentBindingCallback
#   EMSegmentStartEM
#   EMSegmentStartEM
#   EMSegmentClickLabel
#   EMSegmentSegClassDefinition
#   EMSegmentDisplayClassDefinition
#   EMSegmentUseSamples
#   EMSegmentChangeClass
#   EMSegmentChangeClass
#   EMSegmentUpdateSettingButton 
#   EMSegmentUpdateClassButton 
#   EMSegmentCalculateClassMeanSigm 
#   EMSegmentCalculateClassMeanSigma
#   EMSegmentCalcProb  
#   EMSegmentCalcProb
#   EMSegmentWriteTextBox  
#   EMSegmentReadTextBox  
#   EMSegmentScrolledTextBox  
#   EMSegmentCreateDeleteClasses  
#   EMSegmentReadGreyValue
#   EMSegmentCreateGraphButton
#   EMSegmentDeleteGraphButton
#   EMSegmentDrawDeleteCurve
#   EMSegmentCalculateClassCurve 
#   EMSegmentDrawClassCurve
#   EMSegmentDrawSingleCurve
#   EMSegmentCalulateGraphXleUXscnXscpXpos 
#   EMSegmentCalulateGraphXleUXscnXscp 
#   EMSegmentCreateXLabels 
#   EMSegmentRedrawGraph 
#   EMSegmentRescaleGraph
#   EMSegmentExecuteCIM 
#   EMSegmentCreateCIMRowsColumns
#   EMSegmentChangeCIMMatrix
#   EMSegmentTrainCIMField
#   EMSegmentReadCIMFile 
#   EMSegmentSaveCIMFile 
#   EMSegmentDisplayHorizontal 
#   EMSegmentEndSlice 
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
    global EMSegment Module Volume Model Mrml Color
    
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
    set Module($m,procGUI) EMSegmentBuildGUI
    set Module($m,procEnter) EMSegmentEnter
    set Module($m,procExit) EMSegmentExit

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
	    {$Revision: 1.1 $} {$Date: 2001/12/06 22:19:54 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #

    set EMSegment(Volume1) $Volume(idNone)
    set EMSegment(Model1)  $Model(idNone)
    set EMSegment(FileCIM) ""


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

    # Kilian : Set for debugging
    set EMSegment(NumClasses)   4
    set EMSegment(NumClassesNew) -1 
    # set EMSegment(EMiteration)  [vtkEMInit GetNumIter]
    # set EMSegment(EMiteration)  3
    set EMSegment(EMiteration)  3
    # set EMSegment(MFAiteration) [vtkEMInit GetNumRegIter]
    set EMSegment(MFAiteration) 1
    set EMSegment(Alpha)        [vtkEMInit GetAlpha]
    set EMSegment(SmWidth)      [vtkEMInit GetSmoothingWidth]
    set EMSegment(SmSigma)      [vtkEMInit GetSmoothingSigma]
    # Debugging set EMSegment(StartSlice)   [vtkEMInit GetStartSlice]
    set EMSegment(StartSlice)   1
    # set Volume(lastNum)  [MainFileFindImageNumber Last [file join $Mrml(dir) $Volume(firstFile)]]
    # set EMSegment(EndSlice)     [vtkEMInit GetEndSlice]
    set EMSegment(EndSlice)     1
    # set EMSegment(ImgTestNo)        [vtkEMInit GetImgTestNo]
    set EMSegment(ImgTestNo)        -1
    # set EMSegment(ImgTestDivision)  [vtkEMInit GetImgTestDivision]
    set EMSegment(ImgTestDivision)  $EMSegment(NumClasses)
    # set EMSegment(ImgTestPixel)     [vtkEMInit GetImgTestPixel]
    set EMSegment(ImgTestPixel)     2
    set EMSegment(PrintIntermediateResults)   [vtkEMInit GetPrintIntermediateResults] 

    vtkEMInit Delete

    # Should the Forgorund ( = 0 ) or Background (= 1) be read for the sample inputs
    set EMSegment(ReadBackPickel) 1 

    # Class 1 is default class
    set EMSegment(Class) 1 

    # Should Samples be used for calculating Mean and Variance or not (not = 0 / yes = 1)
    set EMSegment(UseSamples) 1

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
    #	lappend EMSegment(ColorLabelList) [MakeColorNormalized [Color($i,node) GetDiffuseColor]]
    #	lappend EMSegment(ColorLabelList) [lindex [Color(1,node) GetLabels] 0]
    # It is always <color> <corresponding label> 
    set EMSegment(ColorLabelList) [list #ffccb2 2 #ffffff 3 #66b2ff 4 #e68080 5 #80e680 6 #80e6e6 7 #e6e680 8 #e6b2e6 9 #e6e680 10]

    # EMSegment(MenuColor,$Color) :
    # -2  = Has not been displayed in list or assigned in class
    # -1  = Has been assigned to class
    # >-1 = Index of Color in list 
    # foreach Color $EMSegment(ColorList) {
    #	set EMSegment(MenuColor,$Color) -2
    # }

    EMSegmentCreateDeleteClasses 

    # Define for Class 0 for Histogram
    set EMSegment(Cattrib,0,Graph) 0
    set EMSegment(Cattrib,0,ColorCode) #ffff00
    set EMSegment(Graph,0,VolumeID) -1 
    set EMSegment(Graph,0,XMin)     -1
    set EMSegment(Graph,0,XMax)     -1
    set EMSegment(Graph,0,YMax)     -1    
  
    # Define Graph for "Class" panel
    # Define length of Canvas system
    set EMSegment(Graph,Cxle) 200
    set EMSegment(Graph,Cyle) 150 
    # Define Border between canvas and coord system on the rigth and left /up and down
    set EMSegment(Graph,XboL) 25
    set EMSegment(Graph,XboR) 5
    set EMSegment(Graph,YboU) 5
    set EMSegment(Graph,YboD) 12
    # Define Number Scalling in value difference
    set EMSegment(Graph,Xsca) 50
    set EMSegment(Graph,XscaNew) $EMSegment(Graph,Xsca)
    set EMSegment(Graph,Ysca) 0.20
    # Min - Max values
    set EMSegment(Graph,Xmin) 10
    set EMSegment(Graph,XminNew) $EMSegment(Graph,Xmin) 
    set EMSegment(Graph,Xmax) 255
    set EMSegment(Graph,XmaxNew) $EMSegment(Graph,Xmax)

    set EMSegment(Graph,Ymax) 1.0
    # Length / 2 of scalling lines 
    set EMSegment(Graph,Xscl) 2
    set EMSegment(Graph,Yscl) 2
    # Difference bettween Text and Scalling line on Axis 
    set EMSegment(Graph,Xsct) 5
    set EMSegment(Graph,Ysct) 12

    # Should Probablilites be displayed in graph or not
    set EMSegment(Graph,DisplayProb) 0
    set EMSegment(Graph,DisplayProbNew) $EMSegment(Graph,DisplayProb)

    # Define coord system len in pixels   
    set EMSegment(Graph,Xlen) [expr $EMSegment(Graph,Cxle) - $EMSegment(Graph,XboL) - $EMSegment(Graph,XboR)]
    set EMSegment(Graph,Ylen) [expr $EMSegment(Graph,Cyle) - $EMSegment(Graph,YboU) - $EMSegment(Graph,YboD)]

    # calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn), EMSegment(Graph,Xscp) and EMSegment(Graph,Xpos)
    EMSegmentCalulateGraphXleUXscnXscpXpos
    # calulate EMSegment(Graph,Yscn) and EMSegment(Graph,Yscp) 
    EMSegmentCalulateGraphYscnYscp


    # Event bindings! (see EMSegmentEnter, EMSegmentExit, tcl-shared/Events.tcl)
    set EMSegment(eventManager)  { \
	    {all <Control-Button-1> {EMSegmentBindingCallback Shift-3 %W %X %Y %x %y %t}}}
  #	    all <Control-Button-1> EMSegmentBindingCallback Shift-1 %W %X %Y %x %y %t

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
    set help "
    The EMSegment module is a realization of the EM-MRF EMSegment algortihm defined in
    Tina Kapur's PhD Thesis. Her thesis can be found on the web : 
    http://www.ai.mit.edu/people/tkapur/publications.html
    Kilian Pohl 15-Oct-2001 
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>MRF-EM:</B> This tab is a user interface to run the algorithm.
    <LI><B>Advenced:</B> This tab is for advenced user to change the default
    attributes defined for the algorith.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags EMSegment $help
    MainHelpBuildGUI EMSegment
    
    #-------------------------------------------
    # EM frame
    #-------------------------------------------
    set fEM $Module(EMSegment,fEM)
    set f $fEM
    
    for {set i 1} {$i < 4} {incr i} {
	frame $f.fStep$i -bg $Gui(activeWorkspace)
	pack $f.fStep$i -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # EM->Step1 frame: Load CIM Field
    #-------------------------------------------
    set f $fEM.fStep1
    
    DevAddLabel $f.lHead "Step 1: Load Class Interaction Matrix"
    pack $f.lHead -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    # file browse box
    DevAddFileBrowse $f EMSegment FileCIM "File" "" "mrf"  "." "" "Load File with Class Interation Matrix"

    #-------------------------------------------
    # EM->Step2 frame : Select and Define Classes
    #-------------------------------------------
    set f $fEM.fStep2

    # Make a box around class 2
    $f config -relief groove -bd 3
 
    DevAddLabel $f.lHead1 "Step 2: Select Class"
    frame $f.fSelect -bg $Gui(activeWorkspace)
    pack $f.lHead1 $f.fSelect -side top -padx $Gui(pad) -pady $Gui(pad)

    # Classes Box 
    # General Information on how to create menubtton
    # Gui = defines general setting for buttons, menues, ..., eg. font size ...
    # Through the eval command a list is created with {..} $Gui(..) 
    # and afterwards executed
    
    set menu $f.fSelect.mbClasses.m 
    set Sclass $EMSegment(Class) 

    DevAddLabel $f.fSelect.lText "Class:"

    #Define Menu button
    eval {menubutton $f.fSelect.mbClasses -text $Sclass -menu $menu -width 10} $Gui(WMBA)

    $f.fSelect.mbClasses configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    
    pack $f.fSelect.lText $f.fSelect.mbClasses -side left -padx $Gui(pad) -pady 0     
    set EMSegment(EM-mbClasses) $f.fSelect.mbClasses
 
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    
    # Add Selection entry
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i 1} {
        # change Menu Button when selected
	$menu add command -label "$i" -command "EMSegmentChangeClass $i" \
		-background $EMSegment(Cattrib,$i,ColorCode) -activebackground $EMSegment(Cattrib,$i,ColorCode)
    } 

    DevAddLabel $f.lHead2 "Step 3: Take Samples:"
    frame $f.fPicture -bg $Gui(activeWorkspace)
    frame $f.fDefine -bg $Gui(activeWorkspace)

    pack $f.lHead2 $f.fPicture $f.fDefine -side top -padx $Gui(pad) -pady 4

    DevAddLabel $f.fPicture.lPicture "from:"

    if {$EMSegment(ReadBackPickel) == 1} { set menutext "Background"
    } else {  set menutext "Foreground"}
 
    #Define Menu button
    set menu $f.fPicture.mbPicture.m 
    eval {menubutton $f.fPicture.mbPicture \
	   -text $menutext -menu $menu -width 10 -relief raised -bd 2} $Gui(WMBA)
    
    pack $f.fPicture.lPicture $f.fPicture.mbPicture -side left -padx $Gui(pad) -pady 0     
    set EMSegment(EM-mbPicture) $f.fPicture.mbPicture
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    
    # Add Selection entry
    $menu add command -label "Background" \
        -command  {set EMSegment(ReadBackPickel) 1;  $EMSegment(EM-mbPicture) config -text "Background"}
    $menu add command -label "Foreground" \
        -command  {set EMSegment(ReadBackPickel) 0;  $EMSegment(EM-mbPicture) config -text "Foreground"}


    DevAddLabel $f.fDefine.lSamptxt "Samples taken:"
    DevAddLabel $f.fDefine.lSampvar [llength $EMSegment(Cattrib,$Sclass,Sample)]
    set EMSegment(EM-lSampvar) $f.fDefine.lSampvar
     
    DevAddLabel $f.fDefine.lMeantxt "Class Mean:"
    DevAddLabel $f.fDefine.lMeanvar  [expr int($EMSegment(Cattrib,$Sclass,Mean)*100)/100.0]  
    set EMSegment(EM-lMeanvar) $f.fDefine.lMeanvar
    pack  $f.fDefine.lSamptxt $f.fDefine.lSampvar $f.fDefine.lMeantxt $f.fDefine.lMeanvar -side left -padx $Gui(pad) -pady 2    

    DevAddLabel $f.lexpltxt "Sample by using Ctrl-Shift-Click on picture"
    pack $f.lexpltxt -side top -padx $Gui(pad) -pady 4

    #-------------------------------------------
    # EM->Step4 Frame: Run Algorithm
    #-------------------------------------------
    set f $fEM.fStep3
        
    # Define Button to start algorithm
    DevAddButton $f.bStart Segement EMSegmentStartEM 
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bStart "Press this button to start EM-MRF Segementation."
    pack $f.bStart -side top -padx $Gui(pad) -pady $Gui(pad)

    #--------------------------------------------
    # Class Frame
    #-------------------------------------------
 
    set fCl $Module(EMSegment,fClass)
    set f $fCl
    
    for {set i 1} {$i < 5} {incr i} {
	frame $f.fSec$i -bg $Gui(activeWorkspace)
	pack $f.fSec$i -side top -padx 0 -pady 1 -fill x
    }

    #--------------------------------------------
    # Class->Section 1 Frame: Sample Display Class values
    #-------------------------------------------

    set f $fCl.fSec1

    #Define Menu button
    set menu $f.mbClasses.m 
    eval {menubutton $f.mbClasses -text "Class $Sclass" -menu $menu -width 10} $Gui(WMBA)
    
    $f.mbClasses configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)

    pack $f.mbClasses -side top -padx $Gui(pad) -pady 2     
    set EMSegment(Cl-mbClasses) $f.mbClasses
 
    # Define Menu selection 
    eval {menu $menu} $Gui(WMA)
    
    # Add Selection entry
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i 1} {
        # change Menu Button when selected
	$menu add command -label "Class $i" -command  "EMSegmentChangeClass $i" \
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
    frame $f.body -bg $Gui(activeWorkspace)
    pack $f.body -side top -padx $Gui(pad) -pady 1

    #--------------------------------------------
    # Class->Section 2->body->right Frame
    #-------------------------------------------
    frame $f.body.right -bg $Gui(activeWorkspace)
    pack $f.body.right -side right -fill x -padx $Gui(pad) -pady 1
    set fr $f.body.right

    #frame $fr.f0  -bg $Gui(activeWorkspace)
    #pack $fr.f0 -side top -padx 0 -pady 0 -anchor e 

    for {set i 1} {$i < 6} {incr i} {
	frame $fr.f$i  -bg $Gui(activeWorkspace)
	pack $fr.f$i -side top -padx 0 -pady 1 -anchor e 
    }


    #DevAddLabel $fr.f1.lSamptxt "Samples:"
    #DevAddLabel $fr.f1.lSampvar [llength $EMSegment(Cattrib,$Sclass,Sample)]
    #set EMSegment(Cl-lSampvar) $fr.f1.lSampvar
     
    DevAddLabel $fr.f2.lMeantxt "Mean:"
    eval {entry  $fr.f2.eMeanvar -textvariable EMSegment(Cattrib,$Sclass,Mean) -width 7} $Gui(WEA) 
    set EMSegment(Cl-eMeanvar) $fr.f2.eMeanvar

    DevAddLabel $fr.f3.lSigmatxt "Variation:"
    eval {entry  $fr.f3.eSigmavar -textvariable EMSegment(Cattrib,$Sclass,Sigma) -width 7} $Gui(WEA)
    set EMSegment(Cl-eSigmavar) $fr.f3.eSigmavar
   
    DevAddLabel $fr.f4.lProb "Probability:"
    eval {entry $fr.f4.eProb -width 7 -textvariable EMSegment(Cattrib,$Sclass,Prob) } $Gui(WEA)
    set EMSegment(Cl-eProb) $fr.f4.eProb

    #Define Color
    DevAddLabel $fr.f5.lColorLabel "Color/Label:"

    eval {button $fr.f5.bColorLabel -text "$EMSegment(Cattrib,$Sclass,Label)" -command "ShowLabels EMSegmentClickLabel" -width 7} $Gui(WBA)
    $fr.f5.bColorLabel configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    TooltipAdd $fr.f5.bColorLabel "Choose label value for class."
    set  EMSegment(Cl-bColorLabel) $fr.f5.bColorLabel

    pack $fr.f2.lMeantxt $fr.f2.eMeanvar $fr.f3.lSigmatxt $fr.f3.eSigmavar $fr.f4.lProb $fr.f4.eProb $fr.f5.lColorLabel $fr.f5.bColorLabel -side top -padx $Gui(pad) -anchor e  
 
    #-------------------------------------------
    # Class->Section 2->left Frame: Sample Box
    #-------------------------------------------
    frame $f.body.left -bg $Gui(activeWorkspace)
    pack $f.body.left -side left -padx $Gui(pad) 
    set fl $f.body.left

    eval {button $fl.bSample -text "Use Samples" -width 13 -command "EMSegmentUseSamples 1"} $Gui(WBA)
    set EMSegment(Cl-bSample) $fl.bSample
    pack  $fl.bSample -side top -padx $Gui(pad) -pady 1

    DevAddLabel $fl.lCoord "    X      Y     Brig"
    pack  $fl.lCoord -side top -padx $Gui(pad) -anchor w
    # EMSegment(textBox) links to path $f.tText.text 
    # where the text is actually displayed

    set EMSegment(Cl-textBox) [EMSegmentScrolledText $fl.tText]
    pack $fl.tText -side top  -padx $Gui(pad) -expand true
    # Write values to Box
    EMSegmentWriteTextBox

    # Depending on the Situation we want to en- or disable certain fields
    EMSegmentUseSamples 1

    #-------------------------------------------
    # Class->Section 3: Display Graph   
    #-------------------------------------------
    set f $fCl.fSec3
    $f config -relief groove -bd 3    

    # Button Frame 
    frame $f.fGraphButtons -bg $Gui(activeWorkspace)
    pack $f.fGraphButtons -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    set EMSegment(Cl-fGraphButtons) $f.fGraphButtons

    # Create Button for Histogram
    for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
	EMSegmentCreateGraphButton $i $EMSegment(Cattrib,$i,ColorCode)
    }

    canvas $f.caGraph -width $EMSegment(Graph,Cxle) -height $EMSegment(Graph,Cyle)      
    set ca $f.caGraph
    set EMSegment(Cl-caGraph) $ca
    pack $ca -side top  -padx $Gui(pad)  -pady $Gui(pad)

    # Coordinate Axis
    $ca create line $EMSegment(Graph,XboL) [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU)] \
	                                      [expr $EMSegment(Graph,Xlen) + $EMSegment(Graph,XboL)] \
					      [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU)] \
					      -width 1 -tag Grayvalue 
    $ca create line $EMSegment(Graph,XboL) [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU)] \
	    $EMSegment(Graph,XboL) $EMSegment(Graph,YboU) -width 1 -tag Probaility 

    # Scalers of X Coordinate Axis
    EMSegmentCreateXLabels

    # Scalers of Y Coordinate Axis
    for {set i 0} { $i <= $EMSegment(Graph,Yscn)} {incr i} {
	$ca create line [expr $EMSegment(Graph,XboL) - $EMSegment(Graph,Yscl)] \
		[expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU) - int($i * $EMSegment(Graph,Yscp) ) ] \
		[expr $EMSegment(Graph,XboL) + $EMSegment(Graph,Yscl)] \
		[expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU) - int($i * $EMSegment(Graph,Yscp) ) ] \
		-width 1 -tag ProbL$i
	$ca create text [expr $EMSegment(Graph,XboL) - $EMSegment(Graph,Yscl) - $EMSegment(Graph,Ysct)] \
		[expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU) - int($i*$EMSegment(Graph,Yscp))] \
		-text [expr $i*$EMSegment(Graph,Ysca)] -tag ProbT$i -font {Helvetica 6} -fill $Gui(textDark)  
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
    set CIMMenu {Load Train Edit Save}

    #-------------------------------------------
    # CIM->Sec1 Frame: Menu Selection      
    #-------------------------------------------
    TabbedFrame EMSegment $f "" $CIMMenu $CIMMenu \
	    {"Load File defining Class Interaction Matrix" \
	     "Define a new Class Interaction Matrix by training at already images"\
	     "Edit Matrix defining Class Interaction"\
             "Save Class Interaction Matrix to a file"}\
	    0 Edit

    foreach i $CIMMenu {
	$f.fTop.fTabbedFrameButtons.f.r$i configure -command "EMSegmentExecuteCIM $i"
    }
    set EMSegment(Ma-tabCIM) $f

    set f $f.fTabbedFrame

    set EMSegment(Ma-fCIM) $f

    #-------------------------------------------
    # CIM->Sec2 Frame: Load CIM Random Field 
    #-------------------------------------------
    set f $EMSegment(Ma-fCIM).fLoad

    DevAddLabel $f.lLoadText "Selecting File defining Class Interaction Matrix"
    pack $f.lLoadText -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    set EMSegment(Ma-lLoadText) $f.lLoadText

    #-------------------------------------------
    # CIM->Sec3 Frame: Train CIM Random Field 
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
    # CIM->Sec4 Frame: Edit CIM Random Field 
    #-------------------------------------------
    set f $EMSegment(Ma-fCIM).fEdit

    # frame $f.fSec4  -bg $Gui(backdrop) -relief sunken -bd 2
    # pack $f.fSec4 -side top -padx 0 -pady $Gui(pad) -fill x

    frame $f.fNeighbour -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fDefinition -bg $Gui(activeWorkspace)
    pack $f.fNeighbour $f.fDefinition -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    $f.fDefinition config -relief groove -bd 3   
 
    #-------------------------------------------
    # CIM->Sec4->Neighbour Frame: 
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
			-variable EMSegment(CIMType) -value $p -width 7 \
			-indicatoron 0} $Gui(WCA)
		pack $f.fNeighbour.fSelection.f$row.r$p -side left -pady 0
            if { $p == "Up" } {incr row};
	}

    pack $f.fNeighbour.lText $f.fNeighbour.fSelection -side left -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w

    set EMSegment(CIMType) [lindex $EMSegment(CIMList) 0]

    #-------------------------------------------
    # CIM->Sec4->Definition Frame: Define CIM Field
    #-------------------------------------------
    set f $f.fDefinition
    $f config -relief groove -bd 3    
 
    DevAddLabel $f.lHead "Class Interaction Matrix"
    pack  $f.lHead -side top -padx $Gui(pad) -pady $Gui(pad)
    frame $f.fMatrix -bg $Gui(activeWorkspace)
    pack $f.fMatrix -side left -padx 0 -pady $Gui(pad)

    set EMSegment(CIM-fMatrix) $f.fMatrix

    EMSegmentCreateCIMRowsColumns 1 $EMSegment(NumClasses)
    
    #-------------------------------------------
    # CIM->Sec5 Frame: Save CIM Random Field 
    #-------------------------------------------
    set f $EMSegment(Ma-fCIM).fSave

    DevAddLabel $f.lSaveText "Saving Class Interaction Matrix to a file"
    pack $f.lSaveText -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    set EMSegment(Ma-lSaveText) $f.lSaveText

    #-------------------------------------------
    # Setting frame: Define Algorithem Paramter
    #-------------------------------------------
    set fSe $Module(EMSegment,fSetting)
    set f $fSe

    for {set i 1} {$i < 3} {incr i} {
	frame $f.fSect$i -bg $Gui(activeWorkspace)
	pack $f.fSect$i -side top -padx 0 -pady $Gui(pad) -fill x
    }

    for {set i 1} {$i < 3} {incr i} {
	frame $f.fSect1.fCol$i -bg $Gui(activeWorkspace)
	pack $f.fSect1.fCol$i -side left -padx 0 -pady $Gui(pad) 
    }
    
    DevAddLabel $f.fSect1.fCol1.lNumClasses "Classes:"  
    eval {entry $f.fSect1.fCol2.eNumClasses -width 4 -textvariable EMSegment(NumClassesNew) } $Gui(WEA)


    TooltipAdd $f.fSect1.fCol2.eNumClasses "Number of Classes - activate change by pressing \<return\> or \<tab\>"
    bind $f.fSect1.fCol2.eNumClasses <Return> EMSegmentCreateDeleteClasses
    bind $f.fSect1.fCol2.eNumClasses <Tab> EMSegmentCreateDeleteClasses

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
    
    DevAddLabel $f.fSect1.fCol1.lXMin "X-Minium:"
    eval {entry $f.fSect1.fCol2.eXMin -width 4 -textvariable EMSegment(Graph,XminNew) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXMin "Minimum X value represented in Graph" 

    DevAddLabel $f.fSect1.fCol1.lXMax "X-Maximum:"
    eval {entry $f.fSect1.fCol2.eXMax -width 4 -textvariable EMSegment(Graph,XmaxNew) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXMax "Maximum X value represented in Graph" 

    DevAddLabel $f.fSect1.fCol1.lXSca "X-Scalling:"
    eval {entry $f.fSect1.fCol2.eXSca -width 4 -textvariable EMSegment(Graph,XscaNew) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eXSca "Value difference between two tabs on the X-axis" 


    DevAddLabel $f.fSect1.fCol1.lUseProb "Use Probability:"
    frame  $f.fSect1.fCol2.fUseProb -bg $Gui(activeWorkspace)
    foreach value "1 0" text "On Off" width "4 4" {
	eval {radiobutton $f.fSect1.fCol2.fUseProb.r$value -width $width -indicatoron 0\
		-text "$text" -value "$value" -variable EMSegment(Graph,DisplayProbNew) } $Gui(WCA)
    }
    TooltipAdd $f.fSect1.fCol2.fUseProb "Use class propability to caluclate curves in graph"
    # eval {checkbutton $f.fSect1.fCol2.cUseProb -variable EMSegment(Graph,DisplayProbNew) -bg $Gui(activeWorkspace) -activebackground $Gui(activeWorkspace) -highlightthickness 0}


    DevAddLabel $f.fSect1.fCol1.lTNo "Image Test No:"
    eval {entry $f.fSect1.fCol2.eTNo -width 4 -textvariable EMSegment(ImgTestNo) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eTNo "Use Test picture for evaluation algorithms (-1 => No Test Picture)"

    DevAddLabel $f.fSect1.fCol1.lTDiv "Image Test Division"
    eval {entry $f.fSect1.fCol2.eTDiv -width 4 -textvariable EMSegment(ImgTestDivision) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eTDiv "Number of divison classes in the test image" 

    DevAddLabel $f.fSect1.fCol1.lTPix "Image Test Pixel:"
    eval {entry $f.fSect1.fCol2.eTPix -width 4 -textvariable EMSegment(ImgTestPixel) } $Gui(WEA)
    TooltipAdd $f.fSect1.fCol2.eTPix "Pixel width/length of divison (-1 = max length/width for division)"

    DevAddButton $f.fSect2.bUpdate Update EMSegmentUpdateSettingButton
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.fSect2.bUpdate "Press this button to activate setting changes."
    pack $f.fSect2.bUpdate -side bottom -padx $Gui(pad)  -pady 1


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
    pack $f.fSect1.fCol1.lSmWidth $f.fSect1.fCol1.lSmSigma $f.fSect1.fCol1.lCreateFile -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eSmWidth $f.fSect1.fCol2.eSmSigma $f.fSect1.fCol2.fCreateFile -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty4 $f.fSect1.fCol2.lEmpty4 -side top -padx $Gui(pad) -pady 1 -anchor w 

    pack $f.fSect1.fCol2.fCreateFile.r1  $f.fSect1.fCol2.fCreateFile.r0 -side left -fill x

    #Pack 5.Block
    pack $f.fSect1.fCol1.lXMin $f.fSect1.fCol1.lXMax $f.fSect1.fCol1.lXSca $f.fSect1.fCol1.lUseProb  -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eXMin $f.fSect1.fCol2.eXMax $f.fSect1.fCol2.eXSca $f.fSect1.fCol2.fUseProb -side top -anchor w
    pack $f.fSect1.fCol1.lEmpty5 $f.fSect1.fCol2.lEmpty5 -side top -padx $Gui(pad) -pady 1 -anchor w 
   
    pack $f.fSect1.fCol2.fUseProb.r1  $f.fSect1.fCol2.fUseProb.r0 -side left -fill x

    #Pack 6.Block
    pack $f.fSect1.fCol1.lTNo $f.fSect1.fCol1.lTDiv $f.fSect1.fCol1.lTPix  -side top -padx $Gui(pad) -pady 2 -anchor w 
    pack $f.fSect1.fCol2.eTNo $f.fSect1.fCol2.eTDiv $f.fSect1.fCol2.eTPix  -side top -anchor w
    # pack $f.fSect1.fCol1.lEmpty6 $f.fSect1.fCol2.lEmpty6 -side top -padx $Gui(pad) -pady 1 -anchor w 
    
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
# .PROC EMSegmentUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
# (Note: to do this, this proc must be this module's procMRML.  Right now,
# these buttons are being updated automatically since they have been added
# to lists updated in VolumesUpdateMRML and ModelsUpdateMRML.  So this procedure
# is not currently used.)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateGUI {} {
    global EMSegment Volume
    
    DevUpdateNodeSelectButton Volume EMSegment Volume1 Volume1 DevSelectNode
    DevUpdateNodeSelectButton Model  EMSegment Model1  Model1  DevSelectNode
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
proc EMSegmentBindingCallback { event W X Y x y t } {
    # W = window where mouse was over !
   
    global EMSegment

    # Find out intensity at pixel and update class
    # EMSegmentReadGreyValue $X $Y
    EMSegmentReadGreyValue $x $y

    # Update Just Mean and Sigma Values
    EMSegmentCalculateClassMeanSigma
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentStartEM
# Starts the EM Algorithm 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentStartEM { } {
   global EMSegment Volume Mrml Module
   set clist {}
   # Kilian: For Debugging 
   # set EMSegment(FileCIM) knee.mrf
   # if {[DevFileExists $EMSegment(FileCIM)]} {
   #    $EMSegment(Ma-lLoadText) configure -text "Reading from file ...."
   #    if {[EMSegmentReadCIMFile 0]} {
   #	   $EMSegment(Ma-lLoadText) configure -text "Finished reading CIM parameters from from file."
   #	   EMSegmentExecuteCIM Edit
   #	   set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
   #    } else {
   #	   $EMSegment(Ma-lLoadText) configure -text "Error: MRF-File was not correct!"
   #    }
   #} else {
   #    $EMSegment(Ma-lLoadText) configure -text "Error: Could not read file !"
   #}

   # Update Values
   EMSegmentCalculateClassMeanSigma
   EMSegmentCalcProb

   # Check if volume(activeID) exist  
   if {$Volume(activeID) == $Volume(idNone) } {
       DevErrorWindow "Please load a volume before starting the segmentation algorithm!"
       return
   }

   for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} { 
       if {[expr ($EMSegment(Cattrib,$i,Mean) < 0) || ($EMSegment(Cattrib,$i,Sigma) < 0)]} {
          lappend clist $i
       }
   }

   if {[llength $clist] > 0}  {
      DevErrorWindow "Please define class(es) $clist before starting segmentation!" 
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


   # Transferring Information
   vtkImageEMSegmenter EMStart    
   # EM Specific Information
   EMStart SetNumClasses      $EMSegment(NumClasses)  
   EMStart SetNumIter         $EMSegment(EMiteration)  
   EMStart SetNumRegIter      $EMSegment(MFAiteration) 
   EMStart SetAlpha           $EMSegment(Alpha) 
   EMStart SetSmoothingWidth  $EMSegment(SmWidth)    
   EMStart SetSmoothingSigma  $EMSegment(SmSigma)      
   EMStart SetStartSlice      $EMSegment(StartSlice)
   EMStart SetEndSlice        $EMSegment(EndSlice)
   EMStart SetImgTestNo       $EMSegment(ImgTestNo)
   EMStart SetImgTestDivision $EMSegment(ImgTestDivision)
   EMStart SetImgTestPixel    $EMSegment(ImgTestPixel)
   EMStart SetPrintIntermediateResults  $EMSegment(PrintIntermediateResults) 

   for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
       EMStart SetProbability  $EMSegment(Cattrib,$i,Prob) $i
       EMStart SetMu           $EMSegment(Cattrib,$i,Mean) $i
       EMStart SetSigma        $EMSegment(Cattrib,$i,Sigma) $i
       EMStart SetLabel        $EMSegment(Cattrib,$i,Label) $i 

       # Reads in the value for each class individually
       for {set j 1} { $j<= $EMSegment(NumClasses)} {incr j} {
	   for {set k 0} { $k< 6} {incr k} {
	       # Input for SetCIMMatrix : value z y x
	       EMStart SetMarkovMatrix $EMSegment(CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [expr $k+1] $j $i 
	   }
       }
   }  

   # Transfer image information
   EMStart SetInput [Volume($Volume(activeID),vol) GetOutput]

   # Create a new Volume - Got this from VolumMath.tcl
   set result [DevCreateNewCopiedVolume $Volume(activeID) "" "EMSegmentResult" ]
   set node [Volume($result,vol) GetMrmlNode]
   $node SetLabelMap 1
   Mrml(dataTree) RemoveItem $node 
   set nodeBefore [Volume($Volume(activeID),vol) GetMrmlNode]
   Mrml(dataTree) InsertAfterItem $nodeBefore $node

   # Display Result in label mode 
   Volume($result,vol) UseLabelIndirectLUTOn
   Volume($result,vol) Update
   Volume($result,node) SetLUTName -1
   Volume($result,node) SetInterpolate 0

   # Update MRML
   MainUpdateMRML
   
   #  Write Solution to new Volume  -> Here the thread is called
   Volume($result,vol) SetImageData [EMStart GetOutput]
   
   set Data [EMStart GetOutput]

   # This is necessary so that the data is updated correctly.
   # If the programmers forgets to call it, it looks like nothing
   # happened
   MainVolumesUpdate $result

   # Delete instance
   EMStart Delete


}
#-------------------------------------------------------------------------------
# .PROC EMSegmentClickLabel
# Sets everything correctly after user choosed label number and color 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentClickLabel {{label ""} {colorcode ""}} {
    global EMSegment Label Mrml 
    set Sclass $EMSegment(Class)
    # if label ="" it was called back from Label.tcl
    if {$label == ""} {
	set EMSegment(Cattrib,$Sclass,Label) $Label(label)	
	set EMSegment(Cattrib,$Sclass,ColorCode) [MakeColorNormalized $Label(diffuse)] 
    } else {
	set EMSegment(Cattrib,$Sclass,Label) $label	
	if {$colorcode == ""} {
	    # Default color is black [MakeColorNormalized "0 0 0"]
	    set EMSegment(Cattrib,$Sclass,ColorCode) "#000000"
	}
	set EMSegment(Cattrib,$Sclass,ColorCode) $colorcode
    }
    # Color Graph new 
    if {$EMSegment(Cattrib,$Sclass,Graph) } {
	$EMSegment(Cl-caGraph) itemconfigure  Graph$Sclass -fill $EMSegment(Cattrib,$Sclass,ColorCode)
    }
    # Change Color of button
    $EMSegment(EM-mbClasses) configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(EM-mbClasses).m entryconfigure [expr $Sclass-1] -background $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)

    $EMSegment(Cl-mbClasses) configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-mbClasses).m entryconfigure [expr $Sclass-1] -background $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)

    $EMSegment(Cl-bColorLabel) configure -text  $EMSegment(Cattrib,$Sclass,Label) -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -bg $EMSegment(Cattrib,$Sclass,ColorCode) \
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDisplayClassDefinition
# Displays in window current definition of class 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDisplayClassDefinition {} {
   global EMSegment

   set Sclass $EMSegment(Class)
   #--------------------- 
   # Update panel EM
   #--------------------- 
   $EMSegment(EM-mbClasses) config -text $Sclass    
   $EMSegment(EM-lSampvar)  config -text [llength $EMSegment(Cattrib,$Sclass,Sample)]
   $EMSegment(EM-lMeanvar)  config -text [expr int($EMSegment(Cattrib,$Sclass,Mean)*10000+0.5)/10000.0] 

   #---------------------
   #Update panel Class
   #---------------------
   $EMSegment(Cl-mbClasses) config -text "Class $Sclass"
   
   EMSegmentWriteTextBox   
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentUseSamples
# Use Sample for calulating Mean or Variance
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUseSamples {change} {
    global EMSegment
    if {$change == 1} {set EMSegment(UseSamples) [expr 1 - $EMSegment(UseSamples)]}
    if {$EMSegment(UseSamples) == 1} {
	$EMSegment(Cl-bSample) configure -relief sunken
	TooltipAdd $EMSegment(Cl-bSample) "Press button to manually enter Mean and Variance."
	$EMSegment(Cl-eMeanvar) configure -state disabled
	$EMSegment(Cl-eSigmavar) configure -state disabled
	$EMSegment(Cl-textBox) configure -state normal
	if {$change == 1} {EMSegmentCalculateClassMeanSigma}
    } else {
	$EMSegment(Cl-bSample) configure -relief raised
	TooltipAdd $EMSegment(Cl-bSample) "Press button to use samples for the calucation of Mean and Variance"
	$EMSegment(Cl-eMeanvar) configure -state normal
	$EMSegment(Cl-eSigmavar) configure -state normal
	$EMSegment(Cl-textBox) configure -state disabled
    }
}


#-------------------------------------------------------------------------------
# .PROC EMSegmentChangeClass
# Changes from one to another class and displays new class in window 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentChangeClass {i} {
    global EMSegment

    # Read Values from Sample Text Box for old class
    EMSegmentReadTextBox
    EMSegmentCalculateClassMeanSigma
    # EMSegmentCalcProb

    # Change active class to new one
    set EMSegment(Class) $i
    set Sclass $EMSegment(Class)

    # Change Variable the Entry field is assigned width
    $EMSegment(Cl-eMeanvar)  config -textvariable  EMSegment(Cattrib,$Sclass,Mean) 
    $EMSegment(Cl-eSigmavar) config -textvariable  EMSegment(Cattrib,$Sclass,Sigma)
    $EMSegment(Cl-eProb)  config  -textvariable EMSegment(Cattrib,$Sclass,Prob)
    $EMSegment(EM-mbClasses) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-mbClasses) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    $EMSegment(Cl-bColorLabel) config -bg $EMSegment(Cattrib,$Sclass,ColorCode) -text $EMSegment(Cattrib,$Sclass,Label)\
	    -activebackground $EMSegment(Cattrib,$Sclass,ColorCode)
    
    # Display new class
    EMSegmentDisplayClassDefinition
}
#-------------------------------------------------------------------------------
# .PROC  EMSegmentUpdateSettingButton 
# Activates changes being defined in Setting Panel
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentUpdateSettingButton { } {
    global EMSegment
    EMSegmentCreateDeleteClasses
    EMSegmentRescaleGraph $EMSegment(Graph,XminNew) $EMSegment(Graph,XmaxNew) $EMSegment(Graph,XscaNew)
    if {$EMSegment(Graph,DisplayProbNew) != $EMSegment(Graph,DisplayProb)} {
	set EMSegment(Graph,DisplayProb) $EMSegment(Graph,DisplayProbNew) 
	EMSegmentRedrawGraph
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
    set Change [EMSegmentCalculateClassMeanSigma]
    if {$EMSegment(Cattrib,$Sclass,Graph)} {
	      # Recaluclate Curve
	      set Change [EMSegmentCalculateClassCurve $Sclass]
    }
    set ChangeProb [EMSegmentCalcProb]
    if {[expr ($Change == 1) && ($ChangeProb == 0) && $EMSegment(Cattrib,$Sclass,Graph)]} {
	# Have to draw curve
	EMSegmentCalculateClassCurve $Sclass
	EMSegmentDrawClassCurve $Sclass
    }    
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassMeanSigma
# Calculates  the mean and Sigma 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassMeanSigma { } {
    global EMSegment
    set Sclass   $EMSegment(Class)
    set oldMean  $EMSegment(Cattrib,$Sclass,Mean)
    set oldSigma $EMSegment(Cattrib,$Sclass,Sigma)
    if { $EMSegment(UseSamples) == 1 } {
	#------------------------------------- 
	# Read Sample Values 
	#------------------------------------- 
	EMSegmentReadTextBox

	#------------------------------------- 
	# Calculate Mean 
	#-------------------------------------
	set MeanValue 0.0
	set Clength [llength $EMSegment(Cattrib,$Sclass,Sample)]
	for {set i 0} {$i < $Clength} {incr i} {
	    set MeanValue [expr $MeanValue + [lindex [lindex $EMSegment(Cattrib,$Sclass,Sample) $i] 2]]
	}
	if {$Clength > 0} {set EMSegment(Cattrib,$Sclass,Mean) [expr int($MeanValue / double($Clength)*10000.0+0.5)/10000.0] 
        } else { set EMSegment(Cattrib,$Sclass,Mean) -1 }
 
	#------------------------------------- 
	# Calculate Variance 
	#-------------------------------------
	set Variance 0.0
	set MeanValue  $EMSegment(Cattrib,$Sclass,Mean)

	# Variance = ((Sum(xi - mean)^2)/(n-1))^0.5
	for {set i 0} {$i < $Clength} {incr i} {
	    set Variance [expr $Variance + pow ([expr [lindex [lindex $EMSegment(Cattrib,$Sclass,Sample) $i] 2] - $MeanValue],2)] 
	}
	if {$Clength > 1} {
	    # set EMSegment(Cattrib,$Sclass,Sigma) [expr sqrt($Variance / double($Clength))]
	    set EMSegment(Cattrib,$Sclass,Sigma) [expr int(sqrt($Variance / double($Clength - 1.0))*10000.0+0.5)/10000.0]

	} elseif {$Clength == 1} {
	    set EMSegment(Cattrib,$Sclass,Sigma) 0.0
	} else {
	    set EMSegment(Cattrib,$Sclass,Sigma) -1
	}
    } else {
	set EMSegment(Cattrib,$Sclass,Mean)  [expr int($EMSegment(Cattrib,$Sclass,Mean)*10000.0+0.5)/10000.0] 
	set EMSegment(Cattrib,$Sclass,Sigma) [expr int($EMSegment(Cattrib,$Sclass,Sigma)*10000.0+0.5)/10000.0]
    }
    if {[expr (($oldMean != $EMSegment(Cattrib,$Sclass,Mean)) || ($oldSigma != $EMSegment(Cattrib,$Sclass,Sigma)))] } {
	return 1
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
    for {set i 1} {$i <= $EMSegment(NumClasses)} {incr i} {
       set EMSegment(Cattrib,$i,Prob) [expr int($EMSegment(Cattrib,$i,Prob) / $NormProb * 100+0.5) / 100.0]
    }  
    #Redraw Graph
    if {[expr ($NormProb != 1.0) && $EMSegment(Graph,DisplayProb)]} {
	EMSegmentRedrawGraph
	return 1
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
   foreach Sline $EMSegment(Cattrib,$Sclass,Sample) {
       $EMSegment(Cl-textBox) insert end "$Sline \n" 
   }
   if {$EMSegment(UseSamples) == 0} {$EMSegment(Cl-textBox) configure -state disabled}
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

   # $EMSegment(Cl-textBox) search -count NumLines \n 1.0 end
   
   while { $ReadLine != {} && $ErrorFlag == 0}  { 
      
      set ReadX [lindex $ReadLine 0]
      set ReadY [lindex $ReadLine 1]
      set ReadBrightness [lindex $ReadLine 2]

      if {$ReadBrightness > -1} {
        lappend ReadSample "$ReadX $ReadY $ReadBrightness"
      } else {
        set ErrorFlag 1 
        DevErrorWindow "Sample $i not correctly defined !" 
      }
      incr i
      set ReadLine [$EMSegment(Cl-textBox) get $i.0 $i.end]
   }

   if {$ErrorFlag == 0} {
      set EMSegment(Cattrib,$Sclass,Sample) $ReadSample
   }
}    

#-------------------------------------------------------------------------------
# .PROC  EMSegmentScrolledTextBox  
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
# .PROC  EMSegmentCreateDeleteClasses  
# Creates or deletes classes for segmentation 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateDeleteClasses {} {
    global EMSegment
    set ColorLabelLength [expr [llength $EMSegment(ColorLabelList)] / 2]

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
	  set color [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)]]
	  $EMSegment(Cl-mbClasses).m add command -label "Class $i" -command  "EMSegmentChangeClass $i" \
		  -background $color -activebackground $color 
	  $EMSegment(EM-mbClasses).m add command -label "$i" -command  "EMSegmentChangeClass $i" \
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
	  EMSegmentCreateGraphButton $i $color
      }
      # Add new Rows and lines to the CIM Matrix 
      EMSegmentCreateCIMRowsColumns $Cstart $EMSegment(NumClassesNew)              
  }
  # Set all parameter for each class
  # it is always defined as EMSegment(Catribute,<Class>,<Attribute>)
 
  for {set i $Cstart} {$i < $Cfinish } {incr i 1} {
      set EMSegment(Cattrib,$i,Prob) $Cprob 
      set EMSegment(Cattrib,$i,Mean) -1       
      set EMSegment(Cattrib,$i,Sigma) -1         
      # Sample S is defined as list S(i:) = {position1 value1} {position2 value2} ... 
      set EMSegment(Cattrib,$i,Sample) {}  
      # Graph of class is plottet (== 1) or not (== 0)  
      set EMSegment(Cattrib,$i,Graph) 0
      set EMSegment(Cattrib,$i,ColorCode) [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)]]
      set EMSegment(Cattrib,$i,Label)     [lindex $EMSegment(ColorLabelList) [expr 2*(($i-1)%$ColorLabelLength)+1]]

      # set EMSegment(MenuColor,$EMSegment(Cattrib,$i,Color)) -1
      # Define Class Specific Graph Parameters
      set EMSegment(Graph,$i,Sigma) -1 
      set EMSegment(Graph,$i,Mean)  -1
      set EMSegment(Graph,$i,XMin)  -1
      set EMSegment(Graph,$i,XMax)  -1
      set EMSegment(Graph,$i,YMax)  -1    
  }
  # Define CIM Field as Matrix M(Class1,Class2,Relation of Pixels)
  # where the "Relation of the Pixels" can be set as Pixel with "left", 
  # "right", "up" or "down" Neighbour  
  # EMSegment(CIMMatrix,<y>,<x>,<Type>)
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
# .PROC EMSegmentReadGreyValue
# When mouse is clicked, find out location and pixel gray value.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentReadGreyValue {x y} {
    global EMSegment Interactor
    
    # Look in which window we are (0 1 or 2)
    set s $Interactor(s)

    # Transform Coordinates:
    # (xs, ys) is the point relative to the lower, left corner 
    # of the slice window (0-255 or 0-511).
    # (x, y) is the point with Zoom and Double taken into consideration
    # (zoom=2 means range is 64-128 instead of 1-256)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # Get Pixel Value
    if {$EMSegment(ReadBackPickel) == 1} {set pixel [Slicer GetBackPixel $s $x $y]
    } else {set pixel [Slicer GetForePixel $s $x $y]}
   
    # Update class 
    set Sclass $EMSegment(Class)    
    lappend EMSegment(Cattrib,$Sclass,Sample) "$x $y $pixel" 
    # Update Display
    $EMSegment(Cl-textBox) insert end "$x $y $pixel \n"
    return
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateGraphButton
# Creates for Class <Sclass> a Button so the class distribution can be diplayed 
# in the graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateGraphButton {Sclass Color} {
    global EMSegment Gui
    if {$Sclass == 0 } {
	set buttonText "H"
	set TooltipText "Press button to display histogram of current active volume" 
    } else {
	set buttonText "$Sclass"
	set TooltipText "Press button to display Gaussian of class $Sclass" 
    }

    eval {button $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass -text $buttonText -width 3 \
	    -command "EMSegmentDrawDeleteCurve $Sclass"} $Gui(WBA)

    $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -bg $Color
    $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -activebackground $Color
    #if {$Sclass == 0 } {
    #	$EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -fg white 
    #	$EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -activeforeground white 
    # }
    pack $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass -side left -padx $Gui(pad)

    TooltipAdd $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass $TooltipText
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
    destroy $EMSegment(Cl-fGraphButtons).bGraphButton$Sclass 
    # Delete Line 
    if {$EMSegment(Cattrib,$Sclass,Graph) == 1} {
	$EMSegment(Cl-caGraph) delete Graph$Sclass
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawDeleteCurve
# Depending if the graph for the class <Sclass> exist it deletes it or 
# otherwise cretes a new one
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawDeleteCurve {Sclass} {
    global EMSegment Volume

    if {$EMSegment(Cattrib,$Sclass,Graph) == 1} {
	# Delete Line and raise button
	$EMSegment(Cl-caGraph) delete Graph$Sclass
	set EMSegment(Cattrib,$Sclass,Graph) 0
	$EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief raised
	EMSegmentRescaleGraph $EMSegment(Graph,Xmin) $EMSegment(Graph,Xmax) $EMSegment(Graph,Xsca)
    } else {
	# Draw Graph and lower button
	set result 0
	if {$Sclass > 0} {
	    if {[expr ($EMSegment(Cattrib,$Sclass,Mean) < 0) || ($EMSegment(Cattrib,$Sclass,Sigma) < 0)] } { 
		DevErrorWindow "Before graph can be plotted Mean and Sigma have to be defined !"
		return
	    } 

	    if {$Sclass  == $EMSegment(Class)} { 
		set result [EMSegmentCalculateClassMeanSigma]
	    }
	} 
   
        EMSegmentCalculateClassCurve $Sclass		   
	set EMSegment(Cattrib,$Sclass,Graph) 1
	set result [EMSegmentCalcProb]
	if {$result == 0} {EMSegmentDrawClassCurve $Sclass}
	$EMSegment(Cl-fGraphButtons).bGraphButton$Sclass configure -relief sunken
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalculateClassCurve 
# Calculates the gaussian distribution of class Sclass 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalculateClassCurve {Sclass} {
    global EMSegment Volume
    if {[expr ($EMSegment(Graph,$Sclass,XMin) == $EMSegment(Graph,Xmin)) && \
	      ($EMSegment(Graph,$Sclass,XMax) == $EMSegment(Graph,Xmax))]} {
       if {$Sclass > 0} {
	   if {[expr ($EMSegment(Graph,$Sclass,Sigma) == $EMSegment(Cattrib,$Sclass,Sigma)) && \
		     ($EMSegment(Graph,$Sclass,Mean)  == $EMSegment(Cattrib,$Sclass,Mean))]} {
		 return 0
	   }
       } else {
	   if {[expr ($EMSegment(Graph,$Sclass,VolumeID) == $Volume(activeID))]} {
	       return 0
	   }
       }
    } 

    set EMSegment(Graph,$Sclass,XMin)  $EMSegment(Graph,Xmin)
    set EMSegment(Graph,$Sclass,XMax)  $EMSegment(Graph,Xmax)
    set EMSegment(Graph,$Sclass,YMax) 0  

    if  {$Sclass == 0} {
	# --------------------------------------------------
	# Curve to be calculated is the Histogram
	# --------------------------------------------------
	if {$Volume(activeID) == $Volume(idNone) } {
	    DevErrorWindow "Please load a volume before triing to plot a histogram!"
	    return
	}

	set EMSegment(Graph,$Sclass,VolumeID)  $Volume(activeID)
	
        vtkImageAccumulate histogram
	# --------------------------------------------------
	# Define Settings for Histogram
	# --------------------------------------------------
	histogram SetInput [Volume($Volume(activeID),vol) GetOutput]

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
	set YhistRangeMin [lindex $histRange 0]
	set YhistRangeMax [lindex $histRange 1]

	# --------------------------------------------------
	# Read values from Histogram 
	# --------------------------------------------------
	if {$EMSegment(Graph,Xmin) > $XhistRangeMin} {
	    set XLower $EMSegment(Graph,Xmin) 
	} else {
	    set XLower $XhistRangeMin 
	    for {set i $EMSegment(Graph,Xmin)} { $i < $XhistRangeMin} {incr i} {
		set EMSegment(Graph,$Sclass,$i) 0
	    }
	}

	if {$EMSegment(Graph,Xmax) > $XhistRangeMax} {
	    set XUpper $XhistRangeMax
	    for {set i [expr $XhistRangeMax + 1]} { $i <= $EMSegment(Graph,Xmax)} {incr i} {
		set EMSegment(Graph,$Sclass,$i) 0
	    }
	} else {
	    set XUpper $EMSegment(Graph,Xmax)
	}

	incr XUpper 
	for {set idx $XLower} {$idx < $XUpper} { incr idx} {
	    set EMSegment(Graph,$Sclass,$idx) [$data GetScalarComponentAsFloat $idx 0 0 0] 
	    if {$EMSegment(Graph,$Sclass,$idx) > $EMSegment(Graph,$Sclass,YMax) } {
		set EMSegment(Graph,$Sclass,YMax) [expr double($EMSegment(Graph,$Sclass,$idx))]
	    }
	    incr histSum $EMSegment(Graph,$Sclass,$idx) 
	}
	if {$histSum > 0} {
	    for {set idx $XLower} {$idx < $XUpper} { incr idx} {
		set EMSegment(Graph,$Sclass,$idx) [expr  $EMSegment(Graph,$Sclass,$idx)/double($histSum)]
	    }
	    set EMSegment(Graph,$Sclass,YMax) [expr  $EMSegment(Graph,$Sclass,YMax)/double($histSum)]
	}
	histogram Delete
	return 1
    }
    # --------------------------------------------------
    # Curve to be calculated is a Gaussian distribution 
    # --------------------------------------------------
    # Gaus Value at the origin of the Coordinate system
    set pi 3.141592653
    set GaussMax 0
    set EMSegment(Graph,$Sclass,Sigma) $EMSegment(Cattrib,$Sclass,Sigma)  
    set EMSegment(Graph,$Sclass,Mean)  $EMSegment(Cattrib,$Sclass,Mean)
    
    for {set i $EMSegment(Graph,Xmin)} {$i <= $EMSegment(Graph,Xmax)} { incr i} {
	# Calulate Gaussian 
	if {$EMSegment(Cattrib,$Sclass,Sigma) == 0.0} {
	    set EMSegment(Graph,$Sclass,$i) [expr ($i == int($EMSegment(Cattrib,$Sclass,Mean)) ? 1 : 0)] 
	} else {
	    set EMSegment(Graph,$Sclass,$i) [expr 1.0/(sqrt(2*$pi)*$EMSegment(Cattrib,$Sclass,Sigma))*\
		    exp(-0.5*pow(($i-$EMSegment(Cattrib,$Sclass,Mean))/$EMSegment(Cattrib,$Sclass,Sigma),2))]
	}
	if {$EMSegment(Graph,$Sclass,YMax) < $EMSegment(Graph,$Sclass,$i)} {
	    set EMSegment(Graph,$Sclass,YMax) $EMSegment(Graph,$Sclass,$i)
	} 
   }
   # Update Graph Values 
   # Caluclation Error Sigma to small
   if {$EMSegment(Graph,$Sclass,YMax) == 0} {set EMSegment(Graph,$Sclass,YMax) 0.1} 
   return 1
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawClassCurve
# Calucates the Class Curve, rescales (if needed) and draws it in the graph. 
# Remember Sclass == 0 is the Histogram of current 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawClassCurve {Sclass} {
    global EMSegment
    set EMSegment(Cattrib,$Sclass,Graph) 1
    if {[EMSegmentRescaleGraph $EMSegment(Graph,Xmin) $EMSegment(Graph,Xmax) $EMSegment(Graph,Xsca)] == 0 } {
	# Curve has not been painted yet 
	EMSegmentDrawSingleCurve $Sclass
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentDrawSingleCurve
# Draws a single curve in the graph. Remember Sclass == 0 is the Histogram of current 
# active Volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentDrawSingleCurve {Sclass} {
   global EMSegment
   # Delete old line 
   if {$EMSegment(Cattrib,$Sclass,Graph) == 1} {$EMSegment(Cl-caGraph) delete Graph$Sclass
   } else {
      set EMSegment(Cattrib,$Sclass,Graph) 1
   }
   if {[expr $EMSegment(Graph,DisplayProb) && ($Sclass > 0)] } {
       set prob $EMSegment(Cattrib,$Sclass,Prob)
   } else {
       set prob 1
   }
   # Draw the Line
   set Xmin $EMSegment(Graph,Xmin)
   set Xlength [expr $EMSegment(Graph,Xmax) - $EMSegment(Graph,Xmin) + 1]
   # Graph does not display those values anymore because of side scaling 
   if {$EMSegment(Graph,Ymax) >  0.001} {
       set YposLast [expr int((1.0 - $prob * $EMSegment(Graph,$Sclass,$Xmin)/double($EMSegment(Graph,Ymax))) * \
	       $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU))]
       for {set i 1 } {$i < $Xlength} {incr i } {
	   set Ypos [expr int((1.0 - $prob*$EMSegment(Graph,$Sclass,[expr $i+$Xmin])/double($EMSegment(Graph,Ymax))) * \
		   $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU))]
	   $EMSegment(Cl-caGraph) create line  $EMSegment(Graph,Xpos,[expr $i-1]) $YposLast $EMSegment(Graph,Xpos,$i) $Ypos \
	       -width 1 -fill $EMSegment(Cattrib,$Sclass,ColorCode) -tag Graph$Sclass
	   set YposLast $Ypos
       }
   } else {
       set Ypos [expr int($EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU))]
       for {set i 1 } {$i < $Xlength} {incr i } {
	   $EMSegment(Cl-caGraph) create line  $EMSegment(Graph,Xpos,[expr $i-1]) $Ypos $EMSegment(Graph,Xpos,$i) $Ypos \
	       -width 1 -fill $EMSegment(Cattrib,$Sclass,ColorCode) -tag Graph$Sclass
       }
   }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalulateGraphXleUXscnXscpXpos 
# It will calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn), 
# EMSegment(Graph,Xscp) and EMSegment(Graph,Xpos)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalulateGraphXleUXscnXscpXpos {} {
    global EMSegment
    # Define Length one unit in pixels
    set EMSegment(Graph,XleU) [expr $EMSegment(Graph,Xlen) / double($EMSegment(Graph,Xmax) - $EMSegment(Graph,Xmin))]
    # Number of scalling lines 
    set EMSegment(Graph,Xscn) [expr int(($EMSegment(Graph,Xmax) - $EMSegment(Graph,Xmin))/double($EMSegment(Graph,Xsca)))]
    # Distance between scalling lines 
    set EMSegment(Graph,Xscp) [expr $EMSegment(Graph,Xlen)*$EMSegment(Graph,Xsca) / double($EMSegment(Graph,Xmax) - $EMSegment(Graph,Xmin))] 

    # Xscaling per point does not change, so I just have to caluclate it once ! Makes it a little bit speedier
    # Start with point 1 not origin ! 
    set Xlength [expr $EMSegment(Graph,Xmax) - $EMSegment(Graph,Xmin) + 1]
    for {set i 0 } {$i < $Xlength} {incr i } {
	set EMSegment(Graph,Xpos,$i) [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,XleU))]
    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCalulateGraphXleUXscnXscp 
# It will calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn) and 
# EMSegment(Graph,Xscp) 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCalulateGraphYscnYscp {} {
    global EMSegment
    # Number of scalling lines 
    set EMSegment(Graph,Yscn) [expr int($EMSegment(Graph,Ymax)/double($EMSegment(Graph,Ysca)))]
    # Distance between scalling lines 
    set EMSegment(Graph,Yscp) [expr $EMSegment(Graph,Ylen) / double($EMSegment(Graph,Yscn))]     
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentCreateXLabels 
# Creates the Lables for the Graph's X-Axis
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentCreateXLabels {} {
    global EMSegment Gui
    # Lables of X Coordinate Axis
    for {set i 0} { $i <= $EMSegment(Graph,Xscn)} {incr i} {
       $EMSegment(Cl-caGraph) create line [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp))]\
	       [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU) + $EMSegment(Graph,Xscl)]\
	       [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp))]\
	       [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU) - $EMSegment(Graph,Xscl)]\
	       -width 1 -tag GreyL$i
       $EMSegment(Cl-caGraph) create text [expr $EMSegment(Graph,XboL) + int($i*$EMSegment(Graph,Xscp))]\
        [expr $EMSegment(Graph,Ylen) + $EMSegment(Graph,YboU)+$EMSegment(Graph,Xscl) + $EMSegment(Graph,Xsct)]\
	       -text [expr ($i * $EMSegment(Graph,Xsca)) + $EMSegment(Graph,Xmin)]\
	       -tag GreyT$i -font {Helvetica 6} -fill $Gui(textDark) 

    }
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRedrawGraph 
# Recalcluates and redraws the whole graph
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRedrawGraph {} {
    global EMSegment
    for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
	if { $EMSegment(Cattrib,$i,Graph) == 1 } {
	    EMSegmentCalculateClassCurve $i
	    # Make sure the whole graph will be painted in EMSegmentRescaleGraph
	    set EMSegment(Graph,Ymax) [expr $EMSegment(Graph,$i,YMax) -1] 
	}
    }
    EMSegmentRescaleGraph $EMSegment(Graph,Xmin) $EMSegment(Graph,Xmax) $EMSegment(Graph,Xsca)
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentRescaleGraph
# It will rescale the Graph if necessary
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentRescaleGraph {Xmin Xmax Xsca} {
    global EMSegment
    set GraphRescale 0
    
    if {[expr ($Xmin !=  $EMSegment(Graph,Xmin)) || \
	      ($Xmax !=  $EMSegment(Graph,Xmax)) || \
	      ($Xsca !=  $EMSegment(Graph,Xsca))]} {
	if [expr ($Xmin !=  $EMSegment(Graph,Xmin)) || ($Xmax !=  $EMSegment(Graph,Xmax))] {set GraphRescale 1}
	# Rescale X-Axis if necessary
	set EMSegment(Graph,Xmin) $Xmin
	set EMSegment(Graph,Xmax) $Xmax
	set EMSegment(Graph,Xsca) $Xsca

	# Delete Labeling of X-Axis
	for {set i 0} { $i <= $EMSegment(Graph,Xscn)} {incr i} {
	    $EMSegment(Cl-caGraph) delete GreyL$i
	    $EMSegment(Cl-caGraph) delete GreyT$i
	}
	# calulate EMSegment(Graph,XleU), EMSegment(Graph,Xscn) and EMSegment(Graph,Xscp) 
	EMSegmentCalulateGraphXleUXscnXscpXpos
	# Relabel X-Axis
	EMSegmentCreateXLabels

	# Recalculate Curves
	for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
	    if { $EMSegment(Cattrib,$i,Graph) == 1 } {
		EMSegmentCalculateClassCurve $i
	    }
	}
    }

    # Check if we have to rescale graph in y-direction
    set GraphMax -1
    for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
	if {$EMSegment(Cattrib,$i,Graph) == 1} {
	    if {[expr $EMSegment(Graph,DisplayProb) && ($i > 0)]} {
		set ClassMax [expr $EMSegment(Cattrib,$i,Prob)*$EMSegment(Graph,$i,YMax)]
	    } else {
		set ClassMax $EMSegment(Graph,$i,YMax) 
	    }
	    if {$ClassMax > $GraphMax } {
		set GraphMax $ClassMax
	    }
	}
    }
    if {[expr ($GraphMax > -1) && ($GraphMax != $EMSegment(Graph,Ymax))]} {
	set GraphRescale 1
	set EMSegment(Graph,Ymax) $GraphMax
    }
    # Rescale Y-Axis if necessary
    if {$GraphRescale == 1 } {
	set EMSegment(Graph,Ysca) [expr $EMSegment(Graph,Ymax)/double($EMSegment(Graph,Yscn))]
	if {$EMSegment(Graph,Ymax) > 0.001} {
	    set EMSegment(Graph,Yscp) [expr $EMSegment(Graph,Ylen)*$EMSegment(Graph,Ysca)/double($EMSegment(Graph,Ymax))]
	} else {
	    set EMSegment(Graph,Yscp) $EMSegment(Graph,Ylen)
	}
	# Relabel Y-Axis
	for {set i 1} { $i <= $EMSegment(Graph,Yscn)} {incr i} {
	    $EMSegment(Cl-caGraph) itemconfigure ProbT$i -text [expr int($i*$EMSegment(Graph,Ysca)*1000.0)/1000.0] 
	}
    }
    # Redraw Curves 
    if {$GraphRescale == 1 } { 
	# Redraw Curves
	for {set i 0} {$i <= $EMSegment(NumClasses)} {incr i} {
	    if { $EMSegment(Cattrib,$i,Graph) == 1 } {
		EMSegmentDrawSingleCurve $i
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
    raise $EMSegment(Ma-fCIM).f$command
    focus $EMSegment(Ma-fCIM).f$command
	
    switch $command {
       Load {   $EMSegment(Ma-lLoadText) configure -text "Selecting File defining Class Interaction Matrix"
                set EMSegment(FileCIM) [ DevGetFile $EMSegment(FileCIM) 1 mrf "." "Load File with Class Interaction Matrix"  "" " " ]; 
	        
                if {[DevFileExists $EMSegment(FileCIM)]} {
		    $EMSegment(Ma-lLoadText) configure -text "Reading from file ...."
		    if {[EMSegmentReadCIMFile 1]} {
			$EMSegment(Ma-lLoadText) configure -text "Finished reading Class Interaction parameters from from file."
			EMSegmentExecuteCIM Edit
			set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
		    } else {
			$EMSegment(Ma-lLoadText) configure -text "Error: MRF-File was not correct!"
		    }
		} else {
		     $EMSegment(Ma-lLoadText) configure -text "Error: Could not read file !"
		}
	    }
	    
       Save { 	        
	        $EMSegment(Ma-lSaveText) configure -text "Saving Class Interaction Matrix to a file" 
	        set EMSegment(FileCIM) [ DevGetFile $EMSegment(FileCIM) 1 mrf "." "Save Class Interaction Matrix to a file" Save "" ];  
                EMSegmentSaveCIMFile
	        $EMSegment(Ma-lSaveText) configure -text "Finished saving. \nUse other buttons for further actions!" 
            }
   }
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
    
      eval {label $f.fLineL.lLabel  -text "" -width 7} $Gui(WLA)
      pack $f.fLineL.lLabel -side left -padx $Gui(pad) -pady 1 
   }

   # Add row text
   for {set i $start} {$i < $end} {incr i} {
       eval {label $f.fLineL.l$i -text "$i" -width 4} $Gui(WLA)
       pack $f.fLineL.l$i -side left -padx 3  -pady 1 
   }

   for {set i 1} {$i < $end} {incr i} {
       if {$i < $start} {
          # Only add new Columns
	  # EMSegment(CIMMatrix,<x>,<y>,<z>)
          for {set j $start} {$j < $end} {incr j} {	     
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(CIMMatrix,$j,$i,$CIMType) -width 4} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
	  } 

        } else {
          # Add new lines to the Matrix
          frame $f.fLine$i -bg $Gui(activeWorkspace)
          pack $f.fLine$i -side top -padx 0 -pady 0 -fill x

          eval {label $f.fLine$i.lLabel  -text "Class $i" -width 7} $Gui(WLA)
          pack $f.fLine$i.lLabel -side left -padx $Gui(pad) -pady 1 
          # Add new columns to the matrix
          for {set j 1} {$j < $end} {incr j} {
             eval {entry $f.fLine$i.eCol$j -textvariable EMSegment(CIMMatrix,$j,$i,$CIMType) -width 4} $Gui(WEA)
             pack $f.fLine$i.eCol$j -side left -padx 1 -pady 1
	  } 
        }
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

    #Debugging
    for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
	if {$EMSegment(ImgTestNo) < 1} { 
	    EMCIM SetMu           $EMSegment(Cattrib,$i,Mean) $i
	    EMCIM SetSigma        $EMSegment(Cattrib,$i,Sigma) $i
        } else { 	
	    EMCIM SetMu           [expr (($i-1)*255)/($EMSegment(NumClasses)-1)] $i
	    EMCIM SetSigma        10 $i
	}
    }  

    # Transfer image information
    EMCIM SetInput [Volume($Volume(activeID),vol) GetOutput]

    set data [EMCIM GetInput]
    $data Update
 
    # $EMSegment(data) SetWholeExtent 0 1 0 1 0 5
    # EMCIM SetComponentExtent 0 1 0 1 0 5

    set data [EMCIM GetOutput]
    # This Command calls the Thread Execute function
    $data Update

    for {set x 1} {$x <= $EMSegment(NumClasses) } {incr x} {
	set EMSegment(Cattrib,$x,Prob) [EMCIM GetProbability $x]
	# EMCIM traines the matrix (y=t, x=t-1) just the other way EMSegment (y=t-1, x=t) needs it - Sorry !
	for {set y 1} {$y <=  $EMSegment(NumClasses) } {incr y} {
	    for {set z 0} {$z < 6} {incr z} {
               set EMSegment(CIMMatrix,$x,$y,[lindex $EMSegment(CIMList) $z]) \
		       [expr int([EMCIM GetMarkovMatrix $y $x [expr $z+1]]*10000+0.5)/10000.0]  
	# set EMSegment(CIMMatrix,[expr $x+1],[expr $y+1],[lindex $EMSegment(CIMList) $z]) [$EMSegment(TData) GetScalarComponentAsFloat $x $y  $z 0]		
	    }
	}
    }
   
    # Delete instance
    EMCIM Delete
 
    # Jump to edit field 
    EMSegmentExecuteCIM Edit
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
   global EMSegment
   # Open file
   set FileID [open $EMSegment(FileCIM) r]
   # Read Lines
   set LineNumber -1
   set MatrixNumber 1
   set ErrorFlag 0 

   # Read Class Paramters
   if {$EMSegment(UseSamples) == 1} {
       $EMSegment(Cl-eMeanvar) configure -state normal
       $EMSegment(Cl-eSigmavar) configure -state normal
   }

   while { [expr ![eof $FileID] && !$ErrorFlag && ($LineNumber < $EMSegment(NumClasses))] } {
       gets $FileID LineInput
       if {[string index $LineInput 0] != "\#"} {
	   incr LineNumber
	   if {$LineNumber == 0} {
	       if {$EMSegment(NumClasses) != [lindex $LineInput 0]} {
		   if {$PermissionFlag == 1} {
		       set ChangeClassNum [DevYesNo "Do you want to overwrite current number of classes ? "]
		       if {$ChangeClassNum == "no"} { 
			   DevErrorWindow "Error(6) - while loading file:\nFile does not define paramters for $EMSegment(NumClasses) classes!"
		       return 6
		       }
		   }
		   set OverwriteSetting "yes"
		   set EMSegment(NumClassesNew) [lindex $LineInput 0]
		   EMSegmentCreateDeleteClasses
	       } else {
		   if {$PermissionFlag == 1} { 
		       set OverwriteSetting [DevYesNo "Do you want to overwrite current class setting ?"]
		   } else {set OverwriteSetting "yes"}
	       }
	   } else { 
	       if {$OverwriteSetting == "yes"} { 
		   # Read Class Probability and Distribution		   
		   if { [llength $LineInput] < 3} {
		       DevErrorWindow "Error(5) - while loading file:\nFile does not define enough Class paramters !" 
		       set ErrorFlag 5
		   }
		   set EMSegment(Cattrib,$LineNumber,Prob)  [lindex $LineInput 0]
		   set EMSegment(Cattrib,$LineNumber,Mean)  [lindex $LineInput 1]
		   set EMSegment(Cattrib,$LineNumber,Sigma) [lindex $LineInput 2]
	       }
	   }
       }
   }

   gets $FileID LineInput 
   set LineNumber 0

   if {$EMSegment(UseSamples) == 1} {
       $EMSegment(Cl-eMeanvar) configure -state normal
       $EMSegment(Cl-eSigmavar) configure -state normal
   }

   # Read CIM Paramters
   while { [expr ![eof $FileID] && !$ErrorFlag] } {
       gets $FileID LineInput
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
		       set EMSegment(CIMMatrix,$j,$k,[lindex $EMSegment(CIMList) [expr $i -1]])\
			       $CIMMatrix($j,$k,$i)
		   }
	       }
	   }
       }
   }
   return [expr 1-$ErrorFlag]
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentSaveCIMFile 
# Reads the File specifing the CIM Matrix and writes it to 
# EMSegment(MarkoMatrix,: ,: ,:) (To use some Matlab Connotation
# Lines Starting with # Will be ignored
# The form of the file: 
# The order of the Matricees has to be Neighbour see CIMlist defined in the 
# begining of the program
# Matricees are devided by one empty line
# eg Input for 2 Class Matrix would be
# # Matrix West
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
proc EMSegmentSaveCIMFile {} {
    global EMSegment
    # Open file
    if [catch {set FileID [open $EMSegment(FileCIM) w]}] {
	DevErrorWindow "Error(1) - while saving file:\nDo not have write access to $EMSegment(FileCIM) !"
	return
    }

    # Read Lines
    puts $FileID "# This is a MRF Field defined for $EMSegment(NumClasses)" 
    puts $FileID "# If you have any further questions email to slicer-devel@bwh.harvard.edu" 
    puts $FileID "# and mention the EM EMSegment and the version number - V1.0-05-Nov-01."
    puts $FileID "# Have fun - Kilian" 
    puts $FileID "# Number of classes defined with this file:"
    puts $FileID "$EMSegment(NumClasses)"

    for {set i 1} {$i <= $EMSegment(NumClasses) } {incr i} { 	
	puts $FileID "# Paramters of Class $i"
	puts $FileID "$EMSegment(Cattrib,$i,Prob) $EMSegment(Cattrib,$i,Mean) $EMSegment(Cattrib,$i,Sigma)"
    }
    # Empty Line for seperation of Class Paramters with CIM Matrix
    puts $FileID " "

    foreach i $EMSegment(CIMList) {
	puts $FileID "# Matrix $i"
	for {set j 1} {$j <= $EMSegment(NumClasses)} { incr j} {
	    for {set k 1} {$k < $EMSegment(NumClasses)} { incr k} {
	       puts -nonewline $FileID "$EMSegment(CIMMatrix,$k,$j,$i) "
	    }
	    # End of Row
	    puts $FileID "$EMSegment(CIMMatrix,$EMSegment(NumClasses),$j,$i)"
	}
	# Empty Line for seperation of Matrizees
	puts $FileID " " 
    }
    # Close File
    close $FileID
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
# Calculates the last slice of the current active Volume 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentEndSlice {flag} {
    global EMSegment Volume
    set SlideRange [Volume($Volume(activeID),node) GetImageRange] 
    set result [expr ([lindex $SlideRange 1] - [lindex $SlideRange 0] + 1)]
    if {$flag} {
	set EMSegment(EndSlice) $result
    } else {
	return $result
    }
}
#---------------------------------------------------------------
# Debugging and Test Blubber
#---------------------------------------------------------------



       # Kilian: Debugging read Volume
       # You entered in 
       # Data.tcl DataAddVolume

       #set Volume(propertyType) Basic
       # VolumesSetPropertyType # Raises window property type in Volumes.tcl
       #MainVolumesSetActive NEW
       #set Volume(freeze) 1
       # Tab Volumes row1 Props
       #set Module(freezer) "Data row1 List"

       # Volumes.tcl Basic
       #set Volume(firstFile) "image/case9.001" 
       #set Volume(readHeaders) 0
       #set Volume(labelMap) 0
       # Gets executed when browisng for image file 
       # set Volume(name)  [file root [file tail $Volume(firstFile)]]
       # set Volume(DefaultDir) [file dirname [file join $Mrml(dir) $Volume(firstFile)]]
       # set Volume(lastNum)  [MainFileFindImageNumber Last [file join $Mrml(dir) $Volume(firstFile)]]
       #set Volume(name)  DebuggBlubber
       #set Volume(DefaultDir) /home/ai2/kpohl/slicer_devel/slicer/image 
       #set Volume(lastNum) 1
       #VolumesPropsApply 
       #RenderAll

       #puts "Got here: $Volume(activeID)"

       # ------------------------------------------------
       # How to half automaitcally load a volume 
       # Add at the end of the if clause at Data.tcl -> DataAddVolume
       # set Volume(firstFile) "image/case9.001" 
       # set Volume(readHeaders) 0
       # set Volume(labelMap) 0
       # set Volume(name)  DebuggBlubber
       # set Volume(DefaultDir) /home/ai2/kpohl/slicer_devel/slicer/image 
       # set Volume(lastNum) 1
       # ------------------------------------------------
       # Add your Module Name into position 5  and "Tester" into position 7
       # of the list in program/Options.xml so its is displayed as button and first on list 
 




   # -- Try out with vtkBlubber
   #vtkBlubber BlubberTest
   #BlubberTest AddDescription "Hey You"
   # -- Start Working
   # set stuff [BlubberTest GetOutput]
   # -- Display it for me 
   #set stuff [BlubberTest ReturnDescription]
   # -- Start Working again
   #puts $stuff
   #BlubberTest Delete







