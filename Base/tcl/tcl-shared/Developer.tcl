#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
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
# FILE:        Developer.tcl
# PROCEDURES:  
#   DevYesNo message
#   DevOKCancel message
#   DevWarningWindow message
#   DevInfoWindow message
#   DevErrorWindow message
#   DevFatalErrorWindow message
#   DevAddLabel LabelName Message Color
#   DevAddEntry
#   DevAddButton ButtonName Message Command Width
#   DevAddSelectButton TabName f Label Message Pack Tooltip width color
#   DevUpdateNodeSelectButton ArrayName type Label Name CommandSet None New LabelMap
#   DevUpdateNodeSelectButton
#   DevUpdateSelectButton ArrayName Label Name ChoiceList Command
#   DevSelectNode type id ArrayName ModelLabel ModelName
#   DevCreateNewCopiedVolume VolumeId Description VolName
#   DevGetFile filename MustPop DefaultExt DefaultDir Title Action
#   DevAddFileBrowse Frame ArrayName VarFileName Message Command DefaultExt DefaultDir Action Title Tooltip PathType
#   DevCreateScrollList ScrollFrame ItemCreateGui ScrollListConfigScrolledGUI: ItemList
#   DevCheckScrollLimits 
#   DevCheckScrollLimits
#   DevFileExists
#   DevSourceTclFilesInDirectory dir verbose
#==========================================================================auto=
# This file exists specifically for user to help fast development
# of Slicer modules
#
# This is a list of useful functions:
#   DevWarningWindow message        A pop-up Warning Window
#   DevErrorWindow message          A pop-up Error   Window
#   DevFatalErrorWindow message     A pop-up Error Window and Exit
#   DevAddLabel LabelName message   Add a label to the GUI
#   DevAddButton ButtonName         Add a button to the GUI that calls a command.
#   DevAddSelectButton              Add a volume or model select button
#   DevSelect                       Called upon selection from a SelectButton.
#   DevCreateNewCopiedVolume        Create a New Volume, Copying an existing one's param
#proc DevGetFile                    Looks for a file, makes a pop-up window if necessary
#proc DevAddFileBrowse              Creates a File Browsing Frame
#
# Other Useful stuff:
#
# MainVolumesCopyData               Copy the image part of a volume.
# MainModelsDelete  idnum           Delete a model
# MainVolumesDelete idnum           Delete a volume
# YesNoPopup                        in tcl-main/Gui.tcl.
# DataAddTransform                  Add a transform to the Mrml Tree.
#
# Useful Variables
# $Mrml(dir)  The directory from which the slicer was run.
#
#-------------------------------------------------------------------------------
# .PROC DevYesNo
#
#  Ask the user a Yes/No question. Force the user to decide before continuing.
#  returns "yes" or "no" 
#
# .ARGS
#  str message The question to ask.
# .END
#-------------------------------------------------------------------------------
proc DevYesNo {message} {
   return [tk_messageBox -title Slicer -icon question -type yesno -message $message]
}

#-------------------------------------------------------------------------------
# .PROC DevOKCancel
#
#  Ask the user a Yes/No question. Force the user to decide before continuing.
#  returns "yes" or "no" 
#
# .ARGS
#  str message The message to give.
# .END
#-------------------------------------------------------------------------------
proc DevOKCancel {message} {
   return [tk_messageBox -title Slicer -icon question -type okcancel -message $message]
}


#-------------------------------------------------------------------------------
# .PROC DevWarningWindow
#
#  Report a Warning to the user. Force them to click OK to continue.
#
# .ARGS
#  str message The error message. Default: \"Unknown Warning\"
# .END
#-------------------------------------------------------------------------------
proc DevWarningWindow {{message "Unknown Warning"}} {
   tk_messageBox -title "Slicer" -icon warning -message $message
}

#-------------------------------------------------------------------------------
# .PROC DevInfoWindow
#
#  Report Information to the user. Force them to click OK to continue.
#
# .ARGS
#  str message The error message. Default: \"Unknown Warning\"
# .END
#-------------------------------------------------------------------------------
proc DevInfoWindow {message} {
   tk_messageBox -title "Slicer" -icon info -message $message -type ok
}

