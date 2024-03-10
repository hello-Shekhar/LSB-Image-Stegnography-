#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

int main(int argc,char*argv[])
{
    EncodeInfo encInfo;		
    DecodeInfo decInfo;
    uint img_size;

    if(argc == 1)		//argv 1 is present or not.
    {
	printf("file reqirements is not complete\n");
	printf("Usage : ./a.out -e beautiful.bmp secret.txt\n");
	return e_failure;
    }

    if(check_operation_type(argv) == e_encode)		//checking operation type is decoding or encoding.
    {
	if(argc <=3)		//encoding 3 argumnets is mendotory.
	{
	    printf("for encoding  pass 3 argumnets\n");
	    printf("Usage : ./a.out -e beautiful.bmp secret.txt\n");
	    return e_failure;
	}
	if(read_and_validate_encode_args(argv,&encInfo) == e_success)	//read and validate all CLA 
	{    
	    if(do_encoding(&encInfo) == e_failure)	//here we start encoding.
	    {
		printf("encoding failure\n");
	    }
		printf("\nENCODING SUCCESSFUL\t\t\t\t<~SUCCESS~>\n");
	}
    }
  else if(check_operation_type(argv) == e_decode )
    {
	if(argc <= 2)			//in decoding 2 argument is mendotory.
	{
	    printf("for encoding  pass 2 argumnets\n");
	    printf("Usage : ./a.out -d stego.bmp \n");
	    return e_failure;
	}
	if(read_and_validate_decode_args(argv,&decInfo) == e_success)	//read and validate All CLA.
	{
	    if(do_decoding(&decInfo) == e_failure)	//here we start decoding.
	    {
		printf("decoding failure\n");
	    }
	    printf("\nDECODEING SUCCSSFUL\t\t\t\t<~SUCCESS~>\n");
	}
    }
}

/* function defination for check operation type*/
OperationType check_operation_type(char *argv[])	//checking CLA is encoding or decoding.
{
	if(strcmp(argv[1],"-e")==0)
	    return e_encode;
	else if(strcmp(argv[1],"-d")==0)
	    return e_decode;
	else
	{
	    printf("Error operation type\n");
	    return e_unsupported;
	}
}
