/*
*********************************************************************************************************
*
*	ģ������ : SD��Fat�ļ�ϵͳ��SD��ģ����ʾģ�顣
*	�ļ����� : demo_sd_filex.c
*	��    �� : V1.0
*	˵    �� : ��ʾ��δ����ļ�����ȡ�ļ�������Ŀ¼��ɾ���ļ�
*			   ���������ļ���д�ٶ�.֧������6�����ܣ��û�ͨ�����Զ˴�������������ָ������弴��:
*              1 - ��ʾ��Ŀ¼�µ��ļ��б�
*              2 - ����һ�����ļ�armfly.txt
*              3 - ��armfly.txt�ļ�������
*              4 - ����Ŀ¼
*              5 - ɾ���ļ���Ŀ¼
*              6 - ��д�ļ��ٶȲ���
*
*              ����ע�⣬SD��ģ��U�����õ�MicroUSB�ӿڡ�
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2021-01-30   Eric2013    ��ʽ����
*
*	Copyright (C), 2021-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "sys.h"


/* ���ڲ��Զ�д�ٶ� */
#define TEST_FILE_LEN			(2*1024*1024)	/* ���ڲ��Ե��ļ����� */
#define BUF_SIZE				(32*1024)		/* ÿ�ζ�дSD����������ݳ��� */