#-------------------------------------------------------------------------------
# .PROC DevErrorWindow
#
#  Report an Error to the user. Force them to click OK to continue.
#
# .ARGS
#  str message The error message. Default: \"Unknown Error\"
# .END
#-------------------------------------------------------------------------------
proc DevErrorWindow {{message "Unknown Error"}} {
   tk_messageBox -title Slicer -icon error -message $message -type ok
}

#-------------------------------------------------------------------------------
# .PROC DevFatalErrorWindow
#
#  Report an Error to the user and then exit.
#
# .ARGS
#  str message The error message. Default: \"Fatal Error\"
# .END
#-------------------------------------------------------------------------------
proc DevFatalErrorWindow {{message "Fatal Error"}} {
   ErrorWindow $message
   MainExitProgram
}

#-------------------------------------------------------------------------------
# .PROC DevAddLabel
#
#  Creates a label
#  Example:  DevAddLabel $f.lmylabel \"Have a nice day\"
#
# 
# .ARGS
#  str LabelName  Name of the button (i.e. $f.stuff.lmylabel)
#  str Message    The text on the label
#  str Color      Label color and attribs from Gui.tcl (BLA or WLA). Optional
# .END
#-------------------------------------------------------------------------------
proc DevAddLabel { LabelName Message {Color WLA}} {
    global Gui
    eval {label $LabelName -text $Message} $Gui($Color)
}

#-------------------------------------------------------------------------------
# .PROC DevAddEntry
# 
#
# Example: DevAddEntry View parallelScale $f.eParallelScale 
# Example: DevAddEntry View parallelScale $f.eParallelScale 20
# adds an entry corresponding to variable View(parallelScale)
# the first one has width 10, the second has width 20
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DevAddEntry { ArrayName Variable EntryName {Width 10}} {
    global Gui $ArrayName

    eval {entry $EntryName -textvariable "$ArrayName\($Variable\)" \
        -width $Width } $Gui(WEA)
}


#-------------------------------------------------------------------------------
# .PROC DevAddButton
#
#  Creates a button.
#  Example:  DevAddButton $f.bmybutton \"Run me\" \"DoStuff\" 10
#  Example:  DevAddButton $f.bmybutton \"Run me\" \"DoStuff\"
#  The first example creates a button of width 10 that says \"Run me\",
#  and Calls procedure \"DoStuff\" when pressed.
#  The second example does the same except it automatically determines 
#  the width of the button.
#
# .ARGS
#  str ButtonName Name of the button (i.e. f.stuff.bStuff)
#  str Message    The text on the button
#  str Command    The command to run
#  str Width      Optional Width. Default: width of the Message.
# .END
#-------------------------------------------------------------------------------
proc DevAddButton { ButtonName Message Command {Width 0} } {
    global Gui
    if {$Width == 0 } {
        set Width [expr [string length $Message] +2]
    }
    eval  {button $ButtonName -text $Message -width $Width \
            -command $Command } $Gui(WBA)
} 


