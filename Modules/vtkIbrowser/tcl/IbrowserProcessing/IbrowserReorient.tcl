
proc IbrowserBuildReorientGUI { f master } {
    global Gui
    #--- This GUI allows a volume to be flipped from
    #--- left to right, up to down in display space.
    #--- set a global variable for frame
    set ::Ibrowser(fProcessReorient) $f
    
    frame $f.fSpace -bg $::Gui(activeWorkspace) -bd 2 
    eval { label $f.fSpace.lSpace -text "       " } $Gui(WLA)
    pack $f.fSpace -side top 
    pack $f.fSpace.lSpace -side top -pady 4 -padx 20 -anchor w -fill x
    
    #--- frame to specify the interval to be processed.
    frame $f.fSelectInterval -bg $::Gui(activeWorkspace) -bd 2
    pack $f.fSelectInterval -side top -anchor w -fill x
    eval { label $f.fSelectInterval.lText -text "interval to process:" } $Gui(WLA)    
    eval { menubutton $f.fSelectInterval.mbIntervals -text "none" -width 20 -relief raised \
               -height 1 -menu $f.fSelectInterval.mbIntervals.m -bg $::Gui(activeWorkspace) \
               -indicatoron 1 } $Gui(WBA)
    eval { menu $f.fSelectInterval.mbIntervals.m } $Gui(WMA)
    foreach i $::Ibrowser(idList) {
        puts "adding $::Ibrowser($i,name)"
        $f.mbVolumes.m add command -label $::Ibrowser($i,name) \
            -command "IbrowserSetActiveInterval $i"
    }
    set ::Ibrowser(Process,Reorient,mbIntervals) $f.fSelectInterval.mbIntervals
    set ::Ibrowser(Process,Reorient,mIntervals) $f.fSelectInterval.mbIntervals.m
    pack $f.fSelectInterval.lText -pady 2 -padx 2 -anchor w
    pack $f.fSelectInterval.mbIntervals -pady 2 -padx 2 -anchor w
    
    #--- frame to configure and drive the reorienting
    frame $f.fConfiguration -bg $Gui(activeWorkspace) -bd 2 -relief groove
    pack $f.fConfiguration -side top -anchor w -padx 2 -pady 5 -fill x
    DevAddLabel $f.fConfiguration.lText "reorient volumes:"
    DevAddButton $f.fConfiguration.bFlipLR "flip LR" "IbrowserFlipVolumeLR" 20
    DevAddButton $f.fConfiguration.bFlipIS "flip IS" "IbrowserFlipVolumeIS" 20
    DevAddButton $f.fConfiguration.bFlipAP "flip AP" "IbrowserFlipVolumeAP" 20
    DevAddButton $f.fConfiguration.bRevert "revert to original" "IbrowserRevertVolumeOrientation" 20

    grid $f.fConfiguration.lText 
    grid $f.fConfiguration.lText -padx 3 -ipady 5
    
    grid $f.fConfiguration.bFlipLR
    grid $f.fConfiguration.bFlipLR -padx 3 -ipady 3

    grid $f.fConfiguration.bFlipIS
    grid $f.fConfiguration.bFlipIS -padx 3 -ipady 3

    grid $f.fConfiguration.bFlipAP
    grid $f.fConfiguration.bFlipAP -padx 3 -ipady 3

    grid $f.fConfiguration.bRevert
    grid $f.fConfiguration.bRevert -padx 3 -ipady 3

    place $f -in $master -relwidth 1.0 -relheight 1.0 -y 0

    
}


proc  IbrowserInitializeOrientation { id } {
    set ::Ibrowser($id,ISFlip) 0
    set ::Ibrowser($id,LRFlip) 0
    set ::Ibrowser($id,APFlip) 0
}


