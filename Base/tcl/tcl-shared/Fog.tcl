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
# FILE:        Fog.tcl
# PROCEDURES:  
#   FogCheckGlobal
#   FogApply the
#   FogBuildGui frame
#==========================================================================auto=

#
#
#  

#-------------------------------------------------------------------------------
# .PROC FogCheckGlobal
# 
#
#  Global variable Fog for the fog Parameters
#
#  Fog(Enabled) On  0ff
#  Fog(mode)    linear exp exp2
#  Fog(start)   start distance for linear fog in [0,1]
#  Fog(end)     end   distance for linear fog in [0,1]
#
#  the real distances are then computed
#
# .ARGS
#    
# .END
#-------------------------------------------------------------------------------
proc FogCheckGlobal {} {
#    --------------

  global Fog

  #
  # check the existence of the global variable or create them
  # with default values
  #
  if {![info exist Fog(Enabled)]}   {    set Fog(Enabled) "On"   }
  if {![info exist Fog(mode)   ]}   {    set Fog(mode)    linear }
  if {![info exist Fog(start)  ]}   {    set Fog(start)   0.5    }
  if {![info exist Fog(end)    ]}   {    set Fog(end)     1      }

  if {$Fog(start) < 0} { set Fog(start) 0 }
  if {$Fog(start) > 1} { set Fog(start) 1 }

  if {$Fog(end) < $Fog(start)} { set Fog(end) [expr $Fog(start)+0.1]}
  if {$Fog(end) > 2}           { set Fog(end) 2}

} 
# end FogCheckGlobal


#-------------------------------------------------------------------------------
# .PROC FogApply
# 
#
#
# .ARGS
#    ren the render
# .END
#-------------------------------------------------------------------------------
proc FogApply {renwin} {
#    --------

  global  boxActor View Fog

  #  set bounds [boxActor GetBounds]


  #
  # check the existence of the global variable or create them
  # with default values
  #
  FogCheckGlobal

  #
  #  Check the values of the parameters
  #

  if {$Fog(mode) != "linear"} {
    puts "Only linear fog supported at the moment \n"
    set Fog(mode) linear
  }


    if {$Fog(mode) == "linear"}  {

        vtkFog f
        
        f SetFogEnabled [expr  {$Fog(Enabled) == "On"}]
        
        set renderers [$renwin GetRenderers]
        set numRenderers [$renderers GetNumberOfItems]
        
        $renderers InitTraversal
        for {set i 0} {$i < $numRenderers} {incr i} {
            set ren  [$renderers GetNextItem]

            set fov   [expr $View(fov) ]
            set fov2  [expr $fov / 2   ]
            set dist  [[$ren GetActiveCamera] GetDistance]

            f SetFogStart [expr $dist - $fov2 + $Fog(start) * $fov ]
            f SetFogEnd   [expr $dist - $fov2 + $Fog(end)   * $fov ]

            f Render  $ren
        }
      
        f Delete
    }
} 
# end FogApply


#-------------------------------------------------------------------------------
# .PROC FogBuildGui
# 
#  Create interface for the Fog parameters
#
# .ARGS
#    f frame for building the fog interface
# .END
#-------------------------------------------------------------------------------
proc FogBuildGui {fFog} {
#    -----------

    global Gui Fog

    FogCheckGlobal ;# set defaults
    set Fog(Enabled) Off
  
    #-------------------------------------------
    # Fog frame
    #-------------------------------------------
    # Setup the fog parameters
#    set fFog $Module(View,fFog)
    set f $fFog

    frame $f.fEnabled  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fMode     -bg $Gui(activeWorkspace)
    frame $f.fLimits   -bg $Gui(activeWorkspace) -relief groove -bd 3

    eval {label $f.lLimits  -text "Limits, bounding box is \[0\,1\]:"} $Gui(WTA)

    pack $f.fEnabled $f.fMode $f.lLimits $f.fLimits \
        -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Fog->Enabled
    #-------------------------------------------
    set f $fFog.fEnabled
    
    eval {label $f.lEnabled -text "Enable Fog: "} $Gui(WLA)
    pack $f.lEnabled -side left -padx $Gui(pad) -pady 0

    foreach value "On Off" width "4 4" {
        eval { radiobutton $f.rEnabled$value -width $width \
               -text "$value" -value "$value" -variable Fog(Enabled) \
               -indicatoron 0 -command "Render3D" \
              } $Gui(WRA)
    
        pack $f.rEnabled$value -side left -padx 2 -pady 2 -fill x
    }

    #-------------------------------------------
    # Fog->Mode
    #-------------------------------------------
    set f $fFog.fMode

    foreach value "linear exp exp2" {
        eval {radiobutton $f.r$value \
              -text "$value" -value "$value" \
          -variable Fog(mode) \
              -indicatoron 0 \
          -command "Render3D" \
          }  $Gui(WRA)
        pack $f.r$value -side left -padx 2 -pady 2 -expand 1 -fill x
    }


    #-------------------------------------------
    # Fog->Limits
    #-------------------------------------------
    set f $fFog.fLimits


    #        pack $f.lLimits  -side left -padx 0 -pady 0

    # Start Slider
    #        
    eval {label $f.lStart  -text "Start" -width 5} $Gui(WTA)

    eval {entry $f.eStart -textvariable Fog(start) -width 4} $Gui(WEA)

    eval {scale $f.sStart -from 0 -to 2        \
          -variable Fog(start) \
          -orient vertical     \
          -command "Render3D"  \
          -resolution .01      \
          } $Gui(WSA)

    bind $f.sStart <Motion> "Render3D"

    grid $f.lStart $f.eStart $f.sStart

    $f.sStart set 0.5

    # End Slider
    #        
    eval {label $f.lEnd  -text "End" -width 5} $Gui(WTA)

    eval {entry $f.eEnd -textvariable Fog(end) -width 4} $Gui(WEA)

    eval {scale $f.sEnd -from 0 -to 2        \
      -variable Fog(end) \
          -orient vertical     \
      -command "Render3D"  \
          -resolution .01      \
          } $Gui(WSA)

    bind $f.sEnd <Motion> "Render3D"

    grid $f.lEnd $f.eEnd $f.sEnd 

    $f.sEnd set 1


} 
# end FogBuildGui
