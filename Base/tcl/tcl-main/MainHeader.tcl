#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        MainHeader.tcl
# DATE:        02/22/2000 11:11
# PROCEDURES:  
#   ReadHeader
#   ParsePrintHeader
#   ParseM3list
#   DumpHeader
#   GetHeaderInfo
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ReadHeader
# .END
#-------------------------------------------------------------------------------
proc ReadHeader {image run utility tk} {
    global Gui
	# Run a header reading utility
	if {$run == 1} {
		if {[catch {set hdr [exec $utility $image]} errmsg] == 1} {
		    # correct return val is in errmsg on unix; on pc it's an error.
		    if {$Gui(pc) == 1} {
			puts $errmsg
			if {$tk == 1} {
				tk_messageBox -icon error -message $errmsg
			}
			return ""
		    } else {
			set hdr $errmsg
		    }
		}
	} else {
		set fid [open $utility]
		set hdr [read $fid]
	       close $fid
	}
	return $hdr
}

#-------------------------------------------------------------------------------
# .PROC ParsePrintHeader
# .END
#-------------------------------------------------------------------------------
proc ParsePrintHeader {text aHeader} {
	
	upvar $aHeader Header
	set text "$text\n"

	# These are commentys
	# 'text' was read from the file
	# This routine parses 'text' to set variables in the Header array.
	# For each key, find it in the 'text', and set the corresponding
	# value in the array.
	#
	# fKey = the key in the file (text)
	# aKey = the key in the Header array

	set errmsg ""
	foreach  \
		fKey "x_resolution y_resolution pixel_xsize pixel_ysize thick space \
		byte_order bytes_per_pixel \
		coord_center_r coord_center_a coord_center_s \
		coord_r_top_left coord_a_top_left coord_s_top_left \
		coord_r_top_right coord_a_top_right coord_s_top_right \
		coord_r_bottom_right coord_a_bottom_right coord_s_bottom_right" \
		\
		aKey "xDim yDim xSpacing ySpacing sliceThick sliceSpace \
		littleEndian scalarSize \
		rC aC sC rTL aTL sTL rTR aTR sTR rBR aBR sBR" \
		{
		if {[regexp "$fKey \*= \*\(\[\^ \]\*\)\n" $text match item] == 1} {
			set Header($aKey) $item
		} else {
			set errmsg "$errmsg $fKey"
		}
	}

	set Header(zSpacing) [expr $Header(sliceThick) + $Header(sliceSpace)]

	# Not in print_header
	set Header(scalarType) Short
	set Header(numScalars) 1
	set Header(sliceTilt) 0
	set Header(order) ""

	if {$errmsg != ""} {
		set errmsg "Error reading header. Can't find:\n$errmsg"
	}
	return $errmsg
}

