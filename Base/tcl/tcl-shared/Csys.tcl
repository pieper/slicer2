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
# FILE:        Csys.tcl
# PROCEDURES:  
#   CsysInit
#   CsysPopBindings
#   CsysPushBindings
#   CsysCreateBindings
#   CsysUnselectActor
#   CsysSelectActor
#   CsysParams
#   CsysResize
#   CsysCreate
#==========================================================================auto=

############################################################################
#
#      YOU SHOULD NOT NEED TO MODIFY THIS FILE IF YOU JUST WANT TO USE
#      A CSYS IN YOUR MODULE. FOR THAT, PLEASE READ tcl-modules/CustomCsys.tcl
#      IT WILL SHOW YOU HOW TO DO IT.
#
#      THIS FILE DEFINES GENERAL OPERATIONS TO CREATE/INTERACT with a CSYS
#
#      By the way, Csys means Coordinate-SYStem and it was originally created
#      by Peter Everett and generalized by Delphine Nain.
#      
#
############################################################################

#-------------------------------------------------------------------------------
# .PROC CsysInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysInit {} {
    
    global Csys

    CsysCreateBindings

    vtkCellPicker Csys(picker)
    Csys(picker) SetTolerance 0.001
    
    set Csys(xactor,selectedColor) "1.0 0 0"
    set Csys(yactor,selectedColor) "0 1.0 0"
    set Csys(zactor,selectedColor) "0 0 1.0"
    
    set Csys(xactor,color) "1.0 0.4 0.4"
    set Csys(yactor,color) "0.4 1.0 0.4"
    set Csys(zactor,color) "0.4 0.4 1.0"

    set Csys(activeModule) ""
    set Csys(activeActor) ""
}


########################################################################
#
#
#          BINDINGS
#
#
#########################################################################

#-------------------------------------------------------------------------------
# .PROC CsysPopBindings
#  Pop the bindings activated in CsysPushBindings
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysPopBindings {module} {
    global Ev Csys

    EvDeactivateBindingSet CsysEvents

    set Csys(activeModule) ""
    set Csys(activeActor) ""
}

#-------------------------------------------------------------------------------
# .PROC CsysPushBindings
# Push the bindings to handle the movement of the csys actor (defined in CsysCreateBindings) as well as other optional handlers
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysPushBindings {module {eventHandler ""}} {
    global Ev Csys Gui 

    set Csys(activeModule) $module

    # csys events for 3d view
    EvAddWidgetToBindingSet CsysEvents $Gui(fViewWin) {{CsysMouseEvents} {tkRegularEvents} {$eventHandler}}
    
    # other events set we'll need if the csys wasn't selected:
    EvAddWidgetToBindingSet CsysEventsTkMotion $Gui(fViewWin) {{tkMotionEvents} {tkRegularEvents} {CsysMouseEvents} {$eventHandler}}
    
    EvActivateBindingSet CsysEvents
}

#-------------------------------------------------------------------------------
# .PROC CsysCreateBindings
# Creates events sets we'll  need for this module
#
# Main events set: CsysMouseEvents
# The entry point is the mouse click/mouse release that selects/unselects
# => Based on whether a csys actor is selected or not, we either interact 
# regularly with the screen (virtual camera moves) or we have a 
# selected a csys actor. If the selected actor is a csys, then the 
# mouse motion moves that gyro axis. 
#
# sub events set: CsysTkMotion (regular tk interaction)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysCreateBindings {} {
    global Gui Ev Csys 
            
    EvDeclareEventHandler CsysMouseEvents <Any-ButtonPress> {CsysSelectActor %W %x %y}
    EvDeclareEventHandler CsysMouseEvents <Any-ButtonRelease> {CsysUnselectActor %W %x %y}
    
}

   

#-------------------------------------------------------------------------------
# .PROC CsysUnselectActor
# Called when any mouse button is released 
# If the user was navigating around the 3D scene (determined in SelectActor), 
# then pop the events from the event stack so we can be ready to handle another
# mouse click
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysUnselectActor {widget x y} {
    
    global Csys CsysEvents
    
    # if we were using regular tk interaction, 
    # then pop the tk interaction events from the stack
    if { $Csys(tkInteraction) == 1} {
    # deactivate the tk motion bindings
    EvDeactivateBindingSet CsysEventsTkMotion
    set Csys(tkInteraction) 0
    EndMotion $widget $x $y
    }
}


