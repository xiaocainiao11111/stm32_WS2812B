/**********************************************************************************
 * 程序名:		OLED显示程序-正式版
 * 编写人:		HGD
 * 创建时间:	2019.07.28
 * 当前版本:	V1.2
 
 * 说明:
	** 本程序支持0.96寸的I2C和SPI接口OLED显示屏
	** OLED.c程序架构：
		|----------------
		|- OLED类型选择
		|- I2C引脚配置（选择OLED_TYPE_I2C时参与编译）
		|- SPI引脚配置（选择OLED_TYPE_SPI时参与编译）
		|- I2C底层代码（选择OLED_TYPE_I2C时参与编译）
		|- SPI底层代码（选择OLED_TYPE_SPI时参与编译）
		|- 应用层代码
		|----------------
	** 使用方法：
		1.将OLED.c,OLED.h,OLEDFont.h,OLEDImage.h,OLEDChinese.h复制到工程目录，并将OLED.c加入工程
		2.选择OLED类型，即OLED_TYPE_I2C或OLED_TYPE_SPI
		3.修改I2C引脚配置或SPI引脚配置部分的代码，包括引脚操作方式和初始化代码
		4.在main.c顶部添加 #include "OLED.h"，如需显示图片，还需要添加 #include "OLEDImage.h"，如需显示汉字，还需要添加 #include "OLEDChinese.h"
		5.调用OLED.h中所包含的应用层函数
		6.实现类似printf功能的方法：
			sprintf(OLED_String,"Num:%d",123);
			OLED_ShowString(0,0,OLED_String,NORMAL);
			其中OLED_String已在本文件声明，在使用时直接调用即可
		
 * 日志:
	** 2019.07.28 V1.0 创建程序
	** 2019.08.04 V1.1 增加显示汉字的函数，将CharType改为了DisplayType，OLED_ShowImage()增加了一个参数
	** 2019.08.13 V1.2 增加了对格式化字符串的支持，增加了开启GPIO时钟的代码
	
**********************************************************************************/

#include "OLED.h"
#include "OLEDFont.h"
#include "stm32f10x.h"

char OLED_String[25];

/*OLED类型，范围：OLED_TYPE_I2C，OLED_TYPE_SPI*/
#define OLED_TYPE_I2C

/*以下部分与硬件有关，移植代码需在以下对应位置修改引脚操作方式和初始化代码*/
/*只需修改所选接口的相关代码，未选择的代码不参与编译，无需修改*/
#ifdef OLED_TYPE_I2C	//I2C引脚配置

#define OLED_SCL_SET	GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define OLED_SCL_RESET	GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define OLED_SDA_SET	GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define OLED_SDA_RESET	GPIO_ResetBits(GPIOB,GPIO_Pin_9)

void I2C_GPIOInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#endif

#ifdef OLED_TYPE_SPI	//SPI引脚配置

#define OLED_D0_SET		GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define OLED_D0_RESET	GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define OLED_D1_SET		GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define OLED_D1_RESET	GPIO_ResetBits(GPIOA,GPIO_Pin_3)
#define OLED_RES_SET	GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define OLED_RES_RESET	GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define OLED_DC_SET		GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define OLED_DC_RESET	GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define OLED_CS_SET		GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define OLED_CS_RESET	GPIO_ResetBits(GPIOA,GPIO_Pin_6)

void SPI_GPIOInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

#endif
/*以上部分与硬件有关，移植程序修改以上代码即可*/

/*底层代码*/
#ifdef OLED_TYPE_I2C		//I2C底层代码

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void I2C_Start(void)
{
	OLED_SCL_RESET;
	OLED_SDA_SET;
	OLED_SCL_SET;
	OLED_SDA_RESET;
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void I2C_Stop(void)
{
	OLED_SDA_RESET;
	OLED_SCL_SET;
	OLED_SDA_SET;
	OLED_SCL_RESET;
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OLED_SCL_RESET;
		if(Byte&(0x80>>i))
		{
			OLED_SDA_SET;
		}
		else
		{
			OLED_SDA_RESET;
		}
		OLED_SCL_SET;
	}
	OLED_SCL_RESET;	//额外的一个时钟，不处理应答信号
	OLED_SCL_SET;
}

