#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        IbrowserControllerIcons.tcl
# PROCEDURES:  
#   IbrowserSetupIcons
#   IbrowserInitIconImages
#   IbrowserInitIconInfo
#   IbrowserIncrementIconCount
#   IbrowserDecrementIconCount
#   IbrowserInitIconGeom
#   IbrowserMoveIcons
#   IbrowserDeleteIntervalIcons
#   IbrowserCreateIcons
#   IbrowserMakeNameIcon
#   IbrowserMakeOrderIcon
#   IbrowserMakeVisibilityIcon
#   IbrowserMakeCopyIcon
#   IbrowserMakeOpacityIcon
#   IbrowserToggleHoldIcon
#   IbrowserToggleVisibilityIcon
#   IbrowserMakeHoldIcon
#   IbrowserMakeDeleteIcon
#   IbrowserMakeFGIcon
#   IbrowserMakeBGIcon
#   IbrowserDeselectFGIcon
#   IbrowserGangFGandBGVisibility
#   IbrowserSelectFGIcon
#   IbrowserLeaveFGIcon
#   IbrowserDeselectBGIcon
#   IbrowserSelectBGIcon
#   IbrowseLeaveBGIcon
#   IbrowserCopyIntervalPopUp
#   IbrowserDeleteIntervalPopUp
#   IbrowserOrderPopUp
#   IbrowserSetupNewNames
#   IbrowserOpacityPopUp
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserSetupIcons
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetupIcons { } {
    IbrowserInitIconImages
    IbrowserInitIconInfo
    IbrowserInitIconGeom
}




#-------------------------------------------------------------------------------
# .PROC IbrowserInitIconImages
# Creates all image icons and stuffs them into an array
# indexed by a name that describes what the icon shows.
# Make a new thingIcon for each new option or state you'd
# like an icon to indicate. For instance, if you want to add a
# new compositing method that performs an XOR operation
# on two inputs, define an 'IbrowserController(Images,Icon,xorIcon)' here.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserInitIconImages { } {
    global PACKAGE_DIR_VTKIbrowser
    
    # Current icons are:
    # intervalname, layerorder, visibility, opacity, renderstyle, compositestyle

    #--- This variable contains the module path plus some stuff
    #--- trim off the extra stuff, and add on the path to tcl files.
    set tmpstr $PACKAGE_DIR_VTKIbrowser
    set tmpstr [string trimright $tmpstr "/vtkIbrowser" ]
    set tmpstr [string trimright $tmpstr "/Tcl" ]
    set tmpstr [string trimright $tmpstr "Wrapping" ]
    set modulePath [format "%s%s" $tmpstr "tcl/"]

    if { $::IbrowserController(UI,Small) } {
        set ::IbrowserController(Images,Icon,visIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/visible.gif]
        set ::IbrowserController(Images,Icon,invisIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/invisible.gif]
        set ::IbrowserController(Images,Icon,opaqIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/opaque.gif]
        set ::IbrowserController(Images,Icon,transpIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/semi-transp.gif]
        set ::IbrowserController(Images,Icon,orderIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/under-order2.gif]
        set ::IbrowserController(Images,Icon,nameIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/under-name2.gif]
        set ::IbrowserController(Images,Icon,holdIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/hold.gif]
        set ::IbrowserController(Images,Icon,noholdIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/nohold.gif]
        set ::IbrowserController(Images,Icon,FGIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/FG.gif]
        set ::IbrowserController(Images,Icon,BGIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/BG.gif]
        set ::IbrowserController(Images,Icon,deleteIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/delete.gif]
        set ::IbrowserController(Images,Icon,copyIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/copy.gif]

    } else {
        set ::IbrowserController(Images,Icon,visIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/visible.gif]
        set ::IbrowserController(Images,Icon,invisIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/invisible.gif]
        set ::IbrowserController(Images,Icon,orderIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/under-order2.gif]
        set ::IbrowserController(Images,Icon,nameIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/under-name2.gif]
        set ::IbrowserController(Images,Icon,opaqIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/opaque.gif]
        set ::IbrowserController(Images,Icon,transpIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/semi-transp.gif]
        set ::IbrowserController(Images,Icon,holdIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/hold.gif]
        set ::IbrowserController(Images,Icon,noholdIcon) \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/nohold.gif]
        set ::IbrowserController(Images,Icon,FGIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/FG.gif]
        set ::IbrowserController(Images,Icon,BGIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/BG.gif]
        set ::IbrowserController(Images,Icon,deleteIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/delete.gif]
        set ::IbrowserController(Images,Icon,copyIcon)  \
            [image create photo \
                 -file ${modulePath}iconPix/20x20/gifs/canvas/copy.gif]

    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserInitIconInfo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserInitIconInfo { } {
    
    # If you add a new icon to the interval bar,
    # please adjust 'numIconsPerInterval' below.
    # This number is the same for all intervals
    #---------------
    set ::IbrowserController(Info,Icon,numIconsPerInterval) 8
    set ::IbrowserController(Info,Icon,iconCount) 0

    # These are different states that various
    # icons can have; depending on its state,
    # the icon may be represented with a different
    # image. If you add a new icon and it has
    # different visual states, include them here,
    # and in intervals.tcl where their string
    # values are defined.
    #---------------    
    set ::IbrowserController(Info,Icon,isVisible) $::IbrowserController(Info,Ival,isVisible)
    set ::IbrowserController(Info,Icon,isInvisible) $::IbrowserController(Info,Ival,isInvisible)
    set ::IbrowserController(Info,Icon,isOpaque) $::IbrowserController(Info,Ival,isOpaque)
    set ::IbrowserController(Info,Icon,isTransparent) $::IbrowserController(Info,Ival,isTransparent)
    set ::IbrowserController(Info,Icon,hold) $::IbrowserController(Info,Ival,hold)
}




