# Plagiarism Detector

A plagiarism detection system with a C++ backend and a React frontend. It compares two documents using Rabin-Karp rolling hash on k-word shingles and returns a similarity score.

✨ Features

Backend: C++17 server (using httplib + json.hpp).
Frontend: React app for entering documents and viewing results.
Algorithm: Rabin-Karp rolling hash on k-word shingles.
Output: Percentage similarity score.

⚙️ Setup & Running
🔹 Backend (C++ Server)

1.Open terminal in the backend folder.

2.Compile the server:

g++ -std=c++17 -O2 -pthread -D_WIN32_WINNT=0x0A00 server.cpp -o server -lws2_32

3.Run the server:

./server

By default, it runs at http://localhost:8080


🔹 Frontend (React App)

1.Navigate to the frontend folder:

cd frontend

2.Install dependencies:

npm install

3.Start the app:

npm start


Open in browser: http://localhost:3000