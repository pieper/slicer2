#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationInit {} {
    global KullbackLeiblerRegistration Module Volume Model

    set m KullbackLeiblerRegistration

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
    #   set Module($m,procVTK) KullbackLeiblerRegistrationBuildVTK
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

#    set Module($m,procGUI) KullbackLeiblerRegistrationBuildGUI
#    set Module($m,procVTK) KullbackLeiblerRegistrationBuildVTK
#    set Module($m,procEnter) KullbackLeiblerRegistrationEnter
#    set Module($m,procExit) KullbackLeiblerRegistrationExit

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
        {$Revision: 1.2 $} {$Date: 2003/12/09 01:43:09 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set KullbackLeiblerRegistration(count) 0
    set KullbackLeiblerRegistration(Volume1) $Volume(idNone)
    set KullbackLeiblerRegistration(Model1)  $Model(idNone)
    set KullbackLeiblerRegistration(FileName)  ""

    set KullbackLeiblerRegistration(sourceId) $Volume(idNone)
    set KullbackLeiblerRegistration(targetId) $Volume(idNone)
    set KullbackLeiblerRegistration(matrixId) ""

    set KullbackLeiblerRegistration(Repeat) 1

    global Matrix KullbackLeiblerRegistration
    #set Matrix(autoFast) mi-fast.txt
    #set Matrix(autoSlow) mi-slow.txt
    set Matrix(allowAutoUndo) 0

    ## Set the default to fast registration
    KullbackLeiblerRegistrationVerySlowParam
}
#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationBuildSubGui
#
# Build the sub-gui under $f whatever frame is calling this one
#
# Example Useg: MIBuildSubGui $f.fMI
#
# .ARGS
# frame framename
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationBuildSubGui {f} {
    global Gui Matrix KullbackLeiblerRegistration

    set framename $f

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Select
    # Level
    #   Help
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

    foreach level "Help Normal Advanced" {
        eval {radiobutton $f.r$level \
            -text "$level" -command "KullbackLeiblerRegistrationSetLevel" \
            -variable KullbackLeiblerRegistration(Level) -value $level -width 10 \
            -indicatoron 0} $Gui(WRA)
        set KullbackLeiblerRegistration(r${level}) $f.r$level
        pack $f.r$level -side left -padx 0 
    }

    set KullbackLeiblerRegistration(Level) Normal

    #-------------------------------------------
    # Level frame
    #-------------------------------------------

    set f $framename.fLevel
    #
    # Swappable Frames for Normal and Advanced Screens
    #
    foreach type "Help Normal Advanced" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set KullbackLeiblerRegistration(f${type}) $f.f${type}
    }
    raise $KullbackLeiblerRegistration(fNormal)

    set fnormal   $framename.fLevel.fNormal
    set fadvanced $framename.fLevel.fAdvanced
    set fhelp     $framename.fLevel.fHelp

    #-------------------------------------------
    # Level->Help frame
    #-------------------------------------------

    set help "
    <UL>
    <LI><B>The Algorithm </B> 
    This is an automatic method of registering two images using mutual information of the two images. It is based on the methods of Wells and Viola (1996).
    <LI><B>Limitations</B>
    The algorithm has a finite capture range. Rotations of more than 30 degrees will likely not be found. Rotations of 10 degrees will likely be found. Thus, an initial alignment can be important. Anything with gantry tilt (like CT) 
