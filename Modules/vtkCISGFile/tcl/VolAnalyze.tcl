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
# FILE:        VolAnalyze.tcl
# PROCEDURES:  
#   VolAnalyzeBuildGUI
#   VolAnalyzeEnter
#   VolAnalyzeExit
#   VolAnalyzeSetFileName
#   VolAnalyzeApply
#   VolAnalyzeMainFileCloseUpdate
#==========================================================================auto=

proc VolAnalyzeInit {} {
    global Gui VolAnalyze Volume Slice Module

    if {$Module(verbose) == 1} {puts "VolAnalyzeInit starting"}
   
    set e VolAnalyze

    set Volume(readerModules,$e,name) "Analyze Readers"
    set Volume(readerModules,$e,tooltip) "This tab lets you read in Analyze volumes"

    set Volume(readerModules,$e,procGUI) ${e}BuildGUI
    set Volume(readerModules,$e,procEnter) ${e}Enter
    set Volume(readerModules,$e,procExit) ${e}Exit

    # some local global variables
    set Volume(VolAnalyze,FileType) Radiological
    set Volume(VolAnalyze,FileTypeList) {Radiological Neurological}
    set Volume(VolAnalyze,FileTypeList,tooltips) {"File contains radiological convention images" "File contains a neurological convention images"}

    # for closing out a scene
    set Volume(VolAnalyze,idList) ""
    set Module($e,procMainFileCloseUpdateEntered) VolAnalyzeMainFileCloseUpdate

}


#-------------------------------------------------------------------------------
# .PROC VolAnalyzeBuildGUI
# Builds the GUI for the free surfer readers, as a submodule of the Volumes module
# .ARGS
# parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeBuildGUI {parentFrame} {
    global Gui Volume Module

    if {$Module(verbose) == 1} {
        puts  "VolAnalyzeBuildGUI"
    }

    set f $parentFrame

    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fFileType -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fVolume $f.fFileType $f.fApply \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # fVolume frame
    #-------------------------------------------

    set f $parentFrame.fVolume

    DevAddFileBrowse $f Volume "VolAnalyze,FileName" "Analyze File:" "VolAnalyzeSetFileName" "hdr" "\$Volume(DefaultDir)" "Open" "Browse for a Analyze file (.hdr that has matching .img)" "Absolute"

    frame $f.fLabelMap -bg $Gui(activeWorkspace)

    frame $f.fDesc     -bg $Gui(activeWorkspace)

    frame $f.fName -bg $Gui(activeWorkspace)

    pack $f.fLabelMap -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

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


    if {$Module(verbose) == 1} {
        puts "Done packing the label map stuff"
    }

    #-------------------------------------------
    # f->FileType
    #-------------------------------------------
    set f $parentFrame.fFileType

    DevAddLabel $f.l "File Type: "
    pack $f.l -side left -padx $Gui(pad) -pady 0
    #set gridList $f.l

    foreach type $Volume(VolAnalyze,FileTypeList) tip $Volume(VolAnalyze,FileTypeList,tooltips) {
        eval {radiobutton $f.rMode$type \
                  -text "$type" -value "$type" \
                  -variable Volume(VolAnalyze,FileType)\
                  -indicatoron 0} $Gui(WCA) 
        pack $f.rMode$type -side left -padx $Gui(pad) -pady 0
        TooltipAdd  $f.rMode$type $tip
    }   


    #-------------------------------------------
    # Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply
        
    DevAddButton $f.bApply "Apply" "VolAnalyzeApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC VolAnalyzeEnter
# Does nothing yet
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeEnter {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolAnalyze ENTER"}
}


#-------------------------------------------------------------------------------
# .PROC VolAnalyzeExit
# Does nothing yet.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeExit {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolAnalyze EXIT"}
}


#-------------------------------------------------------------------------------
# .PROC VolAnalyzeSetFileName
# The filename is set elsehwere, in variable Volume(VolAnalyze,FileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeSetFileName {} {
    global Volume Module

    if {$Module(verbose) == 1} {
        puts "Analyze filename: $Volume(VolAnalyze,FileName)"
    }
    set Volume(name) [file tail $Volume(VolAnalyze,FileName)]
    # replace . with -
    regsub -all {[.]} $Volume(name) {-} Volume(name)
}

