#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        MainFile.tcl
# PROCEDURES:  
#   MainFileInit
#   MainFileBuildGUI
#   MainFileBuildOpenGUI
#   MainFileBuildSaveAsGUI
#   MainFileClose
#   MainFileSaveAsPopup
#   MainFileSaveAs
#   MainFileSaveAsApply
#   MainFileSave
#   MainFileSaveWithOptions
#   MainFileSaveOptions
#   MainFileOpenPopup
#   MainFileOpen
#   MainFileOpenApply
#   MainFileSaveModel
#   MainFileOpenModel
#   MainFileSaveVolume
#   MainFileOpenVolume
#   MainFileGetRelativePrefix
#   MainFileFindUniqueName
#   MainFileCreateDirectory
#   CheckVolumeExists
#   MainFileFindImageNumber which firstfile
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainFileInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileInit {} {
	global Module File Path Gui

	if {$Gui(pc) == 1} {
		set Path(printHeader) [file join $Path(program) [file join bin print_header_NT]]
	} else {
		set Path(printHeader) [file join $Path(program) [file join bin print_header]]
	}

	# Define Procedures
	lappend Module(procGUI) MainFileBuildGUI

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainFile \
		{$Revision: 1.22 $} {$Date: 2000/08/28 18:22:18 $}]

	set File(filePrefix) data
}

#-------------------------------------------------------------------------------
# .PROC MainFileBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileBuildGUI {} {
	global Gui File

	MainFileBuildOpenGUI
	MainFileBuildSaveAsGUI
}

