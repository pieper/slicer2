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
# FILE:        MainTensor.tcl
# PROCEDURES:  
#   MainTensorAddTensor
#   MainTensorInit
#   MainTensorUpdateMRML
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
	    {$Revision: 1.5 $} {$Date: 2002/02/03 22:09:13 $}]


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

    # Lauren we need more attribs, many are so much like volumes...

    # Init variables that shadow active MRML node
    #-------------------------------------------
    # Lauren uncomment this in CVS when check in any nodes
    MainTensorGetAllVariablesFromNode "default"
}

#-------------------------------------------------------------------------------
# .PROC MainTensorUpdateMRML
# Just a wrapper function that calls MainDataUpdateMRML Tensor
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainTensorUpdateMRML {} {
    puts "Lauren in MainTensorUpdateMRML"
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


##################################################################
##  INTERACTIVE VISUALIZATION SECTION
##  Lauren should this be here or in the module or in a vtk class?
##################################################################
