#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        Go.tcl
# PROCEDURES:  
#   Usage
#   SplashShow
#   SplashRaise
#   SplashKill
#   ReadModuleNames
#   FindNames
#   ReadModuleNamesLocalOrCentral
#   GetFullPath
#   START_THE_SLICER
#==========================================================================auto=

wm withdraw .
update


#######################
# version number control for slicer
#

# bump major when incompatibile changes happen either within slicer
# or in vtk or tcl

set SLICER(major_version) 2

# bump minor when features accumulate to a stable state for release

set SLICER(minor_version) 1

# bump revision for something that has been given out to non-developers
# (e.g. bump revsion before packaging, then again after packaging
#  so there's a unique label for the packaged version)

set SLICER(revision) b1

# when packaging a release for distribution, set state to ""
# when packaging a release for testing, set state to the date as "-YYYY-MM-DD"
#  otherwise leave it as "-dev"

set SLICER(state) ""

set SLICER(version) "$SLICER(major_version).$SLICER(minor_version)$SLICER(revision)$SLICER(state)"

#
######################




#
# simple command line argument parsing
#

proc Usage { {msg ""} } {
    global SLICER
    
    set msg "$msg\nusage: slicer2-<arch> \[options\] \[MRML file name .xml | dir with MRML file\]"
    set msg "$msg\n  <arch> is one of win32.exe, solaris-sparc, or linux-x86"
    set msg "$msg\n  \[options\] is one of the following:"
    set msg "$msg\n   --help : prints this message and exits"
    set msg "$msg\n   --verbose : turns on extra debugging output"
    set msg "$msg\n   --no-threads : disables multi threading"
    set msg "$msg\n   --no-tkcon : disables tk console"
    set msg "$msg\n   --load-dicom <dir> : read dicom files from <dir> at startup"
    set msg "$msg\n   --script <file.tcl> : script to execute after slicer loads"
    set msg "$msg\n   --version : print out the version info and continue"
    puts stderr $msg
    tk_messageBox -message $msg -title $SLICER(version) -type ok
}

#
# simple arg parsing 
#
set SLICER(threaded) "true"
set SLICER(tkcon) "true"
set verbose 0
set Module(verbose) 0
set SLICER(load-dicom) ""
set SLICER(script) ""
set SLICER(versionInfo) ""
set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "--verbose" -
        "-v" {
            set verbose 1
            set Module(verbose) 1
        }
        "--help" -
        "-h" {
            Usage
            exit 1
        }
        "--no-threads" {
            set SLICER(threaded) "false"
        }
        "--no-tkcon" {
            set SLICER(tkcon) "false"
        }
        "--load-dicom" {
            incr i
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set SLICER(load-dicom) [lindex $argv $i]
                if { [file type $SLICER(load-dicom)] == "file" } {
                    # user picked a file, load all dicoms in same dir
                    set SLICER(load-dicom) [file dir $SLICER(load-dicom)]
                }
            }
        }
        "--script" {
            incr i
            if { $i == $argc } {
                Usage "missing argument for $a\n"
            } else {
                set SLICER(script) [lindex $argv $i]
            }
        }
        "--version" {
            # for data provenance, print out the executable name, version, vtk and version, compiler name and version and arguments
            # executable-name version libname libversion [compilername compilerversion] cvstag -arg1 val1 -arg2 val2
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
            # add in the compiler info after MainBoot is called
            set SLICER(versionInfo)  "$execName $argv\nTimestamp: [clock format [clock seconds] -format "%D-%T-%Z"] Version: ${SLICER(version)} User: $env(USER) Machine: $tcl_platform(machine) Platform: $tcl_platform(os) PlatformVersion: $tcl_platform(osVersion)"

        }
        "-*" {
            Usage "unknown option $a\n"
        }
        default {
            lappend strippedargs $a
        }
    }
}
set argv $strippedargs
set argc [llength $argv]

if {$argc > 1 } {
    Usage
    exit 1
}


