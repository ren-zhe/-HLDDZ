#include "rules.h"

void Rules::sortPukes(std::vector<puke>& puke)
{
	std::sort(puke.begin(), puke.end(), puke::cmp);
}

CardsType Rules::getCardsType(std::vector<puke> &cards)
{
	if (isSingleCard(cards))
		return SingleCard;
	else if (isPair(cards))
		return Pair;
	else if (isTriplet(cards))
		return Triplet;
	else if (isTripletWithAnAttachedCard(cards))
		return TripletWithAnAttachedCard;
	else if (isTripletWithAnAttachedPair(cards))
		return TripletWithAnAttachedPair;
	else if (isSequence(cards))
		return CardsType::Sequence;
	else if (isSequenceOfPairs(cards))
		return SequenceOfPairs;
	else if (isSequenceOfTriplets(cards))
		return SequenceOfTriplets;
	else if (isSequenceOfTripletsWithAttachedCards(cards))
		return SequenceOfTripletsWithAttachedCards;
	else if (isSequenceOfTripletsWithAttachedPairs(cards))
		return SequenceOfTripletsWithAttachedPairs;
	else if (isBomb(cards))
		return Bomb;
	else if (isRocket(cards))
		return Rocket;
	else if (isQuadplexSet(cards))
		return QuadplexSet;
	else if (isQuadPlexSets(cards))
		return QuadPlexSets;
	else
		return Invalid;
}
bool Rules::isBomb(std::vector<puke> &cards)
{
	return isBomb(cards.begin(), cards.end());
}

bool Rules::isBomb(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if ((end - begin) == 4 && isTriplet(begin, begin + 3) == true && (*(begin + 3)).num == (*begin).num)
		return true;
	else
		return false;
}

bool Rules::isRocket(std::vector<puke> &cards)
{
	return isRocket(cards.begin(), cards.end());
}

bool Rules::isRocket(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if ((end - begin) == 2 && (*begin).num == 15 && (*(begin + 1)).num == 14)
		return true;
	else
		return false;
}

bool Rules::isQuadplexSet(std::vector<puke> &cards)
{
	return isQuadplexSet(cards.begin(), cards.end());
}
bool Rules::isQuadplexSet(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 6)
	{
		//int bombNum = 0;
		int startBomb = -1;
		//int endBomb = -1;
		for (int i = 0; i < end - begin - 3; i++)//找到4张位置
		{
			if (isBomb(begin + i, begin + i + 4) == true)
			{
				//bombNum++;
				/*if (startBomb == -1)
				{
				startBomb = i;
				endBomb = i;
				}
				else
				{
				endBomb = i;
				}*/
				startBomb = i;
				break;
			}
		}
		if (startBomb != -1)//炸弹个数符合，单牌不相等，没有王炸时成功
		{
			std::vector<int> singles;
			//singles.push_back((*(begin + startBomb)).num);
			for (int i = 0; i < startBomb; i++)
			{
				singles.push_back((*(begin + i)).num);
			}
			for (int i = startBomb + 4; i < end - begin; i++)
			{
				singles.push_back((*(begin + i)).num);
			}
			int jokerNum = 0;
			for (int i = 0; i < singles.size() - 1; i++)
			{
				if (singles[i] == 14 || singles[i] == 15)
					jokerNum++;
				for (int j = i + 1; j < singles.size(); j++)
				{
					if (singles[i] == singles[j])
						return false;
				}
			}
			if (jokerNum == 2)
				return false;
			return true;
		}
	}
	return false;
}
bool Rules::isQuadPlexSets(std::vector<puke> &cards)
{
	return isQuadPlexSets(cards.begin(), cards.end());
}
bool Rules::isQuadPlexSets(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 8)
	{
		//int bombNum = 0;
		int startBomb = -1;
		//int endBomb = -1;
		for (int i = 0; i < end - begin - 3; i++)//找到4张位置
		{
			if (isBomb(begin + i, begin + i + 4) == true)
			{
				//bombNum++;
				/*i += 3;*/
				/*if (startBomb == -1)
				{
				startBomb = i;
				endBomb = i;
				}
				else
				{
				endBomb = i;
				}*/
				startBomb = i;
				break;
			}
		}
		if (startBomb != -1 && startBomb % 2 == 0)//炸弹数量及位置合法
		{
			std::vector<int> doubles;
			//doubles.push_back((*(begin + startBomb)).num);
			for (int i = 0; i < startBomb - 1; i += 2)
			{
				if ((*(begin + i)).num == (*(begin + i + 1)).num)
				{
					doubles.push_back((*(begin + i)).num);
				}
				else
					return false;
			}
			for (int i = startBomb + 4; i < end - begin - 1; i += 2)
			{
				if ((*(begin + i)).num == (*(begin + i + 1)).num)
				{
					doubles.push_back((*(begin + i)).num);
				}
				else
					return false;
			}
			//if (doubles.size() != 3)
			//	return false;
			for (int i = 0; i < doubles.size() - 1; i++)
			{
				for (int j = i + 1; j < doubles.size(); j++)
				{
					if (doubles[i] == doubles[j])
						return false;
				}
			}
			return true;
		}
	}
	return false;
}
bool Rules::isSingleCard(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 1)
		return true;
	else
		return false;
}

