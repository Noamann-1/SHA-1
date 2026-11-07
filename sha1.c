#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <stdint.h>
void pauseStage()
{
    printf("\nPress Enter to continue...");
    getchar();
}
uint32_t rotateLeft(uint32_t value,int bits)
{
    return (value<<bits) | (value>>(32-bits));
} 
void expandWords(uint32_t W[80])
{
    for(int t=16;t<80;t++)
    {
        W[t]=rotateLeft(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16],1);
    }
}
unsigned char *padding(unsigned char *m,size_t actual_len,size_t padded_len)
{
    unsigned char *temp=realloc(m,padded_len);
    if (temp == NULL)
    {
        printf("Memory Reallocation Failed!\n");
        free(m);
        return NULL;
    }
    m=temp;
    m[actual_len]=0x80;        //1 byte--->10000000
    uint64_t bit_len=(uint64_t)actual_len*8; //actual msg length bytes to bits
   
    size_t length_position=padded_len-8;               //where the 8 bytes denoting length start
    memset(m+actual_len+1,0,length_position-(actual_len+1));

    for (int i=0;i<8;i++)
    {  
        m[padded_len-1-i]=(unsigned char)(bit_len & 0xFF);  //& for extracting the lower byte......
        bit_len >>=8;  //rshift by 8
    }
    return m;
}
void wordPrepare(unsigned char *block,uint32_t W[80])
{
    for (int i=0;i<16;i++)
    {
        W[i]=((uint32_t)block[i*4]<<24)|
               ((uint32_t)block[i*4+1]<<16) |
               ((uint32_t)block[i*4+2]<<8)  |
               ((uint32_t)block[i*4+3]);
    }
}
void breakdown(size_t padded_len,unsigned char *m)
{
    size_t blocks=padded_len/64;  //calculating no of blocks..../
    printf("No of blocks= %zu\n",blocks);
    for (size_t b=0;b<blocks;b++)
    {
        printf("\n========== BLOCK %zu ==========\n", b);
        unsigned char *block=m+(b*64);
        uint32_t W[80];

        wordPrepare(block, W); //create first 16 words...
        expandWords(W);  //expand to 80 words...

        for (int i=0;i<80;i++)
        {
            printf("W[%2d] = %08X\n",i,W[i]);
        }
    }
}
void processBlock(uint32_t W[80],uint32_t *H0,uint32_t *H1,uint32_t *H2,uint32_t *H3,uint32_t *H4)
{
    uint32_t a=*H0;
    uint32_t b=*H1;
    uint32_t c=*H2;
    uint32_t d=*H3;
    uint32_t e=*H4;
    uint32_t f,K;
    for(int t=0;t<80;t++)
    {
        
        if(t<20)
        {
            f=(b&c) | (~b&d);
            K=0x5A827999;
        }
        else if(t<40)
        {
            f=b^c^d;
            K=0x6ED9EBA1;
        }
        else if(t<60)
        {
            f=(b&c) | (b&d) | (c&d);
            K=0x8F1BBCDC;
        }
        else
        {
            f=b^c^d;
            K=0xCA62C1D6;
        }
        uint32_t TEMP=rotateLeft(a,5)+f+e+K+W[t];
        e=d;
        d=c;
        c=rotateLeft(b, 30);
        b=a;
        a=TEMP;
    }
    *H0+= a; *H1+= b; *H2+= c;
    *H3+= d; *H4+= e;
}
void sha1(unsigned char *msg, size_t padded_len)
{
    uint32_t H0=0x67452301;
    uint32_t H1=0xEFCDAB89;
    uint32_t H2=0x98BADCFE;
    uint32_t H3=0x10325476;
    uint32_t H4=0xC3D2E1F0;
    size_t blocks=padded_len/64;
    for (size_t b=0;b<blocks;b++)
    {
        unsigned char *block=msg+(b*64);
        uint32_t W[80];
        wordPrepare(block,W);
        expandWords(W);
        processBlock(W,&H0,&H1,&H2,&H3,&H4);
    }
    printf("\nMessage Digest: ");
    printf("%08X%08X%08X%08X%08X\n",H0,H1,H2,H3,H4);
    size_t digestSize=sizeof(H0)+sizeof(H1)+sizeof(H2)+sizeof(H3)+sizeof(H4);
    printf("Message Digest Size= %zu bits",digestSize*8);
}
int main()
{
    int length,choice;
    printf("\n========== SHA-1 IMPLEMENTATION ==========\n");
    do{
    printf("Enter Max Length Your Message: ");
    if(scanf("%d",&length)!=1 || length<=0)
    { 
        printf("Invalid Length."); 
        return 0; 
    }
    getchar();

    unsigned char *msg = malloc(length + 1);
    if(msg==NULL)
    {
        printf("Allocation Failed!!!!|\n");
        return 0;
    }
    printf("Enter text:");
    if(fgets((char *)msg,length+1,stdin)==NULL)
    {
        printf("Input Error!\n");
        free(msg);
        return 1;
    }
    if(strchr((char *)msg, '\n') == NULL && !feof(stdin))
    {
        int ch;
        while((ch = getchar())!='\n' && ch!=EOF);
        printf("\nError: Message exceeds maximum allowed length!\n");
        free(msg);
        continue;
    }
    msg[strcspn((char *)msg, "\n")] = '\0';
    
    printf("\n========== SELECT MODE ==========\n");
    printf("1. Direct SHA-1 Hash\n");
    printf("2. Trace SHA-1 Algorithm\n");
    printf("3. Run Another Message\n");
    printf("4. Exit\n");
    scanf("%d", &choice);
    switch(choice)
    {
        case 1:
            printf("\n========== DIRECT SHA-1 MODE ==========\n");
            size_t actual_len=strlen((char *)msg);
            size_t padded_len=((actual_len+9+63)/64)*64;
            msg=padding(msg,actual_len,padded_len);
            if(msg==NULL)
            {
                return 1;
            }
            sha1(msg,padded_len);
            break;

       case 2:

            printf("\n========== SHA-1 Tracing ==========\n");

            actual_len=strlen((char *)msg);
            padded_len=((actual_len+9+63)/64)*64;
            printf("\nSTAGE 1: Original Message\n");
            printf("Message: %s\n",msg);
            printf("Message Length: %zu bytes\n",actual_len);
            pauseStage();
            msg=padding(msg, actual_len,padded_len);
            if(msg==NULL)
            {
                return 1;
            }
            printf("\nSTAGE 2: Message Padding\n");
            printf("Padded Message Size: %zu bytes\n",padded_len);
            for(size_t i=0;i<padded_len;i++)
            {
                printf("%02X ",(unsigned char)msg[i]);
                if((i+1)%16==0)
                    printf("\n");
            }
            pauseStage();
            printf("\nSTAGE 3: Block Division\n");
            size_t blocks=padded_len/64;
            printf("Total Blocks: %zu\n",blocks);
            for(size_t b=0;b<blocks;b++)
            {
                printf("\nBlock %zu\n",b);
                printf("Starting Byte: %zu\n",b*64);
                printf("Ending Byte: %zu\n",(b*64)+63);
                printf("Block Size: 64 bytes (512 bits)\n");
            }

            pauseStage();

            printf("\nSTAGE 4: Word generation and expansion\n");
            breakdown(padded_len,msg);
            pauseStage();

            printf("\nSTAGE 5: COMPRESSION AND FINAL DIGEST\n");
            sha1(msg,padded_len);
            break;
        case 3:
            printf("\nAnother Run...\n");
            break;
        case 4:
            printf("\nExiting Program...\n");
            break;
        default:
            printf("\nInvalid Choice!\n");
    }

    free(msg);
}while (choice!=4);
    return 0;
}