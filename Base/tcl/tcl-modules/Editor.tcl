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
# FILE:        Editor.tcl
# DATE:        01/20/2000 09:41
# LAST EDITOR: gering
# PROCEDURES:  
#   EditorInit
#   EditorBuildVTK
#   EditorUpdateMRML
#   EditorBuildGUI
#   EditorSetPrefix
#   EditorEnter
#   EditorB1
#   EditorB1Motion
#   EditorB1Release
#   EditorChangeInputLabel
#   EditorSetOriginal
#   EditorSetWorking
#   EditorSetComposite
#   EditorUpdateEffect
#   EditorGetOriginalID
#   EditorGetWorkingID
#   EditorGetCompositeID
#   EditorResetDisplay
#   EditorSetEffect
#   EditorGetInputID
#   EditorActivateUndo
#   EditorUpdateAfterUndo
#   EditorUndo
#   EdBuildScopeGUI
#   EdBuildInputGUI
#   EdBuildInteractGUI
#   EdBuildRenderGUI
#   EdSetupBeforeApplyEffect
#   EdUpdateAfterApplyEffect
#   EditorClearOutput
#   EditorMerge
#   EditorWriteOutput
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EditorInit
# .END
#-------------------------------------------------------------------------------
proc EditorInit {} {
	global Editor Ed Gui Volume Module env Path

	# Define Tabs
	set m Editor
	set Module($m,row1List) "Help Effects Details Output"
	set Module($m,row1Name) "Help Effects Details Output"
	set Module($m,row1,tab) Effects

	# Define Procedures
	set Module($m,procGUI)   EditorBuildGUI
	set Module($m,procMRML)  EditorUpdateMRML
	set Module($m,procVTK)   EditorBuildVTK
	set Module($m,procEnter) EditorEnter

	# Define Dependencies
	set Module($m,depend) "Labels"

	# Initialize globals
	set Editor(idOriginal)  $Volume(idNone)
	set Editor(idWorking)   NEW
	set Editor(idComposite) NEW
	set Editor(undoActive)  0
	set Editor(effectMore)  0
	set Editor(activeID)    TBD
	set Editor(firstReset)  0
	set Editor(prefixComposite) ""
	set Editor(prefixWorking) ""

	# Look for Editor effects and form an array, Ed, for them.
	# Each effect has a *.tcl file in the tcl-modules/Editor directory.
	set Ed(idList) ""

	set prog $Path(program)
	set dir  [file join [file join $prog tcl-modules] Editor]
	# fullname is the full path name
	foreach fullname [glob -nocomplain $dir/*.tcl] {
		if {[regexp "$dir/(\.*).tcl" $fullname match name] == 1} {
			lappend Ed(idList) $name
			# If there's an error, print the fullname:
			if {[catch {source $fullname} errmsg] == 1} {
				puts "ERROR in $fullname:\n $errmsg"
				source $fullname
			}
		}
	}

	# Initialize effects
	if {$Module(verbose) == 1} {
		puts Editor-Init:
	}
	foreach m $Ed(idList) {
		if {[info command ${m}Init] != ""} {
			if {$Module(verbose) == 1} {
				puts ${m}Init
			}
			${m}Init
		}
	}

	# Order effects by increasing rank
	set pairs ""
	foreach m $Ed(idList) {
		lappend pairs "$m $Ed($m,rank)"
	}
	set pairs [lsort -index 1 -integer -increasing $pairs]
	set Ed(idList) ""
	foreach p $pairs {
		lappend Ed(idList) [lindex $p 0]
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorBuildVTK
# .END
#-------------------------------------------------------------------------------
proc EditorBuildVTK {} {
	global Editor Ed Module

	vtkImageEditorEffects Ed(editor)
	Ed(editor) SetStartMethod     MainStartProgress
	Ed(editor) SetProgressMethod "MainShowProgress Ed(editor)"
	Ed(editor) SetEndMethod       MainEndProgress

	# Initialize effects
	if {$Module(verbose) == 1} {
		puts Editor-VTK:
	}
	foreach e $Ed(idList) {
		if {[info exists Ed($e,procVTK)] == 1} {
			if {$Module(verbose) == 1} {
				puts $Ed($e,procVTK)
			}
			$Ed($e,procVTK)
		}
	}

	# Node to store for undo
	vtkMrmlVolumeNode Editor(undoNode)
}

#-------------------------------------------------------------------------------
# .PROC EditorUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc EditorUpdateMRML {} {
	global Volume Editor

	# See if the volume for each menu actually exists.
	# If not, use the None volume
	#
	set n $Volume(idNone)
	if {[lsearch $Volume(idList) $Editor(idOriginal)] == -1} {
		EditorSetOriginal $n
	}
	if {$Editor(idWorking) != "NEW" && \
		[lsearch $Volume(idList) $Editor(idWorking)] == -1} {
		EditorSetWorking NEW
	}
	if {$Editor(idComposite) != "NEW" && \
		[lsearch $Volume(idList) $Editor(idComposite)] == -1} {
		EditorSetComposite NEW
	}

	# Original Volume menu
	#---------------------------------------------------------------------------
	set m $Editor(mOriginal)
	$m delete 0 end
	foreach v $Volume(idListForMenu) {
		$m add command -label [Volume($v,node) GetName] -command \
			"EditorSetOriginal $v; RenderAll"
	}

	# Working Volume menu
	#---------------------------------------------------------------------------
	set m $Editor(mWorking)
	$m delete 0 end
	set idWorking ""
	foreach v $Volume(idListForMenu) {
		if {$v != $Volume(idNone) && $v != $Editor(idComposite)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"EditorSetWorking $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Working"} {
			set idWorking $v
		}
	}
	# If there is working, then select it, else add a NEW option
	if {$idWorking != ""} {
		EditorSetWorking $idWorking
	} else {
		$m add command -label NEW -command "EditorSetWorking NEW; RenderAll"
	}

	# Composite Volume menu
	#---------------------------------------------------------------------------
	set m $Editor(mComposite)
	$m delete 0 end
	set idComposite ""
	foreach v $Volume(idListForMenu) {
		if {$v != $Volume(idNone) && $v != $Editor(idWorking)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"EditorSetComposite $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Composite"} {
			set idComposite $v
		}
	}
	# If there is working, then select it, else add a NEW option
	if {$idComposite != ""} {
		EditorSetComposite $idComposite
	} else {
		$m add command -label NEW -command "EditorSetComposite NEW; RenderAll"
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EditorBuildGUI {} {
	global Gui Volume Editor Ed Module Slice Path

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Effects
	#   Effects
	#     Btns
	#     More
	#     Undo
	#   Original
	#   Working
	#   Active
	#   Time
	# Details
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Editor $help
	MainHelpBuildGUI Editor

	############################################################################
	#                                 Effects
	############################################################################

	#-------------------------------------------
	# Effects frame
	#-------------------------------------------
	set fEffects $Module(Editor,fEffects)
	set f $fEffects

	frame $f.fEffects   -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fOriginal  -bg $Gui(activeWorkspace)
	frame $f.fWorking   -bg $Gui(activeWorkspace)
	frame $f.fComposite -bg $Gui(activeWorkspace)
	frame $f.fActive    -bg $Gui(activeWorkspace)
	frame $f.fTime      -bg $Gui(activeWorkspace)
	pack $f.fEffects $f.fOriginal $f.fWorking $f.fComposite $f.fActive $f.fTime \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Effects->Original
	#-------------------------------------------
	set f $fEffects.fOriginal

	# Volume menu
	set c {label $f.lOriginal -text "Original Volume:" $Gui(WLA)}; eval [subst $c]

	set c {menubutton $f.mbOriginal -text "None" -relief raised -bd 2 -width 18 \
		-menu $f.mbOriginal.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbOriginal.m $Gui(WMA)}; eval [subst $c]
	pack $f.lOriginal -padx $Gui(pad) -side left -anchor e
	pack $f.mbOriginal -padx $Gui(pad) -side left -anchor w

	# Save widgets for changing
	set Editor(mbOriginal) $f.mbOriginal
	set Editor(mOriginal)  $f.mbOriginal.m

	#-------------------------------------------
	# Effects->Working
	#-------------------------------------------
	set f $fEffects.fWorking

	# Volume menu
	set c {label $f.lWorking -text "Working Volume:" $Gui(WLA)}; eval [subst $c]

	set c {menubutton $f.mbWorking -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbWorking.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbWorking.m $Gui(WMA)}; eval [subst $c]
	pack $f.lWorking $f.mbWorking -padx $Gui(pad) -side left

	# Save widgets for changing
	set Editor(mbWorking) $f.mbWorking
	set Editor(mWorking)  $f.mbWorking.m

	#-------------------------------------------
	# Effects->Composite
	#-------------------------------------------
	set f $fEffects.fComposite

	# Volume menu
	set c {label $f.lComposite -text "Composite Volume:" $Gui(WLA)}; eval [subst $c]

	set c {menubutton $f.mbComposite -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbComposite.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbComposite.m $Gui(WMA)}; eval [subst $c]
	pack $f.lComposite $f.mbComposite -padx $Gui(pad) -side left

	# Save widgets for changing
	set Editor(mbComposite) $f.mbComposite
	set Editor(mComposite)  $f.mbComposite.m

	#-------------------------------------------
	# Effects->Active frame
	#-------------------------------------------
	set f $fEffects.fActive

	set c {label $f.lActive -text "Active Slice:" $Gui(WLA)}; eval [subst $c]
	pack $f.lActive -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach s $Slice(idList) text "Red Yellow Green" width "4 7 6" {
		set c {radiobutton $f.r$s -width $width -indicatoron 0\
			-text "$text" -value "$s" -variable Slice(activeID) \
			-command "MainSlicesSetActive" $Gui(WCA) -selectcolor  $Gui(slice$s)}
			eval [subst $c]
		pack $f.r$s -side left -fill x -anchor e
	}

	#-------------------------------------------
	# Effects->Time frame
	#-------------------------------------------
	set f $fEffects.fTime

	set c {label $f.lRun -text "Run time:" $Gui(WLA)}; eval [subst $c]
	set c {label $f.lRunTime -text "0 sec," $Gui(WLA)}; eval [subst $c]
	set c {label $f.lTotal -text "Total:" $Gui(WLA)}; eval [subst $c]
	set c {label $f.lTotalTime -text "0 sec" $Gui(WLA)}; eval [subst $c]
	pack $f.lRun $f.lRunTime $f.lTotal $f.lTotalTime \
		-side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	set Editor(lRunTime) $f.lRunTime
	set Editor(lTotalTime) $f.lTotalTime

	#-------------------------------------------
	# Effects->Effects
	#-------------------------------------------
	set f $fEffects.fEffects

	frame $f.fBtns -bg $Gui(activeWorkspace)
	frame $f.fMore -bg $Gui(activeWorkspace)
	frame $f.fUndo -bg $Gui(activeWorkspace)
	pack $f.fBtns $f.fMore $f.fUndo -side top -pady $Gui(pad)

	#-------------------------------------------
	# Effects->Effects->More frame
	#-------------------------------------------
	set f $fEffects.fEffects.fMore

	# Have 10 effects visible, and hide the rest under "More"
	set cnt 0
	foreach e $Ed(idList) {
		set Ed($e,more) 0
		if {$cnt > [expr 10 - 1]} {
			set Ed($e,more) 1
		}
		incr cnt
	}		

	# Don't make the more button unless we'll use it
	set Editor(more) 0
	foreach e $Ed(idList) {
		if {$Ed($e,more) == 1} {set Editor(more) 1}
	}
	if {$Editor(more) == 1} {
		set c {menubutton $f.mbMore -text "More:" -relief raised -bd 2 \
			-width 6 -menu $f.mbMore.m $Gui(WMBA)}; eval [subst $c]
			set c {menu $f.mbMore.m $Gui(WMA)}; eval [subst $c]
		set c {radiobutton $f.rMore -width 13 \
			-text "None" -variable Editor(moreBtn) -value 1 \
			-command "EditorSetEffect Menu" -indicatoron 0 $Gui(WCA)}; eval [subst $c]
		pack $f.mbMore $f.rMore -side left -padx $Gui(pad) -pady 0 

		set Editor(mbMore) $f.mbMore
		set Editor(rMore)  $f.rMore
	}

	#-------------------------------------------
	# Effects->Effects->Btns frame
	#-------------------------------------------
	set f $fEffects.fEffects.fBtns

	set row 0
	if {$Editor(more) == 1} {
		set moreMenu $Editor(mbMore).m
		$moreMenu delete 0 end
		set firstMore ""
	}
	# Display up to 2 effect buttons (e1,e2) on each row 
	foreach {e1 e2} $Ed(idList) {
		frame $f.$row -bg $Gui(inactiveWorkspace)

		foreach e "$e1 $e2" {
			# Either make a button for it, or add it to the "more" menu
			if {$Ed($e,more) == 0} {
				set c {radiobutton $f.$row.r$e -width 13 \
					-text "$Ed($e,name)" -variable Editor(btn) -value $e \
					-command "EditorSetEffect $e" -indicatoron 0 $Gui(WCA)}
					eval [subst $c]
				pack $f.$row.r$e -side left -padx 0 -pady 0
			} else {
				if {$firstMore == ""} {
					set firstMore $e
				}
				$moreMenu add command -label $Ed($e,name) \
					 -command "EditorSetEffect $e"
			}
		}
		pack $f.$row -side top -padx 0 -pady 0

		incr row
	}
	if {$Editor(more) == 1} {
		$Editor(rMore) config -text "$firstMore"
	}

	#-------------------------------------------
	# Effects->Effects->Undo frame
	#-------------------------------------------
	set f $fEffects.fEffects.fUndo

	set c {button $f.bUndo -text "Undo last effect" -width 17 \
		-command "EditorUndo; RenderAll" $Gui(WBA) -state disabled}; 
		eval [subst $c]
	pack $f.bUndo -side left -padx $Gui(pad) -pady 0
	
	set Editor(bUndo) $f.bUndo


	############################################################################
	#                                 Details
	############################################################################

	#-------------------------------------------
	# Details frame
	#-------------------------------------------
	set fDetails $Module(Editor,fDetails)
	set f $fDetails

	frame $f.fTitle  -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fEffect -bg $Gui(activeWorkspace)
	pack $f.fTitle -side top -pady 2 -padx 2 -fill x
	pack $f.fEffect -side top -pady 2 -padx 2 -fill both -expand 1

	#-------------------------------------------
	# Details->Title frame
	#-------------------------------------------
	set f $fDetails.fTitle

	set c {label $f.lName -text "None" $Gui(WLA)}; eval [subst $c]
	set c {label $f.lDesc -text "Does nothing." $Gui(WLA)}; eval [subst $c]
	pack $f.lName $f.lDesc -pady 0

	set Editor(lEffectName) $f.lName
	set Editor(lEffectDesc) $f.lDesc

	#-------------------------------------------
	# Details->Effect frame
	#-------------------------------------------
	set f $fDetails.fEffect

	foreach e $Ed(idList) {
		frame $f.f$e -bg $Gui(activeWorkspace)
		place $f.f$e -in $f -relheight 1.0 -relwidth 1.0
		set Ed($e,frame) $f.f$e
	}

	############################################################################
	#                                 Effects
	############################################################################

	if {$Module(verbose) == 1} {
		puts Editor-GUI:
	}
	foreach e $Ed(idList) {
		if {[info exists Ed($e,procGUI)] == 1} {
			if {$Module(verbose) == 1} {
				puts $Ed($e,procGUI)
			}
			$Ed($e,procGUI)
		}
	}
	
	EditorSetEffect EdNone


	############################################################################
	#                                 Output
	############################################################################

	#-------------------------------------------
	# Output frame
	#-------------------------------------------
	set fOutput $Module(Editor,fOutput)
	set f $fOutput

	frame $f.fWorking   -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fComposite -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fMerge     -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fWorking $f.fComposite $f.fMerge \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Output->Working
	#-------------------------------------------
	set f $fOutput.fWorking

	frame $f.f -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	set c {label $f.f.l -text "Working Volume" $Gui(WTA)}
		eval [subst $c]
	eval {button $f.f.b -text "Browse..." -width 10 \
		-command "EditorSetPrefix Working"} $Gui(WBA)
	pack $f.f.l $f.f.b -side left -padx $Gui(pad)

	#-------------------------------------------
	# Output->Working->Prefix
	#-------------------------------------------
	set f $fOutput.fWorking.fPrefix

	set c {entry $f.e \
		-textvariable Editor(prefixWorking) $Gui(WEA)}; eval [subst $c]
	pack $f.e -padx $Gui(pad) -side left -expand 1 -fill x

	#-------------------------------------------
	# Output->Working->Btns
	#-------------------------------------------
	set f $fOutput.fWorking.fBtns

	set c {button $f.bWrite -text "Write" -width 6 \
		-command "EditorWriteOutput Working; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bClear -text "Clear" -width 6 \
		-command "EditorClearOutput Working; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite $f.bClear -side left -padx $Gui(pad)

	#-------------------------------------------
	# Output->Composite
	#-------------------------------------------
	set f $fOutput.fComposite

	frame $f.f -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	set c {label $f.f.l -text "Composite Volume" $Gui(WTA)}
		eval [subst $c]
	eval {button $f.f.b -text "Browse..." -width 10 \
		-command "EditorSetPrefix Composite"} $Gui(WBA)
	pack $f.f.l $f.f.b -side left -padx $Gui(pad)

	#-------------------------------------------
	# Output->Composite->Prefix
	#-------------------------------------------
	set f $fOutput.fComposite.fPrefix

	set c {entry $f.e \
		-textvariable Editor(prefixComposite) $Gui(WEA)}; eval [subst $c]
	pack $f.e -padx $Gui(pad) -side left -expand 1 -fill x

	#-------------------------------------------
	# Output->Composite->Btns
	#-------------------------------------------
	set f $fOutput.fComposite.fBtns

	set c {button $f.bWrite -text "Write" -width 6 \
		-command "EditorWriteOutput Composite; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bClear -text "Clear" -width 6 \
		-command "EditorClearOutput Composite; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite $f.bClear -side left -padx $Gui(pad)

	#-------------------------------------------
	# Output->Merge
	#-------------------------------------------
	set f $fOutput.fMerge

	set c {label $f.lTitle -text "Merge with the Composite." $Gui(WTA)}
		eval [subst $c]
	set c {button $f.bMergeWorking -text "Write Working over Composite" \
		-width 29 -command "EditorMerge Working 1; RenderAll" $Gui(WBA)}
		eval [subst $c]
	set c {button $f.bMergeComposite -text "Write Composite over Working" \
		-width 29 -command "EditorMerge Working 0; RenderAll" $Gui(WBA)}
		eval [subst $c]
	set c {button $f.bMergeOriginal -text "Write Original over Composite" \
		-width 29 -command "EditorMerge Original 1; RenderAll" $Gui(WBA)}
		eval [subst $c]
	pack $f.lTitle $f.bMergeWorking $f.bMergeComposite $f.bMergeOriginal \
		-side top -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC EditorSetPrefix
# .END
#-------------------------------------------------------------------------------
proc EditorSetPrefix {data} {
	global Editor Mrml

	# Cannot have blank prefix
	if {$Editor(prefix$data) == ""} {
		set Editor(prefix$data) [Uncap $data]
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $Editor(prefix$data)]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Volume" \
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Store it as a relative prefix for next time
	set Editor(prefix$data) [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC EditorEnter
# .END
#-------------------------------------------------------------------------------
proc EditorEnter {} {
	global Editor Volume Slice

	if {[EditorGetOriginalID] == $Volume(idNone)} {
		set v [[[Slicer GetBackVolume $Slice(activeID)] GetMrmlNode] GetID]
		if {$v != $Volume(idNone)} {
			EditorSetOriginal $v
		}
	}
}


################################################################################
#                             Event Bindings
################################################################################


#-------------------------------------------------------------------------------
# .PROC EditorB1
# .END
#-------------------------------------------------------------------------------
proc EditorB1 {x y} {
	global Ed Editor

	switch $Editor(activeID) {
	"EdDraw" {
		# Mark point for moving
		Slicer DrawMoveInit $x $y

		# Act depending on the draw mode:
		#  - Draw:   Insert a point
		#  - Select: Select/deselect a point
		#
		switch $Ed(EdDraw,mode) {
		"Draw" {
			Slicer DrawInsertPoint $x $y
		}
		"Select" {
			Slicer DrawStartSelectBox $x $y
		}
		}
	}
	"EdChangeIsland" {
		EditorChangeInputLabel $x $y
	}
	"EdMeasureIsland" {
		EditorChangeInputLabel $x $y
	}
	"EdSaveIsland" {
		EditorChangeInputLabel $x $y
	}
	"EdChangeLabel" {
		EditorChangeInputLabel $x $y
	}
	"EdRemoveIslands" {
		EditorChangeInputLabel $x $y
	}
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorB1Motion
# .END
#-------------------------------------------------------------------------------
proc EditorB1Motion {x y} {
	global Ed Editor Slice Interactor

	set s $Slice(activeID)

	switch $Editor(activeID) {
	"EdDraw" {
		# Act depending on the draw mode:
		#  - Move:   move points
		#  - Draw:   Insert a point
		#  - Select: draw the "select" box
		#
		switch $Ed(EdDraw,mode) {
		"Draw" {
			Slicer DrawInsertPoint $x $y

#			Slicer SetReformatPoint $s $x $y
#			scan [Slicer GetIjkPoint] "%g %g %g" i j k
#			puts "Slicer DrawInsertPoint $x $y ijk=$i $j $k s=$s"
		}
		"Select" {
			Slicer DrawDragSelectBox $x $y
		}
		"Move" {
			Slicer DrawMove $x $y
		}
		}
	}
	}

}

#-------------------------------------------------------------------------------
# .PROC EditorB1Release
# .END
#-------------------------------------------------------------------------------
proc EditorB1Release {x y} {
	global Ed Editor

	switch $Editor(activeID) {
	"EdDraw" {
		# Act depending on the draw mode:
		#  - Select: stop drawing the "select" box
		#
		switch $Ed(EdDraw,mode) {
		"Select" {
			Slicer DrawEndSelectBox $x $y
		}
		}
	}
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorChangeInputLabel
# .END
#-------------------------------------------------------------------------------
proc EditorChangeInputLabel {x y} {
	global Ed Editor Label

	set e $Editor(activeID)

	# Determine the input label
	set s [Slicer GetActiveSlice]
	if {[info exists Ed($e,input)] == 1 && $Ed($e,input) == "Original"} {
		set Ed($e,inputLabel) [Slicer GetBackPixel $s $x $y]
	} else {
		set Ed($e,inputLabel) [Slicer GetForePixel $s $x $y]
	}

	# Get the seed
	set s [Slicer GetActiveSlice]
	Slicer SetReformatPoint $s $x $y
	if {$Ed($e,scope) == "Single"} {
		scan [Slicer GetSeed2D] "%d %d %d" xSeed ySeed zSeed
	} else {
		scan [Slicer GetSeed] "%d %d %d" xSeed ySeed zSeed
	}
	set Ed($e,xSeed) $xSeed
	set Ed($e,ySeed) $ySeed
	set Ed($e,zSeed) $zSeed

	# Apply the effect
	${e}Apply
}


################################################################################
#                             EFFECTS
################################################################################


#-------------------------------------------------------------------------------
# .PROC EditorSetOriginal
# .END
#-------------------------------------------------------------------------------
proc EditorSetOriginal {v} {
	global Editor Volume

	if {$v == $Editor(idWorking)} {
		tk_messageBox -message "The Original and Working volumes must differ."
		return
	}
	set Editor(idOriginal) $v
	
	# Change button text
    $Editor(mbOriginal) config -text [Volume($v,node) GetName]

	if {$Editor(activeID) != "EdNone"} {
		# Display the origin as the background on the slices
		EditorResetDisplay

		# Refresh the effect, if it's an interactive one
		EditorUpdateEffect
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorSetWorking
# .END
#-------------------------------------------------------------------------------
proc EditorSetWorking {v} {
	global Editor Volume

	if {$v == [EditorGetOriginalID]} {
		tk_messageBox -message "The Original and Working volumes must differ."
		return
	}
	if {$v == $Editor(idComposite) && $v != "NEW"} {
		tk_messageBox -message "The Composite and Working volumes must differ."
		return
	}
	set Editor(idWorking) $v
	
	# Change button text
	if {$v == "NEW"} {
		$Editor(mbWorking) config -text $v
	} else {
		$Editor(mbWorking) config -text [Volume($v,node) GetName]
	}

	# Refresh the effect, if it's an interactive one
	EditorUpdateEffect
}

#-------------------------------------------------------------------------------
# .PROC EditorSetComposite
# .END
#-------------------------------------------------------------------------------
proc EditorSetComposite {v} {
	global Editor Volume

	if {$v == [EditorGetOriginalID]} {
		tk_messageBox -message "The Original and Composite volumes must differ."
		return
	}
	if {$v == $Editor(idWorking) && $v != "NEW"} {
		tk_messageBox -message "The Working and Composite volumes must differ."
		return
	}
	set Editor(idComposite) $v
	
	# Change button text
	if {$v == "NEW"} {
		$Editor(mbComposite) config -text $v
	} else {
		$Editor(mbComposite) config -text [Volume($v,node) GetName]
	}

	# Refresh the effect, if it's an interactive one
	EditorUpdateEffect
}

#-------------------------------------------------------------------------------
# .PROC EditorUpdateEffect
# .END
#-------------------------------------------------------------------------------
proc EditorUpdateEffect {} {
	global Editor Ed

	# Call the Enter procedure of the active effect
	set e $Editor(activeID)
	if {[info exists Ed($e,procEnter)] == 1} {
		$Ed($e,procEnter)
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorGetOriginalID
# .END
#-------------------------------------------------------------------------------
proc EditorGetOriginalID {} {
	global Editor
	
	return $Editor(idOriginal)
}

#-------------------------------------------------------------------------------
# .PROC EditorGetWorkingID
#
# Returns the working volume's ID.
# If there is no working volume (Editor(idWorking)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc EditorGetWorkingID {} {
	global Editor Dag Volume Lut
		
	if {$Editor(idWorking) != "NEW"} {
		return $Editor(idWorking)
	}

	# Create a Working volume
	# -----------------------------------------------------
	set v [expr [lindex [lsort -integer -decreasing $Volume(idList)] 0] + 1]
	lappend Volume(idList) $v
	set Editor(idWorking) $v

	puts "NEW WORKING=$v"

	# MrmlVolumeNode
	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID          $v
	Volume($v,node) SetDescription "Working Volume=$v"
	Volume($v,node) SetName        "Working"
	Volume($v,node) SetLUTName     $Lut(idLabel)
	Volume($v,node) InterpolateOff

	# MrmlVolume
	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode           Volume($v,node)
	Volume($v,vol) SetHistogramWidth     $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight    $Volume(histHeight)
	Volume($v,vol) UseLabelIndirectLUTOn
	Volume($v,vol) SetStartMethod     MainStartProgress
	Volume($v,vol) SetProgressMethod "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod       MainEndProgress

	MainUpdateMRML

	return $v
}

#-------------------------------------------------------------------------------
# .PROC EditorGetCompositeID
#
# Returns the working volume's ID.
# If there is no working volume (Editor(idComposite)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc EditorGetCompositeID {} {
	global Editor Dag Volume Lut
		
	if {$Editor(idComposite) != "NEW"} {
		return $Editor(idComposite)
	}

	# Create a Composite volume
	# -----------------------------------------------------
	set v [expr [lindex [lsort -integer -decreasing $Volume(idList)] 0] + 1]
	lappend Volume(idList) $v
	set Editor(idComposite) $v

	puts "NEW COMPOSITE=$v"

	# MrmlVolumeNode
	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID          $v
	Volume($v,node) SetDescription "Composite Volume=$v"
	Volume($v,node) SetName        "Composite"
	Volume($v,node) SetLUTName     $Lut(idLabel)
	Volume($v,node) InterpolateOff

	# MrmlVolume
	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode           Volume($v,node)
	Volume($v,vol) SetHistogramWidth     $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight    $Volume(histHeight)
	Volume($v,vol) UseLabelIndirectLUTOn
	Volume($v,vol) SetStartMethod     MainStartProgress
	Volume($v,vol) SetProgressMethod "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod       MainEndProgress

	MainUpdateMRML

	return $v
}

	
#-------------------------------------------------------------------------------
# .PROC EditorResetDisplay
# .END
#-------------------------------------------------------------------------------
proc EditorResetDisplay {} {
	global Slice Editor

	# Set slice orientations (unless already done)
	set s0 [Slicer GetOrientString 0]
	set s1 [Slicer GetOrientString 1]
	set s2 [Slicer GetOrientString 2]
	if {$s0 != "AxiSlice" || $s1 != "SagSlice" || $s2 != "CorSlice"} {
		MainSlicesSetOrientAll Slices
	}

	# Set slice volumes
	set o [EditorGetOriginalID]
	set w [EditorGetWorkingID]
	set ok 1
	foreach s $Slice(idList) {
		set b [[[Slicer GetBackVolume  $s] GetMrmlNode] GetID]
		set f [[[Slicer GetForeVolume  $s] GetMrmlNode] GetID]
		set l [[[Slicer GetLabelVolume $s] GetMrmlNode] GetID]
		if {$b != $o} {set ok 0}
		if {$f != $w} {set ok 0}
		if {$l != $w} {set ok 0}
	}
	if {$ok == 0} {
		MainSlicesSetVolumeAll Back  $o
		MainSlicesSetVolumeAll Fore  $w
		MainSlicesSetVolumeAll Label $w
	}

	# Do these things only once
	if {$Editor(firstReset) == 0} {
		set Editor(firstReset) 1

		# Slice opacity
		MainSlicesSetOpacityAll 0.3

		# Cursor
		MainAnnoSetHashesVisibility slices 0

		# Show all slices in 3D
		MainSlicesSetVisibilityAll 1
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorSetEffect
# .END
#-------------------------------------------------------------------------------
proc EditorSetEffect {e} {
	global Editor Gui Ed Volume

	# If "menu" then use currently selected menu item
	if {$e == "Menu"} {
		set name [$Editor(rMore) cget -text]
		foreach ee $Ed(idList) {
			if {$Ed($ee,name) == $name} {
				set e $ee
			}
		}
	}

	# Remember prev
	set prevID $Editor(activeID)

	# Set new
	set Editor(activeID) $e
	set Editor(btn) $e

	# Toggle more radio button
	if {$Ed($e,more) == 1} {
		set Editor(moreBtn) 1
		$Editor(rMore) config -text $Ed($e,name)	
	} else {
		set Editor(moreBtn) 0
	}

	# Reset Display
	if {$e != "EdNone"} {
		EditorResetDisplay
	}

	# Describe effect atop the "Details" frame
	$Editor(lEffectName) config -text $Ed($e,name)
	$Editor(lEffectDesc) config -text $Ed($e,desc)

	# Jump there
	if {$e != "EdNone"} {
		Tab Editor row1 Details
	}
	
	# Execute Exit procedure (if one exists for the prevID module)
	# and the Enter procedure of the new module.
	# But don't do this if there's no change.
	#
	if {$e != $prevID} {
		if {[info exists Ed($prevID,procExit)] == 1} {
			$Ed($prevID,procExit)
		}
	}

	# Show "Details" frame
	raise $Ed($e,frame)

	if {$e != $prevID} {
		if {[info exists Ed($e,procEnter)] == 1} {
			$Ed($e,procEnter)
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorGetInputID
#
# Returns the original volume's ID if input="Original", else the working volume
# .END
#-------------------------------------------------------------------------------
proc EditorGetInputID {input} {
	global Editor
		
	if {$input == "Original"} {
		set v [EditorGetOriginalID]
	} else {
		set v [EditorGetWorkingID]
	}
	return $v
}

#-------------------------------------------------------------------------------
# .PROC EditorActivateUndo
#
# Sets Editor(undoActive) to active.
# Disable/Enables Editor(bUndo).
# .END
#-------------------------------------------------------------------------------
proc EditorActivateUndo {active} {
	global Editor

	set Editor(undoActive) $active
	if {$Editor(undoActive) == 0} {
		$Editor(bUndo) config -state disabled
	} else {
		$Editor(bUndo) config -state normal
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorUpdateAfterUndo
# .END
#-------------------------------------------------------------------------------
proc EditorUpdateAfterUndo {} {
	global Ed Editor Volume Slice
		
	set w [EditorGetWorkingID]
	set e $Editor(activeID)

	# Get output from editor
	Volume($w,vol) SetImageData [Ed(editor) GetOutput]
	EditorActivateUndo [Ed(editor) GetUndoable]

	# Restore MrmlNode
	 Volume($w,node) Copy Editor(undoNode)

	# Update pipeline 
	Volume($w,vol) Update
	MainSlicesSetVolumeAll Fore $w
	Slicer ReformatModified
	Slicer Update

	# Update GUI
	if {$w == $Volume(activeID)} {
		# Refresh Volumes GUI with active volume's parameters
		MainVolumesSetActive
	}
	# Update the effect panel GUI by re-running it's Enter procedure
	if {[info exists Ed($e,procEnter)] == 1} {
		$Ed($e,procEnter)
	}

	# The Update() function reset the offsets to be in the middle of
	# the volume, so I need to set them to what's on the GUI:
	foreach s $Slice(idList) {
		Slicer SetOffset $s $Slice($s,offset)
	}
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EditorUndo
#
# Undo the last effect
# Disable the Undo button
# .END
#-------------------------------------------------------------------------------
proc EditorUndo {} {
	global Volume Ed
	
	# Undo the working volume
	Ed(editor) Undo
	EditorUpdateAfterUndo
}


################################################################################
#                         DETAILS 
################################################################################

# These are helper functions for the Effects files to call.
# No procedures in this section are called by anything other than Effects files.

#-------------------------------------------------------------------------------
# .PROC EdBuildScopeGUI
#
# not = [Single | Multi | 3D]
# .END
#-------------------------------------------------------------------------------
proc EdBuildScopeGUI {f var {not ""}} {
	global Gui

	switch $not {
		Single {
			set modes "Multi 3D"
			set names "{Multi Slice} {3D}"
		}
		Multi {
			set modes "Single 3D"
			set names "{1 Slice} {3D}"
		}
		3D {
			set modes "Single Multi"
			set names "{1 Slice} {Multi Slice}"
		}
		default {
			set modes "Single Multi 3D"
			set names "{1 Slice} {Multi Slice} {3D}"
		}
	}
	set c {label $f.l -text "Scope:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		set c {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildInputGUI
# .END
#-------------------------------------------------------------------------------
proc EdBuildInputGUI {f var {options ""}} {
	global Gui

	set c {label $f.l -text "Input Volume:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach input "Original Working" {
		set c {radiobutton $f.f.r$input \
			-text "$input" -variable $var -value $input -width 8 \
			-indicatoron 0 $options $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$input -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildInteractGUI
# .END
#-------------------------------------------------------------------------------
proc EdBuildInteractGUI {f var {options ""}} {
	global Gui

	set modes "Active Slices All"
	set names "{1 Slice} {3 Slices} {3D}"

	set c {label $f.l -text "Interact:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		set c {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0 $options $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildRenderGUI
# .END
#-------------------------------------------------------------------------------
proc EdBuildRenderGUI {f var {options ""}} {
	global Gui

	set modes "Active Slices All"
	set names "{1 Slice} {3 Slices} {3D}"

	set c {label $f.l -text "Render:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		set c {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0 $options $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdSetupBeforeApplyEffect
# .END
#-------------------------------------------------------------------------------
proc EdSetupBeforeApplyEffect {scope v} {
	global Volume Ed

	set o [EditorGetOriginalID]
	set w [EditorGetWorkingID]

	# Set the editor's input & output
	Ed(editor) SetInput [Volume($o,vol) GetOutput]
	if {$v == $w} {
		Ed(editor) SetOutput [Volume($w,vol) GetOutput]
		Ed(editor) UseInputOff
	} else {
		Ed(editor) UseInputOn
	}

	Ed(editor) SetDimensionTo$scope

	# Set the slice orientation and number
	if {$scope != "3D"} {
		set s      [Slicer GetActiveSlice]
		set orient [Slicer GetOrientString $s]
		set slice  [Slicer GetOffset $s]
		
		if {[lsearch "AxiSlice CorSlice SagSlice" $orient] == -1} {
			tk_messageBox -icon warning -title $Gui(title) -message \
				"The orientation of the active slice\n\
				must be one of: AxiSlice, CorSlice, SagSlice"
			return
		}
		switch $orient {
			"AxiSlice" {
				set order IS
			}
			"SagSlice" {
				set order LR
			}
			"CorSlice" {
				set order PA
			}
		}
		Ed(editor) SetOutputSliceOrder $order
		Ed(editor) SetInputSliceOrder [Volume($v,node) GetScanOrder]
		Ed(editor) SetSlice $slice
	}
}

#-------------------------------------------------------------------------------
# .PROC EdUpdateAfterApplyEffect
# .END
#-------------------------------------------------------------------------------
proc EdUpdateAfterApplyEffect {v {render All}} {
	global Ed Volume Lut Editor Slice
		
	set o [EditorGetOriginalID]
	set w [EditorGetWorkingID]

	# Get output from editor
	Volume($w,vol) SetImageData [Ed(editor) GetOutput]
	EditorActivateUndo [Ed(editor) GetUndoable]

	# Keep a copy for undo
#	Editor(undoNode) Copy Volume($w,node)

	# Copy MrmlNode from v to w 
#	if {$v != $w} {
#		# w copies v 
#		Volume($w,node) Copy Volume($v,node)
#	}
	Volume($w,node) Copy Volume($o,node)
	Volume($w,node) InterpolateOff
	Volume($w,node) LabelMapOn
	Volume($w,node) SetLUTName $Lut(idLabel)
	Editor(undoNode) Copy Volume($w,node)

	# Update pipeline 
	Volume($w,vol) Update
	MainSlicesSetVolumeAll Fore $w
	Slicer ReformatModified
	Slicer Update

	# Update GUI
	if {$w == $Volume(activeID)} {
		# Refresh Volumes GUI with active volume's parameters
		MainVolumesSetActive
	}
	# The BuildUpper() function reset the offsets to be in the middle of
	# the volume, so I need to set them to what's on the GUI:
	foreach s $Slice(idList) {
		Slicer SetOffset $s $Slice($s,offset)
	}

	# Render
	Render$render

	$Editor(lRunTime)   config -text \
		"[format "%.2f" [Ed(editor) GetRunTime]] sec,"
	$Editor(lTotalTime) config -text \
		"[format "%.2f" [Ed(editor) GetTotalTime]] sec"
}


################################################################################
#                           OUTPUT
################################################################################


#-------------------------------------------------------------------------------
# .PROC EditorClearOutput
#
# Clear either the Working or Composite data to all zeros.
# .END
#-------------------------------------------------------------------------------
proc EditorClearOutput {data} {
	global Volume Editor Slice

	switch $data {
		Composite {set v $Editor(idComposite)}
		Working   {set v $Editor(idWorking)}
	}

	vtkImageCopy copy
	copy ClearOn
	copy SetInput [Volume($v,vol) GetOutput]
	copy Update

	copy SetInput ""
	Volume($v,vol) SetImageData [copy GetOutput]
	copy SetOutput ""
	copy Delete
	# This next line deletes the vol's original ImageData
	Volume($v,vol) Update

	# Refresh Volumes GUI with active volume's parameters
	if {$v == $Volume(activeID)} {
		MainVolumesSetActive
	}

	if {$v == [EditorGetWorkingID]} {
		MainSlicesSetVolumeAll Fore $v
	}
	Slicer ReformatModified
	Slicer Update
	# The Update() function reset the offsets to be in the middle of
	# the volume, so I need to set them to what's on the GUI:
	foreach s $Slice(idList) {
		Slicer SetOffset $s $Slice($s,offset)
	}
	RenderAll
}
	
#-------------------------------------------------------------------------------
# .PROC EditorMerge
# .END
#-------------------------------------------------------------------------------
proc EditorMerge {data overwriteComposite} {
	global Ed Volume Gui Lut Slice

	switch $data {
		Original {set v [EditorGetOriginalID]}
		Working  {set v [EditorGetWorkingID]}
	}
	set c [EditorGetCompositeID]

	# bg = back (overwritten), fg = foreground (merged in)

	if {$overwriteComposite == 1} {
		set bg $c
		set fg $v
	} else {
		set bg $v
		set fg $c
	}

	# Disable Undo if we're overwriting working
	if {$bg == [EditorGetWorkingID]} {
		Ed(editor) SetUndoable 0
		EditorActivateUndo 0
	}

	# If extents are equal, then overlay, else copy.
	# If we copy the data, then we also have to copy the nodes.

	set bgExt [[Volume($bg,vol) GetOutput] GetExtent]
	set fgExt [[Volume($fg,vol) GetOutput] GetExtent]

	if {$bgExt != $fgExt} {

		# copy node from fg to bg
		Volume($bg,vol) CopyNode Volume($fg,vol)
		Volume($bg,node) InterpolateOff
		Volume($bg,node) LabelMapOn
		Volume($bg,node) SetLUTName $Lut(idLabel)

		# copy data
		vtkImageCopy copy
		copy SetInput [Volume($fg,vol) GetOutput]
		copy Update
		copy SetInput ""
		Volume($bg,vol) SetImageData [copy GetOutput]
		copy SetOutput ""
		copy Delete
		Volume($bg,vol) Update
	} else {
		
		vtkImageOverlay over
		over SetInput 0 [Volume($bg,vol) GetOutput]
		over SetInput 1 [Volume($fg,vol) GetOutput]
		over SetOpacity 1 1.0
		over Update
		over SetInput 0 ""
		over SetInput 1 ""
		Volume($bg,vol) SetImageData [over GetOutput]
		over SetOutput ""
		over Delete
		Volume($bg,vol) Update
	}
		
	# Update pipeline 
	Volume($bg,vol) Update

	if {$bg == [EditorGetWorkingID]} {
		MainSlicesSetVolumeAll Fore $bg
	}
	Slicer ReformatModified
	Slicer Update

	if {$bg == $Volume(activeID)} {
		MainVolumesSetActive
	}
	# The Update() function reset the offsets to be in the middle of
	# the volume, so I need to set them to what's on the GUI:
	foreach s $Slice(idList) {
		Slicer SetOffset $s $Slice($s,offset)
	}
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC EditorWriteOutput
#
# Write either the Working or Composite output to disk.
# .END
#-------------------------------------------------------------------------------
proc EditorWriteOutput {data} {
	global Volume Gui Path Lut tcl_platform Mrml Editor

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Editor(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to write."
		return
	}

	switch $data {
		Composite {set v [EditorGetCompositeID]}
		Working   {set v [EditorGetWorkingID]}
	}

	# Change prefix and header to differ from the input volume
	#
	set filePrefix $Editor(prefix$data)
	set fileFull [file join $Mrml(dir) $filePrefix]

	# Check that it's not blank
	if {[file isdirectory $fileFull] == 1} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Please enter a file prefix for the $data volume."
		return 0
	}
	
	# Check that it's a prefix, not a directory
	if {[file isdirectory $fileFull] == 1} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Please enter a file prefix, not a directory,\n\
			for the $data volume."
		return 0
	}

	# Check that the directory exists
	set dir [file dirname $fileFull]
	if {[file isdirectory $dir] == 0} {
		if {$dir != ""} {
			file mkdir $dir
		}
		if {[file isdirectory $dir] == 0} {
			tk_messageBox -icon info -type ok -title $Gui(title) \
			-message "Failed to make '$dir', so using current directory."
			set dir ""
		}
	}
	Volume($v,node) SetFilePrefix $filePrefix
	Volume($v,node) SetFullPrefix $fileFull

	# interpolate off
	Volume($v,node) InterpolateOff

	# Determine if littleEndian
	if {$tcl_platform(machine) == "intel" || $tcl_platform(machine) == "mips"} {
		Volume($v,node) SetLittleEndian 1
	} else {
		Volume($v,node) SetLittleEndian 1
	}

	# Write volume data
	set Gui(progressText) "Writing [Volume($v,node) GetName]"
	puts "Writing '$fileFull' ..."
	Volume($v,vol) Write
	puts " ...done."

	# Write MRML file
	set filename $fileFull.xml
	vtkMrmlTree tree
	tree AddItem Volume($v,node)
	tree Write $filename
	tree RemoveAllItems
	tree Delete
	puts "Saved MRML file: $filename"
}

