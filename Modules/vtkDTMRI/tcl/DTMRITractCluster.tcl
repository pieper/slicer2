
proc DTMRITractClusterInit {} {
    global DTMRI 

    set DTMRI(TractCluster,NumberOfClusters) 3
    set DTMRI(TractCluster,Sigma) 25
    set DTMRI(TractCluster,HausdorffN) 20
    set DTMRI(TractCluster,ShapeFeature) MeanAndCovariance
    set DTMRI(TractCluster,ShapeFeature,menu) {MeanAndCovariance Hausdorff}
    set DTMRI(TractCluster,EmbeddingNormalization) RowSum
    set DTMRI(TractCluster,EmbeddingNormalization,menu) {RowSum LengthOne}

    set DTMRI(TractCluster,SettingsList,Names) {{Number of Clusters} Sigma N ShapeFeature EmbedNormalization}
    set DTMRI(TractCluster,SettingsList,Variables) {NumberOfClusters Sigma HausdorffN ShapeFeature EmbeddingNormalization}
    set DTMRI(TractCluster,SettingsList,VariableTypes) {entry entry entry menu menu}
    set DTMRI(TractCluster,SettingsList,Tooltips) {{Number of clusters (colors) when grouping tracts} {Similarity/distance tradeoff} {For Hausdorff shape feature, use every Nth point on the tract in computation.} {How to measure tract similarity} {How to normalize the vectors used in clustering}}

    # for viewing matrices
    vtkImageMagnify DTMRI(TractCluster,vtk,imageMagnify)
    DTMRI(TractCluster,vtk,imageMagnify) InterpolateOff
    vtkImageMathematics DTMRI(TractCluster,vtk,imageMultiply)
    DTMRI(TractCluster,vtk,imageMultiply) SetOperationToMultiplyByK
    DTMRI(TractCluster,vtk,imageMultiply) SetConstantK 100
    set DTMRI(TractCluster,zoom) 3
}


