#include <stdint.h>
#include <stdio.h>
#define BLOCK_SIZE 128
#define AES_KEY_SIZE 194
#define NR 14

typedef unsigned char byte;
uint8_t sbox[256] = {
    41,35,190,132,225,108,214,174,82,144,73,241,187,233,235,179,166,219,60,135,12,62,153,36,94,13,28,6,183,71,222,179,18,77,200,67,187,139,166,31,3,90,125,9,56,37,31,93,212,203,150,245,69,59,19,13,137,10,28,219,174,50,32,154,80,238,64,120,54,253,18,73,50,246,158,125,73,220,173,79,20,242,68,64,102,208,107,196,48,183,50,59,161,34,246,34,145,157,225,139,31,218,176,202,153,2,185,114,157,73,44,128,126,197,153,213,233,128,178,234,201,204,83,191,103,214,191,20,214,126,45,220,142,102,131,239,87,73,255,105,143,205,209,30,157,156,22,114,230,29,240,132,79,74,119,2,215,232,57,44,83,203,201,18,30,51,116,158,12,244,213,212,159,212,164,89,126,53,207,50,34,244,204,207,211,144,45,72,211,143,117,230,217,29,42,229,192,247,43,120,129,135,68,14,95,80,212,141,190,123,5,21,7,59,51,130,31,112,146,218,100,84,206,177,133,62,105,21,248,70,106,4,150,115,14,217,22,47,103,104,212,247,74,208,87,104,118,250,22,187,17,173,174,36,136,121
};


void expand_key(
    uint8_t key[4][4 * NR ] , 
    uint8_t keyText[AES_KEY_SIZE/8]
    ){
    
    int N = AES_KEY_SIZE  / 8 / 4;
    uint8_t temp[4];
    for(int j=0 ; j<N; j++){
        for(int i=0 ; i<4; i++){
            key[i][j] = keyText[i+j * 4];
        }
    }


    


    for (int j = N; j < 4 *NR; j++) {
       if(j % N == 0){
        key[0][j] = sbox[key[1][j-1]]^  key[0][j-N] ^ (j / N) ;
        key[3][j] = sbox[key[0][j-1]] ^ key[3][j-N];
        key[1][j] = sbox[key[2][j-1]] ^  key[1][j-N];
        key[2][j] = sbox[key[3][j-1]] ^  key[2][j-N];    

       }else{
        key[0][j] = key[0][j-N] ^  key[0][j-1];
        key[1][j] = key[1][j-N]^  key[1][j-1];
        key[2][j] = key[2][j-N] ^  key[2][j-1];
        key[3][j] = key[3][j-N] ^ key[3][j-1];

       }
    }

        

   
    
}


void MixColumns(uint8_t state[4][4]) {
    uint8_t tmp[4];
    for (int c = 0; c < 4; c++) {
        tmp[0] = state[0][c];
        tmp[1] = state[1][c];
        tmp[2] = state[2][c];
        tmp[3] = state[3][c];
        state[0][c] = (uint8_t)((uint8_t)(tmp[0] * 2) ^ (uint8_t)(tmp[1] * 3) ^ tmp[2] ^ tmp[3]);
        state[1][c] = (uint8_t)(tmp[0] ^ (uint8_t)(tmp[1] * 2) ^ (uint8_t)(tmp[2] * 3) ^ tmp[3]);
        state[2][c] = (uint8_t)(tmp[0] ^ tmp[1] ^ (uint8_t)(tmp[2] * 2) ^ (uint8_t)(tmp[3] * 3));
        state[3][c] = (uint8_t)((uint8_t)(tmp[0] * 3) ^ tmp[1] ^ tmp[2] ^ (uint8_t)(tmp[3] * 2));
    }
}

void  AddRoundKey(uint8_t  state[4][4], uint8_t  key[4][4]){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            state[i][j] ^= key[i][j];
        }
    }
}

