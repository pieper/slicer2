#-------------------------------------------------------------------------------
# .PROC EMAtlasBrainClassifierInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMAtlasBrainClassifierInit {} {
    global EMAtlasBrainClassifier Module Volume env

    set m EMAtlasBrainClassifier

    set Module($m,overview) "Easy to use segmentation tool for brain MRIs"
    set Module($m,author)   "Kilian, Pohl, MIT, pohl@csail.mit.edu"
    set Module($m,category) "Segmentation"

    set Module($m,row1List) "Help Segmentation Advanced"
    set Module($m,row1Name) "{Help} {Segmentation} {Advanced}"
    set Module($m,row1,tab) Segmentation

    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI)   EMAtlasBrainClassifierBuildGUI
    set Module($m,procVTK)   EMAtlasBrainClassifierBuildVTK
    set Module($m,procEnter) EMAtlasBrainClassifierEnter
    set Module($m,procExit)  EMAtlasBrainClassifierExit
    set Module($m,procMRML)  EMAtlasBrainClassifierUpdateMRML

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    # Kilian: I  currently deactivated it so I wont get nestay error messages 
    set Module($m,depend) ""

    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.2 $} {$Date: 2004/12/16 19:24:00 $}]


    set EMAtlasBrainClassifier(Volume,SPGR) $Volume(idNone)
    set EMAtlasBrainClassifier(Volume,T2W)  $Volume(idNone)
    set EMAtlasBrainClassifier(Save,SPGR)    0
    set EMAtlasBrainClassifier(Save,T2W)     0
    set EMAtlasBrainClassifier(Save,Atlas)   1
    set EMAtlasBrainClassifier(Save,Segmentation) 1
    set EMAtlasBrainClassifier(Save,XMLFile) 1

    set EMAtlasBrainClassifier(WorkingDirectory) "$env(SLICER_HOME)/EMSeg"    
    set EMAtlasBrainClassifier(AtlasDir)         "$env(SLICER_HOME)/Modules/vtkEMAtlasBrainClassifier/atlas"   
    set EMAtlasBrainClassifier(XMLTemplate)      "$env(SLICER_HOME)/Modules/vtkEMAtlasBrainClassifier/template5_c2.xml"     

    set EMAtlasBrainClassifier(Normalize,SPGR) "90"
    set EMAtlasBrainClassifier(Normalize,T2W)  "310"

    if {[info exists EMAtlasBrainClassifier(Batch)] ==0} {
    set EMAtlasBrainClassifier(Batch) 0 
    } else {
    if {$EMAtlasBrainClassifier(Batch)} {
        puts "Run in Batch mode"  
    }
    }
    set EMAtlasBrainClassifier(eventManager) {}

}

