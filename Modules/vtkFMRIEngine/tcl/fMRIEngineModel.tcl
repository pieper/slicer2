#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForSetupTab
# Creates UI for model tab 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForSetupTab {parent} {
    global fMRIEngine Gui

    frame $parent.fHelp    -bg $Gui(activeWorkspace)
    frame $parent.fMethods -bg $Gui(backdrop)
    frame $parent.fModel   -bg $Gui(activeWorkspace)
    frame $parent.fTasks   -bg $Gui(activeWorkspace) \
        -relief groove -bd 3

    pack $parent.fHelp $parent.fMethods \
        -side top -fill x -pady 2 -padx 5 
    pack $parent.fModel $parent.fTasks \
        -side top -fill x -pady 3 -padx 5 

    #-------------------------------------------
    # Help frame 
    #-------------------------------------------
#    set f $parent.fHelp
#    DevAddButton $f.bHelp "?" "fMRIEngineShowHelp" 2 
#    pack $f.bHelp -side left -padx 1 -pady 1 

    #-------------------------------------------
    # Methods frame 
    #-------------------------------------------
    set f $parent.fMethods
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupChooseDetector" 2 
    pack $f.bHelp -side left -padx 1 -pady 1 

    # Build pulldown menu image format 
    eval {label $f.l -text "Choose method:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # GLM is default format 
    set detectorList [list {Linear Modeling}]
    set df [lindex $detectorList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)

    # Add menu items
    foreach m $detectorList  {
        $f.mbType.m add command -label $m \
            -command ""
    }

    # Save menubutton for config
    set fMRIEngine(gui,mbActDetector) $f.mbType

    #-------------------------------------------
    # Model frame 
    #-------------------------------------------
    set f $parent.fModel
    DevAddButton $f.bLoad "Load Model" "fMRIEngineLoadModel"   13 
    DevAddButton $f.bSave "Save Model" "fMRIEngineSaveModel"   13 
    DevAddButton $f.bView "View Model" "fMRIEngineViewModel"   13 
    DevAddButton $f.bClear "Clear Model" "fMRIEngineClearModel" 13 
    grid $f.bLoad $f.bSave -padx 1 -pady 1 -sticky e
    grid $f.bView $f.bClear -padx 1 -pady 1 -sticky e

    #-------------------------------------------
    # Tabs frame 
    #-------------------------------------------
    fMRIEngineBuildUIForTasks $parent.fTasks
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowHelp
# Displays help message 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineShowHelp {} {
    global fMRIEngine Gui
    
    puts "show help"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineLoadModel
# Loads a saved model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineLoadModel {} {
    global fMRIEngine Gui
    
    puts "load model"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSaveModel
# Saves the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSaveModel {} {
    global fMRIEngine Gui
    
    puts "save model"
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineClearModel
# Clears the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineClearModel {} {
    global fMRIEngine

    set fMRIEngine(baselineEVsAdded)  0
 
    # clear paradigm design panel
    set fMRIEngine(paradigmDesignType) blocked
    set fMRIEngine(checkbuttonRunIdentical) 1
    
    fMRIEngineSelectRunForConditionConfig 1

    set fMRIEngine(entry,title) ""
    set fMRIEngine(entry,tr) ""
    set fMRIEngine(entry,startVol) ""
    set fMRIEngine(entry,onsets) ""
    set fMRIEngine(entry,durations) ""

    fMRIEngineSelectRunForConditionShow 1

    $fMRIEngine(condsListBox) delete 0 end 

    for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} {
        if {[info exists fMRIEngine($r,conditionList)]} {
            foreach title $fMRIEngine($r,conditionList) {
                unset -nocomplain fMRIEngine($r,$title,title)
                unset -nocomplain fMRIEngine($r,$title,startVol)
                unset -nocomplain fMRIEngine($r,$title,onsets)
                unset -nocomplain fMRIEngine($r,$title,durations)
            }
            unset -nocomplain fMRIEngine($r,tr)
            unset -nocomplain fMRIEngine($r,conditionList)
        }
    }

    # clear signal modeling panel
    fMRIEngineUpdateConditionsForSignalModeling 
    fMRIEngineSelectWaveFormForSignalModeling {Box Car} 
    fMRIEngineSelectConvolutionForSignalModeling {none} 
    fMRIEngineSelectHighpassForSignalModeling {none} 
    fMRIEngineSelectLowpassForSignalModeling {none}

    set fMRIEngine(checkbuttonTempDerivative) 0
    set fMRIEngine(checkbuttonGlobalEffects)  0
    $fMRIEngine(evsListBox) delete 0 end 
 
    # clear contrasts panel
    set fMRIEngine(contrastOption) t 
    set fMRIEngine(entry,contrastVector) ""
    $fMRIEngine(contrastsListBox) delete 0 end 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineViewModel
