proc FMRIEngineUpdateConditionsForSignalModeling {} {
    global FMRIEngine 

    $FMRIEngine(gui,conditionsMenuForSignal) delete 0 end 
    set start 1
    set end $FMRIEngine(noOfRuns)

    set firstCondition ""
    set i $start
    while {$i <= $end} {
        if {[info exists FMRIEngine($i,conditionList)]} {  
            set len [llength $FMRIEngine($i,conditionList)]
            set count 0
            while {$count < $len} {
                set title [lindex $FMRIEngine($i,conditionList) $count]
                set l "r$i:$title"
                $FMRIEngine(gui,conditionsMenuForSignal) add command -label $l \
                    -command "FMRIEngineSelectConditionForSignalModeling $l"
                if {$firstCondition == ""} {
                    set firstCondition $l
                }

                incr count
            }
        }

        incr i 
    }

    if {$firstCondition == ""} {
        set firstCondition "none"
        $FMRIEngine(gui,conditionsMenuForSignal) add command -label "none" \
            -command "FMRIEngineSelectConditionForSignalModeling none"
    }

    FMRIEngineSelectConditionForSignalModeling $firstCondition 
}


proc FMRIEngineAddBaselineEVs {} {
    global FMRIEngine 

    $FMRIEngine(evsListBox) delete 0 end

    set i 1
    while {$i <= $FMRIEngine(noOfRuns)} {
        $FMRIEngine(evsListBox) insert end "r$i:baseline"
        incr i 
    }

    set FMRIEngine(baselineEVsAdded) $FMRIEngine(noOfRuns) 
}


proc FMRIEngineSelectConditionForSignalModeling {condition} {
    global FMRIEngine 

    # configure menubutton
    $FMRIEngine(gui,conditionsMenuButtonForSignal) config -text $condition
    set FMRIEngine(curConditionForSignal) $condition 
}


proc FMRIEngineSelectWaveFormForSignalModeling {form} {
    global FMRIEngine 

    # configure menubutton
    $FMRIEngine(gui,waveFormsMenuButtonForSignal) config -text $form
    set FMRIEngine(curWaveFormForSignal) $form 
}


proc FMRIEngineSelectConvolutionForSignalModeling {conv} {
    global FMRIEngine 

   # configure menubutton
    $FMRIEngine(gui,convolutionMenuButtonForSignal) config -text $conv
    set FMRIEngine(curConvolutionForSignal) $conv 
}


proc FMRIEngineSelectHighpassForSignalModeling {pass} {
    global FMRIEngine 

   # configure menubutton
    $FMRIEngine(gui,highpassMenuButtonForSignal) config -text $pass
    set FMRIEngine(curHighpassForSignal) $pass 
}


