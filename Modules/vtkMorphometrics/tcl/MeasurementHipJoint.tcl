#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        MeasurementHipJoint.tcl
#    MorphometricsHipJointMeasurementWrapper
#    MorphometricsHipJointMeasurementInit
#    MorphometricsHipJointSetModels idList
#    MorphometricsHipJointMeasurementInitGeometry
#    MorphometricsHipJointPlaceHeadSphere
#    MorphometricsHipJointResultEnter
#    MorphometricsHipJointResultExit
#    MorphometricsHipJointResultUserInterface frame
#    MorphometricsHipJointDisplayInit
#    MorphometricsHipJointCreateModel polydata name
#===============================================================================

# add the module to the list of tools 
lappend Morphometrics(measurementInitTools) MorphometricsHipJointMeasurementWrapper

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointMeasurementWrapper
# Necessary wrapper function 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointMeasurementWrapper {} {
    MorphometricsAddMeasurement "Hip Joint" MorphometricsHipJoint "head sphere, neck and shaft axis, neck shaft angle, acetabular plane, anteversion and inclination of acetabulum" MorphometricsHipJointMeasurementInit
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointMeasurementInit
# Creates all vtk objects and also constructs the workflow
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointMeasurementInit {} {
    global Morphometrics

    # initialize all vtkObjects which contain all basic and derived objects describing the geometry
    MorphometricsHipJointMeasurementInitGeometry

    # initialize all vtkObjects necessary for viewing as well as computing 
    # the results and the necessary user interactions
    MorphometricsHipJointDisplayInit

    # create the workflow
    WorkflowInitWorkflow MorphometricsHipJoint $Morphometrics(workflowFrame)

    # step : the user has to specify which model is the femur and which is the pelvis
    MorphometricsCreateModelChooserStep MorphometricsHipJoint {Femur Pelvis} MorphometricsHipJointSetModels

    # step : the user can set the shaft axis manually
    MorphometricsCreateAxisPlacementStep MorphometricsHipJoint [Femur GetShaftAxis] "Shaft axis" "shaft axis"
    
    # step : the user can set the neck axis manually
    MorphometricsCreateAxisPlacementStep MorphometricsHipJoint [Femur GetNeckAxis] "Neck axis" "neck axis"

    # step : the user can set the head sphere manually
    MorphometricsCreateSpherePlacementStep MorphometricsHipJoint [Femur GetHeadSphere] "Head sphere" "head sphere"

    # step : the user specifies the acetabular plane
    MorphometricsCreatePlanePlacementStep MorphometricsHipJoint  [Pelvis GetAcetabularPlane] "Acetabular Plane" "Place the acetabular plane"

    # step : the user specifies the coronal plane
    MorphometricsCreateAxisPlacementStep MorphometricsHipJoint [Pelvis GetFrontalAxis] "Frontal Axis" "Place the frontal axis of the pelvis"

    # step : display the results
    WorkflowAddStep MorphometricsHipJoint MorphometricsHipJointResultEnter MorphometricsHipJointResultExit  MorphometricsHipJointResultUserInterface "Results"
}


