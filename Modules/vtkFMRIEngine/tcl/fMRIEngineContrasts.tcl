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
# FILE:        fMRIEngineContrasts.tcl
# PROCEDURES:  
#   fMRIEngineBuildUIForContrasts  the
#   fMRIEngineAddOrEditContrast
#   fMRIEngineDeleteContrast
#   fMRIEngineShowContrastToEdit
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC fMRIEngineBuildUIForContrasts 
# Creates UI for task "Contrasts" 
# .ARGS
# parent the parent frame 
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineBuildUIForContrasts {parent} {
    global fMRIEngine Gui

    frame $parent.fTop    -bg $Gui(activeWorkspace) -relief groove -bd 1 
    frame $parent.fMiddle -bg $Gui(activeWorkspace) -relief groove -bd 1
    # frame $parent.fBot    -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $parent.fTop $parent.fMiddle \
        -side top -fill x -pady 2 -padx 1 

    #-------------------------------------------
    # Top frame 
    #-------------------------------------------
    set f $parent.fTop
    frame $f.fTitle    -bg $Gui(activeWorkspace)
    frame $f.fTypes    -bg $Gui(activeWorkspace)
    frame $f.fComp     -bg $Gui(activeWorkspace)
    frame $f.fActions  -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fTypes $f.fComp \
        -side top -fill x -pady 1 -padx 1 
    pack $f.fActions -side top -fill x -pady 1 -padx 1 

    set f $parent.fTop.fTitle
    DevAddLabel $f.l "Compose a contrast:"
    grid $f.l -padx 1 -pady 3

    #-----------------------
    # Type of contrast 
    #-----------------------
    set f $parent.fTop.fTypes
    DevAddLabel $f.l "Type: "
    foreach param "t f" \
        name "{t test} {F test}" {
        eval {radiobutton $f.r$param -width 10 -text $name \
            -variable fMRIEngine(contrastOption) -value $param \
            -relief raised -offrelief raised -overrelief raised \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rt select
    $f.rf configure -state disabled
    grid $f.l $f.rt $f.rf -padx 1 -pady 1 -sticky e

    #-----------------------
    # Compose a contrast 
    #-----------------------
    set f $parent.fTop.fComp
    DevAddLabel $f.lName "Name:"
    eval {entry $f.eName -width 21 -textvariable fMRIEngine(entry,contrastName)} $Gui(WEA)
    grid $f.lName $f.eName -padx 1 -pady 1 -sticky e

    DevAddLabel $f.lVolName "Vol Name:"
    eval {entry $f.eVolName -width 21 -textvariable fMRIEngine(entry,contrastVolName)} $Gui(WEA)
    grid $f.lVolName $f.eVolName -padx 1 -pady 1 -sticky e

    DevAddLabel $f.lExp "Vector:"
    DevAddButton $f.bHelp "?" "fMRIEngineHelpSetupContrasts" 2
    eval {entry $f.eExp -width 21 -textvariable fMRIEngine(entry,contrastVector)} $Gui(WEA)
    grid $f.lExp $f.eExp $f.bHelp -padx 1 -pady 1 -sticky e

    #-----------------------
    # Action panel 
    #-----------------------
    set f $parent.fTop.fActions
    DevAddButton $f.bOK "OK" "fMRIEngineAddOrEditContrast" 6 
    grid $f.bOK -padx 2 -pady 2 

    #-------------------------------------------
    # Middle frame 
    #-------------------------------------------
    #-----------------------
    # Contrast list 
    #-----------------------
    set f $parent.fMiddle
    frame $f.fUp      -bg $Gui(activeWorkspace)
    frame $f.fMiddle  -bg $Gui(activeWorkspace)
    frame $f.fDown    -bg $Gui(activeWorkspace)
    pack $f.fUp $f.fMiddle $f.fDown -side top -fill x -pady 1 -padx 2 

    set f $parent.fMiddle.fUp
    DevAddLabel $f.l "Specified contrasts:"
    grid $f.l -padx 1 -pady 2 

    set f $parent.fMiddle.fMiddle
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set fMRIEngine(contrastsVerScroll) $f.vs
    set fMRIEngine(contrastsHorScroll) $f.hs
    listbox $f.lb -height 4 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::fMRIEngine(contrastsVerScroll) set} \
        -xscrollcommand {$::fMRIEngine(contrastsHorScroll) set}
    set fMRIEngine(contrastsListBox) $f.lb
    $fMRIEngine(contrastsVerScroll) configure -command {$fMRIEngine(contrastsListBox) yview}
    $fMRIEngine(contrastsHorScroll) configure -command {$fMRIEngine(contrastsListBox) xview}

    blt::table $f \
        0,0 $fMRIEngine(contrastsListBox) -padx 1 -pady 1 \
        1,0 $fMRIEngine(contrastsHorScroll) -fill x -padx 1 -pady 1 \
        0,1 $fMRIEngine(contrastsVerScroll) -cspan 2 -fill y -padx 1 -pady 1

    #-----------------------
    # Action  
    #-----------------------
    set f $parent.fMiddle.fDown
    DevAddButton $f.bEdit "Edit" "fMRIEngineShowContrastToEdit" 6 
    DevAddButton $f.bDelete "Delete" "fMRIEngineDeleteContrast" 6 
    grid $f.bEdit $f.bDelete -padx 2 -pady 3 
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineAddOrEditContrast
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineAddOrEditContrast {} {
    global fMRIEngine 

    # Error checking
    set name [string trim $fMRIEngine(entry,contrastName)]
    if {$name == ""} {
        DevErrorWindow "Input a unique name for this contrast."
        return
    }

    set vec [string trim $fMRIEngine(entry,contrastVector)]
    if {$vec == ""} {
        DevErrorWindow "Input the contrast vector."
        return
    }

    set vname [string trim $fMRIEngine(entry,contrastVolName)]
    if {$vname == ""} {
        set vname "ActVol"
    }

    # replace multiple spaces in the middle of the string by one space  
    regsub -all {( )+} $vec " " vec 
    set vecList [split $vec " "]     
    set len [llength $vecList]
    foreach i $vecList { 
        set v [string trim $i]
        set b [string is integer -strict $v]
        if {$b == 0} {
            DevErrorWindow "Input a valid contrast vector." 
            return
        }
    }

    set key "$name-$vname"
    if {! [info exists fMRIEngine($key,contrastName)]} {
        set curs [$fMRIEngine(contrastsListBox) curselection]
        if {$curs != ""} {
            fMRIEngineDeleteContrast
        }
        $fMRIEngine(contrastsListBox) insert end $key 
    } else {
        if {$name == $fMRIEngine($key,contrastName)     &&
            $vname == $fMRIEngine($key,contrastVolName) &&
            $vec == $fMRIEngine($key,contrastVector)} {
            DevErrorWindow "This contrast already exists:\nName: $name\nVol Name: $vname\nVector: $vec"
            return
        }
    }

    set fMRIEngine($key,contrastName) $name
    set fMRIEngine($key,contrastVolName) $vname
    set fMRIEngine($key,contrastVector) $vec
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineDeleteContrast
# Deletes a contrast 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineDeleteContrast {} {
    global fMRIEngine 

    set curs [$fMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$fMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            unset -nocomplain fMRIEngine($name,contrastName) 
            unset -nocomplain fMRIEngine($name,contrastVolName) 
            unset -nocomplain fMRIEngine($name,contrastVector)
        }

        $fMRIEngine(contrastsListBox) delete $curs 
    } else {
        DevErrorWindow "Select a contrast to delete."
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineShowContrastToEdit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineShowContrastToEdit {} {
    global fMRIEngine 

    set curs [$fMRIEngine(contrastsListBox) curselection]
    if {$curs != ""} {
        set name [$fMRIEngine(contrastsListBox) get $curs] 
        if {$name != ""} {
            set fMRIEngine(entry,contrastName) $name
            set fMRIEngine(entry,contrastVector) $fMRIEngine($name,contrastVector) 
        }
    } else {
        DevErrorWindow "Select a contrast to edit."
    }
}


 
