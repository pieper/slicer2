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
# FILE:        Slices.tcl
# PROCEDURES:  
#   SlicesBuildGUI
#   SlicesUpdateMRML
#   SlicesSetIncrSource 
#   SlicesSetIncrs 
#==========================================================================auto=

proc SlicesInit {} {
    global Module Slice

    # Define Tabs
    set m Slices
    set Module($m,row1List) "Help Controls"
    set Module($m,row1Name) "{Help} {Controls}"
    set Module($m,row1,tab) Controls

    # Module Summary Info
    set Module($m,overview) "Display of the 3 slices."
    set Module($m,author) "Core"
    set Module($m,category) [lindex $Module(categories) 0]

    # Define Procedures
    set Module($m,procGUI) SlicesBuildGUI
    set Module($m,procMRML)  SlicesUpdateMRML

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.22 $} {$Date: 2004/03/15 21:04:51 $}]

    # Props
    set Slice(prefix) slice
    set Slice(ext) .tif
    set Slice(IncrsSource) $::Volume(idNone)
}

#-------------------------------------------------------------------------------
# .PROC SlicesBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SlicesBuildGUI {} {
    global Slice Module Gui

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Controls
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
Use these slice controls when the <B>View Mode</B> is set to 3D.
<BR>
The <B>Active Slice</B> is the slice you last clicked on with the mouse.
<BR>
<B>TIP</B></BR>
The AxiSlice, SagSlice, CorSlice, and OrigSlice orientations
produces slices relative to the originally scanned volume.
The other options produces slices at arbitrary orientations in millimeter space.
"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Slices $help
    MainHelpBuildGUI Slices

    #-------------------------------------------
    # Controls frame
    #-------------------------------------------
    set fControls $Module(Slices,fControls)
    set f $fControls

    # Controls->Slice$s frames
    #-------------------------------------------
    foreach s $Slice(idList) {

        frame $f.fSlice$s -bg $Gui(activeWorkspace)
        pack $f.fSlice$s -side top -pady $Gui(pad) -expand 1 -fill both

        MainSlicesBuildControls $s $f.fSlice$s
    }

    frame $f.fActive -bg $Gui(activeWorkspace)
    frame $f.fSave   -bg $Gui(activeWorkspace)
    frame $f.fAdv   -bg $Gui(activeWorkspace)
    frame $f.fIncrs   -bg $Gui(activeWorkspace)
    pack $f.fActive $f.fSave $f.fAdv $f.fIncrs -side top -pady $Gui(pad) \
        -expand 1 -fill x

    #-------------------------------------------
    # Active frame
    #-------------------------------------------
    set f $fControls.fActive

    eval {label $f.lActive -text "Active Slice:"} $Gui(WLA)
    pack $f.lActive -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach s $Slice(idList) text "Red Yellow Green" width "4 7 6" {
        eval {radiobutton $f.r$s -width $width -indicatoron 0\
            -text "$text" -value "$s" -variable Slice(activeID) \
            -command "MainSlicesSetActive"} $Gui(WCA) {-selectcolor $Gui(slice$s)}
        pack $f.r$s -side left -fill x -anchor e
    }

    #-------------------------------------------
    # Save frame
    #-------------------------------------------
    set f $fControls.fSave

    eval {button $f.bSave -text "Save Active" -width 12 \
        -command "MainSlicesSave"} $Gui(WBA)
    eval {entry $f.eSave -textvariable Slice(prefix)} $Gui(WEA)
    bind $f.eSave <Return> {MainSlicesSavePopup}
    pack $f.bSave -side left -padx 3
    pack $f.eSave -side left -padx 2 -expand 1 -fill x

    #-------------------------------------------
    # Adv frame
    #-------------------------------------------
    set f $fControls.fAdv

    eval {button $f.bAdv -text "Show Advanced Slice Controls" \
        -command "MainSlicesAdvancedControlsPopup \$Slice(activeID)"} $Gui(WBA)
    pack $f.bAdv -side left -padx 3

    #-------------------------------------------
    # Incrs frame
    #-------------------------------------------
    set f $fControls.fIncrs
    # Volume menu
    eval {label $f.lIncrs -text "Get Incrs:"} $Gui(WTA)
    
    eval {menubutton $f.mbIncrs -text "None" -relief raised -bd 2 -width 18 \
        -menu $f.mbIncrs.m} $Gui(WMBA)
    eval {menu $f.mbIncrs.m} $Gui(WMA)
    TooltipAdd $f.mbIncrs "Choose the input volume for defining slice increments."
    
    eval {button $f.bIncrs -text "Set" -command SlicesSetIncrs} $Gui(WBA)
    TooltipAdd $f.bIncrs "Set the slice increments to the smallest spacing in selected volume."

    pack $f.lIncrs -padx $Gui(pad) -side left -anchor e
    pack $f.mbIncrs -padx $Gui(pad) -side left -anchor w
    pack $f.bIncrs -padx $Gui(pad) -side left -anchor w

    # Save widgets for changing
    set Slice(mbIncrs) $f.mbIncrs
    set Slice(mIncrs)  $f.mbIncrs.m

}

proc SlicesUpdateMRML {} {
    global Slice Volume
    # Incr Volume menu - update with current list of volumes
    #---------------------------------------------------------------------------
    set m $Slice(mIncrs)
    $m delete 0 end
    foreach v $Volume(idList) {
        $m add command -label [Volume($v,node) GetName] \
            -command "SlicesSetIncrSource $v"
    }
}

proc SlicesSetIncrSource {v} {
    global Slice Volume
    $Slice(mbIncrs) config -text [Volume($v,node) GetName]
    set Slice(IncrsSource) $v
}

proc SlicesSetIncrs {} {
    global Slice Volume

    if { $Slice(IncrsSource) == $Volume(idNone) } {
        return
    }

    set spacings [Volume($Slice(IncrsSource),node) GetSpacing]
    set minspacing [lindex $spacings 0]
    for {set i 1} {$i < 3} {incr i} {
        set spi [lindex $spacings $i]
        if { $spi < $minspacing } {
            set minspacing $spi
        }
    }

    for {set s 0} {$s < 3} {incr s} {
        MainSlicesSetOffsetIncrement $s $minspacing
    }
}
