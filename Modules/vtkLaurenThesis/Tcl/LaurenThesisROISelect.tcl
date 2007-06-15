
proc LaurenThesisROISelectInit {} {

    global LaurenThesis Volume
    
    # labelmap ID numbers
    set LaurenThesis(vROIA) -1
    set LaurenThesis(vROIB) -1

    # directory where the vtk tract models are
    set LaurenThesis(clusterDirectory) ""
    set LaurenThesis(ROISelectOutputDirectory) ""
    set LaurenThesis(outputFilename) "ClusterROISelect.vtk"
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
    foreach frame "ROIA ROIB Directory OutputDirectory" {
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
    # ROISelect->Middle->Output Directory frame
    #-------------------------------------------
    set f $fROISelect.fMiddle.fOutputDirectory

    eval {button $f.b -text "Output directory:" -width 16 \
        -command "LaurenThesisSelectOutputDirectory"} $Gui(WBA)
    eval {entry $f.e -textvariable LaurenThesis(ROISelectOutputDirectory) -width 51} $Gui(WEA)
    bind $f.e <Return> {LaurenThesisSelectOutputDirectory}
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

proc LaurenThesisSelectOutputDirectory {} {
    global LaurenThesis

    set dir $LaurenThesis(clusterDirectory)

    if {[catch {set filename [tk_chooseDirectory -title "Output Directory" \
                                  -initialdir "$dir"]} errMsg] == 1} {
        DevErrorWindow "LaurenThesisSelectOutputDirectory: error selecting output directory:\n$errMsg"
        return ""
    }

    set LaurenThesis(ROISelectOutputDirectory) $filename
}

proc LaurenThesisROISelectUpdateMRML {} {

    global LaurenThesis

    # Update volume selection widgets if the MRML tree has changed

    DevUpdateNodeSelectButton Volume LaurenThesis vROIA vROIA \
        DevSelectNode 0 0 1 

    DevUpdateNodeSelectButton Volume LaurenThesis vROIB vROIB \
        DevSelectNode 0 0 1 


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

    global LaurenThesis

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

    catch {appender Delete}
    vtkAppendPolyData appender

    # go through all model filenames
    foreach model $models {
        puts $model

        # read in the model as polydata
        catch {_reader Delete}
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
    catch {_writer Delete}
    vtkPolyDataWriter _writer
    # get output from the vtkAppendPolyData
    _writer SetInput [appender GetOutput]
    _writer SetFileName $LaurenThesis(ROISelectOutputDirectory)/$LaurenThesis(outputFilename)
    _writer Write
    
    _writer Delete
    appender Delete
}

# arguments: ID number of the labelmaps and the polydata object
# return 1 when model passes the test, otherwise return 0
proc LaurenThesisTestTractIntersectsROI {vROIA vROIB polyData } {
    global Model Tensor Module
    
    catch {_probe Delete}
    vtkProbeFilter _probe
    
    _probe SetSource [Volume($vROIA,vol) GetOutput]
    
    # transform model into IJK of data
    # This assumes the model is already aligned
    # with the tensors in the world coordinate system.
    catch {_transform Delete}
    vtkTransform _transform
    #_transform PreMultiply
    _transform SetMatrix [Volume($vROIA,node) GetWldToIjk]
    # remove scaling from matrix
    # invert it to give ijk->ras, so we can scale with i,j,k spacing
    _transform Inverse
    scan [Volume($vROIA,node) GetSpacing] "%g %g %g" res_x res_y res_z
    _transform Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
        [expr 1.0 / $res_z]
    _transform Inverse
    
    catch {_transformPD Delete}
    vtkTransformPolyDataFilter _transformPD
    _transformPD SetTransform _transform
    _transformPD SetInput $polyData
    _transformPD Update

    # probe with model in IJK
    _probe SetInput [_transformPD GetOutput]
    _probe Update
    
    set pd [_probe GetOutput] 
    #puts "Number of points for this tract:[$pd GetNumberOfPoints]"
    
    set scalars [[$pd GetPointData] GetScalars]
    set scalar_range [[[$pd GetPointData] GetScalars] GetRange]
    #puts "Range: $scalar_range"

    _probe Delete
    _transform Delete
    _transformPD Delete
    
    # repeat for ROIB
    catch {_probe2 Delete}
    vtkProbeFilter _probe2
    
    _probe2 SetSource [Volume($vROIB,vol) GetOutput]
    catch {_transform2 Delete}
    vtkTransform _transform2
    _transform2 SetMatrix [Volume($vROIB,node) GetWldToIjk]
    _transform2 Inverse
    scan [Volume($vROIB,node) GetSpacing] "%g %g %g" res_x res_y res_z
    _transform2 Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
        [expr 1.0 / $res_z]
    _transform2 Inverse
    
    catch {_transformPD2 Delete}
    vtkTransformPolyDataFilter _transformPD2
    _transformPD2 SetTransform _transform2
    _transformPD2 SetInput $polyData
    _transformPD2 Update
    
    _probe2 SetInput  [_transformPD2 GetOutput]
    _probe2 Update
    
    set pd2 [_probe2 GetOutput] 
    #puts "Number of points for this tract:[$pd2 GetNumberOfPoints]"

    set scalar_range2 [[[$pd2 GetPointData] GetScalars] GetRange]
    #puts "Range for ROIB: $scalar_range2"
  
    _probe2 Delete
    _transform2 Delete
    _transformPD2 Delete
    
    if { ([lindex $scalar_range 1] != 0) & ([lindex $scalar_range2 1] != 0) } {
        return 1
    } else {
        return 0
    }
}

