
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
