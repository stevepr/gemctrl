//=================================================================
//  Gemini Class Interface
//=================================================================
#pragma once

#include "wx/wx.h"
#include "wx/socket.h"

//+++++++++++++++++++++++++++++++++++
//  Class interface
//+++++++++++++++++++++++++++++++++++
class gemini
{
public:
	gemini(wxDatagramSocket *sock, wxIPV4address *ipGemini);
	~gemini();

	//------------------
	// getLocalDateTime
	//------------------
	bool getLocalDateTime( wxDateTime *dteGemini );

	//------------------
	// Wake-up
	//------------------
	bool WakeUp();

	//------------------
	//  send a command
	//------------------
	int sendCommand(wxString strCommand, char *strResponse);

private:
	wxDatagramSocket *sock;
	wxIPV4address *addr;
	unsigned short gemPort;		// UDP port for gemini commands

	uint32_t numDatagram;			// current datagram number

};

