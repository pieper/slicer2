#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        fMRIEngineSignalModeling.tcl
# PROCEDURES:  
#   fMRIEngineBuildUIForSignalModeling  parent
#   fMRIEngineViewCoefficients
#   fMRIEngineSelectRunForModelFitting run
#   fMRIEngineUpdateRunsForModelFitting
#   fMRIEngineUpdateConditionsForSignalModeling 
#   fMRIEngineSelectConditionForSignalModeling condition
#   fMRIEngineSelectWaveFormForSignalModeling form
#   fMRIEngineSelectConvolutionForSignalModeling conv
#   fMRIEngineSelectHighpassForSignalModeling pass
#   fMRIEngineSelectLowpassForSignalModeling pass
#   fMRIEngineAddOrEditEV
#   fMRIEngineDeleteEV index
#   fMRIEngineShowEVToEdit
#   fMRIEngineAddInputVolumes run
#   fMRIEngineCheckMultiRuns
#   fMRIEngineAddRegressors run
#   fMRIEngineCountEVs
#   fMRIEngineFitModel
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForSignalModeling 
# Creates UI for task "Signal Modeling" 
# .ARGS
# windowpath parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForSignalModeling {parent} {
    global fMRIEngine Gui

    frame $parent.fChoices      -bg $Gui(activeWorkspace)
    frame $parent.fModeling     -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fFiltering -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fMoreModeling -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $parent.fOK           -bg $Gui(activeWorkspace)
    frame $parent.fEVs          -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $parent.fEstimate     -bg $Gui(activeWorkspace) -relief groove -bd 1
 
    pack $parent.fChoices $parent.fModeling $parent.fFiltering $parent.fMoreModeling \
        $parent.fOK $parent.fEVs  -side top -fill x -pady 2 -padx 1

    #-------------------------------------------
    # Choices frame 
    #-------------------------------------------
    set f $parent.fChoices
    eval {checkbutton $f.cApplyAll \
              -variable fMRIEngine(checkbuttonApplyAllConditions) \
              -indicatoron 1 -relief flat -text "Model all conditions identically" \
              -command "fMRIEngineSelectAllConditionsForSignalModeling" } $Gui(WEA) 
    $f.cApplyAll deselect 
    pack $f.cApplyAll -padx 1 -pady 2 -fill x


    #-------------------------------------------
    # Modeling frame 
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
    pack $f.l -side top -pady 7 -fill x

    set f $parent.fModeling.fMenus

    # Build pulldown menu for all conditions 
    DevAddLabel $f.lCond "Condition:"

    set condiList [list {none}]
    set df [lindex $condiList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 15 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    #bind $f.mbType <1> "fMRIEngineUpdateConditionsForSignalModeling"
    
    foreach m $condiList  {
        $f.mbType.m add command -label $m \
            -command "fMRIEngineSelectConditionForSignalModeling $m"
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
          -relief raised -bd 2 -width 15 \
          -indicatoron 1 \
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
          -relief raised -bd 2 -width 15 \
          -indicatoron 1 \
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
    DevAddLabel $f.lDeriv "Derivatives:"
    DevAddButton $f.bDerivHelp "?" "fMRIEngineHelpSetupTempDerivative" 2 
    #--- wjp changed 09/01/05
    set ::fMRIEngine(numDerivatives) 0
    set ::fMRIEngine(curDerivativesForSignal) $::fMRIEngine(numDerivatives)
    set ::fMRIEngine(derivOptions) [ list {none} {1st} {1st+2nd} {1st+2nd+3rd} ]
    set df [lindex $::fMRIEngine(derivOptions) 0 ]
    eval { menubutton $f.mbDeriv -text $df \
               -relief raised -bd 2 -width 15 \
               -indicatoron 1 \
               -menu $f.mbDeriv.m } $Gui(WMBA)
    eval {menu $f.mbDeriv.m } $Gui(WMA)
    foreach m $::fMRIEngine(derivOptions) {
        $f.mbDeriv.m add command -label $m \
            -command "fMRIEngineSelectNumDerivativesForSignalModeling $m"
    }
    set ::fMRIEngine(gui,derivativeMenuButtonForSignal) $f.mbDeriv

    blt::table $f \
        0,0 $f.lCond -padx 1 -pady 1 -anchor e \
        0,1 $f.mbType -fill x -padx 1 -pady 1 -anchor w \
        1,0 $f.lWave -padx 1 -pady 1 -anchor e \
        1,1 $f.mbType2 -fill x -padx 1 -pady 1 -anchor w \
        1,2 $f.bWaveHelp -padx 1 -pady 1 \
        2,0 $f.lConv -padx 1 -pady 1 -fill x -anchor e \
        2,1 $f.mbType3 -fill x -padx 1 -pady 1 -anchor w \
        2,2 $f.bConvHelp -padx 1 -pady 1 \
        3,0 $f.lDeriv -fill x -padx 1 -pady 1 -anchor e \
        3,1 $f.mbDeriv -padx 1 -pady 1 -anchor w \
        3,2 $f.bDerivHelp -padx 1 -pady 1
    
    #-------------------------------------------
    # Filtering frame 
    #-------------------------------------------
    set f $parent.fFiltering
    frame $f.fTitle    -bg $Gui(activeWorkspace)
    frame $f.fHighpass -bg $Gui(activeWorkspace) -relief groove -bd 1 
    #frame $f.fLowpass -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $f.fGlobalEffects -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $f.fActions  -bg $Gui(activeWorkspace)
    #commenting out lowpass frame 10/16/05
    #pack $f.fTitle $f.fHighpass $f.fLowpass $f.fGlobalEffects $f.fActions -side top -fill x -pady 1 -padx 1 
    pack $f.fTitle $f.fHighpass $f.fGlobalEffects $f.fActions -side top -fill x -pady 1 -padx 1 

    #-----------------------
    # Filtering->Title
    # Filtering->Highpass
    # Filtering->Lowpass
    # Filtering->GlobalEffects
    #-----------------------
    set f $parent.fFiltering.fTitle
    DevAddLabel $f.l "Nuissance signal modeling:" 
    pack $f.l -side top -pady 7 -fill x

    #--- highpass filter
    set f $parent.fFiltering.fHighpass
    DevAddLabel $f.lHighpass "Trend model:"
    DevAddButton $f.bHighpassHelp "?" "fMRIEngineHelpSetupHighpassFilter" 2 
    set highpassFilters [list {none} {Discrete Cosine}]
    set df [lindex $highpassFilters 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 15 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)

    foreach m $highpassFilters  {
        $f.mbType.m add command -label $m \
            -command "fMRIEngineSelectHighpassForSignalModeling \{$m\}" 
    }
    set fMRIEngine(gui,highpassMenuButtonForSignal) $f.mbType
    set fMRIEngine(gui,highpassMenuForSignal) $f.mbType.m
    set fMRIEngine(curHighpassForSignal) $df 

    #--- just wiring this in now... 10/04/05
    set ::fMRIEngine(curRunForSignal) 0
    DevAddLabel $f.lCutoff "Cutoff period:"
    eval {entry $f.eCutoff -width 18  \
        -textvariable fMRIEngine(entry,highpassCutoff) } $Gui(WEA)
    bind $f.eCutoff <Return> "fMRIEngineSelectCustomHighpassTemporalCutoff"
    set ::fMRIEngine(entry,highpassCutoff) "default"
    DevAddButton $f.bHighpassCutoffHelp "?" "fMRIEngineHelpSelectHighpassCutoff" 2 
    DevAddButton $f.bHighpassDefault "use default cutoff" "fMRIEngineSelectDefaultHighpassTemporalCutoff" 2
    blt::table $f \
        0,0 $f.lHighpass -padx 1 -pady 1 \
        0,1 $f.mbType -fill x -padx 1 -pady 1 \
        0,2 $f.bHighpassHelp -padx 1 -pady 1 \
        1,0 $f.lCutoff -padx 1 -pady 1 \
        1,1 $f.eCutoff -fill x -padx 1 -pady 1 \
        1,2 $f.bHighpassCutoffHelp -padx 1 -pady 1 \
        2,1 $f.bHighpassDefault -padx 1 -fill x -pady 1

    #comment out lowpass; forget temporal smoothing for now.
    if { 0 } {
        #--- lowpass filter
        #set f $parent.fFiltering.fLowpass
        #DevAddLabel $f.lLowpass "Lowpass:"
        #DevAddButton $f.bLowpassHelp "?" "fMRIEngineHelpSetupLowpassFilter" 2 
        #set lowpassFilters [list {none} {Gaussian}]
        #set df [lindex $lowpassFilters 0] 
        #eval {menubutton $f.mbType2 -text $df \
        #          -relief raised -bd 2 -width 15 \
        #         -indicatoron 1 \
        #        -menu $f.mbType2.m} $Gui(WMBA)
        #eval {menu $f.mbType2.m} $Gui(WMA)

        #foreach m $lowpassFilters  {
        #   $f.mbType2.m add command -label $m \
        #      -command "fMRIEngineSelectLowpassForSignalModeling \{$m\}" 
        #}
        #set fMRIEngine(gui,lowpassMenuButtonForSignal) $f.mbType2
        #set fMRIEngine(gui,lowpassMenuForSignal) $f.mbType2.m
        #set fMRIEngine(curLowpassForSignal) $df

        #DevAddLabel $f.lCutoff "FWHM (s):"
        #eval {entry $f.eCutoff -width 18 -state disabled \
        #          -textvariable fMRIEngine(entry,lowpassCutoff)} $Gui(WEA)
        #set ::fMRIEngine(entry,lowpassCutoff) 0.0
        #DevAddButton $f.bLowpassCutoffHelp "?" "fMRIEngineHelpSetupLowpassFilter" 2  
        #DevAddButton $f.bLowpassDefault "use default FWHM" "fMRIEngineSelectLowpassTemporalCutoff" 2
        #blt::table $f \
        #    0,0 $f.lLowpass -padx 1 -pady 1 \
        #   0,1 $f.mbType2 -fill x -padx 1 -pady 1 \
        #  0,2 $f.bLowpassHelp -padx 1 -pady 1 \
        #    1,0 $f.lCutoff -padx 1 -pady 1 \
        #   1,1 $f.eCutoff -fill x -padx 1 -pady 1 \
        #  1,2 $f.bLowpassCutoffHelp -padx 1 -pady 1 \
        #    2,1 $f.bLowpassDefault -padx 1 -fill x -pady 1
    }
    

    #--- remove global effects
    #--- why oh why does this checkbutton show up with sunken relief???
    set f $parent.fFiltering.fGlobalEffects
    DevAddLabel $f.lIntensity "Intensity:"
    eval {checkbutton $f.cEffects \
        -variable fMRIEngine(checkbuttonGlobalEffects) \
        -relief flat -indicatoron 1 -text "remove global effects" -anchor w } $Gui(WEA) 
    DevAddButton $f.bEffectsHelp "?" "fMRIEngineHelpSetupGlobalFX" 2 
    $f.cEffects deselect 
    $f.cEffects configure -state disabled 
    blt::table $f \
        0,0 $f.lIntensity -padx 2 -pady 1 -anchor e \
        0,1 $f.cEffects -fill x -padx 1 -pady 1 -anchor e \
        0,2 $f.bEffectsHelp -padx 1 -pady 1 -anchor e


    #-----------------------
    # Custom 
    #-----------------------
    set f $parent.fMoreModeling
    frame $f.fTitle    -bg $Gui(activeWorkspace)
    frame $f.fCustom -bg $Gui(activeWorkspace) -relief groove -bd 1 
    pack $f.fTitle $f.fCustom -side top -fill x -pady 1 -padx 0

    DevAddLabel $f.fTitle.lTitle "Additional modeling:" 
    pack $f.fTitle.lTitle -side top -pady 7 -fill x

    set f $parent.fMoreModeling.fCustom
    DevAddLabel $f.lCustom "Custom:"
    eval {entry $f.eCustom -width 18 -state disabled \
        -textvariable fMRIEngine(entry,custom)} $Gui(WEA)
    DevAddButton $f.bCustomHelp "?" "fMRIEngineHelpSetupCustomFX" 2 
    blt::table $f \
        0,0 $f.lCustom -padx 1 -pady 1 -anchor e \
        0,1 $f.eCustom -fill x -padx 1 -pady 1 -anchor e \
        0,2 $f.bCustomHelp -padx 1 -pady 1 -anchor e
    

    #-------------------------------------------
    # OK frame 
    #-------------------------------------------
    set f $parent.fOK
    DevAddButton $f.bOK "add to model" "fMRIEngineAddOrEditEV" 13 
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


}




proc fMRIEngineBuildUIForModelEstimation {parent} {
    global Gui fMRIEngine
    
    frame $parent.fEstimate -bg $::Gui(activeWorkspace) -relief groove -bd 1
    pack $parent.fEstimate -side top -fill x -pady 4 -padx 1

    #-------------------------------------------
    # Estimate frame 
    #-------------------------------------------
    set f $parent.fEstimate
    frame $f.fTop      -bg $::Gui(activeWorkspace)
    #frame $f.fBot    -bg $::Gui(activeWorkspace)
    #pack $f.fTop $f.fBot -side top -fill x -pady 1 -padx 2 
    pack $f.fTop -side top -fill x -pady 1 -padx 2 

    set f $parent.fEstimate.fTop
    #--- add a title
    DevAddLabel $f.lTitle "Estimate model:" 
    #--- add a label for choosing run
    DevAddLabel $f.lRun "Choose run(s):" 
    #--- add run menu
    set runList [list {none}]
    set df [lindex $runList 0] 
    eval {menubutton $f.mbWhichRun -text $df \
        -relief raised -bd 2 -width 9 \
        -indicatoron 1 \
        -menu $f.mbWhichRun.m} $::Gui(WMBA)
#    bind $f.mbWhichRun <1> "fMRIEngineUpdateRunsForModelFitting" 
    eval {menu $f.mbWhichRun.m} $::Gui(WMA)
    TooltipAdd $f.mbWhichRun "Specify which run is going to be used for LM model fitting.\
    \nSelect 'combined' if you have multiple runs and want to use them all."
    #--- Add menu items
    foreach m $runList  {
        $f.mbWhichRun.m add command -label $m \
            -command "fMRIEngineUpdateRunsForModelFitting" 
    }
    #--- add buttons for estimating
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupEstimate" 2
    DevAddButton $f.bEstimate "Fit Model" "fMRIEngineFitModel" 15 

    set fMRIEngine(curRunForModelFitting) run1 
    # Save menubutton for config
    set fMRIEngine(gui,runListMenuButtonForModelFitting) $f.mbWhichRun
    set fMRIEngine(gui,runListMenuForModelFitting) $f.mbWhichRun.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 7 -anchor c -fill x \
        1,0 $f.lRun -padx 2 -pady 1 -anchor e \
        1,1 $f.mbWhichRun -padx 1 -pady 1 -anchor e -fill x \
        2,1 $f.bEstimate -padx 1 -pady 1 -anchor e -fill x \
        2,2 $f.bHelp -padx 1 -pady 1 -anchor e 

#    set f $parent.fEstimate.fBot
#    DevAddButton $f.bView "View Coefficients" "fMRIEngineViewCoefficients" 27 

}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineViewCoefficients
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineViewCoefficients {} {
    global fMRIEngine 


}





#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectRunForModelFitting
# 
# .ARGS
# string run
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectRunForModelFitting {run} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,runListMenuButtonForModelFitting) config -text $run
    if {$run == "combined" || $run == "none"} {
        set fMRIEngine(curRunForModelFitting) $run 
    } else {
        set r [string range $run 3 end]
        set fMRIEngine(curRunForModelFitting) $r 
    }
}




 
#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateRunsForModelFitting
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateRunsForModelFitting {} {
    global fMRIEngine 
 
    set fMRIEngine(noOfSpecifiedRuns) [$fMRIEngine(seqListBox) size] 

    $fMRIEngine(gui,runListMenuForModelFitting) delete 0 end
    set runs [$fMRIEngine(seqListBox) size] 
    if {$runs == 0} {
        fMRIEngineSelectRunForModelFitting none 
        $fMRIEngine(gui,runListMenuForModelFitting) add command -label none \
            -command "fMRIEngineSelectRunForModelFitting none"
    } else { 
        if {$runs > 1} {
            fMRIEngineSelectRunForModelFitting combined 
            $fMRIEngine(gui,runListMenuForModelFitting) add command -label combined \
                -command "fMRIEngineSelectRunForModelFitting combined"
        }

        set count 1
        while {$count <= $runs} {
            fMRIEngineSelectRunForModelFitting "run$count"
            $fMRIEngine(gui,runListMenuForModelFitting) add command -label "run$count" \
                -command "fMRIEngineSelectRunForModelFitting run$count"
            incr count
        }   
    }
}






