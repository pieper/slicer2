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
# FILE:        VolFreeSurferReaders.tcl
# PROCEDURES:  
#   VolFreeSurferReadersBuildGUI
#   VolFreeSurferReadersEnter
#   VolFreeSurferReadersExit
#   VolFreeSurferReadersSetFileName
#   VolFreeSurferReadersApply
#   VolFreeSurferReadersBuildSurface
#   VolFreeSurferReadersSetSurfaceVisibility
#   VolFreeSurferReadersMainFileCloseUpdate
#   VolFreeSurferReadersAddColors
#==========================================================================auto=

proc VolFreeSurferReadersInit {} {
    global Gui VolFreeSurferReaders Volume Slice Module

    if {$Module(verbose) == 1} {puts "VolFreeSurferReadersInit starting"}
   
    set e VolFreeSurferReaders

    set Volume(readerModules,$e,name) "FreeSurfer Readers"
    set Volume(readerModules,$e,tooltip) "This tab lets you read in FreeSurfer volumes"

    set Volume(readerModules,$e,procGUI) ${e}BuildGUI
    set Volume(readerModules,$e,procEnter) ${e}Enter
    set Volume(readerModules,$e,procExit) ${e}Exit
#    set Volume(readerModules,$e,procColor) ${e}AddColors

    # some local global variables
    set Volume(VolFreeSurferReaders,FileType) Surface
    set Volume(VolFreeSurferReaders,FileTypeList) {Surface Volume}
    set Volume(VolFreeSurferReaders,FileTypeList,tooltips) {"File contains a surface" "File contains a volume"}

    # for closing out a scene
    set Volume(VolFreeSurferReaders,idList) ""
    set Module($e,procMainFileCloseUpdateEntered) VolFreeSurferReadersMainFileCloseUpdate

}


#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersBuildGUI
# Builds the GUI for the free surfer readers, as a submodule of the Volumes module
# .ARGS
# parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersBuildGUI {parentFrame} {
    global Gui Volume Module

    if {$Module(verbose) == 1} {
        puts  "VolFreeSurferReadersBuildGUI"
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

    DevAddFileBrowse $f Volume "VolFreeSurferReaders,FileName" "FreeSurfer File:" "VolFreeSurferReadersSetFileName" "" "\$Volume(DefaultDir)"  "Browse for a FreeSurfer file (.info or a surface)" 

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

    foreach type $Volume(VolFreeSurferReaders,FileTypeList) tip $Volume(VolFreeSurferReaders,FileTypeList,tooltips) {
        eval {radiobutton $f.rMode$type \
                  -text "$type" -value "$type" \
                  -variable Volume(VolFreeSurferReaders,FileType)\
                  -indicatoron 0} $Gui(WCA) 
        pack $f.rMode$type -side left -padx $Gui(pad) -pady 0
        TooltipAdd  $f.rMode$type $tip
    }   


    #-------------------------------------------
    # Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply
        
    DevAddButton $f.bApply "Apply" "VolFreeSurferReadersApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    #-------------------------------------------
    # Demo Button
    #-------------------------------------------
    set f $parentFrame
    eval button $f.demo -text Demo -command "VolFreeSurferDemo" $Gui(WBA)
    pack $f.demo -side left -padx $Gui(pad) -pady 0

}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferDemo
# A hack for testing/development.  Assumes you have the correct data available 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferDemo {} {
    global env

    source $env(SLICER_HOME)/Modules/vtkFreeSurferReaders/tcl/regions.tcl
    set r [regions #auto]
    $r demo
}


#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersEnter
# Does nothing yet
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersEnter {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolFreeSurferReaders ENTER"}
}


#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersExit
# Does nothing yet.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersExit {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolFreeSurferReaders EXIT"}
}


