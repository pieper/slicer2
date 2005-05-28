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
# FILE:        DTMRI.tcl
# PROCEDURES:  
#   DTMRIInit
#   DTMRIUpdateMRML
#   DTMRIEnter
#   DTMRIExit
#   DTMRIBuildGUI
#   DTMRIRaiseMoreOptionsFrame mode
#   DTMRIBuildScrolledGUI f
#   DTMRICheckScrollLimits args
#   DTMRISizeScrolledGUI f
#   DTMRISetPropertyType
#   RunLSDIrecon
#   ShowPatternFrame
#   DTMRIDisplayScrollBar module tab
#   DTMRICreatePatternSlice
#   DTMRICreatePatternVolume
#   DTMRILoadPattern
#   DTMRIUpdateTipsPattern
#   DTMRIViewProps
#   DTMRIViewProps
#   DTMRIDisplayNewData
#   DTMRIPropsApply
#   DTMRIPropsCancel
#   DTMRIAdvancedApply
#   DTMRISetFileName
#   DTMRICreateBindings
#   DTMRIRoundFloatingPoint val
#   DTMRIRemoveAllActors
#   DTMRIAddAllActors
#   DTMRIUpdateReformatType
#   DTMRIUpdateScalarBar
#   DTMRIShowScalarBar
#   DTMRIHideScalarBar
#   DTMRIUpdateThreshold not_used
#   DTMRIUpdateMaskLabel
#   DTMRISpecificVisualizationSettings
#   DTMRIResetDefaultVisualizationSettings
#   DTMRIApplyVisualizationSettings mode
#   DTMRIUpdateGlyphResolution value
#   DTMRIUpdateGlyphEigenvector
#   DTMRIUpdateGlyphColor
#   DTMRIUpdateGlyphScalarRange not_used
#   DTMRISelectRemoveHyperStreamline x y z
#   DTMRISelectChooseHyperStreamline x y z
#   DTMRISelectStartHyperStreamline x y z render
#   DTMRIUpdateStreamlineSettings
#   DTMRIUpdateStreamlines
#   DTMRIUpdateTractingMethod TractingMethod
#   DTMRIUpdateBSplineOrder SplineOrder
#   DTMRIUpdateTractColorToSolid
#   DTMRIUpdateTractColorToMulti
#   DTMRIUpdateTractColor mode
#   DTMRIRemoveAllStreamlines
#   DTMRIAddAllStreamlines
#   DTMRIDeleteAllStreamlines
#   DTMRISeedStreamlinesFromSegmentation verbose
#   DTMRISeedStreamlinesEvenlyInMask verbose
#   DTMRISeedAndSaveStreamlinesFromSegmentation verbose
#   DTMRIFindStreamlinesThroughROI verbose
#   DTMRIUpdate
#   DTMRISetOperation math
#   DTMRIUpdateMathParams
#   DTMRICreateEmptyVolume OrigId Description VolName
#   DTMRIDoMath operation
#   DTMRIApplyVisualizationParameters
#   DTMRIDeleteVTKObject object
#   DTMRIMakeVTKObject class object
#   DTMRIAddObjectProperty object parameter value type desc
#   DTMRIBuildVTK
#   ConvertVolumeToTensors
#   DTMRICreateNewVolume volume name desc scanOrder
#   DTMRIWriteStructuredPoints filename
#   DTMRISaveStreamlinesAsIJKPoints verbose
#   DTMRISaveStreamlinesAsPolyLines subdir name verbose
#   DTMRISaveStreamlinesAsModel verbose
#   DTMRIGetScaledIjkCoordinatesFromWorldCoordinates x y z
#   DTMRICalculateActorMatrix transform t
#   DTMRICalculateIJKtoRASRotationMatrix transform t
#   DTMRI SetActive n
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC DTMRIInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIInit {} {
    global DTMRI Module Volume env
    
    set m DTMRI

    # Source all appropriate tcl files here. 
    #------------------------------------
    source "$env(SLICER_HOME)/Modules/vtkDTMRI/tcl/notebook.tcl"
    
    # List of all submodules (most are tcl files for tabs within this module)
    set DTMRI(submodulesList) "TensorRegistration ODF TractCluster Tractography Glyphs CalculateTensors CalculateScalars Mask"
    
    # Source and Init all submodules
    foreach mod $DTMRI(submodulesList) {
        source [file join $env(SLICER_HOME)/Modules/vtkDTMRI/tcl DTMRI${mod}.tcl]
        DTMRI${mod}Init
    }
    
     
    # Module Summary Info
    #------------------------------------
    set Module($m,overview) "Diffusion DTMRI MRI visualization and more..."
    set Module($m,author) "Lauren O'Donnell"
    set Module($m,category) "Visualisation"

    # version info
    lappend Module(versions) [ParseCVSInfo $m \
                  {$Revision: 1.76 $} {$Date: 2005/05/28 20:50:40 $}]

    # Define Tabs
    #------------------------------------
    set Module($m,row1List) "Help Input Convert Display ROI"
    set Module($m,row1Name) "{Help} {Input} {Convert} {Disp} {ROI}"
    set Module($m,row1,tab) Input
    # Use these lines to add a second row of tabs
    set Module($m,row2List) "Scalars Advanced Regist TC Save ODF"
    set Module($m,row2Name) "{Scalars} {Adv} {Reg} {TC} {Save} {ODF}"

    set Module($m,row2,tab) Scalars
    

    
    # Define Procedures
    #------------------------------------
    set Module($m,procGUI) DTMRIBuildGUI
    set Module($m,procMRML) DTMRIUpdateMRML
    set Module($m,procVTK) DTMRIBuildVTK
    set Module($m,procEnter) DTMRIEnter
    set Module($m,procExit) DTMRIExit
    
    # Define Dependencies
    #------------------------------------
    set Module($m,depend) ""
    
    # Create any specific bindings for this module
    #------------------------------------
    DTMRICreateBindings


    # Props: GUI tab we are currently on
    #------------------------------------
    set DTMRI(propertyType) Basic
    

    #------------------------------------
    # Developers panel variables
    #------------------------------------
    set DTMRI(devel,subdir) ""
    set DTMRI(devel,fileNamePoints) ""
    set DTMRI(devel,fileName) "DTMRIs.vtk"
    set tmp "\
            {1 0 0 0}  \
            {0 1 0 0}  \
            {0 0 1 0}  \
            {0 0 0 1}  "
    set rows {0 1 2 3}
    set cols {0 1 2 3}    
    foreach row $rows {
        foreach col $cols {
            set DTMRI(recalculate,userMatrix,$row,$col) \
                [lindex [lindex $tmp $row] $col]
        } 
    }

    #------------------------------------
    # Number display variables
    #------------------------------------
    # round floats to 8 digits
    set DTMRI(floatingPointFormat) "%0.5f"
    

    set DTMRI(Description) ""
    set DTMRI(Name) ""

}

################################################################
#  Procedures called automatically by the slicer
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMRML {} {
    global Tensor DTMRI

    set t $Tensor(activeID)
    
    if {$t != "" } {
        # in case transformation matrices have moved around
        # our tensor data, set up the tractography matrix again.
        # transform from World coords to scaledIJK of the tensors
        vtkTransform transform
        DTMRICalculateActorMatrix transform $t    
        transform Inverse
        DTMRI(vtk,streamlineControl) SetWorldToTensorScaledIJK transform
        transform Delete 
    }
    
     # Do MRML update for Tensor Registration tab. Necessary because
     # multiple lists are used.
     if {([catch "package require vtkAG"]==0)&&([info exist DTMRI(reg,AG)])} {
       # This is needed to handle deletion of tensors.
       if {[catch "Tensor($DTMRI(InputTensorSource),node) GetName"]==1} {
     set DTMRI(InputTensorSource) $Tensor(idNone)
         $DTMRI(mbInputTensorSource) config -text None
       }
       if {[catch "Tensor($DTMRI(InputTensorTarget),node) GetName"]==1} {
     set DTMRI(InputTensorTarget) $Tensor(idNone)
         $DTMRI(mbInputTensorTarget) config -text None
       }
       if {[catch "Tensor($DTMRI(ResultTensor),node) GetName"]==1} {
     set DTMRI(ResultTensor) -5
       }
       DevUpdateNodeSelectButton Tensor DTMRI InputTensorSource   InputTensorSource   DevSelectNode
       DevUpdateNodeSelectButton Tensor DTMRI InputTensorTarget   InputTensorTarget   DevSelectNode 0 0 0 DTMRIReg2DUpdate
       DevUpdateNodeSelectButton Tensor DTMRI ResultTensor  ResultTensor  DevSelectNode  0 1 0
       DevSelectNode Tensor $DTMRI(ResultTensor) DTMRI ResultTensor ResultTensor
       DevUpdateNodeSelectButton Volume DTMRI InputCoregVol InputCoregVol DevSelectNode
     }
     
     DevUpdateNodeSelectButton Volume DTMRI InputODF InputODF DevSelectNode
       
}

#-------------------------------------------------------------------------------
# .PROC DTMRIEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIEnter {} {
    global DTMRI Slice View
    
    # set global flag to avoid possible render loop
    set View(resetCameraClippingRange) 0

    # add event handling for slices
    EvActivateBindingSet DTMRISlice0Events
    EvActivateBindingSet DTMRISlice1Events
    EvActivateBindingSet DTMRISlice2Events

    # add event handling for 3D
    EvActivateBindingSet DTMRI3DEvents

    # configure all scrolled GUIs so frames inside fit
    DTMRISizeScrolledGUI $DTMRI(scrolledGui,advanced)

    # color label selection widgets
    LabelsColorWidgets


    # Default to reformatting along with the currently active slice
    set DTMRI(mode,reformatType) $Slice(activeID)

    Render3D

    #Update LMI logo
    set modulepath $::PACKAGE_DIR_VTKDTMRI/../../../images
    if {[file exist [ExpandPath [file join \
                     $modulepath "slicerLMIlogo.ppm"]]]} {
        image create photo iWelcome \
        -file [ExpandPath [file join $modulepath "slicerLMIlogo.ppm"]]
    }
}

#-------------------------------------------------------------------------------
# .PROC DTMRIExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIExit {} {
    global DTMRI View
    
    # unset global flag to avoid possible render loop
    set View(resetCameraClippingRange) 1

    # remove event handling for slices
    EvDeactivateBindingSet DTMRISlice0Events
    EvDeactivateBindingSet DTMRISlice1Events
    EvDeactivateBindingSet DTMRISlice2Events

    # remove event handling for 3D
    EvDeactivateBindingSet DTMRI3DEvents

    #Restore standar slicer logo
    image create photo iWelcome \
        -file [ExpandPath [file join gui "welcome.ppm"]]


}

################################################################
#  Procedures for building the GUI
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildGUI
# Builds the GUI panel.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildGUI {} {
    global Module Gui DTMRI Label Volume Tensor

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    #    Active
    #       VisMethods
    #          VisParams
    #             None
    #             Glyphs
    #             Tracts
    #             AutoTracts
    #             SaveTracts
    #       VisUpdate
    # ROI
    # Scalars
    # Props
    # Advanced
    # Diffuse
    #-------------------------------------------

    #puts "Lauren in DTMRIBuildGUI, fix the frame hierarchy comment"

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    This module allows visualization of DTMRI-valued data, 
especially Diffusion DTMRI MRI.
    <P>

    For <B>tractography</B>, point at the voxel of interest with the mouse and click\n the letter '<B>s</B>' (for start, or streamline). To <B>delete</B> a tract, point at it and click '<B>d</B>' (for delete).

    <P>
    Description by tab:
    <BR>
    <B>Disp (Visualization and Display Settings Tab)</B>
    <BR>
    <UL>
    <LI><B>3D View Settings:</B> click 'DTMRIs' view for transparent slices (this makes it easier to see 3D glyphs and tracts). 
    <LI><B>Display Glyphs:</B> turn glyphs on and off. Glyphs are little models for each DTMRI.  They display the eigensystem (principal directions of diffusion).
    <LI><B>Glyphs on Slice:</B> glyphs are displayed in the 3D view over this reformatted slice.  The slice-selection buttons are colored to match the colors of the three slice windows at the bottom of the Viewer window.
    <LI><B>Display Tracts:</B> turn display of tracts on and off, or delete all tracts.  Tracts are seeded when you point the mouse and hit the 's' key.  There are many more settings for tracts under the Visualization Menu below.
    <LI><B>Visualization Menu:</B> Settings for Tracts and Glyphs.
    </UL>


    <P>
    <B>Props Tab</B>
    <BR>
    <UL>
    <LI>This tab is for file reading/DTMRI conversion.
    </UL>
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags DTMRI $help
    MainHelpBuildGUI DTMRI



#############################################################################################
#############################################################################################
#############################################################################################

    #-------------------------------------------
    # Input frame
    #-------------------------------------------
 
    set fInput $Module(DTMRI,fInput)



    set f $fInput
    frame $f.fTitle -bg $Gui(backdrop)
    pack $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w


    Notebook:create $f.fNotebook \
                        -pages {{Option 1} {Option 2} {Option 3}} \
                        -pad 2 \
                        -bg $Gui(activeWorkspace) \
                        -height 260 \
                        -width 240
    pack $f.fNotebook -fill both -expand 1

    set f $fInput.fNotebook

    set FrameOption1 [Notebook:frame $f {Option 1}] 
    set FrameOption2 [Notebook:frame $f {Option 2}]
    set FrameOption3 [Notebook:frame $f {Option 3}]

