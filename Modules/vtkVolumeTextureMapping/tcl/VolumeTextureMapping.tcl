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
# FILE:        VolumeTextureMapping.tcl
# PROCEDURES:  
# VolumeTextureMappingInit 
# VolumeTextureMappingBuildGUI 
# changeDim 
# setColor 
# getColor
# gradImage
# ClipVolumes
# CheckLines 
# ChangeRotDir 
# RotateClipPlanePlane 
# DistanceClipPlanePlane 
# SetClipPlaneType 
# SpacingClipPlane 
# ChangeVolumeDim 
# NumberOfPlanes 
# defaultPoints 
# getPointX 
# getPointY 
# ChangeVolume 
# TFPoints 
# TFInteractions 
# menuPopLine 
# menuPopPoint 
# holdOverLine 
# leaveLine 
# createHistLine
# createTFLine 
# holdOverPoint 
# leavePoint 
# setValues 
# addPoint 
# removePoint
# clickOnPoint
# releasePoint 
# movePoint 
# ChangeTransformMatrix 
# VolumeTextureMappingBuildVTK 
# VolumeTextureMappingRefresh 
#VolumeTextureMappingCameraMotion
# VolumeTextureMappingEnter 
# VolumeTextureMappingExit 
# VolumeTextureMappingUpdateMRML 
# VolumeTextureMappingSetOriginal1 
# VolumeTextureMappingSetOriginal2 
# VolumeTextureMappingSetOriginal3 
# VolumeTextureMappingStorePresets 
# VolumeTextureMappingRecallPresets 

