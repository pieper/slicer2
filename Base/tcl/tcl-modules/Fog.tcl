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
# FILE:        Fog.tcl
# PROCEDURES:  
#   FogCheckGlobal
#   FogDefault the
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
#  Fog(Enabled) On or 0ff
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
# .PROC FogDefault
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