#    foreach frame "$FrameOption1 $FrameOption2 $FrameOption3" {
#        frame $frame -bg $Gui(activeWorkspace)
#        pack $frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
#    }

    # $f.fTitle configure -bg $Gui(backdrop)
    foreach frame "$FrameOption1 $FrameOption2 $FrameOption3" {
        $frame configure  -relief groove -bd 3 
        foreach secframe "OptionNumber Input What How" {
        frame $frame.f$secframe -bg $Gui(activeWorkspace)
        pack $frame.f$secframe -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    }
    DevAddLabel $frame.fInput.l1 "The input is:"
    $frame.fInput.l1 configure -font {helvetica 7 normal}
    DevAddLabel $frame.fWhat.l1 "What to do:"
    $frame.fWhat.l1 configure -font {helvetica 7 normal}
    DevAddLabel $frame.fHow.l1 "How to:"
    $frame.fHow.l1 configure -font {helvetica 7 normal}

    foreach secframe "Input What How" {
        pack $frame.f$secframe.l1 -side top -anchor w -padx $Gui(pad) -pady 0
        }
    }



    #-------------------------------------------
    # Input->Title frame
    #-------------------------------------------
    set f $fInput.fTitle

    foreach frame "Wellcome Select" {
        frame $f.f$frame -bg $Gui(backdrop)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Input->Title frame->Wellcome
    #-------------------------------------------
    set f $fInput.fTitle.fWellcome
    
    DevAddLabel $f.lWellcome "Welcome to the DTMRI Module"
    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
    pack $f.lWellcome -side top -padx $Gui(pad) -pady $Gui(pad)

    DevAddLabel $f.lOption "Select Option"
    $f.lOption configure -fg White -font {helvetica 9 normal}  -bg $Gui(backdrop) -bd 0
    pack $f.lOption -side top -padx $Gui(pad) -pady 0


    #-------------------------------------------
    # Input->Option1 frame->OptionNumber
    #-------------------------------------------
    set f $FrameOption1.fOptionNumber

    DevAddLabel $f.lnumber "Option 1"
    $f.lnumber configure -font {helvetica 10 bold}
    pack $f.lnumber -side top -padx $Gui(pad) -pady $Gui(pad) -anchor w
    

    #-------------------------------------------
    # Input->Option1 frame->Input
    #-------------------------------------------
    set f $FrameOption1.fInput

    DevAddLabel $f.l2 "You have loaded LSDI gradient data\n from scanner ( I.* )"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w
    

    #-------------------------------------------
    # Input->Option1 frame->What
    #-------------------------------------------
    set f $FrameOption1.fWhat

    DevAddLabel $f.l2 "Run LSDI script"
    $f.l2 configure -font {helvetica 8 bold} -justify left
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w
    

    #-------------------------------------------
    # Input->Option1 frame->How
    #-------------------------------------------
    set f $FrameOption1.fHow

    frame $f.f1 -bg $Gui(activeWorkspace)
    pack $f.f1 -side left -padx 8 -pady 2 -anchor w

    # menu to select a volume: will set Volume(activeID)
    DevAddSelectButton  Volume $f.f1 Active "Select Input Volume:" left "Input Volume to Run LSDI Script to." 13 BLA
    $f.f1.lActive configure -bg $Gui(activeWorkspace) -fg black -font {helvetica 8 bold} -justify left
    $f.f1.mbActive configure -heigh 1 -bd 2 -pady 4 -bg $Gui(activeWorkspace)
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.f1.mbActive
    lappend Volume(mActiveList) $f.f1.mbActive.m
    
    frame $f.f2 -bg $Gui(activeWorkspace)
    pack $f.f2 -side left -padx 0 -pady 2 -anchor w

    label $f.f2.l -bg $Gui(activeWorkspace) -text and -font {helvetica 8 bold}
    pack $f.f2.l -side top -padx 0 -pady 2 -anchor n

    button $f.f2.bButton -bg $Gui(activeWorkspace) -text "Run Script" -font {helvetica 6 normal} -heigh 1 -command {
    RunLSDIrecon
        DTMRIDisplayNewData
    }
    pack $f.f2.bButton -side top -padx 0 -pady $Gui(pad) -anchor n -anchor s
    TooltipAdd $f.f2.bButton "Once Selected a Volume Data, \n Press this Button to Run Script.\n Notice that the Volume Data to convert \n must be in a separate directory \n with no other data."

    #-------------------------------------------
    # Input->Option2 frame->OptionNumber
    #-------------------------------------------
    set f $FrameOption2.fOptionNumber

    DevAddLabel $f.lnumber "Option 2"
    $f.lnumber configure -font {helvetica 10 bold}
    pack $f.lnumber -side top -padx $Gui(pad) -pady $Gui(pad) -anchor w
    

    #-------------------------------------------
    # Input->Option2 frame->Input
    #-------------------------------------------
    set f $FrameOption2.fInput

    DevAddLabel $f.l2 "You have loaded DTMRI gradient data\n or output data from LSDI scripts (D.*)\n This option for non LSDI data!"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w
    

    #-------------------------------------------
    # Input->Option2 frame->What
    #-------------------------------------------
    set f $FrameOption2.fWhat

    DevAddLabel $f.l2 "Calculate tensors from gradient data"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w
    

    #-------------------------------------------
    # Input->Option2 frame->How
    #-------------------------------------------
    set f $FrameOption2.fHow

    DevAddLabel $f.l2 "Press 'Convert' Tab above\n and follow instructions"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w

    #-------------------------------------------
    # Input->Option3 frame->OptionNumber
    #-------------------------------------------
    set f $FrameOption3.fOptionNumber

    DevAddLabel $f.lnumber "Option 3"
    $f.lnumber configure -font {helvetica 10 bold}
    pack $f.lnumber -side top -padx $Gui(pad) -pady $Gui(pad) -anchor w
    

    #-------------------------------------------
    # Input->Option3 frame->Input
    #-------------------------------------------
    set f $FrameOption3.fInput

    DevAddLabel $f.l2 "You have loaded a vtk tensor volume"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w
    

    #-------------------------------------------
    # Input->Option3 frame->What
    #-------------------------------------------
    set f $FrameOption3.fWhat

    DevAddLabel $f.l2 "The data does not need to be\n further converted. Ready to start\n visualizing and working"
    $f.l2 configure -font {helvetica 8 bold}
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w


    #-------------------------------------------
    # Input->Option3 frame->How
    #-------------------------------------------
    set f $FrameOption3.fHow

    DevAddLabel $f.l2 "Press 'Display',\n 'ROI' or 'Scalar' Tab"
    $f.l2 configure -font {helvetica 8 bold} -justify left
    pack $f.l2 -side top -padx 10 -pady 2 -anchor w






#############################################################################################
#############################################################################################
#############################################################################################




    
    #-------------------------------------------
    # Display frame
    #-------------------------------------------
    set fDisplay $Module(DTMRI,fDisplay)
    set f $fDisplay

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

#    frame $f.fGlyphsMode  -bg $Gui(activeWorkspace)
#    pack $f.fGlyphsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fNotebook  -bg $Gui(activeWorkspace)
    pack $f.fNotebook -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

#    frame $f.fTractsMode  -bg $Gui(activeWorkspace)
#    pack $f.fTractsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

#    frame $f.fVisMethods  -bg $Gui(activeWorkspace)
#    pack $f.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true
#    #$f.fVisMethods config -relief groove -bd 3
#    $f.fVisMethods config -relief sunken -bd 2

#    foreach frame "VisUpdate" {
#        frame $f.f$frame -bg $Gui(activeWorkspace)
#        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
#    }

    
    #-------------------------------------------
    # Display->Active frame
    #-------------------------------------------
    set f $fDisplay.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m


    #-------------------------------------------
    # Display->Notebook frame
    #-------------------------------------------
    set f $fDisplay.fNotebook


    Notebook:create $f.fNotebook \
                    -pages {{Display Glyphs} {Tractography}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 600 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1

    set f $f.fNotebook

    set Glyph [Notebook:frame $f {Display Glyphs}] 
    set Tract [Notebook:frame $f {Tractography}]

    frame $Glyph.fReformat  -bg $Gui(activeWorkspace)
    pack $Glyph.fReformat -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $Glyph.fGlyphsMode  -bg $Gui(activeWorkspace)
    pack $Glyph.fGlyphsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $Tract.fTractsMode  -bg $Gui(activeWorkspace)
    pack $Tract.fTractsMode -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $Glyph.fVisMethods  -bg $Gui(activeWorkspace) -relief sunken -bd 2
    pack $Glyph.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true

    frame $Tract.fVisMethods  -bg $Gui(activeWorkspace) -bd 2
    pack $Tract.fVisMethods -side top -padx $Gui(pad) -pady $Gui(pad) -fill both -expand true


    #-------------------------------------------
    # Display->Notebook -> Glyph frame -> Reformat
    #-------------------------------------------
    set f $Glyph.fReformat

    DevAddLabel $f.l "Glyphs on Slice:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    set colors [list  $Gui(slice0) $Gui(slice1) $Gui(slice2) $Gui(activeWorkspace) $Gui(activeWorkspace)]
    set widths [list  2 2 2 4 4]

    foreach vis $DTMRI(mode,reformatTypeList) \
    tip $DTMRI(mode,reformatTypeList,tooltips) \
    text $DTMRI(mode,reformatTypeList,text) \
    color $colors \
    width $widths {
        eval {radiobutton $f.rMode$vis \
              -text "$text" -value "$vis" \
              -variable DTMRI(mode,reformatType) \
              -command {DTMRIUpdateReformatType} \
              -indicatoron 0 } $Gui(WCA) \
        {-bg $color -selectcolor $color -width $width}
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }
    
    #-------------------------------------------
    # Display -> Notebook -> Glyph frame ->->GlyphsMode frame
    #-------------------------------------------
    set f $Glyph.fGlyphsMode

    eval {label $f.lVis -text "Display Glyphs: "} $Gui(WLA)
    pack $f.lVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationType,glyphsOnList) \
    tip $DTMRI(mode,visualizationType,glyphsOnList,tooltip) {
        eval {radiobutton $f.r$vis \
              -text $vis \
              -command "DTMRIUpdate" \
              -value $vis \
              -variable DTMRI(mode,visualizationType,glyphsOn) \
              -indicatoron 0} $Gui(WCA)

        pack $f.r$vis -side left -fill x
        TooltipAdd $f.r$vis $tip
    }

    #-------------------------------------------
    # Display-> Notebook -> Tract frame->TractsMode frame
    #-------------------------------------------
    set f $Tract.fTractsMode

    eval {label $f.lVis -text "Display 'Tracts': "} $Gui(WLA)
    pack $f.lVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationType,tractsOnList) \
    tip $DTMRI(mode,visualizationType,tractsOnList,tooltip) {
        eval {radiobutton $f.r$vis \
              -text $vis \
              -command "DTMRIUpdateStreamlines" \
              -value $vis \
              -variable DTMRI(mode,visualizationType,tractsOn) \
              -indicatoron 0} $Gui(WCA)

        pack $f.r$vis -side left -fill x
        TooltipAdd $f.r$vis $tip
    }

    #-------------------------------------------
    # Display-> Notebook -> Tract frame ->VisMethods frame
    #-------------------------------------------
    set f $Tract.fVisMethods

    frame $f.fVisMode -bg $Gui(activeWorkspace) 
    pack $f.fVisMode -side top -padx 0 -pady 0 -fill x

    # note the height is necessary to place frames inside later
    frame $f.fVisParams -bg $Gui(activeWorkspace) -height 500
    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    $f.fVisMode config -relief sunken -bd 2
    #$f.fVisParams config -relief groove -bd 3
    #$f.fVisParams config -bd 3

    #-----------------------------------------2--
    # Display-> Notebook ->Tract frame ->VisMethods ->VisMode frame
    #-------------------------------------------
    set f $Tract.fVisMethods.fVisMode

    eval {label $f.lVis -text "Visualization Menu: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,visualizationTypeGui) \
          -relief raised -bd 2 -width 10 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,visualizationTypeGuiList) {
        $f.mbVis.m add command -label $vis \
        -command "DTMRIRaiseMoreOptionsFrame $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbVisMode) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,visualizationTypeGuiList,tooltip)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame ->VisMethods ->VisParams frame
    #-------------------------------------------
    set f $Tract.fVisMethods.fVisParams
    set fParams $f

    # make a parameters frame for each visualization type
    # types are: Help  Tracts AutoTracts SaveTracts
    foreach frame $DTMRI(mode,visualizationTypeGuiList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
        set DTMRI(frame,$frame) $f.f$frame
    }
    raise $DTMRI(frame,$DTMRI(mode,visualizationTypeGui))

    ##########################################################
    #  HELP   (frame raised when Glyphs are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Help frame
    #-------------------------------------------
    set f $fParams.fHelp

    DevAddLabel $f.l "Select from the Visualization\n menu above to adjust\n visualization parameters."
    DevAddLabel $f.l2 "For tractography, point at \nthe voxel of interest with\n the mouse and click\n the letter 's'. To delete\n a tract, point and click 'd'."
    pack $f.l $f.l2 -side top -padx $Gui(pad) -pady $Gui(pad)

    ##########################################################
    #  GLYPHS   (frame raised when Glyphs are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs frame
    #-------------------------------------------
    frame $Glyph.fVisMethods.fGlyphs -bg $Gui(activeWorkspace)
    pack $Glyph.fVisMethods.fGlyphs -side top -padx 0 -pady $Gui(pad) -fill x

    set f $Glyph.fVisMethods.fGlyphs

    foreach frame "Resolution GlyphType Lines Colors ScalarBar GlyphScalarRange Slider" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->Resolution frame
    #-------------------------------------------
    set f $Glyph.fVisMethods.fGlyphs.fResolution
    
    eval {label $f.l -text "Density(H<->L):"\
          -width 12 -justify right } $Gui(WLA)

    eval {scale $f.s -from $DTMRI(mode,glyphResolution,min) \
                          -to $DTMRI(mode,glyphResolution,max)    \
          -variable  DTMRI(mode,glyphResolution)\
      -command DTMRIUpdateGlyphResolution \
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

      pack $f.l $f.s -side left -padx $Gui(pad) -pady 0


    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->GlyphType frame
    #-------------------------------------------
    set f $Glyph.fVisMethods.fGlyphs.fGlyphType

    DevAddLabel $f.l "Glyph Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    eval {menubutton $f.mbVis -text $DTMRI(mode,glyphType) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack  $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,glyphTypeList) {
        $f.mbVis.m add command -label $vis \
        -command "$f.mbVis config -text $vis; set DTMRI(mode,glyphType) $vis; DTMRIUpdate"
    }
    # save menubutton for config
    set DTMRI(gui,mbGlyphType) $f.mbVis
    # Add a tooltip
    #TooltipAdd $f.mbVis $DTMRI(mode,glyphColorList,tooltip)

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->Lines frame
    #-------------------------------------------

    set f $Glyph.fVisMethods.fGlyphs.fLines

    DevAddLabel $f.l "Line Type:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $DTMRI(mode,glyphEigenvectorList) tip $DTMRI(mode,glyphEigenvectorList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(mode,glyphEigenvector) \
          -command DTMRIUpdateGlyphEigenvector \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 1
        TooltipAdd $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->Colors frame
    #-------------------------------------------
    set f $Glyph.fVisMethods.fGlyphs.fColors

    eval {label $f.lVis -text "Color by: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,glyphColor) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,glyphColorList) {
        $f.mbVis.m add command -label $vis \
        -command "set DTMRI(mode,glyphColor) $vis; DTMRIUpdateGlyphColor"
    }
    # save menubutton for config
    set DTMRI(gui,mbGlyphColor) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,glyphColorList,tooltip)

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->ScalarBar frame
    #-------------------------------------------
    set f $Glyph.fVisMethods.fGlyphs.fScalarBar

    DevAddLabel $f.l "Scalar Bar:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $DTMRI(mode,scalarBarList) tip $DTMRI(mode,scalarBarList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(mode,scalarBar) \
          -command {DTMRIUpdateScalarBar} \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 1
        TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->GlyphScalarRange frame
    #-------------------------------------------
    set f $Glyph.fVisMethods.fGlyphs.fGlyphScalarRange

    DevAddLabel $f.l "Scalar Range:"
    pack $f.l -side left -padx $Gui(pad) -pady 1

    foreach vis $DTMRI(mode,glyphScalarRangeList) tip $DTMRI(mode,glyphScalarRangeList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(mode,glyphScalarRange) \
          -command {DTMRIUpdateGlyphScalarRange; Render3D} \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 1
        TooltipAdd  $f.rMode$vis $tip
    }

    #-------------------------------------------
    # Display-> Notebook ->Glyph frame->VisMethods->VisParams->Glyphs->Slider frame
    #-------------------------------------------
    foreach slider "Low Hi" text "Lo Hi" {

        set f $Glyph.fVisMethods.fGlyphs.fSlider

        frame $f.f$slider -bg $Gui(activeWorkspace)
        pack $f.f$slider -side top -padx $Gui(pad) -pady 1
        set f $f.f$slider

        eval {label $f.l$slider -text "$text:"} $Gui(WLA)
        eval {entry $f.e$slider -width 10 \
          -textvariable DTMRI(mode,glyphScalarRange,[Uncap $slider])} \
        $Gui(WEA)
        eval {scale $f.s$slider -from $DTMRI(mode,glyphScalarRange,min) \
          -to $DTMRI(mode,glyphScalarRange,max) \
          -length 90 \
          -variable DTMRI(mode,glyphScalarRange,[Uncap $slider]) \
          -resolution 0.1 \
          -command {DTMRIUpdateGlyphScalarRange; Render3D}} \
        $Gui(WSA) {-sliderlength 15}
        pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
        set DTMRI(gui,slider,$slider) $f.s$slider
        bind $f.e${slider} <Return>   \
        "DTMRIUpdateGlyphScalarRange ${slider}; Render3D"

    }

    ##########################################################
    #  TRACTS   (frame raised when Tracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts frame
    #-------------------------------------------
    set f $fParams.fTracts

    foreach frame "Colors Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors
    foreach frame "ChooseColor ColorBy ColorByVol" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ChooseColor frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fChooseColor

    DevAddButton $f.b "Color:" {ShowColors DTMRIUpdateTractColorToSolid}
    eval {entry $f.e -width 20 \
          -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    pack $f.b $f.e -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

    lappend Label(colorWidgetList) $f.e

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ColorBy frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fColorBy

    eval {label $f.lVis -text "Color by: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(mode,tractColor) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(mode,tractColorList) {
        $f.mbVis.m add command -label $vis \
        -command "set DTMRI(mode,tractColor) $vis; DTMRIUpdateTractColor"
    }
    # save menubutton for config
    set DTMRI(gui,mbTractColor) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(mode,tractColorList,tooltip)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Colors->ColorByVol frame
    #-------------------------------------------
    set f $fParams.fTracts.fColors.fColorByVol

    # menu to select a volume: will set Volume(activeID)
    set name ColorByVolume
    DevAddSelectButton  Volume $f $name "Color by Volume:" Pack \
    "First select Color by MultiColor, \nthen select the volume to use \nto color the tracts. \nFor example to color by FA, \ncreate the FA volume using the \n<More...> tab in this module, \nthen the <Scalars> tab.  \nThen select that volume from this list." \
    13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Entries frame
    #-------------------------------------------
    set f $fParams.fTracts.fEntries

    frame $f.fTractingMethod -bg $Gui(activeWorkspace) 
    pack $f.fTractingMethod -side top -padx 0 -pady 0 -fill x

    # note the height is necessary to place frames inside later
    frame $f.fTractingVar -bg $Gui(activeWorkspace) -height 500
    pack $f.fTractingVar -side top -padx 0 -pady $Gui(pad) -fill both -expand true

    #    frame $f -bg $Gui(activeWorkspace)
    #    pack $f -side top -padx $Gui(pad) -pady 1 -fill x

    set f $fParams.fTracts.fEntries.fTractingMethod

    eval {label $f.lVis -text "Tracting Method: "} $Gui(WLA)
    eval {menubutton $f.mbVis -text $DTMRI(stream,tractingMethod) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    foreach vis $DTMRI(stream,tractingMethodList) {
        $f.mbVis.m add command -label $vis \
        -command "DTMRIUpdateTractingMethod $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbTractingMethod) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,tractingMethodList,tooltip)

    set f $fParams.fTracts.fEntries.fTractingVar

    #    frame $f -bg $Gui(activeWorkspace)
    #place $f.fVar -in $f -relheight 1.0 -relwidth 1.0
    #    pack $f -side top -padx 0 -pady $Gui(pad) -fill both -expand true
    #    pack $f.fVisParams -side top -padx 0 -pady $Gui(pad) -fill both -expand true

    foreach frame $DTMRI(stream,tractingMethodList) {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        # for raising one frame at a time
        place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        #pack $f.f$frame -side top -padx 0 -pady 1 -fill x
        set DTMRI(stream,tractingFrame,$frame) $f.f$frame
    }
    raise $DTMRI(stream,tractingFrame,$DTMRI(stream,tractingMethod))
    
    set f $DTMRI(stream,tractingFrame,BSpline)
    
    frame $f.fBSplineOrder -bg $Gui(activeWorkspace) 
    pack $f.fBSplineOrder -side top -padx 0 -pady 0 -fill x

    set f $f.fBSplineOrder

    eval {label $f.lVis -text "Spline Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,BSplineOrder) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # Add menu items
    foreach vis $DTMRI(stream,BSplineOrderList) {
        $f.mbVis.m add command -label $vis \
        -command "DTMRIUpdateBSplineOrder $vis"
    }
    # save menubutton for config
    set DTMRI(gui,mbBSplineOrder) $f.mbVis
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    set f $DTMRI(stream,tractingFrame,BSpline)
    
    frame $f.fMethodOrder -bg $Gui(activeWorkspace) 
    pack $f.fMethodOrder -side top -padx 0 -pady 0 -fill x

    set f $f.fMethodOrder

    eval {label $f.lVis -text "Method Order: "} $Gui(WLA)

    eval {menubutton $f.mbVis -text $DTMRI(stream,MethodOrder) \
          -relief raised -bd 2 -width 12 \
          -menu $f.mbVis.m} $Gui(WMBA)
    eval {menu $f.mbVis.m} $Gui(WMA)
    pack $f.lVis $f.mbVis -side left -pady 1 -padx $Gui(pad)
    # save menubutton for config
    set DTMRI(gui,mbMethodOrder) $f.mbVis
    # Add menu items
    foreach vis $DTMRI(stream,MethodOrderList) {
        $f.mbVis.m add command -label $vis \
        -command "set DTMRI(vtk,ivps) DTMRI(vtk,$vis); $DTMRI(gui,mbMethodOrder) config -text $vis"
    }
    # Add a tooltip
    TooltipAdd $f.mbVis $DTMRI(stream,BSplineOrderList,tooltip)

    foreach entry $DTMRI(stream,methodvariableList) \
        text $DTMRI(stream,methodvariableList,text) \
        tip $DTMRI(stream,methodvariableList,tooltips) {
        
        set f $DTMRI(stream,tractingFrame,BSpline)
        
        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        pack $f.f$entry -side top -padx $Gui(pad) -pady 1 -fill x
        set f $f.f$entry

        eval {label $f.l$entry -text "$text:"} $Gui(WLA)
        eval {entry $f.e$entry -width 10 \
              -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
        TooltipAdd $f.l$entry $tip
        TooltipAdd $f.e$entry $tip
        pack $f.l$entry -side left  -padx $Gui(pad)
        pack $f.e$entry -side right  -padx $Gui(pad)
    }

    foreach entry $DTMRI(stream,variableList) \
    text $DTMRI(stream,variableList,text) \
    tip $DTMRI(stream,variableList,tooltips) {

        set f $DTMRI(stream,tractingFrame,NoSpline)

        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        pack $f.f$entry -side top -padx $Gui(pad) -pady 1 -fill x
        set f $f.f$entry

        eval {label $f.l$entry -text "$text:"} $Gui(WLA)
        eval {entry $f.e$entry -width 10 \
              -textvariable DTMRI(stream,$entry)} \
        $Gui(WEA)
        TooltipAdd $f.l$entry $tip
        TooltipAdd $f.e$entry $tip
        pack $f.l$entry -side left  -padx $Gui(pad)
        pack $f.e$entry -side right  -padx $Gui(pad)
    }

    foreach entry $DTMRI(stream,precisevariableList) \
        text $DTMRI(stream,precisevariableList,text) \
        tip $DTMRI(stream,precisevariableList,tooltips) {
        
        set f $DTMRI(stream,tractingFrame,BSpline)
        
        frame $f.f$entry -bg $Gui(activeWorkspace)
        #place $f.f$frame -in $f -relheight 1.0 -relwidth 1.0
        pack $f.f$entry -side top -padx $Gui(pad) -pady 1 -fill x
        set f $f.f$entry

        eval {label $f.l$entry -text "$text:"} $Gui(WLA)
        eval {entry $f.e$entry -width 10 \
              -textvariable DTMRI(stream,$entry)} \
                $Gui(WEA)
        TooltipAdd $f.l$entry $tip
        TooltipAdd $f.e$entry $tip
        pack $f.l$entry -side left  -padx $Gui(pad)
        pack $f.e$entry -side right  -padx $Gui(pad)
    }

    ##########################################################
    #  AUTOTRACTS   (frame raised when AutoTracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts frame
    #-------------------------------------------
    set f $fParams.fAutoTracts

    foreach frame "Label2 Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Autotracts->Label2 frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fLabel2

    DevAddLabel $f.l "Automatically start tractography\nfrom each voxel in an ROI."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)


    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->Tracts->Entries frame
    set f $fParams.fAutoTracts.fEntries
    foreach frame "Volume" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
    foreach frame "ChooseLabel FindTracts" {
        frame $f.f$frame -bg $Gui(activeWorkspace) -bd 2
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->Volume frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fVolume

    # menu to select a volume: will set Volume(activeID)
    set name MaskLabelmap
    DevAddSelectButton  Volume $f $name "ROI Labelmap:" Grid \
    "This labelmap will be used to seed tracts."\
    13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel
    
    foreach frame "Title Label Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
    
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Title frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fTitle
    
    DevAddLabel $f.l "Create Tracts from label value"
    pack $f.l -side top
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Label frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fLabel
    
    # mask label
    eval {button $f.bOutput -text "Label:" \
          -command "ShowLabels DTMRIUpdateMaskLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
          -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "DTMRIUpdateMaskLabel"
    bind $f.eOutput <FocusOut> "DTMRIUpdateMaskLabel"
    eval {entry $f.eName -width 14 \
          -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    TooltipAdd  $f.bOutput $DTMRI(mode,autoTractsLabel,tooltip)
    TooltipAdd  $f.eOutput $DTMRI(mode,autoTractsLabel,tooltip)
    TooltipAdd  $f.eName $DTMRI(mode,autoTractsLabel,tooltip)


    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->ChooseLabel->Apply frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fChooseLabel.fApply
    DevAddButton $f.bApply "Seed 'Tracts' in ROI" \
        {puts "Seeding streamlines"; DTMRISeedStreamlinesFromSegmentation}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Seed a 'tract' from each point in the ROI.\nThis can be slow; be patient."


    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts
    
    foreach frame "Title ListLabels Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }
   
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->Title frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fTitle
    
    DevAddLabel $f.l "Choose Tracts that pass through\na set of labels"
    pack $f.l -side top
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->ListLabels frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fListLabels
    
    DevAddLabel $f.l "List of labels:"
    
    eval {entry $f.eName -width 25 \
          -textvariable DTMRI(stream,ListLabels)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace)}
    
    pack $f.l $f.eName -side left
    
    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->AutoTracts->Entries->FindTracts->Apply frame
    #-------------------------------------------
    set f $fParams.fAutoTracts.fEntries.fFindTracts.fApply
    
    DevAddButton $f.bApply "Find 'Tracts' through ROI" \
        {DTMRIFindStreamlinesThroughROI}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)


    ##########################################################
    #  SAVETRACTS   (frame raised when SaveTracts are selected)
    ##########################################################

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts frame
    #-------------------------------------------
    set f $fParams.fSaveTracts

    foreach frame "Entries" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady 1 -fill x
    }

    #-------------------------------------------
    # Display--> Notebook ->Tract frame>VisMethods->VisParams->SaveTracts->Entries frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries
    foreach frame "Info1 Apply1 Info2 Apply2" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Info frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fInfo1
    DevAddLabel $f.l "Save the tracts you have created."
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fApply1
    DevAddButton $f.bApply "Save tracts in model file(s)" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsModel}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save tracts to vtk file(s).\nEach color of tract will become a separate model.\n Choose the initial part of the filename, and models\nwill be saved as filename_0.vtk, filename_1.vtk, etc.\nThen you can load the models into slicer\n(they must be re-added to the mrml tree)."

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Info frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fInfo2
    DevAddLabel $f.l "Save currently visible tracts as a polyline.\n Useful for further processing on tracts.\nONLY DEVELOPERS"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisMethods->VisParams->SaveTracts->Entries->Apply frame
    #-------------------------------------------
    set f $fParams.fSaveTracts.fEntries.fApply2
    DevAddButton $f.bApply "Save tracts in vtk file" \
        {puts "Saving streamlines"; DTMRISaveStreamlinesAsPolyLines "" tracts}
    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd  $f.bApply "Save visible tracts to vtk file as a set of polylines."

    #-------------------------------------------
    # Display-> Notebook ->Tract frame->VisUpdate frame
    #-------------------------------------------
    #set f $fDisplay.fVisUpdate
    #DevAddButton $f.bTest "Junk" {puts "this button is junk"} 4    
    #pack $f.bTest -side top -padx 0 -pady 0



#############################################################################################
#############################################################################################
#############################################################################################


    #-------------------------------------------
    # ROI frame
    #-------------------------------------------
    set fROI $Module(DTMRI,fROI)
    set f $fROI

    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "Threshold Mask" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
        $f.f$frame config -relief groove -bd 3
    }

    #-------------------------------------------
    # ROI->Active frame
    #-------------------------------------------
    set f $fROI.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # ROI->Threshold frame
    #-------------------------------------------
    set f $fROI.fThreshold

    foreach frame "Label Mode Slider" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # ROI->Threshold->Label frame
    #-------------------------------------------
    set f $fROI.fThreshold.fLabel

    DevAddLabel $f.l "DTMRI Threshold Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Threshold->Mode frame
    #-------------------------------------------
    set f $fROI.fThreshold.fMode

    DevAddLabel $f.l "Value:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(mode,thresholdList) tip $DTMRI(mode,thresholdList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(mode,threshold) \
          -command {DTMRIUpdate} \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # ROI->Threshold->Slider frame
    #-------------------------------------------
    foreach slider "Lower Upper" text "Lo Hi" {

        set f $fROI.fThreshold.fSlider

        frame $f.f$slider -bg $Gui(activeWorkspace)
        pack $f.f$slider -side top -padx $Gui(pad) -pady $Gui(pad)
        set f $f.f$slider

        eval {label $f.l$slider -text "$text:"} $Gui(WLA)
        eval {entry $f.e$slider -width 6 \
          -textvariable DTMRI(thresh,threshold,[Uncap $slider])} \
        $Gui(WEA)
        bind $f.e$slider <Return>   "DTMRIUpdateThreshold"
        #bind $f.e$slider <FocusOut> "EdThresholdUpdate; RenderActive;"
        eval {scale $f.s$slider -from $DTMRI(thresh,threshold,rangeLow) \
          -to $DTMRI(thresh,threshold,rangeHigh) \
          -length 130 \
          -variable DTMRI(thresh,threshold,[Uncap $slider]) \
          -resolution 0.1 \
          -command {DTMRIUpdateThreshold}} \
        $Gui(WSA) {-sliderlength 15}
        #grid $f.l$slider $f.e$slider -padx 2 -pady 2 -sticky w
        #grid $f.l$slider -sticky e
        #grid $f.s$slider -columnspan 2 -pady 2 
        pack $f.l$slider $f.e$slider $f.s$slider -side left  -padx $Gui(pad)
        set DTMRI(gui,slider,$slider) $f.s$slider
    }


    #-------------------------------------------
    # ROI->Mask frame
    #-------------------------------------------
    set f $fROI.fMask

    foreach frame "Label Mode Volume ChooseLabel" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
    }

    #-------------------------------------------
    # ROI->Mask->Label frame
    #-------------------------------------------
    set f $fROI.fMask.fLabel

    DevAddLabel $f.l "DTMRI Mask Settings"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # ROI->Mask->Mode frame
    #-------------------------------------------
    set f $fROI.fMask.fMode

    DevAddLabel $f.l "Mask:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(mode,maskList) tip $DTMRI(mode,maskList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(mode,mask) \
          -command {DTMRIUpdate} \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # ROI->Mask->Volume frame
    #-------------------------------------------
    set f $fROI.fMask.fVolume

    # menu to select a volume: will set Volume(activeID)
    set name MaskLabelmap
    DevAddSelectButton  Volume $f $name "Mask Labelmap:" Grid \
        "Select a labelmap volume to use as a mask.\nDTMRIs will be displayed only where the label matches the label you select below." \
        13
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mb$name
    lappend Volume(mActiveList) $f.mb$name.m

    #-------------------------------------------
    # ROI->Mask->ChooseLabel frame
    #-------------------------------------------
    set f $fROI.fMask.fChooseLabel

    # mask label
    eval {button $f.bOutput -text "Label:" \
          -command "ShowLabels DTMRIUpdateMaskLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
          -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "DTMRIUpdateMaskLabel"
    bind $f.eOutput <FocusOut> "DTMRIUpdateMaskLabel"
    eval {entry $f.eName -width 14 \
          -textvariable Label(name)} $Gui(WEA) \
            {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    
    lappend Label(colorWidgetList) $f.eName

    TooltipAdd  $f.bOutput $DTMRI(mode,maskLabel,tooltip)
    TooltipAdd  $f.eOutput $DTMRI(mode,maskLabel,tooltip)
    TooltipAdd  $f.eName $DTMRI(mode,maskLabel,tooltip)


#######################################################################################
#######################################################################################
#######################################################################################


    #-------------------------------------------
    # Scalars frame
    #-------------------------------------------
    set fScalars $Module(DTMRI,fScalars)
    set f $fScalars
    
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    foreach frame "Top" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill both
        $f.f$frame config -relief groove -bd 3
    }

    #-------------------------------------------
    # Scalars->Active frame
    #-------------------------------------------
    set f $fScalars.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m
    
    #-------------------------------------------
    # Scalars->Top frame
    #-------------------------------------------
    set f $fScalars.fTop
    
    foreach frame "ChooseOutput UseROI ScaleFactor Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Scalars->Top->ChooseOutput frame
    #-------------------------------------------
    set f $fScalars.fTop.fChooseOutput

    eval {label $f.lMath -text "Create Volume: "} $Gui(WLA)
    eval {menubutton $f.mbMath -text $DTMRI(scalars,operation) \
          -relief raised -bd 2 -width 20 \
          -menu $f.mbMath.m} $Gui(WMBA)

    eval {menu $f.mbMath.m} $Gui(WMA)
    pack $f.lMath $f.mbMath -side left -pady $Gui(pad) -padx $Gui(pad)
    # Add menu items
    foreach math $DTMRI(scalars,operationList) {
        $f.mbMath.m add command -label $math \
        -command "DTMRISetOperation $math"
    }
    # save menubutton for config
    set DTMRI(gui,mbMath) $f.mbMath
    # Add a tooltip
    TooltipAdd $f.mbMath $DTMRI(scalars,operationList,tooltip)

    #-------------------------------------------
    # Scalars->Top->UseROI frame
    #-------------------------------------------
    set f $fScalars.fTop.fUseROI

    DevAddLabel $f.l "ROI:"
    pack $f.l -side left -padx $Gui(pad) -pady 0

    foreach vis $DTMRI(scalars,ROIList) tip $DTMRI(scalars,ROIList,tooltips) {
        eval {radiobutton $f.rMode$vis \
          -text "$vis" -value "$vis" \
          -variable DTMRI(scalars,ROI) \
          -command DTMRIUpdateMathParams \
          -indicatoron 0} $Gui(WCA)
        pack $f.rMode$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.rMode$vis $tip
    }    

    #-------------------------------------------
    # Scalars->Top->ScaleFactor frame
    #-------------------------------------------
    set f $fScalars.fTop.fScaleFactor
    DevAddLabel $f.l "Scale Factor:"
    eval {entry $f.e -width 14 \
          -textvariable DTMRI(scalars,scaleFactor)} $Gui(WEA)
    TooltipAdd $f.e $DTMRI(scalars,scaleFactor,tooltip) 
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 0

    #-------------------------------------------
    # Scalars->Top->Apply frame
    #-------------------------------------------
    set f $fScalars.fTop.fApply

    DevAddButton $f.bApply "Apply" "DTMRIDoMath"    
    pack $f.bApply -side top -padx 0 -pady 0

#######################################################################################
#######################################################################################
#######################################################################################

    #-------------------------------------------
    # Convert frame
    #-------------------------------------------
    set fConvert $Module(DTMRI,fConvert)
    set f $fConvert
    
    foreach frame "Convert ShowPattern Pattern" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    }

    pack forget $f.fPattern
    $f.fConvert configure  -relief groove -bd 3 


    #-------------------------------------------
    # Convert->Convert frame
    #-------------------------------------------
    set f $fConvert.fConvert

    foreach frame "Title Select Pattern Repetitions Average Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        $f.fTitle configure -bg $Gui(backdrop)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Convert->Convert->Title frame
    #-------------------------------------------
    set f $fConvert.fConvert.fTitle
     
    DevAddLabel $f.lWellcome "Convert Tensors"
    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
    pack $f.lWellcome -side top -padx $Gui(pad) -pady $Gui(pad)
   
    DevAddLabel $f.lOption "This tab converts gradient data\n to diffusion tensor"
    $f.lOption configure -fg White -font {helvetica 9 normal}  -bg $Gui(backdrop) -bd 0
    pack $f.lOption -side top -padx $Gui(pad) -pady 2
    

    #-------------------------------------------
    # Convert->Convert->Select frame
    #-------------------------------------------
    set f $fConvert.fConvert.fSelect
    # Lauren test
    # menu to select a volume: will set Volume(activeID)
    DevAddSelectButton  Volume $f Active "Input Volume:" Pack \
            "Input Volume to create DTMRIs from." 13 BLA
    

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList) $f.mbActive.m


    #-------------------------------------------
    # Convert->Convert->Pattern frame
    #-------------------------------------------
