#include "CCell.h"
#include <iostream>

using namespace std;
CCell::CCell()
{
	m_pShip = 0;
	m_bBeated = false;
}
string CCell::PrintOwn()
{
	if (m_bBeated)
	{
		if (m_pShip)
			return "N\t";
		else
			return  "#\t";
	}
	else
	{
		if (m_pShip)
			return "S\t";
		else
			return ".\t";
	}

}
string CCell::PrintForeign()
{
	if (m_bBeated)
	{
		if (m_pShip)
			return "!\t";
		else
			return "#\t";
	}
	else
	{
		return ".\t";
	}

}

CCell::~CCell()
{

}