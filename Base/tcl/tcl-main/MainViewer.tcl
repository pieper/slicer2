#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
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
# FILE:        MainViewer.tcl
# PROCEDURES:  
#   MainViewerInit
#   MainViewerBuildGUI
#   MainViewerShowSliceControls
#   MainViewerHideSliceControls
#   MainViewerUserResize
#   MainViewerAnno
#   MainViewerAddViewsSeparation
#   MainViewerSetSecondViewOn
#   MainViewerSetSecondViewOff
#   MainViewerSetMode
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainViewerInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerInit {} {
    global Module Gui Slice View

    # Don't register the BuildGUI routine, because it gets called specifically

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainViewer \
        {$Revision: 1.20 $} {$Date: 2002/03/18 20:54:50 $}]

        # Props
    set Gui(midHeight) 1
        set View(SecondViewOn) 0
}

#-------------------------------------------------------------------------------
# .PROC MainViewerBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerBuildGUI {} {
    global Gui View viewWin Slice Module

    #-------------------------------------------
    # Viewer window
    #-------------------------------------------
    toplevel     .tViewer -visual {truecolor 24} -bg $Gui(backdrop)

    wm resizable .tViewer 0 1
    # never allow resize for Ron's laptop
    if {$Gui(pc) == "1"} {
    wm resizable .tViewer 0 0
    }
    wm title     .tViewer "Viewer"
    # sum heights of 3d window, slice, and middle button frame
    set h [expr $View(viewerHeightNormal) + 256 + $Gui(midHeight)]
    wm geometry  .tViewer +$Gui(xViewer)+0
    set Gui(fViewer) .tViewer

    #-------------------------------------------
    # Viewer frame
    #-------------------------------------------
    set f $Gui(fViewer)
    frame $f.fTop -bg $Gui(backdrop)
    frame $f.fBot -bg $Gui(backdrop)
    frame $f.fMid -bg $Gui(backdrop)
    set Gui(fTop) $f.fTop
    set Gui(fBot) $f.fBot
    set Gui(fMid) $f.fMid
    
    pack $Gui(fTop) -side top
    pack $Gui(fMid) -side top -expand 1 -fill x
    pack $Gui(fBot) -side top

    set Gui(fViewWin) $Gui(fViewer).fViewWin

    vtkTkRenderWidget $Gui(fViewWin) -width $View(viewerHeight) -height $View(viewerHeightNormal)
    BindTkRenderWidget $Gui(fViewWin)



    #-------------------------------------------
    # Mid->Slice$s frames
    #-------------------------------------------
    set f $Gui(fMid)
    
    foreach s $Slice(idList) {

        frame $f.fSlice$s -bg $Gui(activeWorkspace)
        pack $f.fSlice$s -side left -expand 1 -fill both

        MainSlicesBuildControls $s $f.fSlice$s
    }

    #-------------------------------------------
    # Slice Frame Hierarchy:
    #-------------------------------------------
    # Slice0
    #   Thumb
    #   Controls
    #   Image
    # Slice1
    #   Thumb
    #   Controls
    #   Image
    # Slice2
    #   Thumb
    #   Controls
    #   Image
    #-------------------------------------------

    #-------------------------------------------
    # Slice$s frames
    #-------------------------------------------
    foreach s $Slice(idList) {

        set Gui(fSlice$s) $Gui(fViewer).fSlice$s
        frame $Gui(fSlice$s)
        set f $Gui(fSlice$s)
        pack $f -in $Gui(fBot) -side left

        frame $f.fThumb    -bg $Gui(activeWorkspace)
        frame $f.fControls -bg $Gui(activeWorkspace)
        frame $f.fImage    -bg $Gui(activeWorkspace)
        pack $f.fImage -side top -expand 1 -fill both

        # Raise this window to the front when the mouse passes over it.
        place $f.fControls -in $f -relx 1.0 -rely 0.0 -anchor ne 
        bind $f.fControls <Leave> "MainViewerHideSliceControls"

        # Raise this window to the front when view mode is Quad256 or Quad512
        place $f.fThumb -in $f -relx 1.0 -rely 0.0 -anchor ne

        #-------------------------------------------
        # Slice$s->Thumb frame
        #-------------------------------------------
        set f $Gui(fSlice$s).fThumb

        frame $f.fOrient -bg $Gui(slice$s)
        pack $f.fOrient -side top

        # Orientation
        #-------------------------------------------
        set f $Gui(fSlice$s).fThumb.fOrient

        eval {label $f.lOrient -text "INIT" -width 12} \
            $Gui(WLA) {-bg $Gui(slice$s)}
        pack $f.lOrient
        set Slice($s,lOrient) $f.lOrient

        # Show the full controls when the mouse enters the thumbnail
        bind $f.lOrient <Enter>  "MainViewerShowSliceControls $s"

        #-------------------------------------------
        # Slice$s->Controls frame
        #-------------------------------------------
        set f $Gui(fSlice$s).fControls

        MainSlicesBuildControls $s $f

        #-------------------------------------------
        # Slice$s->Image frame
        #-------------------------------------------
        set f $Gui(fSlice$s).fImage

        # The mag (closeup) window's input is [Slicer GetOuput $s].
        # The slice window's inputs are [Slicer GetCursor $s].
        #
        # The Slicer changes the pipeline based on zoom, which slice
        # is active for polygon drawing, and whether the output is
        # being doubled.
        #
        # These changes do not affect the pointer returned from GetCursor.
        # They do affect GetOutput.
        #
        # sl0          = VTK Window (To render, do: "sl0 Render")
        # gui(fSl0Win) = TK frame
        # 
        set frm Sl$s
        set win sl$s
        MakeVTKImageWindow $win
        ${win}Mapper SetInput [Slicer GetCursor $s]
        set Gui(f${frm}Win) $f.f${frm}Win
        vtkTkImageWindowWidget $Gui(f${frm}Win) -iw ${win}Win -width 256 -height 256
        pack $Gui(f${frm}Win) -side left -fill both -expand 1

        MainInteractorBind $Gui(f${frm}Win)
    }

    # Pack
    pack $Gui(fViewWin) -in $Gui(fTop) -side left -fill both -expand t

    # Get a handle to the Render Window
    set viewWin [$Gui(fViewWin) GetRenderWindow]
    $viewWin AddRenderer viewRen    

    # Without this update, not all slice windows render
    $viewWin Render
    update
    set Gui(midHeight) [winfo height $Gui(fMid)]

    #---------------------------------------------
        # VIEWPORT SEPARATOR
        #---------------------------------------------

        vtkLineSource Gui(viewport,source)
        Gui(viewport,source) SetPoint1 [expr $View(viewerWidth) / 2] $View(viewerHeight) 0
        Gui(viewport,source) SetPoint2 [expr $View(viewerWidth) / 2] 0 0
        vtkPolyDataMapper2D Gui(viewport,mapper)
        Gui(viewport,mapper) SetInput [Gui(viewport,source) GetOutput]
        vtkActor2D Gui(viewport,actor)
        Gui(viewport,actor) SetMapper Gui(viewport,mapper)
        Gui(viewport,actor) SetLayerNumber 1
        eval [Gui(viewport,actor) GetProperty] SetColor "1 1 0.5"
        Gui(viewport,actor) SetVisibility 1
        viewRen AddActor2D Gui(viewport,actor)
}

