#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_output_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_output_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }
    // Secret file
    decInfo->fptr_output_secret = fopen(decInfo->output_secret_fname, "w");
    // Do Error handling
    if(decInfo->fptr_output_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_secret_fname);

	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") == NULL)	//checking if CLA 2 is exist or not .if exist than .bmp file or not.
    {
	printf("2st argument is not bmp file\n");	
	printf("Usage : ./a.out -d stego_image.bmp \n");	//printing error 
	return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)				//checking if CLA 3 is .txt file is exist or not .
    {
	if (strstr(argv[3], ".txt") != NULL)
	{
	    decInfo->output_secret_fname = argv[3];
	}
	else
	{
	    printf("argv 3 is not a .txt file\n");	//if not exist than error .
	    return e_failure;
	}
    }

    if (argv[3] == NULL)
    {
	decInfo->output_secret_fname = "output.txt";	//we assign 3rd CLA as output.txt.
    }

    return e_success;
}

/*function defination for decoding */
Status do_decoding(DecodeInfo*decInfo)
{
    if(open_files_decode(decInfo) ==e_failure)		
    {
	printf("open file failure\n");
	return e_failure;
    }
    printf("Open file success\t\t\t\t<~SUCCESS~>\n");
    if(decode_magic_string(strlen(MAGIC_STRING),decInfo)==e_failure)
    {
	printf("magic string decode failure\n");
	return e_failure;
    }
    printf("magic string decode success\t\t\t<~SUCCESS~>\n");
    if (decode_file_extn_size(strlen(".txt"), decInfo->fptr_output_image) ==e_failure)
    {
	printf("Decoded file extension size fail\n");
	return e_failure;
    }
    printf("Decoded file extension size success\t\t<~SUCCESS~>\n");
    if (decode_secret_file_extn(decInfo->extn_secret_file, decInfo) == e_failure)
    {
	printf("Decoded Secret File Extension fail\n");
	return e_failure;
    }
    printf("Decoded Secret File Extension success\t\t<~SUCCESS~>\n");
    if (decode_secret_file_size(decInfo->size_secret_file, decInfo) == e_failure)
    {
	printf("Decoded secret file size fail\n");
	return e_failure;
    }
    printf("Decoded secret file size sucess\t\t\t<~SUCCESS~>\n");
    if (decode_secret_file_data(decInfo) == e_failure)
    {
	printf("Decoded secret file data fail\n");
	return e_failure;
    }
    printf("Decoded secret file data success\t\t<~SUCCESS~>\n");
    return e_success;
}


// Function definition decode magic string
Status decode_magic_string(int size,DecodeInfo*decInfo)
{
    rewind (decInfo->fptr_output_image);
    fseek(decInfo->fptr_output_image,54,SEEK_SET);		//set FILE pointer in 54th position.
    decInfo->magic_data = malloc(size+1);			//allocate memory for magic data.
    decode_data_to_image(size,decInfo->fptr_output_image,decInfo);	//function call data_to_image.
    decInfo->magic_data[size] ='\0';				//at last store null charater .
    if(strcmp(decInfo->magic_data, MAGIC_STRING)==0)		//checking magic data and MAGIC STRING are same .
	return e_success;
    else
	return e_failure;
}

// Function definition decode data from image
Status decode_data_to_image(int size,FILE*fptr_output_image,DecodeInfo*decInfo)
{
    char buff[8];		//creating array with size 8.
    for(int i=0;i<size;i++)		
    {
	fread (buff,8,1,fptr_output_image);			//read 8 elements from FILE pointer.
	decode_byte_from_lsb(&decInfo->magic_data[i],buff);	//function call for byte to lsb.
    }
    return e_success;
}

// Function definition decode byte from lsb
Status decode_byte_from_lsb(char *dataa,char*magic_buffer)
{
    unsigned char ch =0 ;	
    int i,num=7,get;			
    for(i=7;i>=0;i--)			//here we are fetching byte from lsb and convert to charater.		
    {
	get = magic_buffer[i] & 0x01;
	ch = (get << num--)| ch;
    }
    *dataa=ch;
    return e_success;
}

// Function definition decode file_extn_size
Status decode_file_extn_size(int size, FILE *fptr_output_image)
{
    char arr[32];
    int value;
    fread(arr, 32,1, fptr_output_image); //read 32 elements from output file.
    decode_size_from_lsb(arr, &value);	//function call

    if (value == size)			
	return e_success;
    else
	return e_failure;
}

// Function definition decode size from lsb
Status decode_size_from_lsb(char *buffer, int *size)
{
    int num = 0x00;
    for(int i=31;i>=0;i--)
    {
	num = ((buffer[i] & 0x01 ) <<i)| num;	//fetching size from lsb and covert to integer.
    }
    *size =num;		
}

// Function definition for decode secret file extn
Status decode_secret_file_extn(char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt";
    int i = strlen(file_ext);		//string size for file extention.
    decInfo->extn_secret_file = malloc(i + 1);	//memeory allocation for extn_secret_file.
    decode_extn_data_from_image(strlen(file_ext), decInfo->fptr_output_image, decInfo);	//function call

    decInfo->extn_secret_file[i] = '\0';	//at last we store NULL charater .

    if (strcmp(decInfo->extn_secret_file, file_ext) == 0)	//string comparition.
	return e_success;
    else
	return e_failure;
}

// Function definition decode extension data from image
Status decode_extn_data_from_image(int size, FILE *fptr_output_image, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)		
    {
	fread(decInfo->stego_image_fname, 8, 1, fptr_output_image);	//read 8 elements form FILE pointer.
	decode_byte_from_lsb(&decInfo->extn_secret_file[i], decInfo->stego_image_fname);	//function call
    }
    return e_success;
}

// Function definition for decode secret file size
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];	//declare array.
    fread(str, 32,1, decInfo->fptr_output_image);	//read 32 elements of 1 byte from file pointer of output image .
  
    decode_size_from_lsb(str, &file_size);		//function call
  
    decInfo->size_secret_file = file_size;		//file size value store in size secret file .

    return e_success;
}

// Function definition for decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
	fread (decInfo->stego_image_fname, 8, 1, decInfo->fptr_output_image);		//read 8 byte of data from output image file pointer.
	decode_byte_from_lsb(&ch, decInfo->stego_image_fname);				//function call
	fputc(ch,decInfo->fptr_output_secret);						//at last we storing secret message in FILE.
    }
    return e_success;
}
