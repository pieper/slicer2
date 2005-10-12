#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        DTMRICalculateTensors.tcl
# PROCEDURES:  
#   DTMRICalculateTensorsInit
#   DTMRICalculateTensorsBuildGUI
#   RunLSDIrecon
#   ShowPatternFrame
#   DTMRIDisplayScrollBar module tab
#   DTMRICreatePatternSlice
#   DTMRICreatePatternVolume
#   DTMRILoadPattern
#   DTMRIUpdateTipsPattern
#   DTMRIViewProps
#   DTMRIDisplayNewData
#   ConvertVolumeToTensors
#   DTMRICreateNewVolume volume name desc scanOrder
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC DTMRICalculateTensorsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateTensorsInit {} {

    global DTMRI Volume

    # Version info for files within DTMRI module
    #------------------------------------
    set m "CalculateTensors"
    lappend DTMRI(versions) [ParseCVSInfo $m \
                                 {$Revision: 1.21 $} {$Date: 2005/10/12 01:03:34 $}]

    # Initial path to search when loading files
    #------------------------------------
    set DTMRI(DefaultDir) ""

    #------------------------------------
    # LSDI conversion variables
    #------------------------------------
   
    # These variables should be in the main-variables 
    # file and must be modified the first time slicer is 
    # installed to tell the program where are the python 
    # interpreter and the lsdi_slicer.py script.

    #variable that indicates where is the python interpreter
    set DTMRI(pythonintdir) /projects/lmi/local/SunOS/bin/python

    #variable that indicates where is the binary of the lsdi_slicer.py script
    set DTMRI(LSDIpydir) /projects/lmi/local/SunOS/bin/lsdi_slicer.py

    #------------------------------------
    # handling patterns variables
    #------------------------------------

    # List with the existing patterns
    # DTMRI(patternnames)

    # List with the information of the pattern called "patternname"
    # DTMRI("patternname", parameters)

    # Variable with the name of the pattern selected in the menubutton. Used to retrieve information of the pattern when converting tensors.
    # DTMRI(selectedpattern)

    # Variables associated to entries for creating a new pattern
    set DTMRI(name,name) ""
    set DTMRI(name,numberOfGradients) ""
    set DTMRI(name,firstGradientImage) ""
    set DTMRI(name,lastGradientImage) ""
    set DTMRI(name,firstNoGradientImage) ""
    set DTMRI(name,lastNoGradienImage) ""
    set DTMRI(name,gradients) ""
    set DTMRI(name,lebihan) ""
    #This variable specifies the order of the gradients disposal (slice interleaved or volume interleaved)
    set DTMRI(name,order) ""


    #------------------------------------
    # conversion from volume to DTMRIs variables
    #------------------------------------
    vtkImageDiffusionTensor _default
    set DTMRI(convert,numberOfGradients) [_default GetNumberOfGradients]
    set DTMRI(convert,gradients) ""
    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
        _default SelectDiffusionGradient $i
        lappend DTMRI(convert,gradients) [_default GetSelectedDiffusionGradient]
    }
    _default Delete
    # puts $DTMRI(convert,gradients)
    set DTMRI(convert,firstGradientImage) 1
    set DTMRI(convert,lastGradientImage) 6
    set DTMRI(convert,firstNoGradientImage) 7
    set DTMRI(convert,lastNoGradientImage) 7

    #Specific variables for Mosaic format (This should be extracted from Dicom header)
    set DTMRI(convert,mosaicTiles) 8
    set DTMRI(convert,mosaicSlices) 60

    #Variables to control the number of repetitions in the DWI volume
    set DTMRI(convert,numberOfRepetitions) 1
    set DTMRI(convert,numberOfRepetitions,min) 1
    set DTMRI(convert,numberOfRepetitions,max) 10
    set DTMRI(convert,repetition) 1
    set DTMRI(convert,averageRepetitions) 1
    set DTMRI(convert,averageRepetitionsList) {On Off}
    set DTMRI(convert,averageRepetitionsValue) {1 0}
    set DTMRI(convert,averageRepetitionsList,tooltips) [list \
                                 "Average the diffusion weighted images across repetitions."\
                 "If off having several repetitions means that the first repetition is used to compute the tensor" \
                 ]
    set DTMRI(convert,measurementframe) {{1 0 0} {0 1 0} {0 0 1}}

    
    #This variable is used by Create-Pattern button and indicates weather it has to hide or show the create pattern frame. On status 0 --> show. On status 1 --> hide.
    set DTMRI(convert,show) 0

    #Volume to convert is nrrd
    set DTMRI(convert,nrrd) 0
    
    set DTMRI(convert,makeDWIasVolume) 0
    set DTMRI(convertID) $Volume(idNone)

}


