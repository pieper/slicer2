#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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

    # Added by Attila Tanacs 10/18/2000
	set Module($m,procEnter) VolumesEnter
	set Module($m,procExit) VolumesExit
        set Volume(DICOMStartDir) "e:/tanacs/medpic/"

    # End

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
                {$Revision: 1.36 $} {$Date: 2000/10/25 18:20:46 $}]

	# Props
	set Volume(propertyType) Basic
	# text for menus displayed on Volumes->Props->Header GUI
	set Volume(scalarTypeMenu) "Char UnsignedChar Short UnsignedShort\ 
	{Int} UnsignedInt Long UnsignedLong Float Double"
	set Volume(scanOrderMenu) "{Sagittal:LR} {Sagittal:RL} {Axial:SI}\
		{Axial:IS} {Coronal:AP} {Coronal:PA}"
	# corresponding values to use in Volume(scanOrder)
	set Volume(scanOrderList) "LR RL SI IS AP PA" 
	
	MainVolumesSetGUIDefaults

        set Volume(DefaultDir) ""
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

	DevAddLabel $f.lAuto "Window/Level:"
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
		DevAddLabel $f.l${slider} "$text:"
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

	DevAddLabel $f.lAuto "Threshold: "
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
		DevAddLabel $f.l${slider} "$text:"
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

	DevAddLabel $f.lLUT "Palette:"
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

	DevAddLabel $f.lInterpolate "Interpolation:"
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

	foreach type "Basic Header DICOM" {
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
	foreach p "Basic Header DICOM" {
		eval {radiobutton $f.f.r$p \
			-text "$p" -command "VolumesSetPropertyType" \
			-variable Volume(propertyType) -value $p -width 6 \
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

    #DevAddFileBrowse $f Volume firstFile "First Image File:" "VolumesSetFirst" "" ""  "Browse for the first Image file" 
        DevAddFileBrowse $f Volume firstFile "First Image File:" "VolumesSetFirst" "" "\$Volume(DefaultDir)"  "Browse for the first Image file" 

        bind $f.efile <Tab> "VolumesSetLast"

	frame $f.fLast     -bg $Gui(activeWorkspace)
	frame $f.fHeaders  -bg $Gui(activeWorkspace)
	frame $f.fLabelMap -bg $Gui(activeWorkspace)
	frame $f.fOptions  -bg $Gui(activeWorkspace)
	frame $f.fDesc     -bg $Gui(activeWorkspace)

	pack $f.fLast $f.fHeaders $f.fLabelMap $f.fOptions \
		$f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	# Last
	set f $fProps.fBot.fBasic.fVolume.fLast

	DevAddLabel $f.lLast "Number of Last Image:"
	eval {entry $f.eLast -textvariable Volume(lastNum)} $Gui(WEA)
	pack $f.lLast -side left -padx $Gui(pad)
	pack $f.eLast -side left -padx $Gui(pad) -expand 1 -fill x

	# Headers
	set f $fProps.fBot.fBasic.fVolume.fHeaders

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side left -pady 5

	DevAddLabel $f.fTitle.l "Image Headers:"
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

	DevAddLabel $f.fTitle.l "Image Data:"
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

        DevAddButton $f.bApply "Apply" "VolumesPropsApply; RenderAll" 8
        DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
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

	# Scan Order Menu
	set f $fProps.fBot.fHeader.fEntry
	frame $f.fscanOrder -bg $Gui(activeWorkspace)
	pack $f.fscanOrder -side top -fill x -pady 2
	
	set f $f.fscanOrder
	eval {label $f.lscanOrder -text "Scan Order:"} $Gui(WLA)
	# button text corresponds to default scan order value Volume(scanOrder)
	eval {menubutton $f.mbscanOrder -relief raised -bd 2 \
		-text [lindex $Volume(scanOrderMenu)\
		[lsearch $Volume(scanOrderList) $Volume(scanOrder)]] \
		-width 10 -menu $f.mbscanOrder.menu} $Gui(WMBA)
	set Volume(mbscanOrder) $f.mbscanOrder
	eval {menu $f.mbscanOrder.menu} $Gui(WMA)
	
	set m $f.mbscanOrder.menu
	foreach label $Volume(scanOrderMenu) value $Volume(scanOrderList) {
	    $m add command -label $label -command "VolumesSetScanOrder $value"
	}
	pack $f.lscanOrder -side left -padx $Gui(pad) -fill x -anchor w
	pack $f.mbscanOrder -side left -padx $Gui(pad) -expand 1 -fill x 

    
	# Scalar Type Menu
	set f $fProps.fBot.fHeader.fEntry
	frame $f.fscalarType -bg $Gui(activeWorkspace)
	pack $f.fscalarType -side top -fill x -pady 2
	
	set f $f.fscalarType
	eval {label $f.lscalarType -text "Scalar Type:"} $Gui(WLA)
	eval {menubutton $f.mbscalarType -relief raised -bd 2 \
		-text $Volume(scalarType)\
		-width 10 -menu $f.mbscalarType.menu} $Gui(WMBA)
	set Volume(mbscalarType) $f.mbscalarType
	eval {menu $f.mbscalarType.menu} $Gui(WMA)
	
	set m $f.mbscalarType.menu
	foreach type $Volume(scalarTypeMenu) {
	    $m add command -label $type -command "VolumesSetScalarType $type"
	}
	pack $f.lscalarType -side left -padx $Gui(pad) -fill x -anchor w
	pack $f.mbscalarType -side left -padx $Gui(pad) -expand 1 -fill x 
    
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

        DevAddButton $f.bApply "Apply" "VolumesPropsApply; RenderAll" 8
        DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
	grid $f.bApply $f.bCancel -padx $Gui(pad)

    # Added by Attila Tanacs 10/6/2000 
    
    #-------------------------------------------
    # Props->Bot->DICOM frame
    #-------------------------------------------
    set f $fProps.fBot.fDICOM
    
    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fVolume $f.fApply \
            -side top -fill x -pady $Gui(pad)
    
    #-------------------------------------------
    # Props->Bot->DICOM->fVolume frame
    #-------------------------------------------
    
    set f $fProps.fBot.fDICOM.fVolume
    
    #DevAddFileBrowse $f Volume firstFile "First Image File:" "VolumesSetFirst" "" ""  "Browse for the first Image file"
    #bind $f.efile <Tab> "VolumesSetLast"
    
    frame $f.fSelect -bg $Gui(activeWorkspace)
    #frame $f.fLast     -bg $Gui(activeWorkspace)
    #frame $f.fHeaders  -bg $Gui(activeWorkspace)
    #frame $f.fLabelMap -bg $Gui(activeWorkspace)
    set fileNameListbox [ScrolledListbox $f.fFiles 0 0 -height 5 -bg $Gui(activeWorkspace)]
    set Volume(dICOMFileListbox) $fileNameListbox
    #frame $f.fFiles -bg $Gui(activeWorkspace)
    frame $f.fOptions  -bg $Gui(activeWorkspace)
    frame $f.fDesc     -bg $Gui(activeWorkspace)
    
    pack $f.fSelect $f.fFiles $f.fOptions \
            $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    
    # Select
    DevAddButton $f.fSelect.bSelect "Select DICOM Volume" [list DICOMSelectMain $fileNameListbox]
    DevAddButton $f.fSelect.bExtractHeader "Extract Header" { HandleExtractHeader }
    pack $f.fSelect.bSelect $f.fSelect.bExtractHeader -padx $Gui(pad) -pady $Gui(pad)
    
    # Files
    
    $fileNameListbox delete 0 end

    # Options
    set f $fProps.fBot.fDICOM.fVolume.fOptions
    
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable Volume(name) -width 13} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad)
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad)
    
    # Desc row
    set f $fProps.fBot.fDICOM.fVolume.fDesc
    
    eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
    eval {entry $f.eDesc -textvariable Volume(desc)} $Gui(WEA)
    pack $f.lDesc -side left -padx $Gui(pad)
    pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x
    
    
    #-------------------------------------------
    # Props->Bot->Basic->Apply frame
    #-------------------------------------------
    set f $fProps.fBot.fDICOM.fApply
    
    DevAddButton $f.bApply "Apply" "VolumesPropsApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)
    
    # End
    
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

    set Volume(isDICOM) [expr [llength $Volume(dICOMFileList)] > 0]
	
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
    if { $Volume(isDICOM) == 0 } {
	if {[ValidateInt $Volume(lastNum)] == 0} {
		tk_messageBox -message "The last number must be an integer."
		return
	}
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

    if { $Volume(isDICOM) } {
	set Volume(readHeaders) 0
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
	   
        # Added by Attila Tanacs 10/11/2000

        $n DeleteDICOMFileNames
        for  {set j 0} {$j < [llength $Volume(dICOMFileList)]} {incr j} {
            $n AddDICOMFileName [$Volume(dICOMFileListbox) get $j]
        }
	    
	if { $Volume(isDICOM) } {
	    #$Volume(dICOMFileListbox) insert 0 [$n GetNumberOfDICOMFiles];
	    set firstNum 1
	    set Volume(lastNum) [llength $Volume(dICOMFileList)]
	}	    

	# End

        # Manual headers

		if {$Volume(readHeaders) == "0"} {
		    # These get set down below, but we need them before MainUpdateMRML
			$n SetFilePrefix [file root $Volume(firstFile)]
			$n SetFilePattern $Volume(filePattern)
			$n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
			if { !$Volume(isDICOM) } {
			set firstNum [MainFileFindImageNumber First [file join $Mrml(dir) $Volume(firstFile)]]
			}
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
# Called after the User Selects the first file of the volume.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesSetFirst {} {
	global Volume Mrml

	set Volume(name)  [file root [file tail $Volume(firstFile)]]
        set Volume(DefaultDir) [file dirname $Volume(firstFile)]
	# lastNum is an image number
	set Volume(lastNum)  [MainFileFindImageNumber Last \
		[file join $Mrml(dir) $Volume(firstFile)]]
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

#-------------------------------------------------------------------------------
# DICOM related procedures
# Added by Attila Tanacs
# October 2000
# (C) ERC for CISST, Johns Hopkins University, Baltimore
# Any comments? tanacs@cs.jhu.edu
#-------------------------------------------------------------------------------

proc AddListUnique { list arg } {
    upvar $list list2
    if { [expr [lsearch -exact $list2 $arg] == -1] } {
        lappend list2 $arg
    }
}

#-------------------------------------------------------------------------------
# DICOMScrolledListbox modified version of ScrolledListbox
#
# xAlways is 1 if the x scrollbar should be always visible
#-------------------------------------------------------------------------------
proc DICOMScrolledListbox {f xAlways yAlways variable {labeltext "labeltext"} {args ""}} {
	global Gui
	
    set fmain $f
    frame $fmain -bg $Gui(activeWorkspace)
    eval { label $fmain.head -text $labeltext } $Gui(WLA)
    eval { label $fmain.selected -textvariable $variable } $Gui(WLA)

    frame $fmain.f -bg $Gui(activeWorkspace)
    set f $fmain.f
	if {$xAlways == 1 && $yAlways == 1} { 
		listbox $f.list -selectmode single \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand "$f.yscroll set"
	
	} elseif {$xAlways == 1 && $yAlways == 0} { 
		listbox $f.list -selectmode single \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand [list ScrollSet $f.yscroll \
				[list grid $f.yscroll -row 0 -column 1 -sticky ns]]

	} elseif {$xAlways == 0 && $yAlways == 1} { 
		listbox $f.list -selectmode single \
			-xscrollcommand [list ScrollSet $f.xscroll \
				[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
			-yscrollcommand "$f.yscroll set"

	} else {
		listbox $f.list -selectmode single \
			-xscrollcommand [list ScrollSet $f.xscroll \
				[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
			-yscrollcommand [list ScrollSet $f.yscroll \
				[list grid $f.yscroll -row 0 -column 1 -sticky ns]]
	}

	if {$Gui(smallFont) == 1} {
		eval {$f.list configure \
			-font {helvetica 7 bold} \
			-bg $Gui(normalButton) -fg $Gui(textDark) \
			-selectbackground $Gui(activeButton) \
			-selectforeground $Gui(textDark) \
			-highlightthickness 0 -bd $Gui(borderWidth) \
			-relief sunken -selectborderwidth $Gui(borderWidth)}
	} else {
		eval {$f.list configure \
			-font {helvetica 8 bold} \
			-bg $Gui(normalButton) -fg $Gui(textDark) \
			-selectbackground $Gui(activeButton) \
			-selectforeground $Gui(textDark) \
			-highlightthickness 0 -bd $Gui(borderWidth) \
			-relief sunken -selectborderwidth $Gui(borderWidth)}
	}

	if {$args != ""} {
		eval {$f.list configure} $args
	}

	scrollbar $f.xscroll -orient horizontal \
		-command [list $f.list xview] \
		-bg $Gui(activeWorkspace) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -bd $Gui(borderWidth)
	scrollbar $f.yscroll -orient vertical \
		-command [list $f.list yview] \
		-bg $Gui(activeWorkspace) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -bd $Gui(borderWidth)

	grid $f.list $f.yscroll -sticky news
	grid $f.xscroll -sticky news
	grid rowconfigure $f 0 -weight 1
	grid columnconfigure $f 0 -weight 1

    pack $fmain.head $fmain.selected -anchor nw -pady 5
    pack $fmain.f -fill both -expand true
    pack $fmain -fill both -expand true 

	return $fmain.f.list
}

proc FindDICOM2 { StartDir AddDir Pattern } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames

    set pwd [pwd]
    if [expr [string length $AddDir] > 0] {
        if [catch {cd $AddDir} err] {
            puts stderr $err
            return
        }
    }
    
    vtkDCMParser parser
    foreach match [glob -nocomplain -- $Pattern] {
        #puts stdout [file join $StartDir $match]
        if {[file isdirectory $match]} {
            continue
        }
        set FileName [file join $StartDir $AddDir $match]
        set found [parser OpenFile $match]
        if {[string compare $found "0"] == 0} {
            puts stderr "Can't open file [file join $StartDir $AddDir $match]"
        } else {
            set found [parser FindElement 0x7fe0 0x0010]
            if {[string compare $found "1"] == 0} {
                #
                # image data is available
                #
                
                set DICOMFiles($FindDICOMCounter,FileName) $FileName
                
                if [expr [parser FindElement 0x0010 0x0010] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set PatientName [parser ReadText $Length]
		    if {$PatientName == ""} {
			set PatientName "noname"
		    }
                } else  {
                    set PatientName 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,PatientName) $PatientName
                AddListUnique DICOMPatientNames $PatientName
                
                if [expr [parser FindElement 0x0010 0x0020] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set PatientID [parser ReadText $Length]
		    if {$PatientID == ""} {
			set PatientID "noid"
		    }
                } else  {
                    set PatientID 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,PatientID) $PatientID
                set add {}
                append add "<" $PatientID "><" $PatientName ">"
                AddListUnique DICOMPatientIDsNames $add
                set DICOMFiles($FindDICOMCounter,PatientIDName) $add
                
                if [expr [parser FindElement 0x0020 0x000d] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set StudyInstanceUID [parser ReadText $Length]
                } else  {
                    set StudyInstanceUID 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,StudyInstanceUID) $StudyInstanceUID
                
                if [expr [parser FindElement 0x0020 0x000e] == "1"] {
                    set Length [lindex [split [parser ReadElement]] 3]
                    set SeriesInstanceUID [parser ReadText $Length]
                } else  {
                    set SeriesInstanceUID 'unknown'
                }
                set DICOMFiles($FindDICOMCounter,SeriesInstanceUID) $SeriesInstanceUID
                
		set ImageNumber ""
                if [expr [parser FindElement 0x0020 0x0013] == "1"] {
                    #set Length [lindex [split [parser ReadElement]] 3]
                    #set ImageNumber [parser ReadText $Length]
		    #scan [parser ReadText $length] "%d" ImageNumber
		    
		    set NextBlock [lindex [split [parser ReadElement]] 4]
		    set ImageNumber [parser ReadIntAsciiNumeric $NextBlock]
                }
		if { $ImageNumber == "" } {
		    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
			set NextBlock [lindex [split [parser ReadElement]] 4]
			set ImageNumber [parser ReadFloatAsciiNumeric $NextBlock]
		    } else  {
			set ImageNumber 1
		    }		    
		}
                set DICOMFiles($FindDICOMCounter,ImageNumber) $ImageNumber
                
                incr FindDICOMCounter
                #puts [file join $StartDir $AddDir $match]
            } else {
                #set dim 256
            }
            parser CloseFile
        }
    }
    parser Delete
    
    foreach file [glob -nocomplain *] {
        if [file isdirectory $file] {
            FindDICOM2 [file join $StartDir $AddDir] $file $Pattern
        }
    }
    cd $pwd
}

proc FindDICOM { StartDir Pattern } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMStudyInstanceUIDList
    global DICOMSeriesInstanceUIDList
    global DICOMFileNameArray
    global DICOMFileNameList
    
    if [array exists DICOMFiles] {
        unset DICOMFiles
    }
    if [array exists DICOMFileNameArray] {
        unset DICOMFileNameArray
    }
    set pwd [pwd]
    set FindDICOMCounter 0
    set DICOMPatientNames {}
    set DICOMPatientIDsNames {}
    set DICOMStudyList {}
    set DICOMSeriesList {}
    set DICOMFileNameList {}
    
    if [catch {cd $StartDir} err] {
        puts stderr $err
        cd $pwd
        return
    }
    FindDICOM2 $StartDir "" $Pattern
    cd $pwd
}

proc CreateStudyList { PatientIDName } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMStudyList
    
    set DICOMStudyList {}
    for  {set i 0} {$i < $FindDICOMCounter} {incr i} {
        if {[string compare $DICOMFiles($i,PatientIDName) $PatientIDName] == 0} {
            AddListUnique DICOMStudyList $DICOMFiles($i,StudyInstanceUID)
        }
    }
}

proc CreateSeriesList { PatientIDName StudyUID } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMSeriesList
    
    set DICOMSeriesList {}
    for  {set i 0} {$i < $FindDICOMCounter} {incr i} {
        if {[string compare $DICOMFiles($i,PatientIDName) $PatientIDName] == 0} {
            if {[string compare $DICOMFiles($i,StudyInstanceUID) $StudyUID] == 0} {
                AddListUnique DICOMSeriesList $DICOMFiles($i,SeriesInstanceUID)
            }
        }
    }
}

proc CreateFileNameList { PatientIDName StudyUID SeriesUID} {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMFileNameArray
    global DICOMFileNameList
    
    catch {unset DICOMFileNameArray}
    set count 0
    for  {set i 0} {$i < $FindDICOMCounter} {incr i} {
        if {[string compare $DICOMFiles($i,PatientIDName) $PatientIDName] == 0} {
            if {[string compare $DICOMFiles($i,StudyInstanceUID) $StudyUID] == 0} {
                if {[string compare $DICOMFiles($i,SeriesInstanceUID) $SeriesUID] == 0} {
                    set id [format "%04d_%04d" $DICOMFiles($i,ImageNumber) $count]
                    incr count
                    set DICOMFileNameArray($id) $DICOMFiles($i,FileName)
                }
            }
        }
    }
    set idx [lsort [array name DICOMFileNameArray]]
    set DICOMFileNameList {}
    foreach i $idx {
        lappend DICOMFileNameList $DICOMFileNameArray($i)
    }
}

proc ClickListIDsNames { idsnames study series filenames } {
    global DICOMPatientIDsNames
    global DICOMStudyList
    global DICOMListSelectPatientName
    global DICOMListSelectStudyUID
    global DICOMListSelectSeriesUID

    set nameidx [$idsnames curselection]
    set name [lindex $DICOMPatientIDsNames $nameidx]
    set DICOMListSelectPatientName $name
    CreateStudyList $name
    $study delete 0 end
    eval {$study insert end} $DICOMStudyList
    $series delete 0 end
    $filenames delete 0 end
    set DICOMListSelectStudyUID "none selected"
    set DICOMListSelectSeriesUID "none selected"
}

proc ClickListStudyUIDs { idsnames study series filenames } {
    global DICOMPatientIDsNames
    global DICOMStudyList
    global DICOMSeriesList
    global DICOMListSelectStudyUID
    global DICOMListSelectSeriesUID
    
    set nameidx [$idsnames index active]
    set name [lindex $DICOMPatientIDsNames $nameidx]
    set studyididx [$study curselection]
    set studyid [lindex $DICOMStudyList $studyididx]
    set DICOMListSelectStudyUID $studyid
    CreateSeriesList $name $studyid
    $series delete 0 end
    eval {$series insert end} $DICOMSeriesList
    $filenames delete 0 end
    set DICOMListSelectSeriesUID "none selected"
}

proc ClickListSeriesUIDs { idsnames study series filenames } {
    global DICOMPatientIDsNames
    global DICOMStudyList
    global DICOMSeriesList
    global DICOMFileNameList
    global DICOMListSelectSeriesUID
    
    set nameidx [$idsnames index active]
    set name [lindex $DICOMPatientIDsNames $nameidx]
    set studyididx [$study index active]
    set studyid [lindex $DICOMStudyList $studyididx]
    set seriesididx [$series curselection]
    
    if {$seriesididx == ""} {
        return
    }
    
    set seriesid [lindex $DICOMSeriesList $seriesididx]
    set DICOMListSelectSeriesUID $seriesid
    CreateFileNameList $name $studyid $seriesid
    $filenames delete 0 end
    eval {$filenames insert end} $DICOMFileNameList
    $filenames selection set 0 end
}

proc DICOMListSelectClose { parent filelist } {
    global DICOMFileNameList
    
    set DICOMFileNameList {}
    set indices [$filelist curselection]
    foreach idx $indices {
        lappend DICOMFileNameList [$filelist get $idx]
    }
    
    destroy $parent
}

proc DICOMListSelect { parent values } {
    global DICOMListSelectPatientName
    global DICOMListSelectStudyUID
    global DICOMListSelectSeriesUID
    global DICOMListSelectFiles
    global Gui

    set DICOMListSelectFiles ""

    toplevel $parent -bg $Gui(activeWorkspace)
    wm title $parent "List of DICOM studies"
    wm minsize $parent 600 300
    frame $parent.f1 -bg $Gui(activeWorkspace)
    frame $parent.f2 -bg $Gui(activeWorkspace)
    frame $parent.f3 -bg $Gui(activeWorkspace)
    set iDsNames [DICOMScrolledListbox $parent.f1.iDsNames 0 1 DICOMListSelectPatientName "Patient <ID><Name>" -width 50 -height 5]
    set studyUIDs [DICOMScrolledListbox $parent.f1.studyUIDs 0 1 DICOMListSelectStudyUID "Study UID" -width 50 -height 5]
    set seriesUIDs [DICOMScrolledListbox $parent.f2.seriesUIDs 0 1 DICOMListSelectSeriesUID "Series UID" -width 50 -height 5]
    set fileNames [DICOMScrolledListbox $parent.f2.fileNames 0 1 DICOMListSelectFiles "Files" -width 50 -height 5 -selectmode extended]
    #set fileNames [DICOMScrolledListbox $parent.f2.fileNames 0 1 -width 60 -height 5 -selectmode multiple]
    
    #button $parent.f3.close -text "Close" -command "destroy $parent"
    eval {button $parent.f3.close -text "Close" -command [list DICOMListSelectClose $parent $fileNames]} $Gui(WBA)
    
    pack $parent.f1.iDsNames $parent.f1.studyUIDs -side left -expand true -fill both
    pack $parent.f2.seriesUIDs $parent.f2.fileNames -side left -expand true -fill both
    pack $parent.f1 -fill both -expand true
    pack $parent.f2 -fill both -expand true
    pack $parent.f3.close -pady 10
    pack $parent.f3 -fill both -expand true
    #pack $parent
    
    bind $iDsNames <ButtonRelease-1> [list ClickListIDsNames %W $studyUIDs $seriesUIDs $fileNames]
    bind $studyUIDs <ButtonRelease-1> [list ClickListStudyUIDs $iDsNames %W $seriesUIDs $fileNames]
    bind $seriesUIDs <ButtonRelease-1> [list ClickListSeriesUIDs $iDsNames $studyUIDs %W $fileNames]
    
    foreach x $values {
        $iDsNames insert end $x
    }
    
    $iDsNames selection set 0
    ClickListIDsNames $iDsNames $studyUIDs $seriesUIDs $fileNames
    $studyUIDs selection set 0
    ClickListStudyUIDs $iDsNames $studyUIDs $seriesUIDs $fileNames
    $seriesUIDs selection set 0
    ClickListSeriesUIDs $iDsNames $studyUIDs $seriesUIDs $fileNames
}

#
#
#

proc ChangeDir { dirlist } {
    global DICOMStartDir
    
    catch {cd $DICOMStartDir}
    set DICOMStartDir [pwd]
    
    $dirlist delete 0 end
    $dirlist insert end "../"
    foreach match [glob -nocomplain *] {
        if {[file isdirectory $match]} {
            $dirlist insert end $match/
        } else  {
            $dirlist insert end $match
        }
        
    }
}

proc ClickDirList { dirlist } {
    global DICOMStartDir
    set diridx [$dirlist curselection]
    if  { $diridx != "" } {
        set dir [$dirlist get $diridx]
        set DICOMStartDir [file join $DICOMStartDir $dir]
        ChangeDir $dirlist
    }
}

proc DICOMHelp { parent msg {textparams {}}} {
    global Gui

    toplevel $parent
    wm title $parent "DICOM Help"
    #wm minsize $parent 600 300

    frame $parent.f
    frame $parent.b
    pack $parent.f $parent.b -side top -fill both -expand true
    set t [eval {text $parent.f.t -setgrid true -wrap word -yscrollcommand "$parent.f.sy set"} $textparams]
    scrollbar $parent.f.sy -orient vert -command "$parent.f.t yview"
    pack $parent.f.sy -side right -fill y
    pack $parent.f.t -side left -fill both -expand true
    $parent.f.t insert end $msg

    eval { button $parent.b.close -text "Close" -command "destroy $parent" } $Gui(WBA)
    pack $parent.b.close -padx 10 -pady 10
}

proc DICOMSelectDirHelp {} {
    set msg "Select the start directory of DICOM studies either clicking \
the directory names in the listbox or typing the exact name and pressing Enter (or \
clicking the 'Change To' button).
Clicking ordinary files has no effect.
After pressing 'OK', the whole subdirectory will be traversed recursively and all DICOM \
files will be collected into a list."

    DICOMHelp .help $msg [list -width 60 -height 14]
    
    focus .help
    grab .help
    tkwait window .help
#    tk_messageBox -type ok -message "Help" -title "Title" -icon  info
}

proc DICOMSelectDir { top } {
    global DICOMStartDir
    global Pressed
    global Gui
    
    toplevel $top -bg $Gui(activeWorkspace)
    wm minsize $top 100 100
    wm title $top "Select Start Directory"

    set f1 [frame $top.f1 -bg $Gui(activeWorkspace)]
    set f2 [frame $top.f2 -bg $Gui(activeWorkspace)]
    set f3 [frame $top.f3 -bg $Gui(activeWorkspace)]
    
    set dirlist [ScrolledListbox $f2.dirlist 1  1 -width 30 -height 15]
    
    eval { button $f1.changeto -text "Change To:" -command  [list ChangeDir $dirlist]} $Gui(WBA)
    eval { entry $f1.dirname -textvariable DICOMStartDir } $Gui(WEA)
    
    eval {button $f3.ok -text "OK" -command "set Pressed OK; destroy $top"} $Gui(WBA)
    eval {button $f3.cancel -text "Cancel" -command "set Pressed Cancel; destroy $top"} $Gui(WBA)
    eval {button $f3.help -text "Help" -command "DICOMSelectDirHelp"} $Gui(WBA)

    pack $f1.changeto $f1.dirname -side left -padx 10 -pady 10
    pack $f2.dirlist -fill both -expand true
    pack $f3.ok $f3.cancel $f3.help -side left -padx 10 -pady 10
    pack $f1
    pack $f2 -fill both -expand true
    pack $f3
    #pack $window
    
    set pwd [pwd]
    catch {cd $DICOMStartDir}
    set DICOMStartDir [pwd]
    
    ChangeDir $dirlist
    #$dirlist delete 0 end
    #$dirlist insert end ".."
    #foreach match [glob -nocomplain *] {
    #    if {[file isdirectory $match]} {
    #        $dirlist insert end $match
    #        #puts $dir
    #    }
    #}
    
    bind $dirlist <ButtonRelease-1> [list ClickDirList %W]
    bind $f1.dirname <KeyRelease-Return> [list $f1.changeto invoke]
    
    #cd $pwd
}

proc DICOMSelectMain { fileNameListbox } {
    global DICOMStartDir
    global Pressed
    global DICOMPatientIDsNames
    global DICOMFileNameList
    global Volume
    
    if {$Volume(activeID) != "NEW"} {
	return
    }

    set pwd [pwd]
    #set DICOMStartDir "e:/tanacs/medpic/"
    set DICOMStartDir $Volume(DICOMStartDir)
    set DICOMFileNameList {}
    set Volume(dICOMFileList) {}
    DICOMSelectDir .select
    
    focus .select
    grab .select
    tkwait window .select
    
    if { $Pressed == "OK" } {
        FindDICOM $DICOMStartDir *
        
        DICOMListSelect .list $DICOMPatientIDsNames
        
        focus .list
        grab .list
        tkwait window .list
        
        #puts $DICOMFileNameList
        $fileNameListbox delete 0 end
        foreach name $DICOMFileNameList {
            $fileNameListbox insert end $name
        }
        set Volume(dICOMFileList) $DICOMFileNameList

	# use the second and the third
	set file1 [lindex $DICOMFileNameList 1]
	set file2 [lindex $DICOMFileNameList 2]
	DICOMReadHeaderValues [lindex $DICOMFileNameList 0]
	DICOMPredictScanOrder $file1 $file2

	set Volume(DICOMStartDir) $DICOMStartDir
    }
    
    cd $pwd
}

proc HandleExtractHeader {} {
    global Volume

    if {$Volume(activeID) != "NEW"} {
	return
    }

    set fileidx [$Volume(dICOMFileListbox) index active]
    set filename [lindex $Volume(dICOMFileList) $fileidx]
    DICOMReadHeaderValues $filename

    set Volume(propertyType) Header
    VolumesSetPropertyType
}

proc DICOMReadHeaderValues { filename } {
    global Volume

    if {$filename == ""} {
	return
    }
    
    vtkDCMParser parser
    set found [parser OpenFile $filename]
    if {[string compare $found "0"] == 0} {
	puts stderr "Can't open file $filename\n"
	parser Delete
	return
    } else {
	if { [parser FindElement 0x0010 0x0010] == "1" } {
	    set Length [lindex [split [parser ReadElement]] 3]
	    set PatientName [parser ReadText $Length]
	    if {$PatientName == ""} {
		set PatientName "noname"
	    }
	} else  {
	    set PatientName 'unknown'
	}
	#regsub -all {\ |\t|\n} $PatientName "_" Volume(name)
	regsub -all {[^a-zA-Z0-9]} $PatientName "_" Volume(name)
	
	if { [parser FindElement 0x0028 0x0010] == "1" } {
	    #set Length [lindex [split [parser ReadElement]] 3]
	    parser ReadElement
	    set Volume(resolution) [parser ReadUINT16]
	} else  {
	    set Volume(resolution) "unknown"
	}

	if { [parser FindElement 0x0028 0x0030] == "1" } {
	    set NextBlock [lindex [split [parser ReadElement]] 4]
	    set Volume(pixelSize) [parser ReadFloatAsciiNumeric $NextBlock]
	} else  {
	    set Volume(pixelSize) "unknown"
	}

	if { [parser FindElement 0x0018 0x0050] == "1" } {
	    set NextBlock [lindex [split [parser ReadElement]] 4]
	    set Volume(sliceThickness) [parser ReadFloatAsciiNumeric $NextBlock]
	} else  {
	    set Volume(sliceThickness) "unknown"
	}

	if { [parser FindElement 0x0018 0x1120] == "1" } {
	    set NextBlock [lindex [split [parser ReadElement]] 4]
	    set Volume(gantryDetectorTilt) [parser ReadFloatAsciiNumeric $NextBlock]
	} else  {
	    set Volume(gantryDetectorTilt) 0
	}

	if { [parser FindElement 0x0008 0x0060] == "1" } {
	    set Length [lindex [split [parser ReadElement]] 3]
	    set Volume(desc) [parser ReadText $Length]
	} else  {
	    set Volume(desc) "unknown modality"
	}

	if { [parser GetTransferSyntax] == "3"} {
	    set Volume(littleEndian) 0
	} else {
	    set Volume(littleEndian) 1
	}

	# Number of Scalars and ScalarType 

	if { [parser FindElement 0x0028 0x0002] == "1" } {
	    parser ReadElement
	    set Volume(numScalars) [parser ReadUINT16]
	} else  {
	    set Volume(numScalars) "unknown"
	}
	
	if { [parser FindElement 0x0028 0x0103] == "1" } {
	    parser ReadElement
	    set PixelRepresentation [parser ReadUINT16]
	} else  {
	    set PixelRepresentation 1
	}
	
	if { [parser FindElement 0x0028 0x0100] == "1" } {
	    parser ReadElement
	    set BitsAllocated [parser ReadUINT16]
	    if { $BitsAllocated == "16" } {
		if { $PixelRepresentation == "0" } {
		    #VolumesSetScalarType "UnsignedShort"
		    VolumesSetScalarType "Short"
		} else {
		    VolumesSetScalarType "Short"
		}
	    }

	    if { $BitsAllocated == "8" } {
		if { $PixelRepresentation == "0" } {
		    VolumesSetScalarType "UnsignedChar"
		} else {
		    VolumesSetScalarType "Char"
		}
	    }
	} else  {
	    # do nothing
	}

    }

    parser Delete

    set Volume(readHeaders) 0
}

proc DICOMPredictScanOrder { file1 file2 } {
    global Volume

    if { ($file1 == "") || ($file2 == "") } {
	return
    }

    vtkDCMParser parser

    set found [parser OpenFile $file1]
    if {[string compare $found "0"] == 0} {
	puts stderr "Can't open file $file1\n"
	parser Delete
	return
    } else {
	if { [parser FindElement 0x0020 0x0032] == "1" } {
	    set NextBlock [lindex [split [parser ReadElement]] 4]
	    set x1 [parser ReadFloatAsciiNumeric $NextBlock]
	    set y1 [parser ReadFloatAsciiNumeric $NextBlock]
	    set z1 [parser ReadFloatAsciiNumeric $NextBlock]
	} else  {
	    parser Delete
	    return
	}
    }
    parser CloseFile

    set found [parser OpenFile $file2]
    if {[string compare $found "0"] == 0} {
	puts stderr "Can't open file $file2\n"
	parser Delete
	return
    } else {
	if { [parser FindElement 0x0020 0x0032] == "1" } {
	    set NextBlock [lindex [split [parser ReadElement]] 4]
	    set x2 [parser ReadFloatAsciiNumeric $NextBlock]
	    set y2 [parser ReadFloatAsciiNumeric $NextBlock]
	    set z2 [parser ReadFloatAsciiNumeric $NextBlock]
	} else  {
	    parser Delete
	    return
	}
    }

    #set Volume(filePattern) [format "%.2f %.2f %.2f" $x1 $y1 $z1]

    set dx [expr $x2 - $x1]
    set dy [expr $y2 - $y1]
    set dz [expr $z2 - $z1]

    if { abs($dx) > abs($dy) } {
	if { abs($dx) > abs($dz) } {
	    # sagittal
	    if { $dx > 0 } {
		VolumesSetScanOrder "LR"
	    } else {
		VolumesSetScanOrder "RL"
	    }
	} else {
	    if { $dx > 0 } {
		# axial SI
		VolumesSetScanOrder "SI"
	    } else {
		# axial IS
		VolumesSetScanOrder "IS"
	    }
	}
    } else {
	if { abs ($dy) > abs($dz) } {
	    # coronal
	    if { $dy > 0 } {
		VolumesSetScanOrder "AP"
	    } else {
		VolumesSetScanOrder "PA"
	    }
	} else {
	    if { $dx > 0 } {
		# axial SI
		VolumesSetScanOrder "SI"
	    } else {
		# axial IS
		VolumesSetScanOrder "IS"
	    }
	}
    }

    parser CloseFile
    parser Delete
}

proc VolumesEnter {} {
    DataExit
    bind Listbox <Control-Button-1> {tkListboxBeginToggle %W [%W index @%x,%y]}
    #tk_messageBox -type ok -message "VolumesEnter" -title "Title" -icon  info
}

proc VolumesExit {} {
    #tk_messageBox -type ok -message "VolumesExit" -title "Title" -icon  info
}