#-------------------------------------------------------------------------------
# .PROC VolAnalyzeApply
# Check the file type variable and build the appropriate model or volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeApply {{EPIReconOrder ""}} {
    global Module Volume

    if {$Module(verbose) == 1} {
        puts "proc VolAnalyze Apply\nFiletype -s $Volume(VolAnalyze,FileType)"
    }

    if { $Volume(VolAnalyze,FileName) == "" } {
        return
    }

    if { ![info exists Volume(name)] } { set Volume(name) "Analyze"}

    if {[ValidateName $Volume(name)] == 0} {
        DevErrorWindow "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    #
    # if this is a compressed image file, uncompress to tmp dir and read from there
    #
    set compressed [VolAnalyzeHandleCompressed $Volume(VolAnalyze,FileName) retfilename]
    if { $compressed } {
        set Volume(VolAnalyze,FileNameSave) $Volume(VolAnalyze,FileName)  
        set Volume(VolAnalyze,FileName) [file root $retfilename].hdr
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
    # puts "VolAnalyzeApply: NOT calling MainVolumes create on $i"
    # MainVolumesCreate $i
  
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }
    vtkCISGAnalyzeReader Volume($i,vol,rw)

    # get the directory name from the filename
    Volume($i,vol,rw) SetFileName $Volume(VolAnalyze,FileName)
    if {$Module(verbose) == 1} {
        puts "VolAnalyzeApply: set Analyze filename to [Volume($i,vol,rw) GetFileName]\nCalling Update on volume $i"
    }
    Volume($i,vol,rw) Update
    set imdata [Volume($i,vol,rw) GetOutput]

    set Volume(isDICOM) 0
    set Volume($i,type) "Analyze"

    set dims [$imdata GetDimensions]
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

    if { $EPIReconOrder == "" } {
        set fileType Analyze$Volume(VolAnalyze,FileType) 
    } else {
        set fileType AnalyzeEPIRecon$EPIReconOrder
        set Volume(scanOrder) $EPIReconOrder
    }

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
    Volume($i,node) SetScanOrder $Volume(scanOrder)
    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    Volume($i,node) SetFileType $fileType

    Volume($i,node) SetFilePrefix [Volume($i,vol,rw) GetFileName] ;# NB: just one file, not a pattern
    Volume($i,node) SetFullPrefix [Volume($i,vol,rw) GetFileName] ;# needed in the range check
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarType [$imdata GetScalarType]
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "VolAnalyze: setting full prefix for volume node $i"
    }

    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "VolAnalyze: failed to read in the volume $i, $Volume(VolAnalyze,FileName)"
        DevErrorWindow "VolAnalyze: failed to read in the volume $i, $Volume(VolAnalyze,FileName)"
        return
    }

    if {$Module(verbose) == 1} {
        puts "VolAnalyze: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "VolAnalyze: volume $i position 1 3: $badval"
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

    if { $compressed } {
        VolAnalyzeCleanupCompressed $Volume(VolAnalyze,FileNameSave)
        set Volume(VolAnalyze,FileName) $Volume(VolAnalyze,FileNameSave) 
        Volume($i,node) SetFilePrefix $Volume(VolAnalyze,FileNameSave)
        Volume($i,node) SetFullPrefix $Volume(VolAnalyze,FileNameSave)
    }

    return $i
}

proc VolAnalyzeHandleCompressed {filename rfilename} {

    upvar $rfilename retfilename
    set retfilename $filename
    set compressed 0
    set root [file root $filename]
    set tail [file tail $root]
    if { ! [file exists $root.img] } {
        if { ! [file exists $root.img.gz] } {
            DevErrorWindow "Can't find $root.img or $root.img.gz"
            return -1
        } else {
            # compressed version exists, uncompress and make it the one to read
            set compressed 1
            set ret [catch {
                set tmp [VolAnalyzetempdir]
                file copy -force $root.hdr $tmp/$tail.hdr
                file copy -force $root.img.gz $tmp/$tail.img.gz
                exec gzip -df $tmp/$tail.img.gz} res]
            if { $ret } {
                DevErrorWindow $res
                return -1
            }
            ## note: the tmp files get deleted by the next proc
            set retfilename $tmp/$tail.img
        }
    }
    return $compressed
}

proc VolAnalyzeCleanupCompressed { filename } {

    set tmp [VolAnalyzetempdir]
    set root [file root $filename]
    set tail [file tail $root]
    file delete $tmp/$tail.hdr
    file delete $tmp/$tail.img
}

#-------------------------------------------------------------------------------
# .PROC VolAnalyzeMainFileCloseUpdate
# Called to clean up anything created in this sub module. Deletes Volumes read in, along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolAnalyzeMainFileCloseUpdate {} {
    global Volume viewRen Module
    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "VolAnalyzeMainFileCloseUpdate"
    }
    foreach f $Volume(idList) {
        #if {$Module(verbose) == 1} {}
            puts "VolAnalyzeMainFileCloseUpdate: Checking volume $f"
        
        if {[info exists Volume(VolAnalyze,$f,curveactor)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing surface actor for free surfer reader id $f"
            }
            viewRen RemoveActor  Volume(VolAnalyze,$f,curveactor)
            Volume(VolAnalyze,$f,curveactor) Delete
            Volume(VolAnalyze,$f,mapper) Delete
            Volume($f,vol,rw) Delete
#            MainMrmlDeleteNode Volume $f
        }
        if {[info exists Volume($f,vol,rw)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing volume reader for volume id $f"
            }
            Volume($f,vol,rw) Delete
        }
    }
}


## copied from tcllib ##
#
# ::fileutil::tempdir --
#
#    Return the correct directory to use for temporary files.
#    Python attempts this sequence, which seems logical:
#
#       1. The directory named by the `TMPDIR' environment variable.
#
#       2. The directory named by the `TEMP' environment variable.
#
#       3. The directory named by the `TMP' environment variable.
#
#       4. A platform-specific location:
#            * On Macintosh, the `Temporary Items' folder.
#
#            * On Windows, the directories `C:\\TEMP', `C:\\TMP',
#              `\\TEMP', and `\\TMP', in that order.
#
#            * On all other platforms, the directories `/tmp',
#              `/var/tmp', and `/usr/tmp', in that order.
#
#        5. As a last resort, the current working directory.
#
# Arguments:
#    None.
#
# Side Effects:
#    None.
#
# Results:
#    The directory for temporary files.

proc VolAnalyzetempdir {} {
    global tcl_platform env
    set attempdirs [list]

    foreach tmp {TMPDIR TEMP TMP} {
        if { [info exists env($tmp)] } {
            lappend attempdirs $env($tmp)
        }
    }

    switch $tcl_platform(platform) {
        windows {
            lappend attempdirs "C:\\TEMP" "C:\\TMP" "\\TEMP" "\\TMP"
        }
        macintosh {
            set tmpdir $env(TRASH_FOLDER)  ;# a better place?
        }
        default {
            lappend attempdirs [file join / tmp] \
            [file join / var tmp] [file join / usr tmp]
        }
    }

    foreach tmp $attempdirs {
        if { [file isdirectory $tmp] && [file writable $tmp] } {
            return $tmp
        }
    }

    # If nothing else worked...
    return [pwd]
}

