#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        Fiducials.tcl
# PROCEDURES:  
#   FiducialsInit
#   FiducialsBuildVTK
#   FiducialsEnter
#   FiducialsExit
#   FiducialsBuildGUI
#   FiducialsUpdateGUI
#   FiducialsSetTxtScale
#   FiducialsSetScale
#   FiducialsCount
#   FiducialsUpdateMRML
#   FiducialsCreate
#   FiducialsCreatePoint
#   FiducialsUpdatePoints
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC FiducialsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsInit {} {
	global Fiducials Module Volume Model

	# Define Tabs
	#------------------------------------
	# Description:
	#   Each module is given a button on the Slicer's main menu.
	#   When that button is pressed a row of tabs appear, and there is a panel
	#   on the user interface for each tab.  If all the tabs do not fit on one
	#   row, then the last tab is automatically created to say "More", and 
	#   clicking it reveals a second row of tabs.
	#
	#   Define your tabs here as shown below.  The options are:
	#   
	#   row1List = list of ID's for tabs. (ID's must be unique single words)
	#   row1Name = list of Names for tabs. (Names appear on the user interface
	#              and can be non-unique with multiple words.)
	#   row1,tab = ID of initial tab
	#   row2List = an optional second row of tabs if the first row is too small
	#   row2Name = like row1
	#   row2,tab = like row1 
	#
	set m Fiducials
	set Module($m,row1List) "Help Display Edit"
	set Module($m,row1Name) "{Help} {Display} {Edit}"
	set Module($m,row1,tab) Display
	set Module($m,procVTK) FiducialsBuildVTK
	set Module($m,procEnter) FiducialsEnter
	set Module($m,procExit) FiducialsExit
	set Module($m,procMRML) FiducialsUpdateMRML

	# Define Procedures
	#------------------------------------
	# Description:
	#   The Slicer sources all *.tcl files, and then it calls the Init
	#   functions of each module, followed by the VTK functions, and finally
	#   the GUI functions. A MRML function is called whenever the MRML tree
	#   changes due to the creation/deletion of nodes.
	#   
	#   While the Init procedure is required for each module, the other 
	#   procedures are optional.  If they exist, then their name (which
	#   can be anything) is registered with a line like this:
	#
	#   set Module($m,procVTK) FiducialsBuildVTK
	#
	#   All the options are:
	#
	#   procGUI   = Build the graphical user interface
	#   procVTK   = Construct VTK objects
	#   procMRML  = Update after the MRML tree changes due to the creation
	#               of deletion of nodes.
	#   procEnter = Called when the user enters this module by clicking
	#               its button on the main menu
	#   procExit  = Called when the user leaves this module by clicking
	#               another modules button
	#   procStorePresets  = Called when the user holds down one of the Presets
	#               buttons.
	#   procRecallPresets  = Called when the user clicks one of the Presets buttons
	#               
	#   Note: if you use presets, make sure to give a preset defaults
	#   string in your init function, of the form: 
	#   set Module($m,presets) "key1='val1' key2='val2' ..."
	#   
	set Module($m,procGUI) FiducialsBuildGUI

	# Define Dependencies
	#------------------------------------
	# Description:
	#   Record any other modules that this one depends on.  This is used 
	#   to check that all necessary modules are loaded when Slicer runs.
	#   
	set Module($m,depend) ""

        # Set version info
	#------------------------------------
	# Description:
	#   Record the version number for display under Help->Version Info.
	#   The strings with the $ symbol tell CVS to automatically insert the
	#   appropriate info when the module is checked in.
	#   
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.4 $} {$Date: 2000/08/08 15:27:59 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
	set Fiducials(scale) 5
	set Fiducials(minScale) 1
	set Fiducials(maxScale) 40
	set Fiducials(textScale) 6
	set Fiducials(textSlant) .333
	set Fiducials(textPush) 10
	set Fiducials(textColor) "0.4 1.0 1.0"
	set Fiducials(textSelColor) "1.0 0.5 0.5"
}


#-------------------------------------------------------------------------------
# .PROC FiducialsBuildVTK
#
# Create the VTK structures used for displaying Fiducials.
# .END
#-------------------------------------------------------------------------------
proc FiducialsBuildVTK {} {
	global Fiducials Point

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
# .PROC FiducialsEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsEnter {} {
	Render3D
	}

#-------------------------------------------------------------------------------
# .PROC FiducialsExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsExit {} {
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
Help for Fiducials Panel
"
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
	# Edit->Size frame
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
	bind $f.eTxtSize <Return> "FiducialsSetTxtScale $Fiducials(textScale)"
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

	foreach frame "Top Bottom" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Edit->Top frame
	#-------------------------------------------
	set f $fEdit.fTop
        
