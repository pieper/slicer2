#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use and copy (but not distribute) this
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
# FILE:        Gui.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   GuiInit
#   InfoWidget
#   MailWidget
#   MsgPopup
#   YesNoPopup
#   ShowPopup
#   MakeVTKImageWindow
#   ExposeTkImageViewer
#   MakeVTKObject
#   ScrollSet
#   MakeColor
#   MakeColorNormalized
#   GetRelativeFileName
#   ExpandPath
#   GetPrefix
#   GetSuffix
#   FindUniqueFileName
#   DecodeFileName
#   CheckVolumeExists
#   Uncap
#   Cap
#   WaitWindow
#   WaitPopup
#   WaitDestroy
#   IsInt
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC GuiInit
#
# This is my description.
# .END
#-------------------------------------------------------------------------------
proc GuiInit {} {
	global Gui InitProc tcl_platform

	set InitProc() ""

	if {$tcl_platform(platform) == "windows"} {
		set Gui(pc) 1
	} else {
		set Gui(pc) 0
	}
	if {$Gui(pc) == 0} {
		set Gui(smallFont) 0
		set Gui(xViewer) 254
	} else {
		set Gui(smallFont) 1
		set Gui(xViewer) 244
	}
	set Gui(progressText) "Working..."
	set Gui(waitWin) ""
	set Gui(title) "3D Slicer"
	
	# prefix
	set Gui(prefixSaveImage3D) img3D
	set Gui(prefixColors) Colors
	set Gui(prefixSaveCurrentDag) data
	set Gui(prefixSaveRegister) reg
	set Gui(prefixSaveDag) data
	set Gui(prefixOpenDag) data
	set Gui(prefixOpenVolume) I
	set Gui(prefixOpenModel) skin
	set Gui(prefixEditWorking) work
	set Gui(prefixEditComposite) comp
	set Gui(prefixSmgModel) skin
	set Gui(prefixEditSession) edit

	# INIT
	set Gui(waitSemaphore) 0
	set Gui(magDim) 175
	set Gui(borderWidth) 2
	set Gui(minShort) -32768
	set Gui(maxShort) 32767
	set Gui(pad) 5
	set Gui(unsavedDag) 0

	# COLORS

	set Gui(darkGray)    " 81  81  81"
	set Gui(mediumGray)  "132 132 132"
	set Gui(lightGray)   "181 181 181"
	set Gui(darkCream)   "226 205 186"
	set Gui(lightCream)  "239 221 211"
	set Gui(brightCream) "255 242 235"
	set Gui(black)       "  0   0   0"
	set Gui(white)       "255 255 255"
	set Gui(red)         "200 150 150"
	set Gui(green)       "150 200 150"
	set Gui(yellow)      "220 220 100"

	set Gui(backdrop)          [MakeColor $Gui(mediumGray)]
	set Gui(inactiveWorkspace) [MakeColor $Gui(lightGray)]
	set Gui(activeWorkspace)   [MakeColor $Gui(darkCream)]
	set Gui(normalButton)      [MakeColor $Gui(lightCream)]
	set Gui(activeButton)      [MakeColor $Gui(brightCream)]
	set Gui(textDark)          [MakeColor $Gui(black)]
	set Gui(textLight)         [MakeColor $Gui(white)]
	set Gui(slice0)            [MakeColor $Gui(red)]
	set Gui(slice1)            [MakeColor $Gui(yellow)]
	set Gui(slice2)            [MakeColor $Gui(green)]

	# ATTRIBUTES
	set attr ""

	# Tab Attributes (TA)
	lappend attr TA
	set Gui(TA) { -font {helvetica 8}\
		-bg $Gui(backdrop) -fg $Gui(textLight) \
		-activebackground $Gui(backdrop) -activeforeground $Gui(textLight) \
		-bd 0 -padx 0 -pady 2 -relief flat \
		-highlightthickness 0 \
		-cursor hand2}

	# Workspace Button Attributes (WBA)
	lappend attr WBA 
	set Gui(WBA) { -font {helvetica 8}\
		-bg $Gui(normalButton) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -activeforeground $Gui(textDark) \
		-bd $Gui(borderWidth) -padx 0 -pady 0 -relief raised \
		-highlightthickness 0 \
		-cursor hand2}

	# Workspace Menu-Button Attributes (WMBA)
	lappend attr WMBA 
	set Gui(WMBA) { -font {helvetica 8}\
		-bg $Gui(normalButton) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -activeforeground $Gui(textDark) \
		-bd $Gui(borderWidth) -padx 0 -pady 0 -relief raised \
		-highlightthickness 0 \
		-cursor hand2}

	# Workspace Menu Attributes (WMA)
	lappend attr WMA 
	set Gui(WMA) { -font {helvetica 8}\
		-bg $Gui(normalButton) -fg $Gui(textDark) -activeborderwidth 2 \
		-activebackground $Gui(activeButton) -activeforeground $Gui(textDark) \
		-bd $Gui(borderWidth) -tearoff 0}

	# Workspace Label Attributes (WLA)
	lappend attr WLA 
	set Gui(WLA) { -font {helvetica 8}\
		-bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-bd 0 -padx 1 -pady 1 -relief flat }

	# Workspace Title Attributes (WTA)
	lappend attr WTA 
	set Gui(WTA) {-font {helvetica 8 bold italic} \
		-bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-bd 0 -padx 1 -pady 1 -relief flat }

	# Workspace Entry Attributes (WEA)
	lappend attr WEA 
	set Gui(WEA) { -font {helvetica 8}\
		-bg $Gui(normalButton) -fg $Gui(textDark) \
		-highlightthickness 0 \
		-bd $Gui(borderWidth) -relief sunken}

	# Workspace Checkbox Attributes (WCA)
	lappend attr WCA 
	set Gui(WCA) { -font {helvetica 8}\
		-bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -highlightthickness 0 \
		-bd $Gui(borderWidth) -selectcolor $Gui(activeButton) \
		-padx 0 -pady 0 -relief flat}

	# Workspace Radiobutton Attributes (WRA)
	lappend attr WRA 
	set Gui(WRA) { -font {helvetica 8}\
		-bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -highlightthickness 0 \
		-bd $Gui(borderWidth) -relief flat}

	# Workspace Scale Attributes (WSA)
	lappend attr WSA 
	set Gui(WSA) { -font {helvetica 8}\
		-bg $Gui(activeWorkspace) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -orient horizontal -showvalue 0 -sliderlength 24 \
		-bd $Gui(borderWidth) -relief flat}

	# Navigation Scale Attributes (BSA)
	lappend attr BSA 
	set Gui(BSA) { -font {helvetica 8}\
		-bg $Gui(backdrop) -fg $Gui(textDark) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -orient vertical -showvalue 0 -sliderlength 24 \
		-bd $Gui(borderWidth) -relief flat}

	# Help Attributes (HA)
	lappend attr HA 
	set Gui(HA) {  -font {helvetica 8}\
		-bg $Gui(inactiveWorkspace) -fg $Gui(textDark) \
		-bd $Gui(borderWidth) -padx 1 -pady 1 -relief flat }

	# Navigation Label Attributes (NLA)
	lappend attr NLA 
	set Gui(NLA) { -font {helvetica 8} \
		-bg $Gui(inactiveWorkspace) -fg $Gui(textDark) \
		-bd $Gui(borderWidth) -padx 1 -pady 1 -relief flat }

	# Navigation Annotation Attributes (NAA)
	lappend attr NAA 
	set Gui(NAA) {-font {courier 8 bold}  \
		-bg $Gui(inactiveWorkspace) -fg $Gui(textDark) \
		-bd 0 -padx 0 -pady 0 -relief flat}

	# Backdrop Label Attributes (BLA)
	lappend attr BLA 
	set Gui(BLA) { -font {helvetica 8}\
		-bg $Gui(backdrop) -fg $Gui(textLight) \
		-bd 0 -padx 1 -pady 1 -relief flat }

	# Change font to large
	if {$Gui(smallFont) == 0} {
		foreach a $attr {
			regsub {helvetica 8} $Gui($a) {helvetica 10 bold} Gui($a)
		} 
	}
}