proc fMRIEngineSelectAllConditionsForSignalModeling { } {
    global fMRIEngine
    
    if { $fMRIEngine(checkbuttonApplyAllConditions) } {
        $fMRIEngine(gui,conditionsMenuButtonForSignal) config -text "all"
        set fMRIEngine(curConditionForSignal) "all"
    } else {
        $fMRIEngine(gui,conditionsMenuButtonForSignal) config -text "none"
        set fMRIEngine(curConditionForSignal) "none"
    }
}





proc fMRIEngineSelectConditionForSignalModeling { cond } {
    global fMRIEngine 
    #--- cancel the 'allconditions' button by selecting a condition from menu
    #--- and select the newly specified appropriate condition
    if { $cond == "all"} {
        set ::fMRIEngine(checkbuttonApplyAllConditions) 1
    } else {
        set ::fMRIEngine(checkbuttonApplyAllConditions) 0
    }
    #--- which run are we talking about?
    set fMRIEngine(curConditionForSignal) $cond
    $fMRIEngine(gui,conditionsMenuButtonForSignal) config -text $cond
    set ::fMRIEngine(curRunForSignal) [ fMRIEngineGetRunForCurrentCondition ]

    #--- display the current selection for run's cosine transforms
    set r $::fMRIEngine(curRunForSignal)
    if { [info exists ::fMRIEngine(Design,Run$r,HighpassCutoff)] } {
        set ::fMRIEngine(entry,highpassCutoff) $::fMRIEngine(Design,Run$r,HighpassCutoff)
    } else {
        set ::fMRIEngine(entry,highpassCutoff) "default"
    }
}