#-------------------------------------------------------------------------------
# .PROC DevAddSelectButton
#
#  Add a Select Button to the GUI
#
#  Example: DevAddSelectButton MyModule $f Volume1 "Reference Volume" Grid
#    Creates a Volume select button with text "Reference Volume" to the left.
#      Grids the result.
#    Creates $f.lVolume1     : The Label
#    Creates $f.mbVolume1   : The Menubutton
#    Creates $f.mbVolume1.m : The Menu
#    Creates MyModule(mbVolume1) = $f.mbVolume1; This is for update
#       in MyModuleUpdateMrml 
#
#  Example2: DevAddSelectButton MyModule $f Model1 "Model Choice" Grid
#
# Note that we have not yet chosen the variable we are going to effect.
# Also, we need a procedure like DevUpdate to make the update.
#
# .ARGS
#  Array TabName  This is typically the name of the module.
#  Widget f       Frame the button should go on
#  Str   Label    This is the name of the button widget (i.e. MySelectVolumeButton)
#  Str  Message   The message label to put to the left of the Volume Select button. Default \"Select Volume\"
#  Str  Pack          "Pack" packs the buttons. \"Grid\" grids the buttons.
#  Str Tooltip    The tooltip to display over the button. Optional.
#  str width      The width to make the button. Optional
#  str color      Message label color and attribs from Gui.tcl (BLA or WLA). Optional
# .END
#-------------------------------------------------------------------------------
proc DevAddSelectButton { TabName f aLabel message pack {tooltip ""} \
    {width 13} {color WLA}} {

    global Gui Module 
    upvar 1 $TabName LocalArray

    # if the variable is not 1 procedure up, try 2 procedures up.

    if {0 == [info exists LocalArray]} {
        upvar 2 $TabName LocalArray 
    }

    if {0 == [info exists LocalArray]} {
        DevErrorWindow "Error finding $TabName in DevAddSelectButton"
        return
    }

    set Label       "$f.l$aLabel"
    set menubutton  "$f.mb$aLabel"
    set menu        "$f.mb$aLabel.m"
   
    # Kilian: Why should we create a label if we do not have a message
    if {$message != ""} {
      DevAddLabel $Label $message $color
    }

    eval {menubutton $menubutton -text "None" \
            -relief raised -bd 2 -width $width -menu $menu} $Gui(WMBA)
    eval {menu $menu} $Gui(WMA)

    if {$pack == "Pack"} {
    if {$message != ""} {pack $Label -side left -padx $Gui(pad) -pady 0} 
       pack $menubutton -side left -padx $Gui(pad) -pady 0 
    } else {
    if {$message != ""} { grid $Label -sticky e -padx $Gui(pad) -pady $Gui(pad)}
        grid $menubutton -sticky e -padx $Gui(pad) -pady $Gui(pad)
        grid $menubutton -sticky w
    }

    if {$tooltip != ""} {
    TooltipAdd $menubutton $tooltip
    }
    
    set LocalArray(mb$aLabel) $menubutton
    set LocalArray(m$aLabel) $menu

    # Note: for the automatic updating, we can use
    # lappend Model(mbActiveList) $f.mb$ModelLabel
    # lappend Model(mActiveList)  $f.mbActive.m
    # 
    # or we can use DevUpdateVolume in the MyModuleUpdate procedure

    # Note: for the automatic updating, we can use
    # lappend Volume(mbActiveList) $f.mb$VolLabel
    # lappend Volume(mActiveList)  $f.mbActive.m
    # 
    # or we can use DevUpdateVolume in the MyModuleUpdate procedure
}   

