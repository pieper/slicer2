#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        VolNrrd.tcl
# PROCEDURES:  
#   VolNrrdInit
#   VolNrrdBuildGUI parentFrame
#   VolNrrdEnter
#   VolNrrdExit
#   VolNrrdSetFileName
#   VolNrrdApply
#   VolNrrdReaderProc v
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC VolNrrdInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolNrrdInit {} {
    global Gui VolNrrd Volume Slice Module

    if {$Module(verbose) == 1} {puts "VolNrrdInit starting"}
   
    set e VolNrrd

    set Volume(readerModules,$e,name) "Nrrd Reader"
    set Volume(readerModules,$e,tooltip) "This tab lets you read in Nrrd volumes using the ITK I/O Factory methods"

    set Volume(readerModules,$e,procGUI) ${e}BuildGUI
    set Volume(readerModules,$e,procEnter) ${e}Enter
    set Volume(readerModules,$e,procExit) ${e}Exit

    # for closing out a scene
    set Volume(VolNrrd,idList) ""
    set Volume(imageCentered) 1
    # register the procedures in this file that will read in volumes
    set Module(Volumes,readerProc,Nrrd) VolNrrdReaderProc
}


#-------------------------------------------------------------------------------
# .PROC VolNrrdBuildGUI
# Builds the GUI for the free surfer readers, as a submodule of the Volumes module
# .ARGS
# windowpath parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc VolNrrdBuildGUI {parentFrame} {
    global Gui Volume Module

    if {$Module(verbose) == 1} {
        puts  "VolNrrdBuildGUI"
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

    DevAddFileBrowse $f Volume "VolNrrd,FileName" "Nrrd File:" "VolNrrdSetFileName" "nhdr nrrd" "\$Volume(DefaultDir)" "Open" "Browse for a Nrrd file (.nhdr that has matching .img)" "Absolute"

    frame $f.fLabelMap -bg $Gui(activeWorkspace)

    frame $f.fImageCenter -bg $Gui(activeWorkspace)

    frame $f.fDesc     -bg $Gui(activeWorkspace)

    frame $f.fName -bg $Gui(activeWorkspace)

    frame $f.fComp -bg $Gui(activeWorkspace)

    pack $f.fLabelMap -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fImageCenter -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fComp -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

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

    # Image Origin
    set f $parentFrame.fVolume.fImageCenter

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -pady 5

    DevAddLabel $f.fTitle.l "Image Origin:"
    pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

    foreach text "{Centered} {From File}" \
        value "1 0" \
        width "9 9 " {
        eval {radiobutton $f.fBtns.rMode$value -width $width \
            -text "$text" -value "$value" -variable Volume(imageCentered) \
            -indicatoron 0 } $Gui(WCA)
        pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
    }

    #-------------------------------------------
    # Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply
        
    DevAddButton $f.bApply "Apply" "set Volume(fileType) Nrrd; VolNrrdApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC VolNrrdEnter
# Does nothing yet
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolNrrdEnter {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolNrrd ENTER"}
}


#-------------------------------------------------------------------------------
# .PROC VolNrrdExit
# Does nothing yet.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolNrrdExit {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolNrrd EXIT"}
}


#-------------------------------------------------------------------------------
# .PROC VolNrrdSetFileName
# The filename is set elsehwere, in variable Volume(VolNrrd,FileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolNrrdSetFileName {} {
    global Volume Module

    if {$Module(verbose) == 1} {
        puts "VolNrrdSetFileName filename: $Volume(VolNrrd,FileName)"
    }
    set Volume(name) [file tail $Volume(VolNrrd,FileName)]
    # replace . with -
    regsub -all {[.]} $Volume(name) {-} Volume(name)
}