#-------------------------------------------------------------------------------
# .PROC fMRIEngineUpdateConditionsForSignalModeling
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineUpdateConditionsForSignalModeling { } {
    global fMRIEngine 

    #--- reset the checkbox.
    set ::fMRIEngine(checkbuttonApplyAllConditions) 0
    
    #--- regenerate conditions menu and callbacks
    $fMRIEngine(gui,conditionsMenuForSignal) delete 0 end 
    set start 1
    set end $fMRIEngine(noOfRuns)
    #--- list all conditions for each run...
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
    #--- add the none and all options onto the list at the end
    if {$firstCondition == ""} {
        set firstCondition "none"
        $fMRIEngine(gui,conditionsMenuForSignal) add command -label "none" \
            -command "fMRIEngineSelectConditionForSignalModeling none"
    }
    $fMRIEngine(gui,conditionsMenuForSignal) add command -label "all" \
        -command "fMRIEngineSelectConditionForSignalModeling all"            

    set cond [$fMRIEngine(gui,conditionsMenuForSignal) entrycget 0 -label]
    fMRIEngineSelectConditionForSignalModeling $cond
}





proc fMRIEngineGetRunForCurrentCondition { } {

    #--- this sets current run as 1 if all conditions are being
    #--- modeled the same way; returns 0 if no conditions are
    #--- currently selected, and pulls the run number out of haiying'
    #--- if some condition name is selected.
    set c $::fMRIEngine(curConditionForSignal)
    if { $c == "all" } {
        return 1
    } elseif { $c == "none" } {
        return 0
    } else {
        set i [ string first "r" $::fMRIEngine(curConditionForSignal) ]
        set j [ string first ":" $::fMRIEngine(curConditionForSignal) ]
        set start [ expr $i + 1]
        set stop [ expr $j - 1]
        if { $start <= $stop } {
            set currun [ string range $::fMRIEngine(curConditionForSignal) $start $stop ]
        }
        return $currun
    }
}






