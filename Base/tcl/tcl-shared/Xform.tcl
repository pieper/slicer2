#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for internal 
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
# FILE:        Xform.tcl
# PROCEDURES:  
#   XformAxisStart
#   XformAxisEnd
#   XformAxis
#==========================================================================auto=
proc XformInit { } {
    

}

#-------------------------------------------------------------------------------
# .PROC XformAxisStart
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc XformAxisStart { module actor widget axis x y } {
    global lastX lastY lastAxis $module
    global Xform Selected Csys Model Gui
    
    #tk_messageBox -message "start axis $axis"
    # Push Event Manager onto event stack 
    # create the bindings 
    EvDeclareEventHandler xformEv <B1-Motion> "XformAxis $module $actor %W %x %y 1"
    EvDeclareEventHandler xformEv <B3-Motion> "XformAxis $module $actor %W %x %y 2"
    EvDeclareEventHandler xformEv <ButtonRelease> "XformAxisEnd XformEvents %W %x %y 1"
    EvAddWidgetToBindingSet XformEvents $Gui(fViewWin) {xformEv}
    EvActivateBindingSet XformEvents

    # Initialize Csys motion variables
    set lastX $x
    set lastY $y
    set lastAxis $axis
    ${module}($actor,xform) SetMatrix [[viewRen GetActiveCamera] GetViewTransformMatrix]
    ${module}($actor,actor) GetMatrix ${module}($actor,matrix)
    ${module}($actor,xform) Concatenate ${module}($actor,matrix)


    foreach id $Selected(Model) {
    Model($id,actor,viewRen) GetMatrix ${module}($actor,inverse)
    ${module}($actor,inverse) Invert
    ###
    ${module}($actor,actXform) PostMultiply
    ${module}($actor,actXform) SetMatrix ${module}($actor,matrix)
    ${module}($actor,actXform) Concatenate ${module}($actor,inverse)
    ${module}($actor,actXform) TransformPoint 0 0 0
    eval Model($id,actor,viewRen) SetOrigin [${module}($actor,actXform) GetPosition]
    }

    #    DebugMsg [concat "Starting axis-based transformation with axis " $axis ]
    set Xform(xform) [vtkTransform Xform(xform)]
    Xform(xform) Identity 
    Xform(xform) PostMultiply
}


#-------------------------------------------------------------------------------
# .PROC XformAxisEnd
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc XformAxisEnd { bindingSet widget x y button } {
    global Xform
    global lastX lastY lastAxis
#    DebugMsg [concat "Ending axis xform " $lastAxis]
    if {[info exists Xform(xform)]} {
    Xform(xform) Delete
    }
    EvDeactivateBindingSet $bindingSet
}


#-------------------------------------------------------------------------------
# .PROC XformAxis
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc XformAxis { module actor widget x y button } {
    global lastX lastY lastAxis $module
    global Selected Model Module
     
    set cam [viewRen GetActiveCamera]
    set deltaX [expr $x - $lastX]
    set deltaY [expr $lastY - $y]
    set axisX [[${module}($actor,xform) GetMatrix] GetElement 0 $lastAxis]
    set axisY [[${module}($actor,xform) GetMatrix] GetElement 1 $lastAxis]
    set unitX [${module}($actor,matrix) GetElement 0 $lastAxis]
    set unitY [${module}($actor,matrix) GetElement 1 $lastAxis]
    set unitZ [${module}($actor,matrix) GetElement 2 $lastAxis]
    set norm [expr $axisX*$axisX + $axisY*$axisY]
    if { $norm < 0.00001 } { set norm 1.0 }
    set norm [expr sqrt($norm)]
    set dotprod [expr ($deltaX*$axisX + $deltaY*$axisY)/$norm]
    set xprod [expr ($deltaX*$axisY - $deltaY*$axisX)/$norm]
    set WindowY [lindex [$widget configure -height] 4]
    set dotprod [expr $dotprod*[$cam GetDistance]* \
        sin(3.14159*[$cam GetViewAngle]/180.0)/$WindowY]
    set angle [expr $xprod*180.0/$WindowY]
    if { $button == 1 } {
    set angle 0.0
    }
    if { $button == 2 } {
    set dotprod 0.0
    }
#    DebugMsg [concat "moving " $lastAxis $dotprod $xprod "..." ]
    ${module}($actor,actor) AddPosition [expr $unitX*$dotprod] [expr $unitY*$dotprod] \
        [expr $unitZ*$dotprod]

    

    if { $lastAxis == 0 } {
    ${module}($actor,actor) RotateX $angle
    } else {
    if { $lastAxis == 1 } {
        ${module}($actor,actor) RotateY $angle
    } else {
        ${module}($actor,actor) RotateZ $angle
    }   }
   
    
    set lastX $x
    set lastY $y

    # Call each Module's "XformMotion" routine
    # This is a callback mechanism that allows modules
    # to do appropriate things with the Xform's motion
    #-------------------------------------------
    foreach m $Module(idList) {
    if {[info exists Module($m,procXformMotion)] == 1} {
        if {$Module(verbose) == 1} {puts "XformMotion: $m"}
        $Module($m,procXformMotion) ${module}($actor,actor) $angle $dotprod $unitX $unitY $unitZ
    }
    }
    Render3D    
}
