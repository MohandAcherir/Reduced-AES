#include <stdio.h>
#include <string.h>
#include "Matrices.h"


// SubOctet renversée
unsigned char SubOctetInverse(unsigned char byte) {
	unsigned char X, Y;	
	unsigned char a = 0x00, b = 0x00;
	int found = 0;

	// Chercher les coordonées X, Y tq TableSubtitution[X][Y] = byte
	for(int i = 0 ; (i < 16) && (found==0) ; i++) {
		b = 0x00;
		for(int j = 0 ; j < 16 ; j++) {
			if(TableSubstitution[i][j] == byte) {
				X = a;
				Y = b;

				found = 1; // Optimisation pour éviter des itérations redondantes
				break;
			}
			b = b + 0x01;
		}
		a = a + 0x01;
	}

	// res = XY
	unsigned char res = (X*(0x10) | Y);

	return res;
}


// Extension Cle renversée, cad étendre K(4) au lieu de K
void ExtensionCleBis(unsigned char cle_maitre[16], unsigned char cle_etendue[20][4]){
	// cle_etendu[16...19] = K(4) = cle_maitre(4x4)
	for(int i = 16 ; i < 20 ; i++) {
		cle_etendue[i][0] = cle_maitre[4*(i-16)];
		cle_etendue[i][1] = cle_maitre[4*(i-16)+1];
		cle_etendue[i][2] = cle_maitre[4*(i-16)+2];
		cle_etendue[i][3] = cle_maitre[4*(i-16)+3];
	}

	// Calculer K(3) : cle_etendu[12...15]
	// puis Calculer K(2) : cle_etendu[8...11]
	// puis Calculer K(1) : cle_etendu[4...7]
	// puis Calculer K(0) : cle_etendu[0...3] qui est la clé recherchée
	for(int i = 19 ; i >= 4 ; i--) {
		unsigned char tampon[4] = {0x00};
		
		for(int x = 0 ; x < 4 ; x++) {
			tampon[x] = cle_etendue[i-1][x];
		}

		if (i % 4 == 0) {
			RotationOctet(tampon);
			SubWord(tampon);

			tampon[0] ^= Rcon[i/4];
		}// ^ est le XOR en C
		else if (i % 4 == 4) {
			SubWord(tampon);
		}
		for(int x = 0 ; x < 4 ; x++) {
			cle_etendue[i-4][x] = cle_etendue[i][x] ^ tampon[x];
		}
	}
}


void SubOctet(unsigned char Matrice_Etat[4][4]) {
	// Pour chaque x, y dans {0, 3}
	// X = les 4 bits côté MSB de l'octet Matrice_Etat[x][y]
	// Y = les 4 bits côté LSB de l'octet Matrice_Etat[x][y]
	// Matrice_Etat[x][y] = TableSubstitution[X][Y]
	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			unsigned char X = (Matrice_Etat[i][j]/(0x10)) & 0x0f;
			unsigned char Y = (Matrice_Etat[i][j] & 0x0f);

			Matrice_Etat[i][j] = TableSubstitution[X][Y];
		}
	}
}


// Même fonction que SubOctet, sauf que c'est pour un seul octet 
// et pas pour une matrice entière
unsigned char SubByte(unsigned char byte) {
	unsigned char X = (byte & (0xf0))/(0x10);
	unsigned char Y = byte & 0x0f;

	return TableSubstitution[X][Y];
}

// Même fonction que SubByte, sauf que c'est pour 4 octets(Word)
void SubWord(unsigned char arr[4]) {
	for(int i = 0 ; i < 4 ; i++) {
		unsigned char X = (arr[i] & (0xf0))/(0x10);
		unsigned char Y = arr[i] & 0x0f;

		arr[i] = TableSubstitution[X][Y];
	}
}

// Decale la 2ème ligne 1 seule fois à gauche
// Decale la 3ème ligne 2 seule fois à gauche
// Decale la 4ème ligne 3 seule fois à gauche
void DecaleLignes(unsigned char Matrice_Etat[4][4]) {	
	for(int i = 1 ; i < 4 ; i++) {
			unsigned char a = Matrice_Etat[i][0];
			unsigned char b = Matrice_Etat[i][1];
			unsigned char c = Matrice_Etat[i][2];
			unsigned char d = Matrice_Etat[i][3];	
			
			// 2ème ligne
			if(i == 1) {
				Matrice_Etat[i][0] = b;
				Matrice_Etat[i][1] = c;
				Matrice_Etat[i][2] = d;
				Matrice_Etat[i][3] = a;
			}
			else if(i == 2) { // 3ème ligne
				Matrice_Etat[i][0] = c;
				Matrice_Etat[i][1] = d;
				Matrice_Etat[i][2] = a;
				Matrice_Etat[i][3] = b;
			}
			else { // 4ème ligne
				Matrice_Etat[i][0] = d;
				Matrice_Etat[i][1] = a;
				Matrice_Etat[i][2] = b;
				Matrice_Etat[i][3] = c;
			}
	}
}