#-------------------------------------------------------------------------------
# .PROC IbrowserIncrementIconCount
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserIncrementIconCount { } {

    set ::IbrowserController(Info,Icon,iconCount) \
        [expr $::IbrowserController(Info,Icon,iconCount) + 1 ]
}




#-------------------------------------------------------------------------------
# .PROC IbrowserDecrementIconCount
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDecrementIconCount { } {

    set ::IbrowserController(Info,Icon,iconCount) \
        [expr $::IbrowserController(Info,Icon,iconCount) - 1 ]
}





#-------------------------------------------------------------------------------
# .PROC IbrowserInitIconGeom
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserInitIconGeom { } {
    
    # There are currently $::IbrowserController(Info,Icon,numIconsPerInterval) icons shown
    # leftward of the interval in the UI. UI element render position and
    # inter-element spacings are based on this number; if more icons are
    # added, make sure to change the number. Note: all icons are the
    # same size but for the intervalname icon, which is assigned the width
    # of all other icons combined in the UIsetup proc. The current icons are:
    # intervalname, layerorder, visibility, opacity, renderstyle, compositestyle
    # Here, the geometry is specified in actual pixel values.
    #---------------
    if { $::IbrowserController(UI,Small) } {
        set ::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Ival,intervalXbuf)
        set ::IbrowserController(Geom,Icon,iconYstart) $::IbrowserController(Geom,Ival,intervalYbuf)
        # both x and y:
        set ::IbrowserController(Geom,Icon,iconStart) $::IbrowserController(Geom,Ival,intervalXbuf)

        set ::IbrowserController(Geom,Icon,iconWid) 20
        # Name icon is six times the width of small icons
        #---------------
        set ::IbrowserController(Geom,Icon,nameIconMultiple) 6

        # Name icon is six times the width of small icons
        set jigger [ expr $::IbrowserController(Geom,Icon,iconWid) * \
                         $::IbrowserController(Geom,Icon,nameIconMultiple) ]
        set ::IbrowserController(Geom,Icon,nameIconWid) $jigger 
        set ::IbrowserController(Geom,Icon,iconHit) 20
        set ::IbrowserController(Geom,Icon,iconGap) 2

    } elseif { $::IbrowserController(UI,Big) } {
        set ::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Ival,intervalXbuf)
        set ::IbrowserController(Geom,Icon,iconYstart) $::IbrowserController(Geom,Ival,intervalYbuf)
        # both x and y:
        set ::IbrowserController(Geom,Icon,iconStart) $::IbrowserController(Geom,Ival,intervalXbuf)
        
        set ::IbrowserController(Geom,Icon,iconWid) 20
        # Name icon is six times the width of small icons
        #---------------
        set ::IbrowserController(Geom,Icon,nameIconMultiple) 6

        # this maneuver should make the name icon
        # twice the size as all small icons combined.
        set jigger [ expr $::IbrowserController(Geom,Icon,iconWid) \
                         * $::IbrowserController(Geom,Icon,nameIconMultiple) ]
        set ::IbrowserController(Geom,Icon,nameIconWid) $jigger 
        set ::IbrowserController(Geom,Icon,iconHit) 20
        set ::IbrowserController(Geom,Icon,iconGap) 2

    } else {
        set ::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Ival,intervalXbuf)
        set ::IbrowserController(Geom,Icon,iconYstart) $::IbrowserController(Geom,Ival,intervalYbuf)
        # both x and y:
        set ::IbrowserController(Geom,Icon,iconStart) $::IbrowserController(Geom,Ival,intervalXbuf)

        set ::IbrowserController(Geom,Icon,iconWid) 20
        # Name icon is six times the width of small icons
        #---------------
        set ::IbrowserController(Geom,Icon,nameIconMultiple) 6

        # Name icon is six times the width of small icons
        set jigger [ expr $::IbrowserController(Geom,Icon,iconWid) \
                         * $::IbrowserController(Geom,Icon,nameIconMultiple) ]
        set ::IbrowserController(Geom,Icon,nameIconWid)  $jigger 
        set ::IbrowserController(Geom,Icon,iconHit) 20
        set ::IbrowserController(Geom,Icon,iconGap) 2
    }
    set xx [ expr $::IbrowserController(Geom,Icon,iconWid) + $::IbrowserController(Geom,Icon,iconGap) ]
    set xx [ expr $xx * $::IbrowserController(Info,Icon,numIconsPerInterval) ]
    set xx [ expr $xx + $::IbrowserController(Geom,Icon,iconXstart) ]
    set ::IbrowserController(Geom,Icon,iconTotalXspan) $xx
    set ::IbrowserController(Geom,Ival,intervalXstart) $xx

}


#-------------------------------------------------------------------------------
# .PROC IbrowserMoveIcons
# This routine gets called when an intervals' order has changed;
# and the vertical positions of its icons within the canvas must be
# updated.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMoveIcons { Iname oldy newy } {
    
    #move icons by yy.
    #---------------
    set yy [ expr $newy - $oldy ]


    
    #get id from name
    set id $::Ibrowser($Iname,intervalID)
    
    #--- now move the icon image, its rectangular outline, and text if it has any.
    #name
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,nameIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,nameIconHILOtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,nameTXTtag) 0.0 $yy

    #order
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,orderIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,orderIconHILOtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,orderTXTtag) 0.0 $yy

    #visibility
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,visIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,visIconHILOtag) 0.0 $yy

    #opacity
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,opIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,opaqIconHILOtag) 0.0 $yy

    #hold
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,holdIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,holdIconHILOtag) 0.0 $yy

    
    #FG
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,FGIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,FGIconHILOtag) 0.0 $yy

    #BG
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,BGIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,BGIconHILOtag) 0.0 $yy

    #delete
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,deleteIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,deleteIconHILOtag) 0.0 $yy

    #copy
    #---------------    
    $::IbrowserController(Icanvas) move $::IbrowserController($id,copyIMGtag) 0.0 $yy
    $::IbrowserController(Icanvas) move $::IbrowserController($id,copyIconHILOtag) 0.0 $yy
    
}




