# Measure.tcl
# 10/16/98 Peter C. Everett peverett@bwh.harvard.edu: Created
# 12/19/01 Krishna C. Yeshwant kcy@bwh.harvard.edu: Edited

#===============================================================================
# FILE:        Measure.tcl
# PROCEDURES:  
#   MeasureInit
#   MeasureBuildGUI
#   MeasureBuildVTK
#   MeasureEnter
#   MeasureExit
#   MeasureUpdateGUI
#==========================================================================auto=

#------------
# MeasureInit
#------------

proc MeasureInit {} {
    global Measure Module Model
# Point
    
    # Definine Tabs
    set m Measure
    set Module($m,row1List) "Help Measure"
    set Module($m,row1Name) "{Help} {Measure}"
    set Module($m,row1,tab) Measure

    # Define Procedures
    set Module($m,procGUI) MeasureBuildGUI
#    set Module($m,procMRML) MeasureUpdateGUI
    set Module($m,procVTK) MeasureBuildVTK
    set Module($m,procEnter) MeasureEnter
    set Module($m,procExit) MeasureExit

    # Define Dependencies
    set Module($m,depend) ""

    # Set Version Info
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.1.2.1 $} {$Date: 2002/03/01 00:35:33 $}]

    # Initialize module-level variables
#    set Measure(Model1) $Model(idNone)
    array set Point {scale 5.0 selected {} \
	    vtkPoints "Point(vtkPoints)" \
	    vtkScalars "Point(vtkScalars)" -1,name "<None>"}
    # Event Bindings
    set Measure(eventManager) { \
	    {$Gui(fViewWin) <Control-1> {addGlyphPoint %W %x %y}} \
	    {$Gui(fViewWin) <KeyPress-p> {addGlyphPoint %W %x %y}} \
	    {$Gui(fViewWin) <KeyPress-d> {delGlyphPoint %W %x %y}} \
	    {$Gui(fViewWin) <Control-2> {selGlyphPoint %W %x %y}} \
	    {$Gui(fViewWin) <KeyPress-c> {ExtractComponent %W %x %y}} \	    
	    {$Gui(fViewWin) <KeyPress-q> {selGlyphPoint %W %x %y}} }    
}


proc Test {event W X Y x y t } {
#    puts "Button 1 pressed"
}




#----------------
# MeasureBuildGUI
#----------------

proc MeasureBuildGUI {} {
	global Gui Measure Module 
# Point Csys MeasureEventMgr
# Tabs viewRen

#	array set MeasureEventMgr [subst { \
#		$Gui(fViewWin),<Enter> {focus %W} \
#		$Gui(fViewWin),<Control-1> {addGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<KeyPress-p> {addGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<KeyPress-c> {ExtractComponent %W %x %y} \
#		$Gui(fSl0Win),<KeyPress-p> {addGlyphPoint2D %W 0 %x %y} \
#		$Gui(fSl1Win),<KeyPress-p> {addGlyphPoint2D %W 1 %x %y} \
#		$Gui(fSl2Win),<KeyPress-p> {addGlyphPoint2D %W 2 %x %y} \
#		$Gui(fViewWin),<Shift-Control-1> {selGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<Control-2> {selGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<KeyPress-q> {selGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<Control-3> {delGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<KeyPress-d> {delGlyphPoint %W %x %y} \
#		$Gui(fViewWin),<Control-B1-Motion> {set noop 0} \
#		$Gui(fViewWin),<Control-B2-Motion> {set noop 0} \
#		$Gui(fViewWin),<Control-B3-Motion> {set noop 0} } ]

#	DebugMsg "MeasureBuildGUI"

#-----------------------------------------
    # Frame Hierarchy:
#-----------------------------------------
# Help
# Measure
#   Top
#   Middle
#   Bottom
#-----------------------------------------

    #-----------------------------------------
    # Help frame
    #-----------------------------------------

    set help "
    These measuring tools can be used to calculate Surface Area, Volume, and Cross Sectional Area."  
    MainHelpApplyTags Measure $help
    MainHelpBuildGUI Measure


    #-----------------------------------------
    # Measure frame
    #-----------------------------------------
    set fMeasure $Module(Measure,fMeasure)
    set f $fMeasure

    # initialize Top and Bottom frames
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-----------------------------------------
    # Measure->Top frame
    #-----------------------------------------
    TopFrameBuildGUI

    #-----------------------------------------
    # Measure->Middle frame
    #-----------------------------------------    
#    SelectModelGUI $f 1
    MiddleFrameBuildGUI
#    DevAddSelectButton Measure $f Model1 "Models:" Grid
#    lappend Model(mbActiveList) $f.mbModel1
#    lappend Model(mActiveList) $f.mbModel1.m

}


