#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: IbrowserControllerArrayList.tcl,v $
#   Date:      $Date: 2005/12/20 22:55:59 $
#   Version:   $Revision: 1.3.2.1 $
# 
#===============================================================================
# FILE:        IbrowserControllerArrayList.tcl
# PROCEDURES:  
#   IbrowserAddToList
#   IbrowserOrderCompare
#   IbrowserDeleteFromList
#==========================================================================auto=





#-------------------------------------------------------------------------------
# .PROC IbrowserAddToList
# Adds an element to a list.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserAddToList { Aname } {

    #--- move to this list
    set id $::Ibrowser($Aname,intervalID)
    lappend ::Ibrowser(idList) $id

}




#-------------------------------------------------------------------------------
# .PROC IbrowserOrderCompare
# Used to sort the list of interval names
# based on their orders; from lowest to highest.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserOrderCompare { a b } {
    
    set idA $a
    set idB $b

    if { ( [info exists ::Ibrowser($idA,order) ] ) && ( [ info exists ::Ibrowser($idB,order) ] ) } {
        set q $::Ibrowser($idA,order)
        set qq $::Ibrowser($idB,order)

        if { $q > $qq } {
            return 1
        } else {
            return -1
        }
    } else {
        return -1
    }

}





#-------------------------------------------------------------------------------
# .PROC IbrowserDeleteFromList
# Deletes arrayname from list, and
# then deletes the array too
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserDeleteFromList { ival } {
    
    set id $::Ibrowser($ival,intervalID)
    
    # find the list item
    set ix [ lsearch -exact $::Ibrowser(idList) $id ]

    #delete it; otherwise, return orig list
    if { $ix >= 0 } {
        set ::Ibrowser(idList) [ lreplace $::Ibrowser(idList) $ix $ix ]
    } else {
        set tt "IbrowserDeleteFromList: can't find $ival in interval list. Not deleted."
    }

}



