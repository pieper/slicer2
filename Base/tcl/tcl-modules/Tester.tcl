#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        Tester.tcl
# PROCEDURES:  
#   TesterInit
#   TesterBuildGUI
#   TesterEnter
#   TesterExit
#   TesterSourceModule Module type
#   TesterRebuildGui ModuleName
#   TesterRebuildGui
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module is an example for developers.  It shows how to add a module 
#  to the Slicer.  To find it when you run the Slicer, click on More->Tester.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are the variables defined by this module.
# 
#  int Tester(count) counts the button presses for the demo 
#  list Tester(eventManager)  list of event bindings used by this module
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC TesterInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TesterInit {} {
	global Tester Module Volume Model

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
	#   
	#   row1List = list of ID's for tabs. (ID's must be unique single words)
	#   row1Name = list of Names for tabs. (Names appear on the user interface
	#              and can be non-unique with multiple words.)
	#   row1,tab = ID of initial tab
	#   row2List = an optional second row of tabs if the first row is too small
	#   row2Name = like row1
	#   row2,tab = like row1 
	#
	set m Tester
	set Module($m,row1List) "Help Source Watch"
        set Module($m,row1Name) "{Help} {Source} {Watch}"
	set Module($m,row1,tab) Source

	# Define Procedures
	#------------------------------------
	# Description:
	#   The Slicer sources all *.tcl files, and then it calls the Init
	#   functions of each module, followed by the VTK functions, and finally
	#   the GUI functions. A MRML function is called whenever the MRML tree
	#   changes due to the creation/deletion of nodes.
	#   
	#   While the Init procedure is required for each module, the other 
	#   procedures are optional.  If they exist, then their name (which
	#   can be anything) is registered with a line like this:
	#
	#   set Module($m,procVTK) TesterBuildVTK
	#
	#   All the options are:
	#
	#   procGUI   = Build the graphical user interface
	#   procVTK   = Construct VTK objects
	#   procMRML  = Update after the MRML tree changes due to the creation
	#               of deletion of nodes.
	#   procEnter = Called when the user enters this module by clicking
	#               its button on the main menu
	#   procExit  = Called when the user leaves this module by clicking
	#               another modules button
	#   procStorePresets  = Called when the user holds down one of the Presets
	#               buttons.
	#   procRecallPresets  = Called when the user clicks one of the Presets buttons
	#               
	#   Note: if you use presets, make sure to give a preset defaults
	#   string in your init function, of the form: 
	#   set Module($m,presets) "key1='val1' key2='val2' ..."
	#   
	set Module($m,procGUI) TesterBuildGUI
	set Module($m,procEnter) TesterEnter
	set Module($m,procExit) TesterExit

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
		{$Revision: 1.1 $} {$Date: 2000/08/08 15:44:48 $}]

	# Initialize module-level variables
	#------------------------------------
	# Description:
	#   Keep a global array with the same name as the module.
	#   This is a handy method for organizing the global variables that
	#   the procedures in this module and others need to access.
	#
        set Tester(SourceFileName) ""
	set Tester(ModuleFileName) ""
	set Tester(MainFileName)   ""
	set Tester(SharedFileName) ""

        set Tester(MainModuleList)   $Module(mainList)
        set Tester(SharedModuleList) $Module(sharedList) 
        # Include Suppressed Modules
        set Tester(ModuleModuleList) $Module(allList)

        set Tester(Count) 0
	set Tester(eventManager)  ""
}


#-------------------------------------------------------------------------------
# .PROC TesterBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc TesterBuildGUI {} {
        global Gui Tester Module Volume Model

        # A frame has already been constructed automatically for each tab.
        # A frame named "Stuff" can be referenced as follows:
        #   
        #     $Module(<Module name>,f<Tab name>)
        #
        # ie: $Module(Tester,fStuff)

        # This is a useful comment block that makes reading this easy for all:
        #-------------------------------------------
        # Frame Hierarchy:
        #-------------------------------------------
        # Help
        # Source
        #   SourceText
        #   Browse
        #   Main
        #   Module
        #   Shared
        #   Bottom
        # Watch
        #-------------------------------------------

        #-------------------------------------------
        # Help frame
        #-------------------------------------------

        # Write the "help" in the form of psuedo-html.  
        # Refer to the documentation for details on the syntax.
        #
        set help " The Tester module is for developers only.  It 
 allows the developer to re-source code as changes are made to it,
 without restarting the slicer. 
<BR><BR>
For modules, GUI's are re-made so
that one can easily tweak a user-interface and until the results look
nice. 
<BR><BR>
 A good thing to add would be a \"Watch\" that allows the developer to watch the values of selected variables. But, I haven't done it yet.<p>

