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
# FILE:        Volumes.tcl
# PROCEDURES:  
#   VolumesInit
#   VolumesBuildVTK
#   VolumesBuildGUI
#   VolumesCheckForManualChanges n
#   VolumesManualSetPropertyType n
#   VolumesAutomaticSetPropertyType n
#   VolumesSetPropertyType
#   VolumesPropsApply
#   VolumesPropsApply
#   VolumesPropsCancel
#   VolumesSetFirst
#   VolumesSetScanOrder
#   VolumesSetScalarType
#   VolumesSetLast
#   AddListUnique
#   DICOMFileNameTextBoxVisibleButton
#   DICOMPreviewImageClick
#   DICOMFillFileNameTextbox
#   DICOMIncrDecrButton
#   DICOMScrolledTextbox
#   FindDICOM2
#   FindDICOM
#   CreateStudyList
#   CreateSeriesList
#   CreateFileNameList
#   ClickListIDsNames
#   ClickListStudyUIDs
#   ClickListSeriesUIDs
#   DICOMListSelectClose
#   DICOMListSelect
#   ChangeDir
#   ClickDirList
#   DICOMHelp
#   DICOMSelectDirHelp
#   DICOMSelectDir
#   DICOMSelectMain
#   HandleExtractHeader
#   DICOMReadHeaderValues
#   DICOMPredictScanOrder
#   DICOMPreviewAllButton
#   DICOMListHeadersButton
#   DICOMListHeader
#   DICOMPreviewFile
#   DICOMCheckFiles
#   DICOMCheckVolumeInit
#   DICOMCheckFile
#   DICOMShowPreviewSettings
#   DICOMHidePreviewSettings
#   DICOMShowDataDictSettings
#   DICOMHideDataDictSettings
#   DICOMHideAllSettings
#   DICOMSelectFragment
#   DICOMImageTextboxFragmentEnter
#   DICOMImageTextboxFragmentLeave
#   DICOMImageTextboxSelectAll
#   DICOMImageTextboxDeselectAll
#   VolumesEnter
#   VolumesExit
#   VolumesStorePresets
#   VolumesRecallPresets
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC VolumesInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesInit {} {
	global Volumes Volume Module Gui Path prog

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
        set Module($m,procVTK) VolumesBuildVTK
        #set Volumes(DICOMStartDir) "c:/tanacs/medpic/"
        set Volumes(DICOMStartDir) $prog
	set Volumes(FileNameSortParam) "incr"
	set Volumes(prevIncrDecrState) "incr"
	set Volumes(previewCount) 0

	set Volumes(DICOMPreviewWidth) 64
	set Volumes(DICOMPreviewHeight) 64
	set Volumes(DICOMPreviewHighestValue) 2048

    set Volumes(DICOMCheckVolumeList) {}
    set Volumes(DICOMCheckPositionList) {}
    set Volumes(DICOMCheckActiveList) {}
    set Volumes(DICOMCheckActivePositionList) {}
    set Volumes(DICOMCheckSliceDistanceList) {}

    set Volumes(DICOMCheckImageLabelIdx) 0
    set Volumes(DICOMCheckLastPosition) 0
    set Volumes(DICOMCheckSliceDistance) 0

    set dir [file join [file join $prog tcl-modules] Volumes]
    set Volumes(DICOMDataDictFile) $dir/datadict.txt

    set Volumes(eventManager) {}

    lappend Module(procStorePresets) VolumesStorePresets
    lappend Module(procRecallPresets) VolumesRecallPresets
    set Module(Volumes,presets) "DICOMStartDir='$prog' FileNameSortParam='incr' \
DICOMPreviewWidth='64' DICOMPreviewHeight='64' DICOMPreviewHighestValue='2048' \
DICOMDataDictFile='$Volumes(DICOMDataDictFile)'"

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
                {$Revision: 1.53 $} {$Date: 2001/11/29 21:07:00 $}]

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
# .PROC VolumesBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesBuildVTK {} {

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
			-indicatoron 0 } $Gui(WCA)
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
	foreach param "filePattern width height \
		pixelSize sliceThickness sliceSpacing" \
		name "{File Pattern} \
		{Width in pixels} {Height in pixels} {Pixel Size (mm)} \
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
		eval {radiobutton $f.f.r$value -width $width \
                        -indicatoron 0 -text $text -value $value \
                        -variable Volume(littleEndian) } $Gui(WCA)
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
    
    DevAddButton $f.bApply "Header" "set Volume(propertyType) Header; VolumesSetPropertyType" 8
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
# .PROC VolumesCheckForManualChanges
# 
# This Procedure is called to see if any important properties
# were changed that might require re-reading the volume.
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesCheckForManualChanges {n} {
    global Lut Volume Label Module Mrml

    if { !$Volume(isDICOM)} {
        if {[$n GetFilePrefix] != [file root $Volume(firstFile)] } { 
            return 1 
        }
    set firstNum [MainFileFindImageNumber First [file join $Mrml(dir) $Volume(firstFile)]]
        if {[lindex [$n GetImageRange] 0 ]  != $firstNum }  { return 1 }
    
        if {[lindex [$n GetImageRange] 1 ]  != $Volume(lastNum) } { return 1 }
        if {[$n GetLabelMap] != $Volume(labelMap)} { return 1 }
        if {[$n GetFilePattern] != $Volume(filePattern) } { return 1 }
        if {[lindex [$n GetDimensions] 1 ]  != $Volume(height) } { return 1 }
        if {[lindex [$n GetDimensions] 0 ]  != $Volume(width) } { return 1 }
        if {[$n GetScanOrder] != $Volume(scanOrder)} { return 1 }
        if {[$n GetScalarTypeAsString] != $Volume(scalarType)} { return 1 }
        if {[$n GetNumScalars] != $Volume(numScalars)} { return 1 }
        if {[$n GetLittleEndian] != $Volume(littleEndian)} { return 1 }
    }
    return 0
}

#-------------------------------------------------------------------------------
# .PROC VolumesManualSetPropertyType
# 
# This procedure is called when manually setting the properties
# to read in a volume
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesManualSetPropertyType {n} {
    global Lut Volume Label Module Mrml

    # These get set down below, but we need them before MainUpdateMRML
    $n SetFilePrefix [file root $Volume(firstFile)]
    $n SetFilePattern $Volume(filePattern)
    $n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
    if { !$Volume(isDICOM) } {
        set firstNum [MainFileFindImageNumber First [file join $Mrml(dir) $Volume(firstFile)]]
    } else {
	set firstNum 1
    }
    $n SetImageRange $firstNum $Volume(lastNum)
    $n SetDimensions $Volume(width) $Volume(height)
    eval $n SetSpacing $Volume(pixelSize) $Volume(pixelSize) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    $n SetScalarTypeTo$Volume(scalarType)
    $n SetNumScalars $Volume(numScalars)
    $n SetLittleEndian $Volume(littleEndian)
    $n SetTilt $Volume(gantryDetectorTilt)
    $n ComputeRasToIjkFromScanOrder $Volume(scanOrder)
}


