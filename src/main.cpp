#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <thread>
#include <atomic>

const int WIDTH = 1200, HEIGHT = 800;
const int MAX_ITER = 256;

enum FractalType { MANDELBROT, JULIA, BURNING_SHIP };
std::string fractalNames[] = {"Mandelbrot Set", "Julia Set", "Burning Ship"};

double juliaRe = -0.7, juliaIm = 0.27015;

sf::Color palette(int iter, int maxIter) {
    if(iter == maxIter) return sf::Color::Black;
    float t = (float)iter / maxIter;
    // Smooth psychedelic coloring
    float r = 0.5f + 0.5f * std::cos(6.2831f * (t * 3.0f + 0.0f));
    float g = 0.5f + 0.5f * std::cos(6.2831f * (t * 3.0f + 0.33f));
    float b = 0.5f + 0.5f * std::cos(6.2831f * (t * 3.0f + 0.67f));
    return sf::Color(
        (uint8_t)(r * 255),
        (uint8_t)(g * 255),
        (uint8_t)(b * 255)
    );
}

int iterate(double cx, double cy, FractalType type, int maxIter) {
    double zx = 0, zy = 0;
    if(type == JULIA) { zx = cx; zy = cy; cx = juliaRe; cy = juliaIm; }
    for(int i = 0; i < maxIter; i++) {
        double zx2 = zx*zx, zy2 = zy*zy;
        if(zx2 + zy2 > 4.0) return i;
        if(type == BURNING_SHIP) {
            zy = std::abs(2.0*zx*zy) + cy;
            zx = zx2 - zy2 + cx;
        } else {
            zy = 2.0*zx*zy + cy;
            zx = zx2 - zy2 + cx;
        }
    }
    return maxIter;
}

void renderTile(std::vector<uint8_t>& pixels, int y0, int y1,
    double centerX, double centerY, double zoom,
    FractalType type, int maxIter)
{
    for(int y = y0; y < y1; y++) {
        for(int x = 0; x < WIDTH; x++) {
            double cx = centerX + (x - WIDTH/2.0) / zoom;
            double cy = centerY + (y - HEIGHT/2.0) / zoom;
            int iter = iterate(cx, cy, type, maxIter);
            sf::Color col = palette(iter, maxIter);
            int idx = (y * WIDTH + x) * 4;
            pixels[idx]   = col.r;
            pixels[idx+1] = col.g;
            pixels[idx+2] = col.b;
            pixels[idx+3] = 255;
        }
    }
}

