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
# FILE:        mrmlize.tcl
# DATE:        01/18/2000 12:11
# LAST EDITOR: gering
# PROCEDURES:  
#==========================================================================auto=

#set argv "c:/data/dave/axi/I.001 32"
#set argc 2

if {$argc != 2} {
    puts "Usage: mrmlize <img1> <num2>"
	puts "where: <img1> = full pathname of the first image in the volume."
	puts "       <num2> = just the number of the last image"
	puts "Example: mrmlize /data/mr/I.001 124"
	puts "Output: output.mrml is written in the current directory."
    exit
}

# Load vtktcl.dll on PCs
catch {load vtktcl}
wm withdraw .

# Find the Slicer's home from the root directory of this script ($argv0).
set Path(program) [file dirname $argv0]
set Path(root) [file dirname [lindex $argv 0]]

# Source Header.tcl
source [file join $Path(program) [file join tcl-main MainHeader.tcl]]

# Find print_header
set Path(printHeader) [file join $Path(program) [file join bin print_header]]

# Get info from headers
set img1 [lindex $argv 0]
set num2 [lindex $argv 1]
vtkMrmlVolumeNode node
GetHeaderInfo $img1 $num2 node 0
node SetName [file tail [file root $img1]]

# Write MRML
vtkMrmlTree tree
tree AddItem node
tree Write "output.xml"
exit