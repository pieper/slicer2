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
# FILE:        TetraMesh.tcl
# PROCEDURES:  
#   TetraMeshInit
#   TetraMeshBuildGUI
#   TetraMeshEnter
#   TetraMeshExit
#   TetraMeshUpdateGUI
#   TetraMeshSetProcessType
#   TetraMeshProcessTetraMesh
#   TetraMeshFileNameEntered
#   TetraMeshGetData
#   SetModelMoveOriginMatrix n matrix
#   TetraMeshGetTransform
#   TetraMeshProcessEdges2
#   TetraMeshProcessEdges
#   TetraMeshProcessScalarField
#   TetraMeshProcessNodes
#   TetraMeshProcessCorrespond
#   TetraMeshProcessVectorField
#   TetraMeshProcessSurface
#   TetraMeshCorrespondSurfaces
#   TetraMeshCopyPolyData PolyData number
#   TetraMeshCreateModel
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module is an example for developers.  It shows how to add a module 
#  to the Slicer.  To find it when you run the Slicer, click on More->TetraMesh.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are the variables defined by this module.
# 
#  int TetraMesh(count) counts the button presses for the demo 
#  list TetraMesh(eventManager)  list of event bindings used by this module
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC TetraMeshInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshInit {} {
	global TetraMesh Module Volume Model

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
	set m TetraMesh
	set Module($m,row1List) "Help Process View"
        set Module($m,row1Name) "{Help} {Process} {View}"
	set Module($m,row1,tab) Process

#        set Module($m,row2List) "SField VField"
#        set Module($m,row2Name) "{Scalar Field} {Vector Field}"
#        set Module($m,row2,tab) SField

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
	#   set Module($m,procVTK) TetraMeshBuildVTK
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
	set Module($m,procGUI) TetraMeshBuildGUI
	set Module($m,procEnter) TetraMeshEnter
	set Module($m,procExit) TetraMeshExit
	set Module($m,procMRML) TetraMeshUpdateGUI

	# Define Dependencies
	#------------------------------------
	# Description:
	#   Record any other modules that this one depends on.  This is used 
	#   to check that all necessary modules are loaded when Slicer runs.
	#   
	set Module($m,depend) "Data"

        # Set version info
	#------------------------------------
	# Description:
	#   Record the version number for display under Help->Version Info.
	#   The strings with the $ symbol tell CVS to automatically insert the
	#   appropriate revision number and date when the module is checked in.
	#   
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.24 $} {$Date: 2001/12/27 14:47:38 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
	set TetraMesh(count) 0
	set TetraMesh(FileName) ""
	set TetraMesh(OtherMesh) ""
        set TetraMesh(modelbasename) ""
	set TetraMesh(eventManager)  ""
        set TetraMesh(DefaultDir) ""
            ## Tube Radius of edges
        set TetraMesh(TetraTubeRadius) 0.01
        set TetraMesh(ArrowScale) 9.5
        set TetraMesh(ArrowSkip) 2
        set TetraMesh(SphereScale) 9.5
        set TetraMesh(SphereSkip) 2

        # 
        #
        # The following matrix exists to move the origin
        # and orientation of a mesh to the proper origin
        # used in the slicer. See SetModelMoveOriginMatrix
        #
        #

        vtkMatrix4x4 ModelMoveOriginMatrix
}


#-------------------------------------------------------------------------------
# .PROC TetraMeshBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshBuildGUI {} {
	global Gui TetraMesh Module Volume Model View

	# A frame has already been constructed automatically for each tab.
	# A frame named "Props" can be referenced as follows:
	#   
	#     $Module(<Module name>,f<Tab name>)
	#
	# ie: $Module(TetraMesh,fProps)

	# This is a useful comment block that makes reading this easy for all:
	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Process
	#   Top
        #   Middle
        #   Alignment
        #   Select
        #   Options
        #      Surface
        #      Edges
        #      Nodes
        #      ...
        #   Bottom
        #
        #
        # View
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
The TetraMesh module allows a user to read in a Tetrahedral Mesh.  The Mesh is converted into a model in one of two ways. 

<UL>
<LI><B>Extract Surfaces:</B> The mesh is assumed to have integer scalars. One model is produced as the surface of the mesh for each scalar. Each model is a different color.

<LI><B>Extract Edges:</B> The mesh is processed and a model is produced consisting of the lines that connect each node. The lines are color coded according to the scalar value of the mesh. The colors should match those of the surfaces extracted. The mesh need not have integer scalar values.

<LI><B>Useful Comment:</B> Often, the edges of the mesh produce such a dense set of lines that it is difficult to determine useful information. When this occurs, I reccomend making two slicer parallel to each other and clip so that only the region in between the two slices is displayed. (Set Clipping to Union in the View menu). By doing this, you can create a thin region of lines that you can visually interpret. Also, you might consider turning on parallel viewing.

<LI><B>Known Problems:</B> In order to clip the resulting models, you must turn clipping off and on in the <B>Models</B> module. It is not clear if this is a general slicer error or an error in this module.

<LI><B>Advanced Users:</B> The input file need not be a tetrahedral mesh, any vtk unstructured points will do. 

