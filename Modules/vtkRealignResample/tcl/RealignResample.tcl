package require vtk

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

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module realigns and resamples using vtkImageReslice"
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Jacob Albertson, SPL, jacob@bwh.harvard.edu"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "Alpha"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Fiducials RealignResample Help"
    set Module($m,row1Name) "{Fiducials} {Realign Resample} {Help}"
    set Module($m,row1,tab) RealignResample

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) RealignResampleBuildVTK
    #
    #   All the options are:

    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) RealignResampleBuildGUI
    set Module($m,procMRML) RealignResampleUpdateMRML
    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.1 $} {$Date: 2004/07/30 15:50:19 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
   
    set Matrix(volume) $Volume(idNone)
    set Matrix(RealignResampleVolumeName2) None
    set RealignResample(SaveAs) None
}

proc RealignResampleUpdateMRML {} {
    global Matrix Volume Fiducials RealignResample

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $Matrix(volume) ] == -1} {
        RealignResampleSetVolume $n
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
    foreach v $Fiducials(listOfNames) {
    $m add command -label "$v" -command "RealignResampleSetACPCList $v"
    }
    # Menu Midline
    # ------------------------------------
    set m $RealignResample(mbMidline).m
    $m delete 0 end
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
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(RealignResample,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Stuff
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
   #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The RealignResample module is an example for developers.  It shows how to add a module 
    to the Slicer.  The source code is in slicer2/Modules/vtkRealignResample/tcl/RealignResample.tcl.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Stuff:</B> This tab is a demo for developers.
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

    foreach frame "Volume Matrix NewVolume OutputSpacing InterpolationMode BeginResample SaveAs BeginSave" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
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
    set RealignResample(NewVolume) "[Volume($Matrix(volume),node) GetName]_realigned"
    pack $f.lNewVolume $f.eNewVolume -side left -padx $Gui(pad) -pady $Gui(pad)
   

    #-------------------------------------------
    # Resample->Output Spacing
    #-------------------------------------------   
    set f $fRealignResample.fOutputSpacing

    DevAddLabel $f.lOutputSpacing "Output Spacing:"
    pack  $f.lOutputSpacing -side top -padx $Gui(pad) -pady $Gui(pad)
    DevAddLabel $f.lOutputSpacingX "X:"
    eval {entry $f.eOutputSpacingX -width 6 -textvariable RealignResample(OutputSpacingX) } $Gui(WEA)
    set RealignResample(OutputSpacingX) 1
    DevAddLabel $f.lOutputSpacingY "Y:"
    eval {entry $f.eOutputSpacingY -width 6 -textvariable RealignResample(OutputSpacingY) } $Gui(WEA)
    set RealignResample(OutputSpacingY) 1
    DevAddLabel $f.lOutputSpacingZ "Z:"
    eval {entry $f.eOutputSpacingZ -width 6 -textvariable RealignResample(OutputSpacingZ) } $Gui(WEA)
    set RealignResample(OutputSpacingZ) 1

    pack $f.lOutputSpacingX $f.eOutputSpacingX $f.lOutputSpacingY $f.eOutputSpacingY $f.lOutputSpacingZ $f.eOutputSpacingZ -side left -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Resample->Interpolation Mode
    #-------------------------------------------
    set f $fRealignResample.fInterpolationMode
    eval {label $f.lInterpolationMode -text "Interpolation Mode:"} $Gui(WLA)
  
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
    set f $fRealignResample.fSaveAs
    
    DevAddLabel $f.lSaveAs "Save As: "
    eval {menubutton $f.mbSaveAs -text "$RealignResample(SaveAs)" -relief raised -bd 2 -width 15 -menu $f.mbSaveAs.m} $Gui(WMBA)
    eval {menu $f.mbSaveAs.m} $Gui(WMA)
    
    pack $f.lSaveAs $f.mbSaveAs -side left -padx $Gui(pad) -pady $Gui(pad)
    foreach value {"Sagittal:LR" "Sagittal:RL" "Axial:SI" "Axial:IS" "Coronal:AP" "Coronal:PA"} {
    $f.mbSaveAs.m add command -label $value -command "RealignResampleSaveAs $value"
    }

    set Matrix(SaveAs) $f.mbSaveAs


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
    puts "ACPC LIst: $v"
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

    set RealignResample(NewVolume) "[Volume($v,node) GetName]_realign"

    #Display what the user picked from the menu as the Volume to move
    $Matrix(mbVolume2) config -text "[Volume($v,node) GetName]"

    #Set Matrix(FidAlignVolumeName) to be the name of the volume to move
    set Matrix(RealignResampleVolumeName2) "[Volume($v,node) GetName]"

    #Print out what the user has set as the volume to move
    puts "this is the VolumeName: $Matrix(RealignResampleVolumeName2)"   
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
    puts "$RealignResample(mbMidline) active"
    puts "Volume: [Volume($Matrix(volume),vol) GetOutput]"
    set vol [Volume($Matrix(volume),vol) GetOutput]
    puts "Lines: [Volume($Matrix(refVolume),vol) GetOutput]"
    set lines [Volume($Matrix(refVolume),vol) GetOutput]
    puts "Fiducials: $Fiducials($Fiducials($RealignResample(MidlineList),fid),pointIdList)"
    set fids $Fiducials($Fiducials($RealignResample(MidlineList),fid),pointIdList) 
    set acpc $Fiducials($Fiducials($RealignResample(ACPCList),fid),pointIdList) 
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
    pa Execute
    vtkMatrix4x4 mat
    
    set i 0
    foreach point [pa GetZAxis] {
    mat SetElement $i 0 $point
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

    vtkTransform trans
    trans SetMatrix matInverse
    trans RotateX [expr $tangent * -1]
    [Matrix($Matrix(activeID),node) GetTransform] SetMatrix [trans GetMatrix]
    MainUpdateMRML
    RenderAll
    mat Delete
    matInverse Delete
    pa Delete
    points Delete
    polydata Delete
    output Delete
    math Delete
    trans Delete
    puts "Done"
}


proc Resample {} {
    global RealignResample Module Matrix Volume
    vtkMatrix4x4 mat
    mat DeepCopy [[Matrix($Matrix(activeID),node) GetTransform] GetMatrix]
    mat SetElement 1 2 [expr [mat GetElement 1 2] * -1]
    mat SetElement 2 1 [expr [mat GetElement 2 1] * -1]
    
    set volume [Volume($Matrix(volume),vol) GetOutput]
    
    switch [Volume($Matrix(volume),node) GetScanOrder] {
        #                    -R        -A        -S
        "LR" { set axes {  0  0 -1   0  1  0  -1  0  0 } }
        "RL" { set axes {  0  0  1   0  1  0  -1  0  0 } }
        "IS" { set axes {  1  0  0   0  0  1   0  1  0 } }
        "SI" { set axes {  1  0  0   0  0 -1   0  1  0 } }
        "PA" { set axes {  1  0  0   0  1  0   0  0  1 } }
        "AP" { set axes {  1  0  0   0 -1  0   0  0  1 } }
        #TODO - gantry tilt not supported
    }
    vtkMatrix4x4 ijkmatrix
    ijkmatrix Identity
    set ii 0
    for {set i 0} {$i < 3} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            # transpose for inverse - reslice transform requires it
            ijkmatrix SetElement $i $j [lindex $axes $ii]
            incr ii
        }
    }
    puts [Volume($Matrix(volume),vol) Print]
    puts [Volume($Matrix(volume),node) Print]

    vtkImagePermute first
    first SetInput $volume
    first SetFilteredAxes 0 2 1
    
    mat Multiply4x4 ijkmatrix mat mat
    vtkTransform m2lt
    m2lt SetMatrix mat 

    set pos  [split [m2lt GetPosition]]

    vtkImageChangeInformation ici
    ici SetInput [first GetOutput]
    ici CenterImageOn
    
    vtkImageChangeInformation ici2
    ici2 SetInput [ici GetOutput]
    ici2 SetOriginTranslation [expr -.5 + [lindex $pos 0]] [expr -.5 + [lindex $pos 1]] [expr -.5 + [lindex $pos 2]]

    vtkImageReslice reslice
    reslice SetResliceTransform m2lt
    
    puts "[ijkmatrix Print]"
    reslice SetResliceAxes ijkmatrix
            
    set dim [split [$volume GetExtent]]
    set x(0) [expr [expr [lindex $dim 1] - [lindex $dim 0] + 1] / 2]
    set x(1) [expr [expr [lindex $dim 3] - [lindex $dim 2] + 1] / 2]
    set x(2) [expr [expr [lindex $dim 5] - [lindex $dim 4] + 1] / 2]

    set spa [split [$volume GetSpacing]]
    set spacing(0) [lindex $spa 0]
    set spacing(1) [lindex $spa 1]
    set spacing(2) [lindex $spa 2]
    puts "Position: $pos"
    
    puts "Extent: [lindex $dim 0] [lindex $dim 1] [lindex $dim 2] [lindex $dim 3] [lindex $dim 4] [lindex $dim 5]"
    reslice SetInput [ici2 GetOutput]

    puts "SetOutputSpacing $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingY) $RealignResample(OutputSpacingZ)"
    reslice SetOutputSpacing  $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingZ) $RealignResample(OutputSpacingY)
    
    reslice SetOutputOrigin  [expr [lindex $pos 0] - $x(0) * $spacing(0)] \
    [expr [lindex $pos 1] * -1 - $x(2) * $spacing(2)] \
    [expr [lindex $pos 2] * -1 - $x(1) * $spacing(1)]

    reslice SetOutputExtent [lindex $dim 0] [expr round( [lindex $dim 1] * $spacing(0) / $RealignResample(OutputSpacingX) )] \
                            [lindex $dim 4] [expr round( [lindex $dim 5] * $spacing(2) / $RealignResample(OutputSpacingZ) )] \
                            [lindex $dim 2] [expr round( [lindex $dim 3] * $spacing(1) / $RealignResample(OutputSpacingY) )] 
    
    puts "SetInterpolationModeTo$RealignResample(InterpolationMode)"
    reslice SetInterpolationModeTo$RealignResample(InterpolationMode)
    reslice Update
    vtkImagePermute last
    last SetInput [reslice GetOutput]
    last SetFilteredAxes 0 2 1
    last Update
  
    set Matrix(NewVolume) [DevCreateNewCopiedVolume $Matrix(volume) "" "$RealignResample(NewVolume)"]
    eval [Volume($Matrix(NewVolume),node) SetSpacing  $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingY) $RealignResample(OutputSpacingZ)]
    eval [Volume($Matrix(NewVolume),node) SetImageRange 1 [expr round($spacing(2) / $RealignResample(OutputSpacingZ) * [lindex $dim 5] )]
      MainUpdateMRML]
    eval [Volume($Matrix(NewVolume),vol) SetImageData [last GetOutput]]
    Volume($Matrix(NewVolume),node) ComputeRasToIjkFromScanOrder [Volume($Matrix(NewVolume),node) GetScanOrder]
    reslice Delete
    mat Delete
    first Delete
    ici Delete
    ici2 Delete 
    last Delete
    m2lt Delete
    ijkmatrix Delete
    MainUpdateMRML
    RenderAll
    puts "Done"
} 

proc Write {} {
    global RealignResample Volume Matrix
    puts "Writing..."
    set RealignResample(prefixSave) [file join $Volume(DefaultDir) [Volume($Matrix(NewVolume),node) GetName]]
    set RealignResample(prefixSave) [MainFileSaveVolume $Matrix(NewVolume) $RealignResample(prefixSave)]
    
    MainVolumesWrite $Matrix(NewVolume) $RealignResample(prefixSave)

    MainVolumesSetActive $Matrix(NewVolume)
}