# Views the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineViewModel {} {
    global fMRIEngine
    
    # check if we have real evs (not baseline) added
    set i 0
    set count 0
    set found -1 
    set size [$fMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set v [$fMRIEngine(evsListBox) get $i] 
        if {$v != ""} {
            set found [string first "baseline" $v]
            if {$found >= 0} {
                incr count
            }
        }

        incr i
    }

    if {$size == $count} {
        DevErrorWindow "View the model after signal modeling is complete."
        return
    }
 
    fMRIModelViewLaunchModelView
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForTasks
# Creates UI for tasks in model 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForTasks {parent} {
    global fMRIEngine Gui

    frame $parent.fTop  -bg $Gui(backdrop)
    frame $parent.fHelp -bg $Gui(activeWorkspace)
    frame $parent.fBot  -bg $Gui(activeWorkspace) -height 445 
    pack $parent.fTop $parent.fHelp $parent.fBot \
        -side top -fill x -pady 2 -padx 5 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetup" 2 
    pack $f.bHelp -side left -padx 1 -pady 1 
 
    # Build pulldown task menu 
    eval {label $f.l -text "Specify:"} $Gui(BLA)
    pack $f.l -side left -padx $Gui(pad) -fill x -anchor w

    # Paradigm design is default task 
    set taskList [list {Paradigm Design} {Signal Modeling} {Contrasts}]
    set df [lindex $taskList 0] 
    eval {menubutton $f.mbTask -text $df \
          -relief raised -bd 2 -width 35 \
          -menu $f.mbTask.m} $Gui(WMBA)
    eval {menu $f.mbTask.m} $Gui(WMA)
    pack  $f.mbTask -side left -pady 1 -padx $Gui(pad)

    # Save menubutton for config
    set fMRIEngine(gui,currentModelTask) $f.mbTask

    # Add menu items
    set count 1
    foreach m $taskList  {
        $f.mbTask.m add command -label $m \
            -command "fMRIEngineSetModelTask {$m}"
    }

    #-------------------------------------------
    # Help frame 
    #-------------------------------------------
