#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        ModelMaker.tcl
# PROCEDURES:  
#   ModelMakerInit
#   ModelMakerUpdateMRML
#   ModelMakerBuildGUI
#   ModelMakerTransform
#   ModelMakerWrite
#   ModelMakerRead
#   ModelMakerEnter
#   ModelMakerSetVolume
#   ModelMakerCreate
#   ModelMakerLabelCallback
#   ModelMakerSmoothWrapper
#   ModelMakerSmooth
#   ModelMakerReverseNormals
#   ModelMakerMarch
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC ModelMakerInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerInit {} {
    global ModelMaker Module Volume

    set m ModelMaker

    # Module Summary Info
    set Module($m,overview) "Make 3D surface models from segmented data."
    set Module($m,author) "Core"
    set Module($m,category) "Application"

      # Define Tabs
    set Module($m,row1List) "Help Create Edit Save"
    set Module($m,row1Name) "{Help} {Create} {Edit} {Save} "
    set Module($m,row1,tab) Create

    # Define Procedures
    set Module($m,procGUI) ModelMakerBuildGUI
    set Module($m,procMRML) ModelMakerUpdateMRML
    set Module($m,procEnter) ModelMakerEnter

    # Define Dependencies
    set Module($m,depend) "Labels"

    # Set Version Info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.47 $} {$Date: 2004/04/13 21:00:08 $}]

    # Create
    set ModelMaker(idVolume) $Volume(idNone)
    set ModelMaker(name) skin
    set ModelMaker(smooth) 20
    set ModelMaker(decimate) 1
    set ModelMaker(marching) 0
    set ModelMaker(label2) 0
    set ModelMaker(UseSinc) 1

    # Edit
    set ModelMaker(edit,smooth) 20
    set ModelMaker(prefix) ""

    #### Splits normals at sharp points by point duplication
    ## Can be "Off" or "On"
    set ModelMaker(SplitNormals) On

    #### Calculates the Point Normals
    ## Can be "Off" or "On"
    set ModelMaker(PointNormals) On
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerUpdateMRML {} {
    global ModelMaker Volume

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {[lsearch $Volume(idList) $ModelMaker(idVolume)] == -1} {
        ModelMakerSetVolume $n
    }

    # Volume menu
    #---------------------------------------------------------------------------
    set m $ModelMaker(mVolume)
    $m delete 0 end
    foreach v $Volume(idList) {
        $m add command -label [Volume($v,node) GetName] -command \
            "ModelMakerSetVolume $v"
    }
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerBuildGUI {} {
    global Gui ModelMaker Model Module Label Matrix Volume

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Display
    #   Title
    #   All
    #   Grid
    # Properties
    # Clip
    #   Help
    #   Grid
    # Create
    # Edit
    #   
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
Description by Tab:<BR>
<UL>
<LI><B>Create:</B><BR>Set the <B>Volume</B> to the labelmap you wish to
create a surface model from.  When you press the <B>Create</B> button
a surface will be created that bounds all voxels with value equal to
<B>Label</B>. Use the <B>Edit</B> tab to apply additional smoothing,
or change the model's position.  The new model will not be written to 
hard disk until you save it using the <B>Save</B> tab.
<B>Filter Type</B> controls the type of smoothing done after the model is
built.  <B>Sinc</B> with 20 smoothing steps is the default as of January 2003; the 
smoothing in earlier version of Slicer used <B>Laplacian</B> with 5 smoothing
steps.
<BR><LI><B>Edit:</B><BR> Select the model you wish to edit as <B>Active Model</B>
and then apply one of the effects listed. To transform the polygon points
by a transform, select a <B>Matrix</B> that already exists.  If you need to
create one first, go to the <B>Data</B> module, and press the <B>Add Transform</B>
button.<BR>
<B>TIP</B> If you created a model using the voxel size, but no other header
information, such as the volume's position and orientation in space, then
transform the model to align with the volume under the <B>Transform from 
ScaledIJK to RAS</B> section.
<BR><LI><B>Save:</B> Write the model's polygon (*.vtk) file to disk.
Also save your MRML file by selecting <B>Save</B> from the <B>File</B> menu.
</UL>
"
    regsub -all "\n" $help { } help
    MainHelpApplyTags ModelMaker $help
    MainHelpBuildGUI ModelMaker

    #-------------------------------------------
    # Create frame
    #-------------------------------------------
    set fCreate $Module(ModelMaker,fCreate)
    set f $fCreate

    foreach frm " Volume Label Grid Apply Results Advanced Filter" {
        frame $f.f$frm -bg $Gui(activeWorkspace)
        pack  $f.f$frm -side top -pady $Gui(pad)
    }

    #-------------------------------------------
    # Create->Volume frame
    #-------------------------------------------
    set f $fCreate.fVolume

    # Volume menu
    eval {label $f.lVolume -text "Volume:"} $Gui(WLA)

    eval {menubutton $f.mbVolume -text "None" -relief raised -bd 2 -width 18 \
        -menu $f.mbVolume.m} $Gui(WMBA)
    eval {menu $f.mbVolume.m} $Gui(WMA)
    pack $f.lVolume -padx $Gui(pad) -side left -anchor e
    pack $f.mbVolume -padx $Gui(pad) -side left -anchor w

    # Save widgets for changing
    set ModelMaker(mbVolume) $f.mbVolume
    set ModelMaker(mVolume)  $f.mbVolume.m

    #-------------------------------------------
    # Create->Label frame
    #-------------------------------------------
    set f $fCreate.fLabel

    eval {button $f.bLabel -text "Label:" \
        -command "ShowLabels ModelMakerLabelCallback"} $Gui(WBA)
    eval {entry $f.eLabel -width 6 -textvariable Label(label)} $Gui(WEA)
    eval {entry $f.eLabel2 -width 6 -textvariable ModelMaker(label2)} $Gui(WEA)
    bind $f.eLabel <Return>   "LabelsFindLabel; ModelMakerLabelCallback"
    bind $f.eLabel <FocusOut> "LabelsFindLabel; ModelMakerLabelCallback"
    eval {entry $f.eName -width 10 \
        -textvariable Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bLabel $f.eLabel $f.eName \
        -padx $Gui(pad) -pady $Gui(pad) -sticky e

    lappend Label(colorWidgetList) $f.eName

    #-------------------------------------------
    # Create->Advanced frame
    #-------------------------------------------
    set f $fCreate.fAdvanced

    foreach frame "Title Choice" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    set f $fCreate.fAdvanced.fTitle

    eval {label $f.l -text "Advanced Options"} $Gui(WLA)
    eval {label $f.l2 -justify left -text "\nSplitting Normals is useful for visualizing \nsharp features. However, it creates holes\n in surfaces which affects measurements."} $Gui(WLA)
    pack $f.l $f.l2 -side top -pady 1

    set f $fCreate.fAdvanced.fChoice
    DevAddLabel $f.f "Split Normals:"
    pack $f.f -side left -padx $Gui(pad) 

    # frame $f.f -bg $Gui(activeWorkspace)
    foreach p "On Off" {
        eval {radiobutton $f.fr$p \
                -text "$p" \
                -variable ModelMaker(SplitNormals) -value $p -width 5 \
                -indicatoron 0} $Gui(WCA)
        pack $f.fr$p -side left -padx 0
    }

    #-------------------------------------------
    # Create->Grid frame
    #-------------------------------------------
    set f $fCreate.fGrid

    foreach Param "Name Smooth Decimate" width "13 7 7" {
        eval {label $f.l$Param -text "$Param:"} $Gui(WLA)
        eval {entry $f.e$Param -width $width \
            -textvariable ModelMaker([Uncap $Param])} $Gui(WEA)
        grid $f.l$Param $f.e$Param  -padx $Gui(pad) -pady $Gui(pad) -sticky e
        grid $f.e$Param -sticky w
    }

    #-------------------------------------------
    # Create->Filter frame
    #-------------------------------------------
    set f $fCreate.fFilter

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lFilter -text "Filter Type:"} $Gui(WLA)
    pack $f.fTitle.lFilter

    foreach text "Sinc Laplacian" value "1 0" \
        width "6 8" {
        eval {radiobutton $f.fBtns.rFilter$value -width $width \
            -text "$text" -value "$value" -variable ModelMaker(UseSinc) \
            -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rFilter$value -side left -pady 2
    }

    #-------------------------------------------
    # Create->Apply frame
    #-------------------------------------------
    set f $fCreate.fApply

    eval {button $f.bCreate -text "Create" -width 7 \
        -command "ModelMakerCreate; Render3D"} $Gui(WBA)
    pack $f.bCreate -side top -pady $Gui(pad)
    set ModelMaker(bCreate) $f.bCreate

    #-------------------------------------------
    # Create->Results frame
    #-------------------------------------------
    set f $fCreate.fResults

    eval {label $f.l -justify left -text ""} $Gui(WLA)
    pack $f.l -side top -pady 1
    set ModelMaker(msg) $f.l

    #-------------------------------------------
    # Edit frame
    #-------------------------------------------
    set fEdit $Module(ModelMaker,fEdit)
    set f $fEdit

    frame $f.fActive   -bg $Gui(activeWorkspace)
    frame $f.fGrid     -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fPosition -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fVolume   -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack  $f.fActive $f.fGrid $f.fPosition $f.fVolume \
        -side top -padx $Gui(pad) -pady 10 -fill x

    #-------------------------------------------
    # Edit->Active frame
    #-------------------------------------------
    set f $fEdit.fActive

    eval {label $f.lActive -text "Active Model: "} $Gui(WLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Model(mbActiveList) $f.mbActive
    lappend Model(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Edit->Grid frame
    #-------------------------------------------
    set f $fEdit.fGrid

    eval {label $f.lTitle -text "Apply an Effect"} $Gui(WTA)
    frame $f.fSmooth  -bg $Gui(activeWorkspace)
    frame $f.fReverse -bg $Gui(activeWorkspace)
    # BUG: Reverse doesn't seem to be working yet, so I've stripped it.
    pack $f.lTitle $f.fSmooth -side top -pady $Gui(pad)

    set Param Smooth
    set ff $f.fSmooth
    eval {label $ff.l$Param -text "$Param:"} $Gui(WLA)
    eval {entry $ff.e$Param -width 7 \
        -textvariable ModelMaker(edit,[Uncap $Param])} $Gui(WEA)
    eval {button $ff.b$Param -text "$Param" -width 7 \
        -command "ModelMakerSmoothWrapper; Render3D"} $Gui(WBA)
    grid $ff.l$Param $ff.e$Param $ff.b$Param \
        -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $ff.e$Param -sticky w


    set Param Reverse
    set ff $f.fReverse
    eval {label $ff.l$Param -text "Reverse Normals:"} $Gui(WLA)
    eval {button $ff.b$Param -text "$Param" -width 8 \
        -command "ModelMakerReverseNormals; Render3D"} $Gui(WBA)
    grid $ff.l$Param $ff.b$Param \
        -padx $Gui(pad) -pady $Gui(pad) -sticky e

        #-------------------------------------------
        # Edit->Grid->Filter frame  (added inside the Smooth frame)
        #-------------------------------------------
    set f $fEdit.fGrid.fSmooth.fFilter
    frame $f -bg $Gui(activeWorkspace)
    grid $f -columnspan 3

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    eval {label $f.fTitle.lFilter -text "Filter Type:"} $Gui(WLA)
    pack $f.fTitle.lFilter

    foreach text "Sinc Laplacian" value "1 0" \
        width "6 8" {
        eval {radiobutton $f.fBtns.rFilter$value -width $width \
            -text "$text" -value "$value" -variable ModelMaker(UseSinc) \
            -indicatoron 0} $Gui(WCA)
        pack $f.fBtns.rFilter$value -side left -padx 4 -pady 2
    }

    #-------------------------------------------
    # Edit->Position frame
    #-------------------------------------------
    set f $fEdit.fPosition

    eval {label $f.l -text "Transform by Any Matrix"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady $Gui(pad)

    eval {label $f.f.l -text "Matrix: "} $Gui(WLA)
    eval {menubutton $f.f.mb -text "None" -relief raised -bd 2 -width 13 \
        -menu $f.f.mb.m} $Gui(WMBA)
    eval {menu $f.f.mb.m} $Gui(WMA)
    eval {button $f.f.b -text "Apply" -width 6 \
        -command "ModelMakerTransform 0; Render3D"} $Gui(WBA)
    pack $f.f.l $f.f.mb $f.f.b -side left -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Matrix(mbActiveList) $f.f.mb
    lappend Matrix(mActiveList)  $f.f.mb.m

    #-------------------------------------------
    # Edit->Volume frame
    #-------------------------------------------
    set f $fEdit.fVolume

    eval {label $f.l -text "Transform from ScaledIJK to RAS"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady $Gui(pad)

    eval {label $f.f.l -text "Volume: "} $Gui(WLA)
    eval {menubutton $f.f.mb -text "None" -relief raised -bd 2 -width 13 \
        -menu $f.f.mb.m} $Gui(WMBA)
    eval {menu $f.f.mb.m} $Gui(WMA)
    eval {button $f.f.b -text "Apply" -width 6 \
        -command "ModelMakerTransform 1; Render3D"} $Gui(WBA)
    pack $f.f.l $f.f.mb $f.f.b -side left -padx $Gui(pad)

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.f.mb
    lappend Volume(mActiveList)  $f.f.mb.m

    #-------------------------------------------
    # Save frame
    #-------------------------------------------
    set fSave $Module(ModelMaker,fSave)
    set f $fSave

    frame $f.fActive -bg $Gui(activeWorkspace)
    frame $f.fWrite  -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack  $f.fActive $f.fWrite \
        -side top -padx $Gui(pad) -pady 10 -fill x

    #-------------------------------------------
    # Save->Active frame
    #-------------------------------------------
    set f $fSave.fActive

    eval {label $f.lActive -text "Active Model: "} $Gui(WLA)
    eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 

    # Append widgets to list that gets refreshed during UpdateMRML
    lappend Model(mbActiveList) $f.mbActive
    lappend Model(mActiveList)  $f.mbActive.m

    #-------------------------------------------
    # Save->Write frame
    #-------------------------------------------
    set f $fSave.fWrite

    eval {label $f.l1 -text "Save model as a VTK file"} $Gui(WTA)
    eval {label $f.l2 -text "File Prefix (without .vtk):"} $Gui(WLA)
    eval {entry $f.e -textvariable ModelMaker(prefix) -width 50} $Gui(WEA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l1 -side top -pady $Gui(pad) -padx $Gui(pad)
    pack $f.l2 -side top -pady $Gui(pad) -padx $Gui(pad) -anchor w
    pack $f.e -side top -pady $Gui(pad) -padx $Gui(pad) -expand 1 -fill x
    pack $f.f -side top -pady $Gui(pad) -padx $Gui(pad)

    eval {button $f.f.bSave -text "Save" -width 5 \
        -command "ModelMakerWrite; Render3D"} $Gui(WBA)
    eval {button $f.f.bRead -text "Read" -width 5 \
        -command "ModelMakerRead; Render3D"} $Gui(WBA)
    pack $f.f.bSave $f.f.bRead -side left -padx $Gui(pad)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerTransform
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerTransform {volume} {
    global ModelMaker Model Volume Matrix Module
    
    if {$volume == 1} {
        # See if the volume exists
        if {[lsearch $Volume(idList) $Volume(activeID)] == -1} {
            tk_messageBox -message "Please select a volume first."
            return
        }

        set m $Model(activeID)
        set v $Volume(activeID)
    
        set mat [Volume($v,node) GetPosition]
    } else {
        # See if the matrix exists
        if {[lsearch $Matrix(idList) $Matrix(activeID)] == -1} {
            tk_messageBox -message "Please select a matrix first."
            return
        }
        set m $Model(activeID)
        set v $Matrix(activeID)
    
        set mat [[Matrix($v,node) GetTransform] GetMatrix]
    }

    vtkTransform tran
    # special trick to avoid vtk 4.2 legacy hack message 
    # (adds a concatenated identity transform to the transform)
    if { [info commands __dummy_transform] == "" } {
        vtkTransform __dummy_transform
    }
    tran SetInput __dummy_transform
    tran Concatenate $mat

    vtkTransformPolyDataFilter transformer
    transformer SetInput $Model($m,polyData)
    transformer SetTransform tran
    [transformer GetOutput] ReleaseDataFlagOn
    transformer Update
    
    set p normals
    vtkPolyDataNormals $p
    $p ComputePointNormals$ModelMaker(PointNormals)
    $p SetInput [transformer GetOutput]
    $p SetFeatureAngle 60
    $p Splitting$ModelMaker(SplitNormals)
    [$p GetOutput] ReleaseDataFlagOn

    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""
    foreach p "transformer normals stripper" {
        $p SetInput ""
        $p Delete
    }
    tran Delete

    # Mark this model as unsaved
    set Model($m,dirty) 1
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerWrite {} {
    global ModelMaker Model

    # Show user a File dialog box
    set m $Model(activeID)
    set ModelMaker(prefix) [MainFileSaveModel $m $ModelMaker(prefix)]
    if {$ModelMaker(prefix) == ""} {return}

    # Write
    MainModelsWrite $m $ModelMaker(prefix)

    # Prefix changed, so update the Models->Props tab
    MainModelsSetActive $m
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerRead
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerRead {} {
    global ModelMaker Model Mrml

    # Show user a File dialog box
    set m $Model(activeID)
    set ModelMaker(prefix) [MainFileOpenModel $m $ModelMaker(prefix)]
    if {$ModelMaker(prefix) == ""} {return}
    
    # Read
    Model($m,node) SetFileName $ModelMaker(prefix).vtk
    Model($m,node) SetFullFileName \
        [file join $Mrml(dir) [Model($m,node) GetFileName]]
    if {[MainModelsRead $m] < 0} {
        return
    }

    # Prefix changed, so update the Models->Props tab
    MainModelsSetActive $m
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerEnter {} {
    global Volume
    
    ModelMakerSetVolume $Volume(activeID)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSetVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSetVolume {v} {
    global ModelMaker Volume Label

    set ModelMaker(idVolume) $v
    
    # Change button text
    $ModelMaker(mbVolume) config -text [Volume($v,node) GetName]

    # Initialize the label to the highest value in the volume
    set Label(label) [Volume($v,vol) GetRangeHigh]
    LabelsFindLabel
    ModelMakerLabelCallback
    set ModelMaker(label2) [Volume($v,vol) GetRangeLow]
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerCreate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerCreate {} {
    global Model ModelMaker Label

    # Validate name
    if {$ModelMaker(name) == ""} {
        tk_messageBox -message "Please enter a name that will allow you to distinguish this model."
        return
    }
    if {[ValidateName $ModelMaker(name)] == 0} {
        tk_messageBox -message "The name can consist of letters, digits, dashes, or underscores"
        return
    }

    # Validate smooth
    if {[ValidateInt $ModelMaker(smooth)] == 0} {
        tk_messageBox -message "The number of smoothing iterations must be an integer."
        return
    }

    # Validate decimate
    if {[ValidateInt $ModelMaker(decimate)] == 0} {
        tk_messageBox -message "The number of decimate iterations must be an integer."
        return
    }

    # Disable button to prevent another
    $ModelMaker(bCreate) config -state disabled

    # Create the model's MRML node
    set n [MainMrmlAddNode Model]
    $n SetName  $ModelMaker(name)
    $n SetColor $Label(name)

    # Guess the prefix
    set ModelMaker(prefix) $ModelMaker(name)

    # Create the model
    set m [$n GetID]
    MainModelsCreate $m

    # Registration
    set v $ModelMaker(idVolume)
    Model($m,node) SetRasToWld [Volume($v,node) GetRasToWld]

    if {[ModelMakerMarch $m $v $ModelMaker(decimate) $ModelMaker(smooth)] != 0} {
        MainModelsDelete $m
        $ModelMaker(bCreate) config -state normal
        return
    }
    $ModelMaker(msg) config -text "\
Marching cubes: $ModelMaker(t,mcubes) sec.\n\
Decimate: $ModelMaker(t,decimator) sec.\n\
Smooth: $ModelMaker(t,smoother) sec.\n\
$ModelMaker(n,mcubes) polygons reduced to $ModelMaker(n,decimator)."

    # put the model inside the same transform as the source volume
    set nitems [Mrml(dataTree) GetNumberOfItems]
    for {set midx 0} {$midx < $nitems} {incr midx} {
        if { [Mrml(dataTree) GetNthItem $midx] == "Model($m,node)" } {
            break
        }
    }
    if { $midx < $nitems } {
        Mrml(dataTree) RemoveItem $midx
        Mrml(dataTree) InsertAfterItem Volume($v,node) Model($m,node)
        MainUpdateMRML
    }


    MainUpdateMRML
    MainModelsSetActive $m
    $ModelMaker(bCreate) config -state normal


    set name [Model($m,node) GetName]
    # tk_messageBox -message "The model '$name' has been created."

    return $m
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerLabelCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerLabelCallback {} {
    global Label ModelMaker

    set ModelMaker(name)   $Label(name)

    set ModelMaker(label2) $Label(label)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSmoothWrapper
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSmoothWrapper {{m ""}} {
    global Model ModelMaker

    if {$m == ""} {
        set m $Model(activeID)
    }
    if {$m == ""} {return}

    # Validate smooth
    if {[ValidateInt $ModelMaker(edit,smooth)] == 0} {
        tk_messageBox -message "The number of smoothing iterations must be an integer."
        return
    }

    ModelMakerSmooth $m $ModelMaker(edit,smooth)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSmooth
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSmooth {m iterations} {
    global Model Gui ModelMaker Module

    set name [Model($m,node) GetName]

    set p smoother
    if { $ModelMaker(UseSinc) == 1} {
        vtkWindowedSincPolyDataFilter $p
        $p SetPassBand .1
    } else {
        # Laplacian
        vtkSmoothPolyDataFilter $p
    # This next line massively rounds corners
        $p SetRelaxationFactor .33
        $p SetFeatureAngle 60
        $p SetConvergence 0
    }
    
    $p SetInput $Model($m,polyData)
    $p SetNumberOfIterations $iterations
    $p FeatureEdgeSmoothingOff
    $p BoundarySmoothingOff
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Smoothing $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
    set ModelMaker($m,nPolys) $ModelMaker(n,$p)

    set p normals
    vtkPolyDataNormals $p
    $p ComputePointNormals$ModelMaker(PointNormals)
    $p SetInput [smoother GetOutput]
    $p SetFeatureAngle 60
    $p Splitting$ModelMaker(SplitNormals)
    [$p GetOutput] ReleaseDataFlagOn

    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
    Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""
    foreach p "smoother normals stripper" {
        $p SetInput ""
        $p Delete
    }

    # Mark this model as unsaved
    set Model($m,dirty) 1
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerReverseNormals
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ModelMakerReverseNormals {{m ""}} {
    global Model Gui ModelMaker Module

    if {$m == ""} {
        set m $Model(activeID)
    }
    if {$m == ""} {return}

    set name [Model($m,node) GetName]

    set p reverser
    vtkReverseSense $p
    $p SetInput $Model($m,polyData)
    $p ReverseNormalsOn
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Reversing $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress

    set p stripper
    vtkStripper $p
    $p SetInput [reverser GetOutput]
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""
    foreach p "reverser stripper" {
        $p SetInput ""
        $p Delete
    }

    # Mark this model as unsaved
    set Model($m,dirty) 1
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerMarch
# Polina Goland (polina@ai.mit.edu) helped create this routine.  The example
# on Bill Lorensen's web site was adapted to exploit our vtkToRasMatrix.
# .END
#-------------------------------------------------------------------------------
proc ModelMakerMarch {m v decimateIterations smoothIterations} {
    global Model ModelMaker Gui Label Module
    
    if {$ModelMaker(marching) == 1} {
        puts "already marching"
        return -1
    }

    set ModelMaker(marching) 1
    set name [Model($m,node) GetName]

    # Marching cubes cannot run on data of dimension less than 3
    set dim [[Volume($v,vol) GetOutput] GetExtent]
    if {[lindex $dim 0] == [lindex $dim 1] ||
        [lindex $dim 2] == [lindex $dim 3] ||
        [lindex $dim 4] == [lindex $dim 5]} {
        puts "extent=$dim"
        tk_messageBox -message "The volume '[Volume($v,node) GetName]' is not 3D"
        set ModelMaker(marching) 0
        return -1
    }

    set spacing [[Volume($v,vol) GetOutput] GetSpacing]
    set origin  [[Volume($v,vol) GetOutput] GetOrigin]
    # The spacing is accounted for in the rasToVtk transform, 
    # so we have to remove it here, or mcubes will use it.
    [Volume($v,vol) GetOutput] SetSpacing 1 1 1
    [Volume($v,vol) GetOutput] SetOrigin 0 0 0
    
    # Read orientation matrix and permute the images if necessary.
    vtkTransform rot

    # special trick to avoid vtk 4.2 legacy hack message 
    # (adds a concatenated identity transform to the transform)
    if { [info commands __dummy_transform] == "" } {
        vtkTransform __dummy_transform
    }
    rot SetInput __dummy_transform

    set matrixList [Volume($v,node) GetRasToVtkMatrix]
    eval rot SetMatrix $matrixList
    eval rot Inverse

    # Threshold so the only values are the desired label.
    # But do this only for label maps
# BUG crashes:    $p ThresholdBetween $Label(label) $ModelMaker(label2)
    set p thresh
    vtkImageThreshold $p
    $p SetInput [Volume($v,vol) GetOutput]
    $p SetReplaceIn 1
    $p SetReplaceOut 1
    $p SetInValue 200
    $p SetOutValue 0
    $p ThresholdBetween $Label(label) $Label(label)
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Threshold $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress

    vtkImageToStructuredPoints to
    to SetInput [thresh GetOutput]
    to Update

    set p mcubes
    vtkMarchingCubes $p
    $p SetInput [to GetOutput]
    $p SetValue 0 100.5
    $p ComputeScalarsOff
    $p ComputeGradientsOff
    $p ComputeNormalsOff
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Marching $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]

    # If there are no polygons, then the smoother gets mad, so stop.
    if {$ModelMaker(n,$p) == 0} {
        tk_messageBox -message "No polygons can be created."
        thresh SetInput ""
        to SetInput ""
        mcubes SetInput ""
        rot Delete
        thresh Delete
        to Delete
        mcubes Delete
        set ModelMaker(marching) 0
        eval [Volume($v,vol) GetOutput] SetSpacing $spacing
        eval [Volume($v,vol) GetOutput] SetOrigin $origin
        return -1
    }

    set p decimator
    vtkDecimate $p
    $p SetInput [mcubes GetOutput]
    $p SetInitialFeatureAngle 60
    $p SetMaximumIterations $decimateIterations
    $p SetMaximumSubIterations 0
    $p PreserveEdgesOn
    $p SetMaximumError 1
    $p SetTargetReduction 1
    $p SetInitialError .0002
    $p SetErrorIncrement .0002
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Decimating $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
    
    vtkReverseSense reverser

    # Do normals need reversing?
    set mm [rot GetMatrix] 
    if {[$mm Determinant] < 0} {
#      
# History: In a note to Samson Timoner, Dave Gering wrote:
# With some scan orders (AP PA LR RL IS SI), the normals need to be reversed
# for proper surface rendering. I meant to one day validate that this was
# happening correctly, but I never got around to making a model from every
# type of scan order. The popup was to aid my testing, and it certainly
# shouldn't still be in there!!
#
#    tk_messageBox -message Reverse
        set p reverser
        $p SetInput [decimator GetOutput]
        $p ReverseNormalsOn
        [$p GetOutput] ReleaseDataFlagOn
        set Gui(progressText) "Reversing $name"
        $p SetStartMethod     MainStartProgress
        $p SetProgressMethod "MainShowProgress $p"
        $p SetEndMethod       MainEndProgress
    }

    if { $ModelMaker(UseSinc) == 1} {
        vtkWindowedSincPolyDataFilter smoother
        smoother SetPassBand .1
    } else {
        vtkSmoothPolyDataFilter smoother
    # This next line massively rounds corners
        smoother SetRelaxationFactor .33
        smoother SetFeatureAngle 60
        smoother SetConvergence 0
    }
    smoother SetInput [$p GetOutput]
    set p smoother
    $p SetNumberOfIterations $smoothIterations
    # This next line massively rounds corners
    $p FeatureEdgeSmoothingOff
    $p BoundarySmoothingOff
    [$p GetOutput] ReleaseDataFlagOn
    set Gui(progressText) "Smoothing $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
    set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
    set ModelMaker($m,nPolys) $ModelMaker(n,$p)

    set p transformer
    vtkTransformPolyDataFilter $p
    $p SetInput [smoother GetOutput]
    $p SetTransform rot
    set Gui(progressText) "Transforming $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOn

    set p normals
    vtkPolyDataNormals $p
    $p ComputePointNormals$ModelMaker(PointNormals)
    $p SetInput [transformer GetOutput]
    $p SetFeatureAngle 60
    $p Splitting$ModelMaker(SplitNormals)
    set Gui(progressText) "Normals $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOn

    set p stripper
    vtkStripper $p
    $p SetInput [normals GetOutput]
    set Gui(progressText) "Stripping $name"
    $p SetStartMethod     MainStartProgress
    $p SetProgressMethod "MainShowProgress $p"
    $p SetEndMethod       MainEndProgress
    [$p GetOutput] ReleaseDataFlagOff

    # polyData will survive as long as it's the input to the mapper
    set Model($m,polyData) [$p GetOutput]
    $Model($m,polyData) Update
    foreach r $Module(Renderers) {
        Model($m,mapper,$r) SetInput $Model($m,polyData)
    }
    stripper SetOutput ""
    foreach p "to thresh mcubes decimator reverser transformer smoother normals stripper" {
        $p SetInput ""
        $p Delete
    }
    rot Delete

    # Restore spacing
    eval [Volume($v,vol) GetOutput] SetSpacing $spacing
    eval [Volume($v,vol) GetOutput] SetOrigin $origin

    set ModelMaker(marching) 0
    return 0
}