/**
  * @brief  OLED通用IO口初始化
  * @param  无
  * @retval 无
  */
void OLED_GPIOInit(void)
{
	I2C_GPIOInit();
	OLED_SDA_SET;
	OLED_SCL_SET;
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	I2C_Start();
	I2C_SendByte(0x78);		//从机地址
	I2C_SendByte(0x00);		//写命令
	I2C_SendByte(Command); 
	I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	I2C_Start();
	I2C_SendByte(0x78);		//从机地址
	I2C_SendByte(0x40);		//写数据
	I2C_SendByte(Data);
	I2C_Stop();
}

#endif

#ifdef OLED_TYPE_SPI		//SPI底层代码

/**
  * @brief  SPI发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void SPI_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OLED_D0_RESET;
		if(Byte&(0x80>>i))
		{
			OLED_D1_SET;
		}
		else
		{
			OLED_D1_RESET;
		}
		OLED_D0_SET;
	}
}

/**
  * @brief  OLED通用IO口初始化
  * @param  无
  * @retval 无
  */
void OLED_GPIOInit(void)
{
	SPI_GPIOInit();
	OLED_D0_SET;
	OLED_D1_SET;
	OLED_RES_SET;
	OLED_DC_SET;
	OLED_CS_SET;
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_CS_RESET;
	OLED_DC_RESET;
	SPI_SendByte(Command);
	OLED_CS_SET;
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_CS_RESET;
	OLED_DC_SET;
	SPI_SendByte(Data);
	OLED_CS_SET;
}

#endif

/*应用层代码*/
/**
  * @brief  OLED设置光标指针位置（内部调用）
  * @param  X 位置的横坐标，范围：0~127
  * @param  Y 位置的纵坐标，范围：0~7
  * @retval 无
  */
void OLED_SetPointer(uint8_t X, uint8_t Y)
{
	OLED_WriteCommand(0xb0+Y);				//设置页地址（0~7）
	OLED_WriteCommand(((X&0xf0)>>4)|0x10);	//设置显示位置—列低地址
	OLED_WriteCommand((X&0x0f));			//设置显示位置—列高地址
}

/**
  * @brief  OLED部分清屏
  * @param  StartX 起始位置的横坐标，范围：0~127
  * @param  StartY 起始位置的纵坐标，范围：0~7
  * @param  EndX 结束位置的横坐标，范围：1~128
  * @param  EndY 结束位置的纵坐标，范围：1~8
  * @retval 无
  */