#    set f $parent.fHelp
#    DevAddButton $f.bHelp "?" "fMRIEngineShowHelp" 2 
#    pack $f.bHelp -side left -padx 5 -pady 1 

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBot

    # Add menu items
    set count 1
    foreach m $taskList {
        # Makes a frame for each reader submodule
        frame $f.f$count -bg $Gui(activeWorkspace) 
        place $f.f$count -relwidth 1.0 -relx 0.0 -relheight 1.0 -rely 0.0 
        switch $m {
            "Paradigm Design" {
                fMRIEngineBuildUIForParadigmDesign $f.f$count
            }
            "Signal Modeling" {
                fMRIEngineBuildUIForSignalModeling $f.f$count
            }
            "Contrasts" {
                fMRIEngineBuildUIForContrasts $f.f$count
            }
        }
        set fMRIEngine(f$count) $f.f$count
        incr count
    }

    # raise the default one 
    raise $fMRIEngine(f1)

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForContrasts 
# Creates UI for task "Contrasts" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForContrasts {parent} {
    global fMRIEngine Gui

    frame $parent.fTop    -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fMiddle -bg $Gui(activeWorkspace) -relief groove -bd 1
    # frame $parent.fBot    -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $parent.fTop $parent.fMiddle \
        -side top -fill x -pady 2 -padx 1 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    frame $f.fTitle    -bg $Gui(activeWorkspace)
    frame $f.fTypes    -bg $Gui(activeWorkspace)
    frame $f.fComp     -bg $Gui(activeWorkspace)
    frame $f.fActions  -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fTypes $f.fComp \
        -side top -fill x -pady 1 -padx 1 
    pack $f.fActions -side top -fill x -pady 1 -padx 1 

    set f $parent.fTop.fTitle
    DevAddLabel $f.l "Compose a contrast:"
    grid $f.l -padx 1 -pady 3

    #-----------------------
    # Type of contrast 
    #-----------------------
    set f $parent.fTop.fTypes
    DevAddLabel $f.l "Type: "
    foreach param "t f" \
        name "{t test} {F test}" {
        eval {radiobutton $f.r$param -width 10 -text $name \
            -variable fMRIEngine(contrastOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rt select
    $f.rf configure -state disabled
    grid $f.l $f.rt $f.rf -padx 1 -pady 1 -sticky e

    #-----------------------
    # Compose a contrast 
    #-----------------------
    set f $parent.fTop.fComp
    DevAddLabel $f.lName "Name: "
    eval {entry $f.eName -width 22 -textvariable fMRIEngine(entry,contrastName)} $Gui(WEA)
    grid $f.lName $f.eName -padx 1 -pady 1 -sticky e

    DevAddLabel $f.lExp "Vector: "
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupContrasts" 2
    eval {entry $f.eExp -width 22 -textvariable fMRIEngine(entry,contrastVector)} $Gui(WEA)
    grid $f.lExp $f.eExp $f.bHelp -padx 1 -pady 1 -sticky e

    #-----------------------
    # Action panel 
    #-----------------------
    set f $parent.fTop.fActions
    DevAddButton $f.bOK "OK" "fMRIEngineAddOrEditContrast" 6 
    grid $f.bOK -padx 2 -pady 3 

    #-------------------------------------------
    # Middle frame 
    #-------------------------------------------
    #-----------------------
    # Contrast list 
    #-----------------------
    set f $parent.fMiddle
    frame $f.fUp      -bg $Gui(activeWorkspace)
    frame $f.fMiddle  -bg $Gui(activeWorkspace)
    frame $f.fDown    -bg $Gui(activeWorkspace)
    pack $f.fUp $f.fMiddle $f.fDown -side top -fill x -pady 1 -padx 2 

    set f $parent.fMiddle.fUp
    DevAddLabel $f.l "Specified contrasts:"
    grid $f.l -padx 1 -pady 3

    set f $parent.fMiddle.fMiddle
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set fMRIEngine(contrastsVerScroll) $f.vs
    set fMRIEngine(contrastsHorScroll) $f.hs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(contrastsVerScroll) set} \
        -xscrollcommand {$::fMRIEngine(contrastsHorScroll) set}
    set fMRIEngine(contrastsListBox) $f.lb
    $fMRIEngine(contrastsVerScroll) configure -command {$fMRIEngine(contrastsListBox) yview}
    $fMRIEngine(contrastsHorScroll) configure -command {$fMRIEngine(contrastsListBox) xview}

    blt::table $f \
        0,0 $fMRIEngine(contrastsListBox) -padx 1 -pady 1 \
        1,0 $fMRIEngine(contrastsHorScroll) -fill x -padx 1 -pady 1 \
        0,1 $fMRIEngine(contrastsVerScroll) -cspan 2 -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fMiddle.fDown
    DevAddButton $f.bEdit "Edit" "fMRIEngineShowContrastToEdit" 6 
    DevAddButton $f.bDelete "Delete" "fMRIEngineDeleteContrast" 6 
    grid $f.bEdit $f.bDelete -padx 2 -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForSignalModeling 
# Creates UI for task "Signal Modeling" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForSignalModeling {parent} {
    global fMRIEngine Gui

    frame $parent.fModeling     -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fAddiModeling -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fOK           -bg $Gui(activeWorkspace)
    frame $parent.fEVs          -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $parent.fEstimate     -bg $Gui(activeWorkspace) -relief groove -bd 1
 
    pack $parent.fModeling $parent.fAddiModeling $parent.fOK \
        $parent.fEVs $parent.fEstimate -side top -fill x -pady 2 -padx 1 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fModeling
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fMenus -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fMenus -side top -fill x -pady 1 -padx 1 

    #-----------------------
    # Menus 
    #-----------------------

    set f $parent.fModeling.fTitle
    DevAddLabel $f.l "Model a condition:" 
    pack $f.l -side top -pady 3 -fill x

    set f $parent.fModeling.fMenus

    # Build pulldown menu for all conditions 
    DevAddLabel $f.lCond "Condition:"

    set condiList [list {none}]
    set df [lindex $condiList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 18 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    foreach m $condiList  {
        $f.mbType.m add command -label $m \
            -command ""
    }

    # Save menubutton for config
    set fMRIEngine(gui,conditionsMenuButtonForSignal) $f.mbType
    set fMRIEngine(gui,conditionsMenuForSignal) $f.mbType.m

    # Build pulldown menu for wave forms 
    DevAddLabel $f.lWave "Waveform:"
    DevAddButton $f.bWaveHelp "?" "fMRIEngineHelpSetupWaveform" 2 
 
    set waveForms [list {Box Car} {Half Sine}]
    set df [lindex $waveForms 0] 
    eval {menubutton $f.mbType2 -text $df \
          -relief raised -bd 2 -width 18 \
          -menu $f.mbType2.m} $Gui(WMBA)
    eval {menu $f.mbType2.m} $Gui(WMA)

    foreach m $waveForms  {
        $f.mbType2.m add command -label $m \
            -command "fMRIEngineSelectWaveFormForSignalModeling \{$m\}" 
    }

    # Save menubutton for config
    set fMRIEngine(gui,waveFormsMenuButtonForSignal) $f.mbType2
    set fMRIEngine(gui,waveFormsMenuForSignal) $f.mbType2.m
    set fMRIEngine(curWaveFormForSignal) $df

    # Build pulldown menu for convolution functions 
    DevAddLabel $f.lConv "Convolution:"
    DevAddButton $f.bConvHelp "?" "fMRIEngineHelpSetupHRFConvolution" 2 
 
    set convFuncs [list {none} {HRF}]
    set df [lindex $convFuncs 0] 
    eval {menubutton $f.mbType3 -text $df \
          -relief raised -bd 2 -width 18 \
          -menu $f.mbType3.m} $Gui(WMBA)
    eval {menu $f.mbType3.m} $Gui(WMA)

    foreach m $convFuncs  {
        $f.mbType3.m add command -label $m \
            -command "fMRIEngineSelectConvolutionForSignalModeling \{$m\}" 
    }
    set fMRIEngine(gui,convolutionMenuButtonForSignal) $f.mbType3
    set fMRIEngine(gui,convolutionMenuForSignal) $f.mbType3.m
    set fMRIEngine(curConvolutionForSignal) $df 

    #-----------------------
    # Temporal derivative 
    #-----------------------
    DevAddButton $f.bTempHelp "?" "fMRIEngineHelpSetupTempDerivative" 2 
    eval {checkbutton $f.cTemp \
        -variable fMRIEngine(checkbuttonTempDerivative) \
        -text "Add temporal derivative"} $Gui(WEA) 
    $f.cTemp deselect 

    blt::table $f \
        0,0 $f.lCond -padx 1 -pady 1 \
        0,1 $f.mbType -fill x -padx 1 -pady 1 \
        1,0 $f.lWave -padx 1 -pady 1 \
        1,1 $f.mbType2 -fill x -padx 1 -pady 1 \
        1,2 $f.bWaveHelp -padx 1 -pady 1 \
        2,0 $f.lConv -padx 1 -pady 1 -fill x \
        2,1 $f.mbType3 -fill x -padx 1 -pady 1 \
        2,2 $f.bConvHelp -padx 1 -pady 1 \
        3,0 $f.cTemp -cspan 2 -fill x -padx 5 -pady 1 \
        3,2 $f.bTempHelp -padx 1 -pady 1

    #-------------------------------------------
    # Middle frame 
    #-------------------------------------------
    set f $parent.fAddiModeling
    frame $f.fTitle    -bg $Gui(activeWorkspace)
    frame $f.fContents -bg $Gui(activeWorkspace)
    frame $f.fActions  -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fContents $f.fActions -side top -fill x -pady 1 -padx 1 

    #-----------------------
    # Menus 
    #-----------------------

    set f $parent.fAddiModeling.fTitle
    DevAddLabel $f.l "Additional modeling:" 
    pack $f.l -side top -pady 3 -fill x

    set f $parent.fAddiModeling.fContents

    # Build pulldown menu for highpass filters  
    DevAddLabel $f.lHighpass "Highpass filter:"
    DevAddButton $f.bHighpassHelp "?" "fMRIEngineHelpSetupHighpassFilter" 2 
 
    set highpassFilters [list {none} {Discrete Cosine Set}]
    set df [lindex $highpassFilters 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 17 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)

    foreach m $highpassFilters  {
        $f.mbType.m add command -label $m \
            -command "fMRIEngineSelectHighpassForSignalModeling \{$m\}" 
    }
    set fMRIEngine(gui,highpassMenuButtonForSignal) $f.mbType
    set fMRIEngine(gui,highpassMenuForSignal) $f.mbType.m
    set fMRIEngine(curHighpassForSignal) $df 

    # Build pulldown menu for lowpass filters 
    DevAddLabel $f.lLowpass "Lowpass filter:"
    DevAddButton $f.bLowpassHelp "?" "fMRIEngineHelpSetupLowpassFilter" 2 
 
    set lowpassFilters [list {none}]
    set df [lindex $lowpassFilters 0] 
    eval {menubutton $f.mbType2 -text $df \
          -relief raised -bd 2 -width 17 \
          -menu $f.mbType2.m} $Gui(WMBA)
    eval {menu $f.mbType2.m} $Gui(WMA)

    foreach m $lowpassFilters  {
        $f.mbType2.m add command -label $m \
            -command "fMRIEngineSelectLowpassForSignalModeling \{$m\}" 
    }
    set fMRIEngine(gui,lowpassMenuButtonForSignal) $f.mbType2
    set fMRIEngine(gui,lowpassMenuForSignal) $f.mbType2.m
    set fMRIEngine(curLowpassForSignal) $df

    #-----------------------
    # Remove global effects 
    #-----------------------
    DevAddButton $f.bEffectsHelp "?" "fMRIEngineHelpSetupGlobalFX" 2 

    eval {checkbutton $f.cEffects \
        -variable fMRIEngine(checkbuttonGlobalEffects) \
        -text "Remove global effects"} $Gui(WEA) 
    $f.cEffects deselect 

    #-----------------------
    # Custom 
    #-----------------------
    DevAddLabel $f.lCustom "Custom:"
    DevAddButton $f.bCustomHelp "?" "fMRIEngineHelpSetupCustomFX" 2 

    eval {entry $f.eCustom -width 18 -state disabled \
        -textvariable fMRIEngine(entry,custom)} $Gui(WEA)

    blt::table $f \
        0,0 $f.lHighpass -padx 1 -pady 1 \
        0,1 $f.mbType -fill x -padx 1 -pady 1 \
        0,2 $f.bHighpassHelp -padx 1 -pady 1 \
        1,0 $f.lLowpass -padx 1 -pady 1 \
        1,1 $f.mbType2 -fill x -padx 1 -pady 1 \
        1,2 $f.bLowpassHelp -padx 1 -pady 1 \
        2,0 $f.cEffects -cspan 2 -fill x -padx 5 -pady 1 \
        2,2 $f.bEffectsHelp -padx 1 -pady 1 \
        3,0 $f.lCustom -padx 1 -pady 1 \
        3,1 $f.eCustom -fill x -padx 1 -pady 1 \
        3,2 $f.bCustomHelp -padx 1 -pady 1 

    #-------------------------------------------
    # OK frame 
    #-------------------------------------------
    set f $parent.fOK
    DevAddButton $f.bOK "OK" "fMRIEngineAddOrEditEV" 6 
    grid $f.bOK -padx 1 -pady 3 

    #-------------------------------------------
    # EVs frame 
    #-------------------------------------------
    #-----------------------
    # EV list 
    #-----------------------
    set f $parent.fEVs
    frame $f.fUp      -bg $Gui(activeWorkspace)
    frame $f.fMiddle  -bg $Gui(activeWorkspace)
    frame $f.fDown    -bg $Gui(activeWorkspace)
    pack $f.fUp $f.fMiddle $f.fDown -side top -fill x -pady 1 -padx 2 

    set f $parent.fEVs.fUp
    DevAddLabel $f.l "Specified explanatory variables:"
    grid $f.l -padx 1 -pady 3

    set f $parent.fEVs.fMiddle
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set fMRIEngine(evsVerScroll) $f.vs
    set fMRIEngine(evsHorScroll) $f.hs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(evsVerScroll) set} \
        -xscrollcommand {$::fMRIEngine(evsHorScroll) set}
    set fMRIEngine(evsListBox) $f.lb
    $fMRIEngine(evsVerScroll) configure -command {$fMRIEngine(evsListBox) yview}
    $fMRIEngine(evsHorScroll) configure -command {$fMRIEngine(evsListBox) xview}

    blt::table $f \
        0,0 $fMRIEngine(evsListBox) -padx 1 -pady 1 \
        1,0 $fMRIEngine(evsHorScroll) -fill x -padx 1 -pady 1 \
        0,1 $fMRIEngine(evsVerScroll) -cspan 2 -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fEVs.fDown
    DevAddButton $f.bView "Edit" "fMRIEngineShowEVToEdit" 6 
    DevAddButton $f.bDelete "Delete" "fMRIEngineDeleteEV" 6 
    grid $f.bView $f.bDelete -padx 2 -pady 3 

    #-------------------------------------------
    # Estimate frame 
    #-------------------------------------------
    set f $parent.fEstimate
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupEstimate" 2
    DevAddButton $f.bEstimate "Fit Model" "fMRIEngineEstimate" 15 
    grid $f.bEstimate $f.bHelp -padx 1 -pady 4 -sticky e
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForParadigmDesign 
# Creates UI for task "Paradigm Design" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForParadigmDesign {parent} {
    global fMRIEngine Gui

    frame $parent.fTop   -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fBot   -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $parent.fTop $parent.fBot \
        -side top -fill x -pady 2 -padx 1 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    #-----------------------
    # Configure a condition 
    #-----------------------
    set f $parent.fTop
    frame $f.fUp      -bg $Gui(activeWorkspace) 
    frame $f.fMiddle  -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $f.fDown    -bg $Gui(activeWorkspace)
    pack $f.fUp $f.fMiddle $f.fDown -side top -fill x -pady 1 -padx 2 
 
    set f $parent.fTop.fUp
    frame $f.fChoice  -bg $Gui(activeWorkspace) 
    frame $f.fLabel   -bg $Gui(activeWorkspace)
    pack $f.fChoice $f.fLabel -side top -fill x -pady 2 -padx 1 

    set f $parent.fTop.fUp.fChoice
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupBlockEventMixed" 2 
    foreach param "blocked event-related mixed" \
        name "{Blocked} {Event-r} {Mixed}" {
        eval {radiobutton $f.r$param -width 6 -text $name \
            -variable fMRIEngine(paradigmDesignType) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -command "" \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rblocked select
    grid $f.bHelp $f.rblocked $f.revent-related $f.rmixed \
        -padx 0 -pady 1 -sticky e

    set f $parent.fTop.fUp.fLabel
    eval {checkbutton $f.cRunIdentical \
        -variable fMRIEngine(checkbuttonRunIdentical) \
        -text "All runs are identical"} $Gui(WEA) 
    bind $f.cRunIdentical <1> "fMRIEngineSelectRunForConditionConfig 1" 
#    bind $f.cRunIdentical <1> "fMRIEngineIdenticalizeConditions" 
    $f.cRunIdentical select 

    # Build pulldown menu for all runs 
    DevAddLabel $f.lConf "Configure a condition for run#:"
    set runList [list {1}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbType -text $df \
        -relief raised -bd 2 -width 8 \
        -menu $f.mbType.m} $Gui(WMBA)
    bind $f.mbType <1> "fMRIEngineUpdateRunsForConditionConfig" 
    eval {menu $f.mbType.m} $Gui(WMA)

    # Add menu items
    foreach m $runList  {
        $f.mbType.m add command -label $m \
            -command "fMRIEngineUpdateRunsForConditionConfig" 
    }

    set fMRIEngine(curRunForConditionConfig) 1

    # Save menubutton for config
    set fMRIEngine(gui,runListMenuButtonForConditionConfig) $f.mbType
    set fMRIEngine(gui,runListMenuForConditionConfig) $f.mbType.m

    blt::table $f \
        0,0 $f.cRunIdentical -cspan 2 -fill x -padx 3 -pady 3 \
        1,0 $f.lConf -padx 2 -pady 3 \
        1,1 $f.mbType -fill x -padx 2 -pady 3 

    set f $parent.fTop.fMiddle
    # Entry fields (the loop makes a frame for each variable)
    set fMRIEngine(paramConfigLabels) \
        [list {Title} {TR} {Start Vol} {Onsets} {Durations}]
    set fMRIEngine(paramConfigEntries) \
        [list title tr startVol onsets durations]
    set i 0      
    set len [llength $fMRIEngine(paramConfigLabels)]
    while {$i < $len} { 

        set name [lindex $fMRIEngine(paramConfigLabels) $i]
        set param [lindex $fMRIEngine(paramConfigEntries) $i]
 
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 23 -textvariable fMRIEngine(entry,$param)} $Gui(WEA)

        grid $f.l$param $f.e$param -padx 1 -pady 1 -sticky e
        grid $f.e$param -sticky w

        incr i
    }

    #-----------------------
    # Add or update 
    #-----------------------
    set f $parent.fTop.fDown
    DevAddButton $f.bOK "OK" "fMRIEngineAddOrEditCondition" 6 
    grid $f.bOK -padx 1 -pady 2 

    #-------------------------------------------
    # Bottom frame 
    #-------------------------------------------
    set f $parent.fBot
    frame $f.fUp      -bg $Gui(activeWorkspace)
    frame $f.fMiddle  -bg $Gui(activeWorkspace)
    frame $f.fDown    -bg $Gui(activeWorkspace)
    pack $f.fUp $f.fMiddle $f.fDown -side top -fill x -pady 1 -padx 1 

    set f $parent.fBot.fUp
    # Build pulldown menu for all runs 
    DevAddLabel $f.l "Specified conditions for run#:"
    set runList [list {1}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbType -text $df \
        -relief raised -bd 2 -width 8 \
        -menu $f.mbType.m} $Gui(WMBA)
    bind $f.mbType <1> "fMRIEngineUpdateRunsForConditionShow" 
    eval {menu $f.mbType.m} $Gui(WMA)

    # Add menu items
    foreach m $runList  {
        $f.mbType.m add command -label $m \
            -command "fMRIEngineUpdateRunsForConditionShow" 
    }

    set fMRIEngine(curRunForConditionShow) 1 

    # Save menubutton for config
    set fMRIEngine(gui,runListMenuButtonForConditionShow) $f.mbType
    set fMRIEngine(gui,runListMenuForConditionShow) $f.mbType.m
    blt::table $f \
        0,0 $f.l -padx 2 -pady 3 \
        0,1 $f.mbType -fill x -padx 2 -pady 3

    set f $parent.fBot.fMiddle
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set fMRIEngine(condsVerScroll) $f.vs
    listbox $f.lb -height 5 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(condsVerScroll) set}
    set fMRIEngine(condsListBox) $f.lb
    $fMRIEngine(condsVerScroll) configure -command {$fMRIEngine(condsListBox) yview}

    blt::table $f \
        0,0 $fMRIEngine(condsListBox) -padx 1 -pady 1 \
        0,1 $fMRIEngine(condsVerScroll) -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fBot.fDown
    DevAddButton $f.bDelete "Delete" "fMRIEngineDeleteCondition" 6 
    DevAddButton $f.bView "Edit" "fMRIEngineShowConditionToEdit" 6 
    grid $f.bView $f.bDelete -padx 1 -pady 2 
}


proc fMRIEngineShowConditionToEdit {} {
    global fMRIEngine 

    set curs [$fMRIEngine(condsListBox) curselection]
    if {$curs != ""} {
        set con [$fMRIEngine(condsListBox) get $curs] 
        if {$con != ""} {
            set i 1 
            set i2 [string first ":" $con]
            set run [string range $con $i [expr $i2-1]] 
            set title [string range $con [expr $i2+1] end] 
       }

        set run [string trim $run]
        set title [string trim $title]
        set found [lsearch -exact $fMRIEngine($run,conditionList) $title]

        if {$found >= 0} {
            if {! $fMRIEngine(checkbuttonRunIdentical) &&
                $fMRIEngine(noOfRuns) > 1} {
                set run2 $fMRIEngine(curRunForConditionShow)
                fMRIEngineSelectRunForConditionConfig $run2 
            }

            set fMRIEngine(entry,title) $fMRIEngine($run,$title,title)
            set fMRIEngine(entry,tr) $fMRIEngine($run,tr)
            set fMRIEngine(entry,startVol) $fMRIEngine($run,$title,startVol)
            set fMRIEngine(entry,onsets) $fMRIEngine($run,$title,onsets)
            set fMRIEngine(entry,durations) $fMRIEngine($run,$title,durations)
        }

        set fMRIEngine(indexForEdit,condsListBox) $curs
    }
}

 
#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateRunsForConditionShow
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateRunsForConditionShow {} {
    global fMRIEngine 

    set runs [string trim $fMRIEngine(noOfRuns)]
    if {$runs < 1} {
        DevErrorWindow "No of runs must be at least 1."
    } else { 
        $fMRIEngine(gui,runListMenuForConditionShow) delete 0 end
        if {$runs > 1} {
            $fMRIEngine(gui,runListMenuForConditionShow) add command -label All \
                -command "fMRIEngineSelectRunForConditionShow All"
        }

        set count 1
        while {$count <= $runs} {
            $fMRIEngine(gui,runListMenuForConditionShow) add command -label $count \
                -command "fMRIEngineSelectRunForConditionShow $count"
            incr count
        }   
    }
}


proc fMRIEngineSelectRunForConditionShow {run} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,runListMenuButtonForConditionShow) config -text $run
    set fMRIEngine(curRunForConditionShow) $run 

    fMRIEngineShowConditions 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateRunsForConditionConfig
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateRunsForConditionConfig {} {
    global fMRIEngine 

    if {! $fMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $fMRIEngine(noOfRuns)]
        if {$runs < 1} {
            DevErrorWindow "No of runs must be at least 1."
        } else { 
            $fMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 1
            while {$count <= $runs} {
                $fMRIEngine(gui,runListMenuForConditionConfig) add command -label $count \
                    -command "fMRIEngineSelectRunForConditionConfig $count"
                incr count
            } 
        }
    } else {
        $fMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
        $fMRIEngine(gui,runListMenuForConditionConfig) add command -label 1\
            -command "fMRIEngineSelectRunForConditionConfig 1"
    }
}

