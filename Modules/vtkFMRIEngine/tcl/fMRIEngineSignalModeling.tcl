proc fMRIEngineUpdateConditionsForSignalModeling {} {
    global fMRIEngine 

    $fMRIEngine(gui,conditionsMenuForSignal) delete 0 end 
    set start 1
    set end $fMRIEngine(noOfRuns)

    set firstCondition ""
    set i $start
    while {$i <= $end} {
        if {[info exists fMRIEngine($i,conditionList)]} {  
            set len [llength $fMRIEngine($i,conditionList)]
            set count 0
            while {$count < $len} {
                set title [lindex $fMRIEngine($i,conditionList) $count]
                set l "r$i:$title"
                $fMRIEngine(gui,conditionsMenuForSignal) add command -label $l \
                    -command "fMRIEngineSelectConditionForSignalModeling $l"
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
        $fMRIEngine(gui,conditionsMenuForSignal) add command -label "none" \
            -command "fMRIEngineSelectConditionForSignalModeling none"
    }

    fMRIEngineSelectConditionForSignalModeling $firstCondition 
}


proc fMRIEngineAddBaselineEVs {} {
    global fMRIEngine 

    $fMRIEngine(evsListBox) delete 0 end

    set i 1
    while {$i <= $fMRIEngine(noOfRuns)} {
        $fMRIEngine(evsListBox) insert end "r$i:baseline"
        incr i 
    }

    set fMRIEngine(baselineEVsAdded) $fMRIEngine(noOfRuns) 
}


proc fMRIEngineSelectConditionForSignalModeling {condition} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,conditionsMenuButtonForSignal) config -text $condition
    set fMRIEngine(curConditionForSignal) $condition 
}


proc fMRIEngineSelectWaveFormForSignalModeling {form} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,waveFormsMenuButtonForSignal) config -text $form
    set fMRIEngine(curWaveFormForSignal) $form 
}


proc fMRIEngineSelectConvolutionForSignalModeling {conv} {
    global fMRIEngine 

   # configure menubutton
    $fMRIEngine(gui,convolutionMenuButtonForSignal) config -text $conv
    set fMRIEngine(curConvolutionForSignal) $conv 
}


proc fMRIEngineSelectHighpassForSignalModeling {pass} {
    global fMRIEngine 

   # configure menubutton
    $fMRIEngine(gui,highpassMenuButtonForSignal) config -text $pass
    set fMRIEngine(curHighpassForSignal) $pass 
}


