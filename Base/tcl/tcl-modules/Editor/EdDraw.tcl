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
# FILE:        EdDraw.tcl
# PROCEDURES:  
#   EdDrawInit
#   EdDrawBuildGUI
#   EdDrawEnter
#   EdDrawExit
#   EdDrawLabel
#   EdDrawUpdate
#   EdDrawApply
#   EdDrawUnapply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC EdDrawInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawInit {} {
    global Ed

    set e EdDraw
    set Ed($e,name)      "Draw"
    set Ed($e,initials)  "Dr"
    set Ed($e,desc)      "Draw: label pixels using a brush."
    set Ed($e,rank)      4
    set Ed($e,procGUI)   EdDrawBuildGUI
    set Ed($e,procEnter) EdDrawEnter
    set Ed($e,procExit)  EdDrawExit

    # Required
    set Ed($e,scope) Single 
    set Ed($e,input) Working

    set Ed($e,mode)   Draw
    set Ed($e,delete) Yes
    set Ed($e,radius) 0
    set Ed($e,shape)  Polygon
    set Ed($e,preshape) Polygon
    set Ed($e,render) Active
    set Ed($e,density) 3
    set Ed($e,slice) -1
    set Ed($e,polynum) -1
    set Ed($e,unapplynum) -1
    set Ed($e,closed) Closed
    set Ed($e,clear) No

    set Ed($e,eventManager) {}
    
}