proc FMRIEngineSelectLowpassForSignalModeling {pass} {
    global FMRIEngine 

   # configure menubutton
    $FMRIEngine(gui,lowpassMenuButtonForSignal) config -text $pass
    set FMRIEngine(curLowpassForSignal) $pass 
}

  
proc FMRIEngineAddOrEditEV {} {
    global FMRIEngine 

    set con $FMRIEngine(curConditionForSignal)
    if {$con == "none"} {
        DevErrorWindow "Select a valid condition."
        return
    }
    set wform $FMRIEngine(curWaveFormForSignal) 
    set conv  $FMRIEngine(curConvolutionForSignal) 
    set deriv $FMRIEngine(checkbuttonTempDerivative) 
    set hpass $FMRIEngine(curHighpassForSignal)
    set lpass $FMRIEngine(curLowpassForSignal) 
    set effes $FMRIEngine(checkbuttonGlobalEffects) 

    set ev "$con:$wform:$conv:$deriv:$hpass:$lpass:$effes"
    if {[info exists FMRIEngine($ev,ev)]} {
        DevErrorWindow "The following EV has been added: \n$ev"
        return
    }

    # If an EV exists with the same conditon, remove it
    set i 0
    set found -1 
    set index -1
    set size [$FMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set v [$FMRIEngine(evsListBox) get $i] 
        if {$v != ""} {
            set found [string first $con $v]
            if {$found >= 0} {
                FMRIEngineDeleteEV $i
                break
            }
        }

        incr i
    }

    if {$con != ""} {
        set i 1 
        set i2 [string first ":" $con]
        set run [string range $con $i [expr $i2-1]] 
        set run [string trim $run]
        set title [string range $con [expr $i2+1] end] 
        set title [string trim $title]
    }


    set FMRIEngine($ev,ev)               $ev
    set FMRIEngine($ev,run)              $run
    set FMRIEngine($ev,title,ev)         $title
    set FMRIEngine($ev,condition,ev)     $con
    set FMRIEngine($ev,waveform,ev)      $wform
    set FMRIEngine($ev,convolution,ev)   $conv
    set FMRIEngine($ev,derivative,ev)    $deriv
    set FMRIEngine($ev,highpass,ev)      $hpass
    set FMRIEngine($ev,lowpass,ev)       $lpass
    set FMRIEngine($ev,globaleffects,ev) $effes

    $FMRIEngine(evsListBox) insert end $ev 
}


proc FMRIEngineDeleteEV {{index -1}} {
    global FMRIEngine 

    if {$index == -1} {
        set curs [$FMRIEngine(evsListBox) curselection]
    } else {
        set curs $index
    }

    if {$curs != ""} {
        set ev [$FMRIEngine(evsListBox) get $curs] 
        if {$ev != ""} {
            unset -nocomplain FMRIEngine($ev,ev)            
            unset -nocomplain FMRIEngine($ev,run)
            unset -nocomplain FMRIEngine($ev,title,ev) 
            unset -nocomplain FMRIEngine($ev,condition,ev) 
            unset -nocomplain FMRIEngine($ev,waveform,ev)   
            unset -nocomplain FMRIEngine($ev,convolution,ev)
            unset -nocomplain FMRIEngine($ev,derivative,ev)
            unset -nocomplain FMRIEngine($ev,highpass,ev) 
            unset -nocomplain FMRIEngine($ev,lowpass,ev) 
            unset -nocomplain FMRIEngine($ev,globaleffects,ev)
        }

        $FMRIEngine(evsListBox) delete $curs 
    }
}


proc FMRIEngineShowEVToEdit {} {
    global FMRIEngine 

    set curs [$FMRIEngine(evsListBox) curselection]
    if {$curs != ""} {
        set ev [$FMRIEngine(evsListBox) get $curs] 
        if {$ev != "" &&
            [info exists FMRIEngine($ev,ev)]} {
            FMRIEngineSelectConditionForSignalModeling   $FMRIEngine($ev,condition,ev)  
            FMRIEngineSelectWaveFormForSignalModeling    $FMRIEngine($ev,waveform,ev)   
            FMRIEngineSelectConvolutionForSignalModeling $FMRIEngine($ev,convolution,ev)
            FMRIEngineSelectHighpassForSignalModeling    $FMRIEngine($ev,highpass,ev) 
            FMRIEngineSelectLowpassForSignalModeling     $FMRIEngine($ev,lowpass,ev) 
 
            set FMRIEngine(checkbuttonTempDerivative) $FMRIEngine($ev,derivative,ev)
            set FMRIEngine(checkbuttonGlobalEffects)  $FMRIEngine($ev,globaleffects,ev) 
        }
    }
}


