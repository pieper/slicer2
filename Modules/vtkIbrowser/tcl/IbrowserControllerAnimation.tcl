#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        IbrowserControllerAnimation.tcl
# PROCEDURES:  
#   IbrowserSetupAnimationMenuImages
#   IbrowserMakeAnimationMenu
#   IbrowserDecrementFrame
#   IbrowserIncrementFrame
#   IbrowserGoToStartFrame
#   IbrowserGoToEndFrame
#   IbrowserPlayOnce
#   IbrowserPlayOnceReverse
#   IbrowserStopAnimation
#   IbrowserRecordAnimationToFile
#   IbrowserPauseAnimation
#   IbrowserLoopAnimate
#   IbrowserPingPongAnimate
#   IbrowserZoomIn
#   IbrowserZoomOut
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserSetupAnimationMenuImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetupAnimationMenuImages { } {
    global PACKAGE_DIR_VTKIbrowser    

    #--- This variable contains the module path plus some stuff
    #--- trim off the extra stuff, and add on the path to tcl files.
    set tmpstr $PACKAGE_DIR_VTKIbrowser
    set tmpstr [string trimright $tmpstr "/vtkIbrowser" ]
    set tmpstr [string trimright $tmpstr "/Tcl" ]
    set tmpstr [string trimright $tmpstr "Wrapping" ]
    set modulePath [format "%s%s" $tmpstr "tcl/"]

    
    set ::IbrowserController(Images,Menu,frame-incLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/frame-incLO.gif]
    set ::IbrowserController(Images,Menu,frame-incHI) \
    [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/frame-incHI.gif]    
    set ::IbrowserController(Images,Menu,frame-decLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/frame-decLO.gif]
    set ::IbrowserController(Images,Menu,frame-decHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/frame-decHI.gif]
    set ::IbrowserController(Images,Menu,goto-startLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/goto-startLO.gif]
    set ::IbrowserController(Images,Menu,goto-startHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/goto-startHI.gif]
    set ::IbrowserController(Images,Menu,goto-endLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/goto-endLO.gif]
    set ::IbrowserController(Images,Menu,goto-endHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/goto-endHI.gif]    
    set ::IbrowserController(Images,Menu,anim-pingpongLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-pingpongLO.gif]
    set ::IbrowserController(Images,Menu,anim-pingpongHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-pingpongHI.gif]
    set ::IbrowserController(Images,Menu,anim-loopLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-loopLO.gif]
    set ::IbrowserController(Images,Menu,anim-loopHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-loopHI.gif]
    set ::IbrowserController(Images,Menu,anim-stopLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-stopLO.gif]
    set ::IbrowserController(Images,Menu,anim-stopHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-stopHI.gif]    
    set ::IbrowserController(Images,Menu,anim-recLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-recLO.gif]
    set ::IbrowserController(Images,Menu,anim-recHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-recHI.gif]
    set ::IbrowserController(Images,Menu,anim-pauseLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-pauseLO.gif]
    set ::IbrowserController(Images,Menu,anim-pauseHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-pauseHI.gif]    
    set ::IbrowserController(Images,Menu,anim-playLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-playLO.gif]
    set ::IbrowserController(Images,Menu,anim-playHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-playHI.gif]    
    set ::IbrowserController(Images,Menu,anim-rewLO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-rewLO.gif]
    set ::IbrowserController(Images,Menu,anim-rewHI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/anim-rewHI.gif]    
    set ::IbrowserController(Images,Menu,zoomIn-LO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/zoomIn-LO.gif]
    set ::IbrowserController(Images,Menu,zoomIn-HI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/zoomIn-HI.gif]
    set ::IbrowserController(Images,Menu,zoomOut-LO) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/zoomOut-LO.gif]
    set ::IbrowserController(Images,Menu,zoomOut-HI) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/controls/zoomOut-HI.gif]
}





