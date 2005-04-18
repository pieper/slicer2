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
# FILE:        dup_review.tcl
# PROCEDURES:  
#==========================================================================auto=

# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_review - the review pane of the birndup interface

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
if { [itcl::find class dup_review] == "" } {

    itcl::class dup_review {
        inherit iwidgets::Labeledwidget

        public variable parent ""

        variable _frame ""

        constructor {args} {}
        destructor {}

        method refresh {} {}
        method run {studydir} {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup_review::constructor {args} {
    global env

    set cs [$this childsite]

    set f $cs.frame
    pack [iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true -pady 15
    set _frame $f

    eval itk_initialize $args
}


itcl::body dup_review::destructor {} {
}

itcl::body dup_review::refresh {} {

    foreach w [winfo children $_frame] {
        destroy $w
    }

    set defacedir [$parent pref DEFACE_DIR]
    set studies [$parent studies]
    

    set b 0
    foreach s $studies {
        if { [file exists $s/ready_for_upload] } {
            continue
        }
        if { ![file exists $s/ready_for_review] } {
            continue
        }
        set birnid [lindex [file split $s] end-3] 
        set bb $_frame.b$b 
        pack [button $bb -text "Review $birnid" -command "$this run $s"]
        TooltipAdd $bb "$s"
        incr b
    }

    if { $b == 0 } {
        pack [label $_frame.l -text "Nothing to review"]
        return
    }
}

itcl::body dup_review::run {studydir} {

    $parent log "starting review of $studydir"

    # TODO - this avoids warning messages when slicer starts
    set ::env(SLICER_CUSTOM_CONFIG) "true"
    # TODO - this is linux only
    exec $::env(SLICER_HOME)/slicer2-linux-x86 --agree_to_license $::PACKAGE_DIR_BIRNDUP/../../../tcl/gonogo.tcl $studydir

    if { ![file exists $studydir/upload_list.txt] } {
        # user cancelled
        return
    }

    package require fileutil
    set to_upload [::fileutil::cat $studydir/upload_list.txt]
    set to_defer [::fileutil::cat $studydir/defer_list.txt]
    
    set defercount [llength $to_defer]
    if { $defercount > 0 } {
        set resp [DevOKCancel "The Study contains $defercount series that did not pass review.\n\nClick Ok to upload only the approved series or cancel to defer the entire study."]
        if { $resp != "ok" } {
            set sourcedir [::fileutil::cat $studydir/source_directory] 
            DevErrorWindow "The study in $sourcedir did not pass review.  Manual defacing must be used."
            file delete -force $studydir
            $parent log "manual defacing needed for $studydir"
            $parent refresh 
            return
        }
    }
    close [open $studydir/ready_for_upload "w"]


    $parent log "finished review of $studydir"
    $parent refresh 
}