/* �������ļ��ڵ��õĺ������� */
static void DispMenu(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void CreateDir(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);

/* ��FileX���Ķ�̬�ڴ� */
uint32_t media_memory[8*1024];  
char FsReadBuf[1024];
char FsWriteBuf[1024] = {"ThreadX FileX Write Demo \r\n www.armfly.com \r\n"};
uint8_t g_TestBuf[BUF_SIZE];

/* FileX��ر��� */
FX_MEDIA     sdio_disk; 
FX_FILE      fx_file;
CHAR entry_name[FX_MAX_LONG_NAME_LEN];
extern VOID  fx_stm32_sd_driver(FX_MEDIA *media_ptr);

/*
*********************************************************************************************************
*	�� �� ��: DemoFatFS
*	����˵��: FatFS�ļ�ϵͳ��ʾ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoFileX(void)
{
	uint8_t cmd;

	/* ��ӡ�����б��û�����ͨ�����ڲ���ָ�� */
	DispMenu();
    
    /* ��ʼ�� */
    fx_system_initialize();

    /* ����1��500ms���Զ���װ�Ķ�ʱ�� */	
	//bsp_StartAutoTimer(0, 500);	

	while (1)
	{  
        tx_thread_sleep(10);		
		/* �ж϶�ʱ����ʱʱ�� */
//		if (bsp_CheckTimer(0))	
//		{            
//			/* ÿ��500ms ����һ�� */  
//			bsp_LedToggle(2);
//		}

		if (comGetChar(COM1, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
		{
			printf("\r\n");
			switch (cmd)
			{
				case '1':
					printf("��1 - ViewRootDir��\r\n");
					ViewRootDir();		/* ��ʾSD����Ŀ¼�µ��ļ��� */
					break;

				case '2':
					printf("��2 - CreateNewFile��\r\n");
					CreateNewFile();	/* ����һ�����ļ�,д��һ���ַ��� */
					break;

				case '3':
					printf("��3 - ReadFileData��\r\n");
					ReadFileData();		/* ��ȡ��Ŀ¼��armfly.txt������ */
					break;

				case '4':
					printf("��4 - CreateDir��\r\n");
					CreateDir();		/* ����Ŀ¼ */
					break;

				case '5':
					printf("��5 - DeleteDirFile��\r\n");
					DeleteDirFile();	/* ɾ��Ŀ¼���ļ� */
					break;

				case '6':
					printf("��6 - TestSpeed��\r\n");
					WriteFileTest();	/* �ٶȲ��� */
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
*	�� �� ��: DispMenu
*	����˵��: ��ʾ������ʾ�˵�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("��ѡ����������SD��ģ��U�̲����ڼ䲻֧���ٵ�������1-6:\r\n");
	printf("1 - ��ʾ��Ŀ¼�µ��ļ��б�\r\n");
	printf("2 - ����һ�����ļ�armfly.txt\r\n");
	printf("3 - ��armfly.txt�ļ�������\r\n");
	printf("4 - ����Ŀ¼\r\n");
	printf("5 - ɾ���ļ���Ŀ¼\r\n");
	printf("6 - ��д�ļ��ٶȲ���\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: ViewRootDir
*	����˵��: ��ʾSD����Ŀ¼�µ��ļ���
*	��    �Σ���
*	�� �� ֵ: ��
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
	
    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    }
    
    
    printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
	for (cnt = 0; ;cnt++)
	{
        /* ��ȡĿ¼��������Զ����� */
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

		/* �ж����ļ�������Ŀ¼ */
		if (attributes & FX_DIRECTORY)
		{
			printf("Ŀ¼  ");
		}
		else
		{
			printf("�ļ�  ");
		}

		/* ��ӡ�ļ���С, ���4G */
		printf(" %10d", (int)size);


		printf("  %s\r\n", (char *)entry_name);	/* ���ļ��� */
	}
    
    /* SD��ʣ��������С */
    status = fx_media_extended_space_available(&sdio_disk, &available_bytes);    

    if (status == FX_SUCCESS)
    {
        printf("SD��ʣ��������С -- %lldMB\r\n", available_bytes/1024/1024);
    }
    
    /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: CreateNewFile
*	����˵��: ��SD������һ�����ļ����ļ�������д��www.armfly.com��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
    UINT status;

    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    }
    
    /* �����ļ� */
    status =  fx_file_create(&sdio_disk, "armfly.txt");

    /* ���״̬ */
    if (status != FX_SUCCESS)
    {
        /* ʧ�ܵĻ������Կ��Ƕ��δ��� */
        if (status != FX_ALREADY_CREATED)
        {
            printf("�����ļ�ʧ��\r\n");
        }
    }
    
    /* ���ļ�  */
    status =  fx_file_open(&sdio_disk, &fx_file, "armfly.txt", FX_OPEN_FOR_WRITE);

    if (status != FX_SUCCESS)
    {
        printf("���ļ�ʧ��\r\n");
    }
    
    /* ���õ���ʼλ�ö�ȡ  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("���ö�ȡλ��ʧ��\r\n");       
    }


    /* д���ַ������ļ�  */
    status =  fx_file_write(&fx_file, FsWriteBuf, strlen(FsWriteBuf));

    if (status == FX_SUCCESS)
    {
        printf("armfly.txt �ļ�д��ɹ�\r\n");        
    }
    else
    {
        printf("armfly.txt �ļ�д��ʧ�� %d\r\n", status); 
    }

    /* �ر��ļ�  */
    status =  fx_file_close(&fx_file);

    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("�ر��ļ�ʧ��\r\n");    
    }

    /* ��֤�ļ�д��ȫ����Ч */
    status = fx_media_flush(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("flushʧ��\r\n");   
    }
    
     /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }   
}

/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: ��ȡ�ļ�armfly.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ReadFileData(void)
{
    UINT status;
    ULONG bw;
	
    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    }
    

    /* ���ļ�  */
    status =  fx_file_open(&sdio_disk, &fx_file, "armfly.txt", FX_OPEN_FOR_READ);

    if (status != FX_SUCCESS)
    {
        printf("���ļ�ʧ��\r\n");
    } 

    /* ���õ���ʼλ�ö�ȡ  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("���ö�ȡλ��ʧ��\r\n");       
    }

    /*  */
    status =  fx_file_read(&fx_file, FsReadBuf, sizeof(FsReadBuf), &bw);

    printf("--%sReadLen = %d\r\n", FsReadBuf, (int)bw);

    if ((status != FX_SUCCESS))
    {
        printf("��ȡʧ��\r\n");     
    }    
    
    /* �ر��ļ�  */
    status =  fx_file_close(&fx_file);

    if (status != FX_SUCCESS)
    {
        printf("�ر��ļ�ʧ��\r\n");    
    }
    
     /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }   
}