#
# Determine Slicer's home directory from the SLICER_HOME environment 
# variable, or the root directory of this script ($argv0).
#
if {[info exists env(SLICER_HOME)] == 0 || $env(SLICER_HOME) == ""} {
    # set prog [file dirname $argv0]
    set prog [file dirname [info script]]
} else {
    set prog [file join $env(SLICER_HOME) Base/tcl]
}
# Don't use "."
if {$prog == "."} {
    set prog [pwd]
}
# Ensure the program directory exists
if {[file exists $prog] == 0} {
    tk_messageBox -message "The directory '$prog' does not exist."
    exit
}
if {[file isdirectory $prog] == 0} {
    tk_messageBox -message "'$prog' is not a directory."
    exit
}
set Path(program) $prog


########################
# simple splash screen 
#                      
# do this before loading vtk dlls so people have something
# to look at during startup (and so they see the important
# warning message!)
########################
 
proc SplashRaise {} { 
    if {[winfo exists .splash]} {raise .splash; after 100 "after idle SplashRaise"}
}

proc SplashKill {} { 
    global splashim
    catch "destroy .splash" 
    catch "image delete $splashim"
}

proc SplashShow { {delayms 7000} } {
    global Path SLICER splashim

    set oscaling [tk scaling]
    # ignore screen based default scaling and pick scale so words show up nicely inside box
    tk scaling 1.5
    set splashfont [font create -family Helvetica -size 10]
    set splashfontb [font create -family Helvetica -size 10 -weight bold]
    toplevel .splash -relief raised -borderwidth 6 -width 500 -height 400 -bg white
    wm overrideredirect .splash 1
    wm geometry .splash +[expr [winfo screenwidth .splash]/2-250]+[expr [winfo screenheight .splash]/2-200]

    # add the program path so slicer can find the picture no matter what directory it is started in 
    set splashim [image create photo -file [file join $Path(program) gui/welcome.ppm]]
    label .splash.l -image $splashim
    place .splash.l -relx 0.5 -rely 0.35 -anchor center
    label .splash.t1 -text "Slicer is not an FDA approved medical device \nand is for Research Use Only." -bg white -fg red -font $splashfontb
    label .splash.t2 -text "See www.slicer.org for license details." -bg white -fg red -font $splashfont
    place .splash.t1 -relx 0.5 -rely 0.70 -anchor center
    place .splash.t2 -relx 0.5 -rely 0.80 -anchor center
    label .splash.v -text "Version: $SLICER(version)" -bg white -fg darkblue -font $splashfont
    place .splash.v -relx 0.5 -rely 0.95 -anchor center
    after $delayms SplashKill
    SplashRaise
    update
    bind .splash <1> SplashKill
    tk scaling $oscaling
}

SplashShow


#
# startup with the tkcon
#
if { $SLICER(tkcon) == "true" } { 
    set av $argv; set argv "" ;# keep tkcon from trying to interpret command line args
    source $prog/tkcon.tcl
    ::tkcon::Init
    tkcon attach main
    wm geometry .tkcon +10-50
    set argv $av
}

raise .splash
update


#
# load the tcl packages and shared libraries of cxx code
# (proper environment is set up in launch.tcl)
#

puts "Loading Base..."
package require vtkSlicerBase ;# this pulls in all of slicer

foreach m $env(SLICER_MODULES_TO_REQUIRE) {
    puts "Loading Module $m..."
    if { [catch {package require $m} errVal] } {
        puts stderr "Warning: can't load module $m:\n$errVal"
    }
}

#
# turn off if user wants - re-enabled threading by default
# based on Raul's fixes to vtkImageReformat 2002-11-26
#
puts "threaded is $SLICER(threaded)"
if { $SLICER(threaded) == "false" } {
    vtkMultiThreader tempMultiThreader
    tempMultiThreader SetGlobalDefaultNumberOfThreads 1
    tempMultiThreader Delete
}


# Source Tcl scripts
# Source optional local copies of files with programming changes


#-------------------------------------------------------------------------------
# .PROC ReadModuleNames
# Reads Options.xml 
# Returns ordered and suppressed modules
# .END
#-------------------------------------------------------------------------------
proc ReadModuleNames {filename} {

    if {$filename == ""} {
        return [list "" ""]
    }

    set tags [MainMrmlReadVersion2.0 $filename 0]
    if {$tags == 0} {
        return [list "" ""]
    }

    foreach pair $tags {
        set tag  [lindex $pair 0]
        set attr [lreplace $pair 0 0]
        
        switch $tag {
            
            "Options" {
                foreach a $attr {
                    set key [lindex $a 0]
                    set val [lreplace $a 0 0]
                    set node($key) $val
                }
                if {$node(program) == "slicer" && $node(contents) == "modules"} {
                    return [list $node(ordered) $node(suppressed)]
                }
            }
        }
    }
    return [list "" ""]
}

