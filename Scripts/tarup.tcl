
set __comment__ {

    tarup.tcl

    source this file into a running slicer2.1 to make a distribution 
    copy of slicer in a new directory based on the currently loaded Modules.
    The resulting program will be self-contained, without intermediate build
    files, CVS directories, and stripped of debugging symbols if possible.

    the tarup proc does the following steps:
    - makes the destination directory
    - copies the toplevel launcher and script
    - copies the tcl libs and binaries
    - copies the vtk libs and wrapping files
    - copies the slicerbase libs and wrapping files
    - copies each of the modules libs and wrapping files
    - removes CVS dirs that have been copied by accident

    It does all this while taking into account platform differences in naming schemes
    and directory layouts.
}

proc tarup { {destdir "auto"} } {

    set cwd [pwd]
    cd $::env(SLICER_HOME)

    if { $destdir == "auto" } {
        # eventually make automatic dirname with date (once I decide where it should go)
        error "please select destination directory"
    }

    puts "Creating distribution in $destdir..."

    if { ![file exists $destdir] } {
        file mkdir $destdir
    }
    if { ![file writable $destdir] } {
        error "can't write to $destdir"
    }

    #
    # grab the top-level files - the launch executable and script
    #
    puts " -- copying launcher files"
    switch $::env(BUILD) {
        "solaris8" { set launcher slicer2-solaris-sparc }
        "Darwin" { set launcher slicer2-darwin-ppc }
        "redhat7.3" { set launcher slicer2-linux-x86 }
        "Win32VC7" { set launcher slicer2-win32.exe }
    }
    file copy $launcher $destdir
    file copy launch.tcl $destdir

    #
    # grab the tcl libraries and binaries
    #
    puts " -- copying tcl files"
    file mkdir $destdir/Lib/$::env(BUILD)/tcl/lib
    file copy -force $::env(TCL_LIB_DIR)/tcl8.4 $destdir/Lib/$::env(BUILD)/tcl/lib
    file copy -force $::env(TCL_LIB_DIR)/tk8.4 $destdir/Lib/$::env(BUILD)/tcl/lib

    file mkdir $destdir/Lib/$::env(BUILD)/tcl/bin
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" { 
            file copy -force $::env(TCL_LIB_DIR)/libtcl8.4.so $destdir/Lib/$::env(BUILD)/tcl/lib
            file copy -force $::env(TCL_LIB_DIR)/libtk8.4.so $destdir/Lib/$::env(BUILD)/tcl/lib
        }
        "Darwin" {
            file copy -force $::env(TCL_LIB_DIR)/libtcl8.4.dylib $destdir/Lib/$::env(BUILD)/tcl/lib
            file copy -force $::env(TCL_LIB_DIR)/libtk8.4.dylib $destdir/Lib/$::env(BUILD)/tcl/lib
        }
        "Win32VC7" { 
            file copy -force $::env(TCL_BIN_DIR)/tcl84.dll $destdir/Lib/$::env(BUILD)/tcl/bin
            file copy -force $::env(TCL_BIN_DIR)/tclpip84.dll $destdir/Lib/$::env(BUILD)/tcl/bin
            file copy -force $::env(TCL_BIN_DIR)/tk84.dll $destdir/Lib/$::env(BUILD)/tcl/bin
            file copy -force $::env(TCL_BIN_DIR)/wish84.exe $destdir/Lib/$::env(BUILD)/tcl/bin
        }
    }

    #
    # grab the vtk libraries and binaries
    #
    puts " -- copying vtk files"
    file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" - 
        "Darwin" {
            file copy -force $::env(VTK_BIN_DIR)/Wrapping/Tcl/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl
        }
        "Win32VC7" { 
            file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/Debug
            file copy -force $::env(VTK_BIN_DIR)/Wrapping/Tcl/Debug/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/Debug
        }
    }

    file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" { 
            set libs [glob Lib/$::env(BUILD)/vtk/VTK-build/bin/*.so]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
                set ll [file tail $lib]
                exec strip $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/$ll
            }
        file copy Lib/$::env(BUILD)/vtk/VTK-build/bin/vtk $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
        }
        "Darwin" {
            set libs [glob Lib/$::env(BUILD)/vtk/VTK-build/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
            }
        file copy Lib/$::env(BUILD)/vtk/VTK-build/bin/vtk $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
        }
        "Win32VC7" { 
            file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/debug
            set libs [glob Lib/$::env(BUILD)/vtk/VTK-build/bin/debug/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/debug
            }
        }
    }

    #
    # grab the Base build and tcl
    #
    puts " -- copying SlicerBase files"
    file mkdir $destdir/Base
    file copy -force Base/tcl $destdir/Base
    file mkdir $destdir/Base/Wrapping/Tcl/vtkSlicerBase
    file copy Base/Wrapping/Tcl/vtkSlicerBase/pkgIndex.tcl $destdir/Base/Wrapping/Tcl/vtkSlicerBase
    file copy Base/Wrapping/Tcl/vtkSlicerBase/vtkSlicerBase.tcl $destdir/Base/Wrapping/Tcl/vtkSlicerBase
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" { 
            file mkdir $destdir/Base/builds/$::env(BUILD)/bin
            set libs [glob Base/builds/$::env(BUILD)/bin/*.so]
            foreach lib $libs {
                file copy $lib $destdir/Base/builds/$::env(BUILD)/bin
                set ll [file tail $lib]
                exec strip $destdir/Base/builds/$::env(BUILD)/bin/$ll
            }
        }
        "Darwin" {
            file mkdir $destdir/Base/builds/$::env(BUILD)/bin
            set libs [glob Base/builds/$::env(BUILD)/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Base/builds/$::env(BUILD)/bin
            }
        }
        "Win32VC7" { 
            file mkdir $destdir/Base/builds/$::env(BUILD)/bin/debug
            set libs [glob Base/builds/$::env(BUILD)/bin/debug/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Base/builds/$::env(BUILD)/bin/debug
            }
        }
    }

    #
    # grab all the slicer Modules
    # - the ones currently loaded are the ones in env(SLICER_MODULES_TO_REQUIRE)
    # - they are either in env(SLICER_HOME)/Modules or in a directory
    #   in the list env(SLICER_MODULES)
    #
    puts " -- copying Modules"
    foreach mod $::env(SLICER_MODULES_TO_REQUIRE) {

        set moddir ""
        if { [file exists $::env(SLICER_HOME)/Modules/$mod] } {
            set moddir $::env(SLICER_HOME)/Modules/$mod
        } else {
            foreach m $::env(SLICER_MODULES) {
                if { [file exists $m/mod] } {
                    set moddir $m/$mod
                    break
                }
            }
        }
        if { $moddir == "" } {
            puts stderr "can't find source directory for $mod -- skipping"
            continue
        }

        puts "    $mod"

        set moddest $destdir/Modules/$mod
        file mkdir $moddest
        if { [file exists $moddir/tcl] } {
            file copy -force $moddir/tcl $moddest
        }
        file mkdir $moddest/Wrapping/Tcl/$mod
        file copy $moddir/Wrapping/Tcl/$mod/pkgIndex.tcl $moddest/Wrapping/Tcl/$mod
        file copy $moddir/Wrapping/Tcl/$mod/$mod.tcl $moddest/Wrapping/Tcl/$mod
        switch $::env(BUILD) {
            "solaris8" -
            "redhat7.3" { 
                file mkdir $moddest/builds/$::env(BUILD)/bin
                set libs [glob -nocomplain $moddir/builds/$::env(BUILD)/bin/*.so]
                foreach lib $libs {
                    file copy $lib $moddest/builds/$::env(BUILD)/bin
                    set ll [file tail $lib]
                    exec strip $moddest/builds/$::env(BUILD)/bin/$ll
                }
            }
            "Darwin" {
                file mkdir $moddest/builds/$::env(BUILD)/bin
                set libs [glob -nocomplain $moddir/builds/$::env(BUILD)/bin/*.dylib]
                foreach lib $libs {
                    file copy $lib $moddest/builds/$::env(BUILD)/bin
                }
            }
            "Win32VC7" { 
                file mkdir $moddest/builds/$::env(BUILD)/bin/debug
                set libs [glob -nocomplain $moddir/builds/$::env(BUILD)/bin/debug/*.dll]
                foreach lib $libs {
                    file copy $lib $moddest/builds/$::env(BUILD)/bin/debug
                }
            }
        }
        if { [file exists $moddir/images] } {
            file copy -force $moddir/images $moddest
        }
    }

    #
    # remove any stray CVS dirs in target
    #

    foreach cvsdir [rglob $destdir CVS] {
        file delete -force $cvsdir
    }

    cd $cwd

    puts "tarup complete."
}


#
# recursive glob - find all files and/or directories in or below 'path'
# that match 'pattern'
# - note, if a directory matches, it's contents are not searched for further matches
#
proc rglob { path {pattern *} } {

    if { [string match "*/$pattern" $path] } {
        return $path
    } 
    if { ![file isdirectory $path] } {
        return ""
    } else {
        set vals ""
        foreach p [glob -nocomplain $path/*] {
            set v [rglob $p $pattern]
            if { $v != "" } {
                set vals [concat $vals $v]
            }
        }
        return $vals
    }
}

