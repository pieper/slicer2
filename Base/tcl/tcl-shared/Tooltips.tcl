#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        Tooltips.tcl
# PROCEDURES:  
#   TooltipAdd widget tip
#   TooltipEnterWidget
#   TooltipExitWidget
#   TooltipPopUp
#   TooltipPopDown
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC TooltipAdd
# Call this procedure to add a tooltip (floating help text) to a widget. 
# The tooltip will pop up over the widget when the user leaves the mouse
# over the widget for a little while.
# .ARGS
# str widget name of the widget
# str tip text that you would like to appear as a tooltip.
# .END
#-------------------------------------------------------------------------------
proc TooltipAdd {widget tip} {

    # surround the tip string with brackets
    set tip "\{$tip\}"

    # bindings
    bind $widget <Enter> "TooltipEnterWidget %W $tip %X %Y"
    bind $widget <Leave> TooltipExitWidget
}


#-------------------------------------------------------------------------------
# .PROC TooltipEnterWidget
# Internal procedure for Tooltips.tcl.  Called when the mouse enters the widget.
# This proc works with TooltipExitWidget to pop up the tooltip after a delay.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TooltipEnterWidget {widget tip X Y} {
    global Tooltips

    # set Tooltips(stillOverWidget) after a delay.
    set id [after 1000 {set Tooltips(stillOverWidget) 1}]

    # wait until Tooltips(stillOverWidget) is set (by us or by exiting the widget).
    # vwait allows event loop to be entered.
    vwait Tooltips(stillOverWidget)

    # if Tooltips(stillOverWidget) is 1, the mouse is still over widget.
    # So pop up the tooltip!
    if {$Tooltips(stillOverWidget) == 1} {
	TooltipPopUp $widget $tip $X $Y
    } else {
	# the mouse exited the widget already, so cancel the waiting.
	after cancel $id
    }

}

#-------------------------------------------------------------------------------
# .PROC TooltipExitWidget
# Internal procedure for Tooltips.tcl.  Called when the mouse exits the widget. 
# This proc works with TooltipEnterWidget to pop up the tooltip after a delay.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TooltipExitWidget {} {
    global Tooltips

    # mouse is not over the widget anymore, so stop the vwait.
    set Tooltips(stillOverWidget) 0

    # remove the tooltip if there is one.
    TooltipPopDown
}

#-------------------------------------------------------------------------------
# .PROC TooltipPopUp
# Internal procedure for Tooltips.tcl.  Causes the tooltip window to appear. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TooltipPopUp {widget tip X Y} {
    global Tooltips Gui

    # set tooltip window name
    if {[info exists Tooltips(window)] == 0} {
	set Tooltips(window) .wTooltips
    }

    # get rid of any other existing tooltip
    TooltipPopDown

    # make a new tooltip window
    toplevel $Tooltips(window)

    # add an offset to make tooltips fall below cursor
    set Y [expr $Y+15]

    # don't let the window manager "reparent" the tip window
    wm overrideredirect $Tooltips(window) 1

    # display the tip text...
    wm geometry $Tooltips(window) +${X}+${Y}
    eval {label $Tooltips(window).l -text $tip } $Gui(WTTA)
    pack $Tooltips(window).l -in $Tooltips(window)
}

#-------------------------------------------------------------------------------
# .PROC TooltipPopDown
# Internal procedure for Tooltips.tcl.  Removes the tooltip window. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TooltipPopDown {} {
    global Tooltips

    catch {destroy $Tooltips(window)}
}