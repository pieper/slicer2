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

set SLICER(minor_version) 0

# bump revision for something that has been given out to non-developers
# (e.g. bump revsion before packaging, then again after packaging
#  so there's a unique label for the packaged version)

set SLICER(revision) a2

# when packaging a release for distribution, set state to ""
# when packaging a release for testing, set state to the date as "-YYYY-MM-DD"
#  otherwise leave it as "-dev"

set SLICER(state) -dev

set SLICER(version) "$SLICER(major_version).$SLICER(minor_version)$SLICER(revision)$SLICER(state)"

#
#######################

#
# simple command line argument parsing
#

proc usage {} {
    global SLICER

    set msg "usage: slicer2-<arch> \[MRML file name .xml | dir with MRML file\]"
    set msg "$msg\n  <arch> is one of win32.exe, solaris-sparc, or linux-x86"
    tk_messageBox -message $msg -title $SLICER(version) -type ok
}

#
# simple arg parsing - can't yet handle args with parameters
#
set verbose 0
set strippedargs ""
foreach a $argv {
    switch -glob -- $a {
        "--verbose" -
        "-v" {
            set verbose 1
            set Module(verbose) 1
        }
        "--help" -
        "-h" {
            usage
            exit 1
        }
        "-*" {
            puts stderr "unknown option $a\n"
            usage
        }
        default {
            lappend strippedargs $a
        }
    }
}
set argv $strippedargs
set argc [llength $argv]

if {$argc > 1 } {
    usage
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
# to look at during startup
########################
 
toplevel .splash -relief raised -borderwidth 6 -width 500 -height 400 -bg white
wm overrideredirect .splash 1
wm geometry .splash +[expr [winfo screenwidth .splash]/2-250]+[expr [winfo screenheight .splash]/2-200]
# added the program path so slicer can find the picture no matter what directory it is started in 
global Path
set splashim [image create photo -file [file join $Path(program) gui/welcome.ppm]]
label .splash.l -image $splashim
place .splash.l -relx 0.5 -rely 0.35 -anchor center
label .splash.t -text "Please be aware that Slicer is not validated and \nis for Research Use Only.\n\nSee www.slicer.org for license details." -bg white -fg red
place .splash.t -relx 0.5 -rely 0.75 -anchor center
after 7000 "destroy .splash; image delete $splashim"
proc raisesplash {} { if {[winfo exists .splash]} {raise .splash; after 100 "after idle raisesplash"}}
raisesplash
update


#
# set statup options - convert backslashes from windows
# version of SLICER_HOME var into to regular slashes
# (won't matter for unix version)
#
regsub -all {\\} $env(SLICER_HOME) / slicer_home
regsub -all {\\} $env(VTK_SRC_DIR) / vtk_src_dir
lappend auto_path $slicer_home/Base/tcl 
lappend auto_path $slicer_home/Base/Wrapping/Tcl/vtkSlicerBase
lappend auto_path $vtk_src_dir/Wrapping/Tcl

package require vtkSlicerBase ;# this pulls in all of slicer

# Set path to search for plug-in modules, and require them
set modulePath $slicer_home/Modules
set modulePaths ""
catch {set modulePaths [glob $modulePath/vtk*]}

# do two separate loops to solve interdependency problems between modules, add all modules to the autopath first so that any package requiring another module can find it if they are not loaded in the right order
foreach dir $modulePaths {
    # get the module name
    regexp "$modulePath/(\.\*)" $dir match moduleName
    # if it's not the custom one, append it to the path
    if {[string first Custom $moduleName] == -1} {
        puts "Adding module to auto_path: ${moduleName}"
        lappend auto_path ${slicer_home}/Modules/${moduleName}/Wrapping/Tcl/${moduleName}
    }
}
# second loop to deal with all the package requires after the auto path is set up
foreach dir $modulePaths {
    regexp "$modulePath/(\.\*)" $dir match moduleName
    if {[string first Custom $moduleName] == -1} {
        puts "Requiring module: ${moduleName}"
        # catch {package require ${moduleName}}
        if { [catch {package require ${moduleName}} errVal] } {
            puts stderr "ERROR while requiring  ${moduleName}:\n$errVal"
        }
    }
}

#
# turn off multithreading by default
# - not all classes appear to be safe currently - 2002-11-12 sp
#
vtkMultiThreader tempMultiThreader
tempMultiThreader SetGlobalDefaultNumberOfThreads 1
tempMultiThreader Delete

# turn off warnings about old function use
if { $tcl_platform(platform) == "windows" } {
    vtkObject o
    # o SetGlobalWarningDisplay 0
    o Delete
}

#
# startup with the tkcon
#
if {1} { 
    set av $argv; set argv "" ;# keep tkcon from trying to interpret command line args
    source $prog/tkcon.tcl
    ::tkcon::Init
    tkcon attach main
    wm geometry .tkcon +10-50
    set argv $av
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


# override the built in exit routine to provide cleanup
# (for people who type exit into the console)
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

