﻿#pragma once
#include <vector>
#include <string>
#include "Monster.h"

using namespace std;

constexpr const char MONSTER_BITMAP_PATH[] = { "data/bitmapinfo/monster/MonsterBitmapInfoPath.txt" };
constexpr const char MONSTER_INFO_PATH[] = { "data/MonsterInfo/MonsterInfo.txt" };

class MonsterManager
{
private:
	vector<Monster> monsterData;

private:
	static MonsterManager* instance;

	MonsterManager();
	~MonsterManager();
public:
	static MonsterManager* GetInstance();
	static void ReleaseInstance();

public:
	vector<string> monsterBitmapPath;

public:
	vector<Monster> GetMonsterData();

	void LoadBitmapPath();
	void LoadMonsterData();

	const vector<string>* GetbitmapPath();
};