#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingInit {} {
    global VolumeTextureMapping Module Volume Model prog 
    
    set m VolumeTextureMapping


    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "3D Texture Mapping, volume rendering."
    set Module($m,author) "Frida Hernell, Link÷ping University"
    set Module($m,row1List) "Help Settings Transfer Clip"
    set Module($m,row1Name) "{Help} {Settings} {Transfer Functions} {Clip}"
    set Module($m,row1,tab) Settings
    set Module($m,procGUI) VolumeTextureMappingBuildGUI
    set Module($m,procEnter) VolumeTextureMappingEnter
    set Module($m,procExit) VolumeTextureMappingExit
    set Module($m,procVTK) VolumeTextureMappingBuildVTK
    set Module($m,procMRML) VolumeTextureMappingUpdateMRML
    set Module($m,procCameraMotion) VolumeTextureMappingCameraMotion

    lappend Module(procStorePresets) VolumeTextureMappingStorePresets
    lappend Module(procRecallPresets) VolumeTextureMappingRecallPresets
    set Module(VolumeTextureMapping,presets) "idOriginal1='0' idOriginal2='0' idOriginal3='0' hideOnExit='1' " 
    set Module($m,depend) ""

    set VolumeTextureMapping(idOriginal1)  $Volume(idNone)
    set VolumeTextureMapping(idOriginal2)  $Volume(idNone)
    set VolumeTextureMapping(idOriginal3)  $Volume(idNone)

    set VolumeTextureMapping(transferFunctionSaveFileName) "$prog/cttrffunc.xml"
    set VolumeTextureMapping(transferFunctionReadFileName) "$prog/cttrffunc.xml"
    set VolumeTextureMapping(hideOnExit) "1"
    set VolumeTextureMapping(volumeVisible) "0"
    set VolumeTextureMapping(currentVolume) "volume1"

    set VolumeTextureMapping(contents) "VolumeTextureMappingTransferFunctions"
    set VolumeTextureMapping(eventManager)  {  }


}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingBuildGUI
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingBuildGUI {} {
    global Gui VolumeTextureMapping Module Volume Model env
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    #    set help "
    #    The VolumeTextureMapping module is under heavy development."

    set help "
Volume rendering developed by Frida Hernell, CMIV (Link÷ping university).
<P>
Description by tabs:
<P><B>Settings:</B><BR>
<UL>
<LI>Set which volumes to use in volume 1, volume 2 and volume 3. 
<LI>Set the texture dimension if other than default is wanted. Choose which volume to affect with the radio buttons.
<LI>Press Refresh View to render the volumes in the viewport.
<LI>Change the amount of planes with the slider. Many planes give a detailed rendering but can become slow to interact with.
</UL>
<P><B>Transfer functions:</B><BR>
<UL>
<LI>Transfer functions play a crucial role in the rendering process. Along the ray,    at every position, the given voxel value will be turned into a color and opacity value according to these. The transfer functions can be changed if the points in the graphs is moved. Choose which volume to affect with the radio buttons. The colors to vary between can be changed with the max and the min button. The gray graph represents the color and the black graph represents the opacity. 
            <LI> The transformation matrix can be changed in order to place the different volumes correct according to transformation, scaling and rotation. Press the button refresh to see the changes in the viewport. 
</UL>
<P><B>Clip:</B><BR>
<UL>
<LI>Choose which volumes to clip with the check boxes.
<LI>Choose which typ of clipping to use. Single uses one plane, double uses two planes and cube uses six planes in order to form a cube.
<LI>The check box Lines can be choosen in order to se the clip planes.
<LI>Choose which axis to rotate arround.
<LI>Choose how much to rotate with the slider.
<LI>Choose how far from origo the clip plane will be placed with the slider Distance from origin
<LI>If the clip types double or cube are choosen than the slider Spacing between planes can be used. 
</UL>
"


    regsub -all "\n" $help {} help
    MainHelpApplyTags VolumeTextureMapping $help
    MainHelpBuildGUI VolumeTextureMapping
    
    #-------------------------------------------
    # Settings frame
    #-------------------------------------------
    set fSettings $Module(VolumeTextureMapping,fSettings)
    set f $fSettings

    foreach frame "RefreshSettings Planes " {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad)
    }
    foreach frame "Volume Dimension Buttons" {
    frame $f.fRefreshSettings.f$frame -bg $Gui(activeWorkspace)
    pack $f.fRefreshSettings.f$frame -side top -padx 0 -pady $Gui(pad)
    }
    

    #-------------------------------------------
    # Settings->RefreshSettings
    #-------------------------------------------
    set f $fSettings.fRefreshSettings
    $f config -relief groove -bd 3 -width 500


    #-------------------------------------------
    # Settings->Volume frame
    #-------------------------------------------
    set f $fSettings.fRefreshSettings.fVolume
    
    # Add menus that list models and volumes
    DevAddSelectButton VolumeTextureMapping $f Original1 "Volume1" Grid

    # Add menus that list models and volumes
    DevAddSelectButton VolumeTextureMapping $f Original2 "Volume2" Grid

    # Add menus that list models and volumes
    DevAddSelectButton VolumeTextureMapping $f Original3 "Volume3" Grid


    #-------------------------------------------
    # Settings->Dimension frame
    #-------------------------------------------
    set f $fSettings.fRefreshSettings.fDimension
    
    frame $f.fChooseVol -bg $Gui(activeWorkspace)
    foreach i {volume1 volume2 volume3} {
        eval {radiobutton $f.fChooseVol.$i -text $i -value $i -variable VolumeTextureMapping(currentVolume) -relief flat -value $i -bg $Gui(activeWorkspace) -command "ChangeVolumeDim $f" } $Gui(WCA)
        pack $f.fChooseVol.$i -side left -padx 2 
     }
     pack $f.fChooseVol -side top -padx 0 -pady $Gui(pad)
    
    label $f.dims -justify left -text "Texture dimension" -bg $Gui(activeWorkspace)
    pack $f.dims -side top -pady 2

    label $f.dimxL -justify left -text "X:" -bg $Gui(activeWorkspace)
    pack $f.dimxL -side left
    menubutton $f.dimx -text "--" -menu $f.dimx.x -bg $Gui(activeWorkspace)
    $f.dimx config -relief groove -bd 3 
    menu $f.dimx.x 
    $f.dimx.x add command -label "16" -underline 0 -command "changeDim $f.dimx 0 16"
    $f.dimx.x add command -label "32" -underline 0 -command "changeDim $f.dimx 0 32"
    $f.dimx.x add command -label "64" -underline 0 -command "changeDim $f.dimx 0 64"
    $f.dimx.x add command -label "128" -underline 0 -command "changeDim $f.dimx 0 128"
    $f.dimx.x add command -label "256" -underline 0 -command "changeDim $f.dimx 0 256"
    pack $f.dimx -side left -padx 4

    label $f.dimyL -justify left -text "Y:" -bg $Gui(activeWorkspace)
    pack $f.dimyL -side left
    menubutton $f.dimy -text "--" -menu $f.dimy.y -bg $Gui(activeWorkspace)
    $f.dimy config -relief groove -bd 3 
    menu $f.dimy.y 
    $f.dimy.y add command -label "16" -underline 0 -command "changeDim $f.dimy 1 16"
    $f.dimy.y add command -label "32" -underline 0 -command "changeDim $f.dimy 1 32"
    $f.dimy.y add command -label "64" -underline 0 -command "changeDim $f.dimy 1 64"
    $f.dimy.y add command -label "128" -underline 0 -command "changeDim $f.dimy 1 128"
    $f.dimy.y add command -label "256" -underline 0 -command "changeDim $f.dimy 1 256"
    pack $f.dimy -side left -padx 4

    label $f.dimzL -justify left -text "Z:" -bg $Gui(activeWorkspace)
    pack $f.dimzL -side left
    menubutton $f.dimz -text "--" -menu $f.dimz.z -bg $Gui(activeWorkspace)
    $f.dimz config -relief groove -bd 3 
    menu $f.dimz.z 
    $f.dimz.z add command -label "16" -underline 0 -command "changeDim $f.dimz 2 16"
    $f.dimz.z add command -label "32" -underline 0 -command "changeDim $f.dimz 2 32"
    $f.dimz.z add command -label "64" -underline 0 -command "changeDim $f.dimz 2 64"
    $f.dimz.z add command -label "128" -underline 0 -command "changeDim $f.dimz 2 128"
    $f.dimz.z add command -label "256" -underline 0 -command "changeDim $f.dimz 2 256"
    pack $f.dimz -side left -padx 4
    
    #-------------------------------------------
    # Settings->Buttons frame
    #-------------------------------------------
    set f $fSettings.fRefreshSettings.fButtons

    DevAddButton $f.bRefresh {Refresh View} VolumeTextureMappingRefresh
    pack $f.bRefresh -side top
  


    #-------------------------------------------
    # Settings->Planes frame
    #-------------------------------------------
    set f $fSettings.fPlanes

    label $f.lplanes -justify left -text "Amount of planes" -bg $Gui(activeWorkspace)
    pack $f.lplanes -side top

    scale $f.sscalePlanes -orient horizontal -from 0 -to 1500 -tickinterval 0 -length 200 -showvalue true -bg $Gui(activeWorkspace) -command NumberOfPlanes 
    pack $f.sscalePlanes -side top
    $f.sscalePlanes set 128
    
    #-------------------------------------------
    # Transfer frame
    #-------------------------------------------
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer
    
    VolumeTextureMapping(texturevolumeMapper) IniatializeColors
    foreach frame "TFVolume TFVolume1 Transform" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    foreach frame "Buttons IO" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad)
    }

    #-------------------------------------------
    # Transfer->TFVolume frame
    #-------------------------------------------
    set f $fTransfer.fTFVolume
    $f config -relief groove -bd 3 

    foreach i {volume1 volume2 volume3} {
    radiobutton $f.b$i -text $i -variable VolumeTextureMapping(currentVolume) -relief flat -value $i -bg $Gui(activeWorkspace) -command "ChangeVolume f" 
    pack $f.b$i -side left -padx 2 
    }

    #-------------------------------------------
    # Transfer->TFVolume1 frame
    #-------------------------------------------
    set f $fTransfer.fTFVolume1
    
    global colorMin colorMax
    set colorMin "#000000"
    set colorMax "#ffffff"
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 0 0 [format "%i" 0x[lindex [split $colorMin {}] 1][lindex [split $colorMin {}] 2]] [format "%i" 0x[lindex [split $colorMin {}] 3][lindex [split $colorMin {}] 4]] [format "%i" 0x[lindex [split $colorMin {}] 5][lindex [split $colorMin {}] 6]] 
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 0 1 [format "%i" 0x[lindex [split $colorMax {}] 1][lindex [split $colorMax {}] 2]] [format "%i" 0x[lindex [split $colorMax {}] 3][lindex [split $colorMax {}] 4]] [format "%i" 0x[lindex [split $colorMax {}] 5][lindex [split $colorMax {}] 6]] 
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 1 0 [format "%i" 0x[lindex [split $colorMin {}] 1][lindex [split $colorMin {}] 2]] [format "%i" 0x[lindex [split $colorMin {}] 3][lindex [split $colorMin {}] 4]] [format "%i" 0x[lindex [split $colorMin {}] 5][lindex [split $colorMin {}] 6]] 
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 1 1 [format "%i" 0x[lindex [split $colorMax {}] 1][lindex [split $colorMax {}] 2]] [format "%i" 0x[lindex [split $colorMax {}] 3][lindex [split $colorMax {}] 4]] [format "%i" 0x[lindex [split $colorMax {}] 5][lindex [split $colorMax {}] 6]] 
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 2 0 [format "%i" 0x[lindex [split $colorMin {}] 1][lindex [split $colorMin {}] 2]] [format "%i" 0x[lindex [split $colorMin {}] 3][lindex [split $colorMin {}] 4]] [format "%i" 0x[lindex [split $colorMin {}] 5][lindex [split $colorMin {}] 6]] 
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax 2 1 [format "%i" 0x[lindex [split $colorMax {}] 1][lindex [split $colorMax {}] 2]] [format "%i" 0x[lindex [split $colorMax {}] 3][lindex [split $colorMax {}] 4]] [format "%i" 0x[lindex [split $colorMax {}] 5][lindex [split $colorMax {}] 6]] 

    frame $f.graph -bg $Gui(activeWorkspace)
      set grad1 [gradImage 10]
    canvas $f.graph.gradient1 -bd 0 -highlightthickness 0 -height 107 -width 10 
    $f.graph.gradient1 create image 0 0 -anchor nw -image $grad1 -tag grad1 
    pack $f.graph.gradient1 -side left -padx 0
        
    canvas $f.graph.canvas1 -relief raised -width 216 -height 106 -bg $Gui(activeWorkspace)
    $f.graph.canvas1 create polygon 2 2 216 2 216 106 2 106 -fill white -outline black -width 1
    pack $f.graph.canvas1 -side left -padx 0
    pack $f.graph -side top

    frame $f.bVolCol -bg $Gui(activeWorkspace)
    label $f.bVolCol.title -justify left -text "Color:" -bg $Gui(activeWorkspace)
    pack $f.bVolCol.title -side left -padx 2
    button $f.bVolCol.b1 -width 10 -command "setColor $f $f.bVolCol.b1 2" -text max
    pack $f.bVolCol.b1 -side left -padx 2
    button $f.bVolCol.b2 -width 10 -command "setColor $f $f.bVolCol.b2 1" -text min
    pack $f.bVolCol.b2 -side left -padx 2
    pack $f.bVolCol -side top

    global .menuPoint .menuLine
    menu .menuPoint
    menu .menuLine
    defaultPoints $f.graph.canvas1
    
    #-------------------------------------------
    # Transfer->Transform frame
    #-------------------------------------------
    set f $fTransfer.fTransform
    $f config -relief groove -bd 3 
    
    label $f.title -justify left -text "Coordinate transformation matrix" -bg $Gui(activeWorkspace)
    pack $f.title -side top

    set row "row0"
    frame $f.$row -bg $Gui(activeWorkspace)
    set i t00
    entry $f.$row.$i -width 3  -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t01
    entry $f.$row.$i -width 3  -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t02
    entry $f.$row.$i -width 3  -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t03
    entry $f.$row.$i -width 3  -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    pack $f.$row -side top
    set row "row1"
    frame $f.$row -bg $Gui(activeWorkspace)
    set i t10
    entry $f.$row.$i -width 3  -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t11
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t12
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t13
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    pack $f.$row -side top
    set row "row2"
    frame $f.$row -bg $Gui(activeWorkspace)
    set i t20
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t21
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t22
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t23
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    pack $f.$row -side top
    set row "row3"
    frame $f.$row -bg $Gui(activeWorkspace)
    set i t30
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t31
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t32
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    set i t33
    entry $f.$row.$i -width 3 -textvariable $i 
    pack $f.$row.$i -side left -padx 5 -pady 2
    pack $f.$row -side top

    button $f.transRefresh -width 10 -text "Refresh" -command ChangeTransformMatrix
    pack $f.transRefresh -side top


    #-------------------------------------------
    # Clip frame
    #-------------------------------------------
    set fClip $Module(VolumeTextureMapping,fClip)
    set f $fClip
    global lastDir
    global lastAngle
    set lastDir -1
    set lastAngle 0
    VolumeTextureMapping(texturevolumeMapper) InitializeClipPlanes

    foreach frame "Volumes Type Rotation Distance Planes" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad)
    }
    
    #-------------------------------------------
    # Clip->Volumes
    #-------------------------------------------
    set f $fClip.fVolumes
    $f config -relief groove -bd 3 
    
    frame $f.cvolumes -bg $Gui(activeWorkspace)
    foreach i {"volume1" "volume2" "volume3"} {
    checkbutton $f.cvolumes.p$i -relief flat -variable $i -bg $Gui(activeWorkspace) -command ClipVolumes
    pack $f.cvolumes.p$i -side left -padx 2
    label $f.cvolumes.l$i -justify left -text $i -bg $Gui(activeWorkspace)
    pack $f.cvolumes.l$i -side left -padx 2
    }
    pack $f.cvolumes -side top

    
    #-------------------------------------------
    # Clip->Type
    #-------------------------------------------
    set f $fClip.fType

    frame $f.type -bg $Gui(activeWorkspace)
    foreach i {single double cube} {
    radiobutton $f.type.b$i -text $i -variable planetype -relief flat -value $i -bg $Gui(activeWorkspace) -command SetClipPlaneType
    pack $f.type.b$i -side left -padx 2 
    }
    pack $f.type -side top
    
    frame $f.lines -bg $Gui(activeWorkspace)
    checkbutton $f.lines.clines -text "Lines" -variable "lines" -command CheckLines -bg $Gui(activeWorkspace)
    pack $f.lines.clines -side left -padx 2
    pack $f.lines -side top



    #-------------------------------------------
    # Clip->Rotation
    #-------------------------------------------
    set f $fClip.fRotation
    $f config -relief groove -bd 3
    
    label $f.lrotate -justify left -text "Rotation" -bg $Gui(activeWorkspace)
    pack $f.lrotate -side top
    
    global rotDir

    frame $f.rot -borderwidth 10 -bg $Gui(activeWorkspace)
    image create photo rX -file [file join $env(SLICER_HOME) Modules vtkVolumeTextureMapping images rX.gif]
    radiobutton $f.rot.c1 -image rX -selectimage rX    -indicatoron 0 -bg $Gui(activeWorkspace) -variable rotDir -value "x" -command "ChangeRotDir $f"
    pack $f.rot.c1 -side left

    image create photo rY -file [file join $env(SLICER_HOME) Modules vtkVolumeTextureMapping images rY.gif] 
    radiobutton $f.rot.c2 -image rY -selectimage rY    -indicatoron 0 -bg $Gui(activeWorkspace) -variable rotDir -value "y" -command "ChangeRotDir $f"
    pack $f.rot.c2 -side left

    image create photo rZ -file [file join $env(SLICER_HOME) Modules vtkVolumeTextureMapping images rZ.gif] 
    radiobutton $f.rot.c3 -image rZ -selectimage rZ    -indicatoron 0 -bg $Gui(activeWorkspace) -variable rotDir -value "z" -command "ChangeRotDir $f"
    pack $f.rot.c3 -side left

    pack $f.rot -side top

    scale $f.sscale -orient horizontal -from -180 -to 180 -tickinterval 0 -length 200 -showvalue true -bg $Gui(activeWorkspace) -command "RotateClipPlanePlane $f"
    pack $f.sscale -side top
    $f.sscale set 0
    
    #-------------------------------------------
    # Clip->Distance
    #-------------------------------------------
    set f $fClip.fDistance
    $f config -relief groove -bd 3

    label $f.ldist -justify left -text "Distance from origin" -bg $Gui(activeWorkspace)
    pack $f.ldist -side top

    scale $f.sdist -orient horizontal -from -128 -to 128 -tickinterval 0 -length 200 -showvalue true -bg $Gui(activeWorkspace) -command "DistanceClipPlanePlane $f"
    pack $f.sdist -side top
    $f.sdist set 0

    label $f.lspace -justify left -text "Spacing between planes" -bg $Gui(activeWorkspace)
    pack $f.lspace -side top
    
    global xangle yangle zangle
    set xangle 0
    set yangle 0
    set zangle 0
    
    scale $f.sspace -orient horizontal -from 0 -to 256 -length 200 -tickinterval 0 -showvalue true -bg $Gui(activeWorkspace) -command "SpacingClipPlane $f"
    pack $f.sspace -side top
    $f.sspace set 64
}

