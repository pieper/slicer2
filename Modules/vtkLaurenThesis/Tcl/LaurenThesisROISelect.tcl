
proc LaurenThesisROISelectInit {} {

    global LaurenThesis Volume
    
    # labelmap ID numbers
    set LaurenThesis(vROIA) -1
    set LaurenThesis(vROIB) -1

    # directory where the vtk tract models are
    set LaurenThesis(clusterDirectory) ""
}

proc LaurenThesisROISelectBuildGUI {} {

    global Gui LaurenThesis Module Volume

    #-------------------------------------------
    # ROISelect frame
    #-------------------------------------------
    set fROISelect $Module(LaurenThesis,fROISelect)
    set f $fROISelect
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # ROISelect->Top frame
    #-------------------------------------------
    set f $fROISelect.fTop
    DevAddLabel $f.lHelp "Select tracts based on ROIs."

    pack $f.lHelp -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROISelect->Middle frame
    #-------------------------------------------
    set f $fROISelect.fMiddle
    foreach frame "ROIA ROIB Directory" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # ROISelect->Middle->ROIA frame
    #-------------------------------------------
    set f $fROISelect.fMiddle.fROIA

    # menu to select a volume: will set LaurenThesis(vROI)
    # works with DevUpdateNodeSelectButton in UpdateMRML
    set name vROIA
    DevAddSelectButton  LaurenThesis $f $name "ROI A:" Pack \
        "ROIA" \
        25

    #-------------------------------------------
    # ROISelect->Middle->ROIB frame
    #-------------------------------------------
    set f $fROISelect.fMiddle.fROIB

    # menu to select a volume: will set LaurenThesis(vROI)
    # works with DevUpdateNodeSelectButton in UpdateMRML
    set name vROIB
    DevAddSelectButton  LaurenThesis $f $name "ROI B:" Pack \
        "ROIB" \
        25

    #-------------------------------------------
    # ROISelect->Middle->Directory frame
    #-------------------------------------------
    set f $fROISelect.fMiddle.fDirectory

    eval {button $f.b -text "Tract directory:" -width 16 \
        -command "LaurenThesisSelectDirectory"} $Gui(WBA)
    eval {entry $f.e -textvariable LaurenThesis(clusterDirectory) -width 51} $Gui(WEA)
    bind $f.e <Return> {LaurenThesisSelectDirectory}
    pack $f.b -side left -padx $Gui(pad)
    pack $f.e -side left -padx $Gui(pad) -fill x -expand 1

    #-------------------------------------------
    # ROISelect->Bottom frame
    #-------------------------------------------
    set f $fROISelect.fBottom

    DevAddButton $f.bApply "Apply" \
        LaurenThesisValidateParametersAndSelectTracts
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Sample tensors at all points in the tract cluster models. Save new models."

}

proc LaurenThesisSelectDirectory {} {
    global LaurenThesis

    set dir $LaurenThesis(clusterDirectory)

    if {[catch {set filename [tk_chooseDirectory -title "Cluster Directory" \
                                  -initialdir "$dir"]} errMsg] == 1} {
        DevErrorWindow "LaurenThesisSelectDirectory: error selecting cluster directory:\n$errMsg"
        return ""
    }

    set LaurenThesis(clusterDirectory) $filename
}


proc LaurenThesisROISelectUpdateMRML {} {

    global LaurenThesis

    # Update volume selection widgets if the MRML tree has changed

    DevUpdateNodeSelectButton Volume LaurenThesis vROIA vROIA \
        DevSelectNode 0 0 0 

    DevUpdateNodeSelectButton Volume LaurenThesis vROIB vROIB \
        DevSelectNode 0 0 0 


}

# this gets called when the user hits Apply
proc LaurenThesisValidateParametersAndSelectTracts {} {
    global LaurenThesis

    # check that the user entered parameters are okay
    # fill this in if needed
    puts $LaurenThesis(vROIA)
    puts $LaurenThesis(vROIB)
    
    # call the code that does something
    LaurenThesisSelectTracts $LaurenThesis(vROIA) $LaurenThesis(vROIB) $LaurenThesis(clusterDirectory)

}

# this actually reads in all the models in the directory
proc LaurenThesisSelectTracts {vROIA vROIB directory} {

    # Load all models in the directory of the form $pattern
    # for now look at all vtk models in the directory
    # perhaps make this match the case name
    set pattern "*_*.vtk"

    set models [lsort [glob -nocomplain -directory $directory $pattern]]

    # test we found models
    if {$models == ""} {
        puts "ERROR: No models with filenames *_*.vtk were found in the directory"
        return
    }


    vtkAppendPolyData appender

    # go through all model filenames
    foreach model $models {
        puts $model

        # read in the model as polydata
        vtkPolyDataReader _reader
        _reader SetFileName $model
        _reader Update

        # error check
        if {[_reader GetOutput] == "" } {
            puts "ERROR, file $model could not be read."
            _reader Delete
            return
        } else {
            puts " read model $model "
        }

        # call the code to see if this model intersects the ROI
        set intersectROI [LaurenThesisTestTractIntersectsROI \
                              $vROIA $vROIB [_reader GetOutput]]

        # if it passes
        # append to vtkAppendPolyData
        if { $intersectROI == 1} {
            appender AddInput [_reader GetOutput]
        }

        # delete the reader
        _reader Delete
    }

    # write out the tracts that passed the ROI test
    vtkPolyDataWriter _writer
    # get output from the vtkAppendPolyData
    _writer SetInput [appender GetOutput]
    _writer SetFileName test.vtk
    _writer Write
    
    _writer Delete
    appender Delete
}

# TO DO
# arguments: ID number of the labelmaps and the polydata object
proc LaurenThesisTestTractIntersectsROI {vROIA vROIB polyData } {

    global Model Tensor Module

    # do whatever Raul says with this polydata and these volumes

    #raul's filter SetInput $polyData
    # vtkImage data ROI:
    Volume($vROIA,vol) Print
    # node information (matrices RAS->IJK)
    Volume($vROIA,node) Print

    # for transformations info:
    # read code LaurenThesisProbeClusters.tcl
    # LaurenThesisProbeTensorWithPolyData

    # polydata is in RAS space already. image data is in IJK.

    # return whether the tract passed the test
    return 1
}

