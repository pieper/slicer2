#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

###############################################################
# Run this script to automatically generate the slicer website
# (with tcl code documentation!).
#
# Then open slicer2/Doc/index.html to SEE the website.
# 
# All auto* files in the slicer2/Doc subdirectories will be
# combined into one index.html file for that subdirectory,
# with the appropriate website formatting.
#
# Also, all tcl files in your SLICER_HOME/Base/tcl directory will
# be parsed to create the info pages under the Developer's Guide
# in the website.  The quality of the documentation depends on 
# the comment program having been run recently.  
# This is the script slicer2/Scripts/document.tcl.
#
# ######  To edit the website: ########
# 1.Look in the slicer2/Doc/devl for formatting examples,
# also visible under Developer's Guide in the website.
# 2. Edit appropriate file and run this script.
#
# ######  To add a new file to the website: ########
# 1. Add an autoXX.html file to a subdirectory of slicer2/Doc.
# (auto* files will be combined in alphabetical order into index.html)
# 2. Edit slicer2/Doc/index.html to add your new section to the 
# table of contents.
# 3. Run this script.
#
###############################################################

set cwd [pwd]
cd [file dirname [info script]]
cd ..
set SLICER_HOME [pwd]
cd $cwd

puts "SLICER_HOME = $::SLICER_HOME"

# Produce the web pages and tcl documentation
# exec 
puts "Producing documentation pages - web site and tcl code docs"
source $::SLICER_HOME/Base/tcl/GoDocument.tcl

# Produce the VTK class documentation, need environment variables set for doxygen

set ::env(SLICER_DOC) [file join $::SLICER_HOME Doc]
set ::env(SLICER_HOME) $::SLICER_HOME
puts "Producing the VTK documentation, based on SLICER_HOME = ${SLICER_HOME}, SLICER_DOC = $::env(SLICER_DOC)"
catch "eval exec \"doxygen $::SLICER_HOME/Base/cxx/Doxyfile\"" res
puts $res