#nowworking
    set f $fConvert.fConvert.fPattern

    DevAddLabel $f.lLabel "Protocol:"
    $f.lLabel configure -bg $Gui(backdrop) -fg white
    eval {menubutton $f.mbPattern -text "None" -relief raised -bd 2 -menu $f.mbPattern.menu -width 15} $Gui(WMBA)
    eval {menu $f.mbPattern.menu}  $Gui(WMA)
    button $f.bProp -text Prop. -width 5 -font {helvetica 8} -bg $Gui(normalButton) -fg $Gui(textDark)  -activebackground $Gui(activeButton) -activeforeground $Gui(textDark)  -bd $Gui(borderWidth) -padx 0 -pady 0 -relief raised -command {
        catch {DevInfoWindow $DTMRI($DTMRI(selectedpattern),tip)}
        catch {puts $DTMRI($DTMRI(selectedpattern),tip)}
        #DTMRIViewProps
    }

    pack $f.lLabel $f.bProp -side left -padx $Gui(pad) -pady $Gui(pad)
    DTMRILoadPattern
    TooltipAdd $f.lLabel "Choose a protocol to convert tensors.\n If desired does not exist, create one in the frame below."


    #-------------------------------------------
    # Convert->Convert->Repetitions frame
    #-------------------------------------------
    set f $fConvert.fConvert.fRepetitions
    
    DevAddLabel $f.l "Num. Repetitions:"
    $f.l configure -bg $Gui(backdrop) -fg white
    eval {entry $f.e -width 3 \
          -textvariable DTMRI(convert,numberOfRepetitions)} \
        $Gui(WEA)
    eval {scale $f.s -from $DTMRI(convert,numberOfRepetitions,min) \
                          -to $DTMRI(convert,numberOfRepetitions,max)    \
          -variable  DTMRI(convert,numberOfRepetitions)\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)
      
     pack $f.l $f.e $f.s -side left -padx $Gui(pad) -pady $Gui(pad)
     
    #-------------------------------------------
    # Convert->Convert->Average frame
    #-------------------------------------------
    set f $fConvert.fConvert.fAverage
    
    DevAddLabel $f.l "Average Repetitions: "
    pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad)  
    # Add menu items
    foreach vis $DTMRI(convert,averageRepetitionsList) val $DTMRI(convert,averageRepetitionsValue) \
            tip $DTMRI(convert,averageRepetitionsList,tooltips) {
        eval {radiobutton $f.r$vis \
              -text "$vis" \
              -value $val \
              -variable DTMRI(convert,averageRepetitions) \
              -indicatoron 0} $Gui(WCA)
        pack $f.r$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.r$vis $tip     
    }
              
