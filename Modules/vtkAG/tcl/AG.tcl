#   ==================================================
#   Module: vtkAG
#   Author: Lifeng Liu
#   Email:  liu@bwh.harvard.edu
#
#   This module implements a version of #    
#   It comes with a Tcl/Tk interface for the '3D Slicer'.
#   ==================================================
#   Copyright (C) 2003  
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#   ================================================== 
#   The full GNU Lesser General Public License file is in vtkAG/LesserGPL_license.txt


#=auto==========================================================================
#
#===============================================================================
# FILE:        AG.tcl
# PROCEDURES:  
#   AGInit
#   AGBuildGUI
#   AGBuildHelpFrame
#   AGBuildMainFrame
#   AGBuildExpertFrame
#   AGEnter
#   AGExit
#   AGUpdateGUI
#   AGCount
#   AGShowFile
#   AGBindingCallback
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
# 
#  int AG(count) counts the button presses for the demo 
#  list AG(eventManager)  list of event bindings used by this module
#  widget AG(textBox)  the text box widget
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC AGInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AGInit {} {
    global AG Module Volume Transform
    
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
    set m AG
    set Module($m,row1List) "Help Main Expert"
    set Module($m,row1Name) "{Help} {Main} {Expert} "
    set Module($m,row1,tab) Main

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
    #   set Module($m,procVTK) AGBuildVTK
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
    set Module($m,procGUI)   AGBuildGUI
    set Module($m,procEnter) AGEnter
    set Module($m,procExit)  AGExit
    set Module($m,procMRML)  AGUpdateGUI

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
        {$Revision: 1.3 $} {$Date: 2004/11/04 22:35:55 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set AG(TestReadingWriting) 0   
    set AG(CountNewResults) 1
    set AG(InputVolSource2)      $Volume(idNone)
    set AG(InputVolTarget2)      $Volume(idNone)

    set AG(InputVolSource)      $Volume(idNone)
    set AG(InputVolTarget)      $Volume(idNone)
    set AG(InputVolMask)      $Volume(idNone)
    set AG(ResultVol)     $Volume(idNone)
    set AG(ResultVol2)     $Volume(idNone)
    #Set AG(Initial_tfm)   $Transform(idNone)
    
    #set AG(Dimension)     "3"

    #General options

# set AG(DEBUG) to 1 to display more information.
    set AG(Debug) 1
   
    set AG(Linear)    "1"
    set AG(Warp)      "1"
    set AG(Initial_tfm)  "0"
     #? or{}
    set AG(Verbose)  "2"
    set AG(Scale)    "-1"
    set AG(2D)        "0"
    
    #GCR options
    set AG(Linear_group)  "2"
    set AG(Gcr_criterion) "1"
   




    #Demons options
    set AG(Tensors)  "0"
    set AG(Interpolation) "1"
    set AG(Iteration_min) "0"
    set AG(Iteration_max)  "50"
    set AG(Level_min)  "-1"
    set AG(Level_max)  "-1"
    set AG(Epsilon)    "1e-3"
    set AG(Stddev_min) [expr sqrt(-1./(2.*log(.5)))]
    set AG(Stddev_max) "1.25"
    set AG(SSD)    "1" 

   #Intensity correction

    set AG(Intensity_tfm) "mono-functional"   
    set AG(Force)   "1"
    set AG(Degree)   1
    set AG(Ratio)    1
    set AG(Nb_of_functions)  1
    set AG(Nb_of_pieces)    {}
    set AG(Use_bias)        0
    set AG(Boundaries)      {}

   # set AG(StandardDev)   "1"
   # set AG(Threshold)     "10"
   # set AG(Attachment)    "0.05"
   # set AG(Iterations)    "5"
   # set AG(IsoCoeff)      "0.2"
    #set AG(TruncNegValues)  "0"
   # set AG(NumberOfThreads) "4"

    #set AG(TangCoeff)     "1"

    #set AG(MincurvCoeff)  "1"
    #set AG(MaxcurvCoeff)  "0.1"

    # Event bindings! (see AGEnter, AGExit, tcl-shared/Events.tcl)
    set AG(eventManager)  { \
        {all <Shift-1> {AGBindingCallback Shift-1 %W %X %Y %x %y %t}} \
        {all <Shift-2> {AGBindingCallback Shift-2 %W %X %Y %x %y %t}} \
        {all <Shift-3> {AGBindingCallback Shift-3 %W %X %Y %x %y %t}} }
}


#-------------------------------------------------------------------------------
# .PROC AGUpdateGUI
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
proc AGUpdateGUI {} {
    global AG Volume
    
    DevUpdateNodeSelectButton Volume AG InputVolSource   InputVolSource   DevSelectNode
    DevUpdateNodeSelectButton Volume AG InputVolTarget   InputVolTarget   DevSelectNode
    DevUpdateNodeSelectButton Volume AG InputVolMask   InputVolMask   DevSelectNode
    DevUpdateNodeSelectButton Volume AG InputVolSource2   InputVolSource2   DevSelectNode
    DevUpdateNodeSelectButton Volume AG InputVolTarget2   InputVolTarget2   DevSelectNode
 

    DevUpdateNodeSelectButton Volume AG ResultVol  ResultVol  DevSelectNode  0 1 1

    DevUpdateNodeSelectButton Volume AG ResultVol2  ResultVol2  DevSelectNode 0 1 1
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
# .PROC AGBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc AGBuildGUI {} {
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Parameters" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(AG,fMain)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Parameters
    #-------------------------------------------
    
    AGBuildHelpFrame
       
    AGBuildExpertFrame

    AGBuildMainFrame


}

#-------------------------------------------------------------------------------
# .PROC AGBuildHelpFrame
#
#   Create the Help frame
# .END
#-------------------------------------------------------------------------------
proc AGBuildHelpFrame {} {


    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The AG module contains  <P>
    The input parameters are:
    <BR>
    <UL>
    <LI><B> Input Target Channel 1:</B> 
    <LI><B> Input Target Channel 2:</B>
    <LI><B> Input Source Channel 1:</B>
    <LI><B> Input Source Channel 2:</B>
    <LI><B> Input Mask  :</B> 
    <P> 

    If the Soruce channel 2 or Target channel 2 is empty, then only one channel is used for the registration. <P>
"
    regsub -all "\n" $help {} help
    MainHelpApplyTags AG $help
    MainHelpBuildGUI  AG

}
# end AGBuildHelpFrame


#-------------------------------------------------------------------------------
# .PROC AGBuildMainFrame
#
#   Create the Main frame
# .END
#-------------------------------------------------------------------------------
proc AGBuildMainFrame {} {


    global Gui AG Module Volume

    #-------------------------------------------
    # Main frame
    #-------------------------------------------
    set fMain $Module(AG,fMain)
    set f $fMain
  
    frame $f.fIO               -bg $Gui(activeWorkspace) -relief groove -bd 3
    #frame $f.fDimension        -bg $Gui(activeWorkspace)
    #frame $f.fThreshold        -bg $Gui(activeWorkspace)
    #frame $f.fIterations       -bg $Gui(activeWorkspace)
    #frame $f.fNumberOfThreads  -bg $Gui(activeWorkspace)
    #frame $f.fTruncNegValues   -bg $Gui(activeWorkspace)
    frame $f.fRun              -bg $Gui(activeWorkspace)

#  $f.fDimension  
#  $f.fTruncNegValues 
#     $f.fThreshold  \
#      $f.fIterations \
#     $f.fNumberOfThreads \
#       

    pack  $f.fIO \
          $f.fRun \
      -side top -padx 0 -pady 1 -fill x
    
    #-------------------------------------------
    # Parameters->Input/Output Frame
    #-------------------------------------------
    set f $fMain.fIO
    
    # Add menus that list models and volumes
    DevAddSelectButton  AG $f InputVolTarget "Input Target Volume Channel 1" Grid
    DevAddSelectButton  AG $f InputVolTarget2 "Input Target Volume Channel 2" Grid
    DevAddSelectButton  AG $f InputVolSource "Input Source Volume Channel 1" Grid
    DevAddSelectButton  AG $f InputVolSource2 "Input Source Volume Channel 2" Grid  
    DevAddSelectButton  AG $f InputVolMask "Input Mask Volume" Grid

    DevAddSelectButton  AG $f ResultVol "Result Volume Channel 1" Grid
    DevAddSelectButton  AG $f ResultVol2 "Result Volume channel 2" Grid



    #-------------------------------------------
    # Parameters->Run Frame
    #-------------------------------------------
    set f $fMain.fRun
    
    DevAddButton $f.bRun "Run" "RunAG"

    DevAddButton $f.bTestWriting "Test Write vtkImageData" "AGTestWriting"
    
    DevAddButton $f.bTestReading "Test Read vtkImageData" " AGTestReadvtkImageData"
    DevAddButton $f.bTestBatch "Test batch transformation" "AGBatchProcessResampling"

    #DevAddButton $f.bNormalize "Normalize" "NormalizeAG"
    
    pack $f.bRun  $f.bTestBatch
    #$f.bTestWriting  $f.bTestReading
    #$f.bNormalize
  

}
# end AGBuildMainFrame


#-------------------------------------------------------------------------------
# .PROC AGBuildExpertFrame
#
#   Create the Expert frame
# .END
#-------------------------------------------------------------------------------
proc AGBuildExpertFrame {} {
    global Gui AG Module

    set fExpert $Module(AG,fExpert)
    set f $fExpert
     
    # Use option menu 
    #set  menuLRoptions [tk_optionMenu $f.linearRegistrationOptions AG(LinearRegistrationOption) {do not compute linear transformation} one two three] 
   #$f.linearRegistrationOptions configure  -font {helvetica 8} 
   #$menuLRoptions configure -font {helvetica 8} 
  # pack $f.linearRegistrationOptions -side top -pady $Gui(pad) -padx $Gui(pad) -expand 1 -fill x
 # Use menu button.

# constrain for linear registration.    
   eval {label $f.lLR -text "Linear registration"} $Gui(WLA)
    set AG(LRName) "affine group"
    eval {menubutton $f.mbLR -text "$AG(LRName)" -relief raised -bd 2 -width 20 \
        -menu $f.mbLR.m} $Gui(WMBA)
    eval {menu $f.mbLR.m} $Gui(WMA)
    set AG(mbLR) $f.mbLR
    set m $AG(mbLR).m
   foreach v "{do not compute linear transformation} {translation} {rigid group} {similarity group} {affine group}" {
       $m add command -label $v -command "ModifyOption LinearRegistration {$v}"
   }
    TooltipAdd $f.mbLR "Choose how to restrict linear registration." 
    #pack $f.lLR $f.mbLR  -padx $Gui(pad) -side left -anchor w   
   grid $f.lLR  $f.mbLR -pady 2 -padx $Gui(pad) -sticky e


# warp and force
    eval {label $f.lWarp -text "Warp"} $Gui(WLA)
    set AG(WarpName) "Warp with force demons"
    eval {menubutton $f.mbWarp -text "$AG(WarpName)" -relief raised -bd 2 -width 20 \
        -menu $f.mbWarp.m} $Gui(WMBA)
    eval {menu $f.mbWarp.m} $Gui(WMA)
    set AG(mbWarp) $f.mbWarp
    set m $AG(mbWarp).m
   foreach v "{No warp} {Warp with force demons} {warp with force optical flow}" {
       $m add command -label $v -command "ModifyOption Warp {$v}"
   }
    TooltipAdd $f.mbWarp "Choose how to warp." 
    #pack $f.lWarp $f.mbWarp -after $f.lLR  -padx $Gui(pad) -side left -anchor w   
    grid $f.lWarp $f.mbWarp   -pady 2 -padx $Gui(pad) -sticky e

# Intensity transformation
    eval {label $f.lIntensityTFM -text "Intensity Transform"} $Gui(WLA)
    set AG(IntensityTFMName) "mono functional"
    eval {menubutton $f.mbIntensityTFM -text "$AG(IntensityTFMName)" -relief raised -bd 2 -width 20 \
        -menu $f.mbIntensityTFM.m} $Gui(WMBA)
    eval {menu $f.mbIntensityTFM.m} $Gui(WMA)
    set AG(mbIntensityTFM) $f.mbIntensityTFM
    set m $AG(mbIntensityTFM).m
   foreach v "{mono functional} {piecewise median} {no intensity transform}" {
       $m add command -label $v -command "ModifyOption IntensityTFM {$v}"
   }
    TooltipAdd $f.mbIntensityTFM "Choose intensity transform typehow."  
    grid $f.lIntensityTFM $f.mbIntensityTFM   -pady 2 -padx $Gui(pad) -sticky e

# Criterion
    eval {label $f.lCriterion -text "Criterion"} $Gui(WLA)
    set AG(CriterionName) "GCR L1 norm"
    eval {menubutton $f.mbCriterion -text "$AG(CriterionName)" -relief raised -bd 2 -width 20 \
        -menu $f.mbCriterion.m} $Gui(WMBA)
    eval {menu $f.mbCriterion.m} $Gui(WMA)
    set AG(mbCriterion) $f.mbCriterion
    set m $AG(mbCriterion).m
   foreach v "{GCR L1 norm} {GCR L2 norm} {Correlation} {mutual information}" {
       $m add command -label $v -command "ModifyOption Criterion {$v}"
   }
    TooltipAdd $f.mbCriterion "Choose the criterion." 
    grid $f.lCriterion $f.mbCriterion   -pady 2 -padx $Gui(pad) -sticky e


# Verbose level
    eval {label $f.lVerbose -text "Verbose"} $Gui(WLA)
    set AG(VerboseName) "1"
    eval {menubutton $f.mbVerbose -text "$AG(VerboseName)" -relief raised -bd 2 -width 20 \
        -menu $f.mbVerbose.m} $Gui(WMBA)
    eval {menu $f.mbVerbose.m} $Gui(WMA)
    set AG(mbVerbose) $f.mbVerbose
    set m $AG(mbVerbose).m
   foreach v "0 1 2" {
       $m add command -label $v -command "ModifyOption Verbose {$v}"
   }
    TooltipAdd $f.mbVerbose "Choose the Verbose." 
    grid $f.lVerbose $f.mbVerbose   -pady 2 -padx $Gui(pad) -sticky e

# checkbox type options:  not use SSD, 2D registration, estimate bias, last 6 channels of data are tensors
    eval {label $f.lUseSSD -text "SSD:"} $Gui(WLA)
  
    eval {checkbutton $f.cUseSSDLabel \
        -text  "Use SSD" -variable AG(SSD) \
        -width 20  -indicatoron 0 } $Gui(WCA)
    grid $f.lUseSSD $f.cUseSSDLabel  -pady 2 -padx $Gui(pad) -sticky e
    TooltipAdd $f.cUseSSDLabel "Press to set/unset using SSD to stop iterations."
 
    eval {label $f.lEstimateBias -text "Bias:"} $Gui(WLA)
  
    eval {checkbutton $f.cEstimateBias \
        -text  "Estimate Bias" -variable AG(Use_bias) \
        -width 20  -indicatoron 0 } $Gui(WCA)
    grid $f.lEstimateBias $f.cEstimateBias  -pady 2 -padx $Gui(pad) -sticky e
    TooltipAdd $f.cEstimateBias "Press to set/unset to estimate bias with intensity transformation." 
     eval {label $f.l2DRegistration -text "2D registration:"} $Gui(WLA)
  
    eval {checkbutton $f.c2DRegistration \
        -text  "2D" -variable AG(2D) \
        -width 20  -indicatoron 0 } $Gui(WCA)
    grid $f.l2DRegistration  $f.c2DRegistration  -pady 2 -padx $Gui(pad) -sticky e
    TooltipAdd $f.c2DRegistration "Press to set/unset to do 2D registration."

    eval {label $f.lTensor -text "Tensors:"} $Gui(WLA)
  
    eval {checkbutton $f.cTensor \
        -text  "last 6 channels are tensors" -variable AG(Tensors) \
        -width 20  -indicatoron 0 } $Gui(WCA)
    grid $f.lTensor $f.cTensor  -pady 2 -padx $Gui(pad) -sticky e
    TooltipAdd $f.cTensor "Press to set/unset that last 6 channels are tensors."
   
# entry type options

   eval {label $f.lScale -text "Scale factor:"} $Gui(WLA) 
   eval {entry $f.eScale -justify right -width 20 -textvariable AG(Scale)} $Gui(WEA)
   grid $f.lScale $f.eScale -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eScale  "Enter the scale factor to scale the intensities before registration."
 
   eval {label $f.lIteration_min -text "Iteration min:"} $Gui(WLA) 
   eval {entry $f.eIteration_min -justify right -width 20 -textvariable AG(Iteration_min)} $Gui(WEA)
   grid $f.lIteration_min $f.eIteration_min -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eIteration_min  "Enter the number of minumum iterations at each level."
     

   eval {label $f.lIteration_max -text "Iteration max:"} $Gui(WLA) 
   eval {entry $f.eIteration_max -justify right -width 20 -textvariable AG(Iteration_max)} $Gui(WEA)
   grid $f.lIteration_max $f.eIteration_max -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eIteration_max  "Enter the number of maxmimum iterations at each level."
    
   eval {label $f.lDegree -text "Degree:"} $Gui(WLA) 
   eval {entry $f.eDegree -justify right -width 20 -textvariable AG(Degree)} $Gui(WEA)
   grid $f.lDegree $f.eDegree -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eDegree  "Enter the degree of polynomials."

   eval {label $f.lRatio -text "Ratio of points:"} $Gui(WLA) 
   eval {entry $f.eRatio -justify right -width 20 -textvariable AG(Ratio)} $Gui(WEA)
   grid $f.lRatio $f.eRatio -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eRatio  "Enter the ratio of points used for polynomial estimate."


   eval {label $f.lLevel_min  -text "Min Level:"} $Gui(WLA) 
   eval {entry $f.eLevel_min -justify right -width 20 -textvariable AG(Level_min)} $Gui(WEA)
   grid $f.lLevel_min $f.eLevel_min -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eLevel_min  "Enter the minimum level in pyramid."

   eval {label $f.lLevel_max -text "Max Level:"} $Gui(WLA) 
   eval {entry $f.eLevel_max -justify right -width 20 -textvariable AG(Level_max)} $Gui(WEA)
   grid $f.lLevel_max $f.eLevel_max -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eLevel_max  "Enter the maximum level in pyramid."


   eval {label $f.lNb_of_functions -text "Number of functions:"} $Gui(WLA) 
   eval {entry $f.eNb_of_functions -justify right -width 20 -textvariable AG(Nb_of_functions)} $Gui(WEA)
   grid $f.lNb_of_functions $f.eNb_of_functions -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eNb_of_functions  "Enter the number of intensity transformation functions."



   eval {label $f.lEpsilon -text "Epsilon:"} $Gui(WLA) 
   eval {entry $f.eEpsilon -justify right -width 20 -textvariable AG(Epsilon)} $Gui(WEA)
   grid $f.lEpsilon $f.eEpsilon -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eEpsilon  "Enter the maximum SSD value between successive iterations ."

  
   eval {label $f.lStddev_min -text "Min Stddev:"} $Gui(WLA) 
   eval {entry $f.eStddev_min -justify right -width 20 -textvariable AG(Stddev_min)} $Gui(WEA)
   grid $f.lStddev_min $f.eStddev_min -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eStddev_min  "Enter the minimum standard deviation of displacement field smoothing kernel ."
 

   eval {label $f.lStddev_max -text "Max Stddev:"} $Gui(WLA) 
   eval {entry $f.eStddev_max -justify right -width 20 -textvariable AG(Stddev_max)} $Gui(WEA)
   grid $f.lStddev_max $f.eStddev_max -pady 2 -padx $Gui(pad) -sticky e   
   TooltipAdd $f.eStddev_max  "Enter the maximum standard deviation of displacement field smoothing kernel."


}
 


# end  AGBuildExpertFrame

proc Test {}  {
  
  puts "AG(SSD) is $AG(SSD)"
}

#.PROC   ModifyOptions
#  Modify the options for registration according to the user 
#  selection
#.END
proc ModifyOptions {optClass value} {
    global  AG Volume Gui 
  
    switch $optClass {
LinearRegistration  { 
    set AG(LRName)  $value
    $AG(mbLR) config -text $AG(LRName)

    switch $value {
"do not compute linear transformation" { 
                    puts "do not compute linear transformation"
    set AG(Linear)    0 
}
                "translation" { 
    set AG(Linear)    1 
    set AG(Linear_group) -1
                    puts "translation"
}
"rigid group" {
    set AG(Linear) 1
    set AG(Linear_group) 0
                    puts "rigid group"

}
"similarity group" {
    set AG(Linear) 1
    set AG(Linear_group) 1
    puts "similarity group"
}
"affine group" {
    set AG(Linear) 1
    set AG(Linear_group) 2
    puts "affine group"
                    puts "AG(SSD) is $AG(SSD)"
    puts "AG(Scale) is $AG(Scale)"
}
default {
    set AG(Linear) 1
    set AG(Linear_group) 2
    puts "default"
}
    }  
}
Warp {

    set AG(WarpName)  $value
    $AG(mbWarp) config -text $AG(WarpName)
      switch $value {
"No warp" {
    set  AG(Warp) 0
}
"Warp with force demons" {
    set AG(Warp)  1
    set AG(Force) 1
}
"warp with force optical flow" {
    set AG(Warp)  1
    set AG(Force) 2
}
default {
    set AG(Warp)  1
    set AG(Force) 1  
}
    }
}

IntensityTFM {

    set AG(IntensityTFMName)  $value
    $AG(mbIntensityTFM) config -text $AG(IntensityTFMName)
      switch $value {
"no intensity transform" {
    set  AG(Intensity_tfm) "none"
}
"mono functional" {
    set AG(Intensity_tfm)  "mono functional"

}
"piecewise median" {
    set AG(Intensity_tfm)  "piecewise median"

}
default {
    set AG(Intensity_tfm)  "mono functional"
  
}
    }
}

Criterion {

    set AG(CriterionName)  $value
    $AG(mbCriterion) config -text $AG(CriterionName)
      switch $value {
"GCR L1 norm" {
    set  AG(Gcr_criterion) 1
}
"GCR L2 norm" {
    set AG(Gcr_criterion)  2

}
"Correlation" {
    set AG(Gcr_criterion)  3

}

"mutual information" {
    set AG(Gcr_criterion)  4

}
default {
    set AG(Gcr_criterion)  1
  
}
    }
}

Verbose {
    set AG(VerboseName)  $value
    $AG(mbVerbose) config -text $AG(VerboseName)
    set  AG(Verbose)  $value
}
    }
    return
}
#end  ModifyOptions



#-------------------------------------------------------------------------------
# .PROC AGEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AGEnter {} {
    global AG
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $AG(eventManager)

    # clear the text box and put instructions there
#    $AG(textBox) delete 1.0 end
#    $AG(textBox) insert end "Shift-Click anywhere!\n"

}

