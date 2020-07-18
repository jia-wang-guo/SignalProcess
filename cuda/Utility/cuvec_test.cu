#include "Utility.h"

cuVector test(cuVector vec){
    return vec;
}
void test1(){
    cout << "test01" << endl;
    cuVector vec1(100,10);
    test(vec1);
}

void test2(){
    cuVector vec1(100,10);
    cuVector vec2(vec1);
    cuVector vec3 = vec2;
}


int main()
{
    test1();
    return 0;
}