</LI>
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags TetraMesh $help
	MainHelpBuildGUI TetraMesh

	#-------------------------------------------
	# Process frame
	#-------------------------------------------
	set fProcess $Module(TetraMesh,fProcess)
	set f $fProcess

	foreach frame "Top Middle Alignment Select" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

        frame $f.fOptions -bg $Gui(activeWorkspace) -height 100
        pack $f.fOptions -side top -padx 0 -pady $Gui(pad) -fill x

	foreach frame "Bottom" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Process->Top frame
	#-------------------------------------------
	set f $fProcess.fTop

	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)

        DevAddFileBrowse $f.f TetraMesh FileName "Tetrahedral Mesh:" "TetraMeshFileNameEntered" "vtk"\
                "\$TetraMesh(DefaultDir)" "Browse for a Tetrahedral Mesh"

	set f $fProcess.fTop.fBaseName

	frame $f  -bg $Gui(activeWorkspace) 
        pack $f -side top -padx 0 -pady $Gui(pad) -fill x

        DevAddLabel  $f.l "Model Name"
	eval {entry $f.eModelName -textvariable TetraMesh(modelbasename) -width 50} $Gui(WEA)
        
        pack $f.l $f.eModelName -side left -padx $Gui(pad)

	#-------------------------------------------
	# Process->Alignment frame
	#-------------------------------------------
	set f $fProcess.fAlignment

        DevAddSelectButton TetraMesh $f AlignmentVolume "Alignment Volume(Optional)" Pack

        lappend Volume(mbActiveList) $f.mbAlignmentVolume
        lappend Volume(mActiveList)  $f.mbAlignmentVolume.m

	#-------------------------------------------
	# Process->Select
	#-------------------------------------------
	set f $fProcess.fSelect

        eval {label $f.l -text " Process Type: "} $Gui(BLA)
	frame $f.f -bg $Gui(backdrop)

        # the first row and second row
        frame $f.f.1 -bg $Gui(inactiveWorkspace)
        frame $f.f.2 -bg $Gui(inactiveWorkspace)
        pack $f.f.1 $f.f.2 -side top -fill x -anchor w

        #
        # NOTE: As you want more functions, don't forget
        #       to add more rows above.
        #

        set row 1
	foreach p "Surface Edges Nodes ScalarField VectorField" {
            eval {radiobutton $f.f.$row.r$p \
			-text "$p" -command "TetraMeshSetProcessType" \
			-variable TetraMesh(ProcessType) -value $p -width 10 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.$row.r$p -side left -pady 0
            if { $p == "Nodes" } {incr row};
	}

	pack $f.l $f.f -side top -padx $Gui(pad) -fill x -anchor w


	#-------------------------------------------
	# Process->Options
	#-------------------------------------------

	set f $fProcess.fOptions

	foreach type "Surface Edges Nodes ScalarField VectorField" {
		frame $f.f${type} -bg $Gui(activeWorkspace)
		place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
		set TetraMesh(f${type}) $f.f${type}
	}

	raise $TetraMesh(fSurface)

	#-------------------------------------------
	# Process->Options->Surface
	#-------------------------------------------
        set f $fProcess.fOptions.fSurface

        DevAddLabel $f.l "Grab Surfaces of the mesh"
        pack $f.l -side left -padx $Gui(pad)

	#-------------------------------------------
	# Process->Options->Edges
	#-------------------------------------------
        set f $fProcess.fOptions.fEdges

        DevAddLabel $f.l "Grab the Edges of the mesh"
        pack $f.l -side left -padx $Gui(pad)

	#-------------------------------------------
	# Process->Options->Nodes
	#-------------------------------------------
        set ff $fProcess.fOptions.fNodes
        set  f $ff

	foreach frame "Scale Skip" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

           #-------------------------------------------
           # SField->Scale
           #-------------------------------------------
           set f $ff.fScale
   
           DevAddLabel  $f.lSphereScale "Sphere Scaling"
           eval {entry $f.eSphereScale -textvariable TetraMesh(SphereScale) -width 5} $Gui(WEA)
   
           pack $f.lSphereScale $f.eSphereScale -side left -padx $Gui(pad)
   
           #-------------------------------------------
           # SField->Skip
           #-------------------------------------------
   
           set f $ff.fSkip
   
           DevAddLabel  $f.lSphereSkip "Keep Every Nth Node:"
           eval {entry $f.eSphereSkip -textvariable TetraMesh(SphereSkip) -width 5} $Gui(WEA)
   
           pack $f.lSphereSkip $f.eSphereSkip -side left -padx $Gui(pad)


	#-------------------------------------------
	# Process->Options->ScalarField
	#-------------------------------------------
        set ff $fProcess.fOptions.fScalarField
        set  f $ff

	foreach frame "Scale Skip" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

           #-------------------------------------------
           # SField->Scale
           #-------------------------------------------
           set f $ff.fScale
   
           DevAddLabel  $f.lSphereScale "Sphere Scaling"
           eval {entry $f.eSphereScale -textvariable TetraMesh(SphereScale) -width 5} $Gui(WEA)
   
           pack $f.lSphereScale $f.eSphereScale -side left -padx $Gui(pad)
   
           #-------------------------------------------
           # SField->Skip
           #-------------------------------------------
   
           set f $ff.fSkip
   
           DevAddLabel  $f.lSphereSkip "Keep Every Nth Node:"
           eval {entry $f.eSphereSkip -textvariable TetraMesh(SphereSkip) -width 5} $Gui(WEA)
   
           pack $f.lSphereSkip $f.eSphereSkip -side left -padx $Gui(pad)

	#-------------------------------------------
	# Process->Options->VectorField
	#-------------------------------------------
        set ff $fProcess.fOptions.fVectorField
        set f $ff

	foreach frame "Scale Skip" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

           #-------------------------------------------
           # VField->Scale
           #-------------------------------------------
           set f $ff.fScale
   
           DevAddLabel  $f.lArrowScale "Arrow Scaling"
           eval {entry $f.eArrowScale -textvariable TetraMesh(ArrowScale) -width 5} $Gui(WEA)
   
           pack $f.lArrowScale $f.eArrowScale -side left -padx $Gui(pad)
   
           #-------------------------------------------
           # VField->Skip
           #-------------------------------------------
   
           set f $ff.fSkip
   
           DevAddLabel  $f.lArrowSkip "Keep Every Nth Arrow:"
           eval {entry $f.eArrowSkip -textvariable TetraMesh(ArrowSkip) -width 5} $Gui(WEA)
   
           pack $f.lArrowSkip $f.eArrowSkip -side left -padx $Gui(pad)

        #-------------------------------------------
        # Process->Bottom frame
        #-------------------------------------------
        set f $fProcess.fBottom

#	eval {label $f.lSurface -text "You clicked 0 times."} $Gui(WLA)
#        pack $f.lSurface -side top -padx $Gui(pad) -fill x
#	set TetraMesh(lSurface) $f.lSurface

        DevAddButton $f.bGo Process TetraMeshProcessTetraMesh

        # Tooltip example: Add a tooltip for the button
        TooltipAdd $f.bGo "Press this button to start Processing the Mesh"

        pack $f.bGo  -side top -padx $Gui(pad) -pady $Gui(pad)

        #-------------------------------------------
        # View frame
        #-------------------------------------------
        set fView $Module(TetraMesh,fView)
        set f $fView

        foreach frame "Top Bottom" {
                frame $f.f$frame -bg $Gui(activeWorkspace)
                pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
        }

        #-------------------------------------------
        # View->Top frame
        #-------------------------------------------
        set f $fView.fTop

	# Parallel button
         # Copied from MainView.tcl

	eval {label $f.lParallelViewing  -justify left -text \
" Parallel, rather than perspective,\n\
viewing can be helpful:\n"} $Gui(WLA)

        pack $f.lParallelViewing -side top

	eval {checkbutton $f.cParallel \
        -text "Parallel" -variable View(parallelProjection) -width 7 \
        -indicatoron 0 -command "MainViewSetParallelProjection"} $Gui(WCA)
        TooltipAdd $f.cParallel "Toggle parallel/perspective projection"

	DevAddLabel $f.lblank "         "

	# Scale Label
	DevAddLabel $f.lParallelScale "Scale:"

	#  Scale entry box
	DevAddEntry View parallelScale $f.eParallelScale
        TooltipAdd $f.eParallelScale "Scale for parallel projection"

	pack $f.lblank $f.cParallel $f.lParallelScale $f.eParallelScale \
		-side left -padx 3

        #-------------------------------------------
        # View->Bottom frame
        #-------------------------------------------
        set f $fView.fBottom

	eval {label $f.l  -justify left -text \
" Clipping can either be done as Intersection\n\
or Union. Intersection clips all regions that\n\
satisfy the constraints of all clipping planes.\n\
Union clips all regions that satisfy the\n\
constrains of at least one clipping plane.\n"} $Gui(WLA)

	grid $f.l

        foreach p "Union Intersection" {
            eval {radiobutton $f.r$p -width 10 \
                    -text "$p" -value "$p" \
                    -variable Slice(clipType) \
                   -command "Slice(clipPlanes) SetOperationTypeTo$p; Render3D"\
                    -indicatoron 0 \
                } $Gui(WCA) 
        grid $f.r$p -padx 0 -pady 0
    }
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshEnter {} {
    global TetraMesh
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $TetraMesh(eventManager)
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshUpdateGUI {} {
	global TetraMesh Volume

}

#-------------------------------------------------------------------------------
# .PROC TetraMeshSetProcessType
# 
# Called to Alter the Gui based on the processing type.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshSetProcessType {} {
    global TetraMesh
    
    raise $TetraMesh(f$TetraMesh(ProcessType))
    focus $TetraMesh(f$TetraMesh(ProcessType))
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessTetraMesh
#
# Calls the correct processing routine for the TetraMesh.
#
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessTetraMesh {} {
    global TetraMesh

    TetraMeshProcess${TetraMesh(ProcessType)}
}


#-------------------------------------------------------------------------------
# .PROC TetraMeshFileNameEntered
# 
# This procedure is called when a filename has been entered.
#
# It simply guesses the name of the "Model Base Name" and
# updated the DefaultDir for Tetra Meshes. 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshFileNameEntered {} {
	global TetraMesh Volume

    set TetraMesh(DefaultDir) [file dirname $TetraMesh(FileName)]
    if  {$TetraMesh(modelbasename) == "" } {
        set TetraMesh(modelbasename) \
                [ file root [file tail $TetraMesh(FileName)]]
    }
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshGetData
#
# This Routine Checks that information has been entered properly 
# when the Process button has been pressed.
# Returns 0 if the data has not been entered properly
# If the data has been entered properly, reads in the TetraMesh
# into TetraReader.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshGetData {} {
	global TetraMesh Model Volume

    set fileName "$TetraMesh(FileName)"
    set modelbasename "$TetraMesh(modelbasename)"

######################################################################
#### Check for problems in the input
######################################################################

    if {$fileName == ""} { 
        DevWarningWindow "You need to select a Tetrahedral Mesh!"
        return 0
    }
    if {![file exists $fileName]} { 
        DevWarningWindow "$fileName does not exist!"
        return 0
    }
    if {$modelbasename == ""} { 
        DevWarningWindow "You need to select the Model Name!"
        return 0
    }

######################################################################
#### Read the tetrahedral mesh
######################################################################

vtkUnstructuredGridReader TetraReader
  TetraReader SetFileName $fileName
  TetraReader Update
#  TetraReader SetStartMethod     MainStartProgress
#  TetraReader SetProgressMethod "MainShowProgress reader"
#  TetraReader SetEndMethod       MainEndProgress

set CurrentTetraMesh [TetraReader GetOutput]
$CurrentTetraMesh Update

    return 1
}

#-------------------------------------------------------------------------------
# .PROC SetModelMoveOriginMatrix
#
#  Description
#
#
# by Samson Timoner
#
#  Use of the position matrix causes models to have an origin in the 
#  center of a volume. I want it in the corner, in the middle of a voxel
#  with the coordinate system used in my code:
#
#  The coordinate system used in my code is pretty standard:
#  slices are in row-major order.
#  rows are aligned along the x-axis. 
#  The first row is x=0. The last row is       x=NumX*XSpacing.
#  The first column is y=0. The last column is y=NumY*YSpacing.
#  The first slice is z=0. The last slice is   z=NumZ*Zspacing.
#  The space goes from (0,0,0) to 
#  (Numx*XSpacing,NumY*Yspacing,Numz*Zspacing)
#
# SIMON WARFIELD WROTE:
#  In my code own image processing code I use the same convention as you
# describe above.  I assume you know that in graphics it is often the case 
# that x and y are switched w.r.t. this convention, with x indexing columns.  
# Also, at one point in time vtk switched to reflecting a particular axis
# in a read function.
# END
#
# Simon refers to the way it is done in the slicer. To correct for it,
# one inverts the y-axis and does a translation.
#
# To show this, I did the following:
# Create a volume by hand size (64,64,64) with noticable stuff in each corner
# I then created 8 different models consisting of small cubes at (0,0,0)
# (0,64,0), etc.
# I then made correspondances:
#
# in model    in (i,j,k) of image
# (0,0,0)    ->  (0,64,0)
# (64,0,0)   ->  (64,64,0)
# (64,64,0)  ->  (64,0,0)
# (0,64,0)   ->  (0,0,0)
# 
# (64,64,64) ->  (64,0,64)
#
# It seems pretty cleas that x and z are unchanged.
# y is reflected about the center of its axis.
# That is, (New y) = 64 - (Old y);
#
# This should be the matrix to take care of this:
#
# 1   0 0 0
# 0  -1 0 Y Size of image in mm.
# 0   0 1 0
# 0   0 0 1
#
# Note: take incoming data, multiply by above matrix, then the position matrix!
# 
# Transform
## Matrix: Above Matrix
## Matrix: Position matrix
## Model 
# End Transform
#
# Now, while the above test is nice, I did NOT pay careful attention
# to exact placement. So, we may need to translate the resulting image
# by, say, a half voxel dimension or something like that.
#
# DAVE GERING WROTE:
# For medical reasons, an object that fills a 240 mm
# Field of View better be 240 mm long, or we all get sued. The
# PositionMatrix (scaledIJK->RAS) positions the origin exactly in the
# middle of this field. That would be between voxels in a volume
# with an even number of voxels across its width, and mid-voxel if
# odd. 
# END
#
# I conclude from the above that (0,0,0) is on the edge of a voxel.
# I want the origin (0,0,0) to be the middle of a voxel, not the
# corner of a voxel! Thus all I must do is shift all the points by 0.5 voxel.
# Note that you do this BEFORE the inversion of the y-axis.
## (Think of an example: 10 points, spacing = 1. Want 0 -> 9.5. Want 9 -> 0.5).
#
# 1  0 0 0
# 0 -1 0 Y Size of image in mm.  
# 0  0 1 0                       + Translation(Spacex/2,Spacey/2,Spacez/2) 
# 0  0 0 1
#
# = 
# 1  0 0 Spacex/2
# 0 -1 0 Size of image in mm - spacey/2 
# 0  0 1 Spacez/2
# 0  0 0 1
#
#
# Problems: Things may change slightly for gantry tilt, but I don't think
# so. The position matrix should take care of that.
#
# .ARGS
#  vtkMrmlVolumeNode n the vtkMrmlVolumeNode
#  vtkMatrix4x4      matrix the matrix whose elements are to be set
# .END
#-------------------------------------------------------------------------------
proc SetModelMoveOriginMatrix {n matrix} {
  global Volume

    # Deal with 90 degree rotation problem
    $matrix Zero
    $matrix SetElement 0 0  1
    $matrix SetElement 1 1 -1
    $matrix SetElement 2 2  1
    $matrix SetElement 3 3  1

    # Deal with Offsets
    set Space0 [lindex [$n GetSpacing] 0]
    set Space1 [lindex [$n GetSpacing] 1]
    set Space2 [lindex [$n GetSpacing] 2]

    $matrix SetElement 0 3 [ expr $Space0 * 0.5 ]
    $matrix SetElement 2 3 [ expr $Space2 * 0.5 ]

    set numy [lindex [$n GetDimensions] 1]
    set ytrans [expr $numy * $Space1 ]
    puts $ytrans
    set ytrans [expr $ytrans - $Space1 * 0.5 ]
    $matrix SetElement 1 3 $ytrans

    puts [$matrix GetElement 0 3]
    puts [$matrix GetElement 1 3]
    puts [$matrix GetElement 2 3]
    puts [$matrix GetElement 3 3]
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshGetTransform
#
# This Routine Determines the correct transform
# and returns the vtkTransform in \"TheTransform\"
# .END
#-------------------------------------------------------------------------------
proc TetraMeshGetTransform {} {
  global Volume

  ######################################################################
  #### Now, determine the transform
  #### If a volume has been selected, use that volumes ScaledIJK to RAS
  #### Otherwise, just use the identity.
  ######################################################################

  vtkTransform TheTransform

  set v $Volume(activeID)

  puts "$v"
  if {$v != "" && $v != $Volume(idNone) } {
      SetModelMoveOriginMatrix Volume($v,node) ModelMoveOriginMatrix
      ModelMoveOriginMatrix Print
      TheTransform PostMultiply
      TheTransform Concatenate ModelMoveOriginMatrix
      TheTransform Concatenate [Volume($v,node) GetPosition]
     } else {
       TheTransform Identity
   }
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessEdges2
#
# This Routine Reads in the TetraMesh and produces
# a Model for the edges in the mesh
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessEdges2 {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the input mesh
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

set range [$CurrentTetraMesh GetScalarRange]
set lowscalar [ lindex $range 0 ] 
set highscalar [ lindex $range 1 ] 
  ## need to hold on to full range of scalars
  ## so that each model gets a different color
set LOWSCALAR $lowscalar
set HIGHSCALAR $highscalar

#######################################################################
#### Setup the pipeline: Get edges, convert to PolyData, Transform
#######################################################################

  #############################################################
  #### Get Edges
  #############################################################

  vtkExtractEdges TetraEdges
    TetraEdges SetInput [TetraReader GetOutput]

#  #############################################################
#  #### Convert to Tubes
#  #############################################################
#
#
#  Converting to Tubes used too much memory and it lost color
#  And, all I really wanted was lines.
#
#   vtkTubeFilter TetraTubes
#     TetraTubes SetInput [TetraEdges GetOutput]
#     TetraTubes SetRadius $TetraMesh(TetraTubeRadius)
#
  #############################################################
  #### Set Lookup Table??? Not yet...
  #############################################################

  TetraMeshGetTransform
  set v $Volume(activeID)

  vtkTransformPolyDataFilter TransformPolyData
    TransformPolyData SetInput [TetraEdges GetOutput]
    TransformPolyData SetTransform TheTransform
  TransformPolyData Update

  ######################################################################
  #### Now, create the model
  ######################################################################

  ### Create the new Model
  set m [ TetraMeshCreateModel $modelbasename $LOWSCALAR $HIGHSCALAR ]

  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [TransformPolyData GetOutput] $m

  puts [ $Model($m,polyData) GetNumberOfPolys]
   
  #############################################################
  #### Clean Up
  #############################################################

  TheTransform Delete
  TransformPolyData Delete
  TetraReader Delete
  TetraEdges Delete
#  TetraTubes Delete

######################################################################
#### Update and Redraw
######################################################################

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}      

#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessEdges
#
# This Routine Reads in the TetraMesh and produces
# a Model of lines for each scalar in the TetraMesh.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessEdges {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the input mesh
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

set range [$CurrentTetraMesh GetScalarRange]
set lowscalar [ lindex $range 0 ] 
set highscalar [ lindex $range 1 ] 

  ## need to hold on to full range of scalars
  ## so that each model gets a different color
set LOWSCALAR $lowscalar
set HIGHSCALAR $highscalar

#######################################################################
#### Setup the pipeline: Threshold the data, grab lines, Transform
#######################################################################

  #############################################################
  #### Threshold the Data
  #############################################################

vtkThreshold Thresh
  Thresh SetInput $CurrentTetraMesh
  # Only take cells whose entire cells satisfy the criteria
  Thresh SetAttributeModeToUseCellData
  Thresh AllScalarsOn 

  #############################################################
  #### Convert the Tetrahedral Mesh to PolyData
  #############################################################

  vtkExtractEdges TetraEdges
    TetraEdges SetInput [Thresh GetOutput]

  ######################################################################
  #### Now, determine the transform
  #### If a volume has been selected, use that volumes ScaledIJK to RAS
  #### Otherwise, just use the identity.
  ######################################################################

  TetraMeshGetTransform
  set v $Volume(activeID)


vtkTransformPolyDataFilter TransformPolyData
  TransformPolyData SetInput [TetraEdges GetOutput]
  TransformPolyData SetTransform TheTransform

 ######################################################################
 #### For each Scalar Determine if there is any points in there
 #### If so, create an output model
 ######################################################################

set i 0
set first $Model(idNone)

while { [$CurrentTetraMesh GetNumberOfPoints] > 0 } {
#  puts "starting new"
  ### Get the lowest Scalar Data
  Thresh ThresholdBetween $lowscalar $lowscalar
  ### Finish the pipeline
  TransformPolyData Update

  ### Create the new Model
  set m [TetraMeshCreateModel $modelbasename$lowscalar $LOWSCALAR $HIGHSCALAR ]

  if { $first == $Model(idNone) }  { 
     set first $m
  }

  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [TransformPolyData GetOutput] $m

  ### Get the remaining Data ###
  Thresh ThresholdBetween [ expr { $lowscalar + 0.01} ] $highscalar
  set CurrentTetraMesh [Thresh GetOutput]
  $CurrentTetraMesh Update
  set range [$CurrentTetraMesh GetScalarRange]
  set lowscalar [ lindex $range 0 ]
  set highscalar [ lindex $range 1 ]
  incr i
#  puts [ $Model($m,polyData) GetNumberOfPolys]
}

#   puts [ $Model($m,polyData) GetNumberOfPolys]

#TransformMesh Delete

#ugw Delete
#testme Delete
TheTransform Delete
TransformPolyData Delete
Thresh Delete
TetraEdges Delete
TetraReader Delete


## This code is obsolete. Though, it is an excellent example of how
## to add a transform to the Mrml tree. This is the right way to do things,
## but it doesn't work in the slicer. GRUMBLE!
#
#######################################################################
##### Now, add a transform
##### by putting a transform around the newly created models
##### 
#######################################################################
#
#if {$TetraMesh(AlignmentVolume) != "" && \
#        $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
#    
#    set last $m
#
#    set matrixnum [ DataAddTransform 0 Model($first,node) Model($last,node) ]
#    Matrix($matrixnum,node) SetName "TetraMeshTransform"
#
#
#######################################################################
##### Now, transform the data appropriately
##### By editing the new matrix
#######################################################################
#
#      Matrix($matrixnum,node) SetMatrix \
#              [Volume($TetraMesh(AlignmentVolume),node) GetPositionMatrix]
#
##      [Matrix($matrixnum,node) GetTransform] Inverse
#}

######################################################################
#### Update and Redraw
######################################################################
MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}   


#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessScalarField
#
# This Routine Reads in the TetraMesh and produces
# a Model for the Scalar Field of the nodes in the mesh. 
# It assumes the nodes have scalar labels
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessScalarField {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

    set TetraMeshFractionOn $TetraMesh(SphereSkip)
    set TetraMeshArrowScale $TetraMesh(SphereScale)

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

#######################################################################
#### Setup the pipeline: Cones -> PointMaskSelection -> Glyph data
#######################################################################

  TetraMeshGetTransform
  set v $Volume(activeID)

vtkMaskPoints PointSelection
  PointSelection SetInput $CurrentTetraMesh
  PointSelection SetOnRatio $TetraMeshFractionOn
  PointSelection RandomModeOff
vtkTransformFilter TransPoints
  TransPoints SetTransform TheTransform
  TransPoints SetInput [PointSelection GetOutput]
vtkSphereSource TetraSphere
  TetraSphere SetPhiResolution 5
  TetraSphere SetThetaResolution 5
  TetraSphere SetRadius 0.15
vtkGlyph3D ScalarGlyph
  ScalarGlyph SetInput [TransPoints GetOutput]
  ScalarGlyph SetSource [TetraSphere GetOutput]
  ScalarGlyph SetScaleModeToDataScalingOff
  ScalarGlyph SetScaleModeToScaleByScalar
  ScalarGlyph SetScaleFactor $TetraMeshArrowScale
ScalarGlyph Update

  #set m [ TetraMeshCreateModel ${modelbasename}Scalars $LOWSCALAR $HIGHSCALAR ]
  set m [ TetraMeshCreateModel ${modelbasename}Scalars 0 10 ]

  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

TheTransform Delete
TransPoints Delete
TetraReader Delete
TetraSphere Delete
PointSelection Delete
ScalarGlyph Delete

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}


#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessNodes
#
# This Routine Reads in the TetraMesh and produces
# a Model for the Scalar Field of the nodes in the mesh. 
# It assumes the nodes have scalar labels
#
# Only one line difference between this and TetraMeshProcessScalar
#
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessNodes {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

    set TetraMeshFractionOn $TetraMesh(SphereSkip)
    set TetraMeshArrowScale $TetraMesh(SphereScale)

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

#######################################################################
#### Setup the pipeline: Cones -> PointMaskSelection -> Glyph data
#######################################################################

  TetraMeshGetTransform
  set v $Volume(activeID)

vtkMaskPoints PointSelection
  PointSelection SetInput $CurrentTetraMesh
  PointSelection SetOnRatio $TetraMeshFractionOn
  PointSelection RandomModeOff
vtkTransformFilter TransPoints
  TransPoints SetTransform TheTransform
  TransPoints SetInput [PointSelection GetOutput]
vtkSphereSource TetraSphere
  TetraSphere SetPhiResolution 5
  TetraSphere SetThetaResolution 5
  TetraSphere SetRadius 0.15
vtkGlyph3D ScalarGlyph
  ScalarGlyph SetInput [TransPoints GetOutput]
  ScalarGlyph SetSource [TetraSphere GetOutput]
  ScalarGlyph SetScaleModeToDataScalingOff
#  ScalarGlyph SetScaleModeToScaleByScalar
  ScalarGlyph SetScaleFactor $TetraMeshArrowScale
ScalarGlyph Update

  #set m [ TetraMeshCreateModel ${modelbasename}Nodes $LOWSCALAR $HIGHSCALAR ]
  set m [ TetraMeshCreateModel ${modelbasename}Nodes 0 10 ]

  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

TheTransform Delete
TransPoints Delete
TetraReader Delete
TetraSphere Delete
PointSelection Delete
ScalarGlyph Delete

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}


#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessCorrespond
#
# This Routine Reads in the TetraMesh and produces
# a Model for the Scalar Field of the nodes in the mesh. 
# It assumes the nodes have scalar labels
#
# Only one line difference between this and TetraMeshProcessScalar
#
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessCorrespond {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

    set TetraMeshFractionOn $TetraMesh(SphereSkip)
    set TetraMeshArrowScale $TetraMesh(SphereScale)

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

#######################################################################
#### Setup the pipeline: Cones -> PointMaskSelection -> Glyph data
#######################################################################

  TetraMeshGetTransform
  set v $Volume(activeID)

vtkMaskPoints PointSelection
  PointSelection SetInput $CurrentTetraMesh
  PointSelection SetOnRatio $TetraMeshFractionOn
  PointSelection RandomModeOff
vtkTransformFilter TransPoints
  TransPoints SetTransform TheTransform
  TransPoints SetInput [PointSelection GetOutput]
vtkSphereSource TetraSphere
  TetraSphere SetPhiResolution 5
  TetraSphere SetThetaResolution 5
  TetraSphere SetRadius 0.15
vtkGlyph3D ScalarGlyph
  ScalarGlyph SetInput [TransPoints GetOutput]
  ScalarGlyph SetSource [TetraSphere GetOutput]
  ScalarGlyph SetScaleModeToDataScalingOff
#  ScalarGlyph SetScaleModeToScaleByScalar
  ScalarGlyph SetScaleFactor $TetraMeshArrowScale
ScalarGlyph Update

  #set m [ TetraMeshCreateModel ${modelbasename}Correspond $LOWSCALAR $HIGHSCALAR ]
  set m [ TetraMeshCreateModel ${modelbasename}Correspond 0 10 ]


  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

TheTransform Delete
TransPoints Delete
TetraReader Delete
TetraSphere Delete
PointSelection Delete
ScalarGlyph Delete

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}



#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessVectorField
#
# This Routine Reads in the TetraMesh and produces
# a Model for the vector field in the point data
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessVectorField {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

######################################################################
#### Check that the Vectors are there
######################################################################

set tmpvectors [[$CurrentTetraMesh GetPointData] GetVectors] 
if { $tmpvectors == ""} {
    DevErrorWindow "No Vector Data in Mesh"
    TetraReader Delete
    return
}

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

set range [$CurrentTetraMesh GetScalarRange]
set lowscalar [ lindex $range 0 ] 
set highscalar [ lindex $range 1 ] 

  ## need to hold on to full range of scalars
  ## so that each model gets a different color
set LOWSCALAR $lowscalar
set HIGHSCALAR $highscalar
set TetraMeshFractionOn $TetraMesh(ArrowSkip)
set TetraMeshArrowScale $TetraMesh(ArrowScale)

#######################################################################
#### Setup the pipeline: Cones -> PointMaskSelection -> Glyph data
#######################################################################

vtkConeSource TetraCone
  TetraCone SetResolution 2
  TetraCone SetHeight 1
  TetraCone SetRadius 0.15
vtkMaskPoints PointSelection
    PointSelection SetInput $CurrentTetraMesh
    PointSelection SetOnRatio $TetraMeshFractionOn
    PointSelection RandomModeOff
vtkGlyph3D VectorGlyph
  VectorGlyph SetInput [PointSelection GetOutput]
  VectorGlyph SetSource [TetraCone GetOutput]
  VectorGlyph SetScaleModeToScaleByVector
#  VectorGlyph SetScaleModeToDataScalingOff
  VectorGlyph SetScaleFactor $TetraMeshArrowScale
  VectorGlyph Print
  ######################################################################
  #### Now, determine the transform
  #### If a volume has been selected, use that volumes ScaledIJK to RAS
  #### Otherwise, just use the identity.
  ######################################################################

  TetraMeshGetTransform
  set v $Volume(activeID)


vtkTransformPolyDataFilter TransformPolyData
  TransformPolyData SetInput [VectorGlyph GetOutput]
  TransformPolyData SetTransform TheTransform

TransformPolyData Update

set m [ TetraMeshCreateModel ${modelbasename}Vector 0 10 ]

#set m [ TetraMeshCreateModel ${modelbasename}Vector $LOWSCALAR $HIGHSCALAR ]

  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

TheTransform Delete
TransformPolyData Delete
TetraReader Delete
TetraCone Delete
PointSelection Delete
VectorGlyph Delete

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessSurface
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessSurface {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
######################################################################

    if {![TetraMeshGetData]} {
      return;
    }

    set modelbasename "$TetraMesh(modelbasename)"

    set CurrentTetraMesh [TetraReader GetOutput]
    $CurrentTetraMesh Update

######################################################################
#### Get the range of the data, not exactly thread safe. See vtkDataSet.h
#### But, since we are not concurrently modifying the dataset, we should
#### be OK.
######################################################################

#  This did the range of the scalar data and point data
#set range [$CurrentTetraMesh GetScalarRange]
# This is what we want
set range [[[$CurrentTetraMesh GetCellData] GetScalars] GetRange]
set lowscalar [ lindex $range 0 ] 
set highscalar [ lindex $range 1 ] 

  ## need to hold on to full range of scalars
  ## so that each model gets a different color
set LOWSCALAR $lowscalar
set HIGHSCALAR $highscalar

######################################################################
#### I tried to scale the data first, it didn't work. I don't know why.
#### I tested the output by piping to a file, that worked just fine.
#### So, it seems the data is processed more before being displayed.
#### I don't understand why that would be.
######################################################################
#vtkTransform TheTransform
#
#if {$TetraMesh(AlignmentVolume) != "" && \
#        $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
#    
# TheTransform Concatenate [Volume($TetraMesh(AlignmentVolume),node) GetPosition]
#puts "yo"
#} else {
#    TheTransform Identity
#}
#
#vtkMatrix4x4 testme
#  testme Identity
#  testme SetElement 0 0 20
#  testme SetElement 1 1 20
#  testme SetElement 2 2 20
#
## TheTransform Concatenate testme
#
#
#vtkTransformFilter TransformMesh
#  TransformMesh SetInput $CurrentTetraMesh
#  TransformMesh SetTransform TheTransform
#
#set $CurrentTetraMesh [TransformMesh GetOutput]
#$CurrentTetraMesh Update
#
#vtkUnstructuredGridWriter ugw
#   ugw SetInput $CurrentTetraMesh
#   ugw SetFileName "/tmp/tt.vtk"
#   ugw SetFileTypeToASCII
#   ugw Update
#

#######################################################################
#### Setup the pipeline: Threshold the data, convert to PolyData, Transform
#######################################################################

  #############################################################
  #### Threshold the Data
  #############################################################

vtkThreshold Thresh
  Thresh SetInput $CurrentTetraMesh
  # Only take cells whose entire cells satisfy the criteria
  Thresh SetAttributeModeToUseCellData
  Thresh AllScalarsOn 

  #############################################################
  #### Convert the Tetrahedral Mesh to PolyData
  #############################################################

vtkGeometryFilter gf
  gf SetInput [Thresh GetOutput]

  ######################################################################
  #### Now, determine the transform
  #### If a volume has been selected, use that volumes ScaledIJK to RAS
  #### Otherwise, just use the identity.
  ######################################################################

  TetraMeshGetTransform  
  set v $Volume(activeID)


vtkTransformPolyDataFilter TransformPolyData
  TransformPolyData SetInput [gf GetOutput]
  TransformPolyData SetTransform TheTransform

 ######################################################################
 #### For each Scalar Determine if there is any points in there
 #### If so, create an output model
 ######################################################################

set i 0
set first $Model(idNone)

while { [$CurrentTetraMesh GetNumberOfPoints] > 0 } {
#  puts "starting new"
  ### Get the lowest Scalar Data
  Thresh ThresholdBetween $lowscalar $lowscalar
  ### Finish the pipeline
  TransformPolyData Update

  ### Create the new Model
  set m [ TetraMeshCreateModel $modelbasename$lowscalar $LOWSCALAR $HIGHSCALAR ]

  if { $first == $Model(idNone) }  { 
     set first $m
  }


  #############################################################
  #### Copy the output and set up the renderers
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  TetraMeshCopyPolyData [TransformPolyData GetOutput] $m

  ### Get the remaining Data ###
  Thresh ThresholdBetween [ expr { $lowscalar + 0.01} ] $highscalar
  set CurrentTetraMesh [Thresh GetOutput]
  $CurrentTetraMesh Update
  set range [$CurrentTetraMesh GetScalarRange]
  set lowscalar [ lindex $range 0 ]
  set highscalar [ lindex $range 1 ]
  incr i
#  puts [ $Model($m,polyData) GetNumberOfPolys]
}

#   puts [ $Model($m,polyData) GetNumberOfPolys]

#TransformMesh Delete

#ugw Delete
#testme Delete
TheTransform Delete
TransformPolyData Delete
Thresh Delete
gf Delete
TetraReader Delete


## This code is obsolete. Though, it is an excellent example of how
## to add a transform to the Mrml tree. This is the right way to do things,
## but it doesn't work in the slicer. GRUMBLE!
#
#######################################################################
##### Now, add a transform
##### by putting a transform around the newly created models
##### 
#######################################################################
#
#if {$TetraMesh(AlignmentVolume) != "" && \
#        $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
#    
#    set last $m
#
#    set matrixnum [ DataAddTransform 0 Model($first,node) Model($last,node) ]
#    Matrix($matrixnum,node) SetName "TetraMeshTransform"
#
#
#######################################################################
##### Now, transform the data appropriately
##### By editing the new matrix
#######################################################################
#
#      Matrix($matrixnum,node) SetMatrix \
#              [Volume($TetraMesh(AlignmentVolume),node) GetPositionMatrix]
#
##      [Matrix($matrixnum,node) GetTransform] Inverse
#}

######################################################################
#### Update and Redraw
######################################################################

MainModelsUpdateMRML 
MainUpdateMRML
Render3D

set TetraMesh(modelbasename) ""
}   


#-------------------------------------------------------------------------------
# .PROC TetraMeshCorrespondSurfaces
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCorrespondSurfaces {} {
	global TetraMesh Model Volume Module

######################################################################
#### Get the data
#### Check for problems in the input
######################################################################

    set m1   $TetraMesh(Model1)
    set m2   $TetraMesh(Model2)
    set PD1  $Model($m1,polyData)
    set PD2  $Model($m2,polyData)

    set NumNode1 [$PD1 GetNumberOfPoints]
    set NumNode2 [$PD2 GetNumberOfPoints]

    if {$NumNode1 != $NumNode2} {
        puts "Must have same number of nodes in each surface mesh!"
        exit 0;
    }

######################################################################
#### Set scalars on the data
#### This is a bug: I replace Scalars on models!!
######################################################################


######################################################################
#################### Get the Points Set and put them together
######################################################################

vtkMaskPoints PointSelection
  PointSelection SetInput $PD1
  PointSelection SetOnRatio $TetraMesh(CorSphereSkip)
  PointSelection RandomModeOff

  set NumSelectNode [[PointSelection GetOutput] GetNumberOfPoints]

  vtkScalars Scalars
  for {set i 0} { $i < $NumSelectNode } {incr i 1} {
       Scalars InsertScalar $i $i
  }

  PointSelection Update
  [[PointSelection GetOutput] GetPointData] SetScalars Scalars

vtkSphereSource ASphere
  ASphere SetPhiResolution 5
  ASphere SetThetaResolution 5
  ASphere SetRadius [ expr 0.15 * $TetraMesh(CorSphereScale) ]
vtkGlyph3D ScalarGlyph
  ScalarGlyph SetInput  [PointSelection GetOutput]
  ScalarGlyph SetSource [ASphere GetOutput]
  ScalarGlyph SetScaleModeToDataScalingOff
  ScalarGlyph SetColorModeToColorByScalar
#  ScalarGlyph SetScaleModeToScaleByScalar
#  ScalarGlyph SetScaleFactor 
  ScalarGlyph Update

  ########################################
  ##### Form the Model
  ########################################

  set range [[PointSelection GetOutput] GetScalarRange]
  set LOWSCALAR  [ lindex $range 0 ]
  set HIGHSCALAR [ lindex $range 1 ]
  set name [Model($m1,node) GetName]
  set name "${name}-correspond-points"

  ### Create the new Model
  set m [ TetraMeshCreateModel $name $LOWSCALAR $HIGHSCALAR ]
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

  PointSelection SetInput $PD2
  PointSelection Update
  [[PointSelection GetOutput] GetPointData] SetScalars Scalars
  ScalarGlyph Update

  set name [Model($m2,node) GetName]
  set name "${name}-correspond-points"

  set m [ TetraMeshCreateModel $name $LOWSCALAR $HIGHSCALAR ]
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

  PointSelection Delete
  ASphere Delete
  ScalarGlyph Delete
  Scalars Delete

  MainModelsUpdateMRML 
  MainUpdateMRML
  Render3D
}



#-------------------------------------------------------------------------------
# .PROC TetraMeshCopyPolyData
#
# Copies the polydata into a Node's polydata.
# And then, assigns all the renderers to it.
# 
# .ARGS
#
# vtkPolyData PolyData
# m  number of the model to copy data into 
#
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCopyPolyData  {PolyData m} {
    global Model Module 

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  vtkPolyData ModelPolyData$m
  set Model($m,polyData) ModelPolyData$m
  $Model($m,polyData) CopyStructure $PolyData
  [ $Model($m,polyData) GetPointData] PassData [$PolyData GetPointData]
  [ $Model($m,polyData) GetCellData]  PassData  [$PolyData GetCellData ]

  ### The next line would replace the last bunch if we didn't care about
  ### deleting the inputs causing the results to be deleted.
  #  set Model($m,polyData) [$PolyData]
  #  $Model($m,polyData) Update
  #  puts [ $Model($m,polyData) GetNumberOfPolys]

  ## Assign the renderers
  foreach r $Module(Renderers) {
      Model($m,mapper,$r) SetInput $Model($m,polyData)
  }
}

#-------------------------------------------------------------------------------
# .PROC TetraMeshCreateModel
#
# This file has almost everything you need to create a model.
# You still need to assign the PolyData and finish the pipeline
# through to the mapper.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCreateModel  {name scalarLo scalarHi} {
    global Model Volume Mrml Label

 set n [MainMrmlAddNode Model]
 set i [$n GetID]
 $n SetOpacity          1.0
 $n SetVisibility       1
 $n SetClipping         1
 $n SetName $name
 $n SetFileName "NoFile"
 $n SetFullFileName "NoFile"
 $n SetColor $Label(name)
 set Model(freeze) 0
 MainModelsCreate $i
 MainModelsSetActive $i
 set m $i

# This next part says the models are not saved. Therefore
# One can save them in the ModelMaker.
 set Model($m,dirty) 1

 Model($m,node) SetName $name
 Model($m,node) SetFileName "None"
 Model($m,node) SetFullFileName [file join $Mrml(dir) [Model($m,node) GetFileName]]
 Model($m,node) SetDescription "Generated from Tetrahedral Mesh"

 MainModelsSetClipping $m 1
 MainModelsSetVisibility $m 1
 MainModelsSetOpacity $m 1
 MainModelsSetColor $m $Label(name)
 MainModelsSetCulling $m 0
 MainModelsSetScalarVisibility $m 1
 MainModelsSetScalarRange $m $scalarLo $scalarHi
 return $m
}
