/*
  Longan Nano LCD driver
*/

#include "lcd.h"
#include "oledfont.h"

u16 BACK_COLOR;	// Background color

typedef struct{
	u8 configured;
	u8 inverted;
	u8 offset_x;
	u8 offset_y;
}lcd_config_t;

lcd_config_t lcd_conf = {0};


void lcd_delay_1ms(uint32_t count)
{
	uint64_t start_mtime, delta_mtime;

	// Don't start measuring until we see an mtime tick
	uint64_t tmp = get_timer_value();
	do {
	start_mtime = get_timer_value();
	} while (start_mtime == tmp);

	do {
	delta_mtime = get_timer_value() - start_mtime;
	}while(delta_mtime <(SystemCoreClock/4000.0 *count ));
}



int r=0, w=0, queue[256]={0};                       // 256 Byte wr queue

void LCD_Wait_On_Queue(){
	while(r != w) LCD_WR_Queue();					//Blocks while emptying the queue
}

void LCD_WR_Queue(){
    if (r!=w) {                                     // Buffer empty?
       if (spi_i2s_flag_get(SPI1,SPI_FLAG_TBE)) {   // ...no! Device redy?
          OLED_CS_Clr();                            // ......Yes! CS (again)
          (queue[r]>=1<<8) ? OLED_DC_Set() : OLED_DC_Clr(); //    DC
          spi_i2s_data_transmit(SPI1, queue[r++]&0xFF); //        Write!
          r%=256;                                   //            Advance.
        }                                           //       (No! Return!)
    } else {
        OLED_CS_Set();                              // ...yes! CS high, done!
    }
}

void LCD_Write_Bus(int dat) {
   while (((w+1)%256)==r) LCD_WR_Queue(); //If buffer full then spin...
   queue[w++]=dat;                        //...If/when not then store data...
   w%=256;                                //...and advance write index!
}

/*
  Function description: LCD serial data write function (write one byte)
  Entry data: dat: byte to be written
  Return value: None
  Note: DC must already be set (for data) or reset (for command)
*/
void LCD_Writ_Bus(u8 dat) 
{
	OLED_CS_Clr();

	while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
        spi_i2s_data_transmit(SPI1, dat);


	while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE));
        spi_i2s_data_receive(SPI1);

	OLED_CS_Set();  
}


/*
  Function description: LCD write 8-bit data
  Entry data: dat: data to be written
  Return value: None
*/
void LCD_WR_DATA8(u8 dat)
{
	//OLED_DC_Set();  // Write data
	//LCD_Writ_Bus(dat);
    LCD_Write_Bus(((int)dat)+(1<<8));
}


/*
  Function description: LCD write 16-bit data
  Entry data: dat: 16-bit data to be written
  Return value: None
*/
void LCD_WR_DATA(u16 dat)
{
	//OLED_DC_Set();  // Write data
	//LCD_Writ_Bus(dat>>8);
	//LCD_Writ_Bus(dat);
    LCD_Write_Bus(((int)dat>>8)+(1<<8));
    LCD_Write_Bus(((int)dat&0xFF)+(1<<8));
}


/*
  Function description: LCD write 8-bit command
  Entry data: dat: command to be written
  Return value: None
*/
void LCD_WR_REG(u8 dat)
{
	//OLED_DC_Clr();  // Write command
	//LCD_Writ_Bus(dat);
    LCD_Write_Bus((int)dat);
}



/*
  Function description: Set start and end addresses
  Entry data: x1, x2 set the start and end column address
              y1, y2 set the start and end row address
  Return value: None
*/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_WR_REG(0x2a);  // Column address setting
	LCD_WR_DATA(x1+lcd_conf.offset_x);
	LCD_WR_DATA(x2+lcd_conf.offset_x);
	LCD_WR_REG(0x2b);  // row address setting
	LCD_WR_DATA(y1+lcd_conf.offset_y);
	LCD_WR_DATA(y2+lcd_conf.offset_y);
	LCD_WR_REG(0x2c);  // Memory write
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    OLED_CS_Set();
    spi_struct_para_init(&spi_init_struct);

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_4;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

	spi_crc_polynomial_set(SPI1,7);
	spi_enable(SPI1);
}

