
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