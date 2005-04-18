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
# FILE:        notebook.tcl
# PROCEDURES:  
#   Notebook:create
#   Notebook:config
#   Notebook:click
#   Notebook:raise.page
#   Notebook:pageconfig
#   Notebook:raise
#   Notebook:frame
#   Notebook:scheduleExpand
#   Notebook:expand
#==========================================================================auto=
#=========================================================================
#
#  Library   : A Notebook widget for Tcl/Tk
#  Author    : D. Richard Hipp <drh@acm.org>
#  Web       : http://www.hwaci.com/drh/
#  Copyright : (C) 1996,1997,1998 D. Richard Hipp
#  Version   : $Revision: 1.2 $
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
#=========================================================================


#-------------------------------------------------------------------------------
# .PROC Notebook:create
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:create {w args} {
    global Notebook

    set Notebook($w,width) 400
    set Notebook($w,height) 300
    set Notebook($w,pages) {}
    set Notebook($w,top) 0
    set Notebook($w,pad) 5
    set Notebook($w,fg,on) black
    set Notebook($w,fg,off) grey50
    canvas $w -bd 0 -highlightthickness 0 -takefocus 0
    set Notebook($w,bg) [$w cget -bg]
    bind $w <1> "Notebook:click $w %x %y"
    bind $w <Configure> "Notebook:scheduleExpand $w"
    eval Notebook:config $w $args
}



#-------------------------------------------------------------------------------
# .PROC Notebook:config
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:config {w args} {
    global Notebook

    foreach {tag value} $args {
        switch -- $tag {
            -width {
                set Notebook($w,width) $value
            }
            -height {
                set Notebook($w,height) $value
            }
            -pages {
                set Notebook($w,pages) $value
            }
            -pad {
                set Notebook($w,pad) $value
            }
            -bg {
                set Notebook($w,bg) $value
            }
            -fg {
                set Notebook($w,fg,on) $value
            }
            -disabledforeground {
                set Notebook($w,fg,off) $value
            }
        }
    }

    $w delete all
    set Notebook($w,x1) $Notebook($w,pad)
    set Notebook($w,x2) [expr $Notebook($w,x1)+2]
    set Notebook($w,x3) [expr $Notebook($w,x2)+$Notebook($w,width)]
    set Notebook($w,x4) [expr $Notebook($w,x3)+2]
    set Notebook($w,y1) [expr $Notebook($w,pad)+2]
    set Notebook($w,y2) [expr $Notebook($w,y1)+2]
    set Notebook($w,y5) [expr $Notebook($w,y1)+30]
    set Notebook($w,y6) [expr $Notebook($w,y5)+2]
    set Notebook($w,y3) [expr $Notebook($w,y6)+$Notebook($w,height)]
    set Notebook($w,y4) [expr $Notebook($w,y3)+2]
    set x $Notebook($w,x1)
    set cnt 0
    set y7 [expr $Notebook($w,y1)+5]
    foreach p $Notebook($w,pages) {
        set Notebook($w,p$cnt,x5) $x
        set id [$w create text 0 0 -text $p -anchor nw -tags "p$cnt t$cnt"]
        set bbox [$w bbox $id]
        set width [lindex $bbox 2]
        $w move $id [expr $x+10] $y7
        $w create line \
            $x [expr $Notebook($w,y5)-8] \
            $x $Notebook($w,y2) \
            [expr $x+2] $Notebook($w,y1) \
            [expr $x+$width+16] $Notebook($w,y1) \
            -width 1 -fill white -tags p$cnt
        $w create line \
            [expr $x+$width+16] $Notebook($w,y1) \
            [expr $x+$width+18] $Notebook($w,y2) \
            [expr $x+$width+18] [expr $Notebook($w,y5)-8] \
            -width 1 -fill black -tags p$cnt
        set x [expr $x+$width+20]
        set Notebook($w,p$cnt,x6) [expr $x-2]
        if {![winfo exists $w.f$cnt]} {
            frame $w.f$cnt -bd 0
        }
        $w.f$cnt config -bg $Notebook($w,bg)
        place $w.f$cnt -x $Notebook($w,x2) -y [expr $Notebook($w,y6)-6] \
            -width $Notebook($w,width) -height $Notebook($w,height)
        incr cnt
    }
    $w create line \
        $Notebook($w,x1) [expr $Notebook($w,y5)-10] \
        $Notebook($w,x1) $Notebook($w,y3) \
        -width 1 -fill white
    $w create line \
        $Notebook($w,x1) $Notebook($w,y3) \
        $Notebook($w,x2) $Notebook($w,y4) \
        $Notebook($w,x3) $Notebook($w,y4) \
        $Notebook($w,x4) $Notebook($w,y3) \
        $Notebook($w,x4) $Notebook($w,y6) \
        $Notebook($w,x3) $Notebook($w,y5) \
        -width 1 -fill white 
    $w config -width [expr $Notebook($w,x4)+$Notebook($w,pad)] \
        -height [expr $Notebook($w,y4)+$Notebook($w,pad)] \
        -bg $Notebook($w,bg)
    set top $Notebook($w,top)
    set Notebook($w,top) -1
    Notebook:raise.page $w $top
}



