//***********************************************************************
//  gemctrl application
//
//	simple app to access and control a gemini telescope mount controller
//
//
//***********************************************************************


#include "wx/wx.h"


//----------------------------------------------
//  DECLARATIONS
//----------------------------------------------
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
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

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Connect\tCtrl-C",
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
	SetStatusText("Welcome to gemctrl!");
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

	//***************
	//  Find Gemini via DNS naming
	//

	//**************
	//  Get status & display
	//


	wxLogMessage("Hello world from gemctrl!");
	SetStatusText("Connected to Gemini.");


}  // end of Connect