#-------------------------------------------------------------------------------
# .PROC InfoWidget
# .END
#-------------------------------------------------------------------------------
proc InfoWidget {f} {
	global Gui

	set t [text $f.t -height 6 -setgrid true -wrap word \
		-yscrollcommand "$f.sy set" -cursor arrow -insertontime 0]
	scrollbar $f.sy -orient vert -command "$f.t yview"

	pack $f.sy -side right -fill y
	pack $f.t -side left -fill both -expand true
	return $t
}

#-------------------------------------------------------------------------------
# .PROC MailWidget
# .END
#-------------------------------------------------------------------------------
proc MailWidget {f} {
	frame $f
	pack $f -side top -fill both -expand true

	set t [text $f.t -setgrid true -wrap word -width 42 -height 14 \
		-yscrollcommand "$f.sy set"]
	scrollbar $f.sy -orient vert -command "$f.t yview"

	pack $f.sy -side right -fill y
	pack $f.t -side left -fill both -expand true
	return $t
}

#-------------------------------------------------------------------------------
# .PROC MsgPopup
# .END
#-------------------------------------------------------------------------------
proc MsgPopup {win x y msg {title ""}} {
	YesNoPopup $win $x $y $msg "" "" left $title 
}

#-------------------------------------------------------------------------------
# .PROC YesNoPopup
# .END
#-------------------------------------------------------------------------------
proc YesNoPopup {win x y msg {yesCmd ""} {noCmd ""} \
	{justify center} {title ""}} {
	global Gui

	set w .w$win
	if {[RaisePopup $w] == 1} {return}

	if {$title == ""} {
		set title $Gui(title)
	}
	CreatePopup $w $title $x $y 

	set f $w
	frame $f.fMsg  -bg $Gui(activeWorkspace) -bd 1 -relief raised
	frame $f.fBtns -bg $Gui(inactiveWorkspace)
	pack $f.fMsg $f.fBtns -side top -pady $Gui(pad) -padx $Gui(pad)
	
	set f $w.fMsg
	set c {label $f.l -justify $justify -text "$msg" $Gui(WLA)}; eval [subst $c]
	pack $f.l -padx 5 -pady 5 

	set f $w.fBtns
	if {$yesCmd == ""} {
		set c {button $f.bOk -text "Ok" -width 4 \
			-command "destroy $w" $Gui(WBA)}; eval [subst $c]
		pack $f.bOk -side left -padx $Gui(pad)
	} else {
		set c {button $f.bYes -text "Yes" -width 4 \
			-command "destroy $w; $yesCmd" $Gui(WBA)}; eval [subst $c]
		set c {button $f.bNo -text "No" -width 4 \
			-command "destroy $w; $noCmd" $Gui(WBA)}; eval [subst $c]
		pack $f.bYes $f.bNo -side left -padx $Gui(pad)
	}

}

