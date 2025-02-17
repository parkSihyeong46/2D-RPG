﻿// zelda-2d.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "zelda-2d.h"
#include "GameManager.h"
#include "mapEdittor.h"
#include "imageManager.h"
#include "renderManager.h"
#include "Player.h"
#include <commdlg.h>
#include "InteractionManager.h"
#include "WorldMapManager.h"
#include "ItemManager.h"
#include "NPCManager.h"
#include "MonsterManager.h"
#include "SoundManager.h"

#define DEBUG       // 빌드 분기 나눌 때 사용

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;                                    // 전역 윈도우 핸들
HWND g_hMapEdittorDlg;                          // 전역 맵에디터 다이얼로그 핸들
RECT g_clientRect{ 0,0, ClientSize::width,ClientSize::height }; // 클라이언트 크기
SIZE g_clientSize;                              // 클라이언트 사이즈

HWND g_hStartButton;                            // 시작 버튼
#ifdef DEBUG
HWND g_hMapEdittorButton;                       // 맵 에디터 버튼
#endif // DEBUG
bool g_isPause = false;                                 // 게임 중단 변수

ClickLR clickLR{ ClickLR::NONE };

GameManager* gameManager;                       // 게임 매니저
MapEdittor* mapEdittor;                         // 맵 에디터
ImageManager* imageManager;                     // 이미지 매니저
RenderManager* renderManager;                   // 랜더 매니저
WorldMapManager* worldMapManager;               // 월드맵 매니저
ItemManager* itemManager;                       // 아이템 매니저
InteractionManager* interactionManager;         // 상호작용 매니저
NPCManager* npcManager;                         // NPC 매니저
MonsterManager* monsterManager;                 // 몬스터 매니저
SoundManager* soundManager;                     // 사운드 매니저

Player* character;                            // 캐릭터 클래스

