/*!
 ***********************************************************************
 *   @file cos_ser.c
 *   @par Module:    renesas controller serial communication
 *
 *   @brief  Routines for serial communication on renesas Controller RX63N.
 *           Uses the board support package 'bsp' (Zink). The routines use
 *           putchar() and getchar().
 *
 *
 *   @par Author:     Ernst Forgber (Fgb)
 *
 ***********************************************************************
 *
 *   @par History       :
 *   @verbatim
 * Ver  Date        Author            Description
 * 0.0  04.12.2008  E. Forgber        file created
 * 0.1  20.03.2013  E. Forgber        Dokumentation auf Deutsch umgestellt
 * 0.2  09.10.2015  E. Forgber (Fgb)  switch to renesas controller
 *
 *   @endverbatim
 *
 *
 ***********************************************************************/


/**************************************************************************

Copyright 2016 Ernst Forgber


This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*****************************************************************************/




//#include <avr/io.h>
//#include <stdint.h>
#include "cos_ser.h"


/**************************************************************************
*      Renesas RX63N Verion                                               *
**************************************************************************/

#if COS_PLATFORM == PLATFORM_RENESAS_RX63N

#define SER_ECHO 1     /*!< 0: kein Echo, sonst empfangene Zeichen mit Echo */



/*!
 **********************************************************************
 * @par Beschreibung:
 *   Diese Funktion ist notwendig, um den Empfangsinterrupt der seriellen
 *   Schnittstelle zu setzten. Die Funktionen dieses
 *   Moduls nutzen die Routinen des Board Support Package (bsp,  Autor: Zink).
 *   Die Einstellungen der seriellen Schnittstelle werden durch das bsp
 *   initialisiert. Die
 *   Standardeinstellungen dort sind: 9600 Baud, 8 Datenbit, 1 Stoppbit,
 *   keine ParitÃ¤t (9600, 8N1).
 *

 * @see file read.c
 * @arg
 *
 *
 * @param  baudRate           - IN, Baudrate
 *
 * @retval                - kein Rueckgabewert
 *
 * @par Beispiel :
 *    Initialisierung der seriellen Schnittstelle mit 9600 Baud, 8 Datenbit,
 *    kein Paritaetsbit, 1 Stoppbit:
 * @verbatim
int main(void)
{ ...
  serInit(9600UL);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serInit(uint32_t baudRate)
{   //uartInitialize(baudRate, UART_DATABITS_8, UART_PARITY_NONE, UART_STOPBITS_1);
	//nutzt die Funktionen aus dem bsp: putchar(), getchar()

	_initSerialInterface_RX_Interrupt(); // siehe file read.c
}





/*!
 **********************************************************************
 * @par Beschreibung:
 *   Sende ein einzelnes Zeichen (1 Byte)
 *
 * @see
 * @arg  serPuts()
 *
 *
 * @param  x  - IN, das zu sendende Byte
 *
 * @retval   - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim
int main(void)
{ ...
  serInit(3686400UL, 9600UL);
  serPutc('a');
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serPutc(char x)
{
	putchar(x);
}




/*!
 **********************************************************************
 * @par Beschreibung:
 *   Sende eine Zeichenkette (null terminated string)
 *
 * @see
 * @arg  serPutc()
 *
 *
 * @param  pt  - IN, Zeiger auf die Zeichenkette
 *
 * @retval                - keine Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serPuts("hello world\r\n");
  serPuts("this is Atmel on stk500 calling...\r\n");
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serPuts(char *pt)
{
  while(*pt != '\0')
  {  serPutc(*pt);
     pt++;
  }
}




/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 8 Bit Zahl (uint8_t) im Binaerformat auf dem Terminal
 *
 * @see
 * @arg serOutInt16Dec(), serOutUint8Hex(), serOutUint16Hex()
 *
 *
 * @param  x  - IN, Zahlenwert der im Binaerformat gedruckt wird
 *
 * @retval          - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint8Bin(0xa5);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint8Bin(uint8_t x)
{   uint8_t i;

    serPuts(" 0b");  // Praefix fuer Binaerzahl ist 0b
    for(i=0; i<8; i++)
    {   serPutc((x>>7) + '0');
        x <<= 1;
    }
}






/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 8 Bit Zahl (uint8_t) im Hex-Format auf dem Terminal
 *
 * @see
 * @arg serOutInt16Dec(), serOutUint16Hex()
 *
 *
 * @param  x  - IN, Zahlenwert der im Hex-Format gedruckt wird
 *
 * @retval         - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint8Hex(0xa5);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint8Hex(uint8_t x)
{
    uint8_t y = x >> 4;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( (y) < 10) serPutc(y+0x30);  // high nibble
    else          serPutc(y+0x37);
    y = x & 0x0f;
    if( (y) < 10) serPutc(y+0x30);  // low nibble
    else          serPutc(y+0x37);
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer vorzeichenlosen 16 Bit Zahl im Dezimalsystem. Die
 *   Funktion wandelt ins Dezimalsystem mit Hilfe des Division und
 *   Modulo Algorithmus. Die Zeichen werden intern auf einem Stack
 *   gespeichert, um sie in umgekehrter Reihenfolge ausgeben zu koennen.
 *
 * @see
 * @arg serOutInt16Dec(), serOutUint8Hex()
 *
 *
 * @param  x  - IN, zu druckender Wert
 *
 * @retval              - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint16Dec(56000);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint16Dec(uint16_t x)
{   unsigned char letter[5];
    int16_t n=0, i;

    serPutc(' ');
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 16 Bit Zahl ohne Vorzeichen im HEX-Format
 *
 * @see
 * @arg serOutInt16Dec(), serOutUint8Hex()
 *
 *
 * @param  x  - IN, auszugebender Wert
 *
 * @retval             - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint16Hex(0xa5);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint16Hex(uint16_t x)
{
    uint16_t y = x >> 12;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( y < 10) serPutc(y+0x30);   // Bits 15..12
    else        serPutc(y+0x37);

    y = (x>>8) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 11..8
    else        serPutc(y+0x37);

    y = (x>>4) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 7..4
    else        serPutc(y+0x37);

    y = x & 0xF;
    if(y < 10) serPutc(y+0x30);   // Bits 3..0
    else       serPutc(y+0x37);
}







/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 16 Bit Zahl mit Vorzeichen im Dezimalsystem. Die
 *   Funktion wandelt ins Dezimalsystem mit Hilfe des Division und
 *   Modulo Algorithmus. Die Zeichen werden intern auf einem Stack
 *   gespeichert, um sie in umgekehrter Reihenfolge ausgeben zu koennen.
 *
 * @see
 * @arg
 *
 *
 * @param  y  - IN, auszugebender Wert
 *
 * @retval            - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutInt16Dec(-27351);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutInt16Dec(int16_t y)
{   unsigned char letter[5];
    int16_t n=0, i;
    int32_t x;

    x = y;
    serPutc(' ');
    if(x<0)
    {   serPutc('-');
        x = -x;
    }
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}





/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer vorzeichenlosen 32 Bit Zahl im Dezimalsystem. Die
 *   Funktion wandelt ins Dezimalsystem mit Hilfe des Division und
 *   Modulo Algorithmus. Die Zeichen werden intern auf einem Stack
 *   gespeichert, um sie in umgekehrter Reihenfolge ausgeben zu koennen.
 *
 * @see
 * @arg serOutInt16Dec(), serOutUint32Hex()
 *
 *
 * @param  x  - IN, zu druckender Wert
 *
 * @retval              - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint32Dec(56000);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint32Dec(uint32_t x)
{   unsigned char letter[10];
    int16_t n=0, i;

    serPutc(' ');
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}




/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 32 Bit Zahl ohne Vorzeichen im HEX-Format
 *
 * @see
 * @arg serOutInt32Dec(), serOutUint16Hex()
 *
 *
 * @param  x  - IN, auszugebender Wert
 *
 * @retval             - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutUint32Hex(0xa5fe);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutUint32Hex(uint32_t x)
{
    uint32_t y = x >> 28;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( y < 10) serPutc(y+0x30);   // Bits 31..28
    else        serPutc(y+0x37);

    y = (x>>24) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 27..24
    else        serPutc(y+0x37);

    y = (x>>20) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 23..20
    else        serPutc(y+0x37);

    y = (x>>16) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 19..16
    else        serPutc(y+0x37);

    y = (x>>12) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 15..12
    else        serPutc(y+0x37);

    y = (x>>8) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 11..8
    else        serPutc(y+0x37);

    y = (x>>4) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 7..4
    else        serPutc(y+0x37);

    y = x & 0xF;
    if(y < 10) serPutc(y+0x30);   // Bits 3..0
    else       serPutc(y+0x37);
}







/*!
 **********************************************************************
 * @par Beschreibung:
 *   Ausgabe einer 32 Bit Zahl mit Vorzeichen im Dezimalsystem. Die
 *   Funktion wandelt ins Dezimalsystem mit Hilfe des Division und
 *   Modulo Algorithmus. Die Zeichen werden intern auf einem Stack
 *   gespeichert, um sie in umgekehrter Reihenfolge ausgeben zu koennen.
 *
 * @see
 * @arg
 *
 *
 * @param  y  - IN, auszugebender Wert
 *
 * @retval            - kein Rueckgabewert
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ ...
  serInit(9600UL);
  serOutInt32Dec(-27351);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
void serOutInt32Dec(int32_t y)
{   unsigned char letter[10];
    int16_t n=0, i;
    int32_t x;

    x = y;
    serPutc(' ');
    if(x<0)
    {   serPutc('-');
        x = -x;
    }
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}




/*!
 **********************************************************************
 * @par Beschreibung:
 *   Einlesen eines Zeichens (1 Byte) von der seriellen Schnittstelle.
 *   Die Funktion blockiert, bis ein Zeichen vorhanden ist. Wenn das Terminalprogramm
 *   bei 'ENTER' ein 'CR' (0x0D) sendet, bleibt dieses Zeichen im Puffer und wird
 *   mit dem naechsten serGetc() oder serPollc() gelesen.
 *
 * @see
 * @arg  serPollc()
 *
 *
 * @param     - keine Parameter
 *
 * @retval                - das empfangene Zeichen (8 Bit)
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ int x;
  serInit(9600UL);
  x = serGetc();
  serOutInt16Dec(x);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
uint8_t serGetc(void)
{   uint8_t x;

    x=getchar();
    #if SER_ECHO
        serPutc(x);               // gerade gelesenes Byte als Echo senden
    #endif
    return x;
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Pollt den Empfaenger und liest ggf. ein Zeichen ein.
 *   Diese Funktion blockiert nicht.
 *
 * @see
 * @arg  serGetc()
 *
 *
 * @param    keine
 *
 * @retval   ASCII-Code des Zeichens oder -1 falls kein Zeichen vorhanden.
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ int x;
  serInit(9600UL);
  x = serPollc();
  serOutInt16Dec(x);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
int16_t serPollc(void)
{   int16_t x=0;

    x = _pollSerialInterface();  // siehe read.c

    #if SER_ECHO
        if(x>=0) serPutc(x);  // gerade gelesenes Byte als Echo senden
    #endif
    return x;
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Liest einen String von der seriellen Schnittstelle. Das Terminalprogramm
 *   muss die Eingabe mit CR abschliessen. Die Funktion liest blockierend, bis ein
 *   CR empfangen wird. Alle Zeichen, ausser CR werden in die Zeichenkette pt kopiert,
 *   am Ende wird '\0' eingetragen.
 *
 * @see
 * @arg  serGetc()
 *
 *
 * @param    Zeiger auf den String
 *
 * @retval   Anzahl der gelesenen Zeichen, ohne '\0'.
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ char buf[80];
  short anz;
  serInit(9600UL);
  anz = serGets(buf);
  serOutInt16Dec(anz);
  serPuts(buf);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
uint8_t serGets(char *pt)
{	uint8_t c;
    uint8_t count=0;

    c=serGetc();
    while(c != 0x0D)  // CR empfangen?
    {
    	*pt = c;
    	pt++;
    	count++;
    	c=serGetc();
    }
    // CR empfangen, Stringende eintragen:
    *pt = '\0';
	return count;
}





/*!
 **********************************************************************
 * @par Beschreibung:
 *   Liest blockierend eine 16 Bit Zahl ohne Vorzeichen von der
 *   seriellen Schnittstelle im Dezimalsystem.
 *   Die Eingabe wird beendet wenn der Zahlenwert zu gross wird (>65535)
 *   oder <ENTER> empfangen wurde. Bei manchen Terminalprogrammen (z.B. HTerm)
 *   muss man extra einstellen, dass bei 'ENTER' das Zeichen CR (carriage return)
 *   gesendet wird. Fehlt dieses Zeichen, kann diese Funktion sich nicht beenden!
 *
 *
 * @see
 * @arg  serInInt16Dec()
 *
 *
 * @param  - IN/OUT Zeiger auf uint16_t Variable
 *
 * @retval 0       - ok,
 * @retval -1      - Error, es konnte keine Zahl decodiert werden
 * @retval 1       - Error, Zahlenwert zu gross (Ueberlauf)
 *
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ unsigned int x;
  serInit(9600UL);
  serInUint16Dec(&x);
  serOutUint16Dec(x);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
int8_t serInUint16Dec(uint16_t *x)
{
    uint8_t letter, err=0, n=0;
    uint32_t value=0;

    while(1)
    {   letter = serGetc();
        if((letter>='0') && (letter<='9'))  // Ziffer empfangen?
        {   value = value*10 + (letter - '0');
            n++;
            if(value > 65535) err=1;  // Ueberlauf
        }
        else  // keine Ziffer empfangen, Ende
        {   if((n==0)&&(letter<' ')){ return -1;} // Error, nichts gelesen
            if((n!=0)&&(err==1))    { return  1;} // Error, Ueberlauf
            if((n!=0)&&(err==0)){*x=value; return  0;} // ok
        }
    }
    return 0; // wird nie erreicht, vermeidet aber ein warning...
}



/*!
 **********************************************************************
 * @par Beschreibung:
 *   Liest blockierend eine 16 Bit Zahl mit Vorzeichen von der
 *   seriellen Schnittstelle im Dezimalsystem.
 *   Die Eingabe wird beendet wenn der Zahlenwert zu gross/klein wird
 *   oder <ENTER> empfangen wurde. Bei manchen Terminalprogrammen (z.B. HTerm)
 *   muss man extra einstellen, dass bei 'ENTER' das Zeichen CR (carriage return)
 *   gesendet wird. Fehlt dieses Zeichen, kann diese Funktion sich nicht beenden!
 *
 * @see
 * @arg  serInUint16Dec()
 *
 *
 * @param  - IN/OUT Zeiger auf int16_t Variable
 *
 * @retval 0       - ok,
 * @retval -1      - Error, es konnte keine Zahl decodiert werden
 * @retval 1       - Error, Ueberlauf/Unterlauf
 *
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ int x;
  serInit(9600UL);
  serInInt16Dec(&x);
  serOutInt16Dec(x);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
int8_t serInInt16Dec(int16_t *x)
{
    uint8_t letter, err=0, n=0, neg=0;
    int32_t value=0;

    while(1)
    {   letter = serGetc();  // ev. negatives Vorzeichen
        if(letter == '+') {letter = serGetc();}         // Ignoriere '+'
        if(letter == '-') {letter = serGetc(); neg=1;}  // negative Zahl
        if((letter>='0') && (letter<='9'))       // Ziffer empfangen?
        {   value = value*10 + (letter - '0');
            n++;
            if((value>32767) && (!neg)) err=1;  // Overflow, pos. Zahl
            if((value>32768) &&   neg ) err=1;  // Underflow, neg. Zahl
        }
        else  // keine Ziffer empfangen, Ende
        {   if((n==0)&&(letter<' ')){ return -1;} // Error, kein Wert gelesen
            if((n!=0)&&(err==1))    { return  1;} // Error, Overflow/Underflow
            if((n!=0)&&(err==0))  // ok, Ende gefunden, Vorzeichen beachten
            {   if(neg) {value = -value;}
                *x=value;
                return  0;
            }
        }
    }
    return 0; // wird nie erreicht, aber vermeidet ggf. ein warning
}




/*!
 **********************************************************************
 *   Liest blockierend eine 16 Bit Zahl ohne Vorzeichen von der
 *   seriellen Schnittstelle im Hex-System. Die Hex-Zahl muss mit "0x"
 *   beginnen.
 *   Die Eingabe wird beendet wenn der Zahlenwert zu gross wird
 *   oder <ENTER> empfangen wurde. Bei manchen Terminalprogrammen (z.B. HTerm)
 *   muss man extra einstellen, dass bei 'ENTER' das Zeichen CR (carriage return)
 *   gesendet wird. Fehlt dieses Zeichen, kann diese Funktion sich nicht beenden!
 *
 * @see
 * @arg  serInUint16Dec()
 *
 *
 * @param  - IN/OUT Zeiger auf uint16_t Variable
 *
 * @retval 0       - ok,
 * @retval -1      - Error, es konnte keine Zahl decodiert werden
 * @retval 1       - Error, Ueberlauf
 *
 *
 *
 * @par Beispiel :
 * @verbatim

int main(void)
{ unsigned int x;
  serInit(9600UL);
  serInUint16Hex(&x);
  serOutUint16Dec(x);
  ...
  return 0;
}
  @endverbatim
 ************************************************************************/
