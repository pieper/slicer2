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
# FILE:        Editor.tcl
# PROCEDURES:  
#   EditorInit
#   EditorBuildVTK
#   EditorUpdateMRML
#   EditorBuildGUI
#   EditorEnter
#   EditorMakeModel
#   EditorB1
#   EditorB1Motion
#   EditorB1Release
#   EditorChangeInputLabel
#   EditorSetOriginal
#   EditorSetWorking
#   EditorSetComposite
#   EditorUpdateEffect
#   EditorSameExtents
#   EditorCopyNode
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
#   EdBuildMultiGUI
#   EdBuildInputGUI
#   EdBuildInteractGUI
#   EdBuildRenderGUI
#   EdIsNativeSlice
#   EdSetupBeforeApplyEffect
#   EdUpdateAfterApplyEffect
#   EditorWrite
#   EditorRead
#   EditorClear
#   EditorMerge
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EditorInit
# Sets up tabs and global variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorInit {} {
    global Editor Ed Gui Volume Module env Path
    
    # Define Tabs
    set m Editor
    set Module($m,row1List) "Help Volumes Effects Details Merge"
    set Module($m,row1Name) "{Help} {Volumes} {Effects} {Details} {Merge}"
    set Module($m,row1,tab) Volumes
    
    # Define Procedures
    set Module($m,procGUI)   EditorBuildGUI
    set Module($m,procMRML)  EditorUpdateMRML
    set Module($m,procVTK)   EditorBuildVTK
    set Module($m,procEnter) EditorEnter
    
    # Define Dependencies
    set Module($m,depend) "Labels"
    
    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.30 $} {$Date: 2000/07/28 17:50:31 $}]
    
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
    set Editor(fgName) Working
    set Editor(bgName) Composite
    set Editor(nameWorking) Working
    
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
# Calls BuildVTK procs for files in Editor subdirectory. <br>
# Makes VTK objects vtkImageEditorEffects Ed(editor) and 
# vtkMrmlVolumeNode Editor(undoNode).
# .ARGS
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
# Redoes the menus for picking volumes to edit (since these may have changed with
# a change in MRML).
# .ARGS
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
	foreach v $Volume(idList) {
		$m add command -label [Volume($v,node) GetName] -command \
			"EditorSetOriginal $v; RenderAll"
	}

	# Working Volume menu
	#---------------------------------------------------------------------------
	set m $Editor(mWorking)
	$m delete 0 end
	set idWorking ""
	foreach v $Volume(idList) {
		if {$v != $Volume(idNone) && $v != $Editor(idComposite)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"EditorSetWorking $v; RenderAll"
		}
	}
	
	# Always add a NEW option
	$m add command -label NEW -command "EditorSetWorking NEW; RenderAll"

	# Set the working volume
	EditorSetWorking $Editor(idWorking)

	# Working Volume name field  (name for the NEW volume to be created)
	#---------------------------------------------------------------------------
	set v $Editor(idWorking)
	if {$v != "NEW"} {
	    set Editor(nameWorking) [Volume($v,node) GetName]
	    puts $Editor(nameWorking)
	} else {
	    set Editor(nameWorking) Working
	}

	# Composite Volume menu
	#---------------------------------------------------------------------------
	set m $Editor(mComposite)
	$m delete 0 end
	set idComposite ""
	foreach v $Volume(idList) {
		if {$v != $Volume(idNone) && $v != $Editor(idWorking)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"EditorSetComposite $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Composite"} {
			set idComposite $v
		}
	}
	# If there is composite, then select it, else add a NEW option
	if {$idComposite != ""} {
		EditorSetComposite $idComposite
	} else {
		$m add command -label NEW -command "EditorSetComposite NEW; RenderAll"
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorBuildGUI
# Builds the GUI for the Editor tab.  
# Calls the BuildGUI proc for each file in the Editor subdirectory, and gives each
# one a frame inside the Details->Effect frame.
# .ARGS
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
The Editor module allows editing volumes to create labelmaps, where a unique
number, or label, is assigned to each tissue type.
<P>
Description by Tab:
<UL>
<LI><B>Volumes:</B> Set the <B>Original Volume</B> to the volume you wish
to construct the labelmap from, and then click the <B>Effects</B> tab to 
begin editing.
<BR><B>TIP:</B> If you started editing earlier, and now wish to continue where
you left off, then select the <B>Working Volume</B> from before.
<LI><B>Effects:</B> Editing is performed by applying a series of effects
to the data.  Effects can be applied to the entire volume, each slice one at
a time, or to just one slice. 
<BR><LI><B>Details:</B> This tab contains the detailed parameters for each
effect. 
<BR><B>TIP:</B> When changing from one effect to another, you can avoid the
extra work of clicking the <B>Effects</B> tab by clicking on the 2-letter
abreviation for the effect at the top of the <B>Details</B> tab.
</UL>
<P>
"
	regsub -all "\n" $help { } help
	MainHelpApplyTags Editor $help
	MainHelpBuildGUI Editor


	############################################################################
	#                                 Volumes
	############################################################################

	#-------------------------------------------
	# Volumes frame
	#-------------------------------------------
	set fVolumes $Module(Editor,fVolumes)
	set f $fVolumes

	frame $f.fHelp      -bg $Gui(activeWorkspace)
	frame $f.fOriginal  -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fWorking   -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fHelp $f.fOriginal  $f.fWorking \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Volumes->Help
	#-------------------------------------------
	set f $fVolumes.fHelp

	eval {label $f.l -text "Click the 'Effects' tab to begin editing."} $Gui(WLA)
	pack $f.l

	#-------------------------------------------
	# Volumes->Original
	#-------------------------------------------
	set f $fVolumes.fOriginal

	frame $f.fMenu -bg $Gui(activeWorkspace)

	pack $f.fMenu -side top -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Volumes->Original->Menu
	#-------------------------------------------
	set f $fVolumes.fOriginal.fMenu

	# Volume menu
	eval {label $f.lOriginal -text "Original Volume:"} $Gui(WTA)
	TooltipAdd $f.lOriginal "Choose the input grayscale volume for editing."

	eval {menubutton $f.mbOriginal -text "None" -relief raised -bd 2 -width 18 \
		-menu $f.mbOriginal.m} $Gui(WMBA)
	eval {menu $f.mbOriginal.m} $Gui(WMA)
	pack $f.lOriginal -padx $Gui(pad) -side left -anchor e
	pack $f.mbOriginal -padx $Gui(pad) -side left -anchor w

	# Save widgets for changing
	set Editor(mbOriginal) $f.mbOriginal
	set Editor(mOriginal)  $f.mbOriginal.m

	#-------------------------------------------
	# Volumes->Working
	#-------------------------------------------
	set f $fVolumes.fWorking

	frame $f.fMenu -bg $Gui(activeWorkspace)
	frame $f.fName -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)

	pack $f.fMenu -side top -pady $Gui(pad)
	pack $f.fName -side top -pady $Gui(pad) -fill x
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)


	#-------------------------------------------
	# Volumes->Working->Menu
	#-------------------------------------------
	set f $fVolumes.fWorking.fMenu

	# Volume menu
	eval {label $f.lWorking -text "Working Volume:"} $Gui(WTA)
	TooltipAdd $f.lWorking "Choose a labelmap to edit, or NEW for a new one."

	eval {menubutton $f.mbWorking -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbWorking.m} $Gui(WMBA)
	eval {menu $f.mbWorking.m} $Gui(WMA)
	pack $f.lWorking $f.mbWorking -padx $Gui(pad) -side left

	# Save widgets for changing
	set Editor(mbWorking) $f.mbWorking
	set Editor(mWorking)  $f.mbWorking.m


	#-------------------------------------------
	# Volumes->Working->Prefix
	#-------------------------------------------
	set f $fVolumes.fWorking.fPrefix

	eval {label $f.l -text "Filename Prefix:"} $Gui(WLA)
	TooltipAdd $f.l "To save the Working Volume, enter the prefix here or just click Save."
	eval {entry $f.e -textvariable Editor(prefixWorking)} $Gui(WEA)
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Volumes->Working->Name
	#-------------------------------------------
	set f $fVolumes.fWorking.fName

	eval {label $f.l -text "Descriptive Name:"} $Gui(WLA)
	TooltipAdd $f.l "You may name your NEW volume."
	eval {entry $f.e -textvariable Editor(nameWorking)} $Gui(WEA)
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	# Save widget for disabling name field if not NEW volume
	set Editor(eNameWorking) $f.e

	#-------------------------------------------
	# Volumes->Working->Btns
	#-------------------------------------------
	set f $fVolumes.fWorking.fBtns

	eval {button $f.bWrite -text "Save" -width 5 \
		-command "EditorWrite Working; RenderAll"} $Gui(WBA)
	TooltipAdd $f.bWrite "Save the Working Volume."
	eval {button $f.bClear -text "Clear to 0's" -width 12 \
		-command "EditorClear Working; RenderAll"} $Gui(WBA)
	TooltipAdd $f.bClear "Clear the Working Volume."
	eval {button $f.bRead -text "Read" -width 5 \
		-command "EditorRead Working; RenderAll"} $Gui(WBA)
	TooltipAdd $f.bRead "Reread the Working Volume from disk."
	pack $f.bWrite $f.bRead $f.bClear -side left -padx $Gui(pad)


	############################################################################
	#                                 Effects
	############################################################################

	#-------------------------------------------
	# Effects frame
	#-------------------------------------------
	set fEffects $Module(Editor,fEffects)
	set f $fEffects

	frame $f.fEffects   -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fActive    -bg $Gui(activeWorkspace)
	frame $f.fTime      -bg $Gui(activeWorkspace)
	frame $f.fModel    -bg $Gui(activeWorkspace)
	pack $f.fEffects $f.fActive $f.fTime $f.fModel \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Effects->Active frame
	#-------------------------------------------
	set f $fEffects.fActive

	eval {label $f.l -text "Active Slice:"} $Gui(WLA)
	pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach s $Slice(idList) text "Red Yellow Green" width "4 7 6" {
		eval {radiobutton $f.r$s -width $width -indicatoron 0\
			-text "$text" -value "$s" -variable Slice(activeID) \
			-command "MainSlicesSetActive"} $Gui(WCA) {-selectcolor $Gui(slice$s)}
		pack $f.r$s -side left -fill x -anchor e
	}

	#-------------------------------------------
	# Effects->Time frame
	#-------------------------------------------
	set f $fEffects.fTime

	eval {label $f.lRun -text "Run time:"} $Gui(WLA)
	eval {label $f.lRunTime -text "0 sec,"} $Gui(WLA)
	eval {label $f.lTotal -text "Total:"} $Gui(WLA)
	eval {label $f.lTotalTime -text "0 sec"} $Gui(WLA)
	pack $f.lRun $f.lRunTime $f.lTotal $f.lTotalTime \
		-side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	set Editor(lRunTime) $f.lRunTime
	set Editor(lTotalTime) $f.lTotalTime

	#-------------------------------------------
	# Effects->Model frame
	#-------------------------------------------
	set f $fEffects.fModel

	if {[IsModule ModelMaker] == 1} {
		eval {button $f.b -text "Make Model" -command "EditorMakeModel"} $Gui(WBA)
		pack $f.b
	}

	#-------------------------------------------
	# Effects->Effects
	#-------------------------------------------
	set f $fEffects.fEffects

	frame $f.fBtns -bg $Gui(backdrop)
	frame $f.fMore -bg $Gui(backdrop)
	frame $f.fUndo -bg $Gui(backdrop)
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
		eval {menubutton $f.mbMore -text "More:" -relief raised -bd 2 \
			-width 6 -menu $f.mbMore.m} $Gui(WMBA)
			eval {menu $f.mbMore.m} $Gui(WMA)
		eval {radiobutton $f.rMore -width 13 \
			-text "None" -variable Editor(moreBtn) -value 1 \
			-command "EditorSetEffect Menu" -indicatoron 0} $Gui(WCA)
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
				eval {radiobutton $f.$row.r$e -width 13 \
					-text "$Ed($e,name)" -variable Editor(btn) -value $e \
					-command "EditorSetEffect $e" -indicatoron 0} $Gui(WCA)
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

	eval {button $f.bUndo -text "Undo last effect" -width 17 \
		-command "EditorUndo; RenderAll"} $Gui(WBA) {-state disabled}
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

	frame $f.fTitle  -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fEffect -bg $Gui(activeWorkspace)
	pack $f.fTitle -side top -pady 5 -padx 2 -fill x
	pack $f.fEffect -side top -pady 0 -padx 2 -fill both -expand 1

	#-------------------------------------------
	# Details->Title frame
	#-------------------------------------------
	set f $fDetails.fTitle

	frame $f.fBar -bg $Gui(activeWorkspace)
	frame $f.fHelp -bg $Gui(activeWorkspace)
	pack $f.fBar $f.fHelp -side top -pady 2

	# List top 8 effects on a button bar across the top
	set f $fDetails.fTitle.fBar
	foreach e [lrange $Ed(idList) 0 7] {
		eval {radiobutton $f.r$e -width 2 -indicatoron 0\
			-text $Ed($e,initials) -value $e -variable Editor(btn) \
			-command "EditorSetEffect $e"} $Gui(WCA)
		TooltipAdd $f.r$e $Ed($e,name)
		pack $f.r$e -side left -fill x -anchor e
	}
	# Add an Undo button
	eval {button $f.bUndo -width 2 -text Un -command "EditorUndo; RenderAll"} \
		$Gui(WBA) {-state disabled}
	TooltipAdd $f.bUndo "Undo last effect applied"
	pack $f.bUndo -side left -fill x -anchor e
	set Editor(bUndo2) $f.bUndo

	set f $fDetails.fTitle.fHelp
	eval {label $f.lName -text "None"} $Gui(BLA)
	eval {label $f.lDesc -text "Does nothing."} $Gui(BLA)
	pack $f.lDesc

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
	#                                 Merge
	############################################################################

	#-------------------------------------------
	# Merge frame
	#-------------------------------------------
	set fMerge $Module(Editor,fMerge)
	set f $fMerge

	frame $f.fHelp      -bg $Gui(activeWorkspace)
	frame $f.fComposite -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fMerge     -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fHelp $f.fComposite $f.fMerge \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Merge->Help
	#-------------------------------------------
	set f $fMerge.fHelp

	eval {label $f.l -text "Merge two label maps:\nthe first will be copied onto the second."} $Gui(WLA)
	pack $f.l

	#-------------------------------------------
	# Merge->Composite
	#-------------------------------------------
	set f $fMerge.fComposite

	frame $f.fMenu   -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.fMenu -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	#-------------------------------------------
	# Merge->Composite->Menu
	#-------------------------------------------
	set f $fMerge.fComposite.fMenu

	# Volume menu
	eval {label $f.lComposite -text "Composite Volume:"} $Gui(WTA)

	eval {menubutton $f.mbComposite -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbComposite.m} $Gui(WMBA)
	eval {menu $f.mbComposite.m} $Gui(WMA)
	pack $f.lComposite $f.mbComposite -padx $Gui(pad) -side left

	# Save widgets for changing
	set Editor(mbComposite) $f.mbComposite
	set Editor(mComposite)  $f.mbComposite.m

	#-------------------------------------------
	# Merge->Composite->Prefix
	#-------------------------------------------
	set f $fMerge.fComposite.fPrefix

	eval {label $f.l -text "File Prefix:"} $Gui(WLA)
	eval {entry $f.e \
		-textvariable Editor(prefixComposite)} $Gui(WEA)
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Merge->Composite->Btns
	#-------------------------------------------
	set f $fMerge.fComposite.fBtns

	eval {button $f.bWrite -text "Save" -width 5 \
		-command "EditorWrite Composite; RenderAll"} $Gui(WBA)
	eval {button $f.bClear -text "Clear to 0's" -width 12 \
		-command "EditorClear Composite; RenderAll"} $Gui(WBA)
	eval {button $f.bRead -text "Read" -width 5 \
		-command "EditorRead Composite; RenderAll"} $Gui(WBA)
	pack $f.bWrite $f.bRead $f.bClear -side left -padx $Gui(pad)


	#-------------------------------------------
	# Merge->Merge
	#-------------------------------------------
	set f $fMerge.fMerge

	eval {label $f.lTitle -text "Combine 2 Label Maps"} $Gui(WTA)
	frame $f.f  -bg $Gui(activeWorkspace)
	eval {button $f.b -text "Merge" -width 6 \
		-command "EditorMerge merge 0; RenderAll"} $Gui(WBA)
	pack $f.lTitle $f.f $f.b -pady $Gui(pad) -side top

	set f $fMerge.fMerge.f

	eval {label $f.l1 -text "Write"} $Gui(WLA)

	eval {menubutton $f.mbFore -text "$Editor(fgName)" -relief raised -bd 2 -width 9 \
		-menu $f.mbFore.m} $Gui(WMBA)
	eval {menu $f.mbFore.m} $Gui(WMA)
	set Editor(mbFore) $f.mbFore
	set m $Editor(mbFore).m
	foreach v "Working Composite Original" {
		$m add command -label $v -command "EditorMerge Fore $v"
	}

	eval {label $f.l2 -text "over"} $Gui(WLA)

	eval {menubutton $f.mbBack -text "$Editor(bgName)" -relief raised -bd 2 -width 9 \
		-menu $f.mbBack.m} $Gui(WMBA)
	eval {menu $f.mbBack.m} $Gui(WMA)
	set Editor(mbBack) $f.mbBack
	set m $Editor(mbBack).m
	foreach v "Working Composite" {
		$m add command -label $v -command "EditorMerge Back $v"
	}

	pack $f.mbFore $f.l2 $f.mbBack -padx $Gui(pad) -side left -anchor w


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
	
	# Initialize to the first effect
	EditorSetEffect EdNone
}

