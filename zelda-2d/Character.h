﻿#pragma once
#include "DataStruct.h"

constexpr DPOINT START_POS = { 400,400 };

constexpr int INIT_HP = 6;
constexpr int INIT_SPEED = 150;
constexpr int INIT_DAMAGE = 1;

class Character
{
protected:
	DPOINT pos;			// 위치값

	int state;			// 상태값
	int dir;			// 방향

	int maxHp;			// maxHp
	int hp;				// hp

	int speed;			// speed
	int damage;			// damage

	int money;			// money
	int exp;

	ULONGLONG hitTick;		// 피격 딜레이
	ULONGLONG attackTick;	// 공격 딜레이
public:
	Character();
	Character(const DPOINT pos, const int hp, const int speed, const int damage);
	~Character();

	const DPOINT GetPos();
	void SetPos(const DPOINT pos);

	const int GetMaxHp();

	const int GetHp();
	void SetHp(const int hp);

	const int GetSpeed();

	const int GetDamage();
	void SetDamage(const int damage);

	const int GetState();
	void SetState(const int state);

	const int GetDir();
	void SetDir(const int dir);

	const int GetMoney();
	void SetMoney(const int money);

	void SetExp(const int exp);
	const int GetExp();

	const ULONGLONG GetHitTick();
	void SetHitTick(const ULONGLONG hitTick);

	const ULONGLONG GetAttackTick();
	void SetAttackTick(const ULONGLONG attackTick);
};