proc EMAtlasBrainClassifierBuildGUI {} {
    global Gui EMAtlasBrainClassifier Module Volume 
    
    set help "The EMAtlasBrainClassifier module is an easy to use segmentation tool for Brain MRIs. Just define the Brain SPGR and T2W input images 
              and the tool will automatically segment the image into white matter , gray matter, and cortical spinal fluid. 
              Be warned, this process might take longer because we first have to non-rigidly register the atlas to the patient." 

    regsub -all "\n" $help {} help
    MainHelpApplyTags EMAtlasBrainClassifier $help
    MainHelpBuildGUI EMAtlasBrainClassifier

    #-------------------------------------------
    # Segementation frame
    #-------------------------------------------
    set fSeg $Module(EMAtlasBrainClassifier,fSegmentation)
    set f $fSeg
    
    foreach frame "Step1 Step2" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # 1. Step 
    #-------------------------------------------
    set f $fSeg.fStep1

    DevAddLabel $f.lTitle "1. Define Input Channels: "
    pack $f.lTitle -side top -padx $Gui(pad) -pady 1 -anchor w

    foreach frame "Left Right" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side left -padx 0 -pady $Gui(pad)
    }

    foreach Input "SPGR T2W" {
    DevAddLabel $f.fLeft.l$Input "  ${Input}:"
    pack $f.fLeft.l$Input -side top -padx $Gui(pad) -pady 1 -anchor w

    set menubutton   $f.fRight.m${Input}Select 
    set menu        $f.fRight.m${Input}Select.m
 
    eval {menubutton $menubutton -text [Volume($EMAtlasBrainClassifier(Volume,${Input}),node) GetName] -relief raised -bd 2 -width 9 -menu $menu} $Gui(WMBA)
    eval {menu $menu} $Gui(WMA)
    TooltipAdd $menubutton "Select Volume defining ${Input}" 
    set EMAtlasBrainClassifier(mbSeg-${Input}Select) $menubutton
    set EMAtlasBrainClassifier(mSeg-${Input}Select) $menu
        # Have to update at UpdateMRML too 
    DevUpdateNodeSelectButton Volume EMAtlasBrainClassifier Seg-${Input}Select Volume,$Input

    pack $menubutton -side top  -padx $Gui(pad) -pady 1 -anchor w
   }

    #-------------------------------------------
    # 2. Step 
    #-------------------------------------------
    set f $fSeg.fStep2

    DevAddLabel $f.lTitle "2. Save Results: "
    pack $f.lTitle -side top -padx $Gui(pad) -pady 0 -anchor w

    foreach frame "Left Right" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side left -padx 0 -pady $Gui(pad)
    }

    DevAddLabel $f.fLeft.lOutput "  Save Segmentation:" 
    pack $f.fLeft.lOutput -side top -padx $Gui(pad) -pady 2  -anchor w

    frame $f.fRight.fOutput -bg $Gui(activeWorkspace)
    TooltipAdd  $f.fRight.fOutput "Automatically save the segmentation results to the working directory" 

    pack $f.fRight.fOutput -side top -padx 0 -pady 2  -anchor w

    foreach value "1 0" text "On Off" width "4 4" {
    eval {radiobutton $f.fRight.fOutput.r$value -width $width -indicatoron 0\
          -text "$text" -value "$value" -variable EMAtlasBrainClassifier(Save,Segmentation) } $Gui(WCA)
    pack $f.fRight.fOutput.r$value -side left -padx 0 -pady 0 
    }

    # Now define working directory
    DevAddLabel $f.fLeft.lWorking "  Working Directory:" 
    pack $f.fLeft.lWorking -side top -padx $Gui(pad) -pady 2  -anchor w

    frame $f.fRight.fWorking -bg $Gui(activeWorkspace)
    TooltipAdd  $f.fRight.fWorking "Working directory in which any results of the segmentations should be saved in" 
    pack $f.fRight.fWorking -side top -padx 0 -pady 2 -anchor w

    eval {entry  $f.fRight.fWorking.eDir   -width 15 -textvariable EMAtlasBrainClassifier(WorkingDirectory) } $Gui(WEA)
    eval {button $f.fRight.fWorking.bSelect -text "..." -width 2 -command "EMAtlasBrainClassifierDefineWorkingDirectory"} $Gui(WBA)     
    pack $f.fRight.fWorking.eDir  $f.fRight.fWorking.bSelect -side left -padx 0 -pady 0  

    #-------------------------------------------
    # Run Algorithm
    #------------------------------------------
    eval {button $fSeg.bRun -text "Segment" -width 10 -command "EMAtlasBrainClassifierStartSegmentation"} $Gui(WBA)     
    pack $fSeg.bRun -side top -padx 2 -pady 2  

    #-------------------------------------------
    # Segementation frame
    #-------------------------------------------
    set fSeg $Module(EMAtlasBrainClassifier,fAdvanced)
    set f $fSeg

    foreach frame "Save Misc" {
      frame $f.f$frame -bg $Gui(activeWorkspace) -relief sunken -bd 2
      pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    DevAddLabel $f.fSave.lTitle "Save"  
    pack $f.fSave.lTitle -side top -padx $Gui(pad) -pady 2 
    foreach Att "SPGR T2W Atlas XMLFile"  Text "{Normalized SPGR} {Normalized T2W} {Aligned Atlas} {XML-File}" {
    eval {checkbutton  $f.fSave.c$Att -text "$Text" -variable EMAtlasBrainClassifier(Save,$Att) -indicatoron 1} $Gui(WCA)
    pack $f.fSave.c$Att  -side top -padx $Gui(pad) -pady 0 -anchor w 
    }

    DevAddLabel $f.fMisc.lTitle "Miscellaneous"  
    pack  $f.fMisc.lTitle -side top -padx $Gui(pad) -pady 2 

    
    foreach frame "Left Right" {
      frame $f.fMisc.f$frame -bg $Gui(activeWorkspace)
      pack $f.fMisc.f$frame -side left -padx 0 -pady $Gui(pad)
    }

    foreach Att "XMLTemplate AtlasDir" Text "{XML-Template File} {Atlas Directory}" Help "{XML Template file to be used for the segmentation} {Location of the atlases which define spatial distribtution}" {
    DevAddLabel $f.fMisc.fLeft.l$Att "${Text}:"  
    pack $f.fMisc.fLeft.l$Att -side top -padx 2 -pady 2  -anchor w 

    frame $f.fMisc.fRight.f$Att  -bg $Gui(activeWorkspace)
    pack $f.fMisc.fRight.f$Att -side top -padx 2 -pady 2  

    eval {entry  $f.fMisc.fRight.f$Att.eFile   -width 15 -textvariable EMAtlasBrainClassifier($Att) } $Gui(WEA)
    eval {button $f.fMisc.fRight.f$Att.bSelect -text "..." -width 2 -command "EMAtlasBrainClassifierDefine$Att"} $Gui(WBA)     
    pack $f.fMisc.fRight.f$Att.eFile  $f.fMisc.fRight.f$Att.bSelect -side left -padx 0 -pady 0 
    TooltipAdd  $f.fMisc.fRight.f$Att  "$Help" 
    }
}
#-------------------------------------------------------------------------------
# .PROC EMAtlasBrainClassifierBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMAtlasBrainClassifierBuildVTK {} {

}