#-------------------------------------------------------------------------------
# .PROC DevUpdateNodeSelectButton
#
#  Call this routine from MyModuleUpdateDev or its eqivalent.
#  Example: DevUpdateSelectButton Volume MyModule Volume1 Volume1 DevSelect
#     Updates the menubutton Volume List for the button with label Volume1.
#     Updates the menubutton Face text  for the button with label Volume1.
#     Sets the Command to call to set the Volume to be DevSelect.
#
# Example2: DevUpdateSelectButton Model MyModule Model1 Model1 DevSelect
#
# Note that ArrayName(Name) must exist.
#
# .ARGS
#  array ArrayName The array name containing the Volume Choice. Usually the module name.
#  str type Either \"Model\" or \"Volume\".
#  str Label This is the label of the bottons.
#  str Name  The Volume or Model choice is stored in ArrayName(Name)
#  str CommandSet This is the command to run to set the volume or model name. The default is DevSetVolume.  Arguments sent to it are type, the volume id and then ArrayName VolumeLabel VolumeName. Note that if you decide to make your own SetVolume command which requires other arguments, you can do this by setting CommandSetVolume to \"YourCommand arg1 arg2\" You must be able to deal with a \"\" id.
#  bool None 1/0 means do/don't include the None NodeType. 1 is the defaulte
#  bool New 1/0 means do/don't include the New NodeType. 0 is the defaulte
#  bool LabelMap 1/0 means do/don't include LabelMaps. For Volumes Only. 0 is the defaulte
#
# .END
#-------------------------------------------------------------------------------
## 
## I left this code here as an example of how to update Volumes alone.
## The code is slightly less complicated then my implementation for
## both Volumes and Models.
##
#proc DevUpdateVolume {ArrayName VolumeLabel VolumeName { CommandSetVolume DevSetVolume} { None 1 }  { New 0 } { LabelMap 1 }  } {
#
#        global Volume 
#        upvar $ArrayName LocalArray
#
#        # See if the selected volume for each menu actually exists.
#        # If not, use the first volume in the list
#       if {[lsearch $Volume(idList) $LocalArray($VolumeName) ] == -1} {
#           $CommandSetVolume [lindex $Volume(idList) 0] $ArrayName $VolumeLabel $VolumeName
#        }
#
#        # Menu of Volumes
#        # ------------------------------------
#        set m $LocalArray(mb$VolumeLabel).m
#        $m delete 0 end
#        # All volumes except none
#        foreach v $Volume(idList) {
#            set test 1
#            # Show Volume(idNone)?
#            if {$None==0}      { set test [expr $v != $Volume(idNone)] }
#
#            # Show LabelMaps?
#            if {$LabelMap==0 && $test }  {
#                set test Expr[ [Volume($v,node) GetLabelMap] == 0]}
#
#            if $test {
#                $m add command -label "[Volume($v,node) GetName]" \
#                -command "$CommandSetVolume $v $ArrayName $VolumeLabel $VolumeName"
#                }
#        }
#        if {$New} {
#            $m add command -label "Create New" \
#           -command "$CommandSetVolume -5 $ArrayName $VolumeLabel $VolumeName"
#        }
#}   

#-------------------------------------------------------------------------------
# .PROC DevUpdateNodeSelectButton
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DevUpdateNodeSelectButton { type ArrayName Label Name { CommandSet "DevSelect" } { None 1 } { New 0 } { LabelMap 1 } {Command2Set ""} } {

    global Volume Model
        upvar $ArrayName LocalArray
        upvar 0 $type NodeType

    # See if the NodeType for each menu actually exists.
    # If not, use the None NodeType
    set v $NodeType(idNone)

    if {[lsearch $NodeType(idList) $LocalArray($Name) ] == -1} {
            $CommandSet $type [lindex $NodeType(idList) 0]  $ArrayName $Label $Name
    }

    # Menu of NodeTypes
    # ------------------------------------
    set m $LocalArray(mb$Label).m
    $m delete 0 end
    # All volumes except none

    foreach v $NodeType(idList) {
            set test 1
            # Show NodeType(idNone)?
            if {$None==0}      { set test [expr $v != $NodeType(idNone)] }

            # Show LabelMaps?
            if {($LabelMap==0) && ($type=="Volume") && $test }  {
                set test Expr[ [NodeType($v,node) GetLabelMap] == 0]
            }
            if $test {
                $m add command -label [${type}($v,node) GetName] \
            -command "$CommandSet $type $v $ArrayName $Label $Name; $Command2Set"
        }
    }
        if {$New} {
            $m add command -label "Create New" \
           -command "$CommandSet $type -5 $ArrayName $Label $Name"
        }
}


#-------------------------------------------------------------------------------
# .PROC DevUpdateSelectButton
#
# Updates a Simple Select Button.
# Note that ArrayName(Name) and ArrayName(ChoiceList) must exist.
#
# .ARGS
#  array ArrayName The array name containing the Volume Choice. Usually the module name.
#  str Label This is the label of the buttons.
#  str Name  The Current choice is stored in ArrayName(Name)
#  array ChoiceList The possible choices are ArrayName(ChoiceList)
#  str Command  The command to run. The default is no command. (Though, in both cases, the Button display is updated).  Arguments sent to it are the selected choice. Note that if the command requires other arguments, you can do this by setting Command to \"YourCommand arg1 arg2\" You should be able to deal with a \"\" selection. 
#
# .END
#-------------------------------------------------------------------------------
proc DevUpdateSelectButton { ArrayName Label Name ChoiceList {Command ""} } {

        upvar $ArrayName LocalArray

    # Delete all the current options and create the new ones
    # ------------------------------------
    set m $LocalArray(mb$Label).m
    $m delete 0 end

    foreach v $LocalArray($ChoiceList) {
            if {$Command != ""} {
                $m add command -label $v -command "$LocalArray(mb$Label) config -text $v; $Command $v"
            } else {
                $m add command -label $v -command "$LocalArray(mb$Label) config -text $v"
            }
        }
    }

