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
# FILE:        MainModelGroups.tcl
# PROCEDURES:  
#	MainModelGroupsCreateGUI
#	MainModelGroupsPopupCallback
#	MainModelGroupsSetActive
#	MainModelGroupsSetVisibility
#	MainModelGroupsSetOpacityInit
#	MainModelGroupsSetOpacity
#	MainModelGroupsSetExpansion
#	MainModelGroupsDeleteGUI
#	MainModelGroupsDete
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsCreateGUI
# Makes the popup menu that comes up when you right-click a model group.
# This is made for each new model group.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsCreateGUI {f m {hlevel 0}} {
	global Gui ModelGroup Color

	set ModelGroup(frame) $f
	
	# If the GUI already exists, then just change name.
	if {[info command $f.cg$m] != ""} {
		$f.cg$m config -text "[ModelGroup($m,node) GetName]"
		return 0
	}

	# Name / Visible
		
	eval {checkbutton $f.cg$m \
		-text [ModelGroup($m,node) GetName] -variable ModelGroup($m,visibility) \
		-width 17 -indicatoron 0 \
		-command "MainModelGroupsSetVisibility $m; Render3D"} $Gui(WCA)
	$f.cg$m configure -bg [MakeColorNormalized \
			[Color($ModelGroup($m,colorID),node) GetDiffuseColor]]
	$f.cg$m configure -selectcolor [MakeColorNormalized \
			[Color($ModelGroup($m,colorID),node) GetDiffuseColor]]
	
	# Add a tool tip if the string is too long for the button
	if {[string length [ModelGroup($m,node) GetName]] > [$f.cg$m cget -width]} {
		TooltipAdd $f.cg$m "[ModelGroup($m,node) GetName]"
	}

	eval {checkbutton $f.hcg$m \
		-text "-" -variable ModelGroup($m,expansion) \
		-width 1 -indicatoron 0 \
		-command "MainModelGroupsSetExpansion $f $f.hcg$m $m; Render3D"} $Gui(WCA)
	
	eval {label $f.lg1_$m -text "" -width 1} $Gui(WLA)
	
	# menu
	eval {menu $f.cg$m.men} $Gui(WMA)
	set men $f.cg$m.men
	$men add command -label "Change Color..." -command \
		"MainModelGroupsSetActive $m; ShowColors MainModelGroupsPopupCallback"
	$men add command -label "-- Close Menu --" -command "$men unpost"
	bind $f.cg$m <Button-3> "$men post %X %Y"
	
	# Opacity
	eval {entry $f.eg${m} -textvariable ModelGroup($m,opacity) -width 3} $Gui(WEA)
	bind $f.eg${m} <Return> "MainModelGroupsSetOpacity $m 2; Render3D"
	bind $f.eg${m} <FocusOut> "MainModelGroupsSetOpacity $m 2; Render3D"
	eval {scale $f.sg${m} -from 0.0 -to 1.0 -length 40 \
		-variable ModelGroup($m,opacity) \
		-command "MainModelGroupsSetOpacityInit $m $f.sg$m 2" \
		-resolution 0.1} $Gui(WSA) {-sliderlength 14 \
		-troughcolor [MakeColorNormalized \
			[Color($ModelGroup($m,colorID),node) GetDiffuseColor]]}

	set l1_command $f.lg1_${m}
	set c_command $f.cg${m}

	for {set i 0} {$i<$hlevel} {incr i} {
		lappend l1_command "-"
	}
	
	for {set i 5} {$i>$hlevel} {incr i -1} {
		lappend c_command "-"
	}
	
	eval grid $l1_command $f.hcg${m} $c_command $f.eg${m} $f.sg${m} -pady 2 -padx 2 -sticky we
	if {$ModelGroup($m,expansion) == 1} {
		grid remove $f.eg${m} $f.sg${m}
	}
	
	foreach column {0 1 2 3 4 5} {
		# define a minimum size for each column, because otherwise
		# the empty columns used for building the tree would not
		# be drawn
		# maybe this should be moved some time to another procedure... (TODO)
		grid columnconfigure $f $column -minsize 8
	}

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsPopupCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsPopupCallback {} {
	global Label ActiveModelGroup ModelGroup Color Model

	set mg $ActiveModelGroup
	if {$mg == ""} {return}

	ModelGroup($mg,node) SetColor $Label(name)
	
	set ModelGroup($mg,colorID) [lindex $Color(idList) 1]
	foreach c $Color(idList) {
		if {[Color($c,node) GetName] == $Label(name)} {
			set colorid $c
			set ModelGroup($mg,colorID) $c
		}
	}
	
	if {$ModelGroup($mg,expansion) == 0} {		
		# if the model group is collapsed, change the colors for all
		# dependent models
		SharedGetModelsInGroup $mg models
		foreach m $models {
			MainModelsSetColor $m $Label(name)
		}
	}
	
	MainUpdateMRML
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsSetActive {m} {
	global ActiveModelGroup
	
	set ActiveModelGroup $m
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsSetVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsSetVisibility {modelgroup {value ""}} {
	global Model Module
	global Mrml(dataTree) ModelGroup

	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	
	set traversingModelGroup 0
	
	while {$node != ""} {

		if {[string compare -length 10 $node "ModelGroup"] == 0} {
			if {$traversingModelGroup > 0} {
				incr traversingModelGroup
				set ModelGroup([$node GetID],visibility) $ModelGroup($modelgroup,visibility)
				$node SetVisibility $ModelGroup($modelgroup,visibility)
			}
			if {[$node GetID] == $modelgroup} {
				incr traversingModelGroup
				$node SetVisibility $ModelGroup($modelgroup,visibility)
			}
		}
		if {[string compare -length 13 $node "EndModelGroup"] == 0} {
			if {$traversingModelGroup > 0} {
				incr traversingModelGroup -1
			}
		}
		
		if {([string compare -length 8 $node "ModelRef"] == 0) && ($traversingModelGroup > 0)} {
			set m [SharedModelLookup [$node GetModelRefID]]
			
			if {$value != ""} {
			    set ModelGroup($modelgroup,visibility) $value
			}
			Model($m,node)  SetVisibility $ModelGroup($modelgroup,visibility)
			set Model($m,visibility) $ModelGroup($modelgroup,visibility)
		        foreach r $Module(Renderers) {
			    Model($m,actor,$r) SetVisibility [Model($m,node) GetVisibility] 
			}
			# If this is the active model, update GUI
			if {$m == $Model(activeID)} {
			    set Model(visibility) [Model($m,node) GetVisibility]
			}
		}
		set node [Mrml(dataTree) GetNextItem]
	}
}

	
#-------------------------------------------------------------------------------
# .PROC MainModelGroupsSetOpacityInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsSetOpacityInit {m widget {grouponly 0} {value ""}} {

	$widget config -command "MainModelGroupsSetOpacity $m $grouponly; Render3D"
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsSetOpacity
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsSetOpacity {modelgroup {grouponly 0}} {
	global Model ModelGroup
	global Mrml(dataTree)

	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	
	set traversingModelGroup 0
	
	if {$grouponly == 2} {
		# special case: change opacity for the group only, if the group
		# is expanded; otherwise change it for each model in the group
		if {$ModelGroup($modelgroup,expansion) == 1} {
			set grouponly 1
		} else {
			set grouponly 0
		}
	}
	
	while {$node != ""} {

		if {[string compare -length 10 $node "ModelGroup"] == 0} {
			if {$traversingModelGroup > 0} {
				incr traversingModelGroup
				if {$grouponly != 1} {
					set ModelGroup([$node GetID],opacity) [format %#.1f $ModelGroup($modelgroup,opacity)]
					$node SetOpacity $ModelGroup($modelgroup,opacity)
				}
			}
			if {[$node GetID] == $modelgroup} {
				incr traversingModelGroup
				$node SetOpacity $ModelGroup($modelgroup,opacity)
			}
		}
		if {[string compare -length 13 $node "EndModelGroup"] == 0} {
			if {$traversingModelGroup > 0} {
				incr traversingModelGroup -1
			}
		}
		
		if {([string compare -length 8 $node "ModelRef"] == 0) && ($traversingModelGroup > 0)} {
			if {$grouponly != 1} {
				set m [SharedModelLookup [$node GetModelRefID]]
				Model($m,node) SetOpacity $ModelGroup($modelgroup,opacity)
				$Model($m,prop,viewRen) SetOpacity [Model($m,node) GetOpacity]
				set Model($m,opacity) [format %#.1f $ModelGroup($modelgroup,opacity)]
				# If this is the active model, update GUI
				if {$m == $Model(activeID)} {
					set Model(opacity) [Model($m,node) GetOpacity]
				}
			}
		}
		set node [Mrml(dataTree) GetNextItem]
	}
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsSetExpansion
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsSetExpansion {frame widget mg {nochange 0}} {
	global ModelGroup Model OldColors OldOpacities RemovedModels
	
	if {$ModelGroup($mg,expansion) == 0} {
		ModelGroup($mg,node) SetExpansion 0
		# catch is necessary, because sometimes the widget parameter is empty
		# (especially when this procedure is called during a recursion)		
		catch {$widget config -text "+"}
		grid $frame.eg$mg $frame.sg$mg
		set models {}
		set modelgroups {}
		if {!$nochange} {
			SharedGetModelsInGroup $mg models 0
		} else {
			SharedGetModelsInGroup $mg models
		}
		SharedGetModelGroupsInGroup $mg modelgroups
		foreach m $models {
			if {$RemovedModels($m) == 0} {
				grid remove $frame.l1_$m $frame.c$m $frame.e$m $frame.s$m
				set OldColors($m) [Model($m,node) GetColor]
				set OldOpacities($m) [Model($m,node) GetOpacity]
				set RemovedModels($m) 1
			}
			MainModelsSetColor $m [ModelGroup($mg,node) GetColor]
			MainModelsSetOpacity $m [ModelGroup($mg,node) GetOpacity]
		}
		foreach m $modelgroups {
			grid remove $frame.lg1_$m $frame.hcg$m $frame.cg$m $frame.eg$m $frame.sg$m
		}
	} else {
		ModelGroup($mg,node) SetExpansion 1
		# catch is necessary, because sometimes the widget parameter is empty
		# (especially when this procedure is called during a recursion)
		catch {$widget config -text "-"}
		grid remove $frame.eg$mg $frame.sg$mg
		set models {}
		set modelgroups {}
		if {!$nochange} {
			SharedGetModelsInGroup $mg models 1
		} else {
			SharedGetModelsInGroup $mg models
		}
		SharedGetModelGroupsInGroup $mg modelgroups
		foreach m $models {
			if {($Model($m,expansion) == 1) && ($RemovedModels($m) == 1)} {
				# only show the models of expanded groups
				MainModelsSetColor $m $OldColors($m)
				MainModelsSetOpacity $m [format %#.1f $OldOpacities($m)]
				grid $frame.l1_$m $frame.c$m $frame.e$m $frame.s$m
				set RemovedModels($m) 0
			}
		}
		foreach m $modelgroups {
			grid $frame.lg1_$m $frame.hcg$m $frame.cg$m $frame.eg$m $frame.sg$m
			MainModelGroupsSetExpansion $frame "" $m 1
		}
		MainUpdateMRML
	}

	Render3D
	ModelsConfigScrolledGUI
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsDeleteGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsDeleteGUI {f mg} {

	# If the GUI is already deleted, return
	if {[info command $f.cg$mg] == ""} {
		return 0
	}

	# Destroy TK widgets
	destroy $f.cg$mg
	destroy $f.hcg$mg
	destroy $f.eg$mg
	destroy $f.sg$mg
	destroy $f.lg1_$mg

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainModelGroupsDelete
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainModelGroupsDelete {f mg} {
	global ModelGroup Gui

	foreach name [array names ModelGroup] {
		if {[string first "$mg," $name] == 0} {
			unset ModelGroup($name)
		}
	}

	MainModelGroupsDeleteGUI $f $mg
}
