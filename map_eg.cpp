#include <map>
#include <vector>
#include <iostream>

using namespace std;

int main(void)
{
	map <int, vector<double> > a;
	a[5] = {1.,2.,3.};

	cout << "mapped element: ";
	for (auto i=a[5].begin(); i!=a[5].end(); ++i) {
		cout << (*i) << " ";
	}
	cout << endl;

	cout << "unmapped element: ";
	for (auto i=a[100].begin(); i!=a[100].end(); ++i) {
		cout << (*i) << " ";
	}
	cout << endl;
	

	return 0;
}
