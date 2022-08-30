/*
*********************************************************************************************************
*
*	模块名称 : SD卡Fat文件系统和SD卡模拟演示模块。
*	文件名称 : demo_sd_filex.c
*	版    本 : V1.0
*	说    明 : 演示如何创建文件、读取文件、创建目录和删除文件
*			   并测试了文件读写速度.支持以下6个功能，用户通过电脑端串口软件发送数字给开发板即可:
*              1 - 显示根目录下的文件列表
*              2 - 创建一个新文件armfly.txt
*              3 - 读armfly.txt文件的内容
*              4 - 创建目录
*              5 - 删除文件和目录
*              6 - 读写文件速度测试
*
*              另外注意，SD卡模拟U盘是用的MicroUSB接口。
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2021-01-30   Eric2013    正式发布
*
*	Copyright (C), 2021-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "sys.h"


/* 用于测试读写速度 */
#define TEST_FILE_LEN			(2*1024*1024)	/* 用于测试的文件长度 */
#define BUF_SIZE				(32*1024)		/* 每次读写SD卡的最大数据长度 */

/* 仅允许本文件内调用的函数声明 */
static void DispMenu(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void CreateDir(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);

/* 给FileX开的动态内存 */
uint32_t media_memory[8*1024];  
char FsReadBuf[1024];
char FsWriteBuf[1024] = {"ThreadX FileX Write Demo \r\n www.armfly.com \r\n"};
uint8_t g_TestBuf[BUF_SIZE];

/* FileX相关变量 */
FX_MEDIA     sdio_disk; 
FX_FILE      fx_file;
CHAR entry_name[FX_MAX_LONG_NAME_LEN];
extern VOID  fx_stm32_sd_driver(FX_MEDIA *media_ptr);

/*
*********************************************************************************************************
*	函 数 名: DemoFatFS
*	功能说明: FatFS文件系统演示主程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DemoFileX(void)
{
	uint8_t cmd;

	/* 打印命令列表，用户可以通过串口操作指令 */
	DispMenu();
    
    /* 初始化 */
    fx_system_initialize();

    /* 启动1个500ms的自动重装的定时器 */	
	//bsp_StartAutoTimer(0, 500);	

	while (1)
	{  
        tx_thread_sleep(10);		
		/* 判断定时器超时时间 */
//		if (bsp_CheckTimer(0))	
//		{            
//			/* 每隔500ms 进来一次 */  
//			bsp_LedToggle(2);
//		}

		if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{
			printf("\r\n");
			switch (cmd)
			{
				case '1':
					printf("【1 - ViewRootDir】\r\n");
					ViewRootDir();		/* 显示SD卡根目录下的文件名 */
					break;

				case '2':
					printf("【2 - CreateNewFile】\r\n");
					CreateNewFile();	/* 创建一个新文件,写入一个字符串 */
					break;

				case '3':
					printf("【3 - ReadFileData】\r\n");
					ReadFileData();		/* 读取根目录下armfly.txt的内容 */
					break;

				case '4':
					printf("【4 - CreateDir】\r\n");
					CreateDir();		/* 创建目录 */
					break;

				case '5':
					printf("【5 - DeleteDirFile】\r\n");
					DeleteDirFile();	/* 删除目录和文件 */
					break;

				case '6':
					printf("【6 - TestSpeed】\r\n");
					WriteFileTest();	/* 速度测试 */
					break;
				
				default:
					DispMenu();
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("请选择操作命令，打开SD卡模拟U盘操作期间不支持再调用命令1-6:\r\n");
	printf("1 - 显示根目录下的文件列表\r\n");
	printf("2 - 创建一个新文件armfly.txt\r\n");
	printf("3 - 读armfly.txt文件的内容\r\n");
	printf("4 - 创建目录\r\n");
	printf("5 - 删除文件和目录\r\n");
	printf("6 - 读写文件速度测试\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示SD卡根目录下的文件名
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
extern SD_HandleTypeDef uSdHandle;
static void ViewRootDir(void)
{
    UINT status;
    UINT attributes;
    ULONG size;
    UINT year;
    UINT month;
    UINT day;
    UINT hour;
    UINT minute;
    UINT second;
    UINT cnt;
    ULONG64 available_bytes;
	
    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    }
    
    
    printf("属性        |  文件大小 | 短文件名 | 长文件名\r\n");
	for (cnt = 0; ;cnt++)
	{
        /* 读取目录项，索引会自动下移 */
        status = fx_directory_next_full_entry_find(&sdio_disk,
                                                    entry_name, 
                                                    &attributes, 
                                                    &size,
                                                    &year, &month, &day, 
                                                    &hour, &minute, &second);
        
        if (status != FX_SUCCESS || entry_name[0] == 0)
		{
			break;
		}

		if (entry_name[0] == '.')
		{
			continue;
		}

		/* 判断是文件还是子目录 */
		if (attributes & FX_DIRECTORY)
		{
			printf("目录  ");
		}
		else
		{
			printf("文件  ");
		}

		/* 打印文件大小, 最大4G */
		printf(" %10d", (int)size);


		printf("  %s\r\n", (char *)entry_name);	/* 长文件名 */
	}
    
    /* SD卡剩余容量大小 */
    status = fx_media_extended_space_available(&sdio_disk, &available_bytes);    

    if (status == FX_SUCCESS)
    {
        printf("SD卡剩余容量大小 -- %lldMB\r\n", available_bytes/1024/1024);
    }
    
    /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }
}

/*
*********************************************************************************************************
*	函 数 名: CreateNewFile
*	功能说明: 在SD卡创建一个新文件，文件内容填写“www.armfly.com”
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
    UINT status;

    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    }
    
    /* 创建文件 */
    status =  fx_file_create(&sdio_disk, "armfly.txt");

    /* 检测状态 */
    if (status != FX_SUCCESS)
    {
        /* 失败的话，可以考虑二次创建 */
        if (status != FX_ALREADY_CREATED)
        {
            printf("创建文件失败\r\n");
        }
    }
    
    /* 打开文件  */
    status =  fx_file_open(&sdio_disk, &fx_file, "armfly.txt", FX_OPEN_FOR_WRITE);

    if (status != FX_SUCCESS)
    {
        printf("打开文件失败\r\n");
    }
    
    /* 设置到起始位置读取  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("设置读取位置失败\r\n");       
    }


    /* 写入字符串到文件  */
    status =  fx_file_write(&fx_file, FsWriteBuf, strlen(FsWriteBuf));

    if (status == FX_SUCCESS)
    {
        printf("armfly.txt 文件写入成功\r\n");        
    }
    else
    {
        printf("armfly.txt 文件写入失败 %d\r\n", status); 
    }

    /* 关闭文件  */
    status =  fx_file_close(&fx_file);

    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("关闭文件失败\r\n");    
    }

    /* 保证文件写入全部生效 */
    status = fx_media_flush(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("flush失败\r\n");   
    }
    
     /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }   
}

