#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        Ibrowser.tcl
# PROCEDURES:  
#   IbrowserInit
#   IbrowserInit
#   IbrowserBuildGUI
#   IbrowserEnter
#   IbrowserExit
#   IbrowserPushBindings
#   IbrowserPopBindings
#   IbrowserSetDirectory
#   IbrowserGetIntervalNameFromID
#   IbrowserGetIntervalIDFromName
#   IbrowserBuildVTK
#   IbrowserValidateName
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC IbrowserInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#  LANGUAGE: 'Intervals' are containers for 'Sequences'. 'Sequences' are
#  composed of individual 'Drops'. 'Drops' are image, volume, model,
#  data, command, note, or event data objects. A 'Study' is a collection of
#  intervals.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC IbrowserInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserInit {} {
    global Ibrowser Module Volume Model IbrowserController
    global PACKAGE_DIR_VTKIbrowser 

    set m Ibrowser

    #---For now, spew heavily.
    set Module(verbose) 0
    
    #---Module summary info
    set Module($m,overview) "GUI-controller and framework for manipulating sequences of image data."
    set Module($m,author) "Wendy Plesniak, SPL & HCNR, wjp@bwh.harvard.edu"

    #---Define tabs
    set Module($m,row1List) "Help New Display Process Save"
    set Module($m,row1Name) "{help} {new} {display} {process} {save}"
    set Module($m,row1,tab) New

    #---Procedure definitions
    set Module($m,procGUI) IbrowserBuildGUI
    set Module($m,procEnter) IbrowserEnter
    set Module($m,procExit) IbrowserExit    

    #---Dependencies
    set Module($m,depend) ""

    #---Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1 $} {$Date: 2004/07/22 21:11:03 $}]

    #---Initialize module-level variables
    #---Global array with the same name as the module. Ibrowser()
    #---contains just the highest-level framework. IbrowserController(Info,Ival,xx)
    #---and ${intervalName}() contain much more elaborate state about
    #---intervals, both as a group and individually, respectively.

    #--- Just some default values to start.
    set Ibrowser(dir) ""
    set Ibrowser(seqName) ""
    set Ibrowser(numSequences) 0
    set Ibrowser(uniqueNum) 0
    set Ibrowser(ViewDrop) 0
    set Ibrowser(MaxDrops) 1
    set Ibrowser(0,firstMRMLid) 0
    set Ibrowser(0,lastMRMLid) 0
    set Ibrowser(0,0,MRMLid) 0
    set Ibrowser(idNone) 0
    set Ibrowser(activeInterval) 0
    set Ibrowser(FGInterval) $Ibrowser(idNone)
    set Ibrowser(BGInterval) $Ibrowser(idNone)
    #--- Here, 20.0 is chosen to be the number of units wide to make
    #--- the none interval. This arbitrary number makes certain the
    #--- subsequent populated intervals don't have overlapping
    #--- volume icons in them.
    set Ibrowser(initIntervalWid) 20.0
    set Ibrowser(MaxDrops) 0
    set Ibrowser(opacity) 1.0
    
    #--- Animation global variables.
    set Ibrowser(AnimationInterrupt) 0
    set Ibrowser(AnimationFrameDelay) 50
    set Ibrowser(AnimationDirection) 1
    set Ibrowser(currFrametag) "curr_frame_textbox"

    #--- Zooming references
    set IbrowserController(zoomfactor) 0
    #--- Location of popup windows
    set IbrowserController(popupX) 375
    set IbrowserController(popupY) 753
    #--- progress indicator
    set IbrowserController(ProgressBarTxt) ""
    
    #--- This variable contains the module path plus some stuff
    #--- trim off the extra stuff, and add on the path to tcl files.
    set tmpstr $PACKAGE_DIR_VTKIbrowser
    set tmpstr [string trimright $tmpstr "/vtkIbrowser" ]
    set tmpstr [string trimright $tmpstr "/Tcl" ]
    set tmpstr [string trimright $tmpstr "Wrapping" ]
    set modulePath [format "%s%s" $tmpstr "tcl/"]
    set Ibrowser(modulePath) $modulePath
    

    #--- Source all appropriate tcl files here....
    #--- These contain broken out 
    #--- code for the Slicer GUI
    source ${modulePath}IbrowserLoadGUI.tcl
    source ${modulePath}IbrowserDisplayGUI.tcl
    source ${modulePath}IbrowserProcessGUI.tcl
    source ${modulePath}IbrowserSaveGUI.tcl
    source ${modulePath}IbrowserHelpGUI.tcl    

    #--- These contain extra procs for
    #--- IO / processing / visualization
    source ${modulePath}IbrowserBVolReader.tcl
    source ${modulePath}/IbrowserProcessing/IbrowserReorient.tcl
    source ${modulePath}/IbrowserProcessing/IbrowserMotionCorrect.tcl
    source ${modulePath}/IbrowserProcessing/IbrowserSmooth.tcl
    
    #--- These contain tcl code for the interval controller
    #--- which is launched in proc IbrowserEnter().
    source ${modulePath}IbrowserControllerMain.tcl
    source ${modulePath}IbrowserControllerAnimation.tcl
    source ${modulePath}IbrowserControllerGUIbase.tcl
    source ${modulePath}IbrowserControllerArrayList.tcl
    source ${modulePath}IbrowserControllerIcons.tcl
    source ${modulePath}IbrowserControllerIntervals.tcl
    source ${modulePath}IbrowserControllerCanvas.tcl
    source ${modulePath}IbrowserControllerUtils.tcl
    source ${modulePath}IbrowserControllerSlider.tcl
    source ${modulePath}IbrowserControllerDrops.tcl
    source ${modulePath}IbrowserControllerProgressBar.tcl

    #--- Create a new Interval Collection
    #--- set its id, its number of intervals to 0
    #--- set its name to "Collection_0"
    #--- set the number of intervals it contains.
    
    set Ibrowser(numIcollections) 1
    set Ibrowser(IcollectionID) 0
    set i $Ibrowser(IcollectionID)
    #vtkIntervalCollection Ibrowser($i,Icollection)
    #Ibrowser($i,Icollection) SetCollectionID $i
    #Ibrowser($i,Icollection) SetName "Collection_0"
    #Ibrowser($i,Icollection) SetnumIntervals 0

    set ::VolumeGroupCollection(numCollections) 0
}



