#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
extern int margin;
extern bool isSorted;
extern int compare_index1;
extern int compare_index2;

constexpr unsigned int window_width = 1280;
constexpr unsigned int window_height = 720;
constexpr unsigned int MIN_WINDOW_WIDTH = 800;
constexpr unsigned int MIN_WINDOW_HEIGHT = 600;
extern std::vector<sf::RectangleShape> bars;
extern std::vector<int> array;
extern bool sorting;
extern char userInputArray[256];
extern char swapInfo[256];
extern char complexityText[256];
extern int array_size;
extern int delay;
extern int selectedSort;
extern sf::Font font;
