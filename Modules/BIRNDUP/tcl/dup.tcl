
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup - the interface to the birn deidentification and 
upload pipeline (birndup)

# TODO : 
    about_dialog
}
#
#########################################################

#
# Default resources
# 
option add *dup.appname "BIRN Deidentification and Upload Pipeline" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class dup] == "" } {

    itcl::class dup {
        inherit iwidgets::Mainwindow

        itk_option define -appname appname Appname {}

        constructor {args} {}
        destructor {}

        variable _prefs ;# array of preferences

        variable _sort
        variable _deidentify
        variable _review
        variable _upload


        method menus {} {}
        method about_dialog {} {}
        method pref { {KEY ""} } {}
        method prefs {} {}
        method visits {} {}
        method log {message} {}

        method fill { {dir "choose"} } {}
        method refresh { {pane "all"} } {}

        method statusvar {} { return [itcl::scope _statusVar($this)] }
        method w {} {return $_w}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup::constructor {args} {

    $itk_component(status) configure -anchor w

    set cs [$this childsite]

    pack [iwidgets::panedwindow $cs.panes -orient vertical] -fill both -expand true

    foreach p {sort deidentify review upload} {
        $cs.panes add $p
        set w [$cs.panes childsite $p]
        set _$p $w.$p
        label $w.l -text [string totitle $p] -bg white
        pack $w.l -side top -fill x
        pack [dup_$p [set _$p]] -fill both -expand true
    }


    # put the app name and logo at the bottom
    set im [image create photo -file $::PACKAGE_DIR_BIRNDUP/../../../images/new-birn.ppm]
    pack [label $cs.logo -image $im -bg white] -fill x -anchor s

    $this menus
    $this configure -title "BIRN Deidentification and Upload Pipeline - Evaluation Use Only"

    eval itk_initialize $args

    $this prefs

    foreach p {sort deidentify review upload} {
        [set _$p] configure -parent $this
    }

    after idle "$this refresh"
}


itcl::body dup::destructor {} {
}


itcl::body dup::menus {} {
    $this menubar config \
        -font {{MS Sans Serif} 12} \
        -menubuttons {
            menubutton file -text "File" -menu {
                options -tearoff false
                command newstud -label "New Directory..." \
                    -helpstr "Open a New Source Directory for Deidentification" \
                    -command "$this fill"
                separator sep1
                command exit -label "Close" -command "destroy [namespace tail $this]" \
                    -helpstr "Close BIRNDUP"
            menubutton help -text "Help" -menu {
                options -tearoff false
                command about -label "About..." \
                    -helpstr "About BIRNDUP" \
                    -command {$this about_dialog}
                command help -label "Help..." \
                    -helpstr "Help window" \
                    -command {$this help}
            }
        }
    }
}

itcl::body dup::fill { {dir "choose"} } {

    catch "destroy $_sort"
    pack [dup_sort $_sort -parent $this] -fill both -expand true
    
    if { $dir == "choose" } {
        set dir [tk_chooseDirectory]
    }
    if { $dir != "" } { 
        $_sort fill $dir
    }
}

itcl::body dup::refresh { {pane "all"} } {
    switch $pane {
        "deidentify" {
            $_deidentify refresh
        }
        "review" {
            $_review refresh
        }
        "upload" {
            $_upload refresh
        }
        "all" {
            $_deidentify refresh
            $_review refresh
            $_upload refresh
        }
    }
}

itcl::body dup::visits {} {

    set defacedir [$this pref DEFACE_DIR]
    return [glob -nocomplain $defacedir/Project_*/*/Visit_*/Study_*/Raw_Data]
}

itcl::body dup::log { message } {
    set fp [open $::env(HOME)/.birndup/log "a"]
    puts $fp "\{$::env(USER) [clock format [clock seconds]]\} \{$message\}"
    close $fp
}

itcl::body dup::pref { {KEY ""} } {
    if { $KEY == "" } {
        return [parray _prefs]
    }
    if { [info exists _prefs($KEY)] } {
        return $_prefs($KEY)
    } else {
        return ""
    }
}

itcl::body dup::prefs { } {

    if { ![file exists $::env(HOME)/.birndup/prefs] } {
        set resp [tk_messageBox -type okcancel -message "A preferences file must be created for you.\n\nClick Ok to continue or Cancel to exit"]
        if { $resp == "cancel" } {
            itcl::delete object $this
        }

        # put all preference entries here
        set _prefs(DEFACE_DIR) /opt/birn/deface

        file mkdir $::env(HOME)/.birndup    
        set fp [open $::env(HOME)/.birndup/prefs w]
        foreach n [array names _prefs] {
            puts $fp "$n \"$_prefs($n)\""
        }
        close $fp
    }

    set fp [open $::env(HOME)/.birndup/prefs r]
    array set _prefs [read $fp]
    close $fp
}

proc dup_demo {} {

    foreach c { "" _sort _deidentify _review _upload } {
        catch "itcl::delete class dup$c"
        source $::PACKAGE_DIR_BIRNDUP/../../../tcl/dup$c.tcl;
    }

    dup .t
    .t activate
    wm geometry .t 900x700+50+50
}
