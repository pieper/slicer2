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
        {$Revision: 1.1.1.1 $} {$Date: 2003/10/01 21:19:51 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
 
    set AG(InputVolSource)      $Volume(idNone)
    set AG(InputVolTarget)      $Volume(idNone)
    set AG(InputVolMask)      $Volume(idNone)
    set AG(ResultVol)     $Volume(idNone)
    #Set AG(Initial_tfm)   $Transform(idNone)
    
    #set AG(Dimension)     "3"

    #General options
    
    set AG(Linear)    "1"
    set AG(Warp)      "1"
    set AG(Initial_tfm)  "0"
     #? or{}
    set AG(Verbose)  "2"
    set AG(Scale)    "-1"
    set AG(2D)        "0"
    
    #GCR options
    set AG(Linear_group)  "2"
    set AG(GCR_criterion) "1"
   




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


    DevUpdateNodeSelectButton Volume AG ResultVol  ResultVol  DevSelectNode 0 1 1
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
    The AG module contains <P>
    The input parameters are:
    <BR>
    <UL>
    <LI><B> Input source:</B>
    <LI><B> Input Target:</B> 
    <LI><B> Input Mask  :</B> "
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
    frame $f.fThreshold        -bg $Gui(activeWorkspace)
    frame $f.fIterations       -bg $Gui(activeWorkspace)
    frame $f.fNumberOfThreads  -bg $Gui(activeWorkspace)
    #frame $f.fTruncNegValues   -bg $Gui(activeWorkspace)
    frame $f.fRun              -bg $Gui(activeWorkspace)

#  $f.fDimension  
#  $f.fTruncNegValues 
        
    pack  $f.fIO \
      $f.fThreshold  \
      $f.fIterations \
      $f.fNumberOfThreads \
          $f.fRun \
      -side top -padx 0 -pady 1 -fill x
    
    #-------------------------------------------
    # Parameters->Input/Output Frame
    #-------------------------------------------
    set f $fMain.fIO
    
    # Add menus that list models and volumes
    DevAddSelectButton  AG $f InputVolTarget "Input Target Volume" Grid
    DevAddSelectButton  AG $f InputVolSource "Input Source Volume" Grid  
    DevAddSelectButton  AG $f InputVolMask "Input Mask Volume" Grid


    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
#    lappend Volume(mbActiveList) $f.mbInputVol
#    lappend Volume(mActiveList)  $f.mbInputVol.m
 
    #-------------------------------------------
    # Parameters->ResultVol Frame
    #-------------------------------------------
#    set f $fMain.fResultVol
    
    # Add menus that list models and volumes
    DevAddSelectButton  AG $f ResultVol "Result Volume" Grid

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
#    lappend Volume(mbActiveList) $f.mbResultVol
#    lappend Volume(mActiveList)  $f.mbResultVol.m

    #-------------------------------------------
    # Parameters->Dimension frame
    #-------------------------------------------
    #set f $fMain.fDimension
    

    #eval {label $f.l -text "Dimension:"\
    #      -width 16 -justify right } $Gui(WLA)
    #pack $f.l -side left -padx $Gui(pad) -pady 0

    # puts "WCA  $Gui(WCA)"
    
    #foreach value "2 3" width "5 5" {
    #eval {radiobutton $f.r$value              \
    #      -width $width                   \
    #      -text "$value"                  \
    #      -value "$value"                 \
    #      -variable AG(Dimension)   \
    #      -indicatoron 0                  \
    #      -bg $Gui(activeWorkspace)       \
    #      -fg $Gui(textDark)              \
    #      -activebackground               \
    #      $Gui(activeButton)              \
    #      -highlightthickness 0           \
    #      -bd $Gui(borderWidth)           \
    #      -selectcolor $Gui(activeButton)
    #    pack $f.r$value -side left -padx 2 -pady 2 -fill x
    #
    #}
    #}
    


    #-------------------------------------------
    # Parameters->Threshold Frame
    #-------------------------------------------
    set f $fMain.fThreshold
    
    
    eval {label $f.lThreshold -text "Threshold:"\
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eThreshold -justify right -width 6 \
          -textvariable  AG(Threshold)  } $Gui(WEA)
    grid $f.lThreshold $f.eThreshold -pady $Gui(pad) -padx $Gui(pad) -sticky e
    grid $f.eThreshold  -sticky w


    #-------------------------------------------
    # Parameters->Iterations Frame
    #-------------------------------------------
    set f $fMain.fIterations
    
    
    eval {label $f.lIterations -text "Iterations:"\
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eIterations -justify right -width 6 \
          -textvariable  AG(Iterations)  } $Gui(WEA)
    grid $f.lIterations $f.eIterations \
    -pady 2 -padx $Gui(pad) -sticky e


    #-------------------------------------------
    # Parameters->NumberOfThreads Frame
    #-------------------------------------------
    set f $fMain.fNumberOfThreads
    
    
    eval {label $f.lNumberOfThreads -text "NumberOfThreads:"\
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eNumberOfThreads -justify right -width 6 \
          -textvariable  AG(NumberOfThreads)  } $Gui(WEA)
    grid $f.lNumberOfThreads $f.eNumberOfThreads \
    -pady 2 -padx $Gui(pad) -sticky e


    #-------------------------------------------
    # Parameters->TruncNegValues Frame
    #-------------------------------------------
    #set f $fMain.fTruncNegValues
    
    
    #eval {label $f.lTruncNegValues -text "TruncNegValues:"\
          -width 16 -justify right } $Gui(WLA)
    #eval {entry $f.eTruncNegValues -justify right -width 6 \
          -textvariable  AG(TruncNegValues)  } $Gui(WEA)
    #grid $f.lTruncNegValues $f.eTruncNegValues \
    #-pady 2 -padx $Gui(pad) -sticky e


    #-------------------------------------------
    # Parameters->Run Frame
    #-------------------------------------------
    set f $fMain.fRun
    
    DevAddButton $f.bRun "Run" "RunAG"
    
    pack $f.bRun


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
        DevErrorWindow "You cannot use Volume \"None\""
        return 1
	}

    if {  ($AG(InputVolSource) == $AG(ResultVol)) || \
          ($AG(InputVolTarget) == $AG(ResultVol)) || \
          ($AG(InputVolMask)   == $AG(ResultVol))}  {
        DevErrorWindow "You cannot use one of the input Volumes as the result Volume"
        return 1

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
    global AG

    set v1 $AG(InputVolSource)
    set v2 $AG(ResultVol)

    # Do we need to Create a New Volume?
    # If so, let's do it.
    
    if {$v2 == -5 } {
        set v2 [DevCreateNewCopiedVolume $v1 ""  "AGResult" ]
        set node [Volume($v2,vol) GetMrmlNode]
        Mrml(dataTree) RemoveItem $node 
        set nodeBefore [Volume($v1,vol) GetMrmlNode]
    Mrml(dataTree) InsertAfterItem $nodeBefore $node
        MainUpdateMRML
    } else {

        # Are We Overwriting a volume?
        # If so, let's ask. If no, return.
         
        set v2name  [Volume($v2,node) GetName]
    set continue [DevOKCancel "Overwrite $v2name?"]
          
        if {$continue == "cancel"} { return 1 }
        # They say it is OK, so overwrite!
              
        Volume($v2,node) Copy Volume($v1,node)
    }

    set AG(ResultVol) $v2
    

    return 0
}
#-------------------------------------------------------------------------------
# .PROC AGIntensityTransform
# According to the options, set the intensity transformation. 
#  
# .END
#------------------------------------------------------------------------------
proc  AGIntensityTransform {} {

   global AG Volume

    if {($AG(Intensity_tfm) == "mono-functional")} {
      vtkLTSPolynomialIT tfm
      tfm SetDegree $AG(Degree)
      tfm SetRatio $AG(Ratio)
      tfm SetNumberOfFunctions $AG(Nb_of_functions)
	if {($AG(Use_bias) == 1)} {
         tfm UseBiasOn
      }
      set AG(tfm) tfm
      return 0
  } else {
        if { $AG(Intensity_tfm) == "piecewise-median"} {
	   vtkPWMedianIT tfm
	   if {[llength $AG(Nb_of_pieces)] == 0) && ($AG(Boundaries) == 0)} {
         Volume($AG(InputVolSource),vol)  Update
         
	       set low_high [Volume($AG(InputVolSource),vol)  GetScalarRange]
               set low [lindex $low_high 0]
               set high [lindex $low_high 1]
	       for {set index 0} {$index <$AG(Nb_of_functions)}{incr index} {
		   lappend AG(Nb_of_pieces) [expr $high-$low+1]
	       }
         
	       for {set index2 $low+1} {$index2 < $hight+1}{incr index2}{
		   lappend AG(Boundaries) $index2
	       }
	   }
       
         
	   set nf $AG(Nb_of_functions)
	   set np $AG(Nb_of_pieces)
	   set bounds $AG(Boundaries)
	       if {( [llength $np] == 0) || ( [llength $np] != $nf)} {
		   ;#raise Exception
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
	  else  if { AG(Intensity_tfm) == "none" } {
      #set tfm None
	      return 1
	  }
	  else {
	      puts "unknown intensity tfm type: $AG(Intensity_tfm)"
	      #raise exception
	      #set tfm None
	      return 1
	  }
      }
  
  
   #return tfm
}

#-------------------------------------------------------------------------------
# .PROC AGIntensityTransform
# According to the options, set the intensity transformation. 
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

#-------------------------------------------------------------------------------
# .PROC RunAG
#   Run the Registration.
#
# .END
#-------------------------------------------------------------------------------
proc RunAG {} {

  global AG Volume Gui

  puts "RunAG 1: Check Error"

  if {[AGCheckErrors] == 1} {
      return
  }

  puts "RunAG 2: Prepare Result Volume"

  if {[AGPrepareResultVolume] == 1} {
      return
  }

  vtkImageData Target 
  Target DeepCopy   [ Volume($AG(InputVolTarget),vol) GetOutput]
  vtkImageData Source
  Source DeepCopy   [ Volume($AG(InputVolSource),vol) GetOutput]
 
  set targetType  [Target  GetDataObjectType]
  puts "Targert object type is $targetType"
  set targetPointNum  [Target  GetNumberOfPoints]
  puts "Targert object has   $targetPointNum points"
  set targetCellNum  [Target  GetNumberOfCells]
  puts "Targert object has   $targetCellNum cells"

  set sourceType  [Source  GetDataObjectType]
  puts "Source object type is $sourceType"
  set sourcePointNum  [Source  GetNumberOfPoints]
  puts "Source object has   $sourcePointNum points"
  set sourceCellNum  [Source  GetNumberOfCells]
  puts "Source object has   $sourceCellNum cells"

#debug for the same input
vtkImageReader SReader
SReader SetFileName "TestVolSource.vtk"
SReader SetDataExtent 0  255 0 255 1 20
SReader Update
Source  DeepCopy [SReader GetOutput]

vtkImageReader TReader
TReader SetFileName "TestVolTarget.vtk"
TReader SetDataExtent  0 255 0 255 1 20
TReader Update
Target DeepCopy [TReader GetOutput]




  #Source DebugOn
  #Target DebugOn
 # Source Delete
 # Target Delete
 # return
  #set input $AG(InputVol);
  #set res $AG(ResultVol);
  
  if {$AG(Scale) > 0 } {
       AGTransformScale Source Target 
  }


  #vtkIntensityTransform IntensityTransform 
  vtkGeneralTransform Transform

 
  if {$AG(Initial_tfm)} {      
      vtkGeneralTransformReader Reader
      Reader SetFileName $AG(Initial_tfm)
      Set Transform [Reader GetGeneralTransform]
   # How to use this intensity tranform, since it will be overwritten by the AGIntensity transform.
      Set IntensityTransform [Reader GetIntensityTransform]
      set  AG(Inentisy_transform) 1
  } else {

      Transform PostMultiply 
      set  AG(Inentisy_transform) 0
  }

  

  if {$AG(Linear)} {

      vtkImageGCR GCR
      GCR SetVerbose $AG(Verbose)
      GCR DebugOn
      #GCR SetTarget  [ Volume($AG(InputVolTarget),vol) GetOutput]
      #GCR SetSource [ Volume($AG(InputVolSource),vol) GetOutput]
      GCR SetTarget Target
      GCR SetSource Source
      [GCR GetGeneralTransform] SetInput Transform
      GCR SetCriterion $AG(GCR_criterion)
      GCR SetTransformDomain $AG(Linear_group)
      GCR SetTwoD $AG(2D)
      GCR Update

      Transform Concatenate GCR
   
  }

  if {$AG(Warp)} {
      vtkImageWarp warp
      #warp SetSource    [Volume($AG(InputVolSource),vol) GetOutput]
      #warp SetTarget    [Volume($AG(InputVolTarget),vol) GetOutput]
      warp SetSource Source
      warp SetTarget Target 

      if { ($AG(InputVolMask)   != $Volume(idNone)) } {
	    
         vtkImageData Mask

	 Mask DeepCopy  [ Volume($AG(InputVolMask),vol) GetOutput]
         warp SetMask Mask
 
      }

# Set the options for the warp

      puts "RunAG 3"

 #set warpGeneralTransform [warp GetGeneralTransform]
 #warpGeneralTransform SetInput Transform
      warp SetVerbose $AG(Verbose)
      [warp GetGeneralTransform] SetInput Transform
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
 
      #set Gui(progressText)     "executing one iteration"
      #warp SetStartMethod      MainStartProgress
      #warp SetProgressMethod  "MainShowProgress warp"
      #warp SetEndMethod        MainEndProgress

      puts "RunAG 4"

      #warp SetNumberOfThreads     $AG(NumberOfThreads)
      #warp SetTruncNegValues      $AG(TruncNegValues)
  
    
      if {[AGIntensityTransform] == 0 } {
	  warp SetIntensityTransform $AG(tfm)
      }


      # This is necessary so that the data is updated correctly.
      # If the programmers forgets to call it, it looks like nothing
      # happened.

      puts "RunAG 5"

      warp Update
      Transform Concatenate warp

#Test to transform the source using the computed transform.

      set None 0
 
      set ResampleOptions(interp) 1
      set ResampleOptions(intens) 1
      set ResampleOptions(like) $None
      set ResampleOptions(inverse) 0
      set ResampleOptions(tensors) 0
      set ResampleOptions(xspacing) $None
      set ResampleOptions(yspacing) $None
      set ResampleOptions(zspacing) $None
      set ResampleOptions(verbose) 0


      vtkImageCast Cast
      Cast SetInput Source
      Cast SetOutputScalarType [Source GetScalarType]
      if {$ResampleOptions(like) != $None} {
#     Cast SetOutputScalarType [LReader.GetOutput().GetScalarType())
      }
     else {
	 Cast SetOutputScalarType [Source GetScalarType]
     }

     vtkImageTransformIntensity ITran

      ITrans SetInput [Cast GetOutput]
     
      if  {$ResampleOptions(intens) == 1 } {
	  ITrans SetIntensityTransform [Transform GetIntensityTransform]
      }

      if {$ResampleOptions(tensors) == 0} {
	  vtkImageReslice Reslicer
      } else {
	  vtkImageResliceST Reslicer
      }

      Reslicer SetInput [ITrans GetOutput]
      Reslicer SetInterpolationMode $ResampleOptions(interp)

# Should it be this way, or inverse in the other way?     
      if {$ResampleOptions(inverse) == 1} {
	  Reslicer SetResliceTransform [Transform GetGeneralTransform]
      } else {
	  Reslicer SetResliceTransform [[TransforM GetGeneralTransform] GetInverse]
      }

      if  {$ResampleOptions(like) !=  $None} {
#	  Reslicer SetInformationInput [LReader GetOutput]
      }
      if {$ResampleOptions(xspacing) != $None} {
#	  Reslicer SetOutputSpacing {$ResampleOptions(xspacing),$ResampleOptions(yspacing),$ResampleOptions(zspacing)}
      }

      vtkImageData ResampleVolume
      ResampleVolume DeepCopy [Reslicer GetOutput]
     
      Cast Delete
      Reslicer Delete
      ITran Delete





      set DataType [[warp GetDisplacementGrid] GetDataObjectType]
      puts " Transform displacementGrid, data type is $DataType"

      set dim_arr [[warp GetDisplacementGrid] GetDimensions]

      puts " Transform displacementGrid, dimensions:$dim_arr"
      
   

      #set {extent_1 extent_2 extent_3 extent_4 extent_5 extent_6} [[warp GetDisplacementGrid] GetExtent]

      set extent_arr [[warp GetDisplacementGrid] GetExtent]



     
      #parray extent_arr
      puts " Transform displacementGrid, extent:$extent_arr"

    
      set ScalarSize [[warp GetDisplacementGrid] GetScalarSize]
      puts " Transform displacementGrid, ScalarSize is $ScalarSize"

      set ScalarType [[warp GetDisplacementGrid] GetScalarTypeAsString]
      puts " Transform displacementGrid, ScalarType is $ScalarType"
    
      set ScalarComponents [[warp GetDisplacementGrid] GetNumberOfScalarComponents]
      puts " Transform displacementGrid,  $ScalarComponents  scalar comonents."
  }
  
# Write the output of the tranform
  #vtkGeneralTransformWriter TransformWriter

  #TransformWriter SetGeneralTransform Transform
  #TransformWriter SetIntensityTransform IntensityTransform
  #TransformWriter SetFileName "test.tfm"

  #TransformWriter Write

  #Volume($res,vol) SetImageData [warp GetOutput]
  #MainVolumesUpdate $res

  if {$AG(Warp)} {
    warp Delete
  }

  if {$AG(Linear)} {
    GCR Delete
  }

  Transform Delete
  #IntensityTransform Delete

  Target Delete
  Source Delete
  if { ($AG(InputVolMask)   != $Volume(idNone)) } {
    Mask Delete
  }

  puts "RunAG 6"

  # warp UnRegisterAllOutputs
}

