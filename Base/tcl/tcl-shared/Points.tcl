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
# FILE:        Points.tcl
# PROCEDURES:  
#   PointsInit
#   PointsBuildGUI
#   PointsRefreshGUI
#   PointIdFromGlyphCellId
#   PointsNew
#   PointsDeleteAll
#   PointsDelete
#   PointsRefreshVTK
#   PointsVTKId
#   PointLabelVisibility
#==========================================================================auto=
# Points.tcl

#-------------------------------------------------------------------------------
# .PROC PointsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsInit {} {
    global Point Module

    array set Point {scale 5.0 selected {} \
        -1,name "" \
        -1,xyz "" \
        -1,model "" \
        -1,desc "" \
        model "" \
        selected "" \
        vtkPoints "Point(vtkPoints)" \
        vtkFloatArray "Point(vtkFloatArray)" -1,name "<None>"}
    set Point(textscale) 6
    set Point(textpush) 10
    set Point(textvisible) 1
    set Point(textcolor) "0.4 1.0 1.0"
    set Point(textselcolor) "1.0 0.5 0.5"
    
#    set Point(vtkPoints) [vtkPoints Point(vtkPoints)]
#    set Point(vtkScalars) [vtkScalars Point(vtkScalars)]

    set Module(Points,procVTK) PointsBuildVTK
    #puts "Loading Points code"
    
    }

#-------------------------------------------------------------------------------
# .PROC PointsBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsBuildGUI {} {
    }

#-------------------------------------------------------------------------------
# .PROC PointsRefreshGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsRefreshGUI {} {
    }

#-------------------------------------------------------------------------------
# PointsBuildVTK
#    Builds the CellPicker and the Glyph3D objects to support point creation.
#-------------------------------------------------------------------------------
proc PointsBuildVTK {} {
    global viewRen Point

    #puts "PointsBuildVTK begin"

    set Point(glyphPoints) [vtkPoints glyphPoints]
      glyphPoints SetNumberOfPoints 6
      glyphPoints InsertPoint 0 1 0 0
      glyphPoints InsertPoint 1 0 1 0
      glyphPoints InsertPoint 2 0 0 1
      glyphPoints InsertPoint 3 -1 0 0
      glyphPoints InsertPoint 4 0 -1 0
      glyphPoints InsertPoint 5 0 0 -1
    set Point(glyphCells) [vtkCellArray glyphCells]
      glyphCells InsertNextCell 4
        glyphCells InsertCellPoint 0
        glyphCells InsertCellPoint 1
        glyphCells InsertCellPoint 3
        glyphCells InsertCellPoint 4
      glyphCells InsertNextCell 4
        glyphCells InsertCellPoint 1
        glyphCells InsertCellPoint 2
        glyphCells InsertCellPoint 4
        glyphCells InsertCellPoint 5
      glyphCells InsertNextCell 4
        glyphCells InsertCellPoint 2
        glyphCells InsertCellPoint 0
        glyphCells InsertCellPoint 5
        glyphCells InsertCellPoint 3

    set Point(glyphPD) [vtkPolyData glyphPD]
    glyphPD SetPoints glyphPoints
    glyphPoints Delete
    glyphPD SetPolys glyphCells
    glyphCells Delete

    set Point(xform) [vtkTransform glyphTransform]
      set s $Point(scale)
      glyphTransform Scale $s $s $s

    vtkTransformPolyDataFilter glyphFilter
      glyphFilter SetInput glyphPD
      glyphFilter SetTransform glyphTransform

    set Point(vtkPoints) [vtkPoints Point(vtkPoints)]
    set Point(vtkFloatArray) [vtkFloatArray Point(vtkFloatArray)]

    set Point(positions) [vtkPolyData Point(positions)]
      $Point(positions) SetPoints $Point(vtkPoints)
      [$Point(positions) GetPointData] SetScalars $Point(vtkFloatArray)

    set Point(selected) [list]

    vtkGlyph3D Point(glyphs)
      Point(glyphs) SetSource [glyphFilter GetOutput]
      Point(glyphs) SetInput $Point(positions)
      Point(glyphs) SetScaleFactor 1.0
      Point(glyphs) ClampingOn
      Point(glyphs) ScalingOff
      Point(glyphs) SetRange 0 1

    set Point(mapper) [vtkPolyDataMapper glyphMapper]
      glyphMapper SetInput [Point(glyphs) GetOutput]
      [glyphMapper GetLookupTable] SetSaturationRange .65 .65
      [glyphMapper GetLookupTable] SetHueRange .5 0

    set Point(actor) [vtkActor Point(actor)]
      Point(actor) SetMapper glyphMapper
      [Point(actor) GetProperty] SetColor 1 0 0

    set Point(textxform) [vtkTransform Point(textxform)]
    Point(textxform) Translate 0 0 $Point(textpush)
    [Point(textxform) GetMatrix] SetElement 0 1 .333
    Point(textxform) Scale $Point(textscale) $Point(textscale) 1

    PointsRefreshVTK

    viewRen AddActor $Point(actor)

    #puts "PointsBuildVTK end"
}

