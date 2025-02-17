﻿#include "pch.h"
#include "Item.h"

Item::Item()
{
	index = NULL;
	price = NULL;
}

Item::Item(const int index, const int price)
{
	this->index = index;
	this->price = price;
}

Item::~Item()
{
}

void Item::SetIndex(const int index)
{
	this->index = index;
}

const int Item::GetIndex()
{
	return index;
}

const string Item::GetTitle()
{
	return title;
}

const string Item::GetExplain()
{
	return explain;
}

const int Item::GetPrice()
{
	return price;
}
