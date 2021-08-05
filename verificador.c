/*
		FUNCIONES PARA DISPENSADOR																*
*/
#include<verificador.h>
#include <reg51.h>

/*funciones prototipo externas */

extern void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena);
extern void EscribirCadenaSoft(unsigned char tamano_cadena,unsigned char tipo);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_txt_Tibbo(unsigned char * str);
extern unsigned char  ValidaSensoresPaso(void);
extern void send_portERR(unsigned char cod_err);
extern void Debug_chr_Tibbo(unsigned char Dat);
extern void Debug_HexDec(unsigned char xfc);
extern char check_fechaOut(char *buffer);
extern unsigned char Dir_board();
extern void PantallaLCD(unsigned char cod_msg);
extern void Trama_pto_Paralelo_C_s(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
extern void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
extern void Trama_pto_Paralelo(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void Trama_pto_Paralelo_P(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void analiza_tiempo(char *buffer,unsigned int Val_DctoMinutos);
extern void Trama_pto_Paralelo_new(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void Block_read_Clock_Hex(unsigned char *datos_clock);

sbit lock = P1^7;						//Relevo 	
sbit Atascado = P0^3;				//Rele de on/off del verificador o transporte
sbit led_err_imp = P0^2;			//Error 	
/*variables externas*/

extern unsigned char g_cEstadoComSoft;
extern unsigned char ValTimeOutCom;
extern unsigned char g_cEstadoComSeqMF;
extern unsigned char g_cContByteRx;
extern unsigned char xdata Buffer_Rta_Lintech[];
extern int ID_CLIENTE;
extern int COD_PARK;
extern unsigned int T_GRACIA;																				/*tiempo de gracia del parqueo*/
extern unsigned char Timer_wait;
extern unsigned int  SIN_COBRO;
/*externo bit*/

extern bit buffer_ready;
extern unsigned char USE_LPR;

/*----------------------------------------------------------------------------
Definiciones de sequencias de verificador y expedidor
------------------------------------------------------------------------------*/

#define SEQ_INICIO						0X00	
#define SEQ_CARD_INSERCION		0X01
#define SEQ_CHECK_STATUS			0X02
#define SEQ_RTA_CARD_POS			0x03
#define SEQ_RTA_CAPTURE				0X04
#define SEQ_REQUEST						0x05
#define SEQ_MF_LINTECH				0x06
#define SEQ_RD_S1B1		 				0x07
#define SEQ_RD_S1B2 					0x08
#define SEQ_RD_S1B0 					0x09
#define SEQ_WR_S1B2						0x0a
#define SEQ_RTA_S2B0					0x0b
#define SEQ_RD_S1B0_EJECT			0x0c

#define SEQ_EXPULSAR_TARJ			0x20
#define SEQ_EXPULSAR_CHECK 		0x21
#define SEQ_EXPULSAR 					0x22
#define SEQ_EXPULSAR_FROM			0x23


/*----------------------------------------------------------------------------
 definiciones de lintech en la inicializacion de expedidor o verificador
------------------------------------------------------------------------------*/

#define INICIA_LINTECH					0x30
#define	SEQ_CAPTURE_DATOS_INI		0x31
#define GRABA_EEPROM						0x32
#define SEQ_CAPTURA_OK_EEPROM		0X33
#define FIN_OK									0x00

/*----------------------------------------------------------------------------
 definiciones de lintech en el comando Check_Status
------------------------------------------------------------------------------*/

#define	S_DETAIL				0x31
#define S_NORMAL				0x30

/*------------------------------------------------------------------------------
 definiciones de lintech en el comando Card_Insercion
------------------------------------------------------------------------------*/

#define Habilita			 	0x30
#define Inhabilita			0x31

/*------------------------------------------------------------------------------
Definicion de Lintech en el comando Inicializa
------------------------------------------------------------------------------*/

#define	TO_FRONT				'0'
#define	CAPTURE_BOX			'1'
#define	SIN_MOVIMIENTO	'3'

/*------------------------------------------------------------------------------
Definicion de Lintech en el comando mover tarjeta (Mov_Card)
------------------------------------------------------------------------------*/

#define 	MovPos_Front				'0'		
#define 	MovPos_IC						'1'
#define  	MovPos_RF						'2'
#define  	MovPos_Capture			'3'
#define 	MovPos_EjectFront		'9'

/*------------------------------------------------------------------------------
Definicion de la trama Lintech de las respuestas de los cmd
------------------------------------------------------------------------------*/

#define Pos_TipoResp				4
#define Pos_Length					3
#define Pos_St0							7
#define Pos_St1							8
#define Pos_St2							9
#define Pos_IniDatMF				0x0a
#define	Card_type_H					0x0a
#define	Card_type_L					0x0b

/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 

/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					100		//50


/*----------------------------------------------------------------------------
definicion de datos de trama lintech
------------------------------------------------------------------------------*/

#define 	ETX								03
#define 	STX_LINTECH				0xf2

/*----------------------------------------------------------------------------
msj de lcd tarjeta y lcd serie
------------------------------------------------------------------------------*/

#define ERROR_LOOP							0XE0
#define TARJETA_INVALIDA				0XE1
#define TARJETA_SIN_FORMATO	    0xDF
#define ERROR_COD_PARK					0XE5
#define SIN_INGRESO							0XE6
#define SIN_PAGO								0XE7
#define EXCEDE_GRACIA						0XE8

/*----------------------------------------------------------------------------
definiciones para, el debuger. saber si la trama es enviada, o la trama es de respuesta
------------------------------------------------------------------------------*/

#define 	ENVIADOS					0X0
#define		RESPUESTA					0X01

/*----------------------------------------------------------------------------
definiciones de la tarjeta MF tipo de cliente esto esta en la posicion (0) de la memoria MF
(0) si el dato es cero esta inactiva
(1) activa o ROTACION

------------------------------------------------------------------------------*/

#define INACTIVA					0x00
#define ROTACION 					0x01

/*----------------------------------------------------------------------------
posicion de  MF  bloque 1 sector 1
(0) tipo de tarjeta 
(01) el id del cliente
(03)codigo del parqueadero	
------------------------------------------------------------------------------*/
#define 	MF_TIPO_TARJETA		0X00
#define 	MF_ID_CLIENTE			0x01
#define		MF_COD_PARK				0x03

/*----------------------------------------------------------------------------
posicion de  MF bloque 2 sector 1
(00) donde esta grabado la fecha de entrada (año,mes,dia,hora,minutos) estan en hex  
(0b) donde esta grabado la fecha de salida (año,mes,dia,hora,minutos) estan en hex 
------------------------------------------------------------------------------*/

#define 	MF_FECHA_INT			0X00				/*año,mes,dia,hora,minutos*/


#define		MF_DCTO						0x05				/*Tipo de descuento (00)sin descuento, (01xx xxxx) 0x40 fija fecha de salida,
																					10xx xxxx dcto por porcentaje xx xxxx= valor del porcentaje, 11xx xxxx dcto por dinero */
#define		MF_LSB						0x06

#define   MF_TIPO_VEHICULO	0x08							/*tipo vehiculo 00 carro, 01 moto, 02 bicicleta, 04 tractomula*/

#define		MF_IN_PAGO				0x09
#define 	MF_APB						0x0A						/*antipassback 00 inicializado, 01 IN, 02 OUT, 03 NO USA*/

#define		MF_FECHA_OUT			0X0B				/*año,mes,dia,hora,minutos*/

/*----------------------------------------------------------------------------
Definicion de varaibles globales del objeto
------------------------------------------------------------------------------*/

static unsigned char Estado=INICIA_LINTECH;


/*----------------------------------------------------------------------------
funcion de inicializacion del transporte

------------------------------------------------------------------------------*/

void Inicializa(unsigned char TipoMovimiento)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	bcc=0;
	if ((TipoMovimiento==SIN_MOVIMIENTO)||(TipoMovimiento==TO_FRONT)||(TipoMovimiento==CAPTURE_BOX))
	{
		
		Debug_txt_Tibbo((unsigned char *) "Incializa Dispensador\r\n\r\n");
		
		g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]='0';
		g_scArrTxComSoft[6]=TipoMovimiento;
		g_scArrTxComSoft[7]=ETX;
		for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,9,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	}
}

/*------------------------------------------------------------------------------
cmd de lintech que responde en que estado de los sensores se encuentra

(30) solo envia el resumen de los sensores
(31) da un reporte detallado de los sensores
------------------------------------------------------------------------------*/

void Check_Status(unsigned char Detalle)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	Debug_txt_Tibbo((unsigned char *) "Check_Status\r\n\r\n");

	bcc=0;

	g_scArrTxComSoft[0]=STX_LINTECH;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X03;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]='1';
	g_scArrTxComSoft[6]=Detalle;
	g_scArrTxComSoft[7]=ETX;
	for (j=0; j<8; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}
	g_scArrTxComSoft[8]=bcc;
	buffer_ready=0;
	g_cEstadoComSoft=ESPERA_RX;
	DebugBufferMF(g_scArrTxComSoft,9,0);
	EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
	ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
Procedimiento que habilita la insercion o inhabilita la insersion
(31) inhabilita
(30) habilita
------------------------------------------------------------------------------*/

void Card_Insercion(char Tipo)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	if (Tipo==Habilita)
	{
		Debug_txt_Tibbo((unsigned char *) "Habilita Insersion\r\n\r\n");
		g_scArrTxComSoft[6]=Habilita;
	}
	else
	{
		
		Debug_txt_Tibbo((unsigned char *) "Inhabilita Insersion\r\n\r\n");
		g_scArrTxComSoft[6]=Inhabilita;
	
	}

	bcc=0;

	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X03;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]='3';

	g_scArrTxComSoft[7]=ETX;
	for (j=0; j<8; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}

	g_scArrTxComSoft[8]=bcc;

	buffer_ready=0;
	g_cEstadoComSoft=ESPERA_RX;
	DebugBufferMF(g_scArrTxComSoft,9,0);
	EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
	ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
