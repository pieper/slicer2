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
    set Module($m,author) "Haiying Liu, SPL/BWH <hliu@bwh.harvard.edu>
                           Nobuhiko Hata, SPL/BWH, <hata@bwh.harvard.edu> 
                           Steve Haker, SPL/BWH, haker@bwh.harvard.edu
                           Simon DiMaio, SPL/BWH, simond@bwh.harvard.edu"

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

    set Module($m,row1List) "Help Server Template Points Navigation"
    set Module($m,row1Name) "{Help} {Server} {Template} {Points} {Navigation}"
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
        {$Revision: 1.1.2.60 $} {$Date: 2006/09/14 17:33:54 $}]

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

    set MRProstateCare(port)   60000
    set MRProstateCare(connect) 0
    set MRProstateCare(pause) 0
    set MRProstateCare(servLoop) 0
    set MRProstateCare(logTime) 10000

    # Patient/Table position
    set MRProstateCare(tblPosList)   "Front Side"
    set MRProstateCare(patEntryList) "Head-first Feet-first"
    set MRProstateCare(patPosList)   "Supine Prone Left-decub Right-decub"
    set MRProstateCare(tblPos)       [lindex $MRProstateCare(tblPosList) 0]
    set MRProstateCare(patEntry)     [lindex $MRProstateCare(patEntryList) 0]
    set MRProstateCare(patPos)       [lindex $MRProstateCare(patPosList) 0]

    set MRProstateCare(currentTab) 1 
    set MRProstateCare(editIndex) -1 
    set MRProstateCare(editMode)   0 
    set MRProstateCare(pointList) "" 

    switch $::tcl_platform(os) {
        "SunOS" -
        "Darwin" -
        "Linux" {
            set MRProstateCare(tempDir) "/tmp"
        }
        default {
            set MRProstateCare(tempDir) ""
        }
    }
    set MRProstateCare(logFile) "MRProstateCareLog.txt"
    set MRProstateCare(navLoop) 0 
    set MRProstateCare(navTime) 250
    set MRProstateCare(image1,currentVolumeID) 0 
    set MRProstateCare(image2,currentVolumeID) 0 

    set MRProstateCare(portSet) 0
    set MRProstateCare(scaleFactor) 1 
    set MRProstateCare(currentPoint) none 
    set MRProstateCare(lastRealtimeScanOrient) "Coronal" 

    set MRProstateCare(realtimeImageID) 33000 
    set MRProstateCare(displayRSA) "0 0 0"

    # Creates bindings
    MRProstateCareCreateBindings 

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
    global Gui MRProstateCare Module Volume Model Locator
    
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
    The MRProstateCare module is developed to guide the medical procedure of prostate biopsy in the open magnet enviroment. 
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags MRProstateCare $help
    MainHelpBuildGUI MRProstateCare

    set b $Module(MRProstateCare,bHelp)
    bind $b <1> "MRProstateCareSetCurrentTab 0" 
 
    #-------------------------------------------
    # Server frame
    #-------------------------------------------
    set b $Module(MRProstateCare,bServer)
    bind $b <1> "MRProstateCareSetCurrentTab 1" 
    set fServer $Module(MRProstateCare,fServer)
    set f $fServer

    frame $f.fTop -bg $Gui(activeWorkspace) 
    pack $f.fTop -side top -pady 7 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fMid -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top 

    set f $fServer.fTop
    eval {label $f.lConnectTitle -text "Connection to server:"} $Gui(WTA)
    eval {label $f.lConnectStatus -text "none" -width 8} $Gui(WLA)
    grid $f.lConnectTitle $f.lConnectStatus -pady 1 -padx $Gui(pad)
    set MRProstateCare(connectStatus) $f.lConnectStatus

    set f $fServer.fMid
    foreach x "host port msPoll" text \
        "{Host name} {Port number} {Update period (ms)}" {

        DevAddLabel $f.l$x "${text}:" 
        eval {entry $f.e$x -textvariable Locator(Flashpoint,$x) -width 17} $Gui(WEA)
        grid $f.l$x $f.e$x -pady 3 -padx $Gui(pad) -sticky e
        grid $f.e$x -sticky w
    }

    set f $fServer.fBot
    DevAddButton $f.bLoad "Load" "MRProstateCareLoad"  9 
    eval {checkbutton $f.cConnect \
        -text "Connect" -variable Locator(connect) -width 9 \
        -indicatoron 0 -command "MRProstateCareConnect"} $Gui(WCA)
    eval {checkbutton $f.cPause \
        -text "Pause" -variable Locator(pause) -command "LocatorPause" \
        -width 9 -indicatoron 0} $Gui(WCA)
    grid $f.bLoad $f.cConnect $f.cPause -pady 5 -padx 1 

    #-------------------------------------------
    # Template frame
    #-------------------------------------------
    set b $Module(MRProstateCare,bTemplate)
    bind $b <1> "MRProstateCareSetCurrentTab 2" 
    set fTemplate $Module(MRProstateCare,fTemplate)
    set f $fTemplate

    foreach x "1 2 3 4 5" {
        if {$x == 1 || $x == 5} {
            frame $f.f$x -bg $Gui(activeWorkspace)
        } else {
            frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        }
        pack $f.f$x -side top -pady 1 -fill x 
    }

    set f $fTemplate.f1
    eval {label $f.lLocatorTitle -text "Locator status:"} $Gui(WTA)
    eval {label $f.lLocatorStatus -text "none" -width 8} $Gui(WLA)
    grid $f.lLocatorTitle $f.lLocatorStatus -pady 1 -padx $Gui(pad)
    set Locator(lLocStatus) $f.lLocatorStatus

    set f $fTemplate.f2
    foreach x "PDate PName PID Step" text \
        "{Date} {Patient Name} {Patient ID} {Step}" {

       DevAddLabel $f.l$x "${text}:" 
       eval {entry $f.e$x -textvariable MRProstateCare(entry,$x) -width 23} $Gui(WEA)
 
       grid $f.l$x $f.e$x -pady 1 -padx 2 -sticky e
       grid $f.e$x -sticky w
    }
    set MRProstateCare(entry,$x) 5.0
    set var [clock format [clock seconds] -format "%D"]
    set MRProstateCare(entry,PDate) $var 
 

    set f $fTemplate.f3
    foreach x "top bot" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top -pady 1 
    }
 
    set f $fTemplate.f3.ftop
    eval {label $f.l -text "RSA coords of template corners:"} $Gui(WTA)
    grid $f.l -row 0 -column 0 -columnspan 3 -pady 5 -sticky news

    foreach x "AR PR PL AL" text \
        "{Anterior Right} {Posterior Right} {Posterior Left} {Anterior Left}" {

       DevAddLabel $f.l$x "${text}:" 
       eval {entry $f.e$x -textvariable MRProstateCare(entry,$x) -width 15} $Gui(WEA)
       DevAddButton $f.b$x "Query" "MRProstateCareQuery $x" 5 
       set MRProstateCare(button,$x) $f.b$x
 
       grid $f.l$x $f.e$x $f.b$x -pady 1 -padx 2 -sticky e
       grid $f.e$x -sticky w

    }
    set f $fTemplate.f3.fbot
    DevAddButton $f.bSave "Save" "MRProstateCareSaveCornerCoors" 8 
    DevAddButton $f.bReset "Reset" "MRProstateCareResetCornerCoors" 8 
    grid $f.bSave $f.bReset -pady 2 -sticky news
 

    set f $fTemplate.f4
    eval {label $f.l -text "Current locator position & orientation:"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 2 -padx $Gui(pad)

    set f $f.f
    eval {label $f.l -text ""} $Gui(WLA)
    foreach ax "x y z" text "R A S" {
        eval {label $f.l$ax -text $text -width 7} $Gui(WLA)
    }
    grid $f.l $f.lx $f.ly $f.lz -pady 1 -padx $Gui(pad) -sticky e

    foreach axis "N T P" var "n t p" {
        eval {label $f.l$axis -text "$axis:"} $Gui(WLA)
        foreach ax "x y z" text "R A S" {
            eval {entry $f.e$axis$ax -justify right -width 7 \
                -textvariable Locator($var${ax}Str)} $Gui(WEA)
            bind $f.e$axis$ax <Return> "LocatorSetPosition; Render3D"
        }
        grid $f.l$axis $f.e${axis}x $f.e${axis}y $f.e${axis}z \
            -pady 1 -padx 6 -sticky e
    }

    set f $fTemplate.f5
    DevAddButton $f.bCheck "Check" "MRProstateCareCheckTemplateUserInput;\
                                    MRProstateCareVerify 0; \
                                    MRProstateCareView" 10 
    grid $f.bCheck -pady 2 -padx 1 


    #-------------------------------------------
    # Points frame
    #-------------------------------------------
    set b $Module(MRProstateCare,bPoints)
    bind $b <1> "MRProstateCareSetCurrentTab 3" 
    set fPoints $Module(MRProstateCare,fPoints)
    set f $fPoints
    frame $f.fTop -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.fTop -side top -pady 3 
    frame $f.fBot -bg $Gui(activeWorkspace)
    pack $f.fBot -side top -pady 3 

    #-------------------------
    # Top frame
    #-------------------------
    set f $fPoints.fTop
    foreach x "1 2 3" {
        frame $f.f$x -bg $Gui(activeWorkspace) -relief groove -bd 2 
        pack $f.f$x -side top -pady 2 -padx 2 -fill x 
    }

    set f $fPoints.fTop.f1
    foreach x "Target Sextant Penn" \
        text "{Add a target} {Add a sextant position} {Add an aux point}" {
        eval {radiobutton $f.r$x -width 30 -text $text \
            -variable MRProstateCare(category) -value $x \
            -relief raised -offrelief raised -overrelief raised \
            -command "" \
            -selectcolor white} $Gui(WEA)
        pack $f.r$x -side top -pady 1 
    } 
    $f.rTarget select
    $f.rTarget configure -state normal 

    set f $fPoints.fTop.f2
    eval {label $f.lTitle -text "Describe a point:"} $Gui(WTA)
 
    eval {label $f.ltitle -text "Title:"} $Gui(WLA)
    eval {entry $f.etitle -width 20 -textvariable MRProstateCare(entry,Title)} $Gui(WEA)
    eval {label $f.lonsets -text "Coords (RSA):"} $Gui(WLA)
    eval {entry $f.eonsets -width 20 -textvariable MRProstateCare(entry,Coords)} $Gui(WEA)
    DevAddButton $f.bOK "OK" "MRProstateCareAddOrEditPoint" 8 

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 7 -fill x -cspan 2 \
        1,0 $f.ltitle -padx 2 -pady 1 -anchor e \
        1,1 $f.etitle -fill x -padx 2 -pady 1 -anchor w \
        2,0 $f.lonsets -padx 2 -pady 1 -anchor e \
        2,1 $f.eonsets -padx 2 -pady 1 -anchor w \
        3,1 $f.bOK -padx 2 -pady 3 -anchor w

    set f $fPoints.fTop.f3
    foreach x "Up Down" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top 
    }

    set f $fPoints.fTop.f3.fUp
    eval {label $f.lTitle -text "Defined points:"} $Gui(WTA)
    scrollbar $f.vs -orient vertical -bg $Gui(activeWorkspace)
    set MRProstateCare(PointsVerScroll) $f.vs
    listbox $f.lb \
        -height 6 -width 24 \
        -bg $Gui(activeWorkspace) \
        -yscrollcommand {$::MRProstateCare(PointsVerScroll) set}
    set MRProstateCare(pointListBox) $f.lb
    $MRProstateCare(PointsVerScroll) configure -command {$MRProstateCare(pointListBox) yview}

    blt::table $f \
        0,0 $f.lTitle -padx 10 -pady 7 \
        1,0 $MRProstateCare(pointListBox) -padx 2 -pady 1 -fill x \
        1,1 $MRProstateCare(PointsVerScroll) -fill y -padx 2 -pady 1

    set f $fPoints.fTop.f3.fDown
    DevAddButton $f.bDelete "Delete" "MRProstateCareDeletePoint" 8 
    DevAddButton $f.bEdit "Edit" "MRProstateCareShowPointToEdit" 8 
    grid $f.bEdit $f.bDelete -padx 1 -pady 2

    #-------------------------
    # Bottom frame
    #-------------------------
    set f $fPoints.fBot
    DevAddButton $f.bView "View" "MRProstateCareCheckTemplateUserInput;\
                                  MRProstateCareVerify 1; \
                                  MRProstateCareView" 10 
 
    grid $f.bView -padx 1 -pady 3 

    #-------------------------------------------
    # Navigation frame
    #-------------------------------------------
    set b $Module(MRProstateCare,bNavigation)
    bind $b <1> "MRProstateCareSetCurrentTab 4" 
    set fNav $Module(MRProstateCare,fNavigation)
    set f $fNav

    #--- create blt notebook
    blt::tabset $f.tsNotebook -relief flat -borderwidth 0
    pack $f.tsNotebook -side top

    #--- notebook configure
    $f.tsNotebook configure -width 250
    $f.tsNotebook configure -height 370 
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
    foreach t "Display Scan" {
        $f.tsNotebook insert $i $t
        frame $f.tsNotebook.f$t -bg $Gui(activeWorkspace) -bd 2 
        MRProstateCareBuildGUIFor${t} $f.tsNotebook.f$t

        $f.tsNotebook tab configure $t -window $f.tsNotebook.f$t 
        $f.tsNotebook tab configure $t -activebackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -selectbackground $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -background $::Gui(activeWorkspace)
        $f.tsNotebook tab configure $t -fill both -padx 2 -pady 1 

        incr i
    }
}


