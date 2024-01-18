#define SDL_MAIN_HANDLED
#include "main.hpp"

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;


const int radius = 5;
const int SCREEN_HEIGHT = 1000;
const int SCREEN_WIDTH = 1000;

const int NUM_BALLS = 1600;

const int NUM_THREADS = 8;
const int remainingParticles = NUM_BALLS % NUM_THREADS;


Double2 positions[NUM_BALLS];
Double2 velocities[NUM_BALLS];
const double attractionRadius = 400.0;
bool isLeftMouseButtonDown = false;
bool isRightMouseButtonDown = false;

int main(int argc, char* argv[]) {
    if (!initializeSDL()) {
        return 1;
    }

    int particlesPerRow = (int)std::sqrt(NUM_BALLS);
    int particlesPerCol = (NUM_BALLS - 1) / particlesPerRow + 1;
    double spacing = radius * 2;

    for (int i = 0; i < NUM_BALLS; i++){
        double x = (i % particlesPerRow - particlesPerRow / 2.0 + 0.5) * spacing + SCREEN_WIDTH / 2.0;
        double y = (i / particlesPerRow - particlesPerCol / 2.0 + 0.5) * spacing + SCREEN_HEIGHT / 2.0;
        Double2 position(x,y);
        positions[i] = position;
    }

    for(int i = 0; i < NUM_BALLS; i++){
        srand((unsigned) i);
        Double2 velocity(100, 100);
        velocities[i] = velocity;
    }


    SDL_Event e;

    bool quit = false;

    Uint32 lastUpdateTime = SDL_GetTicks();


    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isLeftMouseButtonDown = true;
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    isRightMouseButtonDown = true;
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isLeftMouseButtonDown = false;
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    isRightMouseButtonDown = false;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Calculate attraction or repulsion force for each particle based on mouse position
                for (int i = 0; i < NUM_BALLS; i++) {
                    Double2 particlePosition = positions[i];
                    Double2 mousePosition(mouseX, mouseY);
                    Double2 direction = mousePosition - particlePosition;
                    double distance = direction.magnitude();

                    if (isLeftMouseButtonDown) {
                        if (distance < attractionRadius) {
                            // Adjust the velocity of the particle to move towards the mouse
                            double attractionStrength = 9.8; // Adjust as needed
                            velocities[i] += direction.normalize() * attractionStrength;
                        }
                    } else if (isRightMouseButtonDown) {
                        if (distance < attractionRadius) {
                            // Adjust the velocity of the particle to move away from the mouse
                            double repulsionStrength = -9.8; // Adjust as needed
                            velocities[i] += direction.normalize() * repulsionStrength;
                        }
                    }
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

        SDL_RenderClear(renderer);
        updateFluid();

        SDL_RenderPresent(renderer);
    }
    closeSDL();
    return 0;
}

void updateFluid() {
    std::vector<SDL_Thread*> threads;
    int particlesPerThread = NUM_BALLS / NUM_THREADS;
    int remainingParticles = NUM_BALLS % NUM_THREADS;
    int startIndex = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        int numParticles = particlesPerThread + (i < remainingParticles ? 1 : 0);
        threads.push_back(SDL_CreateThread(updateFluidSection, "Thread", reinterpret_cast<void*>(startIndex)));
        startIndex += numParticles;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int threadStatus;
        SDL_WaitThread(threads[i], &threadStatus);
    }

    handleCollisions();

    for (int i = 0; i < NUM_BALLS; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawCircle(positions[i].x, positions[i].y, radius);
    }
}


int updateFluidSection(void* data) {
    double gravity = 9.8 * 50;
    double dampingFactor = 0.99;
    int startIndex = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
    int endIndex = startIndex + (NUM_BALLS / NUM_THREADS) + (startIndex < remainingParticles ? 1 : 0);

    for (int i = startIndex; i < endIndex; i++) {
        if(i >= NUM_BALLS){
            break;
        }
        velocities[i].y += (gravity * 0.0083);
        positions[i] += velocities[i] * 0.0083;

        if (positions[i].y + radius >= SCREEN_HEIGHT) {
            velocities[i].y *= -1 * dampingFactor;
            positions[i].y = SCREEN_HEIGHT - radius;
            velocities[i].x *= dampingFactor;
            if (velocities[i].y >= -0.1) velocities[i].y = 0;
        }
        if (positions[i].y - radius <= 0) {
            velocities[i].y *= -1 * dampingFactor;
            positions[i].y = radius;
            velocities[i].x *= dampingFactor;
            if (velocities[i].y >= -0.1) velocities[i].y = 0;
        }
        if (positions[i].x + radius >= SCREEN_WIDTH) {
            velocities[i].x *= -1 * dampingFactor;
            velocities[i].y *= dampingFactor;

            positions[i].x = SCREEN_WIDTH - radius;
        }
        if (positions[i].x - radius <= 0) {
            velocities[i].x *= -1 * dampingFactor;
            velocities[i].y *= dampingFactor;

            positions[i].x = radius;
        }
    }
    return 0;
}

void drawCircle(int32_t centreX, int32_t centreY, int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    // Set the render draw color (white)

    while (x >= y) {
        for (int i = centreX - x; i <= centreX + x; i++) {
            SDL_RenderDrawPoint(renderer, i, centreY + y);
            SDL_RenderDrawPoint(renderer, i, centreY - y);
        }
        for (int i = centreX - y; i <= centreX + y; i++) {
            SDL_RenderDrawPoint(renderer, i, centreY + x);
            SDL_RenderDrawPoint(renderer, i, centreY - x);
        }

        if (error <= 0) {
            y++;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            x--;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

double angleBetweenPoints(Double2 point1, Double2 point2){
    return std::atan2(point2.y - point2.y, point2.x - point1.x);
}

void handleCollisions() {
    for (int i = 0; i < NUM_BALLS; i++) {
        for (int j = i + 1; j < NUM_BALLS; j++) {
            Double2 delta = positions[j] - positions[i];
            double distance = delta.magnitude();

            if (distance < 2 * radius) {
                Double2 normal = delta.normalize();
                Double2 relativeVelocity = velocities[j] - velocities[i];
                double relativeSpeed = relativeVelocity.dot(normal);

                if (relativeSpeed < 0) {
                    // Calculate impulse (change in momentum)
                    double mass = 1; // Mass of the balls (you can set this as needed)
                    double impulse = relativeSpeed * mass;

                    // Apply the impulse to the velocities
                    velocities[i] += normal * impulse;
                    velocities[j] -= normal * impulse;

                    // Separate the balls to avoid overlap
                    double overlap = 2 * radius - distance;
                    Double2 separation = normal * (overlap * 0.5);
                    positions[i] -= separation;
                    positions[j] += separation;
                }
            }
        }
    }
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Particle Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

