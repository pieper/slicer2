#=auto==========================================================================
# (c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        IbrowserLoadGUI.tcl
# PROCEDURES:  
#   IbrowserBuildLoadFrame
#   IbrowserRaiseLoadVolumes
#   IbrowserRaiseLoadModels
#   IbrowserRaiseLoadModels
#   IbrowserBuildBasicVolumeReaderGUI
#   IbrowserBuildHeaderVolumeReaderGUI
#   IbrowserBuildBvolumesVolumeReaderGUI
#   IbrowserBuildAnalyzeVolumeReaderGUI
#   IbrowserBuildDICOMVolumeReaderGUI
#   IbrowserRaiseVolumeReaderInfoFrame
#   IbrowserRaiseModelReaderInfoFrame
#   IbrowserLoadAnalyzeVolumes
#==========================================================================auto=



#-------------------------------------------------------------------------------
# .PROC IbrowserBuildLoadFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildLoadFrame { } {
    global Gui Ibrowser Module Volume
    
    #-------------------------------------------
    #--- Load frame: VolumeOrModel
    #--- lets user switch between loading
    #--- volumes and models. Depending
    #--- on which of these is selected, the
    #--- correct ReaderMaster frame is
    #--- raised. Depending on which reader
    #--- is chosen, the correct SetReadInfo
    #--- frame is raised. Reader gui's are
    #--- copies of existing Volumes module
    #--- readers, extended to support
    #--- the loading of multi-volume data.
    #-------------------------------------------

    set fNew $::Module(Ibrowser,fNew)
    #---------------------------------------------------------------
    #--- fNew (packer)
    #---------------------------------------------------------------
    set f $fNew

    frame $f.fVolumeOrModel  -relief groove -bg $Gui(backdrop)  -bd 3
    frame $f.fReaderMaster    -relief groove -bg $Gui(activeWorkspace) -bd 3
    frame $f.fSetReadInfo      -relief groove -bg $Gui(activeWorkspace) -bd 3
    frame $f.fLogos -bg $Gui(activeWorkspace) -bd 3
    pack $f.fVolumeOrModel \
        $f.fReaderMaster \
        $f.fSetReadInfo \
        $f.fLogos \
        -side top -padx 1 -pady 1 -fill x -expand 1

    #--- Catalog of all reader types Ibrowser can do:
    #--- Add menu text for all new readers here too!
    set ::Ibrowser(readerText,Basic) "Basic"
    set ::Ibrowser(readerText,VTK) "(.vtk)"
    set ::Ibrowser(readerText,Header) "Header"
    set ::Ibrowser(readerText,Bvolumes) "Bvolumes"
    set ::Ibrowser(readerText,Analyze) "Analyze"
    set ::Ibrowser(readerText,DICOM) "DICOM"
    
    #---------------------------------------------------------------
    #--- fNew->fVolumeOrModel (packer)
    #---------------------------------------------------------------
    #--- Make the top frame for choosing
    #--- to load models or volumes
    set f $fNew.fVolumeOrModel
    eval {label $f.lWhatToLoad -text "What to load:" -width 14 -justify right } $Gui(BLA)
    DevAddButton $f.bNewVolumes "Volumes" "IbrowserRaiseLoadVolumes"
    DevAddButton $f.bNewModels "Models" "IbrowserRaiseLoadModels"
    pack $f.lWhatToLoad $f.bNewVolumes $f.bNewModels \
        -side left -padx 2 -pady 10 -expand 1

    #---------------------------------------------------------------
    #--- fNew->fSetReadInfo (placer + packer?)
    #---------------------------------------------------------------
    #--- Make frames for each kind of
    #--- volume and model reader, which
    #--- will be raised when the corresponding
    #--- reader is chosen in .fChooseReader frame.
    #--- and raise the default option (basic).
    #---------------------------------------------------------------
    #--- fNew->fSetReadInfo->f(ReaderFrames) (packer)
    #--- Using place and pack to manage fSetReadInfo is not
    #--- correct, but without packing Spacebutt,
    #--- the placed frames do not get drawn in master
    #---------------------------------------------------------------
    set f $fNew.fSetReadInfo
    DevAddButton $f.bSpacebutt "" ""
    pack $f.bSpacebutt -side top -padx 100 -pady 110 -fill x -anchor w   
    IbrowserBuildBasicVolumeReaderGUI $f
    IbrowserBuildHeaderVolumeReaderGUI $f
    IbrowserBuildBvolumesVolumeReaderGUI $f
    IbrowserBuildAnalyzeVolumeReaderGUI $f
    IbrowserBuildDICOMVolumeReaderGUI $f
    IbrowserBuildVTKModelReaderGUI $f
    raise $::Ibrowser(fAnalyzeReader)

    #---------------------------------------------------------------
    #--- fNew->fReaderMaster (placer + packer? uhoh!)
    #--- Using place and pack to manage fSetReadInfo is not
    #--- correct, but without packing Spacebutt,
    #--- the placed frames do not get drawn in master
    #--- SO, this is bad, and may break elsewhere.
    #---------------------------------------------------------------
    #--- Make a frame for reading models,
    #--- and one for reading volumes.
    set f $fNew.fReaderMaster
    DevAddButton $f.bSpacebutt "" ""
    pack $f.bSpacebutt -side top -padx 1 -pady 1 -fill x -anchor w
    frame $f.fNewVolumes -bg $Gui(activeWorkspace)
    place $f.fNewVolumes -in $f -relheight 1.0 -relwidth 1.0
    set ::Ibrowser(fNewVolumes) $f.fNewVolumes
    frame $f.fNewModels -bg $Gui(activeWorkspace)
    place $f.fNewModels -in $f -relheight 1.0 -relwidth 1.0
    set ::Ibrowser(fNewModels) $f.fNewModels

    #---------------------------------------------------------------
    #--- fNew->fReaderMaster->fNewVolumes (packer)
    #---------------------------------------------------------------
    #--- Parameters for ReaderMaster.fNewVolumes
    set f $::Ibrowser(fNewVolumes)
    DevAddLabel $f.lVolumeReaders "Volume Readers: "
    pack $f.lVolumeReaders -side left -padx $Gui(pad) -fill x -anchor w
    #--- Must build a menu button with a pull-down
    #--- menu of volume readers.
    eval {menubutton $f.mbVolType -text \
              $::Ibrowser(readerText,Analyze) \
               -relief raised -bd 2 -width 20 \
               -menu $f.mbVolType.m} $Gui(WMBA)
    #--- save menubutton for configuring text later
    set ::Ibrowser(guiVolumeReaderMenuButton) $f.mbVolType
    eval {menu $f.mbVolType.m} $Gui(WMA)
    pack $f.mbVolType -side left -pady 1 -padx $Gui(pad)
    #--- Add menu items and commands,
    #--- one for each reader type.
    foreach r "Basic Header Bvolumes Analyze DICOM" {
        $f.mbVolType.m add command -label $r \
            -command "IbrowserRaiseVolumeReaderInfoFrame $::Ibrowser(readerText,${r}) $::Ibrowser(f${r}Reader)"
    }
    #--- Raise this frame as default option.
    raise $::Ibrowser(fNewVolumes)

    #---------------------------------------------------------------
    #--- fNew->fReaderMaster->fNewModels (packer)
    #---------------------------------------------------------------
    #--- Parameters for ReaderMaster.fNewModels
    set f $::Ibrowser(fNewModels)
    DevAddLabel $f.lModelFormats "Model Formats: "
    pack $f.lModelFormats -side left -padx $Gui(pad) -fill x -anchor w
    #--- Must build a menu button with a pulldown
    #--- menu for model reader type
     eval {menubutton $f.mbModType -text "(.vtk)" \
               -relief raised -bd 2 -width 20 \
               -menu $f.mbModType.m} $Gui(WMBA)
    #--- save menubutton for configuring text later
    set ::Ibrowser(guiModelReaderMenuButton) $f.mbModType
    eval {menu $f.mbModType.m} $Gui(WMA)
    pack $f.mbModType -side left -pady 1 -padx $Gui(pad)
    #--- Add menu items and commands
     $f.mbModType.m add command -label "(.vtk)" \
         -command "IbrowserRaiseModelReaderInfoFrame (.vtk) $::Ibrowser(fVTKModelReader)"

    #---------------------------------------------------------------
    #--- fNew->fLogos (packer)
    #---------------------------------------------------------------
    #--- Make the top frame for displaying
    #--- logos from BIRN, SPL and HCNR
    set f $fNew.fLogos

    set uselogo [image create photo -file $::Ibrowser(modulePath)/logos/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
              -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 2 -pady 5 -expand 0


}




