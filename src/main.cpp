#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

// Set window dimensions and background colors
constexpr int windowWidth = 1440;
constexpr int windowHeight = 480;
const sf::Vector2f windowSize(windowWidth, windowHeight);
const sf::Color background(40, 40, 40);
constexpr int margin = 50;

// Initial BPM setting, and bounds
int BPM = 120;
constexpr int minBPM = 30;
constexpr int maxBPM = 300;

// Initial time signature settings, and bounds
int signature = 4;
int prevSignature = 4;
constexpr int minSignature = 2;
constexpr int maxSignature = 32;

// Set size and color for the moving bar
const sf::Vector2f barSize(30, 300);
const sf::Color barColor(240, 240, 240);

// Set size and color for beat markers
const sf::Vector2f markerSize(2, 50);
const sf::Color markerColor(180, 180, 180);

const sf::Vector2f accentMarkerSize(4, 75);
const sf::Color accentMarkerColor(210, 210, 210);

// Initial values for the beat counter and array holding the accented beats
int beat = 1;
bool accents[maxSignature] = {false};

int main(void)
{
    // Initialize the main window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "metronome");
    window.setFramerateLimit(120);
    ImGui::SFML::Init(window);

    // Tell Dear ImGui not to make a .ini file, not needed for this application
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

    // Create the moving bar
    sf::RectangleShape bar(barSize);
    bar.setOrigin(0.5f * barSize);
    bar.setPosition(margin, windowHeight / 2);
    bar.setFillColor(barColor);

    // Create and place the center line
    sf::RectangleShape midLine(sf::Vector2f(windowWidth - (2 * margin), 1));
    midLine.setFillColor(markerColor);
    midLine.setOrigin(0.5f * midLine.getSize());
    midLine.setPosition(windowSize * 0.5f);

    // Create the beat marker rectangle
    // There is only ever a single rectangle which is moved around and drawn multiple times per frame,
    // instead of creating multiple instances of the same bar
    sf::RectangleShape marker(markerSize);
    marker.setFillColor(markerColor);
    marker.setOrigin(0.5f * markerSize);

    // Initialize first beat as accented
    accents[0] = true;
    
    // Create two sound buffers for the default click and the accent click, and read them from bundled .wav files
    // Might want to make an option to read other files?
    sf::SoundBuffer lobuf, hibuf;

    if (!lobuf.loadFromFile("snd/low.wav")) {
        return -1;
        }

    if (!hibuf.loadFromFile("snd/high.wav")) {
        return -1;
        }
    
    // Turn the buffers into playable SFML Sound objects
    sf::Sound high, low;
    
    high.setBuffer(hibuf);
    low.setBuffer(lobuf);
    
    // Initialize the clocks, one for keeping the time of the metronome itself and one to keep track of time passed per main loop pass
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

                // Reset all settings to initial values when R is pressed
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

        // Update the window once per loop, restart the loop time tracking clock
        ImGui::SFML::Update(window, deltaClock.restart());

        // Calculate spacing of beat markers along center line
        float step = midLine.getSize().x / (signature - 1);

        // Reset metronome if time signature is changed to prevent issues with timekeeping
        if (signature != prevSignature) {
            prevSignature = signature;
            beat = 1;
        }

        // If it's time for a new tic/toc
        if (metronome.getElapsedTime().asSeconds() >= 60. / float(BPM)) {
            metronome.restart();
            
            // Snap bar into place (in case I'm messing up the movement, this accounts for drift)
            bar.setPosition(sf::Vector2f(margin + (step * (beat - 1)), windowHeight / 2));

            // Play the high sound if we're on an accented beat, otherwise play the low sound
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

        // Shrink the bar and move it to the right a little every frame
        bar.setScale(0.97f * bar.getScale());
        float movestep = step * BPM / 7200;
        bar.move(sf::Vector2f(movestep, 0));

        // Clamp the movement so the bar doesn't move past the end of the centerline
        if (bar.getPosition().x > windowWidth - margin) {
            bar.setPosition(sf::Vector2f(windowWidth - margin, windowHeight / 2));
        }

        // Settings for inner ImGui windows
        ImGuiWindowFlags windowSettings = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        // Place first inner windows with a nice little margin from the top left corner of the main window
        ImGui::SetNextWindowPos(ImVec2(margin / 10, margin / 10));

        // Fill first inner window with an input field for time signature
        ImGui::Begin("Time signature", NULL, windowSettings);
        ImGui::Text("Time signature");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("", &signature);

        // Place next window with a nice little spacing right to the previous window
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + (margin / 10), margin / 10));
        ImGui::End();

        // Fill the second inner window with the BPM input field
        ImGui::Begin("BPM", NULL, windowSettings);
        ImGui::Text("BPM");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("", &BPM);

        // Place next window nicely spaced again, and set its size to 0x0 (Dear ImGui auto-resizes this)
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + (margin / 10), margin / 10));
        ImGui::SetNextWindowSize(ImVec2(0, 0));
        ImGui::End();

        // This window auto-resizes to fill with checkboxes to set/unset beats as accented
        ImGui::Begin(" Accents", NULL, windowSettings);
        ImGui::Text("Accents");
        
        for (int i = 0; i < signature; i++) {
            ImGui::PushID(i);
            ImGui::Checkbox("", &accents[i]);
            ImGui::PopID();
            ImGui::SameLine();
        }

        ImGui::End();

        // Input clamping
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

        // Draw the frame! Layers are background -> centerline -> beat markers -> metronome bar
        window.clear(background);
        window.draw(midLine);

        for (int i = 0; i < signature; i++) {
            marker.setPosition(margin + (step * i), windowHeight / 2);
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