#-------------------------------------------------------------------------------
# .PROC AGExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AGExit {} {

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
# .PROC AGCount
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc AGCount {} {
    global AG
    
    incr AG(count)
    $AG(lParameters) config -text "You clicked the button $AG(count) times"
}


#-------------------------------------------------------------------------------
# .PROC AGShowFile
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc AGShowFile {} {
    global AG
    
    $AG(lfile) config -text "You entered: $AG(FileName)"
}


#-------------------------------------------------------------------------------
# .PROC AGBindingCallback
# Demo of callback routine for bindings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AGBindingCallback { event W X Y x y t } {
    global AG

    set insertText "$event at: $X $Y\n"
    
    switch $event {
    "Shift-2" {
        set insertText "Don't poke the Slicer!\n"
    }
    "Shift-3" {
        set insertText "Ouch!\n"
    }

    }
#    $AG(textBox) insert end $insertText

}


#-------------------------------------------------------------------------------
# .PROC AGPrepareResult
#   Create the New Volume if necessary. Otherwise, ask to overwrite.
#   returns 1 if there is are errors 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc AGCheckErrors {} {
    global AG Volume

    if {  ($AG(InputVolSource) == $Volume(idNone)) || \
        ($AG(InputVolTarget) == $Volume(idNone)) || \
        ($AG(ResultVol)   == $Volume(idNone))}  {
    DevErrorWindow "You cannot use Volume \"None\" for input or output"
    return 1
    }

    if {  ($AG(InputVolSource) == $AG(ResultVol)) || \
        ($AG(InputVolTarget) == $AG(ResultVol)) || \
        ($AG(InputVolMask)   == $AG(ResultVol))}  {
        DevErrorWindow "You cannot use one of the input Volumes as the result Volume"
        return 1

    }

    if { ($AG(InputVolSource2) != $Volume(idNone)) && ($AG(InputVolTarget2) != $Volume(idNone)) }  {    
    if { ($AG(ResultVol2)   == $Volume(idNone))} {    
        DevErrorWindow "You cannot use Volume \"None\" for input or output"
        return 1
    }

    if {  ($AG(InputVolSource2) == $AG(ResultVol)) || ($AG(InputVolSource2) == $AG(ResultVol2)) || \
         ($AG(InputVolTarget2) == $AG(ResultVol)) || ($AG(InputVolTarget2) == $AG(ResultVol2)) ||\
         ($AG(InputVolSource) == $AG(ResultVol2)) || ($AG(InputVolTarget) == $AG(ResultVol2)) || \
         ($AG(InputVolMask)   == $AG(ResultVol2))}  {
         DevErrorWindow "You cannot use one of the input Volumes as the result Volume"
         return 1
     }
     }

     return 0
}


