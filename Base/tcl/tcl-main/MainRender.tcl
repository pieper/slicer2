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
    global Video viewWin Twin twinWin View Slice

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

    if { $View(movie) > 0 && $View(movieSlices) == 0} {
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
    } elseif { $View(movie) > 0 && $View(movieSlices) > 0} {

        # first append the 3 slices horizontally
        vtkImageAppend imAppendSl
        imAppendSl SetAppendAxis 0
        foreach s $Slice(idList) {
        vtkWindowToImageFilter IFSl$s
        IFSl$s SetInput sl${s}Win
        imAppendSl AddInput [IFSl$s GetOutput]
        }

        set w [winfo width .tViewer]
        # translate if viewer width is bigger
        vtkImageTranslateExtent imTrans
        imTrans SetTranslation [expr ($w - 768)/2] 0 0
        imTrans SetInput [imAppendSl GetOutput]
        #pad them with the width of the viewer
        vtkImageConstantPad imPad
        imPad SetInput [imTrans GetOutput]
        imPad SetOutputWholeExtent 0 $w 0 256 0 0
        
        
        # then append the image of the 3 slices to the viewWin screen
        # vertically
        vtkImageAppend imAppendAll
        imAppendAll SetAppendAxis 1
        vtkWindowToImageFilter IFVW
        IFVW SetInput $viewWin
        imAppendAll AddInput [imPad GetOutput]
        imAppendAll AddInput [IFVW GetOutput]



        #vtkImageClip imClip
        #imClip SetInput [imAppendSl GetOutput]
        #set h [winfo height .tViewer]
        #set w [winfo width .tViewer]
        #puts "width $w height $h"
        #imClip SetOutputWholeExtent 0 $w 0 $h 0 0
        #imClip ReleaseDataFlagOff

        

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
            vtkTIFFWriter writer
            writer SetInput [imAppendAll GetOutput]
            writer SetFileName $filename
            writer Write
            
            writer Delete
        }
        "BMP" {
            vtkBMPWriter writer
            writer SetInput [imAppendAll GetOutput]
            writer SetFileName $filename
            writer Write
            
            writer Delete
        }
        }
        incr View(movieFrame)

        imAppendSl Delete
        imAppendAll Delete
        IFVW Delete
        IFSl0 Delete
        IFSl1 Delete
        IFSl2 Delete
        imPad Delete
        imTrans Delete
        #imClip Delete
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

    foreach s $Slice(idList) {
    RenderSlice $s
    }
    # render3d last in case we want the newly rendered slices in the movie
    Render3D
    
}
 
#-------------------------------------------------------------------------------
# .PROC RenderBoth
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RenderBoth {s {scale ""}} {

    RenderSlice $s
    # render3d last in case we want the newly rendered slices in the movie
    Render3D
}
