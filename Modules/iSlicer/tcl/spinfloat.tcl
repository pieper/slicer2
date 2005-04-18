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
# FILE:        spinfloat.tcl
# PROCEDURES:  
#==========================================================================auto=
# Spinfloat 
# ----------------------------------------------------------------------
# Implements an float spinner widget.  It inherits basic spinner
# functionality from Spinner and adds specific features to create 
# an float-only spinner. 
# Arrows may be placed horizontally or vertically.
# User may specify an float range and step value.
# Spinner may be configured to wrap when min or max value is reached.
#
# NOTE:
# Spinfloat float values should not exceed the size of a long float.
# For a 32 bit long the float range is -2147483648 to 2147483647.
#
# ----------------------------------------------------------------------
#   AUTHOR:  Sue Yockey               Phone: (214) 519-2517
#                                     E-mail: syockey@spd.dsccc.com
#                                             yockey@acm.org
#
#   @(#) $Id: spinfloat.tcl,v 1.3 2005/04/18 21:21:56 nicole Exp $
# ----------------------------------------------------------------------
#            Copyright (c) 1995 DSC Technologies Corporation
# ======================================================================
# Permission to use, copy, modify, distribute and license this software 
# and its documentation for any purpose, and without fee or written 
# agreement with DSC, is hereby granted, provided that the above copyright 
# notice appears in all copies and that both the copyright notice and 
# warranty disclaimer below appear in supporting documentation, and that 
# the names of DSC Technologies Corporation or DSC Communications 
# Corporation not be used in advertising or publicity pertaining to the 
# software without specific, written prior permission.
# 
# DSC DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, AND NON-
# INFRINGEMENT. THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, AND THE
# AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, 
# SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. IN NO EVENT SHALL 
# DSC BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
# ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
# WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
# SOFTWARE.
# ======================================================================

#
# Usual options.
#
itk::usual Spinfloat {
    keep -background -borderwidth -cursor -foreground -highlightcolor \
         -highlightthickness -insertbackground  -insertborderwidth \
         -insertofftime -insertontime -insertwidth -labelfont \
         -selectbackground -selectborderwidth -selectforeground \
         -textbackground -textfont
}

# ------------------------------------------------------------------
#                            SPINFLOAT
# ------------------------------------------------------------------
## if { [itcl::find class Spinfloat] == "" } 
if { [info command ::iwidgets::spinfloat] == "" } {
    itcl::class iwidgets::Spinfloat {
        inherit iwidgets::Spinner 

        constructor {args} {
            Spinner::constructor -validate real 
        } {}

        itk_option define -range range Range "" 
        itk_option define -step step Step 0.1 
        itk_option define -wrap wrap Wrap false 

        public method up {}
        public method down {}
    }
} 

#
# Provide a lowercased access method for the Spinfloat class.
# 
proc ::iwidgets::spinfloat {pathName args} {
    uplevel ::iwidgets::Spinfloat $pathName $args
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR
# ------------------------------------------------------------------
itcl::body iwidgets::Spinfloat::constructor {args} {
    eval itk_initialize $args
    
    $itk_component(entry) delete 0 end
    
    if {[lindex $itk_option(-range) 0] == ""} {
        $itk_component(entry) insert 0 "0"
    } else { 
        $itk_component(entry) insert 0 [lindex $itk_option(-range) 0] 
    }
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

# ------------------------------------------------------------------
# OPTION: -range
#
# Set min and max values for spinner.
# ------------------------------------------------------------------
itcl::configbody iwidgets::Spinfloat::range {
    if {$itk_option(-range) != ""} {
        if {[llength $itk_option(-range)] != 2} {
            error "wrong # args: should be\
                    \"$itk_component(hull) configure -range {begin end}\""
            }

            set min [lindex $itk_option(-range) 0]
            set max [lindex $itk_option(-range) 1]

            set nconv [::scan $min %g newmin]
            if { $nconv != 1 || $min != $newmin } {
                error "bad range option \"$min\": begin value must be\
                    an float"
            }
            set nconv [::scan $max %g newmax]
            if { $nconv != 1 || $max != $newmax } {
                error "bad range option \"$max\": end value must be\
                    an float"
            }
            if {$min > $max} {
                error "bad option starting range \"$min\": must be less\
                    than ending: \"$max\""
            }
    } 
}

# ------------------------------------------------------------------
# OPTION: -step
#
# Increment spinner by step value.
# ------------------------------------------------------------------
itcl::configbody iwidgets::Spinfloat::step {
}

# ------------------------------------------------------------------
# OPTION: -wrap
#
# Specify whether spinner should wrap value if at min or max.
# ------------------------------------------------------------------
itcl::configbody iwidgets::Spinfloat::wrap {
}

# ------------------------------------------------------------------
#                            METHODS
# ------------------------------------------------------------------

# ------------------------------------------------------------------
# METHOD: up
#
# Up arrow button press event.  Increment value in entry.
# ------------------------------------------------------------------
itcl::body iwidgets::Spinfloat::up {} {
    set min_range [lindex $itk_option(-range) 0]
    set max_range [lindex $itk_option(-range) 1]
    
    set val [$itk_component(entry) get]
    if {[lindex $itk_option(-range) 0] != ""} {
        
        #
        # Check boundaries.
        #
        if {$val >= $min_range && $val < $max_range} {
            set val [expr $val + $itk_option(-step)]
            $itk_component(entry) delete 0 end
            $itk_component(entry) insert 0 $val
        } else {
            if {$itk_option(-wrap)} {
                if {$val >= $max_range} {
                    $itk_component(entry) delete 0 end
                    $itk_component(entry) insert 0 $min_range 
                } elseif {$val < $min_range} {
                    $itk_component(entry) delete 0 end
                    $itk_component(entry) insert 0 $min_range 
                } else {
                    uplevel #0 $itk_option(-invalid)
                }
            } else {
                uplevel #0 $itk_option(-invalid)
            }
        }
    } else {
        
        #
        # No boundaries.
        #
        set val [expr $val + $itk_option(-step)]
        $itk_component(entry) delete 0 end
        $itk_component(entry) insert 0 $val
    }
}

# ------------------------------------------------------------------
# METHOD: down 
#
# Down arrow button press event.  Decrement value in entry.
# ------------------------------------------------------------------
itcl::body iwidgets::Spinfloat::down {} {
    set min_range [lindex $itk_option(-range) 0]
    set max_range [lindex $itk_option(-range) 1]
    
    set val [$itk_component(entry) get]
    if {[lindex $itk_option(-range) 0] != ""} {
        
        #
        # Check boundaries.
        #
        if {$val > $min_range && $val <= $max_range} {
            set val [expr $val - $itk_option(-step)]
            $itk_component(entry) delete 0 end
            $itk_component(entry) insert 0 $val
        } else {
            if {$itk_option(-wrap)} {
                if {$val <= $min_range} {
                    $itk_component(entry) delete 0 end
                    $itk_component(entry) insert 0 $max_range
                } elseif {$val > $max_range} {
                    $itk_component(entry) delete 0 end
                    $itk_component(entry) insert 0 $max_range
                } else {
                    uplevel #0 $itk_option(-invalid)
                }
            } else {
                uplevel #0 $itk_option(-invalid)
            }
        }
    } else {
        
        #
        # No boundaries.
        #
        set val [expr $val - $itk_option(-step)]
        $itk_component(entry) delete 0 end
        $itk_component(entry) insert 0 $val
    }
}
