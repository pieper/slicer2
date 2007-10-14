/**
 * File:          $RCSfile: matrix_test_noc.c,v $
 * Module:        Fixed size vector test functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
 */

static Gan_Bool TEST_SMALL_MATRIX(void)
{
   GAN_MATTYPE A, B, C;

#ifdef GAN_MATTYPEL   
   GAN_MATTYPEL Al;
   GAN_MATTYPER Ar;

   GAN_GENMAT_TYPE *Am, *Bm, *Cm;
   GAN_GENSQUMAT_TYPE *Sm, *Tm;
#else
   GAN_GENSQUMAT_TYPE *Am, *Bm;
#endif

   GAN_GENVEC_TYPE *av, *bv;
   GAN_VECTYPE1 a, c;
   GAN_VECTYPE2 b;

#ifdef GAN_MATT_MULTV_Q
   GAN_VECTYPE2 d;
#endif   

#ifdef GAN_MAT_RMULTS_Q
   GAN_SQUMATTYPEL Sl, Tl;
   GAN_SQUMATTYPER Sr, Tr;
#endif

   GAN_REALTYPE s, t;

   /* allocate matrices/vectors */
#ifdef GAN_MATTYPEL
   Am = GAN_MATRIX_ALLOC(0,0); cu_assert ( Am != NULL );
   Bm = GAN_MATRIX_ALLOC(0,0); cu_assert ( Bm != NULL );
   Cm = GAN_MATRIX_ALLOC(0,0); cu_assert ( Cm != NULL );
   Sm = GAN_SYMMATRIX_ALLOC(0); cu_assert ( Sm != NULL );
   Tm = GAN_SYMMATRIX_ALLOC(0); cu_assert ( Tm != NULL );
#else
   Am = GAN_SYMMATRIX_ALLOC(0); cu_assert ( Am != NULL );
   Bm = GAN_SYMMATRIX_ALLOC(0); cu_assert ( Bm != NULL );
#endif
   av = GAN_VECTOR_ALLOC(0); cu_assert ( av != NULL );
   bv = GAN_VECTOR_ALLOC(0); cu_assert ( bv != NULL );
   
#ifdef GAN_MAT_ZERO_S
   /* test setting to zero */
   A = GAN_MAT_ZERO_S();
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_SQUMAT_IDENT_Q
   /* test matrix identity */
   Am = GAN_MATRIX_IDENT ( Am, MAT_ROWS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   (void)GAN_SQUMAT_IDENT_Q(&B);
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_COPY_Q
   /* test matrix copy */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   (void)GAN_MAT_COPY_Q ( &A, &B );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_SCALE_Q
   /* test matrix scaling */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATRIX_SCALE_Q ( Am, (GAN_REALTYPE)3.0, Am ) != NULL );
   (void)GAN_MAT_SCALE_Q ( &A, (GAN_REALTYPE)3.0, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_DIVIDE_Q
   /* test matrix division */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATRIX_DIVIDE_Q ( Am, (GAN_REALTYPE)3.0, Am ) != NULL );
   (void)GAN_MAT_DIVIDE_Q ( &A, (GAN_REALTYPE)3.0, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_NEGATE_Q
   /* test matrix negation */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATRIX_NEGATE_Q ( Am, Am ) != NULL );
   (void)GAN_MAT_NEGATE_Q ( &A, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_UNIT_Q
   /* test matrix scaling to unit F-norm */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATRIX_DIVIDE_Q ( Am, GAN_MATRIX_FNORM(Am), Am ) != NULL );
   (void)GAN_MAT_UNIT_Q ( &A, &B );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   (void)GAN_MAT_SUB_Q ( &B, &A, &B );
   cu_assert ( GAN_MAT_FNORM_S(&B) < MAT_THRESHOLD );
#endif   

#ifdef GAN_MAT_ADD_Q
   /* test matrix addition */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL || Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_MATRIX_ADD_Q ( Am, Bm, Am ) != NULL );
   (void)GAN_MAT_ADD_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_SUB_Q
   /* test matrix subtraction */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL || Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_MATRIX_SUB_Q ( Am, Bm, Am ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_VEC_OUTER_Q
   /* test vector outer product */
   av = GAN_VECTOR_RAND ( av, MAT_ROWS );
   bv = GAN_VECTOR_RAND ( bv, MAT_COLS );
   cu_assert ( av != NULL || bv != NULL );
   Am = GAN_VECTOR_OUTER_Q ( av, bv, Am );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_VEC_FROM_VEC1_Q ( av, &a ) != NULL );
   cu_assert ( GAN_VEC_FROM_VEC2_Q ( bv, &b ) != NULL );
   (void)GAN_VEC_OUTER_Q ( &a, &b, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_MULTV_Q
   /* test matrix/vector multiplication */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   bv = GAN_VECTOR_RAND ( bv, MAT_COLS );
   cu_assert ( Am != NULL || bv != NULL );
   av = GAN_MATRIX_MULTV_Q ( Am, bv, av );
   cu_assert ( av != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_VEC_FROM_VEC2_Q ( bv, &b ) != NULL );
   cu_assert ( GAN_VEC_FROM_VEC1_Q ( av, &a ) != NULL );
   (void)GAN_MAT_MULTV_Q ( &A, &b, &c );
   (void)GAN_VEC_SUB1_Q ( &c, &a, &c );
   cu_assert ( GAN_VEC_SQRLEN1_S(&c) < MAT_THRESHOLD );
#endif   

#ifdef GAN_MATT_MULTV_Q
   /* test matrix/vector multiplication with transpose */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   av = GAN_VECTOR_RAND ( av, MAT_ROWS );
   cu_assert ( Am != NULL || av != NULL );
   bv = GAN_MATRIXT_MULTV_Q ( Am, av, bv );
   cu_assert ( bv != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_VEC_FROM_VEC1_Q ( av, &a ) != NULL );
   cu_assert ( GAN_VEC_FROM_VEC2_Q ( bv, &b ) != NULL );
   (void)GAN_MATT_MULTV_Q ( &A, &a, &d );
   (void)GAN_VEC_SUB2_Q ( &d, &b, &d );
   cu_assert ( GAN_VEC_SQRLEN2_S(&d) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_LMULTM_Q
   /* test left matrix multiplication by matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Bm != NULL );
   Cm = GAN_MATRIX_RMULT_GEN ( Bm, GAN_NOTRANSPOSE, Am, GAN_NOTRANSPOSE, Cm );
   cu_assert ( Cm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATL_FROM_MAT_Q ( Bm, &Al ) != NULL );
   (void)GAN_MAT_LMULTM_Q ( &A, &Al, &B );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Cm, &A ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif /* #ifdef GAN_MAT_LMULTM_Q */

#ifdef GAN_MAT_LMULTMT_Q
   /* test left matrix multiplication by matrix transpose */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Bm != NULL );
   Cm = GAN_MATRIX_RMULT_GEN ( Bm, GAN_TRANSPOSE, Am, GAN_NOTRANSPOSE, Cm );
   cu_assert ( Cm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATL_FROM_MAT_Q ( Bm, &Al ) != NULL );
   (void)GAN_MAT_LMULTMT_Q ( &A, &Al, &B );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Cm, &A ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_RMULTM_Q
   /* test right matrix multiplication by matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Bm != NULL );
   Cm = GAN_MATRIX_RMULT_GEN ( Am, GAN_NOTRANSPOSE, Bm, GAN_NOTRANSPOSE, Cm );
   cu_assert ( Cm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATR_FROM_MAT_Q ( Bm, &Ar ) != NULL );
   (void)GAN_MAT_RMULTM_Q ( &A, &Ar, &B );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Cm, &A ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTMT_Q
   /* test right matrix multiplication by matrix transpose */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Bm != NULL );
   Cm = GAN_MATRIX_RMULT_GEN ( Am, GAN_NOTRANSPOSE, Bm, GAN_TRANSPOSE, Cm );
   cu_assert ( Cm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATR_FROM_MAT_Q ( Bm, &Ar ) != NULL );
   (void)GAN_MAT_RMULTMT_Q ( &A, &Ar, &B );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Cm, &A ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_LMULTMT_SYM_Q
   /* test left matrix multiplication by transpose producing a symmetric
      matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   Bm = GAN_MATRIX_COPY_Q ( Am, Bm );
   cu_assert ( Bm != NULL );
   Sm = SYMMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Sm != NULL );
   Cm = Sm->funcs->rmult ( Sm, GAN_NOTRANSPOSE, GAN_NOINVERT,
                           Am, GAN_NOTRANSPOSE, Cm );
   cu_assert ( Cm != NULL );
   Sm = GAN_MATRIX_RMULT_SYM_GEN ( Bm, GAN_TRANSPOSE, Am, GAN_NOTRANSPOSE,
                                   Sm );
   cu_assert ( Sm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   (void)GAN_MAT_LMULTMT_SYM_Q ( &A, &B, &Sr );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Tr ) != NULL );
   (void)GAN_SYMMATR_SUB_Q ( &Sr, &Tr, &Sr );
   cu_assert ( GAN_SYMMATR_FNORM_S(&Sr) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTMT_SYM_Q
   /* test right matrix multiplication by transpose producing a symmetric
      matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   Bm = GAN_MATRIX_COPY_Q ( Am, Bm );
   cu_assert ( Bm != NULL );
   Sm = SYMMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Sm != NULL );
   Cm = MATRIX_SQUMATRIX_MULT ( Am, Sm, Cm );
   cu_assert ( Cm != NULL );
   Sm = GAN_MATRIX_RMULT_SYM_GEN ( Bm, GAN_NOTRANSPOSE, Am, GAN_TRANSPOSE,
                                   Sm );
   cu_assert ( Sm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   (void)GAN_MAT_RMULTMT_SYM_Q ( &A, &B, &Sl );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Tl ) != NULL );
   (void)GAN_SYMMATL_SUB_Q ( &Sl, &Tl, &Sl );
   cu_assert ( GAN_SYMMATL_FNORM_S(&Sl) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_MAT_LMULTS_Q
   /* test left matrix multiplication by symmetric matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = SYMMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = SQUMATRIX_MATRIX_MULT ( Sm, Am, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   (void)GAN_MAT_LMULTS_Q ( &A, &Sl, &C );
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTS_Q
   /* test right matrix multiplication by symmetric matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = SYMMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = MATRIX_SQUMATRIX_MULT ( Am, Sm, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   (void)GAN_MAT_RMULTS_Q ( &A, &Sr, &C );
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

#ifdef GAN_SYMMATR_LRMULT_Q
   /* test multiplication of symmetric matrix on left and right by matrix
      and its transpose respectively */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_COLS, MAT_ROWS );
   Sm = SYMMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Bm != NULL || Sm != NULL );
   Tm = Sm->funcs->lrmult ( Sm, Am, GAN_NOTRANSPOSE, Bm, Tm );
   cu_assert ( Tm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MATRIX_TPOSE_Q ( Bm, Am ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Tm, &Tl ) != NULL );
   (void)GAN_SYMMATR_LRMULT_Q ( &Sr, &A, &C, &Sl );
   (void)GAN_SYMMATL_SUB_Q ( &Sl, &Tl, &Sl );
   cu_assert ( GAN_SYMMATL_FNORM_S(&Sl) < MAT_THRESHOLD );
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif   

