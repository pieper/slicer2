#=auto==========================================================================
#   Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: MRAblationHelpText.tcl,v $
#   Date:      $Date: 2006/03/08 18:39:22 $
#   Version:   $Revision: 1.1.2.2 $
# 
#===============================================================================
# FILE:        MRAblationHelpText.tcl
# PROCEDURES:  
#   MRAblationHelpComputing
#==========================================================================auto=

proc MRAblationGetHelpWinID { } {
    if {! [info exists ::MRAblation(newID) ]} {
        set ::MRAblation(newID) 0
    }
    incr ::MRAblation(newID)
    return $::MRAblation(newID)
}


#-------------------------------------------------------------------------------
# .PROC MRAblationHelpComputing
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRAblationHelpComputing { } {
    #--- Compute 
    #--- compute thermal volumes 
    set i [ MRAblationGetHelpWinID ]
    set txt "<H3>Computing thermal volumes</H3>
<P> Thermal volumes are computed upon the following settings: 
<P> <B>Cold images</B>: In general, images in phase 1 are cold images. However, you can choose cold images from any available phases. 
<P> <B>Hot images</B>: You may pick up any phase (except that for cold images) for hot images or just choose \"all\" on the list. If a specific phase is chosen, only one thermal volume will be computed. If \"all\" is selected, multiple thermal volumes will be computed, one for each hot phase.
<P> <B>Real images</B>: Just verify your real images. 
<P> <B>Imaginary images</B>: Just verify your imaginary images. 

<P> If button <I>One volume per phase</I> is checked, phase 1 will be automatically chosen for cold images and \"all\" selected for hot images if at least 3 phases are available within the experiment." 


    DevCreateTextPopup infowin$i "MRAblation information" 100 100 18 $txt
}