#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointSetModels
# sets the models representing the femur and pelvis according to user choices.
# Required function for MorphometricsCreateModelChooserStep.
# .ARGS
# list idList list of user choices for the necessary models femur and pelvis.
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointSetModels {idList} {
    global Model 
    set femurMTime [Femur GetMTime]
    set pelvisMTime [Pelvis GetMTime]
    Femur SetFemur $Model([lindex $idList 0],polyData)
    Pelvis SetPelvis $Model([lindex $idList 1],polyData)
    
    if {[expr $femurMTime != [Femur GetMTime] || $pelvisMTime != [Pelvis GetMTime]]} {
    MorphometricsHipJointPrecomputeGeometry
    }
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointPrecomputeGeometry
# Invokes the algorithms for automatic placement of the relevant geometry part
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointPrecomputeGeometry {} {
    PelvisConvexHull SetInput [Pelvis GetPelvis]
    PelvisConvexHull Update
    vDP1 SetHull PelvisConvexHull
    vDP1 SetMaximalDistance 2000
    vPF1 SetPredicate vDP1
    vPF1 SetInput [Femur GetFemur]
    vPF1 Update

    JointSphere SetInput [vPF1 GetOutput]
    JointSphere Update

    [Femur GetHeadSphere] SetRadius [JointSphere GetRadius]

    set center [JointSphere GetCenter]
    puts "new Radius  [JointSphere GetRadius], new center  [lindex $center 0] [lindex $center 1] [lindex $center 2]"
    [Femur GetHeadSphere] SetCenter [lindex $center 0] [lindex $center 1] [lindex $center 2]
    Femur Precompute
    PelvisPrecompute
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointInitGeometry
# Creates the variables representing the hip joint.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointMeasurementInitGeometry {} {
    vtkFemurMetric Femur
    vtkPelvisMetric Pelvis
}


#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointResultEnter
# Displays and computes some parts of the hip joint geometry needed for evaluating
# the result.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointResultEnter {} {
    global MorphometricsHipJointResultShaftAxis MorphometricsHipJointResultNeckAxis MorphometricsHipJointResultHeadSphere AcetabularPlane

    # display the defined parts of the femur and the hip joint geometry parts specified by the user
    viewRen AddActor MorphometricsHipJointResultShaftAxis
    viewRen AddActor MorphometricsHipJointResultNeckAxis
    viewRen AddActor MorphometricsHipJointResultHeadSphere
    viewRen AddActor AcetabularPlane
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointResultExit
# Undisplays the hip joint geometry.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointResultExit {} {
    global MorphometricsHipJointResultShaftAxis MorphometricsHipJointResultNeckAxis MorphometricsHipJointResultHeadSphere AcetabularPlane
    viewRen RemoveActor MorphometricsHipJointResultShaftAxis
    viewRen RemoveActor MorphometricsHipJointResultNeckAxis
    viewRen RemoveActor MorphometricsHipJointResultHeadSphere
    viewRen RemoveActor AcetabularPlane

    Render3D
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointResultUserInterface
# User interface presenting (and computing) the derived angles for the hip joint geometry.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointResultUserInterface {frame} {
    global Gui Femur

    Pelvis Normalize
    Femur Normalize
    Femur ComputeNeckShaftAngle

    DevAddLabel $frame.lneckshaftangle "NeckShaft Angle : [Femur GetNeckShaftAngle]"
    pack $frame.lneckshaftangle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    DevAddLabel $frame.lanteversionangle "Anteversion : [Pelvis GetAnteversionAngle]"
    pack $frame.lanteversionangle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    DevAddLabel $frame.linclinationangle "Inclination : [Pelvis GetInclinationAngle]"
    pack $frame.linclinationangle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    DevAddButton $frame.createModels "Create Result Models" [list eval [subst -nocommands {MorphometricsHipJointCreateResultModels; Render3D}]]
    pack $frame.createModels -side bottom -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointCreateResultsModels
# Creates a model hierarchy representing the results of the hip joint workflow. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointCreateResultModels {} {

    set idList {} 
    lappend idList [MorphometricsHipJointCreateModel [[Femur GetShaftAxis] GetOutput] "Shaft Axis"]
    lappend idList [MorphometricsHipJointCreateModel [[Femur GetNeckAxis] GetOutput] "Neck Axis"]
    lappend idList [MorphometricsHipJointCreateModel [[Femur GetHeadSphere] GetOutput] "Head Sphere"]
    lappend idList [MorphometricsHipJointCreateModel [[Pelvis GetAcetabularPlane] GetOutput] "Acetabular Plane"]

    # -> checking whether one exists
    ModelHierarchyCreate

    MainUpdateMRML
    # -> checking whether HipJointModels already exists
    ModelHierarchyCreateGroupOk HipJointModels
    MainUpdateMRML
    foreach resultModel $idList {
    ModelHierarchyMoveModel $resultModel HipJointModels 0
    MainUpdateMRML
    }
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointDisplayInit
# Setup all vtk objects needed for displaying and computing parts of the hip
# joint geometry.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointDisplayInit {} {
    global Femur Pelvis Model

    # fitting algorithms
    vtkEuclideanLineFit MorphometricsHipJointEuclideanLineFit
    vtkBooksteinSphereFit MorphometricsHipJointSphereFit

    vtkActor MorphometricsHipJointResultShaftAxis
    vtkActor MorphometricsHipJointResultNeckAxis
    vtkActor MorphometricsHipJointResultHeadSphere
    vtkActor AcetabularPlane

    vtkPolyDataMapper MorphometricsHipJointResultPDM1
    vtkPolyDataMapper MorphometricsHipJointResultPDM2
    vtkPolyDataMapper MorphometricsHipJointResultPDM3
    vtkPolyDataMapper MorphometricsHipJointResultPDM4

    MorphometricsHipJointResultShaftAxis SetMapper MorphometricsHipJointResultPDM1
    MorphometricsHipJointResultNeckAxis SetMapper MorphometricsHipJointResultPDM2
    MorphometricsHipJointResultHeadSphere SetMapper MorphometricsHipJointResultPDM3
    AcetabularPlane SetMapper MorphometricsHipJointResultPDM4


    MorphometricsHipJointResultPDM1 SetInput [[Femur GetShaftAxis] GetOutput]
    MorphometricsHipJointResultPDM2 SetInput [[Femur GetNeckAxis] GetOutput]
    MorphometricsHipJointResultPDM3 SetInput [[Femur GetHeadSphere] GetOutput]
    MorphometricsHipJointResultPDM4 SetInput [[Pelvis GetAcetabularPlane] GetOutput]


    vtkConvexHullInexact PelvisConvexHull
    vtkBooksteinSphereFit JointSphere
    vtkPredicateFilter vPF1
    vtkDistancePredicate vDP1

    # vtkObjects needed for computing the acetabular plane
    vtkAxisSource coneAxis
    vtkPredicateFilter coneHip
    vtkPredicateFilter borderJoint
    vtkDistanceSpherePredicate femoralHead
    vtkConePredicate coneAngle
    vtkPrincipalAxes vprince
    vtkAndPredicate andHipJoint
    vtkConvexHullInexact roiHip
    vtkDistancePredicate nearConvexHullHipJoint

}