#    #-------------------------------------------
#    # Convert->Convert->Apply frame
#    #-------------------------------------------
    set f $fConvert.fConvert.fApply
    DevAddButton $f.bTest "Convert Volume" ConvertVolumeToTensors 20
    pack $f.bTest -side top -padx 0 -pady $Gui(pad) -fill x -padx $Gui(pad)


    #-------------------------------------------
    # Convert->ShowPattern frame
    #-------------------------------------------
    set f $fConvert.fShowPattern
    
    DevAddLabel $f.lLabel "Create a new protocol if your data\n does not fit the predefined ones"

    button $f.bShow -text "Create New Protocol" -bg $Gui(backdrop) -fg white -font {helvetica 9 bold} -command {
        ShowPatternFrame 
        after 250 DTMRIDisplayScrollBar DTMRI Convert}
    TooltipAdd $f.bShow "Press this button to enter Create-Protocol Frame"
    pack $f.lLabel $f.bShow -side top -pady 2 -fill x




    #-------------------------------------------
    # Convert->Pattern->Gradients Title frame
    #-------------------------------------------

#    set f $fConvert.fPattern
#    frame $f.fTitle -bg $Gui(backdrop)
#    pack $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

#    set f $fConvert.fPattern.fTitle
#    set f $Page.fTitle
   
