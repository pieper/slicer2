#-------------------------------------------------------------------------------
# .PROC RealignResampleInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleInit {} {
    global RealignResample Module Volume Line Matrix

    set m RealignResample
    set Module($m,overview) "This module realigns and resamples using vtkImageReslice"
    set Module($m,author) "Jacob Albertson, SPL, jacob@bwh.harvard.edu"
    set Module($m,category) "Alpha"

    set Module($m,row1List) "Fiducials RealignResample Help"
    set Module($m,row1Name) "{Fiducials} {Realign Resample} {Help}"
    set Module($m,row1,tab) RealignResample

    set Module($m,procGUI) RealignResampleBuildGUI
    set Module($m,procMRML) RealignResampleUpdateMRML
  
    set Module($m,depend) "Morphometrics"

    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.2 $} {$Date: 2004/08/16 21:20:20 $}]

    set Matrix(volume) $Volume(idNone)
    set Matrix(RealignResampleVolumeName2) None
    set RealignResample(SaveAs) None
    set RealignResample(ACPCList) None
    set RealignResample(MidlineList) None

}

proc RealignResampleUpdateMRML {} {
    global Matrix Volume Fiducials RealignResample

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $Matrix(volume) ] == -1} {
        RealignResampleSetVolume2 $n
    }

    # Menu of Volumes 
    # ------------------------------------
    set m $Matrix(mbVolume2).m
    $m delete 0 end
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone)} {
            $m add command -label "[Volume($v,node) GetName]" -command "RealignResampleSetVolume2 $v"
        }
    }
    # Menu ACPC
    # ------------------------------------
    set m $RealignResample(mbACPC).m
    $m delete 0 end
    $m add command -label "None" -command "RealignResampleSetACPCList None"
    foreach v $Fiducials(listOfNames) {
    $m add command -label "$v" -command "RealignResampleSetACPCList $v"
    }
    # Menu Midline
    # ------------------------------------
    set m $RealignResample(mbMidline).m
    $m delete 0 end
    $m add command -label "None" -command "RealignResampleSetMidlineList None"
    foreach v $Fiducials(listOfNames) {
    $m add command -label "$v" -command "RealignResampleSetMidlineList $v"
    }
}
# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------