proc MRProstateCareSaveCornerCoors {} {
    global MRProstateCare 

    foreach x "AR PR PL AL" {
        set MRProstateCare(saved$x) $MRProstateCare(entry,$x)
    }
}


proc MRProstateCareResetCornerCoors {} {
    global MRProstateCare

    foreach x "AR PR PL AL" {
        set MRProstateCare(entry,$x) $MRProstateCare(saved$x)
    }
}
 

proc MRProstateCareQuery {field} {
    global MRProstateCare Locator 

    LocatorFormat 
    set r $Locator(pxStr)
    set a $Locator(pyStr)
    set s $Locator(pzStr)

    set MRProstateCare(entry,$field) "$r $s $a"
}


proc MRProstateCareBuildGUIForScan {parent} {
    global MRProstateCare Gui 

    set f $parent
    frame $f.fT1 -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fT1 -side top -pady 3 -fill x 
    frame $f.fT2 -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $f.fT2 -side top -pady 3 -fill x 
    frame $f.fMid -bg $Gui(activeWorkspace) -relief groove -bd 2  
    pack $f.fMid -side top -pady 3 -fill x 
    frame $f.fBot -bg $Gui(activeWorkspace) 
    pack $f.fBot -side top -pady 3 -fill x 

    #-------------------------
    # Frame T1 
    #-------------------------

    set f $parent.fT1

    # Build pulldown menu for all Points 
    eval {label $f.lTitle -text "Select a point:"} $Gui(WTA)
 
    set tList [list {none}]
    set df [lindex $tList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 28 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectPoint $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,scanPointButton) $f.mbType
    set MRProstateCare(gui,scanPointMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 3 \
        1,0 $f.mbType -fill x -padx 3 -pady 2 


    #-------------------------
    # Frame T2 
    #-------------------------
 
    set f $parent.fT2

    eval {label $f.lTitle -text "Choose scan orientation:"} $Gui(WTA)
 
    foreach x "Axial Sagittal Coronal" \
        text "{Axial} {Sagittal} {Coronal}" {
        eval {radiobutton $f.r$x -width 7 -text $text \
            -variable MRProstateCare(realtimeScanOrient) -value $text \
            -relief raised -offrelief raised -overrelief raised \
            -command "" \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rCoronal select
    $f.rCoronal configure -state normal 
 
    grid $f.lTitle -row 0 -column 0 -columnspan 3 -pady 5 -sticky news
    grid $f.rAxial $f.rSagittal $f.rCoronal -pady 2 -padx 1 
 

    #-------------------------
    # Frame Mid 
    #-------------------------
    set f $parent.fMid
    foreach x "1 2 3" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top -pady 1 
    }

    set f $parent.fMid.f1

    eval {label $f.l -text "Target a new location:"} $Gui(WTA)
    frame $f.f -bg $Gui(activeWorkspace)
    pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

    set f $f.f
    foreach ax "x z y" text "R S A" {
        eval {label $f.l$ax -text $text -width 7} $Gui(WLA)
    }
    grid $f.lx $f.lz $f.ly -pady 1 -padx $Gui(pad) -sticky e

    foreach ax "x z y" text "R S A" {
        eval {entry $f.e$ax -justify right -width 8 \
            -textvariable MRProstateCare(${ax}Str)} $Gui(WEA)
        set MRProstateCare(${ax}Str) 0.0
    }
    grid $f.ex $f.ez $f.ey -pady 1 -padx 5 -sticky e


    set f $parent.fMid.f2

    DevAddButton $f.bPlus "+" "MRProstateCareChangeRSA +"  6 
    DevAddButton $f.bMinus "-" "MRProstateCareChangeRSA -"  6 
    DevAddButton $f.bReset "Reset" "MRProstateCareChangeRSA r" 6 
    DevAddButton $f.bSend "Send" "MRProstateCareSetScannerCommand 2" 6 
 
    blt::table $f \
        0,0 $f.bPlus -fill x -padx 1 -pady 2 \
        0,1 $f.bMinus -fill x -padx 1 -pady 2 \
        0,2 $f.bReset -fill x -padx 1 -pady 2 \
        0,3 $f.bSend -fill x -padx 1 -pady 2 


    #-------------------------
    # Frame Bot
    #-------------------------
    set f $parent.fBot

    frame $f.f1 -bg $Gui(activeWorkspace) 
    pack $f.f1 -side top -pady 0 
    frame $f.f2 -bg $Gui(activeWorkspace) 
    pack $f.f2 -side top -pady 1 

    set f $parent.fBot.f2
    DevAddButton $f.bStart "Start scan" "MRProstateCareSetScannerCommand 1"  10 
    DevAddButton $f.bStop "Stop scan" "MRProstateCareSetScannerCommand 0"  10 
    blt::table $f \
        0,0 $f.bStart -fill x -padx 1 -pady 3 \
        0,2 $f.bStop -fill x -padx 1 -pady 3 
}


proc MRProstateCareChangeRSA {v} {
    global MRProstateCare View 

    if {$v == "r"} {
        foreach v "x y z" {
            set MRProstateCare(${v}Str) \
                $MRProstateCare(${v}Str,original)
        }
    } else {
        if {$v == "+"} {
            set val 1
        } else {
            set val -1
        }

        # Axial slice changes as S
        # Saggital slice changes as R
        # Coronal slice changes as A
        switch $MRProstateCare(realtimeScanOrient) {
            "Axial" {set MRProstateCare(zStr) [expr $MRProstateCare(zStr) + $val]} 
            "Sagittal" {set MRProstateCare(xStr) [expr $MRProstateCare(xStr) + $val]} 
            "Coronal" {set MRProstateCare(yStr) [expr $MRProstateCare(yStr) + $val]} 
        }

        set max [expr $View(fov) / 2]
        set min [expr -$max]
        foreach ii "x y z" {
            if {$MRProstateCare(${ii}Str) > $max} {
                set MRProstateCare(${ii}Str) $max
            }
            if {$MRProstateCare(${ii}Str) < $min} {
                set MRProstateCare(${ii}Str) $min
            }
        }
    }

}


proc MRProstateCareSetScannerCommand {cmd} {
    global MRProstateCare Locator 

    if {$cmd != 0 && $MRProstateCare(currentPoint) == "none"} {
            DevErrorWindow "Please select a valid point for scanning."
            return
    } 

    if {$cmd != 0} {
        # validate values of r, s, and a
        foreach ii "x y z" {
            if {[ValidateInt $MRProstateCare(${ii}Str)] == 0 &&
                [ValidateFloat $MRProstateCare(${ii}Str)] == 0} {
                    DevErrorWindow "RSA values must be integer/float."
                    return
            }
        }


        set r $MRProstateCare(xStr)
        set s $MRProstateCare(zStr)
        set a $MRProstateCare(yStr)


        # patient postition: 
        # 0 = head first, supine
        # 1 = head first, prone
        # 2 = head first, left decub
        # 3 = head first, right decub
        # 4 = feet first, supine
        # 5 = feet first, prone
        # 6 = feet first, left decub
        # 7 = feet first, right decub
        set ppos 4 

        # table position:
        # 0 = axial
        # 1 = side
        # 2 = vertical
        set tpos 1

        set pxyz [MRProstateCareGetPxyz $r $s $a $ppos $tpos] 
        switch $MRProstateCare(realtimeScanOrient) {
            "Axial" {set or 1}
            "Sagittal" {set or 2}
            "Coronal" {set or 3}
        }
        Locator(Flashpoint,src) SetScanOrientation \
            $or [lindex $pxyz 0] [lindex $pxyz 1] [lindex $pxyz 2] $MRProstateCare(realtimeImageID) 

        set idInfo "$MRProstateCare(realtimeImageID) $or $r $s $a"
        set Locator(nextRealtimeImageInfo) $idInfo
        set MRProstateCare(lastRealtimeScanOrient) \
            $MRProstateCare(realtimeScanOrient)

        incr MRProstateCare(realtimeImageID)
    }   

    Locator(Flashpoint,src) OperateScanner $cmd 
}


proc MRProstateCareGetPxyz {r s a ppos tpos} {
    global MRProstateCare

    set PXYZ [MRProstateCareRSAtoXYZ $r $s $a $ppos $tpos]
    set LPx [lindex $PXYZ 0] 
    set LPy [lindex $PXYZ 1] 
    set LPz [lindex $PXYZ 2] 

    # Set image origin to tip location.
    set Px [expr {round($LPx)}]
    set Py [expr {round($LPy)}]
    set Pz [expr {round($LPz)}]
   
    set Pxyz "$Px $Py $Pz" 
}


proc MRProstateCareRSAtoXYZ {R S A patpos tblpos} {

    if {$tblpos == 0} {
        switch $patpos {
            0 {set X $R; set Y $A; set Z $S}
            1 {set X [expr -$R]; set Y [expr -$A]; set Z $S}
            2 {set X [expr -$A]; set Y $R; set Z $S}
            3 {set X $A; set Y [expr -$R]; set Z $S}
            4 {set X [expr -$R]; set Y $A; set Z [expr -$S]}
            5 {set X $R; set Y [expr -$A]; set Z [expr -$S]}
            6 {set X $A; set Y $R; set Z [expr -$S]}
            7 {set X [expr -$A]; set Y [expr -$R]; set Z [expr -$S]}
        } 
        return "$X $Y $Z"
    }

    if {$tblpos == 1} {
        switch $patpos {
            0 {set X $S; set Y $A; set Z [expr -$R]}
            1 {set X $S; set Y [expr -$A]; set Z $R}
            2 {set X $S; set Y $R; set Z $A}
            3 {set X $S; set Y [expr -$R]; set Z [expr -$A]}
            4 {set X [expr -$S]; set Y $A; set Z $R}
            5 {set X [expr -$S]; set Y [expr -$A]; set Z [expr -$R]}
            6 {set X [expr -$S]; set Y $R; set Z [expr -$A]}
            7 {set X [expr -$S]; set Y [expr -$R]; set Z $A}
        }
        return "$X $Y $Z"
    }
}


proc MRProstateCareBuildGUIForDisplay {parent} {
    global MRProstateCare Gui 

    set f $parent
    frame $f.f1 -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.f1 -side top -pady 3 -fill x 
    frame $f.f1b -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.f1b -side top -pady 3 -fill x
    frame $f.f2 -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.f2 -side top -pady 3 -fill x 
    frame $f.f3 -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.f3 -side top -pady 3 -fill x 
    frame $f.f4 -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.f4 -side top -pady 3 -fill x 
    frame $f.f5 -bg $Gui(activeWorkspace)
    pack $f.f5 -side top -pady 3 -fill x 

    #-------------------------
    # Frame 1 
    #-------------------------
    set f $parent.f1
 
    # Build pulldown menu for all Points 
    eval {label $f.lTitle -text "Select a point:"} $Gui(WTA)
 
    set tList [list {none}]
    set df [lindex $tList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 28 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectPoint $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,displayPointButton) $f.mbType
    set MRProstateCare(gui,displayPointMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lTitle -padx 2 -pady 3 \
        1,0 $f.mbType -fill x -padx 3 -pady 2 


    #-------------------------
    # Frame 1b 
    #-------------------------
    set f $parent.f1b
    foreach x "Top Mid Bot" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top -pady 1 
    }

    set f $parent.f1b.fTop
    eval {label $f.lTitle -text "Reorient an image:"} $Gui(WTA)
    pack $f.lTitle -side top -pady 2 
 
    set f $parent.f1b.fMid
    # Build pulldown menu for volumes of image 1 
    DevAddLabel $f.lVolume "Image:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 22 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    foreach m $mList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectFlipVolume $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,flipVolumeButton) $f.mbType
    set MRProstateCare(gui,flipVolumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lVolume -padx 2 -pady 1 -anchor e \
        0,1 $f.mbType -fill x -padx 3 -pady 1 -anchor w


    set f $parent.f1b.fBot
    DevAddButton $f.bAP "Flip A/P" "MRProstateCareFlipImage AP" 9 
    DevAddButton $f.bSI "Flip S/I" "MRProstateCareFlipImage SI" 9 
    DevAddButton $f.bRL "Flip R/L" "MRProstateCareFlipImage RL" 9 
    grid $f.bRL $f.bAP $f.bSI -padx 0 -pady 3 
 
    #-------------------------
    # Frame 2 
    #-------------------------
    set f $parent.f2
    foreach x "Top Mid Bot" {
        frame $f.f$x -bg $Gui(activeWorkspace) 
        pack $f.f$x -side top -pady 1 
    }

    set f $parent.f2.fTop
    eval {label $f.lTitle -text "Choose images:"} $Gui(WTA)
    pack $f.lTitle -side top -pady 2 
 
    set f $parent.f2.fMid
    # Build pulldown menu for volumes of image 1 
    DevAddLabel $f.lVolume "Background:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 17 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectVolume 1 $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,displayImage1VolumeButton) $f.mbType
    set MRProstateCare(gui,displayImage1VolumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lVolume -padx 2 -pady 1 -anchor e \
        0,1 $f.mbType -fill x -padx 3 -pady 1 -anchor w


    set f $parent.f2.fBot
    # Build pulldown menu for volumes 
    DevAddLabel $f.lVolume "Foreground:"

    set mList [list {none}]
    set df [lindex $mList 0] 
    eval {menubutton $f.mbType -text $df \
          -relief raised -bd 2 -width 17 \
          -indicatoron 1 \
          -menu $f.mbType.m} $Gui(WMBA)
    eval {menu $f.mbType.m} $Gui(WMA)
    
    foreach m $tList  {
        $f.mbType.m add command -label $m \
            -command "MRProstateCareSelectVolume 2 $m"
    }

    # Save menubutton for config
    set MRProstateCare(gui,displayImage2VolumeButton) $f.mbType
    set MRProstateCare(gui,displayImage2VolumeMenu) $f.mbType.m

    blt::table $f \
        0,0 $f.lVolume -padx 2 -pady 1 -anchor e \
        0,1 $f.mbType -fill x -padx 3 -pady 1 -anchor w


    #-------------------------
    # Frame 3 
    #-------------------------
    set f $parent.f3

    eval {label $f.lTitle -text "Current display orientation:"} $Gui(WTA)
 
    foreach x "Axial Sagittal Coronal" \
        text "{Axial} {Sagittal} {Coronal}" {
        eval {radiobutton $f.r$x -width 7 -text $text \
            -variable MRProstateCare(imageDisplayOrient) -value $text \
            -relief raised -offrelief raised -overrelief raised \
            -command "" \
            -state disabled \
            -selectcolor white} $Gui(WEA)
    } 
    $f.rCoronal select

    grid $f.lTitle -row 0 -column 0 -columnspan 3 -pady 5 -sticky news
    grid $f.rAxial $f.rSagittal $f.rCoronal -pady 2 -padx 1 


    #-------------------------
    # Frame 4 
    #-------------------------
    set f $parent.f4
    eval {label $f.lTitle -text "Zoom 3D view:"} $Gui(WTA)
    eval {scale $f.s3D -from 1.0 -to 5.0 -length 115 \
        -command "MRProstateCareSetScaleFactor" \
        -resolution 1.0} $Gui(WSA) 

    grid $f.lTitle $f.s3D -padx 2 -pady 1 


    #-------------------------
    # Frame 5 
    #-------------------------
    set f $parent.f5
    DevAddButton $f.bStart "Start" "set MRProstateCare(navLoop) 1; \
                                    MRProstateCareNavLoop"  10 
    DevAddButton $f.bStop "Stop" "MRProstateCareStopNav"  10 
    grid $f.bStart $f.bStop -padx 1 -pady 5 
}



proc MRProstateCareFlipImage {axis} {
    global MRProstateCare Volume  

    set v $MRProstateCare(currentFlipVolumeID)

    #--- get the corresponding axis in Vtk space.
    set newvec [MRProstateCareGetRasToVtkAxis $axis Volume($v,node)]
    #--- unpack the vector into x, y and z
    foreach { x y z } $newvec { }
    vtkImageFlip flip
    flip SetInput [Volume($v,vol) GetOutput]
    #--- now set the flip axis in VTK space
    if { ($x == 1) || ($x == -1) } {
        flip SetFilteredAxis 0
    } elseif { ($y == 1) || ( $y == -1) } {
        flip SetFilteredAxis 1
    } elseif { ($z == 1) || ($z == -1) } {
        flip SetFilteredAxis 2
    }
    Volume($v,vol) SetImageData [ flip GetOutput ]

    MainVolumesUpdate $v
    flip Delete

    RenderAll


}



proc MRProstateCareGetRasToVtkAxis {axis vnode} {
    #
    #--- Given an axis in RAS space, we want
    #--- to find the axis that corresponds in VTK 
    #--- space (which describes the vtkImageData
    #--- represented by a vtkMrmlVolumeNode.)
    #
    #--- create a vtkTransform
    vtkTransform T
    #--- get the transform matrix in string form
    #--- and set it to be the transform's matrix.
    set m [ $vnode GetRasToVtkMatrix ]
    eval T SetMatrix $m

    #--- axis along which to flip in VTK space
    if { $axis == "RL" } {
        #--- if Flipping along R-L
        #puts "RAS axis 1 0 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 -1 0 0 ]
    } elseif { $axis == "AP" } {
        #--- if flipping along A-P
        #puts "RAS axis 0 1 0"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 -1 0 ]
    } elseif { $axis == "SI" } {
        #--- if flipping along S-I
        #puts "RAS axis 0 0 1"
        set newvec [ T TransformFloatVectorAtPoint 0 0 0 0 0 -1 ]
    }
    foreach { x y z } $newvec { }
    #puts "VTK axis: $x $y $z"
    #--- because of scale, newvec might not be unit
    if { $x != 0 } {
        set newvec [ lreplace $newvec 0 0 1 ]
    }
    if { $y != 0 } {
        set newvec [ lreplace $newvec 1 1 1 ]
    }
    if { $z != 0 } {
        set newvec [ lreplace $newvec 2 2 1 ]
    }
    
    #--- clean up
    T Delete
    #--- return the new axis as a vector.
    return $newvec
}



proc MRProstateCareSetScaleFactor {v} {
    global MRProstateCare  

    set MRProstateCare(scaleFactor) $v
}


proc MRProstateCareUpdateForNav {} { 
    global MRProstateCare Locator Slice Anno Volume 


    # update display RSA
    set MRProstateCare(displayRSA) $Locator(realtimeRSA)

    # from scanning orientation to dispaly orientation
    switch $Locator(realtimeScanOrder) {
        "SI" {set MRProstateCare(imageDisplayOrient) "Axial"} 
        "RL" {set MRProstateCare(imageDisplayOrient) "Sagittal"} 
        "AP" {set MRProstateCare(imageDisplayOrient) "Coronal"} 
    }
 
    # clean the 3D view
    MainSlicesSetVisibilityAll 0
    Render3D
    # turn off orientation letters and cube in 3D view
    set Anno(letters) 0
    set Anno(box) 0
    MainAnnoSetVisibility


    # set back and fore images right
    MainSlicesSetVolumeAll Back $MRProstateCare(image1,currentVolumeID) 
    MainVolumesSetActive $MRProstateCare(image1,currentVolumeID)
    MainSlicesSetVolumeAll Fore $MRProstateCare(image2,currentVolumeID) 
    MainVolumesSetActive $MRProstateCare(image2,currentVolumeID)
    MainVolumesRender


    # set right slice to display
    set coords $MRProstateCare(displayRSA) 
    # Axial slice changes as S
    # Saggital slice changes as R
    # Coronal slice changes as A
    foreach s "0 1 2" i "1 0 2" {
        set Slice($s,offset) [lindex $coords $i] 
        MainSlicesSetOffset $s
        RenderBoth $s
    }


    # draw a ball for the point in 3D view
    # write the point name in 3D view
    if {$MRProstateCare(currentPoint) != "none"} {
        set i 0 
        set p $MRProstateCare(currentPoint)
        set i2 [string first ":" $p]
        set title [string range $p $i [expr $i2-1]] 
        set title [string trim $title]
        MRProstateCareShowPoint $title
    }

    MRProstateCareShowSliceIn3D
}


proc MRProstateCareShowSliceIn3D {} { 
    global MRProstateCare Slice Anno Volume 

    # show the slice according to the specified orientation
    switch $MRProstateCare(imageDisplayOrient) {
        "Axial" {
            set Slice(0,visibility) 1 
            set Slice(1,visibility) 0 
            set Slice(2,visibility) 0 
            set s 0

            # adjust the display orientation
            # Click "I" in the direction window to 
            # get Axial display in the 3D view
            MainViewNavReset 40 65 click 
            MRProstateCareZoom
        }
        "Sagittal" {
            set Slice(0,visibility) 0 
            set Slice(1,visibility) 1 
            set Slice(2,visibility) 0 
            set s 1 

            # Click "L" to get Axial display 
            # in the 3D view
            MainViewNavReset 61 25 click 
            MRProstateCareZoom
        }
        "Coronal" {
            set Slice(0,visibility) 0 
            set Slice(1,visibility) 0 
            set Slice(2,visibility) 1 
            set s 2 

            # Click "A" to get Axial display 
            # in the 3D view
            MainViewNavReset 55 38 click 
            MRProstateCareZoom
        }
    }
    MainSlicesSetVisibility ${s}
    MainViewerHideSliceControls 
    Render3D
}

 
proc MRProstateCareNavLoop {} { 
    global MRProstateCare Locator Slice Anno Volume 


    if {! $MRProstateCare(navLoop)} {
        return
    }


    set both [expr {$MRProstateCare(image1,currentVolumeID) > 0  
                    && $MRProstateCare(image2,currentVolumeID) > 0}] 
    if {! $both} {
        DevErrorWindow "Please have both images available for display. Then press Start button to begin."
        return
    } 

    MRProstateCareShowSliceIn3D

    if {$Slice(opacity) == 1} {set MRProstateCare(navLoopFactor) -1}
    if {$Slice(opacity) == 0} {set MRProstateCare(navLoopFactor) +1}
    set Slice(opacity) [expr $Slice(opacity) + $MRProstateCare(navLoopFactor) * 0.1]
    MainSlicesSetOpacityAll
    RenderAll

    after $MRProstateCare(navTime) MRProstateCareNavLoop
}


proc MRProstateCareGetRSAFromPoint {p} {
    global MRProstateCare 

    set coords ""
    if {$p != ""} {
        set i 0 
        set i2 [string first ":" $p]
        set title [string range $p $i [expr $i2-1]] 
        set rsa [string range $p [expr $i2+3] end-1] 

        set rsa [string trim $rsa]
        set title [string trim $title]
        regsub -all {( )+} $rsa " " rsa 
        set coords [split $rsa " "]
    }

    return $coords
}

 
proc MRProstateCareHidePoint {} {
    global MRProstateCare View

    $MRProstateCare(pointTitleActor) SetVisibility 0 
    pointActor SetVisibility 0 

}


proc MRProstateCareShowPoint {title} {
    global MRProstateCare View

    set rb [lindex $MRProstateCare(displayRSA) 0]
    set ab [lindex $MRProstateCare(displayRSA) 2]
    set sb [lindex $MRProstateCare(displayRSA) 1]
 
    set pos [expr   $View(fov) * 0.45]
    set neg [expr - $View(fov) * 0.45]

    switch $MRProstateCare(imageDisplayOrient) {
        "Axial" {
            set rt 0.0
            set at $pos 
            set st $neg
            set sb $st
        }
        "Sagittal" {
            set rt $neg 
            set at 0.0 
            set st $pos
            set rb $rt
        }
        "Coronal" {
            set rt 0.0 
            set at $pos 
            set st $pos
            set ab $at
        }
    }
    $MRProstateCare(pointTitleText) SetText $title 
    $MRProstateCare(pointTitleActor) SetVisibility 1 
    $MRProstateCare(pointTitleActor) SetPosition $rt $at $st  

    pointActor SetVisibility 1 
    pointActor SetPosition $rb $ab $sb  
}


proc MRProstateCareStopNav {} { 
    global MRProstateCare 

    set MRProstateCare(navLoop) 0
}


proc MRProstateCareUpdateVolumes {} {
    global MRProstateCare 

}


proc MRProstateCareUpdatePoints {} {
    global MRProstateCare 

}


proc MRProstateCareSelectPoint {m} {
    global MRProstateCare 

    # configure menubutton
    $MRProstateCare(gui,displayPointButton) config -text $m 
    $MRProstateCare(gui,scanPointButton) config -text $m 
    set MRProstateCare(currentPoint) $m

    # Update rsa values in Display->Scan tab
    MRProstateCareUpdateRSA

    # draw a ball for the point in 3D view
    # write the point name in 3D view
    if {$MRProstateCare(currentPoint) != "none"} {
        set i 0 
        set p $MRProstateCare(currentPoint)
        set i2 [string first ":" $p]
        set title [string range $p $i [expr $i2-1]] 
        set title [string trim $title]
        MRProstateCareShowPoint $title
    }
}


proc MRProstateCareSelectFlipVolume {v} {
    global MRProstateCare Volume 

    set name [Volume($v,node) GetName] 

    # configure menubutton
    $MRProstateCare(gui,flipVolumeButton) config -text $name 
    set MRProstateCare(currentFlipVolumeID) $v

    MainSlicesSetVolumeAll Back $v
    MainVolumesSetActive $v
    MainVolumesRender
}


proc MRProstateCareSelectVolume {which v} {
    global MRProstateCare Volume 

    set name [Volume($v,node) GetName] 

    # configure menubutton
    if {$which == 1} {
        $MRProstateCare(gui,displayImage1VolumeButton) config -text $name 
        set MRProstateCare(image1,currentVolumeID) $v
        set layer Back
    } else {
        $MRProstateCare(gui,displayImage2VolumeButton) config -text $name 
        set MRProstateCare(image2,currentVolumeID) $v
        set layer Fore
    }

    MainSlicesSetVolumeAll $layer $v
    MainVolumesSetActive $v
    MainVolumesRender
}


proc MRProstateCareConnect {} {
    global MRProstateCare 

    LocatorRegisterCallback MRProstateCareUpdateForNav

    LocatorConnect
    MRProstateCareUpdateConnectionStatus

}


proc MRProstateCareLoad {} {
    global MRProstateCare 

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


proc MRProstateCareVerify {all} {
    global MRProstateCare 

    if {$MRProstateCare(userInputError)} {
        return
    }
 
    #-------------------------------------------------------
    # Write user inputs into file for Steve Haker's 
    # template computation
    #-------------------------------------------------------
    set name $MRProstateCare(entry,PName)
    set name [string trim $name]
    # replace all spaces in the middle of name
    regsub -all { +} $name "_" name 
    # replace all , in the middle of name
    regsub -all {,+} $name "_" name 

    set inName $name
    set outName $name
    append inName "_in.txt" 
    append outName "_out.txt" 
    set inFileName [file join $MRProstateCare(tempDir) $inName]
    set outFileName [file join $MRProstateCare(tempDir) $outName]
    set MRProstateCare(outFileName) $outFileName

    # all = 1: four corners and all points
    # all = 0: only verify four corners
    MRProstateCareWrite $inFileName $all

    vtkProstateCoords pc
    pc SetFileName 1 $inFileName
    pc SetFileName 0 $outFileName
    pc Run
    pc Delete
}


proc MRProstateCareLogLoop {} {
    global MRProstateCare Locator 

    if {! $MRProstateCare(servLoop)} {
        return
    }

    #-------------------------------------------------------
    # Write user inputs into file which can be loaded later
    # by clicking the Load button
    #-------------------------------------------------------
 
    set fileName [file join $MRProstateCare(tempDir) $MRProstateCare(logFile)]
 
    set fd [open $fileName w]
    set comment "# This is the log file of MRProstateCare module. Do not edit it.\n"
    puts $fd $comment

    set comment "# host"
    puts $fd $comment
    set str "set Locator(Flashpoint,host) $Locator(Flashpoint,host)\n"
    puts $fd $str

    set comment "# port"
    puts $fd $comment
    set str "set Locator(Flashpoint,port) $Locator(Flashpoint,port)\n"
    puts $fd $str

    set comment "# msPoll"
    puts $fd $comment
    set str "set Locator(Flashpoint,msPoll) $Locator(Flashpoint,msPoll)\n"
    puts $fd $str

    set comment "# date"
    puts $fd $comment
    set str "set MRProstateCare(entry,PDate) $MRProstateCare(entry,PDate)\n"
    puts $fd $str

    set comment "# patient name"
    puts $fd $comment
    set str "set MRProstateCare(entry,PName) $MRProstateCare(entry,PName)\n"
    puts $fd $str

    set comment "# patient id"
    puts $fd $comment
    set str "set MRProstateCare(entry,PID) $MRProstateCare(entry,PID)\n"
    puts $fd $str

    set comment "# step"
    puts $fd $comment
    set str "set MRProstateCare(entry,Step) $MRProstateCare(entry,Step)\n"
    puts $fd $str

    set comment "# anterior right"
    puts $fd $comment
    set str "set MRProstateCare(entry,AR) \{$MRProstateCare(entry,AR)\}\n"
    puts $fd $str

    set comment "# posterior right"
    puts $fd $comment
    set str "set MRProstateCare(entry,PR) \{$MRProstateCare(entry,PR)\}\n"
    puts $fd $str

    set comment "# posterior left"
    puts $fd $comment
    set str "set MRProstateCare(entry,PL) \{$MRProstateCare(entry,PL)\}\n"
    puts $fd $str

    set comment "# anterior left"
    puts $fd $comment
    set str "set MRProstateCare(entry,AL) \{$MRProstateCare(entry,AL)\}\n"
    puts $fd $str

    set comment "# the point list"
    puts $fd $comment
    set str "set MRProstateCare(pointList) \"\"\n"
    puts $fd $str
    set str "\$MRProstateCare(pointListBox) delete 0 end\n"
    puts $fd $str

    foreach x $MRProstateCare(pointList) {
        set str "lappend MRProstateCare(pointList) \{$x\}\n"
        puts $fd $str
        set str "\$MRProstateCare(pointListBox) insert end \{$x\}\n" 
        puts $fd $str
    }

    close $fd

    after $MRProstateCare(logTime) MRProstateCareLogLoop
}


proc MRProstateCareWrite {fn all} {
    global MRProstateCare 

    set fd [open $fn w]
    puts $fd "\n\n\n\n\n"
 
    set comment "# This text file saves the user input. Do not edit it.\n"
    puts $fd $comment

    set name $MRProstateCare(entry,PName)
    set name [string trim $name]
    puts $fd "patient_name = $name\n\n"

    set id $MRProstateCare(entry,PID)
    set id [string trim $id]
    puts $fd "patient_id = $id\n\n"

    set date $MRProstateCare(entry,PDate)
    set date [string trim $date]
    puts $fd "date = $date\n\n\n"

    set v $MRProstateCare(entry,AR)
    set v [string trim $v]
    puts $fd "1) Anterior  Right (RSA) = $v\n\n"

    set v $MRProstateCare(entry,PR)
    set v [string trim $v]
    puts $fd "2) Posterior Right (RSA) = $v\n\n"

    set v $MRProstateCare(entry,PL)
    set v [string trim $v]
    puts $fd "3) Posterior Left  (RSA) = $v\n\n"

    set v $MRProstateCare(entry,AL)
    set v [string trim $v]
    puts $fd "4) Anterior  Left  (RSA) = $v\n\n\n"

    set step $MRProstateCare(entry,Step)
    set step [string trim $step]
    puts $fd "Step = $step\n\n\n"

    if {$all} {
        puts $fd "Points           R       S       A    Col    Row    Depth(cm)\n"
        puts $fd "-------------------------------------------------------------\n\n"

        foreach x $MRProstateCare(pointList) {
            if {$x != ""} {
                set i 0 
                set i2 [string first ":" $x]
                set title [string range $x $i [expr $i2-1]] 
                set rsa [string range $x [expr $i2+3] end-1] 

                set rsa [string trim $rsa]
                set title [string trim $title]

                puts $fd "$title  $rsa \n\n"
            }
        }
    }

    close $fd
}


proc MRProstateCareView {} {
    global MRProstateCare 

    if {$MRProstateCare(userInputError)} {
        return
    }
 
    if {! [info exists MRProstateCare(outFileName)] ||
        ! [file exists $MRProstateCare(outFileName)]} {
        DevErrorWindow "Output file doesn't exist."
        return
    }
 
    if {! [info exists MRProstateCare(newID)]} {
        set MRProstateCare(newID) 0
    }
    incr MRProstateCare(newID)

    if {$MRProstateCare(currentTab) == 2} {
        # Template tab
        set txt "<H3>Template and related info</H3>"
    } else {
        # Points tab
        set txt "<H3>Point coords with respect to the template</H3>"
    }

    append txt "<BR>\n"

    set fd [open $MRProstateCare(outFileName) r]
    set data [read $fd]
    close $fd

    set lines [split $data "\n"]
    set isPreEmpty 0
    foreach line $lines {
        set line [string trim $line]
        if {$line != "" || ($line == "" && ! $isPreEmpty)} {
            append txt "<BR>\n"
            append txt $line

            set isPreEmpty [expr {$line == "" ? 1 : 0}]
        }
    }

    MRProstateCareCreateTextPopup infowin$MRProstateCare(newID) \
        "MRProstateCare Info" 400 100 25 $txt
}


proc MRProstateCareCreateTextPopup {topicWinName title x y textBoxHit txt} {
    set w .w$topicWinName
    #--- if .w$topicWinName exists,
    #--- destroy it, and create a new one
    #--- containing new requested text.
    if { [info exists $w] } {
        -command "destroy $w"
    }
    
    #--- format text.
    regsub -all "\n" $txt {} txt
    DevApplyTextTags $txt
    if { ![info exists ::Dev(TextFormat,tagList)] } {
        set ::Dev(TextFormat,tagList) ""
    }
    
    #--- create popup window and configure
    toplevel $w -class Dialog -background #FFFFFF
    wm title $w $title
    wm iconname $w Dialog
    wm geometry $w +$x+$y
    focus $w

    set dismissButtonHit 4
    set minWinHit [ expr $textBoxHit + $dismissButtonHit ]
    wm minsize $w 40 $minWinHit
    frame $w.fMsg -background #FFFFFF
    frame $w.fButton -background #FFFFFF
    pack $w.fMsg -fill both -expand true
    pack $w.fButton -side top -pady 4 -padx 4

    #--- make scrolled text widget to contain text
    set f $w.fMsg
    set helpt [ text $f.tMessage -height $textBoxHit -width 35 -setgrid true -wrap word \
                -yscrollcommand "$f.sy set" -cursor arrow -insertontime 0 -bg #FFFFFF ]
    scrollbar $f.sy -orient vert -command "$f.tMessage yview" -background #DDDDDD \
                    -activebackground #DDDDDD
    pack $f.sy -side right -anchor e -fill y
    pack $f.tMessage -side left -fill both -expand true -padx 4 -pady 4
    
    #--- make button to dismiss the window
    set f $w.fButton
    button $f.bDismiss -text "close" -width 6 -bg #DDDDDD \
        -command "destroy $w"
    pack $f.bDismiss -padx 4 -pady 4 -side bottom
    
    #--- set the font to be 10 point helvetica
    $f.bDismiss config -font "-Adobe-Helvetica-Normal-R-Normal-*-10-*-*-*-*-*-*-*"

    #--- insert the text and raise window.
    DevInsertPopupText $helpt
#    DevRaisePopup $w
}

proc MRProstateCareShowPointToEdit {} {
    global MRProstateCare 

    set curs [$MRProstateCare(pointListBox) curselection]
    if {$curs != ""} {
        set point [$MRProstateCare(pointListBox) get $curs] 
        if {$point != ""} {
            set i 0 
            set i2 [string first ":" $point]
            set title [string range $point $i [expr $i2-1]] 
            set rsa [string range $point [expr $i2+3] end-1] 

            set rsa [string trim $rsa]
            set title [string trim $title]

            set MRProstateCare(entry,Title) $title
            set MRProstateCare(entry,Coords) $rsa 
            set MRProstateCare(editIndex) $curs
            set MRProstateCare(editMode) 1
       }
    } else {
        set MRProstateCare(editIndex) -1 
        DevErrorWindow "Select a point to edit."
    }
}


proc MRProstateCareDeletePoint {} {
    global MRProstateCare 

    set curs [$MRProstateCare(pointListBox) curselection]
    if {$curs >= 0} {
        $MRProstateCare(pointListBox) delete $curs
        set size [llength $MRProstateCare(pointList)]
        set MRProstateCare(pointList) \
            [lreplace $MRProstateCare(pointList) $curs $curs]
        set size [llength $MRProstateCare(pointList)]

    } else {
        DevErrorWindow "Select a point to delete."
    }
}


proc MRProstateCareAddOrEditPoint {} {
    global MRProstateCare 

    set title $MRProstateCare(entry,Title)
    set title [string trim $title]
    if {$title == ""} {
        DevErrorWindow "Must have the point title set."
        return
    }

    set rsa $MRProstateCare(entry,Coords)
    set rsa [string trim $rsa]
    if {$rsa == ""} {
        DevErrorWindow "Must have the point coords (RSA) set."
        return
    }

    # Replace multiple spaces in the middle of 
    # the string by one space
    regsub -all {( )+} $rsa " " rsa 
    set vl [split $rsa " "]
    if {[llength $vl] != 3} {
        DevErrorWindow "Input 3 integer/float values for the coords."
        return
    }
    foreach x $vl {
        if {[ValidateInt $x] == 0 &&
            [ValidateFloat $x] == 0} {
            DevErrorWindow "Input 3 integer/float values for the coords."
            return
        }
    }


    # Replace one space in the middle of 
    # the string by four spaces
    regsub -all " " $rsa "    " rsa 

    set item "$title : ($rsa)"
    set index [lsearch -exact $MRProstateCare(pointList) $item]
    if {$index != -1} { 
        DevErrorWindow "The point is already added in."
        return
    }


    # remove the old point from the list
    set curs [$MRProstateCare(pointListBox) curselection]
    if {$curs != "" && $curs >= 0 && $MRProstateCare(editMode)} { 
        # the new point will replace the selected point
        # in the list box
        set MRProstateCare(pointList) \
            [lreplace $MRProstateCare(pointList) $curs $curs]
    }

    # if the title of the new point is same as one in 
    # in the list box but their rsa's are different,
    # the new point will be added and the old one will
    # be removed.
    set c 0
    foreach p $MRProstateCare(pointList) {
        set t [string trim $title]
        set i [string first $t $p 0] 
        if {$i >= 0} {
            set rt [DevYesNo "Are you sure you want to replace the existing point with different RSA values?"]
            if {$rt == "no"} {
                return 
            } else {
                set MRProstateCare(pointList) \
                    [lreplace $MRProstateCare(pointList) $c $c]
            }
            break
        }
        incr c
    }
 

    # Keep and sort the new point in the point list
    lappend MRProstateCare(pointList) $item
    set MRProstateCare(pointList) \
        [lsort -dictionary $MRProstateCare(pointList)]  

    # Put the point list into the list box
    $MRProstateCare(pointListBox) delete 0 end
    foreach x $MRProstateCare(pointList) {
        $MRProstateCare(pointListBox) insert end $x 
    }

    set MRProstateCare(editMode)   0 
}


proc MRProstateCareCheckTemplateUserInput {} {
    global MRProstateCare

    # check the user input on Template tab
    set MRProstateCare(userInputError) 0 
 
    # the date field
    set date $MRProstateCare(entry,PDate)
    set date [string trim $date]
    if {$date == ""} {
        DevErrorWindow "Must have the date set (e.g. 07/15/06)."
        set MRProstateCare(userInputError) 1
        return
    }

    # the patient name field
    set name $MRProstateCare(entry,PName)
    set name [string trim $name]
    if {$name == ""} {
        DevErrorWindow "Must have the patient name set."
        set MRProstateCare(userInputError) 1
        return
    }

    # the patient id field
    set id $MRProstateCare(entry,PID)
    set id [string trim $id]
    if {$id == ""} {
        DevErrorWindow "Must have the patient id set."
        set MRProstateCare(userInputError) 1
        return
    }

    # the step field
    set step $MRProstateCare(entry,Step)
    set step [string trim $step]
    if {$step == ""} {
        DevErrorWindow "Must have the step set."
        set MRProstateCare(userInputError) 1
        return
    }
    if {[ValidateInt $step] == 0 &&
        [ValidateFloat $step] == 0} {
        DevErrorWindow "Value of step must be either integer or float."
        set MRProstateCare(userInputError) 1
        return
    }

    # corner coordinates
    foreach x "AR PR PL AL" text \
        "{anterior right} {posterior right} {posterior left} {anterior left}" {
        set v $MRProstateCare(entry,$x)
        set v [string trim $v]
        if {$v == ""} {
            DevErrorWindow "Must have the ${text} corner set."
            set MRProstateCare(userInputError) 1
            return
        }

        # Replace multiple spaces in the middle of 
        # the string by one space
        regsub -all {( )+} $v " " v 
        set vl [split $v " "]
        if {[llength $vl] != 3} {
            DevErrorWindow "Input 3 integer/float values for the ${text} corner."
            set MRProstateCare(userInputError) 1
            return
        }
        foreach x $vl {
            if {[ValidateInt $x] == 0 &&
                [ValidateFloat $x] == 0} {
                DevErrorWindow "Input 3 integer/float values for the ${text} corner."
                set MRProstateCare(userInputError) 1
                return
            }
        }
    }
}




proc MRProstateCareUpdateConnectionStatus {} {
    global MRProstateCare Locator 

    if {$Locator(connect) == 0} {
        $MRProstateCare(connectStatus) config -text "none" 
    } else {
        $MRProstateCare(connectStatus) config -text "OK" 
    }
}


#-------------------------------------------------------------------------------
# .PROC MRProstateCareBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareBuildVTK {} {
    global Gui MRProstateCare View Slice Target Volume Lut Locator Anno 

    set Locator(server) Flashpoint
    set Gui(pc) 0 

    # Actor for point name
    #-------------------------------

    set scale [expr $View(fov) * $Anno(letterSize) ]
    vtkVectorText pointTitleText
    pointTitleText SetText "pointTitle"
    set MRProstateCare(pointTitleText) pointTitleText

    vtkPolyDataMapper  pointTitleMapper
    pointTitleMapper SetInput [pointTitleText GetOutput]
    vtkFollower pointTitleActor
    pointTitleActor SetMapper pointTitleMapper
    pointTitleActor SetScale  $scale $scale $scale 
    pointTitleActor SetPickable 0
    if {$View(bgName)=="White"} {
        [pointTitleActor GetProperty] SetColor 0 0 1 
    } else {
        [pointTitleActor GetProperty] SetColor 0 0 1 
    }
    [pointTitleActor GetProperty] SetDiffuse 0.0
    [pointTitleActor GetProperty] SetAmbient 1.0
    [pointTitleActor GetProperty] SetSpecular 0.0
    # add only to the Main View window
    viewRen AddActor pointTitleActor

    pointTitleActor SetPosition 0.0  0.0 0.0 
    pointTitleActor SetVisibility 0
    set MRProstateCare(pointTitleActor) pointTitleActor

    # Make point title follow camera
    pointTitleActor SetCamera $View(viewCam)

    # Actor for point location 
    #-------------------------------
    MakeVTKObject Sphere point 
    pointSource SetRadius 3.0 
    if {$View(bgName)=="White"} {
        [pointActor GetProperty] SetColor 0 0 1 
    } else {
        [pointActor GetProperty] SetColor 0 0 1 
    }
    pointActor SetPosition 0.0 0.0 0.0 
    pointActor SetVisibility 0 
}


#-------------------------------------------------------------------------------
# .PROC MRProstateCareEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc MRProstateCareEnter {} {
    global MRProstateCare Locator
    global LastX LastY
   
    set fileName [file join $MRProstateCare(tempDir) $MRProstateCare(logFile)]
    if {[file exists $fileName]} {
        set bak [string range $fileName 0 end-4]
        set bak $bak.bak.txt 
        file copy -force $fileName $bak
    }
 
    set MRProstateCare(servLoop) 1 
    MRProstateCareLogLoop

    # default connectiong port is 60000
    if {! $MRProstateCare(portSet)} {
        set Locator(Flashpoint,port) $MRProstateCare(port)
        set MRProstateCare(portSet) 1
    }
 
    MRProstateCareUpdateNavigationTab

    #--- push all event bindings onto the stack.
    MRProstateCarePushBindings

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


proc MRProstateCarePushBindings {} {
   global Ev Csys

    EvActivateBindingSet MRPCSlice0Events
    EvActivateBindingSet MRPCSlice1Events
    EvActivateBindingSet MRPCSlice2Events
}

proc MRProstateCarePopBindings {} {
    global Ev Csys

    EvDeactivateBindingSet MRPCSlice0Events
    EvDeactivateBindingSet MRPCSlice1Events
    EvDeactivateBindingSet MRPCSlice2Events
}

#-------------------------------------------------------------------------------
# .PROC MRProstateCareExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareExit {} {

    # pop event bindings
    MRProstateCarePopBindings

    set MRProstateCare(servLoop) 0

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

#-------------------------------------------------------------------------------
# .PROC MRProstateCareCreateBindings  
# Creates MRProstateCare event bindings for the three slice windows 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MRProstateCareCreateBindings {} {
    global Gui Ev

    EvDeclareEventHandler MRProstateCareSlicesEvents <1> \
        {set xc %x; set yc %y; MRProstateCareProcessMouseEvent $xc $yc}

    EvAddWidgetToBindingSet MRPCSlice0Events \
        $Gui(fSl0Win) {MRProstateCareSlicesEvents}
    EvAddWidgetToBindingSet MRPCSlice1Events \
        $Gui(fSl1Win) {MRProstateCareSlicesEvents}
    EvAddWidgetToBindingSet MRPCSlice2Events \
        $Gui(fSl2Win) {MRProstateCareSlicesEvents}    
}

proc MRProstateCareProcessMouseEvent {x y} {
    global MRProstateCare Interactor Anno

    if {$MRProstateCare(currentTab) != 3} {
        # Only on Points tab are we interested in 
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
    set cat $MRProstateCare(category)
    if {! $MRProstateCare(editMode)} {
        if {$cat != "Sextant"} {
            set MRProstateCare(entry,Title) $cat
            append MRProstateCare(entry,Title) "_"
        } else {
            set MRProstateCare(entry,Title) "" 
        }
    }
    set MRProstateCare(entry,Coords) "$R     $S     $A"
}


proc MRProstateCareSetCurrentTab {index} {
    global MRProstateCare

    set MRProstateCare(currentTab) $index

    if {$index == 4} {
        MRProstateCareUpdateNavigationTab
    }
}


proc MRProstateCareUpdateNavigationTab {} {
    global MRProstateCare Volume

    # Inside the Navigation tab update the point list
    $MRProstateCare(gui,displayPointMenu) delete 0 end 
    $MRProstateCare(gui,scanPointMenu) delete 0 end 
 
    set size [llength $MRProstateCare(pointList)]
    if {$size == 0} {
        MRProstateCareSelectPoint none
        $MRProstateCare(gui,displayPointMenu) add command -label none \
            -command "MRProstateCareSelectPoint none"
        $MRProstateCare(gui,scanPointMenu) add command -label none \
            -command "MRProstateCareSelectPoint none"
 
    } else {
        foreach x $MRProstateCare(pointList) {
            $MRProstateCare(gui,displayPointMenu) add command \
                -label $x \
                -command "MRProstateCareSelectPoint \{$x\}"
            $MRProstateCare(gui,scanPointMenu) add command \
                -label $x \
                -command "MRProstateCareSelectPoint \{$x\}"
        }
        set x [lindex $MRProstateCare(pointList) 0]
        MRProstateCareSelectPoint "$x" 
    }

    # Inside the Navigation tab update the volume list
    # for reorienting 
    $MRProstateCare(gui,flipVolumeMenu) delete 0 end 
    foreach v $Volume(idList) {
        $MRProstateCare(gui,flipVolumeMenu) add command \
            -label [Volume($v,node) GetName] \
            -command "MRProstateCareSelectFlipVolume $v"
    }

    # for image1
    $MRProstateCare(gui,displayImage1VolumeMenu) delete 0 end 
    foreach v $Volume(idList) {
        $MRProstateCare(gui,displayImage1VolumeMenu) add command \
            -label [Volume($v,node) GetName] \
            -command "MRProstateCareSelectVolume 1 $v"
    }

    # for image2
    $MRProstateCare(gui,displayImage2VolumeMenu) delete 0 end 
    foreach v $Volume(idList) {
        $MRProstateCare(gui,displayImage2VolumeMenu) add command \
            -label [Volume($v,node) GetName] \
            -command "MRProstateCareSelectVolume 2 $v"
    }

    MRProstateCareUpdateRSA
}


proc MRProstateCareUpdateRSA {} {
    global MRProstateCare Volume

    if {$MRProstateCare(currentPoint) == "none"} {
        set MRProstateCare(xStr) 0.0
        set MRProstateCare(zStr) 0.0
        set MRProstateCare(yStr) 0.0
    } else {
        set coords [MRProstateCareGetRSAFromPoint $MRProstateCare(currentPoint)] 
        set MRProstateCare(xStr) [lindex $coords 0]
        set MRProstateCare(zStr) [lindex $coords 1]
        set MRProstateCare(yStr) [lindex $coords 2]
    }

    # save values
    foreach v "x z y" {
        set MRProstateCare(${v}Str,original) $MRProstateCare(${v}Str)
    }
} 


proc MRProstateCareZoom {} {
    global CurrentCamera
    global RendererFound
    global View Module
    global MRProstateCare 

    # No zoom for v = 1
    set v $MRProstateCare(scaleFactor)
    if {$v == 1} {
        return
    }

    if {$::View(parallelProjection) == 1} {
        if {$::Module(verbose)} { 
            puts "TkInteractor Zoom: parallel proj on = $::View(parallelProjection), no zooming allowed" 
        }
        return
    }

    if { ! $RendererFound } { return }

    if { [info exists ::FiducialEventHandled] && $::FiducialEventHandled } { 
        # special flag to handle point widget - TODO: generalize
        set ::FiducialEventHandled 0
        return
    }

    set y [expr {$v * 6.0}]
    set zoomFactor [expr pow(1.02,$y)]
    set clippingRange [$CurrentCamera GetClippingRange]
    set minRange [lindex $clippingRange 0]
    set maxRange [lindex $clippingRange 1]

    $CurrentCamera SetClippingRange [expr $minRange / $zoomFactor] \
                                    [expr $maxRange / $zoomFactor]
    $CurrentCamera Dolly $zoomFactor
    
    if {[$CurrentCamera GetParallelProjection] == 1} {
        $CurrentCamera SetParallelScale \
            [expr [$CurrentCamera GetParallelScale] / $zoomFactor]
    }


    # Call each Module's "CameraMotion" routine
    #-------------------------------------------
    foreach m $Module(idList) {
        if {[info exists Module($m,procCameraMotion)] == 1} {
            if {$Module(verbose) == 1} {puts "CameraMotion: $m"}
            $Module($m,procCameraMotion)
        }
    }

    Render
}