#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersSetFileName
# The filename is set elsehwere, in variable Volume(VolFreeSurferReaders,FileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetFileName {} {
    global Volume Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders filename: $Volume(VolFreeSurferReaders,FileName)"
    }
    set Volume(name) [file tail $Volume(VolFreeSurferReaders,FileName)]
    # replace . with -
    regsub -all {[.]} $Volume(name) {-} Volume(name)
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersApply
# Check the file type variable and build the appropriate model or volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersApply {} {
    global Module Volume
    if {$Module(verbose) == 1} {
        puts "proc VolFreeSurferReaders Apply\nFiletype -s $Volume(VolFreeSurferReaders,FileType)"
    }

    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    ################
    # This is a cheat, do the model building stuff here for surfaces and then return
    if {[string equal $Volume(VolFreeSurferReaders,FileType) Surface]} {
        # add a mrml node
        set n [MainMrmlAddNode Model]
        set i [$n GetID]

        VolFreeSurferReadersBuildSurface $i
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
    # puts "VolFreeSurferReadersApply: NOT calling MainVolumes create on $i"
    # MainVolumesCreate $i
  
    if {[string equal $Volume(VolFreeSurferReaders,FileType) Volume]} {
        # read in the COR file
        # Set up the reading
        if {[info command Volume($i,vol,rw)] != ""} {
            # have to delete it first, needs to be cleaned up
            DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
            Volume($i,vol,rw) Delete
        }
        vtkCORReader Volume($i,vol,rw)
    
        # get the directory name from the filename
        Volume($i,vol,rw) SetFilePrefix [file dirname $Volume(VolFreeSurferReaders,FileName)]
        if {$Module(verbose) == 1} {
            puts "VolFreeSurferReadersApply: set COR prefix to [Volume($i,vol,rw) GetFilePrefix]\nCalling Update on volume $i"
        }
        Volume($i,vol,rw) Update
    } 

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
   
    # set the volume properties: read the header first: sets the Volume array values we need
    VolFreeSurferReadersCORHeaderRead $Volume(VolFreeSurferReaders,FileName)
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
    # this is the file prefix that will be used to build the image file names, needs to go up to COR
    Volume($i,node) SetFilePrefix [string trimright [file rootname $Volume(VolFreeSurferReaders,FileName)] "-"]
# [Volume($i,vol,rw) GetFilePrefix]
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarTypeToUnsignedChar
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReaders: setting full prefix for volume node $i"
    }
    Volume($i,node) SetFullPrefix [string trimright [file rootname $Volume(VolFreeSurferReaders,FileName)] "-"]

    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReaders: set up volume node for $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "VolFreeSurferReaders: volume $i position 1 3: $badval"
    
        puts "VolFreeSurferReaders: calling MainUpdateMRML"
    }
    # Reads in the volume via the Execute procedure
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "VolFreeSurferReaders: failed to read in the volume $i, $Volume(VolFreeSurferReaders,FileName)"
        DevErrorWindow "VolFreeSurferReaders: failed to read in the volume $i, $Volume(VolFreeSurferReaders,FileName)"
        return
    }
    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReaders: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "VolFreeSurferReaders: volume $i position 1 3: $badval"
    }
    # allow use of other module GUIs
    set Volumes(freeze) 0
 
    puts "New FreeSurfer Volume:"
    [Volume($i,vol) GetOutput] Print

    # set active volume on all menus
    MainVolumesSetActive $i

    # save the id for later use
    set m $i

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

    # Update all fields that the user changed (not stuff that would need a file reread)
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersBuildSurface
# Builds a model, a surface from a Freesurfer file.
# .ARGS
# m the model id
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersBuildSurface {m} {
    global viewRen Volume Module Model RemovedModels 

    if {$Module(verbose) == 1} { 
        puts "\nVolFreeSurferReadersBuildSurface\n"
    }
    # set up the reader
    vtkFSSurfaceReader Model($m,reader)
    Model($m,reader) SetFileName $Volume(VolFreeSurferReaders,FileName)

    vtkPolyDataNormals Model($m,normals)
    Model($m,normals) SetSplitting 0
    Model($m,normals) SetInput [Model($m,reader) GetOutput]

    vtkStripper Model($m,stripper)
    Model($m,stripper) SetInput [Model($m,normals) GetOutput]

    # should be vtk model
    foreach r $Module(Renderers) {
        # Mapper
        vtkPolyDataMapper Model($m,mapper,$r)
#Model($m,mapper,$r) SetInput [Model($m,reader) GetOutput]
    }

    # Delete the src, leaving the data in Model($m,polyData)
    # polyData will survive as long as it's the input to the mapper
    #
    Model($m,node) SetName $Volume(name)
    Model($m,node) SetFileName $Volume(VolFreeSurferReaders,FileName)
    set Model($m,polyData) [Model($m,normals) GetOutput]
    $Model($m,polyData) Update

    # always read in the thickness file
    set thicknessFileName [file rootname $Volume(VolFreeSurferReaders,FileName)].thickness
    if [file exists $thicknessFileName] {
        DevInfoWindow "Reading in thickness file associated with this surface $thicknessFileName"
        # need to delete these so that if close the scene and reopen a surface file, these won't still exist
        vtkFloatArray Model($m,floatArray)
        vtkFSSurfaceScalarReader Model($m,ssr)
        
        Model($m,ssr) SetFileName $thicknessFileName
        
        # this doesn't work on solaris, can't cast float array to vtkdataobject
        Model($m,ssr) SetOutput Model($m,floatArray)
        
        Model($m,ssr) ReadFSScalars
        [$Model($m,polyData) GetPointData] SetScalars Model($m,floatArray)
    } else {
        DevWarningWindow "Thickness file does not exist: $thicknessFileName"
    }

    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
   
        if {0} {
            # decimate
            vtkDecimate Model($m,decimate,$r) 
            Model($m,decimate,$r) SetInput $Model($m,polyData)
            Model($m,decimate,$r) SetMaximumIterations 6
            Model($m,decimate,$r)  SetMaximumSubIterations 0 
            Model($m,decimate,$r) PreserveEdgesOn
            Model($m,decimate,$r) SetMaximumError 1
            Model($m,decimate,$r) SetTargetReduction 1
            Model($m,decimate,$r) SetInitialError .0002
            Model($m,decimate,$r) SetErrorIncrement .0002
            [ Model($m,decimate,$r) GetOutput] ReleaseDataFlagOn
            vtkSmoothPolyDataFilter smoother
            smoother SetInput [Model($m,decimate,$r) GetOutput]
            set p smoother
            $p SetNumberOfIterations 2
            $p SetRelaxationFactor 0.33
            $p SetFeatureAngle 60
            $p FeatureEdgeSmoothingOff
            $p BoundarySmoothingOff
            $p SetConvergence 0
            [$p GetOutput] ReleaseDataFlagOn

            set Model($m,polyData) [$p GetOutput]
            Model($m,polyData) Update
            foreach r $Module(Renderers) {
                Model($m,mapper,$r) SetInput $Model($m,polyData)
            }
        }
    }
    Model($m,reader) SetOutput ""
    Model($m,reader) Delete
    Model($m,normals) SetOutput ""
    Model($m,normals) Delete
    Model($m,stripper) SetOutput ""
    Model($m,stripper) Delete

    # Clipper
    vtkClipPolyData Model($m,clipper)
    Model($m,clipper) SetClipFunction Slice(clipPlanes)
    Model($m,clipper) SetValue 0.0
    
    vtkMatrix4x4 Model($m,rasToWld)
    
    foreach r $Module(Renderers) {

        # Actor
        vtkActor Model($m,actor,$r)
        Model($m,actor,$r) SetMapper Model($m,mapper,$r)
        # Registration
        Model($m,actor,$r) SetUserMatrix [Model($m,node) GetRasToWld]

        # Property
        set Model($m,prop,$r)  [Model($m,actor,$r) GetProperty]

        # For now, the back face color is the same as the front
        Model($m,actor,$r) SetBackfaceProperty $Model($m,prop,$r)
    }
    set Model($m,clipped) 0
    set Model($m,displayScalarBar) 0

    # init gui vars
    set Model($m,visibility)       [Model($m,node) GetVisibility]
    set Model($m,opacity)          [Model($m,node) GetOpacity]
    set Model($m,scalarVisibility) [Model($m,node) GetScalarVisibility]
    set Model($m,backfaceCulling)  [Model($m,node) GetBackfaceCulling]
    set Model($m,clipping)         [Model($m,node) GetClipping]
    # set expansion to 1 if variable doesn't exist
    if {[info exists Model($m,expansion)] == 0} {
        set Model($m,expansion)    1
    }
    # set RemovedModels to 0 if variable doesn't exist
    if {[info exists RemovedModels($m)] == 0} {
        set RemovedModels($m) 0
    }

    MainModelsSetColor $m
    MainAddModelActor $m
    set Model($m,dirty) 1
    set Model($m,fly) 1

    MainUpdateMRML
    MainModelsSetActive $m

    # old version, not treating the surface file like a model
    if {0} {
        # Read in a freesurfer surface file
        # Set up the reading
        vtkFSSurfaceReader  Volume($i,vol,rw)
        # get the directory name from the filename
        Volume($i,vol,rw) SetFileName $Volume(VolFreeSurferReaders,FileName)
        puts "VolFreeSurferReadersApply: set surface $i filename to [Volume($i,vol,rw) GetFileName]"
        
        vtkPolyDataMapper Volume(VolFreeSurferReaders,$i,mapper)
        Volume(VolFreeSurferReaders,$i,mapper) SetInput [Volume($i,vol,rw) GetOutput]
        vtkActor Volume(VolFreeSurferReaders,$i,curveactor)
        Volume(VolFreeSurferReaders,$i,curveactor) SetMapper Volume(VolFreeSurferReaders,$i,mapper)
        viewRen AddActor Volume(VolFreeSurferReaders,$i,curveactor)
        Volume(VolFreeSurferReaders,$i,curveactor) SetVisibility 1
        
        # triangle stripping stuff
        if {0} {
            vtkTriangleFilter Volume(VolFreeSurferReaders,$i,stripfilter)
            Volume(VolFreeSurferReaders,$i,stripfilter) SetInput [Volume($i,vol,rw) GetOutput]
            
            vtkDecimate Volume(VolFreeSurferReaders,$i,decimate)
            Volume(VolFreeSurferReaders,$i,decimate) SetInput [Volume(VolFreeSurferReaders,$i,stripfilter) GetOutput]
            Volume(VolFreeSurferReaders,$i,decimate) SetTargetReduction 0.8
            Volume(VolFreeSurferReaders,$i,decimate) SetMaximumIterations 6
            Volume(VolFreeSurferReaders,$i,mapper) SetInput [ Volume(VolFreeSurferReaders,$i,decimate) GetOutput]
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersSetSurfaceVisibility
# Set the model's visibility flag.
# .ARGS
# i the model id
# vis the boolean flag determining this model's visibility
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetSurfaceVisibility {i vis} {
    global Volume
    # set the visibility of volume i to visibility vis
    Volume(VolFreeSurferReaders,$i,curveactor) SetVisibility $vis
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersMainFileCloseUpdate
# Called to clean up anything created in this sub module. Deletes Volumes read in, along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersMainFileCloseUpdate {} {
    global Volume viewRen Module
    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReadersMainFileCloseUpdate"
    }
    foreach f $Volume(idList) {
        #if {$Module(verbose) == 1} {}
            puts "VolFreeSurferReadersMainFileCloseUpdate: Checking volume $f"
        
        if {[info exists Volume(VolFreeSurferReaders,$f,curveactor)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing surface actor for free surfer reader id $f"
            }
            viewRen RemoveActor  Volume(VolFreeSurferReaders,$f,curveactor)
            Volume(VolFreeSurferReaders,$f,curveactor) Delete
            Volume(VolFreeSurferReaders,$f,mapper) Delete
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

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersAddColors
# Reads in the freesurfer colour file, ColorsFreesurfer.xml, and appends the color tags to the input argument, returning them all. TODO: check for duplicate colour tags and give a warning. 
# .ARGS
# tags the already set tags, append to this variable.
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersAddColors {tags} {
    global env Module
    # read, and append default freesurfer colors.
    # Return a new list of tags, possibly including default colors.
    # TODO: check for conflicts in color names

    # Set up the file name of the free surfer modules' colour xml file, 
    # it's in the Module's tcl directory. Try setting it from the slicer home
    # environment variable first, otherwise, assume search  is starting from slicer home
    if {[info exists env(SLICER_HOME)] == 1} {
        set fileName [file join $env(SLICER_HOME) Modules vtkFreeSurferReaders tcl ColorsFreesurfer.xml]
    } else {
        set fileName [file join Modules vtkFreeSurferReaders tcl ColorsFreesurfer.xml]
    }
    if {$Module(verbose) == 1} {
        puts "Trying to read Freesurfer colours from \"$fileName\""
    }
    set tagsFSColors [MainMrmlReadVersion2.x $fileName]
    if {$tagsFSColors == 0} {
        set msg "Unable to read file default MRML Freesurfer color file '$fileName'"
        puts $msg
        tk_messageBox -message $msg
        return "$tags"
    }

    return "$tags $tagsFSColors"
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersCORHeaderRead
# Reads in the freesurfer coronal volume's header file.
# .ARGS
# file the full path to the COR-.info file
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersCORHeaderRead {filename} {
    global Volume CORInfo

    # Check that a file name string has been passed in
    if {[string equal filename ""]} {
        DevErrorWindow "VolFreeSurferReadersCORHeaderRead: empty file name"
        return 1
    }
    
    # Open the file for reading
    if {[catch {set fid [open $filename]} errMsg]} {
        puts "Error opening COR header file $filename for reading"
        return 1
    }

    # Read and parse the file into name/value pairs
    set fldPat {([a-zA-Z_]+[0-9]*)}
    set numberPat {([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?}
    set i 0
    set fld ""
    while { ![eof $fid] } {
        # Read a line from the file and analyse it.
        gets $fid line 
        # puts "Line $i: $line"
        set i [expr $i + 1]
        # get the field name
        if { [regexp $fldPat $line fld]} {
            set CORInfo($fld) {}
            # puts "Found field $fld"
        
            # get the values, first test for three floats
            if { [regexp "$numberPat +$numberPat +$numberPat" $line val]} {
                set CORInfo($fld) $val
            } else {
                # one float
                if { [regexp "$numberPat" $line val]} {
                    set CORInfo($fld) $val
                } else {
                    puts "Unable to match line $line"
                }
            }
        } else {
            # only print warning if not an empty line
            if {![regexp {()|( +)} $line]} {
                puts "WARNING: no field name found at the beginning of line: $line"
            }
        }

    }
    close $fid

    # set the global vars
#    set Volume(activeID) 
    set Volume(isDICOM) 0
    if {[info exists Volume(name)] == 0} { set Volume(name) "COR"}
    set Volume(lastNum) $CORInfo(imnr1)
    set Volume(width) $CORInfo(x)
    set Volume(height) $CORInfo(y)
    # distances are in m in the COR info file, we need mm
    set Volume(pixelWidth) [expr 1000.0 * $CORInfo(psiz)]
    set Volume(pixelHeight) [expr 1000.0 * $CORInfo(psiz)]
    set Volume(sliceThickness) [expr 1000.0 * $CORInfo(thick)]
    # use the x dimension to calculate spacing from the field of view
    # set Volume(sliceSpacing) [expr 1000.0 * [expr $CORInfo(fov) /  $Volume(width)]]
    # don't use the slice spacing to anything but 0 for cor files
    set Volume(sliceSpacing) 0
    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    set Volume(littleEndian) 1
    # scan order is Coronal   Posterior Anterior
    set Volume(scanOrder) {PA}
    set Volume(scalarType) {UnsignedChar}
    set Volume(readHeaders) 0
    set Volume(filePattern) {%s-%03d}
    set Volume(dimensions) {256 256}
    set Volume(imageRange) {1 256}
    # puts "VolFreeSurferReadersCORHeaderRead: set slice spacing to $Volume(sliceSpacing), slice thickness to $Volume(sliceThickness)"
    return 0
}
