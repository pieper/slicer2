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
# FILE:        GUI.tcl
# PROCEDURES:  
#   GUIWarningWindow message
#   GUIErrorWindow message
#   GUIFatalErrorWindow message
#   GUIAddButton ButtonName Message Command Width
#   GUIAddSelectButton TabName Label Message Pack type
#   GUIAddVolumeSelectButton TabName VolLabel Message Pack
#   GUIAddModelSelectButton TabName VolLabel Message Pack
#   GUIUpdateVolume ArrayName VolumeLabel VolumeName CommandSetVolume
#   GUISetVolume id ArrayName VolumeLabel VolumeName
#   GUICreateNewCopiedVolume VolumeId Description VolName
#==========================================================================auto=
# This file exists specifically for user to help fast development
# of Slicer modules
#
# This is a list of useful functions:
#   GUIWarningWindow message        A pop-up Warning Window
#   GUIErrorWindow message          A pop-up Error   Window
#   GUIFatalErrorWindow message     A pop-up Error Window and Exit
#   GUIAddButton ButtonName         Add a button to the GUI that calls a command.
#   GUIAddSelectButton TabName        
#   GUIAddVolumeSelectButton        Add a volume selection button.
#   GUIAddModelSelectButton         Add a model selection button.
#   GUIUpdateVolume                 Update the volume list in a button.
#   GUISetVolume                    Set a var based on the selected button
#   GUICreateNewCopiedVolume        Create a New Volume, Copying an existing one's param
# MainVolumesCopyData               Copy the image part of a volume.
# MainModelsDelete  idnum           Delete a model
# MainVolumesDelete idnum           Delete a volume
#
#
#-------------------------------------------------------------------------------
# .PROC GUIWarningWindow
#
#  Report a Warning to the user. Force them to click OK to continue.
#
# .ARGS
#  str message The error message. Default: \"Unknown Warning\"
# .END
#-------------------------------------------------------------------------------
proc GUIWarningWindow {{message "Unknown Warning"}} {
   tk_messageBox -message $message
}

#-------------------------------------------------------------------------------
# .PROC GUIErrorWindow
#
#  Report an Error to the user. Force them to click OK to continue.
#
# .ARGS
#  str message The error message. Default: \"Unknown Error\"
# .END
#-------------------------------------------------------------------------------
proc GUIErrorWindow {{message "Unknown Error"}} {
   tk_messageBox -message $message
}

#-------------------------------------------------------------------------------
# .PROC GUIFatalErrorWindow
#
#  Report an Error to the user and then exit.
#
# .ARGS
#  str message The error message. Default: \"Fatal Error\"
# .END
#-------------------------------------------------------------------------------
proc GUIFatalErrorWindow {{message "Fatal Error"}} {
   ErrorWindow $message
   MainExitProgram
}


#-------------------------------------------------------------------------------
# .PROC GUIAddButton
#
#  Creates a button.
#  Example:  GUIAddButton $f.bmybutton \"Run me\" \"DoStuff\" 10
#  Example:  GUIAddButton $f.bmybutton \"Run me\" \"DoStuff\"
#  The first example creates a button of width 10 that says \"Run me\",
#  and Calls procedure \"DoStuff\" when pressed.
#  The second example does the same except it automatically determines 
#  the width of the button.
#
# .ARGS
#  str ButtonName Name of the button
#  str Message    The text on the button
#  str Command    The command to run
#  str Width      Optional Width. Default: width of the Message.
# .END
#-------------------------------------------------------------------------------
proc GUIAddbutton { ButtonName Message Command {Width 0} } {
	global Gui
    if {$Width == 0 } {
        set Width [ expr [string length $Width] * 3]
    }
    eval  {button $ButtonName -text $Message -width $Width \
            -command $Command } $Gui(WBA)
}