will not work. Also, arbitrary cascades of transforms are not allowed. All of the transforms that affect the Reference volume must also affect the Moving volume. The Moving volume must have one additional matrix, which will be set by this method.
    <LI><B>Easiest way to begin</B>
    Select a \"Volume to Move\" and a \"Reference Volume\" and click \"Start\".
    <LI><B>Normal: Coarse</B>
    The Coarse method will generally do a good job on all images. It takes 5 to 10 minutes to run. It requires no user intervention; though, it updates regularly so that the user can stop the algorithm is she is satisfied. 
    <LI><B>Normal: Fine</B>
    The Fine method can be run after the Coarse method to fine tune the result. Again, the Fine method updates regularly so that the user can stop the algorithm if she is satified. Otherwise, it never stops.
    <LI><B>Normal: Good and Slow</B>
    This method is designed for the user to be able to walk away, and come back and find a good registration. This method sometimes yields a good result. It does not update the alignment until finished.
    <LI><B>Normal: Very Good and Very Slow</B>
    This method is designed for the user to be able to walk away, and come back and find a good registration. This method can be very slow, but it generally works very, very well. It does not update the alignment until finished.
    <LI><B>Advanced</B>
    Change these at your own risk. The input images are normalized, so that the source and target standard deviations should generally be smaller than 1. There are arguments they should be much smaller than 1, but changing them does not seem to make a big difference. The number of samples per iteration can be increased, but also does not seem to help alot. The translation scale is roughly a measure of how much to scale translations over rotations. A variety of numbers may work here. The learning rate should generally be less than 0.001, and often much smaller. The number of update iterations is generally between 100 and 2500
    <LI><B>Known Bugs</B>
    The .mi window is left open and the pipeline is left taking lots of 
    memory.
    </UL>"

    regsub -all "\n" $help { } help
    MainHelpApplyTags KullbackLeiblerRegistration $help