Note that if you re-source the Tester, the Tester windows will get screwed
up. To fix this, simply exit and enter the Tester. "
        regsub -all "\n" $help {} help
        MainHelpApplyTags Tester $help
        MainHelpBuildGUI Tester

        #-------------------------------------------
        # Source frame
        #-------------------------------------------
        set fSource $Module(Tester,fSource)
        set f $fSource

        foreach frame "Browse Main Module Shared Bottom" {
                frame $f.f$frame -bg $Gui(activeWorkspace)
                pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
        }

        #-------------------------------------------
        # Source->Browse frame
        #-------------------------------------------
        set f $fSource.fBrowse


        DevAddFileBrowse $f Tester SourceFileName "File to Source:" "source \$Tester\(SourceFileName\)" "tcl" ""  "Browse for a new module" 

        #-------------------------------------------
        # Source->Main frame
        #-------------------------------------------
        set f $fSource.fMain
        DevAddLabel $f.lSource "Or, Source an existing Module:"
        pack $f.lSource -side top -padx $Gui(pad) -fill x -pady $Gui(pad)
        DevAddSelectButton Tester $f MainModules  "Main" Pack

        set f $fSource.fModule

        DevAddSelectButton Tester $f ModuleModules  "Module" Pack

        set f $fSource.fShared

        DevAddSelectButton Tester $f SharedModules  "Shared" Pack

        #-------------------------------------------
        # Source->Bottom frame
        #-------------------------------------------
        set f $fSource.fBottom

        eval {label $f.lSource -text ""} $Gui(BLA)
        pack $f.lSource -side top -padx $Gui(pad) -fill x
        set Tester(lSource) $f.lSource

#        pack $f.bCount $f.bCount $f.eCount -side left -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC TesterEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TesterEnter {} {
    global Tester Module
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $Tester(eventManager)

    # Refresh the Module Lists
    set Tester(MainModuleList)   $Module(mainList)
    set Tester(SharedModuleList) $Module(sharedList) 
    # Include Suppressed Modules
    set Tester(ModuleModuleList) $Module(allList)

 DevUpdateSelectButton Tester MainModules MainFileName   MainModuleList \
         "TesterSourceModule Main"
 DevUpdateSelectButton Tester ModuleModules ModuleFileName ModuleModuleList \
         "TesterSourceModule Module"
 DevUpdateSelectButton Tester SharedModules SharedFileName SharedModuleList \
         "TesterSourceModule Shared"
}

#-------------------------------------------------------------------------------
# .PROC TesterExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TesterExit {} {

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
# .PROC TesterSourceModule
# Source a module.  Use the usual Slicer search order.
# .ARGS
# str Module  The module name
# str type    either Main, Shared, or Module
# .END
#-------------------------------------------------------------------------------
proc TesterSourceModule {type Module} {
   global Tester

    if {$Module == ""} return

    ## Expand the path name of each type differently

    if {$type == "Main"} { 
        set path [GetFullPath $Module tcl tcl-main] 
    }
    if {$type == "Shared"} { 
        set path [GetFullPath $Module tcl tcl-shared] 
    }

    if {$type == "Module"} { 
	set path [GetFullPath $Module tcl tcl-modules]
    }

    ## Source the file

    if { $path != "" } { 
        source $path 
    } else {
        DevWarningWindow "Didn't find that module!"
	$Tester(lSource) config -text ""
        Return
    }

    ## Rebuild Gui on Modules

    if {$type == "Module"} { 
	TesterRebuildGui $Module
    }

    ## Send message that we update Stuff.
    if {$Module != "Tester"} {
	$Tester(lSource) config -text "Updated $Module."
    }
}
#-------------------------------------------------------------------------------
# .PROC TesterRebuildGui
# 
# Erase the old Gui and rebuild a new one.
#
# .ARGS
# str ModuleName The name of the module
# .END
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC TesterRebuildGui
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc TesterRebuildGui {ModuleName} {
    global Module Gui

    if {[info exists Module($ModuleName,row1List)] == 1} {
        foreach frame $Module($ModuleName,row1List) {
            set f $Module($ModuleName,f$frame)
            catch {destroy $f}
        }
    }

    if {[info exists Module($ModuleName,row1List)] == 1} {
        foreach frame $Module($ModuleName,row2List) {
            set f $Module($ModuleName,f$frame)
            catch {destroy $f}
        }
    }

    set m $ModuleName
    set fWork .tMain.fControls.fWorkspace
    set f .tMain.fControls.fTabs

    catch {destroy $f.f${m}row1}
    catch {destroy $f.f${m}row2}
    MainBuildModuleTabs $ModuleName
    $Module($ModuleName,procGUI)
#    $Module(Tester,procEnter)
#    Tab Tester row1 Source
}