void MelangeColonnes(unsigned char Matrice_Etat[4][4]) {
	for(int j = 0 ; j < 4 ; j++) {
		unsigned char b0 = Matrice_Etat[0][j];
		unsigned char b1 = Matrice_Etat[1][j];
		unsigned char b2 = Matrice_Etat[2][j];
		unsigned char b3 = Matrice_Etat[3][j];

		Matrice_Etat[0][j] = (unsigned char) (Multip2[(unsigned char)(b0/0x10) & 0xf][b0 & 0xf] ^ Multip3[(unsigned char)(b1/0x10) & 0xf][b1 & 0xf] ^ b2 ^ b3);
		Matrice_Etat[1][j] = (unsigned char) (b0 ^ Multip2[(unsigned char)(b1/0x10) & 0xf][b1 & 0xf] ^ Multip3[(unsigned char)(b2/0x10) & 0xf][b2 & 0xf] ^ b3);
		Matrice_Etat[2][j] = (unsigned char) (b0 ^ b1 ^ Multip2[(unsigned char)(b2/0x10) & 0xf][b2 & 0xf] ^ Multip3[(unsigned char)(b3/0x10) & 0xf][b3 & 0xf]);
		Matrice_Etat[3][j] = (unsigned char) (Multip3[(unsigned char)(b0/0x10) & 0xf][b0 & 0xf] ^ b1 ^ b2 ^ Multip2[(unsigned char)(b3/0x10) & 0xf][b3 & 0xf]);
	}

	// Additional Permutation

	unsigned char a00 = Matrice_Etat[0][0];
	unsigned char a01 = Matrice_Etat[0][1];
	unsigned char a02 = Matrice_Etat[0][2];
	unsigned char a03 = Matrice_Etat[0][3];

	unsigned char a10 = Matrice_Etat[1][0];
	unsigned char a11 = Matrice_Etat[1][1];
	unsigned char a12 = Matrice_Etat[1][2];
	unsigned char a13 = Matrice_Etat[1][3];

	unsigned char a20 = Matrice_Etat[2][0];
	unsigned char a21 = Matrice_Etat[2][1];
	unsigned char a22 = Matrice_Etat[2][2];
	unsigned char a23 = Matrice_Etat[2][3];

	unsigned char a30 = Matrice_Etat[3][0];
	unsigned char a31 = Matrice_Etat[3][1];
	unsigned char a32 = Matrice_Etat[3][2];
	unsigned char a33 = Matrice_Etat[3][3];


	Matrice_Etat[0][0] = a00; 
	Matrice_Etat[0][1] = a10;
	Matrice_Etat[0][2] = a20;
	Matrice_Etat[0][3] = a30;

	Matrice_Etat[1][0] = a13;
	Matrice_Etat[1][1] = a23;
	Matrice_Etat[1][2] = a33;
	Matrice_Etat[1][3] = a03;

	Matrice_Etat[2][0] = a22;
	Matrice_Etat[2][1] = a32;
	Matrice_Etat[2][2] = a02;
	Matrice_Etat[2][3] = a12;

	Matrice_Etat[3][0] = a31;
	Matrice_Etat[3][1] = a01;
	Matrice_Etat[3][2] = a11;
	Matrice_Etat[3][3] = a21;
}


// Matrice_Etat XOR = Cle_Tour
void AjoutCleTour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]) {
	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			Matrice_Etat[i][j] = (unsigned char) (Matrice_Etat[i][j] ^ Cle_Tour[i][j]);
		}
	}
}

// Rotation de 4 octets une seule fois à gauche
void RotationOctet(unsigned char arr[4]) {
	unsigned char a = arr[0];
	unsigned char b = arr[1];
	unsigned char c = arr[2];
	unsigned char d = arr[3];

	arr[0] = b;
	arr[1] = c;
	arr[2] = d;
	arr[3] = a;
}