#-------------------------------------------------------------------------------
# .PROC AGPrepareResultVolume
#   Check for Errors in the setup
#   returns 1 if there are errors, 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc AGPrepareResultVolume {}  {
     global AG Volume
   
    #Make or Copy the result volume node from target volume ( but should keep
    # the win, level setting).
    set v1 $AG(InputVolTarget)
    set v2 $AG(ResultVol)
    set v2_2 $AG(ResultVol2)
    # Do we need to Create a New Volume?
    # If so, let's do it.
    
    if {$v2 == -5 } {
        set v2 [DevCreateNewCopiedVolume $v1 ""  "AGResult_$AG(CountNewResults)" ]
        set node [Volume($v2,vol) GetMrmlNode]
        Mrml(dataTree) RemoveItem $node 
        set nodeBefore [Volume($v1,vol) GetMrmlNode]
        Mrml(dataTree) InsertAfterItem $nodeBefore $node
        set AG(ResultVol) $v2
        #VolumesUpdateMRML
        MainUpdateMRML
        AGUpdateGUI
        incr AG(CountNewResults)

    } else { 
    
        # Are We Overwriting a volume?
        # If so, let's ask. If no, return.
    
        set v2name  [Volume($v2,node) GetName]
        set continue [DevOKCancel "Overwrite $v2name?"]
          
        if {$continue == "cancel"} { return 1 }
        # They say it is OK, so overwrite!
              
        Volume($v2,node) Copy Volume($v1,node)
    }


    if { ($AG(InputVolSource2) != $Volume(idNone)) && ($AG(InputVolTarget2) != $Volume(idNone)) } {

        if {($v2_2 == -5)} {
            
            set v1_2 $AG(InputVolTarget2)      
            set v2_2 [DevCreateNewCopiedVolume $v1_2 ""  "AGResult2_$AG(CountNewResults)" ]
            set node [Volume($v2_2,vol) GetMrmlNode]
            Mrml(dataTree) RemoveItem $node 
            set nodeBefore [Volume($v1_2,vol) GetMrmlNode]
            Mrml(dataTree) InsertAfterItem $nodeBefore $node
            set AG(ResultVol2) $v2_2
            #VolumesUpdateMRML
            MainUpdateMRML

            
        } else {
            
            set v2name_2  [Volume($v2_2,node) GetName]
            set continue [DevOKCancel "Overwrite $v2name_2?"]
            
            if {$continue == "cancel"} { return 1 }
            # They say it is OK, so overwrite
            Volume($v2_2,node) Copy Volume($AG(InputVolTarget2),node)
        }
    } 
    
    return 0
}

#-------------------------------------------------------------------------------
# .PROC  AGWritevtkImageData
# Write vtkImageData to file using the vtkStructuredPointWriter
#
#-----------------------------------------------------------------------------
proc AGWritevtkImageData {image filename} {

    global AG
    catch "writer Delete"
    vtkStructuredPointsWriter  writer
    if {$AG(Debug) == 1} {
    writer DebugOn
    }
    writer SetFileTypeToBinary
    writer SetInput  $image
    writer SetFileName $filename
    writer Write
    writer Delete
}




#-------------------------------------------------------------------------------
# .PROC AGIntensityTransform
# According to the options, set the intensity transformation. 
#  
# .END
#------------------------------------------------------------------------------
proc  AGIntensityTransform {Source} {
    global AG Volume

    catch {$AG(tfm) Delete}
    switch $AG(Intensity_tfm) {
      "mono-functional"  {
          puts "$AG(Intensity_tfm)==mono-functional is true"
          vtkLTSPolynomialIT tfm
          tfm SetDegree $AG(Degree)
          tfm SetRatio $AG(Ratio)  
          tfm SetNumberOfFunctions $AG(Nb_of_functions)
          if { $AG(Use_bias) == 1 } {
             tfm UseBiasOn
          }
          set AG(tfm) tfm
          return 0
      }
      "piecewise-median" {
          puts " intensity+tfm is piecewise-median"
          vtkPWMedianIT tfm
          if {([llength $AG(Nb_of_pieces)] == 0) && ($AG(Boundaries) == 0)} { 
            $Source  Update
            set low_high [$Source  GetScalarRange]
            set low [lindex $low_high 0]
            set high [lindex $low_high 1]
            for {set index 0} {$index < $AG(Nb_of_functions)} {incr index} {
              lappend AG(Nb_of_pieces) [expr $high-$low+1]
            }
         
            for {set index2 $low+1} {$index2 < $hight+1} {incr index2} {
               lappend AG(Boundaries) $index2
            }
          }
   
          set nf $AG(Nb_of_functions)
          set np $AG(Nb_of_pieces)
          set bounds $AG(Boundaries)
          if {( [llength $np] == 0) || ( [llength $np] != $nf)} {
             #raise Exception
             puts "length of number of pieces doesn\'t match number of functions"
             return 1
          }
       
          tfm SetNumberOfFunctions $nf
          for {set  f  0}  {$f < $nf} {incr f} {
            tfm SetNumberOfPieces {$f [lindex $np $f]}
            set i 0
            for {set p 0} {$p <  [lindex $np $f]-1} {incr p}{
              tfm SetBoundary {$f $p [lindex $bounds $i]}
              incr i
            }
          }  
          set AG(tfm) tfm
          return 0
      }

      "none" {
         #set tfm None
         return 1

      }

      default  {
           puts "unknown intensity tfm type: $AG(Intensity_tfm)"
           #raise exception
           #set tfm None
           return 1

      }
  }    
  
}

