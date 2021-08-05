#include <monitor.h>
#include <reg51.h>
#include <string.h>

/*variables globales */
extern unsigned char Timer_wait;
extern unsigned char USE_LPR;
extern unsigned char  Debug_Tibbo;
extern unsigned char ValTimeOutCom;		
extern unsigned char Tipo_Vehiculo;
extern unsigned char placa[];

/*configuracion bits*/

sbit rx_ip = P0^0;	
sbit lock = P1^7;						//Relevo 

/*externos bits*/

extern bit placa_ready;

/*funciones prototipo*/
extern unsigned char rx_Data(void);
extern unsigned char Dir_board();
extern void serie_ascii_siceros_l(unsigned char *serie);
extern void Block_read_clock_ascii(unsigned char *datos_clock);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void tx_aux(unsigned char caracter);
extern unsigned char hex_bcd (unsigned char byte);
extern void serie_ascii_siceros_l(unsigned char *serie);


#define STX											02 
#define ETX											03 
/*------------------------------------------------------------------------------
Rutina q recibe  los cmd de Monitor por el tibbo
return el num de caracteres recibidos
y almacena la trama en un apuntador
------------------------------------------------------------------------------*/
unsigned char recibe_cmd_Monitor(unsigned char *buffer)
{
	unsigned char j, NumDatos,time_out,MaxChrRx;
	unsigned int contador;
	
		NumDatos=0;
		MaxChrRx=12;

	if (USE_LPR==1)
	{
			for (j=0; j<MaxChrRx; j++)
			{
				contador=0;
				time_out=0;
				while ((rx_ip==1)&&(time_out==0))
				{
					contador++;
					if (contador>30000)
					{
						time_out=1;
						j=MaxChrRx;
					}				
				}
				if(time_out==1)break;
					
	 			*buffer=rx_Data();
					buffer++;
					NumDatos++;	
			}
						
		*buffer=0;
	}
	return	NumDatos;
}	

/*------------------------------------------------------------------------------
Rutina q valida los cmd de Monitor
------------------------------------------------------------------------------*/

void Valida_Trama_Monitor(unsigned char *buffer, unsigned char length_trama)
{		
	unsigned char j=0;
	unsigned char p=2;
	length_trama=1;
		if	((*(buffer+2)==ETX)&&(*(buffer+1)=='P')) 																																						/* APERTURA DE BARRETA*/ 
				{
	 				lock=1;																																																						/*habilita el relevo ON*/
					Timer_wait=0;
	 			}
		else if (*(buffer+1)=='<')
		{
			/*placa*/
			do
			{
				placa[j]=*(buffer+p);
				p++;
				j++;
			}while (*(buffer+p) != ETX);
			*(buffer+p)=0;
				placa[j-1]=0;
			  placa_ready=1;
			//ValTimeOutCom=10;
			/*placa no play <NO_PLATE>*/ 
			
		}
		else if (*(buffer+1)=='[')
		{
			/*cancel*/
		}
}
/*------------------------------------------------------------------------------
Comando de la tarjeta para monitor
indica la entrada de un vehiculo, envia el tipo de vehiculo detectado
(E) cmd que indica salida
------------------------------------------------------------------------------*/
/*no usado para esta aplicacion*/
//void Cmd_Monitor(unsigned char cmd)
//{
//	unsigned char p,t=0;
//	unsigned char serie[10];
//	static unsigned char Buffer_Lpr[30];
//	unsigned char j=3;
//	Buffer_Lpr[0]=STX;																			/*inicio de cmd STx*/
//	Buffer_Lpr[1]=Dir_board();															/*direccion de la tarjeta*/
//	Buffer_Lpr[2]=cmd;																			/*cmd S que salida wiegand*/
//	if(Tipo_Vehiculo!=0)																		/*Tipo de vehiculo*/
//		{
//			Buffer_Lpr[j++]='M';																/*moto*/
//		}
//		else
//		{
//			Buffer_Lpr[j++]='C';																/*carro*/
//		}
//			serie_ascii_siceros_l(serie);
//			p=strlen(serie);
//	do
//	{
//	 Buffer_Lpr[j++]=serie[t];															/*ticket o consecutivo*/
//		p--;
//		t++;
//	}while (p!=0);	
//	Buffer_Lpr[j++]=':';																/*separador del tab  o tarjeta MF*/
									
//	Block_read_clock_ascii(Buffer_Lpr+j);								/*año,mes,dia,hora,minutos,*/
	
//	Buffer_Lpr[j+10]=':';																/*separador fecha*/
//	Buffer_Lpr[j+11]=ETX;																/*fin de la trama*/
		
					
//	DebugBufferMF(Buffer_Lpr,j+12,0);		
//	Monitor_chr(Buffer_Lpr,j+12);												/*rutina de envio de la trama a monitor*/
//}

/*------------------------------------------------------------------------------
Transmito un caracter al software monitor 
------------------------------------------------------------------------------*/
void Monitor_chr (unsigned char *str,unsigned char num_char)
{
	unsigned char j;
	for (j=0; j<num_char; j++)
		{
		tx_aux(*str);
		str++;
		}
}
	
