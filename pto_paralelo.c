#include <pto_paralelo.h>
#include <reg51.h>
#include <string.h>

extern void Delay (void);
extern void Delay_20us(unsigned int cnt);
extern void off_Timer0_Interrup(void);
extern void on_Timer0_Interrup(void);
extern void          _nop_     (void);
extern void Block_read_Clock(unsigned char *datos_clock);
extern char lee_clk (unsigned char dir_clk);
extern int sprintf  (char *, const char *, ...);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_Dividir_texto();
extern void Block_read_Clock_Hex(unsigned char *datos_clock);
extern void serie_ascii_siceros_l(unsigned char *serie);
extern void LeerMemoria(unsigned int addres,unsigned char *res);
extern void ceros_izq(unsigned char *dato);
extern unsigned char bcd_hex (unsigned char l_data);
extern void Debug_txt_Tibbo(unsigned char * str);


/*pines del pto paralelo*/
sbit port_clk = P3^4;				//Recepcion AUX											*
sbit busy = P3^3;  					/*bussy de Entrada Interrupcion del Procesador principal			*/
sbit ready = P3^2;					//Salida. solicitud envio Datos							*
sbit rx_in_data = P0^6;				//Indicador de Rx Transporte o Lectura Wiegand			*
sbit led_err_imp = P0^2;			//Error 	

#define MAX_DAT_BUFF 		40
#define TIME_WBUS 			20000
#define STX							02 
#define ETX							03 
/*concecutivo de la factura*/
#define EE_CONSECUTIVO3	0X0500

/*definiciones del reloj*/

/*CMD DE LECTURA DEL RELOJ*/
#define RDIA						0x87
#define RMES						0x89
#define RANO						0x8D
#define RHORA						0x85
#define RMIN						0x83

/*definicion de variable globales*/
extern int ID_CLIENTE;						
extern int COD_PARK;
extern unsigned char placa[];
extern unsigned char Tipo_Vehiculo;
/*------------------------------------------------------------------------------

Recibe informacion del Mc principla por pto paralelo
retorna:
num_data= numero de caracteres recibidos
buffer_port= apuntador donde se almacena el dato recibido
busy=0    significa que el principla me transmite una cadena de datos
Ready=0   significa que estoy listo para recibir los datos
time_out= es el tiempo de espera que le damos al ciclo de reloj en alto si se pasa el tiempo se sale por fin de la transmicion 
rx_in_data= 0 prende el led que estoy recibiendo un dato (1) termina la recepcion del dato

------------------------------------------------------------------------------*/
unsigned char recibe_port(unsigned char *buffer_port)
{

	unsigned int count;
	unsigned char time_out,num_data;
	unsigned int T_ANCHO=152;
	
		
		off_Timer0_Interrup();				
		num_data=0;
  	rx_in_data=0;
		time_out=0;
		ready=0;
		count=T_ANCHO;

	
		while((busy==0)&&(time_out==0))
		{
			count=T_ANCHO;
			do
			{
				count--;
					if (count==0)
					{
						time_out=1;
					}
					
			}while((port_clk==1)&&(time_out==0));													/*pulso de reloj alto esperamos el cambio de estado del reloj a 0  para recibir transmicion*/
				if ((port_clk==0)&&(time_out==0))
				{
				*buffer_port=P2;																						/*almaceno informacion en el buffer*/
				num_data++;
				buffer_port++;
				count=T_ANCHO;
				}
				else
				{ break;}
			do
			{
								
			}while ((port_clk==0));																				/*espero el estado de subida del reloj a 1 para comensar el ciclo*/
				
		}
	
		ready=1;
		rx_in_data=1;
		on_Timer0_Interrup();
	
	return num_data;
}


/*-------------------------------------------------------------------------------------------

Rutina de envio de datos pto paralelo

-------------------------------------------------------------------------------------------*/
void  send_port(unsigned char *buffer_port, unsigned char length_char)
{
 
	long int cont;
	unsigned char timeOut,length;
	
	length=length_char;

 	ready=1;																				// Genera interrupcion al Principal
	port_clk=1;	 																		// El que transmite debe fijar primero el Clk en 1
	rx_in_data=0;	 																	// Led de visualizacion 	ON
	timeOut=0;																			// Tiempo de Espera
  ready=0;																				// Genera interrupcion al Principal
	cont=TIME_WBUS;

 	while ((busy==1)&&(timeOut==0)) 								//Espera reconocimiento INT por entrada busy
	{
		ready=0;
		port_clk=1;
		cont--;
		if (cont==0)
		{
			timeOut=1;
			led_err_imp=0;															// LED ERROR
		}
	}
	if ((timeOut==0)&&(busy==0))
	{
			
		
		do 
		{
			
					P2=*buffer_port;
					Pulso_Bus();
					buffer_port++;
			
					length_char--;
		}while (length_char);
			
		}
	P2=0XFF;
	ready=1;
	port_clk=1;
	rx_in_data=1;																		//Led de visualizacion 	OFF
	Delay_20us(98);
			
		
}

