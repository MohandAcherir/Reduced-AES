#include <stdio.h>
#include <string.h>


// Imports
void Dernier_Tour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]);
void Tour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]);
void ExtensionCle(unsigned char cle_maitre[16], unsigned char cle_etendue[44][4]);
void AjoutCleTour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]);
void RotationOctet(unsigned char arr[4]);
void AjoutCleTour(unsigned char Matrice_Etat[4][4], unsigned char Cle_Tour[][4]);
void MelangeColonnes(unsigned char Matrice_Etat[4][4]);
void DecaleLignes(unsigned char Matrice_Etat[4][4]);
void SubWord(unsigned char arr[4]);
void SubOctet(unsigned char Matrice_Etat[4][4]);

int nbLignes;


// La fonction AES qui chiffre un bloc
int AES_128(unsigned char Matrice_Etat[4][4], unsigned char Cle_Maitre[16]) {
	unsigned char Cle_Etendu[44][4];

	ExtensionCle(Cle_Maitre, Cle_Etendu);
	AjoutCleTour(Matrice_Etat, Cle_Etendu);

	for(int i = 1 ; i < 10 ; i++) { // 9 itérations
		Tour(Matrice_Etat, Cle_Etendu + 4*i);
	}
	Dernier_Tour(Matrice_Etat, Cle_Etendu + 4*10); // + le dernier tour
}


// Algorithme pour incrémenter la nonce
void Inc_Nonce(unsigned char Nonce[4][4]) {
	int i = 15, c = 1, d = 1;
	int x = i/4, y = i%4;

	while(i >= 0 && (Nonce[x][y] == 0xff)) {
		Nonce[x][y] = 0x00;

		i--;
		
		x = i/4;
		y = i%4;
	}

	if(i>=0) {
		x = i/4;
		y = i%4;

		if( ((Nonce[x][y] & 0x0f) == 0x0f) ) {
			Nonce[x][y] = ((Nonce[x][y] & 0x0f)) + ( (Nonce[x][y] & 0xf0) + (0x1) )*(0x10);
		}
		else if( ( ( (Nonce[x][y] & 0x0f) != 0x0f) && ((Nonce[x][y] & 0xf0) == 0xf0) ) 
			|| ( ((Nonce[x][y] & 0x0f) != 0x0f) && ((Nonce[x][y] & 0xf0) != 0xf0) ) ) {
			Nonce[x][y] = ((Nonce[x][y] & 0xf0)) + ( (Nonce[x][y] & 0x0f) + (0x1) );	
		}
	}
}


// Le mode de chiffrement CTR
void CTR(unsigned char Matrice_Etat[][4],
		unsigned char Cle_Maitre[16],
		unsigned char Nonce[4][4]) {

	for(int i = 0 ; i < nbLignes ; i=i+4) {
		unsigned char X[4][4];

		for(int x = 0 ; x < 4 ; x++) {
			for(int y = 0 ; y < 4 ; y++) {
				X[x][y] = Nonce[x][y];
			}
		}	

		AES_128(X, Cle_Maitre);

		for(int x = 0 ; x < 4 ; x++) {
			for(int y = 0 ; y < 4 ; y++) {
				Matrice_Etat[i+x][y] = Matrice_Etat[i+x][y] ^ X[x][y];
			}
		}

		Inc_Nonce(Nonce); // Incrémenter la nonce
	}
}

// Le mode de chiffrement OFB
void OFB(unsigned char Matrice_Etat[][4],
		unsigned char Cle_Maitre[16],
		unsigned char IV[4][4]) {

	unsigned char X[4][4];

	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			X[i][j] = IV[i][j];
		}
	}

	for(int i = 0 ; i < nbLignes ; i=i+4) {
		AES_128(X, Cle_Maitre);

		for(int x = 0 ; x < 4 ; x++) {
			for(int y = 0 ; y < 4 ; y++) {
				Matrice_Etat[i+x][y] = Matrice_Etat[i+x][y] ^ X[x][y];
				X[x][y] = Matrice_Etat[i+x][y];
			}
		}
	}	
}

int main() {
	int taille; // Variable pour stocker la taille du texte clair
	printf("Entrez la taille du text clair: \n");
	scanf("%d", &taille);
	
	nbLignes = (taille%16 == 0) ? (taille/4) : (taille + (16-(taille%16)))/4 ; // Taille multiple de 16

	unsigned char Matrice_Etat[nbLignes][4];

	char dummy;
	scanf("%c", &dummy); // Echapper le '\n'

	printf("\nEntrez le text clair:\n");
	for(int i = 0 ; i < nbLignes ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			if((i*4)+j < taille) {
				scanf("%2hhx", &(Matrice_Etat[i][j]));
			}
			else {
				Matrice_Etat[i][j] = 0x00; // Padding avec des 0x00	
			}	
		}
	}

	unsigned char Cle_Maitre[16];

	printf("\nEntrez cle maitre: \n");
	for(int i = 0 ; i < 16 ; i++) {
		scanf("%2hhx", &(Cle_Maitre[i]));
	}

	unsigned char Nonce[4][4];

	printf("\nEntrez Nonce:\n");
	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			scanf("%2hhx", &(Nonce[i][j]));		
		}
	}

	printf("\nTexte clair: \n");
	for(int i = 0 ; i < nbLignes ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			if(i*4 + j < taille) {
				printf("%2.2x, ", Matrice_Etat[i][j]);
				
				if((j == 3) || (i*4+j == taille-1)) {
					printf("\n");
				}
			}	
		}
	}
		
	printf("\nCle: \n");
	
	for(int i = 0 ; i < 16 ; i++) {
		printf("%2.2x, ", Cle_Maitre[i]);
	}
	printf("\n");

	unsigned char ME_backup[4][4];

	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			ME_backup[i][j] = Matrice_Etat[i][j];
		}
	}

	CTR(Matrice_Etat, Cle_Maitre, Nonce); // Chiffrer le texte clair avec le mode CTR

	printf("\nTexte Crypte: CTR\n");
	for(int i = 0 ; i < nbLignes ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			if(i*4 + j < taille) {
				printf("%2.2x, ", Matrice_Etat[i][j]);
				
				if((j == 3) || (i*4+j == taille-1)) {
					printf("\n");
				}
			}
		}
	}

	OFB(ME_backup, Cle_Maitre, Nonce); // Chiffrer le texte clair avec le mode OFB

	printf("\nTexte Crypte: OFB\n");
	for(int i = 0 ; i < nbLignes ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			if(i*4 + j < taille) {
				printf("%2.2x, ", ME_backup[i][j]);

				if((j == 3) || (i*4+j == taille-1)) {
					printf("\n");
				}
			}
		}
	}

	return 0;
}
