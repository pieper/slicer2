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
# FILE:        EdPhaseWire.tcl
# PROCEDURES:  
#
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireInit
# Automatically called init procedure
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireInit {} {
    global Ed Gui
    
    set e EdPhaseWire
    set Ed($e,name)      "PhaseWire"
    set Ed($e,initials)  "Pw"
    set Ed($e,desc)      "PhaseWire: quick 2D segmentation."
    # order this editor module will appear in the list
    set Ed($e,rank)      9
    set Ed($e,procGUI)   EdPhaseWireBuildGUI
    set Ed($e,procVTK)   EdPhaseWireBuildVTK
    set Ed($e,procEnter) EdPhaseWireEnter
    set Ed($e,procExit)  EdPhaseWireExit

    # Required
    set Ed($e,scope)  Single
    # Original volume is input to our filters.
    set Ed($e,input)  Original
    
    # drawing vars
    set Ed($e,radius) 0;        # thickness of lines
    set Ed($e,shape)  Polygon;  # shape to draw when apply
    set Ed($e,render) Active;   # render all 3 or just 1 slice when apply
    
    # settings for combining phase and cert info
    set Ed(EdPhaseWire,phaseWeight) 1
    set Ed(EdPhaseWire,certWeight) 1
    set Ed(EdPhaseWire,gradWeight) 0
    set Ed(EdPhaseWire,certLowerCutoff) 0
    set Ed(EdPhaseWire,certUpperCutoff) 150
}


