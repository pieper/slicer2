#-------------------------------------------------------------------------------
# .PROC MIRegInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRegInit {} {
    global MIReg Module Volume Model

    set m MIReg

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
    #   set Module($m,procVTK) MIRegBuildVTK
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

#    set Module($m,procGUI) MIRegBuildGUI
#    set Module($m,procVTK) MIRegBuildVTK
#    set Module($m,procEnter) MIRegEnter
#    set Module($m,procExit) MIRegExit

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
        {$Revision: 1.1 $} {$Date: 2003/07/14 22:34:08 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set MIReg(count) 0
    set MIReg(Volume1) $Volume(idNone)
    set MIReg(Model1)  $Model(idNone)
    set MIReg(FileName)  ""

    global Matrix
    set Matrix(autoFast) mi-fast.txt
    set Matrix(autoSlow) mi-slow.txt
    set MIReg(autoSpeed)  Fast
    set Matrix(allowAutoUndo) 0
}

#-------------------------------------------------------------------------------
# .PROC MIRegBuildSubGui
#
# Build the sub-gui under $f whatever frame is calling this one
#
# Example Useg: MIBuildSubGui $f.fMI
#
# .ARGS
# frame framename
# .END
#-------------------------------------------------------------------------------
proc MIRegBuildSubGui {f} {
    global Gui Matrix 

    set framename $f
    
    frame $f.fDesc    -bg $Gui(activeWorkspace)
    frame $f.fSpeed   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fRun     -bg $Gui(activeWorkspace)

    pack $f.fDesc $f.fSpeed $f.fRun -pady $Gui(pad) \

    #-------------------------------------------
    # MI->Desc frame
    #-------------------------------------------
    set f $framename.fDesc

    eval {label $f.l -text "\Press 'Run' to start the program \nthat performs automatic registration\nby Mutual Information.\n\Your manual registration is used\n\ as an initial pose.\ "} $Gui(WLA)
    pack $f.l -pady $Gui(pad)

    #-------------------------------------------
    # Mi->Speed Frame
    #-------------------------------------------
    set f $framename.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lSpeed -text "Run Speed:"} $Gui(WLA)
    pack $f.fTitle.lSpeed

    foreach text "Fast Slow" value "Fast Slow" \
        width "6 6" {
        eval {radiobutton $f.fBtns.rSpeed$value -width $width \
            -text "$text" -value "$value" -variable MIReg(autoSpeed) \
            -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rSpeed$value -side left -padx 4 -pady 2
    }

    #-------------------------------------------
    # MI->Run frame
    #-------------------------------------------
    set f $framename.fRun

    foreach str "Run Cancel Undo" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
            -command "AlignmentsAuto$str"} $Gui(WBA)
        set Matrix(b$str) $f.b$str
    }
    pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
    set Matrix(bUndo) $f.bUndo
    $f.bUndo configure -state disabled
}

#-------------------------------------------------------------------------------
# .PROC MIRegEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc MIRegEnter {} {
    global MIReg
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $MIReg(eventManager)

    # clear the text box and put instructions there
    $MIReg(textBox) delete 1.0 end
    $MIReg(textBox) insert end "Shift-Click anywhere!\n"
}


#-------------------------------------------------------------------------------
# .PROC MIRegExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MIRegExit {} {

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
# .PROC AlignmentsAutoRun
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoRun {} {
    if { [info command vtkITKMutualInformationTransform] == "" } {
        AlignmentsAutoRun_vtk 
    } else {
        AlignmentsAutoRun_itk 
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoRun_vtk
#
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoRun_vtk {} {
    global Path env Gui Matrix Volume

    # v = ID of volume to register
    # r = ID of reference volume
    set v $Matrix(volume)
    set r $Matrix(refVolume)

    # Store which transform we're editing
    # If the user has not selected a tranform, then create a new one by default
    # and append it to the volume to register (ie. "Volume to Move")
    set t $Matrix(activeID)
    set Matrix(tAuto) $t
    if {$t == ""} {
        DataAddTransform append Volume($v,node) Volume($v,node)
    }

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
            puts "Current Pose  at res=$res is: [$currentPose Print]"
            set resDisplay $res
            AlignmentsCopyRegImages $res $r $v
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


#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoRun_vtk
#
# use the vtkITK interface to the ITK MI registration routines
# - builds a new user interface panel to control the process
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc AlignmentsAutoRun_itk {} {
    global Path env Gui Matrix Volume

    # v = ID of volume to register (moving)
    # r = ID of reference volume
    set v $Matrix(volume)
    set r $Matrix(refVolume)

    # Store which transform we're editing
    # If the user has not selected a tranform, then create a new one by default
    # and append it to the volume to register (ie. "Volume to Move")
    set t $Matrix(activeID)
    set Matrix(tAuto) $t
    if {$t == ""} {
        DataAddTransform append Volume($v,node) Volume($v,node)
    }

    # TODO make islicer a package
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/isregistration.tcl

    catch "destroy .mi"
    toplevel .mi

    isregistration .mi.reg \
        -transform $t \
        -moving [Volume($v,node) GetName] \
        -reference [Volume($r,node) GetName]

    pack .mi.reg -fill both -expand true
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoUndo
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoUndo {} {
    global Matrix

    set t $Matrix(tAuto)
    set tran [Matrix($t,node) GetTransform]
    $tran Pop

    # Disallow undo
    $Matrix(bUndo) configure -state disabled

    # Update MRML
    MainUpdateMRML
    RenderAll
}

