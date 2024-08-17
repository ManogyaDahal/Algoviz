#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <thread>
#include <sstream>
#include "gui.hpp"
#include "global.hpp"
#include "arrayinfo.hpp"
#include "sorting.hpp"

void ShowMenuBar(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 60), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    if (ImGui::Begin("Menu Bar", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
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

	sf::RectangleShape userInputBox(sf::Vector2f(170+12, 175));
	userInputBox.setPosition(winSize.x - 190+12, 70);
	userInputBox.setFillColor(sf::Color(200, 200, 200));
	userInputBox.setOutlineThickness(1.f);
	userInputBox.setOutlineColor(sf::Color::Black);
	window.draw(userInputBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185+12, 75));
	ImGui::SetNextWindowSize(ImVec2(160+12, 165));
	ImGui::Begin("User Input Array", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Array Size:");
	ImGui::SliderInt("", &array_size, 10, 100);
	ImGui::Dummy(ImVec2(0, 10));

	ImGui::InputText("Input", userInputArray, IM_ARRAYSIZE(userInputArray));
	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::Button("Generate", ImVec2(140+12, 30))) {
		std::vector<int> inputArray;
		std::stringstream ss(userInputArray);
		int value;
		while (ss >> value) {
			inputArray.push_back(value);
			if (ss.peek() == ',') ss.ignore();
		}
		generateArrayFromInput(inputArray);
	}
	ImGui::End();
}

void displaySwapInfoBox(sf::RenderWindow& window) {
	sf::Vector2u winSize = window.getSize();

	sf::RectangleShape swapInfoBox(sf::Vector2f(170+12, 90));
	swapInfoBox.setPosition(winSize.x - 190+12, 245);
	swapInfoBox.setFillColor(sf::Color(200, 200, 200));
	swapInfoBox.setOutlineThickness(1.f);
	swapInfoBox.setOutlineColor(sf::Color::Black);
	window.draw(swapInfoBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185+12, 250));
	ImGui::SetNextWindowSize(ImVec2(160+12, 80));
	ImGui::Begin("Swap Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Swap Information:");
	ImGui::Text("%s",swapInfo);
	ImGui::End();
}

void displayTimeComplexityBox(sf::RenderWindow& window) {
	sf::Vector2u winSize = window.getSize();

	sf::RectangleShape timeComplexityBox(sf::Vector2f(170+12, 90-16));
	timeComplexityBox.setPosition(winSize.x - 190+12, 350-16);
	timeComplexityBox.setFillColor(sf::Color(200, 200, 200));
	timeComplexityBox.setOutlineThickness(1.f);
	timeComplexityBox.setOutlineColor(sf::Color::Black);
	window.draw(timeComplexityBox);

	ImGui::SetNextWindowPos(ImVec2(winSize.x - 185+12, 355-16));
	ImGui::SetNextWindowSize(ImVec2(160+12, 80-16));
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
