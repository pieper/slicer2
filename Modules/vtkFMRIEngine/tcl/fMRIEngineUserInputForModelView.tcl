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
# FILE:        fMRIEngineUserInputForModelView.tcl
# PROCEDURES:  
#==========================================================================auto=
proc fMRIModelViewSortUserInput { } {
    global fMRIEngine
    #---
    #--- numRuns should be set to 1 by default; changed only
    #--- when user wants to analyze several runs at once:
    #--- When numRuns > 1, we must determine whether
    #--- all runs have the same conditions or whether 
    #--- conditions vary in some way from run to run
    #--- (Maybe their onsets, durations or intensities differ...)
    #--- When conditions are identical across runs,
    #--- we set fMRIModelView(Design,identicalRuns) = 1;
    #--- otherwise zero.
    #---
    set ::fMRIModelView(Design,numRuns) $fMRIEngine(noOfRuns) 
    set ::fMRIModelView(Design,identicalRuns) $fMRIEngine(checkbuttonRunIdentical)
    set ::fMRIModelView(Layout,NoDisplay) 0

    #---
    #--- Design types supported: blocked, event-related, or mixed
    #--- default is 'blocked'
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,Type) $fMRIEngine($r,designType) 
    }
    #--- scan interval in seconds.
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,TR) $fMRIEngine($r,tr) 
    }
    #--- fraction-of-second increment used in modeling
    #--- please keep the product ($::fMRIModelView(Design,Run$r,TR) *
    #--- $::fMRIModelView(Design,Run$r,TimeIncrement) an integer value.
    #--- otherwise, the subsampling routine will break.
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,TimeIncrement) 1.0
    }

    #---
    #--- Specify these Onsets and Durations lists for each user-specified condition.
    #--- BLOCKED DESIGN requires both lists for each condition;
    #--- MIXED DESIGN Duration list sets duration=0 for event durations.
    #--- and purely EVENT-RELATED DESIGNS have no Duration lists.
    #--- Assumes onsets and durations are specified in numbers of scans:
    #--- an onset of 10 means 'onset at the 10th scan'
    #--- and a duration of 10 means 'lasting for 10 scans'.
    #--- **if onsets are typically specified in seconds, have to change this.
    #--- For parametric designs, specify Condition intensity too.
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        if {[info exists fMRIEngine($r,conditionList)]} {  
            set len [llength $fMRIEngine($r,conditionList)]
            set ::fMRIModelView(Design,Run$r,numConditions) $len

            set i 0
            while {$i < $len} {
                set title [lindex $fMRIEngine($r,conditionList) $i]
                set indx [expr $i+1]

                # Intensities
                set onsetsStr $fMRIEngine($r,$title,onsets)
                # trim white spaces at beginning and end
                set onsetsStr [string trim $onsetsStr]
                # replace multiple spaces in the middle of the string by one space  
                regsub -all {( )+} $onsetsStr " " onsetsStr 

                set onsets [split $onsetsStr " "]     
                set l [llength $onsets]
                for {set j 0} {$j < $l} {incr j} {
                    lappend intensities
                } 
                fMRIModelViewSetConditionIntensities $r $indx $intensities 

                fMRIModelViewSetConditionOnsets $r $indx $onsets 

                set dursStr $fMRIEngine($r,$title,durations)
                # trim white spaces at beginning and end
                set dursStr [string trim $dursStr]
                # replace multiple spaces in the middle of the string by one space  
                regsub -all {( )+} $dursStr " " dursStr 

                set durs [split $dursStr " "]     
                fMRIModelViewSetConditionDurations $r $indx $durs 

                incr i 
            }
        }
    }

    #--- numEVs derived from conditions.
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set len [llength $fMRIEngine($r,conditionList)]
        set ::fMRIModelView(Design,Run$r,numConditionEVs) $len 
        set ::fMRIModelView(Design,Run$r,numAdditionalEVs) 0
    }


    #---
    #--- Additional EVs for detrending? User specifies:
    # by default, ev for baseline is off
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,UseBaseline) 0
        set ::fMRIModelView(Design,Run$r,UseDCBasis) 0 
        set ::fMRIModelView(Design,Run$r,UsePolyBasis) 0
        set ::fMRIModelView(Design,Run$r,UseSplineBasis) 0
        set ::fMRIModelView(Design,Run$r,UseExploratoryBasis) 0
    }
 
    #---    
    #--- Regular signal types used for condition-derived EVs:
    #--- boxcar,
    #--- boxcar_dt,
    #--- boxcar_cHRF_dt,
    #--- boxcar_cHRF,
    #--- halfsine,
    #--- halfsine_dt,
    #--- halfsine_cHRF_dt,
    #--- halfsine_cHRF,
    #--- linearEffect, and
    #--- quadraticEffect.
    #--- These variables get generated during signal modeling
    #--- of conditions, and selection of additional EVs.
    #--- So here, ::fMRIModelView(Design,EV1,SignalType)
    #--- corresponds to condition1.
    set size [$fMRIEngine(evsListBox) size]
    set i 0
    while {$i < $size} {
        set ev [$fMRIEngine(evsListBox) get $i]
        if {$ev != ""} {
            set found [string first "baseline" $ev 0]
            if {$found >= 0} {
                # baseline ev
                set i1 1 
                set i2 [string first ":" $ev]
                set r [string range $ev $i1 [expr $i2-1]] 
                set r [string trim $r]
                set ::fMRIModelView(Design,Run$r,UseBaseline) 1
            } else {
                set run $fMRIEngine($ev,run)
                if {[info exists fMRIEngine($run,noOfEVs)]} {
                    incr fMRIEngine($run,noOfEVs)
                } else {
                    set fMRIEngine($run,noOfEVs) 1
                }

                set wform $fMRIEngine($ev,waveform,ev)   
                set conv  $fMRIEngine($ev,convolution,ev)
                set deriv $fMRIEngine($ev,derivative,ev)
                set hpass $fMRIEngine($ev,highpass,ev)
                set title $fMRIEngine($ev,title,ev)

                if {$hpass == "Discrete Cosine Set"} {
                    set ::fMRIModelView(Design,Run$run,UseDCBasis) 1
                }

                if {$deriv} {
                    if {$conv == "none"} {
                        if {$wform == "Box Car"} {
                            set wf "boxcar_dt"
                        } else {
                            set wf "halfsine_dt"
                        }
                    } else {
                        if {$wform == "Box Car"} {
                            set wf "boxcar_cHRF_dt"
                        } else {
                            set wf "halfsine_cHRF_dt"
                        }
                    }
                } else {
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
                }

                fMRIModelViewSetEVSignalType $run $fMRIEngine($run,noOfEVs) $wf 
                fMRIModelViewAddConditionName $run $fMRIEngine($run,noOfEVs) $title 
            }
        }
         
        incr i
    } 

    for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} { 
        unset -nocomplain fMRIEngine($r,noOfEVs) 
        unset -nocomplain fMRIEngine($r,noOfEVs) 
        unset -nocomplain fMRIEngine($r,noOfEVs) 
    }
 
    #--- compute totalEVs across runs.
    set sum 0
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set sum [ expr $sum + $::fMRIModelView(Design,Run$r,numConditionEVs) \
                      + $::fMRIModelView(Design,Run$r,numAdditionalEVs) ]
    }
    set ::fMRIModelView(Design,totalEVs) $sum
    
    #---
    #--- Additional EV: use for modeling baseline
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        if { $::fMRIModelView(Design,Run$r,UseBaseline) } {
            set j [ expr $::fMRIModelView(Design,Run$r,numConditionEVs) + 1 ]
            fMRIModelViewSetEVSignalType $r $j "baseline"
            #--- have added another ev, so:
            #--- increment the number of additional EVs,
            incr ::fMRIModelView(Design,Run$r,numAdditionalEVs)
        }
    }
    #--- update totalEVs
    set sum 0
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set sum [ expr $sum + $::fMRIModelView(Design,Run$r,numConditionEVs) \
                      + $::fMRIModelView(Design,Run$r,numAdditionalEVs) ]
    }
    set ::fMRIModelView(Design,totalEVs) $sum


    #---    
    #--- for now, create a set of 7 of these basis functions
    #--- with fixed frequencies. But, probably want to query
    #--- user for a cutoff frequency, below which to span with
    #--- appropriate number of basis funcs.
    set numDCbasis 7
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set j [ expr $::fMRIModelView(Design,Run$r,numConditionEVs) + \
                    $::fMRIModelView(Design,Run$r,numAdditionalEVs) + 1 ]
        if { $::fMRIModelView(Design,Run$r,UseDCBasis) } {
            for { set b 0 } { $b < $numDCbasis } { incr b } {
                fMRIModelViewSetEVSignalType $r $j "DCbasis$b"
                #--- have added another ev, so:
                #--- increment the number of additional EVs,
                incr ::fMRIModelView(Design,Run$r,numAdditionalEVs)            
                incr j
            }
        }
    }
    #--- update totalEVs
    set sum 0
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set sum [ expr $sum + $::fMRIModelView(Design,Run$r,numConditionEVs) \
                      + $::fMRIModelView(Design,Run$r,numAdditionalEVs) ]
    }
    set ::fMRIModelView(Design,totalEVs) $sum

    #---
    #--- assume that t-contrasts will be available in vector form
    set size [$fMRIEngine(contrastsListBox) size]
    set ::fMRIModelView(Design,numTContrasts) $size 
    unset -nocomplain ::fMRIModelView(Design,TContrastNames)
    unset -nocomplain ::fMRIModelView(Design,TContrasts)
 
    set i 0
    while {$i < $size} {
        set name [$fMRIEngine(contrastsListBox) get $i]
        if {$name != ""} {
            set c $fMRIEngine($name,contrastVector)
            # trim white spaces at beginning and end
            set c [string trim $c]
            # replace multiple spaces in the middle of the string by one space  
            regsub -all {( )+} $c " " c 

            set l [split $c " "]
            set noOfEVs [expr $::fMRIModelView(Design,totalEVs) / $::fMRIModelView(Design,numRuns)]
            set len [llength $l]
            if {$len > $noOfEVs} {
                set l [lrange $l 0 [expr $noOfEVs-1]]
            } elseif {$len < $noOfEVs} {
                for {set j $len} {$j < $noOfEVs} {incr j} {
                    lappend l 0
                }
            } else {
            }
        
            set contrast $l 
            for {set r 1} {$r < $::fMRIModelView(Design,numRuns)} {incr r} { 
                set contrast [concat $contrast $l] 
            }

            set index [expr $i+1]
            fMRIModelViewSetTContrast $index $contrast
            fMRIModelViewSetTContrastName $index t-$name 
            fMRIModelViewSetTContrastLabel $index
        }

        incr i
    }

    #---    
    #--- these auto-generated titles & labels of EVs derived from
    #--- user-specified conditions in each run, and the additional
    #--- requested regressors.
    if { $::fMRIModelView(Design,identicalRuns) } {
        #--- copy EVnames derived from user-specified conditions thru
        #--- signal modeling, and additional EVs too.
        for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
            set copyrun 1
            set k 1
            set numevs [ expr $::fMRIModelView(Design,Run$r,numConditionEVs) + \
                            $::fMRIModelView(Design,Run$r,numAdditionalEVs) ]
            for { set i 1 } { $i <= $numevs } { incr i } {
                fMRIModelViewGenerateEVName $i $copyrun
                fMRIModelViewGenerateEVLabel $k
                incr k
            }
        }
    } else {
        #--- specify condition names for each run separately.
        #--- run 1
        for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
            set k 1
            set numevs [ expr $::fMRIModelView(Design,Run$r,numConditionEVs) + \
                             $::fMRIModelView(Design,Run$r,numAdditionalEVs) ]
            for { set i 1 } { $i <= $numevs } { incr i } {
                fMRIModelViewGenerateEVName $i $r
                fMRIModelViewGenerateEVLabel $k
                incr k
            }
        }
    }

    #--- this is the number of files read in, and their fake filenames
    set numFiles 0
    unset -nocomplain ::fMRIModelView(Design,fileNames)
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,numTimePoints) $::MultiVolumeReader(noOfVolumes)        
        for { set i 0 } { $i < $::fMRIModelView(Design,Run$r,numTimePoints) } { incr i } {
            fMRIModelViewAddFileName "Run$r-0$i.img"
            incr numFiles
        }
    }
    set ::fMRIModelView(Design,totalTimePoints) $numFiles
}


