#-------------------------------------------------------------------------------
# .PROC TransformVolumeInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TransformVolumeInit {} {
    global TransformVolume Module Volume Model
    
    set m TransformVolume
    
    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is an example of how to add modules to slicer."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "First name, last name, affiliation, email"
    
    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "Example"
    
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

    set Module($m,row1List) "Help Main Resample"
    set Module($m,row1Name) "{Help} {Main} {Resample}"
    set Module($m,row1,tab) Main
    
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
    #   set Module($m,procVTK) TransformVolumeBuildVTK
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
    set Module($m,procGUI) TransformVolumeBuildGUI
    set Module($m,procVTK) TransformVolumeBuildVTK
    set Module($m,procEnter) TransformVolumeEnter
    set Module($m,procExit) TransformVolumeExit
    set Module($m,procMRML) TransformVolumeUpdateGUI

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
        {$Revision: 1.4 $} {$Date: 2005/03/14 22:55:50 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set TransformVolume(resultPrefix) "xformed"
    set TransformVolume(ForceOutputOrientation) ""
    set TransformVolume(VolIDs) ""
    set TransformVolume(RefVolume) ""
    set TransformVolume(DispVolume) ""
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
# .PROC TransformVolumeBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TransformVolumeBuildVTK {} {

}
proc TransformVolumeEnter {} {
    global TransformVolume
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)

    #pushEventManager $TransformVolume(eventManager)
}


#-------------------------------------------------------------------------------
# .PROC TransformVolumeExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TransformVolumeExit {} {
    global TransformVolume
    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    #popEventManager
}


