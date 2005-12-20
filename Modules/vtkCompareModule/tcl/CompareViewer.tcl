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
# FILE:        MainViewer.tcl       
# PROCEDURES:  
#   CompareViewerInit
#   CompareViewerBuildGUI
#   CompareViewerShowSliceControls
#   CompareViewerHideSliceControls
#   CompareViewerAnno
#   CompareViewerSetMode
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC CompareViewerInit
# Set CompareViewer array to the proper initial values.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareViewerInit {} {
    global CompareViewer

    set CompareViewer(linked) "off"
    set CompareViewer(multiOrMosaik) ""
    set CompareViewer(mode) "2"
}

#-------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------
#
# CompareViewer(linked)            : possible values are "on" and "off".
# Indicates if display is linked.
# CompareViewer(multiOrMosaik)     : possible values are "mosaik" and "multiSlice".
# Indicates if the toplevel window displays the mosaik or the multi-slice
# comparison tool.
# CompareViewer(mode)              : values are "2", "3", "4", "6" and "9".
# In multiSlice mode, indicate how many slices are displayed.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC CompareViewerBuildGUI
# Builds the toplevel container and vtkTkRenderWindows displaying slices (in
# multi-slice mode) and mosaik (in so called mode).
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareViewerBuildGUI {} {
    global Gui View CompareGui CompareViewer CompareSlice CompareMosaik MultiSlicer
    #-------------------------------------------
    # Viewer window
    #-------------------------------------------

    # clean up to allow re-creation
    catch "slCompareMosaikWin Delete"
    foreach s $CompareSlice(idList) {
       catch "slCompare{$s}Win Delete"
    }
    catch "destroy .tCompareViewer"


    toplevel     .tCompareViewer -visual {truecolor 24} -bg $Gui(backdrop)

    wm protocol .tCompareViewer WM_DELETE_WINDOW "wm withdraw .tCompareViewer"
    wm title     .tCompareViewer "Comparator 1000"
    wm withdraw .tCompareViewer
    set h [expr $View(viewerHeightNormal) + 256 + $Gui(midHeight)]

    set CompareGui(tCompareViewer) .tCompareViewer

    #-------------------------------------------
    # Viewer frame
    #-------------------------------------------

    set f $CompareGui(tCompareViewer)

    frame $f.fTop -bg $Gui(backdrop) -width 100 -height 100
    frame $f.fBot -bg $Gui(backdrop)
    frame $f.fMid -bg $Gui(backdrop)

    set CompareGui(fTop) $f.fTop
    set CompareGui(fBot) $f.fBot
    set CompareGui(fMid) $f.fMid

    # FIXME : keep this update. But mandatory?
    update

    #-------------------------------------------
    # Slice$s frames
    #-------------------------------------------
    foreach s $CompareSlice(idList) {
        set CompareGui(fCompareSlice$s) $CompareGui(tCompareViewer).fSlice$s
        frame $CompareGui(fCompareSlice$s)
        set f $CompareGui(fCompareSlice$s)

        frame $f.fThumb    -bg $Gui(activeWorkspace)
        frame $f.fControls -bg $Gui(activeWorkspace)
        frame $f.fImage    -bg $Gui(activeWorkspace)
        pack $f.fImage -side top -expand 1 -fill both

        # Raise this window to the front when the mouse passes over it.
        place $f.fControls -in $f -relx 1.0 -rely 0.0 -anchor ne
        bind $f.fControls <Leave> "CompareViewerHideSliceControls"

        # Raise this window to the front when view mode is Quad256 or Quad512
        place $f.fThumb -in $f -relx 1.0 -rely 0.0 -anchor ne

        #-------------------------------------------
        # Slice$s->Thumb frame
        #-------------------------------------------
        set f $CompareGui(fCompareSlice$s).fThumb

        frame $f.fOrient
        pack $f.fOrient -side top

        set f $CompareGui(fCompareSlice$s).fThumb.fOrient

        eval {label $f.lOrient -text "INIT" -width 12} $Gui(WLA)
        pack $f.lOrient
        set CompareSlice($s,lOrient) $f.lOrient

        # Show the full controls when the mouse enters the thumbnail
        bind $f.lOrient <Enter>  "CompareViewerShowSliceControls $s"

        #-------------------------------------------
        # Slice$s->Controls frame
        #-------------------------------------------
        set f $CompareGui(fCompareSlice$s).fControls
        CompareSlicesBuildControls $s $f

        #-------------------------------------------
        # Slice$s->Image frame
        #-------------------------------------------
        set f $CompareGui(fCompareSlice$s).fImage

        # The slice window's inputs are [MultiSlicer GetCursor $s].
        #
        # The MultiSlicer changes the pipeline based on zoom, and whether
        # the output is being doubled.
        #
        # slCompare0          = VTK Window (To render, do: "sl0 Render")
        # gui(fSlCompare0Win) = TK frame
        #

        set win slCompare$s
        MakeVTKImageWindow $win
        ${win}Mapper SetInput [MultiSlicer GetCursor $s]

        set frm SlCompare$s
        set CompareGui(f${frm}Win) $f.f${frm}Win
        vtkTkRenderWidget $CompareGui(f${frm}Win) -rw ${win}Win -width 256 -height 256
        pack $CompareGui(f${frm}Win) -side left -fill both -expand 1

        CompareInteractorBind $CompareGui(f${frm}Win)
    }

    #-------------------------------------------
    # Mosaik frame
    #-------------------------------------------

    set CompareGui(fCompareMosaik) $CompareGui(tCompareViewer).fMosaik
    frame $CompareGui(fCompareMosaik)
    set f $CompareGui(fCompareMosaik)

    frame $f.fImage    -bg $Gui(activeWorkspace)
    pack $f.fImage -side top -expand 1 -fill both

    #-------------------------------------------
    # Mosaik->Image frame
    #-------------------------------------------
    set f $CompareGui(fCompareMosaik).fImage
    set s $CompareMosaik(mosaikIndex)
    set win slCompare$s

    MakeVTKImageWindow $win
    ${win}Mapper SetInput [MultiSlicer GetCursor $CompareMosaik(mosaikIndex)]

    set frm SlCompare$s
    set CompareGui(f${frm}Win) $f.f${frm}Win
    vtkTkRenderWidget $CompareGui(f${frm}Win) -rw ${win}Win -width 256 -height 256
    pack $CompareGui(f${frm}Win) -side left -fill both -expand 1

    CompareInteractorBind $CompareGui(f${frm}Win)

    #-------------------------------------------
    # Set display to multi-slice mode and build slices annotations
    #-------------------------------------------
    set CompareViewer(multiOrMosaik) "multiSlice"
    CompareAnnoBuildGUI

    update
}


