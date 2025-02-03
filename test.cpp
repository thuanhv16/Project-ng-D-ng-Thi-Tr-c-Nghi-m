#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cctype>   // cho hàm toupper

using namespace std;
using namespace std::chrono;

// Cấu trúc lưu trữ thông tin một câu hỏi
struct Question {
    string content;             // Nội dung câu hỏi
    vector<string> options;     // Các phương án (A, B, C, D)
    char correctAnswer;         // Đáp án đúng (chữ in hoa: A, B, C, D)
};

// Hàm đọc danh sách câu hỏi từ file
vector<Question> readQuestionsFromFile(const string& filename) {
    vector<Question> questions;
    ifstream file(filename);
    
    if (!file) {
        cerr << "Không thể mở file: " << filename << endl;
        return questions;
    }
    
    while (!file.eof()) {
        Question q;
        string line;
        
        // Đọc nội dung câu hỏi
        if (!getline(file, line))
            break;
        // Nếu dòng trống thì bỏ qua (để linh hoạt với file có dòng trống)
        if (line.empty()) continue;
        q.content = line;
        
        // Đọc 4 dòng phương án: A, B, C, D
        for (int i = 0; i < 4; i++) {
            if (!getline(file, line)) break;
            q.options.push_back(line);
        }
        
        // Đọc đáp án đúng
        if (!getline(file, line)) break;
        if (!line.empty()) {
            q.correctAnswer = toupper(line[0]);
        }
        
        questions.push_back(q);
    }
    
    file.close();
    return questions;
}

// Hàm hiển thị một câu hỏi và các lựa chọn của nó
void displayQuestion(const Question& q, int index, char currentAnswer) {
    cout << "\nCâu " << index + 1 << ": " << q.content << "\n";
    for (const auto& option : q.options) {
        cout << option << "\n";
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
    vector<Question> questions = readQuestionsFromFile(filename);
    
    if (questions.empty()) {
        cout << "Không có câu hỏi trong file hoặc lỗi đọc file.\n";
        return 1;
    }
    
    // Tạo vector lưu đáp án của thí sinh, khởi tạo là ký tự khoảng trắng
    vector<char> candidateAnswers(questions.size(), ' ');
    
    // Bắt đầu tính thời gian làm bài
    auto startTime = high_resolution_clock::now();
    
    // Vòng lặp duyệt qua các câu hỏi
    for (size_t i = 0; i < questions.size(); i++) {
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
        for (size_t i = 0; i < questions.size(); i++) {
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
                    cout << "Không thay đổi đáp án cho câu hỏi " << i+1 << ".\n";
                }
            }
        }
    }
    
    // Kết thúc tính thời gian làm bài
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(endTime - startTime);
    
    // Tính toán kết quả
    int correctCount = 0;
    for (size_t i = 0; i < questions.size(); i++) {
        if (candidateAnswers[i] == questions[i].correctAnswer)
            correctCount++;
    }
    
    // Hiển thị kết quả bài thi
    cout << "\n===== KẾT QUẢ THI =====\n";
    cout << "Họ tên: " << fullName << "\n";
    cout << "Số báo danh: " << candidateID << "\n";
    cout << "Số câu đúng: " << correctCount << " / " << questions.size() << "\n";
    cout << "Thời gian làm bài: " << duration.count() << " giây\n";
    
    // Nếu muốn, hiển thị chi tiết các câu hỏi và đáp án
    cout << "\nChi tiết đáp án:\n";
    for (size_t i = 0; i < questions.size(); i++) {
        cout << "Câu " << i + 1 << ": Đáp án của bạn: " << (candidateAnswers[i]==' ' ? "Chưa trả lời" : string(1, candidateAnswers[i]))
             << " | Đáp án đúng: " << questions[i].correctAnswer << "\n";
    }
    
    cout << "\nCảm ơn bạn đã tham gia thi!\n";
    return 0;
}