proc TopFrameBuildGUI {} {
    global Module Gui Measure
    
    set f $Module(Measure,fMeasure).fTop
    
    # Add measurement tool selections
    eval {menubutton $f.mb -menu $f.mb.menu -text "Measure" \
	    -relief raised -bd 2 -width 9} $Gui(WMBA)
    eval {menu $f.mb.menu -tearoff 0} $Gui(WMA)

    foreach item { Position Distance Angle Intersection Volume Surface } {
	$f.mb.menu add command -label $item -command Measure$item
    }

    pack $f -side top -fill x -padx $Gui(pad) -pady $Gui(pad)
    pack $f.mb -in $f -side top

    
    # Add measurement output textbox
    frame $f.output -bg $Gui(activeWorkspace)
    set Measure(listbox) \
	    [ScrolledListbox $f.output.lb 0 0 -width 40 -height 7]

    pack $f.output -side top -fill x -in $f
    pack $f.output.lb -side left -fill x -pady $Gui(pad) -in $f.output


    # Add clear button for output textbox
    set tmp {$foo}
    set c { button $f.clear -text "Clear Output" -bd 2 -width 13 \
	    $Gui(WBA) -command {foreach foo {$Measure(listbox)} {$tmp delete 0 end}} } ; eval [subst $c]    
    
    pack $f.clear -side top -pady $Gui(pad) -in $f

    # Add Csys visibility button
    set c { checkbutton $f.cCsys -text "Csys" -variable Csys(visible) \
                -width 6 -indicatoron 0 -command {CsysSetVisibility} $Gui(WCA) }
    eval [subst $c]
    pack $f.cCsys -side bottom -pady $Gui(pad) -in $f

}



proc MiddleFrameBuildGUI {} {
    global Module

    BuildModelSelector $Module(Measure,fMeasure)
#    UpdateModelSelector $Module(Measure,fMeasure).fSelModel.mb.m 1
}


# places a model selector in {$fRoot}.fSelModel
# with structure:
# fRoot
#   fSelModel : Selector containing frame
#     .l      : "Models:" label
#     .mb     : menubutton labeled "0-Selected" initially
#       .m    : menu of models
#     .bPick  : button labeled "Pick..."
proc BuildModelSelector {fRoot} {
    global Gui Selected Model Module
    
    set f $fRoot.fSelModel
    
    frame $f -bg $Gui(activeWorkspace)
    set c { label $f.l -text "Models:" $Gui(WLA) }
    eval [subst $c]
    pack $f.l -side left
    set c { menubutton $f.mb -text "0-Selected" \
	    -relief raised -bd 2 -width 15 \
	    -menu $f.mb.m $Gui(WMBA) }
    eval [subst $c]
    set c { menu $f.mb.m $Gui(WMA) }
    eval [subst $c]
    pack $f.mb -side left
#    set c { button $f.bPick -command "SelectPickModel $f" \
	    -width 8 -bd 2 -text "Pick..." $Gui(WBA) }
#    eval [subst $c]
#    pack $f.bPick -side left
    pack $f -side top

#    puts "test"
#    puts $f.mb.m
#    puts "done test"
}

# updates model selector passed in
proc UpdateModelSelector {fRoot} {
    global Model Measure Module
    global Mrml(dataTree)

    set m $fRoot.fSelModel.mb.m
    
    $m delete 0 end
    
    # fill $f.mb.m (i.e. $m) menu with existing Models
    # not sure if this is correct, only use 1st node of Mrml(dataTree)?
    set numModels [Mrml(dataTree) GetNumberOfModels]

    for {set i 0} {$i < $numModels} {incr i} {
	set currModel [Mrml(dataTree) GetNthModel $i]
	set currID    [string trimleft [$currModel GetModelID] M]

	set r  [lindex $Module(Renderers) 0]

	if { [info exists Model($currID,selected)] == 0 } {
	    set Model($currID,selected) 0		
	}
	
	if { [Model($currID,actor,$r) GetVisibility] } {
	    set state normal
	} else {
	    set state disabled
	}
	$m add checkbutton -label [$currModel GetName] \
		-command "SelectModel $fRoot $currID" \
		-variable Model($currID,selected) -state $state
    }   
    
    $m add command -label "<None>" -command "SelectModel $fRoot -1"
}





