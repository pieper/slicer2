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
#   SaveInit
#   SaveInitTables
#   SaveWindowToFile
#   SaveImageToFile
#   SaveGetFilePath
#   SaveGetExtensionForImageType
#   SaveGetImageType
#   SaveGetSupportedImageTypes
#   SaveGetSupportedExtensions
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC SaveInit
# 
# Register the module with slicer and set up.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc SaveInit {} {
    set m "Save"

    set Module($m,depend) ""
    set Module($m,overview) "Image and Window file saving routines"
    set Module($m,author) "Michael Halle, SPL"

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.8 $} {$Date: 2003/01/20 17:04:29 $}]

    SaveInitTables
}

#-------------------------------------------------------------------------------
# .PROC SaveInitTables
#  Initialize the type tables.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveInitTables {} {
    upvar \#0  SaveExtensionToImageTypeMap imtype

    set imtype(ps)   "Postscript"
    set imtype(eps)  "Postscript"
    set imtype(prn)  "PostScript"
    set imtype(tif)  "TIFF"
    set imtype(tiff) "TIFF"
    set imtype(pnm)  "PNM"
    set imtype(png)  "PNG"
    set imtype(bmp)  "BMP"
    set imtype(jpg)  "JPEG"
    set imtype(jpeg) "JPEG"

    upvar \#0 SaveImageTypeToExtensionMap ext

    # list of supported extensions, with preferred extension first.
    set ext(BMP)  "bmp"
    set ext(JPEG) {"jpg" "jpeg"}
    set ext(PNG)  "png"
    set ext(PNM)  {"pnm"}
    set ext(PostScript) {"ps" "eps" "prn"}
    set ext(TIFF) {"tif" "tiff"}

}
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
# str filename Filename to save image to (see SaveGetFilePath).
# str imageType Type of image to save (see SaveGetFilePath).
# str window Name of window to save, defaults to viewWin.
# .END
#-------------------------------------------------------------------------------

proc SaveWindowToFile {directory filename imageType {window ""}} {
    global viewWin
    if {"$window" == ""} {
        set window $viewWin
    }

    vtkWindowToImageFilter saveFilter
    saveFilter SetInput $window
    SaveImageToFile $directory $filename $imageType [saveFilter GetOutput]
    saveFilter Delete
}

proc SaveRendererToFile {directory filename imageType {mag 1} {renderer ""}} {
    global viewWin
    if {"$renderer" == ""} {
        set renderer viewRen
    }

    set renwin [$renderer GetRenderWindow]

    if {$mag == 1} {
        # one-to-one magnification, simplify
        SaveWindowToFile $directory $filename $imageType $renwin
    } else {
        # render image in pieces using vtkRenderLargeImage
        vtkRenderLargeImage saveLargeImage
        saveLargeImage SetMagnification $mag
        saveLargeImage SetInput $renderer

        # save to file
        SaveImageToFile $directory $filename $imageType \
            [saveLargeImage GetOutput]
        saveLargeImage Delete

        # re-render scene in normal mode
        $renwin Render
    }
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
# str filename Filename to save image to (see SaveGetFilePath).
# str imageType Type of image to save (see SaveGetFilePath).
# str image The image to save.
# .END
#-------------------------------------------------------------------------------
proc SaveImageToFile {directory filename imageType image} {
    if {$imageType == ""} {
        set newImageType [SaveGetImageType $filename]
    } else {
        set newImageType [SaveGetImageType $imageType]  
    }

    if {"$newImageType" == ""} {
        error "unknown type for image $imageType"
    }
    set imageType $newImageType

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
# str filename Filename to save image to.  If it has no extension, an appropriate one will be added.

# str imageType Type of image to save (BMP, JPEG, PNG, PNM, PostScript, TIFF). If empty, image type will be determined from the filename
# .END
#-------------------------------------------------------------------------------
proc SaveGetFilePath {directory filename {imageType ""}} {
    global SaveImageTypeToExtensionMap SaveExtensionToImageTypeMap

    if {$imageType == ""} {
        set newImageType [SaveGetImageType $filename]
        if {"$newImageType" == ""} {
            error "unknown type for image $imageType"
        }
        set imageType $newImageType
    } else {
        #explicit image type, add extension (if different)
        set imageType [SaveGetImageType $imageType]
        set ext [lindex $SaveImageTypeToExtensionMap($imageType) 0]
        set curExt [string tolower [string range [file extension $filename] 1 end]]
        if {"$ext" != "$curExt"} {
            set filename [format "%s.%s" $filename $ext]
        }
    }
    
    if {"$directory" != ""} {
        set filename [file join $directory $filename]
    }

    return $filename
}

#-------------------------------------------------------------------------------
# .PROC SaveGetExtensionForImageType
# 
# Returns the appropriate file extension (no ".") for a given image type.
#
# .ARGS
# str imageType Type of image.
# .END
#-------------------------------------------------------------------------------

proc SaveGetExtensionForImageType {imageType} {
    global SaveExtensionToImageTypeMap
    if {[info exists SaveImageTypeToExtensionMap($imageType)]} {
        return [lindex $SaveImageTypeToExtensionMap($imageType) 0]
    }
    return ""
}
#-------------------------------------------------------------------------------
# .PROC SaveGetImageType
# 
# Returns the canonical image type given a filename, file extension, or image type.
# Only supported image types are returned; if an image type isn't supported, 
# the empty string is returned.
#
# .ARGS
# str imageType the image type, filename, or file extension (with or without .)
# .END
#-------------------------------------------------------------------------------

proc SaveGetImageType {imageTypeOrExt} {
    global SaveImageTypeToExtensionMap SaveExtensionToImageTypeMap
    # try the most straightforward map

    if {[info exists SaveImageTypeToExtensionMap($imageTypeOrExt)]} {
        return $imageTypeOrExt
    }

    # if not, see if we were handed an extension (or filename w/extension) instead
    set ext [file extension $imageTypeOrExt]
    
    if {"$ext" == ""} {
        # could be the extension, no "."
        set ext $imageTypeOrExt
    } else {
        set ext [string tolower [string range  $ext 1 end]]
    }

    if {[info exists SaveExtensionToImageTypeMap($ext)]} { 
        return $SaveExtensionToImageTypeMap($ext)
    }

    # no luck
    return ""
}

#-------------------------------------------------------------------------------
# .PROC SaveGetSupportedImageTypes
# 
#  Return a list of supported image types for saving.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveGetSupportedImageTypes {} {
    global SaveImageTypeToExtensionMap
    return [lsort [array names SaveImageTypeToExtensionMap]]
}

#-------------------------------------------------------------------------------
# .PROC SaveGetSupportedImageTypes
# 
#  Return a list of extensions that correspond to supported image types
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveGetSupportedExtensions {} {
    global SaveExtensionToImageTypeMap SaveImageTypeToExtensionMap

    foreach i [array names SaveImageTypeToExtensionMap] {
        lappend e [lindex $SaveImageTypeToExtensionMap($i) 0]
    }
    return $e
}

proc SaveGetAllSupportedExtensions {} {
    global SaveExtensionToImageTypeMap
    return [lsort [array names SaveExtensionToImageTypeMap]]
}