#    DevAddLabel $f.lWellcome "Create New Protocol"
#    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
#    pack $f.lWellcome -side top -padx $Gui(pad) -pady 0
   


    #-------------------------------------------
    # Convert->Pattern frame (create tabs)
    #-------------------------------------------
    set f $fConvert.fPattern
    DevAddLabel $f.lIni "Gradient Ordering scheme:"
    pack $f.lIni -side top -pady 2

    Notebook:create $f.fNotebook \
                    -pages {{Slice Interleav.} {Volume Interleav.}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 325 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1

    set f $fConvert.fPattern.fNotebook

    set FrameCont [Notebook:frame $f {Slice Interleav.}] 
    set FrameInter [Notebook:frame $f {Volume Interleav.}]

    foreach Page "$FrameCont $FrameInter" {   

        #-------------------------------------------
        # Convert->Pattern frame
        #-------------------------------------------
    #    set f $fConvert.fPattern
        set f $Page

        foreach frame "Name Disposal GradientNum GradientImages NoGradientImages Gradients Parameter Create" {
            frame $f.f$frame -bg $Gui(activeWorkspace)
            pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
        }

        $f configure  -relief sunken -bd 3 

        #-------------------------------------------
        # Convert->Pattern->Gradients Title frame
        #-------------------------------------------

    #    set f $fConvert.fPattern
    #    frame $f.fTitle -bg $Gui(backdrop)
    #    pack $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

        set f $fConvert.fPattern.fTitle
    #    set f $Page.fTitle
       
    #    DevAddLabel $f.lWellcome "Create New Protocol"
    #    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
    #    pack $f.lWellcome -side top -padx $Gui(pad) -pady $Gui(pad)
       

        #-------------------------------------------
        # Convert->Pattern->Gradients Name frame
        #-------------------------------------------

    #    set f $fConvert.fPattern.fName
        set f $Page.fName

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Protocol Name:"
    #   $f.lTitle configure -relief sunken -background gray -bd 2
        DevAddEntry DTMRI name,name $f.eName 15
        pack $f.lTitle $f.eName -side left -padx $Gui(pad) -pady 4 -fill x


     
        #-------------------------------------------
        # Convert->Pattern->Gradients Disposal frame
        #-------------------------------------------

    #    set f $fConvert.fPattern.fDisposal
        set f $Page.fDisposal

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Gradients/Baselines disposal in Volume:"
        $f.lTitle configure -relief sunken -background gray -bd 2
        pack $f.lTitle -side top -padx $Gui(pad) -pady 4 -fill x
     
        #-------------------------------------------
        # Convert->Pattern->GradientNum frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradientNum
        set f $Page.fGradientNum
        
        DevAddLabel $f.l "Number of Gradient Directions:"
        eval {entry $f.eEntry \
            -textvariable DTMRI(name,numberOfGradients) \
            -width 5} $Gui(WEA)
        pack $f.l $f.eEntry -side left -padx $Gui(pad) -pady 0 -fill x

        #-------------------------------------------
        # Convert->Pattern->GradientImages frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradientImages
        set f $Page.fGradientImages

        DevAddLabel $f.l "Gradient:"
        eval {entry $f.eEntry1 \
              -textvariable DTMRI(name,firstGradientImage) \
              -width 5} $Gui(WEA)
        eval {entry $f.eEntry2 \
              -textvariable DTMRI(name,lastGradientImage) \
              -width 5} $Gui(WEA)
        pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.eEntry1 \
            "First gradient (diffusion-weighted)\nimage number at first slice location"
        TooltipAdd $f.eEntry2 \
            "Last gradient (diffusion-weighted)\niimage number at first slice location"

        #-------------------------------------------
        # Convert->Pattern->NoGradientImages frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fNoGradientImages
        set f $Page.fNoGradientImages


        DevAddLabel $f.l "Baseline:"
        eval {entry $f.eEntry1 \
              -textvariable DTMRI(name,firstNoGradientImage) \
              -width 5} $Gui(WEA)
        eval {entry $f.eEntry2 \
              -textvariable DTMRI(name,lastNoGradientImage) \
              -width 5} $Gui(WEA)
        pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.eEntry1 \
            "First NO gradient (not diffusion-weighted)\nimage number at first slice location"
        TooltipAdd $f.eEntry2 \
            "Last NO gradient (not diffusion-weighted)\n image number at first slice location"

        #-------------------------------------------
        # Convert->Pattern->Gradients frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradients
        set f $Page.fGradients


        DevAddLabel $f.lLabel "Directions:"
        frame $f.fEntry -bg $Gui(activeWorkspace)
        eval {entry $f.fEntry.eEntry \
            -textvariable DTMRI(name,gradients) \
            -width 25 -xscrollcommand [list $f.fEntry.sx set]} $Gui(WEA)
            scrollbar $f.fEntry.sx -orient horizontal -command [list $f.fEntry.eEntry xview] -bg $Gui(normalButton) -width 10 -troughcolor $Gui(normalButton) 
        pack $f.fEntry.eEntry $f.fEntry.sx -side top -padx 0 -pady 0 -fill x
        pack $f.lLabel $f.fEntry -side left -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor n
        #pack $f.sx -side top -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.fEntry.eEntry "List of diffusion gradient directions"

        #-------------------------------------------
        # Convert->Pattern->Parameters frame
        #-------------------------------------------




    # This frame is supposed to hold the entries for needed parameters in tensors conversion.

    #    set f $fConvert.fPattern.fParameter
        set f $Page.fParameter

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Conversion Parameters:"
        $f.lTitle configure -relief sunken -background gray -bd 2
        pack $f.lTitle -side top -padx $Gui(pad) -pady 4 -fill x
        DevAddLabel $f.lLeBihan "LeBihan factor (b):"
        eval {entry $f.eEntrylebihan \
            -textvariable DTMRI(name,lebihan)  \
            -width 4} $Gui(WEA)
        eval {scale $f.slebihan -from 100 -to 5000 -variable DTMRI(name,lebihan) -orient vertical -resolution 10 -width 10} $Gui(WSA)
        pack $f.lLeBihan $f.eEntrylebihan $f.slebihan  -side left -padx $Gui(pad) -pady 0 -fill x -padx $Gui(pad)
        TooltipAdd $f.eEntrylebihan "Diffusion weighting factor, introduced and defined by LeBihan et al.(1986)"
      
    }

    #-------------------------------------------
    # Convert->Pattern->FrameCont-->Create frame
    #-------------------------------------------

    set f $FrameCont.fCreate
    DevAddButton $f.bCreate "Create New Protocol" DTMRICreatePatternSlice 8
    pack $f.bCreate -side top -pady $Gui(pad) -fill x
    TooltipAdd $f.bCreate "Click this button to create a new protocol after filling in parameters entries"
    

    #-------------------------------------------
    # Convert->Pattern->FrameInter-->Create frame
    #-------------------------------------------

    set f $FrameInter.fCreate
    DevAddButton $f.bCreate "Create New Protocol" DTMRICreatePatternVolume 8
    pack $f.bCreate -side top -pady $Gui(pad) -fill x
    TooltipAdd $f.bCreate "Click this button to create a new protocol after filling in parameters entries"
    

     #-------------------------------------------
     # Regist frame
     #-------------------------------------------

     DTMRIBuildRegistFrame

     #-------------------------------------------
     # Cluster frame
     #-------------------------------------------

     DTMRITractClusterBuildClusterFrame


######################################################################################
######################################################################################
######################################################################################

    #-------------------------------------------
    # Advanced frame
    #-------------------------------------------
    set fAdvanced $Module(DTMRI,fAdvanced)
    set f $fAdvanced
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Advanced->Top frame
    #-------------------------------------------
    set f $fAdvanced.fTop
    DevAddLabel $f.l "VTK objects in the pipeline"
    pack $f.l -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Advanced->Middle frame
    #-------------------------------------------
    set f $fAdvanced.fMiddle
    set fScrolled [DTMRIBuildScrolledGUI $f]
    set DTMRI(scrolledGui,advanced) $fScrolled

    # loop through all the vtk objects and build GUIs
    #------------------------------------              
    foreach o $DTMRI(vtkObjectList) {

        set f $fScrolled

        # if the object has properties
        #-------------------------------------------
        if {$DTMRI(vtkPropertyList,$o) != ""} {
            
            # make a new frame for this vtk object
            #-------------------------------------------
            frame $f.f$o -bg $Gui(activeWorkspace)
            $f.f$o configure  -relief groove -bd 3 
            pack $f.f$o -side top -padx $Gui(pad) -pady 2 -fill x
            
            # object name becomes the label for the frame
            #-------------------------------------------
            # Lauren we need an object description
            # and also basic or advanced attrib
            DevAddLabel $f.f$o.l$o [Cap $o]
            pack $f.f$o.l$o -side top \
                -padx 0 -pady 0
        }

        # loop through all the parameters for this object
        # and build appropriate user entry stuff for each
        #------------------------------------
        foreach p $DTMRI(vtkPropertyList,$o) {

            set f $fScrolled.f$o

            # name of entire tcl variable
            set variableName DTMRI(vtk,$o,$p)
            # its value is:
            set value $DTMRI(vtk,$o,$p)
            # description of the parameter
            set desc $DTMRI(vtk,$o,$p,description)
            # datatype of the parameter
            set type $DTMRI(vtk,$o,$p,type)

            # make a new frame for this parameter
            frame $f.f$p -bg $Gui(activeWorkspace)
            pack $f.f$p -side top -padx 0 -pady 1 -fill x
            set f $f.f$p

            # see if value is a list (not used now)
            #------------------------------------        
            set length [llength $value]
            set isList [expr $length > "1"]

            # Build GUI entry boxes, etc
            #------------------------------------        
            switch $type {
                "int" {
                    eval {entry $f.e$p \
                  -width 5 \
                  -textvariable $variableName\
                          } $Gui(WEA)
                    DevAddLabel $f.l$p $desc:
                    pack $f.l$p $f.e$p -side left \
                        -padx $Gui(pad) -pady 2
                }
                "float" {
                    eval {entry $f.e$p \
                          -width 5 \
                          -textvariable $variableName\
                          } $Gui(WEA)
                    DevAddLabel $f.l$p $desc:
                    pack $f.l$p $f.e$p -side left \
                        -padx $Gui(pad) -pady 2
                }
                "bool" {
                    # puts "bool: $variableName, $desc"
                    eval {checkbutton $f.r$p  \
                          -text $desc -variable $variableName \
                          } $Gui(WCA)
                    pack  $f.r$p -side left \
                        -padx $Gui(pad) -pady 2
                }
            }
            
        }
    }
    # end foreach vtk object in DTMRIs object list

    # Here's a button with text "Apply" that calls "AdvancedApply"
    DevAddButton $fAdvanced.fMiddle.bApply Apply DTMRIAdvancedApply
    pack $fAdvanced.fMiddle.bApply -side top -padx $Gui(pad) -pady $Gui(pad)


####################################################################################
####################################################################################
####################################################################################


    #-------------------------------------------
    # Save frame
    #-------------------------------------------
    set fSave $Module(DTMRI,fSave)
    set f $fSave
    
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    pack $f.fActive -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    
    foreach frame "Top Middle Bottom" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    $f.fTop configure  -relief groove -bd 3 
    $f.fMiddle configure  -relief groove -bd 3 

    #-------------------------------------------
    # Scalars->Active frame
    #-------------------------------------------
    set f $fSave.fActive

    # menu to select active DTMRI
    DevAddSelectButton  Tensor $f Active "Active DTMRI:" Pack \
    "Active DTMRI" 20 BLA 
    
    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    lappend Tensor(mbActiveList) $f.mbActive
    lappend Tensor(mActiveList) $f.mbActive.m

    #-------------------------------------------
    # Save->Top frame
    #-------------------------------------------
    set f $fSave.fTop

    DevAddButton $f.bSave "Save Tensors" {DTMRIWriteStructuredPoints $DTMRI(devel,fileName)}
    pack $f.bSave -side top -padx $Gui(pad) -pady $Gui(pad)
    TooltipAdd $f.bSave "Save tensor data (Active DTMRI) to vtk file format."

    #-------------------------------------------
    # Save->Middle frame
    #-------------------------------------------
    set f $fSave.fMiddle
    frame $f.fButton  -bg $Gui(activeWorkspace)
    pack $f.fButton -side top -padx $Gui(pad) -pady $Gui(pad)

    set f $fSave.fMiddle.fButton
    DevAddButton $f.bApply "Save tract points" \
        {DTMRISaveStreamlinesAsIJKPoints}
    TooltipAdd $f.bApply "Save text file(s) with the tract paths.\n This does not save vtk models."

    pack $f.bApply -side top -padx $Gui(pad) -pady $Gui(pad) 


   ##########
   # ODF Frame
   ##########
   DTMRIBuildODFFrame



}