#-------------------------------------------------------------------------------
# .PROC EdDrawBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawBuildGUI {} {
    global Ed Gui Label Editor

    #-------------------------------------------
    # Draw frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame)

    frame $f.fRender  -bg $Gui(activeWorkspace)
    frame $f.fMode    -bg $Gui(activeWorkspace)
    frame $f.fClear   -bg $Gui(activeWorkspace)
    frame $f.fDelete  -bg $Gui(activeWorkspace)
    frame $f.fGrid    -bg $Gui(activeWorkspace)
    frame $f.fBtns    -bg $Gui(activeWorkspace)
    frame $f.fClosed  -bg $Gui(activeWorkspace)
    frame $f.fShape   -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    frame $f.fToggle  -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fBtns $f.fClosed $f.fMode $f.fClear $f.fDelete \
        $f.fRender $f.fShape $f.fApply -side top -pady 2 -fill x
    pack $f.fToggle -side bottom -pady 4 -fill x
    
    EdBuildRenderGUI $Ed(EdDraw,frame).fRender Ed(EdDraw,render)

    #-------------------------------------------
    # Draw->Mode frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fMode

    frame $f.fMode -bg $Gui(activeWorkspace)
    eval {label $f.fMode.lMode -text "Mode:"} $Gui(WLA)
    pack $f.fMode.lMode -side left -padx 0 -pady 0
    foreach mode "Draw Select Move Insert" {
        eval {radiobutton $f.fMode.r$mode \
            -text "$mode" -variable Ed(EdDraw,mode) -value $mode \
            -width [expr [string length $mode] +1] \
            -indicatoron 0} $Gui(WCA)
        pack $f.fMode.r$mode -side left -padx 0 -pady 0
    }
    pack $f.fMode -side top -pady 5 -padx 0 

    #-------------------------------------------
    # Draw->Clear frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fClear

    eval {label $f.l -text "Clear labelmap before apply:"} $Gui(WLA)
    pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach s "Yes No" text "Yes No" width "4 3" {
        eval {radiobutton $f.r$s -width $width -indicatoron 0\
            -text "$text" -value "$s" -variable Ed(EdDraw,clear)} $Gui(WCA)
        pack $f.r$s -side left -fill x -anchor e
    }

    #-------------------------------------------
    # Draw->Delete frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fDelete

    eval {label $f.l -text "Delete points after apply:"} $Gui(WLA)
    pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach s "Yes No" text "Yes No" width "4 3" {
        eval {radiobutton $f.r$s -width $width -indicatoron 0\
            -text "$text" -value "$s" -variable Ed(EdDraw,delete)} $Gui(WCA)
        pack $f.r$s -side left -fill x -anchor e
    }

    #-------------------------------------------
    # Draw->Grid frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fGrid

    # Output label
    eval {button $f.bOutput -text "Output:" \
        -command "ShowLabels EdDrawLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
        -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdDrawLabel"
    bind $f.eOutput <FocusOut> "EdDrawLabel"
    eval {entry $f.eName -width 14 \
        -textvariable Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w

    lappend Label(colorWidgetList) $f.eName

    # Radius
    eval {label $f.lRadius -text "Point Radius:"} $Gui(WLA)
    eval {entry $f.eRadius -width 3 \
        -textvariable Ed(EdDraw,radius)} $Gui(WEA)
        bind $f.eRadius <Return> "EdDrawUpdate SetRadius; RenderActive"

    # Sampling density
    eval {label $f.lDensity -width 12 -text "Sample Density:"} $Gui(WLA)
    eval {entry $f.eDensity -width 3 \
        -textvariable Ed(EdDraw,density)} $Gui(WEA)

    grid $f.lRadius $f.eRadius $f.lDensity $f.eDensity -padx 2 -pady $Gui(pad) -sticky e
    grid $f.eRadius -sticky w
    grid $f.lDensity -sticky e 
    grid $f.eDensity -sticky w

    #-------------------------------------------
    # Draw->Btns frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fBtns

    eval {menubutton $f.mbEdit -text "Edit:" -relief raised -bd 2 \
        -width 6 -menu $f.mbEdit.m} $Gui(WMBA)
    eval {menu $f.mbEdit.m} $Gui(WMA)
    set editMenu $f.mbEdit.m
    $editMenu add command -label "Cut             (CTRL+X)" \
        -command "EdDrawUpdate Cut; RenderActive"
    $editMenu add command -label "Copy            (CTRL+C)" \
        -command "EdDrawUpdate Copy; RenderActive"
    $editMenu add command -label "Paste           (CTRL+V)" \
        -command "EdDrawUpdate Paste; RenderActive"
    $editMenu add command -label "Select All      (CTRL+A)" \
        -command "EdDrawUpdate SelectAll; RenderActive"
    $editMenu add command -label "Deselect All" \
        -command "EdDrawUpdate DeselectAll; RenderActive"
    $editMenu add command -label "Delete Selected (DELETE)" \
        -command "EdDrawUpdate DeleteSelected; RenderActive"
    $editMenu add command -label "Delete All      (CTRL+D)" \
        -command "EdDrawUpdate DeleteAll; RenderActive"
    pack $f.mbEdit -side left -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # Draw->Closed frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fClosed

    eval {label $f.clopen -text "Contour Topology:"} $Gui(WLA)
    pack $f.clopen -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

    foreach s "Closed Open" text "Closed Open" width "8 6" {
        eval {radiobutton $f.r$s -width $width -indicatoron 0\
            -command "EdDrawUpdate SetShape; RenderActive" \
            -text "$text" -value "$s" -variable Ed(EdDraw,closed)} $Gui(WCA)
        pack $f.r$s -side left -fill x -anchor e
    }

    #-------------------------------------------
    # Draw->Toggle frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fToggle
    
    set Editor(toggleWorking) 0
    eval {checkbutton $f.cW -width 21 -indicatoron 0 \
        -variable Editor(toggleWorking) \
        -text "peek under labelmap"  \
        -command EditorToggleWorking} $Gui(WCA) 
    pack $f.cW -side top -padx $Gui(pad) -pady $Gui(pad)

    TooltipAdd  $f.cW "Click to see grayscale only."

    #-------------------------------------------
    # Draw->Shape frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fShape

    eval {label $f.l -text "Shape:"} $Gui(WLA)
    pack $f.l -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach shape "Polygon Points" {
        eval {radiobutton $f.r$shape -width [expr [string length $shape]+1] \
            -text "$shape" -variable Ed(EdDraw,preshape) -value $shape \
            -command "EdDrawUpdate SetShape; RenderActive" \
            -indicatoron 0} $Gui(WCA)
        pack $f.r$shape -side left -fill x -anchor e
    }

    #-------------------------------------------
    # Draw->Apply frame
    #-------------------------------------------
    set f $Ed(EdDraw,frame).fApply

    eval {button $f.bApply -text "Apply" \
        -command "EdDrawApply"} $Gui(WBA) {-width 8}
    eval {button $f.bUnapply -text "Unapply" \
        -command "EdDrawUnapply; RenderActive"} $Gui(WBA) {-width 8}

    # To make Apply, Unapply in same line, make a new frame for them
    #pack $f.bApply $f.bUnapply -side top -padx $Gui(pad) -pady $Gui(pad)
    pack $f.bApply -side left -fill x -anchor e
    pack $f.bUnapply -side left -fill x -anchor e
}

#-------------------------------------------------------------------------------
# .PROC EdDrawEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawEnter {} {
    global Ed Label

    LabelsColorWidgets

    set e EdDraw
    Slicer DrawSetRadius $Ed($e,radius)
    Slicer DrawSetShapeTo$Ed($e,shape)
    if {$Label(activeID) != ""} {
        set color [Color($Label(activeID),node) GetDiffuseColor]
        eval Slicer DrawSetColor $color
    } else {
        Slicer DrawSetColor 0 0 0
    }
    if {$Ed($e,closed) == "Closed"} {
        Slicer DrawSetClosed 1
    } else {
        Slicer DrawSetClosed 0
    }

    # use the bindings stack for adding new bindings.
    pushEventManager $Ed($e,eventManager)
}

