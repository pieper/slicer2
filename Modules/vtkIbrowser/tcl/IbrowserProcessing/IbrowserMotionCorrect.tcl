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
#   IbrowserMotionCorrectStopGo
#   IbrowserHardenTransforms
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
    set ::Ibrowser(Process,MotionCorrect,Hardened) 0
    set ::Ibrowser(Process,InternalReference) $::Volume(idNone)
    set ::Ibrowser(Process,ExternalReference) $::Volume(idNone)

    frame $f.fInput -bg $Gui(activeWorkspace) -bd 2 
    frame $f.fModel -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $f.fResample -bg $Gui(activeWorkspace) -bd 2 -relief groove

    #---------------------------------------------------------------------------
    #---CHOOSE VOLUMES FRAME

    #--- create menu buttons and associated menus...
    set ff $f.fInput
    eval { label $ff.lChooseProcInterval -text "interval:" } $Gui(WLA)
    eval { menubutton $ff.mbIntervals -text "none" \
               -relief raised -bd 2 -width 18 \
               -menu $ff.mbIntervals.m -indicatoron 1 } $::Gui(WMBA)
    eval { menu $ff.mbIntervals.m } $::Gui(WMA)
    foreach i $::Ibrowser(idList) {
        puts "adding $::Ibrowser($i,name) to Motion correction menu"
        $ff.mbIntervals.m add command -label $::Ibrowser($i,name) 
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
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lQuality $ff.rQualityCoarse -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityFair -indicatoron 1\
               -text "fair" -value 2 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityFair -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityGood -indicatoron 1\
               -text "good" -value 3 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityGood -padx $Gui(pad) -sticky w
    
    eval { radiobutton $ff.rQualityBest -indicatoron 1\
               -text "best" -value 4 -variable ::Ibrowser(Process,MotionCorrectQuality) \
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rQualityBest -padx $Gui(pad) -sticky w

    eval { label $ff.lIterate -text "iterations:" } $Gui(WLA)    
    eval { radiobutton $ff.rIterateMany -indicatoron 1\
               -text "repeat" -value 1 -variable ::Ibrowser(Process,MotionCorrectIterate) \
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lIterate $ff.rIterateMany -padx $Gui(pad) -sticky w

    eval { radiobutton $ff.rIterateOnce -indicatoron 1\
               -text "do once" -value 0 -variable ::Ibrowser(Process,MotionCorrectIterate) \
               -command "puts $::Ibrowser(Process,MotionCorrectQuality)" \
           } $Gui(WCA)
    grid $ff.lBlank $ff.rIterateOnce -padx $Gui(pad) -sticky w

    DevAddButton $ff.bGoStop "Go" "IbrowserMotionCorrectStopGo $ff.bGoStop" 8
    grid $ff.lBlank $ff.bGoStop -padx $Gui(pad) -pady $Gui(pad) -sticky w
    set ::Ibrowser(Process,MotionCorrect,GoStop) $ff.bGoStop
    
    #---------------------------------------------------------------------------
    #---HARDEN TRANSFORMS FRAME
    set ff $f.fResample
    DevAddButton $ff.bCancel "Cancel (pre-harden only)" "IbrowserRemoveTransforms" 8
    DevAddButton $ff.bApply "Harden transforms" "IbrowserHardenTransforms" 8
    pack $ff.bCancel $ff.bApply -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
        

    pack $f.fInput $f.fModel $f.fResample -side top -pady $Gui(pad) -padx $Gui(pad) -fill both

    #--- Place the whole collection of widgets in the
    #--- process-specific raised GUI panel.
    place $f -in $master -relheight 1.0 -relwidth 1.0
}





#-------------------------------------------------------------------------------
# .PROC IbrowserMotionCorrectStopGo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMotionCorrectStopGo { stopbutton } {

    #--- check for stopping
    set state [ $stopbutton cget -text ]
    if { $state == "Stop" } {
        $stopbutton configure -text "Go"
        #--- stop the loop.
        IbrowserSayThis "stopping motion correction." 0
        return
    }
    
    #--- otherwise, it's go
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

    #--- Adds a transform around each volume in the sequence.
    IbrowserAddSequenceTransforms
    set ::Ibrowser(Process,MotionCorrect,Hardened) 0
    $stopbutton configure -text "Stop"
}

#-------------------------------------------------------------------------------
# .PROC IbrowserHardenTransforms
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserHardenTransforms { } {
    puts "transform hardened."
    set ::Ibrowser(Process,MotionCorrect,Hardened) 1
}