#-------------------------------------------------------------------------------
# .PROC CompareViewerShowSliceControls
# Called when the mouse enters the label located in the north east corner
# of the slice. Raise slice controls.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareViewerShowSliceControls {s} {
    global CompareGui

    raise $CompareGui(fCompareSlice$s).fControls
}

#-------------------------------------------------------------------------------
# .PROC MainViewerHideSliceControls
# Called when the mouse exits the slice controls. Lower the controls displaying
# back the north east label.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareViewerHideSliceControls {} {
    global CompareGui CompareSlice

    foreach s $CompareSlice(idList) {
       lower $CompareGui(fCompareSlice${s}).fControls $CompareGui(fCompareSlice${s}).fImage
    }
}

#-------------------------------------------------------------------------------
# .PROC CompareViewerAnno
# Sets the location of annotations in the slice window, depending on the slice
# size.
# .ARGS
# s int the slice id
# dim int the slice size (256 or 512)
# .END
#-------------------------------------------------------------------------------
proc CompareViewerAnno {s dim} {
    global CompareAnno CompareSlice

    foreach name $CompareAnno(mouseList) y $CompareAnno(y$dim) {
        [CompareAnno($s,$name,actor) GetPositionCoordinate] SetValue 1 $y
    }

    foreach name $CompareAnno(orientList) x $CompareAnno(orient,x$dim) \
        y $CompareAnno(orient,y$dim) {
        [CompareAnno($s,$name,actor) GetPositionCoordinate] SetValue $x $y
    }
}

