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
# FILE:        TetraMesh.tcl
# PROCEDURES:  
#   TetraMeshInit
#   TetraMeshBuildGUI
#   TetraMeshEnter
#   TetraMeshExit
#   TetraMeshUpdateGUI
#   TetraMeshProcess
#   TetraMeshCreateModel
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
	set Module($m,row1List) "Help Props"
	set Module($m,row1Name) "{Help} {Props}"
	set Module($m,row1,tab) Props

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
	#   appropriate revision number and date when the module is checked in.
	#   
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.1 $} {$Date: 2000/07/27 22:58:12 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
	set TetraMesh(count) 0
	set TetraMesh(prefix) ""
        set TetraMesh(modelbasename) ""
	set TetraMesh(eventManager)  ""
}


#-------------------------------------------------------------------------------
# .PROC TetraMeshBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshBuildGUI {} {
	global Gui TetraMesh Module Volume Model

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
	# Props
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
The TetraMesh module allows a user to read in a Tetrahedral Mesh.  The Mesh is assumed to have integer scalars.  1 model is produced for each scalar.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags TetraMesh $help
	MainHelpBuildGUI TetraMesh

	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(TetraMesh,fProps)
	set f $fProps

	foreach frame "Top Middle Bottom" {
		frame $f.f$frame -bg $Gui(activeWorkspace)
		pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
	}

	#-------------------------------------------
	# Props->Top frame
	#-------------------------------------------
	set f $fProps.fTop

	frame $f.fPrefix  -bg $Gui(activeWorkspace) -relief groove -bd 3
        set f $f.fPrefix

        pack $f -side top -padx 0 -pady $Gui(pad) -fill x

	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)
        
        DevAddLabel  $f.l "File"
        DevAddButton $f.b "Browse..." {
            set TetraMesh(prefix) [ DevGetFile $TetraMesh(prefix) ]
        }
        pack $f.l $f.b -side left -padx $Gui(pad)

	eval {entry $f.eTMfile -textvariable TetraMesh(prefix) -width 50} $Gui(WEA)
	bind $f.eTMfile <Return> {
            set TetraMesh(prefix) [ DevGetFile $TetraMesh(prefix) ]
        }
	pack $f.eTMfile -side top -pady $Gui(pad) -padx $Gui(pad) \
                -expand 1 -fill x


	set f $fProps.fTop.fBaseName

	frame $f  -bg $Gui(activeWorkspace) 
        pack $f -side top -padx 0 -pady $Gui(pad) -fill x

        DevAddLabel  $f.l "Model Base Name"
	eval {entry $f.eModelName -textvariable TetraMesh(modelbasename) -width 50} $Gui(WEA)
        
        pack $f.l $f.eModelName -side left -padx $Gui(pad)

	#-------------------------------------------
	# Props->Bottom frame
	#-------------------------------------------
	set f $fProps.fBottom

	eval {label $f.lProps -text "You clicked 0 times."} $Gui(WLA)
        pack $f.lProps -side top -padx $Gui(pad) -fill x
	set TetraMesh(lProps) $f.lProps

        DevAddButton $f.bGo Process TetraMeshProcess

        # Tooltip example: Add a tooltip for the button
        TooltipAdd $f.bGo "Press this button to start Processing"

	pack $f.bGo  -side top -padx $Gui(pad) -pady $Gui(pad)
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

}


#-------------------------------------------------------------------------------
# .PROC TetraMeshProcess
#
# This Routine Reads in the TetraMesh and produces
# a Model for each scalar in the TetraMesh.
# .END
#-------------------------------------------------------------------------------
proc TetraMeshProcess {} {
	global TetraMesh

    puts "$TetraMesh(prefix) $TetraMesh(modelbasename)"
    set fileName "$TetraMesh(prefix)"
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
### Setup the pipeline: Threshold the data, convert to PolyData, Write it.
######################################################################

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
#### For each Scalar Determine if there is any points in there
#### If so, create an output model
######################################################################

set i 0

while { [$CurrentTetraMesh GetNumberOfPoints] > 0 } {
  ### Get the lowest Scalar Data
  Thresh ThresholdBetween $lowscalar $lowscalar

  ### Create the new Model
  set m [ TetraMeshCreateModel $modelbasename$i $LOWSCALAR $HIGHSCALAR]
  set Model($m,polyData) [gf GetOutput]
   puts [ $Model($m,polyData) GetNumberOfPolys]
  $Model($m,polyData) Update
  Model($m,mapper) SetInput $Model($m,polyData)

  ### Get the remaining Data ###
  Thresh ThresholdBetween [ expr { $lowscalar + 0.01} ] $highscalar
  set CurrentTetraMesh [Thresh GetOutput]
  $CurrentTetraMesh Update
  set range [$CurrentTetraMesh GetScalarRange]
  set lowscalar [ lindex $range 0 ]
  set highscalar [ lindex $range 1 ]
  incr i
}

Thresh Delete
gf Delete
tetra_reader Delete

MainModelsUpdateMRML 
MainUpdateMRML
Render3D
}   

#-------------------------------------------------------------------------------
# .PROC TetraMeshCreateModel
#
# This Routine Creates a new model and returns the id.
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC TetraMeshCreateModel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TetraMeshCreateModel  {name scalarLo scalarHi} {
    global Model Mrml Label

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

# set Model($m,fly) 1 ??? I have a feeling it would be good to add this.
# It marks a model as having been created on the fly.
# Mark this model as saved
 set Model($m,dirty) 0

 Model($m,node) SetName $name
 Model($m,node) SetFileName "$Model(prefix).vtk"
 Model($m,node) SetFullFileName [file join $Mrml(dir) [Model($m,node) GetFileName]]
 Model($m,node) SetDescription "Generated from Tetrahedral Mesh"

 MainModelsSetClipping $m 1
 MainModelsSetVisibility $m 1
 MainModelsSetOpacity $m 1
 MainModelsSetColor $m $Label(name)
 MainModelsSetCulling $m 1
 MainModelsSetScalarVisibility $m 1
 MainModelsSetScalarRange $m $scalarLo $scalarHi
 return $m
}
