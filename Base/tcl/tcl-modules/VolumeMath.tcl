#=auto==========================================================================
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
# FILE:        VolumeMath.tcl
# PROCEDURES:  
#   VolumeMathInit
#   VolumeMathUpdateGUI
#   VolumeMathBuildGUI
#   VolumeMathEnter
#   VolumeMathExit
#   VolumeMathSetMathType
#   VolumeMathPrepareResultVolume
#   VolumeMathPrepareResult
#   VolumeMathDoSubtract
#   VolumeMathDoSubtract
#   VolumeMathDoHausdorff
#   VolumeMathDoDistMap
#   VolumeMathDoAbs
#   VolumeMathDoResample
#   VolumeMathCount
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
        set Module($m,row1List) "Help Math Distance"
        set Module($m,row1Name) "{Help} {Math} {Distance}"
	set Module($m,row1,tab) Math

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
		{$Revision: 1.15 $} {$Date: 2001/08/03 12:21:08 $}]

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

}

#-------------------------------------------------------------------------------
# .PROC VolumeMathUpdateGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathUpdateGUI {} {
	global VolumeMath Volume

    DevUpdateNodeSelectButton Volume VolumeMath Volume1 Volume1 DevSelectNode
    DevUpdateNodeSelectButton Volume VolumeMath Volume2 Volume2 DevSelectNode
    DevUpdateNodeSelectButton Volume VolumeMath Volume3 Volume3 DevSelectNode 0 1 1
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathBuildGUI {} {
	global Gui VolumeMath Module

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
	#	fGrid
	#	    mbVolume1
	#	    mbVolume2
	#	    mbVolume3
	#	fPack
	#	    bRun
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
	#	fPack
        #           ltop
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

For Subtraction: If you wish to subtract two volumes that have
 different spacings or numbers of voxels, or if a transform exists
 between the two images, you MUST resample one image in the coordinates
 of the other. (Note that this module doesn't currently check to make sure
 you did everything correctly. One day...) Once the images are subtracted, the
 results are sometimes difficult to look at. I recommend taking the
 Absolute Valute of the results of the subtraction. <p>

Distance Maps yield the square of the distance.

<b>Known Bugs<b>Don't set the output to be one of the input
files. Sometimes it doesn't work.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags VolumeMath $help
	MainHelpBuildGUI VolumeMath

	#-------------------------------------------
	# Math frame
	#-------------------------------------------

	set fMath $Module(VolumeMath,fMath)
	set f $fMath

	frame $f.fSelectMath  -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fGrid -bg $Gui(activeWorkspace)
	frame $f.fPack -bg $Gui(activeWorkspace)
	pack $f.fSelectMath $f.fGrid $f.fPack -side top -padx 0 -pady $Gui(pad)

	#-------------------------------------------
	# Math->SelectMath
	#-------------------------------------------
	set f $fMath.fSelectMath

        eval {label $f.l -text "Type:"} $Gui(BLA)
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
	foreach p "Subtract Resample Abs DistMap Hausdorff" {
            eval {radiobutton $f.f.$row.r$p \
			-text "$p" -command "VolumeMathSetMathType" \
			-variable VolumeMath(MathType) -value $p -width 10 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.$row.r$p -side left -pady 0
            if { $p == "Abs" } {incr row};
	}

	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

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
##		ViewSetFocalPoint $xRas $yRas $zRas
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
##		ViewSetFocalPoint $xRas $yRas $zRas
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
    }   

#-------------------------------------------------------------------------------
# .PROC VolumeMathEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathEnter {} { 
    global VolumeMath

    array set mgr $VolumeMath(eventMgr)
    pushEventManager mgr

}

#-------------------------------------------------------------------------------
# .PROC VolumeMathExit
# 
#  On Exit, get rid of the event manager.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc VolumeMathExit {} {
    popEventManager
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathSetMathType
#   Set the type of Math to be Done
# .END
#-------------------------------------------------------------------------------
proc VolumeMathSetMathType {}  {

    global Module VolumeMath
	set fMath $Module(VolumeMath,fMath)
	set f $fMath.fGrid

    set a $f.lVolume2 
    set b $f.lVolume1 
    set c $f.lVolume3 


    if {$VolumeMath(MathType) == "Subtract" } {
        $a configure -text "Volume2:"
        $b configure -text "- Volume1:"
        $c configure -text "= Volume3:"
    } elseif {$VolumeMath(MathType) == "Resample" } {
        $a configure -text "Resample"
        $b configure -text "in the coordinates of"
        $c configure -text "and put the results in"
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
    }
}

#-------------------------------------------------------------------------------
# .PROC VolumeMathPrepareResultVolume
#   Check for Errors in the setup
#   returns 1 if there are errors, 0 otherwise
# .END
#-------------------------------------------------------------------------------
proc VolumeMathPrepareResultVolume {}  {
    global VolumeMath

    set v3 $VolumeMath(Volume3)
    set v2 $VolumeMath(Volume2)
    set v1 $VolumeMath(Volume1)

    # Do we need to Create a New Volume?
    # If so, let's do it.
    
    if {$v3 == -5 } {
        set v3 [DevCreateNewCopiedVolume $v2 "" \
                "VolumeMath$VolumeMath(MathType)Result" ]
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
# .PROC VolumeMathDoSubtract
#   Actually do the VolumeMath
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoMath {} {
	global VolumeMath Volume


    if { $VolumeMath(MathType) == "Subtract" } {VolumeMathDoSubtract}
    if { $VolumeMath(MathType) == "Resample" } {VolumeMathDoResample}
    if { $VolumeMath(MathType) == "Abs" }      {VolumeMathDoAbs}
    if { $VolumeMath(MathType) == "DistMap" }  {VolumeMathDoDistMap}
    if { $VolumeMath(MathType) == "Hausdorff" }  {VolumeMathDoHausdorff}

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

    vtkImageEuclideanDistanceTransformation DistMap
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
#   Actually do the VolumeMath
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
    # Resampling

    vtkResliceImage Reslice
     Reslice SetInput            [Volume($v2,vol) GetOutput]
     Reslice SetOutputImageParam [Volume($v1,vol) GetOutput]
     Reslice SetTransformOutputToInput Amatrix
     Reslice Update

    [Reslice GetOutput] Print
    Volume($v3,vol) SetImageData [Reslice GetOutput]
#    [Volume($v3,vol) GetOutput] Print
    puts "$v3 $v2 $v1"

    MainVolumesUpdate $v3
    Amatrix Delete
    Reslice Delete
}


#-------------------------------------------------------------------------------
# .PROC VolumeMathCount
#
# This routine demos how to make button callbacks and use global arrays
# for object oriented programming.
# .END
#-------------------------------------------------------------------------------
proc VolumeMathCount {} {
	global VolumeMath

	incr VolumeMath(count)
	$VolumeMath(lStuff) config -text "You clicked the button $VolumeMath(count) times"
}
