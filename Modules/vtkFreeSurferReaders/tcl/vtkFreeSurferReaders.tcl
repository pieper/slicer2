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
    # flag to say that they've been loaded (is reset to zero when MainFileClose is called)
    set vtkFreeSurferReaders(coloursLoaded) 0
    # Set up the file name of the free surfer module's colour xml file, 
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


    # for QA 
    if {[info exist ::env(SUBJECTS_DIR)]} {
        set vtkFreeSurferReaders(QADirName) $::env(SUBJECTS_DIR)
        # try to read SUBJECTS env var from $SUBJECTS_DIR/scripts/subjects.csh
        if {[file exists [file join $::env(SUBJECTS_DIR) scripts subjects.csh]]} {
            if {$::Module(verbose)} { puts "Trying to read from subjects.csh in scripts subdir of $::env(SUBJECTS_DIR)" }
            set vtkFreeSurferReaders(QASubjectsFileName) [file join $::env(SUBJECTS_DIR) scripts subjects.csh]
            set vtkFreeSurferReaders(QAUseSubjectsFile) 1
            vtkFreeSurferReadersSetQASubjects
        }
    } else {
        set vtkFreeSurferReaders(QADirName) $::env(SLICER_HOME)
    }
    set vtkFreeSurferReaders(QAVolTypes) {aseg brain filled nu norm orig T1 wm}
    set vtkFreeSurferReaders(QADefaultVolTypes) {aseg norm}
    set vtkFreeSurferReaders(QAVolTypeNew) ""
    set vtkFreeSurferReaders(QAVolFiles) ""
    set vtkFreeSurferReaders(QAUseSubjectsFile) 0
    set vtkFreeSurferReaders(QADefaultView) "Normal"
    set vtkFreeSurferReaders(QAResultsList) {Approve Exclude Resegment Review}
    set vtkFreeSurferReaders(scan) 0
    set vtkFreeSurferReaders(scanStep) 1
    set vtkFreeSurferReaders(scanStartCOR) -128
    set vtkFreeSurferReaders(scanStartSAG) -128
    set vtkFreeSurferReaders(scanMs) 2000
    set vtkFreeSurferReaders(QAEdit) 0
    set vtkFreeSurferReaders(QASubjectFileName) "QA.log"
    set vtkFreeSurferReaders(QASubjects) ""
    set vtkFreeSurferReaders(MGHDecompressorExec) "/local/os/bin/gunzip"
    set vtkFreeSurferReaders(QAOpacity) "0.5"
    set vtkFreeSurferReaders(QAtime) 0
    # save these settings and put them back after done qa
    set vtkFreeSurferReaders(QAcast) $vtkFreeSurferReaders(castToShort)
    set vtkFreeSurferReaders(QAopacity) $::Slice(opacity)
    set vtkFreeSurferReaders(QAviewmode) $::View(mode)

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

    set Module($m,row1List) "Help Volumes Models Plot QA"
    set Module($m,row1Name) "{Help} {Volumes} {Models} {Plot} {QA}"
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
        {$Revision: 1.17 $} {$Date: 2005/03/18 21:35:18 $}]

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
    set help "The vtkFreeSuferReaders module allows you to read in FreeSufer format files.<P>Description by tab:<BR><UL><LI><B>Volumes</B>: Load in COR. mgh volumes.<LI><B>Models</B>: Load in model files (${vtkFreeSurferReaders(surfaces)}) and associate scalars with them.(${vtkFreeSurferReaders(scalars)}).<LI><B>Plot</B>: under development: plot statistical data.<LI><B>QA</B>: under development: Load in a series of freesurfer volume files for quality assurance. Select a subjects dir and volume types to load then press Start QA."
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

    #-------------------------------------------
    # Plot frame
    #-------------------------------------------
    set fPlot $Module(vtkFreeSurferReaders,fPlot)
    set f $fPlot

    DevAddLabel $f.lWarning "UNDER CONSTRUCTION"
    pack $f.lWarning -side top -padx $Gui(pad) -pady 0

    DevAddFileBrowse $f vtkFreeSurferReaders "PlotFileName" "Plot header file:" "vtkFreeSurferReadersSetPlotFileName" "fsgd" "\$Volume(DefaultDir)" "Open" "Browse for a plot header file (fsgd)"
    frame $f.fApply -bg $Gui(activeWorkspace)

    pack $f.fApply -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1

    #------------
    # Plot->Apply 
    #------------
    set f $Module(vtkFreeSurferReaders,fPlot).fApply

    DevAddButton $f.bApply "Apply" "vtkFreeSurferReadersPlotApply" 8
    DevAddButton $f.bCancel "Cancel" "vtkFreeSurferReadersPlotCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)

    #-------------------------------------------
    # QA frame
    #-------------------------------------------
    set fQA $Module(vtkFreeSurferReaders,fQA)
    set f $fQA

    DevAddLabel $f.lWarning "UNDER CONSTRUCTION"
    pack $f.lWarning  -side top -padx $Gui(pad) -pady 0

    frame $f.fSubjects  -bg $Gui(activeWorkspace)
    frame $f.fVolumes -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    frame $f.fOptions -bg $Gui(activeWorkspace)

    pack $f.fSubjects -side top  -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fVolumes -side top  -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fBtns -side top  -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1
    pack $f.fOptions  -side top  -padx $Gui(pad) -pady $Gui(pad) -fill x -expand 1

    #---------------
    # QA -> Subjects
    #---------------
    set f $fQA.fSubjects

    DevAddLabel $f.lCurDir "Current Subjects directory:"
    eval {entry $f.eCurDir -textvariable vtkFreeSurferReaders(QADirName) -width 50} $Gui(WEA)
    bind $f.eCurDir <Return> "vtkFreeSurferReadersSetQADirName [$f.eCurDir get]"

    pack $f.lCurDir -padx $Gui(pad) -pady $Gui(pad) -expand 1 -fill x
    pack $f.eCurDir -padx $Gui(pad) -pady $Gui(pad) -expand 1 -fill x

    DevAddButton $f.bSetDir "Set the Subjects directory" {vtkFreeSurferReadersSetQADirName $vtkFreeSurferReaders(QADirName)} 
    TooltipAdd $f.bSetDir "Select a directory containing subjects for which you wish to load FreeSurfer data.\nValid subject directories contain an mri subdirectory"
    pack $f.bSetDir -padx $Gui(pad) -pady $Gui(pad)
    
    # a list of the subjects found in the directory
    set qaSubjectNameListBox [ScrolledListbox $f.slbQASubjects 0 0 -height 4 -bg $Gui(activeWorkspace) -selectmode multiple]
    # make the scroll bars a bit skinnier when they appear
    $f.slbQASubjects.xscroll configure -width 10
    $f.slbQASubjects.yscroll configure -width 10
    set vtkFreeSurferReaders(qaSubjectsListbox) $qaSubjectNameListBox
    pack $f.slbQASubjects  -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    # if the list of subjects has already been set, populate it
    vtkFreeSurferReadersQAResetSubjectsListBox

    DevAddLabel $f.lClick "Click on Subjects you wish to load for QA\nOR set a subjects file name below"
    pack $f.lClick  -side top -padx $Gui(pad) -pady 0

    # alternately, read in the list of subjects from a file
    DevAddFileBrowse $f vtkFreeSurferReaders "QASubjectsFileName" "File with subject list:" {vtkFreeSurferReadersSetQASubjectsFileName ; vtkFreeSurferReadersSetQASubjects } "csh" "\$vtkFreeSurferReaders(QADirName)" "Open" "Browse for the subjects.csh containing a list of subjects"

    #---------------
    # QA -> Volumes
    #---------------
    set f $fQA.fVolumes

    frame $f.fVolSelect  -bg $Gui(activeWorkspace)
    frame $f.fVolAdd  -bg $Gui(activeWorkspace)

    pack $f.fVolSelect  -side top -padx $Gui(pad) -pady 0 -fill x
    pack  $f.fVolAdd  -side top -padx $Gui(pad) -pady 0 -fill x

    #----------------------------
    # QA -> Volumes  -> VolSelect
    #----------------------------

    set f $fQA.fVolumes.fVolSelect

    DevAddLabel $f.lVolumesSelect "Volumes you wish to load for each subject:"
    pack $f.lVolumesSelect  -side top -padx $Gui(pad) -pady 0

    foreach voltype $vtkFreeSurferReaders(QAVolTypes) {
        # turn on the default ones
        if {[lsearch $vtkFreeSurferReaders(QADefaultVolTypes) $voltype] == -1} {
            set defaultOn 0
        } else {
            set defaultOn 1
        }
        eval {checkbutton $f.c$voltype \
                  -text $voltype -command "vtkFreeSurferReadersQASetLoad $voltype" \
                  -variable vtkFreeSurferReaders(QAVolFiles,$voltype) \
                  -width 4 \
                  -indicatoron 0} $Gui(WCA)
        set vtkFreeSurferReaders(QAVolFiles,$voltype) $defaultOn
        vtkFreeSurferReadersQASetLoad $voltype
        pack $f.c$voltype -side left -padx 0
    }

    #-------------------------
    # QA -> Volumes  -> VolAdd
    #-------------------------

    set f $fQA.fVolumes.fVolAdd

    # let the user specify other volume names
    DevAddLabel $f.lVolumesAdd "Add a volume name:"
    pack $f.lVolumesAdd -side left -padx $Gui(pad)
    eval {entry $f.eVolumesAdd -textvariable vtkFreeSurferReaders(QAVolTypeNew)  -width 5} $Gui(WEA)
    pack $f.eVolumesAdd -side left -padx $Gui(pad) -expand 1 -fill x
    TooltipAdd $f.eVolumesAdd "Put the prefix of a volume you wish to load here, no extension (ie aseg2)"
    # now bind a proc to add this to the vol types list
    bind $f.eVolumesAdd <Return> "vtkFreeSurferReadersQASetLoadAddNew"

    #-----------
    # QA -> Btns
    #-----------
    set f $fQA.fBtns

    DevAddButton $f.bStart "Run QA" vtkFreeSurferReadersStartQA
    TooltipAdd $f.bStart "Start the QA process, loading the volumes for the selected subjects (if they exist)"

#    DevAddButton $f.bPause "Pause QA" vtkFreeSurferReadersPauseQA
#    TooltipAdd $f.bPause "Pause the QA process, hitting Continue QA again will continue"