proc EdPhaseWireBuildVTK {} {
    global Ed Volume Slice

    ### This is a fix so that reformatting the extra volumes will work
    vtkMultiThreader temp
    #set normalthreads [temp GetGlobalDefaultNumberOfThreads]
    temp SetGlobalDefaultNumberOfThreads 1
    temp Delete
    # now only one thread will be used for our reformatting
    # when the reformatter objects are created by slicer object below
    # NOTE this affects all objects created from now on - should reset
    ### end fix
    
    set e EdPhaseWire

    foreach s $Slice(idList) {
	
	# maybe not used: for training, though
	# currently this is the filter that the slicer
	# gets to start off our pipeline
	vtkImageGradientMagnitude Ed($e,gradMag$s)
	
	# for combining phase, cert, and any other inputs
	vtkImageWeightedSum Ed($e,imageSumFilter$s)
	
	# for normalization of the phase and cert inputs
	vtkImageLiveWireScale Ed($e,phaseNorm$s)
	vtkImageLiveWireScale Ed($e,certNorm$s)
	vtkImageLiveWireScale Ed($e,gradNorm$s)

	# pipeline (rest done in EdPhaseWireEnter)
	Ed($e,gradNorm$s)  SetInput [Ed($e,gradMag$s) GetOutput]
	
	# transformation functions to emphasize desired features of the
	# phase and cert inputs
	#certNorm SetTransformationFunctionToOneOverX
	Ed($e,certNorm$s) SetTransformationFunctionToInverseLinearRamp
	Ed($e,gradNorm$s) SetTransformationFunctionToOneOverX
	
	# weighted sum of all inputs
	set sum Ed(EdPhaseWire,imageSumFilter$s)
	# pipeline
	$sum SetInput 0 [Ed($e,phaseNorm$s) GetOutput]
	$sum SetInput 1 [Ed($e,certNorm$s) GetOutput]
	$sum SetInput 2 [Ed($e,gradNorm$s) GetOutput]
	
	# this filter finds short paths in the image and draws the wire
	vtkImageLiveWire Ed(EdPhaseWire,lwPath$s)
	# we want our path to be able to go to diagonal pixel neighbors
	Ed(EdPhaseWire,lwPath$s) SetNumberOfNeighbors 8
	# debug
	Ed(EdPhaseWire,lwPath$s) SetVerbose 0

	# for looking at the input to the livewire filter
	vtkImageViewer Ed(EdPhaseWire,viewer$s)
	Ed(EdPhaseWire,viewer$s) SetInput \
		[Ed(EdPhaseWire,lwPath$s) GetInput 0]
	Ed(EdPhaseWire,viewer$s) SetColorWindow 256
	Ed(EdPhaseWire,viewer$s) SetColorLevel 127.5
	[Ed(EdPhaseWire,viewer$s) GetImageWindow] DoubleBufferOn
	
	# pipeline
	set totalInputs 9
	for {set i 0} {$i < $totalInputs} {incr i} {   
	    
	    # set all lw inputs (for all 8 directions) 
	    # to be from phase info
	    Ed(EdPhaseWire,lwPath$s) SetInput $i [$sum GetOutput]
	}
	
	# figure out what the max value is that the filters can output
	# this is needed for shortest path computation
	set scale [Ed(EdPhaseWire,lwPath$s) GetMaxEdgeCost]
	# make sure this is max val output by these filters:
	Ed($e,phaseNorm$s) SetScaleFactor $scale
	Ed($e,certNorm$s) SetScaleFactor  $scale
	Ed($e,gradNorm$s) SetScaleFactor  $scale
	
    }
}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireBuildGUI
# build GUI of module
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireBuildGUI {} {
    global Ed Gui Label Volume

    set f $Ed(EdPhaseWire,frame)

    # this makes the navigation menu (buttons) and the tabs (frames).
    set label ""
    set subframes {Help Basic Advanced}
    set buttonText {"Help" "Basic" "Advanced"}
    set tooltips { "Help: We all need it sometimes." \
	    "Basic: For Users" \
	    "Advanced: Current PhaseWire Settings and Stuff for Developers"}
    set extraFrame 0
    set firstTab Basic

    TabbedFrame EdPhaseWire $f $label $subframes $buttonText \
	    $tooltips $extraFrame $firstTab

    #-------------------------------------------
    # TabbedFrame->Help frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fHelp


    #-------------------------------------------
    # TabbedFrame->Basic frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic
    
    # Standard stuff
    frame $f.fRender  -bg $Gui(activeWorkspace)
    frame $f.fGrid      -bg $Gui(activeWorkspace)
    frame $f.fContour   -bg $Gui(activeWorkspace)
    frame $f.fReset   -bg $Gui(activeWorkspace)
    frame $f.fApply     -bg $Gui(activeWorkspace)
    frame $f.fTest     -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fRender $f.fContour $f.fReset  \
	    $f.fApply $f.fTest \
	    -side top -pady $Gui(pad)

    # Standard Editor interface buttons
    EdBuildRenderGUI $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic.fRender Ed(EdPhaseWire,render)
    
    #-------------------------------------------
    # TabbedFrame->Basic->Grid frame (output color)
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic.fGrid
    
    # Output label
    eval {button $f.bOutput -text "Output:" \
	    -command "ShowLabels EdPhaseWireLabel"} $Gui(WBA)
    TooltipAdd $f.bOutput \
	    "Choose output label value to draw on the slice"
    eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdPhaseWireLabel"
    bind $f.eOutput <FocusOut> "EdPhaseWireLabel"
    eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
	    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName


    #-------------------------------------------
    # TabbedFrame->Basic->Contour frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic.fContour
    eval {button $f.bContour -text "Stay near last slice's contour" \
	    -command {puts "this feature is coming soon."}} $Gui(WBA)
    # Lauren implement this!
    #pack $f.bContour
    #TooltipAdd $f.bContour \
	    "Keep the PhaseWire near the contour you drew on the previous slice."

    #-------------------------------------------
    # TabbedFrame->Basic->Reset Frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic.fReset
    eval {button $f.bReset -text "Clear Contour" \
	    -command "EdPhaseWireClearCurrentSlice"} $Gui(WBA)
    TooltipAdd $f.bReset \
	    "Reset the PhaseWire for this slice."

    eval {button $f.bResetSeg -text "Undo Last Click" \
	    -command "EdPhaseWireClearLastSegment"} $Gui(WBA)
        TooltipAdd $f.bResetSeg \
	    "Clear the latest part of the PhaseWire."

    pack $f.bReset $f.bResetSeg -side left -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # TabbedFrame->Basic->Apply frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fBasic.fApply
    
    # frame for shape control
    frame $f.f -bg $Gui(activeWorkspace)
    eval {label $f.f.l -text "Shape:"} $Gui(WLA)
    pack $f.f.l -side left -padx $Gui(pad)

    # "Line" drawing button really draws our wire of points
    foreach shape "Polygon Lines" draw "Polygon Points" {
	eval {radiobutton $f.f.r$shape -width [expr [string length $shape]+1] \
		-text "$shape" -variable Ed(EdPhaseWire,shape) -value $draw \
		-command "EdPhaseWireUpdate SetShape" \
		-indicatoron 0} $Gui(WCA)
	pack $f.f.r$shape -side left 
    }
    
    # Apply
    eval {button $f.bApply -text "Apply" \
	    -command "EdPhaseWireApply"} $Gui(WBA) {-width 8}
    TooltipAdd $f.bApply \
	    "Apply the PhaseWire contour you have drawn."
    
    
    pack $f.f $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # TabbedFrame->Advanced frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fAdvanced

    frame $f.fSettings   -bg $Gui(activeWorkspace)
    pack $f.fSettings -side top -fill x

    frame $f.fTrainingFile   -bg $Gui(activeWorkspace)
    pack $f.fTrainingFile -side top -pady $Gui(pad) -fill x

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fAdvanced.fSettings

    frame $f.fInputImages   -bg $Gui(activeWorkspace)
    pack $f.fInputImages -side top  -pady $Gui(pad) -fill x

    frame $f.fPhase   -bg $Gui(activeWorkspace)
    pack $f.fPhase -side top  -pady $Gui(pad) -fill x

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->InputImages frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fAdvanced.fSettings.fInputImages

    eval {button $f.bPopup -text "View Edges" \
	    -command "EdPhaseWireRaiseEdgeImageWin"} $Gui(WBA) {-width 12}
    pack $f.bPopup -side top -pady $Gui(pad) 
    TooltipAdd $f.bPopup \
	    "View input weighted graph to PhaseWire.\nImages should emphasize desired features as low costs, or dark areas."


    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Phase frame
    #-------------------------------------------
    set f $Ed(EdPhaseWire,frame).fTabbedFrame.fAdvanced.fSettings.fPhase

    # PHASE
    eval {button $f.bClr -text " invisible PhaseWire tail " \
	    -command "EdPhaseWirePrettyPicture"} $Gui(WBA)
    pack $f.bClr -side top -pady $Gui(pad)
    TooltipAdd $f.bClr \
	    "Don't show the tail of the PhaseWire, for screen shots"
   
    eval {label $f.lPW -text "P:"} $Gui(WLA)
    eval {label $f.lCW -text "C:"} $Gui(WLA)
    eval {label $f.lGW -text "T:"} $Gui(WLA)
    eval {label $f.lCU -text "CU:"} $Gui(WLA)
    eval {label $f.lCL -text "CL:"} $Gui(WLA)
    eval {entry $f.ePW -width 2 -textvariable Ed(EdPhaseWire,phaseWeight)} $Gui(WEA)
    eval {entry $f.eCW -width 2 -textvariable Ed(EdPhaseWire,certWeight)} $Gui(WEA)
    eval {entry $f.eGW -width 2 -textvariable Ed(EdPhaseWire,gradWeight)} $Gui(WEA)
    eval {entry $f.eCU -width 6 -textvariable Ed(EdPhaseWire,certUpperCutoff)} $Gui(WEA)
    eval {entry $f.eCL -width 6 -textvariable Ed(EdPhaseWire,certLowerCutoff)} $Gui(WEA)
    pack $f.lPW $f.ePW $f.lCW $f.eCW  $f.lGW $f.eGW $f.lCL $f.eCL $f.lCU $f.eCU -side left

}