#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseLoadVolumes
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseLoadVolumes { } {

    raise $::Ibrowser(fNewVolumes)
    raise $::Ibrowser(fBasicReader)
}




#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseLoadModels
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseLoadModels { } {

    raise $::Ibrowser(fNewModels)
    raise $::Ibrowser(fVTKModelReader)
}




#-------------------------------------------------------------------------------
# .PROC 
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc  IbrowserBuildVTKModelReaderGUI { myMaster } {
    global Gui

    #--- build a custom version of vtk model reader
    #--- which reads a sequence of models
    #--- and place it within the parentFrame
    set f $myMaster
    frame $f.fVTKModelReader -bg $Gui(activeWorkspace)
    place $f.fVTKModelReader -in $f -relheight 1.0 -relwidth 1.0
    
    set ::Ibrowser(fVTKModelReader) $f.fVTKModelReader
    set f $::Ibrowser(fVTKModelReader)
    DevAddLabel $f.lSpacelabel "Configure the reader: "
    pack $f.lSpacelabel -side top -padx 1 -pady 1 -fill x -anchor w
    DevAddButton $f.bSpacebutt "model load not yet available." ""
    pack $f.bSpacebutt -padx 1 -pady 1 -fill x -anchor w
}




#-------------------------------------------------------------------------------
# .PROC IbrowserBuildBasicVolumeReaderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildBasicVolumeReaderGUI { myMaster } {
    global Gui
    
    #--- build a custom version of basic reader
    #--- which reads a sequence of models
    #--- and place it within the parentFrame
    set f $myMaster
    frame $f.fBasicReader -bg $Gui(activeWorkspace)
    place $f.fBasicReader -in $f -relheight 1.0 -relwidth 1.0
    
    set ::Ibrowser(fBasicReader) $f.fBasicReader
    set f $::Ibrowser(fBasicReader)
    DevAddLabel $f.lSpacelabel "Please use Analyze Reader for now: "
    pack $f.lSpacelabel -side top -padx 1 -pady 1 -fill x -anchor w
    #DevAddButton $f.bSpacebutt "basic not yet available." ""
    #pack $f.bSpacebutt -padx 1 -pady 1 -fill x -anchor w
}




