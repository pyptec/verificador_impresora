
#include "prog.h" 
#include <string.h>
extern int printf   (const char *, ...);
extern char putchar (char);
extern char getchar (void);
extern char  *strtok  (char *str, const char *set);
extern char   strcmp  (const char *s1, const char *s2);
extern char  *strcpy  (char *s1, const char *s2);
extern char _getkey (void);
extern char  *strcat  (char *s1, const char *s2);
extern unsigned char rd_eeprom (unsigned char control,unsigned int Dir); 
extern void wr_eeprom (unsigned char control,unsigned int Dir, unsigned char data_eeprom);
extern int sprintf  (char *, const char *, ...);
extern int    atoi (const char *s1);
extern void LeerMemoria(unsigned int addres,unsigned char *res);
extern void EscribirMemoria(unsigned char control,unsigned int  addres,unsigned char  *res);
extern void clear_buffer();
unsigned char cursor[20] = "/"; //current working directory
unsigned char validacion [9]={"admin"};	//usuario

extern idata unsigned char tbuf [];
extern idata unsigned char rbuf [];
/*define posiciones de memoria*/
#define EE_ID_CLIENTE					0x0000
#define EE_ID_PARK		  			0x0002
#define EE_TIEMPO_GRACIA		  0x0004
#define EE_SIN_COBRO					0x0006
#define EE_DEBUG							0x0008
#define EE_USE_LPR						0x000A
#define EE_TIPO_PANTALLA			0X000B
#define EE_CPRCN_ACTIVA				0x000C
#define EE_QR_BARRAS					0x000E
#define	EE_CLIENTE						0X0010
#define EE_NIT								0x0040
#define EE_PARQUEADERO				0X0050
#define EE_ADDRES							0x0080
#define EE_MSJ1								0X00B0
#define EE_MSJ2								0X00f1
#define EE_MSJ3								0X0132
#define EE_MSJ4								0X0173
#define EE_MSJ5								0X01b4
#define EE_MSJ6								0X01f5
#define EE_MSJ7								0X0236

/* Definicion del tamaño de comando y longitud de cmd*/

#define 	NUMCOMMAND 22
#define 	LONGSIZE 3



char comandos[NUMCOMMAND][LONGSIZE]=
{
  "0",       		//ID_CLIENTEValor del punto cero.
	"1", 	     //COD_PARKTiempo de recolecion entre muestra
	"2",		//T_GRACIA Informacion recolectada enviada al rs232
	"3",		//SIN_COBRO"Ajuste del cero del sensor
	"4",	// DEBUGCalibracion por la curva de la pendiente
	"5",//USE_LPR
	"6",	//tipo de pantalla
	"7",	//COMPARACION_ACTIVA
	"8",	//1=QR 0=codigo de barras
	"9",  	//CLIENTENombre del cliente
	"10",        //NITnit
	"11",		//NOM_PARQ Nombre de parqueadero
	"12",		//direccion del establecimiento 
	"13",		// mensajes
	"14",
	"15",
	"16",
	"17",
	"18",
	"19",
	"20",     // Ayuda!muestra todos los comandos
	"21"		//Salir de programacion
};

/*------------------------------------------------------------------------------
Rutina de lista de comandos 
------------------------------------------------------------------------------*/

unsigned char ListaComandos(unsigned char *cmd)
{
   unsigned char buf[LONGSIZE];
   unsigned char i;
   
   for (i=0; i<NUMCOMMAND; i++)
   {
      strcpy(buf, &comandos[i][0]);
      if (strcmp(buf, cmd)==0)
         return(i);
   }
   
   return(0xFF);
}


/*------------------------------------------------------------------------------
Rutina que valida la clave
------------------------------------------------------------------------------*/
unsigned char  ValidarClave(unsigned char *buffer)
{
unsigned char buf [7];
  
      strcpy(buf, &validacion[0]);
      if (strcmp(buf, buffer)==0)
         return(0);
 
   
   return(0xFF);
}



/*------------------------------------------------------------------------------
Rutina que busca el \
------------------------------------------------------------------------------*/
unsigned char * GetCMD(unsigned char *in)
{
   char tokens[]=" \r\n";
   return(strtok(in,tokens));
}

