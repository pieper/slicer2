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
# FILE:        Data.tcl
# DATE:        01/18/2000 12:17
# LAST EDITOR: gering
# PROCEDURES:  
#   DataInit
#   DataUpdateMRML
#   DataBuildGUI
#   DataDisplayTree
#   DataPostRightMenu
#   DataGetTypeFromNode
#   DataGetIdFromNode
#   DataClipboardCopy
#   DataClipboardPaste
#   DataCutNode
#   DataCopyNode
#   DataPasteNode
#   DataEditNode
#   DataAddModel
#   DataAddMatrix
#   DataAddTransform
#   DataEndTransform
#   DataAddVolume
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC DataInit
# .END
#-------------------------------------------------------------------------------
proc DataInit {} {
	global Data Module Path MRMLDefaults

	# Define Tabs
	set m Data
	set Module($m,row1List) "Help List"
	set Module($m,row1Name) "Help List"
	set Module($m,row1,tab) List

	# Define Procedures
	set Module($m,procGUI) DataBuildGUI
	set Module($m,procMRML) DataUpdateMRML

	set Data(index) ""
	set Data(save) $Path(prefixSaveFile)
	set Data(clipboard) ""
}

#-------------------------------------------------------------------------------
# .PROC DataUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc DataUpdateMRML {} {
	global Gui Model Slice Module Color Volume Label

	# List of nodes
	DataDisplayTree
}

#-------------------------------------------------------------------------------
# .PROC DataBuildGUI
# .END
#-------------------------------------------------------------------------------
proc DataBuildGUI {} {
	global Gui Data Model Slice Module Label

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# List
	#   Btns
	#   Menu
	#   Title
	#   List
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Data $help
	MainHelpBuildGUI Data

	#-------------------------------------------
	# List frame
	#-------------------------------------------
	set fList $Module(Data,fList)
	set f $fList

	frame $f.fMenu  -bg $Gui(activeWorkspace)
	frame $f.fBtns  -bg $Gui(activeWorkspace)
	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fList  -bg $Gui(activeWorkspace)

	pack $f.fBtns $f.fMenu $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) 
	pack $f.fList -side top -expand 1 -padx $Gui(pad) -pady $Gui(pad) -fill both

	#-------------------------------------------
	# Images
	#-------------------------------------------

	set Data(imgSave) [image create photo -file \
		[ExpandPath [file join gui save.gif]]]

	set Data(imgOpen) [image create photo -file \
		[ExpandPath [file join gui open.gif]]]

	foreach img "Volume Model" {
		set Data(img${img}Off) [image create photo -file \
			[ExpandPath [file join gui [Uncap $img]Off.ppm]]]
		set Data(img${img}On) [image create photo -file \
			[ExpandPath [file join gui [Uncap $img]On.ppm]]]
	}

	#-------------------------------------------
	# List->Btns frame
	#-------------------------------------------
	set f $fList.fBtns
	
	set c {button $f.bVolume -image $Data(imgVolumeOff) \
		-command "DataAddVolume" $Gui(WBA)}; eval [subst $c]
	set Data(bVolume) $f.bVolume
	bind $Data(bVolume) <Enter> \
		"$Data(bVolume) config -image $Data(imgVolumeOn)"
	bind $Data(bVolume) <Leave> \
		"$Data(bVolume) config -image $Data(imgVolumeOff)"

	set c {button $f.bModel  -image $Data(imgModelOff) \
		-command "DataAddModel" $Gui(WBA)}; eval [subst $c]
	set Data(bModel) $f.bModel
	bind $Data(bModel) <Enter> \
		"$Data(bModel) config -image $Data(imgModelOn)"
	bind $Data(bModel) <Leave> \
		"$Data(bModel) config -image $Data(imgModelOff)"

	pack $f.bVolume $f.bModel -side left -padx $Gui(pad)

	#-------------------------------------------
	# List->Menu frame
	#-------------------------------------------
	set f $fList.fMenu

	set c {button $f.bTransform  -text "Add Transform" \
		-command "DataAddTransform" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bEnd  -text "End Transform" \
		-command "DataEndTransform" $Gui(WBA)}; eval [subst $c]

	pack $f.bTransform $f.bEnd -side left -padx $Gui(pad)

	#-------------------------------------------
	# List->Title frame
	#-------------------------------------------
	set f $fList.fTitle
	
	set c {label $f.lTitle -text "MRML File Contents:" $Gui(WTA)};
		eval [subst $c]
	pack $f.lTitle 

	#-------------------------------------------
	# List->List frame
	#-------------------------------------------
	set f $fList.fList

	set Data(fNodeList) [ScrolledListbox $f.list 0 0]
	bind $Data(fNodeList) <Button-3>  {DataPostRightMenu %X %Y}
	bind $Data(fNodeList) <Double-1>  {DataEditNode}
	bind all <Control-e> {DataEditNode}
	bind all <Control-x> {DataCutNode}
	bind all <Control-c> {DataCopyNode}
	bind all <Control-v> {DataPasteNode}
	pack $f.list -side top -expand 1 -fill both

	# Menu for right mouse button

	set c {menu $f.list.mRight $Gui(WMA)}; eval [subst $c];
	set Data(rightMenu) $f.list.mRight
	set m $Data(rightMenu)
	set id 0

	set Data(rightMenu,Edit)   $id
	$m add command -label "Edit (Ctrl+e)" -command "DataEditNode"
	incr id
	set Data(rightMenu,Cut)    $id
	$m add command -label "Cut (Ctrl+x)" -command "DataCutNode"
	incr id
	set Data(rightMenu,Copy)   $id
	$m add command -label "Copy (Ctrl+c)" -command "DataCopyNode"
	incr id
	set Data(rightMenu,Paste)  $id
	$m add command -label "Paste (Ctrl+v)" -command "DataPasteNode" \
		-state disabled
	incr id
	$m add command -label "-- Close Menu --" -command "$Data(rightMenu) unpost"

}

