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
# FILE:        Tensor.tcl
# PROCEDURES:  
#   TensorInit
#   TensorUpdateMRML
#   TensorEnter
#   TensorExit
#   TensorBuildGUI
#   TensorBuildScrolledGUI
#   TensorCheckScrollLimits
#   TensorSizeScrolledGUI
#   TensorSetPropertyType
#   TensorPropsApply
#   TensorPropsCancel
#   TensorAdvancedApply
#   TensorSetFileName
#   TensorSelect
#   TensorSetVisualizationMode
#   TensorApplyVisualizationParameters
#   TensorMakeVTKObject
#   TensorAddObjectProperty
#   TensorBuildVTK
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC TensorInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorInit {} {
    global Tensor Module
    
    # Define Tabs
    #------------------------------------
    set m Tensor
    set Module($m,row1List) "Help Display Props Advanced"
    set Module($m,row1Name) "{Help} {Display} {Props} {Advanced}"
    set Module($m,row1,tab) Display
    # Use these lines to add a second row of tabs
    #	set Module($m,row2List) "Meter"
    #	set Module($m,row2Name) "{Meter}"
    #	set Module($m,row2,tab) Meter
    
    # Define Procedures
    #------------------------------------
    set Module($m,procGUI) TensorBuildGUI
    set Module($m,procMRML) TensorUpdateMRML
    set Module($m,procVTK) TensorBuildVTK
    set Module($m,procEnter) TensorEnter
    set Module($m,procExit) TensorExit
   
    # Define Dependencies
    #------------------------------------
    set Module($m,depend) ""
    
    # Set Version Info
    #------------------------------------
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.1 $} {$Date: 2001/07/20 14:38:13 $}]
    
    # Props: GUI tab we are currently on
    #------------------------------------
    set Tensor(propertyType) Basic
    
    # Initial path to search when loading files
    #------------------------------------
    set Tensor(DefaultDir) ""
    # Lauren for testing only
    set Tensor(DefaultDir) "/export/home/odonnell/data/"

    # Lauren: should vis be in tcl or C++?
    # Visualization-related variables
    #------------------------------------
    set Tensor(glyphMode) Axes
    set Tensor(glyphModeList) {Axes Ellipses}
    set Tensor(visualizationMode) None
    set Tensor(visualizationModeList) {None Glyphs Tracks}

    # Event bindings
    #------------------------------------
    lappend Tensor(eventManager) {$Gui(fViewWin)  <KeyPress-l> \
	    { if { [SelectPick Tensor(vtk,picker) %W %x %y] != 0 } \
	    { eval TensorSelect $Select(xyz);Render3D } } }
}

################################################################
#  Procedures called automatically by the slicer
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorUpdateMRML {} {
    puts "Lauren in TensorUpdateMRML"
}

#-------------------------------------------------------------------------------
# .PROC TensorEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorEnter {} {
    global Tensor
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $Tensor(eventManager)

    # configure all scrolled GUIs so frames inside fit
    TensorSizeScrolledGUI $Tensor(scrolledGui,advanced)
}

#-------------------------------------------------------------------------------
# .PROC TensorExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

