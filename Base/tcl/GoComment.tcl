
# Check if the user invoked this script incorrectly
if {$argc != 1} {
    puts "Unix Usage: tclsh83 GoComment.tcl <file to comment>"
    puts "Windows Usage: tclsh82.exe GoComment.tcl <file to comment>"
	exit
}

# Check if file exists, but be nice if it doesn't.
set filename [lindex $argv 0]
if {[file exists $filename] == 0} {
	puts "File '$filename' does not exist, stupid!"
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
source [file join $prog [file join tcl-main Comment.tcl]]

# Run
set ext [file extension $filename]
puts $filename
if {$ext == ".tcl"} { 
	# Go to town
	CommentFile $filename
} else {
	# Just add the copyright
	CopyrightFile $filename
}

exit
