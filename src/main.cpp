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
#include "gui.hpp"
#include "global.hpp"
#include "arrayinfo.hpp"
#include "sorting.hpp"

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
