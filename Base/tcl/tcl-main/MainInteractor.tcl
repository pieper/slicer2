#=auto==========================================================================
# (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
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
# FILE:        MainInteractor.tcl
# PROCEDURES:  
#   MainInteractorInit
#   MainInteractorBind widget
#   MainInteractorXY s x y
#   MainInteractorCursor s xs ys x y
#   MainInteractorKeyPress key widget x y
#   MainInteractorMotion widget x y
#   MainInteractorShiftMotion widget x y
#   MainInteractorB1 widget x y
#   MainInteractorShiftB1 widget x y
#   MainInteractorB1Release widget x y
#   MainInteractorShiftB1Release widget x y
#   MainInteractorB1Motion widget x y
#   MainInteractorControlB1Motion widget x y
#   MainInteractorAltB1Motion widget x y
#   MainInteractorB2Motion widget x y
#   MainInteractorB3Motion widget x y
#   MainInteractorPan s x y xLast yLast
#   MainInteractorZoom s x y xLast yLast
#   MainInteractorWindowLevel s x y xLast yLast
#   MainInteractorThreshold s x y xLast yLast
#   MainInteractorExpose widget
#   MainInteractorRender
#   MainInteractorEnter widget x y
#   MainInteractorExit widget
#   MainInteractorStartMotion widget x y
#   MainInteractorEndMotion widget x y
#   MainInteractorReset widget x y
#   PixelsToMm pix fov dim mag
#   Distance3D x1 y1 z1 x2 y2 z2
#   Angle2D ax1 ay1 ax2 ay2 bx1 by1 bx2 by2
#   MainInteractorControlB1   widget x y
#   MainInteractorControlB1Release   widget x y
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainInteractorInit
# Set up default variables, set the active Slicer instance.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainInteractorInit {} {
    global Interactor

    set Interactor(s)      0
    set Interactor(yLast)  0
    set Interactor(xLast)  0
    set Interactor(ysLast) 0
    set Interactor(xsLast) 0

    #Added the following variable and set it to be Slicer so 
    #that "Slicer" is not hardcoded into each procedure making 
    #it easier to allow window interactions in the same manner 
    #if ever another vtkMrmlSlicer object is added.
    set Interactor(activeSlicer) Slicer
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorBind
# Set up bindings on the view window.
# .ARGS
# windowpath widget the widget for which the bindings are being added.
# .END
#-------------------------------------------------------------------------------
proc MainInteractorBind {widget} {
    global Gui


    # NOTE: <Motion> is not called when <B1-Motion> is called
    #       <Any-ButtonPress> is not called when <Button-1> is called
    
    # Cursor
    bind $widget <Enter>             {MainInteractorEnter %W %x %y;}
    bind $widget <Leave>             {MainInteractorExit %W}
    bind $widget <Expose>            {MainInteractorExpose %W}
    bind $widget <Motion>            {MainInteractorMotion %W %x %y}
    bind $widget <Shift-Motion>      {MainInteractorShiftMotion %W %x %y}

    # Any mouse button
    bind $widget <Any-ButtonPress>   {MainInteractorStartMotion %W %x %y}
    bind $widget <Any-ButtonRelease> {MainInteractorEndMotion %W %x %y}
    # B1
    bind $widget <ButtonPress-1>     {MainInteractorB1 %W %x %y}
    bind $widget <ButtonRelease-1>   {MainInteractorB1Release %W %x %y}
    bind $widget <B1-Motion>         {MainInteractorB1Motion %W %x %y}
    # B2
    bind $widget <B2-Motion>         {MainInteractorB2Motion %W %x %y}
    # B3
    bind $widget <B3-Motion>         {MainInteractorB3Motion %W %x %y}

    # Shift-B1
    bind $widget <Shift-B1-Motion>        {MainInteractorB2Motion %W %x %y}
    bind $widget <Shift-ButtonPress-1>    {MainInteractorShiftB1 %W %x %y}
    bind $widget <Shift-ButtonRelease-1> \
        {MainInteractorShiftB1Release %W %x %y}

    # Control-B1
    bind $widget <Control-B1-Motion>      {MainInteractorControlB1Motion %W %x %y}
    bind $widget <Control-ButtonPress-1>  {MainInteractorControlB1 %W %x %y}
    bind $widget <Control-ButtonRelease-1> \
        {MainInteractorControlB1Release %W %x %y}

    # NOTE: since this has been disabled for years, AltB1 is now
    # mapped to PAN to support two-button mice. - sp 2002-11-14
    # Alt-B1
    #bind $widget <Alt-B1-Motion>          {MainInteractorAltB1Motion %W %x %y}
    #bind $widget <Alt-ButtonPress-1>      {MainInteractorShiftB1 %W %x %y}
    #bind $widget <Alt-ButtonRelease-1>    {MainInteractorShiftB1Release %W %x %y}
    bind $widget <Alt-ButtonPress-1>     {MainInteractorStartMotion %W %x %y}
    bind $widget <Alt-ButtonRelease-1>   {MainInteractorEndMotion %W %x %y}
    bind $widget <Alt-B1-Motion>         {MainInteractorB2Motion %W %x %y}

    # Keyboard
    bind $widget <KeyPress-r>        {MainInteractorReset %W %x %y}
    bind $widget <Up>                {MainInteractorKeyPress Up    %W %x %y}
    bind $widget <Down>              {MainInteractorKeyPress Down  %W %x %y}
    bind $widget <Left>              {MainInteractorKeyPress Left  %W %x %y}
    bind $widget <Shift-Left>        {MainInteractorKeyPress Left  %W %x %y; MainInteractorShiftMotion %W %x %y}
    bind $widget <Right>             {MainInteractorKeyPress Right %W %x %y}
    bind $widget <Shift-Right>       {MainInteractorKeyPress Right %W %x %y; MainInteractorShiftMotion %W %x %y}
    bind $widget <Delete>            {MainInteractorKeyPress Delete %W %x %y}
    bind $widget <KeyPress-d>        {MainInteractorKeyPress d %W %x %y}
    bind $widget <KeyPress-c>        {MainInteractorKeyPress c %W %x %y}
    bind $widget <KeyPress-=>        {MainInteractorKeyPress = %W %x %y}
    bind $widget <KeyPress-->        {MainInteractorKeyPress - %W %x %y}
    bind $widget <Control-a>         {MainInteractorKeyPress Ctla %W %x %y}
    bind $widget <Control-x>         {MainInteractorKeyPress Ctlx %W %x %y}
    bind $widget <Control-c>         {MainInteractorKeyPress Ctlc %W %x %y}
    bind $widget <Control-v>         {MainInteractorKeyPress Ctlv %W %x %y}
    bind $widget <Control-d>         {MainInteractorKeyPress Ctld %W %x %y}
    # toggle between fore and background volumes
    bind $widget <KeyPress-g>        {MainInteractorKeyPress g %W %x %y}

    # bind all the digits on the top row and the key pad
    for {set i 0} {$i < 10} {incr i} {
        bind $widget <KeyPress-$i>        [list MainInteractorKeyPress $i %W %x %y]
        bind $widget <KeyPress-KP_$i>     [list MainInteractorKeyPress $i %W %x %y]
    }

    # Added for Fiducials
    if {[IsModule Fiducials] == 1 || [IsModule Alignments] == 1} {
    bind $widget <KeyPress-p> {
         if { [SelectPick2D %W %x %y] != 0 } \
             { set ::Fiducial(Pick2D) 1; eval FiducialsCreatePointFromWorldXYZ "default" $Select(xyz) ; set ::Fiducial(Pick2D) 0; MainUpdateMRML; Render3D}
     }
    }

     # surreal: added for Navigator
     # have to use SelectPick2D for slices
     if {[IsModule Navigator] == 1} {
       bind $widget <KeyPress-n> {
           if {[SelectPick2D %W %x %y] != 0} {
               set x [lindex $Select(xyz) 0]
               set y [lindex $Select(xyz) 1]
               set z [lindex $Select(xyz) 2]
               puts "slice"
               eval NavigatorPickSlicePoint %W $x $y $z
           }
       }
     }

}