void render(std::vector<uint8_t>& pixels, double cx, double cy, double zoom,
    FractalType type, int maxIter)
{
    int nThreads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> threads;
    int rowsPerThread = HEIGHT / nThreads;
    for(int t = 0; t < nThreads; t++) {
        int y0 = t * rowsPerThread;
        int y1 = (t == nThreads-1) ? HEIGHT : y0 + rowsPerThread;
        threads.emplace_back(renderTile, std::ref(pixels), y0, y1, cx, cy, zoom, type, maxIter);
    }
    for(auto& th : threads) th.join();
}

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH, (unsigned)HEIGHT}),
        "Fractal Explorer | Scroll: Zoom | Drag: Pan | 1-3: Fractal | C: Color Cycle | J: Julia mode");
    window.setFramerateLimit(60);

    double centerX = -0.5, centerY = 0.0;
    double zoom = 250.0;
    FractalType type = MANDELBROT;
    int maxIter = MAX_ITER;
    int colorOffset = 0;

    std::vector<uint8_t> pixels(WIDTH * HEIGHT * 4, 0);
    sf::Texture tex({(unsigned)WIDTH, (unsigned)HEIGHT});
    sf::Sprite sprite(tex);

    bool needsRender = true;
    bool dragging = false;
    sf::Vector2i dragStart;
    double dragCX, dragCY;

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text hud(font);
    hud.setCharacterSize(14);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(1.5f);
    hud.setPosition({8.f, 8.f});

    sf::Text coords(font);
    coords.setCharacterSize(13);
    coords.setFillColor(sf::Color(200, 200, 200));
    coords.setOutlineColor(sf::Color::Black);
    coords.setOutlineThickness(1.f);
    coords.setPosition({8.f, (float)HEIGHT - 28.f});

    while(window.isOpen()) {
        auto mpos = sf::Mouse::getPosition(window);

        while(auto ev = window.pollEvent()) {
            if(ev->is<sf::Event::Closed>()) window.close();

            if(auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if(k->code == sf::Keyboard::Key::Escape) window.close();
                if(k->code == sf::Keyboard::Key::Num1){ type=MANDELBROT; centerX=-0.5;centerY=0;zoom=250;needsRender=true;}
                if(k->code == sf::Keyboard::Key::Num2){ type=JULIA;      centerX=0;   centerY=0;zoom=250;needsRender=true;}
                if(k->code == sf::Keyboard::Key::Num3){ type=BURNING_SHIP;centerX=-0.5;centerY=-0.5;zoom=250;needsRender=true;}
                if(k->code == sf::Keyboard::Key::R)   { centerX=-0.5;centerY=0;zoom=250;needsRender=true;}
                if(k->code == sf::Keyboard::Key::Equal){ zoom*=2.0; needsRender=true; }
                if(k->code == sf::Keyboard::Key::Hyphen){ zoom*=0.5; needsRender=true; }
                if(k->code == sf::Keyboard::Key::Up)  { maxIter=std::min(1024,maxIter*2); needsRender=true; }
                if(k->code == sf::Keyboard::Key::Down){ maxIter=std::max(64,maxIter/2);  needsRender=true; }
                // Julia param tweak
                if(k->code == sf::Keyboard::Key::J){
                    // Set julia param from mouse position
                    juliaRe = centerX + (mpos.x - WIDTH/2.0) / zoom;
                    juliaIm = centerY + (mpos.y - HEIGHT/2.0) / zoom;
                    type = JULIA; centerX=0; centerY=0; zoom=250;
                    needsRender=true;
                }
            }

            if(auto* w = ev->getIf<sf::Event::MouseWheelScrolled>()) {
                double factor = (w->delta > 0) ? 1.3 : 0.77;
                // Zoom toward mouse
                double mx = centerX + (mpos.x - WIDTH/2.0) / zoom;
                double my = centerY + (mpos.y - HEIGHT/2.0) / zoom;
                zoom *= factor;
                centerX = mx - (mpos.x - WIDTH/2.0) / zoom;
                centerY = my - (mpos.y - HEIGHT/2.0) / zoom;
                needsRender = true;
            }

            if(auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if(mb->button == sf::Mouse::Button::Left) {
                    dragging = true;
                    dragStart = mpos;
                    dragCX = centerX; dragCY = centerY;
                }
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if(mb->button == sf::Mouse::Button::Left && dragging) {
                    dragging = false;
                    needsRender = true;
                }
            }
        }

        if(dragging) {
            centerX = dragCX - (mpos.x - dragStart.x) / zoom;
            centerY = dragCY - (mpos.y - dragStart.y) / zoom;
            needsRender = true;
        }

        if(needsRender) {
            render(pixels, centerX, centerY, zoom, type, maxIter);
            tex.update(pixels.data());
            needsRender = false;
        }

        window.clear();
        window.draw(sprite);

        if(hasFont) {
            hud.setString(fractalNames[type] +
                "  |  Scroll: Zoom  |  Drag: Pan  |  1:Mandelbrot  2:Julia  3:Burning Ship  |  J: Julia from mouse  |  R: Reset  |  ↑↓: Detail");
            window.draw(hud);

            // Mouse coords in fractal space
            double fx = centerX + (mpos.x - WIDTH/2.0) / zoom;
            double fy = centerY + (mpos.y - HEIGHT/2.0) / zoom;
            char buf[128];
            snprintf(buf, sizeof(buf), "x: %.8f   y: %.8f   zoom: %.1fx   iterations: %d", fx, fy, zoom/250.0, maxIter);
            coords.setString(buf);
            window.draw(coords);
        }

        window.display();
    }
    return 0;
}

