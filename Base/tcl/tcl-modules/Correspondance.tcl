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
# FILE:        Correspondance.tcl
# PROCEDURES:  
#   CorrespondanceInit
#   CorrespondanceBuildGUI
#   CorrespondanceCreateModelGUI widget int
#   CorrespondanceConfigScrolledGUI canvasScrolledGUI fScrolledGUI
#   CorrespondanceDeleteModelGUI widget int
#   CorrespondanceUpdateGUI
#   CorrespondanceCorrespondSurfaces
#   CorrespondanceCorrespondSurfaces
#   CorrespondanceMatchSurface
#   CorrespondanceMatchSurface
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC CorrespondanceInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceInit {} {
	global Correspondance Module Volume Model

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
	set m Correspondance
	set Module($m,row1List) "Help Correspond"
        set Module($m,row1Name) "{Help} {Correspond}"
	set Module($m,row1,tab) Correspond

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
	#   set Module($m,procVTK) CorrespondanceBuildVTK
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
	set Module($m,procGUI) CorrespondanceBuildGUI
#	set Module($m,procEnter) CorrespondanceEnter
#	set Module($m,procExit) CorrespondanceExit
	set Module($m,procMRML) CorrespondanceUpdateGUI

	# Define Dependencies
	#------------------------------------
	# Description:
	#   Record any other modules that this one depends on.  This is used 
	#   to check that all necessary modules are loaded when Slicer runs.
	#   
	set Module($m,depend) "Data Models TetraMesh"

        # Set version info
	#------------------------------------
	# Description:
	#   Record the version number for display under Help->Version Info.
	#   The strings with the $ symbol tell CVS to automatically insert the
	#   appropriate revision number and date when the module is checked in.
	#   
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.3 $} {$Date: 2002/01/13 22:25:08 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
        set Correspondance(Model1) ""
        set Correspondance(Model2) ""
        set Correspondance(CorSphereScale) 9.5
        set Correspondance(CorSphereSkip) 20
}


#-------------------------------------------------------------------------------
# .PROC CorrespondanceBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceBuildGUI {} {
	global Gui Correspondance Module Volume Model View

	# A frame has already been constructed automatically for each tab.
	# A frame named "Props" can be referenced as follows:
	#   
	#     $Module(<Module name>,f<Tab name>)
	#
	# ie: $Module(Correspondance,fProps)

	# This is a useful comment block that makes reading this easy for all:
	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
        # Correspond
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------

	# Write the "help" in the form of psuedo-html.  
	# Refer to the documentation for details on the syntax.
	#
	set help "
The Correspondance module allows a user to show point correspondances between models.

<UL>
<LI><B>Matching Surfaces:</B> The models are assumed to correspond on a point by point basis. That is point 33 in model 1 corresponds to point 33 in model 2.
</LI>
<LI><B>Known Problems:</B> It overwrites the scalar point data on the models.

</LI></UL>
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Correspondance $help
	MainHelpBuildGUI Correspondance


	#-------------------------------------------
	# Correspond frame
	#-------------------------------------------
	set fCorrespond $Module(Correspondance,fCorrespond)
	set f $fCorrespond

	frame $f.fTop    -bg $Gui(activeWorkspace)
	frame $f.fScroll   -bg $Gui(activeWorkspace)
	frame $f.fMiddle -bg $Gui(activeWorkspace)
	frame $f.fBottom -bg $Gui(activeWorkspace)
	frame $f.fRun    -bg $Gui(activeWorkspace)
	pack $f.fTop $f.fScroll $f.fMiddle $f.fBottom $f.fRun \
                -side top -padx 0 -pady $Gui(pad)

        #-------------------------------------------
        # Correspond->Top frame
        #-------------------------------------------
        set f $fCorrespond.fTop
        DevAddLabel  $f.lCorrespond "Show correspondances between surfaces\n with identical numbers of nodes.\n\n Select Models:"

        pack $f.lCorrespond -side left -padx $Gui(pad)

	set f $fCorrespond.fScroll

	set Correspondance(canvasScrolledGUI)  $f.cGrid
        set Correspondance(fScrolledGUI)       $f.cGrid.fListItems
        DevCreateScrollList $f \
                            CorrespondanceCreateModelGUI \
                            CorrespondanceConfigScrolledGUI \
                            "$Model(idList)"

	#-------------------------------------------
	# Correspond->Middle frame
	#-------------------------------------------

	set f $fCorrespond.fMiddle

        DevAddLabel  $f.lSphereScale "Sphere Scaling"
        eval {entry $f.eSphereScale -textvariable Correspondance(CorSphereScale) -width 5} $Gui(WEA)
   
        pack $f.lSphereScale $f.eSphereScale -side left -padx $Gui(pad)

	#-------------------------------------------
	# Correspond->Bottom frame
	#-------------------------------------------


	set f $fCorrespond.fBottom

        DevAddLabel  $f.lSphereSkip "Keep Every Nth Node:"
        eval {entry $f.eSphereSkip -textvariable Correspondance(CorSphereSkip) -width 5} $Gui(WEA)
   
        pack $f.lSphereSkip $f.eSphereSkip -side left -padx $Gui(pad)

	#-------------------------------------------
	# Correspond->Bottom frame
	#-------------------------------------------

	set f $fCorrespond.fRun

        DevAddButton $f.bRun "Run" "CorrespondanceCorrespondSurfaces"

	pack $f.bRun

}

