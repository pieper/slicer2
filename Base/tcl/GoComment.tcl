
# Check if the user invoked this script incorrectly
if {$argc != 0} {
    puts "Unix Usage: tclsh83 GoComment.tcl"
    puts "Windows Usage: tclsh82.exe GoComment.tcl"
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
source [file join $prog Comment.tcl]

# Run
set dirs "tcl-main tcl-modules tcl-shared ../vtksrc"
if {[file exists tcl-modules/Editor] == 1} {
	set dirs "$dirs tcl-modules/Editor"
}
foreach dir $dirs {
	foreach file "[glob -nocomplain $dir/*.tcl] \
		[glob -nocomplain $dir/*.h] [glob -nocomplain $dir/*.cxx]" {
		puts $file
		set ext [file extension $file]
		if {$ext == ".tcl"} { 
			# Go to town
			CommentFile $file
		} else {
			# Just add the copyright
			CopyrightFile $file
		}
	}
}

exit
