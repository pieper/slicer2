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
    set Module($m,row1List) "Help Main"
    set Module($m,row1Name) "{Help} {Main} "
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
        {$Revision: 1.1 $} {$Date: 2003/10/01 21:19:51 $}]

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
    
    set AG(Linear)    1
    set AG(Warp)      1
    set AG(Initial_tfm)  0
     #? or{}
    set AG(Verbose)  "1"
    set AG(Scale)    -1
    set AG(2D)        0
    
    #GCR options
    set AG(Linear_group)  2
    set AG(GCR_criterion) 1
   




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
    set AG(degree)   1
    set AG(Ratio)    1
    set AG(Nb_of_function)  1
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
       
    #AGBuildExpertFrame

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
    DevAddSelectButton  AG $f InputVolSource "Input Source Volume" Grid
    DevAddSelectButton  AG $f InputVolTarget "Input Target Volume" Grid
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
#proc AGBuildExpertFrame {} {
#    global Gui AG Module Volume

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
      set AG(tfm) $tfm
      return 0
  }
   else {
        if { ((AG(Intensity_tfm) == "piecewise-median")} {
	   vtkPWMedianIT tfm
	   if {[llength $AG(Nb_of_pieces)] == 0) && ($AG(Boundaries) == 0)} {
         Volume($AG(InputVolSource),vol)  Update
         
	       set {low high} [Volume($AG(InputVolSource),vol)  GetScalarRange]
       
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
	   set AG(tfm) $tfm
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
# .PROC RunAG
#   Run the fast marching
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

  #set input $AG(InputVol);
  #set res $AG(ResultVol);


  vtkImageWarp warp

  #vtkIntensityTransform IntensityTransform 
  vtkGeneralTransform Transform
  Transform PostMultiply
  


  warp SetSource               [ Volume($AG(InputVolSource),vol) GetOutput]
  warp SetTarget               [ Volume($AG(InputVolTarget),vol) GetOutput]
  if { ($AG(InputVolMask)   != $Volume(idNone)) } {
	warp SetMask   [ Volume($AG(InputVolMask),vol) GetOutput]
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
  
  #warp SetIntensityTransform IntensityTransform



  # This is necessary so that the data is updated correctly.
  # If the programmers forgets to call it, it looks like nothing
  # happened.

  puts "RunAG 5"

  warp Update
  Transform Concatenate warp

# Write the output of the tranform
  #vtkGeneralTransformWriter TransformWriter
   
  #TransformWriter SetGeneralTransform Transform
  #TransformWriter SetIntensityTransform IntensityTransform
  #TransformWriter SetFileName "test.tfm"

  #TransformWriter Write

  #Volume($res,vol) SetImageData [warp GetOutput]
  #MainVolumesUpdate $res

  puts "RunAG 6"

  # warp UnRegisterAllOutputs
  warp Delete

}

