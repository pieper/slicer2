#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationInit {} {
    global MutualInformationRegistration Module Volume Model

    set m MutualInformationRegistration

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This is a module to do Mutual Information Registration"

    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Samson Timoner MIT AI Lab"

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
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
#    set Module($m,row1List) "Help Stuff"
#    set Module($m,row1Name) "{Help} {Tons o' Stuff}"
#    set Module($m,row1,tab) Stuff

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) MutualInformationRegistrationBuildVTK
    #
    #   All the options are:

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
    #
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."

#    set Module($m,procGUI) MutualInformationRegistrationBuildGUI
#    set Module($m,procVTK) MutualInformationRegistrationBuildVTK
#    set Module($m,procEnter) MutualInformationRegistrationEnter
#    set Module($m,procExit) MutualInformationRegistrationExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   

    ## Should be ITK or vtkITK, but this does not seem to work.
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1 $} {$Date: 2003/09/08 15:47:39 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set MutualInformationRegistration(count) 0
    set MutualInformationRegistration(Volume1) $Volume(idNone)
    set MutualInformationRegistration(Model1)  $Model(idNone)
    set MutualInformationRegistration(FileName)  ""

    global Matrix MutualInformationRegistration
    set Matrix(autoFast) mi-fast.txt
    set Matrix(autoSlow) mi-slow.txt
    set MutualInformationRegistration(autoSpeed)  Fast
    set Matrix(allowAutoUndo) 0

    set MutualInformationRegistration(Resolution)       128
    set MutualInformationRegistration(UpdateIterations) 100
    set MutualInformationRegistration(NumberOfSamples)  50
    set MutualInformationRegistration(LearningRate)    .0001
    set MutualInformationRegistration(TranslateScale)   320
    # If Wells, Viola, Atsumi, etal, 
    # used 2 and 4. Wells claims exact number not critical (personal communication)
    # They scaled data 0...256.
    # We scale data -1 to 1.
    # 2/256*2 = 0.015
    set MutualInformationRegistration(SourceStandardDeviation) 0.4
    set MutualInformationRegistration(TargetStandardDeviation) 0.4
}
#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationBuildSubGui
#
# Build the sub-gui under $f whatever frame is calling this one
#
# Example Useg: MIBuildSubGui $f.fMI
#
# .ARGS
# frame framename
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationBuildSubGui {f} {
    global Gui Matrix MutualInformationRegistration

    set framename $f

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Select
    # Level
    #   Normal
    #   Advanced
    # Props
    #   Top
    #     Active
    #     Type
    #   Bot
    #     Basic
    #     Advanced
    # Manual
    # Auto
    #-------------------------------------------

    # The select and Level Frames
    frame $f.fSelect -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fLevel  -bg $Gui(activeWorkspace) -height 500
    pack $f.fSelect $f.fLevel -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Select frame
    #-------------------------------------------

    set f $framename.fSelect

    foreach level "Normal Advanced" {
        eval {radiobutton $f.r$level \
            -text "$level" -command "MutualInformationRegistrationSetLevel" \
            -variable MutualInformationRegistration(Level) -value $level -width 10 \
            -indicatoron 0} $Gui(WRA)
        set MutualInformationRegistration(r${level}) $f.r$level
        pack $f.r$level -side left -padx 0 
    }

    #-------------------------------------------
    # Level frame
    #-------------------------------------------

    set f $framename.fLevel
    #
    # Swappable Frames for Normal and Advanced Screens
    #
    foreach type "Normal Advanced" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set MutualInformationRegistration(f${type}) $f.f${type}
    }
    raise $MutualInformationRegistration(fNormal)

    set fnormal   $framename.fLevel.fNormal
    set fadvanced $framename.fLevel.fAdvanced

    #-------------------------------------------
    # Level->Normal frame
    #-------------------------------------------

    set f $fnormal

    frame $f.fDesc    -bg $Gui(activeWorkspace)
    frame $f.fSpeed   -bg $Gui(activeWorkspace)
    frame $f.fRun     -bg $Gui(activeWorkspace)

    pack $f.fDesc $f.fSpeed $f.fRun -pady $Gui(pad) 

    #-------------------------------------------
    # Level->Normal->Desc frame
    #-------------------------------------------
    set f $fnormal.fDesc

    eval {label $f.l -text "\Press 'Run' to start the program \nthat performs automatic registration\nby Mutual Information.\n\Your manual registration is used\n\ as an initial pose.\ "} $Gui(WLA)
    pack $f.l -pady $Gui(pad)

    #-------------------------------------------
    # Level->Normal->Speed Frame
    #-------------------------------------------
    set f $fnormal.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lSpeed -text "Run Speed:"} $Gui(WLA)
    pack $f.fTitle.lSpeed

    foreach text "Fast Slow" value "Fast Slow" \
        width "6 6" {
        eval {radiobutton $f.fBtns.rSpeed$value -width $width \
            -text "$text" -value "$value" -variable MutualInformationRegistration(autoSpeed) \
            -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rSpeed$value -side left -padx 4 -pady 2
    }

    #-------------------------------------------
    # Level->Normal->Run frame
    #-------------------------------------------
    set f $fnormal.fRun

    foreach str "Run Cancel Undo" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
            -command "MutualInformationRegistrationAuto$str"} $Gui(WBA)
        set MutualInformationRegistration(b$str) $f.b$str
    }
    pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
    set MutualInformationRegistration(bUndo) $f.bUndo
    $f.bUndo configure -state disabled

    #-------------------------------------------
    # Level->Advanced
    #-------------------------------------------

    set f $fadvanced

    frame $f.fParam    -bg $Gui(activeWorkspace)
    frame $f.fRun      -bg $Gui(activeWorkspace)

    pack $f.fParam $f.fRun -pady $Gui(pad) 