void Lcd_SetType(int type){
	if(type == LCD_NORMAL){
		lcd_conf.configured = 1;
		lcd_conf.offset_x = 1;
		lcd_conf.offset_y = 26;
		lcd_conf.inverted = 0;
	}
	if(type == LCD_INVERTED){
		lcd_conf.configured = 1;
		lcd_conf.offset_x = 0;
		lcd_conf.offset_y = 24;
		lcd_conf.inverted = 1;
	}
}


/*
  Function description: LCD initialization function
  Entry data: None
  Return value: None
*/
void Lcd_Init(void)
{
	if(!lcd_conf.configured) Lcd_SetType(LCD_NORMAL);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);

 	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_SPI1);
	
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 |GPIO_PIN_14| GPIO_PIN_15);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15); //CS

	spi_config();

	gpio_bit_reset(GPIOC, GPIO_PIN_13 | GPIO_PIN_15);
	LCD_Wait_On_Queue();
	lcd_delay_1ms(100);
	

	LCD_WR_REG(0x01); 	//SW reset
	LCD_Wait_On_Queue();
	lcd_delay_1ms(120);
	

	LCD_WR_REG(0x11); 	//SLPOUT
	LCD_Wait_On_Queue();
	lcd_delay_1ms(100);

	if(lcd_conf.inverted) LCD_WR_REG(0x22); 
	else				  LCD_WR_REG(0x21); 	//INVON

	LCD_WR_REG(0xB1); 	//FRMCTRL1 - Full color
	LCD_WR_DATA8(0x05); // Framerate = 333khz / (25) * (196) = 67.9fps
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x3A);

	LCD_WR_REG(0xB2);	//FRMCTRL - 8-bit color
	LCD_WR_DATA8(0x05); //fps = 67.9
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x3A);

	LCD_WR_REG(0xB3);   //Partial mode
	LCD_WR_DATA8(0x05);  
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x3A);

	LCD_WR_REG(0xB4);  //INVCTR - Inversion control
	LCD_WR_DATA8(0x03);//Line inversion(full color) | Frame inversion

	LCD_WR_REG(0xC0);  //PWRCTR1 - Set GVDD voltage
	LCD_WR_DATA8(0x62);//
	LCD_WR_DATA8(0x02);//
	LCD_WR_DATA8(0x04);//

	LCD_WR_REG(0xC1); //More power regulation
	LCD_WR_DATA8(0xC0);

	LCD_WR_REG(0xC2); //Same
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC3); //same
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x6A);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D); 
	LCD_WR_DATA8(0xEE); 

	LCD_WR_REG(0xC5);  /*VCOM*/
	LCD_WR_DATA8(0x0E);    

	LCD_WR_REG(0xE0);  //Gamma correction
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0x3A);  //Set color resolution
	LCD_WR_DATA8(0x05);//16 bit color

	LCD_WR_REG(0x36); //Data access mode
	LCD_WR_DATA8(0x78);
	LCD_WR_REG(0x29); 
} 


/*
  Function description: LCD clear screen function
  Entry data: Color: color to set as background
  Return value: None
*/
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	  {
			for (j=0;j<LCD_H;j++)
				{
					LCD_WR_DATA(Color);
				}
	  }
}


/*
  Function description: LCD display Chinese characters
  Entry data:  x, y: start coordinates
              index: Chinese character number
               size: font size
  Return value: None
*/
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)	
{  
	u8 i,j;
	u8 *temp,size1;
	if(size==16){temp=Hzk16;}               // Choose a font size
	if(size==32){temp=Hzk32;}
    LCD_Address_Set(x,y,x+size-1,y+size-1); // Set a region of Chinese characters
    size1=size*size/8;                      // The bytes occupied by a Chinese character
	temp+=index*size1;                      // Start of writing
	for(j=0;j<size1;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0) {         // Read from the lower bit of the data
				LCD_WR_DATA(color);         // set dot color
			}
			else
			{
				LCD_WR_DATA(BACK_COLOR);    // set dot color to background
			}
		}
		temp++;
	 }
}


/*
  Function description: LCD display Chinese characters
  Entry data: x, y: start coordinates
  Return value: None
*/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y); // Set cursor position
	LCD_WR_DATA(color);
} 


/*
  Function description: LCD draws a large dot
  Entry data: x, y: start coordinates
  Return value: None
*/
void LCD_DrawPoint_big(u16 x,u16 y,u16 color)
{
	LCD_Fill(x-1,y-1,x+1,y+1,color);
} 