#-------------------------------------------------------------------------------
# .PROC IbrowserBuildHeaderVolumeReaderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildHeaderVolumeReaderGUI { myMaster } {
    global Gui
    
    #--- build a custom version of header reader
    #--- which reads a sequence of models
    #--- and place it within the parentFrame
    set f $myMaster
    frame $f.fHeaderReader -bg $Gui(activeWorkspace)
    place $f.fHeaderReader -in $f -relheight 1.0 -relwidth 1.0
    
    set ::Ibrowser(fHeaderReader) $f.fHeaderReader
    set f $::Ibrowser(fHeaderReader)
    DevAddLabel $f.lSpacelabel "Please use Analyze Reader for now: "
    pack $f.lSpacelabel -side top -padx 1 -pady 1 -fill x -anchor w
    #DevAddButton $f.bSpacebutt "header not yet available." ""
    #pack $f.bSpacebutt -padx 1 -pady 1 -fill x -anchor w
}




#-------------------------------------------------------------------------------
# .PROC IbrowserBuildBvolumesVolumeReaderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildBvolumesVolumeReaderGUI { myMaster } {
    global Gui
    
    #--- build a custom version of Bvolumes reader
    #--- which reads a sequence of models
    #--- and place it within the parentFrame
    set f $myMaster
    frame $f.fBvolumesReader -bg $Gui(activeWorkspace)
    place $f.fBvolumesReader -in $f -relheight 1.0 -relwidth 1.0
    
    set ::Ibrowser(fBvolumesReader) $f.fBvolumesReader
    set f $::Ibrowser(fBvolumesReader)
    DevAddLabel $f.lSpacelabel "Please use Analyze Reader for now: "
    pack $f.lSpacelabel -side top -padx 1 -pady 1 -fill x -anchor w
    #DevAddButton $f.bSpacebutt "bdata not yet available." ""
    #pack $f.bSpacebutt -padx 1 -pady 1 -fill x -anchor w
}





