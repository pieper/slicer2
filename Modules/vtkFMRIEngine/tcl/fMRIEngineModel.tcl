#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForSetupTab
# Creates UI for model tab 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForSetupTab {parent} {
    global FMRIEngine Gui

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
#    DevAddButton $f.bHelp "?" "FMRIEngineShowHelp" 2 
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
    set FMRIEngine(gui,mbActDetector) $f.mbType

    #-------------------------------------------
    # Model frame 
    #-------------------------------------------
    set f $parent.fModel
    DevAddButton $f.bLoad "Load Model" "FMRIEngineLoadModel"   13 
    DevAddButton $f.bSave "Save Model" "FMRIEngineSaveModel"   13 
    DevAddButton $f.bView "View Model" "FMRIEngineViewModel"   13 
    DevAddButton $f.bClear "Clear Model" "FMRIEngineClearModel" 13 
    grid $f.bLoad $f.bSave -padx 1 -pady 1 -sticky e
    grid $f.bView $f.bClear -padx 1 -pady 1 -sticky e

    #-------------------------------------------
    # Tabs frame 
    #-------------------------------------------
    FMRIEngineBuildUIForTasks $parent.fTasks
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineShowHelp
# Displays help message 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineShowHelp {} {
    global FMRIEngine Gui
    
    puts "show help"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineLoadModel
# Loads a saved model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineLoadModel {} {
    global FMRIEngine Gui
    
    puts "load model"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSaveModel
# Saves the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSaveModel {} {
    global FMRIEngine Gui
    
    puts "save model"
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineClearModel
# Clears the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineClearModel {} {
    global FMRIEngine

    set FMRIEngine(baselineEVsAdded)  0
 
    # clear paradigm design panel
    set FMRIEngine(paradigmDesignType) blocked
    set FMRIEngine(checkbuttonRunIdentical) 1
    
    FMRIEngineSelectRunForConditionConfig 1

    set FMRIEngine(entry,title) ""
    set FMRIEngine(entry,tr) ""
    set FMRIEngine(entry,startVol) ""
    set FMRIEngine(entry,onsets) ""
    set FMRIEngine(entry,durations) ""

    FMRIEngineSelectRunForConditionShow 1

    $FMRIEngine(condsListBox) delete 0 end 

    for {set r 1} {$r <= $FMRIEngine(noOfRuns)} {incr r} {
        if {[info exists FMRIEngine($r,conditionList)]} {
            foreach title $FMRIEngine($r,conditionList) {
                unset -nocomplain FMRIEngine($r,$title,title)
                unset -nocomplain FMRIEngine($r,$title,startVol)
                unset -nocomplain FMRIEngine($r,$title,onsets)
                unset -nocomplain FMRIEngine($r,$title,durations)
            }
            unset -nocomplain FMRIEngine($r,tr)
            unset -nocomplain FMRIEngine($r,conditionList)
        }
    }

    # clear signal modeling panel
    FMRIEngineUpdateConditionsForSignalModeling 
    FMRIEngineSelectWaveFormForSignalModeling {Box Car} 
    FMRIEngineSelectConvolutionForSignalModeling {none} 
    FMRIEngineSelectHighpassForSignalModeling {none} 
    FMRIEngineSelectLowpassForSignalModeling {none}

    set FMRIEngine(checkbuttonTempDerivative) 0
    set FMRIEngine(checkbuttonGlobalEffects)  0
    $FMRIEngine(evsListBox) delete 0 end 
 
    # clear contrasts panel
    set FMRIEngine(contrastOption) t 
    set FMRIEngine(entry,contrastVector) ""
    $FMRIEngine(contrastsListBox) delete 0 end 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineViewModel
# Views the current model 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineViewModel {} {
    global FMRIEngine
    
    # check if we have real evs (not baseline) added
    set i 0
    set count 0
    set found -1 
    set size [$FMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set v [$FMRIEngine(evsListBox) get $i] 
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
# .PROC FMRIEngineBuildUIForTasks
# Creates UI for tasks in model 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForTasks {parent} {
    global FMRIEngine Gui

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
    set FMRIEngine(gui,currentModelTask) $f.mbTask

    # Add menu items
    set count 1
    foreach m $taskList  {
        $f.mbTask.m add command -label $m \
            -command "FMRIEngineSetModelTask {$m}"
    }

    #-------------------------------------------
    # Help frame 
    #-------------------------------------------
#    set f $parent.fHelp
#    DevAddButton $f.bHelp "?" "FMRIEngineShowHelp" 2 
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
                FMRIEngineBuildUIForParadigmDesign $f.f$count
            }
            "Signal Modeling" {
                FMRIEngineBuildUIForSignalModeling $f.f$count
            }
            "Contrasts" {
                FMRIEngineBuildUIForContrasts $f.f$count
            }
        }
        set FMRIEngine(f$count) $f.f$count
        incr count
    }

    # raise the default one 
    raise $FMRIEngine(f1)

}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForContrasts 
# Creates UI for task "Contrasts" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForContrasts {parent} {
    global FMRIEngine Gui

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
            -variable FMRIEngine(contrastOption) -value $param \
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
    eval {entry $f.eName -width 22 -textvariable FMRIEngine(entry,contrastName)} $Gui(WEA)
    grid $f.lName $f.eName -padx 1 -pady 1 -sticky e

    DevAddLabel $f.lExp "Vector: "
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupContrasts" 2
    eval {entry $f.eExp -width 22 -textvariable FMRIEngine(entry,contrastVector)} $Gui(WEA)
    grid $f.lExp $f.eExp $f.bHelp -padx 1 -pady 1 -sticky e

    #-----------------------
    # Action panel 
    #-----------------------
    set f $parent.fTop.fActions
    DevAddButton $f.bOK "OK" "FMRIEngineAddOrEditContrast" 6 
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
    set FMRIEngine(contrastsVerScroll) $f.vs
    set FMRIEngine(contrastsHorScroll) $f.hs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::FMRIEngine(contrastsVerScroll) set} \
        -xscrollcommand {$::FMRIEngine(contrastsHorScroll) set}
    set FMRIEngine(contrastsListBox) $f.lb
    $FMRIEngine(contrastsVerScroll) configure -command {$FMRIEngine(contrastsListBox) yview}
    $FMRIEngine(contrastsHorScroll) configure -command {$FMRIEngine(contrastsListBox) xview}

    blt::table $f \
        0,0 $FMRIEngine(contrastsListBox) -padx 1 -pady 1 \
        1,0 $FMRIEngine(contrastsHorScroll) -fill x -padx 1 -pady 1 \
        0,1 $FMRIEngine(contrastsVerScroll) -cspan 2 -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fMiddle.fDown
    DevAddButton $f.bEdit "Edit" "FMRIEngineShowContrastToEdit" 6 
    DevAddButton $f.bDelete "Delete" "FMRIEngineDeleteContrast" 6 
    grid $f.bEdit $f.bDelete -padx 2 -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForSignalModeling 