#-------------------------------------------------------------------------------
# .PROC MainViewerShowSliceControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerShowSliceControls {s} {
    global Gui Slice Volume View

    raise $Gui(fSlice$s).fControls
}

#-------------------------------------------------------------------------------
# .PROC MainViewerHideSliceControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerHideSliceControls {} {
    global Gui Slice Volume View

    lower $Gui(fSlice0).fControls $Gui(fSlice0).fImage
    lower $Gui(fSlice1).fControls $Gui(fSlice1).fImage
    lower $Gui(fSlice2).fControls $Gui(fSlice2).fImage
}

#-------------------------------------------------------------------------------
# .PROC MainViewerUserResize
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerUserResize {} {
    global View Gui

    regexp {([^x]*)x([^\+]*)} [wm geometry .tViewer] match w h

        if {$View(mode) == "3D"} {
        # Find the smallest dimension
        set d $w
        if {$h < $w} {
            set d $h
        }
        if {$d != $View(viewerWidth) || $d != $View(viewerHeight)} {
            set View(viewerWidth)  $w
            set View(viewerHeight) $h
            $Gui(fViewWin) config \
                -width $View(viewerWidth) -height $View(viewerHeight)
            MainViewerAddViewsSeparation $View(viewerWidth) $View(viewerHeight)
        }
    } elseif {$View(mode) == "Normal"} {
        set View(viewerHeightNormal) [expr $h - 256 - $Gui(midHeight)]
        $Gui(fViewWin) config -width $w -height $View(viewerHeightNormal)
    }
}

