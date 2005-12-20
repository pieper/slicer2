#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerSlider.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:00 $
#   Version:   $Revision: 1.6.2.1 $
# 
#===============================================================================
# FILE:        IbrowserControllerSlider.tcl
# PROCEDURES:  
#   IbrowserUpdateIndexAndSliderBox
#   IbrowserUpdateIndexAndSliderMarker
#   IbrowserUpdateIndexFromGUI
#   IbrowserUpdateIndexFromAnimControls
#   IbrowserUpdateIndexFromAnimControls
#   IbrowserMarkSlider
#   IbrowserDragSlider
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserUpdateIndexAndSliderBox
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserUpdateIndexAndSliderBox { } {
    #---------------
    #--- This proc creates fresh, or deletes and
    #--- then redraws the slider box and its text
    #--- that represents the metric scale of 
    #--- the canvas slider
    #---------------
    set min $::IbrowserController(Info,Ival,globalIvalUnitSpanMin)
    set max $::IbrowserController(Info,Ival,globalIvalUnitSpanMax)

    #delete these canvas items if they already exist.
    #---------------
    $::IbrowserController(Ccanvas) delete IbrowserSliderboxText
    $::IbrowserController(Ccanvas) delete IbrowserSliderbox

    #calc good position for slider box (which contains the ruler)
    #Before first interval is created,
    #::IbrowserController(Info,Ival,globalIvalPixSpan) = 0 so
    #nothing is drawn.
    #---------------
    set wid $::IbrowserController(Info,Ival,globalIvalPixSpan)
    set ix $::IbrowserController(Info,Ival,globalIvalPixXstart)
    set ::IbrowserController(Slider,BarTopPos) 10
    set iy $::IbrowserController(Slider,BarTopPos)
    set xwid [expr $ix + $wid ]
    set yhit [expr $iy + $::IbrowserController(Geom,Ival,intervalPixHit) ]

    #draw the slider box outline
    #---------------    
    set color $::IbrowserController(Colors,lolite)
    $::IbrowserController(Ccanvas) create rect  $ix $iy $xwid $yhit \
        -fill #FFFFFF -outline $color -tag "IbrowserSliderbox" 

    #draw coarse, large gridlines and text labels
    #---------------    
    set t $min
    set ty [expr $::IbrowserController(Geom,Ival,intervalPixHit) * 1.5 ]
    set ty [expr $iy + $ty]
    set tmp [ expr $max - $min ]
    #calculated to make slider units correspond to ordinal volume number:
    set pixinc [ expr $wid / $tmp ]
    #calculated to make slider units reflect ordinal volume number:
    set unitinc 1.0
    set gap1 5
    set xx $ix
    set yy [expr $yhit + 5 ]
    #--- unit tickmarks are tagged so that we can use their position later
    #--- to move the slider around if necessary (from display gui and
    #--- animation controls, in which we have no pixel on the canvas to
    #--- which to move.)
    while { $xx <= $xwid } {
        if { $t >= 0 } {
            set intt [ expr int( $t ) ]
            set ticktag "IbrowserIndex$intt"
        } else {
            set ticktag "IbrowserIndexMinus"
        }
        $::IbrowserController(Ccanvas) create line $xx [expr $iy + $gap1] $xx $yy \
            -width 1 -tags "IbrowserSliderbox $ticktag" -fill #CCCCCC
        set txt [ format "%1.0f" $t ]
        $::IbrowserController(Ccanvas) create text  $xx $ty -anchor center  -text $txt \
            -fill #225588 -font $::IbrowserController(UI,Smallfont) -tag "IbrowserSliderboxText"
        set xx [expr $xx + $pixinc ]
        set t [expr $t + $unitinc ]
    }

    #draw fine, tiny gridlines
    #---------------
    set pixinc [expr $pixinc / 4.0]
    set gap1 10
    set xx [expr $ix + $pixinc ]
    while { $xx < $xwid } {
        $::IbrowserController(Ccanvas) create line $xx [expr $iy + $gap1] $xx $yhit \
            -width 1 -tag "IbrowserSliderbox" -fill #CCCCCC 
        set xx [expr $xx + $pixinc ]
    }

    #--- Set bindings that allow the slider to be moved when user clicks
    #--- anywhere inside the slider box.
    #--- Clicking should move both the dashed line and the little red
    #--- slider marker that are tagged as "indexDragger".
    $::IbrowserController(Ccanvas) bind IbrowserSliderbox <Button-1> {
        set screenx %x
        set canvasx [ $::IbrowserController(Ccanvas) canvasx $screenx ]
        IbrowserDragSlider $canvasx
    }

    $::IbrowserController(Ccanvas) bind IbrowserSliderbox <B1-Motion> {
        set screenx %x
        set canvasx [ $::IbrowserController(Ccanvas) canvasx $screenx ]
        IbrowserDragSlider $canvasx
    }

}