#-------------------------------------------------------------------------------
# .PROC VolumesAutomaticSetPropertyType
# 
# This procedure is called when reading the header of a volume
# to get the header information. Returns 1 on success
#
# .ARGS
#  vtkMrmlVolumeNode n is the vtkMrmlVolumeNode to edit.
# .END
#-------------------------------------------------------------------------------
proc VolumesAutomaticSetPropertyType {n} {
    global Lut Volume Label Module Mrml

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
        return 0
    } elseif {$errmsg != ""} {
        # File not found, most likely
        puts $errmsg
        tk_messageBox -message $errmsg
        # Remove node
        MainMrmlUndoAddNode Volume $n
        return 0
    }
   return 1
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
    focus $Volume(f$Volume(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC VolumesPropsApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC VolumesPropsApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
    if {[ValidateInt $Volume(width)] == 0} {
        tk_messageBox -message "The width must be an integer."
        return
    }
    if {[ValidateInt $Volume(height)] == 0} {
        tk_messageBox -message "The height must be an integer."
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

        # End of Part added by Attila Tanacs

        # Manual headers

        if {$Volume(readHeaders) == "0"} {
            # These setting are set down below, 
            # but we need them before MainUpdateMRML
            
            VolumesManualSetPropertyType $n
        } else {
            # Read headers
            if {[VolumesAutomaticSetPropertyType $n] == 0} {
                return
            }
        }
        # end Read Headers

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
    } else {
        # End   if the Volume is NEW
        ## Maybe we would like to do a reread of the file?
        if {$m != $Volume(idNone) } {
            if {[VolumesCheckForManualChanges Volume($m,node)] == 1} {
                set ReRead [ DevYesNo "Reread the Image?" ]
                puts "ReRead"
                if {$ReRead == "yes"} {
                    set Volume(readHeaders) 0
                    if {$Volume(readHeaders) == "0"} {
                        # These setting are set down below, 
                        # but we need them before MainUpdateMRML

                        VolumesManualSetPropertyType  Volume($m,node)
                    } else {
                        # Read headers
                if {[VolumesAutomaticSetPropertyType Volume($m,node)] == 0} {
                            return
                        }
                    }
                    if {[MainVolumesRead $m]<0 } {
                        DevErrorWindow "Error reading volume!"
                        return 0
                    }
                }
                # end if they chose to reread
            }
            # end if they should be asked to reread
        }
        # end if the volume id is not none.
    }
    # End thinking about rereading.


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
        set Volume(DefaultDir) [file dirname [file join $Mrml(dir) $Volume(firstFile)]]
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

#-------------------------------------------------------------------------------
# .PROC AddListUnique
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AddListUnique { list arg } {
    upvar $list list2
    if { [expr [lsearch -exact $list2 $arg] == -1] } {
        lappend list2 $arg
    }
}

#-------------------------------------------------------------------------------
# .PROC DICOMFileNameTextBoxVisibleButton
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMFileNameTextBoxVisibleButton {t idx value} {
    global DICOMFileNameSelected

    #tk_messageBox -message "ButtonPress $idx"
    set DICOMFileNameSelected [lreplace $DICOMFileNameSelected $idx $idx $value]
    DICOMFillFileNameTextbox $t
    #$t see $idx.0
}

#-------------------------------------------------------------------------------
# .PROC DICOMPreviewImageClick
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMPreviewImageClick {w idx} {
    global Volumes DICOMFileNameSelected

    set v [lindex $DICOMFileNameSelected $idx]
    if {$v == "0"} {
	$w configure -background green
	DICOMFileNameTextBoxVisibleButton $Volumes(DICOMFileNameTextbox) $idx "1"
    } else {
	$w configure -background red
	DICOMFileNameTextBoxVisibleButton $Volumes(DICOMFileNameTextbox) $idx "0"
    }
}

#-------------------------------------------------------------------------------
# .PROC DICOMFillFileNameTextbox
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMFillFileNameTextbox {t} {
    global DICOMFileNameList DICOMFileNameSelected

    $t configure -state normal
    set yviewfr [lindex [$t yview] 0]
    $t delete 1.0 end

    $t insert insert " "
    $t insert insert "  Select All  " selectall
    $t insert insert "  "
    $t insert insert "  Deselect All  " deselectall
    $t insert insert "\n"
    $t tag add menu 1.0 1.end

    $t tag config selectall -background gray -relief groove -borderwidth 2 -font {helvetica 10 bold}
    $t tag bind selectall <ButtonRelease-1> "DICOMImageTextboxSelectAll"
    $t tag config deselectall -background gray -relief groove -borderwidth 2 -font {helvetica 10 bold}
    $t tag bind deselectall <ButtonRelease-1> "DICOMImageTextboxDeselectAll"
    $t tag config menu -justify center
        
    set num [llength $DICOMFileNameList]
    for {set idx 0} {$idx < $num} {incr idx} {
	set firstpos [$t index insert]
	$t insert insert "\#[expr $idx + 1] "
	if {[lindex $DICOMFileNameSelected $idx] == "1"} {
	    $t insert insert " S " vis$idx
	    $t tag config vis$idx -background green -relief groove -borderwidth 2
	    #$t tag bind vis$idx <Button-1> "DICOMFileNameTextBoxVisibleButton $t $idx 0"
	    set value 0
	} else {
	    $t insert insert " N " vis$idx
	    $t tag config vis$idx -background red -relief groove -borderwidth 2
	    #$t tag bind vis$idx <Button-1> "DICOMFileNameTextBoxVisibleButton $t $idx 1"
	    set value 1
	}
	$t insert insert " "
	$t insert insert [lindex $DICOMFileNameList $idx]
	$t insert insert "\n"
	$t tag add line$idx $firstpos [$t index insert]
	$t tag bind line$idx <Button-1> "DICOMFileNameTextBoxVisibleButton $t $idx $value"
    }
    $t yview moveto $yviewfr
    $t configure -state disabled
}

#-------------------------------------------------------------------------------
# .PROC DICOMIncrDecrButton
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMIncrDecrButton {filenames} {
    global Volumes DICOMFileNameList DICOMFileNameSelected

    if {$Volumes(prevIncrDecrState) != $Volumes(FileNameSortParam)} {
	set temp1 {}
	set temp2 {}
	set num [llength $DICOMFileNameList]
	for {set i [expr $num - 1]} {$i >= 0} {incr i -1} {
	    lappend temp1 [lindex $DICOMFileNameList $i]
	    lappend temp2 [lindex $DICOMFileNameSelected $i]
	}
	set DICOMFileNameList $temp1
	set DICOMFileNameSelected $temp2
	DICOMFillFileNameTextbox $filenames
	set Volumes(prevIncrDecrState) $Volumes(FileNameSortParam)
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

#-------------------------------------------------------------------------------
# .PROC DICOMScrolledTextbox
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMScrolledTextbox {f xAlways yAlways variable {labeltext "labeltext"} {args ""}} {
	global Gui
	
    set fmain $f
    frame $fmain -bg $Gui(activeWorkspace)
    eval { label $fmain.head -text $labeltext } $Gui(WLA)
    eval { label $fmain.selected -textvariable $variable } $Gui(WLA)

    frame $fmain.f -bg $Gui(activeWorkspace)
    set f $fmain.f
	if {$xAlways == 1 && $yAlways == 1} { 
		text $f.list \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand "$f.yscroll set"
	
	} elseif {$xAlways == 1 && $yAlways == 0} { 
		text $f.list \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand [list ScrollSet $f.yscroll \
				[list grid $f.yscroll -row 0 -column 1 -sticky ns]]

	} elseif {$xAlways == 0 && $yAlways == 1} { 
		text $f.list \
			-xscrollcommand [list ScrollSet $f.xscroll \
				[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
			-yscrollcommand "$f.yscroll set"

	} else {
		text $f.list \
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

#-------------------------------------------------------------------------------
# .PROC FindDICOM2
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
		if [expr [parser FindElement 0x0020 0x1041] == "1"] {
		    set NextBlock [lindex [split [parser ReadElement]] 4]
		    set ImageNumber [parser ReadFloatAsciiNumeric $NextBlock]
		} 
		if { $ImageNumber == "" } {
		    if [expr [parser FindElement 0x0020 0x0013] == "1"] {
			#set Length [lindex [split [parser ReadElement]] 3]
			#set ImageNumber [parser ReadText $Length]
			#scan [parser ReadText $length] "%d" ImageNumber
			
			set NextBlock [lindex [split [parser ReadElement]] 4]
			set ImageNumber [parser ReadIntAsciiNumeric $NextBlock]
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

#-------------------------------------------------------------------------------
# .PROC FindDICOM
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FindDICOM { StartDir Pattern } {
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMStudyInstanceUIDList
    global DICOMSeriesInstanceUIDList
    global DICOMFileNameArray
    global DICOMFileNameList DICOMFileNameSelected
    
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
    set DICOMFileNameSelected {}
    
    if [catch {cd $StartDir} err] {
        puts stderr $err
        cd $pwd
        return
    }
    FindDICOM2 $StartDir "" $Pattern
    cd $pwd
}

#-------------------------------------------------------------------------------
# .PROC CreateStudyList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC CreateSeriesList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC CreateFileNameList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CreateFileNameList { PatientIDName StudyUID SeriesUID} {
    global Volumes
    global DICOMFiles
    global FindDICOMCounter
    global DICOMPatientNames
    global DICOMPatientIDsNames
    global DICOMFileNameArray
    global DICOMFileNameList DICOMFileNameSelected
    
    catch {unset DICOMFileNameArray}
    set count 0
    for  {set i 0} {$i < $FindDICOMCounter} {incr i} {
        if {[string compare $DICOMFiles($i,PatientIDName) $PatientIDName] == 0} {
            if {[string compare $DICOMFiles($i,StudyInstanceUID) $StudyUID] == 0} {
                if {[string compare $DICOMFiles($i,SeriesInstanceUID) $SeriesUID] == 0} {
                    #set id [format "%04d_%04d" $DICOMFiles($i,ImageNumber) $count]
		    #set id [format "%010.4f_%04d" $DICOMFiles($i,ImageNumber) $count]
		    set id [format "%012.4f_%04d" [expr 10000.0 + $DICOMFiles($i,ImageNumber)] $count]
                    incr count
                    set DICOMFileNameArray($id) $DICOMFiles($i,FileName)
                }
            }
        }
    }
    #set idx [lsort -decreasing [array name DICOMFileNameArray]]
    #set idx [lsort [array name DICOMFileNameArray]]
    if {$Volumes(FileNameSortParam) == "incr"} {
	set idx [lsort -increasing [array name DICOMFileNameArray]]
    } else {
	set idx [lsort -decreasing [array name DICOMFileNameArray]]
    }
    set DICOMFileNameList {}
    set DICOMFileNameSelected {}
    foreach i $idx {
        lappend DICOMFileNameList $DICOMFileNameArray($i)
	lappend DICOMFileNameSelected "1"
    }
}

#-------------------------------------------------------------------------------
# .PROC ClickListIDsNames
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
    #$filenames delete 0 end
    $filenames delete 1.0 end
    set DICOMListSelectStudyUID "none selected"
    set DICOMListSelectSeriesUID "none selected"
}

#-------------------------------------------------------------------------------
# .PROC ClickListStudyUIDs
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
    #$filenames delete 0 end
    $filenames delete 1.0 end
    set DICOMListSelectSeriesUID "none selected"
}

#-------------------------------------------------------------------------------
# .PROC ClickListSeriesUIDs
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
    DICOMFillFileNameTextbox $filenames
}

#-------------------------------------------------------------------------------
# .PROC DICOMListSelectClose
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMListSelectClose { parent filelist } {
    global DICOMFileNameList DICOMFileNameSelected
    global Pressed
    
#     set list2 $DICOMFileNameList
#     set DICOMFileNameList {}
#     set num [llength $DICOMFileNameSelected]
#     for {set i 0} {$i < $num} {incr i} {
# 	if {[lindex $DICOMFileNameSelected $i] == "1"} {
# 	    lappend DICOMFileNameList [lindex $list2 $i]
# 	}
#     }
    
    set Pressed OK
    destroy $parent
}

#-------------------------------------------------------------------------------
# .PROC DICOMListSelect
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMListSelect { parent values } {
#
# It's a bit messy, but it was my very first Tcl/Tk program.
#
    global DICOMListSelectPatientName
    global DICOMListSelectStudyUID
    global DICOMListSelectSeriesUID
    global DICOMListSelectFiles
    global Gui
    global Pressed
    global Volumes

    set DICOMListSelectFiles ""

    toplevel $parent -bg $Gui(activeWorkspace)
    wm title $parent "List of DICOM studies"
    wm minsize $parent 640 480

    frame $parent.f1 -bg $Gui(activeWorkspace)
    frame $parent.f2 -bg $Gui(activeWorkspace)
    frame $parent.f3 -bg $Gui(activeWorkspace)
    pack $parent.f1 $parent.f2 -fill x
    pack $parent.f3

    set iDsNames [DICOMScrolledListbox $parent.f1.iDsNames 0 1 DICOMListSelectPatientName "Patient <ID><Name>" -width 50 -height 5]
    TooltipAdd $iDsNames "Select a patient"
    set studyUIDs [DICOMScrolledListbox $parent.f1.studyUIDs 0 1 DICOMListSelectStudyUID "Study UID" -width 50 -height 5]
    TooltipAdd $studyUIDs "Select a study of the selected patient"
    pack $parent.f1.iDsNames $parent.f1.studyUIDs -side left -expand true -fill both

    set seriesUIDs [DICOMScrolledListbox $parent.f2.seriesUIDs 0 1 DICOMListSelectSeriesUID "Series UID" -width 50 -height 5]
    TooltipAdd $seriesUIDs "Select a series of the selected study"
    set fileNames [DICOMScrolledTextbox $parent.f2.fileNames 0 1 DICOMListSelectFiles "Files" -width 50 -height 5 -wrap none -cursor hand1 -state disabled]
    set Volumes(DICOMFileNameTextbox) $fileNames
    TooltipAdd $fileNames "Select files of the selected series"
    pack $parent.f2.seriesUIDs $parent.f2.fileNames -side left -expand true -fill both

    eval {button $parent.f3.close -text "OK" -command [list DICOMListSelectClose $parent $fileNames]} $Gui(WBA)
    eval {button $parent.f3.cancel -text "Cancel" -command "set Pressed Cancel; destroy $parent"} $Gui(WBA)
    pack $parent.f3.close $parent.f3.cancel -padx 10 -pady 10 -side left

    frame $parent.f2.fileNames.fIncrDecr -bg $Gui(activeWorkspace)
    pack $parent.f2.fileNames.fIncrDecr

    set f2 $parent.f2.fileNames.fIncrDecr
    eval {radiobutton $f2.rIncr \
	      -text "Increasing" -command "DICOMIncrDecrButton $fileNames" \
	      -variable Volumes(FileNameSortParam) -value "incr" -width 12 \
	      -indicatoron 0} $Gui(WCA)
    eval {radiobutton $f2.rDecr \
	      -text "Decreasing" -command "DICOMIncrDecrButton $fileNames" \
	      -variable Volumes(FileNameSortParam) -value "decr" -width 12 \
	      -indicatoron 0} $Gui(WCA)
    eval {button $f2.bPreviewAll -text "Preview" -command "DICOMPreviewAllButton"} $Gui(WBA)
    eval {button $f2.bListHeaders -text "List Headers" -command "DICOMListHeadersButton"} $Gui(WBA)
    eval {button $f2.bCheck -text "Check" -command "DICOMCheckFiles"} $Gui(WBA)

    pack $parent.f2.fileNames.fIncrDecr.rIncr $parent.f2.fileNames.fIncrDecr.rDecr $parent.f2.fileNames.fIncrDecr.bPreviewAll $parent.f2.fileNames.fIncrDecr.bListHeaders $parent.f2.fileNames.fIncrDecr.bCheck -side left -pady 2 -padx 0

    # ImageTextbox frame
    set Volumes(ImageTextbox) [ScrolledText $parent.f4]
    $Volumes(ImageTextbox) configure -height 8 -width 10
    pack $parent.f4 -fill both -expand true

    # DICOM Preview Settings Frame
    frame $parent.f4.fSettings -bg $Gui(activeWorkspace) -relief sunken -bd 2
    place $parent.f4.fSettings -relx 0.8 -rely 0.0 -anchor ne
    set f $parent.f4.fSettings
    set Volumes(DICOMPreviewSettingsFrame) $f

    frame $f.f1 -bg $Gui(activeWorkspace)
    pack $f.f1
    eval {label $f.f1.lWidth -text "Preview Width:" -width 15} $Gui(WLA)
    eval {entry $f.f1.ePreviewWidth -width 6 \
	      -textvariable Volumes(DICOMPreviewWidth)} $Gui(WEA)
    pack $f.f1.lWidth $f.f1.ePreviewWidth -side left -padx 5 -pady 2

    frame $f.f2 -bg $Gui(activeWorkspace)
    pack $f.f2
    eval {label $f.f2.lHeight -text "Preview Height:" -width 15} $Gui(WLA)
    eval {entry $f.f2.ePreviewHeight -width 6 \
	      -textvariable Volumes(DICOMPreviewHeight)} $Gui(WEA)
    pack $f.f2.lHeight $f.f2.ePreviewHeight -side left -padx 5 -pady 2

    frame $f.f3 -bg $Gui(activeWorkspace)
    pack $f.f3
    eval {label $f.f3.lHighest -text "Highest Value:" -width 15} $Gui(WLA)
    eval {entry $f.f3.eHighest -width 6 \
	      -textvariable Volumes(DICOMPreviewHighestValue)} $Gui(WEA)
    pack $f.f3.lHighest $f.f3.eHighest -side left -padx 5 -pady 2

    eval {button $parent.f4.fSettings.bPreviewAll -text "Preview" -command "DICOMPreviewAllButton"} $Gui(WBA)
    pack $parent.f4.fSettings.bPreviewAll -padx 2 -pady 2

    # DICOMDataDict
    frame $parent.f4.fDataDict -bg $Gui(activeWorkspace) -relief sunken -bd 2
    place $parent.f4.fDataDict -relx 0.9 -rely 0.0 -anchor ne
    set f $parent.f4.fDataDict
    set Volumes(DICOMDataDictSettingsFrame) $f
    DevAddFileBrowse $f Volumes DICOMDataDictFile "DICOM Data Dictionary File" ""

    # >> Bindings

    bind $parent.f2.fileNames.fIncrDecr.bPreviewAll <Enter> "DICOMShowPreviewSettings"
    bind $parent.f4.fSettings <Leave> "DICOMHidePreviewSettings"

    bind $parent.f2.fileNames.fIncrDecr.bListHeaders <Enter> "DICOMShowDataDictSettings"
    bind $parent.f4.fDataDict <Leave> "DICOMHideDataDictSettings"

    bind $iDsNames <ButtonRelease-1> [list ClickListIDsNames %W $studyUIDs $seriesUIDs $fileNames]
    #bind $iDsNames <Double-1> [list ClickListIDsNames %W $studyUIDs $seriesUIDs $fileNames]
    bind $studyUIDs <ButtonRelease-1> [list ClickListStudyUIDs $iDsNames %W $seriesUIDs $fileNames]
    bind $seriesUIDs <ButtonRelease-1> [list ClickListSeriesUIDs $iDsNames $studyUIDs %W $fileNames]

    # << Bindings
    
    foreach x $values {
        $iDsNames insert end $x
    }
    
    $iDsNames selection set 0
    ClickListIDsNames $iDsNames $studyUIDs $seriesUIDs $fileNames
    $studyUIDs selection set 0
    ClickListStudyUIDs $iDsNames $studyUIDs $seriesUIDs $fileNames
    $seriesUIDs selection set 0
    ClickListSeriesUIDs $iDsNames $studyUIDs $seriesUIDs $fileNames

    DICOMHideAllSettings
}

#
#
#

#-------------------------------------------------------------------------------
# .PROC ChangeDir
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC ClickDirList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ClickDirList { dirlist } {
    global DICOMStartDir
    set diridx [$dirlist curselection]
    if  { $diridx != "" } {
        set dir [$dirlist get $diridx]
        set DICOMStartDir [file join $DICOMStartDir $dir]
        ChangeDir $dirlist
    }
}

#-------------------------------------------------------------------------------
# .PROC DICOMHelp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMHelp { parent msg {textparams {}}} {
    global Gui

    toplevel $parent -bg $Gui(activeWorkspace)
    wm title $parent "DICOM Help"
    #wm minsize $parent 600 300

    frame $parent.f -bg $Gui(activeWorkspace)
    frame $parent.b -bg $Gui(activeWorkspace)
    pack $parent.f $parent.b -side top -fill both -expand true
    set t [eval {text $parent.f.t -setgrid true -wrap word -yscrollcommand "$parent.f.sy set"} $textparams]
    scrollbar $parent.f.sy -orient vert -command "$parent.f.t yview"
    pack $parent.f.sy -side right -fill y
    pack $parent.f.t -side left -fill both -expand true
    $parent.f.t insert end $msg

    eval { button $parent.b.close -text "Close" -command "destroy $parent" } $Gui(WBA)
    pack $parent.b.close -padx 10 -pady 10
}

#-------------------------------------------------------------------------------
# .PROC DICOMSelectDirHelp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC DICOMSelectDir
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
    TooltipAdd $dirlist "Select start directory for search"
    
    eval { button $f1.changeto -text "Change To:" -command  [list ChangeDir $dirlist]} $Gui(WBA)
    eval { entry $f1.dirname -textvariable DICOMStartDir } $Gui(WEA)
    
    eval {button $f3.ok -text "OK" -command "set Pressed OK; destroy $top"} $Gui(WBA)
    eval {button $f3.cancel -text "Cancel" -command "set Pressed Cancel; destroy $top"} $Gui(WBA)
    eval {button $f3.help -text "Help" -command "DICOMSelectDirHelp"} $Gui(WBA)

    pack $f1.changeto $f1.dirname -side left -padx 10 -pady 10
    pack $f2.dirlist -fill both -expand true
    pack $f3.ok $f3.cancel $f3.help -side left -padx 10 -pady 10 -anchor center
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
    
#    bind $dirlist <ButtonRelease-1> [list ClickDirList %W]
    bind $dirlist <Double-1> [list ClickDirList %W]
    bind $f1.dirname <KeyRelease-Return> [list $f1.changeto invoke]
    
    #cd $pwd
}

#-------------------------------------------------------------------------------
# .PROC DICOMSelectMain
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMSelectMain { fileNameListbox } {
    global DICOMStartDir
    global Pressed
    global DICOMPatientIDsNames
    global DICOMFileNameList DICOMFileNameSelected
    global Volume Volumes
    
    if {$Volume(activeID) != "NEW"} {
	return
    }

    set pwd [pwd]
    set DICOMStartDir $Volumes(DICOMStartDir)
    set DICOMFileNameList {}
    set DICOMFileNameSelected {}
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
        
	if { $Pressed == "OK" } {
	    #puts $DICOMFileNameList
	    $fileNameListbox delete 0 end
	    set Volume(dICOMFileList) {}
	    foreach name $DICOMFileNameList selected $DICOMFileNameSelected {
		if {$selected == "1"} {
		    $fileNameListbox insert end $name
		    lappend Volume(dICOMFileList) $name
		}
	    }
	    #set Volume(dICOMFileList) $DICOMFileNameList
	    
	    # use the second and the third
# 	    set file1 [lindex $DICOMFileNameList 1]
# 	    set file2 [lindex $DICOMFileNameList 2]
# 	    DICOMReadHeaderValues [lindex $DICOMFileNameList 0]
 	    set file1 [lindex $Volume(dICOMFileList) 1]
 	    set file2 [lindex $Volume(dICOMFileList) 2]
 	    DICOMReadHeaderValues [lindex $Volume(dICOMFileList) 0]
	    DICOMPredictScanOrder $file1 $file2
	    
	    set Volumes(DICOMStartDir) $DICOMStartDir
	}
    }
    
    cd $pwd
}

#-------------------------------------------------------------------------------
# .PROC HandleExtractHeader
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC DICOMReadHeaderValues
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
	    set Volume(height) [parser ReadUINT16]
	} else  {
	    set Volume(height) "unknown"
	}

	if { [parser FindElement 0x0028 0x0011] == "1" } {
	    #set Length [lindex [split [parser ReadElement]] 3]
	    parser ReadElement
	    set Volume(width) [parser ReadUINT16]
	} else  {
	    set Volume(width) "unknown"
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
	if { [parser FindElement 0x0018 0x5100] == "1" } {
	    set Length [lindex [split [parser ReadElement]] 3]
	    set str [parser ReadText $Length]
	    if {[string compare -nocase [string trim $str] "HFP"] == "0"} {
		set Volume(gantryDetectorTilt) [expr -1.0 * $Volume(gantryDetectorTilt)]
	    }
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

#-------------------------------------------------------------------------------
# .PROC DICOMPredictScanOrder
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMPredictScanOrder { file1 file2 } {
    global Volume

    # set default
    VolumesSetScanOrder "IS"

    if { ($file1 == "") || ($file2 == "") } {
	return
    }

    vtkDCMParser parser

    set found [parser OpenFile $file1]
    if {[string compare $found "0"] == 0} {
	puts stderr "Can't open file $file1\n"
	parser Delete
	return
    }

    if { [parser FindElement 0x0020 0x0032] == "1" } {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set x1 [parser ReadFloatAsciiNumeric $NextBlock]
	set y1 [parser ReadFloatAsciiNumeric $NextBlock]
	set z1 [parser ReadFloatAsciiNumeric $NextBlock]
    } else  {
	parser Delete
	return
    }

    set SlicePosition1 ""
    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set SlicePosition1 [parser ReadFloatAsciiNumeric $NextBlock]
    }

    parser CloseFile

    set found [parser OpenFile $file2]
    if {[string compare $found "0"] == 0} {
	puts stderr "Can't open file $file2\n"
	parser Delete
	return
    }

    if { [parser FindElement 0x0020 0x0032] == "1" } {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set x2 [parser ReadFloatAsciiNumeric $NextBlock]
	set y2 [parser ReadFloatAsciiNumeric $NextBlock]
	set z2 [parser ReadFloatAsciiNumeric $NextBlock]
    } else  {
	parser Delete
	return
    }

    set SlicePosition2 ""
    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set SlicePosition2 [parser ReadFloatAsciiNumeric $NextBlock]
    }

    #set Volume(filePattern) [format "%.2f %.2f %.2f" $x1 $y1 $z1]

    # Predict scan order

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
	    if { $dz > 0 } {
		# axial IS
		VolumesSetScanOrder "IS"
	    } else {
		# axial SI
		VolumesSetScanOrder "SI"
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
	    if { $dz > 0 } {
		# axial IS
		VolumesSetScanOrder "IS"
	    } else {
		# axial SI
		VolumesSetScanOrder "SI"
	    }
	}
    }

    # Calculate Slice Distance

    if {($SlicePosition1 != "") && ($SlicePosition2 != "")} {
	set diff [expr abs($SlicePosition2 - $SlicePosition1)]
    } else {
	set diff [expr sqrt($dx * $dx + $dy * $dy + $dz *$dz)]
    }

    scan $Volume(gantryDetectorTilt) "%f" gantrytilt
    if {$gantrytilt != ""} {
	set thickness [expr $diff * cos([expr $gantrytilt * acos(-1.0) / 180.0])]
    } else {
	set thickness $diff
    }

    if {$Volume(sliceThickness) != "unknown"} {
	if {[expr abs($thickness - $Volume(sliceThickness))] > 0.05} {
	    set answer [tk_messageBox -message "Slice thickness is $Volume(sliceThickness) in the header, but $thickness when calculated from Slice Positions.\nWould you like to use the calculated one ($thickness)?" \
			    -type yesno -icon question -title "Slice thickness question."]
	    if {$answer == "yes"} {
		set Volume(sliceThickness) $thickness
	    }
	}
    }

    parser CloseFile
    parser Delete
}

#-------------------------------------------------------------------------------
# .PROC DICOMPreviewAllButton
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMPreviewAllButton {} {
    global Volumes DICOMFileNameList DICOMFileNameSelected

    DICOMHideAllSettings
    $Volumes(ImageTextbox) delete 1.0 end
    for {set i 0} {$i < [llength $DICOMFileNameList]} {incr i} {
	set img [image create photo -width $Volumes(DICOMPreviewWidth) -height $Volumes(DICOMPreviewHeight) -palette 256]
	DICOMPreviewFile [lindex $DICOMFileNameList $i] $img
	if {[lindex $DICOMFileNameSelected $i] == "1"} {
	    set color green
	} else {
	    set color red
	}
	label $Volumes(ImageTextbox).l$i -image $img -background $color -cursor hand1
	bind $Volumes(ImageTextbox).l$i <ButtonRelease-1> "DICOMPreviewImageClick $Volumes(ImageTextbox).l$i $i"
	#label $Volumes(ImageTextbox).l$i -image $img -background green
	$Volumes(ImageTextbox) window create insert -window $Volumes(ImageTextbox).l$i
	#$Volumes(ImageTextbox) insert insert " "
	update idletasks
    }
}

#-------------------------------------------------------------------------------
# .PROC DICOMListHeadersButton
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMListHeadersButton {} {
    global Volumes DICOMFileNameList DICOMFileNameSelected

    DICOMHideAllSettings
    $Volumes(ImageTextbox) delete 1.0 end

    vtkDCMLister Volumes(lister)
    Volumes(lister) ReadList $Volumes(DICOMDataDictFile)
    Volumes(lister) SetListAll 0

    for {set i 0} {$i < [llength $DICOMFileNameList]} {incr i} {
	if {[lindex $DICOMFileNameSelected $i] == "0"} {
	    continue
	}

	set img [image create photo -width $Volumes(DICOMPreviewWidth) -height $Volumes(DICOMPreviewHeight) -palette 256]
	set filename [lindex $DICOMFileNameList $i]
	DICOMPreviewFile $filename $img
	label $Volumes(ImageTextbox).l$i -image $img
	$Volumes(ImageTextbox) window create insert -window $Volumes(ImageTextbox).l$i
	$Volumes(ImageTextbox) insert insert " $filename\n"
	DICOMListHeader $filename
	$Volumes(ImageTextbox) insert insert "\n"
	update idletasks
#	break
    }

    Volumes(lister) Delete
}

#-------------------------------------------------------------------------------
# .PROC DICOMListHeader
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMListHeader {filename} {
    global Volumes

    set ret [Volumes(lister) OpenFile $filename]
    if {$ret == "0"} {
	return
    }
    
    #Volumes(lister) ReadList $Volumes(DICOMDataDictFile)
    #Volumes(lister) SetListAll 0

    while {[Volumes(lister) IsStatusOK] == "1"} {
	set ret [Volumes(lister) ReadElement]
	if {[Volumes(lister) IsStatusOK] == "0"} {
	    break
	}
	set group [lindex $ret 1]
	set element [lindex $ret 2]
	set length [lindex $ret 3]
	set vr [lindex $ret 0]
	set msg [Volumes(lister) callback $group $element $length $vr]
	if {$msg != "Empty."} {
	    $Volumes(ImageTextbox) insert insert $msg
	    #$Volumes(ImageTextbox) insert insert "\n"
	    $Volumes(ImageTextbox) see end
	    update idletasks
	}
    }

    Volumes(lister) CloseFile
}

#-------------------------------------------------------------------------------
# .PROC DICOMPreviewFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMPreviewFile {file img} {
    global Volumes Volume

    vtkDCMLister parser

    set found [parser OpenFile $file]
    if {$found == "0"} {
	puts stderr "Can't open file $file\n"
	parser Delete
	return
    }

    if { [parser FindElement 0x0028 0x0010] == "1" } {
	parser ReadElement
	set height [parser ReadUINT16]
    } else  {
	parser Delete
	return
    }

    if { [parser FindElement 0x0028 0x0011] == "1" } {
	parser ReadElement
	set width [parser ReadUINT16]
    } else  {
	parser Delete
	return
    }

    set SkipColumn [expr int(($width - 1) / $Volumes(DICOMPreviewWidth)) * 2]
    set SkipRow [expr int(($height - 1) / $Volumes(DICOMPreviewHeight)) * $width * 2]
    set WidthInBytes [expr $width * 2]

    if { [parser FindElement 0x7fe0 0x0010] == "1" } {
	parser ReadElement

	for {set i 0} {$i < $Volumes(DICOMPreviewHeight)} {incr i} {
	    set row {}

	    set FilePos [parser GetFilePosition]
	    $img put [list [parser GetTCLPreviewRow $Volumes(DICOMPreviewWidth) $SkipColumn $Volumes(DICOMPreviewHighestValue)]] -to 0 $i
	    parser SetFilePosition [expr $FilePos + $WidthInBytes]
	    parser Skip $SkipRow
	}
    }

    parser CloseFile
    parser Delete
}

