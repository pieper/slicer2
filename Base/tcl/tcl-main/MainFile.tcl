
proc MainFileInit {} {
	global Module File Path

	lappend Module(procGUI) MainFileBuildGUI

	set File(filePrefix) $Path(prefixOpenFile)
}

proc MainFileBuildGUI {} {
	global Gui File

	#-------------------------------------------
	# the "Open File" Popup Window
	#-------------------------------------------
	set w .wOpen
	set File(wOpen) $w
	toplevel $w -class Dialog -bg $Gui(inactiveWorkspace)
	wm title $w "Open File"
    wm iconname $w Dialog
    wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
    wm transient $w .
	wm withdraw $w

	# Frames
	frame $w.fTop  -bg $Gui(activeWorkspace) -bd 2 -relief raised
	frame $w.fBtns -bg $Gui(inactiveWorkspace)
	pack $w.fTop $w.fBtns -side top -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Top frame
	#-------------------------------------------
	set f $w.fTop
	frame $f.fHelp -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fHelp $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# Top->Help frame
	#-------------------------------------------
	set f $w.fTop.fHelp

	set c {label $f.lTitle -text "Open a MRML file." $Gui(WLA)}; eval [subst $c]
	pack $f.lTitle
	
	#-------------------------------------------
	# Top->Grid frame
	#-------------------------------------------
	set f $w.fTop.fGrid

	set c {button $f.b -text "Prefix:" -width 7 \
		-command "MainFileOpen" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.e -textvariable File(filePrefix) -width 60 $Gui(WEA)}
		eval [subst $c]
	bind $f.e <Return> {MainFileOpen}
	pack $f.b -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -fill x -expand 1

	#-------------------------------------------
	# Top->Buttons frame
	#-------------------------------------------
	set f $w.fBtns
	set c {button $f.bCancel -text "Cancel" \
		-command "wm withdraw $w" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bApply -text "Apply" \
		-command "wm withdraw $w; MainFileOpenApply" $Gui(WBA)}; eval [subst $c]
	pack $f.bApply $f.bCancel -side left -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpenPopup
# .END
#-------------------------------------------------------------------------------
proc MainFileOpenPopup {{callback ""} {x 100} {y 100}} {
	global Gui File

	# Recreate popup if user killed it
	if {[winfo exists $File(wOpen)] == 0} {
		MainFileBuildGUI
	}
	
	set File(callback) $callback

	ShowPopup $File(wOpen) $x $y
}

proc MainFileOpen {} {
	global Path File
	
	set typelist {
		{"MRML Files" {.mrml}}
		{"All Files" {*}}
	}
	set filename [GetOpenFile $Path(root) $File(filePrefix) \
		$typelist .mrml "Open Scene" 1 $File(wOpen)]
	if {$filename == ""} {return}

    # Store for next time 
	set Path(prefixOpenFile) [file rootname $filename]
	set File(filePrefix)   $Path(prefixOpenFile)
}

proc MainFileOpenApply {} {
	global File Path

	# Relative to root
	set mrmlFile [file join $Path(root) $File(filePrefix).mrml]

	MainMrmlReadDag $mrmlFile
	MainUpdateMRML
	MainSetup
	RenderAll

	if {$File(callback) != ""} {
		$File(callback)
	}


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
# ext      = default file extension (ie: .mrml) including the dot
# title    = popup window title (ie: "Save File")
# forceExt = if 1, forces the user to select a file with extension 'ext'
#-------------------------------------------------------------------------------
proc GetOpenFile {root rel typelist ext title {forceExt 1} {widget ""}} {
	global Gui

	set path   [file join $root $rel]
	set dir    [file dirname $path]
	# Don't use "."
	if {$dir == "."} {
		set dir [pwd]
	}
	set prefix [file root [file tail $path]]

	# See if the path is just a valid directory
	if {[file isdirectory $path] == 1} {
		# Yes, so use no prefix
		set dir    $path
		set prefix ""
	} else {
		# See if the path is a valid directory and prefix
		if {[file isdirectory $dir] == 0} {
			# No, so just use root
			set dir    $root
			set prefix ""
		}
	}

	set filename [tk_getOpenFile -title "$title" -defaultextension "$ext" \
		-filetypes $typelist -initialdir  "$dir" \
		-initialfile [file join $dir "$prefix$ext"]]
	
	# User cancelled
	if {$filename == ""} {return}

	if {$forceExt == 1} {
		# Allow *ext files only
		if {[file extension $filename] != "$ext"} {
			tk_messageBox -icon error -title $Gui(title) \
				-message "Please select a file that ends with '$ext'"
			if {$widget != ""} {
				focus $widget
			}
			return ""
		}
	}

	if {$widget != ""} {
		focus $widget
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
# .PROC DecodeFileName
# .END
#-------------------------------------------------------------------------------
proc DecodeFileName {filename} {
    set dir  [file dirname  $filename]
	# Don't use "."
	if {$dir == "."} {
		set dir [pwd]
	}
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
proc CheckVolumeExists {filePrefix filePattern firstNum lastNum {verbose 0}} {
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
			if {$filename == ""} {
				return filename
			}
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
# MainFileFindLastImageNumber
#
# 'firstFile' is a full path 
#-------------------------------------------------------------------------------
proc MainFileFindLastImageNumber {firstFile} {

	scan [file extension $firstFile] "%d" firstNum
	if {[info exists firstNum] == 0} {
		set firstNum 0
	}

	# Find filePattern of firstFile
	set filePrefix [file root $firstFile]
	set pattern ""
	foreach volumeFirstPattern "%s.%03d %s.%d" {
		set fileName [format $volumeFirstPattern $filePrefix $firstNum]
		if {$fileName == $firstFile} {
			set pattern $volumeFirstPattern
		}
	}
	if {$pattern == ""} {
		return ""
	}

	# See if first file exists.  If not, then we're powerless.
	if {[CheckFileExists $firstFile 0] == 0} {
		return ""
	}

	# Find last file number
	set lastNum $firstNum
	set done 0
	set num $firstNum
	while {$done == 0} {
		set fileName [format $pattern $filePrefix $num]
		if {[CheckFileExists $fileName 0] == 0} {
			set done 1
			set lastNum [expr $num - 1]
		}
		incr num
	}

	return $lastNum
}