#-------------------------------------------------------------------------------
# .PROC EditorEnter
# Called when the Editor panel is entered by the user. 
# If no "Original" volume has been selected, tries to set it to the one being 
# displayed as background.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorEnter {} {
	global Editor Volume Slice

	# If the Original is None, then select what's being displayed,
	# otherwise the first volume in the mrml tree.
	
	if {[EditorGetOriginalID] == $Volume(idNone)} {
		set v [[[Slicer GetBackVolume $Slice(activeID)] GetMrmlNode] GetID]
		if {$v == $Volume(idNone)} {
			set v [lindex $Volume(idList) 0]
		}
		if {$v != $Volume(idNone)} {
			EditorSetOriginal $v
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorMakeModel
# Sets the active volume (to the first one of Composite, Working, or Original 
# that has been defined by the user).  Then tabs to ModelMaker panel. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorMakeModel {} {
	global Editor

	if {$Editor(idComposite) == "NEW"} {
		if {$Editor(idWorking) == "NEW"} {
			MainVolumesSetActive [EditorGetOriginalID]
		} else {
			MainVolumesSetActive [EditorGetWorkingID]
		}
	} else {
		MainVolumesSetActive [EditorGetCompositeID]
	}
	Tab ModelMaker row1 Create
}

################################################################################
#                             Event Bindings
################################################################################


#-------------------------------------------------------------------------------
# .PROC EditorB1
# Effect-specific response to B1 mouse click.
# .ARGS
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
	"EdIdentifyIslands" {
		EditorChangeInputLabel $x $y
	}
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorB1Motion
# Effect-specific response to B1 mouse motion. 
# Currently only used for Draw.
# .ARGS
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

# DAVE: allow drawing on non-native slices someday
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
# Effect-specific response to B1 mousebutton release.
# Currently only used for Draw.
# .ARGS
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
# 
# .ARGS
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
# 
# .ARGS
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

	# Update the display and the effect.
	if {$Editor(activeID) != "EdNone"} {
		# Display the original in the background layer of the slices
		EditorResetDisplay

		# Refresh the effect, if it's an interactive one
		EditorUpdateEffect
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorSetWorking
# 
# .ARGS
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
	
	# Change button text, show name and file prefix
	# Disable name field if not NEW volume
	if {$v == "NEW"} {
		$Editor(mbWorking) config -text $v
		set Editor(prefixWorking) ""
		set Editor(nameWorking) Working
		$Editor(eNameWorking) configure -state normal
	} else {
		$Editor(mbWorking) config -text [Volume($v,node) GetName]
		set Editor(prefixWorking) [MainFileGetRelativePrefix \
			[Volume($v,node) GetFilePrefix]]
		set Editor(nameWorking) [Volume($v,node) GetName]
		$Editor(eNameWorking) configure -state disabled
	}

	# Refresh the effect, if it's an interactive one
	EditorUpdateEffect
}

#-------------------------------------------------------------------------------
# .PROC EditorSetComposite
# 
# .ARGS
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
	
	# Change button text, and show file prefix
	if {$v == "NEW"} {
		$Editor(mbComposite) config -text $v
		set Editor(prefixComposite) ""
	} else {
		$Editor(mbComposite) config -text [Volume($v,node) GetName]
		set Editor(prefixComposite) [MainFileGetRelativePrefix \
			[Volume($v,node) GetFilePrefix]]
	}

	# Refresh the effect, if it's an interactive one
	EditorUpdateEffect
}

