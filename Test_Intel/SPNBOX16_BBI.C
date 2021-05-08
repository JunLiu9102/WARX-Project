/**************    reference black-box implementation of SPNBOX-16    ************/
/*******            2019.12                          ***************/
/**********     junjunll1212@gmail.com            ***********************/
  
#include <stdlib.h>
#include<sys/time.h>
#include <givaro/gfq.h>
#include <ctime>
#include <stdio.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include<vector>
#include <aessbox.h>
#define u8 uint8_t
#define u16 uint16_t   
#define u32 uint32_t
#define roundaes16 32
#define round 10
#define block 128// message of length 2048 bytes
#define loops 100000 //  repeat 100000 times
static	u16 input[block][8][1],temp[block][8][1];
static	u8 rk[roundaes16 + 1][2];
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
int modulus[] = { 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }; // x^16 + x^5 + x^3 + x + 1 (1,002b)
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
/**************       AES16  ***************************/

using namespace Givaro;
int modulus2[] = { 1, 1, 0, 1, 1, 0, 0, 0, 1 }; // x^8 + x^4 + x^3 + x + 1 (AES) (right is high-order)
GFqDom<int32_t> GF28(2, 8, modulus2);

/***************    key generation of AES-16   *********************/
void AES16KeySchedule(u8 rk[roundaes16 + 1][2])
{
	u8 i;
	for (i = 0; i < roundaes16 + 1; ++i)
	{
		//rk[i] = rand();
		rk[i][0] = i; //ignore KDF, we fix the key
		rk[i][1] = i + 1;
	}
}
/***************    round function of AES-16   *********************/
void AES16Roundfunc(u8* arrc1, u8* arrc0, u8 arrrk[])
{
	/********      Subbyte       **************/
	*arrc1 = aessbox[*arrc1]; *arrc0 = aessbox[*arrc0];
	/***********    Mixcolumn  *****************/// MC = [02, 01; 03, 02];
	GFqDom<int32_t>::Element s1, s0, a1, b1, c1, a2, b2, c2;
	GF28.init(s1, 0); GF28.init(s0, 0);

	GF28.init(a1, 0x02);  GF28.init(a2, 0x01); // initialize
	GF28.init(b1, *arrc1); GF28.init(b2, *arrc0);
	GF28.mul(c1, a1, b1); GF28.mul(c2, a2, b2);  // field multiplication
	GF28.add(s1, c1, c2);   //0x02*arrc1+0x01*arrc0
	int32_t s1_int;
	GF28.convert(s1_int, s1);

	GF28.init(a1, 0x03);  GF28.init(a2, 0x02); // initialize
	GF28.init(b1, *arrc1); GF28.init(b2, *arrc0);
	GF28.mul(c1, a1, b1); GF28.mul(c2, a2, b2);  // field multiplication
	GF28.add(s0, c1, c2);   //0x03*arrc1+0x02*arrc0
	int32_t s0_int;
	GF28.convert(s0_int, s0);
	*arrc1 = (u8)s1_int;
	*arrc0 = (u8)s0_int;
	/*********      Addroundkey         ***************/
	*arrc1 ^= arrrk[1]; *arrc0 ^= arrrk[0];
}
/***************    inverse round function of AES-16   *********************/
void AES16invRoundfunc(u8* arrc1, u8* arrc0, u8 arrrk[])
{
	/*********      Addroundkey         ****************/
	*arrc1 ^= arrrk[1]; *arrc0 ^= arrrk[0];

	/***********    Mixcolumn  ******************/ //invMC = [b9, d1; 68, b9];
	GFqDom<int32_t>::Element s1, s0, a1, b1, c1, a2, b2, c2;
	GF28.init(s1, 0); GF28.init(s0, 0);

	GF28.init(a1, 0xb9);  GF28.init(a2, 0xd1); // initialize
	GF28.init(b1, *arrc1); GF28.init(b2, *arrc0);
	GF28.mul(c1, a1, b1); GF28.mul(c2, a2, b2);  // field multiplication
	GF28.add(s1, c1, c2);   //0xb9*arrc1+0xd1*arrc0
	int32_t s1_int;
	GF28.convert(s1_int, s1);

	GF28.init(a1, 0x68);  GF28.init(a2, 0xb9); // initialize
	GF28.init(b1, *arrc1); GF28.init(b2, *arrc0);
	GF28.mul(c1, a1, b1); GF28.mul(c2, a2, b2);  // field multiplication
	GF28.add(s0, c1, c2);   //0x68*arrc1+0xb9*arrc0
	int32_t s0_int;
	GF28.convert(s0_int, s0);
	*arrc1 = (u8)s1_int;
	*arrc0 = (u8)s0_int;
	/********      Subbyte       ***************/
	*arrc1 = aesinvsbox[*arrc1]; *arrc0 = aesinvsbox[*arrc0];
}
/***************    encryption of AES-16   *********************/
void AES16Encrypt(u8 Pt[], u8 Ct[], u8 rk[roundaes16 + 1][2])
{
	u8 i;
	Ct[0] = Pt[0] ^ rk[0][0]; Ct[1] = Pt[1] ^ rk[0][1];
	for (i = 0; i < roundaes16; i++)
	{
		AES16Roundfunc(&Ct[1], &Ct[0], rk[i + 1]);
	}
}
/***************    decryption of AES-16   *********************/
void AES16Decrypt(u8 Ct[], u8 Pt[], u8 rk[roundaes16 + 1][2])
{
	u8 i;
	Pt[0] = Ct[0]; Pt[1] = Ct[1];
	for (i = roundaes16; i > 0; i--)
	{
		AES16invRoundfunc(&Pt[1], &Pt[0], rk[i]);
	}
	Pt[0] ^= rk[0][0]; Pt[1] ^= rk[0][1];
}
/*******************   (inv)nonlinear layer of SPNBOX16  ****************************/
void nonlinear(u16 arr[8][1], u8 roundkey[roundaes16 + 1][2]) // need to be modified
{
	u8 arrP[8][2],arrC[8][2];
	for (u16 i = 0; i < 8; i++)
	{
		arrP[i][0]= (u8)(arr[i][0] >> 8); //high
		arrP[i][1] = (u8)(arr[i][0]); //low
		AES16Encrypt(arrP[i], arrC[i], roundkey);
		arr[i][0]=(arrC[i][0] << 8) ^ arrC[i][1];	
	}  
}
void invnonlinear(u16 arr[8][1], u8 roundkey[roundaes16 + 1][2])
{
	u8 arrP[8][2], arrC[8][2];
	for (u16 i = 0; i < 8; i++)
	{
		arrC[i][0] = (u8)(arr[i][0] >> 8); //high
		arrC[i][1] = (u8)(arr[i][0]); //low
		AES16Decrypt(arrC[i], arrP[i], roundkey);
		arr[i][0] = (arrP[i][0] << 8) ^ arrP[i][1];
	}
}
/*******************   (inv)linear layer of SPNBOX16  ****************************/
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
/*******************   encryption of SPNBOX16  ****************************/
void encryptionblack(u16 input[block][8][1], u8 roundkey[roundaes16 + 1][2])
{
	for (int i = 0; i < round; i++)
	{
		for (int j = 0; j < block; j++)
		{
			nonlinear(input[j],roundkey);  // nonlinear layer
			linear(input[j]);    //linear layer	
			for (int k = 0; k < 8; k++)  //affine layer
			{
				input[j][k][0] ^= (u16)(8 * i + k + 1);
			}
		}
	}
}
/*******************   decryption of SPNBOX16  ****************************/
void decryptionblack(u16 input[block][8][1], u8 roundkey[roundaes16 + 1][2])
{
	for (int i = round-1; i >= 0 ; i--)
	{
		for (int j = 0; j < block; j++)
		{
			for (int k = 0; k < 8; k++)  //inverse affine layer
			{
				input[j][k][0] ^= (u8)(8 * i + k + 1);
			}
			linear(input[j]);    //inverse linear layer is the same as linear layer
			invnonlinear(input[j],roundkey);  // inverse nonlinear layer
		}
	}
}
/*********************************************************************************************

BELOW are some useful functions!

************************************************************************************************/
uint64_t Average(uint64_t list[], int lenlist)
{
	uint64_t ave, sum = 0;
	for (int i = 0; i < lenlist; i++) {
		sum += list[i];
	}
	ave = sum / lenlist;
	return ave;
}
void printmessage(u16 arr[block][8][1])
{
	for (int i = 0; i < block; i++)
		printf("%04X%04X%04X%04X%04X%04X%04X%04X",
			arr[i][0][0], arr[i][1][0], arr[i][2][0], arr[i][3][0], arr[i][4][0], arr[i][5][0], arr[i][6][0], arr[i][7][0]);
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
/***********************  verify decryption   **********************************/
void verifydecryption(u16 arr1[block][8][1], u16 arr2[block][8][1])
{
	int i = memcmp(arr1, arr2, 16);
	if (i == 0)
		printf("VERIFY DECRYPTION CORRECT!");
	else
		printf("VERIFY DECRYPTION WRONG!");
}
int main(int argc, char** argv)
{
	srand(time(0));
	uint64_t begin; uint64_t end; uint64_t elapsed_cycles[loops];
	AES16KeySchedule(rk);
	for (int k = 0; k < loops; k++)
	{
		printf("********* loop %d results below  ************\n", k);		
		generatemessage(input);
		memcpy(temp, input, 16*block);
		begin = start_rdtsc();
		encryptionblack(input,rk); //encrypt message
		end = end_rdtsc();
		elapsed_cycles[k] = (end - begin);
		decryptionblack(input,rk);//decrypt message
		printf("\n");
		verifydecryption(input, temp);	//verify decryption result
		printf("\n"); 	
	}
	uint64_t avecycles = Average(elapsed_cycles, loops);
	printf ("average cost %llu CPU cycles \n",avecycles);
	uint64_t cpb = avecycles/(block*16);
	printf ("average CPB for encryption is %llu \n",cpb);
	return 0;
}







