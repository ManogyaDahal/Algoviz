#include "sorting.hpp"
#include "global.hpp"
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

void insertionSort() {
    isSorted = false; 
    for (int i = 1; i < array.size(); ++i) {
        int key = array[i];
        int j = i - 1;

        while (j >= 0 && array[j] > key) {
            compare_index1 = j;
            compare_index2 = j + 1;
            array[j + 1] = array[j];
            j = j - 1;

            // Update swap information
            snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[j + 1], array[j]);

            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        array[j + 1] = key;

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    sorting = false;
    compare_index1 = compare_index2 = -1;
	isSorted = true;

}
void quickSort(int low, int high) {
	isSorted = false; 
	if (low < high) {
		int pi = partition(low, high);
		quickSort(low, pi - 1);
		quickSort(pi + 1, high);
	}
	compare_index1 = -1;
	compare_index2 = -1;
	isSorted = true;
}
int partition(int low, int high) {
	int pivot = array[high];
	int i = low - 1;
	for (int j = low; j < high; j++) {
		if (array[j] < pivot) {
			i++;
			std::swap(array[i], array[j]);
			compare_index1 = i;
			compare_index2 = j;
			snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[i], array[j]);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
	}
	std::swap(array[i + 1], array[high]);
	compare_index1 = i + 1;
	compare_index2 = high;
	snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[i + 1], array[high]);
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	return i + 1;
}

void timSort() {
	const int RUN = 32;
	int n = array.size();
	for (int i = 0; i < n; i += RUN) {
		insertionSort(i, std::min((i + 31), (n - 1)));
	}
	for (int size = RUN; size < n; size = 2 * size) {
		for (int left = 0; left < n; left += 2 * size) {
			int mid = left + size - 1;
			int right = std::min((left + 2 * size - 1), (n - 1));
			merge(left, mid, right);
		}
	}
	compare_index1 = -1;
	compare_index2 = -1;
	isSorted = true;
}

void insertionSort(int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = array[i];
        int j = i - 1;
        while (j >= left && array[j] > key) {
            array[j + 1] = array[j];
            j--;
            compare_index1 = j + 1;
            compare_index2 = j;
            snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[j + 1], array[j]);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        array[j + 1] = key;
    }
}


void merge(int left, int mid, int right) {
	int n1 = mid - left + 1;
	int n2 = right - mid;
	std::vector<int> L(n1), R(n2);
	for (int i = 0; i < n1; i++)
		L[i] = array[left + i];
	for (int i = 0; i < n2; i++)
		R[i] = array[mid + 1 + i];
	int i = 0, j = 0, k = left;
	while (i < n1 && j < n2) {
		if (L[i] <= R[j]) {
			array[k] = L[i];
			i++;
		} else {
			array[k] = R[j];
			j++;
		}
		snprintf(swapInfo, sizeof(swapInfo), "Merging %d and %d", L[i-1], R[j-1]);
		compare_index1 = k;
		compare_index2 = j;
		k++;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
	while (i < n1) {array[k] = L[i];
		i++;
		k++;
		compare_index1 = k;
		compare_index2 = i;
		snprintf(swapInfo, sizeof(swapInfo), "Merging %d", L[i-1]);
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
	while (j < n2) {
		array[k] = R[j];
		j++;
		k++;
		compare_index1 = k;
		compare_index2 = j;
		snprintf(swapInfo, sizeof(swapInfo), "Merging %d", R[j-1]);
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