CMD q mueve la tarjeta 
------------------------------------------------------------------------------*/

void Mov_Card(unsigned char Posicion)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	bcc=0;

 	if ((Posicion==MovPos_RF)||(Posicion==MovPos_IC)||(Posicion==MovPos_Front)||(Posicion==MovPos_EjectFront)||(Posicion==MovPos_Capture))
	{
	 	if (Posicion==MovPos_RF)
		{
		 	Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a RF\r\n\r\n");
		}
		else if (Posicion==MovPos_IC)
		{
			Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a IC\r\n\r\n");
   		}
		else if (Posicion==MovPos_Front)
		{
			Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a Bezel\r\n\r\n");
 		}
		else if (Posicion==MovPos_EjectFront)
		{
			Debug_txt_Tibbo((unsigned char *) "Expulsando Tarjeta\r\n\r\n");
		}
		else if (Posicion==MovPos_Capture)
		{
			Debug_txt_Tibbo((unsigned char *) "Capturando Tarjeta\r\n\r\n");
 		}

		g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]='2';
		g_scArrTxComSoft[6]=Posicion;
		g_scArrTxComSoft[7]=ETX;
		for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;
		g_cEstadoComSoft=ESPERA_RX;
		DebugBufferMF(g_scArrTxComSoft,9,0);
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
		ValTimeOutCom=TIME_CARD;
	}

}

