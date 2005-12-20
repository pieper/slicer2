#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerProgressBar.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:00 $
#   Version:   $Revision: 1.2.8.1 $
# 
#===============================================================================
# FILE:        IbrowserControllerProgressBar.tcl
# PROCEDURES:  
#   IbrowserRaiseProgressBar
#   IbrowserLowerProgressBar
#==========================================================================auto=

proc IbrowserUpdateProgressBar  { percentdone fillchar } {

    #--- label width is 20 characters wide, as
    #--- specified in IbrowseControllerAnimate.tcl
    #--- in proc IbrowserMakeAnimationMenu.
    #--- So 20 chars is the max we can display
    #--- convert the percentdone to a fraction of
    #--- 20 chars: percentdone = numchars/20
    set numchars [ expr $percentdone * 20 ]
    set numchars [ expr int ( $numchars ) ]
    
    #--- now assemble 'numchars' of whatever the
    #--- progress bar character will be.
    set progressStr [string repeat $fillchar $numchars ]
    set ::IbrowserController(ProgressBarTxt) $progressStr 
}



#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseProgressBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseProgressBar { } {

    #--- makes progress bar visible by raising its groove
    $::IbrowserController(ProgressBar) configure -anchor nw \
        -foreground #111111 -relief groove
}



#-------------------------------------------------------------------------------
# .PROC IbrowserLowerProgressBar
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserLowerProgressBar { } {

    #--- makes progress bar invisible by blending into background
    $::IbrowserController(ProgressBar) configure -foreground #FFFFFF -relief flat
    set ::IbrowserController(ProgressBarTxt) ""
}
