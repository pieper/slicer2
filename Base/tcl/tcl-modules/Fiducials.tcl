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
# FILE:        Fiducials.tcl
# PROCEDURES:  
#   FiducialsInit
#   FiducialsEnter
#   FiducialsExit
#   FiducialsBuildGUI
#   FiducialsBuildVTK
#   FiducialsVTKCreateFiducialsList id
#   FiducialsVTKCreatePoint fid pid
#   FiducialsVTKUpdatePoints
#   FiducialsSetTxtScale
#   FiducialsSetScale
#   FiducialsUpdateMRML
#   FiducialsResetVariables
#   FiducialsSetActiveList name menu scroll
#   FiducialsDeletePoint fid pid
#   FiducialsActiveDeleteList
#   FiducialsDeleteList fid
#   FiducialsPointIdFromGlyphCellId fid cid
#   FiducialsScalarIdFromPointId pid
#   FiducialsSelectionFromPicker actor cellId
#   FiducialsSelectionFromScroll actor cellId
#   FiducialsUpdateSelectionForActor fid
#   FiducialsDeleteFromPicker actor cellId
#   FiducialsSetFiducialsVisibility rendererName name visibility
#   FiducialsAddActiveListFrame frame scrollHeight scrollWidth defaultNames
#   FiducialsCreateFiducialsList name
#   FiducialsCreatePointFromWorldXYZ x y z name listName
#   FiducialsWorldPointXYZ
#   FiducialsGetPointCoordinates
#   FiducialsGetPointIdListFromName
#   FiducialsGetSelectedPointIdListFromName
#   FiducialsGetAllSelectedPointIdList
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC FiducialsInit
#       The init procedure that creates tcl variables for that module
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsInit {} {
    global Fiducials Module Volume Model
    
    set m Fiducials
    set Module($m,row1List) "Help Display Edit"
    set Module($m,row1Name) "{Help} {Display} {Edit}"
#    set Module($m,row1List) "Help Edit"
#    set Module($m,row1Name) "{Help} {Edit}"
    set Module($m,row1,tab) Edit
    set Module($m,procVTK) FiducialsBuildVTK
    set Module($m,procEnter) FiducialsEnter
    set Module($m,procExit) FiducialsExit
    set Module($m,procMRML) FiducialsUpdateMRML
    
    
    set Module($m,procGUI) FiducialsBuildGUI

    # Set Dependencies
    set Module($m,depend) ""

    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.23 $} {$Date: 2002/09/06 14:54:27 $}]
    
    # Initialize module-level variables
    
    set Fiducials(renList) "viewRen matRen"
    set Fiducials(scale) 6
    set Fiducials(minScale) 0
    set Fiducials(maxScale) 40
    set Fiducials(textScale) 4.5
    set Fiducials(textSlant) .333
    set Fiducials(textPush) 10
    set Fiducials(textColor) "0.4 1.0 1.0"
    set Fiducials(textSelColor) "1.0 0.5 0.5"
    
    # Append widgets to list that gets refreshed during UpdateMRML
    set Fiducials(mbActiveList) ""
    set Fiducials(mActiveList)  ""
    set Fiducials(scrollActiveList) ""

    set Fiducials(activeList) NONE
    set Fiducials(defaultNames) ""
    # List of Fiducials node names
    set Fiducials(listOfNames) ""
    set Fiducials(listOfIds) ""
    
    set Fiducials(displayList) ""
    
    set Fiducials(howto) "
You can add Fiducials point on the volume in the 2D slice windows or on any models in the 3D View.

Fiducial points are grouped in lists. Each Fiducial list has a name. You have to select a list before creating a Fiducial point. 
If you want to create a new list, go to the Fiducials module.

To create a Fiducial point: point to the location with the mouse and press 'p' on the keyboard.
To select/unselect a Fiducial point: point to the Fiducial that you want to select/unselect with the mouse and press 'q' on the keyboard.
To delete a Fiducial: point to the Fiducial that you want to delete with the mouse and press 'd' on the keyboard. 
NOTE: it is important to press 'p' and not 'P', 'd' and not 'D' and 'q' and not 'Q'. "

set Fiducials(help) "
<BR> Fiducial points can be added by the user on any models or any actor in the 2D slice screens or the 3D screens. Fiducial points are useful for measuring distances and angles, as well as for other modules (i.e slice reformatting)
<BR>
<BR> Fiducial points are grouped in lists. Each Fiducial list has a name. You have to select a list before creating a Fiducial. 
If you want to create a new list, go to the Fiducials module.
<BR>
<BR> You can add Fiducial points on the volume in the 2D slice windows or on any models in the 3D View. Here is how to do it:
<BR>
<BR> <LI><B>To create a Fiducial point </B>: point to the location with the mouse and press 'p' on the keyboard
<BR> <LI><B> To select/unselect a Fiducial </B>: point to the Fiducial that you want to select/unselect with the mouse and press 'q' on the keyboard. You can also select/unselect Fiducials points in the scrolled textbox.
<BR> <LI> <B> To delete a Fiducial </B>: point to the Fiducial that you want to delete with the mouse and press 'd' on the keyboard. "

}


#-------------------------------------------------------------------------------
# .PROC FiducialsEnter
#  The event Manager for the Fiducials is pushed onto the event stack. The event manager binds user actions such as key presses to actions.
# The Fiducials event manager has bindings for the keys 'p'(create a Fiducial point), 'q'(select/unselect a Fiducial Point), and 'd' (delete a Fiducial Point)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsEnter {} {
    global Fiducials Events
    
    Render3D 
}

#-------------------------------------------------------------------------------
# .PROC FiducialsExit
# Pops the event manager
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsExit {} {
    global Events
    
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC FiducialsBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc FiducialsBuildGUI {} {
    global Gui Fiducials Module Volume Model

    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(Fiducials,fStuff)

    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    # Edit
    #   Top
    #   Bottom
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------

    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
$Fiducials(help) "

    regsub -all "\n" $help {} help
    MainHelpApplyTags Fiducials $help
    MainHelpBuildGUI Fiducials

    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Fiducials,fDisplay)
    set f $fDisplay

    foreach frame "buttons list scroll" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display->Size frame
    #-------------------------------------------
    set f $fDisplay.fbuttons

    DevAddButton $f.bAll "Show All" \
            "FiducialsSetFiducialsVisibility ALL; Render3D" 10 
    DevAddButton $f.bNone "Show None" \
            "FiducialsSetFiducialsVisibility NONE; Render3D" 10 
    pack $f.bAll $f.bNone -side left -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # fDisplay->Grid frame
    #-------------------------------------------
    set f $fDisplay.flist
    eval {label $f.lV -text "Visibility"} $Gui(WLA)
    eval {label $f.lO -text "Symbol Size" } $Gui(WLA)
    eval {label $f.tO -text "Text Size"} $Gui(WLA) 
    grid $f.lV $f.lO $f.tO  -pady 0 -padx 5
    

    set f $fDisplay.fscroll
    

    
    DevCreateScrollList $Module(Fiducials,fDisplay).fscroll \
        FiducialsCreateGUI \
        FiducialsConfigScrolledGUI \
        "$Fiducials(idList)"

    set Fiducials(canvasScrolledGUI)  $Module(Fiducials,fDisplay).fscroll.cGrid
    set Fiducials(fScrolledGUI)       $Fiducials(canvasScrolledGUI).fListItems

    # Done in MainModelsCreateGUI

    #-------------------------------------------
    # Edit frame
    #-------------------------------------------
    set fEdit $Module(Fiducials,fEdit)
    set f $fEdit

    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Edit->Top frame
    #-------------------------------------------
    set f $fEdit.fTop
        
    eval {label $f.lCreateList -text "Create a Fiducials List:"} $Gui(WLA)
    eval {entry $f.eCreateList -width 15 -textvariable Fiducials(newListName) } \
                $Gui(WEA)
    bind $f.eCreateList <Return> {FiducialsCreateFiducialsList "default" $Fiducials(newListName)}

    pack $f.lCreateList $f.eCreateList -side left -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Edit->Middle frame
    #-------------------------------------------
    set f $fEdit.fMiddle

    FiducialsAddActiveListFrame $f 15 25 
    
    }
    

