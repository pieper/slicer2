#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerDrops.tcl,v $
#   Date:      $Date: 2005/12/20 22:55:59 $
#   Version:   $Revision: 1.3.8.2 $
# 
#===============================================================================
# FILE:        IbrowserControllerDrops.tcl
# PROCEDURES:  
#   IbrowserSetupDropImages
#   IbrowserCreateImageDrops
#   IbrowserCreateDataDrops
#   IbrowserCreateEventDrops
#   IbrowserCreateCommandDrops
#   IbrowserCreateGeometryDrops
#   IbrowserCreateNoteDrops
#   IbrowserMoveIntervalDrops
#   IbrowserDeleteIntervalDrops
#   IbrowserRedrawDrops
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC IbrowserSetupDropImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetupDropImages { } {
    global PACKAGE_DIR_VTKIbrowser

    #--- This variable contains the module path plus some stuff
    #--- trim off the extra stuff, and add on the path to tcl files.
    set tmpstr $PACKAGE_DIR_VTKIbrowser
    set tmpstr [string trimright $tmpstr "/vtkIbrowser" ]
    set tmpstr [string trimright $tmpstr "/Tcl" ]
    set tmpstr [string trimright $tmpstr "Wrapping" ]
    set modulePath [format "%s%s" $tmpstr "tcl/"]

    set ::IbrowserController(Images,Drop,imageDrop) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/canvas/drop.gif]
    set ::IbrowserController(Images,Drop,geomDrop) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/canvas/drop.gif]    
    set ::IbrowserController(Images,Drop,dataDrop) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/canvas/drop.gif]    
    set ::IbrowserController(Images,Drop,eventDrop) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/canvas/drop.gif]    
    set ::IbrowserController(Images,Drop,noteDrop) \
        [image create photo -file ${modulePath}iconPix/20x20/gifs/canvas/drop.gif]    
}


#-------------------------------------------------------------------------------
# .PROC IbrowserCreateImageDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateImageDrops { ivalName posArray numDrops } {
    
    set id $::Ibrowser($ivalName,intervalID)
    upvar $posArray unitpos
   
    #What we want to do, is to indicate each
    #sample's position in the intervalRect.
    #Use ::IbrowserController($id,pixytop) to verticallyposition the Drop
    #and use posArray to position it horizontally.
    #---------------
    for { set i 0} {$i < $numDrops } { incr i } {
        set pixpos [ IbrowserUnitValToPixelVal $unitpos($i) ]
        set ::Ibrowser($id,$i,pos) $pixpos

        set ycenter [ expr $::IbrowserController(Geom,Ival,intervalPixHit) / 2.0 ]
        set ypos [ expr $::IbrowserController($id,pixytop) + $ycenter ]
        
        set ::IbrowserController($id,$i,dropTAG) ${id}_${i}_dropTAG
        set itemtag $::IbrowserController($id,$i,dropTAG)
        #Tag each individual drop, the group of interval drops, and all Interval drops.
        #---------------
        $::IbrowserController(Icanvas) create image $pixpos $ypos \
            -image $::IbrowserController(Images,Drop,imageDrop) \
            -anchor c -tags "$itemtag $::IbrowserController($id,allDROPtag) IbrowserDropTags"

        #And make sure they draw atop the interval rect
        #---------------
        $::IbrowserController(Icanvas) raise IbrowserDropTags
    }

}



#-------------------------------------------------------------------------------
# .PROC IbrowserCreateDataDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateDataDrops { ivalName posArray numDrops} {
}

#-------------------------------------------------------------------------------
# .PROC IbrowserCreateEventDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateEventDrops { ivalName posArray numDrops} {
}

#-------------------------------------------------------------------------------
# .PROC IbrowserCreateCommandDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateCommandDrops { ivalName posArray numDrops} {
}

#-------------------------------------------------------------------------------
# .PROC IbrowserCreateGeometryDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateGeometryDrops { ivalName posArray numDrops} {
}

#-------------------------------------------------------------------------------
# .PROC IbrowserCreateNoteDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserCreateNoteDrops { ivalName posArray numDrops} {
}



#-------------------------------------------------------------------------------
# .PROC IbrowserMoveIntervalDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserMoveIntervalDrops { ivalName oldy newy } {
    
    set id $::Ibrowser($ivalName,intervalID)
    
    set numdrops $::Ibrowser($id,numDrops)
    if { $::Ibrowser($id,numDrops) == 0 } {
        return
    }

    #move an interval's drops by yy
    #---------------
    set yy [ expr $newy - $oldy ]
    if {$yy != 0} {
        $::IbrowserController(Icanvas) move $::IbrowserController($id,allDROPtag) 0 $yy
    }
}




#-------------------------------------------------------------------------------
# .PROC IbrowserDeleteIntervalDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeleteIntervalDrops { ivalName } {
    
    set id $::Ibrowser($ivalName,intervalID)
    
    #delete the GUI representation of the drops
    #---------------
    $::IbrowserController(Icanvas) delete $::IbrowserController($id,allDROPtag) 

    #delete an interval's Drops
    #---------------
    for { set i 0} {$i < $::Ibrowser($id,numDrops) } { incr i } {
        if { [info exists ::Ibrowser($id,$i,data) ] } {
            #--- delete the ImageData and the MrmlNodes...
            unset -nocomplain ::Ibrowser($id,$i,matrixID)
            unset -nocomplain ::Ibrowser($id,$i,transformID)
            unset ::Ibrowser($id,$i,data)
        }
     }
    set ::Ibrowser($id,numDrops) 0

}




#-------------------------------------------------------------------------------
# .PROC IbrowserRedrawDrops
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRedrawDrops { } {

}