#-------------------------------------------------------------------------------
# .PROC ParseM3list
# .END
#-------------------------------------------------------------------------------
proc ParseM3list {text aHeader} {

	upvar $aHeader Header

	# Little endian:
	# within a given 16- or 32-bit word, bytes at lower addresses have
	# lower significance (the word is stored `little-end-first').  The
	# PDP-11 and VAX families of computers and Intel microprocessors and
	# a lot of communications and networking hardware are little-endian
	if {[regexp {High Bit[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*([0-9\.-]*)[ ]*} \
		$text match x] == 1} {
		if {$x > 7} {
			set Header(littleEndian) 1
		} else {
			set Header(littleEndian) 0
		}
	}
	# Columns
	if {[regexp {Columns[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*([0-9\.-]*)[ ]*} \
		$text match x] == 1} {
		set Header(xDim) $x
	}
	# Rows
	if {[regexp {Rows[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*([0-9\.-]*)[ ]*} \
		$text match x] == 1} {
		set Header(yDim) $x
		set res $x
	}
	# Slice Thickness
	if {[regexp {Slice Thickness[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*\[[ ]*([0-9\.-]*)[ ]*\]} \
		$text match x] == 1} {
		set Header(sliceThick) $x
	}
	# Space Btwn Slices
	# Note: CT does not have that
	if {[regexp {Space Btwn Slices[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*\[[ ]*([0-9\.-]*)[ ]*\]} \
		$text match x] == 1} {
		set Header(sliceSpace) $x
	} else {
		set Header(sliceSpace) 0
	}
	# Pixel Size
	if {[regexp {Pixel Size[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*\[[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)[ ]*\]} \
		$text match x y] == 1} {
		set Header(xSpacing) $x
		set Header(ySpacing) $y
		set pixel $x
	}
	# Orien(Patient)
	if {[regexp {Orien\(Patient\)[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*\[[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)[ ]*\]} \
		$text match ir ia is jr ja js] == 1} {
	}
	# Image Pos(Patient)
	if {[regexp {Image Pos\(Patient\)[ ]*[^ ]*[ ]*[^ ]*[ ]*[^ ]*[ ]*\[[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)\\[ ]*([0-9\.-]*)[ ]*\]} \
		$text match or oa os] == 1} {
	}

	# Compute Corner points
	set fov [expr $pixel * $res * 1.0]
	set cr [expr 1.0*$or + ($ir + $jr) * $fov/2]
	set ca [expr 1.0*$oa + ($ia + $ja) * $fov/2]
	set cs [expr 1.0*$os + ($is + $js) * $fov/2]

	# center
	set Header(rC) $cr
	set Header(aC) $ca
	set Header(sC) $cs

	# top left
	set Header(rTL) $or
	set Header(aTL) $oa
	set Header(sTL) $os

	# top right
	set Header(rTR) [expr $or + $ir * $fov]
	set Header(aTR) [expr $oa + $ia * $fov]
	set Header(sTR) [expr $os + $is * $fov]

	# bottom right
	set Header(rBL) [expr $or + ($ir + $jr) * $fov]
	set Header(aBL) [expr $oa + ($ia + $ja) * $fov]
	set Header(sBL) [expr $os + ($is + $js) * $fov]

	set Header(zSpacing) [expr $Header(sliceThick) + $Header(sliceSpace)]

	# Not in print_header
	set Header(scalarType) Short
	set Header(numScalars) 1
	set Header(sliceTilt) 0
	set Header(order) ""

	return

	# dump for debugging
	puts "fov=$fov"
	puts "position = $or $oa $os"
	puts "orientation = $ir $ia $is, $jr $ja $js"
	puts "CENTER: $cr $ca $cs"
	puts "TOP-LEFT: $or $oa $os"
	puts "TOP-RIGHT: \
		[expr $or + $ir * $fov] \
		[expr $oa + $ia * $fov] \
		[expr $os + $is * $fov]"
	puts "BOT-RIGHT: \
		[expr $or + ($ir + $jr) * $fov] \
		[expr $oa + ($ia + $ja) * $fov] \
		[expr $os + ($is + $js) * $fov]"
}

#-------------------------------------------------------------------------------
# .PROC DumpHeader
# .END
#-------------------------------------------------------------------------------
proc DumpHeader {aHeader} {
	upvar $aHeader Header
	foreach item [lsort [array names Header]] {
		puts "$item = '$Header($item)'"
	}
}

#-------------------------------------------------------------------------------
# .PROC GetHeaderInfo
# .END
#-------------------------------------------------------------------------------
proc GetHeaderInfo {img1 num2 node tk} {
	global Mrml Path

	if {[CheckFileExists $img1 0] == 0} {
		return "Cannot open '$img1'."
	}

	# Get filename pattern
	set prefix [file root $img1]
	if {[regexp {\.([0-9]*)([^0-9]*)$} $img1 match num1 suffix] == 0} {
		return ""
	}
	# Rid unnecessary 0's
	set num1 [string trimleft $num1 "0"]
	if {$num1 == ""} {set num1 0}

	# Determine the pattern by testing a couple cases
	if {[format "%s.%03d$suffix" $prefix $num1] == $img1} {
		set filePattern "%s.%03d$suffix"
	} elseif {[format "%s.%d$suffix" $prefix $num1] == $img1} {
		set filePattern "%s.%d$suffix"
	} else {
		set filePattern ""
	}

	# Compute the full path of the last image in the volume
	set img2 [format $filePattern $prefix $num2]
	if {[CheckFileExists $img2 0] == 0} {
		return "Cannot open '$img2'."
	}

	# Read headers
	set hdr1 [ReadHeader $img1 1 $Path(printHeader) $tk]
	set hdr2 [ReadHeader $img2 1 $Path(printHeader) $tk]

	# Parse headers
	set errmsg [ParsePrintHeader $hdr1 Header1]
	if {$errmsg != ""} {
		return $errmsg
	}
	set errmsg [ParsePrintHeader $hdr2 Header2]
	if {$errmsg != ""} {
		return $errmsg
	}

	# Set the volume node's attributes using header info
	$node SetFilePrefix $prefix
	$node SetFullPrefix [file join $Mrml(dir) [$node GetFilePrefix]]
	$node SetFilePattern $filePattern
	$node SetImageRange $num1 $num2
	$node SetDimensions $Header1(xDim) $Header1(yDim)
	$node SetSpacing $Header1(xSpacing) $Header1(ySpacing) $Header1(zSpacing)
	$node SetScalarTypeTo$Header1(scalarType)
	$node SetNumScalars $Header1(numScalars)
	$node SetLittleEndian $Header1(littleEndian)
	$node SetTilt $Header1(sliceTilt)
	set result [$node ComputeRasToIjkFromCorners \
		$Header1(rC)  $Header1(aC)  $Header1(sC) \
		$Header1(rTL) $Header1(aTL) $Header1(sTL) \
		$Header1(rTR) $Header1(aTR) $Header1(sTR) \
		$Header1(rBR) $Header1(aBR) $Header1(sBR) \
		$Header2(rC)  $Header2(aC)  $Header2(sC) \
		$Header2(rTL) $Header2(aTL) $Header2(sTL)]
	# result should be -1 if header info bad/nonexistent

	# If description field is empty, then write the scan order
	if {[$node GetDescription] == ""} {
		$node SetDescription [$node GetScanOrder]
	}
	if {$result == 0} {
		return ""
	} else {
		return $result
	}
}
