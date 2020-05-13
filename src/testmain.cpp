#include <cstdio>
#include "config.h"

using namespace std;

int main()
{
    try
    {
        throw 1;
    }
    catch(int a)
    {
        printf("%d\n", a);
    }
}