#-------------------------------------------------------------------------------
# .PROC PointIdFromGlyphCellId
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointIdFromGlyphCellId { cellId } {
    global Point

    set num [$Point(glyphPD) GetNumberOfCells]
    set vtkId [expr $cellId/$num]
    return [lindex $Point(idList) $vtkId]
}


#-------------------------------------------------------------------------------
# .PROC PointsNew
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsNew { x y z {name PNT}} {
    global Point

    set id $Point(nextID)
    incr Point(nextID)
    lappend Point(idList) $id
    lappend Point(selected) $id
#    incr Point(num)
    set Point($id,name) $name
    set Point($id,model) $Point(model)
    set Point($id,xyz) "$x $y $z"
    set Point($id,desc) ""
    if { $name == "PNT" } {
        append Point($id,name) [format %03d $id]
    }
    PointsRefreshVTK

    return $id
}

# status 1=toggle status. status 2=deselect
proc PointsSelect { pntid {status 1} } {
    global Point

    set selidx [lsearch $Point(selected) $pntid]
    if { $status != 0 } {
        if { $selidx < 0 } {
            lappend Point(selected) $pntid
        } else {
            set Point(selected) [lreplace $Point(selected) $selidx $selidx]
        }
    } else {
        if { $selidx > -1 } {
            set Point(selected) [lreplace $Point(selected) $selidx $selidx]
        }
    }

    PointsRefreshVTK
    }

#-------------------------------------------------------------------------------
# .PROC PointsDeleteAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsDeleteAll {} {
    if { [info exists Points(idList)] } {
        foreach id $Points(idList) {
            PointsDelete $id
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC PointsDelete
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsDelete { pntid } {
    global Point
    
    set idx [lsearch $Point(idList) $pntid]
    puts [concat "DEBUG: PointsDelete: idx: " $idx]
    if { $idx < 0 } {
        return
    }
    
    #    incr Point(num) -1y
    [viewRen GetActors] RemoveItem Point($pntid,follower)
    foreach foo [array names Point $pntid,*] {
        unset Point($foo)
    }
    if { $idx >= 0 } {
        set Point(idList) [lreplace $Point(idList) $idx $idx]
    }
    set selidx [lsearch $Point(selected) $pntid]
    if { $selidx >= 0 } {
        set Point(selected) [lreplace $Point(selected) $selidx $selidx]
    }
    PointsRefreshVTK
}

#-------------------------------------------------------------------------------
# .PROC PointsRefreshVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsRefreshVTK {} {
    global Point

    $Point(vtkPoints) SetNumberOfPoints 0
    $Point(vtkFloatArray) SetNumberOfTuples 0
    foreach id $Point(idList) {
        if {![info exists Point($id,xyz)]} {
            puts "no xyz for point $id"
            continue
        }
        set vtkId [eval $Point(vtkPoints) InsertNextPoint $Point($id,xyz)]
        $Point(vtkFloatArray) InsertNextTuple1 0
        if { [info commands Point($id,text)] == "" } {
            set Point($id,text) [vtkVectorText Point($id,text)]
        }
        Point($id,text) SetText $Point($id,name)
        if { [info commands Point($id,mapper)] == "" } {
            set Point($id,mapper) [vtkPolyDataMapper \
                        Point($id,mapper)]
            set Point($id,textxform) [vtkTransformPolyDataFilter \
                        Point($id,textxform)]
            Point($id,textxform) SetTransform Point(textxform)
        }
        Point($id,textxform) SetInput [Point($id,text) GetOutput]
        Point($id,mapper) SetInput [Point($id,textxform) GetOutput]
        if { [info commands Point($id,follower)] == "" } {
            set Point($id,follower) [vtkFollower \
                        Point($id,follower)]
        }
        Point($id,follower) SetMapper Point($id,mapper)
        Point($id,follower) SetCamera [viewRen GetActiveCamera]
        Point($id,follower) SetVisibility $Point(textvisible)
        eval [Point($id,follower) GetProperty] SetColor $Point(textcolor)
        eval Point($id,follower) SetPosition $Point($id,xyz)
        [viewRen GetActors] RemoveItem Point($id,follower)
        viewRen AddActor Point($id,follower)
    }
    foreach id $Point(selected) {
        set vtkId [PointsVTKId $id]
        $Point(vtkFloatArray) SetTuple1 $vtkId 1
        if { [info commands Point($id,follower)] == "" } {
            puts "no follower for point $id"
        } else {
            eval [Point($id,follower) GetProperty] SetColor $Point(textselcolor)
        }
    }
    $Point(vtkPoints) Modified
    $Point(vtkFloatArray) Modified

    if { [info command OsteoPlanRefreshGUI] != "" } {
        OsteoPlanRefreshGUI
    }
}

#-------------------------------------------------------------------------------
# .PROC PointsVTKId
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointsVTKId { pntid } {
    global Point

    return [lsearch $Point(idList) $pntid]
    }

#-------------------------------------------------------------------------------
# .PROC PointLabelVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc PointLabelVisibility { vis } {
    global Point

    set Point(textvisible) $vis

    PointsRefreshVTK
    Render3D
    }

