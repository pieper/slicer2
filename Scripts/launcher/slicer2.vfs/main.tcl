
#
# slicer2.vfs/main.tcl
#
# This is the first entry point for slicer2.  This is run from a "starpack"
# which is STand Alone Runtime version of tcl.  The purpose is the this
# script to determine:
# - where the program is installed, so all data, scripts, and libs can be relative
# - what platform you're on so correct libs are loaded
# - what modules are present so they can be loaded
#   

package require Tk
wm withdraw .

#
# follow links to find "real" install dir
#
set cmdname [file dir [info script]]
while { [file type $cmdname] == "link" } {
    set cmdname [file readlink $cmdname]
}

set env(SLICER_HOME) [file dir $cmdname]

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
set env(VTK_SRC_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK
set env(VTK_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK-build
set env(TCL_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/ActiveTcl-8.4.1/bin
set env(TCL_LIB_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/ActiveTcl-8.4.1/bin


#
# set the base library paths for this build 
# 
switch $env(BUILD) {
    "solaris8" -
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

set env(TCLLIBPATH) "$env(VTK_SRC_DIR) $env(SLICER_HOME)/Base/Wrapping/Tcl/vtkSlicerBase $env(TCLLIBPATH)"

# Add the module bin directories to the load library path and the Wrapping/Tcl directories to the tcl library path
set modulePath ${env(SLICER_HOME)}/Modules
set modulePaths [glob ${modulePath}/vtk*]
foreach dir $modulePaths {
    # get the module name
    regexp "$modulePath/(\.\*)" $dir match moduleName
    # if it's not the custom one, append it to the path
    if {[string first Custom $moduleName] == -1} {
        puts "Adding module: ${moduleName}"
        switch $env(BUILD) {
            "solaris8" -
            "redhat7.3" {
                set env(LD_LIBRARY_PATH) $env(SLICER_HOME)/Modules/$moduleName/builds/$env(BUILD)/bin:$env(LD_LIBRARY_PATH)
                set env(TCLLIBPATH) "$env(SLICER_HOME)/Modules/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
            }
            "Win32VC7" {
                set env(Path) $env(SLICER_HOME)/Modules/$moduleName/builds/$env(BUILD)/bin/debug\;$env(Path)
                set env(TCLLIBPATH) "$env(SLICER_HOME)/Modules/$moduleName/Wrapping/Tcl $env(TCLLIBPATH)"
            }
        }
    }
}


set msg "Slicer is an experimental software package.
Any human use requires proper research controls.  
Clicking \"Ok\" below binds you to the license agreement.
See www.slicer.org for details.
"
set resp [tk_messageBox -message $msg -type okcancel -title "Slicer2"]
set resp okay

if {$resp == "cancel"} {
    exit
}

switch $env(BUILD) {
    "solaris8" -
    "redhat7.3" {
        # need to run the specially modified tcl interp in the executable 'vtk'
        # on unix
        eval exec $env(VTK_BIN_DIR)/bin/vtk $env(SLICER_HOME)/Base/tcl/Go.tcl $argv &
    }
    "Win32VC7" {
        eval exec $env(TCL_BIN_DIR)/wish84.exe $env(SLICER_HOME)/Base/tcl/Go.tcl $argv &
    }
}

exit