#-------------------------------------------------------------------------------
# .PROC EMAtlasBrainClassifierEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc EMAtlasBrainClassifierEnter {} {
    global EMAtlasBrainClassifier

    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $EMAtlasBrainClassifier(eventManager)
    set WarningMsg ""
    if { [catch "package require vtkEMLocalSegment"] } {
    set WarningMsg "- vtkEMLocalSegment \n"
    }

    if {[catch "package require vtkAG"]} {
    set WarningMsg "${WarningMsg}- vtkAG: This module does not come with the standard distribution. \n  You can download it from ... . \n  You will need to modify INCLUDE_DIRECTORIES in vtkAG/cxx/CMakeListLocals.txt to correctly compile the module"
    }
    if {$WarningMsg != ""} {DevWarningWindow "Please install the following three modules before working with this module: \n$WarningMsg"}
}


#-------------------------------------------------------------------------------
# .PROC EMAtlasBrainClassifierExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMAtlasBrainClassifierExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

proc EMAtlasBrainClassifierUpdateMRML { } { 
    global EMAtlasBrainClassifier
    DevUpdateNodeSelectButton Volume EMAtlasBrainClassifier Seg-SPGRSelect Volume,SPGR
    DevUpdateNodeSelectButton Volume EMAtlasBrainClassifier Seg-T2WSelect  Volume,T2W
}

proc EMAtlasBrainClassifierDefineWorkingDirectory {} {
    global EMAtlasBrainClassifier
    set dir [tk_chooseDirectory -initialdir $EMAtlasBrainClassifier(WorkingDirectory)]
    if { $dir == "" } {
    return
    }
    set EMAtlasBrainClassifier(WorkingDirectory) "$dir"    
}

proc EMAtlasBrainClassifierDefineAtlasDir {} {
    global EMAtlasBrainClassifier
    set dir [tk_chooseDirectory -initialdir $EMAtlasBrainClassifier(AtlasDir) -title "Atlas Directory"]
    if { $dir == "" } {
    return
    }
    set EMAtlasBrainClassifier(AtlasDir) "$dir"    
}

