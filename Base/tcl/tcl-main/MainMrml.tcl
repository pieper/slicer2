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
# FILE:        MainMrml.tcl
# PROCEDURES:  
#   MainMrmlInit
#   MainMrmlInitIdLists
#   MainMrmlUpdateMRML
#   MainMrmlDumpTree
#   MainMrmlPrint
#   MainMrmlClearList
#   MainMrmlAddNode nodeType
#   MainMrmlInsertBeforeNode nodeType
#   MainMrmlInsertAfterNode nodeType
#   MainMrmlUndoAddNode
#   MainMrmlDeleteNodeDuringUpdate
#   MainMrmlDeleteNode
#   MainMrmlDeleteAll
#   MainMrmlSetFile
#   MainMrmlRead mrmlFile
#   MainMrmlBuildTreesVersion2.0
#   MainMrmlReadVersion1.0
#   MainMrmlBuildTreesVersion1.0
#   MainMrmlAddColors
#   MainMrmlCheckColors
#   MainMrmlRelativity
#   MainMrmlWrite
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainMrmlInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInit {} {
    global Mrml

    # List of all types of MRML nodes understood by slicer
    set Mrml(nodeTypeList) "Model Volume Color Transform \
        EndTransform Matrix TransferFunction WindowLevel \
        TFPoint ColorLUT Options Fiducials EndFiducials \
        Point Path EndPath Landmark \
        Hierarchy EndHierarchy ModelGroup EndModelGroup ModelRef \
        Scenes EndScenes VolumeState EndVolumeState CrossSection \
        SceneOptions ModelState Locator TetraMesh"

    MainMrmlInitIdLists 

    # Read MRML defaults file for version 1.0
    set fileName [ExpandPath "Defaults.mrml"]
    if {[CheckFileExists $fileName] == "0"} {
        set msg "Unable to read file MRML defaults file '$fileName'"
        puts $msg
        tk_messageBox -message $msg
        exit    
    }
    MRMLReadDefaults $fileName

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainMrml \
        {$Revision: 1.57 $} {$Date: 2002/03/27 15:51:31 $}]

    set Mrml(colorsUnsaved) 0
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlInitIdLists
# 
# Init the Id list for each data type
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInitIdLists {} {
    global Mrml 
    eval {global} $Mrml(nodeTypeList)
     
    foreach node $Mrml(nodeTypeList) {
    set ${node}(nextID) 0
    set ${node}(idList) ""
    set ${node}(idListDelete) ""
    }
    # Volumes are a special case because the "None" always exists
    
    set Volume(idList) 0
    set Volume(nextID) 1
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlUpdateMRML {} {
    global Mrml

    # Compute geometric relationships
    if {[info command Mrml(dataTree)] != ""} {
        Mrml(dataTree) ComputeTransforms
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDumpTree
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDumpTree {type} {
    global Mrml

    set tree Mrml(${type}Tree)
    $tree InitTraversal
    set node [$tree GetNextItem]
    while {$node != ""} {
        puts "dump='$node'"
        set node [$tree GetNextItem]
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlPrint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlPrint {tags} {

    set level 0
    foreach pair $tags {
        set tag  [lindex $pair 0]
        set attr [lreplace $pair 0 0]

        # Process EndTransform & EndFiducials & EndPath & EndModelGroup & EndHierarchy
        if {$tag == "EndTransform" || $tag == "EndFiducials" || $tag == "EndPath" || $tag == "EndModelGroup" || $tag == "EndHierarchy"} {
            set level [expr $level - 1]
        }
        set indent ""
        for {set i 0} {$i < $level} {incr i} {
            set indent "$indent  "
        }

        puts "${indent}$tag"

        # Process Transform & Fiducials & Path & ModelGroup & Hierarchy
        if {$tag == "Transform" || $tag == "Fiducials" || $tag == "Path" || $tag == "ModelGroup" || $tag == "Hierarchy"} {
            incr level
        }
        set indent ""
        for {set i 0} {$i < $level} {incr i} {
            set indent "$indent  "
        }

        foreach a $attr {
            set key   [lindex $a 0]
            set value [lreplace $a 0 0]
            puts "${indent}  $key=$value"
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlClearList
# 
# Delete the Id list for each data type
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlClearList {} {
    global Mrml 
    eval {global} $Mrml(nodeTypeList)
    
    foreach node $Mrml(nodeTypeList) {
    set ${node}(idListDelete) ""
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlAddNode
#
#  Adds a node to the data tree.
#  Returns the MrmlMode
# 
# .ARGS
#  str nodeType the type of node, \"Volume\", \"Color\", etc.
# .END
#-------------------------------------------------------------------------------
proc MainMrmlAddNode {nodeType  {globalArray ""}} {
    global Mrml

    if {$globalArray == ""} {
    set globalArray $nodeType
    }

    # the #0 puts the array in global scope
    upvar #0 $globalArray Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    # Add ID to idList
    set i $Array(nextID)
    incr Array(nextID)
    lappend Array(idList) $i

    # Put the None volume at the end
    if {$globalArray == "Volume"} {
        set j [lsearch $Array(idList) $Array(idNone)]
        set Array(idList) "[lreplace $Array(idList) $j $j] $Array(idNone)"
    }

    # Create vtkMrmlNode
    set n ${globalArray}($i,node)
    vtkMrml${nodeType}Node $n
    $n SetID $i

    # Add node to tree
    Mrml($tree) AddItem $n

    # Return node
    return ${globalArray}($i,node)
}


#-------------------------------------------------------------------------------
# .PROC MainMrmlInsertBeforeNode
#
#  Adds a node to the data tree right after NodeBefore
#  Returns the MrmlMode
# 
# .ARGS
#  str nodeType the type of node, \"Volume\", \"Color\", etc.
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInsertBeforeNode {nodeBefore nodeType} {
    global Mrml

    # the #0 puts the nodeType in global scope
    upvar #0 $nodeType Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    # Add ID to idList
    set i $Array(nextID)
    incr Array(nextID)
    lappend Array(idList) $i

    # Put the None volume at the end
    if {$nodeType == "Volume"} {
        set j [lsearch $Array(idList) $Array(idNone)]
        set Array(idList) "[lreplace $Array(idList) $j $j] $Array(idNone)"
    }

    # Create vtkMrmlNode
    set n ${nodeType}($i,node)

    vtkMrml${nodeType}Node $n
    $n SetID $i

    # Add node to tree
    Mrml($tree) InsertBeforeItem $nodeBefore $n

    # Return node
    return ${nodeType}($i,node)
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlInsertAfterNode
#
#  Adds a node to the data tree right after NodeBefore
#  Returns the MrmlNode
# 
# .ARGS
#  str nodeType the type of node, \"Volume\", \"Color\", etc.
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInsertAfterNode {nodeBefore nodeType} {
    global Mrml

    # the #0 puts the nodeType in global scope
    upvar #0 $nodeType Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    # Add ID to idList
    set i $Array(nextID)
    incr Array(nextID)
    lappend Array(idList) $i

    # Put the None volume at the end
    if {$nodeType == "Volume"} {
        set j [lsearch $Volume(idList) $Volume(idNone)]
        set Volume(idList) "[lreplace $Volume(idList) $j $j] $Volume(idNone)"
    }

    # Create vtkMrmlNode
    set n ${nodeType}($i,node)

    vtkMrml${nodeType}Node $n
    $n SetID $i

    # Add node to tree
    Mrml($tree) InsertAfterItem $nodeBefore $n

    # Return node
    return ${nodeType}($i,node)
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlUndoAddNode
# 

# use this routine to remove a node that was accidentally added.
#
# Don't call this routine to delete a node, it should only
# happen if adding one fails (i.e reading a volume from disk fails).
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlUndoAddNode {nodeType n} {
    global Mrml

    # the #0 puts the nodeType in global scope
    upvar #0 $nodeType Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    # Remove node's ID from idList
    set id [$n GetID]
    set i [lsearch $Array(idList) $id]
    if {$i == -1} {return}
    set Array(idList) [lreplace $Array(idList) $i $i]
    set Array(nextID) [expr $Array(nextID) - 1]

    # Remove node from tree, and delete it
    Mrml($tree) RemoveItem $n
    $n Delete
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNodeDuringUpdate
# Call this routine to delete a node during MainUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNodeDuringUpdate {nodeType id} {
    global Mrml

    # the #0 puts the nodeType in global scope
    upvar #0 $nodeType Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    lappend Array(idListDelete) $id

    # Remove node's ID from idList
    set i [lsearch $Array(idList) $id]
    if {$i == -1} {return}
    set Array(idList) [lreplace $Array(idList) $i $i]

    # Remove node from tree, and delete it
    Mrml($tree) RemoveItem ${nodeType}($id,node)
    ${nodeType}($id,node) Delete
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNode {nodeType id} {
    global Mrml 
    
    # the #0 puts the nodeType in global scope
    upvar #0 $nodeType Array

    set tree "dataTree"
    if {$nodeType == "Color"} {
        set tree "colorTree"
    }

    MainMrmlClearList
    set Array(idListDelete) $id

    # Remove node's ID from idList
    set i [lsearch $Array(idList) $id]
    if {$i == -1} {return}
    set Array(idList) [lreplace $Array(idList) $i $i]

    # Remove node from tree, and delete it
    Mrml($tree) RemoveItem ${nodeType}($id,node)
    ${nodeType}($id,node) Delete

    MainUpdateMRML

    MainMrmlClearList
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteAll
# 
#  Delete all volumes, models and transforms.
#
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteAll {} {
    global Mrml 
    eval {global} $Mrml(nodeTypeList)    

    # Volumes are a special case because the "None" always exists
    foreach id $Volume(idList) {
        if {$id != $Volume(idNone)} {

            # Add to the deleteList
            lappend Volume(idListDelete) $id

            # Remove from the idList
            set i [lsearch $Volume(idList) $id]
            set Volume(idList) [lreplace $Volume(idList) $i $i]

            # Remove node from tree, and delete it
            Mrml(dataTree) RemoveItem Volume($id,node)
            Volume($id,node) Delete
        }
    }

    # dataTree
    foreach node $Mrml(nodeTypeList) {
        if {$node != "Volume" && $node != "Color"} {
        upvar #0 $node Array
        
        foreach id $Array(idList) {

            # Add to the deleteList
            lappend Array(idListDelete) $id

            # Remove from the idList
            set i [lsearch $Array(idList) $id]
            set Array(idList) [lreplace $Array(idList) $i $i]

            # Remove node from tree, and delete it
            Mrml(dataTree) RemoveItem ${node}($id,node)
            ${node}($id,node) Delete

        }
        }
    }

    # colorTree
    foreach node "Color" {
        upvar #0 $node Array

        foreach id $Array(idList) {

            # Add to the deleteList
            lappend Array(idListDelete) $id

            # Remove from the idList
            set i [lsearch $Array(idList) $id]
            set Array(idList) [lreplace $Array(idList) $i $i]

            # Remove node from tree, and delete it
            Mrml(colorTree) RemoveItem ${node}($id,node)
            ${node}($id,node) Delete
        }
    }

    MainUpdateMRML

    MainMrmlClearList

    MainMrmlInitIdLists
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlSetFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlSetFile {filename} {
    global Mrml File

    # Store the directory of the MRML file as the Mrml(dir)
    set dir [file dirname $filename]
    if {$dir == "" || $dir == "."} {
        set dir [pwd]
    }
    set Mrml(dir) $dir

    # Store the new relative prefix
    set File(filePrefix) [MainFileGetRelativePrefix $filename]

}

#-------------------------------------------------------------------------------
# .PROC MainMrmlRead
#
#
#  Delete the loaded Mrml data from memory.  Replace with a new Mrml file.
#  Append \".xml\" or \".mrml\" an necessary to the file name as necessary.
# .ARGS
# str mrmlFile name of a MRML file to load
# .END
#-------------------------------------------------------------------------------
proc MainMrmlRead {mrmlFile} {
    global Path Mrml Volume

    # Open the file 'mrmlFile' to determine which MRML version it is,
    # and then call the appropriate routine to handle it.

    # Determine name of MRML input file.
    # Append ".mrml" or ".xml" if necessary.
    set fileName $mrmlFile
    if {[file extension $fileName] != ".mrml" && [file extension $fileName] != ".xml"} {
        if {[file exists $fileName.xml] == 1} {
            set fileName $fileName.xml
        } elseif {[file exists $fileName.mrml] == 1} {
            set fileName "$fileName.mrml"
        }
    }

    # Build a MRML Tree for data, and another for colors and LUTs
    if {[info command Mrml(dataTree)] == ""} {
        vtkMrmlTree Mrml(dataTree)
    }
    if {[info command Mrml(colorTree)] == ""} {
        vtkMrmlTree Mrml(colorTree)
    }

    # Check the file exists
    if {$fileName != "" && [CheckFileExists $fileName 0] == "0"} {
        set errmsg "Unable to read input MRML file '$fileName'"
        puts $errmsg
        tk_messageBox -message $errmsg

        # At least read colors
        set tags [MainMrmlAddColors ""]
        MainMrmlBuildTreesVersion2.0 $tags
        return    
    }

    # no file?
    if {$fileName == ""} {
        # At least read colors
        set tags [MainMrmlAddColors ""]
        MainMrmlBuildTreesVersion2.0 $tags
        return    
    }

    MainMrmlDeleteAll

    # Store file and directory name
    MainMrmlSetFile $fileName

    # Colors don't need saving now
    set Mrml(colorsUnsaved) 0

    # Open the file to determine its type
    set version 2
    if {$fileName == ""} {
        set version 1
    } else {
        set fid [open $fileName r]
        gets $fid line
         close $fid
        if {[lindex $line 0] == "MRML"} {
            set version 1
        }
    }
    if {$version == 1} {
        puts "Reading MRML V1.0: $fileName"
        MainMrmlReadVersion1.0 $fileName
        MainMrmlBuildTreesVersion1.0

        # Always add Colors.xml
        set tags [MainMrmlAddColors ""]
        MainMrmlBuildTreesVersion2.0 $tags
    } else {
        puts "Reading MRML V2.0: $fileName"
        set tags [MainMrmlReadVersion2.x $fileName]

        # Only add colors if none exist
        set tags [MainMrmlAddColors $tags]

        MainMrmlBuildTreesVersion2.0 $tags
    }

    # Put the None volume at the end
    set i [lsearch $Volume(idList) $Volume(idNone)]
    set Volume(idList) "[lreplace $Volume(idList) $i $i] $Volume(idNone)"
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlBuildTreesVersion2.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlBuildTreesVersion2.0 {tags} {
    global Mrml 
    eval {global} $Mrml(nodeTypeList)

    foreach pair $tags {
        set tag  [lindex $pair 0]
        set attr [lreplace $pair 0 0]

        switch $tag {
        
        "Transform" {
            set n [MainMrmlAddNode Transform]
        }
        
        "EndTransform" {
            set n [MainMrmlAddNode EndTransform]
        }
        
        "Matrix" {
            set n [MainMrmlAddNode Matrix]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                "desc"   {$n SetDescription $val}
                "name"   {$n SetName        $val}
                "matrix" {$n SetMatrix      $val}
                }
            }
        }

        "Color" {
            set n [MainMrmlAddNode Color]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                "desc"         {$n SetDescription  $val}
                "name"         {$n SetName         $val}
                "ambient"      {$n SetAmbient      $val}
                "diffuse"      {$n SetDiffuse      $val}
                "specular"     {$n SetSpecular     $val}
                "power"        {$n SetPower        $val}
                "labels"       {$n SetLabels       $val}
                "diffusecolor" {eval $n SetDiffuseColor $val}
                }
            }
        }
        
        "Model" {
            set n [MainMrmlAddNode Model]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                "id"               {$n SetModelID      $val}
                "desc"             {$n SetDescription  $val}
                "name"             {$n SetName         $val}
                "filename"         {$n SetFileName     $val}
                "color"            {$n SetColor        $val}
                "opacity"          {$n SetOpacity      $val}
                "scalarrange"      {eval $n SetScalarRange $val}
                "visibility" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetVisibility 1
                    } else {
                        $n SetVisibility 0
                    }
                }
                "clipping" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetClipping 1
                    } else {
                        $n SetClipping 0
                    }
                }
                "backfaceculling" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetBackfaceCulling 1
                    } else {
                        $n SetBackfaceCulling 0
                    }
                }
                "scalarvisibility" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetScalarVisibility 1
                    } else {
                        $n SetScalarVisibility 0
                    }
                }
                }
            }

            # Compute full path name relative to the MRML file
            $n SetFullFileName [file join $Mrml(dir) [$n GetFileName]]
            # Generate model ID if necessary
           if {[$n GetModelID] == ""} {
               $n SetModelID "M[$n GetID]"
           }
        }
        
        "Volume" {
            set n [MainMrmlAddNode Volume]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                "id"              {$n SetVolumeID       $val}
                "desc"            {$n SetDescription    $val}
                "name"            {$n SetName           $val}
                "filepattern"     {$n SetFilePattern    $val}
                "fileprefix"      {$n SetFilePrefix     $val}
                "imagerange"      {eval $n SetImageRange $val}
                "spacing"         {eval $n SetSpacing   $val}
                "dimensions"      {eval $n SetDimensions $val}
                "scalartype"      {$n SetScalarTypeTo$val}
                "numscalars"      {$n SetNumScalars     $val}
                "rastoijkmatrix"  {$n SetRasToIjkMatrix $val}
                "rastovtkmatrix"  {$n SetRasToVtkMatrix $val}
                "positionmatrix"  {$n SetPositionMatrix $val}
                "scanorder"       {$n SetScanOrder $val}
                "colorlut"        {$n SetLUTName        $val}
                "window"          {$n SetWindow         $val}
                "level"           {$n SetLevel          $val}
                "lowerthreshold"  {$n SetLowerThreshold $val}
                "upperthreshold"  {$n SetUpperThreshold $val}
                "autowindowlevel" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetAutoWindowLevel 1
                    } else {
                        $n SetAutoWindowLevel 0
                    }
                }
                "autothreshold" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetAutoThreshold 1
                    } else {
                        $n SetAutoThreshold 0
                    }
                }
                "applythreshold" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetApplyThreshold 1
                    } else {
                        $n SetApplyThreshold 0
                    }
                }
                "interpolate" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetInterpolate 1
                    } else {
                        $n SetInterpolate 0
                    }
                }
                "labelmap" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetLabelMap 1
                    } else {
                        $n SetLabelMap 0
                    }
                }
                "littleendian" {
                    if {$val == "yes" || $val == "true"} {
                        $n SetLittleEndian 1
                    } else {
                        $n SetLittleEndian 0
                    }
                }
                "dicomfilenamelist" {
                    set filelist {}
                    eval {lappend filelist} $val
                    foreach file $filelist {
                        set DICOMName [file join $Mrml(dir) $file]
                        $n AddDICOMFileName $DICOMName
                    }
                }
                "dicommultiframeoffsetlist" {
                    set offsetlist {}
                    eval {lappend offsetlist} $val
                    foreach offset $offsetlist {
                        $n AddDICOMMultiFrameOffset $offset
                    }
                }
                    "frequencyphaseswap" {
                        # added by odonnell for DTI data: will move 
                        # to submodule of Volumes.tcl
                        if {$val == "yes" || $val == "true"} {
                            $n SetFrequencyPhaseSwap 1
                        }
                    }

                }
            }

            # Compute full path name relative to the MRML file
            $n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
            # Set volume ID if necessary
            if {[$n GetVolumeID] == ""} {
                $n SetVolumeID "V[$n GetID]"
            }

            $n UseRasToVtkMatrixOn
        }

        "TetraMesh" {
                    MainTetraMeshProcessMrml "$attr"
                  }

        "Options" {
            # Legacy: options shouldn't be stored in an options tag,
            # use the fancy XML tags instead

            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "options"      {set options $val}
                    "program"      {set program $val}
                    "contents"     {set contents $val}
                }
            }
            # I don't want any of gimpy's stinkin' modules in my tree!
            if {$contents != "modules"} {
                set n [MainMrmlAddNode Options]
                $n SetOptions $options
                $n SetProgram $program
                $n SetContents $contents
            }

            # Check that this is a slicer options node.
            if {[$n GetProgram] != "slicer"} {
                return
            }

            # If these are presets, then do preset stuff on stuffing, not attr
            if {[$n GetContents] == "presets"} {
                # Since presets aren't written to the MRML file when different
                # from their default values, I must first reset them to their defaults.
                MainOptionsUseDefaultPresets
                MainOptionsParsePresets $attr
            }
            }

        "Fiducials" {
            set n [MainMrmlAddNode Fiducials]
        }
        
        "EndFiducials" {
            set n [MainMrmlAddNode EndFiducials]
        }
        "Point" {
            set n [MainMrmlAddNode Point]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                "desc"             {$n SetDescription  $val}
                "name"             {$n SetName         $val}
                "xyz"              {eval $n SetXYZ     $val}
                }

            }
            }
        "Path" {
            set n [MainMrmlAddNode Path]
        }
        
        "EndPath" {
            set n [MainMrmlAddNode EndPath]
        }
        "Landmark" {
            set n [MainMrmlAddNode Landmark]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                    switch [string tolower $key] {
                
                "desc"             {$n SetDescription  $val}
                "name"             {$n SetName         $val}
                "xyz"              {eval $n SetXYZ     $val}
                "focalxyz"              {eval $n SetFXYZ     $val}
                "pathposition"    {$n SetPathPosition  $val}

                }
            }
            }

        "Hierarchy" {
            set n [MainMrmlAddNode Hierarchy]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "id" {$n SetHierarchyID $val}
                    "type" {$n SetType $val}
                }
            }
        }
        "EndHierarchy" {
            set n [MainMrmlAddNode EndHierarchy]
        }
        "ModelGroup" {
            set n [MainMrmlAddNode ModelGroup]
            $n SetExpansion 1; #always show groups expanded
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "id" {$n SetModelGroupID $val}
                    "name" {$n SetName $val}
                    "color" {$n SetColor $val}
                    "opacity" {$n SetOpacity $val}
                    "visibility" {
                        if {$val == "yes" || $val == "true"} {
                            $n SetVisibility 1
                        } else {
                            $n SetVisibility 0
                        }
                    }
                }
            }        
        }
        "EndModelGroup" {
            set n [MainMrmlAddNode EndModelGroup]
        }
        "ModelRef" {
            set n [MainMrmlAddNode ModelRef]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "modelrefid" {$n SetModelRefID $val}
                }
            }
        }
        "Scenes" {
                set n [MainMrmlAddNode Scenes]
                foreach a $attr {
                    set key [lindex $a 0]
                    set val [lreplace $a 0 0]
                    switch [string tolower $key] {
                        "lang" {$n SetLang $val}
                        "name" {$n SetName $val}
                        "description" {$n SetDescription $val}
                    }
                }
        }
        "EndScenes" {
            set n [MainMrmlAddNode EndScenes]
        }
        "VolumeState" {
            set n [MainMrmlAddNode VolumeState]
            foreach a $attr {
                    set key [lindex $a 0]
                    set val [lreplace $a 0 0]
                    switch [string tolower $key] {
                        "volumerefid" {$n SetVolumeRefID $val}
                        "colorlut" {$n SetColorLUT $val}
                        "foreground" {
                            if {$val == "true"} {
                                $n SetForeground 1
                            } else {
                                $n SetForeground 0
                            }
                        }
                        "background" {
                            if {$val == "true"} {
                                $n SetForeground 1
                            } else {
                                $n SetForeground 0
                            }
                        }
                        "fade" {
                            if {$val == "true"} {
                                $n SetFade 1
                            } else {
                                $n SetFade 0
                            }
                        }
                        "opacity" {
                        $n SetOpacity $val
                    }
                    }
                }
        }
        "EndVolumeState" {
            set n [MainMrmlAddNode EndVolumeState]
        }
        "CrossSection" {
            set n [MainMrmlAddNode CrossSection]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "position" {$n SetPosition $val}
                    "direction" {$n SetDirection $val}
                    "sliceslider" {$n SetSliceSlider $val}
                    "rotatorx" {$n SetRotatorX $val}
                    "rotatory" {$n SetRotatorY $val}
                    "inmodel" {
                        if {$val == "true"} {
                            $n SetInModel 1
                        } else {
                            $n SetInModel 0
                        }
                    }
                    "clipstate" {
                        if {$val == "true"} {
                            $n SetClipState 1
                        } else {
                            $n SetClipState 0
                        }
                    }
                    "zoom" {$n SetZoom $val}
                    "backvolrefid" {$n SetBackVolRefID $val}
                    "forevolrefid" {$n SetForeVolRefID $val}
                    "labelvolrefid" {$n SetLabelVolRefID $val}
                }
            }
        }
        "SceneOptions" {
            set n [MainMrmlAddNode SceneOptions]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "viewup" {$n SetViewUp $val}
                    "position" {$n SetPosition $val}
                    "focalpoint" {$n SetFocalPoint $val}
                    "clippingrange" {$n SetClippingRange $val}
                    "viewmode" {$n SetViewMode $val}
                    "viewbgcolor" {$n SetViewBgColor $val}
                    "showaxes" {
                        if {$val == "true"} {
                            $n SetShowAxes 1
                        } else {
                            $n SetShowAxes 0
                        }
                    }
                    "showbox" {
                        if {$val == "true"} {
                            $n SetShowBox 1
                        } else {
                            $n SetShowBox 0
                        }
                    }
                    "showannotations" {
                        if {$val == "true"} {
                            $n SetShowAnnotations 1
                        } else {
                            $n SetShowAnnotations 0
                        }
                    }
                    "showslicebounds" {
                        if {$val == "true"} {
                            $n SetShowSliceBounds 1
                        } else {
                            $n SetShowSliceBounds 0
                        }
                    }
                    "showletters" {
                        if {$val == "true"} {
                            $n SetShowLetters 1
                        } else {
                            $n SetShowLetters 0
                        }
                    }
                    "showcross" {
                        if {$val == "true"} {
                            $n SetShowCross 1
                        } else {
                            $n SetShowCross 0
                        }
                    }
                    "showhashes" {
                        if {$val == "true"} {
                            $n SetShowHashes 1
                        } else {
                            $n SetShowHashes 0
                        }
                    }
                    "showmouse" {
                        if {$val == "true"} {
                            $n SetShowMouse 1
                        } else {
                            $n SetShowMouse 0
                        }
                    }
                    "dicomstartdir" {$n SetDICOMStartDir $val}
                    "filenamesortparam" {$n SetFileNameSortParam $val}
                    "dicomdatadictfile" {$n SetDICOMDataDictFile $val}
                    "dicompreviewwidth" {$n SetDICOMPreviewWidth $val}
                    "dicompreviewheight" {$n SetDICOMPreviewHeight $val}
                    "dicompreviewhighestvalue" {$n SetDICOMPreviewHighestValue $val}
                }
            }
        }
        "ModelState" {
            set n [MainMrmlAddNode ModelState]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "modelrefid" {$n SetModelRefID $val}
                    "opacity" {$n SetOpacity $val}
                    "visible" {
                        if {$val == "true"} {
                            $n SetVisible 1
                        } else {
                            $n SetVisible 0
                        }
                    }
                    "slidervisible" {
                        if {$val == "true"} {
                            $n SetSliderVisible 1
                        } else {
                            $n SetSliderVisible 0
                        }
                    }
                    "sonsvisible" {
                        if {$val == "true"} {
                            $n SetSonsVisible 1
                        } else {
                            $n SetSonsVisible 0
                        }
                    }
                }
            }
        }
        "WindowLevel" {
            set n [MainMrmlAddNode WindowLevel]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "window" {$n SetWindow $val}
                    "level" {$n SetLevel $val}
                    "lowerthreshold" {$n SetLowerThreshold $val}
                    "upperthreshold" {$n SetUpperThreshold $val}
                    "autowindowlevel" {
                        if {$val == "true"} {
                            $n SetAutoWindowLevel 1
                        } else {
                            $n SetAutoWindowLevel 0
                        }
                    }
                    "applythreshold" {
                        if {$val == "true"} {
                            $n SetApplyThreshold 1
                        } else {
                            $n SetApplyThreshold 0
                        }
                    }
                    "autothreshold" {
                        if {$val == "true"} {
                            $n SetAutoThreshold 1
                        } else {
                            $n SetAutoThreshold 0
                        }
                    }
                }
            }
        }
        "Locator" {
            set n [MainMrmlAddNode Locator]
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "driver" {$n SetDriver $val}
                    "diffusecolor" {$n SetDiffuseColor $val}
                    "visibility" {
                        if {$val == "true"} {
                            $n SetVisibility 1
                        } else {
                            $n SetVisibility 0
                        }
                    }
                    "transversevisibility" {
                        if {$val == "true"} {
                            $n SetTransverseVisibility 1
                        } else {
                            $n SetTransverseVisibility 0
                        }
                    }
                    "normallen" {$n SetNormalLen $val}
                    "transverselen" {$n SetTransverseLen $val}
                    "radius" {$n SetRadius $val}
                }
            }
        }
        }
        }
    }

