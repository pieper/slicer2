#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        MainTensor.tcl
# PROCEDURES:  
#   MainTensorAddTensor
#   MainTensorInit
#   MainTensorUpdateMRML
#   MainTensorSetActive
#   MainTensorValidateUserInput
#   MainTensorSetAllVariablesToNode
#   MainTensorGetAllVariablesFromNode
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainTensorAddTensor
#  testing.......
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorAddTensor {} {
    global Tensor Module

    #puts "in MainTensorAddTensor"
    if {[IsModule Tensor] == 1} {
    # Lauren simplify this junk, put something in MainData.tcl?
    #set Tensor(propertyType) Basic
    set Tensor(propertyType) Props
    TensorSetPropertyType
    MainDataSetActive Tensor NEW
    #MainTensorSetActive NEW
    # disallow some user actions while adding a data object
    set Tensor(freeze) 1
    # show the props tab
    Tab Tensor row1 Props
    # tab to return to after the freeze
    set Module(freezer) "Data row1 List"
    }
    
    
    #puts "exiting MainTensorAddTensor"
}

#-------------------------------------------------------------------------------
# .PROC MainTensorInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorInit {} {
    global Module Tensor

    # Call "superclass constructor" for slicer data object handling
    #-------------------------------------------
    MainDataInitialize Tensor
    
    # Define Procedures
    #-------------------------------------------
    set m MainTensor
    # Lauren why were the main mrml routines called in order in Main.tcl?
    # this is weird since only modules are supposed to do this, so it 
    # doesn't work:
    #set Module($m,procMRML) {MainDataUpdateMRML Tensor}
    lappend Module(procMRML) MainTensorUpdateMRML

    # Set version info
    #-------------------------------------------
    lappend Module(versions) [ParseCVSInfo MainTensor \
        {$Revision: 1.15 $} {$Date: 2004/11/03 22:15:04 $}]


    # List variables that shadow the active MRML node 
    # These should correspond exactly to vtkMrmlTensor.h
    # These will be automatically set into the node when
    # MainTensorSetAllVariablesToNode is called.
    # Each of these should have a GUI interface created
    # in the module (tcl-modules/Tensor.tcl)
    #------------------------------------
    # vtkMrmlNode (superclass) attributes
    lappend Tensor(MrmlNode,tclVars) {Name String}
    lappend Tensor(MrmlNode,tclVars) {Description String}
    # vtkMrmlTensorNode attributes
    # Lauren we don't have this in the regular volume node!
    lappend Tensor(MrmlNode,tclVars) {FileName String}

    set Tensor(idNode) -1
    set Tensor(activeID) ""
    set Tensor(freeze) ""
 
    # Append widgets to list that gets refreshed during UpdateMRML
    set Tensor(mbActiveList) ""
    set Tensor(mActiveList) ""

    # Lauren we need more attribs, many are so much like volumes...

    # Init variables that shadow active MRML node
    #-------------------------------------------
    # Lauren uncomment this in CVS when check in any nodes
    MainTensorGetAllVariablesFromNode "default"
}

#-------------------------------------------------------------------------------
# .PROC MainTensorSetActive
# Just a wrapper function that calls MainDataSetActive Tensor
# .ARGS
# .END
#-------------------------------------------------------------------------------
#proc MainTensorSetActive {id} {
    #puts "Lauren in MainTensorUpdateMRML"
#    MainDataSetActive Tensor $id

#}

proc MainTensorSetActive {Module id} {
    #puts "Lauren in MainTensorUpdateMRML"
    MainDataSetActive Tensor $id

}
#-------------------------------------------------------------------------------
# .PROC MainTensorUpdateMRML
# Just a wrapper function that calls MainDataUpdateMRML Tensor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorUpdateMRML {} {
    # puts "Lauren in MainTensorUpdateMRML"
    
    MainDataUpdateMRML Tensor

}

#-------------------------------------------------------------------------------
# .PROC MainTensorValidateUserInput
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorValidateUserInput {} {
    global Tensor

    puts "Lauren move the validation and node-set/get into MainData.tcl, \
        then write wrappers around them for extra module junk"

    # Lauren perhaps this should be in module file
    foreach variableInfo $Tensor(MrmlNode,tclVars) {
    set variable [lindex $variableInfo 0]
    set type [lindex $variableInfo 1]

    set ok 1

    if {$type != "String"} {
        set ok [Validate$type $Tensor($variable)]
    }

    if {$ok != "1"} {
        puts "Lauren ERROR, $variable did not validate as $type"
        return 0
    }
    }

    # Lauren need to write wrappers around validate fcns
    # that return a reasonable error message somehow
    # ->perhaps a label for each tcl shadow var in list, can use that
    # for the GUI too?
    
    return 1
}

#-------------------------------------------------------------------------------
# .PROC MainTensorSetAllVariablesToNode
#
#  Set all TCL variable values into the node they shadow.
#  Uses the list of node variables and datatypes, each element
#  of which has this format: {VariableName Type}
#  returns 1 on success
#  The programmer must call MainTensorValidateUserInput first 
#  to check that parameters currently set in the GUI are valid.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorSetAllVariablesToNode {{n \$Tensor(activeID)}} {
    global Tensor

    puts "MainTensorSetAllVariablesToNode $n"
    # Check all user input
    #-------------------------------------------
    #set errcode [MainTensorValidateUserInput]
    #if {$errcode != "1"} {
    #return $errcode
    #}

    # Check the node exists
    #-------------------------------------------
    if {$n == $Tensor(idNone)} {
    puts "Trying to set vars of none node"
    return 0
    } 

    # Get vtk node object
    #-------------------------------------------
    set node Tensor($n,node)

    # Set into the node all TCL vars which shadow it
    #-------------------------------------------
    foreach variableInfo $Tensor(MrmlNode,tclVars) {
    set variable [lindex $variableInfo 0]
    $node Set$variable $Tensor($variable)
    }
}



#-------------------------------------------------------------------------------
# .PROC MainTensorGetAllVariablesFromNode
#  Set all tcl variables (i.e. displayed in the GUI, etc.)
#  which reflect current state of the active vtk tensor node.
#  If called with the paremeter "default" will set all tcl
#  vars using the default node settings from the c++ constructor.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorGetAllVariablesFromNode {n} {
    global Tensor

    # Lauren temporary fix for CVS since no tensor node there yet
    return

    puts "Lauren set tensors vars from node $n"
    
    # Find the node to use
    #-------------------------------------------
    set deleteNode 0
    if {$n == "default" || $n == $Tensor(idNone)} {
    # make temporary default node
    vtkMrmlTensorNode tmp
    set node tmp
    set deleteNode 1
    } else {
    # get active node
    set node Tensor($n,node)
    }

    # Get all TCL vars which shadow this node
    #-------------------------------------------
    foreach variableInfo $Tensor(MrmlNode,tclVars) {
    set variable [lindex $variableInfo 0]
    puts $variable
    # Get the value from the node and save in tcl-land
    set err [catch {set Tensor($variable) [$node Get$variable]}]
    if {$err != "0" } {
        puts "MainTensor Set all vars to node error: $err"
    }
    }
    
    # Remove temporary node if created
    #-------------------------------------------
    if {$deleteNode == "1"} {
    $node Delete
    }
}

