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
# FILE:        Matrices.tcl
# PROCEDURES:  
#   MatricesInit
#   MatricesUpdateMRML
#   MatricesBuildGUI
#   MatricesIdentity
#   MatricesInvert
#   MatricesSetPropertyType
#   MatricesPropsApply
#   MatricesPropsCancel
#   MatricesManualTranslate
#   MatricesManualTranslateDual
#   MatricesManualRotate
#   MatricesSetRefVolume
#   MatricesSetVolume
#   MatricesWritePseudoMrmlVolume
#   MatricesAutoRun
#   MatricesAutoCancel
#   MatricesPoll
#   MatricesAutoApply
#   MatricesAutoUndo
#   MatricesB1
#   MatricesB1Motion
#   MatricesB1Release
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MatricesInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesInit {} {
    global Matrix Module Volume

    # Define Tabs
    set m Matrices
    set Module($m,row1List) "Help Props Manual Auto Fiducial"
    set Module($m,row1Name) "{Help} {Props} {Manual} {Auto} {Fiducial}"
    set Module($m,row1,tab) Manual

    # Module Summary Info
    set Module($m,overview) "Edit transformation matrices to move volumes/models."

    # Define Procedures
    set Module($m,procGUI)   MatricesBuildGUI
    set Module($m,procMRML)  MatricesUpdateMRML

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
            {$Revision: 1.30 $} {$Date: 2002/07/17 14:31:10 $}]

    # Props
    set Matrix(propertyType) Basic
    set Matrix(volumeMatrix) None

    set Matrix(autoOutput) mi-output.mrml
    set Matrix(autoInput)  mi-input.pmrml
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
    set Matrix(prevTranLR) 0
    set Matrix(prevTranPA) 0
    set Matrix(prevTranIS) 0
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0
    set Matrix(regRotIS) 0
    set Matrix(regRotPA) 0

    #Create a new renderer in order to display the split view
    vtkRenderer matRen
    lappend Module(Renderers) matRen
}

