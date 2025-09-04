#include <iostream>
using namespace std;

class printdate
{
	private :
		int num;
	public :
		void print(int i)
		{cout<<"int si "<<i<<endl;}
		void print(double f)
		{cout<<"float is "<<f<<endl;}
		void print(char c[])
		{cout<<"char is "<<c<<endl;}
		void set_num(int n)
		{
		num = n;}

		int get_num()
		{
		return num;
		}

		printdate operator +(const printdate& other)
		{
		printdate pd;
		pd.num = this->num + other.num;
		return pd;
		}
};

int main ()
{
printdate pd1,pd2,pd;
pd.print(99);
pd.print(3.1415926);
char c[] = "what's your name and fuck you off!";
pd.print(c);
pd.set_num(666);
pd1.set_num(999);

pd2 =pd1+pd;
cout<<pd2.get_num()<<endl;

return 0;
}