#-------------------------------------------------------------------------------
# .PROC DTMRIRaiseMoreOptionsFrame
# 
# .ARGS
# string mode identifies the frame to raise
# .END
#-------------------------------------------------------------------------------
proc DTMRIRaiseMoreOptionsFrame {mode} {
    global DTMRI

    raise $DTMRI(frame,$mode)
    focus $DTMRI(frame,$mode)

    set DTMRI(mode,visualizationTypeGui) $mode
    # config menubutton
    $DTMRI(gui,mbVisMode)    config -text $mode
}

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildScrolledGUI
# 
# .ARGS
# string f path to frame
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildScrolledGUI {f} {
    global Gui DTMRI

    # window paths
    #-------------------------------------------
    set canvas $f.cGrid
    set s $f.sGrid
    
    # create canvas and scrollbar
    #-------------------------------------------
    canvas $canvas -yscrollcommand "$s set" -bg $Gui(activeWorkspace)
    eval "scrollbar $s -command \"DTMRICheckScrollLimits $canvas yview\"\
            $Gui(WSBA)"
    pack $s -side right -fill y
    pack $canvas -side top -fill both -pady $Gui(pad) -expand true
    
    set f $canvas.f
    #frame $f -bd 0 -bg $Gui(activeWorkspace)
    frame $f -bd 0
    $f configure  -relief groove -bd 3 
    
    # put the frame inside the canvas (so it can scroll)
    #-------------------------------------------
    $canvas create window 0 0 -anchor nw -window $f
    
    # y spacing important for calculation of frame height for scrolling
    set pady 2
    
    # save name of canvas to configure later
    set DTMRI(scrolledGUI,$f,canvas) $canvas

    # return path to the frame for filling
    return $f
}


#-------------------------------------------------------------------------------
# .PROC DTMRICheckScrollLimits
# This procedure allows scrolling only if the entire frame is not visible
# .ARGS
# list args
# .END
#-------------------------------------------------------------------------------
proc DTMRICheckScrollLimits {args} {

    set canvas [lindex $args 0]
    set view   [lindex $args 1]
    set fracs [$canvas $view]

    if {double([lindex $fracs 0]) == 0.0 && \
            double([lindex $fracs 1]) == 1.0} {
        return
    }
    eval $args
}

#-------------------------------------------------------------------------------
# .PROC DTMRISizeScrolledGUI
# 
# .ARGS
# string f path to frame
# .END
#-------------------------------------------------------------------------------
proc DTMRISizeScrolledGUI {f} {
    global DTMRI

    # see how tall our frame is these days
    set height [winfo reqheight  $f]    
    set canvas $DTMRI(scrolledGUI,$f,canvas)

    # tell the canvas to scroll so we can see the whole frame
    $canvas config -scrollregion "0 0 1 $height"
}

#-------------------------------------------------------------------------------
# .PROC DTMRISetPropertyType
# raise a panel in the GUI
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRISetPropertyType {} {
    global Module DTMRI

    #Lauren need properties subframes for this to work
    #raise $Volume(f$Volume(propertyType))
    #focus $Volume(f$Volume(propertyType))

    # Lauren temporarily use this
    raise $Module(DTMRI,f$DTMRI(propertyType))
}

################################################################
#  Procedures called by the GUI: Apply, Cancel, etc.
################################################################



#-------------------------------------------------------------------------------
# .PROC DTMRIAdvancedApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAdvancedApply {} {
    global DTMRI    
    puts "Applying"
    DTMRIApplyVisualizationParameters    
    puts "done applying"
    
    # display our changes
    Render3D
}








################################################################
#  bindings for user interaction
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRICreateBindings
#  Makes bindings for the module.  These are in effect when module is entered
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICreateBindings {} {
    global Gui Ev; # CustomCsys Csys
    
    #EvDeclareEventHandler DTMRICsysEvents <KeyPress-c> {CustomCsysDoSomethingCool}

    # this seeds a stream when the l key is hit (use s instead, it's nicer)
    EvDeclareEventHandler DTMRISlicesStreamlineEvents <KeyPress-l> \
    { if { [SelectPick2D %W %x %y] != 0 } \
          {  eval DTMRISelectStartHyperStreamline $Select(xyz); Render3D } }
    # this seeds a stream when the s key is hit
    EvDeclareEventHandler DTMRISlicesStreamlineEvents <KeyPress-s> \
    { if { [SelectPick2D %W %x %y] != 0 } \
          {  eval DTMRISelectStartHyperStreamline $Select(xyz); Render3D } }
    
    EvAddWidgetToBindingSet DTMRISlice0Events $Gui(fSl0Win) {DTMRISlicesStreamlineEvents}
    EvAddWidgetToBindingSet DTMRISlice1Events $Gui(fSl1Win) {DTMRISlicesStreamlineEvents}
    EvAddWidgetToBindingSet DTMRISlice2Events $Gui(fSl2Win) {DTMRISlicesStreamlineEvents}

    # this seeds a stream when the l key is hit (use s instead, it's nicer)
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-l> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectStartHyperStreamline $Select(xyz);Render3D } }
    # this seeds a stream when the s key is hit
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-s> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectStartHyperStreamline $Select(xyz);Render3D } }
    # this deletes a stream when the d key is hit
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-d> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectRemoveHyperStreamline $Select(xyz);Render3D } }
    # this chooses a streamline when c is hit
    EvDeclareEventHandler DTMRI3DStreamlineEvents <KeyPress-c> \
    { if { [SelectPick DTMRI(vtk,picker) %W %x %y] != 0 } \
          { eval DTMRISelectChooseHyperStreamline $Select(xyz);Render3D } }

    # This contains all the regular events from tkInteractor.tcl, 
    # which will happen after ours.  For some reason we don't need 
    # this for the slice windows, apparently their original bindings
    # are not done using Ev.tcl and they stay even when we add ours.
    EvAddWidgetToBindingSet DTMRI3DEvents $Gui(fViewWin) {{DTMRI3DStreamlineEvents} {tkMouseClickEvents} {tkMotionEvents} {tkRegularEvents}}
}




################################################################
#  little procedures to handle display control, interaction with user
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRIRoundFloatingPoint
# Format floats for GUI display (we don't want -5e-11)
# .ARGS
# float val
# .END
#-------------------------------------------------------------------------------
proc DTMRIRoundFloatingPoint {val} {
    global DTMRI

    return [format $DTMRI(floatingPointFormat) $val]
}


#-------------------------------------------------------------------------------
# .PROC DTMRIRemoveAllActors
# Rm all actors from scene.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIRemoveAllActors {} {
    global DTMRI
    
    # rm glyphs
    MainRemoveActor DTMRI(vtk,glyphs,actor)

    # rm streamlines
    DTMRIRemoveAllStreamlines

    Render3D

    set DTMRI(glyphs,actorsAdded) 0
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddAllActors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddAllActors {} {
    global DTMRI
    
    # rm glyphs
    MainAddActor DTMRI(vtk,glyphs,actor)

    # rm streamlines
    DTMRIAddAllStreamlines

    Render3D

    set DTMRI(glyphs,actorsAdded) 1
}



################################################################
#  Procedures to set up pipelines and create/modify vtk objects.
#  TODO: try to create objects only if needed!
################################################################


