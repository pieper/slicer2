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
#               /ActiveTcl-8.4.1 - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#           /redhat7.3
#               /ActiveTcl-8.4.1 - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#           /Win32VC7
#               /ActiveTcl-8.4.1 - all tcl libs
#               /vtk
#                   /VTK - vtk source (or just Wrapping for distribution)
#                   /VTK-build - vtk build tree (or just stripped libs in bin for distribution)
#


#
# add the necessary library directories, both Base and Modules, to the 
# LD_LIBRARY_PATH environment variable
#

# check the build os: SunOS, Linux, Windows NT are possible responses
switch $tcl_platform(os) {
    "SunOS" {
        set env(BUILD) solaris8
    }
    "Linux" {
        set env(BUILD) redhat7.3
    }
    "Darwin" {
        set env(BUILD) Darwin
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)
        set env(BUILD) Win32VC7
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
 
#
# VTK source and binary dirs and tcl dirs should be in the Lib directory
#
if { ![info exists env(VTK_SRC_DIR)] || $env(VTK_SRC_DIR) == "" } {
    set env(VTK_SRC_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK
}
if { ![info exists env(VTK_BIN_DIR)] || $env(VTK_BIN_DIR) == "" } {
    set env(VTK_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK-build
}
if { ![info exists env(TCL_BIN_DIR)] || $env(TCL_BIN_DIR) == "" } {
    set env(TCL_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/ActiveTcl-8.4.1/bin
}
if { ![info exists env(TCL_LIB_DIR)] || $env(TCL_LIB_DIR) == "" } {
    set env(TCL_LIB_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/ActiveTcl-8.4.1/lib
}


#
# set the base library paths for this build 
# 
switch $env(BUILD) {
    "solaris8" -
    "Darwin" -
    "redhat7.3" {
        # add vtk bins
        set env(LD_LIBRARY_PATH) $env(VTK_BIN_DIR)/bin:$env(LD_LIBRARY_PATH)
        # add slicer bins
        set env(LD_LIBRARY_PATH) $env(SLICER_HOME)/Base/builds/$env(BUILD)/bin:$env(LD_LIBRARY_PATH)
        # add tcl bins
        set env(LD_LIBRARY_PATH) $env(TCL_LIB_DIR):$env(LD_LIBRARY_PATH)
    }
    "Win32VC7" {
        # add vtk bins
        set env(Path) $env(VTK_BIN_DIR)/bin/debug\;$env(Path)
        # add slicer bins
        set env(Path) $env(SLICER_HOME)/Base/builds/$env(BUILD)/bin/debug\;$env(Path)
        # add tcl bins
        set env(Path) $env(TCL_BIN_DIR)\;$env(Path)
    }
}

# set the base tcl/tk library paths, looking in the slicer home Lib 
# subdirectory for this build. 
set env(TCL_LIBRARY) $env(SLICER_HOME)/Lib/$env(BUILD)/ActiveTcl-8.4.1/lib/tcl8.4
set env(TK_LIBRARY) $env(SLICER_HOME)/Lib/$env(BUILD)/ActiveTcl-8.4.1/lib/tk8.4

# add the default search locations for tcl packages
set env(TCLLIBPATH) "$env(SLICER_HOME)/Base/Wrapping/Tcl/vtkSlicerBase $env(TCLLIBPATH)"
set env(TCLLIBPATH) "$env(VTK_SRC_DIR)/Wrapping/Tcl $env(TCLLIBPATH)"

#
# Add the module bin directories to the load library path 
# and the Wrapping/Tcl directories to the tcl library path
# check both the base slicer home and the user's home dir
#

set baseModulePath ${env(SLICER_HOME)}/Modules
regsub -all {\\} $env(HOME) / home
set userModulePath $home/Modules
foreach modulePath "${baseModulePath} ${userModulePath}" {
    set modulePaths [glob -nocomplain ${modulePath}/vtk*]
    foreach dir $modulePaths {
        # get the module name
        regexp "$modulePath/(\.\*)" $dir match moduleName
        # if it's not the custom one, append it to the path
        if {[string first Custom $moduleName] == -1} {
            puts "Adding module to library paths: ${moduleName}"
            switch $env(BUILD) {
                "solaris8" -
                "Darwin" -
                "redhat7.3" {
                    set env(LD_LIBRARY_PATH) ${modulePath}/$moduleName/builds/$env(BUILD)/bin:$env(LD_LIBRARY_PATH)
                    set env(TCLLIBPATH) "${modulePath}/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
                }
                "Win32VC7" {
                    set env(Path) $modulePath/$moduleName/builds/$env(BUILD)/bin/debug\;$env(Path)
                    set env(TCLLIBPATH) "$modulePath/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
                }
            }
        }
    }
}

set msg "Slicer is an experimental software package.
Any human use requires proper research controls.  
Clicking \"Ok\" below binds you to the license agreement.
See www.slicer.org for details.
"
set resp okay
#set resp [tk_messageBox -message $msg -type okcancel -title "Slicer2"]

if {$resp == "cancel"} {
    exit
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

switch $env(BUILD) {
    "solaris8" -
    "Darwin" -
    "redhat7.3" {
        # - need to run the specially modified tcl interp in the executable 'vtk' on unix
        # - don't put process in background so that jdemo can track its status
        set fp [open "| csh -c \"$env(VTK_BIN_DIR)/bin/vtk $mainscript $argv \" |& cat" r]
    }
    "Win32VC7" {
        # put slicer in the background on windows so it won't be "Not Responding" in
        # task manager
        set fp [open "| $env(TCL_BIN_DIR)/wish84.exe $mainscript $argv" r]
    }
}

fileevent $fp readable "file_event $fp"

set END 0
while { ![catch "pid $fp"] && ![eof $fp] } {
    vwait END
}

exit