#-------------------------------------------------------------------------------
# .PROC IbrowserMakeAnimationMenu
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeAnimationMenu { root } {
    
    frame $root.fibProcAndControl -relief flat -background white -height 80 -pady 10
    label $root.fibProcAndControl.lframe_dec -background white \
        -image $::IbrowserController(Images,Menu,frame-decLO) -relief flat
    bind $root.fibProcAndControl.lframe_dec <Enter> {
        %W config -image $::IbrowserController(Images,Menu,frame-decHI) }
    bind $root.fibProcAndControl.lframe_dec <Leave> {
        %W config -image $::IbrowserController(Images,Menu,frame-decLO) }
    bind $root.fibProcAndControl.lframe_dec <Button-1> {
        IbrowserDecrementFrame }
    
    label $root.fibProcAndControl.lframe_curr -background white \
        -textvariable ::Ibrowser(ViewDrop) -width 10 -relief groove 

    label $root.fibProcAndControl.lframe_inc -background white \
        -image $::IbrowserController(Images,Menu,frame-incLO) -relief flat
    bind $root.fibProcAndControl.lframe_inc <Enter> {
        %W config -image $::IbrowserController(Images,Menu,frame-incHI) }
    bind $root.fibProcAndControl.lframe_inc <Leave> {
        %W config -image $::IbrowserController(Images,Menu,frame-incLO) }
    bind $root.fibProcAndControl.lframe_inc <Button-1> {
        IbrowserIncrementFrame }

    label $root.fibProcAndControl.lgoto_start -background white \
        -image $::IbrowserController(Images,Menu,goto-startLO) -relief flat
    bind $root.fibProcAndControl.lgoto_start <Enter> {
        %W config -image $::IbrowserController(Images,Menu,goto-startHI) }
    bind $root.fibProcAndControl.lgoto_start <Leave> {
        %W config -image $::IbrowserController(Images,Menu,goto-startLO) }
    bind $root.fibProcAndControl.lgoto_start <Button-1> {
        IbrowserGoToStartFrame }    

    label $root.fibProcAndControl.lanim_rew -background white \
        -image $::IbrowserController(Images,Menu,anim-rewLO) -relief flat
    bind $root.fibProcAndControl.lanim_rew <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-rewHI) }
    bind $root.fibProcAndControl.lanim_rew <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-rewLO) }
    bind $root.fibProcAndControl.lanim_rew <Button-1> {
        IbrowserPlayOnceReverse }    
    
    label $root.fibProcAndControl.lanim_stop -background white \
        -image $::IbrowserController(Images,Menu,anim-stopLO) -relief flat
    bind $root.fibProcAndControl.lanim_stop <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-stopHI) }
    bind $root.fibProcAndControl.lanim_stop <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-stopLO) }
    bind $root.fibProcAndControl.lanim_stop <Button-1> {
        IbrowserStopAnimation }

    label $root.fibProcAndControl.lanim_rec -background white \
    -image $::IbrowserController(Images,Menu,anim-recLO) -relief flat
    bind $root.fibProcAndControl.lanim_rec <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-recHI) }
    bind $root.fibProcAndControl.lanim_rec <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-recLO) }
    bind $root.fibProcAndControl.lanim_rec <Button-1> {
        IbrowserRecordAnimationToFile "mpeg" }

    label $root.fibProcAndControl.lanim_pause -background white \
        -image $::IbrowserController(Images,Menu,anim-pauseLO) -relief flat            
    bind $root.fibProcAndControl.lanim_pause <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-pauseHI) }
    bind $root.fibProcAndControl.lanim_pause <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-pauseLO) }
    bind $root.fibProcAndControl.lanim_pause <Button-1> {
        IbrowserPauseAnimation }    
    
    label $root.fibProcAndControl.lanim_play -background white \
        -image $::IbrowserController(Images,Menu,anim-playLO) -relief flat
    bind $root.fibProcAndControl.lanim_play <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-playHI) }
    bind $root.fibProcAndControl.lanim_play <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-playLO) }
    bind $root.fibProcAndControl.lanim_play <Button-1> {
        IbrowserPlayOnce }
    
    label $root.fibProcAndControl.lgoto_end -background white \
        -image $::IbrowserController(Images,Menu,goto-endLO) -relief flat                
    bind $root.fibProcAndControl.lgoto_end <Enter> {
        %W config -image $::IbrowserController(Images,Menu,goto-endHI) }
    bind $root.fibProcAndControl.lgoto_end <Leave> {
        %W config -image $::IbrowserController(Images,Menu,goto-endLO) }
    bind $root.fibProcAndControl.lgoto_end <Button-1> {
        IbrowserGoToEndFrame }
    
    label $root.fibProcAndControl.lanim_loop -background white \
        -image $::IbrowserController(Images,Menu,anim-loopLO) -relief flat
    bind $root.fibProcAndControl.lanim_loop <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-loopHI) }
    bind $root.fibProcAndControl.lanim_loop <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-loopLO) }
    bind $root.fibProcAndControl.lanim_loop <Button-1> {
        set ::Ibrowser(AnimationInterrupt) 0
        IbrowserLoopAnimate }
    
    label $root.fibProcAndControl.lanim_pingpong -background white \
        -image $::IbrowserController(Images,Menu,anim-pingpongLO) -relief flat
    bind $root.fibProcAndControl.lanim_pingpong <Enter> {
        %W config -image $::IbrowserController(Images,Menu,anim-pingpongHI) }
    bind $root.fibProcAndControl.lanim_pingpong <Leave> {
        %W config -image $::IbrowserController(Images,Menu,anim-pingpongLO) }
    bind $root.fibProcAndControl.lanim_pingpong <Button-1> {
        set ::Ibrowser(AnimationInterrupt) 0
        IbrowserPingPongAnimate }
    
    label $root.fibProcAndControl.lzoomIn -background white \
        -image $::IbrowserController(Images,Menu,zoomIn-LO) -relief flat
    bind $root.fibProcAndControl.lzoomIn <Enter> {
        %W config -image $::IbrowserController(Images,Menu,zoomIn-HI) }
    bind $root.fibProcAndControl.lzoomIn <Leave> {
        %W config -image $::IbrowserController(Images,Menu,zoomIn-LO) }
    bind $root.fibProcAndControl.lzoomIn <Button-1> {
        IbrowserZoomIn }

    label $root.fibProcAndControl.lzoomOut -background white \
        -image $::IbrowserController(Images,Menu,zoomOut-LO) -relief flat
    bind $root.fibProcAndControl.lzoomOut <Enter> {
        %W config -image $::IbrowserController(Images,Menu,zoomOut-HI) }
    bind $root.fibProcAndControl.lzoomOut <Leave> {
        %W config -image $::IbrowserController(Images,Menu,zoomOut-LO) }
    bind $root.fibProcAndControl.lzoomOut <Button-1> {
        IbrowserZoomOut }

    label $root.fibProcAndControl.lProgressBarblank -background white \
     -width 1 -relief flat -padx 0
    
    label $root.fibProcAndControl.lProgressBar -background white \
        -textvariable ::IbrowserController(ProgressBarTxt) -width 20 \
        -relief flat -padx 0 -font $::IbrowserController(UI,Medfont) \
        -highlightcolor #FFFFFF -highlightbackground #FFFFFF -foreground #FFFFFF

    #--- we can configure this later to have grooved or flat relief
    #--- which makes it effectively invisible or visible.
    set ::IbrowserController(ProgressBar) $root.fibProcAndControl.lProgressBar
    IbrowserLowerProgressBar
    
    pack $root.fibProcAndControl.lframe_dec -side left
    pack $root.fibProcAndControl.lframe_curr -side left
    pack $root.fibProcAndControl.lframe_inc -side left
    pack $root.fibProcAndControl.lzoomIn -side left
    pack $root.fibProcAndControl.lzoomOut -side left    
    pack $root.fibProcAndControl.lgoto_start -side left
    pack $root.fibProcAndControl.lanim_rew -side left
    pack $root.fibProcAndControl.lanim_stop -side left
    pack $root.fibProcAndControl.lanim_rec -side left
    pack $root.fibProcAndControl.lanim_pause -side left
    pack $root.fibProcAndControl.lanim_play -side left    
    pack $root.fibProcAndControl.lgoto_end -side left    
    pack $root.fibProcAndControl.lanim_loop -side left    
    pack $root.fibProcAndControl.lanim_pingpong -side left
    pack $root.fibProcAndControl.lProgressBarblank -side left
    pack $root.fibProcAndControl.lProgressBar -side left
    pack $root.fibProcAndControl -side top -fill x -expand false

}