#    MainHelpBuildGUI  KullbackLeiblerRegistration 

    global Help
    set f  $fhelp
    frame $f.fWidget -bg $Gui(activeWorkspace)
    pack $f.fWidget -side top -padx 2 -fill both -expand true
    set tmp [HelpWidget $f.fWidget]
    MainHelpShow $tmp KullbackLeiblerRegistration

    #-------------------------------------------
    # Level->Normal frame
    #-------------------------------------------

    set f $fnormal

    frame $f.fDesc    -bg $Gui(activeWorkspace)
    frame $f.fSpeed   -bg $Gui(activeWorkspace)
    frame $f.fRepeat  -bg $Gui(activeWorkspace)
    frame $f.fRun     -bg $Gui(activeWorkspace)

    pack $f.fDesc $f.fSpeed $f.fRepeat $f.fRun -pady $Gui(pad) 

    #-------------------------------------------
    # Level->Normal->Desc frame
    #-------------------------------------------
    set f $fnormal.fDesc

    eval {label $f.l -text "\Press 'Start' to start the program \nthat performs automatic registration\nby Mutual Information.\n\Your manual registration is used\n\ as an initial pose.\ "} $Gui(WLA)
    pack $f.l -pady $Gui(pad)

    #-------------------------------------------
    # Level->Normal->Speed Frame
    #-------------------------------------------
    set f $fnormal.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lSpeed -text "Run\n Objective:"} $Gui(WLA)
    pack $f.fTitle.lSpeed -anchor w

    # the first row and second row
    frame $f.fBtns.1 -bg $Gui(inactiveWorkspace)
    frame $f.fBtns.2 -bg $Gui(inactiveWorkspace)
    frame $f.fBtns.3 -bg $Gui(inactiveWorkspace)
    pack $f.fBtns.1 $f.fBtns.2 $f.fBtns.3 -side top -fill x -anchor w

    set row 1
    foreach text "Coarse Fine {Good and Slow} {Very Good and Very Slow}" value "Coarse Fine GSlow VerySlow" \
        width "6 6 15 21" {
        eval {radiobutton $f.fBtns.$row.r$value -width $width \
        -text "$text" -value "$value" \
        -command KullbackLeiblerRegistration${value}Param \
        -variable KullbackLeiblerRegistration(Objective) \
        -indicatoron 0} $Gui(WCA) 
        pack $f.fBtns.$row.r$value -side left -padx 4 -pady 2
        if { $value == "Fine" } {incr row};
        if { $value == "GSlow" } {incr row};
    }

   set KullbackLeiblerRegistration(Objective) VerySlow

    #-------------------------------------------
    # Level->Normal->Repeat Frame
    #-------------------------------------------
    set f $fnormal.fRepeat
    
    eval {label $f.l -text "Repeat:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x

    foreach value "1 0" text "Yes No" width "4 3" {
        eval {radiobutton $f.f.r$value -width $width \
              -indicatoron 0 -text "$text" -value "$value" \
              -variable KullbackLeiblerRegistration(Repeat) } $Gui(WCA)
        pack $f.f.r$value -side left -fill x -anchor w
    }

    #-------------------------------------------
    # Level->Normal->Run frame
    #-------------------------------------------
    set f $fnormal.fRun

    eval {button $f.bRun -text "Start" -width [expr [string length "Start"]+1] \
            -command "KullbackLeiblerRegistrationAutoRun"} $Gui(WBA)

    pack $f.bRun -side left -padx $Gui(pad) -pady $Gui(pad)
    set KullbackLeiblerRegistration(b1Run) $f.bRun

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
                   {SourceShrinkFactors} \
                   {TargetShrinkFactors} \
                   {NumberOfSamples} \
                   {TranslateScale} \
                   } name \
                  { \
                   {Update Iterations} \
                   {Learning Rate} \
                   {Source Standard Deviation} \
                   {Target Standard Deviation} \
                   {Source MultiRes Reduction} \
                   {Target Multires Reduction} \
                   {Number Of Samples} \
                   {Translate Scale} \
                   } {
        set f $fadvanced.fParam
        frame $f.f$param   -bg $Gui(activeWorkspace)
        pack $f.f$param -side top -fill x -pady 2
        
        set f $f.f$param
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 10 -textvariable KullbackLeiblerRegistration($param)} $Gui(WEA)
        pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
        pack $f.e$param -side left -padx $Gui(pad) -expand 1
    }

    #-------------------------------------------
    # Level->Advanced->Run frame
    #-------------------------------------------
    set f $fadvanced.fRun

    foreach str "Run" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
            -command "KullbackLeiblerRegistrationAuto$str"} $Gui(WBA)
        set KullbackLeiblerRegistration(b$str) $f.b$str
    }
    pack $f.bRun -side left -padx $Gui(pad) -pady $Gui(pad)
    set KullbackLeiblerRegistration(b2Run) $f.bRun

}  

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationSetLevel
#
# Set the registration mechanism depending on which button the user selected in
# the Auto tab.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationSetLevel {} {
    global KullbackLeiblerRegistration

    set level $KullbackLeiblerRegistration(Level)
    raise $KullbackLeiblerRegistration(f${level})
    focus $KullbackLeiblerRegistration(f${level})
}

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationCoarseParam
#
#  These parameters should allow the user the ability to intervene
#  and decide when he/she is done.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationCoarseParam {} {
    global KullbackLeiblerRegistration

    set KullbackLeiblerRegistration(Resolution)       128
    set KullbackLeiblerRegistration(LearningRate)    3e-5
    set KullbackLeiblerRegistration(UpdateIterations) 100
    set KullbackLeiblerRegistration(NumberOfSamples)  50
    set KullbackLeiblerRegistration(TranslateScale)   320
    # If Wells, Viola, Atsumi, etal, 
    # used 2 and 4. Wells claims exact number not critical (personal communication)
    # They scaled data 0...256.
    # We scale data -1 to 1.
    # 2/256*2 = 0.015
    set KullbackLeiblerRegistration(SourceStandardDeviation) 0.4
    set KullbackLeiblerRegistration(TargetStandardDeviation) 0.4
    set KullbackLeiblerRegistration(SourceShrinkFactors)   "1 1 1"
    set KullbackLeiblerRegistration(TargetShrinkFactors)   "1 1 1"
    set KullbackLeiblerRegistration(Repeat) 1
}