#-------------------------------------------------------------------------------
# .PROC IbrowserBuildAnalyzeVolumeReaderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildAnalyzeVolumeReaderGUI { myMaster } {
    global Gui Volume Module Model
    
    #--- build a custom version of analyze reader
    #--- which reads a sequence of models
    #--- and place its frame within the parentFrame
    set f $myMaster
    frame $f.fAnalyzeReader -bg $Gui(activeWorkspace)
    set ::Ibrowser(fAnalyzeReader) $f.fAnalyzeReader
    place $::Ibrowser(fAnalyzeReader) -in $myMaster -relheight 1.0 -relwidth 1.0

    set f $::Ibrowser(fAnalyzeReader)
    DevAddLabel $f.lSpacelabel "Configure the reader: "
    frame $f.fVolume -bg $Gui(activeWorkspace) -relief groove -bd 3
     frame $f.fFileType -bg $Gui(activeWorkspace) 
    frame $f.fApply -bg $Gui(activeWorkspace)
    pack $f.lSpacelabel $f.fVolume $f.fFileType $f.fApply \
        -side top -fill x -pady $Gui(pad)
    #---------------------------------------------------------
    #--- myMaster->fAnalyzeReader->fVolume frame
    #---------------------------------------------------------
    set f $myMaster.fAnalyzeReader.fVolume
    DevAddFileBrowse $f Volume "VolAnalyze,FileName" \
        "Analyze Volume:" "VolAnalyzeSetFileName" \
        "hdr" "\$Volume(DefaultDir)"  "Open" \
        "Browse for an Analyze header file (.hdr that has matching .img)" 

    frame $f.fLabelMap  -bg $Gui(activeWorkspace)
    frame $f.fDesc        -bg $Gui(activeWorkspace)
    frame $f.fName       -bg $Gui(activeWorkspace)
    frame $f.fFirstVolNum   -bg $Gui(activeWorkspace)
    frame $f.fLastVolNum   -bg $Gui(activeWorkspace)
    pack $f.fFirstVolNum -side top -padx $Gui(pad) -pady 1 -fill x
    pack $f.fLastVolNum -side top -padx $Gui(pad) -pady 1 -fill x
    pack $f.fLabelMap -side top -padx $Gui(pad) -pady 1 -fill x
    pack $f.fDesc -side top -padx $Gui(pad) -pady 1 -fill x
    pack $f.fName -side top -padx $Gui(pad) -pady 1 -fill x

    #--- fName row
    set f $myMaster.fAnalyzeReader.fVolume.fName
    eval {label $f.lName -text "IntervalName:"} $Gui(WLA)
    set ::Ibrowser(loadVol,name) "imageData"
    eval {entry $f.eName -textvariable ::Ibrowser(loadVol,name) -width 13} $Gui(WEA)
    pack  $f.lName -side left -padx $Gui(pad) 
    pack $f.eName -side left -padx $Gui(pad) -expand 1 -fill x

    #--- fDesc row
    set f $myMaster.fAnalyzeReader.fVolume.fDesc
    eval {label $f.lDesc -text "Optional Description:"} $Gui(WLA)
    set ::Ibrowser(loadVol,desc) "none"
    eval {entry $f.eDesc -textvariable ::Ibrowser(loadVol,desc)} $Gui(WEA)
    pack $f.lDesc -side left -padx $Gui(pad)
    pack $f.eDesc -side left -padx $Gui(pad) -expand 1 -fill x

    #--- fLabelMap
    set f $myMaster.fAnalyzeReader.fVolume.fLabelMap
    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -pady 5
    DevAddLabel $f.fTitle.l "Image Data:"
    pack $f.fTitle.l -side left -padx $Gui(pad) -pady 0

    foreach text "{Grayscale} {Label Map}" \
        value "0 1" \
        width "9 9 " {
        eval {radiobutton $f.fBtns.rMode$value -width $width \
            -text "$text" -value "$value" -variable Volume(labelMap) \
            -indicatoron 0 } $Gui(WCA)
        pack $f.fBtns.rMode$value -side left -padx 0 -pady 0
    }

    #---------------------------------------------------------
    #--- myMaster->fAnalyzeReader->fFileType
    #---------------------------------------------------------
    set f $myMaster.fAnalyzeReader.fFileType
    DevAddLabel $f.l "File Type: "
    pack $f.l -side left -padx $Gui(pad) -pady 0
    foreach type $Volume(VolAnalyze,FileTypeList) tip $Volume(VolAnalyze,FileTypeList,tooltips) {
        eval {radiobutton $f.rMode$type \
                  -text "$type" -value "$type" \
                  -variable Volume(VolAnalyze,FileType)\
                  -indicatoron 0} $Gui(WCA) 
        pack $f.rMode$type -side left -padx $Gui(pad) -pady 0
        TooltipAdd  $f.rMode$type $tip
    }
    
    #---------------------------------------------------------
    #--- myMaster->fAnalyzeReader->fApply
    #---------------------------------------------------------
    set f $myMaster.fAnalyzeReader.fApply

    #--- initialize load volume...
    set ::Volume(VolAnalyze,FileName) ""
    DevAddButton $f.bApply "Load" "IbrowserLoadAnalyzeVolumes" 8
    DevAddButton $f.bCancel "Cancel" "VolumesPropsCancel" 8
    grid $f.bApply $f.bCancel -padx $Gui(pad)
}




