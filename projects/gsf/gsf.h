 /*
 * GSFIFO input stream declration macro
 *   See spec in gsf_is.svh
 */

#ifndef GSF_IS_H
#define GSF_IS_H

#define gsf_is_buf_int(hd)		\
  extern int hd ## _fill (int, int);	\
  extern void hd ## _put  (unsigned *);			\
  extern void hd ## _zyackf  (int);				\
  void hd ##  _zyprefetch(int n, int pid){					\
    int eos;								\
     eos = hd ## _fill(n, pid);						\
     hd ## _zyackf(eos);						\
  }

#define gsf_sbuf_int(hd)		\
  extern void hd ## _put  (unsigned *);			


#define gsf_is_func_int(fn,width) \
  extern int fn (unsigned *);	  \
  extern void fn ## _zyputf  (unsigned *);  \
  extern void fn ## _zyackf  (int);	  \
  void fn ## _zyprefetch(int n){	  \
    int k, r, eos;		  \
    unsigned d[(width+31)/32];	  \
    for(k=0; k < n; k++){	  \
      r = fn (d);		  \
      if(r & 1){		  \
	fn ## _zyputf(d);		  \
      }				  \
      if(r != 1) break;		  \
    }				  \
    fn ## _zyackf((r>>1)); \
  }

#define gsf_is_func2_int(fn,width) \
  extern int fn (int, unsigned *);	  \
  extern void fn ## _zyputf  (unsigned *);  \
  extern void fn ## _zyackf  (int);	  \
  void fn ## _zyprefetch(int hd, int n){	  \
    int k, r, eos;		  \
    unsigned d[(width+31)/32];	  \
    for(k=0; k < n; k++){	  \
      r = fn (hd, d);		  \
      if(r & 1){		  \
	fn ## _zyputf(d);		  \
      }				  \
      if(r != 1) break;		  \
    }				  \
    fn ## _zyackf((r>>1)); \
  }



#ifdef __cplusplus
#define gsf_is_buf(hd) extern "C" { gsf_is_buf_int(hd) }
#define gsf_sbuf(hd) extern "C" { gsf_sbuf_int(hd) }
#define gsf_is_func(fn,width) extern "C" { gsf_is_func_int(fn,width) }
#define gsf_is_func2(fn,width) extern "C" { gsf_is_func2_int(fn,width) }
#else 
#define gsf_is_buf(hd) gsf_is_buf_int(hd)
#define gsf_sbuf(hd)   gsf_sbuf_int(hd)
#define gsf_is_func(fn,width) gsf_is_func_int(fn,width)
#define gsf_is_func2(fn,width) gsf_is_func2_int(fn,width)
#endif


#endif /* GSF_IS_H */