/*
*********************************************************************************************************
*	�� �� ��: CreateDir
*	����˵��: ��SD����Ŀ¼����Dir1��Dir2Ŀ¼����Dir1Ŀ¼�´�����Ŀ¼Dir1_1
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateDir(void)
{
    UINT status;
	
    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    } 

    /* ����Ŀ¼/Dir1 */
    status = fx_directory_create(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
    {
        printf("�ļ���Dir1�����ɹ�\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir1 Ŀ¼�Ѿ�����(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir1 ʧ�� (%d)\r\n", status);
		return;
	}  
    
	/* ����Ŀ¼/Dir2 */
    status = fx_directory_create(&sdio_disk, "Dir2");
    
    if (status == FX_SUCCESS)
    {
        printf("�ļ���Dir2�����ɹ�\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir2 Ŀ¼�Ѿ�����(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir2 ʧ�� (%d)\r\n", status);
		return;
	}     

	/* ������Ŀ¼ /Dir1/Dir1_1	   ע�⣺������Ŀ¼Dir1_1ʱ�������ȴ�����Dir1 �� */    
    status = fx_directory_create(&sdio_disk, "Dir1/Dir1_1");
    
    if (status == FX_SUCCESS)
    {
        printf("�ļ���Dir1/Dir1_1�����ɹ�\r\n");
    } 
	else if (status == FX_ALREADY_CREATED)
	{
		printf("Dir1/Dir1_1 Ŀ¼�Ѿ�����(%d)\r\n", status);
	}
	else
	{
		printf("fx_directory_create Dir1/Dir1_1 ʧ�� (%d)\r\n", status);
		return;
	}  
    
    /* ��֤�ļ�д��ȫ����Ч */
    status = fx_media_flush(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("flushʧ��\r\n");   
    }
    
     /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }  
}

/*
*********************************************************************************************************
*	�� �� ��: DeleteDirFile
*	����˵��: ɾ��SD����Ŀ¼�µ� armfly.txt �ļ��� Dir1��Dir2 Ŀ¼
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
    UINT status;
    UINT i;
    char path[50];
    
    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    }

    /* ɾ��Ŀ¼/Dir1 ����Ϊ������Ŀ¼�ǿգ�������Ŀ¼)���������ɾ����ʧ�ܡ�*/
    status = fx_directory_delete(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
	{
		printf("ɾ��Ŀ¼Dir1�ɹ�\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "/Dir1");
	}
	else
	{
		printf("ɾ��Dir1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", status);
	}
    
    
 	/* ��ɾ��Ŀ¼/Dir1/Dir1_1 */  
    status = fx_directory_delete(&sdio_disk, "Dir1/Dir1_1");
    
    if (status == FX_SUCCESS)
	{
		printf("ɾ��Ŀ¼Dir1/Dir1_1�ɹ�\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "Dir1/Dir1_1");
	}
	else
	{
		printf("ɾ��Dir1/Dir1_1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", status);
	}    
    
    /* ɾ��Ŀ¼/Dir1*/  
    status = fx_directory_delete(&sdio_disk, "Dir1");
    
    if (status == FX_SUCCESS)
	{
		printf("ɾ��Ŀ¼Dir1�ɹ�\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "Dir1");
	}
	else
	{
		printf("ɾ��Dir1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", status);
	}   

    /* ɾ��Ŀ¼/Dir2*/  
    status = fx_directory_delete(&sdio_disk, "Dir2");
    
    if (status == FX_SUCCESS)
	{
		printf("ɾ��Ŀ¼Dir2�ɹ�\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "Dir2");
	}
	else
	{
		printf("ɾ��Dir2ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", status);
	}   

	/* ɾ���ļ� armfly.txt */
    status = fx_file_delete(&sdio_disk, "armfly.txt");
    
    if (status == FX_SUCCESS)
	{
		printf("ɾ��Ŀ¼armfly.txt�ɹ�\r\n");
	}
	else if (status == FX_NOT_FOUND)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "armfly.txt");
	}
	else
	{
		printf("ɾ��armfly.txtʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", status);
	}   

	/* ɾ���ļ� speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(path, "Speed%02d.txt", i);/* ÿд1�Σ���ŵ��� */	
        
        status = fx_file_delete(&sdio_disk, path);
    
        if (status == FX_SUCCESS)
        {
            printf("ɾ���ļ�%s�ɹ�\r\n", path);
        }
        else if (status == FX_NOT_FOUND)
        {
            printf("û�з����ļ�:%s\r\n", path);
        }
        else
        {
            printf("ɾ��%s�ļ�ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", path, status);
        }   
	}

     /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }  
}

