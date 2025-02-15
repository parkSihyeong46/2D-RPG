﻿#pragma once

#include "DataStruct.h"
#include <map>
#include <fstream>
#include <string>
#include "AnimationObject.h"

constexpr const char UI_BITMAP_PATH[] = { "data/bitmapinfo/UIBitmapInfo.txt" };
constexpr const char ITEM_BITMAP_PATH[] = { "data/bitmapinfo/Item/ItemBitmapInfo.txt" };
constexpr const char NPC_BITMAP_PATH[] = { "data/bitmapinfo/NPCBitmapInfo.txt" };

class ImageManager
{
private:
	HBITMAP mainFrameBitmap;		// 메인화면 배경

	map<int, HBITMAP> backgroundBitmapData;		// 비트맵 데이터
	map<int, HBITMAP> objectBitmapData;			// 비트맵 데이터
	map<int, HBITMAP> uiBitmapData;				// 비트맵 데이터
	map<int, HBITMAP> itemBitmapData;			// 비트맵 데이터
	map<int, HBITMAP> npcBitmapData;			// 비트맵 데이터

	map<int, string> backgroundStringData;		// 문자열 데이터
	map<int, string> objectStringData;			// 문자열 데이터

	vector<AnimationObject> playerAnimationData;	// 플레이어 애니메이션 데이터
	vector<AnimationObject> MonsterAnimationData;	// 몬스터 애니메이션 데이터
private:
	static ImageManager* instance;

	ImageManager();
	~ImageManager();

public:
	static ImageManager* GetInstance();
	static void ReleaseInstance();

public:
	void LoadBitmapPathData(const BitmapKind kind, const string str);
	void LoadMapBitmapData();

	void LoadAnimationBitmapData(const AnimationKind kind, const string str);
public:
	const HBITMAP GetMainFrameBitmap();
	
	const HBITMAP GetBitmapData(const BitmapKind kind, const int selectNumber);
	const string GetStringData(const BitmapKind kind, const int selectNumber);

	AnimationObject* GetPlayerAnimationData(const int uiName);
	vector<AnimationObject>* GetMonsterAnimation();
};