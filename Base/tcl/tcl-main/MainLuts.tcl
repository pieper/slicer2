#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for internal 
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
# FILE:        MainLuts.tcl
# PROCEDURES:  
#   MainLutsInit
#   MainLutsBuildVTK
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainLutsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainLutsInit {} {
    global Module Lut

    # Define Procedures
    lappend Module(procVTK) MainLutsBuildVTK

    # Set version info
    lappend Module(versions) [ParseCVSInfo MainLuts \
    {$Revision: 1.18 $} {$Date: 2003/03/13 22:30:56 $}]

    # Create an ID for Labels
    set Lut(idLabel) -1

    set Lut(idList) " 0 1 2 3 4 5 $Lut(idLabel)"

    set Lut(0,name) Gray
    set Lut(0,fileName) ""
    set Lut(0,numberOfColors) 256
    set Lut(0,hueRange) "0 0"
    set Lut(0,saturationRange) "0 0"
    set Lut(0,valueRange) "0 1"
    # set Lut(0,annoColor) = Set color for Histogram 
    set Lut(0,annoColor) "1 0 0"

    set Lut(1,name) Iron
    set Lut(1,fileName) ""
    set Lut(1,numberOfColors) 156
    set Lut(1,hueRange) "0 .15"
    set Lut(1,saturationRange) "1 1"
    set Lut(1,valueRange) "1 1"
    set Lut(1,annoColor) "1 1 1"

    set Lut(2,name) Rainbow
    set Lut(2,fileName) ""
    set Lut(2,numberOfColors) 256
    set Lut(2,hueRange) "0 .8"
    set Lut(2,saturationRange) "1 1"
    set Lut(2,valueRange) "1 1"
    set Lut(2,annoColor) "1 1 1"

    set Lut(3,name) Ocean
    set Lut(3,fileName) ""
    set Lut(3,numberOfColors) 256
    set Lut(3,hueRange) "0.666667 0.5"
    set Lut(3,saturationRange) "1 1"
    set Lut(3,valueRange) "1 1"
    set Lut(3,annoColor) "0 0 1"
    
    set Lut(4,name) Desert
    set Lut(4,fileName) ""
    set Lut(4,numberOfColors) 256
    set Lut(4,hueRange) "0 0.1"
    set Lut(4,saturationRange) "1 1"
    set Lut(4,valueRange) "1 1"
    set Lut(4,annoColor) "0 0 1"

    set Lut(5,name) InvGray
    set Lut(5,fileName) ""
    set Lut(5,numberOfColors) 256
    set Lut(5,hueRange) "0 0"
    set Lut(5,saturationRange) "0 0"
    set Lut(5,valueRange) "1 0"
    set Lut(5,annoColor) "0 1 1"

}

#-------------------------------------------------------------------------------
# .PROC MainLutsBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainLutsBuildVTK {} {
    global Volume Lut Dag

    foreach l $Lut(idList) {
        if {$l >= 0} {
            # Hue, Saturation, Intensity
            if {$Lut($l,fileName) == ""} {
            
                vtkLookupTable Lut($l,lut)
                foreach param "NumberOfColors HueRange SaturationRange ValueRange" {
                    eval Lut($l,lut) Set${param} $Lut($l,[Uncap ${param}])
                }
                # sp - 2002-11-11 changed default SCurve to Linear to improve
                # fidelity of image display
                Lut($l,lut) SetRampToLinear
                Lut($l,lut) Build
            
            # File
            } else {
                vtkLookupTable Lut($l,lut)

                # Open palette file
                set filename $Lut($l,fileName)
                if {[CheckFileExists $filename] == 0} {
                    puts "Cannot open file '$filename'"
                    return
                }
                set fid [open $filename r]

                # Read colors represented by 3 numbers (RGB) on a line
                set numColors 0
                gets $fid line
                while {[eof $fid] == "0"} {
                    if {[llength $line] == 3} {
                        set colors($numColors) $line
                        incr numColors
                    }
                    gets $fid line
                }
                if {[catch {close $fid} errorMessage]} {
                    tk_messageBox -type ok -message "The following error occurred saving a file: ${errorMessage}" 
                    puts "Aborting due to : ${errorMessage}"
                    exit 1
                }

                # Set colors into the Lut
                set Lut($l,numberOfColors) $numColors
                Lut($l,lut) SetNumberOfTableValues $Lut($l,numberOfColors)
                Lut($l,lut) SetNumberOfColors $Lut($l,numberOfColors)
                for {set n 0} {$n < $numColors} {incr n} {
                    eval Lut($l,lut) SetTableValue $n $colors($n) 1
                }
            }
        }
    }

    # Add a lut for Labels
    #--------------------------------------

    # Give it a name
    set l $Lut(idLabel)
    set Lut($l,name) "Label"
    set Lut($l,annoColor) "1.0 1.0 0.5"

    # Make a LookupTable, vtkIndirectLookupTable
    vtkLookupTable Lut($l,lut)

    vtkIndirectLookupTable Lut($l,indirectLUT)
    Lut($l,indirectLUT) DirectOn
    Lut($l,indirectLUT) SetLowerThreshold 1
    Lut($l,indirectLUT) SetLookupTable Lut($l,lut)
}