proc RealignResampleBuildGUI {} {
    global Gui RealignResample Module Volume Line Matrix Fiducials
    
    set help "
    The RealignResample module realigns a volume based on a transformation matrix. It can also resample the volume by changing the extent and spacing. Finally it can make a transform using a list of fiducials. The source code is in slicer2/Modules/vtkRealignResample/tcl/RealignResample.tcl.
       "
    regsub -all "\n" $help {} help
    MainHelpApplyTags RealignResample $help
    MainHelpBuildGUI RealignResample

    #-------------------------------------------
    # Realign frame
    #-------------------------------------------
    set fFiducials $Module(RealignResample,fFiducials)
    set f $fFiducials
    
    foreach frame "Matrix Midline ACPC Calculate" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    $f.fCalculate config -relief groove -bd 3 
    #-------------------------------------------
    # Realign->Matrix
    #-------------------------------------------
    set f $fFiducials.fMatrix

    DevAddLabel $f.lActive "Matrix: " 
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20  -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady $Gui(pad)
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m
    
    #-------------------------------------------
    # Realign->Midline
    #-------------------------------------------   
    set f $fFiducials.fMidline
    
    DevAddLabel $f.lMidline "Midline List (>3):"
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20  -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    lappend Fiducials(mbActiveList) $f.mbActive
    lappend Fiducials(mActiveList)  $f.mbActive.m
    pack $f.lMidline $f.mbActive -side left -padx $Gui(pad) 
    set RealignResample(mbMidline) $f.mbActive

     
    #-------------------------------------------
    # Realign->ACPC
    #-------------------------------------------   
    set f $fFiducials.fACPC
    
    DevAddLabel $f.lACPC "ACPC List (2):"
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20  -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    lappend Fiducials(mbActiveList) $f.mbActive
    lappend Fiducials(mActiveList)  $f.mbActive.m
    pack $f.lACPC $f.mbActive -side left -padx $Gui(pad) 
    set RealignResample(mbACPC) $f.mbActive
        
    #-------------------------------------------
    # Realign->Calculate
    #-------------------------------------------
    set f $fFiducials.fCalculate
  
    DevAddButton $f.bCalculate "Calculate Transform"  RealignCalculate 
    pack $f.bCalculate  -side top -padx $Gui(pad) -pady $Gui(pad)
    
    #-------------------------------------------
    # Resample
    #-------------------------------------------
    
    set fRealignResample $Module(RealignResample,fRealignResample)
    set f $fRealignResample

    foreach frame "Volume Matrix NewVolume OutputSpacing SetSpacing AutoSpacing OutputExtent SetExtent AutoExtent InterpolationMode BeginResample SaveAs BeginSave" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady 0 -fill x
    }

    #-------------------------------------------
    # Resample->Volume
    #-------------------------------------------
    set f $fRealignResample.fVolume
    
    DevAddLabel $f.lVolume "Volume: "
    eval {menubutton $f.mbVolume -text "None" -relief raised -bd 2 -width 15 -menu $f.mbVolume.m} $Gui(WMBA)
    eval {menu $f.mbVolume.m} $Gui(WMA)
    
    pack $f.lVolume $f.mbVolume -side left -padx $Gui(pad) -pady $Gui(pad)
    set Matrix(mbVolume2) $f.mbVolume 

    #-------------------------------------------
    # Resample->Matrix
    #-------------------------------------------
    set f $fRealignResample.fMatrix
    DevAddLabel $f.lActive "Matrix: " 
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20  -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady $Gui(pad)
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m
    

    #-------------------------------------------
    # Resample->NewVolume
    #-------------------------------------------
    set f $fRealignResample.fNewVolume
    
    DevAddLabel $f.lNewVolume "New Volume: "
    eval {entry $f.eNewVolume -width 30 -textvariable RealignResample(NewVolume) } $Gui(WEA)
    #set the new name to the oldname + _realign
    set RealignResample(NewVolume) "[Volume($Matrix(volume),node) GetName]_realign"
    pack $f.lNewVolume $f.eNewVolume -side left -padx $Gui(pad) -pady $Gui(pad)
   

    #-------------------------------------------
    # Resample->Output Spacing
    #-------------------------------------------   
    set f $fRealignResample.fOutputSpacing

    DevAddLabel $f.lOutputSpacing "Output Spacing:"
    pack  $f.lOutputSpacing -side top -padx $Gui(pad) -pady 0
    DevAddLabel $f.lOutputSpacingX "LR:"
    eval {entry $f.eOutputSpacingX -width 6 -textvariable RealignResample(OutputSpacingX) } $Gui(WEA)
    set RealignResample(OutputSpacingX) 1
    DevAddLabel $f.lOutputSpacingY "IS:"
    eval {entry $f.eOutputSpacingY -width 6 -textvariable RealignResample(OutputSpacingY) } $Gui(WEA)
    set RealignResample(OutputSpacingY) 1
    DevAddLabel $f.lOutputSpacingZ "PA:"
    eval {entry $f.eOutputSpacingZ -width 6 -textvariable RealignResample(OutputSpacingZ) } $Gui(WEA)
    set RealignResample(OutputSpacingZ) 1

    pack $f.lOutputSpacingX $f.eOutputSpacingX $f.lOutputSpacingY $f.eOutputSpacingY $f.lOutputSpacingZ $f.eOutputSpacingZ -side left -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Set Spacing
    #-------------------------------------------   
    set f $fRealignResample.fSetSpacing

    DevAddButton $f.bIsoSpacingX " Iso LR  "  IsoSpacingX
    DevAddButton $f.bIsoSpacingY " Iso IS  "  IsoSpacingY
    DevAddButton $f.bIsoSpacingZ " Iso PA  "  IsoSpacingZ
    pack $f.bIsoSpacingX $f.bIsoSpacingY $f.bIsoSpacingZ -side left -padx $Gui(pad)
    
    #-------------------------------------------
    # Resample->Auto Spacing
    #-------------------------------------------   
    set f $fRealignResample.fAutoSpacing
    DevAddButton $f.bAutoSpacing "Original Spacing"  AutoSpacing
    pack $f.bAutoSpacing -side bottom -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Output Extent
    #-------------------------------------------   
    set f $fRealignResample.fOutputExtent

    DevAddLabel $f.lOutputExtent "Output Extent:"
    pack  $f.lOutputExtent -side top -padx $Gui(pad) -pady 0
    DevAddLabel $f.lOutputExtentX "LR:"
    eval {entry $f.eOutputExtentX -width 6 -textvariable RealignResample(OutputExtentX) } $Gui(WEA)
    DevAddLabel $f.lOutputExtentY "IS:"
    eval {entry $f.eOutputExtentY -width 6 -textvariable RealignResample(OutputExtentY) } $Gui(WEA)
    DevAddLabel $f.lOutputExtentZ "PA:"
    eval {entry $f.eOutputExtentZ -width 6 -textvariable RealignResample(OutputExtentZ) } $Gui(WEA)
  
    pack $f.lOutputExtentX $f.eOutputExtentX $f.lOutputExtentY $f.eOutputExtentY $f.lOutputExtentZ $f.eOutputExtentZ -side left -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Set Extent
    #-------------------------------------------   
    set f $fRealignResample.fSetExtent

    DevAddButton $f.bIsoExtentX " Auto LR "  AutoExtentX
    DevAddButton $f.bIsoExtentY " Auto IS "  AutoExtentY
    DevAddButton $f.bIsoExtentZ " Auto PA "  AutoExtentZ
    pack $f.bIsoExtentX $f.bIsoExtentY $f.bIsoExtentZ -side left -padx $Gui(pad)

    #-------------------------------------------
    # Resample->Auto Extent
    #-------------------------------------------   
    set f $fRealignResample.fAutoExtent
    DevAddButton $f.bAutoExtent "Auto Extent"  AutoExtent
    pack $f.bAutoExtent -side bottom -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Interpolation Mode
    #-------------------------------------------
    set f $fRealignResample.fInterpolationMode
    eval {label $f.lInterpolationMode -text "Interpolation Mode:"} $Gui(WLA)
  
    #Create label foreach type of interpolation
    foreach label {"NearestNeighbor" "Linear" "Cubic"} text {"Nearest Neighbor" "Linear" "Cubic"} {
        eval {radiobutton $f.rb$label -text $text -variable RealignResample(InterpolationMode) -value $label} $Gui(WLA)
    }
    set RealignResample(InterpolationMode) NearestNeighbor
    pack $f.lInterpolationMode -side top -padx $Gui(pad) -pady $Gui(pad)
    pack $f.rbNearestNeighbor $f.rbLinear $f.rbCubic -side left -anchor w -padx 1 -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Begin Resample
    #-------------------------------------------
    set f $fRealignResample.fBeginResample
    
    DevAddButton $f.bBeginResample "Realign Resample"  Resample

    pack $f.bBeginResample -side top -padx $Gui(pad) -pady $Gui(pad)
    
    RealignResampleUpdateMRML

    #-------------------------------------------
    # Resample->SaveAs
    #-------------------------------------------
