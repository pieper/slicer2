#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        RigidIntensityRegistration.tcl
# PROCEDURES:  
#   RigidIntensityRegistrationInit
#   RigidIntensityRegistrationBuildSubGUI f
#   RigidIntensityRegistrationSetRegType RegType
#   RigidIntensityRegistrationBuildVTK
#   RigidIntensityRegistrationEnter
#   RigidIntensityRegistrationExit
#   RigidIntensityRegistrationMainExit
#   RigidIntensityRegistrationSetUp
#   RigidIntensityTestTransformConnection vIdMoving vIdStationary transformid
#   RigidIntensityRegistrationGantryTiltTest vId
#   RigidIntensityRegistrationUpdateParam isreg
#==========================================================================auto=

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
    set Module($m,category) "Registration"

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
    set Module($m,procMainExit) RigidIntensityRegistrationMainExit

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
        {$Revision: 1.15 $} {$Date: 2005/08/01 19:44:50 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #

    set RigidIntensityRegistration(sourceId) $Volume(idNone)
    set RigidIntensityRegistration(targetId) $Volume(idNone)
    set RigidIntensityRegistration(matrixId) ""
    set RigidIntensityRegistration(Repeat) 1
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
# frame f frame name
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

    set RigidIntensityRegistration(RegType) VersorMattesMI

    eval {menubutton $f.mbType -text "Rigid Mattes MI" \
            -relief raised -bd 2 -width 20 \
            -menu $f.mbType.m} $Gui(WMBA) 
    eval {menu $f.mbType.m} $Gui(WMA)
    pack  $f.mbType -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach RegType {{MI} {TranslationMI} {TranslationMattesMI} {VersorMattesMI} {AffineMattesMI} {KL}} \
        name {{Rigid MI} {Translation MI} {Translation Mattes MI} {Rigid Mattes MI} {Affine Mattes MI} {KL}} { 
            set RigidIntensityRegistration($RegType) $name 
            $f.mbType.m add command -label $name \
                -command "RigidIntensityRegistrationSetRegType $RegType"
        }
    # save menubutton for config
    set RigidIntensityRegistration(gui,mbRegistrationType) $f.mbType
    # put a tooltip over the menu
    TooltipAdd $f.mbType \
            "Choose the type of Registration Algorithm. Choose Rigid Mattes MI unless you know what you are doing."

    #-------------------------------------------
    # Choice frame
    #-------------------------------------------

    set f $framename.fChoice
    #
    # Swappable Frames for MI/KL methods
    #
    foreach type "MI TranslationMI TranslationMattesMI VersorMattesMI AffineMattesMI KL" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set RigidIntensityRegistration(f${type}) $f.f${type}
    }
    raise $RigidIntensityRegistration(fVersorMattesMI)

    MutualInformationRegistrationBuildSubGui $f.fMI
    TranslationMIGradientDescentRegistrationBuildSubGui $f.fTranslationMI
    VersorMattesMIRegistrationBuildSubGui $f.fVersorMattesMI
    AffineMattesMIRegistrationBuildSubGui $f.fAffineMattesMI
    TranslationMattesMIRegistrationBuildSubGui $f.fTranslationMattesMI
    KullbackLeiblerRegistrationBuildSubGui $f.fKL
    set fKL $f.fKL
}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationSetRegType
#   raise the appropriate GUI
# .ARGS
# string RegType
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationSetRegType { RegType} {
 global RigidIntensityRegistration

 set RigidIntensityRegistration(RegType) $RegType

 raise $RigidIntensityRegistration(f$RegType)

 $RigidIntensityRegistration(gui,mbRegistrationType) config -text $RigidIntensityRegistration($RegType)
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
# .PROC RigidIntensityRegistrationMainExit
# Called when slicer is going to exit -- clean up anything that needs a 
# special shutdown sequence.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationMainExit {} {

    catch ".mi.reg pre_destroy"
    catch "destroy .mi"
}