#-------------------------------------------------------------------------------
# .PROC DICOMCheckFiles
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMCheckFiles {} {
    global Volumes Volume DICOMFileNameList DICOMFileNameSelected

    set t $Volumes(ImageTextbox)
    $t delete 1.0 end

    set num 0
    foreach selected $DICOMFileNameSelected {
	# It is assumed that the possible values of selected
	# are 0 and 1 only.
	incr num $selected
    }
    if {$num < 2} {
	$t insert insert "Not a volume - can't check.\n"
	return
    }

    set num [llength $DICOMFileNameSelected]
    set localDICOMFileNameList {}
    set localIndex {}
    for {set i 0} {$i < $num} {incr i} {
	if {[lindex $DICOMFileNameSelected $i] == "1"} {
	    lappend localDICOMFileNameList [lindex $DICOMFileNameList $i]
	    lappend localIndex $i
	}
    }

    set file1 [lindex $localDICOMFileNameList 0]
    set file2 [lindex $localDICOMFileNameList 1]
    set ret [DICOMCheckVolumeInit $file1 $file2 [lindex $localIndex 0]]
    if {$ret == "1"} {
	set max [llength $localDICOMFileNameList]
	for {set i 1} {$i < $max} {incr i} {
	    set file [lindex $localDICOMFileNameList $i]
	    set msg "Checking $file\n"
	    $t insert insert $msg
	    $t see end
	    update idletasks
	    DICOMCheckFile $file [lindex $localIndex $i] [lindex $localIndex [expr $i - 1]]
	}
    }
    lappend Volumes(DICOMCheckVolumeList) $Volumes(DICOMCheckActiveList)
    lappend Volumes(DICOMCheckPositionList) $Volumes(DICOMCheckActivePositionList)
    lappend Volumes(DICOMCheckSliceDistanceList) $Volumes(DICOMCheckSliceDistance)

    $t insert insert "Volume checking finished.\n"

    $t insert insert $Volumes(DICOMCheckVolumeList)
    $t insert insert "\n"
    $t insert insert $Volumes(DICOMCheckPositionList)
    $t insert insert "\n"
    $t insert insert $Volumes(DICOMCheckSliceDistanceList)
    $t insert insert "\n"

    set len [llength $Volumes(DICOMCheckSliceDistanceList)]
    if {$len == "1"} {
	if {[llength [lindex $Volumes(DICOMCheckVolumeList) 0]] > 0} {
	    $t insert insert "The volume seems to be OK.\n"
	    $t see end
	}
	return
    }

    set msg "$len fragments detected.\n"
    $t insert insert $msg

    set Volumes(DICOMCheckImageLabelIdx) 0
    for {set i 0} {$i < $len} {incr i} {
	set firstpos [$t index insert]
	set dist [lindex $Volumes(DICOMCheckSliceDistanceList) $i]
	set activeList [lindex $Volumes(DICOMCheckVolumeList) $i]
	set activeLength [llength $activeList]
	set activePositionList [lindex $Volumes(DICOMCheckPositionList) $i]
	$t insert insert "Fragment \#[expr $i + 1], $activeLength slices, slice distance is $dist:\n"
	$t insert insert "Positions: "
	$t insert insert $activePositionList
	$t insert insert "\n"

	for {set j 0} {$j < $activeLength} {incr j} {
	    set idx [lindex $activeList $j]
	    set file [lindex $DICOMFileNameList $idx]
	    set img [image create photo -width $Volumes(DICOMPreviewWidth) -height $Volumes(DICOMPreviewHeight) -palette 256]
	    DICOMPreviewFile $file $img
	    set labelIdx $Volumes(DICOMCheckImageLabelIdx)
	    label $Volumes(ImageTextbox).l$Volumes(DICOMCheckImageLabelIdx) -image $img -cursor hand1
	    bind $Volumes(ImageTextbox).l$Volumes(DICOMCheckImageLabelIdx) <ButtonRelease-1> "DICOMSelectFragment $i"
	    bind $Volumes(ImageTextbox).l$Volumes(DICOMCheckImageLabelIdx) <Enter> "DICOMImageTextboxFragmentEnter %W fragm${i}"
	    bind  $Volumes(ImageTextbox).l$Volumes(DICOMCheckImageLabelIdx) <Leave> "DICOMImageTextboxFragmentLeave %W fragm${i}"
	    $Volumes(ImageTextbox) window create insert -window $Volumes(ImageTextbox).l$Volumes(DICOMCheckImageLabelIdx)
	    incr Volumes(DICOMCheckImageLabelIdx)
	}
	$t insert insert "\n\n"
	$t tag add fragm${i} $firstpos [$t index insert]
	$t tag bind fragm${i} <ButtonRelease-1> "DICOMSelectFragment $i"
	$t tag bind fragm${i} <Enter> "DICOMImageTextboxFragmentEnter %W fragm${i}"
	$t tag bind fragm${i} <Leave> "DICOMImageTextboxFragmentLeave %W fragm${i}"

	$t see end
	update idletasks
    }
}