#-------------------------------------------------------------------------------
# .PROC IbrowserUpdateIndexAndSliderMarker
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserUpdateIndexAndSliderMarker { } {
    #---------------
    #--- This proc creates fresh, or deletes and
    #--- then redraws the little slider marker and
    #--- dotted lnie that represent the slider position
    #--- on the canvas.
    #---------------

    #delete these canvas items if they already exist.
    #---------------
    $::IbrowserController(Icanvas) delete "indexDragger"
    $::IbrowserController(Ccanvas) delete "indexDragger"

    #--- dx is the halfwidth of the little red slider marker.
    set dx 3
    set ytop 20
    set ybot [expr 10 + $::IbrowserController(Geom,Ival,intervalPixHit) ]
    set maxhit [ winfo screenheight .]
    set wid $::IbrowserController(Info,Ival,globalIvalPixSpan)
    set color $::IbrowserController(Colors,lolite)
    
    # set global index value
    #---------------    
    set min $::IbrowserController(Info,Ival,globalIvalPixSpanMin)
    set max $::IbrowserController(Info,Ival,globalIvalPixSpanMax)
    set pixspan [expr $max - $min ]
    set halfspan [expr $pixspan / 2 ]

    #--- position the marker at zero.
    #set ::IbrowserController(Slider,ClickX) [ IbrowserUnitValToPixelVal 0 ]
    set ::IbrowserController(Slider,ClickX) [ IbrowserUnitValToPixelVal $::Ibrowser(ViewDrop) ]
    
    # Create dashed line thru both canvases
    # marking index position
    #---------------    
    $::IbrowserController(Icanvas) create line $::IbrowserController(Slider,ClickX) 0 \
        $::IbrowserController(Slider,ClickX) $maxhit -width 1 -fill $::IbrowserController(Colors,indexRule) \
        -dash . -tags "indexDragger" 
    $::IbrowserController(Ccanvas) create line $::IbrowserController(Slider,ClickX) 0 \
        $::IbrowserController(Slider,ClickX) $::IbrowserController(Slider,BarTopPos) -width 1 \
        -fill $::IbrowserController(Colors,indexRule) \
        -dash . -tags "indexDragger" 
    set xleft [ expr $::IbrowserController(Slider,ClickX) - $dx ]
    set xright [ expr $::IbrowserController(Slider,ClickX) + $dx ]
    
    # Create little triangle marker that shows index.
    #---------------    
    $::IbrowserController(Ccanvas) create poly $xleft $ybot $::IbrowserController(Slider,ClickX) \
        $ytop $xright $ybot -fill $::IbrowserController(Colors,indexMarker) -tags "indexDragger pindexDragger" 


    #--- have to create same click/drag rules for the little red marker
    #--- as we have for the sliderbox, since clicking on the red marker
    #--- doesn't register as a click in the sliderbox.
    $::IbrowserController(Ccanvas) bind pindexDragger <Button-1> {
        set screenx %x
        set canvasx [ $::IbrowserController(Ccanvas) canvasx $screenx ]
        IbrowserMarkSlider $canvasx
    }
    
    $::IbrowserController(Ccanvas) bind pindexDragger <B1-Motion> {
        set screenx %x
        set canvasx [ $::IbrowserController(Ccanvas) canvasx $screenx ]
        IbrowserDragSlider $canvasx
    }
    
    $::IbrowserController(Icanvas) raise indexDragger
    $::IbrowserController(Ccanvas) raise indexDragger
           
}



#-------------------------------------------------------------------------------
# .PROC IbrowserUpdateIndexFromGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserUpdateIndexFromGUI { } {
    #--- if someone is sliding the scale in the Display GUI,
    #--- the slider in the controller needs to update in unison.
    #--- The display GUI slider selects the ViewDrop, so
    #--- it's as if we clicked on ViewDrop's pixel position in the canvas.
    #--- so we find the canvasx of that pixel position, and then
    #--- drag slider to there.

    set ticktag "IbrowserIndex$::Ibrowser(ViewDrop)"
    #--- the first coord in the list is the xvalue of the unit tickmark
    #--- on the scale; this is where we want to move the slider
    set xval [ lindex [ $::IbrowserController(Ccanvas) coords $ticktag ] 0 ]
    IbrowserDragSlider $xval
}



