#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions on this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        /usr/local/bin/comment
# DATE:        11/20/1999 15:03
# LAST EDITOR: gering
# PROCEDURES:  
#   PrintCopyright
#   CopyrightFile
#   CommentFile
#   Polish
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC PrintCopyright
# .END
#-------------------------------------------------------------------------------
proc PrintCopyright {fid isTcl} {
	if {$isTcl == 1} {
		puts $fid "# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
	} else {
		puts $fid "/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/"
	 }
}

#-------------------------------------------------------------------------------
# .PROC CopyrightFile
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
# .END
#-------------------------------------------------------------------------------
proc Polish {data} {

	set line "#-------------------------------------------------------------------------------"
	
	# Fix procs with no comments
	while {[regexp "\n\nproc (\[^ \]*) " $data match name] == 1} {
		regsub "proc $name " $data \
	  	  "$line\n# .PROC ${name}\n# .END\n$line\nproc $name " data
	}

	# Fix procs with just a procedure name
	while {[regexp "$line\n# (\[^ \n\]*) *\n$line\nproc (\[^ \]*) " \
		$data match cname name] == 1 && $cname == $name} {
		regsub "$line\n# ${name} *\n$line\nproc $name " $data \
		  "$line\n# .PROC ${name}\n# .END\n$line\nproc $name " data
	}

	return $data
}

#-------------------------------------------------------------------------------
# .PROC Commment
# This procedure forms a global array 'Comments' containing the contents of
# the comments at the procedure level in the file
# .ARGS
# str data    The file contents to parse.
# .END
#-------------------------------------------------------------------------------
proc Comment {data} {
	global Comments

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

		# Strip off the first word of the desc as the proc name
		set proc [lindex $desc 0]
		set desc [lrange $desc 1 end]

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

}

