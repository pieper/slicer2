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
# FILE:        IbrowserProcessGUI.tcl
# PROCEDURES:  
#   IbrowserBuildProcessFrame
#   IbrowserRaiseProcessingFrame
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
    frame $f.fProcessInfo -relief groove -bg $::Gui(activeWorkspace) -bd 3 -height 350 
    pack $f.fProcessMaster -side top -padx 0 -pady $::Gui(pad) -fill x 
    pack $f.fProcessInfo -side top -padx 0 -pady $::Gui(pad) -fill both -expand 1 


    #-------------------------------------------
    #--- Catalog of all processing that the Ibrowser can do:
    #--- Developers: Add menu text for all new processing options here.
    #-------------------------------------------
    set ::Ibrowser(Process,Text,Reorient) "Reorient"
    #set ::Ibrowser(Process,Text,MotionCorrect) "MotionCorrect"
    #set ::Ibrowser(Process,Text,Smooth) "Smooth"
    #set ::Ibrowser(Process,Text,Coregister) "Co-register"

    #-------------------------------------------
    #--- ProcessInfo frame; one raised for each process.
    #--- Developers: Create new process frames here.
    #--- fProcess->fProcessInfo
    #--- fProcess->fProcessInfo:fReorient
    #--- fProcess->fProcessInfo:fMotionCorrect
    #--- fProcess->fProcessInfo:fSmooth
    #--- fProcess->fProcessInfo:fCoregister
    #--- These are the set of frames inside the
    #-------------------------------------------
    set ff $f.fProcessInfo
    frame $ff.fReorient -bg  $::Gui(activeWorkspace)  
    IbrowserBuildReorientGUI $ff.fReorient $f.fProcessInfo

    #--- do not expose until code is completed.
    if { 0 } {
        frame $ff.fMotionCorrect -bg $::Gui(activeWorkspace) 
        IbrowserBuildMotionCorrectGUI $ff.fMotionCorrect $f.fProcessInfo

        frame $ff.fCoregister -bg  $::Gui(activeWorkspace)
        IbrowserBuildCoregisterGUI $ff.fCoregister $f.fProcessInfo

        frame $ff.fSmooth -bg  $::Gui(activeWorkspace) 
        IbrowserBuildSmoothGUI $ff.fSmooth $f.fProcessInfo
    }
    raise $ff.fReorient
    
    #-------------------------------------------
    #--- fProcess->fProcessMaster
    #--- Build pull-down GUI for processes
    #--- inside the ProcessMaster frame
    #--- Developers: Add new processes here.
    #-------------------------------------------
    set ff $f.fProcessMaster
    eval {label $ff.lChoose -text "Select processing: " -width 20 -justify right } $::Gui(BLA)
    pack $ff.lChoose -side left -padx $::Gui(pad) -fill x -anchor w
    #--- build a menu button with a pull-down menu
    #--- of processing options
    eval { menubutton $ff.mbProcessType -text \
               $::Ibrowser(Process,Text,Reorient) \
               -relief raised -bd 2 -width 25 \
               -menu $ff.mbProcessType.m } $::Gui(WMBA)
    #--- save menu button for configuring its text later
    set ::Ibrowser(Process,ProcessSelectionButton) $ff.mbProcessType
    pack $ff.mbProcessType -side left -pady 1 -padx $::Gui(pad)
    #-------------------------------------------
    #--- make menu that pulls down from menubutton.
    #--- Developers: add your new processes in foreach list
    #-------------------------------------------
    eval { menu $ff.mbProcessType.m } $::Gui(WMA)

    #--- do not expose until code is complete
    #foreach r "Reorient MotionCorrect Smooth Coregister" 
    foreach r "Reorient" {
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
    puts "raising $processFrame"
    raise $processFrame
}



proc IbrowserProcessingSelectSequence { id } {

    set ::Ibrowser(Process,SelectSequence)  $id
    IbrowserResetInternalReference
    set name $::Ibrowser($id,name)
    $::Ibrowser(Process,MotionCorrect,mbSequence) config -text $name
    IbrowserConfigureInternalReference
}



proc IbrowserProcessingSelectInternalReference { name id } {
    #--- name gives the text that goes on the menu
    #--- and id gives the MRML id of the volume in the sequence
    #--- to be used as the reference volume.
    #--- specifies a reference volume within the sequence being processed.
    set ::Ibrowser(Process,SelectInternalReference) $id
    $::Ibrowser(Process,MotionCorrect,mbIntReference) config -text $name
}



proc IbrowserProcessingSelectExternalReference { name id } {
    #---specifies a reference sequence outside the sequence being processed.
    set ::Ibrowser(Process,SelectExternalReference) $id
    $::Ibrowser(Process,Coregister,mbExtReference) config -text $name

}



proc IbrowserConfigureInternalReference { } {

    #--- this menu gets configured based on what sequence is selected.
    #--- the menu should contain the volume numbers or names of
    #--- volumes within a selected sequence.
    set s $::Ibrowser(idNone)
    set v $::Volume(idNone)
    
    #---configure Menus of reference volumes
    set m $::Ibrowser(Process,MotionCorrect,mIntReference)
    $m delete 0 end
    set start 0
    set stop 0
    if { $::Ibrowser(Process,SelectSequence) != $v } {
        set start $::Ibrowser($::Ibrowser(Process,SelectSequence),firstMRMLid)
        set stop $::Ibrowser($::Ibrowser(Process,SelectSequence),lastMRMLid)
    }
    set count 0
    #---based on which interval drop is selected,
    #---set the MRMLid of the volume that drop represents.
    for { set i $start } { $start < $stop } { incr start } {
        set id $::Ibrowser(Process,SelectSequence)
        set name $::Ibrowser($id,name)
        set name $name\_$count
        $m add command -label $name \
            -command "IbrowserProcessingSelectInternalReference $name $::Ibrowser($id,$start,MRMLid)"
        incr count
    }
}



proc IbrowserResetSelectSequence { } {

    set s $::Ibrowser(idNone)
    set ::Ibrowser(Process,SelectSequence) $s
    #---configure sequence menu
    $::Ibrowser(Process,MotionCorrect,mbSequence) config -text "none"
}



proc IbrowserResetInternalReference { } {

    #--- Each time the user selects a new sequence,
    #--- a new reference should be specified. So, this proc
    #--- resets the internal reference to 'none' when the
    #--- sequence has changed. 
    set v $::Volume(idNone)
    set ::Ibrowser(Process,SelectIntReference) $v
    #---configure Menus of reference volumes
    set m $::Ibrowser(Process,MotionCorrect,mIntReference)
    $m delete 0 end
    $::Ibrowser(Process,MotionCorrect,mbIntReference) config -text "none"

}
