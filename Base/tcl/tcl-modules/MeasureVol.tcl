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
# FILE:        MeasureVol.tcl
# PROCEDURES:  
#   MeasureVolInit
#   MeasureVolBuildGUI
#   MeasureVolEnter
#   MeasureVolExit
#   MeasureVolVolume
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module counts voxels, converts to mL, and outputs a file.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are the variables defined by this module.
# 
#  list MeasureVol(eventManager)  list of event bindings used by this module
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC MeasureVolInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MeasureVolInit {} {
    global MeasureVol Module Volume Model
    
    # Define Tabs
    #------------------------------------
    set m MeasureVol
    set Module($m,row1List) "Help Setup Results"
    set Module($m,row1Name) "{Help} {Measure} {Results}"
    set Module($m,row1,tab) Setup

    # Define Procedures
    #------------------------------------
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
    set Module($m,procGUI) MeasureVolBuildGUI
    set Module($m,procEnter) MeasureVolEnter
    set Module($m,procExit) MeasureVolExit

    # Define Dependencies
    #------------------------------------
    set Module($m,depend) ""
    
    # Set version info
    #------------------------------------
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.2 $} {$Date: 2000/10/27 20:49:17 $}]
    
    # Initialize module-level variables
    #------------------------------------
    set MeasureVol(fileName)  ""
    set MeasureVol(eventManager)  ""
}

#-------------------------------------------------------------------------------
# .PROC MeasureVolBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc MeasureVolBuildGUI {} {
    global Gui MeasureVol Module Volume Model
    
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Setup
    #   Top
    #     ActiveVolume
    #   Bottom
    #     File
    #     Measure
    # Results
    #   Volume
    #   Output
    #     Label
    #     TextBox
    #-------------------------------------------
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------

    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The MeasureVol module measures the volume of segmented structures in a labelmap.  
    It outputs a file containing the total volume in milliliters for each label value.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Measure:</B> First enter a filename.  Then click the Measure Button
    to measure the volumes and output your file.
    <LI><B>Results:</B> Go to 'Results' to see the output file.
    "
    regsub -all "\n" $help {} help
    # remove emacs indentation
    regsub -all "   " $help {} help
    MainHelpApplyTags MeasureVol $help
    MainHelpBuildGUI MeasureVol
    
    ##########################################################
    #             Setup
    ##########################################################

    #-------------------------------------------
    # Setup frame
    #-------------------------------------------
    set fSetup $Module(MeasureVol,fSetup)
    set f $fSetup
    
    foreach frame "Top Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Setup->Top frame
    #-------------------------------------------
    set f $fSetup.fTop
    
    foreach frame "ActiveVolume" {
	frame $f.f$frame -bg $Gui(backdrop) -relief sunken -bd 2
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Setup->Top->ActiveVolume frame
    #-------------------------------------------
    
    set f $fSetup.fTop.fActiveVolume
    
    # Add menus that list volumes
    # The selected volume will become $Volume(activeID)
    DevAddSelectButton  MeasureVol $f VolumeSelect "Volume:" Grid \
	    "Volume to measure segmented regions of." 20 BLA
    
    # Append menus and buttons to lists that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbVolumeSelect
    lappend Volume(mActiveList) $f.mbVolumeSelect.m
    
    #-------------------------------------------
    # Setup->Bottom frame
    #-------------------------------------------
    set f $fSetup.fBottom
    
    foreach frame "File Measure" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Setup->Bottom->File frame
    #-------------------------------------------
    set f $fSetup.fBottom.fFile
    
    DevAddFileBrowse $f MeasureVol fileName "Output File:" [] "txt" [] \
	    "Save" "Output File" "Choose the file where the output will be written."
    
    #-------------------------------------------
    # Setup->Bottom->Measure frame
    #-------------------------------------------
    set f $fSetup.fBottom.fMeasure
    
    # Measure button
    DevAddButton $f.bMeasure "Measure Volume" MeasureVolVolume 
    
    TooltipAdd $f.bMeasure "Measure segmented regions in a volume and write the output to a file."
    
    pack $f.bMeasure -side top -padx $Gui(pad) -pady $Gui(pad)
    
    ##########################################################
    #             Results
    ##########################################################

    #-------------------------------------------
    # Results frame
    #-------------------------------------------
    set fResults $Module(MeasureVol,fResults)
    set f $fResults

    frame $f.fVolume -bg $Gui(activeWorkspace) 
    pack $f.fVolume -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    frame $f.fOutput -bg $Gui(activeWorkspace) -relief groove -bd 3   
    pack $f.fOutput -side top -padx $Gui(pad) -pady $Gui(pad) -fill x 

    #-------------------------------------------
    # Results->Volume frame
    #-------------------------------------------
    set f $fResults.fVolume

    DevAddLabel $f.lResultVol ""
    pack $f.lResultVol -side top -padx $Gui(pad)  -pady $Gui(pad) -fill x
    set MeasureVol(lResultVol)  $f.lResultVol

    
    #-------------------------------------------
    # Results->Output frame
    #-------------------------------------------
    set f $fResults.fOutput

    foreach frame "Label  TextBox" {
	frame $f.f$frame -bg $Gui(activeWorkspace) 
	pack $f.f$frame -side top -padx $Gui(pad) -pady 0 -fill x
    }

    #-------------------------------------------
    # Results->Output->Label frame
    #-------------------------------------------
    set f $fResults.fOutput.fLabel

    DevAddLabel $f.lTextBox "  Label\t\tVolume in mL"
    pack $f.lTextBox -side left -padx $Gui(pad)  -pady $Gui(pad)
    set MeasureVol(lTextBox) $f.lTextBox

    #-------------------------------------------
    # Results->Output->TextBox frame
    #-------------------------------------------
    set f $fResults.fOutput.fTextBox
    set MeasureVol(textBox) [ScrolledText $f.tText -width 40 -height 8]
    pack $f.tText -side top -fill both -expand true
    
}

#-------------------------------------------------------------------------------
# .PROC MeasureVolEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MeasureVolEnter {} {
    global MeasureVol
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $MeasureVol(eventManager)

}

#-------------------------------------------------------------------------------
# .PROC MeasureVolExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MeasureVolExit {} {

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
# .PROC MeasureVolVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MeasureVolVolume {} {
    global MeasureVol Volume Module
    
    # validate inputs
    if {$MeasureVol(fileName) == ""} {
	DevErrorWindow "Please enter a filename first."
	return
    }
    
    # clear the text box.
    $MeasureVol(textBox) delete 1.0 end
    
    puts "Lauren clip volume to measure parts and make a UI for this!"

    # currently selected volume
    set v $Volume(activeID)

    # pipeline
    vtkImageMeasureVoxels measure
    measure SetInput [Volume($v,vol) GetImageData]
    measure SetFileName $MeasureVol(fileName)
    measure Update

    # delete objects we created
    measure Delete

    puts "Lauren check file exists"

    # display the output that was written to the file.
    set in [open $MeasureVol(fileName)]
    $MeasureVol(textBox) insert end [read $in]
    close $in

    # label output in Results frame
    $MeasureVol(lResultVol) configure -text \
	    "Results for [Volume($v,node) GetName] volume:"

    # tab to Results frame if desired
    YesNoPopup test1 100 100 \
	    "Done measuring volume [Volume($v,node) GetName].\nView output file?" \
	    {Tab MeasureVol row1 Results}

}

