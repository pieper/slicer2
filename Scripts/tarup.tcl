
set __comment__ {

    tarup.tcl
    sp - 2003-05

    source this file into a running slicer2.1 to make a distribution 
    copy of slicer in a new directory based on the currently loaded Modules.
    The resulting program will be self-contained, without intermediate build
    files, CVS directories, and stripped of debugging symbols if possible.

    the tarup proc does the following steps:
    - makes the destination directory
    - copies the toplevel launcher and script
    - copies the tcl libs and binaries
    - copies the vtk libs and wrapping files
    - copies the itk libs
    - copies the slicerbase libs and wrapping files
    - copies each of the modules libs and wrapping files
    - removes CVS dirs that have been copied by accident
    - makes a .tar.gz or a .zip of the resulting distribution
    - if SLICER(state) is -dev, uploads to snapshots web site for testing
       else uploads to birn rack for wider distribution

    It does all this while taking into account platform differences in naming schemes
    and directory layouts.
    

    NB: remember to check that all shared libraries are included in the appropriate
    bin directory.  These should be placed in the VTK_DIR
    for windows visual studio 7, the following files are needed for a debug build:
        msvci70d.dll msvcp70d.dll msvcr70d.dll
    for linux redhat7.3
        ld-2.2.5.so libpthread-0.9.so libstdc++-3-libc6.2-2-2.10.0.so  
    for solaris
        libgcc_s.so.1 libstdc++.so.3  
    for darwin
        nothing known
}