#-------------------------------------------------------------------------------
# .PROC Notebook:click
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:click {w x y} {
    global Notebook

    if {$y<$Notebook($w,y1) || $y>$Notebook($w,y6)} return
    set N [llength $Notebook($w,pages)]
    for {set i 0} {$i<$N} {incr i} {
        if {$x>=$Notebook($w,p$i,x5) && $x<=$Notebook($w,p$i,x6)} {
            Notebook:raise.page $w $i
            break
        }
    }
}



#-------------------------------------------------------------------------------
# .PROC Notebook:raise.page
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:raise.page {w n} {
    global Notebook

    if {$n < 0 || $n >= [llength $Notebook($w,pages)]} {
        return
    }
    set top $Notebook($w,top)
    if {$top>=0 && $top<[llength $Notebook($w,pages)]} {
        $w move p$top 0 2
    }
    $w move p$n 0 -2
    $w delete topline
    if {$n > 0} {
        $w create line \
        $Notebook($w,x1) [expr $Notebook($w,y6)-8] \
        $Notebook($w,x2) [expr $Notebook($w,y5)-8] \
        $Notebook($w,p$n,x5) [expr $Notebook($w,y5)-8] \
        $Notebook($w,p$n,x5) [expr $Notebook($w,y5)-10] \
        -width 1 -fill white -tags topline
    }
    $w create line \
        $Notebook($w,p$n,x6) [expr $Notebook($w,y5)-10] \
        $Notebook($w,p$n,x6) [expr $Notebook($w,y5)-8] \
        -width 1 -fill white -tags topline

    $w create line \
        $Notebook($w,p$n,x6) [expr $Notebook($w,y5)-8] \
        $Notebook($w,x3) [expr $Notebook($w,y5)-8] \
        -width 1 -fill white -tags topline
 
    set Notebook($w,top) $n
    raise $w.f$n
}



#-------------------------------------------------------------------------------
# .PROC Notebook:pageconfig
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:pageconfig {w name args} {
    global Notebook

    set i [lsearch $Notebook($w,pages) $name]
    if {$i < 0} {
        return
    }
    foreach {tag value} $args {
        switch -- $tag {
            -state {
                if {"$value"=="disabled"} {
                    $w itemconfig t$i -fg $Notebook($w,fg,off)
                } else {
                    $w itemconfig t$i -fg $Notebook($w,fg,on)
                }
            }
            -onexit {
                set Notebook($w,p$i,onexit) $value
            }
        }
    }
}
 


#-------------------------------------------------------------------------------
# .PROC Notebook:raise
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:raise {w name} {
    global Notebook

    set i [lsearch $Notebook($w,pages) $name]
    if {$i < 0} {
        return
    }
    if {[info exists Notebook($w,p$i,onexit)]} {
        set onexit $Notebook($w,p$i,onexit)
        if {"$onexit"!="" && [eval uplevel #0 $onexit]!=0} {
            Notebook:raise.page $w $i
        }
    } else {
        Notebook:raise.page $w $i
    }
}



#-------------------------------------------------------------------------------
# .PROC Notebook:frame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:frame {w name} {
    global Notebook

    set i [lsearch $Notebook($w,pages) $name]
    if {$i >= 0} {
        return $w.f$i
    } else {
        return {}
    }
}



#-------------------------------------------------------------------------------
# .PROC Notebook:scheduleExpand
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:scheduleExpand w {
    global Notebook

    if {[info exists Notebook($w,expand)]} {
        return
    }
    set Notebook($w,expand) 1
    after idle "Notebook:expand $w"
}



#-------------------------------------------------------------------------------
# .PROC Notebook:expand
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Notebook:expand w {
    global Notebook

    set wi [expr [winfo width $w]-($Notebook($w,pad)*2+4)]
    set hi [expr [winfo height $w]-($Notebook($w,pad)*2+36)]
    Notebook:config $w -width $wi -height $hi
    catch {
        unset Notebook($w,expand)
    }
}
