#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        View.tcl
# PROCEDURES:  
#   ViewInit
#   ViewBuildGUI
#   ViewBuildVTK
#   ViewSetMovieFileType
#   ViewBuildLightsVTK
#   ViewSetLightIntensity
#   ViewUpdateLightIntensity
#   ViewSwitchLightKit
#==========================================================================auto=
#
# Use this Module for Changing the Viewing Windows sizes
# and relative positions.
#
#
# KNOWN BUG: Turning off the Close-up window doesn't actually turn it off.
#            It prevents the window from updating, but the close-up window
#            Still gets initialized upon entering a slice.
#
#

#-------------------------------------------------------------------------------
# .PROC ViewInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewInit {} {

    global View Module

    # Define Tabs
    set m View
    set Module($m,row1List) "Help View Fog Lights"
    set Module($m,row1Name) "Help View Fog Lights"
    set Module($m,row1,tab) View

    # Module Summary Info
    set Module($m,overview) "Settings for 3D View, stereo, make movies."

    # Define Procedures
    set Module($m,procGUI) ViewBuildGUI
    set Module($m,procVTK) ViewBuildVTK

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.31 $} {$Date: 2002/10/26 19:42:17 $}]

    set View(movie) 0
    set View(movieDirectory) "/tmp"
    set View(movieFrame) 1
    set View(movieFileType) "PNG"
}

#-------------------------------------------------------------------------------
# .PROC ViewBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewBuildGUI {} {

    global Gui View Module Fog

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # View
    #   Size     : Size of window in 3D mode
    #   Closeup  : Turns on/off Close-up Window
    #   Stereo   : Turns on/off Stereo 3D
    #
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
<UL>
<LI><B>Size</B> If you want to save the 3D view as an image on disk 
(using the <B>Save 3D</B> command from the <B>File</B> menu), first
set the size of the window to the desired size of the saved image.
<BR><LI><B>Closeup</B> The Closeup window magnifies the slice window
around the mouse cursor.  This option is not presently available on PCs.
<BR><LI><B>Stereo</B> The Stereo mode allows viewing the 3D window in
3D if you have Red/Blue glasses.  If you have crystal eyes glasses,
then select the CrystalEyes stereo mode.
<BR><LI><B>Background Color</B> When you threshold graylevel volumes,
the view will look better if the background is black.
<BR><LI><B>Record Movie</B> When this button is activated the 3D view
is recorded as a .ppm file each time it is re-rendered. By default, each
frame is recorded as /tmp/movieNNNN.ppm. These frames can be converted into
a movie using various utilities. One way is to use <I>convert</I> which accepts
a command line like: <I>convert movie*.ppm movie.mpg</I> to create a movie
called <I>movie.mpg</I>.
<BR><LI><B>Fog</B> The fog allows to mix the  color of the 3D object
with the background depending on its distance to the camera.
The transformation is linear with two parameters <I>Start</I> and <I>End</I>.
The value 0 is the closest position of the bounding box and
the value 1 is the furthest position of the bounding box.
<BR><LI><B>Lights</B>The Lights panel lets you select between the default,
fairly ugly 'Blair Witch'-type headlight lighting and a more pleasing multiple
light arrangement using the vtkLightKit. The panel also allows some other 
light parameters (e.g., overall intensity) to be adjusted.
</UL>
"
    regsub -all "\n" $help { } help
    MainHelpApplyTags View $help
    MainHelpBuildGUI View

    #-------------------------------------------
    # View frame
    #-------------------------------------------
    set fView $Module(View,fView)
    set f $fView

    frame $f.fSize    -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fBg      -bg $Gui(activeWorkspace)
    frame $f.fStereo  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fCloseup -bg $Gui(activeWorkspace)
    frame $f.fMovie   -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fMovieSlices   -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.fSize $f.fBg $f.fCloseup $f.fStereo $f.fMovie $f.fMovieSlices\
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # View->Bg Frame
    #-------------------------------------------
    set f $fView.fBg
    
    eval {label $f.l -text "Background: "} $Gui(WLA)
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach value "Blue Black Midnight White" width "5 6 9 6" {
        eval {radiobutton $f.r$value -width $width \
            -text "$value" -value "$value" -variable View(bgName) \
            -indicatoron 0 -command "MainViewSetBackgroundColor; Render3D"} $Gui(WCA)
        pack $f.r$value -side left -padx 0 -pady 0
    }
    
    #-------------------------------------------
    # View->Closeup Frame
    #-------------------------------------------
    set f $fView.fCloseup
    
    eval {label $f.lCloseup -text "Closeup Window: "} $Gui(WLA)
    pack $f.lCloseup -side left -padx $Gui(pad) -pady 0

    foreach value "On Off" width "4 4" {
        eval {radiobutton $f.rCloseup$value -width $width \
            -text "$value" -value "$value" -variable View(closeupVisibility) \
            -indicatoron 0 -command "MainViewSetWelcome Welcome"} $Gui(WCA)
        pack $f.rCloseup$value -side left -padx 0 -pady 0
    }
    
    #-------------------------------------------
    # View->Size Frame
    #-------------------------------------------
    set f $fView.fSize
    
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
       pack $f.fTitle $f.fBtns -side top -pady 5

    eval {label $f.fTitle.lTitle -text "Window Size in 3D Mode:"} $Gui(WLA)
    pack $f.fTitle.lTitle -side left -padx $Gui(pad) -pady 0

    eval {label $f.fBtns.lW -text "Width:"} $Gui(WLA)
    eval {label $f.fBtns.lH -text "Height:"} $Gui(WLA)
    eval {entry $f.fBtns.eWidth -width 5 -textvariable View(viewerWidth)} $Gui(WEA)
    eval {entry $f.fBtns.eHeight -width 5 -textvariable View(viewerHeight)} $Gui(WEA)
        bind $f.fBtns.eWidth  <Return> {MainViewerSetMode}
        bind $f.fBtns.eHeight <Return> {MainViewerSetMode}
    pack $f.fBtns.lW $f.fBtns.eWidth $f.fBtns.lH $f.fBtns.eHeight \
        -side left -padx $Gui(pad)

    #-------------------------------------------
    # View->Stereo Frame
    #-------------------------------------------
    set f $fView.fStereo

    frame $f.fStereoType -bg $Gui(activeWorkspace)
    frame $f.fStereoOn -bg $Gui(activeWorkspace)
       pack $f.fStereoType $f.fStereoOn -side top -pady 5

    #-------------------------------------------
    # View->Stereo->StereoType Frame
    #-------------------------------------------
    set f $fView.fStereo.fStereoType

    foreach value "RedBlue CrystalEyes Interlaced" {
        eval {radiobutton $f.r$value \
            -text "$value" -value "$value" \
            -variable View(stereoType) \
            -indicatoron 0 \
            -command "MainViewSetStereo; Render3D"} \
            $Gui(WCA)
        pack $f.r$value -side left -padx 0 -pady 0
    }

    #-------------------------------------------
    # View->Stereo->StereoOn Frame
    #-------------------------------------------
    set f $fView.fStereo.fStereoOn
    
    # Stereo button
    eval {checkbutton $f.cStereo \
        -text "Stereo" -variable View(stereo) -width 6 \
        -indicatoron 0 -command "MainViewSetStereo; Render3D"} $Gui(WCA)
 
    pack $f.cStereo -side top -padx 0 -pady 2

    #-------------------------------------------
    # View->Movie Frame
    #-------------------------------------------
    




        set f $fView.fMovie
        eval {checkbutton $f.cMovie -text "Save View" -variable View(movie) \
        -width 22 -indicatoron 0 } $Gui(WCA)
    TooltipAdd $f.cMovie "When this button is selected, you can save the 
