PROJECT(coords)

ADD_LIBRARY(coords 
Decompose.c 
Mat.c
free_bs.c
write_output.c
compute_CRD.c
compute_transform.c
parse_input.c
)

TARGET_LINK_LIBRARIES (coords)

