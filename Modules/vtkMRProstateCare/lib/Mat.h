#ifndef _Mat_stuff
#define _Mat_stuff

void MatTrans(double *in, double *out); /* transpose */
double MatSubDet(double * mat, int * rows, int * cols); /* det of 3x3 submatrix */
double MatDet(double * mat);  /* determinant */
void MatMinor(double * mat, double *min); /* matrix of minors */
void MatCofact(double * mat, double *cof); /* matrix of cofactors */
void MatAdjCof(double * mat, double *adj); /* transposed cofactor matrix */
double MatInv(double * mat, double *inv); /* inverse */
void MatConst(double *mat, double c); /* set mat to a constant */
void MatAdd(double *in1, double * in2, double * out); /* add matrices */
void MatDiff(double *in1, double * in2, double * out); /* subtract matrices */
void MatDot(double *in1, double * in2, double * out); /* componentwise mult */
void MatDiv(double *in1, double * in2, double * out); /* componentwise division */
double MatNorm2(double * mat);  /* sum of the squares of the elements */
void MatEye(double * mat); /* set mat to the identity */
void MatMult(double *in1, double * in2, double * out); /* multiply two matrices */
void MatCopy(double *in, double *out); /* copy in to out */
void MatScale(double *mat, double c); /* componentwise mult by a constant */

#endif