/*-------------------------------------------------------------------------------------------
Funcion q arma la trama a transmitir pto paralelo trama P
-------------------------------------------------------------------------------------------*/	
/*TEMPORALMENTE FUERA DE SERVICIO EXPEDIDOR TICKET*/
//void Trama_pto_Paralelo_P(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd)
//{
	
//static unsigned char Buffer_port[30];
//	unsigned char j=3;
//	Buffer_port[0]=STX;																/*inicio de cmd STx*/
//	Buffer_port[1]=cmd;																/*cmd*/
//	Buffer_port[2]=21;																/*numero de digitos de transmicion NO IMPORTA NO ES VALIDADO EN PRINCIPAL*/
//	do
//	{
//	 Buffer_port[j++]=*buffer_S1_B0;									/*ticket*/
//		buffer_S1_B0++;
//	}while (*buffer_S1_B0!=0);
//	Buffer_port[2]=j-3;
//	if(*(buffer_S1_B2+5)!=0)													/*MF_DCTO						0x05				Tipo de descuento (00)sin descuento, (01xx xxxx) 0x40 fija fecha de salida,
//																										10xx xxxx dcto por porcentaje xx xxxx= valor del porcentaje, 11xx xxxx dcto por dinero */
//	{
//		Buffer_port[j++]='-';														/*separador del descuento*/
//		Buffer_port[j++]=*(buffer_S1_B2+5)+0x30;				/*descuento pasado a ascii*/
//	}
	

	
//		Buffer_port[j++]='.';														/*separador tipo de vehiculo*/
																										/*MF_TIPO_VEHICULO	0x08							tipo vehiculo 00 carro, 01 moto, 02 bicicleta, 04 tractomula*/
		
//		if(*(buffer_S1_B2+8)!=0)
//		{
//			Buffer_port[j++]='M';													/*moto*/
//		}
//		else
//		{
//			Buffer_port[j++]='C';													/*carro*/
//		}
		
		
//		ready=0;
			//while(busy==0);
//		send_port(Buffer_port,j);													/*trama transmitida pto paralelo*/
//			Debug_Dividir_texto();																							/*division del texto */
							
//			DebugBufferMF(Buffer_port,j,1);		
//			Debug_Dividir_texto();	
		
			
//}

/*-------------------------------------------------------------------------------------------
Funcion q arma la trama a transmitir pto paralelo trama 
C = cajero
s =
-------------------------------------------------------------------------------------------*/	
//void Trama_pto_Paralelo_C_s(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2)
//{
//	unsigned char Buffer_port[30];
//	unsigned char j=2;
//	Buffer_port[0]=STX;																/*inicio de cmd STx*/
	//Buffer_port[1]=cmd;																/*cmd*/
	
//	do
	//{
	// Buffer_port[j++]=*buffer_S1_B0;									/*ticket*/
	//	buffer_S1_B0++;
	//}while (*buffer_S1_B0!=0);
	
	
				
	
	
	//if(*(buffer_S1_B2+5)!=0)													/*MF_DCTO						0x05				Tipo de descuento (00)sin descuento, (01xx xxxx) 0x40 fija fecha de salida,
//																										10xx xxxx dcto por porcentaje xx xxxx= valor del porcentaje, 11xx xxxx dcto por dinero */
	//{
		//Buffer_port[j++]='-';														/*separador del descuento*/
		//Buffer_port[j++]=*(buffer_S1_B2+5)+0x30;				/*descuento pasado a ascii*/
	//}
	//else
	//{
	//	Buffer_port[j++]='-';														/*separador del descuento*/
	//	Buffer_port[j++]='0';
	//}
	//if((*(buffer_S1_B2+9)&0x0f)==0x0f)									/*MF_IN_PAGO=0x09				si es =0x0f fue liquidado en cajero 	*/	
	//{
		//Buffer_port[1]='C';
	//}
	//else
	//{
	//	Buffer_port[1]='s';
	//}
	//	Buffer_port[j++]='-';														/*separador de la fecha de entrada*/
	//	Buffer_port[j++]=*(buffer_S1_B2+0)+0x030;				/*año de entrada*/
	//	Buffer_port[j++]=*(buffer_S1_B2+1)+0x030;				/*mes de entrada*/
	//	Buffer_port[j++]=*(buffer_S1_B2+2)+0x030;				/*dia de entrada*/
	//	Buffer_port[j++]=*(buffer_S1_B2+3)+0x030;				/*hora de entrada*/
	//	Buffer_port[j++]=*(buffer_S1_B2+4)+0x030;				/*minutos de entrada*/
	
