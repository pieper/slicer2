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
# FILE:        Volumes.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   VolumesInit
#   VolumesBuildGUI
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC VolumesInit
# .END
#-------------------------------------------------------------------------------
proc VolumesInit {} {
	global Volume Module Gui

	# Define Tabs
	set m Volumes
	set Module($m,row1List) "Help WL Other"
	set Module($m,row1Name) "{Help} {Window/Level} {Other}"
	set Module($m,row1,tab) WL

	# Define Procedures
	set Module($m,procGUI)  VolumesBuildGUI

	# Windows98 Version II can't render histograms
	set Volume(histogram) On
	if {$Gui(pc) == 1} {
		set Volume(histogram) Off
	}
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
	# WL
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
	# WL frame
	#-------------------------------------------
	set fWL $Module(Volumes,fWL)
	set f $fWL

	# Frames
	frame $f.fActive    -bg $Gui(activeWorkspace)
	frame $f.fWinLvl    -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fThresh    -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fHistogram -bg $Gui(activeWorkspace)
	frame $f.fInterpolate -bg $Gui(activeWorkspace)
	pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
	pack $f.fWinLvl $f.fThresh $f.fHistogram $f.fInterpolate \
		-side top -pady $Gui(pad) -padx 2 -fill x

	#-------------------------------------------
	# WL->Active frame
	#-------------------------------------------
	set f $fWL.fActive

	set c {label $f.lActive -text "Active Volume: " $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbActive
	lappend Volume(mActiveList)  $f.mbActive.m


	#-------------------------------------------
	# WL->WinLvl frame
	#-------------------------------------------
	set f $fWL.fWinLvl

	frame $f.fAuto    -bg $Gui(activeWorkspace)
	frame $f.fSliders -bg $Gui(activeWorkspace)
	pack $f.fAuto $f.fSliders -side top -fill x -expand 1

	#-------------------------------------------
	# WL->WinLvl->Auto frame
	#-------------------------------------------
	set f $fWL.fWinLvl.fAuto

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
	# WL->WinLvl->Sliders frame
	#-------------------------------------------
	set f $fWL.fWinLvl.fSliders

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
	# WL->Thresh frame
	#-------------------------------------------
	set f $fWL.fThresh

	frame $f.fAuto    -bg $Gui(activeWorkspace)
	frame $f.fSliders -bg $Gui(activeWorkspace)
	pack $f.fAuto $f.fSliders -side top  -fill x -expand 1

	#-------------------------------------------
	# WL->Thresh->Auto frame
	#-------------------------------------------
	set f $fWL.fThresh.fAuto

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
	# WL->Thresh->Sliders frame
	#-------------------------------------------
	set f $fWL.fThresh.fSliders

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
	# WL->Histogram frame
	#-------------------------------------------
	set f $fWL.fHistogram

	frame $f.fHistBorder -bg $Gui(activeWorkspace) -relief sunken -bd 2
	frame $f.fLut -bg $Gui(activeWorkspace)
	pack $f.fLut $f.fHistBorder -side left -padx $Gui(pad) -pady $Gui(pad)
	
	#-------------------------------------------
	# WL->Histogram->Lut frame
	#-------------------------------------------
	set f $fWL.fHistogram.fLut

	set c {label $f.lLUT -text "Palette:" $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mbLUT -text "Gray" -relief raised -bd 2 -width 9 \
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
	# WL->Histogram->HistBorder frame
	#-------------------------------------------
	set f $fWL.fHistogram.fHistBorder

	if {$Volume(histogram) == "On"} {
		MakeVTKImageWindow hist
		histMapper SetInput [Volume(0,vol) GetHistogramPlot]

		vtkTkImageWindowWidget $f.fHist -iw histWin \
			-width $Volume(histWidth) -height $Volume(histHeight)  
		bind $f.fHist <Expose> {ExposeTkImageViewer %W %x %y %w %h}
		pack $f.fHist
	}

	#-------------------------------------------
	# WL->Interpolate frame
	#-------------------------------------------
	set f $fWL.fInterpolate

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
	# Other frame
	#-------------------------------------------
	set fOther $Module(Volumes,fOther)
	set f $fOther

	# Frames
	frame $f.fActive -bg $Gui(activeWorkspace)
	frame $f.fRange -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
	pack $f.fRange  -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Other->Active frame
	#-------------------------------------------
	set f $fOther.fActive

	set c {label $f.lActive -text "Active Volume: " $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left

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
	# WL->Range->Auto frame
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
	# WL->Range->Sliders frame
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

