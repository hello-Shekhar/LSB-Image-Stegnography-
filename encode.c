#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
//    printf("\twidth = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
//    printf("\theight = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*defination for read and validate encode arg*/
Status read_and_validate_encode_args(char*argv[],EncodeInfo *encInfo)
{
    if(argv[2] == NULL && strstr(argv[2],".bmp") == NULL)	//checking argument3 is present and is file is .bmp or not.
    {
	printf("Its not bmp file or file not send.\n");
	printf("Usage : ./a.out -e beautiful.bmp secret.txt\n");
	return e_failure;
    }
	encInfo->src_image_fname=argv[2];
    if(argv[3] == NULL)						//checking argument2 is NULL 
    {
	printf("secret file is not given\n");
	printf("Usage : ./a.out -e beautiful.bmp secret.txt\n");
	return e_failure;
    }
	encInfo->secret_fname=argv[3];				//store in char pointer.
	strcpy((encInfo->extn_secret_file),(strstr(argv[3],".")));
    if(argv[4]!=NULL)					//checking arguments 4 is present or not.
    {
	if (strstr(argv[4],".bmp") != NULL)		//cheking file extension is  .bmp or not.
	{
	    encInfo->stego_image_fname=argv[4];		//store in stego_image_fname.
	}
	else
	{
	    printf("its not bmp file\n");
	    printf("Usage : output_image.bmp\n");
	    return e_failure;
	}
    }
    else
    {
	encInfo->stego_image_fname="stego_image.bmp";
    }
    return e_success;
}
/*functon defination for do encoding.*/
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
	printf("open file failure\n");
	return e_failure;
    }
    printf("Open file successful\t\t\t\t<~SUCCESS~>\n");
    if(check_capacity(encInfo) == e_failure)
    {
	printf("check capacity file failure\n");
	return e_failure;
    }
    printf("Check capacity successful\t\t\t<~SUCCESS~>\n");

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
	printf("copy bmp header 54 byte failure\n");
	return e_failure;
    }
    printf("Copy bmp header 54 byte successful\t\t<~SUCCESS~>\n");

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
    {
	printf("magic string failure\n");
	return e_failure;
    }
    printf("Magic string successful\t\t\t\t<~SUCCESS~>\n");

    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
	printf("failure in extn size\n");
    }
    printf("Secret file extn size encode successful\t\t<~SUCCESS~>\n");


    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
	printf("error in secret file extension\n");
	return e_failure;
    }
    printf("Extension of secret file is encoded succesful\t<~SUCCESS~>\n");

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
	printf("error in the encoded the secret file size\n");
	return e_failure;
    }
    printf("Secret file size is encoded succesful\t\t<~SUCCESS~>\n");

    if (encode_secret_file_data(encInfo) == e_failure)
    {
	printf("error in the encode the secret file data\n");
	return e_failure;
    }
    printf("Secret file data is encoded successful\t\t<~SUCCESS~>\n");

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
	printf("error in the encoding the remaining data\n");
	return e_failure;
    }
    printf("Remaining data is copied succesful\t\t<~SUCCESS~>\n");

    return e_success;
}

/*function defination for check capacity*/
Status check_capacity(EncodeInfo*encInfo)
{
    if(get_image_size_for_bmp(encInfo->fptr_src_image) >= (( 54+ (strlen(MAGIC_STRING)) + 4 + strlen(encInfo->extn_secret_file) + get_file_size(encInfo->fptr_secret))*8))
    {
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

/*file size */
uint get_file_size(FILE*fptr)		
{
    fseek(fptr,0L,SEEK_END);	//geting file size .

    return ftell(fptr);
}
/*get_image size for bmp*/
Status copy_bmp_header(FILE*fptr_src_image,FILE*fptr_stego_image)
{
    char buffarr[54];	                	//creating array of 54 byte.
    rewind(fptr_src_image);			
    rewind(fptr_stego_image);
    fread(buffarr,1,54,fptr_src_image);		//read 54 byte and store in stego_image.bmp.
    fwrite(buffarr,1,54,fptr_stego_image);
    if(ftell(fptr_stego_image) ==54)		//checking it storing or not.
    {
	return e_success;
    }else
    {
	printf("failure to copy header file\n");
	return e_failure;
    }
}
/*magic string*/
Status encode_magic_string(const char *magic_string,EncodeInfo*encInfo)
{
    if( encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)	//function call
	return e_success;
    else
	return e_failure;
}

/*encode data to image*/
Status encode_data_to_image(char*data,int size,FILE*fptr_src_image,FILE*fptr_stego_image)
{
    char buff[8];
    for(int i = 0; i < size; i++)
    {
	fread(buff, 1, 8, fptr_src_image);	//read 8 elements and store in stego_image.bmp.
	encode_byte_to_lsb(data[i], buff);	//function call.
	fwrite(buff, 1, 8, fptr_stego_image);
    }
    return e_success;
}

/*encode byte to lsb*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
	image_buffer[i] = ((data) & (1 << i)) >> i | (image_buffer[i] & ~1);  //logic for encoding byte to lsb.
    }
    return e_success;
}

/*secret file extren size*/
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];			//creating 32 size buffer.
    fread(buffer, 1, 32, fptr_src_image);
    for (int i = 0; i < 32; i++)	//encoding 32 byte in lsb in stego_imag.bmp.
    {
	buffer[i] = ((size) & (1 << i)) >> i | (buffer[i] & ~1);
    }
    fwrite(buffer, 1, 32, fptr_stego_image);
    return e_success;
}

/*secret file extren*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	return e_success;
    else
	return e_failure;
}
/*secret file size */

Status encode_secret_file_size(unsigned int file_size, EncodeInfo *encInfo)
{
    if ((encode_secret_file_extn_size(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image)) == e_success)
	return e_success;
    else
	return e_failure;
}

/*secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];		//here we encoding all secret data in output image.
    fseek(encInfo->fptr_secret,0L,SEEK_SET);	
    fread(buffer,1,encInfo->size_secret_file,encInfo->fptr_secret);
    if(encode_data_to_image(buffer, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	return e_success;
    else
	return e_failure;
}
/*copy remaining img data*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{							//here all remeaning data we store in our output image file.
    char ch[4];	
    while (fread(ch, 1, 1, fptr_src) > 0)	
    {
	fwrite(ch, 1, 1, fptr_stego);
    }
}
