proc CsysInit {} {
}

proc CsysParams { {axislen -1} {axisrad -1} {conelen -1} } {
	global Csys

	if { $axislen == -1 } { set axislen 100 }
	if { $axisrad == -1 } { set axisrad [expr $axislen*0.02] }
	if { $conelen == -1 } { set conelen [expr $axislen*0.2]}
	set axislen [expr $axislen-$conelen]

	set Csys(size) $axislen

	# set parameters for cylinder geometry and transform
	MeasureAxisCyl SetRadius $axisrad
	MeasureAxisCyl SetHeight $axislen
	MeasureAxisCyl SetCenter 0 [expr -0.5*$axislen] 0
	MeasureAxisCyl SetResolution 8
	MeasureCylXform Identity
	MeasureCylXform RotateZ 90

	# set parameters for cone geometry and transform
	MeasureAxisCone SetRadius [expr $axisrad * 2.5]
	MeasureAxisCone SetHeight $conelen
	MeasureAxisCone SetResolution 8
	MeasureConeXform Identity
	MeasureConeXform Translate $axislen 0 0
	# pce_debug_msg [concat "Csys params: axislen=" $axislen " axisrad=" \
	# 		$axisrad " conelen=" $conelen]
	}

proc CsysResize { size } {
	CsysParams $size
	Render3D
	}

proc CsysSetVisibility {} { 
	global Csys 
	Csys(actor) SetVisibility $Csys(visible)
	Render3D 
	}

# procedure culminates with creation of "Csys" Assembly Actor
# with a Red X-Axis, a Green Y-Axis, and a Blue Z-Axis
proc CsysCreate { axislen axisrad conelen  } {
	global Csys

	vtkCylinderSource MeasureAxisCyl
	vtkConeSource MeasureAxisCone
	vtkTransform MeasureCylXform
	vtkTransform MeasureConeXform
	vtkTransformPolyDataFilter MeasureConeXformFilter
	  MeasureConeXformFilter SetInput [MeasureAxisCone GetOutput]
	  MeasureConeXformFilter SetTransform MeasureConeXform
	vtkTransformPolyDataFilter MeasureCylXformFilter
	  MeasureCylXformFilter SetInput [MeasureAxisCyl GetOutput]
	  MeasureCylXformFilter SetTransform MeasureCylXform
	vtkAppendPolyData MeasureAxis
	  MeasureAxis AddInput [MeasureCylXformFilter GetOutput]
	  MeasureAxis AddInput [MeasureConeXformFilter GetOutput]
	vtkPolyDataMapper MeasureAxisMapper
	  MeasureAxisMapper SetInput [MeasureAxis GetOutput]
	vtkActor Xactor
	  Xactor SetMapper MeasureAxisMapper
	  [Xactor GetProperty] SetColor 1 0.4 0.4
	  Xactor PickableOn
	vtkActor Yactor
	  Yactor SetMapper MeasureAxisMapper
	  [Yactor GetProperty] SetColor 0.4 1 0.4
	  Yactor RotateZ 90
	  Yactor PickableOn
	vtkActor Zactor
	  Zactor SetMapper MeasureAxisMapper
	  [Zactor GetProperty] SetColor 0.4 0.4 1
	  Zactor RotateY -90
	  Zactor PickableOn
	CsysParams $axislen $axisrad $conelen
	set Csys(actor) [vtkAssembly Csys(actor)]
	  Csys(actor) AddPart Xactor
	  Csys(actor) AddPart Yactor
	  Csys(actor) AddPart Zactor
	  Csys(actor) PickableOff
	vtkMatrix4x4 Csys(matrix)
	vtkTransform Csys(xform)
	vtkTransform Csys(actXform)
	vtkMatrix4x4 Csys(inverse)
	vtkTransform Csys(rasToWldTransform)
	Csys(actor) SetUserMatrix [Csys(rasToWldTransform) GetMatrix]
	}
