# This scripte generates the right view for the validation of Birn Data 
# ./slicer2-linux-x86 --exec ValidationSelectWindow
set ::Validation(SubjectFolder) /nas/nas0/miriad/subjects
set ::Validation(Subject) ""

proc ValidationLoadSubject {} {
    if { $::Validation(Subject) != "" } { 
    set ::Mrml(dir) [file join $::Validation(SubjectFolder) $::Validation(Subject)] 
    set ::File(filePrefix) Visit_001/Study_0001/DerivedData/SPL/EM-reg_LONIbAb__params_params-sp-duke-2004-02-19_ic/Validation.xml
    ValidationLoadXML 
    ValidationSetView
    } else {
    DevWarningWindow "No Subject selected"
    }
}

proc ValidationLoadXML { } {
    MainMrmlRead [file join $::Mrml(dir) $::File(filePrefix)] 
    MainUpdateMRML
    MainOptionsRetrievePresetValues
    MainSetup
    RenderAll
}

proc ValidationSetView {} {
    puts "==== Start Setup Validation View "

    # Image for the  Background - later put here segmentation 
    set id [MIRIADSegmentGetVolumeByName EMSegResult1]
    MainSlicesSetVolumeAll Fore $id 

    # Image for the Background
    set id [MIRIADSegmentGetVolumeByName PD]
    MainSlicesSetVolumeAll Back $id  
    # 3D View 
    MainMenu View Quad512

    # Show 2 D Slices in 3D View 
    foreach s $::Slice(idList) {
    set ::Slice($s,visibility) 1
    MainSlicesSetVisibility ${s} 
    MainViewerHideSliceControls
    Render3D
    }

    # Set view to Slices 
    MainSlicesSetOrientAll Slices
    MainViewerHideSliceControls
    RenderAll

    puts "==== End Setup Validation View "
}

proc ValidationSelectWindow {} {
    global Gui
    set w .wValidation 
    set ::Gui(wValidation) $w

    if {[winfo exists $::Gui(wValidation)]} {destroy  $::Gui(wValidation)}
    #-------------------------------------------
    # Popup Window
    #-------------------------------------------
    toplevel $w  -bg $::Gui(activeWorkspace)
    wm title $w "Load Subject"
    wm geometry $w +[expr [winfo screenwidth $w]/2-250]+[expr [winfo screenheight $w]/2-200]
    set f $w
    
    frame $f.fSelect -bg $::Gui(activeWorkspace)
    pack $f.fSelect -side top -padx 4 -pady 4
    eval {label $f.fSelect.lText -text "Select Subject:  " } $::Gui(WTA)
    eval {menubutton $f.fSelect.mbSubject -text "None"  -menu $f.fSelect.mbSubject.m -width 13} $::Gui(WMBA) 
    pack $f.fSelect.lText  $f.fSelect.mbSubject -side left
    if {[catch { set SubjectList [exec ls -1 $::Validation(SubjectFolder) | grep 0003 ] } ErrorMessage] } {
       DevErrorWindow "No subjects in $::Validation(SubjectFolder) ErrorMessage: $ErrorMessage" 
       return
    }
    TooltipAdd  $f.fSelect.mbSubject "Select Subject to be validated"

    # Define Menu selection 

    eval {menu $f.fSelect.mbSubject.m} $::Gui(WMA)
    # Add Selection entry
    set MenuMaxLength 25 
    set NumCasecade [expr [llength $SubjectList] / $MenuMaxLength + 1]
    for {set i 0} {$i < $NumCasecade} { incr i } {
    set Min [expr $i*$MenuMaxLength +1 ]
    set Max [expr ($i+1)*$MenuMaxLength]
        $f.fSelect.mbSubject.m add cascade -label "Subjects $Min - $Max" \
        -menu $f.fSelect.mbSubject.m.m$i
    menu $f.fSelect.mbSubject.m.m$i
    incr Min -1 
    incr Max -1
    set SelectedSubjectList  [lrange $SubjectList $Min $Max] 
    set index 0
    foreach SUBJECT  $SelectedSubjectList {
        set SUBJECT [file tail $SUBJECT]
        $f.fSelect.mbSubject.m.m$i add command -label $SUBJECT -command "set ::Validation(Subject) $SUBJECT; $f.fSelect.mbSubject configure -text $SUBJECT"
        incr index
    }
    }

    eval {button $f.bLoad -text "Load Subject" -width 16 -command "ValidationLoadSubject"} $::Gui(WBA)  
    eval {label $f.lText -text "Do not close the window otherwise you \nhave to restart slicer to load a subject !" } $::Gui(WTA)   
    pack $f.bLoad $f.lText -side top -padx 4 -pady 4
}


#puts "=== Start Setup Validation View ==="
# ValidationSelectWindow
# puts "=== Finished Setup Validation View ==="

