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
# FILE:        vtkFreeSurferReaders.tcl
# PROCEDURES:  
#   vtkFreeSurferReadersBuildGUI
#   vtkFreeSurferReadersEnter
#   vtkFreeSurferReadersExit
#   vtkFreeSurferReadersSetVolumeFileName
#   vtkFreeSurferReadersApply
#   vtkFreeSurferReadersBuildSurface
#   vtkFreeSurferReadersSetSurfaceVisibility
#   vtkFreeSurferReadersMainFileCloseUpdate
#   vtkFreeSurferReadersAddColors
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersInit {} {
    global vtkFreeSurferReaders Module Volume Model env

    set m vtkFreeSurferReaders

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set vtkFreeSurferReaders(assocFiles) ""
    set vtkFreeSurferReaders(scalars) "thickness curv sulc area"
    set vtkFreeSurferReaders(surfaces) "inflated pial smoothwm sphere"
    set vtkFreeSurferReaders(annots) "aparc cma_aparc" 
    set vtkFreeSurferReaders(castToShort) 1
    # flag to load in free surfer colour file when loading a label map
    set vtkFreeSurferReaders(loadColours) 1
    # Set up the file name of the free surfer modules' colour xml file, 
    # it's in the Module's tcl directory. Try setting it from the slicer home
    # environment variable first, otherwise, assume search  is starting from slicer home
    if {[info exists env(SLICER_HOME)] == 1} {
        set vtkFreeSurferReaders(colourFileName) [file join $env(SLICER_HOME) Modules vtkFreeSurferReaders tcl ColorsFreesurfer.xml]
    } else {
        set vtkFreeSurferReaders(colourFileName) [file join Modules vtkFreeSurferReaders tcl ColorsFreesurfer.xml]
    }

    # the default colour table file name
    set vtkFreeSurferReaders(colorTableFilename) [ExpandPath [file join $::PACKAGE_DIR_VTKFREESURFERREADERS ".." ".." ".." tcl "Simple_surface_labels2002.txt"]]

    # for closing out a scene
    set vtkFreeSurferReaders(idList) ""
    set Module($m,procMainFileCloseUpdateEntered) vtkFreeSurferReadersMainFileCloseUpdate

    # set up the mapping between the surface labels and umls ids
    vtkFreeSurferReadersSetUMLSMapping

    lappend Module($m,fiducialsPointCreatedCallback) FreeSurferReadersFiducialsPointCreatedCallback

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module allows you to read in FreeSurfer volume, model, and scalar files."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Nicole Aucoin, BWH, nicole@bwh.harvard.edu"
    set Module($m,category) "IO"

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
    #   row1Name = list of Names for tabs. Names appear on the user interface
    #              and can be non-unique with multiple words.
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Help Volumes Models"
    set Module($m,row1Name) "{Help} {Volumes} {Models}"
    set Module($m,row1,tab) Volumes

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name, which
    #   can be anything, is registered with a line like this:
    #
    #   set Module($m,procVTK) vtkFreeSurferReadersBuildVTK
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
    set Module($m,procGUI) vtkFreeSurferReadersBuildGUI
    set Module($m,procVTK) vtkFreeSurferReadersBuildVTK
    set Module($m,procEnter) vtkFreeSurferReadersEnter
    set Module($m,procExit) vtkFreeSurferReadersExit
    set Module($m,procMainFileCloseUpdateEntered) vtkFreeSurferReadersMainFileCloseUpdate

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # register the procedures in this file that will read in volumes
    set Module($m,readerProc,MGH) vtkFreeSurferReadersReadMGH
    set Module($m,readerProc,bfloat) vtkFreeSurferReadersReadBfloat

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.7 $} {$Date: 2004/11/12 19:32:08 $}]

}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersBuildGUI
# Builds the GUI for the free surfer readers, as a submodule of the Volumes module
# .ARGS
# parentFrame the frame in which to build this Module's GUI
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersBuildGUI {} {
    global Gui Volume Module vtkFreeSurferReaders

    if {$Module(verbose) == 1} {
        puts  "vtkFreeSurferReadersBuildGUI"
    }

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "The vtkFreeSuferReaders module allows you to read in FreeSufer format files.<P>Description by tab:<BR><UL><LI><B>Volumes</B>: Load in COR. mgh volumes.<LI><B>Models</B>: Load in model files (${vtkFreeSurferReaders(surfaces)}) and associate scalars with them.(${vtkFreeSurferReaders(scalars)})."
    regsub -all "\n" $help {} help
    MainHelpApplyTags vtkFreeSurferReaders $help
    MainHelpBuildGUI vtkFreeSurferReaders

    #-------------------------------------------
    # Volumes Frame
    #-------------------------------------------
    set fVolumes $Module(vtkFreeSurferReaders,fVolumes)
    set f $fVolumes

    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $f.fLogo -bg $Gui(activeWorkspace)
    frame $f.fDemo -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $f.fLogo $f.fVolume $f.fDemo \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Logo frame
    #-------------------------------------------
    set f $fVolumes.fLogo
    set logoFile [ExpandPath [file join $::PACKAGE_DIR_VTKFREESURFERREADERS ".." ".." ".." tcl images "FreeSurferlogo.ppm"]]
    if {[file exists $logoFile]} {
        image create photo iFSLogo -file $logoFile
        eval {label $f.lLogo -image iFSLogo -width 179 -height 30 -anchor center} \
            -bg $Gui(activeWorkspace)   -padx 0 -pady 0 
    } else {
        eval {label $f.lLogo -text "FreeSurfer" -anchor center} $Gui(WLA)
    }
    pack $f.lLogo 

    #-------------------------------------------
    # fVolume frame
    #-------------------------------------------

    set f $fVolumes.fVolume

    DevAddFileBrowse $f  vtkFreeSurferReaders "VolumeFileName" "FreeSurfer File:" "vtkFreeSurferReadersSetVolumeFileName" "" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer volume file (.info, .mgh, .bfloat, .bshort)" 

    frame $f.fLabelMap -bg $Gui(activeWorkspace)
    frame $f.fCast  -bg $Gui(activeWorkspace)
    frame $f.fColours -bg $Gui(activeWorkspace)
    frame $f.fDesc     -bg $Gui(activeWorkspace)
    frame $f.fName -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)

    pack $f.fLabelMap -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fCast -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fColours -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fApply -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    #---------
    # Volume->Name 
    #---------
    set f $fVolumes.fVolume.fName
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable Volume(name) -width 13} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad) 

    #---------
    # Volume->Desc
    #---------
    set f $fVolumes.fVolume.fDesc

    eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
    eval {entry $f.eDesc -textvariable Volume(desc)} $Gui(WEA)
    pack $f.lDesc -side left -padx $Gui(pad)
    pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x

    #---------
    # Volume->LabelMap
    #---------
    set f $fVolumes.fVolume.fLabelMap

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)

    pack $f.fTitle $f.fBtns -side left -pady 5

    DevAddLabel $f.fTitle.l "Image Data:"
    pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

    foreach text "{Grayscale} {Label Map}" \
        value "0 1" \
        width "9 9 " {
        eval {radiobutton $f.fBtns.rMode$value -width $width \
            -text "$text" -value "$value" -variable Volume(labelMap) \
            -indicatoron 0 } $Gui(WCA)
        pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
    }
    if {$Module(verbose) == 1} {
        puts "Done packing the label map stuff"
    }

    #------------
    # Volume->Cast 
    #------------
    set f $fVolumes.fVolume.fCast
    eval {checkbutton $f.cCastToShort \
        -text "Cast to short" -variable vtkFreeSurferReaders(castToShort) -width 13 \
        -indicatoron 0 -command "vtkFreeSurferReadersSetCast"} $Gui(WCA)
    TooltipAdd $f.cCastToShort "Cast this volume to short when reading it in. This allows use of the editing tools."
    pack $f.cCastToShort -side top -padx 0

    
    #------------
    # Volume->Colours
    #------------
    set f $fVolumes.fVolume.fColours
    eval {checkbutton $f.cLoadColours \
              -text "Load FreeSurfer Colors" -variable vtkFreeSurferReaders(loadColours) -width 23 \
              -indicatoron 0 -command "vtkFreeSurferReadersSetLoadColours"} $Gui(WCA)
    TooltipAdd $f.cLoadColours "Load in a FreeSurfer colour definition file when loading a COR label map.\nWARNING: will override other colours, use at your own risk."
    pack $f.cLoadColours -side top -padx $Gui(pad)
    DevAddFileBrowse $f vtkFreeSurferReaders "colourFileName" "Colour xml file:" "vtkFreeSurferReadersSetColourFileName" "xml" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer colors file"

    #------------
    # Volume->Apply 
    #------------
    set f $fVolumes.fVolume.fApply
        
    DevAddButton $f.bApply "Apply" "vtkFreeSurferReadersApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    #-------------------------------------------
    # Demo Button
    #-------------------------------------------
    set f $fVolumes.fDemo
    eval button $f.bDemo -text Demo -command "vtkFreeSurferDemo" $Gui(WBA)
    pack $f.bDemo  -side left -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # Model frame
    #-------------------------------------------
    set fModel $Module(vtkFreeSurferReaders,fModels)
    set f $fModel

    DevAddFileBrowse $f  vtkFreeSurferReaders "ModelFileName" "Model File:" "vtkFreeSurferReadersSetModelFileName" "" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer surface file (orig ${vtkFreeSurferReaders(surfaces)})"
    frame $f.fName -bg $Gui(activeWorkspace)
    frame $f.fSurface -bg $Gui(activeWorkspace)
    frame $f.fScalar -bg $Gui(activeWorkspace)
    frame $f.fAnnotation -bg $Gui(activeWorkspace)
    frame $f.fAnnotColor -bg $Gui(activeWorkspace)
    frame $f.fApply  -bg $Gui(activeWorkspace)
 
    pack $f.fName -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fSurface -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fScalar -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fAnnotation -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fAnnotColor -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    DevAddFileBrowse $f.fAnnotColor vtkFreeSurferReaders "colorTableFilename" "Annotation Color file:" "vtkFreeSurferReadersSetAnnotColorFileName" "txt" "\$Volume(DefaultDir)" "Open" "Browse for a FreeSurfer annotation colour table file (txt)"

    pack $f.fApply -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1

    #------------
    # Model->Name
    #------------
    set f $Module(vtkFreeSurferReaders,fModels).fName
    eval {label $f.lName -text "Name:"} $Gui(WLA)
    eval {entry $f.eName -textvariable vtkFreeSurferReaders(ModelName) -width 8} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x
    pack $f.lName -side left -padx $Gui(pad) 


    if {0} {
        # need to work this out still, for now you can load these instead of origs
    #------------
    # Model->Surface 
    #------------
    # surface files (mesh): e.g., lh.inflated, lh.pial, lh.smoothwm, lh.sphere
    set f $Module(vtkFreeSurferReaders,fModels).fSurface
    DevAddLabel $f.lTitle "Load Associated Surface Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach surface $vtkFreeSurferReaders(surfaces) {
        eval {checkbutton $f.c$surface \
                  -text $surface -command "vtkFreeSurferReadersSetLoad $surface " \
                  -variable vtkFreeSurferReaders(assocFiles,$surface) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$surface -side top -padx 0
    }
}
    #------------
    # Model->Scalar 
    #------------
    # curvature (scalar): e.g., lh.thickness, lh.curv, lh.sulc, lh.area
    set f $Module(vtkFreeSurferReaders,fModels).fScalar
    DevAddLabel $f.lTitle "Load Associated Scalar files:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach scalar $vtkFreeSurferReaders(scalars) {
        eval {checkbutton $f.r$scalar \
                  -text "$scalar" -command "vtkFreeSurferReadersSetLoad $scalar" \
                  -variable vtkFreeSurferReaders(assocFiles,$scalar) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.r$scalar -side top -padx 0
    }

    #------------
    # Model->Annotation 
    #------------
    # annotation files: lh.xxx.annot
    set f $Module(vtkFreeSurferReaders,fModels).fAnnotation
    DevAddLabel $f.lTitle "Load Associated Annotation files:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach annot $vtkFreeSurferReaders(annots) {
        eval {checkbutton $f.c$annot \
                  -text "$annot" -command "vtkFreeSurferReadersSetLoad $annot" \
                  -variable vtkFreeSurferReaders(assocFiles,$annot) -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$annot -side top -padx 0
    }

    #------------
    # Model->Apply 
    #------------
    set f $Module(vtkFreeSurferReaders,fModels).fApply
        
    DevAddButton $f.bApply "Apply" "vtkFreeSurferReadersModelApply; RenderAll" 8
    DevAddButton $f.bCancel "Cancel" "vtkFreeSurferReadersModelCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)


}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferDemo
# A hack for testing/development.  Assumes you have the correct data available 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferDemo {} {
    global env

    source $env(SLICER_HOME)/Modules/vtkFreeSurferReaders/tcl/regions.tcl
    source $env(SLICER_HOME)/Modules/vtkFreeSurferReaders/tcl/ccdb.tcl
    set r [regions #auto]
           
    $r demo
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReaderBuildVTK
# Does nothing right now, vtk models are built when model files are read in.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersBuildVTK {} {
    global Module
    if {$Module(verbose) == 1} {puts "proc vtkFreeSurferReaders Build VTK"}
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersEnter
# Does nothing yet
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersEnter {} {
    global Module vtkFreeSurferReaders
    if {$Module(verbose) == 1} {puts "proc vtkFreeSurferReaders ENTER"}
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersExit
# Does nothing yet.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersExit {} {
    global vtkFreeSurferReaders Module
    if {$Module(verbose) == 1} {puts "proc vtkFreeSurferReaders EXIT"}
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetVolumeFileName
# The filename is set elsehwere, in variable vtkFreeSurferReaders(VolumeFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetVolumeFileName {} {
    global Volume Module vtkFreeSurferReaders

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders filename: $vtkFreeSurferReaders(VolumeFileName)"
    }
    if {[string equal [file extension $vtkFreeSurferReaders(VolumeFileName)]  ".mgh"]} { 
        set Volume(name) [file rootname [file tail $vtkFreeSurferReaders(VolumeFileName)]]
    } elseif {[string equal [file extension $vtkFreeSurferReaders(VolumeFileName)]  ".bfloat"]} {
        # use the stem of the file, the part between the last directory separator and the underscore
        set bdir [file dirname $vtkFreeSurferReaders(VolumeFileName)]
        regexp "$bdir/(.*)_.*" $vtkFreeSurferReaders(VolumeFileName) match Volume(name)
#        set Volume(name) [file rootname [file tail $vtkFreeSurferReaders(VolumeFileName)]]
    } else {
        # COR: make the volume name be the name of the directory rather than COR-.info
        set Volume(name) [file tail [file dirname $vtkFreeSurferReaders(VolumeFileName)]]
        # replace . with -
        regsub -all {[.]} $Volume(name) {-} Volume(name)
    }
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetModelFileName
# The filename is set elsehwere, in variable vtkFreeSurferReaders(ModelFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetModelFileName {} {
    global vtkFreeSurferReaders Volume Model Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders filename: $vtkFreeSurferReaders(ModelFileName)"
    }
    set Model(name) [file tail $vtkFreeSurferReaders(ModelFileName)]
    # replace . with -
    regsub -all {[.]} $Model(name) {-} Model(name)
    set vtkFreeSurferReaders(ModelName) $Model(name)
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetAnnotColorFileName
# The filename is set elsehwere, in variable vtkFreeSurferReaders(colorTableFilename)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetAnnotColorFileName {} {
    global vtkFreeSurferReaders Volume
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersSetAnnotColorFileName: annotation colour file name set to $vtkFreeSurferReaders(colorTableFilename)"
    }
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersApply
# Read in the freesurfer volume specified by calling the appropriate reader function.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersApply {} {
    global vtkFreeSurferReaders Module Volume

    # switch on the file name, it can be:
    # a COR file (*.info that defines the volume) 
    # an mgh file (*.mgh, volume in one file)
    # a bfloat file (*.bfloat)
    if {[string match *.info $vtkFreeSurferReaders(VolumeFileName)]} {
        vtkFreeSurferReadersCORApply
    } elseif {[string match *.mgh $vtkFreeSurferReaders(VolumeFileName)]} {
        vtkFreeSurferReadersMGHApply
    } elseif {[string match *.bfloat $vtkFreeSurferReaders(VolumeFileName)]} {
        vtkFreeSurferReadersBfloatApply
    } else {
        DevErrorWindow "ERROR: Invalid file extension, file $vtkFreeSurferReaders(VolumeFileName) does not match info or mgh extensions for COR or MGH files."
    }

    # allow use of other module GUIs
    set Volumes(freeze) 0

    # If tabs are frozen, then return to the "freezer"
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}
#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersCORApply
# Read in the freesurfer COR volume specified
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersCORApply {} {
    global Volume vtkFreeSurferReaders Module View Color

    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]
    # NOTE:
    # this normally happens in MainVolumes.tcl
    # this is needed here to set up reading
    # this should be fixed - the node should handle this somehow
    # so that MRML can be read in with just a node and this will
    # work
    # puts "vtkFreeSurferReadersApply: NOT calling MainVolumes create on $i"
    # MainVolumesCreate $i
  
    # read in the COR file
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }

    vtkCORReader Volume($i,vol,rw)
    #  read the header first: sets the Volume array values we need
    vtkFreeSurferReadersCORHeaderRead $vtkFreeSurferReaders(VolumeFileName)
    
    # get the directory name from the filename
    Volume($i,vol,rw) SetFilePrefix [file dirname $vtkFreeSurferReaders(VolumeFileName)]
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReadersApply: set prefix to [Volume($i,vol,rw) GetFilePrefix]\nCalling Update on volume $i"
    }
    Volume($i,vol,rw) Update

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)
    
   
    # set the volume properties: read the header first: sets the Volume array values we need
    # vtkFreeSurferReadersCORHeaderRead $vtkFreeSurferReaders(VolumeFileName)
    Volume($i,node) SetName $Volume(name)
    Volume($i,node) SetDescription $Volume(desc)
    Volume($i,node) SetLabelMap $Volume(labelMap)
    eval Volume($i,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($i,node) SetTilt $Volume(gantryDetectorTilt)

    Volume($i,node) SetFilePattern $Volume(filePattern) 
    Volume($i,node) SetScanOrder $Volume(scanOrder)
    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    # this is the file prefix that will be used to build the image file names, needs to go up to COR
    Volume($i,node) SetFilePrefix [string trimright [file rootname $vtkFreeSurferReaders(VolumeFileName)] "-"]
# [Volume($i,vol,rw) GetFilePrefix]
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarTypeToUnsignedChar
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)

    # so can read in the volume
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReaders: setting full prefix for volume node $i"
    }
    Volume($i,node) SetFullPrefix [string trimright [file rootname $vtkFreeSurferReaders(VolumeFileName)] "-"]

    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReaders: set up volume node for $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "vtkFreeSurferReaders: volume $i position 1 3: $badval"
    
        puts "vtkFreeSurferReaders: calling MainUpdateMRML"
    }
    # Reads in the volume via the Execute procedure
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        DevErrorWindow "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        return
    }
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReaders: after mainupdatemrml volume node  $i:"
        Volume($i,node) Print
        set badval [[Volume($i,node) GetPosition] GetElement 1 3]
        puts "vtkFreeSurferReaders: volume $i position 1 3: $badval"
    }
 
    # set active volume on all menus
    MainVolumesSetActive $i

    # save the id for later use
    set m $i

    # if we are successful set the FOV for correct display of this volume
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]
    set fov     [expr $dim*$spacing]
    set View(fov) $fov
    MainViewSetFov

    set iCast -1
    if {$vtkFreeSurferReaders(castToShort)} {
        if {$::Module(verbose)} {
            puts "vtkFreeSurferReadersCORApply: Casting volume to short."
        }
        set iCast [vtkFreeSurferReadersCast $i Short]
        if {$iCast != -1} {
            DevInfoWindow "Cast input volume to Short, use ${Volume(name)}-Short for editing."
        }
    } 

    # load in free surfer colours for a label map?
    if {[Volume($i,node) GetLabelMap] == 1 &&
        $vtkFreeSurferReaders(loadColours)} {
        if {$::Module(verbose)} {
            DevInfoWindow "vtkFreeSurferReadersCORApply: loading colour file $vtkFreeSurferReaders(colourFileName)."
        }
        # piggy back on the Color module
        set Color(fileName) $vtkFreeSurferReaders(colourFileName)
        ColorsLoadApply
    }


    # display the new volume in the background of all slices if not a label map
    if {$iCast == -1} {
        if {[Volume($i,node) GetLabelMap] == 1} {
            MainSlicesSetVolumeAll Label $i
        } else {
            MainSlicesSetVolumeAll Back $i
        }
    } else {
        if {[Volume($iCast,node) GetLabelMap] == 1} {
            MainSlicesSetVolumeAll Label $iCast
        } else {
            MainSlicesSetVolumeAll Back $iCast
        }
    }

    # Update all fields that the user changed (not stuff that would need a file reread)
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersMGHApply
# Read in the freesurfer MGH volume specified
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersMGHApply {} {
    global vtkFreeSurferReaders Module Volume View

    if {![info exists Volume(name)] } { set Volume(name) "MGH"}

    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersMGHApply:\n\tLoading MGH file $vtkFreeSurferReaders(VolumeFileName)"
    }

    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]

    if {$::Module(verbose)} {
        # turn debugging on in this node - causes a seg fault
        Volume($i,node) DebugOn
    }

    # read in the MGH file
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }
    vtkMGHReader Volume($i,vol,rw)
    
    Volume($i,vol,rw) SetFilePrefix [file dirname $vtkFreeSurferReaders(VolumeFileName)]
    # set the filename
    Volume($i,vol,rw) SetFileName $vtkFreeSurferReaders(VolumeFileName)

    # have to fudge it a little here, read the header to get the info needed
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersMGHApply:\n\tReading volume header"
    }

    Volume($i,vol,rw) ReadVolumeHeader
    
    set updateReturn [Volume($i,vol,rw) Update]
    if {$updateReturn == 0} {
        DevErrorWindow "vtkMGHReader: update on volume $i failed."
    }

   
    set Volume(isDICOM) 0
    set Volume($i,type) "MGH"

    set dims [Volume($i,vol,rw) GetDataDimensions]
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) dimensions: [lindex $dims 0] [lindex $dims 1]  [lindex $dims 2]"
        # DevErrorWindow "$vtkFreeSurferReaders(VolumeFileName) dimensions: [lindex $dims 0] [lindex $dims 1]  [lindex $dims 2]"
    }
    set Volume(lastNum) [expr [lindex $dims 2] - 1]
    set Volume(width) [expr [lindex $dims 0] - 1]
    set Volume(height) [expr [lindex $dims 1] - 1]

    set spc [Volume($i,vol,rw) GetDataSpacing]
    set Volume(pixelWidth) [lindex $spc 0]
    set Volume(pixelHeight) [lindex $spc 1]
    set Volume(sliceThickness) [lindex $spc 2]
    set Volume(sliceSpacing) [lindex $spc 2]

    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) numScalars = $Volume(numScalars)"
    }
    set Volume(littleEndian) 0

    # Sag:LR RL Ax:SI IS Cor: AP PA
    set Volume(scanOrder) {RL}
    set scalarType [Volume($i,vol,rw) GetScalarType]
    # Scalar type can be VTK_UNSIGNED_CHAR (3),  VTK_INT (6), VTK_FLOAT (10), VTK_SHORT (4), 
    # set it to the valid volume values of $Volume(scalarTypeMenu)
    switch $scalarType {
        "3" { set Volume(scalarType) UnsignedChar}
        "4" { set  Volume(scalarType) Short}
        "6" { set  Volume(scalarType) Int}
        "10" { set  Volume(scalarType) Float }
        default {
            puts "Unknown scalarType $scalarType, using Float"
            DevErrorWindow "vtkFreeSurferReadersMGHApply: Unknown scalarType $scalarType, using Float"
            set Volume(scalarType) Float 
        }
    }
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) scalarType $Volume(scalarType)"
    }
    set Volume(readHeaders) 0
    set Volume(filePattern) %s
    set Volume(dimensions) "[lindex $dims 0] [lindex $dims 1]"

    set Volume(imageRange) "1 [lindex $dims 2]"
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) imageRange $Volume(imageRange)"
    }
    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)

    # set the volume properties
    Volume($i,node) SetName $Volume(name)
    Volume($i,node) SetDescription $Volume(desc)
    Volume($i,node) SetLabelMap $Volume(labelMap)
    eval Volume($i,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($i,node) SetTilt $Volume(gantryDetectorTilt)

    Volume($i,node) SetFilePattern $Volume(filePattern) 
    Volume($i,node) SetScanOrder $Volume(scanOrder)
    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    Volume($i,node) SetFilePrefix [Volume($i,vol,rw) GetFileName] ;# NB: just one file, not a pattern
    Volume($i,node) SetFullPrefix [Volume($i,vol,rw) GetFileName] ;# needed in the range check
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarType $scalarType
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    # without these, getting a seg fault when debug is turned on in the vtkMrmlVolumeNode
    Volume($i,node) SetLUTName ""
    Volume($i,node) SetFileType "MGH"

    # read in the matrix from the headers and use it
     
    # get the IJK to RAS matrix from the volume:
    # x_r x_a x_s y_r y_a y_s z_r z_a z_s c_r c_a c_s
    set ijkmat [Volume($i,vol,rw) GetRASMatrix]
    if {$::Module(verbose)} {
        puts "MGH Reader: IJK matrix for volume $i: $ijkmat"
    }
    # fill in a matrix
    if {[info command rasmat$i] != ""} {
        rastmat$i Delete
    }

    # there's a problem with getting the MGH volume to display properly,
    # it comes up okay in slices mode but not when the RasToIjk matrix is set
    vtkMatrix4x4 rasmat$i

    rasmat$i SetElement 0 0 [lindex $ijkmat 0]
    rasmat$i SetElement 0 1 [lindex $ijkmat 1]
    rasmat$i SetElement 0 2 [lindex $ijkmat 2]
    rasmat$i SetElement 0 3 [lindex $ijkmat 9]

    rasmat$i SetElement 1 0 [lindex $ijkmat 3]
    rasmat$i SetElement 1 1 [lindex $ijkmat 4]
    rasmat$i SetElement 1 2 [lindex $ijkmat 5]
    rasmat$i SetElement 1 3 [lindex $ijkmat 10]

    rasmat$i SetElement 2 0 [lindex $ijkmat 6]
    rasmat$i SetElement 2 1 [lindex $ijkmat 7]
    rasmat$i SetElement 2 2 [lindex $ijkmat 8]
    rasmat$i SetElement 2 3 [lindex $ijkmat 11]

    rasmat$i SetElement 3 0 0
    rasmat$i SetElement 3 1 0
    rasmat$i SetElement 3 2 0
    rasmat$i SetElement 3 3 1

    # rasmat$i Identity

    # invert it to get the RAS to IJK
    rasmat$i Invert

    # the Slicer assumption shifts the MGH volume by 128 into the R
    # the MGH volume center is ... info to be updated when tosa@nmr.mgh.harvard.edu responds
    rasmat$i SetElement 0 3 [expr [rasmat$i GetElement 0 3] - 128]
    
    # turn off using the ras to vtk matrix, as otherwise the MGH volume is flipped in Y
    if {$::Module(verbose)} {
        puts "Turning off UseRasToVtkMatrix on volume $i"
    }
    Volume($i,node) UseRasToVtkMatrixOff

    # set the volume's RAS to IJK matrix
#    Volume($i,node) SetRasToIjkMatrix rasmat$i
    set curRasToIjk [Volume($i,node) GetRasToIjk]
    $curRasToIjk DeepCopy rasmat$i


    # set up a secondary window to view the slices
    if {$::Module(verbose)} {
#        vtkFreeSurferReadersShowMGH $i
    }

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReaders: About  to call main update mrml for an MGH volume, \# $i"
        DevErrorWindow "vtkFreeSurferReaders: About  to call main update mrml for an MGH volume"
    }

    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # try doing some manual reading here - necessary to show the data legibly
    Volume($i,vol) SetImageData [Volume($i,vol,rw) GetOutput]

    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        DevErrorWindow "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        return
    }
     
    # allow use of other module GUIs
    set Volume(freeze) 0
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }

    # set active volume on all menus
    MainVolumesSetActive $i

    # save the id for later use
    set m $i

    # if we are successful set the FOV for correct display of this volume
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]
    set fov     [expr $dim*$spacing]
    if {$::Module(verbose)} { 
        puts "MGH Reader setting fov to $fov - should be 256 (dim $dim spacing $spacing)"
    }
    set View(fov) $fov
    MainViewSetFov

    # display the new volume in the background of all slices if not a label map
    if {[Volume($i,node) GetLabelMap] == 1} {
        MainSlicesSetVolumeAll Label $i
    } else {
        MainSlicesSetVolumeAll Back $i
    }

    # Update all fields that the user changed (not stuff that would need a file reread)
    return $i
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersMGHUpdateMRML
# Takes the necessary updates from MainUpdateMrml
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersUpdateMRML {} {
    global Module vtkFreeSurferReaders

    if {$Module(verbose)} { 
        puts "vtkFreeSurferReadersMGHUpdateMRML"
    }

}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersShowMGH
# A debugging tool, opens a new window with slices shown of the MGH volume i
# .ARGS
# i the volume id
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersShowMGH {i} {
    global Volume Module

    if {[info command Volume($i,vol,rw)] == ""} {
        puts "vtkFreeSurferReadersShowMGH: no reader exists for volume $i"
        return
    }
    scan [[Volume($i,vol,rw) GetOutput] GetWholeExtent] "%d %d %d %d %d %d" \
        xMin xMax yMin yMax zMin zMax
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersShowMGH: vol $i: whole extent $xMin $xMax $yMin $yMax $zMin $zMax"
    }
    if {[info command viewer$i] == ""} {
        vtkImageViewer viewer$i
    }
    viewer$i SetInput [Volume($i,vol,rw) GetOutput]
    viewer$i SetZSlice [expr $zMax / 2]
    viewer$i SetColorWindow 2000
    viewer$i SetColorLevel 1000
    toplevel .top$i
    # wm protocol .top$i WM_DELETE_WINDOW {puts "exit, i=$i"; wm withdraw .top$i; destroy .top$i; viewer$i Delete}
    frame .top$i.f1 
    set vtkiw vtkiw$i
    set $vtkiw [vtkTkImageViewerWidget .top$i.f1.r1 \
                   -width [expr ($xMax - $xMin + 1)] \
                   -height [expr ($yMax - $yMin + 1)] \
                   -iv viewer$i]
    ::vtk::bind_tk_imageviewer_widget [subst "$$vtkiw"]
    set slice_number$i [viewer$i GetZSlice]
    
    
    scale .top$i.slice \
        -from $zMin \
        -to $zMax \
        -orient horizontal \
        -command "SetSlice $i"\
        -variable slice_number$i \
        -label "Z Slice"
    
    proc SetSlice {i slice} {
        if {$::Module(verbose)} {
            puts "SetSlice slice = $slice, i = $i"
        }
        viewer$i SetZSlice $slice
        viewer$i Render
    }
    pack [subst "$$vtkiw"] \
        -side left -anchor n \
        -padx 3 -pady 3 \
        -fill x -expand f
    pack .top$i.f1 \
        -fill both -expand t
    pack .top$i.slice \
        -fill x -expand f
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersVolumesPropsApplyPre
# Encapsulates the stuff that volume props apply does before the custom stuff here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersVolumesPropsApplyPre {} {
    global vtkFreeSurferReaders Lut Volume Label Module Mrml View

    set Volume(isDICOM) 0

    # Validate name
    if {$Volume(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this volume."
        return -1
    }
    if {[ValidateName $Volume(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return -1
    }

    
    return 1
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersBfloatApply
# Read in the freesurfer Bfloat volume specified
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersBfloatApply {} {
    global vtkFreeSurferReaders Module Volume View

    if {![info exists Volume(name)] } { set Volume(name) "Bfloat"}

    # encapsulate the standard stuff
    set retval [vtkFreeSurferReadersVolumesPropsApplyPre]
    if {$retval == -1} { 
        puts "Error from vtkFreeSurferReadersVolumesPropsApplyPre, exiting vtkFreeSurferReadersBfloatApply"
        return
    }

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersBfloatApply:\n\tLoading Bfloat file $vtkFreeSurferReaders(VolumeFileName)"
    }


    # add a mrml node
    set n [MainMrmlAddNode Volume]
    set i [$n GetID]

    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }
    vtkBVolumeReader Volume($i,vol,rw)
    if {$::Module(verbose)} {
        puts "Setting bvolume reader debug to on for volume $i"
        Volume($i,vol,rw) DebugOn
    }

    # set the filename  stem
    if {[regexp {(.*)_(.*)} $vtkFreeSurferReaders(VolumeFileName) match stem ext] == 0} {
        DevErrorWindow "vtkFreeSurferReadersBfloatApply: error parsing file name $vtkFreeSurferReaders(VolumeFileName)"
        set stem $vtkFreeSurferReaders(VolumeFileName)
    } else {
        puts "Set stem to $stem"
    }

    Volume($i,vol,rw) SetFileName $vtkFreeSurferReaders(VolumeFileName)
    Volume($i,vol,rw) SetFilePrefix $stem

    Volume($i,vol,rw) Update
    set newstem [Volume($i,vol,rw) GetStem]
    DevInfoWindow "After reading, new stem = $newstem (orig stem = $stem)"

    # try setting the registration filename
    Volume($i,vol,rw) SetRegistrationFileName ${stem}.dat
    set regmat [Volume($i,vol,rw) GetRegistrationMatrix]
    if {$::Module(verbose)} {
        puts "Got registration matrix $regmat"
        if {$regmat != ""} {
            puts "[$regmat GetElement 0 0] [$regmat GetElement 1 0] [$regmat GetElement 2 0] [$regmat GetElement 3 0]"
            puts "[$regmat GetElement 0 1] [$regmat GetElement 1 1] [$regmat GetElement 2 1] [$regmat GetElement 3 1]"
            puts "[$regmat GetElement 0 2] [$regmat GetElement 1 2] [$regmat GetElement 2 2] [$regmat GetElement 3 2]"
            puts "[$regmat GetElement 0 3] [$regmat GetElement 1 3] [$regmat GetElement 2 3] [$regmat GetElement 3 3]"
        } else {
            puts "\tregmat is empty"
        }
    }

    set Volume(isDICOM) 0
    set Volume($i,type) "bfloat"

    set dims [Volume($i,vol,rw) GetDataDimensions]
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) dimensions: [lindex $dims 0] [lindex $dims 1]  [lindex $dims 2]"
        # DevErrorWindow "$vtkFreeSurferReaders(VolumeFileName) dimensions: [lindex $dims 0] [lindex $dims 1]  [lindex $dims 2]"
    }
    set Volume(lastNum) [expr [lindex $dims 2] - 1]
    set Volume(width) [expr [lindex $dims 0] - 1]
    set Volume(height) [expr [lindex $dims 1] - 1]

    set spc [Volume($i,vol,rw) GetDataSpacing]
    set Volume(pixelWidth) [lindex $spc 0]
    set Volume(pixelHeight) [lindex $spc 1]
    set Volume(sliceThickness) [lindex $spc 2]
    set Volume(sliceSpacing) [lindex $spc 2]

    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) numScalars = $Volume(numScalars)"
    }
    set Volume(littleEndian) 0

    # Sag:LR RL Ax:SI IS Cor: AP PA
    set Volume(scanOrder) {RL}
    set scalarType [Volume($i,vol,rw) GetScalarType]
    # Scalar type can be VTK_UNSIGNED_CHAR (3),  VTK_INT (6), VTK_FLOAT (10), VTK_SHORT (4), 
    # set it to the valid volume values of $Volume(scalarTypeMenu)
    switch $scalarType {
        "3" { set Volume(scalarType) UnsignedChar}
        "4" { set  Volume(scalarType) Short}
        "6" { set  Volume(scalarType) Int}
        "10" { set  Volume(scalarType) Float }
        default {
            puts "Unknown scalarType $scalarType, using Float"
            DevErrorWindow "vtkFreeSurferReadersBfloatApply: Unknown scalarType $scalarType, using Float"
            set Volume(scalarType) Float 
        }
    }

    set Volume(readHeaders) 0

    set Volume(filePattern) {%s_%03d.bfloat}
    set Volume(dimensions) "[lindex $dims 0] [lindex $dims 1]"

    set Volume(imageRange) "0 [expr [lindex $dims 2] - 1]"
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) imageRange $Volume(imageRange)"
    }
    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)

    # set the volume properties
    Volume($i,node) SetName $Volume(name)
    Volume($i,node) SetDescription $Volume(desc)
    Volume($i,node) SetLabelMap $Volume(labelMap)
    eval Volume($i,node) SetSpacing $Volume(pixelWidth) $Volume(pixelHeight) \
            [expr $Volume(sliceSpacing) + $Volume(sliceThickness)]
    Volume($i,node) SetTilt $Volume(gantryDetectorTilt)

    Volume($i,node) SetFilePattern $Volume(filePattern) 
    Volume($i,node) SetScanOrder $Volume(scanOrder)
    Volume($i,node) SetNumScalars $Volume(numScalars)
    Volume($i,node) SetLittleEndian $Volume(littleEndian)
    Volume($i,node) SetFilePrefix [Volume($i,vol,rw) GetStem] 
    Volume($i,node) SetFullPrefix [Volume($i,vol,rw) GetStem] ;# needed in the range check
    Volume($i,node) SetImageRange [lindex $Volume(imageRange) 0] [lindex $Volume(imageRange) 1]
    Volume($i,node) SetScalarType $scalarType
    Volume($i,node) SetDimensions [lindex $Volume(dimensions) 0] [lindex $Volume(dimensions) 1]
    # without these, getting a seg fault when debug is turned on in the vtkMrmlVolumeNode
    Volume($i,node) SetLUTName ""
    Volume($i,node) SetFileType "bfloat"
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReaders: About  to call main update mrml for a Bfloat volume, \# $i"
        DevErrorWindow "vtkFreeSurferReaders: About  to call main update mrml for an Bfloat volume"
        puts "\tFile prefix = [Volume($i,node) GetFilePrefix]"
        puts "\tFull prefix = [Volume($i,node) GetFullPrefix]"
        puts "\tFile pattern = [Volume($i,node) GetFilePattern]"
    }

    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # If failed, then it's no longer in the idList
    if {[lsearch $Volume(idList) $i] == -1} {
        puts "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        DevErrorWindow "vtkFreeSurferReaders: failed to read in the volume $i, $vtkFreeSurferReaders(VolumeFileName)"
        return
    }

    # try doing some manual reading here - necessary to show the data legibly
    if {1} {
        if {$::Module(verbose)} {
            puts "BfloatReaderApply: calling SetImageData"
        }
        Volume($i,vol) SetImageData [Volume($i,vol,rw) GetOutput]
    }

    # mark the volume as saved
    set Volume($i,dirty) 0

    # allow use of other module GUIs
    set Volume(freeze) 0
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }

    # set active volume on all menus
    MainVolumesSetActive $i

    # save the id for later use
    set m $i

    # if we are successful set the FOV for correct display of this volume
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]
    set fov     [expr $dim*$spacing]
    if {$::Module(verbose)} { 
        puts "Bfloat Reader setting fov to $fov (dim $dim spacing $spacing)"
    }
    set View(fov) $fov
    MainViewSetFov

    # display the new volume in the background of all slices if not a label map
    if {[Volume($i,node) GetLabelMap] == 1} {
        MainSlicesSetVolumeAll Label $i
    } else {
        MainSlicesSetVolumeAll Back $i
    }

    # set up a secondary window to view the slices
    if {$::Module(verbose)} {
        vtkFreeSurferReadersShowMGH $i
    }
    # Update all fields that the user changed (not stuff that would need a file reread)
    return $i
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersBuildSurface
# Builds a model, a surface from a Freesurfer file.
# .ARGS
# m the model id
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersBuildSurface {m} {
    global vtkFreeSurferReaders viewRen Volume Module Model RemovedModels 

    if {$Module(verbose) == 1} { 
        puts "\nvtkFreeSurferReadersBuildSurface\n"
    }
    # set up the reader
    vtkFSSurfaceReader Model($m,reader)
    Model($m,reader) SetFileName $vtkFreeSurferReaders(ModelFileName)

    vtkPolyDataNormals Model($m,normals)
    Model($m,normals) SetSplitting 0
    Model($m,normals) SetInput [Model($m,reader) GetOutput]

    vtkStripper Model($m,stripper)
    Model($m,stripper) SetInput [Model($m,normals) GetOutput]

    # should be vtk model
    foreach r $Module(Renderers) {
        # Mapper
        if {$::Module(verbose)} { puts "Deleting Model($m,mapper,$r)" }
        catch "Model($m,mapper,$r) Delete"
        vtkPolyDataMapper Model($m,mapper,$r)
#Model($m,mapper,$r) SetInput [Model($m,reader) GetOutput]
    }

    # Delete the src, leaving the data in Model($m,polyData)
    # polyData will survive as long as it's the input to the mapper
    #
    Model($m,node) SetName $vtkFreeSurferReaders(ModelName)
    Model($m,node) SetFileName $vtkFreeSurferReaders(ModelFileName)
    set Model($m,polyData) [Model($m,stripper) GetOutput]
    $Model($m,polyData) Update

    #-------------------------
    # read in the other surface files somehow, as additional vectors doesn't work

    if {0} {

    set numSurfaces 0
    foreach s $vtkFreeSurferReaders(surfaces) {
        if {[lsearch $vtkFreeSurferReaders(assocFiles) $s] != -1} {
            incr numSurfaces
        }
    }
    if {$::Module(verbose)} {
        DevInfoWindow "Have $numSurfaces vector arrays associated with this model, call:\nvtkFreeSurferReadersSetModelVector $m vectorName"
    }
    set numSurfacesAdded 0
    foreach s $vtkFreeSurferReaders(surfaces) {
        if {[lsearch $vtkFreeSurferReaders(assocFiles) $s] != -1} {
            set surfaceFileName [file rootname $vtkFreeSurferReaders(ModelFileName)].$s
            if [file exists $surfaceFileName] {
                DevInfoWindow "Model $m: Reading in $s file associated with this surface: $surfaceFileName"
                # need to delete these so that if close the scene and reopen a surface file, these won't still exist
                if {$::Module(verbose)} {
                    puts "Deleting Model($m,floatArray$s)..."
                }
                catch "Model($m,floatArray$s) Delete"
                vtkFloatArray Model($m,floatArray$s)
                Model($m,floatArray$s) SetName $s
                vtkFSSurfaceReader Model($m,sr$s)
                
                Model($m,sr$s) SetFileName $surfaceFileName
                Model($m,sr$s) SetOutput Model($m,floatArray$s)
                Model($m,sr$s) Update
                # may need to call Model($m,sr$s) Execute
                
                if {$::Module(verbose)} {
                    DevInfoWindow "Adding surface named $s to model id $m"
                }
                [$Model($m,polyData) GetPointData] AddVector Model($m,floatArray$s)
                # may have some missing files
                incr numSurfacesAdded 
                if {$numSurfacesAdded == 1} {
                    # set the first one active
                    [$Model($m,polyData) GetPointData] SetActiveVectors $s
                }   
                # may need to set reader output to "" and delete
            } else {
                 DevWarningWindow "Surface file does not exist: $surfaceFileName"
            }
        }
    }
} 
    #-------------------------
    # read in the scalar files
    # check if there's more than one
    set numScalars 0
    foreach s $vtkFreeSurferReaders(scalars) {
        if {[lsearch $vtkFreeSurferReaders(assocFiles) $s] != -1} {
            incr numScalars
        }
    }
    if {$::Module(verbose)} {
        DevInfoWindow "Have $numScalars scalar arrays associated with this model, call:\nvtkFreeSurferReadersSetModelScalar $m scalarName"
    }
    set numScalarsAdded 0
    foreach s $vtkFreeSurferReaders(scalars) {
        if {[lsearch $vtkFreeSurferReaders(assocFiles) $s] != -1} {
            set scalarFileName [file rootname $vtkFreeSurferReaders(ModelFileName)].$s
            if [file exists $scalarFileName] {
                puts "Model $m: Reading in $s file associated with this surface: $scalarFileName"
                # need to delete these so that if close the scene and reopen a surface file, these won't still exist
                if {$::Module(verbose)} {
                    puts "Deleting Model($m,floatArray$s)..."
                }
                catch "Model($m,floatArray$s) Delete"
                vtkFloatArray Model($m,floatArray$s)
                Model($m,floatArray$s) SetName $s
                catch "Model($m,ssr$s) Delete"
                vtkFSSurfaceScalarReader Model($m,ssr$s)

                Model($m,ssr$s) SetFileName $scalarFileName
                # this doesn't work on solaris, can't cast float array to vtkdataobject
                Model($m,ssr$s) SetOutput Model($m,floatArray$s)
                
                Model($m,ssr$s) ReadFSScalars

                # if there's going to be more than one, use add array, otherwise just set it
                if {$numScalars == 1} {
                    [$Model($m,polyData) GetPointData] SetScalars Model($m,floatArray$s)
                } else {
                    if {$::Module(verbose)} {
                        DevInfoWindow "Adding scalar named $s to model id $m"
                    }
                    [$Model($m,polyData) GetPointData] AddArray Model($m,floatArray$s)
                    # may have some missing files
                    incr numScalarsAdded 
                    if {$numScalarsAdded == 1} {
                        # set the first one active
                        [$Model($m,polyData) GetPointData] SetActiveScalars $s
                    }
                }
                # may need to set reader output to "" and delete here
            } else {
                DevWarningWindow "Scalar file does not exist: $scalarFileName"
            }
        }
    }
    

    
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
   
        if {0} {
            # decimate
            vtkDecimate Model($m,decimate,$r) 
            Model($m,decimate,$r) SetInput $Model($m,polyData)
            Model($m,decimate,$r) SetMaximumIterations 6
            Model($m,decimate,$r)  SetMaximumSubIterations 0 
            Model($m,decimate,$r) PreserveEdgesOn
            Model($m,decimate,$r) SetMaximumError 1
            Model($m,decimate,$r) SetTargetReduction 1
            Model($m,decimate,$r) SetInitialError .0002
            Model($m,decimate,$r) SetErrorIncrement .0002
            [ Model($m,decimate,$r) GetOutput] ReleaseDataFlagOn
            vtkSmoothPolyDataFilter smoother
            smoother SetInput [Model($m,decimate,$r) GetOutput]
            set p smoother
            $p SetNumberOfIterations 2
            $p SetRelaxationFactor 0.33
            $p SetFeatureAngle 60
            $p FeatureEdgeSmoothingOff
            $p BoundarySmoothingOff
            $p SetConvergence 0
            [$p GetOutput] ReleaseDataFlagOn

            set Model($m,polyData) [$p GetOutput]
            Model($m,polyData) Update
            foreach r $Module(Renderers) {
                Model($m,mapper,$r) SetInput $Model($m,polyData)
            }
        }
    }
    Model($m,reader) SetOutput ""
    Model($m,reader) Delete
    Model($m,normals) SetOutput ""
    Model($m,normals) Delete
    Model($m,stripper) SetOutput ""
    Model($m,stripper) Delete

    # Clipper
    vtkClipPolyData Model($m,clipper)
    Model($m,clipper) SetClipFunction Slice(clipPlanes)
    Model($m,clipper) SetValue 0.0
    
    vtkMatrix4x4 Model($m,rasToWld)
    
    foreach r $Module(Renderers) {

        # Actor
        vtkActor Model($m,actor,$r)
        Model($m,actor,$r) SetMapper Model($m,mapper,$r)
        # Registration
        Model($m,actor,$r) SetUserMatrix [Model($m,node) GetRasToWld]

        # Property
        set Model($m,prop,$r)  [Model($m,actor,$r) GetProperty]

        # For now, the back face color is the same as the front
        Model($m,actor,$r) SetBackfaceProperty $Model($m,prop,$r)
    }
    set Model($m,clipped) 0
    set Model($m,displayScalarBar) 0

    # init gui vars
    set Model($m,visibility)       [Model($m,node) GetVisibility]
    set Model($m,opacity)          [Model($m,node) GetOpacity]
    set Model($m,scalarVisibility) [Model($m,node) GetScalarVisibility]
    set Model($m,backfaceCulling)  [Model($m,node) GetBackfaceCulling]
    set Model($m,clipping)         [Model($m,node) GetClipping]
    # set expansion to 1 if variable doesn't exist
    if {[info exists Model($m,expansion)] == 0} {
        set Model($m,expansion)    1
    }
    # set RemovedModels to 0 if variable doesn't exist
    if {[info exists RemovedModels($m)] == 0} {
        set RemovedModels($m) 0
    }

    MainModelsSetColor $m
    MainAddModelActor $m
    set Model($m,dirty) 1
    set Model($m,fly) 1

    MainUpdateMRML
    MainModelsSetActive $m
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetSurfaceVisibility
# Set the model's visibility flag.
# .ARGS
# i the model id
# vis the boolean flag determining this model's visibility
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetSurfaceVisibility {i vis} {
    global vtkFreeSurferReaders Volume
    # set the visibility of volume i to visibility vis
    vtkFreeSurferReaders($i,curveactor) SetVisibility $vis
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetModelScalar
# Set the model's active scalars. If an invalid scalar name is passed, will pop up an info box.
# .ARGS
# modelID the model id
# scalarName the name given to the scalar field
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetModelScalar {modelID {scalarName ""}} {
    global Volume vtkFreeSurferReaders
    if {$scalarName == ""} {
        foreach s "$vtkFreeSurferReaders(scalars) $vtkFreeSurferReaders(annots)" {
            if {$vtkFreeSurferReaders(current$s)} {
                set scalarName $s
                puts "Displaying $s"
            }
        }
    }
    if {$::Module(verbose)} {
        puts "set model scalar, model $modelID -> $scalarName"
    }
    if {$scalarName != ""} {
        set retval [[$::Model(${modelID},polyData) GetPointData] SetActiveScalars $scalarName]
        if {$retval == -1} {
            DevInfoWindow "Model $modelID does not have a scalar field named $scalarName.\nPossible options are: $::vtkFreeSurferReaders(scalars)"
        } else {
            # reset the scalar range here
            Render3D
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetModelVector
# Set the model's active vector. If an invalid vector name is passed, will pop up an info box.
# .ARGS
# modelID the model id
# vectorName the name given to the vector field
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetModelVector {modelID {vectorName ""}} {
    if {$vectorName == ""} {
        set vectorName $vtkFreeSurferReaders(currentSurface)
    }
    if {$::Module(verbose)} {
        puts "set model vector, model $modelID -> $vectorName"
    }
    set retval [[$::Model(${modelID},polyData) GetPointData] SetActiveVectors $vectorName]
    if {$retval == -1} {
        DevInfoWindow "Model $modelID does not have a vector field named $vectorName.\nPossible options are: orig $::vtkFreeSurferReaders(surfaces)"
    } else {
        Render3D
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersDisplayPopup
# A convenience pop up window that will allow you to set the active vectors and scalars for a model
# .ARGS
# modelID the id of the model to build the gui for
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersDisplayPopup {modelID} {
    global vtkFreeSurferReaders Volume Model Gui

    DevInfoWindow "Not working 100%... use at your own risk"

    set _w .freeSurferDisplay$modelID
    catch "destroy $_w"
    toplevel $_w
    wm title $_w "FreeSurfer Model $modelID Display Options"
    frame $_w.fSurfaces -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $_w.fScalars -bg $Gui(activeWorkspace) -relief groove -bd 1
    pack $_w.fSurfaces $_w.fScalars -side top -fill x -pady $Gui(pad) -expand 1

    #--------
    # surfaces
    #--------
    if {0} {
    set f $_w.fSurfaces
    DevAddLabel $f.lTitle "Possible Surface Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach surface {orig $vtkFreeSurferReaders(surfaces)} {
        eval {checkbutton $f.c$surface \
                  -text $surface -command "vtkFreeSurferReadersSetModelVector $modelID" \
                  -variable vtkFreeSurferReaders(currentSurface)
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$surface -side top -padx 0
    }
    }
    #---------
    # scalars
    #---------
    set f $_w.fScalars
    DevAddLabel $f.lTitle "Possible Scalar Models:"
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0
    foreach scalar "$vtkFreeSurferReaders(scalars) $vtkFreeSurferReaders(annots)" {
        eval {checkbutton $f.c$scalar \
                  -text $scalar -command "vtkFreeSurferReadersSetModelScalar $modelID" \
                  -variable vtkFreeSurferReaders(current$scalar) \
                  -width 9 \
                  -indicatoron 0} $Gui(WCA)
        pack $f.c$scalar -side top -padx 0
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersMainFileCloseUpdate
# Called to clean up anything created in this sub module. Deletes Volumes read in, along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersMainFileCloseUpdate {} {
    global vtkFreeSurferReaders Volume viewRen Module
    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReadersMainFileCloseUpdate"
    }
    foreach f $Volume(idList) {
        #if {$Module(verbose) == 1} {}
            puts "vtkFreeSurferReadersMainFileCloseUpdate: Checking volume $f"
        
        if {[info exists vtkFreeSurferReaders($f,curveactor)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing surface actor for free surfer reader id $f"
            }
            viewRen RemoveActor  vtkFreeSurferReaders($f,curveactor)
            vtkFreeSurferReaders($f,curveactor) Delete
            vtkFreeSurferReaders($f,mapper) Delete
            Volume($f,vol,rw) Delete
#            MainMrmlDeleteNode Volume $f
        }
        if {[info exists Volume($f,vol,rw)] == 1} {
            if {$Module(verbose) == 1} {
                puts "Removing volume reader for volume id $f"
            }
            Volume($f,vol,rw) Delete
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersAddColors
# Reads in the freesurfer colour file, ColorsFreesurfer.xml, and appends the color tags to the input argument, returning them all. TODO: check for duplicate colour tags and give a warning. 
# .ARGS
# tags the already set tags, append to this variable.
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersAddColors {tags} {
    global vtkFreeSurferReaders env Module
    # read, and append default freesurfer colors.
    # Return a new list of tags, possibly including default colors.
    # TODO: check for conflicts in color names

    set filename $vtkFreeSurferReaders(colourFileName)

    if {$Module(verbose) == 1} {
        puts "Trying to read Freesurfer colours from \"$fileName\""
    }
    set tagsFSColors [MainMrmlReadVersion2.x $fileName]
    if {$tagsFSColors == 0} {
        set msg "Unable to read file default MRML Freesurfer color file '$fileName'"
        puts $msg
        tk_messageBox -message $msg
        return "$tags"
    }

    return "$tags $tagsFSColors"
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersCORHeaderRead
# Reads in the freesurfer coronal volume's header file.
# .ARGS
# file the full path to the COR-.info file
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersCORHeaderRead {filename} {
    global vtkFreeSurferReaders Volume CORInfo

    # Check that a file name string has been passed in
    if {[string equal filename ""]} {
        DevErrorWindow "vtkFreeSurferReadersCORHeaderRead: empty file name"
        return 1
    }
    
    # Open the file for reading
    if {[catch {set fid [open $filename]} errMsg]} {
        puts "Error opening COR header file $filename for reading"
        return 1
    }

    # Read and parse the file into name/value pairs
    set fldPat {([a-zA-Z_]+[0-9]*)}
    set numberPat {([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?}
    set i 0
    set fld ""
    while { ![eof $fid] } {
        # Read a line from the file and analyse it.
        gets $fid line 
        # puts "Line $i: $line"
        set i [expr $i + 1]
        # get the field name
        if { [regexp $fldPat $line fld]} {
            set CORInfo($fld) {}
            # puts "Found field $fld"
        
            # get the values, first test for three floats
            if { [regexp "$numberPat +$numberPat +$numberPat" $line val]} {
                set CORInfo($fld) $val
            } else {
                # one float
                if { [regexp "$numberPat" $line val]} {
                    set CORInfo($fld) $val
                } else {
                    puts "Unable to match line $line"
                }
            }
        } else {
            # only print warning if not an empty line
            if {![regexp {()|( +)} $line]} {
                puts "WARNING: no field name found at the beginning of line: $line"
            }
        }

    }
    close $fid

    # set the global vars
#    set Volume(activeID) 
    set Volume(isDICOM) 0
    if {[info exists Volume(name)] == 0} { set Volume(name) "COR"}
    set Volume(lastNum) $CORInfo(imnr1)
    set Volume(width) $CORInfo(x)
    set Volume(height) $CORInfo(y)
    # distances are in m in the COR info file, we need mm
    set Volume(pixelWidth) [expr 1000.0 * $CORInfo(psiz)]
    set Volume(pixelHeight) [expr 1000.0 * $CORInfo(psiz)]
    set Volume(sliceThickness) [expr 1000.0 * $CORInfo(thick)]
    # use the x dimension to calculate spacing from the field of view
    # set Volume(sliceSpacing) [expr 1000.0 * [expr $CORInfo(fov) /  $Volume(width)]]
    # don't use the slice spacing to anything but 0 for cor files
    set Volume(sliceSpacing) 0
    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    set Volume(littleEndian) 1
    # scan order is Coronal   Posterior Anterior
    set Volume(scanOrder) {PA}
    set Volume(scalarType) {UnsignedChar}
    set Volume(readHeaders) 0
    set Volume(filePattern) {%s-%03d}
    set Volume(dimensions) {256 256}
    set Volume(imageRange) {1 256}
    # puts "vtkFreeSurferReadersCORHeaderRead: set slice spacing to $Volume(sliceSpacing), slice thickness to $Volume(sliceThickness)"
    return 0
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetUMLSMapping
# Set the surface label to UMLS mapping, generated via /home/nicole/bin/readumls.sh
# .ARGS
# 
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetUMLSMapping {} {
    # set up Freesurfer surface labels to UMLS mapping
    global vtkFreeSurferReadersSurface
    set vtkFreeSurferReadersSurface(0,surfacelabel) Unknown
    set vtkFreeSurferReadersSurface(0,umls) Unknown
    set vtkFreeSurferReadersSurface(1,surfacelabel) Corpus_callosum
    set vtkFreeSurferReadersSurface(1,umls) C0010090
    set vtkFreeSurferReadersSurface(2,surfacelabel) G_and_S_Insula_ONLY_AVERAGE
    set vtkFreeSurferReadersSurface(3,surfacelabel) G_cingulate-Isthmus
    set vtkFreeSurferReadersSurface(3,umls) C0175192
    set vtkFreeSurferReadersSurface(4,surfacelabel) G_cingulate-Main_part
    set vtkFreeSurferReadersSurface(4,umls) C0018427
    set vtkFreeSurferReadersSurface(5,surfacelabel) G_cuneus
    set vtkFreeSurferReadersSurface(5,umls) C0152307
    set vtkFreeSurferReadersSurface(6,surfacelabel) G_frontal_inf-Opercular_part
    set vtkFreeSurferReadersSurface(6,umls) C0262296
    set vtkFreeSurferReadersSurface(7,surfacelabel) G_frontal_inf-Orbital_part
    set vtkFreeSurferReadersSurface(7,umls) C0262300
    set vtkFreeSurferReadersSurface(8,surfacelabel) G_frontal_inf-Triangular_part
    set vtkFreeSurferReadersSurface(8,umls) C0262350
    set vtkFreeSurferReadersSurface(9,surfacelabel) G_frontal_middle
    set vtkFreeSurferReadersSurface(9,umls) C0152297
    set vtkFreeSurferReadersSurface(10,surfacelabel) G_frontal_superior
    set vtkFreeSurferReadersSurface(10,umls) C0152296
    set vtkFreeSurferReadersSurface(11,surfacelabel) G_frontomarginal
    set vtkFreeSurferReadersSurface(12,surfacelabel) G_insular_long
    set vtkFreeSurferReadersSurface(12,umls) C0228261
    set vtkFreeSurferReadersSurface(13,surfacelabel) G_insular_short
    set vtkFreeSurferReadersSurface(13,umls) C0262329
    set vtkFreeSurferReadersSurface(14,surfacelabel) G_occipital_inferior
    set vtkFreeSurferReadersSurface(14,umls) C0228231
    set vtkFreeSurferReadersSurface(15,surfacelabel) G_occipital_middle
    set vtkFreeSurferReadersSurface(15,umls) C0228220
    set vtkFreeSurferReadersSurface(16,surfacelabel) G_occipital_superior
    set vtkFreeSurferReadersSurface(16,umls) C0228230
    set vtkFreeSurferReadersSurface(17,surfacelabel) G_occipit-temp_lat-Or_fusiform
    set vtkFreeSurferReadersSurface(17,umls) C0228243
    set vtkFreeSurferReadersSurface(18,surfacelabel) G_occipit-temp_med-Lingual_part
    set vtkFreeSurferReadersSurface(18,umls) C0228244
    set vtkFreeSurferReadersSurface(19,surfacelabel) G_occipit-temp_med-Parahippocampal_part
    set vtkFreeSurferReadersSurface(19,umls) C0228249
    set vtkFreeSurferReadersSurface(20,surfacelabel) G_orbital
    set vtkFreeSurferReadersSurface(20,umls) C0152301
    set vtkFreeSurferReadersSurface(21,surfacelabel) G_paracentral
    set vtkFreeSurferReadersSurface(21,umls) C0228203
    set vtkFreeSurferReadersSurface(22,surfacelabel) G_parietal_inferior-Angular_part
    set vtkFreeSurferReadersSurface(22,umls) C0152305
    set vtkFreeSurferReadersSurface(23,surfacelabel) G_parietal_inferior-Supramarginal_part
    set vtkFreeSurferReadersSurface(23,umls) C0458319
    set vtkFreeSurferReadersSurface(24,surfacelabel) G_parietal_superior
    set vtkFreeSurferReadersSurface(24,umls) C0152303
    set vtkFreeSurferReadersSurface(25,surfacelabel) G_postcentral
    set vtkFreeSurferReadersSurface(25,umls) C0152302
    set vtkFreeSurferReadersSurface(26,surfacelabel) G_precentral
    set vtkFreeSurferReadersSurface(26,umls) C0152299
    set vtkFreeSurferReadersSurface(27,surfacelabel) G_precuneus
    set vtkFreeSurferReadersSurface(27,umls) C0152306
    set vtkFreeSurferReadersSurface(28,surfacelabel) G_rectus
    set vtkFreeSurferReadersSurface(28,umls) C0152300
    set vtkFreeSurferReadersSurface(29,surfacelabel) G_subcallosal
    set vtkFreeSurferReadersSurface(29,umls) C0175231
    set vtkFreeSurferReadersSurface(30,surfacelabel) G_subcentral
    set vtkFreeSurferReadersSurface(31,surfacelabel) G_temp_sup-G_temp_transv_and_interm_S
    set vtkFreeSurferReadersSurface(31,umls) C0152309
    set vtkFreeSurferReadersSurface(32,surfacelabel) G_temp_sup-Lateral_aspect
    set vtkFreeSurferReadersSurface(32,umls) C0152309
    set vtkFreeSurferReadersSurface(33,surfacelabel) G_temp_sup-Planum_polare
    set vtkFreeSurferReadersSurface(33,umls) C0152309
    set vtkFreeSurferReadersSurface(34,surfacelabel) G_temp_sup-Planum_tempolale
    set vtkFreeSurferReadersSurface(34,umls) C0152309
    set vtkFreeSurferReadersSurface(35,surfacelabel) G_temporal_inferior
    set vtkFreeSurferReadersSurface(35,umls) C0152311
    set vtkFreeSurferReadersSurface(36,surfacelabel) G_temporal_middle
    set vtkFreeSurferReadersSurface(36,umls) C0152310
    set vtkFreeSurferReadersSurface(37,surfacelabel) G_transverse_frontopolar
    set vtkFreeSurferReadersSurface(38,surfacelabel) Lat_Fissure-ant_sgt-ramus_horizontal
    set vtkFreeSurferReadersSurface(38,umls) C0262190
    set vtkFreeSurferReadersSurface(39,surfacelabel) Lat_Fissure-ant_sgt-ramus_vertical
    set vtkFreeSurferReadersSurface(39,umls) C0262186
    set vtkFreeSurferReadersSurface(40,surfacelabel) Lat_Fissure-post_sgt
    set vtkFreeSurferReadersSurface(40,umls) C0262310
    set vtkFreeSurferReadersSurface(41,surfacelabel) Medial_wall
    set vtkFreeSurferReadersSurface(42,surfacelabel) Pole_occipital
    set vtkFreeSurferReadersSurface(42,umls) C0228217
    set vtkFreeSurferReadersSurface(43,surfacelabel) Pole_temporal
    set vtkFreeSurferReadersSurface(43,umls) C0149552
    set vtkFreeSurferReadersSurface(44,surfacelabel) S_calcarine
    set vtkFreeSurferReadersSurface(44,umls) C0228224
    set vtkFreeSurferReadersSurface(45,surfacelabel) S_central
    set vtkFreeSurferReadersSurface(45,umls) C0228188
    set vtkFreeSurferReadersSurface(46,surfacelabel) S_central_insula
    set vtkFreeSurferReadersSurface(46,umls) C0228260
    set vtkFreeSurferReadersSurface(47,surfacelabel) S_cingulate-Main_part
    set vtkFreeSurferReadersSurface(47,umls) C0228189
    set vtkFreeSurferReadersSurface(48,surfacelabel) S_cingulate-Marginalis_part
    set vtkFreeSurferReadersSurface(48,umls) C0259792
    set vtkFreeSurferReadersSurface(49,surfacelabel) S_circular_insula_anterior
    set vtkFreeSurferReadersSurface(49,umls) C0228258
    set vtkFreeSurferReadersSurface(50,surfacelabel) S_circular_insula_inferior
    set vtkFreeSurferReadersSurface(50,umls) C0228258
    set vtkFreeSurferReadersSurface(51,surfacelabel) S_circular_insula_superior
    set vtkFreeSurferReadersSurface(51,umls) C0228258
    set vtkFreeSurferReadersSurface(52,surfacelabel) S_collateral_transverse_ant
    set vtkFreeSurferReadersSurface(52,umls) C0228226
    set vtkFreeSurferReadersSurface(53,surfacelabel) S_collateral_transverse_post
    set vtkFreeSurferReadersSurface(53,umls) C0228226
    set vtkFreeSurferReadersSurface(54,surfacelabel) S_frontal_inferior
    set vtkFreeSurferReadersSurface(54,umls) C0262251
    set vtkFreeSurferReadersSurface(55,surfacelabel) S_frontal_middle
    set vtkFreeSurferReadersSurface(55,umls) C0228199
    set vtkFreeSurferReadersSurface(56,surfacelabel) S_frontal_superior
    set vtkFreeSurferReadersSurface(56,umls) C0228198
    set vtkFreeSurferReadersSurface(57,surfacelabel) S_frontomarginal
    set vtkFreeSurferReadersSurface(57,umls) C0262269
    set vtkFreeSurferReadersSurface(58,surfacelabel) S_intermedius_primus-Jensen
    set vtkFreeSurferReadersSurface(59,surfacelabel) S_intracingulate
    set vtkFreeSurferReadersSurface(59,umls) C0262263
    set vtkFreeSurferReadersSurface(60,surfacelabel) S_intraparietal-and_Parietal_transverse
    set vtkFreeSurferReadersSurface(60,umls) C0228213
    set vtkFreeSurferReadersSurface(61,surfacelabel) S_occipital_anterior
    set vtkFreeSurferReadersSurface(61,umls) C0262194
    set vtkFreeSurferReadersSurface(62,surfacelabel) S_occipital_inferior
    set vtkFreeSurferReadersSurface(62,umls) C0262253
    set vtkFreeSurferReadersSurface(63,surfacelabel) S_occipital_middle_and_Lunatus
    set vtkFreeSurferReadersSurface(63,umls) C0262278
    set vtkFreeSurferReadersSurface(64,surfacelabel) S_occipital_superior_and_transversalis
    set vtkFreeSurferReadersSurface(64,umls) C0262348
    set vtkFreeSurferReadersSurface(65,surfacelabel) S_occipito-temporal_lateral
    set vtkFreeSurferReadersSurface(65,umls) C0228245
    set vtkFreeSurferReadersSurface(66,surfacelabel) S_occipito-temporal_medial_and_S_Lingual
    set vtkFreeSurferReadersSurface(66,umls) C0447427
    set vtkFreeSurferReadersSurface(67,surfacelabel) S_orbital_lateral
    set vtkFreeSurferReadersSurface(67,umls) C0228229
    set vtkFreeSurferReadersSurface(68,surfacelabel) S_orbital_medial-Or_olfactory
    set vtkFreeSurferReadersSurface(68,umls) C0262286
    set vtkFreeSurferReadersSurface(69,surfacelabel) S_orbital-H_shapped
    set vtkFreeSurferReadersSurface(69,umls) C0228206
    set vtkFreeSurferReadersSurface(70,surfacelabel) S_paracentral
    set vtkFreeSurferReadersSurface(70,umls) C0228204
    set vtkFreeSurferReadersSurface(71,surfacelabel) S_parieto_occipital
    set vtkFreeSurferReadersSurface(71,umls) C0228191
    set vtkFreeSurferReadersSurface(72,surfacelabel) S_pericallosal
    set vtkFreeSurferReadersSurface(73,surfacelabel) S_postcentral
    set vtkFreeSurferReadersSurface(73,umls) C0228212
    set vtkFreeSurferReadersSurface(74,surfacelabel) S_precentral-Inferior-part
    set vtkFreeSurferReadersSurface(74,umls) C0262257
    set vtkFreeSurferReadersSurface(75,surfacelabel) S_precentral-Superior-part
    set vtkFreeSurferReadersSurface(75,umls) C0262338
    set vtkFreeSurferReadersSurface(76,surfacelabel) S_subcentral_ant
    set vtkFreeSurferReadersSurface(76,umls) C0262197
    set vtkFreeSurferReadersSurface(77,surfacelabel) S_subcentral_post
    set vtkFreeSurferReadersSurface(77,umls) C0262318
    set vtkFreeSurferReadersSurface(78,surfacelabel) S_suborbital
    set vtkFreeSurferReadersSurface(79,surfacelabel) S_subparietal
    set vtkFreeSurferReadersSurface(79,umls) C0228216
    set vtkFreeSurferReadersSurface(80,surfacelabel) S_supracingulate
    set vtkFreeSurferReadersSurface(81,surfacelabel) S_temporal_inferior
    set vtkFreeSurferReadersSurface(81,umls) C0228242
    set vtkFreeSurferReadersSurface(82,surfacelabel) S_temporal_superior
    set vtkFreeSurferReadersSurface(82,umls) C0228237
    set vtkFreeSurferReadersSurface(83,surfacelabel) S_temporal_transverse
    set vtkFreeSurferReadersSurface(83,umls) C0228239
    set vtkFreeSurferReadersSurface(84,surfacelabel) S_transverse_frontopolar

    return
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersReadAnnotations 
# Read in the annotations specified.
# .ARGS
# _id model id
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersReadAnnotations {_id} {
    global vtkFreeSurferReaders Volume Model

    # read in this annot file, it's one up from the surf dir, then down into label
    set dir [file split [file dirname $vtkFreeSurferReaders(ModelFileName)]]
    set dir [lrange $dir 0 [expr [llength $dir] - 2]]
    lappend dir label
    set dir [eval file join $dir]
    set fname [lrange [file split [file rootname $vtkFreeSurferReaders(ModelFileName)]] end end]

    foreach a $vtkFreeSurferReaders(annots) {
        if {[lsearch $vtkFreeSurferReaders(assocFiles) $a] != -1} {
            set annotFileName [eval file join $dir $fname.$a.annot]
            if [file exists $annotFileName] {
                DevInfoWindow "Model $_id: Reading in $a file associated with this model: $annotFileName"
                set scalaridx [[$Model($_id,polyData) GetPointData] SetActiveScalars "labels"] 
                    
                if { $scalaridx == "-1" } {
                    if {$::Module(verbose)} {
                        DevInfoWindow "labels scalar doesn't exist for model $_id, creating"
                    }
                    set scalars scalars_$a
                    catch "$scalars Delete"
                    vtkIntArray $scalars
                    $scalars SetName "labels"
                    [$Model($_id,polyData) GetPointData] AddArray $scalars
                    [$Model($_id,polyData) GetPointData] SetActiveScalars "labels"
                } else {
                    set scalars [[$Model($_id,polyData) GetPointData] GetScalars $scalaridx]
                }
                set lut [Model($_id,mapper,viewRen) GetLookupTable]
                $lut SetRampToLinear
                
                set fssar fssar_$a
                catch "$fssar Delete"
                vtkFSSurfaceAnnotationReader $fssar
                $fssar SetFileName $annotFileName
                $fssar SetOutput $scalars
                $fssar SetColorTableOutput $lut
                # set the optional colour table filename
                if [file exists $vtkFreeSurferReaders(colorTableFilename)] {
                    if {$::Module(verbose)} {
                        puts "Color table file exists: $vtkFreeSurferReaders(colorTableFilename)"
                    }
                    $fssar SetColorTableFileName $vtkFreeSurferReaders(colorTableFilename)
                    $fssar UseExternalColorTableFileOn
                } else {
                    if {$::Module(verbose)} {
                        puts "Color table file does not exist: $vtkFreeSurferReaders(colorTableFilename)"
                    }
                    $fssar UseExternalColorTableFileOff
                }
                set retval [$fssar ReadFSAnnotation]
                if {$::Module(verbose)} {
                    puts "Return value from reading $annotFileName = $retval"
                }
                if {[vtkFreeSurferReadersCheckAnnotError $retval] != 0} {
                     [$Model($_id,polyData) GetPointData] RemoveArray "labels"
                    return
                }
                
                set ::Model(scalarVisibilityAuto) 0

                array unset _labels
                array set _labels [$fssar GetColorTableNames]
                set entries [lsort -integer [array names _labels]]
                if {$::Module(verbose)} {
                    puts "Label entries:\n$entries"
                    puts "0th: [lindex $entries 0], last:  [lindex $entries end]"
                }
                MainModelsSetScalarRange $_id [lindex $entries 0] [lindex $entries end]
                MainModelsSetScalarVisibility $_id 1
                Render3D
            } else {
                DevInfoWindow "Model $_id: $a file does not exist: $annotFileName"
            }
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersCheckAnnotError
# references vtkFSSurfaceAnnotationReader.h. Returns error value (0 means success)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersCheckAnnotError {val} {
    if {$val == 1} {
        DevErrorWindow "ERROR: error loading colour table"
    }
    if {$val == 2} {
        DevErrorWindow "ERROR: error loading annotation"
    }
    if {$val == 3} {
        DevErrorWindow "ERROR: error parsing color table"
    }
    if {$val == 4} {
        DevErrorWindow "ERROR: error parsing annotation"
    }
    if {$val == 5} {
        DevErrorWindow "WARNING: Unassigned labels"
        return 0
    }
    if {$val == 6} {
        DevErrorWindow "ERROR: no colour table defined"
    }
    return $val
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersModelApply
# Read in the model specified. Used in place of the temp ModelsFreeSurferPropsApply.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersModelApply {} {
    global vtkFreeSurferReaders Module Model Volume
    # Validate name
    set Model(name) $vtkFreeSurferReaders(ModelName)
    if {$Model(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this model."
        return
    }
    if {[ValidateName $Model(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersModelApply: Also loading these assoc files if they exist: $vtkFreeSurferReaders(assocFiles)"
    }
    # add a mrml node
    set n [MainMrmlAddNode Model]
    set i [$n GetID]

    vtkFreeSurferReadersBuildSurface $i
    vtkFreeSurferReadersReadAnnotations $i
    # pop up a window allowing you to choose which vectors and scalars to see
    # vtkFreeSurferReadersDisplayPopup $i

    # allow use of other module GUIs
    set Volumes(freeze) 0
    # If tabs are frozen, then return to the "freezer"
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
    return $i
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersModelCancel
# Cancel reading in the model
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersModelCancel {} {
    global vtkFreeSurferReaders Module Model Volume
    # model this after VolumePropsCancel - ModelPropsCancel?
    if {$Module(verbose)} {
        puts "vtkFreeSurferReadersModelCancel, calling ModelPropsCancel"
    }
    ModelsPropsCancel
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetLoad
# Add this kind of associated file to the list of files to load when reading in a model
# .ARGS
# param - the kind of associated file to read in, added to vtkFreeSurferReaders(assocFiles)
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetLoad {param} {
    global vtkFreeSurferReaders Volume

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersSetLoad: start: loading: $vtkFreeSurferReaders(assocFiles)"
    }
    set ind [lsearch $vtkFreeSurferReaders(assocFiles) $param]
    if {$vtkFreeSurferReaders(assocFiles,$param) == 1} {
        if {$ind == -1} {
            # add it to the list
            lappend vtkFreeSurferReaders(assocFiles) $param
        }
    } else {
        if {$ind != -1} {
            # remove it from the list
            set vtkFreeSurferReaders(assocFiles) [lreplace $vtkFreeSurferReaders(assocFiles) $ind $ind]
        }
    }
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersSetLoad: new: loading: $vtkFreeSurferReaders(assocFiles)"
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersLoadVolume
# Scriptable load function -- tied into command line arguments
# example: 
# ./slicer2-win32.exe --load-freesurfer-model c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw/surf/lh.pial --load-freesurfer-volume c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw/mri/orig/COR-.info --load-freesurfer-label-volume c:/pieper/bwh/data/MGH-Siemens15-SP.1-uw/mri/aseg/COR-.info &
# .ARGS 
# filename volume file to load
# labelMap the label map associated with this volume
# name the volume name
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersLoadVolume { filename {labelMap 0} {name ""} } {

    set ::Volume(labelMap) $labelMap
    set ::vtkFreeSurferReaders(VolumeFileName) $filename
    vtkFreeSurferReadersSetVolumeFileName 
    if { $name != "" } {
        set ::Volume(name) $name
    }
    return [vtkFreeSurferReadersApply]
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersLoadVolume
# Scriptable load function -- tied into command line arguments
# .ARGS 
# filename model file to load
# name name of the model
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersLoadModel { filename {name ""} } {

    set ::vtkFreeSurferReaders(ModelFileName) $filename
    vtkFreeSurferReadersSetModelFileName
    if { $name != "" } {
        set ::Volume(name) $name
    }
    return [vtkFreeSurferReadersModelApply]
}

#-------------------------------------------------------------------------------
# .PROC FreeSurferReadersFiducialsPointCreatedCallback
# This procedures is a callback procedule called when a Fiducial Point is
# created - use this to update the query atlas window
# .ARGS 
# .END
#-------------------------------------------------------------------------------
proc FreeSurferReadersFiducialsPointCreatedCallback {type fid pid} {

    if { [catch "package require Itcl"] } {
        return
    }

    foreach r [itcl::find objects -class regions] {
        $r findptscalars
    }

}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersMainFileCloseUpdate
# Called to clean up anything created in this sub module. 
# Deletes Volumes read in, along with their actors.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersMainFileCloseUpdate {} {
    global Volume viewRen Module

    # delete stuff that's been created
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReadersMainFileCloseUpdate"
    }

    # Delete RAS matrices from MGH files
    set rasmats [info commands rasmat*]
    foreach rasmat $rasmats {
        if {$::Module(verbose)} {
            puts "Deleting ras matrix $rasmat"
        }
        $rasmat Delete
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersReadMGH
# Called by MainVolumes.tcl MainVolumesRead to read in an MGH volume, returns -1
# if there is no vtkMGHReader. Assumes that the volume has been read already
# .ARGS v volume ID
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersReadMGH {v} {
    global Volume
    if { [info commands vtkMGHReader] == "" } {
        DevErrorWindow "No MGH Reader available."
        return -1
    }
    catch "mghreader Delete"
    vtkMGHReader mghreader
            
    mghreader SetFileName [Volume($v,node) GetFullPrefix]
    mghreader Update
    mghreader ReadVolumeHeader
    [[mghreader GetOutput] GetPointData] SetScalars [mghreader ReadVolumeData]
    Volume($v,vol) SetImageData [mghreader GetOutput]
    mghreader Delete
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersReadBfloat
# Called by MainVolumes.tcl MainVolumesRead to read in a bfloat volume
# .ARGS v volume ID
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersReadBfloat {v} {
    global Volume

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersReadBfloat: volume id = $v"
        DevInfoWindow "vtkFreeSurferReadersReadBfloat: volume id = $v - SKIPPING this!"
        return
    }

    if { [info commands vtkBVolumeReader] == "" } {
        DevErrorWindow "No MGH B Volume Reader available."
        return -1
    }

    catch "bfloatreader Delete"
    vtkBVolumeReader bfloatreader

    # not going to work, need to recreate the file name from what's saved in the node: FilePattern, FilePrefix, and ImageRange
    # bfloatreader SetFileName  [Volume($v,node) GetFullPrefix]
    set stem [Volume($v,node) GetFullPrefix]
    bfloatreader SetFilePrefix $stem
    bfloatreader SetStem $stem

    bfloatreader Update
    bfloatreader SetRegistrationFileName ${stem}.dat
    set regmat [bfloatreader GetRegistrationMatrix]

#    Volume($v,vol) SetImageData [bfloatreader GetOutput]
    bfloatreader Delete
}

#-------------------------------------------------------------------------------
# .PROC 
# vtkFreeSurferReadersSetCast
# Prints out confirmation of the castToShort flag
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetCast {} {
    global vtkFreeSurferReaders
    if {$::Module(verbose)} {
        puts "cast to short = $vtkFreeSurferReaders(castToShort)"
    }
}

#-------------------------------------------------------------------------------
# .PROC 
# vtkFreeSurferReadersCast
# Casts the given input volume v to toType. Returns new volume id on success, -1 on failure.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersCast {v {toType "Short"}} {
    global vtkFreeSurferReaders Volume

    if {$v == $Volume(idNone)} {
        DevErrorWindow "vtkFreeSurferReadersCast: You cannot cast Volume \"None\""
        return -1
    }
    set typeList {Float Double Int UnsignedInt Long UnsignedLong Short UnsignedShort Char UnsignedChar}
    if {[lsearch $typeList $toType] == -1} {
        DevErrorWindow "vtkFreeSurferReadersCast: ERROR: casting only is valid to these types:\n$typeList"
        return -1
    }

    if {$::Module(verbose)} { 
        puts "vtkFreeSurferReadersCast: casting volume $v to $toType"
    }
   
    # prepare the new volume
    set name [Volume($v,node) GetName]-$toType
    set vCast [DevCreateNewCopiedVolume $v "" $name]

    # this seems to not get done in the prior call
    Volume($vCast,node) SetLabelMap [Volume($v,node) GetLabelMap]

    set node [Volume($vCast,vol) GetMrmlNode]
    Mrml(dataTree) RemoveItem $node
    set nodeBefore [Volume($v,vol) GetMrmlNode]
    Mrml(dataTree) InsertAfterItem $nodeBefore $node
    MainUpdateMRML

    # do the cast
    catch "vtkFreeSurferReadersCaster Delete"
    vtkImageCast vtkFreeSurferReadersCaster
    vtkFreeSurferReadersCaster SetInput [Volume($v,vol) GetOutput]
    vtkFreeSurferReadersCaster SetOutputScalarTypeTo$toType

    vtkFreeSurferReadersCaster ClampOverflowOn
    vtkFreeSurferReadersCaster Update

    # Start copying in the output data.
    # Taken from VolumeMathDoCast
    Volume($vCast,vol) SetImageData [vtkFreeSurferReadersCaster GetOutput]
    Volume($vCast,node) SetScalarTypeTo$toType
    MainVolumesUpdate $vCast

    vtkFreeSurferReadersCaster Delete

    return $vCast
}

#-------------------------------------------------------------------------------
# .PROC 
# vtkFreeSurferReadersSetLoadColours
# Prints out confirmation of the loadColours flag
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetLoadColours {} {
    global vtkFreeSurferReaders
    if {$::Module(verbose)} {
        puts "load freesurfer colours = $vtkFreeSurferReaders(loadColours)"
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetColourFileName
# The filename is set elsewhere, in variable vtkFreeSurferReaders(colourFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetColourFileName {} {
    global vtkFreeSurferReaders Volume
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersSetColourFileName: colour file name set to $vtkFreeSurferReaders(colourFileName)"
    }
}