#-------------------------------------------------------------------------------
# .PROC IbrowserDeleteIntervalIcons
# Deletes icons after an interval has been deleted.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeleteIntervalIcons { ivalName } {

    #--- get id from name
    set id $::Ibrowser($ivalName,intervalID)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,nameTXTtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,nameIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,nameIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,orderIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,orderIconHILOtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,orderTXTtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,visIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,invisIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,visIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,opIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,transpIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,opaqIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,holdIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,holdIconHILOtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,noholdIconHILOtag)
    
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,FGIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,FGIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,BGIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,BGIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,deleteIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,deleteIconHILOtag)

    $::IbrowserController(Icanvas) delete $::IbrowserController($id,copyIMGtag)
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,copyIconHILOtag)

}



#-------------------------------------------------------------------------------
# .PROC IbrowserCreateIcons
# This routine is called when a new interval is created.
# It creates the icons that go with each interval.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateIcons { ivalName } {

    #--- get id from name
    set id $::Ibrowser($ivalName,intervalID)

    # make an interval's icons.
    #name
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconStart)
    set y2 [ expr $::IbrowserController(Geom,Icon,iconYstart) \
                 + $::IbrowserController(Geom,Icon,iconHit) ]
    set x2 [expr $::IbrowserController(Geom,Icon,iconXstart) \
                + $::IbrowserController(Geom,Icon,nameIconWid) ]
    IbrowserMakeNameIcon $id $::IbrowserController(Geom,Icon,iconXstart) \
        $::IbrowserController(Geom,Icon,iconYstart) \
        $x2 $y2 $::IbrowserController(UI,Medfont)

    #order
    #---------------
    set dx [ expr $::IbrowserController(Geom,Icon,iconGap) + $::IbrowserController(Geom,Icon,iconWid) ]
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $x2 ]
    IbrowserMakeOrderIcon  $id $::Ibrowser($id,orderStatus) \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] \
        $y2 $::IbrowserController(UI,Medfont)

     #visibility
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeVisibilityIcon  $id $::IbrowserController(Info,Icon,isVisible) \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2

    #copy
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeCopyIcon  $id \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2

    #delete
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeDeleteIcon  $id \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2
    

    #hold
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeHoldIcon  $id $::IbrowserController(Info,Icon,hold)\
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2

    #FG
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeFGIcon  $id \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2

    #BG
    #---------------
    set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    IbrowserMakeBGIcon  $id \
        $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
        [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2
    
    #opacity
    #---------------
    #set ::IbrowserController(Geom,Icon,iconXstart) [expr $::IbrowserController(Geom,Icon,iconXstart) + $dx ]    
    #IbrowserMakeOpacityIcon  $id \
    #    $::IbrowserController(Geom,Icon,iconXstart) $::IbrowserController(Geom,Icon,iconYstart) \
    #    [expr $::IbrowserController(Geom,Icon,iconXstart) + $::IbrowserController(Geom,Icon,iconWid)] $y2


    
}



#-------------------------------------------------------------------------------
# .PROC IbrowserMakeNameIcon
# This routine is called each time a new interval is
# created. It creates a name icon indicating the interval's
# given or default name, and returns the icon's ID value.
# FOR NOW, ID these and capture clicks with tags???????
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeNameIcon { id x1 y1 x2 y2 fnt } {
    
    # Create background image for interval's name
    # Create text for name, and rectangle around it,
    # drawn in LO or HI color as mouse rolls over
    # or selects.
    #---------------

    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,nameIcon) \
                -anchor nw -tag "$::IbrowserController($id,nameIMGtag)" ]
    set namejigger 2
    set xx [ expr $x1 + $namejigger ]
    set yy [ expr $y2 - $y1 ]
    set yy [ expr $yy / 4]
    set yy [ expr $yy + $y1 ]
    $::IbrowserController(Icanvas) create text  $xx  $yy  -anchor nw \
        -text $::Ibrowser($id,name) -font $fnt -tag "$::IbrowserController($id,nameTXTtag)"
    $::IbrowserController(Icanvas) create rect $x1 $y1 \
        [ expr $x1 + $::IbrowserController(Geom,Icon,nameIconWid) -1 ]  \
        [ expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] \
        -tags "iconbox" -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,nameIconHILOtag)"

    # rectangle hilights during mouse-over of text
    #---------------
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,nameIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,nameIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Button-1> \
        "IbrowserCanvasSelectText $id $::IbrowserController($id,nameTXTtag)"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Delete> \
        "IbrowserCanvasDelete $::IbrowserController($id,nameTXTtag)"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <BackSpace> \
        "IbrowserCanvasBackSpace"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Return> \
        "IbrowserCanvasFinishEdit $id"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Any-Key> \
        "IbrowserCanvasInsert %A"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Key-Right> \
        "IbrowserCanvasMoveRight"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Key-Left> \
        "IbrowserCanvasMoveLeft"

