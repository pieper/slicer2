#   ==================================================
#   Module : vtkLevelSets
#   Authors: Karl Krissian
#   Email  : karl@bwh.harvard.edu
#
#   This module implements a Active Contour evolution
#   for segmentation of 2D and 3D images.
#   It implements a 'codimension 2' levelsets as an
#   option for the smoothing term.
#   It comes with a Tcl/Tk interface for the '3D Slicer'.
#   ==================================================
#   Copyright (C) 2003  LMI, Laboratory of Mathematics in Imaging, 
#   Brigham and Women's Hospital, Boston MA USA
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
#   The full GNU Lesser General Public License file is in vtkLevelSets/LesserGPL_license.txt


#=auto==========================================================================
#
#===============================================================================
# FILE:        LevelSets.tcl
# PROCEDURES:  
#   LevelSetsInit
#   LevelSetsBuildGUI
#   LevelSetsBuildHelpFrame
#   LevelSetsBuildMainFrame
#   LevelSetsBuildProbFrame
#   LevelSetsBuildEquFrame
#   LevelSetsEnter
#   LevelSetsExit
#   LevelSetsUpdateGUI
#   LevelSetsCount
#   LevelSetsShowFile
#   LevelSetsBindingCallback
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
# 
#  int LevelSets(count) counts the button presses for the demo 
#  list LevelSets(eventManager)  list of event bindings used by this module
#  widget LevelSets(textBox)  the text box widget
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC LevelSetsInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LevelSetsInit {} {
    global LevelSets Module Volume 
    
    puts "LevelSetsInit begin"

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
    set m LevelSets
    set Module($m,row1List) "Help  Main Init Prob Equ"
    set Module($m,row1Name) "{Help} {Main} {Init} {Prob} {Equ}"
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
    #   set Module($m,procVTK) LevelSetsBuildVTK
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
    set Module($m,procGUI)   LevelSetsBuildGUI
    set Module($m,procEnter) LevelSetsEnter
    set Module($m,procExit)  LevelSetsExit
    set Module($m,procMRML)  LevelSetsUpdateGUI

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) "Fiducials"

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.6 $} {$Date: 2003/05/27 21:26:22 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
 
    # Initialization Paramaters
    set LevelSets(InitVol)                $Volume(idNone)
    set LevelSets(InitThreshold)          "30"

    set LevelSets(upsample_xcoeff)                    "1"
    set LevelSets(upsample_ycoeff)                    "1"
    set LevelSets(upsample_zcoeff)                    "1"

    set LevelSets(LowIThreshold)          "-1"
    set LevelSets(HighIThreshold)         "-1"


    set LevelSets(FidPointList)          0

    # Remove this part ...
    set LevelSets(NumInitPoints)          "0"
    set LevelSets(InitRadius)             "4"

    set LevelSets(MeanIntensity)              "100"
    set LevelSets(SDIntensity)                 "15"
    set LevelSets(BalloonCoeff)               "0.3"
    set LevelSets(ProbabilityThreshold)       "0.3"
    set LevelSets(ProbabilityHighThreshold)   "0"

    # Main Parameters
    set LevelSets(InputVol)               $Volume(idNone)
    set LevelSets(ResultVol)              $Volume(idNone)

    set LevelSets(Dimension)              "3"


    set LevelSets(HistoGradThreshold)     "0.2"
    set LevelSets(AttachCoeff)            "1"
    set LevelSets(StepDt)                 "0.8"

    set LevelSets(ReinitFreq)             "6"
    set LevelSets(CurvCoeff)              "0.2"

    set LevelSets(DoMean)                 "1"
    set LevelSets(DMethod)                "DISMAP_FASTMARCHING"

    set LevelSets(BandSize)               "3"
    set LevelSets(TubeSize)               "2"

    set LevelSets(NumIters)               "50"
    set LevelSets(NumberOfThreads)        "2"

    set LevelSets(Processing)             "OFF"

    # Event bindings! (see LevelSetsEnter, LevelSetsExit, tcl-shared/Events.tcl)
    set LevelSets(eventManager)  { \
        {all <KeyPress-l> {LevelSetsBindingCallback KeyPress-l %W %X %Y %x %y %t}} \
        {all <KeyPress-L> {LevelSetsBindingCallback KeyPress-L %W %X %Y %x %y %t}} \
        {all <Control-KeyPress-l> {LevelSetsBindingCallback Control-KeyPress-l %W %X %Y %x %y %t}} \
                   }
    
#        {all <Shift-1> {LevelSetsBindingCallback Shift-1 %W %X %Y %x %y %t}} \
#        {all <Shift-2> {LevelSetsBindingCallback Shift-2 %W %X %Y %x %y %t}} \
#        {all <Shift-3> {LevelSetsBindingCallback Shift-3 %W %X %Y %x %y %t}} 
    
    puts "LevelSetsInit end"

}


#-------------------------------------------------------------------------------
# .PROC LevelSetsUpdateGUI
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
proc LevelSetsUpdateGUI {} {
    global LevelSets Volume
    
    DevUpdateNodeSelectButton Volume LevelSets InputVol   InputVol   DevSelectNode
    DevUpdateNodeSelectButton Volume LevelSets InitVol    InitVol    DevSelectNode 
    DevUpdateNodeSelectButton Volume LevelSets ResultVol  ResultVol  DevSelectNode 0 1 1
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
# .PROC LevelSetsBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildGUI {} {
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Parameters" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(LevelSets,fMain)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Parameters
    #-------------------------------------------
    
    LevelSetsBuildHelpFrame
       
    LevelSetsBuildMainFrame

    LevelSetsBuildInitFrame

    LevelSetsBuildProbFrame

    LevelSetsBuildEquFrame



}

