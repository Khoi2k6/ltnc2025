#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

using namespace std;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

struct Question {
    string questionText;
    vector<string> options;
    int correctAnswer;
};

SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int& w, int& h) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text, color);
    if (!surface) {
        SDL_Log("Không thể tạo surface từ văn bản: %s", TTF_GetError());
        return nullptr;
    }
    w = surface->w;
    h = surface->h;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_Log("Không thể tạo texture từ văn bản: %s", SDL_GetError());
        return nullptr;
    }
    return texture;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Không thể khởi tạo SDL: %s", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() < 0) {
        SDL_Log("Không thể khởi tạo SDL_ttf: %s", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Không thể tạo cửa sổ: %s", SDL_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Không thể tạo renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* questionFont = TTF_OpenFont("arial.ttf", 40);
    TTF_Font* optionFont = TTF_OpenFont("arial.ttf", 30);
    if (!questionFont || !optionFont) {
        SDL_Log("Không thể tải phông chữ: %s", TTF_GetError());
        if (questionFont) TTF_CloseFont(questionFont);
        if (optionFont) TTF_CloseFont(optionFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* bgSurface = IMG_Load("background.png");
    if (!bgSurface) {
        SDL_Log("Không thể tải hình nền: %s", IMG_GetError());
        TTF_CloseFont(questionFont);
        TTF_CloseFont(optionFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        SDL_Log("Không thể tạo texture hình nền: %s", SDL_GetError());
        TTF_CloseFont(questionFont);
        TTF_CloseFont(optionFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    vector<Question> questions = {
        {
            "Câu hỏi 1: Thủ đô của Việt Nam là gì?",
            {"A. Hà Nội", "B. TP. Hồ Chí Minh", "C. Đà Nẵng", "D. Huế"},
            0
        },
        {
            "Câu hỏi 2: 2 + 2 bằng bao nhiêu?",
            {"A. 2", "B. 4", "C. 6", "D. 8"},
            1
        },
        {
            "Câu hỏi 3: Màu của bầu trời là gì?",
            {"A. Xanh", "B. Đỏ", "C. Vàng", "D. Tím"},
            0
        },
        {
            "Câu hỏi 4: Hành tinh nào gần Mặt Trời nhất?",
            {"A. Sao Hỏa", "B. Sao Kim", "C. Sao Thủy", "D. Sao Mộc"},
            2
        }
    };

    // Đổi màu chữ: Vàng nhạt cho câu hỏi và đáp án
    SDL_Color textColor = {0,0,0, 255};  // Vàng nhạt (thay vì trắng)
    SDL_Color greenColor = {0, 255, 0, 255};     // Xanh lá
    SDL_Color redColor = {255, 0, 0, 255};       // Đỏ
    SDL_Color highlightColor = {255, 153, 51, 255}; // Cam nhạt cho highlight (thay vì vàng)

    int w, h;
    SDL_Texture* correctTexture = createTextTexture(renderer, optionFont, "Đúng!", greenColor, w, h);
    SDL_Rect correctRect = {(WINDOW_WIDTH - w) / 2, 600, w, h};
    SDL_Texture* wrongTexture = createTextTexture(renderer, optionFont, "Sai!", redColor, w, h);
    SDL_Rect wrongRect = {(WINDOW_WIDTH - w) / 2, 600, w, h};
    SDL_Texture* nextTexture = createTextTexture(renderer, optionFont, "Nhấn phím cách để tiếp tục", textColor, w, h);
    SDL_Rect nextRect = {(WINDOW_WIDTH - w) / 2, 650, w, h};
    if (!correctTexture || !wrongTexture || !nextTexture) {
        SDL_Log("Không thể tạo texture thông báo");
        TTF_CloseFont(questionFont);
        TTF_CloseFont(optionFont);
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int currentQuestionIndex = 0;
    bool running = true;
    SDL_Event event;
    int selectedOption = -1;
    bool showResult = false;
    int score = 0;

    vector<SDL_Texture*> questionTextures;
    vector<SDL_Rect> questionRects(questions.size());
    vector<vector<SDL_Texture*>> optionTextures(questions.size(), vector<SDL_Texture*>(4));
    vector<vector<SDL_Rect>> optionRects(questions.size(), vector<SDL_Rect>(4));
    for (int i = 0; i < questions.size(); i++) {
        questionTextures.push_back(createTextTexture(renderer, questionFont, questions[i].questionText.c_str(), textColor, w, h));
        questionRects[i] = {(WINDOW_WIDTH - w) / 2, 100, w, h};
        for (int j = 0; j < 4; j++) {
            optionTextures[i][j] = createTextTexture(renderer, optionFont, questions[i].options[j].c_str(), textColor, w, h);
            optionRects[i][j] = {(WINDOW_WIDTH - w) / 2, 250 + j * 60, w, h};
        }
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && !showResult) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                for (int i = 0; i < 4; i++) {
                    int optionY = 250 + i * 60;
                    int optionX = (WINDOW_WIDTH - optionRects[currentQuestionIndex][i].w) / 2;
                    if (mouseX >= optionX && mouseX <= optionX + optionRects[currentQuestionIndex][i].w &&
                        mouseY >= optionY && mouseY <= optionY + optionRects[currentQuestionIndex][i].h) {
                        selectedOption = i;
                        showResult = true;
                        if (selectedOption == questions[currentQuestionIndex].correctAnswer) {
                            score += 10;
                        }
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN && showResult) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (currentQuestionIndex < (int)questions.size() - 1) {
                        currentQuestionIndex++;
                        selectedOption = -1;
                        showResult = false;
                    }
                    else {
                        string endText = "Kết thúc! Điểm của bạn: " + to_string(score);
                        SDL_Texture* endTexture = createTextTexture(renderer, questionFont, endText.c_str(), textColor, w, h);
                        if (!endTexture) {
                            running = false;
                        }
                        else {
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
                            SDL_Rect endRect = {(WINDOW_WIDTH - w) / 2, 300, w, h};
                            SDL_RenderCopy(renderer, endTexture, NULL, &endRect);
                            SDL_RenderPresent(renderer);

                            bool waitingForExit = true;
                            while (waitingForExit) {
                                while (SDL_PollEvent(&event)) {
                                    if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)) {
                                        waitingForExit = false;
                                        running = false;
                                    }
                                }
                            }
                            SDL_DestroyTexture(endTexture);
                        }
                    }
                }
            }
        }

        if (running) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            SDL_RenderCopy(renderer, questionTextures[currentQuestionIndex], NULL, &questionRects[currentQuestionIndex]);

            for (int i = 0; i < 4; i++) {
                if (i == selectedOption) {
                    SDL_SetTextureColorMod(optionTextures[currentQuestionIndex][i], highlightColor.r, highlightColor.g, highlightColor.b);
                } else {
                    SDL_SetTextureColorMod(optionTextures[currentQuestionIndex][i], textColor.r, textColor.g, textColor.b);
                }
                SDL_RenderCopy(renderer, optionTextures[currentQuestionIndex][i], NULL, &optionRects[currentQuestionIndex][i]);
            }

            if (showResult) {
                if (selectedOption == questions[currentQuestionIndex].correctAnswer) {
                    SDL_RenderCopy(renderer, correctTexture, NULL, &correctRect);
                } else {
                    SDL_RenderCopy(renderer, wrongTexture, NULL, &wrongRect);
                }
                SDL_RenderCopy(renderer, nextTexture, NULL, &nextRect);
            }

            SDL_RenderPresent(renderer);
        }
    }

    for (auto texture : questionTextures) {
        SDL_DestroyTexture(texture);
    }
    for (auto& options : optionTextures) {
        for (auto texture : options) {
            SDL_DestroyTexture(texture);
        }
    }
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(correctTexture);
    SDL_DestroyTexture(wrongTexture);
    SDL_DestroyTexture(nextTexture);
    TTF_CloseFont(questionFont);
    TTF_CloseFont(optionFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