#-------------------------------------------------------------------------------
# .PROC changeDim
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc changeDim {f dir dim} {
    global VolumeTextureMapping
    $f configure -text $dim
    if {[string compare $VolumeTextureMapping(currentVolume) "volume1"] == 0} {
    set thisVol 0
    puts "VOLUME1"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume2"] == 0} {
    set thisVol 1
    puts "VOLUME2"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume3"] == 0} {
    set thisVol 2
    puts "VOLUME3"
    }
    VolumeTextureMapping(texturevolumeMapper) SetDimension $thisVol $dir $dim
}

#-------------------------------------------------------------------------------
# .PROC setColor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc setColor {f b dir} {
    global colorMin colorMax currentVolume 
    grab $b
    
    #set initialColor [$b cget -background]
    set color [tk_chooseColor -title "Choose a color" -parent $b ]
    #-initialcolor $initialColor]
    
    if {$dir == 1} {    
    set colorMin $color
    set grad1 [gradImage 10]
    $f.graph.gradient1 delete all
    $f.graph.gradient1 create image 0 0 -anchor nw -image $grad1 -tag grad1
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax $currentVolume 0 [format "%i" 0x[lindex [split $colorMin {}] 1][lindex [split $colorMin {}] 2]] [format "%i" 0x[lindex [split $colorMin {}] 3][lindex [split $colorMin {}] 4]] [format "%i" 0x[lindex [split $colorMin {}] 5][lindex [split $colorMin {}] 6]] 
    } elseif {$dir == 2} {
    set colorMax $color
    set grad1 [gradImage 10]
    $f.graph.gradient1 delete all
    $f.graph.gradient1 create image 0 0 -anchor nw -image $grad1 -tag grad1
    VolumeTextureMapping(texturevolumeMapper) SetColorMinMax $currentVolume 1 [format "%i" 0x[lindex [split $colorMax {}] 1][lindex [split $colorMax {}] 2]] [format "%i" 0x[lindex [split $colorMax {}] 3][lindex [split $colorMax {}] 4]] [format "%i" 0x[lindex [split $colorMax {}] 5][lindex [split $colorMax {}] 6]] 
    }
    
    
    grab release $b
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC getColor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc getColor {f} {
    global currentVolume colorMin colorMax
    
    
    set r1 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 0 0]
    set g1 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 0 1]
    set b1 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 0 2]
    set x [format %2.2x $r1]
    set y [format %2.2x $g1]
    set z [format %2.2x $b1]
    set colorMin "#${x}${y}${z}"
    set r2 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 1 0]
    set g2 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 1 1]
    set b2 [VolumeTextureMapping(texturevolumeMapper) GetColorMinMax $currentVolume 1 2]
    set x [format %2.2x $r2]
    set y [format %2.2x $g2]
    set z [format %2.2x $b2]
    set colorMax "#${x}${y}${z}"
    set grad1 [gradImage 10]
    $f.graph.gradient1 delete all
    $f.graph.gradient1 create image 0 0 -anchor nw -image $grad1 -tag grad1
    
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC gradImage
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc gradImage {{w}} {
    global colorMax colorMin 
    set im [image create photo -width $w -height 106]
    
    set r2 [format "%i" 0x[lindex [split $colorMin {}] 1][lindex [split $colorMin {}] 2]]
    set g2 [format "%i" 0x[lindex [split $colorMin {}] 3][lindex [split $colorMin {}] 4]]
    set b2 [format "%i" 0x[lindex [split $colorMin {}] 5][lindex [split $colorMin {}] 6]]
    set r1 [format "%i" 0x[lindex [split $colorMax {}] 1][lindex [split $colorMax {}] 2]]
    set g1 [format "%i" 0x[lindex [split $colorMax {}] 3][lindex [split $colorMax {}] 4]]
    set b1 [format "%i" 0x[lindex [split $colorMax {}] 5][lindex [split $colorMax {}] 6]]
    
    
    set rdiff [expr [expr $r2 - $r1]/105]
    set gdiff [expr [expr $g2 - $g1]/105] 
    set bdiff [expr [expr $b2 - $b1]/105]
    
    set r $r1
    set g $g1
    set b $b1
    
    for {set i 0; set j 1} {$i < 106} {incr i; incr j} {
    set r [expr $r + $rdiff]
    set g [expr $g + $gdiff]
    set b [expr $b + $bdiff]
    
    if {$r > 255} {
        set r 255
    } elseif {$r < 0} {
        set r 0
    }
    if {$g > 255} {
        set g 255
    } elseif {$g < 0} {
        set g 0
    }
    if {$b > 255} {
        set b 255
    } elseif {$b < 0} {
        set b 0
    }
    
    set x [format %2.2x $r]
    set y [format %2.2x $g]
    set z [format %2.2x $b]
    $im put "#${x}${y}${z}" -to 0 $i $w $j    
    }
    return $im
    
}