#-------------------------------------------------------------------------------
# RaisePopup
#
# If the window 'w' exists, then this procedure raises it, gives it the focus,
# and returns 1.  Else returns 0
#-------------------------------------------------------------------------------
proc RaisePopup {w} {
	if {[winfo exists $w] != 0} {
		raise $w
		focus $w
		return 1
	}
	return 0
}

#-------------------------------------------------------------------------------
# CreatePopup
#
# Create window 'w' at screen coordinates (x, y) with 'title'
#-------------------------------------------------------------------------------
proc CreatePopup {w title x y } {
	global Gui
	
	toplevel $w -class Dialog -bg $Gui(inactiveWorkspace)
	wm title $w $title
    wm iconname $w Dialog
    wm transient $w .
	wm geometry $w +$x+$y
	focus $w
}

#-------------------------------------------------------------------------------
# .PROC ShowPopup
# .END
#-------------------------------------------------------------------------------
proc ShowPopup {w x y} {

	# The window must draw itself before we can see its size
	wm deiconify $w
	update idletasks

	set wWin [winfo width  $w]
	set hWin [winfo height $w]
	set wScr [winfo screenwidth  .]
	set hScr [winfo screenheight .]
	
	set xErr [expr $wScr - 30 - ($x + $wWin)]
	if {$xErr < 0} {
		set x [expr $x + $xErr]
	}
	set yErr [expr $hScr - 30 - ($y + $hWin)]
	if {$yErr < 0} {
		set y [expr $y + $yErr]
	}
	
	raise $w
	wm geometry $w +$x+$y
}