#-------------------------------------------------------------------------------
# .PROC AGTransformScale
# According to the options, do the scale transformation. 
#  
# .END
#------------------------------------------------------------------------------
proc  AGTransformScale { Source Target} {
#def TransformScale(Target,Source,scale):   
   #    log=vtkImageMathematics()
   #    log.SetOperationToLog()
   #    log.SetInput1(cast.GetOutput())
   global AG Volume 

   if { $AG(Scale) <= 0} {
    return 0
   }
   
   vtkImageMathematics div
   div SetOperationToMultiplyByK
   div SetConstantK  $AG(Scale)
   div SetInput1 $Target
# [Volume($AG(InputVolTarget),vol) GetOutput]
   div Update
  # [Volume($AG(InputVolTarget),vol) GetOutput] DeepCopy [div GetOutput]
  $Target  DeepCopy [div GetOutput]  
 # or Volume($AG(InputVolTarget),vol) SetImageData [div GetOutput] , but maybe they share the same copy of data.

   div Delete
   vtkImageMathematics div2
   div2 SetOperationToMultiplyByK
   div2 SetConstantK $AG(Scale)
  # div2 SetInput1  [Volume($AG(InputVolSource),vol) GetOutput]
   
   div2 SetInput1  $Source
   div2 Update
   #[Volume($AG(InputVolSource),vol) GetOutput] DeepCopy [div2 GetOutput]
   $Source  DeepCopy [div2 GetOutput]
   div2 Delete
   return 1
}


proc  WriteHomogeneous {t ii fileid} {

   
    puts " Start to save homogeneous Transform"
    puts $fileid "Homogeneous Transform\n"

    set str ""
    set m [DataAddTransform 1 0 0]
    #set matout [[Matrix($m,node)  GetTransform] GetMatrix]
    set trans [Matrix($m,node) GetTransform] 
    $trans Identity
    set mat [$t GetMatrix]
    catch "mat_copy Delete"
    vtkMatrix4x4 mat_copy
    mat_copy DeepCopy $mat
    $trans Concatenate mat_copy
   
    for {set  i  0}  {$i < 4} {incr i} {
    for {set  j  0}  {$j < 4} {incr j} {
        set one_element [$mat GetElement $i $j]
    #    $matout SetElement $i $j $one_element
        set str "$str $one_element"
            puts $fileid  "  $one_element "
    }
    puts $fileid "\n"
    }
    # Add a transform to the slicer.
  

  
 
    puts " m is $m"
    puts " str is ---$str"
    
   
# SetMatrix $str
    puts " finish saving homogeneous Transform"

} 

proc WriteGrid {t ii fileid} {      
    set g [$t GetDisplacementGrid]
    if { $g == 0}  return

    puts  $fileid "Grid Transform\n" 
    set inverse [$t GetInverseFlag]
    puts $fileid   " $inverse \n"

    set fname "transform.vtk"
   
    puts $fileid "$fname \n"

    AGWritevtkImageData $g  $fname
    #vtkImageWriter Writer 
    #Writer SetInput $g
    #Writer SetFileDimensionality 3 
    #Writer SetFileName "transform.vtk"
    #Writer Write
    #Writer Delete
}

#-------------------------------------------------------------------------------
# .PROC WritePWConstant
#  Save the polynomial intensity transform to file
#
# .END
#-------------------------------------------------------------------------------
proc  WritePWConstant {it fileid} {

    puts $fileid "Piecewise Constant IT\n"
    
    set nf [$it GetNumberOfFunctions]
    
    puts $fileid " nf \n"

    for {set f 0} {$f < $nf} {incr f} {
    set value [$it GetNumberOfPieces $f]
    puts $fileid " $value "
    }
    puts $fileid "\n"

    for {set f 0} {$f < $nf} {incr f} { 
    set np [$it GetNumberOfPieces $f]
    for {set p 0} {$p < [expr $np - 1]} {incr p} { 
        set value [$it GetBoundary $f $p]
        puts $fileid " $value " 
    }
    puts $fileid "\n"
    for {set p 0} {$p < $np} {incr p} { 
        set value [$it GetValue $f $p]
        puts $fileid " $value " 
    }
    puts $fileid "\n"
    }
    
}
#-------------------------------------------------------------------------------
# .PROC WritePolynomial
#  Save the polynomial intensity transform to file
#
# .END
#-------------------------------------------------------------------------------
proc  WritePolynomial {it fileid} {

    puts $fileid "Polynomial IT\n"

    set nf [$it GetNumberOfFunctions]
    set nd [$it GetDegree]

    puts $fileid " $nf $nd \n "
    for {set  f  0}  {$f < $nf} {incr f } {
    for {set  d  0}  {$d < [expr $nd + 1]} {incr d } {
        set value [$it GetAlpha $f $d]
        puts $fileid  " $value "
    }
    puts $fileid "\n"
    }
}


