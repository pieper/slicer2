#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use and copy (but not distribute) this
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
# FILE:        EdThreshold.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   EdThresholdInit
#   EdThresholdBuildVTK
#   EdThresholdBuildGUI
#   EdThresholdEnter
#   EdThresholdExit
#   EdThresholdUpdateSliderRange
#   EdThresholdUpdateInteractive
#   EdThresholdSetInput
#   EdThresholdSetInteract
#   EdThresholdUpdate
#   EdThresholdRenderInteractive
#   EdThresholdLabel
#   EdThresholdApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdThresholdInit
# .END
#-------------------------------------------------------------------------------
proc EdThresholdInit {} {
	global Ed Gui

	set e EdThreshold
	set Ed($e,name)      "Threshold"
	set Ed($e,desc)      "Label pixels that satisfy a threshold."
	set Ed($e,rank)      1
	set Ed($e,procGUI)   EdThresholdBuildGUI
	set Ed($e,procVTK)   EdThresholdBuildVTK
	set Ed($e,procEnter) EdThresholdEnter
	set Ed($e,procExit)  EdThresholdExit

	# Required
	set Ed($e,scope)  3D 
	set Ed($e,input)  Original

	# Windows98 Version II can't render histograms
	set Ed($e,histogram) On
	if {$Gui(pc) == 1} {
		set Ed($e,histogram) Off
	}

	set Ed($e,interact) Active

	set Ed($e,lower) 50
	set Ed($e,upper) 150
	set Ed($e,replaceIn) 1
	set Ed($e,replaceOut) 1
	set Ed($e,bg) 0
	set Ed($e,rangeLow) 0
	set Ed($e,rangeHigh) 500
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdBuildVTK
# .END
#-------------------------------------------------------------------------------
proc EdThresholdBuildVTK {} {
	global Ed Label Slice

	foreach s $Slice(idList) {
		vtkImageThresholdBeyond Ed(EdThreshold,thresh$s)
		Ed(EdThreshold,thresh$s) SetReplaceIn  $Ed(EdThreshold,replaceIn)
		Ed(EdThreshold,thresh$s) SetReplaceOut $Ed(EdThreshold,replaceOut)
		Ed(EdThreshold,thresh$s) SetInValue    0
		Ed(EdThreshold,thresh$s) SetOutValue   $Ed(EdThreshold,bg)
	}
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EdThresholdBuildGUI {} {
	global Ed Gui Label Volume

	#-------------------------------------------
	# Threshold frame
	#-------------------------------------------
	set f $Ed(EdThreshold,frame)

	frame $f.fInput     -bg $Gui(activeWorkspace)
	frame $f.fScope     -bg $Gui(activeWorkspace)
	frame $f.fInteract  -bg $Gui(activeWorkspace)
	frame $f.fHistogram -bg $Gui(activeWorkspace)
	frame $f.fGrid      -bg $Gui(activeWorkspace)
	frame $f.fSliders   -bg $Gui(activeWorkspace)
	frame $f.fApply     -bg $Gui(activeWorkspace)
	pack \
		$f.fInput $f.fScope $f.fInteract \
		$f.fGrid $f.fSliders $f.fHistogram $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildScopeGUI $Ed(EdThreshold,frame).fScope Ed(EdThreshold,scope) Multi

	EdBuildInputGUI $Ed(EdThreshold,frame).fInput Ed(EdThreshold,input) \
		"-command EdThresholdSetInput"

	EdBuildInteractGUI $Ed(EdThreshold,frame).fInteract Ed(EdThreshold,interact) \
		"-command EdThresholdSetInteract"

	#-------------------------------------------
	# Threshold->Histogram frame
	#-------------------------------------------
	set f $Ed(EdThreshold,frame).fHistogram

	if {$Ed(EdThreshold,histogram) == "On"} {
		set c {label $f.l -text "Histogram:" $Gui(WLA)}; eval [subst $c]
		frame $f.fHistBorder -bg $Gui(activeWorkspace) -relief sunken -bd 2
		pack $f.l $f.fHistBorder -side left -padx $Gui(pad) -pady $Gui(pad)

		#-------------------------------------------
		# Threshold->Histogram->HistBorder frame
		#-------------------------------------------
		set f $Ed(EdThreshold,frame).fHistogram.fHistBorder

		MakeVTKImageWindow editThreshHist
		editThreshHistMapper SetInput [Volume(0,vol) GetHistogramPlot]

		vtkTkImageWindowWidget $f.fHist -iw editThreshHistWin \
			-width $Volume(histWidth) -height $Volume(histHeight)  
		bind $f.fHist <Expose> {ExposeTkImageViewer %W %x %y %w %h}
		pack $f.fHist
	}

	#-------------------------------------------
	# Threshold->Grid frame
	#-------------------------------------------
	set f $Ed(EdThreshold,frame).fGrid

	# Output label
	set c {button $f.bOutput -text "Output:" \
		-command "ShowLabels EdThresholdLabel" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eOutput -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eOutput <Return>   "EdThresholdLabel"
	bind $f.eOutput <FocusOut> "EdThresholdLabel"
	set c {entry $f.eName -width 14 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]
	grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
	grid $f.eOutput $f.eName -sticky w

	lappend Label(colorWidgetList) $f.eName

	# Whether to Replace the output
#	set c {checkbutton $f.cReplaceOutput \
#		-text "Replace Output" -width 14 -variable Ed(EdThreshold,replaceIn) \
#		-indicatoron 0 $Gui(WCA) -command "EdThresholdUpdate; RenderAll"}
#		eval [subst $c]
#	grid $f.cReplaceOutput -columnspan 2 -pady $Gui(pad) -sticky e

	#-------------------------------------------
	# Threshold->Sliders frame
	#-------------------------------------------
	set f $Ed(EdThreshold,frame).fSliders

	foreach slider "Lower Upper" text "Lo Hi" {
		set c {label $f.l${slider} -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e${slider} -width 6 \
			-textvariable Ed(EdThreshold,[Uncap $slider]) $Gui(WEA)}; 
			eval [subst $c]
		bind $f.e${slider} <Return>   "EdThresholdUpdate; RenderActive;"
		bind $f.e${slider} <FocusOut> "EdThresholdUpdate; RenderActive;"
		set c {scale $f.s${slider} -from $Ed(EdThreshold,rangeLow) -to $Ed(EdThreshold,rangeHigh)\
			-length 140 -variable Ed(EdThreshold,[Uncap $slider])  -resolution 1 \
			-command "EdThresholdUpdate; RenderActive"\
			$Gui(WSA) -sliderlength 14 }; eval [subst $c]
		grid $f.l${slider} $f.e${slider} $f.s${slider} -padx 2 -pady $Gui(pad) \
			-sticky news

		set Ed(EdThreshold,slider$slider) $f.s$slider
	}

	#-------------------------------------------
	# Threshold->Apply frame
	#-------------------------------------------
	set f $Ed(EdThreshold,frame).fApply

	set c {button $f.bApply -text "Apply" \
		-command "EdThresholdApply" $Gui(WBA) -width 8}
		eval [subst $c]
	pack $f.bApply

}

#-------------------------------------------------------------------------------
# .PROC EdThresholdEnter
# .END
#-------------------------------------------------------------------------------
proc EdThresholdEnter {} {
	global Ed Label

	# Make sure we're colored
	LabelsColorWidgets

	EdThresholdUpdateSliderRange
	EdThresholdUpdateInteractive
	EdThresholdUpdate
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdExit
# .END
#-------------------------------------------------------------------------------
proc EdThresholdExit {} {
	global Ed

	Slicer BackFilterOff
	Slicer ForeFilterOff
	Slicer ReformatModified
	Slicer Update
	EdThresholdRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdUpdateSliderRange
# .END
#-------------------------------------------------------------------------------
proc EdThresholdUpdateSliderRange {} {
	global Volume Ed

	set v [EditorGetInputID $Ed(EdThreshold,input)]

	set lo [Volume($v,vol) GetRangeLow]
	set hi [Volume($v,vol) GetRangeHigh]
	set th [Volume($v,vol) GetBimodalThreshold]

	$Ed(EdThreshold,sliderLower) config -from $lo -to $hi
	$Ed(EdThreshold,sliderUpper) config -from $lo -to $hi

	# Auto EdThresholdold
	set Ed(EdThreshold,lower) $th
	set Ed(EdThreshold,upper) $hi

	# Refresh Histogram
	if {$Ed(EdThreshold,histogram) == "On"} {
		editThreshHistMapper SetInput [Volume($v,vol) GetHistogramPlot]
		editThreshHistWin Render
	}
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdUpdateInteractive
# .END
#-------------------------------------------------------------------------------
proc EdThresholdUpdateInteractive {} {
	global Ed Slice
	
	foreach s $Slice(idList) {
		Slicer SetFirstFilter $s Ed(EdThreshold,thresh$s)
		Slicer SetLastFilter  $s Ed(EdThreshold,thresh$s)
	}

	# Layers: Back=Original, Fore=Working
	if {$Ed(EdThreshold,input) == "Original"} {
		Slicer BackFilterOn
		Slicer ForeFilterOff
	} else {
		Slicer BackFilterOff
		Slicer ForeFilterOn
	}

	# Just active slice?
	if {$Ed(EdThreshold,interact) == "Active"} {
		Slicer FilterActiveOn
	} else {
		Slicer FilterActiveOff
	}

	Slicer ReformatModified
	Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdSetInput
# .END
#-------------------------------------------------------------------------------
proc EdThresholdSetInput {} {
	global Ed Label

	EdThresholdUpdateSliderRange
	EdThresholdUpdateInteractive
	EdThresholdUpdate
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdSetInteract
# .END
#-------------------------------------------------------------------------------
proc EdThresholdSetInteract {} {
	global Ed Label

	EdThresholdUpdateInteractive
	EdThresholdUpdate
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdUpdate
# .END
#-------------------------------------------------------------------------------
proc EdThresholdUpdate {} {
	global Ed Label Slice

	# Validate input
	if {[ValidateInt $Ed(EdThreshold,lower)] == 0} {
		tk_messageBox -message "Lo threshold is not an integer."
		return
	}
	if {[ValidateInt $Ed(EdThreshold,upper)] == 0} {
		tk_messageBox -message "Hi threshold is not an integer."
		return
	}
	if {$Label(label) == ""} {
		return
	}

	foreach s $Slice(idList) {
		Ed(EdThreshold,thresh$s) SetReplaceIn     $Ed(EdThreshold,replaceIn)
		Ed(EdThreshold,thresh$s) SetReplaceOut    $Ed(EdThreshold,replaceOut)
		Ed(EdThreshold,thresh$s) SetInValue       $Label(label)
		Ed(EdThreshold,thresh$s) SetOutValue      $Ed(EdThreshold,bg)
		Ed(EdThreshold,thresh$s) ThresholdBetween $Ed(EdThreshold,lower) $Ed(EdThreshold,upper)
	}
	EdThresholdRenderInteractive
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdRenderInteractive
# .END
#-------------------------------------------------------------------------------
proc EdThresholdRenderInteractive {} {
	global Ed

	Render$Ed(EdThreshold,interact)
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdLabel
# .END
#-------------------------------------------------------------------------------
proc EdThresholdLabel {} {
	global Ed

	LabelsFindLabel
	EdThresholdUpdate
}

#-------------------------------------------------------------------------------
# .PROC EdThresholdApply
# .END
#-------------------------------------------------------------------------------
proc EdThresholdApply {} {
	global Ed Volume Label Gui

	set e EdThreshold
	set v [EditorGetInputID $Ed($e,input)]

	# Validate input
	if {[ValidateInt $Ed($e,lower)] == 0} {
		tk_messageBox -message "Lo threshold is not an integer."
		return
	}
	if {[ValidateInt $Ed($e,upper)] == 0} {
		tk_messageBox -message "Hi threshold is not an integer."
		return
	}
	if {[ValidateInt $Label(label)] == 0} {
		tk_messageBox -message "Output label is not an integer."
		return
	}

	EdSetupBeforeApplyEffect $Ed($e,scope) $v

	set Gui(progressText) "Threshold [Volume($v,node) GetName]"	

	set min        $Ed($e,lower)
	set max        $Ed($e,upper)
	set in         $Label(label)
	set out        $Ed($e,bg)
	set replaceIn  $Ed($e,replaceIn)
	set replaceOut $Ed($e,replaceOut)
	Ed(editor)     Threshold $min $max $in $out $replaceIn $replaceOut
	Ed(editor)     SetInput ""
	Ed(editor)     UseInputOff

	EdUpdateAfterApplyEffect $v

	# Reset sliders if the input was working, because that means
	# it changed.
	if {$v == [EditorGetWorkingID]} {
		EdThresholdSetInput
	}
}

