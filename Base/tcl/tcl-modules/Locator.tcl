#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: Locator.tcl,v $
#   Date:      $Date: 2008/01/28 20:45:03 $
#   Version:   $Revision: 1.38.12.2.2.34 $
# 
#===============================================================================
# FILE:        Locator.tcl
# PROCEDURES:  
#   LocatorInit
#   LocatorUpdateMRML
#   LocatorBuildVTK
#   LocatorBuildGUI
#   LocatorSetActive
#   LocatorSetPatientPosition
#   LocatorSetDriverAll
#   LocatorSetDriver
#   LocatorSetVisibility
#   LocatorSetTransverseVisibility
#   LocatorSetGuideVisibility
#   LocatorSetColor
#   LocatorSetSize
#   LocatorCreateFiducial 
#   LocatorDeleteFiducial 
#   LocatorNameUpated
#   LocatorSetMatrices
#   LocatorSetPosition
#   LocatorUseLocatorMatrix
#   LocatorFormat
#   LocatorGetRealtimeID
#   LocatorSetRealtime
#   LocatorWrite
#   LocatorRead
#   LocatorRegisterCallback
#   LocatorUnRegisterCallback
#   LocatorPause
#   LocatorConnect
#   LocatorLoopFile
#   LocatorLoopImages
#   LocatorLoopFlashpoint
#   LocatorFilePrefix
#   LocatorImagesPrefix
#   LocatorStorePresets
#   LocatorCsysToggle
#   LocatorCsysOn
#   LocatorCsysOff
#   LocatorCsysCallback
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC LocatorInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorInit {} {
    global Locator Module

    # Define Tabs
    set m Locator
    set Module($m,row1List) "Help Tracking Server Handpiece"
    set Module($m,row1Name) "Help Tracking Server Handpiece"
    set Module($m,row1,tab) Tracking

    # Module Summary Info
    set Module($m,overview) "Surgical guidance in the MRT (tracked locator)."
    set Module($m,author) "Core"
    set Module($m,category) "Application"

    # Define Procedures
    set Module($m,procGUI)   LocatorBuildGUI
    set Module($m,procVTK)   LocatorBuildVTK
    set Module($m,procEnter) LocatorEnter
    set Module($m,procExit)  LocatorExit


    lappend Module(procStorePresets) LocatorStorePresets
    lappend Module(procRecallPresets) LocatorRecallPresets

    # Set Default Presets
    set Module(Locator,presets) "0,driver='User' 1,driver='User' 2,driver='User'\
    visibility='0' transverseVisibility='1' guideVisibility='0' normalLen='100' transverseLen='25'\
     radius='1.5' diffuseColor='0 0.9 0'"

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.38.12.2.2.34 $} {$Date: 2008/01/28 20:45:03 $}]

    # Patient/Table position
    set Locator(tblPosList)   "Front Side"
    set Locator(patEntryList) "Head-first Feet-first"
    set Locator(patPosList)   "Supine Prone Left-decub Right-decub"
    set Locator(tblPos)       [lindex $Locator(tblPosList) 0]
    set Locator(patEntry)     [lindex $Locator(patEntryList) 0]
    set Locator(patPos)       [lindex $Locator(patPosList) 0]

    # Locator position
    set Locator(nx) 0
    set Locator(ny) -1
    set Locator(nz) 0
    set Locator(tx) 1
    set Locator(ty) 0
    set Locator(tz) 0
    set Locator(px) 0 
    set Locator(py) 0
    set Locator(pz) 0
    LocatorFormat

    # Locator attributes
    set Locator(visibility) 0
    set Locator(transverseVisibility) 1
    set Locator(guideVisibility) 1
    set Locator(guideSteps) 5 ;# only settable at startup time
    set Locator(normalLen) 100
    set Locator(transverseLen) 25
    set Locator(radius) 1.5 
    set Locator(normalOffset) 0
    set Locator(transverseOffset) 0
    set Locator(crossOffset) 0
    set Locator(0,driver) User
    set Locator(1,driver) User
    set Locator(2,driver) User
    set Locator(diffuseColor) "0 0.9 0"
    scan $Locator(diffuseColor) "%g %g %g" Locator(red) Locator(green) Locator(blue)

    # Fiducial Attributes
    set Locator(fiducialName) ""

    # Realtime image
    set Locator(idRealtime)     NEW
    set Locator(prefixRealtime) ""
    
    # Servers
    set Locator(serverList) "Flashpoint File Images OpenTracker Csys"
    set Locator(server) [lindex $Locator(serverList) 0]
    set Locator(connect) 0
    set Locator(pause) 0
    set Locator(loop) 0
    set Locator(imageNum) ""
    set Locator(recon) 0
    set Locator(callbackList) ""

    # Server specific stuff:
    # File
    set Locator(File,msPoll) 100
    set Locator(File,prefix) loc
    set Locator(File,fid)    ""
    # Flashpoint
    set Locator(Flashpoint,msPoll) 100
    set Locator(Flashpoint,port)   30000
    set Locator(Flashpoint,host)   mrtws 
    # Images
    set Locator(Images,msPoll)   1000
    set Locator(Images,prefix)   ""
    set Locator(Images,firstNum) 1
    set Locator(Images,lastNum)  1
    set Locator(Images,increment) 1
    # Csys
    set Locator(csysVisible) 0
    # OpenTracker
    set Locator(OpenTracker,msPoll) 100
    set Locator(OpenTracker,multiBy) 1.0
    set dir [pwd]
    set Locator(OpenTracker,cfg) [file join $dir defaultConfig.xml] 

    set Locator(bellCount) 0
    set Locator(realtimeScanOrder) "AP" 
    set Locator(realtimeRSA) "0 0 0" 
    set Locator(pointPairList) "" 
    LocatorCreateBindings
}


