
proc TestRigidTransformInterpolate {} {

    catch "rti Delete"
    vtkRigidTransformInterpolate rti

    foreach mm "0 1 T" color {"1 0 0" "0 1 0" "0 0 1"} {
        catch "t$mm Delete"
        vtkTransform t$mm
        rti SetM$mm [t$mm GetMatrix]

        catch "viewRen RemoveActor actor$mm"
        catch "cone$mm Delete"
        catch "pdm$mm Delete"
        catch "actor$mm Delete"
        vtkConeSource cone$mm
        vtkPolyDataMapper pdm$mm
        pdm$mm SetInput [cone$mm GetOutput]
        vtkActor actor$mm
        actor$mm SetMapper pdm$mm
        eval [actor$mm GetProperty] SetColor $color
        actor$mm SetScale 10 10 10
        actor$mm SetUserMatrix [t$mm GetMatrix]
        viewRen AddActor actor$mm
    }

    t0 Translate 100 0 0
    t1 Translate -100 0 0
    t1 RotateX -20
    t1 RotateZ 75
    t1 RotateY 40
    foreach mm "0 1 T" {
        rti SetM$mm [t$mm GetMatrix]
    }

    for {set t 0.0} {$t <= 1.0} {set t [expr $t + .01]} {
        rti SetT $t
        rti Interpolate

        foreach mm "0 1 T" {
            actor$mm SetUserMatrix [t$mm GetMatrix]
        }
        actorT SetUserMatrix [rti GetMT]

        RenderAll
        after 100
        update
    }

}