void SubBytes(uint8_t  state[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}



void ShiftRows(uint8_t  state[4][4] ){
    for (int i = 1; i < 4; i++) {

        int temp[i];

        for (int j = 4 - i; j < 4; j++) {
            temp[i - 4 + j] = state[i][j];
        }

        for (int j = 4 - 1; j >= i; j--) {
            state[i][j] = state[i][j - i];
        }

        for (int j = 0; i < j; i++) {
            state[i][j] = temp[j];
        }  
    }
}

void extractRoundKey(
    uint8_t key[4][4 * NR], 
    uint8_t roundKey[4][BLOCK_SIZE/8/4] , 
    int round){


    int N = BLOCK_SIZE/8/4;

    

    

    for(int i=0 ;i<4 ; i++){
        for(int j  = round * 4 ;  j< (round + 1) * 4 ; j++){
            roundKey[i][j -  round * 4] =  key[i][j];
        }
    }





    

}


void aes_encrypt_block(
    uint8_t key[4][AES_KEY_SIZE * NR / 8 / 4], 
    uint8_t block[BLOCK_SIZE/8], 
    uint8_t *ciphertext ){

    int N =  BLOCK_SIZE/8/4;

    
    uint8_t state[4][N]; 

    uint8_t roundKey[4][N];


   

    extractRoundKey(key , roundKey , 0);


    
    AddRoundKey(state , roundKey);
    for(int round = 1; round<NR ; round++){
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        extractRoundKey(key , roundKey , round);
        AddRoundKey(state , roundKey);
    }

    
    extractRoundKey(key , roundKey , NR);
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state , roundKey);

}




void aes_ctr_encrypt(
    uint8_t key[4][AES_KEY_SIZE * NR / 8 / 4], 
    const  byte *nonce, const byte *plaintext, 
    size_t plaintext_len, byte *ciphertext) {
    uint8_t block[BLOCK_SIZE / 8 ];
    // int i, j, k;
    memcpy(block, nonce, BLOCK_SIZE / 8);

    for (int i = 0; i < plaintext_len; i += BLOCK_SIZE / 8) {
        for (int j = BLOCK_SIZE / 8 - 1; j >= 0; j--) {
            if(++block[j]){
                break;
            }
        }

        aes_encrypt_block(key, block, ciphertext);
        for (int k = 0; k < BLOCK_SIZE / 8 && i + k < plaintext_len; k++) {
            ciphertext[i + k] = plaintext[i + k] ^ block[k];
        }
    }


    
}




int main(int argc, char *argv[]){
    uint8_t keyText[AES_KEY_SIZE / 8]="123456781234567821345678";
    char * outFileName = "out.txt";
    char * inputFileName = "input.txt";
    FILE *outFile , *inputFile;
    uint8_t key[4][4 * NR];
    uint8_t nonce[BLOCK_SIZE / 8]="12345678";


    outFileName = argv[2];
    inputFileName = argv[1];



    expand_key(key , keyText);


    uint8_t plaintext[] = "";
    char bitText = "";
    
    inputFile = fopen(inputFileName,"r");
    int i = 0;
    while(fscanf(inputFile,"%s", &bitText)){
        printf("%c \n" , bitText);
        plaintext[i] =  bitText;
    }

    // fscanf(inputFile,"%s", &plaintext);

    printf("%s \n" , plaintext);

    


    
    // uint8_t plaintext[] ={
    //     0x92, 0x15 , 0x93  , 0xd7 , 0x4a , 0x21 , 0xd5 , 0x03  , 0xa7 , 0xd9,
    //     0xeb , 0x80 , 0xa2, 0xc8 ,0xfe ,0x18 ,0xe6 ,0xa9 , 0x9c ,0xc2 ,0x82 ,
    //     0x98 , 0xfe , 0xf5  , 0x8d  , 0x4c , 0xf5 , 0x08
    // };

    // uint8_t plaintext[] = {
    //     0x65 ,0x5a ,0x5a ,0x47 ,0x15 ,0x5f ,0x44
    // };

    size_t plaintext_size = strlen((char *)plaintext);

    uint8_t ciphertext[plaintext_size];
    memset(ciphertext, 0, plaintext_size);

    aes_ctr_encrypt(key, nonce, plaintext, plaintext_size, ciphertext);


    outFile = fopen(outFileName,"w");
    for(int i = 0 ; i<plaintext_size ; i++){
        fprintf(outFile,"%c",ciphertext[i]);
    }










//    uint8_t state[4][4] = {
//     {0x01, 0x03, 0x01, 0x01},
//     {0x01, 0x02, 0x03, 0x01},
//     {0x01, 0x01, 0x02, 0x03},
//     {0x03, 0x01, 0x01, 0x02}
//   };
//   uint8_t key[4][4] = {
//     {0x0f, 0x0e, 0x0d, 0x0c},
//     {0x0b, 0x0a, 0x09, 0x08},
//     {0x07, 0x06, 0x05, 0x04},
//     {0x03, 0x02, 0x01, 0x00}
//   };


 

//   Ceifer(state , key , 10);

//   printf("After ceifer:\n");
//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       printf("%02x ", state[i][j]);
//     }
//     printf("\n");
//   }


    return 0;






}
