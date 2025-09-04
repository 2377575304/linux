#include <iostream>
using namespace std;

class Animal {
	private:

	public:
		virtual void sound() const{
		cout<<"animals make a sound"<<endl;
		}

		virtual ~Animal(){
		cout <<"animal destory"<<endl;
		}

};

class Dog : public Animal {
	void sound() const override {
		cout << "Dog barks"<<endl;
	}
	~Dog()
	{
	cout << "dog destroyed"<<endl;
	};
};

int main ()
{
Animal * animalptr;
animalptr = new Dog();
animalptr ->sound();
delete animalptr;
	return 0;
}