#-------------------------------------------------------------------------------
# .PROC MatricesUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesUpdateMRML {} {
    global Matrix Volume

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $Matrix(refVolume) ] == -1} {
        MatricesSetRefVolume $n
    }
    if {[lsearch $Volume(idList) $Matrix(volume) ] == -1} {
        MatricesSetVolume $n
    }

    # Menu of Volumes
    # ------------------------------------
    set m $Matrix(mbVolume).m
    $m delete 0 end
    # All volumes except none
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
            $m add command -label "[Volume($v,node) GetName]" \
                    -command "MatricesSetVolume $v"
        }
    }

    # Menu of Reference Volumes
    # ------------------------------------
    set m $Matrix(mbRefVolume).m
    $m delete 0 end
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
            $m add command -label "[Volume($v,node) GetName]" \
                    -command "MatricesSetRefVolume $v"
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MatricesBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesBuildGUI {} {
    global Gui Matrix Module Volume

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
    # Fiducial
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
    Description by tab:<BR>
    <UL>
    <LI><B>Props:</B> Directly set the matrix. The line of 16 numbers are
    in order of the first row, second row, etc. Click <B>Advanced</B> to
    copy one of the matrices derived from the header of a volume.
    <BR><LI><B>Manual:</B> Set the matrix using manual rigid registration
    with 6 degrees of freedom (3 translation, 3 rotation). 
    Either move the sliders, or click the left mouse button on a slice window 
    and drag it.  The <B>Mouse Action</B> buttons indicate whether to translate
    or rotate the volume in the Slice windows.  
    <BR><B>TIP:</B> The <B>Render</B> buttons indicate which windows to render
    as you move the sliders or mouse.  Rendering just one slice is much faster.
    <BR><LI><B>Auto:</B> Automatic registration using the method of Mutual 
    Information (MI) will set the matrix to the transformation matrix needed
    to align the <B>Volume to Move</B> with the <B>Reference Vol.</B>. 
    <BR><B>TIP:</B> Set the <B>Run Speed</B> to <I>Fast</I> if the 2 volumes 
    are already roughly aligned.
    <BR><B>Fiducial:</B> Fiducial registration is not available in this version.
    </UL>"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Matrices $help
    MainHelpBuildGUI  Matrices


    #-------------------------------------------
    # Props frame
    #-------------------------------------------
    set fProps $Module(Matrices,fProps)
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
                -text "$p" -command "MatricesSetPropertyType" \
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
            -command "MatricesPropsApply; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bCancel -text "Cancel" \
            -command "MatricesPropsCancel"} $Gui(WBA) {-width 8}
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

    # Menu of Matrices
    # ------------------------------------
    eval {label $f.l -text "Type of Matrix: "} $Gui(WLA)
    eval {menubutton $f.mb -text "$Matrix(volumeMatrix)" -relief raised -bd 2 -width 20 \
            -menu $f.mb.m} $Gui(WMBA)
    set Matrix(mbVolumeMatrix) $f.mb
    eval {menu $f.mb.m} $Gui(WMA)
    pack $f.l $f.mb -side left -pady $Gui(pad) -padx $Gui(pad)

    foreach v "None ScaledIJK->RAS RAS->IJK RAS->VTK" {
        $f.mb.m add command -label "$v" -command "MatricesSetVolumeMatrix $v"
    }

    #-------------------------------------------
    # Props->Bot->Advanced->Apply frame
    #-------------------------------------------
    set f $fProps.fBot.fAdvanced.fApply

    eval {button $f.bApply -text "Apply" \
            -command "MatricesPropsApply; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bCancel -text "Cancel" \
            -command "MatricesPropsCancel"} $Gui(WBA) {-width 8}
    grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Manual frame
    #-------------------------------------------
    set fManual $Module(Matrices,fManual)
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
            -command "MatricesIdentity; RenderAll"} $Gui(WBA) {-width 8}
    eval {button $f.bInvert -text "Invert" \
            -command "MatricesInvert; RenderAll"} $Gui(WBA) {-width 8}
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
                -width 4} $Gui(WEA)
        bind $f.e${slider} <Return> \
                "MatricesManualTranslate regTran${slider}"
        bind $f.e${slider} <FocusOut> \
                "MatricesManualTranslate regTran${slider}"

        eval {scale $f.s${slider} -from -240 -to 240 -length 120 \
                -command "MatricesManualTranslate regTran${slider}" \
                -variable Matrix(regTran${slider}) -resolution 1} $Gui(WSA)
        bind $f.s${slider} <Leave> "MatricesManualTranslate regTran$slider"

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
                -width 4} $Gui(WEA)
        bind $f.e${slider} <Return> \
                "MatricesManualRotate regRot${slider}"
        bind $f.e${slider} <FocusOut> \
                "MatricesManualRotate regRot${slider}"

        eval {scale $f.s${slider} -from -180 -to 180 -length 120 \
                -command "MatricesManualRotate regRot${slider}" \
                -variable Matrix(regRot${slider}) -resolution 1} $Gui(WSA)

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
    set fAuto $Module(Matrices,fAuto)
    set f $fAuto

    # Frames
    frame $f.fActive  -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fVolumes -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fDesc    -bg $Gui(activeWorkspace)
    frame $f.fSpeed   -bg $Gui(activeWorkspace)
    frame $f.fRun     -bg $Gui(activeWorkspace)

    pack $f.fActive  $f.fVolumes $f.fDesc $f.fSpeed $f.fRun \
            -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Auto->Active frame
    #-------------------------------------------
    set f $fAuto.fActive

    eval {label $f.lActive -text "Active Matrix: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Auto->Volumes frame
    #-------------------------------------------
    set f $fAuto.fVolumes

    eval {label $f.lVolume -text "Volume to Move"} $Gui(WLA)
    eval {menubutton $f.mbVolume -text "None" \
            -relief raised -bd 2 -width 13 -menu $f.mbVolume.m} $Gui(WMBA)
    eval {menu $f.mbVolume.m} $Gui(WMA)

    eval {label $f.lRefVolume -text "Reference Vol."} $Gui(WLA)
    eval {menubutton $f.mbRefVolume -text "None" \
            -relief raised -bd 2 -width 13 -menu $f.mbRefVolume.m} $Gui(WMBA)
    eval {menu $f.mbRefVolume.m} $Gui(WMA)
    grid $f.lVolume $f.mbVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
    grid $f.mbVolume -sticky w
    grid $f.lRefVolume $f.mbRefVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
    grid $f.mbRefVolume -sticky w

    # Append widgets to list that gets refreshed during UpdateMRML
    set Matrix(mbVolume) $f.mbVolume
    set Matrix(mbRefVolume) $f.mbRefVolume
    
    #-------------------------------------------
    # Auto->Desc frame
    #-------------------------------------------
    set f $fAuto.fDesc

    eval {label $f.l -justify left -text "\
            Press 'Run' to start the program\n\
            that performs automatic registration\n\
            by Mutual Information.\n\
            Your manual registration is used\n\
            as an initial pose.\
            "} $Gui(WLA)
    pack $f.l
    
    #-------------------------------------------
    # Auto->Speed Frame
    #-------------------------------------------
    set f $fAuto.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lSpeed -text "Run Speed: "} $Gui(WLA)
    pack $f.fTitle.lSpeed

    foreach text "Fast Slow" value "Fast Slow" \
            width "5 5" {
        eval {radiobutton $f.fBtns.rSpeed$value -width $width \
                -text "$text" -value "$value" -variable Matrix(autoSpeed) \
                -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rSpeed$value -side left -padx 0 -pady 0
    }

    #-------------------------------------------
    # Auto->Run frame
    #-------------------------------------------
    set f $fAuto.fRun

    foreach str "Run Cancel Undo" {
        eval {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
                -command "MatricesAuto$str"} $Gui(WBA)
        set Matrix(b$str) $f.b$str
    }
    pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
    set Matrix(bUndo) $f.bUndo
    $f.bUndo configure -state disabled

    #-------------------------------------------
    # Fiducial frame
    #-------------------------------------------
    set fFiducial $Module(Matrices,fFiducial)
    set f $fFiducial

    # Frames
    frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fPeter  -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack $f.fActive $f.fPeter \
            -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Fiducial->Active frame
    #-------------------------------------------
    set f $fFiducial.fActive

    eval {label $f.lActive -text "Active Matrix: "} $Gui(BLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
            -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.mbActive
    lappend Matrix(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Fiducial->Peter frame
    #-------------------------------------------
    set f $fFiducial.fPeter
}

#-------------------------------------------------------------------------------
proc MatricesSetVolumeMatrix {type} {
    global Matrix

    set Matrix(volumeMatrix) $type

    # update the button text
    $Matrix(mbVolumeMatrix) config -text $type
}

#-------------------------------------------------------------------------------
# .PROC MatricesIdentity
#
#  Resets the transformation to the identity, i.e. to the transform with
#  no effect.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesIdentity {} {
    global Matrix
    
    set m $Matrix(activeID)
    if {$m == ""} {return}
    
    [Matrix($m,node) GetTransform] Identity
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0
    set Matrix(regRotIS) 0
    set Matrix(regRotPA) 0
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC MatricesInvert
#
# Replaces the current transform with its inverse.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesInvert {} {
    global Matrix
    
    set m $Matrix(activeID)
    if {$m == ""} {return}
    
    [Matrix($m,node) GetTransform] Inverse
    set Matrix(rotAxis) "XX"
    set Matrix(regRotLR) 0
    set Matrix(regRotIS) 0
    set Matrix(regRotPA) 0
    MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC MatricesSetPropertyType
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesSetPropertyType {} {
    global Matrix
    
    raise $Matrix(f$Matrix(propertyType))
}

#-------------------------------------------------------------------------------
# .PROC MatricesPropsApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesPropsApply {} {
    global Matrix Label Module Mrml Volume

    # none?
    if {$Matrix(activeID) != "NEW" && $Matrix(idList) == ""} {
        tk_messageBox -message "Please create a matrix first from the Data panel"
        Tab Data row1 List
    }
    
    # Validate Input
    MatricesValidateMatrix

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
        $n SetID               $i

        # These get set down below, but we need them before MainUpdateMRML
        $n SetName $Matrix(name)

        Mrml(dataTree) AddItem $n
        MainUpdateMRML
        set Matrix(freeze) 0
        MainMatricesSetActive $i
        set m $i
    }

    # If user wants to use the matrix from a volume, set it here
    set v $Volume(activeID)
    switch $Matrix(volumeMatrix) {
        "None" {}
        "ScaledIJK->RAS" {
            MatricesSetMatrix [Volume($v,node) GetPositionMatrix]
        }
        "RAS->IJK" {
            MatricesSetMatrix [Volume($v,node) GetRasToIjkMatrix]
        }
        "RAS->VTK" {
            MatricesSetMatrix [Volume($v,node) GetRasToVtkMatrix]
        }
    }

    Matrix($m,node) SetName $Matrix(name)
    MatricesSetMatrixIntoNode $m

    # Return to Basic
    if {$Matrix(propertyType) == "Advanced"} {
        set Matrix(propertyType) Basic
        MatricesSetPropertyType
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
# .PROC MatricesPropsCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesPropsCancel {} {
    global Matrix Module

    # Reset props
    set m $Matrix(activeID)
    if {$m == "NEW"} {
        set m [lindex $Matrix(idList) 0]
    }
    set Matrix(freeze) 0
    MainMatricesSetActive $m

    # Return to Basic
    if {$Matrix(propertyType) == "Advanced"} {
        set Matrix(propertyType) Basic
        MatricesSetPropertyType
    }

    # Unfreeze
    if {$Module(freezer) != ""} {
        set cmd "Tab $Module(freezer)"
        set Module(freezer) ""
        eval $cmd
    }
}

#-------------------------------------------------------------------------------
# .PROC MatricesManualTranslate
#
# Adjusts the "translation part" of the transformation matrix.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesManualTranslate {param {value ""}} {
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
# .PROC MatricesManualTranslateDual
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesManualTranslateDual {param1 value1 param2 value2} {
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
# .PROC MatricesManualRotate
#
# Adjusts the "rotation part" of the transformation matrix.
# If the previous manual adjustment to the transform was a rotation
# about the same axis as the current adjustment, then only the change
# in the degrees specified is applied.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesManualRotate {param {value ""} {mouse 0}} {
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
# .PROC MatricesSetRefVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesSetRefVolume {{v ""}} {
    global Matrix Volume

    if {$v == ""} {
        set v $Matrix(refVolume)
    } else {
        set Matrix(refVolume) $v
    }

    $Matrix(mbRefVolume) config -text "[Volume($v,node) GetName]"
}

#-------------------------------------------------------------------------------
# .PROC MatricesSetVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesSetVolume {{v ""}} {
    global Matrix Volume

    if {$v == ""} {
        set v $Matrix(volume)
    } else {
        set Matrix(volume) $v
    }

    $Matrix(mbVolume) config -text "[Volume($v,node) GetName]"
}