#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion1.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadVersion1.0 {fileName} {
    global Lut Dag Volume Model Config Color Gui Mrml env Transform
    global Fiducials EndFiducials Point
        global Path EndPath Landmark

    # Read file
    if {$fileName == ""} {
        set Dag(read) [MRMLCreateDag]
    } else {
        set Dag(read) [MRMLRead $fileName]
        if {$Dag(read) == "-1"} {
            tk_messageBox -message "Error reading MRML file: '$fileName'\n\
                See message written in console." 
            return
        }
    }

    # Expand URLs
    set Dag(expanded) [MRMLExpandUrls $Dag(read) $Mrml(dir)]
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlBuildTreesVersion1.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlBuildTreesVersion1.0 {} {
    global Dag Color Model Volume Transform EndTransform Matrix Path
    global Fiducials EndFiducials Point
    global Path EndPath Landmark

    set level 0
    set transformCount($level) 0
    set dag $Dag(expanded)
    set num [MRMLGetNumNodes $dag]

    for {set j 0} {$j < $num} {incr j} {
        set node [MRMLGetNode $dag $j]
        set type [MRMLGetNodeType $node]
        switch $type {
        
        "Separator" {
            # Increment the separator level.
            # Initialize the counter of transforms inside this separator.
            # Add a Transform node.
            
            incr level
            set transformCount($level) 0

            set n [MainMrmlAddNode Transform]
        }
        
        "End" {
            # For each transform inside this separator, add an EndTransform node.
            # Also add an EndTransform node for this separator itself.

            for {set c 0} {$c < $transformCount($level)} {incr c} {
                set n [MainMrmlAddNode EndTransform]
            }
            set level [expr $level - 1]

            set n [MainMrmlAddNode EndTransform]
        }
        
        "Transform" {
            # Increment the count of transforms inside the current separator.
            # Add a Transform node and a Matrix node.
            
            incr transformCount($level)

            set n [MainMrmlAddNode Transform]

            set n [MainMrmlAddNode Matrix]
            $n SetDescription  [MRMLGetValue $node desc]
            $n SetName         [MRMLGetValue $node name]
            $n SetMatrix       [MRMLGetValue $node matrix]
            eval $n Scale      [MRMLGetValue $node scale]
            $n RotateX         [MRMLGetValue $node rotateX]
            $n RotateY         [MRMLGetValue $node rotateY]
            $n RotateZ         [MRMLGetValue $node rotateZ]
            eval $n Translate  [MRMLGetValue $node translate]
        }

        "Color" {
            set n [MainMrmlAddNode Color]
            $n SetDescription  ""
            $n SetName         [MRMLGetValue $node name]
            $n SetAmbient      [MRMLGetValue $node ambient]
            $n SetDiffuse      [MRMLGetValue $node diffuse]
            $n SetSpecular     [MRMLGetValue $node specular]
            $n SetPower        [MRMLGetValue $node power]
            $n SetLabels       [MRMLGetValue $node labels]
            eval $n SetDiffuseColor [MRMLGetValue $node diffuseColor]
        }
        
        "Model" {
            set n [MainMrmlAddNode Model]
            $n SetDescription      [MRMLGetValue $node desc]
            $n SetName             [MRMLGetValue $node name]
            $n SetFileName         [MRMLGetValue $node fileName]
            $n SetFullFileName     [MRMLGetValue $node fileName]
            $n SetColor            [MRMLGetValue $node colorName]
            $n SetOpacity          [MRMLGetValue $node opacity]
            $n SetVisibility       [MRMLGetValue $node visibility]
            $n SetClipping         [MRMLGetValue $node clipping]
            $n SetBackfaceCulling  [MRMLGetValue $node backfaceCulling]
            $n SetScalarVisibility [MRMLGetValue $node scalarVisibility]
            eval $n SetScalarRange [MRMLGetValue $node scalarRange]
        }
        
        "Volume" {
            set n [MainMrmlAddNode Volume]
            $n SetDescription      [MRMLGetValue $node desc]
            $n SetName             [MRMLGetValue $node name]
            eval $n SetImageRange  [MRMLGetValue $node imageRange]
            eval $n SetDimensions  [MRMLGetValue $node dimensions]
            eval $n SetSpacing     [MRMLGetValue $node spacing]
            $n SetScalarTypeTo[MRMLGetValue $node scalarType]
            $n SetNumScalars       [MRMLGetValue $node numScalars]
            $n SetLittleEndian     [MRMLGetValue $node littleEndian]
            $n SetTilt             [MRMLGetValue $node tilt]
            $n SetRasToIjkMatrix   [MRMLGetValue $node rasToIjkMatrix]
            $n SetRasToVtkMatrix   [MRMLGetValue $node rasToVtkMatrix]
            $n UseRasToVtkMatrixOn
            $n SetFilePattern      [MRMLGetValue $node filePattern]
            $n SetFilePrefix       [MRMLGetValue $node filePrefix]
            $n SetFullPrefix       [MRMLGetValue $node filePrefix]
            $n SetWindow           [MRMLGetValue $node window]
            $n SetLevel            [MRMLGetValue $node level]
            $n SetAutoWindowLevel  [MRMLGetValue $node autoWindowLevel]
            puts "auto=[$n GetAutoWindowLevel]"
            $n SetLabelMap         [MRMLGetValue $node labelMap]
            $n SetScanOrder        [MRMLGetValue $node scanOrder]

            # Don't interpolate label maps
            if {[MRMLGetValue $node labelMap] == 1} {
                $n SetInterpolate 0
            }
        }
        }
    }

    # Add EndTransforms for each transform at the base level
    # (outside all separators)
    for {set c 0} {$c < $transformCount($level)} {incr c} {
        set n [MainMrmlAddNode EndTransform]
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlAddColors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlAddColors {tags} {

    # If there are no Color nodes, then read, and append default colors.
    # Return a new list of tags, possibly including default colors.

    set colors 0
    foreach pair $tags {
        set tag  [lindex $pair 0]
        if {$tag == "Color"} {
            set colors 1
        }
    }
    if {$colors == 1} {return $tags}
    
    set fileName [ExpandPath Colors.xml]
    set tagsColors [MainMrmlReadVersion2.x $fileName]
    if {$tagsColors == 0} {
        set msg "Unable to read file default MRML color file '$fileName'"
        puts $msg
        tk_messageBox -message $msg
        return $tags
    }

    return "$tags $tagsColors"
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlCheckColors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlCheckColors {} {
    global Mrml
    
    set fileName [ExpandPath Colors.xml]
    set tags [MainMrmlReadVersion2.x $fileName]

    if {$tags != 0} {
        vtkMrmlColorNode default
        set n default

        Mrml(colorTree) InitTraversal
        set node [Mrml(colorTree) GetNextItem]

        foreach pair $tags {
            set tag  [lindex $pair 0]
            set attr [lreplace $pair 0 0]

            # Are we out of nodes?
            if {$node == ""} {
                set Mrml(colorsUnsaved) 1
            } else {
                if {$tag == "Color"} {
                    foreach a $attr {
                        set key [lindex $a 0]
                        set val [lreplace $a 0 0]
                        switch $key {
                        "desc"         {$n SetDescription  $val}
                        "name"         {$n SetName         $val}
                        "ambient"      {$n SetAmbient      $val}
                        "diffuse"      {$n SetDiffuse      $val}
                        "specular"     {$n SetSpecular     $val}
                        "power"        {$n SetPower        $val}
                        "labels"       {$n SetLabels       $val}
                        "diffuseColor" {eval $n SetDiffuseColor $val}
                        }
                    }
                    if {[$node GetDescription] != [$n GetDescription]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetName] != [$n GetName]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetAmbient] != [$n GetAmbient]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetDiffuse] != [$n GetDiffuse]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetSpecular] != [$n GetSpecular]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetPower] != [$n GetPower]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetLabels] != [$n GetLabels]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    if {[$node GetDiffuseColor] != [$n GetDiffuseColor]} {
                        set Mrml(colorsUnsaved) 1
                    }
                    set node [Mrml(colorTree) GetNextItem]
                }
            }
        }
        default Delete

        # Out of tags
        if {$node != ""} {
            set Mrml(colorsUnsaved) 1
        }
    }

}