################################################################
#  Procedures for building the GUI
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorBuildGUI
# Builds the GUI panel.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildGUI {} {
    global Module Gui Tensor

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    # Props
    # Advanced
    #-------------------------------------------

    puts "Lauren in TensorBuildGUI, fix the frame hierarchy comment"

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    This module sucks
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Basic:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags Tensor $help
    MainHelpBuildGUI Tensor
    
    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(Tensor,fDisplay)
    set f $fDisplay

    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Display->Top frame
    #-------------------------------------------
    set f $fDisplay.fTop
    
    #       grid $f.lDisplay -padx $Gui(pad) -pady $Gui(pad)
    #       grid $menubutton -sticky w
    
    # Add a menu that lists volumes
    # for future reformatting of tensors along with this volume
    DevAddSelectButton  Tensor $f Volume1 "Grayscale Volume" Grid
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbVolume1
    lappend Volume(mActiveList) $f.mbVolume1.m
    
    #-------------------------------------------
    # Display->Middle frame
    #-------------------------------------------
    set f $fDisplay.fMiddle
    eval {label $f.lVis -text "Visualization Method: "} $Gui(BLA)
    eval {menubutton $f.mbVis -text $Tensor(visualizationMode) \
	    -relief raised -bd 2 -width 20 \
	    -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $Tensor(visualizationModeList) {
	$f.mbVis.m add command -label $vis \
		-command "TensorSetVisualizationMode $vis"
    }
    # save menubutton for config
    set Tensor(gui,mbVisMode) $f.mbVis

    #-------------------------------------------
    # Display->Bottom frame
    #-------------------------------------------
    set f $fDisplay.fBottom
    foreach vis $Tensor(glyphModeList) {
	eval {radiobutton $f.rMode$vis \
		-text "$vis" -value "$vis" \
		-variable Tensor(glyphMode) \
		-command {TensorSetVisualizationMode $Tensor(visualizationMode)} \
		-indicatoron 0} $Gui(WCA)
	pack $f.rMode$vis -side left -padx 0 -pady 0
    }
    
    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(Tensor,fProps)
    set f $fProps
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Props->Top frame
    #-------------------------------------------
    set f $fProps.fTop
    # file browse box: widget, array, var, label, proc, "", startdir
    DevAddFileBrowse $f Tensor FileName "File" \
	    TensorSetFileName "" {\$Tensor(defaultDir)}
    # Lauren does default dir work?

    #-------------------------------------------
    # Props->Bottom frame
    #-------------------------------------------
    set f $fProps.fBottom
    DevAddButton $f.bApply "Apply" "TensorPropsApply; Render3D" 8
    DevAddButton $f.bCancel "Cancel" "TensorPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)



    #-------------------------------------------
    # Advanced frame
    #-------------------------------------------
    set fAdvanced $Module(Tensor,fAdvanced)
    set f $fAdvanced
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Advanced->Top frame
    #-------------------------------------------
    set f $fAdvanced.fTop
    DevAddLabel $f.l "VTK objects in the pipeline"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Advanced->Middle frame
    #-------------------------------------------
    set f $fAdvanced.fMiddle
    set fScrolled [TensorBuildScrolledGUI $f]
    set Tensor(scrolledGui,advanced) $fScrolled

    # loop through all the vtk objects and build GUIs
    #------------------------------------
    foreach o $Tensor(vtkObjectList) {

	set f $fScrolled

	# if the object has properties
	#-------------------------------------------
	if {$Tensor(vtkPropertyList,$o) != ""} {
	    
	    # make a new frame for this vtk object
	    #-------------------------------------------
	    frame $f.f$o -bg $Gui(activeWorkspace)
	    $f.f$o configure  -relief groove -bd 3 
	    pack $f.f$o -side top -padx $Gui(pad) -pady 2 -fill x
	    
	    # object name becomes the label for the frame
	    #-------------------------------------------
	    # Lauren we need an object description
	    # and also basic or advanced attrib
	    DevAddLabel $f.f$o.l$o [Cap $o]
	    pack $f.f$o.l$o -side top \
		    -padx 0 -pady 0
	}

	# loop through all the parameters for this object
	# and build appropriate user entry stuff for each
	#------------------------------------
	foreach p $Tensor(vtkPropertyList,$o) {

	    set f $fScrolled.f$o

	    # name of entire tcl variable
	    set variableName Tensor(vtk,$o,$p)
	    # its value is:
	    set value $Tensor(vtk,$o,$p)
	    # description of the parameter
	    set desc $Tensor(vtk,$o,$p,description)
	    # datatype of the parameter
	    set type $Tensor(vtk,$o,$p,type)

	    # make a new frame for this parameter
	    frame $f.f$p -bg $Gui(activeWorkspace)
	    pack $f.f$p -side top -padx 0 -pady 1 -fill x
	    set f $f.f$p

	    # see if value is a list
	    #------------------------------------		
	    set length [llength $value]
	    set isList [expr $length > "1"]

	    # Lauren we need to handle lists
	    #------------------------------------		
	    if {$isList == "0"} {
		switch $type {
		    "int" {
			eval {entry $f.e$p \
				-width 5 \
				-textvariable $variableName\
			    } $Gui(WEA)
			DevAddLabel $f.l$p $desc:
			pack $f.l$p $f.e$p -side left \
				-padx $Gui(pad) -pady 2
		    }
		    "float" {
			eval {entry $f.e$p \
				-width 5 \
				-textvariable $variableName\
			    } $Gui(WEA)
			DevAddLabel $f.l$p $desc:
			pack $f.l$p $f.e$p -side left \
				-padx $Gui(pad) -pady 2
		    }
		    "bool" {
			puts "bool: $variableName, $desc"
			eval {checkbutton $f.r$p  \
				-text $desc -variable $variableName \
			    } $Gui(WCA)
			
			#DevAddLabel $f.l$p $desc:
			pack  $f.r$p -side left \
				-padx $Gui(pad) -pady 2
		    }
		}
	    } else {
		puts "Lauren make this junk handle lists"
	    }
	    
	}
    }
    # end foreach vtk object in tensors object list

    # figure out how tall a row is in the above gui, to scroll
    # Lauren you are here
    #set Tensor(advancedScrolledGui,height) [winfo reqheight $fScrolled.f]

    # Here's a button with text "Apply" that calls "AdvancedApply"
    DevAddButton $fAdvanced.fMiddle.bApply Apply TensorAdvancedApply
    pack $fAdvanced.fMiddle.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    
 
}


