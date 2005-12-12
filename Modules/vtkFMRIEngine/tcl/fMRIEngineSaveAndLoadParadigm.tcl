#=auto==========================================================================
# (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
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
# FILE:        fMRIEngineSaveAndLoadParadigm.tcl
# PROCEDURES:  
#   fMRIEngineSaveParadigm
#   fMRIEngineLoadParadigm
#==========================================================================auto=
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

#-------------------------------------------------------------------------------
# .PROC fMRIEngineSaveParadigm
# Saves the current paradigm 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineSaveParadigm {} {
    global fMRIEngine

    if {! [info exists fMRIEngine(1,designType)]} {
        DevErrorWindow "The paradigm is not ready to save."
        return
    }

    # write data to file
    set fileType {{"Text" *.txt}}
    set fileName [tk_getSaveFile -filetypes $fileType -parent .]
    if {[string length $fileName]} {
        set txt ".txt"
        set ext [file extension $fileName]
        if {$ext != $txt} {
            set fileName "$fileName$txt"
        }
        set fHandle [open $fileName w]
        set comment "# This text file saves the paradigm (Set Up -> Paradigm). Do not edit it. \n"
        puts $fHandle $comment

        set comment "# number of runs"
        puts $fHandle $comment
        set str "set fMRIEngine(noOfRuns) $fMRIEngine(noOfRuns) \n"
        puts $fHandle $str

        for {set r 1} {$r <= $fMRIEngine(noOfRuns)} {incr r} {
            set comment "\n# information for run $r"
            puts $fHandle $comment
            set comment "# --------------------- \n"
            puts $fHandle $comment

            set comment "# design type of run $r"
            puts $fHandle $comment
            set str "set fMRIEngine($r,designType) $fMRIEngine($r,designType) \n"
            puts $fHandle $str

            set comment "# tr of run $r"
            puts $fHandle $comment
            set str "set fMRIEngine($r,tr) $fMRIEngine($r,tr) \n"
            puts $fHandle $str

            set comment "# condition list of run $r"
            puts $fHandle $comment
            set str "set fMRIEngine($r,conditionList) \[list $fMRIEngine($r,conditionList)\] \n"
            puts $fHandle $str

            foreach title $fMRIEngine($r,conditionList) {
                set comment "# condition name in run $r"
                puts $fHandle $comment
                set str "set fMRIEngine($r,$title,title) $title \n"
                puts $fHandle $str

                set comment "# add condition name into model view"
                puts $fHandle $comment
                set str "fMRIModelViewAddConditionName $r $title \n"  
                puts $fHandle $str

                set comment "# start volume number of condition \'$title\' in run $r"
                puts $fHandle $comment
                set str "set fMRIEngine($r,$title,startVol) $fMRIEngine($r,$title,startVol) \n"  
                puts $fHandle $str

                set comment "# onsets of condition \'$title\' in run $r"
                puts $fHandle $comment
                set str "set fMRIEngine($r,$title,onsets) \"$fMRIEngine($r,$title,onsets)\" \n"  
                puts $fHandle $str

                set comment "# durations of condition \'$title\' in run $r"
                puts $fHandle $comment
                set str "set fMRIEngine($r,$title,durations) \"$fMRIEngine($r,$title,durations)\" \n"  
                puts $fHandle $str
            }
        }

        close $fHandle
    }
}


#-------------------------------------------------------------------------------
# .PROC fMRIEngineLoadParadigm
# Loads paradigm from file 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc fMRIEngineLoadParadigm {} {
    global fMRIEngine
 
    # read data from file
    set fileType {{"Text" *.txt}}
    set fileName [tk_getOpenFile -filetypes $fileType -parent .]
    
    set fHandle [open $fileName r]
    set data [read $fHandle]
    set lines [split $data "\n"]
    foreach line $lines {
        set line [string trim $line]
        eval $line
    }
    close $fHandle

    # show all conditions for first run
    if {$fMRIEngine(noOfRuns) > 0} {
        fMRIEngineUpdateRunsForConditionShow 
        fMRIEngineSelectRunForConditionShow 1 
    }

    set ::fMRIEngine(SignalModelDirty) 1
    # update the condition list in signal modeling tab
    fMRIEngineUpdateConditionsForSignalModeling
}
