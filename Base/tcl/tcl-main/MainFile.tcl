#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
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
#   MainFileGetRelativeDirPrefix
#   MainFileFindUniqueName
#   MainFileCreateDirectory
#   CheckVolumeExists the the the the set the
#   MainFileParseImageFile ImageFile postfixFlag
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
        set Path(printHeader) [file join $Path(program) [file join bin print_header_NT.exe]]
    } elseif {$Gui(linux) == 1} {
        set Path(printHeader) [file join $Path(program) [file join bin print_header_linux]]
    } elseif {$Gui(mac) == 1} {
        set Path(printHeader) [file join $Path(program) [file join bin print_header-darwin]]
    } else {
        set Path(printHeader) [file join $Path(program) [file join bin print_header]]
    }

    # Define Procedures
    lappend Module(procGUI) MainFileBuildGUI

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainFile \
        {$Revision: 1.64 $} {$Date: 2005/11/08 20:13:01 $}]

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
    if {$Gui(pc) == "0"} {
        wm transient $w .
    }
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
    if {$Gui(pc) == "0"} {
        wm transient $w .
    }
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
#  At the beginning, there is a hook for modules who wish to delete anything
#  that doesnt get deleted in the MainMrmlDeleteAll callback
#  To use this, declare the following in your module's init routine:
#  set Module($m,procMainFileCloseUpdateEntered) MyModuleMainFileCloseUpdate.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileClose {} {
    global Module

    # Call each Module's FileCloseUpdate Routine
    #-------------------------------------------
    foreach m $Module(idList) {
        if {[info exists Module($m,procMainFileCloseUpdateEntered)] == 1} {
            if {$Module(verbose) == 1} {puts "procMainFileCloseUpdateEntered: $m"}
            $Module($m,procMainFileCloseUpdateEntered)
        }
    }
    # call Select's close proc to clear out any selections that were made
    SelectClose

    MainMrmlDeleteAll
    MainUpdateMRML
    MainSetup
    RenderAll
    # Restore default MRML file name
    # MainMrmlSetFile "data"
    # Restore default colors
    MainMrmlBuildTreesVersion2.0 [MainMrmlAddColors ""]   
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
    global File

    # Call SaveAs if the filename is blank
    if {$File(filePrefix) == ""} {
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
    global File

    MainOptionsUnparsePresets

    # Call SaveAs if the filename is blank
    if {$File(filePrefix) == ""} {
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
    global Mrml Preset File Options Module
    if {$Module(verbose) == 1} {
        puts "save options"
    }
    # Get presets
    set options [MainOptionsUnparsePresets $Preset(userOptions)]
    if {$Module(verbose)} { puts "MainFileSaveOptions:\n\tUnparsed options $options.\n\tOptions(moduleList) = $Options(moduleList)" }

    # Make a temporary node for presets
    vtkMrmlOptionsNode pre
    pre SetOptions $options
    pre SetProgram slicer
    pre SetContents presets

    # Make a temporary node for modules
    OptionsUpdateModuleList
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
    if {[tree GetErrorCode] != 0} {
        puts "ERROR: MainFileSaveOptions: unable to write Options file $filename"
        DevErrorWindow "ERROR: MainFileSaveOptions: unable to write Options file $filename"
    } else {
        if {$Module(verbose)} {
            puts "Wrote Options file $filename"
        }
    }

    # Clean up.
    tree RemoveAllItems
    tree Delete
    pre Delete
    mod Delete
    if {$Module(verbose) == 1} {
        puts "Save options done"
    }
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
        DevWarningWindow "No file to open specified."
        return
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
    MainOptionsRetrievePresetValues
    # need to pass a scene number here, or user or system defaults
    MainSetup $::Scenes(currentScene)
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
        return $absPrefix
    }
}

