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
# FILE:        Main.tcl
# PROCEDURES:  
#   BootSlicer mrmlFile
#   MainInit
#   MainBuildVTK
#   MainBuildGUI
#   MainBuildModuleTabs ModuleName
#   MainUpdateMRML
#   MainAddActor
#   MainAddModelActor
#   MainRemoveActor
#   MainRemoveModelActor
#   MainSetup
#   IsModule
#   Tab
#   MainStartProgress
#   MainShowProgress
#   MainEndProgress
#   MainMenu
#   MainExitQuery
#   MainSaveMRMLQuery
#   MainExitProgram
#   Distance
#   FormatCVSInfo
#==========================================================================auto=


#-------------------------------------------------------------------------------
# IMPORTANT VARIABLES and Classes
#
# Please add to this list as you find them.
#-------------------------------------------------------------------------------
#  Slicer is a vtkMrmlSlicer Class.
#  Magwin refers to the close-up window in the toolbox.
#  Volume(id,vol) is a Volume Class
#  Volume(id,??) is an array of tons of useful variables for each Volume.
#  Slice(id,??) information about the 3 slice viewing windows. See Slice.tcl


#-------------------------------------------------------------------------------
# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
# Abrieviations:
# ren  renderer
# dir  direction
# nav  navigation
# win  window
# mag  magnification
# cmd  command
# vol  volume
# id   identification number
# cam  camera
# obj  object
# perp perpendicular
# grnd ground
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC BootSlicer
# 
#  Boots the slicer: the first procedure called.
#  <ul>
#  <li>Hides the TK Window
#  <li>Inits global variables
#  <li>Builds VTK Graphics
#  <li>Builds the GUI
#  </ul>
# .ARGS
# str mrmlFile optional name of a MRML file to load 
# .END
#-------------------------------------------------------------------------------
proc MainBoot {{mrmlFile ""}} {
	global Module Gui Dag Anno View Slice viewWin

	# See which module prevents a slice window from rendering
	set checkSliceRender 0

	# Hide default TK window
	wm withdraw .

	#-------------------------------------------
	# Initialize global variables
	#-------------------------------------------
	MainInit

	# Build the Viewer window before doing anything else to see if that helps
	# with the apparent OpenGL bug on Windows98 version 2
	vtkMrmlSlicer Slicer
	Slicer SetFieldOfView $View(fov)

	vtkRenderer viewRen
	lappend Module(Renderers) viewRen
 	set View(viewCam) [viewRen GetActiveCamera]

	MainViewerBuildGUI

	# Module Init
	#-------------------------------------------
	foreach m $Module(idList) {
		if {[info command ${m}Init] != ""} {
			if {$Module(verbose) == 1} {
				puts "INIT: ${m}Init"
			}
			${m}Init
		}
	}

	#-------------------------------------------
	# Check module dependencies.
	#-------------------------------------------
	foreach m $Module(idList) {
		if {[info exists Module($m,depend)] == 1} {
			foreach d $Module($m,depend) {
				if {[lsearch "$Module(idList) $Module(sharedList)" $d] == -1} {
					tk_messageBox -message "\
The '$m' module depends on the '$d' module, which is not present.\n\
The 3D Slicer will exit so the problem can be corrected."
					exit
				}
			}
		}
	}

	#-------------------------------------------
	# Record default presets
	#-------------------------------------------
	foreach m $Module(idList) {
		if {[info exists Module($m,presets)] == 1} {
			MainOptionsParseDefaults $m
		}
	}

	#-------------------------------------------
	# Build VTK Graphics and Imaging Pipelines
	#-------------------------------------------
	MainBuildVTK

	# Module VTK 
	#-------------------------------------------
	foreach m $Module(idList) {
		if {[info exists Module($m,procVTK)] == 1} {
			if {$Module(verbose) == 1} {
				puts "VTK: $m"
			}
			$Module($m,procVTK)
		}
	}

	#-------------------------------------------
	# Build GUI
	#-------------------------------------------
	MainBuildGUI
	
	foreach p "MainViewBuildGUI MainModelsBuildGUI $Module(procGUI)" {
		if {$Module(verbose) == 1} {
			puts "GUI: $p"
		}
		$p
	}
	MainViewSetFov

	# Debuging the slice rendering (no longer necessary)
	if {$checkSliceRender == 1} {
		Anno(0,curBack,actor) SetVisibility 1
		set i 0
	}

	# Module GUI
	#-------------------------------------------
	foreach m $Module(idList) {
		# See if the module has a GUI callback procedure
		if {[info exists Module($m,procGUI)] == 1} {
			if {$Module(verbose) == 1} {
				puts "GUI: $Module($m,procGUI)"
			}
			$Module($m,procGUI)
		}
		if {$checkSliceRender == 1} {
			incr i
			Anno(0,curBack,mapper) SetInput $i
			RenderAll
			tk_messageBox -message "$i $m"
		}
	}
	
	#-------------------------------------------
	# Read user options from Options.xml
	#-------------------------------------------
	set fileName [ExpandPath "Options.xml"]
	if {[CheckFileExists $fileName 0] == "1"} {
	    puts "Reading $fileName"
	    set tags [MainMrmlReadVersion2.0 $fileName]
	    if {$tags != "0"} {
			# Apply the presets immediately rather than 
			# putting them on the tree.
			foreach pair $tags {
				set tag  [lindex $pair 0]
				set attr [lreplace $pair 0 0]
				if {$tag == "Options"} {
					foreach a $attr {
						set key [lindex $a 0]
						set val [lreplace $a 0 0]
						switch $key {
							"options"      {set options $val}
							"program"      {set program $val}
							"contents"     {set contents $val}
						}
					}
					if {$program == "slicer" && $contents == "presets"} {
						MainOptionsParsePresets $attr
						puts "preset=$attr"
					}
				}
			}
	    }
	}

	#-------------------------------------------
	# Load MRML data
	#-------------------------------------------	
	MainMrmlRead $mrmlFile
	MainUpdateMRML

	#-------------------------------------------
	# Initialize the Program State
	#-------------------------------------------
	MainSetup
	RenderAll

	#-------------------------------------------
	# Initial tab
	#-------------------------------------------
	$Gui(lBoot) config -text "Ready!"
	if {$Module(activeID) == ""} {
		Tab [lindex $Module(idList) 0]
	}
	bind .tViewer <Configure> "MainViewerUserResize"
	puts "Ready"
}

