
void cleanscreen();
void writechar();

int kernalmain()
{
  cleanscreen();
  writechar();
  while(1);
  return 0;
}

void cleanscreen()
{
	unsigned int base = 0xb8000;
      int  i=0;   
	for(i=0; i<1024; ++i)
	{
		unsigned int *num = (unsigned int *)(base+i*4+1);
		*num = 0x0f200f20;
	}
}

void writechar()
{
	char str[30]={'W', 'e', ' ', 'a', 'r', 'e', ' ', 'n', 'o', 'w',' ', 'i', 'n', ' ', 
			't', 'h', 'e', ' ', 'k', 'e', 'r', 'n', 'a', 'l', ' ', 'C', '!', '!', '!', '\0'};
	unsigned char * putschar = (void *)(0xb8000);
	int i=0;
        while(str[i]!='\0')
	{
		putschar[i*2+1]=str[i];
		i++;
	}
}