#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationFineParam
#
#  These parameters should allow the user the ability to intervene
#  and decide when he/she is done.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationFineParam {} {
    global KullbackLeiblerRegistration

    set KullbackLeiblerRegistration(Resolution)       128
    set KullbackLeiblerRegistration(LearningRate)     3e-6
    set KullbackLeiblerRegistration(UpdateIterations) 100
    set KullbackLeiblerRegistration(NumberOfSamples)  50
    set KullbackLeiblerRegistration(TranslateScale)   320
    # If Wells, Viola, Atsumi, etal, 
    # used 2 and 4. Wells claims exact number not critical (personal communication)
    # They scaled data 0...256.
    # We scale data -1 to 1.
    # 2/256*2 = 0.015
    set KullbackLeiblerRegistration(SourceStandardDeviation) 0.4
    set KullbackLeiblerRegistration(TargetStandardDeviation) 0.4
    set KullbackLeiblerRegistration(SourceShrinkFactors)   "1 1 1"
    set KullbackLeiblerRegistration(TargetShrinkFactors)   "1 1 1"
    set KullbackLeiblerRegistration(Repeat) 1
}


#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationGSlowParam
#
# This should run until completion and give a good registration
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationGSlowParam {} {
    global KullbackLeiblerRegistration

    set KullbackLeiblerRegistration(Resolution)       128
    set KullbackLeiblerRegistration(UpdateIterations) "500 1000"
    set KullbackLeiblerRegistration(LearningRate)    "0.0001 0.00001"
    set KullbackLeiblerRegistration(NumberOfSamples)  50
    set KullbackLeiblerRegistration(TranslateScale)   320
    # If Wells, Viola, Atsumi, etal, 
    # used 2 and 4. Wells claims exact number not critical (personal communication)
    # They scaled data 0...256.
    # We scale data -1 to 1.
    # 2/256*2 = 0.015
    set KullbackLeiblerRegistration(SourceStandardDeviation) 0.4
    set KullbackLeiblerRegistration(TargetStandardDeviation) 0.4
    set KullbackLeiblerRegistration(SourceShrinkFactors)   "2 2 2"
    set KullbackLeiblerRegistration(TargetShrinkFactors)   "2 2 2"
    set KullbackLeiblerRegistration(Repeat) 0
}

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationGSlowParam
#
# This should run until completion and give a good registration
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationVerySlowParam {} {
    global KullbackLeiblerRegistration

    set KullbackLeiblerRegistration(Resolution)       128 
    set KullbackLeiblerRegistration(UpdateIterations) "2500 2500 2500 2500 2500"
    set KullbackLeiblerRegistration(LearningRate)    "1e-4 1e-5 5e-6 1e-6 5e-7"
    set KullbackLeiblerRegistration(NumberOfSamples)  50
    set KullbackLeiblerRegistration(TranslateScale)   320
    # If Wells, Viola, Atsumi, etal, 
    # used 2 and 4. Wells claims exact number not critical (personal communication)
    # They scaled data 0...256.
    # We scale data -1 to 1.
    # 2/256*2 = 0.015
    set KullbackLeiblerRegistration(SourceStandardDeviation) 0.4
    set KullbackLeiblerRegistration(TargetStandardDeviation) 0.4
    set KullbackLeiblerRegistration(SourceShrinkFactors)   "4 4 1"
    set KullbackLeiblerRegistration(TargetShrinkFactors)   "4 4 1"
    set KullbackLeiblerRegistration(Repeat) 0
}


#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc KullbackLeiblerRegistrationEnter {} {
    global KullbackLeiblerRegistration
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $KullbackLeiblerRegistration(eventManager)

    # clear the text box and put instructions there
    $KullbackLeiblerRegistration(textBox) delete 1.0 end
    $KullbackLeiblerRegistration(textBox) insert end "Shift-Click anywhere!\n"
}


