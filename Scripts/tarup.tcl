
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

    foreach dirname "VTK_DIR VTK_SRC_DIR ITK_BINARY_PATH TCL_BIN_DIR GSL_LIB_DIR" {
       set dir $::env($dirname)
       eval { \
             if {[file exist $dir] == 0} { \
               puts "$dirname is set to $dir which does not exist"; \
               return
           } \ 
        }
    }

    set exe ""
    switch $::tcl_platform(os) {
        "SunOS" { set target solaris-sparc }
        "Darwin" { set target darwin-ppc }
        "Linux" { set target linux-x86 }
        default { set target win32 ; set exe .exe}
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
                    switch $::tcl_platform(os) {
                        "SunOS" { set destdir /tmp }
                        "Darwin" -  "Linux" { set destdir /var/tmp }
                        default { set destdir c:/Temp }
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
    file copy slicer_variables.tcl $destdir

    #
    # grab the tcl libraries and binaries
    # - take the entire bin and lib dirs, which leaves out demos and doc 
    #   if tcl came from the ActiveTcl distribution.
    # - this is big, but worth having so people can build better apps
    #   (this will include xml, widgets, table, soap, and many other handy things)
    #
    puts " -- copying tcl files"
    file mkdir $destdir/Lib/$::env(BUILD)/tcl-build
    file copy -force $::env(TCL_LIB_DIR) $destdir/Lib/$::env(BUILD)/tcl-build/lib
    file copy -force $::env(TCL_BIN_DIR) $destdir/Lib/$::env(BUILD)/tcl-build/bin

    puts " -- copying gsl files"
    file mkdir $destdir/Lib/$::env(BUILD)/gsl
    file copy -force $::env(GSL_LIB_DIR) $destdir/Lib/$::env(BUILD)/gsl/lib

    #
    # grab the vtk libraries and binaries
    # - bring in the tcl wrapping files and a specially modified
    #   version of pkgIndex.tcl that allows for relocatable packages
    #
    puts " -- copying vtk files"
    file mkdir $destdir/Lib/$::env(BUILD)/VTK/Wrapping/Tcl
    set vtkparts { vtk vtkbase vtkcommon vtkpatented vtkfiltering
            vtkrendering vtkgraphics vtkhybrid vtkimaging 
            vtkinteraction vtkio vtktesting }
    foreach vtkpart $vtkparts {
        file copy -force $::env(VTK_SRC_DIR)/Wrapping/Tcl/$vtkpart $destdir/Lib/$::env(BUILD)/VTK/Wrapping/Tcl
    }



    file mkdir $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl
    switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" - 
        "Darwin" {
            file copy -force $::env(VTK_DIR)/Wrapping/Tcl/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl
        }
        default { 
            file mkdir $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl/$::env(VTK_BUILD_TYPE)
            file copy -force $::env(VTK_DIR)/Wrapping/Tcl/$::env(VTK_BUILD_TYPE)/pkgIndex.tcl $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl/$::env(VTK_BUILD_TYPE)
        }
    }

    file mkdir $destdir/Lib/$::env(BUILD)/VTK-build/bin
    switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" { 
            set libs [glob $::env(VTK_DIR)/bin/*.so*]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/VTK-build/bin
                set ll [file tail $lib]
                exec strip $destdir/Lib/$::env(BUILD)/VTK-build/bin/$ll
            }
            file copy $::env(VTK_DIR)/bin/vtk $destdir/Lib/$::env(BUILD)/VTK-build/bin
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl/pkgIndex.tcl
        }
        "Darwin" {
            set libs [glob $::env(VTK_DIR)/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/VTK-build/bin
            }
            file copy $::env(VTK_DIR)/bin/vtk $destdir/Lib/$::env(BUILD)/VTK-build/bin
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl/pkgIndex.tcl
        }
        default { 
            file mkdir $destdir/Lib/$::env(BUILD)/VTK-build/bin/$::env(VTK_BUILD_TYPE)
            set libs [glob $::env(VTK_DIR)/bin/$::env(VTK_BUILD_TYPE)/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/VTK-build/bin/$::env(VTK_BUILD_TYPE)
            }
            file copy -force $::env(SLICER_HOME)/Scripts/slicer-vtk-pkgIndex.tcl $destdir/Lib/$::env(BUILD)/VTK-build/Wrapping/Tcl/$::env(VTK_BUILD_TYPE)/pkgIndex.tcl
        }
    }
    #
    # grab the shared libraries and put them in the vtk bin dir
    #
    puts " -- copying shared development libraries"
    set sharedLibDir $destdir/Lib/$::env(BUILD)/VTK-build/bin
    set checkForSymlinks 1
    switch $::tcl_platform(os) {
      "SunOS" {
          set sharedLibs [list libgcc_s.so.1 libstdc++.so.3]
          set sharedSearchPath [split $::env(LD_LIBRARY_PATH) ":"]
      }
      "Linux" {
#         set sharedLibs [list ld-2.2.5.so libpthread-0.9.so libstdc++-3-libc6.2-2-2.10.0.so]
          set sharedLibs [list libstdc++-libc6.2-2.so.3]
          set sharedSearchPath [split $::env(LD_LIBRARY_PATH) ":"]
      }
      "Darwin" {
          set sharedLibs [list ]
          set sharedSearchPath [split $::env(DYLD_LIBRARY_PATH) ":"]
      }
      default {
          set sharedLibs [list msvci70d.dll msvci70.dll msvcp70d.dll msvcp70.dll msvcr70d.dll msvcr70.dll]
          set sharedSearchPath [concat [split $::env(PATH) ";"] $::env(LD_LIBRARY_PATH)]
          set sharedLibDir $destdir/Lib/$::env(BUILD)/VTK-build/bin/$::env(VTK_BUILD_TYPE)
          set checkForSymlinks 0
      }
    }
    foreach slib $sharedLibs { 
        if {$::Module(verbose)} { puts "LIB $slib"  }
        # don't copy if it's already in the dest dir
        if {![file exists [file join $sharedLibDir $slib]]} {
            set slibFound 0
            foreach spath $sharedSearchPath { 
                if {$::Module(verbose) && !$slibFound} { puts "checking dir $spath" }
                if {!$slibFound && [file exists [file join $spath $slib]]} { 
                    if {$::Module(verbose)} { puts "found $slib in dir $spath, copying to $sharedLibDir" }
                    # check if it's a symlink (but not on windows)
                    if {$checkForSymlinks} {
                        set checkpath [file join $spath $slib]
                        while {[file type $checkpath] == "link"} {
                            # need to resolve it to a real file so that file copy will work
                            set sympath [file readlink $checkpath]
                            if {[file pathtype $sympath] == "relative"} {
                                # if the link is relative to the last path, take the dirname of the last one 
                                # and append the new path to it, then normalize it
                                set checkpath [file normalize [file join [file dirname $checkpath] $sympath]]
                            }
                        }
                        # the links may have changed the name of the library file 
                        # (ie adding minor version numbers onto the end)
                        # so copy the new file into the old file name
                        file copy $checkpath [file join $sharedLibDir $slib]
                        if {$::Module(verbose)} { puts "copied checkpath $checkpath to $sharedLibDir/$slib" }
                    } else {
                        # copy it into the shared vtk bin dir
                        file copy [file join $spath $slib]  $sharedLibDir
                    }
                    set slibFound 1
                }
            }
            if {!$slibFound} {
                puts "WARNING: $slib not found, tarup may be incomplete. Place it in one of these directories and rerun tarup: \n $sharedSearchPath"
                DevErrorWindow "WARNING: $slib not found, tarup may be incomplete. Place it in a directory in your search path and rerun tarup."
            }
        } else { 
            if {$::Module(verbose)} { puts "$slib is already in $sharedLibDir" } 
        }
    }

    #
    # grab the itk libraries 
    #
    puts " -- copying itk files"
    file mkdir $destdir/Lib/$::env(BUILD)/Insight-build/bin

    switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" { 
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/*.so]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/Insight-build/bin
                set ll [file tail $lib]
                exec strip $destdir/Lib/$::env(BUILD)/Insight-build/bin/$ll
            } 
        }
        "Darwin" {
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/*.dylib]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/Insight-build/bin
            }
        }
        default { 
            file mkdir $destdir/Lib/$::env(BUILD)/Insight-build/bin/$::env(VTK_BUILD_TYPE)
            set libs [glob -nocomplain $::env(ITK_BINARY_PATH)/bin/$::env(VTK_BUILD_TYPE)/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Lib/$::env(BUILD)/Insight-build/bin/$::env(VTK_BUILD_TYPE)
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
    switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" { 
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
        default { 
            file mkdir $destdir/Base/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)
            set libs [glob Base/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)/*.dll]
            foreach lib $libs {
                file copy $lib $destdir/Base/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)
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
        switch $::tcl_platform(os) {
            "SunOS" -
            "Linux" { 
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
            default { 
                file mkdir $moddest/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)
                set libs [glob -nocomplain $moddir/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)/*.dll]
                foreach lib $libs {
                    file copy $lib $moddest/builds/$::env(BUILD)/bin/$::env(VTK_BUILD_TYPE)
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
        switch $::tcl_platform(os) {
            "SunOS" {
                puts " -- making $archroot.tar.gz"
                #exec gtar cvfz $archroot.tar.gz $archroot
                exec tar cfE $archroot.tar $archroot
                exec gzip -f $archroot.tar
            }
            "Linux" -
            "Darwin" {
                puts " -- making $archroot.tar.gz"
                exec tar cfz $archroot.tar.gz $archroot
            }
            default { 
                puts " -- making $archroot.zip"
                exec zip -r $archroot.zip $archroot
            }
        }
    }

    if { $do_upload == "true" } {
    set scpdestination "$::env(USER)@slicerl.bwh.harvard.edu:/usr/local/apache2/htdocs/snapshots/slicer2.4"
        puts " -- upload to $scpdestination"
        switch $::tcl_platform(os) {
            "SunOS" -
            "Linux" - 
            "Darwin" {
                exec xterm -e scp $archroot.tar.gz $scpdestination
            }
            default { 
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