#    DevAddButton $f.bContinue "Continue QA" vtkFreeSurferReadersContinueQA
#    TooltipAdd $f.bContinue "Continue the QA process from pause point"

    pack $f.bStart -side top -padx $Gui(pad) -expand 1

    DevAddLabel $f.lTime "Scan through slices manually"
    eval {scale $f.sTime -from 0 -to 9 \
              -length 220 -resolution 1 \
              -command vtkFreeSurferReadersQASetTime } \
        $::Gui(WSA) {-sliderlength 22}
    set ::vtkFreeSurferReaders(timescale) $f.sTime
    if { ![catch "package require iSlicer"] } {
        if { [info command istask] != "" } { 
            istask $f.play \
                -taskcommand vtkFreeSurferReadersStepFrame \
                -taskdelay $vtkFreeSurferReaders(scanMs) \
                -labeltext "Auto scanning display:" \
                -labelfont {helvetica 8} \
                -background $Gui(activeWorkspace)
            pack $f.play
        }
    }
    pack $f.lTime $f.sTime 

    #-----------
    # QA -> Options
    #-----------
    set f $fQA.fOptions

    eval {label $f.ltitle -text "Options:"} $Gui(WLA)
    pack $f.ltitle

    foreach subf { ScanPause ScanStep ScanStart Edit Opacity } {
        frame $f.f${subf} -bg $Gui(activeWorkspace)
        pack $f.f${subf}  -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    # QA -> Options -> ScanPause
    set f $fQA.fOptions.fScanPause

    # auto scroll time interval
    eval {label $f.lscanpause -text "Scan pause milliseconds:"} $Gui(WLA)
    eval {entry $f.escanpause -textvariable vtkFreeSurferReaders(scanMs) -width 3} $Gui(WEA)
    # if the pause length changes, need to reset the delay on the scanning task
    bind $f.escanpause <Return> "vtkFreeSurferReadersQAResetTaskDelay"
    pack $f.lscanpause -side left -padx $Gui(pad) 
    pack $f.escanpause -side left -padx $Gui(pad) -expand 1 -fill x

    # QA -> Options -> ScanStep
    set f $fQA.fOptions.fScanStep

    # auto scroll slice step increment
    eval {label $f.lscanstep -text "Scan slice step:"} $Gui(WLA)
    eval {entry $f.escanstep -textvariable vtkFreeSurferReaders(scanStep) -width 3} $Gui(WEA)
    pack $f.lscanstep -side left -padx $Gui(pad) 
    pack $f.escanstep -side left -padx $Gui(pad) -expand 1 -fill x
    # reset the time scale on the slider
    bind $f.escanstep <Return> vtkFreeSurferReadersResetTimeScale

    # QA -> Options -> ScanStart
    set f $fQA.fOptions.fScanStart
    # which slice to start at?
    eval {label $f.lscanstartCOR -text "Start at COR slice"} $Gui(WLA)
    eval {entry $f.escanstartCOR -textvariable vtkFreeSurferReaders(scanStartCOR) -width 4} $Gui(WEA)
    pack $f.lscanstartCOR -side left -padx 0
    pack $f.escanstartCOR -side left -padx $Gui(pad) -expand 1 -fill x

    eval {label $f.lscanstartSAG -text "at SAG"} $Gui(WLA)
    eval {entry $f.escanstartSAG -textvariable vtkFreeSurferReaders(scanStartSAG) -width 4} $Gui(WEA)
    pack $f.lscanstartSAG -side left -padx 0
    pack $f.escanstartSAG -side left -padx $Gui(pad) -expand 1 -fill x

    # reset the scale on the slider
    bind $f.escanstartCOR <Return> vtkFreeSurferReadersResetTimeScale
    bind $f.escanstartSAG <Return> vtkFreeSurferReadersResetTimeScale

    # QA -> Options -> Edit
    set f $fQA.fOptions.fEdit
    # have editing enabled?
    eval {checkbutton $f.cQAEdit \
              -text "Allow Editing" -command "vtkFreeSurferReadersSetQAEdit" \
              -variable vtkFreeSurferReaders(QAEdit) \
              -width 13 \
              -indicatoron 0} $Gui(WCA)
    pack $f.cQAEdit -side top -padx 0

    # QA -> Options -> Opacity
    set f $fQA.fOptions.fOpacity
    eval {label $f.lOpacity -text "Opacity of aseg"} $Gui(WLA)
    eval {entry $f.eOpacity -textvariable vtkFreeSurferReaders(QAOpacity) -width 4} $Gui(WEA)
    TooltipAdd $f.eOpacity "Opacity of foreground aseg volume, 0-1"
    pack $f.lOpacity -side left -padx 0
    pack $f.eOpacity -side left -padx $Gui(pad) -expand 1 -fill x

    # once everything's set up, reset the slider scale 
    vtkFreeSurferReadersResetTimeScale
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
    if {[info exist vtkFreeSurferReaders(gdfReader)]} {
        vtkFreeSurferReaders(gdfReader) Delete
    }
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
# Return the volume id.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersApply {} {
    global vtkFreeSurferReaders Module Volume

    set vid ""
    # switch on the file name, it can be:
    # a COR file (*.info that defines the volume) 
    # an mgh file (*.mgh, volume in one file)
    # a bfloat file (*.bfloat)
    if {[string match *.info $vtkFreeSurferReaders(VolumeFileName)]} {
        set vid [vtkFreeSurferReadersCORApply]
    } elseif {[string match *.mgh $vtkFreeSurferReaders(VolumeFileName)]} {
        set vid [vtkFreeSurferReadersMGHApply]
    } elseif {[string match *.mgz $vtkFreeSurferReaders(VolumeFileName)]} {
        # unzip it first and then send it to vtkFreeSurferReadersMGHApply
        if {[vtkFreeSurferReadersUncompressMGH] != -1} {
            set vid [vtkFreeSurferReadersMGHApply]
        }
    } elseif {[string match *.mgh.gz $vtkFreeSurferReaders(VolumeFileName)]} {
        if {[vtkFreeSurferReadersUncompressMGH] != -1} {
            set vid [vtkFreeSurferReadersMGHApply]
        }
    } elseif {[string match *.bfloat $vtkFreeSurferReaders(VolumeFileName)]} {
        set vid [vtkFreeSurferReadersBfloatApply]
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

    return $vid
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
        if {$::Module(verbose)} {
            DevErrorWindow "Problem: reader for this new volume number $i already exists, deleting it"
        } else {
            puts "Problem: reader for this new volume number $i already exists, deleting it"
        }
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
        $vtkFreeSurferReaders(loadColours) &&
        $vtkFreeSurferReaders(coloursLoaded) != 1} {
        if {$::Module(verbose)} {
            puts "vtkFreeSurferReadersCORApply: loading colour file $vtkFreeSurferReaders(colourFileName)."
        }
        # piggy back on the Color module
        set ::Color(fileName) $vtkFreeSurferReaders(colourFileName)
        ColorsLoadApply
        set vtkFreeSurferReaders(coloursLoaded) 1
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

    # return the volume id 
    return $i
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
        # Volume($i,node) DebugOn
    }

    # read in the MGH file
    # Set up the reading
    if {[info command Volume($i,vol,rw)] != ""} {
        # have to delete it first, needs to be cleaned up
        puts "Problem: reader for this new volume number $i already exists, deleting it"
        Volume($i,vol,rw) Delete
    }
    vtkMGHReader Volume($i,vol,rw)
    
    Volume($i,vol,rw) SetFilePrefix [file dirname $vtkFreeSurferReaders(VolumeFileName)]
    # set the filename
    Volume($i,vol,rw) SetFileName $vtkFreeSurferReaders(VolumeFileName)

#----------------
set saveverbose $Module(verbose)
set Module(verbose) 0
set usePos 0
set useMatrices 0

    # have to fudge it a little here, read the header to get the info needed
    if {$::Module(verbose)} {
        # set it up to be debugging on
        # Volume($i,vol,rw) DebugOn
        puts "vtkFreeSurferReadersMGHApply:\n\tReading volume header"
    }

    Volume($i,vol,rw) ReadVolumeHeader
    
    set updateReturn [Volume($i,vol,rw) Update]
    if {$updateReturn == 0} {
        DevErrorWindow "vtkMGHReader: update on volume $i failed."
    }

    # set the name and description of the volume
    $n SetName $Volume(name)
    $n SetDescription $Volume(desc)

   
    set Volume(isDICOM) 0
    set Volume($i,type) "MGH"


    # Set up the Volume(x) variables

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
    # use a slice spacing of 0 since we don't want it doubled in the fov calc
    set Volume(sliceSpacing) 0
# [lindex $spc 2]

    set Volume(gantryDetectorTilt) 0
    set Volume(numScalars) 1
    if {$::Module(verbose)} {
        puts "$vtkFreeSurferReaders(VolumeFileName) numScalars = $Volume(numScalars)"
    }
    set Volume(littleEndian) 0

    # Sag:LR RL Ax:SI IS Cor: AP PA
    # set Volume(scanOrder) {RL}
    set Volume(scanOrder) {PA}

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




    # Set the volume node properties
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
    Volume($i,node) SetFileType  $Volume($i,type)


    # now compute the RAS to IJK matrix from the values set above
    # Volume($i,node) ComputeRasToIjkFromScanOrder $Volume(scanOrder)
    # if that doesn't work, calculate corners
    # it doesn't as this isn't a standard scan order

    # read in the matrix from the headers and use it
    
    # get the IJK to RAS matrix from the volume:
    # x_r x_a x_s y_r y_a y_s z_r z_a z_s c_r c_a c_s
    set ijkmat [Volume($i,vol,rw) GetRASMatrix]
    if {$::Module(verbose)} {
        puts "MGH Reader: IJK matrix for volume $i: $ijkmat"
    }

    # calcualte tr, ta, ts from cr, ca, cs - from solving this equation for t_ras
    # xr yr zr tr    xspacing    0        0     0      width/2     cr
    # xa ya za ta  *   0      yspacing    0     0   *  height/2  = ca
    # xs ys zs ts      0         0     zspacing 0      depth/2     cs
    # 0  0  0  1       0         0        0     1         1        1
    # which yeilds
    # xr*xspacing*width/2 + yr*yspacing*height/2 + zr*zspacing*depth/2 + tr = cr
    # and similarly for xa... xs...
    set xr [lindex $ijkmat 0]
    set xa [lindex $ijkmat 1]
    set xs [lindex $ijkmat 2]
    set yr [lindex $ijkmat 3]
    set ya [lindex $ijkmat 4]
    set ys [lindex $ijkmat 5]
    set zr [lindex $ijkmat 6]
    set za [lindex $ijkmat 7]
    set zs [lindex $ijkmat 8]
    set cr [lindex $ijkmat 9]
    set ca [lindex $ijkmat 10]
    set cs [lindex $ijkmat 11]
    set xspacing [lindex $spc 0]
    set yspacing [lindex $spc 1]
    set zspacing [lindex $spc 2]
    set w2 [expr [lindex $dims 0] / 2]
    set h2 [expr [lindex $dims 1] / 2]
    set d2 [expr [lindex $dims 2] / 2]
        
    # try something - zero out the cras to take out the mgh shift away from origin
    set cr 0
    set ca 0
    set cs 0

    set tr [expr $cr - $xr*$xspacing*$w2 - $yr*$yspacing*$h2 - $zr*$zspacing*$d2]
    set ta [expr $ca - $xa*$xspacing*$w2 - $ya*$yspacing*$h2 - $za*$zspacing*$d2]
    set ts [expr $cs - $xs*$xspacing*$w2 - $ys*$yspacing*$h2 - $zs*$zspacing*$d2]

    if {$::Module(verbose)} {
        puts "tr = $tr, ta = $ta, ts = $ts"
    }

    # there's a problem with getting the MGH volume to display properly,
    # it comes up okay in slices mode but not when the RasToIjk matrix is set
    # in the volume (ijkmat) they have:
    # x_r x_a x_s
    # y_r y_a y_s
    # z_r z_a z_s
    # c_r c_a c_s
    # which, to compute the corners (ftl, ftr, fbr, ltl: first slice top left and right corners, first slice bottom right corner, and last slice top left corner) to pass to 
    # ComputeRasToIjkFromCorners, we need to apply the transform
    # xr yr zr tr
    # xa ya za ta
    # xs ys zs ts
    # 0  0  0  1
    # to the corners of the volume

    catch "rasmat$i Delete"
    vtkMatrix4x4 rasmat$i
    rasmat$i Identity

    # by rows:
    # x_r
    rasmat$i SetElement 0 0 [lindex $ijkmat 0]
    # y_r
    rasmat$i SetElement 0 1 [lindex $ijkmat 3]
    # z_r
    rasmat$i SetElement 0 2 [lindex $ijkmat 6]
    # t_r 
    rasmat$i SetElement 0 3 $tr

    # x_a
    rasmat$i SetElement 1 0 [lindex $ijkmat 1]
    # y_a
    rasmat$i SetElement 1 1 [lindex $ijkmat 4]
    # z_a
    rasmat$i SetElement 1 2 [lindex $ijkmat 7]
    # t_a
    rasmat$i SetElement 1 3 $ta

    # x_s
    rasmat$i SetElement 2 0 [lindex $ijkmat 2]
    # y_s
    rasmat$i SetElement 2 1 [lindex $ijkmat 5]
    # z_s
    rasmat$i SetElement 2 2 [lindex $ijkmat 8]
    # t_s
    rasmat$i SetElement 2 3 $ts

    # now include the scaling factor, from the voxel size
    catch "scalemat$i Delete"
    vtkMatrix4x4 scalemat$i
    scalemat$i Identity
    # s_x
    scalemat$i SetElement 0 0 $Volume(pixelWidth)
    # s_y
    scalemat$i SetElement 1 1 $Volume(pixelHeight)
    # s_z
    scalemat$i SetElement 2 2 $Volume(sliceThickness)
    
    # now apply it to the rasmat
    rasmat$i Multiply4x4 rasmat$i scalemat$i rasmat$i
    
    if {$::Module(verbose)} {
        if {[info command DevPrintMatrix4x4] != ""} {
            DevPrintMatrix4x4 rasmat$i "MGH vol $i RAS -> IJK (with scale)"
        }
    }

    # To get the corners, find the max values of the volume, assume mins are 0
    set maxx [lindex $dims 0]
    set maxy [lindex $dims 1]
    set maxz [lindex $dims 2]
    # set maxx $w2
    # set maxy $h2
    # set maxz $d2

    # first slice, top left corner = (minx,maxy,minz) 0,1,0 y axis
    set ftl [rasmat$i MultiplyPoint 0 0 0 1]

    # first slice, top right corner = (maxx,maxy,minz) 1,1,0
    set ftr [rasmat$i MultiplyPoint $maxx 0 0 1]

    # first slice, bottom right corner =(maxx,miny,minz) 1,0,0 x axis
    set fbr [rasmat$i MultiplyPoint $maxx $maxy 0 1]

    # last slice, top left corner = (minx,maxy,maxz) 0,1,1
    set ltl [rasmat$i MultiplyPoint 0 0 $maxz 1]

    # these aren't used
    set fc [rasmat$i MultiplyPoint 0 0 0 1]
    set lc [rasmat$i MultiplyPoint 0 0 0 1]

    if {$::Module(verbose)} {
        if {[info command DevPrintMatrix4x4] != ""} {
            DevPrintMatrix4x4 rasmat$i "MGH vol $i RAS -> IJK (with scaling, t_ras)"
        } 
        puts "dims $dims"
        puts "spc $spc"
        puts  "ftl $ftl"
        puts  "ftr $ftr"
        puts  "fbr $fbr"
        puts  "ltl $ltl"
    }
    # now do the magic bit
    # Volume($i,node) ComputeRasToIjkFromCorners $fc $ftl $ftr $fbr $lc $ltl
    Volume($i,node) ComputeRasToIjkFromCorners \
        [lindex $fc 0]  [lindex $fc 1]  [lindex $fc 2] \
        [lindex $ftl 0] [lindex $ftl 1] [lindex $ftl 2] \
        [lindex $ftr 0] [lindex $ftr 1] [lindex $ftr 2] \
        [lindex $fbr 0] [lindex $fbr 1] [lindex $fbr 2]  \
        [lindex $lc 0]  [lindex $lc 1]  [lindex $lc 2] \
        [lindex $ltl 0] [lindex $ltl 1] [lindex $ltl 2]


    # Turn off using the ras to vtk matrix, as otherwise the MGH volume is flipped in Y
    if {$::Module(verbose)} {
        puts "Turning off UseRasToVtkMatrix on volume $i"
    }
    Volume($i,node) UseRasToVtkMatrixOff



    # Reads in the volume via the Execute procedure
    MainUpdateMRML

    # Try doing some manual reading here - this is necessary to show the data legibly
    # doesn't seem to be necessary after using compute ras to ijk  from corners
    # Volume($i,vol) SetImageData [Volume($i,vol,rw) GetOutput]


    # Clean up

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

    # if we are successful set the FOV for correct display of this volume - tbd calc the max
    # set dim     [lindex [Volume($i,node) GetImageRange] 1]
    # set spacing [lindex [Volume($i,node) GetSpacing] 2]
    set dim     [lindex [Volume($i,node) GetDimensions] 0]
    set spacing [lindex [Volume($i,node) GetSpacing] 0]

    set fov     [expr $dim * $spacing]
    if {$::Module(verbose)} { 
        puts "MGH Reader setting fov to $fov - (dim $dim spacing $spacing)"
    }
    set View(fov) $fov
    MainViewSetFov

    # load in free surfer colours for a label map?
    if {[Volume($i,node) GetLabelMap] == 1 &&
        $vtkFreeSurferReaders(loadColours) &&
        $vtkFreeSurferReaders(coloursLoaded) != 1} {
        if {$::Module(verbose)} {
            puts "vtkFreeSurferReadersCORApply: loading colour file $vtkFreeSurferReaders(colourFileName)."
        }
        # piggy back on the Color module
        set ::Color(fileName) $vtkFreeSurferReaders(colourFileName)
        ColorsLoadApply
        set vtkFreeSurferReaders(coloursLoaded) 1
    }

    # display the new volume in the background of all slices if not a label map
    if {[Volume($i,node) GetLabelMap] == 1} {
        MainSlicesSetVolumeAll Label $i
    } else {
        MainSlicesSetVolumeAll Back $i
    }



#-------------------
set Module(verbose) $saveverbose

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

    # delete stuff that's been created in this module
    if {$Module(verbose) == 1} {
        puts "vtkFreeSurferReadersMainFileCloseUpdate"
    }
    
    # no id's in the volume idlist. So, find the stuff we made:
    foreach ca [info command vtkFreeSurferReaders(*,curveactor)] {
        if {$::Module(verbose) == 1} {
            puts "Removing surface actor for free surfer $ca"
        }
        viewRen RemoveActor  $ca
        $ca Delete
    }
    foreach ma [info command vtkFreeSurferReaders(*,mapper)] {
        if {$::Module(verbose)} {
            puts "Deleting $ma"
        }
        $ma Delete
    }
    foreach rw [info command Volume(*,vol,rw)] {
        if {$::Module(verbose)} {
            puts "Deleting $rw"
        }
        $rw Delete
    }



    # Delete RAS matrices from MGH files
    set rasmats [info commands rasmat*]
    foreach rasmat $rasmats {
        if {$::Module(verbose)} {
            puts "Deleting ras matrix $rasmat"
        }
        $rasmat Delete
    }

    # let myself know that the FreeSurfer colours were unloaded
    if {$::Module(verbose)} {
        puts "setting coloursLoaded to 0"
    }
    set ::vtkFreeSurferReaders(coloursLoaded) 0
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
    set retval [vtkFreeSurferReadersApply]
    if {$::Module(verbose)} { puts "vtkFreeSurferReadersLoadVolume returning id $retval" }
    return $retval
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersLoadModel
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

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersGDFInit
# Initializes the list of valid markers and colours for GDF
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersGDFInit {} {
    global vtkFreeSurferReaders 

    set vtkFreeSurferReaders(kValid,lMarkers)  {square circle diamond plus cross splus scross triangle}
    set vtkFreeSurferReaders(kValid,lColors) {red blue green yellow black purple orange pink brown}
    set vtkFreeSurferReaders(gGDF,lID) {}
    # should check to see here if the library is loaded
    # incorporate it into the vtkFreeSurferReaders library, then change this to 1
    set vtkFreeSurferReaders(gbLibLoaded) 0
    if {[info command vtkGDFReader] == ""} {
        # the library wasn't loaded in properly
        set vtkFreeSurferReaders(gbLibLoaded) 0
    } else {
        # declare a variable and save it
        catch "vtkFreeSurferReaders(gdfReader) Delete"
        vtkGDFReader vtkFreeSurferReaders(gdfReader)
        set vtkFreeSurferReaders(gbLibLoaded) 1
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersGDFPlotBuildWindow
# Creates the window for plotting into.
# vtkFreeSurferReaders(gGDF) - information gleaned from the header file.
#   lID - list of IDs
#   ID
#     bReadHeader - whether or not his GDF is parsed correctly
#     title - title of the graph
#     measurementName - label for the measurement
#     subjectName - subject name
#     dataFileName - data file name
#     cClasses - number of classes
#     classes,n - n is 0 -> cClasses
#       label - label for this class
#       marker - marker for this class
#       color - color for this class
#       subjects,n - n is 0 -> num subjects in this class
#         index - index of the subject
#     classes,label - label is the label
#       index - index is the index of this label
#     cVariables - number of variables
#     variables,n - n is 0 -> cVariables
#       label - label for this variable
#     nDefaultVariable - index of default variable
#     cSubjects - number of subjects
#     subjects,n - n is 0 -> cSubjects
#       id - label of this subject
#       nClass - index of class of this subject
#       variables,n - n is 0 -> cVariables
#         value - value for this variable for this subject
# vtkFreeSurferReaders(gPlot) - information about the plot, including current state.n
#   ID
#     state
#       nVariable - the index of the current variable
#       info - the info string displayed in lwInfo
#       lPoints - list of points
#       pointsChanged - dirty flag for points
#       data,subjects,n - where n is 0 -> cSubjects
#         variable - variable value for this subject (for state,nVariable)
#         measurement - measurement value for this subject
#       hiElement - name of hilighted element in plot
#       subjects,n - where n is 0 -> cSubjects
#         visible - whether or not is visible
#       classes,n - where n is 0 -> cClasses
#         visible - whether or not is visible
#       legend - subject or class
#       bTryRegressionLine - whether or not to try getting the offset/slope
# vtkFreeSurferReaders(gWidgets) - names of widgets
#   ID
#     wwTop - the top window
#     gwPlot - the graph widget
#     lwInfo - the info label widget
#     bWindowBuilt - boolean indicating if the window has been built
#     state
#       window
#         geometry - if hidden and reshown, will appear with same geometry
# .ARGS
# iID window id 
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersGDFPlotBuildWindow { iID } {
    global vtkFreeSurferReaders 

    set wwTop         .fsgdf-$iID
    set gwPlot        $wwTop.gwPlot
    set lwInfo        $wwTop.lwInfo
    set owVar         $wwTop.owVar
    set owLegendMode  $wwTop.owLegendMode
    set fwClassConfig $wwTop.fwClassConfig


    # Make the to window and set its title.
    toplevel $wwTop -height 500 -width 500
    wm title $wwTop $vtkFreeSurferReaders(gGDF,$iID,title)

    if { [catch "package require BLT"] } {
        DevErrorWindow "Must have the BLT package for plotting."
        return
    }
    # Make the graph.
    blt::graph $gwPlot \
        -title $vtkFreeSurferReaders(gGDF,$iID,title) \
        -plotbackground white \
        -relief raised -border 2

    # Bind our callbacks.
    $gwPlot legend bind all <Enter> [list vtkFreeSurferReadersGDFPlotCBLegendEnter $iID %W]
    $gwPlot legend bind all <Leave> [list vtkFreeSurferReadersGDFPlotCBLegendLeave $iID %W]
    $gwPlot legend bind all <ButtonPress-1> [list vtkFreeSurferReadersGDFPlotCBLegendClick $iID %W]
    bind $gwPlot <Motion> [list vtkFreeSurferReadersGDFPlotCBGraphMotion $iID %W %x %y]
    bind $gwPlot <Destroy> [list vtkFreeSurferReadersGDFPlotCBCloseWindow $iID] 

    # Hooking up the zoom functions seems to break some of the other
    # bindings. Needs more work.  
    # Blt_ZoomStack $gwPlot

    # Set the y axis label to the measurement name.
    $gwPlot axis configure y -title $vtkFreeSurferReaders(gGDF,$iID,measurementName)

    # Make the info label.
    set vtkFreeSurferReaders(gPlot,$iID,state,info) ""
    # tkuMakeActiveLabel
    # will need to update this
    DevAddLabel $lwInfo $vtkFreeSurferReaders(gPlot,$iID,state,info)

    # Make the variable menu.
    iwidgets::optionmenu $owVar \
        -labeltext "Variable menu" \
        -command "vtkFreeSurferReadersGDFPlotSetVariable $iID" 
        # -labelfont $::Gui(WLA)

    # Make the mode menu.
    iwidgets::optionmenu $owLegendMode \
        -labeltext "Mode menu" \
        -command "vtkFreeSurferReadersGDFPlotSetMode $iID" 
        # -labelfont $::Gui(WLA)
    $owLegendMode config -state disabled 
    $owLegendMode add command subject -label "View by subject"
    $owLegendMode add command class -label "View by class"
    $owLegendMode config -state normal 

    # Make a frame for the class controls, which we'll fill in later.
    iwidgets::labeledframe $fwClassConfig -labeltext "Configure Classes"

    # Place everything in the window.
    grid $gwPlot        -column 0 -row 0 -columnspan 3 -sticky news
    grid $lwInfo        -column 0 -row 1 -sticky nwe
    grid $owLegendMode  -column 1 -row 1 -sticky se
    grid $owVar         -column 2 -row 1 -sticky se
    grid $fwClassConfig -column 0 -row 2 -columnspan 3 -sticky ews
    grid columnconfigure $wwTop 0 -weight 1
    grid columnconfigure $wwTop 1 -weight 0
    grid columnconfigure $wwTop 2 -weight 0
    grid rowconfigure $wwTop 0 -weight 1
    grid rowconfigure $wwTop 1 -weight 0
    grid rowconfigure $wwTop 2 -weight 0

    # Set the names in the gWidgets array.
    set vtkFreeSurferReaders(gWidgets,$iID,wwTop)          $wwTop
    set vtkFreeSurferReaders(gWidgets,$iID,gwPlot)         $gwPlot
    set vtkFreeSurferReaders(gWidgets,$iID,lwInfo)         $lwInfo
    set vtkFreeSurferReaders(gWidgets,$iID,owVar)          $owVar
    set vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig)  [$fwClassConfig childsite]

    # Build the dynamic window elements for the window.
    vtkFreeSurferReadersGDFPlotBuildDynamicWindowElements $iID

    # Set the variable menu value to the header's default variable
    # index.
    $owVar config -value $vtkFreeSurferReaders(gGDF,$iID,nDefaultVariable)

    # Set our initial legen mode to class.
    $owLegendMode config -value class

    # Create the pen for our active element.
    $gwPlot pen create activeElement \
        -symbol circle -color red -pixels 0.2i -fill ""

    # Note that the window has been built.
    set vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt) 1
}

# Builds the window elements that are dependant on data, including the
# variable menu and the class configuration section.
proc vtkFreeSurferReadersGDFFPlotBuildDynamicWindowElements { iID } {
    global vtkFreeSurferReaders

    # First delete all entries in the menu. Then for each variable,
    # make an entry with that variable's label. The command for the
    # menu has already been set.
    $vtkFreeSurferReaders(gWidgets,$iID,owVar) config -state disabled
    set lEntries [$vtkFreeSurferReaders(gWidgets,$iID,owVar) entries]
    foreach entry $lEntries { 
        $vtkFreeSurferReaders(gWidgets,$iID,owVar) delete $entry
    }
    for { set nVar 0 } { $nVar < $gGDF($iID,cVariables) } { incr nVar } {
        $vtkFreeSurferReaders(gWidgets,$iID,owVar) add command $nVar \
            -label "$vtkFreeSurferReaders(gGDF,$iID,variables,$nVar,label)"
    }
    $vtkFreeSurferReaders(gWidgets,$iID,owVar) config -state normal

    # Fill out the class config frame. For each class, make an entry
    # with an option widget for colors and one for markers. Set up the
    # entries appropriately and bind it to the right variable.
    for { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { incr nClass } {

        set lw       $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig).lw$nClass
        set owMarker $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig).owMarker$nClass
        set owColor  $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig).owColor$nClass

        DevAddLabel $lw $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,label) 

        iwidgets::optionmenu $owMarker \
            -command "vtkFreeSurferReadersPlotSetNthClassMarker $iID $nClass" 
            # -labelfont $::Gui(WLA)
        $owMarker config -state disabled
        foreach marker $kValid(lMarkers) {
            $owMarker add command $marker -label $marker
        }
        $owMarker config -state normal
        $owMarker config -value $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,marker)
        
        iwidgets::optionmenu $owColor \
            -command "vtkFreeSurferReadersPlotSetNthClassColor $iID $nClass" 
            # -labelfont $::Gui(WLA)
        $owColor config -state disabled
        foreach color $vtkFreeSurferReaders(kValid,lColors) {
            $owColor add command $color -label $color
        }
        $owColor config -state normal
        $owColor config -value $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,color)
        
        # We're packing them in two columns (of three columns each).
        set nCol [expr ($nClass % 2) * 3]
        set nRow [expr $nClass / 2]
        grid $lw       -column $nCol            -row $nRow -sticky ew
        grid $owMarker -column [expr $nCol + 1] -row $nRow -sticky ew
        grid $owColor  -column [expr $nCol + 2] -row $nRow -sticky ew
    }
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 0 -weight 1
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 1 -weight 0
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 2 -weight 0
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 3 -weight 1
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 4 -weight 0
    grid columnconfigure $vtkFreeSurferReaders(gWidgets,$iID,fwClassConfig) 5 -weight 0
}















