#include <iostream>
#include <fstream>
#include <string>
#include <chrono>


using namespace std;
using namespace std::chrono;


struct Question {
    string content;             
    string options[4];          
    char correctAnswer;       
};


char toUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}


void ManhinhQuestion(const Question& q, int index, char currentAnswer) {
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


Question* ĐocCauTrucCau(const string& filename, int& questionCount) {
    ifstream file(filename);
    if (!file) {
        cerr << "Không thể mở file: " << filename << endl;
        return nullptr;
    }

    int count = 0;
    string line;
    
    while (getline(file, line)) {
        if (!line.empty()) {
            count++;
            
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            file.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

   
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
           
            q.correctAnswer = toUpper(answerLine[0]);
        }
    }

    file.close();
    return questions;
}



int main() {
    
    string fullName;
    string candidateID;

    cout << "======= ỨNG DỤNG THI TRẮC NGHIỆM =====\n";
    cout << "Nhập họ tên: ";
    getline(cin, fullName);
    cout << "Nhập số báo danh: ";
    getline(cin, candidateID);

    
    string filename = "questions.txt";
    int questionCount;
    Question* questions = ĐocCauTrucCau(filename, questionCount);

    if (!questions || questionCount <= 0) {
        cout << "Không có câu hỏi trong file hoặc lỗi đọc file.\n";
        delete[] questions;
        return 1;
    }

    
    char* candidateAnswers = new char[questionCount];
    for (int i = 0; i < questionCount; i++) {
        candidateAnswers[i] = ' ';
    }

    
    auto startTime = high_resolution_clock::now();

    
    for (int i = 0; i < questionCount; i++) {
        char ans;
        bool validInput = false;
        do {
            ManhinhQuestion(questions[i], i, candidateAnswers[i]);
            string input;
            getline(cin, input);
            if (input.empty()) continue;

            
            ans = toUpper(input[0]);
            
            if (ans == 'S') {
                candidateAnswers[i] = ' '; 
                validInput = true;
            } else if (ans == 'A' || ans == 'B' || ans == 'C' || ans == 'D') {
                candidateAnswers[i] = ans;
                validInput = true;
            } else {
                cout << "Lựa chọn không hợp lệ. Vui lòng nhập lại.\n";
            }
        } while (!validInput);
    }

   
    cout << "\nBạn có muốn xem lại và thay đổi đáp án các câu hỏi chưa trả lời hay không? (Y/N): ";
    string choice;
    getline(cin, choice);
    if (!choice.empty() && toUpper(choice[0]) == 'Y') {
        for (int i = 0; i < questionCount; i++) {
            
            cout << "\nCâu hỏi số " << i + 1 << ":\n";
            ManhinhQuestion(questions[i], i, candidateAnswers[i]);
            cout << "Nhập đáp án (A, B, C, D) hoặc nhấn Enter để giữ nguyên: ";
            string input;
            getline(cin, input);
            if (!input.empty()) {
                char newAns = toUpper(input[0]);
                if (newAns == 'A' || newAns == 'B' || newAns == 'C' || newAns == 'D') {
                    candidateAnswers[i] = newAns;
                } else {
                    cout << "Không thay đổi đáp án cho câu hỏi " << i + 1 << ".\n";
                }
            }
        }
    }

    
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(endTime - startTime);

    
    int correctCount = 0;
    for (int i = 0; i < questionCount; i++) {
        if (candidateAnswers[i] == questions[i].correctAnswer)
            correctCount++;
    }

    
    cout << "\n===== KẾT QUẢ THI =====\n";
    cout << "Họ tên: " << fullName << "\n";
    cout << "Số báo danh: " << candidateID << "\n";
    cout << "Số câu đúng: " << correctCount << " / " << questionCount << "\n";
    cout << "Thời gian làm bài: " << duration.count() << " giây\n";

    
    cout << "\nChi tiết đáp án:\n";
    for (int i = 0; i < questionCount; i++) {
        cout << "Câu " << i + 1 << ": Đáp án của bạn: " << (candidateAnswers[i] == ' ' ? "Chưa trả lời" : string(1, candidateAnswers[i]))
             << " | Đáp án đúng: " << questions[i].correctAnswer << "\n";
    }

    cout << "\nCảm ơn bạn đã tham gia thi!\n";

    
    delete[] questions;
    delete[] candidateAnswers;

    return 0;
}
