#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        Parse.tcl
# PROCEDURES:  
#   MainMrmlReadVersion2.0 MainMrmlReadVersion2.x
#   MainMrmlReadVersion2.0
#   MainMrmlReadVersion2.x filename verbose
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion2.0
# 
# Left for compatibility reasons; the only thing this procedure does is calling
# MainMrmlReadVersion2.x
#
# .ARGS
# See MainMrmlReadVersion2.x
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion2.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadVersion2.0 {fileName {verbose 1}} {
	
	return [MainMrmlReadVersion2.x $fileName $verbose]
}


#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion2.x
# 
# Opens a MRML2.x file and reads and parses it to return a string that is
# a list of MRML nodes in the file. Each string element is itself a list of
# (key value) pairs of the attributes of the node. So the format is:<br>
# <code>(nodeName (key1 value1) (key2 value2)) (nodeName (key1 value1))</code>
#
# .ARGS
# str filename Full pathname of the MRML2.x file to read and parse.
# str verbose set to 1 if you want to print debug information during parsing.
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadVersion2.x {fileName {verbose 1}} {

	# Returns list of tags on success else 0

	# Read file
	if {[catch {set fid [open $fileName r]} errmsg] == 1} {
	    puts $errmsg
	    tk_messagebox -message $errmsg
	    return 0
	}
	set mrml [read $fid]
        if {[catch {close $fid} errorMessage]} {
           tk_messageBox -type ok -message "The following error occurred saving a file: ${errorMessage}" 
           puts "Aborting due to : ${errorMessage}"
           exit 1
        }

	# Check that it's the right file type and version
	# accepts all versions from MRML 2.0 to 2.5
	if {[regexp {<!DOCTYPE MRML SYSTEM ['"]mrml2[0-5].dtd['"]>} $mrml match] == 0} {
		set errmsg "The file \"$fileName\" is NOT MRML version 2.x"
		tk_messageBox -message $errmsg
		return 0
	}

	# Strip off everything but the body
	if {[regexp {<MRML>(.*)</MRML>} $mrml match mrml] == 0} {
		# There's no content in the file
		return ""
	}

	# Strip leading white space
	regsub "^\[\n\t \]*" $mrml "" mrml

	set tags1 ""
	while {$mrml != ""} {

		# Find next tag
		if {[regexp {^<([^ >]*)([^>]*)>([^<]*)} $mrml match tag attr stuffing] == 0} {
		#([^/>]*)
				set errmsg "Invalid MRML file. Can't parse tags:\n$mrml"
			puts "$errmsg"
			tk_messageBox -message "$errmsg"
			return 0
		}

		# Strip off this tag, so we can continue.
		if {[lsearch "Transform /Transform" $tag] != -1 || \
		    [lsearch "Fiducials /Fiducials" $tag] != -1 || \
		    [lsearch "Hierarchy /Hierarchy" $tag] != -1 || \
		    [lsearch "ModelGroup /ModelGroup" $tag] != -1 || \
		    [lsearch "Scenes /Scenes" $tag] != -1 || \
		    [lsearch "VolumeState /VolumeState" $tag] != -1 || \
		    [lsearch "Path /Path" $tag] != -1 } {
			# set str "<$tag>" doesn't work with tags which have attributes
			set str ">"
		} else {
			set str "</$tag>"
			set str2 " />"
		}
		set str2_used 0
		set i [string first $str $mrml]
		if {($i<=0) && ([info exists str2])} {
			set i [string first $str2 $mrml]
			set str2_used 1
		}
		if {!$str2_used} {
			set mrml [string range $mrml [expr $i + [string length $str]] end]
		} else {
			set mrml [string range $mrml [expr $i + [string length $str2]] end]
		}

		# Give the EndTransform tag a name
		if {$tag == "/Transform"} {
			set tag EndTransform
		}

		# Give the EndFiducials tag a name
		if {$tag == "/Fiducials"} {
			set tag EndFiducials
		}

		# Give the EndPath tag a name
		if {$tag == "/Path"} {
			set tag EndPath
		}

		# Give the EndHierarchy tag a name
		if {$tag == "/Hierarchy"} {
			set tag EndHierarchy
		}
		
		# Give the EndModelGroup tag a name
		if {$tag == "/ModelGroup"} {
			set tag EndModelGroup
		}
		
		# Give the EndScenes tag a name
		if {$tag == "/Scenes"} {
			set tag EndScenes
		}

		# Give the EndVolumeState tag a name
		if {$tag == "/VolumeState"} {
			set tag EndVolumeState
		}
		
		# Append to List of tags1
		lappend tags1 "$tag {$attr} {$stuffing}"

		# Strip leading white space
		regsub "^\[\n\t \]*" $mrml "" mrml
	}

	# Parse the attribute list for each tag
	set tags2 ""
	foreach pair $tags1 {
		set tag [lindex $pair 0]
		set attr [lindex $pair 1]
		set stuffing [lindex $pair 2]
		
		# Add the options (the "stuffing" from inside the start and end tags)
		set attrList ""
		lappend attrList "options $stuffing"

		# Strip leading white space
		regsub "^\[\n\t \]*" "$attr $stuffing" "" attr

		while {$attr != ""} {
		
			# Find the next key=value pair (and also strip it off... all in one step!)
			if {([regexp "^(\[^=\]*)\[\n\t \]*=\[\n\t \]*\['\"\](\[^'\"\]*)\['\"\](.*)$" \
				$attr match key value attr] == 0) && ([string equal -length 1 $attr "/"] == 0)} {
				set errmsg "Invalid MRML file. Can't parse attributes:\n$attr"
				puts "$errmsg"
				tk_messageBox -message "$errmsg"
				return 0
			}
			if {[string equal -length 1 $attr "/"] == 0} {
				lappend attrList "$key $value"
			} else {
				# Strip the / at then end of an XML tag
				regsub "/" $attr "" attr
			}

			# Strip leading white space
			regsub "^\[\n\t \]*" $attr "" attr
		}

		# Add this tag
		lappend tags2 "$tag $attrList"
	}
	return $tags2
}