#-------------------------------------------------------------------------------
# .PROC MainFileGetRelativeDirPrefix
# Get the dir prefix relative to Mrml(dir), where the Mrml file was last
# saved.  If there is no way to make a relative path, returns the 
# absolute path.
# 
# This was added to fix dicom files in dirs that have . extensions 
# that were getting truncated by the routine above.
# Also, the regexp doesn't work on windows - sp 2002-08-20
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileGetRelativeDirPrefix {dir} {
    global Mrml Gui
    
    # Returns the prefix (no extension) of filename relative to Mrml(dir)
    set root $Mrml(dir)
    set rootlist [file split $root]
    set dirlist [file split $dir]
    if {$Gui(pc) == "1"} {
        set dirlist [lreplace $dirlist 0 0 [string tolower [lindex $dirlist 0]]]
        set rootlist [lreplace $rootlist 0 0 [string tolower [lindex $rootlist 0]]]
    }

    set prefixlist ""
    foreach d $dirlist r $rootlist {
        if {$d == $r} {
            lappend prefixlist $d
            set dirlist [lrange $dirlist 1 end]
        } else {
            break
        }
    }
    if { $prefixlist != "" } {
        return "[eval file join $prefixlist] [eval file join $dirlist]"
    } else {
        return "{} [eval file join $dirlist]"
    }
}