// Générer les 10 sous-clés à partir de la clé maitre 
void ExtensionCle(unsigned char cle_maitre[16], unsigned char cle_etendue[44][4]){
	for(int i = 0 ; i < 4 ; i ++) {
		cle_etendue[i][0] = cle_maitre[4*i];
		cle_etendue[i][1] = cle_maitre[4*i+1];
		cle_etendue[i][2] = cle_maitre[4*i+2];
		cle_etendue[i][3] = cle_maitre[4*i+3];
	}

	for(int i = 4 ; i < 4 * (10 + 1) ; i++) { // 4 mots de 32 bits par clé de tour (et il y’en a 11)
		unsigned char tampon[4] = {0x00};
		
		for(int x = 0 ; x < 4 ; x++) {
			tampon[x] = cle_etendue[i-1][x];
		}

		if (i % 4 == 0) {
			RotationOctet(tampon);
			SubWord(tampon);

			tampon[0] ^= Rcon[i/4];
		}// ^ est le XOR en C
		else if (i % 4 == 4) {
			SubWord(tampon);
		}
		for(int x = 0 ; x < 4 ; x++) {
			cle_etendue[i][x] = cle_etendue[i-4][x] ^ tampon[x];
		}
	}
}


// Générer les 4 sous-clés à partir de la clé maitre pour le AES réduit
void ExtensionCle1(unsigned char cle_maitre[16], unsigned char cle_etendue[20][4]){
	for(int i = 0 ; i < 4 ; i ++) {
		cle_etendue[i][0] = cle_maitre[4*i];
		cle_etendue[i][1] = cle_maitre[4*i+1];
		cle_etendue[i][2] = cle_maitre[4*i+2];
		cle_etendue[i][3] = cle_maitre[4*i+3];
	}

	for(int i = 4 ; i < 4 * (5) ; i++) { // remplir les lignes 4 à 19 de cle_etendue
		unsigned char tampon[4] = {0x00};
		
		for(int x = 0 ; x < 4 ; x++) {
			tampon[x] = cle_etendue[i-1][x];
		}

		if (i % 4 == 0) {
			RotationOctet(tampon);
			SubWord(tampon);

			tampon[0] ^= Rcon[i/4];
		}// ^ est le XOR en C
		else if (i % 4 == 4) {
			SubWord(tampon);
		}
		for(int x = 0 ; x < 4 ; x++) {
			cle_etendue[i][x] = cle_etendue[i-4][x] ^ tampon[x];
		}
	}
}

// Un seul tour de l'AES
void Tour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]) {
	SubOctet(Matrice_Etat);
	DecaleLignes(Matrice_Etat);
	MelangeColonnes(Matrice_Etat);
	AjoutCleTour(Matrice_Etat, Cle_Tour);
}

// Dernier tour sans Melange Colonnes
void Dernier_Tour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]) {
	SubOctet(Matrice_Etat);

	DecaleLignes(Matrice_Etat);
	

	{ // Additional permutation
	unsigned char a00 = Matrice_Etat[0][0];
	unsigned char a01 = Matrice_Etat[0][1];
	unsigned char a02 = Matrice_Etat[0][2];
	unsigned char a03 = Matrice_Etat[0][3];

	unsigned char a10 = Matrice_Etat[1][0];
	unsigned char a11 = Matrice_Etat[1][1];
	unsigned char a12 = Matrice_Etat[1][2];
	unsigned char a13 = Matrice_Etat[1][3];

	unsigned char a20 = Matrice_Etat[2][0];
	unsigned char a21 = Matrice_Etat[2][1];
	unsigned char a22 = Matrice_Etat[2][2];
	unsigned char a23 = Matrice_Etat[2][3];

	unsigned char a30 = Matrice_Etat[3][0];
	unsigned char a31 = Matrice_Etat[3][1];
	unsigned char a32 = Matrice_Etat[3][2];
	unsigned char a33 = Matrice_Etat[3][3];


	Matrice_Etat[0][0] = a00; 
	Matrice_Etat[0][1] = a10;
	Matrice_Etat[0][2] = a20;
	Matrice_Etat[0][3] = a30;

	Matrice_Etat[1][0] = a13;
	Matrice_Etat[1][1] = a23;
	Matrice_Etat[1][2] = a33;
	Matrice_Etat[1][3] = a03;

	Matrice_Etat[2][0] = a22;
	Matrice_Etat[2][1] = a32;
	Matrice_Etat[2][2] = a02;
	Matrice_Etat[2][3] = a12;

	Matrice_Etat[3][0] = a31;
	Matrice_Etat[3][1] = a01;
	Matrice_Etat[3][2] = a11;
	Matrice_Etat[3][3] = a21;
	}

	AjoutCleTour(Matrice_Etat, Cle_Tour);
}
