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
# FILE:        DICOMHelper.tcl
# PROCEDURES:  
#   DICOMHelperLoad  a
#   DICOMHelperCreateVolumeNameFromFileName  the
#   DICOMHelperCreateMrmlNodeForVolume the the the the
#   DICOMHelperLoadNonSiemensMosaicVolume  a
#   DICOMHelperLoadStudy
#   DICOMHelperLoadSiemensMosaicVolume  a
#   DICOMHelperIsSiemensMosaic  a
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC DICOMHelperLoad 
# Loads DICOM volume(s). It returns 0 if successful; 1 otherwise. 
# .ARGS
# list fileNames a list of dicom file names
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperLoad {fileNames} {
    global DICOMHelper 

    set fName [lindex $fileNames 0]
    set type [DICOMHelperIsSiemensMosaic $fName]
    set rtVal 1

    if {$type == 1} {
        set rtVal [DICOMHelperLoadSiemensMosaicVolume $fileNames]
    } elseif {$type == 0} {
        set rtVal [DICOMHelperLoadNonSiemensMosaicVolume $fileNames]
    }

    return $rtVal 
}


#-------------------------------------------------------------------------------
# .PROC DICOMHelperCreateVolumeNameFromFileName 
# Creates a volume name from the file name 
# .ARGS
# string fileName the file name
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperCreateVolumeNameFromFileName {fileName} {

    set tail [file tail $fileName]
    set dot [string last "." $tail]
    set name [string replace $tail $dot $dot "-"] 

    return $name
}
 

#-------------------------------------------------------------------------------
# .PROC DICOMHelperCreateMrmlNodeForVolume
# Creates a mrml node for a vtkImageData object 
# .ARGS
# string volName the volume name
# vtkMrmlDataVolume volData the volume data
# string byteOrder the byte order of the data
# float zSpacing the spacing between two slices
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperCreateMrmlNodeForVolume {volName volData byteOrder zSpacing} {
    global DICOMHelper Volume Mrml

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    MainVolumesCreate $i

    # set the name and description of the volume
    $n SetName $volName 
    $n SetDescription $volName 

    Volume($i,node) SetScanOrder {IS} 
    Volume($i,node) SetLittleEndian $byteOrder 

    $volData Update 
    
    set spacing [$volData GetSpacing]
    set spc [$volData GetSpacing]
    set pixelWidth [lindex $spc 0]
    set pixelHeight [lindex $spc 1]
    set sliceThickness [lindex $spc 2]

    eval Volume($i,node) SetSpacing $pixelWidth $pixelHeight $zSpacing 
    Volume($i,node) SetNumScalars [$volData GetNumberOfScalarComponents]
    set ext [$volData GetWholeExtent]
    Volume($i,node) SetImageRange [expr 1 + [lindex $ext 4]] [expr 1 + [lindex $ext 5]]
    Volume($i,node) SetScalarType [$volData GetScalarType]
    Volume($i,node) SetDimensions [lindex [$volData GetDimensions] 0] \
        [lindex [$volData GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [Volume($i,node) GetScanOrder]

    Volume($i,vol) SetImageData $volData
    MainVolumesSetActive $i

    return $i
}


#-------------------------------------------------------------------------------
# .PROC DICOMHelperLoadNonSiemensMosaicVolume 
# Loads a non Siemens mosaic volume. It returns 0 if successful; 1 otherwise. 
# .ARGS
# list fileNames a list of dicom file names
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperLoadNonSiemensMosaicVolume {fileNames} {
    global DICOMHelper Volume 

    set len [llength $fileNames]
    if {$len < 2} {
        DevErrorWindow "The file doesn't represent a volume."
        return 1
    }

    set fName [lindex $fileNames 0]
    set dirName [file dirname $fName]
    DICOMHelperLoadStudy $dirName 

    return 0 
}


#-------------------------------------------------------------------------------
# .PROC DICOMHelperLoadStudy
# sp 2003-07-10 support for loading directories full of dicom
# images via the --load-dicom command line argument
# .ARGS 
# path dir start dir for loading
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperLoadStudy {dir {Pattern "*"}} {
    global DICOMHelper 

    set DICOMHelper(MRMLid) ""
    set DICOMHelper(volumeExtent) ""

    if { $dir != "" } {
        set files [glob -nocomplain $dir/*]
        set dirs [list $dir]
        foreach f $files {
            if { [file isdirectory $f] } {
                lappend dirs $f
            } 
        }

        foreach d $dirs { 
            if { ![file isdirectory $d] } {
                continue
            }
            VolumesSetPropertyType VolDicom
            MainVolumesSetActive NEW
            # Tab Volumes row1 Props
            set ::Volumes(DICOMStartDir) $d
            if { $d == $dir } {
                # if this is the top level dir, then only look for
                # files at this level to avoid duplicate loading
                set ::DICOMrecurse "false"
            } else {
                set ::DICOMrecurse "true"
            }
            DICOMSelectMain $::Volume(dICOMFileListbox) "autoload" $Pattern

            if { $::FindDICOMCounter != 0 } {
                VolumesSetPropertyType VolHeader
                if { [info exists ::Volume(seriesDesc)] &&
                        $::Volume(seriesDesc) != "" } {
                    set seriestag $::Volume(seriesDesc)
                } else {
                    set seriestag [file tail $d]
                }
                regsub -all " " $seriestag "_" seriestag
                regsub -all "." $seriestag "_" seriestag
                regsub -all "__" $seriestag "_" seriestag
                # set ::Volume(name) $seriestag-$::Volume(name)
                lappend DICOMHelper(MRMLid) [VolumesPropsApply]
            }

            RenderAll
            # Tab Data
            set ::Volume(dICOMFileList) ""
        }
    }

    set id [lindex $DICOMHelper(MRMLid) 0]
    set DICOMHelper(volumeExtent) [[::Volume($id,vol) GetOutput] GetWholeExtent]
}


#-------------------------------------------------------------------------------
# .PROC DICOMHelperLoadSiemensMosaicVolume 
# Loads Siemens mosaic volume(s). It returns 0 if successful; 1 otherwise. 
# .ARGS
# list fileNames a list of dicom file names
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperLoadSiemensMosaicVolume {fileNames} {
    global DICOMHelper SiemensMosaicReader 

    set rtVal [SiemensMosaicReaderLoad $fileNames] 
    set DICOMHelper(MRMLid) $SiemensMosaicReader(MRMLid)
    set DICOMHelper(volumeExtent) $SiemensMosaicReader(volumeExtent)

    return $rtVal
}


#-------------------------------------------------------------------------------
# .PROC DICOMHelperIsSiemensMosaic 
# Checks if this DICOM file is Siemens mosaic format. It returns 1 if yes; 
# 0 otherwise. 
# .ARGS
# path fileName a dicom file name
# .END
#-------------------------------------------------------------------------------
proc DICOMHelperIsSiemensMosaic {fileName} {
    global DICOMHelper 

    if {$fileName == ""} {
        return -1
    }

    vtkDCMParser parser

    set found [parser OpenFile $fileName]
    if {[string compare $found "0"] == 0} {
        puts stderr "Can't open file $fileName\n"
        parser Delete
        return -1
    }

    # (0x0008,0x0008) - image type
    if { [parser FindElement 0x0008 0x0008] == "1" } {
        set Length [lindex [split [parser ReadElement]] 3]
        set imageType [parser ReadText $Length]
        parser Delete

        set found [string first "MOSAIC" $imageType 0]
        return [expr {$found == -1 ? 0 : 1}]
    } else  {
        parser Delete
        return -1
    }
}