#-------------------------------------------------------------------------------
# .PROC MainFileGetRelativePrefixNew
# Get the dir and file prefix relative to Mrml(dir), where the Mrml file was last
# saved.  If there is no way to make a relative path, returns the 
# absolute path. Returns an empty string if Mrml(dir) and dir are the same. Returns just a 
# relative dir (with trailing file separator) if no file name specified at the end of dir.
# 
# This is an update to GetRelativeDirPrefix to work with generally relative files (works going up the tree as well as down)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainFileGetRelativePrefixNew {dir} {
    global Mrml Gui
    
    # check that dir is actually a directory
    if {[file isdirectory $dir] != 1} {
        set prefix [file tail $dir]
        set justdir [file dirname $dir]
        if {$::Module(verbose)} {
            puts "***************\nWARNING: MainFileGetRelativePrefixNew input $dir is not a directory, stripping to parent dir, $justdir, and saving prefix, $prefix"
        }
    } else {
        set justdir $dir
        set prefix ""
    }

    # check that dir is an absolute path, then split it into directories
    # file pathtype will return absolute even if have ..'s in the path, 
    # as long as it starts from the root dir, so just normalize - but normalize on a relative path starting from a non root dir won't work. So if the pathtype is relative, prepend the mrml dir. Problem now is if it was previously saved and is actually relative to another Mrml(dir)
    if {[file pathtype $justdir] == "relative"} {
        set dirlist [file split ${Mrml(dir)}/${justdir}]
    } else {
        set dirlist [file split [file normalize $justdir]]
    }
    # split the Mrml(dir) into directories
    set rootlist [file split [file normalize $Mrml(dir)]]

    # replace first drive letter with lowercase drive letter on windows
    if {$Gui(pc) == "1"} {
        set dirlist [lreplace $dirlist 0 0 [string tolower [lindex $dirlist 0]]]
        set rootlist [lreplace $rootlist 0 0 [string tolower [lindex $rootlist 0]]]
    }

    # find the common part of the paths, working from the beginning
    set indexOfCommon 0
    while { ($indexOfCommon < [llength $dirlist])
            && ($indexOfCommon < [llength $rootlist])
            && ([lindex $dirlist $indexOfCommon] == [lindex $rootlist $indexOfCommon])} {
        incr indexOfCommon
    }

    # make new lists with just the not common bits
    set uniquedirlist [lrange $dirlist $indexOfCommon end]
    set uniquerootlist [lrange $rootlist $indexOfCommon end]
    set relPath ""
    # note to self: algorithm taken from jdemo's FileMovement proc, f1=rootlist, f2=dirlist
    # for each directory that's not in dirlist's path, add .. to the relative path
    foreach d $uniquerootlist {
        lappend relPath ".."
    }

    # for each directory that's not in rootlist's path, add the dir from dirlist 
    # to the relative path
    foreach d $uniquedirlist {
        lappend relPath $d
    }

    # put the path together with the system file separator - add the prefix if it's not empty
    if {$prefix != ""} {
        lappend relPath $prefix
    }
    if {$::Module(verbose)} {
        puts "MainFileGetRelativePrefixNew: Done.\n\tfile dir = $justdir with prefix $prefix,\n\tmrml dir = $Mrml(dir),\n\trelative path = [join $relPath /]"
    }
    return [join $relPath /]

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
# filePrefix the directory path and file name up to the separator
# filePattern the argument passed to format that builds the full file name
# firstNum the number of the first file in the volume
# lastNum the number of the last file in the volume
# verbose set to 1 if you wish more information about execution
# afterStuff the final characters in the file name pattern
# .END
#-------------------------------------------------------------------------------
proc CheckVolumeExists {filePrefix filePattern firstNum lastNum  {verbose 0} } {
    global Gui

    if {$::Module(verbose)} {
        puts "CheckVolumeExists:\n\tfilePrefix = $filePrefix \tfilePattern = $filePattern\n\tfirstNum = $firstNum\n\tlastNum = $lastNum"
    }
    # Check that it's a prefix, not a directory
    if {[file isdirectory $filePrefix] == 1} {
        # if the file pattern is from a dicom, the prefix may be  a directory
        tk_messageBox -icon error -title $Gui(title) -message \
            "CheckVolumeExists: '$filePrefix' is a directory instead of a prefix, unless this is a DICOM volume, errors may ensue.\n(pattern = $filePattern)."
        # return ERROR
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
    set mrmlFilename ""
    if {[file exists $filename] == 0} {
        if {[file pathtype $filename] == "relative"} {
            # try prepending the mrml dir to it
            set mrmlFilename [file join $::Mrml(dir) $filename]
            if {[file exists $mrmlFilename] == 0} {
                if {$verbose == 1} {
                    tk_messageBox -icon info -type ok -title $Gui(title) -message \
                        "CheckFileExists: File '$filename' does not exist, and nor does $mrmlFilename."
                    puts "CheckFileExists: File '$filename' does not exist, and nor does $mrmlFilename."
                }
                return 0
            }
        } else {
            return 0
        }
    }
    if {[file isdirectory $filename] == 1} {
        if {$verbose == 1} {
            tk_messageBox -icon info -type ok -title $Gui(title) -message \
                "'$filename' is a directory, not a file."
        }
        return 0
    }
    if {[file readable $filename] == 0} {
        if {$mrmlFilename != ""} {
            if {[file readable $mrmlFilename] == 0} {
                if {$verbose == 1} {
                    tk_messageBox -icon info -type ok -title $Gui(title) -message \
                        "'$filename' exists, but is unreadable."
                }
                return 0
            }
        } else {
            return 0
        }
    }
    return 1
}

#-------------------------------------------------------------------------------
# .PROC MainFileParseImageFile
#
# Takes in the imagefile and returns
# the pattern,prefix, number, and stuff after the number in a list
#
# example: /tmp/file.001.gz returns  %s.%03d%s /tmp/file 1 .gz
# example: /tmp/file.001    returns  %s.%03d%s /tmp/file 1  
#
# .ARGS
# str ImageFile the file name
# int postfixFlag set to true by default, processes the filename with a postfix after the number
# .END
# 
#-------------------------------------------------------------------------------
proc MainFileParseImageFile {ImageFile {postfixFlag 1}} {

    if {$::Module(verbose)} {
        puts "MainFileParseImageFile: file = \"$ImageFile\""
    }
    # skip empty filenames - these come, for example, in dicom files
    if {$ImageFile == ""} {
        if {$::Module(verbose)} {
           puts "MainFileParseImageFile: passed emtpy filename, returning nothing"
        }
        return
    }
    
    # two possibilities: a file name that has the numbers after a separator character, or before, with a constant extention
    set ftail [file tail $ImageFile]
    set fdir [file dirname $ImageFile]
    set fext [file extension $ftail]
    set fname [file rootname $ftail]
    # update: instead of using a regexp to see if the file name starts with letters, 
    # check to see if the file name is constant and therefore the extension is changing.
    # this tests to see if anything else in the directory has the same extension as the first file, 
    # if nothing else does (glob only returns the file name we're checking against), then it is 
    # assumed that the file starts with a constant part
    if {$::Module(verbose)} {
        puts "MainFileParseImageFile: ftail = $ftail\n\tfdir = $fdir\n\tfext = \"$fext\""
    }
    if {$fext == ".mgh"} {
        if {$::Module(verbose)} {
            puts "MainFileParseImageFile: have a volume with only one file, returning a pattern of %s."
        }
        return "%s $ImageFile 0"
    }
    if {$fext == ".bfloat" || $fext == ".bshort"} {
        if {$::Module(verbose)} {
            puts "MainFileParseImageFile: freesurfer binary volume, returning default pattern of %s_%03d${fext}."
        }
        if {[regexp {(.+)_([0-9]*)$} $fname match filePrefix num] == 0} {
            set filePrefix $fdir/$fname
            set ZerolessNum 0
        } else {
            set filePrefix $fdir/$filePrefix
            set ZerolessNum [string trimleft $num "0"]
            if {$ZerolessNum == ""} {set ZerolessNum 0}
        }
        return "%s_%03d.bfloat $filePrefix $ZerolessNum"
    }
    if {$fext == ".pgi" || $fext == ".PGI" || $fext == ".mr" || $fext == ".MR"} {
        if {$::Module(verbose)} {
            puts "MainFileParseImageFile: GE volume, returning pattern of %s%03d$fext."
        }
        # If filename is of format <name>###.pgi, then parse it; else exit with error message
        if {[regexp {(.+)([0-9][0-9][0-9])$} $fname match filePrefix num] == 0} {
            if {$::Module(verbose)} {
                puts "MainFileParseImageFile: expected filename format %s%03d$fext."
            }
            return
        }
        set filePrefix $fdir/$filePrefix
        set ZerolessNum [string trimleft $num "0"]
        if {$ZerolessNum == ""} {set ZerolessNum 0}
        return "%s%03d$fext $filePrefix $ZerolessNum"
    }
    # this will fail if there's another volume in the directory with the same 
    # extension: second test = ftail is in the list, and any other elements
    # have a different rootname. First test that there *is* an extension. Last test
    # that the file name isn't all numbers before the extension
    set filesWithSameExtension [glob -directory $fdir -tails *$fext]
    if {$::Module(verbose)} {
        puts "files with same extension = $filesWithSameExtension"
    }
    if {$fext != "" && ($filesWithSameExtension == $ftail || ([lsearch $filesWithSameExtension $ftail] != -1 && [lsearch $filesWithSameExtension [file rootname $ImageFile]\*] == -1 && [string is integer $fname] != 1))} {
        # the file starts with letters
        if {$::Module(verbose)} {
            puts "File starts with letters"
        }
        ##  Parse the file into its prefix, number, and perhaps stuff afterwards
        
        ##   Note: find the last consecutive string of digits
        ## Added support for - as well as . as a file separator, to add another one, 
        ##   replace the instances of [\.-] in the following regexp
        set filePostfix ""
        if {[regexp {^(.+)[\.-]([0-9]+)([\.-][^[0-9]*)?$} $ImageFile match filePrefix num filePostfix] == 0} {
            if {$::Module(verbose)} {
                DevErrorWindow "Could not parse \"$ImageFile\" in MainFileParseImageFile (postfixFlag = $postfixFlag)"
            }
            puts "Could not parse \"$ImageFile\" in MainFileParseImageFile\n\tpostfixFlag = $postfixFlag\n\tftail = $ftail\n\tfdir = $fdir\n\tfext = \"$fext\""
            return ""
        }
        
        # Get rid of unnecessary 0's
        set ZerolessNum [string trimleft $num "0"]
        if {$ZerolessNum == ""} {set ZerolessNum 0}
        # find the separator character
        if {[regexp "^${filePrefix}(.*)${num}" $ImageFile match sepChars] == 0} {
            # try assuming characters after num
            
            DevErrorWindow "Could not find the seperator character in \"$ImageFile\" between ${filePrefix} and ${num}"
            return ""
        }
        ## Did we trim zeros? This tells us how to look for files
        if { [string equal $ZerolessNum $num] == 1 } {
            set pattern "%s${sepChars}%d";        
        } else {
            ## Someday, we'll have to check for things other than 001... used to be %03d, try counting the number of chars in num
            set pattern "%s${sepChars}%0[string length $num]d";
        }
        # if we're going to check for postfix strings on the file name after the number, ie .gz, append a string variable to the pattern
        if {$postfixFlag} {
            append pattern "%s"
        }
    } else {
        # assume that the number part is first, then a constant extenion
        if {$::Module(verbose)} {
            puts "Trying to parse as a dicom file"
        }
        set filePrefix [file dirname $ImageFile]/
        set num [file rootname [file tail $ImageFile]]
        # check to see if we really have a number here
        if {[regexp {(^.*[a-zA-Z]+)([0-9]*)} $num match moreLetters realNum] == 1} {
            if {$::Module(verbose)} {
                puts "MainFileParseImageFile: WARNING there were letters in my number: $num\n\tResetting prefix to $filePrefix, num to $realNum"
            }
            # append the letters to the prefix
            set filePrefix $filePrefix${moreLetters}
            # set the number to be just the number part (assumes letters then number)
            set num $realNum
            
        }
        set ZerolessNum [string trimleft $num "0"]
        if {$ZerolessNum == ""} {set ZerolessNum 0}
        set filePostfix ""
        set pattern "%s%0[string length $num]d[file extension $ImageFile]"
    }
    set a ""
    
    lappend a  $pattern $filePrefix $ZerolessNum $filePostfix
    if {$::Module(verbose)} {
        puts "MainFileParseImageFile: returning parsed imge file $a"
    }
    return $a
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
    if {$::Module(verbose)} {
        puts "MainFileFindImageNumber: working on $firstFile"
    }
    set parsing [MainFileParseImageFile $firstFile]

    if {$parsing == ""} {
        if {$::Module(verbose)} {
            DevErrorWindow "MainFileFindImageNumber: First file $firstFile cannot be parsed"
        }
        puts "MainFileFindImageNumber: First file $firstFile cannot be parsed"
        return ""
    }
    set pattern    [lindex $parsing 0]
    set filePrefix [lindex $parsing 1]
    set firstNum   [lindex $parsing 2]
    set filePostfix [lindex $parsing 3]

    ## Do they just want the first number?
    if {$which == "First"} { return $firstNum  }

#    puts "MainFileFindImageNumber: Pattern: $pattern"
    set firstFile [format $pattern $filePrefix $firstNum $filePostfix]
#    puts "MainFileFindImageNumber: firstFile: \"$firstFile\" "

    # See if first file exists.  If not, then we're powerless.
    if {[CheckFileExists $firstFile 0] == 0} {
        DevErrorWindow "MainFileFindImageNumber: First file $firstFile does not exist"
        return ""
    }

    # Find last file number
    set lastNum $firstNum
    set done 0
    set num $firstNum
    if {$::Module(verbose)} {
            puts "MainFileFindImageNumber: pattern = $pattern, prefix = $filePrefix, num = $num, postfix = $filePostfix"
    }
    while {$done == 0} {
        set fileName [format $pattern $filePrefix $num $filePostfix]
        if {$::Module(verbose) && ($num > 120)} {
            puts "MainFileFindImageNumber: checking for last number, current \# = $num, file = $fileName"
        }
        if {[CheckFileExists $fileName 0] == 0} {
            set done 1
            set lastNum [expr $num - 1]
        }
        incr num
    }
    # puts "MainFileFindImageNumber: last number $lastNum"
    return $lastNum
}