proc IbrowserFlipVolumeLR { } {
global Volume
    
    #--- Flips a volume around the vertical axis,
    #--- from left to right, around center.
    #--- Flipped vtkImageData replaces its source

    set iid $::Ibrowser(activeInterval)
    set firstID $::Ibrowser($iid,firstMRMLid)
    set lastID $::Ibrowser($iid,lastMRMLid)

    IbrowserRaiseProgressBar
    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)
    
    for { set v $firstID } { $v <= $lastID } { incr v } {
        set progress [ expr double ( $pcount ) / double ( $numvols ) ]
        IbrowserUpdateProgressBar $progress "::"
        #--- without something to make the application idle,
        #--- tk will not handle the drawing of progress bar.
        #--- Instead of calling update, which could cause
        #--- some unstable event loop, we just print some
        #--- '.'s to the tkcon. Slows things down a little,
        #--- but not terribly much. Better ideas are welcome.
        IbrowserPrintProgressFeedback

        vtkImageFlip flipLR
        flipLR SetInput [ ::Volume($v,vol) GetOutput ]
        flipLR SetFilteredAxis 0
        ::Volume($v,vol) SetImageData [ flipLR GetOutput ]
        
        MainVolumesUpdate $v
        flipLR Delete
        incr pcount
    }
    IbrowserEndProgressFeedback
    RenderAll
    set tt "Flipped volumes in $iid around vertical axis."
    IbrowserSayThis $tt 0
    IbrowserLowerProgressBar
    
    #--- keeps track of whether the volume is LR flipped
    if { $::Ibrowser($iid,LRFlip) == 1 } {
        set ::Ibrowser($iid,LRFlip) 0
    } else {
        set ::Ibrowser($iid,LRFlip) 1
    }


}

proc IbrowserFlipVolumeAP { } {
global Volume
    
    #--- Flips a volume from anterior to posterior.
    #--- Flipped vtkImageData replaces its source

    set iid $::Ibrowser(activeInterval)
    set firstID $::Ibrowser($iid,firstMRMLid)
    set lastID $::Ibrowser($iid,lastMRMLid)

    IbrowserRaiseProgressBar
    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)
    
    for { set v $firstID } { $v <= $lastID } { incr v } {
        set progress [ expr double ( $pcount ) / double ( $numvols ) ]
        IbrowserUpdateProgressBar $progress "::"
        #--- without something to make the application idle,
        #--- tk will not handle the drawing of progress bar.
        #--- Instead of calling update, which could cause
        #--- some unstable event loop, we just print some
        #--- '.'s to the tkcon. Slows things down a little,
        #--- but not terribly much. Better ideas are welcome.
        IbrowserPrintProgressFeedback

        vtkImageFlip flipAP
        flipAP SetInput [ ::Volume($v,vol) GetOutput ]
        flipAP SetFilteredAxis 1
        ::Volume($v,vol) SetImageData [ flipAP GetOutput ]
        
        MainVolumesUpdate $v
        flipAP Delete
        incr pcount
    }
    IbrowserEndProgressFeedback
    RenderAll
    set tt "Flipped volumes in $iid around vertical axis."
    IbrowserSayThis $tt 0
    IbrowserLowerProgressBar
    
    #--- keeps track of whether the volume is LR flipped
    if { $::Ibrowser($iid,APFlip) == 1 } {
        set ::Ibrowser($iid,APFlip) 0
    } else {
        set ::Ibrowser($iid,APFlip) 1
    }


}




proc IbrowserFlipVolumeIS { } {
global Volume

    #--- Flips a volume around the horizontal axis,
    #--- from up to down, around center.
    #--- Flipped vtkImageData replaces its source    

    set iid $::Ibrowser(activeInterval)
    set firstID $::Ibrowser($iid,firstMRMLid)
    set lastID $::Ibrowser($iid,lastMRMLid)

    IbrowserRaiseProgressBar
    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)

    for { set v $firstID } { $v <= $lastID } { incr v } {
        set progress [ expr double ( $pcount ) / double ( $numvols ) ]
        IbrowserUpdateProgressBar $progress "::"
        #--- without something to make the application idle,
        #--- tk will not handle the drawing of progress bar.
        #--- Instead of calling update, which could cause
        #--- some unstable event loop, we just print some
        #--- '.'s to the tkcon. Slows things down a little,
        #--- but not terribly much. Better ideas are welcome.
        IbrowserPrintProgressFeedback
        
        vtkImageFlip flipIS
        flipIS SetInput [ ::Volume($v,vol) GetOutput ]
        flipIS SetFilteredAxis 2
        ::Volume($v,vol) SetImageData [ flipIS GetOutput ]
        
        MainVolumesUpdate $v
        flipIS Delete
        incr pcount
    }
    IbrowserEndProgressFeedback
    RenderAll
    set tt "Flipped volumes in $iid around horizontal axis."
    IbrowserSayThis $tt 0

    IbrowserLowerProgressBar

    #--- keeps track of whether the volume is IS flipped
    if { $::Ibrowser($iid,ISFlip) == 1 } {
        set ::Ibrowser($iid,ISFlip) 0
    } else {
        set ::Ibrowser($iid,ISFlip) 1
    }

}