proc TransformVolumeBuildGUI {} {
    global Gui TransformVolume Module Volume 
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Main" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(TransformVolume,fMain)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Main
    #-------------------------------------------
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The TransformVolume module transforms all volumes inside a slected Transformation node using trasformation matrices inside the Transformation node and a specified deformable transformation defined by a selected displacement volume containing 2 or 3 components displacement vectors.  The resulting volumes have the same spacing and orientation as specified reference volume. The source code is in slicer2/Modules/vtkTransformVolume/tcl/TransformVolume.tcl.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags TransformVolume $help
    MainHelpBuildGUI TransformVolume
    
    #-------------------------------------------
    # Main frame
    #-------------------------------------------
    set fMain $Module(TransformVolume,fMain)

    set f $fMain.fTitle
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    
    set f $fMain  
    
    # Transform node
    istransformoption  $f.matopt -labeltext "Transform Node" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5 \
        -command TransformVolumeTransform

    set TransformVolume(transform) $f.matopt
    
    $TransformVolume(transform) allowNone 0
    
    # Displacement volume
    isvolumeoption  $f.volopt -labeltext "Deformation Volume" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5 \
        -command TransformVolumeDispVolume
    set TransformVolume(displacementVol) $f.volopt
    
    $TransformVolume(displacementVol) numScalars 3
    $TransformVolume(displacementVol) allowNone 1
   
    # Reference volume
    isvolumeoption  $f.volref -labeltext "Reference(like) Volume" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5 \
        -command TransformVolumeRefVolume

    set TransformVolume(referenceVol) $f.volref

    $TransformVolume(referenceVol) allowNone 1

    iwidgets::Labeledwidget::alignlabels $f.matopt $f.volopt $f.volref

    pack $f.matopt $f.volopt $f.volref -side top -padx $Gui(pad) -pady $Gui(pad) \
        -fill x \
        -expand yes

    # Result prefix
    set f $fMain.fPrefix
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    DevAddLabel $f.lPrefix "Result Prefix:"
    eval {entry $f.ePrefix -textvariable TransformVolume(resultPrefix)} $Gui(WEA)
    pack $f.lPrefix -side left -padx $Gui(pad)
    pack $f.ePrefix -side left -padx $Gui(pad) -expand 1 -fill x

    # Interpolation Mode
    set f $fMain.fInterpolationMode
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x

    eval {label $f.lInterpolationMode -text "Interpolation Mode:"} $Gui(WLA)
    #Create label foreach type of interpolation
    foreach label {"NearestNeighbor" "Linear" "Cubic"} text {"Nearest Neighbor" "Linear" "Cubic"} {
        eval {radiobutton $f.rb$label -text $text -variable TransformVolume(InterpolationMode) -value $label} $Gui(WLA)
    }
    set TransformVolume(InterpolationMode) Cubic
    pack $f.lInterpolationMode -side top -padx $Gui(pad) -pady $Gui(pad)
    pack $f.rbNearestNeighbor $f.rbLinear $f.rbCubic -side left -anchor w -padx 1 -pady $Gui(pad)


    # Transform button
    set f $fMain.fRun
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x
    DevAddButton $f.bRun "Do Transform" "TransformVolumeRun"
    pack $f.bRun -pady $Gui(pad)
    TooltipAdd $f.bRun "Transform volumes"


    #-------------------------------------------
    # Resample frame
    #-------------------------------------------
    set fResample $Module(TransformVolume,fResample)
    
    set f $fResample.fTitle
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    
    set f $fResample  

    # Reasmple->Orientation
    iwidgets::radiobox $f.rborient -labeltext "New Orientation" -labelpos n \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -orient horizontal \
        -command TransformVolumeOrientation

    foreach or {"LR" "RL" "PA" "AP" "IS" "SI"} {
        $f.rborient add $or -text $or
    }
    pack $f.rborient -side top -padx $Gui(pad) -pady $Gui(pad)
    set TransformVolume(orientation) $f.rborient


    # Resample->Spacing
    set f $fResample.fOutputSpacing
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x

    set f $fResample.fOutputSpacing
    eval {label $f.lOutputSpacing -text "New Spacing"} $Gui(WLA)
    pack $f.lOutputSpacing -side top -padx $Gui(pad) -pady $Gui(pad)
    eval {label $f.lOutputSpacingI -text "I:"} $Gui(WLA)
    eval {entry $f.eOutputSpacingI -width 6 -textvariable TransformVolume(OutputSpacingI) } $Gui(WEA)
    eval {label $f.lOutputSpacingJ -text "J:"} $Gui(WLA)
    eval {entry $f.eOutputSpacingJ -width 6 -textvariable TransformVolume(OutputSpacingJ) } $Gui(WEA)
    eval {label $f.lOutputSpacingK -text "K:"} $Gui(WLA)
    eval {entry $f.eOutputSpacingK -width 6 -textvariable TransformVolume(OutputSpacingK) } $Gui(WEA)
    pack $f.lOutputSpacingI $f.eOutputSpacingI $f.lOutputSpacingJ $f.eOutputSpacingJ $f.lOutputSpacingK $f.eOutputSpacingK -side left -padx $Gui(pad) -pady $Gui(pad)
    
    # Resample->Dimenisons
    set f $fResample.fOutputDimension
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x

    set f $fResample.fOutputDimension
    eval {label $f.lOutputDimension -text "New Dimensions"} $Gui(WLA)
    pack $f.lOutputDimension -side top -padx $Gui(pad) -pady $Gui(pad)
    eval {label $f.lOutputDimensionI -text "I:"} $Gui(WLA)
    eval {entry $f.eOutputDimensionI -width 6 -textvariable TransformVolume(OutputDimensionI) } $Gui(WEA)
    eval {label $f.lOutputDimensionJ -text "J:"} $Gui(WLA)
    eval {entry $f.eOutputDimensionJ -width 6 -textvariable TransformVolume(OutputDimensionJ) } $Gui(WEA)
    eval {label $f.lOutputDimensionK -text "K:"} $Gui(WLA)
    eval {entry $f.eOutputDimensionK -width 6 -textvariable TransformVolume(OutputDimensionK) } $Gui(WEA)
    pack $f.lOutputDimensionI $f.eOutputDimensionI $f.lOutputDimensionJ $f.eOutputDimensionJ $f.lOutputDimensionK $f.eOutputDimensionK -side left -padx $Gui(pad) -pady $Gui(pad)
   
    # Resample->Set Extent
    set f $fResample.fSetExtent
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x
  
    set f $fResample.fSetExtent
    DevAddButton $f.bIsoExtentI " Auto I "  AutoDimensionI
    DevAddButton $f.bIsoExtentJ " Auto J "  AutoDimensionJ
    DevAddButton $f.bIsoExtentK " Auto K "  AutoDimensionK
    pack $f.bIsoExtentI  $f.bIsoExtentJ $f.bIsoExtentK -side left -padx $Gui(pad)
    
    # Resample->Auto Extent
    set f $fResample.fAutoDimension
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x
  
    set f $fResample.fAutoDimension
    DevAddButton $f.bAutoDimension "Auto Dimension"  AutoDimension
    pack $f.bAutoDimension -side left -padx $Gui(pad) -pady $Gui(pad)

    catch "destroy .isv"

    isvolume $fResample.isv
    pack $fResample.isv -side top -padx $Gui(pad) -pady $Gui(pad)

    set TransformVolume(isv) $fResample.isv

}

