#include <sstream>

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

const sf::Vector2f windowSize(1440, 480);
const sf::Color background(50, 50, 50);
constexpr int margin = 50;

int BPM = 120;
const int minBPM = 30;
const int maxBPM = 300;

const sf::Vector2f barSize(30, 300);
const sf::Vector2f markerSize(2, 50);

int signature = 4;
int prevSignature = 0;
const int minSignature = 2;
const int maxSignature = 32;

int beat = 1;

bool accents[maxSignature] = {false};

int main()
{
    sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "metronome");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

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

    accents[0] = true;
    
    sf::SoundBuffer lobuf, hibuf;

    if (!lobuf.loadFromFile("snd/low.wav")) {
        return -1;
        }

    if (!hibuf.loadFromFile("snd/high.wav")) {
        return -1;
        }
    
    sf::Sound high, low;
    
    high.setBuffer(hibuf);
    low.setBuffer(lobuf);
    

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
            metronome.restart();
            
            bar.setPosition(sf::Vector2f(margin + (step * (beat - 1)), windowSize.y / 2));

            if (accents[beat - 1]) {
                high.play();
                bar.setScale(sf::Vector2f(1, 1.5));
                bar.setFillColor(sf::Color(220, 220, 220));
            } else {
                low.play();
                bar.setScale(sf::Vector2f(1, 1));
                bar.setFillColor(sf::Color(150, 150, 150));
            }

            if (beat == signature) {
                beat = 1;
            } else {
                beat++;
            }
        }

        bar.setScale(0.97f * bar.getScale());
        bar.move(sf::Vector2f((windowSize.x - (2 * margin)) * 2.7 / (BPM * signature), 0));

        if (bar.getPosition().x > windowSize.x - margin) {
            bar.setPosition(sf::Vector2f(windowSize.x - margin, windowSize.y / 2));
        }

        ImGui::Begin("Settings");
        ImGui::InputInt("Beats per measure", &signature);
        ImGui::InputInt("BPM", &BPM);
        
        for (int i = 0; i < signature; i++) {
            ImGui::Checkbox("##", &accents[i]);
            ImGui::SameLine();
        }

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