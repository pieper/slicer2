#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        FMRIEngineParadigmParser.tcl
# PROCEDURES:  
#   FFMRIEngineParseParadigm 
#   FMRIEngineCheckParadigm
#   FMRIEngineStoreParadigm
#   FMRIEngineSaveParadigm
#   FFMRIEngineCreateStimulusArray 
#==========================================================================auto=
#===============================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
#===============================================================================
#-------------------------------------------------------------------------------
# .PROC FFMRIEngineParseParadigm 
# Parses the paradigm file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineParseParadigm {} {
    global FMRIEngine

    if {[file exists $FMRIEngine(paradigmFileName)] == 0} { 
        DevErrorWindow "Paradigm file doesn't exist: $FMRIEngine(paradigmFileName)"
        return 0
    }
 
    # if FMRIEngine(paradigm) exists, unset it
    if {[info exists FMRIEngine(paradigm)]} { 
        unset FMRIEngine(paradigm)
        FMRIEngine(paradigmArray) Delete 
    }

    # Reads the data file
    set fp [open $FMRIEngine(paradigmFileName) r]
    set data [read $fp]
    set lines [split [string trim $data] "\n"]

    # Values of parameters in paradigm.txt are saved in list FMRIEngine(paradigm).
    # Here is the index order:
    # 0 --- Total Volumes (85)  
    # 1 --- Number Of Conditions (1)
    # 2 --- Volumes Per Baseline (5)
    # 3 --- Volumes Per Conditions (5)
    # 4 --- Volumes At Start (5)
    # 5 --- Starts With (0)
    vtkFloatArray FMRIEngine(paradigmArray)     
    FMRIEngine(paradigmArray) SetNumberOfTuples 6 
    FMRIEngine(paradigmArray) SetNumberOfComponents 1
    set count 0
    foreach line $lines {
        set pair [split $line ":"]
        set val [string trim [lindex $pair 1]]
        lappend FMRIEngine(paradigm) $val 
        FMRIEngine(paradigmArray) SetComponent $count 0 $val 

        incr count
    }
    close $fp

    set i 0
    set len [llength $FMRIEngine(paradigm)]
    while {$i < $len} { 

        set param [lindex $FMRIEngine(paramVariables) $i]
        set val [lindex $FMRIEngine(paradigm) $i]
        set FMRIEngine($param) $val
        incr i
    }

    FMRIEngineCreateStimulusArray

    return 1 
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineCheckParadigm
# Checks paradigm parameters
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCheckParadigm {} {
    global FMRIEngine 

    foreach param "tVols nStims bVols cVols sVols start" { 

        set val [string trim $FMRIEngine($param)]
        if {$val == "" || [string is digit -strict $val] == 0} {
            DevErrorWindow "Make sure you have valid input for all fields."
            return 0
        }
    }

    return 1
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineStoreParadigm
# Stores the parameters in memory for activation computation
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineStoreParadigm {} {
    global FMRIEngine 

    set check [FMRIEngineCheckParadigm]

    if {$check} {

        # if FMRIEngine(paradigm) exists, unset it
        if {[info exists FMRIEngine(paradigm)]} { 
            unset FMRIEngine(paradigm)
        }

        set i 0      
        set len [llength $FMRIEngine(paramNames)]
        while {$i < $len} { 

            set param [lindex $FMRIEngine(paramVariables) $i]
            lappend FMRIEngine(paradigm) $FMRIEngine($param) 
            incr i
        }

        FMRIEngineCreateStimulusArray
    }
}


#-------------------------------------------------------------------------------
# .PROC FMRIEngineSaveParadigm
# Saves the parameters into a text file 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineSaveParadigm {} {
    global FMRIEngine 

    set check [FMRIEngineCheckParadigm]

    if {$check} {
        set types {
            {{Text Files} {.txt}}
            {{All Files}  *}
        }

        set fileName \
            [tk_getSaveFile -defaultextension ".txt" \
            -initialdir "" \
            -filetypes $types \
            -initialfile $FMRIEngine(paradigmFileName) \
            -title "Save paradigm parameters"]

        if {$fileName != ""} {

            # if FMRIEngine(paradigm) exists, unset it
            if {[info exists FMRIEngine(paradigm)]} { 
                unset FMRIEngine(paradigm)
            }

            set fp [open $fileName "w"]
            set i 0      
            set len [llength $FMRIEngine(paramNames)]
            while {$i < $len} { 

                set name [lindex $FMRIEngine(paramNames) $i]
                set param [lindex $FMRIEngine(paramVariables) $i]
                set data "$name: $FMRIEngine($param)\n"

                lappend FMRIEngine(paradigm) $FMRIEngine($param) 

                # Failure to add '-nonewline' will result in 
                # an extra newline at the end of the file
                puts -nonewline $fp $data

                incr i
            }

            # close the file, ensuring the data is written out before you continue
            #  with processing.
            close $fp

            set FMRIEngine(paradigmFileName) $fileName
            FMRIEngineCreateStimulusArray
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC FFMRIEngineCreateStimulusArray 
# Creates a stimulus array 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc FMRIEngineCreateStimulusArray {} {
    global FMRIEngine

    # if the stimulus array exists, remove it
    if {[info commands FMRIEngine(stimulus)] != ""} {
        FMRIEngine(stimulus) Delete
    }
    unset -nocomplain FMRIEngine(stimulus)

    set totalVolumes [lindex $FMRIEngine(paradigm) 0]
    vtkFloatArray FMRIEngine(stimulus)     
    FMRIEngine(stimulus) SetNumberOfTuples $totalVolumes 
    FMRIEngine(stimulus) SetNumberOfComponents 1

    set count 0
    set doTask [lindex $FMRIEngine(paradigm) 5]
    while {$count < $totalVolumes} {
        if {$count == 0} {
            set length [lindex $FMRIEngine(paradigm) 4] 
        } else {
            set length [expr {$doTask ? [lindex $FMRIEngine(paradigm) 3] : \
                       [lindex $FMRIEngine(paradigm) 2] }]
        }

        set i 0
        while {$i < $length && $count < $totalVolumes} {
            FMRIEngine(stimulus) SetComponent $count 0 [expr {$doTask ? 1.0 : 0.0}] 
            incr count 
            incr i
        }
        set doTask [expr {!$doTask}] 
    }
}

 