#-------------------------------------------------------------------------------
# .PROC LevelSetsBuildHelpFrame
#
#   Create the Help frame
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildHelpFrame {} {


    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The LevelSets module contains a version of 
    a Active Contour Segmentation Tool using the Level Set Method.
    <P>
    The Main parameters are:
    <BR>
    <UL>
    <LI><B> Input image:</B>
    <LI><B> 2D or 3D mode:</B> 
    <LI><B> HistoGradThreshold:  </B> Threshold on the norm of the smoothed gradient 
    <LI><B> AttachCoeff:         </B> Coefficient of the data AttachCoeff term 
    <LI><B> StepDt:              </B> Evolution Step for the PDE
    <LI><B> NumIters:            </B> Number of iterations
    <LI><B> NumberOfThreads:     </B> Number of threads
    <P>
    The Init parameters are:
    <BR>
    <LI><B> Initial Level Set:</B> To specify an initial image (label map, etc...), not available yet
    <LI><B> Threshold:        </B> Starting threshold in case the number of initial points is 0.
It should use the Initial Level Set image if there is one, or the Input image otherwise.
    <LI><B> Points:            </B> Number of initial spheres (or disks in 2D).
    <LI><B> Point1:            </B> I,J, and K coordinates + radius in voxels, Press 'l' to update it from the slices
    <LI><B> Point2:            </B> I,J, and K coordinates + radius in voxels, Press 'L' to update it from the slices
    <LI><B> Point3:            </B> I,J, and K coordinates + radius in voxels, Press 'Ctl-l' to update it from the slices
    <LI><B> Low Intensity      </B> allows preprocessing the image by putting all points lower  than L to L, -1 means inactive
    <LI><B> High Intensity     </B> allows preprocessing the image by putting all points higher than H to H, -1 means inactive
    <LI><B> Mean Intensity     </B>
    <LI><B> Standard Deviation    </B>
    <LI><B> Probability Threshold </B>
    <LI><B> Probability High Threshold </B>
    <BR>
    <P>
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags LevelSets $help
    MainHelpBuildGUI  LevelSets

}
# end LevelSetsBuildHelpFrame

