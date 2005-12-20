#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        CompareFlip.tcl
# PROCEDURES:
#   CompareFlipInit
#   CompareFlipUpdateMRML
#   CompareFlipSetVolume
#   CompareFlipApply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC CompareModuleInit
# Set CompareFlip array to the proper initial values.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareFlipInit {} {
    global CompareFlip Module

    # The id of the volume to flip
    set CompareFlip(VolID) 0

    lappend Module(procMRML) CompareFlipUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC CompareFlipUpdateMRML
# Update volume list on the GUI flip tab
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareFlipUpdateMRML {} {
    global Volume Module CompareFlip

    # See if the volume actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
       if {[lsearch $Volume(idList) $CompareFlip(VolID)] == -1} {
           CompareFlipSetVolume $n
       }

    set m $Module(CompareModule,fFlip).fVolume.mbVolume.m
    $m delete 0 end
    foreach v $Volume(idList) {
        set colbreak [MainVolumesBreakVolumeMenu $m]
        $m add command -label [Volume($v,node) GetName] \
        -command "CompareFlipSetVolume $v" \
        -columnbreak $colbreak
    }
}

#-------------------------------------------------------------------------------
# .PROC CompareFlipSetVolume
# Set the volume to be flipped
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CompareFlipSetVolume {v} {
    global CompareFlip Volume Module

    # Check if volume exists and use the None if not
    if {[lsearch $Volume(idList) $v] == -1} {
        set v $Volume(idNone)
    }

    # If no change, return
    if {$v == $CompareFlip(VolID)} {return}
    set CompareFlip(VolID) $v

    # Change button text
    set mVolume ${Module(CompareModule,fFlip)}.fVolume.mbVolume
    set conf  "-text [Volume($v,node) GetName]"
    eval $mVolume configure $conf
}

#-------------------------------------------------------------------------------
# .PROC CompareFlipApply
# Flip the selected volume following one of the main 3 axis
# .ARGS
# axis string The axis used as flip reference
# .END
#-------------------------------------------------------------------------------
proc CompareFlipApply {axis} {
  global MultiSlicer CompareFlip Volume

  set v $CompareFlip(VolID)

  set newvec [ IbrowserGetRasToVtkAxis $axis ::Volume($v,node) ]
  foreach { x y z } $newvec { }

  vtkImageFlip flip
  flip SetInput [ ::Volume($v,vol) GetOutput ]

  # now set the flip axis in VTK space
  if { ($x == 1) || ($x == -1) } {
  flip SetFilteredAxis 0
  } elseif { ($y == 1) || ( $y == -1) } {
  flip SetFilteredAxis 1
  } elseif { ($z == 1) || ($z == -1) } {
  flip SetFilteredAxis 2
  }

  ::Volume($v,vol) SetImageData [ flip GetOutput ]
  MainVolumesUpdate $v
  RenderAll

  flip Delete

  MultiSlicer ReformatModified
  MultiSlicer Update
  CompareRenderSlices
}