#ifdef GAN_SYMMATL_LRMULT_Q
   /* test multiplication of symmetric matrix on left and right by matrix
      transpose and matrix respectively */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_RAND ( Bm, MAT_ROWS, MAT_COLS );
   Sm = SYMMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Bm != NULL || Sm != NULL );
   Tm = Sm->funcs->lrmult ( Sm, Am, GAN_TRANSPOSE, Bm, Tm );
   cu_assert ( Tm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Tm, &Tr ) != NULL );
   (void)GAN_SYMMATL_LRMULT_Q ( &Sl, &A, &C, &Sr );
   (void)GAN_SYMMATR_SUB_Q ( &Sr, &Tr, &Sr );
   cu_assert ( GAN_SYMMATR_FNORM_S(&Sr) < MAT_THRESHOLD );
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_LMULTL_Q
   /* test multiplication of matrix by lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->rmult ( Sm, GAN_NOTRANSPOSE, GAN_NOINVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   (void)GAN_MAT_LMULTL_Q ( &A, &Sl, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_LMULTLI_Q
   /* test multiplication of matrix by inverse lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->rmult ( Sm, GAN_NOTRANSPOSE, GAN_INVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   (void)GAN_MAT_LMULTLI_Q ( &A, &Sl, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_LMULTLT_Q
   /* test multiplication of matrix by upper triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->rmult ( Sm, GAN_TRANSPOSE, GAN_NOINVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   (void)GAN_MAT_LMULTLT_Q ( &A, &Sl, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_LMULTLIT_Q
   /* test multiplication of matrix by inverse lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_ROWS, MAT_ROWS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->rmult ( Sm, GAN_TRANSPOSE, GAN_INVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATL_FROM_SQUMAT_Q ( Sm, &Sl ) != NULL );
   (void)GAN_MAT_LMULTLIT_Q ( &A, &Sl, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTL_Q
   /* test multiplication of matrix by lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->lmult ( Sm, GAN_NOTRANSPOSE, GAN_NOINVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   (void)GAN_MAT_RMULTL_Q ( &A, &Sr, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTLI_Q
   /* test multiplication of matrix by inverse lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->lmult ( Sm, GAN_NOTRANSPOSE, GAN_INVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   (void)GAN_MAT_RMULTLI_Q ( &A, &Sr, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTLT_Q
   /* test multiplication of matrix by upper triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->lmult ( Sm, GAN_TRANSPOSE, GAN_NOINVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   (void)GAN_MAT_RMULTLT_Q ( &A, &Sr, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_RMULTLIT_Q
   /* test multiplication of matrix by inverse lower triangular matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Sm = LTMATRIX_RAND ( Sm, MAT_COLS, MAT_COLS );
   cu_assert ( Am != NULL || Sm != NULL );
   Bm = Sm->funcs->lmult ( Sm, GAN_TRANSPOSE, GAN_INVERT,
                           Am, GAN_NOTRANSPOSE, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Sr ) != NULL );
   (void)GAN_MAT_RMULTLIT_Q ( &A, &Sr, &A );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_SUMSQR_S
   /* test calculation of sum of squares of matrix elements */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   s = GAN_MATRIX_SUMSQR(Am);
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   t = GAN_MAT_SUMSQR_S(&A);
   cu_assert ( fabs(s-t) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_FNORM_S
   /* test calculation of Frobenius norm of matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   s = GAN_MATRIX_FNORM(Am);
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   t = GAN_MAT_FNORM_S(&A);
   cu_assert ( fabs(s-t) < MAT_THRESHOLD );
#endif

#ifdef GAN_SQUMAT_IDENT_Q
   /* test matrix identity */
   Am = GAN_MATRIX_IDENT ( Am, MAT_ROWS );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   (void)GAN_SQUMAT_IDENT_Q(&B);
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif
   
#ifdef GAN_SQUMAT_TPOSE_Q
   /* test matrix transpose */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   Bm = GAN_MATRIX_TPOSE_Q ( Am, Bm );
   cu_assert ( Bm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   (void)GAN_SQUMAT_TPOSE_Q(&A,&A);
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   (void)GAN_SQUMAT_TPOSE_Q(&A,&C);
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

#ifdef GAN_SQUMAT_INVERT_Q
   /* test matrix inversion */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_MATRIX_INVERT_Q ( Am, Bm );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   (void)GAN_SQUMAT_INVERT_Q ( &A, &C );
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

#ifdef GAN_MAT_SADDT_Q
   /* test matrix addition with own transpose, producing symmetric matrix */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   cu_assert ( Am != NULL );
   Sm = GAN_MATRIX_ADD_SYM_GEN ( Am, GAN_NOTRANSPOSE, Am, GAN_TRANSPOSE, Sm );
   cu_assert ( Sm != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_SQUMATR_FROM_SQUMAT_Q ( Sm, &Tr ) != NULL );
   (void)GAN_MAT_SADDT_Q ( &A, &Sr );
   (void)GAN_SYMMATR_SUB_Q ( &Sr, &Tr, &Sr );
   cu_assert ( GAN_SYMMATR_FNORM_S(&Sr) < MAT_THRESHOLD );
#endif

#ifdef GAN_VEC_OUTER_SYM_Q
   /* test outer product of a vector with itself */
   av = GAN_VECTOR_RAND ( av, MAT_ROWS );
   cu_assert ( av != NULL );
   Am = GAN_VECTOR_OUTER_SYM_Q ( av, Am );
   cu_assert ( Am != NULL );
   cu_assert ( GAN_VEC_FROM_VEC1_Q ( av, &a ) != NULL );
   (void)GAN_VEC_OUTER_SYM_Q ( &a, &A );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &B ) != NULL );
   (void)GAN_MAT_SUB_Q ( &A, &B, &A );
   cu_assert ( GAN_MAT_FNORM_S(&A) < MAT_THRESHOLD );
