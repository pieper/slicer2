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
# FILE:        IbrowserMotionCorrect.tcl
# PROCEDURES:  
#   IbrowserBuildMotionCorrectGUI
#   IbrowserMotionCorrectGo
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserBuildMotionCorrectGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildMotionCorrectGUI { f master } {
    global Gui

    #--- set global variables for frame so we can raise it,
    #--- and specification params
    set ::Ibrowser(fProcessMotionCorrect) $f
    set ::Ibrowser(Process,MotionCorrectQuality) 1
    set ::Ibrowser(Process,MotionCorrectIterate) 0
    set ::Ibrowser(Process,InternalReference) $::Volume(idNone)
    set ::Ibrowser(Process,ExternalReference) $::Volume(idNone)

    frame $f.fOverview -bg $Gui(activeWorkspace) -bd 2 
    frame $f.fInput -bg $Gui(activeWorkspace) -bd 2 
    frame $f.fModel -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $f.fResample -bg $Gui(activeWorkspace) -bd 2 -relief groove

    #---------------------------------------------------------------------------
    #---CHOOSE VOLUMES FRAME

    #--- create menu buttons and associated menus...
    set ff $f.fOverview
    DevAddButton $ff.bHelp "?" "IbrowserHelpMotionCorrection" 2 
    eval { label $ff.lOverview -text \
               "Register interval's volumes to reference." } $Gui(WLA)
    grid $ff.bHelp $ff.lOverview -pady 1 -padx 1 -sticky w
    
    set ff $f.fInput
    eval { label $ff.lChooseProcInterval -text "interval:" } $Gui(WLA)
    eval { menubutton $ff.mbIntervals -text "none" \
               -relief raised -bd 2 -width 18 \
               -menu $ff.mbIntervals.m -indicatoron 1 } $::Gui(WMBA)
    eval { menu $ff.mbIntervals.m } $::Gui(WMA)
    foreach i $::Ibrowser(idList) {
        $ff.mbIntervals.m add command -label $::Ibrowser($i,name) \
        -command "IbrowserSetActiveInterval $i"
    }
    set ::Ibrowser(Process,MotionCorrect,mbIntervals) $ff.mbIntervals
    set ::Ibrowser(Process,MotionCorrect,mIntervals) $ff.mbIntervals.m
    grid $ff.lChooseProcInterval $ff.mbIntervals -pady 1 -padx $::Gui(pad) -sticky e
    grid $ff.mbIntervals -sticky e
    
    eval { label $ff.lReference -text "reference:" } $Gui(WLA)
    eval { menubutton $ff.mbReference -text "none" \
               -relief raised -bd 2 -width 18 -indicatoron 1 \
               -menu $ff.mbReference.m } $::Gui(WMBA)
    eval { menu $ff.mbReference.m } $::Gui(WMA)
    foreach i $::Ibrowser(idList) {
        $ff.mbReference.m add command -label $::Ibrowser($i,name) \
            -command ""
    }
    set ::Ibrowser(Process,MotionCorrect,mbReference) $ff.mbReference
    set ::Ibrowser(Process,MotionCorrect,mReference) $ff.mbReference.m
    grid $ff.lReference $ff.mbReference -pady 1 -padx $::Gui(pad) -sticky e
    grid $ff.mbReference -sticky e
    
    #---------------------------------------------------------------------------
    #---QUALITY AND ITERATION FRAME
    set ff $f.fModel
    eval { label $ff.lQuality -text "quality:" } $Gui(WLA)
    eval { label $ff.lBlank -text "" } $Gui(WLA)
    eval { radiobutton $ff.rQualityCoarse -indicatoron 1\
               -text "coarse" -value 1 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "VersorMattesMIRegistrationCoarseParam" \
           } $Gui(WCA)
    grid $ff.lQuality $ff.rQualityCoarse -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityFair -indicatoron 1\
               -text "fair" -value 2 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "VersorMattesMIRegistrationFineParam" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityFair -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityGood -indicatoron 1\
               -text "good (slow)" -value 3 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "VersorMattesMIRegistrationGSlowParam" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityGood -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityBest -indicatoron 1\
               -text "best (very slow)" -value 4 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "VersorMattesMIRegistrationVerySlowParam" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityBest -padx $Gui(pad) -sticky w

    eval { label $ff.lIterate -text "iterations:" } $Gui(WLA)    
    eval { radiobutton $ff.rIterateMany -indicatoron 1\
               -text "repeat" -value 1 -variable ::Ibrowser(Process,MotionCorrectIterate) \
           } $Gui(WCA)
    grid $ff.lIterate $ff.rIterateMany -padx $Gui(pad) -sticky w

    eval { radiobutton $ff.rIterateOnce -indicatoron 1\
               -text "do once" -value 0 -variable ::Ibrowser(Process,MotionCorrectIterate) \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rIterateOnce -padx $Gui(pad) -sticky w

    DevAddButton $ff.bGo "Go" "IbrowserMotionCorrectGo $ff.bGo" 8
    grid $ff.lBlank $ff.bGo -padx $Gui(pad) -pady $Gui(pad) -sticky w
    set ::Ibrowser(Process,MotionCorrect,Go) $ff.bGo
    
    #---------------------------------------------------------------------------
    #---HARDEN TRANSFORMS FRAME
    set ff $f.fResample
    DevAddButton $ff.bCancel "Cancel" "IbrowserRemoveNonReferenceTransforms" 8
    pack $ff.bCancel -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
        

    pack $f.fOverview $f.fInput $f.fModel $f.fResample -side top -pady $Gui(pad) -padx $Gui(pad) -fill both

    #--- Place the whole collection of widgets in the
    #--- process-specific raised GUI panel.
    place $f -in $master -relheight 1.0 -relwidth 1.0
}





#-------------------------------------------------------------------------------
# .PROC IbrowserMotionCorrectGo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMotionCorrectGo { stopbutton } {

    
    set state [ $stopbutton cget -text ]
    if { $state  == "Stop" } {
        $stopbutton configure -text "Go"
        #--- stop the loop.
        IbrowserSayThis "aborting motion correction." 0
        set ::Ibrowser(AbortMotionCorrection) 1
        puts "Aborting motion correction!"
        return
    }

    set ::Ibrowser(AbortMotionCorrection) 0    
    #--- it's go
    if { [lsearch $::Ibrowser(idList) $::Ibrowser(activeInterval) ] == -1 } {
        DevErrorWindow "First select a valid sequence to motion correct."
        return
    } elseif { $::Ibrowser(activeInterval) == $::Ibrowser(idNone) } {
        DevErrorWindow "First select a valid sequence to motion correct."
        return
    }
    
    if { [lsearch $::Volume(idList) $::Ibrowser(Process,InternalReference) ] == -1 } {
        DevErrorWindow "First select a valid reference volume."
        return
    } elseif { $::Ibrowser(Process,InternalReference) == $::Volume(idNone) } {
        DevErrorWindow "First select a valid reference volume."
        return
    }    

    set tvid $::Ibrowser(Process,InternalReference)
    set iid $::Ibrowser(activeInterval)
    set start $::Ibrowser($iid,firstMRMLid)
    set stop $::Ibrowser($iid,lastMRMLid)
    set ::RigidIntensityRegistration(Repeat) $::Ibrowser(Process,MotionCorrectIterate)
    
    #--- now register each volume in the selected interval to reference...
    set numdrops $::Ibrowser($iid,numDrops)
    IbrowserRaiseProgressBar
    set progcount 0

    #--- test to see if user interrupted registration for any of the volumes.
    $stopbutton configure -text "Stop"
    for { set vid $start } { $vid <= $stop } { incr vid } {
        if { $numdrops != 0} {
            set progress [ expr double ($progcount) / double ($numdrops) ]
            IbrowserUpdateProgressBar $progress "::"
            IbrowserPrintProgressFeedback
        }
        #--- don't try to register a volume to itself...
        if { $vid != $tvid } {
            #--- set targets and sources
            set ::RigidIntensityRegistration(sourceID) $vid
            set ::RigidIntensityRegistration(targetID) $tvid
            set ::Matrix(activeID) ""
            set ::Matrix(volume) $vid
            set ::Matrix(refVolume) $tvid
            #--- register and increment to next volume
            IbrowserSayThis "starting motion correction." 0
            VersorMattesMIRegistrationAutoRun
            #--- save the transform ID so we can use it later
            #--- for deleting, if motion correction is canceled.
            for { set j 0 } { $j < $numdrops } { incr j } {
                set dropID  $::Ibrowser($iid,$j,MRMLid)
                if { $dropID == $vid } {
                    set ::Ibrowser($iid,$j,transformID) [ expr $::Transform(nextID) - 1 ]
                    break
                }
            }
            puts "Registered $j of [expr $numdrops - 1] volumes to reference..."
            #--- try to catch a user's click of the "stop" button
            update idletasks
            if { $::Ibrowser(AbortMotionCorrection) } {
                break
            }
        }
        incr progcount
    }

    IbrowserLowerProgressBar
    #--- clean transforms up if aborted...
    if { $::Ibrowser(AbortMotionCorrection) } {
        IbrowserSayThis "motion correction stopped by user, removing transforms..." 0
        IbrowserRemoveNonReferenceTransforms
    } else {
        IbrowserSayThis "motion correction complete." 0
    }

}




proc IbrowserHelpMotionCorrection { } {

    set i [ IbrowserGetHelpWinID ]
    set txt "<H3>Motion correction</H3>
 <P> This tool lets you select an interval to motion correct (a source interval), to select a reference (target) volume from within the source interval, and register all of the other volumes in the source interval to the reference. Motion correction adds a transform to each non-reference volume in the interval, and may not work properly if the reference or target volume nodes already have transforms applied to them. It is recommended that an interval be motion corrected first, using any of its volumes as the reference, and if subsequent transforms are necessary (i.e. for co-registration) that those operations be carried out in a second step.
<P> To abort motion correction, first stop the registration of an individual volume by clicking the <I>Stop</I> button on the popup window, and then stop the registration of subsequent volumes in the source interval by clicking the <I>Stop</I> button on the gui panel. (You may have to press <I>Stop</I> one more time in the popup window.) Then, wait a short time for the registration to stop and any registration transforms that were added to the scene to be deleted."
    DevCreateTextPopup infowin$i "Ibrowser information" 100 100 18 $txt
}