#-------------------------------------------------------------------------------
# .PROC LevelSetsBuildInitFrame
#
#   Compute the initial level set
#
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildInitFrame {} {

    global Gui LevelSets Module Volume

    #-------------------------------------------
    # Init frame
    #-------------------------------------------
    set fInit $Module(LevelSets,fInit)
    set f $fInit
  
    frame $f.fInitImage            -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fInitThreshold        -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fInitRadius           -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fInitFidPoints        -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fIntensityThresholds  -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack  $f.fInitImage \
          $f.fInitThreshold \
          $f.fInitRadius \
          $f.fInitFidPoints \
      $f.fIntensityThresholds \
      -side top -padx 0 -pady 1 -fill x
    
    #-------------------------------------------
    # Parameters->InitImage
    #-------------------------------------------
    set f $fInit.fInitImage
    
    # Add menus that list models and volumes
    DevAddSelectButton  LevelSets $f InitVol "Initial Level Set" Grid

     #-------------------------------------------
    # Parameters->Threshold
    #-------------------------------------------
    set f $fInit.fInitThreshold
    

    eval {label $f.lInitThreshold -text "Threshold:"\
          -width 16 -justify right } $Gui(WLA)

    eval {entry $f.eInitThreshold -justify right -width 6 \
          -textvariable  LevelSets(InitThreshold)  } $Gui(WEA)

    grid $f.lInitThreshold $f.eInitThreshold -pady $Gui(pad) -padx $Gui(pad) -sticky e
    grid $f.eInitThreshold  -sticky w

    #-------------------------------------------
    # Parameters->Radius Frame
    #-------------------------------------------
    set f $fInit.fInitRadius

    eval {label $f.lInitRadius -text "Radius:"} $Gui(WLA)

    eval {entry $f.eInitRadius -justify right -width 4 \
          -textvariable  LevelSets(InitRadius)  } $Gui(WEA)    
        
    eval {scale $f.sInitRadius -from 1 -to 5     \
          -variable  LevelSets(InitRadius)\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

    grid $f.lInitRadius $f.eInitRadius $f.sInitRadius

    #-------------------------------------------
    # Parameters->FidPoints Frame
    #-------------------------------------------
    set f $fInit.fInitFidPoints
    FiducialsAddActiveListFrame $f 7 25 "LevelSets-seeds"

    #-------------------------------------------
    # Parameters->Intensity Thresholds
    #-------------------------------------------
    set f $fInit.fIntensityThresholds

    eval {label $f.lLIT -text "Low Intensity Thresh.:"} $Gui(WLA)
    eval {entry $f.eLIT -justify right -width 4 \
          -textvariable  LevelSets(LowIThreshold)  } $Gui(WEA)

    eval {label $f.lHIT -text "High Intensity Thresh.:"} $Gui(WLA)
    eval {entry $f.eHIT -justify right -width 4 \
          -textvariable  LevelSets(HighIThreshold)  } $Gui(WEA)
       
   grid $f.lLIT $f.eLIT -pady $Gui(pad) -padx $Gui(pad) -sticky e
   grid $f.lHIT $f.eHIT -pady $Gui(pad) -padx $Gui(pad) -sticky e

}
# end LevelSetsBuildInitFrame


#-------------------------------------------------------------------------------
# .PROC LevelSetsBuildInitFrame
#
#   Compute the initial level set
#
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildProbFrame {} {

    global Gui LevelSets Module Volume

    #-------------------------------------------
    # Intensity Probability frame
    #-------------------------------------------
    set fProb $Module(LevelSets,fProb)
    set f $fProb
  
    frame $f.fIntensityProba       -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack            $f.fIntensityProba \
      -side top -padx 0 -pady 1 -fill x
    
    #-------------------------------------------
    # Parameters->Intensity Probability Frame
    #-------------------------------------------
    set f $fProb.fIntensityProba

    eval {label $f.lMeanI -text "Mean Intensity:"} $Gui(WLA)
    eval {entry $f.eMeanI -justify right -width 4 \
          -textvariable  LevelSets(MeanIntensity)  } $Gui(WEA)

    eval {label $f.lSDI -text "Standard Deviation:"} $Gui(WLA)
    eval {entry $f.eSDI -justify right -width 4 \
          -textvariable  LevelSets(SDIntensity)  } $Gui(WEA)
       
    eval {label $f.lProTh -text "Probability Threshold:"} $Gui(WLA)
    eval {entry $f.eProTh -justify right -width 4 \
          -textvariable  LevelSets(ProbabilityThreshold)  } $Gui(WEA)
       
    eval {label $f.lProHighTh -text "Prob. High Threshold:"} $Gui(WLA)
    eval {entry $f.eProHighTh -justify right -width 4 \
          -textvariable  LevelSets(ProbabilityHighThreshold)  } $Gui(WEA)
       
   grid $f.lMeanI     $f.eMeanI     -pady $Gui(pad) -padx $Gui(pad) -sticky e
   grid $f.lSDI       $f.eSDI       -pady $Gui(pad) -padx $Gui(pad) -sticky e
   grid $f.lProTh     $f.eProTh     -pady $Gui(pad) -padx $Gui(pad) -sticky e
   grid $f.lProHighTh $f.eProHighTh -pady $Gui(pad) -padx $Gui(pad) -sticky e
}
# end LevelSetsBuildProbFrame


#-------------------------------------------------------------------------------
# .PROC LevelSetsBuildMainFrame
#
#   Create the Main frame
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildMainFrame {} {

    global Gui LevelSets Module Volume

    #-------------------------------------------
    # Main frame
    #-------------------------------------------
    set fMain $Module(LevelSets,fMain)
    set f $fMain
  
    frame $f.fProtocol            -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fIO                  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fDimension           -bg $Gui(activeWorkspace)
    frame $f.fUpSample            -bg $Gui(activeWorkspace)
    frame $f.fHistoGradThreshold  -bg $Gui(activeWorkspace)
    frame $f.fNumIters            -bg $Gui(activeWorkspace)
    frame $f.fNumberOfThreads     -bg $Gui(activeWorkspace)
    frame $f.fRun                 -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack  $f.fProtocol \
      $f.fIO \
      $f.fUpSample  \
      $f.fDimension  \
      $f.fHistoGradThreshold  \
      $f.fNumIters \
      $f.fNumberOfThreads \
          $f.fRun \
      -side top -padx 0 -pady 1 -fill x
    
    #-------------------------------------------
    # Parameters->Protocol Frame
    #-------------------------------------------
    set f $fMain.fProtocol

    eval {label $f.l -text "Protocol:"\
          -width 10 -justify left } $Gui(WLA)
    pack $f.l -side left -padx $Gui(pad) -pady 0

    DevAddButton $f.bSPGR_WM   "SPGR White Matter"     "SetSPGR_WM_Param"
    DevAddButton $f.bMRA_param "MR Angio"         "SetMRAParam"

    pack  $f.bSPGR_WM   -side top
    pack  $f.bMRA_param -side top

    #-------------------------------------------
    # Parameters->Input/Output Frame
    #-------------------------------------------
    set f $fMain.fIO
    
    # Add menus that list models and volumes
    DevAddSelectButton  LevelSets $f InputVol "Input Volume" Grid

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
#    lappend Volume(mbActiveList) $f.mbInputVol
#    lappend Volume(mActiveList)  $f.mbInputVol.m
 
    #-------------------------------------------
    # Parameters->ResultVol Frame
    #-------------------------------------------
#    set f $fMain.fResultVol
    
    # Add menus that list models and volumes
    DevAddSelectButton  LevelSets $f ResultVol "Result Volume" Grid

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
#    lappend Volume(mbActiveList) $f.mbResultVol
#    lappend Volume(mActiveList)  $f.mbResultVol.m


    #-------------------------------------------
    # Parameters->Dimension frame
    #-------------------------------------------
    set f $fMain.fDimension
    

    eval {label $f.l -text "Dimension:"\
          -width 16 -justify right } $Gui(WLA)
    pack $f.l -side left -padx $Gui(pad) -pady 0

#    puts "WCA  $Gui(WCA)"
    
    foreach value "2 3" width "5 5" {
    eval {radiobutton $f.r$value              \
          -width $width                   \
          -text "$value"                  \
          -value "$value"                 \
          -variable LevelSets(Dimension)  \
          -indicatoron 0                  \
          -bg $Gui(activeWorkspace)       \
          -fg $Gui(textDark)              \
          -activebackground               \
          $Gui(activeButton)              \
          -highlightthickness 0           \
          -bd $Gui(borderWidth)           \
          -selectcolor $Gui(activeButton)
        pack $f.r$value -side left -padx 2 -pady 2 -fill x
    
    }
    }
    

    #-------------------------------------------
    # Parameters->UpSample frame
    #-------------------------------------------
    set f $fMain.fUpSample
    

    eval {label $f.l -text "UpSample:"\
          -width 16 -justify right } $Gui(WLA)
#    pack $f.l -side left -padx $Gui(pad) -pady 0

#    puts "WCA  $Gui(WCA)"
    
    eval {entry $f.eCx -justify right -width 3 \
          -textvariable  LevelSets(upsample_xcoeff)  } $Gui(WEA)
    eval {entry $f.eCy -justify right -width 3 \
          -textvariable  LevelSets(upsample_ycoeff)  } $Gui(WEA)
    eval {entry $f.eCz -justify right -width 3 \
          -textvariable  LevelSets(upsample_zcoeff)  } $Gui(WEA)

    grid  $f.l $f.eCx $f.eCy $f.eCz  -pady $Gui(pad) -padx $Gui(pad) -sticky w
    

    #-------------------------------------------
    # Parameters->HistoGradThreshold Frame
    #-------------------------------------------
    set f $fMain.fHistoGradThreshold
    
    
    eval {label $f.lHistoGradThreshold -text "GradHistoTh:"\
          -width 12 -justify right } $Gui(WTA)
    eval {entry $f.eHistoGradThreshold -justify right -width 4 \
          -textvariable  LevelSets(HistoGradThreshold)  } $Gui(WEA)

    eval {scale $f.sHistoGradThreshold -from 0.01 -to 0.99        \
          -variable  LevelSets(HistoGradThreshold)\
          -orient vertical     \
          -resolution .01      \
          } $Gui(WSA)

    grid $f.lHistoGradThreshold $f.eHistoGradThreshold $f.sHistoGradThreshold 
#-pady $Gui(pad) -padx $Gui(pad) -sticky e
#    grid $f.eHistoGradThreshold  -sticky w


    #-------------------------------------------
    # Parameters->NumIters Frame
    #-------------------------------------------
    set f $fMain.fNumIters
    
    
    eval {label $f.lNumIters -text "Iterations:"\
          -width 11 -justify right } $Gui(WTA)

    eval {entry $f.eNumIters -justify right -width 4 \
          -textvariable  LevelSets(NumIters)  } $Gui(WEA)

    eval {scale $f.sNumIters -from 1 -to 500     \
          -variable  LevelSets(NumIters)\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

    grid $f.lNumIters $f.eNumIters $f.sNumIters
#    -pady 2 -padx $Gui(pad) -sticky e


    #-------------------------------------------
    # Parameters->NumberOfThreads Frame
    #-------------------------------------------
    set f $fMain.fNumberOfThreads
    
    
    eval {label $f.lNumberOfThreads -text "Threads:"\
          -width 8 -justify right } $Gui(WLA)

    eval {entry $f.eNumberOfThreads -justify right -width 4 \
          -textvariable  LevelSets(NumberOfThreads)  } $Gui(WEA)

    eval {scale $f.sNumberOfThreads -from 1 -to 50     \
          -variable  LevelSets(NumberOfThreads)\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

    grid $f.lNumberOfThreads $f.eNumberOfThreads $f.sNumberOfThreads
#    -pady 2 -padx $Gui(pad) -sticky 


    #-------------------------------------------
    # Parameters->Run Frame
    #-------------------------------------------
    set f $fMain.fRun

    
    DevAddButton $f.bInitRun   "Init & Run"  "RunLevelSetsBegin"
    DevAddButton $f.bReRun     "ReRun"       "ReRunLevelSets"
    DevAddButton $f.bEnd       "End"         "RunLevelSetsEnd"

    DevAddButton $f.bModel     "CreateModel" "LevelSetsCreateModel"
    
    pack $f.bInitRun $f.bReRun $f.bEnd $f.bModel


}
# end LevelSetsBuildMainFrame


#-------------------------------------------------------------------------------
# .PROC LevelSetsBuildEquFrame
#
#   Create the Equation frame
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBuildEquFrame {} {

    global Gui LevelSets Module Volume

    #-------------------------------------------
    # Equation frame
    #-------------------------------------------
    set fEqu $Module(LevelSets,fEqu)
    set f $fEqu
  
    frame $f.fEquationParam      -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fNarrowBandParam    -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack \
    $f.fEquationParam  \
    $f.fNarrowBandParam   \
    -side top -padx 0 -pady 2 -fill x
    
    #-------------------------------------------
    # Parameters->EquationParam Frame
    #-------------------------------------------
    set f $fEqu.fEquationParam

    eval {label $f.lEquationParam -text "Equation Params:"} $Gui(WLA)
        
    grid $f.lEquationParam     -pady 2 -padx $Gui(pad) -sticky e

    #--------------------------------------------------
    eval {label $f.lBalloonCoeff -text "BalloonCoeff:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eBalloonCoeff -justify right -width 6 \
          -textvariable  LevelSets(BalloonCoeff)  } $Gui(WEA)
    grid $f.lBalloonCoeff $f.eBalloonCoeff     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eBalloonCoeff  -sticky w


    #--------------------------------------------------
    eval {label $f.lAttachCoeff -text "AttachCoeff:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eAttachCoeff -justify right -width 6 \
          -textvariable  LevelSets(AttachCoeff)  } $Gui(WEA)
    grid $f.lAttachCoeff $f.eAttachCoeff     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eAttachCoeff  -sticky w


    #--------------------------------------------------
    eval {label $f.lDoMean -text "DoMean:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eDoMean -justify right -width 6 \
          -textvariable  LevelSets(DoMean)  } $Gui(WEA)
    grid $f.lDoMean $f.eDoMean -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eDoMean  -sticky w

    #--------------------------------------------------
    eval {label $f.lCurvCoeff -text "CurvCoeff:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eCurvCoeff -justify right -width 6 \
          -textvariable  LevelSets(CurvCoeff)  } $Gui(WEA)
    grid $f.lCurvCoeff $f.eCurvCoeff     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eCurvCoeff  -sticky w


    #--------------------------------------------------
    eval {label $f.lStepDt -text "StepDt:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eStepDt -justify right -width 6 \
          -textvariable  LevelSets(StepDt)  } $Gui(WEA)
    grid $f.lStepDt $f.eStepDt     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eStepDt  -sticky w



    #-------------------------------------------
    # Parameters->NarrowBandParam Frame
    #-------------------------------------------
    set f $fEqu.fNarrowBandParam

    eval {label $f.lNarrowBandParam -text "Narrow Band Params:"} $Gui(WLA)

    #--------------------------------------------------
    eval {label $f.lBandSize -text "Band Size:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eBandSize -justify right -width 6 \
          -textvariable  LevelSets(BandSize)  } $Gui(WEA)


    #--------------------------------------------------
    eval {label $f.lTubeSize -text "Tube Size:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eTubeSize -justify right -width 6 \
          -textvariable  LevelSets(TubeSize)  } $Gui(WEA)



    #--------------------------------------------------
    eval {label $f.lReinitFreq -text "Re-init. Frequency:" \
          -width 16 -justify right } $Gui(WLA)
    eval {entry $f.eReinitFreq -justify right -width 6 \
          -textvariable  LevelSets(ReinitFreq)  } $Gui(WEA)

    grid $f.lNarrowBandParam     -pady 2 -padx $Gui(pad) -sticky e


    grid $f.lBandSize  $f.eBandSize     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eBandSize  -sticky w

    grid $f.lTubeSize  $f.eTubeSize     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eTubeSize  -sticky w

    grid $f.lReinitFreq $f.eReinitFreq     -pady 0 -padx $Gui(pad) -sticky e
    grid $f.eReinitFreq  -sticky w


}
# end LevelSetsBuildEquFrame


#-------------------------------------------------------------------------------
# .PROC LevelSetsEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LevelSetsEnter {} {
    global LevelSets
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $LevelSets(eventManager)


#    puts
    if {$LevelSets(FidPointList) == 0} {
      set LevelSets(FidPointList) 1
      FiducialsCreateFiducialsList "default" "LevelSets-seeds"
    }
    FiducialsSetActiveList "LevelSets-seeds"

    # clear the text box and put instructions there
#    $LevelSets(textBox) delete 1.0 end
#    $LevelSets(textBox) insert end "Shift-Click anywhere!\n"

}

#-------------------------------------------------------------------------------
# .PROC LevelSetsExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LevelSetsExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager

    #Remove Fiducial List
#    FiducialsDeleteList "LevelSets-seeds"
}


#-------------------------------------------------------------------------------
# .PROC LevelSetsCount
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc LevelSetsCount {} {
    global LevelSets
    
    incr LevelSets(count)
    $LevelSets(lParameters) config -text "You clicked the button $LevelSets(count) times"
}


#-------------------------------------------------------------------------------
# .PROC LevelSetsShowFile
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc LevelSetsShowFile {} {
    global LevelSets
    
    $LevelSets(lfile) config -text "You entered: $LevelSets(FileName)"
}


#-------------------------------------------------------------------------------
# .PROC LevelSetsBindingCallback
# Demo of callback routine for bindings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LevelSetsBindingCallback { event W X Y x y t } {

    global LevelSets Interactor

    set slice_x $x
    set slice_y [expr 255-$y]
    set slice_nr $Interactor(s)
    
    $Interactor(activeSlicer) SetReformatPoint $slice_nr $slice_x $slice_y
    
    scan [$Interactor(activeSlicer) GetIjkPoint] "%g %g %g" xi yi zi
    
    if {[string compare $event "KeyPress-l"] == 0} {
        set LevelSets(P0x) [expr round($xi)]
        set LevelSets(P0y) [expr round($yi)]
        set LevelSets(P0z) [expr round($zi)]
    }
    if {[string compare $event "KeyPress-L"] == 0} {
        set LevelSets(P1x) [expr round($xi)]
        set LevelSets(P1y) [expr round($yi)]
        set LevelSets(P1z) [expr round($zi)]
    } 
    if {[string compare $event "Control-KeyPress-l"] == 0} {
        set LevelSets(P2x) [expr round($xi)]
        set LevelSets(P2y) [expr round($yi)]
        set LevelSets(P2z) [expr round($zi)]
    }
    if {[string compare $event "KeyPress-p"] == 0} {
        puts "Adding Fiducial..."
    }
    if {[string compare $event "KeyPress-d"] == 0} {
        puts "Removing Fiducial..."
    }

}


#-------------------------------------------------------------------------------
# .PROC LevelSetsPrepareResult
#   Create the New Volume if necessary. Otherwise, ask to overwrite.
#   returns 1 if there is are errors 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc LevelSetsCheckErrors {} {
    global LevelSets Volume

    if {  ($LevelSets(InputVol) == $Volume(idNone)) || \
          ($LevelSets(ResultVol)   == $Volume(idNone))}  {
        DevErrorWindow "You cannot use Volume \"None\""
        return 1
    }
    return 0
}


#-------------------------------------------------------------------------------
# .PROC LevelSetsPrepareResultVolume
#   Check for Errors in the setup
#   returns 1 if there are errors, 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc LevelSetsPrepareResultVolume {}  {
    global LevelSets

    set v1 $LevelSets(InputVol)
    set v2 $LevelSets(ResultVol)

    # Do we need to Create a New Volume?
    # If so, let's do it.
    
    if {$v2 == -5 } {
        set v2 [DevCreateNewCopiedVolume $v1 ""  "LevelSetsResult" ]
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

    set LevelSets(ResultVol) $v2
    

    return 0
}


#-------------------------------------------------------------------------------
# .PROC RunLevelSetsBegin
#   Run the fast marching
#
# .END
#-------------------------------------------------------------------------------
proc RunLevelSetsBegin {} {

  global LevelSets Volume Gui Slice

  puts "RunLevelSets 1"
  if {[LevelSetsPrepareResultVolume] == 1} {
      return
  }

  puts "RunLevelSets 2"
  if {[LevelSetsCheckErrors] == 1} {
      return
  }

  set input $LevelSets(InputVol);
  set res $LevelSets(ResultVol);

  if {($LevelSets(upsample_xcoeff) != "1")||($LevelSets(upsample_ycoeff) != "1")||($LevelSets(upsample_zcoeff) != "1")} {
    vtkImageResample magnify
    magnify SetDimensionality 3
    magnify SetInput [Volume($input,vol) GetOutput]

    magnify SetAxisMagnificationFactor 0 2
    magnify SetAxisMagnificationFactor 1 2
    magnify SetAxisMagnificationFactor 2 2

    magnify ReleaseDataFlagOff

    set InputImage  [magnify GetOutput]

    magnify Delete
  }  else {
    set InputImage [Volume($input,vol) GetOutput]
  }

  set LevelSets(spacing) [$InputImage GetSpacing]
  $InputImage   SetSpacing 1 1 1

  puts "RunLevelSets 3"

  #
  # ------ Level Set instanciation --------------------
  #
  vtkLevelSets LevelSets(curv)

  set LevelSets(Processing) "ON"

  #
  # ------ Debug Options ---------------------------------
  #
  LevelSets(curv) Setsavedistmap         0

  #
  # ------ Set Parameters -----------------------------
  #

  # Set the Dimension
  LevelSets(curv) SetDimension           $LevelSets(Dimension)

  # Threshold on the cumulative gradient histogram
  LevelSets(curv) SetHistoGradThreshold  $LevelSets(HistoGradThreshold)

  # Number of iterations
  LevelSets(curv) SetNumIters            $LevelSets(NumIters)

  # Scheme and Coefficient for the advection force
  LevelSets(curv) Setadvection_scheme    2
  LevelSets(curv) SetAdvectionCoeff      $LevelSets(AttachCoeff)

  # Coefficient for the curvature term
  LevelSets(curv) Setcoeff_curvature     $LevelSets(CurvCoeff)
  LevelSets(curv) SetDoMean              $LevelSets(DoMean)

  # Evolution step of the PDE
  LevelSets(curv) SetStepDt              $LevelSets(StepDt)

  if {$LevelSets(LowIThreshold) > 0} {
      LevelSets(curv) SetUseLowThreshold 1
      LevelSets(curv) SetLowThreshold $LevelSets(LowIThreshold)
  }

  if {$LevelSets(HighIThreshold) > 0} {
      LevelSets(curv) SetUseHighThreshold 1
      LevelSets(curv) SetHighThreshold $LevelSets(HighIThreshold)
  }

  #
  # ------ Set Method & Threads ------------------------
  #

  # Method 0: Liana's code, 1: Fast Marching, 2: Fast Chamfer Distance
  LevelSets(curv) SetDMmethod            2

  # Set the number of threads
  LevelSets(curv) SetEvolveThreads       $LevelSets(NumberOfThreads)

  #
  # ------ Set the expansion image ---------------------
  #

  # image between -1 and 1 in float format: evolution based on tissue statistics
  LevelSets(curv) SetNumGaussians         1
  puts "Mean intensity ="
  puts $LevelSets(MeanIntensity)
  puts "SD intensity ="
  puts $LevelSets(SDIntensity)
  LevelSets(curv) SetGaussian                 0 $LevelSets(MeanIntensity) $LevelSets(SDIntensity)
  LevelSets(curv) Setballoon_coeff            $LevelSets(BalloonCoeff)
  LevelSets(curv) SetProbabilityThreshold     $LevelSets(ProbabilityThreshold)
  LevelSets(curv) SetProbabilityHighThreshold $LevelSets(ProbabilityHighThreshold)

  #
  # ------ Set Narrow Band Size ------------------------ 
  #

  # Size of the band
  LevelSets(curv) SetBand                $LevelSets(BandSize)

  # Size of the tube (inner part of the narrow band)
  LevelSets(curv) SetTube                $LevelSets(TubeSize)
  
  # Reinitialization frequency
  LevelSets(curv) SetReinitFreq          $LevelSets(ReinitFreq)


  #
  # ------ Initialize the level set ---------------------
  #

  #
  #------- Check Fiducial list
  #
  set fidlist [FiducialsGetPointIdListFromName "LevelSets-seeds"]

  #Update numPoints module variable  
  set LevelSets(NumInitPoints) [llength $fidlist]

  if {$LevelSets(NumInitPoints) > 0} {
    LevelSets(curv) SetNumInitPoints $LevelSets(NumInitPoints)
  }

  set radius 4
  set RASToIJKMatrix [Volume($input,node) GetRasToIjk]
  for {set n 0} {$n < $LevelSets(NumInitPoints)} {incr n} {
    set coord [FiducialsGetPointCoordinates [lindex $fidlist $n]]
    set cr [lindex $coord 0]
    set ca [lindex $coord 1]
    set cs [lindex $coord 2]
    #Transform from RAS to IJK
    scan [$RASToIJKMatrix MultiplyPoint $cr $ca $cs 1] "%g %g %g %g" xi yi zi hi
    puts "LevelSets(curv) SetInitPoint  $n $xi $yi $zi $LevelSets(InitRadius)"
      LevelSets(curv) SetInitPoint  $n [expr round($xi)] [expr round($yi)] \
                                       [expr round($zi)] $LevelSets(InitRadius)
  }

  if {$LevelSets(NumInitPoints) == 0} {
#    curv SetinitImage           $init
    LevelSets(curv) SetInitThreshold       $LevelSets(InitThreshold)
  }

  #
  # ---------- Set input image and evolve ---------------
  #
#  LevelSets(curv) SetInput               $InputImage
  vtkImageData                           LevelSets(output)
  LevelSets(curv) InitParam              $InputImage LevelSets(output)

  LevelSets(curv) InitEvolution

  for {set j 0} {$j < $LevelSets(NumIters)} {incr j} {
    puts $j
    LevelSets(curv) Iterate
    update
  }


#  set Gui(progressText)     "executing one iteration"
#  curv SetStartMethod      MainStartProgress
#  curv SetProgressMethod  "MainShowProgress curv"
#  curv SetEndMethod        MainEndProgress



#  curv Update
#  Volume($res,vol) SetImageData [curv GetOutput]

  $InputImage  SetSpacing [lindex $LevelSets(spacing) 0] \
    [lindex $LevelSets(spacing) 1] \
    [lindex $LevelSets(spacing) 2]
  LevelSets(output)       SetSpacing [lindex $LevelSets(spacing) 0] \
     [lindex $LevelSets(spacing) 1] \
     [lindex $LevelSets(spacing) 2]

  vtkImageThreshold vtk_th
  vtk_th SetInput  LevelSets(output)
  vtk_th ThresholdBetween 0 100000
  vtk_th ReplaceInOn
  vtk_th ReplaceOutOn
  vtk_th SetInValue 0
  vtk_th SetOutValue 10
  vtk_th SetOutputScalarTypeToUnsignedChar

  Volume($res,vol) SetImageData [vtk_th GetOutput]

  foreach s $Slice(idList) {
    set Slice($s,labelVolID) $res
    set Slice($s,foreVolID)  0
  }

  set Slice(opacity) 0.2
  MainSlicesSetOpacityAll

  # set the window and level
  set Volume(activeID) $res
  MainVolumesSetParam Window 10
  MainVolumesSetParam Level  5
  MainVolumesRender

  MainVolumesUpdate $res

  RenderAll
  

  vtk_th   Delete


#  puts "RunLevelSets 6"

#  set OutputImage [Volume($res,vol) GetOutput]



}
#----- RunLevelSetsBegin


#-------------------------------------------------------------------------------
# .PROC ReRunLevelSets
#   Run the fast marching
#
# .END
#-------------------------------------------------------------------------------
proc ReRunLevelSets {} {

  global LevelSets Slice

  set input $LevelSets(InputVol)
  set res $LevelSets(ResultVol)

  [Volume($input,vol) GetOutput]   SetSpacing 1 1 1
  LevelSets(output)                SetSpacing 1 1 1

  for {set j 0} {$j < $LevelSets(NumIters)} {incr j} {
    puts $j
    LevelSets(curv) Iterate
    update
  }

  [Volume($input,vol) GetOutput]  SetSpacing [lindex $LevelSets(spacing) 0] \
    [lindex $LevelSets(spacing) 1] \
    [lindex $LevelSets(spacing) 2]
  LevelSets(output)       SetSpacing [lindex $LevelSets(spacing) 0] \
     [lindex $LevelSets(spacing) 1] \
     [lindex $LevelSets(spacing) 2]

# give a thresholded output 

  vtkImageThreshold vtk_th
  vtk_th SetInput  LevelSets(output)
  vtk_th ThresholdBetween 0 100000
  vtk_th ReplaceInOn
  vtk_th ReplaceOutOn
  vtk_th SetInValue 0
  vtk_th SetOutValue 10
  vtk_th SetOutputScalarTypeToUnsignedChar

  Volume($res,vol) SetImageData [vtk_th GetOutput]

  foreach s $Slice(idList) {
    set Slice($s,labelVolID) $res
    set Slice($s,foreVolID)  0
  }

  set Slice(opacity) 0.2
  MainSlicesSetOpacityAll


  # set the window and level
  set Volume(activeID) $res
  MainVolumesSetParam Window 10
  MainVolumesSetParam Level  5
  MainVolumesRender

  MainVolumesUpdate $res

  RenderAll

  vtk_th   Delete

}
#----- ReRunLevelSets


#-------------------------------------------------------------------------------
# .PROC RunLevelSetsEnd
#   Run the fast marching
#
# .END
#-------------------------------------------------------------------------------
proc RunLevelSetsEnd {} {

  global LevelSets 

  set input $LevelSets(InputVol);
  set res $LevelSets(ResultVol);

  LevelSets(curv) EndEvolution

  Volume($res,vol) SetImageData LevelSets(output)
  MainVolumesUpdate $res

  LevelSets(curv) UnRegisterAllOutputs
  LevelSets(curv) Delete
  LevelSets(output) Delete

  set LevelSets(Processing) "DONE"

#  vtkImageFlip vtk_flip
#  vtk_flip SetInput LevelSets(output)
#  vtk_flip SetFilteredAxis 1

#  vtkStructuredPointsWriter writer
#  writer SetInput    [vtk_flip GetOutput]
#  writer SetFileName /home/karl/projects/data/MRA-Florin/SPGR_LS.vtk
#  writer SetFileTypeToBinary
#  writer Write
#  writer Delete
#  vtk_flip Delete

}
#----- RunLevelSetsEnd


#-------------------------------------------------------------------------------
# .PROC LevelSetsCreateModel
#   Create a 3D model from the 0-isosurface
#
# .END
#-------------------------------------------------------------------------------
proc  LevelSetsCreateModel {} {

  global LevelSets 

  set res $LevelSets(ResultVol);


  if { $LevelSets(Processing) == "ON" } {

    vtkImageMathematics vtk_immath
    vtk_immath SetInput1 LevelSets(output)
    vtk_immath SetOperationToMultiplyByK
    vtk_immath SetConstantK -1

    Volume($res,vol) SetImageData [vtk_immath GetOutput]

    vtk_immath Delete
  }

  if { $LevelSets(Processing) != "OFF" } {
    SModelMakerCreate [Volume($res,node) GetName] "LS_Model[LevelSets(curv) Getstep]" 0 0 1
  }

}
#----- LevelSetsCreateModel


proc SetSPGR_WM_Param {} {

  global LevelSets Volume Gui

  set LevelSets(Dimension)              "3"

  set LevelSets(HistoGradThreshold)     "0.2"
  set LevelSets(AttachCoeff)            "1"
  set LevelSets(StepDt)                 "0.8"

  set LevelSets(DoMean)                 "0"
  set LevelSets(ReinitFreq)             "6"
  set LevelSets(CurvCoeff)              "0.2"
  set LevelSets(DoMean)                 "1"

  set LevelSets(BandSize)               "3"
  set LevelSets(TubeSize)               "2"

  set LevelSets(NumIters)               "50"
  
  set LevelSets(MeanIntensity)              "100"
  set LevelSets(SDIntensity)                "15"
  set LevelSets(BalloonCoeff)               "0.3"
  set LevelSets(ProbabilityThreshold)       "0.3"


  set LevelSets(NumInitPoints)          "0"
  
#  set LevelSets(InitThreshold)          "80"

  
  

  

#  curv SetNumInitPoints 1
#  curv SetInitPoint 0 128  93  78 5
  #curv SetInitPoint 1 137 102  42 5
  #curv SetInitPoint 2 170 117  80 5
  #curv SetInitPoint 3 178 119  48 5
  #curv SetInitPoint 4 139 160  62 5

  
}


#----------------------------------------------------------------------
proc SetMRAParam {} {
#    -----------

  global LevelSets Volume Gui

  
  #
  set LevelSets(Dimension)              "3"

  set LevelSets(HistoGradThreshold)     "0.4"
  set LevelSets(AttachCoeff)            "0.8"
  set LevelSets(StepDt)                 "0.8"

  set LevelSets(DoMean)                 "0"
  set LevelSets(ReinitFreq)             "6"
  set LevelSets(CurvCoeff)              "0.1"
  set LevelSets(DoMean)                 "0"

  set LevelSets(BandSize)               "3"
  set LevelSets(TubeSize)               "2"

  set LevelSets(NumIters)               "100"
  
  set LevelSets(MeanIntensity)              "80"
  set LevelSets(SDIntensity)                "45"
  set LevelSets(BalloonCoeff)               "0.8"
  set LevelSets(ProbabilityThreshold)       "0.2"
  set LevelSets(ProbabilityHighThreshold)   "100"

  set LevelSets(NumInitPoints)          "0"
  
  set LevelSets(InitThreshold)          "200"


}