#     set f $fRealignResample.fSaveAs
    
#     DevAddLabel $f.lSaveAs "Save As: "
#     eval {menubutton $f.mbSaveAs -text "$RealignResample(SaveAs)" -relief raised -bd 2 -width 15 -menu $f.mbSaveAs.m} $Gui(WMBA)
#     eval {menu $f.mbSaveAs.m} $Gui(WMA)
    
#     pack $f.lSaveAs $f.mbSaveAs -side left -padx $Gui(pad) -pady $Gui(pad)
#     foreach value {"Sagittal:LR" "Sagittal:RL" "Axial:SI" "Axial:IS" "Coronal:AP" "Coronal:PA"} {
#     $f.mbSaveAs.m add command -label $value -command "RealignResampleSaveAs $value"
#     }

#     set Matrix(SaveAs) $f.mbSaveAs


    #-------------------------------------------
    # Resample->BeginSave
    #-------------------------------------------
    set f $fRealignResample.fBeginSave
    
    DevAddButton $f.bBeginSave "Save"  Write

    pack $f.bBeginSave -side top -padx $Gui(pad) -pady $Gui(pad)
    
    RealignResampleUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC RealignResampleSetACPCList
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleSetACPCList {{v ""}} {
    global Matrix Volume RealignResample

    set RealignResample(ACPCList) "$v"
    $RealignResample(mbACPC) config -text "$v"
    puts "ACPC List: $v"
}

