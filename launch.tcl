#
# launch.tcl
#
# This script is called by the platform-specific executables at the top level
# of the slicer tree (e.g. slicer2-win32.exe).  See Scripts/launcher for details
# on how and why the platform-specific executables are built.
#
# - what platform you're on so correct libs are loaded
# - what modules are present so they can be loaded
#   
# This is a tcl script rather than a .bat or .sh file so it can handle all 
# the details in one place.
#
# Depending on your local libary configuration, you may need to edit this file.
#
# The default assumption (e.g. for distribution of slicer2) is that the 
# directory tree will be set up as follows (not all detail shown):
#
#  /$env(SLICER_HOME)
#       /Base - slicer base code and builds
#       /Modules - slicer modules code and builds
#       /Lib - has all support Libs for all platforms
#           /solaris8
#               /tcl - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#               /itk
#                   /ITK-build - itk in bin for distribution
#           /redhat7.3
#               /tcl - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#               /itk
#                   /ITK-build - itk in bin for distribution
#           /Win32VC7
#               /tcl - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin/debug for distribution)
#               /itk
#                   /ITK-build - itk in bin for distribution
#           /Darwin
#               /tcl - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#               /itk
#                   /ITK-build - itk in bin for distribution
#


#
# add the necessary library directories, both Base and Modules, to the 
# LD_LIBRARY_PATH environment variable
#

# set up variables for the OS Builds, to facilitate the move to solaris9
# can be solaris8 or solaris9
set solaris "solaris8"
set linux "redhat7.3"
set darwin "Darwin"
set windows "Win32VC7"

# check the build os: SunOS, Linux, Windows NT are possible responses
switch $tcl_platform(os) {
    "SunOS" {
        set env(BUILD) $solaris
    }
    "Linux" {
        set env(BUILD) $linux
    }
    "Darwin" {
        set env(BUILD) $darwin
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)
        set env(BUILD) $windows
        # take out any spaces in the slicer home dir
        if {[regexp { } $env(SLICER_HOME) match] != 0} {
            # set it to the short name
            set env(SLICER_HOME) [file attributes $env(SLICER_HOME) -shortname]
            puts "Set SLICER_HOME environment variable to shortname: $env(SLICER_HOME)"
        }
    }
}
puts "\nSlicer build directory set to $env(BUILD)"

# if it is an empty string or doesn't exist, set the LD_LIBRARY_PATH 
if {[catch {
    if {$env(LD_LIBRARY_PATH) == ""} { 
        set env(LD_LIBRARY_PATH) " " 
    }} ex]} {
    set env(LD_LIBRARY_PATH) " "
}

# if it is an empty string or doesn't exist, set the TCLLIBPATH 
if {[catch {
    if {$env(TCLLIBPATH) == ""} { 
        set env(TCLLIBPATH) " " 
    }} ex]} {
    set env(TCLLIBPATH) " " 
}

# if it is an empty string or doesn't exist, set the LD_LIBRARY_PATH 
if { $env(BUILD) == $darwin && [catch {
    if {$env(DYLD_LIBRARY_PATH) == ""} { 
        set env(DYLD_LIBRARY_PATH) " " 
    }} ex]} {
    set env(DYLD_LIBRARY_PATH) " "
}
 