#-------------------------------------------------------------------------------
# .PROC MainViewerAnno
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerAnno {s dim} {
    global Anno Slice
    
    #
    foreach name $Anno(mouseList) y $Anno(y$dim) {
        [Anno($s,$name,actor) GetPositionCoordinate] SetValue 1 $y
    }
    #
    foreach name $Anno(orientList) x $Anno(orient,x$dim) \
        y $Anno(orient,y$dim) {
        [Anno($s,$name,actor) GetPositionCoordinate] SetValue $x $y
    }
}


#-------------------------------------------------------------------------------
# .PROC MainViewerAddViewsSeparation
# 
#  Set the position of the line that separates the viewport of the 3D viewer 
#  in two (so that the view window and the endoscopic window are separated)
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerAddViewsSeparation {p1 p2} {
    global Gui View
    
    if { $View(SecondViewOn) == 1 } {
    Gui(viewport,actor) SetVisibility 1
    Gui(viewport,source) SetPoint1 [expr ($p1 / 2) - 2] $p2 0
    Gui(viewport,source) SetPoint2 [expr ($p1/ 2) - 2] 0 0
    } else {
    # make the separation invisible by putting it on the border of the
    # MainView window
    Gui(viewport,actor) SetVisibility 0
    #Gui(viewport,source) SetPoint1 $p1 $p2 0
    #Gui(viewport,source) SetPoint2 $p1 0 0
    }
}
 

#-------------------------------------------------------------------------------
# .PROC MainViewerSetSecondViewOn
#  Call this method if you created a second renderer (other than the MainView)
#  and you want to have a separator between the 2 screens. 
#  Then call MainViewerAddViewsSeparation
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerSetSecondViewOn {} {
    
    global View
    set View(SecondViewOn) 1
}

#-------------------------------------------------------------------------------
# .PROC MainViewerSetSecondViewOff
# Call this method you want to get rid of the separator between the 2 screens
# Then call MainViewerAddViewsSeparation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerSetSecondViewOff {} {
    
    global View
    set View(SecondViewOn) 0
}

