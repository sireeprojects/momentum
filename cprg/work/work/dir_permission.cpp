#include <errno.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv)
{
    int result = access("/root/", W_OK);
    if (result == 0)
    {
        std::cout << "Can W_OK" << std::endl;
    }
    else
    {
        std::cout << "Can't W_OK: " << result << std::endl;
    }
}