#-------------------------------------------------------------------------------
# .PROC DICOMCheckVolumeInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMCheckVolumeInit {file1 file2 file1idx} {
    global Volumes Volume

    vtkDCMParser parser
    set t $Volumes(ImageTextbox)

    set Volumes(DICOMCheckVolumeList) {}
    set Volumes(DICOMCheckPositionList) {}
    set Volumes(DICOMCheckActiveList) {}
    set Volumes(DICOMCheckActivePositionList) {}
    set Volumes(DICOMCheckSliceDistanceList) {}

    set found [parser OpenFile $file1]
    if {$found == "0"} {
	$t insert insert "Can't open file $file1\n"
	parser Delete
	return 0
    }

    set SlicePosition1 ""
    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set SlicePosition1 [parser ReadFloatAsciiNumeric $NextBlock]
    } 
    if { $SlicePosition1 == "" } {
	$t insert insert "Image Position (0020,1041) not found in file $file1.\n"
	parser Delete
	return 0
    }
    
    $t insert insert "Detected slice position in $file1: $SlicePosition1\n"

    parser CloseFile

    set found [parser OpenFile $file2]
    if {$found == "0"} {
	$t insert insert "Can't open file $file2\n"
	parser Delete
	return 0
    }

    set SlicePosition2 ""
    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set SlicePosition2 [parser ReadFloatAsciiNumeric $NextBlock]
    } 
    if { $SlicePosition2 == "" } {
	$t insert insert "Image Position (0020,1041) not found in file $file2.\n"
	parser Delete
	return 0
    }
    
    $t insert insert "Detected slice position in $file2: $SlicePosition2\n"

    set Volumes(DICOMCheckLastPosition) $SlicePosition1
    set Volumes(DICOMCheckSliceDistance) [expr $SlicePosition2 - $SlicePosition1]
    $t insert insert "Detected slice distance: $Volumes(DICOMCheckSliceDistance)\n"

    lappend Volumes(DICOMCheckActiveList) $file1idx
    lappend Volumes(DICOMCheckActivePositionList) $SlicePosition1

    parser CloseFile
    parser Delete
    return 1
}

