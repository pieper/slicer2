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
#   SaveRendererToFile
#   SaveImageToFile
#   SaveGetFilename
#   SaveGetExtensionForImageType
#   SaveGetImageType
#   SaveGetSupportedImageTypes
#   SaveGetSupportedExtensions
#   SaveGetAllSupportedExtensions
#   SaveChooseDirectory
#   SaveDisplayOptionsWindow
#   SaveModeIsMovie
#   SaveModeIsStereo
#   SaveModeIsSingleView
#   SaveGetFileBase
#   SaveIncrementFrameCounter
#   Save3DImage
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
    global Save
    set m "Save"

    set Module($m,depend) ""
    set Module($m,overview) "Image and Window file saving routines"
    set Module($m,author) "Michael Halle, SPL"

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.12 $} {$Date: 2003/02/18 22:40:48 $}]

    SaveInitTables

    set Save(imageDirectory) "/tmp"
    set Save(imageFilePrefix) "slicer-"
    set Save(imageFrameCounter) 1
    set Save(imageFileType) PNG
    set Save(imageSaveMode) "Single view"
    set Save(imageOutputZoom) 1
    set Save(imageIncludeSlices) 0
    set Save(stereoDisparityFactor) 1.0
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
    set imtype(ppm)  "PNM"
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
    set ext(PNM)  {"ppm" "pnm"}
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
# str filename Filename to save image to (see SaveGetFilename).
# str imageType Type of image to save (see SaveGetFilename).
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

#-------------------------------------------------------------------------------
# .PROC SaveRendererToFile
# 
# Saves a given renderer's image to a file of the given name.  The filename 
# should not include an image type;  the appropriate type for the
# files imageType will be appended.  If no window is given, the main
# 3D window will be saved.  The image can be magnified when output
# to produce a larger, higher-quality image for publication.
#
#
# .ARGS
# str directory Directory containing file;  if empty no directory is added.
# str filename Filename to save image to (see SaveGetFilename).
# str imageType Type of image to save (see SaveGetFilename).
# int mag Magnification of the image (uses vtkRenderLargeImage).
# str window Name of window to save, defaults to viewWin.
# .END
#-------------------------------------------------------------------------------
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
# str filename Filename to save image to (see SaveGetFilename).
# str imageType Type of image to save (see SaveGetFilename).
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

    set filename [SaveGetFilename $directory $filename $imageType]
    vtk${imageType}Writer saveWriter
    saveWriter SetInput $image
    saveWriter SetFileName $filename
    saveWriter Write
    saveWriter Delete
}

#-------------------------------------------------------------------------------
# .PROC SaveGetFilename
# 
# Returns the complete path+filebase+extension for a given filename.
#
#
# .ARGS
# str directory Directory containing file;  if empty no directory is added.
# str filename Filename to save image to.  If it has no extension, an appropriate one will be added.

# str imageType Type of image to save (BMP, JPEG, PNG, PNM, PostScript, TIFF). If empty, image type will be determined from the filename
# .END
#-------------------------------------------------------------------------------
proc SaveGetFilename {directory filename {imageType ""}} {
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
# Returns the canonical image type given a filename, file extension,
# or image type.  Only supported image types are returned; if an image
# type isn't supported, the empty string is returned.
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
# .PROC SaveGetSupportedExtensions
# 
#  Return a list of primary extensions that correspond to supported image types
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveGetSupportedExtensions {} {
    global SaveExtensionToImageTypeMap SaveImageTypeToExtensionMap

    foreach i [array names SaveImageTypeToExtensionMap] {
        lappend e [lindex $SaveImageTypeToExtensionMap($i) 0]
    }
    return [lsort $e]
}

#-------------------------------------------------------------------------------
# .PROC SaveGetAllSupportedExtensions
# 
#  Return a list of all extensions that correspond to supported image types
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveGetAllSupportedExtensions {} {
    global SaveExtensionToImageTypeMap
    return [lsort [array names SaveExtensionToImageTypeMap]]
}

#-------------------------------------------------------------------------------
# .PROC SaveChooseDirectory
# 
#  Internal function used to select the Save directory.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveChooseDirectory {} {
    global View Save
    set newdir [tk_chooseDirectory -initialdir $Save(imageDirectory)]
    puts "$newdir"
    if {"$newdir" != ""} {
        set Save(imageDirectory) $newdir
    }
}