proc tarup { {destdir "auto"} } {

    set cwd [pwd]
    cd $::env(SLICER_HOME)

    ### Some simple error checking to see if the directories exist

    foreach dirname "VTK_DIR VTK_SRC_DIR ITK_BINARY_PATH TCL_BIN_DIR " {
       set dir $::env($dirname)
       eval { \
             if {[file exist $dir] == 0} { \
               puts "$dirname is set to $dir which does not exist"; \
               return
           } \ 
        }
    }

    set exe ""
    switch $::env(BUILD) {
        "solaris8" { set target solaris-sparc }
        "darwin-ppc" { set target darwin-ppc }
        "redhat7.3" -
        "linux-x86" { set target linux-x86 }
        "Win32VC7" { set target win32 ; set exe .exe}
    }

    set create_archive "true"
    set do_upload "true"
    switch $destdir {
        "auto" {
            if { [info exists ::env(TMPDIR)] } {
                set destdir [file normalize $::env(TMPDIR)]
            } else {
                if { [info exists ::env(TMP)] } {
                    set destdir [file normalize $::env(TMP)]
                } else {
                    switch $::env(BUILD) {
                        "solaris8" { set destdir /tmp }
                        "darwin-ppc" - "redhat7.3" { set destdir /var/tmp }
                        "Win32VC7" { set destdir c:/Temp }
                    }
                }
            }
            set date [clock format [clock seconds] -format %Y-%m-%d]
            set destdir $destdir/slicer$::SLICER(version)-$target-$date
        }
        "birn" {
            set destdir /usr/local/birn/install/slicer2
            set create_archive "false"
            set do_upload "false"
        }
    }

    puts "Creating distribution in $destdir..."

    if { [file exists $destdir] } {
        set resp [tk_messageBox -message "$destdir exists\nDelete it?" -type okcancel]
        if { $resp == "cancel" } {return}
        file delete -force $destdir
    }

    file mkdir $destdir

    if { ![file writable $destdir] } {
        error "can't write to $destdir"
    }

    #
    # grab the top-level files - the launch executable and script
    #
    puts " -- copying launcher files"
    file copy slicer2-$target$exe $destdir
    file copy launch.tcl $destdir

    #
    # grab the tcl libraries and binaries
    # - take the entire bin and lib dirs, which leaves out demos and doc 
    #   if tcl came from the ActiveTcl distribution.
    # - this is big, but worth having so people can build better apps
    #   (this will include xml, widgets, table, soap, and many other handy things)
    #
    puts " -- copying tcl files"
    file mkdir $destdir/Lib/$::env(BUILD)/tcl
    file copy -force $::env(TCL_LIB_DIR) $destdir/Lib/$::env(BUILD)/tcl/lib
    file copy -force $::env(TCL_BIN_DIR) $destdir/Lib/$::env(BUILD)/tcl/bin


    #
    # grab the vtk libraries and binaries
    # - bring in the tcl wrapping files and a specially modified
    #   version of pkgIndex.tcl that allows for relocatable packages
    #
    puts " -- copying vtk files"
    file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK/Wrapping/Tcl
    set vtkparts { vtk vtkbase vtkcommon vtkpatented vtkfiltering
            vtkrendering vtkgraphics vtkhybrid vtkimaging 
            vtkinteraction vtkio vtktesting }
    foreach vtkpart $vtkparts {
        file copy -force $::env(VTK_SRC_DIR)/Wrapping/Tcl/$vtkpart $destdir/Lib/$::env(BUILD)/vtk/VTK/Wrapping/Tcl
    }



    file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" - 
        "darwin-ppc" {
            file copy -force $::env(VTK_DIR)/Wrapping/Tcl/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl
        }
        "Win32VC7" { 
            file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/Debug
            file copy -force $::env(VTK_DIR)/Wrapping/Tcl/Debug/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/Debug
        }
    }

    file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" { 
            set libs [glob $::env(VTK_DIR)/bin/*.so*]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
                set ll [file tail $lib]
                exec strip $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/$ll
            }
            file copy $::env(VTK_DIR)/bin/vtk $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/pkgIndex.tcl
        }
        "darwin-ppc" {
            set libs [glob $::env(VTK_DIR)/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
            }
            file copy $::env(VTK_DIR)/bin/vtk $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/pkgIndex.tcl
        }
        "Win32VC7" { 
            file mkdir $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/debug
            set libs [glob $::env(VTK_DIR)/bin/debug/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/vtk/VTK-build/bin/debug
            }
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/vtk/VTK-build/Wrapping/Tcl/debug/pkgIndex.tcl
        }
    }

    #
    # grab the itk libraries 
    #
    puts " -- copying itk files"
    file mkdir $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin

    switch $::env(BUILD) {
        "solaris8" -
        "redhat7.3" { 
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/*.so]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin
                set ll [file tail $lib]
                exec strip $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin/$ll
            }
        }
        "darwin-ppc" {
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin
            }
        }
        "Win32VC7" { 
            file mkdir $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin/debug
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/debug/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/itk/ITK-build/bin/debug
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
        "darwin-ppc" {
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
                if { [file exists $m/$mod] } {
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
            "darwin-ppc" {
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
        if { [file exists $moddir/data] } {
            file copy -force $moddir/data $moddest
        }

        # TODO: these as special cases for the birn Query Atlas and 
        # should be generalized (maybe a manifest file or something)
        if { [file exists $moddir/java] } {
            file copy -force $moddir/java $moddest
        }
        if { [file exists $moddir/talairach] } {
            file copy -force $moddir/talairach $moddest
        }
    }

    #
    # remove any stray CVS dirs in target
    #

    foreach cvsdir [rglob $destdir CVS] {
        file delete -force $cvsdir
    }

    #
    # make an archive of the new directory at the same level
    # with the destination
    #
    if { $create_archive == "true" } {
        cd $destdir/..
        set archroot [file tail $destdir]
        switch $::env(BUILD) {
            "solaris8" {
                puts " -- making $archroot.tar.gz"
                #exec gtar cvfz $archroot.tar.gz $archroot
                exec tar cfE $archroot.tar $archroot
                exec gzip -f $archroot.tar
            }
            "redhat7.3" - 
            "darwin-ppc" {
                puts " -- making $archroot.tar.gz"
                exec tar cfz $archroot.tar.gz $archroot
            }
            "Win32VC7" { 
                puts " -- making $archroot.zip"
                exec zip -r $archroot.zip $archroot
            }
        }
    }

    if { $do_upload == "true" } {
        if { $::SLICER(state) == "-dev" } {
            set scpdestination "pieper@slicerl.bwh.harvard.edu:/usr/local/apache2/htdocs/snapshots"
        } else {
            set scpdestination "pieper@gpop.bwh.harvard.edu:slicer-dist"
        }

        puts " -- upload to $scpdestination"
        switch $::env(BUILD) {
            "solaris8" -
            "redhat7.3" - 
            "darwin-ppc" {
                exec xterm -e scp $archroot.tar.gz $scpdestination
            }
            "Win32VC7" { 
                exec rxvt -e scp $archroot.zip $scpdestination &
            }
        }
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

