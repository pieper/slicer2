proc CsysInit {} {
}

proc CsysParams { module actor {axislen -1} {axisrad -1} {conelen -1} } {
    global Csys ${module}
    
    if { $axislen == -1 } { set axislen 100 }
    if { $axisrad == -1 } { set axisrad [expr $axislen*0.02] }
    if { $conelen == -1 } { set conelen [expr $axislen*0.2]}
    set axislen [expr $axislen-$conelen]
    
    set ${module}($actor,size) $axislen
    
    # set parameters for cylinder geometry and transform
    ${module}($actor,AxisCyl) SetRadius $axisrad
    ${module}($actor,AxisCyl) SetHeight $axislen
    ${module}($actor,AxisCyl) SetCenter 0 [expr -0.5*$axislen] 0
    ${module}($actor,AxisCyl) SetResolution 8
    ${module}($actor,CylXform) Identity
    ${module}($actor,CylXform) RotateZ 90
    
    # set parameters for cone geometry and transform
    ${module}($actor,AxisCone) SetRadius [expr $axisrad * 2.5]
    ${module}($actor,AxisCone) SetHeight $conelen
    ${module}($actor,AxisCone) SetResolution 8
    ${module}($actor,ConeXform) Identity
    ${module}($actor,ConeXform) Translate $axislen 0 0
    # pce_debug_msg [concat "Csys params: axislen=" $axislen " axisrad=" \
        #         $axisrad " conelen=" $conelen]
}

proc CsysResize { module actor size } {
    global ${module}
    CsysParams ${module} $actor $size
    Render3D
}

# procedure culminates with creation of "Csys" Assembly Actor
# with a Red X-Axis, a Green Y-Axis, and a Blue Z-Axis

proc CsysCreate { module actor axislen axisrad conelen  } {
    global Csys ${module}

    puts "$module"

    vtkCylinderSource ${module}($actor,AxisCyl)
    vtkConeSource ${module}($actor,AxisCone)
    vtkTransform ${module}($actor,CylXform)
    vtkTransform ${module}($actor,ConeXform)
    vtkTransformPolyDataFilter ${module}($actor,ConeXformFilter)
    ${module}($actor,ConeXformFilter) SetInput [${module}($actor,AxisCone) GetOutput]
    ${module}($actor,ConeXformFilter) SetTransform ${module}($actor,ConeXform)
    vtkTransformPolyDataFilter ${module}($actor,CylXformFilter)
    ${module}($actor,CylXformFilter) SetInput [${module}($actor,AxisCyl) GetOutput]
     ${module}($actor,CylXformFilter) SetTransform ${module}($actor,CylXform)
    vtkAppendPolyData ${module}($actor,Axis)
    ${module}($actor,Axis) AddInput [${module}($actor,CylXformFilter) GetOutput]
    ${module}($actor,Axis) AddInput [${module}($actor,ConeXformFilter) GetOutput]
    vtkPolyDataMapper ${module}($actor,AxisMapper)
    ${module}($actor,AxisMapper) SetInput [${module}($actor,Axis) GetOutput]
    vtkActor ${module}($actor,Xactor)
    ${module}($actor,Xactor) SetMapper ${module}($actor,AxisMapper)
    [${module}($actor,Xactor) GetProperty] SetColor 1 0.4 0.4
    ${module}($actor,Xactor) PickableOn
    vtkActor ${module}($actor,Yactor)
    ${module}($actor,Yactor) SetMapper ${module}($actor,AxisMapper)
    [${module}($actor,Yactor) GetProperty] SetColor 0.4 1 0.4
    ${module}($actor,Yactor) RotateZ 90
    ${module}($actor,Yactor) PickableOn
    vtkActor ${module}($actor,Zactor)
    ${module}($actor,Zactor) SetMapper ${module}($actor,AxisMapper)
    [${module}($actor,Zactor) GetProperty] SetColor 0.4 0.4 1
    ${module}($actor,Zactor) RotateY -90
    ${module}($actor,Zactor) PickableOn
    CsysParams $module $actor $axislen $axisrad $conelen
    set ${module}($actor,actor) [vtkAssembly ${module}($actor,actor)]
    ${module}($actor,actor) AddPart ${module}($actor,Xactor)
    ${module}($actor,actor) AddPart ${module}($actor,Yactor)
    ${module}($actor,actor) AddPart ${module}($actor,Zactor)
    ${module}($actor,actor) PickableOff
    vtkMatrix4x4 ${module}($actor,matrix)
    vtkTransform ${module}($actor,xform)
    vtkTransform ${module}($actor,actXform)
    vtkMatrix4x4 ${module}($actor,inverse)
    vtkTransform ${module}($actor,rasToWldTransform)
    ${module}($actor,actor) SetUserMatrix [${module}($actor,rasToWldTransform) GetMatrix]
}
