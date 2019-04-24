#ifndef WORD_H
#define WORD_H

class Word
{
public:
	string value;
	int weight;

	Word(string w, int num)
	{
		value = w;
		weight = num;
	}
};

bool operator<(const Word& w1, const Word& w2)
{
	return w1.weight < w2.weight;
}

bool operator>(const Word& w1, const Word& w2)
{
	return w1.weight > w2.weight;
}

bool operator==(const Word& w1, const Word& w2)
{
	return w1.weight == w2.weight;
}

#endif // !WORD_H
