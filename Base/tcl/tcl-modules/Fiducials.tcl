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
#   FiducialsPointIdFromGlyphCellId fid cid
#   FiducialsScalarIdFromPointId pid
#   FiducialsSelectionFromPicker actor cellId
#   FiducialsSelectionFromScroll actor cellId
#   FiducialsUpdateSelectionForActor fid
#   FiducialsDeleteFromPicker actor cellId
#   FiducialsSetFiducialsVisibility rendererName name visibility
#   FiducialsAddActiveListFrame  frame scrollHeight scrollWidth defaultNames
#   FiducialsAddActiveListFrame
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
	    {$Revision: 1.11 $} {$Date: 2002/02/06 21:01:43 $}]
    
    # Initialize module-level variables
    
    set Fiducials(eventManager) ""

    set Fiducials(scale) 5
    set Fiducials(minScale) 1
    set Fiducials(maxScale) 40
    set Fiducials(textScale) 6
    set Fiducials(textSlant) .333
    set Fiducials(textPush) 10
    set Fiducials(textColor) "0.4 1.0 1.0"
    set Fiducials(textSelColor) "1.0 0.5 0.5"
    
    # Append widgets to list that gets refreshed during UpdateMRML
    set Fiducials(mbActiveList) ""
    set Fiducials(mActiveList)  ""
    set Fiducials(scrollActiveList) ""

    set Fiducials(activeList) ""
    set Fiducials(defaultNames) ""
    # List of Fiducials node names
    set Fiducials(listOfNames) ""
    set Fiducials(listOfIds) ""
    
    # append events to the event manager    
    lappend Fiducials(eventManager) {$Gui(fSl0Win) <KeyPress-p> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval FiducialsCreatePointFromWorldXYZ $Select(xyz)} } }
    lappend Fiducials(eventManager) {$Gui(fSl1Win) <KeyPress-p> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval FiducialsCreatePointFromWorldXYZ $Select(xyz)} } }
    lappend Fiducials(eventManager) {$Gui(fSl2Win) <KeyPress-p> \
	    { if { [SelectPick2D %W %x %y] != 0 } \
	    { eval FiducialsCreatePointFromWorldXYZ $Select(xyz)} } }	
    lappend Fiducials(eventManager) {$Gui(fViewWin) <KeyPress-p> \
	    { if { [SelectPick Fiducials(picker) %W %x %y] != 0 } \
	    { eval FiducialsCreatePointFromWorldXYZ $Select(xyz)} } }	
    lappend Fiducials(eventManager) {$Gui(fViewWin) <KeyPress-q> \
	    { if { [SelectPick Fiducials(picker) %W %x %y] != 0} \
	    {FiducialsSelectionFromPicker $Select(actor) $Select(cellId)} } }
    lappend Fiducials(eventManager) {$Gui(fViewWin) <KeyPress-d> \
	    { if { [SelectPick Fiducials(picker) %W %x %y] != 0} \
	    {FiducialsDeleteFromPicker $Select(actor) $Select(cellId)}}}       
    
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
    pushEventManager $Fiducials(eventManager)
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
    popEventManager 
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

	foreach frame "Size" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Display->Size frame
	#-------------------------------------------
	set f $fDisplay.fSize

	eval {label $f.lPntSize -text "Symbol size:"} $Gui(WLA)
	eval {entry $f.ePntSize -width 3 -textvariable Fiducials(scale) } \
                $Gui(WEA)
	bind $f.ePntSize <Return> "FiducialsSetScale $Fiducials(scale)"
	eval {scale $f.sPntSize -from $Fiducials(minScale) -to $Fiducials(maxScale) \
		-variable Fiducials(scale) -command FiducialsSetScale \
		-resolution 1 -length 40 } $Gui(WSA)
	eval {label $f.lTxtSize -text "Text size:"} $Gui(WLA)
	eval {entry $f.eTxtSize -width 3 -textvariable Fiducials(textScale) } \
                $Gui(WEA)
	bind $f.eTxtSize <Return> {FiducialsSetTxtScale $Fiducials(textScale)}
	eval {scale $f.sTxtSize -from $Fiducials(minScale) -to $Fiducials(maxScale) \
		-variable Fiducials(textScale) -command FiducialsSetTxtScale \
		-resolution 1 -length 40 } $Gui(WSA)

	grid columnconfigure $f 0 -weight 0
	grid columnconfigure $f 1 -weight 0
	grid columnconfigure $f 2 -weight 1
	grid $f.lPntSize $f.ePntSize $f.sPntSize \
		-padx $Gui(pad) -pady $Gui(pad) -sticky news
	grid $f.lTxtSize $f.eTxtSize $f.sTxtSize \
		-padx $Gui(pad) -pady $Gui(pad) -sticky news

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
	bind $f.eCreateList <Return> {FiducialsCreateFiducialsList $Fiducials(newListName)}

	pack $f.lCreateList $f.eCreateList -side left -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Edit->Middle frame
	#-------------------------------------------
	set f $fEdit.fMiddle

	FiducialsAddActiveListFrame $f 15 25 "default"
	
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

	vtkTransform Fiducials(symbolXform)
	  Fiducials(symbolXform) Scale \
		$Fiducials(scale) $Fiducials(scale) $Fiducials(scale)

	vtkTransformPolyDataFilter Fiducials(symbolXformFilter)
	  Fiducials(symbolXformFilter) SetInput Fiducials(symbolPD)
	  Fiducials(symbolXformFilter) SetTransform Fiducials(symbolXform)

	vtkTransform Point(textXform)
	  Point(textXform) Translate 0 0 $Fiducials(textPush)
	  [Point(textXform) GetMatrix] SetElement 0 1 .333
	  Point(textXform) Scale $Fiducials(textScale) $Fiducials(textScale) 1

	vtkTransform Fiducials(tmpXform)
	  Fiducials(tmpXform) PostMultiply
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
proc FiducialsVTKCreateFiducialsList { id } {
    global Fiducials Mrml Module
    

    vtkPoints Fiducials($id,points)
    
    vtkScalars Fiducials($id,scalars)
    
    vtkPolyData Fiducials($id,pointsPD)
    Fiducials($id,pointsPD) SetPoints Fiducials($id,points)
    [Fiducials($id,pointsPD) GetPointData] SetScalars Fiducials($id,scalars)
    vtkGlyph3D Fiducials($id,glyphs)
    Fiducials($id,glyphs) SetSource \
	    [Fiducials(symbolXformFilter) GetOutput]
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
    foreach r $Module(Renderers) {
	vtkActor Fiducials($r,$id,actor)
	Fiducials($r,$id,actor) SetMapper Fiducials($id,mapper)
	[Fiducials($r,$id,actor) GetProperty] SetColor 1 0 0
	[Fiducials($r,$id,actor) GetProperty] SetInterpolationToFlat
	Fiducials($r,$id,actor) SetUserMatrix Fiducials($id,xform)
	
	$r AddActor Fiducials($r,$id,actor)
    }
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
proc FiducialsVTKCreatePoint { fid pid } {
    global Fiducials Point Mrml Module
    
    
    vtkVectorText Point($pid,text)
    Point($pid,text) SetText [Point($pid,node) GetName]
    vtkPolyDataMapper Point($pid,mapper)
    Point($pid,mapper) SetInput [Point($pid,text) GetOutput]
    foreach r $Module(Renderers) {
	vtkFollower Point($r,$pid,follower)
	Point($r,$pid,follower) SetMapper Point($pid,mapper)
	Point($r,$pid,follower) SetCamera [viewRen GetActiveCamera]
	Point($r,$pid,follower) SetUserMatrix [Point(textXform) GetMatrix]
	Point($r,$pid,follower) SetPickable 0
	eval [Point($r,$pid,follower) GetProperty] SetColor $Fiducials(textColor)
    
	$r AddActor Point($r,$pid,follower)
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
proc FiducialsVTKUpdatePoints {} {
    global Fiducials Point Mrml Module
    
    foreach fid $Fiducials(idList) {
	
	Mrml(dataTree) ComputeNodeTransform Fiducials($fid,node) \
		Fiducials($fid,xform)
	Fiducials(tmpXform) SetMatrix Fiducials($fid,xform)
	Fiducials($fid,points) SetNumberOfPoints 0
	Fiducials($fid,scalars) SetNumberOfScalars 0
	
	foreach pid $Fiducials($fid,pointIdList) {
	    set xyz [Point($pid,node) GetXYZ]
	    eval Fiducials($fid,points) InsertNextPoint $xyz
	    Fiducials($fid,scalars) InsertNextScalar 0		
	    eval Fiducials(tmpXform) SetPoint $xyz 1
	    set xyz [Fiducials(tmpXform) GetPoint]
	    foreach r $Module(Renderers) {
		eval Point($r,$pid,follower) SetPosition $xyz
	    }
	    Point($pid,text) SetText [Point($pid,node) GetName]
	}

	# color the selected glyphs (by setting their scalar to 1)
	if {[info exists Fiducials($fid,oldSelectedPointIdList)]} {
	    foreach pid $Fiducials($fid,pointIdList) {
		# see if that point was previously selected
		if {[lsearch $Fiducials($fid,oldSelectedPointIdList) $pid] != -1} { 
		    # color the point
		    Fiducials($fid,scalars) SetScalar [FiducialsScalarIdFromPointId $fid $pid] 1
		    # color the text
		    foreach r $Module(Renderers) {
			eval [Point($r,$pid,follower) GetProperty] SetColor $Fiducials(textSelColor)
		    }
		    # add it to the current list of selected items
		    lappend Fiducials($fid,selectedPointIdList) $pid
		}
	    }
	}
	
	Fiducials($fid,pointsPD) Modified
    }
    
    Render3D
}



#-------------------------------------------------------------------------------
# .PROC FiducialsSetTxtScale
#
# Set the scale of the Fiducials symbol
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetTxtScale { val } {
	global Fiducials Point

	set s $Fiducials(textScale)
	Point(textXform) Identity
	Point(textXform) Translate 0 0 $Fiducials(textPush)
	[Point(textXform) GetMatrix] SetElement 0 1 .333
	Point(textXform) Scale $Fiducials(textScale) $Fiducials(textScale) 1

	Render3D
	}

#-------------------------------------------------------------------------------
# .PROC FiducialsSetScale
#
# Set the scale of the Fiducials symbol
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetScale { val } {
	global Fiducials

	set s $Fiducials(scale)
	Fiducials(symbolXform) Identity
	Fiducials(symbolXform) Scale $s $s $s

	Render3D
	}


#-------------------------------------------------------------------------------
# .PROC FiducialsUpdateMRML
#
# Update the Fiducials actors and scroll textboxes based on the current Mrml Tree
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdateMRML {} {
    global Fiducials Mrml
    

    Mrml(dataTree) ComputeTransforms
    Mrml(dataTree) InitTraversal
    set item [Mrml(dataTree) GetNextItem]
    
    #reset all data
    FiducialsResetVariables
    while { $item != "" } {
	if { [$item GetClassName] == "vtkMrmlFiducialsNode" } {
	    set fid [$item GetID]
	    # get its name
	    # if there is no name, give it one
	    if {[$item GetName] == ""} {
		$item SetName "Fiducials$fid"
	    }
	    set name [$item GetName]
	    lappend Fiducials(listOfNames) $name
	    lappend Fiducials(listOfIds) $fid
	    set Fiducials($fid,name) $name
	    set Fiducials($name,fid) $fid

	    set Fiducials($fid,pointIdList) ""
	    set Fiducials($fid,selectedPointIdList) ""
	    if {[info exists Fiducials($fid,oldSelectedPointIdList)] == 0 } {
		set Fiducials($fid,oldSelectedPointIdList) ""
	    }
	}
	if { [$item GetClassName] == "vtkMrmlPointNode" } {
	    set pid [$item GetID]
	    
	    lappend Fiducials($fid,pointIdList) $pid
	    FiducialsVTKCreatePoint $fid $pid
	    
	}
	if { [$item GetClassName] == "vtkMrmlEndFiducialsNode" } {
	    set efid [$item GetID]
	    # if the Mrml ID is not in the list already, then this
	    # a new Fiducials Node/EndNode pair
	    
	    FiducialsVTKCreateFiducialsList $fid
	    
	    
	}
	set item [Mrml(dataTree) GetNextItem]
    }
    
    
    # update the modified point List for all the existing Fiducials Node
    FiducialsVTKUpdatePoints
    
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
	    foreach r $Module(Renderers) {
		$r RemoveActor Point($r,$pid,follower)
		Point($r,$pid,follower) Delete
	    }
	    Point($pid,mapper) Delete
	    Point($pid,text) Delete
	}
	
	foreach r $Module(Renderers) {
	    $r RemoveActor Fiducials($r,$id,actor)
	    Fiducials($r,$id,actor) Delete 
	}
	Fiducials($id,mapper) Delete 
	Fiducials($id,glyphs) Delete 
	Fiducials($id,points) Delete 
	Fiducials($id,scalars) Delete 
	Fiducials($id,xform) Delete 
	Fiducials($id,pointsPD) Delete 
	set Fiducials($id,pointIdList) ""
	set Fiducials($id,oldSelectedPointIdList) $Fiducials($id,selectedPointIdList) 
	set Fiducials($id,SelectedPointIdList) ""
    }
    set Fiducials(listOfIds) ""
    set Fiducials(listOfNames) ""
}


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
    
    global Fiducials Point
    
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
    #MainUpdateMRML
    Render3D
}



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
    
    set num [[Fiducials(symbolXformFilter) GetOutput] GetNumberOfCells]
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
	foreach r $Module(Renderers) {
	    if { $actor == "Fiducials($r,$fid,actor)" } {
		set pid [FiducialsPointIdFromGlyphCellId $fid $cellId]
		
		
		# if it is already selected, it needs to unselected
		set index [lsearch $Fiducials($fid,selectedPointIdList) $pid]
		if { $index != -1} {
		    
		    
		    # remove the id from the list
		    set Fiducials($fid,selectedPointIdList) [lreplace $Fiducials($fid,selectedPointIdList) $index $index]
		    
		    # else it needs to be selected     
		} else {
		    
		    lappend Fiducials($fid,selectedPointIdList) $pid
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
	    Fiducials($fid,scalars) SetScalar [FiducialsScalarIdFromPointId $fid $pid] 1
	    # color the text
	    foreach r $Module(Renderers) {
		eval [Point($r,$pid,follower) GetProperty] SetColor $Fiducials(textSelColor)
	    }
	    # if it is not selected
	} else {
	    # color the point the default color
	    Fiducials($fid,scalars) SetScalar [FiducialsScalarIdFromPointId $fid $pid] 0
	    # uncolor the text
	    foreach r $Module(Renderers) {
		eval [Point($r,$pid,follower) GetProperty] SetColor $Fiducials(textColor)
	    }
	}
    }
    
    Fiducials($fid,pointsPD) Modified
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
	foreach r $Module(Renderers) {
	    if { $actor == "Fiducials($r,$fid,actor)" } {
		
		set pid [FiducialsPointIdFromGlyphCellId $fid $cellId]
		FiducialsDeletePoint $fid $pid
		return 1
	    }
	}
    }
    return 0
}