#-------------------------------------------------------------------------------
# .PROC TensorBuildScrolledGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildScrolledGUI {f} {
    global Gui Tensor

    # window paths
    #-------------------------------------------
    set canvas $f.cGrid
    set s $f.sGrid
    
    # create canvas and scrollbar
    #-------------------------------------------
    canvas $canvas -yscrollcommand "$s set" -bg $Gui(activeWorkspace)
    eval "scrollbar $s -command \"TensorCheckScrollLimits $canvas yview\"\
	    $Gui(WSBA)"
    pack $s -side right -fill y
    pack $canvas -side top -fill both -pady $Gui(pad) -expand true
    
    set f $canvas.f
    #frame $f -bd 0 -bg $Gui(activeWorkspace)
    frame $f -bd 0
    $f configure  -relief groove -bd 3 
    
    # put the frame inside the canvas (so it can scroll)
    #-------------------------------------------
    $canvas create window 0 0 -anchor nw -window $f
    
    # y spacing important for calculation of frame height for scrolling
    set pady 2
    
    # save name of canvas to configure later
    set Tensor(scrolledGUI,$f,canvas) $canvas

    # return path to the frame for filling
    return $f
}


#-------------------------------------------------------------------------------
# .PROC TensorCheckScrollLimits
# This procedure allows scrolling only if the entire frame is not visible
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorCheckScrollLimits {args} {

    set canvas [lindex $args 0]
    set view   [lindex $args 1]
    set fracs [$canvas $view]

    if {double([lindex $fracs 0]) == 0.0 && \
	    double([lindex $fracs 1]) == 1.0} {
	return
    }
    eval $args
}

#-------------------------------------------------------------------------------
# .PROC TensorSizeScrolledGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSizeScrolledGUI {f} {
    global Tensor

    # see how tall our frame is these days
    set height [winfo reqheight  $f]    
    set canvas $Tensor(scrolledGUI,$f,canvas)

    # tell the canvas to scroll so we can see the whole frame
    $canvas config -scrollregion "0 0 1 $height"
}