#    $::IbrowserController(Icanvas) config -cursor xterm

    # rectangle hilights during mouse-over of rect too
    #---------------
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,nameIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameIMGtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,nameIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameIMGtag) <Button-1> \
        "IbrowserCanvasSelectText $id $::IbrowserController($id,nameTXTtag)"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Delete> \
        "IbrowserCanvasDelete $::IbrowserController($id,nameTXTtag)"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <BackSpace> \
        "IbrowserCanvasBackSpace"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Return> \
        "IbrowserCanvasFinishEdit $id"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Any-Key> \
        "IbrowserCanvasInsert %A"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Key-Right> \
        "IbrowserCanvasMoveRight"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,nameTXTtag) <Key-Left> \
        "IbrowserCanvasMoveLeft"

    IbrowserIncrementIconCount
             
}




#-------------------------------------------------------------------------------
# .PROC IbrowserMakeOrderIcon
# This routine is called each time a new interval is
# created. It creates a layerOrder icon indicating the interval's
# default layer number, and returns the icon's ID value.
# FOR NOW, ID these and capture clicks with tags???????
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeOrderIcon { id order x1 y1 x2 y2 fnt} {

    set name $::Ibrowser($id,name)
    
    #---------------
    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,orderIcon) \
                -anchor nw -tag "$::IbrowserController($id,orderIMGtag)" ]
    set xx [ expr $x2 - $x1 ]
    set xx [ expr $xx / 2 ]
    set yy [ expr $y2 - $y1 ]
    set yy [ expr $yy / 4 ]

    #---------------
    set txt [ format "%d" $order]
    $::IbrowserController(Icanvas) create text  [expr $x1 +  $xx ]  [expr $y1 + $yy ] \
        -anchor n -text $txt -font $fnt -tag "$::IbrowserController($id,orderTXTtag)"
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,orderIconHILOtag)"

    #---------------
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,orderIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderIMGtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,orderIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderIMGtag) <Button-1> \
        " IbrowserOrderPopUp orderpopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"
    
    #---------------
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderTXTtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,orderIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderTXTtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,orderIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,orderTXTtag) <Button-1> \
        " IbrowserOrderPopUp orderpopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"
    
    IbrowserIncrementIconCount
}






#-------------------------------------------------------------------------------
# .PROC IbrowserMakeVisibilityIcon
# This routine is called each time a new interval is
# created. It creates a visibility icon indicating the interval's
# default state=visible, and returns the icon's ID value.
# If there's a bogus transparency setting, then return 0.
# FOR NOW, ID these and capture clicks with tags???????
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeVisibilityIcon { id state x1 y1 x2 y2 } {
    
    
    if { $state == $::IbrowserController(Info,Icon,isVisible) } {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1  \
                    -image $::IbrowserController(Images,Icon,visIcon) \
                    -anchor nw -tag "$::IbrowserController($id,visIMGtag)" ]

        #hilite rect on mouseover
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,visIMGtag)  <Enter> \
            "%W itemconfig $::IbrowserController($id,visIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,visIMGtag)  <Leave> \
            "%W itemconfig $::IbrowserController($id,visIconHILOtag) -outline $::IbrowserController(Colors,lolite) "

        #toggle image on rightclick
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,visIMGtag) <Button-1> \
            "IbrowserToggleVisibilityIcon $id"

        #post help on leftclick
        #---------------
        
    } elseif { $state ==  $::IbrowserController(Info,Icon,isInvisible) } {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                    -image $::IbrowserController(Images,Icon,invisIcon) \
                    -anchor nw -tag "$::IbrowserController($id,invisIMGtag)" ]
        
        #hilite rect on mouseover
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,invisIMGtag) <Enter> \
            "%W itemconfig $::IbrowserController($id,visIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,invisIMGtag) <Leave> \
            "%W itemconfig $::IbrowserController($id,visIconHILOtag) -outline $::IbrowserController(Colors,lolite) "

        #toggle image on rightclick
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,invisIMGtag) <Button-1> \
            "IbrowserToggleVisibilityIcon $id"

        #post help on leftclick
        #---------------
    }
    
    # this is the hilight around the icon that lights up, and goes down
        #---------------
    eval "$::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
                [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) -tag $::IbrowserController($id,visIconHILOtag)"

    IbrowserIncrementIconCount

}



#-------------------------------------------------------------------------------
# .PROC IbrowserMakeCopyIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeCopyIcon { id x1 y1 x2 y2 } {

    #--- when user clicks an interval's copy icon, they are prompted for
    #--- the name of the new interval they want to contain a copy of
    #--- the selected interval's contents.
    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,copyIcon) \
                -anchor nw -tag "$::IbrowserController($id,copyIMGtag)" ]
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,copyIconHILOtag)"

    $::IbrowserController(Icanvas) bind $::IbrowserController($id,copyIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,copyIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,copyIMGtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,copyIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,copyIMGtag) <Button-1> \
        "IbrowserCopyIntervalPopUp copypopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"
    
    IbrowserIncrementIconCount



}


#-------------------------------------------------------------------------------
# .PROC IbrowserMakeOpacityIcon
# This routine is called each time a new interval is
# created. It creates an opacity icon indicating the interval's
# default or specified opacity, and returns the icon's ID value.
# If there's a bogus transparency setting, then return 0.
# FOR NOW, ID these and capture clicks with tags???????
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeOpacityIcon { id x1 y1 x2 y2 } {

    set state $::Ibrowser($id,opaqStatus)
    
    if { $state == $::IbrowserController(Info,Ival,isOpaque) } {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                    -image $::IbrowserController(Images,Icon,opaqIcon) \
                    -anchor nw -tag "$::IbrowserController($id,opIMGtag)" ]
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,opIMGtag) <Enter> \
            "%W itemconfig $::IbrowserController($id,opaqIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,opIMGtag) <Leave> \
            "%W itemconfig $::IbrowserController($id,opaqIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,opIMGtag) <Button-1> \
            " IbrowserOpacityPopUp opacitypopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"        
    } else {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                    -image $::IbrowserController(Images,Icon,transpIcon) \
                    -anchor nw -tag "$::IbrowserController($id,transpIMGtag)" ]
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,transpIMGtag) <Enter> \
            "%W itemconfig $::IbrowserController($id,opaqIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,transpIMGtag) <Leave> \
            "%W itemconfig $::IbrowserController($id,opaqIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,transpIMGtag) <Button-1> \
            " IbrowserOpacityPopUp opacitypopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"        
    }
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,opaqIconHILOtag)"

    IbrowserIncrementIconCount
}




