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
#   TetraMeshFileNameEntered
#   TetraMeshCheckErrors
#   TetraMeshProcessEdges
#   TetraMeshProcessEdges2
#   TetraMeshProcessSurface
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
	set Module($m,row1List) "Help Surface Edges View"
        set Module($m,row1Name) "{Help} {Surfaces} {Edges} {View}"
	set Module($m,row1,tab) Surface

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
		{$Revision: 1.9 $} {$Date: 2001/02/19 17:53:32 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
	set TetraMesh(count) 0
	set TetraMesh(FileName) ""
        set TetraMesh(modelbasename) ""
	set TetraMesh(eventManager)  ""
	set TetraMesh(AlignmentVolume) $Volume(idNone)
        set TetraMesh(DefaultDir) ""
            ## Tube Radius of edges
        set TetraMesh(TetraTubeRadius) 0.01
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
	# Surface
	#   Top
	#   Alignment
        #   Bottom
        # Edges
	#   Top
	#   Alignment
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
	# Surface frame
	#-------------------------------------------
	set fSurface $Module(TetraMesh,fSurface)
	set f $fSurface

	foreach frame "Top Middle Alignment Bottom" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Surface->Top frame
	#-------------------------------------------
	set f $fSurface.fTop

	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)

        DevAddFileBrowse $f.f TetraMesh FileName "Tetrahedral Mesh:" "TetraMeshFileNameEntered" "vtk"\
                "\$TetraMesh(DefaultDir)" "Browse for a Tetrahedral Mesh"

	set f $fSurface.fTop.fBaseName

	frame $f  -bg $Gui(activeWorkspace) 
        pack $f -side top -padx 0 -pady $Gui(pad) -fill x

        DevAddLabel  $f.l "Model Base Name"
	eval {entry $f.eModelName -textvariable TetraMesh(modelbasename) -width 50} $Gui(WEA)
        
        pack $f.l $f.eModelName -side left -padx $Gui(pad)

	#-------------------------------------------
	# Surface->Alignment frame
	#-------------------------------------------
	set f $fSurface.fAlignment

        DevAddSelectButton TetraMesh $f AlignmentVolume "Alignment Volume(Optional)" Pack

        lappend Volume(mbActiveList) $f.mbAlignmentVolume
        lappend Volume(mActiveList)  $f.mbAlignmentVolume.m

	#-------------------------------------------
	# Surface->Bottom frame
	#-------------------------------------------
	set f $fSurface.fBottom

#	eval {label $f.lSurface -text "You clicked 0 times."} $Gui(WLA)
#        pack $f.lSurface -side top -padx $Gui(pad) -fill x
#	set TetraMesh(lSurface) $f.lSurface

        DevAddButton $f.bGo Process TetraMeshProcessSurface

        # Tooltip example: Add a tooltip for the button
        TooltipAdd $f.bGo "Press this button to start Processing the Mesh into its Surfaces"

	pack $f.bGo  -side top -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Edges frame
	#-------------------------------------------
	set fEdges $Module(TetraMesh,fEdges)
	set f $fEdges

	foreach frame "Top Middle Alignment Bottom" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Edges->Top frame
	#-------------------------------------------
	set f $fEdges.fTop

	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)

        DevAddFileBrowse $f.f TetraMesh FileName "Tetrahedral Mesh:" "TetraMeshFileNameEntered" "vtk"\
                "\$TetraMesh(DefaultDir)" "Browse for a Tetrahedral Mesh"

	set f $fEdges.fTop.fBaseName

	frame $f  -bg $Gui(activeWorkspace) 
        pack $f -side top -padx 0 -pady $Gui(pad) -fill x

        DevAddLabel  $f.l "Model Name"
	eval {entry $f.eModelName -textvariable TetraMesh(modelbasename) -width 50} $Gui(WEA)
        
        pack $f.l $f.eModelName -side left -padx $Gui(pad)

	#-------------------------------------------
	# Edges->Alignment frame
	#-------------------------------------------
	set f $fEdges.fAlignment

        DevAddSelectButton TetraMesh $f AlignmentVolume "Alignment Volume(Optional)" Pack

        lappend Volume(mbActiveList) $f.mbAlignmentVolume
        lappend Volume(mActiveList)  $f.mbAlignmentVolume.m

	#-------------------------------------------
	# Edges->Bottom frame
	#-------------------------------------------
	set f $fEdges.fBottom

