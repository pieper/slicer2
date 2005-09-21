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
    set ::fMRIModelView(Design,numRuns) $fMRIEngine(noOfSpecifiedRuns)
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
            #--- Specify each condition inside each run...
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
    #--- by default, ev for baseline is off
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        set ::fMRIModelView(Design,Run$r,UsePolyBasis) 0
        set ::fMRIModelView(Design,Run$r,UseSplineBasis) 0
        set ::fMRIModelView(Design,Run$r,UseExploratoryBasis) 0
        unset -nocomplain ::fMRIModelView(Design,evNames)
        unset -nocomplain ::fMRIModelView(Design,evs)
    }

    #---    
    #--- Regular signal types used for condition-derived EVs:
    #--- baseline,
    #--- boxcar,
    #--- boxcar_dt1,
    #--- boxcar_dt2,
    #--- boxcar_dt3,
    #--- boxcar_cHRF,
    #--- boxcar_cHRF_dt1,
    #--- boxcar_cHRF_dt2,
    #--- boxcar_cHRF_dt3,
    #--- halfsine,
    #--- halfsine_dt1,
    #--- halfsine_dt2,
    #--- halfsine_dt3,
    #--- halfsine_cHRF,
    #--- halfsine_cHRF_dt1,
    #--- halfsine_cHRF_dt2,
    #--- halfsine_cHRF_dt3,
    #--- linearEffect, and
    #--- quadraticEffect.
    #--- These variables get generated during signal modeling
    #--- of conditions, and selection of additional EVs.
    #--- So here, ::fMRIModelView(Design,EV1,SignalType)
    #--- corresponds to condition1.
    #---
    #--- wjp 09/02/06 Might be a problem here: not all evs with
    #--- names map to a condition (adding derivatives adds new
    #--- names of evs in the list.

    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        #---wjp 09/19/05 adding this unset. ConditionNames keeps getting appended.
        #unset -nocomplain ::fMRIModelView(Design,Run$r,ConditionNames)
        set count 1 
        foreach title $fMRIEngine($r,namesOfEVs) {
            set wf $::fMRIEngine($r,$title,signalType)
            fMRIModelViewSetEVCondition $r $count $::fMRIEngine($r,$title,myCondition)
            fMRIModelViewSetEVSignalType $r $count $wf
            #--- WJP: moving the adding of ConditionName to fMRIEngineAddCondition 
            #fMRIModelViewAddConditionName $r $title 
            incr count
        }
    }
    
    #--- wjp 09/02/05
    #--- add to the count of additional EVs if temporal derivatives are used in
    #--- modeling, as these EVs have been automatically created above.
    for { set r 1 } { $r <= $::fMRIModelView(Design,numRuns) } { incr r } {
        foreach title $fMRIEngine($r,namesOfEVs) {
            set sig $::fMRIEngine($r,$title,signalType)
            set addEV [ string first "dt" $sig  ]
            if { $addEV >= 0 } {
                incr ::fMRIModelView(Design,Run$r,numAdditionalEVs)
            }
        }
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
            set j [ expr \
                        $::fMRIModelView(Design,Run$r,numConditionEVs) + \
                        $::fMRIModelView(Design,Run$r,numAdditionalEVs) + 1 ]
            #--- wjp added.
            fMRIModelViewSetEVCondition $r $j "none"
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
        set j [ expr $::fMRIModelView(Design,Run$r,numConditionEVs) + $::fMRIModelView(Design,Run$r,numAdditionalEVs) + 1 ]
        if { $::fMRIModelView(Design,Run$r,UseDCBasis) } {
            for { set b 0 } { $b < $numDCbasis } { incr b } {
                #--- wjp added
                fMRIModelViewSetEVCondition $r $j "none"
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
    #--- these auto-generated titles & labels of EVs derived from
    #--- user-specified conditions in each run, and the additional
    #--- requested regressors.
    #--- If temporal derivatives have been requested in modeling, additional
    #--- ev's are automatically created in GenerateEVName and GenerateEVLabel.
    #--- these will be added into the count below.
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

            # reformat the contrast string
            set cl [split $c " "]
            set len [llength $cl]
            if {$len > $::fMRIModelView(Design,totalEVs)} {
                set cl [lrange $cl 0 [expr $::fMRIModelView(Design,totalEVs)-1]]
            } elseif {$len < $::fMRIModelView(Design,totalEVs)} {
                for {set j $len} {$j < $::fMRIModelView(Design,totalEVs)} {incr j} {
                    lappend cl 0
                }
            } else {
            }

            set index [expr $i+1]
            fMRIModelViewSetTContrast $index $cl
            fMRIModelViewSetTContrastName $index t-$name 
            fMRIModelViewSetTContrastLabel $index

            set str [join $cl " "]
            set fMRIEngine($name,contrastVector) $str
        }

        incr i
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