#-------------------------------------------------------------------------------
# .PROC MakeVTKImageWindow
# .END
#-------------------------------------------------------------------------------
proc MakeVTKImageWindow {name {input ""}} {
	global Gui

	vtkImageMapper ${name}Mapper
	${name}Mapper SetColorWindow 255
	${name}Mapper SetColorLevel 127.5

	if {$input != ""} {
		${name}Mapper SetInput [$input GetOutput]
	}

	vtkActor2D ${name}Actor
		${name}Actor SetMapper ${name}Mapper
	vtkImager ${name}Imager
		${name}Imager AddActor2D ${name}Actor
	vtkImageWindow ${name}Win
		${name}Win AddImager ${name}Imager
}

#-------------------------------------------------------------------------------
# .PROC ExposeTkImageViewer
# .END
#-------------------------------------------------------------------------------
proc ExposeTkImageViewer {widget x y w h} {

   # Do not render if we are already rendering
   if {[GetWidgetVariableValue $widget Rendering] == 1} {
      return
   }

   # empty the que of any other expose events
   SetWidgetVariableValue $widget Rendering 1
   update
   SetWidgetVariableValue $widget Rendering 0

   # ignore the region to redraw for now.
   $widget Render
}

#-------------------------------------------------------------------------------
# .PROC MakeVTKObject
# .END
#-------------------------------------------------------------------------------
proc MakeVTKObject {shape name} {
	global View

	vtk${shape}Source ${name}Source
	vtkPolyDataMapper ${name}Mapper
		${name}Mapper SetInput [${name}Source GetOutput]
   
# Note: Immediate mode rendering is necessary for rendering the same model
# to more than one render window.  However, it greatly slows performance.

	vtkActor ${name}Actor
		${name}Actor SetMapper ${name}Mapper
		[${name}Actor GetProperty] SetColor 1.0 0.0 0.0
	viewRen AddActor ${name}Actor
}

#-------------------------------------------------------------------------------
# .PROC ScrollSet
# .END
#-------------------------------------------------------------------------------
proc ScrollSet {scrollbar geoCmd offset size} {
	if {$offset != 0.0 || $size != 1.0} {
		eval $geoCmd     ;
		$scrollbar set $offset $size
	} else {
		set manager [lindex $geoCmd 0]
		$manager forget $scrollbar ;
	}
}