#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectWaveFormForSignalModeling
# 
# .ARGS
# string form
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectWaveFormForSignalModeling {form} {
    global fMRIEngine 

    # configure menubutton
    $fMRIEngine(gui,waveFormsMenuButtonForSignal) config -text $form
    set fMRIEngine(curWaveFormForSignal) $form 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectConvolutionForSignalModeling
# 
# .ARGS
# string conv
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectConvolutionForSignalModeling {conv} {
    global fMRIEngine 

   # configure menubutton
    $fMRIEngine(gui,convolutionMenuButtonForSignal) config -text $conv
    set fMRIEngine(curConvolutionForSignal) $conv 
}



proc fMRIEngineSelectNumDerivativesForSignalModeling { option } {


    if { ($option == "none")  || ($option == 0) } {
        set ::fMRIEngine(numDerivatives) 0
    } elseif { ($option == "1st") || ($option == 1) } {
        set ::fMRIEngine(numDerivatives) 1
    } elseif { ($option == "1st+2nd") || ($option == 2) } {
        set ::fMRIEngine(numDerivatives) 2
    } else {
        set ::fMRIEngine(numDerivatives) 3        
    }
    $::fMRIEngine(gui,derivativeMenuButtonForSignal) config -text $option
    set ::fMRIEngine(curDerivativesForSignal) $::fMRIEngine(numDerivatives)
}




#-------------------------------------------------------------------------------
# .PROC fMRIEngineSelectHighpassForSignalModeling
# 
# .ARGS
# string pass
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSelectHighpassForSignalModeling {pass} {
    global fMRIEngine 

   #--- configure menubutton
    $fMRIEngine(gui,highpassMenuButtonForSignal) config -text $pass
    set fMRIEngine(curHighpassForSignal) $pass 
}




proc fMRIEngineInitDefaultHighpassTemporalCutoff { } {
    set ::fMRIEngine(entry,highpassCutoff) "default"
}



proc fMRIEngineSelectDefaultHighpassTemporalCutoff { } {

    #--- set default cutoff frequency for the chosen highpass temporal filter
    set run $::fMRIEngine(curRunForSignal)
    if { ![ info exists ::fMRIEngine(curConditionForSignal) ]} {
        DevErrorWindow "Either no runs have been defined or no condition is selected."
        set ::fMRIEngine(entry,highpassCutoff) "default"
        return
    }
    if { $run <= 0 } {
        DevErrorWindow "No runs have been defined yet!"
        #--- reset widget to default state
        set ::fMRIEngine(entry,highpassCutoff) "default"
        return
    }
    set ::fMRIEngine(entry,highpassCutoff) "default"
    if { $run != 0 } {
        set ::fMRIEngine(Design,Run$run,useCustomCutoff) 0
        set ::fMRIEngine(Design,Run$run,HighpassCutoff) "default"
    }
}



proc fMRIEngineSelectCustomHighpassTemporalCutoff { } {

    set run $::fMRIEngine(curRunForSignal)
    if { $run <= 0 } {
        DevErrorWindow "No runs have been defined yet!"
        #--- reset widget to default state
        set ::fMRIEngine(entry,highpassCutoff) "default"
        return
    }
    if { ($::fMRIEngine(entry,highpassCutoff) == "default") && ($::fMRIEngine(Design,Run$run,useCustomCutoff)) } {
        DevErrorWindow "Please click button to select default cutoff."
        return
    }
    #--- check to see if it's a valid entry....
    if { $::fMRIEngine(entry,highpassCutoff) != "default" } {
        set tst $::fMRIEngine(entry,highpassCutoff)
        if { (! [ string is integer -strict $tst]) && (! [ string is double -strict $tst]) } {
            DevErrorWindow "Cutoff period must be either an integer or floating point value"
            fMRIEngineSelectDefaultHighpassTemporalCutoff 
            return
        }
        if { $tst == 0 || $tst == 0.0 } {
            DevErrorWindow "Cutoff period should be non-zero."
            fMRIEngineSelectDefaultHighpassTemporalCutoff 
            set $::fMRIEngine(Design,Run$run,useCustomCutoff) 0
            return
        }
        #--- set custom cutoff frequency for the chosen highpass temporal filter
        if { $run != 0 } {
            set ::fMRIEngine(Design,Run$run,useCustomCutoff) 1
            set ::fMRIEngine(Design,Run$run,HighpassCutoff)  $::fMRIEngine(entry,highpassCutoff) 
        }
    }
}




proc fMRIEngineComputeDefaultHighpassTemporalCutoff { r } {

    #--- Here's how the default cutoff frequency is computed:
    #--- fmin is the cutoff frequency of the high-pass filter (lowest frequency
    #--- that we let pass through. Choose to let fmin = 0.666666/T (just less than the
    #--- lowest frequency in paradigm). As recommended in S.M. Smith, "Preparing fMRI
    #--- data for statistical analysis, in 'Functional MRI, an introduction to methods', Jezzard,
    #--- Matthews, Smith Eds. 2002, Oxford University Press.

    if { [ fMRIModelViewLongestEpochSpacing $r ] } {
        set T $::fMRIModelView(Design,Run$r,longestEpoch)
        #--- set the model parameter, cutoff Period
        set ::fMRIEngine(Design,Run$r,HighpassCutoff)  [ expr 1.5 * $T ]
        set ::fMRIEngine(entry,highpassCutoff) $::fMRIEngine(Design,Run$r,HighpassCutoff)
        #--- update the GUI
        #set ::fMRIEngine(entry,highpassCutoff) $::fMRIEngine(Design,Run$r,HighpassCutoff)
    } else {
        #--- set the model parameter
        set ::fMRIEngine(Design,Run$r,HighpassCutoff) 0.0
        #--- update the GUI
        #set ::fMRIEngine(entry,highpassCutoff) 0.0
    }

}



#-------------------------------------------------------------------------------
proc fMRIEngineSelectLowpassForSignalModeling {pass} {
    global fMRIEngine 

   #--- configure menubutton
   # configure menubutton
    $fMRIEngine(gui,lowpassMenuButtonForSignal) config -text $pass
    set fMRIEngine(curLowpassForSignal) $pass
    fMRIEngineSelectLowpassTemporalCutoff
}




proc fMRIEngineSelectLowpassTemporalCutoff { } {
    global fMRIEngine 

    #--- set default cutoff frequency for the lowpass temporal filter
    if { [ info exists ::fMRIModelView(Design,Run1,TR) ] } {
        set ::fMRIModelView(Design,LowpassCutoff) $::fMRIModelView(Design,Run1,TR)
    } else {
        DevErrorWindow "Lowpass: Specify run(s) and condition(s) before modeling."
        set ::fMRIModelView(Design,LowpassCutoff) 0.0
    }
}






#-------------------------------------------------------------------------------
# .PROC fMRIEngineAddOrEditEV
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAddOrEditEV {} {
    global fMRIEngine 

    set con $fMRIEngine(curConditionForSignal)
    if { ($con == "none") && (!$fMRIEngine(checkbuttonApplyAllConditions)) } {
        DevErrorWindow "Select a valid condition."
        return
    }
    #--- wjp 11/02/05
    #--- if current condition is "all", then just pick the first in the list.
    if {$con == "all"} {
        set conditiontitle [lindex $fMRIEngine($0,conditionList) $count]
        set con "r$i:$title"
    }

    set wform $fMRIEngine(curWaveFormForSignal) 
    set conv  $fMRIEngine(curConvolutionForSignal) 
    
    #--- wjp 09/01/06
    set deriv $fMRIEngine(curDerivativesForSignal)
    #set deriv $fMRIEngine(checkbuttonTempDerivative) 
    set hpass $fMRIEngine(curHighpassForSignal)
    #set lpass $fMRIEngine(curLowpassForSignal) 
    set effes $fMRIEngine(checkbuttonGlobalEffects) 
    #set ev "$con:$wform:$conv:$deriv:$hpass:$lpass:$effes"
    set ev "$con:$wform:$conv:$deriv:$hpass:$effes"
    if {[info exists fMRIEngine($ev,ev)]} {
        DevErrorWindow "This EV already exists:\n$ev"
        if {! $fMRIEngine(checkbuttonApplyAllConditions)} {
            return
        }
    }

    #--- Deleting evs from the evlistbox
    #--- that use the same condition...
    set i 0
    set found -1 
    set index -1
    set size [$fMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set v [$fMRIEngine(evsListBox) get $i] 
        if {$v != ""} {
            if {! $fMRIEngine(checkbuttonApplyAllConditions)} {
                # If an EV exists with the same conditon, remove it
                set found [string first $con $v]
                if {$found >= 0} {
                    fMRIEngineDeleteEV $i
                    break
                }
            } else {
                set found [string first "baseline" $v]
                if {$found == -1} {
                    $fMRIEngine(evsListBox) delete $i end 
                    break
                }
            }
        } 
        incr i
    }

    # add EVs
    set j 0
    set end [$fMRIEngine(gui,conditionsMenuForSignal) index end] 
    while {$j <= $end} {  
        set v [$fMRIEngine(gui,conditionsMenuForSignal) entrycget $j -label] 
        if {$v != ""} {
            set i 1 
            set i2 [string first ":" $v]
            set run [string range $v $i [expr $i2-1]] 
            set run [string trim $run]
            set title [string range $v [expr $i2+1] end] 
            set title [string trim $title]

            #set ev "$v:$wform:$conv:$deriv:$hpass:$lpass:$effes"
            set ev "$v:$wform:$conv:$deriv:$hpass:$effes"

            if {$fMRIEngine(checkbuttonApplyAllConditions) ||
                ((! $fMRIEngine(checkbuttonApplyAllConditions)) && $con == $v)} {

                set fMRIEngine($ev,ev)               $ev
                set fMRIEngine($ev,run)              $run
                set fMRIEngine($ev,title,ev)         $title
                set fMRIEngine($ev,condition,ev)     $v
                set fMRIEngine($ev,waveform,ev)      $wform
                set fMRIEngine($ev,convolution,ev)   $conv
                set fMRIEngine($ev,derivative,ev)    $deriv
                set fMRIEngine($ev,highpass,ev)      $hpass
                #set fMRIEngine($ev,lowpass,ev)       $lpass
                set fMRIEngine($ev,globaleffects,ev) $effes

                $fMRIEngine(evsListBox) insert end $ev
            }
        } 

        incr j 
    }
} 


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDeleteEV
# 
# .ARGS
# int index defaults to -1
# .END
#-------------------------------------------------------------------------------
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
            #unset -nocomplain fMRIEngine($ev,lowpass,ev) 
            unset -nocomplain fMRIEngine($ev,globaleffects,ev)
        }

        $fMRIEngine(evsListBox) delete $curs 
    } else {
        DevErrorWindow "Select an EV to delete."
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowEVToEdit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
            #fMRIEngineSelectNumDerivativesForSignalModeling $fMRIEngine($ev,derivative,ev)
            set m [ lindex $::fMRIEngine(derivOptions) $::fMRIEngine($ev,derivative,ev) ]
            fMRIEngineSelectNumDerivativesForSignalModeling $m
            fMRIEngineSelectHighpassForSignalModeling    $fMRIEngine($ev,highpass,ev) 
            #fMRIEngineSelectLowpassForSignalModeling     $fMRIEngine($ev,lowpass,ev) 
            #--- wjp 09/06/05
            set fMRIEngine(numDerivatives) $fMRIEngine($ev,derivative,ev)
            #set fMRIEngine(checkbuttonTempDerivative) $fMRIEngine($ev,derivative,ev)
            set fMRIEngine(checkbuttonGlobalEffects)  $fMRIEngine($ev,globaleffects,ev) 
        }
    } else {
        DevErrorWindow "Select an EV to edit." 
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineAddInputVolumes
# 
# .ARGS
# string run
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAddInputVolumes {run} {
    global MultiVolumeReader fMRIEngine

    if {$run == "none"} {
        return
   }

    set start $run
    set last $run
    if {$run == "combined"} {
        set start 1
        set last [$fMRIEngine(seqListBox) size] 
    }

    set fMRIEngine(totalVolsForModelFitting) 0
    for {set r $start} {$r <= $last} {incr r} { 
        set seqName $fMRIEngine($r,sequenceName)
        set id $MultiVolumeReader($seqName,firstMRMLid)
        set id2 $MultiVolumeReader($seqName,lastMRMLid)
        # puts "id = $id"
        # puts "id2 = $id2"
        while {$id <= $id2} {
            Volume($id,vol) Update
            fMRIEngine(actEstimator) AddInput [Volume($id,vol) GetOutput]
            incr fMRIEngine(totalVolsForModelFitting)
 
            incr id
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCheckMultiRuns
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCheckMultiRuns {} {
    global fMRIEngine

    # check number of condition EVs
    for {set r 2} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        if {$fMRIEngine($r,noOfEVs) != $fMRIEngine(1,noOfEVs)} {
            DevErrorWindow "Run1 and run$r are not equal in number of condition EVs."
            return 1
        }
    }

    # check types of condition EVs
    for {set r 2} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        foreach n1 $fMRIEngine(1,namesOfEVs) \
                n2 $fMRIEngine($r,namesOfEVs) {
            if {! [string equal -nocase $n1 $n2]} {
                DevErrorWindow "Run1 and run$r don't have the same types of condition EVs."
                return 1
            }
        }
    }

    # check baseline for each run 
    for {set r 2} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        if {$::fMRIModelView(Design,Run1,UseBaseline) !=  
            $::fMRIModelView(Design,Run$r,UseBaseline)} {
            DevErrorWindow "Run1 and run$r are different in baseline modeling."
            return 1
        }
    }

    # check baseline for each run 
    for {set r 2} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        if {$::fMRIModelView(Design,Run1,UseDCBasis) !=  
            $::fMRIModelView(Design,Run$r,UseDCBasis)} {
            DevErrorWindow "Run1 and run$r are different in DCBasis modeling."
            return 1
        }
    }

    return 0
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineAddRegressors
# 
# .ARGS
# string run
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAddRegressors {run} {
    global MultiVolumeReader fMRIEngine fMRIModelView 

    if {$run == "none"} {
        return 1
    }

    if {[info commands fMRIEngine(designMatrix)] != ""} {
        fMRIEngine(designMatrix) Delete
        unset -nocomplain fMRIEngine(designMatrix)
    }
    vtkFloatArray fMRIEngine(designMatrix)

    #--- Additional EVs: baseline and DCBasis for each run...
    for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        set fMRIEngine($r,totalEVs) [expr $fMRIEngine($r,noOfEVs)]
        if {$::fMRIModelView(Design,Run$r,UseBaseline)} {
            incr fMRIEngine($r,totalEVs)
        }
        if {$::fMRIModelView(Design,Run$r,UseDCBasis)} {
            set fMRIEngine($r,totalEVs) [expr $fMRIEngine($r,totalEVs) + \
                                            $::fMRIEngine(Design,Run$r,numCosines) ]
        }
    }
    #--- if runs are being analyzed separately...
    if {$run != "combined"} {
        # single run
        set ::fMRIEngine($run,totalEVs) [ expr int ($::fMRIEngine($run,totalEVs)) ]
        fMRIEngine(designMatrix) SetNumberOfComponents $fMRIEngine($run,totalEVs)
        set seqName $fMRIEngine($run,sequenceName)
        set vols $MultiVolumeReader($seqName,noOfVolumes) 
        fMRIEngine(designMatrix) SetNumberOfTuples $vols

        for {set j 0} {$j < $vols} {incr j} { 
            for {set i 0} {$i < $fMRIEngine($run,totalEVs)} {incr i} { 
                set index [expr $i+1]
                set data $fMRIModelView(Data,Run$run,EV$index,EVData)
                set e [lindex $data $j]
                fMRIEngine(designMatrix) InsertComponent $j $i $e 
            }
        }
    } else {
        # runs are being combined together and analyzed together.
        if {[fMRIEngineCheckMultiRuns] == 1} {
            return 1
        }

        set vols 0
        for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} { 
            set seqName $fMRIEngine($r,sequenceName)
            set vols [expr $MultiVolumeReader($seqName,noOfVolumes) + $vols]
        }

        fMRIEngine(designMatrix) SetNumberOfTuples $vols 
        fMRIEngine(designMatrix) SetNumberOfComponents $fMRIEngine(1,totalEVs) 

        for {set i 1} {$i <= $fMRIEngine(1,totalEVs)} {incr i} { 
            set data ""
            for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} { 
                set data [concat $data $fMRIModelView(Data,Run$r,EV$i,EVData)]
            }
            set fMRIEngine($i,combinedEVs) $data 
        }

        for {set j 0} {$j < $vols} {incr j} { 
            for {set i 0} {$i < $fMRIEngine(1,totalEVs)} {incr i} { 
                set index [expr $i+1]
                set data $fMRIEngine($index,combinedEVs)
                set e [lindex $data $j]
                fMRIEngine(designMatrix) InsertComponent $j $i $e 
            }
        }
    }

    return 0
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineCountEVs
# Counts real EVs for each run
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineCountEVs {} {
    global fMRIEngine

    #--- wjp 09/02/05 changed derivative modeling
    # cleaning up
    for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} { 
        set ::fMRIModelView(Design,Run$r,UseBaseline) 0
        set ::fMRIModelView(Design,Run$r,UseDCBasis) 0 
        unset -nocomplain fMRIEngine($r,noOfEVs)
        unset -nocomplain fMRIEngine($r,namesOfEVs)
        unset -nocomplain fMRIEngine($r,namesOfConditionEVs)
    }

    # how many real (not including baseline and DCBasis) evs for each run
    set i 0
    set size [$fMRIEngine(evsListBox) size]
    while {$i < $size} {  
        set ev [$fMRIEngine(evsListBox) get $i]
        unset -nocomplain namelist
        if {$ev != ""} {
            set found [string first "baseline" $ev]
            if {$found >= 0} {
                # baseline ev
                set i1 1 
                set i2 [string first ":" $ev]
                set r [string range $ev $i1 [expr $i2-1]] 
                set r [string trim $r]
                set ::fMRIModelView(Design,Run$r,UseBaseline) 1
            } else {
                set run $fMRIEngine($ev,run)
                set wform $fMRIEngine($ev,waveform,ev)   
                set conv  $fMRIEngine($ev,convolution,ev)
                set deriv $fMRIEngine($ev,derivative,ev)
                set hpass $fMRIEngine($ev,highpass,ev)
                set title $fMRIEngine($ev,title,ev)
                set mycon [ lsearch $::fMRIEngine($run,conditionList) $title ]
                incr mycon
                
                #--- wjp 09/02/05
                #--- accrue names of EVs for each run inside each
                #--- individual case; and count up number of EVs
                #--- for each run inside each case too. Made this change
                #--- because adding derivatives ADDS more EVs
                #--- instead of REPLACING the original paradigm signal,
                #--- which is how we mistakenly implemented it at first.
                #--- EV signal type: tedious, but try it for now...
                #--- First, if we're NOT using temporal derivatives in modeling...
                if { $deriv == 0 } {
                    if {$conv == "none"} {
                        if {$wform == "Box Car"} {
                            set wf "boxcar"
                        } else {
                            set wf "halfsine"
                        }
                    } else {
                        if {$wform == "Box Car"} {
                            set wf "boxcar_cHRF"
                        } else {
                            set wf "halfsine_cHRF"
                        }
                    }
                    #--- append name, count ev, and set signal type
                    lappend namelist $title
                    #--- add the namelist to fMRIEngine($r,namesOfEVs)
                    fMRIEngineAppendEVNamesToRun $run $namelist
                    fMRIEngineIncrementEVCountForRun $run 1
                    set fMRIEngine($run,$title,signalType) $wf                    
                    set ::fMRIEngine($run,$title,myCondition) $mycon
                } else { 
                    #--- Now if we ARE using temporal derivatives in modeling
                    if {$conv == "none"} {
                        if {$wform == "Box Car"} {
                            set base "boxcar"
                            if { $deriv == 1 } {
                                set wf "boxcar_dt1"
                                #--- append names, count ev, and set signal type
                                lappend namelist $title ${title}_dt1
                                set numevs 2
                            } elseif { $deriv == 2 } {
                                set wf "boxcar_dt2"
                                lappend namelist $title ${title}_dt1 ${title}_dt2
                                set numevs 3
                            } elseif { $deriv == 3 } {
                                set wf "boxcar _dt3"
                                lappend namelist $title ${title}_dt1 ${title}_dt2 ${title}_dt3
                                set numevs 4
                            }
                        } else {
                            set base "halfsine"
                            if { $deriv == 1 } {
                                set wf "halfsine_dt1"
                                lappend namelist $title ${title}_dt1
                                set numevs 2
                            } elseif { $deriv == 2 } {
                                set wf "halfsine_dt2"
                                lappend namelist $title ${title}_dt1 ${title}_dt2
                                set numevs 3
                            } elseif { $deriv == 3 } {
                                set wf "halfsine_dt3"
                                lappend namelist $title ${title}_dt1 ${title}_dt2 ${title}_dt3
                                set numevs 4
                            }
                        }
                    } else {
                        if {$wform == "Box Car"} {
                            set base "boxcar_cHRF"
                            if { $deriv == 1 } {
                                set wf "boxcar_cHRF_dt1"
                                lappend namelist $title ${title}_dt1
                                set numevs 2
                            } elseif { $deriv == 2 } {
                                set wf "boxcar_cHRF_dt2"
                                lappend namelist $title ${title}_dt1 ${title}_dt2
                                set numevs 3
                            } elseif { $deriv == 3 } {
                                set wf "boxcar _cHRF_dt3"
                                lappend namelist $title ${title}_dt1 ${title}_dt2 ${title}_dt3
                                set numevs 4
                            }
                        } else {
                            set base "halfsine_cHRF"
                            if { $deriv == 1 } {
                                set wf "halfsine_cHRF_dt1"
                                lappend namelist $title ${title}_dt1
                                set numevs 2
                            } elseif { $deriv == 2 } {
                                set wf "halfsine_cHRF_dt2"
                                lappend namelist $title ${title}_dt1 ${title}_dt2
                                set numevs 3
                            } elseif { $deriv == 3 } {
                                set wf "halfsine_cHRF_dt3"
                                lappend namelist $title ${title}_dt1 ${title}_dt2 ${title}_dt3
                                set numevs 4
                            }
                        }
                    }
                    fMRIEngineAppendEVNamesToRun $run $namelist
                    fMRIEngineIncrementEVCountForRun $run $numevs
                    set ::fMRIEngine($run,$title,myCondition) $mycon                    
                    fMRIEngineAddDerivativeSignalsToRun  $run $title $base $deriv $mycon
                }
                # DCBasis
                if {$hpass == "Discrete Cosine"} {
                    set ::fMRIModelView(Design,Run$run,UseDCBasis) 1
                    #---WJP: 09/15/05
                    #lappend fMRIEngine($run,namesOfEVs) $title
                    #if {! [info exists fMRIEngine($run,noOfEVs)]} {
                    #    set fMRIEngine($run,noOfEVs) 1
                    #} else {
                    #    incr fMRIEngine($run,noOfEVs) 
                    #}
                }
            }
        }
        incr i
    }
    #--- Re-order the name lists of all runs to match the order of EVs
    #--- in the first run. This organizes the design matrix so that
    #--- appropriate EVs from each run are combined in analysis.
    for {set r 2} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} { 
        unset -nocomplain names
        foreach name $fMRIEngine(1,namesOfEVs) {
            set found [lsearch -exact $fMRIEngine($r,namesOfEVs) $name]
            if {$found >= 0} {
                lappend names $name
                # delete it from the list of run r
                set fMRIEngine($r,namesOfEVs) \
                    [lreplace $fMRIEngine($r,namesOfEVs) $found $found]
            }
        }
        if { ! [ info exists names] } {
            DevErrorWindow "Runs can't be combined; EV names across runs may differ."
            return 0
        } else {
            set fMRIEngine($r,namesOfEVs) [concat $names $fMRIEngine($r,namesOfEVs)]
        }
    }

    #--- wjp add: this lists EVs that are associated only with conditions
    for {set r 1} {$r <= $fMRIEngine(noOfSpecifiedRuns)} {incr r} {
        foreach name $::fMRIEngine($r,namesOfEVs) {
            set deriv [ string first "_dt" $name ]
            set bline [ string first "baseline" $name ]
            set basis [ string first "DCbasis" $name ]
            if { ($deriv < 0 ) && ( $bline < 0 ) && ( $basis < 0 ) } {
                lappend ::fMRIEngine($r,namesOfConditionEVs) $name
            }
        }
    }
    return 1
}