# Parse the header file, using the gdf functions to read it and pull
# data out of it. Returns -1 if there was an error, else it returns an
# ID number for the fsgdf.
proc vtkFreeSurferReadersPlotParseHeader { ifnHeader } {
    global vtkFreeSurferReaders

    # Generate a new ID.
    set ID 0
    while { [lsearch -exact $vtkFreeSurferReaders(gGDF,lID) $ID] != -1 } { incr ID }

    set err [catch {set vtkFreeSurferReaders(gGDF,$ID,object) [vtkFreeSurferReaders(gdfReader) ReadHeader $ifnHeader 1]} errMsg]
    if { $err } {
        puts "vtkFreeSurferReadersPlotParseHeader: Couldn't read header file $ifnHeader (ID = $ID)"
        puts "Error: $errMsg"
        return -1
    }

    # Grab all the data and put it into our TCL object. All these gdf*
    # functions return a list of results. The first is an integer
    # representing a result code. The second -> whatever is the actual
    # result of the function.
    set lResults [vtkFreeSurferReaders(gdfReader) GetTitle $vtkFreeSurferReaders(gGDF,$ID,object) ignore]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,title)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get the graph title."
        set vtkFreeSurferReaders(gGDF,$ID,title)  "Untitled graph"
    }

    set lResults [vtkFreeSurferReaders(gdfReader) GetMeasurementName $vtkFreeSurferReaders(gGDF,$ID,object) ignore]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,measurementName)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get the measurement label."
        set vtkFreeSurferReaders(gGDF,$ID,measurementName)  "Measurement"
    }

    set lResults [vtkFreeSurferReaders(gdfReader) GetSubjectName $vtkFreeSurferReaders(gGDF,$ID,object) ignore]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,subjectName)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get the subject name."
        set vtkFreeSurferReaders(gGDF,$ID,subjectName) "Unknown"
    }


    set lResults [vtkFreeSurferReaders(gdfReader) GetDataFileName $vtkFreeSurferReaders(gGDF,$ID,object) ignore]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,dataFileName)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get the data file name."
        set vtkFreeSurferReaders(gGDF,$ID,dataFileName)  "Unknown"
    }


    set lResults [vtkFreeSurferReaders(gdfReader) GetNumClasses $vtkFreeSurferReaders(gGDF,$ID,object)]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,cClasses)  [lindex $lResults 1]

        # If they didn't specify color or marker for the class, use
        # these and increment so all the classes are different.
        set nColor 0
        set nMarker 0

        for { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$ID,cClasses) } { incr nClass } {

            set lResults [vtkFreeSurferReaders(gdfReader) GetNthClassLabel $vtkFreeSurferReaders(gGDF,$ID,object) $nClass ignore]
            set err [lindex $lResults 0]
            if { 0 == $err } {
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label)  [lindex $lResults 1]
            } else {
                puts "WARNING: Could not get ${nClass}th label."
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) "Class $nClass"
            }
            
            set lResults [vtkFreeSurferReaders(gdfReader) GetNthClassMarker $vtkFreeSurferReaders(gGDF,$ID,object) $nClass ignore]
            set err [lindex $lResults 0]
            if { 0 == $err } {
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,marker)  [lindex $lResults 1]
            } else {
                puts "WARNING: Could not get ${nClass}th label."
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,marker) ""
            }
            
            
            # Look for the marker in the array of valid markers. If
            # it's not found, output a warning and set it to the
            # default.
            set n [lsearch -exact $vtkFreeSurferReaders(kValid,lMarkers) \
                       $vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,marker)]
            if { $n == -1 } {
                puts "WARNING: Marker for class $vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) was invalid."
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,marker) \
                    [lindex $vtkFreeSurferReaders(kValid,lMarkers) $nMarker]
                incr nMarker
                if { $nMarker >= [llength $vtkFreeSurferReaders(kValid,lMarkers)] } {set nMarker 0 }
            }
            
            set lResults [vtkFreeSurferReaders(gdfReader) GetNthClassColor $vtkFreeSurferReaders(gGDF,$ID,object) $nClass ignore]
            set err [lindex $lResults 0]
            if { 0 == $err } {
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,color)  [lindex $lResults 1]
            } else {
                puts "WARNING: Could not get ${nClass}th label."
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,color) ""
            }
            

            # Look for the coclor in the array of valid color. If
            # it's not found, output a warning and set it to the
            # default.
            set n [lsearch -exact $vtkFreeSurferReaders(kValid,lColors) \
                       $vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,color)]
            if { $n == -1 } {
                puts "WARNING: Color for class $vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) was invalid."
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,color) \
                    [lindex $vtkFreeSurferReaders(kValid,lColors) $nColor]
                incr nColor
                if { $nColor >= [llength $vtkFreeSurferReaders(kValid,lColors)] } { set nColor 0 }
            }
            
            # This is the reverse lookup for a class label -> index.
            set vtkFreeSurferReaders(gGDF,$ID,classes,$vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label),index) $nClass
            
            # Initialize all classes as visible.
            set vtkFreeSurferReaders(gPlot,$ID,state,classes,$nClass,visible) 1
        }
    } else {
        puts "ERROR: Could not get number of classes."
        return -1
    }


    set lResults [vtkFreeSurferReaders(gdfReader) GetNumVariables $vtkFreeSurferReaders(gGDF,$ID,object)]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,cVariables)  [lindex $lResults 1]

        for { set nVariable 0 } \
            { $nVariable < $vtkFreeSurferReaders(gGDF,$ID,cVariables) } { incr nVariable } {
                
                set lResults [vtkFreeSurferReaders(gdfReader) GetNthVariableLabel $vtkFreeSurferReaders(gGDF,$ID,object) $nVariable ignore]
                set err [lindex $lResults 0]
                if { 0 == $err } {
                    set vtkFreeSurferReaders(gGDF,$ID,variables,$nVariable,label)  [lindex $lResults 1]
                } else {
                    puts "WARNING: Could not get ${nClass}th label."
                    set vtkFreeSurferReaders(gGDF,$ID,variables,$nVariable,label)  "Variable $nVariable"
                }
                
            }
    } else {
        puts "ERROR: Could not get number of variables."
        return -1
    }
    

    set lResults [vtkFreeSurferReaders(gdfReader) GetDefaultVariable $vtkFreeSurferReaders(gGDF,$ID,object) ignore]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,defaultVariable)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get default variable."
        set vtkFreeSurferReaders(gGDF,$ID,defaultVariable) $vtkFreeSurferReaders(gGDF,$ID,variables,0,label)
    }

    set lResults [vtkFreeSurferReaders(gdfReader) GetDefaultVariableIndex $vtkFreeSurferReaders(gGDF,$ID,object)]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,nDefaultVariable)  [lindex $lResults 1]
    } else {
        puts "WARNING: Could not get default variable index."
        set vtkFreeSurferReaders(gGDF,$ID,defaultVariable) 0
    }

    set lResults [vtkFreeSurferReaders(gdfReader) GetNumSubjects $vtkFreeSurferReaders(gGDF,$ID,object)]
    set err [lindex $lResults 0]
    if { 0 == $err } {
        set vtkFreeSurferReaders(gGDF,$ID,cSubjects)  [lindex $lResults 1]

        for { set nSubject 0 } \
            { $nSubject < $vtkFreeSurferReaders(gGDF,$ID,cSubjects) } { incr nSubject } {
                
                set lResults [vtkFreeSurferReaders(gdfReader) GetNthSubjectID $vtkFreeSurferReaders(gGDF,$ID,object) $nSubject ignore]
                set err [lindex $lResults 0]
                if { 0 == $err } {
                    set vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubject,id)  [lindex $lResults 1]
                } else {
                    puts "WARNING: Could not get ${nSubject}th subject."
                    set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) "Subject $nSubject"
                }
                
                set lResults [vtkFreeSurferReaders(gdfReader) GetNthSubjectClass $vtkFreeSurferReaders(gGDF,$ID,object) $nSubject]
                set err [lindex $lResults 0]
                if { 0 == $err } {
                    set vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubject,nClass)  [lindex $lResults 1]
                } else {
                    puts "WARNING: Could not get ${nSubject}th subject."
                    set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) 0
                }
                
                
                for { set nVariable 0 } \
                    { $nVariable < $vtkFreeSurferReaders(gGDF,$ID,cVariables) } { incr nVariable } {
                        
                        set lResults [vtkFreeSurferReaders(gdfReader) GetNthSubjectNthValue \
                                          $vtkFreeSurferReaders(gGDF,$ID,object) $nSubject $nVariable]
                        set err [lindex $lResults 0]
                        if { 0 == $err } {
                            set vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubject,variables,$nVariable,value) \
                                [lindex $lResults 1]
                        } else {
                            puts "WARNING: Could not value for ${nSubject}th subject ${nVariable}th variable."
                            set vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubject,variables,$nVariable,value) 0
                        }
                    }
                
                # Initialize all subjects as visible.
                set vtkFreeSurferReaders(gPlot,$ID,state,subjects,$nSubject,visible) 1
            }
    } else {
        puts "ERROR: Could not get number of subjects."
        return -1
    }
    

    # This groups the subjects by the class they are in. For each
    # class, for each subject, if the subject is in the class, assign
    # the subject index to that subject-in-class index.
    for  { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$ID,cClasses) } { incr nClass } {
        set nSubjInClass 0
        for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$ID,cSubjects) } { incr nSubj } {
            if { $vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubj,nClass) == $nClass } {
                set vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,subjects,$nSubjInClass,index) $nSubj
                incr nSubjInClass
            }
        }
    }
    
    # We now have a header.
    set vtkFreeSurferReaders(gGDF,$ID,bReadHeader) 1

    # Start out trying to find the offset/slope for a class/var.
    set vtkFreeSurferReaders(gPlot,$ID,state,bTryRegressionLine) 1

    # If we have a window, build the dynamic elements.
    if { [info exists vtkFreeSurferReaders(gWidgets,$ID,bWindowBuilt)] && 
         $vtkFreeSurferReaders(gWidgets,$ID,bWindowBuilt) } {
        vtkFreeSurferReadersPlotBuildDynamicWindowElements $ID
    }

    if { 0 } {
        puts "$vtkFreeSurferReaders(gGDF,$ID,cClasses) classes:"
        for { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$ID,cClasses) } { incr nClass } {
            puts "$nClass: label=$vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label) marker=$vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,marker) color=$vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,color) reverse index=$vtkFreeSurferReaders(gGDF,$ID,classes,$vtkFreeSurferReaders(gGDF,$ID,classes,$nClass,label),index)"
        }
        
        puts "$vtkFreeSurferReaders(gGDF,$ID,cVariables) variables:"
        for { set nVar 0 } { $nVar < $vtkFreeSurferReaders(gGDF,$ID,cVariables) } { incr nVar } {
            puts "$nVar: label=$vtkFreeSurferReaders(gGDF,$ID,variables,$nVar,label)"
        }
        
        puts "$vtkFreeSurferReaders(gGDF,$ID,cSubjects) subjects:"
        for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$ID,cSubjects) } { incr nSubj } {
            puts "$nSubj: id=$vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubj,id) class=$vtkFreeSurferReaders(gGDF,$ID,subjects,$nSubj,nClass)"
        }
    }
    
    lappend vtkFreeSurferReaders(gGDF,lID) $ID
    return $ID
}

