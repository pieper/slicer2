proc XformInit { } {
    

}

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
    ${module}($actor,xform) SetMatrix [[viewRen GetActiveCamera] GetViewTransform]
    ${module}($actor,actor) GetMatrix ${module}($actor,matrix)
    ${module}($actor,xform) Concatenate ${module}($actor,matrix)
    foreach id $Selected(Model) {
    Model($id,actor,viewRen) GetMatrix ${module}($actor,inverse)
    ${module}($actor,inverse) Invert
    ####        ${module}($actor,actXform) PostMultiply
    ${module}($actor,actXform) SetMatrix ${module}($actor,matrix)
    ${module}($actor,actXform) Concatenate ${module}($actor,inverse)
    ${module}($actor,actXform) SetPoint 0 0 0 1
    eval Model($id,$actor,viewRen) SetOrigin [${module}($actor,actXform) GetPoint]
    }
#    DebugMsg [concat "Starting axis-based transformation with axis " $axis ]
    set Xform(xform) [vtkTransform Xform(xform)]
    Xform(xform) Identity
    Xform(xform) PostMultiply
}


proc XformAxisEnd { bindingSet widget x y button } {
    global Xform
    global lastX lastY lastAxis
#    DebugMsg [concat "Ending axis xform " $lastAxis]
    if {[info exists Xform(xform)]} {
    Xform(xform) Delete
    }
    EvDeactivateBindingSet $bindingSet
}


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
    foreach id $Selected(Model) {
    Model($id,actor,viewRen) RotateWXYZ $angle $unitX $unitY $unitZ
    Model($id,actor,viewRen) AddPosition [expr $unitX*$dotprod] \
        [expr $unitY*$dotprod] [expr $unitZ*$dotprod]
    }
    if { $lastAxis == 0 } {
    ${module}($actor,actor) RotateX $angle
    } else {
    if { $lastAxis == 1 } {
        ${module}($actor,actor) RotateY $angle
    } else {
        ${module}($actor,actor) RotateZ $angle
    }   }
    if { [info commands cutactor] != "" } {
    cutactor AddPosition [expr $unitX*$dotprod] \
        [expr $unitY*$dotprod] [expr $unitZ*$dotprod]
    if { $lastAxis == 0 } {
        cutactor RotateX $angle
    } else {
        if { $lastAxis == 1 } {
        cutactor RotateY $angle
        } else {
        cutactor RotateZ $angle
    }   }
    }
    
    set lastX $x
    set lastY $y

    # Call each Module's "XformMotion" routine
    # This is a callback mechanism that allows modules
    # to do appropriate things with the Xform's motion
    #-------------------------------------------
    foreach m $Module(idList) {
    if {[info exists Module($m,procXformMotion)] == 1} {
        if {$Module(verbose) == 1} {puts "XformMotion: $m"}
        $Module($m,procXformMotion) ${module}($actor,actor)
    }
    }
    Render3D    
}