#-------------------------------------------------------------------------------
# .PROC DTMRICalculateTensorsBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICalculateTensorsBuildGUI {} {
    
    global DTMRI Module Gui Volume

    #-------------------------------------------
    # Convert frame
    #-------------------------------------------
    set fConvert $Module(DTMRI,fConv)
    set f $fConvert
    
    foreach frame "Convert ShowPattern Pattern" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor w
    }

    pack forget $f.fPattern
    $f.fConvert configure  -relief groove -bd 3 


    #-------------------------------------------
    # Convert->Convert frame
    #-------------------------------------------
    set f $fConvert.fConvert

    foreach frame "Title Select Pattern Repetitions Average Apply" {
        frame $f.f$frame -bg $Gui(activeWorkspace)
        $f.fTitle configure -bg $Gui(backdrop)
        pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Convert->Convert->Title frame
    #-------------------------------------------
    set f $fConvert.fConvert.fTitle
     
    DevAddLabel $f.lWellcome "Convert Tensors"
    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
    pack $f.lWellcome -side top -padx $Gui(pad) -pady $Gui(pad)
   
    DevAddLabel $f.lOption "This tab converts gradient data\n to diffusion tensor"
    $f.lOption configure -fg White -font {helvetica 9 normal}  -bg $Gui(backdrop) -bd 0
    pack $f.lOption -side top -padx $Gui(pad) -pady 2
    

    #-------------------------------------------
    # Convert->Convert->Select frame
    #-------------------------------------------
    set f $fConvert.fConvert.fSelect
    # Lauren test
    # menu to select a volume: will set Volume(activeID)
    DevAddSelectButton  DTMRI $f convertID "Input Volume:" Pack \
            "Input Volume to create DTMRIs from." 13 BLA
    

    # Append these menus and buttons to lists 
    # that get refreshed during UpdateMRML
    #lappend Volume(mbActiveList) $f.mbActive
    #lappend Volume(mActiveList) $f.mbActive.m


    #-------------------------------------------
    # Convert->Convert->Pattern frame
    #-------------------------------------------
#nowworking
    set f $fConvert.fConvert.fPattern

    DevAddLabel $f.lLabel "Protocol:"
    $f.lLabel configure -bg $Gui(backdrop) -fg white
    eval {menubutton $f.mbPattern -text "None" -relief raised -bd 2 -menu $f.mbPattern.menu -width 15} $Gui(WMBA)
    eval {menu $f.mbPattern.menu}  $Gui(WMA)
    button $f.bProp -text Prop. -width 5 -font {helvetica 8} -bg $Gui(normalButton) -fg $Gui(textDark)  -activebackground $Gui(activeButton) -activeforeground $Gui(textDark)  -bd $Gui(borderWidth) -padx 0 -pady 0 -relief raised -command {
        catch {DevInfoWindow $DTMRI($DTMRI(selectedpattern),tip)}
        catch {puts $DTMRI($DTMRI(selectedpattern),tip)}
        #DTMRIViewProps
    }

    pack $f.lLabel $f.bProp -side left -padx $Gui(pad) -pady $Gui(pad)
    DTMRILoadPattern
    TooltipAdd $f.lLabel "Choose a protocol to convert tensors.\n If desired does not exist, create one in the frame below."


    #-------------------------------------------
    # Convert->Convert->Repetitions frame
    #-------------------------------------------
    set f $fConvert.fConvert.fRepetitions
    
    DevAddLabel $f.l "Num. Repetitions:"
    $f.l configure -bg $Gui(backdrop) -fg white
    eval {entry $f.e -width 3 \
          -textvariable DTMRI(convert,numberOfRepetitions)} \
        $Gui(WEA)
    eval {scale $f.s -from $DTMRI(convert,numberOfRepetitions,min) \
                          -to $DTMRI(convert,numberOfRepetitions,max)    \
          -variable  DTMRI(convert,numberOfRepetitions)\
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)
      
     pack $f.l $f.e $f.s -side left -padx $Gui(pad) -pady $Gui(pad)
     
    #-------------------------------------------
    # Convert->Convert->Average frame
    #-------------------------------------------
    set f $fConvert.fConvert.fAverage
    
    DevAddLabel $f.l "Average Repetitions: "
    pack $f.l -side left -pady $Gui(pad) -padx $Gui(pad)  
    # Add menu items
    foreach vis $DTMRI(convert,averageRepetitionsList) val $DTMRI(convert,averageRepetitionsValue) \
            tip $DTMRI(convert,averageRepetitionsList,tooltips) {
        eval {radiobutton $f.r$vis \
              -text "$vis" \
              -value $val \
              -variable DTMRI(convert,averageRepetitions) \
              -indicatoron 0} $Gui(WCA)
        pack $f.r$vis -side left -padx 0 -pady 0
        TooltipAdd  $f.r$vis $tip     
    }
              
#    #-------------------------------------------
#    # Convert->Convert->Apply frame
#    #-------------------------------------------
    set f $fConvert.fConvert.fApply
    DevAddButton $f.bTest "Convert Volume" ConvertVolumeToTensors 20
    pack $f.bTest -side top -padx 0 -pady $Gui(pad) -fill x -padx $Gui(pad)


    #-------------------------------------------
    # Convert->ShowPattern frame
    #-------------------------------------------
    set f $fConvert.fShowPattern
    
    DevAddLabel $f.lLabel "Create a new protocol if your data\n does not fit the predefined ones"

    button $f.bShow -text "Create New Protocol" -bg $Gui(backdrop) -fg white -font {helvetica 9 bold} -command {
        ShowPatternFrame 
        after 250 DTMRIDisplayScrollBar DTMRI Conv}
    TooltipAdd $f.bShow "Press this button to enter Create-Protocol Frame"
    pack $f.lLabel $f.bShow -side top -pady 2 -fill x




    #-------------------------------------------
    # Convert->Pattern->Gradients Title frame
    #-------------------------------------------

#    set f $fConvert.fPattern
#    frame $f.fTitle -bg $Gui(backdrop)
#    pack $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

#    set f $fConvert.fPattern.fTitle
#    set f $Page.fTitle
   
#    DevAddLabel $f.lWellcome "Create New Protocol"
#    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
#    pack $f.lWellcome -side top -padx $Gui(pad) -pady 0
   


    #-------------------------------------------
    # Convert->Pattern frame (create tabs)
    #-------------------------------------------
    set f $fConvert.fPattern
    DevAddLabel $f.lIni "Gradient Ordering scheme:"
    pack $f.lIni -side top -pady 2

    Notebook:create $f.fNotebook \
                    -pages {{Slice Interleav.} {Volume Interleav.}} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 325 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1

    set f $fConvert.fPattern.fNotebook

    set FrameCont [Notebook:frame $f {Slice Interleav.}] 
    set FrameInter [Notebook:frame $f {Volume Interleav.}]

    foreach Page "$FrameCont $FrameInter" {   

        #-------------------------------------------
        # Convert->Pattern frame
        #-------------------------------------------
    #    set f $fConvert.fPattern
        set f $Page

        foreach frame "Name Disposal GradientNum GradientImages NoGradientImages Gradients Parameter Create" {
            frame $f.f$frame -bg $Gui(activeWorkspace)
            pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
        }

        $f configure  -relief sunken -bd 3 

        #-------------------------------------------
        # Convert->Pattern->Gradients Title frame
        #-------------------------------------------

    #    set f $fConvert.fPattern
    #    frame $f.fTitle -bg $Gui(backdrop)
    #    pack $f.fTitle -side top -padx $Gui(pad) -pady $Gui(pad) -fill x

        set f $fConvert.fPattern.fTitle
    #    set f $Page.fTitle
       
    #    DevAddLabel $f.lWellcome "Create New Protocol"
    #    $f.lWellcome configure -fg White -font {helvetica 10 bold}  -bg $Gui(backdrop) -bd 0 -relief groove
    #    pack $f.lWellcome -side top -padx $Gui(pad) -pady $Gui(pad)
       

        #-------------------------------------------
        # Convert->Pattern->Gradients Name frame
        #-------------------------------------------

    #    set f $fConvert.fPattern.fName
        set f $Page.fName

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Protocol Name:"
    #   $f.lTitle configure -relief sunken -background gray -bd 2
        DevAddEntry DTMRI name,name $f.eName 15
        pack $f.lTitle $f.eName -side left -padx $Gui(pad) -pady 4 -fill x


     
        #-------------------------------------------
        # Convert->Pattern->Gradients Disposal frame
        #-------------------------------------------

    #    set f $fConvert.fPattern.fDisposal
        set f $Page.fDisposal

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Gradients/Baselines disposal in Volume:"
        $f.lTitle configure -relief sunken -background gray -bd 2
        pack $f.lTitle -side top -padx $Gui(pad) -pady 4 -fill x
     
        #-------------------------------------------
        # Convert->Pattern->GradientNum frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradientNum
        set f $Page.fGradientNum
        
        DevAddLabel $f.l "Number of Gradient Directions:"
        eval {entry $f.eEntry \
            -textvariable DTMRI(name,numberOfGradients) \
            -width 5} $Gui(WEA)
        pack $f.l $f.eEntry -side left -padx $Gui(pad) -pady 0 -fill x

        #-------------------------------------------
        # Convert->Pattern->GradientImages frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradientImages
        set f $Page.fGradientImages

        DevAddLabel $f.l "Gradient:"
        eval {entry $f.eEntry1 \
              -textvariable DTMRI(name,firstGradientImage) \
              -width 5} $Gui(WEA)
        eval {entry $f.eEntry2 \
              -textvariable DTMRI(name,lastGradientImage) \
              -width 5} $Gui(WEA)
        pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.eEntry1 \
            "First gradient (diffusion-weighted)\nimage number at first slice location"
        TooltipAdd $f.eEntry2 \
            "Last gradient (diffusion-weighted)\niimage number at first slice location"

        #-------------------------------------------
        # Convert->Pattern->NoGradientImages frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fNoGradientImages
        set f $Page.fNoGradientImages


        DevAddLabel $f.l "Baseline:"
        eval {entry $f.eEntry1 \
              -textvariable DTMRI(name,firstNoGradientImage) \
              -width 5} $Gui(WEA)
        eval {entry $f.eEntry2 \
              -textvariable DTMRI(name,lastNoGradientImage) \
              -width 5} $Gui(WEA)
        pack $f.l $f.eEntry1 $f.eEntry2 -side left -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.eEntry1 \
            "First NO gradient (not diffusion-weighted)\nimage number at first slice location"
        TooltipAdd $f.eEntry2 \
            "Last NO gradient (not diffusion-weighted)\n image number at first slice location"


        #-------------------------------------------
        # Convert->Pattern->Gradients frame
        #-------------------------------------------
    #    set f $fConvert.fPattern.fGradients
        set f $Page.fGradients


        DevAddLabel $f.lLabel "Directions:"
        frame $f.fEntry -bg $Gui(activeWorkspace)
        eval {entry $f.fEntry.eEntry \
            -textvariable DTMRI(name,gradients) \
            -width 25 -xscrollcommand [list $f.fEntry.sx set]} $Gui(WEA)
            scrollbar $f.fEntry.sx -orient horizontal -command [list $f.fEntry.eEntry xview] -bg $Gui(normalButton) -width 10 -troughcolor $Gui(normalButton) 
        pack $f.fEntry.eEntry $f.fEntry.sx -side top -padx 0 -pady 0 -fill x
        pack $f.lLabel $f.fEntry -side left -padx $Gui(pad) -pady $Gui(pad) -fill x -anchor n
        #pack $f.sx -side top -padx $Gui(pad) -pady 0 -fill x
        TooltipAdd $f.fEntry.eEntry "List of diffusion gradient directions"

        #-------------------------------------------
        # Convert->Pattern->Parameters frame
        #-------------------------------------------




    # This frame is supposed to hold the entries for needed parameters in tensors conversion.

    #    set f $fConvert.fPattern.fParameter
        set f $Page.fParameter

        $f configure -relief raised -padx 2 -pady 2
        DevAddLabel $f.lTitle "Conversion Parameters:"
        $f.lTitle configure -relief sunken -background gray -bd 2
        pack $f.lTitle -side top -padx $Gui(pad) -pady 4 -fill x
        DevAddLabel $f.lLeBihan "LeBihan factor (b):"
        eval {entry $f.eEntrylebihan \
            -textvariable DTMRI(name,lebihan)  \
            -width 4} $Gui(WEA)
        eval {scale $f.slebihan -from 100 -to 5000 -variable DTMRI(name,lebihan) -orient vertical -resolution 10 -width 10} $Gui(WSA)
        pack $f.lLeBihan $f.eEntrylebihan $f.slebihan  -side left -padx $Gui(pad) -pady 0 -fill x -padx $Gui(pad)
        TooltipAdd $f.eEntrylebihan "Diffusion weighting factor, introduced and defined by LeBihan et al.(1986)"
      
    }

    #-------------------------------------------
    # Convert->Pattern->FrameCont-->Create frame
    #-------------------------------------------

    set f $FrameCont.fCreate
    DevAddButton $f.bCreate "Create New Protocol" DTMRICreatePatternSlice 8
    pack $f.bCreate -side top -pady $Gui(pad) -fill x
    TooltipAdd $f.bCreate "Click this button to create a new protocol after filling in parameters entries"
    

    #-------------------------------------------
    # Convert->Pattern->FrameInter-->Create frame
    #-------------------------------------------

    set f $FrameInter.fCreate
    DevAddButton $f.bCreate "Create New Protocol" DTMRICreatePatternVolume 8
    pack $f.bCreate -side top -pady $Gui(pad) -fill x
    TooltipAdd $f.bCreate "Click this button to create a new protocol after filling in parameters entries"



}