//*******************************************************************************************
// rutina q mira el tipo de tarjeta si es valido para el uso 	
//*******************************************************************************************

void Aut_Card_check_Status(void)
{
unsigned char j, bcc;
unsigned char	g_scArrTxComSoft[10];
		  bcc=0;
	
	Debug_txt_Tibbo((unsigned char *) "Aut_Card_check_Status\r\n\r\n");
 	
	g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]=0x50;
		g_scArrTxComSoft[6]=0x31;
		g_scArrTxComSoft[7]=ETX;
	   	for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,9,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	}	

/*------------------------------------------------------------------------------
CMD q programa la clave en el verificador o transporte
------------------------------------------------------------------------------*/

	void Dwload_EEprom (void)
{
 	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[21];
	bcc=0;
	Debug_txt_Tibbo((unsigned char *) "Download MF EEprom\r\n\r\n");
	
	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X0E;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0x60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0Xd0;
	g_scArrTxComSoft[9]=0X00;
	g_scArrTxComSoft[10]=0X01;
	g_scArrTxComSoft[11]=0x06;
	g_scArrTxComSoft[12]='3';
	g_scArrTxComSoft[13]='V';
 	g_scArrTxComSoft[14]='0';
	g_scArrTxComSoft[15]='p';
	g_scArrTxComSoft[16]='4';
	g_scArrTxComSoft[17]='r';
	g_scArrTxComSoft[18]=ETX;
	
 	for (j=0; j<19; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[19]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,20,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,20);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
Funcion q verifica si la clave y la carga en el transporte
------------------------------------------------------------------------------*/

void LoadVerify_EEprom(void)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[15];
	bcc=0;
	Debug_txt_Tibbo((unsigned char *) "Carga y Verifica de EEprom\r\n\r\n");


	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X07;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0x60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0x21;
   	g_scArrTxComSoft[9]=0x00;
	g_scArrTxComSoft[10]=0x01;
	g_scArrTxComSoft[11]=ETX;
	g_scArrTxComSoft[12]=0xc6;

	

	for (j=0; j<13; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[13]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,14,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,14);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
		
}

/*------------------------------------------------------------------------------
Funcion q lee la MF dandole el sector y el bloque
------------------------------------------------------------------------------*/

void RD_MF(unsigned char Sector, unsigned char Bloque)
{

	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[15];
	bcc=0;

	Debug_txt_Tibbo((unsigned char *) "Leyendo MF > Sector: ");
	Debug_chr_Tibbo(Sector);
	
	Debug_txt_Tibbo((unsigned char *) " Bloque: ");
	Debug_chr_Tibbo(Bloque);
	Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");


	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;									  
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X08;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0X60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0xb0;
	g_scArrTxComSoft[9]=Sector;
	g_scArrTxComSoft[10]=Bloque;
	g_scArrTxComSoft[11]=0x01;
	g_scArrTxComSoft[12]=ETX;

	for (j=0; j<13; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}
	g_scArrTxComSoft[13]=bcc;


		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,14,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,14);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;

		
}

/*------------------------------------------------------------------------------
Funcion q lescribe la MF dandole el sector y el bloque y los datos
------------------------------------------------------------------------------*/


void WR_MF(unsigned char Sector, unsigned char Bloque,unsigned char *buffer)	   
	 
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[31];
	bcc=0;

	Debug_txt_Tibbo((unsigned char *) "Escribe MF > Sector: ");
	Debug_chr_Tibbo(Sector);
	
	Debug_txt_Tibbo((unsigned char *) " Bloque: ");
	Debug_chr_Tibbo(Bloque);
	Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");
																	  
	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;									  
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=24;																	/* Numero Datos a programar */ 
	
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0X60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0xd1;
	g_scArrTxComSoft[9]=Sector;															//Sector;
	g_scArrTxComSoft[10]=Bloque;														//Bloque;
	g_scArrTxComSoft[11]=0x01;

	if(Sector!=2)
	{	
/*------------------------------------------------------------------------------
borro la fecha de entrada de entrada 
------------------------------------------------------------------------------*/

  g_scArrTxComSoft[12]=0x00;															/*borro la fecha de entrada año mes dia hora minutos*/
	g_scArrTxComSoft[13]=0x00;
	g_scArrTxComSoft[14]=0x00;
	g_scArrTxComSoft[15]=0x00;
	g_scArrTxComSoft[16]=0x00;

/*------------------------------------------------------------------------------
borro los descuentos
------------------------------------------------------------------------------*/
	g_scArrTxComSoft[17]=0x00;
	g_scArrTxComSoft[18]=0x00;
	g_scArrTxComSoft[19]=0x00;
	
/*------------------------------------------------------------------------------
tipo de vehiculo
------------------------------------------------------------------------------*/
	
	g_scArrTxComSoft[20]=*buffer;
	
/*------------------------------------------------------------------------------
direccion de BOArd_pcb de salida o puerta de salida
------------------------------------------------------------------------------*/	
	g_scArrTxComSoft[21]=0x0f&Dir_board();
	
/*------------------------------------------------------------------------------
programo el APB como salida (02) entrada(01)
------------------------------------------------------------------------------*/
g_scArrTxComSoft[22]=02;

/*------------------------------------------------------------------------------
borro la fecha de salida 
------------------------------------------------------------------------------*/

  g_scArrTxComSoft[23]=0x00;															/*borro la fecha de entrada año mes dia hora minutos*/
	g_scArrTxComSoft[24]=0x00;
	g_scArrTxComSoft[25]=0x00;
	g_scArrTxComSoft[26]=0x00;
	g_scArrTxComSoft[27]=0x00;

	}
		else 
		{
			for (j=12; j<=28; j++)
			{
				g_scArrTxComSoft[j]=0x00;	
			}
		}




	g_scArrTxComSoft[28]=ETX;
	
	for (j=0; j<=28; j++)
	{
		bcc=bcc^g_scArrTxComSoft[j];
	}
	g_scArrTxComSoft[29]=bcc;

	
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,30,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,30);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	
}