proc FMRIEngineAddRegressors {} {
    global MultiVolumeReader FMRIEngine fMRIModelView 

    if {[info commands FMRIEngine(regressors)] != ""} {
        FMRIEngine(regressors) Delete
        unset -nocomplain FMRIEngine(regressors)
    }
    vtkFloatArray FMRIEngine(regressors)

    set evs [expr $fMRIModelView(Design,totalEVs) / $FMRIEngine(noOfRuns)] 
    set vols 0
    for {set r 1} {$r <= $FMRIEngine(noOfRuns)} {incr r} { 
        set seqName $FMRIEngine($r,sequenceName)
        set vols [expr $MultiVolumeReader($seqName,noOfVolumes) + $vols]
    }
#    puts "evs = $evs"
#    puts "vols = $vols"

    FMRIEngine(regressors) SetNumberOfTuples $vols 
    FMRIEngine(regressors) SetNumberOfComponents $evs 

    for {set i 1} {$i <= $evs} {incr i} { 
        set data ""
        for {set r 1} {$r <= $FMRIEngine(noOfRuns)} {incr r} { 
            set data [concat $data $fMRIModelView(Data,Run$r,EV$i,EVData)]
        }

        set FMRIEngine($i,combinedEVs) $data 
    }

    for {set j 0} {$j < $vols} {incr j} { 
        for {set i 0} {$i < $evs} {incr i} { 
            set index [expr $i+1]
            set data $FMRIEngine($index,combinedEVs)
            set e [lindex $data $j]
            FMRIEngine(regressors) InsertComponent $j $i $e 
        }
    }
}


proc FMRIEngineEstimate {} {
    global FMRIEngine Gui Volume MultiVolumeReader

    # generates data without popping up the model image 
    set done [fMRIModelViewGenerateModel]
    if {! $done} {
        return 
    }

    FMRIEngineAddRegressors

    # always uses a new instance of vtkActivationEstimator 
    if {[info commands FMRIEngine(actEstimator)] != ""} {
        FMRIEngine(actEstimator) Delete
        unset -nocomplain FMRIEngine(actEstimator)
    }
    vtkActivationEstimator FMRIEngine(actEstimator)

    # adds progress bar
    set obs1 [FMRIEngine(actEstimator) AddObserver StartEvent MainStartProgress]
    set obs2 [FMRIEngine(actEstimator) AddObserver ProgressEvent \
              "MainShowProgress FMRIEngine(actEstimator)"]
    set obs3 [FMRIEngine(actEstimator) AddObserver EndEvent MainEndProgress]
    set Gui(progressText) "Estimating..."

    for {set r 1} {$r <= $FMRIEngine(noOfRuns)} {incr r} { 
        set seqName $FMRIEngine($r,sequenceName)
        set id $MultiVolumeReader($seqName,firstMRMLid)
        set id2 $MultiVolumeReader($seqName,lastMRMLid)
        # puts "id = $id"
        # puts "id2 = $id2"
        while {$id <= $id2} {
            Volume($id,vol) Update
            FMRIEngine(actEstimator) AddInput [Volume($id,vol) GetOutput]
            incr id
        }
    }

    puts $Gui(progressText)
 
    # always uses a new instance of vtkActivationDetector
    if {[info commands FMRIEngine(detector)] != ""} {
        FMRIEngine(detector) Delete
        unset -nocomplain FMRIEngine(detector)
    }
    vtkActivationDetector FMRIEngine(detector)

    FMRIEngine(detector) SetDetectionMethod 1
    FMRIEngine(detector) SetRegressors FMRIEngine(regressors) 

    if {[info exists FMRIEngine(lowerThreshold)]} {
        FMRIEngine(actEstimator) SetLowerThreshold $FMRIEngine(lowerThreshold)
    }
    FMRIEngine(actEstimator) SetDetector FMRIEngine(detector)  
    FMRIEngine(actEstimator) Update

    set FMRIEngine(actBetaAndStd) [FMRIEngine(actEstimator) GetOutput]
    $FMRIEngine(actBetaAndStd) Update

    FMRIEngine(actEstimator) RemoveObserver $obs1 
    FMRIEngine(actEstimator) RemoveObserver $obs2 
    FMRIEngine(actEstimator) RemoveObserver $obs3 

    puts "...done"
}



 
