#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
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
# FILE:        Data.tcl
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
#   DataDeleteNode
#   DataCopyNode
#   DataPasteNode
#   DataEditNode
#   DataAddModel
#   DataAddMatrix
#   DataAddTransformFromSelection
#   DataAddTransform append firstSel lastSel
#   DataAddVolume
#   DataEnter
#   DataExit
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC DataInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataInit {} {
    global Data Module Path MRMLDefaults

    # Define Tabs
    set m Data
    set Module($m,row1List) "Help List"
    set Module($m,row1Name) "Help List"
    set Module($m,row1,tab) List

    # Module Summary Info
    set Module($m,overview) "Read models/volumes, view contents of current scene (MRML file)."

    # Define Procedures
    set Module($m,procGUI) DataBuildGUI
    set Module($m,procMRML) DataUpdateMRML
    set Module($m,procEnter) DataEnter
    set Module($m,procExit) DataExit

    # Define Dependencies
    set Module($m,depend) "Events"

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.39 $} {$Date: 2002/03/21 23:05:24 $}]

    set Data(index) ""
    set Data(clipboard) ""
}

#-------------------------------------------------------------------------------
# .PROC DataUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataUpdateMRML {} {
    global Gui Model Slice Module Color Volume Label 

    # List of nodes
    DataDisplayTree
}

#-------------------------------------------------------------------------------
# .PROC DataBuildGUI
# 
# .ARGS
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
The 3D Slicer can be thought of as a MRML browser.  MRML is the 3D Slicer's 
language for describing 3D scenes of medical data. 
<P>
The <B>List</B> tab 
lists the contents of the MRML file currently being viewed.  To save the 
current file, or open a different one, use the <B>File</B> menu. 
<P>
To view or edit an item's properties, double-click on it's name in the list. 
To copy, delete, or move it, click on it with the right mouse button to 
show a menu of options. 
<BR><B>TIP:</B> Observe the keyboard shortcuts on the menu and use these 
to quickly cut and paste items.

"
    regsub -all "\n" $help { } help
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
    
    eval {button $f.bVolume -image $Data(imgVolumeOff) \
        -command "DataAddVolume"} $Gui(WBA)
    set Data(bVolume) $f.bVolume
    bind $Data(bVolume) <Enter> \
        "$Data(bVolume) config -image $Data(imgVolumeOn)"
    bind $Data(bVolume) <Leave> \
        "$Data(bVolume) config -image $Data(imgVolumeOff)"

    eval {button $f.bModel  -image $Data(imgModelOff) \
        -command "DataAddModel"} $Gui(WBA)
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

    eval {button $f.bTransform  -text "Add Transform" \
        -command "DataAddTransformFromSelection"} $Gui(WBA)
    eval {button $f.bEnd  -text "Add Matrix" \
        -command "DataAddMatrix"} $Gui(WBA)

    pack $f.bTransform $f.bEnd -side left -padx $Gui(pad)

    #-------------------------------------------
    # List->Title frame
    #-------------------------------------------
    set f $fList.fTitle
    
    eval {label $f.lTitle -text "MRML File Contents (Current Scene):"} $Gui(WTA)
    pack $f.lTitle 

    #-------------------------------------------
    # List->List frame
    #-------------------------------------------
    set f $fList.fList

    set Data(fNodeList) [ScrolledListbox $f.list 0 0 -height 16 -selectmode extended]
    bind $Data(fNodeList) <Button-3>  {DataPostRightMenu %X %Y}
    bind $Data(fNodeList) <Double-1>  {DataEditNode}

    # initialize key-bindings (and hide class Listbox Control button ops)
    set Data(eventManager) { \
        {Listbox <Control-Button-1>  {}} \
        {Listbox <Control-B1-Motion>  {}} \
        {all <Control-e> {DataEditNode}} \
        {all <Control-x> {DataCutNode}} \
        {all <Control-v> {DataPasteNode}} \
        {all <Control-d> {DataDeleteNode}} }

#    bind all <Control-c> {DataCopyNode}

    pack $f.list -side top -expand 1 -fill both

    # Menu for right mouse button

    eval {menu $f.list.mRight} $Gui(WMA)
    set Data(rightMenu) $f.list.mRight
    set m $Data(rightMenu)
    set id 0

    set Data(rightMenu,Edit)   $id
    $m add command -label "Edit (Ctrl+e)" -command "DataEditNode"
    incr id
    set Data(rightMenu,Cut)    $id
    $m add command -label "Cut (Ctrl+x)" -command "DataCutNode"
    incr id
