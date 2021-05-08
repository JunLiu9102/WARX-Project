/**************    reference white-box implementation of WARX-16   ************/
/*******            2019.8                            ***************/
/**********     junjunll1212@gmail.com              ***********************/

#include <stdlib.h>
#include<sys/time.h>
#include <givaro/gfq.h>
#include <ctime>
#include <stdio.h>
#include <warxsbox.h>
#define round 7
#define u8 uint8_t
#define u16 uint16_t   
#define u32 uint32_t
#define block 128// message of length 2048 bytes
#define loops 100000 //  repeat 100000 times
#ifdef __GNUC__
#include <x86intrin.h>
#endif
#ifdef _MSC_VER_
#include <intrin.h>
#endif
#pragma intrinsic(__rdtsc)
uint64_t start_rdtsc()
{
	return __rdtsc();
}
uint64_t end_rdtsc()
{
	return __rdtsc();
}

using namespace Givaro;
int modulus[] = { 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }; // x^16 + x^5 + x^3 + x + 1
GFqDom<int32_t> GF216(2, 16, modulus);
u16 RM[8][8] = { // calculated by diag(1,2,3,4,5,6,7,8)* KHAZAD matrix in GF(2^16)!!!
	{0x01,0x03,0x04,0x05,0x06,0x08,0x0b,0x07},
	{0x06,0x02,0x0a,0x08,0x10,0x0c,0x0e,0x16},
	{0x0c,0x0f,0x03,0x05,0x1d,0x09,0x0a,0x18},
	{0x14,0x10,0x0c,0x04,0x1c,0x2c,0x20,0x18},
	{0x1e,0x28,0x27,0x1b,0x05,0x0f,0x14,0x11},
	{0x30,0x14,0x12,0x3a,0x0a,0x06,0x1e,0x18},
	{0x31,0x15,0x12,0x38,0x1c,0x1b,0x07,0x09},
	{0x38,0x58,0x40,0x30,0x28,0x20,0x18,0x08}
}; //

u16 invRM[8][8] = { // has been verified
	{0x0001,0x8014,0xFFE5,0xC01E,0xFFE7,0xFFE5,0xB6CA,0xA010},
    {0x0003,0x8015,0x0003,0x0001,0x555F,0x0001,0x0001,0x2004},
    {0x0004,0x8017,0xFFE6,0x400A,0xAAB9,0x7FF2,0xDB70,0x0001},
    {0x0005,0x0002,0x0001,0xC01F,0x555C,0x7FF0,0xDB72,0x400A},
    {0x0006,0x0004,0xFFE0,0x400B,0xAABB,0x8015,0x6DB9,0x600F},
    {0x0008,0x0003,0xFFE4,0x4008,0xFFE6,0x7FF3,0xB6C8,0x8015},
    {0x000B,0x8016,0x0002,0x0002,0xAABA,0x8014,0xDB71,0x2005},
    {0x0007,0x8010,0xFFE1,0x8014,0x0001,0xFFE7,0x6DB8,0xE01A}
};