#-------------------------------------------------------------------------------
# .PROC GUIAddSelectButton
#
#  Called by GUIAddVolumeSelectButton and GUIAddModelSelectButton
#
# .ARGS
#  Array TabName  This is typically the name of the module.
#  Str   Label    This is the name to label the button.
#  Str  Message   The message to put to the left of the Select Button.
#  Str  Pack    \"Pack\" packs the buttons. \"Grid\" grids the buttons.
#  Str  type    \"Model\" handles models. \"Volume\" handles volumes.
# .END
#-------------------------------------------------------------------------------
proc GUIAddSelectButton { TabName f aLabel message pack type } {
	global Gui Module 
        upvar $TabName LocalTab 

    set Label       "$f.l$aLabel"
    set menubutton  "$f.mb$aLabel"
    set menu        "$f.mb$aLabel.m"

    eval {label $Label -text $message} $Gui(WLA)
    eval {menubutton $menubutton -text "None" \
            -relief raised -bd 2 -width 13 -menu $menu } $Gui(WMBA)
    eval {menu $menu} $Gui(WMA)

    if {$pack == "Pack"} {
	pack $Label $menubutton -side left -padx $Gui(pad) -pady 0 
    } else {
        grid $Label $menubutton -sticky e -padx $Gui(pad) -pady $Gui(pad)
        grid $menubutton -sticky w
    }

    # This doesn't work for some reason!
    set LocalTab(mb$aLabel) $menubutton
}   


#-------------------------------------------------------------------------------
# .PROC GUIAddVolumeSelectButton
#
#  Add a Volume Button to the GUI
#
#  Example: GUIAddVolumeSelectButton MyModule $f Volume1 "Reference Volume" Grid
#    Creates a Volume select button with text "Reference Volume" to the left.
#      Grids the result.
#    Creates $f.lVolume1     : The Label
#    Creates $f.mbVolume1   : The Menubutton
#    Creates $f.mbVolume1.m : The Menu
#    Creates MyModule(mbVolume1) = $f.mbVolume1; This is for update
#       in MyModuleUpdateMrml 
#
# .ARGS
#  Array TabName  This is typically the name of the module.
#  Str   VolLabel This is the name to label the button.
#  Str  Message The message to put to the left of the Volume Select. Default \"Select Volume\"
#  Str  "Pack" packs the buttons. \"Grid\" grids the buttons.
# .END
#-------------------------------------------------------------------------------
proc GUIAddVolumeSelectButton { TabName f VolLabel {Message "Select Volume"} {pack 1} } {
	global Gui Module
        upvar $TabName LocalTab

    GUIAddSelectButton $TabName $f $VolLabel $Message $pack Volume

    set Label       "$f.l$VolLabel"
    set menubutton  "$f.mb$VolLabel"
    set menu        "$f.mb$VolLabel.m"

    # Append widgets to list that gets refreshed during UpdateMRML
    # I don't know why I can't get this to work in GUIAddSelectButton
     set LocalTab(mb$VolLabel) $menubutton

}


#-------------------------------------------------------------------------------
# .PROC GUIAddModelSelectButton
#
#  Add a Model Button to the GUI
#
#  Example: AddModelSelectButton MyModule $f Model1 "Reference Model" Grid
#    Creates a Model select button with text "Reference Model" to the left.
#      Grids the result.
#    Creates $f.lModel1    : The Label
#    Creates $f.mbModel1   : The Menubutton
#    Creates $f.mbModel1.m : The Menu
#    Creates MyModule(mbModel1) = $f.mbModel1; This is for update
#       in MyModuleUpdateMrml 
#
# .ARGS
#  Array TabName  This is typically the name of the module.
#  Str   VolLabel This is the name to label the button.
#  Str  Message The message to put to the left of the Model Select. Default "Select Model:"
#  Str  Pack \"Pack\" packs the buttons. \"Grid\" grids the buttons.
# .END
#-------------------------------------------------------------------------------
proc GUIAddModelSelectButton { TabName f ModelLabel {Message "Select Model:"} {pack 1 } } {
	global Gui Module
        upvar $TabName LocalTab

    set Label       "$f.l$ModelLabel"
    set menubutton  "$f.mb$ModelLabel"
    set menu        "$f.mb$ModelLabel.m"

    GUIAddSelectButton TabName $f $ModelLabel $Message $pack Model

    # Append widgets to list that gets refreshed during UpdateMRML
    # I don't know why I can't get this to work in GUIAddSelectButton
     set LocalTab(mb$ModelLabel) $menubutton
}

