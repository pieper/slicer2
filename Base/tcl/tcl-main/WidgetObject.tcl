#=auto==========================================================================
# Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
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


