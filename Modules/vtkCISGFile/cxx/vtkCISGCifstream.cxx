/*=========================================================================
 
  Library   : common++
  Module    : $RCSfile: vtkCISGCifstream.cxx,v $
  Authors   : (C)opyright Daniel Rueckert and Julia Schnabel 1994-1998++
              See COPYRIGHT statement in top level directory.
  Purpose   :
  Date      : $Date: 2003/08/14 17:32:34 $
  Version   : $Revision: 1.1 $

=========================================================================*/
#define WORDS_BIGENDIAN 

#include "vtkCISGCifstream.h"

#define ENABLE_COMPRESS

#ifndef SACREDMEM
#define SACREDMEM        0
#endif

#ifndef USERMEM
# define USERMEM         450000
#endif

#ifdef USERMEM
# if USERMEM >= (433484+SACREDMEM)
#  define PBITS        16
# else
#  if USERMEM >= (229600+SACREDMEM)
#   define PBITS        15
#  else
#   if USERMEM >= (127536+SACREDMEM)
#    define PBITS        14
#   else
#    if USERMEM >= (73464+SACREDMEM)
#     define PBITS        13
#    else
#     define PBITS        12
#    endif
#   endif
#  endif
# endif
# undef USERMEM
#endif

#ifdef PBITS                
# ifndef BITS
#  define BITS PBITS
# endif 
#endif 

#if BITS == 16
# define HSIZE        69001        
#endif
#if BITS == 15
# define HSIZE        35023        
#endif
#if BITS == 14
# define HSIZE        18013        
#endif
#if BITS == 13
# define HSIZE        9001        
#endif
#if BITS <= 12
# define HSIZE        5003        
#endif

#define FIRST        257        
#define        CLEAR        256        

#define BIT_MASK        0x1f
#define BLOCK_MASK        0x80

#define INIT_BITS       9        



#define SIZEOF_CHAR    1
#define SIZEOF_UCHAR   1
#define SIZEOF_SHORT   2
#define SIZEOF_USHORT  2
#define SIZEOF_INT     4
#define SIZEOF_UINT    4
#define SIZEOF_FLOAT   4
#define SIZEOF_DOUBLE  8


#if BITS > 15
typedef long int        code_int;
#else
typedef int                code_int;
#endif

typedef long int        count_int;
typedef        unsigned char        char_type;

int n_bits;                                /* number of bits/code */
int maxbits = BITS;                        /* user settable max # bits/code */
code_int maxcode;                        /* maximum code, given n_bits */
code_int maxmaxcode = 1L << BITS;        /* should NEVER generate this code */
#ifdef COMPATIBLE                /* But wrong! */
# define MAXCODE(n_bits)        (1L << (n_bits) - 1)
#else
# define MAXCODE(n_bits)        ((1L << (n_bits)) - 1)
#endif /* COMPATIBLE */
count_int htab [HSIZE];
unsigned short codetab [HSIZE];
code_int hsize = HSIZE;                        /* for dynamic table sizing */
count_int fsize;

#define tab_suffixof(i)        ((char_type *)(htab))[i]
#define de_stack                ((char_type *)&tab_suffixof(1<<BITS))

code_int free_ent = 0;                        /* first unused entry */

int block_compress = BLOCK_MASK;
int clear_flg = 0;

char_type lmask[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};
char_type rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

void vtkCISGCifstream::Open(char *filename)
{
  this->open(filename, ios::in|ios::binary);

  // Check whether file was opened successful
  if (this->is_open() != True){
    cerr << "vtkCISGCifstream::Open: Can't open file " << filename << endl;
    exit(1);
  }

  if ((this->get() != '\037') || (this->get() != ('\235'&0xFF))){
    _compressed = True;
  } else {
    _compressed = False;
  }
  
  _swapped = swapping();
}

/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 *         stdin
 * Outputs:
 *         code or -1 is returned.
 */