### Variables for Gering implementation
#                   {SampleSize} \
#                   {SigmaUU} \
#                   {SigmaVV} \
#                   {SigmaV} \
#                   {Pmin} \
### Variables for Gering implementation
#                   {SampleSize}  \
#                   {SigmaUU} \
#                   {SigmaVV} \
#                   {SigmaV}  \
#                   {Pmin}  \

    foreach param { \
                   {UpdateIterations} \
                   {LearningRate} \
                   {SourceStandardDeviation} \
                   {TargetStandardDeviation} \
                   {NumberOfSamples} \
                   {TranslateScale} \
                   } name \
                  { \
                   {Update Iterations} \
                   {Learning Rate} \
                   {Source Standard Deviation} \
                   {Target Standard Deviation} \
                   {Number Of Samples} \
                   {Translate Scale} \
                   } {
        set f $fadvanced.fParam
        frame $f.f$param   -bg $Gui(activeWorkspace)
        pack $f.f$param -side top -fill x -pady 2
        
        set f $f.f$param
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 7 -textvariable MutualInformationRegistration($param)} $Gui(WEA)
        pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
        pack $f.e$param -side left -padx $Gui(pad) -expand 1
    }

    #-------------------------------------------
    # Level->Advanced->Run frame
    #-------------------------------------------
    set f $fadvanced.fRun

    foreach str "Run Cancel Undo" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
            -command "MutualInformationRegistrationAuto$str"} $Gui(WBA)
        set MutualInformationRegistration(b$str) $f.b$str
    }
    pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
    set MutualInformationRegistration(bUndo) $f.bUndo
    $f.bUndo configure -state disabled
}

#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationSetLevel
#
# Set the registration mechanism depending on which button the user selected in
# the Auto tab.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationSetLevel {} {
    global MutualInformationRegistration

    set level $MutualInformationRegistration(Level)
    raise $MutualInformationRegistration(f${level})
    focus $MutualInformationRegistration(f${level})
}


#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc MutualInformationRegistrationEnter {} {
    global MutualInformationRegistration
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $MutualInformationRegistration(eventManager)

    # clear the text box and put instructions there
    $MutualInformationRegistration(textBox) delete 1.0 end
    $MutualInformationRegistration(textBox) insert end "Shift-Click anywhere!\n"
}


