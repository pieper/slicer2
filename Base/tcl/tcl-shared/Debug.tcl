#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        Debug.tcl
# PROCEDURES:  
#   DebugInit
#==========================================================================auto=
# Debug.tcl
# 10/16/98 Peter C. Everett peverett@bwh.harvard.edu: Created

#-------------------------------------------------------------------------------
# PROCEDURES in order
#-------------------------------------------------------------------------------
# DebugInit
# DebugMsg

#-------------------------------------------------------------------------------
# .PROC DebugInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DebugInit {} {
    global Debug
set Debug(cnt) 0
    # Debug(types) is read from the slicer.init file at bootup
    }

#-------------------------------------------------------------------------------
# DebugMsg
# Prints msg if type is contained in Debug(types) list.
# If Debug(types) contains "all", then prints all messages.
#-------------------------------------------------------------------------------
proc DebugMsg { msg {type all} } {
#    global Debug

#    if {[lsearch $Debug(types) "all"] > -1 || 
#        [lsearch $Debug(types) $type] > -1} {
##        puts $msg
#        }
    }