#-------------------------------------------------------------------------------
# .PROC ClipVolumes
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ClipVolumes {} {
    global volume1 volume2 volume3
    if {$volume1} {
    set num 0
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 1
    } else {
    set num 0
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 0
    } 
    if {$volume2} {
    set num 1
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 1
    } else {
    set num 1
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 0
    }
    
    if {$volume3} {
    set num 2
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 1
    } else {
    set num 2
    VolumeTextureMapping(texturevolumeMapper) VolumesToClip $num 0
    }
    
    
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    
    
}

#-------------------------------------------------------------------------------
# .PROC CheckLines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CheckLines {} {
    global lines
    if {$lines} {
    VolumeTextureMapping(texturevolumeMapper) EnableClipLines 1    
    } else {
    VolumeTextureMapping(texturevolumeMapper) EnableClipLines 0    
    }
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC ChangeRotDir
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ChangeRotDir {f} {
    global xangle yangle zangle rotDir lastDir lastAngle
    
    
    
    if {[string compare $rotDir "x"] == 0} {
    
    set dir 0
    puts "X rotation"
    } elseif {[string compare $rotDir "y"] == 0} {
    
    set dir 1
    puts "Y rotation"
    } elseif {[string compare $rotDir "z"] == 0} {
    
    set dir 2
    puts "Z rotation"
    }
    
    if {$lastDir != $dir} {    
    if {$lastDir == 0} {
        set xangle $lastAngle
        
    } elseif {$lastDir == 1} {
        set yangle $lastAngle
        
    } else {
        set zangle $lastAngle    
    }
    
    if {$dir == 0} {
        $f.sscale set $xangle
    } elseif {$dir == 1} {
        $f.sscale set $yangle
    } else {
        $f.sscale set $zangle
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC RotateClipPlanePlane
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RotateClipPlanePlane {f angle} {
    global lastDir lastAngle rotDir
    
    set dir 0
    if {[string compare $rotDir "x"] == 0} {
    set dir 0
    } elseif {[string compare $rotDir "y"] == 0} {
    set dir 1
    } elseif {[string compare $rotDir "z"] == 0} {
    set dir 2
    }
    
    
    
    VolumeTextureMapping(texturevolumeMapper) ChangeClipPlaneDir 0 $dir $angle
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    
    set lastDir $dir
    set lastAngle $angle
    
}

#-------------------------------------------------------------------------------
# .PROC DistanceClipPlanePlane
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DistanceClipPlanePlane {f distance} {
    VolumeTextureMapping(texturevolumeMapper) ChangeDist 0 $distance
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC SetClipPlaneType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SetClipPlaneType {} {
    global planetype Module
    if {[string compare $planetype "single"] == 0} {
    set currentType 0
    puts "SINGLE"
    } elseif {[string compare $planetype "double"] == 0} {
    set currentType 1
    puts "DOUBLE"
    } elseif {[string compare $planetype "cube"] == 0} {
    set currentType 2
    puts "CUBE"
    }
    VolumeTextureMapping(texturevolumeMapper) ChangeType $currentType
    for {set num 0} {$num < 3} {incr num} {
    VolumeTextureMapping(texturevolumeMapper) ChangeClipPlaneDir 0 $num 0
    }
    set fClip $Module(VolumeTextureMapping,fClip)
    set f $fClip.fRotation
    $f.sscale set 0
    
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC SpacingClipPlane
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SpacingClipPlane {f spacing} {
    VolumeTextureMapping(texturevolumeMapper) ChangeSpacing $spacing
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC ChangeVolumeDim
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ChangeVolumeDim {f} {
    global VolumeTextureMapping Module 
    if {[string compare $VolumeTextureMapping(currentVolume) "volume1"] == 0} {
    set thisVolume 0
    puts "VOLUME1"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume2"] == 0} {
    set thisVolume 1
    puts "VOLUME2"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume3"] == 0} {
    set thisVolume 2
    puts "VOLUME3"
    }
    #get volumedimensions for a specific dataset
    $f.dimx configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension $thisVolume 0]
    $f.dimy configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension $thisVolume 1]
    $f.dimz configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension $thisVolume 2]
}

#-------------------------------------------------------------------------------
# .PROC NumberOfPlanes
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc NumberOfPlanes {planes} {
    VolumeTextureMapping(texturevolumeMapper) SetNumberOfPlanes $planes
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}



#-------------------------------------------------------------------------------
# .PROC defaultPoints
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc defaultPoints {f} {
    #Skicka v„rden till c++ f÷r punkter
    global currentVolume newVolume colorLine opacityLine histLine colorMin colorMax .menuPoint .menuLine 
    set currentVolume 0 
    set newVolume 0
    
    set colorLine [$f create line 0 0 0 0 -width 2 -fill red]
    set opacityLine [$f create line 0 0 0 0 -width 2 -fill red]
    set histLine [$f create line 0 0 0 0 -width 2 -fill red]
    
    
    .menuPoint add command -label "Delete Point" -command "removePoint $f"
    
    .menuLine add command -label "Add Point" -command "addPoint $f"
    
    #createHistLine $f
    
    VolumeTextureMapping(texturevolumeMapper) ClearTF
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 4 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 4 40 40
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 4 120 120
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 4 230 230
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 4 255 255
    TFPoints $f 4
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 3 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 3 20 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 3 110 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 3 230 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 0 3 255 255
    TFPoints $f 3
    
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 4 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 4 40 40
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 4 120 120
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 4 230 230
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 4 255 255
    
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 3 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 3 20 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 3 110 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 3 230 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 1 3 255 255
    
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 4 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 4 40 40
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 4 120 120
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 4 230 230
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 4 255 255
    
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 3 0 0
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 3 20 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 3 110 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 3 230 200
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint 2 3 255 255
    TFInteractions $f  
}

#-------------------------------------------------------------------------------
# .PROC getPointX
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc getPointX {type} {
    global currentVolume pointx numPoints
    set numPoints [VolumeTextureMapping(texturevolumeMapper) GetNumPoint $currentVolume $type]
    
    for {set num 0} {$num <= [expr $numPoints]} {incr num} {
    set pointx($num) [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $num 0] 
    }
}

#-------------------------------------------------------------------------------
# .PROC getPointY
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc getPointY {type} {
    global currentVolume pointy numPoints
    for {set num 0} {$num <= [expr $numPoints]} {incr num} {
    set pointy($num) [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $num 1] 
    }
}

#-------------------------------------------------------------------------------
# .PROC ChangeVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ChangeVolume {f} {
    global currentVolume VolumeTextureMapping Module
    puts "CHANGE VOLUME"
    if {[string compare $VolumeTextureMapping(currentVolume) "volume1"] == 0} {
    set currentVolume 0
    puts "VOLUME1"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume2"] == 0} {
    set currentVolume 1
    puts "VOLUME2"
    } elseif {[string compare $VolumeTextureMapping(currentVolume) "volume3"] == 0} {
    set currentVolume 2
    puts "VOLUME3"
    }
    
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    puts "fTransfer: $fTransfer"
    set f $fTransfer.fTransform
    puts "f: $f"
    
    $f.row0.t00 delete 0 end
    $f.row0.t01 delete 0 end
    $f.row0.t02 delete 0 end
    $f.row0.t03 delete 0 end
    
    $f.row1.t10 delete 0 end
    $f.row1.t11 delete 0 end
    $f.row1.t12 delete 0 end
    $f.row1.t13 delete 0 end
    
    $f.row2.t20 delete 0 end
    $f.row2.t21 delete 0 end
    $f.row2.t22 delete 0 end
    $f.row2.t23 delete 0 end
    
    $f.row3.t30 delete 0 end
    $f.row3.t31 delete 0 end
    $f.row3.t32 delete 0 end
    $f.row3.t33 delete 0 end
    
    $f.row0.t00 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 0 0]
    $f.row0.t01 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 0 1]
    $f.row0.t02 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 0 2]
    $f.row0.t03 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 0 3]
    
    $f.row1.t10 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 1 0]
    $f.row1.t11 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 1 1]
    $f.row1.t12 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 1 2]
    $f.row1.t13 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 1 3]
    
    $f.row2.t20 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 2 0]
    $f.row2.t21 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 2 1]
    $f.row2.t22 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 2 2]
    $f.row2.t23 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 2 3]
    
    $f.row3.t30 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 3 0]
    $f.row3.t31 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 3 1]
    $f.row3.t32 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 3 2]
    $f.row3.t33 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement $currentVolume 3 3]
    
    
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer.fTFVolume1
    getColor $f
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer.fTFVolume1.graph.canvas1
    #createHistLine $f
    TFInteractions $f    
    
}