#-------------------------------------------------------------------------------
# .PROC FiducialsSetFiducialsVisibility
# this procedure sets the visibility on the given screen for a set of fiducials 
# .ARGS 
#       str rendererName 
#       str name  name of the list to set visible/invisible
#       int visibility 1 makes it visible 0 makes it invisible
# .END
#-------------------------------------------------------------------------------
proc FiducialsSetFiducialsVisibility {rendererName name visibility} {
    global Fiducials 

    if {[lsearch $Fiducials(listOfNames) $name] != -1} {
	set fid $Fiducials($name,fid)
	Fiducials($rendererName,$fid,actor) SetVisibility $visibility
	# go through the list of followers as well
	foreach pid $Fiducials($fid,pointIdList) {
	    Point($rendererName,$pid,follower) SetVisibility $visibility
	}
	Render3D
    }
}
    





#############################################################################
#
#
#
########################## USEFUL PROCEDURES FOR OTHER MODULES ##############
#
#
#
#############################################################################



#-------------------------------------------------------------------------------
# .PROC FiducialsAddActiveListFrame 
#  given a frame, this procedure packs into it an fiducials list drop down menu
#  and a scrollbox that contains all the fiducial points of the active list
#  This are updated automatically when a new list/point are created/selected
#
# .ARGS 
#      str frame tk frame in which to pack the Fiducials panel
#      int scrollHeight height of the scrollbar, a good range is 5<->15
#      int scrollWidth width if the scrollbar, a good range is 5<->15
#      list defaultNames (optional) the name of the Fiducial lists you would like to add to the drop down menu 
# .END
#-------------------------------------------------------------------------------


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
    pack $f.bhow 
    TooltipAdd $f.bhow "$Fiducials(howto)"
    
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
# .PROC FiducialsCreateFiducialsList
# Create a new Fiducials/EndFiducials nodes with that name that will hold a set of points
# If a list with that name exists already, return 0
# If the new Fiducials/EndFiducials pair is created, return 1  
# .ARGS 
#  str name the name of the new List
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreateFiducialsList {name} {
    global Fiducials Point
    
    # search in the existing lists to see if one already exists with that name
    if { [lsearch $Fiducials(listOfNames) $name] == -1 } {
	
	set fid [[MainMrmlAddNode Fiducials] GetID] 
	
	Fiducials($fid,node) SetName $name
	
	MainMrmlAddNode EndFiducials

	MainUpdateMRML
	Render3D
	
	#FiducialsSetActiveList $name
	return 1
    } else {
	return 0
    }
}


