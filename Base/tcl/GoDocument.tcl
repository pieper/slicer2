
# Check if the user invoked this script incorrectly
if {$argc != 0} {
    puts "UNIX Usage: tclsh GoDocument.tcl"
    puts "Windows Usage: tclsh82.exe GoDocument.tcl"
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
source [file join [file join $prog tcl-main] Comment.tcl]
source [file join [file join $prog tcl-main] Document.tcl]

#Run
DocumentAll $prog
exit
