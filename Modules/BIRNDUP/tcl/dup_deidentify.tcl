
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_deidentify - the deidentify pane of the birndup interface

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
if { [itcl::find class dup_deidentify] == "" } {

    itcl::class dup_deidentify {
        inherit iwidgets::Labeledwidget

        public variable sourcedir ""

        variable _study
        variable _series
        variable _DICOMFiles 

        constructor {args} {}
        destructor {}

        method view {id} {}
        method setdeident {id method} {}
        method fill {dir} {}
        method deidentify {} {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup_deidentify::constructor {args} {
    global env

    set _study(project) ""
    set _study(visit) ""
    set _study(study) ""
    set _study(birnid) ""

    set cs [$this childsite]

    set f $cs.info
    pack [iwidgets::labeledframe $f -labeltext "Study Info" -labelpos nw] -fill x -pady 5
    set fcs [$f childsite]
    pack [iwidgets::entryfield $fcs.sourcedir -labeltext "Source Dir: " -state readonly]
    pack [iwidgets::entryfield $fcs.project -labeltext "Project #: " -textvariable [itcl::scope _study(project)]]
    pack [iwidgets::entryfield $fcs.visit -labeltext "Visit #: " -textvariable [itcl::scope _study(visit)]]
    pack [iwidgets::entryfield $fcs.study -labeltext "Study #: " -textvariable [itcl::scope _study(study)]]
    pack [iwidgets::entryfield $fcs.birnid -labeltext "BIRN ID: " -textvariable [itcl::scope _study(birnid)]] 
    ::iwidgets::Labeledwidget::alignlabels $fcs.sourcedir $fcs.project $fcs.visit $fcs.study $fcs.birnid
    set f $cs.series
    pack [::iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true

    set f $cs.buttons
    pack [frame $f] -fill x  -pady 5
    pack [iwidgets::entryfield $f.destdir -labeltext "Destination Dir: " -textvariable [itcl::scope _study(destdir)]]
    pack [button $f.deidentify -text "Sort" -command "$this deidentify"]

    eval itk_initialize $args
}


itcl::body dup_deidentify::destructor {} {
}


itcl::body dup_deidentify::fill {dir} {

    [[$this childsite].info childsite].sourcedir configure -state normal
    [[$this childsite].info childsite].sourcedir delete 0 end
    [[$this childsite].info childsite].sourcedir insert end $dir
    [[$this childsite].info childsite].sourcedir configure -state readonly

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
        $sf.om$id insert end "Mask" "Deface" "Ignore"
        $this setdeident $id "Mask"
        button $sf.b$id -text "View" -command "$this view $id"
        grid $sf.l$id $sf.cb$id $sf.om$id $sf.b$id -row $row -sticky ew
        incr row
    }
}

itcl::body dup_deidentify::deidentify {} {
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
    if { $_study(destdir) == "" } {
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

    set studypath $_study(destdir)/Project_$_study(project)/$_study(birnid)/Visit_$_study(visit)/Raw_Data

    foreach id $_series(ids) {
        set dir $studypath/$id
        if { [glob -nocomplain $dir/*] != "" } {
            if { [DevOKCancel "$dir is not empty - okay to delete?"] != "ok" } {
                return
            }
        }
        file delete -force $dir
        file mkdir -force $dir
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
            "Ignore" {
                # nothing
            }
            default {
                puts stderr "unknown deidentification method  $_series($id,deident_method)"
            }
        }
    }
    puts $deident_operations
    return $deident_operations
}

itcl::body dup_deidentify::setdeident {id method} {
    set _series($id,deident_method) $method
}

itcl::body dup_deidentify::view {id} {

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
