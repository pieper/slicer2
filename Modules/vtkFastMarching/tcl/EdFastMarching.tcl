#=auto=========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
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
# FILE:        EdFastMarching.tcl
# PROCEDURES:  
#   EdFastMarchingApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingInit {} {
    global Ed Gui EdFastMarching Volume Slice

    set e EdFastMarching
    set Ed($e,name)      "Fast Marching"
    set Ed($e,initials)  "Fm"
    set Ed($e,desc)      "Fast Marching: PDE based segmentation"
    set Ed($e,rank)      1 ;#FIXME
    set Ed($e,procGUI)   EdFastMarchingBuildGUI
    set Ed($e,procEnter) EdFastMarchingEnter
    set Ed($e,procExit)  EdFastMarchingExit

    set Ed($e,initialized) 0

    # ID of the active mask
    set EdFastMarching(activeMask) "none"

    # total numer of masks
    set EdFastMarching(nMask) 0
    
    # no previous mask ID
    set EdFastMarching(maxIndiceMask) -1

    set Ed($e,waitingCenterClick) 0

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

}

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingBuildGUI {} {
    global Ed Gui Label Volume EdFastMarching

    set e EdFastMarching
    #-------------------------------------------
    # FastMarching frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame)

    frame $f.fWarning  -bg $Gui(activeWorkspace)
    frame $f.fGrid      -bg $Gui(activeWorkspace)
    frame $f.fMask -bg $Gui(activeWorkspace)
    frame $f.fExtra -bg $Gui(activeWorkspace)

    pack $f.fWarning $f.fGrid $f.fMask $f.fExtra \
    -side top -fill x 
    #-------------------------------------------
    # FastMarching->Grid frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fWarning

    eval {label $f.lWarning1 -text "WARNING: for development ONLY !" } $Gui(WLA)
    eval {label $f.lWarning2 -text "Eric Pichon <eric@ece.gatech.edu>" }  $Gui(WLA)

    pack $f.lWarning1 $f.lWarning2

    #-------------------------------------------
    # FastMarching->Grid frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fGrid

    # Output label
    eval {button $f.bOutput -text "Label:" \
          -command "ShowLabels EdFastMarchingLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdFastMarchingLabel"
    bind $f.eOutput <FocusOut> "EdFastMarchingLabel"
    eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w

    lappend Label(colorWidgetList) $f.eName

    #-------------------------------------------
    # FastMarching->Mask frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fMask

    frame $f.fCommands -bg $Gui(activeWorkspace)
    frame $f.fCoords -bg $Gui(activeWorkspace)
    frame $f.fSize -bg $Gui(activeWorkspace)

    #-------------------------------------------
    # Mask: Commands frame
    #-------------------------------------------

    set  f $Ed(EdFastMarching,frame).fMask.fCommands
    
    eval {menubutton $f.mbMask -text "Mask:" -relief raised -bd 2 \
          -width 6 -menu $f.mbMask.m} $Gui(WMBA)    
    
    menu $f.mbMask.m

    eval {label $f.lMask} $Gui(WLA)

    eval {button $f.bNewMask \
          -text "new" \
          -command "EdFastMarchingMaskAdd" } $Gui(WBA)

    eval {button $f.bRemoveMask \
          -text "remove"  \
          -command "EdFastMarchingMaskRemove $EdFastMarching(activeMask)" } $Gui(WBA)

    pack $f.mbMask $f.lMask $f.bNewMask $f.bRemoveMask \
    -side left -expand true -fill x

    #-------------------------------------------
    # Mask: Center/Normal/Size frame
    #-------------------------------------------

    set  f $Ed(EdFastMarching,frame).fMask.fCoords

    set i 0
    foreach dimmension "R A S Theta Phi r" {

    eval {button $f.bDec$dimmension -text "-" } $Gui(WBA)
    grid $f.bDec$dimmension -row $i -column 1

    eval {label $f.l$dimmension -text "$dimmension="} $Gui(WLA)
    grid $f.l$dimmension -row $i -column 2 -sticky e

    eval {entry $f.e$dimmension -width 3 -justify right} $Gui(WEA)
    grid $f.e$dimmension -row $i -column 3 

    bind $f.e$dimmension <Return>   "EdFastMarchingChangedCoord"
    bind $f.e$dimmension <FocusOut> "EdFastMarchingChangedCoord"

    eval {button $f.bInc$dimmension -text "+" } $Gui(WBA)    
    grid $f.bInc$dimmension -row $i -column 4

    incr i
    }

    eval {label $f.lCenter -text "Center:"} $Gui(WLA)
    grid $f.lCenter -row 0 -column 0 -padx 30 -sticky w

    eval {button $f.bCenterClick -text "(click)" -command "EdFastMarchingToggleWaitingCenterClick" } $Gui(WBA)    
    grid $f.bCenterClick -row 1 -column 0 -padx 30 -sticky w

    eval {label $f.lNormal -text "Normal:"} $Gui(WLA)
    grid $f.lNormal -row 3 -column 0 -padx 30 -sticky w

    eval {label $f.lSize -text "Size:"} $Gui(WLA)
    grid $f.lSize -row 5 -column 0 -padx 30 -sticky w


    set f $Ed(EdFastMarching,frame).fMask

    pack $f.fCommands $f.fCoords -side top -fill x -pady $Gui(pad)

    ############


    #-------------------------------------------
    # FastMarching->Extra frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fExtra

    eval {label $f.lDepth -text "Depth:"} $Gui(WLA)
    eval {entry $f.eDepth -width 5 -justify right \
          -textvariable EdFastMarching(Depth) } $Gui(WEA)
    grid $f.lDepth $f.eDepth -padx 2 -pady $Gui(pad)

    eval {label $f.lStdev -text "Std Dev:"} $Gui(WLA)
    eval {entry $f.eStdev -width 5 -justify right \
          -textvariable EdFastMarching(Stdev) } $Gui(WEA)
    grid $f.lStdev $f.eStdev -padx 2 -pady $Gui(pad)

    eval {label $f.lSigma -text "Blurring (sigma):"} $Gui(WLA)
    eval {entry $f.eSigma -width 5 -justify right \
          -textvariable EdFastMarching(Sigma) } $Gui(WEA)
    grid $f.lSigma $f.eSigma -padx 2 -pady $Gui(pad)

    ############
    
    set  f $Ed(EdFastMarching,frame)
    
    eval {button $f.bBack1Step -text "Back 1 step" -command "EdFastMarchingBack1Step" } $Gui(WBA)   
    
    $f.bBack1Step configure -state disabled

    pack $f.bBack1Step -side top -fill x -pady $Gui(pad)

    # create an empty mask
    EdFastMarchingMaskCreate "none" "" "" "" "" "" ""
    EdFastMarchingMaskUpdateWidget "none" "disabled"
}

proc EdFastMarchingBack1Step {} {
    global EdFastMarching Ed
    
    set EdFastMarching(nEvolutions) [expr $EdFastMarching(nEvolutions)-1]

    EdFastMarching(FastMarching) back1Step
    # Ed(editor) FastMarchingBack1Step

    if { $EdFastMarching(nEvolutions) <=0 } {
    $Ed(EdFastMarching,frame).bBack1Step \
        configure -state disabled
    }

    set v [EditorGetInputID $Ed(EdFastMarching,input)]
    EdUpdateAfterApplyEffect $v
}

proc EdFastMarchingAdd {varName incr} {
    global EdFastMarching

    upvar 1 $varName var
    set $varName [expr ($var+$incr)]
}

# create a new mask
# takes care of giving the mask an unique ID
proc EdFastMarchingMaskAdd {} {
    global EdFastMarching

    set e EdFastMarching

    incr EdFastMarching(nMask)
    incr EdFastMarching(maxIndiceMask)
    set EdFastMarching(activeMask) $EdFastMarching(maxIndiceMask)
    
    EdFastMarchingMaskCreate $EdFastMarching(activeMask) [expr 100]  [expr 100] [expr 100] \
    0 0 10
}

# variables are not unset
# we just remove the mask from the menu, remove the vtk actor 
# and set the variable removed to 1
proc EdFastMarchingMaskRemove {ID} {
    global Ed EdFastMarching
    set e EdFastMarching

    set mask mask.$ID
    
    set EdFastMarching($mask.removed) 1

    #note: we use " $ID " with spaces so that Tk does not
    #think this is a numerical index
    $Ed($e,frame).fMask.fCommands.mbMask.m delete " $ID "

    set EdFastMarching(activeMask) "none"
    EdFastMarchingMaskUpdateWidget "none" "disabled"

    MainRemoveActor EdFastMarching(actor$ID)
    MainRemoveActor EdFastMarching(textActor$ID)

    RenderAll;
}

# create the mask of given ID and parameters
proc EdFastMarchingMaskCreate {ID cx cy cz theta phi r} {
    global Ed EdFastMarching View
    set e EdFastMarching

    set mask mask.$ID

    set EdFastMarching($mask.removed) 0
    set EdFastMarching($mask.R) $cx
    set EdFastMarching($mask.A) $cy
    set EdFastMarching($mask.S) $cz
    set EdFastMarching($mask.Theta) $theta
    set EdFastMarching($mask.Phi) $phi
    set EdFastMarching($mask.r) $r
    
    if { $ID != "none" } { 
    set state "normal"

    vtkCylinderSource EdFastMarching(cyl$ID)

    EdFastMarching(cyl$ID) SetCenter 0 0 0
    EdFastMarching(cyl$ID) SetRadius $EdFastMarching($mask.r)
    EdFastMarching(cyl$ID) SetHeight 1

    EdFastMarching(cyl$ID) SetResolution 20

    vtkTransform EdFastMarching(trans$ID)
    EdFastMarching(trans$ID) Identity;

    vtkTransformFilter EdFastMarching(transFilter$ID);
    EdFastMarching(transFilter$ID) SetInput [EdFastMarching(cyl$ID) GetOutput];
    EdFastMarching(transFilter$ID) SetTransform EdFastMarching(trans$ID);

    vtkPolyDataMapper EdFastMarching(mapper$ID)
    EdFastMarching(mapper$ID) SetInput [EdFastMarching(transFilter$ID) GetOutput] 

    vtkActor EdFastMarching(actor$ID)
    EdFastMarching(actor$ID) SetMapper EdFastMarching(mapper$ID)

    MainAddActor EdFastMarching(actor$ID)

       vtkVectorText EdFastMarching(vectorText$ID)
       EdFastMarching(vectorText$ID) SetText "$ID"

    vtkTransformFilter EdFastMarching(textTransFilter$ID);
    EdFastMarching(textTransFilter$ID) SetInput [EdFastMarching(vectorText$ID) GetOutput];

    vtkTransform EdFastMarching(textTrans$ID)
    EdFastMarching(textTrans$ID) Identity;

    EdFastMarching(textTransFilter$ID) SetTransform EdFastMarching(textTrans$ID);
    

    vtkPolyDataMapper  EdFastMarching(PolyDataMapper$ID)
    EdFastMarching(PolyDataMapper$ID) SetInput [EdFastMarching(textTransFilter$ID) GetOutput]

    vtkFollower EdFastMarching(textActor$ID)
    EdFastMarching(textActor$ID) SetMapper EdFastMarching(PolyDataMapper$ID)
    eval [EdFastMarching(textActor$ID) GetProperty] SetColor 0 0 1
    EdFastMarching(textActor$ID) SetScale  10 10 10

    EdFastMarching(textActor$ID) SetCamera $View(viewCam)
    EdFastMarching(textActor$ID) SetPickable 0

    MainAddActor EdFastMarching(textActor$ID)

    } else {
    set state "disabled"
    set EdFastMarching($mask.removed) 1
    }

    #note: we use " $ID " with spaces so that Tk does not
    #think this is a numerical index
    $Ed($e,frame).fMask.fCommands.mbMask.m add command -label " $ID " \
    -command "set EdFastMarching(activeMask) $ID; EdFastMarchingMaskUpdateWidget $ID $state"

    EdFastMarchingMaskUpdateWidget $ID $state
}

proc EdFastMarchingOutputMask {} {

    global EdFastMarching Ed

    for {set i 0} {$i <= $EdFastMarching(maxIndiceMask)} {incr i} {

    set mask mask.$i
    if { $EdFastMarching($mask.removed)==0 } {

        set r $EdFastMarching($mask.R);
        set a $EdFastMarching($mask.A);
        set s $EdFastMarching($mask.S);

#         set I [expr $m11*$r+$m12*$a+$m13*$s+$m14*1 ];
#         set J [expr $m21*$r+$m22*$a+$m23*$s+$m24*1 ];
#         set K [expr $m31*$r+$m32*$a+$m33*$s+$m34*1 ];

        set THETA $EdFastMarching($mask.Theta)
        set PHI $EdFastMarching($mask.Phi)
        set R $EdFastMarching($mask.r)

        #Ed(editor) FastMarchingAddMask $r $a $s $R $THETA $PHI
        EdFastMarching(FastMarching) AddMask $r $a $s $R $THETA $PHI
        puts "mask $i"
        puts "$r $a $s $THETA $PHI $R"
    }
    }
}

# called whenever a coordinate is changed
proc EdFastMarchingChangedCoord {} {
    global EdFastMarching

    set mask mask.$EdFastMarching(activeMask)

    if { $EdFastMarching(activeMask) != "none" } { 

    foreach dimmension "R A S Theta Phi r" {
        if {[ValidateInt $EdFastMarching($mask.$dimmension)] == 0} {
        set EdFastMarching($mask.$dimmension) 0
        tk_messageBox -message "$dimmension should be an integer."
        return
        }
    }
        
    foreach dimmension "Theta Phi" {
        set EdFastMarching($mask.$dimmension) [expr $EdFastMarching($mask.$dimmension)%360]
    }

    if { $EdFastMarching($mask.r)<1 } {
        set EdFastMarching($mask.r) 1
    }

    EdFastMarchingUpdateActor $EdFastMarching(activeMask)
    }
}

# state can be either "normal" or "disabled"
proc EdFastMarchingMaskUpdateWidget {ID state} {
    global Ed EdFastMarching

    set  e EdFastMarching
    set  f $Ed($e,frame).fMask

    set mask mask.$ID

    $f.fCommands.lMask configure -text $EdFastMarching(activeMask)

    $f.fCommands.bRemoveMask configure -state $state \
    -command "EdFastMarchingMaskRemove $EdFastMarching(activeMask)"

    $f.fCoords.bCenterClick configure -state $state

    foreach dimmension "R A S Theta Phi r" incr "1 1 1 10 10 5" {
    $f.fCoords.bDec$dimmension configure -state $state \
        -command "EdFastMarchingAdd EdFastMarching($mask.$dimmension) -$incr; EdFastMarchingChangedCoord"

    $f.fCoords.bInc$dimmension configure -state $state \
        -command "EdFastMarchingAdd EdFastMarching($mask.$dimmension) +$incr; EdFastMarchingChangedCoord"

    $f.fCoords.e$dimmension configure -state $state \
        -textvariable EdFastMarching($mask.$dimmension)
    }

    EdFastMarchingChangedCoord
}

proc EdFastMarchingUpdateActor {ID} {
    global EdFastMarching

    set mask mask.$ID

    EdFastMarching(trans$ID) Identity;

    EdFastMarching(trans$ID) Translate \
    $EdFastMarching($mask.R) $EdFastMarching($mask.A) $EdFastMarching($mask.S)

    EdFastMarching(trans$ID) RotateZ \
    $EdFastMarching($mask.Theta)

    EdFastMarching(trans$ID) RotateX \
    $EdFastMarching($mask.Phi)

    EdFastMarching(cyl$ID) SetRadius $EdFastMarching($mask.r)
    
    EdFastMarching(trans$ID) Update;
    EdFastMarching(cyl$ID) Update;
 
    EdFastMarching(textTrans$ID) Identity
    EdFastMarching(textTrans$ID) Translate \
    0 0 [expr ($EdFastMarching($mask.r)+5)/10]

    EdFastMarching(textActor$ID) SetPosition \
    $EdFastMarching($mask.R) $EdFastMarching($mask.A) $EdFastMarching($mask.S)

    EdFastMarching(textTrans$ID) Update

    RenderAll;    
}

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingEnter {} {
    global Ed Label Slice EdFastMarching
    set e EdFastMarching

    #EdFastMarchingParams

    EdFastMarchingLabel         

    # Make sure we're colored
    LabelsColorWidgets

    puts "taille $Slice(idList)"
    foreach s $Slice(idList) {
#        puts "Volume(width)=$Volume(width) and Volume(height)=$Volume(height) Slice(num)=$Slice(num)"
        #puts [Slicer GetOffsetRangeLow  $s]
        #puts [Slicer GetOffsetRangeHigh $s]
        set dim$s [expr [Slicer GetOffsetRangeHigh $s]-[Slicer GetOffsetRangeLow  $s]]
    }

    if {$Ed(EdFastMarching,initialized)==0} {

    set Ed(EdFastMarching,initialized) 1
    puts "FM Init($dim0,$dim1,$dim2)"

    #Ed(editor) FastMarchingInit [expr $dim0+1] [expr $dim2+1] [expr $dim1+1]
    vtkFastMarching EdFastMarching(FastMarching) 
    puts "vtkFastMarching EdFastMarching(FastMarching)"
    EdFastMarching(FastMarching) init  [expr $dim0+1] [expr $dim2+1] [expr $dim1+1]

    }

    # ID of the active mask
    set EdFastMarching(activeMask) "none"

    # total numer of masks
    set EdFastMarching(nMask) 0
    
    # no previous mask ID
    set EdFastMarching(maxIndiceMask) -1

    set Ed($e,waitingCenterClick) 0

    set EdFastMarching(nEvolutions) 0
    $Ed(EdFastMarching,frame).bBack1Step \
    configure -state disabled


###############

    set e EdFastMarching

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

    set v [EditorGetInputID $Ed($e,input)]


    set EdFastMarching(Depth) [Volume($v,vol) GetRangeHigh]
    set EdFastMarching(Stdev) [expr int($EdFastMarching(Depth)/60.0)]
    set EdFastMarching(Sigma) 0.1

#    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
    EdUpdateAfterApplyEffect $v

EditorActivateUndo 0

EditorClear Working
}

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingExit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingExit {} {
    global Ed EdFastMarching

    if {$Ed(EdFastMarching,initialized)==1} {

    set Ed(EdFastMarching,initialized) 0
    puts "FM Destructor"
#    Ed(editor) FastMarchingDestructor
    EdFastMarching(FastMarching) unInit
    }    

    #remove all masks
    for {set i 0} {$i <= $EdFastMarching(maxIndiceMask)} {incr i} {

    set mask mask.$i
    if { $EdFastMarching($mask.removed)==0 } {
        EdFastMarchingMaskRemove $i
    }
    }

    #delete the object
    EdFastMarching(FastMarching) Delete

    Slicer BackFilterOff
    Slicer ForeFilterOff
    Slicer ReformatModified
    Slicer Update

}


#-------------------------------------------------------------------------------
# .PROC EdFastMarchingLabel
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingLabel {} {
    global Ed

    LabelsFindLabel
    #EdFastMarchingUpdate
}

proc EdFastMarchingToggleWaitingCenterClick {} {
    global Ed

    set e EdFastMarching

    if {$Ed($e,waitingCenterClick)==1} {
    set Ed($e,waitingCenterClick) 0
    $Ed(EdFastMarching,frame).fMask.fCoords.bCenterClick \
        configure -relief raised
    } else {
    set Ed($e,waitingCenterClick) 1
    $Ed(EdFastMarching,frame).fMask.fCoords.bCenterClick \
        configure -relief sunken
    }
}
#-------------------------------------------------------------------------------
# .PROC EdFastMarchingApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EdFastMarchingApply {} {
    global Ed Volume Label Gui EdFastMarching


    puts " EdFastMarchingApply "

    set e EdFastMarching

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

    set v [EditorGetInputID $Ed($e,input)]

    # Validate input
    if {[ValidateInt $Label(label)] == 0} {
    tk_messageBox -message "Output label is not an integer."
    return
    }

    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

    # Only apply to native slices
    if {[set native [EdIsNativeSlice]] != ""} {
    tk_messageBox -message "Please click on the slice with orient = $native."
    return
    }

    puts "FM depth= $EdFastMarching(Depth) stdev=$EdFastMarching(Stdev)"


#    Ed(editor) FastMarchingSetExtra $EdFastMarching(Depth) $EdFastMarching(Stdev) $EdFastMarching(Sigma)

    EdFastMarching(FastMarching) setDepth $EdFastMarching(Depth)
    puts setDepth
    EdFastMarching(FastMarching) setStdev $EdFastMarching(Stdev)
    puts setStdev
    EdFastMarching(FastMarching) setSigma $EdFastMarching(Sigma)
    puts setSigma


    scan [Volume($v,node) GetRasToVtkMatrix] \
    "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" \
    m11 m12 m13 m14 \
    m21 m22 m23 m24 \
    m31 m32 m33 m34 \
    m41 m42 m43 m44;

    if {$Ed($e,waitingCenterClick)==1} {
    puts "Ed($e,waitingCenterClick)==1"

    EdFastMarchingToggleWaitingCenterClick

    set x       $Ed($e,xSeed)
    set y       $Ed($e,ySeed)
    set z       $Ed($e,zSeed)
    
    #note: we assume that the matrice has zeros
    #in certain positions...
    set R [expr round( ($z-$m34)/$m31 ) ]
    set A [expr round( ($x-$m14)/$m12 ) ]
    set S [expr round( ($y-$m24)/$m23 ) ]

    set mask mask.$EdFastMarching(activeMask)

    set EdFastMarching($mask.R) $R
    set EdFastMarching($mask.A) $A
    set EdFastMarching($mask.S) $S

    puts "EdFastMarching(mask.R) $R"

    EdFastMarchingChangedCoord
    #EdFastMarchingMaskUpdateWidget $EdFastMarching(activeMask) "normal"
    
    return
    }


    puts "FM SetRAStoIJKmatrix $m11 $m12 $m13 $m14 $m21 $m22 $m23 $m24 $m31 $m32 $m33 $m34 $m41 $m42 $m43 $m44"

#    Ed(editor) FastMarchingSetRAStoIJKmatrix $m11 $m12 $m13 $m14 $m21 $m22 $m23 $m24 $m31 $m32 $m33 $m34 $m41 $m42 $m43 $m44

    EdFastMarching(FastMarching) setRAStoIJKmatrix $m11 $m12 $m13 $m14 $m21 $m22 $m23 $m24 $m31 $m32 $m33 $m34 $m41 $m42 $m43 $m44

    EdFastMarchingOutputMask

    set Gui(progressText) "FastMarching in [Volume($v,node) GetName]"

    set x       $Ed($e,xSeed)
    set y       $Ed($e,ySeed)
    set z       $Ed($e,zSeed)
    set label   $Label(label)
    puts "FM $x $y $z $label"

    Ed(editor)  UseInputOn

    #Ed(editor) FastMarchingGo $x $y $z $label
    EdFastMarching(FastMarching) setSeedAndLabel $x $y $z $label

    set o [EditorGetOriginalID]
    set w [EditorGetWorkingID]

    
    EdFastMarching(FastMarching) SetInput [Volume($o,vol) GetOutput]
    EdFastMarching(FastMarching) SetOutput [Volume($w,vol) GetOutput]

#    EdFastMarching(FastMarching) ExecuteData NULL

#    EdFastMarching(FastMarching) startEvolution
    
    # make sure the program is run at all times
    EdFastMarching(FastMarching) Modified

    Ed(editor) Apply EdFastMarching(FastMarching) EdFastMarching(FastMarching)

    incr EdFastMarching(nEvolutions)
    $Ed(EdFastMarching,frame).bBack1Step \
    configure -state normal
    
    #EdFastMarching(FastMarching)  SetInput ""
    #EdFastMarching(FastMarching)  UseInputOff

    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v
}


