#-------------------------------------------------------------------------------
proc IbrowserBuildLoadFrame { } {
    global Gui Module Volume
    

    set fNew $::Module(Ibrowser,fNew)
    set f $fNew
    #---------------------------------------------------------------
    #--- fNew (packer)
    #---------------------------------------------------------------
    frame $f.fOption -bg $Gui(activeWorkspace) 
    frame $f.fLogos -bg $Gui(activeWorkspace) -bd 3
    grid $f.fOption -row 0 -column 0 -sticky ew
    grid $f.fLogos -row 1 -column 0 -sticky ew

    #------------------------------
    # fNew->Option frame
    #------------------------------
    set f $fNew.fOption

    Notebook:create $f.fNotebook \
                    -pages {Load Select} \
                    -pad 2 \
                    -bg $Gui(activeWorkspace) \
                    -height 300 \
                    -width 240
    pack $f.fNotebook -fill both -expand 1
    #--- Load or construct a sequence from disk
    set w [ Notebook:frame $f.fNotebook Load ]
    IbrowserBuildUIForLoad $w
    #--- Select or construct a sequence from elsewhere in Slicer
    set w [ Notebook:frame $f.fNotebook Select ]
    IbrowserBuildUIForSelect $w

    #---------------------------------------------------------------
    #--- fNew->fLogos (packer)
    #---------------------------------------------------------------
    #--- Make the top frame for displaying
    #--- logos from BIRN, SPL and HCNR
    set f $fNew.fLogos

    set uselogo [image create photo -file $::Ibrowser(modulePath)/logos/LogosForIbrowser.gif]
    eval {label $f.lLogoImages -width 200 -height 45 \
              -image $uselogo -justify center} $Gui(BLA)
    pack $f.lLogoImages -side bottom -padx 2 -pady 1 -expand 0


}