#-------------------------------------------------------------------------------
# .PROC FiducialsCreateGUI
# Makes the GUI for each model on the Fiducials->Display panel.
# This is called for each new model.
# Also makes the popup menu that comes up when you right-click a model.
#
# .ARGS
# f widget the frame to create the GUI in
# m int the id of the model
# hlevel int the indentation to use when building the GUI
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreateGUI {f id} {
    global Gui Model Color Fiducials

    lappend Fiducials(displayList) $id

    # puts "Creating GUI for model $m"        
    # If the GUI already exists, then just change name.
    if {[info command $f.c$id] != ""} {
        $f.c$id config -text "[Fiducials($id,node) GetName]"
        return 0
    }
    
    # Name / Visible
    set name [Fiducials($id,node) GetName]
    eval {checkbutton $f.c$id \
        -text [Fiducials($id,node) GetName] -variable Fiducials($id,visibility)        -width 17 -indicatoron 0 \
        -command "FiducialsSetFiducialsVisibility [Fiducials($id,node) GetName]; Render3D"} $Gui(WCA)
    
    # menu
    eval {menu $f.c$id.men} $Gui(WMA)
    set men $f.c$id.men
   
    $men add command -label "Delete" -command "FiducialsDeleteList $name; Render3D"
    $men add command -label "-- Close Menu --" -command "$men unpost"
    bind $f.c$id <Button-3> "$men post %X %Y"
    
    # Scale
    #eval {entry $f.e${id} -textvariable Fiducials($id,scale) -width 3} $Gui(WEA)
    #bind $f.e${id} <Return> "FiducialsSetScale $id; Render3D"
    #bind $f.e${id} <FocusOut> "FiducialsSetScale $id; Render3D"
    eval {scale $f.s${id} -from 0.0 -to 80.0 -length 40 \
        -variable Fiducials($id,scale) \
        -command "FiducialsSetScale $id" \
        -resolution 1} $Gui(WSA) {-sliderlength 10 }

# text Scale
    #eval {entry $f.et${id} -textvariable Fiducials($id,textScale) -width 3} $Gui(WEA)
    #bind $f.et${id} <Return> "FiducialsSetTxtScale $id; Render3D"
    #bind $f.et${id} <FocusOut> "FiducialsSetTxtScale $id; Render3D"
    eval {scale $f.st${id} -from 0.0 -to 20.0 -length 40 \
        -variable Fiducials($id,textScale) \
        -command "FiducialsSetTxtScale $id" \
        -resolution 0.2} $Gui(WSA) {-sliderlength 10 }
        

eval grid $f.c${id}  $f.s${id}  $f.st${id} -pady 2 -padx 2 -sticky we

    return 1

}

#-------------------------------------------------------------------------------
# .PROC FiducialsDeleteGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsDeleteGUI {f m} {
    global Gui Model Color Fiducials


    # If the GUI is already deleted, return
    if {[info command $f.c$m] == ""} {
        return 0
    }

    set index [lsearch $Fiducials(displayList) $m]
    set Fiducials(displayList) [lreplace $Fiducials(displayList) $index $index]

    # Destroy TK widgets
    destroy $f.c$m
    destroy $f.e$m
    destroy $f.st$m
    destroy $f.s$m

    return 1
}

#-------------------------------------------------------------------------------
# .PROC FiducialsConfigScrolledGUI
# 
# Set the dimensions of the scrolledGUI
#
# .ARGS
#
# frame  canvasScrolledGUI  The canvas around the scrolled frame
# frame  fScrolledGUI       The frame with the item list of models
# .END   
#-------------------------------------------------------------------------------
proc FiducialsConfigScrolledGUI {canvasScrolledGUI fScrolledGUI} {
    global Fiducials


    set f      $fScrolledGUI
    set canvas $canvasScrolledGUI
    set m [lindex $Fiducials(idList) 0]
    # y spacing important for calculation of frame height for scrolling
    set pady 2

    if {$m != ""} {
        # Find the height of a single button
        # Must use $f.s$m since the scrollbar ("s") fields are tallest
        set lastButton $f.c$m
        # Find how many modules (lines) in the frame
        set numLines 0
        foreach m $Fiducials(idList) {
                incr numLines
        }
        # Find the height of a line
        set incr [expr {[winfo reqheight $lastButton] + 2*$pady}]
        # Find the total height that should scroll
        set height [expr {$numLines * $incr}]
        # Find the width of the scrolling region
        update;     # wait for some stuff to be done before requesting
        # window positions
        set last_x [winfo x $lastButton]
        set width [expr $last_x + [winfo reqwidth $lastButton]]
        $canvas config -scrollregion "0 0 $width $height"
        $canvas config -yscrollincrement $incr -confine true
        $canvas config -xscrollincrement 1 -confine true
    }
#        $canvas config -scrollregion "0 0 100 300"
#        $canvas config -yscrollincrement 1 -confine true
#        $canvas config -xscrollincrement 1 -confine true

}

#-------------------------------------------------------------------------------
# .PROC FiducialsBuildVTK
#
# Create the VTK structures used for displaying Fiducials.
# .END
#-------------------------------------------------------------------------------
proc FiducialsBuildVTK {} {
    global Fiducials Point

    # create the picker
    vtkCellPicker Fiducials(picker)
    Fiducials(picker) SetTolerance 0.001

    ########################################################
    #
    #       CREATE THE VTK SOURCE FOR THE DIAMOND FIDUCIALS
    #
    ########################################################
    vtkPoints Fiducials(symbolPoints)
    Fiducials(symbolPoints) SetNumberOfPoints 6
      Fiducials(symbolPoints) InsertPoint 0 1 0 0
      Fiducials(symbolPoints) InsertPoint 1 0 1 0
      Fiducials(symbolPoints) InsertPoint 2 0 0 1
      Fiducials(symbolPoints) InsertPoint 3 -1 0 0
      Fiducials(symbolPoints) InsertPoint 4 0 -1 0
      Fiducials(symbolPoints) InsertPoint 5 0 0 -1
    vtkCellArray Fiducials(symbolPolys)
      Fiducials(symbolPolys) InsertNextCell 4
        Fiducials(symbolPolys) InsertCellPoint 0
        Fiducials(symbolPolys) InsertCellPoint 1
        Fiducials(symbolPolys) InsertCellPoint 3
        Fiducials(symbolPolys) InsertCellPoint 4
      Fiducials(symbolPolys) InsertNextCell 4
        Fiducials(symbolPolys) InsertCellPoint 1
        Fiducials(symbolPolys) InsertCellPoint 2
        Fiducials(symbolPolys) InsertCellPoint 4
        Fiducials(symbolPolys) InsertCellPoint 5
      Fiducials(symbolPolys) InsertNextCell 4
        Fiducials(symbolPolys) InsertCellPoint 2
        Fiducials(symbolPolys) InsertCellPoint 0
        Fiducials(symbolPolys) InsertCellPoint 5
        Fiducials(symbolPolys) InsertCellPoint 3
    vtkCellArray Fiducials(symbolLines)
      Fiducials(symbolLines) InsertNextCell 2
        Fiducials(symbolLines) InsertCellPoint 0
        Fiducials(symbolLines) InsertCellPoint 3
      Fiducials(symbolLines) InsertNextCell 2
        Fiducials(symbolLines) InsertCellPoint 1
        Fiducials(symbolLines) InsertCellPoint 4
      Fiducials(symbolLines) InsertNextCell 2
        Fiducials(symbolLines) InsertCellPoint 2
        Fiducials(symbolLines) InsertCellPoint 5

    vtkPolyData Fiducials(symbolPD)
    Fiducials(symbolPD) SetPoints Fiducials(symbolPoints)
    Fiducials(symbolPoints) Delete
    Fiducials(symbolPD) SetPolys Fiducials(symbolPolys)
    Fiducials(symbolPD) SetLines Fiducials(symbolLines)
    Fiducials(symbolPolys) Delete
    Fiducials(symbolLines) Delete

    vtkTransform Fiducials(tmpXform)
      Fiducials(tmpXform) PostMultiply

    ########################################################
    #
    #       CREATE THE VTK SOURCE FOR THE DIAMOND FIDUCIALS
    #
    ########################################################
    

    vtkSphereSource   Fiducials(sphereSource)     
    Fiducials(sphereSource) SetRadius 0.1
    Fiducials(sphereSource)     SetPhiResolution 10
    Fiducials(sphereSource)     SetThetaResolution 10
    
}