#-------------------------------------------------------------------------------
# .PROC DICOMCheckFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMCheckFile {file idx previdx} {
    global Volumes Volume

    vtkDCMParser parser
    set t $Volumes(ImageTextbox)

    set found [parser OpenFile $file]
    if {$found == "0"} {
	$t insert insert "Can't open file $file\n"
	parser Delete
	return 0
    }

    set SlicePosition ""
    if [expr [parser FindElement 0x0020 0x1041] == "1"] {
	set NextBlock [lindex [split [parser ReadElement]] 4]
	set SlicePosition [parser ReadFloatAsciiNumeric $NextBlock]
    } 
    if { $SlicePosition == "" } {
	$t insert insert "Image Position (0020,1041) not found in file $file. Skipping.\n"
	parser Delete
	return 0
    }
    
    $t insert insert "Detected slice position in $file: $SlicePosition\n"
    set dist [expr $SlicePosition - $Volumes(DICOMCheckLastPosition)]
    set diff [expr abs($dist - $Volumes(DICOMCheckSliceDistance))]
    if {$diff < 0.1} {
	lappend Volumes(DICOMCheckActiveList) $idx
	lappend Volumes(DICOMCheckActivePositionList) $SlicePosition
    } else {
	lappend Volumes(DICOMCheckVolumeList) $Volumes(DICOMCheckActiveList)
	lappend Volumes(DICOMCheckPositionList) $Volumes(DICOMCheckActivePositionList)
	lappend Volumes(DICOMCheckSliceDistanceList) $Volumes(DICOMCheckSliceDistance)
	set Volumes(DICOMCheckActiveList) [list $previdx $idx]
	set Volumes(DICOMCheckActivePositionList) {}
	lappend Volumes(DICOMCheckActivePositionList) $Volumes(DICOMCheckLastPosition)
	lappend Volumes(DICOMCheckActivePositionList) $SlicePosition
	$t insert insert "Slice Position discrepancy. New distance: $dist\n"
	set Volumes(DICOMCheckSliceDistance) $dist
    }

    set Volumes(DICOMCheckLastPosition) $SlicePosition

    parser CloseFile
    parser Delete

    return 1
}

