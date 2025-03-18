#include <stm32f10x.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACKAGE_HEAD_LEN 4
#define PARITY_LEN 2

typedef unsigned char quint8;
typedef unsigned short quint16;
typedef unsigned int quint32;

typedef struct package
{
    quint8 deviceAddr;
    quint8 function;
    quint16 dataLength;
    quint8 data[];
} Package;


// ??????
Package *pg = NULL;
Package *currentPg = NULL;

// ???????????????????
int currentDataLen = 0;
// ????????
int totalPackageLen = 0;
int flagParse = 0;


const quint16 CRC16Table[256] = {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

//??crc16????
quint16 dataVerificationCRC16(const quint8 *data, int len)
{
  int i;
  quint16 result =0;

  for (i = 0; i < len; i++)
  {
       result = (result << 8) ^ CRC16Table[(result >> 8) ^ data[i]];
  }

  return result;
}


void user_GPIO_init()
{
	GPIO_InitTypeDef gpio_initStruct;
	
	// ??gpio????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// ??????
	gpio_initStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_5;
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	// ?????
	GPIO_Init(GPIOB, &gpio_initStruct);
}

void delay_ms(uint16_t ms)
{
	// 72 000 000  1000
	while (ms--)
	{
		SysTick->CTRL = 0;
		SysTick->LOAD = 72000 * 1 - 1;
		SysTick->VAL = 0;
		SysTick->CTRL = 5;
		while ((SysTick->CTRL & 0x00010000) == 0);
		SysTick->CTRL = 0;
	}
}

void beep_on(int time)
{
	for(int i = 0; i < time; i++)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		delay_ms(1);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		delay_ms(1);
	}
}