# This plots the current data on the graph. It is fast enough that it
# can be called any time the data is changed to completely redraw it
# from scratch.
proc vtkFreeSurferReadersPlotPlotData { iID } {
    global vtkFreeSurferReaders

    # Don't plot if the window isn't built or we don't have data.
    if { ![info exists vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt)] ||
         ![info exists vtkFreeSurferReaders(gGDF,$iID,bReadHeader)] ||
         !$vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt) || 
         !$vtkFreeSurferReaders(gGDF,$iID,bReadHeader) } {
        puts "vtkFreeSurferReadersPlotPlotData: the window isn't built or we don't have data"
        return
    }

    set gw $vtkFreeSurferReaders(gWidgets,$iID,gwPlot)

    # Set the x axis title to the label of the current variable.
    $gw axis configure x \
        -title $vtkFreeSurferReaders(gGDF,$iID,variables,$vtkFreeSurferReaders(gPlot,$iID,state,nVariable),label)

    # Remove all the elements and markers from the graph.
    set lElements [$gw element names *]
    foreach element $lElements {
        $gw element delete $element
    }
    set lMarkers [$gw marker names *]
    foreach marker $lMarkers {
        $gw marker delete $marker
    }
    
    # If we have no points, return.
    if { ![info exists vtkFreeSurferReaders(gPlot,$iID,state,lPoints)] || 
         [llength $vtkFreeSurferReaders(gPlot,$iID,state,lPoints)] == 0 } {
        return
    }

    # Depending on our legend mode, we'll draw by class or subject.
    if { $vtkFreeSurferReaders(gPlot,$iID,state,legend) == "class" } {
    
        # For each class, for each subject, if the subject's class is
        # the same as the current class, get its data points and add
        # them to a list. Then draw the entire list of data in the
        # class's color/marker. If the class is hidden, set the color
        # to white (so it shows up white in the legend) and hide the
        # element.
        for  { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { incr nClass } {
            
            set lData {}
            set nSubjInClass 0
            for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$iID,cSubjects) } { incr nSubj } {
                
                if { $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,nClass) == $nClass } {
                    
                    if { $vtkFreeSurferReaders(gPlot,$iID,state,pointsChanged) } {
                        vtkFreeSurferReadersPlotCalculateSubjectMeasurement $iID $nSubj
                    }
                    
                    set vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,variable) \
                        $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,variables,$vtkFreeSurferReaders(gPlot,$iID,state,nVariable),value)
                    
                    lappend lData $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,variable)
                    lappend lData $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,measurement)
                }
            }
            
            if { $vtkFreeSurferReaders(gPlot,$iID,state,classes,$nClass,visible) } {
                set bHide 0
                set color $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,color)
            } else {
                set bHide 1
                set color white
            }
            $gw element create $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,label) \
                -data $lData \
                -symbol $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,marker) \
                -color $color -linewidth 0 -outlinewidth 1 -hide $bHide \
                -activepen activeElement
        }
        
    } else {
        
        
        # For each subject, if the points have changed, calculate the #
        # measurements. Get the variable value. If the subject is visible,
        # set # the hide flag to 0 and the color to the subject's class
        # color, else # set the hide flag to 1 and set the color to
        # white. Create the # element.
        for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$iID,cSubjects) } { incr nSubj } {
            
            if { $vtkFreeSurferReaders(gPlot,$iID,state,pointsChanged) } {
                vtkFreeSurferReadersPlotCalculateSubjectMeasurement $iID $nSubj
            }
            
            set vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,variable) \
                $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,variables,$vtkFreeSurferReaders(gPlot,$iID,state,nVariable),value)
            
            if {  $vtkFreeSurferReaders(gPlot,$iID,state,subjects,$nSubj,visible) } {
                set bHide 0
                set color $vtkFreeSurferReaders(gGDF,$iID,classes,$vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,nClass),color)
            } else {
                set bHide 1
                set color white
            }
            $gw element create $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,id) \
                -data [list $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,variable) \
                           $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,measurement)] \
                -symbol $vtkFreeSurferReaders(gGDF,$iID,classes,$vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,nClass),marker) \
                -color $color -linewidth 0 -outlinewidth 1 -hide $bHide \
                -activepen activeElement
        }
    }

    # If we're trying to draw the regression line, for each class, if
    # the class is visible, get the offset and slope for that class
    # and the current variable. This depends on the point we're
    # drawing, so get the avg of all the points if necessary. Then
    # make a marker calculating two points on the line. if
    # gdfOffsetSlope() fails, set the bTryRegressionLine flag to
    # false, so we won't try drawing it again.
    if { $vtkFreeSurferReaders(gPlot,$iID,state,bTryRegressionLine) } {

        for  { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { incr nClass } {
        
            if { $vtkFreeSurferReaders(gPlot,$iID,state,classes,$nClass,visible) } {
        
                set nVar $vtkFreeSurferReaders(gPlot,$iID,state,nVariable)
                
                # Calc the avg offset and slope for all points.
                set offset 0
                set slope 0
                set cGood 0
                foreach lPoint $vtkFreeSurferReaders(gPlot,$iID,state,lPoints) {
                    scan $lPoint "%d %d %d" x y z
                    set lResults [vtkFreeSurferReaders(gdfReader) OffsetSlope $vtkFreeSurferReaders(gGDF,$iID,object) \
                                      $nClass $nVar $x $y $z]
                    set err [lindex $lResults 0]
                    if { 0 == $err } {
                        set offset [expr $offset + [lindex $lResults 1]]
                        set slope [expr $slope + [lindex $lResults 2]]
                        incr cGood
                    } else {
                        set vtkFreeSurferReaders(gPlot,$iID,state,bTryRegressionLine) 0
                        break
                    }
                    
                    if { $cGood > 0 } {
                        set x1 -200
                        set y1 [expr ($slope * $x1) + $offset]
                        set x2 200
                        set y2 [expr ($slope * $x2) + $offset]
                        
                        $gw marker create line \
                            -coords [list $x1 $y1 $x2 $y2] \
                            -outline $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,color) \
                            -dashes {5 5}
                    }
                }
            }
            
            if { $vtkFreeSurferReaders(gPlot,$iID,state,bTryRegressionLine) == 0 } { break }
        }
    }
    
    set vtkFreeSurferReaders(gPlot,$iID,state,pointsChanged) 0
}