#-------------------------------------------------------------------------------
# .PROC EditorUpdateEffect
# 
# .ARGS
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
# .PROC EditorSameExtents
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorSameExtents {dst src} {
	set dstExt [[Volume($dst,vol) GetOutput] GetExtent]
	set srcExt [[Volume($src,vol) GetOutput] GetExtent]
	if {$dstExt == $srcExt} {
		return 1
	}
	return 0
}

#-------------------------------------------------------------------------------
# .PROC EditorCopyNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorCopyNode {dst src} {
	global Volume Lut

	Volume($dst,vol) CopyNode Volume($src,vol)
	Volume($dst,node) InterpolateOff
	Volume($dst,node) LabelMapOn
	Volume($dst,node) SetLUTName $Lut(idLabel)
}

#-------------------------------------------------------------------------------
# .PROC EditorGetOriginalID
# 
# .ARGS
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
	global Editor Volume Lut
		
	# If there is no Working volume, then create one
	if {$Editor(idWorking) != "NEW"} {
		return $Editor(idWorking)
	}

	# Create the node
	set n [MainMrmlAddNode Volume]
	set v [$n GetID]

	$n SetDescription "Working Volume=$v"
	$n SetLUTName     $Lut(idLabel)
	$n InterpolateOff
	$n LabelMapOn

	# Make sure the name entered is okay, else use default
	if {[ValidateName $Editor(nameWorking)] == 0} {
	    puts "The Descriptive Name can consist of letters, digits, dashes, or underscoresonly. Using default name Working"
	    $n SetName Working
	} else {
	    $n SetName $Editor(nameWorking)   
	}
	
	# Create the volume
	MainVolumesCreate $v
	Volume($v,vol) UseLabelIndirectLUTOn

	EditorSetWorking $v

        # This updates all the buttons to say that the
        # Volume List has changed.
	MainUpdateMRML

	return $v
}