/*------------------------------------------------------------------------------
funcion que valida la trama del verificador o transporte lintech
(0) significa que la trama es valida y sigue en el proceso
(1)	falla en la respuesta por pto serie o trama invalida
(2)	no ha recibido la trama del verificador o transporte
(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/

char Trama_Validacion_P_N()
{
	char Trama_Validacion_P_N=2;
	
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if (buffer_ready==1)
				{
					buffer_ready=0;
						
				
					if (Buffer_Rta_Lintech[Pos_TipoResp]=='P')
					{
										
						Trama_Validacion_P_N=0;
							
					}
					else if (Buffer_Rta_Lintech[Pos_TipoResp]=='N')
					{
 						
						Trama_Validacion_P_N=3;
						
					}
					else
					{
					 	Debug_txt_Tibbo((unsigned char *) "Respuesta  DESCONOCIDA \r\n");				/*la respuesta es desconocida*/
						Trama_Validacion_P_N=3;		
						
					}
				}
				else
				{
			 							
					Trama_Validacion_P_N=1;																									/*pto serie no responde */
																						
 				}

			}
			
	return Trama_Validacion_P_N;
}

/*------------------------------------------------------------------------------
Secuencia de los cmd de inicio (reset dispositivo y graba eeprom)

funcion  de los cmd de inicio (reset dispositivo y graba eeprom) retorna un (00) cuando a terminado exitoso
------------------------------------------------------------------------------*/

unsigned char  Secuencia_inicio_expedidor(void)
{
	char temp;
	switch (Estado)
	{
		case INICIA_LINTECH:

					
				Inicializa(SIN_MOVIMIENTO);	   																		//Inicio el transporte sin movimiento
				Estado=SEQ_CAPTURE_DATOS_INI;																			// entra a validar la respuesta del transporte
			  
 			
			return (Estado);
		break;


/*------------------------------------------------------------------------------
Envia la primera trama de inicializacion del transporte sin movimiento
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/
			
		case SEQ_CAPTURE_DATOS_INI:

			if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			Estado=SEQ_CAPTURE_DATOS_INI;																													/*(2)no ha respondido*/
			}	
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "INICIALIZA SIN MOVIMIENTO ERROR\r\n\r\n");					/* trama no valida*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			Estado=INICIA_LINTECH;																																/* (3) Trama invalida cmd (N)*/	
			}
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				Estado=INICIA_LINTECH;																															/*(1) no responde el pto*/	
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "INICIALIZA SIN MOVIMIENTO OK\r\n\r\n");						/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			Estado=GRABA_EEPROM;																																	/*(0) respuesta ok*/
		}			
		return (Estado);
 		break;
			
/*------------------------------------------------------------------------------
Envia el cmd de grabar claves a la eeprom del transporte 
------------------------------------------------------------------------------*/
			
			case GRABA_EEPROM:
				
				Dwload_EEprom();	   																																//envio el cmd de grabar la eeprom
				Estado=SEQ_CAPTURA_OK_EEPROM;																												// entra a validar la respuesta del transporte
			
 			

			return (Estado);
				break;
	
/*------------------------------------------------------------------------------
se analiza la respuesta 
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/
			
		case SEQ_CAPTURA_OK_EEPROM:
		
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			Estado=SEQ_CAPTURA_OK_EEPROM;																													/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "DWLOAP EEPROM ERROR\r\n\r\n");											/* trama no valida respuesta incorrecta falla en la escritura de la clave*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			Estado=INICIA_LINTECH;																																/* (3) Trama invalida cmd (N)*/	
			}			
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				Estado=INICIA_LINTECH;																															/*NO RESPONDE PTO SERIE */
			}				
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "DWLOAP EEPROM OK\r\n\r\n");												/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			Estado=FIN_OK;																																				/*respuesta ok clave grabada con exito*/
		}			

		return (Estado);
 		break;

/*------------------------------------------------------------------------------
Fin de la secuencia de comandos con exito 
------------------------------------------------------------------------------*/		
			
				case FIN_OK:

				return (Estado);
		break;
			
					
				default:
				return Estado=INICIA_LINTECH;	
				break;
	}
}

/*------------------------------------------------------------------------------
Prosedimiento  q hace paso a paso la secuencia de la MF
------------------------------------------------------------------------------*/

