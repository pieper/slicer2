


proc IbrowserBuildMotionCorrectGUI { f master } {
    global Gui

    #--- set global variables for frame so we can raise it,
    #--- and specification params
    set ::Ibrowser(fProcessMotionCorrect) $f
    set ::Ibrowser(Process,MotionCorrectQuality) 1
    set ::Ibrowser(Process,MotionCorrectIterate) 0
    set ::Ibrowser(Process,MotionCorrect,Hardened) 0

    frame $f.fInput -bg $Gui(activeWorkspace) -bd 2 
    frame $f.fModel -bg $Gui(activeWorkspace) -bd 2 -relief groove
    frame $f.fResample -bg $Gui(activeWorkspace) -bd 2 -relief groove

    #---------------------------------------------------------------------------
    #---CHOOSE VOLUMES FRAME

    #--- create menu buttons and associated menus...
    set ff $f.fInput
    eval { label $ff.lChooseProcInterval -text "interval:" } $Gui(WLA)
    eval { menubutton $ff.mbIntervals -text "none" \
               -relief raised -bd 2 -width 20 \
               -menu $ff.mbIntervals.m -indicatoron 1 } $::Gui(WMBA)
    eval { menu $ff.mbIntervals.m } $::Gui(WMA)
    foreach i $::Ibrowser(idList) {
        puts "adding $::Ibrowser($i,name)"
        $ff.mbIntervals.m add command -label $::Ibrowser($i,name) \
            -command "IbrowserSetActiveInterval $i"
    }
    set ::Ibrowser(Process,MotionCorrect,mbIntervals) $ff.mbIntervals
    set ::Ibrowser(Process,MotionCorrect,mIntervals) $ff.mbIntervals.m
    grid $ff.lChooseProcInterval $ff.mbIntervals -pady 1 -padx $::Gui(pad) -sticky e
    grid $ff.mbIntervals -sticky e
    
    eval { label $ff.lReference -text "reference:" } $Gui(WLA)
    eval { menubutton $ff.mbReference -text "none" \
               -relief raised -bd 2 -width 20 -indicatoron 1 \
               -menu $ff.mbReference.m } $::Gui(WMBA)
    eval { menu $ff.mbReference.m } $::Gui(WMA)
    foreach i $::Ibrowser(idList) {
        puts "adding $::Ibrowser($i,name)"
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


proc IbrowserAddSequenceTransforms { } {
    global Data Mrml Volume

    #--- Add Transform, Matrix, and EndTransform
    #--- Transform will enclose each volume node in the sequence.

    #--- ID of selected sequence
    set id $::Ibrowser(Process,SelectSequence)

    #--- for each volume within the sequence:
    for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
        set vid $::Ibrowser($id,$i,MRMLid)
        set node Volume($vid,node)
        DataAddTransform 0 Volume($vid,node) Volume($vid,node)
        puts "adding transform to volume $i"
    }
    MainUpdateMRML
}


proc IbrowserRemoveTransforms { } {

    set stopbutton $::Ibrowser(Process,MotionCorrect,GoStop) 
    #--- check for stopping
    set state [ $stopbutton cget -text ]
    if { $state == "Stop" } {
        $stopbutton configure -text "Go"
    }

    if { $::Ibrowser(Process,MotionCorrect,Hardened) == 0 } {
        #--- ID of selected sequence
        set id $::Ibrowser(Process,SelectSequence)

        #--- For each volume within the interval, delete it's
        #--- innermost transform and matrix.
        #--- We know the node before each volume is a matrix node
        #--- and before that node is the target transform node
        #--- and after the volume is the target end-transform node. 
        #--- So, find each volume node in the sequence:
        for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
            set vid $::Ibrowser($id,$i,MRMLid)
            set node Volume($vid,node)
            #---traverse the mrml tree to find each volume node in sequence.
            ::Mrml(dataTree) InitTraversal
            set tstnode [ Mrml(dataTree) GetNextItem ]
            #--- what element is it in the Mrml tree?
            set whereisVolume 1
            while { $tstnode != "" } {
                if { [string compare -length 6 $tstnode "Volume"] == 0 } {
                    if { [$tstnode GetID ] == $vid } {
                        #--- looks like we found the volume node
                        set gotnode 1
                        break
                    }
                }
                set tstnode [ Mrml(dataTree) GetNextItem ]                
                incr whereisVolume
            }
            #--- remove transform, matrix and end-transform nodes
            if { $gotnode == 1 } {
                ::Mrml(dataTree) InitTraversal
                set counter 0
                set whereisTransform [ expr $whereisVolume - 2 ]
                #--- the three nodes start 2 nodes before the volume node
                while { $counter < $whereisTransform } {
                    set tstnode [ Mrml(dataTree) GetNextItem ]
                    incr counter
                }
                #--- transform node
                set tnode $tstnode
                #--- matrix node
                set mnode [ Mrml(dataTree) GetNextItem ]
                #--- volume node again.
                set tstnode [ Mrml(dataTree) GetNextItem ]
                #--- end transform node
                set endtnode [ Mrml(dataTree) GetNextItem ]
                #--- get the IDs of the three nodes.
                set tid [ $tnode GetID ]
                set mid [ $mnode GetID ]
                set etid [ $endtnode GetID ]
                #--- delete the three nodes.
                MainMrmlDeleteNode Matrix $mid
                MainMrmlDeleteNode Transform $tid
                MainMrmlDeleteNode EndTransform $etid
                MainUpdateMRML
            } else {
                DevErrorWindow "Notice: all transforms were not deleted."
            }
        }
        IbrowserResetSelectSequence
        IbrowserResetInternalReference
        IbrowserSayThis "Motion correction cancelled." 0

    } else {
        DevErrorWindow "Motion correction cannot be undone once hardened."
    }
}



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
    if { [lsearch $::Ibrowser(idList) $::Ibrowser(Process,SelectSequence) ] == -1 } {
        DevErrorWindow "First select a valid sequence to motion correct."
        return
    } elseif { $::Ibrowser(Process,SelectSequence) == $::Ibrowser(idNone) } {
        DevErrorWindow "First select a valid sequence to motion correct."
        return
    }
    
    if { [lsearch $::Volume(idList) $::Ibrowser(Process,SelectInternalReference) ] == -1 } {
        DevErrorWindow "First select a valid reference volume."
        return
    } elseif { $::Ibrowser(Process,SelectInternalReference) == $::Volume(idNone) } {
        DevErrorWindow "First select a valid reference volume."
        return
    }    

    #--- Adds a transform around each volume in the sequence.
    IbrowserAddSequenceTransforms
    set ::Ibrowser(Process,MotionCorrect,Hardened) 0
    $stopbutton configure -text "Stop"
}

proc IbrowserHardenTransforms { } {
    puts "transform hardened."
    set ::Ibrowser(Process,MotionCorrect,Hardened) 1
}

