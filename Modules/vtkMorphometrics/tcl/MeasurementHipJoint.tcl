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
#    MorphometricsHipJointPreplaceAxis Axis PolyData
#    MorphometricsHipJointPlaceHeadSphere
#    MorphometricsHipJointPlaceShaftAxis
#    MorphometricsHipJointResultEnter
#    MorphometricsHipJointResultExit
#    MorphometricsHipJointResultUserInterface frame
#    MorphometricsHipJointDisplayInit
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

    # step : the user has to specify the plane at the border of the head and neck
    MorphometricsCreatePlanePlacementStep MorphometricsHipJoint [Femur GetHeadNeckPlane] "Head Neck Border" "Place plane between the head and the neck"

    # step : the user has to specify the plane at the border of the neck and shaft
    MorphometricsCreatePlanePlacementStep MorphometricsHipJoint [Femur GetNeckShaftPlane] "Neck Shaft Border" "Place plane between the neck and the shaft" 

    # step : the user has to specify the upper border of the shaft
    MorphometricsCreatePlanePlacementStep MorphometricsHipJoint [Femur GetUpperShaftEndPlane] "upper end of shaft" "Place plane at the upper end of the shaft at the tuberculum minor"

    # step : the user has to specify the lower border of the shaft
    MorphometricsCreatePlanePlacementStep MorphometricsHipJoint  [Femur GetLowerShaftEndPlane] "lower end of shaft" "Place plane at the lower end of the shaft at the tuberculum minor" 

    # step : the user can set the neck axis manually
    MorphometricsCreateAxisPlacementStep MorphometricsHipJoint [Femur GetNeckAxis] "Neck axis" "neck axis"

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
    # BUG: ignores whether $Model([lindex $p 0],polyData actually exists
    Femur SetFemur $Model([lindex $idList 0],polyData)
    Pelvis SetPelvis $Model([lindex $idList 1],polyData)
    ShaftFilter SetInput [Femur GetFemur]
    HeadFilter SetInput [Femur GetFemur]
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
# .PROC MorphometricsHipJointPreplaceAxis
# Computes the best fitting line through the the given vtkPolyData object. 
# The TransformFilter is assumed to has as input a vtkCylinderSource, the transform 
# filter is therefore updated so that the output of it is the visualization of 
# the best fitting line.
# .ARGS
# str Axis an object of type vtkAxisSource
# str PolyData an object of type vtkPolyData
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointPreplaceAxis {Axis PolyData} {
    global MorphometricsHipJointEuclideanLineFit 

    MorphometricsHipJointEuclideanLineFit SetInput $PolyData
    MorphometricsHipJointEuclideanLineFit Update

    set normal [MorphometricsHipJointEuclideanLineFit GetDirection]
    set center [MorphometricsHipJointEuclideanLineFit GetCenter]

    $Axis SetCenter [lindex $center 0] [lindex $center 1] [lindex $center 2]
    $Axis SetDirection [lindex $normal 0] [lindex $normal 1] [lindex $normal 2]
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointPlaceHeadSphere
# Convenience function. Applies the sphere fitting algorithm to all points
# currently considered to be part of the head sphere. It also saves the result
# in the femur geometry.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointPlaceHeadSphere {} {
    global Femur HeadFilter MorphometricsHipJointSphereFit

    MorphometricsHipJointSphereFit SetInput [HeadFilter GetOutput]
    MorphometricsHipJointSphereFit Update

    [Femur GetHeadSphere] SetRadius [MorphometricsHipJointSphereFit GetRadius]
    set center [MorphometricsHipJointSphereFit GetCenter]
    [Femur GetHeadSphere] SetCenter [lindex $center 0] [lindex $center 1] [lindex $center 2]
}

#-------------------------------------------------------------------------------
# .PROC MorphometricsHipJointPlaceShaftAxis
# Convenience function. Applies the line fitting algorithm to all points
# in the shaft and saves the result in the femur geometry
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsHipJointPlaceShaftAxis {} {
    MorphometricsHipJointPreplaceAxis [Femur GetShaftAxis] [ShaftFilter GetOutput]
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
    viewRen AddActor MorphometricsHipJointResultNeckAxis
    viewRen AddActor AcetabularPlane

    Render3D

    # compute and display the shaft axis
    MorphometricsHipJointPlaceShaftAxis
    viewRen AddActor MorphometricsHipJointResultShaftAxis
    Render3D

    # compute and display the femoral head sphere
    MorphometricsHipJointPlaceHeadSphere
    viewRen AddActor MorphometricsHipJointResultHeadSphere
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


    vtkPredicateFilter ShaftFilter
    vtkHalfspacePredicate h3
    vtkHalfspacePredicate h4
    h3 SetPlane [Femur GetUpperShaftEndPlane]
    h4 SetPlane [Femur GetLowerShaftEndPlane]

    vtkAndPredicate and2
    and2 SetLeftOperand h3
    and2 SetRightOperand h4
    
    ShaftFilter SetPredicate and2

    vtkPredicateFilter HeadFilter
    vtkHalfspacePredicate h1
    h1 SetPlane [Femur GetHeadNeckPlane]
    HeadFilter SetPredicate h1
}