code_int vtkCISGCifstream::getcode(int fflg)
{
  code_int code;
  static int offset = 0, size = 0;
  static char_type buf[BITS];
  int r_off, bits;
  char_type *bp = buf;

    if(fflg) {offset=0;size=0;}

    if ( clear_flg > 0 || offset >= size || free_ent > maxcode ) 
    {
        if ( free_ent > maxcode ) 
        {
            n_bits++;
            if ( n_bits == maxbits )
                maxcode = maxmaxcode;        /* won't get any bigger now */
            else
                maxcode = MAXCODE(n_bits);
        }
        if ( clear_flg > 0) 
        {
                maxcode = MAXCODE (n_bits = INIT_BITS);
            clear_flg = 0;
        }
        this->read(buf, n_bits);
        size = this->gcount();
        if ( size <= 0 )
            return -1;                        /* end of file */
        offset = 0;
        size = (size << 3) - (n_bits - 1);
    }

    r_off = offset;
    bits = n_bits;
    bp += (r_off >> 3);   /*------ Get to the first byte.------- */
    r_off &= 7;
    code = (*bp++ >> r_off);  /* Get first part (low order bits) */
    bits -= (8 - r_off);
    r_off = 8 - r_off;             /*---- now, offset into code word ----*/

        /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */

    if ( bits >= 8 ) 
    {
      code |= *bp++ << r_off;
      r_off += 8;
      bits -= 8;
    }
        /*------ high order bits. -------*/

    code |= (*bp & rmask[bits]) << r_off;

    offset += n_bits;

    return code;
}

int vtkCISGCifstream::CRead(char *mem, int start, int num) 
{
#ifdef ENABLE_COMPRESS

  // Read data compressed if necessary
  char_type *stackp;
  int finchar;
  code_int code, oldcode, incode;
  char *d,*de;

  // Go to beginning of file
  this->seekg(0, ios::beg);

  // Check whether file is compressed
  if((this->get()!='\037')||(this->get()!=('\235'&0xFF))){
    // Read data uncompressed
    this->seekg(start, ios::beg);
    this->read (mem, num);
    return(1==1);
  }

  // Read data compressed
  d=mem;
  de=mem+num;
  free_ent = 0;                        /* first unused entry */
  block_compress = BLOCK_MASK;
  clear_flg = 0;

  maxbits = this->get();        /* set -b from file */
  block_compress = maxbits & BLOCK_MASK;
  maxbits &= BIT_MASK;
  maxmaxcode = 1L << maxbits;

  if(maxbits > BITS) 
  {
    fprintf(stderr,"compressed with %d bits, can only handle %d bits\n",
            maxbits, BITS);
    return(1==0);
  }

    /*------- initialize the first 256 entries in the table.------*/

    maxcode = MAXCODE(n_bits = INIT_BITS);
    for(code=255;code>=0;code--) 
    {
        codetab[code] = 0;
        tab_suffixof(code) = (char_type)code;
    }
    free_ent = ((block_compress) ? FIRST : 256 );

    finchar = oldcode = this->getcode(1==1);
    if(oldcode == -1)        /* EOF already? */
        return(1==0);                        /* Get out of here */
    *d=finchar;
    start--;
    if(start<0) d++;
    stackp = de_stack;

    while ( ((code = this->getcode(1==0)) > -1)&&(d<de) ) 
    {
        if((code == CLEAR)&&block_compress) 
        {
          for(code=255;code>=0;code--)
               codetab[code] = 0;
          clear_flg = 1;
          free_ent = FIRST - 1;
          if((code=this->getcode(1==0))==-1)   /* O, untimely death! */
                return(1==0);
        }
        incode = code;

        /*--------- Special case for KwKwK string.------- */

        if(code>=free_ent)
        {
          *stackp++ = finchar;
          code = oldcode;
        }

        /*------ Generate output characters in reverse order------ */

        while(code>=256) 
        {
            *stackp++ = tab_suffixof(code);
            code = codetab[code];
        }
        *stackp++ = finchar = tab_suffixof(code);

        /*------- And put them out in forward order -------*/
        do 
        {
           *d=*--stackp;
           start--;
           if(start<0) d++;
        }  while (( stackp > de_stack )&&(d<de));

        /*-------- Generate the new entry.------- */

        if ( (code=free_ent) < maxmaxcode ) 
        {
           codetab[code] = (unsigned short)oldcode;
           tab_suffixof(code) = finchar;
           free_ent = code+1;
        }

        /*---------- Remember previous code.------- */

        oldcode = incode;
    }
  return(1==1);

#else

  // Read data uncompressed
  this->seekg(start, ios::beg);
  this->read (mem, num);
  return(1==1);

#endif
}