proc fMRIEngineSelectLowpassForSignalModeling {pass} {
    global fMRIEngine 

   # configure menubutton
    $fMRIEngine(gui,lowpassMenuButtonForSignal) config -text $pass
    set fMRIEngine(curLowpassForSignal) $pass 
}

  
proc fMRIEngineAddOrEditEV {} {
    global fMRIEngine 

    set con $fMRIEngine(curConditionForSignal)
    if {$con == "none"} {
        DevErrorWindow "Select a valid condition."
        return
    }
    set wform $fMRIEngine(curWaveFormForSignal) 
    set conv  $fMRIEngine(curConvolutionForSignal) 
    set deriv $fMRIEngine(checkbuttonTempDerivative) 
    set hpass $fMRIEngine(curHighpassForSignal)
    set lpass $fMRIEngine(curLowpassForSignal) 
    set effes $fMRIEngine(checkbuttonGlobalEffects) 

    set ev "$con:$wform:$conv:$deriv:$hpass:$lpass:$effes"
    if {[info exists fMRIEngine($ev,ev)]} {
        DevErrorWindow "The following EV has been added: \n$ev"
        return
    }

    # If an EV exists with the same conditon, remove it
    set i 0
    set found -1 
    set index -1
    set size [$fMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set v [$fMRIEngine(evsListBox) get $i] 
        if {$v != ""} {
            set found [string first $con $v]
            if {$found >= 0} {
                fMRIEngineDeleteEV $i
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


    set fMRIEngine($ev,ev)               $ev
    set fMRIEngine($ev,run)              $run
    set fMRIEngine($ev,title,ev)         $title
    set fMRIEngine($ev,condition,ev)     $con
    set fMRIEngine($ev,waveform,ev)      $wform
    set fMRIEngine($ev,convolution,ev)   $conv
    set fMRIEngine($ev,derivative,ev)    $deriv
    set fMRIEngine($ev,highpass,ev)      $hpass
    set fMRIEngine($ev,lowpass,ev)       $lpass
    set fMRIEngine($ev,globaleffects,ev) $effes

    $fMRIEngine(evsListBox) insert end $ev 
}


proc fMRIEngineDeleteEV {{index -1}} {
    global fMRIEngine 

    if {$index == -1} {
        set curs [$fMRIEngine(evsListBox) curselection]
    } else {
        set curs $index
    }

    if {$curs != ""} {
        set ev [$fMRIEngine(evsListBox) get $curs] 
        if {$ev != ""} {
            unset -nocomplain fMRIEngine($ev,ev)            
            unset -nocomplain fMRIEngine($ev,run)
            unset -nocomplain fMRIEngine($ev,title,ev) 
            unset -nocomplain fMRIEngine($ev,condition,ev) 
            unset -nocomplain fMRIEngine($ev,waveform,ev)   
            unset -nocomplain fMRIEngine($ev,convolution,ev)
            unset -nocomplain fMRIEngine($ev,derivative,ev)
            unset -nocomplain fMRIEngine($ev,highpass,ev) 
            unset -nocomplain fMRIEngine($ev,lowpass,ev) 
            unset -nocomplain fMRIEngine($ev,globaleffects,ev)
        }

        $fMRIEngine(evsListBox) delete $curs 
    }
}


proc fMRIEngineShowEVToEdit {} {
    global fMRIEngine 

    set curs [$fMRIEngine(evsListBox) curselection]
    if {$curs != ""} {
        set ev [$fMRIEngine(evsListBox) get $curs] 
        if {$ev != "" &&
            [info exists fMRIEngine($ev,ev)]} {
            fMRIEngineSelectConditionForSignalModeling   $fMRIEngine($ev,condition,ev)  
            fMRIEngineSelectWaveFormForSignalModeling    $fMRIEngine($ev,waveform,ev)   
            fMRIEngineSelectConvolutionForSignalModeling $fMRIEngine($ev,convolution,ev)
            fMRIEngineSelectHighpassForSignalModeling    $fMRIEngine($ev,highpass,ev) 
            fMRIEngineSelectLowpassForSignalModeling     $fMRIEngine($ev,lowpass,ev) 
 
            set fMRIEngine(checkbuttonTempDerivative) $fMRIEngine($ev,derivative,ev)
            set fMRIEngine(checkbuttonGlobalEffects)  $fMRIEngine($ev,globaleffects,ev) 
        }
    }
}


proc fMRIEngineAddRegressors {} {
    global MultiVolumeReader fMRIEngine fMRIModelView 

    if {[info commands fMRIEngine(regressors)] != ""} {
        fMRIEngine(regressors) Delete
        unset -nocomplain fMRIEngine(regressors)
    }
    vtkFloatArray fMRIEngine(regressors)

    set evs [expr $fMRIModelView(Design,totalEVs) / $fMRIEngine(noOfRuns)] 
    set vols 0
    for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} { 
        set seqName $fMRIEngine($r,sequenceName)
        set vols [expr $MultiVolumeReader($seqName,noOfVolumes) + $vols]
    }
#    puts "evs = $evs"
#    puts "vols = $vols"

    fMRIEngine(regressors) SetNumberOfTuples $vols 
    fMRIEngine(regressors) SetNumberOfComponents $evs 

    for {set i 1} {$i <= $evs} {incr i} { 
        set data ""
        for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} { 
            set data [concat $data $fMRIModelView(Data,Run$r,EV$i,EVData)]
        }

        set fMRIEngine($i,combinedEVs) $data 
    }

    for {set j 0} {$j < $vols} {incr j} { 
        for {set i 0} {$i < $evs} {incr i} { 
            set index [expr $i+1]
            set data $fMRIEngine($index,combinedEVs)
            set e [lindex $data $j]
            fMRIEngine(regressors) InsertComponent $j $i $e 
        }
    }
}


proc fMRIEngineEstimate {} {
    global fMRIEngine Gui Volume MultiVolumeReader

    # generates data without popping up the model image 
    set done [fMRIModelViewGenerateModel]
    if {! $done} {
        return 
    }

    fMRIEngineAddRegressors

    # always uses a new instance of vtkActivationEstimator 
    if {[info commands fMRIEngine(actEstimator)] != ""} {
        fMRIEngine(actEstimator) Delete
        unset -nocomplain fMRIEngine(actEstimator)
    }
    vtkActivationEstimator fMRIEngine(actEstimator)

    # adds progress bar
    set obs1 [fMRIEngine(actEstimator) AddObserver StartEvent MainStartProgress]
    set obs2 [fMRIEngine(actEstimator) AddObserver ProgressEvent \
              "MainShowProgress fMRIEngine(actEstimator)"]
    set obs3 [fMRIEngine(actEstimator) AddObserver EndEvent MainEndProgress]
    set Gui(progressText) "Estimating..."

    for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} { 
        set seqName $fMRIEngine($r,sequenceName)
        set id $MultiVolumeReader($seqName,firstMRMLid)
        set id2 $MultiVolumeReader($seqName,lastMRMLid)
        # puts "id = $id"
        # puts "id2 = $id2"
        while {$id <= $id2} {
            Volume($id,vol) Update
            fMRIEngine(actEstimator) AddInput [Volume($id,vol) GetOutput]
            incr id
        }
    }

    puts $Gui(progressText)
 
    # always uses a new instance of vtkActivationDetector
    if {[info commands fMRIEngine(detector)] != ""} {
        fMRIEngine(detector) Delete
        unset -nocomplain fMRIEngine(detector)
    }
    vtkActivationDetector fMRIEngine(detector)

    fMRIEngine(detector) SetDetectionMethod 1
    fMRIEngine(detector) SetRegressors fMRIEngine(regressors) 

    if {[info exists fMRIEngine(lowerThreshold)]} {
        fMRIEngine(actEstimator) SetLowerThreshold $fMRIEngine(lowerThreshold)
    }
    fMRIEngine(actEstimator) SetDetector fMRIEngine(detector)  
    fMRIEngine(actEstimator) Update

    set fMRIEngine(actBetaAndStd) [fMRIEngine(actEstimator) GetOutput]
    $fMRIEngine(actBetaAndStd) Update

    fMRIEngine(actEstimator) RemoveObserver $obs1 
    fMRIEngine(actEstimator) RemoveObserver $obs2 
    fMRIEngine(actEstimator) RemoveObserver $obs3 

    puts "...done"
}



 