/*******************   (inv)nonlinear layer of WARX16 :LUT  ****************************/
void nonlinear(u16 arr[8])
{
	for (u16 i = 0; i < 8; i++) {
		arr[i] = sbox[arr[i]];
	}
}
void invnonlinear(u16 arr[8])
{
	for (u16 i = 0; i < 8; i++) {	
		arr[i] = invsbox[arr[i]];	
	}
}
/*******************   (inv)linear layer of WARX16  ****************************/
void linear(u16 arr[8])
{
	u16 temp[8];
	for (u16 i = 0; i < 8; i++) 
	{
		GFqDom<int32_t>::Element s;
		GF216.init(s, 0);
		for (u16 k = 0; k < 8; k++)
		{
			GFqDom<int32_t>::Element a, b, c;
			GF216.init(a, RM[i][k]);   // initialize
			GF216.init(b, arr[k]);
			GF216.mul(c, a, b);   // field multiplication
			GF216.add(s, s, c);   
		}
		int32_t s_int;
		GF216.convert(s_int, s);
		temp[i] = (u16)s_int;
	}
	for (u16 i = 0; i < 8; i++) {
		arr[i] = temp[i];
	}
}
void invlinear(u16 arr[8])
{
	u16 temp[8];
	for (u16 i = 0; i < 8; i++)
	{
		GFqDom<int32_t>::Element s;
		GF216.init(s, 0);
		for (int32_t k = 0; k < 8; k++)
		{
			GFqDom<int32_t>::Element a, b, c;
			GF216.init(a, invRM[i][k]);   // initialize
			GF216.init(b, arr[k]);
			GF216.mul(c, a, b);   // field multiplication
			GF216.add(s, s, c);
		}
		int32_t s_int;
		GF216.convert(s_int, s);
		temp[i] = (u16)s_int;
	}
	for (u16 i = 0; i < 8; i++) {
		arr[i] = temp[i];
	}
}
/*******************   encryption of WARX16  ****************************/
void encryptionwhite(u16 input[block][8])
{
	for (int i = 0; i < round; i++)
	{
		for (int j = 0; j < block; j++)
		{
			nonlinear(input[j]);  // nonlinear layer
			linear(input[j]);    //linear layer
			for (int k = 0; k < 8; k++)  //affine layer
			{
				input[j][k] ^= (u16)(8 * i + k + 1);
			}	
		}
	}
}
/*******************   decryption of WARX16  ****************************/
void decryptionwhite(u16 input[block][8]) 
{	
	for (int i = round-1; i>=0; i--)
	{
		for (int j = 0; j < block; j++)
		{
			for (int k = 0; k < 8; k++)  //inverse affine layer
			{
				input[j][k] ^= (u16)(8 * i + k + 1);
			}
			invlinear(input[j]);    //inverse linear layer
			invnonlinear(input[j]);  //inverse nonlinear layer
		}
	}	
}
/*********************************************************************************************

BELOW are some useful functions!

************************************************************************************************/

/***********************calculate average of list elements ***************************/
uint64_t Average(uint64_t list[], int lenlist)
{
	uint64_t ave, sum = 0;
	for (int i = 0; i < lenlist; i++) {
		sum += list[i];
	}
	ave = sum / lenlist;
	return ave;
}
void printmessage(u16 arr[block][8] )
{
	for (int i=0;i<block;i++) 
	   printf("%04X%04X%04X%04X%04X%04X%04X%04X",
		arr[i][0], arr[i][1], arr[i][2], arr[i][3], arr[i][4], arr[i][5], arr[i][6], arr[i][7]);
}
/***********************  verify decryption   **********************************/
void verifydecryption(u16 arr1[block][8],u16 arr2[block][8])
{
	int i = memcmp(arr1, arr2, 16*block);
	if (i == 0)
		printf("VERIFY DECRYPTION CORRECT!");
	else
		printf("VERIFY DECRYPTION WRONG!");
}
/********************** generate random message (plaintext)  ************************************/
void generatemessage(u16 arr1[block][8])
{
	for (int i = 0; i < block; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			arr1[i][j] = rand();			
		}
	}
}
int main(int argc, char** argv) 
{
	srand(time(0));
	uint64_t begin; uint64_t end; uint64_t elapsed_cycles[loops];
	u16 input[block][8], temp[block][8];
	for (int k = 0; k < loops; k++)
	{	
		printf("********* loop %d results below  ************\n", k);		
		generatemessage(input);
		memcpy(temp, input, 16*block);
		encryptionwhite(input); //encrypt message
		begin = start_rdtsc();
		decryptionwhite(input);//decrypt message
		end = end_rdtsc();
		elapsed_cycles[k] = (end - begin);
		printf("\n");
		verifydecryption(input,temp);	//verify decryption result
		printf("\n");
	}
    uint64_t avecycles = Average(elapsed_cycles, loops);
	printf ("average cost %llu CPU cycles for %d tests \n",avecycles,loops);
	uint64_t cpb = avecycles/(block*16);
	printf ("average CPB for decryption is %llu \n",cpb);
	return 0;
}