proc EMAtlasBrainClassifierDefineXMLTemplate {} {
    global EMAtlasBrainClassifier
    set file [tk_getOpenFile -title "XML Template File" -filetypes {{XML {.xml} }} -defaultextension .xml -initialdir [file dirname $EMAtlasBrainClassifier(XMLTemplate)]]
    if { $file == "" } {
    return
    }
    set EMAtlasBrainClassifier(XMLTemplate) "$file"    
}

proc EMAtlasBrainClassifier_Normalize { VolIDInput VolIDOutput Mode } {
    global Volume Matrix EMAtlasBrainClassifier
    set Vol [Volume($VolIDInput,vol) GetOutput]
    puts [$Vol GetNumberOfScalarComponents] 
    vtkImageData hist
    vtkImageAccumulate ia
    ia SetInput $Vol
    ia SetComponentSpacing 1 1 1
    ia SetComponentOrigin 0 0 0
    ia SetComponentExtent 0 1000 0 0 0 0
    ia Update
    hist DeepCopy [ia GetOutput]
    set max [lindex [ia GetMax] 0]
    puts $max
    set count 0
    set i 0
    # Kilian change this - it is 99% of total amount of voxels
    set Extent [[Volume($VolIDInput,vol) GetOutput] GetExtent]
    set Boundary [expr ([lindex $Extent 1] - [lindex $Extent 0] +1) * ([lindex $Extent 3] - [lindex $Extent 2] +1) * ([lindex $Extent 5] - [lindex $Extent 4] +1) * 0.99]
    while {$i < $max && $count < $Boundary} {    
    set val [hist GetScalarComponentAsFloat $i 0 0 0]
    set count [expr $count + $val]
    incr i
    }
    set max $i 
    set min 0
    puts "Max: $max"
    puts "Min: $min"
    set width [expr $max / 5]
    puts "Width: $width"
    set fwidth [expr 1.0 / $width ]

    set sHistMax  [expr ($max - $min) - $width]
    for {set x 0} {$x <= $sHistMax } {incr x} { 
    set sHist($x) 0
    for {set k 0} {$k <= $width} {incr k} {
        set sHist($x) [expr [hist GetScalarComponentAsFloat [expr $x + $k] 0 0 0] + $sHist($x)]
    }
    set sHist($x) [expr $sHist($x) * $fwidth]
    }
    set x [expr $min + 1]
    set trough [expr $min - 1]
    set noise 1
    incr  sHistMax -2 
    while {$x < $sHistMax && $trough < $min} {
    if {$noise == 1 && $sHist($x) > $sHist([expr $x + 1]) && $x > $min} {
        set noise 0
    } elseif { $sHist($x) < $sHist([expr $x + 1]) && $sHist([expr $x + 1]) < $sHist([expr $x + 2]) && $sHist([expr $x +2]) < $sHist([expr $x + 3]) } {
        set trough $x
    }
    incr x
    }

    puts "Threshold: $trough"
    vtkImageAccumulate ia2
    ia2 SetInput $Vol
    ia2 SetComponentSpacing 1 1 1
    ia2 SetComponentOrigin $trough 0 0
    ia2 SetComponentExtent 0 [expr $max - $trough] 0 0 0 0
    ia2 Update
    hist DeepCopy [ia2 GetOutput]
    set i $trough
    set total 0
    set num 0
    while {$i < [expr $width * 5]} {    
    set val [hist GetScalarComponentAsFloat [expr $i - $trough] 0 0 0]
    set total [expr $total + ($i * $val)]
    set num [expr $num + $val]
    incr i
    }
    set M [expr $total * 1.0 / $num]

    puts "M: $M"

     vtkImageMathematics im
    im SetInput1 $Vol
    im SetConstantK [expr ($EMAtlasBrainClassifier(Normalize,$Mode) / $M)]
    puts "Mode: $Mode"
 
    im SetOperationToMultiplyByK
    Volume($VolIDOutput,vol) SetImageData [im GetOutput]
    ia Delete
    im Delete
    ia2 Delete
    hist Delete
    puts "Done"
} 
 