#-------------------------------------------------------------------------------
# .PROC DataDisplayTree
# .END
#-------------------------------------------------------------------------------
proc DataDisplayTree {{index end}} {
	global Data

	# Clear old
	$Data(fNodeList) delete 0 end

	# Insert new
	set depth 0
    set tree Mrml(dataTree)
	$tree InitTraversal
    set node [$tree GetNextItem]
    while {$node != ""} {

		set class [$node GetClassName]

		# Add node-dependent descriptions
		switch $class {
			vtkMrmlModelNode {
				set desc [$node GetDescription]
				set name [$node GetName]
				if {$desc == ""} {
					set desc [file root [file tail [$node GetFileName]]]
				}
				if {$name == ""} {set name $desc}
				set line "Model: $name"
			}
			vtkMrmlVolumeNode {
				set desc [$node GetDescription]
				set name [$node GetName]
				if {$desc == ""} {
					set desc [file root [file tail [$node GetFilePrefix]]]
				}
				if {$name == ""} {set name $desc}
				if {[$node GetLabelMap] == "1"} {
					set line "Label: $name"
				} else {	
					set line "Volume: $name"
				}
			}
			vtkMrmlColorNode {
				set desc [$node GetDescription]
				set name [$node GetName]
				if {$name == ""} {set name $desc}
				if {$desc == ""} {
					set desc [$node GetDiffuseColor]
				}
				set line "Color: $name"
			}
			vtkMrmlMatrixNode {
				set desc [$node GetDescription]
				set name [$node GetName]
				if {$name == ""} {set name $desc}
				set line "Matrix: $name"
			}
			vtkMrmlTransformNode {
				set line "Transform"
			}
			vtkMrmlEndTransformNode {
				set line "EndTransform"
			}
		}
		
		if {$class == "vtkMrmlEndTransformNode"} {
			set depth [expr $depth - 1]
		}

		set tabs ""
		for {set i 0} {$i < $depth} {incr i} {
			set tabs "${tabs}   "
		}
		$Data(fNodeList) insert end ${tabs}$line

		if {$class == "vtkMrmlTransformNode"} {
			incr depth
		}

		# Traverse
        set node [$tree GetNextItem]
	}
	if {$index == ""} {
		set index "end"
	}
	$Data(fNodeList) selection set $index $index
}

#-------------------------------------------------------------------------------
# .PROC DataPostRightMenu
# .END
#-------------------------------------------------------------------------------
proc DataPostRightMenu {x y} {
	global Data Gui

	# Get selection from listbox
	set index [$Data(fNodeList) curselection]

	# If no selection, then disable certain menu entries
	set m $Data(rightMenu)
	if {$index == ""} {
		foreach entry "Cut Copy Edit" {
			$m entryconfigure $Data(rightMenu,$entry) -state disabled
		}
	} else {
		foreach entry "Cut Copy Edit" {
			$m entryconfigure $Data(rightMenu,$entry) -state normal
		}
	}

	# Show menu
	$m post $x $y
}

