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
# FILE:        Comment.tcl
# PROCEDURES:  
#   PrintCopyright fid isTcl
#   ProcessFile file
#   CopyrightFile filename
#   CommentFile filename verbose
#   Polish data
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC PrintCopyright
#
# Prints the copyright notice on the top of the file.
# .ARGS
# str fid  File ID returned from opening the file
# int isTcl 1 if this is a TCL file, and 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc PrintCopyright {fid isTcl} {
	if {$isTcl == 1} {
		puts $fid \
"# (c) Copyright 2001 Massachusetts Institute of Technology
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
# THE SOFTWARE IS PROVIDED \"AS IS.\"  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#"
	} else {
		puts $fid "/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED \"AS IS.\"  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/"
	 }
}

#-------------------------------------------------------------------------------
# .PROC ProcessFile
#
# This procedure processes a file to call either CommentFile or CopyrightFile
# depending on the file's type.
# .ARGS
# str file  the path of the file to parse, relative to slicer/program
# .END
#-------------------------------------------------------------------------------
proc ProcessFile {file} {
	set ext [file extension $file]
	if {$ext == ".tcl"} { 
		# Go to town
		CommentFile $file
	} else {
		# Just add the copyright
		CopyrightFile $file
	}
}

#-------------------------------------------------------------------------------
# .PROC CopyrightFile
#
# Adds the copyright to any file by stripping off any existing automatically
# generated comments, and adding new ones.
# .ARGS
# str filename the name of the file to edit
# .END
#-------------------------------------------------------------------------------
proc CopyrightFile {filename} {
	global Comments

	# Read file into "data"
	if {[catch {set fid [open $filename r]} errmsg] == 1} {
		puts "CommentFile: $errmsg"
		exit
	}
	set data [read $fid]
	close $fid

	# Strip auto commment block off
	regsub {/\*=auto===.*===auto=\*/} $data REPLACE-AND-CONQUER data
	regsub "REPLACE-AND-CONQUER\n" $data {} data

	# Add comments to the file
	if {[catch {set fid [open $filename w]} errmsg] == 1} {
		"CommentFile: $errmsg"
		exit
	}
	PrintCopyright $fid 0
	puts -nonewline $fid $data
	close $fid
}

#-------------------------------------------------------------------------------
# .PROC CommentFile
#
# This procedure does everything CopyrightFile does except it also adds
# skeleton procedural comments. The file must be TCL source code.
# .ARGS
# str filename the full pathname of the TCL file to comment
# int verbose prints debug info if 1
# .END
#-------------------------------------------------------------------------------
proc CommentFile {filename {verbose 0}} {
	global Comments

	# Read file into "data"
	if {[catch {set fid [open $filename r]} errmsg] == 1} {
		puts "CommentFile: $errmsg"
		exit
	}
	set data [read $fid]
	close $fid

	# Strip auto commment block off
	regsub  "#=auto===.*===auto=\n" $data {} data

	# Polish it
	set data [Polish $data]
	
	# Comment it
	Comment $data

	# Add comments to the file
	if {[catch {set fid [open $filename w]} errmsg] == 1} {
		"CommentFile: $errmsg"
		exit
	}
	puts $fid "#=auto=========================================================================="
	PrintCopyright $fid 1
	puts $fid "#==============================================================================="
	puts $fid "# FILE:        [file tail $filename]"
	puts $fid "# PROCEDURES:  "
	foreach i $Comments(idList) {
		puts -nonewline $fid "#   $Comments($i,proc)"
		foreach a $Comments($i,argList) {
			puts -nonewline $fid " $Comments($i,$a,name)"
		}
		puts $fid ""
	}
	puts $fid "#==========================================================================auto="
	puts -nonewline $fid $data
	close $fid

	# Print comments to stdout
	if {$verbose == 0} {
		return
	}
	foreach i $Comments(idList) {
		puts "PROC=$Comments($i,proc)"
		if {$Comments($i,desc) != ""} {
			puts "DESC=$Comments($i,desc)"
		}
		if {$Comments($i,argList) != ""} {
			puts "ARGS="
			foreach a $Comments($i,argList) {
				foreach p "name type desc" {
					puts "  $p: $Comments($i,$a,$p)"
				}
				puts ""
			}
		}
		puts ""
	}
}