int8_t serInUint16Hex(uint16_t *x)
{
    uint8_t letter;
    int32_t val=0;

    if(serGetc() != '0') return -1;  // Error, sollte mit 0x starten
    letter = serGetc();
    if((letter != 'x') && (letter !='X')) return -1;

    while(1)
    {   letter = serGetc();
        if(letter < ' ')  {*x = val; return 0;}  // ok, Ende der Zahl
        if((letter >= '0') && (letter <= '9')){val = (val<<4)+(letter-0x30);}
        if((letter >= 'A') && (letter <= 'F')){val = (val<<4)+(letter-0x37);}
        if((letter >= 'a') && (letter <= 'f')){val = (val<<4)+(letter-0x57);}
        if(val > 65535) return 1;  // Error, Overflow
    }
    return 0; // wird nie erreicht, vermeidet aber ggf. ein warning
}


#endif // COS_PLATFORM
/**************************************************************************
*   END OF:    Renesas RX63N Verion                                       *
**************************************************************************/








/**************************************************************************
*      openCM9.04  Verion                                               *
**************************************************************************/

#if COS_PLATFORM == PLATFORM_OPEN_CM_9_04



/*!
 **********************************************************************
 * @par Description:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 ************************************************************************/
void serInit(uint32_t baudRate)
{
}