void SecuenciaExpedidor(void)
{
	unsigned char temp;
	static unsigned char buffer_S1_B0[17];
	static unsigned char buffer_S1_B1[17];
	static unsigned char buffer_S1_B2[17];
	static unsigned char error_rx=0; 
	unsigned char clock_temp[6];
	switch (g_cEstadoComSeqMF)
	{
//***********************************************************************************************
		
		case SEQ_INICIO:

			if (ValTimeOutCom==1)
			{

				Card_Insercion(Habilita);																													/* se envia el cmd de inicio insercion por boca*/
				g_cEstadoComSeqMF=SEQ_CARD_INSERCION;																							// entra a validar la respuesta del transporte
			  
 			}
 
 		break;

/*------------------------------------------------------------------------------
Analizo la trama de insercion por boca
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/			
		
		case SEQ_CARD_INSERCION:	
			
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_CARD_INSERCION;																									/*no ha respondido*/
			}	
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "CARD_INSERCION ERROR\r\n\r\n");										/* trama no valida respuesta incorrecta falla en la escritura */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																													/* (3) Trama invalida cmd (N)*/	
			}			
			else
			{
			Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				if (error_rx>= 3)
				{
					error_rx=0;
					Atascado=1;																																						/*apago el verificador*/
					led_err_imp=0;																																				/*error led on*/
					Timer_wait=0;
					ValTimeOutCom=3;				
					g_cEstadoComSeqMF=SEQ_INICIO;			//SEQ_CHECK_STATUS;
						break;																																							/* respuesta incorrecta*/	
				}			

				error_rx++;
				g_cEstadoComSeqMF=SEQ_INICIO;																												/* respuesta incorrecta*/	
			}				
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "CARD_INSERCION OK\r\n\r\n");												/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																										/*respuesta ok*/
		}			
			
		break;
			
/*------------------------------------------------------------------------------
chequeo el estado de los sensores 
------------------------------------------------------------------------------*/			
		
		case SEQ_CHECK_STATUS:	
								
				Check_Status(S_NORMAL);																																/* envio el cmd de pregunta en q estado estan los sensores*/
				g_cEstadoComSeqMF=SEQ_RTA_CARD_POS;																										// entra a validar la respuesta del transporte
			 
			break;			
				
/*------------------------------------------------------------------------------
Valido la informacion de los sensores
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/		

		case SEQ_RTA_CARD_POS:
			
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RTA_CARD_POS;																												/*no ha respondido*/
			}	
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_CARD_INSERCION  FALLO\r\n\r\n");											/* trama no valida*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,1);																					/*imprimo la trama recibida*/
			g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																													/* (3) Trama invalida cmd (N)*/	
			}			
			else
			{
			Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				if (error_rx>= 3)
				{
					error_rx=0;
					Atascado=1;																																						/*apago el verificador*/
					led_err_imp=0;																																				/*error led on*/
					Timer_wait=0;
					ValTimeOutCom=3;				
					g_cEstadoComSeqMF=SEQ_INICIO;			//SEQ_CHECK_STATUS;
						break;																																							/* respuesta incorrecta*/	
				}	
				error_rx++;
				g_cEstadoComSeqMF=SEQ_CHECK_STATUS;
			}			
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "CHECK SATUS OK\r\n\r\n");																	/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																		/*imprimo la trama recibida*/
				if (Buffer_Rta_Lintech[Pos_St0]=='0')																												/* CANAL LIBRE	  no tiene tarjetas en el mecanismo*/
						{
							
							Debug_txt_Tibbo((unsigned char *) "No tiene tarjetas en el mecanismo\r\n\r\n");				/* trama valida Habilitado insercion por boca*/
							g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																										/*volvemos a preguntar	*/					

							
						}
						else if	(Buffer_Rta_Lintech[Pos_St0]=='1')	 																						/*  se detecta la tarjeta en la boca TARJETA EN BEZZEL*/
						{
							
							Debug_txt_Tibbo((unsigned char *) "Tarjeta en la boca\r\n\r\n");											/* trama valida Habilitado insercion por boca*/
							g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																										/*volvemos a preguntar	*/			
																	   
						}
						else if (Buffer_Rta_Lintech[Pos_St0]=='2') 																						// TARJETA EN RF
						{
							Debug_txt_Tibbo((unsigned char *) "Tarjeta lista para leer en RF\r\n\r\n");							/* trama valida Habilitado insercion por boca*/
													
							error_rx=0;
							if (ValidaSensoresPaso()!=0)																												/*pregunto q alla presencia vehicular*/
							{
							Debug_txt_Tibbo((unsigned char *) "Vehiculo en el loop\r\n\r\n");										/* se encuentra un sensor activo*/
							Aut_Card_check_Status();																														/* se pregunta el tipo de tarjeta si es MF50*/
							g_cEstadoComSeqMF=SEQ_REQUEST;																											/*vamos al siguiente paso a con validar el cmd*/		
							}
							else
							{
								Debug_txt_Tibbo((unsigned char *) "Vehiculo no en el loop\r\n\r\n");							/* no hay vehiculo en el loop*/
								send_portERR(ERROR_LOOP);																													/*envio msj al lcd tarjeta*/
								PantallaLCD(ERROR_LOOP);																													/*envio el msj por la pantalla lcd o la raspberry*/
								g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																								/*volvemos a preguntar	*/	
								
							}
																														
						}
																															
		}			
			
		break;
	