proc TransformVolumeUpdateGUI {} {
    global TransformVolume
    $TransformVolume(displacementVol) initSelection
    $TransformVolume(referenceVol) initSelection
    $TransformVolume(transform) initSelection
}

proc TransformVolumeRun {} {
    global TransformVolume Volume
    
    # get displacement volume
    set vDisp $TransformVolume(DispVolume)
    
    isvolume .isv
    
    .isv volmenu_update

    foreach v $TransformVolume(VolIDs) {
        
        puts " TransformVolume : transforming volume [Volume($v,node) GetName]"
        
        .isv configure -volume [Volume($v,node) GetName]
        
        .isv configure -orientation $TransformVolume(OutputOrientation)

        .isv configure -interpolation $TransformVolume(InterpolationMode)

        .isv set_spacing $TransformVolume(OutputSpacingI) \
            $TransformVolume(OutputSpacingJ) \
            $TransformVolume(OutputSpacingK)
        .isv set_dimensions $TransformVolume(OutputDimensionI) \
            $TransformVolume(OutputDimensionJ) \
            $TransformVolume(OutputDimensionK)
        
        if {$vDisp != "" && $vDisp != $Volume(idNone)} {
            .isv configure -warpvolume [Volume($vDisp,node) GetName]
        }
        
        # create result name
        set resVolName $TransformVolume(resultPrefix)
        append resVolName "_" [Volume($v,node) GetName]
        
        # check if the result name exists already
        set exists [TransformVolumeVolumeExists $resVolName]
        if {$exists == "1"} {
            set count 0
            while {1} {
                set name $resVolName
                append name $count
                set exists [TransformVolumeVolumeExists $name]
                if {$exists == "0"} {
                    set resVolName $name
                    break
                }
                incr count
            }
        }

        # get the volume
        .isv slicer_volume $resVolName
        
    }
}

proc TransformVolumeGetVolumes {transformId} {
    global Data Mrml Transform EndTransform
    
    set volIDs ""

    if {$transformId != ""} {
        set tree Mrml(dataTree)
        $tree InitTraversal
        set node [$tree GetNextItem]
        
        set insideTransform 0
        set done 0
        while {$node != ""} {
            
            set nodeClass [$node GetClassName]
            
            switch $nodeClass {
                "vtkMrmlVolumeNode" {
                    if {$insideTransform != 0} {
                        set id [DataGetIdFromNode $node]
                        lappend volIDs $id
                    }
                }
                "vtkMrmlTransformNode" {
                    if {[$node GetName] == [Transform($transformId,node) GetName]} {
                        incr insideTransform
                    }
                }
                "vtkMrmlEndTransformNode" {
                    if {[$node GetName] == [EndTransform($transformId,node) GetName]} {
                        incr insideTransform -1
                        set done 1
                    }
                }
            }
            if {$done} {
                break
            }
            set node [$tree GetNextItem]
        }
    }
    return $volIDs
}

