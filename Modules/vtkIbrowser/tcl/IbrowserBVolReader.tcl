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
# FILE:        IbrowserBVolReader.tcl
# PROCEDURES:  
#   IbrowserBVolReaderSetFileName
#   IbrowserBVolReaderApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC IbrowserBVolReaderSetFileName
# Set the Volume(name) from the Volume(Ibrowser,FileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBVolReaderSetFileName {} {

    if {$::Module(verbose) == 1} {
        puts "selected filename: $::Volume(Ibrowser,FileName)"
    }
    set ::Volume(name) [file tail $::Volume(Ibrowser,FileName)]
    # replace . with -
    regsub -all {[.]} $::Volume(name) {-} ::Volume(name)
}




#-------------------------------------------------------------------------------
# .PROC IbrowserBVolReaderApply
# Check the file type variable and build the appropriate bfloat or bshort
# volume
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBVolReaderApply { } {
    global Ibrowser Module
    
    if {$::Module(verbose) == 1} {
        puts "proc IbrowserBVolReaderApply\n"
    }

    #-------------------------------------------
    #----- Create a bfloat/short reader,
    #----- set the file prefix, and call 
    #----- update on the reader. Assign
    #----- newData to hold the output.
    #----- (Executing the reader reads the
    #----- header file also.)
    #-------------------------------------------
    if {[info command IbrowserBVR] != ""} {
        DevErrorWindow "Deleting existing reader..."
        IbrowserBVR Delete
    }

    vtkBVolReader IbrowserBVR
    puts "made reader"
    IbrowserBVR SetDebug 1
    puts "set debug"

    set dname [file dirname $::Volume(Ibrowser,FileName)]
    set fname [ file tail $::Volume(Ibrowser,FileName)]
    set fstem [ file rootname $fname ]

    #IbrowserBVR SetFileName $fname
    #IbrowserBVR SetDirName $dname
    #IbrowserBVR SetStem $fstem
    IbrowserBVR SetFileName $::Volume(Ibrowser,FileName)

    puts "IbrowserBVolReaderApply: calling Update on volume."
    IbrowserBVR Update
    puts "IbrowserBVolReaderApply: newData now contains volume."
    #----- want this to be ibrowser data object.
    set newData [IbrowserBVR GetOutput ]



    #-------------------------------------------
    #----- add a mrml volume node
    #-------------------------------------------
    set n [MainMrmlAddNode Volume]
    puts "IbrowserBVolReaderApply: added volume mrml node."
    $n SetName $::Volume(name)
    puts "IbrowserBVolReaderApply: set volume name to be $::Volume(name)"
    set i [$n GetID]
    puts "IbrowserBVolReaderApply: mrmlID is $i."
    MainVolumesCreate $i 
    puts "IbrowserBVolReaderApply: stem is: [IbrowserBVR GetStem]."
    puts "IbrowserBVolReaderApply: filename is $fname"
    puts "IbrowserBVolReaderApply: dirname is $dname"

    eval Volume($i,node) SetSpacing [$newData GetSpacing]
    Volume($i,node) SetScanOrder PA
    Volume($i,node) SetNumScalars [$newData GetNumberOfScalarComponents]
    set ext [$newData GetWholeExtent]
    Volume($i,node) SetImageRange [lindex $ext 4] [lindex $ext 5]
    Volume($i,node) SetScalarType [$newData GetScalarType]
    Volume($i,node) SetDimensions [lindex [$newData GetDimensions] 0] [lindex [$newData GetDimensions] 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder [::Volume($i,node) GetScanOrder]
    Volume($i,node) SetName $::Volume(name)
    Volume($i,node) SetLabelMap $::Volume(labelMap)
    puts "IbrowserBVolReaderApply: volume parameters set."
    
  #-------------------------------------------
    #----- updating MRML and fail check
    #-------------------------------------------
    MainUpdateMRML
    if {[lsearch $::Volume(idList) $i] == -1} {
        puts "IbrowserBVolReader: read failed on volume $i, $::Volume(Ibrowser,FileName)"
        DevErrorWindow "IbrowserBVolReader: read failed on volume $i, $::Volume(Ibrowser,FileName)"
        IbrowserBVR Delete
        return -1
    }
    if {$::Module(verbose) == 1} {
        puts "IbrowserBVolReader: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[::Volume($i,node) GetPosition] GetElement 1 3]
        puts "IbrowserBVolReader: volume $i position 1 3: $badval"
    }

    #-------------------------------------------
    #----- data loaded, so update display
    #-------------------------------------------
    puts "New bfloat volume loaded.:"
    Volume($i,vol) SetImageData $newData
    
    #--- set the FOV for correct display of this volume
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]
    set fov     [expr $dim*$spacing]
    set View(fov) $fov
    MainViewSetFov

    #--- display the new volume in the background
    #--- if image data, ontop if a label map
    if {[Volume($i,node) GetLabelMap] == 1} {
        MainSlicesSetVolumeAll Label $i
    } else {
        MainSlicesSetVolumeAll Back $i
    }
    MainVolumesSetActive $i
    RenderAll
    
    #--- delete the reader and return mrml id
    IbrowserBVR Delete
    puts "IbrowserBVolReaderApply: finished."
    return i
}



