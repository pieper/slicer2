

proc uniqueCommandName {} {
    return name.[clock clicks]
}

proc addScalarBar { vlt ren1 renderWindow } {
  set scalarBar [uniqueCommandName]
  vtkScalarBarActor $scalarBar
  $scalarBar SetLookupTable $vlt
  $scalarBar SetMaximumNumberOfColors [$vlt GetNumberOfColors]
  $scalarBar SetOrientationToVertical
#  $ren1 SetBackground 0 0 0
  set numlabels [expr [$vlt GetNumberOfColors] + 1]
  if {$numlabels > 11} {
    set numlabels 11
  }
  $scalarBar SetNumberOfLabels $numlabels
  $scalarBar SetTitle "(mm)"

  $scalarBar SetPosition 0.1 0.1
  $scalarBar SetWidth 0.1
  $scalarBar SetHeight 0.8

  $ren1 AddActor2D $scalarBar
  $renderWindow Render

  puts "scalarBar is $scalarBar, ren1 is $ren1, renderWindow is $renderWindow"

}