#-------------------------------------------------------------------------------
# .PROC LocatorUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorUpdateMRML {} {
    global Volume Locator

    # See if the volume for each menu actually exists.
    # If not, use the None volume
    #
    set n $Volume(idNone)
    if {$Locator(idRealtime) != "NEW" && \
        [lsearch $Volume(idList) $Locator(idRealtime)] == -1} {
        LocatorSetRealtime NEW
    }

    # Realtime Volume menu
    #---------------------------------------------------------------------------
    set m $Locator(mRealtime)
    $m delete 0 end
    set idRealtime ""
    foreach v $Volume(idList) {
        if {$v != $Volume(idNone) && $v != $Locator(idResult)} {
            $m add command -label [Volume($v,node) GetName] -command \
                "LocatorSetRealtime $v; RenderAll"
        }
        if {[Volume($v,node) GetName] == "Realtime"} {
            set idRealtime $v
        }
    }
    # If there is Realtime, then select it, else add a NEW option
    if {$idRealtime != ""} {
        LocatorSetRealtime $idRealtime
    } else {
        $m add command -label NEW -command "LocatorSetRealtime NEW; RenderAll"
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildVTK {} {
    global Gui Locator View Slice Target Volume Lut 

    #------------------------#
    # Realtime image source
    #------------------------#
    # Flashpoint
    set os -1
    switch $::tcl_platform(os) {
        "SunOS"  {set os 1}
        "Linux"  {set os 2}
        "Darwin" {set os 3}
        default  {set os 4}
    }
    vtkImageRealtimeScan Locator(Flashpoint,src)
    Locator(Flashpoint,src) SetOperatingSystem $os

    # OpenTracker
    vtkOpenTracker Locator(OpenTracker,src);

#    Locator(Flashpoint,src) SetTest 1

    # Images
    vtkMrmlVolumeNode Locator(Images,node)
    vtkMrmlDataVolume Locator(Images,vol)
    set n Locator(Images,node)
    set v Locator(Images,vol)
    $n SetDescription "Realtime Images source"
    $n SetName        "Source"
    $n SetLUTName     [lindex $Lut(idList) 0]
    $v SetMrmlNode          $n
    $v SetLabelIndirectLUT  Lut($Lut(idLabel),indirectLUT)
    $v SetLookupTable       Lut([$n GetLUTName],lut)
    $v SetHistogramHeight   $Volume(histHeight)
    $v SetHistogramWidth    $Volume(histWidth)
    $v AddObserver StartEvent       MainStartProgress
    $v AddObserver ProgressEvent   "MainShowProgress $v"
    $v AddObserver EndEvent         MainEndProgress

    #------------------------#
    # Construct handpiece
    #------------------------#
    vtkMatrix4x4 Locator(normalMatrix)
    vtkMatrix4x4 Locator(transverseMatrix)
    vtkMatrix4x4 Locator(tipMatrix)

    set Locator(actors) "normal transverse tip"

    set actor normal 
    MakeVTKObject Cylinder $actor
        ${actor}Source SetRadius $Locator(radius) 
        ${actor}Source SetHeight $Locator(normalLen)
        eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
        ${actor}Actor SetUserMatrix Locator(normalMatrix)

    # On the locator, we show the location of 
    # the opening of the biopsy needle.
    # The opening starts 5 mm away from the tip 
    # and it's 10 mm long.
    set actor opening 
    MakeVTKObject Cylinder $actor
        ${actor}Source SetRadius [expr 1.1 * $Locator(radius)] 
        ${actor}Source SetHeight 10.
        # expr {$Locator(normalLen) / -2.0 + 3.64} is 
        # the tip location. Adding 5 mm moves the opening
        # to the right place.
        ${actor}Actor SetPosition 0 \
                                  [expr $Locator(normalLen) / -2. + 3.64 + 5.0] \
                                  0
        # 1.0 1.0 0.0 = yellow
        eval [${actor}Actor GetProperty] SetColor "1.0 1.0 0.0"
        ${actor}Actor SetUserMatrix Locator(normalMatrix)
        lappend Locator(actors) $actor

    for {set i 0} {$i < $::Locator(guideSteps)} {incr i} {
        set actor guide$i
        MakeVTKObject Cylinder ${actor}
            ${actor}Source SetRadius 1.1 
            ${actor}Source SetHeight 10.
            ${actor}Actor SetPosition 0 [expr $Locator(normalLen) / -2. - 20 * (1+$i)] 0
            eval [${actor}Actor GetProperty] SetColor 1 0 0
            ${actor}Actor SetUserMatrix Locator(normalMatrix)
            lappend Locator(actors) $actor
    }

    set actor transverse
    MakeVTKObject Cylinder ${actor}
        ${actor}Source SetRadius $Locator(radius) 
        ${actor}Source SetHeight [expr $Locator(transverseLen)]
        eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
        ${actor}Actor SetUserMatrix Locator(transverseMatrix)

    
    set actor tip
    MakeVTKObject Sphere ${actor}
        ${actor}Source SetRadius [expr 1.0 * $Locator(radius)] 
        eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
        ${actor}Actor SetUserMatrix Locator(tipMatrix)
    
    LocatorSetMatrices
    LocatorSetVisibility
}


#-------------------------------------------------------------------------------
# .PROC LocatorBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildGUI {} {
    global Gui Module Locator Slice

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Tracking
    # Server
    # Handpiece
    #   
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
Description by Tab:<BR>
<UL>
<LI><B>Tracking:</B> Connect to a server that feeds the 3D Slicer a realtime
stream of coordinates of a tracked device called the <B>Locator</B>.
Press in the <B>Connect</B> button to begin tracking. If there is an error 
connecting to the server, the button will pop back out. 
<BR>
The reformatted slices can be driven (positioned) either by the the user
moving the sliders in the Viewer window, or by the Locator. For the latter, 
set the <B>Driver</B> to <B>Locator</B>.
<BR><LI><B>Server:</B> There are several ways to acquire realtime information
about a Locator or images:
<BR><B>File</B> will read Locator positions from a text file.
<BR><B>Flashpoint</B> will read Locator positions and realtime images from
a server process running on the GE Flashpoint workstation.
<BR><B>Images</B> will read images on disk as if to emulate realtime images
coming from a Flashpoint scanner.
<BR><LI><B>Handpiece</B> Specify how the Locator is rendered in the 3D view
window.  The <B>Normal</B> is the direction along the tip of the Locator's
needle.  The <B>Transverse</B> is angled 90 degrees away from the Normal
and along the handle.  The <B>N x T</B> is the cross-product of the other 2
and is angled 90 degrees from both of them as governed by the right-hand rule.
<BR><B>TIP:</B> Use the <B>Offset from Tip</B> to simulate having a longer
Locator.  Perhaps the Locator is afixed to a surgical device and you wish to
know the location of the tip of this device rather than the Locator.
</UL>"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Locator $help
    MainHelpBuildGUI Locator

    #-------------------------------------------
    # Tracking frame
    #-------------------------------------------
    set fTracking $Module(Locator,fTracking)
    set f $fTracking

    # Frames
    frame $f.fConn     -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fVis      -bg $Gui(activeWorkspace) 
    frame $f.fFid      -bg $Gui(activeWorkspace) -relief groove -bd 3 
    frame $f.fDriver   -bg $Gui(activeWorkspace) -relief groove -bd 3 
    frame $f.fRealtime -bg $Gui(activeWorkspace) -relief groove -bd 3

    pack $f.fConn $f.fVis $f.fFid $f.fDriver $f.fRealtime \
        -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    #-------------------------------------------
    # Tracking->Conn frame
    #-------------------------------------------
    set f $fTracking.fConn

    eval {label $f.l -text "Connection to Server"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $fTracking.fConn.f
    eval {checkbutton $f.cConnect \
        -text "Connect" -variable Locator(connect) -width 8 \
        -indicatoron 0 -command "LocatorConnect"} $Gui(WCA)
    eval {checkbutton $f.cPause \
        -text "Pause" -variable Locator(pause) -command "LocatorPause" -width 6 \
        -indicatoron 0} $Gui(WCA)
    pack $f.cConnect $f.cPause -side left -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Tracking->Driver Frame
    #-------------------------------------------
    set f $fTracking.fDriver

    eval {label $f.lTitle -text "The Locator can Drive Slices"} $Gui(WTA)
    grid $f.lTitle -padx $Gui(pad) -pady 3 -columnspan 2
    
    foreach s $Slice(idList) {

        eval {menubutton $f.mbDriver${s} -text "User" \
            -menu $f.mbDriver${s}.m -width 12 -relief raised \
            -bd 2} $Gui(WMBA) {-bg $Gui(slice$s)}
        set Locator(mDriver$s) $f.mbDriver${s}.m
        set Locator(mbDriver$s) $f.mbDriver${s}
        eval {menu $f.mbDriver${s}.m} $Gui(WMA)
        foreach item "User Locator" {
            $Locator(mDriver$s) add command -label $item \
                -command "LocatorSetDriver ${s} $item; RenderBoth $s"
        }
        if {$s == 0} {
            eval {menubutton $f.mbDriver -text "Driver: " \
                -menu $f.mbDriver.m -width 10 -relief raised \
                -bd 2} $Gui(WMBA) {-anchor e}

            set Locator(mDriver) $f.mbDriver.m
            eval {menu $Locator(mDriver)} $Gui(WMA)
            foreach item "User Locator" {
                $f.mbDriver.m add command -label $item \
                    -command "LocatorSetDriverAll $item; RenderAll"
            }
            grid $f.mbDriver $f.mbDriver${s} \
                -pady $Gui(pad) -padx $Gui(pad)
        } else {
            grid $f.mbDriver${s} -column 1 \
                -pady $Gui(pad) -padx $Gui(pad)
        }

    }

    #-------------------------------------------
    # Tracking->Vis Frame
    #-------------------------------------------
    set f $fTracking.fVis

    eval {checkbutton $f.cLocator \
        -text "Show Locator" -variable Locator(visibility) -width 16 \
        -indicatoron 0 -command "LocatorSetVisibility; Render3D"} $Gui(WCA)
    eval {checkbutton $f.cTransverse \
        -text "Handle" -variable Locator(transverseVisibility) -width 7 \
        -indicatoron 0 -command "LocatorSetTransverseVisibility; Render3D"} $Gui(WCA)
    eval {checkbutton $f.cGuide \
        -text "Guide" -variable Locator(guideVisibility) -width 5 \
        -indicatoron 0 -command "LocatorSetGuideVisibility; Render3D"} $Gui(WCA)
    pack $f.cLocator $f.cTransverse $f.cGuide -side left -padx $Gui(pad)

    #-------------------------------------------
    # Tracking->Fid Frame
    #-------------------------------------------
    set f $fTracking.fFid

    frame $f.fAddDel -bg $Gui(activeWorkspace)
    frame $f.fName -bg $Gui(activeWorkspace)
    pack $f.fAddDel -side top -pady $Gui(pad)
    pack $f.fName -side top -pady $Gui(pad)

    #-------------------------------------------
    # Tracking->Fid->AddDel Frame
    #-------------------------------------------
    eval {button $f.fAddDel.bFiducial \
        -text "Add Fiducial" -width 12 \
        -command "LocatorCreateFiducial; Render3D"} $Gui(WBA)
    eval {button $f.fAddDel.bFiducialDel \
        -text "Delete Fiducial" -width 15 \
        -command "LocatorDeleteFiducial; Render3D"} $Gui(WBA)
    pack $f.fAddDel.bFiducial $f.fAddDel.bFiducialDel -side left -padx $Gui(pad)

    #-------------------------------------------
    # Tracking->Fid->Name Frame
    #-------------------------------------------
    eval {label $f.fName.lName -text "Name:"} $Gui(WTA)
    eval {entry $f.fName.eFiducialName -width 25 -textvariable Locator(fiducialName) } $Gui(WEA)
    bind $f.fName.eFiducialName <Return> {LocatorNameUpated}
    pack $f.fName.lName $f.fName.eFiducialName -side left -padx $Gui(pad)


    #-------------------------------------------
    # Tracking->Realtime
    #-------------------------------------------
    set f $fTracking.fRealtime

    frame $f.fMenu -bg $Gui(activeWorkspace)
    frame $f.fPrefix -bg $Gui(activeWorkspace)
    frame $f.fBtns   -bg $Gui(activeWorkspace)
    pack $f.fMenu -side top -pady $Gui(pad)
    pack $f.fPrefix -side top -pady $Gui(pad) -fill x
    pack $f.fBtns -side top -pady $Gui(pad)

    #-------------------------------------------
    # Tracking->Realtime->Menu
    #-------------------------------------------
    set f $fTracking.fRealtime.fMenu

    # Volume menu
    eval {label $f.lRealtime -text "Realtime Volume:"} $Gui(WTA)

    eval {menubutton $f.mbRealtime -text "NEW" -relief raised -bd 2 -width 18 \
        -menu $f.mbRealtime.m} $Gui(WMBA)
    eval {menu $f.mbRealtime.m} $Gui(WMA)
    pack $f.lRealtime $f.mbRealtime -padx $Gui(pad) -side left

    # Save widgets for changing
    set Locator(mbRealtime) $f.mbRealtime
    set Locator(mRealtime)  $f.mbRealtime.m

    #-------------------------------------------
    # Tracking->Realtime->Prefix
    #-------------------------------------------
    set f $fTracking.fRealtime.fPrefix

    eval {label $f.l -text "Prefix:"} $Gui(WLA)
    eval {entry $f.e -textvariable Locator(prefixRealtime)} $Gui(WEA)
    pack $f.l -padx 3 -side left
    pack $f.e -padx 3 -side left -expand 1 -fill x

    #-------------------------------------------
    # Tracking->Realtime->Btns
    #-------------------------------------------
    set f $fTracking.fRealtime.fBtns

    eval {button $f.bWrite -text "Save" -width 5 \
        -command "LocatorWrite Realtime; RenderAll"} $Gui(WBA)
    eval {button $f.bRead -text "Read" -width 5 \
        -command "LocatorRead Realtime; RenderAll"} $Gui(WBA)
    pack $f.bWrite $f.bRead -side left -padx $Gui(pad)



    #-------------------------------------------
    # Server frame
    #-------------------------------------------
    set fServer $Module(Locator,fServer)
    set f $fServer

    frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fBot -bg $Gui(activeWorkspace) -height 330
    pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

    #-------------------------------------------
    # Server->Bot frame
    #-------------------------------------------
    set f $fServer.fBot

    foreach s $Locator(serverList) {
        frame $f.f$s -bg $Gui(activeWorkspace)
        place $f.f$s -in $f -relheight 1.0 -relwidth 1.0
        set Locator(f$s) $f.f$s
    }
    raise $Locator(f[lindex $Locator(serverList) 0])

    #-------------------------------------------
    # Server->Top frame
    #-------------------------------------------
    set f $fServer.fTop

    frame $f.fActive -bg $Gui(backdrop)
    pack $f.fActive -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Server->Top->Active frame
    #-------------------------------------------
    set f $fServer.fTop.fActive

    eval {label $f.lActive -text "Active Server: "} $Gui(BLA)
    eval {menubutton $f.mbActive \
        -text [lindex $Locator(serverList) 0] \
        -relief raised -bd 2 -width 20 \
        -menu $f.mbActive.m} $Gui(WMBA)
    eval {menu $f.mbActive.m} $Gui(WMA)
    pack $f.lActive $f.mbActive -side left
    set Locator(mbActive) $f.mbActive

    # Form the Active menu 
    #--------------------------------------------------------
    set m $Locator(mbActive).m
    foreach s $Locator(serverList) {
        $m add command -label $s \
            -command "LocatorSetActive $s"
    }

    #-------------------------------------------
    # Server->Bot->File frame
    #-------------------------------------------
    set f $fServer.fBot.fFile

    frame $f.fGrid   -bg $Gui(activeWorkspace)
    frame $f.fPrefix -bg $Gui(activeWorkspace)
    pack $f.fPrefix $f.fGrid \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Server->Bot->File->Prefix frame
    #-------------------------------------------
    set f $fServer.fBot.fFile.fPrefix

    eval {button $f.b -text "Prefix" -width 7 \
        -command "LocatorFilePrefix"} $Gui(WBA)
    eval {entry $f.e -textvariable Locator(File,prefix)} $Gui(WEA)
    pack $f.b -side left -padx $Gui(pad)
    pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

    #-------------------------------------------
    # Server->Bot->File->Grid frame
    #-------------------------------------------
    set f $fServer.fBot.fFile.fGrid

    set s File
    foreach x "msPoll" text \
        "{Update Period (ms)}" {
        eval {label $f.l$x -text "${text}:"} $Gui(WLA)
        eval {entry $f.e$x -textvariable Locator($s,$x) -width 7} $Gui(WEA)
        grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    #-------------------------------------------
    # Server->Bot->OpenTracker frame
    #-------------------------------------------
    set f $fServer.fBot.fOpenTracker

    #--- create blt notebook
    blt::tabset $f.tsNotebook -relief flat -borderwidth 0
    pack $f.tsNotebook -side top

    #--- notebook configure
    $f.tsNotebook configure -width 250
    $f.tsNotebook configure -height 500 
    $f.tsNotebook configure -background $::Gui(activeWorkspace)
    $f.tsNotebook configure -activebackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -selectbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -tabbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightbackground $::Gui(activeWorkspace)
    $f.tsNotebook configure -highlightcolor $::Gui(activeWorkspace)
    $f.tsNotebook configure -foreground black
    $f.tsNotebook configure -activeforeground black
    $f.tsNotebook configure -selectforeground black
    $f.tsNotebook configure -tabforeground black
    $f.tsNotebook configure -relief flat
    $f.tsNotebook configure -tabrelief raised

    #--- tab configure
    set i 0
    foreach t "Setup PC LM ICP" \
            l "Setup PivotCalibration LandmarkRegistration ICPRegistration" {
        $f.tsNotebook insert $i $t
        frame $f.tsNotebook.f$t -bg $Gui(activeWorkspace) -bd 2 
        LocatorBuildGUIFor${l} $f.tsNotebook.f$t

        $f.tsNotebook tab configure $t -window $f.tsNotebook.f$t 
        $f.tsNotebook tab configure $t -activebackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -selectbackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -background $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -fill both -padx 2 -pady 1 

        incr i
    }


    #-------------------------------------------
    # Server->Bot->Images frame
    #-------------------------------------------
    set f $fServer.fBot.fImages

    frame $f.fGrid   -bg $Gui(activeWorkspace)
    frame $f.fPrefix -bg $Gui(activeWorkspace)
    frame $f.fNum    -bg $Gui(activeWorkspace)
    pack $f.fPrefix $f.fGrid $f.fNum \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Server->Bot->Images->Prefix frame
    #-------------------------------------------
    set f $fServer.fBot.fImages.fPrefix

    eval {button $f.b -text "Prefix" -width 7 \
        -command "LocatorImagesPrefix"} $Gui(WBA)
    eval {entry $f.e -textvariable Locator(Images,prefix)} $Gui(WEA)
    pack $f.b -side left -padx $Gui(pad)
    pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

    #-------------------------------------------
    # Server->Bot->Images->Grid frame
    #-------------------------------------------
    set f $fServer.fBot.fImages.fGrid

    set s Images
    foreach x "firstNum lastNum increment msPoll" text \
        "{First image number} {Last image number} {Image increment} {Update period (ms)}" {
        eval {label $f.l$x -text "${text}:"} $Gui(WLA)
        eval {entry $f.e$x -textvariable Locator($s,$x) -width 7} $Gui(WEA)
        grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    #-------------------------------------------
    # Server->Bot->Images->Num frame
    #-------------------------------------------
    set f $fServer.fBot.fImages.fNum

    eval {label $f.l -text "Current Image Number"} $Gui(WLA)
    eval {entry $f.e -textvariable Locator(imageNum) -width 6 -state disabled} $Gui(WEA)
    pack $f.l $f.e -side left -padx $Gui(pad)


    #-------------------------------------------
    # Server->Bot->Flashpoint frame
    #-------------------------------------------
    set f $fServer.fBot.fFlashpoint

    frame $f.fStatus  -bg $Gui(activeWorkspace)
    frame $f.fGrid    -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fPatient -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fNum     -bg $Gui(activeWorkspace)
    pack  $f.fStatus $f.fGrid $f.fPatient $f.fNum \
        -side top -fill x -pady $Gui(pad)

    #-------------------------------------------
    # Server->Bot->Flashpoint->Status frame
    #-------------------------------------------
    set f $fServer.fBot.fFlashpoint.fStatus

    eval {label $f.lLocTitle -text "Locator Status"} $Gui(WTA)
    eval {label $f.lLocStatus -text "None" -width 8} $Gui(WLA)
    grid $f.lLocTitle $f.lLocStatus -pady 0 -padx $Gui(pad)
    set Locator(lLocStatus) $f.lLocStatus

    #-------------------------------------------
    # Server->Bot->Flashpoint->Grid frame
    #-------------------------------------------
    set f $fServer.fBot.fFlashpoint.fGrid

    eval {label $f.lTitle -text "Server Connection"} $Gui(WTA)
    grid $f.lTitle -columnspan 2 -pady $Gui(pad)

    set s Flashpoint
    foreach x "host port msPoll" text \
        "{Host name} {Port number} {Update period (ms)}" {
        eval {label $f.l$x -text "${text}:"} $Gui(WLA)
        eval {entry $f.e$x -textvariable Locator($s,$x) -width 10} $Gui(WEA)
        grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    #-------------------------------------------
    # Server->Bot->Flashpoint->Patient frame
    #-------------------------------------------
    set f $fServer.fBot.fFlashpoint.fPatient

    eval {label $f.lTitle -text "Patient Position"} $Gui(WTA)

    foreach pos "TblPos PatEntry PatPos" \
        name "{Table} {Entry} {Patient}" width "12 12 12"\
        choices "{$Locator(tblPosList)} {$Locator(patEntryList)} \
            {$Locator(patPosList)}" {
        eval {label $f.l$pos -text "$name:"} $Gui(WLA)
        eval {menubutton $f.mb$pos -text "$Locator([Uncap $pos])" \
            -relief raised -bd 2 -width $width -menu $f.mb$pos.menu} $Gui(WMBA)
        set Locator(mb$pos) $f.mb$pos
        eval {menu $f.mb$pos.menu} $Gui(WMA)
            set m $f.mb$pos.menu
            foreach choice $choices {
                $m add command -label $choice -command \
                    "LocatorSetPatientPosition [Uncap $pos] $choice"
            }
    }
    grid $f.lTitle -columnspan 2 -pady $Gui(pad)
    grid $f.lPatEntry $f.mbPatEntry  -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.lPatPos $f.mbPatPos  -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.lTblPos $f.mbTblPos -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.mbTblPos $f.mbPatEntry $f.mbPatPos -sticky w

    #-------------------------------------------
    # Server->Bot->Flashpoint->Num frame
    #-------------------------------------------
    set f $fServer.fBot.fFlashpoint.fNum

    eval {label $f.l -text "Current Image Number"} $Gui(WLA)
    eval {entry $f.e -textvariable Locator(imageNum) -width 6 -state disabled} $Gui(WEA)
    pack $f.l $f.e -side left -padx $Gui(pad)

    #-------------------------------------------
    # Server->Bot->File frame
    #-------------------------------------------
    set f $fServer.fBot.fCsys

    eval {checkbutton $f.cCsys \
        -text "Csys" -variable Locator(csysVisible) -width 4 \
        -indicatoron 0 -command "LocatorCsysToggle"} $Gui(WCA)
    pack $f.cCsys -side top -fill x -pady $Gui(pad)


    #-------------------------------------------
    # Handpiece frame
    #-------------------------------------------
    set fHandpiece $Module(Locator,fHandpiece)
    set f $fHandpiece

    # Frames
    frame $f.fOffsetSize -bg $Gui(activeWorkspace)
    frame $f.fPos        -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fColor      -bg $Gui(activeWorkspace) -relief groove -bd 3 

    pack $f.fOffsetSize -side top -fill x
    pack $f.fPos $f.fColor \
        -side top -padx $Gui(pad) -pady $Gui(pad) -fill x


    #-------------------------------------------
    # Handpiece->OffsetSize Frame
    #-------------------------------------------
    set f $fHandpiece.fOffsetSize

    frame $f.fOffset   -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fSize     -bg $Gui(activeWorkspace) -relief groove -bd 3 

    pack $f.fOffset $f.fSize \
        -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

    #-------------------------------------------
    # Handpiece->OffsetSize->Offse Frame
    #-------------------------------------------
    set f $fHandpiece.fOffsetSize.fOffset

    eval {label $f.l -text "Offset from Tip"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $f.f
    foreach axis "N T NxT" text "Normal Trans. {N x T}" \
        var "normalOffset transverseOffset crossOffset" {
        eval {label $f.l$axis -text "$text"} $Gui(WLA)
        eval {entry $f.e$axis -textvariable Locator($var) -width 4} $Gui(WEA)
        bind $f.e$axis <Return> "LocatorSetPosition; Render3D"
        grid $f.l$axis $f.e$axis -pady 2 -padx $Gui(pad) -sticky e
        grid $f.e$axis -sticky w 
    }

    #-------------------------------------------
    # Handpiece->OffsetSize->Size Frame
    #-------------------------------------------
    set f $fHandpiece.fOffsetSize.fSize

    eval {label $f.l -text "Size (mm)"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $f.f
    foreach var "normalLen transverseLen radius" \
        text "{Normal} {Trans.} {Radius}" {
        eval {label $f.l$var -text "$text"} $Gui(WLA)
        eval {entry $f.e$var -textvariable Locator($var) -width 4} $Gui(WEA)
        bind $f.e$var <Return> "LocatorSetSize; LocatorSetMatrices; Render3D"
        grid $f.l$var $f.e$var -pady 2 -padx $Gui(pad) -sticky e
        grid $f.e$var -sticky w
    }


    #-------------------------------------------
    # Handpiece->Pos Frame
    #-------------------------------------------
    set f $fHandpiece.fPos

    eval {label $f.l -text "Position & Orientation"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $f.f
    eval {label $f.l -text ""} $Gui(WLA)
    foreach ax "x y z" text "R A S" {
        eval {label $f.l$ax -text $text -width 7} $Gui(WLA)
    }
    grid $f.l $f.lx $f.ly $f.lz -pady 2 -padx $Gui(pad) -sticky e

    foreach axis "N T P" var "n t p" {
        eval {label $f.l$axis -text "$axis:"} $Gui(WLA)
        foreach ax "x y z" text "R A S" {
            eval {entry $f.e$axis$ax -justify right -width 7 \
                -textvariable Locator($var${ax}Str)} $Gui(WEA)
            bind $f.e$axis$ax <Return> "LocatorSetPosition; Render3D"
        }
        grid $f.l$axis $f.e${axis}x $f.e${axis}y $f.e${axis}z \
            -pady 2 -padx $Gui(pad) -sticky e
    }


    #-------------------------------------------
    # Handpiece->Color Frame
    #-------------------------------------------
    set f $fHandpiece.fColor

    eval {label $f.l -text "Color"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $fHandpiece.fColor.f
    foreach slider "Red Green Blue" {

        eval {label $f.l${slider} -text "${slider}"} $Gui(WLA)

        eval {entry $f.e${slider} -textvariable Locator([Uncap $slider]) \
            -width 3} $Gui(WEA)
            bind $f.e${slider} <Return>   "LocatorSetColor; Render3D"
            bind $f.e${slider} <FocusOut> "LocatorSetColor; Render3D"

        eval {scale $f.s${slider} -from 0.0 -to 1.0 -length 100 \
            -variable Locator([Uncap $slider]) \
            -command "LocatorSetColor; Render3D" \
            -resolution 0.1} $Gui(WSA) {-sliderlength 20} 
        set Locator(s$slider) $f.s$slider

        grid $f.l${slider} $f.e${slider} $f.s${slider}  \
            -pady 2 -padx 5 -sticky e
    }
}



proc LocatorBuildGUIForPivotCalibration {parent} {
    global Locator Gui 

    set f $parent
    foreach x "1 2 3" {
        if {$x != 1} {
            frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        } else {
            frame $f.f$x -bg $Gui(activeWorkspace)
        }
        pack $f.f$x -side top -pady 1 -fill x 
    }

    set f $parent.f1
    eval {label $f.lTitle -text "Pivot calibration:"} $Gui(WTA)
    grid $f.lTitle -padx 1 -pady 2

    set f $parent.f2
    eval {label $f.lTitle -text "Collect data:"} $Gui(WTA)
    DevAddButton $f.bStart "Start" "LocatorStartCollectPCData" 10 
    DevAddButton $f.bStop "Stop" "LocatorStopCollectPCData" 10 
    blt::table $f \
        0,0 $f.lTitle -padx 1 -pady 5 -fill x -cspan 2 \
        1,0 $f.bStart  -padx 1 -pady 3 -anchor e \
        1,1 $f.bStop  -padx 1 -pady 3 -anchor w


    set f $parent.f3
    foreach x "Up Down" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top 
    }

    set f $parent.f3.fUp
    eval {label $f.lTitle -text "Calibrate:"} $Gui(WTA)
    DevAddButton $f.bCompute "Compute" "LocatorComputePivotCalibration" 8 
    DevAddButton $f.bApply "Apply" "LocatorApplyPivotCalibration" 8 
    DevAddButton $f.bReset "Reset" "LocatorResetPivotCalibration" 8 

    blt::table $f \
        0,0 $f.lTitle -padx 1 -pady 5 -fill x -cspan 3 \
        1,0 $f.bCompute  -fill x -padx 1 -pady 3 -anchor w \
        1,1 $f.bApply  -fill x -padx 1 -pady 3 -anchor w \
        1,2 $f.bReset -fill x -padx 1 -pady 3 -anchor w


    set f $parent.f3.fDown
    eval {label $f.lPivot -text "Pivot pos:"} $Gui(WLA)
    eval {entry $f.ePivotX -width 16 -textvariable Locator(entry,pivotPosX)} $Gui(WEA)
    eval {label $f.lPivotX -text "x"} $Gui(WLA)
    eval {entry $f.ePivotY -width 16 -textvariable Locator(entry,pivotPosY)} $Gui(WEA)
    eval {label $f.lPivotY -text "y"} $Gui(WLA)
    eval {entry $f.ePivotZ -width 16 -textvariable Locator(entry,pivotPosZ)} $Gui(WEA)
    eval {label $f.lPivotZ -text "z"} $Gui(WLA)
 
    eval {label $f.lTrans -text "Translation:"} $Gui(WLA)
    eval {entry $f.eTransX -width 16 -textvariable Locator(entry,pivotTransX)} $Gui(WEA)
    eval {label $f.lTransX -text "x"} $Gui(WLA)
    eval {entry $f.eTransY -width 16 -textvariable Locator(entry,pivotTransY)} $Gui(WEA)
    eval {label $f.lTransY -text "y"} $Gui(WLA)
    eval {entry $f.eTransZ -width 16 -textvariable Locator(entry,pivotTransZ)} $Gui(WEA)
    eval {label $f.lTransZ -text "z"} $Gui(WLA)
 
    eval {label $f.lRMSE -text "RMSE:"} $Gui(WLA)
    eval {entry $f.eRMSE -width 16 -textvariable Locator(entry,pivotRMSE)} $Gui(WEA)


    blt::table $f \
        0,0 $f.lPivot -padx 1 -pady 1 -anchor e \
        0,1 $f.ePivotX -fill x -padx 1 -pady 1 -anchor w \
        0,2 $f.lPivotX -fill x -padx 1 -pady 1 -anchor w \
        1,1 $f.ePivotY -fill x -padx 1 -pady 1 -anchor w \
        1,2 $f.lPivotY -fill x -padx 1 -pady 1 -anchor w \
        2,1 $f.ePivotZ -fill x -padx 1 -pady 1 -anchor w \
        2,2 $f.lPivotZ -fill x -padx 1 -pady 1 -anchor w \
        3,0 $f.lTrans  -fill x -padx 1 -pady 1 -anchor w \
        3,1 $f.eTransX -fill x -padx 1 -pady 1 -anchor w \
        3,2 $f.lTransX -fill x -padx 1 -pady 1 -anchor w \
        4,1 $f.eTransY -fill x -padx 1 -pady 1 -anchor w \
        4,2 $f.lTransY -fill x -padx 1 -pady 1 -anchor w \
        5,1 $f.eTransZ -fill x -padx 1 -pady 1 -anchor w \
        5,2 $f.lTransZ -fill x -padx 1 -pady 1 -anchor w \
        6,0 $f.lRMSE -padx 1 -pady 1 -anchor w \
        6,1 $f.eRMSE -padx 1 -pady 3 -anchor w
}



proc LocatorStartCollectPCData {} {
    global Locator 

    Locator(OpenTracker,src) CollectDataForPivotCalibration 1
}



proc LocatorStopCollectPCData {} {
    global Locator 

    Locator(OpenTracker,src) CollectDataForPivotCalibration 0
}



proc LocatorComputePivotCalibration {} {
    global Locator 

   Locator(OpenTracker,src) ComputePivotCalibration

   # get values and updat to interface
   set rmse [Locator(OpenTracker,src) GetRMSE]
   set Locator(entry,pivotRMSE) $rmse

   set pos [Locator(OpenTracker,src) GetPivotPosition] 
   set Locator(entry,pivotPosX) [lindex $pos 0] 
   set Locator(entry,pivotPosY) [lindex $pos 1] 
   set Locator(entry,pivotPosZ) [lindex $pos 2] 
 
   set trans [Locator(OpenTracker,src) GetTranslation] 
   set Locator(entry,pivotTransX) [lindex $trans 0] 
   set Locator(entry,pivotTransY) [lindex $trans 1] 
   set Locator(entry,pivotTransZ) [lindex $trans 2] 
}



proc LocatorApplyPivotCalibration {} {
    global Locator 

    Locator(OpenTracker,src) ApplyPivotCalibration 1
}



proc LocatorResetPivotCalibration {} {
    global Locator 

    Locator(OpenTracker,src) ApplyPivotCalibration 0
}



proc LocatorBuildGUIForICPRegistration {parent} {
    global Locator Gui Model 

    set f $parent

    foreach x "0 1 2 3" {
        if {$x != 0 && $x != 3} {
            frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        } else {
            frame $f.f$x -bg $Gui(activeWorkspace)
        }
        pack $f.f$x -side top -pady 1 -fill x 
    }


    set f $parent.f0
    eval {label $f.lTitle -text "ICP registration:"} $Gui(WTA)
    grid $f.lTitle -padx 1 -pady 2

    set f $parent.f1
    eval {label $f.lTitle -text "Choose a model:"} $Gui(WTA)
    DevAddButton $f.bUpdate "Update" "LocatorUpdateModelList" 6 

    # choose a model for ICP registration as the target cloud of points 
    eval {menubutton $f.mbICP \
        -text "None" \
        -relief raised -bd 2 -width 30 \
        -menu $f.mbICP.m} $Gui(WMBA)
    eval {menu $f.mbICP.m} $Gui(WMA)
    blt::table $f \
    0,0 $f.lTitle -padx 2 -pady 5 -anchor e \
    0,1 $f.bUpdate  -padx 2 -pady 5 -anchor w \
    1,0 $f.mbICP  -padx 3 -pady 5 -fill x -cspan 2

    set Locator(mbICP) $f.mbICP

    set f $parent.f2
    eval {label $f.lTitle -text "Collect data:"} $Gui(WTA)
    DevAddButton $f.bStart "Start" "LocatorStartCollectICPData" 8 
    DevAddButton $f.bStop "Stop" "LocatorStopCollectICPData" 8 
    DevAddButton $f.bLoad "Load" "LocatorLoadICPPoints" 8 
    DevAddButton $f.bSave "Save" "LocatorSaveICPPoints" 8 
 
    blt::table $f \
        0,0 $f.lTitle -padx 1 -pady 5 -fill x -cspan 2 \
        1,0 $f.bStart  -padx 1 -pady 2 -anchor e \
        1,1 $f.bStop  -padx 1 -pady 2 -anchor w \
        2,0 $f.bLoad  -padx 1 -pady 2 -anchor e \
        2,1 $f.bSave  -padx 1 -pady 2 -anchor w

    set f $parent.f3
    DevAddButton $f.bReg "Go" "LocatorICPRegister" 8 
    DevAddButton $f.bReset "Remove" "LocatorRemoveICPRegistration" 8 

    grid $f.bReg $f.bReset -padx 1 -pady 3 

}



proc LocatorICPRegister {} {
    global Locator Gui Model 

}



proc LocatorRemoveICPRegistration {} {
    global Locator Gui Model 

}



proc LocatorStopCollectICPData {} {
    global Locator Gui Model 

}



proc LocatorStartCollectICPData {} {
    global Locator Gui Model 

}



proc LocatorSaveICPPoints {} {
    global Locator Gui Model 

}



proc LocatorLoadICPPoints {} {
    global Locator Gui Model 

}



proc LocatorUpdateModelList {} {
    global Locator Gui Model 

    set num [llength $Model(idList)]
    if {$num > 0} {
        set m $Locator(mbICP).m
        $m delete 0 [expr {$num -1}]
        foreach id $Model(idList) {
            set name [Model($id,node) GetName]
            $m add command -label $name \
                -command "set Locator(ICPModelId) $id; \
                    $Locator(mbICP) config -text $name; \
                    LocatorSetICPModel $id"
        }
    }


}



proc LocatorSetICPModel {id} {
    global Locator Gui Model 

#    puts "id = $id"
    set poly $Model($id,polyData)
    set Locator(ICPModelPoints) [$poly GetPoints]
 
#   set points [$poly GetPoints]
#    set n [$points GetNumberOfPoints]

#    puts "num of points in model = $n"


#    if {$n > 0} {
#        for {set i 0} {$i < $n} {incr i} {
#            set p [$points GetPoint $i]
#            puts " $i: p = $p"
#        }
#    }

}



proc LocatorBuildGUIForLandmarkRegistration {parent} {
    global Locator Gui 

    set f $parent
    foreach x "0 1 2 3" {
        if {$x == 1 || $x == 2} {
            frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        } else {
            frame $f.f$x -bg $Gui(activeWorkspace)
        }
        pack $f.f$x -side top -pady 1 -fill x 
    }

    set f $parent.f0
    eval {label $f.lTitle -text "Landmark registration:"} $Gui(WTA)
    grid $f.lTitle -padx 1 -pady 2

    set f $parent.f1
    eval {label $f.lTitle -text "Add a point pair:"} $Gui(WTA)
 
    eval {label $f.lPatient -text "Pat space:"} $Gui(WLA)
    eval {entry $f.ePatient -width 20 -textvariable Locator(entry,patientCoords)} $Gui(WEA)
    eval {label $f.lSlicer -text "Slicer space:"} $Gui(WLA)
    eval {entry $f.eSlicer -width 20 -textvariable Locator(entry,slicerCoords)} $Gui(WEA)
    DevAddButton $f.bGet "Get" "LocatorGetPatientCoords" 3 
    DevAddButton $f.bOK "OK" "LocatorAddPointPair" 8 

    blt::table $f \
        0,0 $f.lTitle -padx 1 -pady 5 -fill x -cspan 2 \
        1,0 $f.lPatient -padx 1 -pady 1 -anchor e \
        1,1 $f.ePatient -fill x -padx 1 -pady 1 -anchor w \
        1,2 $f.bGet  -fill x -padx 1 -pady 1 -anchor w \
        2,0 $f.lSlicer -padx 1 -pady 1 -anchor e \
        2,1 $f.eSlicer -padx 1 -pady 1 -anchor w \
        3,1 $f.bOK -padx 1 -pady 3 -anchor w


    set f $parent.f2
    foreach x "Up Down" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top 
    }

    set f $parent.f2.fUp
    eval {label $f.lTitle -text "Defined point pairs:"} $Gui(WTA)
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    scrollbar $f.hs -orient horizontal -bg $Gui(activeWorkspace)
    set Locator(PointsVerScroll) $f.vs
    set Locator(PointsHonScroll) $f.hs
    listbox $f.lb \
        -height 4 -width 24 \
        -bg $Gui(activeWorkspace) \
        -xscrollcommand {$::Locator(PointsHonScroll) set} \
        -yscrollcommand {$::Locator(PointsVerScroll) set}
    set Locator(pointPairListBox) $f.lb
    $Locator(PointsHonScroll) configure -command {$Locator(pointPairListBox) xview}
    $Locator(PointsVerScroll) configure -command {$Locator(pointPairListBox) yview}

    blt::table $f \
        0,0 $f.lTitle -padx 10 -pady 5 \
        1,0 $Locator(pointPairListBox) -padx 2 -pady 1 -fill x \
        1,1 $Locator(PointsVerScroll) -fill y -padx 2 -pady 1 \
        2,0 $Locator(PointsHonScroll) -fill x -padx 2 -pady 1


    set f $parent.f2.fDown
    DevAddButton $f.bLoad "Load" "LocatorLoadPointPairs" 8 
    DevAddButton $f.bSave "Save" "LocatorSavePointPair" 8 
    DevAddButton $f.bDelete "Delete" "LocatorDeletePointPair" 8 
    grid $f.bLoad $f.bSave $f.bDelete -padx 1 -pady 2

    set f $parent.f3
    DevAddButton $f.bReg "Go" "LocatorRegister" 8 
    DevAddButton $f.bReset "Remove" "LocatorRemoveRegistration" 8 

    grid $f.bReg $f.bReset -padx 1 -pady 2 

}



proc LocatorRemoveRegistration {} {
    global Locator

    Locator(OpenTracker,src) DeleteRegistration

}



proc LocatorLoadPointPairs {} {
    global Locator

    # read data from file
    set fileType {{"Text" *.txt}}
    set fileName [tk_getOpenFile -filetypes $fileType -parent .]

    # if user just wanted to cancel
    if {[string length $fileName] <= 0} {
        return
    }
    
    set fd [open $fileName r]
    set data [read $fd]
    set lines [split $data "\n"]
    foreach line $lines {
        set line [string trim $line]
        eval $line
    }
    close $fd
}



proc LocatorSavePointPairs {} {
    global Locator

    set dir [pwd]
    set fileName [file join $dir "opentracker_point_pairs.txt"]

    set fd [open $fileName w]
    puts $fd "\n\n\n\n\n"
 
    set comment "# This text file saves the user input. Do not edit it.\n"
    puts $fd $comment

    set comment "# the point pair list"
    puts $fd $comment
    set str "set Locator(pointPairList) \"\"\n"
    puts $fd $str
    set str "\$Locator(pointPairListBox) delete 0 end\n"
    puts $fd $str

    foreach x $Locator(pointPairList) {
        set str "lappend Locator(pointPairList) \{$x\}\n"
        puts $fd $str
        set str "\$Locator(pointPairListBox) insert end \{$x\}\n" 
        puts $fd $str
    }

    close $fd
}




proc LocatorRegister {} {
    global Locator

    set size [llength $Locator(pointPairList)]
    if {$size < 2} {
        DevErrorWindow "At least 2 pairs of landmarks are needed for registration."
        return
    }

    set size [llength $Locator(pointPairList)]
    Locator(OpenTracker,src) SetNumberOfPoints $size 


    set id 0 
    foreach x $Locator(pointPairList) {
        set c [string trim $x]
        set i [string first ")" $c]
        if {$i < 0} {
            DevErrorWindow "Wrong point pair found: $c."
            return
        }

        set start 1
        set done [expr $i - 1]
        set pc [string range $c $start $done] 

        set start [expr $i + 2] 
        set sc [string range $c $start end-1] 


        Locator(OpenTracker,src) AddPoint $id [lindex $sc 0] [lindex $sc 1] [lindex $sc 2] \
                                              [lindex $pc 0] [lindex $pc 1] [lindex $pc 2]  

        incr id
    }

    set error [Locator(OpenTracker,src) DoRegistration] 
    if {$error} {
        DevErrorWindow "Error registration between TargetLandmarks and SourceLandmarks."
        return
    }

    set Locator(lmtMatrix) [Locator(OpenTracker,src) GetLandmarkTransformMatrix] 
    puts [$Locator(lmtMatrix) Print]

}



proc LocatorDeletePointPair {} {
    global Locator

    set curs [$Locator(pointPairListBox) curselection]
    if {$curs >= 0} {
        $Locator(pointPairListBox) delete $curs
        set size [llength $Locator(pointPairList)]
        set Locator(pointPairList) \
            [lreplace $Locator(pointPairList) $curs $curs]
        set size [llength $Locator(pointPairList)]

    } else {
        DevErrorWindow "Select a point pair to delete."
    }

}



proc LocatorAddPointPair {} {
    global Locator

    set pc $Locator(entry,patientCoords)
    set pc [string trim $pc]
    regsub -all {( )+} $pc "   " pc 

    set sc $Locator(entry,slicerCoords)
    set sc [string trim $sc]
    regsub -all {( )+} $sc "   " sc 


    set psc "($pc)($sc)"
    set index [lsearch -exact $Locator(pointPairList) $psc]
    if {$index != -1} { 
        DevErrorWindow "The point pair is already added in."
        return
    }

 

    # Keep the new point in the point list
    lappend Locator(pointPairList) $psc

    # Put the point pair list into the list box
    $Locator(pointPairListBox) delete 0 end
    foreach x $Locator(pointPairList) {
        $Locator(pointPairListBox) insert end $x 
    }
}



proc LocatorGetPatientCoords {} {
    global Locator

    set Locator(entry,patientCoords) "$Locator(px)   $Locator(py)   $Locator(pz)"
}



proc LocatorBuildGUIForSetup {parent} {
    global Locator Gui 

    set s OpenTracker
    set f $parent
    foreach x "1 2 3" {
        frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        pack $f.f$x -side top -pady 2 -fill x 
    }

    set f $parent.f1
    DevAddFileBrowse $f Locator "OpenTracker,cfg" "Config file:" \
        "" "xml" "\$Volume(DefaultDir)" "Open" \
        "Browse for a config file" "" "Absolute"

    set f $parent.f2
    foreach x "multiBy msPoll" name "{Multiply by:} {Update period (ms):}" {
        DevAddLabel $f.l$x $name 
        eval {entry $f.e$x -textvariable Locator($s,$x) -width 13} $Gui(WEA)

        grid $f.l$x $f.e$x -pady $Gui(pad) -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    # Sensor NO: 1, 2, 3, or 4
    DevAddLabel $f.lSensor "Sensor NO:" 
    eval {menubutton $f.mbSensor \
        -text "1" \
        -relief raised -bd 2 -width 13 \
        -menu $f.mbSensor.m} $Gui(WMBA)
    eval {menu $f.mbSensor.m} $Gui(WMA)
    grid $f.lSensor $f.mbSensor -pady $Gui(pad) -padx $Gui(pad) -sticky e
    grid $f.mbSensor -sticky w
 
    set Locator(mbSensor) $f.mbSensor

    set m $Locator(mbSensor).m
    foreach s "1 2 3 4" {
        $m add command -label $s \
            -command "set Locator(SensorNO) $s; \
                      $Locator(mbSensor) config -text $s; \
                      Locator(OpenTracker,src) SetSensorNO $s"
    }

}



#-------------------------------------------------------------------------------
# .PROC LocatorSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetActive {s} {
    global Locator

    set Locator(server) $s
    $Locator(mbActive) config -text $s
    raise $Locator(f$s)
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetPatientPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetPatientPosition {{key ""} {value ""}} {
    global Locator
    
    if {$key != ""} {
        set Locator($key) $value
    }
    foreach key "TblPos PatEntry PatPos" {
        $Locator(mb$key) config -text "$Locator([Uncap $key])"
    }

    # Send to MRT workstation
    Locator(Flashpoint,src) SetPosition \
        [lsearch $Locator(tblPosList)   $Locator(tblPos)] \
        [lsearch $Locator(patEntryList) $Locator(patEntry)] \
        [lsearch $Locator(patPosList)   $Locator(patPos)]
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriverAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriverAll {x} {
    global Slice 

    foreach s $Slice(idList) {
        LocatorSetDriver $s $x
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriver
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriver {s name} {
    global Locator
    
    # Change button text
    $Locator(mbDriver${s}) config -text $name

    # Change variable
    set Locator($s,driver) $name

    if {$name == "User"} {
        Slicer SetDriver $s 0
    } else {
        Slicer SetDriver $s 1
    }

    # Force recomputation of the reformat matrix
    Slicer SetDirectNTP \
        $Locator(nx) $Locator(ny) $Locator(nz) \
        $Locator(tx) $Locator(ty) $Locator(tz) \
        $Locator(px) $Locator(py) $Locator(pz) 
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetVisibility {} {
    global Locator 

    foreach actor $Locator(actors) {
        ${actor}Actor SetVisibility $Locator(visibility)
    }
    LocatorSetTransverseVisibility
    LocatorSetGuideVisibility
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetTransverseVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetTransverseVisibility {} {
    global Locator 

    if {$Locator(visibility) == 1} {
        transverseActor SetVisibility $Locator(transverseVisibility)
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetGuideVisibility
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetGuideVisibility {} {
    global Locator 

    if {$Locator(visibility) == 1} {
        for {set i 0} {$i < $::Locator(guideSteps)} {incr i} {
            guide${i}Actor SetVisibility $Locator(guideVisibility)
        }
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetColor
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetColor {{value ""}} {
    global Locator

    # Validate input
    if {[ValidateFloat $Locator(red)] == 0} {
        tk_messageBox -message "Red must be a number between 0.0 and 1.0"
        return
    }
    if {[ValidateFloat $Locator(green)] == 0} {
        tk_messageBox -message "Green must be a number between 0.0 and 1.0"
        return
    }
    if {[ValidateFloat $Locator(blue)] == 0} {
        tk_messageBox -message "Blue must be a number between 0.0 and 1.0"
        return
    }

    set Locator(diffuseColor) "$Locator(red) $Locator(green) $Locator(blue)"
    foreach actor $Locator(actors) {
        if {$actor == "opening"} {
            eval [${actor}Actor GetProperty] SetColor "1.0 1.0 0.0" 
        } else {
            eval [${actor}Actor GetProperty] SetColor $Locator(diffuseColor)
        }
    }

    foreach slider "Red Green Blue" {
        ColorSlider $Locator(s$slider) $Locator(diffuseColor)
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetSize
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetSize {} {
    global Locator

    if {[ValidateFloat $Locator(radius)] == 0} {
        tk_messageBox -message "Radius must be a floating point number"
        return
    }
    if {[ValidateFloat $Locator(normalLen)] == 0} {
        tk_messageBox -message "Normal Length must be a floating point number"
        return
    }
    if {[ValidateFloat $Locator(transverseLen)] == 0} {
        tk_messageBox -message "Transverse Length must be a floating point number"
        return
    }

    normalSource SetRadius $Locator(radius) 
    normalSource SetHeight $Locator(normalLen)
    transverseSource SetRadius $Locator(radius) 
    transverseSource SetHeight [expr $Locator(transverseLen)]
    tipSource SetRadius [expr 1.0 * $Locator(radius)] 
}

#-------------------------------------------------------------------------------
# .PROC LocatorCreateFiducial 
# Make a fiducial at the tip of the Locator.  Create a locator list if it 
# doesn't already exist.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorCreateFiducial {} {
    if { ![FiducialsCheckListExistence "Locator"] } {
        FiducialsCreateFiducialsList default "Locator"
    }
    set pid [FiducialsCreatePointFromWorldXYZ "Locator" $::Locator(px) $::Locator(py) $::Locator(pz)]
    Point($pid,node) SetOrientationWXYZFromMatrix4x4 Locator(transverseMatrix)
    FiducialsUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC LocatorDeleteFiducial 
# Delete last fiducial on Locator list if it exists 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorDeleteFiducial {} {

    if { ![FiducialsCheckListExistence "Locator" fid] } {
        return
    }

    if { [llength $::Fiducials($fid,pointIdList)] == 0 } {
        return
    }

    set pid [lindex $::Fiducials($fid,pointIdList) end]

    FiducialsDeletePoint $fid $pid
    FiducialsUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC LocatorNameUpated
# Change the name of the last fiducial created
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorNameUpated {} {

    if { ![FiducialsCheckListExistence "Locator" fid] } {
        DevErrorWindow "Must have created locator fiducials before changing name"
        return
    }

    if { [llength $::Fiducials($fid,pointIdList)] == 0 } {
        DevErrorWindow "Must have created locator fiducials before changing name"
        return
    }

    set pid [lindex $::Fiducials($fid,pointIdList) end]

    Point($pid,node) SetName $::Locator(fiducialName)
    set ::Locator(fiducialName) ""
    FiducialsUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetMatrices
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetMatrices {} {
    global Locator

    if {[ValidateFloat $Locator(radius)] == 0} {
        return
    }
    if {[ValidateFloat $Locator(normalLen)] == 0} {
        return
    }
    if {[ValidateFloat $Locator(transverseLen)] == 0} {
        return
    }

    # Find transform, N, that brings the locator coordinate frame 
    # into the scanner frame.  Then invert N to M and set it to the locator's
    # userMatrix to position the locator within the world space.
    #
    # 1.) Concatenate a translation, T, TO the origin which is (-x,-y,-z)
    #     where the locator's position is (x,y,z).
    # 2.) Concatenate the R matrix.  If the locator's reference frame has
    #     axis Ux, Uy, Uz, then Ux is the TOP ROW of R, Uy is the second, etc.
    # 3.) Translate the cylinder so its tip is at the origin instead
    #     of the center of its tube.  Call this matrix C.
    # Then: N = C*R*T, M = Inv(N)
    #
    # (See page 419 and 429 of "Computer Graphics", Hearn & Baker, 1997,
    #  ISBN 0-13-530924-7)
    # 
    # The alternative approach used here is to find the transform, M, that
    # moves the scanner coordinate frame to the locator's.  
    # 
    # 1.) Translate the cylinder so its tip is at the origin instead
    #     of the center of its tube.  Call this matrix C.
    # 2.) Concatenate the R matrix.  If the locator's reference frame has
    #     axis Ux, Uy, Uz, then Ux is the LEFT COL of R, Uy is the second,etc.
    # 3.) Concatenate a translation, T, FROM the origin which is (x,y,z)
    #     where the locator's position is (x,y,z).
    # Then: M = T*R*C

    vtkMatrix4x4 locator_matrix
    vtkTransform locator_transform

    # Locator's offset: x0, y0, z0
    set x0 $Locator(px)
    set y0 $Locator(py)
    set z0 $Locator(pz)

    # Locator's coordinate axis:
    # Ux = T
    set Ux(x) $Locator(tx)
    set Ux(y) $Locator(ty)
    set Ux(z) $Locator(tz)
    # Uy = -N
    set Uy(x) [expr - $Locator(nx)]
    set Uy(y) [expr - $Locator(ny)]
    set Uy(z) [expr - $Locator(nz)]
    # Uz = Ux x Uy
    set Uz(x) [expr $Ux(y)*$Uy(z) - $Uy(y)*$Ux(z)]
    set Uz(y) [expr $Uy(x)*$Ux(z) - $Ux(x)*$Uy(z)]
    set Uz(z) [expr $Ux(x)*$Uy(y) - $Uy(x)*$Ux(y)]

    # Ux
    locator_matrix SetElement 0 0 $Ux(x)
    locator_matrix SetElement 1 0 $Ux(y)
    locator_matrix SetElement 2 0 $Ux(z)
    locator_matrix SetElement 3 0 0
    # Uy
    locator_matrix SetElement 0 1 $Uy(x)
    locator_matrix SetElement 1 1 $Uy(y)
    locator_matrix SetElement 2 1 $Uy(z)
    locator_matrix SetElement 3 1 0
    # Uz
    locator_matrix SetElement 0 2 $Uz(x)
    locator_matrix SetElement 1 2 $Uz(y)
    locator_matrix SetElement 2 2 $Uz(z)
    locator_matrix SetElement 3 2 0
    # Bottom row
    locator_matrix SetElement 0 3 0
    locator_matrix SetElement 1 3 0
    locator_matrix SetElement 2 3 0
    locator_matrix SetElement 3 3 1

    # Set the vtkTransform to PostMultiply so a concatenated matrix, C,
    # is multiplied by the existing matrix, M: C*M (not M*C)
    locator_transform PostMultiply
    # M = T*R*C

    # TIP PART

    locator_transform Identity
    # T:
    locator_transform Translate [expr $x0] [expr $y0] [expr $z0]
    locator_transform GetMatrix Locator(tipMatrix)
    # VTK BUG (i shouldn't have to call modify for the matrix)?
    Locator(tipMatrix) Modified
    
    # NORMAL PART

    locator_transform Identity
    # C:
    locator_transform Translate 0 [expr $Locator(normalLen) / 2.0] 0
    # R:
    locator_transform Concatenate locator_matrix
    # T:
    locator_transform Translate [expr $x0] [expr $y0] [expr $z0]
    locator_transform GetMatrix Locator(normalMatrix)
    Locator(normalMatrix) Modified

    # TRANSVERSE PART

    locator_transform Identity
    # C: 
    locator_transform RotateZ 90
    locator_transform Translate \
        [expr [expr $Locator(transverseLen) / 2.0] - $Locator(radius)] \
        [expr $Locator(normalLen)] \
         0 
    # R:
    locator_transform Concatenate locator_matrix
    # T:
    locator_transform Translate [expr $x0] [expr $y0] [expr $z0]
    locator_transform GetMatrix Locator(transverseMatrix)
    Locator(transverseMatrix) Modified

    locator_matrix Delete
    locator_transform Delete
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetPosition
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetPosition {{value ""}} {
    global Locator

    foreach p "px py pz nx ny nz tx ty tz" {
        if {[ValidateFloat $Locator(${p}Str)] == 0} {
            tk_messageBox -message "Floating point numbers required."
            return
        }
    }

    # Read matrix
    set Locator(px) $Locator(pxStr) 
    set Locator(py) $Locator(pyStr) 
    set Locator(pz) $Locator(pzStr)
    set Locator(nx) $Locator(nxStr)
    set Locator(ny) $Locator(nyStr)
    set Locator(nz) $Locator(nzStr)
    set Locator(tx) $Locator(txStr)
    set Locator(ty) $Locator(tyStr)
    set Locator(tz) $Locator(tzStr)

    LocatorUseLocatorMatrix
}

#-------------------------------------------------------------------------------
# .PROC LocatorUseLocatorMatrix
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorUseLocatorMatrix {} {
    global Locator Slice

    foreach p "normalOffset transverseOffset crossOffset" {
        if {[ValidateFloat $Locator($p)] == 0} {
            tk_messageBox -message "$p must be a floating point number."
            return
        }
    }

    # Form arrays so we can use vector processing functions
    set P(x) $Locator(px)
    set P(y) $Locator(py)
    set P(z) $Locator(pz)
    set N(x) $Locator(nx)
    set N(y) $Locator(ny)
    set N(z) $Locator(nz)
    set T(x) $Locator(tx)
    set T(y) $Locator(ty)
    set T(z) $Locator(tz)

    # Ensure N, T orthogonal:
    #    C = N x T
    #    T = C x N
    Cross C N T
    Cross T C N

    # Ensure vectors are normalized
    Normalize N
    Normalize T
    Normalize C

    # Offset the Locator
    set n $Locator(normalOffset)
    set t $Locator(transverseOffset)
    set c $Locator(crossOffset)
    set Locator(px) [expr $P(x) + $N(x)*$n + $T(x)*$t + $C(x)*$c]
    set Locator(py) [expr $P(y) + $N(y)*$n + $T(y)*$t + $C(y)*$c]
    set Locator(pz) [expr $P(z) + $N(z)*$n + $T(z)*$t + $C(z)*$c]
    set Locator(nx) $N(x)
    set Locator(ny) $N(y)
    set Locator(nz) $N(z)
    set Locator(tx) $T(x)
    set Locator(ty) $T(y)
    set Locator(tz) $T(z)

    # Format display
    LocatorFormat
                
    # Position the rendered locator
    LocatorSetMatrices
            
    # Find slices with their input set to locator.
    # and set the slice matrix with the new locator data

    Slicer SetDirectNTP \
        $Locator(nx) $Locator(ny) $Locator(nz) \
        $Locator(tx) $Locator(ty) $Locator(tz) \
        $Locator(px) $Locator(py) $Locator(pz) 


}



#-------------------------------------------------------------------------------
# .PROC LocatorFormat
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorFormat {} {
    global Locator

    set Locator(nxStr) [format "%.2f" $Locator(nx)]
    set Locator(nyStr) [format "%.2f" $Locator(ny)]
    set Locator(nzStr) [format "%.2f" $Locator(nz)]
    set Locator(txStr) [format "%.2f" $Locator(tx)]
    set Locator(tyStr) [format "%.2f" $Locator(ty)]
    set Locator(tzStr) [format "%.2f" $Locator(tz)]
    set Locator(pxStr) [format "%.2f" $Locator(px)]
    set Locator(pyStr) [format "%.2f" $Locator(py)]
    set Locator(pzStr) [format "%.2f" $Locator(pz)]
}

#-------------------------------------------------------------------------------
# .PROC LocatorGetRealtimeID
#
# Returns the Realtime volume's ID.
# If there is no Realtime volume (Locator(idRealtime)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc LocatorGetRealtimeID {} {
    global Locator Volume Lut
        
    # If there is no Realtime volume, then create one
    if {$Locator(idRealtime) != "NEW"} {
        return $Locator(idRealtime)
    }
    
    # Create the node
    set n [MainMrmlAddNode Volume]
    set v [$n GetID]
    $n SetDescription "Realtime Volume"
    $n SetName        "Realtime"

    # Create the volume
    MainVolumesCreate $v

    LocatorSetRealtime $v

    MainUpdateMRML

    return $v
}


#-------------------------------------------------------------------------------
# .PROC LocatorSetRealtime
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorSetRealtime {v} {
    global Locator Volume

    set Locator(idRealtime) $v
    
    # Change button text, and show file prefix
    if {$v == "NEW"} {
        $Locator(mbRealtime) config -text $v
        set Locator(prefixRealtime) ""
    } else {
        $Locator(mbRealtime) config -text [Volume($v,node) GetName]
        set Locator(prefixRealtime) [MainFileGetRelativePrefix \
            [Volume($v,node) GetFilePrefix]]
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorWrite {data} {
    global Volume Locator

    # If the volume doesn't exist yet, then don't write it, duh!
    if {$Locator(id$data) == "NEW"} {
        tk_messageBox -message "Nothing to write."
        return
    }

    switch $data {
        Realtime {set v [LocatorGetRealtimeID]}
    }

    # Show user a File dialog box
    set Locator(prefix$data) [MainFileSaveVolume $v $Locator(prefix$data)]
    if {$Locator(prefix$data) == ""} {return}

    # Write
    MainVolumesWrite $v $Locator(prefix$data)

    # Prefix changed, so update the Volumes->Props tab
    MainVolumesSetActive $v
}

#-------------------------------------------------------------------------------
# .PROC LocatorRead
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorRead {data} {
    global Volume Locator Mrml

    # If the volume doesn't exist yet, then don't read it, duh!
    if {$Locator(id$data) == "NEW"} {
        tk_messageBox -message "Nothing to read."
        return
    }

    switch $data {
        Realtime {set v $Locator(idRealtime)}
    }

    # Show user a File dialog box
    set Locator(prefix$data) [MainFileOpenVolume $v $Locator(prefix$data)]
    if {$Locator(prefix$data) == ""} {return}
    
    # Read
    Volume($v,node) SetFilePrefix $Locator(prefix$data)
    Volume($v,node) SetFullPrefix \
        [file join $Mrml(dir) [Volume($v,node) GetFilePrefix]]
    if {[MainVolumesRead $v] < 0} {
        return
    }

    # Update pipeline and GUI
    MainVolumesUpdate $v

    # Prefix changed, so update the Models->Props tab
    MainVolumesSetActive $v
}

#-------------------------------------------------------------------------------
# .PROC LocatorRegisterCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorRegisterCallback {cb} {
    global Locator 

    if {[lsearch $Locator(callbackList) $cb] == -1} {
        lappend Locator(callbackList) $cb
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorUnRegisterCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorUnRegisterCallback {cb} {
    global Locator 

    if {[set i [lsearch $Locator(callbackList) $cb]] != -1} {
        set Locator(callbackList) [lreplace Locator(callbackList) $i $i]
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorPause
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorPause {} {
    global Locator

    if {$Locator(pause) == 0 && $Locator(connect) == 1} {
        LocatorLoop$Locator(server)
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorConnect
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorConnect {{value ""}} {
    global Gui  Locator Mrml

    if {$value != ""} {
        set Locator(connect) $value
    }

    # CONNECT
    if {$Locator(connect) == "1"} {
        switch $Locator(server) {

        "File" {
            set filename [file join $::env(SLICER_HOME) servers $Locator(File,prefix)].txt
            if {[catch {set Locator(File,fid) [open $filename r]} errmsg] == 1} {
                puts $errmsg
                tk_messageBox -message $errmsg
                set Locator(loop) 0
                set Locator(connect) 0
                return
            }
            set Locator(loop) 1
            $Locator(mbActive) config -state disabled
            LocatorLoopFile
        }

        "Flashpoint" {
            if {$Gui(pc) == 1} {
                tk_messageBox -message "\
The 3D Slicer may connect to a GE Flashpoint scanner from a 
Sun UltraSPARC, but not a PC.\n\n\
Set the server to 'Images' to process images on disk as
if they were coming from a scanner in real time."
                set Locator(connect) 0
                return
            }


            # You have to actually connect, dumbo
            set status [Locator(Flashpoint,src) OpenConnection \
             $Locator(Flashpoint,host) $Locator(Flashpoint,port)]
            if {$status == -1} {
                tk_messageBox -icon error -type ok -title $Gui(title) -message "Type start_spl_server on the mrt workstation, rookie\n\
host='$Locator(Flashpoint,host)' port='$Locator(Flashpoint,port)'"
                set Locator(loop) 0
                set Locator(connect) 0
                return
            }
            set Locator(loop) 1
            $Locator(mbActive) config -state disabled
            LocatorLoopFlashpoint
        }

        "Images" {
            # Initialize
            set n Locator(Images,node)
            $n SetFilePrefix $Locator(Images,prefix)
            $n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
            $n SetFilePattern %s.%03d
            set Locator(imageNum) ""

            set Locator(loop) 1
            $Locator(mbActive) config -state disabled
            LocatorLoopImages
        }

    "OpenTracker" {
        # Initialize
        Locator(OpenTracker,src) SetMultiRate $Locator(OpenTracker,multiBy)
        Locator(OpenTracker,src) Init $Locator(OpenTracker,cfg)
        set Locator(loop) 1
        $Locator(mbActive) config -state disabled
        LocatorLoopOpenTracker
        }
    }

    # DISCONNECT
    } else {
        set Locator(loop) 0
        set Locator(imageNum) ""
        $Locator(mbActive) config -state normal

        switch $Locator(server) {

        "File" {
            if {[catch {close $Locator(File,fid)} errmsg] == 1} {
                puts $errmsg
            }
        }

        "Flashpoint" {
            Locator(Flashpoint,src) CloseConnection
            set Locator(loop) 0
        }

        "Images" {
            # Nothing to do
        }
        }
    }
}


#-------------------------------------------------------------------------------
# .PROC LocatorLoopOpenTracker
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopOpenTracker {} {
    global Locator Slice

    if {$Locator(loop) == 0} {
        return
    }

    Locator(OpenTracker,src) PollRealtime


    #----------------    
    # NEW LOCATOR
    #----------------
    set locMatrix [Locator(OpenTracker,src) GetLocatorMatrix]


    # Read matrix
    set Locator(px) [$locMatrix GetElement 0 0]
    set Locator(py) [$locMatrix GetElement 1 0]
    set Locator(pz) [$locMatrix GetElement 2 0]
    set Locator(nx) [$locMatrix GetElement 0 1]
    set Locator(ny) [$locMatrix GetElement 1 1]
    set Locator(nz) [$locMatrix GetElement 2 1]
    set Locator(tx) [$locMatrix GetElement 0 2]
    set Locator(ty) [$locMatrix GetElement 1 2]
    set Locator(tz) [$locMatrix GetElement 2 2]


    LocatorUseLocatorMatrix

    # If we get a button press, draw the fiber
    # at the tip location of the locator
    set button [Locator(OpenTracker,src) GetButton]
    if {$button} { 
        set Select(xyz) "$Locator(px) $Locator(py) $Locator(pz)"
        eval DTMRISelectStartHyperStreamline $Select(xyz)
    }


    # simond - service callback list.
    # Perform realtime image processing
    foreach cb $Locator(callbackList) {
        $cb
    }

    # Render the slices that the locator is driving
    foreach s $Slice(idList) v "$Locator(pz) $Locator(px) $Locator(py)" {
        if {[Slicer GetDriver $s] == 1} {
            RenderSlice $s

            # Update the slice offset for all three windows
            set Slice($s,offset) $v 
        }
    }
    Render3D

    # Call update instead of update idletasks so that we
    # process user input like changing slice orientation
    update
    if {[ValidateInt $Locator(OpenTracker,msPoll)] == 0} {
        set Locator(OpenTracker,msPoll) 100
    }
    if {[ValidateFloat $Locator(OpenTracker,multiBy)] == 0} {
        set Locator(OpenTracker,multiBy) 1.0
    }

    after $Locator(OpenTracker,msPoll) LocatorLoopOpenTracker
}


#-------------------------------------------------------------------------------
# .PROC LocatorLoopFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopFile {} {
    global Slice Volume Locator

    if {$Locator(loop) == 0} {
        return
    }
    if {$Locator(pause) == 1} {
        return
    }

    # Read matrix
    if {[eof $Locator(File,fid)] == 1} {
        LocatorConnect 0
        return
    }
    gets $Locator(File,fid) line
    scan $line "%d %g %g %g %g %g %g %g %g %g" t nx ny nz tx ty tz px py pz

    if {[info exists pz] == 0} {
        if {[ValidateInt $Locator(File,msPoll)] == 0} {
            set Locator(File,msPoll) 100
        }
        after $Locator(File,msPoll) LocatorLoopFile
        return
    }

    set Locator(nx) $nx
    set Locator(ny) $ny
    set Locator(nz) $nz
    set Locator(tx) $tx
    set Locator(ty) $ty
    set Locator(tz) $tz
    set Locator(px) $px
    set Locator(py) $py
    set Locator(pz) $pz

    LocatorUseLocatorMatrix

    # Render the slices that the locator is driving
    foreach s $Slice(idList) {
        if {[Slicer GetDriver $s] == 1} {
            RenderSlice $s
        }
    }
    Render3D

    # Call update instead of update idletasks so that we
    # process user input like changing slice orientation
    update
    if {[ValidateInt $Locator(File,msPoll)] == 0} {
        set Locator(File,msPoll) 100
    }
    after $Locator(File,msPoll) LocatorLoopFile
}

#-------------------------------------------------------------------------------
# .PROC LocatorLoopImages
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopImages {} {
    global Slice Volume Locator Mrml

    if {$Locator(loop) == 0} {
        return
    }
    if {$Locator(pause) == 1} {
        return
    }

    # Compute next image number
    set v Locator(Images,vol)
    set n Locator(Images,node)
    if {$Locator(imageNum) == ""} {
        set num $Locator(Images,firstNum)
    } else {
        set num [expr $Locator(imageNum) + $Locator(Images,increment)]
        if {$num > $Locator(Images,lastNum)} {
            LocatorConnect 0
            return
        }
    }

    set Locator(imageNum) $num
    $n SetImageRange $num $num

    # Read header
    set filename [format [$n GetFilePattern] [$n GetFilePrefix] $num]
    set errmsg ""
    set errmsg [GetHeaderInfo [file join $Mrml(dir) $filename] $num $n 0]
    # BUG: this should say "!=" but then the image is blank!
    if {$errmsg == ""} {
        puts $errmsg
        puts "Assuming scan order of SI"
        $n ComputeRasToIjkFromScanOrder SI
    }

    # Read image data
    scan [$n GetImageRange] "%d %d" lo hi
    if {[CheckVolumeExists [$n GetFullPrefix] [$n GetFilePattern] $lo $hi] != ""} {
        LocatorConnect 0
        return
    }
    set Gui(progressText) "Reading [$n GetName]"
    $v Read
    $v Update

    # Copy the image to the Realtime volume
    set v [LocatorGetRealtimeID]
    set n Volume($v,node)
    vtkImageCopy copy
    copy SetInput [Locator(Images,vol) GetOutput]
    copy Update
    copy SetInput ""
    Volume($v,vol) SetImageData [copy GetOutput]
    copy SetOutput ""
    copy Delete

    # Set the header info
    $n Copy Locator(Images,node)

    # Update pipeline and GUI
    MainVolumesUpdate $v

    # If this Realtime volume is inside transforms, then
    # compute the registration:
    MainUpdateMRML

    # Perform realtime image processing
    foreach cb $Locator(callbackList) {
        $cb
    }
    
    # Render
    RenderAll

    # Loop
    # Call update instead of update idletasks so that we
    # process user input like changing slice orientation
    update
    if {[ValidateInt $Locator(Images,msPoll)] == 0} {
        set Locator(Images,msPoll) 100
    }
    after $Locator(Images,msPoll) LocatorLoopImages
}


#-------------------------------------------------------------------------------
# .PROC LocatorReorientRealtimeVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorReorientRealtimeVolume {} {
    global Slice Volume Locator

    if {$Locator(idRealtime) == "NEW"} {
        return
    }
 
    set i $Locator(idRealtime)
    set n Volume($i,node)

    Volume($i,node) SetScanOrder $Locator(realtimeScanOrder) 
    Volume($i,node) ComputeRasToIjkFromScanOrder [Volume($i,node) GetScanOrder] 

    # To keep variables 'RangeLow' and 'RangeHigh' as float
    # in vtkMrmlDataVolume for float volume, use this function:
    Volume($i,vol) SetRangeAuto 0

    # set the lower threshold to the actLow
    Volume($i,node) AutoThresholdOff
    Volume($i,node) ApplyThresholdOn
    Volume($i,node) SetLowerThreshold 1 

    MainSlicesSetVolumeAll Back $i
    MainVolumesSetActive $i

    MainUpdateMRML
    RenderAll
}

 
#-------------------------------------------------------------------------------
# .PROC LocatorLoopFlashpoint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopFlashpoint {} {
    global Slice Volume Locator
    
    if {$Locator(loop) == 0} {
        return
    }
    if {$Locator(pause) == 1} {
        return
    }

    set status [Locator(Flashpoint,src) PollRealtime]

    if {$status == -1} {
        puts "ERROR: PollRealtime"
        LocatorConnect 0
        return
    }

    # Locator(Flashpoint,src) Modified
    Locator(Flashpoint,src) Update
    set newImage   [Locator(Flashpoint,src) GetNewImage]
    set newLocator [Locator(Flashpoint,src) GetNewLocator]

    #----------------    
    # NEW LOCATOR
    #----------------
    if {$newLocator != 0} {
        set locStatus [Locator(Flashpoint,src) GetLocatorStatus]
        set locMatrix [Locator(Flashpoint,src) GetLocatorMatrix]
            
        # Report status to user
        if {$locStatus == 0} {
            set locText "OK"
            set Locator(bellCount) 0
        } else {
            set locText "BLOCKED"
            set rem [expr $Locator(bellCount) % 50]
            if {$rem == 0} {
                # Every 5 seconds ring the bell if locator is not available
                bell
            }

            incr Locator(bellCount)
        }
        $Locator(lLocStatus) config -text $locText

        # Read matrix
        set Locator(px) [$locMatrix GetElement 0 0]
        set Locator(py) [$locMatrix GetElement 1 0]
        set Locator(pz) [$locMatrix GetElement 2 0]
        set Locator(nx) [$locMatrix GetElement 0 1]
        set Locator(ny) [$locMatrix GetElement 1 1]
        set Locator(nz) [$locMatrix GetElement 2 1]
        set Locator(tx) [$locMatrix GetElement 0 2]
        set Locator(ty) [$locMatrix GetElement 1 2]
        set Locator(tz) [$locMatrix GetElement 2 2]

#        puts "NEW LOC: P=$Locator(px) $Locator(py) $Locator(pz)"
        LocatorUseLocatorMatrix
    }

    #----------------    
    # NEW IMAGE
    #----------------
    if {$newImage != 0} {
        # 1: Axial
        # 2: Sagittal
        # 3: Coronal
        set scanOrder "SI"
        switch $newImage {
            1 {set scanOrder "SI"} 
            2 {set scanOrder "RL"}
            3 {set scanOrder "AP"}
        }

       
        # Force an update so I can read the new matrix
        Locator(Flashpoint,src) Modified
        Locator(Flashpoint,src) Update

        # Update table and patient position
        set Locator(tblPos)   [lindex $Locator(tblPosList) \
            [Locator(Flashpoint,src) GetTablePosition]]
        set Locator(patEntry) [lindex $Locator(patEntryList) \
            [Locator(Flashpoint,src) GetPatientEntry]]
        set Locator(patPos)   [lindex $Locator(patPosList) \
            [Locator(Flashpoint,src) GetPatientPosition]]


        set newRSA [LocatorXYZToRSA]
        # newRSA holds the r,s and a values the new realtime scan
        # should display in the slicer
        if {$Locator(realtimeRSA) != $newRSA || $Locator(realtimeScanOrder) != $scanOrder} {
            set Locator(realtimeRSA) $newRSA 
            set Locator(realtimeScanOrder) $scanOrder 

            # Get other header values
            set Locator(recon)    [Locator(Flashpoint,src) GetRecon]
            set Locator(imageNum) [Locator(Flashpoint,src) GetImageNum]
            set minVal [Locator(Flashpoint,src) GetMinValue]
            set maxVal [Locator(Flashpoint,src) GetMaxValue]
            set imgMatrix [Locator(Flashpoint,src) GetImageMatrix]
            puts mat=$imgMatrix
            puts "ima=$Locator(imageNum), recon=$Locator(recon), range=$minVal $maxVal"

            # Copy the image to the Realtime volume
            set rImage [Locator(Flashpoint,src) GetOutput]

            set i [LocatorGetRealtimeID]
            set n Volume($i,node)

            # Volume($i,node) SetSpacing [$rImage GetSpacing]
            Volume($i,node) SetScanOrder $scanOrder 

            Volume($i,node) SetNumScalars [$rImage GetNumberOfScalarComponents]
            set ext [$rImage GetWholeExtent]
            Volume($i,node) SetImageRange [expr 1 + [lindex $ext 4]] [expr 1 + [lindex $ext 5]]
            Volume($i,node) SetScalarType [$rImage GetScalarType]
            Volume($i,node) SetDimensions [lindex [$rImage GetDimensions] 0] [lindex [$rImage GetDimensions] 1]
            Volume($i,node) ComputeRasToIjkFromScanOrder [Volume($i,node) GetScanOrder] 

            Volume($i,vol) SetImageData $rImage
            Volume($i,vol) SetRangeLow $minVal 
            Volume($i,vol) SetRangeHigh $maxVal 

            # To keep variables 'RangeLow' and 'RangeHigh' as float
            # in vtkMrmlDataVolume for float volume, use this function:
            Volume($i,vol) SetRangeAuto 0

            # set the lower threshold to the actLow
            Volume($i,node) AutoThresholdOff
            Volume($i,node) ApplyThresholdOn
            Volume($i,node) SetLowerThreshold $minVal
            Volume($i,node) SetUpperThreshold $maxVal

            LocatorTranslateRealtimeVolume

            MainSlicesSetVolumeAll Back $i
            MainVolumesSetActive $i
            MainUpdateMRML
            RenderAll

            # Perform realtime image processing
            foreach cb $Locator(callbackList) {
                set cb [string trim $cb]
                if {$cb != ""} {$cb}
            }

        }
    }

    # Render the slices that the locator is driving.
    if {$newImage != 0 || $newLocator != 0} {
        RenderAll
    }

    # Call update instead of update idletasks so that we
    # process user input like changing slice orientation
    update
    if {[ValidateInt $Locator(Flashpoint,msPoll)] == 0} {
        set Locator(Flashpoint,msPoll) 100
    }
    after $Locator(Flashpoint,msPoll) LocatorLoopFlashpoint
}


proc LocatorXYZToRSA {} {
    global Locator

    # patient postition: 
    # 0 = head first, supine
    # 1 = head first, prone
    # 2 = head first, left decub
    # 3 = head first, right decub
    # 4 = feet first, supine
    # 5 = feet first, prone
    # 6 = feet first, left decub
    # 7 = feet first, right decub
    set patpos 4 
    if {$Locator(patEntry) == "Head-first"} {
        if {$Locator(patPos) == "Supine"}      {set patpos 0}
        if {$Locator(patPos) == "Prone"}       {set patpos 1}
        if {$Locator(patPos) == "Left-decub"}  {set patpos 2}
        if {$Locator(patPos) == "Right-decub"} {set patpos 3}
    } else {
        if {$Locator(patPos) == "Supine"}      {set patpos 4}
        if {$Locator(patPos) == "Prone"}       {set patpos 5}
        if {$Locator(patPos) == "Left-decub"}  {set patpos 6}
        if {$Locator(patPos) == "Right-decub"} {set patpos 7}
    }

    # table position:
    # 0 = axial
    # 1 = side
    # 2 = vertical
    set tblpos -1
    if {$Locator(tblPos) == "Front"} {set tblpos 0}
    if {$Locator(tblPos) == "Side"} {set tblpos 1}
    if {$Locator(tblPos) == "Vertical"} {set tblpos 2}

    # The scanning location of the realtime image in magnet coordinate system
    set xyz [Locator(Flashpoint,src) GetRealtimeScanningLocation]

    set x [lindex $xyz 0]
    set x [expr round($x)]
    set y [lindex $xyz 1]
    set y [expr round($y)]
    set z [lindex $xyz 2]
    set z [expr round($z)]
 
    if {$tblpos == 0} {
        switch $patpos {
            0 {set r $x; set a $y; set s $z}
            1 {set r [expr -$x]; set a [expr -$y]; set s $z}
            2 {set a [expr -$x]; set r $y; set s $z}
            3 {set a $x; set r [expr -$y]; set s $z}
            4 {set r [expr -$x]; set a $y; set s [expr -$z]}
            5 {set r $x; set a [expr -$y]; set s [expr -$z]}
            6 {set a $x; set r $y; set s [expr -$z]}
            7 {set a [expr -$x]; set r [expr -$y]; set s [expr -$z]}
        } 
        return "$r $s $a"
    }

    if {$tblpos == 1} {
        switch $patpos {
            0 {set s $x; set a $y; set r [expr -$z]}
            1 {set s $x; set a [expr -$y]; set r $z}
            2 {set s $x; set r $y; set a $z}
            3 {set s $x; set r [expr -$y]; set a [expr -$z]}
            4 {set s [expr -$x]; set a $y; set r $z}
            5 {set s [expr -$x]; set a [expr -$y]; set r [expr -$z]}
            6 {set s [expr -$x]; set r $y; set a [expr -$z]}
            7 {set s [expr -$x]; set r [expr -$y]; set a $z}
        }
        return "$r $s $a"
    }
}


proc LocatorTranslateRealtimeVolume {} {
    global Locator


    # realtime volume id
    set volid $Locator(idRealtime)
 
    # calculate the space directions and origin
    catch "ras_matrix Delete"
    vtkMatrix4x4 ras_matrix
    eval ras_matrix DeepCopy [Volume($volid,node) GetRasToIjkMatrix]
    ras_matrix Invert
    set space_origin [format "(%g, %g, %g)" \
        [ras_matrix GetElement 0 3]\
        [ras_matrix GetElement 1 3]\
        [ras_matrix GetElement 2 3] ]
    set space_directions [format "(%g, %g, %g) (%g, %g, %g) (%g, %g, %g)" \
        [ras_matrix GetElement 0 0]\
        [ras_matrix GetElement 1 0]\
        [ras_matrix GetElement 2 0]\
        [ras_matrix GetElement 0 1]\
        [ras_matrix GetElement 1 1]\
        [ras_matrix GetElement 2 1]\
        [ras_matrix GetElement 0 2]\
        [ras_matrix GetElement 1 2]\
        [ras_matrix GetElement 2 2] ]
    ras_matrix Delete


    LocatorParseSpaceDirections $volid $space_origin $space_directions
} 



#
# convert nrrd-style space directions line into vtk/slicer info
# - unfortunately, this is some nasty math to do in tcl
#
proc LocatorParseSpaceDirections {volid space_origin space_directions} {
    global Locator Volume

    #
    # parse the 'space directions' and 'space origin' information into
    # a slicer RasToIjk and related matrices by telling the mrml node
    # the RAS corners of the volume
    #

    regsub -all "\\(" $space_origin " " space_origin
    regsub -all "\\)" $space_origin " " space_origin
    regsub -all "\\," $space_origin " " space_origin
    regsub -all "\\(" $space_directions " " space_directions
    regsub -all "\\)" $space_directions " " space_directions
    regsub -all "\\," $space_directions " " space_directions


# puts "space_origin $space_origin"
# puts "space_directions $space_directions"

    #
    # normalize and save length for each space direction vector
    #
    set spacei 0
    foreach dir [lrange $space_directions 0 2] {
        set spacei [expr $spacei + $dir * $dir]
    }
    set spacei [expr sqrt($spacei)]
    set unit_space_directions ""
    foreach dir [lrange $space_directions 0 2] {
        lappend unit_space_directions [expr $dir / $spacei]
    }

    set spacej 0
    foreach dir [lrange $space_directions 3 5] {
        set spacej [expr $spacej + $dir * $dir]
    }
    set spacej [expr sqrt($spacej)]
    foreach dir [lrange $space_directions 3 5] {
        lappend unit_space_directions [expr $dir / $spacej]
    }

    set spacek 0
    foreach dir [lrange $space_directions 6 8] {
        set spacek [expr $spacek + $dir * $dir]
    }
    set spacek [expr sqrt($spacek)]
    foreach dir [lrange $space_directions 6 8] {
        lappend unit_space_directions [expr $dir / $spacek]
    }
    
    Volume($volid,node) SetSpacing $spacei $spacej $spacek
    [Volume($volid,vol) GetOutput] SetSpacing $spacei $spacej $spacek


    #
    # fill the ijk to ras matrix
    # - use it to calculate the slicer internal matrices (RasToIjk etc)
    #

    set r [lindex $Locator(realtimeRSA) 0]
    set s [lindex $Locator(realtimeRSA) 1]
    set a [lindex $Locator(realtimeRSA) 2]
    set ras "$r $a $s"
 
    catch "Ijk_matrix Delete"
    vtkMatrix4x4 Ijk_matrix
    Ijk_matrix Identity
    for {set i 0} {$i < 3} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set val [lindex $space_directions [expr 3 * $i + $j]]
            Ijk_matrix SetElement $j $i $val
        }
        set val [expr [lindex $space_origin $i] + [lindex $ras $i]]
        Ijk_matrix SetElement $i 3 $val
    }

    set dims [[Volume($volid,vol) GetOutput] GetDimensions]

    VolumesComputeNodeMatricesFromIjkToRasMatrix2 $volid Ijk_matrix $dims

# puts [Ijk_matrix Print]

    Ijk_matrix Delete
}



#-------------------------------------------------------------------------------
# .PROC LocatorFilePrefix
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorFilePrefix {} {
    global Locator Mrml

    # Cannot have blank prefix
    set prefix $Locator(File,prefix)
    if {$prefix == ""} {
        set prefix loc
    }

     # Show popup initialized to the last file saved
    set filename [file join $Mrml(dir) $prefix]
    set dir [file dirname $filename]
    set typelist {
        {"TXT Files" {.txt}}
        {"All Files" {*}}
    }
    set filename [tk_getOpenFile -title "Open File" -defaultextension .txt \
        -filetypes $typelist -initialdir $dir -initialfile $filename]

    # Do nothing if the user cancelled
    if {$filename == ""} {return ""}

    # Remember to store it as a relative prefix for next time
    set Locator(File,prefix) [MainFileGetRelativePrefix $filename]
}

#-------------------------------------------------------------------------------
# .PROC LocatorImagesPrefix
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorImagesPrefix {} {
    global Locator Mrml

    # Cannot have blank prefix
    set prefix $Locator(Images,prefix)
    if {$prefix == ""} {
        set prefix I
    }

     # Show popup initialized to root plus any typed pathname
    set filename [file join $Mrml(dir) $prefix]
    set dir [file dirname $filename]
    set typelist {
        {"All Files" {*}}
    }
    set filename [tk_getOpenFile -title "First Image To Process" \
        -filetypes $typelist -initialdir "$dir" -initialfile $filename]

    # Do nothing if the user cancelled
    if {$filename == ""} {return}

    # Store first image file as a relative filename to the root (prefix.001)
    set Locator(Images,firstFile) [MainFileGetRelativePrefix $filename][file \
        extension $filename]

    # Remember to store it as a relative prefix for next time
    set Locator(Images,prefix) [MainFileGetRelativePrefix $filename]

    # Image numbers
    set Locator(Images,firstNum) [MainFileFindImageNumber First \
        [file join $Mrml(dir) $filename]]
    set Locator(Images,lastNum) [MainFileFindImageNumber Last \
        [file join $Mrml(dir) $filename]]
}

#-------------------------------------------------------------------------------
# .PROC LocatorStorePresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorStorePresets {p} {
    global Preset Locator Slice

    foreach s $Slice(idList) {
        set Preset(Locator,$p,$s,driver) $Locator($s,driver)
    }
    foreach k "visibility transverseVisibility guideVisibility normalLen transverseLen\
        radius diffuseColor" {
        set Preset(Locator,$p,$k) $Locator($k)
    }
}
        
proc LocatorRecallPresets {p} {
    global Preset Locator Slice

    foreach s $Slice(idList) {
        LocatorSetDriver $s $Preset(Locator,$p,$s,driver)
    }
    foreach k "visibility transverseVisibility guideVisibility normalLen transverseLen\
        radius diffuseColor" {
        set Locator($k) $Preset(Locator,$p,$k)
    }
    LocatorSetVisibility
    scan $Locator(diffuseColor) "%g %g %g" Locator(red) Locator(green) Locator(blue)
    LocatorSetColor
    LocatorSetSize
    LocatorSetMatrices
}


#-------------------------------------------------------------------------------
# .PROC LocatorCsysToggle
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorCsysToggle {} {
    if { $::Locator(csysVisible) } {
        LocatorCsysOn
    } else {
        LocatorCsysOff
    }
}

#-------------------------------------------------------------------------------
# .PROC LocatorCsysOn
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorCsysOn {} {
    global Locator Csys

    if { [info command Locator(csys,actor)] == "" } { 
        CsysCreate Locator csys -1 -1 -1
        set ::Module(Locator,procXformMotion) LocatorCsysCallback
    }

    set Csys(active) 1
    MainAddActor Locator(csys,actor)
    LocatorCsysCallback 
}

#-------------------------------------------------------------------------------
# .PROC LocatorCsysOff
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorCsysOff {} {
    global Locator Csys

    if { [info command Locator(csys,actor)] == "" } { 
        return
    }

    set Csys(active) 0
    MainRemoveActor Locator(csys,actor)
    Render3D
}

#-------------------------------------------------------------------------------
# .PROC LocatorCsysCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LocatorCsysCallback {args} {

    if { [info command Locator(csys,actor)] == "" } { 
        return
    }

    set mat [Locator(csys,actor) GetMatrix]
    set ::Locator(px) [$mat GetElement 0 3]
    set ::Locator(py) [$mat GetElement 1 3]
    set ::Locator(pz) [$mat GetElement 2 3]
    set ::Locator(nx) [$mat GetElement 0 0]
    set ::Locator(ny) [$mat GetElement 1 0]
    set ::Locator(nz) [$mat GetElement 2 0]
    set ::Locator(tx) [$mat GetElement 0 1]
    set ::Locator(ty) [$mat GetElement 1 1]
    set ::Locator(tz) [$mat GetElement 2 1]

    LocatorUseLocatorMatrix

    # Render the slices that the locator is driving
    foreach s $::Slice(idList) {
        if {[Slicer GetDriver $s] == 1} {
            RenderSlice $s
        }
    }
    Render3D
}



proc LocatorPushBindings {} {
   global Ev Csys

    EvActivateBindingSet LocatorSlice0Events
    EvActivateBindingSet LocatorSlice1Events
    EvActivateBindingSet LocatorSlice2Events
}



proc LocatorPopBindings {} {
    global Ev Csys

    EvDeactivateBindingSet LocatorSlice0Events
    EvDeactivateBindingSet LocatorSlice1Events
    EvDeactivateBindingSet LocatorSlice2Events
}



proc LocatorCreateBindings {} {
    global Gui Ev

    EvDeclareEventHandler LocatorSlicesEvents <1> \
        {set xc %x; set yc %y; LocatorProcessMouseEvent $xc $yc}

    EvAddWidgetToBindingSet LocatorSlice0Events $Gui(fSl0Win) {LocatorSlicesEvents}
    EvAddWidgetToBindingSet LocatorSlice1Events $Gui(fSl1Win) {LocatorSlicesEvents}
    EvAddWidgetToBindingSet LocatorSlice2Events $Gui(fSl2Win) {LocatorSlicesEvents}    
}



proc LocatorProcessMouseEvent {x y} {
    global Locator Interactor Anno


    if {$Locator(server) != "OpenTracker"} {
        # Only for the OpenTracker server are we interested in 
        # this mouse event.
        return 
    }

    # Which slice was picked?
    set s $Interactor(s)
    if {$s == ""} {
        DevErrorWindow "No slice was picked."
        return
    }

    # Get RAS coordinates
    set R [Anno($s,cur1,mapper) GetInput]
    set rl [split $R " "]
    set R [lindex $rl 1]

    set A [Anno($s,cur2,mapper) GetInput]
    set al [split $A " "]
    set A [lindex $al 1]

    set S [Anno($s,cur3,mapper) GetInput]
    set sl [split $S " "]
    set S [lindex $sl 1]

    # One point
    set Locator(entry,slicerCoords) "$R   $A   $S"
}



proc LocatorExit {} {

    # pop event bindings
    LocatorPopBindings
}



proc LocatorEnter {} {
    global Locator
   
    #--- push all event bindings onto the stack.
    LocatorPushBindings
}

