#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>


#define urlLength 1000
#define Base_Url "www.chitkara.edu.in"

char urlofthispage[]="";
int Check_Argument(int check)  // function to check wheater user has entered total 3 arguments or not!!
{
    if(check==4)
        return 1;
        printf("ARGUMENTS ARE NOT PROPER");
        exit(0);
}
void removeWhiteSpace(char* html) 
{
  int i;
  char *buffer = malloc(strlen(html)+1), *p=malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++) 
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}

int Check_Depth(char* argv[])  // function to check wheater depth entered in postive or negative!!
{
    int depth;
    sscanf(argv[2], "%d" , &depth);
    if(depth<0)
    {
        printf("DEPTH CAN'T BE NEGATIVE");
        exit(0);
    }
    if(depth>30){
        printf("DEPTH IS TOO HIGH");
        exit(0);
    }
    return 1;
}
int Check_Url(char* argv[])  // function to check wheater url is correct or not!!
{
    char v[]="wget --spider ";
    char *b = (char*)malloc(sizeof(char)*300);
    strcat(b,"wget --spider ");
    strcat(b,argv[1]);
    if(!system(b))
    {
        if(strcmp(Base_Url,argv[1]))
        {
            printf("URL IS NOT CORRECT");
            exit(0);
        }
        else
            return 1;   
    }
}

int Check_Dir(char* argv[])
{
    struct stat statbuf;
    if( stat(argv[3],&statbuf) == -1)
    {
        char str[100]="mkdir ";
        strcat(str,argv[3]);
        printf("%s",argv[3]);
        system(str);
        return 1;
    
    }
    if( !S_ISDIR(statbuf.st_mode))
    {
        printf("Invalid directory entry.Your input is't a directory\n");
        exit(0);
    }
    if( (statbuf.st_mode & S_IWUSR) !=S_IWUSR)
    {
        printf("Invalid directory entry. It isn't writable\n");
        exit(0);
    }
    return 1;
}

void get_Page(char *url,char *argv[])  // function to fetch url from user and contact in urlBuffer and fetch page source code and add it to temp file
{
    char urlBuffer[urlLength + 300] = {0};
    strcat(urlBuffer, "wget -O ");
    strcat(urlBuffer, "./temp.txt ");
    strcat(urlBuffer, url); 
    printf("\n\n%s\n\n",urlBuffer);
    system(urlBuffer);
    //system("clear");
    printf("Page fetched successfully");
}

void Check_Arguments(int argc,char* argv[])  // function to check whether all arguments are correct or not!!
{
    	if(Check_Argument(argc))
        {
            if(Check_Depth(argv))
            {
                if(Check_Url(argv))
                {
                   // system("clear");
                    printf("URL is corrent");
                    if(Check_Dir(argv))
                    {
                        printf("All Arguments are correct\n");
                    }
                }
            }
        }
}
int GetNextURL(char* html, char* urlofthispage, char* result, int pos) 
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) 
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) 
  {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');
    
    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;    

    p2 = strpbrk(p1, "\'\">");
    if (!p2) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#') 
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4)) 
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
	// again...probably good to recursively keep going..
	// NEW
        
        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }    
  return -1;
}


int main(int argc,char* argv[])
{
    Check_Arguments(argc,argv);
    get_Page(argv[1],argv);
    struct stat st;
stat("temp.txt",&st);
int file_size=st.st_size;
file_size++;
    char *data;
    data = (char *)malloc(file_size*sizeof(char));
    FILE *fp;
    int i=0;
    char ch;
    fp = fopen("temp.txt", "r+");
    ch = getc(fp);
    while (ch!=EOF)
    {
       data[i++] +=ch;
       ch = getc(fp);
       
    }
    char *links[100];
    int j=0;
     fclose(fp);
     char *result;
     result = (char *)malloc(file_size*sizeof(char));
     int ans = GetNextURL(data,argv[1],result, 0); 
    while(j<100){
        links[j] = (char *)malloc(200*sizeof(char));
            strcpy(links[j++],result);
             ans = GetNextURL(data,argv[1],result, ans);
    }
    for(int j=0;j<100;j++){
        printf("\n%s",links[j]);
    }
 
    return 0;
}