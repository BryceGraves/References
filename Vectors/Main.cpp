// Bryce Graves
// A program that recieves data and returns useful values.

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

const int DATA_POINTS = 100;

float average(vector<int> data);
int mode(vector<int> data);
int median(vector<int> data);
int minimum(vector<int> data);
int maximum(vector<int> data);
//int range(vector<int> data);
void test();

int main(int argc, const char *argv[]) {
	vector<int> data;

	for (int i = 0; i < DATA_POINTS; ++i) {
		int r = rand();
		cout << "New data : " << r << endl;
		data.push_back(r);
	}

	cout << "Vector contains " << data.size() << " values:\n";
	cout << "Average: " << average(data) << endl;
	cout << "Mode: " << mode(data) << endl;
	cout << "Median: " << median(data) << endl;
	cout << "Minimum: " << minimum(data) << endl;
	cout << "Maximum: " << maximum(data) << endl;
	cout << "Range: " << (maximum(data) - minimum(data)) << endl;

	test();

	return 0;
}

float average(vector<int> data) {
	float average = (accumulate(data.begin(), data.end(), 0.0) / data.size());
	return average;
}

//Checks to see what value appears most, if two or all appear the same amount of times it returns 0.
int mode(vector<int> data) {
	sort(data.begin(), data.end());
	int mode = data[0];
	int newMode = NULL;
	int count = 0;
	int newCount = 0;
	for (int i = 0; i < data.size(); ++i) {
		if (mode != data[i]) {
			if (newMode != data[i]) {
				newMode = data[i];
				newCount = 1;
			}
			else {
				++newCount;
				if (newCount > count) {
					mode = data[i];
					++count;
					newCount = 0;
				}
			}
		}
		else {
			++count;
		}
		if (data[i] = data[data.size() - 1]) {
			if (count == newCount) {
				mode = NULL;
			}
		}
	}
	return mode;
}

int median(vector<int> data) {
	int median = data.at(data.size() / 2);
	//int median = data[data.size() / 2];
	return median;
}

int minimum(vector<int> data) {
	//sort(data.begin(), data.end());
	int minimum = *min_element(data.begin(), data.end());
	return minimum;
}

int maximum(vector<int> data) {
	//sort(data.begin(), data.end());
	int maximum = *max_element(data.begin(), data.end());
	return maximum;
}

/*int range(vector<int> data) {

}*/


// Test functions. Ignore these, and don't modify them.

void testAverage(vector<int> data) {
	cout << "testAverage: ";
	if (18 == (int)average(data)) {
		cout << "PASSED\n";
	}
	else {
		cout << "FAILED\n";
	}
}

void testMedian(vector<int> data) {
	cout << "testMedian: ";
	if (median(data) == 16) {
		cout << "PASSED\n";
	}
	else {
		cout << "FAILED\n";
	}
}

void testMinimum(vector<int> data) {
	cout << "testMinimum: ";
	if (minimum(data) == 4) {
		cout << "PASSED\n";
	}
	else {
		cout << "FAILED\n";
	}
}

void testMaximum(vector<int> data) {
	cout << "testMaximum: ";
	if (maximum(data) == 42) {
		cout << "PASSED\n";
	}
	else {
		cout << "FAILED\n";
	}
}

void test() {
	int seedData[] = { 4, 8, 15, 16, 23, 42 };
	vector<int> testData;
	testData.assign(seedData, seedData + 6);
	testAverage(testData);
	testMedian(testData);
	testMinimum(testData);
	testMaximum(testData);
}