#-------------------------------------------------------------------------------
# .PROC FiducialsVTKCreateFiducialsList
#
# Create a new set of fiducials vtk variables/actors corresponding to that list
# id
# .ARGS 
#  int id The Mrml id of the Fiducials list
# .END
#-------------------------------------------------------------------------------
proc FiducialsVTKCreateFiducialsList { id type {scale ""} {textScale ""} {visibility ""}} {
    global Fiducials Mrml Module
    
    if {$scale == "" } {
    set scale $Fiducials(scale)
    }
    if {$textScale == "" } {
    set textScale $Fiducials(textScale)
    }
    vtkPoints Fiducials($id,points)
    
    vtkFloatArray Fiducials($id,scalars)
    
    vtkPolyData Fiducials($id,pointsPD)
    Fiducials($id,pointsPD) SetPoints Fiducials($id,points)
    [Fiducials($id,pointsPD) GetPointData] SetScalars Fiducials($id,scalars)
    vtkGlyph3D Fiducials($id,glyphs)
    
    # set the default size for text 
    vtkTransform Point($id,textXform)
    Point($id,textXform) Translate 0 0 $Fiducials(textPush)
    [Point($id,textXform) GetMatrix] SetElement 0 1 .333
    Point($id,textXform) Scale $textScale $textScale 1

    # set the default size for symbols
    vtkTransform Fiducials($id,symbolXform)
    Fiducials($id,symbolXform) Scale $scale $scale $scale
    
    
    vtkTransformPolyDataFilter Fiducials($id,XformFilter)
    if {$type == "endoscopic"} {
    Fiducials($id,XformFilter) SetInput [Fiducials(sphereSource) GetOutput]
    } else {
    Fiducials($id,XformFilter) SetInput Fiducials(symbolPD)
    }
    Fiducials($id,XformFilter) SetTransform Fiducials($id,symbolXform)
    
    Fiducials($id,glyphs) SetSource \
        [Fiducials($id,XformFilter) GetOutput]
    
    Fiducials($id,glyphs) SetInput Fiducials($id,pointsPD)
    Fiducials($id,glyphs) SetScaleFactor 1.0
    Fiducials($id,glyphs) ClampingOn
    Fiducials($id,glyphs) ScalingOff
    Fiducials($id,glyphs) SetRange 0 1
    
    vtkPolyDataMapper Fiducials($id,mapper)
    Fiducials($id,mapper) SetInput [Fiducials($id,glyphs) GetOutput]
    [Fiducials($id,mapper) GetLookupTable] SetSaturationRange .65 .65
    [Fiducials($id,mapper) GetLookupTable] SetHueRange .5 0
    
    vtkMatrix4x4 Fiducials($id,xform)
    Mrml(dataTree) ComputeNodeTransform Fiducials($id,node) \
        Fiducials($id,xform)

    # create a different actor for each renderer
    foreach r $Fiducials(renList) {
    vtkActor Fiducials($id,actor,$r)
    Fiducials($id,actor,$r) SetMapper Fiducials($id,mapper)
    [Fiducials($id,actor,$r) GetProperty] SetColor 1 0 0
    [Fiducials($id,actor,$r) GetProperty] SetInterpolationToFlat
    Fiducials($id,actor,$r) SetUserMatrix Fiducials($id,xform)
    
    $r AddActor Fiducials($id,actor,$r)
    }  
    
    # now set the visibility
    FiducialsSetFiducialsVisibility $Fiducials($id,name) $visibility
}

#-------------------------------------------------------------------------------
# .PROC FiducialsVTKCreatePoint
#
# Create a point follower (vtk actor) 
# .ARGS 
#       int fid Mrml id of the list that contains the new Point
#       int pid Mrml id of the Point
# .END
#-------------------------------------------------------------------------------
proc FiducialsVTKCreatePoint { fid pid visibility} {
    global Fiducials Point Mrml Module
    
    
    vtkVectorText Point($pid,text)
    Point($pid,text) SetText [Point($pid,node) GetName]
    vtkPolyDataMapper Point($pid,mapper)
    Point($pid,mapper) SetInput [Point($pid,text) GetOutput]
    foreach r $Fiducials(renList) {
    vtkFollower Point($pid,follower,$r)
    Point($pid,follower,$r) SetMapper Point($pid,mapper)
    Point($pid,follower,$r) SetCamera [$r GetActiveCamera]
    Point($pid,follower,$r) SetUserMatrix [Point($fid,textXform) GetMatrix]
    Point($pid,follower,$r) SetPickable 0
    eval [Point($pid,follower,$r) GetProperty] SetColor $Fiducials(textColor)
    
    $r AddActor Point($pid,follower,$r)
    Point($pid,follower,$r) SetVisibility $visibility
    }
}


#-------------------------------------------------------------------------------
# .PROC FiducialsVTKUpdatePoints
#
# Update the points contained within all Fiducials/EndFiducials node after 
# the UpdateMRML
#
# .END
#-------------------------------------------------------------------------------
proc FiducialsVTKUpdatePoints {fid symbolSize textSize} {
    global Fiducials Point Mrml Module
    
    
    Mrml(dataTree) ComputeNodeTransform Fiducials($fid,node) \
        Fiducials($fid,xform)
    Fiducials(tmpXform) SetMatrix Fiducials($fid,xform)
    Fiducials($fid,points) SetNumberOfPoints 0
    Fiducials($fid,scalars) SetNumberOfTuples 0
    
    foreach pid $Fiducials($fid,pointIdList) {
        set xyz [Point($pid,node) GetXYZ]
        eval Fiducials($fid,points) InsertNextPoint $xyz
        Fiducials($fid,scalars) InsertNextTuple1 0        
        #eval Fiducials(tmpXform) SetPoint $xyz 1
        #set xyz [Fiducials(tmpXform) GetPoint]
    eval Fiducials(tmpXform) TransformPoint $xyz
        foreach r $Fiducials(renList) {
        eval Point($pid,follower,$r) SetPosition $xyz
        }
        Point($pid,text) SetText [Point($pid,node) GetName]
    }

    # color the selected glyphs (by setting their scalar to 1)
    if {[info exists Fiducials($fid,oldSelectedPointIdList)]} {
        foreach pid $Fiducials($fid,pointIdList) {
        # see if that point was previously selected
        if {[lsearch $Fiducials($fid,oldSelectedPointIdList) $pid] != -1} { 
            # color the point
            Fiducials($fid,scalars) SetTuple1 [FiducialsScalarIdFromPointId $fid $pid] 1
            # color the text
            foreach r $Fiducials(renList) {
            eval [Point($pid,follower,$r) GetProperty] SetColor $Fiducials(textSelColor)
            }
            # add it to the current list of selected items
            lappend Fiducials($fid,selectedPointIdList) $pid
        }
        }
    }
    
    Fiducials($fid,pointsPD) Modified

}