# Creates UI for task "Signal Modeling" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForSignalModeling {parent} {
    global FMRIEngine Gui

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
    set FMRIEngine(gui,conditionsMenuButtonForSignal) $f.mbType
    set FMRIEngine(gui,conditionsMenuForSignal) $f.mbType.m

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
            -command "FMRIEngineSelectWaveFormForSignalModeling \{$m\}" 
    }

    # Save menubutton for config
    set FMRIEngine(gui,waveFormsMenuButtonForSignal) $f.mbType2
    set FMRIEngine(gui,waveFormsMenuForSignal) $f.mbType2.m
    set FMRIEngine(curWaveFormForSignal) $df

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
            -command "FMRIEngineSelectConvolutionForSignalModeling \{$m\}" 
    }
    set FMRIEngine(gui,convolutionMenuButtonForSignal) $f.mbType3
    set FMRIEngine(gui,convolutionMenuForSignal) $f.mbType3.m
    set FMRIEngine(curConvolutionForSignal) $df 

    #-----------------------
    # Temporal derivative 
    #-----------------------
    DevAddButton $f.bTempHelp "?" "fMRIEngineHelpSetupTempDerivative" 2 
    eval {checkbutton $f.cTemp \
        -variable FMRIEngine(checkbuttonTempDerivative) \
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
            -command "FMRIEngineSelectHighpassForSignalModeling \{$m\}" 
    }
    set FMRIEngine(gui,highpassMenuButtonForSignal) $f.mbType
    set FMRIEngine(gui,highpassMenuForSignal) $f.mbType.m
    set FMRIEngine(curHighpassForSignal) $df 

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
            -command "FMRIEngineSelectLowpassForSignalModeling \{$m\}" 
    }
    set FMRIEngine(gui,lowpassMenuButtonForSignal) $f.mbType2
    set FMRIEngine(gui,lowpassMenuForSignal) $f.mbType2.m
    set FMRIEngine(curLowpassForSignal) $df

    #-----------------------
    # Remove global effects 
    #-----------------------
    DevAddButton $f.bEffectsHelp "?" "fMRIEngineHelpSetupGlobalFX" 2 

    eval {checkbutton $f.cEffects \
        -variable FMRIEngine(checkbuttonGlobalEffects) \
        -text "Remove global effects"} $Gui(WEA) 
    $f.cEffects deselect 

    #-----------------------
    # Custom 
    #-----------------------
    DevAddLabel $f.lCustom "Custom:"
    DevAddButton $f.bCustomHelp "?" "fMRIEngineHelpSetupCustomFX" 2 

    eval {entry $f.eCustom -width 18 -state disabled \
        -textvariable FMRIEngine(entry,custom)} $Gui(WEA)

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
    DevAddButton $f.bOK "OK" "FMRIEngineAddOrEditEV" 6 
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
    set FMRIEngine(evsVerScroll) $f.vs
    set FMRIEngine(evsHorScroll) $f.hs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::FMRIEngine(evsVerScroll) set} \
        -xscrollcommand {$::FMRIEngine(evsHorScroll) set}
    set FMRIEngine(evsListBox) $f.lb
    $FMRIEngine(evsVerScroll) configure -command {$FMRIEngine(evsListBox) yview}
    $FMRIEngine(evsHorScroll) configure -command {$FMRIEngine(evsListBox) xview}

    blt::table $f \
        0,0 $FMRIEngine(evsListBox) -padx 1 -pady 1 \
        1,0 $FMRIEngine(evsHorScroll) -fill x -padx 1 -pady 1 \
        0,1 $FMRIEngine(evsVerScroll) -cspan 2 -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fEVs.fDown
    DevAddButton $f.bView "Edit" "FMRIEngineShowEVToEdit" 6 
    DevAddButton $f.bDelete "Delete" "FMRIEngineDeleteEV" 6 
    grid $f.bView $f.bDelete -padx 2 -pady 3 

    #-------------------------------------------
    # Estimate frame 
    #-------------------------------------------
    set f $parent.fEstimate
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupEstimate" 2
    DevAddButton $f.bEstimate "Fit Model" "FMRIEngineEstimate" 15 
    grid $f.bEstimate $f.bHelp -padx 1 -pady 4 -sticky e
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineBuildUIForParadigmDesign 
# Creates UI for task "Paradigm Design" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineBuildUIForParadigmDesign {parent} {
    global FMRIEngine Gui

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
            -variable FMRIEngine(paradigmDesignType) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -command "" \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rblocked select
    grid $f.bHelp $f.rblocked $f.revent-related $f.rmixed \
        -padx 0 -pady 1 -sticky e

    set f $parent.fTop.fUp.fLabel
    eval {checkbutton $f.cRunIdentical \
        -variable FMRIEngine(checkbuttonRunIdentical) \
        -text "All runs are identical"} $Gui(WEA) 
    bind $f.cRunIdentical <1> "FMRIEngineSelectRunForConditionConfig 1" 