bool Rules::isPair(std::vector<puke> &cards)
{
	return isPair(cards.begin(), cards.end());
}

bool Rules::isPair(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 2 && (*begin).num == (*(begin + 1)).num)
		return true;
	else
		return false;
}

bool Rules::isTriplet(std::vector<puke> &cards)
{
	return isTriplet(cards.begin(), cards.end());
}

bool Rules::isTriplet(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 3 && (*begin).num == (*(begin + 1)).num && (*begin).num == (*(begin + 2)).num)
		return true;
	else
		return false;
}

bool Rules::isTripletWithAnAttachedCard(std::vector<puke> &cards)
{
	return isTripletWithAnAttachedCard(cards.begin(), cards.end());
}

bool Rules::isTripletWithAnAttachedCard(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 4 && (isTriplet(begin, begin + 3) == true && (*begin).num != (*(begin + 3)).num
		|| isTriplet(begin + 1, end) == true && (*begin).num != (*(begin + 1)).num))
		return true;
	else
		return false;
}

bool Rules::isTripletWithAnAttachedPair(std::vector<puke> &cards)
{
	return isTripletWithAnAttachedPair(cards.begin(), cards.end());
}

bool Rules::isTripletWithAnAttachedPair(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	if (end - begin == 5 && (isTriplet(begin, begin + 3) == true && isPair(begin + 3, end)
		|| isPair(begin, begin + 2) == true && isTriplet(begin + 2, end)))
		return true;
	else
		return false;

}

bool Rules::isSequence(std::vector<puke> &cards)
{
	return isSequence(cards.begin(), cards.end());
}

bool Rules::isSequence(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	int num = end - begin;
	if (num >= 5 && num <= 12)//必须满足数量条件
	{
		if ((*begin).num >= 13)//最大扑克是否越界
			return false;
		for (int i = 0; i < num - 1; i++)//循环至倒数第二张扑克
		{
			if ((*(begin + i)).num != (*(begin + i + 1)).num + 1)//不满足依此递减关系
				return false;
		}
		return true;
	}
	else
		return false;
}

bool Rules::isSequenceOfPairs(std::vector<puke> &cards)
{
	return isSequenceOfPairs(cards.begin(), cards.end());
}

bool Rules::isSequenceOfPairs(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	int num = end - begin;
	if (num % 2 == 0 && num >= 6)//满足数量条件
	{
		if ((*begin).num >= 13)//最大扑克是否越界
			return false;
		for (int i = 0; i < num - 1; i += 2)//循环至倒数第二张牌，比较是否构成对子
		{
			if ((*(begin + i)).num == (*(begin + i + 1)).num)
			{
				if (i != num - 2)//循环至倒数第4张牌，比较是否依次递减
				{
					if ((*(begin + i)).num != (*(begin + i + 2)).num + 1)
						return false;
				}
			}
			else
				return false;
		}
	}
	else
		return false;
	return true;
}

