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
# FILE:        IbrowserProcessGUI.tcl
# PROCEDURES:  
#   IbrowserBuildProcessFrame
#   IbrowserRaiseProcessingFrame
#   IbrowserProcessingSelectInternalReference
#   IbrowserProcessingSelectExternalReference
#   IbrowserResetSelectSequence
#   IbrowserResetInternalReference
#   IbrowserAddSequenceTransforms
#   IbrowserRemoveTransforms
#   IbrowserGetRasToVtkAxis
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserBuildProcessFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildProcessFrame { } {
    global Gui
    
    #-------------------------------------------
    #--- general processing frame
    #-------------------------------------------
    set fProcess $::Module(Ibrowser,fProcess)
    set f $fProcess

    frame $f.fProcessMaster -relief groove -bg $::Gui(backdrop) -bd 3
    #--- This frame's size determines the height of all processing frames
    #--- placed within it. I'd like to have each of the placed frames have
    #--- control the size of the master frame, but can't figure out how
    #--- to do that. For now, Just set the height here when you need more room.
    frame $f.fProcessInfo -relief groove -bg $::Gui(activeWorkspace) -bd 3 -height 350
    pack $f.fProcessMaster -side top -padx 0 -pady $::Gui(pad) -fill x 
    pack $f.fProcessInfo -side top -padx 0 -pady $::Gui(pad) -fill both -expand 1 


    #-------------------------------------------
    #--- Catalog of all processing that the Ibrowser can do:
    #--- Developers: Add menu text for all new processing options here.
    #-------------------------------------------
    set ::Ibrowser(Process,Text,Reassemble) "Reassemble"
    set ::Ibrowser(Process,Text,Reorient) "Reorient"
    set ::Ibrowser(Process,Text,MotionCorrect) "MotionCorrect"
    set ::Ibrowser(Process,Text,Smooth) "Smooth"
    set ::Ibrowser(Process,Text,KeyframeRegister) "KeyframeRegister"

    #-------------------------------------------
    #--- ProcessInfo frame; one raised for each process.
    #--- Developers: Create new process frames here.
    #--- fProcess->fProcessInfo
    #--- fProcess->fProcessInfo:fReorient
    #--- fProcess->fProcessInfo:fMotionCorrect
    #--- fProcess->fProcessInfo:fSmooth
    #--- fProcess->fProcessInfo:fKeyframeRegister
    #--- These are the set of frames for each processing option
    #-------------------------------------------
    set ff $f.fProcessInfo
    frame $ff.fReorient -bg  $::Gui(activeWorkspace)  
    IbrowserBuildReorientGUI $ff.fReorient $f.fProcessInfo

    #--- WJP comment out during development
    frame $ff.fMotionCorrect -bg $::Gui(activeWorkspace) 
    IbrowserBuildMotionCorrectGUI $ff.fMotionCorrect $f.fProcessInfo
    #frame $ff.fSmooth -bg $::Gui(activeWorkspace)
    #IbrowserBuildSmoothGUI $ff.fSmooth $f.fProcessInfo    

    frame $ff.fReassemble -bg $::Gui(activeWorkspace)
    IbrowserBuildReassembleGUI $ff.fReassemble $f.fProcessInfo

    frame $ff.fKeyframeRegister -bg  $::Gui(activeWorkspace) -height 600
    IbrowserBuildKeyframeRegisterGUI $ff.fKeyframeRegister $f.fProcessInfo
    raise $ff.fReorient
    
    #-------------------------------------------
    #--- fProcess->fProcessMaster
    #--- Build pull-down GUI for processes
    #--- inside the ProcessMaster frame
    #--- Developers: Add new processes here.
    #-------------------------------------------
    set ff $f.fProcessMaster
    eval {label $ff.lChoose -text "Select processing: " -width 15 -justify right } $::Gui(BLA)
    pack $ff.lChoose -side left -padx $::Gui(pad) -fill x -anchor w
    #--- build a menu button with a pull-down menu
    #--- of processing options
    eval { menubutton $ff.mbProcessType -text \
               $::Ibrowser(Process,Text,Reorient) \
               -relief raised -bd 2 -width 25 \
               -menu $ff.mbProcessType.m -indicatoron 1} $::Gui(WMBA)
    #--- save menu button for configuring its text later
    set ::Ibrowser(Process,ProcessSelectionButton) $ff.mbProcessType
    pack $ff.mbProcessType -side left -pady 1 -padx $::Gui(pad)
    #-------------------------------------------
    #--- make menu that pulls down from menubutton.
    #--- Developers: add your new processes in foreach list
    #-------------------------------------------
    eval { menu $ff.mbProcessType.m } $::Gui(WMA)

    #--- do not expose until code is complete
    #--- WJP comment out during development
    #foreach r "Reassemble Reorient Smooth MotionCorrect KeyframeRegister" 
    foreach r "Reassemble Reorient MotionCorrect KeyframeRegister" {
        $ff.mbProcessType.m add command -label $r \
            -command "IbrowserRaiseProcessingFrame $::Ibrowser(Process,Text,${r}) $::Ibrowser(fProcess${r})"
    }

    #--- By default, raise first frame on th process list.
    raise $::Ibrowser(fProcessReorient)
}




