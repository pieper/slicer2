#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        scalarbar.tcl
# PROCEDURES:  
#   uniqueCommandName
#   addScalarBar
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC uniqueCommandName
# Returns a string made up from a standard string (name) followed by a time stamp
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc uniqueCommandName {} {
    return name.[clock clicks]
}

#-------------------------------------------------------------------------------
# .PROC addScalarBar
# Adds a scalar bar to a render window.
# .ARGS
# LUT vlt look up table
# renderer ren1 renderer to add the scalar bar to
# window renderWindow the window the renderer displays in
# .END
#-------------------------------------------------------------------------------
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