void OLED_AreaClear(uint8_t StartX,uint8_t StartY,uint8_t EndX,uint8_t EndY)
{
	uint8_t DistanceX=EndX-StartX,DistanceY=EndY-StartY;
	uint8_t i,j;
	for(j=0;j<DistanceY;j++)
	{
		OLED_SetPointer(StartX,StartY+j);
		for(i=0;i<DistanceX;i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED全屏清屏
  * @param  无
  * @retval 无
  */
void OLED_AllClear(void)
{  
	OLED_AreaClear(0,0,128,8);
}

/**
  * @brief  OLED显示一个字符
  * @param  X 起始位置的横坐标，范围：0~127
  * @param  Y 起始位置的纵坐标，范围：0~7
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @param  DisplayType 显示类型，
			范围：
				SMALL			小号字体
				NORMAL			正常号字体
				SMALL|REVERSE	小号字体反白显示
				NORMAL|REVERSE	正常号字体反白显示
  * @retval 无
  */
void OLED_ShowChar(uint8_t X,uint8_t Y,int8_t Char,uint8_t DisplayType)
{      	
	uint8_t OffsetChar,i;
	OffsetChar=Char-' ';
	if(DisplayType&NORMAL)		//正常号字体，高16个像素，宽8个像素
	{
		OLED_SetPointer(X,Y);
		for(i=0;i<8;i++)
		{
			if(DisplayType&REVERSE)
			{
				OLED_WriteData(~F8x16[OffsetChar][i]);
			}
			else
			{
				OLED_WriteData(F8x16[OffsetChar][i]);
			}
		}
		OLED_SetPointer(X,Y+1);
		for(i=0;i<8;i++)
		{
			if(DisplayType&REVERSE)
			{
				OLED_WriteData(~F8x16[OffsetChar][i+8]);
			}
			else
			{
				OLED_WriteData(F8x16[OffsetChar][i+8]);
			}
		}
	}
	else if(DisplayType&SMALL)		//小号字体，高8个像素，宽6个像素
	{
		OLED_SetPointer(X,Y);
		for(i=0;i<6;i++)
		{
			if(DisplayType&REVERSE)
			{
				OLED_WriteData(~F6x8[OffsetChar][i]);
			}
			else
			{
				OLED_WriteData(F6x8[OffsetChar][i]);
			}
		}
	}
}

/**
  * @brief  OLED显示字符串
  * @param  X 起始位置的横坐标，范围：0~127
  * @param  Y 起始位置的纵坐标，范围：0~7
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @param  DisplayType 显示类型，
			范围：
				SMALL			小号字体
				NORMAL			正常号字体
				SMALL|REVERSE	小号字体反白显示
				NORMAL|REVERSE	正常号字体反白显示
  * @retval 无
  */
void OLED_ShowString(uint8_t X,uint8_t Y,char *String,uint8_t DisplayType)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		OLED_ShowChar(X+(DisplayType&0x0F)*i,Y,String[i],DisplayType);
	}
}

/**
  * @brief  OLED次方函数（内部调用）
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result*=X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字
  * @param  X 起始位置的横坐标，范围：0~127
  * @param  Y 起始位置的纵坐标，范围：0~7
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  NumType 要显示数字的长度及方式，
			范围：
				1~10，SIGNED，HEX
				例如：
				3			以3位无符号形式显示
				4|SIGNED	以4位有符号形式显示
				2|HEX		以2位无符号十六进制形式显示
				不支持有符号十六进制显示
  * @param  DisplayType 显示类型，
			范围：
				SMALL			小号字体
				NORMAL			正常号字体
				SMALL|REVERSE	小号字体反白显示
				NORMAL|REVERSE	正常号字体反白显示
  * @retval 无
  */
void OLED_ShowNumber(uint8_t X,uint8_t Y,int32_t Number,uint8_t NumType,uint8_t DisplayType)
{
	uint8_t i,SingleHexNumber;
	if(NumType&HEX)			//十六进制形式显示
	{
		OLED_ShowChar(X,Y,'0',DisplayType);
		OLED_ShowChar(X+(DisplayType&0x0F),Y,'x',DisplayType);
		for(i=NumType&0x0F;i>0;i--)
		{
			SingleHexNumber=Number/OLED_Pow(16,i-1)%16;
			if(SingleHexNumber<10)
			{
				OLED_ShowChar(X+(DisplayType&0x0F)*((NumType&0x0F)-i+2),Y,SingleHexNumber+'0',DisplayType);
			}
			else
			{
				OLED_ShowChar(X+(DisplayType&0x0F)*((NumType&0x0F)-i+2),Y,SingleHexNumber-10+'A',DisplayType);
			}
		}
	}
	else if(NumType&SIGNED)	//有符号十进制形式显示
	{
		if(Number<0)
		{
			Number=-Number;
			OLED_ShowChar(X,Y,'-',DisplayType);
		}
		else
		{
			OLED_ShowChar(X,Y,'+',DisplayType);
		}
		for(i=NumType&0x0F;i>0;i--)
		{
			OLED_ShowChar(X+(DisplayType&0x0F)*((NumType&0x0F)-i+1),Y,Number/OLED_Pow(10,i-1)%10+'0',DisplayType);
		}
	}
	else	//无符号十进制形式显示
	{
		for(i=NumType&0x0F;i>0;i--)
		{
			OLED_ShowChar(X+(DisplayType&0x0F)*((NumType&0x0F)-i),Y,Number/OLED_Pow(10,i-1)%10+'0',DisplayType);
		}
	}
}

/**
  * @brief  OLED显示汉字串
  * @param  X 起始位置的横坐标，范围：0~127
  * @param  Y 起始位置的纵坐标，范围：0~7
  * @param  Chinese[] 要显示的汉字串
			Chinese[0]标明汉字串的个数，范围：1~8
			汉字仅支持16*16点
  * @param  DisplayType 显示类型，
			范围：
				NORMAL		正常显示
				REVERSE		反色显示
  * @retval 无
  */
void OLED_ShowChinese(uint8_t X,uint8_t Y,uint8_t *Chinese,uint8_t DisplayType)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		OLED_SetPointer(X,j+Y);
		for(i=0;i<16*Chinese[0];i++)
		{
			if(DisplayType==NORMAL)
			{
				OLED_WriteData(Chinese[j*16+i%16+i/16*32+1]);
			}
			else if(DisplayType==REVERSE)
			{
				OLED_WriteData(~Chinese[j*16+i%16+i/16*32+1]);
			}
		}
	}
}