#-------------------------------------------------------------------------------
# .PROC DTMRIApplyVisualizationParameters
#  For interaction with pipeline under advanced tab.
#  Apply all GUI parameters into our vtk objects.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIApplyVisualizationParameters {} {
    global DTMRI

    # this code actually makes a bunch of calls like the following:
    # DTMRI(vtk,axes) SetScaleFactor $DTMRI(vtk,axes,scaleFactor)
    # DTMRI(vtk,tubeAxes) SetRadius $DTMRI(vtk,tubeAxes,radius)
    # DTMRI(vtk,glyphs) SetClampScaling 1
    # we can't do calls like MyObject MyVariableOn now

    # our naming convention is:
    # DTMRI(vtk,object)  is the name of the object
    # paramName is the name of the parameter
    # $DTMRI(vtk,object,paramName) is the value 

    # Lauren we need to validate too!!!!!!!!
    #  too bad vtk can't return a string type desrciptor...

    # loop through all vtk objects
    #------------------------------------
    foreach o $DTMRI(vtkObjectList) {

        # loop through all parameters of the object
        #------------------------------------
        foreach p $DTMRI(vtkPropertyList,$o) {

            # value of the parameter is $DTMRI(vtk,$o,$p)
            #------------------------------------
            set value $DTMRI(vtk,$o,$p)
            
            # Actually set the value appropriately in the vtk object
            #------------------------------------    

            # first capitalize the parameter name
            set param [Cap $p]
            
            # MyObject SetMyParameter $value    
            # handle the case in which value is a list with an eval 
            # this puts it into the correct format for feeding to vtk
            eval {DTMRI(vtk,$o) Set$param} $value
        }
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIDeleteVTKObject
# 
# .ARGS
# string object
# .END
#-------------------------------------------------------------------------------
proc DTMRIDeleteVTKObject {object} {
    global DTMRI

    # delete the object
    #------------------------------------
    DTMRI(vtk,$object) Delete

    # rm from list for updating of its variables by user
    #------------------------------------
    set i [lsearch $DTMRI(vtkObjectList) $object]
    set DTMRI(vtkObjectList) [lreplace $DTMRI(vtkObjectList) $i $i]

    # kill list of its variables
    #------------------------------------
    unset DTMRI(vtkPropertyList,$object) 
}

#-------------------------------------------------------------------------------
# .PROC DTMRIMakeVTKObject
#  Wrapper for vtk object creation.
# .ARGS
# string class
# string object
# .END
#-------------------------------------------------------------------------------
proc DTMRIMakeVTKObject {class object} {
    global DTMRI

    # make the object
    #------------------------------------
    $class DTMRI(vtk,$object)

    # save on list for updating of its variables by user
    #------------------------------------
    lappend DTMRI(vtkObjectList) $object

    # initialize list of its variables
    #------------------------------------
    set DTMRI(vtkPropertyList,$object) ""
}

#-------------------------------------------------------------------------------
# .PROC DTMRIAddObjectProperty
#  Initialize vtk object access: saves object's property on list
#  for automatic GUI creation so user can change the property.
# .ARGS
# string object
# string parameter
# string value
# string type
# string desc
# .END
#-------------------------------------------------------------------------------
proc DTMRIAddObjectProperty {object parameter value type desc} {
    global DTMRI

    # create tcl variables of the form:
    #set DTMRI(vtk,tubeAxes,numberOfSides) 6
    #set DTMRI(vtk,tubeAxes,numberOfSides,type) int
    #set DTMRI(vtk,tubeAxes,numberOfSides,description) 
    #lappend DTMRI(vtkPropertyList,tubeAxes) numberOfSides 


    # make tcl variable and save its attributes (type, desc)
    #------------------------------------
    set param [Uncap $parameter]
    set DTMRI(vtk,$object,$param) $value
    set DTMRI(vtk,$object,$param,type) $type
    set DTMRI(vtk,$object,$param,description) $desc

    # save on list for updating variable by user
    #------------------------------------
    lappend DTMRI(vtkPropertyList,$object) $param
    
}

#-------------------------------------------------------------------------------
# .PROC DTMRIBuildVTK
# Called automatically by the slicer program.
# builds pipelines.
# See also DTMRIUpdate for pipeline use.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIBuildVTK {} {
    global DTMRI Module

    # Lauren: maybe this module should kill its objects on exit.
    # We must be hogging a ton of memory all over the slicer.
    # So possibly build (some) VTK upon entering module.

    #---------------------------------------------------------------
    # coordinate system actor (also called "Csys" and "gyro" 
    # in other modules).  Used for user interaction with hyperstreamlines
    #---------------------------------------------------------------
    #CsysCreate DTMRI coordinateAxesTool -1 -1 -1

    #---------------------------------------------------------------
    # Pipeline for preprocessing of glyphs
    #---------------------------------------------------------------

    # objects for thresholding before glyph display
    #------------------------------------


    #---------------------------------------------------------------
    # Pipeline for display of DTMRIs over 2D slice
    #---------------------------------------------------------------
    
    # Lauren how should reformatting be hooked into regular
    # slicer slice reformatting?  Ideally want to follow
    # the 3 slices.
    foreach plane "0 1 2" {
    DTMRIMakeVTKObject vtkImageReformat reformat$plane
    }

    # compute scalar data for thresholding
    set object thresh,math
    DTMRIMakeVTKObject vtkTensorMathematics $object
    DTMRIAddObjectProperty $object ExtractEigenvalues 1 bool {Extract Eigenvalues}

    # threshold the scalar data to produce binary mask 
    set object thresh,threshold
    #DTMRIMakeVTKObject vtkImageThresholdBeyond $object
    DTMRIMakeVTKObject vtkImageThreshold $object
    DTMRI(vtk,$object) SetInValue       1
    DTMRI(vtk,$object) SetOutValue      0
    DTMRI(vtk,$object) SetReplaceIn     1
    DTMRI(vtk,$object) SetReplaceOut    1
    DTMRI(vtk,$object) SetInput \
        [DTMRI(vtk,thresh,math) GetOutput]

    # convert the mask to unsigned char
    # Lauren it would be better to have the threshold filter do this
    set object thresh,cast
    DTMRIMakeVTKObject vtkImageCast $object
    DTMRI(vtk,$object) SetOutputScalarTypeToUnsignedChar    
    DTMRI(vtk,$object) SetInput \
        [DTMRI(vtk,thresh,threshold) GetOutput]

    # mask the DTMRIs 
    set object thresh,mask
    DTMRIMakeVTKObject vtkTensorMask $object
    #DTMRI(vtk,$object) SetMaskInput \
    #    [DTMRI(vtk,thresh,threshold) GetOutput]
    DTMRI(vtk,$object) SetMaskInput \
        [DTMRI(vtk,thresh,cast) GetOutput]

    # objects for masking before glyph display
    #------------------------------------

    # produce binary mask from the input mask labelmap
    set object mask,threshold
    #DTMRIMakeVTKObject vtkImageThresholdBeyond $object
    DTMRIMakeVTKObject vtkImageThreshold $object
    DTMRI(vtk,$object) SetInValue       1
    DTMRI(vtk,$object) SetOutValue      0
    DTMRI(vtk,$object) SetReplaceIn     1
    DTMRI(vtk,$object) SetReplaceOut    1

    # convert the mask to short
    # (use this most probable input type to try to avoid data copy)
    set object mask,cast
    DTMRIMakeVTKObject vtkImageCast $object
    DTMRI(vtk,$object) SetOutputScalarTypeToShort    
    DTMRI(vtk,$object) SetInput \
        [DTMRI(vtk,mask,threshold) GetOutput]

    # mask the DTMRIs 
    set object mask,mask
    DTMRIMakeVTKObject vtkTensorMask $object
    DTMRI(vtk,$object) SetMaskInput \
        [DTMRI(vtk,mask,cast) GetOutput]

    #---------------------------------------------------------------
    # Pipeline for display of glyphs
    #---------------------------------------------------------------

    # User interaction objects
    #------------------------------------
    # Lauren: doing this like Endoscopic (this vs PointPicker?)
    set object picker
    DTMRIMakeVTKObject vtkCellPicker $object
    DTMRIAddObjectProperty $object Tolerance 0.001 float {Pick Tolerance}

    # Lauren test by displaying picked point
    #DTMRI(vtk,picker) SetEndPickMethod "annotatePick DTMRI(vtk,picker)"
    vtkTextMapper textMapper
    if {[info commands vtkTextProperty] != ""} {
        [textMapper GetTextProperty] SetFontFamilyToArial
        [textMapper GetTextProperty] SetFontSize 20
        [textMapper GetTextProperty] BoldOn
        [textMapper GetTextProperty] ShadowOn
    } else {
        textMapper SetFontFamilyToArial
        textMapper SetFontSize 20
        textMapper BoldOn
        textMapper ShadowOn
    }
    vtkActor2D textActor
    textActor VisibilityOff
    textActor SetMapper textMapper
    [textActor GetProperty] SetColor 1 0 0
    viewRen AddActor2D textActor

    # objects for creation of polydata glyphs
    #------------------------------------

    # Axes
    set object glyphs,axes
    DTMRIMakeVTKObject vtkAxes $object
    DTMRIAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeAxes
    #DTMRIMakeVTKObject vtkTubeFilter $object
    #DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs,axes) GetOutput]
    #DTMRIAddObjectProperty $object Radius 0.1 float {Radius}
    #DTMRIAddObjectProperty $object NumberOfSides 6 int \
    #    {Number Of Sides}

    # One line
    set object glyphs,line
    DTMRIMakeVTKObject vtkLineSource $object
    DTMRIAddObjectProperty $object Resolution 10 int {Resolution}
    #DTMRI(vtk,$object) SetPoint1 0 0 0
    # use a stick that points both ways, not a vector from the origin!
    DTMRI(vtk,$object) SetPoint1 -1 0 0
    DTMRI(vtk,$object) SetPoint2 1 0 0
    
    # too slow: maybe useful for nice photos
    #set object glyphs,tubeLine
    #DTMRIMakeVTKObject vtkTubeFilter $object
    #DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs,line) GetOutput]
    #DTMRIAddObjectProperty $object Radius 0.1 float {Radius}
    #DTMRIAddObjectProperty $object NumberOfSides 6 int \
    #    {Number Of Sides}

    # Ellipsoids
    set object glyphs,sphere
    DTMRIMakeVTKObject vtkSphereSource  $object
    #DTMRIAddObjectProperty $object ThetaResolution 1 int ThetaResolution
    #DTMRIAddObjectProperty $object PhiResolution 1 int PhiResolution
    DTMRIAddObjectProperty $object ThetaResolution 12 int ThetaResolution
    DTMRIAddObjectProperty $object PhiResolution 12 int PhiResolution

    # Boxes
    set object glyphs,box
    DTMRIMakeVTKObject vtkCubeSource  $object

    # objects for placement of Standard glyphs in dataset
    #------------------------------------
    set object glyphs
    foreach plane "0 1 2" {
    #DTMRIMakeVTKObject vtkDTMRIGlyph $object
    DTMRIMakeVTKObject vtkInteractiveTensorGlyph $object$plane
    DTMRI(vtk,$object$plane) SetInput ""
    #DTMRI(vtk,glyphs$plane) SetSource [DTMRI(vtk,glyphs,axes) GetOutput]
    #DTMRI(vtk,glyphs$plane) SetSource [DTMRI(vtk,glyphs,sphere) GetOutput]
    #DTMRIAddObjectProperty $object ScaleFactor 1 float {Scale Factor}
    DTMRIAddObjectProperty $object$plane ScaleFactor 1000 float {Scale Factor}
    DTMRIAddObjectProperty $object$plane ClampScaling 0 bool {Clamp Scaling}
    DTMRIAddObjectProperty $object$plane ExtractEigenvalues 1 bool {Extract Eigenvalues}
    DTMRI(vtk,$object$plane) AddObserver StartEvent MainStartProgress
    DTMRI(vtk,$object$plane) AddObserver ProgressEvent "MainShowProgress DTMRI(vtk,$object$plane)"
    DTMRI(vtk,$object$plane) AddObserver EndEvent MainEndProgress
    }
    
    # objects for placement of Superquadric glyphs in dataset
    #------------------------------------    
    set object glyphsSQ
    foreach plane "0 1 2" {
    DTMRIMakeVTKObject vtkSuperquadricTensorGlyph $object$plane
    DTMRI(vtk,$object$plane) SetInput ""
    DTMRIAddObjectProperty $object$plane ScaleFactor 1000 float {Scale Factor}
    DTMRIAddObjectProperty $object$plane ClampScaling 0 bool {Clamp Scaling}
    DTMRIAddObjectProperty $object$plane Gamma 1 float {Gamma}
    DTMRIAddObjectProperty $object$plane ThetaResolution 12 int {ThetaResolution}
    DTMRIAddObjectProperty $object$plane PhiResolution 12 int {PhiResolution}
    DTMRI(vtk,$object$plane) AddObserver StartEvent MainStartProgress
    DTMRI(vtk,$object$plane) AddObserver ProgressEvent "MainShowProgress DTMRI(vtk,$object$plane)"
    DTMRI(vtk,$object$plane) AddObserver EndEvent MainEndProgress
    }
    
    set object glyphs,trans
    DTMRIMakeVTKObject vtkTransform $object
    
    #poly data append to join glyphs from the 3 slice planes
    set object glyphs,append
    DTMRIMakeVTKObject vtkAppendPolyData $object
    DTMRI(vtk,$object) UserManagedInputsOn
    
    # poly data normals filter cleans up polydata for nice display
    # use this for ellipses/boxes only
    #------------------------------------
    # very slow
    set object glyphs,normals
    DTMRIMakeVTKObject vtkPolyDataNormals $object
    DTMRI(vtk,$object) SetInput [DTMRI(vtk,glyphs,append) GetOutput]

    # Display of DTMRI glyphs: LUT and Mapper
    #------------------------------------
    set object glyphs,lut
    #DTMRIMakeVTKObject vtkLogLookupTable $object
    DTMRIMakeVTKObject vtkLookupTable $object
    DTMRIAddObjectProperty $object HueRange \
        {0 1} float {Hue Range}

    # mapper
    set object glyphs,mapper
    DTMRIMakeVTKObject vtkPolyDataMapper $object
    #Raul
    DTMRI(vtk,glyphs,mapper) SetInput [DTMRI(vtk,glyphs,append) GetOutput]
    #DTMRI(vtk,glyphs,mapper) SetInput [DTMRI(vtk,glyphs,normals) GetOutput]
    DTMRI(vtk,glyphs,mapper) SetLookupTable DTMRI(vtk,glyphs,lut)
    DTMRIAddObjectProperty $object ImmediateModeRendering \
        1 bool {Immediate Mode Rendering}    

    # Display of DTMRI glyphs: Actor
    #------------------------------------
    set object glyphs,actor
    #DTMRIMakeVTKObject vtkActor $object
    DTMRIMakeVTKObject vtkLODActor $object
    DTMRI(vtk,glyphs,actor) SetMapper DTMRI(vtk,glyphs,mapper)
    # intermediate level of detail produces visible points with 10
    [DTMRI(vtk,glyphs,actor) GetProperty] SetPointSize 10
    
    [DTMRI(vtk,glyphs,actor) GetProperty] SetAmbient 1
    [DTMRI(vtk,glyphs,actor) GetProperty] SetDiffuse .2
    [DTMRI(vtk,glyphs,actor) GetProperty] SetSpecular .4

    # Scalar bar actor
    #------------------------------------
    set object scalarBar,actor
    DTMRIMakeVTKObject vtkScalarBarActor $object
    DTMRI(vtk,scalarBar,actor) SetLookupTable DTMRI(vtk,glyphs,lut)
    viewRen AddProp DTMRI(vtk,scalarBar,actor)
    DTMRI(vtk,scalarBar,actor) VisibilityOff

    #---------------------------------------------------------------
    # Pipeline for display of tractography
    #---------------------------------------------------------------
    vtkMultipleStreamlineController DTMRI(vtk,streamlineControl)
    #DTMRI(vtk,streamlineControl) DebugOn
    # give it the renderers in which we display streamlines
    vtkCollection DTMRI(vtk,renderers)
    foreach r $Module(Renderers) {
        DTMRI(vtk,renderers) AddItem $r
    }
    DTMRI(vtk,streamlineControl) SetInputRenderers DTMRI(vtk,renderers)
    # This will be the input to the streamline controller. It lets us merge 
    # scalars from various datasets with the input tensor field
    vtkMergeFilter DTMRI(vtk,streamline,merge)

    # these are example objects used in creation of hyperstreamlines
    set streamline "streamlineControl,vtkHyperStreamlinePoints"
    vtkHyperStreamlinePoints DTMRI(vtk,$streamline) 
    DTMRI(vtk,streamlineControl) SetVtkHyperStreamlinePointsSettings \
        DTMRI(vtk,$streamline)
    set streamline "streamlineControl,vtkPreciseHyperStreamlinePoints"
    vtkPreciseHyperStreamlinePoints DTMRI(vtk,$streamline)
    DTMRI(vtk,streamlineControl) SetVtkPreciseHyperStreamlinePointsSettings \
        DTMRI(vtk,$streamline)
    


    #---------------------------------------------------------------
    # Pipeline for BSpline tractography (moved from proc DTMRIInit)
    #---------------------------------------------------------------

    DTMRIMakeVTKObject vtkTensorImplicitFunctionToFunctionSet itf
    set DTMRI(vtk,BSpline,init) 0
    set DTMRI(vtk,BSpline,data) 0
    
    if {[info command vtkITKBSplineImageFilter] == ""} {
        DevErrorWindow "DTMRI\nERROR: vtkITKBSplineImageFilter does not exist, cannot use bspline filter"
    }
    for {set i 0} {$i < 6} {incr i 1} {
        DTMRIMakeVTKObject vtkBSplineInterpolateImageFunction impComp($i)
        DTMRI(vtk,itf) AddImplicitFunction DTMRI(vtk,impComp($i)) $i
        if {[info command vtkITKBSplineImageFilter] != ""} {
            DTMRIMakeVTKObject vtkITKBSplineImageFilter bspline($i)
        } 
        DTMRIMakeVTKObject vtkExtractTensorComponents extractor($i)
        DTMRI(vtk,extractor($i)) PassTensorsToOutputOff
        DTMRI(vtk,extractor($i)) ExtractScalarsOn
        DTMRI(vtk,extractor($i)) ExtractVectorsOff
        DTMRI(vtk,extractor($i)) ExtractNormalsOff
        DTMRI(vtk,extractor($i)) ExtractTCoordsOff
        DTMRI(vtk,extractor($i)) ScalarIsComponent
    }
    
    DTMRI(vtk,extractor(0)) SetScalarComponents 0 0
    DTMRI(vtk,extractor(1)) SetScalarComponents 0 1
    DTMRI(vtk,extractor(2)) SetScalarComponents 0 2
    DTMRI(vtk,extractor(3)) SetScalarComponents 1 1
    DTMRI(vtk,extractor(4)) SetScalarComponents 1 2
    DTMRI(vtk,extractor(5)) SetScalarComponents 2 2
    
    DTMRIMakeVTKObject vtkRungeKutta45 rk45
    DTMRIMakeVTKObject vtkRungeKutta4 rk4
    DTMRIMakeVTKObject vtkRungeKutta2 rk2
    
    DTMRI(vtk,rk45) SetFunctionSet DTMRI(vtk,itf)
    DTMRI(vtk,rk4) SetFunctionSet DTMRI(vtk,itf)
    DTMRI(vtk,rk2) SetFunctionSet DTMRI(vtk,itf)
    
    set DTMRI(vtk,ivps) DTMRI(vtk,rk4)

    #Objects for finding streamlines through several ROIS
    vtkShortArray DTMRI(vtk,ListLabels)
    vtkDoubleArray DTMRI(vtk,convKernel)    
    
    #Get Kernel 
    vtkStructuredPointsReader DTMRI(vtk,tmp1)
    global PACKAGE_DIR_VTKDTMRI
    DTMRI(vtk,tmp1) SetFileName $PACKAGE_DIR_VTKDTMRI/../../../data/GKernel.vtk
    DTMRI(vtk,tmp1) Update

    vtkImageCast DTMRI(vtk,tmp2)
    DTMRI(vtk,tmp2) SetInput [DTMRI(vtk,tmp1) GetOutput]
    DTMRI(vtk,tmp2) SetOutputScalarTypeToDouble
    DTMRI(vtk,tmp2) Update
    
    DTMRI(vtk,convKernel) DeepCopy [[[DTMRI(vtk,tmp2) GetOutput] GetPointData] GetScalars]

    DTMRI(vtk,tmp1) Delete
    DTMRI(vtk,tmp2) Delete
    
    # Apply all settings from tcl variables that were
    # created above using calls to DTMRIAddObjectProperty
    #------------------------------------
    DTMRIApplyVisualizationParameters


    DTMRIBuildVTKODF

}