/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(void)
{
    UINT status;
    ULONG bw;
	
    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    }
    

    /* 打开文件  */
    status =  fx_file_open(&sdio_disk, &fx_file, "armfly.txt", FX_OPEN_FOR_READ);

    if (status != FX_SUCCESS)
    {
        printf("打开文件失败\r\n");
    } 

    /* 设置到起始位置读取  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("设置读取位置失败\r\n");       
    }

    /*  */
    status =  fx_file_read(&fx_file, FsReadBuf, sizeof(FsReadBuf), &bw);

    printf("--%sReadLen = %d\r\n", FsReadBuf, (int)bw);

    if ((status != FX_SUCCESS))
    {
        printf("读取失败\r\n");     
    }    
    
    /* 关闭文件  */
    status =  fx_file_close(&fx_file);

    if (status != FX_SUCCESS)
    {
        printf("关闭文件失败\r\n");    
    }
    
     /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }   
}

/*
*********************************************************************************************************
*	函 数 名: CreateDir
*	功能说明: 在SD卡根目录创建Dir1和Dir2目录，在Dir1目录下创建子目录Dir1_1
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateDir(void)
{
    UINT status;
	
    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    } 

    /* 创建目录/Dir1 */
    status = fx_directory_create(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
    {
        printf("文件夹Dir1创建成功\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir1 目录已经存在(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir1 失败 (%d)\r\n", status);
		return;
	}  
    
	/* 创建目录/Dir2 */
    status = fx_directory_create(&sdio_disk, "Dir2");
    
    if (status == FX_SUCCESS)
    {
        printf("文件夹Dir2创建成功\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir2 目录已经存在(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir2 失败 (%d)\r\n", status);
		return;
	}     

	/* 创建子目录 /Dir1/Dir1_1	   注意：创建子目录Dir1_1时，必须先创建好Dir1 ？ */    
    status = fx_directory_create(&sdio_disk, "Dir1/Dir1_1");
    
    if (status == FX_SUCCESS)
    {
        printf("文件夹Dir1/Dir1_1创建成功\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir1/Dir1_1 目录已经存在(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir1/Dir1_1 失败 (%d)\r\n", status);
		return;
	}  
    
    /* 保证文件写入全部生效 */
    status = fx_media_flush(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("flush失败\r\n");   
    }
    
     /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }  
}

