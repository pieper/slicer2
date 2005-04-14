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
# FILE:        VolGeneric.tcl
# PROCEDURES:  
#   VolGenericBuildGUI
#   VolGenericEnter
#   VolGenericExit
#   VolGenericSetFileName
#   VolGenericApply
#   VolGenericMainFileCloseUpdate
#==========================================================================auto=

proc VolGenericInit {} {
    global Gui VolGeneric Volume Slice Module

    if {$Module(verbose) == 1} {puts "VolGenericInit starting"}
   
    set e VolGeneric

    set Volume(readerModules,$e,name) "Generic Readers"
    set Volume(readerModules,$e,tooltip) "This tab lets you read in Generic volumes using the ITK I/O Factory methods"

    set Volume(readerModules,$e,procGUI) ${e}BuildGUI
    set Volume(readerModules,$e,procEnter) ${e}Enter
    set Volume(readerModules,$e,procExit) ${e}Exit

    # for closing out a scene
    set Volume(VolGeneric,idList) ""
    set Module($e,procMainFileCloseUpdateEntered) VolGenericMainFileCloseUpdate

    # register the procedures in this file that will read in volumes
    set Module(Volumes,readerProc,Generic) VolGenericReaderProc
}


#-------------------------------------------------------------------------------
# .PROC VolGenericBuildGUI
# Builds the GUI for the free surfer readers, as a submodule of the Volumes module
# .ARGS
# parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc VolGenericBuildGUI {parentFrame} {
    global Gui Volume Module

    if {$Module(verbose) == 1} {
        puts  "VolGenericBuildGUI"
    }

    set f $parentFrame

    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fVolume $f.fApply \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # fVolume frame
    #-------------------------------------------

    set f $parentFrame.fVolume

    DevAddFileBrowse $f Volume firstFile "First Image File:" "VolumesSetFirst" "" "\$Volume(DefaultDir)" "Open" "Browse for the first Image file" "Browse to the first file in the volume" "Absolute"

    frame $f.fLabelMap -bg $Gui(activeWorkspace)

    frame $f.fDesc     -bg $Gui(activeWorkspace)

    frame $f.fName -bg $Gui(activeWorkspace)

    frame $f.fscalarType -bg $Gui(activeWorkspace)
    

    pack $f.fLabelMap -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fscalarType -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    # Name row
    set f $parentFrame.fVolume.fName
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable Volume(name) -width 13} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad) 

    # Desc row
    set f $parentFrame.fVolume.fDesc

    eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
    eval {entry $f.eDesc -textvariable Volume(desc)} $Gui(WEA)
    pack $f.lDesc -side left -padx $Gui(pad)
    pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x

     # Scalar Type Menu
    set f $parentFrame.fVolume.fscalarType
    eval {label $f.lscalarType -text "Scalar Type:"} $Gui(WLA)
    eval {menubutton $f.mbscalarType -relief raised -bd 2 \
        -text $Volume(scalarType)\
        -width 10 -menu $f.mbscalarType.menu} $Gui(WMBA)
    set Volume(VolGeneric,mbscalarType) $f.mbscalarType
    eval {menu $f.mbscalarType.menu} $Gui(WMA)
    
    set m $f.mbscalarType.menu
    foreach type $Volume(scalarTypeMenu) {
        $m add command -label $type -command "VolGenericSetScalarType $type"
    }
    pack $f.lscalarType -side left -padx $Gui(pad) -fill x -anchor w
    pack $f.mbscalarType -side left -padx $Gui(pad) -expand 1 -fill x 
    

    # LabelMap
    set f $parentFrame.fVolume.fLabelMap

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -pady 5

    DevAddLabel $f.fTitle.l "Image Data:"
    pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

    foreach text "{Grayscale} {Label Map}" \
        value "0 1" \
        width "9 9 " {
        eval {radiobutton $f.fBtns.rMode$value -width $width \
            -text "$text" -value "$value" -variable Volume(labelMap) \
            -indicatoron 0 } $Gui(WCA)
        pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
    }


    if {$Module(verbose) == 1} {
        puts "Done packing the label map stuff"
    }

    #-------------------------------------------
    # Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply
        
    DevAddButton $f.bApply "Apply" "set Volume(fileType) Generic; VolGenericApply; VolumesSetPropertyType VolHeader" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC VolGenericEnter
