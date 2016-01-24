#ifndef _JDATABUF_H_INCLUDED_
#define _JDATABUF_H_INCLUDED_

#include <jpeglib.h>
#include <jerror.h>

//METHODDEF(long)
long
get_jpeg_size (j_compress_ptr cinfo);

//GLOBAL(void)
void
jpeg_buff_dest (j_compress_ptr cinfo);

//METHODDEF(void)
void
jpeg_buff_free (j_compress_ptr cinfo);

typedef struct {
  struct jpeg_destination_mgr pub;

  long total_buffer_size;
  JOCTET * buffer;
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

#endif