#-------------------------------------------------------------------------------
# .PROC RealignResampleSetACPCList
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleSetMidlineList {{v ""}} {
    global Matrix Volume RealignResample

    set RealignResample(MidlineList) "$v"
    $RealignResample(mbMidline) config -text "$v"
    puts "Midline List: $v"
}

#-------------------------------------------------------------------------------
# .PROC RealignResampleSetVolume
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleSetVolume2 {{v ""}} {
    global Matrix Volume RealignResample

    if {$v == ""} {
        set v $Matrix(volume)
    } else {
        set Matrix(volume) $v
    }
    set spacing [split [[Volume($Matrix(volume),vol) GetOutput] GetSpacing]]
    set RealignResample(OutputSpacingX) [lindex $spacing 0]
    set RealignResample(OutputSpacingY) [lindex $spacing 1]
    set RealignResample(OutputSpacingZ) [lindex $spacing 2]
    set RealignResample(OutputExtentX) 256
    set RealignResample(OutputExtentY) 256
    AutoExtentZ

    set RealignResample(NewVolume) "[Volume($v,node) GetName]_realign"

    #Display what the user picked from the menu as the Volume to move
    $Matrix(mbVolume2) config -text "[Volume($v,node) GetName]"

    #Set Matrix(FidAlignVolumeName) to be the name of the volume to move
    set Matrix(RealignResampleVolumeName2) "[Volume($v,node) GetName]"

    #Print out what the user has set as the volume to move
    puts "this is the VolumeName: $Matrix(RealignResampleVolumeName2)"   
}
#-------------------------------------------------------------------------------
# .PROC AutoSpacing
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoSpacing {} {
    global Matrix Volume RealignResample
    set spacing [split [[Volume($Matrix(volume),vol) GetOutput] GetSpacing]]
    set RealignResample(OutputSpacingX) [lindex $spacing 0]
    set RealignResample(OutputSpacingY) [lindex $spacing 1]
    set RealignResample(OutputSpacingZ) [lindex $spacing 2]
}