/*
*********************************************************************************************************
*	�� �� ��: WriteFileTest
*	����˵��: �����ļ���д�ٶ�
*	��    �Σ���
*	�� �� ֵ: ��
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
	
    
    /* ����SD�� */
    status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
    {
        printf("�����ļ�ϵͳʧ�� -- %d\r\n", status);
        return;
    }
    
    /* ���ļ� */
	sprintf(path, "Speed%02d.txt", s_ucTestSn++); /* ÿд1�Σ���ŵ��� */	
    
    /* дһ������ */
	printf("��ʼд�ļ�%s %dKB ...\r\n", path, TEST_FILE_LEN / 1024);
    
    /* �����ļ� */
    status =  fx_file_create(&sdio_disk, path);

    /* ���״̬ */
    if (status != FX_SUCCESS)
    {
        /* ʧ�ܵĻ������Կ��Ƕ��δ��� */
        if (status != FX_ALREADY_CREATED)
        {
            printf("�����ļ�ʧ��\r\n");
        }
    }
    
    /* ���ļ�  */
    status =  fx_file_open(&sdio_disk, &fx_file, path, FX_OPEN_FOR_WRITE);

    if (status != FX_SUCCESS)
    {
        printf("���ļ�ʧ��\r\n");
    }
    
    /* ���õ���ʼλ�ö�ȡ  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("���ö�ȡλ��ʧ��\r\n");       
    }
    
    runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
        /* д���ַ������ļ�  */
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
			printf("%s�ļ�дʧ��\r\n", path);
			break; 
        }
	}
	runtime2 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
	
	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  д��ʱ : %dms   ƽ��д�ٶ� : %dB/S (%dKB/S)\r\n",
                timelen,
                (TEST_FILE_LEN * 1000) / timelen,
                ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}

    /* �ر��ļ�  */
    status =  fx_file_close(&fx_file);

    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("�ر��ļ�ʧ��\r\n");    
    }

    /* ��֤�ļ�д��ȫ����Ч */
    status = fx_media_flush(&sdio_disk);
    
    /* Check the file close status.  */
    if (status != FX_SUCCESS)
    {
        printf("fx_media_flushʧ��\r\n");    
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /* ��ʼ���ļ����� */
	printf("��ʼ���ļ� %dKB ...\r\n", TEST_FILE_LEN / 1024);
    
      /* ���ļ�  */
    status =  fx_file_open(&sdio_disk, &fx_file, path, FX_OPEN_FOR_READ);

    if (status != FX_SUCCESS)
    {
        printf("���ļ�ʧ��\r\n");
    }
    
    /* ���õ���ʼλ�ö�ȡ  */
    status =  fx_file_seek(&fx_file, 0);

    if (status != FX_SUCCESS)
    {
        printf("���ö�ȡλ��ʧ��\r\n");       
    }
    
    runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
        /* д���ַ������ļ�  */
        
        status =  fx_file_read(&fx_file, g_TestBuf, sizeof(g_TestBuf), &bw);
        if (status == FX_SUCCESS)
        {
        	if (((i + 1) % 8) == 0)
			{
				printf(".");
			}

			/* �Ƚ�д��������Ƿ���ȷ�������ᵼ�¶����ٶȽ�����͵� 3.5MBytes/S */
			for (k = 0; k < sizeof(g_TestBuf); k++)
			{
				if (g_TestBuf[k] != (k / 512) + '0')
				{
				  	err = 1;
					printf("Speed1.txt �ļ����ɹ����������ݳ���\r\n");
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
			printf("Speed1.txt �ļ���ʧ��\r\n");
			break; 
        }
	}
	runtime2 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  ����ʱ : %dms   ƽ�����ٶ� : %dB/S (%dKB/S)\r\n", timelen,
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
    
     /* ж��SD�� */
    status =  fx_media_close(&sdio_disk);

    if (status != FX_SUCCESS)
    {
        printf("ж���ļ�ϵͳж��ʧ�� -- %d\r\n", status);
    }  

}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