#-------------------------------------------------------------------------------
# .PROC DICOMShowPreviewSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMShowPreviewSettings {} {
    global Volumes

    raise $Volumes(DICOMPreviewSettingsFrame)
    DICOMHideDataDictSettings
}

#-------------------------------------------------------------------------------
# .PROC DICOMHidePreviewSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMHidePreviewSettings {} {
    global Volumes

    lower $Volumes(DICOMPreviewSettingsFrame) $Volumes(ImageTextbox)
}

#-------------------------------------------------------------------------------
# .PROC DICOMShowDataDictSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMShowDataDictSettings {} {
    global Volumes

    raise $Volumes(DICOMDataDictSettingsFrame)
    DICOMHidePreviewSettings
}

#-------------------------------------------------------------------------------
# .PROC DICOMHideDataDictSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMHideDataDictSettings {} {
    global Volumes

    lower $Volumes(DICOMDataDictSettingsFrame) $Volumes(ImageTextbox)
}

#-------------------------------------------------------------------------------
# .PROC DICOMHideAllSettings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMHideAllSettings {} {
    global Volumes

    DICOMHidePreviewSettings
    DICOMHideDataDictSettings
}

#-------------------------------------------------------------------------------
# .PROC DICOMSelectFragment
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMSelectFragment {fragment} {
    global Volumes DICOMFileNameList DICOMFileNameSelected

    set activeList [lindex $Volumes(DICOMCheckVolumeList) $fragment]
    set activeLength [llength $activeList]
    set activePositionList [lindex $Volumes(DICOMCheckPositionList) $fragment]

    set DICOMFileNameSelected {}
    set num [llength $DICOMFileNameList]
    for {set j 0} {$j < $num} {incr j} {
	lappend DICOMFileNameSelected "0"
    }

    for {set j 0} {$j < $activeLength} {incr j} {
	set idx [lindex $activeList $j]
	set DICOMFileNameSelected [lreplace $DICOMFileNameSelected $idx $idx "1"]
    }
    $Volumes(DICOMFileNameTextbox) see [lindex $activeList 0].0
    DICOMFillFileNameTextbox $Volumes(DICOMFileNameTextbox)
}

