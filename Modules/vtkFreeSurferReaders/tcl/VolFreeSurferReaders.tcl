proc VolFreeSurferReadersInit {} {
    global Gui VolFreeSurferReaders Volume Slice

    if {1} {puts "VolFreeSurferReadersInit"}
   
    set e VolFreeSurferReaders

    set Volume(readerModules,$e,name) "FreeSurfer Readers"
    set Volume(readerModules,$e,tooltip) "This tab lets you read in FreeSurfer volumes"

    set Volume(readerModules,$e,procGUI) ${e}BuildGUI
    set Volume(readerModules,$e,procEnter) ${e}Enter
    set Volume(readerModules,$e,procExit) ${e}Exit

 #   set Volume(readerModules,$e,procVTK) VolFreeSurferReadersBuildVTK
    # doesn't seem to be getting called, so do it explicitly
#    VolFreeSurferReadersBuildVTK

}
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersBuildGUI {parentFrame} {
    global Gui Volume Module

    if {$Module(verbose) == 1} {puts  "VolFreeSurferReadersBuildGUI"}

    set f $parentFrame

    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3

    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fVolume $f.fApply \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # fVolume frame
    #-------------------------------------------

    set f $parentFrame.fVolume

    DevAddFileBrowse $f Volume "VolFreeSurferReaders,FileName" "FreeSurfer File:\n(COR-*)" "VolFreeSurferReadersSetFileName" "" "\$Volume(DefaultDir)"  "Browse for a FreeSurfer file" 

    frame $f.fDesc     -bg $Gui(activeWorkspace)

    frame $f.fName -bg $Gui(activeWorkspace)

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



    #-------------------------------------------
    # Apply frame
    #-------------------------------------------
    set f $parentFrame.fApply
        
    DevAddButton $f.bApply "Apply" "VolFreeSurferReadersApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

}
#-------------------------------------------------------------------------------
proc VolFreeSurferReadersEnter {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolFreeSurferReaders ENTER"}
}

#-------------------------------------------------------------------------------
proc VolFreeSurferReadersExit {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc VolFreeSurferReaders EXIT"}
}

#-------------------------------------------------------------------------------
proc VolFreeSurferReadersSetFileName {} {
    global Volume

    puts "FreeSurferReaders filename: $Volume(VolFreeSurferReaders,FileName)"

}
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
    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    # NOTE:
    # this normally happens in MainVolumes.tcl
    # this is needed here to set up reading
    # this should be fixed - the node should handle this somehow
    # so that MRML can be read in with just a node and this will
    # work
    MainVolumesCreate $i

    # read in the COR file
    # Set up the reading
    vtkCORReader Volume($i,vol,rw)
    
    # get the directory name from the filename
    Volume($i,vol,rw) SetFilePrefix [file dirname $Volume(VolFreeSurferReaders,FileName)]
    puts "VolFreeSurferReadersApply: set COR prefix to [Volume($i,vol,rw) GetFilePrefix]"


    if  {0} {
        # old curve file stuff
        # Set up the reading
        vtkFSSurfaceReader  Volume($i,vol,rw)
        # get the directory name from the filename
        Volume($i,vol,rw) SetFileName $Volume(VolFreeSurferReaders,FileName)
        puts "VolFreeSurferReadersApply: set surface filename to [Volume($i,vol,rw) GetFileName]"

        Volume(VolFreeSurferReaders,curvemapper) SetInput [Volume($i,vol,rw) GetOutput]

        Volume(VolFreeSurferReaders,curveactor) SetVisibility 1
    }

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
    # Reads in the volume via the Execute procedure
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "VolFreeSurferReaders: failed to read in the volume $i, $Volume(VolFreeSurferReaders,FileName)"
        return
    }
    # allow use of other module GUIs
    set Volume(freeze) 0
    
    # set the volume properties
    set Volume(pixelWidth) 1
    set Volume(pixelHeight) 1
    set Volume(sliceSpacing) 1 
    set Volume(sliceThickness) 1
    set Volume(gantryDetectorTilt) 0

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

    # display the new volume in the background of all slices
    MainSlicesSetVolumeAll Back $i


    # Update all fields that the user changed (not stuff that would 
    # need a file reread)
    
    Volume($m,node) SetName $Volume(name)
    Volume($m,node) SetDescription $Volume(desc)
    eval Volume($m,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($m,node) SetTilt $Volume(gantryDetectorTilt)
 
    # Update pipeline
    MainVolumesUpdate $m

    # Update MRML: this reads in new volumes, among other things
    MainUpdateMRML  
}

proc VolFreeSurferReadersBuildVTK {} {
    global viewRen Volume 

    puts "\nVolFreeSurferReadersBuildVTK\n"

    vtkPolyDataMapper Volume(VolFreeSurferReaders,curvemapper)
    # Volume(VolFreeSurferReaders,curvemapper) SetInput [Volume($i,vol,rw) GetOutput]
    vtkActor Volume(VolFreeSurferReaders,curveactor)
    Volume(VolFreeSurferReaders,curveactor) SetMapper Volume(VolFreeSurferReaders,curvemapper)
    viewRen AddActor Volume(VolFreeSurferReaders,curveactor)
    Volume(VolFreeSurferReaders,curveactor) SetVisibility 0
}
