#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
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
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        Document.tcl
# PROCEDURES:  
#   ReadInput
#   HtmlHead
#   HtmlFoot
#   HtmlTableStart
#   HtmlTableEnd
#   DocumentProc
#   DocumentFile
#   DocumentIndex
#   DocumentAll
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ReadInput
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ReadInput {filename} {
	if {[catch {set fid [open $filename r]} errmsg] == 1} {
		puts "$errmsg"
		exit
	}
	set data [read $fid]
	close $fid

	return $data
}

#-------------------------------------------------------------------------------
# .PROC HtmlHead
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc HtmlHead {fid title {stylePath ../../../style.css"}} {
	puts $fid \
"<html>
<head>
<title>$title</title>
<link rel=stylesheet type='text/css' href='$stylePath'>
</head>
<body bgcolor=white>"
}

#-------------------------------------------------------------------------------
# .PROC HtmlFoot
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc HtmlFoot {fid} {
	puts $fid \
"</body>
</html>"
}

#-------------------------------------------------------------------------------
# .PROC HtmlTableStart
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc HtmlTableStart {fid options} {
	puts $fid \
"<p>
<table cellpadding=10 cellspacing=0 border=0 $options>
<tr>
<td>
	<table cellpadding=3 cellspacing=3 border=0>"
}

#-------------------------------------------------------------------------------
# .PROC HtmlTableEnd
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc HtmlTableEnd {fid} {
	puts $fid \
"	</table>
</td>
</tr>
</table>"
}

#-------------------------------------------------------------------------------
# .PROC DocumentProc
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DocumentProc {fid p} {
	global Comments
	
	set name $Comments($p,proc)
	set desc $Comments($p,desc)
	set args $Comments($p,argList)

	puts $fid \
"<a name='$name'></a>
<h2>$name</h2>
<p>
$desc
"

	if {$args != ""} {
		HtmlTableStart $fid "align=center"
		puts $fid \
"	<tr>
	<th class=box>Parameter</th>
	<th class=box>Type</th>
	<th class=box>Description</th>
	</tr>"

		set n 1
		foreach a $args {
			set n [expr [incr n] % 2]
			set type $Comments($p,$a,type)
			set name $Comments($p,$a,name)
			set desc $Comments($p,$a,desc)
			puts $fid \
"	<tr>
	<td class=box$n>$name</td>
	<td class=box$n>$type</td>
	<td class=box$n>$desc</td>
	</td>
	</tr>"
		}
		HtmlTableEnd $fid
	}
}

#-------------------------------------------------------------------------------
# .PROC DocumentFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DocumentFile {docdir dir filename} {
	global Comments
	
	Comment [ReadInput $filename]
	set name [file root [file tail $filename]]

	# Open output file
	set docfile [file join [file join $docdir $dir] $name.html]
	if {[catch {set fid [open $docfile w]} errmsg] == 1} {
		puts "$errmsg"
		exit
	}
	HtmlHead $fid $name

	# List procecures
	puts $fid \
"<h2><font color='#993333'>[file tail $filename]</font> Procedures:</h2>
<ul>"
	foreach p $Comments(idList) {
		set name $Comments($p,proc)
		puts $fid \
"<li><a href='#$name'>$name</a>"
	}
	puts $fid \
"</ul>"

	# Document each procedure
	foreach p $Comments(idList) {
		DocumentProc $fid $p
	}

	HtmlFoot $fid
	close $fid
}

#-------------------------------------------------------------------------------
# .PROC DocumentIndex
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DocumentIndex {docdir} {
	global Index
	
	# Open output file
	set docfile [file join $docdir index.html]
	if {[catch {set fid [open $docfile w]} errmsg] == 1} {
		puts "$errmsg"
		exit
	}

	# Head
	HtmlHead $fid "TCL Source Index" "../../style.css"

	# List files
	foreach dir [lsort $Index(dirList)] {
		puts $fid \
"<h2>$dir</h2>
<ul>"

		foreach name [lsort $Index($dir)] {
			puts $fid \
"<li><a href='[file join $dir $name.html]'>$name</a>"
		}
		puts $fid \
"</ul>"
	}

	HtmlFoot $fid
	close $fid
}

#-------------------------------------------------------------------------------
# .PROC DocumentAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DocumentAll {prog} {
	global Index

	# Write output into the slicer/doc/developers/tcl directory
	set docdir [file join [file dirname $prog] doc]
	set docdir [file join [file join $docdir developers] tcl]

	# Document each file
	set Index(dirList) ""
	foreach dir "tcl-main tcl-modules tcl-shared" {
		set Index($dir) ""
		lappend Index(dirList) $dir
		foreach file "[glob -nocomplain $prog/$dir/*.tcl]" {
			puts $file
			lappend Index($dir) [file root [file tail $file]]
			set filename [file join $prog $file]
			DocumentFile $docdir $dir $filename
		}
	}

	# Build an index
	DocumentIndex $docdir
}
