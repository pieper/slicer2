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
# FILE:        WidgetObject.tcl
# PROCEDURES:  
#==========================================================================auto=
# These procs allow widgets to behave like objects with their own
# state variables of processing objects.


# generate a "unique" name for a widget variable
proc GenerateWidgetVariable {widget varName} {
   regsub -all {\.} $widget "_" base

   return "$varName$base"
}

# returns an object which will be associated with a widget
# no error checking
proc NewWidgetObject {widget type varName} {
   set var [GenerateWidgetVariable $widget $varName]
   # create the vtk object
   $type $var

   return $var
}

# returns the name of an object previously created by NewWidgetObject
proc GetWidgetObject {widget varName} {
   return [GenerateWidgetVariable $widget $varName]
}

# sets the value of a widget variable
proc SetWidgetVariableValue {widget varName value} {
   set var [GenerateWidgetVariable $widget $varName]
   global $var
   set $var $value
}

# This proc has alway eluded me.
proc GetWidgetVariableValue {widget varName} {
   set var [GenerateWidgetVariable $widget $varName]
   global $var
   set temp ""
   catch {eval "set temp [format {$%s} $var]"}

   return $temp
}