#-------------------------------------------------------------------------------
# .PROC SaveDisplayOptionsWindow
# 
#  Displays a floating dialog box that allows image saving parameters to
#  be modified.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveDisplayOptionsWindow {{toplevelName .saveOptions}} {
    global Save Gui
    if {[winfo exists $toplevelName]} {
        wm deiconify $toplevelName
        raise $toplevelName
        return
    }
    set root [toplevel $toplevelName]
    wm title $root "Save 3D View Options"

    set f [frame $root.fSaveOptions -relief flat -border 2]
    pack $f -fill both -expand true
    GuiApplyStyle WFA $f

    grid [tkSpace $f.space20 -height 5] -columnspan 2

    label $f.lSaveMode -text "Save Mode:"
    eval tk_optionMenu $f.mbSaveMode Save(imageSaveMode) {"Single view" "Stereo pair" "Movie"}
    GuiApplyStyle WMBA $f.mbSaveMode
    $f.mbSaveMode config -pady 3
    GuiApplyStyle WMA $f.mbSaveMode.menu


    grid $f.lSaveMode $f.mbSaveMode -sticky w
    grid $f.lSaveMode -sticky e -padx $Gui(pad)
    GuiApplyStyle WLA $f.lSaveMode

    grid [tkHorizontalLine $f.line0] -columnspan 2 -pady 5 -sticky we

    label $f.lFileOptionsTitle -text "File Options"
    GuiApplyStyle WTA $f.lFileOptionsTitle
    grid $f.lFileOptionsTitle -sticky w -columnspan 2
    grid [tkSpace $f.space0 -height 5] -columnspan 2

    label $f.lDir -text "Directory:"
    GuiApplyStyle WLA $f.lDir

    entry $f.eDir -width 16 -textvariable Save(imageDirectory)
    GuiApplyStyle WEA $f.eDir

    grid $f.lDir $f.eDir -sticky w
    grid config $f.lDir -sticky e -padx $Gui(pad)

    button $f.bChooseDir -text "Browse..." -command SaveChooseDirectory
    GuiApplyStyle WBA $f.bChooseDir
    grid [tkSpace $f.space3] $f.bChooseDir -sticky w 

    grid [tkSpace $f.space4 -height 5] -columnspan 2

    label $f.lPrefix -text "File prefix:"
    GuiApplyStyle WLA $f.lPrefix
    entry $f.ePrefix -width 16 -textvariable Save(imageFilePrefix)
    GuiApplyStyle WEA $f.ePrefix

    grid $f.lPrefix $f.ePrefix -sticky w  -pady $Gui(pad)
    grid config $f.lPrefix -sticky e  -padx $Gui(pad)

    label $f.lFrame -text "Next frame #:"
    GuiApplyStyle WLA $f.lFrame
    entry $f.eFrame -width 6 -textvariable Save(imageFrameCounter)
    GuiApplyStyle WEA $f.eFrame

    grid $f.lFrame $f.eFrame -sticky w  -pady $Gui(pad)

    grid config $f.lFrame -sticky e  -padx $Gui(pad)

    label $f.lFileType -text "File type:"
    GuiApplyStyle WLA $f.lFileType
    eval tk_optionMenu $f.mbFileType Save(imageFileType) [SaveGetSupportedImageTypes]
    GuiApplyStyle WMBA $f.mbFileType
    $f.mbFileType config -pady 3
    GuiApplyStyle WMA $f.mbFileType.menu

    grid $f.lFileType $f.mbFileType -sticky w  -pady $Gui(pad)
    grid config $f.lFileType -sticky e  -padx $Gui(pad)

    grid [tkHorizontalLine $f.line1] -columnspan 2 -pady 5 -sticky we

    label $f.lSaveTitle -text "Save Options" -anchor w
    grid $f.lSaveTitle -sticky news -columnspan 1
    GuiApplyStyle WTA $f.lSaveTitle

    label $f.lScale -text "Output zoom:"
    GuiApplyStyle WLA $f.lScale
    $f.lScale config -anchor sw

    eval scale $f.sScale -from 1 -to 8 -orient horizontal \
        -variable Save(imageOutputZoom) $Gui(WSA) -showvalue true

    TooltipAdd $f.sScale "Renders the image in multiple pieces toproduce a higher resolution image (useful for publication)." 
    grid $f.lScale $f.sScale -sticky w 
    grid $f.lScale -sticky sne -ipady 10  -padx $Gui(pad)

    label $f.lStereo -text "Stereo disparity:"
    GuiApplyStyle WLA $f.lStereo
    entry $f.eStereo -width 6 -textvariable Save(stereoDisparityFactor)
    GuiApplyStyle WEA $f.eStereo

    TooltipAdd $f.eStereo "Changes the disparity (apparent depth) of the stereo image by this scale factor."

    grid $f.lStereo $f.eStereo -sticky w   -pady $Gui(pad)
    grid $f.lScale $f.lStereo -sticky e  -padx $Gui(pad)

    checkbutton $f.cIncludeSlices -text "Include slice windows" -indicatoron 1 -variable Save(imageIncludeSlices)
    GuiApplyStyle WCA $f.cIncludeSlices
    grid $f.cIncludeSlices -sticky we -columnspan 2

    grid [tkHorizontalLine $f.line10] -columnspan 2 -pady 5 -sticky we
    grid [tkSpace $f.space2 -height 10] -columnspan 2
    button $f.bCloseWindow -text "Close" -command "destroy $root"
    button $f.bSaveNow     -text "Save View Now" -command "Save3DImage"
    GuiApplyStyle WBA $f.bSaveNow $f.bCloseWindow
    grid $f.bCloseWindow $f.bSaveNow -sticky we -padx $Gui(pad) -pady $Gui(pad) -ipadx 2 -ipady 5

    grid columnconfigure $f 0 -weight 1
    grid columnconfigure $f 1 -weight 1
    grid columnconfigure $f 2 -weight 1

    return $root
}

