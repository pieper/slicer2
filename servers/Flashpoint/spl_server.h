/* API for functions from GE libraries */

char *mror_imagebuf_init();
int mror_hdrdata(void *ibuf, int off, int len, void * out);
short * mror_pixdata(void *ibuf, int *x, int *y);
char * mrt_imagebuf_ptr(int off,int imageIndex);
int get_current_image_index();

int locator_set(char * in);
short locator_status();
void locator_last_xyz_nxyz_txyz(float * x, float *y, float *z,
                                float * nx, float *ny, float *nz,
                                float * tx, float *ty, float *tz);
void xyz_to_ras(float * xyz, float *ras, int patpos, int tblpos);



