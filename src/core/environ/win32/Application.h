
#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__

#include <vector>
#include <map>
#include "tstring.h"

//tstring ParamStr( int index );
tstring ExePath();

// 後で見通しのよう方法に変更する
extern int _argc;
extern char ** _argv;

enum {
	mrOk,
	mrAbort,
	mrCancel,
};

class AcceleratorKey {
	HACCEL hAccel_;
	ACCEL* keys_;
	int key_count_;

public:
	AcceleratorKey();
	~AcceleratorKey();
	void AddKey( WORD id, WORD key, BYTE virt );
	void DelKey( WORD id );
	HACCEL GetHandle() { return hAccel_; }
};
class AcceleratorKeyTable {
	std::map<HWND,AcceleratorKey*> keys_;
	HACCEL hAccel_;

public:
	AcceleratorKeyTable();
	~AcceleratorKeyTable();
	void AddKey( HWND hWnd, WORD id, WORD key, BYTE virt );
	void DelKey( HWND hWnd, WORD id );
	void DelTable( HWND hWnd );
	HACCEL GetHandle(HWND hWnd) {
		std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
		if( i != keys_.end() ) {
			return i->second->GetHandle();
		}
		return hAccel_;
	}
};
class TApplication {
	std::vector<class TTVPWindowForm*> windows_list_;
	tstring title_;

	bool is_attach_console_;
	FILE* oldstdin_;
	FILE* oldstdout_;
	tstring console_title_;
	AcceleratorKeyTable accel_key_;

public:
	TApplication() : is_attach_console_(false) {}
	void CheckConsole();
	void CloseConsole();
	bool IsAttachConsole() { return is_attach_console_; }
	void CheckDigitizer();

	HWND GetHandle();
	bool IsIconic() {
		HWND hWnd = GetHandle();
		if( hWnd != INVALID_HANDLE_VALUE ) {
			return 0 != ::IsIconic(hWnd);
		}
		return true; // そもそもウィンドウがない
	}
	void Minimize();
	void Restore();
	void BringToFront();

	void AddWindow( class TTVPWindowForm* win ) {
		windows_list_.push_back( win );
	}
	void RemoveWindow( class TTVPWindowForm* win );

	void ProcessMessages();
	void HandleMessage();

	tstring GetTitle() const { return title_; }
	void SetTitle( const tstring& caption );

	static inline int MessageDlg( const tstring& string, const tstring& caption, int type ) {
		::MessageBox( NULL, string.c_str(), caption.c_str(), type  );
	}
	void Terminate() {
		::PostQuitMessage(0);
	}
	void SetHintHidePause( int v ) {}
	void SetShowHint( bool b ) {}
	void SetShowMainForm( bool b ) {}

	void Initialize() {}
	void ShowException( class Exception* e );
	void Run();

	HWND GetMainWindowHandle();

	int ArgC;
	char ** ArgV;
	std::vector<std::string> CommandLines;

	void PostMessageToMainWindow(UINT message, WPARAM wParam, LPARAM lParam);


	void ModalStarted() {}
	void ModalFinished() {}
	void DisableWindows();
	void EnableWindows( const std::vector<class TTVPWindowForm*>& ignores );
	void GetDisableWindowList( std::vector<class TTVPWindowForm*>& win );

	
	void RegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd);
	void UnregisterAcceleratorKey(HWND hWnd, short cmd);
	void DeleteAcceleratorKeyTable( HWND hWnd );
};
std::vector<std::string>* LoadLinesFromFile( const tstring& path );

// スタブ、正しくは動作しないはず。
inline HWND AllocateHWnd( LRESULT (CALLBACK *UtilWndProc)(HWND,UINT,WPARAM,LPARAM) ) {
	
	const TCHAR* classname = _T("TPUtilWindow");
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, UtilWndProc, 0L, 0L,
						GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
						classname, NULL };
	::RegisterClassEx( &wc );
	HWND hWnd = ::CreateWindow( classname, _T(""),
						WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
						NULL, NULL, wc.hInstance, NULL );
	return hWnd;
}
inline bool DeallocateHWnd( HWND hWnd ) {
	return 0 != ::DestroyWindow( hWnd );
}


inline HINSTANCE GetHInstance() { return ((HINSTANCE)GetModuleHandle(0)); }
extern class TApplication* Application;


#endif // __T_APPLICATION_H__