#-------------------------------------------------------------------------------
# .PROC DICOMImageTextboxFragmentEnter
#   Changes the cursor over the PointTextbox to a cross
#   and stores the old one.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMImageTextboxFragmentEnter {w tag} {
    global Volumes

    set f2 $Volumes(ImageTextbox)
    #set Volumes(ImageTextboxOldCursor) [$f2 cget -cursor]
    set Volumes(ImageTextboxOldCursor) [$w cget -cursor]
    #$f2 configure -cursor pencil
    $w configure -cursor hand1
    $f2 tag configure $tag -background #43ce80 -relief raised -borderwidth 1
}

#-------------------------------------------------------------------------------
# .PROC DICOMImageTextboxFragmentLeave
#   Changes back the original cursor after leaving
#   the PointTextbox.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMImageTextboxFragmentLeave {w tag} {
    global Volumes

    set f2 $Volumes(ImageTextbox)
    #$f2 configure -cursor $Volumes(ImageTextboxOldCursor)
    $w configure -cursor $Volumes(ImageTextboxOldCursor)
    $f2 tag configure $tag -background {} -relief flat
}

#-------------------------------------------------------------------------------
# .PROC DICOMImageTextboxSelectAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMImageTextboxSelectAll {} {
    global Volumes DICOMFileNameSelected

    set num [llength $DICOMFileNameSelected]
    set DICOMFileNameSelected {}
    for {set i 0} {$i < $num} {incr i} {
	lappend DICOMFileNameSelected "1"
    }
    DICOMFillFileNameTextbox $Volumes(DICOMFileNameTextbox)    
}