# Accesses and calculates the (averaged if necessary) measurment
# values at the current point(s). Stores the values in gPlot.
proc vtkFreeSurferReadersPlotCalculateSubjectMeasurement { iID inSubject } {
    global vtkFreeSurferReaders

    # Get the average of the points we've been given.
    set meas 0
    set cGood 0
    foreach lPoint $vtkFreeSurferReaders(gPlot,$iID,state,lPoints) {
    
        scan $lPoint "%d %d %d" x y z
        set lResults [vtkFreeSurferReaders(gdfReader) GetNthSubjectMeasurement $vtkFreeSurferReaders(gGDF,$iID,object) \
                          $inSubject $x $y $z]
        set err [lindex $lResults 0]
        if { 0 == $err } {
            set meas [expr $meas + [lindex $lResults 1]]
            incr cGood
        }
    }
    if { $cGood > 0 } {
        set meas [expr $meas / $cGood.0]
    }
    
    # Store the values in gPlot.
    set vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$inSubject,measurement) $meas
}



# Hilight/UnhilightElement works on an element by name (which could be
# a subject or class, depending on viewing mode). It will
# select/unselect the element name in the legend and change the
# drawing pen of the element in the graph, which if activated draws it
# with a red circle around it.
proc vtkFreeSurferReadersGDFPloHilightElement { iID iElement } {
    global vtkFreeSurferReaders
    $vtkFreeSurferReaders(gWidgets,$iID,gwPlot) legend activate $iElement
    $vtkFreeSurferReaders(gWidgets,$iID,gwPlot) element activate $iElement
}

proc vtkFreeSurferReadersGDFPlotUnhilightElement { iID iElement } {
    global vtkFreeSurferReaders
    $vtkFreeSurferReaders(gWidgets,$iID,gwPlot) legend deactivate $iElement
    $vtkFreeSurferReadersgWidgets,$iID,gwPlot) element deactivate $iElement
}

# Shows or hide an element by name, in subject or class mode. Changes
# the value of the gPlot visibility flag.
proc vtkFreeSurferReadersGDFPlotToggleVisibility { iID iElement } {
    global vtkFreeSurferReaders

    # If we're in subject legend mode, the legend label is a subject
    # name. Get the subject index and toggle its visibility. If we're in
    # class legend mode, the legend label is a class name, so get the
    # class index and toggle its visibility.
    if { $vtkFreeSurferReaders(gPlot,$iID,state,legend) == "subject" } {
        set nSubj [vtkFreeSurferReadersGDFPlotGetSubjectIndexFromID $iID $iElement]
        if { $vtkFreeSurferReaders(gPlot,$iID,state,subjects,$nSubj,visible) } {
            set vtkFreeSurferReaders(gPlot,$iID,state,subjects,$nSubj,visible) 0
        } else {
            set vtkFreeSurferReaders(gPlot,$iID,state,subjects,$nSubj,visible) 1
        }
    } else {
        set nClass [vtkFreeSurferReadersGDFPlotGetClassIndexFromLabel $iID $iElement]
        if { $vtkFreeSurferReaders(gPlot,$iID,state,classes,$nClass,visible) } {
            set vtkFreeSurferReaders(gPlot,$iID,state,classes,$nClass,visible) 0
        } else {
            set vtkFreeSurferReaders(gPlot,$iID,state,classes,$nClass,visible) 1
        }
    }
}

# Focus/Unfocus is called to 'mouseover' an element. It
# Hilight/Unhilights an element and puts or removes the subject name
# in a text marker in the graph.
proc vtkFreeSurferReadersGDFPlotUnfocusElement { iID } {
    global vtkFreeSurferReaders

    # If we have a focused element, unhighlight it, set the
    # highlighted element name to null, and delete the hover text
    # marker.
    if { [info exists vtkFreeSurferReaders(gPlot,$iID,state,hiElement)] && \
             "$vtkFreeSurferReaders(gPlot,$iID,state,hiElement)" != "" } {
        vtkFreeSurferReadersGDFPlotUnhilightElement $iID $vtkFreeSurferReaders(gPlot,$iID,state,hiElement)
        set vtkFreeSurferReaders(gPlot,$iID,state,hiElement) ""
        $vtkFreeSurferReaders(gWidgets,$iID,gwPlot) marker delete hover
    }
}

proc vtkFreeSurferReadersGDFPlotFocusElement { iID iElement inSubjInClass iX iY } {
    global vtkFreeSurferReaders

    # Set the highlighted element name and highlight the element.
    set vtkFreeSurferReaders(gPlot,$iID,state,hiElement) $iElement
    vtkFreeSurferReadersGDFPlotHilightElement $iID $vtkFreeSurferReaders(gPlot,$iID,state,hiElement)

    # Need to get the subject name. If we're in subject mode, this is
    # just the element name, otherwise we're getting the class name in
    # the element name so get the class index, then use that and the
    # parameter we got (index of the data point, also the
    # subject-in-class index) to get th subject index, and then the
    # subject name.
    if { $vtkFreeSurferReaders(gPlot,$iID,state,legend) == "subject" } {
        set sId $iElement
    } else {
        set nClass [vtkFreeSurferReadersGDFPlotGetClassIndexFromLabel $iID $iElement]
        set nSubj $vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,subjects,$inSubjInClass,index)
        set sId $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,id)
    }
    $vtkFreeSurferReaders(gWidgets,$iID,gwPlot) marker create text \
        -name hover -text $sId -anchor nw \
        -coords [list $iX $iY]
}


# Finds the element under the mouse.
proc vtkFreeSurferReadersGDFPlotFindMousedElement { iID iX iY } {
    global vtkFreeSurferReaders
    set bFound [$vtkFreeSurferReaders(gWidgets,$iID,gwPlot) element closest $iX $iY aFound -halo 10]
    if { $bFound } {
        return [list $aFound(name) $aFound(index) $aFound(x) $aFound(y)]
    }
    return ""
}


# Converts from subject or class names to indicies.
proc vtkFreeSurferReadersGDFPlotGetSubjectIndexFromID { iID iSubjID } {
    global vtkFreeSurferReaders
    for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$iID,cSubjects) } { incr nSubj } {
        if { "$iSubjID" == "$vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,id)" } { 
            return $nSubj 
        }
    }
    return -1
}

proc vtkFreeSurferReadersGDFPlotGetClassIndexFromLabel { iID iLabel } {
    global vtkFreeSurferReaders
    for { set nClass 0 } { $nClass < $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { incr nClass } {
        if { "$iLabel" == "$vtkFreeSurferReaders(gGDF,$iID,classes,$nClass,label)" } { 
            return $nClass 
        }
    }
    return -1
}

# Our callbacks.
proc vtkFreeSurferReadersGDFPlotCBCloseWindow { iID } {
    global vtkFreeSurferReaders
    set vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt) 0
}

proc vtkFreeSurferReadersGDFPlotCBLegendEnter { iID igw } {
    vtkFreeSurferReadersGDFPlotHilightElement $iID [$igw legend get current]
}

proc vtkFreeSurferReadersGDFPlotCBLegendLeave { iID igw } {
    vtkFreeSurferReadersGDFPlotUnhilightElement $iID [$igw legend get current]
}

proc vtkFreeSurferReadersGDFPlotCBLegendClick { iID igw } {
    vtkFreeSurferReadersGDFPlotToggleVisibility $iID [$igw legend get current]
    vtkFreeSurferReadersGDFPlotPlotData $iID
}

proc vtkFreeSurferReadersGDFPlotCBGraphMotion { iID igw iX iY } {
    vtkFreeSurferReadersGDFPlotUnfocusElement $iID
    set lResult [vtkFreeSurferReadersGDFPlotFindMousedElement $iID $iX $iY]
    set element [lindex $lResult 0]
    if { "$element" != "" } { 
        set index [lindex $lResult 1]
        set x [lindex $lResult 2]
        set y [lindex $lResult 3]
        vtkFreeSurferReadersGDFPlotFocusElement $iID $element $index $x $y
    }
}

# Read a header file.
proc vtkFreeSurferReadersGDFPlotRead { ifnHeader } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        return -1
    }
    set ID [vtkFreeSurferReadersGDFPlotParseHeader $ifnHeader]
    return $ID
}

# Print information about the header.
proc vtkFreeSurferReadersGDFPlotPrint { iID } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersGDFPlotPrint: gb lib is not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    gdfPrintStdout $vtkFreeSurferReaders(gGDF,$iID,object)
}

# GDF callbacks.
proc vtkFreeSurferReadersGDFPlotCBCloseWindow { iID } {
    global vtkFreeSurferReaders
    set vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt) 0
}

proc vtkFreeSurferReadersGDFPlotCBLegendEnter { iID igw } {
    vtkFreeSurferReadersGDFPlotHilightElement $iID [$igw legend get current]
}

proc vtkFreeSurferReadersGDFPlotCBLegendLeave { iID igw } {
    vtkFreeSurferReadersGDFPlotUnhilightElement $iID [$igw legend get current]
}

proc vtkFreeSurferReadersGDFPlotCBLegendClick { iID igw } {
    vtkFreeSurferReadersGDFPlotToggleVisibility $iID [$igw legend get current]
    vtkFreeSurferReadersGDFPlotPlotData $iID
}

proc vtkFreeSurferReadersGDFPlotCBGraphMotion { iID igw iX iY } {
    vtkFreeSurferReadersGDFPlotUnfocusElement $iID
    set lResult [vtkFreeSurferReadersGDFPlotFindMousedElement $iID $iX $iY]
    set element [lindex $lResult 0]
    if { "$element" != "" } { 
        set index [lindex $lResult 1]
        set x [lindex $lResult 2]
        set y [lindex $lResult 3]
        vtkFreeSurferReadersGDFPlotFocusElement $iID $element $index $x $y
    }
}