/**
  * @brief  OLED显示图片
  * @param  X 起始位置的横坐标，范围：0~127
  * @param  Y 起始位置的纵坐标，范围：0~7
  * @param  Image[] 要显示的图片
			Image[0]标明图片X方向长度，范围：0~128
			Image[1]标明图片Y方向长度，范围：0~64
			超出屏幕边界的部分将被裁剪
			若Y方向长度不为8的倍数，多出的部分将被裁剪
  * @param  DisplayType 显示类型，
			范围：
				NORMAL		正常显示
				REVERSE		反色显示
  * @retval 无
  */
void OLED_ShowImage(uint8_t X,uint8_t Y,uint8_t *Image,uint8_t DisplayType)
{
	uint8_t i,j;
	for(j=0;j<Image[1]/8;j++)
	{
		if(j+Y>=8)break;
		OLED_SetPointer(X,j+Y);
		for(i=0;i<Image[0];i++)
		{
			if(i+X>=128)break;
			if(DisplayType==NORMAL)
			{
				OLED_WriteData(Image[j*Image[0]+i+2]);
			}
			else if(DisplayType==REVERSE)
			{
				OLED_WriteData(~Image[j*Image[0]+i+2]);
			}
		}
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i,j;
	for(i=0;i<1000;i++)			//上电延时
	{
		for(j=0;j<1000;j++);
	}
	
	OLED_GPIOInit();			//通用IO口初始化
	
	OLED_WriteCommand(0xAE);	//关闭OLED面板
	OLED_WriteCommand(0x00);	//设置低列地址
	OLED_WriteCommand(0x10);	//设置高列地址
	OLED_WriteCommand(0x40);	//设置起始行地址
	OLED_WriteCommand(0x81);	//设置对比度控制寄存器
	OLED_WriteCommand(0xCF);	//设置输出电流亮度
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0 左右反置
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0 上下反置
	OLED_WriteCommand(0xA6);	//设置正常显示
	OLED_WriteCommand(0xA8);	//设置多路复用率(1 to 64)
	OLED_WriteCommand(0x3f);	//1/64占空比
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);	//不设置偏移
	OLED_WriteCommand(0xd5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);	//设置分频比，设置时钟为100帧/秒
	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);	//设定预充时间为15个时钟，放电时间为1个时钟
	OLED_WriteCommand(0xDA);	//设置硬件配置
	OLED_WriteCommand(0x12);
	OLED_WriteCommand(0xDB);	//设置vcomh
	OLED_WriteCommand(0x40);	//设置VCOM取消选择级别
	OLED_WriteCommand(0x20);	//设置页面寻址模式(0x00/0x01/0x02)
	OLED_WriteCommand(0x02);
	OLED_WriteCommand(0x8D);	//设置充电泵启用/禁用
	OLED_WriteCommand(0x14);	//设置(0x10)失能
	OLED_WriteCommand(0xA4);	//禁用整个显示(0xA4/0xA5)
	OLED_WriteCommand(0xA6);	//禁用逆显示(0xA6/A7) 
	OLED_WriteCommand(0xAF);	//打开OLED面板
	OLED_WriteCommand(0xAF);	//开启显示
	
	OLED_AllClear();			//OLED全屏清屏
}
