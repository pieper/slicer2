
# Visualization Toolkit (VTK) Tcl package configuration.


#
# SLICER NOTE: 
# This is a modified version to allow location of the vtk tcl directories
# to be controlled by the VTK_SRC_DIR and VTK_BIN_DIR environment 
# variables rather than being hard coded to the build directory
#

package ifneeded vtkinit {4.2} {
  namespace eval ::vtk::init {
    proc load_library_package {libName libPath {libPrefix {lib}}} {
      set libExt [info sharedlibextension]
      set currentDirectory [pwd]
      set libFile [file join $libPath "$libPrefix$libName$libExt"]
      if {[catch "cd {$libPath}; load {$libFile}" errorMessage]} {
        puts $errorMessage
      }
      cd $currentDirectory
    }
    proc require_package {name {version {4.2}}} {
      if {[catch "package require -exact $name $version" errorMessage]} {
        puts $errorMessage
        return 0
      } else {
        return 1
      }
    }
  }
  package provide vtkinit {4.2}
}

foreach kit { Common Filtering IO Imaging Graphics
              Rendering Hybrid 
              Patented  } {
  package ifneeded "vtk${kit}TCL" {4.2} "
    package require -exact vtkinit {4.2}
    ::vtk::init::load_library_package {vtk${kit}TCL} {$::env(VTK_BIN_DIR)/bin}
  "
  package ifneeded "vtk[string tolower ${kit}]" {4.2} "
    package require -exact vtkinit {4.2}
    if {\[catch {source \[file join {$::env(VTK_SRC_DIR)/Wrapping/Tcl} {vtk[string tolower ${kit}]} {vtk[string tolower ${kit}].tcl}\]} errorMessage\]} {
      puts \$errorMessage
    }
  "
}

foreach src {vtk vtkbase vtkinteraction vtktesting} {
  package ifneeded ${src} {4.2} "
    package require -exact vtkinit {4.2}
    if {\[catch {source \[file join {$::env(VTK_SRC_DIR)/Wrapping/Tcl} {$src} {$src.tcl}\]} errorMessage\]} {
      puts \$errorMessage
    }
  "
}