#-------------------------------------------------------------------------------
# .PROC WriteIntensityTransform
#  Save the transform to file
#
# .END
#-------------------------------------------------------------------------------
proc WriteIntensityTransform {it fileid } {
      
    if {($it != 0)} {
    if {[$it IsA vtkPolynomialIT]} {
        WritePolynomial $it $fileid
    }
    if {[$it IsA vtkPWConstantIT]} {
        WritePWConstant $it    $fileid
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC WriteTransform
#  Save the transform to file
#
# .END
#-------------------------------------------------------------------------------
proc WriteTransform {gt flag it FileName} {

    global AG  
  
    set fileid [open $FileName  w+]

    seek $fileid 0 start

    puts $fileid "VTK Transform File\n"
     
    if { ($gt != 0 ) } {
         set n [$gt GetNumberOfConcatenatedTransforms]
     if {$AG(Debug) == 1} {
        puts " There are $n concatenated transforms"
         }
         for {set  i  0}  {$i < $n} {incr i } {
         set t [$gt GetConcatenatedTransform $i]
         set int_H [$t IsA vtkHomogeneousTransform]
         set int_G [$t IsA vtkGridTransform]
         if { ($int_H != 0) } {
         WriteHomogeneous $t $i  $fileid
         } 
         if { ($int_G != 0) } {
         WriteGrid $t $i $fileid
         }
     }
    }
    if {$flag == 1} {
    WriteIntensityTransform $it  $fileid
    }
    close $fileid
}


#-------------------------------------------------------------------------------
# .PROC RunAG
#   Run the Registration.
#
# .END
#-------------------------------------------------------------------------------
proc RunAG {} {

  global AG Volume Gui

  set intesity_transform_object 0

  puts "RunAG 1: Check Error"

  if {[AGCheckErrors] == 1} {
      return
  }

  puts "RunAG 2: Prepare Result Volume"

  if {[AGPrepareResultVolume] == 1} {
      return
  }

  catch "Target Delete"
  catch "Source Delete"

  vtkImageData Target
  vtkImageData Source

#If source and target have two channels, combine them into one vtkImageData object 
  if { ($AG(InputVolSource2) == $Volume(idNone)) || ($AG(InputVolTarget2) == $Volume(idNone)) }  {
      Target DeepCopy  [ Volume($AG(InputVolTarget),vol) GetOutput]
      Source DeepCopy   [ Volume($AG(InputVolSource),vol) GetOutput]
  } else {
      catch "Target1 Delete"
      vtkImageData Target1 
      Target1 DeepCopy   [ Volume($AG(InputVolTarget),vol) GetOutput]
      catch "Source1 Delete"
      vtkImageData Source1
      Source1 DeepCopy   [ Volume($AG(InputVolSource),vol) GetOutput]
      catch "Target2 Delete"
      vtkImageData Target2 
      Target2 DeepCopy   [ Volume($AG(InputVolTarget2),vol) GetOutput]
      catch "Source2 Delete"
      vtkImageData Source2
      Source2 DeepCopy   [ Volume($AG(InputVolSource2),vol) GetOutput]

      set dims_arr1 [Source1  GetDimensions]     
      set dims_arr2 [Source2  GetDimensions]

      set dim1_0 [lindex $dims_arr1 0] 
      set dim1_1 [lindex $dims_arr1 1] 
      set dim1_2 [lindex $dims_arr1 2] 
    
      set dim2_0 [lindex $dims_arr2 0] 
      set dim2_1 [lindex $dims_arr2 1] 
      set dim2_2 [lindex $dims_arr2 2] 
 
 
      if {($dim1_0 != $dim2_0) || ($dim1_1 != $dim2_1) || ($dim1_2 != $dim2_2) } {
      DevErrorWindow "Two Source channels have different dimensions"
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
      Source Delete
      Target Delete
      
      return
      } 

    

      set dims_arr1 [Target1  GetDimensions]     
      set dims_arr2 [Target2  GetDimensions]

      set dim1_0 [lindex $dims_arr1 0] 
      set dim1_1 [lindex $dims_arr1 1] 
      set dim1_2 [lindex $dims_arr1 2] 
    
      set dim2_0 [lindex $dims_arr2 0] 
      set dim2_1 [lindex $dims_arr2 1] 
      set dim2_2 [lindex $dims_arr2 2] 
 
 
      if {($dim1_0 != $dim2_0) || ($dim1_1 != $dim2_1) || ($dim1_2 != $dim2_2) } {
      DevErrorWindow "Two Target channels have different dimensions"
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
      Source Delete
      Target Delete
      
      return
      } 
    
      catch "combineS  Delete"
      vtkImageAppendComponents combineS     
      combineS SetInput 0 Source1       
      combineS SetInput 1 Source2
      combineS Update
      Source DeepCopy [combineS GetOutput]
      Source Update 
      combineS  Delete 
      # ReleaseDataFlagOff
 
      catch "combineT  Delete"
      vtkImageAppendComponents combineT
      combineT SetInput  0 Target1 
      combineT SetInput  1 Target2
      combineT Update
      Target  DeepCopy [combineT GetOutput]
      Target Update
      combineT Delete 
      #ReleaseDataFlagOff
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
     #set $AG(Nb_of_functions) 2 ??
  }
 

 #set sourceType  [Source  GetDataObjectType]
  #puts "Source object type is $sourceType"
  #set sourcePointNum  [Source  GetNumberOfPoints]
  #puts "Source object has   $sourcePointNum points"
  #set sourceCellNum  [Source  GetNumberOfCells]
  #puts "Source object has   $sourceCellNum cells"

  set extent_arr [Source  GetExtent]
  #parray extent_arr
  #puts " Source, extent:$extent_arr"
  
  set spacing [Source GetSpacing]
  #puts " Source, spacing is  $spacing"
  
  #set origin [Source GetOrigin]
  #puts " Source, spacing is  $origin"
  #set scalarSize [Source GetScalarSize]
  #puts " Source, scalar size is  $scalarSize"
  #set scalarType [Source GetScalarType]
  #puts " Source, scalar type is  $scalarType"

 
  if { ([lindex $extent_arr 1] < 0) || ([lindex $extent_arr 3] < 0) || ([lindex $extent_arr 5] < 0)   } {
      DevErrorWindow "Source is not correct or empty"

      Source Delete
      Target Delete
      
      return 
  }


 
  #set targetType  [Target  GetDataObjectType]
  #puts "Targert object type is $targetType"
  #set targetPointNum  [Target  GetNumberOfPoints]
  #puts "Targert object has   $targetPointNum points"
  #set targetCellNum  [Target  GetNumberOfCells]
  #puts "Targert object has   $targetCellNum cells"

  set extent_arr [Target  GetExtent]
  #parray extent_arr
  #puts " Target, extent:$extent_arr"

  if { ([lindex $extent_arr 1] < 0) || ([lindex $extent_arr 3] < 0) || ([lindex $extent_arr 5] < 0)   } {
      DevErrorWindow "Target is not correct or empty"

      Target Delete
      Source Delete

      return 
  }


  AGPreprocess Source Target $AG(InputVolSource)  $AG(InputVolTarget) 



  #AG(TestReadingWriting)
  if {$AG(TestReadingWriting) == 1} {
  #  if {$AG(Debug) == 1} {}

      AGWritevtkImageData Source "TestSource.vtk"
      AGWritevtkImageData Target "TestTarget.vtk"

      puts "finish writing vtkdata"

      Source Delete
      Target Delete 
  
      vtkImageData Source
      vtkImageData Target 
  
      AGReadvtkImageData  Source "TestSource.vtk"
      AGReadvtkImageData  Target "TestTarget.vtk"
      puts "finish reading vtkdata"

      if {$AG(Debug) == 1} {
      puts " Debug information \n\n" 
      set targetType  [Target  GetDataObjectType]
      puts "Targert object type is $targetType"
      set targetPointNum  [Target  GetNumberOfPoints]
      puts "Targert object has   $targetPointNum points"
      set targetCellNum  [Target  GetNumberOfCells]
      puts "Targert object has   $targetCellNum cells"

      set extent_arr [Target  GetExtent]
      #parray extent_arr
      puts " Target, extent:$extent_arr"


      set spacing [Target GetSpacing]
      puts " Target, spacing is  $spacing"
      
      set origin [Target GetOrigin]
      puts " Target, spacing is  $origin"
      
      set scalarSize [Target GetScalarSize]
      puts " Target, scalar size is  $scalarSize"
      set scalarType [Target GetScalarType]
      puts " Target, scalar type is  $scalarType"
      set ScalarComponents [Target GetNumberOfScalarComponents]
      puts " Target, $ScalarComponents scalar components."


      set sourceType  [Source  GetDataObjectType]
      puts "Source object type is $sourceType"
      set sourcePointNum  [Source  GetNumberOfPoints]
      puts "Source object has   $sourcePointNum points"
      set sourceCellNum  [Source  GetNumberOfCells]
      puts "Source object has   $sourceCellNum cells"

      set extent_arr [Source  GetExtent]
      #parray extent_arr
      puts " Source, extent:$extent_arr"
  
      set spacing [Source GetSpacing]
      puts " Source, spacing is  $spacing"
      
      set origin [Source GetOrigin]
      puts " Source, spacing is  $origin"
      set origin [Source GetOrigin]
      puts " Source, spacing is  $origin"
      set scalarSize [Source GetScalarSize]
      puts " Source, scalar size is  $scalarSize"
      set scalarType [Source GetScalarType]
      puts " Source, scalar type is  $scalarType"
      set ScalarComponents [Source GetNumberOfScalarComponents]
      puts " Source,  $ScalarComponents  scalar components."


      }
  }



  #Source DebugOn
  #Target DebugOn
  
  if {$AG(Scale) > 0 } {
       AGTransformScale Source Target 
  }


  #vtkIntensityTransform IntensityTransform 

  catch "TransformAG Delete"
  catch {$AG(Transform) Delete}
  
  vtkGeneralTransform TransformAG

 
  if {$AG(Initial_tfm)} {      
      vtkGeneralTransformReader Reader
      Reader SetFileName $AG(Initial_tfm)
      Set TransformAG [Reader GetGeneralTransform]
      TransformAG PostMultiply 
   # How to use this intensity tranform, since it will be overwritten by the AGIntensity transform.
      Set IntensityTransform [Reader GetIntensityTransform]
      set  AG(Inentisy_transform) 1
  } else {
      TransformAG PostMultiply 
      set  AG(Inentisy_transform) 0
  }


  

  if {$AG(Linear)} {

      if { [info commands __dummy_transform] == ""} {
              vtkTransform __dummy_transform
      }

      catch "GCR Delete"
      vtkImageGCR GCR
      GCR SetVerbose $AG(Verbose)
     # GCR DebugOn
      GCR SetTarget Target
      GCR SetSource Source
      GCR PostMultiply      

      # It seems that the following line will result in error, the affine matrix used in the resampling and writing is only
      # identical matrix.
      GCR SetInput  __dummy_transform  
      [GCR GetGeneralTransform] SetInput TransformAG
      
      GCR SetCriterion $AG(Gcr_criterion)
      GCR SetTransformDomain $AG(Linear_group)
      GCR SetTwoD $AG(2D)
      GCR Update     
   

      #set AffineMatrix [[[GCR GetGeneralTransform] GetConcatenatedTransform 1] GetMatrix] 

      #catch "mat_copy Delete"    
      #vtkMatrix4x4 mat_copy
      #mat_copy DeepCopy $AffineMatrix
     
      #GCR Identity   
      #GCR Concatenate mat_copy
      #TransformAG Concatenate GCR
      TransformAG Concatenate [[GCR GetGeneralTransform] GetConcatenatedTransform 1]
   
  }

  if {$AG(Warp)} {
      catch "warp Delete"
      vtkImageWarp warp
      warp SetSource Source
      warp SetTarget Target 

      if { ($AG(InputVolMask)   != $Volume(idNone)) } {
          catch "Mask Delete"
    
      vtkImageData Mask

      Mask DeepCopy  [ Volume($AG(InputVolMask),vol) GetOutput]
      warp SetMask Mask
 
      }

# Set the options for the warp

      puts "RunAG 3"

      warp SetVerbose $AG(Verbose)
      [warp GetGeneralTransform] SetInput TransformAG
      warp SetResliceTensors $AG(Tensors)  
      warp SetForceType $AG(Force)   
      warp SetMinimumIterations  $AG(Iteration_min) 
      warp SetMaximumIterations $AG(Iteration_max)  
      warp SetMinimumLevel $AG(Level_min)  
      warp SetMaximumLevel $AG(Level_max)  
      warp SetUseSSD $AG(SSD)    
      warp SetSSDEpsilon  $AG(Epsilon)    
      warp SetMinimumStandardDeviation $AG(Stddev_min) 
      warp SetMaximumStandardDeviation $AG(Stddev_max) 
 

      puts "RunAG 4"
 
      if {[AGIntensityTransform Source] == 0 } {
      warp SetIntensityTransform $AG(tfm)
      set intesity_transform_object 1
          
      }  else  {
      set intesity_transform_object 0
      }

      # This is necessary so that the data is updated correctly.
      # If the programmers forgets to call it, it looks like nothing
      # happened.

      puts "RunAG 5"
      warp Update
      TransformAG Concatenate warp
  }


  catch "Resampled Delete"
  vtkImageData Resampled

  # Do not delete   AG(Transform), otherwise, it will be wrong. ( delete the just allocated "TransformAG")
  set AG(Transform) TransformAG 
 
  AGResample Source Target Resampled

  if { ($AG(InputVolSource2) == $Volume(idNone)) || ($AG(InputVolTarget2) == $Volume(idNone)) }  {     
      Volume($AG(ResultVol),vol) SetImageData  Resampled
      Resampled SetOrigin 0 0 0
      #Reslicer ReleaseDataFlagOff
# set ImageData only change the volume, but the discription of the volume is 
#not changed ( such as dimensions, extensions, spacings in the volume node, 
#so should copy the result node from the target node).
      MainVolumesUpdate $AG(ResultVol)
      #[Reslicer GetOutput] SetOrigin 0 0 0
  } else {
      
      vtkImageExtractComponents extractImage
      extractImage SetInput Resampled
      #[Reslicer GetOutput]
      
      extractImage SetComponents 0  
      extractImage ReleaseDataFlagOff
      
      Volume($AG(ResultVol),vol) SetImageData [extractImage  GetOutput]
      
      
      vtkImageExtractComponents extractImage2
      #extractImage2 SetInput [Reslicer GetOutput]
      extractImage2 SetInput Resampled
      
      extractImage2 SetComponents 1  
      extractImage2  ReleaseDataFlagOff
      Volume($AG(ResultVol2),vol) SetImageData [extractImage2  GetOutput]
      
      MainVolumesUpdate $AG(ResultVol)
      MainVolumesUpdate $AG(ResultVol2)
      
      [extractImage GetOutput] SetOrigin 0 0 0
      [extractImage2 GetOutput] SetOrigin 0 0 0
      
      extractImage UnRegisterAllOutputs
      extractImage2 UnRegisterAllOutputs
      
      extractImage Delete
      extractImage2 Delete
      Resampled Delete
  }
  
  if {$AG(Debug)} {
      if {$AG(Warp)} {
      
      set DataType [[warp GetDisplacementGrid] GetDataObjectType]
      puts " Transform displacementGrid, data type is $DataType"
      
      set dim_arr [[warp GetDisplacementGrid] GetDimensions]
      
      puts " Transform displacementGrid, dimensions:$dim_arr"
      
      #set {extent_1 extent_2 extent_3 extent_4 extent_5 extent_6} [[warp GetDisplacementGrid] GetExtent]

      set extent_arr [[warp GetDisplacementGrid] GetExtent]

      set origin_arr [[warp GetDisplacementGrid] GetOrigin]

      puts " Transform DisplacementGrid, origin : $origin_arr"
     
      #parray extent_arr
      puts " Transform displacementGrid, extent:$extent_arr"

    
      set ScalarSize [[warp GetDisplacementGrid] GetScalarSize]
      puts " Transform displacementGrid, ScalarSize is $ScalarSize"
      
      set ScalarType [[warp GetDisplacementGrid] GetScalarTypeAsString]
      puts " Transform displacementGrid, ScalarType is $ScalarType"
      
      set ScalarComponents [[warp GetDisplacementGrid] GetNumberOfScalarComponents]
      puts " Transform displacementGrid,  $ScalarComponents  scalar components."
      
      }
  }

  # Write  Transforms

  if {$intesity_transform_object == 1} {
      WriteTransform TransformAG 1 $AG(tfm) "Test_transform.txt"
  } else {
      WriteTransform TransformAG 0 0  "Test_transform.txt"
  }

  # keep the transforms until the next round for registration.
  #if {$AG(Warp)} {
  #    warp Delete
  #}

  #if {$AG(Linear)} {
  #    GCR Delete
  #}
  
  #if {$intesity_transform_object == 1}  {
  #    $AG(tfm) Delete
  #}

  Target Delete
  Source Delete
  if { ($AG(InputVolMask)   != $Volume(idNone)) } {
      Mask Delete
  }

  puts "RunAG 6"
  

}

#-------------------------------------------------------------------------------
# .PROC AGBatchProcessResampling
# Transform all volumes(except the source and target) to  new volumes based on the target volume and the
# transform stored in AG(transform)
# .END
#-------------------------------------------------------------------------------
proc AGBatchProcessResampling  {  }  {
    global AG Volume Gui
    if {[AGCheckErrors] == 1} {
    return
    }
 
    #upvar $ArrayName LocalArray
    upvar 0 Volume NodeType

    foreach v $NodeType(idList) {
    if { ($v != $NodeType(idNone)) && ($v !=  $AG(InputVolTarget)) && ($v != $AG(InputVolSource))} {
        
        set name [Volume($v,node) GetName]
        set subname [string range $name 0 7]

     
            if { ($subname != "resample") && ($name != "None") } {
        puts "Resample volume whose name is $name..."
        AGTransformOneVolume $v $AG(InputVolTarget)
        }
    }
    }

}
#-------------------------------------------------------------------------------
# .PROC AGTransformOneVolume
#Transform one volume to a new volume based on the target volume and the
# transform stored in AG(transform)
# .END
#-------------------------------------------------------------------------------
proc AGTransformOneVolume {SourceVolume TargetVolume} {
 global AG Volume Gui
# Create a new volume based on the name of the source volume and the node descirption of the target volume

    set v1 $TargetVolume
    set v2name  [Volume($SourceVolume,node) GetName]
    set v2 [DevCreateNewCopiedVolume $v1 ""  "resample_$v2name" ]
    set node [Volume($v2,vol) GetMrmlNode]
    Mrml(dataTree) RemoveItem $node 
    set nodeBefore [Volume($v1,vol) GetMrmlNode]
    Mrml(dataTree) InsertAfterItem $nodeBefore $node
   
    #VolumesUpdateMRML
    MainUpdateMRML
    
    catch "Source Delete"
    vtkImageData Source  
    catch "Target Delete"
    vtkImageData Target
    Target DeepCopy  [ Volume($TargetVolume,vol) GetOutput]
    Source DeepCopy  [ Volume($SourceVolume,vol) GetOutput]


    catch "Resampled Delete"
    vtkImageData Resampled

    AGPreprocess Source Target $SourceVolume  $TargetVolume
    
    AGResample Source Target Resampled

    Volume($v2,vol) SetImageData  Resampled
    Resampled SetOrigin 0 0 0
    MainVolumesUpdate $v2

    Source Delete
    Target Delete

}

#-------------------------------------------------------------------------------
# .PROC  AGPreprocess
#  Check the source and target, and set the target's origin to be at the
#  center. Set source to be at the same orientation and resolution as the target
# 
# .END
#-------------------------------------------------------------------------------
proc AGPreprocess {Source Target SourceVol TargetVol} {
 global AG Volume Gui

  set spacing [$Target GetSpacing]
  #puts " Target, spacing is  $spacing"
  
  #set origin [Target GetOrigin]
  #puts " Target, spacing is  $origin"
  #set scalarSize [Target GetScalarSize]
  #puts " Target, scalar size is  $scalarSize"
  #set scalarType [Target GetScalarType]
  #puts " Target, scalar type is  $scalarType"

  set dims  [$Target GetDimensions]

# set the origin to be the center of the volume for inputing to warp.  

  set spacing_x [lindex $spacing 0]
  set spacing_y [lindex $spacing 1]
  set spacing_z [lindex $spacing 2]
  set dim_0     [lindex $dims 0]        
  set dim_1     [lindex $dims 1]      
  set dim_2     [lindex $dims 2]

  set origin_0  [expr (1-$dim_0)*$spacing_x/2.0]
  set origin_1  [expr (1-$dim_1)*$spacing_y/2.0] 
  set origin_2  [expr (1-$dim_2)*$spacing_z/2.0] 

  # Must set origin for Target before using the reslice for orientation normalization.        
  $Target  SetOrigin  $origin_0 $origin_1 $origin_2


  catch "NormalizedSource Delete"
  vtkImageData NormalizedSource

  set  SourceScanOrder [Volume($SourceVol,node) GetScanOrder]
  set  TargetScanOrder [Volume($TargetVol,node) GetScanOrder]
  AGNormalize $Source $Target NormalizedSource $SourceScanOrder $TargetScanOrder

  #Volume($AG(ResultVol),vol) SetImageData  NormalizedSource
  #return

  $Source DeepCopy NormalizedSource

  NormalizedSource Delete

 

 


  #  Target SetOrigin  -119.53125 -119.53125 -87.0
  # Target SetSpacing  0.9375 0.9375 3

  #  Source SetOrigin  -119.53125 -119.53125 -87.0
  #  Source SetSpacing  0.9375 0.9375 3
  set dims  [$Source GetDimensions]

# set the origin to be the center of the volume for inputing to warp.  

  set spacing_x [lindex $spacing 0]
  set spacing_y [lindex $spacing 1]
  set spacing_z [lindex $spacing 2]
  set dim_0     [lindex $dims 0]        
  set dim_1     [lindex $dims 1]      
  set dim_2     [lindex $dims 2]

  set origin_0  [expr (1-$dim_0)*$spacing_x/2.0]
  set origin_1  [expr (1-$dim_1)*$spacing_y/2.0] 
  set origin_2  [expr (1-$dim_2)*$spacing_z/2.0] 
       
  $Source  SetOrigin  $origin_0 $origin_1 $origin_2

  if {$AG(Debug) == 1} {
      puts " Debug information \n\n" 
      set targetType  [$Target  GetDataObjectType]
      puts "Targert object type is $targetType"
      set targetPointNum  [$Target  GetNumberOfPoints]
      puts "Targert object has   $targetPointNum points"
      set targetCellNum  [$Target  GetNumberOfCells]
      puts "Targert object has   $targetCellNum cells"

      set extent_arr [$Target  GetExtent]
      #parray extent_arr
      puts " Target, extent:$extent_arr"


      set spacing [$Target GetSpacing]
      puts " Target, spacing is  $spacing"
      
      set origin [$Target GetOrigin]
      puts " Target, spacing is  $origin"
      
      set scalarSize [$Target GetScalarSize]
      puts " Target, scalar size is  $scalarSize"
      set scalarType [$Target GetScalarType]
      puts " Target, scalar type is  $scalarType"
      

      set sourceType  [$Source  GetDataObjectType]
      puts "Source object type is $sourceType"
      set sourcePointNum  [$Source  GetNumberOfPoints]
      puts "Source object has   $sourcePointNum points"
      set sourceCellNum  [$Source  GetNumberOfCells]
      puts "Source object has   $sourceCellNum cells"

      set extent_arr [$Source  GetExtent]
      #parray extent_arr
      puts " Source, extent:$extent_arr"
  
      set spacing [$Source GetSpacing]
      puts " Source, spacing is  $spacing"
      
      set origin [$Source GetOrigin]
      puts " Source, spacing is  $origin"
      set origin [$Source GetOrigin]
      puts " Source, spacing is  $origin"
      set scalarSize [$Source GetScalarSize]
      puts " Source, scalar size is  $scalarSize"
      set scalarType [$Source GetScalarType]
      puts " Source, scalar type is  $scalarType"
  }
}

#-------------------------------------------------------------------------------
# .PROC AGResample
# .Resample a new source according to the target and the transform saved i
# AG(Transform).
#
# .END
#-------------------------------------------------------------------------------
proc AGResample {Source Target Resampled} {

  global AG Volume Gui
#Test to transform the source using the computed transform.

  set None 0
 
  set ResampleOptions(interp) 1
  set ResampleOptions(intens) 0
  set ResampleOptions(like) 1
  #set ResampleOptions $None
  set ResampleOptions(inverse) 0
  set ResampleOptions(tensors) 0
  set ResampleOptions(xspacing) $None
  set ResampleOptions(yspacing) $None
  set ResampleOptions(zspacing) $None
  set ResampleOptions(verbose) 0

  catch "Cast Delete"
  vtkImageCast Cast
  Cast SetInput $Source
  Cast SetOutputScalarType [$Source GetScalarType]
  if {$ResampleOptions(like) != $None} {
      #     Cast SetOutputScalarType [LReader.GetOutput().GetScalarType()]
      Cast SetOutputScalarType [$Target GetScalarType]
  }     else {
      Cast SetOutputScalarType [$Source GetScalarType]
  }
  
  catch "ITrans Delete"
  vtkImageTransformIntensity ITrans

  ITrans SetInput [Cast GetOutput]
  
  if  {$ResampleOptions(intens) == 1 } {
      if {$AG(Warp)} {
        ITrans SetIntensityTransform $AG(tfm)
      }
  }

  catch "Reslicer Delete"
  if {$ResampleOptions(tensors) == 0} {
     
      vtkImageReslice Reslicer
  } else {
      vtkImageResliceST Reslicer
  }

  Reslicer SetInput [ITrans GetOutput]
  Reslicer SetInterpolationMode $ResampleOptions(interp)

# Should it be this way, or inverse in the other way?     
  if {$ResampleOptions(inverse) == 1} {
      Reslicer SetResliceTransform $AG(Transform) 
  } else {
      Reslicer SetResliceTransform [$AG(Transform)  GetInverse]
  }

  #Reslicer SetInformationInput Target
  if  {$ResampleOptions(like) !=  $None} {
      Reslicer SetInformationInput $Target
  }
  if {$ResampleOptions(xspacing) != $None} {
       Reslicer SetOutputSpacing {$ResampleOptions(xspacing),$ResampleOptions(yspacing),$ResampleOptions(zspacing)}
  }

  Reslicer Update

  if {$AG(Debug) == 1} {

      set scalar_range [[Reslicer GetOutput] GetScalarRange]
      puts "Resclier's scalar range is : $scalar_range"
      

      set DataType [[Reslicer GetOutput] GetDataObjectType]
      puts " Reliscer output, data type is $DataType"

      set dim_arr [[Reslicer GetOutput] GetDimensions]

      puts " Reliscer output, dimensions:$dim_arr"
      
   
      set origin_arr [[Reslicer GetOutput] GetOrigin]

      puts " Reliscer output, origin : $origin_arr"
      
      #set {extent_1 extent_2 extent_3 extent_4 extent_5 extent_6} [[Reslicer GetOutput] GetExtent]

      set extent_arr [[Reslicer GetOutput] GetExtent]



      #parray extent_arr
      puts " Reliscer output, extent:$extent_arr"


      set spacing_arr [[Reslicer GetOutput] GetSpacing]



      #parray extent_arr
      puts " Reliscer output, spacings:$spacing_arr"
      
    
      
    
      set ScalarSize [[Reslicer GetOutput] GetScalarSize]
      puts " Reliscer output, ScalarSize is $ScalarSize"
      
      set ScalarType [[Reslicer GetOutput] GetScalarTypeAsString]
      puts " Reliscer output, ScalarType is $ScalarType"
      
      set ScalarComponents [[Reslicer GetOutput] GetNumberOfScalarComponents]
      puts " Reliscer output,  $ScalarComponents  scalar comonents."



  }
  
          
  Cast Delete
  
  $Resampled DeepCopy [Reslicer GetOutput]

  #if { ($AG(InputVolSource2) == $Volume(idNone)) || ($AG(InputVolTarget2) == $Volume(idNone)) }  {     
  #   Reslicer UnRegisterAllOutputs
  #}
  Reslicer Delete
  
  ITrans Delete
}





#-------------------------------------------------------------------------------
# .PROC AGNormalize
#   Run the Orientation Normalization.
#
# .END
#-------------------------------------------------------------------------------
proc AGNormalize { SourceImage TargetImage NormalizedSource SourceScanOrder TargetScanOrder} {

    global AG Volume Gui

    
    catch "ijkmatrix Delete"
    catch "reslice Delete"

    vtkMatrix4x4 ijkmatrix
    vtkImageReslice reslice
   
    reslice SetInterpolationModeToLinear
  
    catch "xform Delete"
    catch "changeinfo Delete"
    vtkTransform xform
    vtkImageChangeInformation changeinfo
    changeinfo CenterImageOn


    changeinfo SetInput $SourceImage
    # [Volume($AG(InputVolSource),vol) GetOutput]

    reslice SetInput [changeinfo GetOutput]

    switch  $SourceScanOrder {    
    "LR" { set axes {  0  0 -1  -1  0  0   0  1  0 } }
    "RL" { set axes {  0  0  1  -1  0  0   0  1  0 } }
    "IS" { set axes {  1  0  0   0  1  0   0  0  1 } }
    "SI" { set axes {  1  0  0   0  1  0   0  0 -1 } }
    "PA" { set axes {  1  0  0   0  0  1   0  1  0 } }
    "AP" { set axes {  1  0  0   0  0  1   0 -1  0 } }
    }


    if {$AG(Debug) == 1} {
    puts "  axes are $axes"
    }

  
    set ii 0
    for {set i 0} {$i < 3} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            # transpose for inverse - reslice transform requires it
            ijkmatrix SetElement $j $i [lindex $axes $ii]
            incr ii
        }
    }
    
    ijkmatrix SetElement 3 3 1

    # TODO - add other orientations here...
    catch "transposematrix Delete"
    vtkMatrix4x4 transposematrix
    
    switch $TargetScanOrder {

    "LR" {  
        transposematrix DeepCopy \
            0  0  -1  0 \
            -1  0   0  0 \
            0  1   0  0 \
            0  0   0  1 
    }
    "RL" {
        transposematrix DeepCopy \
            0  0  1  0 \
           -1  0  0  0 \
            0  1  0  0 \
            0  0  0  1 
    }
        
    "IS" {   transposematrix Identity }
        "SI" { 
        transposematrix  DeepCopy \
            1  0  0   0 \
            0  1  0   0 \
            0  0 -1   0 \
            0  0  0   1
    }
        "PA" {
        transposematrix  DeepCopy \
            1  0  0 0 \
            0  0  1 0 \
            0  1  0 0 \
            0  0  0 1    
    }
        "AP" {
        transposematrix  DeepCopy \
            1  0  0 0 \
            0  0  1 0 \
            0 -1  0 0 \
            0  0  0 1 
    }
    }

 

    if {$AG(Debug) == 1} {
    puts " before using the transpose matrix, ijkmatrix is:"
    for {set i 0} {$i < 4} {incr i} {    
        set element0 [ijkmatrix GetElement $i 0]
        set element1 [ijkmatrix GetElement $i 1]
        set element2 [ijkmatrix GetElement $i 2]
        set element3 [ijkmatrix GetElement $i 3]
        puts " $element0  $element1  $element2  $element3"
    }

    puts " transpose matrixis:"
    for {set i 0} {$i < 4} {incr i} {    
        set element0 [transposematrix GetElement $i 0]
        set element1 [transposematrix GetElement $i 1]
        set element2 [transposematrix GetElement $i 2]
        set element3 [transposematrix GetElement $i 3]
        puts " $element0  $element1  $element2  $element3"
    }
    }

    ijkmatrix Multiply4x4 ijkmatrix transposematrix ijkmatrix
    
    transposematrix Delete

    if {$AG(Debug) == 1} {
    puts " After using the transpose matrix, ijkmatrix is:"
    for {set i 0} {$i < 4} {incr i} {    
        set element0 [ijkmatrix GetElement $i 0]
        set element1 [ijkmatrix GetElement $i 1]
        set element2 [ijkmatrix GetElement $i 2]
        set element3 [ijkmatrix GetElement $i 3]
        puts "$element0  $element1  $element2  $element3" 
    }
    }
    

    xform SetMatrix ijkmatrix

    reslice SetResliceTransform xform 

    #reslice SetInformationInput $TargetImage
    set spacing [$SourceImage GetSpacing]
    set spa_0  [lindex $spacing 0]
    set spa_1  [lindex $spacing 1]
    set spa_2  [lindex $spacing 2]


    set outspa [xform TransformPoint $spa_0 $spa_1 $spa_2]
    
    set outspa_0 [lindex $outspa 0]
    set outspa_1 [lindex $outspa 1]
    set outspa_2 [lindex $outspa 2]
    
    set outspa_0 [expr abs($outspa_0)]
    set outspa_1 [expr abs($outspa_1)]
    set outspa_2 [expr abs($outspa_2)]

    set extent [$SourceImage  GetExtent]
    
    set ext_0 [lindex $extent 0] 
    set ext_1 [lindex $extent 1] 
    set ext_2 [lindex $extent 2] 
    set ext_3 [lindex $extent 3] 
    set ext_4 [lindex $extent 4] 
    set ext_5 [lindex $extent 5] 

    set dim_0 [expr $ext_1 -$ext_0+1]
    set dim_1 [expr $ext_3 -$ext_2+1]
    set dim_2 [expr $ext_5 -$ext_4+1]



    set outdim [xform TransformPoint $dim_0 $dim_1 $dim_2]
    
    set outdim_0 [lindex $outdim 0] 
    set outdim_1 [lindex $outdim 1] 
    set outdim_2 [lindex $outdim 2] 
    
    set outext_0 0    
    set outext_1 [expr abs($outdim_0)-1]  
    set outext_2 0    
    set outext_3 [expr abs($outdim_1)-1]  
    
    set outext_4 0    
    set outext_5 [expr abs($outdim_2)-1]  



    set spacing [$TargetImage GetSpacing]
    set outspa_0  [lindex $spacing 0]
    set outspa_1  [lindex $spacing 1]
    set outspa_2  [lindex $spacing 2]


   

    set extent [$TargetImage  GetExtent]
    
    set ext_0 [lindex $extent 0] 
    set ext_1 [lindex $extent 1] 
    set ext_2 [lindex $extent 2] 
    set ext_3 [lindex $extent 3] 
    set ext_4 [lindex $extent 4] 
    set ext_5 [lindex $extent 5] 

    set outdim_0 [expr $ext_1 -$ext_0+1]
    set outdim_1 [expr $ext_3 -$ext_2+1]
    set outdim_2 [expr $ext_5 -$ext_4+1]



   
    
    set outext_0 0    
    set outext_1 [expr abs($outdim_0)-1]  
    set outext_2 0    
    set outext_3 [expr abs($outdim_1)-1]  
    
    set outext_4 0    
    set outext_5 [expr abs($outdim_2)-1]  



    reslice SetOutputSpacing $outspa_0 $outspa_1 $outspa_2
    reslice SetOutputExtent $outext_0 $outext_1 $outext_2 $outext_3 $outext_4 $outext_5
#    [reslice GetOutput] SetUpdateExtent $outext_0 $outext_1 $outext_2 $outext_3 $outext_4 $outext_5
    if {$AG(Debug) == 1} {
        puts " out dim:  $outdim"
        puts " out spacing :  $outspa" 
    }

    reslice Update

    #Volume($AG(ResultVol),vol) SetImageData  [reslice GetOutput]

    [reslice GetOutput]  SetOrigin 0 0 0

    $NormalizedSource DeepCopy  [reslice GetOutput]
  
   
    if {$AG(Debug) == 1} {

    set scalar_range [[reslice GetOutput] GetScalarRange]
    puts "Resclier's scalar range is : $scalar_range"
      

    set DataType [[reslice GetOutput] GetDataObjectType]
    puts " Reliscer output, data type is $DataType"

    set dim_arr [[reslice GetOutput] GetDimensions]

    puts " Reliscer output, dimensions:$dim_arr"
      
   
    set origin_arr [[reslice GetOutput] GetOrigin]

    puts " Reliscer output, origin : $origin_arr"
      
    #set {extent_1 extent_2 extent_3 extent_4 extent_5 extent_6} [[reslice GetOutput] GetExtent]

    set extent_arr [[reslice GetOutput] GetExtent]



    #parray extent_arr
    puts " Reliscer output, extent:$extent_arr"
    

    set spacing_arr [[reslice GetOutput] GetSpacing]
    


    #parray extent_arr
    puts " Reliscer output, spacings:$spacing_arr"
    
    
      
    
    set ScalarSize [[reslice GetOutput] GetScalarSize]
    puts " Reliscer output, ScalarSize is $ScalarSize"
      
    set ScalarType [[reslice GetOutput] GetScalarTypeAsString]
    puts " Reliscer output, ScalarType is $ScalarType"
    
    set ScalarComponents [[reslice GetOutput] GetNumberOfScalarComponents]
    puts " Reliscer output,  $ScalarComponents  scalar comonents."

    }
    #reslice UnRegisterAllOutputs
    reslice Delete
}


#Test writing vtkImageData for the input.
proc AGTestWriting {} {

  global AG Volume Gui

  set intesity_transform_object 0

  puts "RunAG 1: Check Error"

  if {[AGCheckErrors] == 1} {
      return
  }

 
  catch "Target Delete"
  catch "Source Delete"

  vtkImageData Target
  vtkImageData Source

#If source and target have two channels, combine them into one vtkImageData object 
  if { ($AG(InputVolSource2) == $Volume(idNone)) || ($AG(InputVolTarget2) == $Volume(idNone)) }  {
      Target DeepCopy  [ Volume($AG(InputVolTarget),vol) GetOutput]
      Source DeepCopy   [ Volume($AG(InputVolSource),vol) GetOutput]
  } else {
      catch "Target1 Delete"
      vtkImageData Target1 
      Target1 DeepCopy   [ Volume($AG(InputVolTarget),vol) GetOutput]
      catch "Source1 Delete"
      vtkImageData Source1
      Source1 DeepCopy   [ Volume($AG(InputVolSource),vol) GetOutput]
      catch "Target2 Delete"
      vtkImageData Target2 
      Target2 DeepCopy   [ Volume($AG(InputVolTarget2),vol) GetOutput]
      catch "Source2 Delete"
      vtkImageData Source2
      Source2 DeepCopy   [ Volume($AG(InputVolSource2),vol) GetOutput]

      set dims_arr1 [Source1  GetDimensions]     
      set dims_arr2 [Source2  GetDimensions]

      set dim1_0 [lindex $dims_arr1 0] 
      set dim1_1 [lindex $dims_arr1 1] 
      set dim1_2 [lindex $dims_arr1 2] 
    
      set dim2_0 [lindex $dims_arr2 0] 
      set dim2_1 [lindex $dims_arr2 1] 
      set dim2_2 [lindex $dims_arr2 2] 
 
 
      if {($dim1_0 != $dim2_0) || ($dim1_1 != $dim2_1) || ($dim1_2 != $dim2_2) } {
      DevErrorWindow "Two Source channels have different dimensions"
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
      Source Delete
      Target Delete
      
      return
      } 

    

      set dims_arr1 [Target1  GetDimensions]     
      set dims_arr2 [Target2  GetDimensions]

      set dim1_0 [lindex $dims_arr1 0] 
      set dim1_1 [lindex $dims_arr1 1] 
      set dim1_2 [lindex $dims_arr1 2] 
    
      set dim2_0 [lindex $dims_arr2 0] 
      set dim2_1 [lindex $dims_arr2 1] 
      set dim2_2 [lindex $dims_arr2 2] 
 
 
      if {($dim1_0 != $dim2_0) || ($dim1_1 != $dim2_1) || ($dim1_2 != $dim2_2) } {
      DevErrorWindow "Two Target channels have different dimensions"
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
      Source Delete
      Target Delete
      
      return
      } 
    
      catch "combineS  Delete"
      vtkImageAppendComponents combineS     
      combineS SetInput 0 Source1       
      combineS SetInput 1 Source2
      combineS Update
      Source DeepCopy [combineS GetOutput]
      Source Update 
      combineS  Delete 
      # ReleaseDataFlagOff
 
      catch "combineT  Delete"
      vtkImageAppendComponents combineT
      combineT SetInput  0 Target1 
      combineT SetInput  1 Target2
      combineT Update
      Target  DeepCopy [combineT GetOutput]
      Target Update
      combineT Delete 
      #ReleaseDataFlagOff
      Source1 Delete
      Source2 Delete
      Target1 Delete
      Target2 Delete
     #set $AG(Nb_of_functions) 2 ??
  }
 

 
 
  #set targetType  [Target  GetDataObjectType]
  #puts "Targert object type is $targetType"
  #set targetPointNum  [Target  GetNumberOfPoints]
  #puts "Targert object has   $targetPointNum points"
  #set targetCellNum  [Target  GetNumberOfCells]
  #puts "Targert object has   $targetCellNum cells"

  set extent_arr [Target  GetExtent]
  #parray extent_arr
  #puts " Target, extent:$extent_arr"

  if { ([lindex $extent_arr 1] < 0) || ([lindex $extent_arr 3] < 0) || ([lindex $extent_arr 5] < 0)   } {
      DevErrorWindow "Target is not correct or empty"

      Target Delete
      Source Delete

      return 
  }

  if {$AG(Debug) == 1} {
      puts " Debug information \n\n" 
      set targetType  [Target  GetDataObjectType]
      puts "Targert object type is $targetType"
      set targetPointNum  [Target  GetNumberOfPoints]
      puts "Targert object has   $targetPointNum points"
      set targetCellNum  [Target  GetNumberOfCells]
      puts "Targert object has   $targetCellNum cells"

      set extent_arr [Target  GetExtent]
      #parray extent_arr
      puts " Target, extent:$extent_arr"


      set spacing [Target GetSpacing]
      puts " Target, spacing is  $spacing"
      
      set origin [Target GetOrigin]
      puts " Target, spacing is  $origin"
      
      set scalarSize [Target GetScalarSize]
      puts " Target, scalar size is  $scalarSize"
      set scalarType [Target GetScalarType]
      puts " Target, scalar type is  $scalarType"
      

      set sourceType  [Source  GetDataObjectType]
      puts "Source object type is $sourceType"
      set sourcePointNum  [Source  GetNumberOfPoints]
      puts "Source object has   $sourcePointNum points"
      set sourceCellNum  [Source  GetNumberOfCells]
      puts "Source object has   $sourceCellNum cells"

      set extent_arr [Source  GetExtent]
      #parray extent_arr
      puts " Source, extent:$extent_arr"
  
      set spacing [Source GetSpacing]
      puts " Source, spacing is  $spacing"
      
      set origin [Source GetOrigin]
      puts " Source, spacing is  $origin"
      set origin [Source GetOrigin]
      puts " Source, spacing is  $origin"
      set scalarSize [Source GetScalarSize]
      puts " Source, scalar size is  $scalarSize"
      set scalarType [Source GetScalarType]
      puts " Source, scalar type is  $scalarType"
      set ScalarComponents [Source GetNumberOfScalarComponents]
      puts " Source,  $ScalarComponents  scalar components."

      set dim_arr [Source GetDimensions]
      
      puts "Source, dimensions:$dim_arr"
  }

  AGWritevtkImageData Source "testwrite.vtk"

}



proc AGReadvtkImageData {image filename}  {
    

    catch "TReader Delete"

    vtkStructuredPointsReader TReader

    TReader SetFileName $filename

    #TReader.SetNumberOfScalarComponents(2)
    TReader Update

    $image DeepCopy [TReader GetOutput]

    TReader Delete
}


proc AGTestReadvtkImageData {}  {
  


    catch "SourceTest Delete"
    vtkImageData SourceTest
    
    AGReadvtkImageData  SourceTest  "testwrite.vtk"
   
    set sourceType  [SourceTest  GetDataObjectType]
      puts "Source object type is $sourceType"
      set sourcePointNum  [SourceTest  GetNumberOfPoints]
      puts "Source object has   $sourcePointNum points"
      set sourceCellNum  [SourceTest  GetNumberOfCells]
      puts "Source object has   $sourceCellNum cells"

      set extent_arr [SourceTest  GetExtent]
      #parray extent_arr
      puts " Source, extent:$extent_arr"
  
      set spacing [SourceTest GetSpacing]
      puts " Source, spacing is  $spacing"
      
      set origin [SourceTest GetOrigin]
      puts " Source, spacing is  $origin"
      set origin [SourceTest GetOrigin]
      puts " Source, spacing is  $origin"
      set scalarSize [SourceTest GetScalarSize]
      puts " Source, scalar size is  $scalarSize"
      set scalarType [SourceTest GetScalarType]
      puts " Source, scalar type is  $scalarType"
      set ScalarComponents [SourceTest GetNumberOfScalarComponents]
      puts " Source,  $ScalarComponents  scalar components."

      set dim_arr [SourceTest GetDimensions]
      
      puts "Source, dimensions:$dim_arr"

}