#    bind $f.cRunIdentical <1> "FMRIEngineIdenticalizeConditions" 
    $f.cRunIdentical select 

    # Build pulldown menu for all runs 
    DevAddLabel $f.lConf "Configure a condition for run#:"
    set runList [list {1}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbType -text $df \
        -relief raised -bd 2 -width 8 \
        -menu $f.mbType.m} $Gui(WMBA)
    bind $f.mbType <1> "FMRIEngineUpdateRunsForConditionConfig" 
    eval {menu $f.mbType.m} $Gui(WMA)

    # Add menu items
    foreach m $runList  {
        $f.mbType.m add command -label $m \
            -command "FMRIEngineUpdateRunsForConditionConfig" 
    }

    set FMRIEngine(curRunForConditionConfig) 1

    # Save menubutton for config
    set FMRIEngine(gui,runListMenuButtonForConditionConfig) $f.mbType
    set FMRIEngine(gui,runListMenuForConditionConfig) $f.mbType.m

    blt::table $f \
        0,0 $f.cRunIdentical -cspan 2 -fill x -padx 3 -pady 3 \
        1,0 $f.lConf -padx 2 -pady 3 \
        1,1 $f.mbType -fill x -padx 2 -pady 3 

    set f $parent.fTop.fMiddle
    # Entry fields (the loop makes a frame for each variable)
    set FMRIEngine(paramConfigLabels) \
        [list {Title} {TR} {Start Vol} {Onsets} {Durations}]
    set FMRIEngine(paramConfigEntries) \
        [list title tr startVol onsets durations]
    set i 0      
    set len [llength $FMRIEngine(paramConfigLabels)]
    while {$i < $len} { 

        set name [lindex $FMRIEngine(paramConfigLabels) $i]
        set param [lindex $FMRIEngine(paramConfigEntries) $i]
 
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 23 -textvariable FMRIEngine(entry,$param)} $Gui(WEA)

        grid $f.l$param $f.e$param -padx 1 -pady 1 -sticky e
        grid $f.e$param -sticky w

        incr i
    }

    #-----------------------
    # Add or update 
    #-----------------------
    set f $parent.fTop.fDown
    DevAddButton $f.bOK "OK" "FMRIEngineAddOrEditCondition" 6 
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
    bind $f.mbType <1> "FMRIEngineUpdateRunsForConditionShow" 
    eval {menu $f.mbType.m} $Gui(WMA)

    # Add menu items
    foreach m $runList  {
        $f.mbType.m add command -label $m \
            -command "FMRIEngineUpdateRunsForConditionShow" 
    }

    set FMRIEngine(curRunForConditionShow) 1 

    # Save menubutton for config
    set FMRIEngine(gui,runListMenuButtonForConditionShow) $f.mbType
    set FMRIEngine(gui,runListMenuForConditionShow) $f.mbType.m
    blt::table $f \
        0,0 $f.l -padx 2 -pady 3 \
        0,1 $f.mbType -fill x -padx 2 -pady 3

    set f $parent.fBot.fMiddle
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set FMRIEngine(condsVerScroll) $f.vs
    listbox $f.lb -height 5 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::FMRIEngine(condsVerScroll) set}
    set FMRIEngine(condsListBox) $f.lb
    $FMRIEngine(condsVerScroll) configure -command {$FMRIEngine(condsListBox) yview}

    blt::table $f \
        0,0 $FMRIEngine(condsListBox) -padx 1 -pady 1 \
        0,1 $FMRIEngine(condsVerScroll) -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fBot.fDown
    DevAddButton $f.bDelete "Delete" "FMRIEngineDeleteCondition" 6 
    DevAddButton $f.bView "Edit" "FMRIEngineShowConditionToEdit" 6 
    grid $f.bView $f.bDelete -padx 1 -pady 2 
}


proc FMRIEngineShowConditionToEdit {} {
    global FMRIEngine 

    set curs [$FMRIEngine(condsListBox) curselection]
    if {$curs != ""} {
        set con [$FMRIEngine(condsListBox) get $curs] 
        if {$con != ""} {
            set i 1 
            set i2 [string first ":" $con]
            set run [string range $con $i [expr $i2-1]] 
            set title [string range $con [expr $i2+1] end] 
       }

        set run [string trim $run]
        set title [string trim $title]
        set found [lsearch -exact $FMRIEngine($run,conditionList) $title]

        if {$found >= 0} {
            if {! $FMRIEngine(checkbuttonRunIdentical) &&
                $FMRIEngine(noOfRuns) > 1} {
                set run2 $FMRIEngine(curRunForConditionShow)
                FMRIEngineSelectRunForConditionConfig $run2 
            }

            set FMRIEngine(entry,title) $FMRIEngine($run,$title,title)
            set FMRIEngine(entry,tr) $FMRIEngine($run,tr)
            set FMRIEngine(entry,startVol) $FMRIEngine($run,$title,startVol)
            set FMRIEngine(entry,onsets) $FMRIEngine($run,$title,onsets)
            set FMRIEngine(entry,durations) $FMRIEngine($run,$title,durations)
        }

        set FMRIEngine(indexForEdit,condsListBox) $curs
    }
}

 
#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateRunsForConditionShow
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateRunsForConditionShow {} {
    global FMRIEngine 

    set runs [string trim $FMRIEngine(noOfRuns)]
    if {$runs < 1} {
        DevErrorWindow "No of runs must be at least 1."
    } else { 
        $FMRIEngine(gui,runListMenuForConditionShow) delete 0 end
        if {$runs > 1} {
            $FMRIEngine(gui,runListMenuForConditionShow) add command -label All \
                -command "FMRIEngineSelectRunForConditionShow All"
        }

        set count 1
        while {$count <= $runs} {
            $FMRIEngine(gui,runListMenuForConditionShow) add command -label $count \
                -command "FMRIEngineSelectRunForConditionShow $count"
            incr count
        }   
    }
}