#-------------------------------------------------------------------------------
# .PROC Polish
#
# For each procedure in a file, this routine polishes the procedural comments
# if the existing comments fall into one of 3 pathological cases.
# The polished output looks like the following (except that the keywords that
# begin with a . are listed in small letters in this documentation so that
# the comment program can run on itself!
# <code><pre>
# #------------------------------
# # .proc MyProc
# #
# # .args
# # .end
# #------------------------------
# proc MyProc {} {
# }
# </code></pre>
# <p>
# The 3 pathological cases are:
# <p>
# <code><pre>
# proc MyProc {} {
# }
# 
# #-----------------------------
# # MyProc
# #-----------------------------
# proc MyProc {} {
# }
#
# #-----------------------------
# # .proc MyProc
# # .end
# #-----------------------------
# proc MyProc {} {
# }
#
# </code></pre>
#
# .ARGS
# str data the text for the entire file
# .END
#-------------------------------------------------------------------------------
proc Polish {data} {

	# .IGNORE

	set line "#-------------------------------------------------------------------------------"
	
	# Fix procs with no comments
	while {[regexp "\n\nproc (\[^ \]*) " $data match name] == 1} {
		regsub "proc $name " $data \
	  	  "$line\n# .PROC ${name}\n# \n# .ARGS\n# .END\n$line\nproc $name " data
	}

	# Fix procs with just a procedure name
	while {[regexp "$line\n# (\[^ \n\]*) *\n$line\nproc (\[^ \]*) " \
		$data match cname name] == 1 && $cname == $name} {
		regsub "$line\n# ${name} *\n$line\nproc $name " $data \
		  "$line\n# .PROC ${name}\n# \n# .ARGS\n# .END\n$line\nproc $name " data
	}

	# Fix procs with just the .PROC and .END
	while {[regexp "$line\n# .PROC (\[^ \]*) *\n# .END\n$line\nproc (\[^ \]*) " \
		$data match cname name] == 1 && $cname == $name} {
		regsub "$line\n# .PROC ${name} *\n# .END\n$line\nproc $name " $data \
		  "$line\n# .PROC ${name}\n# \n# .ARGS\n# .END\n$line\nproc $name " data
	}

	# .ENDIGNORE
	return $data
}

#-------------------------------------------------------------------------------
# .PROC Comment
#
# This procedure forms a global array 'Comments' containing the contents of
# the comments at the procedure level in the file
# .ARGS
# str data    The file contents to parse.
# .END
#-------------------------------------------------------------------------------
proc Comment {data} {
	global Comments

	# .IGNORE
	# Ignore this routine so that the comment program can run on itself
	regsub -all {\.IGNORE.*\.ENDIGNORE} $data {} data

	# Delete all comment markers
	#
	regsub -all "^#" $data {} data
	regsub -all "\n#" $data "\n" data
	regsub -all "^//" $data {} data
	regsub -all "\n//" $data "\n" data
	regsub -all "^/\*" $data {} data
	regsub -all {/[\*]} $data {} data

	# Replace all instances of ".PROC" with "|" and then split the
	# data into a list of the text that was delimited by "|"
	#
	regsub -all {\|} $data "##pipe##" data
	regsub -all {\.PROC} $data "|" data
	set procList [lrange [split $data |] 1 end]

	# Build an array of comment parts:
	# Comments(idList)
	# Comments(id,proc)
	# Comments(id,desc)
	# Comments(id,argList)
	# Comments(id,arg,type)
	# Comments(id,arg,name)
	# Comments(id,arg,desc)
	#
	set Comments(idList) ""
	set id 1
	foreach p $procList {

		# Delete everything after, and including, ".END"
		regsub {\.END.*} $p {} p 

		# Strip off everything before ".ARGS" as the description
		regsub -all {\.ARGS} $p "|" p 
		set list [split $p |]
		set desc [lindex $list 0]
		set args [lindex $list 1] 

		# Strip leading white space off
		set desc [string trimleft $desc]

		# Strip off the first word of the desc as the proc name
		regexp "(.*?)\[\n\].*" $desc match proc

		# Note the following line treats the string as a list, which	    
		# is too dangerous since a quote-comma sequence crashes it
		#set proc [lindex $desc 0]

		regsub "$proc" $desc {} desc

		# Note the following line would have stripped off the
		# line returns:
		# set desc [lrange $desc 1 end]

		# Strip leading white space of the description
		regsub "^\[\n\t \]*" $desc {} desc

		# Delineate args by newlines 
		regsub -all "\n" $args "|" args 
		set argList [lrange [split $args |] 1 end]

		# Restore "##pipe##" to "|"
		regsub -all {##pipe##} $desc {|} desc 
		regsub -all {##pipe##} $args {|} args

		# Set array values
		lappend Comments(idList) $id 
		set Comments($id,proc) $proc
		set Comments($id,desc) $desc

		# Break arguments into components: type, name, desc
		set arg 1
		set Comments($id,argList) ""
		foreach a $argList {
			if {[llength $a] > 1} { 
				lappend Comments($id,argList) $arg
				set Comments($id,$arg,type) [lindex $a 0]
				set Comments($id,$arg,name) [lindex $a 1]
				set Comments($id,$arg,desc) [lrange $a 2 end]
				incr arg
			}
		}
		incr id
	}
	# .ENDIGNORE
}
