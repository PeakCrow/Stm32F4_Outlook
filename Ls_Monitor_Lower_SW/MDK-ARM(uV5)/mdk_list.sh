#!/bin/bash
# Author: Osprey
# 本脚本用于生成 MDK 包含的源文件信息 
# 必须使用 MDK 编译后，在工程文件目录下执行本脚本
              
dir_obj=./Out_Obj           # *.d 文件所在目录 
out_file_name=absolute_files.txt     # 输出文件名（绝对路径）
relative_out_file_name=relative_files.txt     # 输出文件名（相对路径）
###当前路径处理

# 斜杠替换为反斜杠  \ 也是特殊字符，需要转义   操作 `` 有问题
curr_dir=`pwd`/
echo ${curr_dir} | sed 's/\//\\/g' > ${out_file_name} && curr_dir=`cat ${out_file_name}` 
curr_dir=${curr_dir#*\\}  # 去除第一个斜杠
# 第一个反斜杠替换    :\ 
echo ${curr_dir} | sed 's/\\/:\\/' > ${out_file_name} && curr_dir=`cat ${out_file_name}`

# 替换 \ 为 \\ 防止 sed 命令出错
echo ${curr_dir} | sed 's/\\/\\\\/g' > ${out_file_name} && curr_dir=`cat ${out_file_name}`

echo "curr dir: ${curr_dir}"

# *.d 文件中包含了源文件信息所以不需要从 *.uvoptx 中提取，下面这些 if 里面的命令没有意义
if false ; then
    dir_uvoptx=./ 
    file_uvoptx=`find ${dir_uvoptx} -name "*.uvoptx"`
    echo "file the name: ${file_uvoptx}"

    sed -i "s/      <PathWithFileName>/"${curr_dir}"/g" ${out_file_name}
    sed -i  's/<\/PathWithFileName>//g' ${out_file_name}
fi

# 首先进入目录
cd ${dir_obj}

temp_file_name=temp_files.txt
find ./ -name "*.d" | xargs cat > head_temp.txt # 合并所有 d 文件

if [[ ! -s head_temp.txt ]] 
then
  echo "file is null, please build the project"  # 文件空白
  exit 0
fi

# 去除重复文件，并排序
sort -u head_temp.txt > head_temp1.txt
# 去除标准头文件  搜索关键 ':\' （包含了标准头文件），分隔符 ':' 去除 *.O 文件名                   'o: ..'   'o: \.\.' 
cat head_temp1.txt | grep -v ':\\' | awk -F ": " '{print $2}' > head_temp.txt 


# 替换 / 为 \ 
sed -i 's/\//\\/g' head_temp.txt


# 添加绝对路径
absolute_files=../${out_file_name}
echo "add absolute dir: ${curr_dir}"  # 双反斜杠是为了防止 sed 命令出错
#行首（^ 代表行首）添加绝对路径字符串
sed "s/^/"${curr_dir}"/g" head_temp.txt > ${absolute_files}  
# 添加相对路径（去除 ..\ ）必须用单引号（根据 SI 工程的情况决定是否去除 ..\）
relative_files=../${relative_out_file_name}
sed 's:\.\.\\::g' head_temp.txt > ${relative_files} 

rm head_temp.txt head_temp1.txt  # 删除文件

cnt=`wc ${absolute_files} | awk '{print $2}'`  # 计文件数

echo "all files is saved in file: ${absolute_files##*/} && ${relative_files##*/}, files number is: ${cnt} "