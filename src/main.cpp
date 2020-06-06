//***********************************************************************
//  gemctrl application
//
//	simple app to access and control a gemini telescope mount controller
//
//
//***********************************************************************


#include "wx/wx.h"
#include "wx/socket.h"
#include "gemini.h"

//----------------------------------------------
//  INTERFACES
//----------------------------------------------
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	
private:

	// local variables
	//
	bool blnFound;
	gemini *cGem;
	wxDatagramSocket *sock;
	wxIPV4address ipGemini;
	wxIPV4address ipLocal;
	unsigned short gemPort;		// UDP port for gemini commands

	// events
	//
	void OnConnect(wxCommandEvent& event);		// Connect to Gemini
	void OnExit(wxCommandEvent& event);			// Disconnect and Exit
	void OnAbout(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

//----------------------------
// EVENT TABLE
//----------------------------
enum
{
	ID_Hello = 1
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(wxID_OPEN, MyFrame::OnConnect)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

//----------------------------------------
//  METHODS
//----------------------------------------
bool MyApp::OnInit()
{

	MyFrame *frame = new MyFrame("Gemini Controller", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	return true;
}
int MyApp::OnExit()
{
	wxSocketBase::Shutdown();
	return(0);
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxString strResult;

	//***************
	//  Find Gemini via DNS naming and setup socket
	//
	if (!ipGemini.Hostname("gemini"))
	{
		blnFound = false;
	}
	else
	{
		// setup UDP port
		//
		gemPort = 11110;
		if (!ipGemini.Service(gemPort))
		{
			blnFound = false;
		}
		else
		{
			// setup local ip
			//
			ipLocal.AnyAddress();
			ipLocal.Service(gemPort);

			// setup the socket
			//
			sock = new wxDatagramSocket(ipLocal, wxSOCKET_NONE);
			sock->SetTimeout(5);		// 5 second timeouts

			// init the gemini object
			//
			cGem = new gemini(sock,&ipGemini);

		}
	}


	// UI
	//
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Connect\tCtrl-C",
		"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&Gemini");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();

	if (blnFound)
	{
		strResult.Printf("Gemini found at %s", ipGemini.IPAddress());
	}
	else
	{
		strResult = "Gemini NOT found!";
	}
	SetStatusText(strResult);
}


//---------------------------------------
//  EVENT HANDLERS
//---------------------------------------

//*******************
// Event: OnExit
//*******************
void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

//*********************
//  Event: OnAbout
//*********************
void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a Gemini Mount Control program",
		"About gemctrl", wxOK | wxICON_INFORMATION);
}

//**********************
//  Event: OnConnect
//**********************
void MyFrame::OnConnect(wxCommandEvent& event)
{
	wxIPV4address addr;
	wxDateTime *dteLocal;


	//*****************
	//  get local date
	//
	dteLocal = new wxDateTime();
	cGem->getLocalDateTime(dteLocal);

	//**************
	//  Update Status
	//
	SetStatusText("");


}  // end of Connect