#-------------------------------------------------------------------------------
# .PROC DevSelectNode
#
# Usually called when a Select button has been
# clicked on. Sets the text to put on the button as well as setting
# the variable to the volume id chosen.
# 
# .ARGS
# str type \"Volume\" or \"Model\"
# int id the id of the selected volume
# Array ArrayName The name of the array whose variables will be changed.
# str ModelLabel The name of the menubutton, without the \"mb\"
# str ModelName  The name of the variable to set.
# .END
#-------------------------------------------------------------------------------
proc DevSelectNode { type id ArrayName ModelLabel ModelName} {
    global Model Volume
    upvar $ArrayName LocalArray

    if {0 == [info exists LocalArray]} {
        upvar 2 $ArrayName LocalArray 
    }

    if {0 == [info exists LocalArray]} {
        DevErrorWindow "Error finding $TabName in DevAddSelectButton"
        return
    }
    if {$id == ""} {
            $LocalArray(mb$ModelLabel) config -text "None"
    } elseif {$id == -5} {
            set LocalArray($ModelName) $id
            $LocalArray(mb$ModelLabel) config -text \
                    "Create New"
        } else {
           set LocalArray($ModelName) $id
            $LocalArray(mb$ModelLabel) config -text \
                    "[${type}($id,node) GetName]"
    }
}

#-------------------------------------------------------------------------------
# .PROC DevCreateNewCopiedVolume
# 
# returns the idnumber of the new Volume.
# Note: does not copy the volume data. Use MainVolumesCopyData to do that.
#
# .ARGS
# int VolumeId  The id of the volume to copy.
# str Description The Description of the new Volume.Default:Copy VolumeId's Description.
# str VolName     The Name of the new Volume. Default: copy the VolumeId's Name.
# .END
#-------------------------------------------------------------------------------
proc DevCreateNewCopiedVolume { OrigId {Description ""} { VolName ""} } {
    global Volume Lut

    # Create the new node
    # newvol is now a vtkMrmlVolumeNode, a subclass of vtkMrmlNode. 
    # How about that?
    set newvol [MainMrmlAddNode Volume]

    # Copies all the important stuff of the vtkMrmlVolumeNode
    # Copy the node's attributes to this object: strings, numbers, vectors, matricies..
    # Does NOT copy: ID, FilePrefix, Name
    # This copy does not include the data. (Lauren is that right?)

    $newvol Copy Volume($OrigId,node)

#    # Let's say you want to create a new Volume and only copy the minimum amount of
#    # stuff. As far as I can tell, this is how to do it. Just get rid of the "Copy"
#    # line and use this stuff.
#
#    # Largely copied from EditorGetWorkingID on Feb 25,2000
#    # Also from  EditorCopyNode
#    #  by Samson Timoner
#
#    # Copies everything in the vtkMrmlNode except the id.
#    # As I write this, CopyNode only copies the description
#    # and a few "options" which aren't necessary. However,
#    # this function should be kept for future compatibility
#    # in case the function actually does something important
#    # someday.
#    # Note that vtkMrmlNode is part of vtkMrmlVolume. So,
#    # we have yet to update most of vtkMrmlVolume.
#
#    $newvol CopyNode Volume($OrigId,vol)
#
#    # Copy the lookup table from the given volume
#                                     
#    $newvol SetLUTName     $Lut($OrigId)
#    $newvol SetInterpolate [$Volume($OrigId,vol) GetInterpolate]
#    $newvol SetLabelMap    [$Volume($OrigId,vol) GetLabelMap]
#

    # Set the Description and Name
    if {$Description != ""} {
        $newvol SetDescription $Description 
    }
    if {$VolName != ""} {
        $newvol SetName $VolName
    }

    # Create the volume
    set n [$newvol GetID]
    MainVolumesCreate $n
#    Volume($n,vol) UseLabelIndirectLUTOn

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML

    return $n
}