# Does nothing yet
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericEnter {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolGeneric ENTER"}
}


#-------------------------------------------------------------------------------
# .PROC VolGenericExit
# Does nothing yet.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericExit {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolGeneric EXIT"}
}


#-------------------------------------------------------------------------------
# .PROC VolGenericSetFileName
# The filename is set elsehwere, in variable Volume(VolGeneric,FileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericSetFileName {} {
    global Volume Module

    if {$Module(verbose) == 1} {
        puts "Generic filename: $Volume(VolGeneric,FileName)"
    }
    set Volume(name) [file tail $Volume(VolGeneric,FileName)]
    # replace . with -
    regsub -all {[.]} $Volume(name) {-} Volume(name)
}

#-------------------------------------------------------------------------------
# .PROC VolGenericApply
# Check the file type variable and build the appropriate model or volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericApply {} {
    global View Module Volume

    set Volume(VolGeneric,FileName) $Volume(firstFile)

    if {$Module(verbose) == 1} {
        puts "proc VolGeneric"
    }

    if { $Volume(VolGeneric,FileName) == "" } {
        return
    }

    if { ![info exists Volume(name)] } { set Volume(name) "Generic"}

    if {[ValidateName $Volume(name)] == 0} {
        DevErrorWindow "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    # NOTE:
    # this normally happens in MainVolumes.tcl
    # this is needed here to set up reading
    # this should be fixed - the node should handle this somehow
    # so that MRML can be read in with just a node and this will
    # work
    # puts "VolGenericApply: NOT calling MainVolumes create on $i"
    # MainVolumesCreate $i
  
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }

    vtkITKArchetypeImageSeriesReader genreader
    genreader SetArchetype $Volume(VolGeneric,FileName)
    genreader SetOutputScalarTypeTo$Volume(scalarType)
   
    # flip the image data going from ITK to VTK
    vtkImageFlip Volume($i,vol,rw)
    Volume($i,vol,rw) SetFilteredAxis 1
    Volume($i,vol,rw) SetInput  [genreader GetOutput]   

    if {$Module(verbose) == 1} {
        puts "VolGenericApply: set Generic filename to [Volume($i,vol,rw) GetFileName]\nCalling Update on volume $i"
    }

    set imdata [Volume($i,vol,rw) GetOutput]
    $imdata UpdateInformation

    set Volume(isDICOM) 0
    set Volume($i,type) "Generic"

    set extents [$imdata GetWholeExtent]
    set dims "[expr [lindex $extents 1] - [lindex $extents 0] + 1] \
              [expr [lindex $extents 3] - [lindex $extents 2] + 1] \
              [expr [lindex $extents 5] - [lindex $extents 4] + 1]"

    set Volume(lastNum) [expr [lindex $dims 2] - 1]
    set Volume(width) [expr [lindex $dims 0] - 1]
    set Volume(height) [expr [lindex $dims 1] - 1]

    set spc [$imdata GetSpacing]
    set Volume(pixelWidth) [lindex $spc 0]
    set Volume(pixelHeight) [lindex $spc 1]
    set Volume(sliceThickness) [lindex $spc 2]

    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    set scalarType [$imdata GetScalarTypeAsString]
    set Volume(scalarType) [VolumesVtkToSlicerScalarType $scalarType]
    set Volume(readHeaders) 0
    set Volume(filePattern) %s
    set Volume(dimensions) "[lindex $dims 0] [lindex $dims 1]"
    set Volume(imageRange) "1 [lindex $dims 2]"

    set Volume(scanOrder) IS
    set Volume(littleEndian) 1
    set Volume(sliceSpacing) 0 

    set fileType Generic

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
    # set the volume properties: read the header first: sets the Volume array values we need
    Volume($i,node) SetName $Volume(name)
    Volume($i,node) SetDescription $Volume(desc)
    Volume($i,node) SetLabelMap $Volume(labelMap)

    if { ![string is double $Volume(sliceThickness)] } {
        DevWarningWindow "Bad slice thickess/spacing from Generic Reader - assuming 1.0"
        set Volume(pixelWidth) 1.0
        set Volume(pixelHeight) 1.0
        set Volume(sliceThickness) 1.0
    }

    eval Volume($i,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($i,node) SetTilt $Volume(gantryDetectorTilt)

    Volume($i,node) SetFilePattern $Volume(filePattern) 
    Volume($i,node) SetScanOrder $Volume(scanOrder)
    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    Volume($i,node) SetFileType $fileType

    Volume($i,node) SetFilePrefix [genreader GetArchetype] ;# NB: just one file, not a pattern
    Volume($i,node) SetFullPrefix [genreader GetArchetype] ;# needed in the range check
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarType [$imdata GetScalarType]
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "VolGeneric: setting full prefix for volume node $i"
    }

    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "VolGeneric: failed to read in the volume $i, $Volume(VolGeneric,FileName)"
        DevErrorWindow "VolGeneric: failed to read in the volume $i, $Volume(VolGeneric,FileName)"
        return
    }

    if {$Module(verbose) == 1} {
        puts "VolGeneric: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "VolGeneric: volume $i position 1 3: $badval"
    }

    # allow use of other module GUIs
    set Volume(freeze) 0

    # set active volume on all menus
    MainVolumesSetActive $i

    # if we are successful set the FOV for correct display of this volume
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]
    set fov     [expr $dim*$spacing]
    set View(fov) $fov

    MainViewSetFov


    # display the new volume in the background of all slices if not a label map
    if {[Volume($i,node) GetLabelMap] == 1} {
        MainSlicesSetVolumeAll Label $i
    } else {
        MainSlicesSetVolumeAll Back $i
    }


    return $i
}