#-------------------------------------------------------------------------------
# .PROC EditorGetCompositeID
#
# Returns the composite volume's ID.
# If there is no composite volume (Editor(idComposite)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc EditorGetCompositeID {} {
	global Editor Dag Volume Lut
		
	# If there is no Composite volume, then create one
	if {$Editor(idComposite) != "NEW"} {
		return $Editor(idComposite)
	}
	
	# Create the node
	set n [MainMrmlAddNode Volume]
	set v [$n GetID]
	$n SetDescription "Composite Volume=$v"
	$n SetName        "Composite"
	$n SetLUTName     $Lut(idLabel)
	$n InterpolateOff
	$n LabelMapOn

	# Create the volume
	MainVolumesCreate $v
	Volume($v,vol) UseLabelIndirectLUTOn

	EditorSetComposite $v

	MainUpdateMRML

	return $v
}

	
#-------------------------------------------------------------------------------
# .PROC EditorResetDisplay
# 
# .ARGS
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
		MainSlicesSetFadeAll 0

		# Cursor
		MainAnnoSetHashesVisibility slices 0

		# Show all slices in 3D
		MainSlicesSetVisibilityAll 1
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorSetEffect
# 
# .ARGS
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
		RenderAll
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
		$Editor(bUndo2) config -state disabled
	} else {
		$Editor(bUndo) config -state normal
		$Editor(bUndo2) config -state normal
	}
}