//		Buffer_port[j++]='.';														/*separador tipo de vehiculo*/
																										/*MF_TIPO_VEHICULO	0x08							tipo vehiculo 00 carro, 01 moto, 02 bicicleta, 04 tractomula*/
		
	//	if(*(buffer_S1_B2+8)!=0)
		//{
			//Buffer_port[j++]='M';													/*moto*/
		//}
		//else
		//{
		//	Buffer_port[j++]='C';													/*carro*/
		//}
		//Buffer_port[j++]=ETX;	
		//while(busy==0);
		//send_port(Buffer_port,j);													/*trama transmitida pto paralelo*/
		
	
//}

/*-------------------------------------------------------------------------------------------
Funcion q arma la trama a transmitir pto paralelo trama g
-------------------------------------------------------------------------------------------*/	
/*TEMPORARMENTE FUERA DE SERVICIO POR EXPEDIDOR TICKET*/
//void Trama_pto_Paralelo_new(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd)
//{
//	unsigned char Buffer_port[30];
//	unsigned char j=2;
	
//	Buffer_port[0]=STX;																/*inicio de cmd STx*/
//	Buffer_port[1]=cmd;																/*cmd*/

//	do
//	{
//	 Buffer_port[j++]=*buffer_S1_B0;									/*ticket*/
//		buffer_S1_B0++;
//	}while (*buffer_S1_B0!=0);
	
//	if(*(buffer_S1_B2+5)!=0)													/*MF_DCTO						0x05				Tipo de descuento (00)sin descuento, (01xx xxxx) 0x40 fija fecha de salida,
//																										10xx xxxx dcto por porcentaje xx xxxx= valor del porcentaje, 11xx xxxx dcto por dinero */
//	{
	//	Buffer_port[j++]='-';														/*separador del descuento*/
//		Buffer_port[j++]=*(buffer_S1_B2+5)+0x30;				/*descuento pasado a ascii*/
//	}
//	else
//	{
//		Buffer_port[j++]='-';														/*separador del descuento*/
//		Buffer_port[j++]='0';
//	}
	
//	if((*(buffer_S1_B2+9)&0x0f)==0x0f)									/*MF_IN_PAGO=0x09				si es =0x0f fue liquidado en cajero 	*/	
//	{
//		Buffer_port[1]='C';
//	}
//	else
//	{
//		Buffer_port[1]='s';
//	}
	
//		Buffer_port[j++]='-';														/*separador de la fecha de entrada*/
//		Buffer_port[j++]=*(buffer_S1_B2+0)+0x030;				/*año de entrada*/
//		Buffer_port[j++]=*(buffer_S1_B2+1)+0x030;				/*mes de entrada*/
//		Buffer_port[j++]=*(buffer_S1_B2+2)+0x030;				/*dia de entrada*/
//		Buffer_port[j++]=*(buffer_S1_B2+3)+0x030;				/*hora de entrada*/
//		Buffer_port[j++]=*(buffer_S1_B2+4)+0x030;				/*minutos de entrada*/
//		Buffer_port[j++]='-';	
		
//		Block_read_Clock_Hex(Buffer_port+j);
		
//		Buffer_port[j++]=Buffer_port[j]+ 0x30;
//		Buffer_port[j++]=Buffer_port[j]+ 0x30;
//		Buffer_port[j++]=Buffer_port[j]+ 0x30;
//		Buffer_port[j++]=Buffer_port[j]+ 0x30;
//		Buffer_port[j++]=Buffer_port[j]+ 0x30;
		
		
	
//		Buffer_port[j++]='.';														/*separador tipo de vehiculo*/
																										/*MF_TIPO_VEHICULO	0x08							tipo vehiculo 00 carro, 01 moto, 02 bicicleta, 04 tractomula*/
		
//		if(*(buffer_S1_B2+8)!=0)
//		{
//			Buffer_port[j++]='M';													/*moto*/
	//	}
	//	else
//		{
//			Buffer_port[j++]='C';													/*carro*/
//		}
//		Buffer_port[j++]=ETX;	
	
		
//		ready=0;
//			while(busy==0);
//		send_port(Buffer_port,j);													/*trama transmitida pto paralelo*/
		
