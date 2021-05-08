/**************    reference white-box implementation of WEM-16   ************/
/*******            2019.12                           ***************/
/**********     junjunll1212@gmail.com              ***********************/

#include <stdlib.h>
#include<sys/time.h>
#include <givaro/gfq.h>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <wemsbox.h>
#include <aessbox.h>
#define round 12
#define roundAES 5
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define block 128// message of length 2048 bytes
#define loops 100000 //  repeat 100000 times
static	uint64_t begin; uint64_t end; uint64_t elapsed_cycles[loops];
static	u16 input[block][8], temp[block][8];
static	u8 roundkey[roundAES+1][16];
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
int modulus[] = { 1, 1, 0, 1, 1, 0, 0, 0, 1 }; // x^8 + x^4 + x^3 + x + 1 (AES) (right is high-order)
GFqDom<int32_t> GF28(2, 8, modulus);
u32 rcon[10] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
0x1b000000, 0x36000000};

/***************************    AES  functions                *********************************/
u8 multi(u8 num1, u8 num2)   //num1 * num2   in GF(2^8)
{
	GFqDom<int32_t>::Element a, b, c;
	GF28.init(a, num1);
	GF28.init(b, num2);
	GF28.mul(c, a, b);
	int32_t c_int;
	GF28.convert(c_int, c);
	return (u8)c_int;
}
void bytetoword(u8 a1, u8 a2, u8 a3, u8 a4, u32 *a)
{
	 *a = ((u32)a1 << 24) | ((u32)a2 << 16) | ((u32)a3 << 8) | ((u32)a4);
}
void wordtobyte(u32 a, u8 *a1, u8 *a2, u8 *a3, u8 *a4)
{
	*a1 = (u8)(a >> 24);
	*a2 = (u8)(a >> 16);
	*a3 = (u8)(a >> 8);
    *a4 = (u8)(a);
}
u32 rotword(u32 a)
{
	return a = (a << 8) ^ (a >> 24);
}
u32 subword(u32 a)
{
	return a = (u32)aessbox[(u8)(a >> 24)] << 24 | (u32)aessbox[(u8)(a >> 16)] << 16 | (u32)aessbox[(u8)(a >> 8)] << 8 | 
		(u32)aessbox[(u8)(a)];
}
void AESKeySchedule(u8 rk[(roundAES + 1)][16])//u8 rk[96]   // AES KEY SCHEDULE
{
	u8 masterkey[16];
	for (u8 i = 0; i < 16; i++)
	{
		masterkey[i] = 0x00;   // master key is 0
	}
	u32 w[44];
	u32 temp;
	int  i;
	for (i = 0; i < 4; i++)
	{
		bytetoword(masterkey[4 * i], masterkey[4 * i + 1], masterkey[4 * i + 2], masterkey[4 * i + 3],&w[i]);
	}
	for (i = 4; i < 4* (roundAES+1); i++)
	{
		temp = w[i - 1];
		if (i % 4 == 0)
			temp = subword(rotword(temp)) ^ rcon[i/4-1];
		w[i] = w[i - 4] ^ temp;
	}
	for (i = 0; i < roundAES+1; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			rk[i][4 * j + 0] = (u8)(w[4 * i + j] >> 24);
			rk[i][4 * j + 1] = (u8)(w[4 * i + j] >> 16);
			rk[i][4 * j + 2] = (u8)(w[4 * i + j] >> 8);
			rk[i][4 * j + 3] = (u8)(w[4 * i + j]);
		}
	}
}
void AESRoundfunc(u8 arrc1[], u8 arrrk[])
{
	/***********      Subbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = aessbox[arrc1[i]];
	}
	/**********    shiftrows            **********************/
	u8 temp[16];
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	int index[16] = { 0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11 };
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = temp[index[i]];
	}
	/***********    Mixcolumn  *****************/
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	for (int i = 0; i < 4; i++)
	{
		u8 t = arrc1[4 * i + 0] ^ arrc1[4 * i + 1] ^ arrc1[4 * i + 2] ^ arrc1[4 * i + 3];

		arrc1[4 * i + 0] = t ^ temp[4 * i + 0] ^ multi(0x02, temp[4 * i + 0] ^ temp[4 * i + 1]);
		arrc1[4 * i + 1] = t ^ temp[4 * i + 1] ^ multi(0x02, temp[4 * i + 1] ^ temp[4 * i + 2]);
		arrc1[4 * i + 2] = t ^ temp[4 * i + 2] ^ multi(0x02, temp[4 * i + 2] ^ temp[4 * i + 3]);
		arrc1[4 * i + 3] = t ^ temp[4 * i + 3] ^ multi(0x02, temp[4 * i + 3] ^ temp[4 * i + 0]);
	}
	/*************   addroundkey   **********/
	for (int i=0;i<16;i++)
	{
		arrc1[i] ^= arrrk[i];
	}
}
void AESRoundfunclast(u8 arrc1[], u8 arrrk[])
{
	/***********      Subbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = aessbox[arrc1[i]];
	}
	/**********    shiftrows            ***********************/
	u8 temp[16];
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	int index[16] = { 0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11 };
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = temp[index[i]];
	}
	/*********      Addroundkey         ***************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] ^= arrrk[i];
	}
}
void AESinvRoundfunc(u8 arrc1[], u8 arrrk[])
{
	u8 temp[16];
		/**********    invshiftrows            ***********************/
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	int index[16] = { 0,13,10,7,4,1,14,11,8,5,2,15,12,9,6,3 };
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = temp[index[i]];
	}
	/***********      invSubbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = aesinvsbox[arrc1[i]];
	}
	/*********      Addroundkey         ***************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] ^= arrrk[i];
	}
       /***********    invMixcolumn  *****************/// 
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	for (int i = 0; i < 4; i++)
	{
		u8 t = arrc1[4 * i + 0] ^ arrc1[4 * i + 1] ^ arrc1[4 * i + 2] ^ arrc1[4 * i + 3];
		arrc1[4 * i + 0] = temp[4 * i + 0] ^ multi(0x09, t) ^ multi(0x04, temp[4 * i + 0] ^ temp[4 * i + 2]) ^ multi(0x02, temp[4 * i + 0] ^ temp[4 * i + 1]);
		arrc1[4 * i + 1] = temp[4 * i + 1] ^ multi(0x09, t) ^ multi(0x04, temp[4 * i + 1] ^ temp[4 * i + 3]) ^ multi(0x02, temp[4 * i + 1] ^ temp[4 * i + 2]);
		arrc1[4 * i + 2] = temp[4 * i + 2] ^ multi(0x09, t) ^ multi(0x04, temp[4 * i + 0] ^ temp[4 * i + 2]) ^ multi(0x02, temp[4 * i + 2] ^ temp[4 * i + 3]);
		arrc1[4 * i + 3] = temp[4 * i + 3] ^ multi(0x09, t) ^ multi(0x04, temp[4 * i + 1] ^ temp[4 * i + 3]) ^ multi(0x02, temp[4 * i + 3] ^ temp[4 * i + 0]);
	}
}
void AESinvRoundfunclast(u8 arrc1[], u8 arrrk[])
{
	/**********    invshiftrows            ***********************/
	u8 temp[16];
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	int index[16] = { 0,13,10,7,4,1,14,11,8,5,2,15,12,9,6,3 };
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = temp[index[i]];
	}
	/***********      invSubbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = aesinvsbox[arrc1[i]];
	}
	/*********      Addroundkey         ***************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] ^= arrrk[i];
	}
}
void AES5Encrypt(u8 Pt[],u8 rk[roundAES + 1][16])
{
	for (int i = 0; i < 16; i++)  //pre-whitening
	{
		Pt[i] ^= rk[0][i];
	}

	for (int i = 1; i < roundAES; i++)
	{
		AESRoundfunc(Pt, rk[i]);//
	}
	AESRoundfunclast(Pt, rk[roundAES]);//
}

void AES5Decrypt(u8 Ct[], u8 rk[roundAES + 1][16])
{
	for (int i = 0; i < 16; i++)  //
	{
		Ct[i] ^= rk[roundAES][i];
	}

	for (int i = roundAES - 1; i > 0; i--)
	{
		AESinvRoundfunc(Ct, rk[i]);//
	}
    AESinvRoundfunclast(Ct, rk[0]);//
}
/*******************   (inv)nonlinear layer of WEM-16 :LUT  ****************************/
void nonlinear(u16 arr[8])
{
	for (u16 i = 0; i < 8; i++) {
		arr[i]= sbox[arr[i]];
	}
}
void invnonlinear(u16 arr[8])
{
	for (u16 i = 0; i < 8; i++) {	
		arr[i] = invsbox[arr[i]];	
	}
}
/*******************   (inv)linear layer of WEM-16:5-round AES ****************************/
void transfer168(u16 arr1[8], u8 arr2[16])
{
	for (int i = 0; i < 16; i=i+2)
	{
		arr2[i] = (u8)((arr1[i / 2]) >> 8);
		arr2[i + 1] = (u8)(arr1[i / 2]);
	}
}
void transfer816(u8 arr1[16], u16 arr2[8])
{
	for (int i = 0; i < 8; i ++)
	{
		arr2[i] = ((arr1[2 * i]) << 8) ^ arr1[2 * i + 1];
	}
}
void linear(u16 arr[8], u8 rk[roundAES + 1][16])
{
	u8 arr2[16];
	transfer168(arr, arr2);
	AES5Encrypt(arr2,rk);
	transfer816(arr2,arr);
}
void invlinear(u16 arr[8], u8 rk[roundAES + 1][16])
{
	u8 arr2[16];
	transfer168(arr, arr2);
	AES5Decrypt(arr2,rk);
	transfer816(arr2, arr);
}
/*******************   encryption of WEM-16  ****************************/
void encryptionwhite(u16 input[block][8],u8 rk[roundAES+1][16])
{
	for (int i = 0; i < round; i++)
	{
		for (int j = 0; j < block; j++)
		{
			nonlinear(input[j]);  // nonlinear layer
			linear(input[j],rk);    //linear layer	
		}
	}
	for (int j = 0; j < block; j++)
	{
		nonlinear(input[j]);  // last nonlinear layer
	}
}
/*******************   decryption of WEM-16  ****************************/
void decryptionwhite(u16 input[block][8], u8 rk[roundAES + 1][16])
{	
	for (int j = 0; j < block; j++)
	{
		invnonlinear(input[j]);  // nonlinear layer
	}
	for (int i = round - 1; i >= 0; i--)
	{
		for (int j = 0; j < block; j++)
		{
			invlinear(input[j],rk);    //invlinear layer 
			invnonlinear(input[j]);  // nonlinear layer
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
void printmessage(u16 arr[block][8])
{
	for (int i=0;i<block;i++) 
	   printf("%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X",
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
	AESKeySchedule(roundkey);
	for (int k = 0; k < loops; k++)
	{	
		printf("*********  %d-th loop results below  ************\n", k); 
	    generatemessage(input);
		memcpy(temp, input, 16*block);
		encryptionwhite(input,roundkey); //encrypt message
		begin = start_rdtsc();
		decryptionwhite(input,roundkey);//decrypt message
		end = end_rdtsc();
		elapsed_cycles[k] = (end - begin);	
		verifydecryption(input,temp);	//verify decryption result
		printf("\n");		
	}
	uint64_t avecycles = Average(elapsed_cycles, loops);
	printf ("average cost %llu CPU cycles for %d tests\n",avecycles,loops);
	uint64_t cpb = avecycles/(block*16);
	printf ("average CPB for (encryption) decryption is %llu \n",cpb);
	return 0;
}
