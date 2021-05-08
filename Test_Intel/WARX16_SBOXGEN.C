/************************  S-box generation of WARX-16     ***********/
/*************************            2019.8    *************************/
/*************************   junjunll1212@gmail.com******************************/
    
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include<vector>
#ifdef _WIN32 #define _CRT_SECURE_NO_DEPRECATE 
#endif 
#define u8 uint8_t
#define u16 uint16_t   //
#define u32 uint32_t

#define round1632 24

#define numpair 65536  // original
#define numkey 1 // original

#define ROTL8(x,r)  (((x)<<(r)) | (x>>(8-(r))))
#define ROTR8(x,r) (((x)>>(r)) | ((x)<<(8-(r))))

#define ER8(x,y,k) (x=ROTR8(x,7), x+=y, x^=k, y=ROTL8(y,2), y^=x)
#define DR8(x,y,k) (y^=x, y=ROTR8(y,2), x^=k, x-=y, x=ROTL8(x,7))
#define newER8(x,y,k_left,k_right) (x^=k_left, y^=k_right, x=ROTR8(x,7), x+=y, y=ROTL8(y,2), y^=x)
#define newDR8(x,y,k_left,k_right) (y^=x, y=ROTR8(y,2), x-=y, x=ROTL8(x,7), y^=k_right, x^=k_left)


/***************   original    SPECK16  *****************************************************************************/

void Speck1632Encrypt(u8 Pt[], u8 Ct[], u8 rk[])
{
	u8 i;
	Ct[0] = Pt[0]; Ct[1] = Pt[1];
	for (i = 0; i < round1632; i++)
		ER8(Ct[1], Ct[0], rk[i]);
}
void Speck1632Decrypt(u8 Pt[], u8 Ct[], u8 rk[])
{
	int i;
	Pt[0] = Ct[0]; Pt[1] = Ct[1];
	for (i = round1632; i >= 0;)
		DR8(Pt[1], Pt[0], rk[i--]);
}
/**************       new speck16    ***************************/
void newSpeck1632KeySchedule(u8 rk[])
{
	u8 i;
	for (i = 0; i < 2 * round1632; ++i)
	{
		//rk[i] = rand();
		rk[i] = i; //ignore KDF, we fix the key
	}
}

void newSpeck1632Encrypt(u8 Pt[], u8 Ct[], u8 rk[])
{
	u8 i;
	Ct[0] = Pt[0]; Ct[1] = Pt[1];
	for (i = 0; i < 2 * round1632; i += 2)
		newER8(Ct[1], Ct[0], rk[i], rk[i + 1]);
}
void newSpeck1632Decrypt(u8 Ct[], u8 Pt[], u8 rk[])
{
	u8 i;
	Pt[0] = Ct[0]; Pt[1] = Ct[1];
	for (i = 2 * round1632; i > 0; i -= 2)
		newDR8(Pt[1], Pt[0], rk[i - 2], rk[i - 1]);
}
void generateroundkey(u8 arr[numkey][2 * round1632])
{
	for (int j = 0; j < numkey; j++) // generate round keys
	{
		newSpeck1632KeySchedule(arr[j]);
		for (int i = 0; i < 2 * round1632; i = i + 2)
		{
			printf("0x%02X%02X,", arr[j][i], arr[j][i + 1]);
			//printf("roundkey is 0x%04X\n", rk[j][i]);
		}
		printf("\n");

	}
}
void generatesbox(u8 P[256][256][2], u8 C[256][256][2], u8 rk[numkey][2 * round1632])
{
	for (u8 j = 0x00; j <= 0xff; j++)  // 0xff+1=0xfe
	{
		for (u8 i = 0x00; i <= 0xff; i++)
		{
			P[j][i][0] = j;   P[j][i][1] = i;   // initilize plaintext pairs (lexicographical)
			newSpeck1632Encrypt(P[j][i], C[j][i], rk[0]);
			printf("0x%02X%02X,", C[j][i][0], C[j][i][1]);
			if (i == 0xff)   // be careful about dead loop
				break;
		}
		if (j == 0xff)
			break;
	}
}
void generateinvsbox(u8 PP[256][256][2], u8 CC[256][256][2], u8 rk[numkey][2 * round1632])
{
	for (u8 j = 0; j <= 0xff; j++)
	{
		for (u8 i = 0; i <= 0xff; i++)
		{
			CC[j][i][0] = j;   CC[j][i][1] = i;   // initilize plaintext pairs (lexicographical)
			newSpeck1632Decrypt(CC[j][i], PP[j][i], rk[0]);
			printf("0x%02X%02X,", PP[j][i][0], PP[j][i][1]);
			if (i == 0xff)
				break;
		}
		if (j == 0xff)
			break;
	}
}
int main()
{
	u8 P[256][256][2], PP[256][256][2]; //plaintext
	u8 C[256][256][2], CC[256][256][2]; //ciphertext
	u8 rk[numkey][2 * round1632]; //roundkey
	double sec1,sec2;

	printf("round keys following:\n");
	generateroundkey(rk);
	
	printf("sbox following:\n");
	clock_t begin1=clock();
	generatesbox(P, C, rk);
	clock_t end1=clock();
	sec1=double(end1 - begin1) / CLOCKS_PER_SEC;
	printf("\n\n\n");

	printf("inverse sbox following:\n");
	clock_t begin2=clock();
	generateinvsbox(PP, CC, rk);
	clock_t end2=clock();
	sec2=double(end2 - begin2) / CLOCKS_PER_SEC;
	printf("\n time is %f\n",sec1);
	printf("\n");
	

	return 0;
}
