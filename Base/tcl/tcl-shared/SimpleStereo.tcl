package require tclVectorUtils
package require NestedList

namespace eval ::SimpleStereo {
    namespace export moveCameraToView formatCameraParams restoreCameraParams
}

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

proc ::SimpleStereo::restoreCameraParams {cam params} {
    eval $cam SetClippingRange [::NestedList::getValue $params ClippingRange]
    eval $cam SetFocalPoint    [::NestedList::getValue $params FocalPoint]
    eval $cam SetPosition      [::NestedList::getValue $params Position]
    eval $cam SetViewShear     [::NestedList::getValue $params ViewShear]
    eval $cam SetViewAngle     [::NestedList::getValue $params ViewAngle]
    eval $cam SetFocalDisk     [::NestedList::getValue $params FocalDisk]
}