# Show or hide the window. If it hasn't been built, builds the window
# first.
proc vtkFreeSurferReadersPlotShowWindow { iID } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotShowWindow: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    if { ![info exists vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt)] ||
         !$vtkFreeSurferReaders(gWidgets,$iID,bWindowBuilt) } {
        vtkFreeSurferReadersPlotBuildWindow $iID
    }
    wm deiconify $vtkFreeSurferReaders(gWidgets,$iID,wwTop)
    if { [info exists vtkFreeSurferReaders(gWidgets,$iID,state,window,geometry)] } {
        wm geometry $vtkFreeSurferReaders(gWidgets,$iID,wwTop) $vtkFreeSurferReaders(gWidgets,$iID,state,window,geometry)
    }
}

proc vtkFreeSurferReadersPlotHideWindow { iID } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { return }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    if { [info exists vtkFreeSurferReaders(gWidgets,$iID,wwTop)] } {
        set vtkFreeSurferReaders(gWidgets,$iID,state,window,geometry) \
        [wm geometry $vtkFreeSurferReaders(gWidgets,$iID,wwTop)]
        wm withdraw $vtkFreeSurferReaders(gWidgets,$iID,wwTop)
    }
}


# Set the current variable.
proc vtkFreeSurferReadersPlotSetVariable { iID inVariable } {
    global vtkFreeSurferReaders 
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetVariable: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }

    set vtkFreeSurferReaders(gPlot,$iID,state,nVariable) $inVariable

    vtkFreeSurferReadersPlotPlotData $iID
}


# Set legend mode to subject or class.
proc vtkFreeSurferReadersPlotSetMode { iID iMode } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetMode: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    if { $iMode != "subject" && $iMode != "class" } { 
        return 
    }

    set vtkFreeSurferReaders(gPlot,$iID,state,legend) $iMode

    vtkFreeSurferReadersPlotPlotData $iID
}


# Set display settings for a class.
proc vtkFreeSurferReadersPlotSetNthClassMarker { iID inClass iMarker } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetNthClassMarker: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    if { $inClass < 0 || $inClass >= $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { 
        return 
    }
    if { [lsearch -exact $vtkFreeSurferReaders(kValid,lMarkers) $iMarker] == -1 } { 
        return 
    }

    set vtkFreeSurferReaders(gGDF,$iID,classes,$inClass,marker) $iMarker

    vtkFreeSurferReadersPlotPlotData $iID
}

proc vtkFreeSurferReadersPlotSetNthClassColor { iID inClass iColor } {
    global vtkFreeSurferReaders 
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetNthClassColor: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    if { $inClass < 0 || $inClass >= $vtkFreeSurferReaders(gGDF,$iID,cClasses) } { 
        return 
    }
    if { [lsearch -exact $vtkFreeSurferReaders(kValid,lColors) $iColor] == -1 } { 
        return 
    }

    set vtkFreeSurferReaders(gGDF,$iID,classes,$inClass,color) $iColor

    vtkFreeSurferReadersPlotPlotData $iID
}


# Choose a point to be displayed. Either choose one point or make a
# point list to be averaged.
proc vtkFreeSurferReadersPlotSetPoint { iID iX iY iZ } {
    global vtkFreeSurferReaders 
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetPoint: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    vtkFreeSurferReadersPlotBeginPointList $iID
    vtkFreeSurferReadersPlotAddPoint $iID $iX $iY $iZ
    vtkFreeSurferReadersPlotEndPointList $iID
}

proc vtkFreeSurferReadersPlotBeginPointList { iID } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotBeginPointList: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    set vtkFreeSurferReaders(gPlot,$iID,state,lPoints) {}
}

proc vtkFreeSurferReadersPlotAddPoint { iID iX iY iZ } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotAddPoint: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    lappend vtkFreeSurferReaders(gPlot,$iID,state,lPoints) [list $iX $iY $iZ]
    set vtkFreeSurferReaders(gPlot,$iID,state,pointsChanged) 1
}

proc vtkFreeSurferReadersPlotEndPointList { iID } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotEndPointList: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    vtkFreeSurferReadersPlotPlotData $iID
}


# Set the info string displayed under the graph.
proc vtkFreeSurferReadersPlotSetInfo { iID isInfo } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSetInfo: gb lib is not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    set vtkFreeSurferReaders(gPlot,$iID,state,info) $isInfo
}


# Save the currently plotted data to a table.
proc vtkFreeSurferReadersPlotSaveToTable { iID ifnTable } {
    global vtkFreeSurferReaders
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }

    set fp 0
    set err [catch {set fp [open $ifnTable w+]}]
    if { $err || $fp == 0 } {
        puts "vtkFreeSurferReadersPlotSaveToTable: Couldn't write file $ifnTable."
        return
    }
    
    puts $fp "Graph: $vtkFreeSurferReaders(gGDF,$iID,title)"
    puts $fp "Data: $vtkFreeSurferReaders(gGDF,$iID,dataFileName)"
    puts $fp "Variable: $vtkFreeSurferReaders(gGDF,$iID,variables,$vtkFreeSurferReaders(gPlot,$iID,state,nVariable),label)"
    puts $fp "Measurement: $vtkFreeSurferReaders(gGDF,$iID,measurementName)"
    puts $fp "subject id, class id, variable value, measurement value"
    puts $fp "------------"
    for { set nSubj 0 } { $nSubj < $vtkFreeSurferReaders(gGDF,$iID,cSubjects) } { incr nSubj } {

        set subjLabel $vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,id)
        set classLabel $vtkFreeSurferReaders(gGDF,$iID,classes,$vtkFreeSurferReaders(gGDF,$iID,subjects,$nSubj,nClass),label)
        set var $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,variable)
        set meas $vtkFreeSurferReaders(gPlot,$iID,state,data,subjects,$nSubj,measurement)

        puts $fp "$subjLabel $classLabel $var $meas"
    }
    puts $fp "------------"
    puts ""

    close $fp
}


# Save the current plot graphic to a postscript file.
proc vtkFreeSurferReadersPlotSaveToPostscript { iID ifnPS } {
    global vtkFreeSurferReaders
    if { !$vtkFreeSurferReaders(gbLibLoaded) } { 
        puts "vtkFreeSurferReadersPlotSaveToPostscript: gb lib not loaded."
        return 
    }
    if { [lsearch $vtkFreeSurferReaders(gGDF,lID) $iID] == -1 } { 
        puts "ID $iID not found"
        return 
    }
    set err [catch {$vtkFreeSurferReaders(gWidgets,$iID,gwPlot) postscript output $ifnPS} sResult]
    if { $err } {
        puts "vtkFreeSurferReadersPlotSaveToPostscript: Could not save postscript file: $sResult"
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetPlotFileName
# The filename is set elsehwere, in variable vtkFreeSurferReaders(PlotFileName)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetPlotFileName {} {
    global vtkFreeSurferReaders Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders Plot filename: $vtkFreeSurferReaders(PlotFileName)"
    }
    # vtkFreeSurferReaders(gdfReader) SetHeaderFileName $vtkFreeSurferReaders(PlotFileName)
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersPlotApply
# Read in the plot specified. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersPlotApply {} {
    global vtkFreeSurferReaders
    if {!$::Module(verbose)} {
        DevInfoWindow "In development... probably not working"
    }
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersPlotApply: starting"
    }
    vtkFreeSurferReadersGDFInit
    if {$::Module(verbose)} {
        vtkFreeSurferReaders(gdfReader) DebugOn
    }
