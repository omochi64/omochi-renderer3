#include <Windows.h>
#include "WindowViewer.h"
#include <unordered_map>
#include "renderer/Camera.h"

using namespace std;

namespace {
  wstring widen(const std::string &src) {
	  wchar_t *wcs = new wchar_t[src.length() + 1];
	  mbstowcs(wcs, src.c_str(), src.length() + 1);
	  wstring dest = wcs;
	  delete [] wcs;
    return dest;
  }
}

namespace OmochiRenderer {
  class WindowViewer::WindowImpl {
    WindowViewer &viewer;
    static unordered_map<size_t, WindowImpl *> handleToInstance;

  public:
    explicit WindowImpl(WindowViewer &viewer_)
      : viewer(viewer_)
    {
    }

    bool CreateNewWindow() {
      HWND hWnd = CreateNewWindow_impl();
      if (hWnd != INVALID_HANDLE_VALUE) {
        RegisterWindow(hWnd, this);
        return true;
      }
      return false;
    }

    static void MessageLoop() {
      // メッセージループ
      MSG msg;
	    while(true)
	    {
		    BOOL ret = GetMessage( &msg, NULL, 0, 0 );  // メッセージを取得する
		    if( ret == 0 || ret == -1 )
		    {
			    // アプリケーションを終了させるメッセージが来ていたら、
			    // あるいは GetMessage() が失敗したら( -1 が返されたら ）、ループを抜ける
			    break;
		    }
		    else
		    {
			    // メッセージを処理する
			    TranslateMessage( &msg );
			    DispatchMessage( &msg );
		    }
	    }

    }

  private:
    HWND CreateNewWindow_impl() {
      WNDCLASSEX wc;
      HWND hWnd;
      HINSTANCE hInst = static_cast<HINSTANCE>(GetModuleHandle(NULL));

      // ウィンドウクラスの情報を設定
      wc.cbSize = sizeof(wc);               // 構造体サイズ
      wc.style = CS_HREDRAW | CS_VREDRAW;   // スタイル
      wc.lpfnWndProc = WindowImpl::WndProc;             // ウィンドウプロシージャ
      wc.cbClsExtra = 0;                    // 拡張情報１
      wc.cbWndExtra = 0;                    // 拡張情報２
      wc.hInstance = hInst;                 // インスタンスハンドル
      wc.hIcon = (HICON)LoadImage(          // アイコン
	      NULL, MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON,
	      0, 0, LR_DEFAULTSIZE | LR_SHARED
      );
      wc.hIconSm = wc.hIcon;                // 子アイコン
      wc.hCursor = (HCURSOR)LoadImage(      // マウスカーソル
	      NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
	      0, 0, LR_DEFAULTSIZE | LR_SHARED
      );
      wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ウィンドウ背景
      wc.lpszMenuName = NULL;                     // メニュー名
      // ugly code
      wc.lpszClassName = widen(viewer.m_windowTitle.c_str()).c_str();// ウィンドウクラス名

      // ウィンドウクラスを登録する
      if( RegisterClassEx( &wc ) == 0 ){ return (HWND)INVALID_HANDLE_VALUE; }

      int width = viewer.m_camera.GetScreenWidth();
      int height = viewer.m_camera.GetScreenHeight();

      int x = ( GetSystemMetrics( SM_CXSCREEN ) - width ) / 2;
      int y = ( GetSystemMetrics( SM_CYSCREEN ) - height ) / 2;

      // ウィンドウを作成する
      hWnd = CreateWindow(
	      wc.lpszClassName,      // ウィンドウクラス名
        widen(viewer.m_windowTitle.c_str()).c_str(),  // タイトルバーに表示する文字列
	      WS_OVERLAPPEDWINDOW,   // ウィンドウの種類
	      x,         // ウィンドウを表示する位置（X座標）
	      y,         // ウィンドウを表示する位置（Y座標）
	      width,         // ウィンドウの幅
	      height,         // ウィンドウの高さ
	      NULL,                  // 親ウィンドウのウィンドウハンドル
	      NULL,                  // メニューハンドル
	      hInst,                 // インスタンスハンドル
	      NULL                   // その他の作成データ
      );


      if (hWnd == INVALID_HANDLE_VALUE) return (HWND)INVALID_HANDLE_VALUE;

      RECT wrect, crect;
      GetWindowRect(hWnd, &wrect); GetClientRect(hWnd, &crect);
      int truewidth = width + (wrect.right-wrect.left) - (crect.right-crect.left);
      int trueheight = height + (wrect.bottom-wrect.top) - (crect.bottom-crect.top);
      x = (GetSystemMetrics(SM_CXSCREEN) - truewidth)/2;
      y = (GetSystemMetrics(SM_CYSCREEN) - trueheight)/2;
      MoveWindow(hWnd, x, y, truewidth, trueheight, TRUE);

      // ウィンドウを表示する
	    ShowWindow( hWnd, SW_SHOW );
	    UpdateWindow( hWnd );

      return hWnd;
    }

    static void RegisterWindow(HWND hWnd, WindowImpl *instance) {
      size_t sizethWnd = reinterpret_cast<size_t>(hWnd);
      handleToInstance[sizethWnd] = instance;
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
      size_t sizethWnd = reinterpret_cast<size_t>(hWnd);
      if (handleToInstance.find(sizethWnd) == handleToInstance.end()) {
        return DefWindowProc( hWnd, msg, wp, lp );
      }

      return handleToInstance[sizethWnd]->WndProc_impl(hWnd, msg, wp, lp);
    }

  private:
    LRESULT WndProc_impl(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
      switch (msg) {
      case WM_DESTROY:
		      PostQuitMessage( 0 );
		      return 0;
      }

      return DefWindowProc( hWnd, msg, wp, lp );
    }
  };

  unordered_map<size_t, WindowViewer::WindowImpl *> WindowViewer::WindowImpl::handleToInstance;

  WindowViewer::WindowViewer(const std::string &windowTitle, const Camera &camera, const PathTracer &renderer)
    : m_windowTitle(windowTitle)
    , m_camera(camera)
    , m_renderer(renderer)
    , m_pWindow(NULL)
  {
  }
  WindowViewer::~WindowViewer() {
  }

  void WindowViewer::StartViewerOnThisThread() {
    m_pWindow.reset(new WindowImpl(*this));
    m_pWindow->CreateNewWindow();

    WindowImpl::MessageLoop();
  }

  void WindowViewer::StartViewerOnNewThread() {
  }

  bool WindowViewer::CreateViewWindow() {
  }
}
