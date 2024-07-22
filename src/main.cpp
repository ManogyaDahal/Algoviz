#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>

const int window_width = 1000;
const int window_height = 600;
const int margin = 20; // Margin around the window

std::vector<int> array;
bool sorting = false;
bool isSorted =false; 
int array_size = 50;
int compare_index1 = -1;
int compare_index2 = -1;
int delay = 100;
char swapInfo[128] = ""; // Array to store swap information
char userInputArray[256] = ""; // Array to store user input array

void generateArray(int size);
void drawArray(sf::RenderWindow& window);
void insertionSort();
void displayInfoBoxes(sf::RenderWindow& window);
void displayUserInputBox(sf::RenderWindow& window);

int main() {
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Insertion Sort Visualizer");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Display boxes on the right side
        displayUserInputBox(window);
        displayInfoBoxes(window);

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
    std::generate(array.begin(), array.end(), []() { return rand() % 91+ 10; });
    snprintf(userInputArray, sizeof(userInputArray), "Generated array of size %d", size);
    isSorted = false; 
}

void drawArray(sf::RenderWindow& window) {
    if (array.empty()) return;

    sf::Vector2u winSize = window.getSize();
    int numBars = array.size();
    float bar_width = static_cast<float>(winSize.x - 2 * margin - 170) / static_cast<float>(numBars); // Adjusted for margins and info boxes
    int min_height = 10;
    int max_height = winSize.y - 2 * margin;
    int min_value = *std::min_element(array.begin(), array.end());
    int max_value = *std::max_element(array.begin(), array.end());
    float range = static_cast<float>(max_value - min_value);

    for (int i = 0; i < numBars; ++i) {
        float normalized_height = min_height + ((static_cast<float>(array[i]) - min_value) / range) * (max_height - min_height);
        float bar_height = std::max(static_cast<float>(min_height), std::min(static_cast<float>(max_height), normalized_height));

        sf::RectangleShape bar(sf::Vector2f(bar_width - 2.0f, bar_height));
        bar.setPosition(i * bar_width + margin, winSize.y - bar_height - margin);

        if (i == compare_index1 || i == compare_index2) {
            bar.setFillColor(sf::Color::Red);
        } else {
            bar.setFillColor(sf::Color::White);
        }
        if (isSorted == true){
             bar.setFillColor(sf::Color::Green);
        }

        window.draw(bar);

    }
}

void displayInfoBoxes(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();

    // Box for swap information
    sf::RectangleShape swapBox(sf::Vector2f(170, 175));
    swapBox.setPosition(winSize.x - 190, 210);
    swapBox.setFillColor(sf::Color(200, 200, 200));
    swapBox.setOutlineThickness(1.f);
    swapBox.setOutlineColor(sf::Color::Black);
    window.draw(swapBox);

    // Display swap information
    ImGui::SetNextWindowPos(ImVec2(winSize.x -185,215));
    ImGui::SetNextWindowSize(ImVec2(160, 165));
    ImGui::Begin("Swap Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted("Swap Information:");
    ImGui::TextUnformatted(swapInfo);
    ImGui::End();

    // Box for time complexity information
    sf::RectangleShape timeComplexityBox(sf::Vector2f(170, 175));
    timeComplexityBox.setPosition(winSize.x -190, 400);
    timeComplexityBox.setFillColor(sf::Color(200, 200, 00));
    timeComplexityBox.setOutlineThickness(1.f);
    timeComplexityBox.setOutlineColor(sf::Color::Black);
    window.draw(timeComplexityBox);

    // Display time complexity information
    ImGui::SetNextWindowPos(ImVec2(winSize.x - 185,405));
    ImGui::SetNextWindowSize(ImVec2(160, 165));
    ImGui::Begin("Complexity Information", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted("Time Complexity:");
    ImGui::Text("O(n^2)"); // Example for insertion sort
    ImGui::TextUnformatted("Space Complexity:");
    ImGui::Text("O(1)"); // Example for insertion sort
    ImGui::End();
}

void displayUserInputBox(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();

    // Box for user input array
    sf::RectangleShape userInputBox(sf::Vector2f(170, 175));
    userInputBox.setPosition(winSize.x - 190, 20);
    userInputBox.setFillColor(sf::Color(200, 200, 200));
    userInputBox.setOutlineThickness(1.f);
    userInputBox.setOutlineColor(sf::Color::Black);
    window.draw(userInputBox);

    // Display user input array information
    ImGui::SetNextWindowPos(ImVec2(winSize.x - 185, 25));
    ImGui::SetNextWindowSize(ImVec2(160, 165));
    ImGui::Begin("User Input Array", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted("Array Size:");
    ImGui::Dummy(ImVec2(0, 10)); // Add space
    ImGui::SliderInt("", &array_size, 10, 100);
    ImGui::Dummy(ImVec2(0, 10)); // Add space

    if (ImGui::Button("Generate Array")) {
        generateArray(array_size);
    }
    ImGui::Dummy(ImVec2(0, 10)); // Add space
    if (ImGui::Button("Sort")) {
        sorting = true;
        delay = 3000 / array_size;
        std::thread sortThread(insertionSort);
        sortThread.detach();
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




