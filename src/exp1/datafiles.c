#include "stdio.h"

/**
 * @brief 创建数据文件，读写数据文件 
 *
 * @param 
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16 
 **/
 
 struct student
 {
 	char id[10];
 	char name[10]; 
 };
 
 int createDataFile()
 {
/**
 * @brief 创建数据文件 
 *
 * @param 
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16 
 **/
 	FILE *fp;
 	char ch;
 	fp=fopen("datafile1.dat", "wb");
	fseek( fp, 8192*2, SEEK_SET );		//创建一个2个块的数据文件 
	ch = '\0';
	fwrite(ch, sizeof(char), 1, fp );	
 	fclose(fp);
 	return 0;
 };
 
  
 int writePageToFile()
 {
/**
 * @brief 写数据文件 
 *
 * @param 
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16 
 **/
 	printf("Write a page to file.\n");
 	FILE *fp;
 	struct student s1;
 	strcpy( s1.id, "2016000669");
 	strcpy( s1.name, "冷友方");
 	fp=fopen("datafile1.dat", "wb");
	fwrite(&s1, sizeof(struct student), 1, fp); //把数据写回文件 
	fclose(fp);
 }; 

 int readDataFile()
 {
/**
 * @brief 读数据文件 
 *
 * @param 
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16 
 **/
 	printf("Read a page from file.\n");
	FILE *fp;
 	struct student s2;
 	fp=fopen("datafile1.dat", "rb");
	fread(&s2,sizeof(struct student),1,fp);//把文件内容读入到缓存
	fclose(fp);
	printf("%s", s2.id);
	printf("%s", s2.name);
 };