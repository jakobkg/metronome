#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

const sf::Vector2f windowSize(960, 480);
const sf::Color background(50, 50, 50);
constexpr int margin = 50;
int BPM = 120;

const sf::Vector2f barSize(40, 200);
const sf::Vector2f markerSize(2, 50);

int signature = 4;
int prevSignature = 0;
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
    bar.setOrigin(0.5f * barSize);
    bar.setPosition(margin, windowSize.y / 2);
    bar.setFillColor(sf::Color(220, 220, 220));

    sf::RectangleShape midLine(sf::Vector2f(windowSize.x - (2 * margin), 1));
    midLine.setOrigin(0.5f * midLine.getSize());
    midLine.setPosition(windowSize * 0.5f);

    sf::RectangleShape marker(markerSize);
    marker.setFillColor(sf::Color(150, 150, 150));
    marker.setOrigin(0.5f * markerSize);
    

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

                if (event.key.code == sf::Keyboard::Up) {
                    BPM++;
                }

                if (event.key.code == sf::Keyboard::Down) {
                    BPM--;
                }

                if (event.key.code == sf::Keyboard::Left) {
                    signature--;
                }

                if (event.key.code == sf::Keyboard::Right) {
                    signature++;
                }
            }

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        float step = (windowSize.x - (2 * margin)) / (signature - 1);

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
                bar.setPosition(margin, windowSize.y / 2);
            } else {
                bar.setScale(sf::Vector2f(1, 1));
                bar.setFillColor(sf::Color(150, 150, 150));
                bar.move(sf::Vector2f(step, 0));
            }

            if (beat == signature) {
                beat = 1;
            } else {
                beat++;
            }
        }

        bar.setScale(0.98f * bar.getScale());

        ImGui::Begin("Settings");
        ImGui::InputInt("Beats per measure", &signature);
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

        window.clear(background);
        for (int i = 0; i < signature; i++) {
            marker.setPosition(margin + (step * i), windowSize.y / 2);
            window.draw(marker);
        }
        window.draw(midLine);
        window.draw(bar);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}