#-------------------------------------------------------------------------------
# .PROC TFPoints
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TFPoints {f type} {
    global currentVolume 
    global pointx pointy numPoints point
    getPointX $type
    getPointY $type
    
    if {$type == 4} {
    set color "grey"
    getPointX 0
    getPointY 0
    } else {
    set color "black"
    getPointX $type
    getPointY $type
    }
    
    for {set i 0} {$i <= [expr $numPoints]} {incr i} {
    set x [expr {$pointx($i)*210/255+4}]
       set y [expr {(100+4) - $pointy($i)*100/255}]
        set item [$f create oval [expr {$x-3}] [expr {$y-3}] \
              [expr {$x+3}] [expr {$y+3}] -width 1 -outline black \
              -fill $color]
    $f addtag point withtag $item        
    }
    
    
    

}

#-------------------------------------------------------------------------------
# .PROC TFInteractions
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TFInteractions {f} {
    global colorLine point currentVolume .menuPoint .menuLine opacityLine
    
    $f delete point
    createTFLine $f 4
    createTFLine $f 3
    TFPoints $f 4
    TFPoints $f 3
    
    $f bind point <3> "menuPopPoint $f %x %y %X %Y"
    $f bind myLine <3> "menuPopLine $f %x %y %X %Y"
    $f bind point <Any-Enter> "holdOverPoint $f %x %y"
    $f bind myLine <Any-Enter> "holdOverLine $f %x %y"
    $f bind point <Any-Leave> "leavePoint $f"
    $f bind myLine <Any-Leave> "leaveLine $f"
    $f bind point <ButtonRelease-1> "releasePoint $currentVolume $f %x %y"
    $f bind point <1> "clickOnPoint $currentVolume $f %x %y"
    bind $f <B1-Motion> "movePoint  $f %x %y"
    

}

#-------------------------------------------------------------------------------
# .PROC menuPopLine
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc menuPopLine {f x y x2 y2} {
    global newPointPosX newPointPosY type
    set vx [expr $x*255/210+4]
    set vy [expr ((100+4)-$y)*255/100]
set newPointPosX $vx
    set newPointPosY $vy
    puts "newPointPosX: $newPointPosX"
    puts "newPointPosY: $newPointPosY"
    tk_popup .menuLine $x2 $y2
}

#-------------------------------------------------------------------------------
# .PROC menuPopPoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc menuPopPoint {f x y x2 y2} {
    global newPointPosX newPointPosY type
    set vx [expr $x*255/210+4]
    set vy [expr ((100+4)-$y)*255/100]
    set newPointPosX $vx
    set newPointPosY $vy
    puts "newPointPosX: $newPointPosX"
    puts "newPointPosY: $newPointPosY"
    tk_popup .menuPoint $x2 $y2
}

#-------------------------------------------------------------------------------
# .PROC holdOverLine
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc holdOverLine {f x y} {
    global type lineColor
    set lineColor [$f itemcget current -fill]
    $f itemconfig current -fill yellow
    
    if {[string compare $lineColor "grey"] == 0} {
    set type 4
    } elseif {[string compare $lineColor "black"] == 0} {
    set type 3
    }
    
}

#-------------------------------------------------------------------------------
# .PROC leaveLine
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc leaveLine {f} {
    global lineColor
    $f itemconfig current -fill $lineColor
}

#-------------------------------------------------------------------------------
# .PROC createHistLine
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc createHistLine {f} {
    global histLine coords currentVolume Module
    puts "HISTLINE"
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer.fTFVolume1.graph.canvas1
    $f delete $histLine
    lappend coords 0
    unset coords
    for {set i 0} {$i < 255} {incr i} {
    lappend coords [expr {$i*210/255+4}]
    lappend coords [expr {(100+4)-0*100/255}]
    lappend coords [expr {$i*210/255+4}]
    lappend coords [expr {(100+4)-[VolumeTextureMapping(texturevolumeMapper) GetHistValue $currentVolume $i]*100/255}]
    lappend coords [expr {$i*210/255+4}]
    lappend coords [expr {(100+4)-0*100/255}]
    }
    set histLine [$f create line $coords -width 3 -fill "red"]
}