#-------------------------------------------------------------------------------
# .PROC EditorUpdateAfterUndo
# 
# .ARGS
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

	# Update pipeline and GUI
	MainVolumesUpdate $w

	# Update the effect panel GUI by re-running it's Enter procedure
	if {[info exists Ed($e,procEnter)] == 1} {
		$Ed($e,procEnter)
	}

	# Mark the volume as changed
	set Volume($w,dirty) 1

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
	eval {label $f.l -text "Scope:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		eval {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildMultiGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdBuildMultiGUI {f var} {
	global Gui

	eval {label $f.l -text "Multi-Slice Orient:"} $Gui(WLA)
	pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach s "Native Active" text "Native Active" width "7 7" {
		eval {radiobutton $f.r$s -width $width -indicatoron 0\
			-text "$text" -value "$s" -variable $var} $Gui(WCA)
		pack $f.r$s -side left -fill x -anchor e
	}
}

#-------------------------------------------------------------------------------
# .PROC EdBuildInputGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdBuildInputGUI {f var {options ""}} {
	global Gui

	eval {label $f.l -text "Input Volume:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	foreach input "Original Working" {
		eval {radiobutton $f.f.r$input \
			-text "$input" -variable $var -value $input -width 8 \
			-indicatoron 0} $options $Gui(WCA)
		pack $f.f.r$input -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildInteractGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdBuildInteractGUI {f var {options ""}} {
	global Gui

	set modes "Active Slices All"
	set names "{1 Slice} {3 Slices} {3D}"

	eval {label $f.l -text "Interact:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		eval {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0} $options $Gui(WCA)
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdBuildRenderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdBuildRenderGUI {f var {options ""}} {
	global Gui

	set modes "Active Slices All"
	set names "{1 Slice} {3 Slices} {3D}"

	eval {label $f.l -text "Render:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		eval {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable $var -value $mode \
			-indicatoron 0} $options $Gui(WCA)
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w
}

#-------------------------------------------------------------------------------
# .PROC EdIsNativeSlice
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdIsNativeSlice {} {
	global Ed
	
	set outOrder [Ed(editor) GetOutputSliceOrder]
	set inOrder  [Ed(editor) GetInputSliceOrder]

	# Output order is one of IS, LR, PA
	if {$inOrder == "RL"} {set inOrder LR}
	if {$inOrder == "AP"} {set inOrder PA}
	if {$inOrder == "SI"} {set inOrder IS}
	if {$outOrder != $inOrder} {
		if {$inOrder == "LR"} {
			set native SagSlice
		}
		if {$inOrder == "PA"} {
			set native CorSlice
		}
		if {$inOrder == "IS"} {
			set native AxiSlice
		}
		return $native
	}
	return ""
}

#-------------------------------------------------------------------------------
# .PROC EdSetupBeforeApplyEffect
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdSetupBeforeApplyEffect {v scope multi} {
	global Volume Ed Editor Gui

	set o [EditorGetOriginalID]
	set w [EditorGetWorkingID]

	if {[EditorSameExtents $w $o] != 1} {
		EditorCopyNode $w $o
		MainVolumesCopyData $w $o On
	}
	
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
	# (not used for 3D)

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

		# Does the user want the orien of the active slice or native slices?
		if {$scope == "Multi" && $multi == "Native"} {
			set order [Volume($o,node) GetScanOrder]
		}
		switch $order {
			"SI" {
				set order IS
			}
			"RL" {
				set order LR
			}
			"AP" {
				set order PA
			}
		}

		Ed(editor) SetOutputSliceOrder $order
		Ed(editor) SetInputSliceOrder [Volume($v,node) GetScanOrder]
		Ed(editor) SetSlice $slice
}

#-------------------------------------------------------------------------------
# .PROC EdUpdateAfterApplyEffect
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdUpdateAfterApplyEffect {v {render All}} {
	global Ed Volume Lut Editor Slice
		
	set o [EditorGetOriginalID]
	set w [EditorGetWorkingID]

	# Get output from editor
	Volume($w,vol) SetImageData [Ed(editor) GetOutput]
	EditorActivateUndo [Ed(editor) GetUndoable]

	# w copies o's MrmlNode if the Input was the Original
	if {$v == $o} {
		EditorCopyNode $w $o
	}

	# Keep a copy for undo
	Editor(undoNode) Copy Volume($w,node)

	# Update pipeline and GUI
	MainVolumesUpdate $w

	# Render
	Render$render

	# Mark the volume as changed
	set Volume($w,dirty) 1

	$Editor(lRunTime)   config -text \
		"[format "%.2f" [Ed(editor) GetRunTime]] sec,"
	$Editor(lTotalTime) config -text \
		"[format "%.2f" [Ed(editor) GetTotalTime]] sec"
}


################################################################################
#                           OUTPUT
################################################################################

#-------------------------------------------------------------------------------
# .PROC EditorWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorWrite {data} {
	global Volume Editor

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Editor(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to write."
		return
	}

	switch $data {
		Composite {set v [EditorGetCompositeID]}
		Working   {set v [EditorGetWorkingID]}
	}

	# Show user a File dialog box
	set Editor(prefix$data) [MainFileSaveVolume $v $Editor(prefix$data)]
	if {$Editor(prefix$data) == ""} {return}

	# Write
	MainVolumesWrite $v $Editor(prefix$data)

	# Prefix changed, so update the Volumes->Props tab
	MainVolumesSetActive $v
}

#-------------------------------------------------------------------------------
# .PROC EditorRead
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorRead {data} {
	global Volume Editor Mrml

	# If the volume doesn't exist yet, then don't read it, duh!
	if {$Editor(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to read."
		return
	}

	switch $data {
		Composite {set v $Editor(idComposite)}
		Working   {set v $Editor(idWorking)}
	}

	# Show user a File dialog box
	set Editor(prefix$data) [MainFileOpenVolume $v $Editor(prefix$data)]
	if {$Editor(prefix$data) == ""} {return}
		
	# Read
	Volume($v,node) SetFilePrefix $Editor(prefix$data)
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

#-------------------------------------------------------------------------------
# .PROC EditorClear
#
# Clear either the Working or Composite data to all zeros.
# .END
#-------------------------------------------------------------------------------
proc EditorClear {data} {
	global Volume Editor Slice

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Editor(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to clear."
		return
	}

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

	# Mark the volume as changed
	set Volume($v,dirty) 1

	MainVolumesUpdate $v
	RenderAll
}
	
#-------------------------------------------------------------------------------
# .PROC EditorMerge
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EditorMerge {op arg} {
	global Ed Volume Gui Lut Slice Editor

	if {$op == "Fore"} {
		set Editor(fgName) $arg
		$Editor(mbFore) config -text $Editor(fgName)
		return
	} elseif {$op == "Back"} {
		set Editor(bgName) $arg
		$Editor(mbBack) config -text $Editor(bgName)
		return
	}
	
	# bg = back (overwritten), fg = foreground (merged in)

	switch $Editor(fgName) {
		Original  {set fg [EditorGetOriginalID]}
		Working   {set fg [EditorGetWorkingID]}
		Composite {set fg [EditorGetCompositeID]}
		default   {tk_messageBox -message "\
Merge the Original, Working, or Composite, not '$fgName'"; return}
	}
	switch $Editor(bgName) {
		Working   {set bg [EditorGetWorkingID]}
		Composite {set bg [EditorGetCompositeID]}
		default   {tk_messageBox -message "\
Merge with the Working or Composite, not '$bgName'"; return}
	}

	# Do nothing if fg=bg
	if {$fg == $bg} {
		return
	}

	# Disable Undo if we're overwriting working
	if {$bg == [EditorGetWorkingID]} {
		Ed(editor) SetUndoable 0
		EditorActivateUndo 0
	}

	# If extents are equal, then overlay, else copy.
	# If we copy the data, then we also have to copy the nodes.

	if {[EditorSameExtents $bg $fg] != 1} {
		# copy node from fg to bg
		EditorCopyNode $bg $fg

		# copy data
		MainVolumesCopyData $bg $fg Off
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
	}
		
	# Mark the volume as changed
	set Volume($bg,dirty) 1

	# Update pipeline and gui
	MainVolumesUpdate $bg
	RenderAll
}