# Selects the model with id of second parameter and updates
# fSelModel button in fRoot with number of models selected
proc SelectModel { fRoot id } {
    global Selected Model Module

    set index [lsearch $Selected(Model) $id]
	
    set r  [lindex $Module(Renderers) 0]
    if { $id == -1 } {
	# Unselect ALL
	foreach v $Selected(Model) {
	    set Model($v,selected) 0
	    [Model($v,actor,$r) GetProperty] SetAmbient 0
	}
	set Selected(Model) ""
    } elseif { $index > -1 } {
	# Remove from selection list
	set Selected(Model) [lreplace $Selected(Model) $index $index]
	set Model($id,selected) 0
	[Model($id,actor,$r) GetProperty] SetAmbient 0
    } else {
	# Add to selection list
	lappend Selected(Model) $id
	set Model($id,selected) 1
	[Model($id,actor,$r) GetProperty] SetAmbientColor 1 0 0
	[Model($id,actor,$r) GetProperty] SetAmbient .6
    }
    
    set num [llength $Selected(Model)]
    $fRoot.fSelModel.mb config -text "$num-Selected"
    if { $num } {
	$fRoot.fSelModel.mb config -fg #ff0000
	$fRoot.fSelModel.mb config -activeforeground #ff0000
    } else {
	$fRoot.fSelModel.mb config -fg #000000
	$fRoot.fSelModel.mb config -activeforeground #000000
    }
    
    MeasureRefreshGUI
    Render3D
}


proc MeasureRefreshGUI {} {
    global Module

#    DebugMsg "MeasureRefreshGUI"
    UpdateModelSelector $Module(Measure,fMeasure)
}


proc MeasureBuildVTK {} {
    global Measure Csys
   
    PointsBuildVTK
    set Point(picker) [vtkCellPicker Point(picker)]
    Point(picker) SetTolerance 0.001

    CsysCreate -1 -1 -1
    set Csys(visible) 0
    viewRen AddActor Csys(actor)
#    Csys(actor) SetVisibility 1
    Csys(actor) SetVisibility $Csys(visible)
   
    # Initialize cutter (vtkPolyBoolean.h -> operation 4 -> A_TOUCHES_B)
    set Measure(cutter)  [vtkPolyBoolean Measure(cutter)]
    set Measure(filterA) [vtkTriangleFilter Measure(filterA)]
    set Measure(filterB) [vtkTriangleFilter Measure(filterB)]
#    set Measure(filterC) [vtkTriangleFilter Measure(filterC)]
    set Measure(cutResult) [vtkPolyData Measure(cutResult)]
    Measure(cutter) SetOperation 2
    Measure(cutter) SetInput [Measure(filterA) GetOutput]
    Measure(cutter) SetPolyDataB [Measure(filterB) GetOutput]
    
#    set Measure(surfProps) [vtkSurfaceProps Measure(surfProps)]
}


proc MeasureEnter {} {
    global Measure

    pushEventManager $Measure(eventManager)
    MeasureRefreshGUI
}

proc MeasureExit {} {
#    DebugMsg "Exiting Measure Tab"
    popEventManager
}


#--------------------------------------
# Code to Report Surface Properties
#--------------------------------------


#-------------------------------------------------------------------------------
# MeasurePosition
#-------------------------------------------------------------------------------
proc MeasurePosition {} {
    global Point
    
    foreach id $Point(selected) {
	set pos $Point($id,xyz)
	set msg [eval format {"%s: %.2f %.2f %.2f"} $Point($id,name) \
		$pos]
	MeasureOutput $msg
    }
    foreach id $Point(selected) {
	PointsSelect $id 0
    }
    MeasureRefreshGUI
    Render3D
}


#-------------------------------------------------------------------------------
# MeasureDistance
#-------------------------------------------------------------------------------
proc MeasureDistance {} {
	global Point

	set idA [lindex $Point(selected) 0]
	set idB [lindex $Point(selected) 1]
#	MeasureTmpPoints SetNumberOfPoints 2
#	eval MeasureTmpPoints InsertPoint 0 $Point($idA,xyz)
#	eval MeasureTmpPoints InsertPoint 1 $Point($idB,xyz)
#	MeasureTmpLines InsertNextCell 2
#		MeasureTmpLines InsertCellPoint 0
#		MeasureTmpLines InsertCellPoint 1
	set dist [format "%.2f" [get_distance $idA $idB]]
	set msg [concat "Distance from" $Point($idA,name) "to" \
			$Point($idB,name) "=" $dist "(mm)"]
	MeasureOutput $msg
	PointsSelect $idA 0
	PointsSelect $idB 0
	Render3D
	MeasureRefreshGUI
	}