proc DTMRIConvertUpdate {} {
  global DTMRI Volume Module
  
  set id $DTMRI(convertID)
  
  #Check if DTMRI headerKeys exits
  set headerkey [array names Volume "$id,headerKeys,modality"]
  
  set f $Module(DTMRI,fConv).fConvert
  if {$headerkey == ""} {
    #Active protocol frame
    $f.fPattern.mbPattern configure -state normal
    $f.fRepetitions.e configure -state normal
    $f.fRepetitions.s configure -state normal
    foreach vis $DTMRI(convert,averageRepetitionsList) {
      $f.fAverage.r$vis configure -state normal
    }
    
    set DTMRI(convert,nrrd) 0  
    return
  }
  
  if {$Volume($headerkey) != "DWMRI"} {
    # Prompt advise
    puts "Selected volume is not a proper nrrd DWI volume"
    return
  }     
  
  
  set headerkeys [array names Volume "$id,headerKeys,DW*"]
  
  if {$headerkeys == ""} {
     #Active protocols frame
     puts "There is not protocol info. Nrrd header might be corrupted."
     puts "If you feel conformtable, choose a protocol"
     return
  }
  
  #At this point with are dealing with a Nrrd DWI volume.
  
  #Disable protocol frame
  $f.fPattern.mbPattern configure -state disable
  $f.fRepetitions.e configure -state disable
  $f.fRepetitions.s configure -state disable
  foreach vis $DTMRI(convert,averageRepetitionsList) {
      $f.fAverage.r$vis configure -state disable
  }  

  #Build protocol from headerKeys
  set key DWMRI_b-value
  set DTMRI(convert,lebihan) $Volume($id,headerKeys,$key) 
  #Find baseline
  set DTMRI(convert,gradients) ""
  set gradientkeys [array names Volume "$id,headerKeys,DWMRI_gradient_*" ]
  
  set DTMRI(convert,numberOfGradients) 0
  
  set baselinepos 1
  set keyprefix "$id,headerKeys"
  set gradprefix "$keyprefix,DWMRI_gradient_"
  set nexprefix "$keyprefix,DWMRI_NEX_"

  set idx 0
  while {1} {
    set grad [format %04d $idx]
    set key "$gradprefix$grad"
    
    
    if {![info exists Volume($key)]} {
      break
    }
    
    #Check for baseline
    if {$Volume($key) == " 0  0  0"} {
      #Check for NEX
      set keynex "$nexprefix$grad"
      if {[info exists Volume($keynex)]} {
        set nex $Volume($keynex)
      } else {
        set nex 1
      }
      set DTMRI(convert,firstNoGradientImage) [expr $idx + 1]
      set DTMRI(convert,lastNoGradientImage) [expr $idx + $nex]
      set idx [expr $idx + $nex]
     } else {
      set keynex "$nexprefix$grad"
      if {[info exists Volume($keynex)]} {
        set nex $Volume($keynex)
      } else {
        set nex 1
      }
      for {set nidx 0} {$nidx < $nex} {incr nidx} {
        lappend DTMRI(convert,gradients) $Volume($key)
        incr DTMRI(convert,numberOfGradients)
      }
      set idx [expr $idx + $nex]    
    }
    
  }
  
  set DTMRI(convert,firstGradientImage) [expr $DTMRI(convert,lastNoGradientImage) + 1]
  set DTMRI(convert,lastGradientImage) [expr $DTMRI(convert,numberOfGradients) + $DTMRI(convert,lastNoGradientImage)]   
  
         
  #Nrrd by default is VOLUME-Interslice
  set DTMRI(convert,order) "VOLUME"
     
  #Measure frame: In VolNRRD we have converted measurement frame
  # from an attribute to an header key. This is a temporary solution
  # to accomodate somehow until this information is incorporated in
  # vtkMrmrlVolumeNode.
  set key "measurementframe"
  set DTMRI(convert,measurementframe) $Volume($id,headerKeys,$key)
  
  #Set nrrd flag
  set DTMRI(convert,nrrd) 1
       
  #Disable protocols

}

#-------------------------------------------------------------------------------
# .PROC RunLSDIrecon
# Convert volume data from scanner to a module readable data 
# so that DTMRI can convert tensors
#  active MRML node (this is NEW or an existing node). 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc RunLSDIrecon {} {
    global DTMRI Volume  Mrml Module PACKAGE_DIR_VTKDTMRI
    
    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create new data from NONE Volume. Load a Volume Data and Select it."
        DevInfoWindow "Can't create new data from NONE Volume.\n Load a Volume Data and Select it."
        return
    }

    #
    # Copy LSDIrecon.par in the selected volume data directory  
    #

    #puts $Mrml(dir)/Modules/vtkDTMRI/LSDIrecon_par
    #puts $Volume(DefaultDir)

    if {![file exists $PACKAGE_DIR_VTKDTMRI/../../../LSDIrecon_par]} {
        DevErrorWindow "Error: script file not found: Modules/vtkDTMRI/LSDIrecon_par"
        return
    }
    puts "Copying LSDIrecon_par to Volume Data directory $Volume(DefaultDir)..."
    set a [catch {file copy -force $PACKAGE_DIR_VTKDTMRI/../../../LSDIrecon_par $Volume(DefaultDir)} errMsg]
    if {$a} {
       DevInfoWindow "You don't have permission to write in the selected directory\n$Volume(DefaultDir).\n$errMsg"
       return
    }

    puts "Changing to Volume Data directory..."
    cd $Volume(DefaultDir)

    #
    # Running LSDI script 
    #

    if {[file exists $DTMRI(LSDIpydir)] == 1 &&
        [file executable $DTMRI(pythonintdir)] == 1} {
        puts "Creating new volume data (D.###)..."

        catch {exec $DTMRI(pythonintdir) $DTMRI(LSDIpydir)} convertingerror
    } else {
        DevInfoWindow "Error: Cannot find $DTMRI(LSDIpydir)\nor execute $DTMRI(pythonintdir)\nUnable to create new volume data"
        return
    }
}