#         grid $f.lEdit -padx $Gui(pad) -pady $Gui(pad)
#        grid $menubutton -sticky w

        # Add menus that list models and volumes
        DevAddSelectButton  Fiducials $f Volume1 "Ref Volume" Grid
        DevAddSelectButton  Fiducials $f Model1  "Ref Model"  Grid

	# Append these menus and buttons to lists that get refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbVolume1
	lappend Volume(mActiveList) $f.mbVolume1.m
	lappend Model(mbActiveList) $f.mbModel1
	lappend Model(mActiveList) $f.mbModel1.m

	#-------------------------------------------
	# Edit->Bottom frame
	#-------------------------------------------
	set f $fEdit.fBottom

	eval {label $f.lEdit -text "You clicked 0 times."} $Gui(WLA)
        pack $f.lEdit -side top -padx $Gui(pad) -fill x
	set Fiducials(lEdit) $f.lEdit

        # Here's a button with text "Count" that calls "FiducialsCount" when
        # pressed.
        DevAddButton $f.bCount Count FiducialsCount 

	eval {entry $f.eCount -width 5 -textvariable Fiducials(count) } \
                $Gui(WEA)

	pack $f.bCount $f.bCount $f.eCount -side left -padx $Gui(pad) -pady $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC FiducialsUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
# (Note: to do this, this proc must be this module's procMRML.  Right now,
# these buttons are being updated automatically since they have been added
# to lists updated in VolumesUpdateMRML and ModelsUpdateMRML.  So this procedure
# is not currently used.)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdateGUI {} {
	global Fiducials Volume

   DevUpdateNodeSelectButton Volume Fiducials Volume1 Volume1 DevSelectNode
   DevUpdateNodeSelectButton Model  Fiducials Model1  Model1  DevSelectNode
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
# .PROC FiducialsCount
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc FiducialsCount {} {
	global Fiducials

	incr Fiducials(count)
	$Fiducials(lEdit) config -text "You clicked the button $Fiducials(count) times"
}

#-------------------------------------------------------------------------------
# .PROC FiducialsUpdateMRML
#
# Update the Fiducials
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdateMRML {} {
	global Fiducials Mrml

	Mrml(dataTree) ComputeTransforms
	Mrml(dataTree) InitTraversal
	set item [Mrml(dataTree) GetNextItem]
	while { $item != "" } {
		if { [$item GetClassName] == "vtkMrmlFiducialsNode" } {
			set fid [$item GetID]
			set Fiducials($fid,pointIdList) ""
		}
		if { [$item GetClassName] == "vtkMrmlPointNode" } {
			set pid [$item GetID]
			lappend Fiducials($fid,pointIdList) $pid
			FiducialsCreatePoint $fid $pid
		}
		if { [$item GetClassName] == "vtkMrmlEndFiducialsNode" } {
			FiducialsCreate $fid
		}
		set item [Mrml(dataTree) GetNextItem]
	}
	FiducialsUpdatePoints
}

#-------------------------------------------------------------------------------
# .PROC FiducialsCreate
#
# Create a new set of fiducials
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreate { id } {
	global Fiducials Mrml

	if { [info command Fiducials($id,actor)] != "" } {
		return 0
	}
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

	vtkActor Fiducials($id,actor)
	  Fiducials($id,actor) SetMapper Fiducials($id,mapper)
	  [Fiducials($id,actor) GetProperty] SetColor 1 0 0
	  [Fiducials($id,actor) GetProperty] SetInterpolationToFlat
	  Fiducials($id,actor) SetUserMatrix Fiducials($id,xform)

	viewRen AddActor Fiducials($id,actor)
}

#-------------------------------------------------------------------------------
# .PROC FiducialsCreatePoint
#
# Create a point
# .END
#-------------------------------------------------------------------------------
proc FiducialsCreatePoint { fid pid } {
	global Fiducials Point Mrml

	if { [info command Point($pid,follower)] != "" } {
		return 0
	}

	vtkVectorText Point($pid,text)
	  Point($pid,text) SetText [Point($pid,node) GetName]
	vtkPolyDataMapper Point($pid,mapper)
	  Point($pid,mapper) SetInput [Point($pid,text) GetOutput]
	vtkFollower Point($pid,follower)
	  Point($pid,follower) SetMapper Point($pid,mapper)
	  Point($pid,follower) SetCamera [viewRen GetActiveCamera]
	  Point($pid,follower) SetUserMatrix [Point(textXform) GetMatrix]
	  eval [Point($pid,follower) GetProperty] SetColor $Fiducials(textColor)

	viewRen AddActor Point($pid,follower)
}

#-------------------------------------------------------------------------------
# .PROC FiducialsUpdatePoints
#
# Update the points
# .END
#-------------------------------------------------------------------------------
proc FiducialsUpdatePoints {} {
	global Fiducials Point Mrml

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
			eval Point($pid,follower) SetPosition $xyz
		}
		Fiducials($fid,pointsPD) Modified
	}
}
