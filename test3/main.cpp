#include <SDL2/SDL.h> // W³¹czamy bibliotekê SDL.
#include <iostream>
#include <vector>
using namespace std;
#define predkosc_pocisku 10
#define rozmiar_pocisku 5

class Alien {
public:
    int x, y, width, height;
    SDL_Rect rect;
    int alive;
    Alien()
    {
        this->x = 10;
        this->y = 10;
        this->width = 30;
        this->height = 30;
        this->rect = { x,y,width,height };
        this->alive = 1;
    }

    void MoveCoordinates(int nx, int ny)
    {
        this->x = nx;
        this->y = ny;
        this->rect = { x,y,width,height };
    }
};
class Enemies {
public:
    Alien tab[40];
    int x, y;
    int direction = 1;
    Enemies()
    {
        this->x = 10;
        this->y = 10;
    }
    void IntEnemies()
    {
        int i = 0;
        for (int j = 0; j < (sizeof(tab) / sizeof(tab[0])); j++)
        {
            tab[j].MoveCoordinates(x, y);
            x = x + 40;
            i++;
            if (i == 10) {
                y += 40;
                x = 10;
                i = 0;
            }
        }
    }
    void Move()
    {
        //int direction = 1;
        int change = 0;
        int speed = 5;
        int alien_maxX = 0;
        int alien_maxY = 0;
        int alien_minX = 10000;
        int alien_minY = 10000;
        for (int i = 0; i < (sizeof(tab) / sizeof(tab[0])); i++)
        {
            if (tab[i].alive)
            {
                int IndexX = i % 10;
                int IndexY = i % 4;//Height
                if (IndexX > alien_maxX)alien_maxX = IndexX;
                if (IndexX < alien_minX)alien_minX = IndexX;
                if (IndexY > alien_maxY)alien_maxY = IndexY;
                if (IndexY < alien_minY)alien_minY = IndexY;
                //alien_alive_amount++;
            }
        }
        if (tab[alien_maxX].x + tab[alien_maxX].width >= 800)
        {
            direction = -1;
            change = 1;
        }
        if (tab[alien_minX].x <= 0)
        {
            direction = 1;
            change = 1;
        }
        for (int j = 0; j < (sizeof(tab) / sizeof(tab[0])); j++)
        {
            tab[j].x = tab[j].x + (direction * speed);
            if (change == 1)
                tab[j].y += tab[j].height;
            tab[j].MoveCoordinates(tab[j].x, tab[j].y);
        }
    }
};

class Bullet {
public:
    int x, y, width, height;
    SDL_Rect rect;
    bool active;

    Bullet() {
        width = rozmiar_pocisku;
        height = rozmiar_pocisku;
        rect = { x, y, width, height };
        active = false;
    }

    void Shoot(int playerX, int playerY) {
        x = playerX + (width / 2); // pocisk na środku naszego pleyera
        y = playerY;
        rect = { x, y, width, height };
        active = true;
    }

    void Move() {
        if (active) {
            y -= predkosc_pocisku;
            rect = { x, y, width, height };
        }
    }
};


class Player {
public:
    int x, y, width, height;
    SDL_Rect rect;
    vector<Bullet> bullets;
    int currentBullet;
    Uint32 lastWeaponFire; ///////////////////////////testy
    Player()
    {
        this->x = 10;
        this->y = 10;
        this->width = 30;
        this->height = 30;
        this->rect = { x,y,width,height };
        lastWeaponFire= SDL_GetTicks(); ///////////////////////////testy
    }
    void MoveCoordinates(int nx, int ny)
    {
        this->x = nx;
        this->y = ny;
        this->rect = { x,y,width,height };
    }
    void MoveLeft()
    {
        if (x > 0)
            x = x - 10;
        this->rect = { x,y,width,height };
    }
    void MoveRight()
    {
        if (x + width < 800) //w przyszlosci dac rozdzielczosc nie sztywna wartosc
            x = x + 10;
        this->rect = { x,y,width,height };
    }

    void Shoot() {
        Bullet newBullet;
        newBullet.Shoot(x, y);
        bullets.push_back(newBullet);
    }


};

