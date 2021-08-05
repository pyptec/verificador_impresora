//#include <printer.h>
//#include <string.h>
/*
#define EOT	0x04
#define ENQ	0x05
#define ACK	0x06
#define CR	0x0d
#define LF	0x0a
#define DLE	0x10
#define ESC	0x1B
#define GS	0x1D
#define SP	0x20
*/
//extern int printf   (const char *, ...);
//extern char putchar (char);

/*------------------------------------------------
Cmd de inicio de impresora
------------------------------------------------*/
/*void Inicializa_Print(void)
{
	putchar(ESC);
	putchar('@');
}
*/
/*------------------------------------------------
Cmd de justificado del texto
------------------------------------------------*/
//void Justify(unsigned char dato_print)
//{
//	switch (dato_print)
	/*{

/			case 'L':
				JustificaIzq();
			break;
			case 'C':
				Centrado();
			break;
			case 'R':
				JustificaDrc();
			break;
		
			default:
			
			break;
		}
	
}
*/
/*
void Posicion_tabulacion(void)
{
	putchar(ESC);
	putchar('D');
	putchar(0x35);
	putchar(0);
}

void Move_car()
{
	putchar(GS);
	putchar('P');
	putchar(0xB4);
	putchar(0XB4);
}
*/
//***********************************************************************************************************
/*
void Status(unsigned char StatusConsulta)
{
 	
	putchar(DLE);
	putchar(EOT);
	putchar(StatusConsulta);
}

void MargenIzq(void)
{
	putchar(GS);
	putchar('L');	
	putchar(0x10);
	putchar(0x00);
}
//***********************************************************************************************************
void Bold(void)
{
	putchar(ESC);
	putchar('E');	
	putchar(0x01);
}*/
/*-----------------------------------------------------------------------------------------------
posicion del texto
(C)= 0 centrado
(l)= 1 justificado a la izquierda
(2)= 2	Justificado a la derecha
-------------------------------------------------------------------------------------------------*/	
/*
void JustificaIzq(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('0');
}

void Centrado(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('1');
}

void JustificaDrc(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('2');
}
*/
/*------------------------------------------------
Cmd tamaño del caracter  impresora
00 tamaño normal
0x11	tamaño grande
------------------------------------------------*/
/*
void Tamaño_Char(unsigned char tamaño)
{
	putchar(GS);
	putchar('!');
	putchar(tamaño);
}
*/
/*------------------------------------------------
Cmd de alimenta el papel N lineas 
------------------------------------------------*/
/*
void Alimenta_nline(unsigned char nline)
{
	putchar(ESC);
	putchar('d');
	putchar(nline);
}
void Alimentar_paper(unsigned char nline)
{
	putchar(ESC);
	putchar('J');
	putchar(nline);
}
*/
/*------------------------------------------------
Cmd de ESPACIOS ENTRE CARACTERES 
0= normal
1= separacion de espacios entre caracter
------------------------------------------------*/
/*
void Espcs_ent_chr(unsigned char espacios)
{
	putchar(ESC);
	putchar(SP);
	putchar(espacios);
}
*/
//***********************************************************************************************************
/*
void QR_Size(void)
{
	putchar(GS);
	putchar('q');	
	putchar(EOT);

}
//***********************************************************************************************************
void FullCut(void)
{
	putchar(ESC);
	putchar('i');	
}
*/
//void Print_ticket()
///{
//		Inicializa_Print();													/*cmd de inicio de impresora*/
	//	Posicion_tabulacion();											/*posiciona el carro*/
	//	Move_car();																	/*establece las unidades de movimiento H y V*/
	//	Justify('C')																/*texto justificado al centro*/
	//	Bold();																			/*pone el texto en negrilla*/
	//	Tamaño_Char(0x11);														/*tamaño del caracter*/						
	//	Espcs_ent_chr(01);													/*espacio entre caracteres*/
	//	Alimentar_paper(06);												/*alimenta el papel y se posiciona para imprimir*/
	//	LeerMemoria(EE_PARQUEADERO,buffer);	
	//	printf("%s\n",buffer);											/*nombre del parqueadero*/
		
	
			//		printf("TEL. 7456434-7456672\r\n\r\n"); 
	
	
//}