proc  fMRIEngineAppendEVNamesToRun { run namelist } {

    # Append list of  names of EVs for each run.
    foreach n $namelist {
        lappend ::fMRIEngine($run,namesOfEVs) $n
    }
}





proc  fMRIEngineIncrementEVCountForRun { run numToAdd } {

    # Count number of EVs for each run
    if {! [info exists ::fMRIEngine($run,noOfEVs)]} {
        set ::fMRIEngine($run,noOfEVs) $numToAdd
    } else {
        set ::fMRIEngine($run,noOfEVs) [expr $::fMRIEngine($run,noOfEVs) + $numToAdd ]
    }
}





proc fMRIEngineAddDerivativeSignalsToRun { run title base derivnum condition } {

    #--- Recording the signal type of each derivative signal and
    #--- associating these derivative signals (used in the linear regression)
    #--- with the condition for which they are modeling latency.
    #--- We do this because the derivative signals are generated by
    #--- first building the waveform with appropriately specified onsets and
    #--- durations, and THEN taking the derivative(s) of it.
    set ::fMRIEngine($run,$title,signalType) $base
    if { $derivnum > 0 } {
        set ::fMRIEngine($run,${title}_dt1,signalType) "${base}_dt1"
        set ::fMRIEngine($run,${title}_dt1,myCondition) $condition
    }
    if {$derivnum > 1 } {
        set ::fMRIEngine($run,${title}_dt2,signalType) "${base}_dt2"
        set ::fMRIEngine($run,${title}_dt2,myCondition) $condition
    }
    if {$derivnum > 2 } {
        set ::fMRIEngine($run,${title}_dt3,signalType) "${base}_dt3"
        set ::fMRIEngine($run,${title}_dt3,myCondition) $condition
    }
}





