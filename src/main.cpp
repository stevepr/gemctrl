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

//=======================================
//  INTERFACE:  MyApp
//=======================================
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

//=======================================
//  INTERFACE:  MyFrame
//=======================================
class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual void OnClose(wxCloseEvent& event);
	
private:

	// local variables
	//
	bool blnFound;
	int iError;
	gemini *cGem;
	wxDatagramSocket *sock;
	wxIPV4address ipGemini;
	wxIPV4address ipLocal;
	unsigned short gemPort;		// UDP port for gemini commands

	wxTimer m_timer;
	int ExecCommand;
	wxDateTime dteExec;			// Time to execute a command (LOCAL timezone)

	// events
	//
	void OnConnect(wxCommandEvent& event);		// Connect to Gemini
	void OnExit(wxCommandEvent& event);			// Disconnect and Exit
	void OnAbout(wxCommandEvent& event);

	void OnSetTimer(wxCommandEvent& event);
	void OnTimerEvent(wxTimerEvent& event);

	wxDECLARE_EVENT_TABLE();
};

//----------------------------
// MyFrame: EVENT TABLE
//----------------------------
enum
{
	ID_SET_TIMER = 1,
	ID_TIMER_EVENT = 2
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(wxID_OPEN, MyFrame::OnConnect)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(ID_SET_TIMER, MyFrame::OnSetTimer)
	EVT_TIMER(ID_TIMER_EVENT, MyFrame::OnTimerEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

//============================================
//  IMPLEMENTATION: MyApp
//============================================
bool MyApp::OnInit()
{

	MyFrame *frame = new MyFrame("Gemini Controller", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	return true;
}
int MyApp::OnExit()
{
	return(0);
}

//============================================
//  IMPLEMENTATION MyFrame
//============================================
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size),
	m_timer(this,ID_TIMER_EVENT)
{


	// UI
	//
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Connect\tCtrl-C",
		"Connect to Gemnini");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenu *menuActions = new wxMenu;
	menuActions->Append(ID_SET_TIMER,"&Set Timer");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&Gemini");
	menuBar->Append(menuActions, "&Actions");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();

	// setup timer
	//
	ExecCommand = -1;		// no command now
	m_timer.Start(500);				// fire timer every 1/2 second

} // end of constructor/init


//---------------------------------------
//  MyFrame: EVENT HANDLERS
//---------------------------------------

//*******************
// Event: OnClose
//*******************
void MyFrame::OnClose(wxCloseEvent& event)
{

	// clean up...
	//
	wxDELETE(cGem);
	wxDELETE(sock);

	wxSocketBase::Shutdown();

} // end of close event

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
	wxString strResult;
	wxString strHostname;
	wxTextEntryDialog dlgHost(this, "Enter hostname of Gemini :", "Connect to Gemini", "gemini", wxOK | wxCANCEL);

	//***************
	//  Find Gemini via DNS naming and setup socket
	//
	if (!(dlgHost.ShowModal() == wxID_OK))
	{
		return;
	}
	strHostname = dlgHost.GetValue();

	if (!ipGemini.Hostname(strHostname))
	{
		blnFound = false;
		iError = 0;
	}
	else
	{
		// setup UDP port
		//
		gemPort = 11110;
		if (!ipGemini.Service(gemPort))
		{
			blnFound = false;
			iError = 1;
		}
		else
		{
			blnFound = true;		// found it without errors

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
			cGem = new gemini(sock, &ipGemini);
		}
	}

	if (blnFound)
	{
		strResult.Printf("Gemini found at %s", ipGemini.IPAddress());
	}
	else
	{
		strResult = "Gemini NOT found! " + wxString::Format("%i", iError);
	}
	SetStatusText(strResult);

}  // end of Connect

//*******************
// Event: OnSetTimer
//*******************
void MyFrame::OnSetTimer(wxCommandEvent& event)
{
	wxString strNow;
	wxString::const_iterator end;
	wxString strExec;
	wxDateTime dteInput;

	wxTextEntryDialog dlgTime(this,"Enter UTC time to Start tracking (yyyy-mm-dd hh:mm:ss) :","Start Time","yyyy-mm-dd hh:mm:ss", wxOK | wxCANCEL);

	strNow = wxDateTime::UNow().Format("%Y-%m-%d %H:%M:%S",wxDateTime::UTC);
	dlgTime.SetValue(strNow);

	if (!(dlgTime.ShowModal() == wxID_OK))
	{
		return;
	}
	strExec = dlgTime.GetValue();

	// now parse new UTC value
	//
	if (!dteInput.ParseFormat(strExec, "%Y-%m-%d %H:%M:%S", &end))
	{
		wxMessageBox(strExec, "Error parsing Date/Time");
		return;
	}
	else if (end != strExec.end())
	{
		wxMessageBox(strExec, "Error parsing Date/Time");
		return;
	}
	dteExec = dteInput.FromTimezone(wxDateTime::UTC);

	// enable timer for WakeUp
	//
	ExecCommand = 0;

	// update status bar
	//
	strExec = dteInput.Format(" WakeUp Command will be sent at %Y-%m-%d %H:%M:%S UTC") + dteExec.Format(", %Y-%m-%d %H:%M:%S Local Time");
	SetStatusText(strExec);

} // end of OnSetTimer

//*******************
// Event: OnTimerEvent
//*******************
void MyFrame::OnTimerEvent(wxTimerEvent& event)
{
	wxDateTime dteNow;
	wxString strExec;

	// do we have a pending command?
	//
	if (ExecCommand < 0)
	{
		return;	// no
	}

	// have we passed the time to execute our command?
	//
	dteNow = wxDateTime::Now();
	if (dteNow >= dteExec)
	{
		// yes...
		//
		cGem->WakeUp();

		// done... no more commands to send
		//
		ExecCommand = -1;
		strExec = dteNow.Format(" WakeUp sent at %Y-%m-%d %H:%M:%S UTC",wxDateTime::UTC);
		SetStatusText(strExec);
	}

} // end of OnTimerEvent