#-------------------------------------------------------------------------------
# .PROC DICOMImageTextboxDeselectAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DICOMImageTextboxDeselectAll {} {
    global Volumes DICOMFileNameSelected

    set num [llength $DICOMFileNameSelected]
    set DICOMFileNameSelected {}
    for {set i 0} {$i < $num} {incr i} {
	lappend DICOMFileNameSelected "0"
    }
    DICOMFillFileNameTextbox $Volumes(DICOMFileNameTextbox)    
}

#-------------------------------------------------------------------------------
# .PROC VolumesEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesEnter {} {
    global Volumes

    pushEventManager $Volumes(eventManager)

    DataExit
    bind Listbox <Control-Button-1> {tkListboxBeginToggle %W [%W index @%x,%y]}
    #tk_messageBox -type ok -message "VolumesEnter" -title "Title" -icon  info
}

#-------------------------------------------------------------------------------
# .PROC VolumesExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesExit {} {
    global Volumes

    popEventManager
    #tk_messageBox -type ok -message "VolumesExit" -title "Title" -icon  info
}

# >> Presets

#-------------------------------------------------------------------------------
# .PROC VolumesStorePresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesStorePresets {p} {
    global Preset Volumes

    set Preset(Volumes,$p,DICOMStartDir) $Volumes(DICOMStartDir)
    set Preset(Volumes,$p,FileNameSortParam) $Volumes(FileNameSortParam)
    set Preset(Volumes,$p,DICOMPreviewWidth) $Volumes(DICOMPreviewWidth)
    set Preset(Volumes,$p,DICOMPreviewHeight) $Volumes(DICOMPreviewHeight)
    set Preset(Volumes,$p,DICOMPreviewHighestValue) $Volumes(DICOMPreviewHighestValue)
    set Preset(Volumes,$p,DICOMDataDictFile) $Volumes(DICOMDataDictFile)
}

#-------------------------------------------------------------------------------
# .PROC VolumesRecallPresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumesRecallPresets {p} {
    global Preset Volumes

    set Volumes(DICOMStartDir) $Preset(Volumes,$p,DICOMStartDir)
    set Volumes(FileNameSortParam) $Preset(Volumes,$p,FileNameSortParam)
    set Volumes(DICOMPreviewWidth) $Preset(Volumes,$p,DICOMPreviewWidth)
    set Volumes(DICOMPreviewHeight) $Preset(Volumes,$p,DICOMPreviewHeight)
    set Volumes(DICOMPreviewHighestValue) $Preset(Volumes,$p,DICOMPreviewHighestValue)
    set Volumes(DICOMDataDictFile) $Preset(Volumes,$p,DICOMDataDictFile)
}

# << Presets
