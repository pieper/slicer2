
proc IbrowserBuildReorientGUI { f master } {

    #--- This GUI allows a volume to be flipped from
    #--- left to right, up to down in display space.
    pack $f -side top -padx 0 -pady $::Gui(pad) -fill both

    #--- set a global variable for frame
    set ::Ibrowser(fProcessReorient) $f
    
    DevAddLabel $f.lSpace "" 
    DevAddLabel $f.lText "Reorient Volume:"
    DevAddLabel $f.lFlipLR "Flip horizontal" 
    DevAddButton $f.bFlipLR "Apply" "IbrowserFlipVolumeLR" 10
    DevAddLabel $f.lFlipUD "Flip vertical"
    DevAddButton $f.bFlipUD "Apply" "IbrowserFlipVolumeUD" 10
    DevAddLabel $f.lRevert "Revert to original"
    DevAddButton $f.bRevert "Apply" "IbrowserRevertVolumeOrientation" 10

    grid $f.lText $f.lSpace
    grid $f.lSpace -sticky w -padx 3 -ipady 5
    grid $f.lText -padx 3 -ipady 5
    
    grid $f.lFlipLR $f.bFlipLR
    grid $f.lFlipLR -sticky w -padx 3 -ipady 3
    grid $f.bFlipLR -padx 3 -ipady 3

    grid $f.lFlipUD $f.bFlipUD
    grid $f.lFlipUD -sticky w -padx 3 -ipady 3
    grid $f.bFlipUD -padx 3 -ipady 3

    grid $f.lRevert $f.bRevert
    grid $f.lRevert -sticky w -padx 3 -ipady 3
    grid $f.bRevert -padx 3 -ipady 3
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
    if { $::Ibrowser($iid,HorizontalFlip) == 1 } {
        set ::Ibrowser($iid,HorizontalFlip) 0
    } else {
        set ::Ibrowser($iid,HorizontalFlip) 1
    }


}




proc IbrowserFlipVolumeUD { } {
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
        
        vtkImageFlip flipUD
        flipUD SetInput [ ::Volume($v,vol) GetOutput ]
        flipUD SetFilteredAxis 2
        ::Volume($v,vol) SetImageData [ flipUD GetOutput ]
        
        MainVolumesUpdate $v
        flipUD Delete
        incr pcount
    }
    IbrowserEndProgressFeedback
    RenderAll
    set tt "Flipped volumes in $iid around horizontal axis."
    IbrowserSayThis $tt 0

    IbrowserLowerProgressBar

    #--- keeps track of whether the volume is UD flipped
    if { $::Ibrowser($iid,VerticalFlip) == 1 } {
        set ::Ibrowser($iid,VerticalFlip) 0
    } else {
        set ::Ibrowser($iid,VerticalFlip) 1
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
    if { $::Ibrowser($iid,VerticalFlip) } {

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
            
            vtkImageFlip flipUD
            flipUD SetInput [ ::Volume($v,vol) GetOutput ]
            flipUD SetFilteredAxis 2
            ::Volume($v,vol) SetImageData [ flipUD GetOutput ]
            
            MainVolumesUpdate $v
            flipUD Delete
            incr pcount
        }
        IbrowserEndProgressFeedback
        RenderAll
        set ::Ibrowser($iid,VerticalFlip) 0
    }

    set pcount 0
    set numvols $::Ibrowser($iid,numDrops)
    #--- any horizontal flip?
    if { $::Ibrowser($iid,HorizontalFlip) } {

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
        set ::Ibrowser($iid,HorizontalFlip) 0
    }
    set tt "Volumes in interval $iid are reset"
    IbrowserSayThis $tt 0
    IbrowserLowerProgressBar
         
}