/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @see
 * @arg  serPuts()
 *
 *
 * @param  x  - IN, character to be sent
 *

 ************************************************************************/
void serPutc(char x)
{

}




/*!
 **********************************************************************
 * @par Description:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @see
 * @arg  serPutc()
 *
 *
 * @param  pt  - IN, pointer to string
 *
 ************************************************************************/
void serPuts(char *pt)
{
  while(*pt != '\0')
  {  serPutc(*pt);
     pt++;
  }
}




/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint8Bin(uint8_t x)
{   uint8_t i;

    serPuts(" 0b");  // Praefix fuer Binaerzahl ist 0b
    for(i=0; i<8; i++)
    {   serPutc((x>>7) + '0');
        x <<= 1;
    }
}



/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint8Hex(uint8_t x)
{
    uint8_t y = x >> 4;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( (y) < 10) serPutc(y+0x30);  // high nibble
    else          serPutc(y+0x37);
    y = x & 0x0f;
    if( (y) < 10) serPutc(y+0x30);  // low nibble
    else          serPutc(y+0x37);
}


/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint16Dec(uint16_t x)
{   unsigned char letter[5];
    int16_t n=0, i;

    serPutc(' ');
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}


/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint16Hex(uint16_t x)
{
    uint16_t y = x >> 12;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( y < 10) serPutc(y+0x30);   // Bits 15..12
    else        serPutc(y+0x37);

    y = (x>>8) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 11..8
    else        serPutc(y+0x37);

    y = (x>>4) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 7..4
    else        serPutc(y+0x37);

    y = x & 0xF;
    if(y < 10) serPutc(y+0x30);   // Bits 3..0
    else       serPutc(y+0x37);
}







