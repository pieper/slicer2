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
# FILE:        MainAlignments.tcl
# PROCEDURES:  
#   MainAlignmentsInit
#   MainAlignmentsUpdateMRML
#   MainAlignmentsBuildVTK
#   MainAlignmentsCreate
#   MainAlignmentsDelete
#   MainAlignmentsSetActive
#   AlignmentsSetMatrix str
#   AlignmentsValidateMatrix
#   AlignmentsSetMatrixIntoNode m
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainAlignmentsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsInit {} {
    global Module Matrix

    # Define Procedures
    lappend Module(procVTK)  MainAlignmentsBuildVTK

    # Set version info
    lappend Module(versions) [ParseCVSInfo MainAlignments \
            {$Revision: 1.1 $} {$Date: 2002/07/26 03:26:00 $}]

    # Append widgets to list that gets refreshed during UpdateMRML
    set Matrix(mbActiveList) ""
    set Matrix(mActiveList)  ""

    set Matrix(activeID) ""
    set Matrix(regTranLR)  0
    set Matrix(regTranPA)  0
    set Matrix(regTranIS)  0
    set Matrix(regRotLR)   0
    set Matrix(regRotPA)   0
    set Matrix(regRotIS)   0
    set Matrix(rotAxis) ""
    set Matrix(freeze) ""

    # Props
    set Matrix(name) "manual"
    set Matrix(desc) ""

    # size of current matrix
    set Matrix(rows) {0 1 2 3}
    set Matrix(cols) {0 1 2 3}
    # Initialize to default matrix, I
    AlignmentsSetMatrix "1 0 0 0  0 1 0 0  0 0 1 0  0 0 0 1"


}

#-------------------------------------------------------------------------------
# .PROC MainAlignmentsUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsUpdateMRML {} {
    global Matrix

    # Build any new volumes
    #--------------------------------------------------------
    foreach t $Matrix(idList) {
        if {[MainAlignmentsCreate $t] == 1} {
            # Success
        }
    }    

    # Delete any old volumes
    #--------------------------------------------------------
    foreach t $Matrix(idListDelete) {
        if {[MainAlignmentsDelete $t] == 1} {
            # Success
        }
    }
    # Did we delete the active volume?
    if {[lsearch $Matrix(idList) $Matrix(activeID)] == -1} {
        MainAlignmentsSetActive [lindex $Matrix(idList) 0]
    }

    # Form the menu
    #--------------------------------------------------------
    foreach m $Matrix(mActiveList) {
        $m delete 0 end
        foreach t $Matrix(idList) {
            $m add command -label [Matrix($t,node) GetName] \
                    -command "MainAlignmentsSetActive $t"
        }
    }

    # In case we changed the name of the active transform
    MainAlignmentsSetActive $Matrix(activeID)
}

#-------------------------------------------------------------------------------
# .PROC MainAlignmentsBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsBuildVTK {} {
    global Matrix

    vtkMatrix4x4 Matrix(rotMatrix)
}

#-------------------------------------------------------------------------------
# .PROC MainAlignmentsCreate
#
# Returns:
#  1 - success
#  0 - already built this volume
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsCreate {t} {
    global View Matrix Gui Dag Lut

    # If we've already built this volume, then do nothing
    if {[info command Matrix($t,transform)] != ""} {
        return 0
    }

    # We don't really use this, I just need to mark that it's created
    vtkTransform Matrix($t,transform)

    return 1
}


#-------------------------------------------------------------------------------
# .PROC MainAlignmentsDelete
#
# Returns:
#  1 - success
#  0 - already deleted this volume
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsDelete {t} {
    global Matrix

    # If we've already deleted this transform, then return 0
    if {[info command Matrix($t,transform)] == ""} {
        return 0
    }

    # Delete VTK objects (and remove commands from TCL namespace)
    Matrix($t,transform)  Delete

    # Delete all TCL variables of the form: Matrix($t,<whatever>)
    foreach name [array names Matrix] {
        if {[string first "$t," $name] == 0} {
            unset Matrix($name)
        }
    }

    return 1
}


