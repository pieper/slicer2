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
# FILE:        EdLiveWire.tcl
# PROCEDURES:  
#   EdLiveWireInit
#   EdLiveWireBuildVTK
#   EdLiveWireBuildGUI
#   EdLiveWireSetActiveEdgeDirection
#   EdLiveWireToggleWeight feat
#   EdLiveWireRaiseEdgeImageWin
#   EdLiveWireUpdateEdgeImageWin viewerWidget edgeNum
#   EdLiveWireWriteEdgeImage
#   EdLiveWireGetFeatureParams
#   EdLiveWireSetFeatureParams edgedir
#   EdLiveWireSetFeatureParamsForAllFilters
#   EdLiveWireAdvancedApply
#   EdLiveWireAdvancedApplyToAll
#   EdLiveWireStartPipeline
#   EdLiveWireStopPipeline
#   EdLiveWireEnter
#   EdLiveWireExit
#   EdLiveWireUpdateInteractive
#   EdLiveWireSetInteract
#   EdLiveWireUpdateInit
#   EdLiveWireUpdate
#   EdLiveWireB1
#   EdLiveWireMotion
#   EdLiveWireB1Motion
#   EdLiveWireRenderInteractive
#   EdLiveWireLabel
#   EdLiveWireClearCurrentSlice
#   EdLiveWireResetSlice s
#   EdLiveWireApply
#   EdLiveWireStartProgress
#   EdLiveWireShowProgress
#   EdLiveWireEndProgress
#   EdLiveWireSetMode
#   EdLiveWireReformatSlice offset reformat volume
#   EdLiveWireUseDistanceFromPreviousContour
#   EdLiveWireGetContourSlice offset
#   EdLiveWireTrain
#   EdLiveWireReadFeatureParams
#   EdLiveWireWriteFeatureParams
#   EdLiveWireTestCF
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdLiveWireInit
# Automatically called init procedure
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireInit {} {
    global Ed Gui
    
    set e EdLiveWire
    set Ed($e,name)      "LiveWire"
    set Ed($e,initials)  "Lw"
    set Ed($e,desc)      "LiveWire: quick 2D segmentation."
    # order this editor module will appear in the list
    set Ed($e,rank)      9
    set Ed($e,procGUI)   EdLiveWireBuildGUI
    set Ed($e,procVTK)   EdLiveWireBuildVTK
    set Ed($e,procEnter) EdLiveWireEnter
    set Ed($e,procExit)  EdLiveWireExit

    # Required
    set Ed($e,scope)  Single
    # Original volume is input to our filters.
    set Ed($e,input)  Original
    
    # drawing vars
    set Ed($e,radius) 0
    set Ed($e,shape)  Polygon
    set Ed($e,render) Active

    # training vars
    set Ed($e,trainingRadius) 0
    set Ed($e,trainingOutputFileName) "LiveWireSettings.xml"
    set Ed($e,trainingInputFileName) "LiveWireSettings.xml"

    # slider range
    set Ed($e,sliderLow) 0.0
    set Ed($e,sliderHigh) 1.0
    
    # active slice
    set Ed($e,interact) Active
    
    # mode of user interaction (choices are LiveWire or Training)
    set Ed($e,mode) LiveWire
    set Ed($e,previousMode) LiveWire

    # mode of acquiring training points image 
    # choices are CurrentSlice, DrawOnSlice, or RangeOfSlices
    set Ed(EdLiveWire,trainingMode) CurrentSlice
    
    # first and last slices to use for training
    set Ed(EdLiveWire,trainingSlice1) 0
    set Ed(EdLiveWire,trainingSlice2) 0

    # edge direction displayed and modified in advanced gui
    set Ed(EdLiveWire,activeEdgeDirection) 0

    # for writing settings as option
    set Ed(EdLiveWire,contents) "LiveWireSettings"
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireBuildVTK
# build vtk objects used in this module
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireBuildVTK {} {
    global Ed Label Slice Volume
    
    # Lauren rename these dumb things!

    set fakeInput [Volume($Volume(idNone),vol) GetOutput]

    foreach s $Slice(idList) {    
	# this filter feeds edges to live wire
	vtkImageLiveWireTester Ed(EdLiveWire,lwSetup$s)

	# this filter finds short paths using edges
	vtkImageLiveWire Ed(EdLiveWire,lwPath$s)

	# these filters find directional edge maps
	set Ed(EdLiveWire,numEdgeFilters) \
		[Ed(EdLiveWire,lwSetup$s) GetNumberOfEdgeFilters]

	# attach progress callback to first filter only
	set f 0
	set filt [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $f]
	$filt SetStartMethod EdLiveWireStartProgress
	$filt SetProgressMethod \
		"EdLiveWireShowProgress $filt"
	$filt SetEndMethod EdLiveWireEndProgress

	# debug
	Ed(EdLiveWire,lwPath$s)  SetVerbose 0

	# pipeline:
	Ed(EdLiveWire,lwSetup$s) SetLiveWire Ed(EdLiveWire,lwPath$s)

	# Lauren is this needed?
	# give a blank input for now, just to get things connected
	Ed(EdLiveWire,lwSetup$s) SetInput $fakeInput
	# now hook up edge filters and livewire filters
	Ed(EdLiveWire,lwSetup$s) InitializePipeline

	# LiveWire short path filter gets 0th input from setup filter
	Ed(EdLiveWire,lwPath$s) SetOriginalImage [Ed(EdLiveWire,lwSetup$s) GetOutput]

	# for looking at edge weight image
	vtkImageViewer Ed(EdLiveWire,viewer$s)
	Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage 0]
	#Ed(EdLiveWire,viewer$s) SetInput [Slicer GetActiveOutput $s]
	Ed(EdLiveWire,viewer$s) SetColorWindow 256
	Ed(EdLiveWire,viewer$s) SetColorLevel 127.5
	[Ed(EdLiveWire,viewer$s) GetImageWindow] DoubleBufferOn
    }
    
    # initialize tcl variables that mirror vtk object settings
    EdLiveWireGetFeatureParams

    # some filters that are regularly used
    #vtkImageMathematics Ed(EdLiveWire,imageMath)
    vtkImageThresholdBeyond Ed(EdLiveWire,imageThreshold)

    # make list of edge numbers
    set Ed(EdLiveWire,edgeIDList) ""
    for {set i 0} {$i < $Ed(EdLiveWire,numEdgeFilters)} {incr i} {
	lappend  Ed(EdLiveWire,edgeIDList) $i
    }


}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireBuildGUI
# build GUI of module
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireBuildGUI {} {
    global Ed Gui Label Volume

    set f $Ed(EdLiveWire,frame)

    # this makes the navigation menu (buttons) and the tabs (frames).
    #set label "Settings:"
    set label ""
    set subframes {Basic Training Advanced}
    set buttonText {"Basic" "Train" "Settings"}
    set tooltips {"Basic: For Users" \
	    "Train LiveWire using segmented slices and save the settings." \
	    "Current LiveWire Settings: Advanced"}

    TabbedFrame EdLiveWire $f $label $subframes $buttonText \
	    $tooltips

    #-------------------------------------------
    # TabbedFrame->Basic frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic
    
    # Standard stuff
    frame $f.fInteract  -bg $Gui(activeWorkspace)
    frame $f.fGrid      -bg $Gui(activeWorkspace)
    frame $f.fContour   -bg $Gui(activeWorkspace)
    frame $f.fReset   -bg $Gui(activeWorkspace)
    frame $f.fApply     -bg $Gui(activeWorkspace)
    frame $f.fTest     -bg $Gui(activeWorkspace)
    frame $f.fFile     -bg $Gui(activeWorkspace)
    pack $f.fFile $f.fGrid $f.fInteract $f.fContour $f.fReset  \
	    $f.fApply $f.fTest \
	    -side top -pady $Gui(pad) -fill x
    #pack $f.fFile -side top  -pady [expr 3*$Gui(pad)] -fill x

    # Standard Editor interface buttons
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
    # TabbedFrame->Basic->Contour frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fContour
    eval {button $f.bContour -text "Stay near last slice's contour" \
	    -command {puts "this feature is coming soon."}} $Gui(WBA)
    # Lauren implement this!
    #pack $f.bContour
    #TooltipAdd $f.bContour \
	    "Keep the LiveWire near the contour you drew on the previous slice."

    #-------------------------------------------
    # TabbedFrame->Basic->Reset Frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fReset
    eval {button $f.bReset -text "Clear Contour" \
	    -command "EdLiveWireClearCurrentSlice"} $Gui(WBA)
    pack $f.bReset
    TooltipAdd $f.bReset \
	    "Start over with a new LiveWire."

    #-------------------------------------------
    # TabbedFrame->Basic->Apply frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fApply
    
    eval {button $f.bApply -text "Apply" \
	    -command "EdLiveWireApply"} $Gui(WBA) {-width 8}
    pack $f.bApply
    TooltipAdd $f.bApply \
	    "Apply the LiveWire contour you have drawn."

    #-------------------------------------------
    # TabbedFrame->Basic->Test frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fTest
    
    #eval {button $f.bTest -text "Test quadrature filter input on slice" \
	    #-command "EdLiveWireTestCF"} $Gui(WBA) {-width 30}
    #pack $f.bTest
    #TooltipAdd $f.bTest \
	    #"hello"

    #-------------------------------------------
    # TabbedFrame->Basic->File frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fBasic.fFile

    DevAddFileBrowse $f Ed "EdLiveWire,trainingInputFileName" \
	    "Open Settings File:" EdLiveWireReadFeatureParams \
	    "xml" [] "Open" "Open LiveWire Settings File" \
	    "Read LiveWire settings for the structure you want to segment."\
	    "Absolute"

    #-------------------------------------------
    # TabbedFrame->Training frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining

    frame $f.fGrid   -bg $Gui(activeWorkspace)
    pack $f.fGrid -side top -pady $Gui(pad) -fill x
    frame $f.fSlices   -bg $Gui(activeWorkspace)
    pack $f.fSlices -side top -pady $Gui(pad) -fill x
    frame $f.fTrainingMode   -bg $Gui(activeWorkspace)
    pack $f.fTrainingMode -side top -pady $Gui(pad) -fill x    
    frame $f.fTrainingFile   -bg $Gui(activeWorkspace)
    pack $f.fTrainingFile -side top -pady $Gui(pad) -fill x
    frame $f.fTrain   -bg $Gui(activeWorkspace)
    pack $f.fTrain -side top -pady $Gui(pad) -fill x    

    #-------------------------------------------
    # TabbedFrame->Training->TrainingFile frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining.fTrainingFile

    DevAddFileBrowse $f Ed "EdLiveWire,trainingOutputFileName" \
	    "Save Settings As:" [] "xml" [] \
	    "Save" "Output File" \
	    "Choose the file where the training information will be written."\
	    "Absolute"

    #-------------------------------------------
    # TabbedFrame->Training->Grid frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining.fGrid
    
    # Output label
    eval {button $f.bOutput -text "Choose Label:" \
	    -command "ShowLabels EdLiveWireLabel"} $Gui(WBA)
    TooltipAdd $f.bOutput "Choose label to train on"
    eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdLiveWireLabel"
    bind $f.eOutput <FocusOut> "EdLiveWireLabel"
    TooltipAdd $f.eOutput "Choose label to train on"
    eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
	    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    #-------------------------------------------
    # TabbedFrame->Training->Slices frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining.fSlices

    set tips "{Slice number of the first slice to use for training}\
	    {Slice number of the last slice to use for training}"

    foreach lab {"first slice:" "last slice:"} s "1 2" tip $tips {
	eval {label $f.lSlice$s -text $lab} $Gui(WLA)
	eval {entry $f.eSlice$s -width 3 \
		-textvariable Ed(EdLiveWire,trainingSlice$s)} $Gui(WEA)
	TooltipAdd $f.eSlice$s $tip
	pack $f.lSlice$s $f.eSlice$s -side left -pady $Gui(pad) -padx $Gui(pad)	
    }

    #-------------------------------------------
    # TabbedFrame->Training->TrainingMode frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining.fTrainingMode

    eval {label $f.lTrainingMode -text "Train on:"} $Gui(WLA)
    pack $f.lTrainingMode -side left -pady $Gui(pad)

    set tips "{Use the contour from the current slice.}\
	    {Train from first slice to last slice.}"
	    
    foreach mode " CurrentSlice RangeOfSlices" \
	    text { "Current Slice" "Several Slices" } \
	    width "12 12" tip $tips {
	eval {radiobutton $f.r$mode -width $width -indicatoron 0\
		-text "$text" -value "$mode" \
		-variable Ed(EdLiveWire,trainingMode)} $Gui(WCA)
	pack $f.r$mode -side left -fill x -anchor e
	TooltipAdd $f.r$mode $tip
    }

    #-------------------------------------------
    # TabbedFrame->Training->Train frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fTraining.fTrain

    eval {button $f.bTrain -text "Train LiveWire from Contour" \
	    -command "EdLiveWireTrain"} $Gui(WBA) {-width 30}
    pack $f.bTrain -side top

    TooltipAdd $f.bTrain \
	    "Teach livewire using the contour you have drawn.\nSettings will be written to the file you have selected."


    #-------------------------------------------
    # TabbedFrame->Advanced frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced

    frame $f.fSettings   -bg $Gui(activeWorkspace)
    pack $f.fSettings -side top -fill x

    frame $f.fTrainingFile   -bg $Gui(activeWorkspace)
    pack $f.fTrainingFile -side top -pady $Gui(pad) -fill x

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings

    frame $f.fEdgeDir   -bg $Gui(activeWorkspace)
    pack $f.fEdgeDir -side top  -padx $Gui(pad) 

    frame $f.fFeatures   -bg $Gui(activeWorkspace)
    pack $f.fFeatures -side top  -pady $Gui(pad) -fill x

    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fApply -side top -pady $Gui(pad)

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Features frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings.fEdgeDir

    #eval {label $f.l -text "Edge Direction:"} $Gui(WLA)
    #pack $f.l -side left -padx $Gui(pad) -fill x

    set tips {"Show settings for UP edges" \
	    "Show settings for DOWN edges" \
	    "Show settings for LEFT edges" \
	    "Show settings for RIGHT edges" \
	    "Show settings for UP_LEFT edges" \
	    "Show settings for UP_RIGHT edges" \
	    "Show settings for DOWN_LEFT edges" \
	    "Show settings for DOWN_RIGHT edges" \
	      }

    #set texts {"^" "v" "<-" "->" "^\\"  "/^" "v/" "\\v"}
    set texts {"^" "v" "<-" "->" "'\\"  "/'" ",/" "\\,"}

    set widths "2 2 3 3 3 3 3 3"

    # chop lists if not this many edge filters
    #puts "edges: $Ed(EdLiveWire,numEdgeFilters)"
    #puts "length: [llength $tips]"
    if {$Ed(EdLiveWire,numEdgeFilters) < [llength $tips] } {
	set tips [lreplace $tips $Ed(EdLiveWire,numEdgeFilters) end]
	set texts [lreplace $texts $Ed(EdLiveWire,numEdgeFilters) end]
	set widths [lreplace $widths $Ed(EdLiveWire,numEdgeFilters) end]
    }
    foreach edge $Ed(EdLiveWire,edgeIDList) text $texts width $widths tip $tips {
	eval {radiobutton $f.r$edge -width $width -indicatoron 0\
		  -text "$text" -value "$edge" \
		  -variable Ed(EdLiveWire,activeEdgeDirection) \
		  -command EdLiveWireSetActiveEdgeDirection }\
	    $Gui(WCA) 
	pack $f.r$edge -side left -fill x -anchor e
	TooltipAdd $f.r$edge $tip
    }

    #-------------------------------------------
    # TabbedFrame->Advanced->Settings->Features frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fSettings
    set f $f.fFeatures

    # hard-coded feature names:
    # should match with the order in vtkImageLiveWireEdgeWeights
    set featureNames "{0: in pix} {1: out pix} {2: out-in} \
	    {3: gradient} {4: gradient} {5: gradient} {6: new feature...}"
    set paramNames "{Feature} {Weight} {Mean} {Variance}"

    set n 0
    foreach name $paramNames {
	eval {label $f.lparamName$n -text $name} $Gui(WLA)
	# 0th row is param name
	grid $f.lparamName$n -row 0 -col $n
	incr n
    }

    set buttontip "Click to toggle this feature off (weight = 0) and back on (weight = 1)"

    # loop over features
    for {set feat 0} {$feat < $Ed(EdLiveWire,numFeatures)} {incr feat} {
	
	set name [lindex $featureNames $feat]
	if {$name == ""} {
	    set $name "$feat: new feature"
	}

	set row [expr $feat +1]
	
	# 0th column is feature name (on a button that can turn feature off)
	set Ed(EdLiveWire,weightOff,$feat) 0
	eval {button $f.bfeatureName$feat -width 11 -text "$name" \
		-command "EdLiveWireToggleWeight $feat"} $Gui(WBA)
	TooltipAdd $f.bfeatureName$feat $buttontip
	grid $f.bfeatureName$feat  -row $row -column 0 -sticky w \
		-ipadx 1

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
    set f $f.fApply
    eval {button $f.bApply -text "Apply" \
	    -command "EdLiveWireAdvancedApply"} $Gui(WBA) {-width 7}
    TooltipAdd $f.bApply \
	    "Apply these settings to the current directional filter in the current slice."
    pack $f.bApply  -side left

    eval {button $f.bApplyToAll -text "ApplyToAll" \
	    -command "EdLiveWireAdvancedApplyToAll"} $Gui(WBA) {-width 10}
    TooltipAdd $f.bApplyToAll \
	    "Apply these settings to all directional filters (UP, DOWN, etc.) for the current slice."
    pack $f.bApplyToAll  -side left

    eval {button $f.bPopup -text "View Edges" \
	    -command "EdLiveWireRaiseEdgeImageWin"} $Gui(WBA) {-width 12}
    pack $f.bPopup -side left
    TooltipAdd $f.bPopup \
	    "View images of edge weights in each direction."

    #-------------------------------------------
    # TabbedFrame->Advanced->TrainingFile frame
    #-------------------------------------------
    set f $Ed(EdLiveWire,frame).fTabbedFrame.fAdvanced.fTrainingFile

    DevAddFileBrowse $f Ed "EdLiveWire,trainingOutputFileName" \
	    "Save Settings:" EdLiveWireWriteFeatureParams \
	    "xml" [] "Save" "Save LiveWire Settings" \
	    "Save LiveWire settings for the structure you are segmenting."\
	    "Absolute"
	    
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetActiveEdgeDirection
# Called when arrow button is clicked on GUI: displays 
# the settings for that filter by calling EdLiveWireGetFeatureParams
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetActiveEdgeDirection {} {
    global Ed

    #puts $Ed(EdLiveWire,activeEdgeDirection)

    # refresh values on GUI for this edge direction
    EdLiveWireGetFeatureParams
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireToggleWeight
# Set weight for feature to 0 when the button is pressed,
# then to 0 when unpressed.
# Just for user's convenience.
# .ARGS
# int feat the number of the feature
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireToggleWeight {feat} {
    global Ed
    
    if {$Ed(EdLiveWire,weightOff,$feat) == 0} {
	# if button pressed, set weight to 0
	set Ed(EdLiveWire,feature$feat,weight) 0
	# the weight is turned off now:
	set Ed(EdLiveWire,weightOff,$feat) 1
    } else {
	# if unpressing button, set weight to 1
	set Ed(EdLiveWire,feature$feat,weight) 1
	# the weight is turned on now:
	set Ed(EdLiveWire,weightOff,$feat) 0
    }

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireRaiseEdgeImageWin
# Displays \"edge image,\" which shows edge weights (costs)
# that are derived from the image.
# Boundaries of interest should be enhanced in these images.
# This proc creates the window with GUI and sets inputs for display.
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
	    -iv Ed(EdLiveWire,viewer$s)
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
    #scan [[Ed(EdLiveWire,viewer$s) GetInput] GetExtent] "%d %d %d %d %d %d" x1 x2 y1 y2 z1 z2
    ## this should match the first image displayed
    #HistogramWidgetSetInput $hist [Ed(EdLiveWire,viewer$s) GetInput]
    #HistogramWidgetSetExtent $hist $x1 $x2 $y1 $y2 $z1 $z2
    #pack $hist -side left -padx 3 -pady 3 -fill both -expand t
    #HistogramWidgetBind $f.hist

    # save vars 
    #set Ed(EdLiveWire,edgeHistWidget$s) $hist

    #-------------------------------------------
    # Bottom frame
    #-------------------------------------------

    # window/level controls   
    set win [Ed(EdLiveWire,viewer$s) GetColorWindow]
    set lev [Ed(EdLiveWire,viewer$s) GetColorLevel]
    set Ed(EdLiveWire,viewerWindow$s) $win
    set Ed(EdLiveWire,viewerLevel$s) $lev

    frame $w.fBottom.fwinlevel
    set f $w.fBottom.fwinlevel
    frame $f.f1
    label $f.f1.windowLabel -text "Window"
    scale $f.f1.window -from 1 -to [expr $win * 2]  \
	    -variable Ed(EdLiveWire,viewerWindow$s) \
	    -orient horizontal \
	    -command "Ed(EdLiveWire,viewer$s) SetColorWindow"
    frame $f.f2
    label $f.f2.levelLabel -text "Level"
    scale $f.f2.level -from [expr $lev - $win] -to [expr $lev + $win] \
	    -variable Ed(EdLiveWire,viewerLevel$s) \
	    -orient horizontal \
	    -command "Ed(EdLiveWire,viewer$s) SetColorLevel"
    pack $f -side top
    pack $f.f1 $f.f2 -side top
    pack $f.f1.windowLabel $f.f1.window -side left
    pack $f.f2.levelLabel $f.f2.level -side left
    
    # radiobuttons switch between edge images
    frame $w.fBottom.fedgeBtns
    set f $w.fBottom.fedgeBtns
    label $f.lradio -text "Edge Direction"
    pack $f.lradio -side left

    foreach edge $Ed(EdLiveWire,edgeIDList) text $Ed(EdLiveWire,edgeIDList) {
	radiobutton $f.r$edge -width 2 -indicatoron 0\
		-text "$text" -value "$edge" \
		-variable Ed(EdLiveWire,edge$s) \
		-command "EdLiveWireUpdateEdgeImageWin $viewerWidget $edge"
	pack $f.r$edge -side left -fill x -anchor e
    }
    pack $f -side top

    #"Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage $edge]; $viewer Render" 
    #"Ed(EdLiveWire,viewer$s) SetInput [Slicer GetActiveOutput $s]; $viewer Render" 

    # make save image button
    frame $w.fBottom.fSave
    set f $w.fBottom.fSave
    button $f.b -text "Save Edge Image" -command EdLiveWireWriteEdgeImage
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
# .PROC EdLiveWireUpdateEdgeImageWin
# Display the edge image from the requested filter 
# (up, down, left, or right edges can be shown).
# .ARGS
# widget viewerWidget what to render
# int edgeNum number of the edge direction
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireUpdateEdgeImageWin {viewerWidget edgeNum} {
    global Slice Ed
    
    set s $Slice(activeID)
    
    # image window
    Ed(EdLiveWire,viewer$s) SetInput [Ed(EdLiveWire,lwSetup$s) \
	    GetEdgeImage $edgeNum]
    $viewerWidget Render

    # histogram
    #HistogramWidgetSetInput $Ed(EdLiveWire,edgeHistWidget$s) \
#	    [Ed(EdLiveWire,viewer$s) GetInput]
    #scan [[Ed(EdLiveWire,viewer$s) GetInput] GetExtent] \
	#    "%d %d %d %d %d %d" x1 x2 y1 y2 z1 z2
    #HistogramWidgetSetExtent $Ed(EdLiveWire,edgeHistWidget$s) \
	#    $x1 $x2 $y1 $y2 $z1 $z2
    #HistogramWidgetRender $Ed(EdLiveWire,edgeHistWidget$s)
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireWriteEdgeImage
# Dump edge image to a file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireWriteEdgeImage {} {
    global Ed Slice
    
    set s $Slice(activeID)
    # currently chosen edge dir on GUI
    set edge $Ed(EdLiveWire,edge$s)
    if {$edge == ""} {
	set edge 0
    }
    # get filename
    set filename "edgeImage${edge}.001"
    

    # save it  (ppm default now)
    vtkImageCast cast
    cast SetInput [Ed(EdLiveWire,lwSetup$s) GetEdgeImage $edge]
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
# .PROC EdLiveWireGetFeatureParams
# initialize tcl variables that mirror vtk object settings
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireGetFeatureParams {} {
    global Ed Slice
    
    set s $Slice(activeID)
    set e $Ed(EdLiveWire,activeEdgeDirection) 

    # set up parameters for control of edge filters  
    set filter [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $e]
    set Ed(EdLiveWire,numFeatures) [$filter GetNumberOfFeatures]

    # loop over features
    for {set f 0} {$f < $Ed(EdLiveWire,numFeatures)} {incr f} {

	# get weight (importance given to feature)
	set Ed(EdLiveWire,feature$f,weight) \
		[$filter GetWeightForFeature $f]	
	
	# get number of params for feature calculation
	set Ed(EdLiveWire,feature$f,numParams) \
		[$filter GetNumberOfParamsForFeature $f]

	# loop over params for each feature
	for {set p 0} {$p < $Ed(EdLiveWire,feature$f,numParams)} {incr p} {
	    set Ed(EdLiveWire,feature$f,param$p) \
		[$filter GetParamForFeature $f $p]
	}
    }
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetFeatureParams
# set vtkImageLiveWireEdgeWeights filters settings from tcl variables
# sets one of the active slice's filters
# .ARGS
# int edgedir edge filter to set parameters in (optional, defaults to active one)
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetFeatureParams {{edgedir ""}} {
    global Ed Slice

    if {$edgedir == ""} {
	set edgedir $Ed(EdLiveWire,activeEdgeDirection)
    }

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

    # Set in vtk-land:

    # only modify filters for the active slice
    set s $Slice(activeID)
    
    # get filter that corresponds to the active edge direction selected in the GUI
    set filt [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $edgedir]
    
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


#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetFeatureParamsForAllFilters
# Set the settings from the GUI into all filters for this slice
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetFeatureParamsForAllFilters {} {
    global Ed
    
    for {set i 0} {$i < $Ed(EdLiveWire,numEdgeFilters)} {incr i} {

	EdLiveWireSetFeatureParams $i
    }
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireAdvancedApply
# Changes filter settings:
# Applies settings to current filter for current slice.
# Called using Apply button in advanced tab.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireAdvancedApply {} {
    global Ed

    EdLiveWireSetFeatureParams

    Slicer ReformatModified
    Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireAdvancedApplyToAll
# Changes filter settings:
# Applies settings to all filters for current slice.
# Called using ApplyToAll button in advanced tab.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireAdvancedApplyToAll {} {
    global Ed

    EdLiveWireSetFeatureParamsForAllFilters

    Slicer ReformatModified
    Slicer Update
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireStartPipeline
# Sets up filters to get input from Slicer (vtkMrmlSlicer) object.
# Updates the pipeline.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireStartPipeline {} {
    global Ed Slice Gui Volume

    #puts "EDLW START pipeline________________"
    
    set Gui(progressText) "Livewire Initialization"	

    # tell the slicer to overlay our filter output on the fore layer
    # (the Working volume)
    Slicer FilterOverlayOn
    
    # set up pipeline
    foreach s $Slice(idList) {
	Slicer SetFirstFilter $s Ed(EdLiveWire,lwSetup$s)
	Slicer SetLastFilter  $s Ed(EdLiveWire,lwPath$s)  

    }

    # Layers: Back=Original, Fore=Working
    # The original volume needs to be the filter input
    Slicer BackFilterOn
    Slicer ForeFilterOff

    # only apply filters to active slice
    Slicer FilterActiveOn

    Slicer ReformatModified
    Slicer Update
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireStopPipeline
# Shut down the pipeline that hooks into the slicer object
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireStopPipeline {} {
    global Ed Slice Gui

    Slicer FilterOverlayOff
    Slicer BackFilterOff
    Slicer ForeFilterOff
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
    global Ed Label Slice Editor

    # we are drawing in the label layer, so it had
    # better be visible
    if {$Editor(display,labelOn) == 0} {
	MainSlicesSetVolumeAll Label [EditorGetWorkingID]
    }

    # all 4 edge filters need to execute
    #set Ed(EdLiveWire,numEdgeFiltersReady) 0
    # ignore mouse clicks until we are all ready
    #set Ed(EdLiveWire,pipelineActive) 0
    # ignore mouse movement until we have a start point
    set Ed(EdLiveWire,pipelineActiveAndContourStarted) 0

    # Lauren reset this slice's contour?

    # set up Slicer pipeline
    EdLiveWireStartPipeline

    # keep track of active slice to reset contour if slice changes
    set Ed(EdLiveWire,activeSlice) $Slice(activeID)

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
    
    # make sure tcl vars correspond to vtk filter settings
    EdLiveWireGetFeatureParams
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
    EdLiveWireStopPipeline

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
proc EdLiveWireB1 {x y} {
    global Ed Slice

    set s $Slice(activeID)

    switch $Ed(EdLiveWire,mode) {
	"LiveWire" {
	    # if pipeline not set up yet do nothing
	    #if {$Ed(EdLiveWire,pipelineActive) == 0} {
	#	return
	#    }
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
	"Training" {
	    Slicer DrawInsertPoint $x $y
	}
    }

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireMotion
# When mouse moves over slice, if pipeline is operational, 
# pass end point to live wire filter and then render the slice.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireMotion {x y} {
    global Ed Slice

    set s $Slice(activeID)

    switch $Ed(EdLiveWire,mode) {
	"LiveWire" {
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
	"Training" {
	    # do nothing
	}

    }
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireB1Motion
# used only in training mode (currently unused)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireB1Motion {x y} {
    global Ed

    switch $Ed(EdLiveWire,mode) {
	"LiveWire" {
	    # do nothing
	}
	"Training" {
	    # paint training drawing on slice
	    Slicer DrawInsertPoint $x $y
	}
    }
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
# Called when label changes.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireLabel {} {
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
	Ed(EdLiveWire,lwPath$s) SetLabel $Label(label)	
    }

    EdLiveWireUpdate

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireClearCurrentSlice
# Clears contour from filters and makes slice redraw
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireClearCurrentSlice {} {
    global Slice
    
    set s $Slice(activeID)
    EdLiveWireResetSlice $s
    Slicer Update
    RenderSlice $s
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireResetSlice
# Clear the previous contour to start over with new start point.
# After, must do Slicer Update  and  RenderSlice $s
# to clear the slice (just call EdLiveWireClearCurrentSlice to do it all)
# .ARGS
# int s number of the slice
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireResetSlice {s} {
    global Ed

    # reset contour points
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
    set rasPoints [Ed(EdLiveWire,lwPath$s) GetContourPixels]
    if {[$rasPoints GetNumberOfPoints] == 0} {
	puts "no points to apply!"
	return
    }
    
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
    # give points to editor object to actually draw them
    Ed(editor)   Draw $label $points $radius $shape
    
    # clear points that livewire object was storing
    EdLiveWireResetSlice $s

    # reset editor object
    Ed(editor)     SetInput ""
    Ed(editor)     UseInputOff

    EdUpdateAfterApplyEffect $v Active

    # always delete points for now...
    Slicer DrawDeleteAll

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
# 
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireEndProgress {} {
    global Ed

    MainEndProgress
    
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireSetMode
# unused.  from when there was a training mode.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireSetMode {} {
    global Ed Slice

    puts $Ed(EdLiveWire,mode)
    
    set s $Slice(activeID)
    set e EdLiveWire

    # Lauren: do this for all slices???????
    switch $Ed($e,mode) {
	"LiveWire" {
	    if {$Ed($e,previousMode) == "Training"} {
		for {set f 0} {$f < $Ed($e,numEdgeFilters)} {incr f} {
		    set filt [Ed($e,lwSetup$s) GetEdgeFilter $f]
		    $filt TrainingModeOff
		}
		# clear any drawn training points
		Slicer DrawDeleteAll		

		# restore drawing settings
		Slicer DrawSetRadius $Ed($e,radius)
		Slicer DrawSetShapeTo$Ed($e,shape)
	    }
	}
	"Training" {
	    if {$Ed(EdLiveWire,previousMode) == "LiveWire"} {
		# clear livewire contour from all slices.  This is so that
		# only the hand-drawn contour is used for training.
		foreach s $Slice(idList) {
		    EdLiveWireResetSlice $s
		    RenderSlice $s
		    Slicer Update
		}

		# training drawing settings
		Slicer DrawSetRadius $Ed($e,trainingRadius)
		Slicer DrawSetShapeToLines
	    }	    
	}
    }
    set Ed($e,previousMode) $Ed($e,mode)
}


################################
# Lauren make a function in the slicer that reformats a slice
# and makes it available!
# this code should not all be in tcl!
###############################


#-------------------------------------------------------------------------------
# .PROC EdLiveWireReformatSlice
# reformats a slice in a volume in the slicer.
# THIS SHOULD BE IN C++ CODE SOMEWHERE (vtkMrmlSlicer.cxx)
# .ARGS
# int offset slice number, essentially
# object reformat reformatter to use
# int volume volume id of the volume (or \"Working\" or \"Original\")
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireReformatSlice {offset realReformat {volume "Working"}} {
    global Ed Slice Volume View Label
    
    # reformat slice of Working or Original volume
    set s $Slice(activeID)
    if {$volume == "Working"} {
	set v [EditorGetWorkingID]
    } elseif {$volume == "Original"} {
	set v [EditorGetOriginalID]
    } else {
	# input is a volume number, not a name
	set v $volume
    }

    set vol Volume($v,vol)
    set node Volume($v,node)

    # proper way to set fov for correct reformatting
    # Lauren the fov should be update for all reformatting in the slicer
    #set dim     [lindex [$node GetDimensions] 0]
    #set spacing [lindex [$node GetSpacing] 0]
    #set fov     [expr $dim*$spacing]

    # use a reformat IJK to compute the reformat matrix
    vtkImageReformatIJK reformat
    vtkMatrix4x4 ref
    reformat SetWldToIjkMatrix [$node GetWldToIjk]
    reformat SetInput [$vol GetOutput]
    reformat SetInputOrderString [$node GetScanOrder]
    puts "scan order from node: [$node GetScanOrder]"
    # output order should match the displayed slices:
    set orient [Slicer GetOrientString $s]
    puts "orient: $orient"

    # know orientation is IJK since we're in the editor:
    set order ""
    switch $orient {
	"OrigSlice" {
	    puts "original slice"
	    set order [$node GetScanOrder]
	}
	"AxiSlice" {
	    set order "IS"
	}
	"SagSlice" {
	    set order "LR"
	}
	"CorSlice" {
	    set order "PA"
	}
    } 
    if {$order == ""} {
	puts "ERROR in EdLiveWire.tcl: scan order not found!"
    }

    puts "order found as: $order"
    reformat SetOutputOrderString $order
    reformat SetSlice $offset
    #reformat ComputeTransform  BUG: this is not wrapped!
    reformat ComputeTransform2
    reformat ComputeOutputExtent
    reformat Update
    reformat ComputeReformatMatrix ref   
    #puts [ref Print]

    # give the matrix to a real reformatter
    #vtkImageReformat realReformat 
    $realReformat SetInput [$vol GetOutput]
    $realReformat SetReformatMatrix ref
    $realReformat SetWldToIjkMatrix [$node GetWldToIjk]
    # no interpolation for Working
    $realReformat InterpolateOff
    # These should match current reformatter settings
    # (in the vtkMrmlSlicer object) so that images will line up. 
    # Lauren if reformatting is not at 256 resolution in future 
    # this will need to be fixed
    $realReformat SetResolution 256
    $realReformat SetFieldOfView [Slicer GetFieldOfView]

    reformat Delete
    ref Delete
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireUseDistanceFromPreviousContour
#  not finished.  This will make a distance map for input to livewire
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireUseDistanceFromPreviousContour {} {
    global Ed Slice
    
    set s $Slice(activeID)

    # image we need is the previous one from the Working
    # volume (the last slice a contour was drawn on)
    # Lauren: may need to add OR subtract 1, depending!
    set offset [expr $Slice($s,offset) - 1]
    puts "offset: $offset"
    
    EdLiveWireGetContourSlice $offset

    # test image is okay
    Ed(EdLiveWire,viewer$s) SetColorWindow 15
    Ed(EdLiveWire,viewer$s) SetColorLevel 5
    Ed(EdLiveWire,viewer$s) SetInput $Ed(EdLiveWire,contourSlice)

}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireGetContourSlice
# Grabs a slice in working volume, gets only the desired label
# .ARGS
# int offset slice offset
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireGetContourSlice {offset} {
    global Ed Slice Volume View Label

    set s $Slice(activeID)  
    
    # first reformat the appropriate slice
    vtkImageReformat realReformat
    EdLiveWireReformatSlice $offset realReformat
    
    # 1. remove other labels from image.
    set lab 0
    if {$Label(label) == ""} {
	puts "ERROR in EdLiveWire, no label exists"
    } else {
	set lab $Label(label)
    }    
    puts " LABEL: $Label(label)"

    #vtkImageThresholdBeyond thresh
    Ed(EdLiveWire,imageThreshold) SetReplaceIn 1
    Ed(EdLiveWire,imageThreshold) SetReplaceOut 1
    # output 1's for the label we want
    Ed(EdLiveWire,imageThreshold) SetInValue 1
    Ed(EdLiveWire,imageThreshold) SetOutValue 0
    Ed(EdLiveWire,imageThreshold) SetInput [realReformat GetOutput]
    Ed(EdLiveWire,imageThreshold) ThresholdBetween $lab $lab
    
    # result:
    set Ed(EdLiveWire,contourSlice) [Ed(EdLiveWire,imageThreshold) GetOutput]

    realReformat Delete

    return

    # Lauren get rid of the following, probably:

    # pipeline to get just the boundary of the segmented area:
    #2. get actual border btwn structure and non
    vtkImageErode erode
    erode SetBackground 0
    erode SetForeground 1
    erode SetInput [thresh GetOutput]  
    # subtract eroded from regular for 1-pix border
    #vtkImageMathematics Ed(EdLiveWire,imageMath)
    Ed(EdLiveWire,imageMath) SetInput1 [thresh GetOutput]
    Ed(EdLiveWire,imageMath) SetInput2 [erode GetOutput]
    Ed(EdLiveWire,imageMath) SetOperationToSubtract

    set Ed(EdLiveWire,contourSlice) [Ed(EdLiveWire,imageMath) GetOutput]    

    # test
    #Ed(EdLiveWire,viewer$s) SetInput [realReformat GetOutput]    
    #Ed(EdLiveWire,viewer$s) SetInput [border GetOutput]    

    # make a distance map!!!!!!!!!

    # pass reformatted slice on to the edge weight filter
    
    #puts "output: [ [realReformat GetOutput] Print]"
    realReformat Delete
    thresh Delete
    erode Delete
    #border Delete
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireTrain
# makes edge filters train on slice(s) with contours in them
# writes settings to a file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireTrain {} {
    global Ed Slice Volume

    #if {$Ed(EdLiveWire,mode) != "Training"} {
	#puts "Can't train if not in training mode"
	#return
    #}

    set s $Slice(activeID)
    set e EdLiveWire

    # set the training image input to the edge filter.
    for {set f 0} {$f < $Ed($e,numEdgeFilters)} {incr f} {

	set filt [Ed($e,lwSetup$s) GetEdgeFilter $f]
	
	# see if the edge filter already has its other inputs
	if {[$filt GetNumberOfInputs] < 1} {
	    tk_messageBox -message \
		    "Please try training again after LiveWire initialization has finished."
	    return
	}
	
	puts "edge direction: [$filt GetEdgeDirection]"

	# tell filter to do training calculations
	$filt TrainingModeOn		

	puts "mode: $Ed(EdLiveWire,trainingMode)"

	switch $Ed(EdLiveWire,trainingMode) {
	    "DrawOnSlice" {
		# draw points on a blank slice for input to filter
		vtkImageFillROI fillroi
		fillroi SetInput [Volume($Volume(idNone),vol) GetOutput]
		fillroi SetValue 1
		fillroi SetRadius $Ed($e,trainingRadius)
		
		fillroi SetShapeString Lines
		fillroi SetPoints [Slicer DrawGetPoints]
		
		$filt SetTrainingPointsImage [fillroi GetOutput]
		
		# Lauren for now set the 2nd input to this too!
		$filt SetPreviousContourImage [fillroi GetOutput]
		
		fillroi Delete
		
		$filt Update

	    }	    
	    "CurrentSlice" {
		# reformat current slice in working volume
		EdLiveWireGetContourSlice $Slice($s,offset)
		
		# test image is okay
		#Ed(EdLiveWire,viewer$s) SetColorWindow 8
		#Ed(EdLiveWire,viewer$s) SetColorLevel 3
		#Ed(EdLiveWire,viewer$s) SetInput $Ed(EdLiveWire,contourSlice)

		$filt SetTrainingPointsImage $Ed(EdLiveWire,contourSlice)
		
		# Lauren for now set the 2nd input to this too!
		$filt SetPreviousContourImage $Ed(EdLiveWire,contourSlice)
		$filt Update		
	    }
	    "RangeOfSlices" {
		# validate user input
		if {[ValidateInt $Ed(EdLiveWire,trainingSlice1)] == 0} {
		    tk_messageBox  -message \
			    "The slice number must be an integer."
		    return
		}	
		if {[ValidateInt $Ed(EdLiveWire,trainingSlice2)] == 0} {
		    tk_messageBox  -message \
			    "The slice number must be an integer."
		    return
		}	

		# tell filter to compute across many slices
		$filt TrainingComputeRunningTotalsOn

		vtkImageReformat origReformat
		# reformat each slice since we want to train in the 
		# same orientation as drawing will be done
		for {set i $Ed(EdLiveWire,trainingSlice1)} \
			{$i <= $Ed(EdLiveWire,trainingSlice2)} {incr i} {
		    puts "processing slice number $i"

		    # reformat slice in working volume
		    EdLiveWireGetContourSlice $i
		    $filt SetTrainingPointsImage $Ed(EdLiveWire,contourSlice)

		    # reformat the same slice in original volume
		    EdLiveWireReformatSlice $i origReformat "Original"
		    $filt SetOriginalImage [origReformat GetOutput]    

		    # test image is okay
		    #Ed(EdLiveWire,viewer$s) SetColorWindow 8
		    #Ed(EdLiveWire,viewer$s) SetColorLevel 3
		    #Ed(EdLiveWire,viewer$s) SetInput $Ed(EdLiveWire,contourSlice)
	    
		    # Lauren for now set the 2nd input to this too!
		    $filt SetPreviousContourImage $Ed(EdLiveWire,contourSlice)
		    
		    # if last slice, stop running total
		    if {$i == $Ed(EdLiveWire,trainingSlice2)} {
			$filt TrainingComputeRunningTotalsOff
		    }

		    $filt Update
		}
		origReformat Delete
	    }
	    
	}
	# turn computation of training data off for every filter.
	$filt TrainingModeOff

    }

    Slicer ReformatModified
    Slicer Update

    # save settings to the file.
    EdLiveWireWriteFeatureParams
    
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireReadFeatureParams
# Reads the options-format file that contains the livewire
# settings, and applies the settings to the filters of the 
# current slice.  NOTE: this does not apply to all slices
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireReadFeatureParams {} {
    global Ed Slice

    set e EdLiveWire

    if {$Ed($e,trainingInputFileName) == ""} {
	return
    }
    
    # read in the settings
    set tags [MainMrmlReadVersion2.0 $Ed($e,trainingInputFileName)]
    set node(program) ""
    set node(contents) ""

    foreach pair $tags {
	set tag  [lindex $pair 0]
	set attr [lreplace $pair 0 0]
	
	switch $tag {
	    "Options" {
		foreach a $attr {
		    set key [lindex $a 0]
		    set val [lreplace $a 0 0]
		    set node($key) $val
		}
	    }    
	}
    }
    
    # check program and contents
    if {$node(program) != "slicer"} {
	puts "This is not a LiveWire settings file. It is from $program"
    }
    if {$node(contents) != $Ed(EdLiveWire,contents)} {
	puts "This is not a LiveWire settings file. It is $contents."
    }

    set filters [array names node "filter*"]
    foreach filter $filters {
	# find filter number
	set number ""
	regexp "filter(.*)" $filter match number
	
	# make a list of settings for the filter
	set settings [split $node($filter) ","]
	
	# see how many features, how many parameters for each feature:
	set numFeatures [llength $settings]

	# subtract one since the weight is the first number and not a "param"
	set numParams [expr [llength [lindex $settings 0]] - 1]

	# check to make sure we don't have too many features in the file
	if {$numFeatures > $Ed(EdLiveWire,numFeatures)} {
	    set numFeatures $Ed(EdLiveWire,numFeatures)
	}
	# assume all features have same number of params: does the
	# numParams match the number we expect for the first feature?
	if {$numParams > $Ed(EdLiveWire,feature0,numParams)} {
	    set numParams $Ed(EdLiveWire,feature0,numParams)
	}

	# get this filter (the right number)
	set s $Slice(activeID)
	set filter [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $number]

	# use the settings: loop over features
	for {set feat 0} {$feat < $numFeatures} {incr feat} {
	    # get this feature's info from the list
	    set params [lindex $settings $feat]

	    # set weight (importance given to feature)	
	    set Ed(EdLiveWire,feature$feat,weight) [lindex $params 0]

	    set index 1
	    # loop over params for the feature
	    for {set p 0} {$p < $numParams} {incr p} {
		set Ed($e,feature$feat,param$p) [lindex $params $index]
		set index [expr $index + 1]
	    }
	}

	# Apply the settings to this filter
	EdLiveWireSetFeatureParams $number
    }


    puts "Read settings from file $Ed($e,trainingInputFileName)"
}


#-------------------------------------------------------------------------------
# .PROC EdLiveWireWriteFeatureParams
# writes settings to a file (Options format)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireWriteFeatureParams {} {
    global Ed Slice Options

    if {$Ed(EdLiveWire,trainingOutputFileName) == ""} {
	return
    }

    # these lines wrote the plain numbers to a file
    #Ed(EdLiveWire,lwSetup$s) SetSettingsFileName $Ed($e,trainingOutputFileName)
    #Ed(EdLiveWire,lwSetup$s) WriteFilterSettings

    # use the Options format instead:
    vtkMrmlOptionsNode node
    node SetProgram $Options(program)
    node SetContents $Ed(EdLiveWire,contents)

    set s $Slice(activeID)
    set settings ""

    # loop over edge filters  
    for {set e 0} {$e < $Ed(EdLiveWire,numEdgeFilters)} {incr e} {
	set filter [Ed(EdLiveWire,lwSetup$s) GetEdgeFilter $e]
	set settings "${settings}filter$e=\n'"

	# loop over features
	for {set f 0} {$f < $Ed(EdLiveWire,numFeatures)} {incr f} {

	    # get weight (importance given to feature)
	    set settings "$settings[$filter GetWeightForFeature $f]"
	    
	    # loop over params for each feature
	    for {set p 0} {$p < $Ed(EdLiveWire,feature$f,numParams)} {incr p} {
		set settings "$settings [$filter GetParamForFeature $f $p]"
	    }

	    set settings "$settings,\n"
	}
	# remove newline and last comma
	set settings [string trimright $settings]
	set settings [string trimright $settings ,]
	set settings "$settings'\n"

	#puts "e: $e"
	#puts $settings
    } 

    # fill in the options info
    node SetOptions $settings

    # temp tree for writing
    vtkMrmlTree tempTree
    tempTree AddItem node

    # tell the tree to write
    tempTree Write $Ed(EdLiveWire,trainingOutputFileName)
    tempTree RemoveAllItems

    tempTree Delete
    node Delete

    puts "Saved settings to file $Ed(EdLiveWire,trainingOutputFileName)"
}

#-------------------------------------------------------------------------------
# .PROC EdLiveWireTestCF
# development
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdLiveWireTestCF {} {
    global Ed Volume Slice

    set phaseVol ""
    set certVol ""

    # figure out which volumes to use: hack for now
    foreach v $Volume(idList) {
	set n Volume($v,node)
	set name [$n GetName]
	puts $name
	if {$name == "phase"} {
	    set phaseVol $v
	}
	if {$name == "cert"} {
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
    EdLiveWireReformatSlice $Slice($s,offset) phaseReformat $phaseVol
    EdLiveWireReformatSlice $Slice($s,offset) certReformat $certVol

    # give to (all) lw edge filters as input
    set e EdLiveWire
    for {set f 0} {$f < $Ed($e,numEdgeFilters)} {incr f} {
	
	set filt [Ed($e,lwSetup$s) GetEdgeFilter $f]
	
	# see if the edge filter already has its other inputs
	if {[$filt GetNumberOfInputs] < 1} {
	    tk_messageBox -message \
		    "Please test again after LiveWire initialization."
	    return
	}

	$filt SetPhaseImage [phaseReformat GetOutput]     
	$filt SetCertaintyImage [certReformat GetOutput]     

	# Lauren for now set the other inputs to this too!
	$filt SetPreviousContourImage [phaseReformat GetOutput]
	$filt SetTrainingPointsImage [phaseReformat GetOutput]
    }

    # clean up
    phaseReformat Delete
    certReformat Delete

    puts "almost done!"

    # update slicer
    Slicer Update
    
    puts "woo hoo!!!"
}