################################################################
# procedures for saving DTMRIs, streamlines, etc.
# TODO: some of this should happen automatically and be in MRML
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIWriteStructuredPoints
# Dump DTMRIs to structured points file.  this ignores
# world to RAS, DTMRIs are just written in scaled ijk coordinate system.
# .ARGS
# path filename
# .END
#-------------------------------------------------------------------------------
proc DTMRIWriteStructuredPoints {filename} {
    global DTMRI Tensor

    set t $Tensor(activeID)

    set filename [tk_getSaveFile -defaultextension ".vtk" -title "Save tensor as vtkstructurepoints"]
    if { $filename == "" } {
        return
    }

    vtkStructuredPointsWriter writer
    writer SetInput [Tensor($t,data) GetOutput]
    writer SetFileName $filename
    writer SetFileTypeToBinary
    puts "Writing $filename..."
    writer Write
    writer Delete
    puts "Wrote DTMRI data, id $t, as $filename"
}



################################################################
#  Procedures that deal with coordinate systems
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRIGetScaledIjkCoordinatesFromWorldCoordinates
#
# Use our world to ijk matrix information to correct x,y,z.
# The streamline class doesn't know about the
# DTMRI actor's "UserMatrix" (actually implemented with two matrices
# in the glyph class).  We need to transform xyz by
# the inverse of this matrix (almost) so the streamline will start 
# from the right place in the DTMRIs.  The "almost" is because the 
# DTMRIs know about their spacing (vtkImageData) and so we must
# remove the spacing from this matrix.
#
#
# .ARGS
# int x x-coordinate of input world coordinates point 
# int y y-coord
# int z z-coord
# .END
#-------------------------------------------------------------------------------
proc DTMRIGetScaledIjkCoordinatesFromWorldCoordinates {x y z} {
    global DTMRI Tensor

    set t $Tensor(activeID)

    vtkTransform transform
    DTMRICalculateActorMatrix transform $t    
    transform Inverse
    set point [transform TransformPoint $x $y $z]
    transform Delete

    # check point is in bounds of the dataset
    set dims [[Tensor($t,data) GetOutput] GetDimensions]
    set space [[Tensor($t,data) GetOutput] GetSpacing]
    # return "-1 -1 -1" if out of bounds error 
    foreach d $dims s $space p $point {
        if {$p < 0} {
            set point "-1 -1 -1"
        } elseif {$p > [expr $d*$s]} {
            set point "-1 -1 -1"
        }
    }

    return $point
}

#-------------------------------------------------------------------------------
# .PROC DTMRICalculateActorMatrix
# Place the entire Tensor volume in world coordinates
# using this transform.  Uses world to IJk matrix but
# removes the spacing since the data/actor know about this.
# .ARGS
# vtkTransform transform the transform to modify
# int t the id of the DTMRI volume to calculate the matrix for
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateActorMatrix {transform t} {
    global Tensor
    # Grab the node whose data we want to position 
    set node Tensor($t,node)

    # the user matrix is either the reformat matrix
    # to place the slice, OR it needs to place the entire 
    # DTMRI volume.

    # In this procedure we calculate the second kind of matrix,
    # to place the whole volume.
    $transform Identity
    $transform PreMultiply

    # Get positioning information from the MRML node
    # world space (what you see in the viewer) to ijk (array) space
    $transform SetMatrix [$node GetWldToIjk]

    # now it's ijk to world
    $transform Inverse

    # the data knows its spacing already so remove it
    # (otherwise the actor would be stretched, ouch)
    scan [$node GetSpacing] "%g %g %g" res_x res_y res_z

    $transform Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
        [expr 1.0 / $res_z]
}


#-------------------------------------------------------------------------------
# .PROC DTMRICalculateIJKtoRASRotationMatrix
# 
#  The IJK to RAS matrix has two actions on the DTMRIs.
#  <p>
#  1.  Each DTMRI glyph must be placed at the (x,y,z) location
#  determined by the matrix.  This is analogous to setting the
#  reformat matrix as the actor's user matrix when placing 
#  scalar data.  However, actor matrices do not work here because
#  of number 2, next.
#  <p>
#  2.  Each DTMRI itself must be rotated from ijk to ras.  This
#  uses the ijk to ras matrix, but without any scaling or translation.
#  The DTMRIs are created in the ijk coordinate system so that 
#  diffusion-simulation filters and hyperstreamlines, which do not 
#  know about RAS or actor placement, can correctly handle the data.
#
#
#  <p> This procedure removes translation and scaling 
#  from a volume's ijk to ras matrix, and it returns
#  a rotation matrix that can act on each DTMRI.
#
# .ARGS
# string transform
# int t
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateIJKtoRASRotationMatrix {transform t} {
    global Volume Tensor

    # --------------------------------------------------------
    # Rotate DTMRIs to RAS  (actually to World space)
    # --------------------------------------------------------
    # We want the DTMRIs to be displayed in the RAS coordinate system

    # The upper left 3x3 part of this matrix is the rotation.
    # (It also has voxel scaling which we will remove.)
    # -------------------------------------
    #$transform SetMatrix [Tensor($t,node)  GetRasToIjk]
    $transform SetMatrix [Tensor($t,node)  GetWldToIjk]
    # Now it's ijk to ras
    $transform Inverse

    # Remove the voxel scaling from the matrix.
    # -------------------------------------
    scan [Tensor($t,node) GetSpacing] "%g %g %g" res_x res_y res_z

    # We want -y since vtk flips the y axis
    #puts "Not flipping y"
    #set res_y [expr -$res_y]
    $transform Scale [expr 1.0 / $res_x] [expr 1.0 / $res_y] \
    [expr 1.0 / $res_z]

    # Remove the translation part from the last column.
    # (This was in there to center the volume in the cube.)
    # -------------------------------------
    [$transform GetMatrix] SetElement 0 3 0
    [$transform GetMatrix] SetElement 1 3 0
    [$transform GetMatrix] SetElement 2 3 0
    # Set element (4,4) to 1: homogeneous point
    [$transform GetMatrix] SetElement 3 3 1

    # Now this matrix JUST does the rotation needed for ijk->ras.
    # -------------------------------------
    puts "-----------------------------------"
    puts [$transform Print]
    puts "-----------------------------------"

}


#-------------------------------------------------------------------------------
# .PROC DTMRI SetActive
# Set the active tensor on the menus, and make it input to the 
# tractography pipeline. (The glyph pipeline input is set up when 
# the glyph button is pressed, in the procedure DTMRIUpdate. It could
# be faster to set it up here also.)
# .ARGS
# int n
# .END
#-------------------------------------------------------------------------------
proc DTMRISetActive {n} {
    global Tensor DTMRI

    # Call the procedure that puts this tensor on the menus
    MainDataSetActive Tensor $n

    # Make sure this tensor is the input to the glyph pipeline
    DTMRIUpdate

    # get the active ID (index into tensor list)
    set t $Tensor(activeID)

    # set up the tractography pipeline with both data and location
    # information from the active tensor dataset
    DTMRI(vtk,streamline,merge) SetTensors [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) SetGeometry [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) SetScalars [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) SetVectors [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) SetNormals [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) SetTCoords [Tensor($t,data) GetOutput]
    DTMRI(vtk,streamline,merge) Update
    DTMRI(vtk,streamlineControl) SetInputTensorField \
        [DTMRI(vtk,streamline,merge) GetOutput] 
 
    # set correct transformation from World coords to scaledIJK of the tensors
    vtkTransform transform
    DTMRICalculateActorMatrix transform $t    
    transform Inverse
    DTMRI(vtk,streamlineControl) SetWorldToTensorScaledIJK transform
    transform Delete    

    # start with solid colors since we can't be sure selected volume
    # is okay to color tracts with (i.e. may not have same size).
    # this also sets up the correct color for the first tract.
    DTMRIUpdateTractColorToSolid

    # initial setup of the streamline control object for the
    # type of streamline to create.
    DTMRIUpdateStreamlineSettings

#     # set up the BSpline tractography pipeline
#     set DTMRI(vtk,BSpline,data) 1
#     set DTMRI(vtk,BSpline,init) 1;
#     DTMRI(vtk,itf) SetDataBounds [Tensor($t,data) GetOutput]
#     #DTMRI(vtk,itf) SetDataBounds [DTMRI(vtk,streamline,merge) GetOutput]
#     for {set i 0} {$i < 6} {incr i} {
#         DTMRI(vtk,extractor($i)) SetInput [Tensor($t,data) GetOutput]
#         #DTMRI(vtk,extractor($i)) SetInput \
#         #    [DTMRI(vtk,streamline,merge) GetOutput]
#     }
#     for {set i 0} {$i < 6} {incr i} {
#         #DTMRI(vtk,extractor($i)) Update
#         DTMRI(vtk,bspline($i)) SetInput [DTMRI(vtk,extractor($i)) GetOutput]
#     }          
#     DTMRIUpdateBSplineOrder $DTMRI(stream,BSplineOrder)
#     for {set i 0} {$i < 6} {incr i} {
#         #DTMRI(vtk,bspline($i)) Update
#         DTMRI(vtk,impComp($i)) SetInput [DTMRI(vtk,bspline($i)) GetOutput]
#     }
}