class Engine {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    int Width, Height;
    Player p1;
    Enemies e1;
    /////////////////////
    //fpsy od
    const int FPS = 30;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    float old_time = SDL_GetTicks();
    // fps do oraz w while
    bool isRunning = true;
    //SDL_Event event;
    ///////////////////
   /// SDL_Rect Testa;/////////////////////////testy
   // int tmpTime=0;/////////////////////////testy

    Engine(int x, int y) {
        //this->Testa= { 10,300,25,25 };/////////////////////////testy
        this->Width = x;
        this->Height = y;
        p1.MoveCoordinates(this->Width / 2, this->Height - (p1.height + 5));
        e1.IntEnemies();
        // inicjalizacja biblioteki od
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            exit(1); // Wyjœcie z programu w przypadku b³êdu inicjalizacji.
        } // do
        //Tworzenie okna
        window = SDL_CreateWindow("Space Intruders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!window) {
            exit(1); // Wyjœcie z programu w przypadku b³êdu tworzenia okna.
        }
    }
    void CheckCollisions() {
        for (int i = 0; i < p1.bullets.size(); i++) {
            if (p1.bullets[i].active) {
                for (int j = 0; j < sizeof(e1.tab) / sizeof(e1.tab[0]); j++) {
                    if (e1.tab[j].alive) {
                        if (CheckCollision(p1.bullets[i], e1.tab[j])) {
                            // Kolizja między pociskiem a obcym statkiem kosmicznym.
                            p1.bullets[i].active = false;
                            e1.tab[j].alive = false;
                        }
                    }
                }
            }
        }
    }


    bool CheckCollision(const Bullet& bullet, const Alien& alien) {
        // sprawdza nachodzenie sie pocisku z pojedynczym kwadratem z tych tam kilku w ocb
        if (bullet.x < alien.x + alien.width &&
            bullet.x + bullet.width > alien.x &&
            bullet.y < alien.y + alien.height &&
            bullet.y + bullet.height > alien.y) {
            return true; // Kolizja.
        }
        return false; // Brak kolizji.
    }



    //Pętla główna gry, wyświetlanie okna

    void Run() {
        ///// wywalone do góry
        //     ||
        /////  \/
        /*//fpsy od
        const int FPS = 30;
        const int frameDelay = 1000 / FPS;
        Uint32 frameStart;
        int frameTime;
        float old_time = SDL_GetTicks();
        // fps do oraz w while
        bool isRunning = true;
        //SDL_Event event;*/
        while (isRunning) {
            frameStart = SDL_GetTicks();
            MovementHandle();
            e1.Move();
            UpdateBullets();
            Draw();
            CheckCollisions();
            //time function od
            frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime)
            {
               // tmpTime = frameDelay - frameTime;
                SDL_Delay(frameDelay - frameTime);
            } // do
           // Testa.x += 1 * (frameDelay - frameTime);/////////////////////////testy
            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
            DrawBullets();
        }

    }

    void UpdateBullets() {
        for (int i = 0; i < p1.bullets.size(); i++) {
            p1.bullets[i].Move();
        }
    }


    void DrawBullets() {
        for (int i = 0; i < p1.bullets.size(); i++) {
            if (p1.bullets[i].active) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &p1.bullets[i].rect);
            }
        }
    }


    void Draw()
    {
        //Player p1(this->Width/2,this->Height-25);
        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
        SDL_RenderFillRect(renderer, &p1.rect);
        //SDL_RenderFillRect(renderer, &Testa);/////////////////////////testy
        for (int i = 0; i < (sizeof(e1.tab) / sizeof(e1.tab[0])); i++)
        {
            if(e1.tab[i].alive==1)
            SDL_RenderFillRect(renderer, &e1.tab[i].rect);
        }

    }
void MovementHandle()
{
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LEFT])
    {
        p1.MoveLeft();
    }

    if (state[SDL_SCANCODE_RIGHT])
    {
        p1.MoveRight();
    }

    if (state[SDL_SCANCODE_SPACE] && SDL_GetTicks() > p1.lastWeaponFire + 500) ///to 500 to opóżnienie w ms
    {
        p1.Shoot();
        p1.lastWeaponFire = SDL_GetTicks();
    }
}




    // Desktruktor
    ~Engine() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }


};

int main(int argc, char* argv[]) {
    Engine game(800, 600); // Tworzenie instancji klasy Engine.
    game.Run();
    return 0; // Zakoñczenie programu.
}