proc FMRIEngineSelectRunForConditionShow {run} {
    global FMRIEngine 

    # configure menubutton
    $FMRIEngine(gui,runListMenuButtonForConditionShow) config -text $run
    set FMRIEngine(curRunForConditionShow) $run 

    FMRIEngineShowConditions 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineUpdateRunsForConditionConfig
# Chooses one sequence from the sequence list loaded within the Ibrowser module 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineUpdateRunsForConditionConfig {} {
    global FMRIEngine 

    if {! $FMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $FMRIEngine(noOfRuns)]
        if {$runs < 1} {
            DevErrorWindow "No of runs must be at least 1."
        } else { 
            $FMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 1
            while {$count <= $runs} {
                $FMRIEngine(gui,runListMenuForConditionConfig) add command -label $count \
                    -command "FMRIEngineSelectRunForConditionConfig $count"
                incr count
            } 
        }
    } else {
        $FMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
        $FMRIEngine(gui,runListMenuForConditionConfig) add command -label 1\
            -command "FMRIEngineSelectRunForConditionConfig 1"
    }
}

proc FMRIEngineSelectRunForConditionConfig {run} {
    global FMRIEngine 

    # configure menubutton
    $FMRIEngine(gui,runListMenuButtonForConditionConfig) config -text $run
    set FMRIEngine(curRunForConditionConfig) $run 
}