proc EMAtlasBrainClassifierVolumeWriter {VolID} {
    global Volume
    set prefix [MainFileGetRelativePrefix [Volume($VolID,node) GetFilePrefix]] 
    MainVolumesWrite $VolID $prefix 
    # RM unnecssary xml file 
    catch {file delete -force [file join [file dirname [Volume($VolID,node) GetFullPrefix]] [Volume($VolID,node) GetFilePrefix]].xml }
}

proc EMAtlasBrainClassifierReadXMLFile { FileName } {
    global EMAtlasBrainClassifier
    if {[catch {set fid [open $FileName r]} errmsg] == 1} {
    puts $errmsg
    return ""
    }

    set file [read $fid]

    if {[catch {close $fid} errorMessage]} {
    puts "Could not close file : ${errorMessage}"
        return ""
    }
    return $file 
}

proc EMAtlasBrainClassifierGrepLine {input search_string} {
    set foundIndex [string first $search_string  $input]
    if {$foundIndex < 0} {
        return "-1 -1"
    }

    set start [expr [string last "\n" [string range $input 0 [expr $foundIndex -1]]] +1]
    set last  [string first "\n" [string range  $input $start end]]

    if  {$last < 0} { set last  [expr [string length $input] -1] 
    } else { incr last $start}
    return "$start $last"
}

proc EMAtlasBrainClassifierReadNextKey {input} {
    if {([regexp "^(\[^=\]*)\[\n\t \]*=\[\n\t \]*\['\"\](\[^'\"\]*)\['\"\](.*)$" \
             $input match key value input] != 0) && ([string equal -length 1 $input "/"] == 0)} {
    return "$key $value "
    }
    return "" 
}

# In the future should probably be more independent but should work for right now 
proc EMAtlasBrainClassifierLoadAtlasVolume {AtlasDir AtlasName} {
    global Volume EMAtlasBrainClassifier
    set Volume(firstFile) "$EMAtlasBrainClassifier(AtlasDir)/$AtlasDir/I.001"
    set Volume(propertyType) "VolHeader"
    set Volume(lastNum)   124
    set Volume(readHeaders) 0
    set Volume(labelMap)    0
    set Volume(name) $AtlasName
    set Volume(desc)        ""
    set Volume(filePattern) "%s.%03d"
    set Volume(width)       256
    set Volume(height)      256
    set Volume(pixelWidth)  0.9375
    set Volume(pixelHeight) 0.9375
    set Volume(sliceThickness) 1.5
    set Volume(sliceSpacing) 0.0
    set Volume(scanOrder)   LR
    set Volume(scalarType)  Short
    set Volume(gantryDetectorTilt)  0
    set Volume(numScalars)   1
    set Volume(littleEndian) 0
    set Volume(tensors,DTIdata) 0
    set Volume(tensors,pfSwap) 0
    set Volume(activeID) NEW
    set VolID [VolumesPropsApply]
    if {$VolID == "" } {
    DevErrorWindow "Could not load Volume $AtlasName in directory $EMAtlasBrainClassifier(AtlasDir)/$AtlasDir/I.001"
    return ""
    } 
    RenderAll
    return $VolID 
}