//			Debug_Dividir_texto();																							/*division del texto */
							
//			DebugBufferMF(Buffer_port,j,1);		
//			Debug_Dividir_texto();	
//}

/*-------------------------------------------------------------------------------------------
Funcion q arma la trama a transmitir pto paralelo trama de salida codigo qr o barras
-------------------------------------------------------------------------------------------*/	

void Trama_print_cod_barras(unsigned char *msj,unsigned char vehiculo)
{
	unsigned char temp[6];
	unsigned char Buffer_port[30];
	unsigned char j=2;

	Buffer_port[0]=STX;																/*inicio de cmd STx*/
	Buffer_port[1]='s';																/*cmd*/
	Buffer_port[2]=0x00;															/*finalizo la trama para sumarla con la otra trama*/
	j=strlen(msj);																		/*longitud de la trama*/
	strncat(Buffer_port,msj,j); 											/*une los dos strim copio el Ticket mes el caracter del enter*/
  strcat(Buffer_port,"-0s-");												/*sin descuento y liquidacion*/
	/*fecha actual*/
	/*año*/
	temp[0]=(bcd_hex(lee_clk(RANO)))+0x30;	
	/*mes*/
	temp[1]=(bcd_hex(lee_clk(RMES)))+0x30;	
 	/*Dia*/
	temp[2]=(bcd_hex(lee_clk(RDIA)))+0x30;
 	/*hora*/	
	temp[3]=(bcd_hex(lee_clk(RHORA)))+0x30;
  /*minutos*/	
	temp[4]=(bcd_hex(lee_clk(RMIN)))+0x30;
	temp[5]=0;
  strcat(Buffer_port,temp);												/*sumo la fecha actual*/
	strcat(Buffer_port,"-11111.");									/*sumo fecha de salida y tipo de vehiculo*/	
	temp[0]=vehiculo;
	temp[1]=0;
	strcat(Buffer_port,temp);
	j=strlen(Buffer_port);
	//Buffer_port[j]=vehiculo;	
	//j++;
	Buffer_port[j]=ETX;															/*le sumo el fin de la trama*/
/*debuger trama */	
	ready=0;
	while(busy==0);
	send_port(Buffer_port,j+1);													/*trama transmitida pto paralelo*/
		
	Debug_Dividir_texto();																							/*division del texto */
	DebugBufferMF(Buffer_port,j+1,1);		
	Debug_Dividir_texto();	

}
/*-------------------------------------------------------------------------------------------
Funcion q arma la trama a transmitir pto paralelo trama de salida codigo qr 
-------------------------------------------------------------------------------------------*/	
/*
void Trama_print_cod_QR(unsigned char *msj)
{
	unsigned char temp[6];
	unsigned char Buffer_port[30];
	unsigned char j=2;

	Buffer_port[0]=STX;																/*inicio de cmd STx*/
//	Buffer_port[1]='g';																/*cmd*/
//	Buffer_port[2]=0x00;															/*finalizo la trama para sumarla con la otra trama*/	
	
	
//}
/*--------------------------------------------------------------------------------------------------------
la trama 02,a,No ticket,,tipo vehiculo,,fecha_int,10 ceros o fecha maxima de salida, placa, etx
---------------------------------------------------------------------------------------------------------*/
//void Trama_pto_paralelo_printer(unsigned char cmd)
//{
//	unsigned char Buffer_port[30];
//	unsigned char serie[10];
//	unsigned char j=2;
//	unsigned char t,p,temp=0;
//	Buffer_port[0]=STX;																		/*inicio de cmd STx*/
//	Buffer_port[1]=cmd;																		/*cmd*/
	
	
//	serie_ascii_siceros_l(serie);													/*numero de ticket*/
//	p=strlen(serie);																			/*longitud del ticket*/
//	t=0;
//	j=2;
//	do
//	{
//	 Buffer_port[j++]=serie[t];														/*ticket o consecutivo almacenado en el buffer*/
//		p--;
//		t++;
//	}while (p!=0);	
			
//	Buffer_port[j++]='-';
//	temp=j;
//	Block_read_Clock_Hex(Buffer_port+j);									 /*fecha de entrada*/
//	j=temp;
//	Buffer_port[j++]=Buffer_port[j]+ 0x30;
//	Buffer_port[j++]=Buffer_port[j]+ 0x30;
//	Buffer_port[j++]=Buffer_port[j]+ 0x30;
//	Buffer_port[j++]=Buffer_port[j]+ 0x30;
//	Buffer_port[j++]=Buffer_port[j]+ 0x30;

//	Buffer_port[j++]='-';
		