proc IbrowserBuildUIForSelect { parent } {
       global Gui

    frame $parent.fTop    -bg $Gui(activeWorkspace) 
    frame $parent.fMiddle -bg $Gui(activeWorkspace) 
    frame $parent.fBottom -bg $Gui(activeWorkspace) 
    pack $parent.fTop $parent.fMiddle -side top -padx $Gui(pad) 
    pack $parent.fBottom -side top -pady 15 
    
    #--- Frame for Sequence listbox
    set f $parent.fTop
    DevAddLabel $f.l "Available sequences:"
    listbox $f.lb -height 3 -bg $Gui(activeWorkspace) 
    set ::Ibrowser(seqsListBox) $f.lb
    pack $f.l $f.lb -side top -pady $Gui(pad)   

    #--- Frame for Sequence listbox buttons
    set f $parent.fMiddle
    DevAddButton $f.bSelect "Select" "IbrowserSelectSequence" 10 
    DevAddButton $f.bUpdate "Update" "IbrowserUpdateSequences" 10 
    pack $f.bUpdate $f.bSelect -side left -expand 1 -pady $Gui(pad) -padx $Gui(pad) -fill both

    #--- Frame for previewing new sequence
    set f $parent.fBottom

    DevAddLabel $f.lVolNo "Volume:"
    eval { scale $f.sSlider \
               -orient horizontal \
               -from 0 -to $::Ibrowser(MaxDrops) \
               -resolution 1 \
               -bigincrement 10 \
               -length 130 \
               -state disabled \
               -variable ::Ibrowser(ViewDrop) } $Gui(WSA) {-showvalue 1}

    set ::Ibrowser(selectSlider) $f.sSlider
     bind $f.sSlider <ButtonPress-1> {
        IbrowserUpdateIndexFromGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    bind $f.sSlider <ButtonRelease-1> {
        IbrowserUpdateIndexFromGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    bind $f.sSlider <B1-Motion> {
        IbrowserUpdateIndexFromGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    TooltipAdd $f.sSlider \
        "Slide this scale to preview sequence of volumes."
 
    #The "sticky" option aligns items to the left (west) side
    grid $f.lVolNo -row 0 -column 0 -padx 1 -pady 1 -sticky w
    grid $f.sSlider -row 0 -column 1 -padx 1 -pady 1 -sticky w

}



proc IbrowserBuildUIForLoad { parent } {
        global Gui

    frame $parent.fTop -bg $Gui(activeWorkspace)
    pack $parent.fTop -side top 
 
    set f $parent.fTop

    # error if no private segment
    if {[catch "package require MultiVolumeReader"]} {
        DevAddLabel $f.lError \
            "Loading function is disabled\n\
            due to the unavailability\n\
            of module MultiVolumeReader." 
        pack $f.lError -side top -pady 30
        return
    }
    #--- This duplicates the interface for MultiVolumeReaderBuildGUI
    #--- as of 11/22/04. Must fix rather than duplicate code.
    IbrowserMultiVolumeReaderBuildGUI $f
    
}




proc IbrowserMultiVolumeReaderBuildGUI {parent} {
    global Gui MultiVolumeReader Module Volume Model
   
    set f $parent
    frame $f.fVols -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fNav -bg $Gui(activeWorkspace) 
    pack $f.fVols $f.fNav -side top -pady $Gui(pad)

    # The Volume frame
    set f $parent.fVols
    DevAddLabel $f.lNote "Configure the multi-volume reader:"
    frame $f.fConfig -bg $Gui(activeWorkspace) -relief groove -bd 2 
    pack $f.lNote -side top -pady 2
    pack $f.fConfig -side top

    set f $parent.fVols.fConfig
    set MultiVolumeReader(fileTypes) {hdr .bxh}
    DevAddFileBrowse $f MultiVolumeReader "fileName" "File from load dir:" \
        "MultiVolumeReaderSetFileFilter" "\$MultiVolumeReader(fileTypes)" \
        "\$Volume(DefaultDir)" "Open" "Browse for a volume file" "" "Absolute"

    frame $f.fFilter -bg $Gui(activeWorkspace)
    pack $f.fFilter -pady $Gui(pad)
    set f $f.fFilter

    set filter \
        "Load a single file: Only read the specified input file.\n\
        Load multiple files: Read files in the same directory\n\
        matching the pattern in the Filter field.              "

    eval {radiobutton $f.r1 -width 27 -text {Load a single file} \
        -variable MultiVolumeReader(filterChoice) -value single \
        -relief flat -offrelief flat -overrelief raised \
        -selectcolor white} $Gui(WEA)
    pack $f.r1 -side top -pady 2 
    TooltipAdd $f.r1 $filter 
    frame $f.fMulti -bg $Gui(activeWorkspace) -relief groove -bd 1 
    pack $f.fMulti -pady 3
    set f $f.fMulti
    eval {radiobutton $f.r2 -width 27 -text {Load multiple files} \
        -variable MultiVolumeReader(filterChoice) -value multiple \
        -relief flat -offrelief flat -overrelief raised \
        -selectcolor white} $Gui(WEA)
    TooltipAdd $f.r2 $filter 

    DevAddLabel $f.lFilter " Filter:"
    eval {entry $f.eFilter -width 24 \
        -textvariable MultiVolumeReader(filter)} $Gui(WEA)
    bind $f.eFilter <Return> "IbrowserMultiVolumeReaderLoad" 
    TooltipAdd $f.eFilter $filter 

    #The "sticky" option aligns items to the left (west) side
    grid $f.r2 -row 0 -column 0 -columnspan 2 -padx 5 -pady 3 -sticky w
    grid $f.lFilter -row 1 -column 0 -padx 1 -pady 3 -sticky w
    grid $f.eFilter -row 1 -column 1 -padx 1 -pady 3 -sticky w

    set MultiVolumeReader(filterChoice) single
    set MultiVolumeReader(singleRadiobutton) $f.r1
    set MultiVolumeReader(multipleRadiobutton) $f.r2
    set MultiVolumeReader(filterEntry) $f.eFilter
    
    set f $parent.fVols
    DevAddButton $f.bApply "Apply" "IbrowserMultiVolumeReaderLoad" 12 
    pack $f.bApply -side top -pady 5 

    set f $parent.fVols
    frame $f.fVName -bg $Gui(activeWorkspace)
    pack $f.fVName  -pady 2
    set f $f.fVName
    DevAddLabel $f.lVName "Loading:"
    label $f.eVName -width 20 -relief flat  -textvariable ::Volume(name) \
        -bg $Gui(activeWorkspace) -fg $Gui(textDark) -font {helvetica 8 }
    grid $f.lVName -row 0 -column 0 -padx 1 -pady 3 -sticky w
    grid $f.eVName -row 0 -column 1 -padx 1 -pady 3 -sticky w


    # The Navigate frame
    set f $parent.fNav

    #--- for now; need to create MultiVolumeReader(slider) but don't
    #--- want to do anything with it.
    if { 0 } {
    scale $f.sHack -orient horizontal -from 0 -to $::Ibrowser(MaxDrops) \
        -resolution 1 -bigincrement 10 -length 100 -state disabled -showvalue 1
    set MultiVolumeReader(slider) $f.sHack
    }
    DevAddLabel $f.lVolNo "Volume:"
    eval { scale $f.sSlider \
               -orient horizontal \
               -from 0 -to $::Ibrowser(MaxDrops) \
               -resolution 1 \
               -bigincrement 10 \
               -length 130 \
               -state disabled \
               -variable ::Ibrowser(ViewDrop) } $Gui(WSA) {-showvalue 1}
    set ::Ibrowser(loadSlider) $f.sSlider
     bind $f.sSlider <ButtonPress-1> {
         IbrowserUpdateIndexFromGUI
         IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    bind $f.sSlider <ButtonRelease-1> {
        IbrowserUpdateIndexFromGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    bind $f.sSlider <B1-Motion> {
        IbrowserUpdateIndexFromGUI
        IbrowserUpdateMainViewer $::Ibrowser(ViewDrop)
    }
    TooltipAdd $f.sSlider \
        "Slide this scale to navigate multi-volume sequence."
 
    #The "sticky" option aligns items to the left (west) side
    grid $f.lVolNo -row 0 -column 0 -padx 1 -pady 0 -sticky w
    grid $f.sSlider -row 0 -column 1 -padx 1 -pady 0 -sticky w
}




proc IbrowserUpdateSequences { } {
    global MultiVolumeReader 

    # clears the listbox
    set size [$::Ibrowser(seqsListBox) size]
    $::Ibrowser(seqsListBox) delete 0 [expr $size - 1]
 
    # lists all sequence loaded from Ibrowser
    set b1 [info exists ::Ibrowser(idList)]
    set n1 [expr {$b1 == 0 ? 0 : [llength $::Ibrowser(idList)]}]
    # lists all sequences loaded from other modules
    set b2 0
    set n2 0
    
    set n [expr $n1 + $n2 ]
    if {$n > 1} {
        set i 1 
        while {$i < $n1} {
            set id [lindex $::Ibrowser(idList) $i]
            $::Ibrowser(seqsListBox) insert end "$::Ibrowser($id,name)" 
            incr i
        }
        if {$n2 > 1} {
            $::Ibrowser(seqsListBox) insert end "Elsewhere-in-Slicer" 
        }
    } else {
        $::Ibrowser(seqsListBox) insert end none 
    }
}

proc IbrowserSelectSequence { } {
    global MultiVolumeReader

    set ci [$::Ibrowser(seqsListBox) cursel]
    set size [$::Ibrowser(seqsListBox) size]

    if {[string length $ci] == 0} {
        if {$size > 1} {
            DevErrorWindow "Please select a sequence."
            return
        } else {
            set ci 0 
        }
    }

    set cc [$::Ibrowser(seqsListBox) get $ci]
    set l [string trim $cc]

    if {$l == "none"} {
        DevErrorWindow "No sequence available."
        return
    } else {
        #--- for now, just bring this interval to the FG and make active.
        #--- make this the active interval.
        #--- display the first volume
        #--- make it the active volume
        #--- and put it in the background
        #--- as is the loading convention.
        set id $::Ibrowser($l,intervalID)
        IbrowserDeselectActiveInterval $::IbrowserController(Icanvas)
        IbrowserDeselectFGIcon $::IbrowserController(Icanvas)

        set ::Ibrowser(activeInterval) $id
        set ::Ibrowser(FGInterval) $id

        IbrowserSelectActiveInterval $id $::IbrowserController(Icanvas)
        IbrowserSelectFGIcon $id $::IbrowserController(Icanvas)

        MainSlicesSetVolumeAll Fore $::Ibrowser($id,0,MRMLid) 
        MainVolumesSetActive $::Ibrowser($id,0,MRMLid)
        MainSlicesSetVisibilityAll 1
        RenderAll
    }
}




proc IbrowserMultiVolumeReaderLoad { } {

    set readfailure [ MultiVolumeReaderLoad ]
    if { $readfailure } {
        return
    }

    set id $::Ibrowser(uniqueNum)
    set first $::MultiVolumeReader(firstMRMLid)
    set last $::MultiVolumeReader(lastMRMLid)

    set ::Ibrowser(loadVol,name) "imageData"
    set ::Ibrowser(loadVol,name) [format "%s-%d" $::Ibrowser(loadVol,name) $id]
    set ::Ibrowser(seqName) $::Ibrowser(loadVol,name)
    set iname $::Ibrowser(seqName)

    set vcount 0
    for {set i $first } { $i <= $last } { incr i } {
        #--- give ibrowser a way to refer to each vol
        set ::Ibrowser($id,$vcount,MRMLid) $i
        puts "loading mrmlID $i"
        incr vcount
    }
    set ::Ibrowser($id,firstMRMLid) $first
    set ::Ibrowser($id,lastMRMLid) $last
    set m [ expr $::MultiVolumeReader(noOfVolumes) -1 ]
    set tt "Loaded $::MultiVolumeReader(noOfVolumes) files: populating interval"
    
    #--- populate intervalBrowser controller.
    IbrowserMakeNewInterval $iname $::IbrowserController(Info,Ival,imageIvalType) 0.0 $m

    #--- For now, fill a position array with the
    #--- time point that each drop represents.
    #--- Assume the interval is m units long.
    #--- how many files do we have?
    set top $::MultiVolumeReader(noOfVolumes)
    set ::Ibrowser($id,numDrops) $top
    for {set zz 0} {$zz < $top} { incr zz} {
        set posVec($zz) $zz
    }
    IbrowserCreateImageDrops  $iname posVec $::Ibrowser($id,numDrops)

    if { $::Ibrowser($id,numDrops) == 1 } {
        set ::Ibrowser($id,lastMRMLid) $::Ibrowser($id,firstMRMLid)
    }

    #--- reconfigure the loadSlider, displaySlider and selectSlider
    IbrowserUpdateMaxDrops
    IbrowserSynchronizeAllSliders "active"
    
    #--- report in Ibrowser's message panel"
    set tt "Finished populating interval."
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

    #--- remove traces of these volumes from the MultiVolumeReader
    #--- this way, no sequence represented in the Ibrowser Module
    #--- will also be represented in the MultiVolumeReader Module
    #--- which seems to preserve info for the last sequence read.
    unset -nocomplain ::MultiVolumeReader(noOfVolumes)
    unset -nocomplain ::MultiVolumeReader(firstMRMLid)
    unset -nocomplain ::MultiVolumeReader(lastMRMLid)
    unset -nocomplain ::MultiVolumeReader(volumeExtent)
    set ::Volume(name) ""
}