/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  y  - IN, value to be printed
 ************************************************************************/
void serOutInt16Dec(int16_t y)
{   unsigned char letter[5];
    int16_t n=0, i;
    int32_t x;

    x = y;
    serPutc(' ');
    if(x<0)
    {   serPutc('-');
        x = -x;
    }
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}



/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint32Dec(uint32_t x)
{   unsigned char letter[10];
    int16_t n=0, i;

    serPutc(' ');
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}




/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  x  - IN, value to be printed
 ************************************************************************/
void serOutUint32Hex(uint32_t x)
{
    uint32_t y = x >> 28;

    serPuts(" 0x");  // Hex Praefix ist 0x
    if( y < 10) serPutc(y+0x30);   // Bits 31..28
    else        serPutc(y+0x37);

    y = (x>>24) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 27..24
    else        serPutc(y+0x37);

    y = (x>>20) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 23..20
    else        serPutc(y+0x37);

    y = (x>>16) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 19..16
    else        serPutc(y+0x37);

    y = (x>>12) & 0xF;
    if( y < 10) serPutc(y+0x30);   // Bits 15..12
    else        serPutc(y+0x37);

    y = (x>>8) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 11..8
    else        serPutc(y+0x37);

    y = (x>>4) & 0xF;
    if( y < 10) serPutc(y+0x30);  // Bits 7..4
    else        serPutc(y+0x37);

    y = x & 0xF;
    if(y < 10) serPutc(y+0x30);   // Bits 3..0
    else       serPutc(y+0x37);
}







