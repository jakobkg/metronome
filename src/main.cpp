#include <sstream>

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


constexpr int windowWidth = 1440;
constexpr int windowHeight = 480;
const sf::Vector2f windowSize(windowWidth, windowHeight);
const sf::Color background(40, 40, 40);
constexpr int margin = 50;

int BPM = 120;
constexpr int minBPM = 30;
constexpr int maxBPM = 300;

const sf::Vector2f barSize(30, 300);
const sf::Vector2f markerSize(2, 50);
const sf::Vector2f accentMarkerSize(4, 75);
const sf::Color markerColor(180, 180, 180);
const sf::Color accentMarkerColor(210, 210, 210);

int signature = 4;
int prevSignature = 0;
constexpr int minSignature = 2;
constexpr int maxSignature = 32;

int beat = 1;

bool accents[maxSignature] = {false};

int main()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "metronome");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

    sf::RectangleShape bar(barSize);
    bar.setOrigin(0.5f * barSize);
    bar.setPosition(margin, windowSize.y / 2);
    bar.setFillColor(sf::Color(240, 240, 240));

    sf::RectangleShape midLine(sf::Vector2f(windowSize.x - (2 * margin), 1));
    midLine.setFillColor(sf::Color(180, 180, 180));
    midLine.setOrigin(0.5f * midLine.getSize());
    midLine.setPosition(windowSize * 0.5f);

    sf::RectangleShape marker(markerSize);
    marker.setFillColor(markerColor);
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

                if (event.key.code == sf::Keyboard::R) {
                    BPM = 120;
                    signature = 4;
                    accents[0] = true;

                    for (int index = 1; index < maxSignature; index++) {
                        accents[index] = false;
                    }
                }
            }

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        float step = midLine.getSize().x / (signature - 1);

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
        float movestep = step * BPM / 7200;
        bar.move(sf::Vector2f(movestep, 0));

        if (bar.getPosition().x > windowSize.x - margin) {
            bar.setPosition(sf::Vector2f(windowSize.x - margin, windowSize.y / 2));
        }

        ImGuiWindowFlags windowSettings = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImGui::SetNextWindowPos(ImVec2(10, 10));

        ImGui::Begin("Time Signature", NULL, windowSettings);
        ImGui::Text("Time signature");
        //ImGui::PushID("sign");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("", &signature);
        ImGui::PopItemWidth();
        //ImGui::PopID();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + 10 + 10, 10));
        ImGui::SetNextWindowSize(ImVec2(0, 0));
        ImGui::End();


        ImGui::Begin("BPM", NULL, windowSettings);
        ImGui::Text("BPM");
        //ImGui::PushID("BPM");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("", &BPM);
        ImGui::PopItemWidth();
        //ImGui::PopID();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + 10 + 10, 10));
        ImGui::SetNextWindowSize(ImVec2(0, 0));

        ImGui::End();

        ImGui::Begin(" Accents", NULL, windowSettings);
        ImGui::Text("Accents");
        
        for (int i = 0; i < signature; i++) {
            ImGui::PushID(i);
            ImGui::Checkbox("", &accents[i]);
            ImGui::PopID();
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

        window.draw(midLine);

        for (int i = 0; i < signature; i++) {
            marker.setPosition(margin + (step * i), windowSize.y / 2);
            window.draw(marker);

            if (accents[i]) {
                marker.setScale(2.f, 1.5f);
                marker.setFillColor(accentMarkerColor);
                window.draw(marker);
                marker.setScale(1.f, 1.f);
                marker.setFillColor(markerColor);
            }
        }

        window.draw(bar);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}