#-------------------------------------------------------------------------------
# .PROC SaveModeIsMovie
# 
#  Returns 1 if the current save mode is movie mode, 0 otherwise.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc SaveModeIsMovie {} {
    global Save
    return [expr {"$Save(imageSaveMode)" == "Movie"}]
}

#-------------------------------------------------------------------------------
# .PROC SaveModeIsStereo
# 
#  Returns 1 if the current save mode is stereo pair mode, 0 otherwise.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveModeIsStereo {} {
    global Save
    return [expr {"$Save(imageSaveMode)" == "Stereo pair"}]
}

#-------------------------------------------------------------------------------
# .PROC SaveModeIsSingleView
# 
#  Returns 1 if the current save mode is single view mode, 0 otherwise.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveModeIsSingleView {} {
    global Save
    return [expr {"$Save(imageSaveMode)" == "Single view"}]
}

#-------------------------------------------------------------------------------
# .PROC SaveGetFileBase
# 
#  Returns the image file base name, checking to see if the frame number
#  is really a number.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc SaveGetFileBase {} {
    global Save
    if {! [ValidateInt $Save(imageFrameCounter)]} {
        return $Save(imageFilePrefix)
    } else {
        return [format "%s%04d" $Save(imageFilePrefix) $Save(imageFrameCounter)]
    }
}

#-------------------------------------------------------------------------------
# .PROC SaveIncrementFrameCounter
# 
#  If the frame counter is an int, increment it.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SaveIncrementFrameCounter {} {
    global Save
    if {[ValidateInt $Save(imageFrameCounter)]} {
        incr Save(imageFrameCounter)
    }
}

#-------------------------------------------------------------------------------
# .PROC Save3DImage
# 
#  Save the main viewer window using the current Save image options.
#  This function will be called automatically in movie mode, when the
#  "Save View Now" button is pressed, or when "Control-s" is pressed
#  over the view window.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc Save3DImage {} {
    global Save Slice viewWin Gui

    set filebase [SaveGetFileBase]
    set filename [SaveGetFilename $Save(imageDirectory) \
                      $filebase $Save(imageFileType)]

    $Gui(fViewer) config -cursor watch
    puts -nonewline "Save3DImage: writing $filename..."    
    if { [SaveModeIsSingleView] || [SaveModeIsMovie] } {

        if { $Save(imageIncludeSlices) == 0} {
            # no slices, just 3D image
            SaveRendererToFile $Save(imageDirectory) $filebase \
                $Save(imageFileType) $Save(imageOutputZoom) viewRen
            
        } else { 
            # save slices too

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

            SaveImageToFile $Save(imageDirectory) $filebase \
                $Save(imageFileType) [imAppendAll GetOutput]

            imAppendSl Delete
            imAppendAll Delete
            IFVW Delete
            IFSl0 Delete
            IFSl1 Delete
            IFSl2 Delete
            imPad Delete
            imTrans Delete
        }
    } elseif { [SaveModeIsStereo] } {
        global viewWin

        catch {stereoPairImage Delete}
        catch {stereoImage_left Delete}
        catch {stereoImage_right Delete}

        set renderer viewRen
        set window   $viewWin

        set cam [$renderer GetActiveCamera]
        set savecam [::SimpleStereo::formatCameraParams $cam]
    
        set views {right left}

        vtkImageAppend stereoPairImage
        stereoPairImage SetAppendAxis 0

        set disparity [expr {30.0/$Save(stereoDisparityFactor)}]
        set magnification $Save(imageOutputZoom)
        foreach v $views {
            ::SimpleStereo::moveCameraToView viewRen $v $disparity
            set image stereoImage_${v}
            
            if {$magnification == 1} {
                Render3D
                vtkWindowToImageFilter $image
                $image SetInput $window
            } else {
                vtkRenderLargeImage $image
                $image SetMagnification $magnification
                $image SetInput $renderer
            }
            $image Update
            stereoPairImage AddInput [$image GetOutput]
            ::SimpleStereo::restoreCameraParams $cam $savecam
        }

        SaveImageToFile $Save(imageDirectory) $filebase \
            $Save(imageFileType) [stereoPairImage GetOutput]

        Render3D
        stereoPairImage Delete
        stereoImage_left Delete
        stereoImage_right Delete
    }

    puts "done."
    $Gui(fViewer) config -cursor {}
    SaveIncrementFrameCounter
}