current View (with or without the slice windows) by 
left-clicking in 3D View. If you leave this button selected, 
a frame will be saved everytime the 3D View is rendered "
    
    eval {checkbutton $f.cMovieSlices -text "with slice windows" -variable View(movieSlices) -indicatoron 1 } $Gui(WCA)
    TooltipAdd $f.cMovieSlices "When this button is selected, the frames saved will contain the 2D window slices"

    eval {label $f.lFrame -text "Next frame #:"} $Gui(WLA)
    eval {entry $f.eFrame -width 6 -textvariable View(movieFrame)} $Gui(WEA)
 
    eval {label $f.lPrefix -text "Directory:"} $Gui(WLA)

    eval {entry $f.ePrefix -width 16 -textvariable View(movieDirectory)} $Gui(WEA)

    grid $f.cMovie -columnspan 2 -padx $Gui(pad) -pady $Gui(pad)
    grid $f.cMovieSlices -columnspan 2 -padx $Gui(pad) -pady $Gui(pad)
    grid $f.lFrame $f.eFrame -sticky w -padx $Gui(pad) -pady $Gui(pad)
    grid $f.lPrefix $f.ePrefix -sticky w -padx $Gui(pad) -pady $Gui(pad)
        grid configure $f.lFrame -sticky e
        grid configure $f.lPrefix -sticky e

    # File type
    eval {label $f.lFile -text "File type:"} $Gui(WLA)
    eval {menubutton $f.mbFile -text $View(movieFileType) -width 10 -menu $f.mbFile.m} \
        $Gui(WMBA)
    grid $f.lFile $f.mbFile -sticky w -padx $Gui(pad) -pady $Gui(pad)
    grid configure $f.lFile -sticky e

    eval {menu $f.mbFile.m} $Gui(WMA)
    foreach item "BMP JPEG PNG PNM PostScript TIFF" {
        $f.mbFile.m add command -label $item -command "ViewSetMovieFileType $item"
    }

    #-------------------------------------------
    # Fog frame
    #-------------------------------------------

    FogBuildGui $Module(View,fFog)

    #-------------------------------------------
    # Lights frame
    #-------------------------------------------
    set fLights $Module(View,fLights)
    
    set f $fLights

    frame $f.fLightMode -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fLightIntensity -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.fLightMode $f.fLightIntensity -side top -pady 5 -fill x -padx 5

    set f $fLights.fLightMode
    eval {label $f.lLightMode -text "Light Mode"} $Gui(WLA)
    eval {radiobutton $f.rbLK -text "LightKit (better)" \
              -variable View(LightModeIndicator) -value "LightKit" \
              -command "ViewSwitchLightKit 1"} $Gui(WRA)
    TooltipAdd $f.rbLK "Light the model more naturally with three lights."

    eval {radiobutton $f.rbHL -text "Headlight (VTK default)" \
              -variable View(LightModeIndicator) -value "Headlight" \
              -command {ViewSwitchLightKit 0}} $Gui(WRA)
    TooltipAdd $f.rbHL "Light the model with a headlight (a light at the camera)."

    pack $f.lLightMode -side top -anchor w -padx $Gui(pad) -pady $Gui(pad)
    pack $f.rbLK -side top -anchor w -padx $Gui(pad) -pady $Gui(pad)
    pack $f.rbHL -side top -anchor w -padx $Gui(pad) -pady $Gui(pad)

    set f $fLights.fLightIntensity
     eval {scale $f.sIntensity -from 0.0 -to 2.0 -showvalue true \
               -label "Light Intensity" -tickinterval 0.5 \
               -variable View(LightIntensity) -resolution 0.1 -orient horizontal \
               -command ViewUpdateLightIntensity} $Gui(WSA)

    TooltipAdd $f.sIntensity "Set the overall lighting intensity of the scene."
    pack $f.sIntensity -fill x -side top -padx $Gui(pad) -pady $Gui(pad) -expand true
}