#-------------------------------------------------------------------------------
# .PROC AutoExtent
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoExtent {} {
    global RealignResample Module Matrix Volume
    
    #Get matrix
    vtkMatrix4x4 mat
    mat DeepCopy [[Matrix($Matrix(activeID),node) GetTransform] GetMatrix]
    
    #Set default values
    vtkTransform m2lt
    m2lt SetMatrix mat 
    set volume [Volume($Matrix(volume),vol) GetOutput]
    set dim [split [$volume GetWholeExtent]]
    set q [m2lt TransformPoint [lindex $dim 0] [lindex $dim 4] [lindex $dim 2]]
    set ext(0) [lindex $q 0]
    set ext(1) [lindex $q 0]
    set ext(2) [lindex $q 1]
    set ext(3) [lindex $q 1]
    set ext(4) [lindex $q 2]
    set ext(5) [lindex $q 2]
    
    set origin [split [$volume GetOrigin]]
    set wholeExtent [split [$volume GetWholeExtent]]
    set spacing [split [$volume GetSpacing]]
    #loop through each point transforming it and finding if it is greater than max or less than min
    for {set i 0} {$i < 8} {incr i} {
    set point(0) [expr [lindex $origin 0] + [lindex $wholeExtent [expr $i %  2]]]
    set point(1) [expr [lindex $origin 1] + [lindex $wholeExtent [expr 2 + ($i / 2) % 2]]]
    set point(2) [expr [lindex $origin 2] + [lindex $wholeExtent [expr 4 + ($i / 4) % 2]]]
    set newpoint [m2lt TransformPoint $point(0) $point(2) $point(1)]
    puts  "Point: $point(0) $point(2) $point(1) New: $newpoint" 
    for {set j 0} { $j < 3} {incr j} {
        if {[lindex $newpoint $j] > $ext([expr 2 * $j + 1])} {
        set ext([expr 2 * $j + 1]) [lindex $newpoint $j]
        }
        if {[lindex $newpoint $j] < $ext([expr 2 * $j])} {
        set ext([expr 2 * $j]) [lindex $newpoint $j]
        }
    }
    }
    puts "$ext(0) $ext(1) $ext(2) $ext(3) $ext(4) $ext(5)"
    #calculates the final extent
    set RealignResample(OutputExtentY) [expr round(($ext(5) - $ext(4)) * [lindex $spacing 1] / $RealignResample(OutputSpacingY)) + 1]
    set RealignResample(OutputExtentZ) [expr round(($ext(3) - $ext(2)) * [lindex $spacing 2] / $RealignResample(OutputSpacingZ)) + 1]
    set RealignResample(OutputExtentX) [expr round(($ext(1) - $ext(0)) * [lindex $spacing 0] / $RealignResample(OutputSpacingX)) + 1]

    mat Delete
    m2lt Delete
}
#-------------------------------------------------------------------------------
# .PROC AutoExtentX
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoExtentX {} {
    global RealignResample
    set tmpZ $RealignResample(OutputExtentZ)
    set tmpY $RealignResample(OutputExtentY)
    AutoExtent
    set RealignResample(OutputExtentZ) $tmpZ
    set RealignResample(OutputExtentY) $tmpY

}  
#-------------------------------------------------------------------------------
# .PROC AutoExtentY
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoExtentY {} {
    global RealignResample
    set tmpZ $RealignResample(OutputExtentZ)
    set tmpX $RealignResample(OutputExtentX)
    AutoExtent
    set RealignResample(OutputExtentZ) $tmpZ
    set RealignResample(OutputExtentX) $tmpX
} 
#-------------------------------------------------------------------------------
# .PROC AutoExtentZ
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoExtentZ {} {
    global RealignResample
    set tmpY $RealignResample(OutputExtentY)
    set tmpX $RealignResample(OutputExtentX)
    AutoExtent
    set RealignResample(OutputExtentY) $tmpY
    set RealignResample(OutputExtentX) $tmpX
} 

#-------------------------------------------------------------------------------
# .PROC IsoSpacingX
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IsoSpacingX {} {
    global RealignResample
    set RealignResample(OutputSpacingZ) $RealignResample(OutputSpacingX)
    set RealignResample(OutputSpacingY) $RealignResample(OutputSpacingX)
}  
#-------------------------------------------------------------------------------
# .PROC IsoSpacingY
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IsoSpacingY {} {
    global RealignResample
    set RealignResample(OutputSpacingX) $RealignResample(OutputSpacingY)
    set RealignResample(OutputSpacingZ) $RealignResample(OutputSpacingY)
} 
#-------------------------------------------------------------------------------
# .PROC IsoSpacingZ
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IsoSpacingZ {} {
    global RealignResample
    set RealignResample(OutputSpacingX) $RealignResample(OutputSpacingZ)
    set RealignResample(OutputSpacingY) $RealignResample(OutputSpacingZ)
} 
  
#-------------------------------------------------------------------------------
# .PROC RealignResampleSaveAs
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleSaveAs {value} {
    global Matrix RealignResample

    $Matrix(SaveAs) config -text "$value"

    #Set Matrix(FidAlignVolumeName) to be the name of the volume to move
    set RealignResample(SaveAs) "$value"

    #Print out what the user has set as the volume to move
    puts "Save As: $value"   
}