#-------------------------------------------------------------------------------
# .PROC IbrowserBuildDICOMVolumeReaderGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserBuildDICOMVolumeReaderGUI { myMaster } {
    global Gui
    
    #--- build a custom version of dicom reader
    #--- which reads a sequence of models
    #--- and place it within the parentFrame
    set f $myMaster
    frame $f.fDICOMReader -bg $Gui(activeWorkspace)
    place $f.fDICOMReader -in $f -relheight 1.0 -relwidth 1.0
    
    set ::Ibrowser(fDICOMReader) $f.fDICOMReader
    set f $::Ibrowser(fDICOMReader)
    DevAddLabel $f.lSpacelabel "Please use Analyze Reader for now: "
    pack $f.lSpacelabel -side top -padx 1 -pady 1 -fill x -anchor w
    #DevAddButton $f.bSpacebutt "DICOM not yet available." ""
    #pack $f.bSpacebutt -padx 1 -pady 1 -fill x -anchor w
}




#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseVolumeReaderInfoFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseVolumeReaderInfoFrame { menuText readerFrame } {

    $::Ibrowser(guiVolumeReaderMenuButton) config -text $menuText
    raise $readerFrame
}




#-------------------------------------------------------------------------------
# .PROC IbrowserRaiseModelReaderInfoFrame
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserRaiseModelReaderInfoFrame { menuText readerFrame } {

    $::Ibrowser(guiModelReaderMenuButton) config -text $menuText
    raise $readerFrame
}




