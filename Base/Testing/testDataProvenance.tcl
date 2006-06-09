package require vtkSlicerBase
package require vtkITK


# open up a notes file for writing
set fname [file join $::env(SLICER_HOME) Base Testing Notes dp.txt]

if {[file exists [file dirname $fname]] == 0} {
    # make the directory
    file mkdir [file dirname $fname]
}
if {[catch "set fd [open $fname w]" errmsg] != 0} {
    puts "Error opening file $fname for writing, exit 1"
    exit 1
}

if {$fd == ""} {
    puts "Error opening file $fname for writing, exit 1"
    exit 1
}

vtkMrmlSlicer Slicer

# for ParseCVSInfo
source [file join $::env(SLICER_HOME) Base tcl tcl-main Main.tcl]

set execName ""
switch $tcl_platform(os) {
    "SunOS" {
        set execName "slicer2-solaris-sparc"
    }
    "Linux" {
        set execName "slicer2-linux-x86"
    }
    "Darwin" {
        set execName "slicer2-darwin-ppc"
    }
    default {
        set execName "slicer2-win32.exe"
    }
}

set compilerVersion [Slicer GetCompilerVersion]
set compilerName [Slicer GetCompilerName]
set vtkVersion [Slicer GetVTKVersion]
if {[info command vtkITKVersion] == ""} {
    set itkVersion "none"
} else {
    catch "vtkITKVersion vtkitkver"
    catch "set itkVersion [vtkitkver GetITKVersion]"
    catch "vtkitkver Delete"
}

puts $fd "ProgramName: $execName\nProgramArguments: $argv\nTimeStamp: [clock format [clock seconds] -format "%D-%T-%Z"]\nUser: $::env(USER)\nMachine: $tcl_platform(machine)\nPlatform: $tcl_platform(os) PlatformVersion: $tcl_platform(osVersion)"
puts $fd "Version: [ParseCVSInfo "" {$Name:  $}]"
puts $fd "CVS: [ParseCVSInfo "" {$Id: testDataProvenance.tcl,v 1.1 2006/06/09 20:01:41 nicole Exp $}]"
puts $fd "CompilerName: ${compilerName}\nCompilerVersion: $compilerVersion"
puts $fd "LibName: VTK LibVersion: ${vtkVersion}\nLibName: TCL LibVersion: ${tcl_patchLevel}\nLibName: TK LibVersion: ${tk_patchLevel}\nLibName: ITK LibVersion: ${itkVersion}"

close $fd

if {0} {
set ::SLICER(versionInfo)  "ProgramName: $execName ProgramArguments: $argv\nTimeStamp: [clock format [clock seconds] -format "%D-%T-%Z"] User: $::env(USER) Machine: $tcl_platform(machine) Platform: $tcl_platform(os) PlatformVersion: $tcl_platform(osVersion)"
set libVersions "LibName: VTK LibVersion: ${vtkVersion} LibName: TCL LibVersion: ${tcl_patchLevel} LibName: TK LibVersion: ${tk_patchLevel} LibName: ITK LibVersion: ${itkVersion}"
set SLICER(versionInfo) "$SLICER(versionInfo)  Version: $SLICER(version) CompilerName: ${compilerName} CompilerVersion: $compilerVersion ${libVersions} CVS: [ParseCVSInfo "" {$Id: testDataProvenance.tcl,v 1.1 2006/06/09 20:01:41 nicole Exp $}] "

puts "$SLICER(versionInfo)"
}

puts "0"
exit 0
