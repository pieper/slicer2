#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationInit {} {
    global RigidIntensityRegistration Module Volume Model

    set m RigidIntensityRegistration

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is an example of how to add modules to slicer."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Samson Timoner MIT AI Lab"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Help Stuff"
    set Module($m,row1Name) "{Help} {Tons o' Stuff}"
    set Module($m,row1,tab) Stuff

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) RigidIntensityRegistrationBuildVTK
    #
    #   All the options are:

    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
#    set Module($m,procGUI) RigidIntensityRegistrationBuildGUI
#    set Module($m,procVTK) RigidIntensityRegistrationBuildVTK
    set Module($m,procEnter) RigidIntensityRegistrationEnter
    set Module($m,procExit) RigidIntensityRegistrationExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.2 $} {$Date: 2003/12/08 03:53:20 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set RigidIntensityRegistration(count) 0
    set RigidIntensityRegistration(Volume1) $Volume(idNone)
    set RigidIntensityRegistration(Model1)  $Model(idNone)
    set RigidIntensityRegistration(FileName)  ""

}

# NAMING CONVENTION:
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
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationBuildSubGUI
#
# Build the sub-gui under $f whatever frame is calling this one
#
# Example Useg: RigidIntensityRegistrationBuildSubGui $f.fIntensity
#
# .ARGS
# frame framename
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationBuildSubGui {f} {
    global Gui RigidIntensityRegistration Module Volume Model

    set framename $f

    # The select and Level Frames
    frame $f.fSelect -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fChoice  -bg $Gui(activeWorkspace) -height 500
    pack $f.fSelect $f.fChoice -side top -fill x
    #-------------------------------------------
    # Select frame
    #-------------------------------------------

    set f $framename.fSelect

    # Build pulldown menu for volume properties
    eval {label $f.l -text "Intensity Based Method:"} $Gui(BLA)
    frame $f.f -bg $Gui(backdrop)
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    set RigidIntensityRegistration(RegType) MI

    eval {menubutton $f.mbType -text $RigidIntensityRegistration(RegType) \
            -relief raised -bd 2 -width 5 \
            -menu $f.mbType.m} $Gui(WMBA) 
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach RegType "MI KL" {
        $f.mbType.m add command -label $RegType \
                -command "RigidIntensityRegistrationSetRegType $RegType"
    }
    # save menubutton for config
    set Volume(gui,mbPropertyType) $f.mbType
    # put a tooltip over the menu
    TooltipAdd $f.mbType \
            "Choose the type of Registration Algorithm. Choose MI unless you know what you are doing."

    #-------------------------------------------
    # Choice frame
    #-------------------------------------------

    set f $framename.fChoice
    #
    # Swappable Frames for MI/KL methods
    #
    foreach type "MI KL" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set RigidIntensityRegistration(f${type}) $f.f${type}
    }
    raise $RigidIntensityRegistration(fMI)

    MutualInformationRegistrationBuildSubGui $f.fMI
    KullbackLeiblerRegistrationBuildSubGui $f.fKL
    set fKL $f.fKL
}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationSetRegType
#   raise the appropriate GUI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationSetRegType { RegType} {
 global RigidIntensityRegistration

  raise $RigidIntensityRegistration(f$RegType)
}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationBuildVTK {} {

}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc RigidIntensityRegistrationEnter {} {
    global RigidIntensityRegistration
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $RigidIntensityRegistration(eventManager)

    # clear the text box and put instructions there
    $RigidIntensityRegistration(textBox) delete 1.0 end
    $RigidIntensityRegistration(textBox) insert end "Shift-Click anywhere!\n"

}


#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}


#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationCheckSetUp
#
# Make sure the volumes and the transform are OK.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationCheckSetUp {} {
  global Matrix Volume
    ###
    ### Check for Errors
    ###
    if {$Matrix(volume) == $Volume(idNone)} {
        DevWarningWindow "The Volume to Move is None! Please choose one."
        return 0
    }

    if {$Matrix(refVolume) == $Volume(idNone)} {
        DevWarningWindow "The Reference Volume is None! Please choose one."
        return 0
    }

    #
    # Store which transform we're editing
    # If the user has not selected a tranform, then create a new one by default
    # and append it to the volume to register (ie. "Volume to Move")
    #
    set Matrix(tAuto) $Matrix(activeID)
    if {$Matrix(activeID) == ""} {
        set v $Matrix(volume)
        DataAddTransform append Volume($v,node) Volume($v,node)
        MainUpdateMRML
    }

    ## Now test the transforms to make sure they affect the right volumes
   set err1 [RigidIntensityRegistrationTestTransformConnections $Matrix(volume) $Matrix(activeID) 1]
    if {$err1 != ""} {
      DevErrorWindow $err1
    return 0
    }

   set err2 [RigidIntensityRegistrationTestTransformConnections $Matrix(refVolume) $Matrix(activeID) 0]
    if {$err2 != ""} {
      DevErrorWindow $err2
    return 0
    }

}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityTestTransformConnection
#
# Makesure the transformid has an effect on the volumeid
# returns the error message, on "" if none
#
# desiredresult is the number of transforms that should affect the volumeid
#  it should be 1 or 0
# Matrix(transformid,node) should affect the volumeid of interest IF
# desiredresult is set to 1
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationTestTransformConnections \
       {volumeid transformid desiredresult} {

 set name [Volume($volumeid,node) GetName]

 vtkTransform MIRegTmpTransform

 Mrml(dataTree) ComputeNodeTransform Volume($volumeid,node) MIRegTmpTransform

 set NumTrans [MIRegTmpTransform GetNumberOfConcatenatedTransforms]

 if {$NumTrans > 1} {
     MIRegTmpTransform Delete
     return "There are several transforms affecting $name. Sorry. We do not handle more than 1 transform. Please simplify your MRML file"
 }

 if {$desiredresult == 0} {
     MIRegTmpTransform Delete
     if {$NumTrans == 0} {
     return ""
     } else {
     return "There is a transform affecting $name. There should not be one. Please remove it."
     }
 }

 if {$desiredresult == 1} {
     if {$NumTrans == 0} {
     MIRegTmpTransform Delete
     return "No transform affecting $name. Is it possible you have a transform affecting the Refence Volume rather than the Volume to Move?"
     }
     set tmptrans [MIRegTmpTransform GetConcatenatedTransform 0]
     if {$tmptrans != [Matrix($transformid,node) GetTransform] } {
         MIRegTmpTransform Delete
     return "The transform you have selected does not seem to be the one affecting $name. Please choose the correct transform."
     }
 }

 MIRegTmpTransform Delete
 return ""
}


