#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

const sf::Vector2f windowSize(640, 480);
constexpr int border = 20;
int BPM = 130;

const sf::Vector2f barSize(20, 100);

int signature = 4;
int prevSignature = 4;
const int minSignature = 2;
const int maxSignature = 16;

int beat = 1;
const int minBPM = 30;
const int maxBPM = 300;

int main()
{
    sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "metronome");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);

    sf::RectangleShape bar(barSize);
    bar.setOrigin(0.5f * bar.getSize());
    bar.setPosition(border, windowSize.y / 2);
    bar.setFillColor(sf::Color(220, 220, 220));

    sf::Clock deltaClock;
    sf::Clock metronome;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        float step = (windowSize.x - (2 * border)) / (signature - 1);

        if (signature != prevSignature) {
            prevSignature = signature;
            beat = 1;
        }

        if (metronome.getElapsedTime().asSeconds() >= 60. / float(BPM)) {
            //TODO: Add beeps and boops, sound is easier to follow than just a flashing bar
            metronome.restart();

            if (beat == 1) {
                bar.setScale(sf::Vector2f(1, 1.5));
                bar.setFillColor(sf::Color(220, 220, 220));
                bar.setPosition(border, windowSize.y / 2);
            } else {
                bar.setScale(sf::Vector2f(1, 1));
                bar.setFillColor(sf::Color(120, 120, 120));
                bar.move(sf::Vector2f(step, 0));
            }

            if (beat == signature) {
                beat = 1;
            } else {
                beat++;
            }
        }

        ImGui::Begin("Settings");
        ImGui::InputInt("Beats per bar", &signature);
        ImGui::InputInt("BPM", &BPM);
        ImGui::End();

        if (signature > maxSignature) {
            signature = maxSignature;
        }

        if (signature < minSignature) {
            signature = minSignature;
        }

        if (BPM > maxBPM) {
            BPM = maxBPM;
        }

        if (BPM < minBPM) {
            BPM = minBPM;
        }
        window.clear();
        window.draw(bar);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}