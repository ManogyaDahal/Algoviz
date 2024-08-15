#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>
#include <mutex>

const unsigned int MIN_WINDOW_WIDTH = 1920;
const unsigned int MIN_WINDOW_HEIGHT = 1080;
const int window_width = 1920;
const int window_height = 1080;
const int margin = 25; 

std::vector<int> array;
bool sorting = false;
bool isSorted = false;
int array_size = 50;
int compare_index1 = -1;
int compare_index2 = -1;
int delay = 100;
char swapInfo[128] = ""; // Array to store swap information
char userInputArray[256] = ""; // Array to store user input array
int selectedSort = 0;

std::mutex mtx;

sf::Font font; 

void generateArray(int size);
void drawArray(sf::RenderWindow& window);
void insertionSort();
void ShowMenuBar(bool* p_open);
void displayUserInputBox(sf::RenderWindow& window);
void displaySwapInfoBox(sf::RenderWindow& window);
void displayTimeComplexityBox(sf::RenderWindow& window);
void generateArrayFromInput(const std::vector<int>& inputArray);
void quickSort(int low, int high);
int partition(int low, int high);
void timSort();
void insertionSort(int left, int right);
void merge(int left, int mid, int right);


int main() {
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Algoviz - Sorting Algorithm Visualizer", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

	if (!ImGui::SFML::Init(window)) {
		std::cerr << "Failed to initialize ImGui-SFML." << std::endl;
		return -1;
	}

    if(!font.loadFromFile("font/GeistMonoNerdFontMono-UltraBlack.otf")) {
	    std::cerr <<"Failed to load font" <<std::endl; 
	    return -1;
    }

    sf::Clock deltaClock;
	bool showMenuBar = true;

	generateArray(array_size);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized)
			{
				if (event.size.width < MIN_WINDOW_WIDTH || event.size.height < MIN_WINDOW_HEIGHT)
				{
					unsigned int newWidth = event.size.width < MIN_WINDOW_WIDTH ? MIN_WINDOW_WIDTH : event.size.width;
					unsigned int newHeight = event.size.height < MIN_WINDOW_HEIGHT ? MIN_WINDOW_HEIGHT : event.size.height;
					window.setSize(sf::Vector2u(newWidth, newHeight));
				}
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		if (showMenuBar)
		{
			ShowMenuBar(&showMenuBar);
		}

		displayUserInputBox(window);
		displaySwapInfoBox(window);
		displayTimeComplexityBox(window);

		window.clear();
		drawArray(window);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}

void generateArray(int size) {
    array.clear();
    array.resize(size);
    std::generate(array.begin(), array.end(), []() { return rand() % 91 + 10; });
    isSorted = false; 
}

void generateArrayFromInput(const std::vector<int>& inputArray) {
	isSorted=false; 
    array = inputArray;
    array_size = array.size();
}

void drawArray(sf::RenderWindow& window) {
    if (array.empty()) return;

    sf::Vector2u winSize = window.getSize();
    int numBars = array.size();
    float bar_width = static_cast<float>(winSize.x - 2 * margin - 185) / static_cast<float>(numBars); 
    int min_height = 10;
    int max_height = winSize.y - 2 * margin - 70;
    int min_value = *std::min_element(array.begin(), array.end());
    int max_value = *std::max_element(array.begin(), array.end());
    float range = static_cast<float>(max_value - min_value);

    sf::Text valueText;
    valueText.setFont(font);
    valueText.setFillColor(sf::Color::Black); // Set text color to black

    for (int i = 0; i < numBars; ++i) {
        float normalized_height = min_height + ((static_cast<float>(array[i]) - min_value) / range) * (max_height - min_height);
        float bar_height = std::max(static_cast<float>(min_height), std::min(static_cast<float>(max_height), normalized_height));

        sf::RectangleShape bar(sf::Vector2f(bar_width - 2.0f, bar_height));
        bar.setPosition(i * bar_width + margin, winSize.y - bar_height - margin);

        if ((i == compare_index1 || i == compare_index2) && (sorting==true)) {
            bar.setFillColor(sf::Color::Red);
        } else {
            bar.setFillColor(sf::Color::White);
        }

        if (isSorted) {
            bar.setFillColor(sf::Color::Cyan); // After sorting is complete
        }

        window.draw(bar);

        // Set the string to display the value
        valueText.setString(std::to_string(array[i]));

        // Calculate the maximum font size that fits within the bar
        sf::FloatRect textBounds = valueText.getLocalBounds();
        int maxCharacterSizeWidth = static_cast<int>((bar_width - 4) / (textBounds.width / valueText.getCharacterSize()));
        int maxCharacterSizeHeight = static_cast<int>((bar_height - 4) / (textBounds.height / valueText.getCharacterSize()));
        int characterSize = std::min(maxCharacterSizeWidth, maxCharacterSizeHeight);

        // Limit the maximum character size to 20
        characterSize = std::min(20, characterSize);
        characterSize = std::max(8, characterSize); // Ensure it is not smaller than 8

        // Set the calculated character size
        valueText.setCharacterSize(characterSize);

        // Recalculate the text bounds with the new size
        sf::FloatRect textBoundsAfterSize = valueText.getLocalBounds();
        float textWidth = textBoundsAfterSize.width;
        float textHeight = textBoundsAfterSize.height;

        // Set the text position to be centered horizontally and positioned at the top inside the bar
        float barLeft = i * bar_width + margin;
        float barTop = winSize.y - bar_height - margin;

        valueText.setPosition(
            barLeft + (bar_width - 2.0f - textWidth) / 2, // Center horizontally
            barTop + 2 // Position at the top with a small margin inside the bar
        );

        window.draw(valueText);
    }
}