#-------------------------------------------------------------------------------
# .PROC TensorSetPropertyType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSetPropertyType {} {
    global Module Tensor

    #Lauren need properties subframes for this to work
    #raise $Volume(f$Volume(propertyType))
    #focus $Volume(f$Volume(propertyType))

    # Lauren temporarily use this
    raise $Module(Tensor,f$Tensor(propertyType))
}

################################################################
#  Procedures called by the GUI: Apply, Cancel, etc.
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorPropsApply
#  Apply the settings from the GUI into the currently
#  active MRML node (this is NEW or an existing node).  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorPropsApply {} {
    global Tensor Module
    puts "Lauren in TensorPropsApply"
    
    set d $Tensor(activeID)
    if {$d == ""} {return}

    # Check all user input
    #-------------------------------------------
    set errcode [MainTensorValidateUserInput]
    if {$errcode != "1"} {
	return $errcode
    }

    # If we are in the process of adding a new data object
    #--------------------------------------------------------
    if {$d == "NEW"} {

	# Create the node
	set n [MainMrmlAddNode Tensor]
	set i [$n GetID]

	# Set everything up in the node.
	MainTensorSetAllVariablesToNode $i

	# Update MRML (this will read in the new data)
	MainUpdateMRML
        # If failed, then it's no longer in the idList
	# Lauren this is a bug that the user is not informed?
	# And we stay frozen...
        if {[lsearch $Tensor(idList) $i] == -1} {
	    puts "Lauren node doesn't exist, should unfreeze and fix volumes.tcltoo"
            return
        }

	# Lauren what is this for?
        set Tensor(freeze) 0

	# Activate the new data object
        MainDataSetActive Tensor $i

    } else {
	# do we do a re-read of the object if user changed certain things?
	# Lauren implement this?
	#MainTensorSetAllVariablesToNode
    }

    # Apply normal settings changed by user (no file reread needed)
    #--------------------------------------------------------
    # Lauren these are only things that can be changed,
    # need to sort all vars into change/no change categories
    # OR just need to do a reread after if certain things changed.
    #MainTensorSetAllVariablesToNode "update"

    # If we were adding a NEW object, unfreeze tabs now
    #--------------------------------------------------------
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }

    # Lauren do we need this?
    # Update pipeline
    #MainVolumesUpdate $m

    # Update MRML to apply changed settings
    #--------------------------------------------------------
    MainUpdateMRML
    puts "STILL ALIVE!!!"
}

