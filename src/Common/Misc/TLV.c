//--------------------------Q Controllers---------------------------//
/*
Q-Ctrl是一套基于事件的控制器框架，类比于MCV框架，Q-Ctrl用于协调
存储（Data）、输入输出（IO）、控制器（Controller）三者的逻辑处理，
简称DIC框架。
Q-Ctrl基于stm32有大量的驱动代码可直接调用，也可以移植于其他单片机平台，
无需操作系统的支持，在遵守控制器编程规则的情况下，可处理以往需要操作系统
才能处理的复杂业务。
By Karlno 酷享科技

本文件定义了tlv的操作库，可被开发者用于其他项目，减少代码开发量
*/
//------------------------------------------------------------------//

#include "SysDefines.h"

void TLV_Debug(u8 *pTlv,u16 Len)
{
	u8 Buf[64];
	TLV_DATA *pItem=(void *)Buf;
	u16 Idx=1;
	u8 TlvLen=1;
	
	while(TlvLen)
	{
		TlvLen=TLV_Decode(pTlv,Len,Idx++,pItem);
		if(TlvLen) Debug("[%d]%s:%s\n\r",pItem->Len,gNameSrvValueType[pItem->Type],pItem->Str);
	}
}

//会将值加入到pOut的末尾
//返回整体长度
//错误返回0
u16 TLV_Build(u8 *pOut,u16 BufLen,SRV_VALUE_TYPE Type,u8 *ValueStr)
{
	u16 Len,Sum=0;
	u8 i;
	
	for(i=0;i<SVT_MAX;i++)//不允许添加太多
	{
		if(pOut[0]==SVT_NULL)//空闲位置
		{
			//开始添加
			Len=strlen((void *)ValueStr)+1;
			if(Len>0xfe) return 0; 
			pOut[0]=Type;
			pOut[1]=Len;
			pOut+=2;
			MemCpy(pOut,ValueStr,Len);
			pOut[Len-1]=0;//结束符
			pOut[Len]=SVT_NULL;//给下一个tlv的type赋值，防止下次越界
			return Sum+Len+2;
		}
		else//有值
		{
			Len=pOut[1];
			pOut=&pOut[Len+2];
			Sum+=(Len+2);
			if((Sum+3+strlen((void *)ValueStr))>=BufLen) return 0;//越界检查
		}
	}

	return 0;
}

//解码tlv
//idx从1开始
//解码内容被拷贝到pItem中
//错误返回0，正确返回长度
u8 TLV_Decode(u8 *pIn,u16 BufLen,u16 Idx,TLV_DATA *pItem)
{
	u16 Len,Sum=0;
	u8 i;

	for(i=1;i<=SVT_MAX;i++)//无需解码太多
	{
		if(pIn[0]==SVT_NULL)//空闲位置
		{
			return 0;
		}
		else//有值
		{
			if(i==Idx)//取值
			{
				pItem->Type=pIn[0];
				pItem->Len=pIn[1];
				MemCpy(pItem->Str,&pIn[2],pItem->Len);
				return pItem->Len;
			}
			else//跳过
			{
				Len=pIn[1];
				pIn=&pIn[Len+2];
				Sum+=(Len+2);
				if((Sum+2)>=BufLen) return 0;//越界检查
			}
		}
	}

	return 0;
}


