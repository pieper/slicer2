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
# FILE:        Save.tcl
# PROCEDURES:  
#   SaveWindowToFile
#   SaveImageToFile
#   SaveGetFilePath
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC SaveWindowToFile
# 
# Saves a given window to a file of the given name.  The filename 
# should not include an image type;  the appropriate type for the
# files imageType will be appended.  If no window is given, the main
# 3D window will be saved.  
#
#
# .ARGS
# str directory Directory containing file;  if empty no directory is added.
# str filename Filename to save image to (no extension).
# str imageType Type of image to save (BMP, JPEG, PNG, PNM, PostScript, TIFF).
# str window Name of window to save, defaults to viewWin.
# .END
#-------------------------------------------------------------------------------

proc SaveWindowToFile {directory filename imageType {window ""}} {
    if {"$window" == ""} {
        set window $viewWin
    }

    vtkWindowToImageFilter saveFilter
    saveFilter SetInput $window
    saveImageToFile $directory $filename $imageType [saveFilter GetOutput]
    saveFilter Delete
}


#-------------------------------------------------------------------------------
# .PROC SaveImageToFile
# 
# Saves a given image to a file of the given name.  The filename 
# should not include an image type;  the appropriate type for the
# files imageType will be appended.  
#
#
# .ARGS
# str directory Directory containing file;  if empty no directory is added.
# str filename Filename to save image to (no extension).
# str imageType Type of image to save (BMP, JPEG, PNG, PNM, PostScript, TIFF).
# str image The image to save.
# .END
#-------------------------------------------------------------------------------
proc SaveImageToFile {directory filename imageType image} {

    set filename [SaveGetFilePath $directory $filename $imageType]

    vtk${imageType}Writer saveWriter
    saveWriter SetInput $image
    saveWriter SetFileName $filename
    saveWriter Write
    saveWriter Delete
}


#-------------------------------------------------------------------------------
# .PROC SaveGetFilePath
# 
# Returns the complete path for a given filename.
#
#
# .ARGS
# str directory Directory containing file;  if empty no directory is added.
# str filename Filename to save image to (no extension).
# str imageType Type of image to save (BMP, JPEG, PNG, PNM, PostScript, TIFF).
# .END
#-------------------------------------------------------------------------------
proc SaveGetFilePath {directory filename imageType} {
    set ext(BMP) bmp
    set ext(JPEG) jpg 
    set ext(PNG) png
    set ext(PNM) pnm
    set ext(PostScript) ps
    set ext(TIFF) tif

    set filename [format "%s.%s" $filename $ext($imageType)]

    if {"$directory" != ""} {
        set filename [file join $directory $filename]
    }

    return $filename
}