void user_UART_init()
{
	GPIO_InitTypeDef gpio_initStruct;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	 /* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	gpio_initStruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio_initStruct);
	
	gpio_initStruct.GPIO_Pin = GPIO_Pin_9;
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio_initStruct);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	 
	 
	 NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	 
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	 
	 //USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	 
	 USART_Cmd(USART1, ENABLE);
}


void send_byte(uint8_t ch)
{
	USART_SendData(USART1, ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void send_buf(uint8_t *buf, quint16 len)
{
	uint16_t i = 0;
	for (i = 0; i < len; i++)
	{
		send_byte(buf[i]);
	}
}


static void user_usart_IThandler1(void)
{
	//rxd_buf[rxd_index++] = USART_ReceiveData(USART1);
	
		Package *tmpPg = NULL;
	// ???????????????
    // QByteArray buf = serialPort->readAll();
		uint8_t buf[1] = {0};
		buf[0] = USART_ReceiveData(USART1);
		
		// ????????????
    tmpPg = pg;
    
    // ??????????????
    pg = (Package *)malloc(currentDataLen + sizeof(buf));
    // ???????????????????????????
    memcpy(pg, (quint8 *)tmpPg, currentDataLen);
    // ???????????????
    if (tmpPg != NULL)
    {
        free(tmpPg);
        tmpPg = NULL;
    }
    // ????????????????????????????
    memcpy((quint8 *)pg + currentDataLen, buf, sizeof(buf));
    // ???????????????????????
    currentDataLen += sizeof(buf);
    // ???????????????????? ? ??????????????
    if ((currentDataLen >= PACKAGE_HEAD_LEN) && (totalPackageLen == 0))
    {
        // ???????????
        totalPackageLen = PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN;
    }
    // ????????????????????????
    if ((currentDataLen >= totalPackageLen) && (totalPackageLen != 0))
    {
			  Package *newTmpPg = NULL;
        // ?????????????
        //Package *currentPg = (Package *)malloc(totalPackageLen);
				currentPg = (Package *)malloc(totalPackageLen);
        // ?????????????
        memcpy(currentPg, pg, totalPackageLen);
        // ?????????????????
        currentDataLen -= totalPackageLen;
        // ??????????????
        newTmpPg = pg;
        // ??????????
        pg = (Package *)malloc(currentDataLen);
        // ??????????????
        memcpy(pg, (quint8 *)newTmpPg + totalPackageLen, currentDataLen);
        // ??????????????????
        if (newTmpPg != NULL)
        {
            free(newTmpPg);
            newTmpPg = NULL;
        }
        // ???????????
        //serialPort->write((char *)currentPg, totalPackageLen);
				//send_buf((uint8_t *)currentPg, totalPackageLen);
        // parsePackage(currentPg);
				flagParse = 1;

        totalPackageLen = 0;
    }
}


void USART1_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET)
	{
		user_usart_IThandler1();
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	
	if (USART_GetFlagStatus(USART1, USART_IT_IDLE) != RESET)
	{
		int clear;
		

		
//		rxd_index = 0;
//		
//		rxd_flag = 1;
		
		clear = USART1->SR;
		clear = USART1->DR;
		
		
		
	}
}
	

 int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	return ch;
}

void sendPackage(Package *pg)
{
    quint16 local_crc = dataVerificationCRC16((quint8 *)pg, PACKAGE_HEAD_LEN + pg->dataLength);
    pg->data[pg->dataLength] = local_crc;
    pg->data[pg->dataLength + 1] = local_crc >> 8;

    //serialPort->write((char *)pg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
		//send_buf(pg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
		send_buf((uint8_t *)pg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);

    if (pg != NULL)
    {
        free(pg);
        pg = NULL;
    }
}
int engine_ori_value[6] = {1500, 1500, 1500, 1500, 1500, 1500};

extern void tim2_pwm_init(void);
extern void tim3_pwm_init(void);
extern void tim4_pwm_init(void);

void ctrl_engine(int engine_num, int engine_value, int speed)
{
	int n = 0;
	int count = 0;
	int ori = engine_ori_value[engine_num - 1];
	count = (engine_ori_value[engine_num - 1] <= engine_value) ? (engine_value - engine_ori_value[engine_num - 1]) : (engine_ori_value[engine_num - 1] - engine_value);
	count = count / 20;

	
	switch (engine_num)
	{
		case 1:
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare2(TIM2, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
		case 2:
			//??     ???,???
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare3(TIM4, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
		case 3:
			//??     ???,???
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare4(TIM4, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
		case 6:
			//??     ???,???
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare1(TIM3, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
		case 5:
			//??     ???,???
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare2(TIM4, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
		case 4:
			//??     ???,???
			for (n = 0; n < count; n++)
			{		
				//??     ???,???
				TIM_SetCompare1(TIM4, engine_ori_value[engine_num - 1] += (engine_value - ori) / count);
				delay_ms(1000 - speed * 100);
			}
			break;
	}
}

void parsePackage(Package *pg)
{   
		//delay_ms(2000);
    quint16 localCRC = dataVerificationCRC16((quint8 *)pg, PACKAGE_HEAD_LEN + pg->dataLength);
    quint16 recvCRC = 0;
    recvCRC |= (pg->data[pg->dataLength + 1] << 8);
    recvCRC |= pg->data[pg->dataLength];
		//delay_ms(2000);
    if (localCRC == recvCRC)
    {
			//send_buf((uint8_t *)currentPg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
			if(pg->function >= 0x01 && pg->function <= 0x06){
				ctrl_engine(pg->function, *(uint32_t *)pg->data, 7);
				send_buf((uint8_t *)currentPg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
			}
			else if(pg->function == 0x07)
			{
				for(int i=0; i<=6; i++)
				{
					ctrl_engine(i, *(uint32_t *)(pg->data + 4 * (i-1)), 7);
				}
				send_buf((uint8_t *)currentPg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
			}
			else if(pg->function == 0x08)
			{
				beep_on(300);
				send_buf((uint8_t *)currentPg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);
			}
    }
    else
    {
//        ui->textBrowser->insertPlainText("CRC????!\n");
    }
		
		free(pg);
		flagParse = 0;
}

int main()
{
	uint8_t buf[10] = {0x12, 0x34, 0x56, 0x78, 0x9a};
	Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + 5 + PARITY_LEN);
	user_GPIO_init();
	user_UART_init();
	
	tim2_pwm_init();
	tim3_pwm_init();
	tim4_pwm_init();
	//Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + 5 + PARITY_LEN);
	pg->deviceAddr = 0x00;
	pg->function = 0x06; 
	pg->dataLength = 5;
	memcpy(pg->data, buf, 5);
	//sendPackage(pg);
	
	//send_buf(buf, 10);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	
	while (1)
	{
		if (flagParse == 1)
		{
			parsePackage(currentPg);
		}
	}
	
	return 0;
}