#-------------------------------------------------------------------------------
# .PROC TensorPropsCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorPropsCancel {} {
    global Tensor
    puts "Lauren in TensorPropsCancel: need to get out of freezer"

    set Tensor(freeze) 0
    # If we gave up on adding a NEW object, unfreeze tabs now
    #--------------------------------------------------------
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC TensorAdvancedApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorAdvancedApply {} {
    global Tensor    
    puts "Applying"
    TensorApplyVisualizationParameters    
    puts "done applying"
    
    # display our changes
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorSetFileName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSetFileName {} {
    global Tensor
    
    # Do nothing if the user cancelled
    if {$Tensor(FileName) == ""} {return}
    
    # Update the Default Directory
    set Tensor(DefaultDir) [file dirname $Tensor(FileName)]
    
    # Name the tensors based on the entered file.
    set Tensor(Name) [ file root [file tail $Tensor(FileName)]]
    puts "Lauren handle naming $Tensor(FileName), $Tensor(Name)"
}


################################################################
#  Procedures to handle visualization interaction
################################################################

#-------------------------------------------------------------------------------
# .PROC TensorSelect
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSelect {x y z} {
    global Tensor

    puts "Tensor Select: x y z $x $y $z"

    # start hyperstreamline here
    Tensor(vtk,streamln) SetStartPosition $x $y $z

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC TensorSetVisualizationMode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorSetVisualizationMode {mode} {
    global Tensor

    set t $Tensor(activeID)
    if {$t == $Tensor(idNone)} {
	puts "TensorSetVisualizationMode: Can't visualize Nothing"
	set mode None
    }

    set source [Tensor($t,data) GetOutput]

    # if mode is valid
    set valid 0
    
    switch $mode {
	"Glyphs" {
	    puts "glyphs! $Tensor(glyphMode)"

	    # start pipeline
	    #------------------------------------
	    Tensor(vtk,glyphs) SetInput $source

	    # Use axes or ellipses
	    #------------------------------------
	    switch $Tensor(glyphMode) {
		"Axes" {
		    Tensor(vtk,glyphs) SetSource \
			    [Tensor(vtk,glyphs,axes) GetOutput]
		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs) GetOutput]
		}
		"Ellipses" {
		    Tensor(vtk,glyphs) SetSource \
			    [Tensor(vtk,glyphs,sphere) GetOutput]
		    # way too expensive?
		    #Tensor(vtk,glyphs,mapper) SetInput \
			#    [Tensor(vtk,glyphs,normals) GetOutput]
		    Tensor(vtk,glyphs,mapper) SetInput \
			    [Tensor(vtk,glyphs) GetOutput]
		}
	    }

	    # Add our actors to the renderers
	    #------------------------------------
	    MainAddActor Tensor(vtk,glyphs,actor)
	    
	    set valid 1
	}
	"Tracks" {

	    puts "tracks!"

	    # start pipeline
	    #------------------------------------
	    Tensor(vtk,streamln) SetInput $source

	    # Add our actors to the renderers
	    #------------------------------------
	    MainAddActor Tensor(vtk,streamln,actor)

	    set valid 1
	}
	"None" {
	    puts "Turning off tensor visualization"

	    # disconnect from renderer so output
	    # not requested from pipeline anymore
	    #------------------------------------
	    MainRemoveActor Tensor(vtk,glyphs,actor)
	    MainRemoveActor Tensor(vtk,streamln,actor)
	    set valid 1
	}
    }

    # if we encountered a valid mode, update menubutton
    if {$valid == "1"} {
	set Tensor(visualizationMode) $mode
	
	# config menubutton
	$Tensor(gui,mbVisMode)	config -text $mode

	# update 3D window
	Render3D
    }
}

################################################################
#  Procedures to set up pipelines
#  Lauren: try to create objects only if needed!
#  Should this be in a vtk class?
################################################################


#-------------------------------------------------------------------------------
# .PROC TensorApplyVisualizationParameters
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorApplyVisualizationParameters {} {
    global Tensor

    # this code actually makes a bunch of calls like the following:
    # Tensor(vtk,axes) SetScaleFactor $Tensor(vtk,axes,scaleFactor)
    # Tensor(vtk,tubeAxes) SetRadius $Tensor(vtk,tubeAxes,radius)
    # Tensor(vtk,glyphs) SetClampScaling 1
    # we can't do calls like MyObject MyVariableOn now

    # our naming convention is:
    # Tensor(vtk,object)  is the name of the object
    # paramName is the name of the parameter
    # $Tensor(vtk,object,paramName) is the value 

    # Lauren we need to validate too!!!!!!!!
    #  too bad vtk can't return a string type desrciptor...

    # loop through all vtk objects
    #------------------------------------
    foreach o $Tensor(vtkObjectList) {

	# loop through all parameters of the object
	#------------------------------------
	foreach p $Tensor(vtkPropertyList,$o) {

	    # value of the parameter is $Tensor(vtk,$o,$p)
	    #------------------------------------
	    set value $Tensor(vtk,$o,$p)
	    
	    # Actually set the value appropriately in the vtk object
	    #------------------------------------	

	    # first capitalize the parameter name
	    set param [Cap $p]
	    
	    # MyObject SetMyParameter $value	
	    # handle the case in which value is a list with an eval 
	    # this puts it into the correct format for feeding to vtk
	    eval {Tensor(vtk,$o) Set$param} $value
	}
    }

}

