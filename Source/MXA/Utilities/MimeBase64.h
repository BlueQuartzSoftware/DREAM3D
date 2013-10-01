
#include <stdio.h>

const unsigned char DCODE_MIMIE[][16] = {
                    {0x0},                        //0
                    {0x0},                        //1
                    {0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63},        //2
                    {52,53,54,55,56,57,58,59,60,61,0},                              //3
                    {0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14},       //4
                    {15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0},   //5
                    {0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40}, //6
                    {41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0}
                    };


const unsigned char MIME_ARRAY[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
                  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
                  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
                  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
                  };

unsigned char DCode_mime(unsigned char code,const unsigned char *pMimiArray,int len)
{
  unsigned char index = 0;
  for(index = 0; index < len ; index++)
  {
    if(pMimiArray[index] == code)
      break;
  }
  return index;
}

int Code_MIME(unsigned char *pInBuff , int len , unsigned char *pOutBuff, int *RLen)
{
  int i = 0 , j = 0;
  unsigned char pCode[3] = {0};
  unsigned char pDCode[3] = {0};
  unsigned char index = 0x0;
  while(i < len)
  {
    pCode[0] = (i++ < len) ? pInBuff[i-1] : 0x0;
    pCode[1] = (i++ < len) ? pInBuff[i-1] : 0x0;
    pCode[2] = (i++ < len) ? pInBuff[i-1] : 0x0;
    //
    index = (pCode[0] & 0xFC)>>2;
    pOutBuff[j++] = MIME_ARRAY[index];
    index = ((pCode[0] & 0x03) << 4) | ((pCode[1] & 0xF0)>>4);
    pOutBuff[j++] = MIME_ARRAY[index];
    //
    index = ((pCode[1] & 0x0F) << 2) | ((pCode[2] & 0xC0) >> 6);
    pOutBuff[j++] = i > len ? '=' : MIME_ARRAY[index];
    index = pCode[2] & 0x3F;
    pOutBuff[j++] = i >= len ? '=' : MIME_ARRAY[index];

  }
  *RLen = j;
  return 0;
}
int DCode_MIME(unsigned char *pInBuff,int len , unsigned char *pOutBuff , int *RLen)
{
  int i = 0,j= 0;
  unsigned char pCode[4] = {0x0};
  unsigned char code1 = 0x0,code2 = 0x0;
  unsigned char index = 0x0;
  while(i < len)
  {
    pCode[0] = pInBuff[i++];
    pCode[1] = pInBuff[i++];
    pCode[2] = pInBuff[i++];
    pCode[3] = pInBuff[i++];
    //DCode_mime 只有6位
    code1 = DCODE_MIMIE[(pCode[0]&0xF0)>>4][pCode[0]&0x0F];//DCode_mime(pCode[0],MIME_ARRAY,sizeof(MIME_ARRAY));
    code2 = DCODE_MIMIE[(pCode[1]&0xF0)>>4][pCode[1]&0x0F];//DCode_mime(pCode[1],MIME_ARRAY,sizeof(MIME_ARRAY));
    pOutBuff[j++] = ((code1 << 2))|((code2 & 0x30) >> 4);
    if(pCode[2] != '=')
    {
      code1 = DCODE_MIMIE[(pCode[2]&0xF0)>>4][pCode[2]&0x0F];//DCode_mime(pCode[2],MIME_ARRAY,sizeof(MIME_ARRAY));
      pOutBuff[j++] = ((code2 & 0x0F) << 4) | ((code1 & 0x3C) >> 2);
    }
    else
      ;
    if(pCode[3] != '=')
    {
      code2 = DCODE_MIMIE[(pCode[3]&0xF0)>>4][pCode[3]&0x0F];//DCode_mime(pCode[3],MIME_ARRAY,sizeof(MIME_ARRAY));
      pOutBuff[j++] = ((code1 & 0x03) << 6) | code2;
    }
  }
  *RLen = j;
  return 0;
}

int main(int argc , char *argv[])
{
  unsigned  char pBuff[1024] = {0};
  unsigned char pUBuff[1024] = {0};
  int Len = 0x0;
  unsigned char piBuff[] = "";

  Code_MIME(piBuff,sizeof(piBuff),pBuff,&Len);
  DCode_MIME(pBuff,Len,pUBuff,&Len);
  printf("B C s = %s\r\nA C s = %s\r\nA D s = %s\r\n",piBuff,pBuff,pUBuff);
  return 0;
}
