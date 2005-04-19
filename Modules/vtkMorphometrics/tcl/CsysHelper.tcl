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
# FILE:        CsysHelper.tcl
# PROCEDURES:  
#   MorphometricsPositionCsys Center DirectionXAxis
#   MorphometricsHowToInteractWithCsys
#   MorphometricsHideCsys
#   MorphometricsViewCsys
#   MorphometricsCsysCenter
#   MorphometricsCsysDirectionX
#   MorphometricsCsysDirectionY
#   MorphometricsCsysDirectionZ
#==========================================================================auto=

# Public Interface Description
# CsysHelper is a collection of accessing and interacting with Morphometrics(csys,actor).
# Namely toggling whether it is displayed on viewRen and Set/Get methods for the center
# and orientation of the actor.

#------------------------------------------------------------------------
# .PROC MorphometricsPositionCsys
# Positions the Csys. The first argument specifies the position of the Csys, the 
# second argument the direction of the x-Axis of the Csys.
# .ARGS
# list Center three real numbers, specifying the position of the csys.
# list DirectionXAxis three real numbers, specifying the direction of the x axis of the csys.
# .END
#------------------------------------------------------------------------
proc MorphometricsPositionCsys { Center DirectionXAxis} {
    global Morphometrics
    $Morphometrics(csys,actor) SetPosition 0 0 0 
    $Morphometrics(csys,actor) SetOrientation 0 0 0
    $Morphometrics(csys,actor) RotateWXYZ 180 [expr 1 + [lindex $DirectionXAxis 0]] [lindex $DirectionXAxis 1] [lindex $DirectionXAxis 2]
    $Morphometrics(csys,actor) SetPosition [lindex $Center 0] [lindex $Center 1] [lindex $Center 2]
}
#------------------------------------------------------------------------
# .PROC MorphometricsHowToInteractWithCsys
# Returns a description of how to use a csys. Copied from tcl/tcl-modules/CustomCsys.tcl
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsHowToInteractWithCsys {} {
    return "To move the coordinate system :

To Translate: press the left mouse button over 
the axis you want to translate and move the 
mouse in the appropriate direction.

To Rotate: press the right mouse button over 
the axis you want to rotate and move the 
mouse in the appropriate direction"
}


#------------------------------------------------------------------------
# .PROC MorphometricsHideCsys
# Removes the Csys from the 3D-View
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsHideCsys {} {
    global Morphometrics
    Morphometrics(csys,actor) VisibilityOff
    Render3D
}

#------------------------------------------------------------------------
# .PROC MorphometricsViewCsys
# Displays the Csys in the 3D-View
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsViewCsys {} {
    global Morphometrics
    Morphometrics(csys,actor) VisibilityOn
    Render3D
}


#------------------------------------------------------------------------
# .PROC MorphometricsCsysCenter
# Retrieve the current center of the Csys
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsCsysCenter {} {
    global Morphometrics
    return [Morphometrics(csys,actor) GetPosition]
}


#------------------------------------------------------------------------
# .PROC MorphometricsCsysDirectionX
# Retrieve the direction vector of the first axis of the Csys
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsCsysDirectionX {} {
    global Morphometrics
    set result [[Morphometrics(csys,actor) GetMatrix] MultiplyPoint 1 0 0 0]
    return [list [lindex $result 0] [lindex $result 1] [lindex $result 2]]
}


#------------------------------------------------------------------------
# .PROC MorphometricsCsysDirectionY
# Retrieve the direction vector of the second axis of the Csys
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsCsysDirectionY {} {
    global Morphometrics
    set result [[Morphometrics(csys,actor) GetMatrix] MultiplyPoint 0 1 0 0]
    return [list [lindex $result 0] [lindex $result 1] [lindex $result 2]]
}


#------------------------------------------------------------------------
# .PROC MorphometricsCsysDirectionZ
# Retrieve the direction vector of the third axis of the Csys
# .ARGS
# .END
#------------------------------------------------------------------------
proc MorphometricsCsysDirectionZ {} {
    global Morphometrics
    set result [[Morphometrics(csys,actor) GetMatrix] MultiplyPoint 0 0 1 0]
    return [list [lindex $result 0] [lindex $result 1] [lindex $result 2]]
}

