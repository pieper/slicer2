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
    frame $f.fProcessInfo -relief groove -bg $::Gui(activeWorkspace) -bd 3 
    pack $f.fProcessMaster -side top -padx 0 -pady $::Gui(pad) -fill x 
    pack $f.fProcessInfo -side top -padx 0 -pady $::Gui(pad) -fill both

    
    #-------------------------------------------
    #--- Catalog of all processing that the Ibrowser can do:
    #--- Developers: Add menu text for all new processing options here.
    #-------------------------------------------
    set ::Ibrowser(processText,Reorient) "Reorient"
    set ::Ibrowser(processText,MotionCorrect) "MotionCorrect"
    set ::Ibrowser(processText,Smooth) "Smooth"

    #-------------------------------------------
    #--- fProcess->fProcessInfo
    #--- fProcess->fProcessInfo:fReorient
    #--- fProcess->fProcessInfo:fMotionCorrect
    #--- fProcess->fProcessInfo:fSmooth
    #--- These are the set of frames inside the
    #--- ProcessInfo frame; one raised for each process.
    #--- Developers: Create new process frames here.
    #-------------------------------------------
    set ff $f.fProcessInfo
    frame $ff.fReorient -bg  $::Gui(activeWorkspace)
    IbrowserBuildReorientGUI $ff.fReorient $f.fProcessInfo

    frame $ff.fMotionCorrect -bg $::Gui(activeWorkspace)
    IbrowserBuildMotionCorrectGUI $ff.fMotionCorrect $f.fProcessInfo

    frame $ff.fSmooth -bg  $::Gui(activeWorkspace)
    IbrowserBuildSmoothGUI $ff.fSmooth $f.fProcessInfo
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
               $::Ibrowser(processText,Reorient) \
               -relief raised -bd 2 -width 25 \
               -menu $ff.mbProcessType.m } $::Gui(WMBA)
    #--- save menu button for configuring its text later
    set ::Ibrowser(guiProcessMenuButton) $ff.mbProcessType
    pack $ff.mbProcessType -side left -pady 1 -padx $::Gui(pad)
    
    #-------------------------------------------
    #--- make menu
    #--- Developers: add your new processes in foreach list
    #-------------------------------------------
    eval { menu $ff.mbProcessType.m } $::Gui(WMA)
    foreach r "Reorient MotionCorrect Smooth" {
        $ff.mbProcessType.m add command -label $r \
            -command "IbrowserRaiseProcessingFrame $::Ibrowser(processText,${r}) $::Ibrowser(fProcess${r})"
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

    $::Ibrowser(guiProcessMenuButton) config -text $menuText
    puts "raising $processFrame"
    raise $processFrame
}