#
# VTK source and binary dirs and tcl dirs should be in the Lib directory
#
if { ![info exists env(VTK_SRC_DIR)] || $env(VTK_SRC_DIR) == "" } {
    set env(VTK_SRC_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK
}
if { ![info exists env(VTK_BIN_DIR)] || $env(VTK_BIN_DIR) == "" } {
    set env(VTK_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK-build
}
if { ![info exists env(ITK_BIN_DIR)] || $env(ITK_BIN_DIR) == "" } {
    set env(ITK_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/itk/ITK-build
}
if { ![info exists env(TCL_BIN_DIR)] || $env(TCL_BIN_DIR) == "" } {
    set env(TCL_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/tcl/bin
}
if { ![info exists env(TCL_LIB_DIR)] || $env(TCL_LIB_DIR) == "" } {
    set env(TCL_LIB_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/tcl/lib
}


#
# set the base library paths for this build 
# 
switch $env(BUILD) {
    $solaris -
    $linux {
        # add vtk, slicer, and tcl bins
        set env(LD_LIBRARY_PATH) $env(VTK_BIN_DIR)/bin:$env(LD_LIBRARY_PATH)
        set env(LD_LIBRARY_PATH) $env(ITK_BIN_DIR)/bin:$env(LD_LIBRARY_PATH)
        set env(LD_LIBRARY_PATH) $env(SLICER_HOME)/Base/builds/$env(BUILD)/bin:$env(LD_LIBRARY_PATH)
        set env(LD_LIBRARY_PATH) $env(TCL_LIB_DIR):$env(LD_LIBRARY_PATH)
    }
    $darwin {
        # add vtk, slicer, and tcl bins
        set env(DYLD_LIBRARY_PATH) $env(VTK_BIN_DIR)/bin:$env(DYLD_LIBRARY_PATH)
        set env(DYLD_LIBRARY_PATH) $env(ITK_BIN_DIR)/bin:$env(DYLD_LIBRARY_PATH)
        set env(DYLD_LIBRARY_PATH) $env(SLICER_HOME)/Base/builds/$env(BUILD)/bin:$env(DYLD_LIBRARY_PATH)
        set env(DYLD_LIBRARY_PATH) $env(TCL_LIB_DIR):$env(DYLD_LIBRARY_PATH)
    }
    $windows {
        # add vtk, slicer, and tcl bins
        set env(Path) $env(VTK_BIN_DIR)/bin/debug\;$env(Path)
        set env(Path) $env(ITK_BIN_DIR)/bin/debug\;$env(Path)
        set env(Path) $env(SLICER_HOME)/Base/builds/$env(BUILD)/bin/debug\;$env(Path)
        set env(Path) $env(TCL_BIN_DIR)\;$env(Path)
    }
}

# set the base tcl/tk library paths, using the previously defined TCL_LIB_DIR
set env(TCL_LIBRARY) $env(TCL_LIB_DIR)/tcl8.4
set env(TK_LIBRARY) $env(TCL_LIB_DIR)/tk8.4

#
# add the default search locations for tcl packages
#  (window has special tcl packages depending on build type)
#
switch $env(BUILD) {
    $solaris -
    $linux -
    $darwin {
        set env(TCLLIBPATH) "$env(VTK_BIN_DIR)/Wrapping/Tcl $env(TCLLIBPATH)"
    }
    $windows {
        set env(TCLLIBPATH) "$env(VTK_BIN_DIR)/Wrapping/Tcl/Debug $env(TCLLIBPATH)"
    }
}
# same for all platforms
set env(TCLLIBPATH) "$env(SLICER_HOME)/Base/Wrapping/Tcl/vtkSlicerBase $env(TCLLIBPATH)"


#
# Add the module bin directories to the load library path 
# and the Wrapping/Tcl directories to the tcl library path
# check :
# - Bbase in slicer home 
# - the user's home dir Modules directory
# - dirs listed in the SLICER_MODULES env variable
#
regsub -all {\\} $env(SLICER_HOME) / slicer_home
regsub -all {\\} $env(HOME) / home
set modulePaths $slicer_home/Modules
lappend modulePaths $home/Modules
if { [info exists env(SLICER_MODULES)] } {
    foreach mpath $env(SLICER_MODULES) {
        lappend modulePaths [string trimright $mpath "/"]
    }
}

set env(SLICER_MODULES_TO_REQUIRE) " "
foreach modulePath $modulePaths {
    set modulePath [string trimright $modulePath "/"] ;# remove trailing slash
    set modules [glob -nocomplain $modulePath/*]
    foreach dir $modules {
        if { ![file exists $dir/Wrapping/Tcl] } {
            continue ;# without this dir then it's not one we want
        }
        # get the module name
        regexp "$modulePath/(\.\*)" $dir match moduleName
        # if it's not the custom one, append it to the path
        if {[string first Custom $moduleName] == -1} {
            lappend env(SLICER_MODULES_TO_REQUIRE) $moduleName
            switch $env(BUILD) {
                $solaris -
                $linux {
                    set env(LD_LIBRARY_PATH) ${modulePath}/$moduleName/builds/$env(BUILD)/bin:$env(LD_LIBRARY_PATH)
                    set env(TCLLIBPATH) "${modulePath}/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
                }
                $darwin {
                    set env(DYLD_LIBRARY_PATH) ${modulePath}/$moduleName/builds/$env(BUILD)/bin:$env(DYLD_LIBRARY_PATH)
                    set env(TCLLIBPATH) "${modulePath}/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
                }
                $windows {
                    set env(Path) $modulePath/$moduleName/builds/$env(BUILD)/bin/debug\;$env(Path)
                    set env(TCLLIBPATH) "$modulePath/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
                }
            }
        }
    }
}


if { $env(BUILD) == $darwin } {
    # vtk uses the LD_ version to do it's own search for what to load
    # so need to set this even though MAC OSX uses the DYLD_ version
    set env(LD_LIBRARY_PATH) $env(DYLD_LIBRARY_PATH)
}


set msg "Slicer is an experimental software package.
Any clinical use requires proper research controls.  
Clicking \"Ok\" below binds you to the license agreement.
See www.slicer.org for details.
"
set argv0 [lindex $argv 0]
if { $argv == "-y" || $argv0 == "--agree_to_license" } {
    set argv [lreplace $argv 0 0]
} else {
    if { ![file exists $env(HOME)/.IAgreeToSlicersLicense] } {
        set resp [tk_messageBox -message $msg -type okcancel -title "Slicer2"]

        if {$resp == "cancel"} {
            exit
        }
    }
}


# 
# if a tcl script is the first argument on the command line, run it
# otherwise, run the default application startup script
#
set argv0 [lindex $argv 0]
if { [string match *.tcl $argv0] } {
    set mainscript $argv0
    set argv [lreplace $argv 0 0] ;# trim the script name off 
} else {
    set mainscript $env(SLICER_HOME)/Base/tcl/Go.tcl
}


# 
# launch the slicer main script as a sub-process and monitor the output
# - process is started with the 'open' command through a pipe
# - the file_event callback is triggered when subprocess has written to stdout
# - pipe is closed when process exits, causing launcher to exit too.
#
# why? well, some messages only go to console stdout and we don't want to lose them.
# Also, on windows if you block waiting for the process to exit the launcher would
# be "Not Responding" and things like screen savers and power management might not 
# work right.
#

proc file_event {fp} {
    global END
    if {[eof $fp]} {
        catch "close $fp"
        set END 1
    } else {
        gets $fp line
        puts $line
    }
}

# change from tcl escape to shell escape for command line arguments 
# that contain spaces -- note that shell notation only works for a single
# level of nesting
# - change backslashes to forward slashes (for windows paths)
# - escape the spaces in each of the arguments
# - then remove the curly braces

regsub -all "\\\\" $argv "/" argv

set newargv ""
foreach a $argv {
    regsub -all " " $a "\\\ " a
    lappend newargv $a
}
set argv $newargv


switch $env(BUILD) {
    $solaris -
    $darwin -
    $linux {
        # - need to run the specially modified tcl interp in the executable 'vtk' on unix
        # - don't put process in background so that jdemo can track its status
        regsub -all "{|}" $argv "\\\"" argv
        set fp [open "| csh -c \"$env(VTK_BIN_DIR)/bin/vtk $mainscript $argv \" |& cat" r]
    }
    $windows {
        # put slicer in the background on windows so it won't be "Not Responding" in
        # task manager
        regsub -all "{|}" $argv "" argv
        set fp [open "| $env(TCL_BIN_DIR)/wish84.exe $mainscript $argv" r]
    }
    default {
        puts stderr "Unknown build: $env(BUILD)"
        exit
    }
}

fileevent $fp readable "file_event $fp"

set END 0
while { ![catch "pid $fp"] && ![eof $fp] } {
    vwait END
}

exit