void SetMapEdittorData();                       // 함수 선언
void SaveTextMapData(const char* filePath);           // 맵 정보 저장
void SetMapEdittorDlgData();                                // mapEdittorDlg 데이터 설정
void GetSelectListBoxData(const SelectMapState state);     // mapEdittorDlg 리스트박스 데이터 가져오기
void SelectListBoxSetting(const SelectMapState state);     // mapEdittorDlg 리스트박스, 버튼 선택 시 설정
void ShowMainFrameButton();                                 // 버튼 출력
void HideMainFrameButton();                                 // 버튼 숨기기
void GoTheGame();                                           // 멈춤 상태에서 다시 게임으로 돌아가기
void GoToMainMenu();                                        // 게임 중 메인메뉴로 이동하기

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MapEdittorDlg(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ZELDA2D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZELDA2D));

    MSG msg;

    // 초기화
    gameManager = GameManager::GetInstance();
    mapEdittor = MapEdittor::GetInstance();  
    imageManager = ImageManager::GetInstance();
    renderManager = RenderManager::GetInstance();
    worldMapManager = WorldMapManager::GetInstance();
    itemManager = ItemManager::GetInstance();
    interactionManager = InteractionManager::GetInstance();
    npcManager = NPCManager::GetInstance();
    monsterManager = MonsterManager::GetInstance();
    soundManager = SoundManager::GetInstance();

    soundManager->PlaySoundTrack(BGM::MAIN_MENU);
    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        switch (gameManager->GetState())
        {
        case GameState::MAIN:
            renderManager->MainFrameDataRender();
            break;
#ifdef DEBUG
        case GameState::MAPEDITTOR:
            renderManager->MapEdittorDataRender();
            break;
#endif // DEBUG
        case GameState::INGAME:
            if (gameManager->GetPlayer()->GetHp() <= 0)
            { 
                GoToMainMenu();
                break;
            }

            if (g_isPause)
                break;

            gameManager->Run();
            renderManager->InGameDataRender();
            break;
        default:
            break;
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZELDA2D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_ZELDA2D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static LPDRAWITEMSTRUCT lpdis;
    static int preFindItemIndex = -1;

    switch (message)
    {
    case WM_CREATE:
        AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // 메뉴창 크기 빼고 윈도우 크기 계산
        g_clientSize.cx = g_clientRect.right - g_clientRect.left;
        g_clientSize.cy = g_clientRect.bottom - g_clientRect.top;
        MoveWindow(hWnd, HWND_SPAWN_POS.x, HWND_SPAWN_POS.y, g_clientSize.cx, g_clientSize.cy, true);   // 500,200 지점에 클라이언트 크기만큼 설정 후 출력
        
        g_hStartButton = CreateWindow("button", "NEW START", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
            , START_BUTTON_POINT.x, START_BUTTON_POINT.y, BUTTON_SIZE.cx, BUTTON_SIZE.cy, hWnd, (HMENU)ButtonKind::NEW_START, hInst, NULL);    // 메인화면의 시작 버튼 생성
#ifdef DEBUG
        g_hMapEdittorButton = CreateWindow("button", "MapEdittor", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
            , MAPEDITTOR_BUTTON_POINT.x, MAPEDITTOR_BUTTON_POINT.y, BUTTON_SIZE.cx, BUTTON_SIZE.cy, hWnd, (HMENU)ButtonKind::MAPEDITTOR, hInst, NULL);    // 메인 화면의 맵 에디터 버튼 생성
#endif // DEBUG  
        break;
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case ButtonKind::NEW_START:         // 시작 버튼 누르면
                soundManager->PlaySoundTrack(BGM::INGAME);

                gameManager->Init(); // 초기화
                HideMainFrameButton();                               // 버튼 숨기기
                gameManager->SetState(GameState::INGAME);            // 인게임 실행
                gameManager->SetPlayer(new Player());           // 플레이어 생성
                gameManager->SetInventory(new Inventory());     // 인벤토리 생성

                if (imageManager->GetBitmapData(BitmapKind::BACKGROUND, 0) == NULL &&
                    imageManager->GetBitmapData(BitmapKind::OBJECT, 0) == NULL)      // 이전에 로드 한 것이 없다면 로드
                {
                    imageManager->LoadMapBitmapData();                      // 인게임에서 사용할 맵 관련 비트맵 로드
                }
                if (imageManager->GetPlayerAnimationData(0) == nullptr) // 이전에 로드 한 것이 없다면 로드
                {
                    imageManager->LoadAnimationBitmapData(AnimationKind::PLAYER, PLAYER_ANIMATION_PATH);    // 인게임에서 사용할 플레이어 애니메이션 로드
                }
                
                if (monsterManager->GetbitmapPath()->size() <= 0)   // 이전에 로드 한 것이 없다면 로드
                {
                    monsterManager->LoadBitmapPath();   // 몬스터 이미지 경로 로드
                }

                if (imageManager->GetMonsterAnimation()->size() <= 0)   // 이전에 로드 한 것이 없다면 로드
                {
                    for (const auto iterator : (*monsterManager->GetbitmapPath()))
                    {
                        imageManager->LoadAnimationBitmapData(AnimationKind::MONSTER, iterator);    // 인게임에서 사용할 몬스터 애니메이션 로드
                    }
                }
                
                if (imageManager->GetBitmapData(BitmapKind::UI, 0) == NULL &&
                    imageManager->GetBitmapData(BitmapKind::ITEM, 0) == NULL &&
                    imageManager->GetBitmapData(BitmapKind::NPC, 0) == NULL)
                {
                    imageManager->LoadBitmapPathData(BitmapKind::UI, UI_BITMAP_PATH);   // 인게임에서 사용할 UI 비트맵 로드
                    imageManager->LoadBitmapPathData(BitmapKind::ITEM, ITEM_BITMAP_PATH);   // 인게임에서 사용할 아이템 비트맵 로드
                    imageManager->LoadBitmapPathData(BitmapKind::NPC, NPC_BITMAP_PATH);   // 인게임에서 사용할 NPC 비트맵 로드
                }

                if (itemManager->GetItemData()->size() <= 0)
                {
                    itemManager->LoadItemData();         // 인게임에서 사용할 아이템 정보 로드
                }
                itemManager->AddFieldItem({ 10, 10 }, 2);   // 테스트용 아이템 출력

                if (monsterManager->GetMonsterData().size() <= 0)
                {
                    monsterManager->LoadMonsterData();  // 인게임에서 사용할 몬스터 정보 로드
                }
                if (npcManager->GetshopNPCVector()->size() <= 0)
                {
                    npcManager->LoadNPCData();          // 인게임에서 사용할 npc 정보 로드
                }

                // 맵 정보 세팅
                worldMapManager->LoadMapData(GameState::INGAME, worldMapManager->GetCurrentStage());
                worldMapManager->LoadEventData(worldMapManager->GetCurrentStage());
                break;
#ifdef DEBUG
            case ButtonKind::MAPEDITTOR:       // 맵에디터 버튼 누르면
                soundManager->PlaySoundTrack(BGM::STOP);

                HideMainFrameButton();                               // 버튼 숨기기

                gameManager->SetState(GameState::MAPEDITTOR);        // 맵 에디터 실행
                imageManager->LoadMapBitmapData();                // 맵 에디터에서 사용할 이미지 로드
                mapEdittor->Init();

                g_hMapEdittorDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MapEdittorDlg);  // 다이얼로그 생성
                SetMapEdittorDlgData();

                RECT dlgRect;
                SIZE dlgSize;
                GetWindowRect(g_hMapEdittorDlg, &dlgRect);
                dlgSize.cx = dlgRect.right - dlgRect.left;
                dlgSize.cy = dlgRect.bottom - dlgRect.top;
                MoveWindow(g_hMapEdittorDlg, 500 + g_clientSize.cx, 200,
                    dlgSize.cx, dlgSize.cy, true);   // 해당 지점에 클라이언트 크기만큼 설정 후 출력

                ShowWindow(g_hMapEdittorDlg, SW_SHOW);

                InvalidateRect(hWnd, nullptr, true);    // 화면 초기화
                break;
#endif // DEBUG
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_LBUTTONDBLCLK:
        if (GameState::INGAME == gameManager->GetState())    // 인게임 상태
        {
            interactionManager->UseItem();      // 아이템 사용 체크
            interactionManager->BuyItem();      // 아이템 구매 체크
        }
        break;
    case WM_LBUTTONDOWN:
        if (GameState::INGAME == gameManager->GetState() && clickLR == ClickLR::NONE)    // 인게임 상태
        {
            preFindItemIndex = interactionManager->FindInventoryItemIndex();

            if (g_isPause)
            {
                switch (interactionManager->FindEscMenuIndex())
                {
                case TO_THE_GAME:
                    GoTheGame();
                    break;
                case TO_THE_MAIN_MENU:
                    GoToMainMenu();
                    break;
                default:
                    break;
                }
            }
        }

        clickLR = ClickLR::LEFT;    // 클릭 상태 설정

        if (GameState::MAPEDITTOR == gameManager->GetState())    // 맵 에디터 상태
        {
            SetMapEdittorData();        // 맵에디터에 데이터 추가
        }
        break;
    case WM_RBUTTONDOWN:
        clickLR = ClickLR::RIGHT;    // 클릭 상태 설정
      
        if (GameState::MAPEDITTOR == gameManager->GetState())    // 맵 에디터 상태
        {
            SetMapEdittorData();        // 맵에디터에 데이터 추가
        }

        break;
    case WM_MOUSEMOVE:
        if (GameState::MAPEDITTOR == gameManager->GetState())    // 맵 에디터 상태
        {
            SetMapEdittorData();        // 맵에디터에 데이터 추가
        }
        break;
    case WM_LBUTTONUP:
        clickLR = ClickLR::NONE;    // 클릭 상태 설정 

        if (!(-1 == preFindItemIndex))
        {
            interactionManager->SwitchInventoryItem(preFindItemIndex);
            preFindItemIndex = -1;
        }
        break;
    case WM_RBUTTONUP:
        clickLR = ClickLR::NONE;    // 클릭 상태 설정
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)    // esc 키
        {
            if (GameState::INGAME != gameManager->GetState())
                break;

            if (!g_isPause)
            {
                g_isPause = true;
                renderManager->DrawESCMenu();
            }
            else
            {
                GoTheGame();
            }
        }

        if (wParam == 'I')  // I 키
        {
            if (GameState::INGAME != gameManager->GetState())
                break;

            if (gameManager->GetInventory()->IsOpen())
                gameManager->GetInventory()->SetOpen(false);
            else
                gameManager->GetInventory()->SetOpen(true);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        GameManager::ReleaseInstance();
        MapEdittor::ReleaseInstance();
        ImageManager::ReleaseInstance();
        RenderManager::ReleaseInstance();
        WorldMapManager::ReleaseInstance();
        ItemManager::ReleaseInstance();
        InteractionManager::ReleaseInstance();
        NPCManager::ReleaseInstance();
        MonsterManager::ReleaseInstance();
        SoundManager::ReleaseInstance();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

#ifdef DEBUG
INT_PTR CALLBACK MapEdittorDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    OPENFILENAME openFileName;
    static char strFileTitle[MAX_PATH], strFileExtension[10], strFilePath[100];
    TCHAR curDirectoryPath[256];

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_lBACKGROUND:
        case IDC_rBACKGROUND:
            SelectListBoxSetting(SelectMapState::BACKGROUND);
            return (INT_PTR)TRUE;
        case IDC_lOBJECT:
        case IDC_rOBJECT:
            SelectListBoxSetting(SelectMapState::OBJECT);
            return (INT_PTR)TRUE;
        case IDC_rCOLLIDER:
            mapEdittor->SetSelectState(SelectMapState::COLLIDER);   
            return (INT_PTR)TRUE;
        case IDC_bSAVE: 
            GetCurrentDirectory(256, curDirectoryPath);         // GetOpenFileName 호출하면 기본 경로명이 바뀌기 때문에 기본 경로명 미리 저장

            ZeroMemory(&openFileName, sizeof(OPENFILENAME));    // 구조체를 0으로 셋업
            openFileName.lStructSize = sizeof(OPENFILENAME);
            openFileName.hwndOwner = g_hWnd;
            openFileName.lpstrTitle = "저장";
            openFileName.lpstrFileTitle = strFileTitle;
            openFileName.lpstrFile = strFilePath;
            openFileName.lpstrFilter = "맵 데이터(*.txt)\0*.txt\0";
            openFileName.nMaxFile = MAX_PATH;
            openFileName.nMaxFileTitle = MAX_PATH;

            if (GetSaveFileName(&openFileName) != 0)    // 인덱스가 1부터 시작하기 때문에 지정
            {
                switch (openFileName.nFilterIndex)
                {
                case 1:
                    SaveTextMapData(strFilePath);
                    break;
                default:
                    break;
                }
            }
            SetCurrentDirectory(curDirectoryPath);  // 변경된 경로를 원래 경로로 설정
            return (INT_PTR)TRUE;
        case IDC_bLOAD:
            GetCurrentDirectory(256, curDirectoryPath);         // GetOpenFileName 호출하면 기본 경로명이 바뀌기 때문에 기본 경로명 미리 저장

            ZeroMemory(&openFileName, sizeof(openFileName));    // 구조체를 0으로 셋업
            openFileName.lStructSize = sizeof(openFileName);
            openFileName.hwndOwner = g_hWnd;
            openFileName.lpstrTitle = "로드";
            openFileName.lpstrFileTitle = strFileTitle;
            openFileName.lpstrFile = strFilePath;
            openFileName.lpstrFilter = "맵 데이터(*.txt)\0*.txt\0";
            openFileName.nMaxFile = MAX_PATH;
            openFileName.nMaxFileTitle = MAX_PATH;        

            if (GetOpenFileName(&openFileName) != 0)    // 인덱스가 1부터 시작하기 때문에 지정
            {
                switch (openFileName.nFilterIndex)
                {
                case 1:
                    WorldMapManager::GetInstance()->LoadMapData(GameState::MAPEDITTOR, strFilePath);
                    break;
                }
            }
            SetCurrentDirectory(curDirectoryPath);  // 변경된 경로를 원래 경로로 설정

            return (INT_PTR)TRUE;
        case IDC_bEXIT:
            GoToMainMenu();
            DestroyWindow(g_hMapEdittorDlg);                    // 다이얼로그 삭제
            InvalidateRect(g_hWnd, nullptr, true);              // 화면 초기화
            return (INT_PTR)TRUE;
        default:
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
#endif // DEBUG

void SetMapEdittorData()
{
    if (GameState::MAPEDITTOR != gameManager->GetState())
        return;

    POINT mousePoint;
    GetCursorPos(&mousePoint);              // 커서 위치를 가져오고
    ScreenToClient(g_hWnd, &mousePoint);    // 클라이언트 영역 좌표로 변환 후

    switch (clickLR) // 왼쪽 오른쪽 마우스 판단 후 맵 데이터 저장
    {
    case ClickLR::NONE:
        break;
    case ClickLR::LEFT:
        mapEdittor->SetMapData(mousePoint, true);   // 맵에 선택된 이미지 정보 저장
        break;
    case ClickLR::RIGHT:
        mapEdittor->SetMapData(mousePoint, false);   // 맵에 선택된 이미지 정보 저장
        break;
    default:
        break;
    }
}

void SaveTextMapData(const char* filePath)
{
    ofstream writeFile;
    int value[2] = { MAP_MAX_Y , MAP_MAX_X };   // 상단 맵 크기 저장할 변수
    WorldMap mapData = mapEdittor->GetWorldMapData();   // 저장할 맵 에디터의 맵 데이터
    SelectMapState selectState;
    string str;
    try
    {
        writeFile.open(filePath);
        if (writeFile.is_open())
        {
       
            for (int i = 0; i < 2; i++) // 맵의 크기, x,y값 저장
            {
                writeFile << value[i]<<' ';
            }

            writeFile << '\n';

            for (int i = 0; i < 3; i++) // 배경, 오브젝트, 콜라이더 데이터를 받는다.
            {
                switch (i)
                {
                case 0:
                    selectState = SelectMapState::BACKGROUND;
                    str = "background";                         break;
                case 1:
                    selectState = SelectMapState::OBJECT;
                    str = "objects";                            break;
                case 2:
                    selectState = SelectMapState::COLLIDER;
                    str = "collider";                           break;
                }
                writeFile << str<<'\n';    // background, objects, colider 구분 문자열

                for (int y = 0; y < MAP_MAX_Y; y++)
                {
                    for (int x = 0; x < MAP_MAX_X; x++)
                    {
                        writeFile << mapData.GetData(selectState, { x,y }) << ' ';
                    }
                    writeFile << '\n';
                }
            }
        }
        
    }
    catch (const std::exception&)
    {
        
    }

    writeFile.close();
}

void SetMapEdittorDlgData()
{
    SendMessage(GetDlgItem(g_hMapEdittorDlg, IDC_rBACKGROUND), BM_SETCHECK, BST_CHECKED, 0);    // 버튼 선택상태 초기화

    HWND hwndBackGroundImageListBox = GetDlgItem(g_hMapEdittorDlg, IDC_lBACKGROUND);
    int i = 1;
    while (true)    // 리스트 박스 배경 문자열 추가 코드
    {
        string name = ImageManager::GetInstance()->GetStringData(BitmapKind::BACKGROUND, i++);
        if (name.empty())
            break;
        SendMessage(hwndBackGroundImageListBox, LB_ADDSTRING, 0, (LPARAM)name.c_str());
    }

    HWND hwndObjectImageListBox = GetDlgItem(g_hMapEdittorDlg, IDC_lOBJECT);

    i = 1;
    while (true)    // 리스트 박스 오브젝트 문자열 추가 코드
    {
        string name = ImageManager::GetInstance()->GetStringData(BitmapKind::OBJECT, i++);
        if (name.empty())
            break;
        SendMessage(hwndObjectImageListBox, LB_ADDSTRING, 0, (LPARAM)name.c_str());
    }

    SendMessage(GetDlgItem(g_hMapEdittorDlg, IDC_lBACKGROUND), LB_SETCURSEL, 0, 0);     // 리스트박스 선택 초기화
    SendMessage(GetDlgItem(g_hMapEdittorDlg, IDC_lOBJECT), LB_SETCURSEL, 0, 0);         // 리스트박스 선택 초기화
}

void GetSelectListBoxData(const SelectMapState state)
{
    HWND hwndList;

    switch (state)
    {
    case SelectMapState::BACKGROUND:
        hwndList = GetDlgItem(g_hMapEdittorDlg, IDC_lBACKGROUND);       break;
    case SelectMapState::OBJECT:
        hwndList = GetDlgItem(g_hMapEdittorDlg, IDC_lOBJECT);           break;
    default :
        return;
    }

    int selectListBoxItemIndex = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

    selectListBoxItemIndex += 1;        // 이미지 인덱스 1부터 시작하기 때문에 +1 시켜줌
    MapEdittor::GetInstance()->SetSelectIndex(selectListBoxItemIndex);
}

void SelectListBoxSetting(const SelectMapState state)
{
    int unCheckButton;
    int unCheckButton2;
    int checkButton;

    switch (state)
    {
    case SelectMapState::BACKGROUND:
        unCheckButton = IDC_rOBJECT;
        unCheckButton2 = IDC_rCOLLIDER;
        checkButton = IDC_rBACKGROUND;
        break;
    case SelectMapState::OBJECT:
        unCheckButton = IDC_rBACKGROUND;
        unCheckButton2 = IDC_rCOLLIDER;
        checkButton = IDC_rOBJECT; 
        break;
    default:
        return;
    }

    SendMessage(GetDlgItem(g_hMapEdittorDlg, unCheckButton), BM_SETCHECK, BST_UNCHECKED, 0);        // 버튼 선택상태 초기화
    SendMessage(GetDlgItem(g_hMapEdittorDlg, unCheckButton2), BM_SETCHECK, BST_UNCHECKED, 0);        // 버튼 선택상태 초기화
    SendMessage(GetDlgItem(g_hMapEdittorDlg, checkButton), BM_SETCHECK, BST_CHECKED, 0);            // 버튼 선택상태 초기화
    mapEdittor->SetSelectState(state);
    GetSelectListBoxData(state);
}

void ShowMainFrameButton()
{
    ShowWindow(g_hStartButton, SW_SHOW);                // 버튼 출력
#ifdef DEBUG
    ShowWindow(g_hMapEdittorButton, SW_SHOW);           // 버튼 출력
#endif // DEBUG 
}

void HideMainFrameButton()
{
    ShowWindow(g_hStartButton, SW_HIDE);                // 버튼 숨기기
#ifdef DEBUG
    ShowWindow(g_hMapEdittorButton, SW_HIDE);           // 버튼 숨기기
#endif // DEBUG
}

void GoTheGame()
{
    g_isPause = false;
    // deltaTime을 갱신 시키지 않는다면 다음번 deltaTime이 멈춘 시간동안 늘어나 비정상적인 이동 수행함
    Timmer::GetInstance()->Update();
}

void GoToMainMenu()
{
    g_isPause = false;
    gameManager->Init();
    worldMapManager->Init();
    itemManager->Init();
    renderManager->GetHud()->Init();

    gameManager->SetState(GameState::MAIN); // 게임오버, 초기화 처리 해야함
    ShowMainFrameButton();

    soundManager->PlaySoundTrack(BGM::MAIN_MENU);
}