/*------------------------------------------------------------------------------
Transmito CMD de salida wiegand 
------------------------------------------------------------------------------*/
void Cmd_LPR_Salida_wiegand(unsigned char *buffer)
{
	unsigned char Buffer_Lpr[30];
	unsigned char j=3;
	Buffer_Lpr[0]=STX;																			/*inicio de cmd STx*/
	Buffer_Lpr[1]=Dir_board();															/*direccion de la tarjeta*/
	Buffer_Lpr[2]='S';																			/*cmd S que salida wiegand*/
	if(Tipo_Vehiculo!=0)																		/*Tipo de vehiculo*/
		{
			Buffer_Lpr[j++]='M';																/*moto*/
		}
		else
		{
			Buffer_Lpr[j++]='C';																/*carro*/
		}
	
		
		
		for (j=4; *buffer != '\0'; j++)												/*numero del tab o tarjeta Mf*/
			{
		   		Buffer_Lpr[j]=*buffer;
					buffer++;
				
			}
			Buffer_Lpr[j++]=':';																/*separador del tab  o tarjeta MF*/
									
			Block_read_clock_ascii(Buffer_Lpr+j);								/*año,mes,dia,hora,minutos,*/
			Buffer_Lpr[j+10]=':';																/*separador fecha*/
			Buffer_Lpr[j+11]=ETX;																/*fin de la trama*/
		
					
		
			Monitor_chr(Buffer_Lpr,j+12);												/*rutina de envio de la trama a monitor*/
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
/*TEMPORALMENTE FUERA DE SERVICIO*/

//void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2)
//{
	
	
//	unsigned char Buffer_Lpr[30];
//	unsigned temp;
//	unsigned char j=3;
//	Buffer_Lpr[0]=STX;																/*inicio de cmd STx*/
//	Buffer_Lpr[1]=Dir_board();												/*direccion de la tarjeta*/
//	Buffer_Lpr[2]='S';																/*numero de digitos de transmicion NO IMPORTA NO ES VALIDADO EN PRINCIPAL*/
	
//		if(*(buffer_S1_B2+8)!=0)												/*Tipo de vehiculo*/
//		{
//			Buffer_Lpr[j++]='M';													/*moto*/
//		}
//		else
//		{
//			Buffer_Lpr[j++]='C';													/*carro*/
//		}
	
	
//	do
//	{
//	 Buffer_Lpr[j++]=*buffer_S1_B0;									/*ticket o consecutivo*/
//		buffer_S1_B0++;
//	}while (*buffer_S1_B0!=0);
	
	
	
	
	
//		Buffer_Lpr[j++]=':';														/*separador de la fecha de entrada*/

//		temp=hex_bcd(*(buffer_S1_B2+0));								/*año a ascii*/
//		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
//		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
		
//		temp=hex_bcd(*(buffer_S1_B2+1));								/*mes a ascii*/
//		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
//		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
//		temp=hex_bcd(*(buffer_S1_B2+2));								/*Dia a ascii*/
//		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
//		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
//		temp=hex_bcd(*(buffer_S1_B2+3));								/*Hora a ascii*/
//		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
//		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
//		temp=hex_bcd(*(buffer_S1_B2+4));								/*Minutos a ascii*/
//		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
//		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
		
	
//		Buffer_Lpr[j++]=':';														/*separador tipo fecha*/
																										/**/
				
//		Buffer_Lpr[j++]=ETX;	
	
//		Monitor_chr(Buffer_Lpr,j);												/*rutina de envio de la trama a monitor*/
//}
void Cmd_LPR_Salida_print(unsigned char *msj,unsigned char vehiculo)
{
	
	unsigned char Buffer_Lpr[30];
	unsigned char j;
 	unsigned char temp[4];
	Buffer_Lpr[0]=STX;																/*inicio de cmd STx*/
	Buffer_Lpr[1]=Dir_board();												/*direccion de la tarjeta*/
	Buffer_Lpr[2]='S';
	Buffer_Lpr[3]=0;																	/*finalizo la trama para sumarla con la otra trama*/
	temp[0]=vehiculo;
	temp[1]=0;																					
  strcat(Buffer_Lpr,temp);												  /*Tipo de vehiculo*/
  strcat(Buffer_Lpr,msj);											  		/*une los dos strim copio el Ticket */
	temp[0]=':';
	temp[1]=0;
	strcat(Buffer_Lpr,temp);											  		/*une los dos strim copio los : del inicio de trama de fecha */
	j=strlen(Buffer_Lpr);															/*longitud de la trama*/
	
	Block_read_clock_ascii(Buffer_Lpr+j);							/*leo la fecha*/
	

	Buffer_Lpr[j+10]=':';	
	Buffer_Lpr[j+11]=ETX;
	Buffer_Lpr[j+12]=0;
	Buffer_Lpr[j+13]=0;
	//strcat(Buffer_Lpr,":");									/*sumo fecha de salida y tipo de vehiculo*/	
	j=strlen(Buffer_Lpr);
	//Buffer_Lpr[j]=ETX;															 /*le sumo el fin de la trama*/	
	Monitor_chr(Buffer_Lpr,j);												/*rutina de envio de la trama a monitor*/

}
/*
void clear_placa()
{
	unsigned char i;
	for(i=0;i<9;i++)
	{
	 placa[i]=0;
	}
	 placa_ready=0;
}
*/