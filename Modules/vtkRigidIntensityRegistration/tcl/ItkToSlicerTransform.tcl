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
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This file exists to transform data from ITK space to VTK space
#  (The routines herein are also used in the TetraMesh module)
#
#
# =================================
# == The ITK Coordinate System ====
# =================================
#
#  slices are in row-major order. In the first slice, that means:
#  (0,0,0),(1,0,0),(2,0,0)...(Numx-1,0,0),(0,1,0)...
#
# Call the first coordinate x, the second y and the third z.
#
# The first row    is x=0. The last row is       x=(NumX-1)*XSpacing.
# The first column is y=0. The last column is    y=(NumY-1)*YSpacing.
# The first slice  is z=0. The last slice is     z=(NumZ-1)*Zspacing.
#
# =================================
# == The VTK Coordinate System ====
# =================================
#
# The VTK Coordinate system is almost the same as ITK
# The major difference is that the y-axis is read in flipped.
#
# =================================
# == The Slicer Coordinate Sytem == 
# =================================
# 
# The slicer coordinate system is built on top of VTK. Volumes are
# read into the slicer through VTK. Depending on how the data was read
# (RL,LR,IS,SI,PA,AP), the data is permuted and a translation is added
# so that (0,0,0) is at the center of the volume.  After the permution
# and translation is done, the data is considered in RAS space.
# (Note, CT data can have gantry tilt which makes the matrix more
# complicated. Also DICOM data can have more information which makes
# the matrix more complicated.)
#
# There is then another transformation from RAS to World space.
# Often, this matrix is the identity. It is not the identity if
# someone has applied one or multiple transformations to the volume
# inside the slicer.
#
#
# Note: there is much folklore that suggests that the slicer does not
# succeed in centering the coordinate system at the center of the
# volume. There is a strong suggestion that there is a half-voxel
# error. I've seen the fix (by Steve Haker) which is not currently
# turned on (as of Oct 14, 2003). However, I think his fix will only
# work sometimes.
#
# ====================
# ITK to ITK Transformations 
# ====================
#
#
# Let's say you wrote a standalone itk program which read in 2 volumes
# and found the transformation M between the 2 volumes. Let's say that
# you called the CenteringImage routine, so that the coordinates are now
# relative to the center of the volume. Now, let's say you want to get
# a Matrix M', between the original 2 volumes. It is straightforward to show
# that M' = Trans2^-1 M Trans1
# 
# where Trans1 is for the first volume and Trans2 is for the second.
# and Trans1 = -Offset((Numx-1)*XSpacing,(Numy-1)*YSpacing,(Numz-1)*ZSpacing)
#
# See ITK Application MultiResMIRegistration for an example of this
#
## ====================
# VTK to ITK Transformations 
# ====================
#
# Now, let's say you had the same problem as before, but you read in
# the volumes using VTK, and then passed them to ITK. Let's say you wanted
# to get the exact same matrix as in the standalone itk program that read
# in your data without VTK. Let Mvtk be the matrix you got using VTK. Then
# 
#  Then M = yflip^-1 Mvtk yflip 
#  and M' = ( yflip Trans2 )^-1 Mvtk ( yflip Trans1 )
#
# yflip = 1  0 0 0
#        0 -1 0 0 
#        0  0 1 0
#        0  0 0 1
#
# ======================
# Slicer to ITK Transformation
# ======================
#
# Now, let's say you had 2 volumes read into the slicer. Let's assume
# there are no RAS to world matricies, and we do an alignment so that
# we have a matrix from RAS space of the first volume to RAS space of
# the second volume. Call that M'' 
#
# Let P1 and P2 be the matricies that goes from RAS space of volume 1 and 2
# to ITK stand-alone space.
#
# M'' = P2^-1 M P1
# M'' = ( yflip Trans2 P2)^-1 Mvtk ( yflip Trans1 P1)
#
# So, what is P1 and P2?
#
# The "Position Matrix" is an RAS to ScaledIJK space (think millimeter
# space), with the origin of the matrix unfortunately on the wrong side
# of the volume (due to the j-axis flip). Note that if there is an error
# in the slicer matrix calculations, this matrix should include that error
# and should therefore take care of it. Call the Position Matrix 'Pos'.
#
# The "Position Matrix" must be "fixed". This is done in
# TetraMesh.tcl.  The result is essentially a y-flip and a y-translation.
# See "SetModelMoveOriginMatrix" in that file.  Note,
# there are again issues of a half-voxel shift. I'm not sure if it is
# done correctly there. Let this matrix be called PosFix
#
# Then P1 = PosFix1 Pos1
#      P2 = PosFix2 Pos2
#
# Then M''=(yflip Trans2 PosFix2 Pos2)^-1 Mvtk (yflip Trans1 PosFix1 Pos1)
#
# ===============
# Problems
# ===============
# The matricies I described are between RAS to RAS space. What about World to
# World space? Imagine there is a cascade of transforms around the volume we
# will be moving. We are changing one of those transforms. But, which one?
# At this point, I have no way of knowing that.
#
# For the volume we are not moving, it is clear that we should be able to
# just grab its Ras to World matrix with no problem. But, I do not do this
# now.
#
# Another problem is that if one volume needs to be inverted before being 
# compared to the other, I have no way of doing that right now.
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC GetSlicerWldToItkMatrix
#
# For a Volume, get the Wld to ITK transform
# This is the transform used to go from RAS space to ITK space
# if the volume was read in by the slicer (which it was)
#
# .END
#-------------------------------------------------------------------------------

