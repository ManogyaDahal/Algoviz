#include "arrayinfo.hpp"
#include "global.hpp"
#include <SFML/Graphics.hpp>

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
