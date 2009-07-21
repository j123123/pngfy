#include "pngrw.h"
int read_png(const char *filename, char **data, unsigned int *width, unsigned int *height)
{
    int i;
    FILE *file;
    static const int PNG_SIG_SIZE = 8;
    unsigned char png_sig[PNG_SIG_SIZE];
    int sig_bytes;
    png_struct *png;
    png_info *info;
    png_uint_32 png_width, png_height;
    int depth, color_type, interlace;
    unsigned int pixel_size;
    png_byte **row_pointers;

    file = fopen (filename, "rb");
    if (file == NULL)
      return 1;

    sig_bytes = fread (png_sig, 1, PNG_SIG_SIZE, file);

    if (png_check_sig (png_sig, sig_bytes) == 0) {
      fclose (file);
      return 2;
    }
    png = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                  NULL,
                                  NULL,
                                  NULL);
    if (png == NULL) {
      fclose (file);
      return 3;
    }

    info = png_create_info_struct (png);
    if (info == NULL) {
      fclose (file);
      png_destroy_read_struct (&png, NULL, NULL);
      return 3;
    }

    png_init_io (png, file);
    png_set_sig_bytes (png, sig_bytes);

    png_read_info (png, info);
    png_get_IHDR (png, info,
                  &png_width, &png_height, &depth,
                  &color_type, &interlace, NULL, NULL);
    *width = png_width;
    *height = png_height;
    /* convert palette/gray image to rgb */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb (png);

    /* expand gray bit depth if needed */
    if (color_type == PNG_COLOR_TYPE_GRAY && depth < 8)
      png_set_gray_1_2_4_to_8 (png);

    /* transform transparency to alpha */
    if (png_get_valid(png, info, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha (png);

    if (depth == 16)
      png_set_strip_16 (png);

    if (depth < 8)
      png_set_packing (png);

    /* convert grayscale to RGB */
    if (color_type == PNG_COLOR_TYPE_GRAY
        || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb (png);

    if (interlace != PNG_INTERLACE_NONE)
      png_set_interlace_handling (png);

    png_read_update_info (png, info);

    pixel_size = 3;
    *data = (char *) malloc (png_width * png_height * pixel_size);
    if (*data == NULL) {
      fclose (file);
      return 3;
    }
    row_pointers = (png_byte **) malloc (png_height * sizeof(char *));
    for (i=0; i < png_height; i++)
      row_pointers[i] = (png_byte *) (*data + i * png_width * pixel_size);

    png_read_image (png, row_pointers);
    png_read_end (png, info);

    free (row_pointers);
    fclose (file);

    png_destroy_read_struct (&png, &info, NULL);

    return 0;
}

int write_png(const char *file_name,const unsigned char *rgba,int width,int height)
{
  FILE *fp = fopen(file_name, "wb");
  if (!fp) return 3;
  
  png_structp png_ptr = png_create_write_struct
    (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,NULL, NULL);
  if (!png_ptr){
    fclose(fp);
    return 3;
  }
  
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,
                 (png_infopp)NULL);
    fclose(fp);
    return 3;
  }

  if (setjmp(png_ptr->jmpbuf)) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 3;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height,
           8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
           PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr, info_ptr);
  png_write_flush(png_ptr);

  png_byte **row_pointers=(png_byte**)malloc(height*sizeof(png_byte*));
  int i;
  for(i=0;i<height;i++)
    row_pointers[i]=(png_byte*)(rgba+i*width*3);

  png_write_image(png_ptr,row_pointers);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
  free(row_pointers);
  return 0;
}