/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param  y  - IN, value to be printed
 ************************************************************************/
void serOutInt32Dec(int32_t y)
{   unsigned char letter[10];
    int16_t n=0, i;
    int32_t x;

    x = y;
    serPutc(' ');
    if(x<0)
    {   serPutc('-');
        x = -x;
    }
    do
    {   letter[n++] = (x % 10) + 0x30;
        x /= 10;
    } while(x != 0);
    for(i=n-1; i>=0; i--)
    {   serPutc(letter[i]); // Ausgabe in umgekehrter Reihenfolge
    }
}




/*!
 **********************************************************************
 * @par Description
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
   @par Code example:

  @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
 ************************************************************************/
uint8_t serGetc(void)
{
    return 0;
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
   @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
 ************************************************************************/
int16_t serPollc(void)
{   return 0;
}


/*!
 **********************************************************************
 * @par Beschreibung:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param pt -IN, pointer to string
 *
   @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
************************************************************************/
uint8_t serGets(char *pt)
{	uint8_t c;
    uint8_t count=0;

    c=serGetc();
    while(c != 0x0D)  // CR empfangen?
    {
    	*pt = c;
    	pt++;
    	count++;
    	c=serGetc();
    }
    // CR empfangen, Stringende eintragen:
    *pt = '\0';
	return count;
}





/*!
 **********************************************************************
 * @par Beschreibung:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param x -IN, pointer to variable
 *
   @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
************************************************************************/
int8_t serInUint16Dec(uint16_t *x)
{
    uint8_t letter, err=0, n=0;
    uint32_t value=0;

    while(1)
    {   letter = serGetc();
        if((letter>='0') && (letter<='9'))  // Ziffer empfangen?
        {   value = value*10 + (letter - '0');
            n++;
            if(value > 65535) err=1;  // Ueberlauf
        }
        else  // keine Ziffer empfangen, Ende
        {   if((n==0)&&(letter<' ')){ return -1;} // Error, nichts gelesen
            if((n!=0)&&(err==1))    { return  1;} // Error, Ueberlauf
            if((n!=0)&&(err==0)){*x=value; return  0;} // ok
        }
    }
    return 0; // wird nie erreicht, vermeidet aber ein warning...
}




/*!
 **********************************************************************
 * @par Beschreibung:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param x -IN, pointer to variable
 *
   @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
************************************************************************/
int8_t serInInt16Dec(int16_t *x)
{
    uint8_t letter, err=0, n=0, neg=0;
    int32_t value=0;

    while(1)
    {   letter = serGetc();  // ev. negatives Vorzeichen
        if(letter == '+') {letter = serGetc();}         // Ignoriere '+'
        if(letter == '-') {letter = serGetc(); neg=1;}  // negative Zahl
        if((letter>='0') && (letter<='9'))       // Ziffer empfangen?
        {   value = value*10 + (letter - '0');
            n++;
            if((value>32767) && (!neg)) err=1;  // Overflow, pos. Zahl
            if((value>32768) &&   neg ) err=1;  // Underflow, neg. Zahl
        }
        else  // keine Ziffer empfangen, Ende
        {   if((n==0)&&(letter<' ')){ return -1;} // Error, kein Wert gelesen
            if((n!=0)&&(err==1))    { return  1;} // Error, Overflow/Underflow
            if((n!=0)&&(err==0))  // ok, Ende gefunden, Vorzeichen beachten
            {   if(neg) {value = -value;}
                *x=value;
                return  0;
            }
        }
    }
    return 0; // wird nie erreicht, aber vermeidet ggf. ein warning
}





/*!
 **********************************************************************
 * @par Beschreibung:
 *   Empty function provided for compatibility, on openCM use 'SerialUSB' instead.
 *
 * @param x -IN, pointer to variable
 *
   @verbatim
void setup()
{ SerialUSB.begin();
}

byte temp = 0;

void loop()
{   if ( SerialUSB.available() )
    {
        temp = SerialUSB.read();
        SerialUSB.print(temp);
    }
}
  @endverbatim
************************************************************************/
int8_t serInUint16Hex(uint16_t *x)
{
    uint8_t letter;
    int32_t val=0;

    if(serGetc() != '0') return -1;  // Error, sollte mit 0x starten
    letter = serGetc();
    if((letter != 'x') && (letter !='X')) return -1;

    while(1)
    {   letter = serGetc();
        if(letter < ' ')  {*x = val; return 0;}  // ok, Ende der Zahl
        if((letter >= '0') && (letter <= '9')){val = (val<<4)+(letter-0x30);}
        if((letter >= 'A') && (letter <= 'F')){val = (val<<4)+(letter-0x37);}
        if((letter >= 'a') && (letter <= 'f')){val = (val<<4)+(letter-0x57);}
        if(val > 65535) return 1;  // Error, Overflow
    }
    return 0; // wird nie erreicht, vermeidet aber ggf. ein warning
}


#endif // COS_PLATFORM
/**************************************************************************
*  END OF:    openCM9.04  Verion                                               *
**************************************************************************/