#-----------------------------

proc TransformVolumeVolumeExists {name} {
    global Volume
    foreach v $Volume(idList) {
        set index [lsearch -exact $name [Volume($v,node) GetName]]
        if {$index > -1} {
            # name exists
            return 1
        }
    }
    return 0
}

#-----------------------------
proc TransformVolumeTransform {} {
    global TransformVolume

    set TransformVolume(VolIDs) [TransformVolumeGetVolumes [$TransformVolume(transform) selectedID]]
    set TransformVolume(ForceOutputOrientation) ""

    TransformVolumeUpdateResample
    TransformVolumeUpdatePreview
}

#-----------------------------
proc TransformVolumeDispVolume {} {
    global TransformVolume

    set TransformVolume(DispVolume) [$TransformVolume(displacementVol) selectedID]

    TransformVolumeUpdatePreview
}

#-----------------------------
proc TransformVolumeRefVolume {} {
    global TransformVolume

    set TransformVolume(RefVolume) [$TransformVolume(referenceVol) selectedID]
    set TransformVolume(ForceOutputOrientation) ""

    TransformVolumeUpdateResample
    TransformVolumeUpdatePreview
}

#-----------------------------
proc TransformVolumeOrientation {} {
    global TransformVolume

    set TransformVolume(OutputOrientation) [$TransformVolume(orientation) get]
    set TransformVolume(ForceOutputOrientation) "1"
    TransformVolumeUpdatePreview
}

#-----------------------------
proc TransformVolumeUpdateResample {} {
    global TransformVolume Volume

    # set reference volume
    set vRef $TransformVolume(RefVolume)

    # get volumes to transform under a transform node
    set volIDs $TransformVolume(VolIDs)
        
    set v $Volume(idNone)

    if {$vRef == "" || $vRef == $Volume(idNone)} {
        # use source as reference
        if {[llength $volIDs] != 0} {
            set v [lindex $volIDs 0]
        }
    } else {
        # use reference volume
        set v $vRef
    }
    
    if {$v != $Volume(idNone)} {
        set spacing [split [[Volume($v,vol) GetOutput] GetSpacing]] 

        set TransformVolume(OutputSpacingI) [lindex $spacing 0] 
        set TransformVolume(OutputSpacingJ) [lindex $spacing 1] 
        set TransformVolume(OutputSpacingK) [lindex $spacing 2] 

        set dimension [split [[Volume($v,vol) GetOutput] GetDimensions]] 
        
        set TransformVolume(OutputDimensionI) [expr round(abs([lindex $dimension 0]))]
        set TransformVolume(OutputDimensionJ) [expr round(abs([lindex $dimension 1]))]
        set TransformVolume(OutputDimensionK) [expr round(abs([lindex $dimension 2]))]

        if {$TransformVolume(ForceOutputOrientation) == ""} {
            set TransformVolume(OutputOrientation) [Volume($v,node) GetScanOrder]
        }
    }
    set TransformVolume(ForceOutputOrientation) ""
    $TransformVolume(orientation) select $TransformVolume(OutputOrientation)
}

