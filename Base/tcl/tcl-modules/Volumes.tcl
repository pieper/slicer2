#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        Volumes.tcl
# PROCEDURES:  
#   VolumesInit
#   VolumesBuildGUI
#   VolumesSetPropertyType
#   VolumesPropsCancel
#   VolumesSetFirst
#   VolumesSetScanOrder
#   VolumesSetScalarType
#   VolumesSetLast
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC VolumesInit
# 
# .ARGS
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

	# For now, never display histograms to avoid bug in histWin Render
	# call in MainVolumesSetActive. (This happened when starting slicer,
	# switching to Volumes panel, switching back to Data, and then 
	# adding 2 transforms.)
	# Windows98 Version II can't render histograms
	set Volume(histogram) Off
#	if {$Gui(pc) == 1} {
#		set Volume(histogram) Off
#	}

	# Define Dependencies
	set Module($m,depend) ""

	# Set version info
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.29 $} {$Date: 2000/02/25 16:26:33 $}]

	# Props
	set Volume(propertyType) Basic
	# text for menus displayed on Volumes->Props->Header GUI
	set Volume(scalarTypeMenu) "Char UnsignedChar Short UnsignedShort\ 
	{Int} UnsignedInt Long UnsignedLong Float Double"
	set Volume(scanOrderMenu) "{Sagittal:LR} {Sagittal:RL} {Axial:SI}\
		{Axial:IS} {Coronal:AP} {Coronal:PA}"
	# corresponding values to use in Volume(scanOrder)
	set Volume(scanOrderList) "LR RL SI IS AP PA OB" 
	
	MainVolumesSetGUIDefaults
}

