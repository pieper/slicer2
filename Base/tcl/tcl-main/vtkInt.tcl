#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        vtkInt.tcl
# PROCEDURES:  
#   vtkInteract
#==========================================================================auto=
# a generic interactor for tcl and vtk
#
catch {unset vtkInteract.bold}
catch {unset vtkInteract.normal}
catch {unset vtkInteract.tagcount}
set vtkInteractBold "-background #43ce80 -foreground #221133 -relief raised -borderwidth 1"
set vtkInteractNormal "-background #dddddd -foreground #221133 -relief flat"
set vtkInteractTagcount 1
set vtkInteractCommandList ""
set vtkInteractCommandIndex 0

#-------------------------------------------------------------------------------
# .PROC vtkInteract
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkInteract {} {
    global vtkInteractCommandList vtkInteractCommandIndex
    global vtkInteractTagcount

    proc dovtk {s w} {
    global vtkInteractBold vtkInteractNormal vtkInteractTagcount 
    global vtkInteractCommandList vtkInteractCommandIndex

    set tag [append tagnum $vtkInteractTagcount]
        set vtkInteractCommandIndex $vtkInteractTagcount
    incr vtkInteractTagcount 1
    .vtkInteract.display.text configure -state normal
    .vtkInteract.display.text insert end $s $tag
    set vtkInteractCommandList [linsert $vtkInteractCommandList end $s]
    eval .vtkInteract.display.text tag configure $tag $vtkInteractNormal
    .vtkInteract.display.text tag bind $tag <Any-Enter> \
        ".vtkInteract.display.text tag configure $tag $vtkInteractBold"
    .vtkInteract.display.text tag bind $tag <Any-Leave> \
        ".vtkInteract.display.text tag configure $tag $vtkInteractNormal"
    .vtkInteract.display.text tag bind $tag <1> "dovtk [list $s] .vtkInteract"
    .vtkInteract.display.text insert end \n;
    .vtkInteract.display.text insert end [uplevel 1 $s]
    .vtkInteract.display.text insert end \n\n
    .vtkInteract.display.text configure -state disabled
    .vtkInteract.display.text yview end
    }

    catch {destroy .vtkInteract}
    toplevel .vtkInteract -bg #bbbbbb
    wm title .vtkInteract "vtk Interactor"
    wm iconname .vtkInteract "vtk"
    
    frame .vtkInteract.buttons -bg #bbbbbb
    pack  .vtkInteract.buttons -side bottom -fill both -expand 0 -pady 2m
    button .vtkInteract.buttons.dismiss -text Dismiss \
    -command "wm withdraw .vtkInteract" \
    -bg #bbbbbb -fg #221133 -activebackground #cccccc -activeforeground #221133
    pack .vtkInteract.buttons.dismiss -side left -expand 1 -fill x
    
    frame .vtkInteract.file -bg #bbbbbb
    label .vtkInteract.file.label -text "Command:" -width 10 -anchor w \
    -bg #bbbbbb -fg #221133
    entry .vtkInteract.file.entry -width 40 \
    -bg #dddddd -fg #221133 -highlightthickness 1 -highlightcolor #221133
    bind .vtkInteract.file.entry <Return> {
    dovtk [%W get] .vtkInteract; %W delete 0 end}
    pack .vtkInteract.file.label -side left
    pack .vtkInteract.file.entry -side left -expand 1 -fill x
    
    frame .vtkInteract.display -bg #bbbbbb
    text .vtkInteract.display.text -yscrollcommand ".vtkInteract.display.scroll set" \
    -setgrid true -width 60 -height 8 -wrap word -bg #dddddd -fg #331144 \
    -state disabled
    scrollbar .vtkInteract.display.scroll \
    -command ".vtkInteract.display.text yview" -bg #bbbbbb \
    -troughcolor #bbbbbb -activebackground #cccccc -highlightthickness 0 
    pack .vtkInteract.display.text -side left -expand 1 -fill both
    pack .vtkInteract.display.scroll -side left -expand 0 -fill y

    pack .vtkInteract.display -side bottom -expand 1 -fill both
    pack .vtkInteract.file -pady 3m -padx 2m -side bottom -fill x 

    set vtkInteractCommandIndex 0
    
    bind .vtkInteract <Down> {
      if { $vtkInteractCommandIndex < [expr $vtkInteractTagcount - 1] } {
        incr vtkInteractCommandIndex
        set command_string [lindex $vtkInteractCommandList $vtkInteractCommandIndex]
        .vtkInteract.file.entry delete 0 end
        .vtkInteract.file.entry insert end $command_string
      } elseif { $vtkInteractCommandIndex == [expr $vtkInteractTagcount - 1] } {
        .vtkInteract.file.entry delete 0 end
      }
    }

    bind .vtkInteract <Up> {
      if { $vtkInteractCommandIndex > 0 } { 
        set vtkInteractCommandIndex [expr $vtkInteractCommandIndex - 1]
        set command_string [lindex $vtkInteractCommandList $vtkInteractCommandIndex]
        .vtkInteract.file.entry delete 0 end
        .vtkInteract.file.entry insert end $command_string
      }
    }

    wm withdraw .vtkInteract
}

vtkInteract
