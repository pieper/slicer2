#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        MainRender.tcl
# PROCEDURES:  
#   Render3D
#   RenderSlice
#   RenderActive
#   RenderSlices
#   RenderAll
#   RenderBoth
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC Render3D
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Render3D {{scale ""}} {
	global Video viewWin Twin twinWin

	$viewWin Render
	
	if {$Twin(mode) == "On"} {
		Twin(src) Modified
		Twin(mapper) Modified
		$twinWin Render
	}

	if {[IsModule Video] == 1 && $Video(record) == "On"} {
		VideoSave
	}
}

#-------------------------------------------------------------------------------
# .PROC RenderSlice
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderSlice {s {scale ""}} {
	global Slice View Interactor

	# Clear que of render events to make panning much more interactive
	update
	
	sl${s}Win Render

	if {$s == $Interactor(s)} {
		if {$View(createMagWin) == "Yes" && $View(closeupVisibility) == "On"} {
			magWin Render
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC RenderActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderActive {{scale ""}} {
	global Slice 

	RenderSlice $Slice(activeID)
}

#-------------------------------------------------------------------------------
# .PROC RenderSlices
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderSlices {{scale ""}} {
	global Slice 

	foreach s $Slice(idList) {
		RenderSlice $s
	}
}

#-------------------------------------------------------------------------------
# .PROC RenderAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderAll { {scale ""}} {
	global Slice

	Render3D
	foreach s $Slice(idList) {
		RenderSlice $s
	}
}
 
#-------------------------------------------------------------------------------
# .PROC RenderBoth
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderBoth {s {scale ""}} {

	RenderSlice $s
	Render3D
}
