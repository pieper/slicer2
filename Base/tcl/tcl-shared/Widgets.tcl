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
# FILE:        Widgets.tcl
# PROCEDURES:  
#   ScrolledText
#   TabbedFrame arrayName buttonsFrame tabsFrame buttonsLabel tabs titles tooltips
#   TabbedFrameTab
#==========================================================================auto=

# This file contains widgets that may be useful to developers
# of Slicer modules.
#
#   ScrolledText     A text box with a scrollbar
#
#   Tabbed Frame     Makes button navigation menu and multiple frames ("tabs")


#-------------------------------------------------------------------------------
# .PROC ScrolledText
# Makes a simple scrolled text box.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ScrolledText { f args } {
    global Gui

    frame $f -bg $Gui(activeWorkspace)
    eval {text $f.text \
	    -xscrollcommand [list $f.xscroll set] \
	    -yscrollcommand [list $f.yscroll set] -bg} $Gui(normalButton)
    eval {scrollbar $f.xscroll -orient horizontal \
	    -command [list $f.text xview]} $Gui(WSBA)
    eval {scrollbar $f.yscroll -orient vertical \
	    -command [list $f.text yview]} $Gui(WSBA)
    grid $f.text $f.yscroll -sticky news
    grid $f.xscroll -sticky news
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    return $f.text
}


#-------------------------------------------------------------------------------
# .PROC TabbedFrame
# This creates a menu of buttons and the frames they tab to.  
# It saves the path to each frame and creates an indicator
# variable to tell which is the current frame, and it puts
# these things into your global array, in case they are needed.
# Before calling this, you need
# to make a frame for the buttons to go into and also a frame
# for all the frames to go into, and pass these as parameters.  It is
# very important to make the frame to hold frames have height 310 or
# things won't show up inside. See MeasureVol.tcl for an example of 
# how to use this. 
# .ARGS
# str arrayName        name of the global array for your module.
# str buttonsFrame     the frame you created for the buttons
# str tabsFrame        the frame you created to hold all tabs (height =310!)
# str buttonsLabel     label to go to left of button menu
# str tabs             list of the tabs to create. no whitespace in tab names.
# str titles           list of titles corresponding to each tab.  shown on buttons.
# str tooltips         list of tooltips to appear over buttons. optional.
# .END
#-------------------------------------------------------------------------------
proc TabbedFrame {arrayName containerFrame buttonsLabel tabs titles \
	{tooltips ""} {extraTopFrame "0"} } {
    global Gui Widgets $arrayName

    # get the global array.
    upvar #0 $arrayName globalArray

    #-------------------------------------------
    # Container frame
    #-------------------------------------------
    set f $containerFrame

    ###### make the top frame (sunken with dark background) ####
    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fTop -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    set fTop $f.fTop

    ##### make the bottom frame (lighter colored) ######
    # height 310 needed to put the tab-to frames inside!
    frame $f.fTabbedFrame -bg $Gui(activeWorkspace) -height 310
    pack $f.fTabbedFrame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    set fBottom $f.fTabbedFrame   


    #-------------------------------------------
    # Container->Top frame
    #-------------------------------------------
    set f $fTop
    set topFrames ""
    if {$extraTopFrame == 1} {
	lappend topFrames Extra
    }
    lappend topFrames Buttons

    foreach frame $topFrames {
	frame $f.fTabbedFrame$frame -bg $Gui(backdrop)
	pack $f.fTabbedFrame$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
	set f$frame $f.fTabbedFrame$frame
    }

    #-------------------------------------------
    # Container->Top->Extra frame
    #-------------------------------------------

    # With the extra frame, users can add their own things 
    # above the buttons for tabbing.
    # For example, MeasureVol puts in a Volume Select menu.
    if {$extraTopFrame == 1} {
	set globalArray(TabbedFrame,$containerFrame,extraFrame) $fExtra
    }

    #-------------------------------------------
    # Container->Top->Buttons frame
    #-------------------------------------------
    set f $fButtons

    # set button width to max word length
    set width 0
    foreach title $titles {
	if {[expr [string length $title] +2] > $width} {
	    set width [expr [string length $title] +2]
	}
    }
    # label next to the buttons
    eval {label $f.l -text $buttonsLabel} $Gui(BLA)

    # make buttons for navigation btwn frames
    frame $f.f -bg $Gui(backdrop)
    foreach tab $tabs title $titles {
	eval {radiobutton $f.f.r$tab \
		-text "$title" \
		-command "TabbedFrameTab $fBottom.f$tab" \
		-variable "$arrayName\(TabbedFrame,$containerFrame,tab)" \
		-value $tab -width  $width \
		-indicatoron 0} $Gui(WCA)
	pack $f.f.r$tab -side left -padx 0
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    # add tooltips, if there's one for every tab
    if {[llength $tooltips] == [llength $tabs]} {
	foreach tab $tabs tip $tooltips {
	    TooltipAdd $f.f.r$tab $tip
	}
    }

    #-------------------------------------------
    # Container->Bottom frame
    #-------------------------------------------
    set f $fBottom
    
    foreach tab $tabs {
	frame $f.f$tab -bg $Gui(activeWorkspace)
	place $f.f$tab -in $f -relheight 1.0 -relwidth 1.0
	set globalArray(TabbedFrame,$containerFrame,f$tab) $f.f$tab
    }


    ###### Make the first tab active #####
    set first [lindex $tabs 0]
    # press first button
    set globalArray(TabbedFrame,$containerFrame,tab) $first
    # go to first tab
    raise $fBottom.f$first
}

#-------------------------------------------------------------------------------
# .PROC TabbedFrameTab
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TabbedFrameTab {frame} {

    raise $frame
    focus $frame

}
