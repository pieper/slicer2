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
#   VolFreeSurferReadersSetVolumeFileName
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

    # for closing out a scene
    set Volume(VolFreeSurferReaders,idList) ""
    set Module($e,procMainFileCloseUpdateEntered) VolFreeSurferReadersMainFileCloseUpdate

    # set up the mapping between the surface labels and umls ids
    VolFreeSurferReadersSetUMLSMapping

    # for loading associated model files
    set Volume(VolFreeSurferReaders,assocFiles) ""
    set Volume(VolFreeSurferReaders,scalars) "thickness curv sulc area"
    set Volume(VolFreeSurferReaders,surfaces) "inflated pial smoothwm sphere"
    set Volume(VolFreeSurferReaders,annots) "aparc cma_aparc" 

    # the default colour table file name
    set Volume(VolFreeSurferReaders,colorTableFilename) [ExpandPath [file join $::PACKAGE_DIR_VTKFREESURFERREADERS ".." ".." ".." tcl "Simple_surface_labels2002.txt"]]
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
    package require Iwidgets
    iwidgets::scrolledframe $parentFrame.sfFreeSurferReaders \
        -vscrollmode static -hscrollmode dynamic \
        -background $Gui(activeWorkspace)  -activebackground $Gui(activeButton) -troughcolor $Gui(normalButton)  -highlightthickness 0 -relief flat -sbwidth 10

    pack $parentFrame.sfFreeSurferReaders -expand 1 -fill both
    set f [$parentFrame.sfFreeSurferReaders childsite]
    # hack
    set parentFrame  [$parentFrame.sfFreeSurferReaders childsite]

    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $f.fModel -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $f.fLogo -bg $Gui(activeWorkspace)
    frame $f.fDemo -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $f.fLogo $f.fVolume $f.fModel $f.fDemo \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Logo frame
    #-------------------------------------------
    set f $parentFrame.fLogo
    set logoFile [ExpandPath [file join $::PACKAGE_DIR_VTKFREESURFERREADERS ".." ".." ".." tcl images "FreeSurferlogo.ppm"]]
    if {[file exists $logoFile]} {
        image create photo iFSLogo -file $logoFile
        eval {label $f.lLogo -image iFSLogo -width 179 -height 30 -anchor center} \
            -bg $Gui(activeWorkspace)   -padx 0 -pady 0 
    } else {
        eval {label $f.lLogo -text "FreeSurfer" -anchor center} $Gui(WLA)
    }
    pack $f.lLogo 

    #-------------------------------------------
    # fVolume frame
    #-------------------------------------------

    set f $parentFrame.fVolume

    DevAddFileBrowse $f Volume "VolFreeSurferReaders,FileName" "FreeSurfer File:" "VolFreeSurferReadersSetFileName" "" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer file (.info or a surface)" 

    frame $f.fLabelMap -bg $Gui(activeWorkspace)
    frame $f.fDesc     -bg $Gui(activeWorkspace)
    frame $f.fName -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)

    pack $f.fLabelMap -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fApply -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    #---------
    # Volume->Name 
    #---------
    set f $parentFrame.fVolume.fName
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable Volume(name) -width 13} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad) 

    #---------
    # Volume->Desc
    #---------
    set f $parentFrame.fVolume.fDesc

    eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
    eval {entry $f.eDesc -textvariable Volume(desc)} $Gui(WEA)
    pack $f.lDesc -side left -padx $Gui(pad)
    pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x

    #---------
    # Volume->LabelMap
    #---------
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

    #------------
    # Volume->Apply 
    #------------
    set f $parentFrame.fVolume.fApply
        
    DevAddButton $f.bApply "Apply" "VolFreeSurferReadersApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    #-------------------------------------------
    # fModel frame
    #-------------------------------------------

    set f $parentFrame.fModel

    DevAddFileBrowse $f Volume "VolFreeSurferReaders,ModelFileName" "Model File:" "VolFreeSurferReadersSetModelFileName" "orig" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer surface file (orig ${Volume(VolFreeSurferReaders,surfaces)})"
    frame $f.fName -bg $Gui(activeWorkspace)
    frame $f.fSurface -bg $Gui(activeWorkspace)
    frame $f.fScalar -bg $Gui(activeWorkspace)
    frame $f.fAnnotation -bg $Gui(activeWorkspace)
    frame $f.fAnnotColor -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)
 
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fSurface -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fScalar -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fAnnotation -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fAnnotColor -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    DevAddFileBrowse $f.fAnnotColor Volume "VolFreeSurferReaders,colorTableFilename" "Annotation Color file:" "VolFreeSurferReadersSetAnnotColorFileName" "txt" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer annotation colour table file (txt)"

    pack $f.fApply -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1

    #------------
    # Model->Name
    #------------
    set f $parentFrame.fModel.fName
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable Volume(VolFreeSurferReaders,ModelName) -width 8} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad) 


    if {0} {
        # need to work this out still, for now you can load these instead of origs
    #------------
    # Model->Surface 
    #------------
    # surface files (mesh): e.g., lh.inflated, lh.pial, lh.smoothwm, lh.sphere
    set f $parentFrame.fModel.fSurface
    DevAddLabel $f.lTitle "Load Associated Surface Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach surface $Volume(VolFreeSurferReaders,surfaces) {
        eval {checkbutton $f.c$surface \
                  -text $surface -command "VolFreeSurferReadersSetLoad $surface " \
                  -variable Volume(VolFreeSurferReaders,assocFiles,$surface) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$surface -side top -padx 0
    }
}
    #------------
    # Model->Scalar 
    #------------
    # curvature (scalar): e.g., lh.thickness, lh.curv, lh.sulc, lh.area
    set f $parentFrame.fModel.fScalar
    DevAddLabel $f.lTitle "Load Associated Scalar files:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach scalar $Volume(VolFreeSurferReaders,scalars) {
        eval {checkbutton $f.r$scalar \
                  -text "$scalar" -command "VolFreeSurferReadersSetLoad $scalar" \
                  -variable Volume(VolFreeSurferReaders,assocFiles,$scalar) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.r$scalar -side top -padx 0
    }

    #------------
    # Model->Annotation 
    #------------
    # e)annotation files: lh.xxx.annot
    set f $parentFrame.fModel.fAnnotation
    DevAddLabel $f.lTitle "Load Associated Annotation files:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach annot $Volume(VolFreeSurferReaders,annots) {
        eval {checkbutton $f.c$annot \
                  -text "$annot" -command "VolFreeSurferReadersSetLoad $annot" \
                  -variable Volume(VolFreeSurferReaders,assocFiles,$annot) -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$annot -side top -padx 0
    }

    #------------
    # Model->Apply 
    #------------
    set f $parentFrame.fModel.fApply
        
    DevAddButton $f.bApply "Apply" "VolFreeSurferReadersModelApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolFreeSurferReadersModelCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    #-------------------------------------------
    # Demo Button
    #-------------------------------------------
    set f $parentFrame.fDemo
    eval button $f.bDemo -text Demo -command "VolFreeSurferDemo" $Gui(WBA)
    pack $f.bDemo  -side left -padx $Gui(pad) -pady 0

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
    catch "source $env(SLICER_HOME)/Modules/vtkFreeSurferReaders/tcl/ccdb.tcl"
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
# .PROC VolFreeSurferReadersSetVolumeFileName
# The filename is set elsehwere, in variable Volume(VolFreeSurferReaders,VolumeFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetVolumeFileName {} {
    global Volume Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders filename: $Volume(VolFreeSurferReaders,VolumeFileName)"
    }
    set Volume(name) [file tail $Volume(VolFreeSurferReaders,VolumeFileName)]
    # replace . with -
    regsub -all {[.]} $Volume(name) {-} Volume(name)
}