#-------------------------------------------------------------------------------
# .PROC fMRIEngineFitModel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineFitModel {} {
    global fMRIEngine Gui Volume MultiVolumeReader

    if {$fMRIEngine(curRunForModelFitting) == "none"} {
        DevErrorWindow "Select a valid run for model fitting."
        return 
    }

    if {$fMRIEngine(noOfSpecifiedRuns) == 0} {
        DevErrorWindow "No run has been specified."
        return
    }

    if { ! [ fMRIEngineCountEVs] } {
        return
    }

    set start $fMRIEngine(curRunForModelFitting)
    set last $start
    if {$start == "combined"} {
        set start 1
        set last $fMRIEngine(noOfSpecifiedRuns)
    }
    for {set r $start} {$r <= $last} {incr r} { 
        if {! [info exists fMRIEngine($r,noOfEVs)]} {
            DevErrorWindow "Complete signal modeling first for run$r."
            return
        }
    }

    # generates data without popping up the model image 
    set done [fMRIModelViewGenerateModel]
    if {! $done} {
        DevErrorWindow "Error in generating model for model fitting."
        puts "Error in generating model for model fitting."
        return 
    }

    # always uses a new instance of vtkActivationEstimator 
    if {[info commands fMRIEngine(actEstimator)] != ""} {
        fMRIEngine(actEstimator) Delete
        unset -nocomplain fMRIEngine(actEstimator)
    }
    vtkActivationEstimator fMRIEngine(actEstimator)
    fMRIEngineAddInputVolumes $fMRIEngine(curRunForModelFitting)
 
    # adds progress bar
    set obs1 [fMRIEngine(actEstimator) AddObserver StartEvent MainStartProgress]
    set obs2 [fMRIEngine(actEstimator) AddObserver ProgressEvent \
              "MainShowProgress fMRIEngine(actEstimator)"]
    set obs3 [fMRIEngine(actEstimator) AddObserver EndEvent MainEndProgress]
    if {$fMRIEngine(curRunForModelFitting) == "combined"} {
        set Gui(progressText) "Estimating all runs..."
    } else {
        set Gui(progressText) "Estimating run$fMRIEngine(curRunForModelFitting)..."
    }
    puts $Gui(progressText)
 
    # always uses a new instance of vtkActivationDetector
    if {[info commands fMRIEngine(detector)] != ""} {
        fMRIEngine(detector) Delete
        unset -nocomplain fMRIEngine(detector)
    }
    vtkActivationDetector fMRIEngine(detector)

    set rt [fMRIEngineAddRegressors $fMRIEngine(curRunForModelFitting)]
    if {$rt == 1} {
        puts "...failed"
        return 
    }

    fMRIEngine(detector) SetDetectionMethod 1
    fMRIEngine(detector) SetDesignMatrix fMRIEngine(designMatrix) 
    if {[info exists fMRIEngine(lowerThreshold)]} {
        fMRIEngine(actEstimator) SetLowerThreshold $fMRIEngine(lowerThreshold)
    }
    fMRIEngine(actEstimator) SetDetector fMRIEngine(detector)  
    fMRIEngine(actEstimator) Update
    set fMRIEngine(actBetaAndStd) [fMRIEngine(actEstimator) GetOutput]

    fMRIEngine(actEstimator) RemoveObserver $obs1 
    fMRIEngine(actEstimator) RemoveObserver $obs2 
    fMRIEngine(actEstimator) RemoveObserver $obs3 

    puts "...done"
}