#-------------------------------------------------------------------------------
# MeasureAngle
#-------------------------------------------------------------------------------
proc MeasureAngle {} {
	global Point

	set idA [lindex $Point(selected) 0]
	set idB [lindex $Point(selected) 1]
	set idC [lindex $Point(selected) 2]
	set npts [llength $Point(selected)]
	if { $npts == 4 } {
		set idD [lindex $Point(selected) 3]
		set xyzB $Point($idB,xyz)
		set xyzC $Point($idC,xyz)
		set xyzD $Point($idD,xyz)
		set cx [expr [lindex $xyzD 0] - \
			[lindex $xyzC 0] + [lindex $xyzB 0 ]]
		set cy [expr [lindex $xyzD 1] - \
			[lindex $xyzC 1] + [lindex $xyzB 1 ]]
		set cz [expr [lindex $xyzD 2] - \
			[lindex $xyzC 2] + [lindex $xyzB 2 ]]
		PointsSelect $idD 0
		PointsSelect $idC 0
		set Point(model) ""
		set idC [PointsNew $cx $cy $cz]
		}
#	MeasureTmpPoints SetNumberOfPoints 3
#	eval MeasureTmpPoints InsertPoint 0 $Point($idA,xyz)
#	eval MeasureTmpPoints InsertPoint 1 $Point($idB,xyz)
#	eval MeasureTmpPoints InsertPoint 2 $Point($idC,xyz)
#	MeasureTmpLines InsertNextCell 3
#		MeasureTmpLines InsertCellPoint 0
#		MeasureTmpLines InsertCellPoint 1
#		MeasureTmpLines InsertCellPoint 2
	set angle [format "%.2f" [get_angle $idA $idB $idC]]
	set msg [concat "Angle" $Point($idA,name) $Point($idB,name) \
		$Point($idC,name) "=" $angle "deg."]
	MeasureOutput $msg
	PointsSelect $idA 0
	PointsSelect $idB 0
	PointsSelect $idC 0
	Render3D
	MeasureRefreshGUI
	}



# Reports the volume of all selected models
# in the output listbox
proc MeasureVolume {} {
    global Model Selected Module
    global Mrml(dataTree)
    vtkSurfaceProps surfProps
    set vol -1000

    foreach id $Selected(Model) {
	set currModel [Mrml(dataTree) GetNthModel $id]
#	puts $name
	set name [$currModel GetName]

	set r  [lindex $Module(Renderers) 0]

	surfProps SetInput [Model($id,mapper,viewRen) GetInput]
	surfProps Update
#	set vol [surfProps GetTest]
	set vol [surfProps GetVolume]
	if { $vol < 0.0 } {
	    set vol [expr -1*$vol]
	}
	set msg [concat "Volume of" $name "=" [expr $vol*.001] "(ml)"]
	MeasureOutput $msg
	set err [surfProps GetVolumeError]
	if { [expr $err * 10000] > $vol } {
	    set msg [concat "Warning:" $name "volume may not be valid."]
	    MeasureOutput $msg
	}
    }
    
    surfProps Delete
}



#-------------------------------------------------------------------------------
# MeasureSurface
#-------------------------------------------------------------------------------
proc MeasureSurface {} {
    global Model Selected

    vtkSurfaceProps surfProps
    
    foreach id $Selected(Model) {
	set currModel [Mrml(dataTree) GetNthModel $id]
	set name [$currModel GetName]
	surfProps SetInput [Model($id,mapper,viewRen) GetInput]
	surfProps Update
	set area [surfProps GetSurfaceArea]
	set msg [concat "Area of" $name "=" $area "(mm^2)"]
	MeasureOutput $msg
    }

    surfProps Delete
}



