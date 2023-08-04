#include <iostream>
#include <random>

int main()
{
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution die100{1,100};
	int answer{};
	int tries{};
	int number{die100(mt)};
	while(answer!=number)
	{
		std::cout<<"Make your guess: ";
		tries++;
		std::cin>>answer;
		if(answer==number)
			{std::cout<<"Congratulations, you guessed correctly in " << tries << " trie(s).";
			return 0;
			}
		else if(answer>number)
			{std::cout<<"Too high, ";}
		else if(answer<number)
			{std::cout<<"Too low, ";}
		if(tries==7)
		{std::cout<<"Wow, you seriously suck at this..";
		return 0;}
	}
}