#-------------------------------------------------------------------------------
# .PROC ViewSetMovieFileType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewSetMovieFileType {item} {
    global View Module

    # Update variable
    set View(movieFileType) $item

    # Update GUI
    set f $Module(View,fView).fMovie
    eval $f.mbFile config "-text $item"
}

#-------------------------------------------------------------------------------
# .PROC ViewBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewBuildVTK {} {
    ViewBuildLightsVTK
}

#-------------------------------------------------------------------------------
# .PROC ViewBuildLightsVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewBuildLightsVTK {} {
    global View
    ViewCreateLightKit
    set View(LightIntensity) 1.0

    ViewSwitchLightKit 1
    set View(LightModeIndicator) "LightKit"
}

proc ViewSaveHeadlight {} {
    global View

    set ren viewRen

    set lights [$ren GetLights]
    $lights InitTraversal
    set hl [$lights GetNextItem]
    set View(LightSavedHeadlight) $hl
    set View(LightMode) "Headlight"
    set View(LightModeIndicator) "Headlight"
}

proc ViewCreateLightKit {} {
    global View
    vtkLightKit ViewLightKit
    set View(LightKit) ViewLightKit
}

#-------------------------------------------------------------------------------
# .PROC ViewSetLightIntensity
# 
# .ARGS
# float value Light intensity value
# .END
#-------------------------------------------------------------------------------
proc ViewSetLightIntensity {value} {
    global View
    set View(LightIntensity) $value
    ViewSwitchLightKit
}
#-------------------------------------------------------------------------------
# .PROC ViewUpdateLightIntensity
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ViewUpdateLightIntensity {args} {
    ViewSwitchLightKit
}

#-------------------------------------------------------------------------------
# .PROC ViewSwitchLightKit
# 
# .ARGS
# str state on or off
# .END
#-------------------------------------------------------------------------------
proc ViewSwitchLightKit {{state ""}} {
    global View

    set ren viewRen

    if {![info exists View(LightSavedHeadlight)]} {
        ViewSaveHeadlight
    }

    if {"$state" == ""} {
        if {$View(LightMode) == "Headlight"} {
            set state 0
        } else { set state 1 }
    }

    if {($state == 1 || "$state" == "on") && 
        $View(LightMode) == "Headlight"} {

        $View(LightKit) AddLightsToRenderer $ren
        $View(LightSavedHeadlight) SetIntensity 0.0

        set View(LightMode) "LightKit"

    } elseif {($state == 0 || "$state" == "off") && 
              $View(LightMode) == "LightKit"} {

        $View(LightKit) RemoveLightsFromRenderer $ren

        set View(LightMode) "Headlight"
    }

    if {$View(LightMode) == "Headlight"} {
        $View(LightSavedHeadlight) SetIntensity $View(LightIntensity)
    } else {
        $View(LightKit) SetKeyLightIntensity $View(LightIntensity)
    }   
    Render3D
}

