
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME_PROGRESS, m_time_progress);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_HINT_BTN, &CMFCApplication1Dlg::OnBnClickedHintBtn)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CString str;

	for (int i = 0; i < 19; i++)
	{
		str.Format(L".\\card_image\\%03d.bmp", i);
		m_card_image[i].Load(str);
	}
	
	m_time_progress.SetRange(0, 60);
	srand((unsigned int)time(NULL));

	SetTimer(1, 3000, NULL);
	SetTimer(10, 1000, NULL);
	
	StartGame();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::StartGame()
{
	SetDlgItemInt(IDC_HINT_BTN, 3);
	for (int i = 0; i < 18; i++) {
		m_table[i] = i + 1;
		m_table[18 + i] = i + 1;
	}

	// 숫자를 좀 섞어보자.
	int first, second, temp;
	for (int i = 0; i < 100; i++) {
		first = rand() % 36;
		second = rand() % 36;

		if (first != second) {
			// m_table[first] <--> m_table[second]
			temp = m_table[first];
			m_table[first] = m_table[second];
			m_table[second] = temp;
		}
	}

	m_time_progress.SetPos(60);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CString str;
		int card_index;
		for (int i = 0; i < 36; i++)
		{
			if (m_table[i] == 0) continue;;		// 제거된 카드

			if (m_view_flag) card_index = m_table[i];
			else card_index = 0;
			
			m_card_image[card_index].Draw(dc, (i%6) * 36, i/6*56);

			str.Format(L"%d", m_table[i]);
			dc.TextOutW(5+ (i % 6) * 36,5+ i / 6 * 56, str);
		}

		
		// CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_view_flag != 0) return;	// 앞면을 보여주는 상태에서는 리턴

	int x = point.x / 36;
	int y = point.y / 56;

	if (x < 6 && y < 6) {
		int select_pos = y*6+x;		// 0 ~ 35
		if (m_table[select_pos] == 0) return;	// 사라진 카드를 선택

		if (m_first_pos == -1) {
			// 첫 번째 카드 선택
			m_first_pos = select_pos;
		}
		else {
			// 두 번째 카드 선택
			if (m_first_pos == select_pos) return;		// 같은 위치의 카드 선택

			if (m_table[m_first_pos] == m_table[select_pos]) {
				m_table[m_first_pos] = m_table[select_pos] = 0;
				Invalidate();

				m_find_count++;
				if (m_find_count == 18) {
					// 게임 종료(승리)
					EndOfGame(L"당신은 게임에서 승리했습니다.");
				}
			}
			else {
				m_view_flag = 2;
				SetTimer(2, 1000, NULL);
			}
			m_first_pos = -1;
		}

		CClientDC dc(this);
		int card_index = m_table[select_pos];
		m_card_image[card_index].Draw(dc, x*36, y*56);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMFCApplication1Dlg::EndOfGame(const wchar_t * ap_ment)
{
	KillTimer(10);
	if (IDOK == MessageBox(L"다시 게임을 하시겠습니까?", ap_ment, MB_OKCANCEL | MB_ICONQUESTION))
	{
		StartGame();
		Invalidate();
	}
	else {
		// 프로그램 종료
		EndDialog(IDOK);
	}
}

void CMFCApplication1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		KillTimer(1);
		m_view_flag = 0;
		Invalidate();
	}
	else if (nIDEvent == 2) {
		KillTimer(2);
		Invalidate();
	}
	else if (nIDEvent == 10) {
		int num = m_time_progress.GetPos() - 1;
		if (num > 0) {
			m_time_progress.SetPos(num);
		}
		else {
			// 게임 종료(패배)
			EndOfGame(L"당신은 게임에서 졌습니다.");
		}
	}
	else {
		CDialogEx::OnTimer(nIDEvent);
	}
}

void CMFCApplication1Dlg::OnBnClickedHintBtn()
{
	int num = GetDlgItemInt(IDC_HINT_BTN);
	if (num > 0) {
		SetDlgItemInt(IDC_HINT_BTN, num - 1);
		m_view_flag = 1;
		Invalidate();
		SetTimer(1, 3000, NULL);
	}
}