#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationExit {} {

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
#        AUTO REGISTRATION
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationAutoRun
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationAutoRun {} {
    global Matrix Volume

    ###
    ### Check for Errors
    ###
    if {$Matrix(volume) == $Volume(idNone)} {
        DevWarningWindow "The Volume to Move is None! Please choose one."
        return 0
    }

    if {$Matrix(refVolume) == $Volume(idNone)} {
        DevWarningWindow "The Reference Volume is None! Please choose one."
        return 0
    }

    #
    # Store which transform we're editing
    # If the user has not selected a tranform, then create a new one by default
    # and append it to the volume to register (ie. "Volume to Move")
    #
    set Matrix(tAuto) $Matrix(activeID)
    if {$Matrix(activeID) == ""} {
        set v $Matrix(volume)
        DataAddTransform append Volume($v,node) Volume($v,node)
        MainUpdateMRML
    }

    # sourceId = ID of volume to register (source, moving)
    # targetId = ID of reference volume   (target, stationary)
    # matrixId = ID of the transform to change
    set sourceId $Matrix(volume)
    set targetId $Matrix(refVolume)
    set matrixId $Matrix(activeID)

#    MutualInformationRegistrationAutoRun_Vtk  
    if { [info command vtkITKMutualInformationTransform] == "" } {

    } else {
        MutualInformationRegistrationAutoRun_Itk $sourceId $targetId $matrixId
    }
}

#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationAutoRun_Itk
#
# use the vtkITK interface to the ITK MI registration routines
# - builds a new user interface panel to control the process
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc MutualInformationRegistrationAutoRun_Itk {sourceId targetId matrixId} {
    global Path env Gui Matrix Volume MutualInformationRegistration

    # TODO make islicer a package
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/isregistration.tcl

    catch "destroy .mi"
    toplevel .mi

    isregistration .mi.reg \
        -transform       $matrixId                           \
        -source          [Volume($sourceId,node) GetName]    \
        -target          [Volume($targetId,node) GetName]    \
        -resolution      $MutualInformationRegistration(Resolution)          \
        -iterations      $MutualInformationRegistration(UpdateIterations)    \
        -samples         $MutualInformationRegistration(NumberOfSamples)     \
        -learningrate    $MutualInformationRegistration(LearningRate)        \
        -translatescale  $MutualInformationRegistration(TranslateScale)      \
        -source_standarddev    $MutualInformationRegistration(SourceStandardDeviation)  \
        -target_standarddev    $MutualInformationRegistration(TargetStandardDeviation)  

    pack .mi.reg -fill both -expand true
}