proc MeasureIntersection { } {
    # create large single triangle model
    # select single triangle model
    # orient triangle with Csys tool
    # select polyData model
    # intersect between polyData model and triangle
    # count # of intersected triangles
    # scale and report count
    
    global Selected Measure Model
        
    # cut selected[0] with selected[1] for now
    set Measure(modelA) [lindex $Selected(Model) 0]
    set Measure(modelB) [lindex $Selected(Model) 1]

    set idA $Measure(modelA)
    set idB $Measure(modelB)
    
#    DebugMsg "finding cross section of $idA with $idB"

    if { $idA > -1 && $idB > -1 && $idA != $idB } {
	Measure(filterA) SetInput [Model($idA,mapper,viewRen) GetInput]
	Measure(filterB) SetInput [Model($idB,mapper,viewRen) GetInput]
	Measure(cutter) SetXformA [Model($idA,actor,viewRen) GetMatrix]
	Measure(cutter) SetXformB [Model($idB,actor,viewRen) GetMatrix]
#	Model($idA,mapper,viewRen) SetInput [Measure(cutter) GetOutput]
	Measure(cutter) UpdateCutter
	Measure(cutResult) Initialize
	Measure(cutResult) CopyStructure [Measure(cutter) GetOutput]
	Measure(cutResult) Squeeze
	Measure(cutResult) Modified


	# Measure volume of cutResult <vtkPolyData>
	vtkSurfaceProps surfProps
	set vol -1000
	
	surfProps SetInput Measure(cutResult)
	surfProps Update
	set vol [surfProps GetVolume]
	
	if { $vol < 0.0 } {
	    set vol [expr -1*$vol]
	}

###	set name [$currModel GetName]
	set msg [concat "Volume of intersection =" [expr $vol*.001] "(ml)"]
	MeasureOutput $msg
	set err [surfProps GetVolumeError]
	if { [expr $err * 10000] > $vol } {
	    set msg [concat "Warning: volume of intersection may not be valid."]
	    MeasureOutput $msg
	}
	
	surfProps Delete
	
#	puts [Measure(cutter) GetIntersectionCount]
#	puts [Measure(cutter) GetTriangleCount]

	# previous method was to set the cutResult to be a model, then measure the volume
	# of that model
	#	Model($idB,mapper,viewRen) SetInput Measure(cutResult)
	
	#	puts  Measure(cutter) NodeCount
	
	Render3D
    } else {
	puts "Cutting not defined."
    }
}





# writes message into output listbox
proc MeasureOutput { msg } {
    global Gui Measure
    
    foreach foo $Measure(listbox) {
	$foo insert end $msg
    }
}




proc get_distance { a_id b_id } {
	global Point

	set a_pos $Point($a_id,xyz)
	set b_pos $Point($b_id,xyz)
	set dist 0
	for { set ii 0 } { $ii<3 } { incr ii } {
		set delta [expr [lindex $a_pos $ii] - [lindex $b_pos $ii]]
		set delta [expr $delta * $delta]
		set dist [expr $dist + $delta]
		}
	set dist [expr sqrt( $dist )]
#	DebugMsg [concat "Distance = " $dist "mm"]
	return $dist
	}

proc get_angle { a_id b_id c_id } {
	global Point

	set a_vec $Point($a_id,xyz)
	set c_vec $Point($c_id,xyz)
	set b_pos $Point($b_id,xyz)
	set dotprod 0
	for { set ii 0 } { $ii<3 } { incr ii } {
		set delta [expr [lindex $a_vec $ii] - [lindex $b_pos $ii]]
		set a_vec [lreplace $a_vec $ii $ii $delta]
		set delta [expr [lindex $c_vec $ii] - [lindex $b_pos $ii]]
		set c_vec [lreplace $c_vec $ii $ii $delta]
		set dotprod [expr $dotprod + \
			          [lindex $a_vec $ii]*[lindex $c_vec $ii] ]
		}
	set xprodX [expr [lindex $a_vec 1]*[lindex $c_vec 2] - \
			  [lindex $a_vec 2]*[lindex $c_vec 1] ]
	set xprodY [expr [lindex $a_vec 2]*[lindex $c_vec 0] - \
			  [lindex $a_vec 0]*[lindex $c_vec 2] ]
	set xprodZ [expr [lindex $a_vec 0]*[lindex $c_vec 1] - \
			  [lindex $a_vec 1]*[lindex $c_vec 0] ]
	set xprod [expr $xprodX*$xprodX + $xprodY*$xprodY + $xprodZ*$xprodZ]
	set xprod [expr sqrt( $xprod ) ]
	set angle [expr atan2( $xprod, $dotprod )*180/3.141592654]
#	DebugMsg [concat "Angle = " $angle "degrees"]
	return $angle
	}


