#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserSaveGUI.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:02 $
#   Version:   $Revision: 1.2.8.1 $
# 
#===============================================================================
# FILE:        IbrowserSaveGUI.tcl
# PROCEDURES:  
#   IbrowserBuildSaveFrame
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserBuildSaveFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildSaveFrame { } {
    global Gui Ibrowser Module Volume

    
    #-------------------------------------------
    #--- Save frame: add buttons and pack
    #-------------------------------------------
    set fSave $::Module(Ibrowser,fSave)
    set f $fSave
    frame $f.fTop -bg $::Gui(activeWorkspace)
    pack $f.fTop -side top -padx 0 -pady $::Gui(pad) 
    frame $f.fBottom -bg $::Gui(activeWorkspace)
    pack $f.fBottom -side top -padx 0 -pady $::Gui(pad) -fill x
    
    set f $fSave.fTop
    set f $fSave.fBottom

}

