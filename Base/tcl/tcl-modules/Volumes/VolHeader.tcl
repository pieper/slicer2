#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        VolHeader.tcl
# PROCEDURES:  
#   VolHeaderInit
#   VolHeaderBuildGUI
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC VolHeaderInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolHeaderInit {} {
    global Volume


    # Define Procedures for communicating with Volumes.tcl
    #---------------------------------------------
    set m VolHeader
       
    # procedure for building GUI in this module's frame
    set Volume(readerModules,$m,procGUI)  ${m}BuildGUI

    # Define Module Description to be used by Volumes.tcl
    #---------------------------------------------
    # name for menu button
    set Volume(readerModules,$m,name)  Header

    # tooltip for help
    set Volume(readerModules,$m,tooltip)  \
            "This tab displays header information\n
    for the currently selected volume."
}

#-------------------------------------------------------------------------------
# .PROC VolHeaderBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolHeaderBuildGUI {parentFrame} {
    global Gui Volume

    #-------------------------------------------
    # Props->Bot->Header frame
    #-------------------------------------------
    set f $parentFrame

    frame $f.fEntry   -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fEntry $f.fApply \
        -side top -fill x -pady 2

    #-------------------------------------------
    # Props->Bot->Header->Entry frame
    #-------------------------------------------

        # Entry fields (the loop makes a frame for each variable)
        foreach param "filePattern" name "{File Pattern}" {

            set f $parentFrame.fEntry
            frame $f.f$param   -bg $Gui(activeWorkspace)
            pack $f.f$param -side top -fill x -pady 2

            set f $f.f$param
            eval {label $f.l$param -text "$name:"} $Gui(WLA)
            eval {entry $f.e$param -width 10 -textvariable Volume($param)} $Gui(WEA)
            pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
            pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
        }

        set Volume(entryBoxWidth) 7

        # two entry boxes per line to save space
        foreach params "{height width} \
                {pixelHeight pixelWidth } "\
                name "{Image Size} \
                {Pixel Size}" \
                tip1 "{height width } {height width }" \
                tip "{units are pixels} \
                {units are mm}" {

            set f $parentFrame.fEntry
            set param [lindex $params 0]
            frame $f.f$param   -bg $Gui(activeWorkspace)
            pack $f.f$param -side top -fill x -pady 2 

            set f $f.f$param
            eval {label $f.l$param -text "$name:"} $Gui(WLA)
            pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
            foreach param $params t $tip1 {
                eval {entry $f.e$param -width $Volume(entryBoxWidth) \
                        -textvariable Volume($param)} $Gui(WEA)
                pack $f.e$param -side right -padx $Gui(pad) 
                TooltipAdd $f.e$param "$t: $tip"
            }
        }

        # now back to one box per line
        foreach param "sliceThickness sliceSpacing" \
                name "{Slice Thickness} {Slice Spacing}" {

            set f $parentFrame.fEntry
            frame $f.f$param   -bg $Gui(activeWorkspace)
            pack $f.f$param -side top -fill x -pady 2

            set f $f.f$param
            eval {label $f.l$param -text "$name:"} $Gui(WLA)
            eval {entry $f.e$param -width $Volume(entryBoxWidth)\
                    -textvariable Volume($param)} $Gui(WEA)
            pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
            pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
        }

    # Scan Order Menu
    set f $parentFrame.fEntry
    frame $f.fscanOrder -bg $Gui(activeWorkspace)
    pack $f.fscanOrder -side top -fill x -pady 2
    
    set f $f.fscanOrder
    eval {label $f.lscanOrder -text "Scan Order:"} $Gui(WLA)
    # button text corresponds to default scan order value Volume(scanOrder)
    eval {menubutton $f.mbscanOrder -relief raised -bd 2 \
        -text [lindex $Volume(scanOrderMenu)\
        [lsearch $Volume(scanOrderList) $Volume(scanOrder)]] \
        -width 10 -menu $f.mbscanOrder.menu} $Gui(WMBA)
    set Volume(mbscanOrder) $f.mbscanOrder
    eval {menu $f.mbscanOrder.menu} $Gui(WMA)
    
    set m $f.mbscanOrder.menu
    foreach label $Volume(scanOrderMenu) value $Volume(scanOrderList) {
        $m add command -label $label -command "VolumesSetScanOrder $value"
    }
    pack $f.lscanOrder -side left -padx $Gui(pad) -fill x -anchor w
    pack $f.mbscanOrder -side left -padx $Gui(pad) -expand 1 -fill x 

    
    # Scalar Type Menu
    set f $parentFrame.fEntry
    frame $f.fscalarType -bg $Gui(activeWorkspace)
    pack $f.fscalarType -side top -fill x -pady 2
    
    set f $f.fscalarType
    eval {label $f.lscalarType -text "Scalar Type:"} $Gui(WLA)
    eval {menubutton $f.mbscalarType -relief raised -bd 2 \
        -text $Volume(scalarType)\
        -width 10 -menu $f.mbscalarType.menu} $Gui(WMBA)
    set Volume(mbscalarType) $f.mbscalarType
    eval {menu $f.mbscalarType.menu} $Gui(WMA)
    
    set m $f.mbscalarType.menu
    foreach type $Volume(scalarTypeMenu) {
        $m add command -label $type -command "VolumesSetScalarType $type"
    }
    pack $f.lscalarType -side left -padx $Gui(pad) -fill x -anchor w
    pack $f.mbscalarType -side left -padx $Gui(pad) -expand 1 -fill x 
    
    # more Entry fields (the loop makes a frame for each variable)
    foreach param "    gantryDetectorTilt numScalars" \
        name "{Slice Tilt} {Num Scalars}" {

        set f $parentFrame.fEntry
        frame $f.f$param -bg $Gui(activeWorkspace)
        pack $f.f$param -side top -fill x -pady 2

        set f $f.f$param
        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 10 -textvariable Volume($param)} $Gui(WEA)
        pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
        pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
    }

    # byte order
    set f $parentFrame.fEntry
    frame $f.fEndian -bg $Gui(activeWorkspace)
    pack $f.fEndian -side top -fill x -pady 2
    set f $f.fEndian

    eval {label $f.l -text "Little Endian (PC,SGI):"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach value "1 0" text "Yes No" width "4 3" {
        eval {radiobutton $f.f.r$value -width $width \
                        -indicatoron 0 -text $text -value $value \
                        -variable Volume(littleEndian) } $Gui(WCA)
        pack $f.f.r$value -side left -fill x
    }

        # odonnell Diffusion tensors (DTI data)
        set f $parentFrame.fEntry
        frame $f.fTensor -bg $Gui(activeWorkspace)
        pack $f.fTensor -side top -fill x -pady 2       
        set f $f.fTensor

        eval {checkbutton $f.cTensor \
                -text "DTI data" -variable Volume(tensors,DTIdata) \
                -indicatoron 0} $Gui(WCA)
        pack $f.cTensor -side left -padx $Gui(pad)
        TooltipAdd $f.cTensor "Diffusion tensor data"

        foreach value "1 0" text "{Swap} {No Swap}" \
                tip {{Phase/Frequency Swapped DTI data (PF swap)} \
                {No Phase/Frequency Swap in DTI data}} {
            eval {radiobutton $f.r$value \
                        -indicatoron 0 -text $text -value $value \
                        -variable Volume(tensors,pfSwap) } $Gui(WCA)
            pack $f.r$value -side left -fill x
            TooltipAdd  $f.r$value $tip
        }


    #-------------------------------------------
    # Props->Bot->Header->Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply

        DevAddButton $f.bApply "Apply" "VolumesPropsApply; RenderAll" 8
        DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

}