#-------------------------------------------------------------------------------
# .PROC createTFLine
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc createTFLine {f type} {
    #ta bort de linjer som finns och rita ut nya mha v„rden fr…n c++
    global colorLine numPoints pointx pointy coords point opacityLine
    
    if {$type == 4} {
    set color "grey"
    getPointX 0
    getPointY 0
    $f delete $colorLine
    } else {
    set color "black"
    getPointX $type
    getPointY $type
    $f delete $opacityLine
    }
    
    lappend coords 0
    #puts "numPoints: $numPoints"
    #puts "coords: $coords"
    
    unset coords
    for {set i 0} {$i <= [expr $numPoints]} {incr i} {
    lappend coords [expr {$pointx($i)*210/255+4}]
    lappend coords [expr {(100+4) - $pointy($i)*100/255}]     
    
    }
    
    if {$type == 4} {
    set colorLine [$f create line $coords -width 3 -fill $color]
    $f addtag myLine withtag $colorLine 
    } else {
    set opacityLine [$f create line $coords -width 3 -fill $color]
    $f addtag myLine withtag $opacityLine 
    }
    
    $f raise point 
}

#-------------------------------------------------------------------------------
# .PROC holdOverPoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc holdOverPoint {f x y} {
    global pointColor type pointPos plot currentVolume
    set pointColor [$f itemcget current -fill]
    $f itemconfig current -fill yellow    
    if {[string compare $pointColor "grey"] == 0} {
    set type 4
    } elseif {[string compare $pointColor "black"] == 0} {
    set type 3
    } else {
    set type 3        
} 
    set plot(lastX) $x
    set plot(lastY) $y
    set vx [expr ($x-4)*255/210]
    set vy [expr (100+4)*255/100]
    #inom ovalen
    set boundX [expr 8*255/210]
    set boundY [expr 8*255/100]
    set pointPos [VolumeTextureMapping(texturevolumeMapper) GetArrayPos $currentVolume $type $vx $vy $boundX $boundY]
}

#-------------------------------------------------------------------------------
# .PROC leavePoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc leavePoint {f} {
    global pointColor
    $f itemconfig current -fill $pointColor
}

#-------------------------------------------------------------------------------
# .PROC setValues
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc setValues {f x y} {
    global thisx thisy .menuLine
    set thisx $x
    set thisy $y
    tk_popup .menuLine $x $y
}


#-------------------------------------------------------------------------------
# .PROC addPoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc addPoint {f} {
    global currentVolume type newPointPosX newPointPosY
    #kolla vilken volym vi tillh÷r och vilken f„rgà 
    #anropa c++ och l„gg till nya punktv„rden
    #global thisx thisy type pointColor
    
    
    set vx [expr $newPointPosX*255/210+4]
    set vy [expr ((100+4)-$newPointPosY)*255/100]
    
    set neighbourMinPointPos [VolumeTextureMapping(texturevolumeMapper) GetNeighbourMin $currentVolume $type $vx]
    set neighbourMaxPointPos [VolumeTextureMapping(texturevolumeMapper) GetNeighbourMax $currentVolume $type $vx]
    set neighbourMinX [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $neighbourMinPointPos 0] 
    #set neighbourMinY [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $neighbourMinPointPos 1] 
    set neighbourMaxX [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $neighbourMaxPointPos 0] 
    set neighbourMaxY [VolumeTextureMapping(texturevolumeMapper) GetPoint $currentVolume $type $neighbourMaxPointPos 1] 
    
    #VolumeTextureMapping(texturevolumeMapper) AddTFPoint $currentVolume $type [expr $neighbourMinX+(($neighbourMaxX - $neighbourMinX)/2)] $neighbourMaxY
    VolumeTextureMapping(texturevolumeMapper) AddTFPoint $currentVolume $type $newPointPosX $newPointPosY
    TFInteractions $f
}


#-------------------------------------------------------------------------------
# .PROC removePoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc removePoint {f} {
    global pointPos type currentVolume newPointPosX newPointPosY numPoints
    #    getArrayPos I c++. Ta bort pukten och sortera I c++
    set boundX [expr 10*255/210]
    set boundY [expr 10*255/100]
    set pointPos [VolumeTextureMapping(texturevolumeMapper) GetArrayPos $currentVolume $type $newPointPosX $newPointPosY $boundX $boundY]
    if {($pointPos != -1) && ($pointPos != 0) && ($pointPos != $numPoints)} {
    
    puts "pointPos: $pointPos"
    
    VolumeTextureMapping(texturevolumeMapper) RemoveTFPoint $currentVolume $type $pointPos
    TFInteractions $f
    }
}


#-------------------------------------------------------------------------------
# .PROC clickOnPoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc clickOnPoint {volume f x y} {
    global plot pointPos type pointColor currentVolume
    # set volume $f
    
    $f dtag selected
    $f addtag selected withtag current
    $f raise current 
    #raise- change stack order #current -whatever object is over the mouse
    set plot(lastX) $x
    set plot(lastY) $y
    set vx [expr ($x-4)*255/210]
    set vy [expr ((100+4)-$y)*255/100]
    #inom ovalen
    set boundX [expr 8*255/210]
    set boundY [expr 8*255/100]
    set pointPos [VolumeTextureMapping(texturevolumeMapper) GetArrayPos $currentVolume $type $vx $vy $boundX $boundY]
    #om pointPos = -1 set error msg...
}

#-------------------------------------------------------------------------------
# .PROC releasePoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc releasePoint {volume f x y} {
    global pointPos type currentVolume plot numPoints
    $f dtag selected
    
    if { [expr ($x-4)*255/210] < 0} {
    set x [expr {0*210/255+4}]
    } elseif {[expr ($x-4)*255/210] > 255} {
    set x [expr {255*210/255+4}]
    }
    if { [expr ((100+4)-$y)*255/100] < 0} {
    set y [expr {(100+4) - 0*100/255}]
    } elseif {[expr ((100+4)-$y)*255/100] > 255} {
    set y [expr {(100+4) - 255*100/255}]
    }
    if {$pointPos == 0}    {
    set x [expr {0*210/255+4}]
    
    } elseif {$pointPos == $numPoints} {
    set x [expr {255*210/255+4}]
    
    }
    set vx [expr ($x-4)*255/210]
    set vy [expr ((100+4)-$y)*255/100]
    VolumeTextureMapping(texturevolumeMapper) ChangeTFPoint $volume $type $pointPos $vx $vy        
    
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    set pointPos -1
    TFInteractions $f
}

#-------------------------------------------------------------------------------
# .PROC movePoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc movePoint {f x y} {
    global currentVolume pointPos type numPoints
    global plot pointPos colorLine
    if {$pointPos != -1} {
    
    if { [expr ($x-4)*255/210] < 0} {
        set x [expr {0*210/255+4}]
    } elseif {[expr ($x-4)*255/210] > 255} {
        set x [expr {255*210/255+4}]
    }
    if { [expr ((100+4)-$y)*255/100] < 0} {
        set y [expr {(100+4) - 0*100/255}]
    } elseif {[expr ((100+4)-$y)*255/100] > 255} {
        set y [expr {(100+4) - 255*100/255}]
    }
    
    if {$pointPos == 0} {
        set x [expr {0*210/255+4}]            
    } elseif {$pointPos == $numPoints} {
        set x [expr {255*210/255+4}]                
    }
    $f move selected [expr {$x-$plot(lastX)}] [expr {$y-$plot(lastY)}]
            set plot(lastX) $x
    set plot(lastY) $y    
    set vx [expr ($x-4)*255/210]
    set vy [expr ((100+4)-$y)*255/100]
    
    VolumeTextureMapping(texturevolumeMapper) ChangeTFPoint $currentVolume $type $pointPos $vx $vy
    
    createTFLine $f $type
    
    }
}

#-------------------------------------------------------------------------------
# .PROC ChangeTransformMatrix
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ChangeTransformMatrix {} {
    global t00 t01 t02 t03 t10 t11 t12 t13 t20 t21 t22 t23 t30 t31 t32 t33 currentVolume
    VolumeTextureMapping(texturevolumeMapper) UpdateTransformMatrix $currentVolume $t00 $t01 $t02 $t03 $t10 $t11 $t12 $t13 $t20 $t21 $t22 $t23 $t30 $t31 $t32 $t33 
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
}