#-------------------------------------------------------------------------------
# .PROC MainAlignmentsSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainAlignmentsSetActive {t} {
    global Matrix

    if {$Matrix(freeze) == 1} {return}
    
    # Don't reset the rotAxis if we're not changing the active matrix.
    # Just update the GUI. NOTE: Registration fails without this section.
    if {$t != "" && $t != "NEW" && $t == $Matrix(activeID)} {
        set Matrix(name)   [Matrix($t,node) GetName]
        set Matrix(desc)   [Matrix($t,node) GetDescription]
        AlignmentsSetMatrix [Matrix($t,node) GetMatrix]
        set mat [[Matrix($t,node) GetTransform] GetMatrix]
        set Matrix(regTranLR) [$mat GetElement 0 3]
        set Matrix(regTranPA) [$mat GetElement 1 3]
        set Matrix(regTranIS) [$mat GetElement 2 3]
        return
    }
    
    # Set activeID to t
    set Matrix(activeID) $t

    set Matrix(rotAxis) ""
    Matrix(rotMatrix) Identity

    if {$t == ""} {
        # Change button text
        foreach mb $Matrix(mbActiveList) {
            $mb config -text None
        }
        return
    } elseif {$t == "NEW"} {
        # Change button text
        foreach mb $Matrix(mbActiveList) {
            $mb config -text "NEW"
        }
        # Use defaults to update GUI
        vtkMrmlMatrixNode default
        set Matrix(name)   "manual"
        set Matrix(desc)   [default GetDescription]
        AlignmentsSetMatrix [default GetMatrix]
        default Delete
        set Matrix(regTranLR) 0
        set Matrix(regTranPA) 0
        set Matrix(regTranIS) 0
    } else {
        # Change button text
        foreach mb $Matrix(mbActiveList) {
            $mb config -text [Matrix($t,node) GetName]
        }
        # Update GUI
        set Matrix(name)   [Matrix($t,node) GetName]
        set Matrix(desc)   [Matrix($t,node) GetDescription]
        AlignmentsSetMatrix [Matrix($t,node) GetMatrix]
        set mat [[Matrix($t,node) GetTransform] GetMatrix]
        set Matrix(regTranLR) [$mat GetElement 0 3]
        set Matrix(regTranPA) [$mat GetElement 1 3]
        set Matrix(regTranIS) [$mat GetElement 2 3]
    }

    # Update GUI
    foreach item "regRotLR regRotPA regRotIS" {
        set Matrix($item) 0
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetMatrix
# Set the matrix displayed on the GUI.
# .ARGS
# string str 16 numbers in row-major order.
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetMatrix {str} {
    global Matrix

    set count 0

    foreach i $Matrix(rows) {
        foreach j $Matrix(cols) {
            set Matrix(matrix,$i,$j) [lindex $str $count]
            incr count
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsValidateMatrix
# Validate each number in the matrix in the GUI.
# If a number is no good (not a float), pops up an error window.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsValidateMatrix {} {
    global Matrix

    set okay 1

    foreach i $Matrix(rows) {
        foreach j $Matrix(cols) {
            if {[ValidateFloat $Matrix(matrix,$i,$j)] == 0} {
                set okay 0
                set badrow $i
                set badcol $j
                set badnum $Matrix(matrix,$i,$j)
            }
        }
    }

    if {$okay == 0} {
        tk_messageBox -message \
                "The matrix must be 16 numbers \n\
                to form a 4-by-4 row-major matrix,\n\
                but '$badnum' at row $badrow, column $badcol \n\
                is not a floating point number."
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetMatrixIntoNode
# Set the matrix from the GUI into a vtkMrmlMatrixNode.
# .ARGS
# int m ID number of the Matrix node to set the matrix for
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetMatrixIntoNode {m} {
    global Matrix 

    # this replaces the old code:
    #Matrix($m,node) SetMatrix $Matrix(matrix)

    set str ""
    foreach i $Matrix(rows) {
        foreach j $Matrix(cols) {
            set str "$str $Matrix(matrix,$i,$j)"
            puts $str
        }
    }
    puts "---- $str"

    Matrix($m,node) SetMatrix $str
}
