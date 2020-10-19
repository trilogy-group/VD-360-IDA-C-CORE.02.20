#ifndef IdaCpd_h
#define IdaCpd_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaCpd.h 1.1
//
//   File:      IdaCpd.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:55
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------


static const char * SCCS_Id_IdaCpd_h = "@(#) IdaCpd.h 1.1";



////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	Immer, wenn wir einen DOMString mit der Methode transcode() in einen
	char* wandeln, bekommen wir eine Kopie zurück, die wir nach Verwendung
	wieder löschen müssen ! Deshalb kapseln wir den Aufruf in den Konstruktor-
	Aufruf der Klasse <code>Cdp</code>. Die Objekte der Klasse werde dann auf dem
	Stack abgelegt und automatisch aufgeräumt. Dabei wird eben auch der
	Destruktor aufgerufen, der den Speicher wieder frei gibt.<br><br>

	Beispiel:<br>
<pre>
		DOMString childNodeName;
		if (0 == strcmp(CPD(childNodeName.transcode()), "Search"))
</pre>
                        ----						 -

	childNodeName.transcode() liefert Speicher mit dem Typ "char*" zurück.
	"CPD(childNodeName.transcode())" erzeugt ein temporäres Object vom Typ "CPD",
	dass den Pointer übernimmt. Der Cast-Operator von "CPD" liefert für strcmp(...)
	wieder denn richtigen Typ (char*). Nachdem die Zeile abgearbeitet ist wird das
	"anonyme" Objekt wieder bereinigt. Der automatische Destruktor-Aufruf löscht
	dabei den Speicher des Strings, den "transcode()" lieferte.

*/
class Cpd
{
	private:
		char* pChar;
	
	public:
//		Cpd() { pChar = 0; };
		Cpd(char*& cp) { pChar = cp; };

		~Cpd() { if (pChar) delete [] pChar; pChar = 0; };

//		char* operator char*() { return pChar; };		// für HPUX
		operator char*() { return pChar; };
};


#endif	// IdaCpd_h


// *** EOF ***


