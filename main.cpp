#define SDL_MAIN_HANDLED
#include "main.hpp"

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;

const int SCREEN_HEIGHT = 980;
const int SCREEN_WIDTH = 1024;

const int radius = 1;

const int NUM_BALLS = 100000;

Double2 positions[NUM_BALLS];
Double2 velocities[NUM_BALLS];



int main(int argc, char* argv[]) {
    if (!initializeSDL()) {
        return 1;
    }

    int particlesPerRow = (int)std::sqrt(NUM_BALLS);
    int particlesPerCol = (NUM_BALLS - 1) / particlesPerRow + 1;
    double spacing = radius * 2 + radius;

    for (int i = 0; i < NUM_BALLS; i++){
        double x = (i % particlesPerRow - particlesPerRow / 2.0 + 0.5) * spacing + SCREEN_WIDTH / 2.0;
        double y = (i / particlesPerRow - particlesPerCol / 2.0 + 0.5) * spacing + SCREEN_HEIGHT / 2.0;
        Double2 position(x,y);
        positions[i] = position;
    }

    for(int i = 0; i < NUM_BALLS; i++){
        srand((unsigned) i);
        Double2 velocity(10, 0);
        velocities[i] = velocity;
    }


    SDL_Event e;

    bool quit = false;
    double gravity = 9.8;
    double dampingFactor = 0.9;

    Uint32 lastUpdateTime = SDL_GetTicks();


    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

        SDL_RenderClear(renderer);

        for(int i = 0; i < NUM_BALLS; i++) {
            velocities[i].y += (gravity * 0.0083);
            positions[i] += velocities[i] * 0.0083;

            if(positions[i].y + radius >= SCREEN_HEIGHT){
                velocities[i].y *= -1 * dampingFactor;
                positions[i].y = SCREEN_HEIGHT - radius;
                velocities[i].x *= dampingFactor; 
                if(velocities[i].y >= -0.1) velocities[i].y = 0;
            } 
            if(positions[i].y - radius <= 0) {
                velocities[i].y *= -1 * dampingFactor;
                positions[i].y = radius;
                velocities[i].x *= dampingFactor; 
                if(velocities[i].y >= -0.1) velocities[i].y = 0;
            }
            if(positions[i].x + radius >= SCREEN_WIDTH) {
                velocities[i].x *= -1 * dampingFactor;
                positions[i].x = SCREEN_WIDTH - radius;
            }
            if(positions[i].x - radius <= 0) {
                velocities[i].x *= -1 * dampingFactor;
            }

            SDL_SetRenderDrawColor(renderer, i, i, i, 255);
            drawCircle(positions[i].x, positions[i].y, radius);
        }
        SDL_RenderPresent(renderer);
    }

    closeSDL();
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

void handleCollisions(int index) {
    for (int i = 0; i < NUM_BALLS; i++) {
        if (i == index) {
            continue;  // Skip checking collision with itself
        }

        Double2 distance = positions[i] - positions[index];
        double distanceMagnitude = distance.magnitude();

        if (distanceMagnitude < 2 * radius) {
            // Collision detected; particles are too close

            // Calculate the overlap and the separation vector
            double overlap = 2 * radius - distanceMagnitude;
            Double2 separation = distance * (overlap / distanceMagnitude);

            // Update positions to separate the colliding particles
            positions[index] -= separation * 0.5;
            positions[i] += separation * 0.5;

            // Calculate and apply a simplified elastic collision response
            Double2 relativeVelocity = velocities[i] - velocities[index];
            double relativeSpeed = dotProduct(relativeVelocity, separation);
            
            if (relativeSpeed > 0) {
                double impulse = (2.0 * relativeSpeed) / (1.0 + 1.0); // Assuming equal mass
                velocities[index] += separation * impulse;
                velocities[i] -= separation * impulse;
            }
        }
    }
}



// Helper function to calculate dot product of two vectors
double dotProduct(const Double2& a, const Double2& b) {
    return a.x * b.x + a.y * b.y;
}
