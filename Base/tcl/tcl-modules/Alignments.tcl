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
# FILE:        Alignments.tcl
# PROCEDURES:  
#   AlignmentsInit
#   AlignmentsUpdateMRML
#   AlignmentsBuildGUI
#   AlignmentsBuildVTK
#   AlignmentsSetVolumeMatrix
#   AlignmentsIdentity
#   AlignmentsInvert
#   AlignmentsSetPropertyType
#   AlignmentsPropsApply
#   AlignmentsPropsCancel
#   AlignmentsManualTranslate
#   AlignmentsManualTranslateDual
#   AlignmentsManualRotate
#   AlignmentsSetRefVolume
#   AlignmentsSetVolume
#   AlignmentsWritePseudoMrmlVolume
#   AlignmentsAutoRun
#   AlignmentsAutoCancel
#   AlignmentsPoll
#   AlignmentsAutoApply
#   AlignmentsAutoUndo
#   AlignmentsB1
#   AlignmentsB1Motion
#   AlignmentsB3Motion
#   AlignmentsInteractorZoom
#   AlignmentsSlicesSetZoom
#   AlignmentsB1Release
#   AlignmentsSetRegistrationMech
#   AlignmentsBuildSliceControls
#   AlignmentsBuildControls
#   AlignmentsBuildActiveVolSelectControls
#   AlignmentsConfigSliceGUI
#   AlignmentsUnpackFidSelectScreenControls
#   AlignmentsSetSliceControls
#   AlignmentsFiducialPick
#   AlignmentsSplitVolumes
#   AlignmentsPick2D
#   AlignmentsSetViewMenuState
#   AlignmentsAddLetterActors
#   AlignmentsMatRenUpdateCamera
#   AlignmentsUpdateFidSelectViewVisibility
#   AlignmentsUpdateMainViewVisibility
#   AlignmentsAddMainView
#   AlignmentsAddFidSelectView
#   AlignmentsRemoveFidSelectView
#   AlignmentsRemoveMainView
#   AlignmentsGetCurrentView
#   AlignmentsSetActiveScreen
#   AlignmentsFiducialsUpdated
#   AlignmentsBuildBoxesForList
#   AlignmentsDeleteFiducialsList
#   AlignmentsApplyFiducialSelection
#   AlignmentsFidSelectCancel
#   AlignmentsSlicesSetOrientAll
#   AlignmentsSlicesSetOrient
#   AlignmentsSlicesOffsetUpdated
#   AlignmentsSlicesSetSliderRange int
#   AlignmentsSlicesSetOffsetIncrement int float
#   AlignmentsSlicesSetOffset
#   AlignmentsSlicesSetOffsetInit
#   AlignmentsSlicesSetVisibilityAll
#   AlignmentsSlicesSetVisibility
#   AlignmentsSlicesRefreshClip int
#   AlignmentsSetSliceWindows
#   AlignmentsLandTrans
#   AlignmentsSetActiveSlicer
#   AlignmentsExit
#   AlignmentsViewerUpdate
#   AlignmentsTestSliceControls
#TBD: change the name of these to be AlignmentsShow...
#   AlignmentsViewerShowSliceControls
#   AlignmentsViewerHideSliceControls
#   AlignmentsSetRegTabState
#   AlignmentsTestHead
#   AlignmentsMainFileCloseUpdated
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC AlignmentsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsInit {} {
    global Matrix Module Volume View Slice

    # Define Tabs
    set m Alignments
    set Module($m,row1List) "Help Props Manual Auto"
    set Module($m,row1Name) "{Help} {Props} {Manual} {Auto}"
    set Module($m,row1,tab) Manual

    # Module Summary Info
    set Module($m,overview) "Edit transformation matrices to move volumes/models. Use the Auto tab perform registrations using embedded algorithms. Select the FidAlign option to select corresponding points on two volumes so as to get a coarse alignment of the two volumes. Use the MI option to perform registration by mutual information."

    # Define Procedures
    set Module($m,procGUI)   AlignmentsBuildGUI
    set Module($m,procMRML)  AlignmentsUpdateMRML
    set Module($m,procVTK)   AlignmentsBuildVTK

    # Callbacks from other modules
    set Module($m,procViewerUpdate) AlignmentsViewerUpdate
    set Module($m,procMainFileCloseUpdateEntered) AlignmentsMainFileCloseUpdated
    set Module($m,fiducialsStartCallback) AlignmentsFiducialsUpdated
    
    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.3 $} {$Date: 2002/07/27 21:14:06 $}]

    # Props
    set Matrix(propertyType) Basic
    set Matrix(volumeMatrix) None

    set Matrix(autoOutput) mi-output.mrml
    set Matrix(autoInput)  mi-input.pmrml
    set Matrix(autoFast) mi-fast.txt
    set Matrix(autoSlow) mi-slow.txt
    set Matrix(autoSpeed)  Fast
    set Matrix(render)     All
    set Matrix(pid) ""
    set Matrix(volume) $Volume(idNone)
    set Matrix(refVolume) $Volume(idNone)
    set Matrix(allowAutoUndo) 0
    set Matrix(tAuto) ""
    set Matrix(mouse) Translate
    set Matrix(xHome) 0
    set Matrix(yHome) 0
    set Matrix(prevTranLR) 0.00
    set Matrix(prevTranPA) 0.00
    set Matrix(prevTranIS) 0.00
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0.00
    set Matrix(regRotIS) 0.00
    set Matrix(regRotPA) 0.00

    set Matrix(eventManager) ""    


    #Additions for the registration mechanisms in the auto tab

    #Current Alignment Mode on the Auto Tab
    set Matrix(regMech) ""

    #Create a new vtkMrmlSlicer object so that two volumes can be interacted with independent of one 
    #another. The active Slicer is initially Slicer and only set to be any other slicer when the
    #fiducial selection mode is entered
    vtkMrmlSlicer MatSlicer
    set Matrix(activeSlicer) Slicer
  
    #These are what we will name the fiducial lists for the volumes of interest
    set Matrix(FidAlignRefVolumeName) None
    set Matrix(FidAlignVolumeName) None
    
    #Create a new renderer in order to display the split view showing two volumes
    vtkRenderer matRen
    lappend Module(Renderers) matRen
 
    #Camera for the new Renderer
    set View(MatCam) [matRen GetActiveCamera]
    #add the R A S L P I actors to the new renderer
    AlignmentsAddLetterActors
  
    #Renderer visibility settings
    set Matrix(mainview,visibility) 1
    set Matrix(matricesview,visibility) 0
    set Matrix(fidSelectViewOn) 0
    set Matrix(splitScreen) 0
    set Matrix(FiducialPickEntered) 0

    #Slice visibility settings 
    #The slice actors that go in the viewRen renderer 
    set Slice(0,Back,visibility) 0
    set Slice(1,Back,visibility) 0
    set Slice(2,Back,visibility) 0

    #The slice actors that go in the matRen renderer
    set Slice(0,Fore,visibility) 0
    set Slice(1,Fore,visibility) 0
    set Slice(2,Fore,visibility) 0

    #Event bindings
    lappend Matrix(eventManager) { $Gui(fViewWin) <Motion> { AlignmentsGetCurrentView %W %x %y } }

    #Hanifa - I started working on incorporating Thin Plate Spline into this module. 
    #Currently only the interface for TPS is here, but I have commented out everything that 
    #deals with TPS since the code has not yet been plugged in. 
    set Matrix(TPSVolumeName) None
    set Matrix(TPSRefVolumeName) None
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsUpdateMRML {} {
    global Matrix Volume

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $Matrix(refVolume) ] == -1} {
        AlignmentsSetRefVolume $n
    }
    if {[lsearch $Volume(idList) $Matrix(volume) ] == -1} {
        AlignmentsSetVolume $n
    }

    # Menu of Volumes
    # ------------------------------------
    set m $Matrix(mbVolume).m
    $m delete 0 end
    # All volumes except none
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
            $m add command -label "[Volume($v,node) GetName]" \
                    -command "AlignmentsSetVolume $v"
        }
    }

    # Menu of Reference Volumes
    # ------------------------------------
    set m $Matrix(mbRefVolume).m
    $m delete 0 end
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
            $m add command -label "[Volume($v,node) GetName]" \
                    -command "AlignmentsSetRefVolume $v"
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildGUI {} {
    global Gui Matrix Module Volume Fiducials

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Props
    #   Top
    #     Active
    #     Type
    #   Bot
    #     Basic
    #     Advanced
    # Manual
    # Auto
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
    Description by tab:<BR>
    <UL>
    <LI><B>Props:
    </B> Directly set the matrix. The line of 16 numbers are in order of the first row, second row, etc. Click <B>Advanced</B> to copy one of the matrices derived from the header of a volume.

    <BR>
    <LI><B>Manual:
    </B> Set the matrix using manual rigid registration with 6 degrees of freedom (3 translation, 3 rotation). Either move the sliders, or click the left mouse button on a slice window and drag it.  The <B>Mouse Action</B> buttons indicate whether to translate or rotate the volume in the Slice windows.  

    <BR><BR><B>TIP:</B> The <B>Render</B> buttons indicate which windows to render as you move the sliders or mouse.  Rendering just one slice is much faster. 
    
    <BR>
    <LI><B>Auto:
    </B> Automatic and semi-automatic registration mechanisms. The <B>FidAlign</B> tab allows selection of corresponding fiducial points on the volume to move and the reference volume. Once this button is pressed, the screen is split into half and the volume to move is displayed in the screen on the right while the reference volume is displayed on the left. The screen color indicates which screen is the active one (yellow) and which one is the inactive one (blue). The active screen is set either by the motion of the mouse or the radio buttons found on the control panel. It is important to note that only the control panel for the active volume is shown. In the FidAlign mode, pick at least three corresponding points on each of the volumes and click the apply button to obtain a coarse registration of the two volumes. Click the cancel button to exit the fiducial selection mode. The matrix is set to the transformation matrix that was used to coarsly align the volume to move with the reference volume. This matrix can be set as the active matrix for MI or it can be manually adjusted using the sliders on the manual tab. The <B>TPS</B> button will be used to access registration using the thin plate spline method in the future. It is currently not available in this version. Clicking on the <B>MI<\B> button performs automatic registration using the method of Mutual Information (MI). This will set the matrix to the transformation matrix needed to align the <B>Volume to Move</B> with the <B>Reference Vol.</B>.<BR><B>TIP:</B> Set the <B>Run Speed</B> to <I>Fast</I> if the 2 volumes are already roughly aligned. Click on the <I>View color correspondence between the two overlayed images</I> button to visually assess the quality of registration. When the reference volume(red) and the volume to move(blue) overlap, they form a pink color.  
    </UL>"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Alignments $help
    MainHelpBuildGUI  Alignments


    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(Alignments,fProps)
    set f $fProps

    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 300
    pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Props->Bot frame
    #-------------------------------------------
    set f $fProps.fBot

    foreach type "Basic Advanced" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set Matrix(f${type}) $f.f${type}
    }
    raise $Matrix(fBasic)

    #-------------------------------------------
    # Props->Top frame
    #-------------------------------------------
    set f $fProps.fTop

    frame $f.fActive -bg $Gui(backdrop)
    frame $f.fType   -bg $Gui(backdrop)
    pack $f.fActive $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Props->Top->Active frame
    #-------------------------------------------
    set f $fProps.fTop.fActive

    eval {label $f.lActive -text "Active Matrix: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Props->Top->Type frame
    #-------------------------------------------
    set f $fProps.fTop.fType

    eval {label $f.l -text "Properties:"} $Gui(BLA)
    frame $f.f -bg $Gui(backdrop)
    foreach p "Basic Advanced" {
        eval {radiobutton $f.f.r$p \
                -text "$p" -command "AlignmentsSetPropertyType" \
                -variable Matrix(propertyType) -value $p -width 8 \
                -indicatoron 0} $Gui(WCA)
        pack $f.f.r$p -side left -padx 0
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Props->Bot->Basic frame
    #-------------------------------------------
    set f $fProps.fBot.fBasic

    frame $f.fName    -bg $Gui(activeWorkspace)
    frame $f.fMatrix  -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fName $f.fMatrix $f.fApply \
            -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Props->Bot->Advanced frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced

    frame $f.fDesc    -bg $Gui(activeWorkspace)
    frame $f.fVolume  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fDesc $f.fVolume $f.fApply \
            -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Props->Bot->Basic->Name frame
    #-------------------------------------------
    set f $fProps.fBot.fBasic.fName

    eval {label $f.l -text "Name:" } $Gui(WLA)
    eval {entry $f.e -textvariable Matrix(name)} $Gui(WEA)
    pack $f.l -side top -padx $Gui(pad) -anchor w
    pack $f.e -side top -padx $Gui(pad) -anchor w -expand 1 -fill x

    #-------------------------------------------
    # Props->Bot->Basic->Matrix frame
    #-------------------------------------------
    set f $fProps.fBot.fBasic.fMatrix

    eval {label $f.l -text "Matrix:" } $Gui(WLA)
    pack $f.l -side top -padx $Gui(pad) -anchor w

    foreach row $Matrix(rows) {
        set f $fProps.fBot.fBasic.fMatrix
        frame $f.f$row -bg $Gui(activeWorkspace)
        pack $f.f$row -side top -padx $Gui(pad) -pady $Gui(pad)
        set f $f.f$row
        foreach col $Matrix(cols) {
            eval {entry $f.e$col -width 5 \
                    -textvariable \
                    Matrix(matrix,$row,$col) \
                } $Gui(WEA)
            pack $f.e$col -side left -padx $Gui(pad) -pady 2        
        }
    }  
    #-------------------------------------------
    # Props->Bot->Basic->Apply frame
    #-------------------------------------------
    set f $fProps.fBot.fBasic.fApply

    eval {button $f.bApply -text "Apply" \
            -command "AlignmentsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bCancel -text "Cancel" \
            -command "AlignmentsPropsCancel"} $Gui(WBA) {-width 8}
    grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Props->Bot->Advanced->Desc frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fDesc

    eval {label $f.l -text "Optional Description:"} $Gui(WLA)
    eval {entry $f.e -textvariable Matrix(desc)} $Gui(WEA)
    pack $f.l -side top -padx $Gui(pad) -fill x -anchor w
    pack $f.e -side top -padx $Gui(pad) -expand 1 -fill x

    #-------------------------------------------
    # Props->Bot->Advanced->Volume frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fVolume

    eval {label $f.l -text "Get Matrix from Volume"} $Gui(WTA)
    frame $f.fVolume  -bg $Gui(activeWorkspace)
    frame $f.fMatrix  -bg $Gui(activeWorkspace)
    pack $f.l $f.fVolume $f.fMatrix \
            -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Props->Bot->Advanced->Volume->Volume frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fVolume.fVolume

    eval {label $f.lActive -text "Volume: "} $Gui(WLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbActive
    lappend Volume(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Props->Bot->Advanced->Volume->Matrix frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fVolume.fMatrix

    # Menu of Alignments
    # ------------------------------------
    eval {label $f.l -text "Type of Matrix: "} $Gui(WLA)
    eval {menubutton $f.mb -text "$Matrix(volumeMatrix)" -relief raised -bd 2 -width 20 \
            -menu $f.mb.m} $Gui(WMBA)
    set Matrix(mbVolumeMatrix) $f.mb
    eval {menu $f.mb.m} $Gui(WMA)
    pack $f.l $f.mb -side left -pady $Gui(pad) -padx $Gui(pad)

    foreach v "None ScaledIJK->RAS RAS->IJK RAS->VTK" {
        $f.mb.m add command -label "$v" -command "AlignmentsSetVolumeMatrix $v"
    }

    #-------------------------------------------
    # Props->Bot->Advanced->Apply frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fApply

    eval {button $f.bApply -text "Apply" \
            -command "AlignmentsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bCancel -text "Cancel" \
            -command "AlignmentsPropsCancel"} $Gui(WBA) {-width 8}
    grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Manual frame
    #-------------------------------------------
    set fManual $Module(Alignments,fManual)
    set f $fManual

    # Frames
    frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fRender    -bg $Gui(activeWorkspace)
    frame $f.fTranslate -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fRotate    -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fBtns      -bg $Gui(activeWorkspace)
    frame $f.fMouse    -bg $Gui(activeWorkspace)
    pack $f.fActive $f.fRender  $f.fTranslate $f.fRotate $f.fBtns $f.fMouse\
            -side top -pady 4 -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Manual->Btns frame
    #-------------------------------------------
    set f $fManual.fBtns

    eval {button $f.bIdentity -text "Identity" \
            -command "AlignmentsIdentity; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bInvert -text "Invert" \
            -command "AlignmentsInvert; RenderAll"} $Gui(WBA) {-width 8}
    grid $f.bIdentity $f.bInvert -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Manual->Active frame
    #-------------------------------------------
    set f $fManual.fActive

    eval {label $f.lActive -text "Active Matrix: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Manual->Render frame
    #-------------------------------------------
    set f $fManual.fRender

    set modes "Active Slices All"
    set names "{1 Slice} {3 Slices} {3D}"

    eval {label $f.l -text "Render:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    foreach mode $modes name $names {
        eval {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
                -text "$name" -variable Matrix(render) -value $mode \
                -indicatoron 0} $Gui(WCA)
        pack $f.f.r$mode -side left -padx 0 -pady 0
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Manual->Translate frame
    #-------------------------------------------
    set f $fManual.fTranslate

    eval {label $f.lTitle -text "Translation (mm)"} $Gui(WLA)
    grid $f.lTitle -columnspan 3 -pady $Gui(pad) -padx 1 

    foreach slider "LR PA IS" {

        eval {label $f.l${slider} -text "${slider} : "} $Gui(WLA)

        eval {entry $f.e${slider} -textvariable Matrix(regTran${slider}) \
                -width 7} $Gui(WEA)
          bind $f.e${slider} <Return> \
                "AlignmentsManualTranslate regTran${slider}"
        bind $f.e${slider} <FocusOut> \
                "AlignmentsManualTranslate regTran${slider}"

        eval {scale $f.s${slider} -from -240 -to 240 -length 120 \
                -command "AlignmentsManualTranslate regTran${slider}" \
                -variable Matrix(regTran${slider}) -resolution 0.01 -digits 5} $Gui(WSA)
        bind $f.s${slider} <Leave> "AlignmentsManualTranslate regTran$slider"
        
        grid $f.l${slider} $f.e${slider} $f.s${slider} -pady 2
    }

    #-------------------------------------------
    # Manual->Rotate frame
    #-------------------------------------------
    set f $fManual.fRotate

    eval {label $f.lTitle -text "Rotation (deg)"} $Gui(WLA)
    grid $f.lTitle -columnspan 3 -pady $Gui(pad) -padx 1 

    foreach slider "LR PA IS" {

        eval {label $f.l${slider} -text "${slider} : "} $Gui(WLA)

        eval {entry $f.e${slider} -textvariable Matrix(regRot${slider}) \
                -width 7} $Gui(WEA)
        bind $f.e${slider} <Return> \
                "AlignmentsManualRotate regRot${slider}"
        bind $f.e${slider} <FocusOut> \
                "AlignmentsManualRotate regRot${slider}"

        eval {scale $f.s${slider} -from -180 -to 180 -length 120 \
                -command "AlignmentsManualRotate regRot${slider}" \
                -variable Matrix(regRot${slider}) -resolution 0.01 -digits 5} $Gui(WSA)

        grid $f.l${slider} $f.e${slider} $f.s${slider} -pady 2
    }

    #-------------------------------------------
    # Manual->Mouse Frame
    #-------------------------------------------
    set f $fManual.fMouse

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -pady $Gui(pad) -padx 1

    eval {label $f.fTitle.l -text "Mouse Action: "} $Gui(WLA)
    pack $f.fTitle.l

    foreach text "Translate Rotate" value "Translate Rotate" \
            width "10 7" {
        eval {radiobutton $f.fBtns.rSpeed$value -width $width \
                -text "$text" -value "$value" -variable Matrix(mouse) \
                -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rSpeed$value -side left -padx 0 -pady 0
        TooltipAdd  $f.fBtns.rSpeed$value  \
                "$value volumes in the Slice Window"

    }

    #-------------------------------------------
    # Auto frame
    #-------------------------------------------
    set fAuto $Module(Alignments,fAuto)
    set f $fAuto
 
    #Frames
    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fMid -bg $Gui(activeWorkspace) -relief sunken -bd 2
    frame $f.fBot  -bg $Gui(activeWorkspace) -height 1000
    pack $f.fTop $f.fMid $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
 
    #-------------------------------------------
    # Auto->Bot frame
    #-------------------------------------------
    set f $fAuto.fBot

    #Registration options
    foreach type "AlignBegin FidAlign TPS MI" {
        frame $f.f${type} -bg $Gui(activeWorkspace)
        place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
        set Matrix(f${type}) $f.f${type}
    }
    
    raise $Matrix(fAlignBegin) 

    #-------------------------------------------
    # Auto->Top frame
    #-------------------------------------------
    set f $fAuto.fTop

    frame $f.fActive -bg $Gui(backdrop)
    frame $f.fVolumes   -bg $Gui(backdrop)
    pack $f.fActive $f.fVolumes -side top -padx $Gui(pad) -pady 2

    #-------------------------------------------
    # Auto->Top->Active frame
    #-------------------------------------------
    set f $fAuto.fTop.fActive

    eval {label $f.lActive -text "Active Matrix: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Auto->Volumes frame
    #-------------------------------------------
    set f $fAuto.fTop.fVolumes

    eval {label $f.lVolume -text "Volume to Move:"} $Gui(BLA)
    eval {menubutton $f.mbVolume -text "None" \
            -relief raised -bd 2 -width 15 -menu $f.mbVolume.m} $Gui(WMBA)
    eval {menu $f.mbVolume.m} $Gui(WMA)

    eval {label $f.lRefVolume -text "Reference Volume:"} $Gui(BLA)
    eval {menubutton $f.mbRefVolume -text "None" \
            -relief raised -bd 2 -width 15 -menu $f.mbRefVolume.m} $Gui(WMBA)
    eval {menu $f.mbRefVolume.m} $Gui(WMA)
    grid $f.lVolume $f.mbVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
    grid $f.mbVolume -sticky e
    grid $f.lRefVolume $f.mbRefVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
    grid $f.mbRefVolume -sticky e

    # Append widgets to list that gets refreshed during UpdateMRML
    set Matrix(mbVolume) $f.mbVolume
    set Matrix(mbRefVolume) $f.mbRefVolume
    
    #-------------------------------------------
    # Auto->Mid frame
    #------------------------------------------
    set f $fAuto.fMid
    
    frame $f.fType -bg $Gui(activeWorkspace) -bg $Gui(backdrop) -relief sunken 
    pack $f.fType -side top -fill x 

    #-------------------------------------------
    # Auto->Mid->Type frame
    #-------------------------------------------
    set f $fAuto.fMid.fType

    eval {label $f.l -text "Registration Mechanism:"} $Gui(BLA) 
    frame $f.f -bg $Gui(backdrop)
    foreach mech "FidAlign TPS MI" {
        eval {radiobutton $f.f.r$mech \
            -text "$mech" -command "AlignmentsSetRegistrationMech" \
            -variable Matrix(regMech) -value $mech -width 10 \
            -indicatoron 0} $Gui(WCA)
    set Matrix(r${mech}) $f.f.r$mech
    pack $f.f.r$mech -side left -padx 0
    }
    pack $f.l -side top
    pack $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Auto->Bot->AlignBegin Frame
    #-------------------------------------------
    set f $fAuto.fBot.fAlignBegin

    eval {label $f.fTitle -text "Select a Registration mode\n
    using the buttons above"} $Gui(WLA)
    
    pack $f.fTitle -pady 60

    frame $f.fColorCorresp -bg $Gui(activeWorkspace) -bd 2
    pack $f.fColorCorresp -fill x -side top -padx 0 -pady 0
    set f $f.fColorCorresp  
        
    eval {checkbutton $f.cColorCorresp -variable Matrix(colorCorresp) \
          -text "View the color correspondence \n between the two overlayed images" -command "AlignmentsSetColorCorrespondence" \
          -indicatoron 0} $Gui(WCA)
    
    pack $f.cColorCorresp -pady 20


    #-------------------------------------------
    # Auto->Bot->FidAlign Frame
    #-------------------------------------------
    set f $fAuto.fBot.fFidAlign
 
    frame $f.fFidViewParams -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fFidLists -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fApply -bg $Gui(activeWorkspace)  
   
    pack $f.fFidViewParams $f.fFidLists $f.fApply \
        -side top -fill x -pady $Gui(pad)
 
    #-------------------------------------------
    # Auto->Bot->FidViewParams Frame
    #-------------------------------------------
    set f $fAuto.fBot.fFidAlign.fFidViewParams

    eval {label $f.fTitle -text "Viewing Parameters: "} $Gui(WLA)
    
    eval {checkbutton $f.cLeftView -variable Matrix(mainview,visibility) \
          -text "None" -command "AlignmentsUpdateMainViewVisibility; AlignmentsViewerUpdate" \
          -indicatoron 1} $Gui(WCA)
    
    eval {checkbutton $f.cRightView -variable Matrix(matricesview,visibility) \
          -text "None" -command "AlignmentsUpdateFidSelectViewVisibility; AlignmentsViewerUpdate" \
          -indicatoron 1} $Gui(WCA)

    grid $f.fTitle -pady 2
    grid $f.cLeftView $f.cRightView -sticky w
    
    set Matrix(viewRefVol) $f.cLeftView
    set Matrix(viewVolToMove) $f.cRightView    
 
    #-------------------------------------------
    # Auto->Bot->FidLists Frame
    #-------------------------------------------
    set f $fAuto.fBot.fFidAlign.fFidLists 

    eval {label $f.l -text "Fiducial XYZ Coordinates"} $Gui(WTA)
    eval {button $f.bhow -width 38 -text "How To Add Fiducials"} $Gui(WBA)
    
    frame $f.fListNames -bg $Gui(activeWorkspace)
    #These are of button type instead of labels because they made the GUI look better -pressing them does nothing
    eval {button $f.fListNames.bFidRefVolume -text $Matrix(FidAlignRefVolumeName) \
              -relief raised -bd 2 -width 14} $Gui(WMBA)
    
    eval {button $f.fListNames.bFidVolume -text $Matrix(FidAlignVolumeName) \
              -relief raised -bd 2 -width 14} $Gui(WMBA)

    pack $f.l $f.bhow $f.fListNames -side top -pady 2 -padx $Gui(pad)
    grid $f.fListNames.bFidRefVolume $f.fListNames.bFidVolume

    #Save paths so that the names of the buttons can be configured when the volumes are chosen
    set Matrix(bFidVolume) $f.fListNames.bFidVolume
    set Matrix(bFidRefVolume) $f.fListNames.bFidRefVolume

    TooltipAdd $f.bhow "$Fiducials(howto)"
    
    #The frames in which the point boxes listing the fiducial coordinates will be displayed
    frame $f.fPointBoxes -bg $Gui(activeWorkspace)
    pack $f.fPointBoxes -side top -pady 2

    set Matrix(FidPointBoxes) $f.fPointBoxes
    frame $Matrix(FidPointBoxes).fRefVolumePoints -bg $Gui(activeWorkspace)
    frame $Matrix(FidPointBoxes).fVolumePoints -bg $Gui(activeWorkspace)
    grid $Matrix(FidPointBoxes).fRefVolumePoints $Matrix(FidPointBoxes).fVolumePoints    

    #Save the paths to the frames so we can add/remove the boxes to them when fiducials are updated
    set Matrix(FidRefVolPointBoxes) $Matrix(FidPointBoxes).fRefVolumePoints
    set Matrix(FidVolPointBoxes) $Matrix(FidPointBoxes).fVolumePoints

    #-------------------------------------------
    # Auto->Bot->FidLists->Apply frame
    #-------------------------------------------
    set f $fAuto.fBot.fFidAlign.fApply

    #Align the Datasets based on the fiducial sets chosen
    DevAddButton $f.bApply "Apply" "AlignmentsApplyFiducialSelection" 8

    #Clear the Fiducial lists
    DevAddButton $f.bCancel "Cancel" "AlignmentsFidSelectCancel" 8
    
    grid $f.bApply $f.bCancel -padx $Gui(pad)  

    #-------------------------------------------
    # Auto->Bot->TPS Frame
    #-------------------------------------------
    set f $fAuto.fBot.fTPS
    frame $f.fTPSFidPoints -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fTPSVolume -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fTPSOptions -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fTPSOutputSliceRange -bg $Gui(activeWorkspace) -relief groove -bd 2 
    frame $f.fTPSWarp -bg $Gui(activeWorkspace) 
    frame $f.fTPSSaveVolume -bg $Gui(activeWorkspace) 
    

    pack $f.fTPSFidPoints $f.fTPSVolume $f.fTPSOptions $f.fTPSOutputSliceRange  $f.fTPSWarp $f.fTPSSaveVolume\
    -side top -pady 2 -padx $Gui(pad) -fill x
 
    #-------------------------------------------
    # Auto->Bot->TPS->Volume frame
    #-------------------------------------------
    set f $fAuto.fBot.fTPS.fTPSVolume
    
    #Allow the user to create a new volume to store the warped volume.
    frame $f.fTPSNewVolume -bg $Gui(activeWorkspace)
    pack $f.fTPSNewVolume -side top -padx $Gui(pad) -pady $Gui(pad) -anchor w

    eval {label $f.fTPSNewVolume.lLabel -text "Generate New Volume Using TPS:"} $Gui(WLA) -foreground blue   
    pack  $f.fTPSNewVolume.lLabel -pady 2 -anchor w

    eval {label $f.fTPSNewVolume.lName -text "Name: "} $Gui(WLA)     
    eval {entry $f.fTPSNewVolume.eName -textvariable $Matrix(TPSVolumeName) -width 13} $Gui(WEA)
    pack $f.fTPSNewVolume.lName -side left -pady $Gui(pad) 
    pack $f.fTPSNewVolume.eName -side left -padx $Gui(pad) -expand 1 -fill x
 

    #Allow the user to pick a target volume in which to save the warped volume.
    frame $f.fTPSTargetVolume -bg $Gui(activeWorkspace)

    #In the callback command make sure you add this to menubutton "target volume" 
    eval {label $f.fTPSTargetVolume.lTargetVolume -text "Target Volume: "} $Gui(WLA)
    eval {menubutton $f.fTPSTargetVolume.mbTargetVolume -text "Matrix(TpsTargetVolume)" \
          -relief raised -bd 2 -width 20 \
          -menu $f.fTPSTargetVolume.mbTargetVolume.m} $Gui(WMBA)
    eval {menu $f.fTPSTargetVolume.mbTargetVolume.m} $Gui(WMA)

    pack $f.fTPSTargetVolume -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.fTPSTargetVolume.lTargetVolume $f.fTPSTargetVolume.mbTargetVolume -side left -padx 1
    

    #-------------------------------------------
    # TPS->TPS Options frame
    #-------------------------------------------
    set f  $fAuto.fBot.fTPS.fTPSOptions
    
    eval {label $f.lTitle -text "TPS Parameters:"} $Gui(WLA) -foreground blue
    pack $f.lTitle -padx $Gui(pad) -pady 4 -anchor w 
    set Matrix(gui,test) $f.lTitle

    ###Basis###
    frame $f.fBasisMode -bg $Gui(activeWorkspace)    
    eval {label $f.fBasisMode.lBasisMode -text "Basis Function: "} $Gui(WLA)
    eval {menubutton $f.fBasisMode.mbBasisMode -text "Matrix(TpsBasisMode)" \
          -relief raised -bd 2 -width 20 \
          -menu $f.fBasisMode.mbBasisMode.m} $Gui(WMBA)
    eval {menu $f.fBasisMode.mbBasisMode.m} $Gui(WMA)

    pack $f.fBasisMode -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.fBasisMode.lBasisMode $f.fBasisMode.mbBasisMode -side left -padx 1

    # Add menu items
    foreach mode {r r*r} {
    $f.fBasisMode.mbBasisMode.m add command -label $mode \
        -command "AlignmentsWhichCommand; set Matrix(TpsBasisMode) $mode; $f.fBasisMode.mbBasisMode config -text $mode "
    }

    # save menubutton for config
    # set Matrix(gui,mbBasisMode) $f.mbBasisMode

    ###Fidelity###
    frame $f.fFidelity -bg $Gui(activeWorkspace) 
    eval {label $f.fFidelity.lFidelity -text "Fidelity: "} $Gui(WLA)
    eval {entry $f.fFidelity.eFidelity -textvariable Matrix(TpsFidelity) \
    -width 7} $Gui(WEA)
    bind $f.fFidelity.eFidelity <Return> \
    "AlignmentsWhichCommand"
    bind $f.fFidelity.eFidelity <FocusOut> \
    "AlignmentsWhichCommand"
    
    eval {scale $f.fFidelity.sFidelity -from 0 -to 1 -length 100 \
          -command "AlignmentsWhichCommand" \
          -variable Matrix(TpsFidelity) -resolution 0.01 -digits 3} $Gui(WSA)

    pack $f.fFidelity -side top -pady 2 -padx $Gui(pad)
    grid $f.fFidelity.lFidelity $f.fFidelity.eFidelity $f.fFidelity.sFidelity -padx 1 -pady 2
    
    #-------------------------------------------
    # Auto->Bot->TPS->TPSOutputSliceRange frame
    #-------------------------------------------
    set f  $fAuto.fBot.fTPS.fTPSOutputSliceRange 
    #Output slice range
    
    eval {label $f.lRange -text "Output Slice Range: "} $Gui(WLA) -foreground blue
    pack $f.lRange -padx $Gui(pad) -pady $Gui(pad) -anchor w

    #Beginning of Range
    frame $f.fFirstSlice -bg $Gui(activeWorkspace)
    eval {label $f.fFirstSlice.lStartRange -text "First Slice: "} $Gui(WLA)
 
    eval {entry $f.fFirstSlice.eOutputRange -textvariable Matrix(TpsStartSliceRange) \
         -width 4} $Gui(WEA)
    bind $f.fFirstSlice.eOutputRange <Return> \
    "AlignmentsWhichCommand"
    bind $f.fFirstSlice.eOutputRange <FocusOut> \
    "AlignmentsWhichCommand"

    #TBD:change the range to be what the range is for the volume
    eval {scale $f.fFirstSlice.sOutputRange -from 0 -to 100 -length 100 \
          -command "AlignmentsWhichCommand" \
          -variable Matrix(TpsStartSliceRange) -resolution 1} $Gui(WSA)
    pack $f.fFirstSlice -side top -padx $Gui(pad) -anchor w
    grid $f.fFirstSlice.lStartRange $f.fFirstSlice.eOutputRange \
    $f.fFirstSlice.sOutputRange -padx 1 -pady 1

    #End of Range
    frame $f.fLastSlice -bg $Gui(activeWorkspace)
    eval {label $f.fLastSlice.lEndRange -text "Last Slice: "} $Gui(WLA)
 
    eval {entry $f.fLastSlice.eOutputRange -textvariable Matrix(TpsEndSliceRange) \
         -width 4} $Gui(WEA)
    bind $f.fLastSlice.eOutputRange <Return> \
    "AlignmentsWhichCommand"
    bind $f.fLastSlice.eOutputRange <FocusOut> \
    "AlignmentsWhichCommand"

    #TBD:change the range 
    eval {scale $f.fLastSlice.sOutputRange -from 0 -to 100 -length 100 \
          -command "AlignmentsWhichCommand" \
          -variable Matrix(TpsEndSliceRange) -resolution 1} $Gui(WSA)
    pack $f.fLastSlice -padx $Gui(pad) -anchor w 
    grid $f.fLastSlice.lEndRange $f.fLastSlice.eOutputRange \
    $f.fLastSlice.sOutputRange -padx 1  

    #-------------------------------------------
    # Auto->Bot->TPS->TPSFidPointsFrame
    #-------------------------------------------
    #These are separate from the Points on the FidAlign Screen 
    #since the two methods are independent of one another

    set f $fAuto.fBot.fTPS.fTPSFidPoints

    eval {label $f.l -text "Fiducial XYZ Coordinates for TPS:"} $Gui(WTA) -foreground blue
    eval {button $f.bhow -width 38 -text "How To Add Fiducials"} $Gui(WBA)
    
    frame $f.fListNames -bg $Gui(activeWorkspace)
    #I made these buttons just because they looked nicer
    eval {button $f.fListNames.bFidRefVolume -text $Matrix(FidAlignRefVolumeName) \
              -relief raised -bd 2 -width 14} $Gui(WMBA)
    
    eval {button $f.fListNames.bFidVolume -text $Matrix(FidAlignVolumeName) \
              -relief raised -bd 2 -width 14} $Gui(WMBA)

    pack $f.l $f.bhow $f.fListNames -side top -pady 2 -padx $Gui(pad)
    grid $f.fListNames.bFidRefVolume $f.fListNames.bFidVolume

    #Save paths for later
    set Matrix(bTPSVolume) $f.fListNames.bFidVolume
    set Matrix(bTPSRefVolume) $f.fListNames.bFidRefVolume

    TooltipAdd $f.bhow "$Fiducials(howto)"
    
    frame $f.fPointBoxes -bg $Gui(activeWorkspace)
    pack $f.fPointBoxes -side top -pady 2 -padx $Gui(pad)

    set Matrix(TPSPointBoxes) $f.fPointBoxes
    frame $Matrix(TPSPointBoxes).fRefVolumePoints -bg $Gui(activeWorkspace)
    frame $Matrix(TPSPointBoxes).fVolumePoints -bg $Gui(activeWorkspace)
    grid $Matrix(TPSPointBoxes).fRefVolumePoints $Matrix(TPSPointBoxes).fVolumePoints    

    set Matrix(TPSRefVolPointBoxes) $Matrix(TPSPointBoxes).fRefVolumePoints
    set Matrix(TPSVolPointBoxes) $Matrix(TPSPointBoxes).fVolumePoints

    #-------------------------------------------
    # Auto->Bot->TPS->TPSWarp Frame
    #-------------------------------------------
    set f   $fAuto.fBot.fTPS.fTPSWarp     

    #TBD: Do you Really need a cancel button?
    eval {button $f.bWarp -text "Warp" \
          -command "AlignmentsWhichCommand"} $Gui(WBA) {-width 10}
    eval {button $f.bCancel -text "Cancel" \
          -command "AlignmentsFidSelectCancel"} $Gui(WBA) {-width 10}
    grid $f.bWarp $f.bCancel -padx $Gui(pad) -pady $Gui(pad)
    
    #-------------------------------------------
    # Auto->Bot->TPS->TPSSaveVolume Frame
    #-------------------------------------------
    set f $fAuto.fBot.fTPS.fTPSSaveVolume

    #TBD:should you have something here that says "as target volume?"
    #TBD:Include an error that pops up if the user has not selected the fiducial points
    eval {button $f.bSave -text "Save Warped Volume" \
       -command "AlignmentsWhichCommand"} $Gui(WBA) {-width 20}
    pack $f.bSave -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Auto->Bot->MI Frame
    #-------------------------------------------
    set f $fAuto.fBot.fMI

    frame $f.fDesc    -bg $Gui(activeWorkspace) 
    frame $f.fSpeed   -bg $Gui(activeWorkspace) -relief groove -bd 2
    frame $f.fRun     -bg $Gui(activeWorkspace)

    pack $f.fDesc $f.fSpeed $f.fRun -pady $Gui(pad) \

    #-------------------------------------------
    # Auto->Bot->MI->Desc frame
    #-------------------------------------------
    set f $fAuto.fBot.fMI.fDesc

    eval {label $f.l -text "\Press 'Run' to start the program \nthat performs automatic registration\nby Mutual Information.\n\Your manual registration is used\n\ as an initial pose.\ "} $Gui(WLA)
    pack $f.l -pady $Gui(pad)
    
    #-------------------------------------------
    # Auto->Bot->Mi->Speed Frame
    #-------------------------------------------
    set f $fAuto.fBot.fMI.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lSpeed -text "Run Speed:"} $Gui(WLA)
    pack $f.fTitle.lSpeed

    foreach text "Fast Slow" value "Fast Slow" \
        width "6 6" {
        eval {radiobutton $f.fBtns.rSpeed$value -width $width \
            -text "$text" -value "$value" -variable Matrix(autoSpeed) \
            -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rSpeed$value -side left -padx 4 -pady 2
    }

    #-------------------------------------------
    # Auto->Bot->MI->Run frame
    #-------------------------------------------
    set f $fAuto.fBot.fMI.fRun

    foreach str "Run Cancel Undo" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
            -command "AlignmentsAuto$str"} $Gui(WBA)
        set Matrix(b$str) $f.b$str
    }
    pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
    set Matrix(bUndo) $f.bUndo
    $f.bUndo configure -state disabled

   #These are the new controls that are used to switch between volumes once the screen is split into two.
    AlignmentsBuildSliceControls Slicer
    AlignmentsTestSliceControls
    AlignmentsBuildActiveVolSelectControls
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildVTK
#
# These vtk objects needed to be created in order to display the volumes separately in 
# two different renderers when wanting to use fiducial alignment. What happens is when
# the user enters the fiducial selection mode, the vtkMrmlSlicer object "slicer" sets 
# the reference volume as the foreground image and the volume to move as the background 
# image. Each of the two volumes are then are texture mapped onto three new slice actors. 
# What was set as the foreground image (the reference volume) is texture mapped onto the
# three slice actors that are added to viewRen (the left renderer) and what was set as the 
# background image (the volume to move) is texture mapped onto the three slice actors 
# that are added to matRen (the right renderer). I originally did this just to separate 
# the two volumes, but it turned out that although they were being displayed separately,
# they were still being reformatted by the same matrix. So later, I added the new
# vtkMrmlSlicer object that allowed each of the volumes to have separate reformat matrices.
#
# Note: Since there are now two separate vtkMrmlSlicer objects, there is not a need to
# texture map only the foreground and background images. Each slicer object can take in
# the entire output of slicer when the split view was entered. 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildVTK {} {
    global Matrix Slice View Volume
    
   
    foreach layer "Back Fore" {    
        if {$layer == "Fore" } { 
           AlignmentsSetActiveSlicer Slicer
         } else {        
            AlignmentsSetActiveSlicer MatSlicer
        }

     foreach s $Slice(idList) { 
         vtkImageMapToColors Matrix(Mapper$s$layer)
         MainSlicesBuildVTKForSliceActor $s,$layer
         Slice($s,$layer,planeActor) SetUserMatrix [$Matrix(activeSlicer) GetReformatMatrix $s]

         Matrix(Mapper$s$layer) SetOutputFormatToRGBA
    
         switch $layer {
         "Fore" {
             Matrix(Mapper$s$layer) SetInput [[$Matrix(activeSlicer) GetForeReformat3DView $s] GetOutput]
         }
         "Back" {
             Matrix(Mapper$s$layer) SetInput [[$Matrix(activeSlicer) GetBackReformat3DView $s] GetOutput]
         }
         }       
         
         Slice($s,$layer,texture) SetInput [Matrix(Mapper$s$layer) GetOutput]
         Slice($s,$layer,planeMapper) SetInput [Slice($s,$layer,planeSource) GetOutput]   
         Slice($s,$layer,planeActor) SetTexture Slice($s,$layer,texture)
         
     }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetVolumeMatrix
#
# Copy one of the matrices derived from the header of a volume
# .ARGS
# type The type of matrix to be copied - ie. RAS->IJK, RAS->VTK, ScaledIJK->RAS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetVolumeMatrix {type} {
    global Matrix

    set Matrix(volumeMatrix) $type

    # update the button text
    $Matrix(mbVolumeMatrix) config -text $type
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsIdentity
#
#  Resets the transformation to the identity, i.e. to the transform with
#  no effect.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsIdentity {} {
    global Matrix
    
    set m $Matrix(activeID)
    if {$m == ""} {return}
    
    [Matrix($m,node) GetTransform] Identity
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0.00
    set Matrix(regRotIS) 0.00
    set Matrix(regRotPA) 0.00
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsInvert
#
# Replaces the current transform with its inverse.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsInvert {} {
    global Matrix
    
    set m $Matrix(activeID)
    if {$m == ""} {return}
    
    [Matrix($m,node) GetTransform] Inverse
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0.00
    set Matrix(regRotIS) 0.00
    set Matrix(regRotPA) 0.00
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetPropertyType
# 
# Display the basic or advanced frame depending on user selection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetPropertyType {} {
    global Matrix
    
    raise $Matrix(f$Matrix(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsPropsApply
# 
# Apply the Transform
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsPropsApply {} {
    global Matrix Label Module Mrml Volume

    # none?
    if {$Matrix(activeID) != "NEW" && $Matrix(idList) == ""} {
        tk_messageBox -message "Please create a matrix first from the Data panel"
        Tab Data row1 List
    }
    
    # Validate Input
    AlignmentsValidateMatrix

    # Validate name
    if {$Matrix(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this matrix."
        return
    }
    if {[ValidateName $Matrix(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    set m $Matrix(activeID)
    if {$m == ""} {return}

    if {$m == "NEW"} {
        set i $Matrix(nextID)
        incr Matrix(nextID)
        lappend Matrix(idList) $i
        vtkMrmlMatrixNode Matrix($i,node)
        set n Matrix($i,node)
        $n SetID $i

        # These get set down below, but we need them before MainUpdateMRML
        $n SetName $Matrix(name)

        Mrml(dataTree) AddItem $n
        MainUpdateMRML
        set Matrix(freeze) 0
        MainAlignmentsSetActive $i
        set m $i
    }

    # If user wants to use the matrix from a volume, set it here
    set v $Volume(activeID)
    switch $Matrix(volumeMatrix) {
        "None" {}
        "ScaledIJK->RAS" {
            AlignmentsSetMatrix [Volume($v,node) GetPositionMatrix]
        }
        "RAS->IJK" {
            AlignmentsSetMatrix [Volume($v,node) GetRasToIjkMatrix]
        }
        "RAS->VTK" {
            AlignmentsSetMatrix [Volume($v,node) GetRasToVtkMatrix]
        }
    }

    Matrix($m,node) SetName $Matrix(name)
    AlignmentsSetMatrixIntoNode $m

    # Return to Basic
    if {$Matrix(propertyType) == "Advanced"} {
        set Matrix(propertyType) Basic
        AlignmentsSetPropertyType
    }

    # If tabs are frozen, then 
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
    
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsPropsCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsPropsCancel {} {
    global Matrix Module

    # Reset props
    set m $Matrix(activeID)
    if {$m == "NEW"} {
        set m [lindex $Matrix(idList) 0]
    }
    set Matrix(freeze) 0
    MainAlignmentsSetActive $m

    # Return to Basic
    if {$Matrix(propertyType) == "Advanced"} {
        set Matrix(propertyType) Basic
        AlignmentsSetPropertyType
    }

    # Unfreeze
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsManualTranslate
#
# Adjusts the "translation part" of the transformation matrix.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsManualTranslate {param {value ""}} {
    global Matrix

    # Value is blank if used entry field instead of slider
    if {$value == ""} {
        set value $Matrix($param)
    } else {
        set Matrix($param) $value
    }

    # Validate input
    if {[ValidateFloat $value] == 0} {
        tk_messageBox -message "$value must be a floating point number"
        return
    }

    # If there is no active transform, then do nothing
    set t $Matrix(activeID)
    if {$t == "" || $t == "NEW"} {return}

    # Transfer values from GUI to active transform
    set tran [Matrix($t,node) GetTransform]
    set mat  [$tran GetMatrix]
    
    switch $param {
        "regTranLR" {
            set oldVal [$mat GetElement 0 3]
        }
        "regTranPA" {
            set oldVal [$mat GetElement 1 3]
        }
        "regTranIS" {
            set oldVal [$mat GetElement 2 3]
        }
    }

    # Update all MRML only if the values changed
    if {$oldVal != $value} {
        switch $param {
            "regTranLR" {
                $mat SetElement 0 3 $value
            }
            "regTranPA" {
                $mat SetElement 1 3 $value
            }
            "regTranIS" {
                $mat SetElement 2 3 $value
            }
        }
        set Matrix(rotAxis) "XX"
        set Matrix(regRotLR) 0
        set Matrix(regRotIS) 0
        set Matrix(regRotPA) 0
        MainUpdateMRML
        Render$Matrix(render)
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsManualTranslateDual
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsManualTranslateDual {param1 value1 param2 value2} {
    global Matrix

    # Validate input
    if {[ValidateFloat $value1] == 0} {
        tk_messageBox -message "$value1 must be a floating point number"
        return
    }
    if {[ValidateFloat $value2] == 0} {
        tk_messageBox -message "$value2 must be a floating point number"
        return
    }

    # Value is blank if used entry field instead of slider
    set Matrix($param1) $value1
    set Matrix($param2) $value2

    # If there is no active transform, then do nothing
    set t $Matrix(activeID)
    if {$t == "" || $t == "NEW"} {return}

    # Transfer values from GUI to active transform
    set tran [Matrix($t,node) GetTransform]
    set mat  [$tran GetMatrix]
    
    switch $param1 {
        "regTranLR" {
            set oldVal1 [$mat GetElement 0 3]
        }
        "regTranPA" {
            set oldVal1 [$mat GetElement 1 3]
        }
        "regTranIS" {
            set oldVal1 [$mat GetElement 2 3]
        }
    }
    switch $param2 {
        "regTranLR" {
            set oldVal2 [$mat GetElement 0 3]
        }
        "regTranPA" {
            set oldVal2 [$mat GetElement 1 3]
        }
        "regTranIS" {
            set oldVal2 [$mat GetElement 2 3]
        }
    }

    # Update all MRML only if the values changed
    if {$oldVal1 != $value1} {
        switch $param1 {
            "regTranLR" {
                $mat SetElement 0 3 $value1
            }
            "regTranPA" {
                $mat SetElement 1 3 $value1
            }
            "regTranIS" {
                $mat SetElement 2 3 $value1
            }
        }
    }
    if {$oldVal2 != $value2} {
        switch $param2 {
            "regTranLR" {
                $mat SetElement 0 3 $value2
            }
            "regTranPA" {
                $mat SetElement 1 3 $value2
            }
            "regTranIS" {
                $mat SetElement 2 3 $value2
            }
        }
    }
    if {$oldVal2 != $value2 || $oldVal1 != $value1} {
        set Matrix(rotAxis) "XX"
        set Matrix(regRotLR) 0
        set Matrix(regRotIS) 0
        set Matrix(regRotPA) 0
        MainUpdateMRML
        if {$Matrix(render) == "All"} {
            Render3D
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsManualRotate
#
# Adjusts the "rotation part" of the transformation matrix.
# If the previous manual adjustment to the transform was a rotation
# about the same axis as the current adjustment, then only the change
# in the degrees specified is applied.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsManualRotate {param {value ""} {mouse 0}} {
    global Matrix

    # "value" is blank if used entry field instead of slider
    if {$value == ""} {
        set value $Matrix($param)
    } else {
        set Matrix($param) $value
    }

    # Validate input
    if {[ValidateFloat $value] == 0} {
        tk_messageBox -message "$value must be a floating point number"
        return
    }

    # If there is no active transform, then do nothing
    set t $Matrix(activeID)
    if {$t == "" || $t == "NEW"} {return}

    # If this is a different axis of rotation than last time,
    # then store the current transform in "Matrix(rotMatrix)"
    # so that the user's rotation can be concatenated with this matrix.
    # This way, this routine can be called with angles of 35, 40, 45,
    # and the second rotation will produce a visible change of 
    # 5 degrees instead of 40.
    #
    set axis [string range $param 6 7]
    if {$axis != $Matrix(rotAxis)} {
        [Matrix($t,node) GetTransform] GetMatrix Matrix(rotMatrix)
        set Matrix(rotAxis) $axis
        # O-out the rotation in the other 2 axes
        switch $axis {
            "LR" {
                set Matrix(regRotPA) 0
                set Matrix(regRotIS) 0
            }
            "PA" {
                set Matrix(regRotLR) 0
                set Matrix(regRotIS) 0
            }
            "IS" {
                set Matrix(regRotPA) 0
                set Matrix(regRotLR) 0
            }
        }
    }

    # Now, concatenate the rotation with the stored transform
    set tran [Matrix($t,node) GetTransform]
    $tran SetMatrix Matrix(rotMatrix)
    switch $param {
        "regRotLR" {
            $tran RotateX $value
        }
        "regRotPA" {
            $tran RotateY $value
        }
        "regRotIS" {
            $tran RotateZ $value
        }
    }

    # Only UpdateMRML if the transform changed
    set mat1 [Matrix($t,transform) GetMatrix]
    set mat2 [[Matrix($t,node) GetTransform] GetMatrix]
    set differ 0
    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            if {[$mat1 GetElement $i $j] != [$mat2 GetElement $i $j]} {
                set differ 1
            }
        }
    }
    if {$differ == 1} {
        [Matrix($t,node) GetTransform] DeepCopy $tran
        MainUpdateMRML
        if {$mouse == 1} {
            if {$Matrix(render) == "All"} {
                Render3D
            }
        } else {
            Render$Matrix(render)
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetRefVolume
# 
# .ARGS
# vtkMrmlVolumeNode v is the vtkMrmlVolumeNode to set or get
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetRefVolume {{v ""}} {
    global Matrix Volume

    if {$v == ""} {
        set v $Matrix(refVolume)
    } else {
        set Matrix(refVolume) $v
    }

    #Display what the user picked from the menu as the reference volume 
    $Matrix(mbRefVolume) config -text "[Volume($v,node) GetName]"
   
    #Set Matrix(FidAlignRefVolumeName) to be the name of the reference volume
    #This will be the name of the fiducial list for the reference volume
    set Matrix(FidAlignRefVolumeName) "[Volume($v,node) GetName]"

    #Print out what the user has set as the reference volume
    puts "this is the refVolumeName: $Matrix(FidAlignRefVolumeName)"
    
    #In the FidAlign frame, set the label on the button above the 
    #fiducial list for the reference volume
    $Matrix(bFidRefVolume) config -text "[Volume($v,node) GetName]"
 
    #In the TPS frame, set the label on the button above the fiducial 
    #list for the reference volume
    $Matrix(bTPSRefVolume) config -text "[Volume($v,node) GetName]"

    #Set the label for the checkbutton that sets the view parameters
    $Matrix(viewRefVol) config -text "[Volume($v,node) GetName]"

    #Set the label for the radio button on the new control panel
    $Matrix(rFidScActivateRefVolume) config  -text "[Volume($v,node) GetName]"
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetVolume {{v ""}} {
    global Matrix Volume

    if {$v == ""} {
        set v $Matrix(volume)
    } else {
        set Matrix(volume) $v
    }

    #Display what the user picked from the menu as the Volume to move
    $Matrix(mbVolume) config -text "[Volume($v,node) GetName]"
 
    #Set Matrix(FidAlignVolumeName) to be the name of the volume to move
    #This will be the name of the fiducial list for the volume to move
    set Matrix(FidAlignVolumeName) "[Volume($v,node) GetName]"

    #Print out what the user has set as the volume to move
    puts "this is the FidAlignVolumeName: $Matrix(FidAlignVolumeName)"
    
    #In the FidAlign frame set the label on the button above the fiducial
    #list for the volume to move
    $Matrix(bFidVolume) config -text "[Volume($v,node) GetName]"

    #In the TPS frame, set the label on the button above the fiducial list
    #for the reference volume
    $Matrix(bTPSVolume) config -text "[Volume($v,node) GetName]"

    #Set the label for the checkbutton that sets the view parameters
    $Matrix(viewVolToMove) config -text "[Volume($v,node) GetName]"

    #Set the label for the radio button on the new control panel
    $Matrix(rFidScActivateVolume) config  -text "[Volume($v,node) GetName]"
}

#-------------------------------------------------------------------------------
#        AUTO REGISTRATION
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC AlignmentsWritePseudoMrmlVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsWritePseudoMrmlVolume {fid v} {
    global Volume
    
    scan [Volume($v,node) GetDimensions] "%d %d" x y
    set filePattern [Volume($v,node) GetFilePattern]
    set filePrefix [Volume($v,node) GetFullPrefix]
    set imageRange [Volume($v,node) GetImageRange]
    set filename [format $filePattern $filePrefix [lindex $imageRange 0]]
    set headerSize [expr [file size $filename] - ($x * $y * 2)]

    puts $fid "filePrefix $filePrefix"
    puts $fid "filePattern $filePattern"
    puts $fid "headerSize $headerSize"
    puts $fid "imageRange $imageRange"
    puts $fid "dimensions [Volume($v,node) GetDimensions]"
    puts $fid "spacing [Volume($v,node) GetSpacing]"
    puts $fid "rasToIjkMatrix [Volume($v,node) GetRasToIjkMatrix]"
    puts $fid "littleEndian [Volume($v,node) GetLittleEndian]"
    puts $fid "window [Volume($v,node) GetWindow]"
    puts $fid "level [Volume($v,node) GetLevel]"

}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoRun
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoRun {} {
    global Path env Gui Matrix Volume
    
    # No-can-do on PCs
    if {$Gui(pc) == 1} {
        tk_messageBox -message "\
                Automatic registration is presently available only on UNIX systems."
        return
    }

    # Store which transform we're editing
    set t $Matrix(activeID)
    set Matrix(tAuto) $t
    if {$t == ""} {
        tk_messageBox -message "Please select a transform"
    }

    # v = ID of volume to register
    # r = ID of reference volume
    set v $Matrix(volume)
    set r $Matrix(refVolume)

    # Check that the volumes exist
    scan [Volume($v,node) GetImageRange] "%d %d" lo hi
    if {[CheckVolumeExists [Volume($v,node) GetFullPrefix] \
            [Volume($v,node) GetFilePattern] $lo $hi] != ""} {
        set str "The [Volume($v,node) GetName] volume cannot be found on disk."
        puts $str
        tk_messageBox -message $str
        return
    }
    scan [Volume($r,node) GetImageRange] "%d %d" lo hi
    if {[CheckVolumeExists [Volume($r,node) GetFullPrefix] \
            [Volume($r,node) GetFilePattern] $lo $hi] != ""} {
        set str "The [Volume($r,node) GetName] volume cannot be found on disk."
        puts $str
        tk_messageBox -message $str
        return
    }

    # Write a "pseudo-MRML" file for the MI program consisting of:
    #   - Reference volume
    #   - Initial pose (from manual registration)
    #   - Volume to register

    set filename $Matrix(autoInput)
    set fid [open $filename w]

    AlignmentsWritePseudoMrmlVolume $fid $r
    puts $fid "matrix [Matrix($t,node) GetMatrix]"
    AlignmentsWritePseudoMrmlVolume $fid $v

    if {[catch {close $fid} errorMessage]} {
        tk_messageBox -type ok -message "The following error occurred saving a file: ${errorMessage}"
        puts "Aborting due to : ${errorMessage}"
        exit 1
    }

    # Command to run MI
    set argBin     "[file join $Path(program) mi-bin]"
    set argProgram "[file join $argBin mi]" 
    set argInput   "[file join $env(PWD) $filename]"
    set argTmp     "$Path(tmpDir)"
    if {$Matrix(autoSpeed) == "Fast"} {
        set argSpeed fast
    } else {
        set argSpeed slow 
    }
    set cmd "$argProgram $argBin $argInput $argSpeed $argTmp"
    puts "MI command:\n$cmd"

    # Delete the output file
    catch {file delete $Matrix(autoOutput)}

    # Exec
    catch {set pid [eval exec $cmd &]} errmsg

    # pid does not exist on error
    if {[info exists pid] == 0} {
        puts "Attempt to run MI produced error:\n'$errmsg'"
        return
    }
    puts "Running MI with pid=$pid"
    set Matrix(pid) $pid

    AlignmentsPoll
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoCancel {} {
    global Matrix Gui

    set pid $Matrix(pid)
    if {$pid == ""} {
        return
    }

    set cmd "kill -9 $pid"
    catch {exec $cmd} errmsg
    puts "$cmd --> $errmsg"
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsPoll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsPoll {} {
    global Matrix Gui
    
    set pid $Matrix(pid)
    
    if {$pid != ""} {
        catch {exec kill -0 $pid} errmsg
        if {$errmsg != ""} {
            
            # Stop polling
            set Matrix(pid) ""
            
            # Determine status (if file exists, then success)
            set filename $Matrix(autoOutput)
            set status 1 
            if {[CheckFileExists $filename 0] == "0"} {
                set status 0
            }

            # Apply and Notify user with popup
            if {$status == 1} {
                # Overwrite initial pose with final pose
                AlignmentsAutoApply
                set msg "Automatic registration completed successfully." 
                
                # Delete the output file
                catch {file delete $Matrix(autoOutput)}
                catch {file delete $Matrix(autoInput)}

            } else {
                set msg "Automatic registration failed." 
            }
            set x [winfo rootx $Matrix(bRun)]
            set y [winfo rooty $Matrix(bRun)]
            MsgPopup RegistrationReady $x $y $msg "Registration Complete"
            return
        }
    }
    
    # Poll every 10 seconds
    after 10000 AlignmentsPoll
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoApply {} {
    global Volume Matrix Mrml

    set v $Matrix(volume)
    if {$v == $Volume(idNone)} {return}

    # Read transform from mrml file created by MI
    set filename $Matrix(autoOutput)
    set dag [MRMLRead $filename]

    if {$dag == "-1"} {
        tk_messageBox -icon error -message "Failed to read '$filename'"
        return
    }
    set dag [MRMLExpandUrls $dag $Mrml(dir)]
    set dag [MRMLComputeRelationships $dag]

    set n [MRMLCountTypeOfNode $dag Transform]
    if {$n == 0} {
        tk_messageBox -icon error -message \
                "No transform nodes in MRML file '$filename'."
        return
    }

    set i    [MRMLGetIndexOfNodeInDag $dag 0 Transform]
    set node [MRMLGetNode $dag $i] 
    set str  [MRMLGetValue $node matrix]

    tk_messageBox -message "Automatic Registration Matrix:$str"

    # Convert str to matrix
    vtkMatrix4x4 mat
    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            mat SetElement $i $j [lindex $str [expr $i*4+$j]]
        }
    }

    # Set the current transform to this matrix
    set t $Matrix(tAuto)
    set tran [Matrix($t,node) GetTransform]
    $tran Push
    $tran SetMatrix mat
    mat Delete

    # Change name
    set name [Matrix($t,node) GetName]
    if {[regexp {^manual(.*)} $name match id] == 1} {
        Matrix($t,node) SetName auto$id
    }

    # Allow undo
    $Matrix(bUndo) configure -state normal

    # Update MRML
    MainUpdateMRML
    RenderAll
}



#*********************************************************************
#This is for running Dave's new MI code
#****
proc AlignmentsAutoRunTest {} {
    global Path env Gui Matrix Volume
    
    # No-can-do on PCs
    if {$Gui(pc) == 1} {
        tk_messageBox -message "\
Automatic registration is presently available only on UNIX systems."
        return
    }

    # Store which transform we're editing
    set t $Matrix(activeID)
    set Matrix(tAuto) $t
    if {$t == ""} {
        tk_messageBox -message "Please select a transform"
    }

    # v = ID of volume to register
    # r = ID of reference volume
    set v $Matrix(volume)
    set r $Matrix(refVolume)

    # Check that the volumes exist
    scan [Volume($v,node) GetImageRange] "%d %d" lo hi
    if {[CheckVolumeExists [Volume($v,node) GetFullPrefix] \
        [Volume($v,node) GetFilePattern] $lo $hi] != ""} {
        set str "The [Volume($v,node) GetName] volume cannot be found on disk."
        puts $str
        tk_messageBox -message $str
        return
    }
    scan [Volume($r,node) GetImageRange] "%d %d" lo hi
    if {[CheckVolumeExists [Volume($r,node) GetFullPrefix] \
        [Volume($r,node) GetFilePattern] $lo $hi] != ""} {
        set str "The [Volume($r,node) GetName] volume cannot be found on disk."
        puts $str
        tk_messageBox -message $str
        return
    }

    # Read parameters
    set filename $Matrix(autoInput)
    set fid [open $filename w]

    if {$Matrix(autoSpeed) == "Fast"} {
        set filename $Matrix(autoFast)
    } else {
        set filename $Matrix(autoSlow)
    }

    set fid [open $filename]
    set params(cmdList) ""
    foreach line [split [read $fid] \n] {
        set cmd [lindex $line 0]
        set val [lindex $line 1]
        lappend params(cmdList) $cmd
        set params($cmd) $val
        puts "$cmd $val"
    }
    close $fid

    # Get initial pose
    set tran [Matrix($t,node) GetTransform]
    set init  [$tran GetMatrix]
    puts "initialPose = [$init Print]"

    # Run MI Registration
    vtkImageMIReg reg
    reg SetReference [Volume($r,vol) GetOutput]
    reg SetSubject   [Volume($v,vol) GetOutput]
    reg SetInitialPose $init
    reg SetRefNode Volume($r,node)
    reg SetSubNode Volume($v,node)

    foreach cmd $params(cmdList) {
        #reg $cmd $params($cmd)
    }
    puts "GetNumIter= [reg GetNumIterations]"
    reg Update
    set final [reg GetFinalPose]
    puts "finalPose = [$final GetElement 0 3]"

    # Transfer values from finalPose to active transform
    $tran SetMatrix $final
    for {set i 0} {$i < 10} {incr i} {
        reg Update
        puts "finalPose = [$final GetElement 0 3]"
        
        # Update all MRML
        MainUpdateMRML
        Render$Matrix(render)
    }

    # Cleanup
    reg Delete
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAutoUndo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAutoUndo {} {
    global Matrix
    
    set t $Matrix(tAuto)
    set tran [Matrix($t,node) GetTransform]
    $tran Pop

    # Disallow undo
    $Matrix(bUndo) configure -state disabled

    # Update MRML
    MainUpdateMRML
    RenderAll
}

################################################################################
#                             Event Bindings
################################################################################


#-------------------------------------------------------------------------------
# .PROC AlignmentsB1
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsB1 {x y} {
    global Matrix Slice

    if {$Matrix(FiducialPickEntered) != 1} {
    set s $Slice(activeID)
    set orient [Slicer GetOrientString $s]
    if {[lsearch "Axial Sagittal Coronal" $orient] == -1} {
        tk_messageBox -message \
                "Set 'Orient' to Axial, Sagittal, or Coronal."
        return
    }
    set Matrix(xHome) $x
    set Matrix(yHome) $y

    # Translate
    if {$Matrix(mouse) == "Translate"} {

        Anno($s,msg,mapper) SetInput "0 mm"
        Anno($s,r1,actor)  SetVisibility 1
        Anno($s,r1,source) SetPoint1 $x $y 0
        Anno($s,r1,source) SetPoint2 $x [expr $y+1] 0
        
        # To make this translation add to the current translation amount,
        # store the current amount.
        set Matrix(prevTranLR) $Matrix(regTranLR)
        set Matrix(prevTranPA) $Matrix(regTranPA)
        set Matrix(prevTranIS) $Matrix(regTranIS)
    }
    # Rotate
    if {$Matrix(mouse) == "Rotate"} {

        Anno($s,msg,mapper) SetInput "0 deg"
        Anno($s,r1,actor)  SetVisibility 1
        Anno($s,r1,source) SetPoint1 128 128 0
        Anno($s,r1,source) SetPoint2 $x $y 0
        Anno($s,r2,actor)  SetVisibility 1
        Anno($s,r2,source) SetPoint1 128 128 0
        Anno($s,r2,source) SetPoint2 $x $y 0
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsB1Motion
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsB1Motion {x y} {
    global Matrix Slice Interactor View Anno

    #If Fiducial selection has been entered then do not allow the user to be able to
    #set the matrix by dragging the mouse over the 2D screens
    if {$Matrix(FiducialPickEntered) != 1} {
    # This only works on orthogonal slices
    set s $Slice(activeID)
    set orient [Slicer GetOrientString $s]
    if {[lsearch "Axial Sagittal Coronal" $orient] == -1} {
        return
    }

    # Translate
    if {$Matrix(mouse) == "Translate"} {
        
        set xPixels [expr $x - $Matrix(xHome)]
        set yPixels [expr $y - $Matrix(yHome)]
        set xMm [PixelsToMm $xPixels $View(fov) 256 $Slice($s,zoom)]
        set yMm [PixelsToMm $yPixels $View(fov) 256 $Slice($s,zoom)]
        
        Anno($s,r1,source) SetPoint2 $x $y 0

        switch $orient {
            Axial {
                # X:R->L, Y:P->A
                set xMm [expr -$xMm]
                set text "LR: $xMm, PA: $yMm mm"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualTranslateDual \
                        regTranLR [expr $xMm + $Matrix(prevTranLR)] \
                        regTranPA [expr $yMm + $Matrix(prevTranPA)]
            }
            Sagittal {
                # X:A->P, Y:I->S
                set xMm [expr -$xMm]
                set text "PA: $xMm, IS: $yMm mm"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualTranslateDual \
                        regTranPA [expr $xMm + $Matrix(prevTranPA)] \
                        regTranIS [expr $yMm + $Matrix(prevTranIS)]
            }
            Coronal {
                # X:R->L, Y:I->S
                set xMm [expr -$xMm]
                set text "LR: $xMm, IS: $yMm mm"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualTranslateDual \
                        regTranLR [expr $xMm + $Matrix(prevTranLR)] \
                        regTranIS [expr $yMm + $Matrix(prevTranIS)]
            }
        }
    }

    # Rotate
    if {$Matrix(mouse) == "Rotate"} {
        
        set degrees [Angle2D 128 128 $Matrix(xHome) $Matrix(yHome) \
                128 128 $x $y]
        set degrees [expr int($degrees)]
        Anno($s,r2,source) SetPoint2 $x $y 0
        
        switch $orient {
            Axial {
                # IS-axis
                set text "IS-axis: $degrees deg"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualRotate regRotIS $degrees 1
            }
            Sagittal {
                # LR-axis
                set text "LR-axis: $degrees deg"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualRotate regRotLR $degrees 1
            }
            Coronal {
                # PA-axis
                set degrees [expr -$degrees]
                set text "PA-axis: $degrees deg"
                Anno($s,msg,mapper)  SetInput $text
                AlignmentsManualRotate regRotPA $degrees 1
            }
        }
    }
}
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsB3Motion
#
# Zooms the 2D slice windows. 
# This exists here because the bindings in the original zoom function 
# do not allow zooming of the 2d slices for the volume in the matRen when fiducial
# selection is on. This procedure should probably be replaced with pushing and 
# popping bindings. 
#
# .ARGS
# float x the x position of the mouse cursor where zooming begins
# float y the y position of the mouse cursor where zooming stops
# .END
#-------------------------------------------------------------------------------
proc AlignmentsB3Motion {x y} {
    global Interactor View Matrix

    if {$Matrix(FiducialPickEntered) == 1} {
    set s $Interactor(s)
    scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
    
    AlignmentsInteractorZoom $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)   
    
    # Cursor
    MainInteractorCursor $s $xs $ys $x $y
    
    set Interactor(xLast)  $x
    set Interactor(yLast)  $y
    set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys
    
    # Render this slice
    MainInteractorRender
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsInteractorZoom
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsInteractorZoom {s x y xLast yLast} {
    global Interactor Anno 
    
    set dy [expr $yLast - $y]
    
    set b      1.02
   
    #set zPrev [[$Interactor(Slicer) GetBackReformat $s] GetZoom]
    set zPrev [$Interactor(Slicer) GetZoomNew $s]
    set dyPrev [expr log($zPrev) / log($b)]

    set zoom [expr pow($b, ($dy + $dyPrev))]
    if {$zoom < 0.01} {
    set zoom 0.01
    }
    set z [format "%.2f" $zoom]

    Anno($s,msg,mapper)  SetInput "ZOOM: x $z"

    AlignmentsSlicesSetZoom $s $z
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetZoom
# 
# Checks to see which vtkMrmlSlicer object is active and sets the zoom factor
# for the slices belonging to that object only.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetZoom {s {zoom}} {
    global Matrix Slice

        puts "this is the zoom $zoom" 
    # if user-entered zoom is okay then do the rest of the procedure
    if {$Matrix(activeSlicer) == "Slicer"} {
        # Change Slice's Zoom variable
            set Slice($s,zoom) $zoom    
        # Use Attila's new zooming code
        Slicer SetZoomNew $s $zoom
        Slicer Update
    } else { 
        # Change Slice's Zoom variable
        set Matrix($s,zoom) $zoom    
        # Use Attila's new zooming code
        MatSlicer SetZoomNew $s $zoom
        MatSlicer Update
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsB1Release
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsB1Release {x y} {
    global Matrix Slice

   #If Fiducial selection has been entered then do not allow the user to be able to
   #set the matrix by dragging the mouse over the 2D screens
   if {$Matrix(FiducialPickEntered) != 1} {
   set s $Slice(activeID)
    set orient [Slicer GetOrientString $s]
    if {[lsearch "Axial Sagittal Coronal" $orient] == -1} {
        return
    }

    # Translate
    if {$Matrix(mouse) == "Translate"} {
        Anno($s,msg,mapper) SetInput ""
        Anno($s,r1,actor)  SetVisibility 0
    }

    # Rotate
    if {$Matrix(mouse) == "Rotate"} {
        Anno($s,msg,mapper) SetInput ""
        Anno($s,r1,actor)  SetVisibility 0
        Anno($s,r2,actor)  SetVisibility 0
    }
}
}


################################################################################
# Procedures added to handle split screens for "Registration/Alignment 
# using Fiducials" 
###############################################################################

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetRegistrationMech
#
# Set the registration mechanism depending on which button the user selected in
# the Auto tab. 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetRegistrationMech {} { 
    global Matrix
          
    if {$Matrix(f$Matrix(regMech)) == "$Matrix(fFidAlign)"} {
        puts "you are in the fiducial alignment mode"
        AlignmentsFiducialPick
        #Split the screens and set the controls if they are not already set. 
        #freeze all other tabs
    } elseif {$Matrix(f$Matrix(regMech)) == "$Matrix(fTPS)"} {
        puts "you are in the Thin plate spline registration mode"
        AlignmentsFiducialPick
        #Split the screens and set the controls if they are not already set. 
        #freeze all other tabs
    } elseif {$Matrix(f$Matrix(regMech)) == "$Matrix(fMI)"} {
        puts "you are in the registration by mutual information mode"
        #Unsplit the screens if they are split 
        #freeze all other tabs??
        raise $Matrix(f$Matrix(regMech))
        focus $Matrix(f$Matrix(regMech))
    }
}

#TEMP
proc AlignmentsWhichCommand {{inputs ""} {inputs ""} {inputs ""}} {
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildSliceControls
# 
# Builds the slice controls that will be displayed when the screen is spilt
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildSliceControls {whichSlicer} {
    global Gui Slice
   
    set f $Gui(fViewer)

    frame $f.fMatVol -bg $Gui(backdrop)
    set Gui(fMatVol) $f.fMatVol

    frame $f.fMatMid$whichSlicer -bg $Gui(backdrop)
    set Gui(fMatMid$whichSlicer) $f.fMatMid$whichSlicer 

    set f $Gui(fMatMid$whichSlicer) 
    
    foreach s $Slice(idList) {
        frame $f.fMatMidSlice$s -bg $Gui(activeWorkspace)
        pack $f.fMatMidSlice$s -side left -expand 1 -fill both
        AlignmentsBuildControls $s $f.fMatMidSlice$s $whichSlicer
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildControls
#  Builds the controls for offset and orientation for MatSlicer 
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildControls {s F whichSlicer} {
    global Gui View Slice Matrix


    if {$whichSlicer == "Slicer" } {
    set layer "Fore"
    } else {
    set layer "Back"
    }

    frame $F.fOffset -bg $Gui(activeWorkspace)
    frame $F.fOrient -bg $Gui(activeWorkspace)
    pack $F.fOffset $F.fOrient -fill x -side top -padx 0 -pady 3

    # Offset
    #-------------------------------------------
    set f $F.fOffset
    set fov2 [expr $View(fov) / 2]

    #The offset entry box
    eval {entry $f.eOffset -width 4 -textvariable Slice(s,offset)} $Gui(WEA)
    bind $f.eOffset <Return>   "AlignmentsSlicesOffsetUpdated $s"
    bind $f.eOffset <FocusOut> "AlignmentsSlicesOffsetUpdated $s"

    # tooltip for entry box
    set tip "Current slice: in mm or slice increments,\n \
        depending on the slice orientation you have chosen.\n \
        The default (AxiSagCor orientation) is in mm. \n \
        When editing (Slices orientation), slice numbers are shown.\n\
        To change the distance between slices from the default\n\
        1 mm, right-click on the V button."

    TooltipAdd $f.eOffset $tip
    eval {scale $f.sOffset -from -$fov2 -to $fov2 \
          -variable Slice($s,offset) -length 160 -resolution 1.0 \
          -command "AlignmentsSlicesSetOffsetInit $s $f.sOffset $whichSlicer"} $Gui(WSA) \
        {-troughcolor $Gui(slice$s)}
    
    pack $f.sOffset $f.eOffset -side left -anchor w -padx 2 -pady 0

    # Visibility
    #-------------------------------------------
    # This Slice
   
    eval {checkbutton $f.cVisibility${s} \
          -variable Matrix($s,$layer,visibility) -indicatoron 0 -text "V" -width 2 \
          -command "AlignmentsSlicesSetVisibility $s; RenderBoth $s"} $Gui(WCA) \
    {-selectcolor $Gui(slice$s)}

    # tooltip for Visibility checkbutton
    TooltipAdd $f.cVisibility${s} "Click to make this slice visible.\n \
        Right-click for menu: \nzoom, slice increments, \
        volume display."

    pack $f.cVisibility${s} -side left -padx 2

    # Menu on the Visibility checkbutton 
    # Ron said menu items not impt in this view so I have only left the first 2 to 
    # avoid code duplication from MainSlices.tcl
    eval {menu $f.cVisibility${s}.men} $Gui(WMA)
    set men $f.cVisibility${s}.men
    $men add command -label "All Visible" \
        -command "AlignmentsSlicesSetVisibilityAll 1; Render3D"
    $men add command -label "All Invisible" \
        -command "AlignmentsSlicesSetVisibilityAll 0; Render3D"
    $men add command -label "-- Close Menu --" -command "$men unpost"
    bind $f.cVisibility${s} <Button-3> "$men post %X %Y"

    # Orientation
    #-------------------------------------------
    set f $F.fOrient

    # All Slices
    eval {menubutton $f.mbOrient -text "Or:" -width 3 -menu $f.mbOrient.m} \
        $Gui(WMBA) {-anchor e}
    pack $f.mbOrient -side left -pady 0 -padx 2 -fill x
    # tooltip for orientation menu
    TooltipAdd $f.mbOrient "Set Orientation of all slices."

    eval {menu $f.mbOrient.m} $Gui(WMA)
    foreach item "AxiSagCor Orthogonal Slices ReformatAxiSagCor" {
        $f.mbOrient.m add command -label $item -command \
            "AlignmentsSlicesSetOrientAll $item; RenderAll"
    }

    # This slice

    eval {menubutton $f.mbOrient${s} -text INIT -menu $f.mbOrient${s}.m \
        -width 13} $Gui(WMBA) {-bg $Gui(slice$s)}
    pack $f.mbOrient${s} -side left -pady 0 -padx 2 -fill x

    # tooltip for orientation menu for slice
    TooltipAdd $f.mbOrient${s} "Set Orientation of just this slice."

    eval {menu $f.mbOrient${s}.m} $Gui(WMA)
    set Slice($s,menu) $f.mbOrient${s}.m
    foreach item "[Slicer GetOrientList]" {
        $f.mbOrient${s}.m add command -label $item -command \
            "AlignmentsSlicesSetOrient ${s} $item; RenderBoth $s"
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildActiveVolSelectControls
# 
# Builds the radio buttons that allow the user to toggle which of the two data
# sets in the split screens are active.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildActiveVolSelectControls {} {    
    global Gui Matrix

    #The volume Selection frame
    set f $Gui(fMatVol)
    frame $f.fActiveVol -bg $Gui(activeWorkspace) -bd 3
    pack $f.fActiveVol -fill x -side top -padx 0 -pady 0
    
    set f $f.fActiveVol    
    eval {label $f.fTitle -text "Active Slices: "} $Gui(WLA)
        
    frame $f.f -bg $Gui(backdrop)
    foreach slicer "Slicer MatSlicer" \
    text "$Matrix(FidAlignRefVolumeName) $Matrix(FidAlignVolumeName)" {
        eval {radiobutton $f.f.r$slicer \
              -text "$text" -value "$slicer"\
              -variable Matrix(activeSlicer)\
              -indicatoron 1 -command "AlignmentsSetActiveSlicer $slicer; \
AlignmentsSetActiveScreen; AlignmentsConfigSliceGUI; \
AlignmentsSetSliceWindows $slicer;"} $Gui(WCA)
          
        pack $f.f.r$slicer -side left -padx 0 -ipadx 15
    }
    pack $f.fTitle $f.f -side left -padx $Gui(pad) -fill x -anchor w

  #save the path to the radio buttons 
  set Matrix(rFidScActivateRefVolume) $f.f.rSlicer    
  set Matrix(rFidScActivateVolume) $f.f.rMatSlicer
}


#----------------------------------------------------------------------------------
# .PROC AlignmentsConfigSliceGUI
#  Configures the slice controls GUI to reflect which dataset the user is 
#  interacting with
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsConfigSliceGUI {} {
    global Matrix Slice Gui

    foreach s $Slice(idList) {
    if {$Matrix(activeSlicer) == "Slicer"} {
        set Matrix($s,Fore,visibility) [Slice($s,Fore,planeActor) GetVisibility]
        set offset Slice($s,offset)
        set visibility Matrix($s,Fore,visibility)
        set orient $Slice($s,orient)
    } else {
        set Matrix($s,Back,visibility) [Slice($s,Back,planeActor) GetVisibility]
        set offset Matrix($s,offset)
        set visibility Matrix($s,Back,visibility)
        set orient $Matrix($s,orient)
    }

    
    #offset entry boxes
        .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.eOffset configure -textvariable $offset
    $Gui(fSlice$s).fControlsMat.fOffset.eOffset configure -textvariable $offset
    
        #offset sliders
        .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset configure -variable $offset
        $Gui(fSlice$s).fControlsMat.fOffset.sOffset configure -variable $offset
    
        #visibility buttons
        .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.cVisibility$s configure -variable $visibility
    $Gui(fSlice$s).fControlsMat.fOffset.cVisibility$s configure -variable $visibility

        #orientation buttons
        .tViewer.fMatMidSlicer.fMatMidSlice$s.fOrient.mbOrient$s configure -text $orient
    $Gui(fSlice$s).fControlsMat.fOrient.mbOrient$s configure -text $orient
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsUnpackFidSelectScreenControls
# Unpacks the Slice controls that were built specifically for the fiducial
# selection screen and repacks in the stuff that belongs to the original gui
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsUnpackFidSelectScreenControls {} {
    global Gui Slice

    set f $Gui(fViewer)
    pack forget $Gui(fTop) $Gui(fMatVol) $Gui(fMatMidSlicer) $Gui(fBot) 
    #Repack the original stuff
    pack $Gui(fTop) -side top
    pack $Gui(fMid) -side top -expand 1 -fill x
    pack $Gui(fBot) -side top
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetSliceControls
#  Changes the Slice controls on the MainViewer Depending on which Volume the 
#  User is interacting with 
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetSliceControls {} {
    global Matrix Gui Slice View

        pack forget $Gui(fTop) $Gui(fMid) $Gui(fBot)
        pack forget $Gui(fTop) $Gui(fMatVol) $Gui(fMatMidSlicer) $Gui(fBot) 

        #Repack the GUI
        pack $Gui(fTop) -side top
        pack $Gui(fMatVol) -side top -expand 0 -fill x 
        pack $Gui(fMatMidSlicer) -side top -expand 1 -fill x
        pack $Gui(fBot) -side top
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsFiducialPick
# This is the callback proc for the Fiducial selection button in the auto tab
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsFiducialPick {} {
    global Matrix Module Volume Slice Gui View Fiducials

    if {[IsModule Alignments] == 1} { 
      
        # Store which transform we're editing 
    set t $Matrix(activeID)
    set Matrix(tAuto) $t
    if {$t == ""} {
        tk_messageBox -message "Please select a transform"
    set Matrix(regMech) ""
    raise $Matrix(fAlignBegin)
        return
    } 

        #Do not allow entry into the Fiducial tab unless there is a volume to move
        #and a reference volume selected. 
        if {$Matrix(volume) == $Volume(idNone) || $Matrix(refVolume) == $Volume(idNone)} {
            tk_messageBox -icon error -message "Either the reference volume or the volume to 
            move has not yet been set" 
            set Matrix(regMech) ""
            raise $Matrix(fAlignBegin)
            return
        }
                
        # display an error message if the user has selected the same volume twice
        if {$Matrix(volume) == $Matrix(refVolume)} {
            tk_messageBox -icon error -message "You have selected the same\nvolume for both the reference volume\n and the volume to move" 
        set Matrix(regMech) ""
            raise $Matrix(fAlignBegin)
            return
        }

        #Jump to the Fiducials tab 
        set Alignments(freeze) 1
        Tab Alignments row1 Auto
        set Module(freezer) "Alignments row1 Auto" 
        raise $Matrix(f$Matrix(regMech))

        #I put this in here because it was giving an error when the user tried to close 
        #the file or do any other operation on the fiduicials tab before the mouse was 
        #moved in the 3D view
        set Matirx(currentDataList) ""

        AlignmentsMatRenUpdateCamera
        
        #Depending on what the user selected as the volume to move and the reference volume
        #Put the "Reference Volume" in the viewRen renderer and put the "Volume to Move" 
        #in the matRen renderer.
        #The way this works is to set the fore image of the slicer to be the reference vol
        #volume and to set the back image of the slicer to be the volume to move
        #When the volumes are split the back/fore reformat image is taken and texture mapped
        #onto the slice actors in the renderers.
        foreach v $Volume(idList) {
            #put the reference volume in the left renderer
            if {[Volume($v,node) GetName] == $Matrix(FidAlignRefVolumeName)} {
                MainSlicesSetVolumeAll Fore $v
                AlignmentsSplitVolumes $v viewRen Fore
                #put the volume to move in the right renderer
            } elseif {[Volume($v,node) GetName] == $Matrix(FidAlignVolumeName)} {
                MainSlicesSetVolumeAll Back $v
                AlignmentsSplitVolumes $v matRen Back
            }
        }
        
        #Hanifa TBD: load in the fiducials lists here and make a copy of them to 
        #the lists created below so that they can be deleted when fiducials exits
        #how about saving them? -- dont delete them.. just have the counter
        #but how do you set up the counter?
        #2 buttons- load fiducials list for : $referncevolume $volume



        #If the fiducials lists have not been created then create them here
        if { [lsearch $Fiducials(listOfNames) $Matrix(FidAlignRefVolumeName)] == -1} { 
        FiducialsCreateFiducialsList "Alignments" $Matrix(FidAlignRefVolumeName)     
            set Matrix(oldDataList,$Matrix(FidAlignRefVolumeName)) ""
            set Matrix(textBox,currentID,$Matrix(FidAlignRefVolumeName)) -1  
        }
        
        if { [lsearch $Fiducials(listOfNames) $Matrix(FidAlignVolumeName)] == -1} {
        FiducialsCreateFiducialsList "Alignments" $Matrix(FidAlignVolumeName)
            set Matrix(oldDataList,$Matrix(FidAlignVolumeName)) ""
            set Matrix(textBox,currentID,$Matrix(FidAlignVolumeName)) -1
        }
        
        #Call this in case it is not the first time Fiducials tab has been entered  
        AlignmentsFiducialsUpdated
        
        #split the window and display both of the volumes loaded
        set Matrix(FiducialPickEntered) 1
        AlignmentsSetRegTabState disabled
        set Matrix(matricesview,visibility) 1
        AlignmentsUpdateFidSelectViewVisibility
             
        #Display the new Slice control windows 
        #These new controls limit the user from being able to alter options 
        #such as the ability to change the bg/fg images when they are in fiducial selection 
        #AlignmentsSetSliceControls
        AlignmentsViewerUpdate             
        pushEventManager $Matrix(eventManager)
    
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSplitVolumes
# 
# split 3d view window in two, put back volume in one side and fore in other.
# send in the current volume and the renderer to add/remove from  
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSplitVolumes {v ren layer} {
    global Volume Slice View Matrix Lut Anno Gui

    AlignmentsSetActiveSlicer Slicer

    if {$ren=="matRen"} {
        MatSlicer DeepCopy Slicer
    MatSlicer SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)
        MatSlicer SetForeOpacity 0
        foreach s $Slice(idList) {
            set Matrix($s,orient) $Slice($s,orient)
            set Matrix($s,zoom) 1 
          # set Matrix($s,offsetIncrement) $Slice($s,offsetIncrement)
            AlignmentsSlicesSetSliderRange $s
            set Matrix($s,offset) $Slice($s,offset)     
        }
        AlignmentsSetActiveSlicer MatSlicer
    }

    
    #Need this new Mapper because the colors get messed up if we use the other one
    foreach s $Slice(idList) {    
    Matrix(Mapper$s$layer) SetLookupTable [Volume($v,vol) GetIndirectLUT]
    $ren RemoveActor Slice($s,planeActor)
        eval Slice($s,$layer,planeActor) SetScale [Slice($s,planeActor) GetScale]
        $ren AddActor Slice($s,$layer,planeActor)
    set Matrix($s,$layer,visibility) $Slice($s,visibility)
        Slice($s,$layer,planeActor) SetVisibility [Slice($s,planeActor) GetVisibility] 
    } 
       
    #tbd : this should be called when the camera is updated for matren
    #Set the scale for the letter actors in the new renderer
    set scale [expr $View(fov) * $Anno(letterSize) ]
    foreach axis "R A S L P I" {
    ${axis}ActorMatRen SetScale  $scale $scale $scale 
    }
            
    #THIS IS TEMP
    set pos [expr   $View(fov) * 0.6]
    set neg [expr - $View(fov) * 0.6]
    RActorMatRen SetPosition $pos 0.0  0.0
    AActorMatRen SetPosition 0.0  $pos 0.0
    SActorMatRen SetPosition 0.0  0.0  $pos 
    LActorMatRen SetPosition $neg 0.0  0.0
    PActorMatRen SetPosition 0.0  $neg 0.0
    IActorMatRen SetPosition 0.0  0.0  $neg   

     #Set the orientation for the MatSlicer to be AxiSagCor
    AlignmentsSlicesSetOrientAll AxiSagCor
    $Matrix(activeSlicer) Update    
    Render3D

    # TEMP -put a "push bindings someplace -see how its done in select.tcl
    set widgets "$Gui(fSl0Win) $Gui(fSl1Win) $Gui(fSl2Win)"
    foreach widget $widgets {
    bind $widget <KeyPress-p> {
        # like SelectPick2D, sets right coords in Alignments(xyz)
        # returns 0 if nothing picked
        if { [AlignmentsPick2D %W %x %y] != 0 } \
        { eval FiducialsCreatePointFromWorldXYZ "default" $Matrix(xyz) ; MainUpdateMRML; Render3D}
    }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsPick2D
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsPick2D { widget x y } {
    global Select Interactor Matrix
    set s $Interactor(s)
    if { $s != "" } {
        scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs yz x y
        $Matrix(activeSlicer) SetReformatPoint $s $x $y
        scan [$Matrix(activeSlicer) GetWldPoint] "%g %g %g" xRas yRas zRas
        set Matrix(xyz) "$xRas $yRas $zRas"
        return 1
    } else {
        return 0
        }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsSetViewMenuState
# This is temp until I fix the ability to set the view params from the View 
# pulldown menu on the 3DSlicer Gui
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetViewMenuState {mode} {
    global Gui

    #Disable/enable the ability to add the endoscopic ren when in this mode
    #Should be disabled when we are in the split screen mode
    $Gui(mView) entryconfigure 11 -state $mode 
    $Gui(mView) entryconfigure 12 -state $mode
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAddLetterActors
# Adds the R A S L P I letters to the matRen Renderer. 
# The letters follow the View(matCam)
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAddLetterActors {} {
    global Anno View

    set fov2 [expr $View(fov) / 2] 
    set scale [expr $View(fov) * $Anno(letterSize) ]

    foreach axis "R A S L P I" {

        vtkVectorText ${axis}TextMatRen
        ${axis}TextMatRen SetText "${axis}"
        vtkPolyDataMapper  ${axis}MapperMatRen
        ${axis}MapperMatRen SetInput [${axis}TextMatRen GetOutput]
        vtkFollower ${axis}ActorMatRen
        ${axis}ActorMatRen SetMapper ${axis}MapperMatRen
        ${axis}ActorMatRen SetScale  $scale $scale $scale 
        ${axis}ActorMatRen SetPickable 0
    
        matRen AddActor ${axis}ActorMatRen
        ${axis}ActorMatRen SetCamera $View(MatCam)
    }

    set pos [expr   $View(fov) * 0.6]
    set neg [expr - $View(fov) * 0.6]
    RActorMatRen SetPosition $pos 0.0  0.0
    AActorMatRen SetPosition 0.0  $pos 0.0
    SActorMatRen SetPosition 0.0  0.0  $pos 
    LActorMatRen SetPosition $neg 0.0  0.0
    PActorMatRen SetPosition 0.0  $neg 0.0
    IActorMatRen SetPosition 0.0  0.0  $neg   
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsMatRenUpdateCamera
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsMatRenUpdateCamera {} {
    global View
    
    eval $View(MatCam) SetClippingRange [$View(viewCam) GetClippingRange]
    eval $View(MatCam) SetPosition [$View(viewCam) GetPosition]
    eval $View(MatCam) SetFocalPoint [$View(viewCam) GetFocalPoint]
    eval $View(MatCam) SetViewUp [$View(viewCam) GetViewUp]
    $View(MatCam) ComputeViewPlaneNormal  
}



#-------------------------------------------------------------------------------
# .PROC AlignmentsUpdateFidSelectViewVisibility
#  Called by the checkbuttons in the Fiducials tab. Turns on/off a renderer.
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsUpdateFidSelectViewVisibility {} {
    global View viewWin Matrix

    if {$Matrix(matricesview,visibility) == 1 && $Matrix(mainview,visibility) == 1} {
        AlignmentsAddFidSelectView
    } elseif {$Matrix(matricesview,visibility) == 0 && $Matrix(mainview,visibility) == 1} {
        AlignmentsRemoveFidSelectView
        AlignmentsSetActiveSlicer Slicer
        AlignmentsSetSliceWindows Slicer
    } elseif {$Matrix(matricesview,visibility) == 1 && $Matrix(mainview,visibility) == 0} {
        AlignmentsAddFidSelectView
        AlignmentsRemoveMainView
        AlignmentsSetActiveSlicer MatSlicer
        AlignmentsSetSliceWindows MatSlicer
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsUpdateMainViewVisibility
#  Called by the checkbuttons in the Fiducials tab. Determine which renderer to 
#  display
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsUpdateMainViewVisibility {} {
    global View viewWin Matrix
    
    if {$Matrix(mainview,visibility) == 1 && $Matrix(matricesview,visibility) == 1} {
        AlignmentsAddMainView 
    } elseif {$Matrix(mainview,visibility) == 0 && $Matrix(matricesview,visibility) == 1} {
        AlignmentsRemoveMainView
        AlignmentsSetActiveSlicer MatSlicer
        AlignmentsSetSliceWindows MatSlicer
    } elseif {$Matrix(mainview,visibility) == 1 && $Matrix(matricesview,visibility) == 0} {
        AlignmentsAddMainView
        AlignmentsRemoveFidSelectView
        AlignmentsSetActiveSlicer Slicer
        AlignmentsSetSliceWindows Slicer
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAddMainView
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAddMainView {} {
    global View viewWin Matrix
    
    $viewWin AddRenderer viewRen
    viewRen SetViewport 0 0 0.5 1
    matRen SetViewport 0.5 0 1 1
    MainViewerSetSecondViewOn
    MainViewerSetMode $View(mode)
    set Matrix(fidSelectViewOn) 1
    set Matrix(splitScreen) 1
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsAddFidSelectView
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsAddFidSelectView {} {
     global View viewWin Matrix
    
    if {$Matrix(fidSelectViewOn) == 0} {
        $viewWin AddRenderer matRen
        viewRen SetViewport 0 0 0.5 1
        matRen SetViewport 0.5 0 1 1
        MainViewerSetSecondViewOn
        MainViewerSetMode $View(mode)
        set Matrix(fidSelectViewOn) 1
        set Matrix(splitScreen) 1
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsRemoveFidSelectView
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsRemoveFidSelectView {} {
    global View viewWin Matrix

    if {$Matrix(fidSelectViewOn) == 1} {
        $viewWin RemoveRenderer matRen
        viewRen SetViewport 0 0 1 1
        MainViewerSetSecondViewOff
        MainViewerSetMode $View(mode)
        set Matrix(fidSelectViewOn) 0
        set Matrix(splitScreen) 0
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsRemoveMainView
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsRemoveMainView {} {
   global View viewWin Matrix
    
    $viewWin RemoveRenderer viewRen
    matRen SetViewport 0 0 1 1
    MainViewerSetSecondViewOn
    MainViewerSetMode $View(mode)
    set Matrix(fidSelectViewOn) 1
    set Matrix(splitScreen) 0
}    


#-------------------------------------------------------------------------------
# .PROC AlignmentsGetCurrentView
# 
#     Set the current window based on the mouse interaction
#     Set the current dataset so that Fiducials knows which list    
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsGetCurrentView {widget x y} {
    global Gui View Module Matrix Fiducials Volume

    #check if you are in the fiducial pick screen and if the 
    if {$Matrix(FiducialPickEntered) == 1 && $Matrix(splitScreen) == 1} {
    set View(bgColor) "0.7 0.7 0.9"
 
    #set width [expr $View(viewerWidth) /2]
    #This is the viewRenWindow

    set width [expr [lindex [[$Gui(fViewWin) GetRenderWindow] GetSize] 0] /2]

    if {$x < $width} {
    AlignmentsSetActiveSlicer Slicer
        FiducialsSetActiveList $Matrix(FidAlignRefVolumeName)
        set Matrix(currentDataList) $Matrix(FidAlignRefVolumeName)
        set Matrix(currentTextFrame) $Matrix(FidRefVolPointBoxes)
        #Change the color of the renderer to indicate that it is the active one
        eval matRen SetBackground $View(bgColor)
        set View(bgColor) "0.9 0.9 0.1" 
        eval viewRen SetBackground $View(bgColor)

    #This is the matRen Window       
    } else {
        AlignmentsSetActiveSlicer MatSlicer
        FiducialsSetActiveList $Matrix(FidAlignVolumeName)
        set Matrix(currentDataList) $Matrix(FidAlignVolumeName)
        set Matrix(currentTextFrame) $Matrix(FidVolPointBoxes)
        #Change the color of the renderer to indicate that it is the active one
        eval viewRen SetBackground $View(bgColor)
        set View(bgColor) "0.9 0.9 0.1" 
        eval matRen SetBackground $View(bgColor)    
    }
    #2D Slice Windows
    AlignmentsSetSliceWindows $Matrix(activeSlicer)

    AlignmentsConfigSliceGUI

    RenderSlices    
    Render3D
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetActiveScreen
# 
# Called by the radio buttons on the slicer controls. 
# Sets which screen is the active one based on the button pressed 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetActiveScreen {} {
    global Matrix View
    set View(bgColor) "0.7 0.7 0.9"
    if {$Matrix(activeSlicer) == "Slicer"} {    
    eval matRen SetBackground $View(bgColor)
    set View(bgColor) "0.9 0.9 0.1" 
    eval viewRen SetBackground $View(bgColor)    
    } else {
    eval viewRen SetBackground $View(bgColor)
    set View(bgColor) "0.9 0.9 0.1" 
    eval matRen SetBackground $View(bgColor)
    }
    Render3D
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsFiducialsUpdated
# Called whenever a fiducial point is updated
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsFiducialsUpdated {} {
    global Matrix Fiducials

    if {$Matrix(FiducialPickEntered) == 1 && $Matrix(currentDataList) != ""} {     
        AlignmentsBuildBoxesForList $Matrix(currentDataList)     
    }
    if { [lsearch $Fiducials(listOfNames) $Matrix(FidAlignVolumeName)] == -1 || 
         [lsearch $Fiducials(listOfNames) $Matrix(FidAlignRefVolumeName)] == -1} { 
        return
    } else {
#Set all the other fiducials the user picked elsewhere to be invisible
#       FiducialsSetFiducialsVisibility NONE 0 matRen
#    FiducialsSetFiducialsVisibility NONE 0 viewRen
    #Now Set only the ones that are picked for the datasets to be visibile
        FiducialsSetFiducialsVisibility $Matrix(FidAlignVolumeName) 0 viewRen
        FiducialsSetFiducialsVisibility $Matrix(FidAlignRefVolumeName) 1 viewRen
        FiducialsSetFiducialsVisibility $Matrix(FidAlignVolumeName) 1 matRen
        FiducialsSetFiducialsVisibility $Matrix(FidAlignRefVolumeName) 0 matRen
    }    
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsBuildBoxesForList
# Rebuilds the entry boxes that hold the xyz coordinate values for each fiducial
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsBuildBoxesForList {ListName} {
    global Gui Matrix Point Fiducials

    set dataList [FiducialsGetPointIdListFromName $ListName]

    #Clear the entries in the boxes and delete them
    #$Matrix(oldDataList,$ListName) is the list of all the fiducial points before 
    #a new one was added or deleted. 
    if {[llength $Matrix(oldDataList,$ListName)] > 0} {
        foreach pid $Matrix(oldDataList,$ListName) {
            #clear the text boxes
            $Matrix(currentTextFrame).entry$pid delete 0 end
            #remove the labels 
            destroy $Matrix(currentTextFrame).lEntry$pid
            #remove the boxes
            destroy $Matrix(currentTextFrame).entry$pid
        }
    }
  
    #Build the textboxes again for all the points in the current list
    #set dataList [FiducialsGetPointIdListFromName $ListName]
    set count 0  
    foreach pid $dataList {
        eval {label $Matrix(currentTextFrame).lEntry$pid -text "Pt:$count"} $Gui(BLA)
        eval {entry $Matrix(currentTextFrame).entry$pid -justify right -width 10} $Gui(WEA)
        set count [expr $count + 1]
    }
    
    foreach pid $dataList {
        grid $Matrix(currentTextFrame).lEntry$pid $Matrix(currentTextFrame).entry$pid
        #save the path to the textbox so we can access it later
        set Matrix(textBox,$pid,currentDataList) $Matrix(currentTextFrame).entry$pid
    }
   
    foreach pid $dataList {
        set xyz [FiducialsGetPointCoordinates $pid]
        $Matrix(currentTextFrame).entry$pid config -state normal
        $Matrix(currentTextFrame).entry$pid insert 0 $xyz
        $Matrix(currentTextFrame).entry$pid config -state disabled
    }

    #set this new list to be the old list for next time
    set Matrix(oldDataList,$ListName) $dataList
} 

#-------------------------------------------------------------------------------
# .PROC AlignmentsDeleteFiducialsList
# Deletes the specified Fiducial list and its corresponding text boxes on the 
# Fiducial Selection tab
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsDeleteFiducialsList {ListName currentTextFrame} {
    global Matrix Fiducials

    #Check if the lists already exist
    if { [lsearch $Fiducials(listOfNames) $ListName] == -1} {
       return 
    } else {    
        #Delete them if they do
    FiducialsDeleteList $ListName

        #Delete the textboxes associated with the List
        if {[llength $Matrix(oldDataList,$ListName)] > 0} {
            foreach pid $Matrix(oldDataList,$ListName) {
                #clear the text boxes
                $currentTextFrame.entry$pid delete 0 end
                #remove the labels 
                destroy $currentTextFrame.lEntry$pid
                #remove the boxes
                destroy $currentTextFrame.entry$pid
            }
       }
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsApplyFiducialSelection
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsApplyFiducialSelection {} {
    global Matrix Module Label Slice 
  
    #Validate the input

    # If there is no active transform, then do nothing
    set t $Matrix(activeID)
    if {$t == "" || $t == "NEW"} {
    tk_messageBox -icon error -message "Please Set An Active Transformation Matrix"
        return 
    }
        
    # If tabs are frozen, then 
    if {$Module(freezer) != ""} {
    
    #check to see if three points for each list have been selected
    #if three points not selected, diplay an error message
    
    if {[llength [FiducialsGetPointIdListFromName $Matrix(FidAlignRefVolumeName)]] < 2} {
        tk_messageBox -icon error -message "You must select at least\n\
            THREE fiducial points for\n\ rigid transformation"
        return
    }
    
    if {[llength [FiducialsGetPointIdListFromName $Matrix(FidAlignVolumeName)]] < 2} {
        tk_messageBox -icon error -message "You must select at least\n\
            THREE fiducial points for\n\ rigid transformation"
        return
    }
    
    set cmd "Tab $Module(freezer)"
    set Module(freezer) ""
    eval $cmd
    
        set Matrix(currentDataList) ""
    #reset the view so that now only the output of the main slicer is shown again
    set Matrix(matricesview,visibility) 0
    set Matrix(mainview,visibility) 1
    AlignmentsUpdateMainViewVisibility
        
        #Unpack the Slice controls that are specific to the fiducials selection tab
    AlignmentsUnpackFidSelectScreenControls
     
    #set the slices that were visible in the fiducial selection mode to be 
    #visible here as well
    foreach s $Slice(idList) {
        set Slice($s,visibility) $Matrix($s,Fore,visibility)
        Slice($s,planeActor) SetVisibility $Matrix($s,Fore,visibility)
    }
       
    #make sure that the background color is changed back to the normal bgcolor
    set View(bgColor) "0.7 0.7 0.9"
    eval viewRen SetBackground $View(bgColor)
    
    #Remove the slice actors that were used in the Fiducial Selection mode
    #to show the volumes separately
    foreach s $Slice(idList) {
        viewRen RemoveActor Slice($s,Back,planeActor)
        viewRen RemoveActor Slice($s,Fore,planeActor)
        viewRen AddActor Slice($s,planeActor)
    }
    
    set Matrix(FiducialPickEntered) 0
        AlignmentsSetRegTabState normal    

    #Apply the rigid transformation
    AlignmentsLandTrans
    
    #Indicate to the user that registration is complete
    set Matrix(regMech) ""
    raise $Matrix(fAlignBegin)
       
        #Set the mode back to the normal view
    MainViewerSetMode

        MainSlicesSetOrientAll AxiSagCor
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsFidSelectCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsFidSelectCancel {} {
    global Matrix Module Gui Slice View Fiducials

    set Matrix(currentDataList) ""
    #Unpack the Slice controls that are specific to the fiducials selection tab
    AlignmentsUnpackFidSelectScreenControls 

    #unfreeze
    set Matrix(freeze) 0
    
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
    
    set Matrix(regMech) ""
    raise $Matrix(fAlignBegin)

    #reset the view so that now only the reference volume is displayed again
    set Matrix(matricesview,visibility) 0
    set Matrix(mainview,visibility) 1
    AlignmentsUpdateMainViewVisibility
    
    #set the slices that were visible before to be visible here as well
    foreach s $Slice(idList) {
        set Slice($s,visibility) $Matrix($s,Fore,visibility)
        Slice($s,planeActor) SetVisibility $Matrix($s,Fore,visibility)
    } 
          
    #pack the original controls
    AlignmentsUnpackFidSelectScreenControls

    #make sure that the background color is changed back to the normal bgcolor
    set View(bgColor) "0.7 0.7 0.9"
    eval viewRen SetBackground $View(bgColor)
  
    AlignmentsSetActiveSlicer Slicer
  
    #show the original slice actors again and set the 2D slices
    foreach s $Slice(idList) {
        viewRen RemoveActor Slice($s,Back,planeActor)
        viewRen RemoveActor Slice($s,Fore,planeActor)
        viewRen AddActor Slice($s,planeActor)
        AlignmentsSetSliceWindows $Matrix(activeSlicer) 
    }

       
    #Delete the Fiducials Lists and any associate text boxes
    AlignmentsDeleteFiducialsList $Matrix(FidAlignRefVolumeName) $Matrix(FidRefVolPointBoxes)
    AlignmentsDeleteFiducialsList $Matrix(FidAlignVolumeName) $Matrix(FidVolPointBoxes)

    set Matrix(FiducialPickEntered) 0
    AlignmentsSetRegTabState normal
    MainViewerSetMode
    MainSlicesSetOrientAll AxiSagCor 
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetOrientAll
# 
#  change orientation of slices (depends on which slicer you have active) 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetOrientAll {orient} {
    global Slice View Matrix Gui
 
    if {$Matrix(activeSlicer) == "Slicer"} {
        set cam $View(viewCam)
    } else {
        set cam $View(MatCam)
    }

    $Matrix(activeSlicer) ComputeNTPFromCamera $cam
    $Matrix(activeSlicer) SetOrientString $orient

    foreach s $Slice(idList) {
        if {$Matrix(activeSlicer) == "Slicer"} {
            set sliceorient Slice($s,orient)
            set sliceoffset Slice($s,offset)
        } else {
            set sliceorient Matrix($s,orient)
            set sliceoffset Matrix($s,offset)
        }
    
        set orient [$Matrix(activeSlicer) GetOrientString $s]
        set $sliceorient $orient
    
        # Always update Slider Range when change Back volume or orient
        AlignmentsSlicesSetSliderRange $s
    
        # Set slider increments
        #MainSlicesSetOffsetIncrement $s
    
        # Set slider to the last used offset for this orient
        eval set $sliceoffset [$Matrix(activeSlicer) GetOffset $s]

        # Change text on menu button
        set f .tViewer.fMatMidSlicer.fMatMidSlice$s.fOrient
        eval $f.mbOrient$s configure -text $$sliceorient
        eval $Gui(fSlice$s).fThumbMat.fOrientMat.lOrientMat configure -text $$sliceorient
    
        #TBD: this anno stuff needs to be fixed?
        #Need a Matrix($s,orient)
        # Anno TBD - see the other orient method
        #should be MainSlicesSetAnno $s $sliceorient
        MainSlicesSetAnno $s $orient
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetOrient
# 
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetOrient {s orient} {
    global Slice Volume View Module Matrix Gui 

    if {$Matrix(activeSlicer) == "Slicer"} {
    set cam View(viewCam)
    set sliceorient Slice($s,orient)
    set sliceoffset Slice($s,offset)
    } else {
    set cam View(MatCam)
    set sliceorient Matrix($s,orient)
    set sliceoffset Matrix($s,offset)
    }

    eval $Matrix(activeSlicer) ComputeNTPFromCamera $$cam
   
    $Matrix(activeSlicer) SetOrientString $s $orient

    set $sliceorient [$Matrix(activeSlicer) GetOrientString $s]
    
    # Always update Slider Range when change Back volume or orient
    AlignmentsSlicesSetSliderRange $s

    # Set slider increments
    #AlignmentsSlicesSetOffsetIncrement $s

    #Set slider to the last used offset for this orient
    set $sliceoffset [$Matrix(activeSlicer) GetOffset $s]
    
    # Change text on menu button
    set f .tViewer.fMatMidSlicer.fMatMidSlice$s.fOrient
    eval $f.mbOrient$s configure -text $orient
    eval $Gui(fSlice$s).fThumbMat.fOrientMat.lOrientMat configure -text $$sliceorient

    # Anno
    MainSlicesSetAnno $s $orient 
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesOffsetUpdated
# Configures the sliders and corresponding entry boxes when the slice offsets 
# have changed for one of the datasets in the split view. 
#
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesOffsetUpdated {s} {
    global Matrix Slice Gui

    set f .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset

    if {$Matrix(activeSlicer) == "Slicer"} {

    #configure the slider's entry box
    AlignmentsSlicesSetOffset $s Slicer
    
    .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.eOffset \
        configure -textvariable Slice($s,offset)

    #configure the slider bar
    AlignmentsSlicesSetOffsetInit $s $f Slicer
    .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset \
        configure -variable Slice($s,offset)
 
    } else {    
    #configure the slider's entry box
    AlignmentsSlicesSetOffset $s MatSlicer

    .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.eOffset \
        configure -textvariable Matrix($s,offset)

    #configure the slider
    AlignmentsSlicesSetOffsetInit $s $f MatSlicer

    .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset \
        configure -variable Matrix($s,offset)
    }
RenderBoth $s
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetSliderRange
# Set the max and min values reachable with the slice selection slider.
# Called when the volume in the background changes 
# (in case num slices, resolution have changed)
# .ARGS
# s int slice window (0,1,2)
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetSliderRange {s} {
    global Slice Matrix Gui

    if {$Matrix(activeSlicer) == "Slicer"} {
    set offset Slice($s,offset)
    } else { 
    set offset Matrix($s,offset)
    }

    set lo [Slicer GetOffsetRangeLow  $s]
    set hi [Slicer GetOffsetRangeHigh $s]

    eval .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset configure -from $lo -to $hi
    #Thumbnails
    eval $Gui(fSlice$s).fControlsMat.fOffset.sOffset configure -from $lo -to $hi

    # Update Offset 
    eval set $$offset [$Matrix(activeSlicer) GetOffset $s]
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetOffsetIncrement
# Set the increment by which the slice slider should move.
# The default in the slicer is 1, which is 1 mm.
# Note this procedure will force increment to 1 if in any
# of the Slices orientations which just grab original data from the array.
# In this case the increment would mean 1 slice instead of 1 mm.
# .ARGS
# s int slice number (0,1,2)
# incr float increment slider should move by. is empty str if called from GUI
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetOffsetIncrement {s {incr ""}} {
    global Slice Matrix

    if {$Matrix(activeSlicer) == "Slicer"} {
    set offsetIncr Slice($s,offsetIncrement)
    } else { 
    set offsetIncr Matrix($s,offsetIncrement)
    }

    # set slider increments to 1 if in original orientation
    set orient [$Matrix(activeSlicer) GetOrientString $s]
    if {$orient == "AxiSlice" || $orient == "CorSlice" \
        || $orient == "SagSlice" || $orient == "OrigSlice" } {
    set incr 1    
    }
    
    # if called without an incr arg it's from user entry
    if {$incr == ""} {

    if { [ValidateFloat $$offsetIncr] == 0} {
        tk_messageBox -message "The increment must be a number."
        
        #reset the incr
        eval set $$offsetIncr 1
        return
    }

    # if user-entered incr is okay then do the rest of the procedure
    eval set incr $$offsetIncr
    }

    # Change Slice's offset increment variable
    eval set $$offsetIncr $incr

    # Make the slider allow this resolution
    .tViewer.fMatMidSlicer.fMatMidSlice$s.fOffset.sOffset configure -resolution $incr    

}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetOffset
# Set the slice offset  
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetOffset {s whichSlicer {value ""}} {
    global Slice Matrix
    
    if {$Matrix(activeSlicer) == "Slicer"} {
    set offsetval $Slice($s,offset)
    } else {
    set offsetval $Matrix($s,offset)
    }

    if {$value == ""} { 
    set value $offsetval
    } elseif {$value == "Prev"} {
    set value [expr $offsetval - 1]
    
    } elseif {$value == "Next"} {
    set value [expr $offsetval + 1]
    }

    if {[ValidateFloat $value] == 0}  {
    set value [$Matrix(activeSlicer) GetOffset $s]
    } 
 
    if {$Matrix(activeSlicer) == "Slicer"} {
    set Slice($s,offset) $value
    Slicer SetOffset $s $value
    AlignmentsSlicesRefreshClip $s Slicer
    } else {
    set Matrix($s,offset) $value
    MatSlicer SetOffset $s $value
    AlignmentsSlicesRefreshClip $s MatSlicer
    }

 Render3D
 RenderSlices
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetOffsetInit
# wrapper around MainSlicesSetOffset. Also calls RenderBoth
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetOffsetInit {s widget whichSlicer {value ""}} {

    # This prevents Tk from calling RenderBoth when it first creates
    # the slider, but before a user uses it.
    $widget config -command "AlignmentsSlicesSetOffset $s $whichSlicer; RenderBoth $s"
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetVisibilityAll
#
# Callback for the V button on the Slice controls frame of the split screen.
# Sets the visibility to be on or off for all the slices for the dataset selected
# 
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetVisibilityAll {{value ""}} {
    global Matrix Slice Matrix
    
    if {$Matrix(activeSlicer) == "Slicer"} {
    set layer Fore
    } else {
    set layer Back
    }

    if {$value != ""} {
        set Matrix(visibilityAll,$layer) $value
    }

    foreach s $Slice(idList) {
        set Matrix($s,$layer,visibility) $Matrix(visibilityAll,$layer)
        Slice($s,$layer,planeActor) SetVisibility $Matrix($s,$layer,visibility)  
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesSetVisibility
# Callback for the V button on the Slice controls frame of the split screen.
# Sets the visibility to be on or off for the indicated slice of the dataset
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesSetVisibility {s} {
    global Slice Matrix
   
    if {$Matrix(activeSlicer) == "Slicer"} {
    set layer Fore
    } else {
    set layer Back
    }

    Slice($s,$layer,planeActor) SetVisibility $Matrix($s,$layer,visibility) 
    
    # If any slice is invisible, then Slice(visibilityAll) should be 0
    set Matrix(visibilityAll,$layer) 1
    foreach s $Slice(idList) {
        if {$Matrix($s,$layer,visibility) == 0} {
            set Matrix(visibilityAll,$layer) 0
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsSlicesRefreshClip
# Update clipping.
# Set normal and origin of clip plane using current
# info from vtkMrmlSlicer's reformat matrix.
# .ARGS
# s int slice id (0,1,2)
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSlicesRefreshClip {s whichSlicer} {
    global Slice
    
    # Set normal and orient of slice
    if {$Slice($s,clipState) == "1"} {
        set sign 1
    } elseif {$Slice($s,clipState) == "2"} {
        set sign -1
    } else {
        return
    }
    set mat [$whichSlicer GetReformatMatrix $s]

    set origin "[$mat GetElement 0 3] \
        [$mat GetElement 1 3] [$mat GetElement 2 3]"

    set normal "[expr $sign*[$mat GetElement 0 2]] \
        [expr $sign*[$mat GetElement 1 2]] \
        [expr $sign*[$mat GetElement 2 2]]"

    # WARNING: objects may not exist yet!
    if {[info command Slice($s,clipPlane)] != ""} {
        eval Slice($s,clipPlane) SetOrigin  $origin     
        eval Slice($s,clipPlane) SetNormal $normal
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetSliceWindows
#
# Sets all the slice windows to show the output of the active slicer
#
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetSliceWindows {whichSlicer} {
    global Slice Volume Matrix
 
   foreach s $Slice(idList) { 
       sl${s}Mapper SetInput [$whichSlicer GetCursor $s]
    }   
    RenderSlices
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsLandTrans
#
# Apply fiducial using vtkLandmarkTransform
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsLandTrans {} {
    global Matrix Volume Fiducials View
    
    set t $Matrix(activeID)

    # Compute the landmark based transform
    vtkLandmarkTransform landTrans
    landTrans SetModeToRigidBody
    
    #set the Source landmarks
    set id $Fiducials($Matrix(FidAlignVolumeName),fid)
    landTrans SetSourceLandmarks Fiducials($id,points)

    #set the Target landmarks
    set id $Fiducials($Matrix(FidAlignRefVolumeName),fid)
    landTrans SetTargetLandmarks Fiducials($id,points) 
        
    # Transfer values from landmark to active transform
    set tran [Matrix($t,node) GetTransform]
    $tran SetMatrix [landTrans GetMatrix]

    # Update all MRML
    MainUpdateMRML
    Render$Matrix(render)
    landTrans Delete

    #Delete the fiducial lists 
    AlignmentsDeleteFiducialsList $Matrix(FidAlignRefVolumeName) $Matrix(FidRefVolPointBoxes)
    AlignmentsDeleteFiducialsList $Matrix(FidAlignVolumeName) $Matrix(FidVolPointBoxes)
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsSetActiveSlicer
# TBD:fix this
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetActiveSlicer {whichSlicer} {
    global Matrix Interactor
    set Matrix(activeSlicer) $whichSlicer
    set Interactor(Slicer) $whichSlicer
}



#-------------------------------------------------------------------------------
# .PROC AlignmentsViewerUpdate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsViewerUpdate {} {
    global Module View Gui Slice Matrix

    # do nothing if we are in some other module
    if {$Module(activeID) != "Alignments" || $Matrix(FiducialPickEntered) != 1} {
        return
    }         
    switch $View(mode) {
    "Normal" {
    #pack forget all the Fiducial Pick Frames
    pack forget $Gui(fTop) $Gui(fMid) $Gui(fBot)
    pack forget $Gui(fTop) $Gui(fMatVol) $Gui(fMatMidSlicer) $Gui(fBot)
    #Repack the GUI
    pack $Gui(fTop) -side top
    pack $Gui(fMatVol) -side top -expand 0 -fill x 
    pack $Gui(fMatMidSlicer) -side top -expand 1 -fill x
    pack $Gui(fBot) -side top
    } 
    "3D" {
    pack forget $Gui(fMatVol) $Gui(fMatMidSlicer)
    }
    "Quad256" {
    pack forget $Gui(fMatVol) $Gui(fMatMidSlicer)
    foreach s $Slice(idList) {
        lower $Gui(fSlice$s).fThumb
        raise $Gui(fSlice$s).fThumbMat
    }
    }
    "Quad512" {
    # remove middle control stuff we added
    pack forget $Gui(fMatVol) $Gui(fMatMidSlicer)
    # make our controls pop up when mouse goes over Orient thumbnail
    # (instead of original slicer controls)
     foreach s $Slice(idList) {
         #raise $Gui(fSlice$s).fThumb
         # undo the normal raise that just happened
         lower $Gui(fSlice$s).fThumb
         # raise ours instead
         raise $Gui(fSlice$s).fThumbMat
     }
    }
    "Single512" {
    pack forget $Gui(fMatVol) $Gui(fMatMidSlicer)
    foreach s $Slice(idList) {
         #raise $Gui(fSlice$s).fThumb
         # undo the normal raise that just happened
         lower $Gui(fSlice$s).fThumb
         # raise ours instead
         raise $Gui(fSlice$s).fThumbMat
        }
    }
    }
    if {$View(mode) == "Normal" || $View(mode) == "Quad256"} {
    foreach s $Slice(idList) {
        MatSlicer SetDouble $s 0
        MatSlicer SetCursorPosition $s 128 128
        }
    } elseif {$View(mode) == "Quad512"} {
    foreach s $Slice(idList) {
        MatSlicer SetDouble $s 1
        MatSlicer SetCursorPosition $s 256 256
    }
    } elseif {$View(mode) == "Single512"} {
    foreach s $Slice(idList) {
        MatSlicer SetDouble $s 0
        MatSlicer SetCursorPosition $s 128 128
        }
    set s 0
    MatSlicer SetDouble $s 1
    MatSlicer SetCursorPosition $s 256 256
    }    
    MatSlicer Update
}



#-------------------------------------------------------------------------------
# .PROC AlignmentsTestSliceControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsTestSliceControls {} {
    global Gui View viewWin Slice Module

    foreach s $Slice(idList) {

 
        set f $Gui(fSlice$s)

        frame $f.fThumbMat    -bg $Gui(activeWorkspace)
        frame $f.fControlsMat -bg $Gui(activeWorkspace)

        # Raise this window to the front when the mouse passes over it.
        place $f.fControlsMat -in $f -relx 1.0 -rely 0.0 -anchor ne 
        bind $f.fControlsMat <Leave> "AlignmentsViewerHideSliceControls"

        # Raise this window to the front when view mode is Quad256 or Quad512
        place $f.fThumbMat -in $f -relx 1.0 -rely 0.0 -anchor ne

        #-------------------------------------------
        # Slice$s->Thumb frame
        #-------------------------------------------
        set f $Gui(fSlice$s).fThumbMat

        frame $f.fOrientMat -bg $Gui(slice$s)
        pack $f.fOrientMat -side top

        # Orientation
        #-------------------------------------------
        set f $Gui(fSlice$s).fThumbMat.fOrientMat

        eval {label $f.lOrientMat -text "INIT" -width 12} \
            $Gui(WLA) {-bg $Gui(slice$s)}
        pack $f.lOrientMat
# ljo what do we do here?
#        lower $f.lOrientMat

        # Show the full controls when the mouse enters the thumbnail
        bind $f.lOrientMat <Enter>  "AlignmentsViewerShowSliceControls $s"

        #-------------------------------------------
        # Slice$s->Controls frame
        #-------------------------------------------
        set f $Gui(fSlice$s).fControlsMat

        AlignmentsBuildControls $s $f Slicer

    lower $Gui(fSlice$s).fControlsMat 
  }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsViewerShowSliceControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsViewerShowSliceControls {s} {
    global Gui Slice Volume View Matrix

    if {$Matrix(FiducialPickEntered) == 1} {
    raise $Gui(fSlice$s).fControlsMat
    }
}
#-------------------------------------------------------------------------------
# .PROC AlignmentsViewerHideSliceControls
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsViewerHideSliceControls {} {
    global Gui Slice Volume View Matrix

    if {$Matrix(FiducialPickEntered) == 1} {
    lower $Gui(fSlice0).fControlsMat $Gui(fSlice0).fImage
    lower $Gui(fSlice1).fControlsMat $Gui(fSlice1).fImage
    lower $Gui(fSlice2).fControlsMat $Gui(fSlice2).fImage
    }
}


#-------------------------------------------------------------------------------
# .PROC AlignmentsSetRegTabState
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsSetRegTabState {mode} {
    global Matrix
  
    #These only affect TPS and FidAlign because of the split view
    switch $Matrix(regMech) {
        "FidAlign" {
            $Matrix(rTPS) config -state $mode
            $Matrix(rMI) config -state $mode
        }
        "TPS" {
        $Matrix(rFidAlign) config -state $mode
            $Matrix(rMI) config -state $mode
        }
    default {
        $Matrix(rFidAlign) config -state $mode
        $Matrix(rTPS) config -state $mode
            $Matrix(rMI) config -state $mode
    }
    }
    #set the volume buttons 
    $Matrix(mbVolume) config -state $mode
    $Matrix(mbRefVolume) config -state $mode
}





#-------------------------------------------------------------------------------
# .PROC AlignmentsSetColorCorrespondence
# 
# Shows the amount of convergence given two volumes, reference volume and the 
# volume to move. Colors the reference volume red and the volume to move blue.
#
# NOTE: Im setting the correspondence colors to be "desert" and "ocean" 
# if these names change, then this needs to be changed too 
#
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc AlignmentsSetColorCorrespondence {} {
    global Matrix Lut Volume

    #Check to see that the volume to move and the reference volume are set.
    if {$Matrix(volume) == $Volume(idNone) || $Matrix(refVolume) == $Volume(idNone)} {
    tk_messageBox -icon error -message "Either the reference volume or the volume to 
            move has not yet been set" 
    set Matrix(regMech) ""
    raise $Matrix(fAlignBegin)
    return
    }
    
    if {$Matrix(colorCorresp) == 1} {
    
     MainSlicesSetVolumeAll Fore $Matrix(refVolume)
    MainSlicesSetVolumeAll Back $Matrix(volume)
    
        #set the reference volume to be red
        set Volume(activeID) $Matrix(refVolume) 
    foreach v $Lut(idList) {
            if {$Lut($v,name) == "Desert"} {
        MainVolumesSetParam LutID $v
        MainVolumesSetParam AutoThreshold 1    
        }
    }
    
        #set the volume to move to be blue
    set Volume(activeID) $Matrix(volume) 
    foreach v $Lut(idList) {
        if {$Lut($v,name) == "Ocean"} {
        MainVolumesSetParam LutID $v
        MainVolumesSetParam AutoThreshold 1
        }
    }
    
    #if the color correspondence button is set to off then set both volumes to grey
    } elseif {$Matrix(colorCorresp) == 0} {
    foreach v $Lut(idList) {
        if {$Lut($v,name) == "Gray"} {
        set Volume(activeID) $Matrix(refVolume) 
        MainVolumesSetParam LutID $v
        MainVolumesSetParam AutoThreshold
        set Volume(activeID) $Matrix(volume) 
        MainVolumesSetParam LutID $v
        MainVolumesSetParam AutoThreshold
        }
    }
    }
    RenderAll
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsMainFileCloseUpdated
# Called when File close is seleceted from the main menu.
# Added this because the new slice actors allowing the viewing of the split 
# view were not being deleted when the file was closed.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsMainFileCloseUpdated {} {
    global Matrix Slice

    if { $Matrix(FiducialPickEntered) == 1} {
       puts "hello"
       AlignmentsFidSelectCancel
    }
}

#-------------------------------------------------------------------------------
# .PROC AlignmentsExit
# Called when the Alignments module is exit
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AlignmentsExit {} {
    global Matrix
    
    popEventManager 
    Render3D 

}