#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseProcessingFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseProcessingFrame { menuText processFrame } {

    $::Ibrowser(Process,ProcessSelectionButton) config -text $menuText
    raise $processFrame
}





#-------------------------------------------------------------------------------
# .PROC IbrowserProcessingSelectInternalReference
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserProcessingSelectInternalReference { name id } {
    #---
    #--- name gives the text that goes on the menu
    #--- and id gives the MRML id of the volume in the sequence
    #--- to be used as the reference volume.
    #--- specifies a reference volume within the sequence being processed.
    set ::Ibrowser(Process,InternalReference) $id
    foreach process "MotionCorrect KeyframeRegister" {
        if { [info exists ::Ibrowser(Process,$process,mbReference)] } {
            $::Ibrowser(Process,$process,mbReference) config -text $name
        }
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserProcessingSelectExternalReference
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserProcessingSelectExternalReference { name id } {
    #---
    #---specifies a reference sequence outside the sequence being processed.
    set ::Ibrowser(Process,ExternalReference) $id
    foreach process "MotionCorrect KeyframeRegister" {
        if { [info exists ::Ibrowser(Process,$process,mbReference)] } {
            $::Ibrowser(Process,$process,mbReference) config -text $name
        }
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserResetSelectSequence
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserResetSelectSequence { } {
    #---
    #--- sets the selected sequence to be "none" in all menus.
    set ::Ibrowser(activeInterval) $::Ibrowser(idNone)
    #set ::Ibrowser(Process,SelectSequence) $::Ibrowser(idNone)
    IbrowserUpdateMRML
}



#-------------------------------------------------------------------------------
# .PROC IbrowserResetInternalReference
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserResetInternalReference { } {

    #--- Each time the user selects a new sequence,
    #--- a new reference should be specified. So, this proc
    #--- resets the reference to 'none' when the
    #--- sequence has changed. 
    set ::Ibrowser(Process,SelectIntReference) $::Volume(idNone)
    IbrowserUpdateMRML


}




proc IbrowserAddNonReferenceSequenceTransforms { } {

    #--- Add Transform, Matrix, and EndTransform
    #--- Transform will enclose each volume node in the active interval.

    #--- ID of selected sequence
    set refvol $::Ibrowser(Process,InternalReference)
    set id $::Ibrowser(activeInterval)
    IbrowserRaiseProgressBar
    set pcount 0
    IbrowserSayThis "Adding transforms for $::Ibrowser($id,name)..." 0
    
    #--- for each volume within the sequence:
    #--- add a new transform node for each volume
    for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
        if { $::Ibrowser($id,numDrops)  != 0 } {
            set progress [ expr double ($pcount) / double ($::Ibrowser($id,numDrops)) ]
            IbrowserUpdateProgressBar $progress "::"
            IbrowserPrintProgressFeedback
        }
        set vid $::Ibrowser($id,$i,MRMLid)
        #--- exclude the reference volume, which should not have a transform.
        if { $vid != $refvol } {
            set node Volume($vid,node)
            set mid [ DataAddTransform 0 Volume($vid,node) Volume($vid,node) ]
            #--- save a way to find this matrix later.
            set ::Ibrowser($id,$i,matrixID) $mid
            incr pcount
        }
    }
    IbrowserEndProgressFeedback
    IbrowserSayThis "Transforms for $::Ibrowser($id,name) added." 0
    MainUpdateMRML
    IbrowserLowerProgressBar
}


#-------------------------------------------------------------------------------
# .PROC IbrowserAddSequenceTransforms
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserAddSequenceTransforms { } {
    global Data Mrml Volume

    #--- Add Transform, Matrix, and EndTransform
    #--- Transform will enclose each volume node in the active interval.

    #--- ID of selected sequence
    set id $::Ibrowser(activeInterval)
    IbrowserRaiseProgressBar
    set pcount 0
    IbrowserSayThis "Adding transforms for $::Ibrowser($id,name)..." 0
    
    #--- for each volume within the sequence:
    #--- add a new transform node for each volume
    for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
        if { $::Ibrowser($id,numDrops)  != 0 } {
            set progress [ expr double ($pcount) / double ($::Ibrowser($id,numDrops)) ]
            IbrowserUpdateProgressBar $progress "::"
            IbrowserPrintProgressFeedback
        }
        
        set vid $::Ibrowser($id,$i,MRMLid)
        set node Volume($vid,node)
        set mid [ DataAddTransform 0 Volume($vid,node) Volume($vid,node) ]
        #--- save a way to find this matrix later.
        set ::Ibrowser($id,$i,matrixID) $mid
        incr pcount
    }
    IbrowserEndProgressFeedback
    IbrowserSayThis "Transforms for $::Ibrowser($id,name) added." 0
    MainUpdateMRML
    IbrowserLowerProgressBar
}




proc IbrowserRemoveNonReferenceTransforms { } {

    set refvol $::Ibrowser(Process,InternalReference)
    #--- ID of selected sequence
    set id $::Ibrowser(activeInterval)

    #--- For each volume within the interval, delete it's
    #--- innermost transform and matrix.
    #--- We know the node before each volume is a matrix node
    #--- and before that node is the target transform node
    #--- and after the volume is the target end-transform node. 
    #--- So, find each volume node in the sequence:
    IbrowserSayThis "Removing transforms for $::Ibrowser($id,name)..." 0
    IbrowserRaiseProgressBar
    set pcount 0
    
    for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
        if { $::Ibrowser($id,numDrops) != 0 } {
            set progress [ expr double ($pcount) / double ($::Ibrowser($id,numDrops)) ]
            IbrowserUpdateProgressBar $progress "::"
            IbrowserPrintProgressFeedback
        }
        
        set vid $::Ibrowser($id,$i,MRMLid)
        #--- exclude the reference volume which had no transform added.
        if {$vid != $refvol } {
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
            incr pcount
        }
    }
    IbrowserEndProgressFeedback
    IbrowserResetSelectSequence
    IbrowserResetInternalReference
    IbrowserSayThis "Transforms for $::Ibrowser($id,name) deleted." 0
    IbrowserLowerProgressBar

}


#-------------------------------------------------------------------------------
# .PROC IbrowserRemoveTransforms
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRemoveTransforms { } {

    #--- ID of selected sequence
    set id $::Ibrowser(activeInterval)

    #--- For each volume within the interval, delete it's
    #--- innermost transform and matrix.
    #--- We know the node before each volume is a matrix node
    #--- and before that node is the target transform node
    #--- and after the volume is the target end-transform node. 
    #--- So, find each volume node in the sequence:
    IbrowserSayThis "Removing transforms for $::Ibrowser($id,name)..." 0
    IbrowserRaiseProgressBar
    set pcount 0
    
    for { set i 0 } { $i < $::Ibrowser($id,numDrops) } { incr i } {
        if { $::Ibrowser($id,numDrops) != 0 } {
            set progress [ expr double ($pcount) / double ($::Ibrowser($id,numDrops)) ]
            IbrowserUpdateProgressBar $progress "::"
            IbrowserPrintProgressFeedback
        }
        
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
        incr pcount
    }
    IbrowserEndProgressFeedback
    IbrowserResetSelectSequence
    IbrowserResetInternalReference
    IbrowserSayThis "Transforms for $::Ibrowser($id,name) deleted." 0
    IbrowserLowerProgressBar

}



#-------------------------------------------------------------------------------
# .PROC IbrowserGetRasToVtkAxis
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGetRasToVtkAxis { axis vnode } {
    #
    #--- Given an axis in RAS space, we want
    #--- to find the axis that corresponds in VTK 
    #--- space (which describes the vtkImageData
    #--- represented by a vtkMrmlVolumeNode.)
    #
    #--- create a vtkTransform
    vtkTransform T
    #--- get the transform matrix in string form
    #--- and set it to be the transform's matrix.
    set m [ $vnode GetRasToVtkMatrix ]
    eval T SetMatrix $m

    #--- axis along which to flip in VTK space
    if { $axis == "RL" } {
        #--- if Flipping along R-L
        #puts "RAS axis 1 0 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 -1 0 0 ]
    } elseif { $axis == "AP" } {
        #--- if flipping along A-P
        #puts "RAS axis 0 1 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 -1 0 ]
    } elseif { $axis == "SI" } {
        #--- if flipping along S-I
        #puts "RAS axis 0 0 1"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 0 -1 ]
    }
    foreach { x y z } $newvec { }
    #puts "VTK axis: $x $y $z"
    #--- because of scale, newvec might not be unit
    if { $x != 0 } {
        set newvec [ lreplace $newvec 0 0 1 ]
    }
    if { $y != 0 } {
        set newvec [ lreplace $newvec 1 1 1 ]
    }
    if { $z != 0 } {
        set newvec [ lreplace $newvec 2 2 1 ]
    }
    
    #--- clean up
    T Delete
    #--- return the new axis as a vector.
    return $newvec
}