#-------------------------------------------------------------------------------
# .PROC TensorMakeVTKObject
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorMakeVTKObject {class object} {
    global Tensor

    # make the object
    #------------------------------------
    $class Tensor(vtk,$object)

    # save on list for updating of its variables by user
    #------------------------------------
    lappend Tensor(vtkObjectList) $object

    # initialize list of its variables
    #------------------------------------
    set Tensor(vtkPropertyList,$object) ""
}

#-------------------------------------------------------------------------------
# .PROC TensorAddObjectProperty
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorAddObjectProperty {object parameter value type desc} {
    global Tensor

    # create tcl variables of the form:
    #set Tensor(vtk,tubeAxes,numberOfSides) 6
    #set Tensor(vtk,tubeAxes,numberOfSides,type) int
    #set Tensor(vtk,tubeAxes,numberOfSides,description) 
    #lappend Tensor(vtkPropertyList,tubeAxes) numberOfSides 


    # make tcl variable and save its attributes (type, desc)
    #------------------------------------
    set param [Uncap $parameter]
    set Tensor(vtk,$object,$param) $value
    set Tensor(vtk,$object,$param,type) $type
    set Tensor(vtk,$object,$param,description) $desc

    # save on list for updating variable by user
    #------------------------------------
    lappend Tensor(vtkPropertyList,$object) $param
    
}

#-------------------------------------------------------------------------------
# .PROC TensorBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TensorBuildVTK {} {
    global Tensor

    # Lauren: maybe this module should kill its objects on exit.
    # We must be hogging a ton of memory all over the slicer.
    # So possibly build (some) VTK upon entering module.

    # User interaction objects
    #------------------------------------
    # Lauren: doing this like Endoscopic (this vs PointPicker?)
    vtkCellPicker Tensor(vtk,picker)
    

    #---------------------------------------------------------------
    # Pipeline for display of glyphs
    #---------------------------------------------------------------

    # objects for creation of polydata glyphs
    #------------------------------------

    # Axes
    set object glyphs,axes
    TensorMakeVTKObject vtkAxes $object
    TensorAddObjectProperty $object ScaleFactor 0.2 float {Scale Factor}

    set object glyphs,tubeAxes
    TensorMakeVTKObject vtkTubeFilter $object
    Tensor(vtk,$object) SetInput [Tensor(vtk,glyphs,axes) GetOutput]
    TensorAddObjectProperty $object Radius 0.1 float {Radius}
    TensorAddObjectProperty $object NumberOfSides 6 int \
	    {Number Of Sides}

    # Ellipsoids
    set object glyphs,sphere
    TensorMakeVTKObject vtkSphereSource  $object
    TensorAddObjectProperty $object ThetaResolution 1 int ThetaResolution
    TensorAddObjectProperty $object PhiResolution 1 int PhiResolution

    # objects for placement of glyphs in dataset
    #------------------------------------
    set object glyphs
    TensorMakeVTKObject vtkTensorGlyph $object
    #Tensor(vtk,glyphs) SetSource [Tensor(vtk,glyphs,axes) GetOutput]
    #Tensor(vtk,glyphs) SetSource [Tensor(vtk,glyphs,sphere) GetOutput]
    TensorAddObjectProperty $object ScaleFactor 2 float {Scale Factor}
    TensorAddObjectProperty $object ClampScaling 1 bool {Clamp Scaling}
    TensorAddObjectProperty $object ExtractEigenvalues 1 bool {Extract Eigenvalues}

    # poly data normals filter cleans up polydata for nice display
    # use this for ellipses only
    #------------------------------------
    set object glyphs,normals
    TensorMakeVTKObject vtkPolyDataNormals $object
    Tensor(vtk,$object) SetInput [Tensor(vtk,glyphs) GetOutput]

    # Display of tensor glyphs: LUT and Mapper
    #------------------------------------
    set object glyphs,lut
    TensorMakeVTKObject vtkLogLookupTable $object
    #    TensorAddObjectProperty $object HueRange \
	    #	    {.6667 0.0} float {Hue Range}

    # mapper
    set object glyphs,mapper
    TensorMakeVTKObject vtkPolyDataMapper $object
    #Tensor(vtk,glyphs,mapper) SetInput [Tensor(vtk,glyphs) GetOutput]
    #Tensor(vtk,glyphs,mapper) SetInput [Tensor(vtk,glyphs,normals) GetOutput]
    Tensor(vtk,glyphs,mapper) SetLookupTable Tensor(vtk,glyphs,lut)
    TensorAddObjectProperty $object ImmediateModeRendering \
	    1 bool {Immediate Mode Rendering}    

    # Lauren fix this!!!!! (copied from a tcl example)
    #eval Tensor(vtk,glyphs,mapper)  SetScalarRange 0.0415224 0.784456 

    # Display of tensor glyphs: Actor
    #------------------------------------
    set object glyphs,actor
    TensorMakeVTKObject vtkActor $object
    Tensor(vtk,glyphs,actor) SetMapper Tensor(vtk,glyphs,mapper)
    [Tensor(vtk,glyphs,actor) GetProperty] SetAmbient 1
    [Tensor(vtk,glyphs,actor) GetProperty] SetDiffuse 0


    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    set object streamln
    TensorMakeVTKObject vtkHyperStreamline $object
    
    Tensor(vtk,$object) SetStartMethod      MainStartProgress
    Tensor(vtk,$object) SetProgressMethod  "MainShowProgress Tensor(vtk,$object)"
    Tensor(vtk,$object) SetEndMethod        MainEndProgress

    TensorAddObjectProperty $object StartPosition {9 9 -9} float {Start Pos}
    #TensorAddObjectProperty $object IntegrateMinorEigenvector \
	    #1 bool IntegrateMinorEv
    TensorAddObjectProperty $object MaximumPropagationDistance 18.0 \
	    float {Max Propagation Distance}
    TensorAddObjectProperty $object IntegrationStepLength 0.1 \
	    float {Integration Step Length}
    TensorAddObjectProperty $object StepLength 0.01 \
	    float {Step Length}	    
    TensorAddObjectProperty $object Radius 0.25 \
	    float {Radius}
    TensorAddObjectProperty $object  NumberOfSides 18 \
	    int {Number of Sides}
    TensorAddObjectProperty $object  IntegrationDirection 2 \
	    int {Integration Direction}
    
    # Display of tensor streamline: LUT and Mapper
    #------------------------------------
    set object streamln,lut
    TensorMakeVTKObject vtkLogLookupTable $object