#endif

#ifdef GAN_SYMMAT_CHOLESKY_Q
   /* test matrix Cholesky factorisation */
   Am = GAN_MATRIX_RAND ( Am, MAT_ROWS, MAT_COLS );
   Bm = GAN_SQUMATRIX_CHOLESKY_Q ( Am, Bm );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Am, &A ) != NULL );
   cu_assert ( GAN_MAT_FROM_MAT_Q ( Bm, &B ) != NULL );
   (void)GAN_SYMMAT_CHOLESKY_Q ( &A, &C );
   (void)GAN_LTMAT_SRMULTT_I(&B);
   (void)GAN_LTMAT_SRMULTT_I(&C);
   (void)GAN_MAT_SUB_Q ( &C, &B, &C );
   cu_assert ( GAN_MAT_FNORM_S(&C) < MAT_THRESHOLD );
#endif

   /* free matrices and vectors */
   GAN_VECTOR_FREE ( bv );
   GAN_VECTOR_FREE ( av );
#ifdef GAN_MATTYPEL
   GAN_SQUMATRIX_FREE ( Tm );
   GAN_SQUMATRIX_FREE ( Sm );
   GAN_MATRIX_FREE ( Cm );
   GAN_MATRIX_FREE ( Bm );
   GAN_MATRIX_FREE ( Am );
#else
   GAN_SQUMATRIX_FREE ( Am );
   GAN_SQUMATRIX_FREE ( Bm );
#endif

   return GAN_TRUE;
}