#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersSetModelFileName
# The filename is set elsehwere, in variable Volume(VolFreeSurferReaders,ModelFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetModelFileName {} {
    global Volume Model Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders filename: $Volume(VolFreeSurferReaders,ModelFileName)"
    }
    set Model(name) [file tail $Volume(VolFreeSurferReaders,ModelFileName)]
    # replace . with -
    regsub -all {[.]} $Model(name) {-} Model(name)
}

proc VolFreeSurferReadersSetAnnotColorFileName {} {
    global Volume
    if {$::Module(verbose)} {
        puts "VolFreeSurferReadersSetAnnotColorFileName: annotation colour file name set to $Volume(VolFreeSurferReaders,colorTableFilename)"
    }
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersApply
# Read in the volume specified.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersApply {} {
    global Module Volume

    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
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
  
    # read in the COR file
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }
    vtkCORReader Volume($i,vol,rw)
    
    # get the directory name from the filename
    Volume($i,vol,rw) SetFilePrefix [file dirname $Volume(VolFreeSurferReaders,VolumeFileName)]
    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReadersApply: set COR prefix to [Volume($i,vol,rw) GetFilePrefix]\nCalling Update on volume $i"
    }
    Volume($i,vol,rw) Update

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
   
    # set the volume properties: read the header first: sets the Volume array values we need
    VolFreeSurferReadersCORHeaderRead $Volume(VolFreeSurferReaders,VolumeFileName)
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
    Volume($i,node) SetFilePrefix [string trimright [file rootname $Volume(VolFreeSurferReaders,VolumeFileName)] "-"]