#-------------------------------------------------------------------------------
# .PROC MainFileBuildOpenGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileBuildOpenGUI {} {
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

	eval {label $f.lTitle -text "Open a MRML file with this prefix:"} $Gui(WLA)
	pack $f.lTitle
	
	#-------------------------------------------
	# Top->Grid frame
	#-------------------------------------------
	set f $w.fTop.fGrid

	eval {button $f.b -text "Browse:" -width 7 \
		-command "MainFileOpen"} $Gui(WBA)
	eval {entry $f.e -textvariable File(filePrefix) -width 60} $Gui(WEA)
	bind $f.e <Return> {MainFileOpen}
	pack $f.b -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -fill x -expand 1

	#-------------------------------------------
	# Top->Buttons frame
	#-------------------------------------------
	set f $w.fBtns
	eval {button $f.bCancel -text "Cancel" \
		-command "wm withdraw $w"} $Gui(WBA)
	eval {button $f.bApply -text "Apply" \
		-command "wm withdraw $w; MainFileOpenApply"} $Gui(WBA)
	pack $f.bApply $f.bCancel -side left -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC MainFileBuildSaveAsGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileBuildSaveAsGUI {} {
	global Gui File

	#-------------------------------------------
	# the "SaveAs File" Popup Window
	#-------------------------------------------
	set w .wSaveAs
	set File(wSaveAs) $w
	toplevel $w -class Dialog -bg $Gui(inactiveWorkspace)
	wm title $w "SaveAs File"
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

	eval {label $f.lTitle -text "Save a MRML file with this prefix:"} $Gui(WLA)
	pack $f.lTitle
	
	#-------------------------------------------
	# Top->Grid frame
	#-------------------------------------------
	set f $w.fTop.fGrid

	eval {button $f.b -text "Browse:" -width 7 \
		-command "MainFileSaveAs"} $Gui(WBA)
	eval {entry $f.e -textvariable File(filePrefix) -width 60} $Gui(WEA)
	bind $f.e <Return> {MainFileSaveAs}
	pack $f.b -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -fill x -expand 1

	#-------------------------------------------
	# Top->Buttons frame
	#-------------------------------------------
	set f $w.fBtns
	eval {button $f.bCancel -text "Cancel" \
		-command "wm withdraw $w"} $Gui(WBA)
	eval {button $f.bApply -text "Apply" \
		-command "wm withdraw $w; MainFileSaveAsApply"} $Gui(WBA)
	pack $f.bApply $f.bCancel -side left -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC MainFileClose
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileClose {} {

	MainMrmlDeleteAll
	MainUpdateMRML
	MainSetup
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveAsPopup
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveAsPopup {{callback ""} {x 100} {y 100}} {
	global Gui File

	# Recreate popup if user killed it
	if {[winfo exists $File(wSaveAs)] == 0} {
		MainFileBuildSaveAsGUI
	}
	
	set File(callback) $callback

	ShowPopup $File(wSaveAs) $x $y
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveAs
# Presents a popup to allow the user to set a new File(filePrefix).
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveAs {} {
	global Mrml File Gui
	
	# Cannot have blank prefix
	if {$File(filePrefix) == ""} {
		set File(filePrefix) data
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $File(filePrefix)]
	set dir [file dirname $filename]
	set typelist {
		{"XML Files" {.xml}}
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Scene" -defaultextension ".xml"\
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Make it a relative prefix
	set File(filePrefix) [MainFileGetRelativePrefix $filename]

	MainFileSaveAsApply
	wm withdraw $File(wSaveAs)
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveAsApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveAsApply {} {
	global File Mrml

	# Prefix cannot be blank
	if {$File(filePrefix) == ""} {
		tk_messageBox -message "A file name must be specified"
	}
	
	# Relative to root
	set filename [file join $Mrml(dir) $File(filePrefix).xml]

	MainMrmlWrite $filename
}

#-------------------------------------------------------------------------------
# .PROC MainFileSave
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileSave {} {
	global Mrml

	# Call SaveAs if the filename is blank
	if {$Mrml(filePrefix) == ""} {
		MainFileSaveAsPopup "" 50 50
	}

	MainFileSaveAsApply
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveWithOptions
# Puts Options in current MRML file.
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveWithOptions {} {
	global Mrml

	MainOptionsUnparsePresets

	# Call SaveAs if the filename is blank
	if {$Mrml(filePrefix) == ""} {
		MainFileSaveAsPopup "" 50 50
	}

	MainFileSaveAsApply
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveOptions
#  Saves Options.xml 
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveOptions {} {
    global Mrml Preset File Options
    puts "save options"    
    # Get presets
    set options [MainOptionsUnparsePresets $Preset(userOptions)]
    
    # Make a temporary node for presets
    vtkMrmlOptionsNode pre
    pre SetOptions $options
    pre SetProgram slicer
    pre SetContents presets

    # Make a temporary node for modules
    vtkMrmlOptionsNode mod
    mod SetOptions $Options(moduleList)
    mod SetProgram slicer
    mod SetContents modules

    # Make a temporary tree for writing
    vtkMrmlTree tree
    tree AddItem pre
    tree AddItem mod

    # Write Options.xml
    set filename Options.xml
    tree Write $filename

    # Clean up.
    tree RemoveAllItems
    tree Delete
    pre Delete
    mod Delete
puts "save options done"
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpenPopup
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileOpenPopup {{callback ""} {x 100} {y 100}} {
	global Gui File

	# Recreate popup if user killed it
	if {[winfo exists $File(wOpen)] == 0} {
		MainFileBuildOpenGUI
	}
	
	# not using this
	set File(callback) $callback

	ShowPopup $File(wOpen) $x $y
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpen
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileOpen {} {
	global Mrml File
	
	# Cannot have blank prefix
	if {$File(filePrefix) == ""} {
		set File(filePrefix) data
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $File(filePrefix)]
	set dir [file dirname $filename]
	set typelist {
		{"XML Files" {.xml}}
		{"MRML Files" {.mrml}}
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "Open File" -defaultextension ".xml" \
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]
	if {$filename == ""} {return}

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Make it a relative prefix
	set File(filePrefix) [MainFileGetRelativePrefix $filename]

	# If it's MRML instead of XML, then add the .mrml back
	if {[regexp {.*\.mrml$} $filename] == 1} {
		set File(filePrefix) $File(filePrefix).mrml
	}

	wm withdraw $File(wOpen)
	MainFileOpenApply
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpenApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileOpenApply {} {
	global File Mrml

	# Prefix cannot be blank
	if {$File(filePrefix) == ""} {
		tk_messageBox -message "A file name must be specified"
	}
	
	# Relative to root.
	# If it's MRML instead of XML, then don't add the .xml
	if {[regexp {.*\.mrml$} $File(filePrefix)] == 0} {
		set filename [file join $Mrml(dir) $File(filePrefix).xml]
	} else {
		set filename [file join $Mrml(dir) $File(filePrefix)]
	}

	MainMrmlRead $filename
	MainUpdateMRML
	MainSetup
	RenderAll

	if {$File(callback) != ""} {
		$File(callback)
	}
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveModel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveModel {m prefix} {
	global Model Mrml

	# Ensure model exists
	if {$m == "" || $m == "NEW" || [lsearch $Model(idList) $m] == -1} {
		tk_messageBox -message "\
To save a model, it must already exist in the 3D Slicer."
		return ""
	}

	# Cannot have blank prefix
	if {$prefix == ""} {
		set prefix [Model($m,node) GetName]
		if {$prefix == ""} {	
			set prefix model
		}
	}

 	# Show popup initialized to "prefix"
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"VTK Files" {.vtk}}
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Model" -defaultextension .vtk \
		-filetypes $typelist -initialdir $dir -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return ""}

	# Remember to store it as a relative prefix for next time
	return [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpenModel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileOpenModel {m prefix} {
	global Model Mrml

	# Ensure model exists
	if {$m == "" || $m == "NEW" || [lsearch $Model(idList) $m] == -1} {
		tk_messageBox -message "\
Use this utility to re-read a model that is already in the 3D Slicer.\n\
To read a model for the first time, click 'Add Model' on the Data panel."
		return ""
	}

	# Cannot have blank prefix
	if {$prefix == ""} {
		set prefix [Model($m,node) GetName]
		if {$prefix == ""} {	
			set prefix model
		}
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"VTK Files" {.vtk}}
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "Open Model" -defaultextension .vtk \
		-filetypes $typelist -initialdir $dir -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return ""}

	# Remember to store it as a relative prefix for next time
	return [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC MainFileSaveVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileSaveVolume {v prefix} {
	global Volume Mrml

	# Ensure volume exists
	if {$v == "" || $v == "NEW" || [lsearch $Volume(idList) $v] == -1} {
		tk_messageBox -message "\
To save a volume, it must already exist in the 3D Slicer."
		return ""
	}

	# Cannot have blank prefix
	if {$prefix == ""} {
		set prefix [Volume($v,node) GetName]
		if {$prefix == ""} {	
			set prefix volume
		}
	}

 	# Show popup initialized to "prefix"
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Volume" \
		-filetypes $typelist -initialdir $dir -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return ""}

	# Remember to store it as a relative prefix for next time
	return [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC MainFileOpenVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileOpenVolume {v prefix} {
	global Volume Mrml

	# Ensure volume exists
	if {$v == "" || $v == "NEW" || [lsearch $Volume(idList) $v] == -1} {
		tk_messageBox -message "\
Use this utility to re-read a volume that is already in the 3D Slicer.\n\
To read a volume for the first time, click 'Add Volume' on the Data panel."
		return ""
	}

	# Cannot have blank prefix
	if {$prefix == ""} {
		set prefix [Volume($v,node) GetName]
		if {$prefix == ""} {	
			set prefix volume
		}
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $prefix]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "Open Volume" \
		-filetypes $typelist -initialdir $dir -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return ""}

	# Remember to store it as a relative prefix for next time
	return [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC MainFileGetRelativePrefix
# Get the file prefix relative to Mrml(dir), where the Mrml file was last
# saved.  If there is no way to make a relative path, returns the 
# absolute path.
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileGetRelativePrefix {filename} {
	global Mrml Gui
	
	# Returns the prefix (no extension) of filename relative to Mrml(dir)
	set root $Mrml(dir)
	set absPrefix [file rootname $filename]

	if {[regexp "^$root/(\.*)" $absPrefix match relPrefix] == 1} {
		return $relPrefix
	} else {
		return [file rootname $absPrefix]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainFileFindUniqueName
# Form an absolute filename by concatenating the root, name, and ext.
# If a file of this name already exists, then find a number to add before the
# extension that would make it unique.  Return this filename.
# Note: the prefix cannot be blank, or "" is returned.
# .END
#-------------------------------------------------------------------------------
proc MainFileFindUniqueName {root prefix ext} {

	# See if the extension is already there
	if {[expr [string length [file rootname $prefix]] + 1] == \
		[string last $ext $prefix]} {
		set prefix [file rootname $prefix]
	}
	
	# The prefix cannot be blank
	if {$prefix == ""} {
		return ""
	}

    # Form an absolute prefix
    set abs [file join $root $prefix]

	set num ""
    set filename $abs$num$ext
    if {[file exists $filename] == 1} {
		set num 1
		set filename $abs$num$ext
		while {[file exists $filename] == 1} {
			incr num
			set filename $abs$num$ext
		}
	}
	return $filename
}

#-------------------------------------------------------------------------------
# .PROC MainFileCreateDirectory
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileCreateDirectory {filename} {
	# Create directory if it does not exist.
	# If this fails, then use the current directory

	set dir [file dirname $filename]
	if {[file isdirectory $dir] == 0} {
		if {$dir != ""} {
			file mkdir $dir
		}
		if {[file isdirectory $dir] == 0} {
			set dir ""
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC CheckVolumeExists
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CheckVolumeExists {filePrefix filePattern firstNum lastNum {verbose 0}} {
	global Gui

	# Check that it's a prefix, not a directory
	if {[file isdirectory $filePrefix] == 1} {
		tk_messageBox -icon error -title $Gui(title) -message \
			"'$filePrefix' is a directory instead of a prefix."
		return ERROR
	}

	# Check directory is there
	set dir [file dirname $filePrefix]
	if {[file isdirectory $dir] == 0} {
		tk_messageBox -icon error -title $Gui(title) -message \
			"Directory '$dir' does not exist."
		return ERROR
	}

	set num $firstNum
	while {$num <= $lastNum} {
		set filename [format $filePattern $filePrefix $num]
		if {[CheckFileExists $filename $verbose] == 0} {
			if {$filename == ""} {
				# Return the word, filename just to indicate error
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
			tk_messageBox -icon info -type ok -title $Gui(title) -message \
				"File '$filename' does not exist."
		}
		return 0
	}
	if {[file isdirectory $filename] == 1} {
		if {$verbose == 1} {
			tk_messageBox -icon info -type ok -title $Gui(title) -message \
				"'$filename' is a directory, not a file."
		}
		return 0
	}
	if {[file readable $filename] == 0} {
		if {$verbose == 1} {
			tk_messageBox -icon info -type ok -title $Gui(title) -message \
				"'$filename' exists, but is unreadable."
		}
		return 0
	}
	return 1
}

#-------------------------------------------------------------------------------
# .PROC MainFileFindImageNumber
#
# .ARGS
# str which \"First\" means the image being sent is the first one. Otherwise, it means nothing.
# str firstfile the full path
# .END
# 
#-------------------------------------------------------------------------------
proc MainFileFindImageNumber {which firstFile} {

	if {[regexp {\.([0-9]*)([^0-9]*)$} $firstFile match firstNum suffix] == 0} {
		return ""
	}
	# Rid unnecessary 0's
	set firstNum [string trimleft $firstNum "0"]
	if {$firstNum == ""} {set firstNum 0}

	if {$which == "First"} {
		return $firstNum
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


