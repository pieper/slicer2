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
    set Module($m,procVTK) RealignResampleBuildVTK
   # set Module($m,procEnter) RealignResampleEnter
   # set Module($m,procExit) RealignResampleExit
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
        {$Revision: 1.1 $} {$Date: 2004/07/30 15:50:20 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
   
    set Matrix(volume) $Volume(idNone)
    set Matrix(RealignResampleVolumeName2) None
}

proc RealignResampleUpdateMRML {} {
    global Matrix Volume 

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
    global Gui RealignResample Module Volume Line Matrix
    
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
    
    foreach frame "Matrix Fiducials Calculate" {
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
    # Realign->Fiducials
    #-------------------------------------------   
    set f $fFiducials.fFiducials
    
    DevAddLabel $f.lFiducials "Fiducials >3"
    pack $f.lFiducials -side top -anchor w -padx $Gui(pad) 
    FiducialsAddActiveListFrame $f 10 25
         
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

    foreach frame "Volume Matrix NewVolume OutputSpacing OutputOrigin InterpolationMode BeginResample" {
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
    # Resample->Output Origin
    #-------------------------------------------
    set f $fRealignResample.fOutputOrigin

    DevAddLabel $f.lOutputOrigin "Output Origin:"
    pack  $f.lOutputOrigin -side top -padx $Gui(pad) -pady $Gui(pad)
    DevAddLabel $f.lOutputOriginX "X:"
    eval {entry $f.eOutputOriginX -width 6 -textvariable RealignResample(OutputOriginX) } $Gui(WEA)
    set RealignResample(OutputOriginX) 0
    DevAddLabel $f.lOutputOriginY "Y:"
    eval {entry $f.eOutputOriginY -width 6 -textvariable RealignResample(OutputOriginY) } $Gui(WEA)
    set RealignResample(OutputOriginY) 0
    DevAddLabel $f.lOutputOriginZ "Z:"
    eval {entry $f.eOutputOriginZ -width 6 -textvariable RealignResample(OutputOriginZ) } $Gui(WEA)
    set RealignResample(OutputOriginZ) 0

    pack $f.lOutputOriginX $f.eOutputOriginX $f.lOutputOriginY $f.eOutputOriginY $f.lOutputOriginZ $f.eOutputOriginZ -side left -padx $Gui(pad) -pady $Gui(pad)

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
    
    set RealignResample(NewVolume) "[Volume($v,node) GetName]_realign"
    #Display what the user picked from the menu as the Volume to move
    $Matrix(mbVolume2) config -text "[Volume($v,node) GetName]"

    #Set Matrix(FidAlignVolumeName) to be the name of the volume to move
    set Matrix(RealignResampleVolumeName2) "[Volume($v,node) GetName]"

    #Print out what the user has set as the volume to move
    puts "this is the VolumeName: $Matrix(RealignResampleVolumeName2)"   
}

#-------------------------------------------------------------------------------
# .PROC RealignResampleBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc RealignResampleBuildVTK {} {
}

#-------------------------------------------------------------------------------
# .PROC RealignResampleEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc RealignResampleEnter {} {
    global RealignResample
    RealignResampleUpdateMRML
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $RealignResample(eventManager)

    # clear the text box and put instructions there
    $RealignResample(textBox) delete 1.0 end
    $RealignResample(textBox) insert end "Shift-Click anywhere!\n"
    RealignResampleUpdateMRML

}


#-------------------------------------------------------------------------------
# .PROC RealignResampleExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RealignResampleExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

proc RealignCalculate {} {
    global RealignResample Module Matrix Volume Fiducials Point
    puts "Volume: [Volume($Matrix(volume),vol) GetOutput]"
    set vol [Volume($Matrix(volume),vol) GetOutput]
    puts "Lines: [Volume($Matrix(refVolume),vol) GetOutput]"
    set lines [Volume($Matrix(refVolume),vol) GetOutput]
    puts "Fiducials: $Fiducials($Fiducials($Fiducials(activeList),fid),pointIdList)"
    set fids $Fiducials($Fiducials($Fiducials(activeList),fid),pointIdList) 
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
    set vol [Volume($Matrix(volume),vol) GetOutput]
    [Matrix($Matrix(activeID),node) GetTransform] SetMatrix matInverse
    puts [matInverse Print]
    MainUpdateMRML
    RenderAll
    mat Delete
    matInverse Delete
    pa Delete
    points Delete
    polydata Delete
    output Delete
    math Delete
}