//	p=strlen(placa);																				/*longitud de la placa*/
	
//	t=0;
//	if(p!=0)
//	{		
//		do
//		{
//		Buffer_port[j++]=placa[t];															/*almaceno la placa en el buffer*/
//		p--;
//		t++;
//		}while (p!=0);
//	}
//	else
//	{
//		Buffer_port[j++]=' ';																		/*no hay placa*/
//	}

//	Buffer_port[j++]=ETX;																			/*final de la trama*/
	
//	ready=0;
//	while(busy==0);
//	send_port(Buffer_port,j);																/*trama transmitida pto paralelo*/
		
//	Debug_Dividir_texto();																	/*division del texto */
							
//	DebugBufferMF(Buffer_port,j,1);													/*debuger de la trama*/
//	Debug_Dividir_texto();		
//}

/*-------------------------------------------------------------------------------------------
Reloj del pto paralelo
-------------------------------------------------------------------------------------------*/
void Pulso_Bus(void)
{
	port_clk=0;
	Delay_20us(40);									//33
	port_clk=1;
	Delay_20us(40);									/*wait_ancho*/
}
/*-------------------------------------------------------------------------------------------
Envia msj de error visualizar en el lcd del micro principal
-------------------------------------------------------------------------------------------*/
void send_portERR(unsigned char cod_err)
{
  unsigned char timeOut;
	long int cont;
	
	port_clk=1;	 																	/*Reloj del pto paralelo El que transmite debe fijar primero el Clk en 1*/
	rx_in_data=0;	 																//Led de visualizacion 	ON
	timeOut=0;
 	cont=15000;
	
	P2=cod_err;
	ready=0;																			//Genera interrupcion al Principal
 	while ((busy==1)&&(timeOut==0)) 							//Espera reconocimiento INT por entrada busy
	{
		cont--;
		if (cont==0)
		{
			timeOut=1;
		}
	}
	if ((timeOut==0)&&(busy==0))
	{
   			P2=cod_err;
			Pulso_Bus();
	}
 	ready=1;
	port_clk=1;
	rx_in_data=1;																		//Led de visualizacion 	OFF

	P2=0XFF;

 	Delay_20us(98);	
	Delay_20us(98);	
	
}
/*-------------------------------------------------------------------------------------------
Condiciones de Inicio del pto paralelo
-------------------------------------------------------------------------------------------*/
void cond_ini_pto()
{	
	
	port_clk=1;
	ready=1;
	rx_in_data=1;
	P2=0xff;
}
/*------------------------------------------------------------------------------
Funcion q carga el reloj y lo envia por el pto paralelo
msj informativo con el pulsador funcion 
------------------------------------------------------------------------------*/
void load_and_send_info_reloj()
{
	unsigned char buffer_info[11];
	
		buffer_info[0]=STX; 									/*STX*/
		
		buffer_info[1]='i';										/*se envia el cmd*/
		
		Block_read_Clock(buffer_info+2);			/*año, mes,dia, hora, minutos, segundos*/
		buffer_info[8]=lee_clk(0x8b); 				//Dia semana
		buffer_info[9]=ETX; 									//ETX

   

		send_port(buffer_info,10);
}

/*------------------------------------------------------------------------------
procedimiento que transmite por el pto paralelo ID_cliente,COD_park
con el pulsador funcion
------------------------------------------------------------------------------*/ 
void load_and_send_id_cod()
{
unsigned char buffer_info[11];
unsigned char buf[4];
unsigned char i,k;
k=0;

		buffer_info[0]=STX; 											//STX
		buffer_info[1]='D';												// nombre del comando de id_cliente, cod_park
		sprintf(buf,"%d",ID_CLIENTE);							/*ID_CLIENTE lo paso a strim */
		for(i=2;;i++)
		{
			if(buf[k] !=0)													/*alamaceno id del clinete en el buffer*/
			{
			buffer_info[i]=buf[k] ;									/*alamaceno el dato en el buffer*/
			k++;
			}
				else {buffer_info[i]=';'; i++; break;}	/*le pongo un  separador */
				
		 }
	
	
		 sprintf(buf,"%d",COD_PARK);								/*cod_park lo paso a strim*/
		
		for(k=0;;i++)
		{
			if(buf[k] !=0)
			{
			buffer_info[i]=buf[k] ;										/*lo almaceno en el  buffer*/
			k++;
			}else {buffer_info[i]=';'; i++; break;}
		 }
	
	
	    buffer_info[i]=ETX; 										//ETX
			send_port(buffer_info,i);									/*se transmite informacion pt0 paralelo*/
}