#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        ismatrixoption.tcl
# PROCEDURES:  
#==========================================================================auto=

#########################################################
#
if {0} { ;# comment
select a Slicer matrix
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
#option add *ismatrixoption.background #cccccc widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class ismatrixoption] == "" } {

    itcl::class ismatrixoption {
        inherit iwidgets::Optionmenu
        
        constructor {args} {}
        destructor {}
        
        #itk_option define -command command Command {}
        #itk_option define -background background Background {}
        
        variable _allowNone
        
        method initSelection {} {}
        method allowNone {{allow "1"}} {}
        method selectedID {} {}
    }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body ismatrixoption::constructor {args} {
    #
    # Initialize the widget based on the command line options.
    #
    set _allowNone "1"
    $this initSelection
    eval itk_initialize $args
}
# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body ismatrixoption::initSelection {} {
    global Matrix

    $this delete 0 end
    set matrixNames ""
    foreach v $Matrix(idList) {
#        if {$v == $Matrix(idNone) } {
#            if {$_allowNone == "1"} {
#                lappend matrixNames "None"
#            }
#        } else {
            lappend matrixNames [Matrix($v,node) GetName]
#        }
    }
    if {$matrixNames != "" && ( $_allowNone == "1" || $matrixNames != "None")} {
        foreach n $matrixNames {
            $this insert end $n
        }
    }
}


itcl::body ismatrixoption::allowNone {{allow "1"}} {
    set _allowNone $allow
}


itcl::body ismatrixoption::selectedID {} {
    global Matrix

    set selIdList ""
    set selectedMatrix [$this get]
    foreach v $Matrix(idList) {
        set index [lsearch -exact $selectedMatrix [Matrix($v,node) GetName]]
        if {$index > -1} {
            lappend selIdList $v
            break;
        }
    }
    return $selIdList
}

