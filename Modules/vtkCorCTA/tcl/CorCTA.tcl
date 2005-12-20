#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: CorCTA.tcl,v $
#   Date:      $Date: 2005/12/20 22:55:02 $
#   Version:   $Revision: 1.8.2.1 $
# 
#===============================================================================
# FILE:        CorCTA.tcl
# PROCEDURES:  
#   CorCTAInit
#   CorCTABuildGUI
#   CorCTAUpdateGUI
#   CorCTABuildVTK
#   CorCTAEnter
#   CorCTAExit
#   CorCTAThinning
#   CorCTAConvert
#   CorCTACenterlines
#   CorCTAPickLine Actor CellId
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC CorCTAInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAInit {} {
    global CorCTA Module Volume Model

    set m CorCTA

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "Coronary CT Angiography Tools"
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Arne Hans, SPL"
    set Module($m,category) "Application"

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

    set Module($m,row1List) "Help Centerlines"
    set Module($m,row1Name) "{Help} {Centerlines}"
    set Module($m,row1,tab) Centerlines

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
    #   set Module($m,procVTK) CorCTABuildVTK
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
    set Module($m,procGUI) CorCTABuildGUI
    set Module($m,procVTK) CorCTABuildVTK
    set Module($m,procEnter) CorCTAEnter
    set Module($m,procExit) CorCTAExit
    set Module($m,procMRML) CorCTAUpdateGUI

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
        {$Revision: 1.8.2.1 $} {$Date: 2005/12/20 22:55:02 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set CorCTA(InputVolume) $Volume(idNone)
    set CorCTA(OutputVolume) $Volume(idNone)
    set CorCTA(ModelName) Centerlines
    
    set CorCTA(UseLineEndpoints) 1
    set CorCTA(UseFiducialEndpoints) 0
    set CorCTA(UseSurfaceEndpoints) 0
    
    set CorCTA(Splines) 1
    set CorCTA(SplineSegmentLength) 2
    
    set CorCTA(eventManager) {}
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


#-------------------------------------------------------------------------------
# .PROC CorCTABuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTABuildGUI {} {
    global Gui CorCTA Module Volume
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Main" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(CorCTA,fCenterlines)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Main
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
    This module extracts center lines from segmented vessels. It requires
a label map as an input and generates a model. 
<BR>The input label map must contain a segementation of the vessels labeled with the Vessels color as defined in Base/tcl/Colors.xml.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Centerlines:</B> Select input volume, enter a model name, and
    press the button to extract the centerlines.<BR>
    Currently, only line endpoints are supported.<BR>
    Check 'Smooth lines' to smooth the extracted centerlines. The spline segment length can
    be set in the respective input box.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags CorCTA $help
    MainHelpBuildGUI CorCTA

    #-------------------------------------------
    # Main frame
    #-------------------------------------------
    set fCenterlines $Module(CorCTA,fCenterlines)
    set f $fCenterlines
    
    foreach frame "Top Middle Bottom" {
    frame $f.f$frame -bg $Gui(activeWorkspace)
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Main->Top frame
    #-------------------------------------------
    set f $fCenterlines.fTop
    
    #       grid $f.lMain -padx $Gui(pad) -pady $Gui(pad)
    #       grid $menubutton -sticky w
    
    # Add menus that list models and volumes
    DevAddSelectButton  CorCTA $f InputVolume "Input volume" Pack
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    #lappend Volume(mbActiveList) $f.mbVolume1
    #lappend Volume(mActiveList) $f.mbVolume1.m
        
    #-------------------------------------------
    # Main->Middle frame
    #-------------------------------------------
    set f $fCenterlines.fMiddle
    frame $f.fName -bg $Gui(activeWorkspace)
    frame $f.fEP -bg $Gui(activeWorkspace)
    DevAddLabel $f.fName.lName "Model name:"
    eval {entry $f.fName.eName -width 20 -textvariable CorCTA(ModelName)} $Gui(WEA)
    DevAddLabel $f.lEP "Endpoints"
    eval {checkbutton $f.fEP.cLineEP -text "Lines" -variable CorCTA(UseLineEndpoints)} $Gui(WBA)
    eval {checkbutton $f.fEP.cFiducialEP -text "Fiducials" -variable CorCTA(UseFiducialEndpoints)} $Gui(WBA)
    eval {checkbutton $f.fEP.cSurfaceEP -text "Surfaces" -variable CorCTA(UseSurfaceEndpoints)} $Gui(WBA)
    pack $f.fName -side top -padx 0 -pady $Gui(pad) -fill x
    pack $f.fName.lName $f.fName.eName -side left -padx $Gui(pad) -pady $Gui(pad)
    pack $f.lEP -side top -padx $Gui(pad) -pady $Gui(pad)
    pack $f.fEP -side top -padx 0 -pady $Gui(pad)
    pack $f.fEP.cLineEP $f.fEP.cFiducialEP $f.fEP.cSurfaceEP -side left -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Main->Bottom frame
    #-------------------------------------------
    set f $fCenterlines.fBottom
    frame $f.fLength -bg $Gui(activeWorkspace)
    eval {checkbutton $f.cSplines -text "Smooth lines" -variable CorCTA(Splines)} $Gui(WBA)
    DevAddLabel $f.fLength.lLength "Segment length:"
    eval {entry $f.fLength.eLength -width 5 -textvariable CorCTA(SplineSegmentLength)} $Gui(WEA)
    DevAddButton $f.bCenterlines "Extract Centerlines" CorCTACenterlines
    pack $f.cSplines -side top -padx $Gui(pad) -pady $Gui(pad) 
    pack $f.fLength -side top -padx 0 -pady $Gui(pad) -fill x
    pack $f.fLength.lLength $f.fLength.eLength -side left -padx $Gui(pad) -pady $Gui(pad)
    pack $f.bCenterlines -side top -padx $Gui(pad) -pady $Gui(pad)
    
    # make frames inside the Bottom frame for nice layout
    #foreach frame "CountDemo TextBox" {
    #frame $f.f$frame -bg $Gui(activeWorkspace) 
    #pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    #}

    #$f.fTextBox config -relief groove -bd 3 

    #-------------------------------------------
    # Main->Bottom->CountDemo frame
    #-------------------------------------------
    set f $fCenterlines.fBottom.fCountDemo

    #DevAddLabel $f.lMain "You clicked 0 times."
    #pack $f.lMain -side top -padx $Gui(pad) -fill x
    #set CorCTA(lMain) $f.lMain
    
    # Here's a button with text "Count" that calls "CorCTACount" when
    # pressed.
    #DevAddButton $f.bCount Count CorCTACount 
    
    # Tooltip example: Add a tooltip for the button
    #TooltipAdd $f.bCount "Press this button to increment the counter."
    # entry box
    #eval {entry $f.eCount -width 5 -textvariable CorCTA(count) } $Gui(WEA)
    
    #pack $f.bCount $f.eCount -side left -padx $Gui(pad) -pady $Gui(pad)
    

    #-------------------------------------------
    # Main->Bottom->TextBox frame
    #-------------------------------------------
    set f $fCenterlines.fBottom.fTextBox

    # this is a convenience proc from tcl-shared/Developer.tcl
    #DevAddLabel $f.lBind "Bindings Demo"
    #pack $f.lBind -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
    
    # here's the text box widget from tcl-shared/Widgets.tcl
    #set CorCTA(textBox) [ScrolledText $f.tText]
    #pack $f.tText -side top -pady $Gui(pad) -padx $Gui(pad) \
     #   -fill x -expand true

}


#-------------------------------------------------------------------------------
# .PROC CorCTAUpdateGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAUpdateGUI {} {
    global CorCTA Volume

    DevUpdateNodeSelectButton Volume CorCTA InputVolume InputVolume DevSelectNode 0 0 1
    #DevUpdateNodeSelectButton Volume CorCTA OutputVolume OutputVolume DevSelectNode 0 1 1
}


#-------------------------------------------------------------------------------
# .PROC CorCTABuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTABuildVTK {} {
    global CorCTA
    
    vtkSphereSource CorCTA(sphereSource1)
    vtkSphereSource CorCTA(sphereSource2)
    vtkPolyDataMapper CorCTA(sphereMapper1)
    vtkPolyDataMapper CorCTA(sphereMapper2)
    vtkActor CorCTA(sphereActor1)
    vtkActor CorCTA(sphereActor2)
    
    CorCTA(sphereSource1) SetRadius 1
    CorCTA(sphereSource1) SetPhiResolution 10
    CorCTA(sphereSource1) SetThetaResolution 10
    CorCTA(sphereSource2) SetRadius 1
    CorCTA(sphereSource2) SetPhiResolution 10
    CorCTA(sphereSource2) SetThetaResolution 10
    
    CorCTA(sphereMapper1) SetInput [CorCTA(sphereSource1) GetOutput]
    CorCTA(sphereMapper2) SetInput [CorCTA(sphereSource2) GetOutput]
    CorCTA(sphereActor1) SetMapper CorCTA(sphereMapper1)
    CorCTA(sphereActor2) SetMapper CorCTA(sphereMapper2)

    [CorCTA(sphereActor1) GetProperty] SetColor 1 0 0
    [CorCTA(sphereActor2) GetProperty] SetColor 1 0 0
}

