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
# FILE:        ModelHierarchy.tcl
# PROCEDURES:  
#   ModelHierarchyInit
#   ModelHierarchyBuildGUI
#   ModelHierarchyEnter
#   ModelHierarchyExit
#   ModelHierarchyUpdateGUI
#   ModelHierarchyDeleteNode
#   ModelHierarchyRedrawFrame
#   ModelHierarchyCreate
#   ModelHierarchyRemoveAsk
#   ModelHierarchyRemove
#   ModelHierarchyCreateGroup
#   ModelHierarchyCreateGroupOk
#   ModelHierarchyMoveModelDialog
#   ModelHierarchyMoveModelDialogOk
#   ModelHierarchyMoveModel
#   ModelHierarchyDrag
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module enables the user to change the hierarchy of models.
#  If no hierarchy exists, a new one can be created.
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyInit {} {
    global ModelHierarchy Module Volume Model
    
    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
    set m ModelHierarchy
    set Module($m,row1List) "Help HDisplay"
    set Module($m,row1Name) "{Help} {Hierarchy}"
    set Module($m,row1,tab) HDisplay

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources all *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) ModelHierarchyBuildVTK
    #
    #   All the options are:
    #
    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #   procRecallPresets  = Called when the user clicks one of the Presets buttons
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) ModelHierarchyBuildGUI
    set Module($m,procEnter) ModelHierarchyEnter
    set Module($m,procExit) ModelHierarchyExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.1 $} {$Date: 2001/11/13 20:44:53 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
}


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
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC ModelHierarchyBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyBuildGUI {} {
    global Gui ModelHierarchy Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(ModelHierarchy,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
	# Frame ModelHierarchy:
    #-------------------------------------------
    # Help
    # HDisplay
    #   cEditor
    #     fModels
    #-------------------------------------------
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The ModelHierarchy module displays the hierarchy of your models.  It also allows you to change the hierarchy, to delete it or to create a new one.<BR>
    To move a model to another model group, simply use drag and drop. You can also drop models on other models. In this case they will be inserted directly before the model on which they were dropped. Doing this, you can reorganize the order of your models in any way you like.<BR>
    You can also move whole model groups, but only to other model groups.<BR>
    If you like to create a new group, click the respective button and enter the desired name.<BR>
    The buttons 'Create' and 'Delete' affect the whole hierarchy.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags ModelHierarchy $help
    MainHelpBuildGUI ModelHierarchy
    
    #-------------------------------------------
    # Hierarchy frame
    #-------------------------------------------
    set fHierarchy $Module(ModelHierarchy,fHDisplay)
    set f $fHierarchy
    pack configure $f -expand true -fill both

    # make canvas inside the Hierarchy frame
    canvas $f.cEditor -bg $Gui(activeWorkspace) -yscrollcommand "$f.sScrollBar set" -bd 0 -relief flat
    frame $f.fButtons -bg $Gui(activeWorkspace)
    eval "scrollbar $f.sScrollBar -orient vertical -command \"$f.cEditor yview\" $Gui(WSBA)"
    pack $f.fButtons -side bottom -fill x
    pack $f.sScrollBar -side right -fill y
    pack $f.cEditor -side top -fill both -expand true -padx 0 -pady $Gui(pad)
    $f.cEditor config -relief flat -bd 0

    #-------------------------------------------
    # Hierarchy->Editor canvas
    #-------------------------------------------
    set ModelHierarchy(ModelCanvas) $fHierarchy.cEditor
    set ModelHierarchy(ModelFrame) $ModelHierarchy(ModelCanvas).fModels
    set f $ModelHierarchy(ModelCanvas)
    
    frame $f.fModels -bg $Gui(activeWorkspace)
    $f create window 0 0 -anchor nw -window $f.fModels
    
    #-------------------------------------------
    # Hierarchy->Buttons frame
    #-------------------------------------------
    set f $fHierarchy.fButtons
    eval {button $f.bCreate -text "Create" -command "ModelHierarchyCreate"} $Gui(WBA)
    eval {button $f.bDelete -text "Delete" -command "ModelHierarchyDeleteAsk $f.bDelete"} $Gui(WBA)
    eval {button $f.bCreateGroup -text "Create group" -command "ModelHierarchyCreateGroup"} $Gui(WBA)
    #eval {label $f.lTrash -text "T"} $Gui(WLA)
    #grid $f.bCreate $f.bDelete $f.bCreateGroup $f.lTrash -padx $Gui(pad)
    grid $f.bCreate $f.bDelete $f.bCreateGroup -padx $Gui(pad)
    TooltipAdd $f.bCreate "Create a new hierarchy"
    TooltipAdd $f.bDelete "Delete the hierarchy"
    TooltipAdd $f.bCreateGroup "Create a new model group"
    #bindtags $f.lTrash [list DragDrop $f.lTrash Label . all]
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyRootEntry
# Creates the entry for the root level of the hierarchy display.
# .ARGS
# f: frame where the entry is created
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyCreateRootEntry {f} {
	global Gui
	
	frame $f.froot -bg $Gui(activeWorkspace)
	pack $f.froot -side top -fill x -expand true
	eval {label $f.lgroot -text "<root>"} $Gui(WLA)
	pack $f.lgroot -side left -in $f.froot
	bindtags $f.lgroot [list DragDrop $f.lroot Label . all]
}

#-------------------------------------------------------------------------------
# .PROC ModelHierarchyEnter
# Called when this module is entered by the user. Creates the hierarchy display
# for the models.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyEnter {} {
    global ModelHierarchy Gui Module
    global Mrml(dataTree)
        
    set HierarchyLevel 0
	set numLines 0

	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	set f $ModelHierarchy(ModelFrame)
	
	set success 0
	
	# Drag and Drop bindings
	ModelHierarchyDrag init 0
	bind DragDrop {}
	bind DragDrop <ButtonPress-1> 	[list ModelHierarchyDrag start %W]
	bind DragDrop <Motion> 		[list ModelHierarchyDrag motion %X %Y]
	bind DragDrop <ButtonRelease-1>	[list ModelHierarchyDrag stop %X %Y]
	bind DragDrop <<DragOver>>	[list ModelHierarchyDrag over %W]
	bind DragDrop <<DragLeave>>	[list ModelHierarchyDrag leave %W]
	bind DragDrop <<DragDrop>>	[list ModelHierarchyDrag drop %W %X %Y]
		
	while {$node != ""} {
		if {[string compare -length 10 $node "ModelGroup"] == 0} {
			if {$success==0} {
				set success 1
				ModelHierarchyCreateRootEntry $f
			}
			incr HierarchyLevel
			eval {label $f.lg[$node GetID] -text "Group: [$node GetName]"} $Gui(WLA)
			bindtags $f.lg[$node GetID] [list DragDrop $f.lg[$node GetID] Label . all]
			
			eval {label $f.l1g_[$node GetID] -text "" -width [expr $HierarchyLevel*2]} $Gui(WLA)
			frame $f.f1g_[$node GetID] -bg $Gui(activeWorkspace)
			set l1_command $f.l1g_[$node GetID]
			
			pack $f.f1g_[$node GetID] -side top -expand true -fill x
			pack $l1_command $f.lg[$node GetID] -in $f.f1g_[$node GetID] -side left
			lower $f.f1g_[$node GetID]
			incr numLines
		}
		if {[string compare -length 13 $node "EndModelGroup"] == 0} {
			incr HierarchyLevel -1
		}
		if {[string compare -length 8 $node "ModelRef"] == 0} {
			if {$success==0} {
				set success 1
				ModelHierarchyCreateRootEntry $f
			}
			set CurrentModelID [SharedModelLookup [$node GetmodelRefID]]
			if {$CurrentModelID != -1} {
					eval {label $f.l$CurrentModelID -text "[Model($CurrentModelID,node) GetName]"} $Gui(WLA)
					bindtags $f.l$CurrentModelID [list DragDrop $f.l$CurrentModelID Label . all]
					
					frame $f.f1_$CurrentModelID -bg $Gui(activeWorkspace)
					eval {label $f.l1_$CurrentModelID -text "" -width [expr ($HierarchyLevel+1)*2]} $Gui(WLA)
					set l1_command $f.l1_$CurrentModelID
					
					pack $f.f1_$CurrentModelID -side top -expand true -fill x
					pack $l1_command $f.l$CurrentModelID -in $f.f1_$CurrentModelID -side left
					lower $f.f1_$CurrentModelID
					incr numLines
			}
		}
		set node [Mrml(dataTree) GetNextItem]
	}
	
	set fb $Module(ModelHierarchy,fHDisplay).fButtons
	
	if {$success > 0} {
		# Find the height of a single label
		set lastLabel $f.l$CurrentModelID
		# Find the height of a line
		set incr [expr {[winfo reqheight $lastLabel]}]
		# Find the total height that should scroll and add some "safety space"
		set height [expr {$numLines * $incr + 10}]
		$ModelHierarchy(ModelCanvas) config -scrollregion "0 0 1 $height"
		$ModelHierarchy(ModelCanvas) config -yscrollincrement $incr -confine true
		$fb.bCreate config -state disabled
		$fb.bDelete config -state normal
		$fb.bCreateGroup config -state normal
	} else {
		# no hierarchy found
		$ModelHierarchy(ModelCanvas) config -scrollregion "0 0 0 0"
		eval {label $f.l -text "No hierarchy found."} $Gui(WLA)
		grid $f.l
		$fb.bCreate config -state normal
		$fb.bDelete config -state disabled
		$fb.bCreateGroup config -state disabled
	}
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyExit
# Called when this module is exited by the user.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyExit {} {
    global ModelHierarchy Gui
    
    # destroy frame and create it again to delete all labels and buttons
    
    set f $ModelHierarchy(ModelCanvas)

    destroy $f.fModels
    frame $f.fModels -bd 0 -bg $Gui(activeWorkspace)
    $f create window 0 0 -anchor nw -window $f.fModels
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyRedrawFrame
# Redraws the model hierarchy view.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyRedrawFrame {} {
	ModelHierarchyExit
	ModelHierarchyEnter
}




#-------------------------------------------------------------------------------
# .PROC ModelHierarchyDeleteNode
# A downsized copy of MainMrmlDeleteNode. Removes only the node's ID from idList and
# the node itself from the tree, doesn't call MainMrmlUpdate.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyDeleteNode {nodeType id} {
	global Mrml ModelRef ModelGroup EndModelGroup Hierarchy EndHierarchy

	upvar $nodeType Array

	MainMrmlClearList
	set Array(idListDelete) $id

	# Remove node's ID from idList
	set i [lsearch $Array(idList) $id]
	if {$i == -1} {return}
	set Array(idList) [lreplace $Array(idList) $i $i]

	# Remove node from tree, and delete it
	Mrml(dataTree) RemoveItem ${nodeType}($id,node)
	${nodeType}($id,node) Delete
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyUpdateGUI
# Refreshes both the models GUI and the ModelHierarchy GUI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyUpdateGUI {} {

	MainModelsDestroyGUI
	ModelsUpdateMRML
	ModelHierarchyRedrawFrame
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyCreate
# Creates a new hierarchy
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyCreate {} {
	global Model ModelRef Hierarchy EndHierarchy
	
	if {[llength $Model(idList)] == 0} {
		DevErrorWindow "Cannot create a hierarchy without any models."
		return
	}
	set newHierarchyNode [MainMrmlAddNode "Hierarchy"]
	# set default values
	$newHierarchyNode SetType "MEDICAL"
	$newHierarchyNode SetHierarchyID "H1"
	
	# add all existing models to hierarchy
	foreach m $Model(idList) {
		set newModelRefNode [MainMrmlAddNode "ModelRef"]
		$newModelRefNode SetmodelRefID [Model($m,node) GetModelID]
	}
	
	MainMrmlAddNode "EndHierarchy"
	
	ModelHierarchyUpdateGUI
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyDeleteAsk
# Asks for a confirmation of deleting the entire hierarchy
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyDeleteAsk {widget} {
	
	YesNoPopup important_question\
		[winfo rootx $widget]\
		[winfo rooty $widget]\
		"Do you really want to delete the hierarchy?" ModelHierarchyDelete
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyDelete
# Deletes the existing hierarchy
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyDelete {} {
	global ModelRef Hierarchy EndHierarchy ModelGroup EndModelGroup
	global Mrml(dataTree)
	
	MainModelsDestroyGUI
	
	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	
	while {$node != ""} {
		if {[string compare -length 8 $node "ModelRef"] == 0} {
			ModelHierarchyDeleteNode "ModelRef" [$node GetID]
		}
		if {[string compare -length 10 $node "ModelGroup"] == 0} {
			ModelHierarchyDeleteNode "ModelGroup" [$node GetID]
		}
		if {[string compare -length 13 $node "EndModelGroup"] == 0} {
			ModelHierarchyDeleteNode "EndModelGroup" [$node GetID]
		}
		if {[string compare -length 9 $node "Hierarchy"] == 0} {
			ModelHierarchyDeleteNode "Hierarchy" [$node GetID]
		}
		if {[string compare -length 12 $node "EndHierarchy"] == 0} {
			ModelHierarchyDeleteNode "EndHierarchy" [$node GetID]
		}
		
		set node [Mrml(dataTree) GetNextItem]
	}
	
	ModelHierarchyRedrawFrame
	MainUpdateMRML
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyCreateGroup
# Shows a dialog to ask for the name of a new model group.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyCreateGroup {} {
	global Gui

	if {[winfo exists .askforname] == 0} {
		toplevel .askforname -class Dialog -bg $Gui(activeWorkspace)
		wm title .askforname "New model group"
		eval {label .askforname.l1 -text "Enter the name of the new group:"} $Gui(WLA)
		eval {entry .askforname.e1} $Gui(WEA)
		eval {button .askforname.bOk -text "Ok" -width 8 -command "ModelHierarchyCreateGroupOk"} $Gui(WBA)
		eval {button .askforname.bCancel -text "Cancel" -width 8 -command "destroy .askforname"} $Gui(WBA)
		grid .askforname.l1
		grid .askforname.e1
		grid .askforname.bOk .askforname.bCancel -padx 5 -pady 3
		
		# make the dialog modal
		update idle
		grab set .askforname
		tkwait window .askforname
		grab release .askforname
	}
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyCreateGroupOk
# Creates a new model group.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyCreateGroupOk {} {
	global Mrml ModelGroup Color
	
	
	Mrml(dataTree) InitTraversal
	set tmpnode [Mrml(dataTree) GetNextItem]
	
	while {$tmpnode != ""} {
		if {[string compare -length 12 $tmpnode "EndHierarchy"] == 0} {
			break
		}
		set tmpnode [Mrml(dataTree) GetNextItem]
	}
	
	set node [MainMrmlInsertBeforeNode $tmpnode "ModelGroup"]
	set newID [$node GetID]

	# Set some ModelGroup properties
	set ModelGroup($newID,visibility) [$node GetVisibility]
	set ModelGroup($newID,opacity) [format %#.1f [$node GetOpacity]]
	set ModelGroup($newID,expansion) [$node GetExpansion]
	$node SetColor [Color(0,node) GetName]
	set ModelGroup($newID,colorID) 0
	$node SetName [.askforname.e1 get]
	
	MainMrmlInsertBeforeNode $tmpnode "EndModelGroup"

	destroy .askforname
	ModelHierarchyUpdateGUI
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyDeleteModelGroup
# Deletes a model group.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyDeleteModelGroup {modelgroup} {
	global Mrml ModelGroup EndModelGroup
	
	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	
	set depth -1
	
	while {$node != ""} {
		if {[string compare -length 10 $node "ModelGroup"] == 0} {
			if {([$node GetID] == $modelgroup) && ($depth == -1)} {
				# the model group is found
				set depth 0
				ModelHierarchyDeleteNode ModelGroup [$node GetID]
			} else {
				if {$depth > -1} {incr depth}
			}
		}
		if {[string compare -length 13 $node "EndModelGroup"] == 0} {
			if {$depth == 0} {
				# found the right EndModelGroup node
				ModelHierarchyDeleteNode EndModelGroup [$node GetID]
				set depth -1
			} else {
				if {$depth>-1} {incr depth -1}
			}
		}
		set node [Mrml(dataTree) GetNextItem]
	}
	ModelHierarchyUpdateGUI
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyMoveModel
# Moves a model (group) from one model group to another
# .ARGS
# id: id of the model
# targetGroup: name of the target model group
# modelgroup: is different from 0 if a modelgroup is moved
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyMoveModel {id targetGroup src_modelgroup {trg_modelgroup 1}} {
	global Mrml ModelRef Model ModelGroup EndModelGroup Color
	
	# destroy the whole models gui before doing any changes, because model id's
	# and model group id's won't remain the same and MainModelsDestroyGUI depends
	# on these id's
	
	MainModelsDestroyGUI
	
	# are we moving only one model or a whole group?
	if {$src_modelgroup == 0} {
		# move a single model only
		
		Mrml(dataTree) InitTraversal
		set node [Mrml(dataTree) GetNextItem]
		set depth -1
		
		set sourceModelID [Model($id,node) GetModelID]
				
		while {$node != ""} {
			if {[string compare -length 10 $node "ModelGroup"] == 0} {
				# are we moving to a model group?
				if {$trg_modelgroup == 1} {
					# if we are moving the model to root, insert it
					# before the first model group
					if {$targetGroup == "<root>"} {
						set newModelRefNode [MainMrmlInsertBeforeNode $node ModelRef]
						$newModelRefNode SetmodelRefID [Model($id,node) GetModelID]
						set targetGroup ""
					}
					if {$depth < 0} {
						if {[$node GetName] == $targetGroup} {
							set depth 0
						}
					} else {
						incr depth
					}
				}
				# else don't care about modelgroups
			}
			if {[string compare -length 13 $node "EndModelGroup"] == 0} {
				if {$trg_modelgroup == 1} {
					if {$depth == 0} {
						set newModelRefNode [MainMrmlInsertBeforeNode $node "ModelRef"]
						$newModelRefNode SetmodelRefID [Model($id,node) GetModelID]
					}
					incr depth -1
				}
			}
			if {[string compare -length 8 $node "ModelRef"] == 0} {
				if {[$node GetmodelRefID] == $sourceModelID} {
					# remove this node
					ModelHierarchyDeleteNode ModelRef [$node GetID]
				} else {
					if {[Model([SharedModelLookup [$node GetmodelRefID]],node) GetName] == $targetGroup} {
						# insert new node here
						set newModelRefNode [MainMrmlInsertBeforeNode $node ModelRef]
						$newModelRefNode SetmodelRefID $sourceModelID
					}
				}
			}
			
			set node [Mrml(dataTree) GetNextItem]
		}
		
		ModelHierarchyUpdateGUI
	} else {
		if {$trg_modelgroup == 0} {
			DevErrorWindow "Model groups can only be moved to other model groups!"
			return
		}
		
		# move a complete model group
		
		# first step: copy the entire model group to a temporary mrml tree
		# and delete it from the original tree
		
		vtkMrmlTree tempTree
		
		Mrml(dataTree) InitTraversal
		set node [Mrml(dataTree) GetNextItem]
		set depth -1
		set i 0
		
		while {$node != ""} {
			incr i
			if {[string compare -length 10 $node "ModelGroup"] == 0} {
				if {[$node GetID] == $id} {
					# this is the model group to be moved
					set depth 0
				}
				if {$depth >= 0} {
					incr depth
					set n ModelGroupTemp($i,node)
					vtkMrmlModelGroupNode $n
					$n SetModelGroupID [$node GetModelGroupID]
					$n SetName [$node GetName]
					$n SetDescription [$node GetDescription]
					$n SetColor [$node GetColor]
					$n SetVisibility [$node GetVisibility]
					$n SetOpacity [$node GetOpacity]
					tempTree AddItem $n
					ModelHierarchyDeleteNode ModelGroup [$node GetID]
				}
			}
			if {[string compare -length 8 $node "ModelRef"] == 0} {
				if {$depth > 0} {
					set n ModelRefTemp($i,node)
					vtkMrmlModelRefNode $n
					$n SetmodelRefID [$node GetmodelRefID]
					tempTree AddItem $n
					ModelHierarchyDeleteNode ModelRef [$node GetID]
				}
			}
			if {[string compare -length 13 $node "EndModelGroup"] == 0} {
				if {$depth > 0} {
					incr depth -1
					if {$depth == 0} {
						set depth -1
					}
					set n EndModelGroupNodeTemp($i,node)
					vtkMrmlEndModelGroupNode $n
					tempTree AddItem $n
					ModelHierarchyDeleteNode EndModelGroup [$node GetID]
				}
			}
			
			set node [Mrml(dataTree) GetNextItem]
		}
		
		# second step: find the target position in the actual mrml tree
		
		Mrml(dataTree) InitTraversal
		set node [Mrml(dataTree) GetNextItem]
		set success 0
		
		while {($node != "") && ($success == 0)} {
			if {[string compare -length 10 $node "ModelGroup"] == 0} {
				# if we are moving the group to root, insert it
				# before the first model group
				
				if {$targetGroup == "<root>"} {
					set targetNode $node
					set targetGroup ""
				}
				if {[$node GetName] == $targetGroup} {
					set targetNode [Mrml(dataTree) GetNextItem]
				}
			}
			set node [Mrml(dataTree) GetNextItem]
		}
		
		# third step: insert the contents of the temporary tree at
		# the desired location of the actual mrml tree
		
		tempTree InitTraversal
		set node [tempTree GetNextItem]
		
		while {$node != ""} {
			if {[string compare -length 10 $node "ModelGroup"] == 0} {
				set newNode [MainMrmlInsertBeforeNode $targetNode ModelGroup]
				$newNode SetModelGroupID [$node GetModelGroupID]
				$newNode SetName [$node GetName]
				$newNode SetDescription [$node GetDescription]
				$newNode SetColor [$node GetColor]
				$newNode SetVisibility [$node GetVisibility]
				$newNode SetOpacity [$node GetOpacity]
				set newID [$newNode GetID]
				
				# Set some ModelGroup properties
				set ModelGroup($newID,visibility) [$newNode GetVisibility]
				set ModelGroup($newID,opacity) [format %#.1f [$newNode GetOpacity]]
				set ModelGroup($newID,expansion) [$newNode GetExpansion]
				set colorname [$newNode GetColor]
				set ModelGroup($newID,colorID) 0
				foreach c $Color(idList) {
					if {[Color($c,node) GetName] == $colorname} {
						set ModelGroup($newID,colorID) $c
					}
				}
			}
			if {[string compare -length 13 $node "EndModelGroup"] == 0} {
				MainMrmlInsertBeforeNode $targetNode EndModelGroup
			}
			if {[string compare -length 8 $node "ModelRef"] == 0} {
				set newNode [MainMrmlInsertBeforeNode $targetNode ModelRef]
				$newNode SetmodelRefID [$node GetmodelRefID]
			}
			
			set node [tempTree GetNextItem]
		}
		
		# fourth step: remove all temporary nodes
		
		tempTree InitTraversal
		set node [tempTree GetNextItem]
		
		while {$node != ""} {
			$node Delete
			set node [tempTree GetNextItem]
		}
		
		ModelHierarchyUpdateGUI
		MainUpdateMRML
		
		tempTree RemoveAllItems
		tempTree Delete
	}
}


#-------------------------------------------------------------------------------
# .PROC ModelHierarchyDrag
# Handles the Drag and Drop functions
# .ARGS
# command: drag-and-drop command
# args: guess what...
# .END
#-------------------------------------------------------------------------------
proc ModelHierarchyDrag {command args} {
	global _dragging
	global _lastwidget
	global _dragwidget
	global _dragcursor
	global ModelHierarchy
	
	switch $command {
		init {
			set _lastwidget 0
			set _dragging 0
		}
		
		start {
			set w [lindex $args 0]
			set _dragging 1
			set _lastwidget $w
			set _dragwidget $w
			set _dragcursor [$w cget -cursor]
			$w config -cursor target
		}
		
		motion {
			if {!$_dragging} {return}
			
			set x [lindex $args 0]
			set y [lindex $args 1]
			set w [winfo containing $x $y]
			if {[info exists ModelHierarchy(ModelCanvasY)] == 0} {
				set ModelHierarchy(ModelCanvasYTop) [winfo rooty $ModelHierarchy(ModelCanvas)]
				set ModelHierarchy(ModelCanvasYBottom) [expr [winfo rooty $ModelHierarchy(ModelCanvas)]+[winfo height $ModelHierarchy(ModelCanvas)]]
			}
			# check if the canvas has to be scrolled
			if {$y<[expr $ModelHierarchy(ModelCanvasYTop)+10]} {
				if {[lindex [$ModelHierarchy(ModelCanvas) yview] 0] > 0} {
					# scroll if there is something hidden at the top
					# of the canvas
					$ModelHierarchy(ModelCanvas) yview scroll -1 units
				}
			}
			if {$y>[expr $ModelHierarchy(ModelCanvasYBottom)-10]} {
				if {[lindex [$ModelHierarchy(ModelCanvas) yview] 1] < 1} {
					# scroll if there is something hidden at the bottom
					# of the canvas
					$ModelHierarchy(ModelCanvas) yview scroll 1 units
				}
			}
			if {$w != $_lastwidget && [winfo exists $_lastwidget]} {
				event generate $_lastwidget <<DragLeave>>
			}
			set _lastwidget $w
			if {[winfo exists $w]} {
				event generate $w <<DragOver>>
			}
		}
		
		stop {
			if {!$_dragging} {return}
			set x [lindex $args 0]
			set y [lindex $args 1]
			set w [winfo containing $x $y]
			if {[winfo exists $w]} {
				event generate $w <<DragLeave>>
				event generate $w <<DragDrop>> -rootx $x -rooty $y
			}
			set _dragging 0
		}
		
		over {
			if {!$_dragging} {return}
			set w [lindex $args 0]
			$w configure -relief raised
		}
		
		leave {
			if {!$_dragging} {return}
			set w [lindex $args 0]
			$w configure -relief groove
		}
		
		drop {
			set w [lindex $args 0]
			set x [lindex $args 1]
			set y [lindex $args 2]
			$_dragwidget config -cursor $_dragcursor
			if {[string match {*lTrash} $w]} {
				regexp {.*\.l(.+)$} $_dragwidget match SourceID
				if {![string match {g*} $SourceID]} {
					DevErrorWindow "Models can't be deleted from the hierarchy."
					return
				} else {
					regexp {g(.+)} $SourceID match SourceGroupID
					ModelHierarchyDeleteModelGroup $SourceGroupID
				}
				return
			}
			# don't move if source and target are equal
			if {$_dragwidget != $w} {
				regexp {.*\.l(.+)$} $_dragwidget match SourceID
				regexp {.*\.l(.+)$} $w match TargetGroupID
				if {![string match {g*} $TargetGroupID]} {
					regexp {(.+)} $TargetGroupID match TargetID
					set TargetGroup [Model($TargetID,node) GetName]
					set target_is_modelgroup 0
				} else {
					regexp {g(.+)} $TargetGroupID match TargetID
					set target_is_modelgroup 1
					if {$TargetID == "root"} {
						set TargetGroup "<root>"
					} else {
						set TargetGroup [ModelGroup($TargetID,node) GetName]
					}
				}
				# check if we are moving a model group or just a single model
				set match ""
				regexp {g.+} $SourceID match
				if {$match == ""} {
					# move a single model
					ModelHierarchyMoveModel $SourceID $TargetGroup 0
				} else {
					# move a model group
					if {$target_is_modelgroup == 0} {
						DevErrorWindow "Model groups can only be moved to other model groups."
						return
					}
					regexp {g(.+)} $SourceID match SourceGroupID
					if {$SourceGroupID == "root"} {
						DevErrorWindow "Can't move root!"
						return
					}
					YesNoPopup ynp $x $y "Really move the model group?" \
					"ModelHierarchyMoveModel $SourceGroupID \"$TargetGroup\" 1"
				}
			}
		}
	}
}
