/*
*********************************************************************************************************
*
*	ģ������ : SD��Fat�ļ�ϵͳ��SD��ģ����ʾģ�顣
*	�ļ����� : demo_sd_fatfs.c
*	��    �� : V1.0
*	˵    �� : ��������ֲFatFS�ļ�ϵͳ���汾 R0.12c������ʾ��δ����ļ�����ȡ�ļ�������Ŀ¼��ɾ���ļ�
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
*		V1.0    2020-10-06   Eric2013    ��ʽ����
*
*	Copyright (C), 2020-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "sys.h"
#include "ff.h"				/* FatFS�ļ�ϵͳģ��*/
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"


/* ���ڲ��Զ�д�ٶ� */
#define TEST_FILE_LEN			(2*1024*1024)	/* ���ڲ��Ե��ļ����� */
#define BUF_SIZE				(4*1024)		/* ÿ�ζ�дSD����������ݳ��� */

/* �������ļ��ڵ��õĺ������� */
static void DispMenu(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void CreateDir(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);

FATFS fs;
FIL file;
char FsReadBuf[1024];
char FsWriteBuf[1024] = {"FatFS Write Demo \r\n www.armfly.com \r\n"};
uint8_t g_TestBuf[BUF_SIZE];

DIR DirInf;
FILINFO FileInf;
char DiskPath[4]; /* SD���߼�����·�������̷�0������"0:/" */

/* FatFs API�ķ���ֵ */
static const char * FR_Table[]= 
{
	"FR_OK���ɹ�!",				                             /* (0) Succeeded */
	"FR_DISK_ERR���ײ�Ӳ������!",			                 /* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR������ʧ��!",				                     /* (2) Assertion failed */
	"FR_NOT_READY����������û�й���!",			             /* (3) The physical drive cannot work */
	"FR_NO_FILE���ļ�������!",				                 /* (4) Could not find the file */
	"FR_NO_PATH��·��������!",				                 /* (5) Could not find the path */
	"FR_INVALID_NAME����Ч�ļ���!",		                     /* (6) The path name format is invalid */
	"FR_DENIED�����ڽ�ֹ���ʻ���Ŀ¼�������ʱ��ܾ�!",         /* (7) Access denied due to prohibited access or directory full */
	"FR_EXIST���ļ��Ѿ�����!",			                     /* (8) Access denied due to prohibited access */
	"FR_INVALID_OBJECT���ļ�����Ŀ¼������Ч!",		         /* (9) The file/directory object is invalid */
	"FR_WRITE_PROTECTED������������д����!",		             /* (10) The physical drive is write protected */
	"FR_INVALID_DRIVE���߼���������Ч!",		                 /* (11) The logical drive number is invalid */
	"FR_NOT_ENABLED�������޹�����!",			                 /* (12) The volume has no work area */
	"FR_NO_FILESYSTEM��û����Ч��FAT��!",		             /* (13) There is no valid FAT volume */
	"FR_MKFS_ABORTED�����ڲ�������f_mkfs()����ֹ!",	         /* (14) The f_mkfs() aborted due to any parameter error */
	"FR_TIMEOUT���ڹ涨��ʱ�����޷���÷��ʾ�����!",		 /* (15) Could not get a grant to access the volume within defined period */
	"FR_LOCKED�������ļ�������Բ������ܾ�!",				 /* (16) The operation is rejected according to the file sharing policy */
	"FR_NOT_ENOUGH_CORE���޷����䳤�ļ���������!",		     /* (17) LFN working buffer could not be allocated */
	"FR_TOO_MANY_OPEN_FILES����ǰ�򿪵��ļ�������_FS_SHARE!", /* (18) Number of open files > _FS_SHARE */
	"FR_INVALID_PARAMETER��������Ч!"	                     /* (19) Given parameter is invalid */
};

/*
*********************************************************************************************************
*	�� �� ��: DemoFatFS
*	����˵��: FatFS�ļ�ϵͳ��ʾ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoFatFS(void)
{
	uint8_t cmd;

	/* ��ӡ�����б��û�����ͨ�����ڲ���ָ�� */
	DispMenu();
	
	/* ע��SD������ */
	FATFS_LinkDriver(&SD_Driver, DiskPath);
    
//	bsp_StartAutoTimer(0, 500);	/* ����1��500ms���Զ���װ�Ķ�ʱ�� */
	
	while (1)
	{
//        /* �ж϶�ʱ����ʱʱ�� */
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
static void ViewRootDir(void)
{
	FRESULT result;
	uint32_t cnt = 0;
	FILINFO fno;
	
	
 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);	/* Mount a logical drive */
	printf("SD�̷���%s\r\n",DiskPath);
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* �򿪸��ļ��� */
	result = f_opendir(&DirInf, DiskPath); /* ���������������ӵ�ǰĿ¼��ʼ */
	printf("SD�̷���%s\r\n",DiskPath);
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
		return;
	}

	printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf, &FileInf); 		/* ��ȡĿ¼��������Զ����� */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* �ж����ļ�������Ŀ¼ */
		if (FileInf.fattrib & AM_DIR)
		{
			printf("(0x%02d)Ŀ¼  ", FileInf.fattrib);
		}
		else
		{
			printf("(0x%02d)�ļ�  ", FileInf.fattrib);
		}

		f_stat(FileInf.fname, &fno);
		
		/* ��ӡ�ļ���С, ���4G */
		printf(" %10d", (int)fno.fsize);


		printf("  %s\r\n", (char *)FileInf.fname);	/* ���ļ��� */
	}  
  
	/* ж���ļ�ϵͳ */
	 f_mount(NULL, DiskPath, 0);
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
	FRESULT result;
	uint32_t bw;
	char path[32];


 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* ���ļ� */
	sprintf(path, "%sarmfly.txt", DiskPath);
	result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == FR_OK)
	{
		printf("armfly.txt �ļ��򿪳ɹ�\r\n");
	}
	else
	{
		printf("armfly.txt �ļ���ʧ��  (%s)\r\n", FR_Table[result]);
	}

	/* дһ������ */
	result = f_write(&file, FsWriteBuf, strlen(FsWriteBuf), &bw);
	if (result == FR_OK)
	{
		printf("armfly.txt �ļ�д��ɹ�\r\n");
	}
	else
	{
		printf("armfly.txt �ļ�д��ʧ��  (%s)\r\n", FR_Table[result]);
	}

	/* �ر��ļ�*/
	f_close(&file);

	/* ж���ļ�ϵͳ */
	f_mount(NULL, DiskPath, 0);
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
	FRESULT result;
	uint32_t bw;
	char path[64];

	
 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* ���ļ� */
	sprintf(path, "%sarmfly.txt", DiskPath);
	result = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : armfly.txt\r\n");
		return;
	}

	/* ��ȡ�ļ� */
	result = f_read(&file, FsReadBuf, sizeof(FsReadBuf), &bw);
	if (bw > 0)
	{
		FsReadBuf[bw] = 0;
		printf("\r\narmfly.txt �ļ����� : \r\n%s\r\n", FsReadBuf);
	}
	else
	{
		printf("\r\narmfly.txt �ļ����� : \r\n");
	}

	/* �ر��ļ�*/
	f_close(&file);

	/* ж���ļ�ϵͳ */
	f_mount(NULL, DiskPath, 0);
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
	FRESULT result;
	char path[64]; 

	
 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* ����Ŀ¼/Dir1 */
	sprintf(path, "%sDir1", DiskPath);
	result = f_mkdir(path);
	if (result == FR_OK)
	{
		printf("f_mkdir Dir1 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1 Ŀ¼�Ѿ�����(%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir1 ʧ�� (%s)\r\n", FR_Table[result]);
		return;
		
	}

	/* ����Ŀ¼/Dir2 */
	sprintf(path, "%sDir2", DiskPath);
	result = f_mkdir(path);
	if (result == FR_OK)
	{
		printf("f_mkdir Dir2 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir2 Ŀ¼�Ѿ�����(%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir2 ʧ�� (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ������Ŀ¼ /Dir1/Dir1_1	   ע�⣺������Ŀ¼Dir1_1ʱ�������ȴ�����Dir1 */
	sprintf(path, "%sDir1/Dir1_1", DiskPath);
	result = f_mkdir(path); /* */
	if (result == FR_OK)
	{
		printf("f_mkdir Dir1_1 �ɹ�\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1_1 Ŀ¼�Ѿ����� (%d)\r\n", result);
	}
	else
	{
		printf("f_mkdir Dir1_1 ʧ�� (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ж���ļ�ϵͳ */
	f_mount(NULL, DiskPath, 0);
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
	FRESULT result;
	uint8_t i;
	char path[64]; 
	
 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}
	
	/* ɾ��Ŀ¼/Dir1 ����Ϊ������Ŀ¼�ǿգ�������Ŀ¼)���������ɾ����ʧ�ܡ�*/
	sprintf(path, "%sDir1", DiskPath);
	result = f_unlink(path);
	if (result == FR_OK)
	{
		printf("ɾ��Ŀ¼Dir1�ɹ�\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "/Dir1");
	}
	else
	{
		printf("ɾ��Dir1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
	}

	/* ��ɾ��Ŀ¼/Dir1/Dir1_1 */
	sprintf(path, "%sDir1/Dir1_1", DiskPath);
	result = f_unlink(path);
	if (result == FR_OK)
	{
		printf("ɾ����Ŀ¼/Dir1/Dir1_1�ɹ�\r\n");
	}
	else if ((result == FR_NO_FILE) || (result == FR_NO_PATH))
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "/Dir1/Dir1_1");
	}
	else
	{
		printf("ɾ����Ŀ¼/Dir1/Dir1_1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
	}

	/* ��ɾ��Ŀ¼/Dir1 */
	sprintf(path, "%sDir1", DiskPath);
	result = f_unlink(path);
	if (result == FR_OK)
	{
		printf("ɾ��Ŀ¼Dir1�ɹ�\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "/Dir1");
	}
	else
	{
		printf("ɾ��Dir1ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
	}

	/* ɾ��Ŀ¼/Dir2 */
	sprintf(path, "%sDir2", DiskPath);
	result = f_unlink(path);
	if (result == FR_OK)
	{
		printf("ɾ��Ŀ¼ Dir2 �ɹ�\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "/Dir2");
	}
	else
	{
		printf("ɾ��Dir2 ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
	}

	/* ɾ���ļ� armfly.txt */
	sprintf(path, "%sarmfly.txt", DiskPath);
	result = f_unlink(path);
	if (result == FR_OK)
	{
		printf("ɾ���ļ� armfly.txt �ɹ�\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("û�з����ļ���Ŀ¼ :%s\r\n", "armfly.txt");
	}
	else
	{
		printf("ɾ��armfly.txtʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", result);
	}

	/* ɾ���ļ� speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(path, "%sSpeed%02d.txt", DiskPath, i);/* ÿд1�Σ���ŵ��� */	
		result = f_unlink(path);
		if (result == FR_OK)
		{
			printf("ɾ���ļ�%s�ɹ�\r\n", path);
		}
		else if (result == FR_NO_FILE)
		{
			printf("û�з����ļ�:%s\r\n", path);
		}
		else
		{
			printf("ɾ��%s�ļ�ʧ��(������� = %d) �ļ�ֻ����Ŀ¼�ǿ�\r\n", path, result);
		}
	}

	/* ж���ļ�ϵͳ */
	f_mount(NULL, DiskPath, 0);
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
	FRESULT result;
	char path[64]; 
	uint32_t bw;
	uint32_t i,k;
	uint32_t runtime1,runtime2,timelen;
	uint8_t err = 0;
	static uint8_t s_ucTestSn = 0;

	
	for (i = 0; i < sizeof(g_TestBuf); i++)
	{
		g_TestBuf[i] = (i / 512) + '0';
	}

  	/* �����ļ�ϵͳ */
	result = f_mount(&fs, DiskPath, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* ���ļ� */
	sprintf(path, "%sSpeed%02d.txt", DiskPath, s_ucTestSn++); /* ÿд1�Σ���ŵ��� */	
	result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);

	/* дһ������ */
	printf("��ʼд�ļ�%s %dKB ...\r\n", path, TEST_FILE_LEN / 1024);
	
	runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_write(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
		if (result == FR_OK)
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

	f_close(&file);		/* �ر��ļ�*/


	/* ��ʼ���ļ����� */
	result = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("û���ҵ��ļ�: %s\r\n", path);
		return;
	}

	printf("��ʼ���ļ� %dKB ...\r\n", TEST_FILE_LEN / 1024);
	
	runtime1 = bsp_GetRunTime();	/* ��ȡϵͳ����ʱ�� */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_read(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
		if (result == FR_OK)
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

	/* �ر��ļ�*/
	f_close(&file);

	/* ж���ļ�ϵͳ */
	f_mount(NULL, DiskPath, 0);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