#-------------------------------------------------------------------------------
# .PROC DevGetFile
# 
# If a filename exists, simply returns it.
# Otherwise pops up a window to find a filename.
# Default directory to start searching is the one Slicer was called from
#
# .ARGS
# str filename The name of the file entered so far
# int MustPop  1 means that we will pop up a window even if \"filename\" exists. 
# str DefaultExt The name of the extension for the type of file: Default \"\"
# str DefaultDir The name of the default directory to choose from: Default is the directory Slicer was started from.
# str Title      The title of the window to display.  Optional.
# str Action     Whether to Open (file must exist) or Save.  Default is \"Open\".
# .END
#-------------------------------------------------------------------------------
proc DevGetFile { filename { MustPop 0} { DefaultExt "" } { DefaultDir "" } {Title "Choose File"} {Action "Open"} {PathType "Relative"}} {
    global Mrml
#        puts "filename: $filename"
#        puts "DefaultExt $DefaultExt"
#        puts "DefaultDir $DefaultDir"
#        puts "Title $Title"

    # Default Directory Choice
    if {$DefaultDir == ""} {
            set DefaultDir $Mrml(dir);
    }

       ############################################################
       ######  Check if the filename exists
       ######  Check with/without DefaulExt, and with or without
       ######  Default dir.
       ######  Do this only if the filename is not "" and is not a dir.
       ############################################################

        if {$filename != "" && ![file isdir $filename] && !$MustPop} {
            if [file exists $filename]  {
                return [MainFileGetRelativePrefix $filename][file \
                        extension $filename]
            }
            if [file exists "$filename.$DefaultExt"] {
                return [MainFileGetRelativePrefix $filename].$DefaultExt
            }
            set filename [file join $DefaultDir $filename]
            if [file exists $filename]  {
                return [MainFileGetRelativePrefix $filename][file \
                        extension $filename]
            }
            if [file exists "$filename.$DefaultExt"] {
                return [MainFileGetRelativePrefix $filename].$DefaultExt
            }
        }

       ############################################################
       ######  Didn't find it, now set up filter for files
       ######  If an extension is provided, use it.
       ############################################################

        if { $DefaultExt != ""} {
            set typelist \
                    " \{\"$DefaultExt Files\" \{\*.$DefaultExt\}\} \{\"All Files\" \{\*\}\}"
#            set typelist [ eval $typelist ]
        } else {
            set typelist {
        {"All Files" {*}}
            }
        }

       ############################################################
       ######  Browse for the file
       ############################################################

        set dir [file dirname $filename]
        if { $filename == "" && $DefaultDir != "" } { set dir $DefaultDir }
        if { [file isdir $filename] } { set dir $filename }

    # if we are saving, the file doesn't have to exist yet.
    if {$Action == "Save"} {
        set filename [tk_getSaveFile -title $Title \
            -filetypes $typelist -initialdir "$dir" -initialfile $filename]
    } else {
        set filename [tk_getOpenFile -title $Title \
            -filetypes $typelist -initialdir "$dir" -initialfile $filename]
    }
    

       ############################################################
       ######  Return Nothing is nothing was selected
       ######  Return the file relative to the current path otherwise
       ############################################################

    # Return nothing if the user cancelled
    if {$filename == ""} {return "" }

    # if the file will be Saved (not Opened) make sure it has an extension
    if {$Action == "Save"} {
        if {[file extension $filename] == ""} {
        set filename "$filename.$DefaultExt"
        }   
    }
    
    # If the file is not to be stored relative to the Mrml dir.
    if {$PathType == "Absolute"} {
        return $filename
    }
        
    # Store first image file as a relative filename to the root 
    # Return the relative Directory Path
    return [MainFileGetRelativePrefix $filename][file \
        extension $filename]
    }   

