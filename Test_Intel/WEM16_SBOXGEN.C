/************************  S-box generation of WEM-16     ***********/
/*************************            2019.12    *************************/
/*************************   junjunll1212@gmail.com******************************/
    
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h> 
#include<errno.h>
#include<string.h>
#include<vector>
#include <givaro/gfq.h>

 
#define u8 uint8_t
#define u16 uint16_t   //
#define u32 uint32_t
#define roundaes 10

u8 sbox[256]=
{
	    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
u8 invsbox[256]=
{
    	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


using namespace Givaro;
int modulus[] = { 1, 1, 0, 1, 1, 0, 0, 0, 1 }; // x^8 + x^4 + x^3 + x + 1 (AES) (right is high-order)
GFqDom<int32_t> GF28(2, 8, modulus);

/**************       AES  ***************************/
u32 rcon[10] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
0x1b000000, 0x36000000 };
void bytetoword(u8 a1, u8 a2, u8 a3, u8 a4, u32* a)
{
	*a = ((u32)a1 << 24) | ((u32)a2 << 16) | ((u32)a3 << 8) | ((u32)a4);
}
void wordtobyte(u32 a, u8 * a1, u8 * a2, u8 * a3, u8 * a4)
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
	return a = (u32)sbox[(u8)(a >> 24)] << 24 | (u32)sbox[(u8)(a >> 16)] << 16 | (u32)sbox[(u8)(a >> 8)] << 8 |
		(u32)sbox[(u8)(a)];
}
void AESKeySchedule(u8 rk[roundaes + 1][16])
{
	/*for (u8 i = 0; i < roundaes + 1; ++i)
	{
		//rk[i] = rand();
		for (u8 j = 0; j < 16; j++)
		{
			rk[i][j] = i+j; //ignore AES key schedule, we fix the key
		}
	}*/

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
		bytetoword(masterkey[4 * i], masterkey[4 * i + 1], masterkey[4 * i + 2], masterkey[4 * i + 3], &w[i]);
	}
	for (i = 4; i < 44; i++)
	{
		temp = w[i - 1];
		if (i % 4 == 0)
                    {             
			temp = subword(rotword(temp)) ^ rcon[i / 4 -1];
                       }
		w[i] = w[i - 4] ^ temp;
	}

	for (i = 0; i < roundaes + 1; i++)
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

u8 multi(u8 num1,u8 num2)   //num1 * num2   in GF(2^8)
{
	GFqDom<int32_t>::Element a,b,c;
	GF28.init(a,num1); 
	GF28.init(b,num2);
	GF28.mul(c, a, b);
	int32_t c_int;
	GF28.convert(c_int, c);
	return (u8)c_int;
}

void AESRoundfunc(u8 arrc1[],  u8 arrrk[])
{
	/***********      Subbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = sbox[arrc1[i]]; 
	}
	
	/**********    shiftrows            ***********************/
	u8 temp[16];
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	int index[16] = {0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11};  
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = temp[index[i]];
	}
	/***********    Mixcolumn  *****************/// 
	for (int i = 0; i < 16; i++)
	{
		temp[i] = arrc1[i];
	}
	for (int i = 0; i < 4; i++)
	{
		u8 t= arrc1[4 * i + 0] ^ arrc1[4 * i + 1] ^ arrc1[4 * i + 2] ^ arrc1[4 * i + 3];

		arrc1[4 * i + 0] = t ^ temp[4 * i + 0] ^ multi(0x02,temp[4 * i + 0] ^ temp[4 * i + 1]);
		arrc1[4 * i + 1] = t ^ temp[4 * i + 1] ^ multi(0x02,temp[4 * i + 1] ^ temp[4 * i + 2]);
		arrc1[4 * i + 2] = t ^ temp[4 * i + 2] ^ multi(0x02,temp[4 * i + 2] ^ temp[4 * i + 3]);
		arrc1[4 * i + 3] = t ^ temp[4 * i + 3] ^ multi(0x02,temp[4 * i + 3] ^ temp[4 * i + 0]);
	}

	/*********      Addroundkey         ***************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] ^= arrrk[i];
	}
}
void AESRoundfunclast(u8 arrc1[], u8 arrrk[])
{
	/***********      Subbyte       **************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] = sbox[arrc1[i]];
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
		arrc1[i] = invsbox[arrc1[i]];
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
		arrc1[i] = invsbox[arrc1[i]];
	}

	/*********      Addroundkey         ***************/
	for (int i = 0; i < 16; i++)
	{
		arrc1[i] ^= arrrk[i];
	}
}


void AESEncrypt(u8 Pt[], u8 Ct[], u8 rk[roundaes + 1][16])
{
	for (u8 i = 0; i < 16; i++)  //pre-whitening
	{
		Ct[i] = Pt[i] ^ rk[0][i];
	}
	 
	for (u8 i = 1; i < roundaes; i++)
	{
		AESRoundfunc(Ct, rk[i]);//
	}
	AESRoundfunclast(Ct, rk[roundaes]);//
	for (u8 i = 0; i < 16; i++)  
	{
		Ct[i] = Pt[i] ^ Ct[i];//CTR
	}
}

