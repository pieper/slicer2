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
# FILE:        View.tcl
# DATE:        01/20/2000 09:41
# LAST EDITOR: gering
# PROCEDURES:  
#   ViewInit
#   ViewBuildGUI
#==========================================================================auto=
#
# Use this Module for Changing the Viewing Windows sizes
# and relative positions.
#
#
# KNOWN BUG: Turning off the Close-up window doesn't actually turn it off.
#            It prevents the window from updating, but the close-up window
#            Still gets initialized upon entering a slice.
#
#

#-------------------------------------------------------------------------------
# .PROC ViewInit
# .END
#-------------------------------------------------------------------------------
proc ViewInit {} {
	global View Module

	# Define Tabs
	set m View
	set Module($m,row1List) "Help View"
	set Module($m,row1Name) "Help View"
	set Module($m,row1,tab) View

	# Define Procedures
	set Module($m,procGUI) ViewBuildGUI
}

#-------------------------------------------------------------------------------
# .PROC ViewBuildGUI
# .END
#-------------------------------------------------------------------------------
proc ViewBuildGUI {} {
	global Gui View Module

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# View
	#   Size     : Size of window in 3D mode
	#   Closeup  : Turns on/off Close-up Window
	#   Stereo   : Turns on/off Stereo 3D
	#
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags View $help
	MainHelpBuildGUI View

	#-------------------------------------------
	# View frame
	#-------------------------------------------
	set fView $Module(View,fView)
	set f $fView

	frame $f.fSize    -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fBg      -bg $Gui(activeWorkspace)
	frame $f.fStereo  -bg $Gui(activeWorkspace)
	frame $f.fCloseup -bg $Gui(activeWorkspace)
	pack $f.fSize $f.fBg $f.fCloseup $f.fStereo \
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# View->Bg Frame
	#-------------------------------------------
	set f $fView.fBg
	
    set c {label $f.l -text "Background Color: " $Gui(WLA)}
		eval [subst $c]
	pack $f.l -side left -padx $Gui(pad) -pady 0

	foreach value "Blue Black" width "5 6" {
		set c {radiobutton $f.r$value -width $width \
			-text "$value" -value "$value" -variable View(bgName) \
			-indicatoron 0 -command "MainViewSetBackgroundColor; Render3D" $Gui(WCA)}
			eval [subst $c]
		pack $f.r$value -side left -padx 0 -pady 0
	}
	
	#-------------------------------------------
	# View->Closeup Frame
	#-------------------------------------------
	set f $fView.fCloseup
	
    set c {label $f.lCloseup -text "Closeup Window: " $Gui(WLA)}
		eval [subst $c]
	pack $f.lCloseup -side left -padx $Gui(pad) -pady 0

	foreach value "On Off" width "4 4" {
		set c {radiobutton $f.rCloseup$value -width $width \
			-text "$value" -value "$value" -variable View(closeupVisibility) \
			-indicatoron 0 -command "MainViewSetWelcome Welcome" $Gui(WCA)}
			eval [subst $c]
		pack $f.rCloseup$value -side left -padx 0 -pady 0
	}
	
	#-------------------------------------------
	# View->Size Frame
	#-------------------------------------------
	set f $fView.fSize
	
	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fBtns -bg $Gui(activeWorkspace)
   	pack $f.fTitle $f.fBtns -side top -pady 5

    set c {label $f.fTitle.lTitle -text "Window Size in 3D Mode:" $Gui(WLA)}
		eval [subst $c]
	pack $f.fTitle.lTitle -side left -padx $Gui(pad) -pady 0

    set c {label $f.fBtns.lW -text "Width:" $Gui(WLA)}; eval [subst $c]
    set c {label $f.fBtns.lH -text "Height:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.fBtns.eWidth -width 5 -textvariable View(viewerWidth) $Gui(WEA)}
		eval [subst $c]
	set c {entry $f.fBtns.eHeight -width 5 -textvariable View(viewerHeight) $Gui(WEA)}
		eval [subst $c]
		bind $f.fBtns.eWidth  <Return> {MainViewerSetMode}
		bind $f.fBtns.eHeight <Return> {MainViewerSetMode}
	pack $f.fBtns.lW $f.fBtns.eWidth $f.fBtns.lH $f.fBtns.eHeight \
		-side left -padx $Gui(pad)

	#-------------------------------------------
	# View->Stereo Frame
	#-------------------------------------------
	set f $fView.fStereo
	
	# Stereo button
    set c {checkbutton $f.cStereo \
        -text "Stereo" -variable View(stereo) -width 6 \
        -indicatoron 0 -command "MainViewSetStereo; Render3D" $Gui(WCA)}
        eval [subst $c]
 
	pack $f.cStereo -side top -padx 0 -pady 2

}

