
package require Tk
wm withdraw .

set env(SLICER_HOME) [file dir [file dir [info script]]]

set msg "
Slicer is an experimental software package.
Any human use requires proper research controls.  
Clicking \"Ok\" below binds you to the license agreement.
See www.slicer.org for details.
"
set resp [tk_messageBox -message $msg -type okcancel -title "Slicer2"]

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