#-------------------------------------------------------------------------------
# .PROC CsysSelectActor
#  This is called when any mouse button is pressed
#  * If the selected actor is a csys of the active module, then
#     move or rotate it accordingly
#  * Otherwise call a callback procedure of the active module defined in 
#    Module(<active module>,procCsysSelectActor). If this callback returns
#    1, that means an actor that we wanted was picked, so do nothing else
#  * In all other cases, just push onto the event stack the regular 
#    tk interaction events so the user can navigate in the 3D scene 
#    
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysSelectActor {widget x y} {

    global Csys Ev 
    global $Csys(activeModule) 

    set module $Csys(activeModule)
    set Csys(tkInteraction) 0

    if { [SelectPick Csys(picker) $widget $x $y] != 0 } {
    
    # get the path of actors selected by the picker
    set assemblyPath [Csys(picker) GetPath]
    $assemblyPath InitTraversal
    set assemblyNode [$assemblyPath GetLastNode]
    set pickactor [$assemblyNode GetProp]
    set actor [lindex $Csys($module,actors) 0]
    
    # shouldn't happen, but defensive programming can't hurt
    if {$actor == ""} {
        # problem here, just go back to the regular interaction mode
        EvActivateBindingSet CsysEventsTkMotion
        set Csys(tkInteraction) 1
        StartMotion $widget $x $y
        return
    }
    
    if { [$pickactor GetProperty] == [${module}($actor,Xactor) GetProperty] } {
        
        eval [${module}($actor,Xactor) GetProperty] SetColor \
            $Csys(xactor,selectedColor)
        eval [${module}($actor,Yactor) GetProperty] SetColor \
            $Csys(yactor,color)
        eval [${module}($actor,Zactor) GetProperty] SetColor \
            $Csys(zactor,color)
        XformAxisStart $module $actor $widget 0 $x $y
        
        
    } elseif { [$pickactor GetProperty] == [${module}($actor,Yactor) GetProperty] } {
        eval [${module}($actor,Xactor) GetProperty] SetColor \
            $Csys(xactor,color) 
        eval [${module}($actor,Yactor) GetProperty] SetColor \
            $Csys(yactor,selectedColor)
        eval [${module}($actor,Zactor) GetProperty] SetColor \
            $Csys(zactor,color) 
        XformAxisStart $module $actor $widget 1 $x $y
        
        
    } elseif { [$pickactor GetProperty]  == [${module}($actor,Zactor) GetProperty] } {
        eval [${module}($actor,Xactor) GetProperty] SetColor \
            $Csys(xactor,color) 
        eval [${module}($actor,Yactor) GetProperty] SetColor \
            $Csys(yactor,color) 
        eval [${module}($actor,Zactor) GetProperty] SetColor \
            $Csys(zactor,selectedColor)
        XformAxisStart $module $actor $widget 2 $x $y
    } else {
    
    
    # we haven't returned at this point, which means none of the csys
    # for the active module were picked
    
    # If it exists, Call the callback procedure of the active module
    #-------------------------------------------
        if {[info exists Module($module,procCsysSelectActor)] == 1} {
        if { [$Module($m,procCsysSelectActor) $pickactor]== 0 } {
            
            # the callback function didn't find an interesting actor, interact as usual
            # if none of the above is picked, 
            # then interact with the virtual camera, as usual
            # activate the tk motion bindings
            EvActivateBindingSet CsysEventsTkMotion
            set Csys(tkInteraction) 1
            StartMotion $widget $x $y
        }
        } else {
        # if none of the above is picked and there is no callback, 
        # then interact with the virtual camera, as usual
        # activate the tk motion bindings
        EvActivateBindingSet CsysEventsTkMotion
        set Csys(tkInteraction) 1
        StartMotion $widget $x $y
        }
    }
    } else {
    # if none of the above is picked and there is no callback, 
    # then interact with the virtual camera, as usual
    # activate the tk motion bindings
    EvActivateBindingSet CsysEventsTkMotion
    set Csys(tkInteraction) 1
    StartMotion $widget $x $y
    }

    Render3D
}

##################################################################
#
#             VTK ACTOR CREATION
#
#
#################################################################

