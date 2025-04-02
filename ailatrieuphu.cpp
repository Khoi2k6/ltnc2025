#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Question {
    string text;
    vector<string> options;
    int correctAnswer; // Chỉ số của đáp án đúng (0-3)
};

void renderText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color) {
    // Giả lập vẽ text (SDL2 cần SDL_ttf để vẽ text thực sự)
    SDL_Rect rect = { x, y, (int)strlen(text) * 10, 20 };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
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
    SDL_Surface* bgSurface = IMG_Load("background.png");
    if (!bgSurface) {
        SDL_Log("Không thể tải hình nền: %s", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        SDL_Log("Không thể tạo texture từ hình nền: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Danh sách câu hỏi
    vector<Question> questions = {
        {
            "Thủ đô của Việt Nam là gì?",
            {"A. Hà Nội", "B. TP. Hồ Chí Minh", "C. Đà Nẵng", "D. Huế"},
            0 // Đáp án đúng: A
        },
        {
            "1 + 1 bằng bao nhiêu?",
            {"A. 1", "B. 2", "C. 3", "D. 4"},
            1 // Đáp án đúng: B
        },
        {
            "Mặt trời mọc ở hướng nào?",
            {"A. Đông", "B. Tây", "C. Nam", "D. Bắc"},
            0 // Đáp án đúng: A
        },
        {
            "Nước nào có diện tích lớn nhất thế giới?",
            {"A. Mỹ", "B. Trung Quốc", "C. Nga", "D. Canada"},
            2 // Đáp án đúng: C
        }
    };

    int currentQuestionIndex = 0; // Chỉ số câu hỏi hiện tại
    bool running = true;
    SDL_Event event;
    int selectedOption = -1; // Đáp án được chọn (-1 là chưa chọn)
    bool showResult = false; // Hiển thị kết quả đúng/sai

    while (running) {
        // Xử lý sự kiện
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
                        running = false; // Kết thúc khi hết câu hỏi
                    }
                }
            }
        }

        // Vẽ hình nền
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Lấy câu hỏi hiện tại
        Question& currentQuestion = questions[currentQuestionIndex];

        // Vẽ câu hỏi và đáp án
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};
        renderText(renderer, currentQuestion.text.c_str(), 50, 100, white);

        for (int i = 0; i < 4; i++) {
            SDL_Color color = (i == selectedOption) ? yellow : white;
            renderText(renderer, currentQuestion.options[i].c_str(), 50, 200 + i * 50, color);
        }

        // Kiểm tra và hiển thị kết quả
        if (showResult) {
            const char* result = (selectedOption == currentQuestion.correctAnswer) ? "Đúng! (Nhấn Space để tiếp tục)" : "Sai! (Nhấn Space để tiếp tục)";
            renderText(renderer, result, 50, 500, yellow);
        }

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