#-----------------------------
proc TransformVolumeUpdatePreview {} {
    global TransformVolume Volume

    # get volumes to transform under a transform node
    set volIDs $TransformVolume(VolIDs)
        
    set v $Volume(idNone)

    if {[llength $volIDs] != 0} {
        set v [lindex $volIDs 0]
    }
    
    # get displacement volume
    set vDisp $TransformVolume(DispVolume)
    
    if {$v != $Volume(idNone)} {

        set isv $TransformVolume(isv)

        $isv volmenu_update

        $isv configure -volume [Volume($v,node) GetName]
        
        $isv configure -orientation $TransformVolume(OutputOrientation)

        $isv configure -interpolation $TransformVolume(InterpolationMode)

        $isv set_spacing $TransformVolume(OutputSpacingI) \
            $TransformVolume(OutputSpacingJ) \
            $TransformVolume(OutputSpacingK)
        $isv set_dimensions $TransformVolume(OutputDimensionI) \
            $TransformVolume(OutputDimensionJ) \
            $TransformVolume(OutputDimensionK)
        
        if {$vDisp != "" && $vDisp != $Volume(idNone)} {
            $isv configure -warpvolume [Volume($vDisp,node) GetName]
        }
        
        $isv configure -slice [expr int(0.5*$TransformVolume(OutputDimensionK))]
    }
        
}


#-------------------------------------------------------------------------------
# .PROC AutoExtent
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoDimension {} {
    global TransformVolume Module Matrix Volume

    set volIDs [TransformVolumeGetVolumes [$TransformVolume(transform) selectedID]]
        
    set v $Volume(idNone)

    if {[llength $volIDs] != 0} {
        set v [lindex $volIDs 0]
    }
    if {$v != $Volume(idNone)} {
        
        #Set default values
        # Make a RAS to VTK matrix for realign resample
        # based on the position matrix
        catch "ModelRasToVtk Delete"
        vtkMatrix4x4 ModelRasToVtk
        set position [Volume($v,node) GetPositionMatrix]
        
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
        
        # Now we can build the Vtk1ToVtk2 matrix based on
        # ModelRasToVtk and ras1toras2
        # vtk1tovtk2 = inverse(rastovtk) ras1toras2 rastovtk
        # RasToVtk
        catch "RasToVtk Delete"
        vtkMatrix4x4 RasToVtk
        RasToVtk DeepCopy ModelRasToVtk    
        # Inverse Matrix RasToVtk
        catch "InvRasToVtk Delete"
        vtkMatrix4x4 InvRasToVtk
        InvRasToVtk DeepCopy ModelRasToVtk
        InvRasToVtk Invert
        # Ras1toRas2 given by the slicer MRML tree
        catch "Ras1ToRas2 Delete"    
        vtkMatrix4x4 Ras1ToRas2
        #Ras1ToRas2 DeepCopy [[Matrix($Matrix(activeID),node) GetTransform] GetMatrix]
        Ras1ToRas2 DeepCopy [Volume($v,node) GetRasToWld]
        # Now build Vtk1ToVtk2
        catch "Vtk1ToVtk2 Delete"    
        vtkMatrix4x4 Vtk1ToVtk2
        Vtk1ToVtk2 Identity
        Vtk1ToVtk2 Multiply4x4 Ras1ToRas2 RasToVtk  Vtk1ToVtk2
        Vtk1ToVtk2 Multiply4x4 InvRasToVtk  Vtk1ToVtk2 Vtk1ToVtk2
        
        # Get the origin, spacing and extent of the input volume
        catch "InVolume Delete"
        vtkImageData InVolume
        InVolume DeepCopy [Volume($v,vol) GetOutput]
        catch "ici Delete"    
        vtkImageChangeInformation ici
        ici CenterImageOn
        ici SetInput InVolume
        ici Update
        set volume [ici GetOutput]
        set inorigin [split [$volume GetOrigin]]
        set inwholeExtent [split [$volume GetWholeExtent]]
        set inspacing [split [$volume GetSpacing]]
        
        # Transforms the corners of the extent according to Vtk1ToVtk2
        # and finds the min/max coordinates in each dimension
        for {set i 0} {$i < 3} {incr i} {
            set bounds([expr 2 * $i]) 10000000
            set bounds([expr 2*$i+1])  -10000000
        }
        for {set i 0} {$i < 8} {incr i} {
            # setup the bounding box with origin and spacing
            set point(0) [expr [lindex $inorigin 0] + [expr [lindex $inwholeExtent [expr $i %  2]] * [lindex $inspacing 0] ]]
            set point(1) [expr [lindex $inorigin 1] + [expr [lindex $inwholeExtent [expr 2 + ($i / 2) % 2]] * [lindex $inspacing 1]]]
            set point(2) [expr [lindex $inorigin 2] + [expr [lindex $inwholeExtent [expr 4 + ($i / 4) % 2]] * [lindex $inspacing 2]]]
            # applies the transform 
            set newpoint [Vtk1ToVtk2 MultiplyPoint $point(0) $point(1) $point(2) 1]
            set point(0) [lindex $newpoint 0]
            set point(1) [lindex $newpoint 1]
            set point(2) [lindex $newpoint 2]
            # finds max/min in each dimension
            for {set j 0}  {$j < 3} {incr j} {
                if {$point($j) > $bounds([expr 2*$j+1])} {
                    set bounds([expr 2*$j+1]) $point($j)
                }
                if {$point($j) < $bounds([expr 2*$j])} {
                    set bounds([expr 2*$j]) $point($j)
                }
            }
        }
        
        # Transforms in RAS space
        set outspacing [InvRasToVtk MultiplyPoint $TransformVolume(OutputSpacingI) $TransformVolume(OutputSpacingJ) $TransformVolume(OutputSpacingK) 1]
        set spacing(0) [expr abs([lindex $outspacing 0])]
        set spacing(1) [expr abs([lindex $outspacing 1])]
        set spacing(2) [expr abs([lindex $outspacing 2])]
        # Compute the new extent
        for {set i 0} {$i < 3} {incr i} {
            set outExt($i) [expr round (( $bounds([expr 2*$i+1])- $bounds([expr 2 * $i])) / $spacing($i))] 
        }
        # Go back in RAS space 
        set outExtRAS [RasToVtk MultiplyPoint $outExt(0) $outExt(1) $outExt(2) 1]
        set TransformVolume(OutputDimensionI) [expr 1 + round(abs([lindex $outExtRAS 0]))]
        set TransformVolume(OutputDimensionJ) [expr 1 + round(abs([lindex $outExtRAS 1]))]
        set TransformVolume(OutputDimensionK) [expr 1 + round(abs([lindex $outExtRAS 2]))]
        
        InVolume Delete
        ici Delete
        ModelRasToVtk Delete
        Ras1ToRas2 Delete
        RasToVtk Delete
        InvRasToVtk Delete
        Vtk1ToVtk2 Delete

    }
    TransformVolumeUpdatePreview
}

