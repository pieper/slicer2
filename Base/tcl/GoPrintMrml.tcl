
# Check if the user invoked this script incorrectly
if {$argc != 1} {
    puts "UNIX Usage: tclsh83 GoPrintMrml.tcl <filename>"
    puts "Windows Usage: tclsh82.exe GoPrintMrml.tcl <filename>"
	puts "where: <filename> = name of a MRML file (including the suffix)"
	puts "Example: tclsh83 GoPrintMrml.tcl /data/mr/case.xml"
    exit
}

# Determine Slicer's home directory from the SLICER_HOME environment 
# variable, or the root directory of this script ($argv0).
if {[info exists env(SLICER_HOME)] == 0 || $env(SLICER_HOME) == ""} {
	set prog [file dirname $argv0]
} else {
	set prog [file join $env(SLICER_HOME) program]
}

# Read source files
source [file join $prog [file join tcl-main MainMrml.tcl]]

# Run
set tags [MainMrmlReadVersion2.0 $argv]
MainMrmlPrint $tags
exit
