#ifndef __RULES_H__
#define __RULES_H__
#include <vector>
#include<algorithm>
struct puke
{
	puke(int t, int n) :type(t), num(n) {}
	static bool cmp(const puke lhs, const puke rhs)
	{
		if (lhs.num > rhs.num)
			return true;
		else
			return false;
	}
	int type;
	int num;
};
enum CardsType
{
	SingleCard,
	Pair,
	Triplet,
	TripletWithAnAttachedCard,
	TripletWithAnAttachedPair,
	Sequence,
	SequenceOfPairs,
	SequenceOfTriplets,
	SequenceOfTripletsWithAttachedCards,
	SequenceOfTripletsWithAttachedPairs,
	Bomb,
	Rocket,
	QuadplexSet,
	QuadPlexSets,
	Invalid,
};
class Rules
{
public:
	void sortPukes(std::vector<puke> &puke);
	CardsType getCardsType(std::vector<puke> &cards);//获取牌的类型
	bool isSingleCard(std::vector<puke> &cards);//
	bool isSingleCard(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isPair(std::vector<puke> &cards);
	bool isPair(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isTriplet(std::vector<puke> &cards);
	bool isTriplet(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isTripletWithAnAttachedCard(std::vector<puke> &cards);
	bool isTripletWithAnAttachedCard(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isTripletWithAnAttachedPair(std::vector<puke> &cards);
	bool isTripletWithAnAttachedPair(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isSequence(std::vector<puke> &cards);
	bool isSequence(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isSequenceOfPairs(std::vector<puke> &cards);
	bool isSequenceOfPairs(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isSequenceOfTriplets(std::vector<puke> &cards);
	bool isSequenceOfTriplets(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isSequenceOfTripletsWithAttachedCards(std::vector<puke> &cards);
	bool isSequenceOfTripletsWithAttachedCards(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isSequenceOfTripletsWithAttachedPairs(std::vector<puke> &cards);
	bool isSequenceOfTripletsWithAttachedPairs(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isBomb(std::vector<puke> &cards);
	bool isBomb(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isRocket(std::vector<puke> &cards);
	bool isRocket(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isQuadplexSet(std::vector<puke> &cards);
	bool isQuadplexSet(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);
	bool isQuadPlexSets(std::vector<puke> &cards);
	bool isQuadPlexSets(std::vector<puke>::iterator begin, std::vector<puke>::iterator end);//

	std::vector<puke> hintCards(std::vector<puke> &cards, std::vector<puke>& myCards);//找出可以管上的牌
	bool compare(std::vector<puke> lhs, std::vector<puke> rhs);//
};
#endif
