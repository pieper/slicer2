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
# FILE:        CustomCsys.tcl
# PROCEDURES:  
#   CustomCsysInit
#   CustomCsysBuildVTK
#   CustomCsysBuildGUI
#   CustomCsysEnter
#   CustomCsysExit
#   CustomCsysCreateBindings
#   CustomCsysDoSomethingCool
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
    global CustomCsys Module Volume Model

    set m CustomCsys
    
    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "This module is an example of how to hava a csys actor in your own module"
    set Module($m,author) "Delphine, Nain, SlicerHaker, delfin@mit.edu"
    # version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1 $} {$Date: 2002/04/19 16:23:45 $}]

    # Define Tabs
    #------------------------------------
    set Module($m,row1List) "Help Stuff"
    set Module($m,row1Name) "{Help} {Tons o' Stuff}"
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

    # Create any specific bindings for this module
    #------------------------------------
    #
    CustomCsysCreateBindings
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

    # passing the custom handler doesn't work right now
    # I need to ask Mike for help...
    CsysPushBindings CustomCsys CustomCsysEvents
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
    # Deactivate the bindings that were created by the procedure 
    # CustomCsysPushBindings called from the Enter procedure in your module
    CsysPopBindings CustomCsys 
    MainRemoveActor CustomCsys(csys,actor)
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC CustomCsysCreateBindings
# Creates specific events sets we'll need for this module
#
# (and then pass that handler to the procedure CsysPushBindings that should
# be called from the Enter procedure so that they become active when the user
# enters the module, along with the handler to move the csys). 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CustomCsysCreateBindings {} {
    global Gui Ev CustomCsys Csys
        
    EvDeclareEventHandler CustomCsysEvents <KeyPress-c> {CustomCsysDoSomethingCool}
    
}



#-------------------------------------------------------------------------------
# .PROC CustomCsysDoSomethingCool
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CustomCsysDoSomethingCool {} {
    puts "something cool"
}
