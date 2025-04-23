#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Question {
    string imagePath; // Đường dẫn đến hình ảnh câu hỏi
    vector<string> optionImages; // Đường dẫn đến hình ảnh các đáp án
    int correctAnswer; // Chỉ số của đáp án đúng (0-3)
};

// Hàm tải texture từ file hình ảnh
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        SDL_Log("Không thể tải hình ảnh %s: %s", filePath, IMG_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_Log("Không thể tạo texture từ %s: %s", filePath, SDL_GetError());
        return nullptr;
    }
    return texture;
}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Không thể khởi tạo SDL: %s", SDL_GetError());
        return 1;
    }

    // Khởi tạo SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Ai La Trieu Phu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Không thể tạo cửa sổ: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Không thể tạo renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải hình nền
    SDL_Texture* bgTexture = loadTexture(renderer, "background.png");
    if (!bgTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Danh sách câu hỏi (dùng hình ảnh thay vì text)
    vector<Question> questions = {
        {
            "question1.png", // Hình ảnh câu hỏi 1
            {"option1_A.png", "option1_B.png", "option1_C.png", "option1_D.png"}, // Hình ảnh các đáp án
            0 // Đáp án đúng: A
        },
        {
            "question2.png",
            {"option2_A.png", "option2_B.png", "option2_C.png", "option2_D.png"},
            1 // Đáp án đúng: B
        },
        {
            "question3.png",
            {"option3_A.png", "option3_B.png", "option3_C.png", "option3_D.png"},
            0 // Đáp án đúng: A
        },
        {
            "question4.png",
            {"option4_A.png", "option4_B.png", "option4_C.png", "option4_D.png"},
            2 // Đáp án đúng: C
        }
    };
    // Tải hình ảnh cho thông báo kết quả
    SDL_Texture* correctTexture = loadTexture(renderer, "correct.jpg"); // Hình ảnh "Đúng!"
    SDL_Texture* wrongTexture = loadTexture(renderer, "wrong.jpg");     // Hình ảnh "Sai!"
    SDL_Texture* nextTexture = loadTexture(renderer, "next.jpg");       // Hình ảnh "Nhấn Space để tiếp tục"
    if (!correctTexture || !wrongTexture || !nextTexture) {
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int currentQuestionIndex = 0;
    bool running = true;
    SDL_Event event;
    int selectedOption = -1;
    bool showResult = false;
    int score = 0; // Thêm điểm số

    // Tải trước các texture cho câu hỏi và đáp án
    vector<SDL_Texture*> questionTextures;
    vector<vector<SDL_Texture*>> optionTextures(questions.size(), vector<SDL_Texture*>(4));
    for (int i = 0; i < questions.size(); i++) {
        questionTextures.push_back(loadTexture(renderer, questions[i].imagePath.c_str()));
        for (int j = 0; j < 4; j++) {
            optionTextures[i][j] = loadTexture(renderer, questions[i].optionImages[j].c_str());
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
                    int optionY = 200 + i * 50;
                    if (mouseX >= 50 && mouseX <= 750 && mouseY >= optionY && mouseY <= optionY + 40) {
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
                        // Hiển thị thông báo kết thúc
                        SDL_Texture* endTexture = loadTexture(renderer, "endgame.png"); // Hình ảnh "Trò chơi kết thúc!"
                        if (!endTexture) {
                            running = false;
                        }
                        else {
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
                            SDL_Rect endRect = {50, 300, 700, 50};
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

            // Vẽ câu hỏi
            SDL_Rect questionRect = {50, 100, 700, 50};
            SDL_RenderCopy(renderer, questionTextures[currentQuestionIndex], NULL, &questionRect);

            // Vẽ đáp án
            for (int i = 0; i < 4; i++) {
                SDL_Rect optionRect = {50, 200 + i * 50, 700, 40};
                if (i == selectedOption) {
                    SDL_SetTextureColorMod(optionTextures[currentQuestionIndex][i], 255, 255, 0); // Highlight vàng
                } else {
                    SDL_SetTextureColorMod(optionTextures[currentQuestionIndex][i], 255, 255, 255); // Màu trắng
                }
                SDL_RenderCopy(renderer, optionTextures[currentQuestionIndex][i], NULL, &optionRect);
            }

            // Hiển thị kết quả
            if (showResult) {
                SDL_Rect resultRect = {50, 500, 700, 50};
                if (selectedOption == questions[currentQuestionIndex].correctAnswer) {
                    SDL_RenderCopy(renderer, correctTexture, NULL, &resultRect);
                } else {
                    SDL_RenderCopy(renderer, wrongTexture, NULL, &resultRect);
                }
                SDL_Rect nextRect = {50, 550, 700, 30};
                SDL_RenderCopy(renderer, nextTexture, NULL, &nextRect);
            }

            SDL_RenderPresent(renderer);
        }
    }

    // Giải phóng tài nguyên
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