#-------------------------------------------------------------------------------
# .PROC VolumesBuildGUI
# 
# .ARGS
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
Description by tab:
<P>
<UL>
<LI><B>Display:</B> Set the volume you wish to change as the 
<B>Active Volume</B>.  Then adjust these:
<BR><B>Window/Level:</B> sets which pixel values will have the most
visible range of color values.
<BR><B>Threshold:</B> pixels that do not satisfy the threshold appear
transparent on the reformatted slices.  Use this to clip away occluding
slice planes.
<BR><B>Palette:</B> select the color scheme for the data. Overlay colored
functional data on graylevel anatomical data.
<BR><B>Interpolate:</B> indicate whether to interpolate between voxels
on the reformatted slices.
<BR><LI><B>Props</B> set the header information
<BR><LI><B>Other</B> The <B>Slider Range</B> for the Window/Level/Threshold
sliders is normally automatically set according to the min and max voxel 
values in the volume.  However, some applications, such as monitoring 
thermal surgery, requires setting these manually to retain the same color 
scheme as the volume's data changes over time due to realtime data
acquisition.
 "
	regsub -all "\n" $help { } help
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

	eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
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

	eval {label $f.lAuto -text "Window/Level:"} $Gui(WLA)
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -padx $Gui(pad)  -pady $Gui(pad) -fill x

	foreach value "1 0" text "Auto Manual" width "5 7" {
		eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoWindowLevel) \
			-command "MainVolumesSetParam AutoWindowLevel; MainVolumesRender"} $Gui(WCA)
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Display->WinLvl->Sliders frame
	#-------------------------------------------
	set f $fDisplay.fWinLvl.fSliders

	foreach slider "Window Level" text "Win Lev" {
		eval {label $f.l${slider} -text "$text:"} $Gui(WLA)
		eval {entry $f.e${slider} -width 6 \
			-textvariable Volume([Uncap ${slider}])} $Gui(WEA)
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		eval {scale $f.s${slider} -from 1 -to 700 -length 140\
			-variable Volume([Uncap ${slider}])  -resolution 1 \
			-command "MainVolumesSetParam ${slider}; MainVolumesRenderActive"} \
			 $Gui(WSA) {-sliderlength 14}
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

	eval {label $f.lAuto -text "Threshold: "} $Gui(WLA)
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -fill x

	foreach value "1 0" text "Auto Manual" width "5 7" {
		eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoThreshold) \
			-command "MainVolumesSetParam AutoThreshold; MainVolumesRender"} $Gui(WCA)
		pack $f.fAuto.rAuto$value -side left -fill x
	}
	eval {checkbutton $f.cApply \
		-text "Apply" -variable Volume(applyThreshold) \
		-command "MainVolumesSetParam ApplyThreshold; MainVolumesRender" -width 6 \
		-indicatoron 0} $Gui(WCA)
	pack $f.cApply -side left -padx $Gui(pad)

	#-------------------------------------------
	# Display->Thresh->Sliders frame
	#-------------------------------------------
	set f $fDisplay.fThresh.fSliders

	foreach slider "Lower Upper" text "Lo Hi" {
		eval {label $f.l${slider} -text "$text:"} $Gui(WLA)
		eval {entry $f.e${slider} -width 6 \
			-textvariable Volume([Uncap ${slider}]Threshold)} $Gui(WEA)
			bind $f.e${slider} <Return>   \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
			bind $f.e${slider} <FocusOut> \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
		eval {scale $f.s${slider} -from 1 -to 700 -length 140 \
			-variable Volume([Uncap ${slider}]Threshold)  -resolution 1 \
			-command "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"} \
			$Gui(WSA) {-sliderlength 14}
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

	eval {label $f.lLUT -text "Palette:"} $Gui(WLA)
	eval {menubutton $f.mbLUT \
		-text "$Lut([lindex $Lut(idList) 0],name)" \
			-relief raised -bd 2 -width 9 \
		-menu $f.mbLUT.menu} $Gui(WMBA)
		eval {menu $f.mbLUT.menu} $Gui(WMA)
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

	eval {label $f.lInterpolate -text "Interpolation:"} $Gui(WLA)
	pack $f.lInterpolate -pady $Gui(pad) -padx $Gui(pad) -side left -fill x

	foreach value "1 0" text "On Off" width "4 4" {
		eval {radiobutton $f.rInterp$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(interpolate) \
			-command "MainVolumesSetParam Interpolate; MainVolumesRender"} $Gui(WCA)
		pack $f.rInterp$value -side left -fill x
	}


	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(Volumes,fProps)
	set f $fProps

	frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fBot -bg $Gui(activeWorkspace) -height 310
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

	eval {label $f.l -text "First Image File:"} $Gui(WLA)
	eval {button $f.bFind -text "Browse..." -width 9 \
		-command "VolumesSetFirst"} $Gui(WBA)
	pack $f.l $f.bFind -side left -padx $Gui(pad) 

	# First
	set f $fProps.fBot.fBasic.fVolume.fFirst

	eval {entry $f.eFirst -textvariable Volume(firstFile)} $Gui(WEA)
		bind $f.eFirst <Return> "VolumesSetFirst"
		bind $f.eFirst <Tab> "VolumesSetLast"
	pack $f.eFirst -side left -padx $Gui(pad) -expand 1 -fill x

	# Last
	set f $fProps.fBot.fBasic.fVolume.fLast

	eval {label $f.lLast -text "Number of Last Image:"} $Gui(WLA)
	eval {entry $f.eLast -textvariable Volume(lastNum)} $Gui(WEA)
	pack $f.lLast -side left -padx $Gui(pad)
	pack $f.eLast -side left -padx $Gui(pad) -expand 1 -fill x

	# Headers
	set f $fProps.fBot.fBasic.fVolume.fHeaders

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side left -pady 5

	eval {label $f.fTitle.l -text "Image Headers:"} $Gui(WLA)
	pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

	foreach text "Auto Manual" \
		value "1 0" \
		width "5 7" {
		eval {radiobutton $f.fBtns.rMode$value -width $width \
			-text "$text" -value "$value" -variable Volume(readHeaders) \
			-indicatoron 0} $Gui(WCA)
		pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
	}

	# LabelMap
	set f $fProps.fBot.fBasic.fVolume.fLabelMap

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side left -pady 5

	eval {label $f.fTitle.l -text "Image Data:"} $Gui(WLA)
	pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

	foreach text "{Grayscale} {Label Map}" \
		value "0 1" \
		width "9 9 " {
		eval {radiobutton $f.fBtns.rMode$value -width $width \
			-text "$text" -value "$value" -variable Volume(labelMap) \
			-indicatoron 0} $Gui(WCA)
		pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
	}

	# Options
	set f $fProps.fBot.fBasic.fVolume.fOptions

	eval {label $f.lName -text "Name:"} $Gui(WLA)
	eval {entry $f.eName -textvariable Volume(name) -width 13} $Gui(WEA)
	pack  $f.lName -side left -padx $Gui(pad) 
	pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
	pack $f.lName -side left -padx $Gui(pad) 

	# Desc row
	set f $fProps.fBot.fBasic.fVolume.fDesc

	eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
	eval {entry $f.eDesc -textvariable Volume(desc)} $Gui(WEA)
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
	grid $f.bApply $f.bCancel -padx $Gui(pad)


	#-------------------------------------------
	# Props->Bot->Header frame
	#-------------------------------------------
	set f $fProps.fBot.fHeader

	frame $f.fEntry   -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fEntry $f.fApply \
		-side top -fill x -pady 2

	#-------------------------------------------
	# Props->Bot->Header->Entry frame
	#-------------------------------------------

	# Entry fields (the loop makes a frame for each variable)
	foreach param "filePattern resolution \
		pixelSize sliceThickness sliceSpacing" \
		name "{File Pattern} \
		{Resolution (width in pixels)} {Pixel Size (mm)} \
		{Slice Thickness} {Slice Spacing}" {

	    set f $fProps.fBot.fHeader.fEntry
	    frame $f.f$param   -bg $Gui(activeWorkspace)
	    pack $f.f$param -side top -fill x -pady 2

	    set f $f.f$param
	    eval {label $f.l$param -text "$name:"} $Gui(WLA)
	    eval {entry $f.e$param -width 10 -textvariable Volume($param)} $Gui(WEA)
	    pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
	    pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
	}

	# scan order Menu
	set param scanOrder
	set name "Scan Order"
	set f $fProps.fBot.fHeader.fEntry
	frame $f.f$param -bg $Gui(activeWorkspace)
	pack $f.f$param -side top -fill x -pady 2
	
	set f $f.f$param
	eval {label $f.l${param} -text "$name:"} $Gui(WLA)
	# button text corresponds to default scan order value in Volume(scanOrder)
	eval {menubutton $f.mb${param} -relief raised -bd 2 \
		-text [lindex $Volume(scanOrderMenu)\
		[lsearch $Volume(scanOrderList) $Volume(scanOrder)]] \
		-width 10 -menu $f.mb${param}.menu} $Gui(WMBA)
	set Volume(mb${param}) $f.mb${param}
	eval {menu $f.mb${param}.menu} $Gui(WMA)
	
	set m $f.mb${param}.menu
	foreach label $Volume(${param}Menu)  value $Volume(${param}List) {
	    $m add command -label $label -command "VolumesSetScanOrder $value"
	}
	pack $f.l${param} -side left -padx $Gui(pad) -fill x -anchor w
	pack $f.mb${param} -side left -padx $Gui(pad) -expand 1 -fill x 
    
	# scalar type menu
	set param scalarType
	set name "Scalar Type"
	set f $fProps.fBot.fHeader.fEntry
	frame $f.f$param -bg $Gui(activeWorkspace)
	pack $f.f$param -side top -fill x -pady 2
	
	set f $f.f$param
	eval {label $f.l${param} -text "$name:"} $Gui(WLA)
	eval {menubutton $f.mb${param} -relief raised -bd 2 \
		-text $Volume(scalarType)\
		-width 10 -menu $f.mb${param}.menu} $Gui(WMBA)
	set Volume(mb${param}) $f.mb${param}
	eval {menu $f.mb${param}.menu} $Gui(WMA)
	
	set m $f.mb${param}.menu
	foreach type $Volume(${param}Menu) {
	    $m add command -label $type -command "VolumesSetScalarType $type"
	}
	pack $f.l${param} -side left -padx $Gui(pad) -fill x -anchor w
	pack $f.mb${param} -side left -padx $Gui(pad) -expand 1 -fill x 
    
	# more Entry fields (the loop makes a frame for each variable)
	foreach param "	gantryDetectorTilt numScalars" \
		name "{Slice Tilt} {Num Scalars}" {

	    set f $fProps.fBot.fHeader.fEntry
	    frame $f.f$param -bg $Gui(activeWorkspace)
	    pack $f.f$param -side top -fill x -pady 2

	    set f $f.f$param
	    eval {label $f.l$param -text "$name:"} $Gui(WLA)
	    eval {entry $f.e$param -width 10 -textvariable Volume($param)} $Gui(WEA)
	    pack $f.l$param -side left -padx $Gui(pad) -fill x -anchor w
	    pack $f.e$param -side left -padx $Gui(pad) -expand 1 -fill x
	}

	# byte order
	set f $fProps.fBot.fHeader.fEntry
	frame $f.fEndian -bg $Gui(activeWorkspace)
	pack $f.fEndian -side top -fill x -pady 2
	set f $f.fEndian

	eval {label $f.l -text "Little Endian (PC,SGI):"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach value "1 0" text "Yes No" width "4 3" {
		eval {radiobutton $f.f.r$value -width $width -indicatoron 0\
			-text $text -value $value -variable Volume(littleEndian)} $Gui(WCA)
		pack $f.f.r$value -side left -fill x
	}


	#-------------------------------------------
	# Props->Bot->Header->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fHeader.fApply

	eval {button $f.bApply -text "Apply" \
		-command "VolumesPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "VolumesPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad)


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

	eval {label $f.lActive -text "Active Volume: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
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

	eval {label $f.lAuto -text "Slider Range:"} $Gui(WLA)
	frame $f.fAuto -bg $Gui(activeWorkspace)
	pack $f.lAuto $f.fAuto -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach value "1 0" text "Auto Manual" width "5 7" {
		eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(rangeAuto) \
			-command "MainVolumesSetParam RangeAuto; MainVolumesRender"} $Gui(WCA)
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Other->Range->Sliders frame
	#-------------------------------------------
	set f $fOther.fRange.fSliders

	foreach slider "Low High" {
		eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
		eval {entry $f.e${slider} -width 7 \
			-textvariable Volume(range${slider})} $Gui(WEA)
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam Range${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam Range${slider}; MainVolumesRender"
		grid $f.l${slider} $f.e${slider}  -padx 2 -pady $Gui(pad) -sticky w
	}
	
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetPropertyType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetPropertyType {} {
	global Volume
	
	raise $Volume(f$Volume(propertyType))
}
 
proc VolumesPropsApply {} {
	global Lut Volume Label Module Mrml

	set m $Volume(activeID)
	if {$m == ""} {return}
	
	# Validate name
	if {$Volume(name) == ""} {
		tk_messageBox -message "Please enter a name that will allow you to distinguish this model."
		return
	}
	if {[ValidateName $Volume(name)] == 0} {
		tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
		return
	}
	# lastNum
	if {[ValidateInt $Volume(lastNum)] == 0} {
		tk_messageBox -message "The last number must be an integer."
		return
	}
	# resolution
	if {[ValidateInt $Volume(resolution)] == 0} {
		tk_messageBox -message "The resolution must be an integer."
		return
	}
	# pixel size
	if {[ValidateFloat $Volume(pixelSize)] == 0} {
		tk_messageBox -message "The pixel size must be a number."
		return
	}
	# slice thickness
	if {[ValidateFloat $Volume(sliceThickness)] == 0} {
		tk_messageBox -message "The slice thickness must be a number."
		return
	}
	# slice spacing
	if {[ValidateFloat $Volume(sliceSpacing)] == 0} {
		tk_messageBox -message "The slice spacing must be a number."
		return
	}
	# tilt
	if {[ValidateFloat $Volume(gantryDetectorTilt)] == 0} {
		tk_messageBox -message "The tilt must be a number."
		return
	}
	# num scalars
	if {[ValidateInt $Volume(numScalars)] == 0} {
		tk_messageBox -message "The number of scalars must be an integer."
		return
	}

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
		set n [MainMrmlAddNode Volume]
		set i [$n GetID]

		# Manual headers
		if {$Volume(readHeaders) == "0"} {
		    # These get set down below, but we need them before MainUpdateMRML
			$n SetFilePrefix [file root $Volume(firstFile)]
			$n SetFilePattern $Volume(filePattern)
			$n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
			set firstNum [MainFileFindImageNumber First \
				[file join $Mrml(dir) $Volume(firstFile)]]
			$n SetImageRange $firstNum $Volume(lastNum)
			$n SetDimensions $Volume(resolution) $Volume(resolution)
			eval $n SetSpacing $Volume(pixelSize) $Volume(pixelSize) \
				[expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
			$n SetScalarTypeTo$Volume(scalarType)
			$n SetNumScalars $Volume(numScalars)
			$n SetLittleEndian $Volume(littleEndian)
			$n SetTilt $Volume(gantryDetectorTilt)
			$n ComputeRasToIjkFromScanOrder $Volume(scanOrder)
		
		# Read headers
		} else {
			set errmsg [GetHeaderInfo [file join $Mrml(dir) $Volume(firstFile)] \
				$Volume(lastNum) $n 1]
			if {$errmsg == "-1"} {
			    set msg "No header information found. Please enter header info manually."
			    puts $msg
			    tk_messageBox -message $msg
			    # set readHeaders to manual
			    set Volume(readHeaders) 0
			    # switch to vols->props->header frame
			    set Volume(propertyType) Header
			    VolumesSetPropertyType
				# Remove node
				MainMrmlUndoAddNode Volume $n
			    return
			} elseif {$errmsg != ""} {
				# File not found, most likely
			    puts $errmsg
			    tk_messageBox -message $errmsg
				# Remove node
				MainMrmlUndoAddNode Volume $n
				return
			}
		}

		$n SetName $Volume(name)
		$n SetDescription $Volume(desc)
		$n SetLabelMap $Volume(labelMap)

		MainUpdateMRML
		# If failed, then it's no longer in the idList
		if {[lsearch $Volume(idList) $i] == -1} {
			return
		}
		set Volume(freeze) 0
		MainVolumesSetActive $i
		set m $i

		MainSlicesSetVolumeAll Back $i
	}

	# Update all fields that the user changed (not stuff that would 
	# need a file reread)

	Volume($m,node) SetName $Volume(name)
	Volume($m,node) SetDescription $Volume(desc)
	Volume($m,node) SetLabelMap $Volume(labelMap)
	eval Volume($m,node) SetSpacing $Volume(pixelSize) $Volume(pixelSize) \
		[expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
	Volume($m,node) SetTilt $Volume(gantryDetectorTilt)

	# This line can't be allowed to overwrite a RasToIjk matrix made
	# from headers when the volume is first created.
	#
	if {$Volume(readHeaders) == "0"} {
		Volume($m,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)
	}

	# If tabs are frozen, then 
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
	
	# Update pipeline
	MainVolumesUpdate $m

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC VolumesPropsCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
# .PROC VolumesSetFirst
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetFirst {} {
	global Volume Mrml

	# Cannot have blank prefix
	if {$Volume(firstFile) == ""} {
		set Volume(firstFile) I
	}

 	# Show popup initialized to root plus any typed pathname
	set filename [file join $Mrml(dir) $Volume(firstFile)]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "First Image In Volume" \
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Store first image file as a relative filename to the root (prefix.001)
	set Volume(firstFile) [MainFileGetRelativePrefix $filename][file \
		extension $filename]

	set Volume(name)  [file root [file tail $filename]]

	# lastNum is an image number
	set Volume(lastNum)  [MainFileFindImageNumber Last \
		[file join $Mrml(dir) $filename]]
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetScanOrder
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetScanOrder {order} {
	global Volume
    

        set Volume(scanOrder) $order

        # set the button text to the matching order from the scanOrderMenu
        $Volume(mbscanOrder) config -text [lindex $Volume(scanOrderMenu)\
		[lsearch $Volume(scanOrderList) $order]]
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetScalarType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetScalarType {type} {
	global Volume

        set Volume(scalarType) $type

        # update the button text
        $Volume(mbscalarType) config -text $type
}

#-------------------------------------------------------------------------------
# .PROC VolumesSetLast
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetLast {} {
	global Mrml Volume

	set Volume(lastNum) [MainFileFindImageNumber Last\
		[file join $Mrml(dir) $Volume(firstFile)]]
	set Volume(name) [file root [file tail $Volume(firstFile)]]
}
