#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use and copy (but not distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        TkInteractor.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   BindTkRenderWidget
#   Expose
#   UpdateRenderer
#   Enter
#   StartMotion
#   EndMotion
#   Rotate
#   Pan
#   Zoom
#   Reset
#   Wireframe
#   Surface
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC BindTkRenderWidget
# .END
#-------------------------------------------------------------------------------
proc BindTkRenderWidget {widget} {
	global Gui

    bind $widget <Any-ButtonPress> {StartMotion %W %x %y}
    bind $widget <Any-ButtonRelease> {EndMotion %W %x %y}
    bind $widget <B1-Motion> {Rotate %W %x %y}
    bind $widget <B2-Motion> {Pan %W %x %y}
    bind $widget <B3-Motion> {Zoom %W %x %y}
    bind $widget <Shift-B1-Motion> {Pan %W %x %y}
    bind $widget <KeyPress-r> {Reset %W %x %y}
    bind $widget <KeyPress-u> {wm deiconify .vtkInteract}
    bind $widget <KeyPress-w> Wireframe
    bind $widget <KeyPress-s> Surface
    bind $widget <Enter> {Enter %W %x %y;}
    bind $widget <Leave> {focus $oldFocus;}
	bind $widget <Expose> {Expose %W}
}

#-------------------------------------------------------------------------------
# .PROC Expose
# a litle more complex than just "bind $widget <Expose> {%W Render}"
# we have to handle all pending expose events otherwise they que up.
# .END
#-------------------------------------------------------------------------------
proc Expose {widget} {

   if {[GetWidgetVariableValue $widget InExpose] == 1} {
      return
   }
   SetWidgetVariableValue $widget InExpose 1
   update
   [$widget GetRenderWindow] Render
   SetWidgetVariableValue $widget InExpose 0
}
 
# Global variable keeps track of whether active renderer was found
set RendererFound 0

# Create event bindings
#
proc Render {} {
    global CurrentCamera CurrentLight CurrentRenderWindow
	global View

    eval $CurrentLight SetPosition [$CurrentCamera GetPosition]
    eval $CurrentLight SetFocalPoint [$CurrentCamera GetFocalPoint]

	Render3D
}

#-------------------------------------------------------------------------------
# .PROC UpdateRenderer
# .END
#-------------------------------------------------------------------------------
proc UpdateRenderer {widget x y} {
    global CurrentCamera CurrentLight 
    global CurrentRenderWindow CurrentRenderer
    global RendererFound LastX LastY
    global WindowCenterX WindowCenterY

    # Get the renderer window dimensions
    set WindowX [lindex [$widget configure -width] 4]
    set WindowY [lindex [$widget configure -height] 4]

    # Find which renderer event has occurred in
    set CurrentRenderWindow [$widget GetRenderWindow]
    set renderers [$CurrentRenderWindow GetRenderers]
    set numRenderers [$renderers GetNumberOfItems]

    $renderers InitTraversal; set RendererFound 0
    for {set i 0} {$i < $numRenderers} {incr i} {
	set CurrentRenderer [$renderers GetNextItem]
	set vx [expr double($x) / $WindowX]
	set vy [expr ($WindowY - double($y)) / $WindowY]
	set viewport [$CurrentRenderer GetViewport]
	set vpxmin [lindex $viewport 0]
	set vpymin [lindex $viewport 1]
	set vpxmax [lindex $viewport 2]
	set vpymax [lindex $viewport 3]
	if { $vx >= $vpxmin && $vx <= $vpxmax && \
	$vy >= $vpymin && $vy <= $vpymax} {
            set RendererFound 1
            set WindowCenterX [expr double($WindowX)*(($vpxmax - $vpxmin)/2.0\
                                + $vpxmin)]
            set WindowCenterY [expr double($WindowY)*(($vpymax - $vpymin)/2.0\
		                + $vpymin)]
            break
        }
    }
    
    set CurrentCamera [$CurrentRenderer GetActiveCamera]
    set lights [$CurrentRenderer GetLights]
    $lights InitTraversal; set CurrentLight [$lights GetNextItem]

    set LastX $x
    set LastY $y
}

#-------------------------------------------------------------------------------
# .PROC Enter
# .END
#-------------------------------------------------------------------------------
proc Enter {widget x y} {
    global oldFocus

    set oldFocus [focus]
    focus $widget
    UpdateRenderer $widget $x $y
}

#-------------------------------------------------------------------------------
# .PROC StartMotion
# .END
#-------------------------------------------------------------------------------
proc StartMotion {widget x y} {
    global CurrentCamera CurrentLight 
    global CurrentRenderWindow CurrentRenderer
    global LastX LastY
    global RendererFound

    UpdateRenderer $widget $x $y
    if { ! $RendererFound } { return }

    $CurrentRenderWindow SetDesiredUpdateRate 5.0
}

#-------------------------------------------------------------------------------
# .PROC EndMotion
# .END
#-------------------------------------------------------------------------------
proc EndMotion {widget x y} {
    global CurrentRenderWindow
    global RendererFound

    if { ! $RendererFound } {return}
    $CurrentRenderWindow SetDesiredUpdateRate 0.01
    Render
}

#-------------------------------------------------------------------------------
# .PROC Rotate
# .END
#-------------------------------------------------------------------------------
proc Rotate {widget x y} {
    global CurrentCamera 
    global LastX LastY
    global RendererFound
	global View

    if { ! $RendererFound } { return }

    $CurrentCamera Azimuth [expr ($LastX - $x)]
    $CurrentCamera Elevation [expr ($y - $LastY)]
    $CurrentCamera OrthogonalizeViewUp

    set LastX $x
    set LastY $y

    Render
}