#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingBuildVTK {} {
    global VolumeTextureMapping Volume
    
    vtkVolumeTextureMapper3D VolumeTextureMapping(texturevolumeMapper)
    global numVolumes
    set numVolumes 0
    
    
    VolumeTextureMapping(texturevolumeMapper) SetNumberOfPlanes 128
    vtkVolume VolumeTextureMapping(volume)
    
    
    #vtkImageCast filter casts the input type to match the output type in the image processing pipeline
    vtkImageCast VolumeTextureMapping(imageCast1)
    vtkImageCast VolumeTextureMapping(imageCast2)
    vtkImageCast VolumeTextureMapping(imageCast3)
    

}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingRefresh
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingRefresh {} {
    global VolumeTextureMapping Slice Volume Module currentVolume
    global numVolumes View
    
    set boxSize 128
    VolumeTextureMapping(texturevolumeMapper) SetCounter 0
    set boxSize [expr $View(fov)]
    puts "boxSize: $boxSize"
    VolumeTextureMapping(texturevolumeMapper) SetBoxSize $boxSize
    
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 0 0
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 1 0
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 2 0
    
    if {$VolumeTextureMapping(idOriginal1) != $Volume(idNone)} {
    set numVolumes [expr $numVolumes+1]
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 0 1
    } 
    if {$VolumeTextureMapping(idOriginal2) != $Volume(idNone)} {
    set numVolumes [expr $numVolumes+1]
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 1 1
    }
    if {$VolumeTextureMapping(idOriginal3) != $Volume(idNone)} {
    set numVolumes [expr $numVolumes+1]
    VolumeTextureMapping(texturevolumeMapper) SetEnableVolume 2 1
    }
    
    VolumeTextureMapping(texturevolumeMapper) SetNumberOfVolumes $numVolumes
    set numVolumes 0
    

    ##-----------VOLUME1
   
    if {$VolumeTextureMapping(idOriginal1) != $Volume(idNone)} {

    set currentVolume 0
    VolumeTextureMapping(imageCast1) SetInput [Volume($VolumeTextureMapping(idOriginal1),vol) GetOutput]
    VolumeTextureMapping(imageCast1) SetOutputScalarTypeToUnsignedShort    
    VolumeTextureMapping(volume) SetMapper VolumeTextureMapping(texturevolumeMapper)
    VolumeTextureMapping(texturevolumeMapper) SetInput [VolumeTextureMapping(imageCast1) GetOutput]
    
    if {[info commands t1] == ""} {
        vtkTransform t1
    }
    t1 Identity
    t1 PreMultiply
    t1 SetMatrix [Volume($VolumeTextureMapping(idOriginal1),node) GetWldToIjk]
    t1 Inverse
    scan [Volume($VolumeTextureMapping(idOriginal1),node) GetSpacing] "%g %g %g" res_x res_y res_z
        
    t1 PreMultiply
    t1 Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] [expr 1.0 / $res_z]
    
    VolumeTextureMapping(volume) SetUserMatrix [t1 GetMatrix]
    t1 Delete
    }
    VolumeTextureMappingCameraMotion
  
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    
    ##--------------VOLUME2
    if {$VolumeTextureMapping(idOriginal2) != $Volume(idNone)} {
     set currentVolume 1
    VolumeTextureMapping(imageCast2) SetInput [Volume($VolumeTextureMapping(idOriginal2),vol) GetOutput]
    VolumeTextureMapping(imageCast2) SetOutputScalarTypeToUnsignedShort    
    VolumeTextureMapping(volume) SetMapper VolumeTextureMapping(texturevolumeMapper)
    VolumeTextureMapping(texturevolumeMapper) SetInput [VolumeTextureMapping(imageCast2) GetOutput]

    if {[info commands t1] == ""} {
        vtkTransform t1
    }
    t1 Identity
    t1 PreMultiply
    t1 SetMatrix [Volume($VolumeTextureMapping(idOriginal2),node) GetWldToIjk]
    t1 Inverse
    scan [Volume($VolumeTextureMapping(idOriginal2),node) GetSpacing] "%g %g %g" res_x res_y res_z
    t1 PreMultiply
    t1 Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] [expr 1.0 / $res_z]
    
    VolumeTextureMapping(volume) SetUserMatrix [t1 GetMatrix]
    t1 Delete
    }
    VolumeTextureMappingCameraMotion
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    ##--------------VOLYM3
    if {$VolumeTextureMapping(idOriginal3) != $Volume(idNone)} {
 set currentVolume 2
    VolumeTextureMapping(imageCast3) SetInput [Volume($VolumeTextureMapping(idOriginal3),vol) GetOutput]
    VolumeTextureMapping(imageCast3) SetOutputScalarTypeToUnsignedShort
    VolumeTextureMapping(volume) SetMapper VolumeTextureMapping(texturevolumeMapper)
       VolumeTextureMapping(texturevolumeMapper) SetInput [VolumeTextureMapping(imageCast3) GetOutput]
    
    if {[info commands t1] == ""} {
        vtkTransform t1
    }
    t1 Identity
    t1 PreMultiply
    t1 SetMatrix [Volume($VolumeTextureMapping(idOriginal3),node) GetWldToIjk]
    t1 Inverse
    scan [Volume($VolumeTextureMapping(idOriginal3),node) GetSpacing] "%g %g %g" res_x res_y res_z
    t1 PreMultiply
    t1 Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] [expr 1.0 / $res_z]
    
    
    VolumeTextureMapping(volume) SetUserMatrix [t1 GetMatrix]
    t1 Delete
    }
VolumeTextureMappingCameraMotion
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer.fTFVolume1.graph.canvas1
    #createHistLine $f
    TFInteractions $f
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll
    
    set fTransfer $Module(VolumeTextureMapping,fTransfer)
    set f $fTransfer.fTransform
    
    $f.row0.t00 delete 0 end
    $f.row0.t01 delete 0 end
    $f.row0.t02 delete 0 end
    $f.row0.t03 delete 0 end
    
    $f.row1.t10 delete 0 end
    $f.row1.t11 delete 0 end
    $f.row1.t12 delete 0 end
    $f.row1.t13 delete 0 end
    
    $f.row2.t20 delete 0 end
    $f.row2.t21 delete 0 end
    $f.row2.t22 delete 0 end
    $f.row2.t23 delete 0 end
    
    $f.row3.t30 delete 0 end
    $f.row3.t31 delete 0 end
    $f.row3.t32 delete 0 end
    $f.row3.t33 delete 0 end

    $f.row0.t00 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 0 0]
    $f.row0.t01 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 0 1]
    $f.row0.t02 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 0 2]
    $f.row0.t03 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 0 3]
    
    $f.row1.t10 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 1 0]
    $f.row1.t11 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 1 1]
    $f.row1.t12 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 1 2]
    $f.row1.t13 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 1 3]
    
    $f.row2.t20 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 2 0]
    $f.row2.t21 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 2 1]
    $f.row2.t22 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 2 2]
    $f.row2.t23 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 2 3]
    
    $f.row3.t30 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 3 0]
    $f.row3.t31 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 3 1]
    $f.row3.t32 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 3 2]
    $f.row3.t33 insert 0 [VolumeTextureMapping(texturevolumeMapper) GetTransformMatrixElement 0 3 3]
    
    set fSettings $Module(VolumeTextureMapping,fSettings)
    set f $fSettings.fRefreshSettings.fDimension
    
    $f.dimx configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension 0 0]
    $f.dimy configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension 0 1]
    $f.dimz configure -text [VolumeTextureMapping(texturevolumeMapper) GetTextureDimension 0 2]
    
}


