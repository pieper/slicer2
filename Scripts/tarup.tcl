
set __comment__ {

    tarup.tcl
    sp - 2003-05

    source this file into a running slicer2.6 to make a distribution 
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
        ld-2.2.5.so libpthread-0.9.so libstdc++-3-libc6.2-2-2.10.0.so libstdc++.so.5  
    for solaris
        libgcc_s.so.1 libstdc++.so.3  
    for darwin
        nothing known
}

# returns a list of full paths to libraries that match the input list and that the vtk binary dynamically links to.
# doesn't work with ++ in the toMatch string
# on error, return an empty list
proc GetLinkedLibs { {toMatch {}} } {
    set liblist ""
    if {$toMatch == {}} {
        set toMatch [list "libgcc" "libstdc"]
    }
    switch $::tcl_platform(os) {
        "SunOS" -
        "Linux" {
            if {[catch {set lddpath [exec which ldd]} errMsg] == 1} {
                puts "Using which to find ldd is not working: $errMsg"
                return ""
            }
            # check if it says no first
            if {[regexp "^no ldd" $lddpath matchVar] == 1} {
                puts "No ldd in the path: $lddpath"
                return ""
            }
            set lddresults [exec $lddpath $::env(VTK_DIR)/bin/vtk]
        }
        "Darwin" {
            if {[catch {set lddpath [exec which otools]} errMsg] == 1} {
                puts "Using which to find otools is not working: $errMsg"
                return ""
            }
            # did it find it?
            if {[regexp "^no otools" $lddpath matchVar] == 1} {
                puts "No otools in the path: $lddpath"
                return ""
            }
            set lddpath "$lddpath -L"
            set lddresults [exec $lddpath $::env(VTK_DIR)/bin/vtk]
        }
        default { 
            puts "Unable to get libraries"
            return ""
        }
    }
    set lddlines [split $lddresults "\n"]
    foreach l $lddlines {
        # just grab anyones that match the input strings
        foreach strToMatch $toMatch {
            if {[regexp $strToMatch $l matchVar] == 1} {
                # puts "working on $l"
                foreach lddtoken [split $l] {
                    if {[file pathtype $lddtoken] == "absolute"} {
                        lappend liblist $lddtoken
                        if {$::Module(verbose)} {
                            puts "Found [lindex $liblist end]"
                        }
                        break
                    }
                }
            } else { 
                # puts "skipping $l" 
            }
        }
    }
    if {$::Module(verbose)} {
        puts "Got library list $liblist"
    }
    return $liblist
}

proc tarup_usage {} {
    puts "Call 'tarup' to create a binary archive. Optional arguments:"
    puts "\tdestdir\n\t\tauto, to upload to slicerl (default)\n\t\tbirn, to copy to /usr/local/birn/install/slicer2\n\t\tlocal, to make a local copy."
    puts "\tincludeSource\n\t\t0, to make a binary release (default)\n\t\t1, to include the cxx directories"
    puts "Example: tarup local 0"
}