#-------------------------------------------------------------------------------
# .PROC EdDrawExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawExit {} {

    # Delete points
    EdDrawUpdate DeleteAll
    RenderActive

    popEventManager
}

#-------------------------------------------------------------------------------
# .PROC EdDrawLabel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawLabel {} {
    global Label

    LabelsFindLabel

    if {$Label(activeID) != ""} {
        set color [Color($Label(activeID),node) GetDiffuseColor]
        eval Slicer DrawSetColor $color
    } else {
        Slicer DrawSetColor 0 0 0
    }
}

#-------------------------------------------------------------------------------
# .PROC EdDrawUpdate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawUpdate {type} {
    global Ed Volume Label Gui

    set e EdDraw
    
    switch $type {
        NextMode {
            switch $Ed($e,mode) {
                "Draw" {
                    set Ed($e,mode) Select
                }
                "Select" {
                    set Ed($e,mode) Insert
                }
                "Move" {
                    set Ed($e,mode) Insert
                }
                "Insert" {
                    set Ed($e,mode) Draw
                }
            }
        }
        Delete {
            Slicer DrawDeleteSelected
            if {0} {
                EditorInsertPoint update
            }
            MainInteractorRender
        }
        "0" {
            switch $Ed($e,mode) {
                "Draw" {
                    EditorIdleProc apply 0
                }
                "Select" {
                    # nothing
                }
                "Move" {
                    # nothing
                }
                "Insert" {
                    # nothing
                }
            }
        }
        SelectAll {
            Slicer DrawSelectAll
            set Ed($e,mode) Select
        }
        DeselectAll {
            Slicer DrawDeselectAll
            set Ed($e,mode) Select
        }
        DeleteSelected {
            Slicer DrawDeleteSelected
            set Ed($e,mode) Draw
        }
        DeleteAll {
            Slicer DrawDeleteAll
            set Ed($e,mode) Draw
        }
        SetRadius {
            Slicer DrawSetRadius $Ed($e,radius)
            set Ed($e,radius) [Slicer DrawGetRadius]
        }
        SetShape {
            if { $Ed($e,preshape) == "Points" } {
                set Ed($e,shape) "Points"
            } else { # preshape is Polygon
                if { $Ed($e,closed) == "Closed" } {
                    set Ed($e,shape) "Polygon"
                    Slicer DrawSetClosed 1
                } else {
                    set Ed($e,shape) "Lines"
                    Slicer DrawSetClosed 0
                }
            }
            Slicer DrawSetShapeTo$Ed($e,shape)
            set Ed($e,shape) [Slicer GetShapeString]
        }
        Cut {
            set n [Slicer DrawGetNumPoints]
            if { $n < 1 } {
                tk_messageBox -message "There is no polygon to Cut.  The last cut/copied polygon is still available."
                # don't delete current CopyPoly if PolyDraw is empty
                return
            }
            Slicer CopySetDrawPoints
            Slicer DrawDeleteAll
        }
        Copy {
            set n [Slicer DrawGetNumPoints]
            if { $n < 1 } {
                tk_messageBox -message "There is no polygon to Copy.  The last cut/copied polygon is still available."
                # don't delete current CopyPoly if PolyDraw is empty
                return
            }
            Slicer CopySetDrawPoints
        }
        Paste {
            set poly [Slicer CopyGetPoints]
            set n [$poly GetNumberOfPoints]
            if { $n < 1 } {
                tk_messageBox -message "There is nothing to Paste.  The current polygon you are drawing remains unchanged."
                # don't erase PolyDraw if CopyPoly is empty
                return
            }
            Slicer DrawDeleteAll
            # Add each point of CopyPoly to PolyDraw
            for {set i 0} {$i < $n} {incr i} {
                set p [$poly GetPoint $i]
                scan $p "%d %d %d" xx yy zz
                Slicer DrawInsertPoint $xx $yy
            }
            Slicer DrawSelectAll
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC EdDrawApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawApply { {delete_pending true} } {
    global Ed Volume Label Gui Slice Interactor

    set e EdDraw
    set v [EditorGetInputID $Ed($e,input)]
 
    # Validate input
    if {[ValidateInt $Label(label)] == 0} {
        tk_messageBox -message "Output label is not an integer."
        return
    }
    if {[ValidateInt $Ed($e,radius)] == 0} {
        tk_messageBox -message "Point Radius is not an integer."
        return
    }

    EdSetupBeforeApplyEffect $v $Ed($e,scope) Active

    set Gui(progressText) "Draw on [Volume($v,node) GetName]"

    set label    $Label(label)
    set radius   $Ed($e,radius)
    set shape    $Ed($e,shape)
    set density  $Ed($e,density)

    #### How points selected by the user get here ###########
    # odonnell, 11-3-2000
    #
    # 1. Click point
    # 2. MainInteractorXY converts to "reformat point".
    #    This point is really just y-flipped and unzoomed.
    #    This does not actually use the reformat matrix at all.    
    # 3. Point goes on list in vtkMrmlSlicer, a vtkImageDrawROI object
    #    called PolyDraw.  This draws it on the screen.
    # 4. User hits Apply.
    # 5. Point is converted using reformat matrix from the volume
    #    in DrawComputeIjkPoints.  The output is "sort of 3D".  It is
    #    number,number,0 where the numbers are the i,j, or k
    #    coordinates for the point (and the slice would define 
    #    the other coordinate, but it is just 0).
    #    The regular ROI points would work in the original 
    #    (scanned) slice, but this conversion is needed for the 
    #    other two slices.
    #########################################################

    if { $Ed($e,slice) != $Slice($Interactor(s),offset) } {
        set Ed($e,slice) $Slice($Interactor(s),offset)
        set Ed($e,polynum) -1
        set Ed($e,unapplynum) -1
    }
    if { $Ed($e,unapplynum) != -1 } {
        # Remove unapplied polygon so we can reapply in same slot
        Slicer StackRemovePolygon $Slice($Interactor(s),offset) $Ed($e,unapplynum)
        Volume($v,vol) StackRemovePolygon $Slice($Interactor(s),offset) $Ed($e,unapplynum)
        Slicer RasStackRemovePolygon $Slice($Interactor(s),offset) $Ed($e,unapplynum)
        Volume($v,vol) RasStackRemovePolygon $Slice($Interactor(s),offset) $Ed($e,unapplynum)
    }
    set Ed($e,polynum) [Slicer StackGetNextInsertPosition $Slice($Interactor(s),offset) $Ed($e,polynum)]
    if { $Ed($e,polynum) == -1} { # Should only be true if polynum was -1 already
        return
    }
    if { $Ed($e,closed) == "Open" } {
        set closed 0
    } else {
        set closed 1
    }
    if { $Ed($e,preshape) == "Points" } {
        set preshape 0
    } else {
        set preshape 1
    }

    # set polygon and raspolygon in vtkMrmlSlicer and volume object
    Slicer StackSetPolygon $Slice($Interactor(s),offset) $Ed($e,polynum) $density $closed $preshape $label
    Volume($v,vol) StackSetPolygon [Slicer DrawGetPoints] $Slice($Interactor(s),offset) $Ed($e,polynum) $density $closed $preshape $label
    set raspoly [Slicer RasStackSetPolygon $Slice($Interactor(s),offset) $Ed($e,polynum) $density $closed $preshape $label]
    Volume($v,vol) RasStackSetPolygon $raspoly $Slice($Interactor(s),offset) $Ed($e,polynum) $density $closed $preshape $label

    set Ed($e,unapplynum) -1
    # (CTJ) For users who want to clear the current slice's labelmap reapply
    # all manually drawn polygons, there is now a "Clear" option that can be
    # chosen before applying; represented by the toggle variable Ed($e,clear)
    if { $Ed($e,clear) == "Yes" } {
        Ed(editor)   Clear
    }
    set numapply [Slicer StackGetNumApplyable $Slice($Interactor(s),offset)]
    for {set q 0} {$q < $numapply} {incr q} {
        # Get index p of qth polygon to apply
        set p [Slicer StackGetApplyable $Slice($Interactor(s),offset) $q]
        set poly [Slicer StackGetPoints $Slice($Interactor(s),offset) $p]
        set n [$poly GetNumberOfPoints]
        # If polygon empty, don't apply it.  It was already removed above
        if { $n > 0 } {
            Slicer DrawComputeIjkPoints $Slice($Interactor(s),offset) $p
            set points [Slicer GetDrawIjkPoints]
            set preshape [Slicer StackGetPreshape $Slice($Interactor(s),offset) $p]
            set label [Slicer StackGetLabel $Slice($Interactor(s),offset) $p]
            if { $preshape == 0 } {
                set shape "Points"
            } else {
                if { $Ed($e,closed) == "Closed" } {
                    set shape "Polygon"
                } else {
                    set shape "Lines"
                }
            }
            Ed(editor)   Draw $label $points $radius $shape
        }
    }

    # Dump points
    # points selected by the user and sent through MainInteractorXY
    #set oldpoints   [Slicer DrawGetPoints]
    #set n [$points GetNumberOfPoints]
    # compare to points converted to '~3D space' in DrawComputeIJKPoints
    #for {set i 0} {$i < $n} {incr i} {
    #    puts "ijk: [$points GetPoint $i] 2D: [$oldpoints GetPoint $i]"
    #}
    
    Ed(editor)   SetInput ""
    Ed(editor)   UseInputOff

    # Delete points?
    if {$Ed($e,delete) == "Yes"} {
        EdDrawUpdate DeleteAll
    } else {
        EdDrawUpdate DeselectAll
    }

    if { $delete_pending == "true" } {
        # the "__EditorPending_Points" is a special vtk object to communicate to 
        # the Editor.tcl module that the user really wants to apply now
        catch {__EditorPending_Points Delete}
    }

    EdUpdateAfterApplyEffect $v $Ed($e,render)
}

#-------------------------------------------------------------------------------
# .PROC EdDrawUnapply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdDrawUnapply {} {
    global Ed Volume Label Gui Slice Interactor

    set e EdDraw

    Slicer DrawDeleteAll
    if { $Ed($e,slice) != $Slice($Interactor(s),offset) } {
        set poly [Slicer StackGetPoints $Slice($Interactor(s),offset)]
        set ind [Slicer StackGetRetrievePosition $Slice($Interactor(s),offset)]
        set Label(label) [Slicer StackGetLabel $Slice($Interactor(s),offset) $ind]
        EdDrawLabel
        # n == -1 if poly is NULL
        set n [Slicer StackGetNumberOfPoints $Slice($Interactor(s),offset)]
        for {set i 0} {$i < $n} {incr i} {
            set p [$poly GetPoint $i]
            scan $p "%d %d %d" xx yy zz
            Slicer DrawInsertPoint $xx $yy
        }
        set Ed($e,slice) $Slice($Interactor(s),offset)
        set Ed($e,polynum) [Slicer StackGetRetrievePosition $Slice($Interactor(s),offset)]
        set Ed($e,unapplynum) $Ed($e,polynum)
    } else {
        set Ed($e,polynum) [Slicer StackGetNextRetrievePosition $Slice($Interactor(s),offset) $Ed($e,polynum)]
        if { $Ed($e,polynum) != -1 } {
            set Ed($e,unapplynum) $Ed($e,polynum)
            set poly [Slicer StackGetPoints $Slice($Interactor(s),offset) $Ed($e,polynum)]
            set Label(label) [Slicer StackGetLabel $Slice($Interactor(s),offset) $Ed($e,polynum)]
            EdDrawLabel
            set n [$poly GetNumberOfPoints]
            for {set i 0} {$i < $n} {incr i} {
                set p [$poly GetPoint $i]
                scan $p "%d %d %d" xx yy zz
                Slicer DrawInsertPoint $xx $yy
            }
        }
    }
}