#-------------------------------------------------------------------------------
# .PROC IbrowserDecrementFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDecrementFrame { } {

    set ::Ibrowser(AnimationInterrupt) 1
    #--- what is current frame?
    set curDrop $::Ibrowser(ViewDrop)

    #--- how many drops in this interval?
    set id $::Ibrowser(activeInterval)
    set name $::Ibrowser($id,name)
    set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]

    #--- update if appropriate.
    set newDrop [ expr $curDrop - 1 ]
    if { $newDrop >= 0 } {
        set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
        set ::Ibrowser(ViewDrop) $newDrop
        IbrowserUpdateIndexFromAnimControls
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
}


#-------------------------------------------------------------------------------
# .PROC IbrowserIncrementFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserIncrementFrame { } {

    set ::Ibrowser(AnimationInterrupt) 1
    #--- what is current frame?
    set curDrop $::Ibrowser(ViewDrop)

    #--- how many drops in this interval?
    set id $::Ibrowser(activeInterval)
    set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]
    set newDrop [ expr $curDrop + 1 ]
    if { $newDrop <= $numdrops } {
        set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
        set ::Ibrowser(ViewDrop) $newDrop
        IbrowserUpdateIndexFromAnimControls
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
}

#-------------------------------------------------------------------------------
# .PROC IbrowserGoToStartFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGoToStartFrame { } {

    set ::Ibrowser(AnimationInterrupt) 1
    set id $::Ibrowser(activeInterval)
    set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
    set ::Ibrowser(ViewDrop) 0
    IbrowserUpdateIndexFromAnimControls
    IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
}