proc DTMRITractClusterBuildClusterFrame {} {

    global Gui Module Volume Tensor DTMRI Matrix
    
    #-------------------------------------------
    # TractCluster frame
    #-------------------------------------------
    set fCluster $Module(DTMRI,fTC)
    set f $fCluster

    foreach frame "Top Bottom" {
        frame $f.f$frame -bg  $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # TractCluster->Top frame
    #-------------------------------------------

    set f $fCluster.fTop
    
    frame $f.fLabel -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fLabel -side top -padx $Gui(pad) -pady $Gui(pad) -fill x


    #-------------------------------------------
    # TractCluster->Top->Label frame
    #-------------------------------------------
    set f $fCluster.fTop.fLabel

    DevAddLabel $f.lInfo "Tract Clustering and Coloring"
    eval {$f.lInfo configure} $Gui(BLA)
    pack $f.lInfo -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # TractCluster->Bottom frame
    #-------------------------------------------

    set f $fCluster.fBottom

    frame $f.fSettings -bg  $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fSettings -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "Apply Save" {
        frame $f.f$frame -bg  $Gui(activeWorkspace) 
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # TractCluster->Bottom->Settings frame
    #-------------------------------------------

    foreach var $DTMRI(TractCluster,SettingsList,Variables) \
        tip $DTMRI(TractCluster,SettingsList,Tooltips) \
        text $DTMRI(TractCluster,SettingsList,Names) \
        type $DTMRI(TractCluster,SettingsList,VariableTypes) {

            set f $fCluster.fBottom.fSettings

            frame $f.f$var -bg  $Gui(activeWorkspace)
            pack $f.f$var -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

            set f $fCluster.fBottom.fSettings.f$var

            DevAddLabel $f.l$var $text
            pack $f.l$var -side left  -padx $Gui(pad) -pady 0

            if {$type == "entry"} {
                eval {entry $f.e$var -width 6 \
                          -textvariable DTMRI(TractCluster,$var)} $Gui(WEA)
                pack $f.e$var -side right -padx $Gui(pad) -pady 0
                TooltipAdd  $f.e$var $tip
            } elseif {$type == "menu"} {
                eval {menubutton $f.mb$var -text "$DTMRI(TractCluster,$var)" \
                          -relief raised -bd 2 -width 20 \
                          -menu $f.mb$var.m} $Gui(WMBA)
                eval {menu $f.mb$var.m} $Gui(WMA)
                pack $f.l$var $f.mb$var -side left -pady $Gui(pad) -padx $Gui(pad)

                # save menubutton for config
                set DTMRI(TractCluster,gui,mb$var) $f.mb$var
                # Add a tooltip
                TooltipAdd $f.mb$var $tip

                # add menu items
                foreach item $DTMRI(TractCluster,$var,menu) {
                    $f.mb$var.m add command \
                        -label $item \
                        -command "set DTMRI(TractCluster,$var) $item; \
                    $f.mb$var config -text $item"
                }
            }
        }

    #-------------------------------------------
    # TractCluster->Bottom->Apply frame
    #-------------------------------------------

    set f $fCluster.fBottom.fApply
    DevAddButton $f.b "Cluster" {DTMRITractClusterComputeClusters}
    pack $f.b -side top -padx $Gui(pad) -pady $Gui(pad)

    TooltipAdd  $f.b "Apply above settings and cluster tracts.\nEach cluster will get a unique color."


    #-------------------------------------------
    # TractCluster->Bottom->Save frame
    #-------------------------------------------
    set f $fCluster.fBottom.fSave
    DevAddButton $f.bSave "Save clusters" \
        {DTMRIractClusterSaveTractClusters}

    TooltipAdd $f.bSave "Save text files with the tract paths, FA, and class lebals.\n This does not save vtk models, please save those under Display->Tracts->SaveTracts."

    pack $f.bSave -side top -padx $Gui(pad) -pady $Gui(pad) 

}

proc DTMRITractClusterApplyUserSettings {} {
    global DTMRI 

    set clusterer [DTMRI(vtk,streamlineControl) GetTractClusterer]
    set classifier [$clusterer GetNormalizedCuts]
    set features [$clusterer GetTractShapeFeatures]

    #$features DebugOn

    # Apply all variables from the GUI into the objects
    $features SetSigma $DTMRI(TractCluster,Sigma)
    $features SetHausdorffN $DTMRI(TractCluster,HausdorffN)
    $classifier SetNumberOfClusters $DTMRI(TractCluster,NumberOfClusters)
    $features SetFeatureTypeTo$DTMRI(TractCluster,ShapeFeature)
    $classifier SetEmbeddingNormalizationTo$DTMRI(TractCluster,EmbeddingNormalization)
}

proc DTMRITractClusterComputeClusters {} {
    global DTMRI 

    DTMRITractClusterApplyUserSettings

    puts "[DTMRI(vtk,streamlineControl) GetNumberOfStreamlines] streamlines"
    puts "Calling ComputeStreamlineFeatures"
    #DTMRI(vtk,streamlineControl) ClusterTracts
    DTMRI(vtk,streamlineControl) ClusterTracts 1
    puts "ComputeStreamlineFeatures Done"

    Render3D

}

proc DTMRITractClusterBuildMatrixViewer {} {
    global DTMRI Gui

    set w .dtmritractcluster1

    # if already created, raise and return
    if {[winfo exists $w] != 0} {
        raise $w
        # find the name of the viewer to set its input later
        #set tmp [$DTMRI(TractCluster,vtkTkImageViewerWidget) configure -iv]
        #set DTMRI(TractCluster,vtk,viewer) [lindex $tmp end]
        return
    }

    toplevel $w
    wm title $w "Display Matrices"

    # Create the vtkTkImageViewerWidget

    frame $w.f1 

    set dim 50
    set DTMRI(TractCluster,vtkTkImageViewerWidget) \
        [vtkTkImageViewerWidget $w.f1.r1 \
             -width $dim  -height $dim ]

    set widget $DTMRI(TractCluster,vtkTkImageViewerWidget) 

    # Set up some Tk bindings, a generic renwin interactor and VTK observers 
    # for that widget

    ::vtk::bind_tk_imageviewer_widget \
        $widget 

    set iren [[[$widget GetImageViewer] GetRenderWindow] GetInteractor]
    # Add our PickEvent 
    set istyle [$iren GetInteractorStyle]
    #$istyle RemoveObservers PickEvent
    $istyle AddObserver PickEvent \
        [list DTMRITractClusterSelect $widget]
    # add our bindings
    #bind $w.f1 <ButtonPress> {DTMRITractClusterSelect %x %y}

    # find the name of the viewer to set its input later
    set tmp [$widget configure -iv]
    set DTMRI(TractCluster,vtk,viewer) [lindex $tmp end]

    # Set the window manager (wm command) so that it registers a
    # command to handle the WM_DELETE_WINDOW protocal request. This
    # request is triggered when the widget is closed using the standard
    # window manager icons or buttons. In this case the exit callback
    # will be called and it will free objects associated with the window.

    wm protocol $w WM_DELETE_WINDOW "$DTMRI(TractCluster,vtk,viewer) Delete; destroy $w"

    #    bind $widget \
        #        <ButtonPress> "puts %x"
    #        ButtonPress "DTMRITractClusterSelect %x %y"

    pack $widget \
        -side left -anchor n \
        -padx 3 -pady 3 \
        -fill x -expand f

    pack $w.f1 \
        -fill both -expand t

    # create a menu for changing viewer's input
    set f $w.f1
    eval {label $f.lVis -text "Matrix: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text "Choose..." \
              -relief raised -bd 2 -width 15 \
              -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady $Gui(pad) -padx $Gui(pad)

    # save menubutton for config
    set DTMRI(TractCluster,gui,mbMatrixViewer) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis "Select the tract-comparison matrix to view."

    # add a label area below the menu
    eval {label $f.lMat -text ""} $Gui(WLA)
    pack $f.lMat  -side bottom -pady $Gui(pad) -padx $Gui(pad)
    # save label for config
    set DTMRI(TractCluster,gui,lMatrixInfo) $f.lMat
}

proc DTMRITractClusterSelect {widget} {
    global DTMRI 

    set pos [[[[$widget GetImageViewer] GetRenderWindow] GetInteractor] GetEventPosition]
    set x [lindex $pos 0]
    set y [lindex $pos 1]
    puts "$x $y"

    # y is the row of interest. 
    # select the tract that corresponds to this row.
    # flip y axis, un-zoom, check bounds
    set maxY [lindex [lindex [$widget configure ] 0] end]
    
    if {$y < 0 | $y > $maxY} {
        return
    }
    set y [expr $maxY-$y]

    set y [expr $y/$DTMRI(TractCluster,zoom)]

    # now make that tract highlight yellow twice
    set tracts [DTMRI(vtk,streamlineControl) GetActors]
    set actor [$tracts GetItemAsObject $y]
    if {$actor != ""} {

        set color [[$actor GetProperty] GetColor]
        [$actor GetProperty] SetColor 255 255 0
        Render3D
        eval {[$actor GetProperty] SetColor} $color
        Render3D
        [$actor GetProperty] SetColor 255 255 0
        Render3D
        eval {[$actor GetProperty] SetColor} $color
        Render3D
    }
}


proc DTMRITractClusterAdvancedDisplayMatrix {input getOutput name} {
    global DTMRI 

    set imageData [$input $getOutput]

    DTMRI(TractCluster,vtk,imageMultiply) SetInput1 $imageData

    DTMRI(TractCluster,vtk,imageMagnify) SetMagnificationFactors \
        $DTMRI(TractCluster,zoom) $DTMRI(TractCluster,zoom) 1
    DTMRI(TractCluster,vtk,imageMagnify) SetInput \
        [DTMRI(TractCluster,vtk,imageMultiply) GetOutput]

    # update so scalar range is right below
    DTMRI(TractCluster,vtk,imageMagnify) Update     

    $DTMRI(TractCluster,vtk,viewer) SetInput \
        [DTMRI(TractCluster,vtk,imageMagnify) GetOutput]
    ${DTMRI(TractCluster,gui,mbMatrixViewer)} config -text $name

    set range [[DTMRI(TractCluster,vtk,imageMagnify) GetOutput] GetScalarRange]
    $DTMRI(TractCluster,gui,lMatrixInfo) config -text $range

    set len [expr [lindex $range 1] - [lindex $range 0]]
    set l [expr [lindex $range 0] + $len/2]
    set w [expr $len + 1]
    $DTMRI(TractCluster,vtk,viewer) SetColorWindow $w
    $DTMRI(TractCluster,vtk,viewer) SetColorLevel $l
    #puts "$w $l"

    # configure widgets to current data
    set dim [DTMRI(vtk,streamlineControl) GetNumberOfStreamlines]
    set dim [expr $dim*$DTMRI(TractCluster,zoom)]
    $DTMRI(TractCluster,vtkTkImageViewerWidget) configure \
        -width $dim -height $dim

    # highlight the last picked streamline
    puts "Active streamline $DTMRI(activeStreamlineID)"

    $DTMRI(TractCluster,vtk,viewer) Render
}

proc DTMRITractClusterAdvancedViewMatrices {} {
    global DTMRI 

    # show/build pop-up window to view output images from computation
    DTMRITractClusterBuildMatrixViewer

    # Add menu items for viewing various images
    set clusterer [DTMRI(vtk,streamlineControl) GetTractClusterer]
    set classifier [$clusterer GetNormalizedCuts]
    set features [$clusterer GetTractShapeFeatures]

    set inputList "$features $features $classifier $classifier"
    set getOutputList "GetInterTractDistanceMatrixImage GetInterTractSimilarityMatrixImage GetNormalizedWeightMatrixImage GetEigenvectorsImage"
    set inputNames "Distance Weights NormWeights Eigenvectors"

    foreach input $inputList name $inputNames get $getOutputList {
        ${DTMRI(TractCluster,gui,mbMatrixViewer)}.m add command \
            -label $name \
            -command "DTMRITractClusterAdvancedDisplayMatrix $input $get $name"

    }

}


#-------------------------------------------------------------------------------
# .PROC DTMRIractClusterSaveTractClusters
# Save all points from the streamline paths as text files
# .ARGS
# int verbose default is 1 
# .END
#-------------------------------------------------------------------------------
proc DTMRIractClusterSaveTractClusters {{verbose "1"}} {
    
    # check we have streamlines
    if {[DTMRI(vtk,streamlineControl) GetNumberOfStreamlines] < 1} {
        set msg "There are no tracts to save. Please create tracts first."
        tk_messageBox -message $msg
        return
    }

    # set base filename for all stored files
    set filename [tk_getSaveFile  -title "Save Tracts: Choose Initial Filename"]
    if { $filename == "" } {
        return
    }

    # save the tracts
    DTMRI(vtk,streamlineControl) SaveTractClustersAsTextFiles \
        $filename 

    # let user know something happened
    if {$verbose == "1"} {
        set msg "Finished writing tracts. The filenames are: $filename*.*"
        tk_messageBox -message $msg
    }

} 

proc DTMRITractClusterTest {} {
    global DTMRI Label Tensor Volume

    set t $Tensor(activeID)
    set v $Volume(activeID)

    # make sure they are using a segmentation (labelmap)
    if {[Volume($v,node) GetLabelMap] != 1} {
        set name [Volume($v,node) GetName]
        set msg "The volume $name is not a label map (segmented ROI). Continue anyway?"
        if {[tk_messageBox -type yesno -message $msg] == "no"} {
            return
        }

    }

    # ask for user confirmation first
    set name [Volume($v,node) GetName]
    set msg "About to seed streamlines in all labelled voxels of volume $name.  This may take a while, so make sure the Tracts settings are what you want first. Go ahead?"
    if {[tk_messageBox -type yesno -message $msg] == "no"} {
        return
    }

    # set mode to On (the Display Tracts button will go On)
    set DTMRI(mode,visualizationType,tractsOn) On

    # make sure the settings are current
    DTMRIUpdateTractColor
    DTMRIUpdateStreamlineSettings
    
    # set up the input segmented volume
    DTMRI(vtk,streamlineControl) SetInputROI [Volume($v,vol) GetOutput] 
    DTMRI(vtk,streamlineControl) SetInputROIValue $Label(label)

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    vtkTransform transform
    transform SetMatrix [Volume($v,node) GetWldToIjk]
    # now it's ijk to world
    transform Inverse
    DTMRI(vtk,streamlineControl) SetROIToWorld transform
    transform Delete

    # Apply the settings for number of clusters, etc.
    DTMRITractClusterApplyUserSettings

    # create all streamlines
    puts "Original number of tracts: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
    DTMRI(vtk,streamlineControl) SeedStreamlinesFromROIClusterAndDisplay
    puts "New number of tracts is: [[DTMRI(vtk,streamlineControl) GetStreamlines] GetNumberOfItems]"
}