#-------------------------------------------------------------------------------
# .PROC CsysParams
#  set length, width, height parameters 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysParams { module actor {axislen -1} {axisrad -1} {conelen -1} } {
    global Csys ${module}
    
    if { $axislen == -1 } { set axislen 100 }
    if { $axisrad == -1 } { set axisrad [expr $axislen*0.02] }
    if { $conelen == -1 } { set conelen [expr $axislen*0.2]}
    set axislen [expr $axislen-$conelen]
    
    set ${module}($actor,size) $axislen
    
    # set parameters for cylinder geometry and transform
    ${module}($actor,AxisCyl) SetRadius $axisrad
    ${module}($actor,AxisCyl) SetHeight $axislen
    ${module}($actor,AxisCyl) SetCenter 0 [expr -0.5*$axislen] 0
    ${module}($actor,AxisCyl) SetResolution 8
    ${module}($actor,CylXform) Identity
    ${module}($actor,CylXform) RotateZ 90
    
    # set parameters for cone geometry and transform
    ${module}($actor,AxisCone) SetRadius [expr $axisrad * 2.5]
    ${module}($actor,AxisCone) SetHeight $conelen
    ${module}($actor,AxisCone) SetResolution 8
    ${module}($actor,ConeXform) Identity
    ${module}($actor,ConeXform) Translate $axislen 0 0
    # pce_debug_msg [concat "Csys params: axislen=" $axislen " axisrad=" \
        #         $axisrad " conelen=" $conelen]
}

#-------------------------------------------------------------------------------
# .PROC CsysResize
#  Set the size of the csys 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysResize { module actor size } {
    global ${module}
    CsysParams ${module} $actor $size
    Render3D
}

# procedure culminates with creation of "Csys" Assembly Actor
# with a Red X-Axis, a Green Y-Axis, and a Blue Z-Axis

#-------------------------------------------------------------------------------
# .PROC CsysCreate
#  create the Csys vtk actor $module($actor,actor)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CsysCreate { module actor axislen axisrad conelen  } {
    global Csys ${module}

    vtkCylinderSource ${module}($actor,AxisCyl)
    vtkConeSource ${module}($actor,AxisCone)
    vtkTransform ${module}($actor,CylXform)
    vtkTransform ${module}($actor,ConeXform)
    vtkTransformPolyDataFilter ${module}($actor,ConeXformFilter)
    ${module}($actor,ConeXformFilter) SetInput [${module}($actor,AxisCone) GetOutput]
    ${module}($actor,ConeXformFilter) SetTransform ${module}($actor,ConeXform)
    vtkTransformPolyDataFilter ${module}($actor,CylXformFilter)
    ${module}($actor,CylXformFilter) SetInput [${module}($actor,AxisCyl) GetOutput]
     ${module}($actor,CylXformFilter) SetTransform ${module}($actor,CylXform)
    vtkAppendPolyData ${module}($actor,Axis)
    ${module}($actor,Axis) AddInput [${module}($actor,CylXformFilter) GetOutput]
    ${module}($actor,Axis) AddInput [${module}($actor,ConeXformFilter) GetOutput]
    vtkPolyDataMapper ${module}($actor,AxisMapper)
    ${module}($actor,AxisMapper) SetInput [${module}($actor,Axis) GetOutput]
    vtkActor ${module}($actor,Xactor)
    ${module}($actor,Xactor) SetMapper ${module}($actor,AxisMapper)
    eval [${module}($actor,Xactor) GetProperty] SetColor $Csys(xactor,color) 
    ${module}($actor,Xactor) PickableOn
    vtkActor ${module}($actor,Yactor)
    ${module}($actor,Yactor) SetMapper ${module}($actor,AxisMapper)
    eval [${module}($actor,Yactor) GetProperty] SetColor $Csys(yactor,color) 
    ${module}($actor,Yactor) RotateZ 90
    ${module}($actor,Yactor) PickableOn
    vtkActor ${module}($actor,Zactor)
    ${module}($actor,Zactor) SetMapper ${module}($actor,AxisMapper)
    eval [${module}($actor,Zactor) GetProperty] SetColor $Csys(zactor,color) 
    ${module}($actor,Zactor) RotateY -90
    ${module}($actor,Zactor) PickableOn
    CsysParams $module $actor $axislen $axisrad $conelen
    set ${module}($actor,actor) [vtkAssembly ${module}($actor,actor)]
    ${module}($actor,actor) AddPart ${module}($actor,Xactor)
    ${module}($actor,actor) AddPart ${module}($actor,Yactor)
    ${module}($actor,actor) AddPart ${module}($actor,Zactor)
    ${module}($actor,actor) PickableOff
    vtkMatrix4x4 ${module}($actor,matrix)
    vtkTransform ${module}($actor,xform)
    vtkTransform ${module}($actor,actXform)
    vtkMatrix4x4 ${module}($actor,inverse)
    vtkTransform ${module}($actor,rasToWldTransform)
    ${module}($actor,actor) SetUserMatrix [${module}($actor,rasToWldTransform) GetMatrix]

    # store all the csys actors by module
    if {[info exists Csys($module,actors)] == 0 } {
    set Csys($module,actors) $actor
    } else {
    lappend Csys($module,actors) $actor
    }
}