proc EMAtlasBrainClassifierResetEMSegment { } {
    global EMSegment 
    eval {global} $EMSegment(MrmlNode,TypeList) 

    EMSegmentChangeClass 0
    set EMSegment(NumClassesNew) 0
    EMSegmentCreateDeleteClasses 1 1 0
    # now delete Nodes of Superclass 0
    if {$EMSegment(Cattrib,0,Node) != ""} {    
    MainMrmlDeleteNode SegmenterSuperClass [$EMSegment(Cattrib,0,Node) GetID]
    set EMSegment(Cattrib,0,Node) ""
    }

    foreach dir $EMSegment(CIMList) {
    if {$EMSegment(Cattrib,0,CIMMatrix,$dir,Node) != ""}  {
        MainMrmlDeleteNode SegmenterCIM [$EMSegment(Cattrib,0,CIMMatrix,$dir,Node) GetID]
        set EMSegment(Cattrib,0,CIMMatrix,$dir,Node) ""
    }
    } 

    if {$EMSegment(Cattrib,0,EndNode) != ""} {
    lappend  MrmlNodeDeleteList "EndSegmenterSuperClass [$EMSegment(Cattrib,0,EndNode) GetID]" 
    set EMSegment(Cattrib,0,EndNode) ""
    }

    # Delete All Remaining Segmenter Nodes that we might have forgotten 
    # Should only be node Segmenter and InputImages 
    set EMSegment(SegmenterNode) ""

    foreach node $EMSegment(MrmlNode,TypeList) {
    upvar #0 $node Array    
    foreach id $Array(idList) {
        # Add to the deleteList
        lappend Array(idListDelete) $id

        # Remove from the idList
        set i [lsearch $Array(idList) $id]
        set Array(idList) [lreplace $Array(idList) $i $i]

        # Remove node from tree, and delete it
        Mrml(dataTree) RemoveItem ${node}($id,node)
        ${node}($id,node) Delete
    }
    }
    MainUpdateMRML
    foreach node $EMSegment(MrmlNode,TypeList) {set ${node}(idListDelete) "" }
    MainMrmlUpdateIdLists "$EMSegment(MrmlNode,TypeList)"

}

proc EMAtlasBrainClassifierDeleteAllVolumeNodesButSPGRAndT2W { } {
   global  EMAtlasBrainClassifier Volume Mrml

   foreach id $Volume(idList) {
        if {($id != $Volume(idNone)) && ($id != $EMAtlasBrainClassifier(Volume,SPGR)) && ($id != $EMAtlasBrainClassifier(Volume,T2W)) } {
            # Add to the deleteList
            lappend Volume(idListDelete) $id

            # Remove from the idList
            set i [lsearch $Volume(idList) $id]
            set Volume(idList) [lreplace $Volume(idList) $i $i]

            # Remove node from tree, and delete it
            Mrml(dataTree) RemoveItem Volume($id,node)
            Volume($id,node) Delete
        }
    }
    MainUpdateMRML
}

