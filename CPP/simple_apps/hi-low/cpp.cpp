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
        {   
            if(tries==1)
            {
                std::cout<<"Congratulations, you guessed correctly in 1 try.";
            } else {
                std::cout<<"Congratulations, you guessed correctly in " << tries << " tries.";
            }
            return 0;
        }
        else if(answer>number)
        {
            std::cout<<"Too high, \n";
        }
        else if(answer<number)
        {
            std::cout<<"Too low, \n";
        }
        if(tries==6)
        {   std::cout<<"Wow, not even after 6 tries..";
            return 0;
        }
    }
}