#-------------------------------------------------------------------------------
# .PROC DevAddFileBrowse
#
# Calls DevGetFile, so defaults for Optional Arguments are set there.
# ArrayName(VarFileName) must exist already!
# 
# Make a typical button for browsing for files
#  Example:  DevAddFileBrowse $f.fPrefix Custom Prefix \"File\"
#  Example:  DevAddFileBrowse $f.fPrefix Custom Prefix \"vtk File\" \"vtk\" \"\" \"Browse for a model\"
#  Example: DevAddFileBrowse $f Volume firstFile "First Image File:" "VolumesSetFirst" "" "\$Volume(DefaultDir)"  "Browse for the first Image file" #
#
# In the last example, the trick using "\$Volume(DefaultDir)" allows you
# to change the default directory later.
#
# Calls DevGetFile, so defaults for Optional Arguments are set there.
# ArrayName(VarFileName) must exist already!
#
# .ARGS
# str Frame      The name of the existing frame to modify.
# Array ArrayName The name of the array whose variables will be changed.
# str VarFileName The name of the file name variable within the array.
# str Message     The message to display near the "Browse" button.
# str Command     A command to run when a file name is entered AND the file entered exists (unless Action is Save, when the file need not exist yet). 
# str DefaultExt The name of the extension for the type of file. Optional
# str DefaultDir The name of the default directory to choose from. Optional
# str Action     Whether this is \"Open\" or \"Save\".  Optional
# str Title      The title of the window to display. Optional
# str Tooltip    The tooltip to display over the button. Optional
# str PathType   Default is filename is relative to Mrml(dir).  Use "Absolute" for absolute pathnames
# .END
#-------------------------------------------------------------------------------

    proc DevAddFileBrowse {Frame ArrayName VarFileName Message { Command ""} { DefaultExt "" } { DefaultDir "" } {Action ""} {Title ""} {Tooltip ""} {PathType ""}} {

    global Gui $ArrayName Model

        set f $Frame
        $f configure  -relief groove -bd 3 -bg $Gui(activeWorkspace)

    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.f -side top -padx $Gui(pad) -pady $Gui(pad)

        ## Need to make the string that will become the command.
    # this pops up file browser when the button is pressed.
       set SetVarString  "set $ArrayName\($VarFileName\) \[ DevGetFile \"\$$ArrayName\($VarFileName\)\" 1  \"$DefaultExt\" \"$DefaultDir\" \"$Title\"  \"$Action\" \"$PathType\" \]; if \{\[DevFileExists \$$ArrayName\($VarFileName\)\] || \"$Action\" == \"Save\"\}  \{ $Command \}"
#$Action == Save
#        puts $SetVarString

        DevAddLabel  $f.f.l $Message
        DevAddButton $f.f.b "Browse..." $SetVarString

        pack $f.f.l $f.f.b -side left -padx $Gui(pad)
    
    # tooltip over the button.
    if {$Tooltip != ""} {
        TooltipAdd $f.f.b $Tooltip
    }

    # this pops up file browser when return is hit.
    set SetVarString  "set $ArrayName\($VarFileName\) \[ DevGetFile \"\$$ArrayName\($VarFileName\)\" 0  \"$DefaultExt\" \"$DefaultDir\" \"$Title\" \"$Action\" \"$PathType\" \]; if \{\[DevFileExists \$$ArrayName\($VarFileName\)\] || \"$Action\" == \"Save\"\}  \{ $Command \}"

    eval {entry $f.efile -textvariable "$ArrayName\($VarFileName\)" -width 50} $Gui(WEA)
        bind $f.efile <Return> $SetVarString

        pack $f.efile -side top -pady $Gui(pad) -padx $Gui(pad) \
                -expand 1 -fill x
}