#    set fsgdfID [vtkFreeSurferReadersPlotParseHeader $vtkFreeSurferReaders(PlotFileName)]

    if {$::Module(verbose)} {
        vtkFreeSurferReaders(gdfReader) DebugOff
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersPlotCancel
# Cancel reading in the plot
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersPlotCancel {} {
    global vtkFreeSurferReaders
    if {$::Module(verbose)} {
       puts "vtkFreeSurferReadersPlotCancel: does nothing"
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersUncompressMGH 
# Uncompresses gzipped mgh files, which can be named .mgz or .mgh.gz.
# Will reset the vtkFreeSurferReaders(VolumeFileName), creating a new file (if possible) that's the uncompressed mgh volume.
# Returns -1 on failure.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersUncompressMGH {} {
    global vtkFreeSurferReaders Module

    # if this is the first time we've hit an zipped mgh file, set up the uncompression program (and the temp dir?)
    if {$vtkFreeSurferReaders(MGHDecompressorExec) == "" ||
        ![file executable $vtkFreeSurferReaders(MGHDecompressorExec)]} {
        set vtkFreeSurferReaders(MGHDecompressorExec) [tk_getOpenFile \
                                                           -initialdir $::env(SLICER_HOME) \
                                                           -parent .tMain \
                                                           -title "A file that can gunzip:"]
    }
    # if it's a .mgz file, copy it to .mgh.gz so that gunzip can process it, otherwise just make a copy to uncompress
    if {[string match *.mgz $vtkFreeSurferReaders(VolumeFileName)]} {
        set longFileName [file rootname $vtkFreeSurferReaders(VolumeFileName)].mgh.gz
        set retval [catch {file copy -force $vtkFreeSurferReaders(VolumeFileName) $longFileName} errmsg]
        if {$retval != 0} {
            DevErrorWindow "Error: Cannot copy $vtkFreeSurferReaders(VolumeFileName) to $longFileName for decompression:\n$errmsg"
            return -1
        } else {
            set vtkFreeSurferReaders(VolumeFileName) $longFileName
        }
    } else {
        # if it's just an mgh.gz file:
        # make a copy to work from, as when unzip it, it's going to replace itself
        set fileCopyName COPY${vtkFreeSurferReaders(VolumeFileName)}
        set reteval [catch {file copy -force $vtkFreeSurferReaders(VolumeFileName) $fileCopyName} errmsg]
        if {$retval != 0} {
            DevErrorWindow "Error: Cannot copy $vtkFreeSurferReaders(VolumeFileName) to $fileCopyName for decompression:\n$errmsg"
            return -1
        } else {
            set vtkFreeSurferReaders(VolumeFileName) $fileCopyName
        }
    }

    if {[file exist [file rootname $vtkFreeSurferReaders(VolumeFileName)]]} {
        puts "Warning: file [file rootname $vtkFreeSurferReaders(VolumeFileName)] already exists, using it instead of unzipping $vtkFreeSurferReaders(VolumeFileName)"

    } else {
        puts " -- calling $vtkFreeSurferReaders(MGHDecompressorExec) $vtkFreeSurferReaders(VolumeFileName)"
        exec $vtkFreeSurferReaders(MGHDecompressorExec) $vtkFreeSurferReaders(VolumeFileName)
    }

    # take the .gz off the volume file name
    set vtkFreeSurferReaders(VolumeFileName) [file rootname $vtkFreeSurferReaders(VolumeFileName)]
    puts " -- reset the volume file name to the uncompressed version"

    return 0
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetQADirName
# Set the directory name that contains a series of subjects, then populates a frame with the subject names for selection.
# .ARGS
# startdir a default directory to start looking from
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetQADirName { { startdir $::env(SLICER_HOME) } } {
    global vtkFreeSurferReaders Module

    set vtkFreeSurferReaders(QADirName) [tk_chooseDirectory \
                                             -initialdir $startdir \
                                             -mustexist true \
                                             -title "Select Directory Containing Subject Files" \
                                             -parent .tMain ]
    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders QA directory name: $vtkFreeSurferReaders(QADirName)"
    }

    # got a valid directory, so let's not use the subjects file
    set vtkFreeSurferReaders(QAUseSubjectsFile) 0

    # pick up subject dirs from this directory and put them in the list box
    vtkFreeSurferReadersSetQASubjects
    vtkFreeSurferReadersQAResetSubjectsListBox
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersSetQASubjectsFileName
# The filename is set elsewhere, in variable vtkFreeSurferReaders(QASubjectsFileName)
# Set the flag to use it or not, vtkFreeSurferReaders(QAUseSubjectsFile) 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersSetQASubjectsFileName {} {
    global vtkFreeSurferReaders Module

    if {$Module(verbose) == 1} {
        puts "FreeSurferReaders QA Subjects filename: $vtkFreeSurferReaders(QASubjectsFileName)"
    }
    if {$vtkFreeSurferReaders(QASubjectsFileName) != ""} {
        set vtkFreeSurferReaders(QAUseSubjectsFile) 1
    } else {
        set vtkFreeSurferReaders(QAUseSubjectsFile) 0
    } 
}


proc vtkFreeSurferReadersSetQASubjects {} {
    global vtkFreeSurferReaders Module

    set dirs ""
    set subjectnames ""

    # do we want to read from the subjects file or from the directory?
    if {!$vtkFreeSurferReaders(QAUseSubjectsFile)} {
        set dir $vtkFreeSurferReaders(QADirName)
        if { $dir != "" } {
            set files [glob -nocomplain $dir/*]
            
            foreach f $files {
                if { [file isdirectory $f] &&
                     [file exists [file join $f mri]]} {
                    lappend dirs $f
                    lappend subjectnames [file tail $f]
                } 
                if {$::Module(verbose)} {
                    if {[file isdirectory $f] && ![file exists [file join $f mri]]} {
                        puts "Skipping subject dir $f, no mri subdirectory"
                    }
                }
            }
            
        }
    } else {
        # parse the names from the subject file
        
        # open the file
        if {[catch {set qasubfid [open $vtkFreeSurferReaders(QASubjectsFileName) r]} errmsg] == 1} {
            puts "ERROR opening $vtkFreeSurferReaders(QASubjectsFileName):\n$errmsg"
            DevErrorWindow "ERROR opening $vtkFreeSurferReaders(QASubjectsFileName):\n$errmsg"
            return
        }

        set subjectsdirstr ""
        # run through it looking for the setenv SUBJECT_DIR and set SUBJECTS lines
        while {![eof $qasubfid]} {
            set line [gets $qasubfid]
            # look at the start of the line so that any commented out ones don't get caught
            if {[regexp "^setenv SUBJECTS_DIR (.*)" $line matchVar subjectsdirstr] == 1} {
                if {$::Module(verbose)} { puts "Got subjects directory $subjectsdirstr"}
            }
            if {[regexp "^set SUBJECTS = (.*)" $line matchVar subjectstr] == 1} {
                if {$::Module(verbose)} { puts "Got subjects $subjectstr"}
                # take the brackets off and convert to a list, the subject names are space separated
                set subjectstrtrim [string trim $subjectstr {( )}]
                set subjectnames [split $subjectstrtrim]
            }
        }
        # close the file
        close $qasubfid
        # do a sanity check that the subjects dir contains the subjects
        # if yes, reset the global var
        # if not, put up a warning to reset the var or something... set from script location?
        if {$subjectsdirstr != ""} {
            set missingdirs ""
            foreach s $subjectnames {
                if {![file isdirectory [file join $subjectsdirstr $s]]} {
                    lappend missingdirs $s
                }
            }
            if {$missingdirs != ""} {
                DevErrorWindow "Subjects file $vtkFreeSurferReaders(QASubjectsFileName) specifies invalid subjects directory $subjectsdirstr : missing subjects:\n $missingdirs\nTherefore, not resetting the subjects directory, fix the .csh file or set the subject dir explicity and select subjects interactively"
            } else {
                # use the subjects dir from the file
                if {$::Module(verbose)} {
                    DevInfoWindow "Using the subjects dir from the subjects file:\n$subjectsdirstr"
                }
                set vtkFreeSurferReaders(QADirName) $subjectsdirstr
            }
        }
    }
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersSetQASubjects: dirs =\n$dirs\nnames = \n$subjectnames"
    }
    # sort them alpah
    set vtkFreeSurferReaders(QASubjectNames) [lsort $subjectnames]
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersLoadQA
# Called when reading in a subjects.csh file from the command line
# .ARGS
# fname the name of the subjects.csh file, full path to it
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersLoadQA { fname } {
     global vtkFreeSurferReaders

    # set the file name
    set vtkFreeSurferReaders(QASubjectsFileName) $fname
    vtkFreeSurferReadersSetQASubjectsFileName

    # read the subject
    vtkFreeSurferReadersSetQASubjects

    # set up the list box
    vtkFreeSurferReadersQAResetSubjectsListBox

    # tab to the Freesurfers window, QA tab
    Tab vtkFreeSurferReaders row1 QA
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersQASetLoad
# Add this kind of volume to the list of volumes to load when doing a QA process.
# .ARGS
# voltype - the kind of volume file to read in, added to vtkFreeSurferReaders(QAVolFiles)
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersQASetLoad {voltype} {
    global vtkFreeSurferReaders
 
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQASetLoad: start: loading: $vtkFreeSurferReaders(QAVolFiles)"
    }
    set ind [lsearch $vtkFreeSurferReaders(QAVolFiles) $voltype]
    if {$vtkFreeSurferReaders(QAVolFiles,$voltype) == 1} {
        if {$ind == -1} {
            # add it to the list
            # if it's aseg, add it to the front
            if {$voltype == "aseg"} {
                set vtkFreeSurferReaders(QAVolFiles) [linsert $vtkFreeSurferReaders(QAVolFiles) 0 $voltype]
                if {$::Module(verbose)} { puts "added aseg to the beginning $vtkFreeSurferReaders(QAVolFiles)"}
            } else {
                lappend vtkFreeSurferReaders(QAVolFiles) $voltype
            }
        }
    } else {
        if {$ind != -1} {
            # remove it from the list
            set vtkFreeSurferReaders(QAVolFiles) [lreplace $vtkFreeSurferReaders(QAVolFiles) $ind $ind]
        }
    }


    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQASetLoad: new: loading: $vtkFreeSurferReaders(QAVolFiles)"
    }
}

proc vtkFreeSurferReadersQASetLoadAddNew {} {
    global vtkFreeSurferReaders

    # if the QAVolTypeNew isn't empty, and not on the list of types to check for, add it
    if {$vtkFreeSurferReaders(QAVolTypeNew) != ""} {
        # has more than one been added?
        foreach newvol $vtkFreeSurferReaders(QAVolTypeNew) {
            if {[lsearch $vtkFreeSurferReaders(QAVolFiles) $newvol] == -1} {
                # if it's a new aseg, add it to the front
                if {[regexp "^aseg.*" $newvol matchVar] == 1} {
                    set vtkFreeSurferReaders(QAVolFiles) [linsert $vtkFreeSurferReaders(QAVolFiles) 0 $newvol]
                } else {
                    # put it at the end
                    lappend vtkFreeSurferReaders(QAVolFiles) $newvol
                }
            }
        }
    }
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQASetLoadAddNew: now loading $vtkFreeSurferReaders(QAVolFiles)"
    }
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersStartQA
# This will set up the interface for the first subject. Once it's done, the pop up
# panel button push will launch the next subject
# .ARGS
# 
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersStartQA {} {
    global vtkFreeSurferReaders Module

    # get the selected subjects from the list box
    set vtkFreeSurferReaders(QASubjects) ""
    foreach ind [$vtkFreeSurferReaders(qaSubjectsListbox) curselection] {
        lappend vtkFreeSurferReaders(QASubjects) [$vtkFreeSurferReaders(qaSubjectsListbox) get $ind]
    }
    
    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersStartQA"
        puts "\t subjects dir =  $vtkFreeSurferReaders(QADirName)"
        puts "\t subjects to QA = $vtkFreeSurferReaders(QASubjects)"
        puts "\t vols to load = $vtkFreeSurferReaders(QAVolFiles)"
    }

    # close out anything that's open
    puts "Closing all volumes to start QA"
    MainFileClose

    # turn off casting to short, as we're just viewing
    set vtkFreeSurferReaders(QAcast) $vtkFreeSurferReaders(castToShort)
    if {$vtkFreeSurferReaders(QAEdit)} {
         set vtkFreeSurferReaders(castToShort) 1
    } else {
        set vtkFreeSurferReaders(castToShort) 0
    }

    # set the foreground opacity level 
    set vtkFreeSurferReaders(QAopacity) $::Slice(opacity)
    set ::Slice(opacity) $vtkFreeSurferReaders(QAOpacity)
    MainSlicesSetOpacityAll

    # turn it into a better viewing set up
    set vtkFreeSurferReaders(QAviewmode) $::View(mode)
    MainViewerSetMode $vtkFreeSurferReaders(QADefaultView)
    RenderAll

    set islabelmap 0

    # clear out the global message
    set vtkFreeSurferReaders(QAmsg) ""

    # reset the scanning time?

    # this call will set up with the next subject
    vtkFreeSurferReadersQAReviewSubject [lindex  $vtkFreeSurferReaders(QASubjects) 0]    
}

proc vtkFreeSurferReadersPauseQA {} {
    global vtkFreeSurferReaders Module

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersPauseQA"
    }
    DevInfoWindow "Not Implemented"

    # stop scanning
    set vtkFreeSurferReaders(scan) 0
}

proc vtkFreeSurferReadersContinueQA {} {
    global vtkFreeSurferReaders Module

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersContinueQA"
    }
    DevInfoWindow "Not Implemented"
    
    # continue scanning
    set vtkFreeSurferReaders(scan) 1
}

proc vtkFreeSurferReadersQAResetSubjectsListBox {} {
    global vtkFreeSurferReaders

    if {[info exist vtkFreeSurferReaders(QASubjectNames)] && $vtkFreeSurferReaders(QASubjectNames) != "" && [info exist vtkFreeSurferReaders(qaSubjectsListbox)]} {
        if {$::Module(verbose)} { puts "vtkFreeSurferReadersQAResetSubjectsListBox: Using already set QASubjectNames" }
        # clear out the old stuff
        $vtkFreeSurferReaders(qaSubjectsListbox) delete 0 end
        # use for instead of foreach to preserve ordering
        for {set i 0} {$i < [llength $vtkFreeSurferReaders(QASubjectNames)]} {incr i} {
            set sub [lindex $vtkFreeSurferReaders(QASubjectNames) $i]
            $vtkFreeSurferReaders(qaSubjectsListbox) insert end $sub
            # for now make them all active as add them
            $vtkFreeSurferReaders(qaSubjectsListbox) selection set end end
        }
    }
}


proc vtkFreeSurferReadersScan { slice } {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} { puts "Scan $slice offset = $::Slice(${slice},offset)" }

    if {$vtkFreeSurferReaders(scan) == "1"} {
        if {$::Slice(${slice},offset) >= 128} {
            # stop the loop
            set vtkFreeSurferReaders(scan) 0
            puts "Done scanning through slice $slice"
            # ooh, a hack, start the second scan here
            if {$slice == 2} {
                # then in saggital mode
                MainViewerSetMode "Single512SAG"                   
                MainSlicesSetOffset 1 $vtkFreeSurferReaders(scanStartSAG)
                RenderBoth 1
                set vtkFreeSurferReaders(scan) 1
                vtkFreeSurferReadersScan 1
            } 
        } else {
            MainSlicesSetOffset $slice [expr $::Slice(${slice},offset) + $vtkFreeSurferReaders(scanStep)]
            RenderSlice $slice
            update idletasks
            after $vtkFreeSurferReaders(scanMs) "vtkFreeSurferReadersScan $slice"
        }
    }
}

proc vtkFreeSurferReadersSetQAEdit {} {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} {
        puts $vtkFreeSurferReaders(QAEdit)
    }
}

proc vtkFreeSurferReadersBuildQAInteractor { subject vol } {
    global vtkFreeSurferReaders Gui

    if {$subject == "" || $vol == ""} {
        puts "No subject or volume..."
        return
    }
    if {$::Module(verbose)} {
        puts "Building interactor window for subject $subject and vol $vol"
    }

    if {[info command .top${subject}${vol}] !=  ""} {
        if {$::Module(verbose)} {
            puts "Already have a .top${subject}${vol}"
        }
        wm deiconify .top${subject}${vol}
        return
    }

    toplevel .top${subject}${vol}
    wm geometry .top${subject}${vol} +[winfo x .tMain]+10

    frame .top${subject}${vol}.f1 -bg $Gui(activeWorkspace)
    pack .top${subject}${vol}.f1  -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    set f .top${subject}${vol}.f1

    eval {label $f.lSubject -text "Subject = $subject"} $Gui(WLA)
    pack $f.lSubject

    eval {label $f.lVol -text "Volume = $vol"} $Gui(WLA)
    pack $f.lVol

    frame $f.fNotes -bg $Gui(activeWorkspace)
    frame $f.fEval -bg $Gui(activeWorkspace)

    pack $f.fNotes -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    pack $f.fEval -side top -padx $Gui(pad) -pady $Gui(pad) -fill x


    set f .top${subject}${vol}.f1.fNotes
    eval {label $f.lNotes -text "Notes:"} $Gui(WLA)
    eval {entry $f.eNotes -textvariable vtkFreeSurferReaders($subject,$vol,Notes) -width 50} $Gui(WEA)
    pack $f.lNotes -side left -padx 0
    pack $f.eNotes -side left -padx $Gui(pad) -expand 1 -fill x

    set f .top${subject}${vol}.f1.fEval
    foreach eval $vtkFreeSurferReaders(QAResultsList) {
        DevAddButton $f.b$eval $eval "vtkFreeSurferReadersRecordSubjectQA $subject $vol $eval"
        pack $f.b$eval -side left -padx $Gui(pad) -expand 1
    }
}

# this provides the button that will go onto the next subject
proc vtkFreeSurferReadersBuildQAInteractorNextSubject { subject } {
    global vtkFreeSurferReaders Gui

    if {$::Module(verbose)} {
        puts "Building next subject interactor window for subject $subject"
    }

    if {[info command .top${subject}] !=  ""} {
        puts "Already have a .top${subject}"
        wm deiconify .top${subject}
        return
    }
    toplevel .top${subject}
    wm geometry .top${subject} +[winfo x .tMain]+50

    frame .top${subject}.f1 -bg $Gui(activeWorkspace)
    pack .top${subject}.f1  -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    set f .top${subject}.f1

    eval {label $f.lSubject -text "Subject = $subject"} $Gui(WLA)
    pack $f.lSubject

    eval {label $f.lVol -text "Volumes = $vtkFreeSurferReaders(QAVolFiles)"} $Gui(WLA)
    pack $f.lVol

    frame $f.fNext -bg $Gui(activeWorkspace)
    pack $f.fNext -side top -padx $Gui(pad) -pady $Gui(pad) -fill x


    set f .top${subject}.f1.fNext
    # when you press this, it gets the next subject in the list, and starts the review process for them
    DevAddButton $f.bNext "Click for Next Subject" "vtkFreeSurferReadersReviewNextSubject $subject"
    pack $f.bNext -side left -padx $Gui(pad) -expand 1
}

proc vtkFreeSurferReadersRecordSubjectQA { subject vol eval } {
    global vtkFreeSurferReaders 

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersRecordSubjectQA subject = $subject, eval = $eval, vol = $vol"
    }

    # open the file for appending
    set fname [file join $vtkFreeSurferReaders(QADirName) $subject $vtkFreeSurferReaders(QASubjectFileName)]
    if {$::Module(verbose)} { puts "vtkFreeSurferReadersRecordSubjectQA fname = $fname" }

    if {[catch {set fid [open $fname "a"]} errmsg] == 1} {
        DevErrorWindow "Cannot open subject's file for appending this QA run:\nfilename = $fname\n$errMsg"
        return 
    }
    
    # write it out
    set msg "[clock format [clock seconds] -format "%D-%T-%Z"] $::env(USER) Slicer-$::SLICER(version) \"[ParseCVSInfo FreeSurferQA {$Revision: 1.17 $}]\" $::tcl_platform(machine) $::tcl_platform(os) $::tcl_platform(osVersion) $vol $eval \"$vtkFreeSurferReaders($subject,$vol,Notes)\""
    puts $fid $msg
    close $fid

    # now close down the window that called me
    if {$::Module(verbose)} {
        puts "Closing .top${subject}${vol}"
    }
    wm withdraw .top${subject}${vol}

    # and add to the global qa run's message
    append vtkFreeSurferReaders(QAmsg) "\n$msg"
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersResetTimeScale
#
#  When the starting slice or the slice step change, reset the timescale slider.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersResetTimeScale { } {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersResetTimeScale: startCOR: $vtkFreeSurferReaders(scanStartCOR), startSAG: $vtkFreeSurferReaders(scanStartSAG), step: $vtkFreeSurferReaders(scanStep), slice cor range high =  [Slicer GetOffsetRangeHigh 2], slice sag range high = [Slicer GetOffsetRangeHigh 1]"
    }
    # break up the scanning into cor scanning and sag scanning
    # cor goes from the high offset on slice 2 to the start value
    set corsteps [expr ( [Slicer GetOffsetRangeHigh 2] -  $vtkFreeSurferReaders(scanStartCOR) ) / $vtkFreeSurferReaders(scanStep)]
    # sag goes from the high offset on slice 1 to the start value
    set sagsteps [expr ( [Slicer GetOffsetRangeHigh 1] - $vtkFreeSurferReaders(scanStartSAG) ) / $vtkFreeSurferReaders(scanStep)]

    # then reset the slider to go from 0 to (corsteps + sagsteps) - 1
    if {[$::Module(vtkFreeSurferReaders,fQA).fBtns.sTime cget -to] != [expr $corsteps + $sagsteps - 1]} {
        $::Module(vtkFreeSurferReaders,fQA).fBtns.sTime configure -to [expr $corsteps + $sagsteps - 1]
        if {$::Module(verbose)} {
            puts "\n\n\treset time slider's to value to [expr $corsteps + $sagsteps - 1]"
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersQAResetTaskDelay
#
#  When the scan pause changes, reset the task delay on the Start button.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersQAResetTaskDelay { } {
    global vtkFreeSurferReaders Module

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQAResetTaskDelay: new delay: $vtkFreeSurferReaders(scanMs) ms"
    }
    if {[info command istask] != ""} {
        $::Module(vtkFreeSurferReaders,fQA).fBtns.play configure -taskdelay $vtkFreeSurferReaders(scanMs)
    }
}


#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersQASetTime
#
#  Show the slice and view for the current time
# .ARGS
# t the time step
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersQASetTime { {t ""} } {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} { puts "vtkFreeSurferReadersQASetTime t = $t, QAtime = $::vtkFreeSurferReaders(QAtime)" }

    # if time hasn't been changed, return
    if { $t != "" } {
        if { $t == $::vtkFreeSurferReaders(QAtime) } {
            return
        }
    }

    if {$t == "" } {
        set t $vtkFreeSurferReaders(QAtime)
    } else {
        set vtkFreeSurferReaders(QAtime) $t
    }

    # recalibrate the slider if necessary
    vtkFreeSurferReadersResetTimeScale

    # set the slider
    $vtkFreeSurferReaders(timescale) set $t

    # now display the right slice
    # break up the scanning into cor scanning and sag scanning
    # cor goes from the high offset on slice 2 to the start value
    set corsteps [expr ( [Slicer GetOffsetRangeHigh 2] -  $vtkFreeSurferReaders(scanStartCOR) ) / $vtkFreeSurferReaders(scanStep)]
    # sag goes from the high offset on slice 1 to the start value
    set sagsteps [expr ( [Slicer GetOffsetRangeHigh 1] - $vtkFreeSurferReaders(scanStartSAG) ) / $vtkFreeSurferReaders(scanStep)]

    # if the time is less than the corsteps, reset the COR slice
    if {$t < $corsteps} {
        if {$::Module(verbose)} { 
            puts "$t < $corsteps, doing cor" 
        }
        # check the view mode
        if {$::View(mode) != "Single512COR"} {
            MainViewerSetMode "Single512COR"
        }
        # take the starting slice and add the time mult by the scan step
        set corslice [expr $vtkFreeSurferReaders(scanStartCOR) + [expr $t * $vtkFreeSurferReaders(scanStep)]]

        # puts -nonewline "\t C${corslice}" 

        MainSlicesSetOffset 2 $corslice
        RenderSlice 2
    } else {
        if {$t == $corsteps} {
            # reset the slices on the cor and axi to be 0
            MainSlicesSetOffset 0 0
            MainSlicesSetOffset 2 0
            RenderSlice 0
            RenderSlice 2
        }
        # reset the SAG slice
        if {$::Module(verbose)} { 
            puts "$t >= $corsteps, doing sag" 
        }
        if {$::View(mode) != "Single512SAG"} {
            MainViewerSetMode "Single512SAG"
        }
        # take the starting slice and add t then take way the cor steps that have already been done
        set sagslice [expr $vtkFreeSurferReaders(scanStartSAG) + [expr ($t - $corsteps) * $vtkFreeSurferReaders(scanStep)]]
        # puts -nonewline "\tS${sagslice}" 

        MainSlicesSetOffset 1 $sagslice
        RenderSlice 1
    }
}

#-------------------------------------------------------------------------------
# .PROC vtkFreeSurferReadersStepFrame
#
#  adjust the frame according to current increment and handle boundaries
# .END
#-------------------------------------------------------------------------------
proc vtkFreeSurferReadersStepFrame {} {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersStepFrame"
    }
    set first [$::Module(vtkFreeSurferReaders,fQA).fBtns.sTime cget -from]
    set last [$::Module(vtkFreeSurferReaders,fQA).fBtns.sTime cget -to]
    # set inc $::vtkFreeSurferReaders(scanStep)
    # incrementing is handled in the qa set time function
    set inc 1
    set t $::vtkFreeSurferReaders(QAtime)

    set t [expr $t + $inc]

    if {$t > $last} {
        set t $last
        # reset the view mode to normal if not there already
        if {$::View(mode) != $vtkFreeSurferReaders(QADefaultView)} {
            MainViewerSetMode $vtkFreeSurferReaders(QADefaultView)
        }
    } 
    if {$t < $first} {
        set t $first 
    } 
    vtkFreeSurferReadersQASetTime $t
}

proc vtkFreeSurferReadersReviewNextSubject { subject } {
    global vtkFreeSurferReaders

    # turn off any istask
    $::Module(vtkFreeSurferReaders,fQA).fBtns.play off

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQAReviewNextSubject"
    }

    # if I'm the last one, just quit
    if {$subject == [lindex  $vtkFreeSurferReaders(QASubjects) end]} {
        puts "vtkFreeSurferReadersReviewNextSubject: No more subjects."
        return
    }

    # close the last ones
    MainFileClose

    # close the window that called me
    wm withdraw .top${subject}

    # find out where I am in the subjects list
    set myindex [lsearch  $vtkFreeSurferReaders(QASubjects)  $subject]

    # get the next one
    set nextindex [incr myindex]
    set nextsubject [lindex $vtkFreeSurferReaders(QASubjects) $nextindex]

    # review it
    vtkFreeSurferReadersQAReviewSubject $nextsubject
}

proc vtkFreeSurferReadersQAReviewSubject { subject } {
    global vtkFreeSurferReaders

    if {$::Module(verbose)} {
        puts "vtkFreeSurferReadersQAReviewSubject"
    }
    
    foreach vol $vtkFreeSurferReaders(QAVolFiles) {

        vtkFreeSurferReadersBuildQAInteractor $subject $vol

        set subfilename [file join $vtkFreeSurferReaders(QADirName) $subject mri $vol]
        set filetoload ""
        # check to see if there's an mgh file there
        set mghfilenames [glob -nocomplain ${subfilename}*.*]
        if {$mghfilenames != ""} {
            # figure out if have more than one
            if {$::Module(verbose)} { puts $mghfilenames }
            set filetoload [lindex $mghfilenames 0]
        } else {
            # otherwise check for a COR file in a subdir of that name
            if {[file isdirectory $subfilename]} {
                # check to see if there's a COR-.info file there
                set corfilename [file join $subfilename COR-.info]
                if {[file exist $corfilename]} {
                    set filetoload $corfilename
                }
            }
        }
        if {$filetoload != ""} {
            # load it up
            puts "Loading $filetoload"
            
            # if it's an aseg volume (could be an added one) it's a label map, so keep it 
            # loaded in the foreground as well
            if {$vol == "aseg" || [regexp "^aseg.*" $vol matchVar] == 1} {
                set islabelmap 1

                # load the colours
                if {$vtkFreeSurferReaders(coloursLoaded) != 1} {
                    set ::Color(fileName) $vtkFreeSurferReaders(colourFileName)
                    puts "Loading $::Color(fileName)"
                    ColorsLoadApply
                    set vtkFreeSurferReaders(coloursLoaded) 1
                }

                # vtkFreeSurferReadersLoadVolume $filetoload $islabelmap ${subject}-${vol}-lb
                # but, load it as a regular volume too
                # just load it as a label map and set both fore and label to it
                set volID [vtkFreeSurferReadersLoadVolume $filetoload $islabelmap ${subject}-${vol}]
                # and put it in the background instead so can see the colours
                if {$::Module(verbose)} { puts "Loaded aseg, got volume id $volID" }
                MainSlicesSetVolumeAll Fore $volID
                MainSlicesSetVolumeAll Label $volID
                RenderAll
            } else {
                set islabelmap 0
                set volID [vtkFreeSurferReadersLoadVolume $filetoload $islabelmap ${subject}-${vol}]
                # and put it in the foreground
                MainSlicesSetVolumeAll Back $volID
            }
            # make all slices visible
            foreach s {0 1 2} {
                set ::Slice($s,visibility) 1
                MainSlicesSetVisibility $s
            }
            # then update the viewer
            RenderAll
            
            # don't pause if this is the aseg and there are more to load
            if {$vol != "aseg" || [llength $vtkFreeSurferReaders(QAVolFiles)] == 1} {
                # reset the time scale

                # kick off the istask to cycle through the volumes
            }
        } else {
            puts "Can't find a file to load for $subject $vol, skipping"
        }
    }
    # done the volumes for this subject

   
    # scan through slices now
    set vtkFreeSurferReaders(QAtime) 0
    $::Module(vtkFreeSurferReaders,fQA).fBtns.play on
    vtkFreeSurferReadersStepFrame

    # is this the last subject?
    if {$subject == [lindex $vtkFreeSurferReaders(QASubjects) end]} {
        vtkFreeSurferReadersBuildQAInteractorStop
    } else {
        # make the interactor that allows you to go onto the next subject
        vtkFreeSurferReadersBuildQAInteractorNextSubject $subject
    }
}

# basically builds a button to stop everything
proc vtkFreeSurferReadersBuildQAInteractorStop {} {
    global vtkFreeSurferReaders Gui

    if {$::Module(verbose)} {
        puts "Building done QA interactor window"
    }
    if {[info command .topStopQA] !=  ""} {
        if {$::Module(verbose)} {
            puts "Already have a .topStopQA"
        }
        wm deiconify .topStopQA
        return
    }
    toplevel .topStopQA
    wm geometry .topStopQA +[winfo x .tMain]+0

    frame .topStopQA.f1 -bg $Gui(activeWorkspace)
    pack .topStopQA.f1  -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    set f .topStopQA.f1

    frame $f.fStop -bg $Gui(activeWorkspace)
    pack $f.fStop -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    set f .topStopQA.f1.fStop
    DevAddButton $f.bStop "Click to Stop QA" "vtkFreeSurferReadersQAStop"
    TooltipAdd $f.bStop "Writes results to a file. Turns off slice scanning, resets editing, view options.\nCloses all volumes."
    pack $f.bStop -side left -padx $Gui(pad) -expand 1

}

proc vtkFreeSurferReadersQAStop {} {
    global vtkFreeSurferReaders 

    # puts stuff back the way it was
    puts "Resetting everything to the way it was before we changed things for qa"

    # turn off istask that scans through slices
    $::Module(vtkFreeSurferReaders,fQA).fBtns.play off
    
    # reset the time
    set vtkFreeSurferReaders(QAtime) 0

    set vtkFreeSurferReaders(castToShort) $vtkFreeSurferReaders(QAcast)
    MainViewerSetMode $vtkFreeSurferReaders(QAviewmode)
    
    set ::Slice(opacity) $vtkFreeSurferReaders(QAopacity)
    MainSlicesSetOpacityAll
    
    # also write out the overall QA message to a file
    set fname [file join $vtkFreeSurferReaders(QADirName) QA-[clock format [clock seconds] -format "%Y-%m-%d-%T-%Z"].log]
    if {[catch {set fid [open $fname "w"]} errmsg] == 1} {
        DevErrorWindow "Cannot open file for writing about this QA run:\nfilename = $fname\n$errMsg"
        return 
    }
    puts $fid  $vtkFreeSurferReaders(QAmsg) 
    close $fid
    
    #        set closeup [tk_messageBox -type yesno -message "Do you want to close all subject volumes?"]
    #        if {$closeup == "yes"} {
    #            MainFileClose
    #        }
    # close the last subject
    MainFileClose

    # close the window that called us
    wm withdraw .topStopQA
}