#    set Data(rightMenu,Copy)   $id
#    $m add command -label "Copy (Ctrl+c)" -command "DataCopyNode"
#    incr id
    set Data(rightMenu,Paste)  $id
    $m add command -label "Paste (Ctrl+v)" -command "DataPasteNode" \
        -state disabled
    incr id
    set Data(rightMenu,Delete)  $id
    $m add command -label "Delete (Ctrl+d)" -command "DataDeleteNode"
    $m add command -label "-- Close Menu --" -command "$Data(rightMenu) unpost"

}

#-------------------------------------------------------------------------------
# .PROC DataDisplayTree
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataDisplayTree {{index end}} {
    global Data
    
    # Clear old
    $Data(fNodeList) delete 0 end
    
    # Insert new: traverse the MRML tree and get Title for each node
    set depth 0
    set tree Mrml(dataTree)
    $tree InitTraversal
    set node [$tree GetNextItem]
    while {$node != ""} {
    #if {[$node GetLabelMap] == "1"} {
    #    set line "Label: $name"
    #} else {    
    #    set line "Volume: $name"
    #}
    #vtkMrmlOptionsNode {
    #    set name [$node GetContents]
    #    set line "Options: $name"
    #}
    #vtkMrmlLandmarkNode {
    #    set name [$node GetName]
    #    set line "Landmark: camera XYZ = [$node GetXYZ], focalPoint XYZ =[$node GetFXYZ], position = [$node GetPathPosition]"
    #}

    # Update indentation for display of this node
    set indent [$node GetIndent]
    if {$indent < 0} {
        incr depth $indent
    }

    # Indentation using tabs
    set tabs ""
    for {set i 0} {$i < $depth} {incr i} {
        set tabs "${tabs}   "
    }

    # Get node's title
    set title [$node GetTitle]

    # Insert node's title into list with proper indentation
    $Data(fNodeList) insert end ${tabs}$title

    # Update indentation for display of the following nodes
    if {$indent > 0} {
        incr depth $indent
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataPostRightMenu {x y} {
    global Data Gui

    # Get selection from listbox
    set index [$Data(fNodeList) curselection]

    # If no selection, then disable certain menu entries
    set m $Data(rightMenu)
    if {$index == ""} {
        foreach entry "Cut Edit Delete" {
            $m entryconfigure $Data(rightMenu,$entry) -state disabled
        }
    } else {
        foreach entry "Cut Edit Delete" {
            $m entryconfigure $Data(rightMenu,$entry) -state normal
        }
    }

    # Show menu
    $m post $x $y
}

#-------------------------------------------------------------------------------
# .PROC DataGetTypeFromNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataGetTypeFromNode {node} {

    if {[regexp {(.*)\((.*),} $node match nodeType id] == 0} {
        tk_messageBox -message "Ooops! node='$node'"
        return ""
    }
    return $nodeType
}

#-------------------------------------------------------------------------------
# .PROC DataGetIdFromNode
# 
# .ARGS
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataClipboardCopy {nodes} {
    global Data Mrml Volume Model Transform EndTransform Matrix Options
        global Color 
    
    # If the clipboard already has a node(s), delete it
    if {$Data(clipboard) != ""} {
        foreach node $Data(clipboard) {
            set nodeType [DataGetTypeFromNode $node]
            set id [DataGetIdFromNode $node]
            
            # For the next line to work, Volume, Model, etc need to
            # be on the "global" line of this procedure
            MainMrmlDeleteNode $nodeType $id
            RenderAll
        }
    }

    # Copy the node(s) to the clipboard
    set Data(clipboard) $nodes
    
    # Enable paste
    $Data(rightMenu) entryconfigure $Data(rightMenu,Paste) -state normal
}

#-------------------------------------------------------------------------------
# .PROC DataClipboardPaste
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataClipboardPaste {} {
    global Data Mrml
    
    set newNodes $Data(clipboard)
    set Data(clipboard) ""
    $Data(rightMenu) entryconfigure $Data(rightMenu,Paste) -state disabled

    return $newNodes
}

#-------------------------------------------------------------------------------
# .PROC DataCutNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataCutNode {} {
    global Data Mrml
    
    # Get the index of selected node(s)
    set selection [$Data(fNodeList) curselection]
    if {$selection == ""} {return}
 
    # If Transform selected, remove whole thing. Ignore End Tr. if unmatched.
    set remove [DataCheckSelectedTransforms $selection \
        [$Data(fNodeList) index end]]
    if {$remove == ""} {return}

    # Identify node(s)
    foreach node $remove {
    lappend nodes [Mrml(dataTree) GetNthItem $node]
    }

    # Remove node(s) from the MRML tree
    foreach node $nodes {
    Mrml(dataTree) RemoveItem $node 
    }
    
    # Copy to clipboard
    DataClipboardCopy $nodes
    
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC DataDeleteNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataDeleteNode {} {
    global Data Mrml Volume Model Transform EndTransform Matrix Color Options
    
    # Get the index of selected node(s)
    set selection [$Data(fNodeList) curselection]
    if {$selection == ""} {return}

    # If Transform selected, remove whole thing. Ignore End Tr. if unmatched.
    set remove [DataCheckSelectedTransforms $selection \
        [$Data(fNodeList) index end]]
    if {$remove == ""} {return}
    
    # Identify node(s)
    foreach node $remove {
    lappend nodes [Mrml(dataTree) GetNthItem $node]
    }
    
    foreach node $nodes {
    # Delete
    set nodeType [DataGetTypeFromNode $node]
    set id [DataGetIdFromNode $node]
    # For the next line to work, Volume, Model, etc need to
    # be on the "global" line of this procedure
    MainMrmlDeleteNode $nodeType $id
    }
    
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC DataCopyNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataCopyNode {} {
    global Data Mrml

    # Get the index of selected node(s)
    set selection [$Data(fNodeList) curselection]
    if {$selection == ""} {return}

    # Identify node(s)
    foreach node $selection {
        lappend nodes [Mrml(dataTree) GetNthItem $node]
    }
    # Copy to clipboard
    DataClipboardCopy $nodes

}

#-------------------------------------------------------------------------------
# .PROC DataPasteNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataPasteNode {} {
    global Data Mrml

    # If there's nothing to paste, do nichts
    if {$Data(clipboard) == ""} {
        return
    }

    # Empty list is a special case
    if {[$Data(fNodeList) index end] == 0} {
        foreach node [DataClipboardPaste] {    
        Mrml(dataTree) AddItem $node 
        }
        MainUpdateMRML
        return
    }

    # Get the index of selected node(s)
    set selection [$Data(fNodeList) curselection]

    if {$selection == ""} {
        tk_messageBox -message "First select an item to paste after."
        return
    }
    
    # Find the last selected node to paste after
    set last [expr [llength $selection] - 1]
    set lastSel [Mrml(dataTree) GetNthItem [lindex $selection $last]]
    
    # Paste from clipboard
    set newNodes [DataClipboardPaste]
    
    # Figure out which item each node should be pasted after
    set prevNodes "$lastSel [lrange $newNodes 0 [expr \
        [llength $newNodes] - 2]]"

    # Insert into MRML tree after the last selected node
    foreach node $newNodes prev $prevNodes {
        Mrml(dataTree) InsertAfterItem $prev $node
    }

    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC DataEditNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataEditNode {} {
    global Data Mrml Model

    # Get the selected node
    set selection [$Data(fNodeList) curselection]

    # Edit only one node
    if {[llength $selection] != 1} {
        tk_messageBox -message "Please select only one node to edit."
        return
    }

    set node [Mrml(dataTree) GetNthItem $selection]

    set class [$node GetClassName]
    switch $class {
    "vtkMrmlVolumeNode" {
        set id [DataGetIdFromNode $node]
        MainVolumesSetActive $id
        if {[IsModule Volumes] == 1} {
            Tab Volumes row1 Display
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
    "vtkMrmlTetraMeshNode" {
        set id [DataGetIdFromNode $node]
        MainTetraMeshSetActive $id
        if {[IsModule TetraMesh] == 1} {
            Tab TetraMesh row1 Visualize
        }
    }
    "vtkMrmlMatrixNode" {
        set id [DataGetIdFromNode $node]
        MainMatricesSetActive $id
        if {[IsModule Matrices] == 1} {
            Tab Matrices row1 Manual
        }
    }
    "vtkMrmlOptionsNode" {
        set id [DataGetIdFromNode $node]
        MainOptionsSetActive $id
        if {[IsModule Options] == 1} {
            Tab Options row1 Props
        }
    }
    "vtkMrmlModelRefNode" -
    "vtkMrmlModelGroupNode" -
    "vtkMrmlEndModelGroupNode" -
    "vtkMrmlEndHierarchyNode" -
    "vtkMrmlHierarchyNode" {
        if {[IsModule ModelHierarchy] == 1} {
            Tab ModelHierarchy row1 HDisplay
        }
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC DataAddModel
# 
# .ARGS
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataAddMatrix {} {
    global Data Matrix Mrml

    # Check that the matrix is being added after another matrix or transform
    set selection [$Data(fNodeList) curselection]
    if {$selection == ""} {
    tk_messageBox -message "Select a Transform to add the Matrix to."
    return
    }
    set lastSel [Mrml(dataTree) GetNthItem [lindex $selection end]]    
    set class [$lastSel GetClassName]
    if {$class != "vtkMrmlTransformNode" && $class != "vtkMrmlMatrixNode"} {
    tk_messageBox -message "Select a Transform to add the Matrix to."
    return
    }
    
    set i $Matrix(nextID)
    incr Matrix(nextID)
    lappend Matrix(idList) $i
    vtkMrmlMatrixNode Matrix($i,node)
    set n Matrix($i,node)
    $n SetID $i
    $n SetName manual$i
    Mrml(dataTree) InsertAfterItem $lastSel $n

    MainMatricesSetActive $i

    MainUpdateMRML    
}

#-------------------------------------------------------------------------------
# .PROC DataAddTransformFromSelection
#  
# Adds a transform from Selection on Mrml Node Tree
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataAddTransformFromSelection {} {
    global Data Mrml

    set firstSel ""
    set lastSel ""

    # Add Transform, Matrix, EndTransform
    # Transform will enclose selected nodes
    set selection [$Data(fNodeList) curselection]

    # Check that transform will only enclose transform-end transform pairs.
    set numTrans [DataCountTransforms $selection]
    
    # Empty list, no selection, or partial transform in selection: put Transform at end    
    if {[$Data(fNodeList) index end] == 0 || $selection == "" || $numTrans != 0} {
    set append 1
    } else {
    set append 0
    set firstSel [Mrml(dataTree) GetNthItem [lindex $selection 0]]
    set lastSel [Mrml(dataTree) GetNthItem [lindex $selection end]]
    }

    DataAddTransform append $firstSel $lastSel 
}

#-------------------------------------------------------------------------------
# .PROC DataAddTransform
#  
# Adds a transform with a matrix to the Mrml Node Tree
# This function can be called from anywhere, not just from Data
#
# Example usage:
# set matrixnum [ DataAddTransform 0 Model($first,node) Model($last,node) ]
#
# Returns the id of the Matrix created. The default name
# of the matrix is \"manual$i\" where $i is the id number
# of the matrix. But, you can change that easily enough...
#
# If append, firstSel and lastSel are not checked.
#
# .ARGS
# bool append if 1, simply appends the transform to the end of the tree
# vtkMrmlNode firstSel The first item to be included in the transform
# vtkMrmlNode lastSel  The last item to be included in the transform
# .END
#-------------------------------------------------------------------------------
proc DataAddTransform {append firstSel lastSel} {
    global Transform Matrix Mrml EndTransform

    ###########
    ### Get the ID of the next transform
    ### Create the Begin portion of the transform
    ###########

    ### Get the Next ID and increment that ID
    set i $Transform(nextID)
    incr Transform(nextID)

    ### Change the list of existing Transforms
    lappend Transform(idList) $i
    vtkMrmlTransformNode Transform($i,node)
    set n Transform($i,node)
    $n SetID $i
    if {$append == 1} {
    Mrml(dataTree) AddItem $n
    } else {
    Mrml(dataTree) InsertBeforeItem $firstSel $n
    }

    ###########
    ### Make a new matrix 
    ### Insert the matrix into the Mrml Tree
    ###########

    # Matrix
    set m $Matrix(nextID)
    incr Matrix(nextID)
    lappend Matrix(idList) $m
    vtkMrmlMatrixNode Matrix($m,node)
    set n Matrix($m,node)
    $n SetID $m
    $n SetName manual$i
    if {$append == 1} {
    Mrml(dataTree) AddItem $n
    } else {
    Mrml(dataTree) InsertBeforeItem $firstSel $n
    }
    MainMatricesSetActive $m

    ###########
    ### Make a new EndTransform and insert it into the Mrml Tree
    ### 
    ###########

    # EndTransform
    set i $EndTransform(nextID)
    incr EndTransform(nextID)
    lappend EndTransform(idList) $i
    vtkMrmlEndTransformNode EndTransform($i,node)
    set n EndTransform($i,node)
    $n SetID $i
    if {$append == 1} {
    Mrml(dataTree) AddItem $n
    } else {
    Mrml(dataTree) InsertAfterItem $lastSel $n
    }

    MainUpdateMRML

    ### Return the id of the matrix 

    return $m
}


#-------------------------------------------------------------------------------
# .PROC DataAddVolume
# 
# .ARGS
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

# Returns the number of open transforms in the selected area
proc DataCountTransforms {selection {start ""} {end ""}} {
    global Mrml
    
    set T "0"
    foreach line $selection {
    set node [Mrml(dataTree) GetNthItem $line]
    set class [$node GetClassName]
    switch $class {
        vtkMrmlTransformNode {
        incr T
        }
        vtkMrmlEndTransformNode {
        incr T -1
        } 
    }
    }
    return $T
}

# If partial Transform nodes were in selection, find the rest of the node 
# and add it to the selection.  Else if unmatched End Transform nodes were 
# selected, remove them from the selection.
proc DataCheckSelectedTransforms {selection lastItem} {
    global Mrml

    set numTrans [DataCountTransforms $selection]

    # Return if the selection contains only matching T-ET pairs
    if {$numTrans == 0} {
    return $selection
    }

    # If open Transforms are in selection ($numTrans>0), find the rest of 
    # their contents.
    if {$numTrans > 0} {
    set TList ""
    set line [lindex $selection end]
    incr line

    # T is the number of nested transforms we are inside; numTrans is the 
    # number of transforms whose contents we want to find.
    set T $numTrans
    
    while {$T > 0 && $line < $lastItem} {
        set node [Mrml(dataTree) GetNthItem $line]
        set class [$node GetClassName]
        switch $class {
        vtkMrmlTransformNode {
            incr T
        }
        vtkMrmlMatrixNode {
            if {$T <= $numTrans} {
            lappend TList $line
            }
        }
        vtkMrmlEndTransformNode {
            if {$T <= $numTrans} {
            lappend TList $line
            }
            incr T -1
        }
        }
        # Get the next line
        incr line
    }
    
    # Add the transform contents to the selection
    return [concat $selection $TList]
    }

    # If there are unmatched End Transform tags ($numTrans<0), remove them 
    # from the selection.
    set ETList ""
    set line [lindex $selection 0]
    set lastSel [lindex $selection end]
    # number of open transforms above selection: find ETs that match them
    set numOpenTrans [expr  - $numTrans]
    # T is the number of nested transforms we are inside
    set T $numOpenTrans
    
    while {$T > 0 && $line <= $lastSel} {
    set node [Mrml(dataTree) GetNthItem $line]
    set class [$node GetClassName]
    switch $class {
        vtkMrmlTransformNode {
        incr T
        }
        vtkMrmlEndTransformNode {
        if {$T <= $numOpenTrans} {
            lappend ETList $line
        }
        incr T -1
        }
    }
    # Get the next line
    incr line
    }

    #Remove items we are saving from the selection
    foreach item $ETList {
    set index [lsearch -exact $selection $item]
    set selection [lreplace $selection $index $index]
    }
    return $selection
}


#-------------------------------------------------------------------------------
# .PROC DataEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataEnter {} { 
    global Data

    pushEventManager $Data(eventManager)

}

#-------------------------------------------------------------------------------
# .PROC DataExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DataExit {} {

    popEventManager
}