/*
  Function description: fill color in the specified area
  Entry data: xsta, ysta:  start coordinates
              xend, yend:  end coordinates
  Return value: None
*/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);          //Set cursor position
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color); //Set cursor position
	} 					  	    
}


/*
  Function description: draw a line
  Entry data: x1, y1:  start coordinates
              x2, y2:  end coordinates
  Return value: None
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1;                       // Calculate coordinate increments
	delta_y=y2-y1;
	uRow=x1;                             // Coordinates of starting point of drawing
	uCol=y1;
	if(delta_x>0)incx=1;                 // Set single step direction
	else if (delta_x==0)incx=0;          // Vertical line
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;          // Horizontal line
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; // Pick basic incremental axis
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);  // Dot
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/*
  Function description: draw a rectangle
  Entry data: x1, y1:  start coordinates
              x2, y2:  end coordinates
  Return value: None
*/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/*
  Function description: draw circle
  Entry data: x0, y0:  center coordinates
                   r:  radius
  Return value: None
*/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	// int di;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);  //3           
		LCD_DrawPoint(x0+b,y0-a,color);  //0           
		LCD_DrawPoint(x0-a,y0+b,color);  //1                
		LCD_DrawPoint(x0-a,y0-b,color);  //2             
		LCD_DrawPoint(x0+b,y0+a,color);  //4               
		LCD_DrawPoint(x0+a,y0-b,color);  //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r)) // Determine whether the points to be drawn are too far away
		{
			b--;
		}
	}
}


/*
  Function description: display characters
  Entry data: x, y:  start point coordinates
               num:  characters to display
              mode:  1: transparent mode
                     0: non-transparent mode
  Return value: None
*/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color)
{
    u8 temp;
    u8 pos,t;
	  u16 x0=x;    
    if(x>LCD_W-8 || y>LCD_H-16)return;	// Outside of display area
	num=num-' ';                        // Get offset value
	LCD_Address_Set(x,y,x+8-1,y+16-1);  // Set cursor position
	if(!mode)
	{
		// non-trasparent mode
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];  // load 1608 font character
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(color);
				else LCD_WR_DATA(BACK_COLOR);
				temp>>=1;
				x++;
		    }
			x=x0;
			y++;
		}	
	}else
	{
		// Transparent mode
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos]; // load 1608 font character
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color); //Draw a dot
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}


/*
  Function description: display string
  Entry data: x, y:  start point coordinates
                *p:  string start address
  Return value: None
  Note: If character position is outside the display area
        display is cleared in red color and the character
		position is set to (0,0)
*/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-8){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }  
}


/*
  Function description: display string
  Entry data: x, y:  start point coordinates
                *p:  string start address
              mode:  1: transparent mode
                     0: non-transparent mode
  Return value: None
  Note: If character position is outside the display area
        the character is not displayed
*/
void LCD_ShowStr(u16 x,u16 y,const u8 *p,u16 color, u8 mode)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-8){x=0;y+=16;}
        if(y>LCD_H-16) break;
        LCD_ShowChar(x,y,*p,mode,color);
        x+=8;
        p++;
    }  
}


/*
  Function description: Calculate m^n
  Entry data: m: base
              n: exponent
  Return value: m^n
*/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}


/*
  Function description: display 16-bit integer numbers
  Entry data: x, y:  start point coordinates
               num:  number to display
               len:  number of digits to display
  Return value: None
*/
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0,color);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color); 
	}
} 


/*
  Function description: display float number with 2 decimal places
  Entry data: x, y:  start point coordinates
               num:  float number to display
               len:  number of digits to display
  Return value: None
*/
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
{         	
	u8 t,temp;
	// u8 enshow=0;
	u16 num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+8*(len-2),y,'.',0,color);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
	}
}


/*
  Function description: display the image 
  Entry data: x1, y1:  start coordinates
              x2, y2:  end coordinates
              *image:  pointer to image buffer
  Return value: None
  Note: image buffere contains 16-bit pixel colors
        and its size must be (x2-x1+1) * (y2-y1+1) * 2
  */
void LCD_ShowPicture(u16 x1, u16 y1, u16 x2, u16 y2, u8 *image)
{
	int i;
	int size = (x2-x1+1) * (y2-y1+1) * 2;
	LCD_Address_Set(x1,y1,x2,y2);
	for(i=0;i<size;i++)
	{ 	
		LCD_WR_DATA8(image[i]);
	}			
}