proc FMRIEngineIdenticalizeConditions {} {
    global FMRIEngine 

    FMRIEngineSelectRunForConditionConfig 1

    # If we have multiple runs, copy all conditions of Run# 1 
    # to the rest of runs.
    if {$FMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $FMRIEngine(noOfRuns)]
        if {$runs > 1} {
            $FMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 2 
            while {$count <= $runs} {
                set FMRIEngine($count,$title,title) $FMRIEngine(1,$title,title)  
                set FMRIEngine($count,$title,startVol) $FMRIEngine(1,$title,startVol) 
                set FMRIEngine($count,$title,onsets) $FMRIEngine(1,$title,onsets) 
                set FMRIEngine($count,$title,durations) $FMRIEngine(1,$title,durations) 

                incr count
            } 

            FMRIEngineShowConditions 
        }
    } 

}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineConfigureEntryDurations
# Configures the Durations entry 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineConfigureEntryDurations {} {
    global FMRIEngine

    if {$FMRIEngine(durationsOption) == "No"} {
        $FMRIEngine(entryWidget,durations) configure -state disabled 
    } else {
        $FMRIEngine(entryWidget,durations) configure -state normal 
    }

}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineEditCondition
# Views a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineEditCondition {} {
    global FMRIEngine

    set index [$FMRIEngine(condHierTable) curselection] 
    if {$index == ""} {
        return
    }

    set name [eval $FMRIEngine(condHierTable) get -full $index] 
    set b [info exists FMRIEngine(conditionList)]
    if {$b} {
        set found [lsearch -exact $FMRIEngine(conditionList) $name]
    }

    if {$found >= 0} {
        set FMRIEngine(entry,title) $FMRIEngine($name,title)
        set FMRIEngine(entry,onsets) $FMRIEngine($name,onsets)
        set FMRIEngine(entry,durations) $FMRIEngine($name,durations)
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineDeleteCondition
# Deletes a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineDeleteCondition {} {
    global FMRIEngine

    set curs [$FMRIEngine(condsListBox) curselection]
    if {$curs != ""} {
        set con [$FMRIEngine(condsListBox) get $curs] 
        if {$con != ""} {
            set i 1 
            set i2 [string first ":" $con]
            set run [string range $con $i [expr $i2-1]] 
            set title [string range $con [expr $i2+1] end] 
        }

        set run [string trim $run]
        set title [string trim $title]
        set found [lsearch -exact $FMRIEngine($run,conditionList) $title]

        if {$found >= 0} {
            set FMRIEngine($run,conditionList) \
                [lreplace $FMRIEngine($run,conditionList) $found $found]
            unset -nocomplain FMRIEngine($run,$title,title)
            unset -nocomplain FMRIEngine($run,$title,startVol)
            unset -nocomplain FMRIEngine($run,$title,onsets)
            unset -nocomplain FMRIEngine($run,$title,durations)
        }

        $FMRIEngine(condsListBox) delete $curs 
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineAddOrEditCondition
# Adds or edit a condition 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineAddOrEditCondition {} {
    global FMRIEngine

    set title [string trim $FMRIEngine(entry,title)]
    set tr [string trim $FMRIEngine(entry,tr)]
    set startVol [string trim $FMRIEngine(entry,startVol)]
    set onsets [string trim $FMRIEngine(entry,onsets)]
    set durations [string trim $FMRIEngine(entry,durations)]

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
    if {$FMRIEngine(paradigmDesignType) != "event-related" && $durations == ""} {
        DevErrorWindow "Input the duration vector in seconds."
        return
    }

    set currRun $FMRIEngine(curRunForConditionConfig)
    set found -1
    set b [info exists FMRIEngine($currRun,conditionList)]
    if {$b} {
        set found [lsearch -exact $FMRIEngine($currRun,conditionList) $title]
    }

    if {$found == -1} {
        lappend FMRIEngine($currRun,conditionList) $title
    }
    
    set FMRIEngine($currRun,designType) $FMRIEngine(paradigmDesignType)
    set FMRIEngine($currRun,tr) $tr

    set FMRIEngine($currRun,$title,title) $title
    set FMRIEngine($currRun,$title,startVol) $startVol
    set FMRIEngine($currRun,$title,onsets) $onsets
    set FMRIEngine($currRun,$title,durations) $durations

    if {$FMRIEngine(checkbuttonRunIdentical)} {
        set runs [string trim $FMRIEngine(noOfRuns)]
        if {$runs > 1} {
            $FMRIEngine(gui,runListMenuForConditionConfig) delete 0 end
            set count 2 
            while {$count <= $runs} {
                set b [info exists FMRIEngine($count,conditionList)]
                if {$b} {
                    set found [lsearch -exact $FMRIEngine($count,conditionList) $title]
                }

                if {$found == -1} {
                    lappend FMRIEngine($count,conditionList) $title
                }

                set FMRIEngine($count,designType) $FMRIEngine(paradigmDesignType)
                set FMRIEngine($count,tr) $FMRIEngine(1,tr) 

                set FMRIEngine($count,$title,title) $FMRIEngine(1,$title,title)  
                set FMRIEngine($count,$title,startVol) $FMRIEngine(1,$title,startVol) 
                set FMRIEngine($count,$title,onsets) $FMRIEngine(1,$title,onsets) 
                set FMRIEngine($count,$title,durations) $FMRIEngine(1,$title,durations) 

                incr count
            } 
        }
    } 

    FMRIEngineShowConditions 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineShowConditions
# Displays conditions 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineShowConditions {} {
    global FMRIEngine 

    set start 1
    set end [$FMRIEngine(gui,runListMenuForConditionShow) index end] 
    set currRun $FMRIEngine(curRunForConditionShow)
    $FMRIEngine(condsListBox) delete 0 end

    if {$currRun != "All"} {
        set start $currRun
        set end $currRun
    } 

    set i $start
    while {$i <= $end} {
        if {[info exists FMRIEngine($i,conditionList)]} {  
            set len [llength $FMRIEngine($i,conditionList)]
            set count 0
            while {$count < $len} {
                set title [lindex $FMRIEngine($i,conditionList) $count]
                $FMRIEngine(condsListBox) insert end "r$i:$title"
                incr count
            }
        }

        incr i 
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetConvFunction
# Switches convolution function 
# .ARGS
# func the convolution function 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetConvFunction {func} {
    global FMRIEngine
    
    set FMRIEngine(currentConvFunc) $func

    # configure menubutton
    $FMRIEngine(gui,currentConvFunc) config -text $func

}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSetModelTask
# Switches model task 
# .ARGS
# task the model task 
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSetModelTask {task} {
    global FMRIEngine
    
    set FMRIEngine(currentModelTask) $task

    # configure menubutton
    $FMRIEngine(gui,currentModelTask) config -text $task

    set count -1 
    switch $task {
        "Paradigm Design" {
            set count 1
        }
        "Signal Modeling" {
            set count 2
            FMRIEngineUpdateConditionsForSignalModeling
       }
        "Contrasts" {
            set count 3
        }
    }

    set f  $FMRIEngine(f$count)
    raise $f
    focus $f
}


