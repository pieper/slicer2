#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
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
# If $View(movie) > 0, saves the frame for a movie
#
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Render3D {{scale ""}} {
	global Video viewWin Twin twinWin View

	$viewWin Render
	
	if {[IsModule Twin] == 1 && $Twin(mode) == "On"} {
		Twin(src) Modified
		Twin(src) Update
		Twin(mapper) Modified
		$twinWin Render
	}

	if {[IsModule Video] == 1 && $Video(record) == "On"} {
		VideoSave
	}

	if { $View(movie) > 0 } {
		# Compute filename
		set ext(PPM)  ppm
		set ext(TIFF) tif
		set ext(BMP)  bmp
		set filename [format %04d.$ext($View(movieFileType)) $View(movieFrame)]
		set filename [file join $View(movieDirectory) $filename]
		# Write file
		switch $View(movieFileType) {
		"PPM" {
			$viewWin SetFileName $filename
			$viewWin SaveImageAsPPM
		}
		"TIFF" {
			vtkWindowToImageFilter filter
			filter SetInput $viewWin
			vtkTIFFWriter writer
			writer SetInput [filter GetOutput]
			writer SetFileName $filename
			writer Write
			filter Delete
			writer Delete
		}
		"BMP" {
			vtkWindowToImageFilter filter
			filter SetInput $viewWin
			vtkBMPWriter writer
			writer SetInput [filter GetOutput]
			writer SetFileName $filename
			writer Write
			filter Delete
			writer Delete
		}
		}
		incr View(movieFrame)
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
