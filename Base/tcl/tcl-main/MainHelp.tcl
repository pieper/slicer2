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
# FILE:        MainHelp.tcl
# DATE:        01/18/2000 12:16
# LAST EDITOR: gering
# PROCEDURES:  
#   MainHelpInit
#   MainHelpBuildGUI
#   HelpWidget
#   MainHelpApplyTags
#   MainHelpShow
#   MainHelpLink
#   MainHelpLaunchBrowser
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainHelpInit
# .END
#-------------------------------------------------------------------------------
proc MainHelpInit {} {
	global Help Module

	set Help(tagNormal)   "-font {times 10}"
	set Help(tagItalic)   "-font {times 10 italic}"
	set Help(tagBold)     "-font {times 10 bold}"
	set Help(tagLink)     "-font {times 10} -underline true -foreground red"
	set Help(tagHeading3) "-font {helvetica 12 bold}"
	set Help(tagHeading4) "-font {helvetica 10 bold}"
	set Help(tagHeading5) "-font {times 12 bold italic}"

	# Initialize all tabs to have empty tagLists (hypertext)
	foreach m $Module(idList) {	
		
		if {![info exists Help($m,tagList)]} {
			set Help($m,tagList) ""
		}

		# The "Intro" module has a Help frame on all of its tabs
		if {$m == "Intro"} {
			foreach tab "$Module($m,row1List) $Module($m,row2List)" {
				if {![info exists Help($tab,tagList)]} {
					set Help($tab,tagList) ""
				}
			}
		}
	}
}