/*
*********************************************************************************************************
*	函 数 名: DeleteDirFile
*	功能说明: 删除SD卡根目录下的 armfly.txt 文件和 Dir1，Dir2 目录
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
    UINT status;
    UINT i;
    char path[50];
    
    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    }

    /* 删除目录/Dir1 【因为还存在目录非空（存在子目录)，所以这次删除会失败】*/
    status = fx_directory_delete(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
	{
		printf("删除目录Dir1成功\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("没有发现文件或目录 :%s\r\n", "/Dir1");
	}
	else
	{
		printf("删除Dir1失败(错误代码 = %d) 文件只读或目录非空\r\n", status);
	}
    
    
 	/* 先删除目录/Dir1/Dir1_1 */  
    status = fx_directory_delete(&sdio_disk, "Dir1/Dir1_1");
    
    if (status == FX_SUCCESS)
	{
		printf("删除目录Dir1/Dir1_1成功\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("没有发现文件或目录 :%s\r\n", "Dir1/Dir1_1");
	}
	else
	{
		printf("删除Dir1/Dir1_1失败(错误代码 = %d) 文件只读或目录非空\r\n", status);
	}    
    
    /* 删除目录/Dir1*/  
    status = fx_directory_delete(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
	{
		printf("删除目录Dir1成功\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("没有发现文件或目录 :%s\r\n", "Dir1");
	}
	else
	{
		printf("删除Dir1失败(错误代码 = %d) 文件只读或目录非空\r\n", status);
	}   

    /* 删除目录/Dir2*/  
    status = fx_directory_delete(&sdio_disk, "Dir2");
    
    if (status == FX_SUCCESS)
	{
		printf("删除目录Dir2成功\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("没有发现文件或目录 :%s\r\n", "Dir2");
	}
	else
	{
		printf("删除Dir2失败(错误代码 = %d) 文件只读或目录非空\r\n", status);
	}   

	/* 删除文件 armfly.txt */
    status = fx_file_delete(&sdio_disk, "armfly.txt");
    
    if (status == FX_SUCCESS)
	{
		printf("删除目录armfly.txt成功\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("没有发现文件或目录 :%s\r\n", "armfly.txt");
	}
	else
	{
		printf("删除armfly.txt失败(错误代码 = %d) 文件只读或目录非空\r\n", status);
	}   

	/* 删除文件 speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(path, "Speed%02d.txt", i);/* 每写1次，序号递增 */	
        
        status = fx_file_delete(&sdio_disk, path);
    
        if (status == FX_SUCCESS)
        {
            printf("删除文件%s成功\r\n", path);
        }
        else if (status == FX_NOT_FOUND)
        {
            printf("没有发现文件:%s\r\n", path);
        }
        else
        {
            printf("删除%s文件失败(错误代码 = %d) 文件只读或目录非空\r\n", path, status);
        }   
	}

     /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }  
}

/*
*********************************************************************************************************
*	函 数 名: WriteFileTest
*	功能说明: 测试文件读写速度
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void WriteFileTest(void)
{
    UINT status;
    char path[64]; 
    
    ULONG bw;
	uint32_t i,k;
	uint32_t runtime1,runtime2,timelen;
	uint8_t err = 0;
	static uint8_t s_ucTestSn = 0;

	
	for (i = 0; i < sizeof(g_TestBuf); i++)
	{
		g_TestBuf[i] = (i / 512) + '0';
	}
	
    
    /* 挂载SD卡 */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("挂载文件系统失败 -- %d\r\n", status);
        return;
    }
    
    /* 打开文件 */
	sprintf(path, "Speed%02d.txt", s_ucTestSn++); /* 每写1次，序号递增 */	
    
    /* 写一串数据 */
	printf("开始写文件%s %dKB ...\r\n", path, TEST_FILE_LEN / 1024);
    
    /* 创建文件 */
    status =  fx_file_create(&sdio_disk, path);

    /* 检测状态 */
    if (status != FX_SUCCESS)
    {
        /* 失败的话，可以考虑二次创建 */
        if (status != FX_ALREADY_CREATED)
        {
            printf("创建文件失败\r\n");
        }
    }
    
    /* 打开文件  */
    status =  fx_file_open(&sdio_disk, &fx_file, path, FX_OPEN_FOR_WRITE);

    if (status != FX_SUCCESS)
    {
        printf("打开文件失败\r\n");
    }
    
    /* 设置到起始位置读取  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("设置读取位置失败\r\n");       
    }
    
    runtime1 = bsp_GetRunTime();	/* 读取系统运行时间 */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
        /* 写入字符串到文件  */
        status =  fx_file_write(&fx_file, g_TestBuf, sizeof(g_TestBuf));

        if (status == FX_SUCCESS)
        {
            if (((i + 1) % 8) == 0)
			{
				printf(".");
			}     
        }
        else
        {
            err = 1;
			printf("%s文件写失败\r\n", path);
			break; 
        }
	}
	runtime2 = bsp_GetRunTime();	/* 读取系统运行时间 */
	
	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  写耗时 : %dms   平均写速度 : %dB/S (%dKB/S)\r\n",
                timelen,
                (TEST_FILE_LEN * 1000) / timelen,
                ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}

    /* 关闭文件  */
    status =  fx_file_close(&fx_file);

    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("关闭文件失败\r\n");    
    }

    /* 保证文件写入全部生效 */
    status = fx_media_flush(&sdio_disk);
    
    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("fx_media_flush失败\r\n");    
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /* 开始读文件测试 */
	printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
    
      /* 打开文件  */
    status =  fx_file_open(&sdio_disk, &fx_file, path, FX_OPEN_FOR_READ);

    if (status != FX_SUCCESS)
    {
        printf("打开文件失败\r\n");
    }
    
    /* 设置到起始位置读取  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("设置读取位置失败\r\n");       
    }
    
    runtime1 = bsp_GetRunTime();	/* 读取系统运行时间 */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
        /* 写入字符串到文件  */
        
        status =  fx_file_read(&fx_file, g_TestBuf, sizeof(g_TestBuf), &bw);
        if (status == FX_SUCCESS)
        {
        	if (((i + 1) % 8) == 0)
			{
				printf(".");
			}

			/* 比较写入的数据是否正确，此语句会导致读卡速度结果降低到 3.5MBytes/S */
			for (k = 0; k < sizeof(g_TestBuf); k++)
			{
				if (g_TestBuf[k] != (k / 512) + '0')
				{
				  	err = 1;
					printf("Speed1.txt 文件读成功，但是数据出错\r\n");
					break;
				}
			}
			if (err == 1)
			{
				break;
			}
        }
        else
        {
           	err = 1;
			printf("Speed1.txt 文件读失败\r\n");
			break; 
        }
	}
	runtime2 = bsp_GetRunTime();	/* 读取系统运行时间 */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  读耗时 : %dms   平均读速度 : %dB/S (%dKB/S)\r\n", timelen,
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
    
     /* 卸载SD卡 */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("卸载文件系统卸载失败 -- %d\r\n", status);
    }  

}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
