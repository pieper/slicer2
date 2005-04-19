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
# FILE:        EMSegmentBatch.tcl
# PROCEDURES:  
#   EMSegmentBatch1
#   EMSegmentBatch2
#   EMSegmentBatch3
#   EMSegmentBatch4
#   EMSegmentBatch6
#   EMSegmentBatch7
#==========================================================================auto=

# -----------------------------------------------------------------------------
# This script allows the direct access of the vtkImageEMLocalSegmenter Filter. 
# It is very useful for batch processing. Function can be called through
# setenv SLICER_HOME /home/ai2/kpohl/slicer_devel/slicer
# setenv SCRIPT_HOME /home/ai2/kpohl/slicer_devel/slicer/Modules/vtkEMPrivateSegment
# setenv LD_LIBRARY_PATH /home/ai2/kpohl/slicer_devel/pkg/lib:${LD_LIBRARY_PATH}
# setenv SLICER_LOCAL
# exec /home/ai2/kpohl/slicer_devel/pkg/bin/vtk $SLICER_HOME/program/tcl-modules/EMSegment/EMSegmentBatch.tcl <Mrml File Defining Segmentation> <Display Graph Afterwards>
# ------------------------------------------------------------------------------

package require vtk
package require vtkSlicerBase
package require vtkEMLocalSegment

#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch1
# 1. Step Initialize and check Parameters 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch1 {} {
    puts "============================ Start EMSegmentBatch ======================="

    puts "Run Basic Version"
    set ::EMSegment(SegmentMode) 1

    # The following Lines are from vtkImageInclude
    set ::VTK_VOID            0
    set ::VTK_BIT             1 
    set ::VTK_CHAR            2
    set ::VTK_UNSIGNED_CHAR   3
    set ::VTK_SHORT           4
    set ::VTK_UNSIGNED_SHORT  5
    set ::VTK_INT             6
    set ::VTK_UNSIGNED_INT    7
    set ::VTK_LONG            8
    set ::VTK_UNSIGNED_LONG   9
    set ::VTK_FLOAT          10
    set ::VTK_DOUBLE         11 

    set ::VTK_IMAGE_X_AXIS             0
    set ::VTK_IMAGE_Y_AXIS             1
    set ::VTK_IMAGE_Z_AXIS             2

    set ::VTK_CLAW_NEAREST_NETWORK      0
    set ::VTK_CLAW_NEAREST_MINIMUM      1
    set ::VTK_CLAW_NEAREST_GLOBAL       2
    set ::VTK_CLAW_PIONEER_LOCAL        3
    set ::VTK_CLAW_PIONEER_GLOBAL       4
    set ::VTK_CLAW_WELL_NOISE           6
    set ::VTK_CLAW_WELL_DIRECTED_NOISE  7
    set ::VTK_CLAW_MINIMUM_WELL         8
    set ::VTK_CLAW_INSERT               9
    set ::VTK_CLAW_NARROW_WEL           10

    if { ![info exists ::PACKAGE_DIR_VTKMIRIADSEGMENT] } { 
        source [file join $Script_Home tcl/EMSegmentXMLReaderWriter.tcl]   
        source [file join $Slicer_Home Modules/vtkEMLocalSegment/tcl/EMSegmentAlgorithm.tcl]  
        # source [file join $Slicer_Home Base/tcl/tcl-main/Parse.tcl]  
    }

    # Ihave to do this to execute this tcl file from other tcl files . Do not like this setup
    if {$argc < 2} {
        puts "Input for EMSegmentBatch has to be :"
        puts "EMSegmentBatch <Mrml File Defining Segmentation> <Display Graph Afterwards>"
        puts "<Display Graph Afterwards> : 0 = No Graph is displayed to view results 1 = Graph is displayed"  
        exit 1
    }
    set XMLFile        [lindex $argv 0] 
    set DisplayWindow  [lindex $argv 1]

    if {$DisplayWindow == 0} {
        wm withdraw .
    }
    set EMSegment(DebugVolume) 1
    # Should be already sourced  
    #set EMSegment(debug) 0
    #set EMSegment(CIMList) {West North Up East South Down}
    #set EMSegment(ImgTestNo) -1
    #set EMSegment(ImgTestDivision) 0
    #set EMSegment(ImgTestPixel) 0
    #set Volume(idNone) 0
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch2
# 2. Step Load Paramters 
# Read file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch2 {} {
    puts "Load File $XMLFile"
    # Check File for correctness .We have to do it otherwise warning window will pop up in Parse.tcl
    EMSegmentReadXMLFile $XMLFile    
    # Check Values 
    if {$EMSegment(NumInputChannel) == 0} {
        puts "Error::EMSegmentStartEM: No Input channels defined !"
        exit 1
    } 
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch3
# 3. Segment Images
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch3 {} {
    set EMSegment(VolumeNameList) ""
    set Volume(idList) ""
    for {set i 1} {$i < $EMSegment(VolNumber)} {incr i} {
       lappend EMSegment(VolumeNameList)  $Volume($i,Name)
       lappend Volume(idList) $i
    }
    EMSegmentAlgorithmStart
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch4
# 4. Write Result to a File 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch4 {} {
    vtkImageWriter WriteResult 
    WriteResult SetInput [EMSegment(vtkEMSegment) GetOutput] 
    EMSegment(vtkEMSegment) Update  
    # puts "[EMSegment(vtkEMSegment) Print]"
    #    puts "Error Report: "
    #    puts [EMSegment(vtkEMSegment) GetErrorMessages]

    if {[EMSegment(vtkEMSegment) GetErrorFlag] == 0} {
        set FileDir [file dirname $XMLFile] 
        set FilePrefix [file join $FileDir EMResult]
        WriteResult  SetFilePrefix $FilePrefix
        WriteResult  SetFilePattern %s.%03d
        # Write volume data
        puts "Writing $FilePrefix ..."
        WriteResult  Write
        puts " ...done."
        WriteResult Delete
        # Bug in VTK starts slicer with 000 instaed of 001 !!!
        IncrFileNumber 1 $FilePrefix
    }
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch6
# 6. Delete all the old instances 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch6 {} {
    DeleteVolumes 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentBatch7
# 7. Display window if necessary 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentBatch7 {} {
    if {$DisplayWindow && ([EMSegment(vtkEMSegment) GetErrorFlag] == 0)} {
        vtkImageViewer viewer
        viewer SetZSlice  0
        # Find out Maximum Label Number
        # Kilian Simplified version 
        set max -1
        foreach i $EMSegment(Cattrib,0,ClassList) {
            if {($EMSegment(Cattrib,$i,IsSuperClass) == 0) && ($EMSegment(Cattrib,$i,Label) > $max)} {
                set max $EMSegment(Cattrib,$i,Label)
            }
        }
        # How many values should be displayed
        set max_half [expr round($max / 2.0)]   
        viewer SetColorWindow [expr 2*$max_half]
        # Where are they centered around 
        viewer SetColorLevel $max_half
        viewer SetInput [EMSegment(vtkEMSegment) GetOutput] 
        source [file join $Slicer_Home Base/tests/WindowLevelInterface.tcl]
    } else {
        EMSegment(vtkEMSegment) Delete
    }
    puts " ============================ End EMSegmentBatch ======================="
}