#-------------------------------------------------------------------------------
# .PROC MainMrmlRelativity
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlRelativity {oldRoot} {
    global Mrml

    Mrml(dataTree) InitTraversal
    set node [Mrml(dataTree) GetNextItem]
    while {$node != ""} {
        set class [$node GetClassName]

        if {$class == "vtkMrmlVolumeNode"} {

            $node SetFilePrefix [MainFileGetRelativePrefix \
                [file join $oldRoot [$node GetFilePrefix]]]
            $node SetFullPrefix [file join $Mrml(dir) \
                [file join $oldRoot [$node GetFilePrefix]]]
            
            # >> AT 7/6/01

            set num [$node GetNumberOfDICOMFiles]
            for {set i 0} {$i < $num} {incr i} {
                set filename [$node GetDICOMFileName $i]
                set dir [file dirname $filename]
                set name [file tail $filename]
                set reldir [MainFileGetRelativePrefix $dir]
                $node SetDICOMFileName $i [file join $reldir $name]
            }

            # << AT 7/6/01

        } elseif {$class == "vtkMrmlModelNode"} {

            set ext [file extension [$node GetFileName]]
            $node SetFileName [MainFileGetRelativePrefix \
                [file join $oldRoot [$node GetFileName]]]$ext
            $node SetFullFileName [file join $Mrml(dir) \
                [file join $oldRoot [$node GetFileName]]]$ext

        }
        set node [Mrml(dataTree) GetNextItem]
    }
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlWrite {filename} {
    global Mrml

    # Store the new root and filePrefix
    set oldRoot $Mrml(dir)
    MainMrmlSetFile $filename

    # Rename all file with relative path names
    MainMrmlRelativity $oldRoot

    # See if colors are different than the defaults
    MainMrmlCheckColors

    # If colors have changed since last save, then save colors too
    if {$Mrml(colorsUnsaved) == 1} {
        puts SaveColors

        # Combine trees
        vtkMrmlTree tree

        # Data tree
        Mrml(dataTree) InitTraversal
        set node [Mrml(dataTree) GetNextItem]
        while {$node != ""} {
            tree AddItem $node
            set node [Mrml(dataTree) GetNextItem]
        }

        # Color tree
        Mrml(colorTree) InitTraversal
        set node [Mrml(colorTree) GetNextItem]
        while {$node != ""} {
            tree AddItem $node
            set node [Mrml(colorTree) GetNextItem]
        }

        tree Write $filename
        tree RemoveAllItems
        tree Delete
    } else {
        Mrml(dataTree) Write $filename
    }
    # Colors don't need saving now
    set Mrml(colorsUnsaved) 0
}