#-------------------------------------------------------------------------------
# .PROC CorrespondanceCreateModelGUI
# Makes the GUI for each model on the Models->Display panel.
# This is called for each new model.
# Also makes the popup menu that comes up when you right-click a model.
#
# returns 1 if it did anything, 0 otherwise.
#
# .ARGS
# f widget the frame to create the GUI in
# m int the id of the model
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceCreateModelGUI {f m } {
	global Gui Model Color Correspondance 


        # puts "Creating GUI for model $m"		
	# If the GUI already exists, then just change name.
	if {[info command $f.c$m] != ""} {
		$f.c$m config -text "[Model($m,node) GetName]"
		return 0
	}

	# Name / Visible
	set Correspondance($m,match) 0
	eval {checkbutton $f.c$m \
		-text [Model($m,node) GetName] -variable Correspondance($m,match) \
		-width 17 -indicatoron 0} $Gui(WCA)
#        $f.c$m configure -bg [MakeColorNormalized \
#                        [Color($Model($m,colorID),node) GetDiffuseColor]]
#        $f.c$m configure -selectcolor [MakeColorNormalized \
#                        [Color($Model($m,colorID),node) GetDiffuseColor]]
			
	# Add a tool tip if the string is too long for the button
	if {[string length [Model($m,node) GetName]] > [$f.c$m cget -width]} {
		TooltipAdd $f.c$m [Model($m,node) GetName]
	}
	
#	eval grid $l1_command $c_command $f.e${m} $f.s${m} -pady 2 -padx 2 -sticky we

        eval grid $f.c$m -pady 2 -padx 2 -sticky we
	return 1
}

#-------------------------------------------------------------------------------
# .PROC CorrespondanceConfigScrolledGUI
# 
# Set the dimensions of the scrolledGUI
#
# .ARGS
#
# frame  canvasScrolledGUI  The canvas around the scrolled frame
# frame  fScrolledGUI       The frame with the item list of models
# .END   
#-------------------------------------------------------------------------------
proc CorrespondanceConfigScrolledGUI {canvasScrolledGUI fScrolledGUI} {
	global Model ModelGroup RemovedModels

	set f      $fScrolledGUI
	set canvas $canvasScrolledGUI
	set m [lindex $Model(idList) 0]

        # y spacing important for calculation of frame height for scrolling
        set pady 2

	if {$m != ""} {
	    # Find the height of a single button
	    # Must use $f.s$m since the scrollbar ("s") fields are tallest
	    set lastButton $f.c$m
	    # Find how many modules (lines) in the frame
	    set numLines 0
	    foreach m $Model(idList) {
	    	if {$RemovedModels($m) == 0} {
	    		incr numLines
	    	}
	    }
	    incr numLines [llength $ModelGroup(idList)]
	    #set numLines [expr [llength $Model(idList)] + [llength $ModelGroup(idList)]]
	    # Find the height of a line
	    set incr [expr {[winfo reqheight $lastButton] + 2*$pady}]
	    # Find the total height that should scroll
	    set height [expr {$numLines * $incr}]
	    # Find the width of the scrolling region
	    update; 	# wait for some stuff to be done before requesting
	    		# window positions
	    set last_x [winfo x $lastButton]
	    set width [expr $last_x + [winfo reqwidth $lastButton]]
	    $canvas config -scrollregion "0 0 $width $height"
	    $canvas config -yscrollincrement $incr -confine true
	    $canvas config -xscrollincrement 1 -confine true
	}
}

#-------------------------------------------------------------------------------
# .PROC CorrespondanceDeleteModelGUI
# 
# .ARGS
# f widget the frame to create the GUI in
# m int the id of the model
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceDeleteModelGUI {f m} {
    global Correspondance

    # If the GUI is already deleted, return
    if {[info command $f.c$m] == ""} {
        return 0
    }

    # Destroy TK widgets
    destroy $f.c$m

    set Correspondance($m,match) 0;

    return 1
}

#-------------------------------------------------------------------------------
# .PROC CorrespondanceUpdateGUI
# 
# This procedure is called to update the buttons
# due to such things as volumes or models being added or subtracted.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceUpdateGUI {} {
	global Correspondance Model

    set gui 0

    # Now build GUI for any models not in hierarchies
    foreach m $Model(idList) {
        set gui [expr $gui + [CorrespondanceCreateModelGUI \
                               $Correspondance(fScrolledGUI) $m]]
    }

    # Delete the GUI for any old models
    foreach m $Model(idListDelete) {
        set gui [expr $gui + [CorrespondanceDeleteModelGUI \
                  $Correspondance(fScrolledGUI) $m]]
    }

    # Tell the scrollbar to update if the gui height changed
    if {$gui > 0} {
        CorrespondanceConfigScrolledGUI $Correspondance(canvasScrolledGUI) \
                $Correspondance(fScrolledGUI)
    }
}