#	eval {label $f.lEdges -text "You clicked 0 times."} $Gui(WLA)
#        pack $f.lEdges -side top -padx $Gui(pad) -fill x
#	set TetraMesh(lEdges) $f.lEdges

        DevAddButton $f.bGo Process TetraMeshProcessEdges2

        # Tooltip example: Add a tooltip for the button
        TooltipAdd $f.bGo "Press this button to start Processing the Mesh into its Edges"

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
# (Note: to do this, this proc must be this module's procMRML.  Right now,
# these buttons are being updated automatically since they have been added
# to lists updated in VolumesUpdateMRML and ModelsUpdateMRML.  So this procedure
# is not currently used.)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshUpdateGUI {} {
	global TetraMesh Volume

#   DevUpdateNodeSelectButton Volume TetraMesh AlignmentVolume AlignmentVolume DevSelectNode
#"   DevUpdateNodeSelectButton Volume TetraMesh AlignmentVolume AlignmentVolume DevSelectNode
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
# .PROC TetraMeshCheckErrors
#
# This Routine Checks that information has been entered properly 
# when the Process button has been pressed.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCheckErrors {} {
	global TetraMesh

#    puts "$TetraMesh(FileName) $TetraMesh(modelbasename)"
    set fileName "$TetraMesh(FileName)"
    set modelbasename "$TetraMesh(modelbasename)"

    if {$fileName == ""} { 
        DevWarningWindow "You need to select a Tetrahedral Mesh!"
        return 
    }
    if {![file exists $fileName]} { 
        DevWarningWindow "$fileName does not exist!"
        return 
    }
    if {$modelbasename == ""} { 
        DevWarningWindow "You need to select the Model Name!"
        return 
    }
}
#-------------------------------------------------------------------------------
# .PROC TetraMeshProcessEdges
#
# This Routine Reads in the TetraMesh and produces
# a Model for the edges in the mesh
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessEdges {} {
	global TetraMesh Model Volume

#    puts "$TetraMesh(FileName) $TetraMesh(modelbasename)"
    set fileName "$TetraMesh(FileName)"
    set modelbasename "$TetraMesh(modelbasename)"

######################################################################
#### Check for problems in the input
######################################################################

    TetraMeshCheckErrors

######################################################################
#### Read the tetrahedral mesh
######################################################################

vtkUnstructuredGridReader tetra_reader
  tetra_reader SetFileName $fileName
  tetra_reader Update
#  tetra_reader SetStartMethod     MainStartProgress
#  tetra_reader SetProgressMethod "MainShowProgress reader"
#  tetra_reader SetEndMethod       MainEndProgress

set CurrentTetraMesh [tetra_reader GetOutput]
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
    TetraEdges SetInput [tetra_reader GetOutput]

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

  ######################################################################
  #### Now, determine the transform
  #### If a volume has been selected, use that volumes ScaledIJK to RAS
  #### Otherwise, just use the identity.
  ######################################################################

  vtkTransform TheTransform

  if {$TetraMesh(AlignmentVolume) != "" && \
          $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
    
     TheTransform Concatenate [Volume($TetraMesh(AlignmentVolume),node) GetPosition]
     } else {
       TheTransform Identity
   }

  vtkTransformPolyDataFilter TransformPolyData
    TransformPolyData SetInput [TetraEdges GetOutput]
    TransformPolyData SetTransform TheTransform
  TransformPolyData Update

  ######################################################################
  #### Now, create the model
  ######################################################################

  ### Create the new Model
  set m [ TetraMeshCreateModel $modelbasename $LOWSCALAR $HIGHSCALAR \
          $TetraMesh(AlignmentVolume) ]

  #############################################################
  #### Copy the output
  #############################################################

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  vtkPolyData TetraEdgePolyData$m
  set Model($m,polyData) TetraEdgePolyData$m
  $Model($m,polyData) CopyStructure [TransformPolyData GetOutput]
  [ $Model($m,polyData) GetPointData] PassData [[TransformPolyData GetOutput] GetPointData]
  [ $Model($m,polyData) GetCellData] PassData [[TransformPolyData GetOutput] GetCellData]

  #############################################################
  #### Set Up the Mapper
  #############################################################

  ### The next line would replace the last bunch if we didn't care about
  ### deleting the inputs causing the results to be deleted.
  #  set Model($m,polyData) [gf GetOutput]
  #  $Model($m,polyData) Update
  #  puts [ $Model($m,polyData) GetNumberOfPolys]
  Model($m,mapper) SetInput $Model($m,polyData)
   puts [ $Model($m,polyData) GetNumberOfPolys]

  #############################################################
  #### Clean Up
  #############################################################

  TheTransform Delete
  TransformPolyData Delete
  tetra_reader Delete
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
# .PROC TetraMeshProcessEdges2
#
# This Routine Reads in the TetraMesh and produces
# a Model of lines for each scalar in the TetraMesh.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessEdges2 {} {
	global TetraMesh Model Volume

#    puts "$TetraMesh(FileName) $TetraMesh(modelbasename)"
    set fileName "$TetraMesh(FileName)"
    set modelbasename "$TetraMesh(modelbasename)"

######################################################################
#### Check for problems in the input
######################################################################

    TetraMeshCheckErrors

######################################################################
#### Read the tetrahedral mesh
######################################################################

vtkUnstructuredGridReader tetra_reader
  tetra_reader SetFileName $fileName
  tetra_reader Update
#  tetra_reader SetStartMethod     MainStartProgress
#  tetra_reader SetProgressMethod "MainShowProgress reader"
#  tetra_reader SetEndMethod       MainEndProgress

set CurrentTetraMesh [tetra_reader GetOutput]
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

vtkTransform TheTransform

if {$TetraMesh(AlignmentVolume) != "" && \
        $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
    
 TheTransform Concatenate [Volume($TetraMesh(AlignmentVolume),node) GetPosition]
} else {
    TheTransform Identity
}

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
  set m [ TetraMeshCreateModel $modelbasename$lowscalar $LOWSCALAR $HIGHSCALAR \
          $TetraMesh(AlignmentVolume) ]

  if { $first == $Model(idNone) }  { 
     set first $m
  }

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  vtkPolyData ModelPolyData$m
  set Model($m,polyData) ModelPolyData$m
  $Model($m,polyData) CopyStructure [TransformPolyData GetOutput]
  [ $Model($m,polyData) GetPointData] PassData [[TransformPolyData GetOutput] GetPointData]
  [ $Model($m,polyData) GetCellData] PassData [[TransformPolyData GetOutput] GetCellData]

  ### The next line would replace the last bunch if we didn't care about
  ### deleting the inputs causing the results to be deleted.
  #  set Model($m,polyData) [gf GetOutput]
  #  $Model($m,polyData) Update
  #  puts [ $Model($m,polyData) GetNumberOfPolys]
  Model($m,mapper) SetInput $Model($m,polyData)

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
tetra_reader Delete


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
# .PROC TetraMeshProcessSurface
#
# This Routine Reads in the TetraMesh and produces
# a Model for each scalar in the TetraMesh.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcessSurface {} {
	global TetraMesh Model Volume

#    puts "$TetraMesh(FileName) $TetraMesh(modelbasename)"
    set fileName "$TetraMesh(FileName)"
    set modelbasename "$TetraMesh(modelbasename)"

######################################################################
#### Check for problems in the input
######################################################################

    TetraMeshCheckErrors

######################################################################
#### Read the tetrahedral mesh
######################################################################

vtkUnstructuredGridReader tetra_reader
  tetra_reader SetFileName $fileName
  tetra_reader Update
#  tetra_reader SetStartMethod     MainStartProgress
#  tetra_reader SetProgressMethod "MainShowProgress reader"
#  tetra_reader SetEndMethod       MainEndProgress

set CurrentTetraMesh [tetra_reader GetOutput]
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

vtkTransform TheTransform

if {$TetraMesh(AlignmentVolume) != "" && \
        $TetraMesh(AlignmentVolume) != $Volume(idNone) } {
    
 TheTransform Concatenate [Volume($TetraMesh(AlignmentVolume),node) GetPosition]
} else {
    TheTransform Identity
}

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
  set m [ TetraMeshCreateModel $modelbasename$lowscalar $LOWSCALAR $HIGHSCALAR \
          $TetraMesh(AlignmentVolume) ]

  if { $first == $Model(idNone) }  { 
     set first $m
  }

  ### Need to copy the output of the pipeline so that the results
  ### Don't get over-written later. Also, when we delete the inputs,
  ### We don't want the outputs deleted. These lines should prevent this.
  vtkPolyData ModelPolyData$m
  set Model($m,polyData) ModelPolyData$m
  $Model($m,polyData) CopyStructure [TransformPolyData GetOutput]
  [ $Model($m,polyData) GetPointData] PassData [[TransformPolyData GetOutput] GetPointData]
  [ $Model($m,polyData) GetCellData] PassData [[TransformPolyData GetOutput] GetCellData]

  ### The next line would replace the last bunch if we didn't care about
  ### deleting the inputs causing the results to be deleted.
  #  set Model($m,polyData) [gf GetOutput]
  #  $Model($m,polyData) Update
  #  puts [ $Model($m,polyData) GetNumberOfPolys]
  Model($m,mapper) SetInput $Model($m,polyData)

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
tetra_reader Delete


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
# .PROC TetraMeshCreateModel
#
# This file has almost everything you need to create a model.
# You still need to assign the PolyData and finish the pipeline
# through to the mapper.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCreateModel  {name scalarLo scalarHi v} {
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