### JUNK



proc MeasureXSection { } {
    # create large single triangle model
    # select single triangle model
    # orient triangle with Csys tool
    # select polyData model
    # intersect between polyData model and triangle
    # count # of intersected triangles
    # scale and report count
    
    global Selected Measure Model
        
    # cut selected[0] with selected[1] for now
    set Measure(targetModelID) [lindex $Selected(Model) 0]
    set Measure(xSectionToolModelID) [lindex $Selected(Model) 1]

    set idA $Measure(targetModelID)
    set idB $Measure(xSectionToolModelID)
    
#    DebugMsg "finding cross section of $idA with $idB"

    if { $idA > -1 && $idB > -1 && $idA != $idB } {
	Measure(filterA) SetInput [Model($idA,mapper,viewRen) GetInput]
	Measure(filterB) SetInput [Model($idB,mapper,viewRen) GetInput]
	Measure(cutter) SetXformA [Model($idA,actor,viewRen) GetMatrix]
	Measure(cutter) SetXformB [Model($idB,actor,viewRen) GetMatrix]
#	Model($idA,mapper,viewRen) SetInput [Measure(cutter) GetOutput]
	Measure(cutter) UpdateCutter
	Measure(cutResult) Initialize
	Measure(cutResult) CopyStructure [Measure(cutter) GetOutput]
	Measure(cutResult) Squeeze
	Measure(cutResult) Modified
	Model($idB,mapper,viewRen) SetInput Measure(cutResult)
#	puts [Measure(cutter) GetIntersectionCount]
#	puts [Measure(cutter) GetTriangleCount]
#	puts  Measure(cutter) NodeCount

	Render3D
    } else {
	puts "Cutting not defined."
    }
}

# Code to Measure Cross Section
# related to Cutting Code
# use Csys tool to orient a cross section tool
# select the model to measure
# measure intersection of cross section tool and selected model

proc MeasureXSection1 { } {
    global Selected Measure Model
    
    # cut selected[0] with selected[1] for now
    set Measure(targetModelID) [lindex $Selected(Model) 0]
    set Measure(xSectionToolModelID) [lindex $Selected(Model) 1]
    
    set idA $Measure(targetModelID)
    set idB $Measure(xSectionToolModelID)
    
#    DebugMsg "MeasureXSection  Testing TestIntersection Code"
    
    if { $idA > -1 && $idB > -1 && $idA != $idB } {
	Measure(filterA) SetInput [Model($idA,mapper,viewRen) GetInput]
	Measure(filterB) SetInput [Model($idB,mapper,viewRen) GetInput]
	Measure(cutter) SetXformA [Model($idA,actor,viewRen) GetMatrix]
	Measure(cutter) SetXformB [Model($idB,actor,viewRen) GetMatrix]
	
	Measure(cutter) SPLTestIntersection
#	DebugMsg "TestIntersection Successful"
#	puts [Measure(cutter) GetTestCount]
    }
}








#--------------------------------------
# Code to deal with Coordinate System
#--------------------------------------


# if a model is picked, places a landmark on the model
# if the Csys is picked allows movement of Csys
# otherwise does nothing

proc addGlyphPoint { widget x y } {
    global viewRen Point Model Module

#    DebugMsg "addGlyphPoint"
    MeasureSetModelsPickable 0
    MeasureSetGlyphsPickable 0
    MeasureSetCsysPickable 1

    if { [SelectPick Point(picker) $widget $x $y] == 0 } {
#	DebugMsg "NoCsysPick"
	MeasureSetModelsPickable 1
	MeasureSetCsysPickable 0
	if { [SelectPick Point(picker) $widget $x $y] == 0 } {
#	    DebugMsg [concat "Nothing picked at" $x "," $y "in" $widget ]
	} else {
	    set actor [Point(picker) GetActor]
	    set Point(model) ""
	    foreach id $Model(idList) {
		foreach r $Module(Renderers) {
#		    DebugMsg [concat "Check if " $actor " is " $id " " $r]
		    if { $actor == "Model($id,actor,$r)" } {	
#			DebugMsg [concat "Choosing model " $id]
			set Point(model) Model($id,actor,$r)
		    } 
		}
	    }
	    eval PointsNew [Point(picker) GetPickPosition]
	    MeasureRefreshGUI
	    Render3D
	}
    } else {
#	DebugMsg "Picking Csys"

	set assemblyPath [Point(picker) GetPath]
	$assemblyPath InitTraversal
	set assemblyNode [$assemblyPath GetLastNode]
	set actor [$assemblyNode GetProp]

	if { [$actor GetProperty] == [Xactor GetProperty] } {
	    
#	    DebugMsg "xaxis"
	    XformAxisStart $widget 0 $x $y
	} elseif { [$actor GetProperty] == [Yactor GetProperty] } {
#	    DebugMsg "yaxis"
	    XformAxisStart $widget 1 $x $y
	} elseif { [$actor GetProperty] == [Zactor GetProperty] } {
#	    DebugMsg "zaxis"
	    XformAxisStart $widget 2 $x $y
	}
    }
}