#-------------------------------------------------------------------------------
# .PROC FiducialsSetTxtScale
#
# Set the scale of the Fiducials symbol
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetTxtScale { id {val ""} } {
    global Fiducials Point

    if { $val == ""} {
    set val $Fiducials($id,textScale)
    }
    set s $Fiducials(textScale)
    Point($id,textXform) Identity
    Point($id,textXform) Translate 0 0 $Fiducials(textPush)
    [Point($id,textXform) GetMatrix] SetElement 0 1 .333
    Point($id,textXform) Scale $val $val 1
    Point($id,textXform) Update
    Fiducials($id,node) SetTextSize $val
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC FiducialsSetScale
#
# Set the scale of the Fiducials symbol
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetScale { id {val ""}} {
    global Fiducials
    
    if { $val == ""} {
    set val $Fiducials($id,scale)
    }
    
    set s $Fiducials(scale)
    Fiducials($id,symbolXform) Identity
    Fiducials($id,symbolXform) Scale $val $val $val
    Fiducials($id,symbolXform) Modified
    Fiducials($id,XformFilter) Update
    Fiducials($id,node) SetSymbolSize $val
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC FiducialsUpdateMRML
#
# Update the Fiducials actors and scroll textboxes based on the current Mrml Tree
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdateMRML {} {
    global Fiducials Mrml Module Models Model Landmark Path EndPath


    # start callback in case any module wants to know that Fiducials are 
    # about to be updated
    foreach m $Module(idList) {
    if {[info exists Module($m,fiducialsStartUpdateMRMLCallback)] == 1} {
        if {$Module(verbose) == 1} {puts "Fiducials Start Callback: $m"}
        $Module($m,fiducialsStartUpdateMRMLCallback)  
    }
    }
       
    ############################################################
    # Read through the Mrml tree and create all the variables 
    # and vtk entities for the fiducial/points nodes
    ############################################################
    
    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
 
    #reset all data
    FiducialsResetVariables
    set readOldNodesForCompatibility 0
    set gui 0

    # the next line is for the Fiducials->Display panel where all lists have a 
    # corresponding button with attributes when you right click
    
    # the "removeFromDisplayList" variable holds all the names of 
    # the lists that have a button on the Fiducials-> Display panel 
    # before the Mrml update
    # Since we are reading through the new updated Mrml tree, 
    # we will delete from the "removeFromDisplayList" all the lists that
    # still exist and therefore keep their button
    # The remaining lists in the "removeFromDisplayList" will be deleted from
    # the display

    set Fiducials(removeDisplayList) $Fiducials(displayList)
    
    while { $item != "" } {
    
    if { [$item GetClassName] == "vtkMrmlFiducialsNode"} {
    set fid [$item GetID]
    # get its name
        # if there is no name, give it one
        if {[$item GetName] == ""} {
        $item SetName "Fiducials$fid"
        }
        set name [$item GetName]

        lappend Fiducials(listOfNames) $name
        lappend Fiducials(listOfIds) $fid
    # reset/create variables for that list
        set Fiducials($fid,name) $name
        set Fiducials($name,fid) $fid
        set Fiducials($fid,pointIdList) ""
        set Fiducials($fid,selectedPointIdList) ""
        if {[info exists Fiducials($fid,oldSelectedPointIdList)] == 0 } {
        set Fiducials($fid,oldSelectedPointIdList) ""
        }
    set Fiducials($fid,pointsExist) 0
    


    # get type and options to create the right type of list
    set type [$item GetType]
    set symbolSize [$item GetSymbolSize]
    set Fiducials($fid,scale) $symbolSize
    if {$type == "endoscopic"} {
        $item SetTextSize 0
    }
    set textSize [$item GetTextSize]
    set Fiducials($fid,textScale) $textSize
    set visibility [$item GetVisibility]
    set Fiducials($fid,visibility) $visibility
    FiducialsVTKCreateFiducialsList $fid $type $symbolSize $textSize $visibility
    }
    if { [$item GetClassName] == "vtkMrmlPointNode" } {
        set pid [$item GetID]
        
        lappend Fiducials($fid,pointIdList) $pid
    #set its index based on its position in the list
    Point($pid,node) SetIndex [lsearch $Fiducials($fid,pointIdList) $pid]
    Point($pid,node) SetName [concat $Fiducials($fid,name) [Point($pid,node) GetIndex]]
        FiducialsVTKCreatePoint $fid $pid $visibility
        set Fiducials($fid,pointsExist) 1
    }
    if { [$item GetClassName] == "vtkMrmlEndFiducialsNode" } {
        set efid [$item GetID]
        # if the Mrml ID is not in the list already, then this
        # a new Fiducials Node/EndNode pair
    
    # update the modified point List for all the existing Fiducials Node
    if { $Fiducials($fid,pointsExist) ==  1} { 
        FiducialsVTKUpdatePoints $fid $symbolSize $textSize
    }
    # if this is a new list and it doesn't exist in Fiducials->Display, then
    # create its button and attributes
    if { [lsearch $Fiducials(displayList) $fid] == -1 } { 
        set gui [expr $gui + [FiducialsCreateGUI $Fiducials(fScrolledGUI) $fid]]
    } else {
    # otherwise the button for that list exists already so remove it 
    # from the "to be deleted list"
        set index [lsearch $Fiducials(removeDisplayList) $fid]
        if {$index != -1} {
        set Fiducials(removeDisplayList) [lreplace $Fiducials(removeDisplayList) $index $index]
    }
    }
    # callback in case any module wants to know what list of fiducials 
    # (and its type) was just read in the MRML tree
    # see the endoscopic module for examples

        foreach m $Module(idList) {
        if {[info exists Module($m,fiducialsCallback)] == 1} {
        if {$Module(verbose) == 1} {puts "Fiducials Callback: $m"}
        $Module($m,fiducialsCallback) $type $fid $Fiducials($fid,pointIdList)
        }
    }   
    }

    # BACKWARD COMPATIBILITY for old files that still use the 
    # Path/Landmark Mrml nodes (the new ones use the Fiducials/Point nodes)

    if { [$item GetClassName] == "vtkMrmlPathNode"} {
    set fid [[MainMrmlAddNode Fiducials] GetID] 
    set efid [[MainMrmlAddNode EndFiducials] GetID] 
    Fiducials($fid,node) SetName "savedPath"
    Fiducials($fid,node) SetType "endoscopic"
    MainMrmlDeleteNodeDuringUpdate Path [$item GetID]
    } elseif { [$item GetClassName] == "vtkMrmlLandmarkNode"} {
    set pid [[MainMrmlInsertBeforeNode EndFiducials($efid,node) Point] GetID]
    # set its world coordinates    
    eval Point($pid,node) SetXYZ [$item GetXYZ]
    eval Point($pid,node) SetFXYZ [$item GetFXYZ]
    Point($pid,node) SetIndex [$item GetPathPosition]
    Point($pid,node) SetName [concat "savedPath" [$item GetPathPosition]]
    MainMrmlDeleteNodeDuringUpdate Landmark [$item GetID]
    } elseif { [$item GetClassName] == "vtkMrmlEndPathNode" } {
    MainMrmlDeleteNodeDuringUpdate EndPath [$item GetID]
    set readOldNodesForCompatibility 1
    }
    
    set item [Mrml(dataTree) GetNextItem]
    
}

Render3D
 
 ##################################################
# UPDATE ACTIVE LISTS
# Check to see if the active list still exists
# and tell other modules what list is active
##################################################

# if the list is in the listOfNames, then the active list before the
# UpdateMRML still exists, and it stays active

if { [lsearch $Fiducials(listOfNames) $Fiducials(activeList) ] > -1 } {
    set name $Fiducials(activeList)
    set id $Fiducials($name,fid)
    set type [Fiducials($id,node) GetType]
    # callback in case any module wants to know the name of the active list    
    foreach m $Module(idList) {
    if {[info exists Module($m,fiducialsActivatedListCallback)] == 1} {
        if {$Module(verbose) == 1} {puts "Fiducials Activated List Callback: $m"}
        $Module($m,fiducialsActivatedListCallback)  $type $name $id
    }
    }
   
} else {
# if the list that was active before the UpdateMRML does not exist anymore, 
# then make the active list the "NONE" list
    FiducialsSetActiveList "NONE"
}

##################################################
# Update the Fiducials->Display panel
##################################################

# Remove the buttons on the Fiducials->Display panel
# for the fiducials not on the list
foreach i $Fiducials(removeDisplayList) {
    FiducialsDeleteGUI $Fiducials(fScrolledGUI) $i
}

# Tell the Fiducials->Display scrollbar to update if the gui height changed
if {$gui > 0} {
    FiducialsConfigScrolledGUI $Fiducials(canvasScrolledGUI) $Fiducials(fScrolledGUI)
}
    
##################################################
# Update all the Fiducials menus 
##################################################

    # Form the menus with all mrml fiducials plus the defaults that are not saved in mrml
    #--------------------------------------------------------
   
    set index 0
    foreach m $Fiducials(mActiveList) {
    # get the corresponding scroll
    set scroll [lindex $Fiducials(scrollActiveList) $index]
    set mb [lindex $Fiducials(mbActiveList) $index]
    $m delete 0 end
    foreach v $Fiducials(idList) {
        $m add command -label [Fiducials($v,node) GetName] \
            -command "FiducialsSetActiveList [Fiducials($v,node) GetName] $mb $scroll"
    }
    foreach d $Fiducials(defaultNames) {
        # if this default name doesn't exist in the list of fiducials in the mrml tree
        if {[lsearch $Fiducials(listOfNames) $d] == -1} {
        $m add command -label $d \
            -command "FiducialsSetActiveList $d $mb $scroll"
        }
    }
    incr index
    }
    # re-write the scrolls
    set counter 0
    foreach scroll $Fiducials(scrollActiveList) {
    $scroll delete 0 end
    # get the current name to use 
    set menu [lindex $Fiducials(mbActiveList) $counter]    
    set name [$menu cget -text]
    # if the name is valid
    if {[lsearch $Fiducials(listOfNames) $name] != -1} {
        # rewrite the list of points
        foreach pid $Fiducials($Fiducials($name,fid),pointIdList) {
        $scroll insert end "[Point($pid,node) GetName] : [Point($pid,node) GetXYZ]"
        # if it is selected, tell the scroll
        if {[lsearch $Fiducials($Fiducials($name,fid),selectedPointIdList) $pid] != -1} {
            set index [lsearch $Fiducials($fid,pointIdList) $pid]
          
            $scroll selection set $index $index
        }
        }
    } else {
        # if the name is not valid, just set the text to None
        $menu configure -text "None"
    }
    incr counter
}

#################################################################
# Tell the user if their file still has old nodes and give them 
# the option to update their files
#################################################################
if {$readOldNodesForCompatibility == 1} {

    # tell the user to save the file
    tk_messageBox -message "The file read uses a deprecated version of the endoscopic path. 
The current data was updated to use the new version. 
Please save the scene and use that new file instead to not get this message again."
    MainUpdateMRML
}

# end callback in case any module wants to know that Fiducials 
   # are done being updated 
    foreach m $Module(idList) {
    if {[info exists Module($m,fiducialsEndUpdateMRMLCallback)] == 1} {
        if {$Module(verbose) == 1} {puts "Fiducials End Callback: $m"}
        $Module($m,fiducialsEndUpdateMRMLCallback)  
    }
    }

}


#-------------------------------------------------------------------------------
# .PROC FiducialsResetVariables
# Reset all the tcl and vtk variables 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsResetVariables {} {

    global Fiducials Module
    # go through the list of existing fiducial list and clear them
    foreach id $Fiducials(listOfIds) {
    
    foreach pid $Fiducials($id,pointIdList) {
        foreach r $Fiducials(renList) {
        $r RemoveActor Point($pid,follower,$r)
        Point($pid,follower,$r) Delete
        }
        Point($pid,mapper) Delete
        Point($pid,text) Delete
    }
    
    foreach r $Fiducials(renList) {
        $r RemoveActor Fiducials($id,actor,$r)
        Fiducials($id,actor,$r) Delete 
    }
    Fiducials($id,mapper) Delete 
    Fiducials($id,glyphs) Delete
    Fiducials($id,symbolXform) Delete
    Fiducials($id,XformFilter) Delete
    Fiducials($id,points) Delete 
    Fiducials($id,scalars) Delete 
    Fiducials($id,xform) Delete 
    Fiducials($id,pointsPD) Delete 
    Point($id,textXform) Delete
    set Fiducials($id,pointIdList) ""
    set Fiducials($id,oldSelectedPointIdList) $Fiducials($id,selectedPointIdList) 
    set Fiducials($id,SelectedPointIdList) ""
    }
    set Fiducials(listOfIds) ""
    set Fiducials(listOfNames) ""
}



####################################################################
#
#
#        USER OPERATIONS THAT CHANGES THE STATE OF
#        FIDUCIALS IN MRML
#
#
####################################################################



##################### CREATION ####################################



#-------------------------------------------------------------------------------
# .PROC FiducialsCreateFiducialsList
# Create a new Fiducials/EndFiducials nodes with that name that will hold a set of points
# If a list with that name exists already, return 0
# If the new Fiducials/EndFiducials pair is created, return 1  
# .ARGS 
#  str name the name of the new List
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreateFiducialsList {type name {textSize ""} {symbolSize ""}} {
    global Fiducials Point
    
    # search in the existing lists to see if one already exists with that name
    if { [lsearch $Fiducials(listOfNames) $name] == -1 } {
    
    set fid [[MainMrmlAddNode Fiducials] GetID] 
    
    Fiducials($fid,node) SetName $name
    Fiducials($fid,node) SetType $type
    #Fiducials($fid,node) SetTextSize $textSize
    #Fiducials($fid,node) SetSymbolSize $SymbolSize
    MainMrmlAddNode EndFiducials

    MainUpdateMRML
    Render3D
    
    return $fid
    } else {
    return 0
    }
}


#-------------------------------------------------------------------------------
# .PROC FiducialsCreatePointFromWorldXYZ
#  Create a Point at the xyz location for the Fiducials list that is currently active and add it to the MRML tree (but does not call UpdateMRML)
#
# .ARGS
#       float x the x world coordinate of the new point
#       float y the y world coordinate of the new point
#       float z the z world coordinate of the new point
#       str name (optional) name of that new point
#       str listName (optional) the name of the Fiducials list you want to add this point to. If a list with that name doesn't exist, it is created automatically.
# 
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreatePointFromWorldXYZ {type x y z  {listName ""} {name ""} } {


    global Fiducials Point Module Select

    # if the user specified a list, use that name
    # otherwise, if the user specified a default list for their module/tab, 
    # use that name
    # otherwise, use the active list

    
    if {[info exists Select(actor)] != 0} {
    set actor $Select(actor)
    set cellId $Select(cellId)
    } else {
    set actor ""
    set cellId ""
    }

    if {$listName != ""} {
    # check that the name exists, if not, create new list
    if { [lsearch $Fiducials(listOfNames) $listName] == -1 } {
        FiducialsCreateFiducialsList $type $listName
        FiducialsSetActiveList $listName
    }
    } else {
    
    set module $Module(activeID) 
    set row $Module($module,row) 
    set tab $Module($module,$row,tab) 
    
    if { [info exists Fiducials($module,$tab,defaultList)] == 1 } {
        set listName $Fiducials($module,$tab,defaultList)
        # check that the name exists, if not, create new list
        if { [lsearch $Fiducials(listOfNames) $listName] == -1 } {
        FiducialsCreateFiducialsList $type $listName
        }
        FiducialsSetActiveList $listName
        
    }  else {
        if { $Fiducials(activeList) == "NONE" } {
        FiducialsCreateFiducialsList $type "default"
        FiducialsSetActiveList "default"
        } else {
        # if the active list string is not empty, but it doesn't exist, create it (in Mrml)
        if {[lsearch $Fiducials(listOfNames) $Fiducials(activeList)] == -1} {
            FiducialsCreateFiducialsList $type $Fiducials(activeList)
        }
        }
    }
    }
    
    # now use the id of the active list 
    set fid $Fiducials($Fiducials(activeList),fid)

    # find out its position in the list
    set index [llength $Fiducials($fid,pointIdList)]
    
    set pid [[MainMrmlInsertBeforeNode EndFiducials($fid,node) Point] GetID]
    # set its world coordinates

    Point($pid,node) SetXYZ $x $y $z
    Point($pid,node) SetIndex $index
    Point($pid,node) SetName [concat $Fiducials($fid,name) $index]
    
   # calculate FXYZ
   # if the actor and cell Id is not empty, get the normal of that cell
   if {$actor != ""} {
       set normals [[[[$actor GetMapper] GetInput] GetPointData] GetNormals]
       if {$normals != ""} {
       set cell [[[$actor GetMapper] GetInput] GetCell $cellId]
       set pointIds [$cell GetPointIds]
       
       # average the normals
       set count 0
       set sumX 0
       set sumY 0
       set sumZ 0
       set num [$pointIds GetNumberOfIds]
       while {$num > 0} {
           set num [expr $num - 1]
           incr count
           set id [$pointIds GetId $num]
          # set normal [$normals GetNormal $id]
           set normal [$normals GetTuple3 $id]

           set sumX [expr $sumX + [lindex $normal 0]]
           set sumY [expr $sumY + [lindex $normal 1]]
           set sumZ [expr $sumZ + [lindex $normal 2]]
           
       }
       # now average
       set avSumX [expr $sumX/$count]
       set avSumY [expr $sumY/$count]
       set avSumZ [expr $sumZ/$count]
       
       # set the camera position to be a distance of 10 units in the direction of the normal from the picked point
       
       set fx [expr $x + 30 * $avSumX]
       set fy [expr $y + 30 * $avSumY]
       set fz [expr $z + (30 * $avSumZ)]
       Point($pid,node) SetFXYZ $fx $fy $fz
       }
   }
   
   
   # callback for modules who wish to know a point was created
   foreach m $Module(idList) {
       if {[info exists Module($m,fiducialsPointCreatedCallback)] == 1} {
       if {$Module(verbose) == 1} {puts "Fiducials Point Created Callback: $m"}
       $Module($m,fiducialsPointCreatedCallback) $type $fid $pid
       }
   }
    return $pid
}




############################## DELETION ################################



#-------------------------------------------------------------------------------
# .PROC FiducialsDeletePoint
# Delete from Mrml/vtk the selected Point
# .ARGS 
#       int fid the Mrml id of the Fiducial list that contains the point
#       int pid the Mrml id of the Point
# .END
#-------------------------------------------------------------------------------
proc FiducialsDeletePoint {fid pid} {
     
    global Fiducials Point
    # first check if the ID of the Point to be deleted is in the selected 
    # list and if so, delete it
    set index [lsearch $Fiducials($fid,selectedPointIdList) $pid]
    if { $index != -1 } {
    # remove the id from the list
    set Fiducials($fid,selectedPointIdList) [lreplace $Fiducials($fid,selectedPointIdList) $index $index]
    }
    
    # delete from Mrml
    MainMrmlDeleteNode Point $pid
    MainUpdateMRML
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC FiducialsDeleteFromPicker
# If an existing Fiducial point matches the actor and cellId, then it is deleted and the Mrml Tree is updated  
#
# .ARGS 
#       str actor a vtkActor
#       int cellId ID of the selected cell in the actor
# .END
#-------------------------------------------------------------------------------
proc FiducialsDeleteFromPicker {actor cellId} {
    global Fiducials Point Module
    
    foreach fid $Fiducials(idList) {
    foreach r $Fiducials(renList) {
        if { $actor == "Fiducials($fid,actor,$r)" } {
        
        set pid [FiducialsPointIdFromGlyphCellId $fid $cellId]
        FiducialsDeletePoint $fid $pid
        return 1
        }
    }
    }
    return 0
}


#-------------------------------------------------------------------------------
# .PROC FiducialsActiveDeleteList
# Delete from Mrml/vtk the whole active list
# .ARGS 
#       
# .END
#-------------------------------------------------------------------------------
proc FiducialsDeleteActiveList {} {
    global Fiducials
    if {$Fiducials(activeList) == "NONE"} {
    # do nothing 
    return
    } else {
    FiducialsDeleteList $Fiducials(activeList)
    }
    
}
#-------------------------------------------------------------------------------
# .PROC FiducialsDeleteList
# Delete from Mrml/vtk the whole list
# .ARGS 
#       int fid the Mrml id of the Fiducial list to delete
#       
# .END
#-------------------------------------------------------------------------------
proc FiducialsDeleteList {name} {
    
    global Fiducials Point
    
    if {$name == "NONE"} {
    # do nothing 
    return
    }
    if { $Fiducials(activeList) == $name } {
    set Fiducials(activeList) "NONE"
    }
    
    set fid $Fiducials($name,fid)

    foreach pid $Fiducials($fid,pointIdList) {
    # delete from Mrml
    MainMrmlDeleteNode Point $pid
    }
    MainUpdateMRML
    MainMrmlDeleteNode EndFiducials $fid
    MainMrmlDeleteNode Fiducials $fid
    
    MainUpdateMRML
    Render3D
}

########################### VISIBILITY #####################################

#-------------------------------------------------------------------------------
# .PROC FiducialsSetFiducialsVisibility
# this procedure sets the visibility on the given screen for a set of fiducials 
# .ARGS 
#       str rendererName 
#       str name  name of the list to set visible/invisible
#       int visibility 1 makes it visible 0 makes it invisible
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetFiducialsVisibility {name {visibility ""} {rendererName ""}} {
    global Fiducials Module

    if {$rendererName == ""} {
    set rendererName "viewRen"
    }
   
    if {$name == "ALL"} {
    set visibility 1 
    set name $Fiducials(listOfNames)
    } elseif {$name == "NONE"} {
    set visibility 0 
    set name $Fiducials(listOfNames)
    } else {
    set fid $Fiducials($name,fid)
    if {$visibility == ""} {
        set visibility $Fiducials($fid,visibility)
    }
     }
     
    foreach l $name {
    foreach ren $rendererName {
        if {[lsearch $Fiducials(listOfNames) $l] != -1} {
        set fid $Fiducials($l,fid)
            Fiducials($fid,actor,$ren) SetVisibility $visibility
        Fiducials($fid,node) SetVisibility $visibility
            # go through the list of followers as well
            foreach pid $Fiducials($fid,pointIdList) {
            Point($pid,follower,$ren) SetVisibility $visibility
            }
            Render3D
        
        }
    }
    }
}
    


############################# SELECTION ##############################3

#-------------------------------------------------------------------------------
# .PROC FiducialsSetActiveList
# Set the active list of that widget to be the list of Fiducials with the name 
# given as an argument and update the display of the scroll and menu given as 
# argument (that active list doesn't necessarily have to exist in Mrml already,
# it will be created if it is not in the Mrml tree next time a point gets added
# to the list). If no menu or scroll are specified, then all fiducial scrolls that exist are updated.
# .ARGS 
#      str name name of the list that becomes active
#      str menu (optional) the menu that needs to be updated to show the new name
#      str scroll (optional) the scroll that needs to be updated to show the points of the new list
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetActiveList {name {menu ""} {scroll ""}} {
    
    global Fiducials Point Module
    
    set Fiducials(activeList) $name
    if { $menu == "" } {
    foreach m $Fiducials(mbActiveList) {
        $m config -text $name
    } 
    } else {
    $menu config -text $name
    }

    # change the content of the selection box to display only the points
    # that belong to that list

    # clear the scroll text

    if {$scroll == ""} {
    foreach s $Fiducials(scrollActiveList) {
        $s delete 0 end
        foreach pid [FiducialsGetPointIdListFromName $name] {
        $s insert end "[Point($pid,node) GetName] : [Point($pid,node) GetXYZ]"
        }
    }
    } else {


    $scroll delete 0 end
    
    foreach pid [FiducialsGetPointIdListFromName $name] {
        $scroll insert end "[Point($pid,node) GetName] : [Point($pid,node) GetXYZ]"
        if {[info exists Fiducials($name,fid)] == 1} {
        set fid $Fiducials($name,fid)
        # if it is selected, tell the scroll
        if {[lsearch $Fiducials($fid,selectedPointIdList) $pid] != -1} {
            set index [lsearch $Fiducials($fid,pointIdList) $pid]
            $scroll selection set $index $index
        }
        }    
    }
    }
}


proc FiducialsSelectionUpdate {fid pid on} {
    
    global Fiducials Module

    ### ON CASE #####
    if {$on } {
    set index [lsearch $Fiducials($fid,selectedPointIdList) $pid]
    if { $index == -1} {
        lappend Fiducials($fid,selectedPointIdList) $pid
        # tell procedure who want to know about it
        # callback 
        foreach m $Module(idList) {
        if {[info exists Module($m,fiducialsPointSelectedCallback)] == 1} {
            if {$Module(verbose) == 1} {puts "Fiducials Point Selected Callback: $m"}
            $Module($m,fiducialsPointSelectedCallback) $fid $pid
        }
        }
        
    } else {
        # if it is already selected, do nothing
        return
    }
    }
    
    if {!$on} {
    
    ### OFF CASE ###
    set index [lsearch $Fiducials($fid,selectedPointIdList) $pid]
    if { $index != -1} {
        # remove the id from the list
        set Fiducials($fid,selectedPointIdList) [lreplace $Fiducials($fid,selectedPointIdList) $index $index]
    } else {
        # if it is already deselected, do nothing
        return 
    }
    }
    # update all the scrollboxes
    
    set counter 0
    foreach menu $Fiducials(mbActiveList) {
    # get the corresponding scrollbox
    set scroll [lindex $Fiducials(scrollActiveList) $counter]
    if {[$menu cget -text] == $Fiducials($fid,name)} {
            # clear everything
            $scroll selection clear 0 end
            #re-color the entries
            foreach pid $Fiducials($fid,selectedPointIdList) {
                set sid [lsearch $Fiducials($fid,pointIdList) $pid]
                $scroll selection set $sid $sid
            }
            incr counter
    }
    }
    
    FiducialsUpdateSelectionForActor $fid
}

#-------------------------------------------------------------------------------
# .PROC FiducialsSelectionFromPicker
#  If an existing Fiducial point matches the actor and cellId, then it selects it or unselects it, depending on its current state
#  
# .ARGS 
#       str actor a vtkActor
#       int cellId ID of the selected cell in the actor
# .END
#-------------------------------------------------------------------------------
proc FiducialsSelectionFromPicker {actor cellId} {
    global Fiducials Point Module
    
    foreach fid $Fiducials(idList) {
    foreach r $Fiducials(renList) {
        if { $actor == "Fiducials($fid,actor,$r)" } {
        set pid [FiducialsPointIdFromGlyphCellId $fid $cellId]
        
        set index [lsearch $Fiducials($fid,selectedPointIdList) $pid]
        if { $index != -1} {
        # if it is already selected, it needs to unselected
        FiducialsSelectionUpdate $fid $pid 0
        } else {
        FiducialsSelectionUpdate $fid $pid 1
        }
        return 1
    }
    }
}
return 0
}
        

#-------------------------------------------------------------------------------
# .PROC FiducialsSelectionFromScroll
# 
# given a scroll box and a menu with a selected Fiducials list, update the list of selected/unselected Points to match the selections of the user in the scrollbox. The scrollbox can have multiple selections.
#  
#
# .ARGS 
#       str actor a vtkActor
#       int cellId ID of the selected cell in the actor
# .END
#-------------------------------------------------------------------------------
proc FiducialsSelectionFromScroll {menu scroll} {
    global Fiducials Module

    
    # get the active list from the menu
    set name [$menu cget -text]
    
    if { $name != "None" } {
    # get the id of the active list for that menu
    set fid $Fiducials($name,fid)
    # get the list of all selections
    set idList [$scroll curselection]
    
    # find the matching point id and
    # update the selection lists

    # there is a problem here when we switch to another panel and the selections have changed because tcl sucks

    set Fiducials($fid,selectedPointIdList) ""
    foreach id $idList {
        set pid [lindex $Fiducials($fid,pointIdList) $id]
        lappend Fiducials($fid,selectedPointIdList) $pid
    # tell procedure who want to know about it
    # callback 
    foreach m $Module(idList) {
        if {[info exists Module($m,fiducialsPointSelectedCallback)] == 1} {
        if {$Module(verbose) == 1} {puts "Fiducials Start Callback: $m"}
        $Module($m,fiducialsPointSelectedCallback) $fid $pid
        }
    }
        
    }
    
    # now update the actors
    FiducialsUpdateSelectionForActor $fid
}
}


#-------------------------------------------------------------------------------
# .PROC FiducialsUpdateSelectionForActor
# 
# Update the color of the glyphs (points) for a given Fiducials actor, based
# on the current selection list  
#
# .ARGS 
#       int fid Mrml id of the Fiducial list to be updated
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdateSelectionForActor {fid} {
    global Fiducials Module
    
    foreach pid $Fiducials($fid,pointIdList) {
    # if the point is selected
    if {[lsearch $Fiducials($fid,selectedPointIdList) $pid] != -1} { 
    
        # color the point to show it is selected
        Fiducials($fid,scalars) SetTuple1 [FiducialsScalarIdFromPointId $fid $pid] 1
        # color the text
        foreach r $Fiducials(renList) {
        eval [Point($pid,follower,$r) GetProperty] SetColor $Fiducials(textSelColor)
        }
        # if it is not selected
    } else {
        # color the point the default color
        Fiducials($fid,scalars) SetTuple1 [FiducialsScalarIdFromPointId $fid $pid] 0
        # uncolor the text
        foreach r $Fiducials(renList) {
        eval [Point($pid,follower,$r) GetProperty] SetColor $Fiducials(textColor)
        }
    }
    }
    
    Fiducials($fid,pointsPD) Modified
    Render3D
}



###############################################################################
#
#
#                   HELPER METHODS
#
#
##############################################################################


#-------------------------------------------------------------------------------
# .PROC FiducialsPointIdFromGlyphCellId
#
# Returns the Point Id that corresponds to the cell id of a picked Fiducials vtk actor. This is a convenient way to know which glyph (point) was picked since a Fiducials actor can have many glyphs (points).
#
# .ARGS 
#       int fid the Mrml id of the Fiducial list that contains the point
#       int cid the vtk cell Id
# .END
#-------------------------------------------------------------------------------
proc FiducialsPointIdFromGlyphCellId { fid cid } {
    global Fiducials Point
    
    # it's either a symbol or a sphere
    set num [ [Fiducials($fid,glyphs) GetSource 0]  GetNumberOfCells]
    
    set vtkId [expr $cid/$num]
    return [lindex $Fiducials($fid,pointIdList) $vtkId]
}


#-------------------------------------------------------------------------------
# .PROC FiducialsScalarIdFromPointId
#
#  Return the vtk scalar ID that corresponds to that point ID
# .ARGS 
#       int pid Point ID
# .END
#-------------------------------------------------------------------------------
proc FiducialsScalarIdFromPointId {fid pid } {
    global Fiducials Point

    # returns the index of the Point with the corresponding pid 
    # (its position in the list of pointIdList)
    # This works because scalars are organized like the list of pointIdList
    # so if Point with id 4 is in 2nd position in the list, the corresponding 
    # scalar for that point is also in 2nd position in the list of scalars 
    return [lsearch $Fiducials($fid,pointIdList) $pid]
}



#############################################################################
#
#
#
#                   USEFUL GETTER PROCEDURES FOR OTHER MODULES              #
#
#
#
#############################################################################

#-------------------------------------------------------------------------------
# .PROC FiducialsGetAllNodesFromList
# return the mrml Point and EndFiducials nodes belonging to that Fiducials list
# (used in DataCutNode->DataGetChildrenSelectedNode)
# .ARGS 
#       str name the name of the Fiducial list 
#       
# .END
#-------------------------------------------------------------------------------
proc FiducialsGetAllNodesFromList {name} {
    
    global Fiducials Point Mrml
    
    if {$name == "NONE"} {
    # do nothing 
    return
    }
    if { $Fiducials(activeList) == $name } {
    set Fiducials(activeList) "NONE"
    }
    
    set fid $Fiducials($name,fid)
    
    set list ""
    foreach pid $Fiducials($fid,pointIdList) {
    lappend list Point($pid,node)
    }

    lappend list EndFiducials($fid,node)
    return $list 
}



#-------------------------------------------------------------------------------
# .PROC FiducialsAddActiveListFrame
#  given a frame, this procedure packs into it an fiducials list drop down menu
#  and a scrollbox that contains all the fiducial points of the active list
#  This are updated automatically when a new list/point are created/selected
# .ARGS
#      str frame tk frame in which to pack the Fiducials panel
#      int scrollHeight height of the scrollbar, a good range is 5<->15
#      int scrollWidth width if the scrollbar, a good range is 5<->15
#      list defaultNames (optional) the name of the Fiducial lists you would like to add to the drop down menu 
# .END
#-------------------------------------------------------------------------------
proc FiducialsAddActiveListFrame {frame scrollHeight scrollWidth {defaultNames ""}} {
    global Fiducials Gui
    
    foreach subframe "how menu scroll" {
    frame $frame.f$subframe -bg $Gui(activeWorkspace)
    pack $frame.f$subframe -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # frame->How frame
    #-------------------------------------------
     set f $frame.fhow
    
    
    eval {button $f.bhow -text "How do I create Fiducials?"} $Gui(WBA)
    TooltipAdd $f.bhow "$Fiducials(howto)"
    
    eval {button $f.bdel -text "Delete Active List" -command "FiducialsDeleteActiveList"} $Gui(WBA)
    pack $f.bhow $f.bdel -side top
    TooltipAdd $f.bdel "Deletes all the points of the active list"
    
    #-------------------------------------------
    # frame->Menu frame
    #-------------------------------------------
    set f $frame.fmenu
    
    eval {label $f.lActive -text "Fiducials Lists: "} $Gui(WLA)\
        {-bg $Gui(inactiveWorkspace)}
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 
    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Fiducials(mbActiveList) $f.mbActive
    lappend Fiducials(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # frame->ScrollBox frame
    #-------------------------------------------
    set f $frame.fscroll
    
    # Create and Append widgets to list that gets refreshed during UpdateMRML
    set scroll [ScrolledListbox $f.list 1 1 -height $scrollHeight -width $scrollWidth -selectforeground red -selectmode multiple]
    
    bind $scroll <ButtonRelease-1> "FiducialsSelectionFromScroll $frame.fmenu.mbActive $scroll" 
    

    lappend Fiducials(scrollActiveList) $scroll
    pack $f.list -side top

    # if there any default names specified, add them to the list
    foreach d $defaultNames {
    $frame.fmenu.mbActive.m add command -label $d \
        -command "FiducialsSetActiveList $d $frame.fmenu.mbActive $scroll"
    lappend Fiducials(defaultNames) $d
    }

} 



#-------------------------------------------------------------------------------
# .PROC FiducialsGetPointCoordinates
#  This procedure returns the xyz coordinates of the Point with Mrml id pid
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsGetPointCoordinates { pid } {
    global Fiducials Point
    
    return [Point($pid,node) GetXYZ]
}


#-------------------------------------------------------------------------------
# .PROC FiducialsWorldPointXYZ
#
#  Created by Peter Everett. Returns the xyz coordinates for the point with id pid in the list with id fid and deals with transformations. We don't support transformations of Fiducials, so use FiducialsGetPointCoordinates instead. When we support transformations of Fiducials, FiducialsGetPointCoordinates should probably be changed to take the transformations into account, and we should get rid of this method.
#   
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsWorldPointXYZ { fid pid } {
    global Fiducials Point

    Fiducials(tmpXform) SetMatrix Fiducials($fid,xform)
    #eval Fiducials(tmpXform) SetPoint [Point($pid,node) GetXYZ] 1
    #set xyz [Fiducials(tmpXform) GetPoint]
    eval Fiducials(tmpXform) TransformPoint $xyz
    return $xyz
}




#-------------------------------------------------------------------------------
# .PROC FiducialsGetPointIdListFromName
# Return the MRML ID of the Fiducials Points in the list with that name
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsGetPointIdListFromName { name } {
    global Fiducials Point
    if { [lsearch $Fiducials(listOfNames) $name] != -1 } {
    set fid $Fiducials($name,fid)
    return $Fiducials($fid,pointIdList) 
    } else {
    return ""
    }
}


#-------------------------------------------------------------------------------
# .PROC FiducialsGetSelectedPointIdListFromName
# Return the MRML ID of the selected Fiducials Points in the list with that name
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsGetSelectedPointIdListFromName { name } {
    global Fiducials Point
    if { [lsearch $Fiducials(listOfNames) $name] != -1 } {
    set fid $Fiducials($name,fid)
    return $Fiducials($fid,selectedPointIdList) 
    } else {
    return ""
    }
}


#-------------------------------------------------------------------------------
# .PROC FiducialsGetAllSelectedPointIdList
# Return the Id List of all the selected Points, regardless what list they belong to.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsGetAllSelectedPointIdList {} {
    global Fiducials Point
    set list ""
    foreach name $Fiducials(listOfNames) {
    set fid $Fiducials($name,fid)
    set list [concat $list $Fiducials($fid,selectedPointIdList)] 
    }
    return $list
}