/*------------------------------------------------------------------------------
El vehiculo no se encuentra en el loop  ejecto la tarjeta y espero la respuesta
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/	

	case SEQ_RTA_CAPTURE:
		
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
					
			g_cEstadoComSeqMF=SEQ_RTA_CAPTURE;																										/*no ha respondido*/
		
			}	
				else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "NO EJECT TARJET ERROR\r\n\r\n");										/* Tama incorrecta no ejecta la tarjeta*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			g_cEstadoComSeqMF=SEQ_INICIO;																													/* (3) Trama invalida cmd (N)*/	
			}	
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");	/* respuesta incorrecta*/	
				g_cEstadoComSeqMF=SEQ_INICIO;	
			
			}				
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "EJECT TARJET OK\r\n\r\n");													/* trama ok la tarjeta fue ejectada*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																										/*respuesta ok inicia revisando sensores*/
		}			
			

 		break;	
/*------------------------------------------------------------------------------
El vehiculo esta ubicado en el loop vengo de preguntar el tipo de tarjeta que esta en el lector
(1 0)	 es una tarjeta valida Mf50	
las demas combinaciones son tomadas como no validas
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N) 		
------------------------------------------------------------------------------*/	

	case SEQ_REQUEST:
		
	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_REQUEST;																														/*no ha respondido*/
			}	
				else if (temp==3)
			{
				
				Debug_txt_Tibbo((unsigned char *) "SEQ_REQUEST FALLO\r\n\r\n");												/* trama no valida*/
				DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,1);																		/*imprimo la trama recibida*/
				g_cEstadoComSeqMF=SEQ_CHECK_STATUS;																										/* error de la trama */	
																																															/* (3) Trama invalida cmd (N)*/	
			}	
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde SEQ REQUEST...\r\n\r\n");			/* pto serie no responde*/
				send_portERR(TARJETA_INVALIDA);	   																												/*la tarjeta no es valida no la puede identificar*/
				PantallaLCD(TARJETA_INVALIDA);																														/*envio el msj por la pantalla lcd o la raspberry*/
				g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																											/* capturo la respuesta y regreso a chequear verificador*/	
			
			}				
		}
		else
		{
			 if (Buffer_Rta_Lintech[Card_type_H]=='1')																									/* pregunto si la tarjeta en el transporte es MF 50 */
						  {
								if (Buffer_Rta_Lintech[Card_type_L]=='0')	
							   	{
										Debug_txt_Tibbo((unsigned char *) "Tarjeta valida MF50\r\n\r\n");							/* trama valida son MF50*/
										DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,1);														/* se muestra la trama*/
										LoadVerify_EEprom();  																												// verifico q la clave este cargada en el transporte
										g_cEstadoComSeqMF=SEQ_MF_LINTECH;																							/* la tarjeta es MF50*/
										
								 }else
								 	{
									Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida no es MF50\r\n\r\n");					/* trama no valida */
									DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,1);		
									send_portERR(TARJETA_INVALIDA);
									PantallaLCD(TARJETA_INVALIDA);																									/*envio el msj por la pantalla lcd o la raspberry*/
									g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																						/* capturo la respuesta y regreso a chequear verificador*/
							     
								 	}
						 }
							else 
						 	{
							Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida no es MF50\r\n\r\n");						/* trama no valida */
							DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,1);			
							send_portERR(TARJETA_INVALIDA);	   																									//la tarjeta no es valida 
							PantallaLCD(TARJETA_INVALIDA);																											/*envio el msj por la pantalla lcd o la raspberry*/
							g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																								/* capturo la respuesta y regreso a chequear verificador*/			 
						
						 	}
																																																			/*respuesta ok inicia revisando sensores*/
		}			
					
			break;
/*------------------------------------------------------------------------------
Verifica la eeprom 
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/	
	case SEQ_MF_LINTECH:
		
	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_MF_LINTECH;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "LOAD VERIFY EEPROM ERROR\r\n\r\n");										/* trama no valida respuesta incorrecta falla en la escritura de la clave*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																													/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "LOAD VERIFY EEPROM OK\r\n\r\n");												/* trama OK CARGA LA EEPROM Y LA VERIFICA*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			RD_MF(1,1);																																								/*leo MF */
			g_cEstadoComSeqMF=SEQ_RD_S1B1;																														/*respuesta ok inicia clave verificada*/
		}			

	
	break;
/*------------------------------------------------------------------------------
Leo la Mf en el sector 1  bloque 1
		se lee ID_CLIENTE, COD_PARK Y TIPO_TARJETA
------------------------------------------------------------------------------*/			
	
	case SEQ_RD_S1B1:
		