/*------------------------------------------------------------------------------
Rutina de 
------------------------------------------------------------------------------*/
unsigned char * GetOption(unsigned char *in)
{
   unsigned char tokensSpace[]=" \r\n";
   unsigned char tokensQuote[]="\"\r\n";
   
   //trim leading spaces
   while (*in==' ')
      in++;
   
   //if first char is a quote, then end token on a quote.  ELSE end token on a space
   if (*in == '\"')
      return(strtok(in,tokensQuote));
   else
      return(strtok(in,tokensSpace));
} 
/*------------------------------------------------------------------------------
Rutina de muestra el cursor
------------------------------------------------------------------------------*/

void DisplayCursor(void)
{
   printf("\r\n\n%s> ", cursor);
}

/*------------------------------------------------------------------------------
Rutina que captura los datos pto serie y los muestra en el pto
------------------------------------------------------------------------------*/

void IngresaDato(unsigned char  *buffer, unsigned char modo)
{

 int i;
 
	do
	{ 
		*buffer = _getkey();
	if (modo == 0) 
		{
  		 printf("%c", *buffer);
		}
		else 
		{
			putchar('*');
		}
	   i=*buffer; 
	   buffer++;
	 	*buffer='\0';	
		
	} while(i != '\r');
    
      
         
} 

