#pragma once
#include "headers.h"

class CInputManager;
extern CInputManager GInputManager;

enum EKeys
{
	MSG_DESTROY,
	MSG_CLOSE,
	K_ESCAPE,
	K_F2,
	K_F3,
	K_TAB,
	K_SPACE = 32,
	K_1 = '1',
	K_2 = '2',
	K_3 = '3',
	K_0 = '0',
	K_W = 'w',
	K_S = 's',
	K_D = 'd',
	K_A = 'a',
	K_Q = 'q',
	K_E = 'e',
	K_LESS = 44,
	K_MORE = 46,
	K_LEFT_MOUSE_BUTTON = 100,
	K_RIGHT_MOUSE_BUTTON = 101,
};

struct SKeyInfo
{
	bool m_isDown : 1;
	bool m_inFrameDown : 1;
	bool m_inFrameUp : 1;
};
POD_TYPE(SKeyInfo)

typedef TPair<UINT8, SKeyInfo> tKeyState;
POD_TYPE(tKeyState)

class IInputObserver
{
public:
	virtual void Input( EKeys key ) = 0;
};

class CInputManager
{
private:
	TArray<IInputObserver*> m_observers;
	TArray<tKeyState> m_keys;
	HWND m_hWnd;

	void SetKey( char const& key, bool const& value )
	{
		for ( tKeyState& pair : m_keys )
		{
			if ( pair.m_key == key )
			{
				pair.m_value.m_isDown = value;
				pair.m_value.m_inFrameDown = value ? true : false;
				pair.m_value.m_inFrameUp = value ? false : true;
			}
		}
	}

public:
	void Init()
	{
		m_keys.Add( tKeyState( 'W',						{ false, false, false } ) );
		m_keys.Add( tKeyState( 'A',						{ false, false, false } ) );
		m_keys.Add( tKeyState( 'S',						{ false, false, false } ) );
		m_keys.Add( tKeyState( 'D',						{ false, false, false } ) );
		m_keys.Add( tKeyState( 'Q',						{ false, false, false } ) );
		m_keys.Add( tKeyState( 'E',						{ false, false, false } ) );
		m_keys.Add( tKeyState( ' ',						{ false, false, false } ) );
		m_keys.Add( tKeyState( K_LEFT_MOUSE_BUTTON,		{ false, false, false } ) );
		m_keys.Add( tKeyState( K_RIGHT_MOUSE_BUTTON,	{ false, false, false } ) );
	}
	bool IsKeyDown( UINT8 const key ) const
	{
		for ( tKeyState const& pair : m_keys )
		{
			if ( pair.m_key == key )
			{
				return pair.m_value.m_isDown;
			}
		}

		return false;
	}

	bool KeyDownLastFrame( UINT8 const key ) const
	{
		for ( tKeyState const& pair : m_keys )
		{
			if ( pair.m_key == key )
			{
				return pair.m_value.m_inFrameDown;
			}
		}

		return false;
	}

	bool KeyUpLastFrame( UINT8 const key ) const
	{
		for ( tKeyState const& pair : m_keys )
		{
			if ( pair.m_key == key )
			{
				return pair.m_value.m_inFrameUp;
			}
		}

		return false;
	}

	void Tick()
	{
		for ( tKeyState& pair : m_keys )
		{
			pair.m_value.m_inFrameDown = false;
			pair.m_value.m_inFrameUp = false;
		}
	}

	void AddObserver( IInputObserver* observer )
	{
		m_observers.Add( observer );
	}
	static LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch ( message )
		{
		case WM_DESTROY:
			for ( IInputObserver* observer : GInputManager.m_observers )
			{
				observer->Input( MSG_DESTROY );
			}
			break;
		case WM_CLOSE:
			for ( IInputObserver* observer : GInputManager.m_observers )
			{
				observer->Input( MSG_CLOSE );
			}
			break;
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
			case VK_ESCAPE:
				for ( IInputObserver* observer : GInputManager.m_observers )
					observer->Input( K_ESCAPE );
				break;
			case VK_F2:
				for ( IInputObserver* observer : GInputManager.m_observers )
					observer->Input( K_F2 );
				break;
			case VK_F3:
				for (IInputObserver* observer : GInputManager.m_observers)
					observer->Input(K_F3);
				break;
			case VK_SPACE:
				GInputManager.SetKey( static_cast<char const>(wParam), true );
				for ( IInputObserver* observer : GInputManager.m_observers )
					observer->Input( K_SPACE );
				break;
			case VK_TAB:
				for (IInputObserver* observer : GInputManager.m_observers)
					observer->Input(K_TAB);
				break;
			default:
				GInputManager.SetKey( static_cast<char const>(wParam), true );
				break;
			}
			break;
		}
		case WM_KEYUP:
		{
			switch ( wParam )
			{
			default:
				GInputManager.SetKey( static_cast<char const>(wParam), false );
				break;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			GInputManager.SetKey(K_LEFT_MOUSE_BUTTON, true);
			break;
		case WM_LBUTTONUP:
			GInputManager.SetKey(K_LEFT_MOUSE_BUTTON, false);
			break;
		case WM_RBUTTONDOWN:
			GInputManager.SetKey(K_RIGHT_MOUSE_BUTTON, true);
			break;
		case WM_RBUTTONUP:
			GInputManager.SetKey(K_RIGHT_MOUSE_BUTTON, false);
			break;
		case WM_CHAR:
		{
			for ( IInputObserver* observer : GInputManager.m_observers )
				observer->Input( (EKeys)(wParam) );
		}
		break;
		}

		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	void GetMousePosition( Vec2i& point )
	{
		GetCursorPos( (POINT*)(&point) );
		ScreenToClient( m_hWnd, (POINT*)(&point) );
	}
	void SetMousePosition( Vec2i point )
	{
		ClientToScreen( m_hWnd, (POINT*)(&point) );
		SetCursorPos( point.x, point.y );
	}
	void SetShowMouse( bool show )
	{
		ShowCursor( show );
	}
	void SetHWND( HWND hWnd )
	{
		m_hWnd = hWnd;
	}
};

class CSystemInput : public IInputObserver
{
public:
	virtual void Input(EKeys key)  override
	{
		switch (key)
		{
		case K_ESCAPE:
		case MSG_DESTROY:
			PostQuitMessage(0);
			break;
		}
	}
};
extern CSystemInput GSystemInput;
