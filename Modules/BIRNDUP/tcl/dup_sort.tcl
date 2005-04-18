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
# FILE:        dup_sort.tcl
# PROCEDURES:  
#==========================================================================auto=

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
    pack [iwidgets::entryfield $fcs.sourcedir -labeltext "Source Dir: " -state readonly] -fill x
    pack [iwidgets::entryfield $fcs.project -labeltext "Project #: " -textvariable [itcl::scope _study(project)]] -fill x
    pack [iwidgets::entryfield $fcs.visit -labeltext "Visit #: " -textvariable [itcl::scope _study(visit)]] -fill x
    pack [iwidgets::entryfield $fcs.study -labeltext "Study #: " -textvariable [itcl::scope _study(study)]] -fill x
    pack [iwidgets::entryfield $fcs.birnid -labeltext "BIRN ID: " -textvariable [itcl::scope _study(birnid)]]  -fill x
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

    set ::DICOMrecurse "true"
    set aborted [DefaceFindDICOM $dir *]

    if { $aborted == "true" } {
        $parent fill choose
        return
    }

    if {$::FindDICOMCounter <= 0} {
        DevErrorWindow "No DICOM files found"
        $parent fill choose
        return
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
            DevErrorWindow "Multiple patients and/or studies in source directory\n\n$_DICOMFiles(0,FileName)\nand\n$_DICOMFiles($i,FileName)\nThis must be corrected before you can run the files through this pipeline."
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

    #
    # set the BIRN ID for this subject
    # - first be sure that the entry exists for this birn id
    # - then pull the value from the table
    #

    set birnid_manager [$parent pref UPLOAD2_DIR]/external/birnid_man.jar
    set linktable [$parent pref LINKTABLE]
    set inst [$parent pref INSTITUTION]

    if { [catch "exec java -jar $birnid_manager -create -p $inst -l $linktable -c $patient" resp] } {
        DevErrorWindow "Cannot execute BIRN ID manager.  Ensure that UPLOAD2_DIR preference is correct and that Java is installed on your machine.\n\n$resp"
    } else {

        if { [catch "exec java -jar $birnid_manager -find -l $linktable -c $patient" resp] } {
            DevErrorWindow "Cannot execute BIRN ID manager to access BIRN ID.  Ensure that LINKTABLE preference is correct.\n\n$resp"
            set birnid ""
        } else {
            set birnid ""
            scan $resp {Birn ID=%[^,]s} birnid

            if { $birnid == "" } {
                DevErrorWindow "Cannot parse BIRN ID.  Response is: \n$resp"
                puts stderr "Cannot parse BIRN ID.  Response is: \n$resp"
            }

            set _study(birnid) $birnid
            $infocs.birnid configure -state normal
            $infocs.birnid delete 0 end
            $infocs.birnid insert end $birnid
            $infocs.birnid configure -state readonly
        }
    }


    #
    # create the facial deidentification options
    #
    set sf [[$this childsite].series childsite]
    set _series(master) ""
    grid [iwidgets::entryfield $sf.lmaster -labeltext "Mask Master:" -textvariable [itcl::scope _series(master)] -state readonly] -columnspan 4

    set row 1
    foreach id $_series(ids) {
        label $sf.l$id -text "Ser $id, Flip $_series($id,FlipAngle)" -anchor w -justify left
        radiobutton $sf.cb$id -value $id -variable [itcl::scope _series(master)]
        iwidgets::optionmenu $sf.om$id -command "$this setdeident $id \[$sf.om$id get\]"
        $sf.om$id insert end "Mask" "Deface" "Header Only" "As Is" "Do Not Upload"
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

    set has_mask "no"
    foreach id $_series(ids) {
        if { $_series($id,deident_method) == "Mask" } {
            set has_mask "yes"
        }
    }

    if { $has_mask == "yes" && $_series(master) == "" } {
        DevErrorWindow "Please select master series for masking"
        return
    }
    if { $has_mask == "no" && $_series(master) != "" } {
        DevWarningWindow "No series selected for masking - Mask Master ignored."
    }

    if { $_series(master) != "" && ($_series($_series(master),deident_method) != "Deface") } {
        DevErrorWindow "Master series must be set to deface to be used as a mask.  Leave Mask Master blank if no masking is needed."
        set _series(master) ""
        return
    }

    set studypath $_defacedir/Project_$_study(project)/$_study(birnid)/Visit_$_study(visit)/Study_$_study(study)/Raw_Data

    if { [glob -nocomplain $studypath] != "" } {
        if { [DevOKCancel "$studypath is not empty - okay to delete?"] != "ok" } {
            return
        } 
        file delete -force $studypath
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

    set deident_operations ""
    if { $_series(master) != "" } {
        # must run the master deface first if it exists
        lappend deident_operations "dcanon -deface $_series($_series(master),destdir)"
    }
    foreach id $_series(ids) {
        if { $id == $_series(master) } {
            continue
        }
        switch $_series($id,deident_method) {
            "Deface" {
                lappend deident_operations "dcanon -deface $_series($id,destdir)"
            }
            "Mask" {
                lappend deident_operations "dcanon -mask $_series($_series(master),destdir)-anon $_series($id,destdir)"
            }
            "Header Only" {
                lappend deident_operations "dcanon -convert $_series($id,destdir)"
            }
            "As Is" {
                lappend deident_operations "dcanon -noanon -convert $_series($id,destdir)"
            }
            "Do Not Upload" {
                # nothing
            }
            default {
                puts stderr "unknown deidentification method  $_series($id,deident_method)"
            }
        }
    }
    $this fill ""
    
    set fp [open "$studypath/source_directory" w]
    puts $fp $sourcedir
    close $fp

    set fp [open "$studypath/deidentify_operations" w]
    puts $fp $deident_operations
    close $fp

    $parent log "sort operation complete for $sourcedir to $studypath"

    tk_messageBox -message "Directory sorted"
    $parent refresh 
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