#-------------------------------------------------------------------------------
# .PROC VolNrrdApply
# Check the file type variable and build the appropriate model or volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolNrrdApply {} {
    global View Module Volume

    if {$Module(verbose) == 1} {
        puts "proc VolNrrd: Enter"
        puts "proc VolNrrd: filename = $Volume(VolNrrd,FileName) "
    }

    if { $Volume(VolNrrd,FileName) == "" } {
        return
    }

    if { ![info exists Volume(name)] } { set Volume(name) "Nrrd"}

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
    # puts "VolNrrdApply: NOT calling MainVolumes create on $i"
    # MainVolumesCreate $i
  
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }

    catch "nrrdReader Delete"
    vtkNRRDReader nrrdReader

    if {![nrrdReader CanReadFile $Volume(VolNrrd,FileName)]} {
        DevErrorWindow "Cannot read file $Volume(VolNrrd,FileName)"
        MainMrmlDeleteNode Volume $i
        return
    }
    nrrdReader SetFileName $Volume(VolNrrd,FileName)

    if {$Volume(imageCentered)} {
        nrrdReader SetUseNativeOriginOff
    } else {
        nrrdReader SetUseNativeOriginOn
    }
   
    if {$Module(verbose) == 1} {
        puts "proc VolNrrd: filename = $Volume(VolNrrd,FileName) "
    }
    # flip the image data going from ITK to VTK
    catch "Volume($i,vol,rw)  Delete"
    #vtkImageFlip Volume($i,vol,rw)
    #Volume($i,vol,rw) SetFilteredAxis 1
    #Volume($i,vol,rw) SetInput  [nrrdReader GetOutput]   

    vtkNRRDReader Volume($i,vol,rw) 
    #set imdata [Volume($i,vol,rw) GetOutput]
    set imdata [nrrdReader GetOutput]

    if {[catch "$imdata UpdateInformation"]} {
        DevErrorWindow "Cannot read file $Volume(VolNrrd,FileName)"
        MainMrmlDeleteNode Volume $i
        return;
    }

    nrrdReader UpdateInformation
    $imdata UpdateInformation

    if {$Module(verbose) == 1} {
        puts "proc VolNrrd: UpdateInformation done"
    }
    set Volume(isDICOM) 0
    set Volume($i,type) "Nrrd"

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

    if {[nrrdReader GetDataByteOrderAsString] == "LittleEndian"} {
        set Volume(littleEndian) 1
    } else {
        set Volume(littleEndian)
    }

    set Volume(sliceSpacing) 0 

    set fileType Nrrd

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
    # set the volume properties: read the header first: sets the Volume array values we need
    Volume($i,node) SetName $Volume(name)
    Volume($i,node) SetDescription $Volume(desc)
    Volume($i,node) SetLabelMap $Volume(labelMap)

    eval Volume($i,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($i,node) SetTilt $Volume(gantryDetectorTilt)

    Volume($i,node) SetFilePattern $Volume(filePattern)

    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    Volume($i,node) SetFileType $fileType

    Volume($i,node) SetFilePrefix [nrrdReader GetFileName] ;# just one file, not a pattern
    Volume($i,node) SetFullPrefix [nrrdReader GetFileName] ;# needed in the range check
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarType [$imdata GetScalarType]
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    set Volume(scanOrder)  [Volume($i,node) ComputeScanOrderFromRasToIjk [nrrdReader GetRasToIjkMatrix]]
    puts "Scan order: $Volume(scanOrder)"
    Volume($i,node) SetScanOrder $Volume(scanOrder)

    VolumesComputeNodeMatricesFromRasToIjkMatrix Volume($i,node) [nrrdReader GetRasToIjkMatrix] $dims

    #
    # Filling headerKeys in the volume array. This key might eventually belong to the MrmlNode
    #
    puts "Header Keys = [nrrdReader GetHeaderKeys]"
    foreach key [nrrdReader GetHeaderKeys] {
        set Volume($i,headerKeys,$key) [nrrdReader GetHeaderValue $key]
    }
    
    #
    # Setting measurement frame as a key value. This might eventually be part of the MrmlNode
    set mframe ""
    foreach r "0 1 2" {
      set axis ""
      foreach c "0 1 2" {
        lappend axis [[nrrdReader GetMeasurementFrameMatrix] GetElement $r $c]
      }
      lappend mframe $axis
    }
    puts "Measurement frame: $mframe"
    
    set Volume($i,headerKeys,measurementframe) $mframe    
    
    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "VolNrrd: setting full prefix for volume node $i"
    }


    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "VolNrrd: failed to read in the volume $i, $Volume(VolNrrd,FileName)"
        DevErrorWindow "VolNrrd: failed to read in the volume $i, $Volume(VolNrrd,FileName)"
        return
    }

    if {$Module(verbose) == 1} {
        puts "VolNrrd: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "VolNrrd: volume $i position 1 3: $badval"
    }

    # allow use of other module GUIs
    set Volume(freeze) 0

    # Unfreeze
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }

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

    nrrdReader Delete

    return $i
}

#-------------------------------------------------------------------------------
# .PROC VolNrrdReaderProc
# Called by MainVolumes.tcl MainVolumesRead to read in an MGH volume, returns -1
# if there is no vtkMGHReader. Assumes that the volume has been read already
# .ARGS 
# int v volume ID
# .END
#-------------------------------------------------------------------------------
proc VolNrrdReaderProc {v} {
    global Volume

    if { ! [ file exists [Volume($v,node) GetFullPrefix] ] } {
        DevErrorWindow "Nrrd volume does not exist: [Volume($v,node) GetFullPrefix]"
        return -1
    }

    if { [info commands vtkNRRDReader] == "" } {
        DevErrorWindow "No Nrrd Reader available."
        return -1
    }
    catch "nrrdReader1 Delete"
    vtkNRRDReader nrrdReader1

    if {![nrrdReader1 CanReadFile [Volume($v,node) GetFullPrefix]]} {
        DevErrorWindow "Cannot read file [Volume($v,node) GetFullPrefix]"
        return
    }

  
    nrrdReader1 SetFileName [Volume($v,node) GetFullPrefix]

    nrrdReader1 Update
    puts "[[nrrdReader1 GetOutput] Print]"
    
    catch "ap Delete"
    vtkImageAppend ap
    ap SetAppendAxis 2
    set ncomp [[nrrdReader1 GetOutput] GetNumberOfScalarComponents]
    puts "Num comp: $ncomp"
    if { $ncomp > 3 } {
       for {set i 0} {$i < $ncomp} {incr i} {
         catch "e$i Delete"
         vtkImageExtractComponents e$i
         e$i SetInput [nrrdReader1 GetOutput]
         e$i SetComponents $i
         e$i Update
         ap AddInput [e$i GetOutput]
      }
      ap Update  
        for {set i 0} {$i <$ncomp} {incr i} {
          e$i Delete
       }
      Volume($v,vol) SetImageData [ap GetOutput]
    } else {       
      Volume($v,vol) SetImageData [nrrdReader1 GetOutput]
   }
   
    set n [[[[nrrdReader1 GetOutput] GetPointData] GetScalars] GetNumberOfComponents]
    Volume($v,node) SetNumScalars $n
    
    catch "ap Delete"
    nrrdReader1 Delete

    set Volume(fileType) ""
}

