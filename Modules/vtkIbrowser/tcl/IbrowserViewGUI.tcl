#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserViewGUI.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:02 $
#   Version:   $Revision: 1.2.8.1 $
# 
#===============================================================================
# FILE:        IbrowserViewGUI.tcl
# PROCEDURES:  
#   IbrowserBuildViewFrame
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserBuildViewFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildViewFrame { } {
    global Gui Ibrowser Module Volume

    
    #-------------------------------------------
    #--- View frame: add buttons and pack
    #-------------------------------------------
    set fView $::Module(Ibrowser,fView)
    set f $fView
    frame $f.fTop -bg $::Gui(activeWorkspace)
    pack $f.fTop -side top -padx 0 -pady $::Gui(pad) 
    frame $f.fBottom -bg $::Gui(activeWorkspace)
    pack $f.fBottom -side top -padx 0 -pady $::Gui(pad) -fill x
    
    set f $fView.fTop
    set f $fView.fBottom

}

