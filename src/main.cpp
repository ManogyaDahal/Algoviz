#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

// Global variables
const int window_width = 800;
const int window_height = 600;

std::vector<int> array;
bool sorting = false;
int array_size = 50;
int compare_index1 = -1;
int compare_index2 = -1;
int delay = 100; // delay in milliseconds for visualization

// Function prototypes
void generateArray(int size);
void drawArray(sf::RenderWindow& window);
void insertionSort();

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

        // ImGui window for controls
        ImGui::Begin("Controls");
        if (ImGui::Button("Generate Array")) {
            generateArray(array_size);
        }
        if (ImGui::Button("Sort")) {
            sorting = true;
			delay = 3000/array_size;
            std::thread sortThread(insertionSort);
            sortThread.detach();
        }
        ImGui::SliderInt("Array Size", &array_size, 10, 100);
        ImGui::End();

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
    std::generate(array.begin(), array.end(), []() { return rand() % 400 + 10; });
}

void drawArray(sf::RenderWindow& window) {
	if(array.empty()) return;

	sf::Vector2u winSize = window.getSize();
    int bar_width = winSize.x / array.size(); // dynamically calculate bar width
	int min_height = 10; // Minimum bar height
    int max_height = winSize.y - 50; // Maximum bar height
    int min_value = *std::min_element(array.begin(), array.end());
    int max_value = *std::max_element(array.begin(), array.end());
    int range = max_value - min_value;

    for (int i = 0; i < array.size(); ++i) {
		float bar_height = min_height + (static_cast<float>(array[i] - min_value) / range) * (max_height - min_height);

        sf::RectangleShape bar(sf::Vector2f(bar_width - 2, array[i]));
        bar.setPosition(i * bar_width, winSize.y - array[i]);

        if (i == compare_index1 || i == compare_index2) {
            bar.setFillColor(sf::Color::Red);
        } else {
            bar.setFillColor(sf::Color::White);
        }

        window.draw(bar);
    }
}
void insertionSort() {
    for (int i = 1; i < array.size(); ++i) {
        int key = array[i];
        int j = i - 1;

        while (j >= 0 && array[j] > key) {
            compare_index1 = j;
            compare_index2 = j + 1;
            array[j + 1] = array[j];
            j = j - 1;

            // Update the visualizer
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        array[j + 1] = key;

        // Update the visualizer
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    sorting = false;
    compare_index1 = compare_index2 = -1;
}
