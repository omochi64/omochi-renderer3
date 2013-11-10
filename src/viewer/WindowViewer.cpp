#include "WindowViewer.h"
#include <unordered_map>
#include "renderer/Camera.h"
#include "renderer/ToonMapper.h"
#include "renderer/Renderer.h"

#include <iostream>
#include <Windows.h>

using namespace std;

namespace {
  wstring widen(const std::string &src) {
	  wchar_t *wcs = new wchar_t[src.length() + 1];
    size_t converted;
    mbstowcs_s(&converted, wcs, src.length()+1, src.c_str(), src.length());
	  wstring dest = wcs;
	  delete [] wcs;
    return dest;
  }
}

namespace OmochiRenderer {
  class WindowViewer::WindowImpl {
    WindowViewer &viewer;
    static unordered_map<size_t, WindowImpl *> handleToInstance;

    wstring wstr_title;
    HWND m_hWnd;
    size_t m_timerID;

  public:
    explicit WindowImpl(WindowViewer &viewer_)
      : viewer(viewer_)
      , wstr_title()
      , m_hWnd(NULL)
      , m_timerID(0)
    {
    }

    bool CreateNewWindow() {
      HWND hWnd = CreateNewWindow_impl();
      m_hWnd = hWnd;
      if (hWnd != INVALID_HANDLE_VALUE) {
        RegisterWindow(hWnd, this);
        ResetTimer();
        return true;
      }
      return false;
    }

    void ResetTimer() {
      if (viewer.m_refreshTimeInMsec < 2) return;
      if (m_timerID != 0) {
        KillTimer(m_hWnd, m_timerID);
      }
      m_timerID = WM_USER + rand() % 10000;
      SetTimer(m_hWnd, m_timerID, viewer.m_refreshTimeInMsec - 1, NULL);
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

      wstr_title = widen(viewer.m_windowTitle.c_str());

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
      wc.lpszClassName = wstr_title.c_str();// ウィンドウクラス名

      // ウィンドウクラスを登録する
      if( RegisterClassEx( &wc ) == 0 ){ return (HWND)INVALID_HANDLE_VALUE; }

      int width = viewer.m_camera.GetScreenWidth();
      int height = viewer.m_camera.GetScreenHeight();

      int x = ( GetSystemMetrics( SM_CXSCREEN ) - width ) / 2;
      int y = ( GetSystemMetrics( SM_CYSCREEN ) - height ) / 2;

      // ウィンドウを作成する
      hWnd = CreateWindow(
	      wc.lpszClassName,      // ウィンドウクラス名
        wstr_title.c_str(),  // タイトルバーに表示する文字列
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

      if (hWnd == 0 || hWnd == INVALID_HANDLE_VALUE) return (HWND)INVALID_HANDLE_VALUE;

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

      cerr << "HWND = " << hWnd << endl;

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
      //return WndProc_impl(hWnd, msg, wp, lp);
    }

  private:
    LRESULT WndProc_impl(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
      switch (msg) {
      case WM_CREATE:
        cerr << "WM_CREATE = " << hWnd << endl;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

      case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

      case WM_PAINT:
        {
          PAINTSTRUCT paint;
          HDC hdc = BeginPaint(hWnd, &paint);


          int width = viewer.m_camera.GetScreenWidth();
          int height = viewer.m_camera.GetScreenHeight();

          const Color *result = viewer.m_renderer.GetResult();
          for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
              int index = x+y*width;

              SetPixel(hdc,x,y,
                RGB(viewer.m_mapper.Map(result[index].x),
                  viewer.m_mapper.Map(result[index].y),
                  viewer.m_mapper.Map(result[index].z)
                ));
            }
          }

          // draw information
          //SetBkMode(hdc, TRANSPARENT);
          wstring info = widen(viewer.m_renderer.GetCurrentRenderingInfo());
          RECT rc; GetClientRect(hWnd, &rc);
          DrawText(hdc, info.c_str(), -1, &rc, DT_LEFT|DT_WORDBREAK);

          EndPaint(hWnd, &paint);
        }
        return 0;

      case WM_TIMER:
        if (static_cast<size_t>(wp) == m_timerID) {
          InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
      }

      return DefWindowProc( hWnd, msg, wp, lp );
    }
  };

  unordered_map<size_t, WindowViewer::WindowImpl *> WindowViewer::WindowImpl::handleToInstance;

  WindowViewer::WindowViewer(
    const std::string &windowTitle,
    const Camera &camera, 
    const PathTracer &renderer,
    const ToonMapper &mapper, 
    const size_t refreshSpanInMsec)
    : m_windowTitle(windowTitle)
    , m_camera(camera)
    , m_renderer(renderer)
    , m_mapper(mapper)
    , m_pWindow(NULL)
    , m_windowThread()
    , m_callbackWhenClosed()
    , m_refreshTimeInMsec(refreshSpanInMsec)
  {
  }
  WindowViewer::~WindowViewer() {
  }

  void WindowViewer::StartViewerOnThisThread() {
    m_pWindow.reset(new WindowImpl(*this));
    m_pWindow->CreateNewWindow();

    WindowImpl::MessageLoop();

    if (m_callbackWhenClosed) {
      m_callbackWhenClosed();
    }
  }

  void WindowViewer::StartViewerOnNewThread() {
    m_windowThread.reset(new std::thread(
      [this]{
        m_pWindow.reset(new WindowImpl(*this));
        m_pWindow->CreateNewWindow();
        WindowImpl::MessageLoop();
        if (m_callbackWhenClosed) {
          m_callbackWhenClosed();
        }
    }
    ));
  }

  void WindowViewer::WaitWindowFinish() {
    if (m_windowThread) {
      m_windowThread->join();
      m_windowThread.reset();
    }
  }
}