/*------------------------------------------------------------------------------
Rutina que programa el id_cliente en eeprom
------------------------------------------------------------------------------*/
void prog_id_cliente()
{
	unsigned char buffer[10];
	unsigned int dataee;
	
	dataee=rd_eeprom(0xa8,EE_ID_CLIENTE);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	
	printf("\r\n\n ACTUAL ID_CLIENTE=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	
	printf("\r\n\n DIGITE SU NUEVO ID_CLIENTE=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_ID_CLIENTE,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_ID_CLIENTE);																					/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);																									/*lo convierto a un string*/
	printf("\r\n\n DATO MODIFICADO ID_CLIENTE=%s\r\n\n",buffer);									/*lo muetro por la pantalla el dato grabado*/
}
/*------------------------------------------------------------------------------
Rutina que programa el cod_park
------------------------------------------------------------------------------*/
void prog_cod_park()
{
	unsigned char buffer[10];
	unsigned int dataee;
	
	dataee=rd_eeprom(0xa8,EE_ID_PARK);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	
	printf("\r\n\n ACTUAL COD_PARK=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	
	printf("\r\n\n DIGITE SU NUEVO COD_PARK=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_ID_PARK,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_ID_PARK);																					/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);																									/*lo convierto a un string*/
	printf("\r\n\n DATO MODIFICADO COD_PARK=%s\r\n\n",buffer);									/*lo muetro por la pantalla el dato grabado*/
}
/*------------------------------------------------------------------------------
Rutina que programa el tiempo de gracia de cobro (00) no tiene tiempo de gracia
------------------------------------------------------------------------------*/
void Prog_tiempo_gracia()
{
	unsigned char buffer[10];
	unsigned int dataee;
	
	dataee=rd_eeprom(0xa8,EE_TIEMPO_GRACIA);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	
	printf("\r\n\n ACTUAL TIEMPO_GRACIA=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	
	printf("\r\n\n DIGITE SU NUEVO TIEMPO_GRACIA=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_TIEMPO_GRACIA,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_TIEMPO_GRACIA);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);																									/*lo convierto a un string*/
	printf("\r\n\n DATO MODIFICADO TIEMPO_GRACIA=%s\r\n\n",buffer);								/*lo muetro por la pantalla el dato grabado*/
}
/*------------------------------------------------------------------------------
Rutina que programa SIN COBRO
SIN_COBRO=0 inhabilitado, SIN_COBRO=1 sin cobro, SIN_COBRO=2 sin cobro y salida el mismo dia");
------------------------------------------------------------------------------*/
void Prog_Sin_Cobro()
{
	

	unsigned char buffer[10];
	unsigned int dataee;

	dataee=rd_eeprom(0xa8,EE_SIN_COBRO);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL SIN_COBRO INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else if (dataee==1)
	{
		printf("\r\n\n ACTUAL SIN_COBRO GRATIS=%s\r\n\n",buffer);		
	}
	else 
	{
			printf("\r\n\n ACTUAL SIN_COBRO GRATIS Y SALIDA EL MISMO DIA=%s\r\n\n",buffer);		
	}
	
	printf("\r\n\n DIGITE SU NUEVO SIN_COBRO=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_SIN_COBRO,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_SIN_COBRO);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);																									/*lo convierto a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL SIN_COBRO INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else if (dataee==1)
	{
		printf("\r\n\n ACTUAL SIN_COBRO GRATIS=%s\r\n\n",buffer);		
	}
	else 
	{
			printf("\r\n\n ACTUAL SIN_COBRO GRATIS Y SALIDA EL MISMO DIA=%s\r\n\n",buffer);		
	}
	
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void Prog_debug()
{
	unsigned char buffer[10];
	unsigned int dataee;

	
	dataee=rd_eeprom(0xa8,EE_DEBUG);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL DEBUG INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL DEBUG HABILITADO=%s\r\n\n",buffer);			
	}
	
	printf("\r\n\n DIGITE EL NUEVO ESTADO DE DEBUG=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_DEBUG,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_DEBUG);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);	
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL DEBUG INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL DEBUG HABILITADO=%s\r\n\n",buffer);			
	}
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void Prog_Use_Lpr()
{
		unsigned char buffer[10];
	unsigned int dataee;

	
	dataee=rd_eeprom(0xa8,EE_USE_LPR);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL USE_LPR INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL USE_LPR HABILITADO=%s\r\n\n",buffer);			
	}
	
	printf("\r\n\n DIGITE EL NUEVO ESTADO DE USE_LPR=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_USE_LPR,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_USE_LPR);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);	
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL USE_LPR INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL USE_LPR HABILITADO=%s\r\n\n",buffer);			
	}
}
/*------------------------------------------------------------------------------------
cmd del tipo de pantalla a usar
Rasberry = 1
lcd = 0
--------------------------------------------------------------------------------------*/
void tipo_pantalla()
{
	unsigned char buffer[10];
	unsigned int dataee;

	
	dataee=rd_eeprom(0xa8,EE_TIPO_PANTALLA);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n PANTALLA LCD HABILITADA=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n PANTALLA RASPBERRY  HABILITADA=%s\r\n\n",buffer);			
	}
	
	printf("\r\n\n DIGITE EL NUEVO ESTADO DE LA PANTALLA =");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_TIPO_PANTALLA,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_TIPO_PANTALLA);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);	
	if(dataee==0)
	{
		printf("\r\n\n PANTALLA LCD HABILITADA=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL COMPARACION ACTIVA  HABILITADA=%s\r\n\n",buffer);			
	}
	
}



/*------------------------------------------------------------------------------	

------------------------------------------------------------------------------*/
void Prog_Comparacion_Activa()
{
	unsigned char buffer[10];
	unsigned int dataee;

	
	dataee=rd_eeprom(0xa8,EE_CPRCN_ACTIVA);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL COMPARACION ACTIVA INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL COMPARACION ACTIVA  HABILITADO=%s\r\n\n",buffer);			
	}
	
	printf("\r\n\n DIGITE EL NUEVO ESTADO DE COMPARACION ACTIVA =");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_CPRCN_ACTIVA,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_CPRCN_ACTIVA);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);	
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL COMPARACION ACTIVA  INHABILITADO=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL COMPARACION ACTIVA  HABILITADO=%s\r\n\n",buffer);			
	}
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void Prog_QR_barras()
{
	unsigned char buffer[10];
	unsigned int dataee;

	
	dataee=rd_eeprom(0xa8,EE_QR_BARRAS);																					/*se lee el id_cliente actual */
	sprintf(buffer,"%d",dataee);																									/*se convierte  un entero a un string*/
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL CODIGO DE BARRAS ACTIV0=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL CODIGO QR ACTIVO=%s\r\n\n",buffer);			
	}
	
	printf("\r\n\n DIGITE EL NUEVO ESTADO DE CODIGO QR O CODIGO DE BARRAS=");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	dataee=atoi(buffer);																													/*lo convierto a un dato hex*/
	wr_eeprom(0xa8,EE_QR_BARRAS,dataee);																					/*grabo el dato en la eeprom*/
	
	dataee=rd_eeprom(0xa8,EE_QR_BARRAS);																				/*leo el dato grabado*/
	sprintf(buffer,"%d",dataee);	
	if(dataee==0)
	{
		printf("\r\n\n ACTUAL CODIGO DE BARRAS ACTIV0=%s\r\n\n",buffer);														/*se muestra el id_cliente actual en pantalla*/
	}
	else
	{
		printf("\r\n\n ACTUAL CODIGO QR ACTIVO=%s\r\n\n",buffer);			
	}
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void Prog_Cliente(void)
{
	unsigned char buffer[48];
	
	printf("\r\n\n Digite el nombre del cliente ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_CLIENTE,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_CLIENTE,buffer);	
	printf("\r\n\n texto grabado=%s\r\n\n",buffer);		
}
void Prog_Nit(void)
{
	unsigned char buffer[16];
	
	printf("\r\n\n Digite el numero de Nit ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_NIT,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_NIT,buffer);	
	printf("\r\n\n NIT: %s\r\n\n",buffer);		
}
/*------------------------------------------------------------------------------
Rutina que captura el nombre del parqueadero y lo almacena en eeeprom
------------------------------------------------------------------------------*/
void Prog_Nom_Parq()
{
	unsigned char buffer[48];
	
	printf("\r\n\n Digite el Nombre del Parqueadero ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_PARQUEADERO,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_PARQUEADERO,buffer);	
	printf("\r\n\n Nombre: %s\r\n\n",buffer);		
}
void Prog_Direccion()
{
	unsigned char buffer[48];
	
	printf("\r\n\n Digite la direccion del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_ADDRES,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_ADDRES,buffer);	
	printf("\r\n\n Direccion: %s\r\n\n",buffer);		
}	
void Prog_Msj1()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj1 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ1,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ1,buffer);	
	printf("\r\n\n msj1: %s\r\n\n",buffer);		
}
//EE_MSJ1
	void Prog_Msj2()
{
unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj2 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ2,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ2,buffer);	
	printf("\r\n\n msj1: %s\r\n\n",buffer);	
}
	void Prog_Msj3()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj3 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ3,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ3,buffer);	
	printf("\r\n\n msj3: %s\r\n\n",buffer);	
}
	void Prog_Msj4()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj4 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ4,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ4,buffer);	
	printf("\r\n\n msj4: %s\r\n\n",buffer);	
}
	void Prog_Msj5()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj5 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ5,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ5,buffer);	
	printf("\r\n\n msj5: %s\r\n\n",buffer);	
}

	void Prog_Msj6()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj6 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ6,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ6,buffer);	
	printf("\r\n\n msj6: %s\r\n\n",buffer);	
}


	void Prog_Msj7()
{
	unsigned char buffer[62];
	
	printf("\r\n\n Digite el msj7 del establecimiento ");																	/*digite el nuevo id_cliente*/
	IngresaDato(buffer,0);																												/*trae el dato digitado*/
	
	EscribirMemoria(0xa8,EE_MSJ7,buffer);																					/*grabo el dato en la eeprom*/
	LeerMemoria(EE_MSJ7,buffer);	
	printf("\r\n\n msj7: %s\r\n\n",buffer);	
}
/*------------------------------------------------------------------------------
Rutina que muestra la lista de comandos
------------------------------------------------------------------------------*/
void Show()
{
   printf("\r\n ID_CLIENTE    			--- cmd 0 Identificador del cliente maximo 255");
   printf("\r\n COD_PARK      			--- cmd 1 El numero del parqueadero maximo 255");
   printf("\r\n T_GRACIA      			--- cmd 2 Tiempo sin cobro 00,  maximo 255");
   printf("\r\n SIN_COBRO    				--- cmd 3 Inhabilitado= 0, Gratis= 1 , Gratis y salida el mismo dia= 2");
   printf("\r\n DEBUG         			--- cmd 4 Habilitar = 1, Inhabilitar = 0");
	 printf("\r\n USE_LPR       			--- cmd 5 Habilitar = 1, Inhabilitar = 0");
   printf("\r\n TIPO_PANTALLA      	--- cmd 6 PANTALLA LCD =0 PANTALLA RASPBERRI=1");
	 printf("\r\n COMPARACION_ACTIVA 	--- cmd 7 Habilitar = 1, Inhabilitar = 0");
	 printf("\r\n QR_COD_BARRAS 			--- cmd 8 Habilitar QR = 1, Habilitar COD_BARRAS = 0"); 
	 printf("\r\n CLIENTE       			--- cmd 9 Nombre del cliente");
	 printf("\r\n NIT           	    --- cmd 10 Numero del nit");
	 printf("\r\n NOM_PARQ      	    --- cmd 11 Nombre del parqueadero");
	 printf("\r\n DIRECCION    	      --- cmd 12 Nombre del parqueadero");
	 printf("\r\n MSJ_1         	    --- cmd 13 msj de informacion del parqueadero");
	 printf("\r\n MSJ_2         	    --- cmd 14 msj de informacion del parqueadero");
	 printf("\r\n MSJ_3         	    --- cmd 15 msj de informacion del parqueadero");
	 printf("\r\n MSJ_4        	      --- cmd 16 msj de informacion del parqueadero");
	 printf("\r\n MSJ_5        	      --- cmd 17 msj de informacion del parqueadero");
	 printf("\r\n MSJ_6        	      --- cmd 18 msj de informacion del parqueadero");
	 printf("\r\n MSJ_7         	    --- cmd 19 msj de informacion del parqueadero");
	 printf("\r\n AYUDA         	    --- cmd 20 Muestra los comandos");
   printf("\r\n SALIR         	    --- cmd 21 Salir de programacion");

}

