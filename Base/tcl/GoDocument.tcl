
# Check if the user invoked this script incorrectly
if {$argc > 1} {
    puts "UNIX Usage: tclsh GoDocument.tcl [doc | tcl]"
    puts "Windows Usage: tclsh82.exe GoDocument.tcl [doc | tcl]"
	exit
}

# Determine Slicer's home directory from the SLICER_HOME environment 
# variable, or the root directory of this script ($argv0).
if {[info exists env(SLICER_HOME)] == 0 || $env(SLICER_HOME) == ""} {
	set prog [file dirname $argv0]
} else {
	set prog [file join $env(SLICER_HOME) program]
}

if {[info exists env(SLICER_DOC)] == 0 || $env(SLICER_DOC) == ""} {
	set outputdir [file dirname $prog]
} else {
	set outputdir $env(SLICER_DOC)
}

puts "prog $prog output $outputdir"

# Read source files
source [file join [file join $prog tcl-main] Comment.tcl]
source [file join [file join $prog tcl-main] Document.tcl]

# Run
if {$argv == ""} {
	set argv "doc tcl"
}
DocumentAll $prog $outputdir $argv
exit