#-------------------------------------------------------------------------------
# .PROC MainInteractorXY
#
# returns 'xs ys x y'
# <br>
# (xs, ys) is the point relative to the lower, left corner 
# of the slice window (0-255 or 0-511).
# <br>
# (x, y) is the point with Zoom and Double taken into consideration
# (zoom=2 means range is 64-128 instead of 1-256)
# .ARGS
# int s the slice window number
# int x location in x that was selected
# int y location in y that was selected
# .END
#-------------------------------------------------------------------------------
proc MainInteractorXY {s x y} {
    global Interactor

    # Compute screen coordinates
    set y [expr $Interactor(ySize) - 1 - $y]
    set xs $x
    set ys $y

    # Convert Screen coordinates to Reformatted image coordinates
    $Interactor(activeSlicer) SetScreenPoint $s $x $y
    scan [$Interactor(activeSlicer) GetReformatPoint] "%d %d" x y

    return "$xs $ys $x $y"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorCursor
# 
# .ARGS
# int s slice window number
# int xs
# int ys
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorCursor {s xs ys x y} {
    global Slice Anno View Interactor

    # pixel value
    set forePix [$Interactor(activeSlicer) GetForePixel $s $x $y]
    set backPix [$Interactor(activeSlicer) GetBackPixel $s $x $y]

    # pixel display format from scalar type
    set id VolID
    set f PixelDispFormat
    foreach m "back fore" {
        set v $Slice($s,$m$id)
        if {$v} {
            set scalarType [Volume($v,node) GetScalarType]
            # VTK_FLOAT = 10; VTK_DOUBLE = 11
            set b [expr {$scalarType == 10 || $scalarType == 11}]
            #set Anno($m$f) [expr {$b == 1 ? "%.1f" : $Anno(pixelDispFormat)}] 
            if { ($b == 1) && ($Anno(pixelDispFormat) == "%.f") }  {
                set Anno($m$f) "%6.2f"
            } else {
                set Anno($m$f) $Anno(pixelDispFormat)
            }
        }
    }

    # Get RAS and IJK coordinates
    $Interactor(activeSlicer) SetReformatPoint $s $x $y
    scan [$Interactor(activeSlicer) GetWldPoint] "%g %g %g" xRas yRas zRas 
    scan [$Interactor(activeSlicer) GetIjkPoint] "%g %g %g" xIjk yIjk zIjk

    # Write Annotation
    if {$Anno(mouse) == 1} {
        foreach name "$Anno(mouseList)" {
            if {$name != "msg"} {
                # Warning: actor may not exist yet, so check!
                if {[info command Anno($s,$name,actor)] != ""} {
                    Anno($s,$name,actor) SetVisibility 1
                }
            }
        }
        if {[info command Anno($s,cur1,mapper)] != ""} {
            switch $Anno(cursorMode) {
                "RAS" {
                    Anno($s,cur1,mapper) SetInput [format "R %.f" $xRas]
                    Anno($s,cur2,mapper) SetInput [format "A %.f" $yRas]
                    Anno($s,cur3,mapper) SetInput [format "S %.f" $zRas]
                }
                "IJK" {
                    Anno($s,cur1,mapper) SetInput [format "I %.f" $xIjk]
                    Anno($s,cur2,mapper) SetInput [format "J %.f" $yIjk]
                    Anno($s,cur3,mapper) SetInput [format "K %.f" $zIjk]
                }
                "XY" {
                    Anno($s,cur1,mapper) SetInput [format "X %.f" $x]
                    Anno($s,cur2,mapper) SetInput [format "Y %.f" $y]
                    Anno($s,cur3,mapper) SetInput " "
                }
            }
        }
        if {[info command Anno($s,curBack,mapper)] != ""} {
            Anno($s,curBack,mapper) SetInput \
                [format "Bg $Anno(backPixelDispFormat)" $backPix]
            Anno($s,curFore,mapper) SetInput \
                [format "Fg $Anno(forePixelDispFormat)" $forePix]
        }

        set backnode [[$Interactor(activeSlicer) GetBackVolume $s] GetMrmlNode]
        set ::Anno(curBack,label) ""
        if { [$backnode GetLUTName] == -1 } {
            set curtext [Anno($s,curBack,mapper) GetInput] 
            set labelid [MainColorsGetColorFromLabel $backPix]
            if { $labelid != "" } {
                set label [Color($labelid,node) GetName]
            } else {
                set label unknown
            }
            Anno($s,curBack,mapper) SetInput "$curtext : $label"
            set ::Anno(curBack,label) $label
        } else {
            set ::Anno(curBack,label) ""
        }
        set forenode [[$Interactor(activeSlicer) GetForeVolume $s] GetMrmlNode]
        set ::Anno(curFore,label) ""
        if { [$forenode GetLUTName] == -1 } {
            set curtext [Anno($s,curFore,mapper) GetInput] 
            set labelid [MainColorsGetColorFromLabel $forePix]
            if { $labelid != "" } {
                set label [Color($labelid,node) GetName]
            } else {
                set label unknown
            }
            Anno($s,curFore,mapper) SetInput "$curtext : $label"
            set ::Anno(curFore,label) $label
        } else {
            set ::Anno(curFore,label) ""
        }
    } else {
        foreach name "$Anno(mouseList)" {
            if {[info command Anno($s,$name,actor)] != ""} {
                Anno($s,$name,actor) SetVisibility 0
            }
        }
    }

    # Move cursor
    $Interactor(activeSlicer) SetCursorPosition $s $xs $ys
    
    # Show close-up image
    if {$View(createMagWin) == "Yes" && $View(closeupVisibility) == "On"
            && [info command View(mag)] != "" } {
        View(mag) SetX $x
        View(mag) SetY $y
    }
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorKeyPress
#
# Called when a key is pressed in the 2D window.
# Deals with Up, Down, Left and Right.
#
# Up and Down moves the slice offset.
# Left and Right calles EditApplyFilter from Edit.tcl
# .ARGS
# string key  one of Right, Left, Up, Down, Delete, d, c, 0, g, Ctla, Ctlx, Ctlc, Ctlv, Ctld
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorKeyPress {key widget x y} {
    global View Slice Interactor Module Editor

    focus $widget

    # Determine which slice this is
    set s $Interactor(s)
    if {$s == ""} {return}

    MainInteractorMotion $widget $x $y
    
    switch -glob -- $key {
        "Right" {
            MainSlicesSetOffset $s Next;
            MainSlicesRefreshClip $s

            scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
            MainInteractorCursor $s $xs $ys $x $y
            MainInteractorRender
        }
        "Left" {
            MainSlicesSetOffset $s Prev;
            MainSlicesRefreshClip $s

            scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
            MainInteractorCursor $s $xs $ys $x $y
            MainInteractorRender
        }
        "Up" {
            switch $Module(activeID) {
                "Editor" {
                    $Editor(activeID)Apply
                }
            }
        }
        "Down" {
            switch $Module(activeID) {
                "Editor" {
                    switch $Editor(activeID) {
                        "EdDraw" {
                            EdDrawUpdate NextMode
                        }
                    }
                }
            }
        }
        "Delete" {
            switch $Module(activeID) {
                "Editor" {
                    switch $Editor(activeID) {
                        "EdDraw" {
                            EdDrawUpdate Delete
                        }
                    }
                }
            }
        }
        "d" {
            switch $Module(activeID) {
                "Editor" {
                    EditorSetEffect EdDraw
                }
            }
        }
        "c" {
            switch $Module(activeID) {
               "Editor" {
                   EditorSetEffect EdChangeIsland
               }        
            }
         }
         "=" - 
         "-" - 
         "\[0-9\]" {
            switch $Module(activeID) {
                "Editor" {
                    switch $Editor(activeID) {
                        "EdDraw" {
                            EdDrawUpdate $key
                        }
                    }
                }
            }
        }

        "g" {
            # call the toggle between fore and background volumes
            set toggleCmd [.tMain.fDisplay.fRight.bToggle cget -command]
            eval $toggleCmd
        }
        "Ctla" {
            switch $Module(activeID) {
                "Editor" {
                    switch $Editor(activeID) {
                        "EdDraw" {
                            EdDrawUpdate SelectAll
                            RenderActive
                        }  
                    }  
                }  
            }  
        }  
        "Ctlx" {  
            switch $Module(activeID) {  
                "Editor" {  
                    switch $Editor(activeID) {  
                        "EdDraw" {  
                            EdDrawUpdate Cut  
                            RenderActive  
                        }  
                    }  
                }  
            }  
        }  
        "Ctlc" {  
            switch $Module(activeID) {  
                "Editor" {  
                    switch $Editor(activeID) {  
                        "EdDraw" {  
                            EdDrawUpdate Copy  
                            RenderActive  
                        }  
                    }  
                }  
            }  
        }  
        "Ctlv" {  
            switch $Module(activeID) {  
                "Editor" {  
                    switch $Editor(activeID) {  
                        "EdDraw" {  
                            EdDrawUpdate Paste  
                            RenderActive  
                        }  
                    }  
                }  
            }  
        }  
        "Ctld" {  
            switch $Module(activeID) {  
                "Editor" {  
                    switch $Editor(activeID) {  
                        "EdDraw" {  
                            EdDrawUpdate DeleteAll  
                            RenderActive  
                        }  
                    }  
                }  
            }  
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorMotion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorMotion {widget x y} {
    global Interactor Module

    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 


    # added for LiveWire
    switch $Module(activeID) {
        "Editor" {
            EditorMotion $x $y
        }
    }

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y


    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorShiftMotion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorShiftMotion {widget x y} {
    global Interactor Module

    MainInteractorMotion $widget $x $y

    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    $Interactor(activeSlicer) SetReformatPoint $s $x $y
    scan [$Interactor(activeSlicer) GetWldPoint] "%g %g %g" rRas aRas sRas 

    for {set slice 0} {$slice < 3} {incr slice} {
        if {$slice != $s} {
            
            switch -glob [$Interactor(activeSlicer) GetOrientString $slice] {
                "Axial" { MainSlicesSetOffset $slice $sRas}
                "Sagittal" { MainSlicesSetOffset $slice $rRas}
                "Coronal" { MainSlicesSetOffset $slice $aRas}

                # TODO need to get the unscaled pixel coordinates for RAS and
                # know the offset relative to the I, L, P origin of the volume

                *Slice {
                    set vol [Slicer GetBackVolume $s]
                    set node [$vol GetMrmlNode]
                    set scanorder [$node GetScanOrder]
                    set dims [concat [[$vol GetOutput] GetDimensions] 1]
                    catch "IjkToRas Delete"
                    vtkMatrix4x4 IjkToRas 
                    eval IjkToRas DeepCopy [$node GetRasToIjkMatrix]
                    IjkToRas Invert
                    set ras_origin [IjkToRas MultiplyPoint 0 0 0 1]
                    set ras_bound [eval IjkToRas MultiplyPoint $dims]
                    set ras_extent [list \
                        [expr abs([lindex $ras_bound 0] - [lindex $ras_origin 0])] \
                        [expr abs([lindex $ras_bound 1] - [lindex $ras_origin 1])] \
                        [expr abs([lindex $ras_bound 2] - [lindex $ras_origin 2])] ]
                    IjkToRas Delete
                    set lpi_point [list \
                        [expr min([lindex $ras_bound 0],[lindex $ras_origin 0])] \
                        [expr min([lindex $ras_bound 1],[lindex $ras_origin 1])] \
                        [expr min([lindex $ras_bound 2],[lindex $ras_origin 2])] ]



                    switch -glob [$Interactor(activeSlicer) GetOrientString $slice] {
                        "AxiSlice" { 
                            switch [$node GetScanOrder] {
                                "RL" - "LR" {
                                    set dim [lindex $dims 1]
                                }
                                "PA" - "AP" {
                                    set dim [lindex $dims 1]
                                }
                                "IS" - "SI" {
                                    set dim [lindex $dims 2]
                                }
                            }
                            set off [expr $dim * ($sRas - [lindex $lpi_point 2]) / [lindex $ras_extent 2]]
                            MainSlicesSetOffset $slice $off
                        }
                        "SagSlice" { 
                            switch [$node GetScanOrder] {
                                "RL" - "LR" {
                                    set dim [lindex $dims 2]
                                }
                                "PA" - "AP" {
                                    set dim [lindex $dims 0]
                                }
                                "IS" - "SI" {
                                    set dim [lindex $dims 1]
                                }
                            }
                            set off [expr $dim * ($rRas - [lindex $lpi_point 0]) / [lindex $ras_extent 0]]
                            MainSlicesSetOffset $slice $off
                        }
                        "CorSlice" {
                            switch [$node GetScanOrder] {
                                "RL" - "LR" {
                                    set dim [lindex $dims 0]
                                }
                                "PA" - "AP" {
                                    set dim [lindex $dims 2]
                                }
                                "IS" - "SI" {
                                    set dim [lindex $dims 1]
                                }
                            }
                            set off [expr $dim * ($aRas - [lindex $lpi_point 1]) / [lindex $ras_extent 1]]
                            MainSlicesSetOffset $slice $off
                        }
                    }
                }
            }
        }
    }
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1 {widget x y} {
    global Interactor Module

    focus $widget
    
    set s $Interactor(s)
    scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y 

    # Here is a switch statement for the current active module,
    # but it really should be Peter Everett's stack for bindings.
    switch $Module(activeID) {
        "Editor" {
            EditorB1 $x $y
        }
        "Alignments" {
            AlignmentsB1 $x $y
        }
    }

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorShiftB1
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorShiftB1 {widget x y} {
    global Interactor
    
    set s $Interactor(s)
    scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y 

    MainInteractorCursor $s $xs $ys $x $y
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1Release
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1Release {widget x y} {
    global Interactor Module
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y
    
    switch $Module(activeID) {
    "Editor" {
        EditorB1Release $x $y
    }
    "Alignments" {
        AlignmentsB1Release $x $y
    }
    }

    Anno($s,msg,actor)  SetVisibility 0
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorShiftB1Release
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorShiftB1Release {widget x y} {
    global Interactor Module
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    Anno($s,msg,actor)  SetVisibility 0
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1Motion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1Motion {widget x y} {
    global Interactor Module
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    switch $Module(activeID) {
        "Editor" {
            EditorB1Motion $x $y
        }
        "Alignments" {
            AlignmentsB1Motion $x $y
        }
    }

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorControlB1Motion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorControlB1Motion {widget x y} {
    global Interactor Module
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # W/L using screen coordinates so that the same number
    # of screen pixels covered (not % of image) produces the
    # same zoom factor. To put it another way, I want the 
    # user to have to drag the mouse a consistent distance 
    # across the mouse pad.
    #
    # BUG: I'm commenting this out because window/leveling for too long
    # causes a stack overflow.
#    MainInteractorWindowLevel $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)
   
     switch $Module(activeID) {  
         "Editor" {  
             EditorControlB1Motion $x $y  
         }  
     }

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorAltB1Motion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorAltB1Motion {widget x y} {
    global Interactor
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # Threshold using screen coordinates so that the same number
    # of screen pixels covered (not % of image) produces the
    # same zoom factor. To put it another way, I want the 
    # user to have to drag the mouse a consistent distance 
    # across the mouse pad.
    #
    # BUG: I'm commenting this out because window/leveling for too long
    # causes a stack overflow.
#    MainInteractorThreshold $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

    # NOTE: since this has been disabled for years, AltB1 is now
    # mapped to PAN to support two-button mice. - sp 2002-11-14

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB2Motion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB2Motion {widget x y} {
    global Interactor
    
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # I want the image feature under the mouse to follow
    # the mouse around the screen during panning.
    # 
    MainInteractorPan $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB3Motion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB3Motion {widget x y} {
    global Interactor Module 

    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
  
    #This should be replaced with Mike Halle's binding mechanism
    #switch $Module(activeID) {  
    #    "Alignments" {
    #        AlignmentsB3Motion $widget $x $y
    #        return
    #    }
    #}

    # Zoom using screen coordinates so that the same number
    # of screen pixels covered (not % of image) produces the
    # same zoom factor. To put it another way, I want the 
    # user to have to drag the mouse a consistent distance 
    # across the mouse pad.
    #
    MainInteractorZoom $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    # Render this slice
    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorPan
# Pan the 2D slices.
# Bouix 4/23/03 reversed to the old method to solve the bug when drawing
# .ARGS
# int s slice number
# int x current x location
# int y current y location
# int xLast last x location
# int yLast last y location
# .END
#-------------------------------------------------------------------------------
proc MainInteractorPan {s x y xLast yLast} {
     global View

     set dx [expr $xLast - $x]
     set dy [expr $yLast - $y]
     Slicer GetZoomCenter
     scan [Slicer GetZoomCenter$s] "%g %g" cx cy

     set z [Slicer GetZoom $s]
     if {$View(mode) == "Quad512"} {
         set z [expr $z * 2.0]
     }

     if {[Slicer GetZoomAutoCenter $s] == 1} {
         Slicer SetZoomAutoCenter $s 0
         Slicer Update
     }
     set cx [expr $cx + $dx / $z]
     set cy [expr $cy + $dy / $z]    
     Slicer SetZoomCenter $s $cx $cy
 }

# New version by Attila Tanacs 11/07/01
# proc MainInteractorPan {s x y xLast yLast} {
#     global View Interactor

#     set dx [expr $xLast - $x]
#     set dy [expr $yLast - $y]
#     set os [[$Interactor(activeSlicer) GetBackReformat $s] GetOriginShift]
#     scan $os "%g %g" cx cy
    
#     set z [[$Interactor(activeSlicer) GetBackReformat $s] GetZoom]
#     set ps [[$Interactor(activeSlicer) GetBackReformat $s] GetPanScale]

# #    if {[Slicer GetZoomAutoCenter $s] == 1} {
# #        Slicer SetZoomAutoCenter $s 0
# #        Slicer Update
# #    }

#     set cx [expr $cx + $dx * $ps]
#     set cy [expr $cy + $dy * $ps]

#     $Interactor(activeSlicer) SetOriginShift $s $cx $cy
#     $Interactor(activeSlicer) Update
#     RenderAll
# }

#-------------------------------------------------------------------------------
# .PROC MainInteractorZoom
# Bouix 4/23/03 Changed back to old zoom to account for the drawing problem
# .ARGS
# int s slice window id
# int x
# int y
# int xLast
# int yLast
# .END
#-------------------------------------------------------------------------------
proc MainInteractorZoom {s x y xLast yLast} {

     set dy [expr $yLast - $y]

     # log base b of x = log(x) / log(b)
     set b      1.02
     set zPrev  [Slicer GetZoom $s]
     set dyPrev [expr log($zPrev) / log($b)]

     set zoom [expr pow($b, ($dy + $dyPrev))]
     if {$zoom < 0.01} {
         set zoom 0.01
     }
     set z [format "%.2f" $zoom]

     Anno($s,msg,mapper)  SetInput "ZOOM: x $z"

     MainSlicesSetZoom $s $z
}

# New version by Attila Tanacs 11/07/01
# proc MainInteractorZoom {s x y xLast yLast} {
#     global View Interactor
    
#     set dy [expr $yLast - $y]
    
#     # log base b of x = log(x) / log(b)
#     set b      1.02
#     set zPrev [[$Interactor(activeSlicer) GetBackReformat $s] GetZoom]
#     set dyPrev [expr log($zPrev) / log($b)]

#     set zoom [expr pow($b, ($dy + $dyPrev))]
# #    set zoom [expr pow($b, ($dy))]
#     if {$zoom < 0.01} {
#     set zoom 0.01
#     }
#     set z [format "%.2f" $zoom]

#     Anno($s,msg,mapper)  SetInput "ZOOM: x $z"

#     # Use this instead of directly accessing the slicer
#     # object.  MainSlices.tcl uses Attila's "newzoom" method
#     # now too.
#     MainSlicesSetZoom $s $z

#     #Slicer SetZoomNew $s $zoom
#     #Slicer Update
#     #RenderAll
# }
# << Bouix 

#-------------------------------------------------------------------------------
# .PROC MainInteractorWindowLevel
# 
# .ARGS
# int s slice window number
# int x current x location
# int y current y location
# int xLast last x location
# int yLast last y location
# .END
#-------------------------------------------------------------------------------
proc MainInteractorWindowLevel {s x y xLast yLast} {
    global View Volume Interactor

    set dx [expr $xLast - $x]
    set dy [expr $yLast - $y]

    set v [[[$Interactor(activeSlicer) GetBackVolume $s] GetMrmlNode] GetID]
    MainVolumesSetActive $v

    set window [Volume($v,node) GetWindow]
    set level  [Volume($v,node) GetLevel]

    set window [expr int($window - $dx)] 
    set level  [expr int($level  - $dy)]

    MainVolumesSetParam Window $window
    MainVolumesSetParam Level  $level

    set window [Volume($v,node) GetWindow]
    set level  [Volume($v,node) GetLevel]
    Anno($s,msg,mapper)  SetInput "Window: $window, Level: $level"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorThreshold
# 
# .ARGS
# int s slice window number
# int x current x location
# int y current y location
# int xLast last x location
# int yLast last y location
# .END
#-------------------------------------------------------------------------------
proc MainInteractorThreshold {s x y xLast yLast} {
    global View Volume Interactor

    set dx [expr $xLast - $x]
    set dy [expr $yLast - $y]

    set v [[[$Interactor(activeSlicer) GetBackVolume $s] GetMrmlNode] GetID]
    MainVolumesSetActive $v

    set lower [Volume($v,node) GetLowerThreshold]
    set upper [Volume($v,node) GetUpperThreshold]

    set lower [expr int($lower - $dx)] 
    set upper [expr int($upper - $dy)]

    MainVolumesSetParam LowerThreshold $lower
    MainVolumesSetParam UpperThreshold $upper

    set lower [Volume($v,node) GetLowerThreshold]
    set upper [Volume($v,node) GetUpperThreshold]
    Anno($s,msg,mapper)  SetInput "Threshold: $lower...$upper"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorExpose
# a litle more complex than just "bind $widget <Expose> {%W Render}"
# we have to handle all pending expose events otherwise they que up.
# .ARGS
# windowpath widget
# .END
#-------------------------------------------------------------------------------
proc MainInteractorExpose {widget} {

   # Do not render if we are already rendering
   if {[::vtk::get_widget_variable_value $widget Rendering] == 1} {
      return
   }

   # empty the que of any other expose events
   ::vtk::set_widget_variable_value $widget Rendering 1
   update
   ::vtk::set_widget_variable_value $widget Rendering 0

   # ignore the region to redraw for now.
   $widget Render
}
 
#-------------------------------------------------------------------------------
# .PROC MainInteractorRender
# Render the Interactor slice in 2d and in 3d if it's visible.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainInteractorRender {} {
    global Interactor View Slice

    set s $Interactor(s)

    RenderSlice $s

    if { $Slice($s,visibility) } {
        Render3D
    }
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorEnter
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorEnter {widget x y} {
    global Interactor Gui

    # Determine what slice this is
    set s [string index $widget [expr [string length $widget] - 4]]
    set Interactor(s) $s

    # Display this slice in the mag window
    MainViewSetWelcome sl$s

    # Focus
    # - do click focus on PC, but focus-follows-mouse on unix
    if { !$Gui(pc) } {
        set Interactor(oldFocus) [focus]
        focus $widget
    }

    # Get the renderer window dimensions
    set Interactor(xSize) [lindex [$widget configure -width] 4]
    set Interactor(ySize) [lindex [$widget configure -height] 4]

    set Interactor(xCenter) [expr double($Interactor(xSize))/2.0]
    set Interactor(yCenter) [expr double($Interactor(ySize))/2.0]
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorExit
# 
# .ARGS
# windowpath widget
# .END
#-------------------------------------------------------------------------------
proc MainInteractorExit {widget} {
    global Interactor Anno Gui

    set s $Interactor(s)
    
    # Center cursor
    $Interactor(activeSlicer) SetCursorPosition $s \
        [expr int($Interactor(xCenter))] [expr int($Interactor(yCenter))]

    # Turn off cursor anno
    foreach name "$Anno(mouseList)" {
        if {[info command Anno($s,$name,actor)] != ""} {
            Anno($s,$name,actor) SetVisibility 0
        }
    }

    # Render
    MainInteractorRender

    # Return the focus
    if { !$Gui(pc) } {
        focus $Interactor(oldFocus)
    }
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorStartMotion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorStartMotion {widget x y} {
    global Interactor

    set s $Interactor(s)
    MainSlicesSetActive $s

    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

    Anno($s,msg,mapper)  SetInput ""
    Anno($s,msg,actor)   SetVisibility 1

    return "$xs $ys $x $y"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorEndMotion
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorEndMotion {widget x y} {
    global Interactor

    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

    # Cursor
    MainInteractorCursor $s $xs $ys $x $y

    Anno($s,msg,actor)  SetVisibility 0

    MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorReset
# 
# .ARGS
# windowpath widget
# int x
# int y
# .END
#-------------------------------------------------------------------------------
proc MainInteractorReset {widget x y} {
    global Interactor

    MainSlicesResetZoomAll
    MainInteractorRender
    RenderSlices
}

#-------------------------------------------------------------------------------
# .PROC PixelsToMm
# mm = pix * fov/dim / mag
# pix = mm * dim/fox * mag 
# .ARGS
# int pix
# float fov
# float dim
# float mag
# .END
#-------------------------------------------------------------------------------
proc PixelsToMm {pix fov dim mag} {


    return [expr int($pix * $fov/$dim / $mag + 0.5)]
}

#-------------------------------------------------------------------------------
# .PROC Distance3D
# Returns distance between two points
# .ARGS
# float x1 x of first point  
# float y1 y of first point
# float z1 z of first point
# float x2 x of second point  
# float y2 y of second point
# float z2 z of second point
# .END
#-------------------------------------------------------------------------------
proc Distance3D {x1 y1 z1 x2 y2 z2} {
    set dx [expr $x2 - $x1]
    set dy [expr $y2 - $y1]
    set dz [expr $z2 - $z1]
    return [expr sqrt($dx*$dx + $dy*$dy + $dz*$dz)]
}

#-------------------------------------------------------------------------------
# .PROC Angle2D
# Returns the angle in degrees between two vectors defined by two points each.
# .ARGS
# float ax1 first point on first vector
# float ay1 first point on first vector
# float ax2 second point on first vector
# float ay2 second point on first vector
# float bx1 first point on second vector
# float by1 first point on second vector
# float bx2 second point on second vector
# float by2 second point on second vector
# .END
#-------------------------------------------------------------------------------
proc Angle2D {ax1 ay1 ax2 ay2 bx1 by1 bx2 by2} {

    # Form vector 'a'=[ax ay] with magnitude 'am'   
    set ax [expr $ax2 - $ax1]
    set ay [expr $ay2 - $ay1]
    set am [expr sqrt($ax*$ax + $ay*$ay)]

    # Form vector 'b'=[bx by] with magnitude 'bm' 
    set bx [expr $bx2 - $bx1]
    set by [expr $by2 - $by1]
    set bm [expr sqrt($bx*$bx + $by*$by)]

    # Find angle between a, b from their dot product
    set dot [expr $ax*$bx + $ay*$by]
    if {$am == 0 || $bm == 0} {return 0}
    set cos [expr $dot/($am*$bm)]
    if { $cos > 1. } { set cos 1 } ;# fix possible rounding error
    set deg [expr acos($cos)]

    # See if angle is negative from the cross product of a and b
    if {$ax*$by - $bx*$ay > 0} {
        set deg [expr -$deg] 
    }

    return [expr $deg*180/3.1415962]
}

#-------------------------------------------------------------------------------  
# .PROC MainInteractorControlB1  
#  
# .ARGS  
# windowpath widget
# int x
# int y
# .END  
#-------------------------------------------------------------------------------  
proc MainInteractorControlB1 {widget x y} {  
    global Interactor Module  

    set s $Interactor(s)  
    scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y  

    switch $Module(activeID) {  
        "Editor" {  
            EditorControlB1 $x $y  
        }  
    }  

    MainInteractorCursor $s $xs $ys $x $y  
    MainInteractorRender  
}  

#-------------------------------------------------------------------------------  
# .PROC MainInteractorControlB1Release  
#  
# .ARGS
# windowpath widget
# int x
# int y 
# .END  
#-------------------------------------------------------------------------------  
proc MainInteractorControlB1Release {widget x y} {  
    global Interactor Module  

    set s $Interactor(s)  
    scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y  

    switch $Module(activeID) {  
        "Editor" {  
            EditorControlB1Release $x $y  
        }  
    }  

    MainInteractorCursor $s $xs $ys $x $y  
    MainInteractorRender  
}

