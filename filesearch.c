#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<process.h>

char* path = "E:\\HUGE\\大数据相关数据\\大数据相关数据\\kaifang.txt";
char* index = "D:\\indexkaifang.txt";

#define N 20151574
#define THREADNUMBER 100   //100个线程

struct index
{
          int* pindex;    //索引内存首地址
          int length;     //长度
}INDEX;

/*多线程*/
struct info
{
          int* pstart;   //开始地址
          int length;   //长度
          char findstr[20];   //要查找的数据
          int id;   //线程编号
};

void init()
{
          INDEX.length = N;
          INDEX.pindex = (int*)calloc(N,sizeof(int));
          FILE* pf = fopen(path, "rb");
          if (pf == NULL)
          {
                    printf("分配失败\n");
          }
          else
          {
                    int line = 0;
                    for (int i = 0; i < N; ++i)
                    {
                              char str[1500] = { 0 };
                              fgets(str, 1500, pf);
                              INDEX.pindex[i] = line;
                              int length = strlen(str);
                              line += length;    //索引的位置
                    }
          }
          fclose(pf);
          FILE* pfw = fopen(index,"wb");
          fwrite(INDEX.pindex, sizeof(int), INDEX.length, pfw);
          fclose(pfw);
}

/*快速启动*/
void kickstart()
{
          INDEX.length = N;
          INDEX.pindex = (int*)calloc(N, sizeof(int));
          FILE* pf = fopen(index, "rb");
          if (pf == NULL)
          {
                    printf("分配失败\n");
          }
          else
          {
                    fread(INDEX.pindex, sizeof(int), INDEX.length, pf);
          }
          fclose(pf);
}

void thread(void* p)
{
          FILE* pf = fopen(path, "rb");
          struct info* p1 = p;   //指针类型转换
          for (int i = 0; i < p1->length; ++i)
          {
                    int temp = p1->pstart[i];
                    fseek(pf, temp, SEEK_SET);   //根据偏移读取文件
                    char str[512] = { 0 };
                    fgets(str, 512, pf);
                    char* px = strstr(str, p1->findstr);
                    if (px != NULL)
                    {
                              printf("线程%d，找到%s", p1->id, str);    //打印找到的数据
                    }
          }
          fclose(pf);
}

int main()
{
          kickstart();
          printf("请输入要查询的数据:\n");
          char str[100] = { 0 };
          scanf("%s", str);
          struct info pthread[THREADNUMBER] = { 0 };
          if (N % THREADNUMBER == 0)
          {
                    for (int i = 0; i < THREADNUMBER; ++i)
                    {
                              pthread[i].id = i;
                              strcpy(pthread[i].findstr, str);  //拷贝
                              pthread[i].length = N / THREADNUMBER;
                              pthread[i].pstart = INDEX.pindex + i * (N / THREADNUMBER);
                              _beginthread(thread, 0, &pthread[i]);
                    }
          }
          else
          {
                    for (int i = 0; i < THREADNUMBER - 1; ++i)
                    {
                              pthread[i].id = i;
                              strcpy(pthread[i].findstr, str);  //拷贝
                              pthread[i].length = N / (THREADNUMBER - 1);
                             pthread[i].pstart = INDEX.pindex + i * (N / (THREADNUMBER - 1));
                    }
                    pthread[THREADNUMBER - 1].id = THREADNUMBER - 1;
                    strcpy(pthread[THREADNUMBER - 1].findstr, str);  //拷贝
                    pthread[THREADNUMBER - 1].length = N % (THREADNUMBER - 1);
                    pthread[THREADNUMBER - 1].pstart = INDEX.pindex + (THREADNUMBER - 1) * (N / (THREADNUMBER - 1));
                    _beginthread(thread, 0, &pthread[THREADNUMBER - 1]);
          }
          system("pause");
          return 0;
}
