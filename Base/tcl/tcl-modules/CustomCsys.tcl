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
# FILE:        CustomCsys.tcl
# PROCEDURES:  
#   CustomCsysInit
#   CustomCsysBuildVTK
#   CustomCsysBuildGUI
#   CustomCsysEnter
#   CustomCsysExit
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC CustomCsysInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CustomCsysInit {} {
    global CustomCsys Module Volume Model Csys

    set m CustomCsys
    
    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "This module is an example of how to hava a csys actor in your own module"
    set Module($m,author) "Delphine, Nain, SlicerHaker, delfin@mit.edu"
    # version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.6 $} {$Date: 2003/03/19 19:16:30 $}]

    # Define Tabs
    #------------------------------------
    set Module($m,row1List) "Help Stuff"
    set Module($m,row1Name) "{Help} {Coord Sys}"
    set Module($m,row1,tab) Stuff

    # Define Procedures
    #------------------------------------
    # For a thorough description, see the Description in the Custom.tcl file
    #
    # To use a Csys, you need at least a vtk,  enter and exit procedure

    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button

    set Module($m,procGUI) CustomCsysBuildGUI
    set Module($m,procEnter) CustomCsysEnter
    set Module($m,procExit) CustomCsysExit
    set Module($m,procVTK) CustomCsysBuildVTK

    # Define Dependencies
    #------------------------------------
    # This module depends on Csys
    set Module($m,depend) Csys


    # set to 1 when the Csys is visible and should be Picked on
    # mouse down
    set Csys(active) 0

}
#-------------------------------------------------------------------------------
# .PROC CustomCsysBuildVTK
#
# Builds the VTK Csys
# .END
#-------------------------------------------------------------------------------
proc CustomCsysBuildVTK {} {

    global CustomCsys Csys

    # How to create a Csys:
    #
    # CsysCreate <module> <actorName> <length width height>
    #
    # This procedure creates a Csys actor with the name 
    # $module($actorName,actor)
    # To use the default size, use the parameters -1 -1 -1 
    CsysCreate CustomCsys csys -1 -1 -1
}

#-------------------------------------------------------------------------------
# .PROC CustomCsysBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc CustomCsysBuildGUI {} {
    global Gui CustomCsys Module Volume Model Gui
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(CustomCsys,fStuff)
    
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The CustomCsys module is an example for developers on how to add a Csys actor to their module.  It shows how to add the actor and the event bindings to be able to move the actor.  The source code is in slicer/program/tcl-modules/CustomCsys.tcl.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Stuff:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags CustomCsys $help
    MainHelpBuildGUI CustomCsys
    
    #-------------------------------------------
    # Stuff frame
    #-------------------------------------------
    set fStuff $Module(CustomCsys,fStuff)
    set f $fStuff

    eval {label $f.lhow -text "
To move the csys actor :

To Translate: press the left mouse button over 
the axis you want to translate and move the 
mouse in the appropriate direction.

To Rotate: press the right mouse button over 
the axis you want to rotate and move the 
mouse in the appropriate direction." } $Gui(WLA)
    pack $f.lhow
}

#-------------------------------------------------------------------------------
# .PROC CustomCsysEnter
# Called when this module is entered by the user.  
#
# The event handler that handles the movement of the csys defined in this \
# module is activated when you call 
# CsysCreateBindings <module> <custom event handler>
# from inside this procedure. It activates both the handler for the csys and 
# your own handler defined as <custom event handler>.
#
# The handler for the csys detects when the user clicks on one of its axis 
# and drags the mouse and moves the csys actor accordingly
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CustomCsysEnter {} {
    global CustomCsys Csys

    set Csys(active) 1

    MainAddActor CustomCsys(csys,actor)
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC CustomCsysExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CustomCsysExit {} {
    global CustomCsys Csys

    set Csys(active) 0
    MainRemoveActor CustomCsys(csys,actor)
    Render3D
}


