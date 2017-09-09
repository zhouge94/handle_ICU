#include "my_ini.h"

my_ini::my_ini(char *file_name)
{
    filename=file_name;
}
char *my_ini::GetIniKeyString(char *title,char *key)//从INI文件读取字符串类型数据
{
    char szLine[1024];
    static char tmpstr[1024];
    int rtnval;
    int i = 0;
    int flag = 0;
    char *tmp;
    FILE *fp;
    if((fp = fopen(filename, "r")) == NULL)
    {
        printf("have   no   such   file \n");
        return 0;
    }
    while(!feof(fp))
    {
        rtnval = fgetc(fp);
        if(rtnval == EOF) break;
        else szLine[i++] = rtnval;
        if(rtnval == '\n')
        {
            szLine[--i] = '\0';
            i = 0;
            tmp = strchr(szLine, '=');
            if(( tmp != NULL )&&(flag == 1))
            {
                if(strstr(szLine,key)!=NULL)
                {
                    //注释行
                    if ('#' == szLine[0]);
                    else if ( '/' == szLine[0] && '/' == szLine[1] );
                    else  //找打key对应变量
                    {
                        strcpy(tmpstr,tmp+1);
                        fclose(fp);
                        printf("Find [%s]->%s=%s\n\r",title,key,tmpstr);
                        return tmpstr;
                    }
                }
            }
            else
            {
                strcpy(tmpstr,"[");
                strcat(tmpstr,title);
                strcat(tmpstr,"]");
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 )  flag = 1;//找到title
            }
        }
    }
    fclose(fp);
    return 0;
}

//从INI文件读取整类型数据
int my_ini::GetIniKeyInt(char *title,char *key)
{
    return atoi(GetIniKeyString(title,key));
}

int my_ini::test(void)
{
    int iCatAge;

    char *a = GetIniKeyString("CAT","name");
    printf("%s\r\n",a);
    iCatAge = GetIniKeyInt("CAT","age");
    printf("%d\r\n",iCatAge);

    return 0;
}
