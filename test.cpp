#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cctype>   // cho hàm toupper

using namespace std;
using namespace std::chrono;

// Cấu trúc lưu trữ thông tin một câu hỏi
struct Question {
    string content;             // Nội dung câu hỏi
    string options[4];          // Các phương án (A, B, C, D)
    char correctAnswer;         // Đáp án đúng (chữ in hoa: A, B, C, D)
};

// Hàm đọc danh sách câu hỏi từ file
Question* readQuestionsFromFile(const string& filename, int& questionCount) {
    ifstream file(filename);
    if (!file) {
        cerr << "Không thể mở file: " << filename << endl;
        return nullptr;
    }

    int count = 0;
    string line;
    // Đếm số câu hỏi đầu tiên (dựa trên các dòng không trống)
    while (getline(file, line)) {
        if (!line.empty()) {
            count++;
            // Chuyển tiếp các dòng với đáp án
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    // Quay lại đầu file
    file.clear();
    file.seekg(0);

    Question* questions = new Question[count];
    questionCount = count;

    for (int i = 0; i < count; i++) {
        Question& q = questions[i];
        if (!getline(file, q.content)) break;

        for (int j = 0; j < 4; j++) {
            if (!getline(file, q.options[j])) break;
        }

        string answerLine;
        if (getline(file, answerLine)) {
            q.correctAnswer = toupper(answerLine[0]);
        }
    }

    file.close();
    return questions;
}

// Hàm hiển thị một câu hỏi và các lựa chọn của nó
void displayQuestion(const Question& q, int index, char currentAnswer) {
    cout << "\nCâu " << index + 1 << ": " << q.content << "\n";
    for (int i = 0; i < 4; i++) {
        cout << q.options[i] << "\n";
    }
    if (currentAnswer == ' ') {
        cout << "Đáp án hiện tại: (chưa trả lời)\n";
    } else {
        cout << "Đáp án hiện tại: " << currentAnswer << "\n";
    }
    cout << "Nhập đáp án (A, B, C, D) hoặc S để bỏ qua: ";
}

int main() {
    // Nhập thông tin thí sinh
    string fullName;
    string candidateID;

    cout << "===== CHƯƠNG TRÌNH THI TRẮC NGHIỆM =====\n";
    cout << "Nhập họ tên: ";
    getline(cin, fullName);
    cout << "Nhập số báo danh: ";
    getline(cin, candidateID);

    // Đọc danh sách câu hỏi từ file
    string filename = "questions.txt";
    int questionCount;
    Question* questions = readQuestionsFromFile(filename, questionCount);

    if (!questions || questionCount <= 0) {
        cout << "Không có câu hỏi trong file hoặc lỗi đọc file.\n";
        delete[] questions;
        return 1;
    }

    // Tạo mảng lưu đáp án của thí sinh, khởi tạo là ký tự khoảng trắng
    char* candidateAnswers = new char[questionCount];
    for (int i = 0; i < questionCount; i++) {
        candidateAnswers[i] = ' ';
    }

    // Bắt đầu tính thời gian làm bài
    auto startTime = high_resolution_clock::now();

    // Vòng lặp duyệt qua các câu hỏi
    for (int i = 0; i < questionCount; i++) {
        char ans;
        bool validInput = false;
        do {
            displayQuestion(questions[i], i, candidateAnswers[i]);
            string input;
            getline(cin, input);
            if (input.empty()) continue;

            ans = toupper(input[0]);
            // Nếu nhập S: bỏ qua (skip)
            if (ans == 'S') {
                candidateAnswers[i] = ' ';  // Để lại chưa trả lời
                validInput = true;
            } else if (ans == 'A' || ans == 'B' || ans == 'C' || ans == 'D') {
                candidateAnswers[i] = ans;
                validInput = true;
            } else {
                cout << "Lựa chọn không hợp lệ. Vui lòng nhập lại.\n";
            }
        } while (!validInput);
    }

    // Cho phép thí sinh duyệt lại các câu hỏi chưa trả lời hoặc muốn thay đổi đáp án
    cout << "\nBạn có muốn xem lại và thay đổi đáp án các câu hỏi chưa trả lời hay không? (Y/N): ";
    string choice;
    getline(cin, choice);
    if (!choice.empty() && toupper(choice[0]) == 'Y') {
        for (int i = 0; i < questionCount; i++) {
            // Nếu chưa trả lời hoặc muốn thay đổi, thí sinh có thể nhập lại
            cout << "\nCâu hỏi số " << i + 1 << ":\n";
            displayQuestion(questions[i], i, candidateAnswers[i]);
            cout << "Nhập đáp án (A, B, C, D) hoặc nhấn Enter để giữ nguyên: ";
            string input;
            getline(cin, input);
            if (!input.empty()) {
                char newAns = toupper(input[0]);
                if (newAns == 'A' || newAns == 'B' || newAns == 'C' || newAns == 'D') {
                    candidateAnswers[i] = newAns;
                } else {
                    cout << "Không thay đổi đáp án cho câu hỏi " << i + 1 << ".\n";
                }
            }
        }
    }

    // Kết thúc tính thời gian làm bài
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(endTime - startTime);

    // Tính toán kết quả
    int correctCount = 0;
    for (int i = 0; i < questionCount; i++) {
        if (candidateAnswers[i] == questions[i].correctAnswer)
            correctCount++;
    }

    // Hiển thị kết quả bài thi
    cout << "\n===== KẾT QUẢ THI =====\n";
    cout << "Họ tên: " << fullName << "\n";
    cout << "Số báo danh: " << candidateID << "\n";
    cout << "Số câu đúng: " << correctCount << " / " << questionCount << "\n";
    cout << "Thời gian làm bài: " << duration.count() << " giây\n";

    // Nếu muốn, hiển thị chi tiết các câu hỏi và đáp án
    cout << "\nChi tiết đáp án:\n";
    for (int i = 0; i < questionCount; i++) {
        cout << "Câu " << i + 1 << ": Đáp án của bạn: " << (candidateAnswers[i] == ' ' ? "Chưa trả lời" : string(1, candidateAnswers[i]))
             << " | Đáp án đúng: " << questions[i].correctAnswer << "\n";
    }

    cout << "\nCảm ơn bạn đã tham gia thi!\n";

    // Giải phóng bộ nhớ
    delete[] questions;
    delete[] candidateAnswers;

    return 0;
}
