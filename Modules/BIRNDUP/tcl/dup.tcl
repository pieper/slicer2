
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

        method menus {} {}
        method about_dialog {} {}

        method statusvar {} { return [itcl::scope _statusVar($this)] }
        method w {} {return $_w}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup::constructor {args} {

    $itk_component(status) configure -anchor w

    
    $this menus
    $this configure -title "BIRN Deidentification and Upload Pipeline"

    set cs [$this childsite]

    pack [iwidgets::panedwindow $cs.panes -orient vertical] -fill both -expand true
    $cs.panes add "sort"
    pack [dup_sort [$cs.panes childsite "sort"].sort] -fill both -expand true
    $cs.panes add "deident"
    $cs.panes add "review"
    $cs.panes add "upload"

    # put the app name and logo at the bottom
    set im [image create photo -file $::PACKAGE_DIR_BIRNDUP/../../../images/new-birn.ppm]
    pack [label $cs.logo -image $im -bg white] -fill x -anchor s
    eval itk_initialize $args
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
                    -command ""
                separator sep1
                command exit -label "Exit" -command "::clean_exit" \
                    -helpstr "Exit BIRNDUP"
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

proc dup_demo {} {

    catch "itcl::delete class dup"
    source $::PACKAGE_DIR_BIRNDUP/../../../tcl/dup.tcl;

    dup .t
    .t activate
    wm geometry .t 900x700
}