#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationExit {} {

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
# .PROC KullbackLeiblerRegistrationAutoRun
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationAutoRun {} {
    global Matrix KullbackLeiblerRegistration

    if {[RigidIntensityRegistrationCheckSetUp] == 0} {
      return 0
    }

    if {[llength $KullbackLeiblerRegistration(LearningRate) ] != \
        [llength $KullbackLeiblerRegistration(UpdateIterations) ] } {
       DevErrorWindow "Must Have same number of levels of iterations as learning rates"
       return 0
     }

    # sourceId = ID of volume to register (source, moving)
    # targetId = ID of reference volume   (target, stationary)
    # matrixId = ID of the transform to change
    set KullbackLeiblerRegistration(sourceId) $Matrix(volume)
    set KullbackLeiblerRegistration(targetId) $Matrix(refVolume)
    set KullbackLeiblerRegistration(matrixId) $Matrix(activeID)

     KullbackLeiblerRegistrationAutoRun_Itk 
}

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationAutoRun_Itk
#
# use the vtkITK interface to the ITK MI registration routines
# - builds a new user interface panel to control the process
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc KullbackLeiblerRegistrationAutoRun_Itk { } {
    global Path env Gui Matrix Volume KullbackLeiblerRegistration

    # TODO make islicer a package
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/isregistration.tcl
#    source $env(SLICER_HOME)/Modules/vtkKullbackLeiblerRegistration/tcl/ItkToSlicerTransform.tcl

    ## if it is not already there, create it.
    set notalreadythere [catch ".mi cget -background"]
    if {$notalreadythere} {
        toplevel .mi
        wm withdraw .mi
        isregistration .mi.reg
    }
    # catch "destroy .mi"

    .mi.reg config \
        -update_procedure KullbackLeiblerRegistrationUpdateParam         \
        -stop_procedure KullbackLeiblerRegistrationStop                  \
        -source          $KullbackLeiblerRegistration(sourceId)          \
        -target          $KullbackLeiblerRegistration(targetId)          \
        -resolution      $KullbackLeiblerRegistration(Resolution)        

    puts "to see the pop-up window, type: pack .mi.reg -fill both -expand true"
  #  pack .mi.reg -fill both -expand true
    $KullbackLeiblerRegistration(b1Run) configure -command \
                                      "KullbackLeiblerRegistrationStop"
    $KullbackLeiblerRegistration(b2Run) configure -command \
                                      "KullbackLeiblerRegistrationStop"
    $KullbackLeiblerRegistration(b1Run) configure -text "Stop"
    $KullbackLeiblerRegistration(b2Run) configure -text "Stop"
    .mi.reg start
}

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationUpdateParam
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationUpdateParam {} {
    global KullbackLeiblerRegistration

    .mi.reg config \
        -update_procedure KullbackLeiblerRegistrationUpdateParam         \
        -transform       $KullbackLeiblerRegistration(matrixId)          \
        -source          $KullbackLeiblerRegistration(sourceId)          \
        -target          $KullbackLeiblerRegistration(targetId)          \
        -resolution      $KullbackLeiblerRegistration(Resolution)        \
        -iterations      $KullbackLeiblerRegistration(UpdateIterations)  \
        -samples         $KullbackLeiblerRegistration(NumberOfSamples)   \
        -learningrate    $KullbackLeiblerRegistration(LearningRate)      \
        -translatescale  $KullbackLeiblerRegistration(TranslateScale)    \
        -source_standarddev $KullbackLeiblerRegistration(SourceStandardDeviation)  \
        -target_standarddev $KullbackLeiblerRegistration(TargetStandardDeviation)  \
        -source_shrink $KullbackLeiblerRegistration(SourceShrinkFactors) \
        -target_shrink $KullbackLeiblerRegistration(TargetShrinkFactors) \
        -auto_repeat   $KullbackLeiblerRegistration(Repeat) 
}

#-------------------------------------------------------------------------------
# .PROC KullbackLeiblerRegistrationStop
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc KullbackLeiblerRegistrationStop {} {
    global KullbackLeiblerRegistration
.mi.reg stop
$KullbackLeiblerRegistration(b1Run) configure -command \
                                      "KullbackLeiblerRegistrationAutoRun"
$KullbackLeiblerRegistration(b2Run) configure -command \
                                      "KullbackLeiblerRegistrationAutoRun"
$KullbackLeiblerRegistration(b1Run) configure -text "Start"
$KullbackLeiblerRegistration(b2Run) configure -text "Start"
}




