#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define APP_NAME "Keil5_disp_size_bar V0.2"



void print_msg(const char* tag, const char* format, ...);
FILE* find_file_in_dir_by_name(const char* dir_name, const char* file_name);
int64_t file_sreach_string_get_pos(FILE *fp, char *sub_str, int seek, uint64_t offset);
void prtint_percentage_bar(char *label,uint8_t cnt ,uint32_t val, uint32_t max );
uint64_t str_get_hex(char *str, char *str_hex);
void process_execution_region(FILE *fp_map);

//argv[0] 是当前exe文件的路径
//exe放在工程文件的同一目录下就可以获取到工程目录了
int main(int argc, char *argv[])
{
    DIR *dir;
	char *path = NULL;
    // FILE *fp_project;
	FILE *fp_map;

	print_msg(APP_NAME,"start find map file\n");
	//打开当前目录
    // fp_project = find_file_in_dir_by_name("./",".uvprojx");
    fp_map = find_file_in_dir_by_name("./", ".map");

    process_execution_region(fp_map);


	// fclose(fp_project);  // 关闭文件
	fclose(fp_map);
}



void print_msg(const char* tag, const char* format, ...) {
    printf("[%s]: ", tag);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

FILE* find_file_in_dir_by_name(const char* dir_name, const char* file_name) {
    FILE *fp;
    DIR *dir;
	struct dirent *st;
	struct stat sta;
	int ret = 0;
	char tmp_name[1024] = {0};

		//打开当前目录
	if ((dir = opendir(dir_name)) == NULL)
	{
		print_msg(APP_NAME,"open dir error");

		return NULL;
	}

	while (1)
	{
		st = readdir(dir);
		if (NULL == st) // 读取完毕
		{
			break;
		}

		strcpy(tmp_name, dir->dd_name);
		tmp_name[strlen(dir->dd_name) - 1] = '\0';
		strcat(tmp_name, st->d_name); // 新文件路径名
		ret = stat(tmp_name, &sta);	  // 查看目录下文件属性
		
		if (ret < 0)
		{
			print_msg(APP_NAME,"read stat fail");
		
			return NULL;
		}

		if (S_ISDIR(sta.st_mode)) // 如果为目录文件
		{
			if (0 == strcmp("..", st->d_name) || 0 == strcmp(".", st->d_name)) // 忽略当前目录和上一层目录
				continue;
			else
			{
				fp = find_file_in_dir_by_name(tmp_name,file_name); // 递归读取
				if (fp)
				{

					return fp;
				}
			}
		}
		else // 不为目录则打印文件路径名
		{

			if (strstr(tmp_name,file_name))
			{
				
	
				fp = fopen(tmp_name, "r+");

				if (!fp)
				{
					print_msg(APP_NAME,"Found open_erro:%s\r\n", tmp_name);
					return NULL;
				}
				else
				{
					print_msg(APP_NAME,"Found:%s\r\n", tmp_name);
					return fp;
				}
			}
		}
	}
	closedir(dir);
	return NULL;
}

int64_t file_sreach_string_get_pos(FILE *fp, char *sub_str, int seek, uint64_t offset)
{
	char str[1000];
	int64_t pos = 0;
	fseek(fp, offset, seek);

	while (NULL != fgets(str, 1000, fp))
	{

		if (strstr(str, sub_str))
		{

			// printf("%s", str);
			pos = ftell(fp) - strlen(str);
			fseek(fp, pos, SEEK_SET);
			return pos;
		}
	}
	return -1;
}

void prtint_percentage_bar(char *label,uint8_t cnt, uint32_t val, uint32_t max)
{
	uint8_t i;
	printf("%5dKB %5s%d:|", (uint32_t)(max / 1024.0), label,cnt);

	for (i = 0; i < 20; i++)
	{
		if (i < (uint8_t)((val / 1.0) / (max / 1.0) * 20))
		{
			
			printf("▇");
		}else
		{
			printf("_");
		}
	}
	printf("|(%d B/%d B) %.2f %%\n", val, max, (val / 1.0) / (max / 1.0) * 100);
}

uint64_t str_get_hex(char *str, char *str_hex)
{

	char *str_buf;
	uint64_t hex;

	str_buf = strstr(str, str_hex);
	str_buf += strlen(str_hex);
	sscanf(str_buf, "%x", &hex);

	// printf("%s:%#x\n", str_hex,hex);
	return hex;
}

void process_execution_region(FILE *fp_map) {
    
int64_t pos=0;
char str[1000];
char* ch_p=NULL;
uint64_t exec_base=0, load_base=0, size=0, max=0;
uint8_t ram_cnt=0,flash_cnt=0;

uint8_t cnt=0;
uint64_t ram_size[10];
uint64_t flash_size[10];

uint64_t ram_max[10];
uint64_t flash_max[10];

while(1)
{

	pos=file_sreach_string_get_pos(fp_map, "(Exec", SEEK_SET, pos);
	if(-1== pos )
	{
	print_msg(APP_NAME,"found end\n");
	break;
	}
    

	fseek(fp_map, pos, SEEK_SET);
	fgets(str, 1000, fp_map);

	if(strstr(str,"RAM"))
	{


	ram_size[ram_cnt] =str_get_hex(str,"Size: ");
	pos+=strstr(str,"Size: ")-str;
    fseek(fp_map, pos, SEEK_SET);


    ram_max[ram_cnt] = str_get_hex(str, "Max: ");
	pos+=strstr(str,"Max: ")-str;
    fseek(fp_map, pos, SEEK_SET);	
	if(ram_cnt<10)ram_cnt++;
    // prtint_percentage_bar("ram",ram_cnt, size, max);	

	}else
	{
	
	flash_size[flash_cnt] =str_get_hex(str,"Size: ");
	pos+=strstr(str,"Size: ")-str;
    fseek(fp_map, pos, SEEK_SET);

    
    flash_max[flash_cnt] = str_get_hex(str, "Max: ");
	pos+=strstr(str,"Max: ")-str;
    fseek(fp_map, pos, SEEK_SET);	
	if(flash_cnt<10)flash_cnt++;
    // prtint_percentage_bar("flash",flash_cnt, size, max);

	}

}	

for(cnt=0;cnt<ram_cnt;cnt++)
{
	prtint_percentage_bar("ram",cnt+1, ram_size[cnt], ram_max[cnt]);	
}

for(cnt=0;cnt<flash_cnt;cnt++)
{
	prtint_percentage_bar("flash",cnt+1,flash_size[cnt], flash_max[cnt]);	
}


}