void ShowMenuBar(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 60), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    if (ImGui::Begin("Menu Bar", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        float padding = 20.0f;
        float margin = 20.0f;
        float itemWidth = 150.0f;
        float itemHeight = 30.0f;
        float buttonWidth = 100.0f;
        float totalItemsWidth = itemWidth * 3 + buttonWidth * 2;

        float remainingWidth = windowSize.x - totalItemsWidth - 2 * margin;
        float spacing = remainingWidth / 6.0f;

        ImGui::SetCursorPos(ImVec2(margin, (60 - itemHeight) / 2));
        ImGui::Text("SortVisualizer");

        ImGui::SameLine();
        ImGui::SetCursorPosX(margin + ImGui::CalcTextSize("SortVisualizer").x + spacing);
        const char* items[] = { "Quick Sort", "Insertion Sort", "Tim Sort" };
        ImGui::PushItemWidth(itemWidth);
        ImGui::Combo("##Combo", &selectedSort, items, IM_ARRAYSIZE(items));
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::SetCursorPosX(margin + ImGui::CalcTextSize("SortVisualizer").x + itemWidth + spacing * 2);
        ImGui::Text("Delay:");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("##delay", &delay, 1, 1000);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::SetCursorPosX(margin + ImGui::CalcTextSize("SortVisualizer").x + itemWidth * 2 + spacing * 3);
        if (ImGui::Button("Randomize", ImVec2(buttonWidth, itemHeight))) {
            generateArray(array_size);
            sorting = false;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(windowSize.x - margin - buttonWidth - buttonWidth - spacing);
        if (ImGui::Button("Start", ImVec2(buttonWidth, itemHeight))) {
            sorting = true;
            //            delay = 3000 / array_size;
            std::thread sortThread;
            if (selectedSort == 0) {
                sortThread = std::thread(quickSort, 0, array.size() - 1);
            } else if (selectedSort == 1) {
                sortThread = std::thread([]{insertionSort();});
            } else if (selectedSort == 2) {
                sortThread = std::thread(timSort);
            }
            sortThread.detach();
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(windowSize.x - margin - buttonWidth);
        if (ImGui::Button("Stop", ImVec2(buttonWidth, itemHeight))) {
            sorting = false;
            isSorted = false;
            compare_index1 = -1;
            compare_index2 = -1;
            memset(swapInfo, 0, sizeof(swapInfo)); // Clear swap information
            memset(userInputArray, 0, sizeof(userInputArray)); // Clear user input array
        }

        ImGui::SetCursorPos(ImVec2(margin, (60 - itemHeight) / 2 + itemHeight + 10));
    }
    ImGui::End();
}

void displayUserInputBox(sf::RenderWindow& window) {
	sf::Vector2u winSize = window.getSize();

	sf::RectangleShape userInputBox(sf::Vector2f(170, 175));
	userInputBox.setPosition(winSize.x - 190, 60);
	userInputBox.setFillColor(sf::Color(200, 200, 200));
	userInputBox.setOutlineThickness(1.f);
	userInputBox.setOutlineColor(sf::Color::Black);
	window.draw(userInputBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185, 65));
	ImGui::SetNextWindowSize(ImVec2(160, 165));
	ImGui::Begin("User Input Array", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Array Size:");
	ImGui::SliderInt("", &array_size, 10, 100);
	ImGui::Dummy(ImVec2(0, 10));

	ImGui::InputText("Input", userInputArray, IM_ARRAYSIZE(userInputArray));
	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::Button("Generate", ImVec2(140, 30))) {
		std::vector<int> inputArray;
		std::stringstream ss(userInputArray);
		int value;
		while (ss >> value) {
			inputArray.push_back(value);
			if (ss.peek() == ',') ss.ignore();
		}
		generateArrayFromInput(inputArray);
	}

	/*    if (ImGui::Button("Use Input", ImVec2(140, 30))) {
		  std::vector<int> inputArray;
		  std::istringstream iss(userInputArray);
		  int num;
		  while (iss >> num) {
		  inputArray.push_back(num);
		  }
		  if (!inputArray.empty()) {
		  generateArrayFromInput(inputArray);
		  }
		  }*/
	ImGui::End();
}

void displaySwapInfoBox(sf::RenderWindow& window) {
	sf::Vector2u winSize = window.getSize();

	sf::RectangleShape swapInfoBox(sf::Vector2f(170, 90));
	swapInfoBox.setPosition(winSize.x - 190, 240);
	swapInfoBox.setFillColor(sf::Color(200, 200, 200));
	swapInfoBox.setOutlineThickness(1.f);
	swapInfoBox.setOutlineColor(sf::Color::Black);
	window.draw(swapInfoBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185, 245));
	ImGui::SetNextWindowSize(ImVec2(160, 80));
	ImGui::Begin("Swap Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Swap Information:");
	ImGui::Text("%s",swapInfo);
	ImGui::End();
}

