#=auto==========================================================================
# Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        VolumeMath.tcl
# PROCEDURES:  
#   VolumeMathInit
#   VolumeMathUpdateGUI
#   VolumeMathBuildGUI
#   VolumeMathEnter
#   VolumeMathExit
#   VolumeMathDoMath
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
		{$Revision: 1.6 $} {$Date: 2001/01/11 18:48:46 $}]

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
Also, this module only does subtraction.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags VolumeMath $help
	MainHelpBuildGUI VolumeMath

	#-------------------------------------------
	# Math frame
	#-------------------------------------------

	set fMath $Module(VolumeMath,fMath)
	set f $fMath

	frame $f.fGrid -bg $Gui(activeWorkspace)
	frame $f.fPack -bg $Gui(activeWorkspace)
	pack $f.fGrid $f.fPack -side top -padx 0 -pady $Gui(pad)

	#-------------------------------------------
	# Math->Grid frame
	#-------------------------------------------
	set f $fMath.fGrid

        DevAddSelectButton VolumeMath $f Volume2 "Volume2:" Grid
        DevAddSelectButton VolumeMath $f Volume1 "- Volume1:" Grid
        DevAddSelectButton VolumeMath $f Volume3 "= Volume3:" Grid

	#-------------------------------------------
	# Math->Pack frame
	#-------------------------------------------

	set f $fMath.fPack

        DevAddButton $f.bRun "Run" "VolumeMathDoMath" 

	pack $f.bRun

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
# .PROC VolumeMathDoMath
#   Actually do the VolumeMath
#
# .END
#-------------------------------------------------------------------------------
proc VolumeMathDoMath {} {
	global VolumeMath Volume

        # Check to make sure no volume is none

  	  if {($VolumeMath(Volume1) == $Volume(idNone)) || \
  	     ($VolumeMath(Volume2) == $Volume(idNone)) || \
  	     ($VolumeMath(Volume3) == $Volume(idNone))} {
                DevErrorWindow "You cannot use Volume \"None\""
		return
          }

        
	tk_messageBox -type yesno -message "You chose $VolumeMath(Volume1) $VolumeMath(Volume2) $VolumeMath(Volume3)"

	set v3 $VolumeMath(Volume3)
	set v2 $VolumeMath(Volume2)
	set v1 $VolumeMath(Volume1)

        # Do we need to Create a New Volume?
        # If so, let's do it.

          if {$v3 == -5 } {
              set v3 [DevCreateNewCopiedVolume $v2 "" "VolumeMathResult" ]
          } else {

        # Are We Overwriting a volume?
        # If so, let's ask. If no, return.
         
        set v3name  [Volume($v3,node) GetName]
	set continue [tk_messageBox -type okcancel -message "Overwrite $v3name?"]
#          YesNoPopup OverWrite 20 50 "" return
          
        if {$continue == "cancel"} return
        # They say it is OK, so overwrite!
              
           Volume($v3,node) Copy Volume($v2,node)
          }

        # Set up the VolumeMath Subtract

          vtkImageMathematics SubMath
          SubMath SetInput1 [Volume($v2,vol) GetOutput]
          SubMath SetInput2 [Volume($v1,vol) GetOutput]
          SubMath SetOperationToSubtract

       # Start copying in the output data.
       # Taken from MainVolumesCopyData

        Volume($v3,vol) SetImageData [SubMath GetOutput]

        SubMath Delete
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
