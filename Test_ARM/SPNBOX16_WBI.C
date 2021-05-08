/**************    reference white-box implementation of SPNBOX-16   ************/
/*******            2019.12                           ***************/
/**********     junjunll1212@gmail.com              ***********************/

#include <stdlib.h>
#include<sys/time.h>
#include <givaro/gfq.h>
#include <ctime>
#include <stdio.h>
#include <spnboxsbox.h>
#include <unistd.h>
#include <testarmcycle.h>// header for counting CPU cycles on ARMv8
#define round 10
#define block 128// message with length 2048 bytes
#define loops 100000//loops 100000
#define u8 uint8_t
#define u16 uint16_t
//#include
static inline uint64_t get_cycles(void)
{
	uint32_t timelo,timehi;
	asm("rdtsc":"=a"(timelo),"=d"(timehi):);
	return (uint64_t) ( ((uint64_t)timehi) << 32 | (uint64_t)timelo);
}
using namespace Givaro;
int modulus[] = { 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }; // x^16 + x^5 + x^3 + x + 1
GFqDom<int32_t> GF216(2, 16, modulus);


/****************     MDS matrix comes from block cipher khazad    ***********************/
u16 PM[8][8] = { //khazad matrix = had(1,3,4,5,6,8,b,7)
	{0x01,0x03,0x04,0x05,0x06,0x08,0x0b,0x07},
	{0x03,0x01,0x05,0x04,0x08,0x06,0x07,0x0b},
	{0x04,0x05,0x01,0x03,0x0b,0x07,0x06,0x08},
	{0x05,0x04,0x03,0x01,0x07,0x0b,0x08,0x06},
	{0x06,0x08,0x0b,0x07,0x01,0x03,0x04,0x05},  
	{0x08,0x06,0x07,0x0b,0x03,0x01,0x05,0x04},
	{0x0b,0x07,0x06,0x08,0x04,0x05,0x01,0x03},
	{0x07,0x0b,0x08,0x06,0x05,0x04,0x03,0x01}
}; /*************************************************************************************/


/*******************   (inv)nonlinear layer of SPNBOX-16 :LUT  ****************************/
void nonlinear(u16 arr[8][1])
{
	for (u16 i = 0; i < 8; i++) {
		arr[i][0] = sbox[arr[i][0]];
	}
}
void invnonlinear(u16 arr[8][1])
{
	for (u16 i = 0; i < 8; i++) {
		arr[i][0] = invsbox[arr[i][0]];	
	}
}
/*******************   (inv)linear layer of SPNBOX-16  ****************************/
void linear(u16 arr[8][1])
{
	u16 temp[8][1];
	for (u16 i = 0; i < 8; i++)
	{
		GFqDom<int32_t>::Element s;
		GF216.init(s, 0);
		for (u16 k = 0; k < 8; k++)
		{
			GFqDom<int32_t>::Element a, b, c;
			GF216.init(a, PM[i][k]);   // initialize
			GF216.init(b, arr[k][0]);
			GF216.mul(c, a, b);   // field multiplication
			GF216.add(s, s, c);   
		}
		int32_t s_int;
		GF216.convert(s_int, s);
		temp[i][0] = (u16)s_int;
	}
	for (u16 i = 0; i < 8; i++) {
		arr[i][0] = temp[i][0];
	}
}
/*******************   encryption of SPNBOX-16  ****************************/
void encryptionwhite(u16 input[block][8][1])
{
	for (int i = 0; i < round; i++)
	{
		for (int j = 0; j < block; j++)
		{
			nonlinear(input[j]);  // nonlinear layer
			linear(input[j]);    //linear layer	
			for (int k = 0; k < 8; k++)  //affine layer
			{
				input[j][k][0] ^= (u8)(8 * i + k + 1);
			}
		}
	}
}
/*******************   decryption of SPNBOX-16  ****************************/
void decryptionwhite(u16 input[block][8][1])
{	
	for (int i = round - 1; i >= 0; i--)
	{
		for (int j = 0; j < block; j++)
		{
			for (int k = 0; k < 8; k++)  //inverse affine layer
			{
				input[j][k][0] ^= (u8)(8 * i + k + 1);
			}
			linear(input[j]);    //invlinear layer is the same as linear layer 
			invnonlinear(input[j]);  // nonlinear layer
		}
	}	
}
/*********************************************************************************************

BELOW are some useful functions!

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
void printmessage(u16 arr[block][8][1] )
{
	for (int i=0;i<block;i++) 
	   printf("%04X%04X%04X%04X%04X%04X%04X%04X",
		arr[i][0][0], arr[i][1][0], arr[i][2][0], arr[i][3][0], arr[i][4][0], arr[i][5][0], arr[i][6][0], arr[i][7][0]);
}

/***********************  verify decryption   **********************************/
void verifydecryption(u16 arr1[block][8][1], u16 arr2[block][8][1])
{
	int i = memcmp(arr1, arr2, 16);
	if (i == 0)
		printf("VERIFY DECRYPTION CORRECT!");
	else
		printf("VERIFY DECRYPTION WRONG!");
}
/********************** generate random message (plaintext)  ************************************/
void generatemessage(u16 arr1[block][8][1])
{
	for (int i = 0; i < block; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			arr1[i][j][0] = rand();
		}
	}
}

/***************************************                     main function           ***************************************************************/
int main() 
{
	
	srand(time(0));
	uint64_t begin; uint64_t end; uint64_t elapsed_cycles[loops];
	u16 input[block][8][1], temp[block][8][1];

	for (int k = 0; k < loops; k++)
	{	
		printf("********* loop %d results below  ************\n", k);
		generatemessage(input);
		memcpy(temp, input, 16);	
		encryptionwhite(input);//encrypt message
	
		begin = arch_counter_get_cntpct();
		decryptionwhite(input);//decrypt message
		end = arch_counter_get_cntpct();
		elapsed_cycles[k] = end - begin;

		printf("\n");
		verifydecryption(input,temp);	//verify decryption result
		printf("\n");
	}
	uint64_t avecycles=Average(elapsed_cycles,loops);
	printf("average cost %llu CPU cycles \n",avecycles);
	uint64_t cpb=avecycles/(block*16);
	printf("average CPB for decryption is %llu \n",cpb);
	return 0;	
}