#-------------------------------------------------------------------------------
# .PROC Pan
# .END
#-------------------------------------------------------------------------------
proc Pan {widget x y} {
    global CurrentRenderer CurrentCamera
    global WindowCenterX WindowCenterY LastX LastY
    global RendererFound
	global View

    if { ! $RendererFound } { return }

    set FPoint [$CurrentCamera GetFocalPoint]
        set FPoint0 [lindex $FPoint 0]
        set FPoint1 [lindex $FPoint 1]
        set FPoint2 [lindex $FPoint 2]

    set PPoint [$CurrentCamera GetPosition]
        set PPoint0 [lindex $PPoint 0]
        set PPoint1 [lindex $PPoint 1]
        set PPoint2 [lindex $PPoint 2]

    $CurrentRenderer SetWorldPoint $FPoint0 $FPoint1 $FPoint2 1.0
    $CurrentRenderer WorldToDisplay
    set DPoint [$CurrentRenderer GetDisplayPoint]
    set focalDepth [lindex $DPoint 2]

    set APoint0 [expr $WindowCenterX + ($x - $LastX)]
    set APoint1 [expr $WindowCenterY - ($y - $LastY)]

    $CurrentRenderer SetDisplayPoint $APoint0 $APoint1 $focalDepth
    $CurrentRenderer DisplayToWorld
    set RPoint [$CurrentRenderer GetWorldPoint]
        set RPoint0 [lindex $RPoint 0]
        set RPoint1 [lindex $RPoint 1]
        set RPoint2 [lindex $RPoint 2]
        set RPoint3 [lindex $RPoint 3]
    if { $RPoint3 != 0.0 } {
        set RPoint0 [expr $RPoint0 / $RPoint3]
        set RPoint1 [expr $RPoint1 / $RPoint3]
        set RPoint2 [expr $RPoint2 / $RPoint3]
    }

    $CurrentCamera SetFocalPoint \
      [expr ($FPoint0 - $RPoint0)/2.0 + $FPoint0] \
      [expr ($FPoint1 - $RPoint1)/2.0 + $FPoint1] \
      [expr ($FPoint2 - $RPoint2)/2.0 + $FPoint2]

    $CurrentCamera SetPosition \
      [expr ($FPoint0 - $RPoint0)/2.0 + $PPoint0] \
      [expr ($FPoint1 - $RPoint1)/2.0 + $PPoint1] \
      [expr ($FPoint2 - $RPoint2)/2.0 + $PPoint2]

	set LastX $x
    set LastY $y

	MainAnnoUpdateFocalPoint $FPoint0 $FPoint1 $FPoint2
    Render
}

#-------------------------------------------------------------------------------
# .PROC Zoom
# .END
#-------------------------------------------------------------------------------
proc Zoom {widget x y} {
    global CurrentCamera
    global LastX LastY
    global RendererFound
	global View

    if { ! $RendererFound } { return }

    set zoomFactor [expr pow(1.02,($y - $LastY))]
    set clippingRange [$CurrentCamera GetClippingRange]
    set minRange [lindex $clippingRange 0]
    set maxRange [lindex $clippingRange 1]

    $CurrentCamera SetClippingRange [expr $minRange / $zoomFactor] \
                                    [expr $maxRange / $zoomFactor]
    $CurrentCamera Dolly $zoomFactor

	set LastX $x
    set LastY $y

    Render
}

#-------------------------------------------------------------------------------
# .PROC Reset
# .END
#-------------------------------------------------------------------------------
proc Reset {widget x y} {
    global CurrentRenderWindow
    global RendererFound
    global CurrentRenderer

    # Get the renderer window dimensions
    set WindowX [lindex [$widget configure -width] 4]
    set WindowY [lindex [$widget configure -height] 4]

    # Find which renderer event has occurred in
    set CurrentRenderWindow [$widget GetRenderWindow]
    set renderers [$CurrentRenderWindow GetRenderers]
    set numRenderers [$renderers GetNumberOfItems]

    $renderers InitTraversal; set RendererFound 0
    for {set i 0} {$i < $numRenderers} {incr i} {
	set CurrentRenderer [$renderers GetNextItem]
	set vx [expr double($x) / $WindowX]
	set vy [expr ($WindowY - double($y)) / $WindowY]

	set viewport [$CurrentRenderer GetViewport]
	set vpxmin [lindex $viewport 0]
	set vpymin [lindex $viewport 1]
	set vpxmax [lindex $viewport 2]
	set vpymax [lindex $viewport 3]
	if { $vx >= $vpxmin && $vx <= $vpxmax && \
	$vy >= $vpymin && $vy <= $vpymax} {
            set RendererFound 1
            break
        }
    }

    if { $RendererFound } {$CurrentRenderer ResetCamera}

    Render
}

#-------------------------------------------------------------------------------
# .PROC Wireframe
# .END
#-------------------------------------------------------------------------------
proc Wireframe {} {
    global CurrentRenderer

    set actors [$CurrentRenderer GetActors]

    $actors InitTraversal
    set actor [$actors GetNextItem]
    while { $actor != "" } {
        [$actor GetProperty] SetRepresentationToWireframe
        set actor [$actors GetNextItem]
    }

    Render
}

#-------------------------------------------------------------------------------
# .PROC Surface
# .END
#-------------------------------------------------------------------------------
proc Surface {} {
    global CurrentRenderer

    set actors [$CurrentRenderer GetActors]

    $actors InitTraversal
    set actor [$actors GetNextItem]
    while { $actor != "" } {
        [$actor GetProperty] SetRepresentationToSurface
        set actor [$actors GetNextItem]
    }

    Render
}
