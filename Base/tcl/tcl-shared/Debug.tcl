# Debug.tcl
# 10/16/98 Peter C. Everett peverett@bwh.harvard.edu: Created

#-------------------------------------------------------------------------------
# PROCEDURES in order
#-------------------------------------------------------------------------------
# DebugInit
# DebugMsg

#-------------------------------------------------------------------------------
# DebugInit
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