proc EMAtlasBrainClassifierStartSegmentation { } {
    global EMAtlasBrainClassifier Volume EMSegment Mrml

    # ---------------------------------------------------------------
    # Initialize values 
    # Check if input is set
    if {($EMAtlasBrainClassifier(Volume,SPGR) == $Volume(idNone)) || ($EMAtlasBrainClassifier(Volume,T2W) == $Volume(idNone))} {
    DevErrorWindow "Please define both SPGR and T2W before starting the segmentation" 
    return 
    } 
    # Kilian Check the names of the input so that they are not the same as we expect
    if {([Volume($EMAtlasBrainClassifier(Volume,SPGR),node) GetName] == "NormedSPGR") || ([Volume($EMAtlasBrainClassifier(Volume,T2W),node) GetName] == "NormedT2W") } {
    DevErrorWindow "Please rename the SPGR and T2W Volume. They cannot be named NormedSPGR or NormedT2W" 
    return 
    }
    set Mrml(dir) $EMAtlasBrainClassifier(WorkingDirectory)/EMSegmentation

    EMAtlasBrainClassifierDeleteAllVolumeNodesButSPGRAndT2W
    EMAtlasBrainClassifierResetEMSegment 
   
    # ---------------------------------------------------------------
    # 1. Step: Normalize images
    foreach input "SPGR T2W" {
    # Create a New Volume
    puts "=========== Normalize $input ============ "
    set VolIDInput $EMAtlasBrainClassifier(Volume,${input})
    set VolIDOutput [DevCreateNewCopiedVolume $VolIDInput "" "Normed$input"]
    set Prefix "$EMAtlasBrainClassifier(WorkingDirectory)/[string tolower $input]/[file tail [Volume($EMAtlasBrainClassifier(Volume,${input}),node) GetFilePrefix]]norm"
    Volume($VolIDOutput,node) SetFilePrefix "$Prefix"
    Volume($VolIDOutput,node) SetFullPrefix "$Prefix" 
    Volume($VolIDOutput,node) SetFilePattern "%s.%03d"
    set EMAtlasBrainClassifier(Volume,Normalized${input}) $VolIDOutput

    # Normalize intentensities
    EMAtlasBrainClassifier_Normalize  $VolIDInput $VolIDOutput $input 

    # Clean Up 
    MainUpdateMRML
    RenderAll
    if {$EMAtlasBrainClassifier(Save,$input)} {
        EMAtlasBrainClassifierVolumeWriter $VolIDOutput  
    }
    }
    # ---------------------------------------------------------------
    # 2. Step: Align atlas - wait for script from Sylvain
    # Read Template File to figure out which atlases to use for registration 
    set RegisterAtlasDirList "" 
    set RegisterAtlasNameList "" 
    
    # a. Define which atlases to register 
    set XMLTemplateText [EMAtlasBrainClassifierReadXMLFile $EMAtlasBrainClassifier(XMLTemplate)]
    if {$XMLTemplateText == "" } {
    DevErrorWindow "Could not read template file $EMAtlasBrainClassifier(XMLTemplate) or it was empty!" 
    return
    }

    set NextLineIndex [EMAtlasBrainClassifierGrepLine "$XMLTemplateText" "<SegmenterClass"] 
    set NumCases 0
    while {$NextLineIndex != "-1 -1"} {
    set Line [string range "$XMLTemplateText" [lindex $NextLineIndex 0] [lindex $NextLineIndex 1]]
    set PriorPrefixIndex [string first "LocalPriorPrefix"  "$Line"]
    set PriorNameIndex   [string first "LocalPriorName"  "$Line"]

    if {($PriorPrefixIndex > -1) && ($PriorNameIndex > -1)} {
        set ResultPrefix [lindex [EMAtlasBrainClassifierReadNextKey  "[string range \"$Line\" $PriorPrefixIndex end]"] 1]
        set AtlasDir [file tail [file dirname $ResultPrefix]]
        set AtlasName   [lindex [EMAtlasBrainClassifierReadNextKey  "[string range \"$Line\" $PriorNameIndex end]"] 1]

        if {($ResultPrefix != "") && ($AtlasName != "") && ([lsearch $RegisterAtlasNameList "$AtlasName"] < 0) && ($AtlasDir != "") } {
        lappend  RegisterAtlasDirList "$AtlasDir"
        lappend  RegisterAtlasNameList "$AtlasName"
        }

    }
    set XMLTemplateText  [string range "$XMLTemplateText" [expr [lindex $NextLineIndex 1] +1] end]
    set NextLineIndex [EMAtlasBrainClassifierGrepLine "$XMLTemplateText" "<SegmenterClass"] 
    } 
  
    # b. Register Atlases 
    if {$RegisterAtlasDirList != "" } {

    set TemplateIDInput $EMAtlasBrainClassifier(Volume,NormalizedSPGR)
    puts "=========== Register atlas spgr with patient ============ "
    # Load SPGR 
    set VolIDInput      [EMAtlasBrainClassifierLoadAtlasVolume spgr  AtlasSPGR]
    if {$VolIDInput == "" } {return}

    # Define Registration output volume 
    set VolIDOutput [DevCreateNewCopiedVolume $TemplateIDInput "" "RegisteredSPGR"]

    # Sylvain register spgr of atlas to patient
    # Input Volume ID:  VolIDInput ,  EMAtlasBrainClassifier(Volume,NormalizedSPGR) or EMAtlasBrainClassifier(Volume,SPGR) 
    # Output Volume ID: VolIDOutput
  
        puts  "Dummy function so we see it works for right now "
    EMAtlasBrainClassifier_Normalize  $VolIDInput $VolIDOutput SPGR 

    # Clean up 
    if {$EMAtlasBrainClassifier(Save,Atlas)} {
        set Prefix "$EMAtlasBrainClassifier(WorkingDirectory)/atlas/spgr/I"
        Volume($VolIDOutput,node) SetFilePrefix "$Prefix"
        Volume($VolIDOutput,node) SetFullPrefix "$Prefix" 
        EMAtlasBrainClassifierVolumeWriter $VolIDOutput
    }
    MainMrmlDeleteNode Volume $VolIDInput 
    MainMrmlDeleteNode Volume $VolIDOutput 
    MainUpdateMRML
    RenderAll

    # b.2 Resmaple atlas 
    foreach Dir "$RegisterAtlasDirList" Name "$RegisterAtlasNameList" {
        puts "=========== Resample Atlas $Name  ============ "
        # Load In the New Atlases
        set VolIDInput [EMAtlasBrainClassifierLoadAtlasVolume $Dir Atlas$Name]

        # Define Registration output volumes
        set VolIDOutput [DevCreateNewCopiedVolume $TemplateIDInput "" "$Name"]
        set Prefix "$EMAtlasBrainClassifier(WorkingDirectory)/atlas/$Dir/I"
        Volume($VolIDOutput,node) SetFilePrefix "$Prefix"
        Volume($VolIDOutput,node) SetFullPrefix "$Prefix" 

        # Sylvain resample atlas
        # Input Volume ID:  VolIDInput 
        # Output Volume ID: VolIDOutput
            puts  "Dummy function so we see it works for right now "
        EMAtlasBrainClassifier_Normalize  $VolIDInput $VolIDOutput SPGR 
        # Clean up 
        if {$EMAtlasBrainClassifier(Save,Atlas)} {EMAtlasBrainClassifierVolumeWriter $VolIDOutput}
        MainMrmlDeleteNode Volume $VolIDInput 
        MainUpdateMRML
        RenderAll
    }
    }

    # ---------------------------------------------------------------------- 
    # 3. Segment Image `
    # Read XML File  
    catch {exec mkdir $EMAtlasBrainClassifier(WorkingDirectory)/EMSegmentation} 
    set tags [MainMrmlReadVersion2.x $EMAtlasBrainClassifier(XMLTemplate)]
    set tags [MainMrmlAddColors $tags]
    MainMrmlBuildTreesVersion2.0 $tags
    MainUpdateMRML
    # Set Segmentation Boundary  so that if you have images of other dimension it will segment them correctly
    set VolID $EMAtlasBrainClassifier(Volume,SPGR)
    set EMSegment(SegmentationBoundaryMax,0) [lindex [Volume($VolID,node) GetDimensions] 0]
    set EMSegment(SegmentationBoundaryMax,1) [lindex [Volume($VolID,node) GetDimensions] 1]
    set Range [Volume($VolID,node) GetImageRange]
    set EMSegment(SegmentationBoundaryMax,2) [expr [lindex $Range 1] - [lindex $Range 0] + 1] 
    puts "=========== Segment Image ============ "
    EMSegmentStartEM

    if {$EMSegment(LatestLabelMap) == $Volume(idNone)} { 
    DevErrorWindow "Error: Could not segment subject"
    } else {
    set Prefix "$EMAtlasBrainClassifier(WorkingDirectory)/EMSegmentation/EMResult"
    set VolIDOutput $EMSegment(LatestLabelMap)
    Volume($VolIDOutput,node) SetFilePrefix "$Prefix"
    Volume($VolIDOutput,node) SetFullPrefix "$Prefix" 
    EMAtlasBrainClassifierVolumeWriter $VolIDOutput
    }
    # Change xml directory
    if {$EMAtlasBrainClassifier(Save,XMLFile)}      {MainMrmlWrite  $EMAtlasBrainClassifier(WorkingDirectory)/EMSegmentation/segmentation.xml}
    puts "=========== Finshed  ============ "
}