#-------------------------------------------------------------------------------
# .PROC DevCreateScrollList
#
# Creates a Scrolled List. The programmer can pass a procedure on how
# to create each line in the list. 
# Note: Checks if the list already exists and deletes it if it does.
#
# Creates $ScrollFrame.cGrid which is the canvas.
# Creates $ScrollFrame.cGrid.fListItems which is the item frame
#
# Example Usage : See Models.tcl
#   frame $f.fScroll -bg $Gui(activeWorkspace)
#   pack  .... $f.fScroll -side top -pady 1
#
#   DevCreateScrollList $Module(Models,fDisplay).fScroll MainModelsCreateGUI \
#                       ModelsConfigScrolledGUI "$Model(idList)"
#
#
# 
# .ARGS
#   frame ScrollFrame
#   func  ItemCreateGui 2 args: the frame for the Item list and the item. 
#   func  ScrollListConfigScrolledGUI: 2 args: canvas and item frame.
#   func  ItemList list of items
# .END
#-------------------------------------------------------------------------------
proc DevCreateScrollList {ScrollFrame ItemCreateGui ScrollListConfigScrolledGUI ItemList} {
    global Mrml Gui Module

    #################################
    # Delete everything from last time, if there was a last time.
    #################################
    set f $ScrollFrame
    set canvas $f.cGrid
    catch {destroy $canvas}
    set sy $f.syGrid
    set sx $f.sxGrid
    catch {destroy $sy}
    catch {destroy $sx}

    #################################
    # Create the new canvas
    #################################
    canvas $canvas -yscrollcommand "$sy set" \
                   -xscrollcommand "$sx set" -bg $Gui(activeWorkspace)
    eval "scrollbar $sy -command \"DevCheckScrollLimits $canvas yview\" \
            $Gui(WSBA)"
    eval "scrollbar $sx -command \"$canvas xview\" \
            -orient horizontal $Gui(WSBA)"

    pack $sy -side right -fill y
    pack $sx -side bottom -fill x
    pack $canvas -side top  -fill both -expand true

    set f $canvas.fListItems
    frame $f -bd 0 -bg $Gui(activeWorkspace)
    
    # put the frame inside the canvas (so it can scroll)
    $canvas create window 0 0 -anchor nw -window $f

    foreach m $ItemList {
        $ItemCreateGui $f $m
    }

    $ScrollListConfigScrolledGUI $canvas $f
}

#-------------------------------------------------------------------------------
# .PROC DevCheckScrollLimits 
# 
# This procedure allows scrolling only if the entire frame is not visible
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC DevCheckScrollLimits
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DevCheckScrollLimits {args} {

    set canvas [lindex $args 0]
    set view   [lindex $args 1]
    set fracs [$canvas $view]

    if {double([lindex $fracs 0]) == 0.0 && \
        double([lindex $fracs 1]) == 1.0} {
    return
    }
    eval $args
}


#-------------------------------------------------------------------------------
# .PROC DevFileExists
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DevFileExists {filename} {
    global Mrml

    # returns 1 if file exists, either relative to mrml directory or not

    if {[file exists $filename]} {
    return 1
    }

    if {[file exists [file join $Mrml(dir) $filename]]} {
    return 1
    }

    return 0
}


#-------------------------------------------------------------------------------
# .PROC DevSourceTclFilesInDirectory
# Source all tcl files found in directory dir.  Returns a list of
# the files (without the leading path or file extension).
# .ARGS
# path dir location of the files
# int verbose optional, defaults to 0, whether to puts the filenames
# .END
#-------------------------------------------------------------------------------
proc DevSourceTclFilesInDirectory {dir {verbose "0"}} {

    # from Go.tcl.  Looks locally and centrally.
    set found [FindNames $dir]
    if {$verbose == 1} {puts $found}

    set sourced ""

    # If it's a tcl file source it and save its name on a list
    foreach name $found {
        # from Go.tcl.  Finds local or central full path of tcl file
        set path [GetFullPath $name tcl $dir]
        # If a tcl file exists source it and save name
        if {$path != ""} {
            if {$verbose == 1} {puts "source $path"}
            source $path
            lappend sourced $name
        } 
    }

    # return the list of sourced files
    return $sourced
}