#-------------------------------------------------------------------------------
# .PROC DataGetTypeFromNode
# .END
#-------------------------------------------------------------------------------
proc DataGetTypeFromNode {node} {

	if {[regexp {(.*)\((.*),} $node match nodeType id] == 0} {
		tk_messageBox -message "Ooops!"
		return ""
	}
	return $nodeType
}

#-------------------------------------------------------------------------------
# .PROC DataGetIdFromNode
# .END
#-------------------------------------------------------------------------------
proc DataGetIdFromNode {node} {

	if {[regexp {(.*)\((.*),} $node match nodeType id] == 0} {
		tk_messageBox -message "Ooops!"
		return ""
	}
	return $id
}

#-------------------------------------------------------------------------------
# .PROC DataClipboardCopy
# .END
#-------------------------------------------------------------------------------
proc DataClipboardCopy {node} {
	global Data Mrml Volume Model Transform Matrix Color
	
	# If the clipboard already has a node, delete it
	if {$Data(clipboard) != ""} {
		set nodeType [DataGetTypeFromNode $Data(clipboard)]
		set id [DataGetIdFromNode $Data(clipboard)]
		# For the next line to work, Volume, Model, etc need to
		# be on the "global" line of this procedure
		MainMrmlDeleteNode $nodeType $id
	}

	# Copy the node to the clipboard
	set Data(clipboard) $node
	
	# Enable paste
	$Data(rightMenu) entryconfigure $Data(rightMenu,Paste) -state normal
}

#-------------------------------------------------------------------------------
# .PROC DataClipboardPaste
# .END
#-------------------------------------------------------------------------------
proc DataClipboardPaste {} {
	global Data Mrml
	
	set newNode $Data(clipboard)
	set Data(clipboard) ""
	$Data(rightMenu) entryconfigure $Data(rightMenu,Paste) -state disabled

	return $newNode
}

#-------------------------------------------------------------------------------
# .PROC DataCutNode
# .END
#-------------------------------------------------------------------------------
proc DataCutNode {} {
	global Data Mrml

	# Get the selected node
	set n [$Data(fNodeList) curselection]
	if {$n == ""} {return}
	set node [Mrml(dataTree) GetNthItem $n]

	# Identify nodeType
	set node [Mrml(dataTree) GetNthItem $n]

	# Copy to clipboard
	DataClipboardCopy $node

	# Remove from MRML tree
	Mrml(dataTree) RemoveItem $node

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC DataCopyNode
# .END
#-------------------------------------------------------------------------------
proc DataCopyNode {} {
	global Data Mrml

	# Get the selected node
	set n [$Data(fNodeList) curselection]
	if {$n == ""} {return}
	set node [Mrml(dataTree) GetNthItem $n]

	# Copy to clipboard
	DataClipboardCopy $node
}

#-------------------------------------------------------------------------------
# .PROC DataPasteNode
# .END
#-------------------------------------------------------------------------------
proc DataPasteNode {} {
	global Data Mrml

	# If there's nothing to paste, do nichts
	if {$Data(clipboard) == ""} {
		return
	}

	# Get the selected node
	set n [$Data(fNodeList) curselection]
	if {$n == ""} {
		tk_messageBox -message "First select an item to paste after."
		return
	}
	set node [Mrml(dataTree) GetNthItem $n]

	# Paste from clipboard
	set newNode [DataClipboardPaste]

	# Insert into MRML tree after the selected node
	Mrml(dataTree) InsertAfterItem $node $newNode

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC DataEditNode
# .END
#-------------------------------------------------------------------------------
proc DataEditNode {} {
	global Data Mrml Model

	# Get the selected node
	set n [$Data(fNodeList) curselection]
	if {$n == ""} {return}
	set node [Mrml(dataTree) GetNthItem $n]

	set class [$node GetClassName]
	switch $class {
	"vtkMrmlVolumeNode" {
		set id [DataGetIdFromNode $node]
		MainVolumesSetActive $id
		if {[IsModule Volumes] == 1} {
			Tab Volumes row1 Props
		}
	}
	"vtkMrmlModelNode" {
		set id [DataGetIdFromNode $node]
		MainModelsSetActive $id
		if {[IsModule Models] == 1} {
			set Model(propertyType) Basic
			Tab Models row1 Props
		}
	}
	"vtkMrmlMatrixNode" {
		set id [DataGetIdFromNode $node]
		MainMatricesSetActive $id
		if {[IsModule Matrices] == 1} {
			Tab Matrices row1 Props
		}
	}
	}
}

#-------------------------------------------------------------------------------
# .PROC DataAddModel
# .END
#-------------------------------------------------------------------------------
proc DataAddModel {} {
	global Model Module

	if {[IsModule Models] == 1} {
		set Model(propertyType) Basic
		ModelsSetPropertyType
		MainModelsSetActive NEW
		set Model(freeze) 1
		Tab Models row1 Props
		set Module(freezer) "Data row1 List"
	}
}

#-------------------------------------------------------------------------------
# .PROC DataAddMatrix
# .END
#-------------------------------------------------------------------------------
proc DataAddMatrix {} {
	global Matrix Module

	if {[IsModule Matrixs] == 1} {
		set Matrix(propertyType) Basic
		MatrixSetPropertyType
		MainMatricesSetActive NEW
		set Matrix(freeze) 1
		Tab Matrices row1 Props
		set Module(freezer) "Data row1 List"
	}
}

#-------------------------------------------------------------------------------
# .PROC DataAddTransform
# .END
#-------------------------------------------------------------------------------
proc DataAddTransform {} {
}

#-------------------------------------------------------------------------------
# .PROC DataEndTransform
# .END
#-------------------------------------------------------------------------------
proc DataEndTransform {} {
}


#-------------------------------------------------------------------------------
# .PROC DataAddVolume
# .END
#-------------------------------------------------------------------------------
proc DataAddVolume {} {
	global Volume Module

	if {[IsModule Volumes] == 1} {
		set Volume(propertyType) Basic
		VolumesSetPropertyType
		MainVolumesSetActive NEW
		set Volume(freeze) 1
		Tab Volumes row1 Props
		set Module(freezer) "Data row1 List"
	}
}