#    TensorAddObjectProperty $object HueRange \
#	    {.6667 0.0} float {Hue Range}

    set object streamln,mapper
    TensorMakeVTKObject vtkPolyDataMapper $object
    Tensor(vtk,streamln,mapper) SetInput [Tensor(vtk,streamln) GetOutput]
    Tensor(vtk,streamln,mapper) SetLookupTable Tensor(vtk,streamln,lut)
    TensorAddObjectProperty $object ImmediateModeRendering \
	    1 bool {Immediate Mode Rendering}    

    # Lauren gross hack (????????????????)
    # this kills negative values in the tensors like 
    # in TenEllip.tcl in graphics
    eval Tensor(vtk,streamln,mapper)  SetScalarRange 0.0415224 0.784456 

    # Display of tensor streamline: Actor
    #------------------------------------
    set object streamln,actor
    TensorMakeVTKObject vtkActor $object
    Tensor(vtk,streamln,actor) SetMapper Tensor(vtk,streamln,mapper)
    [Tensor(vtk,streamln,actor) GetProperty] SetAmbient 1
    [Tensor(vtk,streamln,actor) GetProperty] SetDiffuse 0

    #---------------------------------------------------------------
    # Pipeline for display of 2D tensor info in slice
    #---------------------------------------------------------------
    
    # Lauren how should reformatting be hooked into regular
    # slicer slice reformatting?  Ideally want to follow
    # the 3 slices.

    # Apply all settings from tcl variables that were
    # created above using calls to TensorAddObjectProperty
    #------------------------------------
    TensorApplyVisualizationParameters

}
