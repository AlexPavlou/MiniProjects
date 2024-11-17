#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

int main()
{
    boost::multiprecision::int128_t mySignedInt128 =-1;
    boost::multiprecision::uint128_t myUnsignedInt128 =2;
    boost::multiprecision::int256_t mySignedInt256 =-3;
    boost::multiprecision::uint256_t myUnsignedInt256 =4;
    boost::multiprecision::int512_t mySignedInt512 =-5;
    boost::multiprecision::uint512_t myUnsignedInt512 =6;
    boost::multiprecision::int1024_t mySignedInt1024 =-7;
    boost::multiprecision::uint1024_t myUnsignedInt1024 =8;
    boost::multiprecision::uint1024_t x{0};
    boost::multiprecision::uint1024_t y{1};
    int max{};
    std::cout<<"Tell me how many of the fibonacci numbers you would like me to print out: ";
    std::cin>>max;
    boost::multiprecision::uint1024_t next{};
    int c{1};
    for(int i{0}; i<max; i++)
    {
        if(i<=1)
        {
            next=i;
        }
        else
        {
            next=x+y;
            x=y;
            y=next;
        }
        std::cout<<"#" << c << ' ' << next << '\n';
        c++; //lols
    }
    return 0;
}
