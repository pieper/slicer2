
package require Tk
wm withdraw .

set env(SLICER_HOME) [file dir [file dir [info script]]]

# add the necessary library directories, both Base and Modules, to the 
# LD_LIBRARY_PATH environment variable

# check the build os: SunOS, Linux, Windows NT are possible responses
set osName $tcl_platform(os)
if {[string equal $osName SunOS]} {
   set env(BUILD) solaris8
} else {
   set env(BUILD) redhat-7.3
}
puts "\nSlicer build directory set to $env(BUILD)"

set env(VTK_SRC_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK
# set env(VTK_BIN_DIR) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/vtk/VTK-build
set env(VTK_BIN_DIR) /local/os/src/vtk-4.0 

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
 
# set the base library paths for this build
set env(LD_LIBRARY_PATH) ${env(VTK_BIN_DIR)}/bin:${env(SLICER_HOME)}/Base/builds/${env(BUILD)}/bin:${env(SLICER_HOME)}/Lib/${env(BUILD)}/tcl:${env(LD_LIBRARY_PATH)}

# set the base tcl/tk library paths
# set tclversion [glob ${env(SLICER_HOME)}/Lib/${env(BUILD)}/tcl/tcl*]
set env(TCL_LIBRARY) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/tcl/tcl8.4
set env(TK_LIBRARY) ${env(SLICER_HOME)}/Lib/${env(BUILD)}/tk/tk8.4
set env(TCLLIBPATH) "${env(VTK_SRC_DIR)} ${env(SLICER_HOME)}/Base/Wrapping/Tcl/vtkSlicerBase ${env(TCLLIBPATH)}"

# Add the module bin directories to the load library path and the Wrapping/Tcl directories to the tcl library path
set modulePath ${env(SLICER_HOME)}/Modules
set modulePaths [glob ${modulePath}/vtk*]
foreach dir $modulePaths {
    # get the module name
    regexp "$modulePath/(\.\*)" $dir match moduleName
    # if it's not the custom one, append it to the path
    if {[string first Custom $moduleName] == -1} {
        puts "Adding module: ${moduleName}"
        set env(LD_LIBRARY_PATH) ${env(SLICER_HOME)}/Modules/${moduleName}/builds/${env(BUILD)}/bin:${env(LD_LIBRARY_PATH)}
        set env(TCLLIBPATH) "${env(SLICER_HOME)}/Modules/${moduleName}/Wrapping/Tcl $env(TCLLIBPATH)"
    }
}


set msg "
Slicer is an experimental software package.
Any human use requires proper research controls.  
Clicking \"Ok\" below binds you to the license agreement.
See www.slicer.org for details.
"
# set resp [tk_messageBox -message $msg -type okcancel -title "Slicer2"]
set resp okay

if {$resp == "cancel"} {
    exit
}


if {$tcl_platform(platform) == "unix"} {
    eval exec $env(SLICER_HOME)/slicer $argv &
} else {
    # must be win32
    eval exec $env(SLICER_HOME)/slicer.bat $argv &
}

exit