#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingUpdateTransform
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingCameraMotion {} {
global View currentVolume VolumeTextureMapping Volume

    set id $VolumeTextureMapping(idOriginal[expr $currentVolume +1])
    set extent [[Volume($id,vol) GetOutput] GetExtent]

    set x2 [expr [lindex $extent 1] + 1] 
    set x4 [expr [lindex $extent 3] + 1] 
    set x6 [expr [lindex $extent 5]+ 1] 

    #puts "get scan order [Volume($id,node) GetScanOrder]"

    set fp [$View(viewCam) GetFocalPoint]
    set xFP [lindex $fp 0]        
    set yFP [lindex $fp 1]
    set zFP [lindex $fp 2]
    VolumeTextureMapping(texturevolumeMapper) SetOrigin $xFP $yFP $zFP

    vtkMatrix4x4 resultMatrix
    vtkMatrix4x4 DisplaceMatrix
    vtkMatrix4x4 IjkToWld

    resultMatrix Identity
    DisplaceMatrix Identity
 
    #get the local and global transformation (instead of geting IJK->RAS and RAS->WLD)
    IjkToWld DeepCopy [Volume($id,node) GetWldToIjk]
    #invert since we want to go from IJK->WLD instead of WLD->IJK
    IjkToWld Invert
    
    #displace since the coordinates in the volume renderer goes from -extent/2 -> extent/2 instead of 0 -> extent
    DisplaceMatrix  DeepCopy \
    1  0  0  [expr $x2/2] \
    0  1  0  [expr $x4/2] \
    0  0  1  [expr $x6/2] \
    0  0  0  1    
    
    
    resultMatrix Multiply4x4 DisplaceMatrix resultMatrix resultMatrix
    resultMatrix Multiply4x4 IjkToWld resultMatrix resultMatrix
    
    #update the transformmatrix in the volume renerer
    VolumeTextureMapping(texturevolumeMapper) UpdateTransformMatrix 0 resultMatrix
    
    
    DisplaceMatrix Delete
    IjkToWld Delete
    resultMatrix Delete
   
 
    VolumeTextureMapping(texturevolumeMapper) Update
    RenderAll

}



#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingEnter {} {
    global VolumeTextureMapping Volume Slice Module
    
    # If the Original is None, then select what's being displayed,
    # otherwise the first volume in the mrml tree.
    
    if {$VolumeTextureMapping(idOriginal1) == $Volume(idNone)} {
        set v [[[Slicer GetBackVolume $Slice(activeID)] GetMrmlNode] GetID]
        if {$v == $Volume(idNone)} {
            set v [lindex $Volume(idList) 0]
        }
        if {$v != $Volume(idNone)} {
            VolumeTextureMappingSetOriginal1 $v
        }
    }
    
    pushEventManager $VolumeTextureMapping(eventManager)
    
    
    
    if {$VolumeTextureMapping(volumeVisible) == "0"} {
    
    foreach r $Module(Renderers) {
        $r AddVolume VolumeTextureMapping(volume)
        
    }
    }
    set VolumeTextureMapping(volumeVisible) "1"
    
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingExit
# Called when this module is exitedVolume($VolumeTextureMapping(idOriginal),node) GetPosition by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingExit {} {
    #  global VolumeTextureMapping Module Volume
    
    # if {$VolumeTextureMapping(hideOnExit)} {
    # foreach r $Module(Renderers) {
    #     $r RemoveVolume VolumeTextureMapping(volume)
    # }   
    # VolumeTextureMapping(imageCast1) SetInput [Volume($Volume(idNone),vol) GetOutput]
    #    VolumeTextureMapping(imageCast2) SetInput [Volume($Volume(idNone),vol) GetOutput]
    #    VolumeTextureMapping(imageCast3) SetInput [Volume($Volume(idNone),vol) GetOutput]
    
    #   set VolumeTextureMapping(volumeVisible) "0"
    
    #   RenderAll
    #   }
    
    #   popEventManager
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingUpdateMRML {} {
    global Volume VolumeTextureMapping
    
    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $VolumeTextureMapping(idOriginal1)] == -1} {
        VolumeTextureMappingSetOriginal1 $n
        VolumeTextureMappingSetOriginal2 $n
        VolumeTextureMappingSetOriginal3 $n
    }
    
    # Original1 Volume menu
    #---------------------------------------------------------------------------
    set m $VolumeTextureMapping(mOriginal1)
    $m delete 0 end
    foreach v $Volume(idList) {
        $m add command -label [Volume($v,node) GetName] -command \
            "VolumeTextureMappingSetOriginal1 $v; RenderAll"
    }
    # Original2 Volume menu
    #---------------------------------------------------------------------------
    set m $VolumeTextureMapping(mOriginal2)
    $m delete 0 end
    foreach v $Volume(idList) {
        $m add command -label [Volume($v,node) GetName] -command \
            "VolumeTextureMappingSetOriginal2 $v; RenderAll"
    }
    # Original3 Volume menu
    #---------------------------------------------------------------------------
    set m $VolumeTextureMapping(mOriginal3)
    $m delete 0 end
    foreach v $Volume(idList) {
        $m add command -label [Volume($v,node) GetName] -command \
            "VolumeTextureMappingSetOriginal3 $v; RenderAll"
    }
    
    
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingSetOriginal1
#   Sets which volume is used in this module.
#   Called from VolumeTextureMappingUpdateMRML and VolumeTextureMappingEnter.
# .ARGS
#   v    Volume ID
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingSetOriginal1 {v} {
    global VolumeTextureMapping Volume
    
    set VolumeTextureMapping(idOriginal1) $v
    
    # Change button text
    $VolumeTextureMapping(mbOriginal1) config -text [Volume($v,node) GetName]
    
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingSetOriginal2
#   Sets which volume is used in this module.
#   Called from VolumeTextureMappingUpdateMRML and VolumeTextureMappingEnter.
# .ARGS
#   v    Volume ID
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingSetOriginal2 {v} {
    global VolumeTextureMapping Volume
    
    set VolumeTextureMapping(idOriginal2) $v
    
    # Change button text
    $VolumeTextureMapping(mbOriginal2) config -text [Volume($v,node) GetName]
    
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingSetOriginal3
#   Sets which volume is used in this module.
#   Called from VolumeTextureMappingUpdateMRML and VolumeTextureMappingEnter.
# .ARGS
#   v    Volume ID
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingSetOriginal3 {v} {
    global VolumeTextureMapping Volume
    
    set VolumeTextureMapping(idOriginal3) $v
    
    
    # Change button text
    $VolumeTextureMapping(mbOriginal3) config -text [Volume($v,node) GetName]
}




#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingStorePresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingStorePresets {p} {
    global Preset VolumeTextureMapping Volume
    
    set Preset(VolumeTextureMapping,$p,idOriginal1) $VolumeTextureMapping(idOriginal1)
    set Preset(VolumeTextureMapping,$p,idOriginal2) $VolumeTextureMapping(idOriginal2)
    set Preset(VolumeTextureMapping,$p,idOriginal3) $VolumeTextureMapping(idOriginal3)
    set Preset(VolumeTextureMapping,$p,hideOnExit) $VolumeTextureMapping(hideOnExit)
    
}

#-------------------------------------------------------------------------------
# .PROC VolumeTextureMappingRecallPresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeTextureMappingRecallPresets {p} {
    global Preset VolumeTextureMapping
    
    set VolumeTextureMapping(idOriginal1) $Preset(VolumeTextureMapping,$p,idOriginal1)
    set VolumeTextureMapping(idOriginal2) $Preset(VolumeTextureMapping,$p,idOriginal2)
    set VolumeTextureMapping(idOriginal3) $Preset(VolumeTextureMapping,$p,idOriginal3)
    set VolumeTextureMapping(hideOnExit) $Preset(VolumeTextureMapping,$p,hideOnExit)

}