#-------------------------------------------------------------------------------
# .PROC CorrespondanceCorrespondSurfaces
#
# Call each surface going to be corresponded and call the processing step
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC CorrespondanceCorrespondSurfaces
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceCorrespondSurfaces {} {
    global Correspondance Model Mrml

    ############################################################
    # First check for problems.
    # Make sure all surfaces have the same number of nodes.
    ############################################################

    set NumNode -1;

    foreach m $Model(idList) {
        if {$Correspondance($m,match) == 1} {
            set ThisNumNode [$Model($m,polyData) GetNumberOfPoints]
            if {$NumNode == -1} { set NumNode $ThisNumNode }
            if {$ThisNumNode != $NumNode} {
                DevWarningWindow "Must have same number of nodes in each Model!"
                return 0
            }
        }
    }

    if {$NumNode == -1} {
        DevWarningWindow "No Models Selected!"
        return 0
    }

    ############################################################
    # First check for problems.
    # Make sure all surfaces have the same number of nodes.
    ############################################################

    foreach m $Model(idList) {
        if {$Correspondance($m,match) == 1} {
            set p [CorrespondanceMatchSurface $m]

            ## move the returning node to be after the polyData node
            Mrml(dataTree) RemoveItem Model($p,node)
            Mrml(dataTree) InsertAfterItem Model($m,node) Model($p,node)
        }
    }


  ############################################################
  # Update the displays
  ############################################################

  MainModelsUpdateMRML 
  MainUpdateMRML
  Render3D
}


#-------------------------------------------------------------------------------
# .PROC CorrespondanceMatchSurface
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC CorrespondanceMatchSurface
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorrespondanceMatchSurface {m} {
	global Correspondance Model Volume Module

    set PD  $Model($m,polyData)
    set NumNode [$PD GetNumberOfPoints]

######################################################################
#################### Get the Points Set and put them together
######################################################################

vtkMaskPoints PointSelection
  PointSelection SetInput $PD
  PointSelection SetOnRatio $Correspondance(CorSphereSkip)
  PointSelection RandomModeOff
  PointSelection Update

set PointData     [PointSelection GetOutput]

set NumSelectNode [$PointData GetNumberOfPoints]
set Increment [ expr $NumSelectNode / 2 ]

vtkPolyData tempPolyData
  tempPolyData ShallowCopy [PointSelection GetOutput]
  tempPolyData Print

vtkIntArray seen
  seen SetNumberOfValues $NumSelectNode
  for {set i 0} { $i < $NumSelectNode } {incr i 1} {
    seen SetValue $i 0
  }

puts "Increment: $Increment"
vtkScalars Scalars
set p 0
for {set i 0} { $i < $NumSelectNode } {incr i 1} {
    # increment $p
    set p [expr $p + $Increment]
    if {$p >= $NumSelectNode} {set p [expr $p - $NumSelectNode] }
    while { [seen GetValue $p] == 1 } { 
        incr p  
        if {$p >= $NumSelectNode} {set p [expr $p - $NumSelectNode] }
    }
    Scalars InsertScalar $i [expr $i * $Increment]
    seen SetValue $p 1
    puts "$i $p"
}

seen Delete

[tempPolyData GetPointData] SetScalars Scalars

#
#  PointSelection Update
#  [[PointSelection GetOutput] GetPointData] SetScalars Scalars

vtkSphereSource ASphere
  ASphere SetPhiResolution 5
  ASphere SetThetaResolution 5
  ASphere SetRadius [ expr 0.15 * $Correspondance(CorSphereScale) ]
vtkGlyph3D ScalarGlyph
  ScalarGlyph SetInput  tempPolyData
  ScalarGlyph SetSource [ASphere GetOutput]
  ScalarGlyph SetScaleModeToDataScalingOff
  ScalarGlyph SetColorModeToColorByScalar
#  ScalarGlyph SetScaleModeToScaleByScalar
#  ScalarGlyph SetScaleFactor 
  ScalarGlyph Update

  ########################################
  ##### Form the Model
  ########################################

  set range [tempPolyData GetScalarRange]
  set LOWSCALAR  [ lindex $range 0 ]
  set HIGHSCALAR [ lindex $range 1 ]
  set name [Model($m,node) GetName]
  set name "${name}-correspond-points"

  ### Create the new Model
  set m [ TetraMeshCreateModel $name $LOWSCALAR $HIGHSCALAR ]
  TetraMeshCopyPolyData [ScalarGlyph GetOutput] $m

  PointSelection Delete
  ASphere Delete
  ScalarGlyph Delete
  Scalars Delete
  tempPolyData Delete

  return $m
}