#-------------------------------------------------------------------------------
# .PROC GUIUpdateVolume
#
#  Call this routine from MyModuleUpdateGUI or its eqivalent.
#  Example: GUIUpdateVolume MyModule Volume1 Volume1 GUISetVolume
#     Updates the menubutton Volume List for the button with label Volume1.
#     Updates the menubutton Face text  for the button with label Volume1.
#     Sets the Command to call to set the Volume to be GUISetVolume
#
#
#  Known Bug: If you delete a model, line 11 complains bitterly for a reason
#  I don't understand. That is, the commandsetvolume.
#
# .ARGS
#  array ArrayName The array name containing the Volume Choice. Usually the module name.
#  str VolumeLabel This is the label of the bottons.
#  str VolumeName  The Volume choice is stored in ArrayName(VolumeName)
#  str CommandSetVolume This is the command to run to set the volume name. The default is GUISetVolume.  Arguments sent to it are the volume id followed by: ArrayName VolumeLabel VolumeName. Note that if you decide to make your own SetVolume command which requires other arguments, you can do this by setting CommandSetVolume to \"YourCommand arg1 arg2\"
#
# .END
#-------------------------------------------------------------------------------
proc GUIUpdateVolume {ArrayName VolumeLabel VolumeName { CommandSetVolume GUISetVolume}} {
    # It is practically the same for Models, I'll have to update this soon.

	global Volume Model
        upvar $ArrayName LocalArray

	# See if the volume for each menu actually exists.
	# If not, use the None volume
	set v $Volume(idNone)
	if {[lsearch $Volume(idList) $LocalArray($VolumeName) ] == -1} {
             $CommandSetVolume $v $ArrayName $VolumeLabel $VolumeName
	}

        puts "got here in updatevolume"

	# Menu of Volumes
	# ------------------------------------
	set m $LocalArray(mb$VolumeLabel).m
	$m delete 0 end
	# All volumes except none
	foreach v $Volume(idList) {
            # Let's be inclusive of all volumes for the time being.
#		if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
			$m add command -label "[Volume($v,node) GetName]" \
           -command "$CommandSetVolume $v $ArrayName $VolumeLabel $VolumeName"
#		}
	}
}

#-------------------------------------------------------------------------------
# .PROC GUISetVolume
#
# Usually called when a VolumeSelect button has been
# clicked on. Sets the text to put on the button as well as setting
# the variable to the volume id chosen.
# 
# .ARGS
# int id the id of the selected volume
# Array ArrayName The name of the array whose variables will be changed.
# str VolumeLabel The name of the menubutton, without the \"mb\".
# str VolumeName  The name of the variable to set.
# .END
#-------------------------------------------------------------------------------
proc GUISetVolume { id ArrayName VolumeLabel VolumeName} {
	global Volume 
        upvar $ArrayName LocalModule

    puts "got here in guisetvolume: $id"

	if {$id == ""} {
		set id $LocalModule($VolumeName)
	} else {
		set LocalModule($VolumeName) $id
	}

	$LocalModule(mb$VolumeLabel) config -text "[Volume($id,node) GetName]"
}

#-------------------------------------------------------------------------------
# .PROC GUICreateNewCopiedVolume
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
proc GUICreateNewCopiedVolume { OrigId {Description ""} { VolName ""} } {
	global Volume Lut

    # Create the new node
    # newvol is now a vtkMrmlVolumeNode, a subclass of vtkMrmlNode. 
    # How about that?
    set newvol [MainMrmlAddNode Volume]

    # Copies all the important stuff of the vtkMrmlVolumeNode
    # Copy the node's attributes to this object: strings, numbers, vectors, matricies..
    # Does NOT copy: ID, FilePrefix, Name
    # This copy does not include the data. (Lauren is that right?)

    $newvol Copy Volume($OrigId,vol)

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
    MainVolumesCreate $n
#    Volume($n,vol) UseLabelIndirectLUTOn

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML

    return [$newvol GetID]
}