proc Junk {} {
    puts [mat Print]
    puts [matInverse Print]
    puts [[[Volume($Matrix(volume),vol) GetMrmlNode] GetRasToIjk ] Print]
    puts [[[Volume($Matrix(volume),vol) GetMrmlNode] GetWldToIjk ] Print]
    puts [[Volume($Matrix(volume),vol) GetMrmlNode] ListMethods]
    vtkMatrix4x4 testing 
    testing Multiply4x4 mat [[Volume($Matrix(volume),vol) GetMrmlNode] GetRasToIjk] testing
    puts [testing Print]
    mat DeepCopy [[Volume($Matrix(volume),vol) GetMrmlNode] GetWldToIjk ] 
    vtkTransform m2lt
    m2lt SetMatrix mat
  
   
       puts "Done"
}


proc Resample {} {
    global RealignResample Module Matrix Volume
    vtkMatrix4x4 mat
    mat DeepCopy [[Matrix($Matrix(activeID),node) GetTransform] GetMatrix]
    
    set volume [Volume($Matrix(volume),vol) GetOutput]
      
    vtkImagePermute first
    first SetInput $volume
    first SetFilteredAxes 0 2 1

    vtkTransform m2lt
    m2lt SetMatrix mat
    
    vtkImageChangeInformation ici
    ici SetInput [first GetOutput]
    ici CenterImageOn
  

    vtkImageReslice reslice
    reslice SetResliceTransform m2lt
    reslice AutoCropOutputOn
    set dim [split [$volume GetExtent]]
    set x(0) [expr [expr [lindex $dim 1] - [lindex $dim 0] + 1] / 2]
    set x(1) [expr [expr [lindex $dim 3] - [lindex $dim 2] + 1] / 2]
    set x(2) [expr [expr [lindex $dim 5] - [lindex $dim 4] + 1] / 2]
    puts "Extent: [lindex $dim 0] [lindex $dim 1] [lindex $dim 2] [lindex $dim 3] [lindex $dim 4] [lindex $dim 5]"
    reslice SetInput [ici GetOutput]

    puts "SetOutputSpacing $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingY) $RealignResample(OutputSpacingZ)"
    reslice SetOutputSpacing $RealignResample(OutputSpacingX) $RealignResample(OutputSpacingZ) $RealignResample(OutputSpacingY)
    
    puts "SetOutputOrigin $RealignResample(OutputOriginX) $RealignResample(OutputOriginY) $RealignResample(OutputOriginZ)"
    reslice SetOutputOrigin  [expr $RealignResample(OutputOriginX) * -1 - $x(0) * $RealignResample(OutputSpacingX)] [expr $RealignResample(OutputOriginZ) * -1 - $x(2) * $RealignResample(OutputSpacingZ)] [expr $RealignResample(OutputOriginY) * -1 - $x(1) * $RealignResample(OutputSpacingY)]

    reslice SetOutputExtent [lindex $dim 0] [lindex $dim 1] [lindex $dim 4] [lindex $dim 5] [lindex $dim 2] [lindex $dim 3]
    puts "SetInterpolationModeTo$RealignResample(InterpolationMode)"
    reslice SetInterpolationModeTo$RealignResample(InterpolationMode)
    
    vtkImagePermute last
    last SetInput [reslice GetOutput]
    last SetFilteredAxes 0 2 1
  

    set NewVolume [DevCreateNewCopiedVolume $Matrix(volume) "" "$RealignResample(NewVolume)"]
    Volume($NewVolume,vol) SetImageData [last GetOutput]

    puts "Cleaning Up..."
    reslice Delete
    mat Delete
    first Delete
    ici Delete
    last Delete
    m2lt Delete
    MainUpdateMRML
    RenderAll
    puts "Done"
} 

proc Write {} {
    vtkImageWriter iw
    iw SetFilePrefix "/projects/schiz/guest/jake/test"
    iw SetFilePattern "%s.%03d"
    iw SetFileDimensionality 2
    iw SetInput [reslice GetOutput]
    puts "Writing ..."
    iw Write
}
