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
# FILE:        MainHeader.tcl
# PROCEDURES:  
#   ReadHeaderTcl
#   ReadHeader
#   ParsePrintHeader
#   ParseM3list
#   DumpHeader
#   GetHeaderInfo
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC ReadHeaderTcl 
#
#  Opens an Image with a header and sets
#  Header(xDim yDim xSpacing ySpacing sliceThick sliceSpace  
#        littleEndian scalarSize rC aC sC rTL aTL sTL rTR aTR sTR rBR aBR sBR)
#
#  This corresponds to 
#     x_resolution y_resolution pixel_xsize pixel_ysize thick
#     space byte_order bytes_per_pixel coord_center_r coord_center_a
#     coord_center_s coord_r_top_left coord_a_top_left coord_s_top_left
#     coord_r_top_right coord_a_top_right coord_s_top_right
#     coord_r_bottom_right coord_a_bottom_right coord_s_bottom_right
#
# xDim yDim have problems
# littleEndian has problems
# bytes_per_Pixel or scalarSize has problems
# need to have error message for no header
# only handles signa and genesis
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ReadHeaderTcl { filename } {

    upvar $aHeader Header
        
    set f [open $filename r]
    
    proc getvar_from_file {filehandle offset length {ScanString ""}} {
        seek $filehandle $offset start
        set var [read $filehandle $length]
        if {[string length $ScanString] != 0} {
            binary scan $var $ScanString var2
            return $var2
        } else {
            return $var
        }
    }
    
    # define some header offsets
    set ihsize 156
    set p_suite 124
    set p_suite2 [expr $p_suite + 2]
    
    set SU_HDR_LEN 114
    set EX_HDR_LEN 1024
    set SE_HDR_LEN 1020
    set IM_HDR_LEN 1022
    
    set SU_HDR_START 0
    set EX_HDR_START $SU_HDR_LEN
    set SE_HDR_START [expr $EX_HDR_START+$EX_HDR_LEN]
    set IM_HDR_START [expr $SE_HDR_START+$SE_HDR_LEN]
    set IMG_HDR_START 0
    
    # define some variable-specific offsets
    
    set EX_ex_no 8
    set EX_patname 97
    set EX_patid 84
    set EX_ex_typ 305
    set EX_patage 122
    set EX_patsex 126
    set EX_hospname  10
    set EX_ex_desc 282
    
    set SE_se_actual_dt 16
    
    set SE_se_desc 20
    
    set MR_slthick 26
    set MR_dim_X 42
    set MR_dim_Y 46
    set MR_pixsize_X 50
    set MR_pixsize_Y 54
    set MR_dfov 34
    set MR_scanspacing 116
    set MR_loc 126
    
    # coordinate info
    
    set MR_ctr_R 130
    set MR_ctr_A 134
    set MR_ctr_S 138
    set MR_norm_R 142
    set MR_norm_A 146
    set MR_norm_S 150
    set MR_tlhc_R 154
    set MR_tlhc_A 158
    set MR_tlhc_S 162
    set MR_trhc_R 166
    set MR_trhc_A 170
    set MR_trhc_S 174
    set MR_brhc_R 178
    set MR_brhc_A 182
    set MR_brhc_S 186
    
    
    puts "print_header V1.0"
    set fs [file size  $filename]
    if {$fs > 500000} {
        set hs [expr $fs - 524288]
        puts -nonewline "header_size "
        puts $hs
        puts "x_resolution = 512"
        puts "y_resolution = 512"
        puts "bytes_per_slice = 524288"
        set aHeader(xdim) 512
        set aHeader(ydim) 512
        
    }
    if {$fs > 131072} {
        set hs [expr $fs - 131072]
        puts -nonewline "header_size = "
        puts $hs
        puts "x_resolution = 256"
        puts "y_resolution = 256"
        puts "bytes_per_slice = 131072"
        set aHeader(xdim) 256
        set aHeader(ydim) 256
    }
    set t [read $f 4]
    if {[string compare $t "IMGF"] == 0} {
        puts "image_type_text = genesis"
    } else {
        puts "image_type_text = signa"
    }
    
    set variable_header_count [getvar_from_file $f $p_suite2 2 S*]

    set exam_offset  [expr $variable_header_count + $EX_HDR_START]
    set series_offset  [expr $variable_header_count + $SE_HDR_START]
    set image_offset [expr $variable_header_count + $IM_HDR_START]
    
    #patient name
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_patname] 25]
    puts  -nonewline  "patient_name = "
    puts $var
    
    # patient id
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_patid]  13]
    puts  -nonewline  "patient_id = "
    puts $var
    
    # exam number
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_ex_no] 2 S ]
    puts  -nonewline  "exam_number = "
    puts $var
    
    #hospital
    
    set hn2 [getvar_from_file $f [expr $exam_offset+$EX_hospname] 33 a33 ]
    puts  -nonewline  "hospital_name = "
    puts $hn2
    
    #exam description
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_ex_desc]  23 a23 ]
    puts  -nonewline  "exam_description = "
    puts $var
    
    #series description
    
    set var [getvar_from_file $f [expr $series_offset+$SE_se_desc]  30 a30 ]
    puts  -nonewline  "series_description = "
    puts $var
    
    # patient age
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_patage]  2 S]
    puts  -nonewline  "patient_age = "
    puts $var
    
    # patient sex
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_patsex]  2 s ]
    if {$t == 2} {
        puts  "patient_sex = female"
    }
    if {$t == 1} {
        puts  "patient_sex = male"
    }
    
    #date 
    set var [getvar_from_file $f [expr $series_offset+$SE_se_actual_dt]  4 I ]
    set tm [clock format $var]
    puts $tm
    
    #modality
    
    set var [getvar_from_file $f [expr $exam_offset+$EX_ex_typ]  3]
    puts  -nonewline  "modality = "
    puts $var
    
    # slice thickness
    
    set var [getvar_from_file $f [expr $image_offset+$MR_slthick]  4 f ]
    puts  -nonewline  "thickness = "
    puts $var
    set aHeader(sliceThick) $var    
    
    # spacing
    
    set var [getvar_from_file $f [expr $image_offset+$MR_scanspacing]  4 f]
    puts  -nonewline  "spacing = "
    puts $var
    set aHeader(sliceSpace) $var    
    
    
    # pxel_X_size
    
    set var [getvar_from_file $f [expr $image_offset+$MR_pixsize_X]  4 f]
    puts  -nonewline  "pixel_x_size = "
    puts $var
    set aHeader(xSpacing) $var
    
    
    # pixel_Y_size
    
    set var [getvar_from_file $f [expr $image_offset+$MR_pixsize_Y]  4 f]
    puts  -nonewline  "pixel_y_size = "
    puts $var
    set aHeader(ySpacing) $var
    
    # fov
    
    set var [getvar_from_file $f [expr $image_offset+$MR_dfov]  4 f ]
    puts  -nonewline  "field_of_view = "
    puts $var
    
    # location
    
    set var [getvar_from_file $f [expr $image_offset+$MR_loc]  4 f]
    puts  -nonewline  "image_location = "
    puts $var
    
    # coordinate info
    
    set var [getvar_from_file $f [expr $image_offset+$MR_ctr_R]  4 f ]
    puts  -nonewline  "coord_center_r = "
    puts $var
    set aHeader(rC) $var

    
    set var [getvar_from_file $f [expr $image_offset+$MR_ctr_A]  4 f]
    puts  -nonewline  "coord_center_a = "
    puts $var
    set aHeader(aC) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_ctr_S]  4 f]
    puts  -nonewline  "coord_center_s = "
    puts $var
    set aHeader(sC) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_norm_R]  4 f]
    puts  -nonewline  "coord_normal_r = "
    puts $var

    set var [getvar_from_file $f [expr $image_offset+$MR_norm_A]  4 f]
    puts  -nonewline  "coord_normal_a  = "
    puts $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_norm_S]  4 f]
    puts  -nonewline  "coord_normal_s  = "
    puts $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_tlhc_R]  4 f ]
    puts  -nonewline  "coord_r_top_left  = "
    puts $var
    set aHeader(rTL) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_tlhc_A]  4 f]
    puts  -nonewline  "coord_a_top_left  = "
    puts $var
    set aHeader(aTL) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_tlhc_S]  4 f]
    binary scan  $t f var
    puts  -nonewline  "coord_s_top_left  = "
    puts $var
    set aHeader(sTL) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_trhc_R]  4 f]
    puts  -nonewline  "coord_r_top_right  = "
    puts $var
    set aHeader(rTR) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_trhc_A]  4 f]
    puts  -nonewline  "coord_a_top_right  = "
    puts $var
    set aHeader(aTR) $var

    set var [getvar_from_file $f [expr $image_offset+$MR_trhc_S] 4 f]
    puts  -nonewline  "coord_s_top_right  = "
    puts $var
    set aHeader(sTR) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_brhc_R] 4 f ]
    puts  -nonewline  "coord_r_bottom_right  = "
    puts $var
    set aHeader(rBR) $var

    set var [getvar_from_file $f [expr $image_offset+$MR_brhc_A]  4 f]
    puts  -nonewline  "coord_a_bottom_right  = "
    puts $var
    set aHeader(aBR) $var
    
    set var [getvar_from_file $f [expr $image_offset+$MR_brhc_S]  4 f]
    binary scan  $t f var
    puts  -nonewline  "coord_s_bottom_right  = "
    puts $var
    set aHeader(sBR) $var
}
#-------------------------------------------------------------------------------
# .PROC ReadHeader
#
# Calls the print_header program.
# This routine is obsolete. It exists only
# for the possible eventuality that ReadHeaderTCl does
# not handle some type of file.
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ReadHeader {image run utility tk} {
    global Gui
	# Run a header reading utility
	if {$run == 1} {
		if {[file exists $utility] == 0 } {
		    puts "ReadHeader: print_header program not found."
		    return ""
		}
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
                        #puts $errmsg
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
# Parses result from the print_header program
# This routine is obsolete. It exists only
# for the possible eventuality that ReadHeaderTCl does
# not handle some type of file.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ParsePrintHeader {text aHeader} {
	
	upvar $aHeader Header
	set text "$text\n"

	# These are comments
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
# 
# .ARGS
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
# 
# .ARGS
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
# Return an error message if files don't exist, else empty list.
# .ARGS
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

	# exit if failed to read
	if {$hdr1 == ""} {
	    return "-1"
	}

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
