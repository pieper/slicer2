proc XformInit { } {

}

proc XformAxisStart { widget axis x y } {
    global lastX lastY lastAxis
    global Xform Selected Csys Model

    puts "axis interaction begins"

    # Push Event Manager onto event stack 
    set Xform(eventManager) [subst { \
        {$widget <Control-B1-Motion> {XformAxis %W %x %y 1}} \
#        {$widget <Control-B2-Motion> {XformAxis %W %x %y 2}} \
        {$widget <Shift-B1-Motion> {XformAxis %W %x %y 2}} \
        {$widget <ButtonRelease> {XformAxisEnd %W %x %y 1}} }]
    
    pushEventManager $Xform(eventManager)

    # Initialize Csys motion variables
    set lastX $x
    set lastY $y
    set lastAxis $axis
    Csys(xform) SetMatrix [[viewRen GetActiveCamera] GetViewTransform]
    Csys(actor) GetMatrix Csys(matrix)
    Csys(xform) Concatenate Csys(matrix)
    foreach id $Selected(Model) {
    Model($id,actor,viewRen) GetMatrix Csys(inverse)
    Csys(inverse) Invert
    ####        Csys(actXform) PostMultiply
    Csys(actXform) SetMatrix Csys(matrix)
    Csys(actXform) Concatenate Csys(inverse)
    Csys(actXform) SetPoint 0 0 0 1
    eval Model($id,actor,viewRen) SetOrigin [Csys(actXform) GetPoint]
    }
#    DebugMsg [concat "Starting axis-based transformation with axis " $axis ]
    set Xform(xform) [vtkTransform Xform(xform)]
    Xform(xform) Identity
    Xform(xform) PostMultiply
}


proc XformAxisEnd { widget x y button } {
    global Xform
    global lastX lastY lastAxis
    
#    DebugMsg [concat "Ending axis xform " $lastAxis]
    if {[info exists Xform(xform)]} {
    Xform(xform) Delete
    }

    popHandler $widget <ButtonRelease>
    popHandler $widget <Control-B1-Motion>
#    popHandler $widget <Control-B2-Motion>
    popHandler $widget <Shift-B1-Motion>
#    popHandler $widget <Shift-Control-B1-Motion>
    
#    popEventManager
}


proc XformAxis { widget x y button } {
    global lastX lastY lastAxis
    global Selected Model
    
    set cam [viewRen GetActiveCamera]
    set deltaX [expr $x - $lastX]
    set deltaY [expr $lastY - $y]
    set axisX [[Csys(xform) GetMatrix] GetElement 0 $lastAxis]
    set axisY [[Csys(xform) GetMatrix] GetElement 1 $lastAxis]
    set unitX [Csys(matrix) GetElement 0 $lastAxis]
    set unitY [Csys(matrix) GetElement 1 $lastAxis]
    set unitZ [Csys(matrix) GetElement 2 $lastAxis]
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
    Csys(actor) AddPosition [expr $unitX*$dotprod] [expr $unitY*$dotprod] \
        [expr $unitZ*$dotprod]
    foreach id $Selected(Model) {
    Model($id,actor,viewRen) RotateWXYZ $angle $unitX $unitY $unitZ
    Model($id,actor,viewRen) AddPosition [expr $unitX*$dotprod] \
        [expr $unitY*$dotprod] [expr $unitZ*$dotprod]
    }
    if { $lastAxis == 0 } {
    Csys(actor) RotateX $angle
    } else {
    if { $lastAxis == 1 } {
        Csys(actor) RotateY $angle
    } else {
        Csys(actor) RotateZ $angle
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
    Render3D
    
}
