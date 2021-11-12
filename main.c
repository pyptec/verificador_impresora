/*------------------------------------------------------------------------------
MAIN.C:  sistema verificador

Diseñado Ing. Jaime Pedraza
------------------------------------------------------------------------------*/
#include <main.h>
extern unsigned char placa[];
/*------------------------------------------------------------------------------
interrupcion por timer 
ValTimeOutCom se decrementa cada overflow de la interrupcion
Timer_wait		Incrementa cada overflow de la interrrupcion
------------------------------------------------------------------------------*/
void  timer0_int() interrupt 1 using 2
{
			
			ValTimeOutCom--;
			Timer_wait++;
			TF0=0;
			
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void ini_timer(void)
{

	TMOD=(TMOD & 0xF0) | 0x01;// configuro el reg TMOD sin precalador a 16 bits
		
 	TH0=0X00;								//registro de la parte alta del timer0 de 16 bits									*						
	TL0=0X00;								// registro de la parte baja del timer0
	TR0=1;									// habilita el timer0 reg TCOM bit 4 para interrup

}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void on_Timer0_Interrup(void)
{
	TF0=0;									//registro TCON bit 5 bandera de overflow 
	ET0=1;									// enable interrupcion bit 1 del registro IENO
	
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void off_Timer0_Interrup(void)
{
	ET0=0;									// enable interrupcion bit 1 del registro IENO
	
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
_getkey waits until a character is received from the serial port.  This may not
be the exact desired operation (for example if the buffer is empty, this
function hangs waiting for a character to be received).
------------------------------------------------------------------------------*/
char _getkey (void)
{
int k;

do
  {
  k = com_getchar ();
  }
while (k == -1);

return ((unsigned char) k);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
char putchar (char c)
{
volatile unsigned int i;

while (com_putchar (c) != 0)
  {
  for (i=0; i<1000; i++)
    {
    /*** DO NOTHING ***/
    }
  }

return (c);
}
/*------------------------------------------------------------------------------
escribir una cadena de datos por pto serie
------------------------------------------------------------------------------*/
/*TEMPORALMENTE SIN OPERAR
void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena)
{
	unsigned char i,d;
		for(i=0;i<tamano_cadena;i++)
   	{
      d=putchar(*buffer);
			buffer++;
    }
}
*/
/*------------------------------------------------------------------------------
secuencia de inicio
------------------------------------------------------------------------------*/
void inicia_board(void)
{
cond_ini_tibbo();							/* inicia tibbo*/
cond_ini_clock();							/* inicia clock*/
cond_ini_pto();								/*Inicializacion del pto*/
Atascado=0;										/*prendo el verificador*/
sel_com=1;										/*seleccionar el pto serie */
lock=0;												/*barrera off*/	
	
}
/*------------------------------------------------------------------------------
leo la configuracion en eeprom  y la cargo en registros fijos
------------------------------------------------------------------------------*/
void variable_inicio()
{
	
	ID_CLIENTE=rd_eeprom(0xa8,EE_ID_CLIENTE);	
	COD_PARK=rd_eeprom(0xa8,EE_ID_PARK);
	T_GRACIA=rd_eeprom(0xa8,EE_TIEMPO_GRACIA);
	SIN_COBRO=rd_eeprom(0xa8,EE_SIN_COBRO);
	Debug_Tibbo=rd_eeprom(0xa8,EE_DEBUG);
	USE_LPR=rd_eeprom(0xa8,EE_USE_LPR);
	COMPARACION_ACTIVA=rd_eeprom(0xa8,EE_CPRCN_ACTIVA);
	QR_BARRAS=rd_eeprom(0xa8,EE_QR_BARRAS);
	Raspberry = rd_eeprom(0xa8,EE_TIPO_PANTALLA);
}
/*------------------------------------------------------------------------------
Note that the two function above, _getkey and putchar, replace the library
functions of the same name.  These functions use the interrupt-driven serial
I/O routines in SIO.C.
------------------------------------------------------------------------------*/


void main (void)
{
	
	static unsigned char buffer[40];
	//unsigned char buffer_clock[]={0x02,0x30,0x30,0x30,0x30,0x30,0x35,0x30,0x30,0x30,0x30,0x03,0}; //dia,mes,año,hora,minutos,SEGUNDOS,Dia de la semana
	unsigned char buffer1[13];
	
	unsigned char Length_trama;
ini_timer();
on_Timer0_Interrup();	
com_initialize ();              																					/* initialize interrupt driven serial I/O */
com_baudrate ();            																							/* setup for 9600 baud */
inicia_board();
EA = 1;                         																					/* Enable Interrupts */
/*solo de pruebas */
time_bit();
//Delay_20us(33);
time_mbit();
tx_aux('a');	

sel_Funcion();																														/*el pulsador funcion es precionadoa para entrar a programacion */
	if (DataIn==0)
	{
	menu();
	}
	variable_inicio();
	Limpiar_memoria_ticket();

	ValTimeOutCom=3;																												/*tiempo inicial para ver los sensores*/
	
	while (1) 																															// Loop Principal								*	
	{   
		
                    				
			if (rx_ip==0)
			{
			
				Length_trama=recibe_cmd_Monitor(buffer1);
				Debug_Dividir_texto();	
				Valida_Trama_Monitor(buffer1,Length_trama);
				DebugBufferMF(buffer1,Length_trama,1);																/*imprimo la trama recibida*/				
				Debug_txt_Tibbo(placa);	
				Debug_Dividir_texto();				
			}
				
				Lee_ticket();																										/*procedimiento de imprimir el ticket de entrada*/
			
				//Debug_txt_Tibbo((unsigned char *) "tiempo ");		/*msj longitud de la trama */
				//Debug_chr_Tibbo(ValTimeOutCom);	
				//Debug_txt_Tibbo((unsigned char *) " \r\n");	
			
			if (busy==0)
			{
				
				Length_trama=recibe_port(buffer);																		/*recibe informacion del pto paralelo*/
																					
				//Debug_Dividir_texto();																							/*division del texto */
				//Debug_txt_Tibbo((unsigned char *) "Recibe trama pto:");					/*trama recibida pto paralelo */
				DebugBufferMF(buffer,Length_trama,1);																/*imprimo la trama recibida*/
			//	Debug_txt_Tibbo((unsigned char *) " \r\n");
		   //	Debug_txt_Tibbo((unsigned char *) "longitud de la trama:");		/*msj longitud de la trama */
				Debug_chr_Tibbo(Length_trama);																			/*numero de caracteres recibidos*/
				Debug_txt_Tibbo((unsigned char *) " \n");	
				//Debug_Dividir_texto();																							/*divido el texto*/
				
				Valida_Trama_Pto(buffer,Length_trama);															/*valido la informacion recibida */
				 
			}
			msj_lcd_informativo();
	
	
	}

}