void AESDecrypt(u8 Ct[], u8 Pt[], u8 rk[roundaes + 1][16])
{
	for (u8 i = 0; i < 16; i++)
	{
		Pt[i] = Ct[i] ^ Pt[i];//CTR
	}
	for (u8 i = 0; i < 16; i++)  //
	{
		Pt[i] = Pt[i] ^ rk[roundaes][i];
	}

	for (u8 i = roundaes - 1; i>0; i--)
	{
		AESinvRoundfunc(Pt, rk[i]);//
	}
	AESinvRoundfunclast(Pt, rk[0]);//
}

void generateroundkey(u8 arr[roundaes + 1][16])
{	
		AESKeySchedule(arr);
		for (int i = 0; i < roundaes + 1; i++)
		{
			printf("%d-round key is: ",i);
			for (int j = 0; j < 16; j++)
			{
				printf("0x%02X,", arr[i][j]);
			}
			printf("\n");
		}
		printf("\n");
}

void generatesbox(u8 P[32][256][16], u8 C[32][256][16], u8 rk[roundaes + 1][16])
{
	for (u8 j = 0x00; j < 0x20; j++)  // 
	{
		for (u8 i = 0x00; i <= 0xff; i++)
		{
			for (u8 k = 0; k < 14; k++)
			{
				P[j][i][k] = 0x00;
			}
			P[j][i][14] = j;
			P[j][i][15] = i;   // initilize counter and plaintext (lexicographical)
	
			AESEncrypt(P[j][i], C[j][i], rk);

			/*AESDecrypt(C[j][i], P[j][i], rk);   //verify encryption
			for (u8 k = 0; k < 16; k++)  
			{				
				printf("%02X,", P[j][i][k]);
			}
			printf("\n");
			*/
			if (i == 0xff)   // be careful about dead loop
				break;
		}	
	}
}

void generateinvsbox(u16 arr1[65536], u16 arr2[65536])  //return index in sboxgen[65536]
{
	u16 i, j;
	for ( i = 0x0000; i <= 0xffff; i++)
	{
		for ( j = 0; j <= 0xffff; j++)
		{
			if (arr1[j] == i)
				break;
		}
	    arr2[i] = j;
		printf("0x%04X,",arr2[i]);
		if (i == 0xffff)
			break;
	}
}

/*********************   BELOW ARE SOME USEFUL FUNCTIONS    **********************************/
void transfer(u8 arr1[32][256][16], u16 arr2[65536])
{
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j <256; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				arr2[2048 * i + 8 * j + k] = (arr1[i][j][2*k] << 8) ^ (arr1[i][j][2*k+1]);
			}
		}
	}
}

void FisherYates(u16 arr[65536],u16 arr2[65536])  //shuffle in arr2 (arr controls)
{
	for (int i = 0; i < 65536; i++)
	{
		arr2[i] = (u16)i;
	}
	for (int i = 65535; i >= 0; i--)
	{
	
		int j = ((int)(arr[i])) % (i+1);
		u16 temp = arr2[j];    //exchange arr2[j] and arr2[i]
 		arr2[j] = arr2[i];
		arr2[i] = temp;
	}
	for (int i = 0; i < 65536; i++)
	{
		printf("0x%04X,",arr2[i]);

	}
}

int search(u16 key, u16 arr[])
{
	u16 i;
	int exist = 0;
	for (i = 0; i < 0xffff; i++)
	{
		if (arr[i] == key)
			break;
	}
	if (i == 0x10000)
	{
		exist = 1; // if key is not in the array
		printf("0x%04X not found\n", key);
	}
	else
	{
		exist = 0;
		printf("index of 0x%04X  is 0x%04X\n", key, i);
	}
	return exist;
}
void checkpermutation(u16 arr[65536])
{
	int exist[65536];
	for (u16 i = 0; i <= 0xFFFF; i++)
	{
		exist[i] = search(i, arr);
		if (i == 0xffff)
			break;
	}
	int counter = 0;
	for (int i = 0; i < 65536; i++)
	{
		if (exist[i] == 1)
			counter++;
	}
	printf("there are %d elements not in the array\n", counter);
}


int main()
{
	u8 P[32][256][16]; //8192 AES-128 plaintexts
	u8 C[32][256][16]; //ciphertext
	u8 rk[roundaes +1][16]; //roundkey
	u16 FYarray[65536];//Fisher-Yates random sequence
	u16 sboxgen[65536],invsboxgen[65536];   //generated sbox
	double sec1,sec2;

	printf("round keys following:\n");
	generateroundkey(rk);

	printf("sbox following:\n");
	clock_t begin1=clock();
	generatesbox(P, C, rk);
	transfer(C, FYarray);  //transfer sequence of random bits
	FisherYates(FYarray,sboxgen); //Fisher-Yates shuffle algorithm
	clock_t end1=clock();
	sec1=double(end1 - begin1) / CLOCKS_PER_SEC;

	printf("\n\n\n");
	//checkpermutation(sboxgen); //verify whether the generated sbox is a PRP
	printf("inverse sbox following:\n");
	clock_t begin2=clock();
	generateinvsbox(sboxgen, invsboxgen);
	clock_t end2=clock();
	sec2=double(end2 - begin2) / CLOCKS_PER_SEC;
	//checkpermutation(invsboxgen);
	printf("\n\n\n");
	printf("time is %f\n",sec1);
	return 0;
}