#-------------------------------------------------------------------------------
# .PROC FindNames
#
# Looks for all the modules.
# For example, for a non-essential module, looks first for ./tcl-modules/*.tcl
# Then looks for $SLICER_HOME/program/tcl-modules/*.tcl
#
# .END
#-------------------------------------------------------------------------------
proc FindNames {dir} {
    global prog
    set names ""

    # Form a full path by appending the name (ie: Volumes) to
    # a local, and then central, directory.
    set local   $dir
    set central [file join $prog $dir]

    # Look locally
    foreach fullname [glob -nocomplain $local/*] {
        if {[regexp "$local/(\.*).tcl$" $fullname match name] == 1} {
            lappend names $name
        }
    }
    # Look centrally
    foreach fullname [glob -nocomplain $central/*] {
        if {[regexp "$central/(\.*).tcl$" $fullname match name] == 1} {
            if {[lsearch $names $name] == -1} {
                lappend names $name
            }
        }
    }
    return $names
}

#-------------------------------------------------------------------------------
# .PROC ReadModuleNamesLocalOrCentral
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ReadModuleNamesLocalOrCentral {name ext} {
    global prog verbose

    set path [GetFullPath $name $ext "" 0]
    set names [ReadModuleNames $path]
    return $names
}

#-------------------------------------------------------------------------------
# .PROC GetFullPath
# 
#
# .END
#-------------------------------------------------------------------------------
proc GetFullPath {name ext {dir "" } {verbose 1}} {
    global prog

    # Form a full path by appending the name (ie: Volumes) to
    # a local, and then central, directory.
    set local   [file join $dir $name].$ext
    set central [file join [file join $prog $dir] $name].$ext

    if {[file exists $local] == 1} {
        return $local
    } elseif {[file exists $central] == 1} {
        return $central
    } else {
        if {$verbose == 1} {
            set msg "File '$name.$ext' cannot be found"
            puts $msg
            tk_messageBox -message $msg
        }
        return ""
    }
}


#-------------------------------------------------------------------------------
# .PROC START_THE_SLICER
#
# Looks in ./tcl-shared ./tcl-modules and ./tcl-shared for names of tcl files
# Also looks in $central/tcl-shared ... (which is $SLICER_HOME/program/..)
#
# Source those files
# Boot the slicer
#
# If the environment variable SLICER_SCRIPT exist, 
# and it points to a tcl file, source the file. 
# Then, if the function SlicerScript exists, run it after booting.
#
# .END
#-------------------------------------------------------------------------------

# Steps to sourcing files:
# 1.) Get an ordered list of module names (ie: Volumes, not Volumes.tcl).
# 2.) Append names of other existing modules to this list.
# 3.) Remove names from the list that are in the "suppressed" list.
#

# Source Parse.tcl to read the XML
set path [GetFullPath Parse tcl tcl-main]
source $path

# Look for an Options.xml file locally and then centrally
set moduleNames [ReadModuleNamesLocalOrCentral Options xml]
set ordered [lindex $moduleNames 0]
set suppressed [lindex $moduleNames 1]

# Find all module names
set found [FindNames tcl-modules]

if {$verbose == 1} {
    puts "found=$found"
    puts "ordered=$ordered"
    puts "suppressed=$suppressed"
}

# Append found names to ordered names
# - after this ordered includes:
# --- specifically named modules from Options.xml
# --- modules found by looking for tcl files
foreach name $found {
    if {[lsearch $ordered $name] == -1} {
        lappend ordered $name
    }
}

# Suppress unwanted (need a more PC term for this) modules
foreach name $suppressed {
    set i [lsearch $ordered $name]
    if {$i != -1} {
        set ordered [lreplace $ordered $i $i]
    }
}

# Source the modules
set foundOrdered ""
foreach name $ordered {
    if { [info command ${name}Init] == "" } {
        # if the entry point proc doesn't exist yet,
        # then read the file (Modules loaded through 
        # 'package require' will already have had their code sourced
        set path [GetFullPath $name tcl tcl-modules]
        if {$path != ""} {
            if {$verbose == 1} {puts "source $path"}
            source $path
            lappend foundOrdered $name
        } 
    } else {
        if {$verbose == 1} {puts "aready have $name"}
        lappend foundOrdered $name
    }
}

# Add any custom Modules to foundOrdered, they will have had to have added
# their name to Module(customModules)
# otherwise would have to leave out any that are loaded by 
# sub sections of the interface, ie the Volumes or the Editors scripts 
# ie no matches for Vol*Init or Ed*Init
if {[info exists Module(customModules)]  == 1} {
    if { $verbose == 1 } {
        puts "Custom modules we need to add: $Module(customModules)"
    }
    # it's already been sourced, so just add to the foundOrdered list
    foreach customModule $Module(customModules) {
        lappend foundOrdered $customModule
    }
}

# Ordered list only contains modules that exist
set ordered $foundOrdered

# Source shared stuff either locally or globally
# For example for a module MyModule, we looks for
# ./tcl-modules/MyModule.tcl and then for $SLICER_HOME/program/tcl-modules/MyModule.tcl
# Similar for tcl-shared and tcl-main

set shared [FindNames tcl-shared]
foreach name $shared {
    set path [GetFullPath $name tcl tcl-shared]
    if {$path != ""} {
        if {$verbose == 1} {puts "source $path"}
        source $path
    }
}

# Source main stuff either locally or globally
set main [FindNames tcl-main]
foreach name $main {
    set path [GetFullPath $name tcl tcl-main]
    if {$path != ""} {
        if {$verbose == 1} {puts "source $path"}
        source $path
    }
}

# Set global variables
set Module(idList)     $ordered
set Module(mainList)   $main
set Module(sharedList) $shared
set Module(supList)    $suppressed
set Module(allList)    [concat $ordered $suppressed]

if {$verbose == 1} {
    puts "ordered=$ordered"
    puts "main=$main"
    puts "shared=$shared"
}

# Bootup
global View
set View(render_on) 1
MainBoot [lindex $argv 0]
set View(render_on) 0

### print out the versioning info
if { $SLICER(versionInfo) != "" } {
    # have to get the compiler information after MainBoot
    set compilerVersion [Slicer GetCompilerVersion]
    set compilerName [Slicer GetCompilerName]
    set vtkVersion [Slicer GetVTKVersion]
    set libVersions "LibName1: VTK LibVersion1: ${vtkVersion} LibName2: TCL LibVersion2: ${tcl_patchLevel} LibName3: TK LibVersion2: ${tk_patchLevel}"
    set SLICER(versionInfo) "$SLICER(versionInfo) CompilerName: ${compilerName} CompilerVersion: $compilerVersion ${libVersions} CVS: [ParseCVSInfo "" {$Id: Go.tcl,v 1.59 2003/05/30 21:47:31 pieper Exp $}] "
    puts "$SLICER(versionInfo)"
}

#
# read a dicom volume specified on command line
#
if { $SLICER(load-dicom) != "" } {
    global Volumes 
    VolumesSetPropertyType VolDicom
    MainVolumesSetActive NEW
    Tab Volumes row1 Props
    set Volumes(DICOMStartDir) $SLICER(load-dicom)
    DICOMSelectMain $Volume(dICOMFileListbox) "autoload"
    VolumesSetPropertyType VolHeader
    VolumesPropsApply
    RenderAll
    Tab Data
}


#
# override the built in exit routine to provide cleanup
# (for people who type exit into the console)
#
rename exit tcl_exit
proc exit {} {
    MainExitProgram
}


### Did someone set the SLICER_SCRIPT environment variable
### If they did and it is a tcl file, source it.
### If the SlicerScript function exists, run it

if {[info exists env(SLICER_SCRIPT)] != 0 && $env(SLICER_SCRIPT) != ""} {

    ## Is it a tcl file
    if {[regexp "\.tcl$\s*$" $env(SLICER_SCRIPT)] == 1} {
        source $env(SLICER_SCRIPT)
        if {[info commands SlicerScript] != ""} {
            puts "Running slicer script..."
            SlicerScript
            puts "Done running slicer script."
        }
    }
}

### or run a script specified on the command line with --script

if { $SLICER(script) != "" } {
    source $SLICER(script)
}