#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationSetUp
#
# Make sure the volumes and the transform are OK.
# Also, set some variables
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationSetUp {} {
  global Matrix Volume RigidIntensityRegistration
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

    if {[RigidIntensityRegistrationGantryTiltTest $Matrix(refVolume)] == 0} {
        DevWarningWindow "The Reference Volume has Gantry Tilt. This is not allowed."
        return 0
    }

    if {[RigidIntensityRegistrationGantryTiltTest $Matrix(volume)] == 0} {
        DevWarningWindow "The Moving Volume has Gantry Tilt. This is not allowed."
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
   set err1 [RigidIntensityRegistrationTestTransformConnections $Matrix(volume) $Matrix(refVolume) $Matrix(activeID)]
    if {$err1 != ""} {
      DevErrorWindow $err1
    return 0
    }
    
    set RegType $RigidIntensityRegistration(RegType)
    if {[RigidIntensityRegistrationCheckParameters$RegType] == 0} {
        return 0
    }
    
    # sourceId = ID of volume to register (source, moving)
    # targetId = ID of reference volume   (target, stationary)
    # matrixId = ID of the transform to change
    set RigidIntensityRegistration(sourceId) $Matrix(volume)
    set RigidIntensityRegistration(targetId) $Matrix(refVolume)
    set RigidIntensityRegistration(matrixId) $Matrix(activeID)

     return 1
}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityTestTransformConnection
#
# Make sure the transforms fit the restrictions
# .ARGS
# int vIdMoving 
# int vIdStationary 
# int transformid
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationTestTransformConnections \
       {vIdMoving vIdStationary transformid} {

 set Movingname     [Volume($vIdMoving,node) GetName]
 set Stationaryname [Volume($vIdStationary,node) GetName]

 vtkTransform MIRegVMovingTransform
 vtkTransform MIRegVStationaryTransform

 Mrml(dataTree) ComputeNodeTransform Volume($vIdMoving,node) \
    MIRegVMovingTransform
 Mrml(dataTree) ComputeNodeTransform Volume($vIdStationary,node) \
    MIRegVStationaryTransform
   
 set NumMovTrans [MIRegVMovingTransform GetNumberOfConcatenatedTransforms]
 set NumStaTrans [MIRegVStationaryTransform GetNumberOfConcatenatedTransforms]

  ### now check all the error messages

 if {$NumMovTrans != $NumStaTrans+1} {
    MIRegVMovingTransform     Delete
    MIRegVStationaryTransform Delete

     ## deal with a pretty typical error
     if {$NumMovTrans == $NumStaTrans-1} {
         return "One more transform is effecting the Reference volume
than the Moving volume. However, the opposite should be true!
Switching the Moving and Reference volumes will likely fix this
problem."
     } else {
    return "There are $NumMovTrans transforms affecting the Moving volume and $NumStaTrans affecting the Refence Volume. The Moving volume should have one more Transform affecting it than the moving volume."
     }
 }

 ### At this point there are the correct number of transforms in each
 ### Now, make sure the tree is identical.
 ### Moving should have an extra transform at the beginning

 for {set i 0} {$i < $NumStaTrans} { incr i} {
   if {[MIRegVMovingTransform GetConcatenatedTransform [expr $i+1]] !=
       [MIRegVStationaryTransform GetConcatenatedTransform $i] } {
           MIRegVMovingTransform     Delete
           MIRegVStationaryTransform Delete
           return "Except for the last transform, the transforms affecting both the Moving and Reference volume must be the same. It seems that one of the transforms was not the same for both volumes. Please correct this problem. Read the help if you need more assistance."
   }
  }

 ### Now, is the last transform the one they selected?
 set tmptrans [MIRegVMovingTransform GetConcatenatedTransform 0]
 if {$tmptrans != [Matrix($transformid,node) GetTransform] } {
           MIRegVMovingTransform     Delete
           MIRegVStationaryTransform Delete
     return "The transform you have selected does not seem to be the last one affecting $Movingname. Please correct this problem. Read Help if you need more information."
     }

           MIRegVMovingTransform     Delete
           MIRegVStationaryTransform Delete
 return ""
}   


#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationGantryTiltTest
#
# Make sure the volume has no gantry tilt. Return 0 if GantryTilt
# .ARGS
# int vId
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationGantryTiltTest {vId} {

  set det [[Volume($vId,node) GetPosition] Determinant]
  set risidual [expr abs(abs($det) - 1)]
    if {$risidual > 0.01} {
        return 0
    }
 return 1
}

#-------------------------------------------------------------------------------
# .PROC RigidIntensityRegistrationUpdateParam
#
# Update parameters in case anyone changed them
#
# .ARGS
# boolean isreg
# .END
#-------------------------------------------------------------------------------
proc RigidIntensityRegistrationUpdateParam { isreg } {
    global RigidIntensityRegistration

    $isreg config \
        -transform       $RigidIntensityRegistration(matrixId)            \
        -source          $RigidIntensityRegistration(sourceId)            \
        -target          $RigidIntensityRegistration(targetId)            \
        -resolution      $RigidIntensityRegistration(Resolution)          \
        -source_shrink   $RigidIntensityRegistration(SourceShrinkFactors) \
        -target_shrink   $RigidIntensityRegistration(TargetShrinkFactors) \
        -auto_repeat     $RigidIntensityRegistration(Repeat)
}

