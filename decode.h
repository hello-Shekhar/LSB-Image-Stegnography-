#ifndef DECODE_H
#define DECODE_H

#include "types.h" 	//contains user defined types

typedef struct _DecodeInfo
{
    /* Source image info */
    char*stego_image_fname;
    FILE*fptr_output_image;
    char*magic_data;
    char *extn_secret_file;
    char image_data[8];

    /* Decode file info */   
    int size_secret_file; 
    char*output_secret_fname;
    FILE*fptr_output_secret;

} DecodeInfo;

	/*Prototype for all Decoding functions*/
   Status read_and_validate_decode_args(char *argv[] , DecodeInfo*decInfo);

   Status do_decoding(DecodeInfo * decInfo);

   Status open_files_decode(DecodeInfo *decInfo);

   Status decode_magic_string(int size,DecodeInfo*decInfo);

   Status decode_data_to_image(int size,FILE*fptr_output_image,DecodeInfo*decInfo);

   Status decode_byte_from_lsb(char * buff , char *image_data);

   Status decode_file_extn_size (int size,FILE*fptr_output_image);

   Status decode_size_from_lsb(char *buffer,int *size);

   Status decode_secret_file_size(int file_size,DecodeInfo*decInfo);

   Status decode_secret_file_data(DecodeInfo *decInfo);

   Status decode_secret_file_extn(char * file_ext,DecodeInfo *decInfo);

   Status decode_extn_data_from_image(int size,FILE*fptr_output_image,DecodeInfo *decInfo);

#endif
