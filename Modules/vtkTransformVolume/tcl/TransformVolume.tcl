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

    set Module($m,row1List) "Help Main"
    set Module($m,row1Name) "{Help} {Main} "
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
        {$Revision: 1.2 $} {$Date: 2005/03/07 20:41:53 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set TransformVolume(resultPrefix) "xformed"

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
    set f $fMain
    
    set f $fMain.fTitle
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    
    set f $fMain  
    
    # Transform node
    istransformoption  $f.matopt -labeltext "Transform Node" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5
    set TransformVolume(transform) $f.matopt
    
    $TransformVolume(transform) allowNone 0
    
    # Displacement volume
    isvolumeoption  $f.volopt -labeltext "Deformation Volume" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5
    set TransformVolume(displacementVol) $f.volopt
    
    $TransformVolume(displacementVol) numScalars 3
    $TransformVolume(displacementVol) allowNone 1
   
    # Referebce volume
    isvolumeoption  $f.volref -labeltext "Reference(like) Volume" -labelpos w \
        -background "#e2cdba" -foreground "#000000" \
        -labelfont {helvetica 8} -labelmargin 5
    set TransformVolume(referenceVol) $f.volref

    $TransformVolume(referenceVol) allowNone 1

    iwidgets::Labeledwidget::alignlabels $f.matopt $f.volopt $f.volref

    pack $f.matopt $f.volopt $f.volref -side top -padx $Gui(pad) -pady $Gui(pad) \
        -fill x \
        -expand yes

#    pack $f.volopt -side top -padx $Gui(pad) -pady $Gui(pad)
#    pack $f.volref -side top -padx $Gui(pad) -pady $Gui(pad)


    # Result prefix
    set f $fMain.fPrefix
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    DevAddLabel $f.lPrefix "Result Prefix:"
    eval {entry $f.ePrefix -textvariable TransformVolume(resultPrefix)} $Gui(WEA)
    pack $f.lPrefix -side left -padx $Gui(pad)
    pack $f.ePrefix -side left -padx $Gui(pad) -expand 1 -fill x

    # Transform button
    set f $fMain.fRun
    frame $f -bg $Gui(activeWorkspace)
    pack $f -side top -padx 0 -pady $Gui(pad) -fill x
    DevAddButton $f.bRun "Do Transform" "TransformVolumeRun"
    pack $f.bRun -pady $Gui(pad)
    TooltipAdd $f.bRun "Transform volumes"
}

proc TransformVolumeUpdateGUI {} {
    global TransformVolume
    $TransformVolume(displacementVol) initSelection
    $TransformVolume(referenceVol) initSelection
    $TransformVolume(transform) initSelection
}

proc TransformVolumeRun {} {
   global TransformVolume Volume
    
    
    # set reference volume
    set vRef [$TransformVolume(referenceVol) selectedID]
    
    # get displacement volume
    set vDisp [$TransformVolume(displacementVol) selectedID]
    
    # get volumes to transform under a transform node
    set volIDs [TransformVolumeGetVolumes [$TransformVolume(transform) selectedID]]
    catch "destroy .isv"
    
    isvolume .isv

    foreach v $volIDs {
        
        puts " TransformVolume : transforming volume [Volume($v,node) GetName]"
        
        if {$vRef == "" || $vRef == $Volume(idNone)} {
            #use source as reference
            set vRef $v
        }
        
        .isv configure -volume [Volume($v,node) GetName]
        
        .isv configure -refvolume [Volume($vRef,node) GetName]
        
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
