#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
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
# FILE:        SharedFunctions.tcl
# PROCEDURES:  
#   SharedModelLookup
#   SharedVolumeLookup
#   SharedGetModelsInGroup the a if only
#   SharedGetModelsInGroupOnly the a
#   SharedGetModelGroupsInGroup
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC SharedModelLookup
# Gets the internal model ID that belongs to a given alphanumerical model ID.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SharedModelLookup {ModelRefID} {
    global Model
    
    set ModelID -1
    
    foreach m $Model(idList) {
        if {[Model($m,node) GetModelID] == $ModelRefID} {
            set ModelID [Model($m,node) GetID]
        }
    }
    return $ModelID
}


#-------------------------------------------------------------------------------
# .PROC SharedVolumeLookup
# Gets the internal volume ID that belongs to a given alphanumerical volume ID.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SharedVolumeLookup {VolumeRefID} {
    global Volume
    
    set VolumeID -1
    
    foreach v $Volume(idList) {
        if {[Volume($v,node) GetVolumeID] == $VolumeRefID} {
            set VolumeID [Volume($v,node) GetID]
        }
    }
    return $VolumeID
}


#-------------------------------------------------------------------------------
# .PROC SharedGetModelsInGroup
# Gets all the models in a model group (including all dependent model groups).
# .ARGS
# modelgroup: the group where to get the dependent models from
# umodels: a list where the models are stored
# changeExpansion: if >=0, change the variable Model(id,expansion) to this value,
# but only in the group $modelgroup
# .END
#-------------------------------------------------------------------------------
proc SharedGetModelsInGroup {modelgroup umodels {changeExpansion -1}} {
    global Mrml(dataTree) Model
    
    upvar $umodels models
    
    Mrml(dataTree) InitTraversal
    set node [Mrml(dataTree) GetNextItem]
    
    set traversingModelGroup 0
    set models {}
    
    while {$node != ""} {

        if {[string compare -length 10 $node "ModelGroup"] == 0} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup
            }
            if {[$node GetID] == $modelgroup} {
                incr traversingModelGroup
            }
        }
        if {[string compare -length 13 $node "EndModelGroup"] == 0} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup -1
            }
        }
        
        if {([string compare -length 8 $node "ModelRef"] == 0) && ($traversingModelGroup > 0)} {
            set m [SharedModelLookup [$node GetModelRefID]]
            lappend models $m
            if {($traversingModelGroup == 1) && ($changeExpansion >= 0)} {
                set Model($m,expansion) $changeExpansion
            }            
        }
        set node [Mrml(dataTree) GetNextItem]
    }
}


#-------------------------------------------------------------------------------
# .PROC SharedGetModelsInGroupOnly
# Gets all the models in a model group (without dependent model groups).
# .ARGS
# modelgroup: the group where to get the dependent models from
# umodels: a list where the models are stored
# .END
#-------------------------------------------------------------------------------
proc SharedGetModelsInGroupOnly {modelgroup umodels} {
    global Model
    
    upvar $umodels models
    
    Mrml(dataTree) InitTraversal
    set node [Mrml(dataTree) GetNextItem]
    
    set traversingModelGroup 0
    set models ""
    
    while {$node != ""} {
        if {[string equal -length 10 $node "ModelGroup"] == 1} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup
            }
            if {[$node GetID] == $modelgroup} {
                incr traversingModelGroup
            }
        }
        if {[string equal -length 13 $node "EndModelGroup"] == 1} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup -1
            }
        }
        
        if {([string equal -length 8 $node "ModelRef"] == 1) && ($traversingModelGroup == 1)} {
            set m [SharedModelLookup [$node GetModelRefID]]
            lappend models $m
        }
        set node [Mrml(dataTree) GetNextItem]
    }
}


#-------------------------------------------------------------------------------
# .PROC SharedGetModelGroupsInGroup
# Gets all model groups which depend of a given model group.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SharedGetModelGroupsInGroup {modelgroup umodelgroups} {
    global Mrml(dataTree)
    
    upvar $umodelgroups mgs
    
    Mrml(dataTree) InitTraversal
    set node [Mrml(dataTree) GetNextItem]
    
    set traversingModelGroup 0
    set mgs {}
    
    while {$node != ""} {

        if {[string compare -length 10 $node "ModelGroup"] == 0} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup
                lappend mgs [$node GetID]
            }
            if {[$node GetID] == $modelgroup} {
                incr traversingModelGroup
            }
        }
        if {[string compare -length 13 $node "EndModelGroup"] == 0} {
            if {$traversingModelGroup > 0} {
                incr traversingModelGroup -1
            }
        }
        set node [Mrml(dataTree) GetNextItem]
    }
}