proc GetSlicerWldToItkMatrix {vnode matrix} {
 
    GetSlicerRASToItkStandAloneMatrix $vnode $matrix

    vtkMatrix4x4 WldToRAS_
      WldToRAS_ DeepCopy [$vnode GetRasToWld]
      WldToRAS_ Invert

    $matrix Multiply4x4 $matrix WldToRAS_ $matrix

    WldToRAS_ Delete
}

 
#-------------------------------------------------------------------------------
# .PROC GetSlicerRASToItkMatrix
#
# For a Volume, get the RAS to ITK transform
# This is the transform used to go from RAS space to ITK space
# if the volume was read in by the slicer (which it was)
#
# .END
#-------------------------------------------------------------------------------

proc GetSlicerRASToItkMatrix {vnode matrix} {

    GetSlicerToItkStandAloneMatrix $vnode $matrix

    vtkMatrix4x4 SlicerToItkTmp_
    GetYFlippedItkCenteringMatrix  $vnode SlicerToItkTmp_

    $matrix Multiply4x4 SlicerToItkTmp_ $matrix $matrix

    SlicerToItkTmp_ Delete
}

#-------------------------------------------------------------------------------
# .PROC GetSlicerRasToItkStandAloneMatrix
#
# For a Volume, get the RAS to ITK transform
# where ITK coordinates refers to the coordinates that would be
# used if ITK read in the volume as a stand-alone program.
#
# This is P = PosFix*Pos, as described above.
#
# .END
#-------------------------------------------------------------------------------

proc GetSlicerToItkStandAloneMatrix  {vnode matrix} {
    SetModelMoveOriginMatrix $vnode $matrix
    $matrix Multiply4x4 [$vnode GetPosition] $matrix $matrix
    $matrix Invert
}

#-------------------------------------------------------------------------------
# .PROC GetYFlippedItkCenteringMatrix
#
# For a volume with volume node vnode
# Set the incoming vtkMatrix4x4 to the ITK un-centering matrix
# Then do the y-flip.
# This is refered to as yflip*Trans1 above.
# .END
#-------------------------------------------------------------------------------
proc GetYFlippedItkCenteringMatrix  {vnode matrix} {
    set Space0 [lindex [$vnode GetSpacing] 0]
    set Space1 [lindex [$vnode GetSpacing] 1]
    set Space2 [lindex [$vnode GetSpacing] 2]

    set num0 [lindex [$vnode GetDimensions] 0]
    set num1 [lindex [$vnode GetDimensions] 1]
    set num2 [expr [lindex [$vnode GetImageRange] 1] - \
               [lindex [$vnode GetImageRange] 0] + 1 ]

    $matrix Identity
    $matrix SetElement 1 1 -1

    $matrix SetElement 0 3 [ expr ($num0 - 1 ) * $Space0 * -0.5 ]
    $matrix SetElement 1 3 [ expr ($num1 - 1 ) * $Space1 *  0.5 ]
    $matrix SetElement 2 3 [ expr ($num2 - 1 ) * $Space2 * -0.5 ]
}