if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RD_S1B1;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B1 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta falla en la escritura de la clave*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B1 OK\r\n\r\n");															/* trama OK CARGA LA EEPROM Y LA VERIFICA*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; temp++)
					{
						buffer_S1_B1[temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					}
					if (((buffer_S1_B1[MF_ID_CLIENTE]==ID_CLIENTE)&&(buffer_S1_B1[MF_COD_PARK]==COD_PARK))||((ID_CLIENTE==0)&&(COD_PARK==0)))		
					{
							
						Debug_txt_Tibbo((unsigned char *) "ID_CLIENTE: ");
						Debug_HexDec(buffer_S1_B1[MF_ID_CLIENTE]);
						Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");
						
						Debug_txt_Tibbo((unsigned char *) "COD_PARK:");
						Debug_HexDec(buffer_S1_B1[MF_COD_PARK]);
						Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");


						
						if 	((buffer_S1_B1[MF_TIPO_TARJETA]==ROTACION))					
						{
							Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA 1 ROTACION: ");
							Debug_chr_Tibbo(buffer_S1_B1[MF_TIPO_TARJETA]);
							Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");
							
							RD_MF(1, 2);																																				/* Leo fecha y hora de ingreso y de salida*/
							g_cEstadoComSeqMF=SEQ_RD_S1B2;
							
						}
						else 
						{
							send_portERR(TARJETA_INVALIDA);
							PantallaLCD(TARJETA_INVALIDA);																											/*envio el msj por la pantalla lcd o la raspberry*/
							g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																								/* expulso la tarjeta */		
						}
					}
					else
					{
						send_portERR(ERROR_COD_PARK);
						PantallaLCD(ERROR_COD_PARK);																												/*envio el msj por la pantalla lcd o la raspberry*/
						g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																								/* codigo de parqueo erro expulso la tarjeta */		
					}
			}
			else
			{
				send_portERR(TARJETA_SIN_FORMATO);
				PantallaLCD(TARJETA_SIN_FORMATO);																												/*envio el msj por la pantalla lcd o la raspberry*/
				g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;
			}
			
		}			
	break;
/*------------------------------------------------------------------------------
Leo la Mf en el  sector 1 bloque 2
	Se lee Fecha in, fecha out
------------------------------------------------------------------------------*/
	case SEQ_RD_S1B2:
				
	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RD_S1B2;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B2 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta falla en la escritura de la clave*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B2 OK\r\n\r\n");															/* trama OK CARGA LA EEPROM Y LA VERIFICA*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			for (temp=0; temp<16; temp++)
			{
				buffer_S1_B2[temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];															/*almaceno la informacion de MF en un arreglo*/
			}
			
			if((buffer_S1_B2[MF_APB]==02)||(buffer_S1_B2[MF_APB]==0) )															/*Pregunto por el antipasban si es 02 ya salio y no posee entrada*/
			{																																																							/*00 inhabilitada la tarjeta*/
			Debug_txt_Tibbo((unsigned char *) "ERROR: SIN INGRESO\r\n\r\n");
			send_portERR(SIN_INGRESO);
			PantallaLCD(SIN_INGRESO);
			g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;
				break;
			}
			
			if(SIN_COBRO!=0)																																																/*SINCOBRO =0 deshabilitado  =(1) sin cobro = (2) sin cobro salida el mismo dia*/
			{
				if (SIN_COBRO==1)
				{
					Debug_txt_Tibbo((unsigned char *) "Salida Autorizada\r\n\r\n");
					RD_MF(1, 0);																																																/* Leo el ticket */
					g_cEstadoComSeqMF=SEQ_RD_S1B0;																																							/*sin cobro*/
					break;
				}
				else if( SIN_COBRO ==2)
				{
					Block_read_Clock_Hex(clock_temp);																																						/*leo el reloj año,mes,dia*/
									
		
					if ((buffer_S1_B2[0]==clock_temp[0])&&(buffer_S1_B2[1]==clock_temp[1])&&(buffer_S1_B2[2]==clock_temp[2]))		/*se analiza la fecha de entrada de la tarjeta MF y la fecha del dia */
					{
						Debug_txt_Tibbo((unsigned char *) "Salida Autorizada\r\n\r\n");
						RD_MF(1, 0);																																															/* Leo el ticket */
						g_cEstadoComSeqMF=SEQ_RD_S1B0;																																						/*sin cobro el mismo dia*/
						break;
					}																							                                            
						else
						{
						Debug_txt_Tibbo((unsigned char *) "Excede T.GRACIA\r\n\r\n");																							/*excede el timepo de gracia del dia */
						send_portERR(EXCEDE_GRACIA);
						PantallaLCD(EXCEDE_GRACIA);																																								/*envio el msj por la pantalla lcd o la raspberry*/
						g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;
						break;							
		 				}
					
					
				}
			}
		
			
			else if(T_GRACIA!=0)																																		/*si es diferente de cero tiene tiempo de gracia*/
			{
				Debug_txt_Tibbo((unsigned char *) "Tiempo Gracia: ");
				Debug_chr_Tibbo(T_GRACIA);																														/*imprimo la trama recibida*/	
				Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");		
				analiza_tiempo(buffer_S1_B2,T_GRACIA); 																								// Simula Fecha Max de Salida.
			
			}
			
			temp=check_fechaOut(buffer_S1_B2+MF_FECHA_OUT);																					/*se analiza la fecha de salida*/
				if(temp==1)
				{
						Debug_txt_Tibbo((unsigned char *) "TARJETA SIN PAGO\r\n\r\n");
						send_portERR(SIN_PAGO);
						PantallaLCD(SIN_PAGO);																														/*envio el msj por la pantalla lcd o la raspberry*/
						RD_MF(1, 0);																																			/**leo el ticket*/	
						g_cEstadoComSeqMF=SEQ_RD_S1B0_EJECT;					
												
				}
				else if (temp==2)
				{
					Debug_txt_Tibbo((unsigned char *) "Excede T.GRACIA\r\n\r\n");
					send_portERR(EXCEDE_GRACIA);
					PantallaLCD(EXCEDE_GRACIA);																												/*envio el msj por la pantalla lcd o la raspberry*/
					g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;	
				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "Salida Autorizada\r\n\r\n");
					RD_MF(1, 0);																																			/* Leo el ticket */
					g_cEstadoComSeqMF=SEQ_RD_S1B0;	
				}
			
		}
	
			
			
			
	break;

