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
# FILE:        VolBXH.tcl
# PROCEDURES:  
#   VolBXHInit
#   VolBXHBuildGUI the
#   VolBXHUpdateVolume the
#   VolBXHBuildVTK
#   VolBXHEnter
#   VolBXHExit
#   VolBXHMainFileCloseUpdate
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC VolBXHInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolBXHInit {} {
    global VolBXH Module Volume Model env 

    set m VolBXH

    if {$Module(verbose) == 1} {puts "VolBXHInit starting"}

    set Volume(readerModules,$m,name) "BXH Readers"
    set Volume(readerModules,$m,tooltip) "This tab lets you read in volumes encapsulated in bxh file"
    set Volume(readerModules,$m,procGUI) ${m}BuildGUI
    set Volume(readerModules,$m,procEnter) ${m}Enter
    set Volume(readerModules,$m,procExit) ${m}Exit
    
    # for closing out a scene
    set Volume(VolBXH,idList) ""
    set Module($m,procMainFileCloseUpdateEntered) VolBXHMainFileCloseUpdate

    set VolBXH(modulePath) "$env(SLICER_HOME)/Modules/vtkBXH"

    # Source all appropriate tcl files here. 
    source "$VolBXH(modulePath)/tcl/BXH.tcl"
}


#-------------------------------------------------------------------------------
# .PROC VolBXHBuildGUI
# Builds the GUI for the bxh readers, as a submodule of the Volumes module
# .ARGS
# parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc VolBXHBuildGUI {parentFrame} {
    global Gui VolBXH Module Volume Model

    if {$Module(verbose) == 1} {
        puts  "VolBXHBuildGUI"
    }

    set f $parentFrame
    frame $f.fVolume -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fSlider -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)
    frame $f.fStatus -bg $Gui(activeWorkspace)
 
    pack $f.fVolume $f.fSlider $f.fApply $f.fStatus \
        -side top -fill x -pady $Gui(pad)

    set f $parentFrame.fVolume
    DevAddFileBrowse $f VolBXH "bxh-fileName" "BXH File:" \
        "" "bxh" "\$Volume(DefaultDir)" \
        "Open" "Browse for a BXH file" \
        "" "Absolute"

    set f $parentFrame.fSlider
    DevAddLabel $f.label "Volume No:"
    eval { scale $f.slider \
        -orient horizontal \
        -from 0 -to 0 \
        -resolution 1 \
        -bigincrement 10 \
        -length 160 \
        -state disabled \
        -command {VolBXHUpdateVolume}} \
        $Gui(WSA) {-showvalue 1}

    lappend VolBXH(slider) $f.slider
    pack $f.label $f.slider -side left -expand false -fill x

    set f $parentFrame.fApply
    DevAddButton $f.bApply "Apply" "VolBXHLoadVolumes"  8 
    # If we are in the following data flow:
    # Add Volume -> BXH Readers
    # VolumesPropsCancel will bring you back to the parent frame
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8 
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    set f $parentFrame.fStatus
    set VolBXH(name) ""
    eval {label $f.eName -textvariable VolBXH(name) -width 50} $Gui(WLA)
    pack $f.eName -side left -padx 0 -pady 30
}


#-------------------------------------------------------------------------------
# .PROC VolBXHUpdateVolume
# Updates image volume as user moves the slider 
# .ARGS
# volumeNo the volume number
# .END
#-------------------------------------------------------------------------------
proc VolBXHUpdateVolume {volumeNo} {
    global VolBXH 

    if {$volumeNo == 0} {
#        DevErrorWindow "Volume number must be greater than 0."
        return
    }

    if {[info exists VolBXH($volumeNo,id)] == 0} {
        return
    }

    MainSlicesSetVolumeAll Back $VolBXH($volumeNo,id)
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC VolBXHBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolBXHBuildVTK {} {

}

#-------------------------------------------------------------------------------
# .PROC VolBXHEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolBXHEnter {} {
    global VolBXH
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    # pushEventManager $VolBXH(eventManager)

    # clear the text box and put instructions there
    # $VolBXH(textBox) delete 1.0 end
    # $VolBXH(textBox) insert end "Shift-Click anywhere!\n"

}


#-------------------------------------------------------------------------------
# .PROC VolBXHExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolBXHExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    # popEventManager
}


#-------------------------------------------------------------------------------
# .PROC VolBXHMainFileCloseUpdate
# Called to clean up anything created in this sub module. Deletes Volumes read in, 
# along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolBXHMainFileCloseUpdate {} {
    global Volume viewRen Module

    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "VolBXHMainFileCloseUpdate"
    }
    foreach f $Volume(idList) {
        #if {$Module(verbose) == 1} {}
            puts "VolBXHMainFileCloseUpdate: Checking volume $f"
        
        if {[info exists Volume(VolBXH,$f,curveactor)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing surface actor for bxh reader id $f"
            }
            viewRen RemoveActor  Volume(VolBXH,$f,curveactor)
            Volume(VolBXH,$f,curveactor) Delete
            Volume(VolBXH,$f,mapper) Delete
            Volume($f,vol,rw) Delete
#            MainMrmlDeleteNode Volume $f
        }
        if {[info exists Volume($f,vol,rw)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing volume reader for volume id $f"
            }
            Volume($f,vol,rw) Delete
        }
    }
}