int vtkCISGCifstream::CReadAsChar(char *data, int length, int offset)
{
  int i= length * SIZEOF_CHAR;
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  return i;
}

int vtkCISGCifstream::CReadAsUChar(unsigned char *data, int length, int offset)
{
  int i= length * SIZEOF_UCHAR;
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  return i;
}

int vtkCISGCifstream::CReadAsShort(short *data, int length, int offset)
{
  int i= length * SIZEOF_SHORT;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  
  // Swap data
  swap16((char *)data, (char *)data, length);
  return i;
}

int vtkCISGCifstream::CReadAsUShort(unsigned short *data, int length, int offset)
{
  int i= length * SIZEOF_USHORT;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset,i);
  
  // Swap data
  swap16((char *)data, (char *)data, length);
  return i;
}

int vtkCISGCifstream::CReadAsInt(int *data, int length, int offset)
{
  int i= length * SIZEOF_INT;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  
  // Swap data
  swap32((char *)data, (char *)data, length);
  return i;
}

int vtkCISGCifstream::CReadAsUInt(unsigned int *data, int length, int offset)
{
  int i= length * SIZEOF_UINT;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  
  // Swap data
  swap32((char *)data, (char *)data, length);
  return i;
}



int vtkCISGCifstream::CReadAsFloat(float *data, int length, int offset)
{
  int i= length * SIZEOF_FLOAT;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  
  // Swap data
  swap32((char *)data, (char *)data, length);
  return i;
}

int vtkCISGCifstream::CReadAsDouble(double *data, int length, int offset)
{
  int i= length * SIZEOF_DOUBLE;
  // Read data and swap if necessary
  // Read data (possibly compressed)
  this->CRead((char *)data, offset, i);
  
  // Swap data
  swap64((char *)data, (char *)data, length);
  return i;
}


int vtkCISGCifstream::swapping() {

  short byte_order_test;
  char  *byte1, *byte2;
  byte_order_test = 256;
  byte1 = (char*)&byte_order_test;
  byte2 = byte1 + 1;
  if(*byte1 > *byte2) return 0 ;
  else return 1;
}


void vtkCISGCifstream::swap16(char *a, char *b, int n)
{
  int i;
  char c;

  if (_swapped) {
    for (i = 0; i < n * 2; i += 2){
      c = a[i];
      a[i] = b[i+1];
      b[i+1] = c;
    }
  }
}

void vtkCISGCifstream::swap32(char *a, char *b, int n)
{
  int i;
  char c;

  if (_swapped) {
    for (i = 0; i < n * 4; i += 4){
      c = a[i];
      a[i] = b[i+3];
      b[i+3] = c;
      c = a[i+1];
      a[i+1] = b[i+2];
      b[i+2] = c;
    }
  }
}

void vtkCISGCifstream::swap64(char *a, char *b, int n)
{
  int i;
  char c;

  if (_swapped) {
    for (i = 0; i < n * 8; i += 8){
      c = a[i];
      a[i] = b[i+7];
      b[i+7] = c;
      c = a[i+1];
      a[i+1] = b[i+6];
      b[i+6] = c;
      c = a[i+2];
      a[i+2] = b[i+5];
      b[i+5] = c;
      c = a[i+3];
      a[i+3] = b[i+5];
      b[i+5] = c;
    }
  }
}


