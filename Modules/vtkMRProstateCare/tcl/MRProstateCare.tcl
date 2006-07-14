#-------------------------------------------------------------------------------
# .PROC MRProstateCareInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareInit {} {
    global MRProstateCare Module Volume Model

    set m MRProstateCare

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is intended for MR-guided prostate biopsy."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Simon DiMaio, SPL/BWH, simond@bwh.harvard.edu
                           Steve Haker, SPL/BWH, haker@bwh.harvard.edu
                           Nobuhiko Hata, SPL/BWH, <hata@bwh.harvard.edu> 
                           Haiying Liu, SPL/BWH <hliu@bwh.harvard.edu>"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "Application"

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

    set Module($m,row1List) "Help Server Template Targets Navigation"
    set Module($m,row1Name) "{Help} {Server} {Template} {Targets} {Navigation}"
    set Module($m,row1,tab) Server 

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
    #   set Module($m,procVTK) MRProstateCareBuildVTK
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
    set Module($m,procGUI) MRProstateCareBuildGUI
    set Module($m,procVTK) MRProstateCareBuildVTK
    set Module($m,procEnter) MRProstateCareEnter
    set Module($m,procExit) MRProstateCareExit

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
        {$Revision: 1.1.2.2 $} {$Date: 2006/07/14 20:05:05 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set MRProstateCare(count) 0
    set MRProstateCare(Volume1) $Volume(idNone)
    set MRProstateCare(Model1)  $Model(idNone)
    set MRProstateCare(FileName)  ""

    set MRProstateCare(msPoll) 100
    set MRProstateCare(port)   15000
    set MRProstateCare(host)   mrtws
    set MRProstateCare(connect) 0
    set MRProstateCare(pause) 0
    set MRProstateCare(loop) 0

    # Patient/Table position
    set MRProstateCare(tblPosList)   "Front Side"
    set MRProstateCare(patEntryList) "Head-first Feet-first"
    set MRProstateCare(patPosList)   "Supine Prone Left-decub Right-decub"
    set MRProstateCare(tblPos)       [lindex $MRProstateCare(tblPosList) 0]
    set MRProstateCare(patEntry)     [lindex $MRProstateCare(patEntryList) 0]
    set MRProstateCare(patPos)       [lindex $MRProstateCare(patPosList) 0]


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


proc MRProstateCareBuildGUI {} {
    global Gui MRProstateCare Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(MRProstateCare,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Stuff
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
   #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The MRProstateCare module is an example for developers.  It shows how to add a module 
    to the Slicer.  The source code is in slicer2/Modules/vtkMRProstateCare/tcl/MRProstateCare.tcl.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Stuff:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags MRProstateCare $help
    MainHelpBuildGUI MRProstateCare

    #-------------------------------------------
    # Server frame
    #-------------------------------------------
    set fServer $Module(MRProstateCare,fServer)
    set f $fServer

    frame $f.fTop -bg $Gui(activeWorkspace) 
    pack $f.fTop -side top -pady 7 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top 

    set f $fServer.fTop
    eval {label $f.lConnectTitle -text "Connection status:"} $Gui(WTA)
    eval {label $f.lConnectStatus -text "None" -width 8} $Gui(WLA)
    grid $f.lConnectTitle $f.lConnectStatus -pady 1 -padx $Gui(pad)
    set MRProstateCare(connectStatus) $f.lConnectStatus

    set f $fServer.fMid
    foreach x "host port msPoll" text \
        "{Host name} {Port number} {Update period (ms)}" {

        DevAddLabel $f.l$x "${text}:" 
        eval {entry $f.e$x -textvariable MRProstateCare($x) -width 17} $Gui(WEA)
        grid $f.l$x $f.e$x -pady 3 -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    set f $fServer.fBot
    eval {checkbutton $f.cConnect \
        -text "Connect" -variable MRProstateCare(connect) -width 9 \
        -indicatoron 0 -command "MRProstateCareConnect"} $Gui(WCA)
    eval {checkbutton $f.cPause \
        -text "Pause" -variable MRProstateCare(pause) -command "MRProstateCarePause" \
        -width 9 -indicatoron 0} $Gui(WCA)
    pack $f.cConnect $f.cPause -side left -pady $Gui(pad) -padx 2


    #-------------------------------------------
    # Template frame
    #-------------------------------------------
    set fTemplate $Module(MRProstateCare,fTemplate)
    set f $fTemplate

    frame $f.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fTop -side top -pady 7 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 7

    set f $fTemplate.fTop
    set s Template 
    foreach x "PDate Name ID Step" text \
        "{Date} {Patient Name} {Patient ID} {Step}" {

       DevAddLabel $f.l$x "${text}:" 
       eval {entry $f.e$x -textvariable MRProstateCare($s,entry,$x) -width 23} $Gui(WEA)
 
       grid $f.l$x $f.e$x -pady 3 -padx 2 -sticky e
       grid $f.e$x -sticky w
    }
    set MRProstateCare($s,entry,$x) 5.0
    set var [clock format [clock seconds] -format "%D"]
    set MRProstateCare($s,entry,PDate) $var 
 
    set f $fTemplate.fMid
    DevAddLabel $f.lTitle "RSA coords of corner points:" 
    grid $f.lTitle -row 0 -column 0 -columnspan 3 -pady 10 -sticky news

    foreach x "AR PR PL AL" text \
        "{Anterior  Right} {Posterior Right} {Posterior Left} {Anterior Left}" {

       DevAddLabel $f.l$x "${text}:" 
       eval {entry $f.e$x -textvariable MRProstateCare($s,entry,$x) -width 15} $Gui(WEA)
       DevAddButton $f.b$x "Query" "MRProstateCareQuery$x" 5 
       set MRProstateCare($s,button,$x) $f.b$x
 
       grid $f.l$x $f.e$x $f.b$x -pady 3 -padx 2 -sticky e
       grid $f.e$x -sticky w

    }

    set f $fTemplate.fBot
    DevAddButton $f.bCheck "Check" "MRProstateCareAcceptTemplateCornerCoords" 10 
    pack $f.bCheck -side top


    #-------------------------------------------
    # Targets frame
    #-------------------------------------------
    set fTargets $Module(MRProstateCare,fTargets)
    set f $fTargets
    frame $f.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fTop -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 3 

    #-------------------------
    # Top frame
    #-------------------------
    set f $fTargets.fTop
    foreach x "1 2" {
        frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        pack $f.f$x -side top -pady 2 -padx 2 
    }

    set f $fTargets.fTop.f1
    DevAddLabel $f.lTitle "Targets/sextant biopsy positions:"

    set s Targets
    eval {label $f.ltitle -text "Name:"} $Gui(WLA)
    eval {entry $f.etitle -width 25 -textvariable MRProstateCare($s,entry,Name)} $Gui(WEA)
    eval {label $f.lonsets -text "Coords (RSA):"} $Gui(WLA)
    eval {entry $f.eonsets -width 25 -textvariable MRProstateCare($s,entry,Coords)} $Gui(WEA)
    DevAddButton $f.bOK "OK" "MRProstateCareAddOrEditTarget" 8 

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 7 -fill x -cspan 2 \
        1,0 $f.ltitle -padx 2 -pady 1 -anchor e \
        1,1 $f.etitle -fill x -padx 2 -pady 1 -anchor w \
        2,0 $f.lonsets -padx 2 -pady 1 -anchor e \
        2,1 $f.eonsets -padx 2 -pady 1 -anchor w \
        3,1 $f.bOK -padx 2 -pady 3 -anchor w

    set f $fTargets.fTop.f2
    foreach x "Up Down" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top 
    }

    set f $fTargets.fTop.f2.fUp
    DevAddLabel $f.lTitle "Defined targets/positions:"
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set MRProstateCare(targetsVerScroll) $f.vs
    listbox $f.lb -height 5 -width 24 -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::MRProstateCare(targetsVerScroll) set}
    set MRProstateCare(targetsListBox) $f.lb
    $MRProstateCare(targetsVerScroll) configure -command {$MRProstateCare(targetsListBox) yview}

    blt::table $f \
        0,0 $f.lTitle -padx 10 -pady 7 \
        1,0 $MRProstateCare(targetsListBox) -padx 2 -pady 1 -fill x \
        1,1 $MRProstateCare(targetsVerScroll) -fill y -padx 2 -pady 1

    set f $fTargets.fTop.f2.fDown
    DevAddButton $f.bDelete "Delete" "MRProstateCareDeleteTarget" 8 
    DevAddButton $f.bEdit "Edit" "MRProstateCareShowTargetToEdit" 8 
    grid $f.bEdit $f.bDelete -padx 1 -pady 2

    #-------------------------
    # Bottom frame
    #-------------------------
    set f $fTargets.fBot
    DevAddButton $f.bSave "Save" "MRProstateCareSaveUserInput"  10 
    DevAddButton $f.bRegister "Register" "MRProstateCareRegisterTemplate"  10 
    DevAddButton $f.bView "View" "MRProstateCareViewResult"  10 
    grid $f.bSave $f.bRegister $f.bView -padx 1 -pady 5 

    #-------------------------------------------
    # Navigation frame
    #-------------------------------------------
    set fNav $Module(MRProstateCare,fNavigation)
    set f $fNav

    #--- create blt notebook
    blt::tabset $f.tsNotebook -relief flat -borderwidth 0
    pack $f.tsNotebook -side top

    #--- notebook configure
    $f.tsNotebook configure -width 250
    $f.tsNotebook configure -height 310 
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
    foreach t "Level1 Level2 Level3" {
        $f.tsNotebook insert $i $t
        frame $f.tsNotebook.f$t -bg $Gui(activeWorkspace) -bd 2 
        MRProstateCareBuildGUIFor${t} $f.tsNotebook.f$t

        $f.tsNotebook tab configure $t -window $f.tsNotebook.f$t 
        $f.tsNotebook tab configure $t -activebackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -selectbackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -background $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -fill both -padx $::Gui(pad) -pady 1 

        incr i
    }
}


proc MRProstateCareBuildGUIForLevel3 {parent} {
    global MRProstateCare Gui 

    set f $parent
    frame $f.fTop -bg $Gui(activeWorkspace)
    pack $f.fTop -side top -pady 3 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 3 

    #-------------------------
    # Top frame
    #-------------------------
    set f $parent.fTop
 
    # Build pulldown menu for all targets 
    DevAddLabel $f.lTarget "Current target:"

    set tList [list {none}]
    set df [lindex $tList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 18 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateTargets"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectTarget $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,targetButton) $f.mbType
    set MRProstateCare(gui,targetMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTarget -padx 2 -pady 2 -anchor e \
        0,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w


    #-------------------------
    # Mid frame
    #-------------------------
    set f $parent.fMid

    DevAddLabel $f.lTitle "Displayed images:"
    DevAddLabel $f.lImage1Label "Image 1:"
    DevAddLabel $f.lImage1Value "Realtime"

    # Build pulldown menu for volumes 
    DevAddLabel $f.lVolume "Image 2:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateVolumes"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectVolume $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,volumeButton) $f.mbType
    set MRProstateCare(gui,volumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 2 -cspan 2 \
        1,0 $f.lImage1Label -padx 2 -pady 2 -anchor e \
        1,1 $f.lImage1Value -fill x -padx 2 -pady 2 -anchor w \
        2,0 $f.lVolume -padx 2 -pady 2 -anchor e \
        2,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w

 
    #-------------------------
    # Bot frame
    #-------------------------
    set f $parent.fBot
    DevAddButton $f.bStart "Start" "MRProstateCareStartNav"  10 
    DevAddButton $f.bStop "Stop" "MRProstateCareStopNav"  10 
    grid $f.bStart $f.bStop -padx 1 -pady 5 
}



proc MRProstateCareBuildGUIForLevel2 {parent} {
    global MRProstateCare Gui 

    set f $parent
    frame $f.fTop -bg $Gui(activeWorkspace)
    pack $f.fTop -side top -pady 3 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 3 

    #-------------------------
    # Top frame
    #-------------------------
    set f $parent.fTop
 
    # Build pulldown menu for all targets 
    DevAddLabel $f.lTarget "Current target:"

    set tList [list {none}]
    set df [lindex $tList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 18 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateTargets"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectTarget $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,targetButton) $f.mbType
    set MRProstateCare(gui,targetMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTarget -padx 2 -pady 2 -anchor e \
        0,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w


    #-------------------------
    # Mid frame
    #-------------------------
    set f $parent.fMid

    DevAddLabel $f.lTitle "Displayed images:"
    DevAddLabel $f.lImage1Label "Image 1:"
    DevAddLabel $f.lImage1Value "realtime"

    # Build pulldown menu for volumes 
    DevAddLabel $f.lVolume "Image 2:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateVolumes"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectVolume $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,volumeButton) $f.mbType
    set MRProstateCare(gui,volumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 2 -cspan 2 \
        1,0 $f.lImage1Label -padx 2 -pady 2 -anchor e \
        1,1 $f.lImage1Value -fill x -padx 2 -pady 2 -anchor w \
        2,0 $f.lVolume -padx 2 -pady 2 -anchor e \
        2,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w

 
    #-------------------------
    # Bot frame
    #-------------------------
    set f $parent.fBot
    DevAddButton $f.bStart "Start" "MRProstateCareStartNav"  10 
    DevAddButton $f.bStop "Stop" "MRProstateCareStopNav"  10 
    grid $f.bStart $f.bStop -padx 1 -pady 5 
}



proc MRProstateCareBuildGUIForLevel1 {parent} {
    global MRProstateCare Gui 

    set f $parent
    frame $f.fTop -bg $Gui(activeWorkspace)
    pack $f.fTop -side top -pady 3 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 3 

    #-------------------------
    # Top frame
    #-------------------------
    set f $parent.fTop
 
    # Build pulldown menu for all targets 
    DevAddLabel $f.lTarget "Current target:"

    set tList [list {none}]
    set df [lindex $tList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 18 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateTargets"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectTarget $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,targetButton) $f.mbType
    set MRProstateCare(gui,targetMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTarget -padx 2 -pady 2 -anchor e \
        0,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w


    #-------------------------
    # Mid frame
    #-------------------------
    set f $parent.fMid

    DevAddLabel $f.lTitle "Displayed images:"
    DevAddLabel $f.lImage1Label "Image 1:"
    DevAddLabel $f.lImage1Value "realtime"

    # Build pulldown menu for volumes 
    DevAddLabel $f.lVolume "Image 2:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 20 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    bind $f.mbType <1> "MRProstateCareUpdateVolumes"
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectVolume $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,volumeButton) $f.mbType
    set MRProstateCare(gui,volumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 2 -cspan 2 \
        1,0 $f.lImage1Label -padx 2 -pady 2 -anchor e \
        1,1 $f.lImage1Value -fill x -padx 2 -pady 2 -anchor w \
        2,0 $f.lVolume -padx 2 -pady 2 -anchor e \
        2,1 $f.mbType -fill x -padx 2 -pady 2 -anchor w

 
    #-------------------------
    # Bot frame
    #-------------------------
    set f $parent.fBot
    DevAddButton $f.bStart "Start" "MRProstateCareStartNav"  10 
    DevAddButton $f.bStop "Stop" "MRProstateCareStopNav"  10 
    grid $f.bStart $f.bStop -padx 1 -pady 5 
}


proc MRProstateCareUpdateVolumes {} {
    global MRProstateCare 

}


proc MRProstateCareSelectVolume {m} {
    global MRProstateCare 

}


proc MRProstateCareUpdateTargets {} {
    global MRProstateCare 

}


proc MRProstateCareSelectTarget {m} {
    global MRProstateCare 

}

proc MRProstatCareRegisterTemplate {} {
    global MRProstateCare 

}


proc MRProstateCareSaveUserInput {} {
    global MRProstateCare 

    set fileType {{"Text" *.txt}}
    set fileName [tk_getSaveFile -filetypes $fileType -parent .]

    if {[string length $fileName]} {
        # write data into a file
    }
}


proc MRProstateCareShowTargetToEdit {} {

}


proc MRProstateCareDeleteTarget {} {

}


proc MRProstateCareAddOrEditTarget {} {

}


proc MRProstateCareAcceptTemplateCornerCoords {} {

}


proc MRProstateCareLoop {} {
    global Slice Volume MRProstateCare
    
    if {$MRProstateCare(loop) == 0} {
        return
    }
    if {$MRProstateCare(pause) == 1} {
        return
    }

    set status [MRProstateCare(src) PollRealtime]

    if {$status == -1} {
        puts "ERROR: PollRealtime"
        MRProstateCareConnect 0
        return
    }

    set newImage   [MRProstateCare(src) GetNewImage]
    set newLocator [MRProstateCare(src) GetNewLocator]

    #----------------    
    # NEW locator 
    #----------------
    if {$newLocator != 0} {
        set locStatus [MRProstateCare(src) GetLocatorStatus]
        set locMatrix [MRProstateCare(src) GetLocatorMatrix]
            
if {0} {
        # Report status to user
        if {$locStatus == 0} {
            set locText "OK"
            set MRProstateCare(bellCount) 0
        } else {
            set locText "BLOCKED"
            set rem [expr $MRProstateCare(bellCount) % 50]
            if {$rem == 0} {
                # Every 5 seconds ring the bell if MRProstateCare is not available
                bell
            }

            incr MRProstateCare(bellCount)
        }
        $MRProstateCare(lLocStatus) config -text $locText
}

        # Read matrix
        set MRProstateCare(px) [$locMatrix GetElement 0 0]
        set MRProstateCare(py) [$locMatrix GetElement 1 0]
        set MRProstateCare(pz) [$locMatrix GetElement 2 0]
        set MRProstateCare(nx) [$locMatrix GetElement 0 1]
        set MRProstateCare(ny) [$locMatrix GetElement 1 1]
        set MRProstateCare(nz) [$locMatrix GetElement 2 1]
        set MRProstateCare(tx) [$locMatrix GetElement 0 2]
        set MRProstateCare(ty) [$locMatrix GetElement 1 2]
        set MRProstateCare(tz) [$locMatrix GetElement 2 2]

        # display the new locator; we don't display locator
        # for prostate care
        # MRProstateCareUseLocatorMatrix
    }

    #----------------    
    # NEW IMAGE
    #----------------
    if {$newImage != 0} {
        
        # Force an update so I can read the new matrix
        MRProstateCare(src) Modified
        MRProstateCare(src) Update
    
        # Update patient position
        set MRProstateCare(tblPos)   [lindex $MRProstateCare(tblPosList) \
            [MRProstateCare(src) GetTablePosition]]
        set MRProstateCare(patEntry) [lindex $MRProstateCare(patEntryList) \
            [MRProstateCare(src) GetPatientEntry]]
        set MRProstateCare(patPos)   [lindex $MRProstateCare(patPosList) \
            [MRProstateCare(Flashpoint,src) GetPatientPosition]]

        # MRProstateCareSetPatientPosition

        # Get other header values
        set MRProstateCare(recon)    [MRProstateCare(src) GetRecon]
        set MRProstateCare(imageNum) [MRProstateCare(src) GetImageNum]
        set minVal [MRProstateCare(src) GetMinValue]
        set maxVal [MRProstateCare(src) GetMaxValue]
        set imgMatrix [MRProstateCare(src) GetImageMatrix]
        puts "imgNo = $MRProstateCare(imageNum), recon = $MRProstateCare(recon), range = $minVal $maxVal"

        # Copy the image to the Realtime volume
        set v [MRProstateCareGetRealtimeID]
        vtkImageCopy copy
        copy SetInput [MRProstateCare(src) GetOutput]
        copy Update
        copy SetInput ""
        Volume($v,vol) SetImageData [copy GetOutput]
        copy SetOutput ""
        copy Delete

        # Set the header info
        set n Volume($v,node)
        $n SetImageRange $MRProstateCare(imageNum) $MRProstateCare(imageNum)
        $n SetDescription "recon=$MRProstateCare(recon)"
        set str [$n GetMatrixToString $imgMatrix]
        $n SetRasToVtkMatrix $str
        $n UseRasToVtkMatrixOn

        # Update pipeline and GUI
        MainVolumesUpdate $v

        # If this Realtime volume is inside transforms, then
        # compute the registration:
        MainUpdateMRML

        # Perform realtime image processing
        # foreach cb $MRProstateCare(callbackList) {
        #    $cb
        # }
    }

    # Render the slices that the MRProstateCare is driving.
    if {$newImage != 0 || $newLocator != 0} {
        RenderAll
    }

    # Call update instead of update idletasks so that we
    # process user input like changing slice orientation
    update
    if {[ValidateInt $MRProstateCare(msPoll)] == 0} {
        set MRProstateCare(msPoll) 100
    }
    after $MRProstateCare(msPoll) MRProstateCareLoop
}


proc MRProstateCareSetRealtime {v} {
    global MRProstateCare Volume

    set MRProstateCare(idRealtime) $v
    
    # Change button text, and show file prefix
if {0} {
    if {$v == "NEW"} {
        $MRProstateCare(mbRealtime) config -text $v
        set MRProstateCare(prefixRealtime) ""
    } else {
        $MRProstateCare(mbRealtime) config -text [Volume($v,node) GetName]
        set MRProstateCare(prefixRealtime) [MainFileGetRelativePrefix \
            [Volume($v,node) GetFilePrefix]]
    }
} 
}

proc MRProstateCareGetRealtimeID {} {
    global MRProstateCare Volume Lut
        
    # If there is no Realtime volume, then create one
    if {$MRProstateCare(idRealtime) != "NEW"} {
        return $MRProstateCare(idRealtime)
    }
    
    # Create the node
    set n [MainMrmlAddNode Volume]
    set v [$n GetID]
    $n SetDescription "Realtime Volume"
    $n SetName        "Realtime"

    # Create the volume
    MainVolumesCreate $v

    MRProstateCareSetRealtime $v

    MainUpdateMRML

    return $v
}


proc MRProstateCareUseLocatorMatrix {} {
    global MRProstateCare Slice

    foreach p "normalOffset transverseOffset crossOffset" {
        if {[ValidateFloat $MRProstateCare($p)] == 0} {
            tk_messageBox -message "$p must be a floating point number."
            return
        }
    }

    # Form arrays so we can use vector processing functions
    set P(x) $MRProstateCare(px)
    set P(y) $MRProstateCare(py)
    set P(z) $MRProstateCare(pz)
    set N(x) $MRProstateCare(nx)
    set N(y) $MRProstateCare(ny)
    set N(z) $MRProstateCare(nz)
    set T(x) $MRProstateCare(tx)
    set T(y) $MRProstateCare(ty)
    set T(z) $MRProstateCare(tz)

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
    set n $MRProstateCare(normalOffset)
    set t $MRProstateCare(transverseOffset)
    set c $MRProstateCare(crossOffset)
    set MRProstateCare(px) [expr $P(x) + $N(x)*$n + $T(x)*$t + $C(x)*$c]
    set MRProstateCare(py) [expr $P(y) + $N(y)*$n + $T(y)*$t + $C(y)*$c]
    set MRProstateCare(pz) [expr $P(z) + $N(z)*$n + $T(z)*$t + $C(z)*$c]
    set MRProstateCare(nx) $N(x)
    set MRProstateCare(ny) $N(y)
    set MRProstateCare(nz) $N(z)
    set MRProstateCare(tx) $T(x)
    set MRProstateCare(ty) $T(y)
    set MRProstateCare(tz) $T(z)

    # Format display
    MRProstateCareFormat
                
    # Position the rendered locator
    MRProstateCareSetMatrices
            
    # Find slices with their input set to locator.
    # and set the slice matrix with the new locator data

    Slicer SetDirectNTP \
        $MRProstateCare(nx) $MRProstateCare(ny) $MRProstateCare(nz) \
        $MRProstateCare(tx) $MRProstateCare(ty) $MRProstateCare(tz) \
        $MRProstateCare(px) $MRProstateCare(py) $MRProstateCare(pz) 
}


proc MRProstateCareConnect {{value ""}} {
    global Gui MRProstateCare Mrml

    if {$value != ""} {
        set MRProstateCare(connect) $value
    }

    # CONNECT
    if {$MRProstateCare(connect) == "1"} {
            # You have to actually connect, dumbo
            set status [MRProstateCare(src) OpenConnection \
                $MRProstateCare(host) $MRProstateCare(port)]
            if {$status == -1} {
                tk_messageBox -icon error -type ok -title $Gui(title) \
                    -message "Make sure spl_server is running on the mrt workstation: host='$MRProstateCare(host)' port='$MRProstateCare(port)'"
                set MRProstateCare(loop) 0
                set MRProstateCare(connect) 0
                return
            }

            $MRProstateCare(connectStatus) config -text "OK" 

            set MRProstateCare(loop) 1
            MRProstateCareLoop
    } else {
        set MRProstateCare(loop) 0
        set MRProstateCare(imageNum) ""
        MRProstateCare(src) CloseConnection
        $MRProstateCare(connectStatus) config -text "None" 
    }

}


proc MRProstateCarePause {} {
    global MRProstateCare

    if {$MRProstateCare(pause) == 0 && $MRProstateCare(connect) == 1} {
        MRProstateCareLoop
    }

    $MRProstateCare(connectStatus) config -text "None" 
}


#-------------------------------------------------------------------------------
# .PROC MRProstateCareBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareBuildVTK {} {
    global Gui MRProstateCare View Slice Target Volume Lut 

    #------------------------#
    # Realtime image source
    #------------------------#
    set os -1
    switch $::tcl_platform(os) {
        "SunOS"  {set os 1}
        "Linux"  {set os 2}
        "Darwin" {set os 3}
        default  {set os 4}
    }
    vtkImageRealtimeScan MRProstateCare(src)
    MRProstateCare(src) SetOperatingSystem $os


}

#-------------------------------------------------------------------------------
# .PROC MRProstateCareEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc MRProstateCareEnter {} {
    global MRProstateCare
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    # pushEventManager $MRProstateCare(eventManager)

    # clear the text box and put instructions there
    # $MRProstateCare(textBox) delete 1.0 end
    # $MRProstateCare(textBox) insert end "Shift-Click anywhere!\n"

}


#-------------------------------------------------------------------------------
# .PROC MRProstateCareExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}