proc tarup { {destdir "auto"} {includeSource 0} } {

    set cwd [pwd]
    cd $::env(SLICER_HOME)

    # need to figure out which version of windows visual studio was used to build
    # so we need the original variables
    source $::env(SLICER_HOME)/slicer_variables.tcl

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
    switch $::env(BUILD) {
        "solaris8" { set target solaris-sparc }
        "darwin-ppc" { set target darwin-ppc }
        "redhat7.3" -
        "linux-x86" { set target linux-x86 }
        "win32" { set target win32 ; set exe .exe}
        default {error "unknown build target $::env(BUILD)"}
    }

    set create_archive "true"
    if {$destdir == "local"} {
        set do_upload "false"
    } else {
        set do_upload "true"
    }
    switch $destdir {
        "auto" -
        "local" {
            if { [info exists ::env(TMPDIR)] } {
                set destdir [file normalize $::env(TMPDIR)]
            } else {
                if { [info exists ::env(TMP)] } {
                    set destdir [file normalize $::env(TMP)]
                } else {
                    switch $::env(BUILD) {
                        "solaris8" { set destdir /tmp }
                        "Darwin" - "darwin-ppc" - "linux-x86" - "redhat7.3" { set destdir /var/tmp }
                        "win32" { set destdir c:/Temp }
                    }
                }
            }
            set date [clock format [clock seconds] -format %Y-%m-%d]
            if { $::tcl_platform(machine) == "x86_64" } {
                set destdir $destdir/slicer$::SLICER(version)-${target}_64-$date
            } else {
                set destdir $destdir/slicer$::SLICER(version)-$target-$date
            }
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
          set sharedLibs [GetLinkedLibs [list libgcc libstd]]
          if {$sharedLibs == ""} {
              set sharedLibs [list libgcc_s.so.1 libstdc++.so.3]
              set sharedSearchPath [split $::env(LD_LIBRARY_PATH) ":"]
          } else {
              set sharedSearchPath ""
              if {$::Module(verbose)} { puts "GetLinkedLibs returned $sharedLibs" }
          }
      }
      "Linux" {
#         set sharedLibs [list ld-2.2.5.so libpthread-0.9.so libstdc++-3-libc6.2-2-2.10.0.so]
          set sharedLibs [GetLinkedLibs [list libstdc]]
          if {$sharedLibs == ""} {
              set sharedLibs [list libstdc++-libc6.2-2.so.3 libstdc++.so.5]
              set sharedSearchPath [split $::env(LD_LIBRARY_PATH) ":"]
          } else {
              set sharedSearchPath ""
          }
      }
      "Darwin" {
          set sharedLibs [list ]
          set sharedSearchPath [split $::env(DYLD_LIBRARY_PATH) ":"]
      }
      default {
          switch $::GENERATOR {
              "Visual Studio 7" {
                  set sharedLibs [list msvci70d.dll msvci70.dll msvcp70d.dll msvcp70.dll msvcr70d.dll msvcr70.dll]
              }
              "Visual Studio 7 .NET 2003" {
                  set sharedLibs [list msvcp71d.dll msvcp71.dll msvcr71d.dll msvcr71.dll]
              }
              default {
                  error "unknown build system for tarup: $GENERATOR"
              }
        
          }
          set sharedSearchPath [concat [split $::env(PATH) ";"] $::env(LD_LIBRARY_PATH)]
          set sharedLibDir $destdir/Lib/$::env(BUILD)/VTK-build/bin/$::env(VTK_BUILD_TYPE)
          set checkForSymlinks 0
      }
    }
    set foundLibs ""
    foreach slib $sharedLibs { 
        if {$::Module(verbose)} { puts "LIB $slib"  }
        # don't copy if it's already in the dest dir (take the tail of the full path to slib)
        if {![file exists [file join $sharedLibDir [file tail $slib]]]} {
            set slibFound 0
            if {$sharedSearchPath == ""} {
                if {$::Module(verbose)} { puts "Should have fully qualified path from GetLinkedLibs for $slib" }
                lappend foundLibs $slib
                set slibFound 1
            }
            foreach spath $sharedSearchPath { 
                if {$::Module(verbose) && !$slibFound} { puts "checking dir $spath" }
                if {!$slibFound && [file exists [file join $spath $slib]]} { 
                    if {$::Module(verbose)} { puts "found $slib in dir $spath, copying to $sharedLibDir" }
                    lappend foundLibs [file join $spath $slib]
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
    foreach slib $foundLibs {
        # check if it's a symlink (but not on windows)
        if {$checkForSymlinks} {
            # at this point each one is a fully qualified path
            set checkpath $slib
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
            file copy $checkpath [file join $sharedLibDir [file tail $slib]]
            puts "\tCopied $checkpath to $sharedLibDir/[file tail $slib]" 
        } else {
            # copy it into the shared vtk bin dir
            file copy $slib  $sharedLibDir
            puts "\tCopied $slib to $sharedLibDir"
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
    if {$includeSource} {
        file copy -force Base/cxx $destdir/Base
    }
    # get the servers directory
    file copy -force servers $destdir
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
        if {$includeSource} {
            if { [file exists $moddir/cxx] } {
                file copy -force $moddir/cxx $moddest
            }
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
                puts " -- making $archroot.tar.gz from $destdir"
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
    set scpdestination "$::env(USER)@slicerl.bwh.harvard.edu:/usr/local/apache2/htdocs/snapshots/slicer2.6"
        puts " -- upload to $scpdestination"
        switch $::tcl_platform(os) {
            "SunOS" -
            "Linux" - 
            "Darwin" {
                exec xterm -e scp $archroot.tar.gz $scpdestination
            }
            default { 
                puts "rxvt -e scp $archroot.zip $scpdestination &"
                exec rxvt -e scp $archroot.zip $scpdestination &
            }
        }
    } else {
        switch $::tcl_platform(os) {
            "SunOS" -
            "Linux" - 
            "Darwin" {
                puts "Archive complete: [file dirname $destdir]/$archroot.tar.gz"
            }
            default { 
                puts "Archive complete: [file dirname $destdir]/$archroot.zip"
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

tarup_usage