#-------------------------------------------------------------------------------
# .PROC MainViewerSetMode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainViewerSetMode {{mode ""}} {
    global Slice View Gui Anno

    # set View(mode) if called with an argument
    if {$mode != ""} {
    if {$mode == "Normal" || $mode == "Quad256"  || $mode == "Quad512" \
        || $mode == "3D" || $mode == "Single512"} {
        set View(mode) $mode
    } else {
        return
    }   
    }
    
    set f $Gui(fViewer)
    pack forget $f.fBot $f.fTop $f.fMid \
        $f.fSlice0 $f.fSlice1 $f.fSlice2 $f.fViewWin

    # Pack the image windows depending on mode
    switch $View(mode) {
    "Normal" {
        pack $f.fSlice0 $f.fSlice1 $f.fSlice2  -in $Gui(fBot) -side left
        pack $f.fViewWin -in $Gui(fTop) -side left
        pack $Gui(fTop) -side top
        pack $Gui(fMid) -side top -expand 1 -fill x
        pack $Gui(fBot) -side top

        set w [expr $View(viewerHeightNormal) + $Gui(midHeight) + 256]
        wm geometry .tViewer $View(viewerWidth)x$w

        wm resizable .tViewer 0 1
        # never allow resize for Ron's laptop
        if {$Gui(pc) == "1"} {
        wm resizable .tViewer 0 0
        }

        $Gui(fSl0Win)  config -width 256 -height 256
        $Gui(fSl1Win)  config -width 256 -height 256
        $Gui(fSl2Win)  config -width 256 -height 256
        $Gui(fViewWin) config -width 768 -height $View(viewerHeightNormal)

            MainViewerAddViewsSeparation $View(viewerWidth) $View(viewerHeightNormal)

        # Do NOT show the thumbnails on top of the slice images
        foreach s $Slice(idList) {
            raise $Gui(fSlice$s).fImage
            MainViewerAnno $s 256
        }
    }
    "3D" {
        set hReq $View(viewerHeight)
        set wReq $View(viewerWidth)

        pack $Gui(fTop) -side top
        pack $Gui(fViewWin) -in $Gui(fTop) -side left -expand 1 -fill both

        wm geometry .tViewer ${wReq}x$hReq
        wm resizable .tViewer 1 1
        $Gui(fViewWin) config -width $wReq -height $hReq
                   
            # Delphine
            MainViewerAddViewsSeparation $wReq $hReq
    }
    "Quad256" {
        pack $Gui(fTop) $Gui(fBot) -side top
        pack $f.fSlice0 $f.fViewWin -in $Gui(fTop) -side left
        pack $f.fSlice1 $f.fSlice2  -in $Gui(fBot) -side left

        wm geometry .tViewer 512x512
        wm resizable .tViewer 0 0
        $Gui(fViewWin) config -width 256 -height 256
        $Gui(fSl0Win)  config -width 256 -height 256
        $Gui(fSl1Win)  config -width 256 -height 256
        $Gui(fSl2Win)  config -width 256 -height 256

            # Delphine
            MainViewerAddViewsSeparation 256 256
        
            # Show the control thumbnails on top of the slice images
        foreach s $Slice(idList) {
            raise $Gui(fSlice$s).fThumb
            MainViewerAnno $s 256
        }
    }
    "Quad512" {
        pack $Gui(fTop) $Gui(fBot) -side top
        pack $f.fSlice0 $f.fViewWin -in $Gui(fTop) -side left
        pack $f.fSlice1 $f.fSlice2  -in $Gui(fBot) -side left

        wm geometry .tViewer 1024x1024
        wm resizable .tViewer 0 0
        $Gui(fViewWin) config -width 512 -height 512
        $Gui(fSl0Win)  config -width 512 -height 512
        $Gui(fSl1Win)  config -width 512 -height 512
        $Gui(fSl2Win)  config -width 512 -height 512
        
        # Ron's laptop
        if {$Gui(pc) == "1"} {
            # allow user to maximize window which can help
            wm resizable .tViewer 0 1
            # this does not work though it's in the man pgs
            # if the user does a maximize in the upper
            # right corner all things will render okay.
            #wm state .tViewer zoomed
        }    

            # Delphine
            MainViewerAddViewsSeparation 512 512

        # Show the control thumbnails on top of the slice images
        foreach s $Slice(idList) {
            raise $Gui(fSlice$s).fThumb
            MainViewerAnno $s 512
        }
    }
    "Single512" {
        pack $Gui(fTop) $Gui(fBot) -side top -anchor w
        pack $f.fSlice0 $f.fViewWin -in $Gui(fTop) -side left -anchor n
        pack $f.fSlice1 $f.fSlice2  -in $Gui(fBot) -side left -anchor w

        wm geometry .tViewer 768x768
        wm resizable .tViewer 0 0
        $Gui(fViewWin) config -width 256 -height 256
        $Gui(fSl0Win)  config -width 512 -height 512
        $Gui(fSl1Win)  config -width 256 -height 256
        $Gui(fSl2Win)  config -width 256 -height 256

        # Ron's laptop
        if {$Gui(pc) == "1"} {
            # allow user to maximize window which can help
            wm resizable .tViewer 0 1
            # this does not work though it's in the man pgs
            # if the user does a maximize in the upper
            # right corner all things will render okay.
            #wm state .tViewer zoomed
        }    

            # Delphine
            MainViewerAddViewsSeparation 256 256


        # Show the control thumbnails on top of the slice images
        foreach s $Slice(idList) {
            raise $Gui(fSlice$s).fThumb
            MainViewerAnno $s 256
        }
        MainViewerAnno 0 512
    }
    }
    # Double the slice size in 512 mode

    if {$View(mode) == "Normal" || $View(mode) == "Quad256"} {
        foreach s $Slice(idList) {
            Slicer SetDouble $s 0
            Slicer SetCursorPosition $s 128 128
        }
    } elseif {$View(mode) == "Quad512"} {
        foreach s $Slice(idList) {
            Slicer SetDouble $s 1
            Slicer SetCursorPosition $s 256 256
        }
    } elseif {$View(mode) == "Single512"} {
        foreach s $Slice(idList) {
            Slicer SetDouble $s 0
            Slicer SetCursorPosition $s 128 128
        }
        set s 0
        Slicer SetDouble $s 1
        Slicer SetCursorPosition $s 256 256
    }
    
    Slicer Update
}