#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#--- Builds the external control panel
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC IbrowserBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildGUI {} {

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Load
    #     VolumeOrModel
    #     ChooseReader
    #     SetReadInfo
    #     ApplyReader
    # Process
    #     Top
    #     Bottom
    # Display
    #     Top
    #     Bottom
    # Save
    #     Top
    #     Bottom
    #-------------------------------------------

    IbrowserBuildHelpFrame
    IbrowserBuildLoadFrame
    IbrowserBuildProcessFrame
    IbrowserBuildDisplayFrame
    IbrowserBuildSaveFrame
}




#-------------------------------------------------------------------------------
# .PROC IbrowserEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserEnter {{toplevelName .controllerGUI} } {
# Called when this module is entered by a user. 

    #pushEventManager $Ibrowser(eventManager)
    #--- Create or Raise the Ibrowser Controller
    #--- and push all event bindings onto the stack.
    IbrowserControllerLaunch

    
#    IbrowserPushBindings
    set ::IbrowserController(topLevel) $toplevelName

    #--- If you want to iconify controller; 
    if { 0 } {
        if {[winfo exists $toplevelName]} {
            lower $toplevelName
            wm iconify $toplevelName
        }
    }

}



#-------------------------------------------------------------------------------
# .PROC IbrowserExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserExit {{toplevelName .controllerGUI}} {
# Called when this module is exited by a user. 

    # popEventManager
 #   IbrowserPopBindings
    #--- Lower and iconify the Ibrowser Controller
    if {[winfo exists $toplevelName]} {
        lower $toplevelName
        wm iconify $toplevelName
    }
}



#-------------------------------------------------------------------------------
# .PROC IbrowserPushBindings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPushBindings { } {
    #push onto the even stack a new event manager that
    #deals with events when the Ibrowser module is active
    global Ev Csys
    EvActivateBindingSet Slice0Events
    EvActivateBindingSet Slice1Events
    EvActivateBindingSet Slice2Events
    
}




#-------------------------------------------------------------------------------
# .PROC IbrowserPopBindings
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserPopBindings { } {
    #remove bindings when Ibrowser module is inactive
    global Ev Csys
    EvDeactivateBindingSet Slice0Events
    EvDeactivateBindingSet Slice1Events
    EvDeactivateBindingSet Slice2Events

}




#-------------------------------------------------------------------------------
# .PROC IbrowserSetDirectory
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserSetDirectory { } {
    if { $dir == ""} {
        set dir [tk_chooseDirectory]
    }
     if { ![file isdirectory $dir/deformed_template] } {
        DevErrorWindow "$dir doesn't appear to be an FMRI directory"
        return
    }
    set ::Ibrowser(dir) $dir
}





#-------------------------------------------------------------------------------
# .PROC IbrowserGetIntervalNameFromID
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGetIntervalNameFromID { num } {

    set iname $::Ibrowser($num,name)
    return $iname
}




#-------------------------------------------------------------------------------
# .PROC IbrowserGetIntervalIDFromName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserGetIntervalIDFromName { name } {

    set inum $::Ibrowser($name,intervalID)
    return $inum
}





#-------------------------------------------------------------------------------
# .PROC IbrowserBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildVTK {} {
}





#-------------------------------------------------------------------------------
# .PROC IbrowserValidateName
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserValidateName { } {

    #--- make sure the name is unique, and that it
    #--- contains no unrecognized characters
    if { [ValidateName $::Ibrowser(seqName)] == 0}{
        set $::Ibrowser(seqName) ""
        set m1 "Specify a unique basename for the sequence using"
        set m2 "only letters, digits, dashes and/or underscores."
        tk_messageBox -message "$m1 $m2"
        return
    }
}