bool Rules::isSequenceOfTriplets(std::vector<puke> &cards)
{
	return isSequenceOfTriplets(cards.begin(), cards.end());
}

bool Rules::isSequenceOfTriplets(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	int num = end - begin;
	if (num % 3 == 0 && num >= 6)//满足数量条件
	{
		if ((*begin).num >= 13)//最大扑克是否越界
			return false;
		for (int i = 0; i < num - 2; i += 3)//循环至倒数第三张牌，比较是否构成三张相同的
		{
			if (isTriplet(begin + i, begin + i + 3) == false)
				return false;
			if (i != num - 3)
			{
				if ((*(begin + i)).num != (*(begin + i + 3)).num + 1)
					return false;
			}
		}
	}
	else
		return false;
	return true;
}

bool Rules::isSequenceOfTripletsWithAttachedCards(std::vector<puke> &cards)
{
	return isSequenceOfTripletsWithAttachedCards(cards.begin(), cards.end());
}

bool Rules::isSequenceOfTripletsWithAttachedCards(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{//99987776
	int num = end - begin;//牌的数量
	int triNum = 0;//三张的数量
	int startTri = -1;//第一个三张位置
	int endTri = -1;//最后一个三张位置
	for (int i = 0; i < num - 2; i++)
	{
		if (isTriplet(begin + i, begin + i + 3) == true)
		{
			triNum++;
			if (startTri == -1)
			{
				startTri = i;
				endTri = i;
			}
			else
			{
				endTri = i;
			}
		}
	}
	if (triNum < 2 || (endTri - startTri) % 3 != 0 || (endTri - startTri) / 3 != triNum - 1)//检验每个三张是否是连着的
		return false;
	if (triNum * 4 != end - begin)
		return false;
	if ((*(begin + startTri)).num == 13)
		return false;
	for (int i = startTri; i < endTri; i += 3)
	{
		if ((*(begin + i)).num != (*(begin + i + 3)).num + 1)
			return false;
	}
	std::vector<int> singles;
	for (int i = 0; i < startTri; i++)
	{
		singles.push_back((*(begin + i)).num);
	}
	for (int i = endTri + 3; i < num; i++)
	{
		singles.push_back((*(begin + i)).num);
	}
	int jokerNum = 0;//统计joker数量，判断是否有相同的扑克
	for (int i = 0; i < singles.size() - 1; i++)
	{
		if (singles[i] == 14 || singles[i] == 15)
			jokerNum++;
		for (int j = i + 1; j < singles.size(); j++)
		{
			if (singles[i] == singles[j])
				return false;
			else //跟这个不等，跟之后的也不可能相等，所以跳出。
				break;
		}
	}
	if (jokerNum == 2)
		return false;
	return true;
}

bool Rules::isSequenceOfTripletsWithAttachedPairs(std::vector<puke> &cards)
{
	return isSequenceOfTripletsWithAttachedPairs(cards.begin(), cards.end());
}

bool Rules::isSequenceOfTripletsWithAttachedPairs(std::vector<puke>::iterator begin, std::vector<puke>::iterator end)
{
	int num = end - begin;
	int triNum = 0;
	int startTri = -1;
	int endTri = -1;
	for (int i = 0; i < num - 2; i++)
	{
		if (isTriplet(begin + i, begin + i + 3) == true)
		{
			triNum++;
			if (startTri == -1)
			{
				startTri = i;
				endTri = i;
			}
			else
			{
				endTri = i;
			}
		}
	}
	//9998884433/
	if (triNum < 2 || triNum * 5 != end - begin || (endTri - startTri) % 3 != 0 || (endTri - startTri) / 3 != triNum - 1)//检验每个三张是否是连着的)
		return false;
	if ((*(begin + startTri)).num == 13)
		return false;
	for (int i = startTri; i < endTri; i += 3)
	{
		if ((*(begin + i)).num != (*(begin + i + 3)).num + 1)
			return false;
	}
	std::vector<int> doubles;
	for (int i = 0; i < startTri - 1; i += 2)
	{
		if ((*(begin + i)).num == (*(begin + i + 1)).num)
		{
			doubles.push_back((*(begin + i)).num);
		}
		else
			return false;
	}
	for (int i = endTri + 3; i < num - 1; i += 2)//以修正由i++ -> i += 2
	{
		if ((*(begin + i)).num == (*(begin + i + 1)).num)
		{
			doubles.push_back((*(begin + i)).num);
		}
		else
			return false;
	}
	for (int i = 0; i < doubles.size() - 1; i++)
	{
		for (int j = i + 1; j < doubles.size(); j++)
		{
			if (doubles[i] == doubles[j])
				return false;
			else
				break;
		}
	}
	return true;
}
bool Rules::isSingleCard(std::vector<puke> &cards)
{
	return isSingleCard(cards.begin(), cards.end());
}
std::vector<puke> Rules::hintCards(std::vector<puke>& cards, std::vector<puke>& myCards)
{
	CardsType type = getCardsType(cards);
	std::vector<puke> hints;

	switch (type)
	{
	case SingleCard:
		for (int i = myCards.size() - 1; i >= 0; --i)
		{
			if (myCards.at(i).num > cards.at(0).num)
			{
				hints.push_back(myCards.at(i));
				break;
			}
		}
		break;
	case Pair:
		for (int i = myCards.size() - 2; i >= 0; i--)
		{
			if (myCards.at(i).num == myCards.at(i + 1).num && myCards.at(i).num > cards.at(0).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				break;
			}
		}
		break;
	case Triplet:
		for (int i = myCards.size() - 3; i >= 0; i--)
		{
			if (isTriplet(myCards.begin() + i, myCards.begin() + i + 3) == true && myCards.at(i).num > cards.at(0).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				hints.push_back(myCards.at(i + 2));
				break;
			}
		}
		break;
	case TripletWithAnAttachedCard:
	{
		bool can = false;
		int startCards = -1;
		for (int i = 0; i < cards.size(); i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 3) == true)
			{
				startCards = i;
				break;
			}
		}
		int start = -1;
		for (int i = myCards.size() - 3; i >= 0; i--)
		{
			if (isTriplet(myCards.begin() + i, myCards.begin() + i + 3) == true && myCards.at(i).num > cards.at(startCards).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				hints.push_back(myCards.at(i + 2));
				can = true;
				start = i;
				break;
			}
		}
		if (can)
		{
			for (int i = myCards.size() - 1; i >= 0; i--)
			{
				if (i >= start && i <= start + 2)
					continue;
				else
				{
					hints.push_back(myCards.at(i));
					break;
				}
			}
		}
	}
	break;
	case TripletWithAnAttachedPair:
	{
		bool can = false;
		int startCards = -1;
		for (int i = 0; i < cards.size(); i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 3) == true)
			{
				startCards = i;
				break;
			}
		}
		int start = -1;
		for (int i = myCards.size() - 3; i >= 0; i--)
		{
			if (isTriplet(myCards.begin() + i, myCards.begin() + i + 3) == true && myCards.at(i).num > cards.at(startCards).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				hints.push_back(myCards.at(i + 2));
				start = i;
				can = true;
				break;
			}
		}
		if (can)
		{
			for (int i = myCards.size() - 2; i >= 0; i--)
			{
				if (i >= start - 1 && i <= start + 2)
					continue;
				else
				{
					if (isPair(myCards.begin() + i, myCards.begin() + 2 + i) == true)
					{
						hints.push_back(myCards.at(i));
						hints.push_back(myCards.at(i + 1));
						break;
					}
				}
			}
		}
	}
	break;
	case CardsType::Sequence:
	{
		int size = cards.size();
		std::vector<bool> temp(12, false);
		for (int i = 0; i < myCards.size(); i++)
		{
			if (myCards.at(i).num < 13)
				temp[myCards.at(i).num - 1] = true;
		}
		for (int i = size - 1; i < 12; i++)
		{
			bool can = true;
			for (int j = i; j >= i - size + 1; j--) // i - i - size + 1
			{
				if (temp[j] == false)
				{
					can = false;
					break;
				}
			}
			if (can && i + 1 > cards.at(0).num)
			{
				for (int j = 0; j < myCards.size(); j++)//挑出那些数
				{
					if (myCards.at(j).num - 1 <= i  && myCards.at(j).num - 1 >= i - size + 1 && temp[myCards.at(j).num - 1] == true)
					{
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1] = false;
					}
				}
				break;
			}
		}
	}
	break;
	case SequenceOfPairs:
	{
		int size = cards.size() / 2;
		std::vector<int> temp(12);
		for (int i = 0; i < myCards.size(); i++)
		{
			if (myCards.at(i).num < 13)
				temp[myCards.at(i).num - 1]++;
		}
		for (int i = size - 1; i < 12; i++)
		{
			bool can = true;
			for (int j = i; j >= i - size + 1; j--)
			{
				if (temp[j] < 2)
				{
					can = false;
					break;
				}
			}
			if (can && i + 1 > cards.at(0).num)
			{
				for (int j = i; j >= i - size + 1; j--)
					temp[j] = 4;
				for (int j = 0; j < myCards.size(); j++)//挑出那些数
				{
					if (myCards.at(j).num - 1 <= i && myCards.at(j).num - 1 >= i - size + 1 && temp[myCards.at(j).num - 1] > 2)
					{
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1]--;
					}
				}
				break;
			}
		}
	}
	break;
	case SequenceOfTriplets:
	{
		int size = cards.size() / 3;
		std::vector<int> temp(12);
		for (int i = 0; i < myCards.size(); i++)
		{
			if (myCards.at(i).num < 13)
				temp[myCards.at(i).num - 1]++;
		}
		for (int i = size - 1; i < 12; i++)
		{
			bool can = true;
			for (int j = i; j >= i - size + 1; j--)
			{
				if (temp[j] < 3)
				{
					can = false;
					break;
				}
			}
			if (can && i + 1 > cards.at(0).num)
			{
				for (int j = i; j >= i - size + 1; j--)
					temp[j] = 5;
				for (int j = 0; j < myCards.size(); j++)//挑出那些数
				{
					if (myCards.at(j).num - 1 <= i && myCards.at(j).num - 1 >= i - size + 1 && temp[myCards.at(j).num - 1] > 2)
					{
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1]--;
					}
				}
				break;
			}
		}
	}
	break;
	case SequenceOfTripletsWithAttachedCards:
	{
		int start = -1;
		for (int i = 0; i < cards.size() - 2; i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 3) == true)
			{
				start = i;
				break;
			}
		}
		int size = cards.size() / 4;
		std::vector<int> temp(12);
		for (int i = 0; i < myCards.size(); i++)
		{
			if (myCards.at(i).num < 13)
				temp[myCards.at(i).num - 1]++;
		}
		for (int i = size - 1; i < 12; i++)
		{
			bool can = true;
			for (int j = i; j >= i - size + 1; j--)
			{
				if (temp[j] < 3)
				{
					can = false;
					break;
				}
			}
			if (can && i + 1 > cards.at(start).num)
			{
				for (int j = i; j >= i - size + 1; j--)
					temp[j] = 5;
				int singles = 0;
				for (int j = myCards.size() - 1; j >= 0; j--)//挑出那些数
				{
					if (myCards.at(j).num - 1 <= i && myCards.at(j).num - 1 >= i - size + 1)
					{
						if (temp[myCards.at(j).num - 1] > 2)
						{
							hints.push_back(myCards.at(j));
							temp[myCards.at(j).num - 1]--;
						}
					}
					else if (temp[myCards.at(j).num - 1] > 0)
					{
						if (singles == size)
							break;
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1] = 0;
						singles++;
					}
				}
				if (singles == size)
				{
					break;
				}
			}
		}
	}
	break;
	case SequenceOfTripletsWithAttachedPairs:
	{
		int start = -1;
		for (int i = 0; i < cards.size() - 2; i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 3) == true)
			{
				start = i;
				break;
			}
		}
		int size = cards.size() / 5;
		std::vector<int> temp(12);
		for (int i = 0; i < myCards.size(); i++)
		{
			if (myCards.at(i).num < 13)
				temp[myCards.at(i).num - 1]++;
		}
		for (int i = size - 1; i < 12; i++)
		{
			bool can = true;
			for (int j = i; j >= i - size + 1; j--)
			{
				if (temp[j] < 3)
				{
					can = false;
					break;
				}
			}
			if (can && i + 1 > cards.at(start).num)
			{
				for (int j = i; j >= i - size + 1; j--)
					temp[j] = 5;
				for (int j = 0; j < i - size + 1; j++)
				{
					if (temp[j] > 2)
						temp[j] = 4;
				}
				for (int j = i + 1; j < 11; j++)
				{
					if (temp[j] > 2)
						temp[j] = 4;
				}
				int doubles = 0;
				for (int j = myCards.size() - 1; j > 0; j--)//挑出那些数
				{
					if (myCards.at(j).num - 1 <= i && myCards.at(j).num - 1 >= i - size + 1 && temp[myCards.at(j).num - 1] > 2)
					{
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1]--;
					}
					else if (temp[myCards.at(j).num - 1] > 2)
					{
						hints.push_back(myCards.at(j));
						temp[myCards.at(j).num - 1]--;
						doubles++;
					}
				}
				//std::vector<int> singles;
				//for (int j = 0; j < myCards.size(); j++)//挑出那些数
				//{
				//	if (myCards[j].num - 1 <= i && myCards[j].num - 1 >= i - size + 1 && temp[myCards[j].num - 1] > 2)
				//	{
				//		hints.push_back(myCards[j]);
				//		temp[myCards[j].num - 1]--;
				//	}
				//	else
				//	{
				//		if (singles.size() < size)
				//		{
				//			bool have = false;
				//			for (int k = 0; k < singles.size(); k++)
				//			{
				//				if (singles[k] == myCards[j].num)
				//				{
				//					have = true;
				//					break;
				//				}
				//			}
				//			if (!have)
				//			{
				//				singles.push_back(myCards[j].num);
				//				hints.push_back(myCards.at(i));
				//			}
				//		}
				//	}
				//}
				if (doubles == size)
				{
					break;
				}
			}
		}
	}
	break;
	case Bomb:
	{
		for (int i = myCards.size() - 4; i >= 0; i--)
		{
			if (isBomb(myCards.begin() + i, myCards.begin() + i + 4) == true && myCards.at(i).num > cards.at(0).num)
			{
				for (int j = i; j < i + 4; j++)
				{
					hints.push_back(myCards.at(j));
				}
				break;
			}
		}
	}
	break;
	case QuadplexSet:
	{
		bool can = false;
		int startCards = -1;
		for (int i = 0; i < cards.size(); i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 4) == true)
			{
				startCards = i;
				break;
			}
		}
		int start = -1;
		for (int i = myCards.size() - 4; i >= 0; i--)
		{
			if (isTriplet(myCards.begin() + i, myCards.begin() + i + 4) == true && myCards.at(i).num > cards.at(startCards).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				hints.push_back(myCards.at(i + 2));
				hints.push_back(myCards.at(i + 3));
				can = true;
				start = i;
				break;
			}
		}
		if (can)
		{
			int single1 = -1;
			for (int i = myCards.size() - 1; i >= 0; i--)
			{
				if (i >= start && i <= start + 2)
					continue;
				else
				{
					if (single1 == -1)
					{
						single1 = i;
						hints.push_back(myCards.at(single1));
					}
					else
					{
						hints.push_back(myCards.at(i));
						break;
					}
				}
			}
		}
	}
	break;
	case QuadPlexSets:
	{
		bool can = false;
		int startCards = -1;
		for (int i = 0; i < cards.size(); i++)
		{
			if (isTriplet(cards.begin() + i, cards.begin() + i + 4) == true)
			{
				startCards = i;
				break;
			}
		}
		int start = -1;
		for (int i = myCards.size() - 4; i >= 0; i--)
		{
			if (isTriplet(myCards.begin() + i, myCards.begin() + i + 4) == true && myCards.at(i).num > cards.at(startCards).num)
			{
				hints.push_back(myCards.at(i));
				hints.push_back(myCards.at(i + 1));
				hints.push_back(myCards.at(i + 2));
				hints.push_back(myCards.at(i + 3));
				can = true;
				start = i;
				break;
			}
		}
		if (can)
		{
			int double1 = -1;
			for (int i = myCards.size() - 2; i >= 0; i--)
			{
				if (i >= start - 1 && i <= start + 2)
					continue;
				else
				{
					if (isPair(myCards.begin() + i, myCards.begin() + i + 2) == true)
					{
						if (double1 == -1)
						{
							double1 = i;
							hints.push_back(myCards.at(double1));
							hints.push_back(myCards.at(double1 + 1));
						}
						else
						{
							hints.push_back(myCards.at(i));
							hints.push_back(myCards.at(i));
							break;
						}
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}
	
	std::vector<int> cardNum(15, 0);//在没有直接管上的情况下，查看是否有炸弹能够管上
	for (int i = 0; i < myCards.size(); i++)
	{
		cardNum[myCards[i].num - 1] += 1;
	}
	int fourNum = -1;
	for (int i = 0; i < cardNum.size(); i++)
	{
		if (cardNum[i] == 4)
		{
			fourNum = i + 1;
			break;
		}
	}
	if (hints.empty())
	{
		if (fourNum != -1)
		{
			for (int i = 0; i < myCards.size(); i++)
			{
				if (myCards[i].num == fourNum)
					hints.push_back(myCards[i]);
			}
		}
		else if (cardNum[13] == 1 && cardNum[14] == 1)
		{
			for (int i = 0; i < myCards.size(); i++)
				if (myCards[i].num == 14 || myCards[i].num == 15)
					hints.push_back(myCards[i]);
		}
	}
	return hints;
}
bool Rules::compare(std::vector<puke> lhs, std::vector<puke> rhs) // 按大于比较
{
	CardsType lType = getCardsType(lhs);
	CardsType rType = getCardsType(rhs);
	if (lType == rType) //同类型比较
	{
		switch (lType)
		{
		case SingleCard:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case Pair:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case Triplet:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case TripletWithAnAttachedCard:
		case TripletWithAnAttachedPair:
		case SequenceOfTriplets:
		case SequenceOfTripletsWithAttachedCards:
		case SequenceOfTripletsWithAttachedPairs:
		{
			int lf = -1, rf = -1;
			for (int i = 0; i < lhs.size() - 2; i++)
			{
				if (isTriplet(lhs.begin() + i, lhs.begin() + i + 3) == true)
				{
					lf = i;
					break;
				}
			}
			for (int i = 0; i < rhs.size() - 2; i++)
			{
				if (isTriplet(rhs.begin() + i, rhs.begin() + i + 3) == true)
				{
					rf = i;
					break;
				}
			}
			if (lhs.at(lf).num > rhs.at(rf).num)
				return true;
		}
		break;
		case CardsType::Sequence:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case SequenceOfPairs:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case Bomb:
			if (lhs.at(0).num > rhs.at(0).num)
				return true;
			break;
		case QuadplexSet:
		case QuadPlexSets:
		{
			int lf = -1, rf = -1;
			for (int i = 0; i < lhs.size() - 3; i++)
			{
				if (isBomb(lhs.begin() + i, lhs.begin() + i + 4) == true)
				{
					lf = i;
					break;
				}
			}
			for (int i = 0; i < rhs.size() - 3; i++)
			{
				if (isTriplet(rhs.begin() + i, rhs.begin() + i + 4) == true)
				{
					rf = i;
					break;
				}
			}
			if (lhs.at(lf).num > rhs.at(rf).num)
				return true;
		}
		default:
			return false;
		}
	}
	else //不同类型的比较
	{
		if (lType == Rocket)
			return true;
		else if (lType == Bomb)
		{
			if (rType == Rocket)
				return false;
			else
				return true;
		}
	}
	return false;
}