proc IbrowserRevertVolumeOrientation { } {
global Volume
    
    #--- Reverts flipped volumes to their original state.
    set iid $::Ibrowser(activeInterval)    
    set firstID $::Ibrowser($iid,firstMRMLid)
    set lastID $::Ibrowser($iid,lastMRMLid)

    IbrowserRaiseProgressBar
    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)

    #--- any vertical flip?
    if { $::Ibrowser($iid,ISFlip) } {

        for { set v $firstID } { $v <= $lastID } { incr v } {
            set progress [ expr double ( $pcount ) / double ( $numvols ) ]
            IbrowserUpdateProgressBar $progress "::"
            #--- without something to make the application idle,
            #--- tk will not handle the drawing of progress bar.
            #--- Instead of calling update, which could cause
            #--- some unstable event loop, we just print some
            #--- '.'s to the tkcon. Slows things down a little,
            #--- but not terribly much. Better ideas are welcome.
            IbrowserPrintProgressFeedback
            
            vtkImageFlip flipIS
            flipIS SetInput [ ::Volume($v,vol) GetOutput ]
            flipIS SetFilteredAxis 2
            ::Volume($v,vol) SetImageData [ flipIS GetOutput ]
            
            MainVolumesUpdate $v
            flipIS Delete
            incr pcount
        }
        IbrowserEndProgressFeedback
        RenderAll
        set ::Ibrowser($iid,ISFlip) 0
    }

    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)
    #--- any horizontal flip?
    if { $::Ibrowser($iid,LRFlip) } {

        for { set v $firstID } { $v <= $lastID } { incr v } {
            set progress [ expr double ( $pcount ) / double ( $numvols ) ]
            IbrowserUpdateProgressBar $progress "::"
            #--- without something to make the application idle,
            #--- tk will not handle the drawing of progress bar.
            #--- Instead of calling update, which could cause
            #--- some unstable event loop, we just print some
            #--- '.'s to the tkcon. Slows things down a little,
            #--- but not terribly much. Better ideas are welcome.
            IbrowserPrintProgressFeedback

            vtkImageFlip flipLR
            flipLR SetInput [ ::Volume($v,vol) GetOutput ]
            flipLR SetFilteredAxis 0
            ::Volume($v,vol) SetImageData [ flipLR GetOutput ]
            
            MainVolumesUpdate $v
            flipLR Delete
            incr pcount
        }
        IbrowserEndProgressFeedback
        RenderAll
        set ::Ibrowser($iid,LRFlip) 0
    }

    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)
    #--- any frontback flip?
    if { $::Ibrowser($iid,APFlip) } {

        for { set v $firstID } { $v <= $lastID } { incr v } {
            set progress [ expr double ( $pcount ) / double ( $numvols ) ]
            IbrowserUpdateProgressBar $progress "::"
            #--- without something to make the application idle,
            #--- tk will not handle the drawing of progress bar.
            #--- Instead of calling update, which could cause
            #--- some unstable event loop, we just print some
            #--- '.'s to the tkcon. Slows things down a little,
            #--- but not terribly much. Better ideas are welcome.
            IbrowserPrintProgressFeedback

            vtkImageFlip flipLR
            flipLR SetInput [ ::Volume($v,vol) GetOutput ]
            flipLR SetFilteredAxis 1
            ::Volume($v,vol) SetImageData [ flipLR GetOutput ]
            
            MainVolumesUpdate $v
            flipLR Delete
            incr pcount
        }
        IbrowserEndProgressFeedback
        RenderAll
        set ::Ibrowser($iid,APFlip) 0
    }

    set tt "Volumes in interval $iid are reset"
    IbrowserSayThis $tt 0
    IbrowserLowerProgressBar
         
}