void displayTimeComplexityBox(sf::RenderWindow& window) {
	sf::Vector2u winSize = window.getSize();

	sf::RectangleShape timeComplexityBox(sf::Vector2f(170, 90));
	timeComplexityBox.setPosition(winSize.x - 190, 340);
	timeComplexityBox.setFillColor(sf::Color(200, 200, 200));
	timeComplexityBox.setOutlineThickness(1.f);
	timeComplexityBox.setOutlineColor(sf::Color::Black);
	window.draw(timeComplexityBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185, 345));
	ImGui::SetNextWindowSize(ImVec2(160, 80));
	ImGui::Begin("Time Complexity", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Time Complexity:");
	if (selectedSort == 0) {
		ImGui::TextUnformatted("O(n log n)");
	} else if (selectedSort == 1) {
		ImGui::TextUnformatted("O(n^2)");
	} else if (selectedSort == 2) {
		ImGui::TextUnformatted("O(n log n)");
	}
	ImGui::End();
}

void insertionSort() {
    isSorted = false; 
    for (int i = 1; i < array.size(); ++i) {
        int key = array[i];
        int j = i - 1;

        while (j >= 0 && array[j] > key) {
            compare_index1 = j;
            compare_index2 = j + 1;
	    std::swap(array[j], array[j+1]); 

            snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[j + 1], array[j]);


            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            j = j - 1;
            if (!sorting) {
                return; // exit the function
            }
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
    if (!sorting) {
        isSorted = false;
        compare_index1 = -1;
        compare_index2 = -1;
        return; // exit the function
    }
    if (low < high) {
        int pi = partition(low, high);
        if (pi == -1) {
            return; // exit the function
        }
        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
    isSorted = true;
    compare_index1 = -1;
    compare_index2 = -1;
}

int partition(int low, int high) {
    int pivot = array[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (!sorting) {
            return -1; // exit the function
        }
        if (array[j] < pivot) {
            i++;
            std::swap(array[i], array[j]);
            compare_index1 = i;
            compare_index2 = j;
            snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[i], array[j]);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }
    if (!sorting) {
        return -1; // exit the function
    }
    std::swap(array[i + 1], array[high]);
    compare_index1 = i + 1;
    compare_index2 = high;
    snprintf(swapInfo, sizeof(swapInfo), "Swapping %d with %d", array[i + 1], array[high]);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    return i + 1;
}

void timSort() {
    if (!sorting) {
        isSorted = false;
        compare_index1 = -1;
        compare_index2 = -1;
        return; // exit the function
    }
    const int RUN = 32;
    int n = array.size();
    for (int i = 0; i < n; i += RUN) {
        insertionSort(i, std::min((i + 31), (n - 1)));
        if (!sorting) {
            isSorted = false;
            compare_index1 = -1;
            compare_index2 = -1;
            return; // exit the function
        }
    }
    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min((left + 2 * size - 1), (n - 1));
            merge(left, mid, right);
            if (!sorting) {
                isSorted = false;
                compare_index1 = -1;
                compare_index2 = -1;
                return; // exit the function
            }
        }
    }
    isSorted = true;
    compare_index1 = -1;
    compare_index2 = -1;
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
            if (!sorting) {
                return; // exit the function
            }
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
        if (!sorting) {
            return; // exit the function
        }
    }
    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
        compare_index1 = k;
        compare_index2 = i;
        snprintf(swapInfo, sizeof(swapInfo), "Merging %d", L[i-1]);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if (!sorting) {
            return; // exit the function
        }
    }
    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
        compare_index1 = k;
        compare_index2 = j;
        snprintf(swapInfo, sizeof(swapInfo), "Merging %d", R[j-1]);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if (!sorting) {
            return; // exit the function
        }
    }
}