#-------------------------------------------------------------------------------
# .PROC AutoDimensionI
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoDimensionI {} {
    global TransformVolume
    set tmpZ $TransformVolume(OutputDimensionJ)
    set tmpY $TransformVolume(OutputDimensionK)
    AutoDimension
    set TransformVolume(OutputDimensionJ) $tmpZ
    set TransformVolume(OutputDimensionK) $tmpY
    TransformVolumeUpdatePreview
}  

#-------------------------------------------------------------------------------
# .PROC AutoDimensionJ
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoDimensionJ {} {
    global TransformVolume
    set tmpY $TransformVolume(OutputDimensionK)
    set tmpX $TransformVolume(OutputDimensionI)
    AutoDimension
    set TransformVolume(OutputDimensionK) $tmpY
    set TransformVolume(OutputDimensionI) $tmpX
    TransformVolumeUpdatePreview
} 

#-------------------------------------------------------------------------------
# .PROC AutoDimensionK
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AutoDimensionK {} {
    global TransformVolume
    set tmpZ $TransformVolume(OutputDimensionJ)
    set tmpX $TransformVolume(OutputDimensionI)
    AutoDimension
    set TransformVolume(OutputDimensionJ) $tmpZ
    set TransformVolume(OutputDimensionI) $tmpX
    TransformVolumeUpdatePreview   
} 