/*------------------------------------------------------------------------------
Leo la Mf en el  sector 1 bloque 0
		lee el ticket
------------------------------------------------------------------------------*/		
		
	case SEQ_RD_S1B0:
	
	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RD_S1B0;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B0 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta el leer el bloque*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B0 OK\r\n\r\n");															/* trama OK CARGA numero de ticket*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/		
			
	for (temp=0; temp<16; temp++)
			{
				
				buffer_S1_B0[temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];																	/*almaceno la informacion de MF el ticket*/
				if(Buffer_Rta_Lintech[Pos_IniDatMF+temp]==0)
				{
					break;
				}
			}
			Debug_txt_Tibbo((unsigned char *) "Numero Ticket: ");
			Debug_txt_Tibbo(buffer_S1_B0);
			Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");
			
			lock=1;		
			Timer_wait=0;																																								/*activo la barrera*/
			Trama_pto_Paralelo_new(buffer_S1_B0,buffer_S1_B2,'s');																			/*transmito pto paralelo*/
			if(USE_LPR)
			{
				Cmd_LPR_Salida(buffer_S1_B0,buffer_S1_B2);																								/*envio datos a Monitor*/
			}
			
			WR_MF(1, 2,buffer_S1_B2+MF_TIPO_VEHICULO);
			g_cEstadoComSeqMF=SEQ_WR_S1B2;
		}
	break;
	
	case SEQ_WR_S1B2:
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_WR_S1B2;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_WR_S1B2 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta el escribir bloque*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_WR_S1B2 OK\r\n\r\n");															/* trama OK CARGA numero de ticket*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/		
			WR_MF(2, 0,buffer_S1_B2+MF_TIPO_VEHICULO);
			g_cEstadoComSeqMF=SEQ_RTA_S2B0;	
		}
		break;
	
	case SEQ_RTA_S2B0:

	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RTA_S2B0;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RTA_S2B0 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta el leer el bloque*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RTA_S2B0 OK\r\n\r\n");															/* trama OK CARGA numero de ticket*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/		
			

	
			Debug_txt_Tibbo((unsigned char *) "ok tarjeta ok...\r\n\r\n");															/* pto serie no responde*/
			Mov_Card(MovPos_Capture);
			g_cEstadoComSeqMF=SEQ_RTA_CAPTURE;
		}
		break;
/*------------------------------------------------------------------------------
Leo la Mf en el  sector 1 bloque 0 leo el ticket
LA tarjeta no tiene pago		
	
------------------------------------------------------------------------------*/
	
	
	case SEQ_RD_S1B0_EJECT:	
		
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_RD_S1B0_EJECT;																														/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B0 ERROR\r\n\r\n");														/* trama no valida respuesta incorrecta el leer el bloque*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/	
			g_cEstadoComSeqMF=SEQ_INICIO;																															/* (3) Trama invalida cmd (N)*/	
			}				
			else
			{	
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");				
				g_cEstadoComSeqMF=SEQ_INICIO;																														/* respuesta incorrecta*/
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "SEQ_RD_S1B0_EJECT OK\r\n\r\n");															/* trama OK CARGA numero de ticket*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);																/*imprimo la trama recibida*/		
			
	for (temp=0; temp<16; temp++)
			{
				
				buffer_S1_B0[temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];																	/*almaceno la informacion de MF el ticket*/
				if(Buffer_Rta_Lintech[Pos_IniDatMF+temp]==0)
				{
					break;
				}
			}
			Trama_pto_Paralelo_P(buffer_S1_B0,buffer_S1_B2,'P');		
			g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;		
		}	
			break;
	
/*------------------------------------------------------------------------------
expulsa la tarjeta por que no pertenece a MF50
------------------------------------------------------------------------------*/		
	
	case SEQ_EXPULSAR_TARJ:
			Card_Insercion(Inhabilita);	
			g_cEstadoComSeqMF=SEQ_EXPULSAR_CHECK;
	break;
	
	
	case SEQ_EXPULSAR_CHECK:
	if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_EXPULSAR_CHECK;																												/*no ha respondido*/
			}	
			else	g_cEstadoComSeqMF=SEQ_INICIO;																													/* respuesta incorrecta*/																									
		}
		else
		{
			g_cEstadoComSeqMF=SEQ_EXPULSAR;																															/*respuesta ok inicia clave verificada*/
		}		
		
		break;
		
	case SEQ_EXPULSAR:
		Mov_Card(MovPos_EjectFront);
	g_cEstadoComSeqMF=SEQ_EXPULSAR_FROM;
	
	case SEQ_EXPULSAR_FROM:                       
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			g_cEstadoComSeqMF=SEQ_EXPULSAR_FROM;																												/*no ha respondido*/
			}	
			else	g_cEstadoComSeqMF=SEQ_INICIO;																													/* respuesta incorrecta*/																									
		}
		else
		{
			g_cEstadoComSeqMF=SEQ_INICIO;																																/*respuesta ok inicia clave verificada*/
		}		
		
	break;
	
/*------------------------------------------------------------------------------
expulsa la tarjeta por que no pertenece a MF50
------------------------------------------------------------------------------*/			
		
				default:
				g_cEstadoComSeqMF=SEQ_INICIO;	
				break;	
			
	}	
}


	
