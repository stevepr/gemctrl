//***********************************************************************
//  gemini class implementation
//
//	class to communicate with Gemini telescope controller over UDP using wxWidgets
//
//
//***********************************************************************

#include "gemini.h"

//++++++++++++++++++++++++++++
//  constructor
//++++++++++++++++++++++++++++
gemini::gemini(wxDatagramSocket *sock_in, wxIPV4address *ipGemini_in)
	: numDatagram{0},					// start with datagram #0
	gemPort{11110}					// gemini port defaults to 11110
{
	sock = sock_in;
	addr = ipGemini_in;

} // end of constructor

//++++++++++++++++++++++++++++
//  destructor
//++++++++++++++++++++++++++++
gemini::~gemini()
{
	
} // end of destructor


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  getLocalDateTime - return local datetime from Gemini
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool gemini::getLocalDateTime( wxDateTime *dteGemini)
{
	char msgResponse[255];
	int retval;

	retval = sendCommand(":GC#", msgResponse);

	return true;

} // end of getLocalDateTime

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// sendCommand - send a command and return the response
//    return value is >= 0 for success = # of bytes returned
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
int gemini::sendCommand(wxString strCommand, char *strResponse)
{
	char msgBuffer[100];
	int numBytes;
	PUINT32 pNum = (PUINT32) msgBuffer;
	PUINT32 plastNum = (PUINT32) (msgBuffer + 4);
	char *pData = msgBuffer + 8;


	//*****************************
	// construct datagram to send to gemini
	//
	*pNum = numDatagram;							// set datagram number for this command
	*plastNum = 0;									// always 0 for outgoing command
	strcpy(pData, strCommand.ToAscii());			// copy in command
	numBytes = 8 + strCommand.Len() + 1;
	
	//******************************
	// send datagram command to gemini
	//
	sock->SendTo(*addr, msgBuffer, numBytes);
	if (sock->Error())
	{
		return -1;
	}

	//********************************
	// wait for response from gemini
	//
	numBytes = sock->RecvFrom(*addr, strResponse, sizeof(msgBuffer)).LastCount();
	if (sock->Error())
	{
		return -2;
	}


	//***********************************
	// extract response string and return
	//
	return numBytes;

} // end of sendCommand



//*********** END OF MODULE ****************************