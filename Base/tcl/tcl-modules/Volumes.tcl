#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, and distribute this
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
# FILE:        Volumes.tcl
# DATE:        12/09/1999 14:15
# LAST EDITOR: odonnell
# PROCEDURES:  
#   VolumesInit
#   VolumesBuildGUI
#   VolumesSetPropertyType
#   VolumesPropsApply
#   VolumesPropsCancel
#   VolumesSetFirst
#   VolumesSetOrder
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC VolumesInit
# .END
#-------------------------------------------------------------------------------
proc VolumesInit {} {
	global Volume Module Gui Path

	# Define Tabs
	set m Volumes
	set Module($m,row1List) "Help Display Props Other"
	set Module($m,row1Name) "{Help} {Display} {Props} {Other}"
	set Module($m,row1,tab) Display

	# Define Procedures
	set Module($m,procGUI)  VolumesBuildGUI

	# Windows98 Version II can't render histograms
	set Volume(histogram) On
	if {$Gui(pc) == 1} {
		set Volume(histogram) Off
	}

	# Props
	set Volume(propertyType) Basic
	set Volume(desc) ""
	set Volume(first) $Path(prefixOpenVolume)
	set Volume(orderMenu) "{Sagittal:LR} {Sagittal:RL} {Axial:SI}\
		{Axial:IS} {Coronal:AP} {Coronal:PA}"
	set Volume(orderList) "LR RL SI IS AP PA OB" 
	set Volume(name) [file root [file tail $Volume(first)]]
	set Volume(littleEndian) 0
	set Volume(firstPattern) %s.%03d
	set Volume(littleEndian) 0
	set Volume(resolution) 256
	set Volume(pixelSize) 0.9375
	set Volume(sliceThickness) 1.5
	set Volume(sliceSpacing) 0.0
	set Volume(gantryDetectorTilt) 0
	set Volume(order) LR
	set Volume(desc) ""
	set Volume(readHeaders) 1
	set Volume(labelMap) 0
	# Transform
	set Volume(transformName) ""
	set Volume(transformMatrix) "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"
	set Volume(transformTranslateX) 0
	set Volume(transformTranslateY) 0
	set Volume(transformTranslateZ) 0
	set Volume(transformRotateX) 0
	set Volume(transformRotateY) 0
	set Volume(transformRotateZ) 0
	set Volume(transformDesc) ""
}