#-------------------------------------------------------------------------------
# .PROC IbrowserUpdateIndexFromAnimControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserUpdateIndexFromAnimControls { } {

    #---anytime that ViewDrop is changed elsewhere,
    #---like for instance, when using the displayGUI slider,
    #---must update the slider index on ::IbrowserController too.

    #--- it's as if we clicked on ViewDrop's pixel position.
    #--- so we find the canvasx of that pixel position, and then
    #--- drag slider to there.
    set ticktag "IbrowserIndex$::Ibrowser(ViewDrop)"
    #puts "viewdrop=$::Ibrowser(ViewDrop) tag=$ticktag"

    #--- the first coord in the list is the xvalue of the unit tickmark
    #--- on the scale; this is where we want to move the slider
    set xval [ lindex [ $::IbrowserController(Ccanvas) coords $ticktag ] 0 ]
    IbrowserDragSlider $xval
}




#-------------------------------------------------------------------------------
# .PROC 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc   IbrowserSynchronizeAllSliders { target } {
    if { $target == "active" } {
        foreach s "display load select keyframe1 keyframe2" {
            if { [info exists ::Ibrowser(${s}Slider)] } {
                $::Ibrowser(${s}Slider) configure -state active
            }            
        }
    } elseif { $target == "disabled" } {
        foreach s "display load select keyframe1 keyframe2" {
            if { [info exists ::Ibrowser(${s}Slider)] } {
                $::Ibrowser(${s}Slider) configure -state disabled
            }            
        }
    } else {
        foreach s "display load select keyframe1 keyframe2" {
            if { [info exists ::Ibrowser(${s}Slider)] } {
                $::Ibrowser(${s}Slider) configure -state active -from 0 -to $target
            }            
        }
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserMarkSlider
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMarkSlider { zz  } {

    #--- restrict slider movement to be above unit 0
    set ux [ IbrowserPixelValToUnitVal $zz ]
    if { $ux < 0 } {
        set zz [ IbrowserUnitValToPixelVal 0 ]
        set ux 0
    }

    set  top [ expr $::Ibrowser(MaxDrops) - 1 ]
    if { $top < 0 } {
        set top 0
    }
    #--- restrict slider movement to be below unit $max
    if { $ux > $top } {
        set zz [ IbrowserUnitValToPixelVal $top ]
        set ux $top 
    }
    
    set dx [expr $zz - $::IbrowserController(Slider,ClickX) ]
    $::IbrowserController(Ccanvas) move indexDragger $dx 0
    $::IbrowserController(Icanvas) move indexDragger $dx 0
    set ::IbrowserController(Slider,ClickX) $zz
    #---compute appropriate ViewDrop
    set round_ux [ expr round ($ux) ]
    set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
    set ::Ibrowser(ViewDrop) $round_ux
    IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
}



#-------------------------------------------------------------------------------
# .PROC IbrowserDragSlider
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDragSlider { zz } {

    #--- have we moved to a unit less than 0?
    set ux [ IbrowserPixelValToUnitVal $zz ]
    if { $ux < 0 } {
        set zz [ IbrowserUnitValToPixelVal 0 ]
        set ux 0
    }

    set  top [ expr $::Ibrowser(MaxDrops) - 1 ]
    if { $top < 0 } {
        set top 0
    }
    #--- restrict slider movement to be below unit $max
    if { $ux > $top } {
        set zz [ IbrowserUnitValToPixelVal $top ]
        set ux $top 
    }

    set dx [expr $zz - $::IbrowserController(Slider,ClickX) ]
    $::IbrowserController(Ccanvas) move indexDragger  $dx 0
    $::IbrowserController(Icanvas) move indexDragger $dx 0
    set ::IbrowserController(Slider,ClickX) [expr $::IbrowserController(Slider,ClickX) + $dx ]

    #---compute appropriate ViewDrop
    set round_ux [ expr round ($ux) ]
    set ::Ibrowser(LastViewDrop) $::Ibrowser(ViewDrop)
    set ::Ibrowser(ViewDrop) $round_ux
    IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)

}