proc RealignCalculate {} {
    global RealignResample Module Matrix Volume Fiducials Point
    puts $RealignResample(MidlineList)
    puts $RealignResample(ACPCList)
    vtkTransform trans
    trans Identity
    trans PostMultiply
    if {$RealignResample(MidlineList) != "None" } {
    puts "Doing Midline..."
    set fids $Fiducials($Fiducials($RealignResample(MidlineList),fid),pointIdList) 
    
    vtkMath math
    set x 0
    foreach fid $fids {
        set list($x) [split [FiducialsGetPointCoordinates $fid] " "]
        puts "Point $x: $list($x)"
        incr x
    }
    
    vtkPolyData polydata
    vtkPolyData output
    vtkPoints points
    puts "Total Number of Points: $x"
    points SetNumberOfPoints $x
    for {set i 0} {$i < $x} {incr i} {
        points SetPoint $i [lindex $list($i) 0] [lindex $list($i) 1] [lindex $list($i) 2]
    }
    polydata SetPoints points
    puts "Calling vtkPrincipleAxes"
    vtkPrincipalAxes pa
    puts "Making vtkPoints"
    puts "Set Input to PrincipleAxes"
    pa SetInput polydata
    puts "Executing PrincipleAxes"
    pa Update
    set normal [pa GetZAxis]
    puts "$normal"
    set nx [lindex $normal 0 ]
    set ny [lindex $normal 1 ]
    set nz [lindex $normal 2 ]
    puts "$nx $ny $nz"

    vtkMatrix4x4 mat
    mat Identity

#     mat SetElement 0 0 [expr  1 - $nx*$nx]
#     mat SetElement 0 1 [expr -1 * $nx*$ny]
#     mat SetElement 0 2 [expr -1 * $nx*$nz]

#     mat SetElement 1 0 [expr -1 * $nx*$ny]
#     mat SetElement 1 1 [expr  1 - $ny*$ny]
#     mat SetElement 1 2 [expr -1 * $ny*$nz]
    
#     mat SetElement 2 0 [expr -1 * $nx*$nz]
#     mat SetElement 2 1 [expr -1 * $ny*$nz]
#     mat SetElement 2 2 [expr  1 - $nz*$nz]
    
# void Align(Matrix& u)
# {
#   const double epsilon = 1.0e-6;
#   int i;

#   int rowCount = u.rows();
#   int colCount = u.cols(); 

#   for ( int j = 0; j < colCount; j++ ) {
#     double maxU = u[0][j];
#     for ( i = 1; i < rowCount; i++ ) 
#       if ( fabs(maxU) < fabs(u[i][j])-epsilon ) maxU = u[i][j];
    
#     if ( maxU < 0 ) {
#       for ( i = 0; i < rowCount; i++ ) 
#         u[i][j] = -u[i][j];
#     }
#   }
# }


    set Max $nx
    if {[expr $ny*$ny] > [expr $Max*$Max]} {
        set Max $ny
    }
    if {[expr $nz*$nz] > [expr $Max*$Max]} {
        set Max $nz
    }
    set sign 1
    if {$Max < 0} {
        set sign -1
    }

     set i 0
     foreach point [pa GetZAxis] {
         mat SetElement $i 0 [expr $sign * $point]
         incr i
     }
    
      set oneAndAlpha [expr 1 + [mat GetElement 0 0]]
    
      mat SetElement 0 1 [expr -1 * [mat GetElement 1 0]]
    
      mat SetElement 0 2 [expr -1 * [mat GetElement 2 0]] 
      mat SetElement 2 1 [expr -1 * [mat GetElement 1 0] * [mat GetElement 2 0] / $oneAndAlpha]
      mat SetElement 1 2 [expr -1 * [mat GetElement 1 0] * [mat GetElement 2 0] / $oneAndAlpha]
      mat SetElement 1 1 [expr 1  - [mat GetElement 1 0] * [mat GetElement 1 0] / $oneAndAlpha]
      mat SetElement 2 2 [expr 1  - [mat GetElement 2 0] * [mat GetElement 2 0] / $oneAndAlpha]
    
    vtkMatrix4x4 matInverse
    matInverse DeepCopy mat
    matInverse Invert
    trans SetMatrix matInverse
#    trans SetMatrix mat
    mat Delete
    matInverse Delete
    pa Delete
    points Delete
    polydata Delete
    output Delete
    math Delete
    }
    
    
    

    if {$RealignResample(ACPCList) != "None"} {
    puts "Doing ACPC..."
    set acpc $Fiducials($Fiducials($RealignResample(ACPCList),fid),pointIdList) 
    set y 0
    foreach fid $acpc {
        if { $y < 2 } {
        set ACPCpoints($y) [split [FiducialsGetPointCoordinates $fid] " "]
        puts "ACPC Point $y: $ACPCpoints($y)"
        incr y
        }
    }
    set top [expr [lindex $ACPCpoints(0) 2] - [lindex $ACPCpoints(1) 2]]
    set bot [expr [lindex $ACPCpoints(0) 1] - [lindex $ACPCpoints(1) 1]]
    set tangent [expr atan( $top / $bot) * (180.0/(4.0*atan(1.0)))]
    puts $tangent
    trans RotateX [expr $tangent * -1]
    }
    [Matrix($Matrix(activeID),node) GetTransform] SetMatrix [trans GetMatrix]
    MainUpdateMRML
    RenderAll
    trans Delete
    puts "Done"
}