#-------------------------------------------------------------------------------
# .PROC IbrowserToggleHoldIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserToggleHoldIcon { win  id } {

    if { $::Ibrowser($id,name) != "none" } {
        if { $::Ibrowser($id,holdStatus) == $::IbrowserController(Info,Ival,hold) } {
            set thing [$win get current ]
            set thing [ IbrowserNameFromCurrent [$win get current] ]
            $win itemconfig $thing -image $::IbrowserController(Images,Icon,noholdIcon)
            set ::Ibrowser($id,holdStatus) $::IbrowserController(Info,Ival,nohold)
        } else {
            set thing [$win get current ]
            set thing [ IbrowserNameFromCurrent [$win get current] ]
            $win itemconfig  current -image $::IbrowserController(Images,Icon,holdIcon)
            set ::Ibrowser($id,holdStatus) $::IbrowserController(Info,Ival,hold)
        }
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserToggleVisibilityIcon
# This routine is bound to the visibilityIcon so that
# the visibility status of an interval toggles when
# the icon is clicked. Visibile/Invisible. Nowthen.
# Foreground and Background intervals have their
# visibility ganged together. So, if the visibility is
# changed in either the FG or the BG interval, the
# same visibility state is applied to the ganged interval
# as well.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserToggleVisibilityIcon { id } {

    #--- get id from name
    
    set name $::Ibrowser($id,name)

    set win $::IbrowserController(Icanvas)
    
    if { $::Ibrowser($id,visStatus) == $::IbrowserController(Info,Ival,isVisible) } {
        #--- If the interval is currently visible,
        #--- set its status to invisible and changs the icon state.
        set thing "$::IbrowserController($id,visIMGtag)"
        $win itemconfig  $thing -image $::IbrowserController(Images,Icon,invisIcon)
        set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isInvisible)

        if {$id == $::Ibrowser(FGInterval) } {
            #--- If the interval is the foreground interval,
            #--- must update the visibility status of the
            #--- background interval to match.
            set bgID $::Ibrowser(BGInterval)
            set otherthing "$::IbrowserController($bgID,visIMGtag)"
            $win itemconfig $otherthing -image $::IbrowserController(Images,Icon,invisIcon)
            set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isInvisible)

            #--- Then, update the MainViewer
            MainSlicesSetVisibilityAll 0
            RenderAll
        
        } elseif { $id == $::Ibrowser(BGInterval) } {
            #--- If the interval is the background interval,
            #--- must update the visibility status of the
            #--- foreground interval to match.
            set fgID $::Ibrowser(FGInterval)
            set otherthing "$::IbrowserController($fgID,visIMGtag)"
            $win itemconfig $otherthing -image $::IbrowserController(Images,Icon,invisIcon)
            set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isInvisible)
            
            #--- Then, update the MainViewer
            MainSlicesSetVisibilityAll 0
            RenderAll
        }
        
    } else {
        #--- If the interval is currently invisible,
        #--- set its status to visible and changes the icon state.
        set thing "$::IbrowserController($id,visIMGtag)"
        $win itemconfig  $thing -image $::IbrowserController(Images,Icon,visIcon)
        set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isVisible)

        if {$id == $::Ibrowser(FGInterval) } {
            #--- If the interval is the foreground interval,
            #--- must update the visibility status of the
            #--- background interval to match.
            #--- If the interval is the foreground interval,
            #--- must update the visibility status of the
            #--- background interval to match.
            set bgID $::Ibrowser(BGInterval)
            set otherthing "$::IbrowserController($bgID,visIMGtag)"
            $win itemconfig $otherthing -image $::IbrowserController(Images,Icon,visIcon)
            set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isVisible)

            #--- Then, update the MainViewer
            MainSlicesSetVisibilityAll 1
            RenderAll
            
        } elseif { $id == $::Ibrowser(BGInterval) } {
            #--- If the interval is the background interval,
            #--- must update the visibility status of the
            #--- foreground interval to match.
            set fgID $::Ibrowser(FGInterval)
            set otherthing "$::IbrowserController($fgID,visIMGtag)"
            $win itemconfig $otherthing -image $::IbrowserController(Images,Icon,visIcon)
            set ::Ibrowser($id,visStatus) $::IbrowserController(Info,Ival,isVisible)            

            #--- Then, update the MainViewer
            MainSlicesSetVisibilityAll 1
            RenderAll
        }
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserMakeHoldIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeHoldIcon { id state x1 y1 x2 y2 } {

    
    if { $state == $::IbrowserController(Info,Icon,hold) } {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1  \
                    -image $::IbrowserController(Images,Icon,holdIcon) \
                    -anchor nw -tag "$::IbrowserController($id,holdIMGtag)" ]

        #hilite rect on mouseover
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,holdIMGtag)  <Enter> \
            "%W itemconfig $::IbrowserController($id,holdIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,holdIMGtag)  <Leave> \
            "%W itemconfig $::IbrowserController($id,holdIconHILOtag) -outline $::IbrowserController(Colors,lolite) "

        #toggle image on rightclick
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,holdIMGtag) <Button-1> \
            " IbrowserToggleHoldIcon %W $id "

        #post help on leftclick
        #---------------
        
    } elseif { $state ==  $::IbrowserController(Info,Icon,nohold) } {
        set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                    -image $::IbrowserController(Images,Icon,noholdIcon) \
                    -anchor nw -tag "$::IbrowserController($id,noholdIMGtag)" ]
        
        #hilite rect on mouseover
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,noholdIMGtag) <Enter> \
            "%W itemconfig $::IbrowserController($id,noholdHILOtag)  -outline $::IbrowserController(Colors,hilite) "
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,noholdIMGtag) <Leave> \
            "%W itemconfig $::IbrowserController($id,noholdHILOtag) -outline $::IbrowserController(Colors,lolite) "

        #toggle image on rightclick
        #---------------
        $::IbrowserController(Icanvas) bind $::IbrowserController($id,noholdIMGtag) <Button-1> \
            " IbrowserToggleHoldIcon %W $id "

        #post help on leftclick
        #---------------
    }
    
    # this is the hilight around the icon that lights up, and goes down
    #---------------
    eval "$::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
                [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
                -tag $::IbrowserController($id,holdIconHILOtag)"

    IbrowserIncrementIconCount
}



