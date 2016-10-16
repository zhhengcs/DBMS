#include "stdio.h"
#include "structfile.c"

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
 	char id[11];
 	char name[10]; 
 };
 
 int createDataFile(long fileSize)
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
 	struct DataFile df1;
	
	/** 初始化文件头信息 **/ 
	df1.pageOfFile = fileSize/8192;
 	df1.sizePerPage = 8192;
 	df1.freeCount = fileSize/8192-1;
 	df1.pagefreeHead = 2;
	df1.pagefreeTail = 128;
	df1.pageDataMemoryHead = 2; 	
 	/** 初始化文件头信息 **/
 	
 	printf("Start the database.\n");
 	fp=fopen("datafile1.dat", "wb");
 	
 	fwrite(&df1, sizeof(struct DataFile), 1, fp );
	fseek( fp, df1.pageOfFile*df1.sizePerPage, SEEK_SET);	//创建一个数据文件 
	ch = '\0';
	fwrite(&ch, sizeof(char), 1, fp);	
 	fclose(fp);
 	printf("Db start successfully.\n");
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
 	fp=fopen("datafile1.dat", "rb+");
 	fseek(fp, 8192, SEEK_SET);
	fwrite(&s1, sizeof(struct student), 1, fp ); //把数据写回文件 
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
 	struct DataFile df2;
 	fp=fopen("datafile1.dat", "rb");
 	rewind(fp);
 	fread(&df2, sizeof(struct DataFile),1,fp);//把文件内容读入到缓存
 	
 	fseek(fp, 8192, SEEK_SET);
	fread(&s2, sizeof(struct student),1,fp);//把文件内容读入到缓存

	fclose(fp);

	printf("************DB file information*************\n");
	printf("Page of file: %d\n", df2.pageOfFile);
	printf("Size per page: %d\n", df2.sizePerPage);
	printf("Number of free page: %d\n", df2.freeCount);
	
	printf("\n************Data information*************\n");
	printf("Student id:%s\n", s2.id);
	printf("Student name:%s\n", s2.name);
 };
 
 
 int startDataFile()
 {
	createDataFile(1024*1024);
	writePageToFile();
	readDataFile();
 }
 