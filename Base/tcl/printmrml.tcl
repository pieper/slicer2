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
# FILE:        printmrml.tcl
# DATE:        01/18/2000 12:11
# LAST EDITOR: gering
# PROCEDURES:  
#==========================================================================auto=

if {$argc != 1} {
    puts "Usage: printmrml <filename>"
	puts "where: <filename> = name of a MRML file (including the suffix)"
	puts "Example: printmrml /data/mr/case.mrml"
    exit
}

# Load vtktcl.dll on PCs
catch {load vtktcl}
wm withdraw .

# Find the Slicer's home from the root directory of this script ($argv0).
set Path(program) [file dirname $argv0]

# Source MainMrml.tcl
source [file join $Path(program) [file join tcl-main MainMrml.tcl]]

set tags [MainMrmlReadVersion2.0 $argv]

set level 0
foreach pair $tags {
	set tag  [lindex $pair 0]
	set attr [lreplace $pair 0 0]

	# Process EndTransform
	if {$tag == "EndTransform"} {
		set level [expr $level - 1]
	}
	set indent ""
	for {set i 0} {$i < $level} {incr i} {
		set indent "$indent  "
	}

	puts "${indent}$tag"

	# Process Transform
	if {$tag == "Transform"} {
		incr level
	}
	set indent ""
	for {set i 0} {$i < $level} {incr i} {
		set indent "$indent  "
	}

	foreach a $attr {
		set key   [lindex $a 0]
		set value [lreplace $a 0 0]
		puts "${indent}  $key=$value"
	}
}