# [Volume($i,vol,rw) GetFilePrefix]
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarTypeToUnsignedChar
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "VolFreeSurferReaders: setting full prefix for volume node $i"
    }
    Volume($i,node) SetFullPrefix [string trimright [file rootname $Volume(VolFreeSurferReaders,VolumeFileName)] "-"]

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
        puts "VolFreeSurferReaders: failed to read in the volume $i, $Volume(VolFreeSurferReaders,VolumeFileName)"
        DevErrorWindow "VolFreeSurferReaders: failed to read in the volume $i, $Volume(VolFreeSurferReaders,VolumeFileName)"
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
    Model($m,reader) SetFileName $Volume(VolFreeSurferReaders,ModelFileName)

    vtkPolyDataNormals Model($m,normals)
    Model($m,normals) SetSplitting 0
    Model($m,normals) SetInput [Model($m,reader) GetOutput]

    vtkStripper Model($m,stripper)
    Model($m,stripper) SetInput [Model($m,normals) GetOutput]

    # should be vtk model
    foreach r $Module(Renderers) {
        # Mapper
        if {$::Module(verbose)} { puts "Deleting Model($m,mapper,$r)" }
        catch "Model($m,mapper,$r) Delete"
        vtkPolyDataMapper Model($m,mapper,$r)
#Model($m,mapper,$r) SetInput [Model($m,reader) GetOutput]
    }

    # Delete the src, leaving the data in Model($m,polyData)
    # polyData will survive as long as it's the input to the mapper
    #
    Model($m,node) SetName $Volume(VolFreeSurferReaders,ModelName)
    Model($m,node) SetFileName $Volume(VolFreeSurferReaders,FileName)
    set Model($m,polyData) [Model($m,stripper) GetOutput]
    $Model($m,polyData) Update

    #-------------------------
    # read in the other surface files somehow, as additional vectors doesn't work

    if {0} {

    set numSurfaces 0
    foreach s $Volume(VolFreeSurferReaders,surfaces) {
        if {[lsearch $Volume(VolFreeSurferReaders,assocFiles) $s] != -1} {
            incr numSurfaces
        }
    }
    if {$::Module(verbose)} {
        DevInfoWindow "Have $numSurfaces vector arrays associated with this model, call:\nVolFreeSurferReadersSetModelVector $m vectorName"
    }
    set numSurfacesAdded 0
    foreach s $Volume(VolFreeSurferReaders,surfaces) {
        if {[lsearch $Volume(VolFreeSurferReaders,assocFiles) $s] != -1} {
            set surfaceFileName [file rootname $Volume(VolFreeSurferReaders,ModelFileName)].$s
            if [file exists $surfaceFileName] {
                DevInfoWindow "Model $m: Reading in $s file associated with this surface: $surfaceFileName"
                # need to delete these so that if close the scene and reopen a surface file, these won't still exist
                if {$::Module(verbose)} {
                    puts "Deleting Model($m,floatArray$s)..."
                }
                catch "Model($m,floatArray$s) Delete"
                vtkFloatArray Model($m,floatArray$s)
                Model($m,floatArray$s) SetName $s
                vtkFSSurfaceReader Model($m,sr$s)
                
                Model($m,sr$s) SetFileName $surfaceFileName
                Model($m,sr$s) SetOutput Model($m,floatArray$s)
                Model($m,sr$s) Update
                # may need to call Model($m,sr$s) Execute
                
                if {$::Module(verbose)} {
                    DevInfoWindow "Adding surface named $s to model id $m"
                }
                [$Model($m,polyData) GetPointData] AddVector Model($m,floatArray$s)
                # may have some missing files
                incr numSurfacesAdded 
                if {$numSurfacesAdded == 1} {
                    # set the first one active
                    [$Model($m,polyData) GetPointData] SetActiveVectors $s
                }   
                # may need to set reader output to "" and delete
            } else {
                 DevWarningWindow "Surface file does not exist: $surfaceFileName"
            }
        }
    }
} 
    #-------------------------
    # read in the scalar files
    # check if there's more than one
    set numScalars 0
    foreach s $Volume(VolFreeSurferReaders,scalars) {
        if {[lsearch $Volume(VolFreeSurferReaders,assocFiles) $s] != -1} {
            incr numScalars
        }
    }
    if {$::Module(verbose)} {
        DevInfoWindow "Have $numScalars scalar arrays associated with this model, call:\nVolFreeSurferReadersSetModelScalar $m scalarName"
    }
    set numScalarsAdded 0
    foreach s $Volume(VolFreeSurferReaders,scalars) {
        if {[lsearch $Volume(VolFreeSurferReaders,assocFiles) $s] != -1} {
            set scalarFileName [file rootname $Volume(VolFreeSurferReaders,ModelFileName)].$s
            if [file exists $scalarFileName] {
                DevInfoWindow "Model $m: Reading in $s file associated with this surface: $scalarFileName"
                # need to delete these so that if close the scene and reopen a surface file, these won't still exist
                if {$::Module(verbose)} {
                    puts "Deleting Model($m,floatArray$s)..."
                }
                catch "Model($m,floatArray$s) Delete"
                vtkFloatArray Model($m,floatArray$s)
                Model($m,floatArray$s) SetName $s
                catch "Model($m,ssr$s) Delete"
                vtkFSSurfaceScalarReader Model($m,ssr$s)

                Model($m,ssr$s) SetFileName $scalarFileName
                # this doesn't work on solaris, can't cast float array to vtkdataobject
                Model($m,ssr$s) SetOutput Model($m,floatArray$s)
                
                Model($m,ssr$s) ReadFSScalars

                # if there's going to be more than one, use add array, otherwise just set it
                if {$numScalars == 1} {
                    [$Model($m,polyData) GetPointData] SetScalars Model($m,floatArray$s)
                } else {
                    if {$::Module(verbose)} {
                        DevInfoWindow "Adding scalar named $s to model id $m"
                    }
                    [$Model($m,polyData) GetPointData] AddArray Model($m,floatArray$s)
                    # may have some missing files
                    incr numScalarsAdded 
                    if {$numScalarsAdded == 1} {
                        # set the first one active
                        [$Model($m,polyData) GetPointData] SetActiveScalars $s
                    }
                }
                # may need to set reader output to "" and delete here
            } else {
                DevWarningWindow "Scalar file does not exist: $scalarFileName"
            }
        }
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
# .PROC VolFreeSurferReadersSetModelScalar
# Set the model's active scalars. If an invalid scalar name is passed, will pop up an info box.
# .ARGS
# modelID the model id
# scalarName the name given to the scalar field
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetModelScalar {modelID {scalarName ""}} {
    global Volume VolFreeSurferReaders
    if {$scalarName == ""} {
        foreach s "$Volume(VolFreeSurferReaders,scalars) $Volume(VolFreeSurferReaders,annots)" {
            if {$VolFreeSurferReaders(current$s)} {
                set scalarName $s
                puts "Displaying $s"
            }
        }
    }
    if {$::Module(verbose)} {
        puts "set model scalar, model $modelID -> $scalarName"
    }
    if {$scalarName != ""} {
        set retval [[$::Model(${modelID},polyData) GetPointData] SetActiveScalars $scalarName]
        if {$retval == -1} {
            DevInfoWindow "Model $modelID does not have a scalar field named $scalarName.\nPossible options are: $::Volume(VolFreeSurferReaders,scalars)"
        } else {
            # reset the scalar range here
            Render3D
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersSetModelVector
# Set the model's active vector. If an invalid vector name is passed, will pop up an info box.
# .ARGS
# modelID the model id
# vectorName the name given to the vector field
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetModelVector {modelID {vectorName ""}} {
    if {$vectorName == ""} {
        set vectorName $VolFreeSurferReaders(currentSurface)
    }
    if {$::Module(verbose)} {
        puts "set model vector, model $modelID -> $vectorName"
    }
    set retval [[$::Model(${modelID},polyData) GetPointData] SetActiveVectors $vectorName]
    if {$retval == -1} {
        DevInfoWindow "Model $modelID does not have a vector field named $vectorName.\nPossible options are: orig $::Volume(VolFreeSurferReaders,surfaces)"
    } else {
        Render3D
    }
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersDisplayPopup
# A convenience pop up window that will allow you to set the active vectors and scalars for a model
# .ARGS
# modelID the id of the model to build the gui for
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersDisplayPopup {modelID} {
    global Volume Model Gui

    DevInfoWindow "Not working 100%... use at your own risk"

    set _w .freeSurferDisplay$modelID
    catch "destroy $_w"
    toplevel $_w
    wm title $_w "FreeSurfer Model $modelID Display Options"
    frame $_w.fSurfaces -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $_w.fScalars -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $_w.fSurfaces $_w.fScalars -side top -fill x -pady $Gui(pad) -expand 1

    #--------
    # surfaces
    #--------
    if {0} {
    set f $_w.fSurfaces
    DevAddLabel $f.lTitle "Possible Surface Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach surface {orig $Volume(VolFreeSurferReaders,surfaces)} {
        eval {checkbutton $f.c$surface \
                  -text $surface -command "VolFreeSurferReadersSetModelVector $modelID" \
                  -variable VolFreeSurferReaders(currentSurface)
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$surface -side top -padx 0
    }
    }
    #---------
    # scalars
    #---------
    set f $_w.fScalars
    DevAddLabel $f.lTitle "Possible Scalar Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach scalar "$Volume(VolFreeSurferReaders,scalars) $Volume(VolFreeSurferReaders,annots)" {
        eval {checkbutton $f.c$scalar \
                  -text $scalar -command "VolFreeSurferReadersSetModelScalar $modelID" \
                  -variable VolFreeSurferReaders(current$scalar) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$scalar -side top -padx 0
    }
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

# generated via /home/nicole/bin/readumls.sh
proc VolFreeSurferReadersSetUMLSMapping {} {
    # set up Freesurfer surface labels to UMLS mapping
    global VolFreeSurferReadersSurface
    set VolFreeSurferReadersSurface(0,surfacelabel) Unknown
    set VolFreeSurferReadersSurface(0,umls) Unknown
    set VolFreeSurferReadersSurface(1,surfacelabel) Corpus_callosum
    set VolFreeSurferReadersSurface(1,umls) C0010090
    set VolFreeSurferReadersSurface(2,surfacelabel) G_and_S_Insula_ONLY_AVERAGE
    set VolFreeSurferReadersSurface(3,surfacelabel) G_cingulate-Isthmus
    set VolFreeSurferReadersSurface(3,umls) C0175192
    set VolFreeSurferReadersSurface(4,surfacelabel) G_cingulate-Main_part
    set VolFreeSurferReadersSurface(4,umls) C0018427
    set VolFreeSurferReadersSurface(5,surfacelabel) G_cuneus
    set VolFreeSurferReadersSurface(5,umls) C0152307
    set VolFreeSurferReadersSurface(6,surfacelabel) G_frontal_inf-Opercular_part
    set VolFreeSurferReadersSurface(6,umls) C0262296
    set VolFreeSurferReadersSurface(7,surfacelabel) G_frontal_inf-Orbital_part
    set VolFreeSurferReadersSurface(7,umls) C0262300
    set VolFreeSurferReadersSurface(8,surfacelabel) G_frontal_inf-Triangular_part
    set VolFreeSurferReadersSurface(8,umls) C0262350
    set VolFreeSurferReadersSurface(9,surfacelabel) G_frontal_middle
    set VolFreeSurferReadersSurface(9,umls) C0152297
    set VolFreeSurferReadersSurface(10,surfacelabel) G_frontal_superior
    set VolFreeSurferReadersSurface(10,umls) C0152296
    set VolFreeSurferReadersSurface(11,surfacelabel) G_frontomarginal
    set VolFreeSurferReadersSurface(12,surfacelabel) G_insular_long
    set VolFreeSurferReadersSurface(12,umls) C0228261
    set VolFreeSurferReadersSurface(13,surfacelabel) G_insular_short
    set VolFreeSurferReadersSurface(13,umls) C0262329
    set VolFreeSurferReadersSurface(14,surfacelabel) G_occipital_inferior
    set VolFreeSurferReadersSurface(14,umls) C0228231
    set VolFreeSurferReadersSurface(15,surfacelabel) G_occipital_middle
    set VolFreeSurferReadersSurface(15,umls) C0228220
    set VolFreeSurferReadersSurface(16,surfacelabel) G_occipital_superior
    set VolFreeSurferReadersSurface(16,umls) C0228230
    set VolFreeSurferReadersSurface(17,surfacelabel) G_occipit-temp_lat-Or_fusiform
    set VolFreeSurferReadersSurface(17,umls) C0228243
    set VolFreeSurferReadersSurface(18,surfacelabel) G_occipit-temp_med-Lingual_part
    set VolFreeSurferReadersSurface(18,umls) C0228244
    set VolFreeSurferReadersSurface(19,surfacelabel) G_occipit-temp_med-Parahippocampal_part
    set VolFreeSurferReadersSurface(19,umls) C0228249
    set VolFreeSurferReadersSurface(20,surfacelabel) G_orbital
    set VolFreeSurferReadersSurface(20,umls) C0152301
    set VolFreeSurferReadersSurface(21,surfacelabel) G_paracentral
    set VolFreeSurferReadersSurface(21,umls) C0228203
    set VolFreeSurferReadersSurface(22,surfacelabel) G_parietal_inferior-Angular_part
    set VolFreeSurferReadersSurface(22,umls) C0152305
    set VolFreeSurferReadersSurface(23,surfacelabel) G_parietal_inferior-Supramarginal_part
    set VolFreeSurferReadersSurface(23,umls) C0458319
    set VolFreeSurferReadersSurface(24,surfacelabel) G_parietal_superior
    set VolFreeSurferReadersSurface(24,umls) C0152303
    set VolFreeSurferReadersSurface(25,surfacelabel) G_postcentral
    set VolFreeSurferReadersSurface(25,umls) C0152302
    set VolFreeSurferReadersSurface(26,surfacelabel) G_precentral
    set VolFreeSurferReadersSurface(26,umls) C0152299
    set VolFreeSurferReadersSurface(27,surfacelabel) G_precuneus
    set VolFreeSurferReadersSurface(27,umls) C0152306
    set VolFreeSurferReadersSurface(28,surfacelabel) G_rectus
    set VolFreeSurferReadersSurface(28,umls) C0152300
    set VolFreeSurferReadersSurface(29,surfacelabel) G_subcallosal
    set VolFreeSurferReadersSurface(29,umls) C0175231
    set VolFreeSurferReadersSurface(30,surfacelabel) G_subcentral
    set VolFreeSurferReadersSurface(31,surfacelabel) G_temp_sup-G_temp_transv_and_interm_S
    set VolFreeSurferReadersSurface(31,umls) C0152309
    set VolFreeSurferReadersSurface(32,surfacelabel) G_temp_sup-Lateral_aspect
    set VolFreeSurferReadersSurface(32,umls) C0152309
    set VolFreeSurferReadersSurface(33,surfacelabel) G_temp_sup-Planum_polare
    set VolFreeSurferReadersSurface(33,umls) C0152309
    set VolFreeSurferReadersSurface(34,surfacelabel) G_temp_sup-Planum_tempolale
    set VolFreeSurferReadersSurface(34,umls) C0152309
    set VolFreeSurferReadersSurface(35,surfacelabel) G_temporal_inferior
    set VolFreeSurferReadersSurface(35,umls) C0152311
    set VolFreeSurferReadersSurface(36,surfacelabel) G_temporal_middle
    set VolFreeSurferReadersSurface(36,umls) C0152310
    set VolFreeSurferReadersSurface(37,surfacelabel) G_transverse_frontopolar
    set VolFreeSurferReadersSurface(38,surfacelabel) Lat_Fissure-ant_sgt-ramus_horizontal
    set VolFreeSurferReadersSurface(38,umls) C0262190
    set VolFreeSurferReadersSurface(39,surfacelabel) Lat_Fissure-ant_sgt-ramus_vertical
    set VolFreeSurferReadersSurface(39,umls) C0262186
    set VolFreeSurferReadersSurface(40,surfacelabel) Lat_Fissure-post_sgt
    set VolFreeSurferReadersSurface(40,umls) C0262310
    set VolFreeSurferReadersSurface(41,surfacelabel) Medial_wall
    set VolFreeSurferReadersSurface(42,surfacelabel) Pole_occipital
    set VolFreeSurferReadersSurface(42,umls) C0228217
    set VolFreeSurferReadersSurface(43,surfacelabel) Pole_temporal
    set VolFreeSurferReadersSurface(43,umls) C0149552
    set VolFreeSurferReadersSurface(44,surfacelabel) S_calcarine
    set VolFreeSurferReadersSurface(44,umls) C0228224
    set VolFreeSurferReadersSurface(45,surfacelabel) S_central
    set VolFreeSurferReadersSurface(45,umls) C0228188
    set VolFreeSurferReadersSurface(46,surfacelabel) S_central_insula
    set VolFreeSurferReadersSurface(46,umls) C0228260
    set VolFreeSurferReadersSurface(47,surfacelabel) S_cingulate-Main_part
    set VolFreeSurferReadersSurface(47,umls) C0228189
    set VolFreeSurferReadersSurface(48,surfacelabel) S_cingulate-Marginalis_part
    set VolFreeSurferReadersSurface(48,umls) C0259792
    set VolFreeSurferReadersSurface(49,surfacelabel) S_circular_insula_anterior
    set VolFreeSurferReadersSurface(49,umls) C0228258
    set VolFreeSurferReadersSurface(50,surfacelabel) S_circular_insula_inferior
    set VolFreeSurferReadersSurface(50,umls) C0228258
    set VolFreeSurferReadersSurface(51,surfacelabel) S_circular_insula_superior
    set VolFreeSurferReadersSurface(51,umls) C0228258
    set VolFreeSurferReadersSurface(52,surfacelabel) S_collateral_transverse_ant
    set VolFreeSurferReadersSurface(52,umls) C0228226
    set VolFreeSurferReadersSurface(53,surfacelabel) S_collateral_transverse_post
    set VolFreeSurferReadersSurface(53,umls) C0228226
    set VolFreeSurferReadersSurface(54,surfacelabel) S_frontal_inferior
    set VolFreeSurferReadersSurface(54,umls) C0262251
    set VolFreeSurferReadersSurface(55,surfacelabel) S_frontal_middle
    set VolFreeSurferReadersSurface(55,umls) C0228199
    set VolFreeSurferReadersSurface(56,surfacelabel) S_frontal_superior
    set VolFreeSurferReadersSurface(56,umls) C0228198
    set VolFreeSurferReadersSurface(57,surfacelabel) S_frontomarginal
    set VolFreeSurferReadersSurface(57,umls) C0262269
    set VolFreeSurferReadersSurface(58,surfacelabel) S_intermedius_primus-Jensen
    set VolFreeSurferReadersSurface(59,surfacelabel) S_intracingulate
    set VolFreeSurferReadersSurface(59,umls) C0262263
    set VolFreeSurferReadersSurface(60,surfacelabel) S_intraparietal-and_Parietal_transverse
    set VolFreeSurferReadersSurface(60,umls) C0228213
    set VolFreeSurferReadersSurface(61,surfacelabel) S_occipital_anterior
    set VolFreeSurferReadersSurface(61,umls) C0262194
    set VolFreeSurferReadersSurface(62,surfacelabel) S_occipital_inferior
    set VolFreeSurferReadersSurface(62,umls) C0262253
    set VolFreeSurferReadersSurface(63,surfacelabel) S_occipital_middle_and_Lunatus
    set VolFreeSurferReadersSurface(63,umls) C0262278
    set VolFreeSurferReadersSurface(64,surfacelabel) S_occipital_superior_and_transversalis
    set VolFreeSurferReadersSurface(64,umls) C0262348
    set VolFreeSurferReadersSurface(65,surfacelabel) S_occipito-temporal_lateral
    set VolFreeSurferReadersSurface(65,umls) C0228245
    set VolFreeSurferReadersSurface(66,surfacelabel) S_occipito-temporal_medial_and_S_Lingual
    set VolFreeSurferReadersSurface(66,umls) C0447427
    set VolFreeSurferReadersSurface(67,surfacelabel) S_orbital_lateral
    set VolFreeSurferReadersSurface(67,umls) C0228229
    set VolFreeSurferReadersSurface(68,surfacelabel) S_orbital_medial-Or_olfactory
    set VolFreeSurferReadersSurface(68,umls) C0262286
    set VolFreeSurferReadersSurface(69,surfacelabel) S_orbital-H_shapped
    set VolFreeSurferReadersSurface(69,umls) C0228206
    set VolFreeSurferReadersSurface(70,surfacelabel) S_paracentral
    set VolFreeSurferReadersSurface(70,umls) C0228204
    set VolFreeSurferReadersSurface(71,surfacelabel) S_parieto_occipital
    set VolFreeSurferReadersSurface(71,umls) C0228191
    set VolFreeSurferReadersSurface(72,surfacelabel) S_pericallosal
    set VolFreeSurferReadersSurface(73,surfacelabel) S_postcentral
    set VolFreeSurferReadersSurface(73,umls) C0228212
    set VolFreeSurferReadersSurface(74,surfacelabel) S_precentral-Inferior-part
    set VolFreeSurferReadersSurface(74,umls) C0262257
    set VolFreeSurferReadersSurface(75,surfacelabel) S_precentral-Superior-part
    set VolFreeSurferReadersSurface(75,umls) C0262338
    set VolFreeSurferReadersSurface(76,surfacelabel) S_subcentral_ant
    set VolFreeSurferReadersSurface(76,umls) C0262197
    set VolFreeSurferReadersSurface(77,surfacelabel) S_subcentral_post
    set VolFreeSurferReadersSurface(77,umls) C0262318
    set VolFreeSurferReadersSurface(78,surfacelabel) S_suborbital
    set VolFreeSurferReadersSurface(79,surfacelabel) S_subparietal
    set VolFreeSurferReadersSurface(79,umls) C0228216
    set VolFreeSurferReadersSurface(80,surfacelabel) S_supracingulate
    set VolFreeSurferReadersSurface(81,surfacelabel) S_temporal_inferior
    set VolFreeSurferReadersSurface(81,umls) C0228242
    set VolFreeSurferReadersSurface(82,surfacelabel) S_temporal_superior
    set VolFreeSurferReadersSurface(82,umls) C0228237
    set VolFreeSurferReadersSurface(83,surfacelabel) S_temporal_transverse
    set VolFreeSurferReadersSurface(83,umls) C0228239
    set VolFreeSurferReadersSurface(84,surfacelabel) S_transverse_frontopolar

    return
}

#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersReadAnnotations 
# Read in the annotations specified.
# .ARGS
# _id model id
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersReadAnnotations {_id} {
    global Volume Model

    # read in this annot file, it's one up from the surf dir, then down into label
    set dir [file split [file dirname $Volume(VolFreeSurferReaders,ModelFileName)]]
    set dir [lrange $dir 0 [expr [llength $dir] - 2]]
    lappend dir label
    set dir [eval file join $dir]
    set fname [lrange [file split [file rootname $Volume(VolFreeSurferReaders,ModelFileName)]] end end]

    foreach a $Volume(VolFreeSurferReaders,annots) {
        if {[lsearch $Volume(VolFreeSurferReaders,assocFiles) $a] != -1} {
            set annotFileName [eval file join $dir $fname.$a.annot]
            if [file exists $annotFileName] {
                DevInfoWindow "Model $_id: Reading in $a file associated with this model: $annotFileName"
                set scalaridx [[$Model($_id,polyData) GetPointData] SetActiveScalars "labels"] 
                    
                if { $scalaridx == "-1" } {
                    if {$::Module(verbose)} {
                        DevInfoWindow "labels scalar doesn't exist for model $_id, creating"
                    }
                    set scalars scalars_$a
                    catch "$scalars Delete"
                    vtkIntArray $scalars
                    $scalars SetName "labels"
                    [$Model($_id,polyData) GetPointData] AddArray $scalars
                    [$Model($_id,polyData) GetPointData] SetActiveScalars "labels"
                } else {
                    set scalars [[$Model($_id,polyData) GetPointData] GetScalars $scalaridx]
                }
                set lut [Model($_id,mapper,viewRen) GetLookupTable]
                $lut SetRampToLinear
                
                set fssar fssar_$a
                catch "$fssar Delete"
                vtkFSSurfaceAnnotationReader $fssar
                $fssar SetFileName $annotFileName
                $fssar SetOutput $scalars
                $fssar SetColorTableOutput $lut
                # set the optional colour table filename
                if [file exists $Volume(VolFreeSurferReaders,colorTableFilename)] {
                    if {$::Module(verbose)} {
                        puts "Color table file exists: $Volume(VolFreeSurferReaders,colorTableFilename)"
                    }
                    $fssar SetColorTableFileName $Volume(VolFreeSurferReaders,colorTableFilename)
                    $fssar UseExternalColorTableFileOn
                } else {
                    if {$::Module(verbose)} {
                        puts "Color table file does not exist: $Volume(VolFreeSurferReaders,colorTableFilename)"
                    }
                    $fssar UseExternalColorTableFileOff
                }
                set retval [$fssar ReadFSAnnotation]
                if {$::Module(verbose)} {
                    puts "Return value from reading $annotFileName = $retval"
                }
                if {[VolFreeSurferReadersCheckAnnotError $retval] != 0} {
                     [$Model($_id,polyData) GetPointData] RemoveArray "labels"
                    return
                }
                
                set ::Model(scalarVisibilityAuto) 0

                array unset _labels
                array set _labels [$fssar GetColorTableNames]
                set entries [lsort -integer [array names _labels]]
                if {$::Module(verbose)} {
                    puts "Label entries:\n$entries"
                    puts "0th: [lindex $entries 0], last:  [lindex $entries end]"
                }
                MainModelsSetScalarRange $_id [lindex $entries 0] [lindex $entries end]
                MainModelsSetScalarVisibility $_id 1
                Render3D
            } else {
                DevInfoWindow "Model $_id: $a file does not exist: $annotFileName"
            }
        }
    }
}

# references vtkFSSurfaceAnnotationReader.h. Returns error value (0 means success)
proc VolFreeSurferReadersCheckAnnotError {val} {
    if {$val == 1} {
        DevErrorWindow "ERROR: error loading colour table"
    }
    if {$val == 2} {
        DevErrorWindow "ERROR: error loading annotation"
    }
    if {$val == 3} {
        DevErrorWindow "ERROR: error parsing color table"
    }
    if {$val == 4} {
        DevErrorWindow "ERROR: error parsing annotation"
    }
    if {$val == 5} {
        DevErrorWindow "WARNING: Unassigned labels"
        return 0
    }
    if {$val == 6} {
        DevErrorWindow "ERROR: no colour table defined"
    }
    return $val
}
#-------------------------------------------------------------------------------
# .PROC VolFreeSurferReadersModelApply
# Read in the model specified. Used in place of the temp ModelsFreeSurferPropsApply.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersModelApply {} {
    global Module Model Volume
    # Validate name
    set Model(name) $Volume(VolFreeSurferReaders,ModelName)
    if {$Model(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this model."
        return
    }
    if {[ValidateName $Model(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    if {$::Module(verbose)} {
        puts "VolFreeSurferReadersModelApply: Also loading these assoc files if they exist: $Volume(VolFreeSurferReaders,assocFiles)"
    }
    # add a mrml node
    set n [MainMrmlAddNode Model]
    set i [$n GetID]

    VolFreeSurferReadersBuildSurface $i
    VolFreeSurferReadersReadAnnotations $i
    # pop up a window allowing you to choose which vectors and scalars to see
    # VolFreeSurferReadersDisplayPopup $i

    # allow use of other module GUIs
    set Volumes(freeze) 0
    # If tabs are frozen, then return to the "freezer"
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
    return
}
proc VolFreeSurferReadersModelCancel {} {
    global Module Model Volume
    # model this after VolumePropsCancel - ModelPropsCancel?
    if {$Module(verbose)} {
        puts "VolFreeSurferReadersModelCancel, calling ModelPropsCancel"
    }
    ModelsPropsCancel
}

proc VolFreeSurferReadersSetLoad {param} {
    global Volume

    if {$::Module(verbose)} {
        puts "VolFreeSurferReadersSetLoad: start: loading: $Volume(VolFreeSurferReaders,assocFiles)"
    }
    set ind [lsearch $Volume(VolFreeSurferReaders,assocFiles) $param]
    if {$Volume(VolFreeSurferReaders,assocFiles,$param) == 1} {
        if {$ind == -1} {
            # add it to the list
            lappend Volume(VolFreeSurferReaders,assocFiles) $param
        }
    } else {
        if {$ind != -1} {
            # remove it from the list
            set Volume(VolFreeSurferReaders,assocFiles) [lreplace $Volume(VolFreeSurferReaders,assocFiles) $ind $ind]
        }
    }
    if {$::Module(verbose)} {
        puts "VolFreeSurferReadersSetLoad: new: loading: $Volume(VolFreeSurferReaders,assocFiles)"
    }
}