proc PelvisPrecompute {} {
    vprince SetInput [Pelvis GetPelvis]
    vprince Update

    set maxAngle [MorphometricsHipJointPelvisFemurHeadAxis coneAxis 2]

    coneAngle SetMaximalAngle $maxAngle
    coneAngle SetAxis coneAxis
    coneHip SetPredicate coneAngle
    coneHip SetInput [Pelvis GetPelvis]
    coneHip Update

    roiHip SetInput [coneHip GetOutput]
    roiHip SetGranularity 3
    roiHip Update

    femoralHead SetSphere [Femur GetHeadSphere]
    femoralHead SetOnlyInside 0
    femoralHead SetMaximalDistance 6

    borderJoint SetPredicate andHipJoint
    andHipJoint SetLeftOperand femoralHead
    andHipJoint SetRightOperand nearConvexHullHipJoint

    nearConvexHullHipJoint SetHull roiHip
    nearConvexHullHipJoint SetMaximalDistance 4

    borderJoint SetInput [Pelvis GetPelvis]
    borderJoint Update

    vprince SetInput [borderJoint GetOutput]
    vprince Update

    set center [vprince GetCenter]
    set normal [vprince GetZAxis]
    [Pelvis GetAcetabularPlane] SetCenter [lindex $center 0] [lindex $center 1] [lindex $center 2]
    [Pelvis GetAcetabularPlane] SetNormal [lindex $normal 0] [lindex $normal 1] [lindex $normal 2]
}

proc MorphometricsHipJointPelvisFemurHeadAxis {Axis factor } {
    set pelvisCenter [vprince GetCenter]

    set femurCenter [[Femur GetHeadSphere] GetCenter]
    set femurRadius [expr $factor * [[Femur GetHeadSphere] GetRadius]]

    set direction {}
    lappend direction [expr [lindex $femurCenter 0] - [lindex $pelvisCenter 0]]
    lappend direction [expr [lindex $femurCenter 1] - [lindex $pelvisCenter 1]]
    lappend direction [expr [lindex $femurCenter 2] - [lindex $pelvisCenter 2]]

    $Axis SetCenter [lindex $pelvisCenter 0] [lindex $pelvisCenter 1] [lindex $pelvisCenter 2]
    $Axis SetDirection [lindex $direction 0] [lindex $direction 1] [lindex $direction 2]

    set length 0
    set length [expr $length + [lindex $direction 0] * [lindex $direction 0]]
    set length [expr $length + [lindex $direction 1] * [lindex $direction 1]]
    set length [expr $length + [lindex $direction 2] * [lindex $direction 2]]
    set length [expr sqrt($length)]
    set angle [expr acos($length / [expr sqrt($length * $length + $femurRadius * $femurRadius)])]
    return [expr $angle *  57.2957795131]
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointCreateModel
# Create a MRML node for $polydata with name $name. No fancy stuff is done, so 
# this could be in Base/tcl/tcl-main/ModelMaker.tcl as well. This function is 
# based upon the function for creating a model out of a segmentation.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointCreateModel {polydata name} {
    global Model Module ModelMaker Label

    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $name
    $n SetColor Brain

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    puts [MainModelsCreate $m]

    # Registration
    set v $ModelMaker(idVolume)
    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) $polydata
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }

    # put the model inside the same transform as the source volume
    set nitems [Mrml(dataTree) GetNumberOfItems]
    for {set midx 0} {$midx < $nitems} {incr midx} {
        if { [Mrml(dataTree) GetNthItem $midx] == "Model($m,node)" } {
            break
        }
    }
    if { $midx < $nitems } {
        Mrml(dataTree) RemoveItem $midx
        Mrml(dataTree) InsertAfterItem Volume($v,node) Model($m,node)
        MainUpdateMRML
    }

    MainUpdateMRML
    MainModelsSetActive $m
    return [$n GetID]
}
