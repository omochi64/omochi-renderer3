#include "WindowViewer.h"
#include <unordered_map>
#include "renderer/Camera.h"
#include "renderer/ToonMapper.h"
#include "renderer/Renderer.h"
#include "GLUtils.h"

#include <iostream>
#include <Windows.h>

using namespace std;

#include "glew.h"
#include "glext.h"
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

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

    typedef GLfloat Position[2];

  public:
    explicit WindowImpl(WindowViewer &viewer_)
      : viewer(viewer_)
      , wstr_title()
      , m_hWnd(NULL)
      , m_timerID(0)
      , m_glrc(NULL)
      , m_shader()
      , buffer(0)
    {
    }
    ~WindowImpl()
    {
      if (m_glrc != NULL) wglDeleteContext(m_glrc);
    }

    bool CreateNewWindow() {
      HWND hWnd = CreateNewWindow_impl();
      m_hWnd = hWnd;
      if (hWnd != INVALID_HANDLE_VALUE) {
        RegisterWindow(hWnd, this);
        ResetTimer();
        InitOpenGL(hWnd);
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

    void InitOpenGL(HWND hWnd) {
      HDC hdc = GetDC(hWnd);

      try {
        // ピクセルフォーマットの設定
        PIXELFORMATDESCRIPTOR pdf = {
          sizeof(PIXELFORMATDESCRIPTOR),
          1, // version
          PFD_DRAW_TO_WINDOW |
          PFD_SUPPORT_OPENGL |
          PFD_DOUBLEBUFFER,
          24,
          0, 0, 0, 0, 0, 0,
          0, 
          0, 
          0, 
          0, 0, 0, 0, 
          32, 
          0, 
          0, 
          PFD_MAIN_PLANE,
          0,
          0, 0, 0
        };
        int format = ChoosePixelFormat(hdc, &pdf);
        if (format == 0) throw "";

        if (!SetPixelFormat(hdc, format, &pdf)) throw "";

        // レンダリングコンテキスト作成
        m_glrc = wglCreateContext(hdc);
      }
      catch (...) {
        ReleaseDC(hWnd, hdc);
        return;
      }

      wglMakeCurrent(hdc, m_glrc);

      GLenum err = glewInit();
      if (err != GLEW_OK) {
        cerr << "failed to init GLEW!!:" << glewGetErrorString(err) << endl;
        return;
      }

      // init shader
      GLShaderUtils vertShader(GL_VERTEX_SHADER);
      GLShaderUtils fragShader(GL_FRAGMENT_SHADER);

      if (!vertShader.ReadShaderSource("shaders/simple.vert")) {
        cerr << "failed to read shaders/simple.vert" << endl;
      }
      if (!vertShader.CompileShader()) vertShader.PrintShaderInfoLog();

      if (!fragShader.ReadShaderSource("shaders/simple.frag")) {
        cerr << "failed to read shaders/simple.frag" << endl;
      }
      if (!fragShader.CompileShader()) fragShader.PrintShaderInfoLog();

      m_shader.Init(vertShader, fragShader);

      InitBuffer();

      if (!m_shader.LinkProgram()) m_shader.PrintProgramInfoLog();

      wglMakeCurrent(hdc, 0);
      ReleaseDC(hWnd, hdc);
      SendMessage(hWnd, WM_PAINT, NULL, NULL);
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
          wglMakeCurrent(hdc, m_glrc);
          Render(hdc, hWnd);
          wglMakeCurrent(hdc, NULL);
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

    void InitBuffer() {
      /*
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Position)* 4, NULL, GL_STATIC_DRAW);

      m_position = reinterpret_cast<Position *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

      m_position[0][0] = 0.5;
      m_position[0][1] = 0.5;
      m_position[1][0] = -0.5;
      m_position[1][1] = 0.5;
      m_position[2][0] = -0.5;
      m_position[2][1] = -0.5;
      m_position[3][0] = 0.5;
      m_position[3][1] = -0.5;

      glUnmapBuffer(GL_ARRAY_BUFFER);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      m_shader.BindAttributeLocation(0, "position");

      glGenBuffers(1, &offsetbuffer);
      glBindBuffer(GL_ARRAY_BUFFER, offsetbuffer);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Position)* 4, NULL, GL_STATIC_READ);

      Position *offsetp = reinterpret_cast<Position *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

      offsetp[0][0] = 0.0;
      offsetp[0][1] = 0.0;
      offsetp[1][0] = 0.1;
      offsetp[1][1] = 0.1;
      offsetp[2][0] = 0.2;
      offsetp[2][1] = 0.2;
      offsetp[3][0] = 0.3;
      offsetp[3][1] = 0.3;

      glUnmapBuffer(GL_ARRAY_BUFFER);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      m_shader.BindAttributeLocation(1, "offset");
      */
    }

    void Render(HDC hdc, HWND hWnd) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClearDepth(1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*      m_shader.UseShader();

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, offsetbuffer);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

      glDrawArrays(GL_LINE_LOOP, 0, 4);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(0);

      SwapBuffers(hdc);
      */
      m_shader.UseShader();

      int width = viewer.m_camera.GetScreenWidth();
      int height = viewer.m_camera.GetScreenHeight();

      double gl_width = 2.0 / width, gl_height = 2.0 / height;
      double gl_offset_x = gl_width / 2.0 - 1.0, gl_offset_y = gl_height / 2.0 - 1.0;



      clock_t b = clock();
      const Color *result = viewer.m_renderer.GetResult();
      for (int y = 0; y<height-1; y++) {
        for (int x = 0; x<width-1; x++) {
          int index = x + y*width;
          int index_x1 = x + 1 + y*width;
          int index_y1 = x + (y + 1)*width;
          int index_xy1 = x + 1 + (y + 1)*width;

          double gl_x = gl_offset_x + gl_width*x;
          double gl_x1 = gl_offset_x + gl_width*(x + 1);
          double gl_y = gl_offset_y + gl_height*(height - y - 1);
          double gl_y1 = gl_offset_y + gl_height*(height - y - 2);

          glBegin(GL_TRIANGLE_FAN);
          glColor3d(result[index].x, result[index].y, result[index].z);
          glVertex2d(gl_x, gl_y);

          glColor3d(result[index_x1].x, result[index_x1].y, result[index_x1].z);
          glVertex2d(gl_x1, gl_y);

          glColor3d(result[index_xy1].x, result[index_xy1].y, result[index_xy1].z);
          glVertex2d(gl_x1, gl_y1);

          glColor3d(result[index_y1].x, result[index_y1].y, result[index_y1].z);
          glVertex2d(gl_x, gl_y1);

          glEnd();
        }
      }
      SwapBuffers(hdc);

      // draw information
      //SetBkMode(hdc, TRANSPARENT);
      wstring info = widen(viewer.m_renderer.GetCurrentRenderingInfo());
      RECT rc; GetClientRect(hWnd, &rc);
      DrawText(hdc, info.c_str(), -1, &rc, DT_LEFT | DT_WORDBREAK);

      //cerr << "time for rendering to window: " << 1.0*(clock() - b) / CLOCKS_PER_SEC << endl;

    }
  private:
    wstring wstr_title;
    HWND m_hWnd;
    size_t m_timerID;
    HGLRC m_glrc;

    GLProgramUtils m_shader;
    GLuint buffer;
    GLuint offsetbuffer;
    Position *m_position;

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
