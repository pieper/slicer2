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
# FILE:        IbrowserProcessingUtils.tcl
# PROCEDURES:  
#   IbrowserGetRasToVtkAxis
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserGetRasToVtkAxis
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGetRasToVtkAxis { axis vnode } {
    #
    #--- Given an axis in RAS space, we want
    #--- to find the axis that corresponds in VTK 
    #--- space (which describes the vtkImageData
    #--- represented by a vtkMrmlVolumeNode.)
    #
    #--- create a vtkTransform
    vtkTransform T
    #--- get the transform matrix in string form
    #--- and set it to be the transform's matrix.
    set m [ $vnode GetRasToVtkMatrix ]
    eval T SetMatrix $m

    #--- axis along which to flip in VTK space
    if { $axis == "RL" } {
        #--- if Flipping along R-L
        #puts "RAS axis 1 0 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 -1 0 0 ]
    } elseif { $axis == "AP" } {
        #--- if flipping along A-P
        #puts "RAS axis 0 1 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 -1 0 ]
    } elseif { $axis == "SI" } {
        #--- if flipping along S-I
        #puts "RAS axis 0 0 1"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 0 -1 ]
    }
    foreach { x y z } $newvec { }
    #puts "VTK axis: $x $y $z"
    #--- because of scale, newvec might not be unit
    if { $x != 0 } {
        set newvec [ lreplace $newvec 0 0 1 ]
    }
    if { $y != 0 } {
        set newvec [ lreplace $newvec 1 1 1 ]
    }
    if { $z != 0 } {
        set newvec [ lreplace $newvec 2 2 1 ]
    }
    
    #--- clean up
    T Delete
    #--- return the new axis as a vector.
    return $newvec
}
