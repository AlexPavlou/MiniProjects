#include <iostream>
#include <string.h>
#include <bits/stdc++.h>

int main()
{
    std::cout<<"Please input your card number: ";
    std::string card_number{};
    std::cin>>card_number;
    int length {(int)card_number.length()};
    int Sum{};
    // Gets first four, first three, first two and the first digit of the argument
    std::string first_four = card_number.substr(0,4);
    std::string first_three = card_number.substr(0,3);
    std::string first_two = card_number.substr(0,2);
    std::string first = card_number.substr(0,1);
    bool isSecond = false;
    for (int i = length - 1; i >= 0; i--)
    {

        int d = card_number[i] - '0';
        if (isSecond == true) {
            d = d * 2;
        }
        Sum += d / 10;
        Sum += d % 10;
        isSecond = !isSecond;
    }

    /* Matches criteria to examine if the entered card number is valid,
    * and it also mentions the card's company etc
    */

    if(Sum%10==0)
    {
        std::cout<<"Your card is valid. ";
    } else {
        std::cout<<"Your card is not valid. ";
        return 0;
    }

    std::cout<<"Your card's length is: " << length << " numbers, its first 3 digits are: " << first_three << ". ";

    /*info can be found in https://www.ibm.com/docs/en/order-management-sw/9.3.0?topic=cpms-handling-credit-cards*/

    if(length==13) {
        std::cout<<" Thus, your card is Visa. ";
        return 0;
    }
    if(length==14) {
        std::cout<<" Thus, your card is Diners Club and Carte Blanche. ";
        return 0;
    }
    if(length==15)
    {
        if(first_two =="34" || first_two == "37")
        {   std::cout<<"Your card is American Express. ";
            return 0;
        }
        else {
            std::cout<<"Your card is JCB. ";
            return 0;
        }
    }
    if(length==16)
    {
        if(first_four=="6011") {
            std::cout<<"Your card is Discover. ";
            return 0;
        }
        if(first=="4") {
            std::cout<<"Thus, your card is Visa. ";
            return 0;
        }
        if(first=="3") {
            std::cout<<"Thus, your card is JCB. ";
            return 0;
        }
        else {
            std::cout<<"Your card is MasterCard. ";
            return 0;
        }
    }
}
