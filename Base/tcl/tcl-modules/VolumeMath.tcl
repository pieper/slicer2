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
# FILE:        VolumeMath.tcl
# PROCEDURES:  
#   VolumeMathInit
#   VolumeMathUpdateGUI
#   VolumeMathBuildGUI
#   VolumeMathBuildResampParamPopup
#   VolumeMathShowPopup
#   VolumeMathUpdateInterpParams
#   VolumeMathCancelInterpParams
#   VolumeMathGetInitParams
#   VolumeMathEnter
#   VolumeMathExit
#   VolumeMathSetMathType
#   VolumeMathSetLogicType
#   VolumeMathPrepareResultVolume logic
#   VolumeMathPrepareResult
#   VolumeMathDoMath
#   VolumeMathDoLogic
#   VolumeMathDoSubtract
#   VolumeMathDoAdd
#   VolumeMathDoHausdorff
#   VolumeMathDoStatistics
#   VolumeMathDoDistMap
#   VolumeMathDoAbs
#   VolumeMathDoResample
#   VolumeMathDoAnd
#   VolumeMathDoMultiply
#   VolumeMathDoMask
#==========================================================================auto=
#-------------------------------------------------------------------------------
# .PROC VolumeMathInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathInit {} {
    global VolumeMath Module Volume

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
    set m VolumeMath
    set Module($m,row1List) "Help Math Distance Logic"
    set Module($m,row1Name) "{Help} {Math} {Distance} {Logic}"
    set Module($m,row1,tab) Math

    # Module Summary Info
    #------------------------------------
    # Description:
    #   This summary info will be displayed on the main menu
    #   under Help->Module Summaries
    #   and under Help->Module Credits
    set Module($m,overview) "Math on volumes: addition, etc."
    set Module($m,author) "Samson Timoner, MIT AI Lab"

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
    #   set Module($m,procVTK) VolumeMathBuildVTK
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
    set Module($m,procGUI)   VolumeMathBuildGUI
    set Module($m,procMRML)  VolumeMathUpdateGUI
    set Module($m,procEnter)  VolumeMathEnter


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
    #   appropriate info when the module is checked in.
    #   
        lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.34 $} {$Date: 2003/08/11 16:34:21 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #

        # Volume3 = Volume2 operation Volume1
    set VolumeMath(Volume1) $Volume(idNone)
    set VolumeMath(Volume2) $Volume(idNone)
    set VolumeMath(Volume3) $Volume(idNone)

    # For Distance Calculations
    set VolumeMath(CurrentPoint) 2

    # Coordinates, in mm, of selected points 2 and 1
    set VolumeMath(Point1x)  0
    set VolumeMath(Point1y)  0
    set VolumeMath(Point1z)  0
    set VolumeMath(Point2x)  0
    set VolumeMath(Point2y)  0
    set VolumeMath(Point2z)  0
    # Difference of Point2 - Point1
    set VolumeMath(Differencex)  0
    set VolumeMath(Differencey)  0
    set VolumeMath(Differencez)  0
    # Magnitude of the difference
    set VolumeMath(Magnitude)  0

    #Hanifa 
    #Set the default interpolation mode to linear
    set VolumeMath(interpolationMode) Linear
    #A check to see if the popup box is open
    set VolumeMath(resampMenuOpen) 0
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathUpdateGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathUpdateGUI {} {
    global VolumeMath Volume

    #Hanifa
    #I changed the following line so that the GUI picks up the initial extent origin and spacing
    #values as soon as they are selected on the menu. This also required a change in Developer.tcl
    DevUpdateNodeSelectButton Volume VolumeMath Volume1 Volume1 DevSelectNode 1 0 1 VolumeMathGetInitParams
    DevUpdateNodeSelectButton Volume VolumeMath Volume2 Volume2 DevSelectNode 
    DevUpdateNodeSelectButton Volume VolumeMath Volume3 Volume3 DevSelectNode 0 1 1
    
    # for the Logic buttons
    # these have to not be named the same as the above since Developer.tcl
    # stores the widget path in VolumeMath(mbVolume1) so creating
    # the second menu like this will overwrite the first's saved
    # widget path...
    DevUpdateNodeSelectButton Volume VolumeMath Volume1L Volume1 DevSelectNode 
    DevUpdateNodeSelectButton Volume VolumeMath Volume2L Volume2 DevSelectNode
    DevUpdateNodeSelectButton Volume VolumeMath Volume3L Volume3 DevSelectNode 0 1 1
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathBuildGUI {} {
    global Gui VolumeMath Module Label

    #Hanifa
    #The following popup allows the user to see the currently set volumes for resampling
    #ie.origin, extent and spacing values. This GUI allows the user to input new values.
    #-------------------------------------------
    # Popup->Options frame for Resampling 
    #-------------------------------------------
    VolumeMathBuildResampParamPopup

       # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(VolumeMath,fStuff)

    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # fHelp
        # fMath
        #       fSelectMath
    #    fGrid
    #        mbVolume1
    #        mbVolume2
    #        mbVolume3
    #    fPack
    #        bRun
    # fDistance
        #       fGrid
        #           rPoint1
        #            ePoint1x
        #            ePoint1y
        #            ePoint1z
        #           rPoint2
        #            ePoint2x
        #            ePoint2y
        #            ePoint2z
        #           ldifference
        #            eDifferencex
        #            eDifferencey
        #            eDifferencez
        #           lMagnitude
        #            eMagnitude
        #    fPack
        #           ltop
        # fLogic
        #       fSelectMath
    #    fGrid
    #        mbVolume1
    #        mbVolume2
    #        mbVolume3
    #    fPack
    #        bRun
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------

    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
This Module Exists to do things like subtract and add volumes.
It also exists to find the distance between two points in a volume.
Currently, the distance finder is not functional.

<P><B>Output volume:</B> The output volume will be called
VolumeMath*Result, where * is the name of the mathematical operation
you have done to make the volume.  This name can be changed under
Volumes->Props->Basic if desired.
For Resampling, the type of volume 2 (greyscale or labelmap) will 
determine the type of the output volume. If you wish to resample a 
labelmap into the space of a grey scale image, you will need a 
label map already in the grey scale image space to serve as V2 in 
order for the output of the resampling to be a label map.

<P>
<B>For Subtraction or Addition</B>: If you wish to subtract two
volumes that have different spacings or numbers of voxels, or if a
transform exists between the two images, you MUST resample one image
in the coordinates of the other. (Note that this module doesn't
currently check to make sure you did everything correctly. One day...)
Once the images are subtracted, the results are sometimes difficult to
look at. I recommend taking the Absolute Value of the results of the
subtraction. 

<P>
Distance Maps yield the square of the distance.

<P>
<B>Logic Tab</B>: This is for logical operations on volumes, such as AND.
Practically, the AND function can be used to select all voxels that
are labeled in both of two volumes.  

<P>For example, if you wish to threshold only within a segmented
region, use the Editor to produce two Working volumes.  The first
Working volume is a labelmap of the desired region.  The second
Working volume is made by applying the desired threshold (to the whole
volume).

<P>Then you can use the VolumeMath module to create a new volume where
the thresholding is restricted to the segmented area.  Do this by
selecting Working1 and Working2 as Volumes 1 and 2 under
VolumeMath->Logic->And.  Then for Volume3, select Create New.  Make
sure to select the label of interest in the Working volumes.  The new
output volume will be thresholded only within the region of the
original segmentation.

<P><B>Known Bugs</B> Don't set the output to be one of the input
files. Sometimes it doesn't work.
"
    regsub -all "\n" $help " " help
    MainHelpApplyTags VolumeMath $help
    MainHelpBuildGUI VolumeMath

    #-------------------------------------------
    # Math frame
    #-------------------------------------------

    set fMath $Module(VolumeMath,fMath)
    set f $fMath

    frame $f.fSelectMath  -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fGrid -bg $Gui(activeWorkspace)
    #Hanifa
    frame $f.fResampButton -bg $Gui(activeWorkspace)
    #sp - 2003-08-11
    frame $f.fMaskLabel -bg $Gui(activeWorkspace)
    frame $f.fPack -bg $Gui(activeWorkspace)

  
    pack $f.fSelectMath $f.fGrid $f.fResampButton $f.fMaskLabel $f.fPack  -side top -padx 0 -pady $Gui(pad)

    #-------------------------------------------
    # Math->SelectMath
    #-------------------------------------------
    set f $fMath.fSelectMath

    frame $f.f -bg $Gui(backdrop)

    # the first row and second row
    frame $f.f.1 -bg $Gui(inactiveWorkspace)
    frame $f.f.2 -bg $Gui(inactiveWorkspace)
    frame $f.f.3 -bg $Gui(inactiveWorkspace)
    pack $f.f.1 $f.f.2 $f.f.3 -side top -fill x -anchor w

    #
    # NOTE: As you want more functions, don't forget
    #       to add more rows above.
    #

    set row 1
    foreach p "Subtract Add Resample Abs DistMap Hausdorff Multiply Statistics Mask" {
        eval {radiobutton $f.f.$row.r$p \
            -text "$p" -command "VolumeMathSetMathType" \
            -variable VolumeMath(MathType) -value $p -width 10 \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.$row.r$p -side left -pady 0
        if { $p == "Resample" || $p == "Hausdorff" } {incr row};
    }

    pack $f.f -side left -padx $Gui(pad) -fill x -anchor w

    set VolumeMath(MathType) Subtract

    #-------------------------------------------
    # Math->Grid frame
    #-------------------------------------------

    set f $fMath.fGrid

    DevAddSelectButton VolumeMath $f Volume2 "Volume2:"   Grid
    DevAddSelectButton VolumeMath $f Volume1 "- Volume1:" Grid
    DevAddSelectButton VolumeMath $f Volume3 "= Volume3:" Grid

    #-------------------------------------------
    # Math->Pack frame
    #-------------------------------------------

    set f $fMath.fPack

    DevAddButton $f.bRun "Run" "VolumeMathDoMath"

    pack $f.bRun

    #Hanifa 
    #-------------------------------------------
    # Math->Resampling Button 
    #-------------------------------------------
    set f $fMath.fResampButton

    DevAddButton $f.bResampParamPopup "View/change Resampling Params" \
         "set VolumeMath(resampMenuOpen) 1; VolumeMathShowPopup"

    #Save the path to the widget so that it can be accessed later
    set VolumeMath(ResampParamButton) $f.bResampParamPopup

    #sp - 2003-08-11 
    #-------------------------------------------
    # Math->Mask Label 
    #-------------------------------------------
    set f $fMath.fMaskLabel

    # Mask label
    eval {button $f.bLabel -text "Mask:" \
        -command "ShowLabels VolumeMathSetMaskLabel"} $Gui(WBA)
    eval {entry $f.eLabel -width 6 \
        -textvariable ::Label(label)} $Gui(WEA)
    bind $f.eLabel <Return>   "VolumeMathSetMaskLabel"
    bind $f.eLabel <FocusOut> "VolumeMathSetMaskLabel"
    eval {entry $f.eName -width 14 \
        -textvariable ::Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bLabel $f.eLabel $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eLabel $f.eName -sticky w
    #Save the path to the widget so that it can be accessed later
    set VolumeMath(maskLabelFrame) $f
    pack forget $VolumeMath(maskLabelFrame)

#        #-------------------------------------------
#        # Resample frame
#        #-------------------------------------------
#
#        set fResamp $Module(VolumeMath,fResample)
#        set f $fResamp
#
#        frame $f.fGrid -bg $Gui(activeWorkspace)
#        frame $f.fPack -bg $Gui(activeWorkspace)
#        pack $f.fGrid $f.fPack -side top -padx 0 -pady $Gui(pad)
#
#        #-------------------------------------------
#        # Resample->Grid frame
#        #-------------------------------------------
#        set f $fResamp.fGrid
#
#        DevAddSelectButton VolumeMath $f Volume2 "Resample" Grid
#        DevAddSelectButton VolumeMath $f Volume1 "in the coordinates of" Grid
#        DevAddSelectButton VolumeMath $f Volume3 "and put the results in" Grid
#
#        #-------------------------------------------
#        # Resample->Pack frame
#        #-------------------------------------------
#
#        set f $fResamp.fPack
#
#        DevAddButton $f.bRun "Run" "VolumeMathDoResample" 
#
#        pack $f.bRun

    #-------------------------------------------
    # Distance Frame
    #-------------------------------------------

        set fDistance $Module(VolumeMath,fDistance)
        set f $fDistance

#### Junk that I hope will turn into the getting info from a mouse.
#        # VolumeMathDistance
#                # Get RAS coordinates
#                Slicer SetReformatPoint $s $x $y
#                scan [Slicer GetRasPoint] "%g %g %g" xRas yRas zRas
##        ViewSetFocalPoint $xRas $yRas $zRas
#
#                # Zoom
#                Slicer SetZoomCenter $s $x $y
#
#                RenderAll
#                VolumeMathUpdatePoint $xRas $yRas $zRas 
#
#
#        # VolumeMathDistance
#        "VolumeMathDistance" {
#                # Get RAS coordinates
#                Slicer SetReformatPoint $s $x $y
#                scan [Slicer GetRasPoint] "%g %g %g" xRas yRas zRas
##        ViewSetFocalPoint $xRas $yRas $zRas
#
#                SliceMouseAnno $xScr $yScr $x $y
#
#                # Zoom
#                Slicer SetZoomCenter $s $x $y
#
#                RenderAll
#                VolumeMathUpdatePoint $xRas $yRas $zRas 
#        }
#
#       global Slice
#        set fslice $Gui(fMid)
#
#
#        foreach s $Slice(idList) {
#                bind $f.fSlice$s <ButtonPress-1> "puts yo"
#                bind $f.fSlice$s <B1-Motion>     "puts yo"
#        }
#
#
#        bind 
#
#bind $VolumeMath(fNodeList) <Button-3>  {VolumeMathPostRightMenu %X %Y}
#bind $VolumeMath(fNodeList) <Double-1>  {VolumeMathEditNode}
#
#        # initialize key-bindings (and hide class Listbox Control button ops)
#        set VolumeMath(eventMgr) [subst { \
#                Listbox,<Control-Button-1>  {} \
#                Listbox,<Control-B1-Motion>  {} \
#                all,<Control-e> {VolumeMathEditNode} \
#                all,<Control-x> {VolumeMathCutNode} \
#                all,<Control-v> {VolumeMathPasteNode} \
#                all,<Control-d> {VolumeMathDeleteNode} }]
#

        frame $f.fGrid -bg $Gui(activeWorkspace)
        frame $f.fPack -bg $Gui(activeWorkspace)
        pack $f.fPack $f.fGrid -side top -padx 0 -pady $Gui(pad)

        eval  {label $f.fPack.ltop -text "Coordinates are RAS in mm" \
         } $Gui(WLA)

        pack $f.fPack.ltop

        set f $f.fGrid

    foreach i "2 1" {
                # Radio Button For Each Point
#        DevAddRadiobutton $f.rPoint$i "Point $i" VolumeMath(CurrentPoint) 

        eval  {radiobutton $f.rPoint$i -width 9  \
            -text "Point $i" -value $i    \
                        -variable VolumeMath(CurrentPoint) \
            -indicatoron 0 } $Gui(WCA)

                eval  {label $f.lPoint$i -textvariable VolumeMath(Point$i) \
                     } $Gui(WLA)

                # Get The Current Position of Each Point

                foreach dir "x y z" {
                    eval  {entry $f.ePoint$i$dir \
                            -textvariable VolumeMath(Point$i$dir) \
                            -width 6 } $Gui(WEA)
                }


                # Show Everything

                grid $f.rPoint$i $f.ePoint${i}x  $f.ePoint${i}y \
                     $f.ePoint${i}z -padx $Gui(pad) -pady $Gui(pad)
                grid $f.rPoint$i -sticky e 

        }

        # Show the Difference = Point2 - Point 1
        # First the label
        eval  {label $f.ldifference -text "Difference" } $Gui(WLA)

        # Now the difference itself

        foreach dir "x y z" {
            eval  {entry $f.edifference$dir \
                    -textvariable VolumeMath(Difference$dir) \
                    -width 6 } $Gui(WEA)
        }

         grid $f.ldifference $f.edifferencex  $f.edifferencey \
            $f.edifferencez -padx $Gui(pad) -pady $Gui(pad)
         grid $f.ldifference -sticky e 

       # Show the magnitude of the difference
       # First the label
        eval  {label $f.lMagnitude -text "Magnitude" } $Gui(WLA)

        eval  {entry $f.eMagnitude \
              -textvariable VolumeMath(Magnitude) \
              -width 6 } $Gui(WEA)

         grid $f.lMagnitude $f.eMagnitude -padx $Gui(pad) -pady $Gui(pad)
         grid $f.lMagnitude -sticky e 


    #-------------------------------------------
    # Logic frame
    #-------------------------------------------

    set fLogic $Module(VolumeMath,fLogic)
    set f $fLogic

    frame $f.fSelectMath  -bg $Gui(backdrop) -relief sunken -bd 2
    frame $f.fLabel -bg $Gui(activeWorkspace)
    frame $f.fGrid -bg $Gui(activeWorkspace)
    frame $f.fPack -bg $Gui(activeWorkspace)
    pack $f.fSelectMath $f.fLabel $f.fGrid $f.fPack -side top -padx 0 -pady $Gui(pad)

    #-------------------------------------------
    # Logic->SelectMath
    #-------------------------------------------
    set f $fLogic.fSelectMath

    frame $f.f -bg $Gui(backdrop)

    # the first row and second row
    frame $f.f.1 -bg $Gui(inactiveWorkspace)
    frame $f.f.2 -bg $Gui(inactiveWorkspace)
    pack $f.f.1 $f.f.2 -side top -fill x -anchor w

    #
    # NOTE: As you want more functions, don't forget
    #       to add more rows above.
    #

    set row 1
    set tips {"Label all pixels that are labeled in BOTH of the input Volumes (1 and 2).\nUseful for thresholding within a segmented region only."}
    foreach p "And" tip $tips {
            eval {radiobutton $f.f.$row.r$p \
            -text "$p" -command "VolumeMathSetLogicType" \
            -variable VolumeMath(LogicType) -value $p -width 10 \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.$row.r$p -side left -pady 0
        TooltipAdd $f.f.$row.r$p $tip
        #if { $p == "Resample" } {incr row};
    }

    pack $f.f -side left -padx $Gui(pad) -fill x -anchor w

        set VolumeMath(LogicType) And

    #-------------------------------------------
    # Logic->Label
    #-------------------------------------------
    set f $fLogic.fLabel

    # Output label
    set tip "Label of interest (input/output TRUE value)."
    eval {button $f.bOutput -text "Label:" \
        -command "ShowLabels"} $Gui(WBA)
    TooltipAdd  $f.bOutput $tip
    eval {entry $f.eOutput -width 6 \
        -textvariable Label(label)} $Gui(WEA)
    TooltipAdd  $f.eOutput $tip
    eval {entry $f.eName -width 14 \
        -textvariable Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    TooltipAdd  $f.eName $tip
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w

    lappend Label(colorWidgetList) $f.eName



    #-------------------------------------------
    # Logic->Grid frame
    #-------------------------------------------

    set f $fLogic.fGrid

    set tip "Input Volume"
        DevAddSelectButton VolumeMath $f Volume2L "Volume2:"   Grid $tip
        DevAddSelectButton VolumeMath $f Volume1L "AND Volume1:" Grid $tip
    set tip "Output Volume"
        DevAddSelectButton VolumeMath $f Volume3L "= Volume3:" Grid $tip

    #-------------------------------------------
    # Logic->Pack frame
    #-------------------------------------------

    set f $fLogic.fPack

        DevAddButton $f.bRun "Run" "VolumeMathDoLogic"

    pack $f.bRun
    TooltipAdd $f.bRun "Do the logical operation and output Volume3."

}   


#Hanifa
#-------------------------------------------------------------------------------
# .PROC VolumeMathBuildResampParamPopup
#   Create the popup box that allows the user to change/view the resampling
#   parameters
# .END
#-------------------------------------------------------------------------------
proc VolumeMathBuildResampParamPopup {} {
    global Gui VolumeMath Module
    
    set w .wResamplingParams
    set Gui(wResamplingParams) $w
    toplevel $w -class Dialog -bg $Gui(inactiveWorkspace)
    wm title $w "VolumeMath Resample Parameters"
    wm iconname $w Dialog
    wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
    if {$Gui(pc) == "0"} {
        wm transient $w .
    }
    wm withdraw $w
    
    # Frames
    frame $w.fOptions -bg $Gui(activeWorkspace) -relief groove -bd 2
    pack $w.fOptions -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    set f $w.fOptions
    frame $f.fInterpolationMode -bg $Gui(activeWorkspace) -relief groove -bd 1
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fOrigin -bg $Gui(activeWorkspace) 
    frame $f.fExtent -bg $Gui(activeWorkspace)
    frame $f.fSpacing -bg $Gui(activeWorkspace)   
    frame $f.fApplyOrCancel -bg $Gui(activeWorkspace) 

    pack $f.fInterpolationMode $f.fTitle $f.fOrigin $f.fExtent $f.fSpacing \
    $f.fApplyOrCancel -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

    #-------------------------------------------
    # Options->InterpolationMode frame
    #-------------------------------------------
    set f $w.fOptions.fInterpolationMode
    eval {label $f.lInterpolationMode -text "Interpolation Mode:"} $Gui(WLA) -foreground blue
    pack $f.lInterpolationMode -pady $Gui(pad) -padx 25 -side left -fill x
    
    set VolumeMath(interpolationMode) Linear
   
    foreach mode "Linear Nearest_Neighbour" \
    text "Linear Nearest_Neighbour" {
        eval {radiobutton $f.r$mode \
              -text "$text" -value "$mode"\
              -variable VolumeMath(interpolationMode)\
              -indicatoron 1} $Gui(WCA)
        pack $f.r$mode -side left -ipadx 10 -fill x
    }
           
    #-------------------------------------------
    # Options->Title/Instructions frame
    #-------------------------------------------
    set f $w.fOptions.fTitle
    eval {label $f.lTitle -text "These are the Parameters that will be Applied to the Volume to Be Resampled"} $Gui(WLA) 
    eval {label $f.lTitle2 -text "To Change the Parameters Insert the Desired Values and Click Update"} $Gui(WLA)
    eval {label $f.lTitle3 -text "Click Cancel to Exit this Window Without Changing the Parameters"} $Gui(WLA)

    pack $f.lTitle -pady $Gui(pad)
    pack $f.lTitle2 
    pack $f.lTitle3

    #-------------------------------------------
    # Options->Origin frame
    #-------------------------------------------
    set f $w.fOptions.fOrigin
    eval {label $f.lOriginTitle -text "Origin: "} $Gui(WLA) -foreground blue
    eval {entry $f.eOriginVal -textvariable VolumeMath(originValTemp) \
          -width 28} $Gui(WEA)
    grid $f.lOriginTitle $f.eOriginVal -pady 2
    
    #Save the path to the entrybox so that we can configure it later
    set VolumeMath(OriginValEntryBox) $f.eOriginVal
    
    #-------------------------------------------
    # Options->Extent frame
    #-------------------------------------------
    set f $w.fOptions.fExtent
    eval {label $f.lExtentTitle -text "Extent: "} $Gui(WLA) -foreground blue
    eval {entry $f.eExtentVal -textvariable VolumeMath(extentValTemp) \
          -width 28} $Gui(WEA)
    grid $f.lExtentTitle $f.eExtentVal -pady 2
    
    #Save the path to the entrybox so that we can configure it later
    set VolumeMath(ExtentValEntryBox) $f.eExtentVal

    #-------------------------------------------
    # Options->Spacing frame
    #------------------------------------------- 
    set f $w.fOptions.fSpacing
    eval {label $f.lSpacingTitle -text "Spacing: "} $Gui(WLA) -foreground blue
    eval {entry $f.eSpacingVal -textvariable VolumeMath(spacingValTemp) \
          -width 28} $Gui(WEA)
    grid $f.lSpacingTitle $f.eSpacingVal -pady 2
    
    #Save the path to the entrybox so that we can configure it later
    set VolumeMath(SpacingValEntryBox) $f.eSpacingVal

    #-------------------------------------------
    # Options->Apply frame
    #-------------------------------------------
    set f $w.fOptions.fApplyOrCancel
    eval {button $f.bApply -text "Apply"  \
          -command "VolumeMathUpdateInterpParams"} $Gui(WBA) {-width 25}
    eval {button $f.bCancel -text "Cancel"  \
          -command "VolumeMathCancelInterpParams; wm withdraw $w"} $Gui(WBA) {-width 25}
    grid $f.bApply $f.bCancel -padx 10 -pady 2 
 }

#Hanifa
#-------------------------------------------------------------------------------
# .PROC VolumeMathShowPopup
#   Show the popup box that allows the user to change/view the resampling
#   parameters
# .END
#-------------------------------------------------------------------------------
proc VolumeMathShowPopup {{x 255} {y 0}} {
    global Gui VolumeMath
 
    if {[VolumeMathCheckErrors] == 1} {
        return
    }
   
    # Recreate popup if user killed it
    if {[winfo exists $Gui(wResamplingParams)] == 0} {
        VolumeMathBuildResampParamPopup 
    }
    # Show the popup box
    ShowPopup $Gui(wResamplingParams) $x $y
}

#Hanifa
#-------------------------------------------------------------------------------
# .PROC VolumeMathUpdateInterpParams
#  Update the resampling parameters. Tests are done to ensure that the values
#  entered are valid. Note: The parameters are only updated if the user clicks
#  the Apply button on the resampling parameters popup box.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathUpdateInterpParams {} {
    global VolumeMath Gui

    # Validate Input

    # Ensure origin is 3 numbers
    if {[llength $VolumeMath(originValTemp)] != 3} {
        tk_messageBox -message \
            "The origin of the volume must be specified by 3 values representing the x, y, z coordinates"
        #Cancel any changes
        VolumeMathCancelInterpParams
        return
    }

    # Ensure extent is 6 numbers
    if {[llength $VolumeMath(extentValTemp)] != 6} {
        tk_messageBox -message \
            "The extent of the volume must be specified by 6 values"
        #Cancel any changes
        VolumeMathCancelInterpParams
        return
    }
    
    # Ensure spacing is 3 numbers
    if {[llength $VolumeMath(spacingValTemp)] != 3} {
        tk_messageBox -message \
            "The spacing of the volume must be specified by 3 values"
        #Cancel any changes
        VolumeMathCancelInterpParams
        return
    }

    #Now if the tests are passed then set the new origin, extent and spacing values
    set VolumeMath(originVal) $VolumeMath(originValTemp)
    set VolumeMath(extentVal) $VolumeMath(extentValTemp)
    set VolumeMath(spacingVal) $VolumeMath(spacingValTemp)

    #Close the popup window
    wm withdraw $Gui(wResamplingParams)
}

#Hanifa
#-------------------------------------------------------------------------------
# .PROC VolumeMathCancelInterpParams
#   Reset the resampling parameters to what they were before the user started to
#   enter new paramters. Note: This is only done if the user presses the cancel
#   button on the resampling parameters popup box.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathCancelInterpParams {} {
    global VolumeMath

    set VolumeMath(originValTemp) $VolumeMath(originVal)
    set VolumeMath(extentValTemp) $VolumeMath(extentVal)
    set VolumeMath(spacingValTemp) $VolumeMath(spacingVal)
}

#Hanifa
#-------------------------------------------------------------------------------
# .PROC VolumeMathGetInitParams
#   Get and set initial resampling parameters for a volume when the user selects 
#   it on the menu button.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathGetInitParams {} {
    global VolumeMath Volume 
    
    set volID $VolumeMath(Volume1)

    set VolumeMath(originVal) [[Volume($volID,vol) GetOutput] GetOrigin]
    set VolumeMath(spacingVal) [[Volume($volID,vol) GetOutput] GetSpacing]
    set VolumeMath(extentVal) [[Volume($volID,vol) GetOutput] GetExtent] 

    set VolumeMath(originValTemp) $VolumeMath(originVal)
    set VolumeMath(extentValTemp) $VolumeMath(extentVal)
    set VolumeMath(spacingValTemp) $VolumeMath(spacingVal)
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathEnter {} { 
    global VolumeMath Label

    #array set mgr $VolumeMath(eventMgr)
    #pushEventManager mgr

    # color in the label selection widget in the Logic panel
    LabelsColorWidgets

}

#-------------------------------------------------------------------------------
# .PROC VolumeMathExit
# 
#  On Exit, get rid of the event manager.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathExit {} {
    #popEventManager
}


#-------------------------------------------------------------------------------
# .PROC VolumeMathSetMathType
#   Set the type of Math to be Done
# .END
#-------------------------------------------------------------------------------
proc VolumeMathSetMathType {} {

    global Module VolumeMath

    set fMath $Module(VolumeMath,fMath)
    set f $fMath.fGrid
    
    set a $f.lVolume2 
    set b $f.lVolume1 
    set c $f.lVolume3 

    if {$VolumeMath(MathType) != "Resample"} {
        pack forget $VolumeMath(ResampParamButton)
    }

    if {$VolumeMath(MathType) != "Mask"} {
        pack forget $VolumeMath(maskLabelFrame)
    }
    
    if {$VolumeMath(MathType) == "Subtract" } {
        $a configure -text "Volume2:"
        $b configure -text "- Volume1:"
        $c configure -text "= Volume3:"
    } elseif {$VolumeMath(MathType) == "Add" } {
        $a configure -text "Volume2:"
        $b configure -text "+ Volume1:"
        $c configure -text "= Volume3:"
    } elseif {$VolumeMath(MathType) == "Resample" } {
        $a configure -text "Resample"
        $b configure -text "in the coordinates of"
        $c configure -text "and put the results in"
        pack $VolumeMath(ResampParamButton)
    } elseif {$VolumeMath(MathType) == "Abs" } {
        $a configure -text "Absolute Value"
        $b configure -text "(not used)"
        $c configure -text "and put the results in"
    } elseif {$VolumeMath(MathType) == "DistMap" } {
        $a configure -text "Distance Map"
        $b configure -text "(not used)"
        $c configure -text "and put the results in"
    } elseif {$VolumeMath(MathType) == "Hausdorff" } {
        $a configure -text "Undir. Par. Haus. Dist. V2"
        $b configure -text "V1"
        $c configure -text "and put the results in"
    } elseif {$VolumeMath(MathType) == "Statistics" } {
        $a configure -text "Calculate Statistics of"
        $b configure -text "(not used)"
        $c configure -text "(not used)"
    } elseif {$VolumeMath(MathType) == "Multiply" } {
        $a configure -text "Volume2:"
        $b configure -text "Volume1:"
        $c configure -text "=Volume3"
    } elseif {$VolumeMath(MathType) == "Mask" } {
        $a configure -text "Volume to Mask:"
        $b configure -text "Label Map:"
        $c configure -text "Masked Output:"
        VolumeMathSetMaskLabel
        pack $VolumeMath(maskLabelFrame)
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathSetLogicType
#   Set the type of Logic to be Done
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathSetLogicType {} {

    global Module VolumeMath

    set fLogic $Module(VolumeMath,fLogic)
    set f $fLogic.fGrid
    
    set a $f.lVolume2L
    set b $f.lVolume1L
    set c $f.lVolume3L

    if {$VolumeMath(LogicType) == "And" } {
        $a configure -text "Volume2:"
        $b configure -text "AND Volume1:"
        $c configure -text "= Volume3:"
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathPrepareResultVolume
#   Check for Errors in the setup
#   returns 1 if there are errors, 0 otherwise
# 
# .ARGS
# boolean logic default is 0; if calling from the Logic tab, set this to 1
# .END
#-------------------------------------------------------------------------------
proc VolumeMathPrepareResultVolume {{logic "0"}}  {
    global VolumeMath

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Do we need to Create a New Volume?
    # If so, let's do it.
    
    if {$v3 == -5 } {
        # give each one a uniqe name
        if { ![info exists VolumeMath(ImageSerialNumber)] } {
            set VolumeMath(ImageSerialNumber) 1
        } else {
            incr VolumeMath(ImageSerialNumber)
        }

        if {$logic == "0"} {
            set name  "VolumeMath$VolumeMath(MathType)Result-$VolumeMath(ImageSerialNumber)" 
        } else {
            set name  "VolumeMath$VolumeMath(LogicType)Result-$VolumeMath(ImageSerialNumber)" 
        }
        set v3 [DevCreateNewCopiedVolume $v2 "" $name]
        set node [Volume($v3,vol) GetMrmlNode]
        Mrml(dataTree) RemoveItem $node 
        set nodeBefore [Volume($v1,vol) GetMrmlNode]
        Mrml(dataTree) InsertAfterItem $nodeBefore $node
        MainUpdateMRML
    } else {

        # Are We Overwriting a volume?
        # If so, let's ask. If no, return.
         
        set v3name  [Volume($v3,node) GetName]
        set continue [DevOKCancel "Overwrite $v3name?"]
          
        if {$continue == "cancel"} { return 1 }
        # They say it is OK, so overwrite!
              
        Volume($v3,node) Copy Volume($v2,node)
    }

    set VolumeMath(Volume3) $v3
    

    return 0
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathPrepareResult
#   Create the New Volume if necessary. Otherwise, ask to overwrite.
#   returns 1 if there is are errors 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc VolumeMathCheckErrors {} {
    global VolumeMath Volume
    if {($VolumeMath(Volume1) == $Volume(idNone)) || \
            ($VolumeMath(Volume2) == $Volume(idNone)) || \
            ($VolumeMath(Volume3) == $Volume(idNone))} {
        DevErrorWindow "You cannot use Volume \"None\""
        return 1
    }
    return 0
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoMath
#   Calls the correct function
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoMath {} {
    global VolumeMath Volume


    if { $VolumeMath(MathType) == "Subtract" } {VolumeMathDoSubtract}
    if { $VolumeMath(MathType) == "Add" }      {VolumeMathDoAdd}
    if { $VolumeMath(MathType) == "Resample" } {VolumeMathDoResample}
    if { $VolumeMath(MathType) == "Abs" }      {VolumeMathDoAbs}
    if { $VolumeMath(MathType) == "DistMap" }  {VolumeMathDoDistMap}
    if { $VolumeMath(MathType) == "Hausdorff"} {VolumeMathDoHausdorff}
    if { $VolumeMath(MathType) == "Statistics"} {VolumeMathDoStatistics}
    if { $VolumeMath(MathType) == "Multiply"} {VolumeMathDoMultiply}
    if { $VolumeMath(MathType) == "Mask"} {VolumeMathDoMask}

    # This is necessary so that the data is updated correctly.
    # If the programmers forgets to call it, it looks like nothing
    # happened. (skip for statistics that doesn't create a new volume)
    if { $VolumeMath(MathType) != "Statistics"} {
        set v3 $VolumeMath(Volume3)
        MainVolumesUpdate $v3
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoLogic
# Calls the correct function
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoLogic {} {
    global VolumeMath Volume

    if { $VolumeMath(LogicType) == "And"}       {VolumeMathDoAnd}

    # This is necessary so that the data is updated correctly.
    # If the programmers forgets to call it, it looks like nothing
    # happened.
    set v3 $VolumeMath(Volume3)
    MainVolumesUpdate $v3
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoSubtract
#   Actually do the VolumeMath
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoSubtract {} {
    global VolumeMath Volume

    # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Set up the VolumeMath Subtract

    vtkImageMathematics SubMath
    SubMath SetInput1 [Volume($v2,vol) GetOutput]
    SubMath SetInput2 [Volume($v1,vol) GetOutput]
    SubMath SetOperationToSubtract

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [SubMath GetOutput]
    MainVolumesUpdate $v3

    SubMath Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoAdd
#   Actually do the VolumeMath Addition
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoAdd {} {
    global VolumeMath Volume

        # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Set up the VolumeMath Add

    vtkImageMathematics AddMath
    AddMath SetInput1 [Volume($v2,vol) GetOutput]
    AddMath SetInput2 [Volume($v1,vol) GetOutput]
    AddMath SetOperationToAdd

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [AddMath GetOutput]
    MainVolumesUpdate $v3

    AddMath Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoHausdorff
#   Find the distance Map
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoHausdorff {} {
    global VolumeMath Volume

        # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    set vol2 [Volume($v2,vol) GetOutput]
    set vol1 [Volume($v1,vol) GetOutput]

    # The distance Map from volume 1

    vtkImageLogic Logic
    Logic SetOperationToNot
    Logic SetInput1 $vol1

    vtkImageEuclideanDistance DistMap
    DistMap ConsiderAnisotropyOn
    DistMap InitializeOn
    DistMap SetInput [Logic GetOutput]

    ## Get region of Image2 that does not exist in image 1
    ## Get the distance map in that region

    # first a cast.
    vtkImageCast CastToUChar
    CastToUChar SetOutputScalarTypeToUnsignedChar
    CastToUChar ClampOverflowOn
    CastToUChar SetInput $vol2

    vtkImageMask Distances
    Distances SetMaskedOutputValue 0
    Distances SetMaskInput  [CastToUChar GetOutput]
    Distances SetImageInput [DistMap GetOutput]
    Distances Update
    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    vtkImageStatistics stat
    stat SetInput [Distances GetOutput]
    stat IgnoreZeroOn
    stat Update

    Volume($v3,vol) SetImageData [Distances GetOutput]
    MainVolumesUpdate $v3

    set a [stat GetNumExaminedElements]
    set b [stat GetQuartile1 ]
    set c [stat GetMedian    ]
    set d [stat GetQuartile3 ]
    set e [stat GetQuintile1 ]
    set f [stat GetQuintile2 ]
    set g [stat GetQuintile3 ]
    set h [stat GetQuintile4 ]
    set i [stat GetAverage   ]
    set j [stat GetStdev     ]
    set k [stat GetMax       ]
    set l [stat GetMin       ]

    set mes1 "NumElements $a \n Quartiles: $b $c $d \n"
    set mes2 "Min: $k Max: $l \n"
    set mes3 "Quintiles: $e $f $g $h \n"
    set mes4 "Mean $i +/- std: $j \n";
    tk_messageBox -message "$mes1 $mes2 $mes3 $mes4"

    DistMap Delete
    Logic Delete
    Distances Delete
    CastToUChar Delete
    stat Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoStatistics
#   Generate and Display Image Statistics
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoStatistics {} {
    global VolumeMath Volume

    if { $VolumeMath(Volume2) == $Volume(idNone) } {
        DevErrorWindow "You cannot use Volume \"None\""
        return 1
    }

    catch "stat Delete"
    vtkImageStatistics stat
    stat IgnoreZeroOn
    stat SetInput [Volume($VolumeMath(Volume2),vol) GetOutput]
    stat Update

    set msg "Statistics of [Volume($VolumeMath(Volume2),node) GetName]\n"
    set msg "$msg - Min: [stat GetMin] Max: [stat GetMax]\n"
    set msg "$msg - Mean [stat GetAverage] +/- std: [stat GetStdev] \n"
    set msg "$msg \n(zero values ignored)"
    tk_messageBox -message $msg -type ok

    stat Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoDistMap
#   Find the distance Map
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoDistMap {} {
    global VolumeMath Volume

        # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Set up the Distance Map

    vtkImageLogic Logic
    Logic SetOperationToNot
    Logic SetInput1 [Volume($v2,vol) GetOutput]

    vtkImageEuclideanDistanceTransformation DistMap
    DistMap ConsiderAnisotropyOn
    DistMap InitializeOn
#    DistMap SetInput [Volume($v2,vol) GetOutput]
    DistMap SetInput [Logic GetOutput]

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [DistMap GetOutput]
    MainVolumesUpdate $v3

    DistMap Delete
    Logic Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoAbs
#   Actually do the VolumeMath Absolute Value
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoAbs {} {
    global VolumeMath Volume

        # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Set up the VolumeMath Abs

    vtkImageMathematics SubMath
    SubMath SetInput1 [Volume($v2,vol) GetOutput]
    SubMath SetInput2 [Volume($v1,vol) GetOutput]
    SubMath SetOperationToAbsoluteValue

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [SubMath GetOutput]
    MainVolumesUpdate $v3

    SubMath Delete
}


#-------------------------------------------------------------------------------
# .PROC VolumeMathDoResample
#   Actually do the Resampling
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoResample {} {
    global VolumeMath Volume

    # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    puts "$v3 $v2 $v1"


    # Set up the VolumeMath Resampling

    # You would think we would want 
    # ScaledIJKtoRAS2^-1 * ScaledIJKtoRAS1 
    # But in fact, if there is a transformation matrix affecting
    # the two volumes, it shows up in the RasToWld matrix.
    # so that we want
    # (RasToWld2*ScaledIJKToWld2)^-1 (RasToWld1*ScaledIJKToWld1)

    # Get ScaledIJKs
    #vtkMatrix4x4 tIJK1
    #tIJK1 DeepCopy [Volume($v1,node) GetRasToVtkMatrix]
    #tIJK1 Invert
    #set sIJK1 tIJK1    

    #vtkMatrix4x4 tIJK2
    #tIJK2 DeepCopy [Volume($v2,node) GetRasToVtkMatrix]
    #tIJK2 Invert

    #set sIJK2 tIJK2
    set sIJK2 [Volume($v2,node) GetPosition]
    set sIJK1 [Volume($v1,node) GetPosition]

    # Get RasToWlds
    set RasWld2 [Volume($v2,node) GetRasToWld]
    set RasWld1 [Volume($v1,node) GetRasToWld]

    vtkMatrix4x4 Amatrix
    Amatrix Multiply4x4 $RasWld2 $sIJK2 Amatrix
    Amatrix Invert
    Amatrix Multiply4x4 Amatrix $RasWld1 Amatrix
    Amatrix Multiply4x4 Amatrix $sIJK1   Amatrix
    Amatrix Print


#    Resampling
#    Samson's version
#    vtkResliceImage Reslice
#    Reslice SetInput            [Volume($v2,vol) GetOutput]
#    Reslice SetOutputImageParam [Volume($v1,vol) GetOutput]
#    Reslice SetTransformOutputToInput Amatrix
#    Reslice Update
    
#    [Reslice GetOutput] Print
#    Volume($v3,vol) SetImageData [Reslice GetOutput]
#    [Volume($v3,vol) GetOutput] Print
    
    #Hanifa Changed to use vtkImageReslice class 
    vtkImageReslice Reslice

    #check to see which mode is currently selected
    if {$VolumeMath(interpolationMode) == "Linear"} {
        Reslice SetInterpolationModeToLinear
    } else {
        Reslice SetInterpolationModeToNearestNeighbor
    }

    Reslice SetInput [Volume($v2,vol) GetOutput]
    
    #set the origin values  
    Reslice SetOutputOrigin [lindex $VolumeMath(originVal) 0] \
        [lindex $VolumeMath(originVal) 1] [lindex $VolumeMath(originVal) 2]
    #print the origin values 
    set OriginVals [Reslice GetOutputOrigin]
    puts "Origin values: $OriginVals"

    #set the extent values
    Reslice SetOutputExtent \
        [lindex $VolumeMath(extentVal) 0] \
        [lindex $VolumeMath(extentVal) 1] \
        [lindex $VolumeMath(extentVal) 2] \
        [lindex $VolumeMath(extentVal) 3] \
        [lindex $VolumeMath(extentVal) 4] \
        [lindex $VolumeMath(extentVal) 5] 
    #print the extent values
    set ExtentVals [Reslice GetOutputExtent]
    puts "Extent Values: $ExtentVals"

    #set the spacing values
    Reslice SetOutputSpacing [lindex $VolumeMath(spacingVal) 0] \
        [lindex $VolumeMath(spacingVal) 1] [lindex $VolumeMath(spacingVal) 2] 
    #print the spacing values
    set SpacingVals [Reslice GetOutputSpacing]
    puts "Spacing Values: $SpacingVals"

    puts [Amatrix Print]
  
    vtkMatrixToLinearTransform trans
    trans SetInput Amatrix
    Reslice SetResliceTransform trans 
    Reslice Update

    [Reslice GetOutput] Print
    Volume($v3,vol) SetImageData [Reslice GetOutput]
    [Volume($v3,vol) GetOutput] Print

    puts "$v3 $v2 $v1"

    MainVolumesUpdate $v3
    Amatrix Delete
    Reslice Delete
    trans Delete

    #Hanifa 
    #Set the text on the output button to be the newly resampled volume 
    $VolumeMath(mbVolume3) config -text [Volume($v3,node) GetName]

}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoMultiply
#   Actually do the VolumeMath Multiply
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoMultiply {} {
    global VolumeMath Volume

        # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Set up the VolumeMath Multiply

    vtkImageMathematics MultMath
    MultMath SetInput1 [Volume($v2,vol) GetOutput]
    MultMath SetInput2 [Volume($v1,vol) GetOutput]
    MultMath SetOperationToMultiply

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [MultMath GetOutput]
    MainVolumesUpdate $v3

    MultMath Delete
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoMask
#   Actually do the VolumeMath Mask
#  - makes a new volume where everything is zero except the selected label value
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoMask {} {
    global VolumeMath Volume

    # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    if {[VolumeMathPrepareResultVolume] == 1} {
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # create the binary volume of the label
    catch "mathThresh Delete"
    vtkImageThreshold mathThresh
    mathThresh SetInput [Volume($v1,vol) GetOutput]
    mathThresh SetInValue 1
    mathThresh SetOutValue 0
    mathThresh ReplaceInOn
    mathThresh ReplaceOutOn
    mathThresh ThresholdBetween $VolumeMath(maskLabel) $VolumeMath(maskLabel) 

    # Set up the VolumeMath Mask

    catch "MultMath Delete"
    vtkImageMathematics MultMath
    MultMath SetInput1 [Volume($v2,vol) GetOutput]
    MultMath SetInput2 [mathThresh GetOutput]
    MultMath SetOperationToMultiply

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [MultMath GetOutput]
    MainVolumesUpdate $v3

    MultMath Delete
    mathThresh Delete
}

################# Procedures from Logic Tab #################

#-------------------------------------------------------------------------------
# .PROC VolumeMathDoAnd
#   Actually do the And.  Replace label of interest with 1,
# then AND the volumes.  Output true value is label of interest.
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoAnd {} {
    global VolumeMath Volume Label
    
    # Check to make sure no volume is none

    if {[VolumeMathCheckErrors] == 1} {
        return
    }
    # the parameter 1 tells it  to use the name from the 
    # logic operation selected, instead of the math op.
    if {[VolumeMathPrepareResultVolume 1] == 1} {
        return
    }


    # Validate input
    if {[ValidateInt $Label(label)] == 0} {
        tk_messageBox -message "Label of interest is not an integer."
        return
    }

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # set up labels of interest
    set l3 $Label(label)
    set l2 $Label(label)
    set l1 $Label(label)

    # Set up the VolumeMath And

    # first replace label of interest with a 1 in both volumes
    vtkImageThreshold thresh1
    thresh1 ThresholdBetween  $l1 $l1
    thresh1 SetInValue 1
    thresh1 SetOutValue 0
    thresh1 ReplaceInOn
    thresh1 ReplaceOutOn
    thresh1 SetInput [Volume($v1,vol) GetOutput]
    thresh1 SetOutputScalarTypeToShort

    vtkImageThreshold thresh2
    thresh2 ThresholdBetween  $l2 $l2
    thresh2 SetInValue 1
    thresh2 SetOutValue 0
    thresh2 ReplaceInOn
    thresh2 ReplaceOutOn
    thresh2 SetInput [Volume($v2,vol) GetOutput]
    thresh2 SetOutputScalarTypeToShort

    vtkImageLogic AndMath
    AndMath SetInput1 [thresh1 GetOutput]
    AndMath SetInput2 [thresh2 GetOutput]
    AndMath SetOperationToAnd
    AndMath SetOutputTrueValue $l3

    # Start copying in the output data.
    # Taken from MainVolumesCopyData

    Volume($v3,vol) SetImageData [AndMath GetOutput]
    MainVolumesUpdate $v3

    AndMath Delete
    thresh1 Delete
    thresh2 Delete
}


#-------------------------------------------------------------------------------
# .PROC VolumeMathSetMaskLabel
# - callback to set the label for the Mask operator
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathSetMaskLabel {} {
    global Label VolumeMath

    LabelsFindLabel

    if {$Label(label) != ""} {
        set VolumeMath(maskLabel) $Label(label)
    } else {
        set VolumeMath(maskLabel) 0
    }
}
