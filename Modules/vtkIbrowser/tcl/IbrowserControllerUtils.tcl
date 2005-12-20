#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerUtils.tcl,v $
#   Date:      $Date: 2005/12/20 22:56:00 $
#   Version:   $Revision: 1.3.8.1 $
# 
#===============================================================================
# FILE:        IbrowserControllerUtils.tcl
# PROCEDURES:  
#   IbrowserRadians
#   IbrowserNameFromCurrent
#   IbrowserGetAllSequenceNames
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC IbrowserRadians
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRadians x { expr {$x*3.14159/180.0} }
proc IbrowserDegrees x { expr {$x/3.14159*180.0} }

#-------------------------------------------------------------------------------
# .PROC IbrowserNameFromCurrent
# This routine extracts a current canvas item's unique 
# tag from the string passed in, and returns it.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserNameFromCurrent { strg } {

    set cutme " current"
    set thing [ string trimright $strg $cutme ]
    return $thing
}



#-------------------------------------------------------------------------------
# .PROC IbrowserGetAllSequenceNames
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGetAllSequenceNames { } {
    foreach id $::Ibrowser(idList) {
        set name $::Ibrowser(id,name)
        lappend nameList $name
    }
    return $nameList
}

