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
# FILE:        Locator.tcl
# DATE:        02/16/2000 09:13
# LAST EDITOR: gering
# PROCEDURES:  
#   LocatorInit
#   LocatorUpdateMRML
#   LocatorBuildVTK
#   LocatorBuildGUI
#   LocatorSetActive
#   LocatorSetPatientPosition
#   LocatorSetDriverAll
#   LocatorSetDriver
#   LocatorSetVisibility
#   LocatorSetTransverseVisibility
#   LocatorSetColor
#   LocatorSetSize
#   LocatorSetMatrices
#   LocatorSetPosition
#   LocatorUseLocatorMatrix
#   LocatorFormat
#   LocatorGetRealtimeID
#   LocatorSetRealtime
#   LocatorWrite
#   LocatorRead
#   LocatorPause
#   LocatorConnect
#   LocatorLoopFile
#   LocatorLoopImages
#   LocatorLoopSignaSP
#   LocatorFilePrefix
#   LocatorImagesPrefix
#   LocatorStorePresets
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC LocatorInit
# .END
#-------------------------------------------------------------------------------
proc LocatorInit {} {
	global Locator Module

	# Define Tabs
	set m Locator
	set Module($m,row1List) "Help Tracking Server Handpiece"
	set Module($m,row1Name) "Help Tracking Server Handpiece"
	set Module($m,row1,tab) Tracking

	# Define Procedures
	set Module($m,procGUI)   LocatorBuildGUI
	set Module($m,procVTK)   LocatorBuildVTK

	lappend Module(procStorePresets) LocatorStorePresets
	lappend Module(procRecallPresets) LocatorRecallPresets
	set Module(Locator,presets) "0,driver='User' 1,driver='User' 2,driver='User'\
 visibility='0' transverseVisibility='1' normalLen='100' transverseLen='25'\
 radius='3.0' diffuseColor='0.9 0.9 0.1'"

	# Patient/Table position
	set Locator(tblPosList)   "Front Side"
	set Locator(patEntryList) "Head-first Feet-first"
	set Locator(patPosList)   "Supine Prone Left-decub Right-decub"
	set Locator(tblPos)       [lindex $Locator(tblPosList) 0]
	set Locator(patEntry)     [lindex $Locator(patEntryList) 0]
	set Locator(patPos)       [lindex $Locator(patPosList) 0]

	# Locator position
	set Locator(nx) 0
	set Locator(ny) -1
	set Locator(nz) 0
	set Locator(tx) 1
	set Locator(ty) 0
	set Locator(tz) 0
	set Locator(px) 0 
	set Locator(py) 0
	set Locator(pz) 0
	LocatorFormat

	# Locator attributes
	set Locator(visibility) 0
	set Locator(transverseVisibility) 1
	set Locator(normalLen) 100
	set Locator(transverseLen) 25
	set Locator(radius) 3.0
	set Locator(normalOffset) 0
	set Locator(transverseOffset) 0
	set Locator(crossOffset) 0
	set Locator(0,driver) User
	set Locator(1,driver) User
	set Locator(2,driver) User
	set Locator(diffuseColor) ".9 .9 .1"
	scan $Locator(diffuseColor) "%g %g %g" Locator(red) Locator(green) Locator(blue)

	# Realtime image
	set Locator(idRealtime)     NEW
	set Locator(prefixRealtime) ""
	
	# Servers
	set Locator(serverList) "File SignaSP Images"
	set Locator(server) [lindex $Locator(serverList) 0]
	set Locator(connect) 0
	set Locator(pause) 0
	set Locator(loop) 0
	set Locator(imageNum) ""
	set Locator(recon) 0
	set Locator(callbackList) ""

	# Server specific stuff:
	# File
	set Locator(File,msPoll) 100
	set Locator(File,prefix) loc
	set Locator(File,fid)    ""
	# SignaSP
	set Locator(SignaSP,msPoll) 100
	set Locator(SignaSP,port)   10000
	set Locator(SignaSP,host)   mrtsw
	# Images
	set Locator(Images,msPoll)   1000
	set Locator(Images,prefix)   ""
	set Locator(Images,firstNum) 1
	set Locator(Images,lastNum)  1
	set Locator(Images,increment) 1

}