#-------------------------------------------------------------------------------
# .PROC IbrowserMakeDeleteIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeDeleteIcon { id x1 y1 x2 y2 } {

    #--- when user clicks delete icon, they are prompted for
    #--- whether they truly want to delete the interval and
    #--- its contents. If yes, all are deleted.
    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,deleteIcon) \
                -anchor nw -tag "$::IbrowserController($id,deleteIMGtag)" ]
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,deleteIconHILOtag)"

    $::IbrowserController(Icanvas) bind $::IbrowserController($id,deleteIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,deleteIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,deleteIMGtag) <Leave> \
        "%W itemconfig $::IbrowserController($id,deleteIconHILOtag) -outline $::IbrowserController(Colors,lolite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,deleteIMGtag) <Button-1> \
        "IbrowserDeleteIntervalPopUp deletepopup $id $::IbrowserController(popupX) $::IbrowserController(popupY)"
    
    IbrowserIncrementIconCount

}



#-------------------------------------------------------------------------------
# .PROC IbrowserMakeFGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeFGIcon { id x1 y1 x2 y2 } {


    #--- when user clicks FG icon, this moves all the interval's volumes
    #--- to the foreground and makes them active.
    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,FGIcon) \
                -anchor nw -tag "$::IbrowserController($id,FGIMGtag)" ]
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,FGIconHILOtag)"

    $::IbrowserController(Icanvas) bind $::IbrowserController($id,FGIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,FGIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,FGIMGtag) <Leave> \
        "IbrowserLeaveFGIcon $id %W"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,FGIMGtag) <Button-1> \
        "MainSlicesSetVolumeAll Fore $::Ibrowser($id,$::Ibrowser(ViewDrop),MRMLid);
         IbrowserDeselectFGIcon %W;
         IbrowserSelectFGIcon $id %W;
         set ::Ibrowser(FGInterval) $id;
         IbrowserGangFGandBGVisibility;
         RenderAll"
    
    IbrowserIncrementIconCount

}



#-------------------------------------------------------------------------------
# .PROC IbrowserMakeBGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMakeBGIcon { id x1 y1 x2 y2 } {
    
    
    #--- when user clicks BG icon, this moves all the interval's volumes
    #--- to the background but does not make them active.
    set q [ $::IbrowserController(Icanvas) create image $x1 $y1 \
                -image $::IbrowserController(Images,Icon,BGIcon) \
                -anchor nw -tag "$::IbrowserController($id,BGIMGtag)" ]
    $::IbrowserController(Icanvas) create rect $x1 $y1  [expr $x1 + $::IbrowserController(Geom,Icon,iconWid) -1 ] \
        [expr $y1 + $::IbrowserController(Geom,Icon,iconHit) -1 ] -outline $::IbrowserController(Colors,lolite) \
        -tag "$::IbrowserController($id,BGIconHILOtag)"

    $::IbrowserController(Icanvas) bind $::IbrowserController($id,BGIMGtag) <Enter> \
        "%W itemconfig $::IbrowserController($id,BGIconHILOtag) -outline $::IbrowserController(Colors,hilite) "
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,BGIMGtag) <Leave> \
        "IbrowserLeaveBGIcon $id %W"
    $::IbrowserController(Icanvas) bind $::IbrowserController($id,BGIMGtag) <Button-1> \
        "MainSlicesSetVolumeAll Back $::Ibrowser($id,$::Ibrowser(ViewDrop),MRMLid) ;
          IbrowserDeselectBGIcon %W; 
          IbrowserSelectBGIcon $id %W;
          set ::Ibrowser(BGInterval) $id;
          IbrowserGangFGandBGVisibility;
          RenderAll"
    
    IbrowserIncrementIconCount
}