#-------------------------------------------------------------------------------
#        AUTO REGISTRATION
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC MatricesWritePseudoMrmlVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesWritePseudoMrmlVolume {fid v} {
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
# .PROC MatricesAutoRun
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesAutoRun {} {
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

    MatricesWritePseudoMrmlVolume $fid $r
    puts $fid "matrix [Matrix($t,node) GetMatrix]"
    MatricesWritePseudoMrmlVolume $fid $v

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

    MatricesPoll
}

#-------------------------------------------------------------------------------
# .PROC MatricesAutoCancel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesAutoCancel {} {
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
# .PROC MatricesPoll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesPoll {} {
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
                MatricesAutoApply
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
    after 10000 MatricesPoll
}

#-------------------------------------------------------------------------------
# .PROC MatricesAutoApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesAutoApply {} {
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

#-------------------------------------------------------------------------------
# .PROC MatricesAutoUndo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesAutoUndo {} {
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
# .PROC MatricesB1
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesB1 {x y} {
    global Matrix Slice

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

#-------------------------------------------------------------------------------
# .PROC MatricesB1Motion
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesB1Motion {x y} {
    global Matrix Slice Interactor View Anno

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
                MatricesManualTranslateDual \
                        regTranLR [expr $xMm + $Matrix(prevTranLR)] \
                        regTranPA [expr $yMm + $Matrix(prevTranPA)]
            }
            Sagittal {
                # X:A->P, Y:I->S
                set xMm [expr -$xMm]
                set text "PA: $xMm, IS: $yMm mm"
                Anno($s,msg,mapper)  SetInput $text
                MatricesManualTranslateDual \
                        regTranPA [expr $xMm + $Matrix(prevTranPA)] \
                        regTranIS [expr $yMm + $Matrix(prevTranIS)]
            }
            Coronal {
                # X:R->L, Y:I->S
                set xMm [expr -$xMm]
                set text "LR: $xMm, IS: $yMm mm"
                Anno($s,msg,mapper)  SetInput $text
                MatricesManualTranslateDual \
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
                MatricesManualRotate regRotIS $degrees 1
            }
            Sagittal {
                # LR-axis
                set text "LR-axis: $degrees deg"
                Anno($s,msg,mapper)  SetInput $text
                MatricesManualRotate regRotLR $degrees 1
            }
            Coronal {
                # PA-axis
                set degrees [expr -$degrees]
                set text "PA-axis: $degrees deg"
                Anno($s,msg,mapper)  SetInput $text
                MatricesManualRotate regRotPA $degrees 1
            }
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC MatricesB1Release
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MatricesB1Release {x y} {
    global Matrix Slice

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