#-------------------------------------------------------------------------------
# .PROC LocatorUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc LocatorUpdateMRML {} {
	global Volume Locator

	# See if the volume for each menu actually exists.
	# If not, use the None volume
	#
	set n $Volume(idNone)
	if {$Locator(idRealtime) != "NEW" && \
		[lsearch $Volume(idList) $Locator(idRealtime)] == -1} {
		LocatorSetRealtime NEW
	}

	# Realtime Volume menu
	#---------------------------------------------------------------------------
	set m $Locator(mRealtime)
	$m delete 0 end
	set idRealtime ""
	foreach v $Volume(idList) {
		if {$v != $Volume(idNone) && $v != $Locator(idResult)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"LocatorSetRealtime $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Realtime"} {
			set idRealtime $v
		}
	}
	# If there is Realtime, then select it, else add a NEW option
	if {$idRealtime != ""} {
		LocatorSetRealtime $idRealtime
	} else {
		$m add command -label NEW -command "LocatorSetRealtime NEW; RenderAll"
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorBuildVTK
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildVTK {} {
	global Gui Locator View Slice Target Volume Lut

	#------------------------#
	# Realtime image source
	#------------------------#
	# SignaSP
	vtkImageRealtimeScan Locator(SignaSP,src)
#	Locator(SignaSP,src) SetTest 1

	# Images
	vtkMrmlVolumeNode Locator(Images,node)
	vtkMrmlVolume Locator(Images,vol)
	set n Locator(Images,node)
	set v Locator(Images,vol)
	$n SetDescription "Realtime Images source"
	$n SetName        "Source"
	$n SetLUTName     [lindex $Lut(idList) 0]
	$v SetMrmlNode          $n
	$v SetLabelIndirectLUT  Lut($Lut(idLabel),indirectLUT)
	$v SetLookupTable       Lut([$n GetLUTName],lut)
	$v SetHistogramHeight   $Volume(histHeight)
	$v SetHistogramWidth    $Volume(histWidth)
	$v SetStartMethod       MainStartProgress
	$v SetProgressMethod   "MainShowProgress $v"
	$v SetEndMethod         MainEndProgress

	#------------------------#
	# Construct handpiece
	#------------------------#
	vtkMatrix4x4 Locator(normalMatrix)
	vtkMatrix4x4 Locator(transverseMatrix)
	vtkMatrix4x4 Locator(tipMatrix)

	set Locator(actors) "normal transverse tip"

	set actor normal 
	MakeVTKObject Cylinder $actor
		${actor}Source SetRadius $Locator(radius) 
		${actor}Source SetHeight $Locator(normalLen)
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
		${actor}Actor SetUserMatrix Locator(normalMatrix)

	set actor transverse
	MakeVTKObject Cylinder ${actor}
		${actor}Source SetRadius $Locator(radius) 
		${actor}Source SetHeight [expr $Locator(transverseLen)]
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
		${actor}Actor SetUserMatrix Locator(transverseMatrix)
	
	set actor tip
	MakeVTKObject Sphere ${actor}
		${actor}Source SetRadius [expr 1.5 * $Locator(radius)] 
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
		${actor}Actor SetUserMatrix Locator(tipMatrix)
	
	LocatorSetMatrices
	LocatorSetVisibility
}


#-------------------------------------------------------------------------------
# .PROC LocatorBuildGUI
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildGUI {} {
	global Gui Module Locator Slice

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Tracking
	# Server
	# Handpiece
	#   
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Locator $help
	MainHelpBuildGUI Locator

	#-------------------------------------------
	# Tracking frame
	#-------------------------------------------
	set fTracking $Module(Locator,fTracking)
	set f $fTracking

	# Frames
	frame $f.fConn     -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fDriver   -bg $Gui(activeWorkspace) -relief groove -bd 3 
	frame $f.fVis      -bg $Gui(activeWorkspace) 
	frame $f.fRealtime -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fConn $f.fVis $f.fDriver $f.fRealtime \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Tracking->Conn frame
	#-------------------------------------------
	set f $fTracking.fConn

	set c {label $f.l -text "Connection to Server" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fTracking.fConn.f
	set c {checkbutton $f.cConnect \
		-text "Connect" -variable Locator(connect) -width 8 \
		-indicatoron 0 -command "LocatorConnect" $Gui(WCA)}
		eval [subst $c]
	set c {checkbutton $f.cPause \
		-text "Pause" -variable Locator(pause) -command "LocatorPause" -width 6 \
		-indicatoron 0 $Gui(WCA)}
		eval [subst $c]
	pack $f.cConnect $f.cPause -side left -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Tracking->Driver Frame
	#-------------------------------------------
	set f $fTracking.fDriver

	set c {label $f.lTitle -text "The Locator can Drive Slices" $Gui(WTA)}
		eval [subst $c]
	grid $f.lTitle -padx $Gui(pad) -pady 3 -columnspan 2
	
	foreach s $Slice(idList) {

		set c {menubutton $f.mbDriver${s} -text "User" \
			-menu $f.mbDriver${s}.m -width 12 -relief raised \
			-bd 2 $Gui(WMBA) -bg $Gui(slice$s)}; eval [subst $c]
		set Locator(mDriver$s) $f.mbDriver${s}.m
		set Locator(mbDriver$s) $f.mbDriver${s}
		set c {menu $f.mbDriver${s}.m $Gui(WMA)}; eval [subst $c];
		foreach item "User Locator" {
			$Locator(mDriver$s) add command -label $item \
				-command "LocatorSetDriver ${s} $item; RenderBoth $s"
		}
		if {$s == 0} {
			set c {menubutton $f.mbDriver -text "Driver: " \
				-menu $f.mbDriver.m -width 10 -relief raised \
				-bd 2 $Gui(WMBA) -anchor e}; eval [subst $c]

			set Locator(mDriver) $f.mbDriver.m
			set c {menu $Locator(mDriver) $Gui(WMA)}; eval [subst $c];
			foreach item "User Locator" {
				$f.mbDriver.m add command -label $item \
					-command "LocatorSetDriverAll $item; RenderAll"
			}
			grid $f.mbDriver $f.mbDriver${s} \
				-pady $Gui(pad) -padx $Gui(pad)
		} else {
			grid $f.mbDriver${s} -col 1 \
				-pady $Gui(pad) -padx $Gui(pad)
		}

	}

	#-------------------------------------------
	# Tracking->Vis Frame
	#-------------------------------------------
	set f $fTracking.fVis

	set c {checkbutton $f.cLocator \
		-text "Show Locator" -variable Locator(visibility) -width 16 \
		-indicatoron 0 -command "LocatorSetVisibility; Render3D" $Gui(WCA)}
		eval [subst $c]
	set c {checkbutton $f.cTransverse \
		-text "Handle" -variable Locator(transverseVisibility) -width 7 \
		-indicatoron 0 -command "LocatorSetTransverseVisibility; Render3D" $Gui(WCA)}
		eval [subst $c]
	pack $f.cLocator $f.cTransverse -side left -padx $Gui(pad)


	#-------------------------------------------
	# Tracking->Realtime
	#-------------------------------------------
	set f $fTracking.fRealtime

	frame $f.fMenu -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.fMenu -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	#-------------------------------------------
	# Tracking->Realtime->Menu
	#-------------------------------------------
	set f $fTracking.fRealtime.fMenu

	# Volume menu
	set c {label $f.lRealtime -text "Realtime Volume:" $Gui(WTA)}; eval [subst $c]

	set c {menubutton $f.mbRealtime -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbRealtime.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbRealtime.m $Gui(WMA)}; eval [subst $c]
	pack $f.lRealtime $f.mbRealtime -padx $Gui(pad) -side left

	# Save widgets for changing
	set Locator(mbRealtime) $f.mbRealtime
	set Locator(mRealtime)  $f.mbRealtime.m

	#-------------------------------------------
	# Tracking->Realtime->Prefix
	#-------------------------------------------
	set f $fTracking.fRealtime.fPrefix

	eval {label $f.l -text "Prefix:"} $Gui(WLA)
	set c {entry $f.e \
		-textvariable Locator(prefixRealtime) $Gui(WEA)}; eval [subst $c]
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Tracking->Realtime->Btns
	#-------------------------------------------
	set f $fTracking.fRealtime.fBtns

	set c {button $f.bWrite -text "Save" -width 5 \
		-command "LocatorWrite Realtime; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bRead -text "Read" -width 5 \
		-command "LocatorRead Realtime; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite $f.bRead -side left -padx $Gui(pad)



	#-------------------------------------------
	# Server frame
	#-------------------------------------------
	set fServer $Module(Locator,fServer)
	set f $fServer

	frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fBot -bg $Gui(activeWorkspace) -height 330
	pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Server->Bot frame
	#-------------------------------------------
	set f $fServer.fBot

	foreach s $Locator(serverList) {
		frame $f.f$s -bg $Gui(activeWorkspace)
		place $f.f$s -in $f -relheight 1.0 -relwidth 1.0
		set Locator(f$s) $f.f$s
	}
	raise $Locator(f[lindex $Locator(serverList) 0])

	#-------------------------------------------
	# Server->Top frame
	#-------------------------------------------
	set f $fServer.fTop

	frame $f.fActive -bg $Gui(backdrop)
	pack $f.fActive -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Server->Top->Active frame
	#-------------------------------------------
	set f $fServer.fTop.fActive

	eval {label $f.lActive -text "Active Server: "} $Gui(BLA)
	eval {menubutton $f.mbActive \
		-text [lindex $Locator(serverList) 0] \
		-relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left
	set Locator(mbActive) $f.mbActive

	# Form the Active menu 
	#--------------------------------------------------------
	set m $Locator(mbActive).m
	foreach s $Locator(serverList) {
		$m add command -label $s \
			-command "LocatorSetActive $s"
	}

	#-------------------------------------------
	# Server->Bot->File frame
	#-------------------------------------------
	set f $fServer.fBot.fFile

	frame $f.fGrid   -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	pack $f.fPrefix $f.fGrid \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Server->Bot->File->Prefix frame
	#-------------------------------------------
	set f $fServer.fBot.fFile.fPrefix

	eval {button $f.b -text "Prefix" -width 7 \
		-command "LocatorFilePrefix"} $Gui(WBA)
	eval {entry $f.e -textvariable Locator(File,prefix)} $Gui(WEA)
	pack $f.b -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Server->Bot->File->Grid frame
	#-------------------------------------------
	set f $fServer.fBot.fFile.fGrid

	set s File
	foreach x "msPoll" text \
		"{Update Period (ms)}" {
		set c {label $f.l$x -text "${text}:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$x -textvariable Locator($s,$x) -width 7 $Gui(WEA)}; eval [subst $c]
		grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$x -sticky w
	}


	#-------------------------------------------
	# Server->Bot->Images frame
	#-------------------------------------------
	set f $fServer.fBot.fImages

	frame $f.fGrid   -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fNum    -bg $Gui(activeWorkspace)
	pack $f.fPrefix $f.fGrid $f.fNum \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Server->Bot->Images->Prefix frame
	#-------------------------------------------
	set f $fServer.fBot.fImages.fPrefix

	eval {button $f.b -text "Prefix" -width 7 \
		-command "LocatorImagesPrefix"} $Gui(WBA)
	eval {entry $f.e -textvariable Locator(Images,prefix)} $Gui(WEA)
	pack $f.b -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Server->Bot->Images->Grid frame
	#-------------------------------------------
	set f $fServer.fBot.fImages.fGrid

	set s Images
	foreach x "firstNum lastNum increment msPoll" text \
		"{First image number} {Last image number} {Image increment} {Update period (ms)}" {
		set c {label $f.l$x -text "${text}:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$x -textvariable Locator($s,$x) -width 7 $Gui(WEA)}; eval [subst $c]
		grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$x -sticky w
	}

	#-------------------------------------------
	# Server->Bot->Images->Num frame
	#-------------------------------------------
	set f $fServer.fBot.fImages.fNum

	eval {label $f.l -text "Current Image Number"} $Gui(WLA)
	eval {entry $f.e -textvariable Locator(imageNum) -width 6 -state disabled} $Gui(WEA)
	pack $f.l $f.e -side left -padx $Gui(pad)


	#-------------------------------------------
	# Server->Bot->SignaSP frame
	#-------------------------------------------
	set f $fServer.fBot.fSignaSP

	frame $f.fStatus  -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fPatient -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fNum     -bg $Gui(activeWorkspace)
	pack  $f.fStatus $f.fGrid $f.fPatient $f.fNum \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Server->Bot->SignaSP->Status frame
	#-------------------------------------------
	set f $fServer.fBot.fSignaSP.fStatus

	set c {label $f.lLocTitle -text "Locator Status" $Gui(WTA)}; eval [subst $c]
	set c {label $f.lLocStatus -text "None" -width 8 $Gui(WLA)}; eval [subst $c]
	grid $f.lLocTitle $f.lLocStatus -pady 0 -padx $Gui(pad)
	set Locator(lLocStatus) $f.lLocStatus

	#-------------------------------------------
	# Server->Bot->SignaSP->Grid frame
	#-------------------------------------------
	set f $fServer.fBot.fSignaSP.fGrid

	set c {label $f.lTitle -text "Server Connection" $Gui(WTA)}; eval [subst $c]
	grid $f.lTitle -columnspan 2 -pady $Gui(pad)

	set s SignaSP
	foreach x "host port msPoll" text \
		"{Host name} {Port number} {Update period (ms)}" {
		set c {label $f.l$x -text "${text}:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$x -textvariable Locator($s,$x) -width 10 $Gui(WEA)}; eval [subst $c]
		grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$x -sticky w
	}

	#-------------------------------------------
	# Server->Bot->SignaSP->Patient frame
	#-------------------------------------------
	set f $fServer.fBot.fSignaSP.fPatient

	set c {label $f.lTitle -text "Patient Position" $Gui(WTA)}; eval [subst $c]

	foreach pos "TblPos PatEntry PatPos" \
		name "{Table} {Entry} {Patient}" width "12 12 12"\
		choices "{$Locator(tblPosList)} {$Locator(patEntryList)} \
			{$Locator(patPosList)}" {
		set c {label $f.l$pos -text "$name:" $Gui(WLA)}; eval [subst $c]
		set c {menubutton $f.mb$pos -text "$Locator([Uncap $pos])" \
			-relief raised -bd 2 -width $width -menu $f.mb$pos.menu $Gui(WMBA)}
			eval [subst $c]
		set Locator(mb$pos) $f.mb$pos
		set c {menu $f.mb$pos.menu $Gui(WMA)}; eval [subst $c]
			set m $f.mb$pos.menu
			foreach choice $choices {
				$m add command -label $choice -command \
					"LocatorSetPatientPosition [Uncap $pos] $choice"
			}
	}
	grid $f.lTitle -columnspan 2 -pady $Gui(pad)
	grid $f.lPatEntry $f.mbPatEntry  -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.lPatPos $f.mbPatPos  -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.lTblPos $f.mbTblPos -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.mbTblPos $f.mbPatEntry $f.mbPatPos -sticky w

	#-------------------------------------------
	# Server->Bot->SignaSP->Num frame
	#-------------------------------------------
	set f $fServer.fBot.fSignaSP.fNum

	eval {label $f.l -text "Current Image Number"} $Gui(WLA)
	eval {entry $f.e -textvariable Locator(imageNum) -width 6 -state disabled} $Gui(WEA)
	pack $f.l $f.e -side left -padx $Gui(pad)



	#-------------------------------------------
	# Handpiece frame
	#-------------------------------------------
	set fHandpiece $Module(Locator,fHandpiece)
	set f $fHandpiece

	# Frames
	frame $f.fOffsetSize -bg $Gui(activeWorkspace)
	frame $f.fPos        -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fColor      -bg $Gui(activeWorkspace) -relief groove -bd 3 

	pack $f.fOffsetSize -side top -fill x
	pack $f.fPos $f.fColor \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x


	#-------------------------------------------
	# Handpiece->OffsetSize Frame
	#-------------------------------------------
	set f $fHandpiece.fOffsetSize

	frame $f.fOffset   -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fSize     -bg $Gui(activeWorkspace) -relief groove -bd 3 

	pack $f.fOffset $f.fSize \
		-side left -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Handpiece->OffsetSize->Offse Frame
	#-------------------------------------------
	set f $fHandpiece.fOffsetSize.fOffset

	set c {label $f.l -text "Offset from Tip" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $f.f
	foreach axis "N T NxT" text "Normal Trans. {N x T}" \
		var "normalOffset transverseOffset crossOffset" {
		set c {label $f.l$axis -text "$text" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$axis -textvariable Locator($var) \
			-width 4 $Gui(WEA)}; eval [subst $c]
		bind $f.e$axis <Return> "LocatorSetPosition; Render3D"
		grid $f.l$axis $f.e$axis -pady 2 -padx $Gui(pad) -sticky e
		grid $f.e$axis -sticky w 
	}

	#-------------------------------------------
	# Handpiece->OffsetSize->Size Frame
	#-------------------------------------------
	set f $fHandpiece.fOffsetSize.fSize

	set c {label $f.l -text "Size (mm)" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $f.f
	foreach var "normalLen transverseLen radius" \
		text "{Normal} {Trans.} {Radius}" {
		set c {label $f.l$var -text "$text" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$var -textvariable Locator($var) \
			-width 4 $Gui(WEA)}; eval [subst $c]
		bind $f.e$var <Return> "LocatorSetSize; LocatorSetMatrices; Render3D"
		grid $f.l$var $f.e$var -pady 2 -padx $Gui(pad) -sticky e
		grid $f.e$var -sticky w
	}


	#-------------------------------------------
	# Handpiece->Pos Frame
	#-------------------------------------------
	set f $fHandpiece.fPos

	set c {label $f.l -text "Position & Orientation" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $f.f
	set c {label $f.l -text "" $Gui(WLA)}; eval [subst $c]
	foreach ax "x y z" text "R A S" {
		set c {label $f.l$ax -text $text -width 7 $Gui(WLA)}; eval [subst $c]
	}
	grid $f.l $f.lx $f.ly $f.lz -pady 2 -padx $Gui(pad) -sticky e

	foreach axis "N T P" var "n t p" {
		set c {label $f.l$axis -text "$axis:" $Gui(WLA)}; eval [subst $c]
		foreach ax "x y z" text "R A S" {
			set c {entry $f.e$axis$ax -justify right -width 7 \
				-textvariable Locator($var${ax}Str) $Gui(WEA)}; eval [subst $c]
			bind $f.e$axis$ax <Return> "LocatorSetPosition; Render3D"
		}
		grid $f.l$axis $f.e${axis}x $f.e${axis}y $f.e${axis}z \
			-pady 2 -padx $Gui(pad) -sticky e
	}


	#-------------------------------------------
	# Handpiece->Color Frame
	#-------------------------------------------
	set f $fHandpiece.fColor

	set c {label $f.l -text "Color" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fHandpiece.fColor.f
	foreach slider "Red Green Blue" {

		set c {label $f.l${slider} -text "${slider}" $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Locator([Uncap $slider]) \
			-width 3 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   "LocatorSetColor; Render3D"
			bind $f.e${slider} <FocusOut> "LocatorSetColor; Render3D"

		set c {scale $f.s${slider} -from 0.0 -to 1.0 -length 100 \
			-variable Locator([Uncap $slider]) \
			-command "LocatorSetColor; Render3D" \
			-resolution 0.1 $Gui(WSA) -sliderlength 20} 
			eval [subst $c]
		set Locator(s$slider) $f.s$slider

		grid $f.l${slider} $f.e${slider} $f.s${slider}  \
			-pady 2 -padx 5 -sticky e
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetActive
# .END
#-------------------------------------------------------------------------------
proc LocatorSetActive {s} {
	global Locator

	set Locator(server) $s
	$Locator(mbActive) config -text $s
	raise $Locator(f$s)
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetPatientPosition
# .END
#-------------------------------------------------------------------------------
proc LocatorSetPatientPosition {{key ""} {value ""}} {
	global Locator
	
	if {$key != ""} {
		set Locator($key) $value
	}
	foreach key "TblPos PatEntry PatPos" {
		$Locator(mb$key) config -text "$Locator([Uncap $key])"
	}

	# Send to MRT workstation
	Locator(SignaSP,src) SetPosition \
		[lsearch $Locator(tblPosList)   $Locator(tblPos)] \
		[lsearch $Locator(patEntryList) $Locator(patEntry)] \
		[lsearch $Locator(patPosList)   $Locator(patPos)]
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriverAll
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriverAll {x} {
	global Slice 

	foreach s $Slice(idList) {
		LocatorSetDriver $s $x
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriver
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriver {s name} {
	global Locator
	
	# Change button text
	$Locator(mbDriver${s}) config -text $name

	# Change variable
	set Locator($s,driver) $name

	if {$name == "User"} {
		Slicer SetDriver $s 0
	} else {
		Slicer SetDriver $s 1
	}

	# Force recomputation of the reformat matrix
	Slicer SetDirectNTP \
		$Locator(nx) $Locator(ny) $Locator(nz) \
		$Locator(tx) $Locator(ty) $Locator(tz) \
		$Locator(px) $Locator(py) $Locator(pz) 
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetVisibility
# .END
#-------------------------------------------------------------------------------
proc LocatorSetVisibility {} {
	global Locator 

	foreach actor $Locator(actors) {
		${actor}Actor SetVisibility $Locator(visibility)
	}
	LocatorSetTransverseVisibility
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetTransverseVisibility
# .END
#-------------------------------------------------------------------------------
proc LocatorSetTransverseVisibility {} {
	global Locator 

	if {$Locator(visibility) == 1} {
		transverseActor SetVisibility $Locator(transverseVisibility)
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetColor
# .END
#-------------------------------------------------------------------------------
proc LocatorSetColor {{value ""}} {
	global Locator

	# Validate input
	if {[ValidateFloat $Locator(red)] == 0} {
		tk_messageBox -message "Red must be a number between 0.0 and 1.0"
		return
	}
	if {[ValidateFloat $Locator(green)] == 0} {
		tk_messageBox -message "Green must be a number between 0.0 and 1.0"
		return
	}
	if {[ValidateFloat $Locator(blue)] == 0} {
		tk_messageBox -message "Blue must be a number between 0.0 and 1.0"
		return
	}

	set Locator(diffuseColor) "$Locator(red) $Locator(green) $Locator(blue)"
	foreach actor $Locator(actors) {
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
	}

	foreach slider "Red Green Blue" {
		$Locator(s$slider) config -troughcolor [MakeColorNormalized $Locator(diffuseColor)]
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetSize
# .END
#-------------------------------------------------------------------------------
proc LocatorSetSize {} {
	global Locator

	if {[ValidateFloat $Locator(radius)] == 0} {
		tk_messageBox -message "Radius must be a floating point number"
		return
	}
	if {[ValidateFloat $Locator(normalLen)] == 0} {
		tk_messageBox -message "Normal Length must be a floating point number"
		return
	}
	if {[ValidateFloat $Locator(transverseLen)] == 0} {
		tk_messageBox -message "Transverse Length must be a floating point number"
		return
	}

	normalSource SetRadius $Locator(radius) 
	normalSource SetHeight $Locator(normalLen)
	transverseSource SetRadius $Locator(radius) 
	transverseSource SetHeight [expr $Locator(transverseLen)]
	tipSource SetRadius [expr 1.5 * $Locator(radius)] 
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetMatrices
# .END
#-------------------------------------------------------------------------------
proc LocatorSetMatrices {} {
	global Locator

	if {[ValidateFloat $Locator(radius)] == 0} {
		return
	}
	if {[ValidateFloat $Locator(normalLen)] == 0} {
		return
	}
	if {[ValidateFloat $Locator(transverseLen)] == 0} {
		return
	}

	# Find transform, N, that brings the locator coordinate frame 
	# into the scanner frame.  Then invert N to M and set it to the locator's
	# userMatrix to position the locator within the world space.
	#
	# 1.) Concatenate a translation, T, TO the origin which is (-x,-y,-z)
	#     where the locator's position is (x,y,z).
	# 2.) Concatenate the R matrix.  If the locator's reference frame has
	#     axis Ux, Uy, Uz, then Ux is the TOP ROW of R, Uy is the second, etc.
	# 3.) Translate the cylinder so its tip is at the origin instead
	#     of the center of its tube.  Call this matrix C.
	# Then: N = C*R*T, M = Inv(N)
	#
	# (See page 419 and 429 of "Computer Graphics", Hearn & Baker, 1997,
	#  ISBN 0-13-530924-7)
	# 
	# The alternative approach used here is to find the transform, M, that
	# moves the scanner coordinate frame to the locator's.  
	# 
	# 1.) Translate the cylinder so its tip is at the origin instead
	#     of the center of its tube.  Call this matrix C.
	# 2.) Concatenate the R matrix.  If the locator's reference frame has
	#     axis Ux, Uy, Uz, then Ux is the LEFT COL of R, Uy is the second,etc.
	# 3.) Concatenate a translation, T, FROM the origin which is (x,y,z)
	#     where the locator's position is (x,y,z).
	# Then: M = T*R*C

	vtkMatrix4x4 matrix
	vtkTransform transform

	# Locator's offset: x0, y0, z0
	set x0 $Locator(px)
	set y0 $Locator(py)
	set z0 $Locator(pz)

	# Locator's coordinate axis:
	# Ux = T
	set Ux(x) $Locator(tx)
	set Ux(y) $Locator(ty)
	set Ux(z) $Locator(tz)
	# Uy = -N
	set Uy(x) [expr - $Locator(nx)]
	set Uy(y) [expr - $Locator(ny)]
	set Uy(z) [expr - $Locator(nz)]
	# Uz = Ux x Uy
	set Uz(x) [expr $Ux(y)*$Uy(z) - $Uy(y)*$Ux(z)]
	set Uz(y) [expr $Uy(x)*$Ux(z) - $Ux(x)*$Uy(z)]
	set Uz(z) [expr $Ux(x)*$Uy(y) - $Uy(x)*$Ux(y)]

	# Ux
	matrix SetElement 0 0 $Ux(x)
	matrix SetElement 1 0 $Ux(y)
	matrix SetElement 2 0 $Ux(z)
	matrix SetElement 3 0 0
	# Uy
	matrix SetElement 0 1 $Uy(x)
	matrix SetElement 1 1 $Uy(y)
	matrix SetElement 2 1 $Uy(z)
	matrix SetElement 3 1 0
	# Uz
	matrix SetElement 0 2 $Uz(x)
	matrix SetElement 1 2 $Uz(y)
	matrix SetElement 2 2 $Uz(z)
	matrix SetElement 3 2 0
	# Bottom row
	matrix SetElement 0 3 0
	matrix SetElement 1 3 0
	matrix SetElement 2 3 0
	matrix SetElement 3 3 1

	# Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	# is multiplied by the existing matrix, M: C*M (not M*C)
	transform PostMultiply
	# M = T*R*C

	# TIP PART

	transform Identity
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(tipMatrix)
	# VTK BUG (i shouldn't have to call modify for the matrix)?
	Locator(tipMatrix) Modified
	
	# NORMAL PART

	transform Identity
	# C:
	transform Translate 0 [expr $Locator(normalLen) / 2.0] 0
	# R:
	transform Concatenate matrix
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(normalMatrix)
	Locator(normalMatrix) Modified

	# TRANSVERSE PART

	transform Identity
	# C: 
	transform RotateZ 90
	transform Translate \
		[expr [expr $Locator(transverseLen) / 2.0] - $Locator(radius)] \
		[expr $Locator(normalLen)] \
		 0 
	# R:
	transform Concatenate matrix
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(transverseMatrix)
	Locator(transverseMatrix) Modified

	matrix Delete
	transform Delete
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetPosition
# .END
#-------------------------------------------------------------------------------
proc LocatorSetPosition {{value ""}} {
	global Locator

	foreach p "px py pz nx ny nz tx ty tz" {
		if {[ValidateFloat $Locator(${p}Str)] == 0} {
			tk_messageBox -message "Floating point numbers required."
			return
		}
	}

	# Read matrix
	set Locator(px) $Locator(pxStr) 
	set Locator(py) $Locator(pyStr) 
	set Locator(pz) $Locator(pzStr)
	set Locator(nx) $Locator(nxStr)
	set Locator(ny) $Locator(nyStr)
	set Locator(nz) $Locator(nzStr)
	set Locator(tx) $Locator(txStr)
	set Locator(ty) $Locator(tyStr)
	set Locator(tz) $Locator(tzStr)

	LocatorUseLocatorMatrix
}

#-------------------------------------------------------------------------------
# .PROC LocatorUseLocatorMatrix
# .END
#-------------------------------------------------------------------------------
proc LocatorUseLocatorMatrix {} {
	global Locator Slice

	foreach p "normalOffset transverseOffset crossOffset" {
		if {[ValidateFloat $Locator($p)] == 0} {
			tk_messageBox -message "$p must be a floating point number."
			return
		}
	}

	# Form arrays so we can use vector processing functions
	set P(x) $Locator(px)
	set P(y) $Locator(py)
	set P(z) $Locator(pz)
	set N(x) $Locator(nx)
	set N(y) $Locator(ny)
	set N(z) $Locator(nz)
	set T(x) $Locator(tx)
	set T(y) $Locator(ty)
	set T(z) $Locator(tz)

	# Ensure N, T orthogonal:
	#	C = N x T
	#	T = C x N
	Cross C N T
	Cross T C N

	# Ensure vectors are normalized
	Normalize N
	Normalize T
	Normalize C

	# Offset the Locator
	set n $Locator(normalOffset)
	set t $Locator(transverseOffset)
	set c $Locator(crossOffset)
	set Locator(px) [expr $P(x) + $N(x)*$n + $T(x)*$t + $C(x)*$c]
	set Locator(py) [expr $P(y) + $N(y)*$n + $T(y)*$t + $C(y)*$c]
	set Locator(pz) [expr $P(z) + $N(z)*$n + $T(z)*$t + $C(z)*$c]
	set Locator(nx) $N(x)
	set Locator(ny) $N(y)
	set Locator(nz) $N(z)
	set Locator(tx) $T(x)
	set Locator(ty) $T(y)
	set Locator(tz) $T(z)

	# Format display
	LocatorFormat
				
	# Position the rendered locator
	LocatorSetMatrices
			
	# Find slices with their input set to locator.
	# and set the slice matrix with the new locator data

	Slicer SetDirectNTP \
		$Locator(nx) $Locator(ny) $Locator(nz) \
		$Locator(tx) $Locator(ty) $Locator(tz) \
		$Locator(px) $Locator(py) $Locator(pz) 
}

#-------------------------------------------------------------------------------
# .PROC LocatorFormat
# .END
#-------------------------------------------------------------------------------
proc LocatorFormat {} {
	global Locator

	set Locator(nxStr) [format "%.2f" $Locator(nx)]
	set Locator(nyStr) [format "%.2f" $Locator(ny)]
	set Locator(nzStr) [format "%.2f" $Locator(nz)]
	set Locator(txStr) [format "%.2f" $Locator(tx)]
	set Locator(tyStr) [format "%.2f" $Locator(ty)]
	set Locator(tzStr) [format "%.2f" $Locator(tz)]
	set Locator(pxStr) [format "%.2f" $Locator(px)]
	set Locator(pyStr) [format "%.2f" $Locator(py)]
	set Locator(pzStr) [format "%.2f" $Locator(pz)]
}

#-------------------------------------------------------------------------------
# .PROC LocatorGetRealtimeID
#
# Returns the Realtime volume's ID.
# If there is no Realtime volume (Locator(idRealtime)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc LocatorGetRealtimeID {} {
	global Locator Volume Lut
		
	# If there is no Realtime volume, then create one
	if {$Locator(idRealtime) != "NEW"} {
		return $Locator(idRealtime)
	}
	
	# Create the node
	set n [MainMrmlAddNode Volume]
	set v [$n GetID]
	$n SetDescription "Realtime Volume"
	$n SetName        "Realtime"

	# Create the volume
	MainVolumesCreate $v

	LocatorSetRealtime $v

	MainUpdateMRML

	return $v
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetRealtime
# .END
#-------------------------------------------------------------------------------
proc LocatorSetRealtime {v} {
	global Locator Volume

	set Locator(idRealtime) $v
	
	# Change button text, and show file prefix
	if {$v == "NEW"} {
		$Locator(mbRealtime) config -text $v
		set Locator(prefixRealtime) ""
	} else {
		$Locator(mbRealtime) config -text [Volume($v,node) GetName]
		set Locator(prefixRealtime) [MainFileGetRelativePrefix \
			[Volume($v,node) GetFilePrefix]]
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorWrite
# .END
#-------------------------------------------------------------------------------
proc LocatorWrite {data} {
	global Volume Locator

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Locator(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to write."
		return
	}

	switch $data {
		Realtime {set v [LocatorGetRealtimeID]}
	}

	# Show user a File dialog box
	set Locator(prefix$data) [MainFileSaveVolume $v $Locator(prefix$data)]
	if {$Locator(prefix$data) == ""} {return}

	# Write
	MainVolumesWrite $v $Locator(prefix$data)

	# Prefix changed, so update the Volumes->Props tab
	MainVolumesSetActive $v
}

#-------------------------------------------------------------------------------
# .PROC LocatorRead
# .END
#-------------------------------------------------------------------------------
proc LocatorRead {data} {
	global Volume Locator Mrml

	# If the volume doesn't exist yet, then don't read it, duh!
	if {$Locator(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to read."
		return
	}

	switch $data {
		Realtime {set v $Locator(idRealtime)}
	}

	# Show user a File dialog box
	set Locator(prefix$data) [MainFileOpenVolume $v $Locator(prefix$data)]
	if {$Locator(prefix$data) == ""} {return}
	
	# Read
	Volume($v,node) SetFilePrefix $Locator(prefix$data)
	Volume($v,node) SetFullPrefix \
		[file join $Mrml(dir) [Volume($v,node) GetFilePrefix]]
	if {[MainVolumesRead $v] < 0} {
		return
	}

	# Update pipeline and GUI
	MainVolumesUpdate $v

	# Prefix changed, so update the Models->Props tab
	MainVolumesSetActive $v
}

proc LocatorRegisterCallback {cb} {
	global Locator 

	if {[lsearch $Locator(callbackList) $cb] == -1} {
		lappend Locator(callbackList) $cb
	}
}

proc LocatorUnRegisterCallback {cb} {
	global Locator 

	if {[set i [lsearch $Locator(callbackList) $cb]] != -1} {
		set Locator(callbackList) [lreplace Locator(callbackList) $i $i]
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorPause
# .END
#-------------------------------------------------------------------------------
proc LocatorPause {} {
	global Locator

	if {$Locator(pause) == 0 && $Locator(connect) == 1} {
		LocatorLoop$Locator(server)
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorConnect
# .END
#-------------------------------------------------------------------------------
proc LocatorConnect {{value ""}} {
	global Gui  Locator Mrml

	if {$value != ""} {
		set Locator(connect) $value
	}

	# CONNECT
	if {$Locator(connect) == "1"} {

		switch $Locator(server) {

		"File" {
			set filename [file join $Mrml(dir) $Locator(File,prefix)].txt
			if {[catch {set Locator(File,fid) [open $filename r]} errmsg] == 1} {
				puts $errmsg
				tk_messageBox -message $errmsg
				set Locator(loop) 0
				set Locator(connect) 0
				return
			}
			set Locator(loop) 1
			$Locator(mbActive) config -state disabled
			LocatorLoopFile
		}

		"SignaSP" {
			if {$Gui(pc) == 1} {
				tk_messageBox -message "\
The 3D Slicer may connect to a GE SignaSP scanner from a 
Sun UltraSPARC, but not a PC.\n\n\
Set the server to 'Images' to process images on disk as
if they were coming from a scanner in real time."
				set Locator(connect) 0
				return
			}
			set Locator(loop) 1
			$Locator(mbActive) config -state disabled
			LocatorLoopSignaSP
		}

		"Images" {
			# Initialize
			set n Locator(Images,node)
			$n SetFilePrefix $Locator(Images,prefix)
			$n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
			$n SetFilePattern %s.%03d
			set Locator(imageNum) ""

			set Locator(loop) 1
			$Locator(mbActive) config -state disabled
			LocatorLoopImages
		}
		}

	# DISCONNECT
	} else {
		set Locator(loop) 0
		set Locator(imageNum) ""
		$Locator(mbActive) config -state normal

		switch $Locator(server) {

		"File" {
			if {[catch {close $Locator(File,fid)} errmsg] == 1} {
				puts $errmsg
			}
		}

		"SignaSP" {
			# Nothing to do
		}

		"Images" {
			# Nothing to do
		}
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorLoopFile
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopFile {} {
	global Slice Volume Locator

	if {$Locator(loop) == 0} {
		return
	}
	if {$Locator(pause) == 1} {
		return
	}

	# Read matrix
	if {[eof $Locator(File,fid)] == 1} {
		LocatorConnect 0
		return
	}
	gets $Locator(File,fid) line
	scan $line "%d %g %g %g %g %g %g %g %g %g" t nx ny nz tx ty tz px py pz

	if {[info exists pz] == 0} {
		if {[ValidateInt $Locator(File,msPoll)] == 0} {
			set Locator(File,msPoll) 100
		}
		after $Locator(File,msPoll) LocatorLoopFile
		return
	}

	set Locator(nx) $nx
	set Locator(ny) $ny
	set Locator(nz) $nz
	set Locator(tx) $tx
	set Locator(ty) $ty
	set Locator(tz) $tz
	set Locator(px) $px
	set Locator(py) $py
	set Locator(pz) $pz

	LocatorUseLocatorMatrix

	# Render the slices that the locator is driving
	foreach s $Slice(idList) {
		if {[Slicer GetDriver $s] == 1} {
			RenderSlice $s
		}
	}
	Render3D

	# Call update instead of update idletasks so that we
	# process user input like changing slice orientation
	update
	if {[ValidateInt $Locator(File,msPoll)] == 0} {
		set Locator(File,msPoll) 100
	}
	after $Locator(File,msPoll) LocatorLoopFile
}

#-------------------------------------------------------------------------------
# .PROC LocatorLoopImages
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopImages {} {
	global Slice Volume Locator Mrml

	if {$Locator(loop) == 0} {
		return
	}
	if {$Locator(pause) == 1} {
		return
	}

	# Compute next image number
	set v Locator(Images,vol)
	set n Locator(Images,node)
	if {$Locator(imageNum) == ""} {
		set num $Locator(Images,firstNum)
	} else {
		set num [expr $Locator(imageNum) + $Locator(Images,increment)]
		if {$num > $Locator(Images,lastNum)} {
			LocatorConnect 0
			return
		}
	}

	set Locator(imageNum) $num
	$n SetImageRange $num $num

	# Read header
	set filename [format [$n GetFilePattern] [$n GetFilePrefix] $num]
	set errmsg ""
	set errmsg [GetHeaderInfo [file join $Mrml(dir) $filename] $num $n 0]
	# BUG: this should say "!=" but then the image is blank!
	if {$errmsg == ""} {
		puts $errmsg
		puts "Assuming scan order of SI"
		$n ComputeRasToIjkFromScanOrder SI
	}

	# Read image data
	scan [$n GetImageRange] "%d %d" lo hi
	if {[CheckVolumeExists [$n GetFullPrefix] [$n GetFilePattern] $lo $hi] != ""} {
		LocatorConnect 0
		return
	}
	set Gui(progressText) "Reading [$n GetName]"
	$v Read
	$v Update

	# Copy the image to the Realtime volume
	set v [LocatorGetRealtimeID]
	set n Volume($v,node)
	vtkImageCopy copy
	copy SetInput [Locator(Images,vol) GetOutput]
	copy Update
	copy SetInput ""
	Volume($v,vol) SetImageData [copy GetOutput]
	copy SetOutput ""
	copy Delete

	# Set the header info
	$n Copy Locator(Images,node)

	# Update pipeline and GUI
	MainVolumesUpdate $v

	# If this Realtime volume is inside transforms, then
	# compute the registration:
	MainUpdateMRML

	# Perform realtime image processing
	foreach cb $Locator(callbackList) {
		$cb
	}
	
	# Render
	RenderAll

	# Loop
	# Call update instead of update idletasks so that we
	# process user input like changing slice orientation
	update
	if {[ValidateInt $Locator(Images,msPoll)] == 0} {
		set Locator(Images,msPoll) 100
	}
	after $Locator(Images,msPoll) LocatorLoopImages
}

#-------------------------------------------------------------------------------
# .PROC LocatorLoopSignaSP
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopSignaSP {} {
	global Slice Volume Locator
	
	if {$Locator(loop) == 0} {
		return
	}
	if {$Locator(pause) == 1} {
		return
	}

	set status [Locator(SignaSP,src) PollRealtime]
	if {$status == -1} {
		puts "ERROR: PollRealtime"
		return
	}
	set newImage   [Locator(SignaSP,src) GetNewImage]
	set newLocator [Locator(SignaSP,src) GetNewLocator]

	#----------------	
	# NEW LOCATOR
	#----------------
	if {$newLocator != 0} {
		set locStatus [Locator(SignaSP,src) GetLocatorStatus]
		set locMatrix [Locator(SignaSP,src) GetLocatorMatrix]
			
		# Report status to user
		if {$locStatus == 0} {
			set locText "OK"
		} else {
			set locText "BLOCKED"
		}
		$Locator(lLocStatus) config -text $locText

		# Read matrix
		set Locator(px) [$locMatrix GetElement 0 0]
		set Locator(py) [$locMatrix GetElement 1 0]
		set Locator(pz) [$locMatrix GetElement 2 0]
		set Locator(nx) [$locMatrix GetElement 0 1]
		set Locator(ny) [$locMatrix GetElement 1 1]
		set Locator(nz) [$locMatrix GetElement 2 1]
		set Locator(tx) [$locMatrix GetElement 0 2]
		set Locator(ty) [$locMatrix GetElement 1 2]
		set Locator(tz) [$locMatrix GetElement 2 2]

		puts "NEW LOC: P=$Locator(px) $Locator(py) $Locator(pz)"
		LocatorUseLocatorMatrix
	}

	#----------------	
	# NEW IMAGE
	#----------------
	if {$newImage != 0} {
		
		# Force an update so I can read the new matrix
		Locator(SignaSP,src) Modified
		Locator(SignaSP,src) Update
	
		# Update patient position
		set Locator(tblPos)   [lindex $Locator(tblPosList) \
			[Locator(SignaSP,src) GetTablePosition]]
		set Locator(patEntry) [lindex $Locator(patEntryList) \
			[Locator(SignaSP,src) GetPatientEntry]]
		set Locator(patPos)   [lindex $Locator(patPosList) \
			[Locator(SignaSP,src) GetPatientPosition]]
		LocatorSetPatientPosition

		# Get other header values
		set Locator(recon)    [Locator(SignaSP,src) GetRecon]
		set Locator(imageNum) [Locator(SignaSP,src) GetImageNum]
		set minVal [Locator(SignaSP,src) GetMinValue]
		set maxVal [Locator(SignaSP,src) GetMaxValue]
		set imgMatrix [Locator(SignaSP,src) GetImageMatrix]
		puts mat=$imgMatrix
		puts "ima=$Locator(imageNum), recon=$Locator(recon), range=$minVal $maxVal"

		# Copy the image to the Realtime volume
		set v [LocatorGetRealtimeID]
		vtkImageCopy copy
		copy SetInput [Locator(SignaSP,src) GetOutput]
		copy Update
		copy SetInput ""
		Volume($v,vol) SetImageData [copy GetOutput]
		copy SetOutput ""
		copy Delete

		# Set the header info
		set n Volume($v,node)
		$n SetImageRange $Locator(imageNum) $Locator(imageNum)
		$n SetDescription "recon=$Locator(recon)"
		set str [$n GetMatrixToString $imgMatrix]
		$n SetRasToVtkMatrix $str
		$n UseRasToVtkMatrixOn

		# Update pipeline and GUI
		MainVolumesUpdate $v

		# If this Realtime volume is inside transforms, then
		# compute the registration:
		MainUpdateMRML

		# Perform realtime image processing
		foreach cb $Locator(callbackList) {
			$cb
		}
	}

	# Render the slices that the locator is driving.
	if {$newImage != 0 || $newLocator != 0} {
		RenderAll
	}

	# Call update instead of update idletasks so that we
	# process user input like changing slice orientation
	update
	if {[ValidateInt $Locator(SignaSP,msPoll)] == 0} {
		set Locator(SignaSP,msPoll) 100
	}
	after $Locator(SignaSP,msPoll) LocatorLoopSignaSP
}

#-------------------------------------------------------------------------------
# .PROC LocatorFilePrefix
# .END
#-------------------------------------------------------------------------------
proc LocatorFilePrefix {} {
	global Locator Mrml

	# Cannot have blank prefix
	set prefix $Locator(File,prefix)
	if {$prefix == ""} {
		set prefix loc
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"TXT Files" {.txt}}
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "Open File" -defaultextension .txt \
		-filetypes $typelist -initialdir $dir -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return ""}

	# Remember to store it as a relative prefix for next time
	set Locator(File,prefix) [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC LocatorImagesPrefix
# .END
#-------------------------------------------------------------------------------
proc LocatorImagesPrefix {} {
	global Locator Mrml

	# Cannot have blank prefix
	set prefix $Locator(Images,prefix)
	if {$prefix == ""} {
		set prefix I
	}

 	# Show popup initialized to root plus any typed pathname
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "First Image To Process" \
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Store first image file as a relative filename to the root (prefix.001)
	set Locator(Images,firstFile) [MainFileGetRelativePrefix $filename][file \
		extension $filename]

	# Remember to store it as a relative prefix for next time
	set Locator(Images,prefix) [MainFileGetRelativePrefix $filename]

	# Image numbers
	set Locator(Images,firstNum) [MainFileFindImageNumber First \
		[file join $Mrml(dir) $filename]]
	set Locator(Images,lastNum) [MainFileFindImageNumber Last \
		[file join $Mrml(dir) $filename]]
}

#-------------------------------------------------------------------------------
# .PROC LocatorStorePresets
# .END
#-------------------------------------------------------------------------------
proc LocatorStorePresets {p} {
	global Preset Locator Slice

	foreach s $Slice(idList) {
		set Preset(Locator,$p,$s,driver) $Locator($s,driver)
	}
	foreach k "visibility transverseVisibility normalLen transverseLen\
		radius diffuseColor" {
		set Preset(Locator,$p,$k) $Locator($k)
	}
}
	    
proc LocatorRecallPresets {p} {
	global Preset Locator Slice

	foreach s $Slice(idList) {
		LocatorSetDriver $s $Preset(Locator,$p,$s,driver)
	}
	foreach k "visibility transverseVisibility normalLen transverseLen\
		radius diffuseColor" {
		set Locator($k) $Preset(Locator,$p,$k)
	}
	LocatorSetVisibility
	scan $Locator(diffuseColor) "%g %g %g" Locator(red) Locator(green) Locator(blue)
	LocatorSetColor
	LocatorSetSize
	LocatorSetMatrices
}
