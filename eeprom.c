
#include <eeprom.h>
#include <reg51.h>
#include <string.h>
/*funciones externas*/

extern void          _nop_     (void);
extern void Delay (void);
extern void Delay_20us(unsigned int cnt);
extern void Delay_1ms(unsigned int cnt);

extern void Debug_Dividir_texto();
extern void Debug_txt_Tibbo(unsigned char * str);
extern void Debug_chr_Tibbo(unsigned char Dat);
//******************************************************************************************
// 		RUTINAS DE EEPROM 24FC1025
//******************************************************************************************
sbit sda = P1^2;
sbit scl = P1^3;
//******************************************************************************************

bit memory_flag;
unsigned char byte_rd;
unsigned char l_data_eeprom;
unsigned char l_chr;

/*define posiciones de memoria*/
#define EE_ID_CLIENTE		0x0000


//*******************************************************************************************
void ackd(void)
{
	unsigned int i;
	memory_flag=1;
	scl=1;
	sda=1;
	for (i=0; i<1100; i++)					   //500
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;

		break;
	  }
	}
	
	if (memory_flag==1)
	{ 

	}  
}	
//*******************************************************************************************
//*******************************************************************************************
void ack_lect(void)
{
	int i;
	scl=1;
	sda=1;
	memory_flag=1;
	for (i=0; i<650; i++)
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;
		break;
	  }
	}
}	
//*******************************************************************************************
void p_me (void)
{
	  scl=1;
	  _nop_();
	  scl=0;
}
//*******************************************************************************************
void e_d_m (unsigned char a_serial_eeprom)
{
	unsigned char nbits;
  	for (nbits=0; nbits<8; nbits++)
	{
	  a_serial_eeprom <<=1;
	  if (CY==1)
	  { 
	    sda=1;
	  }
	  else
	  {
	    sda=0;
	  }
	  scl=1;
	  p_me();  				
	}
	return;
}
//********************************************************************************************
void start (void)
{
 	sda=1;
 	scl=1;
 	_nop_();
 	sda=0;
 	scl=0;
 	return;
}
//*******************************************************************************************
void stop (void)
{
 	scl=0;
 	sda=0;
 	scl=1;
 	sda=1;
 	return;
}
//*******************************************************************************************
unsigned char l_d_m (void)
{
	unsigned char nbits;
   	for (nbits=0; nbits<8; nbits++)
	{
		scl=0;
   		if (sda==1) 
	  	{
			l_data_eeprom = l_data_eeprom| 0x01;
			if (nbits<=6) 
			{
				l_data_eeprom<<=1;
				scl=1;
			}
	  	}
	   	if (sda==0)
	  	{
	  		l_data_eeprom = l_data_eeprom & 0xFE;
			if (nbits<=6) 
		 	{
		  		l_data_eeprom <<=1;
				scl=1;
		 	}
 	  	}
    }
	scl=0;
 	return l_data_eeprom;
}
//*******************************************************************************************
//*******************************************************************************************
//	ESCRIBE EN EEPROM																		*
//*******************************************************************************************
//void wr_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l,unsigned char data_eeprom)
void wr_eeprom (unsigned char control,unsigned int Dir, unsigned char data_eeprom)
{
	unsigned char dir_h, dir_l;
	dir_l=Dir;
	Dir>>=8;
	dir_h=Dir;


 	scl=0;
	sda=0;
// 	wait();
    start();
	e_d_m(control);
	ackd();
	e_d_m(dir_h);
	ackd();
	e_d_m(dir_l);
	ackd();
	e_d_m(data_eeprom);
	ackd();
	stop();
	Delay_1ms(13);
	
	scl=1;
	sda=1;
	Delay_20us(98);										/*wait long*/
	Delay_20us(98);
}
/*------------------------------------------------------------------------------
	ESCRIBE EN EEPROM	n caracteres		 pasados en un apuntador															
------------------------------------------------------------------------------*/
/*
void wrpage_eeprom (unsigned char control,unsigned int Dir,unsigned char *data_eeprom)  
{
 	unsigned int j;
	unsigned char dir_h, dir_l,num_chr;
	dir_l=Dir;
	//Dir>>=8;
	dir_h=Dir >>8;
  num_chr=strlen(data_eeprom);
	Debug_Dividir_texto();	
	Debug_txt_Tibbo(data_eeprom)	;
	Debug_chr_Tibbo(num_chr);
	Debug_Dividir_texto();			
	scl=0;
	sda=0;
 	//wait();
	Delay_20us(98);
	Delay_20us(98);
	//wait();

    start();
	e_d_m(control);
	ackd();
	e_d_m(dir_h);
	ackd();
	e_d_m(dir_l);
	ackd();
 	for (j=0; j<=num_chr; j++)
	{
		e_d_m(*data_eeprom);
		data_eeprom++;
		ackd();
	}
	stop();
	Delay_1ms(13);
	scl=1;
	sda=1;
 
}		
*/
//*******************************************************************************************	
//*******************************************************************************************	
//	LEE EN EEPROM																			*
//*******************************************************************************************

unsigned char rd_eeprom (unsigned char control,unsigned int Dir) 
{

	unsigned char dir_h, dir_l;
	dir_l=Dir;
	Dir>>=8;
	dir_h=Dir;
	
 	scl=0;
	sda=0;
 //	wait();
 	start();
	e_d_m(control);
	ack_lect();
	e_d_m(dir_h);
  	ack_lect();
	e_d_m(dir_l);
 	ack_lect();
	start();
	e_d_m(control+0x01);
 	ackd();
	scl=0;
 	l_d_m();
	stop();
  	scl=1;
	sda=1;
	return l_data_eeprom;
}

/*---------------------------------------------------------------------------------------------------------------------------------------
Rutina que lee la eeprom  en bloque 
----------------------------------------------------------------------------------------------------------------------------------------*/
void LeerMemoria(unsigned int addres,unsigned char *res)
{
	

unsigned char i;
do 
	{
 	*res=rd_eeprom (0xa8,addres);
	i=*res;
	addres++;
	res++;
	}
	while(i !='\0');
 	
}
/*---------------------------------------------------------------------------------------------------------------------------------------
escribe un bloque de datos en la eeprom
	Escribe hasta encontrar el CR y lo cambia por (0) que es el fin de la cadena
----------------------------------------------------------------------------------------------------------------------------------------*/

void EscribirMemoria(unsigned char control,unsigned int  addres,unsigned char  *res)
{

while(*res !='\0')																	/**/
	{
		if(*res =='\r')
		{
			
			*res='\0';
		}  
    wr_eeprom(control,addres,*res);         
    addres++;
		res++;
	}
	*res=0;
	wr_eeprom(control,addres,*res); 
}
