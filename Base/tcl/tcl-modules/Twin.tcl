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
# FILE:        Twin.tcl
# PROCEDURES:  
#   TwinInit
#   TwinBuildVTK
#   TwinBuildGUI
#   TwinApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC TwinInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TwinInit {} {
    global Twin Module

    # Define Tabs
    set m Twin
    set Module($m,row1List) "Help Twin"
    set Module($m,row1Name) "Help Twin"
    set Module($m,row1,tab) Twin

    # Module Summary Info
    set Module($m,overview) "Display another 3D window on a different (MRT) monitor."

    # Define Procedures
    set Module($m,procGUI) TwinBuildGUI
    set Module($m,procVTK) TwinBuildVTK

    set Twin(mode) Off
    set Twin(xPos) 0
    set Twin(yPos) 0
    set Twin(width) 400
    set Twin(height) 300
    set Twin(screen) 0

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.15 $} {$Date: 2002/05/09 14:50:43 $}]
}

#-------------------------------------------------------------------------------
# .PROC TwinBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TwinBuildVTK {} {
    global Twin viewWin twinWin

    vtkXDisplayWindow Twin(display)

    vtkRenderer twinRen

    vtkImageFrameSource Twin(src)
    Twin(src) SetExtent 0 [expr $Twin(width)-1] 0 [expr $Twin(height)-1]
    Twin(src) SetRenderWindow $viewWin
        
    vtkImageMapper Twin(mapper)
    Twin(mapper) SetColorWindow 255
    Twin(mapper) SetColorLevel 127.5
    Twin(mapper) SetInput [Twin(src) GetOutput]
    
    vtkActor2D Twin(actor)
    Twin(actor) SetMapper Twin(mapper)
    twinRen AddActor2D Twin(actor)
}

#-------------------------------------------------------------------------------
# .PROC TwinBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TwinBuildGUI {} {
    global Gui Twin Module

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Twin
    #   Grid
    #   Mode 
    #
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
The Twin window mirrors the 3D view, and can be set to appear on a different
screen number on UNIX systems.  If you have 2 graphics cards in your computer,
then they are numbered 0 and 1.  To change the screen of the Twin window,
you need to change the number on the interface, AND toggle the Twin window
off and on."
    regsub -all "\n" $help { } help
    MainHelpApplyTags Twin $help
    MainHelpBuildGUI Twin

    #-------------------------------------------
    # Twin frame
    #-------------------------------------------
    set fTwin $Module(Twin,fTwin)
    set f $fTwin

    frame $f.fGrid -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fMode -bg $Gui(activeWorkspace)
    pack $f.fMode $f.fGrid \
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Twin->Mode Frame
    #-------------------------------------------
    set f $fTwin.fMode
    
    eval {label $f.l -text "Mode: "} $Gui(WLA)
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach value "On Pause Off" width "3 6 4" {
        eval {radiobutton $f.r$value -width $width \
            -text "$value" -value "$value" -variable Twin(mode) \
            -indicatoron 0 -command "TwinApply"} $Gui(WCA)
        pack $f.r$value -side left -padx 0 -pady 0
    }
        
    #-------------------------------------------
    # Twin->Grid Frame
    #-------------------------------------------
    set f $fTwin.fGrid
    
    # Entry fields (the loop makes a frame for each variable)
    foreach param "xPos yPos width height screen" \
        name "{X Position} {Y Position} {Width} {Height} {Screen Number}" {

        eval {label $f.l$param -text "$name:"} $Gui(WLA)
        eval {entry $f.e$param -width 5 -textvariable Twin($param)} $Gui(WEA)

        grid $f.l$param $f.e$param -padx $Gui(pad) -pady $Gui(pad) -sticky e
        grid $f.e$param -sticky w
    }

    eval {button $f.b -text "Apply" -command "TwinApply"} $Gui(WBA)
    grid $f.b -columnspan 2 -padx $Gui(pad) -pady $Gui(pad) 
}

#-------------------------------------------------------------------------------
# .PROC TwinApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TwinApply {} {
    global Twin twinWin

    if {$Twin(mode) == "On"} {
        # If window does not exist, create it
        if {[info exists twinWin] == 0 || [info command $twinWin] == ""} {
            set twinWin [Twin(display) GetRenderWindow $Twin(screen)]
            $twinWin AddRenderer twinRen
            $twinWin DoubleBufferOn

        }
        $twinWin SetPosition $Twin(xPos) $Twin(yPos)
        $twinWin SetSize $Twin(width) $Twin(height)
        Twin(src) SetExtent 0 [expr $Twin(width)-1] 0 [expr $Twin(height)-1]
        Render3D

    } elseif {$Twin(mode) == "Off"} {
        # If window exists, delete it
        if {[info exists twinWin] == 1 && [info command $twinWin] != ""} {
            $twinWin Delete
        }
    }
}
