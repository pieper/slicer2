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
#   MeasureVolDisplayExtents
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module counts voxels, converts to mL, and outputs a file.
#  If the extent for the input volume is changed by the user, only
#  measures that part of the volume.
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
	    {$Revision: 1.3 $} {$Date: 2000/10/31 00:25:29 $}]
    
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
    #     Choices 
    #       SelectVolume
    #       Tabs *
    #   Bottom
    #     Basic *
    #       File
    #       Measure
    #     Advanced *
    #       Extents
    # Results
    #   Volume
    #   Output
    #     Label
    #     TextBox
    #     
    # Note: items marked with a * were built as
    # part of a TabbedFrame (see tcl-shared/Widgets.tcl)
    #-------------------------------------------
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------

    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The <B>MeasureVol</B> module measures the volume of segmented structures in a labelmap.  
    It outputs a file containing the total volume in milliliters for each label value.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Measure:</B> First enter a filename.  Then click the 'Measure' button
    to measure the volumes and output your file. 
    <LI><B>Results:</B> Go to 'Results' to see the output file.
    <BR>
    <BR>
    <B>Note for advanced users:</B> 
    The 'Advanced' Settings (under 'Measure') 
    let you measure only part of a volume.  
    The numbers are the number of voxels that will be measured along each axis.
    Normally these numbers will be set automatically to measure the entire volume.
    <BR>
    <B>Example:</B> To measure only slice number 10 (counting from 0 like 
    the numbers above the slices do), set the numbers to 0 255, 0 255, and 
    10 10.  
    The first four numbers say to measure entire (256x256) slices, 
    and the last two numbers say to just measure the 10th slice.
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

    frame $f.fTop -bg $Gui(activeWorkspace)
    pack $f.fTop -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

    # fBottom needs height 310 to put a TabbedFrame inside!
    frame $f.fBottom -bg $Gui(activeWorkspace) -height 310
    pack $f.fBottom -side top -padx $Gui(pad) -pady $Gui(pad) -fill x


    #-------------------------------------------
    # Setup->Top frame
    #-------------------------------------------
    set f $fSetup.fTop
    
    foreach frame "Choices" {
	frame $f.f$frame -bg $Gui(backdrop) -relief sunken -bd 2
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Setup->Top->Choices frame
    #-------------------------------------------
    
    set f $fSetup.fTop.fChoices
    
    foreach frame "SelectVolume Tabs" {
	frame $f.f$frame -bg $Gui(backdrop)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Setup->Top->Choices->SelectVolume frame
    #-------------------------------------------

    set f $fSetup.fTop.fChoices.fSelectVolume

    # Add menus that list volumes
    # The selected volume will become $Volume(activeID)
    DevAddSelectButton  MeasureVol $f VolumeSelect "Volume:" Pack \
	    "Volume to measure segmented regions of." 20 BLA

    # bind menubutton to update the extents.
    bind $MeasureVol(mVolumeSelect) <ButtonRelease-1> \
	    "MeasureVolDisplayExtents" 
    # have this binding execute after menu updates active volume
    bindtags $MeasureVol(mVolumeSelect) [list Menu \
	    $MeasureVol(mVolumeSelect) all]

    # Append menus and buttons to lists that get refreshed during UpdateMRML
    lappend Volume(mbActiveList) $f.mbVolumeSelect
    lappend Volume(mActiveList) $f.mbVolumeSelect.m


    #-------------------------------------------
    # Setup->Top->Choices->Tabs frame
    #-------------------------------------------

    set f $fSetup.fTop.fChoices.fTabs

    # this makes the navigation menu (buttons) and the tabs (frames).
    TabbedFrame MeasureVol $f $fSetup.fBottom "Settings:"\
	    {Basic Advanced} {"Basic" "Advanced"} \
	    {"Basic Settings" "The Advanced Settings let you measure only part of the volume."}

    #-------------------------------------------
    # Setup->Bottom frame
    #-------------------------------------------
    set f $fSetup.fBottom
    
    # the frames inside this one were made with the TabbedFrame command above.

    #-------------------------------------------
    # Setup->Bottom->Basic frame
    #-------------------------------------------
    set f $fSetup.fBottom.fBasic

    foreach frame "File Measure" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Setup->Bottom->Basic->File frame
    #-------------------------------------------
    set f $fSetup.fBottom.fBasic.fFile
    
    DevAddFileBrowse $f MeasureVol fileName "Output File:" [] "txt" [] \
	    "Save" "Output File" "Choose the file where the output will be written."

    #-------------------------------------------
    # Setup->Bottom->Basic->Measure frame
    #-------------------------------------------
    set f $fSetup.fBottom.fBasic.fMeasure
    
    # Measure button
    DevAddButton $f.bMeasure "Measure Volume" MeasureVolVolume 
    
    TooltipAdd $f.bMeasure "Measure segmented regions in a volume and write the output to a file."
    
    pack $f.bMeasure -side top -padx $Gui(pad) -pady $Gui(pad)
    

    #-------------------------------------------
    # Setup->Bottom->Advanced frame
    #-------------------------------------------
    set f $fSetup.fBottom.fAdvanced

    foreach frame "Extents" {
	frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 3
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Setup->Bottom->Advanced->Extents frame
    #-------------------------------------------
    set f $fSetup.fBottom.fAdvanced.fExtents
    
    frame $f.fLabel -bg $Gui(activeWorkspace)
    pack $f.fLabel -side top -padx $Gui(pad) -pady $Gui(pad)
    eval {label $f.lExtents -text "Extent of volume to measure:"} $Gui(WLA)
    pack $f.lExtents -side top -padx $Gui(pad)  -pady $Gui(pad)

    # the 6 IJK extent entry boxes.
    foreach {label n1 n2} {I 1 2   J 3 4   K 5 6} {
	frame $f.fExt$n1 -bg $Gui(activeWorkspace)
	pack $f.fExt$n1 -side top -padx 0 -pady $Gui(pad)

	DevAddLabel $f.fExt$n1.lE$n1 "$label:"
	pack $f.fExt$n1.lE$n1 -side left -padx $Gui(pad)  -pady $Gui(pad)

	# add entry box for variable MeasureVol(Extent1), etc.
	DevAddEntry MeasureVol Extent$n1 $f.fExt$n1.eE$n1
	pack $f.fExt$n1.eE$n1 -side left -padx $Gui(pad)  -pady $Gui(pad)

	DevAddEntry MeasureVol Extent$n2 $f.fExt$n1.eE$n2
	pack $f.fExt$n1.eE$n2 -side left -padx $Gui(pad)  -pady $Gui(pad)
    }


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

    MeasureVolDisplayExtents
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
    
    # validate input from Basic tab
    if {$MeasureVol(fileName) == ""} {
	DevErrorWindow "Please enter a filename first."
	return
    }
    
    # validate input from Advanced tab
    set okay 1
    for {set i 1} {$i < 7} {incr i} {
	if {[ValidateInt $MeasureVol(Extent$i)] == 0} {
	    set okay 0
	}    
    }
    if {$okay == 0} {
	tk_messageBox -message \
		"The extent numbers (under the Advanced Settings tab) must be integers."
	return
    }    

    # clear the text box.
    $MeasureVol(textBox) delete 1.0 end

    # currently selected volume
    set v $Volume(activeID)
    
    # if the user has changed the full extent in the entry
    # boxes, clip and only measure part of the volume.
    vtkImageClip clip
    clip SetInput [Volume($v,vol) GetImageData]
    clip SetOutputWholeExtent  $MeasureVol(Extent1) \
	    $MeasureVol(Extent2)  $MeasureVol(Extent3) \
	    $MeasureVol(Extent4)  $MeasureVol(Extent5) \
	    $MeasureVol(Extent6)
    clip ClipDataOn
    clip ReleaseDataFlagOff

    # pipeline
    vtkImageMeasureVoxels measure
    #measure SetInput [Volume($v,vol) GetImageData]
    measure SetInput [clip GetOutput]
    measure SetFileName $MeasureVol(fileName)
    measure Update

    # delete objects we created
    measure Delete
    clip Delete
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

#-------------------------------------------------------------------------------
# .PROC MeasureVolDisplayExtents
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MeasureVolDisplayExtents {} {

    global MeasureVol Volume

    # currently selected volume
    set v $Volume(activeID)

    # set extent to that of the current volume.
    # this sets the entry boxes too.
    scan [[Volume($v,vol) GetImageData] GetWholeExtent] \
	    "%d %d %d %d %d %d" \
	    MeasureVol(Extent1) MeasureVol(Extent2) MeasureVol(Extent3) \
	    MeasureVol(Extent4) MeasureVol(Extent5) MeasureVol(Extent6)
}

