#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        MainLuts.tcl
# DATE:        02/16/2000 09:11
# LAST EDITOR: gering
# PROCEDURES:  
#   MainLutsInit
#   MainLutsBuildVTK
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainLutsInit
# .END
#-------------------------------------------------------------------------------
proc MainLutsInit {} {
	global Module Lut

	# Define Procedures
	lappend Module(procVTK) MainLutsBuildVTK

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainLuts \
		{$Revision: 1.6 $} {$Date: 2000/02/22 03:50:40 $}]

	# Create an ID for Labels
	set Lut(idLabel) -1

	set Lut(idList) " 0 1 2 $Lut(idLabel)"

	set Lut(0,name) Gray
	set Lut(0,fileName) ""
	set Lut(0,numberOfColors) 256
	set Lut(0,hueRange) "0 0"
	set Lut(0,saturationRange) "0 0"
	set Lut(0,valueRange) "0 1"
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
}

#-------------------------------------------------------------------------------
# .PROC MainLutsBuildVTK
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
			close $fid

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
