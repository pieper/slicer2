#=auto==========================================================================
# Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        EdLiveWire.tcl
# PROCEDURES:  
#   EdLiveWireInit
#   EdLiveWireBuildVTK
#   EdLiveWireBuildGUI
#   EdLiveWireRaiseEdgeImageWin
#   EdLiveWireGetFeatureParams
#   EdLiveWireSetFeatureParams
#   EdLiveWireAdvancedApply
#   EdLiveWireInitPipeline
#   EdLiveWireEnter
#   EdLiveWireExit
#   EdLiveWireUpdateInteractive
#   EdLiveWireSetInput
#   EdLiveWireSetInteract
#   EdLiveWireUpdateInit
#   EdLiveWireUpdate
#   EdLiveWireB1
#   EdLiveWireB1Motion
#   EdLiveWireRenderInteractive
#   EdLiveWireLabel
#   EdLiveWireResetSlice
#   EdLiveWireApply
#   EdLiveWireStartProgress
#   EdLiveWireShowProgress
#   EdLiveWireEndProgress
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdLiveWireInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireInit {} {
    global Ed Gui
    
    set e EdLiveWire
    set Ed($e,name)      "LiveWire"
    set Ed($e,initials)  "LW"
    set Ed($e,desc)      "LiveWire: quick 2D segmentation."
    set Ed($e,rank)      1
    set Ed($e,procGUI)   EdLiveWireBuildGUI
    set Ed($e,procVTK)   EdLiveWireBuildVTK
    set Ed($e,procEnter) EdLiveWireEnter
    set Ed($e,procExit)  EdLiveWireExit

    # Required
    set Ed($e,scope)  Single
    # Original volume is input to our filters.
    set Ed($e,input)  Original

    set Ed($e,radius) 0
    set Ed($e,shape)  Polygon
    set Ed($e,render) Active
    
    # slider range
    set Ed($e,sliderLow) 0.0
    set Ed($e,sliderHigh) 1.0
    
    # active slice
    set Ed($e,interact) Active

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireBuildVTK {} {
    global Ed Label Slice
    
    # Lauren rename these dumb things!

    foreach s $Slice(idList) {    
	# this filter feeds edges to live wire
	vtkImageLiveWireTester Ed(EdLiveWire,lwSetup$s)

	# these filters find directional edge maps
	set Ed(EdLiveWire,numEdgeFilters) \
		[Ed(EdLiveWire,lwSetup$s) GetNumberOfEdgeFilters]
	for {set f 0} {$f < $Ed(EdLiveWire,numEdgeFilters)} {incr f} {
	    set filt [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $f]
	    $filt SetStartMethod EdLiveWireStartProgress
	    $filt SetProgressMethod \
		    "EdLiveWireShowProgress Ed(EdLiveWire,lwPath$s)"
	    $filt SetEndMethod EdLiveWireEndProgress
	}

	# this filter finds short paths using edges
	vtkImageLiveWire Ed(EdLiveWire,lwPath$s)

	# debug
	Ed(EdLiveWire,lwPath$s)  SetVerbose 1

	# pipeline
	Ed(EdLiveWire,lwSetup$s) SetLiveWire Ed(EdLiveWire,lwPath$s)
	Ed(EdLiveWire,lwPath$s) SetOriginalImage [Ed(EdLiveWire,lwSetup$s) GetOutput]
	# Lauren make slicer take LW output straight????????
	vtkImageCopy Ed(EdLiveWire,copy$s)
	Ed(EdLiveWire,copy$s) SetInput [Ed(EdLiveWire,lwPath$s) GetOutput]

	# for looking at edge weight image
	vtkImageViewer Ed(EdLiveWire,viewer$s)
	Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage 0]
	Ed(EdLiveWire,viewer$s) SetColorWindow 256
	Ed(EdLiveWire,viewer$s) SetColorLevel 127.5
	[Ed(EdLiveWire,viewer$s) GetImageWindow] DoubleBufferOn
    }
    
    # initialize tcl variables that mirror vtk object settings
    EdLiveWireGetFeatureParams
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireBuildGUI {} {
    global Ed Gui Label Volume

    set f $Ed(EdLiveWire,frame)

    # this makes the navigation menu (buttons) and the tabs (frames).
    set label "LW Settings:"
    set subframes {Basic Advanced}
    set buttonText {"Basic" "Advanced"}
    set tooltips {"For Users" "For Developers"}

    TabbedFrame EdLiveWire $f $label $subframes $buttonText \
	    $tooltips

    #-------------------------------------------
    # TabbedFrame->Basic frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic
    
    # Standard stuff
    frame $f.fInput     -bg $Gui(activeWorkspace)
    frame $f.fScope     -bg $Gui(activeWorkspace)
    frame $f.fInteract  -bg $Gui(activeWorkspace)
    frame $f.fGrid      -bg $Gui(activeWorkspace)
    frame $f.fApply     -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fInput $f.fScope \
	    $f.fInteract $f.fApply \
	    -side top -pady $Gui(pad) -fill x
    
    # Standard Editor interface buttons
    EdBuildScopeGUI $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fScope Ed(EdLiveWire,scope) Multi

    EdBuildInputGUI $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fInput Ed(EdLiveWire,input) \
	    "-command EdLiveWireSetInput"
    
    EdBuildInteractGUI $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fInteract Ed(EdLiveWire,interact) \
	    "-command EdLiveWireSetInteract"
    
    #-------------------------------------------
    # TabbedFrame->Basic->Grid frame (output color)
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fGrid
    
    # Output label
    eval {button $f.bOutput -text "Output:" \
	    -command "ShowLabels EdLiveWireLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdLiveWireLabel"
    bind $f.eOutput <FocusOut> "EdLiveWireLabel"
    eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
	    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName
    
    #-------------------------------------------
    # TabbedFrame->Basic->Apply frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fApply
    
    eval {button $f.bApply -text "Apply" \
	    -command "EdLiveWireApply"} $Gui(WBA) {-width 8}
    pack $f.bApply

    eval {button $f.bTest -text "Test" \
	    -command "EdLiveWireRaiseEdgeImageWin"} $Gui(WBA) {-width 8}
    pack $f.bTest

    #-------------------------------------------
    # TabbedFrame->Advanced frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced

    frame $f.fSettings   -bg $Gui(activeWorkspace)
    pack $f.fSettings -side top -pady $Gui(pad) -fill x

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings


    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Features frame
    #-------------------------------------------
    frame $f.fFeatures   -bg $Gui(activeWorkspace)
    pack $f.fFeatures -side top -pady $Gui(pad) -fill x
    set f $f.fFeatures

    # hard-coded feature names:
    # should match with the order in vtkImageLiveWireEdgeWeights
    set featureNames "{0: in pix} {1: out pix} {2: out-in} \
	    {3: gradient} {4: gradient} {5: gradient} {6: new feature...}"
    set paramNames "{Weight} {Mean} {Variance}"

    set n 1
    foreach name $paramNames {
	eval {label $f.lparamName$n -text $name} $Gui(WLA)
	# 0th row is param name
	grid $f.lparamName$n -row 0 -col $n
	incr n
    }

    # loop over features
    for {set feat 0} {$feat < $Ed(EdLiveWire,numFeatures)} {incr feat} {
	
	set name [lindex $featureNames $feat]
	if {$name == ""} {
	    set $name "$feat: new feature"
	}

	set row [expr $feat +1]
	
	# 0th column is feature name
	eval {label $f.lfeatureName$feat -text $name} $Gui(WLA)
	grid $f.lfeatureName$feat  -row $row -column 0 -sticky w \
		-ipadx 2 -ipady 2

	# 1st column is feature weight
	eval {entry $f.eFeature${feat}Weight -width 6 \
		-textvariable Ed(EdLiveWire,feature$feat,weight)} $Gui(WEA)
	grid $f.eFeature${feat}Weight  -row $row -column 1 -sticky ns 

	# last columns: loop over parameters for calculation of each feature
	for {set p 0} {$p < $Ed(EdLiveWire,feature$feat,numParams)} {incr p} {

	    # entry box for each param for the feature
	    eval {entry $f.eFeature${feat}Param$p -width 6 \
		    -textvariable Ed(EdLiveWire,feature$feat,param$p)} $Gui(WEA)

	    # 1st 2 cols were name and weight:
	    set col [expr $p + 2]
	    grid $f.eFeature${feat}Param$p  -row $row -column $col \
		    -sticky ns 
	}

    }

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Apply frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings

    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fApply -side top -pady $Gui(pad) -fill x
    set f $f.fApply
    eval {button $f.bApply -text "Apply Settings" \
	    -command "EdLiveWireAdvancedApply"} $Gui(WBA) {-width 20}
    pack $f.bApply

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Popup frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings

    frame $f.fPopup   -bg $Gui(activeWorkspace)
    pack $f.fPopup -side top -pady $Gui(pad) -fill x
    set f $f.fPopup
    eval {button $f.bPopup -text "View Edge Images" \
	    -command "EdLiveWireRaiseEdgeImageWin"} $Gui(WBA) {-width 20}
    pack $f.bPopup

}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireRaiseEdgeImageWin
# Displays "edge image," which shows edge weights (costs)
# that are derived from the image.
# Boundaries of interest should be enhanced in these images.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireRaiseEdgeImageWin {} {
    global Slice Ed

    set s $Slice(activeID)
    set w .liveWireEdgeWin$s

    # if already created, raise and return
    if {[winfo exists $w] != 0} {
	raise $w
	return
    }

    # make the pop up window
    toplevel $w

    # put image viewer in it
    frame $w.fwin
    set f $w.fwin
    vtkTkImageViewerWidget $f.v$s -width 256 -height 256 \
	    -iv Ed(EdLiveWire,viewer$s)
    pack $f -side top -fill both
    bind $f.v$s <Expose> {ExposeTkImageViewer %W %x %y %w %h}
    pack $f.v$s -side top
    set viewer $f.v$s

    # radiobuttons switch between edge images
    frame $w.fedgeBtns
    set f $w.fedgeBtns
    #Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage 0]
    foreach edge "0 1 2 3" text "0 1 2 3" width "2 2 2 2" {
	radiobutton $f.r$edge -width $width -indicatoron 0\
		-text "$text" -value "$edge" \
		-variable Ed(EdLiveWire,edge$s) \
		-command "Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage $edge]; $viewer Render" 
	pack $f.r$edge -side left -fill x -anchor e
    }
    pack $f -side top

    # make close button
    frame $w.fcloseBtn
    set f $w.fcloseBtn
    button $f.b -text Close -command "lower $w"
    pack $f -side top
    pack $f.b -side top -fill x

}



#-------------------------------------------------------------------------------
# .PROC EdLiveWireGetFeatureParams
# initialize tcl variables that mirror vtk object settings
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireGetFeatureParams {} {
    global Ed Slice
    
    set s $Slice(activeID)

    # set up parameters for control of edge filters  
    set default [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter 0]
    set Ed(EdLiveWire,numFeatures) [$default GetNumberOfFeatures]
    
    # loop over features
    for {set f 0} {$f < $Ed(EdLiveWire,numFeatures)} {incr f} {

	# get weight (importance given to feature)
	set Ed(EdLiveWire,feature$f,weight) \
		[$default GetWeightForFeature $f]	
	
	# get number of params for feature calculation
	set Ed(EdLiveWire,feature$f,numParams) \
		[$default GetNumberOfParamsForFeature $f]

	# loop over params for each feature
	for {set p 0} {$p < $Ed(EdLiveWire,feature$f,numParams)} {incr p} {
	    set Ed(EdLiveWire,feature$f,param$p) \
		    [$default GetParamForFeature $f $p]
	}
    }

}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetFeatureParams
# set vtkImageLiveWireEdgeWeights filters settings from tcl variables
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetFeatureParams {} {
    global Ed Slice

    # Lauren do this for all slices or just active?
    # Lauren need to check bounds better, and also make filters modified...

    # loop over slices
    foreach s $Slice(idList) {    
	# loop over edge filters
	for {set filter 0} {$filter < $Ed(EdLiveWire,numEdgeFilters)} {incr filter} {
	    set filt [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $filter]
	    
	    # loop over features
	    for {set feat 0} {$feat < $Ed(EdLiveWire,numFeatures)} {incr feat} {
		
		# set weight (importance given to feature)
		$filt SetWeightForFeature $feat $Ed(EdLiveWire,feature$feat,weight)
		
		# loop over params for each feature
		for {set p 0} {$p < $Ed(EdLiveWire,feature$feat,numParams)} {incr p} {
		    $filt SetParamForFeature $feat $p $Ed(EdLiveWire,feature$feat,param$p)
		}
	    }
	    
	}
    }
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireAdvancedApply
# Changes filter settings:
# validates input and calls EdLiveWireSetFeatureParams.
# Called using Apply button in advanced tab.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireAdvancedApply {} {
    global Ed

    # Validate input:

    # loop over features
    for {set feat 0} {$feat < $Ed(EdLiveWire,numFeatures)} {incr feat} {
	
	# weight (importance given to feature)
	if {[ValidateFloat $Ed(EdLiveWire,feature$feat,weight)] == 0} {
	    tk_messageBox -message "Weight for feature $feat is not a number."
	    return
	}
	
	# loop over params for each feature
	for {set p 0} {$p < $Ed(EdLiveWire,feature$feat,numParams)} {incr p} {

	    if {[ValidateFloat $Ed(EdLiveWire,feature$feat,param$p)] == 0} {
		tk_messageBox -message "Parameter $p for feature $feat is not a number."
		return
	    }    
	}
    }
 
    EdLiveWireSetFeatureParams

}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireInitPipeline
# Sets up filters to get input from Slicer (vtkMrmlSlicer) object.
# Updates the pipeline.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireInitPipeline {} {
    global Ed Slice Gui

    set Gui(progressText) "Livewire Initialization"	

    # set up pipeline
    foreach s $Slice(idList) {
	Slicer SetFirstFilter $s Ed(EdLiveWire,lwSetup$s)
	# Lauren fix? (make slicer accept multiple image filter too)
	Slicer SetLastFilter  $s Ed(EdLiveWire,copy$s)
    }
    # Layers: Back=Original, Fore=Working
    if {$Ed(EdLiveWire,input) == "Original"} {
	Slicer BackFilterOn
	Slicer ForeFilterOff
    } else {
	Slicer BackFilterOff
	Slicer ForeFilterOn
    }

    # only apply filters to active slice
    Slicer FilterActiveOn

    Slicer ReformatModified
    Slicer Update

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireEnter
# Called upon entering module.
# Forces pipeline to update initially.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireEnter {} {
    global Ed Label Slice

    puts "ENTER."

    # all 4 edge filters need to execute
    set Ed(EdLiveWire,numEdgeFiltersReady) 0
    # ignore mouse clicks until we are all ready
    set Ed(EdLiveWire,pipelineActive) 0
    # ignore mouse movement until we are all ready
    set Ed(EdLiveWire,pipelineActiveAndContourStarted) 0

    # set up Slicer pipeline and force edge filters to execute.
    EdLiveWireInitPipeline

    # keep track of active slice to reset contour if slice changes
    set Ed(EdLiveWire,activeSlice) $Slice(activeID)

    # Lauren reset livewire drawing?
    # Lauren make this a proc or c++ thing that resets the 
    # entire livewire object ?

    # Lauren what is this?
    # Make sure we're colored
    LabelsColorWidgets

    # make sure we're drawing the right color
    foreach s $Slice(idList) {
	Ed(EdLiveWire,lwPath$s) SetLabel $Label(label)	
    }

    # Lauren not really necessary to use slicer object to draw,
    # but might be nicer to be able to  save points and use 
    # while in EdDraw...
    set e EdLiveWire
    Slicer DrawSetRadius $Ed($e,radius)
	Slicer DrawSetShapeTo$Ed($e,shape)
	if {$Label(activeID) != ""} {
		set color [Color($Label(activeID),node) GetDiffuseColor]
		eval Slicer DrawSetColor $color
	} else {
		Slicer DrawSetColor 0 0 0
	}
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireExit
# 
# Called upon leaving module.  Shuts down filter pipeline
# that was displaying the interactive livewire over images.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireExit {} {
    global Ed Slice

    # reset livewire drawing
    foreach s $Slice(idList) {
	EdLiveWireResetSlice $s
    }

    # no more filter pipeline
    Slicer BackFilterOff
    Slicer ForeFilterOff
    Slicer ReformatModified
    Slicer Update
    EdLiveWireRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireUpdateInteractive
# Update when the user is drawing on the slice (interactive)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireUpdateInteractive {} {
    global Ed Slice

    # Lauren this should be called if input is changed from orig to work?

#    foreach s $Slice(idList) {
#	Slicer SetFirstFilter $s Ed(EdLiveWire,lwSetup$s)
#	# Lauren fix? (make slicer accept multiple image filter too)
#	#Slicer SetLastFilter  $s Ed(EdLiveWire,lwPath$s)
#	Slicer SetLastFilter  $s Ed(EdLiveWire,copy$s)
#    }

    # Layers: Back=Original, Fore=Working
    if {$Ed(EdLiveWire,input) == "Original"} {
	Slicer BackFilterOn
	Slicer ForeFilterOff
    } else {
	Slicer BackFilterOff
	Slicer ForeFilterOn
    }

    # Just active slice?
#    if {$Ed(EdLiveWire,interact) == "Active"} {
#	Slicer FilterActiveOn
#    } else {
#	Slicer FilterActiveOff
#    }
    
#    Slicer ReformatModified
#    Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetInput
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetInput {} {
    global Ed Label
    
    EdLiveWireUpdateInteractive
    EdLiveWireUpdate
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetInteract
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetInteract {} {
    global Ed Label
    
    EdLiveWireUpdateInteractive
    EdLiveWireUpdate
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireUpdateInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireUpdateInit {widget {value ""}} {

    $widget config -command "EdLiveWireUpdate; RenderActive"
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireUpdate
# Update after user changes GUI.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireUpdate {{value ""}} {
    global Ed Label Slice
    
    # Validate input
    if {$Label(label) == ""} {
	return
    }

    # update filter stuff    
    foreach s $Slice(idList) {
	Ed(EdLiveWire,lwPath$s) SetLabel $Label(label)	
    }

    # render whatever we are supposed to (slice, 3slice, or 3D)
    EdLiveWireRenderInteractive
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireB1
# When mouse is clicked, handle new contour if needed and 
# pass location to live wire filter.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireB1 {x y s} {
    global Ed Slice

    # if pipeline not set up yet do nothing
    if {$Ed(EdLiveWire,pipelineActive) == 0} {
	return
    }
    set Ed(EdLiveWire,pipelineActiveAndContourStarted) 1

    # if we just changed to this slice
    if {$Ed(EdLiveWire,activeSlice) != $s} {
	set Ed(EdLiveWire,activeSlice) $s
#	EdLiveWireResetSlice $s
    }


    #puts "EDLW: start $x $y"
    #Ed(EdLiveWire,lwSetup$s) SetStartPoint $x $y
    Ed(EdLiveWire,lwPath$s) SetStartPoint $x $y

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireB1Motion
# When mouse moves over slice, if pipeline is operational, 
# pass end point to live wire filter and then render the slice.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireMotion {x y s} {
    global Ed Slice

    # if no first click to begin contour, do nothing
    if {$Ed(EdLiveWire,pipelineActiveAndContourStarted) == 0} {
	return
    }

    #puts "EDLW: end $x $y"
    #Ed(EdLiveWire,lwSetup$s) SetEndPoint $x $y
    Ed(EdLiveWire,lwPath$s) SetEndPoint $x $y
    #EdLiveWireUpdateInteractive

    # render whatever we are supposed to (slice, 3slice, or 3D)
    EdLiveWireRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireRenderInteractive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireRenderInteractive {} {
    global Ed
    
    Render$Ed(EdLiveWire,interact)
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireLabel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireLabel {} {
    global Ed
    
    LabelsFindLabel
    EdLiveWireUpdate
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireResetSlice
# Clear the previous contour to start over with new start point.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireResetSlice {s} {
    global Ed

    Ed(EdLiveWire,lwPath$s) ClearContour
    # Lauren have to do this for each slice?
    set Ed(EdLiveWire,pipelineActiveAndContourStarted) 0

}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireApply
# Actually draw the polygon, line, whatever, into the volume.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireApply {} {
    global Ed Volume Label Gui Slice

    set s $Slice(activeID)

    # if there are no points, do nothing
    # Lauren reset the slice and drawing?
    set rasPoints [Ed(EdLiveWire,lwPath$s) GetContourPixels]
    if {[$rasPoints GetNumberOfPoints] == 0} {
	puts "no points to apply!"
	return
    }
    
    # testing
    source "/scratch/src/slicer/program/debug.tcl"
    set file "/scratch/src/slicer/program/test.txt"
    writePoints $rasPoints $file

    puts "a1"

    set e EdLiveWire

    # the working volume is editor input (where we want to draw)
    # (though the original vol is input to the livewire filters.)
    set v [EditorGetInputID Working]

    # Validate input    
    if {[ValidateInt $Label(label)] == 0} {
	tk_messageBox -message "Output label is not an integer."
	return
    }
    if {[ValidateInt $Ed($e,radius)] == 0} {
	tk_messageBox -message "Point Radius is not an integer."
	return
    }

    # Lauren Native or Active?
    #EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
    EdSetupBeforeApplyEffect $v $Ed($e,scope) Active

    # text over blue progress bar
    set Gui(progressText) "Livewire [Volume($v,node) GetName]"	

    puts "a2"

    # attributes of region to draw
    set label    $Label(label)
    set radius   $Ed($e,radius)
    set shape    $Ed($e,shape)

    # Apply points by first converting to IJK and then 
    # giving them to the Editor object.    
    #vtkPoints ijkPoints
    #Slicer ComputeIjkPoints $rasPoints ijkPoints

    puts "a3"

    puts "ras:  [$rasPoints GetNumberOfPoints]"
    #puts "ijk: [ijkPoints GetNumberOfPoints]"

    # test other way (like Draw):
    set numPoints [$rasPoints GetNumberOfPoints]
    for {set p 0} {$p < $numPoints} {incr p} {
	scan [$rasPoints GetPoint $p] "%d %d %d" x y z
	Slicer DrawInsertPoint $x $y
    }
    Slicer DrawComputeIjkPoints
    set points [Slicer GetDrawIjkPoints]
    Ed(editor)   Draw $label $points $radius $shape
    

    # trouble with crashing here. why?
    #Ed(editor)   Draw $label ijkPoints $radius $shape

    puts "a4"

    #ijkPoints Delete

    # clear points that livewire object was storing
    EdLiveWireResetSlice $s

    puts "a5"

    # reset editor object
    Ed(editor)     SetInput ""
    Ed(editor)     UseInputOff

    EdUpdateAfterApplyEffect $v Active

    puts "a6"

    # always delete points for now...
    Slicer DrawDeleteAll

    # Lauren what is this?
    # Reset sliders if the input was working, because that means
    # it changed.
    if {$v == [EditorGetWorkingID]} {
	EdLiveWireSetInput
    }
    puts "Applied"
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireStartProgress
#
#  Wrapper around MainStartProgress (Does Nothing)
#
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireStartProgress {} {

    MainStartProgress

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireShowProgress
# Progress method callback for vtk filters.
# Wrapper around MainShowProgress, which shows the blue bar.
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireShowProgress {filter} {

    MainShowProgress $filter

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireEndProgress
#
# Wrapper around MainEndProgress.
# Then when all edge filters have executed, allows mouse clicks.
# 
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireEndProgress {} {
    global Ed

    MainEndProgress
    
    # all 4 edge filters need to execute
    incr Ed(EdLiveWire,numEdgeFiltersReady)

    if {$Ed(EdLiveWire,numEdgeFiltersReady) == $Ed(EdLiveWire,numEdgeFilters)} {
	# we are all ready: pay attention to mouse clicks
	set Ed(EdLiveWire,pipelineActive) 1
    }
}