proc Resample {} {
    global RealignResample Module Matrix Volume
       
  
    # Create a new Volume node
    set newvol [DevCreateNewCopiedVolume  $Matrix(volume) "" "$RealignResample(NewVolume)"]
    set node [Volume($newvol,vol) GetMrmlNode]
    Mrml(dataTree) RemoveItem $node
    Mrml(dataTree) AddItem $node
    
    # Create a new vtkImageData
    vtkImageData Target
    Target DeepCopy [Volume($Matrix(volume),vol) GetOutput]
    Volume($newvol,vol) SetImageData Target

    MainUpdateMRML
    MainVolumesUpdate $newvol

    # Make a RAS to VTK matrix for realign resample
    # based on the position matrix
    vtkMatrix4x4 ModelRasToVtk
    set position [Volume($Matrix(volume),node) GetPositionMatrix]
    puts "$position"
    ModelRasToVtk Identity
    set ii 0
    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            # Put the element from the position string
            ModelRasToVtk SetElement $i $j [lindex $position $ii]
            incr ii
        }
    # Remove the translation elements
    ModelRasToVtk SetElement $i 3 0
    }
    # add a 1 at the for  M(4,4)
    ModelRasToVtk SetElement 3 3 1
    # Matrix now is
    # a b c 0
    # d e f 0
    # g h i 0 
    # 0 0 0 1
    # a -> i is either -1 0 or 1 depending on 
    # the original orientation of the volume
 

    # Now we can build the Vtk1ToVtk2 matrix based on
    # ModelRasToVtk and ras1toras2
    # vtk1tovtk2 = inverse(rastovtk) ras1toras2 rastovtk
    # RasToVtk
    vtkMatrix4x4 RasToVtk
    RasToVtk DeepCopy ModelRasToVtk    
    puts "RasToVtk Matrix is:"
    for {set i 0} {$i < 4} {incr i} {    
    set element0 [RasToVtk GetElement $i 0]
    set element1 [RasToVtk GetElement $i 1]
    set element2 [RasToVtk GetElement $i 2]
    set element3 [RasToVtk GetElement $i 3]
    puts "$element0  $element1  $element2  $element3" 
    }
    
    # Inverse Matrix RasToVtk
    vtkMatrix4x4 InvRasToVtk
    InvRasToVtk DeepCopy ModelRasToVtk
    InvRasToVtk Invert
    puts "InvRasToVtk Matrix is:"
    for {set i 0} {$i < 4} {incr i} {    
    set element0 [InvRasToVtk GetElement $i 0]
    set element1 [InvRasToVtk GetElement $i 1]
    set element2 [InvRasToVtk GetElement $i 2]
    set element3 [InvRasToVtk GetElement $i 3]
    puts "$element0  $element1  $element2  $element3" 
    }

    # Ras1toRas2 given by the slicer MRML tree
    vtkMatrix4x4 Ras1ToRas2
    Ras1ToRas2 DeepCopy [[Matrix($Matrix(activeID),node) GetTransform] GetMatrix]
    puts "Ras1ToRas2 Matrix is:"
    for {set i 0} {$i < 4} {incr i} {    
    set element0 [Ras1ToRas2 GetElement $i 0]
    set element1 [Ras1ToRas2 GetElement $i 1]
    set element2 [Ras1ToRas2 GetElement $i 2]
    set element3 [Ras1ToRas2 GetElement $i 3]

    puts "$element0  $element1  $element2  $element3" 
    }

    # Now build Vtk1ToVtk2
    vtkMatrix4x4 Vtk1ToVtk2
    Vtk1ToVtk2 Identity
    Vtk1ToVtk2 Multiply4x4 Ras1ToRas2 RasToVtk  Vtk1ToVtk2
    Vtk1ToVtk2 Multiply4x4 InvRasToVtk  Vtk1ToVtk2 Vtk1ToVtk2
    puts "Vtk1ToVtk2 Matrix is:"
    for {set i 0} {$i < 4} {incr i} {    
    set element0 [Vtk1ToVtk2 GetElement $i 0]
    set element1 [Vtk1ToVtk2 GetElement $i 1]
    set element2 [Vtk1ToVtk2 GetElement $i 2]
    set element3 [Vtk1ToVtk2 GetElement $i 3]
    puts "$element0  $element1  $element2  $element3" 
    }


    # Setting up for vtkImageReslice
    # Invert the matrix (because we resample the grid not the object)
    Vtk1ToVtk2 Invert
    vtkTransform trans
    trans SetMatrix Vtk1ToVtk2 
    # Center the input image
    vtkImageChangeInformation ici
    ici CenterImageOn
    ici SetInput Target
    # Set the input of the vtkImageReslice
    vtkImageReslice reslice
    reslice SetInput [ici GetOutput]
    reslice SetResliceTransform trans
    # Set the output spacing to user entered values
    puts "SetOutputSpacing $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingY) $RealignResample(OutputSpacingZ)"
    reslice SetOutputSpacing $RealignResample(OutputSpacingX) \
                             $RealignResample(OutputSpacingY) \
                             $RealignResample(OutputSpacingZ)
    # Set the extent to user or calculated values
    puts "Extent: 0 $RealignResample(OutputExtentX) 0 $RealignResample(OutputExtentY) 0 $RealignResample(OutputExtentZ)"
    reslice SetOutputExtent  0 $RealignResample(OutputExtentX) \
                             0 $RealignResample(OutputExtentY) \
                             0 $RealignResample(OutputExtentZ)
    # Set the interpolation mode 
    puts "SetInterpolationModeTo$RealignResample(InterpolationMode)"
    reslice SetInterpolationModeTo$RealignResample(InterpolationMode)
    # Reslice!
    reslice Update
    
    # Store output in the MRML tree
    # and update its properties
    Volume($newvol,vol) SetImageData [reslice GetOutput]
    eval [Volume($newvol,node) SetSpacing  $RealignResample(OutputSpacingX) \
                                           $RealignResample(OutputSpacingY) \
                                           $RealignResample(OutputSpacingZ)]
    eval [Volume($newvol,node) SetImageRange 1 $RealignResample(OutputExtentZ)]
    eval [Volume($newvol,node) SetDimensions  $RealignResample(OutputExtentX) $RealignResample(OutputExtentY)]
    Volume($newvol,node) ComputeRasToIjkFromScanOrder [Volume($newvol,node) GetScanOrder]
    
    MainUpdateMRML
    MainVolumesUpdate $newvol
   
    reslice Delete
    ici Delete
    ModelRasToVtk Delete
    Target Delete
    trans Delete
    Ras1ToRas2 Delete
    RasToVtk Delete
    InvRasToVtk Delete
    Vtk1ToVtk2 Delete
     
    RenderAll
    puts "Done."
     
} 

proc Write {} {
    global RealignResample Volume Matrix
    set RealignResample(prefixSave) [file join $Volume(DefaultDir) [Volume($Matrix(NewVolume),node) GetName]]
    set RealignResample(prefixSave) [MainFileSaveVolume $Matrix(NewVolume) $RealignResample(prefixSave)]
      
    MainVolumesWrite $Matrix(NewVolume) $RealignResample(prefixSave)
    MainVolumesSetActive $Matrix(NewVolume)
}
