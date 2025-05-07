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

    TTF_Font* questionFont = TTF_OpenFont("arial.ttf", 50);
    TTF_Font* optionFont = TTF_OpenFont("arial.ttf", 40);
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
        },
        {
            "Câu hỏi 5: Sông dài nhất thế giới là sông nào?",
            {"A. Sông Amazon", "B. Sông Nile", "C. Sông Yangtze", "D. Sông Mississippi"},
            1
        },
        {
            "Câu hỏi 6: 5 × 6 − 10 bằng bao nhiêu?",
            {"A. 15", "B. 20", "C. 25", "D. 30"},
            1
        },
        {
            "Câu hỏi 7: Nguyên tố hóa học nào phổ biến nhất trong khí quyển Trái Đất?",
            {"A. Oxy", "B. Nitơ", "C. Hydro", "D. Cacbon dioxit"},
            1
        },
        {
            "Câu hỏi 8: Tác giả của \"Truyện Kiều\" là ai?",
            {"A. Nguyễn Trãi", "B. Nguyễn Du", "C. Hồ Xuân Hương", "D. Nguyễn Bỉnh Khiêm"},
            1
        },
        {
            "Câu hỏi 9: Nước nào có diện tích lớn nhất thế giới?",
            {"A. Canada", "B. Trung Quốc", "C. Nga", "D. Mỹ"},
            2
        },
        {
            "Câu hỏi 10: Loài động vật nào có vòi dài và ngà trắng?",
            {"A. Voi", "B. Tê giác", "C. Hà mã", "D. Trâu"},
            0
        },
        {
            "Câu hỏi 11: Ngày Quốc khánh Việt Nam là ngày nào?",
            {"A. 30 tháng 4", "B. 2 tháng 9", "C. 1 tháng 5", "D. 10 tháng 3"},
            1
        },
        {
            "Câu hỏi 12: Hành tinh nào được gọi là \"Hành tinh đỏ\"?",
            {"A. Sao Hỏa", "B. Sao Kim", "C. Sao Thổ", "D. Sao Mộc"},
            0
        }
    };

    SDL_Color textColor = {255, 255, 102, 255};  // Vàng nhạt
    SDL_Color greenColor = {0, 255, 0, 255};     // Xanh lá
    SDL_Color redColor = {255, 0, 0, 255};       // Đỏ
    SDL_Color highlightColor = {255, 153, 51, 255}; // Cam nhạt

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

    enum GameState { START, PLAYING, OVER };
    GameState gameState = START;

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

    // Tạo texture cho chữ "Bắt đầu"
    SDL_Texture* startTextTexture = createTextTexture(renderer, questionFont, "Bắt đầu", textColor, w, h);
    SDL_Rect startTextRect = {(WINDOW_WIDTH - w) / 2, (WINDOW_HEIGHT - h) / 2, w, h};
    if (!startTextTexture) {
        SDL_Log("Không thể tạo texture cho 'Bắt đầu'");
        TTF_CloseFont(questionFont);
        TTF_CloseFont(optionFont);
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyTexture(correctTexture);
        SDL_DestroyTexture(wrongTexture);
        SDL_DestroyTexture(nextTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo texture cho màn hình kết thúc
    SDL_Texture* overTextTexture = nullptr;
    SDL_Rect overTextRect;
    SDL_Texture* replayTexture = createTextTexture(renderer, optionFont, "Chơi lại", textColor, w, h);
    SDL_Rect replayRect = {(WINDOW_WIDTH - w) / 2 - 150, 450, w, h};
    SDL_Texture* exitTexture = createTextTexture(renderer, optionFont, "Thoát", textColor, w, h);
    SDL_Rect exitRect = {(WINDOW_WIDTH - w) / 2 + 150, 450, w, h};
    if (!replayTexture || !exitTexture) {
        SDL_Log("Không thể tạo texture cho 'Chơi lại' hoặc 'Thoát'");
        TTF_CloseFont(questionFont);
        TTF_CloseFont(optionFont);
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyTexture(correctTexture);
        SDL_DestroyTexture(wrongTexture);
        SDL_DestroyTexture(nextTexture);
        SDL_DestroyTexture(startTextTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (gameState == START) {
                    gameState = PLAYING;
                }
                else if (gameState == PLAYING && !showResult) {
                    for (int i = 0; i < 4; i++) {
                        int optionX = (WINDOW_WIDTH - optionRects[currentQuestionIndex][i].w) / 2;
                        if (mouseX >= optionX && mouseX <= optionX + optionRects[currentQuestionIndex][i].w &&
                            mouseY >= optionRects[currentQuestionIndex][i].y && mouseY <= optionRects[currentQuestionIndex][i].y + optionRects[currentQuestionIndex][i].h) {
                            selectedOption = i;
                            showResult = true;
                            if (selectedOption == questions[currentQuestionIndex].correctAnswer) {
                                score += 10;
                            }
                        }
                    }
                }
                else if (gameState == OVER) {
                    // Xử lý nhấp chuột vào "Chơi lại"
                    if (mouseX >= replayRect.x && mouseX <= replayRect.x + replayRect.w &&
                        mouseY >= replayRect.y && mouseY <= replayRect.y + replayRect.h) {
                        // Reset game
                        currentQuestionIndex = 0;
                        selectedOption = -1;
                        showResult = false;
                        score = 0;
                        gameState = PLAYING;
                    }
                    // Xử lý nhấp chuột vào "Thoát"
                    else if (mouseX >= exitRect.x && mouseX <= exitRect.x + exitRect.w &&
                             mouseY >= exitRect.y && mouseY <= exitRect.y + exitRect.h) {
                        running = false;
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN && gameState == PLAYING && showResult) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (currentQuestionIndex < (int)questions.size() - 1) {
                        currentQuestionIndex++;
                        selectedOption = -1;
                        showResult = false;
                    }
                    else {
                        // Chuyển sang màn hình kết thúc
                        string overText = "Kết thúc! Điểm của bạn: " + to_string(score);
                        if (overTextTexture) SDL_DestroyTexture(overTextTexture);
                        overTextTexture = createTextTexture(renderer, questionFont, overText.c_str(), textColor, w, h);
                        if (overTextTexture) {
                            overTextRect = {(WINDOW_WIDTH - w) / 2, 350, w, h};
                            gameState = OVER;
                        }
                    }
                }
            }
        }

        if (running) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            if (gameState == START) {
                SDL_RenderCopy(renderer, startTextTexture, NULL, &startTextRect);
            }
            else if (gameState == PLAYING) {
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
            }
            else if (gameState == OVER && overTextTexture) {
                SDL_RenderCopy(renderer, overTextTexture, NULL, &overTextRect);
                SDL_RenderCopy(renderer, replayTexture, NULL, &replayRect);
                SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
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
    SDL_DestroyTexture(startTextTexture);
    if (overTextTexture) SDL_DestroyTexture(overTextTexture);
    SDL_DestroyTexture(correctTexture);
    SDL_DestroyTexture(wrongTexture);
    SDL_DestroyTexture(nextTexture);
    SDL_DestroyTexture(replayTexture);
    SDL_DestroyTexture(exitTexture);
    TTF_CloseFont(questionFont);
    TTF_CloseFont(optionFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
