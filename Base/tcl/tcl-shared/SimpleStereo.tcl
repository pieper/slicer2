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
# FILE:        SimpleStereo.tcl
# PROCEDURES:  
#   ::SimpleStereo::moveCameraToView
#   ::SimpleStereo::formatCameraParams
#   ::SimpleStereo::restoreCameraParams
#==========================================================================auto=
# package require tclVectorUtils
# package require NestedList

namespace eval ::SimpleStereo {
    namespace export moveCameraToView formatCameraParams restoreCameraParams
}

#-------------------------------------------------------------------------------
# .PROC ::SimpleStereo::moveCameraToView
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ::SimpleStereo::moveCameraToView {ren view {disparityRatio 30.0}} {
    variable DisplayInfo
    set cam [$ren GetActiveCamera]
    set renWin [$ren GetRenderWindow]
    
    set camFP  [$cam GetFocalPoint]
    set camPos [$cam GetPosition]
    set camVAngle [$cam GetViewAngle]
    set camVShear [$cam GetViewShear]
    set camClipRange [$cam GetClippingRange]
    set camUp [$cam GetViewUp]
    set camNorm [$cam GetViewPlaneNormal]

    set camDist [::tclVectorUtils::VDist $camPos $camFP]
    set camRight [::tclVectorUtils::VCross $camUp $camNorm]
    set camRight [::tclVectorUtils::VNorm $camRight]

    switch -- $view {
        left    {set trackPos -1.0}
        right   {set trackPos 1.0}
        center  {set trackPos 0.0}
        default {set trackPos 0.0}
    }

    set viewShift [expr {$camDist / $disparityRatio / 2.0}]
    set thisViewShift [expr {$trackPos * double($viewShift)}]
    set shiftVec [::tclVectorUtils::VScale $thisViewShift $camRight]
    set newCamPos [::tclVectorUtils::VAdd $shiftVec $camPos]
    set newCamFP [::tclVectorUtils::VAdd $shiftVec $camFP]

    eval $cam SetPosition $newCamPos
    eval $cam SetFocalPoint $newCamFP
}

#-------------------------------------------------------------------------------
# .PROC ::SimpleStereo::formatCameraParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ::SimpleStereo::formatCameraParams {cam} {
    set params {}
    set params [::NestedList::setValue $params ClippingRange [$cam GetClippingRange]]
    set params [::NestedList::setValue $params FocalPoint [$cam GetFocalPoint]]
    set params [::NestedList::setValue $params Position [$cam GetPosition]]
    set params [::NestedList::setValue $params ViewShear [$cam GetViewShear]]
    set params [::NestedList::setValue $params ViewAngle [$cam GetViewAngle]]
    set params [::NestedList::setValue $params FocalDisk [$cam GetFocalDisk]]
    return $params
}

#-------------------------------------------------------------------------------
# .PROC ::SimpleStereo::restoreCameraParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ::SimpleStereo::restoreCameraParams {cam params} {
    eval $cam SetClippingRange [::NestedList::getValue $params ClippingRange]
    eval $cam SetFocalPoint    [::NestedList::getValue $params FocalPoint]
    eval $cam SetPosition      [::NestedList::getValue $params Position]
    eval $cam SetViewShear     [::NestedList::getValue $params ViewShear]
    eval $cam SetViewAngle     [::NestedList::getValue $params ViewAngle]
    eval $cam SetFocalDisk     [::NestedList::getValue $params FocalDisk]
}