#-------------------------------------------------------------------------------
# .PROC VolumesBuildGUI
# .END
#-------------------------------------------------------------------------------
proc VolumesBuildGUI {} {
	global Gui Slice Volume Lut Module

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Display
	# Props
	#   Top
	#     Active
	#     Type
	#   Bot
	#     Basic
	#     Header
	# Other
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Ron, the interpolation button won't work without downloading dll's again.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Volumes $help
	MainHelpBuildGUI  Volumes

	#-------------------------------------------
	# Display frame
	#-------------------------------------------
	set fDisplay $Module(Volumes,fDisplay)
	set f $fDisplay

	# Frames
	frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fWinLvl    -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fThresh    -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fHistogram -bg $Gui(activeWorkspace)
	frame $f.fInterpolate -bg $Gui(activeWorkspace)
	pack $f.fActive $f.fWinLvl $f.fThresh $f.fHistogram $f.fInterpolate \
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Display->Active frame
	#-------------------------------------------
	set f $fDisplay.fActive

	set c {label $f.lActive -text "Active Volume: " $Gui(BLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbActive
	lappend Volume(mActiveList)  $f.mbActive.m


	#-------------------------------------------
	# Display->WinLvl frame
	#-------------------------------------------
	set f $fDisplay.fWinLvl

	frame $f.fAuto    -bg $Gui(activeWorkspace)
	frame $f.fSliders -bg $Gui(activeWorkspace)
	pack $f.fAuto $f.fSliders -side top -fill x -expand 1

	#-------------------------------------------
	# Display->WinLvl->Auto frame
	#-------------------------------------------
	set f $fDisplay.fWinLvl.fAuto

	set c {label $f.lAuto -text "Window/Level:" $Gui(WLA)}; eval [subst $c]
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -padx $Gui(pad)  -pady $Gui(pad) -fill x

	foreach value "1 0" text "Auto Manual" width "5 7" {
		set c {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoWindowLevel) \
			-command "MainVolumesSetParam AutoWindowLevel; MainVolumesRender" \
			$Gui(WCA)}; eval [subst $c]
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Display->WinLvl->Sliders frame
	#-------------------------------------------
	set f $fDisplay.fWinLvl.fSliders

	foreach slider "Window Level" text "Win Lev" {
		set c {label $f.l${slider} -text "$text:" $Gui(WLA)}
			eval [subst $c]
		set c {entry $f.e${slider} -width 6 \
			-textvariable Volume([Uncap ${slider}]) $Gui(WEA)}; eval [subst $c]
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		set c {scale $f.s${slider} -from 1 -to 700 -length 140\
			-variable Volume([Uncap ${slider}])  -resolution 1 \
			-command "MainVolumesSetParam ${slider}; MainVolumesRenderActive"\
			 $Gui(WSA) -sliderlength 14}; eval [subst $c]
		bind $f.s${slider} <Leave> "MainVolumesRender"
		grid $f.l${slider} $f.e${slider} $f.s${slider} -padx 2 -pady $Gui(pad) \
			-sticky news
	}
	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	lappend Volume(sWindowList) $f.sWindow
	lappend Volume(sLevelList)  $f.sLevel

	
	#-------------------------------------------
	# Display->Thresh frame
	#-------------------------------------------
	set f $fDisplay.fThresh

	frame $f.fAuto    -bg $Gui(activeWorkspace)
	frame $f.fSliders -bg $Gui(activeWorkspace)
	pack $f.fAuto $f.fSliders -side top  -fill x -expand 1

	#-------------------------------------------
	# Display->Thresh->Auto frame
	#-------------------------------------------
	set f $fDisplay.fThresh.fAuto

	set c {label $f.lAuto -text "Threshold: " $Gui(WLA)}; eval [subst $c]
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -fill x

	foreach value "1 0 -1" text "Auto Manual None" width "5 7 5" {
		set c {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoThreshold) \
			-command "MainVolumesSetParam AutoThreshold; MainVolumesRender" \
			$Gui(WCA)}; eval [subst $c]
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Display->Thresh->Sliders frame
	#-------------------------------------------
	set f $fDisplay.fThresh.fSliders

	foreach slider "Lower Upper" text "Lo Hi" {
		set c {label $f.l${slider} -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e${slider} -width 6 \
			-textvariable Volume([Uncap ${slider}]Threshold) $Gui(WEA)}; 
			eval [subst $c]
			bind $f.e${slider} <Return>   \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
			bind $f.e${slider} <FocusOut> \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
		set c {scale $f.s${slider} -from 1 -to 700 -length 140 \
			-variable Volume([Uncap ${slider}]Threshold)  -resolution 1 \
			-command "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"\
			$Gui(WSA) -sliderlength 14 }; eval [subst $c]
		grid $f.l${slider} $f.e${slider} $f.s${slider} -padx 2 -pady $Gui(pad) \
			-sticky news
	}
	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	lappend Volume(sLevelList) $f.sLower
	lappend Volume(sLevelList) $f.sUpper


	#-------------------------------------------
	# Display->Histogram frame
	#-------------------------------------------
	set f $fDisplay.fHistogram

	frame $f.fHistBorder -bg $Gui(activeWorkspace) -relief sunken -bd 2
	frame $f.fLut -bg $Gui(activeWorkspace)
	pack $f.fLut $f.fHistBorder -side left -padx $Gui(pad) -pady $Gui(pad)
	
	#-------------------------------------------
	# Display->Histogram->Lut frame
	#-------------------------------------------
	set f $fDisplay.fHistogram.fLut

	set c {label $f.lLUT -text "Palette:" $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mbLUT \
		-text "$Lut([lindex $Lut(idList) 0],name)" \
			-relief raised -bd 2 -width 9 \
		-menu $f.mbLUT.menu $Gui(WMBA)}; eval [subst $c]
		set c {menu $f.mbLUT.menu $Gui(WMA)}; eval [subst $c]
		# Add menu items
		foreach l $Lut(idList) {
			$f.mbLUT.menu add command -label $Lut($l,name) \
				-command "MainVolumesSetParam LutID $l; MainVolumesRender"
		}
		set Volume(mbLUT) $f.mbLUT
	pack $f.lLUT $f.mbLUT -pady $Gui(pad) -side top

	#-------------------------------------------
	# Display->Histogram->HistBorder frame
	#-------------------------------------------
	set f $fDisplay.fHistogram.fHistBorder

	if {$Volume(histogram) == "On"} {
		MakeVTKImageWindow hist
		histMapper SetInput [Volume(0,vol) GetHistogramPlot]

		vtkTkImageWindowWidget $f.fHist -iw histWin \
			-width $Volume(histWidth) -height $Volume(histHeight)  
		bind $f.fHist <Expose> {ExposeTkImageViewer %W %x %y %w %h}
		pack $f.fHist
	}

	#-------------------------------------------
	# Display->Interpolate frame
	#-------------------------------------------
	set f $fDisplay.fInterpolate

	set c {label $f.lInterpolate -text "Interpolation:" $Gui(WLA)}
		eval [subst $c]
	pack $f.lInterpolate -pady $Gui(pad) -padx $Gui(pad) -side left -fill x

	foreach value "1 0" text "On Off" width "4 4" {
		set c {radiobutton $f.rInterp$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(interpolate) \
			-command "MainVolumesSetParam Interpolate; MainVolumesRender" \
			$Gui(WCA)}
			eval [subst $c]
		pack $f.rInterp$value -side left -fill x
	}


	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(Volumes,fProps)
	set f $fProps

	frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fBot -bg $Gui(activeWorkspace) -height 300
	pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Props->Bot frame
	#-------------------------------------------
	set f $fProps.fBot

	foreach type "Basic Header" {
		frame $f.f${type} -bg $Gui(activeWorkspace)
		place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
		set Volume(f${type}) $f.f${type}
	}
	raise $Volume(fBasic)

	#-------------------------------------------
	# Props->Top frame
	#-------------------------------------------
	set f $fProps.fTop

	frame $f.fActive -bg $Gui(backdrop)
	frame $f.fType   -bg $Gui(backdrop)
	pack $f.fActive $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Props->Top->Active frame
	#-------------------------------------------
	set f $fProps.fTop.fActive

	eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbActive
	lappend Volume(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Props->Top->Type frame
	#-------------------------------------------
	set f $fProps.fTop.fType

	eval {label $f.l -text "Properties:"} $Gui(BLA)
	frame $f.f -bg $Gui(backdrop)
	foreach p "Basic Header" {
		eval {radiobutton $f.f.r$p \
			-text "$p" -command "VolumesSetPropertyType" \
			-variable Volume(propertyType) -value $p -width 8 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$p -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Props->Bot->Basic frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic

	frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fVolume $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Basic->fVolume frame
	#-------------------------------------------

	set f $fProps.fBot.fBasic.fVolume
	frame $f.fInstr    -bg $Gui(activeWorkspace)
	frame $f.fFirst    -bg $Gui(activeWorkspace)
	frame $f.fLast     -bg $Gui(activeWorkspace)
	frame $f.fHeaders  -bg $Gui(activeWorkspace)
	frame $f.fLabelMap -bg $Gui(activeWorkspace)
	frame $f.fOptions  -bg $Gui(activeWorkspace)
	frame $f.fDesc     -bg $Gui(activeWorkspace)

	pack $f.fInstr $f.fFirst $f.fLast $f.fHeaders $f.fLabelMap $f.fOptions \
		$f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	# Instr
	set f $fProps.fBot.fBasic.fVolume.fInstr

	set c {label $f.l -text "First Image File:" \
		$Gui(WLA)}; eval [subst $c]
	set c {button $f.bFind -text "Browse..." -width 9 \
		-command "VolumesSetFirst" $Gui(WBA)}; eval [subst $c]
	pack $f.l $f.bFind -side left -padx $Gui(pad) 

	# First
	set f $fProps.fBot.fBasic.fVolume.fFirst

	set c {entry $f.eFirst -textvariable Volume(first) \
		$Gui(WEA)}; eval [subst $c]
		bind $f.eFirst <Return> "VolumesSetFirst"
		bind $f.eFirst <Tab> "VolumesSetLast"
	pack $f.eFirst -side left -padx $Gui(pad) -expand 1 -fill x

	# Last
	set f $fProps.fBot.fBasic.fVolume.fLast

	set c {label $f.lLast -text "Number of Last Image:" \
		 $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eLast -textvariable Volume(last) $Gui(WEA)}
		eval [subst $c]
	pack $f.lLast -side left -padx $Gui(pad)
	pack $f.eLast -side left -padx $Gui(pad) -expand 1 -fill x

	# Headers
	set f $fProps.fBot.fBasic.fVolume.fHeaders

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side left -pady 5

	set c {label $f.fTitle.l -text "Image Headers:" $Gui(WLA)}
		eval [subst $c]
	pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

	foreach text "Auto Manual" \
		value "1 0" \
		width "5 7" {
		set c {radiobutton $f.fBtns.rMode$value -width $width \
			-text "$text" -value "$value" -variable Volume(readHeaders) \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
	}

	# LabelMap
	set f $fProps.fBot.fBasic.fVolume.fLabelMap

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side left -pady 5

	set c {label $f.fTitle.l -text "Image Data:" $Gui(WLA)}
		eval [subst $c]
	pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

	foreach text "{Grayscale} {Label Map}" \
		value "0 1" \
		width "9 9 " {
		set c {radiobutton $f.fBtns.rMode$value -width $width \
			-text "$text" -value "$value" -variable Volume(labelMap) \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
	}

	# Options
	set f $fProps.fBot.fBasic.fVolume.fOptions

	set c {label $f.lName -text "Name:" \
		 $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eName -textvariable Volume(name) -width 13 $Gui(WEA)}
		eval [subst $c]
	pack  $f.lName -side left -padx $Gui(pad) 
	pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
	pack $f.lName -side left -padx $Gui(pad) 

	# Desc row
	set f $fProps.fBot.fBasic.fVolume.fDesc

	set c {label $f.lDesc -text "Optional Description:" \
		 $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eDesc -textvariable Volume(desc) $Gui(WEA)}
		eval [subst $c]
	pack $f.lDesc -side left -padx $Gui(pad)
	pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x


	#-------------------------------------------
	# Props->Bot->Basic->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fApply

	eval {button $f.bApply -text "Apply" \
		-command "VolumesPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "VolumesPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)


	#-------------------------------------------
	# Props->Bot->Header frame
	#-------------------------------------------
	set f $fProps.fBot.fHeader

	frame $f.fEntry   -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fEntry $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Header->Entry frame
	#-------------------------------------------

	# Entry fields (the loop makes a frame for each variable)
	foreach param "firstPattern resolution \
		pixelSize sliceThickness sliceSpacing \
		gantryDetectorTilt" \
		name "{File Pattern} \
		{Resolution (width in pixels)} {Pixel Size (mm)} \
		{Slice Thickness} {Slice Spacing} {Slice Tilt}" {

	    set f $fProps.fBot.fHeader.fEntry
	    frame $f.f$param   -bg $Gui(activeWorkspace)
	    pack $f.f$param \
		    -side top -fill x -pady $Gui(pad)

	    set f $f.f$param
		set c {label $f.l$param -text "$name:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$param -width 10 -textvariable Volume($param) $Gui(WEA)}
			eval [subst $c]
		pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
		pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
	}

	# Orientation Menu
	set f $fProps.fBot.fHeader.fEntry

	set param "order"
	frame $f.f$param -bg $Gui(activeWorkspace)
	pack $f.f$param \
		-side top -fill x -pady $Gui(pad)

	set f $f.f$param
	set c {label $f.l${param} -text "Orient:" $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mb${param} -relief raised -bd 2 \
		-text "[lindex $Volume(${param}Menu) 0]" \
		-width 10 -menu $f.mb${param}.menu $Gui(WMBA)}; eval [subst $c]
	set Volume(mb${param}) $f.mb${param}
	set c {menu $f.mb${param}.menu $Gui(WMA)}; eval [subst $c]
	
	set m $f.mb${param}.menu
	foreach type $Volume(${param}Menu) {
	    $m add command -label $type -command "VolumesSetOrder $type"
	}
	pack $f.l${param} -side left -padx $Gui(pad) -fill x -anchor w
	pack $f.mb${param} -side left -padx $Gui(pad) -expand 1 -fill x 



	#-------------------------------------------
	# Props->Bot->Header->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fHeader.fApply

	eval {button $f.bApply -text "Apply" \
		-command "VolumesPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "VolumesPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)


	#-------------------------------------------
	# Other frame
	#-------------------------------------------
	set fOther $Module(Volumes,fOther)
	set f $fOther

	# Frames
	frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fRange  -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
	pack $f.fRange  -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Other->Active frame
	#-------------------------------------------
	set f $fOther.fActive

	set c {label $f.lActive -text "Active Volume: " $Gui(BLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbActive
	lappend Volume(mActiveList)  $f.mbActive.m


	#-------------------------------------------
	# Other->Range frame
	#-------------------------------------------
	set f $fOther.fRange

	frame $f.fAuto    -bg $Gui(activeWorkspace)
	frame $f.fSliders -bg $Gui(activeWorkspace)
	pack $f.fAuto -pady $Gui(pad) -side top -fill x -expand 1
	pack $f.fSliders -side top -fill x -expand 1

	#-------------------------------------------
	# Other->Range->Auto frame
	#-------------------------------------------
	set f $fOther.fRange.fAuto

	set c {label $f.lAuto -text "Slider Range:" $Gui(WLA)}; eval [subst $c]
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach value "1 0" text "Auto Manual" width "5 7" {
		set c {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(rangeAuto) \
			-command "MainVolumesSetParam RangeAuto; MainVolumesRender" \
			$Gui(WCA)}; eval [subst $c]
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Other->Range->Sliders frame
	#-------------------------------------------
	set f $fOther.fRange.fSliders

	foreach slider "Low High" {
		set c {label $f.l${slider} -text "${slider}:" $Gui(WLA)}
			eval [subst $c]
		set c {entry $f.e${slider} -width 7 \
			-textvariable Volume(range${slider}) $Gui(WEA)}; eval [subst $c]
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam Range${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam Range${slider}; MainVolumesRender"
		grid $f.l${slider} $f.e${slider}  -padx 2 -pady $Gui(pad) -sticky w
	}
	
}

proc VolumesSetPropertyType {} {
	global Volume
	
	raise $Volume(f$Volume(propertyType))
}
 
proc VolumesPropsApply {} {
	global Volume Label Module Mrml Path

	set m $Volume(activeID)
	if {$m == ""} {return}
	
	# Manual headers
	if {$Volume(readHeaders) == "0"} {
		# if on basic frame, switch to header frame.
	    if {$Volume(propertyType) != "Header"} {
			set Volume(propertyType) Header
			VolumesSetPropertyType
			return
	    }
	}

	if {$m == "NEW"} {
		set i $Volume(nextID)
		incr Volume(nextID)
		lappend Volume(idList) $i
		vtkMrmlVolumeNode Volume($i,node)
		set n Volume($i,node)
		$n SetID               $i

		# These get set down below, but we need them before MainUpdateMRML

		# Manual headers
		if {$Volume(readHeaders) == "0"} {
			$n SetFilePrefix [file root $Volume(first)]
			$n SetFilePattern $Volume(firstPattern)
			$n SetFullPrefix [file join $Path(root) [$n GetFilePrefix]]
			set firstNum [MainFileFindImageNumber First \
				[file join $Path(root) $Volume(first)]]
			$n SetImageRange $firstNum $Volume(last)
			$n SetDimensions $Volume(resolution) $Volume(resolution)
			eval $n SetSpacing $Volume(pixelSize) $Volume(pixelSize) \
				[expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
	#		$n SetScalarTypeTo$Volume(scalarType)
	#		$n SetNumScalars $Volume(numScalars)
			$n SetLittleEndian $Volume(littleEndian)
			$n SetTilt $Volume(gantryDetectorTilt)
			$n ComputeRasToIjkFromScanOrder $Volume(order)
		
		# Read headers
		} else {
			if {[GetHeaderInfo [file join $Path(root) $Volume(first)] \
				$Volume(last) $n 1] == "-1"} {
			    set msg "No header information found. Please enter header info manually."
			    puts $msg
			    tk_messageBox -message $msg
			    # set readHeaders to manual
			    set Volume(readHeaders) "0"
			    # switch to vols->props->header frame
			    set Volume(propertyType) Header
			    VolumesSetPropertyType
			    return
			}
		}

		$n SetName $Volume(name)

		Mrml(dataTree) AddItem $n
		MainUpdateMRML
		set Volume(freeze) 0
		MainVolumesSetActive $i
		set m $i
	}

	Volume($m,node) SetName $Volume(name)
	Volume($m,node) SetFilePrefix [file root $Volume(first)]
	Volume($m,node) SetFilePattern $Volume(firstPattern)
	# *** fix this:
	Volume($m,node) SetFullPrefix  $Volume(first)
	# this needs print header to get first image number
	set firstNum [MainFileFindImageNumber First \
			[file join $Path(root) $Volume(first)]]
	Volume($m,node) SetImageRange $firstNum $Volume(last)

	# If tabs are frozen, then 
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
	
	MainUpdateMRML
}

proc VolumesPropsCancel {} {
	global Volume Module

	# Reset props
	set m $Volume(activeID)
	if {$m == "NEW"} {
		set m [lindex $Volume(idList) 0]
	}
	set Volume(freeze) 0
	MainVolumesSetActive $m

	# Unfreeze
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
}

#-------------------------------------------------------------------------------
# VolumesSetFirst
#-------------------------------------------------------------------------------
proc VolumesSetFirst {} {
	global Volume Path
	
	set typelist {
		{"All Datas" {*}}
	}
	set filename [GetOpenFile $Path(root) $Volume(first) \
		$typelist 001 "First Image in Volume" 0]
	if {$filename == ""} {return}
    
	# Store for next time 
	set Path(prefixOpenVolume) [file root $filename]

	# volumeFirst is a relative name (prefix.001) to the root.
	# volumeLast is an image number
	
	set Volume(first) $filename
	set Volume(name)  [file root [file tail $filename]]
	set Volume(last)  [MainFileFindImageNumber Last \
		[file join $Path(root) $filename]]

puts "file tail filename [file tail $filename]"
puts "	[file join $Path(root) $filename]"
puts " last image number: [MainFileFindImageNumber Last\
		[file join $Path(root) $filename]]"
}
#-------------------------------------------------------------------------------
# VolumesSetOrder
#-------------------------------------------------------------------------------
proc VolumesSetOrder {order} {
	global Volume Gui

        set Volume(order) [lindex $Volume(orderList)\
		[lsearch $Volume(orderMenu) $order]]
        $Volume(mborder) config -text $order
}

#-------------------------------------------------------------------------------
# VolumesSetLast
#-------------------------------------------------------------------------------
proc VolumesSetLast {} {
	global Path Volume

	set Volume(last) [MainFileFindImageNumber Last\
		[file join $Path(root) $Volume(first)]]
	set Volume(name) [file root [file tail $Volume(first)]]
}
