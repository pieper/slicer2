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
# FILE:        Workflow.tcl
# PROCEDURES:  
#    IgnoreModelsChoosen ids
#    DescribeStepsUI frame
#    MorphometricsDeveloperMeasurementInit
#
#===============================================================================
# This file serves as an example of how to add a morphometric tool to the 
# vtkMorphometrics module. Essentially you don't have to change anything inside
# Morphometrics.tcl, instead add the following line to your file
#    lappend Morphometrics(measurementInitTools) <an_wrapper_function>
# and <an_wrapper_function> consists solely of a call to
#    MorphometricsAddMeasurement <short_name> <workflow_name> <description> <init_function>
# where
#   - <short_name> will be the entry for your tool in the list of available
#     morphometric tools
#   - <workflow_name> is the name of the workflow your tool uses
#   - <description> is a text explaining what the user achieves when using your tool
#   - <init_function> a function which actually builds the workflow and all needed
#     vtk objects.
#
# The separate <init_function> is necessary, otherwise all tools allocate all their needed
# memory during startup, which in turn is unnecessarily slowed down by this.
#
# You can't call MorphometricsAddMeasurement directly, since it has to be initialized
# first by MorphometricsInit and this function gets called after sourcing all .tcl files.
#
# Furthermore uses of StepFactories.tcl are found in this file.
#


#-------------------------------------------------------------------------------
# .PROC IgnoreModelsChoosen
# Prints the result of user choices to stdout
# .ARGS
# list ids the first entry is users choice for "Femur" and the second for "Pelvis"
# .END
#-------------------------------------------------------------------------------
proc IgnoreModelsChoosen {ids} {
    puts "User choice for Femur  : [lindex $ids 0]"
    puts "User choice for Pelvis : [lindex $ids 1]"
}

#-------------------------------------------------------------------------------
# .PROC DescribeStepsUI
# Display a text what the purpose of this module is.
# .ARGS
# str frame name of frame where the interface is
# .END
#-------------------------------------------------------------------------------
proc DescribeStepsUI {frame} {
    global Gui
    text $frame.tInstructions -bg $Gui(normalButton) -height 20
    $frame.tInstructions insert end "This is an example how to implement a morphometric tool. The source code is located at Modules/vtkMorphometrics/tcl/MeasurementDeveloper.tcl . The following steps demonstrate how to use the 'step factories' available in vtkMorphometrics/tcl/StepFactories.tcl"
    pack $frame.tInstructions -side top -padx $Gui(pad) -pady $Gui(pad)
}
######################

#-------------------------------------------------------------------------------
# .PROC MorphometricsDeveloperMeasurementInit
# Initialize the Workflow as well as the corresponding vtkObjects
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsDeveloperMeasurementInit {} {
    global Morphometrics
    WorkflowInitWorkflow MorphometricsDeveloper $Morphometrics(workflowFrame)
    WorkflowAddStep MorphometricsDeveloper MorphometricsDoNothingOnEnterExit MorphometricsDoNothingOnEnterExit DescribeStepsUI "Introduction"


    vtkPlaneSource MorphometricsExamplePlane
    MorphometricsExamplePlane SetOrigin 0 0 0
    MorphometricsExamplePlane SetPoint1 100 0 0
    MorphometricsExamplePlane SetPoint2 0 100 0
    MorphometricsExamplePlane SetNormal 2 1 3
    MorphometricsExamplePlane SetCenter -110 -10 65
    MorphometricsCreatePlanePlacementStep MorphometricsDeveloper MorphometricsExamplePlane "Place example plane" "The name of the vtk-object is 'MorphometricsExamplePlane'. Place it using the csys  and examine how 'MorphometricsExamplePlane' is updated. The update takes place whenever you enter the next or previous step."


    vtkSphereSource MorphometricsExampleSphere
    MorphometricsExampleSphere SetRadius 25
    MorphometricsExampleSphere SetThetaResolution 30
    MorphometricsExampleSphere SetPhiResolution 30
    MorphometricsExampleSphere SetCenter -110 -10 65
    MorphometricsCreateSpherePlacementStep MorphometricsDeveloper MorphometricsExampleSphere "Place example sphere" "The name of the vtk-object is 'MorphometricsExampleSphere'. Place it using the csys  and examine how 'MorphometricsExampleSphere' is updated. The update takes place whenever you enter the next or previous step."


    vtkCylinderSource MorphometricsExampleCylinder 
    MorphometricsExampleCylinder SetResolution 30
    MorphometricsExampleCylinder SetRadius 3
    MorphometricsExampleCylinder SetHeight 400
    
    vtkTransformPolyDataFilter MorphometricsExampleCylindertpdf
    vtkTransform MorphometricsExampleCylindertpdt
    MorphometricsExampleCylindertpdt RotateX 90
    MorphometricsExampleCylindertpdt RotateY 30
    MorphometricsExampleCylindertpdt RotateZ 7
    MorphometricsExampleCylindertpdt Translate -121 -50 -40
    MorphometricsExampleCylindertpdf SetTransform MorphometricsExampleCylindertpdt
    MorphometricsExampleCylindertpdf SetInput [MorphometricsExampleCylinder GetOutput]

    MorphometricsCreateCylinderPlacementStep MorphometricsDeveloper MorphometricsExampleCylinder MorphometricsExampleCylindertpdf "Place example plane" "The name of the vtk-object is 'MorphometricsExampleCylinder'. Place it using the csys  and examine how 'MorphometricsExampleCylinder' is updated. A cylinder is a good way of representong lines."


    MorphometricsCreateModelChooserStep MorphometricsDeveloper "Femur Pelvis" IgnoreModelsChoosen
}


#-------------------------------------------------------------------------------
# .PROC MorphometricsDeveloperMeasurementWrapper
# Necessary wrapper function 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MorphometricsDeveloperMeasurementWrapper {} {
    MorphometricsAddMeasurement Example MorphometricsDeveloper "An example how to implement a morphometric tool" MorphometricsDeveloperMeasurementInit
}

lappend Morphometrics(measurementInitTools) MorphometricsDeveloperMeasurementWrapper