#-------------------------------------------------------------------------------
# .PROC IbrowserGoToEndFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGoToEndFrame { } {

    set ::Ibrowser(AnimationInterrupt) 1
    #--- how many drops in this interval?
    set id $::Ibrowser(activeInterval)
    set name $::Ibrowser($id,name)
    set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]
    set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
    set ::Ibrowser(ViewDrop) $numdrops
    IbrowserUpdateIndexFromAnimControls
    IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)

}

#-------------------------------------------------------------------------------
# .PROC IbrowserPlayOnce
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPlayOnce { } {

    set done 0
    set ::Ibrowser(AnimationInterrupt) 1
    set curDrop $::Ibrowser(ViewDrop)

    #--- how many drops in this interval?
    set id $::Ibrowser(activeInterval)
    set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]

    if { $curDrop < $numdrops } {
        set newDrop [ expr $curDrop + 1 ]
        set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
        set ::Ibrowser(ViewDrop) $newDrop
        IbrowserUpdateIndexFromAnimControls
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    } else {
        set done 1
    }
    if { $done == 0 } {
        update
        after $::Ibrowser(AnimationFrameDelay) IbrowserPlayOnce
    }
                      
}



    

#-------------------------------------------------------------------------------
# .PROC IbrowserPlayOnceReverse
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPlayOnceReverse { } {

    set done 0
    set ::Ibrowser(AnimationInterrupt) 1
    set curDrop $::Ibrowser(ViewDrop)    

    #--- how many drops in this interval?
    set id $::Ibrowser(activeInterval)
    set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]

    if { $curDrop > 0 } {
        set newDrop [ expr $curDrop - 1 ]
        set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
        set ::Ibrowser(ViewDrop) $newDrop
        IbrowserUpdateIndexFromAnimControls
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    } else {
        set done 1
    }
    if { $done == 0 } {
        update
        after $::Ibrowser(AnimationFrameDelay) IbrowserPlayOnceReverse
    }
                      
}