#-------------------------------------------------------------------------------
# .PROC VolGenericMainFileCloseUpdate
# Called to clean up anything created in this sub module. Deletes Volumes read in, along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericMainFileCloseUpdate {} {
    global Volume viewRen Module
    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "VolGenericMainFileCloseUpdate"
    }
    foreach f $Volume(idList) {
        puts "VolGenericMainFileCloseUpdate: Checking volume $f"
        
        if {[info exists Volume(VolGeneric,$f,curveactor)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing surface actor for free surfer reader id $f"
            }
            viewRen RemoveActor  Volume(VolGeneric,$f,curveactor)
            Volume(VolGeneric,$f,curveactor) Delete
            Volume(VolGeneric,$f,mapper) Delete
            Volume($f,vol,rw) Delete
        }
        if {[info exists Volume($f,vol,rw)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing volume reader for volume id $f"
            }
            Volume($f,vol,rw) Delete
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC VolGenericReaderProc
# Called by MainVolumes.tcl MainVolumesRead to read in an MGH volume, returns -1
# if there is no vtkMGHReader. Assumes that the volume has been read already
# .ARGS v volume ID
# .END
#-------------------------------------------------------------------------------
proc VolGenericReaderProc {v} {
    global Volume

    if { ! [ file exists [Volume($v,node) GetFullPrefix] ] } {
        DevErrorWindow "Generic volume does not exist: [Volume($v,node) GetFullPrefix]"
        return -1
    }

    if { [info commands vtkITKArchetypeImageSeriesReader] == "" } {
        DevErrorWindow "No Generic Reader available."
        return -1
    }
    catch "genreader Delete"
    vtkITKArchetypeImageSeriesReader genreader
    genreader SetArchetype [Volume($v,node) GetFullPrefix]
    genreader SetOutputScalarType [Volume($v,node) GetScalarType]

    catch "flip Delete"
    vtkImageFlip flip
    flip SetFilteredAxis 1
    flip SetInput  [genreader GetOutput]   

    flip Update
    Volume($v,vol) SetImageData [flip GetOutput]

    flip Delete
    genreader Delete

    set Volume(fileType) ""
}

#-------------------------------------------------------------------------------
# .PROC VolGenericSetScalarType
# Set scalar type and config menubutton to match.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolGenericSetScalarType {type} {
    global Volume

    set Volume(scalarType) $type

    # update the button text
    $Volume(VolGeneric,mbscalarType) config -text $type
}