proc EdPhaseWirePrettyPicture {}  {
    global Slice

    set s $Slice(activeID)

    set invis [Ed(EdPhaseWire,lwPath$s) GetInvisibleLastSegment]
    if {$invis == 1} {
	puts "turning invis off"
	set invis 0
    } else {
	puts "turning invis on"
	set invis 1
    }

    Ed(EdPhaseWire,lwPath$s) SetInvisibleLastSegment $invis

}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireRaiseEdgeImageWin
# Displays \"edge image,\" which shows edge weights (costs)
# that are derived from the image.
# Boundaries of interest should be enhanced in these images.
# This proc creates the window with GUI and sets inputs for display.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireRaiseEdgeImageWin {} {
    global Slice Ed

    set s $Slice(activeID)
    set w .phaseWireEdgeWin$s

    # if already created, raise and return
    if {[winfo exists $w] != 0} {
	raise $w
	return
    }

    # make the pop up window
    toplevel $w

    # top frame
    frame $w.fTop
    pack $w.fTop -side top

    # bottom (controls) frame
    frame $w.fBottom
    pack $w.fBottom -side top -fill both
    
    # left-hand frame
    frame $w.fTop.fLeft
    pack $w.fTop.fLeft -side left

    # right-hand frame
    frame $w.fTop.fRight
    pack $w.fTop.fRight -side left

    #-------------------------------------------
    # Left frame
    #-------------------------------------------

    # put image viewer in it
    #frame $w.fTop
    set f $w.fTop.fLeft
    vtkTkImageViewerWidget $f.v$s -width 256 -height 256 \
	    -iv Ed(EdPhaseWire,viewer$s)
    pack $f.v$s -side left -fill both
    bind $f.v$s <Expose> {ExposeTkImageViewer %W %x %y %w %h}
    set viewerWidget $f.v$s

    #-------------------------------------------
    # Right frame
    #-------------------------------------------
    set f $w.fTop.fRight

    # histogram
    ##source /scratch/src/slicer/program/vtkHistogramWidget.tcl
    #source vtkHistogramWidget.tcl
    #set hist [vtkHistogramWidget $f.hist]
    #scan [[Ed(EdPhaseWire,viewer$s) GetInput] GetExtent] "%d %d %d %d %d %d" x1 x2 y1 y2 z1 z2
    ## this should match the first image displayed
    #HistogramWidgetSetInput $hist [Ed(EdPhaseWire,viewer$s) GetInput]
    #HistogramWidgetSetExtent $hist $x1 $x2 $y1 $y2 $z1 $z2
    #pack $hist -side left -padx 3 -pady 3 -fill both -expand t
    #HistogramWidgetBind $f.hist

    # save vars 
    #set Ed(EdPhaseWire,edgeHistWidget$s) $hist

    #-------------------------------------------
    # Bottom frame
    #-------------------------------------------

    # window/level controls   
    set win [Ed(EdPhaseWire,viewer$s) GetColorWindow]
    set lev [Ed(EdPhaseWire,viewer$s) GetColorLevel]
    set Ed(EdPhaseWire,viewerWindow$s) $win
    set Ed(EdPhaseWire,viewerLevel$s) $lev

    frame $w.fBottom.fwinlevel
    set f $w.fBottom.fwinlevel
    frame $f.f1
    label $f.f1.windowLabel -text "Window"
    scale $f.f1.window -from 1 -to [expr $win * 2]  \
	    -variable Ed(EdPhaseWire,viewerWindow$s) \
	    -orient horizontal \
	    -command "Ed(EdPhaseWire,viewer$s) SetColorWindow"
    frame $f.f2
    label $f.f2.levelLabel -text "Level"
    scale $f.f2.level -from [expr $lev - $win] -to [expr $lev + $win] \
	    -variable Ed(EdPhaseWire,viewerLevel$s) \
	    -orient horizontal \
	    -command "Ed(EdPhaseWire,viewer$s) SetColorLevel"
    pack $f -side top
    pack $f.f1 $f.f2 -side top
    pack $f.f1.windowLabel $f.f1.window -side left
    pack $f.f2.levelLabel $f.f2.level -side left
    
    # radiobuttons switch between edge images
    frame $w.fBottom.fedgeBtns
    set f $w.fBottom.fedgeBtns
    label $f.lradio -text "Edge Direction"
    pack $f.lradio -side left

    # hard-code the number of inputs to grab from livewire for now:
    set edges {0 1 2 3 4 5 6 7}
    set Ed(EdPhaseWire,edge$s) 0
    foreach edge $edges text $edges {
	radiobutton $f.r$edge -width 2 -indicatoron 0\
		-text "$text" -value "$edge" \
		-variable Ed(EdPhaseWire,edge$s) \
		-command "EdPhaseWireUpdateEdgeImageWin $viewerWidget $edge"
	pack $f.r$edge -side left -fill x -anchor e
    }
    pack $f -side top

    # make save image button
    frame $w.fBottom.fSave
    set f $w.fBottom.fSave
    button $f.b -text "Save Edge Image" -command EdPhaseWireWriteEdgeImage
    pack $f -side top
    pack $f.b -side top -fill x

    # make close button
    frame $w.fBottom.fcloseBtn
    set f $w.fBottom.fcloseBtn
    button $f.b -text Close -command "lower $w"
    pack $f -side top
    pack $f.b -side top -fill x

}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireUpdateEdgeImageWin
# Display the edge image from the requested filter 
# (up, down, left, or right edges can be shown).
# .ARGS
# widget viewerWidget what to render
# int edgeNum number of the edge direction
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireUpdateEdgeImageWin {viewerWidget edgeNum} {
    global Slice Ed
    
    set s $Slice(activeID)
    
    # test inputs to sum filter here like this
    #Ed(EdPhaseWire,viewer$s) SetInput [Ed(EdPhaseWire,imageSumFilter) GetInput $edgeNum]
    # get input from livewire and show image
    set inputNum [expr $edgeNum + 1]
    Ed(EdPhaseWire,viewer$s) SetInput [Ed(EdPhaseWire,lwPath$s) GetInput $inputNum]

    $viewerWidget Render

    # histogram
    #HistogramWidgetSetInput $Ed(EdPhaseWire,edgeHistWidget$s) \
#	    [Ed(EdPhaseWire,viewer$s) GetInput]
    #scan [[Ed(EdPhaseWire,viewer$s) GetInput] GetExtent] \
	#    "%d %d %d %d %d %d" x1 x2 y1 y2 z1 z2
    #HistogramWidgetSetExtent $Ed(EdPhaseWire,edgeHistWidget$s) \
	#    $x1 $x2 $y1 $y2 $z1 $z2
    #HistogramWidgetRender $Ed(EdPhaseWire,edgeHistWidget$s)
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireWriteEdgeImage
# Dump edge image to a file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireWriteEdgeImage {} {
    global Ed Slice
    
    set s $Slice(activeID)
    # currently chosen edge dir on GUI
    set edge $Ed(EdPhaseWire,edge$s)
    if {$edge == ""} {
	set edge 0
    }
    # get filename
    set filename "edgeImage${edge}.001"
    

    # save it  (ppm default now)
    vtkImageCast cast
    #cast SetInput [Ed(EdPhaseWire,imageSumFilter) GetInput $edge]
    cast SetInput [Ed(EdPhaseWire,lwPath$s) GetInput $edge]
    #cast SetInput [Ed(EdPhaseWire,lwSetup$s) GetEdgeImage $edge]

    cast SetOutputScalarTypeToUnsignedChar

    vtkPNMWriter writer
    writer SetInput [cast GetOutput]
    writer SetFileName $filename
    writer Write

    cast Delete
    writer Delete
    tk_messageBox -message \
	    "Saved image as $filename in dir where slicer was run.\nOpen image as unsigned char to view in slicer."

}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireStartPipeline
# Sets up filters to get input from Slicer (vtkMrmlSlicer) object.
# Updates the pipeline.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireStartPipeline {} {
    global Ed Slice Gui Volume

    set Gui(progressText) "PhaseWire Initialization"	

    set e EdPhaseWire

    # tell the slicer to overlay our filter output on the fore layer
    # (the Working volume)
    Slicer FilterOverlayOn
    
    # Layers: Back=Original, Fore=Working
    # The original volume needs to be the filter input
    Slicer BackFilterOn
    Slicer ForeFilterOff

    # only apply filters to active slice
    Slicer FilterActiveOn

    EdPhaseWireUsePhasePipeline

    # force upper slicer pipeline to execute
    Slicer ReformatModified
    # update slicer object (this gives the 3 main reformatted slices...)
    Slicer Update
}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireStopPipeline
# Shut down the pipeline that hooks into the slicer object
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireStopPipeline {} {
    global Ed Volume

    Slicer FilterOverlayOff
    Slicer BackFilterOff
    Slicer ForeFilterOff
    Slicer ReformatModified
    Slicer Update

    # Stop reformatting phase and cert volumes
    Slicer RemoveAllVolumesToReformat
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireEnter
# Called upon entering module.
# Forces pipeline to update initially.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireEnter {} {
    global Ed Label Slice Editor

    # make sure we've got phase
    if {[EdPhaseWireFindInputPhaseVolumes] == "" } {
	tk_messageBox -message "Cannot find phase and cert volumes"
	return
    }

    # we are drawing in the label layer, so it had
    # better be visible
    if {$Editor(display,labelOn) == 0} {
	MainSlicesSetVolumeAll Label [EditorGetWorkingID]
    }

    # ignore mouse movement until we have a start point
    set Ed(EdPhaseWire,pipelineActiveAndContourStarted) 0

    # set up Slicer pipeline
    EdPhaseWireStartPipeline

    # keep track of active slice to reset contour if slice changes
    set Ed(EdPhaseWire,activeSlice) $Slice(activeID)

    # Make sure we're colored
    LabelsColorWidgets

    # make sure we're drawing the right color
    foreach s $Slice(idList) {
	Ed(EdPhaseWire,lwPath$s) SetLabel $Label(label)
    }

    # use slicer object to draw (like in EdDraw)
    set e EdPhaseWire
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
# .PROC EdPhaseWireExit
# 
# Called upon leaving module.  Shuts down filter pipeline
# that was displaying the interactive PhaseWire over images.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireExit {} {
    global Ed Slice

    # reset PhaseWire drawing
    foreach s $Slice(idList) {
	EdPhaseWireResetSlice $s
    }

    # no more filter pipeline
    EdPhaseWireStopPipeline

    EdPhaseWireRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireUpdate
# Update after user changes parameters in Basic GUI.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireUpdate {type} {
    global Ed Label Slice
    
    set e EdPhaseWire

    switch $type {
	SetShape {
	    Slicer DrawSetShapeTo$Ed($e,shape)
	    set Ed($e,shape) [Slicer GetShapeString]
	}
    }

}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireB1
# When mouse is clicked, pass location to live wire filter.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireB1 {x y} {
    global Ed Slice

    set s $Slice(activeID)

    set Ed(EdPhaseWire,pipelineActiveAndContourStarted) 1
    
    # if we just changed to this slice
    if {$Ed(EdPhaseWire,activeSlice) != $s} {
	set Ed(EdPhaseWire,activeSlice) $s
    }
    
    # tell the livewire filter its start point
    Ed(EdPhaseWire,lwPath$s) SetStartPoint $x $y
    
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireMotion
# When mouse moves over slice, if pipeline is operational, 
# pass end point to live wire filter and then render the slice.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireMotion {x y} {
    global Ed Slice

    set s $Slice(activeID)
    
    # if no first click to begin contour, do nothing
    if {$Ed(EdPhaseWire,pipelineActiveAndContourStarted) == 0} {
	return
    }
	    
    Ed(EdPhaseWire,lwPath$s) SetEndPoint $x $y
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireRenderInteractive
# Render whatever the user has asked to render (on GUI: either 1 slice
# 3 slices, or 3D...)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireRenderInteractive {} {
    global Ed
    
    Render$Ed(EdPhaseWire,render)
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireLabel
# Called when label changes. Gives right label number to livewire and 
# slicer objects
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireLabel {} {
    global Color Label Slice
    
    LabelsFindLabel
	
    if {$Label(activeID) != ""} {
	set color [Color($Label(activeID),node) GetDiffuseColor]
	eval Slicer DrawSetColor $color
    } else {
	Slicer DrawSetColor 0 0 0
    }

    # update filter stuff    
    foreach s $Slice(idList) {
	Ed(EdPhaseWire,lwPath$s) SetLabel $Label(label)	
    }

    # render whatever we are supposed to (slice, 3slice, or 3D)
    EdPhaseWireRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireClearCurrentSlice
# Clears contour from filters and makes slice redraw
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireClearCurrentSlice {} {
    global Slice
    
    set s $Slice(activeID)
    EdPhaseWireResetSlice $s
    Slicer Update
    RenderSlice $s
}


proc EdPhaseWireClearLastSegment {} {
    global Slice
    
    set s $Slice(activeID)
    # reset latest contour points
    Ed(EdPhaseWire,lwPath$s) ClearLastContourSegment

    Slicer Update
    RenderSlice $s
}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireResetSlice
# Clear the previous contour to start over with new start point.
# After, must do Slicer Update  and  RenderSlice $s
# to clear the slice (just call EdPhaseWireClearCurrentSlice to do it all)
# .ARGS
# int s number of the slice
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireResetSlice {s} {
    global Ed

    # reset contour points
    Ed(EdPhaseWire,lwPath$s) ClearContour
    # ignore mouse motion until user clicks a slice
    set Ed(EdPhaseWire,pipelineActiveAndContourStarted) 0
}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireApply
# Actually draw the polygon, line, whatever, into the volume.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireApply {} {
    global Ed Volume Label Gui Slice

    set s $Slice(activeID)

    # if there are no points, do nothing
    set rasPoints [Ed(EdPhaseWire,lwPath$s) GetContourPixels]
    if {[$rasPoints GetNumberOfPoints] == 0} {
	puts "no points to apply!"
	return
    }
    
    set e EdPhaseWire

    # the working volume is editor input (where we want to draw)
    # (though the original vol is input to the PhaseWire filters.)
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

    # standard editor function must be called
    EdSetupBeforeApplyEffect $v $Ed($e,scope) Active

    # text over blue progress bar
    set Gui(progressText) "PhaseWire [Volume($v,node) GetName]"	

    # attributes of region to draw
    set label    $Label(label)
    set radius   $Ed($e,radius)
    set shape    $Ed($e,shape)

    # Give points to slicer object to convert to ijk
    set numPoints [$rasPoints GetNumberOfPoints]
    for {set p 0} {$p < $numPoints} {incr p} {
	scan [$rasPoints GetPoint $p] "%d %d %d" x y z
	Slicer DrawInsertPoint $x $y
    }
    Slicer DrawComputeIjkPoints
    set points [Slicer GetDrawIjkPoints]

    # give ijk points to editor object to actually draw them into the volume
    Ed(editor)   Draw $label $points $radius $shape
    
    # clear points that livewire object was storing
    EdPhaseWireResetSlice $s

    # reset editor object
    Ed(editor)     SetInput ""
    Ed(editor)     UseInputOff

    # standard editor function must be called
    EdUpdateAfterApplyEffect $v $Ed($e,render)

    # always delete points 
    Slicer DrawDeleteAll
}