#-------------------------------------------------------------------------------
# .PROC ShowPatternFrame
#  Show and hide Create-Pattern Frame from the Convert Tab.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
    proc ShowPatternFrame {} {
    
    global DTMRI Volume Mrml Module Gui

    set fConvert $Module(DTMRI,fConv)
    set f $fConvert

    if { $DTMRI(convert,show) == 1} {
        pack forget $f.fPattern
        set DTMRI(convert,show) 0
        return
    }

    if { $DTMRI(convert,show) == 0 } {
        pack $f.fPattern -padx $Gui(pad) -pady $Gui(pad)
    
        set DTMRI(convert,show) 1
        return
    }

}

#-------------------------------------------------------------------------------
# .PROC DTMRIDisplayScrollBar
#  If the size of a workframe changes, display the scrollbar if necessary.
#  
# .ARGS
# string module
# string tab
# .END
#-------------------------------------------------------------------------------
proc DTMRIDisplayScrollBar {module tab} {
    global Module

    set reqHeight [winfo reqheight $Module($module,f$tab)]
    # puts $reqHeight 
    # puts $Module(.tMain.fControls,scrolledHeight)
    MainSetScrollbarHeight $reqHeight
    if {$reqHeight > $Module(.tMain.fControls,scrolledHeight)} { 
        MainSetScrollbarVisibility 1
    } else {
        MainSetScrollbarVisibility 0
    }

}


#-------------------------------------------------------------------------------
# .PROC DTMRICreatePatternSlice
# Write new patterns defined by user in $env(HOME)/PatternData and update patterns selectbutton
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICreatePatternSlice {} {
        global Module Gui Volume DTMRI Mrml env

        set DTMRI(patternpar) ""

        # check if name field is filled in
        if {$DTMRI(name,name) != ""} {

            # check if all fields are filled in
            foreach par {numberOfGradients firstGradientImage lastGradientImage firstNoGradientImage lastNoGradientImage lebihan gradients} {
       
                if {$DTMRI(name,$par) != ""} {

                    # put information of the entries of create pattern frame in a list in order to write this information in a file
                    lappend DTMRI(patternpar) $DTMRI(name,$par)

                } else {

                    puts "You must fill in $par entry"
                    break

                }

            }

            lappend DTMRI(patternpar) "SLICE"

        } else {

            puts "You must fill in name entry"
        return

        }



        if {[file exists $env(HOME)/PatternsData/] != 1} then {

            file mkdir $env(HOME)/PatternsData/

        }

        if {$DTMRI(name,name) != ""} {
    
            if {[file exists $env(HOME)/PatternsData/$DTMRI(name,name)] != 0} then {

                puts "You are modifying an existing file"

            }
    
            set filelist [open $env(HOME)/PatternsData/$DTMRI(name,name) {RDWR CREAT}]
            puts  $filelist "# This line is the label that tells the code that this is a pattern file"
            puts  $filelist "vtkDTMRIprotocol"
            puts  $filelist "\n "
            puts  $filelist "# Enter a new pattern in the following order\n"
            #seek $filelist -0 end
            puts  $filelist "# Name NoOfGradients FirstGradient LastGradient FirstBaseLine LastBaseLine Lebihan GradientDirections\n"
            #seek $filelist -0 end
            puts  $filelist "\n "
            #seek $filelist -0 end
            puts $filelist $DTMRI(patternpar)    
            close $filelist
        
            DTMRILoadPattern

        }
 
}


#-------------------------------------------------------------------------------
# .PROC DTMRICreatePatternVolume
# Write new patterns defined by user in $env(HOME)/PatternData and update patterns selectbutton
#  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRICreatePatternVolume {} {
      global Module Gui Volume DTMRI Mrml env

      set DTMRI(patternpar) ""

      # check if name field is filled in
      if {$DTMRI(name,name) != ""} {

        # check if all fields are filled in
        foreach par {numberOfGradients firstGradientImage lastGradientImage firstNoGradientImage lastNoGradientImage lebihan gradients} {
       
          if {$DTMRI(name,$par) != ""} {

              # put information of the entries of create pattern frame in a list in order to write this information in a file
              lappend DTMRI(patternpar) $DTMRI(name,$par)

          } else {

              puts "You must fill in $par entry"
              break

          }

        }

        lappend DTMRI(patternpar) "VOLUME"

      } else {

        puts "You must fill in name entry"
        return

      }




      if {[file exists $env(HOME)/PatternsData/] != 1} then {

          file mkdir $env(HOME)/PatternsData/

      }
    
      if {$DTMRI(name,name) != ""} {

          if {[file exists $env(HOME)/PatternsData/$DTMRI(name,name)] != 0} then {

              puts "You are modifying an existing file"

          }

          set filelist [open $env(HOME)/PatternsData/$DTMRI(name,name) {RDWR CREAT}]
          puts  $filelist "# This line is the label that tells the code that this is a pattern file"
          puts  $filelist "vtkDTMRIprotocol"
          puts  $filelist "\n "
          puts  $filelist "# Enter a new pattern in the following order\n"
          #seek $filelist -0 end
          puts  $filelist "# Name NoOfGradients FirstGradient LastGradient FirstBaseLine LastBaseLine Lebihan GradientDirections\n"
          #seek $filelist -0 end
          puts  $filelist "\n "
          #seek $filelist -0 end
          puts $filelist $DTMRI(patternpar)    
          close $filelist

          DTMRILoadPattern

      }
} 