#-------------------------------------------------------------------------------
# .PROC IbrowserStopAnimation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserStopAnimation { } {

    set ::Ibrowser(AnimationInterrupt) 1
}

#-------------------------------------------------------------------------------
# .PROC IbrowserRecordAnimationToFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRecordAnimationToFile { type } {
    set ::Ibrowser(AnimationInterrupt) 1
    
}

#-------------------------------------------------------------------------------
# .PROC IbrowserPauseAnimation
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPauseAnimation { } {

    set ::Ibrowser(AnimationInterrupt) 1
}

#-------------------------------------------------------------------------------
# .PROC IbrowserLoopAnimate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserLoopAnimate { } {

    if { $::Ibrowser(AnimationInterrupt) == 0 } {
        set curDrop $::Ibrowser(ViewDrop)    

        #--- how many drops in this interval?
        set id $::Ibrowser(activeInterval)
        set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]
        
        if { $curDrop < $numdrops } {
            set newDrop [ expr $curDrop + 1 ]
            set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
            set ::Ibrowser(ViewDrop) $newDrop
            IbrowserUpdateIndexFromAnimControls
            IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
            update
            after $::Ibrowser(AnimationFrameDelay) IbrowserLoopAnimate
        } else {
            set newDrop 0
            set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
            set ::Ibrowser(ViewDrop) $newDrop
            IbrowserUpdateIndexFromAnimControls
            IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
            update
            after $::Ibrowser(AnimationFrameDelay) IbrowserLoopAnimate            
        }
    }

}

#-------------------------------------------------------------------------------
# .PROC IbrowserPingPongAnimate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPingPongAnimate { } {

    if { $::Ibrowser(AnimationInterrupt) == 0 } {
        #--- get the current frame
        set curDrop $::Ibrowser(ViewDrop)    

        #--- how many drops in this interval?
        set id $::Ibrowser(activeInterval)
        set numdrops [ expr $::Ibrowser($id,lastMRMLid) - $::Ibrowser($id,firstMRMLid) ]
        #--- forward direction...
        if { $::Ibrowser(AnimationDirection) == 1 } {
            if { $curDrop < $numdrops } {
                set newDrop [ expr $curDrop + 1 ]
                set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
                set ::Ibrowser(ViewDrop) $newDrop
                IbrowserUpdateIndexFromAnimControls
                IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
                update
                after $::Ibrowser(AnimationFrameDelay) IbrowserPingPongAnimate
            } else {
                set ::Ibrowser(AnimationDirection) -1
                update
                after $::Ibrowser(AnimationFrameDelay) IbrowserPingPongAnimate
            }
        } else {
            if { $curDrop > 0 } {
                set newDrop [ expr $curDrop - 1 ]
                set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
                set ::Ibrowser(ViewDrop) $newDrop
                IbrowserUpdateIndexFromAnimControls
                IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
                update
                after $::Ibrowser(AnimationFrameDelay) IbrowserPingPongAnimate
            } else {
                set ::Ibrowser(AnimationDirection) 1
                update
                after $::Ibrowser(AnimationFrameDelay) IbrowserPingPongAnimate
            }
        }
    }
    
}

#-------------------------------------------------------------------------------
# .PROC IbrowserZoomIn
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserZoomIn { } {

    if { 0 } {
        if { $::IbrowserController(zoomfactor) < 4 } {
            $::IbrowserController(Icanvas) scale all 0 0 1.1 1.1
            $::IbrowserController(Ccanvas) scale all 0 0 1.1 1.1
            incr ::IbrowserController(zoomfactor)
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC IbrowserZoomOut
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserZoomOut { } {

    if { 0 } {
        if { $::IbrowserController(zoomfactor) > 0 } {
            $::IbrowserController(Icanvas) scale all 0 0 0.9 0.9
            $::IbrowserController(Ccanvas) scale all 0 0 0.9 0.9
            set ::IbrowserController(zoomfactor) [ expr $::IbrowserController(zoomfactor) - 1 ]
        }
    }
}