#-------------------------------------------------------------------------------
# .PROC FiducialsCreatePointFromWorldXYZ
#  Create a Point at the xyz location for the Fiducials list that is currently active and update the MRML Tree
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
proc FiducialsCreatePointFromWorldXYZ {x y z {name ""} {listName ""}} {


    global Fiducials Point

    # if the active list is empty, then the user hasn't specified a list name 
    # yet , tell him/her and return
 

    # if the user specified a list, use that instead of active list
    
    if {$listName != ""} {
	# check that the name exists, if not, create new list
	if { [lsearch $Fiducials(listOfNames) $listName] == -1 } {
	    FiducialsCreateFiducialsList $listName
	}
	 # now use the id of that list
	set fid $Fiducials($name,fid)
    } else {
	if { $Fiducials(activeList) == "" } {
	    tk_messageBox -message "You have to pick an active list"
	    return
	}
	
	# if the active list string is not empty, but it doesn't exist, create it (in Mrml)
	if {[lsearch $Fiducials(listOfNames) $Fiducials(activeList)] == -1} {
	    FiducialsCreateFiducialsList $Fiducials(activeList)
	}
	# now use the id of the active list 
	set fid $Fiducials($Fiducials(activeList),fid)
    }
    set pid [[MainMrmlInsertBeforeNode EndFiducials($fid,node) Point] GetID]
    # set its world coordinates
    
    Point($pid,node) SetXYZ $x $y $z
    Point($pid,node) SetName PNT$pid
    MainUpdateMRML
    Render3D
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
	eval Fiducials(tmpXform) SetPoint [Point($pid,node) GetXYZ] 1
	set xyz [Fiducials(tmpXform) GetPoint]
	return $xyz
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



