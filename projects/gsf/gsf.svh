/* GS channel input stream macro
 *
 *   The user aslo need to use corresponding C macro defined in gsf_is.h
 *    
 ***The following macro defines an input streaming buffer  ***
 *
 *  `define gsf_is_buf(hd,width,depthLg) defines 
 *     1. an input streaning buffer 
 *         bit[width-1:0] hd [0: (1<<depthLg-1)];
 *     2. a status method
 *         function int hd_stat(output [depthLg-1:0] n, output  [depthLg-1:0]  ptr);
 *     3. a peek method
 *         function int hd_peek(output [depthLg-1:0] n, output  [depthLg-1:0]  ptr);
 *     4. a pop method
 *         function void hd_pop(input [depthLg-1:0] m);
 * 
 *     On the C-side, the user declare interface to the buffer  by  macro: gsf_is_buf(hd) 
 *         put method: provided by infrastructure
 *              void  hd_put(unsigned* d); 
 *         put one element to the buffer. 
 *
 *         fill method: to be implemeted by user
 *              int  hd_fill(int n);    
 *                 n: buffer space availabe
 *                 return eos (1: end of stream, 0: more to continue)
 *     The both SV function hd_peek(n,ptr) and hd_stat(n, ptr) returns
 *         the stream status
 *         the total valid data (n) in the circular buffer hd[]
 *         the starting address (ptr) of valid data.
 *     The buffer status code = {eos, data_valid}
 *         0: no data but not end of stream
 *         1: data valid and not end of stream
 *         2: no data and end of stream.
 *         3: data valid and end of stream  (last data)
 *     The hd_peek will return the buffer status and call fill method
 *     The hd_peek only return the buffer status and has no side effect.
 *     The user reads the hd[] buffer directly to process data from buf. 
 *     After done with the object of size (m), the user call 
 *             hd_pop(m)
 *     to recycle the buffer for new streaming data to come.
 *
 *     The hd_peek and hd_pop has side effect on the buffer.
 *     They must be called from the same always process.
 *
 *     The hd_stat function has no side effect and used for GAP control.
 */

`ifndef AXIS
  `define gsf_is_buf_init(hd) 
`else
  `define gsf_is_buf_init(hd) \
    initial begin \
      $ixc_ctrl("gfifo", `"hd``_zyprefetch`"); \
      $ixc_ctrl("sfifo", `"hd``_put`");  \
      $ixc_ctrl("sfifo", `"hd``_zyackf`");  \
    end 
`endif
      
`define gsf_is_buf(hd,width,depthLg) \
      bit [width-1:0] hd [0 : (1 << depthLg)-1]; \
      bit [depthLg-1:0] hd``_zywptr, hd``_zyrptr; \
      bit [depthLg-1:0] hd``_zyfill;  \
      bit [depthLg-1:0] hd``_zyspace; \
      bit [depthLg-1:0]	hd``_zyreq, hd``_zyack; \
      bit 		hd``_zyeos ;  \
      function void hd``_zyackf(int eos); \
	 hd``_zyack += 1; hd``_zyeos = eos; \
      endfunction \
      function void hd``_put(input bit[width-1:0] d); \
	 hd [hd``_zywptr] = d; hd``_zywptr++; \
      endfunction \
      import "DPI-C" context function void hd``_zyprefetch(int size, int pid); \
      export "DPI-C" function hd``_put; \
      export "DPI-C" function hd``_zyackf; \
      function automatic void hd``_pop (input bit [depthLg-1:0] n); \
	 hd``_zyrptr += n; \
	 hd``_zyspace += n; \
      endfunction  \
      function automatic int hd``_stat (output bit[depthLg-1:0] sz, output bit[depthLg-1:0] ptr); \
         int xr; \
	 sz = hd``_zywptr - hd``_zyrptr; \
	 ptr = hd``_zyrptr; \
         xr[0] = (sz != 0); \
	 xr[1] = (sz == 0) && hd``_zyeos; \
	 return xr; \
      endfunction \
      function automatic int hd``_peek (output bit[depthLg-1:0] sz, output bit[depthLg-1:0] ptr); \
         int xr; \
	 xr = 0; \
	 sz = hd``_zywptr - hd``_zyrptr; \
	 ptr = hd``_zyrptr; \
         xr[0] = (sz != 0); \
	 xr[1] = (sz == 0) && hd``_zyeos; \
	 if(! hd``_zyeos) begin \
     if( hd``_zyreq == hd``_zyack ) hd``_zyspace = ~sz; \
  	    if(hd``_zyspace >= ((1 << depthLg)/8)) begin \
	      hd``_zyreq += 1; \
	      hd``_zyprefetch(hd``_zyspace, id); \
	      hd``_zyspace = 0; \
	    end \
         end \
	 return xr; \
      endfunction  \
     `gsf_is_buf_init(hd)
