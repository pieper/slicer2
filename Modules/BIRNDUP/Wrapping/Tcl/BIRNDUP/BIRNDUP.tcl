package require vtk
package require vtkinteraction

#
# rely on the fact that a class loaded from the shared
# library is uniquely available through this module
#

if { [info commands dup] == "" } {
    global PACKAGE_DIR_BIRNDUP
    package provide BIRNDUP 1.0

    # source the Module's tcl file that contains it's init procedure
    set files {
        VolDeface.tcl
        BIRNDUP.tcl
        dup.tcl
        dup_sort.tcl
        dup_deidentify.tcl
        dup_review.tcl
        dup_upload.tcl
        }
        
    foreach f $files {
        source $PACKAGE_DIR_BIRNDUP/../../../tcl/$f
    }

    lappend ::Module(customModules) BIRNDUP
}