proc fMRIEngineSelectRunForConditionConfig {run} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,runListMenuButtonForConditionConfig) config -text $run
    set fMRIEngine(curRunForConditionConfig) $run 
}


proc fMRIEngineIdenticalizeConditions {} {
    global fMRIEngine 

    fMRIEngineSelectRunForConditionConfig 1

    # If we have multiple runs, copy all conditions of Run# 1 
    # to the rest of runs.
    if {$fMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $fMRIEngine(noOfRuns)]
        if {$runs > 1} {
            $fMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 2 
            while {$count <= $runs} {
                set fMRIEngine($count,$title,title) $fMRIEngine(1,$title,title)  
                set fMRIEngine($count,$title,startVol) $fMRIEngine(1,$title,startVol) 
                set fMRIEngine($count,$title,onsets) $fMRIEngine(1,$title,onsets) 
                set fMRIEngine($count,$title,durations) $fMRIEngine(1,$title,durations) 

                incr count
            } 

            fMRIEngineShowConditions 
        }
    } 

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineConfigureEntryDurations
# Configures the Durations entry 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineConfigureEntryDurations {} {
    global fMRIEngine

    if {$fMRIEngine(durationsOption) == "No"} {
        $fMRIEngine(entryWidget,durations) configure -state disabled 
    } else {
        $fMRIEngine(entryWidget,durations) configure -state normal 
    }

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineEditCondition
# Views a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineEditCondition {} {
    global fMRIEngine

    set index [$fMRIEngine(condHierTable) curselection] 
    if {$index == ""} {
        return
    }

    set name [eval $fMRIEngine(condHierTable) get -full $index] 
    set b [info exists fMRIEngine(conditionList)]
    if {$b} {
        set found [lsearch -exact $fMRIEngine(conditionList) $name]
    }

    if {$found >= 0} {
        set fMRIEngine(entry,title) $fMRIEngine($name,title)
        set fMRIEngine(entry,onsets) $fMRIEngine($name,onsets)
        set fMRIEngine(entry,durations) $fMRIEngine($name,durations)
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDeleteCondition
# Deletes a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDeleteCondition {} {
    global fMRIEngine

    set curs [$fMRIEngine(condsListBox) curselection]
    if {$curs != ""} {
        set con [$fMRIEngine(condsListBox) get $curs] 
        if {$con != ""} {
            set i 1 
            set i2 [string first ":" $con]
            set run [string range $con $i [expr $i2-1]] 
            set title [string range $con [expr $i2+1] end] 
        }

        set run [string trim $run]
        set title [string trim $title]
        set found [lsearch -exact $fMRIEngine($run,conditionList) $title]

        if {$found >= 0} {
            set fMRIEngine($run,conditionList) \
                [lreplace $fMRIEngine($run,conditionList) $found $found]
            unset -nocomplain fMRIEngine($run,$title,title)
            unset -nocomplain fMRIEngine($run,$title,startVol)
            unset -nocomplain fMRIEngine($run,$title,onsets)
            unset -nocomplain fMRIEngine($run,$title,durations)
        }

        $fMRIEngine(condsListBox) delete $curs 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineAddOrEditCondition
# Adds or edit a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAddOrEditCondition {} {
    global fMRIEngine

    set title [string trim $fMRIEngine(entry,title)]
    set tr [string trim $fMRIEngine(entry,tr)]
    set startVol [string trim $fMRIEngine(entry,startVol)]
    set onsets [string trim $fMRIEngine(entry,onsets)]
    set durations [string trim $fMRIEngine(entry,durations)]

    # Error checking
    if {$title == ""} {
        DevErrorWindow "Input a unique name for this condition."
        return
    }
    if {$tr == ""} {
        DevErrorWindow "Input the TR in seconds."
        return
    }
    if {$startVol == ""} {
        DevErrorWindow "Input the start volume number"
        return
    }
    if {$onsets == ""} {
        DevErrorWindow "Input the onset vector in seconds."
        return
    }
    if {$fMRIEngine(paradigmDesignType) != "event-related" && $durations == ""} {
        DevErrorWindow "Input the duration vector in seconds."
        return
    }

    set currRun $fMRIEngine(curRunForConditionConfig)
    set found -1
    set b [info exists fMRIEngine($currRun,conditionList)]
    if {$b} {
        set found [lsearch -exact $fMRIEngine($currRun,conditionList) $title]
    }

    if {$found == -1} {
        lappend fMRIEngine($currRun,conditionList) $title
    }
    
    set fMRIEngine($currRun,designType) $fMRIEngine(paradigmDesignType)
    set fMRIEngine($currRun,tr) $tr

    set fMRIEngine($currRun,$title,title) $title
    set fMRIEngine($currRun,$title,startVol) $startVol
    set fMRIEngine($currRun,$title,onsets) $onsets
    set fMRIEngine($currRun,$title,durations) $durations

    if {$fMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $fMRIEngine(noOfRuns)]
        if {$runs > 1} {
            $fMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 2 
            while {$count <= $runs} {
                set b [info exists fMRIEngine($count,conditionList)]
                if {$b} {
                    set found [lsearch -exact $fMRIEngine($count,conditionList) $title]
                }

                if {$found == -1} {
                    lappend fMRIEngine($count,conditionList) $title
                }

                set fMRIEngine($count,designType) $fMRIEngine(paradigmDesignType)
                set fMRIEngine($count,tr) $fMRIEngine(1,tr) 

                set fMRIEngine($count,$title,title) $fMRIEngine(1,$title,title)  
                set fMRIEngine($count,$title,startVol) $fMRIEngine(1,$title,startVol) 
                set fMRIEngine($count,$title,onsets) $fMRIEngine(1,$title,onsets) 
                set fMRIEngine($count,$title,durations) $fMRIEngine(1,$title,durations) 

                incr count
            } 
        }
    } 

    fMRIEngineShowConditions 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowConditions
# Displays conditions 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineShowConditions {} {
    global fMRIEngine 

    set start 1
    set end [$fMRIEngine(gui,runListMenuForConditionShow) index end] 
    set currRun $fMRIEngine(curRunForConditionShow)
    $fMRIEngine(condsListBox) delete 0 end

    if {$currRun != "All"} {
        set start $currRun
        set end $currRun
    } 

    set i $start
    while {$i <= $end} {
        if {[info exists fMRIEngine($i,conditionList)]} {  
            set len [llength $fMRIEngine($i,conditionList)]
            set count 0
            while {$count < $len} {
                set title [lindex $fMRIEngine($i,conditionList) $count]
                $fMRIEngine(condsListBox) insert end "r$i:$title"
                incr count
            }
        }

        incr i 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetConvFunction
# Switches convolution function 
# .ARGS
# func the convolution function 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetConvFunction {func} {
    global fMRIEngine
    
    set fMRIEngine(currentConvFunc) $func

    # configure menubutton
    $fMRIEngine(gui,currentConvFunc) config -text $func

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSetModelTask
# Switches model task 
# .ARGS
# task the model task 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSetModelTask {task} {
    global fMRIEngine
    
    set fMRIEngine(currentModelTask) $task

    # configure menubutton
    $fMRIEngine(gui,currentModelTask) config -text $task

    set count -1 
    switch $task {
        "Paradigm Design" {
            set count 1
        }
        "Signal Modeling" {
            set count 2
            fMRIEngineUpdateConditionsForSignalModeling
       }
        "Contrasts" {
            set count 3
        }
    }

    set f  $fMRIEngine(f$count)
    raise $f
    focus $f
}


