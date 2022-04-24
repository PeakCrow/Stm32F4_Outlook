/*获取文件名称*/
macro GetFileName(pathName)
{
	nlength = strlen(pathName)
	i = nlength - 1
	name = ""
	while (i + 1)
	{
		ch = pathName[i]
		if ("\\" == "@ch@")
			break
		i = i - 1
	}
	i = i + 1
	while (i < nlength)
	{
		name = cat(name, pathName[i])
		i = i + 1
	}

	return name
}

macro InsertFileHeader1()
{
 	//get crrunt time
 	szTime = GetSysTime(1)
 	Day = szTime.Day
 	Month = szTime.Month
 	Year = szTime.Year
 	hbuf = GetCurrentBuf()//申请变量空间，Get a handle to the current file buffer and the name
 	PathName = GetBufName(hBuf)
 	FileName = GetFileName(PathName)

 	hbuf = GetCurrentBuf()
 	InsBufLine(hbuf, 0, "/*******************************************************************************")
 	InsBufLine(hbuf, 1, "  * \@file        @FileName@")
 	InsBufLine(hbuf, 2, "  * \@version     v1.0.0")
 	InsBufLine(hbuf, 3, "  * \@copyright   COPYRIGHT &copy; @Year@ CSG")
 	InsBufLine(hbuf, 4, "  * \@author      trx")
 	InsBufLine(hbuf, 5, "  * \@date        @Year@-@Month@-@Day@")
 	InsBufLine(hbuf, 6, "  * \@brief")
 	InsBufLine(hbuf, 7, "  * \@attention")
 	InsBufLine(hbuf, 8, "  * Modification History")
 	InsBufLine(hbuf, 9, "  * DATE         DESCRIPTION")
	InsBufLine(hbuf, 10, "  * ------------------------")
 	InsBufLine(hbuf, 11, "  * - @Year@-@Month@-@Day@  trx Created")
 	InsBufLine(hbuf, 12,"*******************************************************************************/")
}

macro InsertFunHeader1()
{
    /* 获取光标所在行 */
    handle = GetCurrentWnd()
    process_line = GetWndSelLnFirst(handle)

    /* 获取函数所在行文本 */
    file_txt = GetCurrentBuf()
    process_txt = GetBufLine(file_txt,process_line + 1)
    process_txt_len = strlen(process_txt)

    if(process_txt == "")
    {
        stop
    }

    /* 获取函数名 */
    symbol_rec = GetSymbolLocationFromLn (handle, process_line + 1)
    if(symbol_rec == "")
    {
        stop
    }

    fuction_name = symbol_rec.Symbol

    /* 获取参数个数及参数名           */
    param_num = 0
    param_list = SymListNew()

    i=0,j=0
    while(i < process_txt_len)
    {
        if(process_txt[i] == "," || process_txt[i] == ")")
        {
            j = i
            while(j > 0)
            {
                if(process_txt[j] == " " || process_txt[j] == "*" || process_txt[j] == "&" ||
                   process_txt[j] == "(")
                {
                    symbol_rec.Symbol = strmid(process_txt,j+1,i)
                    SymListInsert(param_list,param_num,symbol_rec)
                    param_num = param_num + 1
                    break;
                }
                j = j - 1
            }
        }

        i = i + 1
    }

    /* 输出注释 */
    PutBufLine(handle,process_line + 0,"/*******************************************************************************")

    var temp_buffer
    temp_buffer = cat("  * \@FunctionName: ",fuction_name)
    InsBufLine(handle,process_line + 1,temp_buffer)

    InsBufLine(handle,process_line + 2,"  * \@Author:       trx")

    sys_time = GetSysTime(1)
    temp_buffer = "  * \@DateTime:     "
    temp_buffer = cat(temp_buffer,sys_time.date)
    temp_buffer = cat(temp_buffer,"T")
    temp_buffer = cat(temp_buffer,sys_time.time)
    temp_buffer = cat(temp_buffer,"")

    InsBufLine(handle,process_line + 3,temp_buffer)
	InsBufLine(handle,process_line + 4,"  * \@Purpose:      ")
    param_line = process_line + 5
    InsBufLine(handle,param_line,"*******************************************************************************/")

    if(strmid(process_txt,0,4) != "void")
    {
        InsBufLine(handle,param_line,"  * \@return:")
    }

    while(param_num > 0)
    {
        param_num = param_num - 1
        symbol_rec = SymListItem(param_list,param_num)

        temp_buffer = "  * \@param:        "
        temp_buffer = cat(temp_buffer,symbol_rec.Symbol)

        temp_buffer_len = strlen(temp_buffer)
        temp_buffer_len = 38 - temp_buffer_len
        while(temp_buffer_len > 0)
        {
            temp_buffer_len = temp_buffer_len - 1
            temp_buffer = cat(temp_buffer," ")
        }

        //temp_buffer = cat(temp_buffer,"[description]")
        InsBufLine(handle,param_line,temp_buffer)
    }

    SymListFree(param_list)
}