#-------------------------------------------------------------------------------
# .PROC MainInit
#
# Sets path names.
# Calls the GuiInit
# Calls all the Init of the Tabs.
# .END
#-------------------------------------------------------------------------------
proc MainInit {} {
	global Module Gui env Path View Anno Mrml
	
	set Module(verbose) 0
	
	set Path(tmpDir) tmp 
	set Path(printHeaderPath) bin/print_header
	set Path(printHeaderFirstWord) print_header
	set Path(remoteHost) forest

	# Set the Mrml(dir) only if the user hasn't done this already
	# (like in the startup script)
	if {[info exists Mrml(dir)] == 0} {
		set Mrml(dir) [pwd]
	}

	GuiInit
	puts "Launching $Gui(title)..."

	# If paths are relative, then make them rooted to Slicer's home
	set Path(printHeaderPath) \
		[file join $Path(program) $Path(printHeaderPath)]
	set Path(tmpDir) \
		[file join $Path(program) $Path(tmpDir)]

	# Initialize Module info
	#-------------------------------------------
	set Module(activeID) ""
	set Module(freezer) ""

	foreach m $Module(idList) {
		set Module($m,more) 0
		set Module($m,row1List) ""
		set Module($m,row1Name) ""
		set Module($m,row1,tab) ""
		set Module($m,row2List) ""
		set Module($m,row2Name) ""
		set Module($m,row2,tab) ""
		set Module($m,row) row1
	}
	set Module(procInit) ""
	set Module(procGUI)  ""
	set Module(procVTK)  ""
	set Module(procMRML) ""
	set Module(procStorePresets) ""
	set Module(procRecallPresets) ""
	# for recording user actions during segmentation trials
	set Module(procSessionLog) ""

	set Module(Renderers) ""

        # Set version info
	lappend Module(versions) [ParseCVSInfo Main \
		{$Revision: 1.58 $} {$Date: 2001/05/27 12:48:23 $}]

	# Call each "Init" routine that's not part of a module
	#-------------------------------------------
	foreach m "$Module(mainList) $Module(sharedList)" {
		if {[info command ${m}Init] != "" && $m != "Main"} {
			if {$Module(verbose) == 1} {
				puts "INIT: ${m}Init"
			}
			${m}Init
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainBuildVTK
#
# Creaters the instance of vtkMrmlSlicer: Slicer
# Inits the Slicer: FieldofView, NoneVolume and LabelWL (a lookup table)
# Creates the vtk Renderer
# Puts purple sphere at origin of 3D window.
# Calls each Tab's BuildVTK: The VTK Pipeline.
# .END
#-------------------------------------------------------------------------------
proc MainBuildVTK {} {
	global Module View Gui Lut

	# Call each "BuildVTK" routine that's not part of a module
	#-------------------------------------------
	foreach p $Module(procVTK) {
		if {$Module(verbose) == 1} {
			puts "VTK: $p"
		}
		$p
	}
	if {$Module(verbose) == 1} {
		puts MainAnnoBuildVTK
	}
	MainAnnoBuildVTK
	
	# Now that the MainLut non-module has built the indirectLUT,
	# I can set it in the Slicer object.
	Slicer SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)
}

#-------------------------------------------------------------------------------
# .PROC MainBuildGUI
#
# This is the Main GUI. 
# It Creates all tabs and packs them appropriately.
# .END
#-------------------------------------------------------------------------------
proc MainBuildGUI {} {
	global Gui fViewBtns viewWin Module Slice View
	
	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Main Window
	#   Modules
	#   Controls
	#     Tabs
	#     Workspace
	#   Display
	#         Right
	#         Left
	#           Image
	#             MagBorder
	#             Nav
	#   Status
	#-------------------------------------------

	#-------------------------------------------
	# Main Window
	#-------------------------------------------
	set f .tMain
	toplevel     $f -bg $Gui(backdrop)
	wm title     $f $Gui(title) 
	wm resizable $f  0 0
	wm geometry  $f +0+0

	# Status bar dimensions

	frame $f.fModules  -bd $Gui(borderWidth) -bg $Gui(backdrop)
	frame $f.fControls -bd $Gui(borderWidth) -bg $Gui(backdrop) -height 420
	frame $f.fDisplay  -bd $Gui(borderWidth) -bg $Gui(backdrop)
	frame $f.fStatus   -bd $Gui(borderWidth) -bg $Gui(inactiveWorkspace) \
		-relief sunken

	# When the mouse enters the display area, show the View controls
	# instead of the welcome image
	bind .tMain.fDisplay <Enter>  "MainViewSetWelcome Controls"

	bind .tMain.fModules  <Enter>  "MainViewSetWelcome Welcome"
	bind .tMain.fControls <Enter>  "MainViewSetWelcome Welcome"
	bind .tMain.fStatus   <Enter>  "MainViewSetWelcome Welcome"

	pack $f.fModules  -side top -expand 1 -fill both -padx 0 -pady 3
	pack $f.fControls -side top -expand 1 -fill both -padx 0 -pady 0
	pack $f.fDisplay  -side top -expand 1 -fill both -padx 0 -pady 0
	pack $f.fStatus   -side top -expand 1 -fill both -padx 0 -pady 0
	pack propagate $f.fControls  false

	#-------------------------------------------
	# System Menu
	#-------------------------------------------
	set f .tMain

	menu .menubar
	# attach it to the main window
	$f config -menu .menubar
	# Create more cascade menus
	foreach m {File View Help} {
		eval {menu .menubar.m$m} $Gui(SMA)
		set Gui(m$m) .menubar.m$m
		.menubar add cascade -label $m -menu .menubar.m$m
	}

	# File menu
	$Gui(mFile) add command -label "Open Scene..." -command \
		"MainMenu File Open"
	$Gui(mFile) add command -label "Save Scene" -command \
		"MainMenu File Save"
	$Gui(mFile) add command -label "Save Scene As..." -command \
		"MainMenu File SaveAs"
	$Gui(mFile) add command -label "Save Scene With Options" -command \
		"MainMenu File SaveWithOptions"
	$Gui(mFile) add separator
	$Gui(mFile) add command -label "Save Current Options" -command \
		"MainMenu File SaveOptions"
	$Gui(mFile) add separator
	$Gui(mFile) add command -label "Save 3D View" -command \
		"MainMenu File Save3D"
	$Gui(mFile) add command -label "Save Active Slice" -command \
		"MainMenu File SaveSlice"
	$Gui(mFile) add command -label "Save 3D View As..." -command \
		"MainMenu File Save3DAs"
	$Gui(mFile) add command -label "Save Active Slice As..." -command \
		"MainMenu File SaveSliceAs"
	$Gui(mFile) add separator
	$Gui(mFile) add command -label "Close" -command \
		"MainMenu File Close"
	$Gui(mFile) add command -label "Exit" -command MainExitQuery

	# View Menu
	$Gui(mView) add command -label "Normal" -command \
		"MainMenu View Normal"
	$Gui(mView) add command -label "3D" -command \
		"MainMenu View 3D"
	$Gui(mView) add command -label "4x512" -command \
		"MainMenu View Quad512"
	$Gui(mView) add command -label "1x512" -command \
		"MainMenu View Single512"
	$Gui(mView) add command -label "4x256" -command \
		"MainMenu View Quad256"
	$Gui(mView) add separator
	$Gui(mView) add command -label "Black" -command \
		"MainViewSetBackgroundColor Black; Render3D"
	$Gui(mView) add command -label "Blue" -command \
		"MainViewSetBackgroundColor Blue; Render3D"
	$Gui(mView) add command -label "Midnight" -command \
		"MainViewSetBackgroundColor Midnight; Render3D"

	# Help menu
	$Gui(mHelp) add command -label "Documentation..." -command \
		"MainMenu Help Documentation"
	$Gui(mHelp) add command -label "Copyright..." -command \
		"MainMenu Help Copyright"
	$Gui(mHelp) add command -label "Version Info..." -command \
		"MainMenu Help Version"
	
	#-------------------------------------------
	# Main->Module Frame
	#-------------------------------------------
	set f .tMain.fModules

	frame $f.fBtns -bg $Gui(backdrop)
	frame $f.fMore -bg $Gui(backdrop)
	pack $f.fBtns $f.fMore -side top -pady 1

	#-------------------------------------------
	# Main->Modules->More frame
	#-------------------------------------------
	set f .tMain.fModules.fMore

	# Have some buttons visible, and hide the rest under "More"
	set cnt 0
	foreach m $Module(idList) {
		set Module($m,more) 0
		if {$cnt > [expr 6 - 1]} {
			set Module($m,more) 1
		}
		incr cnt
	}		

	# Don't make the more button unless we'll use it
	set Module(more) 0
	foreach m $Module(idList) {
		if {$Module($m,more) == 1} {set Module(more) 1}
	}
	if {$Module(more) == 1} {
		eval {menubutton $f.mbMore -text "More:" -relief raised -bd 2 \
			-width 6 -menu $f.mbMore.m} $Gui(WMBA)
			eval {menu $f.mbMore.m} $Gui(WMA)
		set Module(mbMore) $f.mbMore
		eval {radiobutton $f.rMore -width 10 \
			-text "None" -variable Module(moreBtn) -value 1 \
			-command "Tab Menu" -indicatoron 0} $Gui(WCA)
		set Module(rMore) $f.rMore
		pack $f.mbMore $f.rMore -side left -padx $Gui(pad) -pady 0 

		set Module(mbMore) $f.mbMore
		set Module(rMore)  $f.rMore
	}

	#-------------------------------------------
	# Main->Modules->Btns frame
	#-------------------------------------------
	set f .tMain.fModules.fBtns

	set row 0
	if {$Module(more) == 1} {
		set moreMenu $Module(mbMore).m
		$moreMenu delete 0 end
		set firstMore ""
	}
	# Display up to 3 module buttons (m1,m2,m3) on each row 
	foreach {m1 m2 m3} $Module(idList) {
		frame $f.$row -bg $Gui(inactiveWorkspace)

		foreach m "$m1 $m2 $m3" {
			# Either make a button for it, or add it to the "more" menu
			if {$Module($m,more) == 0} {
				eval {radiobutton $f.$row.r$m -width 10 \
					-text "$m" -variable Module(btn) -value $m \
					-command "Tab $m" -indicatoron 0} $Gui(WCA)
				pack $f.$row.r$m -side left -padx 0 -pady 0
			} else {
				if {$firstMore == ""} {
					set firstMore $m
				}
				$moreMenu add command -label $m \
					-command "set Module(btn) More; Tab $m; \
					$Module(rMore) config -text $m"
			}
		}
		pack $f.$row -side top -padx 0 -pady 0

		incr row
	}
	if {$Module(more) == 1} {
		$Module(rMore) config -text "$firstMore"
	}

	#-------------------------------------------
	# Main->Controls Frame
	#-------------------------------------------
	set f .tMain.fControls
	
	frame $f.fTabs -bg $Gui(inactiveWorkspace) -height 20
	frame $f.fWorkspace -bg $Gui(activeWorkspace)
	
	pack $f.fTabs -side top -fill x
	pack $f.fWorkspace -side top -expand 1 -fill both

	set Gui(fTabs) $f.fTabs

	#-------------------------------------------
	# Main->Controls->Tabs Frame
	#-------------------------------------------
	set fWork .tMain.fControls.fWorkspace
	set f .tMain.fControls.fTabs

	foreach m $Module(idList) {
            MainBuildModuleTabs $m
	}
		
	# Blank page to show during boot
	frame $fWork.fBoot -bg $Gui(activeWorkspace)
	eval {label $fWork.fBoot.l -text "Loading data..."} $Gui(WLA)
	set Gui(lBoot) $fWork.fBoot.l
	pack $fWork.fBoot.l -fill both -expand t
	place $fWork.fBoot -in $fWork -relheight 1.0 -relwidth 1.0
	raise $fWork.fBoot

	#-------------------------------------------
	# Main->Display Frame
	#-------------------------------------------
	set f .tMain.fDisplay

	frame $f.fLeft  -bg $Gui(backdrop)
	frame $f.fRight -bg $Gui(backdrop)
	pack $f.fRight $f.fLeft -side left -padx 2 -expand 1 -fill both

	#-------------------------------------------
	# Main->Display->Left Frame
	#-------------------------------------------
	set f .tMain.fDisplay.fLeft

	frame $f.fImage -bg $Gui(inactiveWorkspace) -width 179 -height 179
	pack $f.fImage -side top -padx 3 -pady 3 

	#-------------------------------------------
	# Main->Display->Left->Image Frame
	#-------------------------------------------
	set f .tMain.fDisplay.fLeft.fImage

	foreach name "MagBorder Nav Welcome" {
		frame $f.f$name -bg $Gui(inactiveWorkspace) -bd $Gui(borderWidth) \
			-relief sunken
		set Gui(f$name) $f.f$name
		place $f.f$name -in $f -relwidth 1.0 -relheight 1.0
	}
	raise $Gui(fWelcome)

	#-------------------------------------------
	# Main->Display->Left->Image->Welcome Frame
	#-------------------------------------------
	set f .tMain.fDisplay.fLeft.fImage.fWelcome

	image create photo iWelcome \
		-file [ExpandPath [file join gui "welcome.ppm"]]
	eval {label $f.lWelcome -image iWelcome  \
		-width $Gui(magDim) -height $Gui(magDim) -anchor w} $Gui(WLA)
	pack $f.lWelcome

	#-------------------------------------------
	# Main->Display->Left->Image->MagBorder Frame
	#-------------------------------------------
	set f .tMain.fDisplay.fLeft.fImage.fMagBorder

	if {$View(createMagWin) == "Yes"} {
		MakeVTKImageWindow mag 

		vtkTkImageWindowWidget $f.fMag -iw magWin \
			-width $Gui(magDim) -height $Gui(magDim)  
		bind $f.fMag <Expose> {ExposeTkImageViewer %W %x %y %w %h}
		pack $f.fMag
	}

	#-------------------------------------------
	# Main->Display->Left->Image->Nav Frame
	#-------------------------------------------
	set f .tMain.fDisplay.fLeft.fImage.fNav

	# This is constructed in MainView.tcl

	#-------------------------------------------
	# Main->Display->Right frame
	#-------------------------------------------
	set f .tMain.fDisplay.fRight

	# Exit button
	#-------------------------------------------
	eval {button $f.bExit -text Exit -width 5 \
		-command "MainExitQuery"} $Gui(WBA)
	set Gui(bExit) $f.bExit

	# Opacity Slider
	#-------------------------------------------
	eval {scale $f.sOpacity -from 1.0 -to 0.0 -variable Slice(opacity) \
		-command "MainSlicesSetOpacityAll; RenderAll" \
		-length 80 -resolution 0.1} $Gui(BSA) {-sliderlength 30  \
		-troughcolor [MakeColorNormalized ".7 .7 .9"]}

	# Fade button
	#-------------------------------------------
	eval {checkbutton $f.cFade \
		-text Fade -variable Slice(fade) \
		-width 5 -indicatoron 0 \
		-command "MainSlicesSetFadeAll; RenderAll"} $Gui(WCA)

	pack $f.bExit $f.sOpacity $f.cFade -side top -pady $Gui(pad)

	#-------------------------------------------
	# Main->Status Frame
	#-------------------------------------------
	set f .tMain.fStatus
	
	set Gui(fStatus) $f
	canvas $f.canvas -borderwidth 0 -highlightthickness 0 \
		-width 232 -height 20
	pack $f.canvas

	foreach p "MainAnnoBuildGUI " {
		if {$Module(verbose) == 1} {
			puts "GUI: $p"
		}
		$p
	}
}

#-------------------------------------------------------------------------------
# .PROC MainBuildModuleTabs
# 
# Builds the Tabs for a Module.
#
# .ARGS
# str ModuleName the name of the Module.
# .END
#-------------------------------------------------------------------------------
proc MainBuildModuleTabs {ModuleName}  {
    global Module Gui

    set m $ModuleName
    set fWork .tMain.fControls.fWorkspace
    set f .tMain.fControls.fTabs
    
    # Make page frames for each tab
    foreach tab "$Module($m,row1List) $Module($m,row2List)" {
        frame $fWork.f${m}${tab} -bg $Gui(activeWorkspace)
        place $fWork.f${m}${tab} -in $fWork -relheight 1.0 -relwidth 1.0
        set Module($m,f${tab}) $fWork.f${m}${tab}
    }

    foreach row "row1 row2" {
        # Make tab-row frame for each row
        frame $f.f${m}${row} -bg $Gui(activeWorkspace)
        place $f.f${m}${row} -in $f -relheight 1.0 -relwidth 1.0
        set Module($m,f$row) $f.f${m}${row}

        
        foreach tab $Module($m,${row}List) name $Module($m,${row}Name) {
            set Module($m,b$tab) $Module($m,f$row).b$tab
            eval {button $Module($m,b$tab) -text "$name" \
                    -command "Tab $m $row $tab" \
                    -width [expr [string length "$name"] + 1]} $Gui(TA)
            pack $Module($m,b$tab) -side left -expand 1 -fill both
        }
        
        # "More..." if more than one row exists
        if {$Module($m,row2List) != ""} {
            eval {button $Module($m,f$row).bMore -text "More..." \
                    -command "Tab More"} $Gui(TA)
            pack $Module($m,f$row).bMore -side left -expand 1 -fill both
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MainUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainUpdateMRML {} {
	global Module Label
	
	set verbose $Module(verbose)
	
	# Call each "MRML" routine that's not part of a module
	#-------------------------------------------
	if {$verbose == 1} {puts "MRML: MainMrml"}
	MainMrmlUpdateMRML
	if {$verbose == 1} {puts "MRML: MainColors"}
	MainColorsUpdateMRML
	if {$verbose == 1} {puts "MRML: MainVolumes"}
	MainVolumesUpdateMRML
	if {$verbose == 1} {puts "MRML: MainModels"}
	MainModelsUpdateMRML
	if {$verbose == 1} {puts "MRML: MainMatrices"}
	MainMatricesUpdateMRML

	foreach p $Module(procMRML) {
		if {$verbose == 1} {puts "MRML: $p"}
		$p
	}

	# Call each Module's "MRML" routine
	#-------------------------------------------
	foreach m $Module(idList) {
		if {[info exists Module($m,procMRML)] == 1} {
			if {$verbose == 1} {puts "MRML: $m"}
			$Module($m,procMRML)
		}
	}
}


#-------------------------------------------------------------------------------
# .PROC MainAddActor {actor}
#  Use this method if you want to add an actor that IS NOT A MODEL
#  If your actor is a model, use MainAddModelActor
#
#  With this procedure, the actor is added  to all existing Renderers
#  If you want to add your actor to a specific renderer, for example viewRen 
#  use the vtk call:
#          viewRen AddActor $actor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAddActor { a } {
    global Module 
    
    foreach r $Module(Renderers) {
	$r AddActor $a
    }   
}


#-------------------------------------------------------------------------------
# .PROC MainAddModelActor {modelID}
#
#  Use this method if you want to add an actor that is a model.
#  
#  With this procedure, a different actor for the same model is added to each 
#  existing Renderer. 
#  This allows each renderer to display the same models with different 
#  properties (ie we want the bladder to be visible in the Endoscopic
#  View, but not the MainView)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAddModelActor { m } {
    global Module 
    
    foreach r $Module(Renderers) {
	$r AddActor Model($m,actor,$r)
    }   
}


#-------------------------------------------------------------------------------
# .PROC MainRemoveActor {actor}
#  With this procedure, the actor is removed from all existing Renderers
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainRemoveActor { a } {
	global Module 

    foreach m $Module(Renderers) {
	$m RemoveActor $a
    }
}



#-------------------------------------------------------------------------------
# .PROC MainRemoveModelActor {modelID}
#  With this procedure, every actor for the model is removed from all existing Renderers
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainRemoveModelActor { m } {
	global Module 

    foreach r $Module(Renderers) {
	$r RemoveActor Model($m,actor,$r)
    }
}


#-------------------------------------------------------------------------------
# .PROC MainSetup
# Set many settings to their initial values.
# Called when a scene (mrml file) is opened or closed, and when the
# program starts.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSetup {} {
	global Module Gui Volume Slice View Model Color Matrix Options Preset

	# Set current values to preset 0 (user preferences)
	MainOptionsRecallPresets $Preset(userOptions)

	# Set active volume
	set v [lindex $Volume(idList) 0]
	MainVolumesSetActive $v
		
	# Set FOV
	set dim     [lindex [Volume($v,node) GetDimensions] 0]
	set spacing [lindex [Volume($v,node) GetSpacing] 0]
	set fov     [expr $dim*$spacing]
	set View(fov) $fov
	MainViewSetFov

	# If no volume set in all slices' background, set the active one
	set doit 1 
	foreach s $Slice(idList) {
		if {$Slice($s,backVolID) != 0} { 
			set doit 0 
		}
	}
	if {$doit == 1} {
		MainSlicesSetVolumeAll Back $Volume(activeID)
	}

	# Initialize Slice orientations
	MainSlicesSetOrientAll AxiSagCor

	MainAnnoSetVisibility

	# Active model
	set m [lindex $Model(idList) 0]
	if {$m != ""} {	
		MainModelsSetActive $m
	}

	# Active transform
	set m [lindex $Matrix(idList) 0]
	if {$m != ""} {	
		MainMatricesSetActive $m
	}

	# Active color
	MainColorsSetActive [lindex $Color(idList) 1]

	# Active option
	MainOptionsSetActive [lindex $Options(idList) 0]
}

#-------------------------------------------------------------------------------
# .PROC IsModule
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IsModule {m} {
	global Module

	if {[lsearch $Module(idList) $m] != "-1"} {
		return 1
	}
	return 0
}

#-------------------------------------------------------------------------------
# .PROC Tab
# 
# Command for switching to a new Row or Tab
# Checks to see if we might be frozen -- i.e. not supposed to switch to
# a new frame.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Tab {m {row ""} {tab ""}} {
	global Module Gui View

	# Frozen?
	if {$Module(freezer) != ""} {
		set Module(btn) $Module(activeID)
		set Module(moreBtn) 0
		tk_messageBox -message "Please press the Apply or Cancel button."
		return
	}

	# No modules?
	if {$m == ""} {return}

	# If "More" then switch rows
	if {$m == "More"} {
		set m $Module(activeID)
		set row $Module($m,row)
		switch $row {
			row1 {
				set row row2
				set tab $Module($m,$row,tab)
			}
			row2 {
				set row row1
				set tab $Module($m,$row,tab)
			}
		}
	}

	# If "menu" then use currently selected menu item
	if {$m == "Menu"} {
		set m [$Module(rMore) cget -text]
	}

	# Remember prev
	set prevID $Module(activeID)
	if {$prevID != ""} {
		set prevRow $Module($prevID,row)
		set prevTab $Module($prevID,$prevRow,tab)
	}

	# If no change, do nichts
	if {$m == $prevID} {
		if {$row == $prevRow} {
			if {$tab == $prevTab } {
				return
			}
		}
	}

	# Reset previous tab button
	if {$prevID != ""} {
		$Gui(fTabs).f${prevID}${prevRow}.b${prevTab} config \
			-bg $Gui(backdrop) -fg $Gui(textLight) \
			-activebackground $Gui(backdrop) -activeforeground $Gui(textLight)
	}

	# If no row specified, then use default
	if {$row == ""} {
		set row $Module($m,row)
	}
		
	# If no btn specified, then use default
	if {$tab == ""} {
		set tab $Module($m,$row,tab)
	}

	# Set new
	set Module(activeID) $m
	set Module($m,row) $row
	set Module($m,$row,tab) $tab

	# Show row
	raise $Module($m,f${row})

	# Shrink names of inactive tabs.
	foreach long $Module($m,${row}List) name $Module($m,${row}Name) {
		$Module($m,b$long) config -text "$name" -width \
			[expr [string length "$name"] + 1]
	}

	# Expand name of active tab (only if "name" is shorter)
	set idx [lsearch $Module($m,${row}List) $tab]
	set name [lindex $Module($m,${row}Name) $idx]
	if {[string length $name] < [string length $tab]} {
		set name $tab
	} else {
		set name $name
	}
	$Module($m,b$tab) config -text $name \
		 -width [expr [string length $name] + 1]

	# Activate active tab button	
	$Module($m,b$tab) config -bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-activebackground $Gui(activeWorkspace) \
		-activeforeground $Gui(textDark)

	# Execute Exit procedure (if one exists for the prevID module)
	if {$prevID != $m} {
		if {[info exists Module($prevID,procExit)] == 1} {
			$Module($prevID,procExit)
		}
	}
	
	# Show panel
	raise $Module($m,f$tab)
	set Module(btn) $m
	
	# Give tab the focus.  
	# (make sure entry boxes from other tabs don't keep focus!)
	focus $Module($m,f$tab)
	
	# Execute Entrance procedure
	if {$prevID != $m} {
		if {[info exists Module($m,procEnter)] == 1} {
			$Module($m,procEnter)
		}
	}

	# Toggle more radio button
	if {$Module($m,more) == 1} {
		set Module(moreBtn) 1
	} else {
		set Module(moreBtn) 0
	}
}

#-------------------------------------------------------------------------------
# .PROC MainStartProgress
#
# Does Nothing
#
# .END
#-------------------------------------------------------------------------------
proc MainStartProgress {} {
	global BarId TextId Gui

}

#-------------------------------------------------------------------------------
# .PROC MainShowProgress
#
# Displays progress bar (for when reading off disk, making models, etc.)
# .END
#-------------------------------------------------------------------------------
proc MainShowProgress {filter} {
	global BarId TextId Gui

	set progress [$filter GetProgress]
	set height   [winfo height $Gui(fStatus)]
	set width    [winfo width $Gui(fStatus)]

	if {[info exists BarId] == 1} {
		$Gui(fStatus).canvas delete $BarId
	}
	if {[info exists TextId] == 1} {
		$Gui(fStatus).canvas delete $TextId
	}
       
	set BarId [$Gui(fStatus).canvas create rect 0 0 [expr $progress*$width] \
		$height -fill [MakeColorNormalized ".5 .5 1.0"]]
 
	set TextId [$Gui(fStatus).canvas create text [expr $width/2] \
		[expr $height/3] -anchor center -justify center -text \
		"$Gui(progressText)"]
 
	update idletasks
}

#-------------------------------------------------------------------------------
# .PROC MainEndProgress
#
# Clears the progress bar (for when done reading off disk, etc.)
# 
# .END
#-------------------------------------------------------------------------------
proc MainEndProgress {} {
	global BarId TextId Gui
	
	if {[info exists BarId] == 1} {
		$Gui(fStatus).canvas delete $BarId
	}
	if {[info exists TextId] == 1} {
		$Gui(fStatus).canvas delete $TextId
	}
	set height   [winfo height $Gui(fStatus)]
	set width    [winfo width $Gui(fStatus)]
	set BarId [$Gui(fStatus).canvas create rect 0 0 $width \
		$height -fill [MakeColorNormalized ".7 .7 .7"]]
	update idletasks
}

#-------------------------------------------------------------------------------
# .PROC MainMenu
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMenu {menu cmd} {
    global Gui Module

    set x 50
    set y 50
    
    switch $menu {
	
	"File" {
	    switch $cmd {
		"Open" {
		    MainFileOpenPopup "" 50 50
		}
		"Save" {
		    MainFileSave
		}
		"SaveAs" {
		    MainFileSaveAsPopup "" 50 50
		}
		"SaveWithOptions" {
		    MainFileSaveWithOptions
		}
		"SaveOptions" {
		    MainFileSaveOptions
		}
		"Save3D" {
		    MainViewSaveView
		}
		"SaveSlice" {
		    MainSlicesSave
		}
		"Save3DAs" {
		    MainViewSaveViewPopup
		}
		"SaveSliceAs" {
		    MainSlicesSavePopup
		}
		"Close" {
		    MainFileClose
		}
	    }
	}
	
	"Help" {
	    switch $cmd {
		"Copyright" {
		    MsgPopup Copyright $x $y "\
(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED \"AS IS.\"  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
		}
		"Documentation" {
		    MsgPopup Documentation $x $y "\
For the latest documentation, visit:

http://www.slicer.org"
		}
		"Version" {
		    set msg [FormatCVSInfo $Module(versions)]
		    MsgPopup Version $x $y $msg {Module Version Info}
		}
	    }
	}
	"View" {
	    MainViewerSetMode $cmd
	}  
    }
}

#-------------------------------------------------------------------------------
# .PROC MainExitQuery
#
# Does the user really want to exit?
# .END
#-------------------------------------------------------------------------------
proc MainExitQuery { } {
	global Gui Volume Model

	# See if any models or volumes are unsaved
	set volumes ""
	foreach v $Volume(idList) {
		if {[info exists Volume($v,dirty)] == 1} {
			if {$Volume($v,dirty) == 1} {
				if {$volumes == ""} {
					set volumes "[Volume($v,node) GetName]"
				} else {
					set volumes "${volumes}, [Volume($v,node) GetName]"
				}
			}
		}
	}
	set models ""
	foreach v $Model(idList) {
		if {[info exists Model($v,dirty)] == 1} {
			if {$Model($v,dirty) == 1} {
				if {$models == ""} {
					set models "[Model($v,node) GetName]"
				} else {
					set models "${models}, [Model($v,node) GetName]"
				}
			}
		}
	}

	if {[llength "$models $volumes"] == 0} {
		MainExitProgram
	}

	if {$volumes != ""} {
		set msg "\
The image data for the following volumes are unsaved:\n\
$volumes\n\n"
	}
	if {$models != ""} {
		set msg "\
The polygon data for the following surface models are unsaved:\n\
$models\n\n"
	}
	set msg "${msg}Exit anyway?" 

#	set x 0
#	set y [expr [winfo rooty $Gui(bExit)] - 60]
	set x 20
	set y 50
	YesNoPopup Exit $x $y $msg MainExitProgram 
}

#-------------------------------------------------------------------------------
# .PROC MainSaveMRMLQuery 
#
#
# Save the Mrml File?
# THIS IS CURRENTLY NOT USED
# .END
#-------------------------------------------------------------------------------
proc MainSaveMRMLQuery { } {
	global Gui
	
	# See if Dag is unsaved
	if {$Gui(unsavedDag) != 0} {
		# set x [expr [winfo rootx $Gui(bExit)] - 60]
		# set y [expr [winfo rooty $Gui(bExit)] - 85]
		set x 0
		set y 0
		YesNoPopup SaveMRML $x $y \
			"Do you want to save changes\n\
			 made to the MRML file?" \
			"FileSaveDag; MainExitProgram" MainExitProgram
	} else {
		MainExitProgram
	}
}

#-------------------------------------------------------------------------------
# .PROC MainExitProgram
#
#  Exit the Program
# .END
#-------------------------------------------------------------------------------
proc MainExitProgram { } {
    global Module
	
    # logging
    if {[IsModule SessionLog] == 1} {
	# Execute Exit procedure (if one exists for the prevID module)
	# This is so that it can log anything final it should log.
	set prevID $Module(activeID)
	if {[info exists Module($prevID,procExit)] == 1} {
	    $Module($prevID,procExit)
	}

	# write out the log file if we are logging
	SessionLogEndSession
    }
    # end logging

#### Turn these lines on if you want to see what classes have not yet been
#### deleted. This is vtk3.2 only. it also requires special compilation 
#### of vtkObjectFactory and vtkObject
#### see: http://www.kitware.com/vtkhtml/vtkdata/html/class_vtkdebugleaks.html
#    vtkDebugLeaks DebugLeaks
#    DebugLeaks PrintCurrentLeaks
    exit
}

#-------------------------------------------------------------------------------
# .PROC Distance
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Distance {aArray bArray} {

	upvar $aArray a
	upvar $bArray b

	set x [expr $a(r) - $b(r)]
	set y [expr $a(a) - $b(a)]
	set z [expr $a(s) - $b(s)]
	
	return [expr sqrt($x*$x + $y*$y + $z*$z)]
}

#-------------------------------------------------------------------------------
# Normalize a = |a|
#-------------------------------------------------------------------------------
proc Normalize {aArray} {
	upvar $aArray a

	set d [expr sqrt($a(x)*$a(x) + $a(y)*$a(y) + $a(z)*$a(z))]

	if {$d == 0} {
		return
	}
	set a(x) [expr $a(x) / $d]
	set a(y) [expr $a(y) / $d]
	set a(z) [expr $a(z) / $d]
}

#-------------------------------------------------------------------------------
# Cross a = b x c 
#-------------------------------------------------------------------------------
proc Cross {aArray bArray cArray} {
	upvar $aArray a
	upvar $bArray b
	upvar $cArray c

	set a(x) [expr $b(y)*$c(z) - $c(y)*$b(z)]
	set a(y) [expr $c(x)*$b(z) - $b(x)*$c(z)]
	set a(z) [expr $b(x)*$c(y) - $c(x)*$b(y)]
}

# Remove $ and spaces from CVS version info
proc ParseCVSInfo {module args} {
    set l $module 
    foreach a $args {
	lappend l [string trim $a {$ \t}]
    }
    return $l
}

#-------------------------------------------------------------------------------
# .PROC FormatCVSInfo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FormatCVSInfo {versions} {
    set s ""
    foreach v "$versions" {
	set s [format "%s%-30s" $s "[lindex $v 0]:"]
	set s "${s}\t[lindex $v 1]\t\t[lindex $v 2]\n"
    }
    return $s
}