#-------------------------------------------------------------------------------
# .PROC CompareViewerSetMode
# Change CompareViewer window to display up to 9 slices in multi slice
# mode, or the mosaik window.
# Update toplevel container size, slices window size, slices window packing,
# annotations location...
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareViewerSetMode {{mode ""} {verbose ""}} {
    global MultiSlicer CompareSlice CompareGui CompareViewer CompareMosaik Module

    if {$CompareViewer(multiOrMosaik) == "multiSlice"} {
        if {$mode != ""} {
            if {$mode == "2" || $mode == "3"  || $mode == "4" \
                || $mode == "6" || $mode == "9"} {

                set CompareViewer(mode) $mode
            } else {
                puts "CompareViewerSetMode: invalid mode $mode"
                return
            }
        }

        set f $CompareGui(tCompareViewer)
        pack forget $f.fTop $f.fMid $f.fBot
        foreach s $CompareSlice(idList) {
            pack forget $f.fSlice$s
        }
        pack forget $f.fMosaik

        switch $CompareViewer(mode) {
           "2" {
                pack $CompareGui(fTop) -side top
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                -in $CompareGui(fTop) -side left

                wm geometry .tCompareViewer 1024x512

                # Show the control thumbnails on top of the slice images
                foreach s "0 1" {
                    set str1 fSlCompare$s
                    set str2 Win
                    $CompareGui($str1$str2)  config -width 512 -height 512
                    raise $CompareGui(fCompareSlice$s).fThumb
                    CompareViewerAnno $s 512
                }
           }
           "3" {
                pack $CompareGui(fTop) $CompareGui(fMid) -side top
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                -in $CompareGui(fTop) -side left
                pack $CompareGui(fCompareSlice2) -in $CompareGui(fMid) -side left

                wm geometry .tCompareViewer 1024x1024

                # Show the control thumbnails on top of the slice images
                foreach s "0 1 2" {
                    $CompareGui(fSlCompare${s}Win)  config -width 512 -height 512
                    raise $CompareGui(fCompareSlice$s).fThumb
                    CompareViewerAnno $s 512
                }
            }
            "4" {
                pack $CompareGui(fTop) $CompareGui(fMid) -side top
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                -in $CompareGui(fTop) -side left
                pack $CompareGui(fCompareSlice2) $CompareGui(fCompareSlice3) \
                -in $CompareGui(fMid) -side left
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                $CompareGui(fCompareSlice2) $CompareGui(fCompareSlice3) -side left

                wm geometry .tCompareViewer 1024x1024

                # Show the control thumbnails on top of the slice images
                foreach s "0 1 2 3" {
                    $CompareGui(fSlCompare${s}Win)  config -width 512 -height 512
                    raise $CompareGui(fCompareSlice$s).fThumb
                    CompareViewerAnno $s 512
                }
            }
            "6" {
                pack $CompareGui(fTop) $CompareGui(fMid) -side top
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                $CompareGui(fCompareSlice2) -in $CompareGui(fTop) -side left
                pack $CompareGui(fCompareSlice3) $CompareGui(fCompareSlice4) \
                $CompareGui(fCompareSlice5) -in $CompareGui(fMid) -side left

                wm geometry .tCompareViewer 768x512

                # Show the control thumbnails on top of the slice images
                foreach s "0 1 2 3 4 5" {
                    $CompareGui(fSlCompare${s}Win)  config -width 256 -height 256
                    raise $CompareGui(fCompareSlice$s).fThumb
                    CompareViewerAnno $s 256
                }
            }
            "9" {
                pack $CompareGui(fTop) $CompareGui(fMid) $CompareGui(fBot) -side top
                pack $CompareGui(fCompareSlice0) $CompareGui(fCompareSlice1) \
                $CompareGui(fCompareSlice2) -in $CompareGui(fTop) -side left
                pack $CompareGui(fCompareSlice3) $CompareGui(fCompareSlice4) \
                $CompareGui(fCompareSlice5) -in $CompareGui(fMid) -side left
                pack $CompareGui(fCompareSlice6) $CompareGui(fCompareSlice7) \
                $CompareGui(fCompareSlice8) -in $CompareGui(fBot) -side left
                wm geometry .tCompareViewer 768x768
                # Show the control thumbnails on top of the slice images
                foreach s "0 1 2 3 4 5 6 7 8" {
                    $CompareGui(fSlCompare${s}Win)  config -width 256 -height 256
                    raise $CompareGui(fCompareSlice$s).fThumb
                    CompareViewerAnno $s 256
                }
            }
        }

        # Double the slice size in 512 mode
        if {$CompareViewer(mode) == "2" } {
            foreach s "0 1" {
                MultiSlicer SetDouble $s 1
                MultiSlicer SetCursorPosition $s 256 256
            }
        } elseif {$CompareViewer(mode) == "3"} {
            foreach s "0 1 2" {
                MultiSlicer SetDouble $s 1
                MultiSlicer SetCursorPosition $s 256 256
            }
        } elseif {$CompareViewer(mode) == "4"} {
            foreach s "0 1 2 3" {
                MultiSlicer SetDouble $s 1
                MultiSlicer SetCursorPosition $s 256 256
            }
        } elseif {$CompareViewer(mode) == "6" } {
            foreach s "0 1 2 3 4 5" {
                MultiSlicer SetDouble $s 0
                MultiSlicer SetCursorPosition $s 128 128
            }
        } elseif {$CompareViewer(mode) == "9" } {
            foreach s "0 1 2 3 4 5 6 7 8" {
                MultiSlicer SetDouble $s 0
                MultiSlicer SetCursorPosition $s 128 128
            }
        }
    } else {
        # if mosaik
        set f $CompareGui(tCompareViewer)
        pack forget $f.fTop $f.fMid $f.fBot
        foreach s $CompareSlice(idList) {
            pack forget $f.fSlice$s
        }

        pack forget $f.fMosaik
        pack $CompareGui(fTop) -side top
        pack $CompareGui(fCompareMosaik) -in $CompareGui(fTop) -side left

        wm geometry .tCompareViewer 512x512

        $CompareGui(fSlCompare${CompareMosaik(mosaikIndex)}Win)  config -width 512 -height 512
        CompareViewerAnno $CompareMosaik(mosaikIndex) 512

        MultiSlicer SetDouble $CompareMosaik(mosaikIndex) 1
        MultiSlicer SetCursorPosition $CompareMosaik(mosaikIndex) 256 256
    }
    MultiSlicer Update
}
