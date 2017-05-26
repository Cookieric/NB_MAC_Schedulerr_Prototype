#include <stdio.h>

int z = -7;
void func2(int* *x)
{
    *x = &z;
}

void func1()
{
    int  y = 42;
    int* x = &y;
    printf( "x = %d\n", *x );
    func2( &x );
    printf( "x = %d\n", *x );
}

int main()
{
    func1();
    return 0;
}
// // list::push_back
// #include <iostream>
// #include <list>

// int main ()
// {
// 	std::list<int> mylist;
// 	int myint;
// 	std::cout << "Please enter some integers (enter 0 to end):\n";
// 	std::cin >> myint;
// 	mylist.push_back (myint);
//   	std::cout << "mylist stores " << mylist.size() << " numbers.\n";

//   return 0;
// }