#-------------------------------------------------------------------------------
# .PROC IbrowserDeselectFGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeselectFGIcon { w } {

    #deselect old FG interval in the Controller panel
    foreach id $::Ibrowser(idList) {
        if { $id == $::Ibrowser(FGInterval) } {
            $w itemconfig $::IbrowserController($id,FGIconHILOtag) \
                -outline $::IbrowserController(Colors,lolite)
        }
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserGangFGandBGVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGangFGandBGVisibility { } {

    #--- check the foreground's visibility status.
    #--- check the background's visibility status.
    #--- if they don't match, change the background's
    #--- to match the foreground
    set fgID $::Ibrowser(FGInterval)
    set fgstatus $::Ibrowser($fgID,visStatus)

    set bgID $::Ibrowser(BGInterval)
    set bgstatus $::Ibrowser($bgID,visStatus)

    if { $fgstatus != $bgstatus } {
        IbrowserToggleVisibilityIcon $bgID
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserSelectFGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSelectFGIcon { id w } {

    #hilight FG interval in the Controller panel
    $w itemconfig $::IbrowserController($id,FGIconHILOtag) \
        -outline $::IbrowserController(Colors,hilite)

    #--- update the Slices windows to reflect the foreground
    #--- interval's visibility.
    if { $::Ibrowser($id,visStatus) == $::IbrowserController(Info,Ival,isVisible) } {
        MainSlicesSetVisibilityAll 1
    } else {
        MainSlicesSetVisibilityAll 0
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserLeaveFGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserLeaveFGIcon { id w } {

    #leave FG interval in the proper state

    if { $id != $::Ibrowser(FGInterval) } {
        $w itemconfig $::IbrowserController($id,FGIconHILOtag) \
            -outline $::IbrowserController(Colors,lolite)
    } else {
        $w itemconfig $::IbrowserController($id,FGIconHILOtag) \
            -outline $::IbrowserController(Colors,hilite)
    }
}


#-------------------------------------------------------------------------------
# .PROC IbrowserDeselectBGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeselectBGIcon { w } {

    #deselect old BG interval in the Controller panel
    foreach id $::Ibrowser(idList) {
        if { $id == $::Ibrowser(BGInterval) } {
            $w itemconfig $::IbrowserController($id,BGIconHILOtag) \
                -outline $::IbrowserController(Colors,lolite)
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC IbrowserSelectBGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSelectBGIcon { id w } {

    #hilight BG interval in the Controller panel
    $w itemconfig $::IbrowserController($id,BGIconHILOtag) \
        -outline $::IbrowserController(Colors,hilite)

}


#-------------------------------------------------------------------------------
# .PROC IbrowseLeaveBGIcon
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserLeaveBGIcon { id w } {

    #leave BG interval in the proper state

    if { $id != $::Ibrowser(BGInterval) } {
        $w itemconfig $::IbrowserController($id,BGIconHILOtag) \
            -outline $::IbrowserController(Colors,lolite)
    } else {
        $w itemconfig $::IbrowserController($id,BGIconHILOtag) \
            -outline $::IbrowserController(Colors,hilite)
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserCopyIntervalPopUp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCopyIntervalPopUp { win id x y } {

    set noneID $::Ibrowser(none,intervalID)
    
    if { $id != $noneID } {
        set w .w$win
        if { [IbrowserRaisePopup $w] == 1} {return}

        set title "Copy interval"
        IbrowserCreatePopup $w $title $x $y 

        set f $w
        frame $f.fMsg  -background #FFFFFF 
        frame $f.fName -background #FFFFFF
        frame $f.fButtons -background #FFFFFF
        pack $f.fMsg $f.fName $f.fButtons -side top -pady 4 -padx 4 
        
        set f $w.fMsg
        eval {label $f.l -text "Name a copy of $::Ibrowser($id,name)."} 
        $f.l config -font $::IbrowserController(UI,Medfont) -background #FFFFFF
        pack $f.l -padx 5 -pady 5

        set f $w.fName
        set ::Ibrowser(thisName) $::Ibrowser($id,name)
        set ::Ibrowser(afterName) $::Ibrowser($id,name)-copy
        eval { label $f.l -text "new interval: " -background #FFFFFF \
                   -font $::IbrowserController(UI,Medfont) }
        eval { entry $f.e -width 20 -relief sunken -textvariable ::Ibrowser(afterName) }
        pack $f.l $f.e -side left -padx 4 -pady 4

        #dismiss
        #---------------
        set f $w.fButtons
        button $f.bApply -text "Apply" -width 4 -bg #DDDDDD \
            -command {
                set goodname [ IbrowserUniqueNameCheck $::Ibrowser(afterName) ]
                if { $goodname } {
                    IbrowserCopyInterval $::Ibrowser(thisName) $::Ibrowser(afterName)
                    destroy .wcopypopup
                } else {
                    IbrowserSayThis "This interval name is already in use. Please specify again." 1
                    DevErrorWindow "Please specify a unique interval name."
                }
            }
        $f.bApply config -font $::IbrowserController(UI,Medfont) 

        button $f.bCancel -text "Cancel" -width 4 -bg #DDDDDD \
            -command "destroy $w"
        $f.bCancel config -font $::IbrowserController(UI,Medfont)
        
        pack $f.bApply $f.bCancel -side left  -ipadx 3 -ipady 3 -padx 4
        IbrowserRaisePopup $w
    }
}





#-------------------------------------------------------------------------------
# .PROC IbrowserDeleteIntervalPopUp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeleteIntervalPopUp { win id x y } {

    set noneID $::Ibrowser(none,intervalID)

    if { $id != $noneID } {
        set w .w$win
        if { [IbrowserRaisePopup $w] == 1} {return}

        set title "Delete interval"
        IbrowserCreatePopup $w $title $x $y 

        set f $w
        frame $f.fMsg  -background #FFFFFF 
        frame $f.fClose -background #FFFFFF 
        pack $f.fMsg $f.fClose -side top -pady 4 -padx 4 
        
        set f $w.fMsg
        eval {label $f.l -text "Delete interval $::Ibrowser($id,name) and its contents?"} 
        $f.l config -font $::IbrowserController(UI,Medfont) -background #FFFFFF
        pack $f.l -padx 5 -pady 5

        set ::Ibrowser(thisName) $::Ibrowser($id,name)

        #dismiss
        #---------------
        set f $w.fClose
        button $f.bDelete -text "Delete" -width 4 -bg #DDDDDD \
            -command {
                    IbrowserDeleteInterval $::Ibrowser(thisName)
                    destroy .wdeletepopup
            }
        $f.bDelete config -font $::IbrowserController(UI,Medfont) 

        button $f.bCancel -text "Cancel" -width 4 -bg #DDDDDD \
            -command "destroy $w"
        $f.bCancel config -font $::IbrowserController(UI,Medfont)
        
        pack $f.bDelete $f.bCancel -side left  -ipadx 3 -ipady 3 -padx 4
        IbrowserRaisePopup $w
    }

}


#-------------------------------------------------------------------------------
# .PROC IbrowserOrderPopUp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserOrderPopUp { win id x y } {
    
    set noneID $::Ibrowser(none,intervalID)
    if { $id != $noneID } {
        set w .w$win
        if { [IbrowserRaisePopup $w] == 1} {return}

        set title "Reorder intervals"
        IbrowserCreatePopup $w $title $x $y 
        set ::Ibrowser(afterName) "none"
        set ::Ibrowser(thisName) $::Ibrowser($id,name)

        set f $w
        frame $f.fMsg  -background #FFFFFF 
        frame $f.fBtns 
        frame $f.fClose -background #FFFFFF 
        pack $f.fMsg $f.fBtns $f.fClose -side top -pady 4 -padx 4 
        
        set f $w.fMsg
        eval {label $f.l -text "move $::Ibrowser($id,name) after: "} 
        $f.l config -font $::IbrowserController(UI,Medfont) -background #FFFFFF
        pack $f.l -padx 5 -pady 5 

        #--- menubutton with pulldown menu
        set f $w.fBtns
        eval { menubutton $f.mbIvalOrder -text \
                   $::Ibrowser(afterName) \
                   -relief raised -bd 2 -width 25 \
                   -background #DDDDDD \
                   -menu $f.mbIvalOrder.m }
        pack $f.mbIvalOrder -side top -pady 1 -padx 1
        set ::Ibrowser(guiOrderMenuButton) $f.mbIvalOrder

        #--- menu and commands
        eval { menu $f.mbIvalOrder.m -background #FFFFFF \
               }
        foreach ivalID $::Ibrowser(idList) {
            set name $::Ibrowser($ivalID,name)
            if { $name != $::Ibrowser($id,name) } {
                $f.mbIvalOrder.m add command -label $name \
                    -command "IbrowserSetupNewNames $::Ibrowser($id,name) $name"
            }
        }
        
        #dismiss
        #---------------
        set f $w.fClose
        button $f.bApply -text "Apply" -width 4 -bg #DDDDDD \
            -command {
                if { $::Ibrowser(afterName) != "" } {
                    IbrowserInsertIntervalAfterInterval $::Ibrowser(thisName) $::Ibrowser(afterName)
                    destroy .worderpopup
                }
            }
        $f.bApply config -font $::IbrowserController(UI,Medfont) 

        button $f.bCancel -text "Cancel" -width 4 -bg #DDDDDD \
            -command "destroy $w"
        $f.bCancel config -font $::IbrowserController(UI,Medfont)
        
        pack $f.bApply $f.bCancel -side left  -ipadx 3 -ipady 3 -padx 4
        IbrowserRaisePopup $w
    }
}


#-------------------------------------------------------------------------------
# .PROC IbrowserSetupNewNames
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetupNewNames { this after } {

    set ::Ibrowser(thisName) $this
    set ::Ibrowser(afterName) $after
    $::Ibrowser(guiOrderMenuButton) config -text $::Ibrowser(afterName)
}





#-------------------------------------------------------------------------------
# .PROC IbrowserOpacityPopUp
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserOpacityPopUp { win id x y } {
    
    set noneID $::Ibrowser(none,intervalID)
    set typeTest 0
    #--- only volumes and models can have opacity settings.
    if { ( $::Ibrowser($id,type) == "Image" ) || ($::Ibrowser($id,type) == "Geom") } {
        set typeTest 1
    }
    if { ($id != $noneID) && ( $typeTest ) } {
        set w .w$win
        if { [IbrowserRaisePopup $w] == 1} {return}

        set title "Set opacity"
        set msg "Adjust opacity for $::Ibrowser($id,name)"
        IbrowserCreatePopup $w $title $x $y 

        set f $w
        frame $f.fMsg  -background #FFFFFF 
        frame $f.fSlider -background #FFFFFF
        frame $f.fButton -background #FFFFFF
        pack $f.fMsg $f.fSlider $f.fButton -side top -pady 4 -padx 4 
        
        set f $w.fMsg
        eval {label $f.label -text "$msg"} 
        $f.label config -font $::IbrowserController(UI,Smallfont) -font $::IbrowserController(UI,Medfont) \
            -background #FFFFFF
        pack $f.label -padx 5 -pady 4

        set f $w.fSlider
        #--- for a volume, this is a fade; for a model, it's an opacity
        eval { scale $f.sOpacity -orient horizontal \
                   -from 0.0 -to 1.0 -resolution 0.01 \
                   -length 100 -state active -variable ::Ibrowser(opacity) \
                   -showvalue 1 -background #FFFFFF \
                   -highlightbackground #FFFFFF \
                   -font $::IbrowserController(UI,Smallfont) -highlightcolor #FFFFFF 
        }
        bind $f.sOpacity <B1-Motion> "set $::Ibrowser($id,opacity) $::Ibrowser(opacity)"
        pack $f.sOpacity -padx 4 -pady 4 -side top

        set f $w.fButton

        #dismiss
        #---------------
        button $f.bClose -text "Close" -width 4 -bg #DDDDDD \
            -command "destroy $w"
        pack $f.bClose -ipadx 4 -ipady 4 -side top

        IbrowserRaisePopup $w
    }
}