#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationAutoUndo
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationAutoUndo {} {
    global Matrix MutualInformationRegistration

    set t $Matrix(tAuto)
    set tran [Matrix($t,node) GetTransform]
    $tran Pop

    # Disallow undo
    $MutualInformationRegistration(bUndo) configure -state disabled

    # Update MRML
    MainUpdateMRML
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationAutoRun_Vtk
#
#
# These are the tools written by Dave Gering (and implemented by Hanifa Dostmohamed)
# They are not currently used, though they should work.
# But, I'm not really sure.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationAutoRun_Vtk {} {
    global Path env Gui Matrix Volume MutualInformationRegistration

    # v = ID of volume to register
    # r = ID of reference volume
    set v $Matrix(volume)
    set r $Matrix(refVolume)

    # Store which transform we're editing
    # If the user has not selected a tranform, then create a new one by default
    # and append it to the volume to register (ie. "Volume to Move")
    set t $Matrix(activeID)

    catch "refTrans Delete"
    catch "subTrans Delete"
    catch "initMatrix Delete"
    catch "initPose Delete"
    catch "reg Delete"

    vtkRasToIjkTransform refTrans
    eval refTrans SetExtent  [[Volume($r,vol) GetOutput] GetExtent]
    eval refTrans SetSpacing [[Volume($r,vol) GetOutput] GetSpacing]
    refTrans SetSlicerMatrix [Volume($r,node) GetRasToIjk]
    refTrans ComputeCornersFromSlicerMatrix

    vtkRasToIjkTransform subTrans
    eval subTrans SetExtent  [[Volume($v,vol) GetOutput] GetExtent]
    eval subTrans SetSpacing [[Volume($v,vol) GetOutput] GetSpacing]
    subTrans SetSlicerMatrix [Volume($v,node) GetRasToIjk]
    subTrans ComputeCornersFromSlicerMatrix

    # Get the initial Pose
    # This is either identity when no manual reg has been done
    # or the matrix obtained from manual registration
    set tran   [Matrix($t,node) GetTransform]
    set matrix [$tran GetMatrix]
    vtkMatrix4x4 initMatrix
    initMatrix DeepCopy $matrix
    initMatrix Invert
    vtkPose initPose
    initPose ConvertFromMatrix4x4 initMatrix
    puts "Initial Pose = [initPose Print]"

    # Run MI Registration
    vtkImageMIReg reg
    reg SetReference [Volume($r,vol) GetOutput]
    reg SetSubject   [Volume($v,vol) GetOutput]
    reg SetRefTrans refTrans
    reg SetSubTrans subTrans
    reg SetInitialPose initPose

    # Set parameters (ordered from small res to large)
    reg SetNumIterations 16000 4000 4000 4000
    reg SetLambdaDisplacement .2 0.1 0.05 0.01
    reg SetLambdaRotation 0.00005 0.00002 0.000005 0.000001
    reg SetSampleSize 50
    reg SetSigmaUU 2
    reg SetSigmaVV 2
    reg SetSigmaV 4
    reg SetPMin 0.01
    reg SetUpdateIterations 200

    # Initialize (downsample images)
    set res -1
    set resDisplay 3
    set Gui(progressText) "MI Initializing"
    MainStartProgress
    MainShowProgress reg
    reg Update

    # Iterate
    while {[reg GetInProgress] == 1} {
        reg Update

        # Update the pose (set the transform's matrix)
        set currentPose [reg GetCurrentPose]
        $currentPose ConvertToMatrix4x4 $matrix
        $matrix Invert

        # If we're not done, then display intermediate results
        if {[reg GetInProgress] == 1} {

          # Print out the current status
          set res  [reg GetResolution]
          set iter [reg GetIteration]
          set Gui(progressText) "MI res=$res iter=$iter"
          MainShowProgress reg

          # Update the image data to display
          # Copy the new Subject if its resolution changed since last update
          if {$res != $resDisplay} {
            puts "Current Pose at res=$res is: [$currentPose Print]"
            set resDisplay $res
            MutualInformationRegistrationCopyRegImages $res $r $v
          }
        }

        # Update MRML and display
        MainUpdateMRML
        RenderAll
   }
   MainEndProgress

   # Cleanup
   refTrans Delete
   subTrans Delete
   initMatrix Delete
   initPose Delete
   reg Delete

   #Return the user back to the pick alignment mode tab
   set Matrix(regMode) ""
   raise $Matrix(fAlignBegin)
}


#
#-------------------------------------------------------------------------------
# .PROC MutualInformationRegistrationCopyRegImages
#
# Stuff for Dave Gering implementation
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MutualInformationRegistrationCopyRegImages {res r v} {
  global Volume

  #
  # Copy Subject
  #

  # Copy the downsampled ImageData
  vtkImageCopy copy
  copy SetInput [reg GetSub $res]
  copy Update
  copy SetInput ""
  Volume($v,vol) SetImageData [copy GetOutput]
  copy SetOutput ""
  copy Delete

  # Copy the RasToIjk matrix of downsampled data
  set imgMatrix [[reg GetSubRasToIjk $res] GetRasToIjk]
  puts "Subject RasToIjk = [$imgMatrix Print]"
  set n Volume($v,node)
  set str [$n GetMatrixToString $imgMatrix]
  $n SetRasToVtkMatrix $str
  $n UseRasToVtkMatrixOn

  # Update pipeline and GUI
  MainVolumesUpdate $v

  #
  # Copy Reference
  #

  # Copy the downsampled ImageData
  vtkImageCopy copy
  copy SetInput [reg GetRef $res]
  copy Update
  copy SetInput ""
  Volume($r,vol) SetImageData [copy GetOutput]
  copy SetOutput ""
  copy Delete

  # Copy the RasToIjk matrix of downsampled data
  set imgMatrix [[reg GetRefRasToIjk $res] GetRasToIjk]
  # puts "Reference RasToIjk = [$imgMatrix Print]"
  set n Volume($r,node)
  set str [$n GetMatrixToString $imgMatrix]
  $n SetRasToVtkMatrix $str
  $n UseRasToVtkMatrixOn

  # Update pipeline and GUI
  MainVolumesUpdate $r
}