#-------------------------------------------------------------------------------
# .PROC EdPhaseWireStartProgress
#
#  Wrapper around MainStartProgress (Does Nothing)
#
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireStartProgress {} {
    global Gui

    puts -nonewline $Gui(progressText)
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireShowProgress
# Progress method callback for vtk filters.
# Wrapper around MainShowProgress, which shows the blue bar.
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireShowProgress {filter} {

    puts -nonewline "."

}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireEndProgress
#
# Wrapper around MainEndProgress.
# 
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireEndProgress {} {
    global Ed

    puts ""
    
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireUseDistanceFromPreviousContour
#  not finished.  This will make a distance map for input to PhaseWire
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireUseDistanceFromPreviousContour {} {
    global Ed Slice
    
    set s $Slice(activeID)

    # image we need is the previous one from the Working
    # volume (the last slice a contour was drawn on)
    # Lauren: may need to add OR subtract 1, depending!
    set offset [expr $Slice($s,offset) - 1]
    puts "offset: $offset"
    
    EdPhaseWireGetContourSlice $offset

    # test image is okay
    Ed(EdPhaseWire,viewer$s) SetColorWindow 15
    Ed(EdPhaseWire,viewer$s) SetColorLevel 5
    Ed(EdPhaseWire,viewer$s) SetInput $Ed(EdPhaseWire,contourSlice)

}


proc EdPhaseWireFindInputPhaseVolumes {} {
    global Ed Volume

    # figure out which volumes to use for the phase and cert
    # inputs.  Now this code just expects they will be 
    # named phase and cert.

    # numbers of the volumes
    set phaseVol ""
    set certVol ""

    # names we expect the volumes to have
    set phase "phase"
    set cert "cert"

    foreach v $Volume(idList) {
	set n Volume($v,node)
	set name [$n GetName]
	#puts $name
	if {$name == $phase} {
	    set phaseVol $v
	}
	if {$name == $cert} {
	    set certVol $v
	}
    }
    
    if {$phaseVol == "" || $certVol == ""} {
	puts "can't find phase and cert volumes named $phase and $cert"
	return ""
    }

    set Ed(EdPhaseWire,phaseVol) $phaseVol
    set Ed(EdPhaseWire,certVol) $certVol

    return 0
}


proc EdPhaseWireUsePhasePipeline {} {
    global Ed Slice

    set e EdPhaseWire

    foreach s $Slice(idList) {
	
	# get input grayscale images from Slicer object
	Slicer SetFirstFilter $s Ed($e,gradMag$s)
	# put our output over the slice (so the wire is visible)
	Slicer SetLastFilter  $s Ed(EdPhaseWire,lwPath$s)  

	# test this: all input vals > 1000 will be set to 1000.
	Ed($e,certNorm$s) SetLowerCutoff $Ed(EdPhaseWire,certLowerCutoff)
	Ed($e,certNorm$s) SetUpperCutoff $Ed(EdPhaseWire,certUpperCutoff)

	# NOTE: no on-the-fly training is in use for now.
	# Lauren try training someday
	# training setup:
	# train on all pixels from current wire (can limit number later)
	# Ed($e,gradNorm$s) SetLookupPoints [Ed(EdPhaseWire,lwPath$s) GetContourPixels]
	# Ed($e,gradNorm$s) SetUseLookupTable 1; # estimate distribution
	#Ed($e,gradNorm)  SetUseGaussianLookup 1; # use simple gaussian model
	
	# use current settings from Advanced GUI for combining images
	set sum Ed($e,imageSumFilter$s)
	$sum SetWeightForInput 0 $Ed($e,phaseWeight)
	$sum SetWeightForInput 1 $Ed($e,certWeight)
	$sum SetWeightForInput 2 $Ed($e,gradWeight)
    }
    
    # get input phase and cert images from Slicer object
    set v Volume($Ed(EdPhaseWire,phaseVol),vol)
    # tell the slicer to reformat this volume tracking the active slice
    Slicer AddVolumeToReformat $v
    foreach s $Slice(idList) {
	Ed($e,phaseNorm$s)  SetInput [Slicer GetReformatOutputFromVolume $v]
    }

    set v Volume($Ed(EdPhaseWire,certVol),vol)
    Slicer AddVolumeToReformat $v
    foreach s $Slice(idList) {
	Ed($e,certNorm$s)  SetInput [Slicer GetReformatOutputFromVolume $v]
    }


    # update slicer
    Slicer ReformatModified
    Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC EdPhaseWireTestCF2
# old code, keeping this here for the record (temporarily)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdPhaseWireTestCF2 {} {
    global Ed Volume Slice

    set phaseVol ""
    set certVol ""

    set orig [EditorGetInputID Original]
    set origname [Volume($orig,node) GetName]

    if {[regexp {(.*)var(.*)} $origname match type num] == 0} {
	puts "using default names phase and cert"
	set phase "phase"
	set cert "cert"
    } else {
	set phase "phasevar$num"
	set cert "certvar$num"
    }
    puts "phase: $phase, cert: $cert"

    # figure out which volumes to use: hack for now
    foreach v $Volume(idList) {
	set n Volume($v,node)
	set name [$n GetName]
	puts $name
	if {$name == $phase} {
	    set phaseVol $v
	}
	if {$name == $cert} {
	    set certVol $v
	}
    }
    
    if {$phaseVol == "" || $certVol == ""} {
	puts "can't find phase and cert volumes"
	return
    }

    # reformat the right slices
    set s $Slice(activeID)
    vtkImageReformat phaseReformat
    vtkImageReformat certReformat
    EdPhaseWireReformatSlice $Slice($s,offset) phaseReformat $phaseVol
    EdPhaseWireReformatSlice $Slice($s,offset) certReformat $certVol

    # compute the gradient magnitude
    if {[info commands "gradMag"] == ""} {
	# we haven't made the object yet...
	# really do this in build vtk
	vtkImageGradientMagnitude gradMag
	# PHASE
	vtkImageWeightedSum Ed(EdPhaseWire,imageSumFilter)
    }
    # get input image from Slicer object
    gradMag SetInput [Ed(EdPhaseWire,lwSetup$s) GetInput] ; # hack!!
    #Slicer SetFirstFilter $s gradMag
    #Slicer SetLastFilter  $s Ed(EdPhaseWire,lwPath$s)  
    #Slicer Update

    # normalize...
    vtkImageLiveWireScale phaseNorm
    vtkImageLiveWireScale certNorm
    vtkImageLiveWireScale gradNorm
    phaseNorm SetInput [phaseReformat GetOutput]     

    certNorm SetInput [certReformat GetOutput] 
    #certNorm SetTransformationFunctionToOneOverX
    certNorm SetTransformationFunctionToInverseLinearRamp
    # test this: all input vals > 1000 will be set to 1000.
    certNorm SetLowerCutoff $Ed(EdPhaseWire,certLowerCutoff)
    certNorm SetUpperCutoff $Ed(EdPhaseWire,certUpperCutoff)

    # Lauren 
    # test whether cert works better than gradient mag!
    #gradNorm SetInput [gradMag GetOutput]     

   #  gradNorm SetTransformationFunctionToInverseLinearRamp ; # in case LUT fails, do this
    
    # Lauren test this
    gradNorm SetTransformationFunctionToOneOverX

    # test with all old pixels first (limit the number later)
    gradNorm SetLookupPoints [Ed(EdPhaseWire,lwPath$s) GetContourPixels]
    #gradNorm SetUseLookupTable 1
    gradNorm SetUseGaussianLookup 1

    set scale [Ed(EdPhaseWire,lwPath$s) GetMaxEdgeCost]
    phaseNorm SetScaleFactor $scale
    certNorm SetScaleFactor  $scale
    gradNorm SetScaleFactor  $scale

    # ___________________test_____________
    # Lauren 
    # test whether cert works better than gradient mag!
    #gradNorm SetInput [certNorm GetOutput]     
    vtkImageLiveWireScale testNorm
    testNorm SetScaleFactor  $scale
    testNorm SetInput [certReformat GetOutput]     

    gradNorm SetInput [testNorm GetOutput]     
    testNorm Delete
    #gradNorm SetInput [certReformat GetOutput]     
    # ___________________end test_____________

    # do weighted sum of these
    #vtkImageWeightedSum sum
    set sum Ed(EdPhaseWire,imageSumFilter)
    $sum SetInput 0 [phaseNorm GetOutput]
    $sum SetInput 1 [certNorm GetOutput]
    $sum SetInput 2 [gradNorm GetOutput]
    puts "set inputs to sum"

    # Lauren!
    if {[info exists Ed(EdPhaseWire,phaseWeight)] == 0 } {

	set Ed(EdPhaseWire,phaseWeight) 1
	set Ed(EdPhaseWire,certWeight) 1
	set Ed(EdPhaseWire,gradWeight) 1
    }

    $sum SetWeightForInput 0 $Ed(EdPhaseWire,phaseWeight)
    $sum SetWeightForInput 1 $Ed(EdPhaseWire,certWeight)
    $sum SetWeightForInput 2 $Ed(EdPhaseWire,gradWeight)
    puts "set weights for sum"

    # screw the edge filters, just give this stuff to lw filter
    
    # set all lw inputs to be just this!
    set PhaseWire Ed(EdPhaseWire,lwPath$s)
    # Lauren change numEdgeFilters to be called num edge directions...
    set totalInputs [expr $Ed(EdPhaseWire,numEdgeFilters) + 1]
    for {set e 0} {$e < $totalInputs} {incr e} {   
	
	# PhaseWire wants input 0 to be "original image" 
	# (no good reason, not used except to make pipeline update, or something...)
	# wants next (4 or 8) inputs to be directional info

	# must set input 0 since that's where LW gets type info now!!!

	$PhaseWire SetInput $e [$sum GetOutput]
    }

    # clean up
    phaseReformat Delete
    certReformat Delete
    phaseNorm Delete
    certNorm Delete
    gradNorm Delete
    #sum Delete
    puts "almost done!"

    # update slicer
    Slicer Update
    
    puts "woo hoo!!!"
}