#-------------------------------------------------------------------------------
# .PROC DTMRILoadPattern
# Looks for files with information of patterns and adds this information in the menubutton of the create pattern frame
#nowworking2
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRILoadPattern {} {
    global Module Gui Volume DTMRI Mrml env PACKAGE_DIR_VTKDTMRI

    # if DTMRI(patternames) already exists, initialize it and its information
   
    if {[info exists DTMRI(patternnames)]} {
        foreach a $DTMRI(patternnames) {
            set DTMRI($a,parameters) ""
        }
        set DTMRI(patternnames) ""
    } else {
        set DTMRI(patternnames) ""
    }

    if {[info exists DTMRI(patternnamesdef)]} {
        foreach a $DTMRI(patternnamesdef) {
            set DTMRI($a,parameters) ""
        }
        set DTMRI(patternnamesdef) ""
    }

    # look for a file containing pattern information, if it exists, put this information in variable lists

    set DTMRI(patternFiles) ""
    set home [file normalize $env(HOME)]
    if { [file isdirectory $home/PatternsData/] } {
        eval lappend DTMRI(patternFiles) [glob $home/PatternsData/*]
    }

    if { [file isdirectory $PACKAGE_DIR_VTKDTMRI/../../../data/] } {
        eval lappend DTMRI(patternFiles) [glob $PACKAGE_DIR_VTKDTMRI/../../../data/*]
    }

    # put pattern information into modules variables
    foreach pfile $DTMRI(patternFiles) {
        set pattern [file tail $pfile]
        set pfile_valid 0
        if { [file readable $pfile] && [file isfile $pfile] } {
            set fp [open $pfile]
            while { ![eof $fp] } {

                set line [gets $fp]

                if {[lindex $line 0] == "vtkDTMRIprotocol"} {
                    set pfile_valid 1
                    continue
                }

                if { $line == "" || [string match "#*" $line] } {
                    continue
                }

                # only non-blank, non-comment, non-magic line is list of parameters
                set DTMRI($pattern,parameters) $line 
            }
            close $fp 

            if { $pfile_valid } {
                lappend DTMRI(patternnames) $pattern
            } else {
                unset -nocomplain DTMRI($pattern,parameters)
            }
        }
    }

    destroy $Module(DTMRI,fConv).fConvert.fPattern.mbPattern.menu
    eval {menu $Module(DTMRI,fConv).fConvert.fPattern.mbPattern.menu}  $Gui(WMA)

    # load existing patterns in the menu of the menubutton
    foreach z $DTMRI(patternnames) {
        set DTMRI(patt) $z
        pack forget $Module(DTMRI,fConv).fConvert.fPattern.mbPattern      
        $Module(DTMRI,fConv).fConvert.fPattern.mbPattern.menu add command -label $z -command "
        set DTMRI(selectedpattern) $DTMRI(patt)
        $Module(DTMRI,fConv).fConvert.fPattern.mbPattern config -text $DTMRI(patt) 
        set DTMRI($DTMRI(patt),tip) {Selected Protocol:\n $DTMRI(patt) \n Number of gradients:\n [lindex $DTMRI($DTMRI(patt),parameters) 0] \n First Gradient in Slice:\n [lindex $DTMRI($DTMRI(patt),parameters) 1] \n Last Gradient in Slice:\n [lindex $DTMRI($DTMRI(patt),parameters) 2] \n Baselines:\n from [lindex $DTMRI($DTMRI(patt),parameters) 3] to [lindex $DTMRI($DTMRI(patt),parameters) 4] \n B-value:\n [lindex $DTMRI($DTMRI(patt),parameters) 5] \n Gradients Directions:\n [lindex $DTMRI($DTMRI(patt),parameters) 6] \n The gradient order is:\n [lindex $DTMRI($DTMRI(patt),parameters) 7] interleaved}
     
        "

    }  

    pack  $Module(DTMRI,fConv).fConvert.fPattern.mbPattern -side left -padx $Gui(pad) -pady $Gui(pad) -after $Module(DTMRI,fConv).fConvert.fPattern.lLabel
}

 

#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateTipsPattern
#  Commented out
# .ARGS
# .END
#-------------------------------------------------------------------------------
#proc DTMRIUpdateTipsPattern {} {

#tkwait variable $DTMRI(selectedpattern)
#after 1000
#puts "Reading Module"

#catch {TooltipAdd $Module(DTMRI,fConv).fConvert.fPattern.mbPattern $DTMRI($DTMRI(selectedpattern),tip)}

#puts $DTMRI($DTMRI(selectedpattern),tip)

#}

  

#-------------------------------------------------------------------------------
# .PROC DTMRIViewProps
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIViewProps {} {
    puts $DTMRI(selectedpattern)

    if { [info exists DTMRI(selectedpattern)] } {
        DevInfoWindow $DTMRI($DTMRI(selectedpattern),tip)
    }

}
                                         

#-------------------------------------------------------------------------------
# .PROC DTMRIDisplayNewData
#  Once converted the volume data with LSDI script, load and display new data.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIDisplayNewData {} {
    
    global DTMRI Volume  Mrml Module
    
    set v $Volume(activeID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create new data from NONE Volume. Load a Volume Data and Select it."
        DevInfoWindow "Can't create new data from NONE Volume.\n Load a Volume Data and Select it."
        return
    }

    #
    # Copy LSDIrecon.par in the selected volume data directory  
    #

    

    puts "Ready."

    #
    # Load the new volume data (D.#) 
    #

    set Volume(activeID) NEW
    set Volume(firstFile) $Volume(DefaultDir)/D.001
    VolumesSetFirst
    VolumesSetLast
    puts "Reading $Volume(name)..."
    VolumesPropsApply
    puts "Displaying New Volume Data..."
    RenderAll
    

  
}




################################################################
# procedures for converting volumes into DTMRIs.
# TODO: this should happen automatically and be in MRML
################################################################

#-------------------------------------------------------------------------------
# .PROC ConvertVolumeToTensors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ConvertVolumeToTensors {} {
    global DTMRI Volume Tensor

    set v $DTMRI(convertID)
    if {$v == "" || $v == $Volume(idNone)} {
        puts "Can't create DTMRIs from None volume"
        return
    }

    # DTMRI creation filter
    catch "vtkImageDiffusionTensor DTMRI"
    
    if {$DTMRI(convert,nrrd) == 0} {
    puts "Loading pattern"
    if {[info exists DTMRI(selectedpattern)]} {
        
        set DTMRI(convert,numberOfGradients) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 0]
        set DTMRI(convert,firstGradientImage) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 1]
        set DTMRI(convert,lastGradientImage) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 2]
        set DTMRI(convert,firstNoGradientImage) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 3]
        set DTMRI(convert,lastNoGradientImage) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 4]
        set DTMRI(convert,lebihan) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 5]
        set DTMRI(convert,gradients) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 6]
        set DTMRI(convert,order) [lindex $DTMRI($DTMRI(selectedpattern),parameters) 7]
        
        #Set b-factor
        DTMRI SetB $DTMRI(convert,lebihan)
        
    } else {
        DevErrorWindow "Please select a protocol"
        DTMRI Delete
        return
        
    }
    }

# define if the conversion is volume interleaved or slice interleaved depending on the pattern


    # setup - these are now globals linked with GUI
    #set slicePeriod 8
    #set offsetsGradient "0 1 2 3 4 5"
    #set offsetsNoGradient "6 7"
    #set numberOfGradientImages 6
    #set numberOfNoGradientImages 2
    set count 0
    for {set i $DTMRI(convert,firstGradientImage)} \
            {$i  <= $DTMRI(convert,lastGradientImage) } \
            {incr i} {
        # 0-based offsets, so subtract 1
        lappend offsetsGradient [expr $i -1]
        incr count
    }
    puts $offsetsGradient
    set numberOfGradientImages $count
    set count 0
    for {set i $DTMRI(convert,firstNoGradientImage)} \
            {$i  <= $DTMRI(convert,lastNoGradientImage) } \
            {incr i} {
        # 0-based offsets, so subtract 1
        lappend offsetsNoGradient [expr $i -1]
        incr count
    }
    puts $offsetsNoGradient
    set numberOfNoGradientImages $count
    
    set slicePeriod \
    [expr $numberOfGradientImages+$numberOfNoGradientImages]
    
    puts "Slice period: $slicePeriod, Num No grad:$numberOfNoGradientImages" 
    
    set numberOfGradientImages $DTMRI(convert,numberOfGradients) 
    DTMRI SetNumberOfGradients $numberOfGradientImages

    #puts $offsetsGradient 
    #puts $offsetsNoGradient

    for {set i 0} {$i < $DTMRI(convert,numberOfGradients)} {incr i} {
    #    eval {DTMRI SetDiffusionGradient $i} [lindex DTMRI(convert,gradients) $i]
        eval {DTMRI SetDiffusionGradient $i} [lindex $DTMRI(convert,gradients) $i]
    }

    # volume we use for input
    set input [Volume($v,vol) GetOutput]

    # transform gradient directions to make DTMRIs in ijk
    catch "vtkTransform trans"    
    # special trick to avoid obnoxious windows warnings about legacy hack
    # for vtkTransform
    trans AddObserver WarningEvent ""

    puts "If not phase-freq flipped, swapping x and y in gradient directions"
    set swap [Volume($v,node) GetFrequencyPhaseSwap]
    set scanorder [Volume($v,node) GetScanOrder]
    
    
    
    #Two options: measurement frame known or not known.
    
    
    if {$DTMRI(convert,nrrd)} {
    
      #Get RAS To Ijk Matrix
      catch "vtkMatrix4x4 _RasToIjk"
      _RasToIjk DeepCopy [Volume($v,node) GetRasToIjk]
      #Ignore translation
      _RasToIjk SetElement 0 3 0
      _RasToIjk SetElement 1 3 0
      _RasToIjk SetElement 2 3 0
      
      #Set measurement frame matrix
      catch "vtkMatrix4x4 _MF"
      foreach axis "0 1 2" {
        set axdir [lindex $DTMRI(convert,measurementframe) $axis]
        foreach c "0 1 2" {
      _MF SetElement $c $axis [lindex $axdir $c]
    }
      }
            
      trans PostMultiply
      trans SetMatrix _MF
      trans Concatenate _RasToIjk
      trans Update
      
      _RasToIjk Delete
      _MF Delete
      
    } else {         
      
      if {$swap == 0} {    
        # Gunnar Farneback, April 6, 2004
        #
        # Apparently nobody understands all the involved coordinate
        # systems well enough to actually know how the gradient
        # directions should be transformed. This piece of code is
        # based on the hypothesis that the transformation matrices
        # only need to depend on the scan order and that the values
        # can be determined experimentally. It is perfectly possible
        # that this may break from changes elsewhere.
        #
        # If somebody reading this does know how to properly do these
        # transforms, please replace this code with something better.
        #
        # So far IS and PA have been experimentally verified.
        # SI is hypothesized to be the same as IS.
        # AP is hypothesized to be the same as PA.

        puts $scanorder
        switch $scanorder {
            "SI" -
            "IS" {
            set elements "\
                                    {0 1 0 0}  \
                                    {1 0 0 0}  \
                                    {0 0 -1 0}  \
                                    {0 0 0 1}  "
            }
            "AP" -
            "PA" {
            set elements "\
                                    {0 1 0 0}  \
                                    {-1 0 0 0}  \
                                    {0 0 -1 0}  \
                                    {0 0 0 1}  "
            }
            default {
            set elements "\
                                    {0 1 0 0}  \
                                    {1 0 0 0}  \
                                    {0 0 1 0}  \
                                    {0 0 0 1}  "
            }
        }

        set rows {0 1 2 3}
        set cols {0 1 2 3}    
        foreach row $rows {
            foreach col $cols {
                [trans GetMatrix] SetElement $row $col \
                    [lindex [lindex $elements $row] $col]
            }
        }    
    } else { 
        puts "Creating DTMRIs with -y for vtk compliance"
        trans Scale 1 -1 1
    }
    
    }

    #Hardcode specific parameters for MOSAIC. Experimental.
    if {$DTMRI(convert,order) == "MOSAIC"} {
      DTMRI SetAlpha 50
      set scanorder "IS"
      trans Identity
      trans Scale 1 1 -1
      foreach plane {0 1 2} {
        $DTMRI(mode,glyphsObject$plane) SetScaleFactor 2000
      }
    }
    
    DTMRI SetAlpha 50
    
    DTMRI SetTransform trans
    trans Delete

    #check if input correct

    set dimz [lindex [$input GetDimensions] 2]
    set rest [expr $dimz%$slicePeriod]
    puts "Rest: $rest, Dimz: $dimz, slice Period: $slicePeriod"
   if {$rest != 0 && $DTMRI(convert,order) == "VOLUME"} {
       DevErrorWindow "Check your Input Data.\n Not enough number of slices"
       DTMRI Delete
       return
   }

  if {$DTMRI(convert,order) == "MOSAIC"} {
    #Build list of DICOM files
    set numFiles [Volume($v,node) GetNumberOfDICOMFiles]
    for {set k 0} {$k < $numFiles} {incr k} {
      lappend filesList [Volume($v,node) GetDICOMFileName $k]
    }
    set sortList [lsort -dictionary $filesList]
    
    set numElements [expr $numberOfNoGradientImages + $numberOfGradientImages]
    
    for {set k 0} {$k < $numElements} {incr k} {
      lappend mosaicIndx [lsearch -dictionary $filesList [lindex $sortList $k ]]
    }
    
    puts "$numberOfNoGradientImages"
    puts "$numberOfNoGradientImages"
    puts "Num Elements: $numElements"
    puts "Mosaic Indx: $mosaicIndx"
    
    
  }

        

    # produce input vols for DTMRI creation
    set inputNum 0
    set DTMRI(recalculate,gradientVolumes) ""
    foreach slice $offsetsGradient {
        vtkImageExtractSlices extract$slice
        extract$slice SetInput $input
        extract$slice SetModeTo$DTMRI(convert,order)
        extract$slice SetSliceOffset $slice
        extract$slice SetSlicePeriod $slicePeriod
        extract$slice SetNumberOfRepetitions $DTMRI(convert,numberOfRepetitions)
        extract$slice SetAverageRepetitions $DTMRI(convert,averageRepetitions)
        
        if {$DTMRI(convert,order) == "MOSAIC"} {
          extract$slice SetSliceOffset [lindex $mosaicIndx $slice]   
          extract$slice SetMosaicTiles $DTMRI(convert,mosaicTiles)
          extract$slice SetMosaicSlices $DTMRI(convert,mosaicSlices)
        }

        #puts "----------- slice $slice update --------"    
        extract$slice Update
        #puts [[extract$slice GetOutput] Print]

        # pass along in pipeline
        DTMRI SetDiffusionImage \
            $inputNum [extract$slice GetOutput]
        incr inputNum
        
        # put the filter output into a slicer volume
        # Lauren this should be optional
        # make a MRMLVolume for this output
        if {[expr $slice % 5] == 0 && $DTMRI(convert,makeDWIasVolume)==1} {
          set name [Volume($v,node) GetName]
          set description "$slice gradient volume derived from volume $name"
          set name gradient${slice}_$name
          if {$DTMRI(convert,order) == "MOSAIC"} {
            set id [DTMRICreateNewVolume [extract$slice GetOutput] $name $description $scanorder]
          } else {
            set id [DevCreateNewCopiedVolume $v $description $name]
            Volume($id,vol) SetImageData [extract$slice GetOutput]
          }
        
          # save id in case we recalculate the DTMRIs
          lappend DTMRI(recalculate,gradientVolumes) $id
          puts "created volume $id"
          # fix the image range in the node (less slices than the original)
          set extent [[Volume($id,vol) GetOutput] GetExtent]
          set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
          eval {Volume($id,node) SetImageRange} $range
          # recompute the matrices using this offset to center vol in the cube
          # for some reason this uses the wrong node spacing!
          # Lauren test 
          eval {Volume($id,node) SetSpacing} [Volume($id,node) GetSpacing]

          set order [Volume($id,node) GetScanOrder]
          puts "-------computing ras to ijk from scan order----"
          Volume($id,node) ComputeRasToIjkFromScanOrder $order
          
      if {$DTMRI(convert,nrrd)} {
            DTMRIComputeRasToIjkFromCorners Volume($v,node) Volume($id,node) $extent
          }

          # update slicer internals
          MainVolumesUpdate $id

          # Registration
          # put the new volume inside the same transform as the Original volume
          # by inserting it right after that volume in the mrml file
          set nitems [Mrml(dataTree) GetNumberOfItems]
          for {set widx 0} {$widx < $nitems} {incr widx} {
            if { [Mrml(dataTree) GetNthItem $widx] == "Volume($id,node)" } {
                break
            }
          }
          if { $widx < $nitems } {
            Mrml(dataTree) RemoveItem $widx
            Mrml(dataTree) InsertAfterItem Volume($v,node) Volume($id,node)
            MainUpdateMRML
         }
        
       }
        
    }
    # save ids in case we recalculate the DTMRIs
    set DTMRI(recalculate,noGradientVolumes) ""
    foreach slice $offsetsNoGradient {
        vtkImageExtractSlices extract$slice
        extract$slice SetInput $input
        extract$slice SetModeTo$DTMRI(convert,order)
        extract$slice SetSliceOffset $slice
        extract$slice SetSlicePeriod $slicePeriod
        extract$slice SetNumberOfRepetitions $DTMRI(convert,numberOfRepetitions)
        extract$slice SetAverageRepetitions $DTMRI(convert,averageRepetitions)
        
        
        if {$DTMRI(convert,order) == "MOSAIC"} {
          puts "[lindex $mosaicIndx $slice]"
          eval "extract$slice SetSliceOffset" [lindex $mosaicIndx $slice]     
          extract$slice SetMosaicTiles $DTMRI(convert,mosaicTiles)
          extract$slice SetMosaicSlices $DTMRI(convert,mosaicSlices)
        }
        #puts "----------- slice $slice update --------"    
        extract$slice Update


        # put the filter output into a slicer volume
        # Lauren this should be optional
        # make a MRMLVolume for this output
        if {[expr $slice % 2] == 0 && $DTMRI(convert,makeDWIasVolume)==1} {
          set name [Volume($v,node) GetName]
          set name noGradient${slice}_$name
          set description "$slice no gradient volume derived from volume $name"
          if {$DTMRI(convert,order) == "MOSAIC"} {
            set id [DTMRICreateNewVolume [extract$slice GetOutput] $name $description $scanorder]
          } else {
            set id [DevCreateNewCopiedVolume $v $description $name]
            Volume($id,vol) SetImageData [extract$slice GetOutput]
          }
        
          # save id in case we recalculate the DTMRIs
          lappend DTMRI(recalculate,noGradientVolumes) $id
          puts "created volume $id"
          # fix the image range in the node (less slices than the original)
          set extent [[Volume($id,vol) GetOutput] GetExtent]
          set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
          eval {Volume($id,node) SetImageRange} $range
          # recompute the matrices using this offset to center vol in the cube
          set order [Volume($id,node) GetScanOrder]
          Volume($id,node) ComputeRasToIjkFromScanOrder $order
          
      if {$DTMRI(convert,nrrd)} {
            DTMRIComputeRasToIjkFromCorners Volume($v,node) Volume($id,node) $extent
          }
          # update slicer internals
          MainVolumesUpdate $id

          # Registration
          # put the new volume inside the same transform as the Original volume
          # by inserting it right after that volume in the mrml file
          set nitems [Mrml(dataTree) GetNumberOfItems]
          for {set widx 0} {$widx < $nitems} {incr widx} {
            if { [Mrml(dataTree) GetNthItem $widx] == "Volume($id,node)" } {
                break
            }
          }
          if { $widx < $nitems } {
            Mrml(dataTree) RemoveItem $widx
            Mrml(dataTree) InsertAfterItem Volume($v,node) Volume($id,node)
            MainUpdateMRML
          }

          # display this volume so the user knows something happened
          MainSlicesSetVolumeAll Back $id
        }
    }

 
    if {$numberOfNoGradientImages > 1} {
      vtkImageMathematics math
      math SetOperationToAdd

      catch "vtkImageCast _cast"
      _cast SetInput [extract[lindex $offsetsNoGradient 0] GetOutput]
      _cast SetOutputScalarTypeToFloat
      _cast Update
      
      vtkImageData slicebase 
      slicebase DeepCopy [_cast GetOutput]
      
      for {set k 1} {$k < $numberOfNoGradientImages} {incr k} {
        
        _cast SetInput [extract[lindex $offsetsNoGradient $k] GetOutput]
        _cast SetOutputScalarTypeToFloat
    _cast Update
        set slicechange [_cast GetOutput]
        math SetInput 0 slicebase
        math SetInput 1 $slicechange
        math Update
        slicebase DeepCopy [math GetOutput]
      }
      slicebase Delete
      vtkImageMathematics math2
      math2 SetOperationToMultiplyByK
      math2 SetConstantK [expr 1.0 / $numberOfNoGradientImages]
      math2 SetInput 0 [math GetOutput]
      math2 SetInput 1 ""
      math2 Update
      
      _cast Delete
      catch "vtkImageCast _cast"
      _cast SetInput [math2 GetOutput]
      _cast SetOutputScalarType [[extract[lindex $offsetsNoGradient 0] GetOutput] GetScalarType]
      _cast Update
 
      
      # set the no diffusion input
      #DTMRI SetNoDiffusionImage [extract6 GetOutput]
      DTMRI SetNoDiffusionImage [_cast GetOutput]
      set baseline [_cast GetOutput]
            
      
    } else {
      set slice [lindex $offsetsNoGradient 0]
      DTMRI SetNoDiffusionImage [extract$slice GetOutput]
      set baseline [extract$slice GetOutput]
    }
    
    #Make a MRML node with BaseLine
     set name [Volume($v,node) GetName]
     set description "Baseline from volume $name"
     set name ${name}_Baseline
     if {$DTMRI(convert,order) == "MOSAIC"} {
        set id [DTMRICreateNewVolume $baseline $name $description $scanorder]
     } else {
        set id [DevCreateNewCopiedVolume $v $description $name]
        Volume($id,vol) SetImageData $baseline
     }
        
     puts "created volume $id"
     # fix the image range in the node (less slices than the original)
     set extent [[Volume($id,vol) GetOutput] GetExtent]
     set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
     eval {Volume($id,node) SetImageRange} $range
     # recompute the matrices using this offset to center vol in the cube
     # for some reason this uses the wrong node spacing!
     # Lauren test 
      eval {Volume($id,node) SetSpacing} [Volume($id,node) GetSpacing]
      set order [Volume($id,node) GetScanOrder]
      puts "-------computing ras to ijk from scan order----"
      Volume($id,node) ComputeRasToIjkFromScanOrder $order
      
      if {$DTMRI(convert,nrrd)} {
        DTMRIComputeRasToIjkFromCorners Volume($v,node) Volume($id,node) $extent
      }
      # update slicer internals
      MainVolumesUpdate $id

      # Registration
      # put the new volume inside the same transform as the Original volume
      # by inserting it right after that volume in the mrml file
      set nitems [Mrml(dataTree) GetNumberOfItems]
       for {set widx 0} {$widx < $nitems} {incr widx} {
         if { [Mrml(dataTree) GetNthItem $widx] == "Volume($id,node)" } {
             break
         }
       }
       if { $widx < $nitems } {
         Mrml(dataTree) RemoveItem $widx
         Mrml(dataTree) InsertAfterItem Volume($v,node) Volume($id,node)
         MainUpdateMRML
      }


      # average gradient images for display and checking mechanism. 
      catch "vtkImageMathematics math_g"
      math_g SetOperationToAdd
      
      
      catch "vtkImageCast _cast_g"
      _cast_g SetInput [extract[lindex $offsetsGradient 0] GetOutput]
      _cast_g SetOutputScalarTypeToFloat
      _cast_g Update
      
      vtkImageData slicebase
      slicebase DeepCopy [_cast_g GetOutput]
      
      for {set k 1} {$k < $numberOfGradientImages} {incr k} {
        _cast_g SetInput [extract[lindex $offsetsGradient $k] GetOutput]
        _cast_g SetOutputScalarTypeToFloat
        _cast_g Update
    
        set slicechange [_cast_g GetOutput]
        math_g SetInput 0 slicebase
        math_g SetInput 1 $slicechange
        math_g Update
        slicebase DeepCopy [math_g GetOutput]
      }
      slicebase Delete
      catch "vtkImageMathematics math2_g"
      math2_g SetOperationToMultiplyByK
      math2_g SetConstantK [expr 1.0 / $numberOfGradientImages]
      math2_g SetInput 0 [math_g GetOutput]
      math2_g SetInput 1 ""
      math2_g Update
      
      _cast_g Delete
      catch "vtkImageCast _cast_g"
      _cast_g SetInput [math2_g GetOutput]
      _cast_g SetOutputScalarType [[extract[lindex $offsetsGradient 0] GetOutput] GetScalarType]
      _cast_g Update
      
      set baseline [_cast_g GetOutput]
 
    #Make a MRML node with BaseLine
     set name [Volume($v,node) GetName]
     set description "Average gradient from volume $name"
     set name ${name}_AvGradient
     if {$DTMRI(convert,order) == "MOSAIC"} {
        set id [DTMRICreateNewVolume $baseline $name $description $scanorder]
     } else {
        set id [DevCreateNewCopiedVolume $v $description $name]
        Volume($id,vol) SetImageData $baseline
     }
        
     puts "created volume $id"
     # fix the image range in the node (less slices than the original)
     set extent [[Volume($id,vol) GetOutput] GetExtent]
     set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
     eval {Volume($id,node) SetImageRange} $range
     # recompute the matrices using this offset to center vol in the cube
     # for some reason this uses the wrong node spacing!
     # Lauren test 
      eval {Volume($id,node) SetSpacing} [Volume($id,node) GetSpacing]
      set order [Volume($id,node) GetScanOrder]
      puts "-------computing ras to ijk from scan order----"
      Volume($id,node) ComputeRasToIjkFromScanOrder $order
      
      if {$DTMRI(convert,nrrd)} {
        DTMRIComputeRasToIjkFromCorners Volume($v,node) Volume($id,node) $extent
      }
      # update slicer internals
      MainVolumesUpdate $id

      # Registration
      # put the new volume inside the same transform as the Original volume
      # by inserting it right after that volume in the mrml file
      set nitems [Mrml(dataTree) GetNumberOfItems]
       for {set widx 0} {$widx < $nitems} {incr widx} {
         if { [Mrml(dataTree) GetNthItem $widx] == "Volume($id,node)" } {
             break
         }
       }
       if { $widx < $nitems } {
         Mrml(dataTree) RemoveItem $widx
         Mrml(dataTree) InsertAfterItem Volume($v,node) Volume($id,node)
         MainUpdateMRML
      }



    puts "3----------- DTMRI update --------"
    #DTMRI DebugOn
    DTMRI Update
    puts "----------- after DTMRI update --------"


    # put output into a Tensor volume
    # Lauren if volumes and tensos are the same
    # this should be done like the above
    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume Tensor]
    #Take the baseline as node to copy
    $newvol Copy Volume($id,node)
    $newvol SetDescription "DTMRI volume"
    $newvol SetName "[Volume($v,node) GetName]_Tensor"
    set n [$newvol GetID]

    puts "SPACING [$newvol GetSpacing] DIMS [$newvol GetDimensions] MAT [$newvol GetRasToIjkMatrix]"
    # fix the image range in the node (less slices than the original)
    set extent [[Volume($id,vol) GetOutput] GetExtent]
    set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
    eval {$newvol SetImageRange} $range
    # recompute the matrices using this offset to center vol in the cube
    set order [$newvol GetScanOrder]
    
    $newvol ComputeRasToIjkFromScanOrder $order
        
    puts "SPACING [$newvol GetSpacing] DIMS [$newvol GetDimensions] MAT [$newvol GetRasToIjkMatrix]"
    TensorCreateNew $n 
    
    # Set the slicer object's image data to what we created
    DTMRI Update
    #Tensor($n,data) SetData [DTMRI GetOutput]
    Tensor($n,data) SetImageData [DTMRI GetOutput]
    
    if {$DTMRI(convert,nrrd)} {
      DTMRIComputeRasToIjkFromCorners Volume($v,node) $newvol [[Tensor($n,data) GetOutput] GetExtent]
    }
    
    # Registration
    # put the new tensor volume inside the same transform as the Original volume
    # by inserting it right after that volume in the mrml file
    set nitems [Mrml(dataTree) GetNumberOfItems]
    for {set widx 0} {$widx < $nitems} {incr widx} {
        if { [Mrml(dataTree) GetNthItem $widx] == "Tensor($n,node)" } {
            break
        }
    }
    if { $widx < $nitems } {
        Mrml(dataTree) RemoveItem $widx
        Mrml(dataTree) InsertAfterItem Volume($v,node) Tensor($n,node)
        MainUpdateMRML
    }
    
    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML
    # If failed, then it's no longer in the idList
    if {[lsearch $Tensor(idList) $n] == -1} {
        puts "Lauren node doesn't exist, should unfreeze and fix volumes.tcltoo"
    } else {
        # Activate the new data object
        DTMRISetActive $n
    }


    # kill objects
    foreach slice $offsetsGradient {
        extract$slice SetOutput ""
        extract$slice Delete
    }
    foreach slice $offsetsNoGradient {
        extract$slice SetOutput ""
        extract$slice Delete
    }

    if {$numberOfNoGradientImages > 1} {
        math SetOutput ""
        math2 SetOutput ""
    _cast SetOutput ""
        math Delete
        math2 Delete
    _cast Delete
    }
    
    math_g SetOutput ""
    math2_g SetOutput ""
    math_g Delete
    math2_g Delete
    _cast_g SetOutput ""
    _cast_g Delete
    
    DTMRI SetOutput ""
    DTMRI Delete

    # display volume so the user knows something happened
    MainSlicesSetVolumeAll Back $id

    # display the new volume in the slices
    RenderSlices
}




#-------------------------------------------------------------------------------
# .PROC DTMRICreateNewVolume
# 
# .ARGS
# string volume
# string name
# string desc
# string scanOrder
# .END
#-------------------------------------------------------------------------------
proc DTMRICreateNewVolume {volume name desc scanOrder} {
  global Volume View
  
  set n [MainMrmlAddNode Volume]
  set id [$n GetID]
  MainVolumesCreate $id
  $n SetScanOrder $scanOrder     
  $n SetName $name
  $n SetDescription $desc
  set dim [$volume GetDimensions]
  eval "$n SetDimensions" [lindex $dim 0] [lindex $dim 1]
  eval "$n SetSpacing" [$volume GetSpacing]
  set extent [$volume GetExtent]
  set range "[expr [lindex $extent 4] +1] [expr [lindex $extent 5] +1]"
  eval {$n SetImageRange} $range
  $n ComputeRasToIjkFromScanOrder $scanOrder
  
  # get the pixel size, etc. from the data and set it in the node
  #[Volume($id,vol) GetOutput] DeepCopy $volume
  Volume($id,vol) SetImageData $volume
  MainUpdateMRML
  MainVolumesSetActive $id
  
  
  set fov 0
  for {set i 0} {$i < 2} {incr i} {
    set dim     [lindex [Volume($id,node) GetDimensions] $i]
    set spacing [lindex [Volume($id,node) GetSpacing] $i]
    set newfov     [expr $dim * $spacing]
    if { $newfov > $fov } {
       set fov $newfov
     }
  }
  set View(fov) $fov
  MainViewSetFov
  
  return $id

}

#-------------------------------------------------------------------------------
# .PROC DTMRIComputeRasToIjkFromCorners
# 
# .ARGS
# vtkMrmlNode refnode
# vtkMrmlNode volid: id of the Mrml node to compute the transform.
# array extent: extent of the volume. Needed to compute center
# .END
#-------------------------------------------------------------------------------
proc DTMRIComputeRasToIjkFromCorners {refnode node extent} {

  #Get Ras to Ijk Matrix from reference volume
  catch "_Ijk Delete"
  vtkMatrix4x4 _Ijk
  _Ijk DeepCopy [$refnode GetRasToIjk]
  
  #Set Translation to center of the output volume.
  #This is a particular thing of the slicer: all volumes are centered in their centroid.
  _Ijk SetElement 0 3 [expr ([lindex $extent 1] - [lindex $extent 0])/2.0]
  _Ijk SetElement 1 3 [expr ([lindex $extent 3] - [lindex $extent 2])/2.0]
  _Ijk SetElement 2 3 [expr ([lindex $extent 5] - [lindex $extent 4])/2.0]  
  
  #Trick: Negate y axis to compensate for flip in nrrd.
  _Ijk SetElement 1 0 [expr -1 * [_Ijk GetElement 1 0]]
  _Ijk SetElement 1 1 [expr -1 * [_Ijk GetElement 1 1]]
  _Ijk SetElement 1 2 [expr -1 * [_Ijk GetElement 1 2]]
  
  #Invert to obtain IjkToRas transform
  _Ijk Invert
  
  set dims "[expr [lindex $extent 1] - [lindex $extent 0] + 1] \
              [expr [lindex $extent 3] - [lindex $extent 2] + 1] \
              [expr [lindex $extent 5] - [lindex $extent 4] + 1]"           
  
   # first top left - start at zero, and add origin to all later
   set ftl "0 0 0"
   # first top right = width * row vector
   set ftr [lrange [_Ijk MultiplyPoint [lindex $dims 0] 0 0 0] 0 2]
   # first bottom right = ftr + height * column vector
   set column_vec [lrange [_Ijk MultiplyPoint 0 [lindex $dims 1] 0 0] 0 2]
   set fbr ""
   foreach ftr_e $ftr column_vec_e $column_vec {
        lappend fbr [expr $ftr_e + $column_vec_e]
    }
    # last top left = ftl + slice vector  (and ftl is zero)
    set ltl [lrange [_Ijk MultiplyPoint 0 0 [lindex $dims 2] 0] 0 2]


    # add the origin offset 
    set origin [lrange [_Ijk MultiplyPoint 0 0 0 1] 0 2]
    foreach corner "ftl ftr fbr ltl" {
        set new_corner ""
        foreach corner_e [set $corner] origin_e $origin {
            lappend new_corner [expr $corner_e + $origin_e]
        }
        set $corner $new_corner
    }

    eval $node ComputeRasToIjkFromCorners "0 0 0" $ftl $ftr $fbr "0 0 0" $ltl

    _Ijk Delete
    MainUpdateMRML

}