#-------------------------------------------------------------------------------
# .PROC IbrowserLoadAnalyzeVolumes
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc IbrowserLoadAnalyzeVolumes { } {
    global Volume Mrml 
    
    if {$::Ibrowser(loadVol,name) != "imageData"} {
        if { $::Ibrowser(loadVol,name) == "" } {
            DevErrorWindow "Please select the first volume to load."
            VolumesPropsCancel
            return;            
        }
        set namecheck [IbrowserUniqueNameCheck $::Ibrowser(loadVol,name) ]
        if { $namecheck == 0 } {
            DevErrorWindow "$::Ibrowser(loadVol,name) is already being used by Slicer. Try another name."
            VolumesPropsCancel
            return;
        } 
    } else {
        set ::Ibrowser(loadVol,name) [format "%s-%d" $::Ibrowser(loadVol,name) $::Ibrowser(uniqueNum)]
    }
    set ::Ibrowser(seqName) $::Ibrowser(loadVol,name)
    set iname $::Ibrowser(seqName)

    set filename $Volume(VolAnalyze,FileName)
    set lastslash [string last "/" $filename ]
    set path [ string range $filename 0 $lastslash ]
    set everyfile [file join $path "*.hdr"]
    set files [lsort -dictionary [glob $everyfile]]
    
    set index 1

    #--- how many files:
    set numfiles 0
    foreach f $files {
        incr numfiles
    }
    IbrowserRaiseProgressBar
    
    #--- report in Ibrowser's message panel"
    set tt "Loading $iname..."
    IbrowserSayThis $tt 0

    #--- inum starts at zero; so the first interval we create
    #--- is interval number zero.
    #set id [expr $::IbrowserController(Info,Ival,ivalCount) ]
    set id $::Ibrowser(uniqueNum)
    foreach f $files { 
        set i [expr $index - 1]
        #--- report loading progress
        set progress [ expr double ( $i ) / double ( $numfiles ) ]
        IbrowserUpdateProgressBar $progress "::"
        
        set ::Ibrowser(loadVol,name) [format "%s_%d" $iname $i]    
        MainVolumesSetActive "NEW"
        set Volume(VolAnalyze,FileName) $f
        set ::Ibrowser($id,$index,f) $f

        #--- strip off the filename
        set l [ expr $lastslash+1 ]
        set lastdot [ string last "." $f ]
        set d [ expr $lastdot-1 ]
        set e [ expr $lastdot+1 ]
        set filelen [ string length $f ]
        set basename [ string range $f $l $d ]
        set str {}
        set extension [ string range $f $e $filelen ]
        set name {}

        set Volume(name) $::Ibrowser(loadVol,name)
        set i [VolAnalyzeApply]
        set m [expr $index -1 ]
        #--- let Ibrowser have reference to the volume's MRMLid
        set ::Ibrowser($id,$m,MRMLid) $i

        #--- set first MRMLid for this interval
        #--- This, and the last MRMLid for the interval
        #--- allow us to keep track of which volumes
        #--- are in each interval for now; until MRML
        #--- can officially accommodate volume-series.
        if { $m == 0 } {
            set ::Ibrowser($id,firstMRMLid) $i
        }
        incr index
    }
    IbrowserLowerProgressBar

    #--- set last MRMLid for this interval
    set ::Ibrowser($id,lastMRMLid) $i
    
    #--- populate intervalBrowser controller.
    IbrowserMakeNewInterval $iname $::IbrowserController(Info,Ival,imageIvalType) 0.0 $m

    #--- For now, fill a position array with the
    #--- time point that each drop represents.
    #--- Assume the interval is m units long.
    #--- how many files do we have?
    set top [ expr $m+1 ]
    set ::Ibrowser($id,numDrops) $top
    for {set zz 0} {$zz < $top} { incr zz} {
        set posVec($zz) $zz
    }
    IbrowserCreateImageDrops  $iname posVec $::Ibrowser($id,numDrops)

    #--- reconfigure the indexSlider in DisplayGUI
    IbrowserUpdateMaxDrops
    $::Ibrowser(indexSlider) configure -state active

    #--- report in Ibrowser's message panel"
    set tt "Finished loading $top files."
    IbrowserSayThis $tt 0

    #--- make this the active interval.
    #--- display the first volume
    #--- make it the active volume
    #--- and put it in the background
    #--- as is the loading convention.
    IbrowserDeselectActiveInterval $::IbrowserController(Icanvas)
    IbrowserDeselectBGIcon $::IbrowserController(Icanvas)

    set ::Ibrowser(activeInterval) $id
    set ::Ibrowser(BGInterval) $id

    IbrowserSelectActiveInterval $id $::IbrowserController(Icanvas)
    IbrowserSelectBGIcon $id $::IbrowserController(Icanvas)

    MainSlicesSetVolumeAll Back $::Ibrowser($id,0,MRMLid) 
    MainVolumesSetActive $::Ibrowser($id,0,MRMLid)
    MainSlicesSetVisibilityAll 1
    RenderAll
    set ::Ibrowser(loadVol,name) "imageData"
    set ::Volume(VolAnalyze,FileName) ""
}