#-------------------------------------------------------------------------------
# ScrolledListbox
#
# xAlways is 1 if the x scrollbar should be always visible
#-------------------------------------------------------------------------------
proc ScrolledListbox {f xAlways yAlways {args ""}} {
	global Gui
	
	frame $f
	if {$xAlways == 1 && $yAlways == 1} { 
		listbox $f.list -selectmode single \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand "$f.yscroll set"
	
	} elseif {$xAlways == 1 && $yAlways == 0} { 
		listbox $f.list -selectmode single \
			-xscrollcommand "$f.xscroll set" \
			-yscrollcommand [list ScrollSet $f.yscroll \
				[list grid $f.yscroll -row 0 -column 1 -sticky ns]]

	} elseif {$xAlways == 0 && $yAlways == 1} { 
		listbox $f.list -selectmode single \
			-xscrollcommand [list ScrollSet $f.xscroll \
				[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
			-yscrollcommand "$f.yscroll set"

	} else {
		listbox $f.list -selectmode single \
			-xscrollcommand [list ScrollSet $f.xscroll \
				[list grid $f.xscroll -row 1 -column 0 -sticky we]] \
			-yscrollcommand [list ScrollSet $f.yscroll \
				[list grid $f.yscroll -row 0 -column 1 -sticky ns]]
	}

	if {$Gui(smallFont) == 1} {
		eval {$f.list configure \
			-font {helvetica 7 bold} \
			-bg $Gui(normalButton) -fg $Gui(textDark) \
			-selectbackground $Gui(activeButton) \
			-selectforeground $Gui(textDark) \
			-highlightthickness 0 -bd $Gui(borderWidth) \
			-relief sunken -selectborderwidth $Gui(borderWidth)}
	} else {
		eval {$f.list configure \
			-font {helvetica 8 bold} \
			-bg $Gui(normalButton) -fg $Gui(textDark) \
			-selectbackground $Gui(activeButton) \
			-selectforeground $Gui(textDark) \
			-highlightthickness 0 -bd $Gui(borderWidth) \
			-relief sunken -selectborderwidth $Gui(borderWidth)}
	}

	if {$args != ""} {
		eval {$f.list configure} $args
	}

	scrollbar $f.xscroll -orient horizontal \
		-command [list $f.list xview] \
		-bg $Gui(activeWorkspace) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -bd $Gui(borderWidth)
	scrollbar $f.yscroll -orient vertical \
		-command [list $f.list yview] \
		-bg $Gui(activeWorkspace) \
		-activebackground $Gui(activeButton) -troughcolor $Gui(normalButton) \
		-highlightthickness 0 -bd $Gui(borderWidth)

	grid $f.list $f.yscroll -sticky news
	grid $f.xscroll -sticky news
	grid rowconfigure $f 0 -weight 1
	grid columnconfigure $f 0 -weight 1
	return $f.list
}

#-------------------------------------------------------------------------------
# .PROC MakeColor
# .END
#-------------------------------------------------------------------------------
proc MakeColor {str} {
	return [format "#%02x%02x%02x" \
	[format %.0f [lindex $str 0]] \
	[format %.0f [lindex $str 1]] \
	[format %.0f [lindex $str 2]]]
}

#-------------------------------------------------------------------------------
# .PROC MakeColorNormalized
# .END
#-------------------------------------------------------------------------------
proc MakeColorNormalized {str} {
	return [format "#%02x%02x%02x" \
	[format %.0f [expr [lindex $str 0] * 255.0]] \
	[format %.0f [expr [lindex $str 1] * 255.0]] \
	[format %.0f [expr [lindex $str 2] * 255.0]]]
}

#-------------------------------------------------------------------------------
# .PROC GetRelativeFileName
# .END
#-------------------------------------------------------------------------------
proc GetRelativeFileName {root full} {

	set first [string first $root $full]
	if {$first == 0} {
		set rel [string range $full [expr [string length $root] + 1] end]
		return $rel
	}
	return $full
}

#-------------------------------------------------------------------------------
# .PROC ExpandPath
# .END
#-------------------------------------------------------------------------------
proc ExpandPath {filename} {
	global env Path

	set home $Path(program)
 
	if {[file exists $filename] == 1} {
		return $filename 
	} else {
		return [file join $home $filename]
	}
}

#-------------------------------------------------------------------------------
# .PROC GetPrefix
# .END
#-------------------------------------------------------------------------------
proc GetPrefix {filename} {
	set root [file rootname $filename]
	set f [expr [string last / $root] + 1]
	if {$f == -1} {
		return $root
	}
	set l [expr [string length $root] - 1]
	set prefix [string range $root $f $l]
	return $prefix
}

#-------------------------------------------------------------------------------
# .PROC GetSuffix
# .END
#-------------------------------------------------------------------------------
proc GetSuffix {filename} {
	set f [expr [string last . $filename] + 1]
	if {$f == -1} {
		return ""
	}
	set l [expr [string length $filename] - 1]
	set suffix [string range $filename $f $l]
	return $suffix
}

		
#-------------------------------------------------------------------------------
# GetSaveFile
#
# root     = root path
# rel      = initial path relative to root
# typelist = see documentation for the tk_getSaveFile proc (ie:
#	set typelist {
#		{"MRML File" {".mrml"}}
#		{"All Files" {*}}
#	}
# ext      = default file extension (ie: mrml)
# title    = popup window title (ie: "Save File")
#-------------------------------------------------------------------------------
proc GetSaveFile {root rel typelist ext title {dialog 1}} {

	set path   [file join $root $rel]

	# Allow the prefix to only be a number
	set code   [DecodeFileName $path] 
	set dir    [lindex $code 0]
	set prefix [lindex $code 1]

	# See if the path is just a valid directory
	if {[file isdirectory $path] == 1} {
		# Yes, so use no prefix
		set dir    $path
		set prefix ""
	} else {
		# See if the path is a valid directory and prefix
		if {[file isdirectory $dir] == 1} {
		} else {
			# No, so just use root
			set dir    $root
			set prefix ""
		}
	}

	set num [FindUniqueFileName $dir $prefix $ext]

	if {$dialog == 1} {
		set filename [tk_getSaveFile -title "$title" -defaultextension ".$ext" \
			-filetypes $typelist -initialdir  "$dir" \
			-initialfile [file join $dir "$prefix$num.$ext"]]
	} else {
		set filename [file join $dir "$prefix$num.$ext"]
	}

	return [GetRelativeFileName $root $filename]
}

#-------------------------------------------------------------------------------
# GetOpenFile
#
# root     = root path
# rel      = initial path relative to root
# typelist = see documentation for the tk_getSaveFile proc (ie:
#	set typelist {
#		{"MRML File" {".mrml"}}
#		{"All Files" {*}}
#	}
# ext      = default file extension (ie: mrml)
# title    = popup window title (ie: "Save File")
# forceExt = if 1, forces the user to select a file with extension 'ext'
#-------------------------------------------------------------------------------
proc GetOpenFile {root rel typelist ext title {forceExt 1}} {
	global Gui

	set path   [file join $root $rel]
	set dir    [file dirname $path]
	set prefix [GetPrefix $path]

	# See if the path is just a valid directory
	if {[file isdirectory $path] == 1} {
		# Yes, so use no prefix
		set dir    $path
		set prefix ""
	} else {
		# See if the path is a valid directory and prefix
		if {[file isdirectory $dir] == 1} {
		} else {
			# No, so just use root
			set dir    $root
			set prefix ""
		}
	}

	set filename [tk_getOpenFile -title "$title" -defaultextension ".$ext" \
		-filetypes $typelist -initialdir  "$dir" \
		-initialfile [file join $dir "$prefix.$ext"]]
	
	# User cancelled
	if {$filename == ""} {return}

	if {$forceExt == 1} {
		# Allow *.ext files only
		if {[GetSuffix $filename] != "$ext"} {
			tk_messageBox -icon error -title $Gui(title) \
				-message "Please select a file that ends with '.$ext'"
			return ""
		}
	}

	return [GetRelativeFileName $root $filename]
}

#-------------------------------------------------------------------------------
# .PROC FindUniqueFileName
# .END
#-------------------------------------------------------------------------------
proc FindUniqueFileName {dir prefix ext} {
	
	# Create directory if it does not exist.
	# If this fails, then use the current directory
	if {[file isdirectory $dir] == 0} {
		if {$dir != ""} {
			file mkdir $dir
		}
		if {[file isdirectory $dir] == 0} {
			set dir ""
		}
	}

    # Find a unique filename
    set name [file join $dir $prefix]
    set num 1
    set filename $name$num.$ext
    while {[file exists $filename] == 1} {
        incr num
        set filename $name$num.$ext
    }
	return $num
}

#-------------------------------------------------------------------------------
# .PROC DecodeFileName
# .END
#-------------------------------------------------------------------------------
proc DecodeFileName {filename} {
    set dir  [file dirname  $filename]
    set root [file rootname $filename]
    set i [expr [string length $root] - 1]
    set c [string index $root $i]
    while {$i >= 0 && [lsearch "0 1 2 3 4 5 6 7 8 9" $c] != "-1"} {
        set i [expr $i - 1]
        set c [string index $root $i]
    }  
    if {$i < 0} {
        set i [string length $root]
    }
    set f [expr [string last / $filename] + 1]
    set l $i
    set prefix [string range $root 0 $l]
	return "$dir $prefix" 
}

#-------------------------------------------------------------------------------
# .PROC CheckVolumeExists
# .END
#-------------------------------------------------------------------------------
proc CheckVolumeExists {filePrefix filePattern firstNum lastNum {verbose 1}} {
	global Gui

	# Check that it's a prefix, not a directory
	if {[file isdirectory $filePrefix] == 1} {
		tk_messageBox -icon error -title $Gui(title) \ 
			-message "'$filePrefix' is a directory instead of a prefix."
		return
	}

	# Check directory is there
	set dir [file dirname $filePrefix]
	if {[file isdirectory $dir] == 0} {
		tk_messageBox -icon info -type ok -title $Gui(title) \
		-message "Directory '$dir' does not exist."
		return
	}

	set num $firstNum
	while {$num <= $lastNum} {
		set filename [format $filePattern $filePrefix $num]
		if {[CheckFileExists $filename $verbose] == 0} {
			return $filename
		}
		incr num
	}
	return "" 
}

#-------------------------------------------------------------------------------
# CheckFileExists
#
# Checks if a file exists, is not a directory, and is readable.
# Returns 1 on success, else 0.
#-------------------------------------------------------------------------------
proc CheckFileExists {filename {verbose 1}} {
	global Gui

	if {[file exists $filename] == 0} {
		if {$verbose == 1} {
			tk_messageBox -icon info -type ok -title $Gui(title) \
				-message "File '$filename' does not exist."
		}
		return 0
	}
	if {[file isdirectory $filename] == 1} {
		if {$verbose == 1} {
			tk_messageBox -icon info -type ok -title $Gui(title) \
				-message "'$filename' is a directory, not a file."
		}
		return 0
	}
	if {[file readable $filename] == 0} {
		if {$verbose == 1} {
			tk_messageBox -icon info -type ok -title $Gui(title) \
				-message "'$filename' exists, but is unreadable."
		}
		return 0
	}
	return 1
}

#-------------------------------------------------------------------------------
# .PROC Uncap
# .END
#-------------------------------------------------------------------------------
proc Uncap {s} {
	set a [string tolower [string index $s 0]]
	set b [string range $s 1 [string length $s]]
	return $a$b
}

#-------------------------------------------------------------------------------
# .PROC Cap
# .END
#-------------------------------------------------------------------------------
proc Cap {s} {
	set a [string toupper [string index $s 0]]
	set b [string range $s 1 [string length $s]]
	return $a$b
}

#-------------------------------------------------------------------------------
# .PROC WaitWindow
# .END
#-------------------------------------------------------------------------------
proc WaitWindow {text x y} {
	global Gui

	set tWait .waitWindow
	catch {destroy $tWait}
	
	toplevel $tWait -bg $Gui(backdrop) 
	wm geometry $tWait +$x+$y
	wm title $tWait "Wait..." 

	set f $tWait
    set c {label $f.lDesc -text "$text" \
		-width [expr [string length $text] + 2] $Gui(BLA)}; eval [subst $c]
	pack $f.lDesc -padx 10 -pady 10

	tkwait visibility $tWait

	return $tWait
}


#-------------------------------------------------------------------------------
# .PROC WaitPopup
# .END
#-------------------------------------------------------------------------------
proc WaitPopup {{text "Please wait..."}} {
	global Gui

	incr Gui(waitSemaphore)
	if {$Gui(waitSemaphore) > 1} {return}

	set Gui(waitWin) [WaitWindow $text 150 500]
	raise $Gui(waitWin)
	update

	.tMain config -cursor watch
}

#-------------------------------------------------------------------------------
# .PROC WaitDestroy
# .END
#-------------------------------------------------------------------------------
proc WaitDestroy {} {
	global Gui

	set Gui(waitSemaphore) [expr $Gui(waitSemaphore) - 1]
	if {$Gui(waitSemaphore) > 0} {return}

	if {$Gui(waitWin) == ""} {return}
	destroy $Gui(waitWin)
	set Gui(waitWin) ""
	.tMain config -cursor arrow
}

#-------------------------------------------------------------------------------
# .PROC IsInt
# .END
#-------------------------------------------------------------------------------
proc IsInt {str} {
	set len [string length $str]
	for {set i 0} {$i < $len} {incr i} {
		set c [string index $str $i]
		set n [string match {[0-9-]} $c]
		if {$n == 0} {return 0}
	}
	return 1
}