/*------------------------------------------------------------------------------
Rutina de principal de programacion
------------------------------------------------------------------------------*/

void menu(void)
{

unsigned char *cmd,*option1,*option2;
unsigned char opt_buffer[20];
unsigned char buffer[20];



  
  printf("\r\n\nSistema de Programacion Verificador\r\n\r\n");

 
	
	
	do{
	printf("\r\n\n/>Usuario:");
	clear_buffer();
  // main loop
	IngresaDato(buffer,0);					//ingreso el usuario por teclado 
	cmd = GetCMD(buffer);					//quita el carri return	
	printf("\r\n\n datos del buffer rx %s\r\n\n",rbuf);	
	printf("\r\n\n datos del buffer tx %s\r\n\n",tbuf);		
}while(ValidarClave(cmd)!=0);				//validamos el usuario


  do{
printf("\r\n\n/>Password:");

	IngresaDato(buffer,1);					//ingreso el password por teclado 
	cmd = GetCMD(buffer);					//quita el carri return	
	strcpy (validacion,"123456");
	

	}while(ValidarClave(cmd)!=0);				//

	
	Show();
	while(1)
	{
	DisplayCursor();


		IngresaDato(buffer,0);	
		cmd = GetCMD(buffer);	
		option1 = GetOption(cmd + strlen(cmd) + 1);
    option2 = GetOption(option1 + strlen(option1) + 1);	 

 	 //si empieza con un '/', eso significa que incluye el caminoa
      //al archivo.  si el archivo no empieza con un '/', debe agregarce 
      //la ruta del archivo.
      if (option1 && (option1[0]=='/'))
      {
         //option1 is a full path
         strcpy(opt_buffer, option1);
      }
      else if (option1)
      {
         // tack on the current working directory to option1
         strcpy(opt_buffer, cursor);
         strcat(opt_buffer, option1);
      }         





    if (cmd)
      {
         switch(ListaComandos(cmd))
         {
            case 0:  										/*Se programa la identificacion del cliente*/
							prog_id_cliente();
						
				     break;
            
            case 1: 									 /*Codigo de parqueadero*/
						prog_cod_park();
               break;
            
            case 2:  //Informacion recolectada enviada al rs232
		
						Prog_tiempo_gracia();
					
               break;
            
            case 3:  //Ajuste del cero del sensor
						Prog_Sin_Cobro();

               break;
            
						case 4:  //Ajuste de calibracion por medio de la pendiente
							Prog_debug();
               break;
						case 5:  //help me
							Prog_Use_Lpr();
               break;
						
						case 6: 
						tipo_pantalla();
						break;
						case 7:  //help me
           Prog_Comparacion_Activa();

               break;
							case 8:  									/*Nombre del cliente*/
							Prog_QR_barras();
							
               break;
						case 9:  									/*Nombre del cliente*/
							Prog_Cliente();
							
               break;
						case 10:  									/*Numero del Nit*/
							
						Prog_Nit();
							
               break;
							case 11:  									/*Numero del Nit*/
							
						Prog_Nom_Parq();
							
               break;
								case 12:  									/*Numero del Nit*/
							
						Prog_Direccion();
							
               break;
						
						case 13:  									/*Numero del Nit*/
							
						Prog_Msj1();
							
               break;
						case 14:  									/*Numero del Nit*/
							
						Prog_Msj2();
							
               break;
						
						case 15:  									/*Numero del Nit*/
							
						Prog_Msj3();
							
               break;
						case 16:  									/*Numero del Nit*/
							
						Prog_Msj4();
							
               break;		
						case 17:  									/*Numero del Nit*/
							
						Prog_Msj5();
							
               break;

						case 18:  									/*Numero del Nit*/
							
						Prog_Msj6();
							
               break;		
						case 19:  									/*Numero del Nit*/
							
						Prog_Msj7();
							
               break;		

						
						case 20:  //help me
							
							Show();
               break;
						case 21:  //salir
						return;

               break;
           
		
					
            default:
               printf("\r\nComando no existe '%s'", cmd);
               break;
        }
      }
	}
}

	