proc delGlyphPoint { widget x y } {
    global Point
    
#    DebugMsg "delGlyphPoint"
    MeasureSetAllPickable 0
    MeasureSetGlyphsPickable 1
    set pntid [selGlyphPoint $widget $x $y]
#    puts [concat "DEBUG: delGlyphPoint: " $pntid]
    if { $pntid != -1 } {
	PointsDelete $pntid
#	MeasureRefreshGUI
#	Render3D
    }
    MeasureRefreshGUI
    Render3D
}



proc selGlyphPoint { widget x y } {
    global viewRen Point
    
#    DebugMsg "selGlyphPoint"
    MeasureSetAllPickable 0
    MeasureSetModelsPickable 0
    MeasureSetGlyphsPickable 1
    MeasureSetCsysPickable 1
    if { [SelectPick Point(picker) $widget $x $y] == 0 } {
#	DebugMsg [concat "Nothing picked at" $x "," $y "in" $widget ]
	return -1
    } else {
	set actor [Point(picker) GetActor]
#	DebugMsg [concat "Picked actor:" $actor]
	if { [$actor GetProperty] == [Xactor GetProperty] } {
	    XformAxisStart $widget 0 $x $y
	} elseif { [$actor GetProperty] == [Yactor GetProperty] } {
	    XformAxisStart $widget 1 $x $y
	} elseif { [$actor GetProperty] == [Zactor GetProperty] } {
	    XformAxisStart $widget 2 $x $y
	} else {
	    set pntid [PointIdFromGlyphCellId \
		    [Point(picker) GetCellId]]
	    if { $pntid > -1 } {
#		DebugMsg [concat "PointId: " $pntid \
			", Location: " $Point($pntid,xyz) ]
		PointsSelect $pntid 1
		MeasureRefreshGUI
		Render3D
	    }
	    return $pntid
	}
    }
    return -1
}


#-------------------------------------------------------------------------------
# MeasureSetModelsPickable
#-------------------------------------------------------------------------------
proc MeasureSetModelsPickable { pickable } {
    global Model Slice Module
    
#    DebugMsg MeasureSetModelsPickable
    foreach foo $Model(idList) {
	set r  [lindex $Module(Renderers) 0]

	set actor Model($foo,actor,$r)
#	DebugMsg [concat Model $foo SetPickable $pickable]
	if { [$actor GetVisibility] } {
	    $actor SetPickable $pickable
	} else {
	    $actor SetPickable 0
	}
    }
}


proc MeasureSetCsysPickable { pickable } {
#    DebugMsg "MeasureSetCsysPickable"
    foreach foo {Xactor Yactor Zactor} {
#	DebugMsg [concat $foo SetPickable $pickable]
	$foo SetPickable $pickable
    }
}


#-------------------------------------------------------------------------------
# MeasureSetGlyphsPickable
#-------------------------------------------------------------------------------
proc MeasureSetGlyphsPickable { pickable } {
    global Point
#    DebugMsg [concat Point(actor) SetPickable $pickable]
    $Point(actor) SetPickable $pickable
}


proc MeasureSetAllPickable { pickable } {
#    DebugMsg MeasureSetAllPickable
    foreach foo [vtkActor ListInstances] {
#	DebugMsg [concat $foo SetPickable $pickable]
	if { [$foo GetVisibility] } {
	    $foo SetPickable $pickable
	} else {
	    $foo SetPickable 0
	}
    }
    foreach foo [vtkFollower ListInstances] {
#	DebugMsg [concat $foo SetPickable $pickable]
	$foo SetPickable $pickable
    }
}