# All the Modules have the same Help Button Style.
# Call this procedure from {ModuleName}BuildGUI in the [ModuleName].tcl file
#
# Usage: MainHelpBuildGUI ModuleName
#
#-------------------------------------------------------------------------------
# .PROC MainHelpBuildGUI
#
# All the Tabs have the same Info Button Style.
# Call this procedure from the TabNameBuildGUI in the TabName.tcl file
#
# Usage: MainHelpBuildGUI TabName
# .END
#-------------------------------------------------------------------------------
proc MainHelpBuildGUI {ModuleName} {
	global Module Gui Help
	upvar $ModuleName ModuleArray

	set fHelp $Module($ModuleName,fHelp)
	set f $fHelp

	frame $f.fWidget -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.fBtns   -side top -pady $Gui(pad)
	pack $f.fWidget -side top -padx 2 -fill both -expand true

	set ModuleArray(helpWidget) [HelpWidget $f.fWidget]
	MainHelpShow $ModuleArray(helpWidget) $ModuleName

	set f $f.fBtns
	set c {button $f.bHome -text "Home" -width 5 \
		-command "$ModuleArray(helpWidget) see 0.0" $Gui(WBA)}; eval [subst $c]
	pack $f.bHome -side left -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC HelpWidget
# .END
#-------------------------------------------------------------------------------
proc HelpWidget {f} {
	global Gui

	set t [text $f.t -height 6 -setgrid true -wrap word \
		-yscrollcommand "$f.sy set" -cursor arrow -insertontime 0]
	scrollbar $f.sy -orient vert -command "$f.t yview"

	pack $f.sy -side right -fill y
	pack $f.t -side left -fill both -expand true
	return $t
}

#-------------------------------------------------------------------------------
# .PROC MainHelpApplyTags
# .END
#-------------------------------------------------------------------------------
proc MainHelpApplyTags {tab str} {
	global Help

	set Help($tab,hypertext) $str

	# Replace some tags with text or nothing
	#--------------------------------------------------
	foreach tag "<P> <LI> </LI> <BR> <UL> </UL> <HR> &nbsp; &gt; &lt; &amp;" \
		sub {"\n\n" "\n<G>doc/bullet.gif</G>" "" "\n" "" "" "" " " ">" "<" "&" \
		} {
		set i [string first $tag $str]
		while {$i != -1} {
			set str "[string range $str 0 [expr $i-1]]$sub\
			[string range $str [expr $i+[string length $tag]] end]"
			set i [string first $tag $str]
		}
	}

	# Put sub before tag
	#--------------------------------------------------
	foreach tag "<H3> <H4> <H5>" sub {"\n\n" "\n\n"} {
		set i [string first $tag $str]
		set rest $str
		set str ""
		while {$i != -1} {
			set str "${str}[string range $rest 0 [expr $i-1]]$sub$tag"
			set rest [string range $rest [expr $i+[string length $tag]] end]
			set i [string first $tag $rest]
		}
		set str "$str$rest"
	}
	
	# Find tags
	#--------------------------------------------------
	set tag 0
	set tagList ""
	set type normal
	set text $str
	set tokens "B I H3 H4 H5 A G"
	set names "bold italic heading3 heading4 heading5 link image"

	set a [string length $str]
	set type -1
	foreach token $tokens name $names {
		set d [string first <$token> $str]
		if {$d != "-1" && $d < $a} {
			set a $d
			set type $name
			set symbol $token
		}
	}

	while {$type != -1} {

		set text [string range $str 0 [expr $a-1]]
		if {[string length $text] > 0} {
			set Help($tab,$tag,type) normal
			set Help($tab,$tag,text) $text
			lappend tagList $tag
			incr tag
		}

		set rest [string range $str [expr $a+2+[string length $symbol]] end]
		set b [string first </$symbol> $rest]
		set text [string range $rest 0 [expr $b-1]]

		set Help($tab,$tag,type) $type
		set Help($tab,$tag,text) $text
		lappend tagList $tag
		incr tag
		set str [string range $rest [expr $b+3+[string length $symbol]] end]

		set a [string length $str]
		set type -1
		foreach token $tokens name $names {
			set d [string first <$token> $str]
			if {$d != "-1" && $d < $a} {
				set a $d
				set type $name
				set symbol $token
			}
		}
	}

	set text $str
	if {[string length $text] > 0} {
		set Help($tab,$tag,type) normal
		set Help($tab,$tag,text) $text
		lappend tagList $tag
		incr tag
	}

	set Help($tab,tagList) $tagList

}

#-------------------------------------------------------------------------------
# .PROC MainHelpShow
# .END
#-------------------------------------------------------------------------------
proc MainHelpShow {w id} {
    global Help Gui

	eval $w tag configure normal   $Help(tagNormal)
	eval $w tag configure italic   $Help(tagItalic)
	eval $w tag configure bold     $Help(tagBold)
	eval $w tag configure heading3 $Help(tagHeading3)
	eval $w tag configure heading4 $Help(tagHeading4)
	eval $w tag configure heading5 $Help(tagHeading5)

	foreach tag $Help($id,tagList) {
		set type $Help($id,$tag,type)
		set text $Help($id,$tag,text)

		if {$type == "heading3"} {
			set Help($id,$tag,index) [$w index insert]
		}
		
		if {$type == "link"} {
			set type link$tag
			eval $w tag configure $type $Help(tagLink)
		}

		if {$type == "image"} {
			set img [image create photo -file [ExpandPath "$text"]]
			$w image create insert -image $img
		} else {
			$w insert insert "$text" $type
		}
	}

	foreach tag $Help($id,tagList) {
		set type $Help($id,$tag,type)
		if {$type == "link"} {
			$w tag bind link$tag <ButtonPress> "MainHelpLink $w $id $tag"
			$w tag bind link$tag <Enter> "$w config -cursor hand2"
			$w tag bind link$tag <Leave> "$w config -cursor arrow"
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainHelpLink
# .END
#-------------------------------------------------------------------------------
proc MainHelpLink {w id linkTag} {
    global Help Gui

	set linkText $Help($id,$linkTag,text)

	foreach tag $Help($id,tagList) {
		set type $Help($id,$tag,type)
		set text $Help($id,$tag,text)

		if {$type == "heading3"} {
			if {$text == $linkText} {
				$w see $Help($id,$tag,index)
			}
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainHelpLaunchBrowser
#
#
#  Tries to launch a Web Browser
# .END
#-------------------------------------------------------------------------------
proc MainHelpLaunchBrowser {{section ""}} {
	global Path

	if {$section == ""} {
		catch {exec "$Path(browserPath)" $Path(browserUrl) &}
	} else {
		catch {exec "$Path(browserPath)" $Path(browserUrl)#$section &}
	}
}