#-------------------------------------------------------------------------------
# .PROC CorCTAEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAEnter {} {
    global CorCTA Gui
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $CorCTA(eventManager)
    #vtkCellPicker CorCTA(Picker)
    #CorCTA(Picker) AddObserver EndPickEvent CorCTAPickLine
    #CorCTA(Picker) SetTolerance 0.001
    #bind $Gui(fViewWin) <Shift-Button-2> {CorCTA(Picker) Pick %x %y 0 viewRen}
}


#-------------------------------------------------------------------------------
# .PROC CorCTAExit
# Called when this module is exited by the user. Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAExit {} {
    global Gui
    
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    #bind $Gui(fViewWin) <Shift-Button-2> {}
    #CorCTA(Picker) Delete
    popEventManager
}


#-------------------------------------------------------------------------------
# .PROC CorCTAThinning
# Starts the thinning process
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAThinning {} {

    global CorCTA Volume Mrml Gui
    
    set input $CorCTA(InputVolume)
    
    set output [DevCreateNewCopiedVolume $input "" "ThinningOutput"]
    set CorCTA(OutputVolume) $output
    set node [Volume($output,vol) GetMrmlNode]
    Mrml(dataTree) RemoveItem $node
    set nodeBefore [Volume($input,vol) GetMrmlNode]
    Mrml(dataTree) InsertAfterItem $nodeBefore $node
    MainUpdateMRML

    catch "vtkImageEuclideanDistance DistTrans"
    catch "vtkThinning Thinning"
    
    Thinning SetUseLineEndpoints $CorCTA(UseLineEndpoints)
    Thinning SetUseFiducialEndpoints $CorCTA(UseFiducialEndpoints)
    Thinning SetUseSurfaceEndpoints $CorCTA(UseSurfaceEndpoints)
    
    DistTrans SetInput [Volume($input,vol) GetOutput]
    DistTrans InitializeOn
    DistTrans ConsiderAnisotropyOn
    DistTrans SetMaximumDistance 50
    DistTrans SetAlgorithmToSaitoCached
    set Gui(progressText) "Computing distance transform"
    DistTrans AddObserver StartEvent MainStartProgress
    DistTrans AddObserver ProgressEvent "MainShowProgress DistTrans"
    DistTrans AddObserver EndEvent MainEndProgress
    DistTrans Update
    set Gui(progressText) "Thinning"
    Thinning AddObserver StartEvent MainStartProgress
    Thinning AddObserver ProgressEvent "MainShowProgress Thinning"
    Thinning AddObserver EndEvent MainEndProgress
    Thinning SetInput [Volume($input,vol) GetOutput]
    Thinning SetMaxThreshold 100000
    Thinning SetCriterion [DistTrans GetOutput]
    Thinning Update
    Volume($output,vol) SetImageData [Thinning GetOutput]
    Volume($output,vol) Update
    Thinning UnRegisterAllOutputs
    Thinning Delete
    DistTrans Delete
    set Volume(activeID) $output
    MainVolumesSetParam Window 10
    MainVolumesSetParam Level 127
    MainVolumesRender
    MainVolumesUpdate $output
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC CorCTAConvert
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTAConvert {} {
    global CorCTA Volume Model Module Gui
    
    set output $CorCTA(OutputVolume)
    
    catch "vtkSkeleton2Lines Converter"
    
    set Gui(progressText) "Creating lines"
    Converter AddObserver StartEvent MainStartProgress
    Converter AddObserver ProgressEvent "MainShowProgress Converter"
    Converter AddObserver EndEvent MainEndProgress
    
    set n [MainMrmlAddNode Model]
    $n SetName $CorCTA(ModelName)
    
    set m [$n GetID]
    MainModelsCreate $m
    Model($m,node) SetRasToWld [Volume($output,node) GetRasToWld]
    
    Converter SetInput [Volume($output,vol) GetOutput]
    Converter Update
    [Converter GetOutput] ReleaseDataFlagOff
    
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot
    # special trick to avoid vtk 4.2 legacy hack message
    # (adds a concatenated identity transform to the transform)
    #if { [info commands __dummy_transform] == "" } {
    #  vtkTransform __dummy_transform
    #}
    #rot SetInput __dummy_transform
    set matrixList [Volume($output,node) GetRasToVtkMatrix]
    for {set row 0} { $row < 4 } {incr row} {
        for {set col 0} {$col < 4} {incr col} {
            [rot GetMatrix] SetElement $row $col \
                [lindex $matrixList [expr $row*4+$col]]
        }
    }
    [rot GetMatrix] Invert
    
    catch "vtkTransformPolyDataFilter Transformer"
    Transformer SetInput [Converter GetOutput]
    Transformer SetTransform rot
    Transformer Update
    [Transformer GetOutput] ReleaseDataFlagOff
    
    if {$CorCTA(Splines) == 1} {
      catch "vtkSplineFilter Smoother"
      Smoother SetInput [Transformer GetOutput]
      Smoother SetSubdivideToLength
      Smoother SetLength $CorCTA(SplineSegmentLength)  
      Smoother Update
      [Smoother GetOutput] ReleaseDataFlagOff
      set Model($m,polyData) [Smoother GetOutput]
    } else {
      set Model($m,polyData) [Transformer GetOutput]
    }
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
      Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    set Model($m,dirty) 1
    
    MainMrmlDeleteNode Volume $output
    MainVolumesDelete $output

    MainUpdateMRML

    if {$CorCTA(Splines) == 1} {
      Smoother Delete
    }

    Transformer Delete
    Converter Delete
    rot Delete
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC CorCTACenterlines
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc CorCTACenterlines {} {
    CorCTAThinning
    CorCTAConvert
}


#-------------------------------------------------------------------------------
# .PROC CorCTAPickLine
# 
# .ARGS
# vtkActor Actor 
# int CellId
# .END
#-------------------------------------------------------------------------------
proc CorCTAPickLine {Actor CellId} {
    global CorCTA
    
    if {$CellId >= 0} {
      set TempMapper [$Actor GetMapper]
      set TempData [$TempMapper GetInput]
      set PickedCell [$TempData GetCell $CellId]
      set nPoints [$PickedCell GetNumberOfPoints]
      puts "You picked cell # $CellId with $nPoints points."
      
      set Points [$PickedCell GetPoints]
      #set PointIds [$PickedCell GetPointIds]
      
      set FirstId [$PickedCell GetPointId 0]
      #set FirstId [$PointIds GetId 0]
      set LastId [$PickedCell GetPointId [expr $nPoints-1]]
      #set LastId [$PointIds GetId [expr $nPoints-1]]
      set first [$Points GetPoint $FirstId]
      set last [$Points GetPoint $LastId]
      puts "First point: $first, last point: $last"
      
      CorCTA(sphereActor1) SetPosition [lindex $first 0] [lindex $first 1] [lindex $first 2]
      CorCTA(sphereActor2) SetPosition [lindex $last 0] [lindex $last 1] [lindex $last 2]
      
      viewRen AddActor CorCTA(sphereActor1)
      viewRen AddActor CorCTA(sphereActor2)
      
      RenderAll
    }
}
