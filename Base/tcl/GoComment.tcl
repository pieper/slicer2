
# Check if the user invoked this script incorrectly
if {$argc > 1} {
    puts "UNIX Usage: tclsh GoComment.tcl [optional file]"
    puts "Windows Usage: tclsh82.exe GoComment.tcl [optional file]"
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

# Run on one file if requested, otherwise on ALL files
set file [lindex $argv 0]
if {$file != ""} {
	puts $file
	set filename [file join $prog $file]
	ProcessFile $filename
} else {
	# Process all files
	set dirs "tcl-main tcl-modules tcl-shared ../vtksrc"
	if {[file exists [file join $prog \
		[file join tcl-modules Editor]]] == 1} {
		set dirs "$dirs tcl-modules/Editor"
	}
	foreach dir $dirs {
		foreach file "[glob -nocomplain $prog/$dir/*.tcl] \
			[glob -nocomplain $prog/$dir/*.h] \
			[glob -nocomplain $dir/*.cxx]" {
			puts $file
			set filename [file join $prog $file]
			ProcessFile $filename
		}
	}
}

exit
