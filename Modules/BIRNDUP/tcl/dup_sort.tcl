
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_sort - the sort pane of the birndup interface

# TODO : 
    * make an about_dialog
    * the "view" method currently copies to temp dir - should fix the dicom
    loader that accepts a list of filenames
}
#
#########################################################

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class dup_sort] == "" } {

    itcl::class dup_sort {
        inherit iwidgets::Labeledframe

        public variable parent ""
        public variable sourcedir ""

        variable _defacedir ""
        variable _study
        variable _series
        variable _DICOMFiles 

        constructor {args} {}
        destructor {}

        method refresh {} {}
        method view {id} {}
        method setdeident {id method} {}
        method fill {dir} {}
        method sort {} {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup_sort::constructor {args} {
    global env

    set _study(project) ""
    set _study(visit) ""
    set _study(study) ""
    set _study(birnid) ""

    set cs [$this childsite]

    #
    # the sorting interface is created here in three frames, but they
    # are not packed until the directory is filled in (fill method)
    #
    set f $cs.info
    iwidgets::labeledframe $f -labeltext "Study Info" -labelpos nw
    set fcs [$f childsite]
    pack [iwidgets::entryfield $fcs.sourcedir -labeltext "Source Dir: " -state readonly]
    pack [iwidgets::entryfield $fcs.project -labeltext "Project #: " -textvariable [itcl::scope _study(project)]]
    pack [iwidgets::entryfield $fcs.visit -labeltext "Visit #: " -textvariable [itcl::scope _study(visit)]]
    pack [iwidgets::entryfield $fcs.study -labeltext "Study #: " -textvariable [itcl::scope _study(study)]]
    pack [iwidgets::entryfield $fcs.birnid -labeltext "BIRN ID: " -textvariable [itcl::scope _study(birnid)]] 
    ::iwidgets::Labeledwidget::alignlabels $fcs.sourcedir $fcs.project $fcs.visit $fcs.study $fcs.birnid
    set f $cs.series
    ::iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic

    set f $cs.buttons
    frame $f
    pack [button $f.sort -text "Sort" -command "$this sort"]


    #
    # a button to be packed when the sort interface isn't
    #
    pack [button $cs.new -text "Select new study to process..." -command "$this fill choose"]

    eval itk_initialize $args
}


itcl::body dup_sort::destructor {} {
}

itcl::configbody dup_sort::parent {
    
    if { $parent != "" } {
        set _defacedir [$parent pref DEFACE_DIR]
    }
}

itcl::body dup_sort::fill {dir} {

    if { $dir == "choose" } {
        $parent fill choose
        return
    }

    set cs [$this childsite]
    set infocs [$cs.info childsite]

    $infocs.sourcedir configure -state normal
    $infocs.sourcedir delete 0 end
    $infocs.sourcedir insert end $dir
    $infocs.sourcedir configure -state readonly

    if { $dir == "" } {
        foreach w [winfo children [[$this childsite].series childsite]] {
            destroy $w
        }
        pack forget $cs.info
        pack forget $cs.buttons
        pack forget $cs.series
        pack $cs.new 
        return
    } 

    pack forget $cs.new 
    pack $cs.info -fill x -pady 5
    pack $cs.series -fill both -expand true
    pack $cs.buttons -fill x  -pady 5

    DefaceFindDICOM $dir *
    if {$::FindDICOMCounter <= 0} {
        DevErrorWindow "No DICOM files found"
    }

    # save a local copy
    array set _DICOMFiles [array get ::DICOMFiles]

    array unset _series
    set _series(ids) ""
    set study $_DICOMFiles(0,StudyInstanceUID)
    set patient $_DICOMFiles(0,PatientID)

    for  {set i 0} {$i < $::FindDICOMCounter} {incr i} {
        if { $study != $_DICOMFiles($i,StudyInstanceUID) ||
                $patient != $_DICOMFiles(0,PatientID) } {
            DevErrorWindow "Multiple patients and/or studies in source directory\n\n$_DICOMFiles(0,FileName)\nand\n$_DICOMFiles($i,FileName)"
            return
        }
        set id $_DICOMFiles($i,SeriesInstanceUID)
        if { [lsearch $_series(ids) $id] == -1 } {
            lappend _series(ids) $id
            set _series($id,list) $i
            set _series($id,SeriesInstanceUID) $_DICOMFiles($i,SeriesInstanceUID)
            set _series($id,FlipAngle) $_DICOMFiles($i,FlipAngle)
        } else {
            lappend _series($id,list) $i
        }
    }

    set sf [[$this childsite].series childsite]
    set _series(master) ""
    grid [iwidgets::entryfield $sf.lmaster -labeltext "Mask Master:" -textvariable [itcl::scope _series(master)] -state readonly] -columnspan 4

    set row 1
    foreach id $_series(ids) {
        label $sf.l$id -text "Ser $id, Flip $_series($id,FlipAngle)" -anchor w -justify left
        radiobutton $sf.cb$id -value $id -variable [itcl::scope _series(master)]
        iwidgets::optionmenu $sf.om$id -command "$this setdeident $id \[$sf.om$id get\]"
        $sf.om$id insert end "Mask" "Deface" "Header Only" "No Deident" "Ignore"
        $this setdeident $id "Mask"
        button $sf.b$id -text "View" -command "$this view $id"
        grid $sf.l$id $sf.cb$id $sf.om$id $sf.b$id -row $row -sticky ew
        incr row
    }
}

itcl::body dup_sort::sort {} {
    # create the needed entries for each series

    if { $_study(project) == "" } {
        DevErrorWindow "Please set Project #"
        return
    }
    if { $_study(visit) == "" } {
        DevErrorWindow "Please set Visit #"
        return
    }
    if { $_study(study) == "" } {
        DevErrorWindow "Please set Study #"
        return
    }
    if { $_study(birnid) == "" } {
        DevErrorWindow "Please set BIRN ID"
        return
    }
    if { $_defacedir == "" } {
        DevErrorWindow "Please set Destination Directory (temp area for deface processing)"
        return
    }
    if { $_series(master) == "" } {
        DevErrorWindow "Please select master series for masking"
        return
    }
    if { $_series($_series(master),deident_method) != "Deface" } {
        DevErrorWindow "Master series ($_series(master)) must be set to deface to be used as a mask"
        return
    }

    set studypath $_defacedir/Project_$_study(project)/$_study(birnid)/Visit_$_study(visit)/Study_$_study(study)/Raw_Data

    foreach id $_series(ids) {
        set dir $studypath/$id
        if { [glob -nocomplain $dir/*] != "" } {
            if { [DevOKCancel "$studypath is not empty - okay to delete?"] != "ok" } {
                return
            } 
        }
    }
    foreach id $_series(ids) {
        set dir $studypath/$id
        file delete -force $dir
        file mkdir $dir
        set _series($id,destdir) $dir
    }

    for  {set i 0} {$i < $::FindDICOMCounter} {incr i} {
        set id $_DICOMFiles($i,SeriesInstanceUID)
        set inum $_DICOMFiles($i,ImageNumber)
        file copy $_DICOMFiles($i,FileName) $_series($id,destdir)/$inum.dcm
    }

    set deient_operations ""
    lappend deident_operations "dcanon -deface $_series($_series(master),destdir)"
    foreach id $_series(ids) {
        if { $id == $_series(master) } {
            continue
        }
        switch $_series($id,deident_method) {
            "Deface" {
                lappend deident_operations "dcanon -deface $_series($id,destdir)"
            }
            "Mask" {
                lappend deident_operations "dcanon -mask $_series($_series(master),destdir) $_series($id,destdir)"
            }
            "Header Only" {
                lappend deident_operations "dcanon -convert $_series($id,destdir)"
            }
            "No Dident" {
                lappend deident_operations "dcanon -noanon -convert $_series($id,destdir)"
            }
            "Ignore" {
                # nothing
            }
            default {
                puts stderr "unknown deidentification method  $_series($id,deident_method)"
            }
        }
    }
    $this fill ""

    set fp [open "$studypath/deidentify_operations" w]
    puts $fp $deident_operations
    close $fp

    $parent log "sort operation complete for $sourcedir to $studypath"

    tk_messageBox -message "Directory sorted"
    $parent refresh deidentify
}

itcl::body dup_sort::setdeident {id method} {
    set _series($id,deident_method) $method
}

itcl::body dup_sort::view {id} {

    if { ![info exists _series(ids)] } {
        error "no series loaded"
    }
    if { [lsearch $_series(ids) $id] == -1 } {
        error "series $id not loaded"
    }
    if { [info command VolAnalyzetempdir] == "" } {
        error "no temp dir command available"
    }
    set viewdir [VolAnalyzetempdir]/$id.[pid]
    file delete -force $viewdir
    file mkdir $viewdir

    foreach i $_series($id,list) {
        file copy $_DICOMFiles($i,FileName) $viewdir/
    }

    set volid [DICOMLoadStudy $viewdir]
    Volume($volid,node) SetName "Ser $id, Flip $_series($id,FlipAngle)"
    MainUpdateMRML
}
